/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    mmiSatInfo.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    Implementation of MMI SIM Application Toolkit (SAT)
                        
********************************************************************************

 $History: mmiSatInfo.c

 	Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
	Description:   CT GCF - TC27.22.4.13.1 Seq 1.10 - PROACTIVE SIM COMMANDS: 
	SET UP CALL (NORMAL) - Call Set-up Failed
	Solution:  Removed the Redial flag enabling.

 	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: In the function sat_call_setup_exec() copied the icon data in to display_info structure
	inorder to display on the call screen during SAT call setup.

	25/10/00			Original Condat(UK) BMI version.	
	21/02/03			removed all calls to function sat_add_unicode_tag_if_needed()
	   
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
#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sms.h"
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_tim.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiCall.h"

#include "mmiSat_i.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

/*********************************************************************
 *
 * SUB WINDOW  SAT_CALL_SETUP
 *
 *********************************************************************/
typedef struct
{
    T_MMI_CONTROL mmi_control;
    T_MFW_HND     parent_win;
    T_MFW_HND     win;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
    T_SAT_call_setup_parameter * call_setup_parameter;
    T_MFW_HND       redial_tim;
} T_sat_call_setup;

extern BOOL sat_call_active;   // Marcus: Issue 1812: 13/03/2003
/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/

static void sat_call_setup_destroy (T_MFW_HND own_window);
static void sat_call_setup_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_call_setup_parameter * call_setup_parameter);
static void sat_call_setup_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static int sat_call_setup_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc);

/*******************************************************************************

 $Function:    	sat_call_setup_create

 $Description:	Creation of an instance for the SAT CALL  dialog.Window must be 
 				available after reception of SAT command only one instance.

 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 
*******************************************************************************/
T_MFW_HND sat_call_setup_create (T_MFW_HND parent_window)
{
    T_sat_call_setup * data = (T_sat_call_setup *)ALLOC_MEMORY (sizeof (T_sat_call_setup));
    T_MFW_WIN       * win;

    data->win = win_create (parent_window, 0, 0,NULL);
    sat_set_call_setup_win(data->win);

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_call_setup_exec;
    data->mmi_control.data     = data;
    data->parent_win           = parent_window;
    win                        = ((T_MFW_HDR *)data->win)->data;
    win->user                  = (MfwUserDataPtr)data;

    /*
     * return window handle
     */
    winShow(data->win);
    return data->win;
}

/*******************************************************************************

 $Function:    	sat_call_setup_destroy

 $Description:	Destroy the sat call dialog.

 $Returns:		none

 $Arguments:	own_window - Current window
 
*******************************************************************************/
static void sat_call_setup_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN * win_data   = ((T_MFW_HDR *)own_window)->data;
    T_sat_call_setup * data = (T_sat_call_setup *)win_data->user;

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : sat_call_setup_destroy called with NULL Pointer");
		return;
	}

    if (data)
        {
            /*
             * Delete WIN Handler
             */
            win_delete (data->win);
            /*
             * Free Memory
             */
            FREE_MEMORY ((void *)data, sizeof (T_sat_call_setup));
            sat_set_call_setup_win(NULL);
        }
}

