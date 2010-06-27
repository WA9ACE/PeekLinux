/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    mmiSatInput.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    Implementation of MMI SIM Application Toolkit (SAT)
                        
********************************************************************************

 $History: mmiSatInput.c
 
       Oct 09, 2006 DR: OMAPS00092083 - x0047075 (Archana). 
       Description: CT-PTCRB[27.22.4.1.5]-MS  Resets while Icon Display
       Solution : If the sat icon is present which is self explanatory then editor buffer is intialized to blank.

       Feb 02, 2006 DR: OMAPS00061468 - x0035544.
       Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
       Solution : SAT SET UP IDLE MODE TEXT (Icon) support added.
       
	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

 	Nov 09, 2005 REF: OMAPS00043190 - x0018858
	Description: PTCRB - 27.22.4.1.4 (DISPLAY TEXT (Sustained text) Fails in 1900 DISPLAY TEXT
			    (Sustained text) 
	Solution: Have added a new global variable to store the handle of the window and then release
			it when a higher priority call event comes in. 

    a0876501 (Shashi Shekar B.S.) 03-Oct-2005 MMI-SPR-34554:
    Terminal Response for Get Inkey (Yes/No) response correctly sent now.

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
#include "mfw_ss.h"
#include "mfw_str.h"

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
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "mmiColours.h"
#else
#include "MmiEditor.h"
#endif
#include "psa_util.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
static void sat_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT event);
#else /* NEW_EDITOR */
static void sat_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event);
#endif /* NEW_EDITOR */

#ifdef EASY_TEXT_ENABLED 
#include "MmiChineseInput.h" /* SPR#1700 - DS - Added to implement Chinese Editor support in SAT */
#endif

#define TRACE_SAT_STRING /* SPR#1700 - DS - Debug flag for tracing SAT String */

/*********************************************************************
 *
 * SUB WINDOW  SAT_GET_KEY
 *
 *********************************************************************/
#define SAT_GET_KEY_BUFFER_SIZE (UBYTE)(1+2) /* two spares for mfwEdt */

typedef struct
{
    T_MMI_CONTROL  mmi_control;
    T_MFW_HND      win;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA  editor_data;
#endif
    char          *edt_buffer; /* buffer for editor */ 
     T_MFW_HND		kbd;  /* sbh - keyboard handler, so window can be destroyed by user */
     T_MFW_HND           win_editor; /* SPR#2490 - DS - Store editor window */ 
} T_sat_get_key;

#if (BOARD==61)
// ADDED BY RAVI - 29-11-2005 - OMAPS00042166
extern void sat_add_unicode_tag_if_needed(char* string);
//END RAVI - 29-11-2005
#endif

static void sat_get_key_destroy (T_MFW_HND own_window);
static void sat_get_key_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command);

/*******************************************************************************

 $Function:    	sat_get_key_create

 $Description:	Creation of an instance for the SAT GET KEY dialog.Window must 
 				be available after reception of SAT command,only one instance.

 
 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 
*******************************************************************************/

T_MFW_HND sat_get_key_create (T_MFW_HND parent_window)
{
    T_sat_get_key * data = (T_sat_get_key *)ALLOC_MEMORY (sizeof (T_sat_get_key));
    T_MFW_WIN     * win;

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb); // c030 rsa

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_get_key_exec;
    data->mmi_control.data     = data;
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

 $Function:    	sat_get_key_destroy

 $Description:	Destroy the sat get inkey dialog.

 
 $Returns:		none

 $Arguments:	own_window - window handler
 
*******************************************************************************/

static void sat_get_key_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN * win_data   = ((T_MFW_HDR *)own_window)->data;
    T_sat_get_key * data = (T_sat_get_key *)win_data->user;

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : sat_get_key_destroy called with NULL Pointer");
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
            FREE_MEMORY ((U8 *)data->edt_buffer,
                SAT_GET_KEY_BUFFER_SIZE * sizeof(char));

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		if (data->editor_data.editor_attr.TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)data->editor_data.editor_attr.TitleIcon.data, data->editor_data.editor_attr.TitleIcon.width * 
								data->editor_data.editor_attr.TitleIcon.height);
			data->editor_data.editor_attr.TitleIcon.data = NULL;
		}
#endif			
      	  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			sat_destroy_TEXT_ASCIIZ ((char *)data->editor_data.TitleString); /* label */

                    if (data->win_editor)
                    {
                        AUI_edit_Destroy(data->win_editor); /* SPR#2490 - DS - Destroy editor */
                        data->win_editor = NULL;
                    }
#else /* NEW_EDITOR */
            sat_destroy_TEXT_ASCIIZ (data->editor_data.TextString); /* label */
#endif /* NEW_EDITOR */
            FREE_MEMORY ((U8 *)data, sizeof (T_sat_get_key));
        }
}

/*******************************************************************************

 $Function:    	sat_get_key_exec

 $Description:	Dialog function for sat_get_key_exec window.

 
 $Returns:		none

 $Arguments:	win - window handler
 				event -window event
 				value - unique id
 				sat_command - Sat command info
 
*******************************************************************************/

