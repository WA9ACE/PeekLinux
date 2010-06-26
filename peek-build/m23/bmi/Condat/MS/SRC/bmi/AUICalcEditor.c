/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       AUICalcEditor.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description: Editor for calculator, or generic floating-point number input



********************************************************************************

 $History: AUICalcEditor.c

 	xrashmic 20 Oct, 2005 MMI-SPR-33845
	To display BACK softkey when no more character are left in the calculator editor
	
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

#include "AUICalcEditor.h"

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
    BOOL				haveResult;			/* TRUE if a result is supplied */
    T_ATB_TEXT			result;				/* Text of result */

	BOOL				doNextLongPress;	/* Flag used to notice/not notice long keypress */
	BOOL				hasDecimalPoint;	/* Ensures only one decimal point per number */
}
T_AUI_CALC_DATA;

/* LOCAL FUNCTION PROTOTYPES */
static T_MFW_HND AUI_calc_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
static void AUI_calc_ExecCb(T_MFW_HND win, USHORT event, SHORT value, void *parameter);
static int AUI_calc_WinCb(T_MFW_EVENT event, T_MFW_WIN *win_data);
static int AUI_calc_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
static int AUI_calc_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);




/*******************************************************************************

 $Function:		AUI_calc_Start

 $Description:	Start the Calc editor.

 $Returns:		None.

 $Arguments:	None.

*******************************************************************************/

T_MFW_HND AUI_calc_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("AUI_calc_Start()");
    win = AUI_calc_Create(parent, editor_data);

    return win;
}


/*******************************************************************************

 $Function:		AUI_calc_Create

 $Description:	Create the Calc editor.
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	parent	-	The parent window.

*******************************************************************************/

static T_MFW_HND AUI_calc_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_AUI_CALC_DATA	*data = (T_AUI_CALC_DATA *)ALLOC_MEMORY(sizeof (T_AUI_CALC_DATA));
    T_MFW_WIN     		*win_data;

    TRACE_FUNCTION ("AUI_calc_Create()");

	/* Create window handler */

    data->win = win_create(parent, 0, E_WIN_VISIBLE, (T_MFW_CB)AUI_calc_WinCb);	// Create window

    if (data->win==NULL)														// Make sure window exists
    {
		return NULL;
    }

	/* Connect the dialog data to the MFW-window */
     
    data->mmi_control.dialog	= (T_DIALOG_FUNC)AUI_calc_ExecCb;				/* Setup the destination for events */
    data->mmi_control.data		= data;
    data->parent				= parent;
    win_data					= ((T_MFW_HDR *)data->win)->data;
    win_data->user				= (void *)data;

 	data->kbd					= kbd_create(data->win, KEY_ALL,(T_MFW_CB)AUI_calc_KbdCb);
	data->kbd_long				= kbd_create(data->win, KEY_ALL|KEY_LONG,(T_MFW_CB)AUI_calc_KbdLongCb); 
	data->editor				= ATB_edit_Create(&data->editor_data.editor_attr,0);

	data->editor_data = *editor_data;
	data->entry_data = AUI_entry_Create(data->win, data->editor, E_CALC_UPDATE);

	SEND_EVENT(data->win, E_CALC_INIT, 0, 0);
	
    /* Return window handle */

    return data->win;
}


/*******************************************************************************

 $Function:		AUI_calc_Destroy

 $Description:	Destroy the Calc editor.

 $Returns:		None.

 $Arguments:	window	-	The editor window.

*******************************************************************************/

void AUI_calc_Destroy(T_MFW_HND win)
{
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_CALC_DATA	*data		= (T_AUI_CALC_DATA *)win_data->user;

    if (data)
    {
		AUI_entry_Destroy(data->entry_data);
		
        win_delete (data->win);

		/* Free memory allocated to store result */

		if (data->haveResult)
		{
			FREE_MEMORY(data->result.data, data->result.len*sizeof(char));
		}

		/* Free editor memory */
				
		ATB_edit_Destroy(data->editor);
		FREE_MEMORY ((void *)data, sizeof (T_AUI_CALC_DATA));
    }
    
    return;
}


