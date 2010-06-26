/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       AUIPinEditor.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description: Editor for PIN entry


********************************************************************************

 $History: AUIPinEditor.c

    xreddymn 4, Jul, 2006 OMAPS00083495
    Resolved issues reported in PIN and PUK input through BPM.

        xrashmic 1 Jul, 2006 OMAPS00075784
        The password mode to be turned on while entering the gsm string to 
        unblock PUK during bootup

 	July 09, 2005   REF : MMI - 22565 - a0876501
	Description:	Proper display of IMEI with CD byte.
	Solution:		Increased the IMEI length from 16 to 18.                                                                           

   	June 16, 2005  REF: CRR 31267  x0021334
	Description: Handset ignore the initializtion of the PIN1/PIN2
	Fix:	Cheking is done to ascertain if PIN1/PIN2 are initialised. If not, appropriate
	       message is displayed to the user.

 	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
	Description:	IMEI retreival  by posting a call to ACI sAT_xx API
	Solution:		The direct call to cl_get_imeisv() is replaced with the
					call to the mmi_imei_retrieve() which inturn calls sAT_Dn() and
					retrieves the IMEI info and stores it onto the global 
					variable "imei"                                                                           
  14/11/02      Original Condat(UK) BMI version.
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

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_ss.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_ssi.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiDummy.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiIdle.h"
#include "MmiResources.h"
#include "MmiPins.h"
#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUITextEntry.h"

#include "AUIPinEditor.h"

//Nov 17, 2005   REF : OMAPS00045901 - a0876501
#define IMEI_DISP_LEN 18	/* IMEI display now includes CD byte. Hence the size has been increased from 16 to 18 */

typedef struct
{
    T_MMI_CONTROL		mmi_control;
    T_MFW_HND			parent;				// The parent window
    T_MFW_HND			win;				// The edit window
    T_MFW_HND			kbd;				// The keyboard handler
    T_MFW_HND			kbd_long;			// The longpress keyboard handler
    T_MFW_HND			timer;				// The timer for timeouts

	T_ED_DATA			*editor;			/* The ATB editor */
	T_AUI_EDITOR_DATA	editor_data;		/* The MMI editor data, provided by caller */
    T_AUI_ENTRY_DATA	*entry_data;		/* The MMI text entry data */
	
	BOOL				haveTitle;			/* TRUE if a title is supplied */
    T_ATB_TEXT			title;				/* Title of editor */

	BOOL				hidden;

	BOOL				doNextLongPress;	/* Flag used to notice/not notice long keypress */
	BOOL				enteredMinChars;	/* Set to TRUE when at least min_enter characters have been entered */
	BOOL				enteredSomeChars;	/* Set to TRUE if the length of the entered data is non-zero */
	UBYTE				emergency_call;		/* Set to TRUE if number is an emergency call number */
	UBYTE				ss_string;
	char				imei[IMEI_DISP_LEN];/* IMEI display now includes CD byte. Hence the size has been increased from 16 to 18 */
}
T_AUI_PIN_DATA;

#define AUI_MAX_PIN 16

// June 16, 2005  REF: CRR 31267  x0021334
extern BOOL pin1Flag; // This flag will be needed here

typedef struct
{
	U8		pin_id;
	char		puk_code[AUI_MAX_PIN + 1];
	char		new_pin[AUI_MAX_PIN + 1];
	char		cnf_pin[AUI_MAX_PIN + 1];
}
T_AUI_PINS;
/* LOCAL FUNCTION PROTOTYPES */
static T_MFW_HND AUI_pin_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
static void AUI_pin_ExecCb(T_MFW_HND win, USHORT event, SHORT value, void *parameter);
static int AUI_pin_WinCb(T_MFW_EVENT event, T_MFW_WIN *win_data);
static int AUI_pin_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
static int AUI_pin_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
static UBYTE AUI_pin_CheckEmergency(T_AUI_PIN_DATA *data);
T_MFW_SS_RETURN AUI_pin_CheckSSString(T_AUI_PIN_DATA *data);
static BOOL AUI_pin_StripSSUnblock(char *cmd_string, T_AUI_PINS *pin_data);




