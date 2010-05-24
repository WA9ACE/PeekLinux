/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM (6301)
|  Modul   :  
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Type definitions and function prototypes for the real time clock driver
|             SPR 1725, re-wrote file for new RTC driver implementation.
+----------------------------------------------------------------------------- 

    Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
    Description: Removal of clone code: entity GDI
    Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
    As a result, interface for rtc_get_time_date() has been changed. The change is only for locosto.
*/ 

#ifndef DEF_RTC__H
#define DEF_RTC__H
/*==== INCLUDES ===================================================*/
#include <string.h>
#include "typedefs.h"
#include "gdi.h"
#include "kbd.h"
/*==== EXPORT =====================================================*/
/*
 * type definitions
 */


 typedef enum 
{
	RTC_TIME_FORMAT_12HOUR,
	RTC_TIME_FORMAT_24HOUR
} T_RTC_TIME_FORMAT;

typedef struct {
	UBYTE	day;
	UBYTE	month;
	USHORT	year;
} 	T_RTC_DATE;


typedef struct
{	UBYTE	minute;
	UBYTE	hour;
	UBYTE   second;
	T_RTC_TIME_FORMAT	format;
	BOOL	PM_flag;
} T_RTC_TIME;

/*
    Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
    Description: Removal of clone code: entity GDI
    Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
                 This type is added to differenciate the functionality in the function.
*/
typedef enum
{
	RTC_TIME_TYPE_CURRENT,
	RTC_TIME_TYPE_ALARM
} T_RTC_TIME_TYPE;

typedef void (*RtcCallback) (void*);  /* RTC event handler        */

/*
 * Prototypes
 */


BOOL rtc_clock_cleared();// wrapper for RTC_RtcReset();

UBYTE rtc_set_time_date(T_RTC_DATE* date, T_RTC_TIME* time); // wrapper for  RTC_setTimeDate();

/*
    Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
    Description: Removal of clone code: entity GDI
    Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date()
    As a result interface of rtc_get_time_date() has been changed. A new parameter 'type' has been added.
                 
*/
UBYTE rtc_get_time_date(T_RTC_DATE* date, T_RTC_TIME* time , T_RTC_TIME_TYPE type ); // wrapper for RTC_getTimeDate();

UBYTE rtc_set_alarm(T_RTC_DATE* date , T_RTC_TIME* time, RtcCallback callback_func );//wrapper for RTC_setAlarm();

/*
    Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
    Description: Removal of clone code: entity GDI
    Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date()
    So rtc_get_alarm() is removed. To get the time which is set for alarm, use rtc_get_time_date() with RTC_TIME_TYPE_ALARM as third argument                 
*/

UBYTE rtc_unset_alarm();//wrapper for RTC_UnsetAlarm();

UBYTE rtc_set_time_format(T_RTC_TIME_FORMAT format);//wrapper for RTC_Set12HourMode();





#endif /* #ifndef DEF_RTC_H */