static void sat_get_key_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command)
{
    T_MFW_WIN       * win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_get_key   * data       = (T_sat_get_key *)win_data->user;

    T_SAT_TXT       * txt;
    T_SAT_RES sat_res;

    /* SPR#1428 - SH - New Editor: we'll put data in these, for efficiency */
#ifdef NEW_EDITOR
	USHORT leftSK, rightSK, mode, cursor;
	UBYTE *textStr;
#endif

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
#endif

    TRACE_FUNCTION ("sat_get_key_exec()");

    switch (event)
        {
        case SAT_GET_KEY:
            data->sat_command = sat_command; /* save a pointer to the parameter for later use in callbacks */
            txt = (T_SAT_TXT *)&sat_command->c.text;

            /* allocate a buffer to hold the edited chars */
            data->edt_buffer = (char *)ALLOC_MEMORY (
                SAT_GET_KEY_BUFFER_SIZE * sizeof(char));

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			/*
             * Setup an Editor to get a single key 
             */

            AUI_edit_SetDefault(&data->editor_data);
            AUI_edit_SetDisplay(&data->editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);

            /* SPR#2490 - DS - Initialise editor window reference */
            data->win_editor = NULL;
            
            if ((sat_command->qual & SAT_M_INKEY_HELP_AVAIL) NEQ 0)
                {
                    leftSK  = TxtHelp; /* help available */
                }
            else
                {
                    leftSK  = TxtNull; /* no help available */
                }
            if ((sat_command->qual & SAT_M_INKEY_YES_NO) NEQ 0)
                {
                    /* yes/no decision */
                   	leftSK	= TxtYes;
                    rightSK  = TxtNo;
                    mode	= ED_MODE_READONLY; /* treat as read-only, thus ignoring normal keys */
                    cursor	= ED_CURSOR_NONE;
                }                              
            else                               
                {
                    /* normal single key request */
                    leftSK    = TxtSoftOK;
                    rightSK   = TxtSoftBack;
                    if ((sat_command->qual & SAT_M_INKEY_ALPHA) EQ 0) /* 0=digits, 1=alpha */
                    {
                        mode = 0;		/* Digits mode */
						cursor = ED_CURSOR_UNDERLINE;
                    }
                    else
                    {
                        mode = ED_MODE_ALPHA;		/* Alphanumeric mode */
                        cursor = ED_CURSOR_BAR;
                    }
                }

            if (txt->len > 0)
            {
            	textStr     = (UBYTE *)sat_create_TEXT_ASCIIZ (txt); /* label */

            }
            else
            {
                textStr    =  NULL; /* no label */
            }

		 AUI_edit_SetTextStr(&data->editor_data, leftSK, rightSK, TxtNull, textStr);
		 
            AUI_edit_SetEvents(&data->editor_data, event, FALSE /*SPR#2490 - DS*/, TWENTY_SECS, (T_AUI_EDIT_CB)sat_editor_cb);
			AUI_edit_SetMode(&data->editor_data, mode, cursor);
			
            if ((sat_command->qual & SAT_M_INKEY_HELP_AVAIL) NEQ 0)
            {
            	AUI_edit_SetAltTextStr(&data->editor_data, 1, TxtHelp, FALSE, TxtNull); /* help available */
            }

            /* provide an empty zero terminated buffer */
            data->edt_buffer[0] = '\0'; 

			/* SPR#1559 - SH - Set appropriate editor size */
            AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->edt_buffer,  strlen(data->edt_buffer)+1+1 /* SPR#2321 - DS - Display key */);
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.text.icon.qual != 0xFF)
		{
			/* Copy the icon data to the editor attributes*/
			data->editor_data.editor_attr.TitleIcon.width = sat_command->c.text.iconInfo.width;  
			data->editor_data.editor_attr.TitleIcon.height = sat_command->c.text.iconInfo.height; 

			icon_length = sat_command->c.text.iconInfo.width * sat_command->c.text.iconInfo.height;
										
			data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.text.iconInfo.dst, icon_length);
			data->editor_data.editor_attr.TitleIcon.isTitle = TRUE;

			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.text.icon.qual == 0x00)
			     	{
						/* Icon is self-explanatory. Do not display the text*/
						data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
			     	}
				 
				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.text.iconInfo.dst, icon_length);
				sat_command->c.text.iconInfo.dst = NULL;
		}
#endif

            /*
             * Start the Editor
             */
            /* SPR#2490 - DS - Store editor window reference */
            data->win_editor = AUI_edit_Start(win, &data->editor_data);
            if (data->win_editor == NULL)
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
                sat_get_key_destroy (data->win);
            }
#else /* NEW_EDITOR */
            /*
             * Setup an Editor to get a single key 
             */
            if ((sat_command->qual & SAT_M_INKEY_HELP_AVAIL) NEQ 0)
                {
                    data->editor_data.AlternateLeftSoftKey  = TxtHelp; /* help available */
                }
            else
                {
                    data->editor_data.AlternateLeftSoftKey  = TxtNull; /* no help available */
                }
            if ((sat_command->qual & SAT_M_INKEY_YES_NO) NEQ 0)
                {
                    /* yes/no decision */
                    data->editor_data.LeftSoftKey    = TxtYes;
                    data->editor_data.RightSoftKey   = TxtNo;
                    data->editor_data.mode           = E_EDIT_READ_ONLY_MODE; /* treat as read-only, thus ignoring normal keys */
                }                              
            else                               
                {
                    /* normal single key request */
                    data->editor_data.LeftSoftKey    = TxtSoftOK;
                    data->editor_data.RightSoftKey   = TxtSoftBack;
                    if ((sat_command->qual & SAT_M_INKEY_ALPHA) EQ 0) /* 0=digits, 1=alpha */
                    {
                        data->editor_data.mode = E_EDIT_DIGITS_MODE;
                    }
                    else
                    {
                        data->editor_data.mode = E_EDIT_ALPHA_MODE;
                    }
                }
            data->editor_data.TextId                 = TxtNull;
            if (txt->len > 0)
                {
                    data->editor_data.TextString     = sat_create_TEXT_ASCIIZ (txt); /* label */

                }
            else
                {
                    data->editor_data.TextString     =  NULL; /* no label */
                }
            data->editor_data.min_enter              = 0; /* get a single key */
            data->editor_data.timeout                = TWENTY_SECS; 
            data->editor_data.Identifier             = event;
            if ((sat_command->qual & SAT_M_INKEY_HELP_AVAIL) NEQ 0)
                {
                    data->editor_data.AlternateLeftSoftKey  = TxtHelp; /* help available */
                }
            else
                {
                    data->editor_data.AlternateLeftSoftKey  = 0;       /* no help available */
                }
            data->editor_data.hide                   = FALSE; 


            /* provide an empty zero terminated buffer */
            data->edt_buffer[0] = '\0'; 
            
            /*
             * Start the Editor
             */
            if (editor_start_common (win, data->edt_buffer, 3, &data->editor_data, (T_EDIT_CB)sat_editor_cb) == NULL)
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
                sat_get_key_destroy (data->win);
            }
#endif /* NEW_EDITOR */
            break;

		/* sbh - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:
			sat_get_key_destroy (data->win);
			break;
		/* ...sbh */
		
        default:
            TRACE_EVENT ("sim_get_key_exec() unexpected event");
            break;
        }
}

/*********************************************************************
 *
 * SUB WINDOW  SAT_GET_STRING
 *
 *********************************************************************/
#define SAT_GET_STRING_BUFFER_SIZE (UBYTE)255
typedef struct
{
    T_MMI_CONTROL  mmi_control;
    T_MFW_HND      win;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA  editor_data;
#endif
    char          *edt_buffer; /* buffer for editor */ 
    U16            max_chars;
    T_MFW_HND		kbd;  /* sbh - keyboard handler, so window can be destroyed by user */
    T_MFW_HND chinese_edit_win; /* SPR#1700 - DS - Chinese editor window reference */
    T_MFW_HND           win_editor; /* SPR#2490 - DS - Store editor window */ 
    
} T_sat_get_string;

typedef struct {
    T_SAT_TXT txt;
    UBYTE     buffer[1]; /* open array */ 
} T_sat_get_string_result; /* NOTE: txt and buffer shall be packed */

static void sat_get_string_destroy (T_MFW_HND own_window);
static void sat_get_string_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command);



