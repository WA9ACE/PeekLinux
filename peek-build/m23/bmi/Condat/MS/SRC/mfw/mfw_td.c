
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_td.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 12              $|
| CREATED: 21.09.98                     $Modtime:: 2.03.00 11:49    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_TD

   PURPOSE : time and date functions

   EXPORT  :

   TO DO   :

   $History:: mfw_td.c    
	 Dec 05, 2006 REF:OMAPS00106887 Prabakar R(a0393213)
	 Description:Compilation error when FF_POWER_MANAGEMENT is not defined
	 	 
       Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
       Description: Removal of clone code: entity GDI
       Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
       As a result interface of rtc_get_time_date() has been changed. The changes are only for locosto.
                 
  	Jan 06,2004 REF: CRR 27859 xkundadu
	Description:  Clock: Alarm does not go off even if timer times out.
	Solution: Check whether FFS is formatted before writing into FFS.
			If it is not formatted, format it and create /mmi folder in FFS.

	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX                                     $
 *
// Issue Number : SPR#12159 on 07/04/04 by Rashmi C N and Deepa M D
*/


#define ENTITY_MFW

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif


#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "drv_tmr.h"
#include "mfw_tim.h"
#include "mfw_td.h"
#include "mfw_win.h"
#include "mfw_acie.h"
#include "dspl.h"
		
//	Jan 06,2004 REF: CRR 27859 xkundadu
//	Description:  Clock: Alarm does not go off even if timer times out.
//	Solution: Check whether FFS is formatted before writing into FFS.
//			If it is not formatted, format it and create /mmi folder in FFS.

// Included this header file to call FFS related functions
#include "mfw_ffs.h" 

#define TD_STRING_LEN 15

#define ONE_MINUTE 60000

/***************************Prototypes****************************************
**/
void mfw_td_signal (MfwEvt event, void* para); // Used to send event to MMI.
int mfw_td_sign_exec (MfwHdr *cur_elem, MfwEvt event, T_MFW_TD_PARA *para); // Used by mfw_td_signal() function to send event to MMI.
GLOBAL BOOL mfw_td_response_cb (ULONG opc, void * data);
static int tdCommand (U32 cmd, void *h);

BOOL dates_match(T_MFW_DATE* date1, T_MFW_DATE* date2);//returns TRUE if date paarmeters are the same
BOOL times_match(T_MFW_TIME* time1, T_MFW_TIME* time2);//returns TRUE if time parameters are the same
//void mfw_td_alarm_callback(void* para);/*SPR 2639, alarm callback function no longer needed*/
int mfw_timer_cb(MfwEvt e, MfwTim *t);
extern int flash_formatted(void);
/***************************global variables****************************************
**/
EXTERN MfwHdr * current_mfw_elem;

T_MFW_TIME current_time;
T_MFW_DATE current_date;

/*SPR 2639, added alarm time and date as well as a timer handle global*/
T_MFW_TIME alarm_time;
T_MFW_DATE alarm_date;


MfwHnd timer_handle= NULL;

char time_string[TD_STRING_LEN];
char date_string[TD_STRING_LEN];
static int pSlot;                       /* primitive handler slot   */


/***************************Public methods****************************************
**/

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_init   |
+--------------------------------------------------------------------+

   PURPOSE : initialise time and date and create primitive handler

*/

