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

	July 26, 2005    REF: SPR 29520   x0018858
	Issue: 27.22.4.5 Proactive SIM Command: PLAY TONE fails 
	Solution: The window which used to come up "Please wait" was not getting destroyed.
	Care has been taken to check this.

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

#include "gdi.h"
#include "audio.h"
#include "mmiSat_i.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

static int sat_play_tone_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc);
static void sat_info_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);


/*********************************************************************
 *
 * SUB WINDOW  SAT_PLAY_TONE
 *
 *********************************************************************/
/*
 * Tone Definitions
 */
#define   SAT_TONE_DIAL              1 
#define   SAT_TONE_CALL_SUB_BUSY     2
#define   SAT_TONE_CONGESTION        3
#define   SAT_TONE_RADIO_PATH_ACK    4
#define   SAT_TONE_RADIO_PATH_NOT    5
#define   SAT_TONE_ERROR             6 
#define   SAT_TONE_CALL_WAITING      7
#define   SAT_TONE_RINGING_TONE      8
#define   SAT_TONE_GENERAL_BEEP      0x10
#define   SAT_TONE_POSITIV_ACK       0x11
#define   SAT_TONE_NEGATIV_ACK       0x12

typedef struct
{
    T_MMI_CONTROL mmi_control;
    T_MFW_HND     parent_win;
    T_MFW_HND     win;
    T_MFW_HND     tim;
    T_MFW_HND     info;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
    T_MFW_HND		kbd;  /* sbh - keyboard handler, so window can be destroyed by user */
     char         *TextString;
     UBYTE         sound_id; /* sound to play */
    UBYTE         device_id; /* device to play sound on */
    BYTE          volume; /* volume to play */
} T_sat_play_tone;

typedef enum {
    NOT_SUPPORTED,
    SINGLE_TONE,
    DURATION
} e_TONE_DURATION;


static e_TONE_DURATION sat_set_selected_sound  (SatTone tone_tag, T_sat_play_tone * data);
static void sat_play_tone_destroy (T_MFW_HND own_window);
static void sat_play_tone_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command);
static ULONG sat_calculate_time (UBYTE unit, UBYTE value);

/*******************************************************************************

 $Function:    	sat_play_tone_create

 $Description:	Creation of an instance for the SAT PLAY TONE dialog. Window must
 				be available after reception of SAT command, only one instance.
 
 $Returns:		window handle

 $Arguments:	own_window - window handler
 
*******************************************************************************/

T_MFW_HND sat_play_tone_create (T_MFW_HND parent_window)
{
    T_sat_play_tone * data = (T_sat_play_tone *)ALLOC_MEMORY (sizeof (T_sat_play_tone));
    T_MFW_WIN       * win;

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb); // c030 rsa

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_play_tone_exec;
    data->mmi_control.data     = data;
    data->parent_win           = parent_window;
    win                        = ((T_MFW_HDR *)data->win)->data;
    win->user                  = (MfwUserDataPtr)data;

    data->kbd      = kbdCreate( data->win, KEY_ALL, (T_MFW_CB) sat_kbd_cb); /* sbh - add keyboard handler */
    	
    /*
     * return window handle
     */
    win_show(data->win);
    return data->win;
}

/*******************************************************************************

 $Function:    	sat_play_tone_destroy

 $Description:	Destroy the sat play tone dialog.

 
 $Returns:		none

 $Arguments:	own_window - window handler
 
*******************************************************************************/
static void sat_play_tone_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN * win_data   = ((T_MFW_HDR *)own_window)->data;
    T_sat_play_tone * data = (T_sat_play_tone *)win_data->user;

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : sat_play_tone_destroy called with NULL Pointer");
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
            sat_destroy_TEXT_ASCIIZ (data->TextString);
            FREE_MEMORY ((U8 *)data, sizeof (T_sat_play_tone));
        }
}

