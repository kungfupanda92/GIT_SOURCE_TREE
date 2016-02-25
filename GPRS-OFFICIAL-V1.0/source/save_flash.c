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
extern char buff_frame_300F[];
extern __attribute ((aligned(32))) char my_bl_data[256];
uint32_t day_sector[] = { 0x00001000, 0x00002800, 0x00004000, 0x00005800 };
/* P R I V A T E   F U N C T I O N   P R O T O T Y P E S */

uint32_t check_sector_current(void);
uint32_t check_add_current(_RTC_time day_current);
void prepare_freeze_frame(void);

//-----------------------------------------------------------------------------------------------
void check_freeze_data(void) {

	if (rtc_flag.bits.have_time_300F == 1) {
		rtc_flag.bits.have_time_300F = 0;
		//save frame 300F
		/*---------Sign in meter----------*/
		sign_in();
		buff_frame_300F[0] = 0;
		read_data_meter(buff_frame_300F, 0x0F30, 0, 1);
		rtc_flag.bits.have_data_300F = 1;
	}
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

	ptr_add = (unsigned char*) 0x1000;
	if (*(ptr_add + 2) == DOM)
		return 0x1000;

	if (rtc_flag.bits.mode_save_one_hour == 1) {
		ptr_add = (unsigned char*) 0x2800;
		if (*(ptr_add + 2) == DOM)
			return 0x2800;
	}

	ptr_add = (unsigned char*) 0x4000;
	if (*(ptr_add + 2) == DOM)
		return 0x4000;

	if (rtc_flag.bits.mode_save_one_hour == 1) {
		ptr_add = (unsigned char*) 0x5800;
		if (*(ptr_add + 2) == DOM)
			return 0x5800;
	}

	errase_day_old();

	ptr_add = (unsigned char*) 0x1000;
	if (*(ptr_add + 2) == 0xFF) {
		if (HOUR != 0) {
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

	if (rtc_flag.bits.mode_save_one_hour == 1) {
		ptr_add = (unsigned char*) 0x2800;
		if (*(ptr_add + 2) == 0xFF) {
			if (HOUR != 0) {
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

	}

	ptr_add = (unsigned char*) 0x4000;
	if (*(ptr_add + 2) == 0xFF) {
		if (HOUR != 0) {
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

	if (rtc_flag.bits.mode_save_one_hour == 1) {
		ptr_add = (unsigned char*) 0x5800;
		if (*(ptr_add + 2) == 0xFF) {
			if (HOUR != 0) {
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
	}

	/*-------------------------*/
	iap_Erase_sector(1, 6);
	return 0x1000;
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
	buf_send_server[0] = 0;
	len = strlen(freeze_code);
	i = 0;
	do {
		strncpy(four_bytes, freeze_code + i, 4);
		four_bytes[4] = 0;
		hex_server = (uint16_t) strtol(four_bytes, NULL, 16);

		if (hex_server == 0x0F30 && rtc_flag.bits.have_data_300F == 1) {
			rtc_flag.bits.have_data_300F = 0;
			strcat(buf_send_server, buff_frame_300F);
		} else
			read_data_meter(buf_send_server, hex_server, 0, 1);
		i += 4;

	} while (i < len);

	//send break command
	sprintf(tem, "%cB0%c%c", SOH, ETX, 0x71);
	UART0_Send(tem);

	//printf("buf_send_server=%s\r",buf_send_server);
#ifdef DEBUG_SAVE_FLASH
	printf("buuf=%s",buf_send_server);
#endif
	if (strlen(buf_send_server) >= 480)
		return;

	my_bl_data[0] = (half_hour) ? 30 : 0;
	my_bl_data[1] = HOUR;
	my_bl_data[2] = DOM;
	my_bl_data[3] = MONTH;
	my_bl_data[4] = (uint8_t)(YEAR - 2000);
	my_bl_data[5] = 0xC3;

	StringToHex(my_bl_data + 6, buf_send_server);

#ifdef DEBUG_SAVE_FLASH
	printf("kaka=");
	for(i=0;i<179;i++) {
		printf("%c",my_bl_data[i]);
	}
#endif
}
uint32_t check_add_free(void) {
	uint32_t index;
	uint8_t *ptr;
	ptr = (uint8_t*) 0x1000;
	index = 0;
	while (index < 0x6000) {
		if (*(ptr + index) == 0xFF) {
			if (*(ptr + index + 256) != 0xFF) {
				if (index >= 0x5F00) {
					iap_Erase(0x1000);
					index = 0;
				} else
					iap_Erase(index + 0x1000 + 256);
			}
			return index + 0x1000;
		}
		index += 256;
	}
	iap_Erase_sector(1, 6);
	return 0x1000;
}

void freeze_frame(void) {
	uint32_t current_add;

	current_add = check_add_free();

	prepare_freeze_frame();

	while (iap_Write(current_add))
		;

}

uint32_t check_add_current(_RTC_time day_current) {

	uint32_t index;
	uint8_t *ptr;
	ptr = (uint8_t*) 0x1000;
	index = 0;
	while (index < 0x6000) {
		if (*(ptr + index + 1) == day_current.hour) {
			if (*(ptr + index) == day_current.minute) {
				if (*(ptr + index + 2) == day_current.day_of_month
						&& *(ptr + index + 3) == day_current.month
						&& *(ptr + index + 4) == day_current.year) {
					//
					return index + 0x1000;
				}
			}
		}
		index += 256;
	}
	return 0;
}

uint8_t read_freeze_frame(_RTC_time Time_server, char *return_buff) {
	char string_data[4];
	uint32_t current_add, i;
	uint8_t * ptr_add;

	current_add = check_add_current(Time_server);
	if (current_add == 0)
		return 0;

	ptr_add = (unsigned char*) current_add;
	if (*(ptr_add + 5) != 0xC3) { //dam bao co data
		return 0;
	}

	iap_Read(current_add, buffer_frezze, 256);
	for (i = 0; i < 5; i++) {
		sprintf(string_data, "%02u", buffer_frezze[i]);
		strcat(return_buff, string_data);
	}
	for (i = 5; i < 155; i++) {
		sprintf(string_data, "%02X", buffer_frezze[i]);
		strcat(return_buff, string_data);
	}
	return 1;
}
//====================================================================
uint8_t check_id(void) {
	uint8_t * ptr_add;
	uint8_t i;
	char hexstring[7];

	ptr_add = (unsigned char*) 0x7000;

	StringToHex(hexstring, para_plc._ID);

	for (i = 0; i < 6; i++) {
		if (*(ptr_add + i) != hexstring[i]) {
			iap_Erase_sector(1, 7);
			StringToHex(my_bl_data, para_plc._ID);
			my_bl_data[10] = 0;
			iap_Write(0x7000);
#ifdef CHECK_ID
			printf("erase ok\r");
#endif
			/*----set mode default one hour-----*/
			rtc_flag.bits.mode_save_one_hour = 0;
			return 0;
		}
	}
	if (*(ptr_add + 10) == 1)
		rtc_flag.bits.mode_save_one_hour = 1;
	else
		rtc_flag.bits.mode_save_one_hour = 0;
	return 1; //No change ID
}

//====================================================================
uint8_t check_day_ok(unsigned long add_day) {
	unsigned char *ptr;
	ptr = (unsigned char*) add_day;
	if (*(ptr + 2) > 31) //day of month
		return 0;
	if (*(ptr + 3) > 12) //month
		return 0;
	if (*(ptr + 4) > 100) //year
		return 0;
	return 1;
}
uint8_t compare_date(_RTC_time day1, _RTC_time day2) {
	if (day1.year > day2.year)
		return 1;
	else if (day1.year < day2.year)
		return 2;
	else {
		if (day1.month > day2.month)
			return 1;
		else if (day1.month < day2.month)
			return 2;
		else {
			if (day1.day_of_month > day2.day_of_month)
				return 1;
			else if (day1.day_of_month < day2.day_of_month)
				return 2;
			else
				return 0;
		}

	}
}
uint32_t errase_day_old(void) {
	uint8_t *ptr_day1;
	uint8_t *ptr_day2;
	_RTC_time day1, day2, current;

	if (check_day_ok(0x1000) == 0) {
		iap_Erase_sector(1, 3);
		return 0x1000;
	}
	if (check_day_ok(0x4000) == 0) {
		iap_Erase_sector(4, 6);
		return 0x4000;
	}

	ptr_day1 = (unsigned char*) 0x1000;
	ptr_day2 = (unsigned char*) 0x4000;

	day1.day_of_month = *(ptr_day1 + 2);
	day1.month = *(ptr_day1 + 3);
	day1.year = *(ptr_day1 + 4);

	day2.day_of_month = *(ptr_day2 + 2);
	day2.month = *(ptr_day2 + 3);
	day2.year = *(ptr_day2 + 4);

	current.day_of_month = DOM;
	current.month = MONTH;
	current.year = (uint8_t)(YEAR - 2000);

//printf("Day1:%u--%u--%u\r", day1.day_of_month,day1.month,day1.year);
//printf("Day2:%u--%u--%u\r", day2.day_of_month,day2.month,day2.year);
//printf("current:%u--%u--%u\r", current.day_of_month,current.month,current.year);
//printf("whe:%u\r",compare_date(day1,day2));

	if (compare_date(day1, day2) == 1) {
		if (compare_date(current, day1) == 1) {
			//printf("xoa 4,6\r");
			iap_Erase_sector(4, 6);
			return 0x4000;
		} else {
			//printf("xoa het\r");
			iap_Erase_sector(1, 6);
			return 0x1000;
		}
	} else {
		if (compare_date(current, day2) == 1) {
			//printf("xoa 1,3\r");	
			iap_Erase_sector(1, 3);
			return 0x1000;
		} else {
			//printf("xoa het\r");
			iap_Erase_sector(1, 6);
			return 0x1000;
		}
	}
}
