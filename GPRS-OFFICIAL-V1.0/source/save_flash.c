#include "save_flash.h"
#include "main.h"
#include "iap.h"
//#define DEBUG_SAVE_FLASH ;
extern PARA_PLC para_plc;
extern unsigned int half_hour;
extern char buffer_frezze[];
extern _rtc_flag rtc_flag;
extern char freeze_code[];
extern char buf_send_server[];
extern __attribute ((aligned(32))) char my_bl_data[256];
uint32_t day_sector[] = { 0x00001000, 0x00002800, 0x00004000, 0x00005800 };
/* P R I V A T E   F U N C T I O N   P R O T O T Y P E S */

uint32_t check_sector_current(void);
uint32_t check_add_current(uint8_t day_current);
void prepare_freeze_frame(void);

//-----------------------------------------------------------------------------------------------
void check_freeze_data (void){
	if (rtc_flag.bits.auto_save_data == 0)
		return;

	rtc_flag.bits.auto_save_data = 0;	//clear flag
	#ifdef CHECK_MIN
		printf("%u--%u--%u--%u--%u--\r", MONTH,DOM,HOUR,MIN,SEC);
		printf("half_hour=%u\r",half_hour);
		printf("ALMIN=%u\r",ALMIN);
	#endif
	
	freeze_frame();
}
//----------------------------------------------------------------------------------------------
uint32_t check_sector_current(void) {
	uint8_t *ptr_add;

		ptr_add = (unsigned char*)0x1000;
	if (*(ptr_add + 2) == DOM)
		 return 0x1000;
	
	
		ptr_add = (unsigned char*)0x4000;
	if (*(ptr_add + 2) == DOM)
		 return 0x4000;
	
	ptr_add = (unsigned char*)0x1000;
	if (*(ptr_add + 2) == 0xFF){
		if(HOUR !=0){
				my_bl_data[0] = MIN;
				my_bl_data[1] = HOUR;
				my_bl_data[2] = DOM;
				my_bl_data[3] = MONTH;
				my_bl_data[4] = (uint8_t)(YEAR - 2000);
			  my_bl_data[5] = 0x00;
			  iap_Write(0x1000);
		}
		 return 0x1000;
	}
	
		ptr_add = (unsigned char*)0x4000;
	if (*(ptr_add + 2) == 0xFF){
				if(HOUR !=0){
				my_bl_data[0] = MIN;
				my_bl_data[1] = HOUR;
				my_bl_data[2] = DOM;
				my_bl_data[3] = MONTH;
				my_bl_data[4] = (uint8_t)(YEAR - 2000);
				my_bl_data[5] = 0x00;
			  iap_Write(0x4000);
		}
		 return 0x4000;
	}
	
	iap_Erase_sector(1, 3);
	return 0x001000;
}
void prepare_freeze_frame() {

	unsigned int len;
	char four_bytes[5];
	uint16_t hex_server;
	char tem[10];
	unsigned int i;
	//---------------------------------------------------------------------
		/*---------Sign in meter----------*/
	sign_in();
	buf_send_server[0]=0;
	len = strlen(freeze_code);
	i = 0;
	do {
		strncpy(four_bytes, freeze_code + i, 4);
		four_bytes[4] = 0;
		hex_server = (uint16_t) strtol(four_bytes, NULL, 16);
		read_data_meter(buf_send_server, hex_server, 0,1);
		i += 4;

	} while (i < len);
	
	//send break command
	sprintf(tem, "%cB0%c%c", SOH, ETX, 0x71);
	UART0_Send(tem);
	
	//printf("buf_send_server=%s\r",buf_send_server);
	#ifdef DEBUG_SAVE_FLASH
		printf("buuf=%s",buf_send_server);
	#endif
	if(strlen(buf_send_server)>=480) 
		return;

	my_bl_data[0] = MIN;
	my_bl_data[1] = HOUR;
	my_bl_data[2] = DOM;
	my_bl_data[3] = MONTH;
	my_bl_data[4] = (uint8_t)(YEAR - 2000);
	my_bl_data[5] = 0xC3;
	
	StringToHex(my_bl_data+6,buf_send_server);
	
	#ifdef DEBUG_SAVE_FLASH
		printf("kaka=");
		for(i=0;i<179;i++){
			printf("%c",my_bl_data[i]);
		}
	#endif
}
void freeze_frame(void) {
	uint32_t current_add;
	uint8_t *ptr_add;
	
	static uint8_t mode=0;

	current_add= check_sector_current();

	prepare_freeze_frame();

	current_add += (((uint32_t) HOUR)*2 + half_hour) * 256;

	if (current_add >= 0x4000 && mode==0) {
		mode=1;
		iap_Erase_sector(4, 6);
	} else if (current_add >= 0x6F00 && mode==1) {
		mode=0;
		iap_Erase_sector(1, 3);
	}
	
	ptr_add = (unsigned char*) current_add;
	if (*(ptr_add) == 0xFF) { //dam bao dia chi can luu luon trong
		#ifdef DEBUG_SAVE_FLASH
		printf("sac=%u\r",current_add);
		#endif
		iap_Write(current_add);
	}
}

uint32_t check_add_current(uint8_t day_current) {
	uint8_t *ptr_add;

			ptr_add = (unsigned char*)0x1000;
	if (*(ptr_add + 2) == day_current)
		 return 0x1000;
	
		ptr_add = (unsigned char*)0x4000;
	if (*(ptr_add + 2) == day_current)
		 return 0x4000;
	

	return 0;
}
uint8_t read_freeze_frame(_RTC_time Time_server, char *return_buff) {
	uint32_t _half_hour;
	char string_data[4];
	uint32_t current_add,i;
	uint8_t *ptr_add;
	current_add = check_add_current(Time_server.day_of_month);
	if (current_add == 0)
		return 0;
  
	_half_hour=(Time_server.minute)? 1:0;
	 current_add += (((uint32_t) Time_server.hour)*2 + _half_hour) * 256;
	
	ptr_add = (unsigned char*) current_add;
	if (*(ptr_add+5) != 0xC3) { //dam bao co data
		return 0;
	}

	
	iap_Read(current_add, buffer_frezze, 256);
	for(i=0;i<5;i++){
		sprintf(string_data, "%02u",buffer_frezze[i]);
		strcat(return_buff, string_data);
	}
	for(i=5;i<179;i++){
		sprintf(string_data, "%02X",buffer_frezze[i]);
		strcat(return_buff, string_data);
	}
	return 1;
}
uint8_t check_id(void){
	uint8_t *ptr_add;
	uint8_t i;
	char hexstring[7];

	ptr_add = (unsigned char*)0x7000;
	
  StringToHex(hexstring,para_plc._ID);	
	
	for(i=0;i<6;i++){
		if(*(ptr_add+i) != hexstring[i]){
			iap_Erase_sector(1, 7);
			StringToHex(my_bl_data,para_plc._ID);
			iap_Write(0x7000);
			#ifdef CHECK_ID
				printf("erase ok\r");
			#endif
			return 0;
		}
	}
	return 1; //No change ID
}