/*******************************************************************************

 $Function:    	sat_play_tone_exec

 $Description:	Dialog function for sat_play_tone_exec window.

 $Returns:		none

 $Arguments:	win - window handler
 				event -window event
 				value - unique id
 				sat_command - Sat Command.
 
*******************************************************************************/
static void sat_play_tone_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command)
{
	
    T_MFW_WIN       * win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_play_tone * data       = (T_sat_play_tone *)win_data->user;
	
    T_SAT_TXT       * txt;
    ULONG             time = 0; /* x0039928 - Lint warning fix */
    e_TONE_DURATION   res;
    T_SAT_RES sat_res;
	
    T_DISPLAY_DATA    display_info;

    TRACE_FUNCTION ("sat_play_tone_exec()");
	
    switch (event)
	{
	case SAT_PLAY_TONE:
		TRACE_EVENT("SAT_PLAY_TONE");
		data->sat_command = sat_command; /* save a pointer to the parameter for later use in callbacks */
		data->info = NULL; /* NULL means no info running */
		data->tim = NULL;  /* NULL means no timer running */

		/*
		* --> Select the right Sound ID, if not supported: return
		*/

		res = sat_set_selected_sound(sat_command->c.tone, data);

		switch (res)
		{
		case NOT_SUPPORTED:
			/* sound not supported */
                        TRACE_EVENT("SAT sound not supported");
			sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
			sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
			sat_done (sat_command, sat_res);
			sat_play_tone_destroy (data->win);
			return;
		case SINGLE_TONE:
			/* ignore duration for single tones */
			TRACE_EVENT("SINGLE_TONE");
			audio_PlaySoundID(data->device_id, data->sound_id , data->volume, AUDIO_PLAY_ONCE);
			break;
		case DURATION:
		      TRACE_EVENT("DURATION");
			time = sat_calculate_time (sat_command->c.tone.durUnit, sat_command->c.tone.durValue);
			if (time EQ 0)
			{
				/* use default tone duration */
				audio_PlaySoundID(data->device_id, data->sound_id , data->volume, AUDIO_PLAY_ONCE);
				res = SINGLE_TONE; /* overruled by default */
			}
			else
			{
				/* to be stopped by timer */
				data->tim = tim_create (win, time, (T_MFW_CB)sat_play_tone_tim_cb);
				tim_start (data->tim); 
				audio_PlaySoundID(data->device_id, data->sound_id , data->volume, AUDIO_PLAY_INFINITE);
			}
			break;
		}

		/*
		* --> display alpha identifier (if any)
		*/
		txt = &sat_command->c.tone.alpha;

		/*SPR#2340 - DS - Handle cases where no alpha id and/or duration is present */
		
		if (txt->len > 0) /* alpha id supplied in command */
		{
		   int timer=0;

		   if (time EQ 0)  /* no duration supplied, destroy info_dialog after timeout */
        	         timer = TEN_SECS;
		   else /* duration supplied, info_dialog destroyed when audio timer expires */
        		   timer = FOREVER;
		   
                data->TextString = sat_create_TEXT_ASCIIZ (txt);

//July 26, 2005    REF: SPR 29520   x0018858
//Function call modified to pass the string as a differant argument(initially 4th arg but now as the 5th arg).
                dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtCancel, NULL, data->TextString, COLOUR_STATUS);
			   
                dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_info_cb, timer, KEY_LEFT | KEY_RIGHT | KEY_HUP);
                display_info.Identifier   = event;
//July 26, 2005    REF: SPR 29520   x0018858
//wrapping enabled for the display.
		display_info.WrapStrings=WRAP_STRING_2;
				
                /*
                * Call Info Screen
                */
                data->info = info_dialog (win, &display_info);
                /* destroy when dialog times out */ 
                display_info.TextString   = data->TextString;
		}
		else if (txt->text EQ 0 && res NEQ SINGLE_TONE)
            {
                /* no alpha id supplied - show default text */
                int timer=0;

                if (time EQ 0)  /* no duration supplied, destroy info_dialog after timeout */
                         timer = TEN_SECS;
                else /* duration supplied, info_dialog destroyed when audio timer expires */
                	   timer = FOREVER;

                data->TextString = sat_create_TEXT_ASCIIZ (txt);
                dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtCancel, "Playing tone", NULL, COLOUR_STATUS);
                dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_info_cb, timer, KEY_LEFT | KEY_RIGHT | KEY_HUP);
			display_info.Identifier   = event;
            /*
             * Call Info Screen
             */
            data->info = info_dialog (win, &display_info);
            /* destroy when dialog times out */ 
                 display_info.TextString   = data->TextString;
		}
		else if (res EQ SINGLE_TONE) 
		{
			/* destroy immediately, if SINGLE_TONE and no info */
			sat_res[SAT_ERR_INDEX] = SatResSuccess;
			sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
			sat_done (sat_command, sat_res);
			sat_play_tone_destroy (win);
		}
		break;

		/* sbh - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:
		      TRACE_EVENT("SAT_DESTROY_WINDOW");
			sat_play_tone_destroy (win);
			break;
		/* ...sbh */
			
		default:
			TRACE_EVENT ("sat_play_tone_exec() unexpected event");
			break;
	}
}

