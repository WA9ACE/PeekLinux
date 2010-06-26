#ifndef _MFW_TD_H_
#define _MFW_TD_H_

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	$Workfile:: mfw_td.c			$|
| $Author:: NDH						$Revision:: 1					$|
| CREATED: 03.03.2003		       		$Modtime:: 10.04.00 14:58		$|
| STATE  : code														$|
+--------------------------------------------------------------------+

   MODULE  : MFW_TD

   PURPOSE : This modul contains Time and Date functionality.

*/

#include <string.h>


#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_utils.h"

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"
#include "rtc.h"

/***************************Data types****************************************
**/


#define T_MFW_TIME T_RTC_TIME
#define T_MFW_DATE T_RTC_DATE

typedef enum 
{
	MFW_TIME_FORMAT_12HOUR,
	MFW_TIME_FORMAT_24HOUR
} T_MFW_TIME_FORMAT;
/*SPR 2639, added data structure definition for writing to FFS*/
typedef struct
{	T_MFW_TIME alarm_time;
	T_MFW_DATE alarm_date;

}	T_MFW_ALARM_INFO;

/*
 * TD Parameter
 */
#define T_MFW_TD_PARA UBYTE

/*
 *  Time and date Control Block
 */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_TD_PARA  para;
} T_MFW_TD;
/*************************************Events**********************************************/

#define MFW_TD_ALARM 		1

/***************************Prototypes****************************************
**/
void mfw_td_init(void); // Initialise time and date on ME switch-on  // RAVI


T_MFW_HND mfw_td_create(MfwHnd hWin, MfwEvt event, MfwCb cbfunc);//specify MFW event handler
MfwRes mfw_td_delete (MfwHnd h);//delete event handler
MfwRes mfw_td_set_time(T_MFW_TIME* time);//set the current time
MfwRes mfw_td_set_date(T_MFW_DATE* date);//et the current date



char* mfw_td_get_clock_str(void); // return clock string (ASCII).Used by MmiIdle // RAVI
char* mfw_td_get_date_str(void); // return date string(ASCII) // RAVI

T_MFW_DATE* mfw_td_get_date(void);//return date structure .Used by MmiTimeDate  // RAVI
T_MFW_TIME* mfw_td_get_time(void);//return time structure  // RAVI

MfwRes mfw_td_set_alarm(T_MFW_TIME* time, T_MFW_DATE* date);//set alarm

MfwRes mfw_td_cancel_alarm(void);//cancel alarm  // RAVI

MfwRes mfw_td_get_alarm(T_MFW_TIME* time, T_MFW_DATE* date);// MmiTimeDate, editing

MfwRes mfw_td_set_time_format(T_MFW_TIME_FORMAT format);//set time format
/*SPR 2639, added prototype for this function so it can be called from outside the module */
UBYTE mfw_td_convert_to_24_hour(T_MFW_TIME* time);//returns the hour value in 24-hour clock format

void mfw_td_exit(void); //kill any data structures  // RAVI

/* Added to remove warning Aug - 11 */
#ifdef NEPTUNE_BOARD
EXTERN S32 ffs_fread(const S8 *name, void *addr, S32 size);
EXTERN S8 ffs_fwrite(const S8 *pathname, void *src, S32 size);
#endif //NEPTUNE_BOARD
/* End - remove warning Aug - 11 */
#endif //_MFW_TD_H_