/*******************************************************************************

 $Function:    	sat_get_string_create

 $Description:	Creation of an instance for the SAT GET STRING dialog.Window must 
 				be available after reception of SAT command,only one instance.

 
 $Returns:		window 

 $Arguments:	parent_window - parent window
*******************************************************************************/
T_MFW_HND sat_get_string_create (T_MFW_HND parent_window)
{
    T_sat_get_string * data = (T_sat_get_string *)ALLOC_MEMORY (sizeof (T_sat_get_string));
    T_MFW_WIN        * win;

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb);  // c030 rsa

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_get_string_exec;
    data->mmi_control.data     = data;
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

 $Function:    	sat_get_string_destroy

 $Description:	Destroy the sat get string dialog.

 
 $Returns:		none

 $Arguments:	own_window - current window.
*******************************************************************************/
static void sat_get_string_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN * win_data    = ((T_MFW_HDR *)own_window)->data;
    T_sat_get_string * data = (T_sat_get_string *)win_data->user;

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : sat_get_string_destroy called with NULL Pointer");
		return;
	}

    TRACE_FUNCTION("sat_get_string_destroy()");

    if (data)
        {
            /*
             * Delete WIN Handler
             */
            win_delete (data->win);
            
            /*
             * Free Memory
             */
             if (((data->sat_command->qual & SAT_M_INPUT_ALPHA) == 1) /* 0=digits, 1=alpha */
		    && (data->sat_command->qual & SAT_M_INPUT_UCS2)	/* Unicode input requested */
	          && (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE))
             {
                FREE_MEMORY ((U8 *)data->edt_buffer, 
                (U16)sizeof(U16) + (data->max_chars * sizeof(U16)));

                /* SPR#1700 - DS - Destroy Chinese editor */
#if defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
		  if (data->chinese_edit_win)
		  {
		      chinese_input_destroy(data->chinese_edit_win);
		      data->chinese_edit_win = NULL;
		  }
#endif /* CHINESE_MMI && EASY_TEXT_ENABLED */
             }
             else /* Ascii */
             {
                FREE_MEMORY ((U8 *)data->edt_buffer,
                data->max_chars * sizeof(char));
             }
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		if (data->editor_data.editor_attr.TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)data->editor_data.editor_attr.TitleIcon.data, data->editor_data.editor_attr.TitleIcon.width * 
								data->editor_data.editor_attr.TitleIcon.height);
			data->editor_data.editor_attr.TitleIcon.data = NULL;
		}
#endif
                /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			sat_destroy_TEXT_ASCIIZ ((char *)data->editor_data.TitleString); /* label */
                    if (data->win_editor)
                    {
                        AUI_edit_Destroy(data->win_editor); /* SPR#2490 - DS - Destroy editor */
                        data->win_editor = NULL;
                    }
#else /* NEW_EDITOR */
            sat_destroy_TEXT_ASCIIZ (data->editor_data.TextString); /* label */
#endif /* NEW_EDITOR */

             FREE_MEMORY ((U8 *)data, sizeof (T_sat_get_string));
        }
}


