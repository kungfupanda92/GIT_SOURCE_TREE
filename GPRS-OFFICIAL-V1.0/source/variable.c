#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "read_para.h"
#include "system.h"

char idle_buf_position, idle_buf_counter;
_uart1_rx_frame uart1_rx;
_system_flag system_flag;

char buffer_PLC[MAX_BUFFER];
char buf_send_server[MAX_BUFFER_TX];
char buff_frame_300F[100];
char commands[COMMANDS];
char buffer_frezze[200];
//--------------------------------------------------
char buff_contain_data_read_metter[200];
char buff_contain_data_add_send_server[200];
char code_server[5];
char code_meter[5];
char out_data[15];
char data_PLC[50];
char temp_data1[50];
char buff_time[10];
//--------------------------------------------------
PARA_PLC para_plc;
CONFIG_NEWWORK config_network;
//===================================================================================================
unsigned char check_frame = false;
unsigned char start_frame = NULL;
unsigned char stop_frame = NULL;
unsigned char stop_frame_immediately = false;

_program_counter program_counter;

//-------------------------------------------------------------------------------------------------
//variable for RTC
unsigned int half_hour;
//_RTC_time set_time, current_time;
_RTC_time time_server;
uint16_t time_auto_read;
_rtc_flag rtc_flag;
__attribute ((aligned(32))) char my_bl_data[256];
//-------------------------------------------------------------------------------------------------
//_freeze_data free_data;
char freeze_code[] =
		"A08E10901190129013901490109180A011B612B613B621B622B623B630B631B632B633B640B641B642B643B650B651B652B653B610A060B60F302090209110B011A011B012A012B013A013B014A014B0C000";