/*******************************************************************************

 $Function:		AUI_pin_Start

 $Description:	Start the PIN editor.

 $Returns:		None.

 $Arguments:	None.

*******************************************************************************/

T_MFW_HND AUI_pin_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("AUI_pin_Start()");

    win = AUI_pin_Create(parent, editor_data);

    return win;
}


/*******************************************************************************

 $Function:		AUI_pin_Create

 $Description:	Create the PIN editor.
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	parent	-	The parent window.

*******************************************************************************/

static T_MFW_HND AUI_pin_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_AUI_PIN_DATA	*data;
    T_MFW_WIN     		*win_data;

    TRACE_FUNCTION ("AUI_pin_Create()");
    TRACE_EVENT_P1("Memory left (start): %d", mfwCheckMemoryLeft());

 	data = (T_AUI_PIN_DATA *)ALLOC_MEMORY(sizeof (T_AUI_PIN_DATA));
 	TRACE_EVENT_P1("Memory left (data): %d", mfwCheckMemoryLeft());

	/* Create window handler */

    data->win = win_create(parent, 0, E_WIN_VISIBLE, (T_MFW_CB)AUI_pin_WinCb);	// Create window
	TRACE_EVENT_P1("Memory left (win): %d", mfwCheckMemoryLeft());

    if (data->win==NULL)														// Make sure window exists
    {
		return NULL;
    }

	/* Connect the dialog data to the MFW-window */
     
    data->mmi_control.dialog	= (T_DIALOG_FUNC)AUI_pin_ExecCb;				/* Setup the destination for events */
    data->mmi_control.data		= data;
    data->parent				= parent;
    win_data					= ((T_MFW_HDR *)data->win)->data;
    win_data->user				= (void *)data;

 	data->kbd					= kbd_create(data->win, KEY_ALL,(T_MFW_CB)AUI_pin_KbdCb);
	data->kbd_long				= kbd_create(data->win, KEY_ALL|KEY_LONG,(T_MFW_CB)AUI_pin_KbdLongCb); 
	data->editor				= ATB_edit_Create(&data->editor_data.editor_attr,0);
TRACE_EVENT_P1("Memory left (editor): %d", mfwCheckMemoryLeft());
	data->editor_data = *editor_data;
	data->entry_data = AUI_entry_Create(data->win, data->editor, E_PIN_UPDATE);
TRACE_EVENT_P1("Memory left (entry): %d", mfwCheckMemoryLeft());
	SEND_EVENT(data->win, E_PIN_INIT, 0, 0);
	
    /* Return window handle */

    return data->win;
}


/*******************************************************************************

 $Function:		AUI_pin_Destroy

 $Description:	Destroy the PIN editor.

 $Returns:		None.

 $Arguments:	window	-	The editor window.

*******************************************************************************/

void AUI_pin_Destroy(T_MFW_HND win)
{
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_PIN_DATA	*data		= (T_AUI_PIN_DATA *)win_data->user;

	TRACE_FUNCTION("AUI_pin_Destroy");
	TRACE_EVENT_P1("Memory left at start: %d", mfwCheckMemoryLeft());

    if (data)
    {
    	/* Free memory allocated for title */

		if (data->haveTitle)
		{
			FREE_MEMORY(data->title.data, (data->title.len+1)*ATB_string_Size(&data->title));
			TRACE_EVENT_P1("Memory left (title dest): %d", mfwCheckMemoryLeft());
		}

		/* Free text entry memory */
		
		AUI_entry_Destroy(data->entry_data);
		TRACE_EVENT_P1("Memory left (entry dest): %d", mfwCheckMemoryLeft());
		/* Delete window */
		
        win_delete (data->win);
		TRACE_EVENT_P1("Memory left (win dest): %d", mfwCheckMemoryLeft());
		/* Free editor memory */

		ATB_edit_Destroy(data->editor);
		TRACE_EVENT_P1("Memory left (editor dest): %d", mfwCheckMemoryLeft());	
		/* Free Memory */

		FREE_MEMORY ((void *)data, sizeof (T_AUI_PIN_DATA));
		TRACE_EVENT_P1("Memory left (data dest): %d", mfwCheckMemoryLeft());
    }
    
    return;
}


