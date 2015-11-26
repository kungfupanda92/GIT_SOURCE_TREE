#include "uart0.h"
//----------------------------------------------------------------------------------------------
void initUart0(uint32_t baudrate) {
	unsigned long Fdiv;
	VICIntEnClr = 1 << 6;	/* Disable Interrupt */
	PINSEL0 |= 0x00000005; // Select Txd and Rxd , Txd is not used in our case.
	//PINSEL0 &= 0xFFFFFFFA; // Select Txd and Rxd , Txd is not used in our case.

    //If you'r not acquianted to UARTs then use below settings as it is ,
	//I'll cover UARTs and its related Interrupt programming in upcoming Tutorial.
	//Below Settings Assume PCLK and CCLK are running at 60Mhz!
	/* 7 bits, Even Parity, 1 Stop bit     */
	U0LCR = 0x9A; // set DLAB=1 so we can set divisor bits
	
	Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
	U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
	
	U0FDR = 0x10;
	//The above setting will set Baud rate to 19.2k - its just my Preference
	
	U0FCR = 0x6; // reset Rx and Tx FIFO
	U0FCR = 0x1; // enable FIFO
							 //Bits [7 to 6] = [00] for 1 character interrupt
							 //Bits [7 to 6] = [01] for 4 character interrupt
							 //Bits [7 to 6] = [10] for 8 character interrupt	
							 //Bits [7 to 6] = [11] for 14 character interrupt			
	U0LCR = 0x1A; // now make DLAB=0
	U0IER = 0x1; // to keep things simple just enable "Receive Data Available(RDA)" interrupt
	U0TER = 0x80;
	//----------Setup UART0 Interrupt-------------

	//Any Slot with Lower Priority than TIMER0's slot will suffice
	VICVectAddr1 = (int)myUart0_ISR; //Pointer Interrupt Function (ISR)
	VICVectCntl1 = 0x20 | 6;
	VICIntEnable |= (1<<6); //Enable Uart0 interrupt , 6th bit=1
}
//---------------------------------------------------------------------------------------------
int UART0_Char (int ch)  {                 /* Write character to Serial Port    */

//  if (ch == '\n')  {
//    while (!(U0LSR & 0x20));
//    U0THR = CR;                          /* output CR */
//  }
  while (!(U0LSR & 0x20));
  return (U0THR = ch);
}
//----------------------------------------------------------------------------------------------
void UART0_Send(char *text) {
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART0_Char(text[i]);
}
//----------------------------------------------------------------------------------------------