/*******************************************************************************

 $Function:    	sat_play_tone_tim_cb

 $Description:	Callback function for the play tone timer.
 
 $Returns:		Execution status

 $Arguments:	event -window event
 				tc - timer info
 
*******************************************************************************/

static int sat_play_tone_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
    T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_sat_play_tone * data   = (T_sat_play_tone *)win_data->user;
    T_SAT_RES sat_res;
    
    if ((win EQ NULL) || (win_data EQ NULL) || (data EQ NULL))
        return MFW_EVENT_CONSUMED; /* we have been interrupted by user action */

    TRACE_FUNCTION("sat_play_tone_tim_cb()");
    
    /* timer elapsed */
    
    sat_res[SAT_ERR_INDEX] = SatResSuccess;
    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
    sat_done (data->sat_command, sat_res);
    data->sat_command = NULL; /* signal response issued to info */
    
    /* the last one has to destroy the window */
    data->tim = NULL; /* signal end of timer to info */

	/* API/DSM - 04/09/03 - SPR2491 - Add a call to stop the play of the continuous AUDIO sound*/
    audio_StopSoundbyID(data->device_id, data->sound_id);
	/* API/DSM - 04/09/03 - SPR2491 - END */
    

    /* SPR#2340 - DS - If timer has expired, destroy info_dialog if necessary */
    if (data->info NEQ NULL)
    {
        /* timer has expired therefore destroy info dialog */
        TRACE_EVENT("Destroy Play Tone dialog and data");
// July 26, 2005    REF: SPR 29520   x0018858
//The wndow displaying "Please wait was not getting destroyed. 
//Destroyed the window and initialized to NULL.
//Begin 29520
        //sat_play_tone_destroy(data->info);
        dialog_info_destroy(data->info);
		data->info = NULL;
//End 29520		
    }

    if (data->info EQ NULL)
    {
        /* no info window or info window timed out: we are the last */
        TRACE_EVENT("Destroy Play Tone win and data");
        sat_play_tone_destroy (win);
    }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	sat_calculate_time

 $Description:	Calculate timeout value
 
 $Returns:		time out in ms

 $Arguments:	unit - number of units
 				value - unit value.
 
*******************************************************************************/
static ULONG sat_calculate_time (UBYTE unit, UBYTE value)
{
    ULONG result = (ULONG)value;

    TRACE_FUNCTION("sat_calculate_time()");

    switch (unit)
        {
        case 0: // Minutes
            result = result * 60 * 1000;
            break;
        case 1: // Seconds
            result = result * 1000;
            break;
        case 2: // tenth of seconds
            result = result * 100;
            break;
        }
    TRACE_EVENT_P3("unit %d, value %d = result %d ms", unit, value, result);
    return result;
}

/*******************************************************************************

 $Function:    	sat_set_selected_sound

 $Description:	Choose the Sound ID´s, return appropriate duration and support info
 
 $Returns:		time out

 $Arguments:	tone_tag - tone name
 				data - tone info
 
*******************************************************************************/


static e_TONE_DURATION sat_set_selected_sound  (SatTone tone_tag, T_sat_play_tone * data)

