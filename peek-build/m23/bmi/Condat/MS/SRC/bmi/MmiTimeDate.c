/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiTimeDate.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    22/02/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

  

********************************************************************************

 $History: MmiTimeDate.c

    Apr 03, 2007    REF: OMAPS00123995  x0039928
    Description:   Alarm is playing midi or imelody files only once.
    Solution: Loop parameter is changed from FALSE to TRUE
    
    Aug 10, 2006    REF: OMAPS00076372  x0039928
    Description:   Incoming Alram with midi ringer during voice call
    Solution: Beep tone is played when alarm expires during the call

    Apr 18, 2006    REF:OMAPS00075303 Prabakar R (a0393213)
    Description: The valid date field is 01/01/2000 to 31/12/2099, when input invalid date it will indicate "saved" but
    return to Idle screen, the date is not the input date. (for example: input the date "01/01/2110" then saved,
    it will indicate "saved", but the date in Idle is "01/01/2010" instead.)
    Solution   : Validation for the date was wrong. Upperbound was not included. It is included.
 
    Apr 04, 2006    REF:OMAPS00073057 Prabakar R (a0393213)
    Description: When the time format is 12hr, user can not set current time to be 12 hour.
    After user set the current time to be 12 hour and return the idle screen, the current time isn't 12 hour
    Solution   : Validation for 12 hours format was wrong. It is corrected.

	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.
	
	Apr 05, 2005    REF: ENH 29994 xdeepadh
   	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
  	Solution: Generic Midi Ringer and Midi Test Application were implemented.
  	
	Jan 06,2004 REF: CRR 27859 xkundadu
	Description:  Clock: Alarm does not go off even if timer times out.
	Solution: If the mfw_td_set_alarm() function fails, display the 
	'Write Failed" message.

 	Oct 14, 2004    REF: CRR 25756 xnkulkar
	Description: Alarm setting not allowed
	Solution:	We are checking the time format (12/24 Hour) and accordingly setting the 
			format flag. Also the conversion of time from 24 to 12 or 12 to 24 hour format is done
			before displaying it on the alarm editor screen.

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
#include "gdi.h" //ES!!
#include "audio.h"
#include "cus_aci.h"
#include "mmiColours.h"

#include "mfw_sys.h"
#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"

#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_sat.h"
#include "Mfw_td.h"/*SPR 2639*/

//x0pleela 22 May, 2007 DR: OMAPS00127483
#ifdef FF_PHONE_LOCK
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif //FF_MIDI_RINGER 
#endif /* FF_PHONE_LOCK */

#include "dspl.h"



#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiMenu.h"
#include "MmiMain.h"
#include "MmiIdle.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiTimeDate.h"
#include "MmiSounds.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif

// Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mmiCall.h"
#include "mfw_midi.h"
extern T_call call_data;
#endif

//x0pleela 02 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "mfw_sim.h"

//x0pleela 22 May, 2007 DR: OMAPS00127483
#include "MmiSounds.h"
extern int 			phlock_alarm;				/* flag to check whether alarm event has occured or not 	*/ 

//x0pleela 22 May, 2007 DR: OMAPS00127483
extern T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
#ifdef FF_MMI_AUDIO_PROFILE
extern UBYTE mfwAudPlay;//flag for audio 
#endif

#endif //FF_PHONE_LOCK

void alarm_screen(void);   // RAVI
int time_date_cb(T_MFW_EVENT event,void* para);/*SPR 2639 handles events from MFW*/

	T_MFW_DATE alarmDate;/*SPR 2639 we need a global to store the alarm date */
//#ifdef NEW_EDITOR
	T_MFW_TIME alarmTime; /*SPR 2639*/
//#endif

/* SPR#1428 - SH - New Editor changes.
 * Time and date module pretty much rewritten to use new formatted input of editor.
 */
 
#ifdef NEW_EDITOR

/* LOCAL FUNCTION PROTOTYPES */
T_MFW_HND tida_create(T_MFW_HND parent_win);
static int tida_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void tida_exec_cb(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
T_MFW_HND tida_edit_create(T_MFW_HND parent, USHORT Identifier, USHORT TitleId, char *buffer, char *formatString);
void tida_edit_cb(T_MFW_HND win, USHORT Identifier, SHORT value);
void tida_show_dlg(T_MFW_HND win, USHORT textId1, USHORT textId2);
UBYTE tida_check_date(char *datestring);
UBYTE tida_check_time(char *timestring);

#define MAX_DIG_TIDA		20	        /* Max size of buffer, arbitrary value  */

/* TIME/DATE main structure */

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        parent_win;
  T_MFW_HND			win;				/* The main time/date window */
  T_MFW_HND			edit_win;			/* The editor window */
  char				buffer[MAX_DIG_TIDA];			/* Buffer to store currently edited time/date */
} T_tida;

#else /* NEW_EDITOR */

/* OLD VERSION */

static int tida_edt_kbd_long_cb (MfwEvt e, MfwKbd *kc);
static void tida_edt_tim_out_cb(T_MFW_EVENT event,T_MFW_TIM * t);
static int check_time (T_MFW_HND win, void * edt_dat);
static int check_date (T_MFW_HND win, void * edt_dat);
static int tida_edt_kbd_cb (MfwEvt e, MfwKbd *kc);
static void tida_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int tida_edt_win_cb  (T_MFW_EVENT event,T_MFW_WIN * win);
static int tida_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void tida_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void reset_edt_attr_sett(int editZone, U32 colIndex, U8 font,U8 mode,
					U8 *controls, char *text,U16 size,MfwEdtAttr* attr);

static MfwHnd win;                      /* our window               */
static DisplayData DisplayInfo;

#define MAX_DIG_TIDA		20	        /* size of buffer (??)  */
#define MAX_LEN_DATE		11	        /* maximum number of digits date*/
#define MAX_LEN_TIME		6	        /* maximum number of digits time*/
#define DATE_TEXT_X         0
#define DATE_TEXT_Y         0

typedef enum
{
	ALARM,
	SETTING
} timeDateMode;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        parent_win;
  T_MFW_HND        tida_win;
  USHORT		   display_id1;
  USHORT		   display_id2;
  DAT_TIME_CASE    state;               /* setting state            */
} T_tida;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        parent_win;
  T_MFW_HND        tida_edt_win;
  T_MFW_HND        kbd_handle;
  T_MFW_HND        kbd_long_handle;
  T_MFW_HND        editor_handle;
  T_MFW_HND        tim_out_handle;
  DAT_TIME_CASE    state;               /* setting state            */
  char edtbuf1[MAX_DIG_TIDA];
  MfwEdtAttr edit1Attr;
  UBYTE            index;
}T_tida_edt;



/*
 * Settings menu subs:
 */

#define TIMERCLEAR		2500

//Flag to indicate whether we're setting the current or alarm date/time.
static UBYTE DateTimeEntryMode;

extern UBYTE getcurrentAlarmTone(void);  //mmisounds
#endif /* NEW_EDITOR */

T_MFW_HND td_handle;
/* FUNCTIONS BELOW ARE COMMON TO OLD AND NEW VERSION */

/*SPR 2639 removed function alarm_check()*/


/*******************************************************************************

 $Function:  time_date_init

 $Description:	 initialises time and date

 $Returns:		none.

 $Arguments:	none
 
*******************************************************************************/
void time_date_init()
{	mfw_td_init();

	td_handle = mfw_td_create(NULL, MFW_TD_ALARM, time_date_cb);
}
/*******************************************************************************

 $Function:  time_date_init

 $Description:	 deletes mfw td 

 $Returns:		none.

 $Arguments:	none
 
*******************************************************************************/
void time_date_delete()
{
	mfw_td_delete(td_handle);
	mfw_td_exit();
}
/*******************************************************************************

 $Function:  time_date_cb

 $Description:	 handles alarm event from MFW

 $Returns:		none.

 $Arguments:	none
 
*******************************************************************************/
int time_date_cb(T_MFW_EVENT event,void* para)
{	TRACE_EVENT_P1("time_date_cb():%d",event);
	alarm_screen();

	return 1;  // RAVI.
}


