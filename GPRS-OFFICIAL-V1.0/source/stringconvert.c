#include "stringconvert.h"
#include <string.h>						/* strlen */
#include <stdio.h>						/* printf */
#include <stdint.h>


unsigned char bcd2hex(unsigned char bcd){
	unsigned char decimal;
	decimal= (bcd & 0xF) + (((unsigned char)bcd & 0xF0) >> 4)*10;
	return decimal;
}
//-------------------------------------------------------------------------------------------------
void StringToHex(char* hexstring, char* string) {
	unsigned int ch, i, j, len;
	len = strlen(string);
	for (i = 0, j = 0; i < len; i += 2, j++) {
		ch = string[i];
		if (ch >= 0x30 && ch <= 0x39) {
			//so
			hexstring[j] = (ch - 0x30) << 4;
		} else if (ch >= 65 && ch <= 70) {
			//so
			hexstring[j] = (ch - 55) << 4;
		}


		ch = string[i + 1];
		if (ch >= 30 && ch <= 0x39) {
			//so
			hexstring[j] += (ch - 0x30);
		} else if (ch >= 41 && ch <= 0x46) {
			//so
			hexstring[j] += (ch - 55);
		}
	}
	hexstring[j] = 0; //end frame --> NULL
}
//--------------------------------------------------------------------------------------
unsigned char convert_string2hex(char *data_in) {
	unsigned char data_convert = 0;
	char *p;
	p = data_in;

	if (*p >= 0x30 && *p <= 0x39) // 0 -->9
		data_convert |= (*p - 0x30) << 4;
	else if (*p >= 65 && *p <= 70)	//A-->F
		data_convert |= (*p - 55) << 4;
	else if (*p >= 97 && *p <= 102)	//a-->f
		data_convert |= (*p - 87) << 4;
	//----------------------------
	p++;
	if (*p >= 0x30 && *p <= 0x39) // 0 -->9
		data_convert |= (*p - 0x30);
	else if (*p >= 65 && *p <= 70)	//A-->F
		data_convert |= (*p - 55);
	else if (*p >= 97 && *p <= 102)	//a-->f
		data_convert |= (*p - 87);
	//-----------------------------
	return data_convert;
}
//--------------------------------------------------------------------------------------
unsigned char check_buffer(char* data, char* buffer) {
	uint32_t i, ptr_start=0, ptr_stop=0, len;
	len = strlen(buffer);
	for (i = 0; i < len; i++) {
		if (buffer[i] == '(')
			ptr_start = i;
		else if (buffer[i] == ')') {
			ptr_stop = i;
			break;
		} else if (i >= (len - 1)) {
			//return 0;
			;
		}
	}
	if(ptr_stop<=ptr_start)
		return 0;

	strncpy(data, buffer + ptr_start + 1, ptr_stop - ptr_start - 1);
	data[ptr_stop - ptr_start - 1] = 0;
	return 1;
}