{
    e_TONE_DURATION ret;
    
    TRACE_FUNCTION("sat_set_selected_sound()");

    /* SPR#2340 - DS - Removed code that set ret to NOT_SUPPORTED if durUnit equals zero (i.e. Minutes) */

    /* SPR#2340 - DS - Corrected sound Ids. Use defns rather than magic no.s */

    switch(tone_tag.tone)
    {
    	case   SAT_TONE_CALL_SUB_BUSY:
        	data->sound_id = TONES_BUSY /*17*/;
    		break;
    	case   SAT_TONE_CONGESTION:
        	data->sound_id = TONES_CONGEST /*18*/;
    		break;
    	case   SAT_TONE_RADIO_PATH_ACK:
    	case   SAT_TONE_GENERAL_BEEP:
            data->sound_id = TONES_ACK /*20*/;
            break;
    	case   SAT_TONE_RADIO_PATH_NOT:
        	data->sound_id = TONES_DROPPED /*19 - also call dropped*/;
    		break;
      case   SAT_TONE_DIAL:
    	case   SAT_TONE_CALL_WAITING:
        	data->sound_id = TONES_CW /*21*/;
    		break;
    	case   SAT_TONE_POSITIV_ACK:
    	case   SAT_TONE_RINGING_TONE:
        	data->sound_id = TONES_RINGING_TONE /*0x27*/;
    		break;
    	case   SAT_TONE_ERROR:
    	case   SAT_TONE_NEGATIV_ACK:
        	data->sound_id = TONES_ERROR /*16*/;
        	break;
      default:
             /* Unknown tone value - default is 'general beep' */
             data->sound_id = TONES_KEYBEEP /*0x26*/;
    		break;
    }

    ret = DURATION;
		
    TRACE_EVENT_P2("tone %d, sound_id %x", tone_tag.tone, data->sound_id);

    return ret;
}

/*******************************************************************************

 $Function:    	sat_info_cb

 $Description:	Callback function information dialog.
 
 $Returns:		none

 $Arguments:	win - window
 				identifier - unique id
 				reason - event
 
*******************************************************************************/

static void sat_info_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    /* PLAY TONE   */
    T_MFW_WIN       * play_win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_play_tone * play_data       = (T_sat_play_tone *)play_win_data->user;
    T_SAT_RES sat_res;
	
	if (win EQ NULL)
		return;

	TRACE_FUNCTION("sat_info_cb()");

	TRACE_EVENT_P2("identifier %d, reason %d", identifier, reason);

    /*
	* Who has initiated the information screen
	*/
    switch (identifier)
	{
	case SAT_PLAY_TONE:
		if ((play_win_data EQ NULL) || (play_data EQ NULL))
			return; /* we have been interrupted by sat_play_tone_tim_cb() */

		switch (reason)
		{
		case INFO_TIMEOUT:
			if (play_data->tim NEQ NULL)
			{
                        /* NOP on short info timeout && infinite: 
                        * tone will be stopped and SUCCESS will be signalled by sat_play_tone_tim_cb() 
                            */
                        play_data->info = NULL; /* signal the end of info to timer */
            } 
                        else 
                        {
                            /* no timer (single tone) or timer timed out: we are the last  */
                            if (play_data->sat_command NEQ NULL)
                            {
                                /* response not yet issued, i.e single tone with info */
                                sat_res[SAT_ERR_INDEX] = SatResSuccess; 
                                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                                sat_done (play_data->sat_command, sat_res);
                            }

                            /* the last one has to destroy the window */
                            sat_play_tone_destroy (win);
                        }
                        break;
		case INFO_KCD_LEFT:
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP: /*SPR#2340 - DS - Handle hangup keypress */
			if (play_data->tim NEQ NULL)
                        {
                            /* timer has not yet elapsed, stop it */
                            tim_stop (play_data->tim);
                        }
			
						/* API/DSM - 04/09/03 - SPR2491 - Add a call to stop the play of the continuous AUDIO sound*/
						audio_StopSoundbyID(play_data->device_id, play_data->sound_id);
						/* API/DSM - 04/09/03 - SPR2491 - END */

                        if (play_data->sat_command NEQ NULL)
                        {
                            /* response not yet issued */
                            sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
                            sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                            sat_done (play_data->sat_command, sat_res);
                        }
                        sat_play_tone_destroy (win);
			break;
		}
		break;
		
        default:
            TRACE_EVENT("sat_info_cb(): unexp. event");
            break;
	}
}
