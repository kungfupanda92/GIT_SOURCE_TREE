#ifndef __TIMER_H
#define __TIMER_H

#include <LPC213x.H>                       /* LPC213x definitions  */
#include <stdint.h>

#define CR     0x0D
#define PLOCK 0x00000400
#define MR0I (1<<0) //Interrupt When TC matches MR0
#define MR0R (1<<1)	//Reset TC when TC matches MR0
#define MR1I (1<<0) //Interrupt When TC matches MR0
#define MR1R (1<<1)	//Reset TC when TC matches MR0

#define MR0_DELAY_MS 50 //50ms Delay
#define MR1_DELAY_MS 50 //50ms Delay

#define PRESCALE 55296 //60000 PCLK clock cycles to increment TC by 1


void initTimer0(void);
void initTimer1(void);
void delay_ms(uint32_t milliseconds);

#endif	/*  */