uint16_t table_control_code[LEN_BUFF_FREEZE][4] = { 
	{0x1090, 0x9010, 0, 8},
	{0x1190, 0x9011, 0, 8},
	{0x1290, 0x9012, 0, 8},
	{0x1390, 0x9013, 0, 8},
	{0x1490, 0x9014, 0, 8},
	{0x1091, 0x9030, 0, 8},
	{0x1191, 0x9031, 0, 8},
	{0x1291, 0x9032, 0, 8},
	{0x1391, 0x9033, 0, 8},
	{0x1491, 0x9034, 0, 8},
	{0x2091, 0x9040, 0, 8},
	{0x2191, 0x9041, 0, 8},
	{0x2291, 0x9042, 0, 8},
	{0x2391, 0x9043, 0, 8},
	{0x2491, 0x9044, 0, 8},
	{0x11B6, 0x2023, 1, 4},
	{0x12B6, 0x2024, 1, 4},
	{0x13B6, 0x2025, 1, 4},
	{0x21B6, 0x2020, 0, 4},
	{0x22B6, 0x2021, 0, 4},
	{0x23B6, 0x2022, 0, 4},
	{0x30B6, 0x2029, 5, 6},
	{0x31B6, 0x2026, 5, 6},
	{0x32B6, 0x2027, 5, 6},
	{0x33B6, 0x2028, 5, 6},
	{0x40B6, 0x202D, 6, 4},
	{0x41B6, 0x202A, 6, 4},
	{0x42B6, 0x202B, 6, 4},
	{0x43B6, 0x202C, 6, 4},
	{0x10A0, 0x3000, 3, 6},
	{0x11A0, 0x3001, 3, 6},
	{0x12A0, 0x3002, 3, 6},
	{0x13A0, 0x3003, 3, 6},
	{0x14A0, 0x3004, 3, 6},
	{0x10A1, 0x3020, 0, 6},
	{0x11A1, 0x3021, 0, 6},
	{0x12A1, 0x3022, 0, 6},
	{0x13A1, 0x3023, 0, 6},
	{0x14A1, 0x3024, 0, 6},
	{0x20A1, 0x3030, 0, 6},
	{0x21A1, 0x3031, 0, 6},
	{0x22A1, 0x3032, 0, 6},
	{0x23A1, 0x3033, 0, 6},
	{0x24A1, 0x3034, 0, 6},
	{0x50B6, 0x2033, 0, 4},
	{0x51B6, 0x2030, 0, 4},
	{0x52B6, 0x2031, 0, 4},
	{0x53B6, 0x2032, 0, 4},
	{0x00D2, 0xD200, 0, 4},
	{0xA08E, 0xC714, 0, 4},
	{0x3080, 0xC001, 0, 12},
	{0x32C0, 0xC100, 0, 12},
	{0x05C1, 0xC105, 0, 8},
	{0x08C1, 0xC108, 0, 8},
	{0x06C1, 0xC106, 0, 4},
	{0x07C1, 0xC107, 0, 32},
	{0x00A0, 0xA000, 0, 4},
	{0x01A0, 0xA001, 0, 4},
	{0x02A0, 0xA002, 0, 4},
	{0x06A0, 0xA006, 0, 4},
	{0x11C1, 0xC111, 0, 4},
	{0x00C6, 0xC600, 0, 2},
	{0x01C6, 0xC601, 0, 2},
	{0x02C6, 0xC602, 0, 2},
	{0x10C6, 0xC610, 0, 64},
	{0x12C6, 0xC612, 0, 64},
	{0x10C7, 0xC710, 0, 4},
	{0x11C7, 0xC711, 0, 8},
	{0x12C7, 0xC712, 2, 24},
	{0x00C2, 0xC200, 0, 2},
	{0x01C2, 0xC201, 0, 2},
	{0x02C2, 0xC202, 0, 2},
	{0x03C2, 0xC203, 0, 2},
	{0x04C2, 0xC204, 0, 2},
	{0x00E1, 0xE100, 0, 2},
	{0x01C2, 0xC201, 0, 2},
	{0x10C2, 0xC210, 0, 64},
	{0x11C2, 0xC211, 0, 64},
	{0x12C2, 0xC212, 0, 64},
	{0x13C2, 0xC213, 0, 64},
	{0x14C2, 0xC214, 0, 64},
	{0x15C2, 0xC215, 0, 64},
	{0x16C2, 0xC216, 0, 64},
	{0x17C2, 0xC217, 0, 64},
	{0x01C2, 0xC201, 0, 2},
	{0x18C2, 0xC218, 0, 64},
	{0x19C2, 0xC219, 0, 64},
	{0x1AC2, 0xC21A, 0, 64},
	{0x1BC2, 0xC21B, 0, 64},
	{0x1CC2, 0xC21C, 0, 64},
	{0x1EC2, 0xC21E, 0, 64},
	{0x1FC2, 0xC21F, 0, 64},
	{0x02C2, 0xC202, 0, 2},
	{0x40C2, 0xC240, 0, 36},
	{0x41C2, 0xC241, 0, 36},
	{0x03C2, 0xC203, 0, 2},
	{0x48C2, 0xC248, 0, 42},
	{0x49C2, 0xC249, 0, 42},
	{0x4AC2, 0xC24A, 0, 42},
	{0x4BC2, 0xC24B, 0, 42},
	{0x04C2, 0xC204, 0, 2},
	{0x50C2, 0xC250, 0, 48},
	{0x51C2, 0xC251, 0, 48},
	{0x54C2, 0xC254, 0, 48},
	{0x55C2, 0xC255, 0, 48},
	{0x58C2, 0xC258, 0, 48},
	{0x59C2, 0xC259, 0, 48},
	{0x04C2, 0xC204, 0, 2},
	{0x5CC2, 0xC25C, 0, 48},
	{0x5DC2, 0xC25D, 0, 48},
	{0x60C2, 0xC260, 0, 48},
	{0x61C2, 0xC261, 0, 48},
	{0x64C2, 0xC264, 0, 48},
	{0x65C2, 0xC265, 0, 48},
	{0x70A0, 0xA070, 0, 58},
	{0x71A0, 0xA071, 0, 58},
	{0x80A0, 0xA080, 0, 16},
	{0x90A0, 0xA090, 0, 46},
	{0x00E3, 0xE300, 0, 4},
	{0x05E3, 0xE305, 0, 2},
	{0x06E3, 0xE306, 0, 100},
	{0x01E3, 0xE301, 0, 4},
	{0x07E3, 0xE307, 0, 2},
	{0x08E3, 0xE308, 0, 100},
	{0x02E3, 0xE302, 0, 4},
	{0x03E3, 0xE303, 0, 2},
	{0x04E3, 0xE304, 0, 100},
	{0x0004, 0x0400, 0, 50},
	{0x2004, 0x0420, 0, 50},
	{0x3004, 0x0430, 0, 50},
	{0xA005, 0x05A0, 0, 12},
	{0x0006, 0x0600, 0, 50},
	{0x2006, 0x0620, 0, 50},
	{0x3006, 0x0630, 0, 50},
	{0xA007, 0x07A0, 0, 12},
	{0x0008, 0x0800, 0, 50},
	{0x2008, 0x0820, 0, 50},
	{0x3008, 0x0830, 0, 50},
	{0xA009, 0x09A0, 0, 12},
	{0x000A, 0x0A00, 0, 50},
	{0x200A, 0x0A20, 0, 50},
	{0x300A, 0x0A30, 0, 50},
	{0xA00B, 0x0BA0, 0, 12},
	{0x000C, 0x0C00, 0, 50},
	{0x200C, 0x0C20, 0, 50},
	{0x300C, 0x0C30, 0, 50},
	{0xA00D, 0x0DA0, 0, 50},
	{0x000E, 0x0E00, 0, 50},
	{0x200E, 0x0E20, 0, 50},
	{0x300E, 0x0E30, 0, 50},
	{0xA00F, 0x0FA0, 0, 12},
	{0x0010, 0x1000, 0, 50},
	{0x2010, 0x1020, 0, 50},
	{0x3010, 0x1030, 0, 50},
	{0xA011, 0x11A0, 0, 12},
	{0x0012, 0x1200, 0, 12},
	{0x2012, 0x1220, 0, 50},
	{0x3012, 0x1230, 0, 50},
	{0xA013, 0x13A0, 0, 12},
	{0x0014, 0x1400, 0, 50},
	{0x2014, 0x1420, 0, 50},
	{0x3014, 0x1430, 0, 50},
	{0xA015, 0x15A0, 0, 12},
	{0x0016, 0x1600, 0, 50},
	{0x2016, 0x1620, 0, 50},
	{0x3016, 0x1630, 0, 50},
	{0xA017, 0x17A0, 0, 12},
	{0x0018, 0x1800, 0, 50},
	{0x2018, 0x1820, 0, 50},
	{0x3018, 0x1830, 0, 50},
	{0xA019, 0x19A0, 0, 12},
	{0x001A, 0x1A00, 0, 50},
	{0x201A, 0x1A20, 0, 50},
	{0x301A, 0x1A30, 0, 50},
	{0xA01B, 0x1BA0, 0, 12},
	{0x0024, 0x2400, 0, 80},
	{0x2024, 0x2420, 0, 80},
	{0x3024, 0x2430, 0, 80},
	{0xA024, 0x24A0, 0, 12},
	{0x0025, 0x2500, 0, 80},
	{0x2025, 0x2520, 0, 80},
	{0x3025, 0x2530, 0, 80},
	{0xA025, 0x25A0, 0, 12},
	{0x0026, 0x2600, 0, 80},
	{0x2026, 0x2620, 0, 80},
	{0x3026, 0x2630, 0, 80},
	{0xA026, 0x26A0, 0, 12},
	{0x0027, 0x2700, 0, 80},
	{0x2027, 0x2720, 0, 80},
	{0x3027, 0x2730, 0, 80},
	{0xA027, 0x27A0, 0, 12},
	{0x0028, 0x2800, 0, 80},
	{0x2028, 0x2820, 0, 80},
	{0x3028, 0x2830, 0, 80},
	{0xA028, 0x28A0, 0, 12},
	{0x0029, 0x2900, 0, 80},
	{0x2029, 0x2920, 0, 80},
	{0x3029, 0x2930, 0, 80},
	{0xA029, 0x29A0, 0, 12},
	{0x002A, 0x2A00, 0, 80},
	{0x202A, 0x2A20, 0, 80},
	{0x302A, 0x2A30, 0, 80},
	{0xA02A, 0x2AA0, 0, 12},
	{0x002B, 0x2B00, 0, 80},
	{0x202B, 0x2B20, 0, 80},
	{0x302B, 0x2B30, 0, 80},
	{0xA02B, 0x2BA0, 0, 12},
	{0x002C, 0x2C00, 0, 80},
	{0x202C, 0x2C20, 0, 80},
	{0x302C, 0x2C30, 0, 80},
	{0xA02C, 0x2CA0, 0, 12},
	{0x002D, 0x2D00, 0, 80},
	{0x202D, 0x2D20, 0, 80},
	{0x302D, 0x2D30, 0, 80},
	{0xA02D, 0x2DA0, 0, 12},
	{0x002E, 0x2E00, 0, 80},
	{0x202E, 0x2E20, 0, 80},
	{0x302E, 0x2E30, 0, 80},
	{0xA02E, 0x2EA0, 0, 12},
	{0x002F, 0x2F00, 0, 80},
	{0x202F, 0x2F20, 0, 80},
	{0x302F, 0x2F30, 0, 80},
	{0xA02F, 0x2FA0, 0, 12},
	{0x2090, 0x9020, 0, 8},
	{0x2190, 0x9021, 0, 8},
	{0x2290, 0x9022, 0, 8},
	{0x2390, 0x9023, 0, 8},
	{0x2490, 0x9024, 0, 8},
	{0x20A0, 0x3010, 3, 6},
	{0x21A0, 0x3011, 3, 6},
	{0x22A0, 0x3012, 3, 6},
	{0x23A0, 0x3013, 3, 6},
	{0x24A0, 0x3014, 3, 6},
	{0x0F30, 0x300F, 2, 52},
	{0x11C7, 0xC711, 0, 8},
	{0x03D3, 0xD303, 0, 4},
	{0x00D3, 0xD300, 0, 104},
	{0x13D3, 0xD313, 0, 8},
	{0x60B6, 0x203E, 0, 8},
	{0x10B0, 0x3000, 4, 8},
	{0x11B0, 0x3001, 4, 8},
	{0x12B0, 0x3002, 4, 8},
	{0x13B0, 0x3003, 4, 8},
	{0x14B0, 0x3004, 4, 8},
	{0x0E30, 0x300E, 2, 104},
	{0xC000, 0xC000, 2, 4}
};