/*******************************************************************************

 $Function:  alarm_screen_cb added for SPR 2184

 $Description:	 Handles alarm screen keypresses

 $Returns:		none.

 $Arguments:	window handle, identifier, reason(key pressed)
 
*******************************************************************************/
void alarm_screen_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{	
TRACE_FUNCTION("alarm_screen_cb()");
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
/*get current alarm id*/

//x0pleela 22 May, 2007 DR: OMAPS00127483 
//Stopping the alarm ringer based on the MfwAudPlay flag
#ifdef FF_PHONE_LOCK
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
#endif
	{
#endif // FF_PHONE_LOCK
/*stop alarm*/
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else  	
  #ifdef FF_MMI_RINGTONE /*MSL Stop */
     audio_StopSoundbyID( AUDIO_BUZZER, ALARMTONE_SELECT); 
  #else
  	{
  	UBYTE currentAlarm;
  	currentAlarm=getcurrentAlarmTone();
  	audio_StopSoundbyID(AUDIO_BUZZER, currentAlarm);
  	}
  #endif
#endif
//x0pleela 22 May, 2007  DR: OMAPS00127483 
#ifdef FF_PHONE_LOCK
	}
#endif // FF_PHONE_LOCK
	//x0pleela 05 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	if( phlock_alarm) 
		phlock_alarm = FALSE;
#endif //FF_PHONE_LOCK
		
}


/*******************************************************************************

 $Function:  alarm_screen

 $Description:	 displays alarm screen

 $Returns:		none.

 $Arguments:	none
 
*******************************************************************************/
void alarm_screen(void)
{
	T_MFW_HND idle_win = idle_get_window();
	T_DISPLAY_DATA display_info;
	T_MFW_TIME* time;
    static char text[25];
	
    
    TRACE_FUNCTION("alarm_screen()");
    
  	/*SPR 2639, convert function to use new MFW functions*/
  	time = mfw_td_get_time();

	sprintf(text, "%02d:%02d", time->hour, time->minute);
// Apr 05, 2005    REF: ENH 29994 xdeepadh
/*SPR 2639 get current alarm id and play it*/
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//If Alarm is recieved , while in call screen,dont play the tone.
#ifdef FF_MIDI_RINGER  
if((!(call_data.calls.numCalls)))
{
	/* Apr 03, 2007    REF: OMAPS00123995  x0039928 */
	mfw_ringer_start(AS_RINGER_MODE_ALARM,TRUE,sounds_midi_ringer_start_cb);
}
else
	// Aug 10, 2006    REF: OMAPS00076372  x0039928
	// Fix: Beep tone is played when alarm expires during the call
	audio_PlaySoundID( AUDIO_SPEAKER, TONES_RINGING_TONE, 0 ,  AUDIO_PLAY_INFINITE);
#else

#ifdef FF_MMI_RINGTONE /* MSL Alarm Tone */
       audio_PlaySoundID( AUDIO_BUZZER,  ALARMTONE_SELECT, 0 ,  AUDIO_PLAY_INFINITE);
#else
	{
	UBYTE currentAlarm;/*SPR 2184*/
  	currentAlarm=getcurrentAlarmTone();
  	audio_PlaySoundID( AUDIO_BUZZER, currentAlarm, 0 ,  AUDIO_PLAY_INFINITE);
	}
#endif
#endif
  	/*SPR 2639 end*/

  	
	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtAlarm, TxtNull , COLOUR_STATUS);
		
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)alarm_screen_cb, FOREVER, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
    display_info.TextString2  = text;
//x0pleela 05 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	//x0pleela 22 May, 2007  DR: OMAPS00127483 
	//Instead of querying sec driver for the status of phone lock, we are checking for the phock password entry window
	//When ringer for alarm is playing in phone locked state, Audio usues the DMA channels
	//which will be active. The DMA channels will be available only after the audio file is fully played.
	//This will cause that process to be blocked until the DMA channel becomes in-active and in turn if we try to 
	//query or access secure driver we are blocked until the audio file is completely played.
	//Since this is a hardware constraint, we found a workaround for this issue and are stopping the ringer  
	//for alarm before accessing secure driver for enabling or disbaling the phone lock.
	if( phlock_win_handle )
	{
		phlock_alarm = TRUE;
	     info_dialog( mfwParent(mfwHeader()), &display_info );
	}
	else
#endif //FF_PHONE_LOCK
    /* Call Info Screen
    */
    info_dialog( idle_win, &display_info );


}