/*******************************************************************************

 $Function:    	AUI_pin_ExecCb

 $Description:	Dialog function for PIN editor.
 
 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

static void AUI_pin_ExecCb(T_MFW_HND win, USHORT event, SHORT value, void *parameter)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_PIN_DATA		*data		= (T_AUI_PIN_DATA *)win_data->user;
    T_MFW_HND			parent_win	= data->parent;
	T_ATB_TEXT			text={0,0,NULL};
	/* Store these in case editor is destroyed on callback */
    USHORT				Identifier	= data->editor_data.Identifier;
    T_AUI_EDIT_CB		Callback	= data->editor_data.Callback;
    UBYTE				destroyEditor = data->editor_data.destroyEditor;
	T_AUI_PINS		*pin_unblock_data;
	
    TRACE_FUNCTION ("AUI_pin_ExecCb()");

    switch (event)
    {
    	/* Initialise */
    
        case E_PIN_INIT:
        	TRACE_EVENT("E_PIN_INIT");
        	ATB_edit_Init(data->editor);

        	data->haveTitle = FALSE;

			/* If we require an output line, shrink editor to fit it at bottom
			 * NB TitleString is assumed here to be a numeric string of ascii digits */

			if (data->editor_data.TitleString)
			{
				data->haveTitle = TRUE;
				text.data = data->editor_data.TitleString;
			}

			/* Set up title */

			if (data->editor_data.TitleId!=NULL)
			{
				data->haveTitle = TRUE;
				text.data = (UBYTE *)MmiRsrcGetText(data->editor_data.TitleId);
			}

			/* If title exists, get its dcs and length */
			
			if (data->haveTitle)
			{
				if (text.data[0]==0x80)
				{
					text.data += 2;		/* Skip over two unicode indicator bytes */
					text.dcs = ATB_DCS_UNICODE;
				}
#ifdef EASY_TEXT_ENABLED
				else if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{
					text.dcs = ATB_DCS_UNICODE;
				}
#endif
				else
				{
					text.dcs = ATB_DCS_ASCII;
				}
				
				text.len = ATB_string_Length(&text);
				/* Allocate memory for the title and copy the text there */
				data->title.data = (UBYTE *)ALLOC_MEMORY((text.len+1)*ATB_string_Size(&text));
				TRACE_EVENT_P1("Memory left (title): %d", mfwCheckMemoryLeft());
				ATB_string_Copy(&data->title, &text);
			}

			/* Set flag if min_enter characters are in buffer */

			if (data->editor_data.editor_attr.text.len >= data->editor_data.min_enter)
				data->enteredMinChars = TRUE;
			else
				data->enteredMinChars = FALSE;
				
			/* Set flag if some characters are in the buffer */
			if (data->editor_data.editor_attr.text.len > 0 )
				data->enteredSomeChars = TRUE;
			else
				data->enteredSomeChars = FALSE;

			/* Show the window */
			win_show(data->win);
			break;

		case E_PIN_UPDATE:
        	TRACE_EVENT("E_PIN_UPDATE");
			win_show(data->win);
			break;

		case E_PIN_DEINIT:
			TRACE_EVENT("E_PIN_DEINIT");

			if (Callback)
    	    	(Callback) (parent_win, Identifier, value);
    	    	
			if (destroyEditor)
				AUI_pin_Destroy(data->win);
				
			break;

		case E_PUK1_COMPLETE:
			TRACE_EVENT("E_PIN_COMPLETE");

			pin_unblock_data = (T_AUI_PINS *)parameter;
			
			sim_unblock_pin(pin_unblock_data->pin_id,
							 pin_unblock_data->puk_code,
							 pin_unblock_data->new_pin); /* unblock pin1 */

			if (destroyEditor)
				AUI_pin_Destroy(data->win);
			
			break;

   	}

    return;
}

