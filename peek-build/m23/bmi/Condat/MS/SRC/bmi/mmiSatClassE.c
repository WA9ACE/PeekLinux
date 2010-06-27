/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiSatClassE.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       13/03/03

********************************************************************************

 Description:   Contains the implementation of SAT Class E



********************************************************************************

 $History: MmiSatClassE.c

  13/03/03      Original Condat(UK) BMI version.

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

#endif /* (NEW_FRAME) */

#include "gdi.h"
#include "audio.h"

#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_aci.h"
#include "cnf_aci.h"
#include "mon_aci.h"

#include "tok.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mfw_mfw.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_lng.h"
#include "mfw_win.h"
#include "mfw_icn.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif /* NEW_EDITOR */
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sms.h"

#include "dspl.h"


#include "p_mmi.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "mfw_mme.h"

#include "MmiMmi.h"

#include "Mmiicons.h"
#include "MmiPins.h"
#include "MmiMain.h"
#include "MmiDialogs.h"
#include "MmiIdle.h"
#include "MmiStart.h"
#include "MmiCall.h"
#include "MmiSimToolkit.h"

#include "MmiLists.h"
#include "MmiSounds.h"
#include "MmiResources.h"
#include "MmiTimeDate.h"
#include "mmiSat_i.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif /* NEW_EDITOR */
#include "MmiMenu.h" // SH - 20/11/01 - included to provide symbol SimMenuFunc
#include "mmiSmsRead.h" /*MC SPR 940/2*/

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"

#include "mmiSatClassE.h"
#include "mmiColours.h"

typedef struct
{
    T_MMI_CONTROL mmi_control;
    T_MFW_HND     parent_win;
    T_MFW_HND     win;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
    char * alpha_id;
} T_sat_open_channel;


/* Local function prototypes */
static void sat_open_channel_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void sat_class_e_dialog_destroy(T_MFW_HND win);
static void sat_class_e_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);

/*******************************************************************************

 $Function:    	sat_class_e_create

 $Description:	      Creation of an instance for the SAT Class E dialog.

 $Returns:		mfw window handler 

 $Arguments:        parent_window - Parent window handler
 
*******************************************************************************/
T_MFW_HND sat_class_e_create (T_MFW_HND parent_window)
{
    T_sat_open_channel * data = (T_sat_open_channel *)ALLOC_MEMORY (sizeof (T_sat_open_channel));
    T_MFW_WIN       * win;

    TRACE_FUNCTION("sat_open_channel_create()");

    if (data != NULL)
    {
        data->win = win_create (parent_window, 0, 0,NULL);
    }
    else
    {
        TRACE_ERROR("Error: Cannot allocate memory for data!");
 	 return NULL;  /* x0039928 - Lint warning fix */
    }

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_class_e_exec;
    data->mmi_control.data     = data;
    data->parent_win             = parent_window;
    win                                 = ((T_MFW_HDR *)data->win)->data;
    win->user                        = (MfwUserDataPtr)data;

    /*
     * return window handle
     */
    winShow(data->win);
    return data->win;
}

/*******************************************************************************

 $Function:     sat_open_channel_cb

 $Description:  Callback for accept/reject dialog.
 $Returns:       None.

 $Arguments:  win: current window
                    identifier: unique id
                    reason: event id

*******************************************************************************/
static void sat_open_channel_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    TRACE_FUNCTION("sat_open_channel_cb()");
    
    if (win EQ NULL)
    {
        return;
    }
    
    switch (reason)
    {
    case INFO_KCD_LEFT:        /* user has accepted Open Channel request */
        satAccept();

        /* Destroy the dialog */
        sat_class_e_dialog_destroy(win);
        break;
    case INFO_KCD_RIGHT:        /* user has rejected Open Channel request */
        satReject();

        /* Destroy the dialog */
        sat_class_e_dialog_destroy(win);
        break;
    default:
        break;
    }
}