/*******************************************************************************

 $Function:  clear_alarm 

 $Description:	 handles menu option to cancel any existing alarm setting

 $Returns:		status int

 $Arguments:	menu and item (not used)
 
*******************************************************************************/
int clear_alarm(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND win =  mfwParent( mfw_header() );
	T_DISPLAY_DATA display_info;

	
	/*SPR 2639, changed to use new MFW functions*/
	mfw_td_cancel_alarm();
		
	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtAlarm, TxtCancelled , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
	
    /* Call Info Screen
    */
    info_dialog( win, &display_info );


    return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:  	twelve_hour_clock

 $Description:	 handles selection of twleve hour clock in menu

 $Returns:		status int

 $Arguments:	menu and item (not used)
 
*******************************************************************************/
int twelve_hour_clock(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND win =  mfwParent( mfw_header() );
	T_DISPLAY_DATA display_info;
   // char text[25];  // RAVI
  	
  	setClockFormat(MFW_TIME_FORMAT_12HOUR);/*SPR 2639*/
  	
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtClockSetTo, TxtTwelveHour, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */

    info_dialog( win, &display_info );
    return MFW_EVENT_CONSUMED;

}
/*******************************************************************************

 $Function:  	twentyfour_hour_clock

 $Description:	handles selection of twleve hour clock in menu

 $Returns:		status int

 $Arguments:	menu and item (not used)
*******************************************************************************/
int twentyfour_hour_clock(MfwMnu* m, MfwMnuItem* i)
{	T_MFW_HND win =  mfwParent( mfw_header() );
	T_DISPLAY_DATA display_info;
  //  char text[25];  // RAVI
  	
  	setClockFormat(MFW_TIME_FORMAT_24HOUR);/*SPR 2639*/
  	
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtClockSetTo, TxtTwentyfourHour, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */

    info_dialog( win, &display_info );
    return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:  alarmTimeDate  

 $Description:	Called when user selects alarm in menu.  Begins editor to enter
 				desired alarm time and date.

 $Returns:		status int 

 $Arguments:	menu and item (not used)
 
*******************************************************************************/
int alarmTimeDate(MfwMnu* m, MfwMnuItem* i)
{
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
  T_MFW_HND		idle_win	= idle_get_window();
  T_MFW_HND		win			= tida_create(idle_win); /*  Parent_window is idle */

  T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
  T_tida		*data		= (T_tida *)win_data->user;

  TRACE_FUNCTION ("MmiTimeDate:alarmTimeDate()");
  
  if(data->win)
  {
	  SEND_EVENT(data->win, TIDA_ALARMDATE,0,0);

  }
#else /* NEW_EDITOR */
  T_MFW_HND idle_win = idle_get_window();
  T_MFW_HND win = date_time_create(idle_win);//  parent_window is idle

  T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  T_tida * tida_data = (T_tida *)win_data->user;

  TRACE_FUNCTION ("MmiTimeDate:alarmTimeDate()");
mfw_td_get_alarm(&alarmTime, &alarmDate);
  tida_data->state = DATE_ENTRY;
  DateTimeEntryMode = ALARM;

  if(tida_data->tida_win)
  {
	  SEND_EVENT(tida_data->tida_win,DATE_ENTRY,0,tida_data);

  }
#endif /* NEW_EDITOR */
  return 1;
}

/*******************************************************************************

 $Function:  	settingsTimeDate  

 $Description:	 settings date and time menu function

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int settingsTimeDate(MfwMnu* m, MfwMnuItem* i)
{
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
  T_MFW_HND		idle_win	= idle_get_window();
  T_MFW_HND		win			= tida_create(idle_win); /*  Parent_window is idle */

  T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
  T_tida		*data		= (T_tida *)win_data->user;

  TRACE_FUNCTION ("MmiTimeDate:settingsTimeDate()");
  
  if(data->win)
  {
	  SEND_EVENT(data->win, TIDA_DATE,0,0);

  }
#else /* NEW_EDITOR */
  T_MFW_HND idle_win = idle_get_window();
  T_MFW_HND win = date_time_create(idle_win);//  parent_window is idle

  T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  T_tida * tida_data = (T_tida *)win_data->user;

  TRACE_FUNCTION ("MmiTimeDate:settingsTimeDate()");

  tida_data->state = DATE_ENTRY;
  DateTimeEntryMode = SETTING;
  
  if(tida_data->tida_win)
  {
	  SEND_EVENT(tida_data->tida_win,DATE_ENTRY,0,tida_data);

  }
#endif /* NEW_EDITOR */

  return 1;
}


/* SPR#1428 - SH - New Editor changes
 * New time/date code below.  Uses tida_exec_cb as main "object". */
 
#ifdef NEW_EDITOR

/*******************************************************************************

 $Function:		tida_create 

 $Description:	Create the main time/date window

 $Returns:		The window handle

 $Arguments:	parent_win	- The parent window
 
*******************************************************************************/

T_MFW_HND tida_create(T_MFW_HND parent_win)
{
	T_MFW_WIN     * win_data;

	T_tida * data = (T_tida *)ALLOC_MEMORY (sizeof (T_tida));
	data->win = win_create (parent_win, 0, MfwWinVisible, (T_MFW_CB)tida_win_cb);

	TRACE_FUNCTION("MmiTimeDate:date_time_create");

	if (data->win EQ 0)
		return 0;

	/*
	* Create window handler
	*/
	data->mmi_control.dialog	= (T_DIALOG_FUNC)tida_exec_cb;/* dialog main function    */
	data->mmi_control.data		= data;
	data->parent_win			= parent_win;
	win_data					= ((T_MFW_HDR *)data->win)->data;
	win_data->user				= (void *) data;


	/*
	* return window handle
	*/

	return data->win;
}


/*******************************************************************************

 $Function:		tida_destroy

 $Description:	Destroy the main time/date window

 $Returns:		None

 $Arguments:	win		- The main time/date window
 
*******************************************************************************/

T_MFW_HND tida_destroy(T_MFW_HND win)
{
	T_MFW_WIN	*win_data	= ((T_MFW_HDR *)win)->data;
	T_tida		*data		= (T_tida *)win_data->user;//tida main data

	if (win == NULL)
	{
		TRACE_EVENT ("Error : tida_destroy called with NULL Pointer");
		return NULL;   /*a0393213 warnings removal-NULL is returned*/
	}

	if (data->win)
	{
		win_delete(data->win);
	}
	
	if (data)
	{
		FREE_MEMORY((void *)data, sizeof(T_tida));
	}

	return NULL; /*a0393213 warnings removal-NULL is returned*/   
}


/*******************************************************************************

 $Function:		tida_win_cb  

 $Description:	Window callback function for time and date.  This is a non-displaying
 				window, so does nothing!

 $Returns:		None.

 $Arguments:	
 
*******************************************************************************/

static int tida_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
	TRACE_FUNCTION ("MmiTimeDate: tida_win_cb");

	return 1;  // RAVI - Return changed to Return 1.
}


/*******************************************************************************

 $Function:		tida_exec_cb  

 $Description:	Main dialog function for time and date

 $Returns:		None.

 $Arguments:	
 
*******************************************************************************/

static void tida_exec_cb(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN	*win_data	= ((T_MFW_HDR *)win)->data;
	T_tida		*data		= (T_tida *)win_data->user;//tida main data

	T_MFW_TIME* currclock; /*SPR 2639*/
	T_MFW_DATE* currdate;
	
	TRACE_FUNCTION("MmiTimeDate:tida_main");

	
	
	
	 switch(event)
	 {
	 	case TIDA_INIT:
	 		break;
	 		
	 	case TIDA_DATE:
	 		/*SPR 2639*/
	 		currdate = mfw_td_get_date();
			/* No date is set */
			if (currdate->day == 0)
			{
       			sprintf(data->buffer, "dd/mm/yyyy");
       		}
       		/* Date is already set */
       		else
       		{
				sprintf(data->buffer, "%02.02d/%02d/%04d", currdate->day, currdate->month, currdate->year);
			}
	
	 		data->edit_win = tida_edit_create(win, event, TxtEnterDate, data->buffer, "NN\\/NN\\/NNNN");
	 		break;
	 		
	 	case TIDA_TIME:
	 		currclock = mfw_td_get_time();/*SPR 2639*/
			// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
			// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
			// Bug : In 12 hour fomrat the AM and PM was not being set
			// Solution : Displaying the am/pm for editing in the time edit screen.
			
			if(currclock->format==RTC_TIME_FORMAT_12HOUR)
			{
				if(currclock->PM_flag)
					sprintf(data->buffer, "%02d:%02d pm", currclock->hour, currclock->minute);
				else
					sprintf(data->buffer, "%02d:%02d am", currclock->hour, currclock->minute);
		 		data->edit_win = tida_edit_create(win, event, TxtEnterTime, data->buffer, "NN\\:NN\\aa");
			}	
			else
			{
				sprintf(data->buffer, "%02d:%02d", mfw_td_convert_to_24_hour(currclock), currclock->minute);
	 		data->edit_win = tida_edit_create(win, event, TxtEnterTime, data->buffer, "NN\\:NN");
	 		}
	 		break;

	 	case TIDA_ALARMDATE:
			/*SPR 2639*/
	 		mfw_td_get_alarm(&alarmTime, &alarmDate);
	 		/* No date is set */
			if (alarmDate.day == 0)/*SPR 2639*/
			{	currdate = mfw_td_get_date();
				/* If date is set, insert today's date */
				if (currdate->day!=0)
				{
					sprintf(data->buffer, "%02d/%02d/%04d", currdate->day, currdate->month, currdate->year);
				}
				else
				{
       				sprintf(data->buffer, "dd/mm/yyyy");
       			}
       		}
       		/* Date is already set */
       		else
       		{/*SPR 2639*/
				sprintf(data->buffer, "%02d/%02d/%04d", alarmDate.day, alarmDate.month, alarmDate.year);
			}
	
	 		data->edit_win = tida_edit_create(win, event, TxtEnterDate, data->buffer, "NN\\/NN\\/NNNN");
	 		break;

	 	case TIDA_ALARMTIME:
	 		/*SPR 2639*/
	 		//sprintf(data->buffer,"%02d:%02d",alarmTime.hour, alarmTime.minute);
	 		//data->edit_win = tida_edit_create(win, event, TxtEnterTime, data->buffer, "NN\\:NN");

	 		// xnkulkar SPR-25756: Alarm setting not allowed
	 		// Check the time format set and accordingly convert the time before displaying on 
	 		// the editor screen.
	 		currclock = mfw_td_get_time();
			if(alarmTime.format!= currclock->format)
			{
				alarmTime.format = currclock->format;
				if(currclock->format == RTC_TIME_FORMAT_12HOUR)
				{	//converting from 24 hour alram format to 12 hour alarm format
					if (alarmTime.hour < 12)
					{
						alarmTime.PM_flag=0;
					}
					else
					{	//if after noon					
						alarmTime.PM_flag=1;
						alarmTime.hour = alarmTime.hour-12;
					}
				}
				else
				{	//converting from 12 hour alram format to 24 hour alarm format
					if (alarmTime.PM_flag == TRUE)
					{ //
						alarmTime.hour = alarmTime.hour + 12;
					}
					else
					{	//if midnight, set the hour to 0
						if (alarmTime.hour == 12)
							alarmTime.hour = 0;
					}
				}
			}

			// xnkulkar SPR-25756: Alarm setting not allowed
			// Now proceed with displaying the alarm time with am/pm in  alarm time edit screen
			// if the format is 12 hour.
			if(alarmTime.format == RTC_TIME_FORMAT_12HOUR)
			{
				if(alarmTime.PM_flag)
					sprintf(data->buffer, "%02d:%02d pm",alarmTime.hour, alarmTime.minute);
				else
					sprintf(data->buffer, "%02d:%02d am", alarmTime.hour, alarmTime.minute);
		 		data->edit_win = tida_edit_create(win, event, TxtEnterTime, data->buffer, "NN\\:NN\\aa");
			}	
			else
			{
	 			sprintf(data->buffer,"%02d:%02d", mfw_td_convert_to_24_hour(&alarmTime), alarmTime.minute);
	 			data->edit_win = tida_edit_create(win, event, TxtEnterTime, data->buffer, "NN\\:NN");
			}
	 		break;

	 	case TIDA_SAVE:
	 		tida_show_dlg(win, TxtSaved, TxtNull);
	 		AUI_edit_Destroy(data->edit_win);
			tida_destroy(data->win);
			break;			
//	Jan 06,2004 REF: CRR 27859 xkundadu
//	Description:  Clock: Alarm does not go off even if timer times out.
//	Solution: If the mfw_td_set_alarm() function fails, display the 
//	'Write Failed" message.

		//Could not write the alarm details to FFS.
		case TIDA_SAVEFAILED:
			tida_show_dlg(win, TxtWrite, TxtFailed);
	 		AUI_edit_Destroy(data->edit_win);
			tida_destroy(data->win);
			break;
	 }

	 return;
}