/*******************************************************************************

 $Function:    	AUI_calc_ExecCb

 $Description:	Dialog function for Calc editor.
 
 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

static void AUI_calc_ExecCb(T_MFW_HND win, USHORT event, SHORT value, void *parameter)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_CALC_DATA		*data		= (T_AUI_CALC_DATA *)win_data->user;
    T_MFW_HND			parent_win	= data->parent;
	USHORT				textIndex;
	T_ATB_TEXT			text;
	/* Store these in case editor is destroyed on callback */
    USHORT				Identifier	= data->editor_data.Identifier;
    T_AUI_EDIT_CB		Callback	= data->editor_data.Callback;
    UBYTE				destroyEditor = data->editor_data.destroyEditor;
	
    TRACE_FUNCTION ("AUI_calc_ExecCb()");

    switch (event)
    {
    	/* Initialise */
    
        case E_CALC_INIT:
        	TRACE_EVENT("E_CALC_INIT");
        	ATB_edit_Init(data->editor);

        	data->haveTitle = FALSE;
        	data->haveResult = FALSE;

			/* If we require an output line, shrink editor to fit it at bottom
			 * NB TitleString is assumed here to be a numeric string of ascii digits */

			if (data->editor_data.TitleString)
			{
				data->haveResult = TRUE;
				text.dcs = ATB_DCS_ASCII;
				text.data = data->editor_data.TitleString;
				text.len = ATB_string_Length(&text);

				data->result.data = ALLOC_MEMORY(text.len*sizeof(char));
				ATB_string_Copy(&data->result, &text);

				/* Change size of editor to fit result line in */
				data->editor_data.editor_attr.win_size.sy -= ATB_display_StringHeight(&data->result, NULL);
			}

			/* Set up title */

			if (data->editor_data.TitleId!=NULL)
			{
				data->haveTitle = TRUE;
				data->title.data = (UBYTE *)MmiRsrcGetText(data->editor_data.TitleId);
			}

			/* If title exists, get its dcs and length */
			
			if (data->haveTitle)
			{
				if (data->title.data[0]==0x80)
				{
					data->title.data += 2;		/* Skip over two unicode indicator bytes */
					data->title.dcs = ATB_DCS_UNICODE;
				}
#ifdef EASY_TEXT_ENABLED
				else if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{
					data->title.dcs = ATB_DCS_UNICODE;
				}
#endif
				else
				{
					data->title.dcs = ATB_DCS_ASCII;
				}
				
				data->title.len = ATB_string_Length(&data->title);
			}
	
        	/* Check to see if number already has decimal point */

			data->hasDecimalPoint = FALSE;
			
			for (textIndex = 0; textIndex<data->editor->attr->text.len; textIndex++)
			{
				if (ATB_string_GetChar(&data->editor->attr->text, textIndex)==UNICODE_FULLSTOP)
				{
					data->hasDecimalPoint = TRUE;
				}
			}

			/* Show the window */
			win_show(data->win);
			break;

		case E_CALC_UPDATE:
        	TRACE_EVENT("E_CALC_UPDATE");
			win_show(data->win);
			break;

		case E_CALC_DEINIT:
			TRACE_EVENT("E_CALC_DEINIT");

			if (Callback)
    	    	(Callback) (parent_win, Identifier, value);
    	    	
			if (destroyEditor)
				AUI_calc_Destroy(data->win);
				
			break;

   	}

    return;
}

/*******************************************************************************

 $Function:		AUI_calc_WinCb

 $Description:	Calc editor window event handler.

 $Returns:		None.

 $Arguments:	event	- the event
 				win		- the editor window

*******************************************************************************/

static int AUI_calc_WinCb(T_MFW_EVENT event, T_MFW_WIN *win_data)
{
    T_AUI_CALC_DATA		*data			= (T_AUI_CALC_DATA *)win_data->user;
    T_ED_DATA			*editor			= data->editor;
    T_ATB_WIN_SIZE		*win_size		= &data->editor->attr->win_size;
    T_DS_TEXTFORMAT		format;
//    USHORT				titleLen;   // RAVI

	TRACE_EVENT("AUI_calc_WinCb");
	
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

					/* TITLE */
				
					dspl_Clear(0,0, SCREEN_SIZE_X-1, win_size->py-1);
					
					if (data->haveTitle)
					{
						ATB_display_SetFormatAttr(&format, 0, FALSE);
						ATB_display_Text(0,0,&format, &data->title);
					}

					/* RESULT */

					if (data->haveResult)
					{
						resources_setHLColour(COLOUR_EDITOR);
						ATB_display_SetFormatAttr(&format, 0, FALSE);
						dspl_Clear( win_size->px, win_size->py+win_size->sy,  win_size->px+win_size->sx,
							win_size->py+win_size->sy+ATB_display_StringHeight(&data->result, &format));
						
						ATB_display_Text(win_size->px+win_size->sx-ATB_display_StringWidth(&data->result, &format),
							win_size->py+win_size->sy,&format, &data->result);
					}

					/* Display Soft Keys */
					//xrashmic 20 Oct, 2005 MMI-SPR-33845 
					//Need to display the alternate right softkey when the editor is does not contain any character other than 0.
					if (data->editor_data.editor_attr.text.len == data->editor_data.min_enter && (data->editor_data.editor_attr.text.data[0] == '0'))
					{
						/* entered less than the required number of chars: Alternate Softkey appears */
						displaySoftKeys(data->editor_data.LeftSoftKey,data->editor_data.AltRightSoftKey);
					}
					else
					{
						/* entered sufficient number of chars: Normal Softkey appears */
						displaySoftKeys(data->editor_data.LeftSoftKey, data->editor_data.RightSoftKey);
					}
				}
			}
            break;
    }

	data->editor->update = ED_UPDATE_DEFAULT;

	return MFW_EVENT_CONSUMED;      
}