/*******************************************************************************

 $Function:		AUI_pin_WinCb

 $Description:	PIN editor window event handler.

 $Returns:		None.

 $Arguments:	event	- the event
 				win		- the editor window

*******************************************************************************/

static int AUI_pin_WinCb(T_MFW_EVENT event, T_MFW_WIN *win_data)
{
    T_AUI_PIN_DATA		*data			= (T_AUI_PIN_DATA *)win_data->user;
    T_ED_DATA			*editor			= data->editor;
    T_ATB_WIN_SIZE		*win_size		= &data->editor->attr->win_size;
	T_ATB_TEXT			text;
    T_DS_TEXTFORMAT		format;
  //  USHORT				titleLen;  // RAVI

       if (!data)
		return MFW_EVENT_CONSUMED;
  
    switch(event)
    {
        case E_WIN_VISIBLE: 											/* window is visible */
            if (win_data->flags & E_WIN_VISIBLE)
			{
				if (editor->update==ED_UPDATE_TRIVIAL)
				{
					TRACE_EVENT("Only updating editor.");
					ATB_edit_Show(data->editor);
				}
				else
				{
					TRACE_EVENT("Updating whole screen.");

					ATB_edit_Show(data->editor);

					/* Set the colour for drawing title */

					resources_setTitleColour(COLOUR_EDITOR);

					/* TITLE - only display if update type is FULL or DEFAULT*/

					if (editor->update!=ED_UPDATE_PARTIAL)
					{
						dspl_Clear(0,0, SCREEN_SIZE_X-1, win_size->py-1);
						
						if (data->haveTitle)
						{
							ATB_display_SetFormatAttr(&format, 0, FALSE);
							
							/* Display "Emergency?" prompt in title if emergency call */
							if (data->emergency_call)
							{
								text.dcs = data->title.dcs;
								text.data = (UBYTE *)MmiRsrcGetText(TxtEmergency);
								text.len = ATB_string_Length(&text);
								ATB_display_Text(0,0,&format, &text);
							}
							/* Otherwise, show title */
							else
							{
								ATB_display_Text(0,0,&format, &data->title);
							}
						}
					}
					
					/* Display Soft Keys */
					
					if (data->emergency_call)
		            {
		            	displaySoftKeys(TxtSoftCall, data->editor_data.RightSoftKey);
		            }
					else
					{
						if (data->editor_data.editor_attr.text.len < data->editor_data.min_enter)
						{
							/* entered less than the required number of chars: Alternate Softkey appears */
							if (data->editor_data.editor_attr.text.len > 0)
								displaySoftKeys(data->editor_data.AltLeftSoftKey,data->editor_data.RightSoftKey);
							else
								displaySoftKeys(data->editor_data.AltLeftSoftKey,TxtNull);
						}
						else
						{
							/* entered sufficient number of chars: Normal Softkey appears */
						
							if (data->editor_data.editor_attr.text.len > 0)
								displaySoftKeys(data->editor_data.LeftSoftKey,data->editor_data.RightSoftKey);
							else
								displaySoftKeys(data->editor_data.LeftSoftKey, TxtNull);
						}
					}
				}
			}
            break;
    }

	data->editor->update = ED_UPDATE_DEFAULT;

	/* x0045876, 14-Aug-2006 (WR - non-void function "AUI_pin_WinCb" should return a value) */
	return MFW_EVENT_CONSUMED ;
}


/*******************************************************************************

 $Function:		AUI_pin_KbdCb

 $Description:	PIN editor keyboard event handler

 $Returns:		None.

 $Arguments:	event		- the keyboard event
 				keyboard 

*******************************************************************************/

