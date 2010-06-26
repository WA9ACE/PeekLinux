/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiTimers.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description: This handles the storage of call times.


********************************************************************************

 $History: MmiTimers.c

  25/10/00      Original Condat(UK) BMI version.
  16/06/03      SPR#2142 - SH - Merged in MC's fix from 1.6 branch

 $End

*******************************************************************************/
/*******************************************************************************

                                Include files

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

#include "mfw_sys.h"
#include "prim.h"

#include "vsi.h"

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

#include "dspl.h"

#include "gdi.h" //ES!!

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
#include "MmiSounds.h"
#include "audio.h"
#include "cus_aci.h"

#include "MmiTimers.h"
#include "mfw_ffs.h"

#include "mmiColours.h"

/*******************************************************************************

                                Local Function Prototypes

*******************************************************************************/
/*SPR 2142 - removed timer_create() and timer_destroy() prototypes*/
unsigned long getLastCallDuration(void);  // RAVI
void getTimerString(unsigned long time, char* timeString);
void setIncomingCallsDuration(long call_time);
unsigned long getIncomingCallsDuration( void );
void setOutgoingCallsDuration(long call_time);
unsigned long getOutgoingCallsDuration(void);  // RAVI
static int timer_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static int timer_reset_OK_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
void reset_timer_OK(T_MFW_HND parent, UBYTE Identifier);
void reset_timer(T_MFW_HND parent);

/*******************************************************************************

                                Local Data

*******************************************************************************/
static char timeString[30]; //static string for dialogs

 UBYTE timer_type;/*SPR 2142 created global variable*/

/*******************************************************************************

                               Public Methods

*******************************************************************************/

/*******************************************************************************

 $Function:  setLastCallDuration

 $Description:   writes the duration of the last call to PCM and adds that duration to the
        outgoing or incoming call total.

 $Returns:    none.

 $Arguments:  The call time and the call's direction (incoming or outgoing)

*******************************************************************************/
void setLastCallDuration(long call_time, T_MFW_CM_CALL_DIR call_direction)
{

  TRACE_FUNCTION("setLastCallDuration()");

  FFS_flashData.last_call_duration = call_time;



    if (call_direction == MFW_CM_MTC)
      setIncomingCallsDuration(call_time);
    if (call_direction == MFW_CM_MOC)
      setOutgoingCallsDuration(call_time);

  return;
}

/*******************************************************************************

 $Function:   timerLastCall

 $Description:  handles user selction of "Last Call" in Call Timers Menu.
 $Returns:    status int

 $Arguments:  menu and menu item (neither used)

*******************************************************************************/
int timerLastCall(MfwMnu* m, MfwMnuItem* i)
{ T_MFW_HND       parent        = mfwParent( mfw_header() );

    T_DISPLAY_DATA display_info;


  memset(timeString,'\0',sizeof(timeString));

  timer_type = LAST_CALL;

  TRACE_FUNCTION("timerLastCall()");
    getTimerString((int)getLastCallDuration(), (char*)timeString);

	dlg_initDisplayData_TextStr( &display_info, TxtSoftOK, TxtReset, timeString,  NULL, COLOUR_STATUS_OKRESET);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) timer_cb, SIX_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */
    
    info_dialog( parent, &display_info );


    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:   timerIncomingCalls

 $Description:  handles user selction of "Incoming Calls" in Call Timers Menu.
 $Returns:    status int

 $Arguments:  menu and menu item (neither used)

*******************************************************************************/
int timerIncomingCalls(MfwMnu* m, MfwMnuItem* i)
{ T_MFW_HND       parent        = mfwParent( mfw_header() );
    T_DISPLAY_DATA display_info;

    //clear the editor-buffer before

    timer_type = INCOMING_CALLS;
  TRACE_FUNCTION("timerLastCall()");
    getTimerString((int)getIncomingCallsDuration(), (char*)timeString);

	dlg_initDisplayData_TextStr( &display_info, TxtSoftOK, TxtReset, timeString,  NULL, COLOUR_STATUS_OKRESET);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) timer_cb, SIX_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */
    info_dialog( parent, &display_info );


    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:   timerOutgoingCalls

 $Description:  handles user selction of "Outgoing Calls" in Call Timers Menu.
 $Returns:    status int

 $Arguments:  menu and menu item (neither used)

*******************************************************************************/
int timerOutgoingCalls(MfwMnu* m, MfwMnuItem* i)
{ T_MFW_HND       parent        = mfwParent( mfw_header() );

    T_DISPLAY_DATA display_info;

    //clear the editor-buffer before
  memset(timeString,'\0',sizeof(timeString));

  timer_type = OUTGOING_CALLS;

  TRACE_EVENT("timerLastCall()");
    getTimerString((int)getOutgoingCallsDuration(), (char*)timeString);

	dlg_initDisplayData_TextStr( &display_info, TxtSoftOK, TxtReset, timeString,  NULL, COLOUR_STATUS_OKRESET);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) timer_cb, SIX_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */
    info_dialog( parent, &display_info );


    return MFW_EVENT_CONSUMED;

}





