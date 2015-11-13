#ifndef __MAIN_H
#define __MAIN_H

#include <LPC213x.H>                       /* LPC213x definitions  */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "init_system.h"
#include "gpio.h"
#include "uart0.h"
#include "uart1.h"
#include "timer.h"
#include "system.h"
#include "gprs_com.h"
#include "delay.h"
#include "read_para.h"
#include "LPC213x_isr.h"
#include "wdt.h"
#include "rtc.h"
#include "iap.h"

#include "save_flash.h"
//-----------------------------------------------------------------------
void var_start (void);
void gpio_config(void);
void init_VIC(void);
void enable_ext_wdt (void);
//-----------------------------------------------------------------------
#endif	/*  */
