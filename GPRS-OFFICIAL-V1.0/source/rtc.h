#ifndef __RTC_H
#define __RTC_H
//------------------------------------------------------------------------------------------------
#include <LPC213x.H>                       /* LPC213x definitions  */
#include "system.h"
//-----------------------------------------------------------------------------------------------
#define CIIR_SEC	0x01
#define CIIR_MIN	0x02
#define CIIR_HOUR	0x04
#define CIIR_DOM	0x08
#define CIIR_DOW	0x10
#define CIIR_DOY	0x20
#define CIIR_MON	0x40
#define CIIR_YEAR	0x80

#define AMRSEC		0x00000001  /* Alarm mask for Seconds */
#define AMRMIN		0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR		0x00000004  /* Alarm mask for Hours */
#define AMRDOM		0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW		0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY		0x00000020  /* Alarm mask for Day of Year */
#define AMRMON		0x00000040  /* Alarm mask for Month */
#define AMRYEAR		0x00000080  /* Alarm mask for Year */

#define PREINT_RTC	0x00000696; //New CLK on Keil board //0x000001C8  /* Prescaler value, integer portion,
				    			//PCLK @ 12Mhz //PCLK = 15Mhz */
#define PREFRAC_RTC	0x00004000; //New CLK on Keil board //0x000061C0  /* Prescaler value, fraction portion,
				    			//PCLK @ 12Mhz //PCLK = 15Mhz */
#define ILR_RTCCIF	0x01
#define ILR_RTCALF	0x02

#define CCR_CLKEN	0x01
#define CCR_CTCRST	0x02
#define CCR_CLKSRC	0x10
//--------------------------------------------------------------------------------------
__irq void RTCHandler(void);
void get_time_from_meter(_RTC_time time);
void RTC_init(void);
void RTC_start(void);
void RTC_stop(void);
void RTC_CTCReset(void);
void RTC_SetTime(_RTC_time time);
void RTC_SetAlarm(_RTC_time Alarm);
//void RTC_GetTime (void);
void RTC_SetAlarmMask(uint8_t AlarmMask);

#endif /* end __RTC_H */
//--------------------------------------------------------------------------------------

