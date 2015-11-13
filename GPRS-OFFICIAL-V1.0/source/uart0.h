#ifndef __UART0_H
#define __UART0_H

#include <LPC213x.H>                       /* LPC213x definitions  */
#include <stdint.h>
#include "target.h"
#include "LPC213x_isr.h"
#include "system.h"

#define CR 				0x0D

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


void initUart0(uint32_t baudrate);
int UART0_Char (int ch);
void UART0_Send(char *text);

#endif	/*  */