/*******************************************************************************

 $Function:    	sat_get_string_exec

 $Description:	Dialog function for sat_get_string window.

 
 $Returns:		none

 $Arguments:	win - window handle
 				event - window event
 				value - unique id
 				sat_command - Sat Command info.
*******************************************************************************/
static void sat_get_string_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command)
{
    T_MFW_WIN       * win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_get_string   * data       = (T_sat_get_string *)win_data->user;
    T_SAT_INPUT     * inp;
    T_SAT_TXT      * defRsp;
    T_SAT_RES sat_res;
    UBYTE firstDefRespChar; /* SPR#2321 - DS - First character in default response */
    /* SPR#1428 - SH - New Editor: store data in these, for efficiency */
#ifdef NEW_EDITOR
	USHORT mode;
	UBYTE *textStr;
#endif

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
#endif

    TRACE_FUNCTION ("sat_get_string_exec()");

    switch (event)
        {
        case SAT_GET_STRING:

            /* SPR#1700 - DS - Modifed to handle Chinese/Unicode
             */
             
            data->sat_command = sat_command; /* save a pointer to the parameter for later use in callbacks */
            inp = (T_SAT_INPUT *)&sat_command->c.inp;

	     /* SPR#2321 - DS - Max buffer size of 160 chars for SAT STRING (Get Input) */
	     if ((U16)inp->rspMax > 160)
	     {
	     	  TRACE_EVENT("SAT supplied max is bigger than supported string size. Set to supported max 160");
	     	  data->max_chars = (U16)160+1;
	     }
	     else
	     {
            data->max_chars = (U16)(inp->rspMax)+1;
	     }
            
            /* allocate a buffer to hold the edited chars + terminator  */
           if (((data->sat_command->qual & SAT_M_INPUT_ALPHA) == 1) /* 0=digits, 1=alpha */
		    && (data->sat_command->qual & SAT_M_INPUT_UCS2)	/* Unicode input requested */
	          && (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE))
            {
                /* Include slot for unicode tag */
                data->edt_buffer = (char *)ALLOC_MEMORY ((U16)sizeof(U16) + (data->max_chars * sizeof(U16)));
            }
            else /* Ascii */
            {
                data->edt_buffer = (char *)ALLOC_MEMORY (data->max_chars * sizeof(char));
            }
            
#ifdef TRACE_SAT_STRING
            TRACE_EVENT_P3("rspMax %d, max_chars %d, defRsp.code %02x", inp->rspMax, data->max_chars, inp->defRsp.code);
#endif /* TRACE_SAT_STRING */

            /*MC, SPR940/2 if we are a expecting unicode response. This, of course assumes we're getting the correct DCS*/
            if (inp->defRsp.code == MFW_DCS_UCS2)
            { 	/*we have to put a unicode tag in so the editor knows to use Unicode*/
            	      data->edt_buffer[0]=  (char)0x80;  /* x0039928 - Lint warning fix */
			data->edt_buffer[1]= (char)0x7F;
            }

            /* SPR#2321 - DS - Default response set up */
            defRsp = &inp->defRsp;
            firstDefRespChar = *((UBYTE*)defRsp+defRsp->text);

#ifdef TRACE_SAT_STRING
            TRACE_EVENT_P2("prompt.len %d, defRsp->len %d", inp->prompt.len, defRsp->len);
#endif /* TRACE_SAT_STRING */              
                
            if (defRsp->len > 0) /* default response available? */
            {
                   if (firstDefRespChar != 0x00)
                   {
                        /* provide a zero terminated buffer, initially containing the default response */
                        sat_TEXT_to_ASCIIZ (data->edt_buffer, defRsp);
                   }
        		else
    		      {
                		/* NULL default response: provide an empty buffer */
            			data->edt_buffer[0] = '\0';
    		      }
            }
            else
            {
                /* no default response: provide an empty buffer */
                data->edt_buffer[0] = '\0';
            }
            
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR

#if defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
		/* If chinese is selected use the chinese editor */
		 if (((data->sat_command->qual & SAT_M_INPUT_ALPHA) == 1) /* 0=digits, 1=alpha */
		        && (data->sat_command->qual & SAT_M_INPUT_UCS2)	/* Unicode input requested */
	              && (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			 && (data->edt_buffer[0]==0x00 || data->edt_buffer[0] == (char)0x80))
		{
		    T_CHINESE_DATA chinese_data;

		    /* Initialise the Chinese editor window reference */
		    data->chinese_edit_win = NULL;

		    TRACE_EVENT("Setup Chinese SAT String editor");
		
		    chinese_data.TextString = (char*) data->edt_buffer;

                 if (inp->prompt.len > 0)
                {
                    textStr = (UBYTE *)sat_create_TEXT_ASCIIZ (&inp->prompt); /* label */
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialogue displays them correctly*/
    			 sat_add_unicode_tag_if_needed(textStr);
#endif
                }
                else
                {
                    textStr =  NULL; /* no label */
                }
			
                 /* Setup Chinese editor data */
        	    chinese_data.Callback = (T_AUI_EDIT_CB)sat_editor_cb;
        	    chinese_data.Identifier = event;
        	    chinese_data.LeftSoftKey = TxtSoftOK;
        	    chinese_data.DestroyEditor = FALSE;
        	    chinese_data.EditorSize = data->max_chars-1;
        	    data->chinese_edit_win = chinese_input(win, &chinese_data);
		}
		else
#endif /* CHINESE_MMI && EASY_TEXT_ENABLED */
            {
                 /*
                 * Setup an Editor to get a string 
                 */
                AUI_edit_SetDefault(&data->editor_data);
                AUI_edit_SetDisplay(&data->editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT); 

                /* SPR#2490 - DS - Initialise editor window reference */
                data->win_editor = NULL;
          
                if ((sat_command->qual & SAT_M_INPUT_HELP_AVAIL) NEQ 0)
                {
                    AUI_edit_SetAltTextStr(&data->editor_data, inp->rspMin, TxtHelp, FALSE, TxtNull); /* help available */
                }
                else
                {
                	AUI_edit_SetAltTextStr(&data->editor_data, inp->rspMin, TxtNull, FALSE, TxtNull); /* help available */
                }

    			mode = 0;
    			if ((sat_command->qual & SAT_M_INPUT_NOECHO) NEQ 0)
    			{
    				mode |= ED_MODE_HIDDEN;
    			}

    			if ((sat_command->qual & SAT_M_INPUT_ALPHA) EQ 0) /* 0=digits, 1=alpha */
                {
                   /* Don't need to alter mode */
                }
                else
                {
                    mode |= ED_MODE_ALPHA;
                }

                if (inp->prompt.len > 0)
                {
                    textStr = (UBYTE *)sat_create_TEXT_ASCIIZ (&inp->prompt); /* label */
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialogue displays them correctly*/
    				sat_add_unicode_tag_if_needed(textStr);
#endif
                }
                else
                {
                    textStr =  NULL; /* no label */
                }

                AUI_edit_SetMode(&data->editor_data,mode,ED_CURSOR_UNDERLINE);
                AUI_edit_SetTextStr(&data->editor_data, TxtSoftOK, TxtSoftBack, TxtNull, textStr);
                AUI_edit_SetEvents(&data->editor_data, event, FALSE /*SPR#2490 - DS*/, TWENTY_SECS, (T_AUI_EDIT_CB)sat_editor_cb);

     		   AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->edt_buffer, data->max_chars);

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.inp.prompt.icon.qual != 0xFF)
		{
			/* Copy the icon data to the editor attributes*/
			data->editor_data.editor_attr.TitleIcon.width = sat_command->c.inp.prompt.iconInfo.width;  
			data->editor_data.editor_attr.TitleIcon.height = sat_command->c.inp.prompt.iconInfo.height;  

			icon_length = sat_command->c.inp.prompt.iconInfo.width * sat_command->c.inp.prompt.iconInfo.height;
										
			data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.inp.prompt.iconInfo.dst, icon_length);
			data->editor_data.editor_attr.TitleIcon.isTitle = TRUE;

			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.inp.prompt.icon.qual == 0x00)
			     	{
						/* Icon is self-explanatory. Do not display the text*/
						data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
			     	}
				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.inp.prompt.iconInfo.dst, icon_length);
				sat_command->c.inp.prompt.iconInfo.dst = NULL;
		}
#endif
			   
                /*
                 * Start the Editor
                 */
                 /* SPR#2490 - DS - Store editor window reference */
                 data->win_editor = AUI_edit_Start(win, &data->editor_data);
                 if (data->win_editor == NULL)
                 {
                    sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                    sat_done (sat_command, sat_res);
                    sat_get_string_destroy (data->win);
                }
            }
 #else /* NEW_EDITOR */
            /*
             * Setup an Editor to get a string 
             */
            data->editor_data.LeftSoftKey            = TxtSoftOK;
            if ((sat_command->qual & SAT_M_INPUT_HELP_AVAIL) NEQ 0)
                {
                    data->editor_data.AlternateLeftSoftKey  = TxtHelp; /* help available */
                }
            else
                {
                    data->editor_data.AlternateLeftSoftKey  = TxtNull; /* no help available */
                }
            data->editor_data.RightSoftKey           = TxtSoftBack;
            data->editor_data.hide                   = ((sat_command->qual & SAT_M_INPUT_NOECHO) NEQ 0);
            if ((sat_command->qual & SAT_M_INPUT_ALPHA) EQ 0) /* 0=digits, 1=alpha */
            {
                data->editor_data.mode = E_EDIT_DIGITS_MODE;
            }
            else
            {
                data->editor_data.mode = E_EDIT_ALPHA_MODE;
            }
             data->editor_data.TextId                 = TxtNull;
            if (inp->prompt.len > 0)
                {
                    data->editor_data.TextString     = sat_create_TEXT_ASCIIZ (&inp->prompt); /* label */

                }
            else
                {
                    data->editor_data.TextString     =  NULL; /* no label */
                }
             data->editor_data.min_enter              = inp->rspMin;
            data->editor_data.timeout                = TWENTY_SECS; 
            data->editor_data.Identifier             = event;
 
            /*
             * Start the Editor
             */
             if (editor_start_common (win, data->edt_buffer, data->max_chars, &data->editor_data, (T_EDIT_CB)sat_editor_cb) == NULL)
             {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
                sat_get_string_destroy (data->win);
            }
#endif /* NEW_EDITOR */
            break;

		/* sbh - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:

                    TRACE_EVENT("SAT_DESTROY_WINDOW"); 

                    if (data->win)
                    {
			      sat_get_string_destroy (data->win);
			      data->win = NULL;
                    }
			break;
		/* ...sbh */
		
        default:
            TRACE_EVENT ("sat_get_string_exec() unexpected event");
            break;
        }
}

