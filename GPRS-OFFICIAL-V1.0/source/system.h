#ifndef SYSTEM_H_
#define	SYSTEM_H_

#include <stdint.h>
#include "read_para.h"

#define LEN_BUFF_FREEZE		245
#define MAX_BUFFER					1000
#define MAX_BUFFER_RX				1000
#define MAX_BUFFER_TX 				1000
#define MAX_BUFFER_RX_SERVER 		1000
#define COMMANDS					300	//size buffer contain data command (9010, 9011, ...)
#define NUMBER_COMMAND_MAX			30	//Max command server can reading direct//=============================================================================
//define special ascii (control char)
#define 	LF 			0x0A

//=============================================================================
typedef enum {
	simcard_available,
	simcard_not_available,
	rssi_network_ok,
	rssi_network_weak,

	network_ready,
	using_mobifone,
	using_vinaphone,
	using_viettel
} _flag_mobile_network;
//=============================================================================
typedef union {
	int16_t val;
	struct {
		char byte0; // LSB
		char byte1; // MSB
	} byte;
	char byte_array[2];
} WORD;
//=============================================================================
typedef union {
	uint16_t val;
	struct {
		unsigned char byte0; // LSB
		unsigned char byte1; // MSB
	} byte;
	unsigned char byte_array[2];
} WORD_UNSIGNED;
//=============================================================================
typedef struct {
	char _IP1[15]; //14.168.1.194
	char _IP2[15]; //14.168.1.194
	char _PORT[10]; //8896
	char _APN[20]; //m-wap
	char _time[20];
} CONFIG_NEWWORK;
//=============================================================================
typedef struct {
	char _ID[15]; //000012345678
} PARA_PLC;
//=============================================================================
typedef struct {
	uint32_t timer_delay;
	uint32_t timer_send_socket;
	uint32_t timer_wait_response_login;
} _program_counter;
//=============================================================================
typedef union {
	int32_t val;
	struct {
		WORD dbyte0;
		WORD dbyte1;
	} word;
	WORD byte_array[2];
} DWORD;
//=============================================================================
typedef union {
	struct {
		unsigned ERROR_MODULE :1;	//indicate module GPRS is not working
		unsigned NO_SIMCARD :1;	//indicate the SIM_CART not attach the module GPRS
		unsigned CONNECT_OK :1;		//indicate connect to server ok
		unsigned SEND_LOGIN :1;		//send data to login server (A1_Command)

		unsigned SEND_SOCKET :1;//send data to hold socket of transmission (A4_Command)
		unsigned SEND_ERROR :1;		//send data & module response ERROR
		unsigned LOGIN_OK :1;		//Server response Accept the Login of module
		unsigned SOCKET_OK :1;//Server response the command SOCKET of Module OK

		unsigned SEND_READING_DIRECT :1;//indicate send data reading direct from server
		unsigned CHECKING_NEW_COMMAND :1; //use in int_uart1, indicate for check data rx
		unsigned ERROR_CACULATE_CHECKSUM :1;
		unsigned RESET_CONFIG :1;

		unsigned IP_CONNECT :1; 	//=0: Connect IP1, =1: Connect IP2
		unsigned IP1_CORRECT :1;	//=0: Ok, =1: Error
		unsigned IP2_CORRECT :1;	//=0: Ok, =1: Error
		unsigned TIMEOUT_WAIT_LOGIN :1;
	} bits;
	uint16_t data_bits;
} _system_flag;
//=============================================================================
typedef union {
	struct {
		unsigned counter_second :1;
		unsigned counter_minute :1;
		unsigned counter_hour :1;
		unsigned counter_date :1;

		unsigned alarm_second :1;
		unsigned alarm_minute :1;
		unsigned alarm_hour :1;
		unsigned auto_save_data :1;
	} bits;
	uint8_t data_bits;
} _rtc_flag;
//=============================================================================
typedef union {
	struct {
		unsigned RESPONSE_RECEIVING :1;		//indicate receiving response
		unsigned RESPONSE_OK :1;
		unsigned RESPONSE_ERROR :1;
		unsigned IDLE_RECEIVING :1;	//Indicate receiving data while idle state

		unsigned NEW_DATA :1;
		unsigned DATA_RECEIVING :1;
		unsigned DATA_RECEIVE_FINISH :1;
		unsigned CONNECT_OK :1;

		unsigned SEND_DATA_ERROR :1;
		unsigned SEND_DATA_OK :1;
		unsigned WAIT_SENDING :1;
		unsigned CHECK_PARA_RECEIVING :1;

		unsigned PARA_RECEIVE_FINISH :1;
		unsigned FLAG_14 :1;
		unsigned FLAG_15 :1;
		unsigned FLAG_16 :1;
	} bits;
	uint16_t data_bits;
} _uart1_flag;
//=============================================================================
typedef struct {
	struct {
		unsigned int counter_rx;//variable for count when receive data (increase 1 unit if receive 1 byte)
		unsigned char counter_rx_command;
		unsigned char state_uart;//Indicate the DATA RX: Data from Server, waiting Response from Module SIM, ...
		unsigned char unread;
		char head_need_rx;		//byte header for check received
		_uart1_flag flag;
	} para_rx;
	struct {
		char buf_rx_server[MAX_BUFFER_RX_SERVER];//contain data rx from server
		char buf_response_command[70]; //contain data receive of module gprs response (ex: ATE, AT%TSIM, ....)
	} buffer_rx;
} _uart1_rx_frame;
//=============================================================================
typedef struct {
	union {
		struct {
			char byte0; //LSB
			char byte1;
			char byte2;
			char byte3;
			char byte4;
			char byte5;
			char byte6;
			char byte7;
			char byte8;
			char byte9;
			char byte10;
			char byte11;
			char byte12;
			char byte13;
			char byte14;
			char byte15;
		} ID_byte;
		char ID_array[16];
	} ID_dcu;

	union {
		struct {
			char byte0;
			char byte1;
			char byte2;
			char byte3;
			char byte4;
			char byte5;
			char byte6;
			char byte7;
			char byte8;
			char byte9;
			char byte10;
			char byte11;
			char byte12;
			char byte13;
			char byte14;
			char byte15;
			char byte16;
			char byte17;
			char byte18;
			char byte19;
			char byte20;
			char byte21;
			char byte22;
			char byte23;
			char byte24;
			char byte25;
			char byte26;
			char byte27;
			char byte28;
			char byte29;
			char byte30;
			char byte31;
		} ANP_byte;
		char ANP_array[32];
	} ANP;
	union {
		struct {
			char byte0;
			char byte1;
			char byte2;
			char byte3;
			char byte4;
			char byte5;
			char byte6;
			char byte7;
			char byte8;
			char byte9;
			char byte10;
			char byte11;
			char byte12;
			char byte13;
			char byte14;
			char byte15;
			char byte16;
			char byte17;
			char byte18;
			char byte19;
			char byte20;
			char byte21;
			char byte22;
			char byte23;
			char byte24;
			char byte25;
			char byte26;
			char byte27;
			char byte28;
			char byte29;
			char byte30;
			char byte31;
		} IP1_byte;
		char IP1_array[32];
	} IP1_ADD;
	union {
		struct {
			char byte0;
			char byte1;
			char byte2;
			char byte3;
			char byte4;
			char byte5;
			char byte6;
			char byte7;
			char byte8;
			char byte9;
			char byte10;
			char byte11;
			char byte12;
			char byte13;
			char byte14;
			char byte15;
			char byte16;
			char byte17;
			char byte18;
			char byte19;
			char byte20;
			char byte21;
			char byte22;
			char byte23;
			char byte24;
			char byte25;
			char byte26;
			char byte27;
			char byte28;
			char byte29;
			char byte30;
			char byte31;
		} PORT_byte;
		char PORT_array[32];
	} PORT;
} _data_config_GPRS;
//=============================================================================

