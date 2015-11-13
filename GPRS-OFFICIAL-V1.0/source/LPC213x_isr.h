#ifndef __LPC213x_ISR
#define __LPC213x_ISR

__irq void myTimer0_ISR(void);
__irq void myTimer1_ISR(void);
__irq void myUart0_ISR(void);
__irq void myUart1_ISR(void);
void DefaultVICHandler (void) __irq;

#endif	/*  */