/*******************************************************************************

 $Function:     sat_class_e_dialog_destroy

 $Description:  Close the alert dialog

 $Returns:      None.

 $Arguments:  alert_dialog: window handle.

*******************************************************************************/
static void sat_class_e_dialog_destroy(T_MFW_HND win)
{

    T_MFW_WIN       * win_data;  
    T_sat_open_channel * data;
    
    TRACE_FUNCTION ("sat_class_e_dialog_destroy()");

    if (win == NULL) 
    {
       TRACE_ERROR("ERROR: win is NULL!");
       return;
    }

    win_data = ((T_MFW_HDR *)win)->data;

    if (win_data == NULL)
    {
        TRACE_ERROR("ERROR: win_data is NULL!");
        return;
    }

    data = (T_sat_open_channel *)win_data->user;
    
    if (win != NULL)
    {
        if (data)
        {
            /*
             * Delete WIN Handler
             */
            win_delete (data->win);
            /*
             * Free Memory
             */
            sat_destroy_TEXT_ASCIIZ (data->alpha_id);
            FREE_MEMORY ((U8 *)data, sizeof (T_sat_open_channel));
        }
    }
}

/*******************************************************************************

 $Function:     sat_class_e_cb

 $Description:  Callback for Sat Class E dialogs.
 $Returns:       None.

 $Arguments:  win: current window
                    identifier: unique id
                    reason: event id

*******************************************************************************/
static void sat_class_e_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    TRACE_FUNCTION("sat_class_e_cb()");
    
    if (win EQ NULL)
    {
        return;
    }
    
    switch (reason)
    {
    case INFO_KCD_LEFT:
    case INFO_KCD_RIGHT:
    case INFO_TIMEOUT:

        /* Destroy the dialog */
        sat_class_e_dialog_destroy(win);
        break;
    default:
        break;
    }
}