typedef union {
	struct {
		char start_code[2];
		char id_dcu[8];
		char check_count[4];
		char restart_code[2];
		char control_code[2];

		union {
			struct {
				WORD low_lenght;
				WORD high_lenght;
			} word_lenght;
			char length_array[4];
		} length_data;

		char id_meter[12];
		char meter_index[4];
		char byte_ff[50];

		union {
			struct {
				char data0[4];
				char data1[4];
				char data2[4];
				char data3[4];
				char data4[4];
				char data5[4];
			} data_byte;
			char data_array[MAX_BUFFER_RX - 88];
		} data;

		char check_sum[2];
		char end_code[2];
	} frame_struct;
	char frame_all[MAX_BUFFER_RX];
} __uart1_server_frame;
//=============================================================================
typedef struct {
	uint8_t second; /* Second value - [0,59] */
	uint8_t minute; /* Minute value - [0,59] */
	uint8_t hour; /* Hour value - [0,23] */

	uint8_t day_of_month; /* Day of the month value - [1,31] */
	uint8_t month; /* Month value - [1,12] */
	uint16_t year; /* Year value - [0,4095] */

	uint8_t day_of_week; /* Day of week value - [0,6] */
	uint16_t day_of_year; /* Day of year value - [1,365] */
} _RTC_time;
//=============================================================================
typedef union {
	struct {
		char _8EA0[4];
		char _9010[8];
		char _9011[8];
		char _9012[8];
		char _9013[8];
		char _9014[8];
		char _9110[8];
		char _A080[16];
		char _B611[4];
		char _B612[4];
		char _B613[4];
		char _B621[4];
		char _B622[4];
		char _B623[4];
		char _B630[6];
		char _B631[6];
		char _B632[6];
		char _B633[6];
		char _B640[4];
		char _B641[4];
		char _B642[4];
		char _B643[4];
		char _B650[4];
		char _B651[4];
		char _B652[4];
		char _B653[4];
		char _A010[6];
		char _B660[8];
		char _300E[104];
		char _9020[8];
		char _9120[8];
		char _B010[8];
		char _A011[6];
		char _B011[8];
		char _A012[6];
		char _B012[8];
		char _A013[6];
		char _B013[8];
		char _A014[6];
		char _B014[8];
	} data;
	char array[350]; //346bytes
} _freeze_data;
//=============================================================================

#endif