void mfw_td_init()
{
    T_MFW_ALARM_INFO alarmInfo;/*SPR 2639*/


	TRACE_EVENT("mfw_td_init");
	/*
     * install prim handler
     */
    pSlot = aci_create(mfw_td_response_cb,NULL);/*put in dummy callback handler*/
    mfwCommand[MfwTypTd] = (MfwCb) tdCommand;


/*if clock not cleared*/
#ifndef WIN32
	if(!rtc_clock_cleared())
	{	/*get time/date from driver and initialise these*/
		/*
		Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
             Description: Removal of clone code: entity GDI
             Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
             As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
             consistent with the new interface.
		*/
	  	#ifndef NEPTUNE_BOARD
  		rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
  		#else
		rtc_get_time_date(&current_date, &current_time); 
  		#endif
	}
	else
	{
		/*
		Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
             Description: Removal of clone code: entity GDI
             Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
             As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
             consistent with the new interface.
		*/
	  	#ifndef NEPTUNE_BOARD
  		rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
  		#else
		rtc_get_time_date(&current_date, &current_time); 
  		#endif
		/*use a default time/date*/
		current_time.second = 0;
		current_time.minute = 0;
		current_time.hour = 0;

		current_date.year = 2000;
		current_date.month = 1;
		current_date.day = 1;
		mfw_td_set_time(&current_time);
		mfw_td_set_date(&current_date);
		mfw_td_set_time_format(MFW_TIME_FORMAT_24HOUR);
		mfw_td_cancel_alarm();
	}
	/*SPR 2639, read alarm info from FFS rather than driver*/
	ffs_fread("/mmi/alarminfo",&alarmInfo, sizeof(T_MFW_ALARM_INFO));

	memcpy(&alarm_time, &alarmInfo.alarm_time, sizeof(T_MFW_TIME));
	memcpy(&alarm_date, &alarmInfo.alarm_date, sizeof(T_MFW_DATE));
	if (dates_match(&current_date, &alarm_date) && times_match(&current_time, &alarm_time))
	{	mfw_td_signal(MFW_TD_ALARM, NULL);}

	timer_handle = timCreate(0,ONE_MINUTE,(MfwCb)mfw_timer_cb);
	/*a0393213 power management - enabling/disabling lcd refresh*/
	/*OMAPS00106887 - compilation error when FF_POWER_MANAGEMENT is not defined
	   The if statement put under the flag*/
	#ifdef FF_POWER_MANAGEMENT
	if(FFS_flashData.refresh==1)
	#endif
	timStart(timer_handle);
	/*SPR 2639, set to default values for simulation*/
#else
	current_time.second = 0;
	current_time.minute = 0;
	current_time.hour = 0;
	current_date.year = 2000;
	current_date.month = 1;
	current_date.day = 1;
	alarm_time.second = 0;
	alarm_time.minute = 0;
	alarm_time.hour = 0;
	alarm_date.year = 2000;
	alarm_date.month = 1;
	alarm_date.day = 1;
#endif

}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_create   |
+--------------------------------------------------------------------+

   PURPOSE : Link callback function with MFW+TD events

*/
T_MFW_HND mfw_td_create(MfwHnd hWin, MfwEvt event, MfwCb cbfunc)
{
    MfwHdr *hdr;
    T_MFW_TD *para;

    TRACE_EVENT("mfw_td_timer_create");

    hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    para = (T_MFW_TD *) mfwAlloc(sizeof(T_MFW_TD));

    if (!hdr || !para)
        return 0;

    para->emask = event;
    para->handler = cbfunc;

    hdr->data = para;
    hdr->type = MfwTypTd;

    return mfwInsert((MfwHdr *) hWin,hdr);


}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_set_time   |
+--------------------------------------------------------------------+

   PURPOSE : set the current time

