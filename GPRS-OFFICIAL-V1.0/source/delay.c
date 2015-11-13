#include "delay.h"
//------------------------------------------------------------------------------
extern _program_counter program_counter;
//------------------------------------------------------------------------------
void delay_n50ms(unsigned int n) {
	//--------
	if (n == 0)
		return;
	//--------
	program_counter.timer_delay = 0;
	clear_watchdog();	//clear wdt while wait delay
	while (program_counter.timer_delay < n);	//wait for delay
}
//------------------------------------------------------------------------------
void delay_nsecond(unsigned char nsencond) {
	unsigned int i;

	i = nsencond * 20;
	program_counter.timer_delay = 0;
	clear_watchdog();	//clear wdt while wait delay
	while (program_counter.timer_delay < i);	//wait for delay

}
//*******************************************************************************

//*******************************************************************************

