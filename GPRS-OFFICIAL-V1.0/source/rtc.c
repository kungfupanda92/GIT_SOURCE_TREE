#include "rtc.h"
//--------------------------------------------------------------------------------------
extern unsigned int half_hour;
extern _rtc_flag rtc_flag;
//--------------------------------------------------------------------------------------
__irq void RTCHandler(void) {
	if (ILR & ILR_RTCCIF) {	//interrupt counter
		ILR |= ILR_RTCCIF; /* clear interrupt flag */
	}
	if (ILR & ILR_RTCALF) {	//interrupt alarm
		ILR |= ILR_RTCALF;/* clear interrupt flag */
		rtc_flag.bits.mode_save_halft_hour=0;
		half_hour=0;
		if(rtc_flag.bits.mode_save_halft_hour){
			half_hour = (ALMIN)?1:0;
			ALMIN = (ALMIN)?0:30;
		}
		else ALMIN=0;
		//rtc_flag.bits.counter_minute = 1;
		rtc_flag.bits.auto_save_data = 1;
	}
	VICVectAddr = 0; /* Acknowledge Interrupt */
}
//--------------------------------------------------------------------------------------
void RTC_init(void) {
	/*--- ALARM registers --- AMR - 8 bits*/
	AMR = 0xff;		//Mark All Alarm
	CIIR = 0x00;	//Dissable all counter_alarm
	ALMIN =0;
	AMR &= ~AMRMIN;
	/*-----------------------*/
	//CIIR |= CIIR_MIN;	//set Counter interrupt
	//---------------------------
	CCR = 2;
	CCR = 0;
	PREINT = PREINT_RTC;
	PREFRAC = PREFRAC_RTC;
	// enable interrupts RTC for each second changed
	VICVectAddr13 = (unsigned) RTCHandler; 		//Set the timer ISR vector address
	VICVectCntl13 = 0x20 | 13;					//Set channel
	VICIntEnable |= (1 << 13);					//Enable the interrupt
	return;
}
//--------------------------------------------------------------------------------------
void RTC_start(void) {
	/*--- Start RTC counters ---*/
	//CCR |= CCR_CLKEN;
	CCR = (CCR_CLKEN | CCR_CLKSRC);	//Enable RTC and use the external 32.768kHz crystal
	ILR = ILR_RTCALF;					//Clears the RTC interrupt flag
}
//--------------------------------------------------------------------------------------
void RTC_stop(void) {
	/*--- Stop RTC counters ---*/
	CCR &= ~CCR_CLKEN;
}
//--------------------------------------------------------------------------------------
void RTC_CTCReset(void) {
	/*--- Reset CTC ---*/
	CCR |= CCR_CTCRST;
}
//--------------------------------------------------------------------------------------
void RTC_SetTime(_RTC_time time) {
	SEC = time.second;
	MIN = time.minute;
	HOUR = time.hour;
	DOM = time.day_of_month;
	DOW = time.day_of_week;
	MONTH = time.month;
	YEAR = time.year;
}
//--------------------------------------------------------------------------------------
void RTC_SetAlarm(_RTC_time Alarm) {
	ALSEC = Alarm.second;
	ALMIN = Alarm.minute;
	ALHOUR = Alarm.hour;
	ALDOM = Alarm.day_of_month;
	ALDOW = Alarm.day_of_week;
	ALDOY = Alarm.day_of_year;
	ALMON = Alarm.month;
	ALYEAR = Alarm.year;
}
//--------------------------------------------------------------------------------------
/*
void RTC_GetTime (void) {

	current_time.second = SEC;
	current_time.minute = MIN;
	current_time.hour = HOUR;
	current_time.day_of_month = DOM;
	current_time.day_of_week = DOW;
	current_time.day_of_year = DOY;
	current_time.month = MONTH;
	current_time.year = YEAR;
}
*/
//--------------------------------------------------------------------------------------
void RTC_SetAlarmMask(uint8_t AlarmMask) {
	/*--- Set alarm mask ---*/
	AMR = AlarmMask;
}
//--------------------------------------------------------------------------------------

