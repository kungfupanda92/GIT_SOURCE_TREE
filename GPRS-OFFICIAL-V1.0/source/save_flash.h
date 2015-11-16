#ifndef SAVE_FLASH_H_
#define SAVE_FLASH_H_


#include "main.h"
/* P U B L I C   F U N C T I O N   P R O T O T Y P E S */

void check_int_min(void);

void test_save(void);

void freeze_frame(void);

uint8_t read_freeze_frame(_RTC_time Time_server, char *return_buff);

uint8_t check_id(void);
#endif /*SAVE_FLASH_H_*/