/*******************************************************************************

 $Function:		AUI_calc_KbdCb

 $Description:	Calc editor keyboard event handler

 $Returns:		None.

 $Arguments:	event		- The keyboard event
 				keyboard 	- The keypress info

*******************************************************************************/

static int AUI_calc_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_CALC_DATA		*data		= (T_AUI_CALC_DATA *)win_data->user;
	ED_RES				result;
  	
	TRACE_FUNCTION("AUI_calc_KbdCb()");

   /* Suppress unwanted long keypresses */

	data->doNextLongPress = TRUE;         		/* next Key_long event is correct */
	data->editor->update = ED_UPDATE_DEFAULT;	/* Default behaviour is just to update text */

	switch (keyboard->code)
	{
#ifdef COLOURDISPLAY
		case KCD_MNUSELECT:
			if (data->editor_data.Callback)
    	    	(data->editor_data.Callback) (data->parent, data->editor_data.Identifier, INFO_KCD_SELECT);
			break;

#endif /* COLOURDISPLAY */

		case KCD_STAR:
			if (data->editor_data.Callback)
    	    	(data->editor_data.Callback) (data->parent, data->editor_data.Identifier, INFO_KCD_STAR);
			break;

		case KCD_HASH:
			/* Insert decimal point, if the number doesn't have one already */
			if (!data->hasDecimalPoint)
			{
				data->hasDecimalPoint = TRUE;
				AUI_entry_EditChar(data->entry_data, UNICODE_FULLSTOP, FALSE);
				win_show(data->win);
			}
			break;
			
		case KCD_LEFT:
			if (data->editor_data.editor_attr.text.len < data->editor_data.min_enter)
			{
				/* Entered less than the required number of chars */
				if (data->editor_data.AltLeftSoftKey!=TxtNull)
				{
					/* an alternate softkey is defined: execute it */
					SEND_EVENT (win, E_CALC_DEINIT, INFO_KCD_ALTERNATELEFT, NULL);
				}
			}
			else if (data->editor_data.LeftSoftKey!=TxtNull)
			{
				/* Left Softkey is enabled (and entered sufficient number of chars): execute it */
				SEND_EVENT (win, E_CALC_DEINIT, INFO_KCD_LEFT, NULL);
			}
			break;

		case KCD_HUP:
			SEND_EVENT(data->win, E_CALC_DEINIT, INFO_KCD_HUP, 0);
			break;              

		case KCD_RIGHT:
			/* If we're deleting the decimal point... */
			if (ATB_edit_GetCursorChar(data->editor, -1)==UNICODE_FULLSTOP)
			{
				data->hasDecimalPoint = FALSE;
			}

			/* If we've just got a zero, delete it so we can exit with next call to ATB_edit_DeleteLeft() */
			
			if (ATB_edit_GetCursorChar(data->editor, -1)==ATB_char_Unicode('0'))
			{
				ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
			}

			/* Perform normal delete */
			result = ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */

			/* If buffer is now empty, insert '0' */
			if (data->editor->attr->text.len==0 && result!=ED_DONE)
			{
				AUI_entry_EditChar(data->entry_data, ATB_char_Unicode('0'), FALSE);
			}

			/* If we've deleted on an empty buffer, exit editor */
			if (result==ED_DONE)
			{
				SEND_EVENT(data->win, E_CALC_DEINIT, INFO_KCD_RIGHT, 0);
			}
			else
			{
				win_show(data->win);
			}
			break;

		/* 0 - 9 */
		
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
			/* Remove leading zero */
			if (data->editor->attr->text.len==1 && ATB_edit_GetCursorChar(data->editor, -1)==ATB_char_Unicode('0'))
			{
				ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
			}

			/* Insert the digit */
			AUI_entry_EventKey(data->entry_data, event, keyboard);
			win_show(data->win);
			break;
	}
	
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		AUI_calc_KbdLongCb

 $Description:	Calc editor long keypress event handler

 $Returns:		None.

 $Arguments:	event		- The keyboard event
 				keyboard 	- The keypress info

*******************************************************************************/

static int AUI_calc_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_CALC_DATA		*data		= (T_AUI_CALC_DATA *)win_data->user;
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

			/* If buffer is now empty, insert '0' */
			if (data->editor->attr->text.len==0 && result!=ED_DONE)
			{
				AUI_entry_EditChar(data->entry_data, ATB_char_Unicode('0'), FALSE);
			}
			
			/* If we've deleted on an empty buffer, exit editor */
			if (result==ED_DONE)
			{
				SEND_EVENT(data->win, E_CALC_DEINIT, INFO_KCD_RIGHT, 0);
			}
			else
			{
				win_show(data->win);
			}
			break;
	}

	return MFW_EVENT_CONSUMED;
}