/*********************************************************************
 *
 * SUB WINDOW  SAT_DISPLAY_TEXT
 *
 *********************************************************************/
typedef struct
{
    T_MMI_CONTROL  mmi_control;
    T_MFW_HND      win;
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;		/* SPR#1428 - SH - New Editor data */
#else
     T_EDITOR_DATA  editor_data;
#endif
    char          *info_buffer; /* buffer for info */ 
    T_MFW_HND		kbd;  /* sbh - keyboard handler, so window can be destroyed by user */
 } T_sat_display_text;

//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Added a globale variable to store the handle since it will not be possible to add structure elements 
// for all type of windows 
T_MFW_HND satDisplayTextWin = NULL;

static void sat_display_text_destroy (T_MFW_HND own_window);
static void sat_display_text_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command);

/*******************************************************************************

 $Function:    	sat_display_text_create

 $Description:	Creation of an instance for the SAT DISPLAY TEXT dialog.Window 
 				must be available after reception of SAT command,only one instance.

 $Returns:		Window handle

 $Arguments:	parent_window - parent window
*******************************************************************************/

T_MFW_HND sat_display_text_create (T_MFW_HND parent_window)
{
    T_sat_display_text * data = (T_sat_display_text *)ALLOC_MEMORY (sizeof (T_sat_display_text));
    T_MFW_WIN       * win;

    TRACE_FUNCTION("sat_display_text_create");

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb); // c030 rsa

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)sat_display_text_exec;
    data->mmi_control.data     = data;
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

 $Function:    	sat_display_text_destroy

 $Description:	Destroy the sat play tone dialog.

 $Returns:		none

 $Arguments:	own_window - current window
*******************************************************************************/
static void sat_display_text_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN * win_data   = ((T_MFW_HDR *)own_window)->data;
    T_sat_display_text * data = (T_sat_display_text *)win_data->user;

    TRACE_FUNCTION("sat_display_text_destroy");
	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
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
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		if (data->editor_data.editor_attr.TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)data->editor_data.editor_attr.TitleIcon.data, data->editor_data.editor_attr.TitleIcon.width * 
								data->editor_data.editor_attr.TitleIcon.height);
			data->editor_data.editor_attr.TitleIcon.data = NULL;
		}
#endif
			
             sat_destroy_TEXT_ASCIIZ (data->info_buffer); /* displayed text */
             FREE_MEMORY ((U8 *)data, sizeof (T_sat_display_text));
        }
}

/*******************************************************************************

 $Function:    	sat_display_text_exec

 $Description:	Dialog function for sat_display_text_exec window.

 $Returns:		none

 $Arguments:	win - window
 				event - window event
 				value - unique id
 				sat_command - sat command data.
*******************************************************************************/

static void sat_display_text_exec (T_MFW_HND win, USHORT event, SHORT value,  T_SAT_CMD * sat_command)
{

    T_MFW_WIN       * win_data   = ((T_MFW_HDR *)win)->data;
    T_sat_display_text * data       = (T_sat_display_text *)win_data->user;
    T_SAT_RES sat_res;


// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
#endif

    TRACE_FUNCTION ("sat_display_text_exec()");

    switch (event)
        {
        case SAT_DISPLAY_TEXT:
            data->sat_command = sat_command;

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
            /*
             * Setup an Editor to display a string 
             */
            AUI_edit_SetDefault(&data->editor_data);
            /* SPR#1559 - SH - Now use ZONE_FULLSOFTKEYS */
            AUI_edit_SetDisplay(&data->editor_data, ZONE_FULLSOFTKEYS, COLOUR_EDITOR, EDITOR_FONT);  
            AUI_edit_SetTextStr(&data->editor_data, TxtSoftOK, TxtSoftBack, TxtNull, NULL);

            AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
            
            if ((data->sat_command->qual & SAT_M_TEXT_WAIT) NEQ 0)
            {
            	AUI_edit_SetEvents(&data->editor_data, SAT_DISPLAY_TEXT_WAIT, TRUE, TWO_MIN, (T_AUI_EDIT_CB)sat_editor_cb);
            }
            else
            {
                AUI_edit_SetEvents(&data->editor_data, event, TRUE, TEN_SECS, (T_AUI_EDIT_CB)sat_editor_cb);
            }

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.text.icon.qual != 0xFF)
		{
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.text.icon.qual == 0x00)
			     	{
						/* Icon is self-explanatory. Do not display the text.
						    Only for SAT commands that use ATB editor directly, 
						    we pass NULL as the text string, i.e., there is no Title string.
						    The string is embedded as part of ATB string in these cases.
						    For commands using AUI editor for Title string display,
						    we pass the string & it is not displayed based on 'selfExplanatory' flag.
						    */
						data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
		            			data->info_buffer = NULL;
			     	}
			     else
	 	            		/* provide a buffer, containing the string to be displayed */
		            		data->info_buffer = sat_create_TEXT_ASCIIZ (&sat_command->c.text);

				/* Copy the icon data to the editor*/
				data->editor_data.editor_attr.TitleIcon.width = sat_command->c.text.iconInfo.width;  
				data->editor_data.editor_attr.TitleIcon.height = sat_command->c.text.iconInfo.height; 

				icon_length = sat_command->c.inp.prompt.iconInfo.width * sat_command->c.inp.prompt.iconInfo.height;
											
				data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

				memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.inp.prompt.iconInfo.dst, icon_length);
				data->editor_data.editor_attr.TitleIcon.isTitle = FALSE;
				data->editor_data.editor_attr.TitleIcon.display_type = SAT_ICON_DISPLAY_TEXT;

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.inp.prompt.iconInfo.dst, icon_length);
				sat_command->c.inp.prompt.iconInfo.dst = NULL;
		}
		else
		{
            /* provide a buffer, containing the string to be displayed */
            data->info_buffer = sat_create_TEXT_ASCIIZ (&sat_command->c.text);
		}
#else
            /* provide a buffer, containing the string to be displayed */
            data->info_buffer = sat_create_TEXT_ASCIIZ (&sat_command->c.text);
#endif

#if 0 /* SPR#2340 - DS - Trace out buffer contents */
            for (i=0; i<sat_command->c.text.len+2+2 /* include tag+whole string+terminator */; i++)
                TRACE_EVENT_P2("SAT info_buffer[%d]: 0x%02x", i, data->info_buffer[i]);
