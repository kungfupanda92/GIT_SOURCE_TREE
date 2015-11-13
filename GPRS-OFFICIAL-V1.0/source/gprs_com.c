#include "gprs_com.h"
//---------------------------------------------------------------------------------
extern _system_flag system_flag;
extern _program_counter program_counter;
extern _uart1_rx_frame uart1_rx;
extern char buf_send_server[MAX_BUFFER_TX];
extern PARA_PLC para_plc;
extern CONFIG_NEWWORK config_network;
//-------------------------------------------------------------------------------------------------
void answer_server(void);

bool ON_OFF_mudule_GPRS(void) {
	unsigned char i, j;
	for (i = 0; i < 7; i++) {
		//*****-----*****-----
		clear_array(uart1_rx.buffer_rx.buf_response_command, 70);
		uart1_rx.para_rx.state_uart = UART_START_MODULE_GPRS;
		uart1_rx.para_rx.counter_rx = 0;
		//*****-----*****-----
		GPIO_WriteBit(GPIO_P0, GPIO_PIN_16, 1);
		delay_nsecond(3);	//delay_3s
		GPIO_WriteBit(GPIO_P0, GPIO_PIN_16, 0);
		//*****-----*****-----
		delay_nsecond(10);	//delay 10s
		//----------
		if (strstr(uart1_rx.buffer_rx.buf_response_command,
				"AT-Command Interpreter ready")) {
			delay_nsecond(10);
			return true;
		} else if (strstr(uart1_rx.buffer_rx.buf_response_command,
				"SYSSTART")) {
			delay_nsecond(10);
			return true;
		}
		j = false;
	}
	return j;
}
//-------------------------------------------------------------------------------------------------
void prepare_command_gprs(unsigned char process, char header_rx) {
	switch (process) {
	case UART_WAIT_COMMAND_RESPONSE:
		uart1_rx.para_rx.flag.data_bits = 0x00;
		clear_array(uart1_rx.buffer_rx.buf_response_command, 70);
		uart1_rx.para_rx.state_uart = UART_WAIT_COMMAND_RESPONSE;
		break;
	case UART_WAIT_DATA_GPRS:
		uart1_rx.para_rx.flag.data_bits = 0x00;
		clear_array(uart1_rx.buffer_rx.buf_rx_server, MAX_BUFFER_RX);
		uart1_rx.para_rx.state_uart = UART_WAIT_DATA_GPRS; //Go to Waiting data
		break;
	case UART_SEND_DATA:
		uart1_rx.para_rx.flag.data_bits = 0x00;
		uart1_rx.para_rx.state_uart = UART_SEND_DATA;
		break;
	case UART_CHECK_PARA:
		uart1_rx.para_rx.flag.data_bits = 0x00;
		clear_array(uart1_rx.buffer_rx.buf_response_command, 70);
		uart1_rx.para_rx.state_uart = UART_CHECK_PARA;
		uart1_rx.para_rx.head_need_rx = header_rx;
		break;
	default:
		break;
	}
}
//-------------------------------------------------------------------------------------------------
unsigned char wait_response_command_gprs(unsigned char second) {
	unsigned int i, time_wait;
	unsigned char ret;

	time_wait = second * 4;
	ret = MODULE_NO_RESPONSE;

	if (uart1_rx.para_rx.state_uart == UART_CHECK_PARA) {
		delay_nsecond(second);
		uart1_rx.para_rx.state_uart = UART_IDLE;
		return 0;
	}
	for (i = 0; i < time_wait; i++) {
		if (uart1_rx.para_rx.flag.bits.RESPONSE_OK) {
			ret = MODULE_RESPONSE_OK;
			break;
		} else if (uart1_rx.para_rx.flag.bits.RESPONSE_ERROR) {
			ret = MODULE_RESPONSE_ERROR;
			break;
		} else if (uart1_rx.para_rx.flag.bits.CONNECT_OK) {
			//system_flag.bits.CONNECT_OK = 1;
			ret = MODULE_CONNECT_OK;
			break;
		} else if (uart1_rx.para_rx.flag.bits.SEND_DATA_OK) {
			ret = MODULE_SEND_OK;
			break;
		} else
			delay_n50ms(5);
	}
	delay_nsecond(1);
	return ret;
}
//-------------------------------------------------------------------------------------------------
/*
void check_para(void) {
	char temp_data[15];

	//Check IP2
	system_flag.bits.IP2_CORRECT = 0;		//default that IP2 is correct
	strncat(temp_data, config_network._IP2, 7);
	if (strstr(temp_data, "192.168"))
		system_flag.bits.IP2_CORRECT = 1;//IP2 have been set error (because that is IP_Local Address)
	else {
		strncat(temp_data, config_network._IP2, 4);
		if (strstr(temp_data, "10."))
			system_flag.bits.IP2_CORRECT = 1;//IP2 have been set error (because that is IP_Local Address)
		else if (strstr(temp_data, "127."))
			system_flag.bits.IP2_CORRECT = 1;//IP2 have been set error (because that is IP_Local Address)
	}
}
*/
//-------------------------------------------------------------------------------------------------
void prepare_config_mudule(void) {
	unsigned char i, j;
	char ip_server[15];

	//read_para();	//load para from metter: ID, IP, PORT, APN
	for (i = 0; i < 3; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("ATE0\r");
		if (wait_response_command_gprs(3) == MODULE_RESPONSE_OK)
			break;
	}
	for (i = 0; i < 2; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("AT%%IPCLOSE=5\r");
		if (wait_response_command_gprs(3) == MODULE_RESPONSE_OK)
			break;
	}
	/*
	 for (i = 0; i < 1; i++) {
	 prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
	 printf("AT%%TSIM\r");
	 if (wait_response_command_gprs(5) == MODULE_RESPONSE_OK)
	 break;
	 }
	 */
	for (i = 0; i < 1; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, '+');
		printf("AT+COPS?\r");
		wait_response_command_gprs(2);
		if ((strstr(uart1_rx.buffer_rx.buf_response_command, "VN MOBIFONE"))
				|| (strstr(uart1_rx.buffer_rx.buf_response_command, "VMS"))) {
			strcpy(config_network._APN, "m-wap");
			//vinaphone: m3-world
			//viettel: e-connect (VIETTEL
			//EVN: e-wap
			//Vietnammobile: internet
		} else if (strstr(uart1_rx.buffer_rx.buf_response_command, "VIETTEL"))
			strcpy(config_network._APN, "e-connect");

		else if ((strstr(uart1_rx.buffer_rx.buf_response_command,
				"VN VINAPHONE"))
				|| (strstr(uart1_rx.buffer_rx.buf_response_command, "GPC")))
			strcpy(config_network._APN, "m3-world");
	}
	/*
	 for (i = 0; i < 1; i++) {
	 prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
	 printf("AT+CSQ\r");
	 delay_nsecond(2);
	 }
	 */
	for (i = 0; i < 5; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("AT%%IOMODE=1,1,0\r");
		if (wait_response_command_gprs(3) == MODULE_RESPONSE_OK)
			break;
	}
	for (i = 0; i < 5; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("AT+CGDCONT=1,\"IP\",\"%s\"\r", config_network._APN);//Send APN
		if (wait_response_command_gprs(3) == MODULE_RESPONSE_OK)
			break;
	}
	/*
	 for (i = 0; i < 1; i++) {
	 prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
	 printf("AT+CGREG?\r");
	 delay_nsecond(2);
	 }
	 */