/*******************************************************************************

 $Function:		tida_edit_create  

 $Description:	Creates an editor with the supplied properties

 $Returns:		The editor window.

 $Arguments:	parent		- The parent window
 				Identifier	- ID used in callback to identify type of editor
 				TitleId		- The title of the editor
 				buffer		- The text buffer
 				formatString  - The formatting that governs the input
 
*******************************************************************************/

T_MFW_HND tida_edit_create(T_MFW_HND parent, USHORT Identifier, USHORT TitleId, char *buffer, char *formatString)
{
	T_AUI_EDITOR_DATA editor_data;
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, TIMEDATE_EDITOR_AREA, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetEvents(&editor_data, Identifier, FALSE, FOREVER, (T_AUI_EDIT_CB)tida_edit_cb);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TitleId, NULL);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)buffer, strlen(formatString));
	AUI_edit_SetFormatStr(&editor_data, formatString, FALSE, 0);
	AUI_edit_SetMode(&editor_data, ED_MODE_FORMATTED, ED_CURSOR_UNDERLINE);
	return AUI_edit_Start(parent, &editor_data);
}


/*******************************************************************************

 $Function:		tida_edit_cb  

 $Description:	Callback function for editor

 $Returns:		None

 $Arguments:	win			- The main time/date window
 				Identifier	- ID used in callback to identify type of editor
 				value		- What prompted the callback
 
*******************************************************************************/

void tida_edit_cb(T_MFW_HND win, USHORT Identifier, SHORT value)
{
	T_MFW_WIN	*win_data	= ((T_MFW_HDR *)win)->data;
	T_tida		*data		= (T_tida *)win_data->user;//tida main data
	int			minute, hour, day, month, year;
	T_MFW_TIME_FORMAT current_format;
	char pmFlag = '\0';  /* x0039928 - Lint warning fix */

	T_MFW_TIME* currclock;      /*SPR 2639*/    //, *tempclock;   // RAVI
	T_MFW_DATE* currdate;
	

	
	
	switch(value)
	{
		/* Terminate editor */
		case INFO_KCD_HUP:
		case INFO_KCD_RIGHT:
			AUI_edit_Destroy(data->edit_win);

			/* If on the time editing screen, go back to the date editing screen.
			 * Otherwise exit. */

			switch(Identifier)
			{
				case TIDA_TIME:
					SEND_EVENT(data->win, TIDA_DATE, 0, 0);
					break;

				case TIDA_ALARMTIME:
					SEND_EVENT(data->win, TIDA_ALARMDATE, 0, 0);
					break;

				default:
					tida_destroy(win);
					break;
			}
			break;

		/* OK */
		case INFO_KCD_LEFT:
			switch(Identifier)
			{
				case TIDA_DATE:
					if (tida_check_date(data->buffer))
					{
						currdate = mfw_td_get_date();
						sscanf(data->buffer, "%d/%d/%d", &day, &month, &year);
						currdate->day = day;
						currdate->month = month;
						currdate->year = year;
						mfw_td_set_date(currdate);/*SPR 2639*/
						AUI_edit_Destroy(data->edit_win);
						SEND_EVENT(data->win, TIDA_TIME, 0, 0);
					}
					else
					{
						tida_show_dlg(data->win, TxtNotAllowed, TxtNull);
					}
					break;

				case TIDA_TIME:
					if (tida_check_time(data->buffer))
					{	currclock = mfw_td_get_time();/*SPR 2639*/
						current_format = (T_MFW_TIME_FORMAT)currclock->format;
						if(current_format==MFW_TIME_FORMAT_12HOUR)
							sscanf(data->buffer, "%d:%d %c", &hour, &minute,&pmFlag);
						else
						sscanf(data->buffer, "%d:%d", &hour, &minute);
						currclock->hour = hour;
						currclock->minute = minute;
						currclock->second = 0;
						// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
						// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
						// Bug : In 12 hour fomrat the AM and PM was not being set
						// Solution : Setting the PM flag of the rtc.
						
						if(pmFlag=='p')
						{							
							currclock->PM_flag=1; 
						}
						else
						{
							currclock->PM_flag=0; 
						}
						
						// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
						// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
						// Bug : The AM PM for 12 hour fomrat was not being shown, Also there were some bugs with the 12 hour fomat
						// Solution : The following line was commented, to take the time format from the rtc instead of 
						//setting it to 24 hour format
					
						//currclock->format = MFW_TIME_FORMAT_24HOUR;
						mfw_td_set_time(currclock);/*SPR 2639*/
						mfw_td_set_time_format(current_format);
						SEND_EVENT(data->win, TIDA_SAVE, 0, 0);
					}
					else
					{
						tida_show_dlg(data->win, TxtNotAllowed, TxtNull);
					}
					break;

				case TIDA_ALARMDATE:
					if (tida_check_date(data->buffer))
					{
						sscanf(data->buffer, "%d/%d/%d", &day, &month, &year);
						alarmDate.day = (UBYTE)day;
						alarmDate.month = (UBYTE) month;
						alarmDate.year = (USHORT)year;
						AUI_edit_Destroy(data->edit_win);
						SEND_EVENT(data->win, TIDA_ALARMTIME, 0, 0);/*SPR2639*/
						
					}
					else
					{
						tida_show_dlg(data->win, TxtNotAllowed, TxtNull);
					}
					break;

				case TIDA_ALARMTIME:
					if (tida_check_time(data->buffer))
					{ 	
						// xnkulkar SPR-25756: Alarm setting not allowed
						// get the current time and format and set the time format flag acordingly
						currclock = mfw_td_get_time();
						current_format = (T_MFW_TIME_FORMAT)currclock->format;
						if(current_format==MFW_TIME_FORMAT_12HOUR)
							sscanf(data->buffer, "%d:%d %c", &hour, &minute,&pmFlag);
						else
							sscanf(data->buffer, "%d:%d", &hour, &minute);

						alarmTime.hour = (UBYTE)hour;
						alarmTime.minute = (UBYTE)minute;
						alarmTime.second = 0;
						alarmTime.format =  currclock->format;
					
						// xnkulkar SPR-25756: Alarm setting not allowed
						// Depending on the current time format (12/24 Hour), 
						// set the PM_flag.
						if(pmFlag=='p')
						{							
							alarmTime.PM_flag=1; 
						}
						else
						{
							alarmTime.PM_flag=0; 
						}
						// Now that we have the required info, set the alarm
						
						//	Jan 06,2004 REF: CRR 27859 xkundadu
						//	Description:  Clock: Alarm does not go off even if timer times out.
						//	Solution: If the mfw_td_set_alarm() function fails, display the 
						//	'Write Failed" message.
			
						if(mfw_td_set_alarm(&alarmTime, &alarmDate) == MfwResOk)
						{

							TRACE_EVENT("Set Alarm success");
							SEND_EVENT(data->win, TIDA_SAVE, 0, 0);
						}
						else
						{
							TRACE_EVENT("Set Alarm Failed");
							SEND_EVENT(data->win, TIDA_SAVEFAILED, 0, 0);
						}
					}
					else
					{ // time entered is not within the valid range
						tida_show_dlg(data->win, TxtNotAllowed, TxtNull);
					}
					break;
			}
			break;

		default:
			break;
	}

	return;
}