#endif 
            
          /*
             * Start the Editor
             */

			/* SPR#1559 - SH - Check for unicode strings */
			
                    /*Oct 09, 2006 DRT: OMAPS00092083 - x0047075(Archana)
                       Descrption:CT-PTCRB[27.22.4.1.5]-MS  Resets while Icon Display
                       Solution : If the sat icon is present which is self explanatory then editor buffer is intialized to blank.*/
                     if(data->info_buffer == NULL)
                    {
                       AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE*)"", 1);
                    }                    
			else if (data->info_buffer[0]== (char)0x80) /* x0039928 - Lint warning fix */
		      {
		           /* SPR#2340 - DS - Len is number of characters, NOT number of bytes */
			    AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_UNICODE, (UBYTE *)&data->info_buffer[2], ( (sat_command->c.text.len)/2+1 ));
		      }
			else
			{
			    AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->info_buffer, sat_command->c.text.len+1);
			}

			TRACE_EVENT_P2("Editor len: %d, size: %d", data->editor_data.editor_attr.text.len, data->editor_data.editor_attr.size);
//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Store the handle of the editor in a globale variable for further use. A global variable has to be used here 
// since there is no other way to access this element in the other applications.
			satDisplayTextWin=AUI_edit_Start(win, &data->editor_data);
			if ( satDisplayTextWin == NULL)
			{
				sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
				sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
				sat_done (sat_command, sat_res);
				sat_display_text_destroy (satDisplayTextWin);
			}
            break;
#else /* NEW_EDITOR */
            /*
             * Setup an Editor to display a string 
             */
            data->editor_data.LeftSoftKey            = TxtSoftOK;
            data->editor_data.AlternateLeftSoftKey   = TxtNull; 
            data->editor_data.RightSoftKey           = TxtSoftBack;
            data->editor_data.hide                   = FALSE;
            data->editor_data.mode                   = E_EDIT_READ_ONLY_MODE; 
            data->editor_data.TextId                 = TxtNull;
            data->editor_data.TextString             = NULL; /* no label */
            data->editor_data.min_enter              = 0;
            if ((data->sat_command->qual & SAT_M_TEXT_WAIT) NEQ 0)
                {
                    data->editor_data.timeout         = TWO_MIN;
                    data->editor_data.Identifier      = SAT_DISPLAY_TEXT_WAIT;
                }
            else
                {
                    data->editor_data.timeout         = TEN_SECS;
                    data->editor_data.Identifier      = event;
                }

            /* provide a buffer, containing the string to be displayed */
            data->info_buffer   = sat_create_TEXT_ASCIIZ (&sat_command->c.text);
          /*
             * Start the Editor
             */

			if (editor_start_common (win, data->info_buffer, sat_command->c.text.len+1, &data->editor_data, (T_EDIT_CB)sat_editor_cb) == NULL)
            {
				sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
				sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
				sat_done (sat_command, sat_res);
				sat_display_text_destroy (data->win);
			}
            break;
#endif /* NEW_EDITOR */

		/* SH - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:
			sat_display_text_destroy (data->win);
			break;
		
        default:
            TRACE_EVENT ("sim_display_text_exec() unexpected event");
            break;
        }
}


/*******************************************************************************

 $Function:    	sat_ASCII_to_TEXT

 $Description:	This routine converts an ASCIIZ string into an MFW SAT text 
 				descriptor the coding scheme is taken accordign to the code in <txt>

 $Returns:		none

 $Arguments:	txt  - text info
 				destination - text destination 
 				source - text source

*******************************************************************************/

static void sat_ASCII_to_TEXT (T_SAT_TXT * txt, UBYTE * destination, UBYTE * source)
{
	UBYTE * temp_buffer;
	
	switch (txt->code)
	{
	case MFW_DCS_7bits:
		temp_buffer = (UBYTE *)ALLOC_MEMORY(txt->len+1); 
		sat_ascii_to_gsm ((char *)temp_buffer, (char *)source, (U16)(txt->len));
		utl_cvt8To7 (temp_buffer, txt->len, destination, 0);
		FREE_MEMORY ((U8 *)temp_buffer, txt->len+1);
		break;
	case MFW_DCS_8bits:
		sat_ascii_to_gsm ((char *)destination, (char *)source, (U16)(txt->len));
		break;
	case MFW_DCS_UCS2:
		sat_ascii_to_ucode ((wchar_t *)destination, (char *)source, (U16)(txt->len));
		break;
	default:
		sat_ascii_to_gsm ((char *)destination, (char *)source, (U16)(txt->len));
		TRACE_EVENT("sat_ASCII_to_TEXT() unexp. DCS");
		break;
	}
}

#ifdef INTEGRATION_SEPT00

/*******************************************************************************

 $Function:    	sat_UCODE_to_TEXT

 $Description:	This routine converts an UCODE string into an MFW SAT text descriptor
            the coding scheme is taken accordign to the code in <txt>


 $Returns:		none

 $Arguments:	txt  - text info
 				destination - text destination 
 				source - text source

*******************************************************************************/
static void sat_UCODE_to_TEXT (T_SAT_TXT * txt, UBYTE * destination, cp_wstring_t source)
{
	UBYTE * temp_buffer;
        UBYTE size;
        UBYTE outLen;

	switch (txt->code)
	{
	case MFW_DCS_7bits:
                size = (source->length/sizeof(wchar_t))+1;
		temp_buffer = (UBYTE *)ALLOC_MEMORY(size); 
                string_UCS2_to_GSM( 
                    MFW_DCS_7bits,	// gsm format 
                    (wstring_t *)source,// source unicode string, includes own length
                    size,	        // maximum output size
                    temp_buffer,	// destination byte array
                    &outLen		// actual length in bytes is stored here.
                    );
		txt->len = utl_cvt8To7 (temp_buffer, outLen, destination, 0);
		FREE_MEMORY ((U8 *)temp_buffer, size);
		break;
	case MFW_DCS_8bits:
                string_UCS2_to_GSM( 
                    MFW_DCS_7bits,	// gsm format 
                    (wstring_t *)source,// source unicode string, includes own length
                    size,	        // maximum output size
                    destination,	// destination byte array
                    &txt->len		// actual length in bytes is stored here.
                    );
		break;
	case MFW_DCS_UCS2:
                txt->len = source->length*sizeof(wchar_t); // txt count is in number of bytes
		memcpy(destination, source, txt->len);
		break;
	default:
		string_UCS2_to_GSM( 
                    MFW_DCS_7bits,	// gsm format 
                    (wstring_t *)source,// source unicode string, includes own length
                    size,	        // maximum output size
                    destination,	// destination byte array
                    &txt->len		// actual length in bytes is stored here.
                    );
		TRACE_EVENT("sat_UCODE_to_TEXT() unexp. DCS");
		break;
	}
}
#endif /* INTEGRATION_SEPT00 */

/*******************************************************************************

 $Function:    	sat_get_key_send_result

 $Description:	


 $Returns:		none

 $Arguments:	data  - key info

*******************************************************************************/

