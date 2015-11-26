#include "uart1.h"
//--------------------------------------------------------------------------------------------
/* implementation of putchar (also used by printf function to output data)    */
void initUart1(uint32_t baudrate){
	unsigned long Fdiv;

	VICIntEnClr = 1 << 7;	/* Disable Interrupt */

	PINSEL0 |= 0x00050000; // /* Enable RxD1 and TxD1              */
	//PINSEL0 &= 0xFFFAFFFF; // /* Enable RxD1 and TxD1              */
	
	U1LCR = 0x83; // set DLAB=1 so we can set divisor bits
								//* 8 bits, no Parity, 1 Stop bit     */
	Fdiv = ( Fpclk / 16 ) / baudrate ;	/*baud rate */
	U1DLM = Fdiv / 256;
	U1DLL = Fdiv % 256;
	
	U1FDR = 0x10;
	//The above setting will set Baud rate to 19.2k - its just my Preference
	
	U1FCR = 0x6; // reset Rx and Tx FIFO
	U1FCR = 0x1; // enable FIFO
	U1LCR = 0x03; // now make DLAB=0 
	
	U1IER = 0x1; // Enable RDA UART1
	//U1IER = 0x01 | 0x02 | 0x04;	/* Enable UART1 interrupt */
	U1TER = 0x80;
	//----------Setup UART1 Interrupt-------------

	//Any Slot with Lower Priority than TIMER0's slot will suffice

	VICVectAddr2 = (int)myUart1_ISR; //Pointer Interrupt Function (ISR)

	VICVectCntl2 = 0x20 | 7; 

	VICIntEnable |= (1<<7); //Enable Uart0 interrupt , 7th bit=1 	
}
//***************************************************************************
int UART1_Char (char ch)  {                 /* Write character to Serial Port    */
  if (ch == '\n')  {
    while (!(U1LSR & 0x20));
    U1THR = CR;                          /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}
//***************************************************************************
void UART1_Send(char *text) {
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART1_Char(text[i]);
}
//***************************************************************************