/*******************************************************************************

 $Function:  	tida_show_dlg

 $Description:	Displays a dialog

 $Returns:		None.

 $Arguments:	win		- The parent window
				textId	- Will display "<textId> Not Allowed"
*******************************************************************************/

void tida_show_dlg(T_MFW_HND win, USHORT textId1, USHORT textId2)
{
	T_DISPLAY_DATA display_info;
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, textId1, textId2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */

    info_dialog( win, &display_info );

    return;
}


/*******************************************************************************

 $Function:  	tida_check_date

 $Description:	Checks a date to see if it's valid

 $Returns:		TRUE if it's valid, FALSE otherwise

 $Arguments:	datestring	- The date string
 
*******************************************************************************/

UBYTE tida_check_date(char *datestring)
{
	int day;
	int month;
	int year;

	sscanf(datestring, "%d/%d/%d", &day, &month, &year);

	TRACE_EVENT_P1("String: %s", datestring);
	TRACE_EVENT_P3("Day: %d, Month: %d, Year: %d", day, month, year);

	/* Check for quick fails... */
	
	//Apr 18, 2006    REF:OMAPS00075303 Prabakar R (a0393213)
    //Description: The valid date field is 01/01/2000 to 31/12/2099, when input invalid date it will indicate "saved" but
    //return to Idle screen, the date is not the input date. (for example: input the date "01/01/2110" then saved,
    //it will indicate "saved", but the date in Idle is "01/01/2010" instead.)
    //Solution   : Validation for the date was wrong. Upperbound was not included. It is included.
    
	if (year<2000 || year>2099 || day<1 || day>31 || month<1 || month>12)
		return FALSE;

	/* Check 'day' */
	
	switch(month)
	{
		case 4: /* Apr */
		case 6: /* Jun */
		case 9: /* Sep */
		case 11: /* Nov */
			if (day>30)
				return FALSE;
			break;
			
		case 2: /* Feb */
			/* Leap year... */
			if ((year%4==0) && (year%400!=0))
			{
				if (day>29)
					return FALSE;
			}
			else
			{
				if (day>28)
					return FALSE;
			}
			break;
	}
	
	return TRUE;
}


/*******************************************************************************

 $Function:  	tida_check_time
 
 $Description:	Checks a time to see if it's valid

 $Returns:		TRUE if it's valid, FALSE otherwise

 $Arguments:	timestring	- The time string
 
*******************************************************************************/

UBYTE tida_check_time(char *timestring)
{
	int hour;
	int minute;
	char ampmFlag = '\0'; /* x0039928 - Lint warning fix */
	if(getClockFormat()==MFW_TIME_FORMAT_12HOUR)
		sscanf(timestring, "%d:%d %c", &hour, &minute,&ampmFlag);
	else
	sscanf(timestring, "%d:%d", &hour, &minute);

	TRACE_EVENT_P1("String: %s", timestring);
	TRACE_EVENT_P2("Hour %d, minute %d", hour, minute);

	// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
	// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
	// Bug : In 12 hour fomrat the AM and PM was not being set
	// Solution : Validation for 12 hour format

	// Issue Number : OMAPS00073057 on 04/04/06 by Prabakar R
	// Bug          : When the time format is 12hr, user can not set current time to be 12 hour.
	//                After user set the current time to be 12 hour and return the idle screen, 
	//                the current time isn't 12 hour
	// Solution     : Validation for 12 hour format was wrong. Corrected.
	if (((getClockFormat() == MFW_TIME_FORMAT_24HOUR) && (hour<0 || hour>23 || minute<0 || minute>59))
	     ||((getClockFormat()== MFW_TIME_FORMAT_12HOUR) && (hour<1 || hour>12 || minute<0 || minute>59 || ( ampmFlag!='p' && ampmFlag!='a')))
	   )
		return FALSE;
		
	return TRUE;
}

#else /* NEW_EDITOR */

/* OLD VERSION */