static void sat_get_key_send_result (T_sat_get_key * data)
{
    T_SAT_RES sat_res;
    struct {
        T_SAT_TXT txt;
        UBYTE      buffer[SAT_GET_KEY_BUFFER_SIZE]; /* buffer for result */ 
    } buf; /* NOTE: txt and buffer shall be packed */

    /* update the output parameter */

	/* a0876501 (Shashi Shekar B.S.) 03-Oct-2005 MMI-SPR-34554.
	   Populate Yes/No terminal response.
	   Ignore the DCS type to be sent if 'SAT_M_INKEY_YES_NO' is ON. 
	   Refer GSM 11.14 Sec 12.6, Get Inkey. */
	if ((data->sat_command->qual & SAT_M_INKEY_YES_NO) NEQ 0)
	{
		/* Data coding scheme is not considered for Yes/No response */
		buf.txt.code = SAT_M_INKEY_YES_NO; 

		buf.txt.text = sizeof(T_SAT_TXT);
		buf.txt.len = 1;

		/* Simply copy the data populated in sat_editor_cb */
		buf.buffer[0] = (UBYTE)data->edt_buffer[0]; 
	}
	else
	{
		/* set the required DCS for the result to be returned */
		if ((data->sat_command->qual & SAT_M_INKEY_UCS2) NEQ 0)
		{
			buf.txt.code = MFW_DCS_UCS2;
		}
		else
		{
			buf.txt.code = MFW_DCS_8bits; /* use the SMS default alphabet */
		}
    
		buf.txt.text = sizeof(T_SAT_TXT);
		buf.txt.len = 1;

		/* convert and send the result */
		sat_ASCII_to_TEXT(&buf.txt, buf.buffer, (UBYTE *)data->edt_buffer);
	}

    /* SPR#2340 - DS - Check if an icon was supplied with GET KEY command.
     * If supplied, set the result to 0x04 'Command performed successfully, but
     * requested icon could not be displayed'.
     */
    if (data->sat_command->c.text.icon.qual == 0xFF &&
     data->sat_command->c.text.icon.id == 0xFF)
    {
    /* Icon not present */
    sat_res[SAT_ERR_INDEX] = SatResSuccess;
    }
    else
    {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	 sat_res[SAT_ERR_INDEX] = SatResSuccess;
#else
     /* Icon present but ME cannot display icon */
     sat_res[SAT_ERR_INDEX] = SatResNoIcon;
#endif
    }
    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
    satString (data->sat_command,  &buf.txt, sat_res, sizeof(T_SAT_RES));
}

/*******************************************************************************

 $Function:    	sat_get_string_send_result

 $Description:	


 $Returns:		none

 $Arguments:	data  - string info

 SPR#1700 - DS - Modified to handle Chinese/Unicode

*******************************************************************************/
static void sat_get_string_send_result (T_sat_get_string * data)
{
    T_SAT_RES sat_res;
    T_sat_get_string_result * buf;
    U16 length;
    U16 size;
    T_SAT_INPUT* input; /* SPR#2321 - DS */

    TRACE_FUNCTION("sat_get_string_send_result()");

    /* allocate sufficient temporary space to return the result (avoid huge stack usage) */
    if (((data->sat_command->qual & SAT_M_INPUT_ALPHA) == 1) /* 0=digits, 1=alpha */
	    && (data->sat_command->qual & SAT_M_INPUT_UCS2)	/* Unicode input requested */
	    && (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE))
     {
        length = 2*ATB_string_UCLength((USHORT*)data->edt_buffer);
     }
     else /* Ascii */
     {
        length = strlen(data->edt_buffer);
     }
    size = sizeof(T_sat_get_string_result) + length;
     buf = (T_sat_get_string_result *) ALLOC_MEMORY(size); 
	
    /* update the output parameter */
    
    /* set the required DCS for the result to be returned */
    if ((data->sat_command->qual & SAT_M_INPUT_UCS2) NEQ 0)
    {
        buf->txt.code = MFW_DCS_UCS2;
    }
    else if ((data->sat_command->qual & SAT_M_INPUT_PACKING) NEQ 0)
    {
        buf->txt.code = MFW_DCS_7bits; /* use the packed SMS alphabet */
    }
    else 
    {
        buf->txt.code = MFW_DCS_8bits; /* use the SMS default alphabet */
    }
    
    buf->txt.text = sizeof(T_SAT_TXT);
    buf->txt.len = length;

    /* convert and send the result */
    if (((data->sat_command->qual & SAT_M_INPUT_ALPHA) == 1) /* 0=digits, 1=alpha */
	    && (data->sat_command->qual & SAT_M_INPUT_UCS2)	/* Unicode input requested */
	    && (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE))
     {
        //Copy the data into the SAT txt buffer
        memcpy(buf->buffer, data->edt_buffer, length);
     }
     else
     {
        sat_ASCII_to_TEXT(&buf->txt, buf->buffer, (UBYTE *)data->edt_buffer);
     }
     
     /* SPR#2321 - DS - Check if an icon was supplied with SAT_STRING (Get Input) 
       * command. If supplied, set the result to 0x04 'Command performed successfully, 
       * but requested icon could not be displayed'.
       */
     input = (T_SAT_INPUT*)&data->sat_command->c.inp;
     if (input->prompt.icon.qual == 0xFF &&
         input->prompt.icon.id == 0xFF)
    {
    	/* Icon not present */
        sat_res[SAT_ERR_INDEX] = SatResSuccess;
    }
    else
    {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	 sat_res[SAT_ERR_INDEX] = SatResSuccess;
#else
/* Icon present but ME cannot display icon */
        sat_res[SAT_ERR_INDEX] = SatResNoIcon;
#endif
    }
    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
    satString (data->sat_command,  &buf->txt, sat_res, sizeof(T_SAT_RES));

    FREE_MEMORY((U8 *)buf, size);
}

