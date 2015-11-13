#ifndef __READ_PARA_H
#define __READ_PARA_H
//-----------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>     									 /* strtol */
#include <string.h>

#include "stringconvert.h"
#include "system.h"
#include "timer.h"
#include "uart0.h"
//-----------------------------------------------------------------------------------
#define ACK	0x06
#define SOH 0x01
#define STX 0x02
#define ETX 0x03

#define ID_METER	0
#define ID_DCU 		1
#define LENGHT		2
//------------------------------------------------------------------------------------
//68123477770108681125007777341200000100FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF80A010901F16
#define CHECKCOUNT_POS			10
#define RESTART_POS				14
#define CONTROL_CODE_POS		16
#define LENG_POS				18
#define SELECT_AREA_POS			22
#define TIME_FREEZE_DATA_POS	34

#define TIME_SYN_MODULE_POS		38

/******************************************************************************/
/* T Y P E D E F   N E W    V A R I A B L E */
/******************************************************************************/
typedef enum {
	ok_frame, buffer_overflow, error_frame, error_start_code,

	error_id_dcu, error_restart_code, error_end_frame, error_control_code,

	error_id_meter, error_check_sum, sign_in_ok, commands_ok,

	commands_error, commands_overload, not_request, not_enter_program_mode,

	not_enter_password, not_return_id, process_data_ok, command_login_ok,

	command_socket_ok, command_reading_direct
} flag_system;


/******************************************************************************/
/* P U B L I C   F U N C T I O N   P R O T O T Y P E S */
/******************************************************************************/
flag_system sign_in(void);

void set_time_from_rtc(void);

void read_para(void);	//return: IP, APN, Port, ID DCU

unsigned char caculate_checksum(char *string_data);

unsigned char process_server_reading_data_save(char *data_server);

flag_system process_server_write_mode(char *data_server);

flag_system process_server_reading_direct(char *data_server);
void read_data_meter(char *frame_tx, uint16_t code, uint16_t index,uint8_t freeze_mode);

unsigned char process_server_syntime_module(char *data_server);

unsigned char process_server_readtime_module(char *data_server);

/******************************************************************************/

#endif	/*  */