/*******************************************************************************

 $Function:  date_time_create  

 $Description:	create main dialog for time and date (SINGLE_DYNAMIC)

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND date_time_create (T_MFW_HND parent_window)
{
  T_MFW_WIN     * win;

  T_tida * data = (T_tida *)ALLOC_MEMORY (sizeof (T_tida));
  data->tida_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)tida_win_cb);
   
   TRACE_FUNCTION("MmiTimeDate:date_time_create");

  if (data->tida_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)tida_main;/* dialog main function    */
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win                         = ((T_MFW_HDR *)data->tida_win)->data;
  win->user                   = (void *) data;
 

  /*
   * return window handle
   */

	mfwSetSignallingMethod(1);
	winShow(data->tida_win);

	
  return data->tida_win;
}
/*******************************************************************************

 $Function:  date_time_destroy  

 $Description:	 destroy main dialog for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void date_time_destroy  (T_MFW_HND own_window)
{
  T_tida * data;
  T_MFW_WIN * win;

  if (own_window)
  {
     TRACE_EVENT("MmiTimeDate:date_time_destroy");
	 win = ((T_MFW_HDR *)own_window)->data;
     data = (T_tida *)win->user;

	 if(data)
	 {
		 win_delete (data->tida_win);
		 FREE_MEMORY((void *)data,(sizeof(T_tida)));
	 }
  }
}
/*******************************************************************************

 $Function:  date_time_edt_create  

 $Description:	 create editor dialog for time and date(SINGLE_DYNAMIC)

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND date_time_edt_create(T_MFW_HND parent_window)
{
  T_tida_edt * data = (T_tida_edt *)ALLOC_MEMORY (sizeof (T_tida_edt));

  T_MFW_WIN * win;

  data->tida_edt_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)tida_edt_win_cb);

  TRACE_FUNCTION("MmiTimeDate:date_time_edt_create");

  if (data->tida_edt_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)tida_editor;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win                         = ((T_MFW_HDR *)data->tida_edt_win)->data;
  win->user                   = (void *) data;
  /*
   * Create any other handler
   */
  editor_attr_init(&data->edit1Attr, TIMEDATE_EDITOR_AREA, edtCurBar1 | edtModOverWr, 0, (char*)data->edtbuf1,MAX_DIG_TIDA,COLOUR_EDITOR_XX );

  data->tim_out_handle = tim_create(data->tida_edt_win,TIMERCLEAR,(T_MFW_CB)tida_edt_tim_out_cb);
  data->editor_handle = edt_create(data->tida_edt_win,&data->edit1Attr,0,0);
  data->kbd_handle = kbd_create(data->tida_edt_win,KEY_ALL,(T_MFW_CB)tida_edt_kbd_cb);
  data->kbd_long_handle = kbd_create(data->tida_edt_win,KEY_ALL | KEY_LONG,(T_MFW_CB)tida_edt_kbd_long_cb);
  /*
   * return window handle
   */
  return data->tida_edt_win;
}
/*******************************************************************************

 $Function:  date_time_edt_destroy  

 $Description:	destroy editor dialog for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void date_time_edt_destroy  (T_MFW_HND own_window)
{
  T_tida_edt * data ;
  T_MFW_WIN * win;


  if (own_window)
  {
	  TRACE_FUNCTION ("MmiTimeDate:date_time_edt_destroy");

	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_tida_edt *)win->user;

	  if(data)
	  {
		  win_delete (data->tida_edt_win);
	      FREE_MEMORY((void *)data,(sizeof(T_tida_edt)));
	  }
   }
}
/*******************************************************************************

 $Function:  tida_main  

 $Description:	 main dialog function for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static void tida_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_tida * tida_data = (T_tida *)win_data->user;//tida main data
	T_tida_edt * tida_edt_data = (T_tida_edt *) parameter;//tida editor data
	T_MFW_HND tida_edt_win;
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("MmiTimeDate:tida_main");

	 switch(event)
	 {
		  case DATE_SHOW:
			  tida_data->state = DATE_SHOW;
			  tida_data->display_id1 = TxtEnterDate;
			  tida_edt_win = date_time_edt_create(tida_data->tida_win);
			  if(tida_edt_win)
			  {
				SEND_EVENT(tida_edt_win,event,0,tida_data);
			  }
			  break;
		  case DATE_ENTRY:
			  tida_data->state = DATE_ENTRY;
			  tida_data->display_id1 = TxtEnterDate;
			  tida_edt_win = date_time_edt_create(tida_data->tida_win);
			  {
				SEND_EVENT(tida_edt_win,event,0,tida_data);
			  }
			  break;
		  case TIME_ENTRY:
			  tida_data->state = TIME_ENTRY;
			  tida_data->display_id1 = TxtEnterTime;
			  tida_edt_win = date_time_edt_create(tida_data->tida_win);
			  {
				SEND_EVENT(tida_edt_win,event,0,tida_data);
			  }
			  break;
		  case TIME_SHOW:
			  tida_data->state = TIME_SHOW;
			  tida_data->display_id1 = TxtEnterTime;
			  tida_edt_win = date_time_edt_create(tida_data->tida_win);
			  if(tida_edt_win)
			  {
				SEND_EVENT(tida_edt_win,event,0,tida_data);
			  }
			  break;
		  case ENTRY_OK:
				dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtSaved, TxtNull, COLOUR_STATUS);
				dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)tida_info_cb, THREE_SECS, KEY_ALL );
			  display_info.Identifier = SAVED;
		      info_dialog(tida_data->tida_win,&display_info); /* information screen "Saved" */
		      break;
		  case SAVED:
		  case TIDA_ABORT:
		  case TIDA_CLEAR:
		  case TIDA_TO_IDLE:
			  date_time_destroy(tida_data->tida_win);
			  break;
		  default:
			  break;
	 }
}
/*******************************************************************************

 $Function:  tida_info_cb  

 $Description:	 callback function from info dialog for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void tida_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_tida * tida_data = (T_tida *)win_data->user;

	TRACE_FUNCTION ("MmiTimeDate: tida_info_cb");

	switch(identifier)
	{
		case SAVED:
			SEND_EVENT(tida_data->tida_win,identifier,0,NULL);
			break;
		default:
			break;
	}
}

/*******************************************************************************

 $Function:  tida_win_cb  

 $Description:	 windows main callback function for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int tida_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
TRACE_FUNCTION ("MmiTimeDate: tida_win_cb");
  if (event EQ MfwWinVisible)
  {
    /*
     * Top Window has no output
     */
    dspl_ClearAll();
	return 1;
  }
  return 0;
}
/*******************************************************************************

 $Function:  tida_edt_win_cb  

 $Description:	 windows callback function for editor for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int tida_edt_win_cb  (T_MFW_EVENT event,T_MFW_WIN * win)
{


  T_tida_edt * edt_data = (T_tida_edt *)win->user;//tida edt data
  T_MFW_WIN * win_tida =((T_MFW_HDR *)edt_data->parent_win)->data;
  T_tida     * tida_data = (T_tida *)win_tida->user; // tida main data
  char lin1[MAX_LEN_DATE];
  char lin2[MAX_LEN_TIME];
  	T_MFW_TIME* currclock; /*SPR 2639*/
	T_MFW_DATE* currdate;
	//T_MFW_TIME alarmTime; /*SPR 2639*/
	

	currclock = mfw_td_get_time(&currclock);/*SPR 2639*/
	currdate = mfw_td_get_date(&currdate);
	

  TRACE_FUNCTION ("MmiTimeDate:tida_edt_win_cb");

  if (event EQ MfwWinVisible)
  {
	dspl_ClearAll();
    PROMPT(DATE_TEXT_X,DATE_TEXT_Y,0,tida_data->display_id1);
	softKeys_displayId(TxtSoftOK,TxtSoftBack,0,COLOUR_EDITOR_XX);


	if(tida_data->state EQ DATE_SHOW)
	{
		if ((DateTimeEntryMode == SETTING) || (alarmDate.day == 0))
        {	if (currdate->day == 0) //if no date set
       			sprintf(lin1,"dd/mm/yyyy");
       		else
				sprintf(lin1,"%02d/%02d/%04d",currdate->day,currdate->month,currdate->year);
		}
		else
			sprintf(lin1,"%02d/%02d/%04d",alarmDate.day,alarmDate.month,alarmDate.year);
			
		strncpy(edt_data->edtbuf1,lin1,MAX_LEN_DATE);
		edtShow(edt_data->editor_handle);
	}
	else if(tida_data->state EQ DATE_ENTRY)
	{
		if ((DateTimeEntryMode == SETTING) || (alarmDate.day == 0))
       	{	if (currdate->day == 0)
       			sprintf(lin1,"dd/mm/yyyy");//if no date set
       		else
				sprintf(lin1,"%02d/%02d/%04d",currdate->day,currdate->month,currdate->year);
		}
		else
		{	
			sprintf(lin1,"%02d/%02d/%04d",alarmDate.day,alarmDate.month,alarmDate.year);
		}
		strncpy(edt_data->edtbuf1,lin1,MAX_LEN_DATE);
		edt_data->index = 0;
		edtShow(edt_data->editor_handle);
		softKeys_displayId(TxtSoftOK,TxtSoftBack,0,COLOUR_EDITOR_XX);
	}
	else if(tida_data->state EQ TIME_ENTRY)
	{
		if ((DateTimeEntryMode == SETTING) || (alarmDate.day==0))
			sprintf(lin2,"%02d:%02d",mfw_td_convert_to_24_hour(currclock), currclock->minute);
		else
			sprintf(lin2,"%02d:%02d",alarmTime.hour, alarmTime.minute);
		strncpy(edt_data->edtbuf1,lin2,MAX_LEN_TIME);
		edt_data->index = 0;
		edtShow(edt_data->editor_handle);
		softKeys_displayId(TxtSoftOK,TxtSoftBack,0,COLOUR_EDITOR_XX);
	}
	else if(tida_data->state EQ TIME_SHOW)
	{	if ((DateTimeEntryMode == SETTING) || (alarmDate.day == 0))
			sprintf(lin2,"%02d:%02d",mfw_td_convert_to_24_hour(currclock), currclock->minute);
		else
			sprintf(lin2,"%02d:%02d",alarmTime.hour, alarmTime.minute);
		strncpy(edt_data->edtbuf1,lin2,MAX_LEN_TIME);
		edtShow(edt_data->editor_handle);
	}
    return 1;
  }
  return 0;
  
}
/*******************************************************************************

 $Function:  tida_editor  

 $Description:	 dialog function for editor for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static void tida_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_tida_edt * edt_data = (T_tida_edt *)win_data->user;


    T_tida * tida_data = (T_tida *)parameter;//tida main data

	edt_data->state = tida_data->state;

	TRACE_FUNCTION("MmiTimeDate:tida_editor");

  /*
   * depending on event
   */
  switch (event)
  {
	case DATE_SHOW:
	case TIME_SHOW:
	    memset(edt_data->edtbuf1, '\0', sizeof(edt_data->edtbuf1));/* initial buffer  */
		edt_data->index = 0;
		winShow(edt_data->tida_edt_win);
		break;
	case DATE_ENTRY:
	case TIME_ENTRY:
		winShow(edt_data->tida_edt_win);
		break;
	default:
		break;
  }
}
/*******************************************************************************

 $Function:  tida_edt_kbd_cb  

 $Description:	 keyboard event handler for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int tida_edt_kbd_cb (MfwEvt e, MfwKbd *kc)
{

   T_MFW_HND win = mfw_parent(mfw_header());

   T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
   T_tida_edt * edt_data = (T_tida_edt *)win_data->user;//tida edt data

   T_MFW_WIN * win_tida =((T_MFW_HDR *)edt_data->parent_win)->data;
   T_tida * tida_data = (T_tida *)win_tida->user; // tida main data

   char lin1[MAX_LEN_DATE];
   char lin2[MAX_LEN_TIME];
	T_MFW_TIME* currclock; /*SPR 2639*/
	currclock = mfw_td_get_time();/*SPR 2639*/
	
   TRACE_FUNCTION ("MmiTimeDate: tida_edt_kbd_cb");

		switch(kc->code)
		{
			case KCD_0:
			case KCD_1:
			case KCD_2:
			case KCD_3:
			case KCD_4:
			case KCD_5:
			case KCD_6:
			case KCD_7:
			case KCD_8:
			case KCD_9:
			  if(edt_data->state EQ DATE_ENTRY)                  /* date entering            */
			  {
				if(edt_data->index < (MAX_LEN_DATE - 1))
				{
					if((edt_data->index NEQ 2) && (edt_data->index NEQ 5))
					{
						edt_data->edtbuf1[edt_data->index] = '0' + kc->code;
						edtChar(edt_data->editor_handle,edt_data->edtbuf1[edt_data->index]);
						edt_data->index++;
					}

					if((edt_data->index EQ 2) || (edt_data->index EQ 5))
					{
						edtChar(edt_data->editor_handle,'/');
						edt_data->edtbuf1[edt_data->index] = '/';
						edt_data->index++;
					}
				}
			  }
			  else if(edt_data->state EQ TIME_ENTRY)             /* time entering            */
			  {
					if(edt_data->index < (MAX_LEN_TIME -1))
					{
						if(edt_data->index NEQ 2)
						{
							edt_data->edtbuf1[edt_data->index] = '0' + kc->code;
							edtChar(edt_data->editor_handle,edt_data->edtbuf1[edt_data->index]);
							edt_data->index++;
						}
						if(edt_data->index EQ 2)
						{
							edtChar(edt_data->editor_handle,':');
							edt_data->edtbuf1[edt_data->index] = ':';
							edt_data->index++;
						}
					}
			  }
				break;
			case KCD_MNUSELECT:
			case KCD_LEFT:                                 /* select and ok button */
				if(edt_data->state EQ DATE_ENTRY)
				{
					if(check_date(win,edt_data))
					{

						TRACE_EVENT("datecheck ok");
						edtClear(edt_data->editor_handle);
						edtHide(edt_data->editor_handle);   /* verification date successfull */
						date_time_edt_destroy(edt_data->tida_edt_win);
						SEND_EVENT(tida_data->tida_win,TIME_ENTRY,0,tida_data);

					}
					else
					{
						memset(edt_data->edtbuf1, '\0', sizeof(edt_data->edtbuf1));/* initial buffer  */
						edt_data->index = 0;

						sprintf(lin1,"dd/mm/yyyy");
						strncpy(edt_data->edtbuf1,lin1,MAX_LEN_DATE);
						edtReset(edt_data->editor_handle);
						edtShow(edt_data->editor_handle);
					}
				}
				else if(edt_data->state EQ TIME_ENTRY)
				{
					if(check_time(win,edt_data))
					{
						TRACE_EVENT("timecheck ok");
						edtClear(edt_data->editor_handle);
						edtHide(edt_data->editor_handle);   /* verification date successfull */
						date_time_edt_destroy(edt_data->tida_edt_win);
						SEND_EVENT(tida_data->tida_win,ENTRY_OK,0,tida_data);
					}
					else
					{
						memset(edt_data->edtbuf1, '\0', sizeof(edt_data->edtbuf1));/* initial buffer  */
						edt_data->index = 0;

						sprintf(lin2,"%02d:%02d",mfw_td_convert_to_24_hour(currclock), currclock->minute);
						strncpy(edt_data->edtbuf1,lin2,MAX_LEN_TIME);
						edtReset(edt_data->editor_handle);
						edtShow(edt_data->editor_handle);
					}
				}
				else if(edt_data->state EQ DATE_SHOW)
				{
					date_time_edt_destroy(edt_data->tida_edt_win);
					SEND_EVENT(tida_data->tida_win,DATE_ENTRY,0,tida_data);
				}                                   /* start date entering      */

				else if(edt_data->state EQ TIME_SHOW)
				{
					date_time_edt_destroy(edt_data->tida_edt_win);
					SEND_EVENT(tida_data->tida_win,TIME_ENTRY,0,tida_data);	    /* start time entering      */
				}
			break;
			case KCD_RIGHT:                                /* back to previous menu */
					date_time_edt_destroy(edt_data->tida_edt_win);
					SEND_EVENT(tida_data->tida_win,TIDA_ABORT,0,tida_data);
				break;
			case KCD_MNUUP:                                /* cursor to right  */
				if((edt_data->state EQ DATE_ENTRY) || (edt_data->state EQ TIME_ENTRY))    /* cursor to rigt           */
				{
					edt_data->index++;
					edtChar(edt_data->editor_handle,ecRight);

					if(((edt_data->state EQ DATE_ENTRY) && ((edt_data->index EQ 2)||(edt_data->index EQ 5))) || ((edt_data->state EQ TIME_ENTRY) && (edt_data->index EQ 2)))
					{
						edt_data->index++;
						edtChar(edt_data->editor_handle,ecRight);
					}

				}
				break;
			case KCD_HUP:
			case KCD_MNUDOWN:                            /* cursor to left or clear  */
				if((!(edt_data->state EQ DATE_SHOW)) && (!(edt_data->state EQ TIME_SHOW)))
				{
					if(edt_data->state EQ DATE_ENTRY)
					{
					    if(edt_data->index EQ 0)
						;
						else
							edt_data->index--;

						if((edt_data->index EQ 2)||(edt_data->index EQ 5))
						{
							if(edt_data->index EQ 0)
							;
							else
								edt_data->index--;

							edtChar(edt_data->editor_handle,ecLeft);
							edtChar(edt_data->editor_handle,ecLeft);
						}
						else
							edtChar(edt_data->editor_handle,ecLeft);
					}
					if(edt_data->state EQ TIME_ENTRY)
					{
						if(edt_data->index EQ 0)
						;
						else
							edt_data->index--;

						if(edt_data->index EQ 2)
						{
							if(edt_data->index EQ 0)
							;
							else
								edt_data->index--;

							edtChar(edt_data->editor_handle,ecLeft);
							edtChar(edt_data->editor_handle,ecLeft);
						}
						else
							edtChar(edt_data->editor_handle,ecLeft);
					}
				}
				else
 					timStart(edt_data->tim_out_handle); /* waiting for release clear  otherwise to previous screen */
				break;
			default:
				return MFW_EVENT_CONSUMED;
		}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:  check_date  

 $Description:	 check date after date entering

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int check_date (T_MFW_HND win, void * edt_dat)
{
	U8 i;
	T_tida_edt * edt_data = (T_tida_edt *)edt_dat;
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  	T_tida * tida_data = (T_tida *)win_data->user;
  	T_MFW_DATE tempdate;

	char internday[3];
	char internmonth[3];
	char internyear[5];

	char debug[20];
 	T_MFW_TIME* currclock; /*SPR 2639*/
	T_MFW_DATE* currdate;
	//T_MFW_TIME alarmTime; /*SPR 2639*/
	

	 currclock= mfw_td_get_time();/*SPR 2639*/
	currdate =mfw_td_get_date();
	
    TRACE_FUNCTION ("MmiTimeDate:check_date");

	memset(internday, '\0', sizeof(internday));/* initial string  */
	memset(internmonth, '\0', sizeof(internmonth));
	memset(internyear, '\0', sizeof(internyear));

	internday[0] = edt_data->edtbuf1[0];
	internday[1] = edt_data->edtbuf1[1];
	internmonth[0] = edt_data->edtbuf1[3];
	internmonth[1] = edt_data->edtbuf1[4];
	for(i = 0;i<4;i++)
	{
		internyear[i] = edt_data->edtbuf1[6+i];
	}

	tempdate.month = atoi(internmonth);
	if((tempdate.month > 12) || (tempdate.month < 1))
		return FALSE;

	tempdate.day = atoi(internday);
	if((tempdate.day > 30) || (tempdate.day < 1))
	{
		if(tempdate.day EQ 31)
		{
			switch(tempdate.month)
			{
					case 1:
					case 3:
					case 5:
					case 7:
					case 8:
					case 10:
					case 12:
						break;
					default:
						return FALSE;

			}
		}
		else
			return FALSE;
	}

	tempdate.year = atoi(internyear);

	if(tempdate.year < 2000)
		return FALSE;

	if((tempdate.month EQ 2) && (tempdate.day EQ 29))
	{
		if(!(tempdate.year%4) || (tempdate.year EQ 2000))
        {
			if(DateTimeEntryMode == ALARM)
			{			
				alarmDate.day = tempdate.day;
				alarmDate.month = tempdate.month;
				alarmDate.year = tempdate.year;
			}
			else
			{	
			
				currdate->day = tempdate.day;
				currdate->month = tempdate.month;
				currdate->year = tempdate.year;
				mfw_td_set_date(currdate);/*SPR 2639*/
			}
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		if(DateTimeEntryMode == ALARM)
		{
			
						
			alarmDate.day = tempdate.day;
			alarmDate.month = tempdate.month;
			alarmDate.year = tempdate.year;
			
		}
		else
		{					
			currdate->day = tempdate.day;
			currdate->month = tempdate.month;
			currdate->year = tempdate.year;
		}
		mfw_td_set_date(currdate);/*SPR 2639*/
		return TRUE;
	}
}
/*******************************************************************************

 $Function:  check_time  

 $Description:	check time after time entering

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int check_time (T_MFW_HND win, void * edt_dat)
{
	T_tida_edt * edt_data = (T_tida_edt *)edt_dat;
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
  	T_tida * tida_data = (T_tida *)win_data->user;
  	T_MFW_TIME tempclock;
  	char debug[50];
	char internmin[3];
	char internhour[3];
 	T_MFW_TIME* currclock; /*SPR 2639*/
	T_MFW_DATE* currdate;
	T_MFW_TIME_FORMAT current_format;
	//T_MFW_TIME alarmTime; /*SPR 2639*/
	
	currclock = mfw_td_get_time();/*SPR 2639*/
	currdate = mfw_td_get_date();
	
	current_format = currclock->format;
	memset(internmin, '\0', sizeof(internmin));/* initial string  */
	memset(internhour, '\0', sizeof(internhour));

	internhour[0] = edt_data->edtbuf1[0];
	internhour[1] = edt_data->edtbuf1[1];
	internmin[0] = edt_data->edtbuf1[3];
	internmin[1] = edt_data->edtbuf1[4];

	tempclock.hour = atoi(internhour);
	if(tempclock.hour > 23)
		return FALSE;

	tempclock.minute = atoi(internmin);
	if(tempclock.minute >59)
		return FALSE;
	if (DateTimeEntryMode == ALARM)
	{	
		alarmTime.hour = tempclock.hour;		/*SPR 2639*/
		alarmTime.minute = tempclock.minute;
		alarmTime.format = MFW_TIME_FORMAT_24HOUR;
		mfw_td_set_alarm(&alarmTime, &alarmDate);
	}
	else
	{	
		
	
		currclock->hour = tempclock.hour;
		currclock->minute = tempclock.minute;
		currclock->format = MFW_TIME_FORMAT_24HOUR;
		mfw_td_set_time(currclock);/*SPR 2639*/
		mfw_td_set_time_format(current_format);
	}
	
	return TRUE;
}
/*******************************************************************************

 $Function:  tida_edt_tim_out_cb  

 $Description:	 Callback function for timer for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static void tida_edt_tim_out_cb(T_MFW_EVENT event,T_MFW_TIM * t)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_tida_edt * tida_edt_data = (T_tida_edt *)win_data->user;

    T_MFW_WIN * win_tida =((T_MFW_HDR *)tida_edt_data->parent_win)->data;
    T_tida * tida_data = (T_tida *)win_tida->user; // tida main data

	date_time_edt_destroy(tida_edt_data->tida_edt_win);
	SEND_EVENT(tida_data->tida_win,TIDA_CLEAR,0,tida_data);

}
/*******************************************************************************

 $Function:  tida_edt_kbd_long_cb  

 $Description:	 keyboard long event handler for time and date

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
static int tida_edt_kbd_long_cb (MfwEvt e, MfwKbd *kc)
{
   T_MFW_HND win = mfw_parent(mfw_header());

   T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
   T_tida_edt * edt_data = (T_tida_edt *)win_data->user;//tida edt data

   T_MFW_WIN * win_tida =((T_MFW_HDR *)edt_data->parent_win)->data;
   T_tida * tida_data = (T_tida *)win_tida->user; // tida main data

   TRACE_FUNCTION("MmiTimeDate:tida_edt_kbd_long_cb");

    if ((e & KEY_CLEAR) && (e & KEY_LONG))
	{

		if((!(edt_data->state EQ DATE_SHOW)) && (!(edt_data->state EQ TIME_SHOW)))
		{

				if(edt_data->state EQ DATE_ENTRY)
				{
					date_time_edt_destroy(edt_data->tida_edt_win);
					SEND_EVENT(tida_data->tida_win,DATE_SHOW,0,tida_data);
				}
				else if(edt_data->state EQ TIME_ENTRY)
				{
					date_time_edt_destroy(edt_data->tida_edt_win);

					SEND_EVENT(tida_data->tida_win,TIME_ENTRY,0,tida_data);

				}
				return MFW_EVENT_CONSUMED;

		}
		else
		{
			timStop(edt_data->tim_out_handle);
			date_time_edt_destroy(edt_data->tida_edt_win);
			SEND_EVENT(tida_data->tida_win,TIDA_TO_IDLE,0,tida_data);
			return MFW_EVENT_CONSUMED;
		}
	}
	else
		return MFW_EVENT_CONSUMED;
}
#endif /* NEW_EDITOR */