static int AUI_pin_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_PIN_DATA		*data		= (T_AUI_PIN_DATA *)win_data->user;
//	ED_RES				result;    // RAVI
	UBYTE				*edit_data_buf;
	BOOL				bRetVal;
	T_AUI_PINS			pin_unblock_data;
	T_DISPLAY_DATA 		display_info;
  	
	TRACE_FUNCTION("AUI_pin_KbdCb()");

   /* Suppress unwanted long keypresses */

	data->doNextLongPress = TRUE;         		/* next Key_long event is correct */
	data->editor->update = ED_UPDATE_DEFAULT;	/* Default behaviour is just to update text */

	switch (keyboard->code)
	{
		case KCD_STAR:
			/* Only possible when the first digit is a '*' */
			if ((data->editor_data.editor_attr.text.len==0) ||
			     (data->ss_string))
			{
				if (data->editor_data.editor_attr.text.len==0)
				{
					data->hidden = FALSE;
					data->ss_string = TRUE;
					/* The string should now be visible on screen */
					if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
					{
						data->hidden = TRUE;
						ATB_edit_ResetMode(data->editor, ED_MODE_HIDDEN);
					}
				}

				AUI_entry_EventKey(data->entry_data, event, keyboard);
				win_show(data->win);
			}
			//xrashmic 1 Jul, 2006 OMAPS00075784
                     //The password mode to be turned on while entering the gsm string to 
                    //unblock PUK during bootup
			if(data->editor_data.editor_attr.text.len >=4 )
                       {
                            if((strncmp((char*)data->editor_data.editor_attr.text.data, "**05*", 5) == 0) ||
                                (strncmp((char*)data->editor_data.editor_attr.text.data, "**052*", 6) == 0) ||
                                (strncmp((char*)data->editor_data.editor_attr.text.data, "**04*", 5) == 0) ||
                                (strncmp((char*)data->editor_data.editor_attr.text.data, "**042*", 6) == 0))
                            {
                             ATB_edit_SetMode(data->editor, ED_MODE_HIDDEN);
                            }
                       }
			break;

		case KCD_HASH:
			/* Send SS string if we're entering one */
			if(data->ss_string)
			{
				AUI_entry_EventKey(data->entry_data, event, keyboard);
		    		//xrashmic 1 Jul, 2006 OMAPS00075784
                            //The password mode to be turned on while entering the gsm string to 
                            //unblock PUK during bootup

                            if(ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
                                {
                                     if((strncmp((char*)data->editor_data.editor_attr.text.data, "**05*", 5) == 0) ||
                                     (strncmp((char*)data->editor_data.editor_attr.text.data, "**04*", 5) == 0))
                                     {
                                      strncpy((char*)&data->editor_data.editor_attr.text.data[5],(char*)&data->editor->hiddenText->data[5], data->editor_data.editor_attr.text.len - 5);
                                        }
                                     else if ((strncmp((char*)data->editor_data.editor_attr.text.data, "**052*", 6)== 0) ||
                                     (strncmp((char*)data->editor_data.editor_attr.text.data, "**042*", 6) == 0))
                                     {
                                      strncpy((char*)&data->editor_data.editor_attr.text.data[6],
                                      (char*)&data->editor->hiddenText->data[6], data->editor_data.editor_attr.text.len - 6);
                                     }
                               }         

				edit_data_buf = ALLOC_MEMORY(data->editor_data.editor_attr.size);

				strcpy((char *)edit_data_buf, (char *)data->editor_data.editor_attr.text.data);

				switch (AUI_pin_CheckSSString(data))
				{
					case MFW_SS_MMI:
// 	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
//	replace the call to get_imei() with callNumber					
#ifdef FF_MMI_ATC_MIGRATION					
						mmi_imei_retrieve();
#else
						get_imei((char*)data->imei);
						showIMEI(win,(char*)data->imei);
#endif						
						ATB_edit_ClearAll(data->editor);

						/* Set back to the default */
						data->ss_string = FALSE;

						if (data->hidden == TRUE)
						{
							/* go back in the hide mode */
							ATB_edit_SetMode(data->editor, ED_MODE_HIDDEN);
						}

						FREE_MEMORY((void *)edit_data_buf, data->editor_data.editor_attr.size);
						win_show(data->win);
						return MFW_EVENT_CONSUMED;
//						break;   // RAVI

					case MFW_SS_SIM_UNBLCK_PIN:
						bRetVal = AUI_pin_StripSSUnblock((char *)edit_data_buf, &pin_unblock_data);

						FREE_MEMORY((void *)edit_data_buf, data->editor_data.editor_attr.size);

						if (bRetVal == FALSE)
						{
							/*
							** Clear the entered string, reset the editor and display a "failed" dialog
							*/
							ATB_edit_ClearAll(data->editor);

							/* Set back to the default */
							data->ss_string = FALSE;

							if (data->hidden == TRUE)
							{
								/* go back in the hide mode */
								ATB_edit_SetMode(data->editor, ED_MODE_HIDDEN);
							}

							dlg_initDisplayData_TextId(&display_info, TxtNull, TxtNull, TxtFailed, TxtNull, COLOUR_POPUP);
							dlg_initDisplayData_events(&display_info, NULL, THREE_SECS, 0);
							info_dialog(0, &display_info);

						}
						else
						{
							SEND_EVENT (win, E_PUK1_COMPLETE, INFO_KCD_LEFT, &pin_unblock_data);
						}
						
						return MFW_EVENT_CONSUMED;
//						break;  // RAVI

					default:
						/* xreddymn OMAPS00083495 Jul-04-2006 
						 * Display error dialog for BPM strings that are not handled.
						 */
						ATB_edit_ClearAll(data->editor);
						data->ss_string = FALSE;
						if (data->hidden == TRUE)
						{
							ATB_edit_SetMode(data->editor, ED_MODE_HIDDEN);
						}
						dlg_initDisplayData_TextId(&display_info, TxtNull, TxtNull, TxtNotAllowed, TxtNull, COLOUR_POPUP);
						dlg_initDisplayData_events(&display_info, NULL, THREE_SECS, 0);
						info_dialog(0, &display_info);
						FREE_MEMORY((void *)edit_data_buf, data->editor_data.editor_attr.size);
						break;
				}
				win_show(data->win);

			}
			else if (data->editor_data.editor_attr.text.len >= data->editor_data.min_enter)
			{
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
				{
					ATB_edit_HiddenExit(data->editor);
				}
				SEND_EVENT (win, E_PIN_DEINIT, INFO_KCD_LEFT, NULL);
			}

			break;

#ifdef COLOURDISPLAY
		/* Menu select has same effect as left soft key */
		case KCD_MNUSELECT:
#endif /* COLOURDISPLAY */

		case KCD_LEFT:
			/* Make an emergency call if an appropriate number entered */
			if ( data->emergency_call )
			{
				/* Copy the actual typed text into the buffer */
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
				{
					ATB_edit_HiddenExit(data->editor);
				}
				SEND_EVENT (win, E_PIN_DEINIT, INFO_EMERGENCY, NULL);
			}
			/* Submit PIN if sufficient characters entered */
			else if (data->editor_data.editor_attr.text.len >= data->editor_data.min_enter)
			{
				// June 16, 2005  REF: CRR 31267  x0021334
				// If PIN1/PUK1 =0, Show info dialog that SIM is blocked
				if (pin1Flag EQ TRUE)
				{
					dlg_initDisplayData_TextId(&display_info,  NULL, NULL, TxtSimBlocked, TxtDealer, COLOUR_STATUS);
					dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT | KEY_RIGHT);
					info_dialog (NULL, &display_info);
					break;
				}
				/* Left Softkey is enabled (and entered sufficient number of chars): execute it. */
				/* Copy the actual typed text into the buffer */
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
				{
					ATB_edit_HiddenExit(data->editor);
				}
				SEND_EVENT (win, E_PIN_DEINIT, INFO_KCD_LEFT, NULL);
			}
			break;

		case KCD_HUP:
		break;

		case KCD_CALL:
			/* Make an emergency call if an appropriate number entered */
			if ( data->emergency_call )
			{
				/* Copy the actual typed text into the buffer */
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
				{
					ATB_edit_HiddenExit(data->editor);
				}
				SEND_EVENT (win, E_PIN_DEINIT, INFO_EMERGENCY, NULL);
			}
		break;                    

		case KCD_RIGHT:
			if(data->editor_data.editor_attr.text.len!=0)
			{
        			//xrashmic 1 Jul, 2006 OMAPS00075784
                             //The password mode to be turned on while entering the gsm string to 
                            //unblock PUK during bootup
        			if(ATB_edit_Mode(data->editor, ED_MODE_HIDDEN) &&
                             (((data->editor_data.editor_attr.text.len == 5) && 
                             ((strncmp((char*)data->editor_data.editor_attr.text.data, "**05*", 5) == 0) ||
                             (strncmp((char*)data->editor_data.editor_attr.text.data, "**04*", 5) == 0))) ||
                             ((data->editor_data.editor_attr.text.len == 6) && 
                             ((strncmp((char*)data->editor_data.editor_attr.text.data, "**052*", 6) == 0) ||
                             (strncmp((char*)data->editor_data.editor_attr.text.data, "**042*", 6) == 0)))))
                            {
                                 ATB_edit_ResetMode(data->editor, ED_MODE_HIDDEN);
                            }

				ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
				
				/* For a SS string */
				if ( !ATB_edit_Mode(data->editor,ED_MODE_HIDDEN) )
				{
					if (data->editor_data.editor_attr.text.len==0 && data->ss_string)
					{
						data->ss_string = FALSE;
						if (data->hidden == TRUE)
						{
							/* The string should not be visible on screen */
							ATB_edit_SetMode(data->editor, ED_MODE_HIDDEN);
						}
					}
				}

				/* is it a  emergency call number ?!*/
				AUI_pin_CheckEmergency(data);
				win_show(data->win);
			}
			break;

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
			/* xreddymn OMAPS00083495 Jul-04-2006 
			 * Limit PIN1, PIN2 entries to 8 digits.
			 */
			if(((data->editor_data.Identifier==PIN1_REQ) || 
				(data->editor_data.Identifier==PIN2_REQ)) && 
				(data->editor_data.editor_attr.text.len>=8) && 
				(!data->ss_string))
				return MFW_EVENT_CONSUMED;
			else
			{
				AUI_entry_EventKey(data->entry_data, event, keyboard);
				/* is it a  emergency call number ?!*/
				AUI_pin_CheckEmergency(data);
				win_show(data->win);
			}
			break;
		}

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		AUI_pin_KbdLongCb

 $Description:	PIN editor long keypress keyboard event handler

 $Returns:		None.

 $Arguments:	event		- the keyboard event
 				keyboard 

