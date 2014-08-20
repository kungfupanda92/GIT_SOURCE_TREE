#ifndef SAVE_FLASH_H_
#define SAVE_FLASH_H_


#include "main.h"
/* P U B L I C   F U N C T I O N   P R O T O T Y P E S */

void check_freeze_data(void);

void test_save(void);

void freeze_frame(void);

uint8_t read_freeze_frame(_RTC_time Time_server, char *return_buff);

uint8_t check_id(void);

uint32_t errase_day_old(void);

uint8_t compare_date(_RTC_time day1, _RTC_time day2);

uint8_t check_day_ok(unsigned long add_day);

#endif /*SAVE_FLASH_H_*/