/*******************************************************************************

 $Function:    	sat_call_setup_exec

 $Description:	Dialog function for sat_call_setup_exec window.

 $Returns:		none

 $Arguments:	win - current window
 				event - window event
 				value - unique id
 				call_setup_parameter - call setup info
 
*******************************************************************************/
static void sat_call_setup_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_call_setup_parameter * call_setup_parameter)
{
    
    T_MFW_WIN       * win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_call_setup * data       = (T_sat_call_setup *)win_data->user;
    
    T_DISPLAY_DATA    display_info;
    
    TRACE_FUNCTION ("sat_call_setup_exec()");

    if ((win EQ NULL) || (win_data EQ NULL) || (data EQ NULL))
        return;
    
    switch (event)
    {
    case SAT_CALL_ALERT:

        TRACE_EVENT("sat_call_setup_exec(): SAT_CALL_ALERT");

        /* start an info screen to accept or reject the call setup */
        data->call_setup_parameter = call_setup_parameter;

        /* SPR#1700 - DS - Modified so SAT will not display "Setup call?" if the first alpha id has been supplied by the SIM
         */        
	
	  if (call_setup_parameter->TextString) /* Alpha id supplied by SIM */
        {        
               /* 01-06-2006, x0045876 (OMAPS00070741) */
	        dlg_initDisplayData_TextStr( &display_info, TxtAccept, TxtReject, call_setup_parameter->TextString, " ", COLOUR_STATUS);	
  //x0035544 Feb 07, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON	
	if(call_setup_parameter->IconInfo.dst != NULL)
	{		
		display_info.IconData.width 	=  call_setup_parameter->IconInfo.width;
	  	display_info.IconData.height	= call_setup_parameter->IconInfo.height;		
		display_info.IconData.dst = call_setup_parameter->IconInfo.dst;
		display_info.IconData.selfExplanatory 	= call_setup_parameter->IconInfo.selfExplanatory;		
	}
#endif

	  	}
        
        else /* No alpha id supplied so show "Setup call?" */
        {  
        //x0035544 Mar 14, 2006 DR:OMAPS00061467
        //added missing '?' to the string "Setup call" as below
       /* 01-06-2006, x0045876 (OMAPS00070741) */
	        dlg_initDisplayData_TextStr( &display_info, TxtAccept, TxtReject, "Setup call?", " ", COLOUR_STATUS);
        }
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_call_setup_cb, FOREVER, KEY_LEFT|KEY_RIGHT );


        display_info.Identifier   = event;
	 /* 01-06-2006, x0045876 (Wrapping of text is enabled) */
	 display_info.WrapStrings=WRAP_STRING_2;
        /*
        * Call Info Screen
        */
        info_dialog (win, &display_info);
         break;
        
    case SAT_CALL_REDIAL:
        /* attempt a redial if the timer has not yet elapsed */
        /* if redial is not commanded by SAT_CALL_ALERT the timer will be NULL (see sat_call_setup_cb()) */
        /* if the timer has already elapsed it is set to NULL (see sat_call_setup_tim_cb()) */
            	
    	TRACE_EVENT("sat_call_setup_exec(): SAT_CALL_REDIAL");

        if ((data->redial_tim NEQ NULL) || (call_setup_parameter->redialTime EQ FOREVER))

        {
            // ??? rsa according to ES we shall use a satAccept() at this point (but it returns an error)
            if (!call_get_window())
                call_create(0);
            SEND_EVENT(call_get_window(),CALL_OUTGOING_SAT,0,call_setup_parameter);
        }
        else
        {
            SEND_EVENT(data->parent_win, SAT_CALL_END, 0, NULL); /* inform the parent */
        }
        break;

    case SAT_CALL_END:
            	
    	TRACE_EVENT("sat_call_setup_exec(): SAT_CALL_END");

	/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
   	 -Deleted Redial flag enabling/disabling code*/

        /* clean up after end of call */
        sat_call_setup_destroy(win);
        break;

    default:
        TRACE_EVENT("sat_call_setup_exec() unexpected event");
        return;
    }

}

/*******************************************************************************

 $Function:    	sat_call_setup_cb

 $Description:	Callback function information dialog.

 $Returns:		none

 $Arguments:	win - current window
 				identifier - unique id
 				reason - window event id
 
*******************************************************************************/

static void sat_call_setup_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    T_MFW_WIN          * info_win_data = ((T_MFW_HDR *) win)->data;
    T_sat_call_setup   * data     = (T_sat_call_setup *)info_win_data->user;

    TRACE_FUNCTION("sat_call_setup_cb()");
    
    if ((win EQ NULL) || (info_win_data EQ NULL) || (data EQ NULL))
        return;
    
    switch (reason)
    {
    case INFO_KCD_LEFT:        /* the user has accepted the call setup */
        /* start the call */
        sat_call_active = TRUE;   // Marcus: Issue 1812: 13/03/2003
        if (!call_get_window())
            call_create(0);
        SEND_EVENT(call_get_window(),CALL_OUTGOING_SAT,0,data->call_setup_parameter);
        satAccept();

        /* create and start the redial timer handler */
        if ((data->call_setup_parameter->redialTime NEQ 0) && 
            (data->call_setup_parameter->redialTime NEQ FOREVER))
        {
            data->redial_tim      = 
                tim_create (win, data->call_setup_parameter->redialTime, (T_MFW_CB)sat_call_setup_tim_cb);
            tim_start (data->redial_tim);
        }
        else
        {
            data->redial_tim      = NULL; /* timer not used */
        }
        
        /* destroying will be done in response to SAT_CALL_END */
        SEND_EVENT(data->parent_win, SAT_CALL_END, 0, NULL);   // Marcus: Issue 1812: 13/03/2003
        break;
    case INFO_KCD_RIGHT:        /* the user has rejected the call setup */
        TRACE_EVENT("sat_call_setup_cb(): User rejected call setup");
        satReject();
        data->redial_tim = NULL; /* timer not used */
        SEND_EVENT(data->parent_win, SAT_CALL_END, 0, NULL); /* inform the parent who will take care of destroying */
        break;
    default:
        break;
    }
}

/*******************************************************************************

 $Function:    	sat_call_setup_tim_cb

 $Description:	Callback function for the redial timer.

 $Returns:		Execution status

 $Arguments:	event - window event
 				tc - timer info
 
*******************************************************************************/
static int sat_call_setup_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
    T_MFW_HND          win      = mfw_parent (mfw_header());
    T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;
    T_sat_call_setup * data     = (T_sat_call_setup *)win_data->user;

    if ((win EQ NULL) || (win_data EQ NULL) || (data EQ NULL))
	return MFW_EVENT_CONSUMED;

    data->redial_tim = NULL; /* timer has elapsed */
    return MFW_EVENT_CONSUMED;
}
