#include "save_flash.h"
#include "main.h"
#include "iap.h"
//#define DEBUG_SAVE_FLASH ;
extern char buffer_frezze[];
extern _rtc_flag rtc_flag;
extern char freeze_code[];
extern char buf_send_server[];
//extern unsigned char my_bl_data[512];
extern __attribute ((aligned(32))) char my_bl_data[256];
uint32_t day_sector[] = { 0x00001000, 0x00002800, 0x00004000, 0x00005800 };
/* P R I V A T E   F U N C T I O N   P R O T O T Y P E S */

uint32_t check_sector_current(void);
uint32_t check_add_current(uint8_t day_current);
void prepare_freeze_frame(void);

void check_int_min(void) {
	if (rtc_flag.bits.counter_minute == 0)
		return;
	rtc_flag.bits.counter_minute = 0;
//	#ifdef DEBUG_SAVE_FLASH
//		printf("%u--%u--%u--%u--%u--\r", MONTH,DOM,HOUR,MIN,SEC);
//	#endif

	freeze_frame();
}

uint32_t check_sector_current(void) {
	uint8_t *ptr_add;

		ptr_add = (unsigned char*)0x1000;
	if (*(ptr_add + 2) == DOM)
		 return 0x1000;
	
	ptr_add = (unsigned char*)0x2800;
	if (*(ptr_add + 2) == DOM)
		 return 0x2800;
	
		ptr_add = (unsigned char*)0x4000;
	if (*(ptr_add + 2) == DOM)
		 return 0x4000;
	
	ptr_add = (unsigned char*)0x5800;
	if (*(ptr_add + 2) == DOM)
		 return 0x5800;
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
	ptr_add = (unsigned char*)0x2800;
	if (*(ptr_add + 2) == 0xFF){
				if(HOUR !=0){
				my_bl_data[0] = MIN;
				my_bl_data[1] = HOUR;
				my_bl_data[2] = DOM;
				my_bl_data[3] = MONTH;
				my_bl_data[4] = (uint8_t)(YEAR - 2000);
				my_bl_data[5] = 0x00;
			  iap_Write(0x2800);
		}
		 return 0x2800;
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
	ptr_add = (unsigned char*)0x5800;
	if (*(ptr_add + 2) == 0xFF){
				if(HOUR !=0){
				my_bl_data[0] = MIN;
				my_bl_data[1] = HOUR;
				my_bl_data[2] = DOM;
				my_bl_data[3] = MONTH;
				my_bl_data[4] = (uint8_t)(YEAR - 2000);
				my_bl_data[5] = 0x00;
			  iap_Write(0x5800);
		}
		 return 0x5800;
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
	
//	printf("HEAD\r");
//	for(i=0;i<179;i++){
//		printf("%02X",my_bl_data[i]);
//	}
//	printf("END\r");
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
	//static uint32_t current_add=0x00001000;
	//static uint32_t hour_=0;
	static uint8_t mode=0;
  //printf("toi day");
	current_add= check_sector_current();
	//printf("current_add1=%u\r", current_add);

	prepare_freeze_frame();
  //if(hour_++>=23) hour_=0;
	//printf("hour_=%u\r",hour_);
	current_add += (uint32_t) HOUR * 256;
	//printf("current_add2=%u\r", current_add);
	if (current_add >= 0x4000 && mode==0) {
		mode=1;
		iap_Erase_sector(4, 6);
		//current_add=0x00004000;
	} else if (current_add >= 0x6F00 && mode==1) {
		mode=0;
		iap_Erase_sector(1, 3);
		//current_add=0x00007000;
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
	
	ptr_add = (unsigned char*)0x2800;
	if (*(ptr_add + 2) == day_current)
		 return 0x2800;
	
		ptr_add = (unsigned char*)0x4000;
	if (*(ptr_add + 2) == day_current)
		 return 0x4000;
	
	ptr_add = (unsigned char*)0x5800;
	if (*(ptr_add + 2) == day_current)
		 return 0x5800;

	return 0;
}
uint8_t read_freeze_frame(_RTC_time Time_server, char *return_buff) {
	char string_data[4];
	uint32_t current_add,i;
	uint8_t *ptr_add;
	current_add = check_add_current(Time_server.day_of_month);
	if (current_add == 0)
		return 0;
  
	current_add += (uint32_t) (Time_server.hour) * 256;
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

void test_save(void) {
//	unsigned char temp[256];
//	uint32_t i, add;
////printf("0\r");

//	add = 0x1000;
//	for (i = 0; i < 256; i++)	//Copy data to be written to flash into the RAM
//			{
//		my_bl_data[i] = 0x35;
//	}

//	iap_Write(0x001000);
//	iap_Write(0x001000 + 256);
//	iap_Write(0x001000 + 512);
//	iap_Write(0x001000 + 768);
//	iap_Write(0x002000);

//	iap_Erase(0x002000);
//	iap_Write(0x002000);
//	iap_Read(0x001000, temp, sizeof(temp));
//	for (i = 0; i < 256; i++)	//Copy data to be written to flash into the RAM
//			{
//		printf("%c", temp[i]);
//	}
//	printf("ok 1\r");
//	iap_Read(0x001000 + 256, temp, sizeof(temp));
//	for (i = 0; i < 256; i++)	//Copy data to be written to flash into the RAM
//			{
//		printf("%c", temp[i]);
//	}
//	printf("ok 2\r");
//	iap_Read(0x001000 + 512, temp, sizeof(temp));
//	for (i = 0; i < 256; i++)	//Copy data to be written to flash into the RAM
//			{
//		printf("%c", temp[i]);
//	}
//	printf("ok 3\r");
//	iap_Read(0x001000 + 768, temp, sizeof(temp));
//	for (i = 0; i < 256; i++)	//Copy data to be written to flash into the RAM
//			{
//		printf("%c", temp[i]);
//	}

//	printf("ok baby\r");

}

