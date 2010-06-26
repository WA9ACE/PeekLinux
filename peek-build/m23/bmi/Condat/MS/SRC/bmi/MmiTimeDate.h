#ifndef _DEF_MMI_TIMEDATE_H_
#define _DEF_MMI_TIMEDATE_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiTimeDate.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the root mofule for the basic MMI
                        
********************************************************************************
 $History: MmiTimeDate.h

 	Jan 06,2004 REF: CRR 27859 xkundadu
	Description:  Clock: Alarm does not go off even if timer times out.
	Solution: If the mfw_td_set_alarm() function fails, display the 
	'Write Failed" message.
        Added 'TIDA_SAVEFAILED' enum to display 'Write Failed' for set alarm fail case.


	   
 $End

*******************************************************************************/
#include "mfw_ffs.h"


#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

/* SPR#1428 - SH - New Editor changes
 * New time/date data */
 
#ifdef NEW_EDITOR

typedef enum
{
	TIDA_INIT,			/* Initialise */
	TIDA_DATE, 			/* Enter date */
	TIDA_TIME,			/* Enter time */
	TIDA_ALARMDATE,		/* Enter alarm date */
	TIDA_ALARMTIME,		/* Enter alarm time */
	TIDA_SAVE,			/* Save data to flash & display "Saved" dialog */

//	Jan 06,2004 REF: CRR 27859 xkundadu
//	Description:  Clock: Alarm does not go off even if timer times out.
//	Solution: If the mfw_td_set_alarm() function fails, display the 
//	'Write Failed" message.

//    Added 'TIDA_SAVEFAILED' enum to display 'Write Failed' for set alarm fail case.
	TIDA_SAVEFAILED  // 
}E_TIMER_EVENTS;

T_MFW_HND tida_create(T_MFW_HND parent_win);
T_MFW_HND tida_destroy(T_MFW_HND win);

#else /* NEW_EDITOR */

/* OLD VERSION */

/* REASONS                  */
typedef enum
{
	DATE_SHOW = TimeDateDummy,
	TIME_SHOW,
	DATE_ENTRY,
	TIME_ENTRY,
	ENTRY_OK,
	SAVED,
	TIDA_ABORT,
	TIDA_CLEAR,
	TIDA_TO_IDLE
}
DAT_TIME_CASE;
void time_date_init();
void tida_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons);
void date_time_edt_destroy  (T_MFW_HND own_window);
T_MFW_HND date_time_edt_create(T_MFW_HND parent_window);
void date_time_destroy  (T_MFW_HND own_window);
T_MFW_HND date_time_create (T_MFW_HND parent_window);
void time_date_delete();

#endif /* NEW_EDITOR */


/* Functions below are common to old and new versions */

/* PROTOTYPES               */
//menu handler functions                                        
int alarmTimeDate(MfwMnu* m, MfwMnuItem* i);
int settingsTimeDate(MfwMnu* m, MfwMnuItem* i);
int clear_alarm(MfwMnu* m, MfwMnuItem* i);
int twelve_hour_clock(MfwMnu* m, MfwMnuItem* i);
int twentyfour_hour_clock(MfwMnu* m, MfwMnuItem* i);
void alarm_check(void);  // RAVI

#endif