//*****-----*****----*****
	for (i = 0; i < 3; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("AT%%ETCPIP=\"user\",\"gprs\",1\r");
		if (wait_response_command_gprs(80) == MODULE_RESPONSE_OK)
			break;
	}
	for (i = 0; i < 1; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("AT%%ETCPIP?\r");
		delay_nsecond(3);
	}

	if (system_flag.bits.IP_CONNECT == 0)
		strcpy(ip_server, config_network._IP1);	//connect to IP1
	else
		strcpy(ip_server, config_network._IP2);	//connect to IP2
	//------------------------------------
	for (i = 0; i < 1; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		printf("ATE0\r");
		if (wait_response_command_gprs(1) == MODULE_RESPONSE_OK)
			break;
	}
	//------------------------------------
	for (i = 0; i < 2; i++) {
		prepare_command_gprs(UART_WAIT_COMMAND_RESPONSE, LF);
		system_flag.bits.CONNECT_OK = 0; //clear flag before send Connect command
		printf("AT%%IPOPEN=\"TCP\",\"%s\",%s\r", ip_server,
				config_network._PORT); 	//send IP & Port for connect to server
		j = wait_response_command_gprs(20);

		if ((j == MODULE_CONNECT_OK) || (j == MODULE_RESPONSE_OK)) {
			system_flag.bits.SEND_LOGIN = 1;			//send command login
			system_flag.bits.CONNECT_OK = 1;
			break;
		} else {
			delay_nsecond(2);
			system_flag.bits.SEND_ERROR = 1;	//can not send data to server
		}
		delay_nsecond(2);
	}
}
//-------------------------------------------------------------------------------------------------
void send_command_to_server(unsigned char command) {
	unsigned char i;
	char _check_sum[2];

	buf_send_server[0] = 0;
	strcat(buf_send_server, "68");
	strncat(buf_send_server, para_plc._ID + 4, 4);
	strncat(buf_send_server, para_plc._ID + 10, 2);
	strncat(buf_send_server, para_plc._ID + 8, 2);

	strcat(buf_send_server, "0000");
	strcat(buf_send_server, "68");
	//-----*****-----*****-----
	switch (command) {
	case COMMAND_LOGIN:
		program_counter.timer_wait_response_login = 0;
		program_counter.timer_send_socket = 0;
		system_flag.bits.TIMEOUT_WAIT_LOGIN = 0;
		system_flag.bits.LOGIN_OK = 0;
		system_flag.bits.SEND_SOCKET = 0;
		strcat(buf_send_server, "A1");
		break;
	case COMMAND_SOCKET:
		system_flag.bits.SOCKET_OK = 0;
		strcat(buf_send_server, "A4");
		break;
	}
	//-----*****-----*****-----
	strcat(buf_send_server, "0300");	//length data
	strcat(buf_send_server, "111111");	//password

	sprintf(_check_sum, "%02X", caculate_checksum(buf_send_server));
	strcat(buf_send_server, _check_sum);
	strcat(buf_send_server, "16");

	prepare_command_gprs(UART_SEND_DATA, LF);
	printf("AT%%IPSEND=\"%s\"\r", buf_send_server);

	i = wait_response_command_gprs(5);

	if (i == MODULE_SEND_OK) {
		system_flag.bits.SEND_ERROR = 0;	//send Data to sever ok
	} else {
		system_flag.bits.SEND_ERROR = 1; //Alarm send error <=> Can not send data to server
		system_flag.bits.LOGIN_OK = 0;
		system_flag.bits.SOCKET_OK = 0;
	}
	uart1_rx.para_rx.state_uart = UART_IDLE;
	uart1_rx.para_rx.flag.data_bits = 0;
}
//-------------------------------------------------------------------------------------------------
void get_command_from_server(void) { //Send command to module SIM for READ DATA: AT%IPDR
	unsigned char i;
	char *ptr_data;

	if (uart1_rx.para_rx.unread > 0) {
		delay_n50ms(5);
		uart1_rx.para_rx.unread--;
		prepare_command_gprs(UART_WAIT_DATA_GPRS, LF);
		printf("AT%%IPDR\r");
		for (i = 0; i < 5; i++) {
			delay_n50ms(5);
			if (uart1_rx.para_rx.flag.bits.DATA_RECEIVE_FINISH)
				break;
		}
		//------------PRE1 for process data rx from server---------------------
		if (strstr(uart1_rx.buffer_rx.buf_rx_server, "ERROR")) {
			uart1_rx.para_rx.unread = 0;
			return;
		} else if (strstr(uart1_rx.buffer_rx.buf_rx_server, "68") == 0) {
			uart1_rx.para_rx.unread++;
			return;
		} else if ((uart1_rx.para_rx.counter_rx < 40)
				|| (uart1_rx.para_rx.flag.bits.DATA_RECEIVE_FINISH == 0)) {
			uart1_rx.para_rx.unread++;	//read again command
			return;
		}
		//----------PRE2 for process data rx from server-----------------
		ptr_data = strstr(uart1_rx.buffer_rx.buf_rx_server, ",\"68");
		if (ptr_data != NULL) {
			ptr_data += 2;
			goto CON1_GET_COMMAND_SERVER;
		}
		//-----
		ptr_data = strstr(uart1_rx.buffer_rx.buf_rx_server, ",68");
		if (ptr_data != NULL) {
			if (*(ptr_data + 3) != ',') {
				ptr_data++;
				goto CON1_GET_COMMAND_SERVER;
			} else if ((*(ptr_data + 4) == '6') && (*(ptr_data + 5) == '8')) {
				ptr_data += 4;
				goto CON1_GET_COMMAND_SERVER;
			} else
				return;
		} else
			return;
		//---------------------------------
		CON1_GET_COMMAND_SERVER:
		//-------ptr_data is pointing the frame data (68xxxx16)--------------------------
		if (check_data_rx_server(ptr_data) != ok_frame)
			return;		//frame error
		//---------frame rx from server OK------------------------------------------------
		i = convert_string2hex(ptr_data + CONTROL_CODE_POS); //get control_code
		process_data_rx_from_server(ptr_data, i);
	}
	uart1_rx.para_rx.state_uart = UART_IDLE;
	uart1_rx.para_rx.flag.bits.IDLE_RECEIVING = 0;
}
//---------------------------------------------------------------------------------------
flag_system check_data_rx_server(char *data_server) {
	char *ptr;
	WORD_UNSIGNED length_data;
	unsigned int i, j;
	unsigned char temp_byte;
	//--------------------
	ptr = data_server;
	//--------------------
	//------check start_frame---------
	if ((*ptr != '6') || (*(ptr + 1) != '8'))
		return error_start_code;
	/*-------Check overflow frame----------------------------------------------------*/

	/*-------Check byte check_count----------------------------------------------------*/

	/*-------Check re_start code-------------------------------------------------------*/
	if ((*(ptr + RESTART_POS) != '6') || (*(ptr + RESTART_POS + 1) != '8'))
		return error_restart_code;
	/*-------Check ID DCU-------------------------------------------------------*/
	for (i = 0; i < 4; i++) {
		if (*(ptr + 2 + i) != *(para_plc._ID + 4 + i))
			return error_id_dcu;
	}
	if (*(ptr + 6) != *(para_plc._ID + 10))
		return error_id_dcu;
	if (*(ptr + 7) != *(para_plc._ID + 11))
		return error_id_dcu;
	if (*(ptr + 8) != *(para_plc._ID + 8))
		return error_id_dcu;
	if (*(ptr + 9) != *(para_plc._ID + 9))
		return error_id_dcu;
	/*------------------------------------------------------------------------------*/
	/*-------Get Length of data & check----------------------------------------------*/
	length_data.byte.byte0 = convert_string2hex(ptr + LENG_POS); // byte LSB
	length_data.byte.byte1 = convert_string2hex(ptr + LENG_POS + 2); // byte MSB
	if (length_data.val > (NUMBER_COMMAND_MAX * 2 + 33))
		return commands_overload;
	/*-------check byte end_frame----------------------------------------------*/
	i = length_data.val * 2 + LENG_POS + 4 + 2; //position of end frame
	if ((*(ptr + i) != '1') || (*(ptr + i + 1) != '6'))
		return error_end_frame;
	/*-------check byte checksum of frame----------------------------------------------*/
	i = length_data.val * 2 + LENG_POS + 4; //position of checksum
	temp_byte = 0;
	for (j = 0; j < i; j += 2)
		temp_byte += convert_string2hex(ptr + j);
	if (temp_byte != convert_string2hex(ptr + i))
		return error_check_sum;
	/*----------------FRAME OK - NO ERROR----------------------------------------------*/
	return ok_frame;
}
//----------------------------------------------------------------------------------------------------------------------
void process_data_rx_from_server(char *data_server, unsigned char control_code) {
	char *ptr;
	unsigned char i;
	//--------------
	ptr = data_server;
	//--------------
	switch (control_code) {
	case COMMAND_READ_PARA_DIRECT: //Control_code = 0x11
		i = process_server_reading_direct(ptr);
		if (i == process_data_ok) {
			prepare_command_gprs(UART_SEND_DATA, LF);
			printf("AT%%IPSEND=\"%s\"\r", buf_send_server);
			i = wait_response_command_gprs(7);
			delay_nsecond(1);
			if (i == MODULE_SEND_OK) {
				system_flag.bits.SEND_ERROR = 0;	//send Data to sever ok
				//system_flag.bits.SEND_SOCKET = 0;	//No need send data socket
				//program_counter.timer_send_socket = 0;
			} else {
				system_flag.bits.SEND_ERROR = 1; //Alarm send error <=> Can not send data to server
				system_flag.bits.LOGIN_OK = 0;
				system_flag.bits.SOCKET_OK = 0;
			}
		} else if (i == error_frame)
			uart1_rx.para_rx.unread++;
		else if (i == command_login_ok)
			system_flag.bits.LOGIN_OK = 1;
		else if (i == command_socket_ok)
			system_flag.bits.SOCKET_OK = 1;
		//else
		//printf("%u\r", i);
		delay_nsecond(2);
		break;
	case COMMAND_ACCEPT_LOGIN:
		system_flag.bits.LOGIN_OK = 1;
		break;
	case COMMAND_ACCEPT_SOCKET:
		system_flag.bits.SOCKET_OK = 1;
		break;
	case COMMAND_LOAD_CURVE:
		i = process_server_write_mode(ptr);
		if (i == process_data_ok) {
			prepare_command_gprs(UART_SEND_DATA, LF);
			printf("AT%%IPSEND=\"%s\"\r", buf_send_server);
			i = wait_response_command_gprs(7);
			delay_nsecond(1);
			if (i == MODULE_SEND_OK) {
				system_flag.bits.SEND_ERROR = 0;	//send Data to sever ok
				//system_flag.bits.SEND_SOCKET = 0;	//No need send data socket
				//program_counter.timer_send_socket = 0;
			} else {
				system_flag.bits.SEND_ERROR = 1; //Alarm send error <=> Can not send data to server
				system_flag.bits.LOGIN_OK = 0;
				system_flag.bits.SOCKET_OK = 0;
			}
		} else if (i == error_frame)
			uart1_rx.para_rx.unread++;
		else if (i == command_login_ok)
			system_flag.bits.LOGIN_OK = 1;
		else if (i == command_socket_ok)
			system_flag.bits.SOCKET_OK = 1;
		//else
		//printf("%u\r", i);
		delay_nsecond(2);
		break;
	case COMMAND_READ_DATA_SAVED:
		process_server_reading_data_save(ptr);
	  answer_server();
		break;
	case COMMAND_SYN_TIME_MODULE:
		i = process_server_syntime_module(ptr);
		if (i)
			answer_server();
		else
			uart1_rx.para_rx.unread++;
		break;
	case COMMAND_READ_TIME_MODULE:
		process_server_readtime_module(ptr);
		answer_server();
		break;
	
	default:
		break;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void send_data_to_server(void) {

	if ((system_flag.bits.LOGIN_OK == 0)
			&& (system_flag.bits.TIMEOUT_WAIT_LOGIN)) { //login not yet
		//system_flag.bits.SEND_LOGIN = 0;	//clear flag
		send_command_to_server(COMMAND_LOGIN);
	} else if (system_flag.bits.SEND_SOCKET) {
		system_flag.bits.SEND_SOCKET = 0;	//clear flag
		send_command_to_server(COMMAND_SOCKET);
	}
}
//-------------------------------------------------------------------------------------------------
void clear_array(char *array, unsigned int length) {
	unsigned int i;

	for (i = 0; i < length; i++)
		array[i] = 0x00;
}
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
void answer_server(void) {
	unsigned char i;

	prepare_command_gprs(UART_SEND_DATA, LF);
	printf("AT%%IPSEND=\"%s\"\r", buf_send_server);
	i = wait_response_command_gprs(7);
	delay_nsecond(1);
	if (i == MODULE_SEND_OK) {
		system_flag.bits.SEND_ERROR = 0;	//send Data to sever ok
	} else {
		system_flag.bits.SEND_ERROR = 1; //Alarm send error <=> Can not send data to server
		system_flag.bits.LOGIN_OK = 0;
		system_flag.bits.SOCKET_OK = 0;
	}
}
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
