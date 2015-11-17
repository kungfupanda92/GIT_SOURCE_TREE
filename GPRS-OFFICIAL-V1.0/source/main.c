#include "main.h"

//how are you

//*****-----*****-----*****-----*****-----
extern _system_flag system_flag;
//extern CONFIG_NEWWORK config_network;

//extern _program_counter program_counter;
//*****-----*****-----
extern _RTC_time set_time, current_time;
extern _rtc_flag rtc_flag;
extern _freeze_data free_data;
extern char buf_send_server[MAX_BUFFER_TX];
//*****-----*****-----
int main(void) {
	int temp_var;
	unsigned char test;
	unsigned char counter_reset_gprs;
	char buff_test[50];
	//*****-----*****-----*****-----*****-----
	init_VIC();
	init_PLL(); 		//Initialize CPU and Peripheral Clocks @ 60Mhz
	initTimer0(); 		//Initialize Timer0
	//
	gpio_config();
	initUart0(1200); 	//Initialize Uart0
	initUart1(9600);	//Initialize Uart1

	//--------------------------------
	init_Watchdog();	//Init & Start WDT - TimeOut ~= 310 Second
	//--------------------------------
	var_start();
	//--------------------------------
	printf("hello baby\r");
	

	ON_OFF_mudule_GPRS();
	clear_watchdog();
	read_para();		//load para from metter: ID, IP, PORT, APN
	check_id();
	prepare_config_mudule();
	clear_watchdog();
	enable_ext_wdt();
	//---------------------
	RTC_init();
	set_time_from_rtc();
	RTC_start();
	//while(1){
	test = YEAR - 2000;
	//MIN=59;
	printf("%u--", test);
	printf("%u--", MONTH);
	printf("%u--", DOM);
	printf("%u--", HOUR);
	printf("%u--", MIN);
	printf("%u--\r", SEC);
	delay_nsecond(1);
	//}
	for (temp_var = 0; temp_var < 346; temp_var++) {
		free_data.array[temp_var] = '0';
	}
	//---------------------
	buff_test[0] = 0;
	strcat(buff_test, "0001000100010001");
	for (temp_var = 0; temp_var < 16; temp_var++) {
		free_data.data._A080[temp_var] = buff_test[temp_var];
	}

	buff_test[0] = 0;
	strcat(buff_test, "2802");
	for (temp_var = 0; temp_var < 4; temp_var++) {
		free_data.data._8EA0[temp_var] = buff_test[temp_var];
	}
	//---------------------
	while (1) {
		temp_var = 0;
		counter_reset_gprs = 0;			//clear counter in the main loop
		/*----Process when can not send data to server--------------------------------------
		 ----------------------------------------------------------------------------------*/
		while (system_flag.bits.SEND_ERROR || system_flag.bits.RESET_CONFIG) {
			if (system_flag.bits.RESET_CONFIG)
				system_flag.bits.RESET_CONFIG = 0;
			read_para();	//load para from metter: ID, IP, PORT, APN
			clear_watchdog();
			prepare_config_mudule();
			clear_watchdog();
			if (system_flag.bits.CONNECT_OK) {
				//send_data_to_server();
				send_command_to_server(COMMAND_LOGIN);
				delay_nsecond(3);		//wait 3s after send command login
			} else {
				delay_nsecond(5);		//delay 5s
				temp_var++;
			}
			//*****-----
			if ((system_flag.bits.SEND_ERROR) && (temp_var >= 2)) {
				//Need reset Module GPRS
				temp_var = 0;
				ON_OFF_mudule_GPRS();
				clear_watchdog();
				//------------------------------------
				counter_reset_gprs++;//increase counter after reset module gprs
				//------------------------------------
				if (counter_reset_gprs == 3) { 	//module gprs was reset 3 time
					system_flag.bits.IP_CONNECT = 1;	//Switch to connect IP2
				} else if (counter_reset_gprs == 4) {
					system_flag.bits.IP_CONNECT = 0;	//Switch to connect IP1
				} else
					system_flag.bits.IP_CONNECT = 0;//Switch to connect IP1 - default
				//------------------------------------
				if (counter_reset_gprs >= 5)
					counter_reset_gprs = 0;				//reset counter
				//------------------------------------
				check_int_min();
				
			}
		}
		/*--------------------------------------------------------------------------------
		 ---------------------------------------------------------------------------------*/
		get_command_from_server();
		check_int_min();
		clear_watchdog();
		send_data_to_server();
	}
}
//----------------------------------------------------------------------------------------
void var_start(void) {
	system_flag.data_bits = 0x0000;
	system_flag.bits.TIMEOUT_WAIT_LOGIN = 1;
}
//----------------------------------------------------------------------------------------
void gpio_config(void) {
	//IO0DIR = 0xFFFFFFFF; //Configure all pins as outputs

	GPIO_Output(GPIO_P0, GPIO_PIN_16);//Port Output, used for ON/OFF mudule GPRS
	GPIO_Output(GPIO_P0, GPIO_PIN_0);				//Port Output, TX0
	GPIO_Output(GPIO_P0, GPIO_PIN_8);				//Port Output, TX1

	GPIO_Output(GPIO_P0, GPIO_PIN_14);//Port Output, --> control Power for MAX705
	GPIO_Output(GPIO_P0, GPIO_PIN_18);		//Port Output, --> Toggled MAX705
	
	GPIO_Output(GPIO_P0, GPIO_PIN_3); // Port output SDA
	

	GPIO_WriteBit(GPIO_P0, GPIO_PIN_14, 0); //--> Power_Off for MAX705
}
//----------------------------------------------------------------------------------------
void init_VIC(void) {
	/* initialize VIC*/
	VICIntEnClr = 0xffffffff;
	VICVectAddr = 0;
	VICIntSelect = 0;

	/* Install the default VIC handler here */
	VICDefVectAddr = (int) DefaultVICHandler;
	return;
}
//----------------------------------------------------------------------------------------
void enable_ext_wdt(void) {
	GPIO_WriteBit(GPIO_P0, GPIO_PIN_14, 1); //--> Power_On for MAX705 - EXT WDT
}
//----------------------------------------------------------------------------------------

void restart_gprs(void){
	GPIO_WriteBit(GPIO_P0, GPIO_PIN_3, 1);
	delay_ms(3000);
	GPIO_WriteBit(GPIO_P0, GPIO_PIN_3, 0);
}