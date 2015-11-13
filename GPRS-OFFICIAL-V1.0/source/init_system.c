#include <LPC213x.H>                       /* LPC213x definitions  */
#include "init_system.h"

/************************************************************/
/*default(if not call init_PLL function)
 Fosc = Fxtal
 M=5,P=2
 Fcclk = Fosc*M
 Fcco = Fcclk*2*P
 Fpclk = Fcclk/4
 */
/*sample 
 Fosc = 12M
 Fcclk= Fosc*M = 12*5 = 60M
 Fcco = Fcclk*2*P = 60*2*2 = 240M
 Fpclk = Fcclk / 2 = 30M
 */
/************************************************************/

void init_PLL(void) {
	PLLCFG = 0x00000024;// P=2,M=5 Set multiplier and divider of PLL to give 60.00 Mhz
	PLLCON = 0x00000001;				      	// Enable the PLL

	PLLFEED = 0x000000AA;			// Update PLL registers with feed sequence
	PLLFEED = 0x00000055;//A correct feed sequence must be written to the PLLFEED register in order for changes to
						 //the PLLCON and PLLCFG registers to take effect.

	while (!(PLLSTAT & 0x00000400));   // test Lock bit

	PLLCON = 0x00000003;				      	// Connect the PLL

	PLLFEED = 0x000000AA;				   		//Update PLL registers
	PLLFEED = 0x00000055;

	VPBDIV = 0x00000001;			//Set the VLSI peripheral bus to 60.000Mhz
}
//-----------------------------------------------------------------------

