#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>
#include "system.h"
#include "wdt.h"
//*******************************************************************************
void delay_n50ms (unsigned int n);
void delay_nsecond (unsigned char nsencond);

#endif	/*  */