/*******************************************************************************

 $Function:    	sat_editor_cb

 $Description:	Sat editor call back function.


 $Returns:		none

 $Arguments:	win  - window
 				Identifier - unique id
 				event - event id
 				
*******************************************************************************/
/* SPR#1428 - SH - New Editor: minor change so function matches callback type */
#ifdef NEW_EDITOR
static void sat_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT event)
#else
static void sat_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event)
#endif
{
    T_MFW_WIN    * win_data = ((T_MFW_HDR *)win)->data;
    T_SAT_RES sat_res;
    
    TRACE_EVENT ("sat_editor_cb()");
    
    TRACE_EVENT_P1("event number %d",event);
		  
    if ((win EQ NULL) || (win_data EQ NULL))
        return;
    
    switch (Identifier)
    {
    case SAT_GET_KEY:
	{
	    T_sat_get_key * data = (T_sat_get_key *)win_data->user;
	    
            if (data EQ NULL)
                return;
		
	    switch(event)
	    {
	    case INFO_KCD_LEFT: /* OK or single digit key or YES */
		/* update the output parameter */
		if ((data->sat_command->qual & SAT_M_INKEY_YES_NO) NEQ 0)
		{
		    data->edt_buffer[0] = 0x01; /* means YES */
		    data->edt_buffer[1] = 0x00; /* means YES */
		}
		
		sat_get_key_send_result (data);
		break;
	    case INFO_KCD_ALTERNATELEFT: /* help */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_HELP;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_KCD_RIGHT: /* back or NO */
		if ((data->sat_command->qual & SAT_M_INKEY_YES_NO) NEQ 0)
		{
		    data->edt_buffer[0] = 0x00; /* means NO */
		    data->edt_buffer[1] = 0x00; /* means NO */
		    sat_get_key_send_result (data);
		}
		else
		{
		    sat_res[SAT_ERR_INDEX] = SAT_RES_USER_BACK;
		    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		    sat_done (data->sat_command, sat_res);
		}
		break;
	    case INFO_KCD_CLEAR: /* clear on empty buffer or long clear */
		case INFO_KCD_HUP:		
			sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
			sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
			sat_done (data->sat_command, sat_res);
		break;
	    case INFO_TIMEOUT: /* timeout */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_NO_RESP;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_KCD_CALL: /* SEND key */
	      /* SPR#2490 - DS - Explicitly ignore SEND key press */
	      TRACE_EVENT("SEND keypress ignored");
	      return;
            default:
                TRACE_EVENT("sat_editor_cb() unexp. event");
                return;
                //break;
	    }
	    sat_get_key_destroy (data->win);
	}
	break;
    case SAT_GET_STRING:
	{
	    T_sat_get_string * data = (T_sat_get_string *)win_data->user;
	    
            if (data EQ NULL)
                return;

	    switch(event)
	    {
	    case INFO_KCD_LEFT: /* OK */	        
	      sat_get_string_send_result (data);
		SEND_EVENT(data->win, SAT_DESTROY_WINDOW, 0, 0); /* SPR#1700 - DS - Destroy SAT string data */
             break;		
	    case INFO_KCD_ALTERNATELEFT: /* help */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_HELP;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_KCD_RIGHT: /* back */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_BACK;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_KCD_CLEAR: /* clear on empty buffer or long clear */	
	    case INFO_KCD_HUP: /* SPR#2321 - DS - Handle hangup keypress */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_TIMEOUT: /* timeout */
		sat_res[SAT_ERR_INDEX] = SAT_RES_USER_NO_RESP;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		sat_done (data->sat_command, sat_res);
		break;
	    case INFO_KCD_CALL: /* SEND key */
	      /* SPR#2490 - DS - Explicitly ignore SEND key press */
	      TRACE_EVENT("SEND keypress ignored");
	      return;
          default:
                TRACE_EVENT("sat_editor_cb() unexp. event");
                return;
                //break;
	    }

	    /* SPR#1700 - DS - Only destroy string if not already destroyed */
	    if (data->win)
	    {
	        sat_get_string_destroy (data->win);
	        data->win = NULL;
	    }
	}
	break;

    case SAT_DISPLAY_TEXT:
        {
            T_sat_display_text   * data     = (T_sat_display_text *)win_data->user;
            
            switch (event)
            {
            case INFO_KCD_RIGHT: /* back */
                sat_res[SAT_ERR_INDEX] = SAT_RES_USER_BACK;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_TIMEOUT: /* timeout */
            case INFO_KCD_LEFT: /* OK */
                 /* SPR#2340 - DS - Check if an icon was supplied with DISPLAY TEXT 
                   * command. If supplied, set the result to 0x04 'Command performed successfully, 
                   * but requested icon could not be displayed'.
                   */
                 if (data->sat_command->c.text.icon.qual == 0xFF &&
                     data->sat_command->c.text.icon.id == 0xFF)
                {
                    /* Icon not present */
                    sat_res[SAT_ERR_INDEX] = SatResSuccess;
                }
                else
                {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
			 sat_res[SAT_ERR_INDEX] = SatResSuccess;
#else
			/* Icon present but ME cannot display icon */
        		sat_res[SAT_ERR_INDEX] = SatResNoIcon;
#endif					
                }
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_KCD_CLEAR: /* abort */
			case INFO_KCD_HUP:		
                sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_KCD_CALL: /* SEND key */
	         /* SPR#2490 - DS - Explicitly ignore SEND key press */
	         TRACE_EVENT("SEND keypress ignored");
	         return;
            default:
                TRACE_EVENT("sat_editor_cb() unexp. event");
                return;
                //break;
            }
            sat_display_text_destroy (data->win);
//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Re-initialize the global variable to NULL.
		satDisplayTextWin = NULL;
        }
        break;
        
    case SAT_DISPLAY_TEXT_WAIT:
        {
            T_sat_display_text   * data     = (T_sat_display_text *)win_data->user;
            
            switch (event)
            {
            case INFO_TIMEOUT: /* timeout */
                sat_res[SAT_ERR_INDEX] = SAT_RES_USER_NO_RESP;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_KCD_LEFT: /* OK */
                /* SPR#2340 - DS - Check if an icon was supplied with DISPLAY TEXT 
                   * command. If supplied, set the result to 0x04 'Command performed successfully, 
                   * but requested icon could not be displayed'.
                   */
                 if (data->sat_command->c.text.icon.qual == 0xFF &&
                     data->sat_command->c.text.icon.id == 0xFF)
                {
                    /* Icon not present */
                    sat_res[SAT_ERR_INDEX] = SatResSuccess;
                }
                else
                {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
			 sat_res[SAT_ERR_INDEX] = SatResSuccess;
#else
			/* Icon present but ME cannot display icon */
        		sat_res[SAT_ERR_INDEX] = SatResNoIcon;
#endif					
                }
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_KCD_RIGHT: /* back */
                sat_res[SAT_ERR_INDEX] = SAT_RES_USER_BACK;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            case INFO_KCD_CLEAR: /* abort */
			case INFO_KCD_HUP:		
                sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (data->sat_command, sat_res);
                break;
            /* SPR#2490 - DS - Explicitly ignore SEND key press */
            case INFO_KCD_CALL: /* SEND key */
        	   TRACE_EVENT("SEND keypress ignored");
        	   return;
            default:
                TRACE_EVENT("sat_editor_cb() unexp. event");
                return;
                //break;
            }
            sat_display_text_destroy (data->win);
//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Re-initialize the global variable to NULL.
		satDisplayTextWin = NULL;
        }
        break;
    default:
        TRACE_EVENT("sat_editor_cb() unexp. Identifier");
        break;
    }
    return;
}
// end c014 rsa