*******************************************************************************/

static int AUI_pin_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_PIN_DATA		*data		= (T_AUI_PIN_DATA *)win_data->user;
	ED_RES				result;
	
    /* Suppress unwanted long keypresses */

	 if ( data->doNextLongPress )
		data->doNextLongPress = FALSE;       			/* mark Key_long event as consumed but do current long press */
	else
		return MFW_EVENT_CONSUMED;      		/* don't do current long press */

	switch (keyboard->code)
	{        
		case KCD_RIGHT:
			/* Long press of RSK deletes buffer */
			result = ATB_edit_ClearAll(data->editor);
			
			/* If we've deleted on an empty buffer, exit editor */
			if (result==ED_DONE)
			{
				SEND_EVENT(data->win, E_PIN_DEINIT, INFO_KCD_RIGHT, 0);
			}
			else
			{
				win_show(data->win);
			}
			break;
	}

	return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:		AUI_pin_CheckEmergency

 $Description:	Check number to see if it's an emergency number

 $Returns:		None.

 $Arguments:	data		- The editor data 

*******************************************************************************/

static UBYTE AUI_pin_CheckEmergency(T_AUI_PIN_DATA *data)
{
	UBYTE previous_emergency = data->emergency_call;
	UBYTE previous_entered = data->enteredMinChars;
	UBYTE previous_someentered = data->enteredSomeChars;
	UBYTE emergency_call;

	/*SPR#2235 - DS - If the edit mode is 'hidden' then pass
	 *the 'hidden buffer' as the parameter to cm_check_emergency.
	 *If the edit mode is not 'hidden' then pass the normal editor
	 *text buffer as the parameter.
	 */
	if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
	    emergency_call = cm_check_emergency(data->editor->hiddenText->data);
	else
	    emergency_call = cm_check_emergency(data->editor_data.editor_attr.text.data);

	if(emergency_call == TRUE)
	{
		TRACE_EVENT(" it is a emergency number !");
		data->emergency_call = TRUE;
	}
	else
	{
		data->emergency_call = FALSE;
	}

	/* Check if we've entered min_enter characters */

	if (data->editor_data.editor_attr.text.len >= data->editor_data.min_enter)
		data->enteredMinChars = TRUE;
	else
		data->enteredMinChars = FALSE;

	if (data->editor_data.editor_attr.text.len > 0)
		data->enteredSomeChars = TRUE;
	else
		data->enteredSomeChars = FALSE;
		
	/* Check if the softkeys have changed */

	if (data->emergency_call!=previous_emergency)
	{
		data->editor->update = ED_UPDATE_FULL;		/* Update softkeys, title and editor */
	}
	else if (data->enteredMinChars!=previous_entered || data->enteredSomeChars!=previous_someentered)
	{
		data->editor->update = ED_UPDATE_PARTIAL;	/* Update softkeys and editor */
	}
	else
	{
		data->editor->update = ED_UPDATE_TRIVIAL;	/* Just update editor */
	}

	return data->emergency_call;
}


