#include "timer.h"
#include <stdint.h>
#include "LPC213x_isr.h"

void initTimer0(void) {
	/*Assuming that PLL0 has been setup with CCLK = 60Mhz and PCLK also = 60Mhz.*/
	//----------Configure Timer0-------------
	T0CTCR = 0x0;

	T0PR = PRESCALE - 1; //60000 clock cycles @60Mhz = 1 mS

	T0MR0 = MR0_DELAY_MS - 1; // 50Ms (Value in Decimal!) Zero Indexed Count - hence subtracting 1

	T0MCR = MR0I | MR0R; //Set the Match control register 

	//----------Setup TIMER0 Interrupt-------------

	//Using Slot 0 for TIMER0

	VICVectAddr0 = (int) myTimer0_ISR; //Pointer Interrupt Function (ISR)

	VICVectCntl0 = 0x20 | 4;

	VICIntEnable |= (1 << 4); //Enable timer0 int , 4th bit=1

	T0TCR = 0x02; //Reset Timer

	T0TCR = 0x01; //Enable timer
}
//------------------------------------------------------------------------------------------------
void initTimer1(void) {
//	/*Assuming that PLL0 has been setup with CCLK = 60Mhz and PCLK also = 60Mhz.*/
//
//	T1CTCR = 0x0;
//	T1PR = PRESCALE - 1; //(Value in Decimal!) - Increment T0TC at every 60000 clock cycles
//						 //Count begins from zero hence subtracting 1
//						 //60000 clock cycles @60Mhz = 1 mS
//
//	T1TCR = 0x02; //Reset Timer

	/*Assuming that PLL0 has been setup with CCLK = 60Mhz and PCLK also = 60Mhz.*/
	/* CCLK = 55.296M PCLK also = 55.296M */

	//----------Configure Timer0-------------
	T1CTCR = 0x0;

	T1PR = PRESCALE - 1; //60000 clock cycles @60Mhz = 1 mS

	T1MR0 = MR1_DELAY_MS - 1; // 0.5sec (Value in Decimal!) Zero Indexed Count - hence subtracting 1

	T1MCR = MR1I | MR1R; //Set the Match control register

	//----------Setup TIMER0 Interrupt-------------

	//Using Slot 0 for TIMER0

	VICVectAddr3 = (unsigned) myTimer1_ISR; //Pointer Interrupt Function (ISR)

	VICVectCntl3 = 0x20 | 5;

	VICIntEnable |= (1 << 5); //Enable timer0 int , 4th bit=1

	T1TCR = 0x02; //Reset Timer

	T1TCR = 0x01; //Enable timer
}
//------------------------------------------------------------------------------------------------
void delay_ms(uint32_t milliseconds) {
//	T1TCR = 0x02; //Reset Timer
//	T1TCR = 0x01; //Enable timer
//
//	while (T1TC < milliseconds)
//		; //wait until timer counter reaches the desired delay
//
//	T1TCR = 0x00; //Disable timer
	uint32_t index = 0;

	/* default system clock is 72MHz */
	for (index = (11026 * milliseconds); index != 0; index--) {
	}

}
//------------------------------------------------------------------------------------------------