/*******************************************************************************

 $Function:    	sat_class_e_exec

 $Description:	      Dialog function for Sat Class E window.

 $Returns:		none

 $Arguments:	      win - current window
 				event - window event
 				value - unique id
 				call_setup_parameter - call setup info

 SPR#2321 - DS - Corrected misleading comments.
 
*******************************************************************************/
void sat_class_e_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command)
{
    
    T_MFW_WIN       * win_data;  
    T_sat_open_channel * data;
    char* text_string;
    
    T_DISPLAY_DATA    display_info;
    
    TRACE_FUNCTION ("sat_class_e_exec()");

    if (win == NULL) 
    {
       TRACE_ERROR("ERROR: win is NULL!");
       return;
    }

    win_data = ((T_MFW_HDR *)win)->data;

    if (win_data == NULL)
    {
        TRACE_ERROR("ERROR: win_data is NULL!");
        return;
    }

    data = (T_sat_open_channel *)win_data->user;

    if (data == NULL)
    {
        TRACE_ERROR("ERROR: data is NULL!");
        return;
    }
    
    switch (event)
    {
        case SAT_OPEN_CHANNEL:

            TRACE_EVENT("SAT_OPEN_CHANNEL");
                
            /* Start an info screen to accept or reject the Open Channel request */
            data->sat_command = sat_command;	

            /* Extract the data from the Open Channel sat command */
            if (sat_command != NULL)
            {

		  TRACE_EVENT_P2("dcs: %x, len: %d", sat_command->c.open.alpha_id.code, sat_command->c.open.alpha_id.len);
		  
                /* Alpha identifier */

                /* Check alpha identifier has valid length */
                if ( (sat_command->c.open.alpha_id.len > 0) &&
                     (sat_command->c.open.alpha_id.len != (U8)-1) )
                {
                    /* Extract alpha_id data and create string */
                    data->alpha_id = sat_create_TEXT_ASCIIZ(&sat_command->c.open.alpha_id);
                }
                else
                {
                    data->alpha_id = NULL;
                }
            }
	     else
	     {
	     	     TRACE_ERROR("ERROR: NULL sat_command!");
	     }

            if (data->alpha_id)
            {
                text_string = data->alpha_id;
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialog displays them correctly*/
		   sat_add_unicode_tag_if_needed(text_string);
#endif               
            }
            else
            {
                text_string = "Open Channel"; //Need to implement for Chinese
            }
                    
            
            dlg_initDisplayData_TextStr( &display_info, TxtAccept, TxtReject, text_string, NULL, COLOUR_STATUS);
            dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_open_channel_cb, FOREVER, KEY_LEFT|KEY_RIGHT );

            display_info.Identifier   = event;
            
            /*
            * Call Info Screen
            */
            info_dialog (win, &display_info);
                
            break;
        
        case SAT_CLOSE_CHANNEL:

            TRACE_EVENT("SAT_CLOSE_CHANNEL");
                
            /* Start an info screen to inform the user that the SIM is closing the open channel*/
            data->sat_command = sat_command;

            /* Extract the data from the Close Channel sat command */
            if (sat_command != NULL)
            {
                TRACE_EVENT_P2("dcs: %x, len: %d", sat_command->c.close.alpha_id.code, sat_command->c.close.alpha_id.len);
		  
                /* Alpha identifier */

                /* SPR#2321 - DS - Check alpha identifier has valid length */
                if ( (sat_command->c.close.alpha_id.len > 0) &&
                     (sat_command->c.close.alpha_id.len != (U8)-1) )
                {
                    /* Extract alpha_id data and create string */
                    data->alpha_id = sat_create_TEXT_ASCIIZ(&sat_command->c.close.alpha_id);
                }
                else
                {
                    data->alpha_id = NULL;
                }
            }

            if (data->alpha_id)
            {
                text_string = data->alpha_id;
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialog displays them correctly*/
		   sat_add_unicode_tag_if_needed(text_string);
#endif               
            }
            else
            {
                text_string = "Closing Channel"; //Need to implement for Chinese
            }
                    
            
            dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, text_string, NULL, COLOUR_STATUS);
            dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_class_e_cb, THREE_SECS, KEY_LEFT|KEY_RIGHT );

            display_info.Identifier   = event;
            
            /*
            * Call Info Screen
            */
            info_dialog (win, &display_info);
                
            break;

        case SAT_SEND_DATA:

	     TRACE_EVENT("SAT_SEND_DATA");
                
            /* Start an info screen to inform the user that the SIM is sending data */
            data->sat_command = sat_command;

            /* Extract the data from the Send Data sat command */
            if (sat_command != NULL)
            {
                TRACE_EVENT_P2("dcs: %x, len: %d", sat_command->c.send.alpha_id.code, sat_command->c.send.alpha_id.len);
		  
                /* Alpha identifier */

                /* SPR#2321 - DS - Check alpha identifier has valid length */
                if ( (sat_command->c.send.alpha_id.len > 0) &&
                     (sat_command->c.send.alpha_id.len != (U8)-1) )
                {
                    /* Extract alpha_id data and create string */
                    data->alpha_id = sat_create_TEXT_ASCIIZ(&sat_command->c.send.alpha_id);
                }
                else
                {
                    data->alpha_id = NULL;
                }
            }

            if (data->alpha_id)
            {
                text_string = data->alpha_id;
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialog displays them correctly*/
		   sat_add_unicode_tag_if_needed(text_string);
#endif               
            }
            else
            {
                text_string = "Sending Data"; //Need to implement for Chinese
            }                   
            
            dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, text_string, NULL, COLOUR_STATUS);
            dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_class_e_cb, THREE_SECS, KEY_LEFT|KEY_RIGHT );

            display_info.Identifier   = event;
            
            /*
            * Call Info Screen
            */
            info_dialog (win, &display_info);
                
            break;

        case SAT_RECEIVE_DATA:

	     TRACE_EVENT("SAT_RECEIVE_DATA");

            /* Start an info screen to inform the user that the SIM is receiving data */
            data->sat_command = sat_command;

            /* Extract the data from the Receive Data sat command */
            if (sat_command != NULL)
            {
                TRACE_EVENT_P2("dcs: %x, len: %d", sat_command->c.receive.alpha_id.code, sat_command->c.receive.alpha_id.len);
		  
                /* Alpha identifier */

                /* SPR#2321 - DS - Check alpha identifier has valid length */
                if ( (sat_command->c.receive.alpha_id.len > 0) &&
                     (sat_command->c.receive.alpha_id.len != (U8)-1) )
                {
                    /* Extract alpha_id data and create string */
                    data->alpha_id = sat_create_TEXT_ASCIIZ(&sat_command->c.receive.alpha_id);
                }
                else
                {
                    data->alpha_id = NULL;
                }
            }

            if (data->alpha_id)
            {
                text_string = data->alpha_id;
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialog displays them correctly*/
		   sat_add_unicode_tag_if_needed(text_string);
#endif               
            }
            else
            {
                text_string = "Receiving Data"; //Need to implement for Chinese
            }                   
            
            dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, text_string, NULL, COLOUR_STATUS);
            dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)sat_class_e_cb, THREE_SECS, KEY_LEFT|KEY_RIGHT );

            display_info.Identifier   = event;
            
            /*
            * Call Info Screen
            */
            info_dialog (win, &display_info);
                
            break;

        default:
            TRACE_EVENT("sat_open_channel_exec() unexpected event");
            return;
    }

}

/* EOF - End Of File */
