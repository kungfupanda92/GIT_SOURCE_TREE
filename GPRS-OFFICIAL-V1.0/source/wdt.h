/*
 * wdt.h
 *
 *  Created on: Oct 11, 2015
 *      Author: TaiVo
 */

#ifndef WDT_H_
#define WDT_H_

#include <LPC213x.H>                       /* LPC213x definitions  */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define Tpclk	72//ns

#define WDEN		0x00000001
#define WDRESET		0x00000002
#define WDTOF		0x00000004
#define WDINT		0x00000008

#define WDT_FEED_VALUE	0xFFFFFFFF	//value reload - Max Value - 310 Seconds

void init_Watchdog(void);
void clear_watchdog(void);

#endif /* WDT_H_ */