*/
MfwRes mfw_td_set_time(T_MFW_TIME* time)
{	int result;

	TRACE_EVENT("mfw_td_set_time");
#ifndef WIN32
	result = rtc_set_time_date(&current_date, time);
	if (result == 0)
	{	memcpy(&current_time, time, sizeof(T_MFW_TIME));
		return MfwResOk;
	}
	TRACE_EVENT_P1("RTC driver Error:%d", result);
	return MfwResErr;
#else/*SPR 2639*/
	memcpy(&current_time, time, sizeof(T_MFW_TIME));
	return MfwResOk;
#endif
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_set_date   |
+--------------------------------------------------------------------+

   PURPOSE : set the current date

*/
MfwRes mfw_td_set_date(T_MFW_DATE* date)
{	int result;
	TRACE_EVENT("mfw_td_set_date");
#ifndef WIN32
	result = rtc_set_time_date(date, &current_time);
	if (result == 0)
	{
		memcpy(&current_date, date, sizeof(T_MFW_DATE));
		return MfwResOk;
	}
	TRACE_EVENT_P1("RTC driver Error:%d", result);

	return MfwResErr;
#else/*SPR 2639*/
	memcpy(&current_date, date, sizeof(T_MFW_DATE));
	return MfwResOk;
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_get_clock_str   |
+--------------------------------------------------------------------+

   PURPOSE : returns an ASCII string containing the current time

*/
char* mfw_td_get_clock_str()
{	TRACE_EVENT("mfw_td_get_clock_str");
	/*SPR 2639, use global var, don't need to update time as it's already updated every minute*/
	/*
	Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
         Description: Removal of clone code: entity GDI
         Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
         As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
         consistent with the new interface.
	*/
  	#ifndef NEPTUNE_BOARD
	rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
	#else
	rtc_get_time_date(&current_date, &current_time); 
	#endif
        
        // Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
	// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
	// Bug : The AM PM for 12 hour fomrat was not being displayed, Also there were some bugs with the 12 hour fomat
	// Change 1: Uncommented the above line "rtc_get_time_date(&current_date, &current_time); "
	// as the time was not getting refreshed when the time format was changed
	// Change 2 : To display the time in appropriate format in idle screen 

	if(current_time.format==RTC_TIME_FORMAT_12HOUR)
	{
		if(current_time.PM_flag==1)
			sprintf(time_string, "%02d:%02d pm ", current_time.hour, current_time.minute);
		else
			sprintf(time_string, "%02d:%02d am ", current_time.hour, current_time.minute);
	}  
	else
	      sprintf(time_string, "%02d:%02d", current_time.hour, current_time.minute);
	return time_string;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_get_date_str   |
+--------------------------------------------------------------------+

   PURPOSE : returns an ASCII string containing the current date

*/
char* mfw_td_get_date_str()
{	TRACE_EVENT("mfw_td_get_date_str");
	/*SPR 2639, use global var, don't need to update time as it's already updated every minute*/
	//rtc_get_time_date(&current_date, &current_time);
	sprintf(date_string, "%02d/%02d/%04d", current_date.month, current_date.day, current_date.year);
	return date_string;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_get_date   |
+--------------------------------------------------------------------+

   PURPOSE : returns the current date in a data structure

*/
T_MFW_DATE* mfw_td_get_date()
{ //	int result;    // RAVI
	TRACE_EVENT("mfw_td_get_date");
/*SPR 2639*/
#ifndef WIN32
	/*
	Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
         Description: Removal of clone code: entity GDI
         Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
         As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
         consistent with the new interface.
	*/
  	#ifndef NEPTUNE_BOARD
	rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
	#else
	rtc_get_time_date(&current_date, &current_time); 
	#endif
#endif
	return &current_date;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_get_time   |
+--------------------------------------------------------------------+

   PURPOSE : returns the current time in a data structure

*/
T_MFW_TIME* mfw_td_get_time()
{
	TRACE_EVENT("mfw_td_get_time");
/*SPR 2639*/
#ifndef WIN32
	/*
	Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
         Description: Removal of clone code: entity GDI
         Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
         As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
         consistent with the new interface.
	*/
  	#ifndef NEPTUNE_BOARD
	rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
	#else
	rtc_get_time_date(&current_date, &current_time); 
	#endif
#endif
	return &current_time;
}



/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_set_alarm   |
+--------------------------------------------------------------------+

   PURPOSE : set alarm to date and time passed to function

*/
MfwRes mfw_td_set_alarm(T_MFW_TIME* time, T_MFW_DATE* date)
{
/*SPR 2639*/
#ifndef WIN32
	int result;
	T_MFW_ALARM_INFO alarmInfo;
	
	/*SPR 2639, save alarm info to FFS*/
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P5("alarm Time:%d:%d, date %d/%d%d", time->hour, time->minute, date->day,
		date->month, date->year);
	/***************************Go-lite Optimization changes end***********************/
	memcpy(&alarmInfo.alarm_time, time, sizeof(T_MFW_TIME));
	memcpy(&alarmInfo.alarm_date, date, sizeof(T_MFW_DATE));

//	Jan 06,2004 REF: CRR 27859 xkundadu
//	Description:  Clock: Alarm does not go off even if timer times out.
//	Solution: Format FFS, if it is not formatted.Create mmi folder in FFS if it's not present.

	// Check whether FFS is formatted or not.
	if(flash_formatted() == FALSE)
	{
		TRACE_EVENT("FFS  NOT formatted ");
		// If FFS is not formatted, format the /mmi volume.
		if(ffs_format("/mmi", 0x2BAD ) == EFFS_OK)
		{
			TRACE_EVENT("Format success");
		}
		else 
		{
			TRACE_EVENT("FFS  NOT formatted ");
			//FFS Format failed return ERROR
			return MfwResErr;
		}
		
	}
	else
	{
		// Already formatted.
		TRACE_EVENT("FFS  formatted ");
	}

	//Create 'mmi' folder in FFS.
	//If it is already there, ffs_mkdir() wont create it again.
	switch(ffs_mkdir("/mmi") )
	{
		case EFFS_OK:
			TRACE_EVENT("Created mmi!!");
			break;
		case EFFS_EXISTS: 
			TRACE_EVENT("EFFS_EXISTS!");
			break;
		case EFFS_NAMETOOLONG: 
			TRACE_EVENT("EFFS_NAMETOOLONG!");
			break;
		case EFFS_BADNAME: 
			TRACE_EVENT("EFFS_BADNAME!");
			break;
		case EFFS_NOSPACE: 
			TRACE_EVENT("EFFS_NOSPACE!");
		case EFFS_FSFULL: 
			TRACE_EVENT("EFFS_FSFULL!");
			break;
		case EFFS_MEMORY: 
			TRACE_EVENT("EFFS_MEMORY!");
			break;
		case EFFS_MSGSEND: 
			TRACE_EVENT("EFFS_MSGSEND!");
			break;
		default:
			TRACE_EVENT("default!");
			break;
			
	}
	
	
	result =ffs_fwrite("/mmi/alarminfo",&alarmInfo, sizeof(T_MFW_ALARM_INFO));
	
	if (result<0)
	{	TRACE_EVENT("RTC alarm files not written");
		return MfwResErr;
	}
#endif
	TRACE_EVENT("mfw_td_set_alarm");
	memcpy(&alarm_time, time, sizeof(T_MFW_TIME));
	memcpy(&alarm_date, date, sizeof(T_MFW_DATE));

	return MfwResOk;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_cancel_alarm   |
+--------------------------------------------------------------------+

   PURPOSE : Cancels the alarm setting

*/
MfwRes mfw_td_cancel_alarm()
{
/*SPR 2639*/
#ifndef WIN32
	int result;
	T_MFW_ALARM_INFO alarmInfo;
	/*SPR 2639*/
	alarm_date.day = 0;/*Impossible date*/
	alarm_date.month= 0;
	alarm_date.year=0;

	memcpy(&alarmInfo.alarm_time, &alarm_time, sizeof(T_MFW_TIME));
	memcpy(&alarmInfo.alarm_date, &alarm_date, sizeof(T_MFW_DATE));
	result =ffs_fwrite("/mmi/alarminfo",&alarmInfo, sizeof(T_MFW_ALARM_INFO));

	if (result<0)
	{	TRACE_EVENT("RTC alarm file not written");
		return MfwResErr;
	}
#endif
	TRACE_EVENT("mfw_td_cancel_alarm");
	alarm_date.day = 0;/*Impossible date*/
	alarm_date.month= 0;
	alarm_date.year=0;
	return MfwResOk;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_get_alarm   |
+--------------------------------------------------------------------+

   PURPOSE : gets the current alarm settings

*/
MfwRes mfw_td_get_alarm(T_MFW_TIME* time, T_MFW_DATE* date)
{
/*SPR 2639*/
#ifndef WIN32
	int result;
//	char debug[50];  // RAVI
	T_MFW_ALARM_INFO alarmInfo;

	/*SPR 2639 copy alarm info from FFS to memory*/
	result =ffs_fread("/mmi/alarminfo",&alarmInfo, sizeof(T_MFW_ALARM_INFO));
	memcpy( time,&alarmInfo.alarm_time, sizeof(T_MFW_TIME));
	memcpy( date,&alarmInfo.alarm_date, sizeof(T_MFW_DATE));
	/*if that failed set to default values*/
	if (result<0)
	{
		TRACE_EVENT("Alarm files don't exist");
		time->minute=0;
		time->second=0;
		time->hour=0;

		date->day = 0;
		date->month=0;
		date->year =0;

	}
#endif
	TRACE_EVENT("mfw_td_get_alarm");
	memcpy(&alarm_time, time, sizeof(T_MFW_TIME));
	memcpy(&alarm_date, date, sizeof(T_MFW_DATE));
	return MfwResOk;


}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_set_time_format   |
+--------------------------------------------------------------------+

   PURPOSE : sets the time format of the clock

*/
MfwRes mfw_td_set_time_format(T_MFW_TIME_FORMAT format)
{
	int result;

	TRACE_EVENT("mfw_td_time_format");
/*SPR 2639*/
#ifndef WIN32
	result = rtc_set_time_format((T_RTC_TIME_FORMAT)format);
	if (result == 0)
		return MfwResOk;
	TRACE_EVENT_P1("RTC driver Error:%d", result);
	return MfwResErr;
#else
	return MfwResOk;
#endif
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_set_time_format   |
+--------------------------------------------------------------------+

   PURPOSE : sets the time format of the clock

*/
void mfw_td_exit()
{
    TRACE_FUNCTION("mfw_td_exit()");

    /*
     * remove prim handler
     */
    aci_delete(pSlot);
   timDelete(timer_handle); /*SPR 2639, delete time handle*/

}
/***************************Private methods****************************************
**/
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: dates_match   |
+--------------------------------------------------------------------+

   PURPOSE : returns TRUE if the two dates passed match

*/
BOOL dates_match(T_MFW_DATE* date1, T_MFW_DATE* date2)
{
	if (date1->year == date2->year)
		if (date1->month == date2->month)
			if (date1->day == date2->day)
				return TRUE;
	return FALSE;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_convert_to_24_hour   |
+--------------------------------------------------------------------+

   PURPOSE : converts a time to 24-hour format

*/
UBYTE mfw_td_convert_to_24_hour(T_MFW_TIME* time)
{
	if (time->format == RTC_TIME_FORMAT_24HOUR)
	{
		return time->hour;
	}
	/*SPR 2639, improved function*/
	else
	{	/*if after noon*/
		if (time->PM_flag == TRUE)
		{
			return (time->hour + 12);
		}
		else
		{	/*if midnight*/
			if (time->hour == 12)
				return 0;
			else
				return time->hour;
		}
	}
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: times_match   |
+--------------------------------------------------------------------+

   PURPOSE : returns TRUE if time data strcutures passed match

*/
BOOL times_match(T_MFW_TIME* time1, T_MFW_TIME* time2)
{
	if (time1->minute == time2->minute)
	{
		if ( mfw_td_convert_to_24_hour(time1) == mfw_td_convert_to_24_hour(time2))
			return TRUE;
	}
	return FALSE;

}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_signal   |
+--------------------------------------------------------------------+

   PURPOSE : called to pass event to MMI layer

*/
void mfw_td_signal (MfwEvt event, void *para)
{
 UBYTE temp = dspl_Enable(0);
    

  if (mfwSignallingMethod EQ 0)
  {
    if (mfwFocus)
        if (mfw_td_sign_exec(mfwFocus,event,para))
			{
				dspl_Enable(temp);
				return;
			}
    if (mfwRoot)
        mfw_td_sign_exec(mfwRoot,event,para);
  }
  else
    {
      MfwHdr * h = 0;

      /*
       * Focus set, then start here
       */
      if (mfwFocus)
        h = mfwFocus;
      /*
       * Focus not set, then start root
       */
      if (!h)
        h = mfwRoot;

      /*
       * No elements available, return
       */

      while (h)


      {
        /*
         * Signal consumed, then return
         */
        if (mfw_td_sign_exec (h, event, para))
			{
				dspl_Enable(temp);
				return;
			}

        /*
         * All windows tried inclusive root
         */
        if (h == mfwRoot)
			{
				dspl_Enable(temp);
				return;
			}

        /*
         * get parent window
         */
        h = mfwParent(mfwParent(h));
		if(h)
			h = ((MfwWin * )(h->data))->elems;
      }
      mfw_td_sign_exec (mfwRoot, event, para);
    }
	dspl_Enable(temp);
	return;
}
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_sign_exec   |
+--------------------------------------------------------------------+

   PURPOSE : Used by mfw_td_signal() to pass event to MMI layer

*/
int mfw_td_sign_exec (MfwHdr *cur_elem, MfwEvt event, T_MFW_TD_PARA *para)
{   
	

    while (cur_elem)
    {

	
		
        if (cur_elem->type == MfwTypTd)
        {
         
            T_MFW_TD *td_data;
            td_data = (T_MFW_TD *) cur_elem->data;
	  
			
            if (td_data->emask & event)
            {
                td_data->event = event;
				
//	Jan 06,2004 REF: CRR 27859 xkundadu
//	Description:  Clock: Alarm does not go off even if timer times out.
//	Solution:  Assign para to NULL, instead of *para = NULL which is not correct.
	
           //    para = NULL; /*we're not passing any data to MMI*/  // RAVI
			   
                if (td_data->handler)
                {
                  // PATCH LE 06.06.00
                  // store current mfw elem
                  current_mfw_elem = cur_elem;
                  // END PATCH LE 06.06.00
                  if ((*(td_data->handler))(td_data->event,
                                             (void *) &td_data->para))
                    return TRUE;
                }
            }
        }
        cur_elem = cur_elem->next;
    }

    return FALSE;

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TD           |
| STATE   : code                        ROUTINE : td_delete         |
+--------------------------------------------------------------------+

  PURPOSE : delete TD event handler

*/

MfwRes mfw_td_delete (MfwHnd h)
{
    TRACE_FUNCTION("mfw_td_delete()");


    if (!h || !((MfwHdr *) h)->data)
        return MfwResIllHnd;

    if (!mfwRemove((MfwHdr *) h))
        return MfwResIllHnd;

    mfwFree((U8 *) ((MfwHdr *) h)->data,sizeof(T_MFW_TD));
    mfwFree((U8 *) h,sizeof(MfwHdr));

    return MfwResOk;
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_TD           |
| STATE   : code                        ROUTINE : tdCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int tdCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            mfw_td_delete(h);
            return 1;
        default:
            break;
    }

    return 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_td_response_cb   |
+--------------------------------------------------------------------+

   PURPOSE : Response Callback Handler.

*/

GLOBAL BOOL mfw_td_response_cb (ULONG opc, void * data)
{
  //TRACE_FUNCTION ("mfw_td_response_cb");


  return FALSE;       /* not processed by extension */
}

/*SPR 2639, removed alarm callback and added timer callback*/
/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_TD            |
| STATE  : code                         ROUTINE: mfw_timer_cb  |
+--------------------------------------------------------------------+

   PURPOSE : timer callback, called every minute to check alarm
*/

int mfw_timer_cb(MfwEvt e, MfwTim *t)
{	// char debug[60]; // RAVI
	TRACE_EVENT("mfw_timer_cb()");

#ifndef WIN32
	/*
	Aug 02, 2006 REF:OMAPS00083404 Prabakar R (a0393213)
         Description: Removal of clone code: entity GDI
         Solution: Two similar functions(rtc_get_time_date() and rtc_get_alarm()) are made into one function rtc_get_time_date().
         As a result, interface for rtc_get_time_date() has been changed. Change has been done here to be 
         consistent with the new interface.
	*/
  	#ifndef NEPTUNE_BOARD
	rtc_get_time_date(&current_date, &current_time,RTC_TIME_TYPE_CURRENT); 
	#else
	rtc_get_time_date(&current_date, &current_time); 
	#endif

	timStart(timer_handle);/*restart timer */
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P3("current date:%d/%d/%d",current_date.day, current_date.month, current_date.year);
	TRACE_EVENT_P2("current time:%d/%d",current_time.hour, current_time.minute);
	TRACE_EVENT_P2("format:%d,PM flag:%d",current_time.format, current_time.PM_flag);
	TRACE_EVENT_P3("alarm date:%d/%d/%d",alarm_date.day, alarm_date.month, alarm_date.year);
	TRACE_EVENT_P2("alarm time:%d/%d",alarm_time.hour, alarm_time.minute);
	TRACE_EVENT_P2("format:%d,PM flag:%d",alarm_time.format, alarm_time.PM_flag);
	/***************************Go-lite Optimization changes end***********************/
	/*if alarm is set*/
	if (alarm_date.day != NULL)
	{  if (dates_match(&current_date, &alarm_date))
		{	if (times_match(&current_time, &alarm_time))
				{	
					mfw_td_signal(MFW_TD_ALARM, NULL);
					mfw_td_cancel_alarm();

				}
			else
				TRACE_EVENT("times don't match");
		}
		else
			TRACE_EVENT("dates don't match");
	}
#endif
	return 1;
}