/*******************************************************************************

 $Function:     timer_cb

 $Description:  Callback func for the info display screen

 $Returns:    status int

 $Arguments:  Window, Identifier (which timer), Reason (which key pressed)

*******************************************************************************/
static int timer_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{

  switch (Reason)
  {
    case INFO_KCD_LEFT: 

    	break;

    case INFO_KCD_RIGHT: 
    	reset_timer_OK(win, timer_type);
    	break;
  }

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:   setIncomingCallsDuration

 $Description:  Adds call time to Incoming Calls total
 $Returns:    none

 $Arguments:  call time

*******************************************************************************/

void setIncomingCallsDuration(long call_time)
{
  TRACE_FUNCTION("setIncomingCallsDuration()");

    //add call time to existing call time
    call_time = call_time + getIncomingCallsDuration();
    //save new value to flash
    FFS_flashData.incoming_calls_duration = call_time;
    flash_write();

  return;
}

/*******************************************************************************

 $Function:   setOutgoingCalls

 $Description:  Adds call time to Outgoing Calls total
 $Returns:    none

 $Arguments:  call time

*******************************************************************************/
void setOutgoingCallsDuration(long call_time)
{

  TRACE_FUNCTION("setOutgoingCallsDuration()");
  //add call time to current total
    call_time = call_time + getOutgoingCallsDuration();
    FFS_flashData.outgoing_calls_duration = call_time;
    //write to flash
    flash_write();


  return;

}


/*******************************************************************************

 $Function:   reset_timer_OK

 $Description:  displays "Are you sure you want to reset this timer" window
 $Returns:    none

 $Arguments:  parent window, identifier (which timer)

*******************************************************************************/
void reset_timer_OK(T_MFW_HND parent, UBYTE identifier)
{
  T_DISPLAY_DATA display_info;


	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtCancel, TxtPressOk, TxtReset, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) timer_reset_OK_cb, FOUR_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
    display_info.Identifier = identifier;

    info_dialog( parent, &display_info );
}


/*******************************************************************************

 $Function:   reset_timer_OK_cb

 $Description:  Callback function for "OK to Delete timer" window
 $Returns:    status int

 $Arguments:  window, identifier (which timer), Reason (which key pressed)

*******************************************************************************/
static int timer_reset_OK_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{

  switch (Reason)
  {
    case INFO_KCD_LEFT:
	    {
			switch (Identifier)
			{ 
		      	case LAST_CALL: 
		      		setLastCallDuration(0, MFW_CM_INV_DIR);
		      		reset_timer(win);
		      		/*SPR 2142, removed window destruction*/
		      		break;
		        case INCOMING_CALLS: 
		        	resetIncomingCallsDuration();
		        	reset_timer(win);
		        	/*SPR 2142, removed window destruction*/
		        	break;
		        case OUTGOING_CALLS: 
		        	resetOutgoingCallsDuration();
		        	reset_timer(win);
		        	/*SPR 2142, removed window destruction*/
		        	break;
			}
	    }
    	break;

    case INFO_KCD_RIGHT: 
    	{
    		/*SPR 2142, removed window destruction*/
    	}
    	break;
  }

  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:   reset_timer

 $Description:  Dsiplays confirmation that timer has been reset.
 $Returns:    none

 $Arguments:  parent window

*******************************************************************************/
void reset_timer(T_MFW_HND parent)
{
  T_DISPLAY_DATA display_info;

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTimerReset, TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    info_dialog( parent, &display_info );
}

/*******************************************************************************

 $Function:   resetIncomingCallsDuration

 $Description:  Resets incoming call total to 0
 $Returns:    nothing

 $Arguments:  none

*******************************************************************************/
 void resetIncomingCallsDuration( void )
{
  TRACE_FUNCTION("resetIncomingCallsDuration()");
  FFS_flashData.incoming_calls_duration =0;
  flash_write();

 }


/*******************************************************************************

 $Function:   resetOutgoingCallsDuration

 $Description:  Resets outgoing call total to 0
 $Returns:    nothing

 $Arguments:  none

*******************************************************************************/
  void resetOutgoingCallsDuration( void )
{

  TRACE_FUNCTION("resetOutgoingCallsDuration()");

    FFS_flashData.outgoing_calls_duration = 0;
    flash_write();

  return;
}

/*******************************************************************************

 $Function:   getLastCallDuration

 $Description:  gets the length of the last call made/received from PCM storage
 $Returns:    length of call (long)

 $Arguments:  none

*******************************************************************************/
unsigned long getLastCallDuration()
{
 
    return FFS_flashData.last_call_duration; /* x0039928 - Lint warning fix */
}

/*******************************************************************************

 $Function:   getIncomingCallsDuration

 $Description:  gets the total length of incoming calls received from PCM storage
 $Returns:    length of calls (long)

 $Arguments:  none

*******************************************************************************/
unsigned long getIncomingCallsDuration()
{
  
    return FFS_flashData.incoming_calls_duration;
}

/*******************************************************************************

 $Function:   getOutgoingCallsDuration

 $Description:  gets the total length of incoming calls received from PCM storage
 $Returns:    length of calls (long)

 $Arguments:  none

*******************************************************************************/
unsigned long getOutgoingCallsDuration()
{

    return FFS_flashData.outgoing_calls_duration;
}


/*******************************************************************************

 $Function:   getTimerString

 $Description:  Converts a time in seconds into a string denoting hours, minutes and seconds

 $Returns:    none,

 $Arguments:  time pointer to string to store time

*******************************************************************************/
void getTimerString(unsigned long time, char* timeString)
{
//  char* debug_buf;  // RAVI
  int hour,min,sec;
  TRACE_FUNCTION("getTimerString()");
  hour = time/3600;
  min = (time-hour*3600)/60;
  sec = (time-hour*3600-min*60);
  //this prevents the time being too wide for the LCD display
  if (hour > 999999)
    hour = 999999;
  sprintf(timeString,"%02d:%02d:%02d",hour,min,sec);

}

