#ifndef __UART1_H
#define __UART1_H

#include <LPC213x.H>                       /* LPC213x definitions  */
#include <stdint.h>
#include "uart0.h"
#include "target.h"
#include "LPC213x_isr.h"


void initUart1(uint32_t baudrate);
int UART1_Char (char ch);
void UART1_Send(char *text);
#endif	/*  */