/*******************************************************************************

 $Function:		AUI_pin_CheckSSString

 $Description:	Check number to see if it's an ss string

 $Returns:		None.

 $Arguments:	event		- the keyboard event
 				keyboard 

*******************************************************************************/

T_MFW_SS_RETURN AUI_pin_CheckSSString(T_AUI_PIN_DATA *data)
{
	return ss_check_ss_string(data->editor_data.editor_attr.text.data);
}


static BOOL AUI_pin_StripSSUnblock(char *cmd_string, T_AUI_PINS *pin_data)
{
	T_MFW_SS_RETURN	retVal;
	char	*rest;
	T_KSD_SEQPARAM ss_params;

	TRACE_EVENT_P2("AUI_pin_StripSSUnblock : &cmd_string = 0x%08lx, &pin_data = 0x%08lx", cmd_string, pin_data);
	
	/*
	** The incoming parameters chould not be NULL
	*/
	if ((cmd_string == NULL) ||
	     (pin_data == NULL))
		return FALSE;

	retVal = ss_decode((UBYTE *)cmd_string, &rest, &ss_params);

	if (retVal == MFW_SS_SIM_UNBLCK_PIN)
	{
		if (ss_params.ublk.ssCd == KSD_SS_PIN1)
		{
			pin_data->pin_id = MFW_SIM_PUK1;
			strcpy(pin_data->puk_code, (char *)ss_params.ublk.puk);
			strcpy(pin_data->new_pin, (char *)ss_params.ublk.pin);
			strcpy(pin_data->cnf_pin, (char *)ss_params.ublk.pin);

			return TRUE;
		}
		else
		{
			TRACE_EVENT("AUI_pin_StripSSUnblock : Not unblocking PIN1");
			return FALSE;
		}
	}
	else
	{
		TRACE_EVENT_P1("AUI_pin_StripSSUnblock : ss_decode returned %d", retVal);
		return FALSE;
	}
}

