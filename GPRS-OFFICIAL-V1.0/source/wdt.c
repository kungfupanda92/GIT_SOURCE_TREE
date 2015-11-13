/*
 * wdt.c
 *
 *  Created on: Oct 11, 2015
 *      Author: TaiVo
 */
#include "wdt.h"

//----------------------------------------------------------------------
void init_Watchdog(void){// unit of time is milisecond

	WDMOD &= ~WDTOF;		/* clear the time-out terrupt flag */

	//WDTC = time*(1000000/Tpclk);// watchdog timer out = (1/(11.0592*5/4))*2^32 =
	WDTC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
	WDMOD = WDEN | WDRESET;	// watchdog timer enable and reset mode

	WDFEED = 0xAA;		/* Feeding sequence */
	WDFEED = 0x55;
}
//-----------------------------------------------------------------------
void clear_watchdog(void){
	WDFEED = 0xAA;		/* Feeding again */
	WDFEED = 0x55;
}
//------------------------------------------------------------------------
