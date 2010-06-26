/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       AUIEditor.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description:
	Generic MMI Editor.

********************************************************************************

 $History: AUIEditor.c

    May 15, 2006 DR: OMAPS00075852 - xreddymn
    Description: New SMS and Message Full indications will not be seen by user
    when CB is enabled, because CB messages have higher priority.
    Solution: CB messages and New SMS or Message Full indications are displayed
    in two different zones on the Idle screen.
    Changes: Added new display zone which is located one line above
    the softkeys.

 	Mar 14, 2006 DR: OMAPS00061468 - x0035544.
   	Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
   	Solution : In the function AUI_edit_SetDefault() set the default value for 
   	display_type to SAT_ICON_NONE.

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

    Dec-20-2004 MMI-SPR-27382 xreddymn
    Added NULL window handle check for AUI_edit_Destroy

 	Sep 2, 2004    REF: CRR 21370 xkundadu
	Description: Giving wrong expected result by long press on "#" key
	Fix:	When user presses long on '#' key it will display 'p'.
		Changed the long press of '*' key to display 'w'.

	 Jul 22,2004 CRR:21605 xrashmic - SASKEN
	 Description: After deleting all the characters in the editor the case does not change to 
	 sentence case.
	 Fix: After deleting a character, check if editor is empty and then set the case to
	 sentence case only if the user has not modified the case.


 	Jul 29, 2004         REF: CRR:20892 xrashmic - SASKEN
	Description: By Long press on Left/Right key at 'Message Editor' screen, 
	cursor not scrolling continuously
	Fix: The Long press of left/right key is now handled in AUI_edit_KbdLongCB.
	The continuous long press of a key was being ignored for all keys. Now
	only for left and right keys this is handled, for rest of the keys the continuos 
	long press is ignored.

 
	31/01/02      Original Condat(UK) BMI version.
	
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
#include "mfw_edt.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"

#include "MmiDummy.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiIdle.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "Mmiicons.h"
#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "mfw_ffs.h"
#ifdef EASY_TEXT_ENABLED
#include "MmiDictionary.h"
#endif
#include "mmiColours.h"

#include "MmiDictionary.h"
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUITextEntry.h"
#include "AUIEditor.h"
#include "AUIEditor_i.h"
#include "AUISymbol.h"

/* Mar 08, 2006    REF:ER OMAPS00065168  */
#ifdef NEPTUNE_BOARD
#include "audio.h"
#endif



#ifndef MFW_EVENT_PASSED
#define MFW_EVENT_PASSED 0
#endif

#define TRACE_AUIEditor			// Def/undef this to show/hide the trace_events for this file

static void AUI_edit_ClearPredText(T_AUI_EDITOR_INFO *data);

/* API - 08/07/03 - CQ10728*/
static void AUI_Edit_Display_Only_CB(T_MFW_HND win, USHORT Identifier, SHORT reason);
static void AUI_Edit_Display_Only_Destroy (T_MFW_HND window);
/* API - 08/07/03 - CQ10728 END*/


/*******************************************************************************

 Static Data

********************************************************************************/

/* eZiText icon */

static const unsigned char TextIconeZiTextSmall[] =
{
  0x03,0x03,0x18,0x7c,0xcc,0x08,0x10,0x0c
};

 // Jul 22,2004 CRR:21605 xrashmic - SASKEN
 // This variable would tell us if the user had changed the case explicitly
UBYTE CaseChanged = FALSE;

#define ED_IND_UPPER "ABC"
#define ED_IND_LOWER "abc"
#define ED_IND_CAPS "Abc"
#define ED_IND_NUM "123"
#define ED_IND_NONE "---"


/*******************************************************************************

 $Function:		AUI_edit_Start

 $Description:	Start the editor.

 $Returns:		None.

 $Arguments:	None.

*******************************************************************************/

T_MFW_HND AUI_edit_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("AUI_edit_Start()");

    win = AUI_edit_Create(parent, editor_data);

    return win;
}


/*******************************************************************************

 $Function:		AUI_edit_QuickStart

 $Description:	Start the editor.

 $Returns:		Dialog handler for the editor.

 $Arguments:	win			- the parent window
 				TitleId		- the text id of the title (or NULL)
 				TitleString	- the text string of the title (or NULL)
 				buffer		- the unicode input buffer
 				len			- maximum length of the text entered
 				editor_info	- the T_AUI_EDITOR_DATA structure (allocated but empty)

*******************************************************************************/

T_MFW_HND AUI_edit_QuickStart(T_MFW_HND win, USHORT TitleId, UBYTE *TitleString, USHORT *buffer, USHORT len,
	T_AUI_EDITOR_DATA *editor_info, T_AUI_EDIT_CB editor_cb)
{

    TRACE_FUNCTION ("AUI_edit_QuickStart()");

	AUI_edit_SetDisplay(editor_info, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetEvents(editor_info, 0, TRUE, FOREVER, editor_cb);
	AUI_edit_SetBuffer(editor_info, ATB_DCS_UNICODE, (UBYTE *)buffer, len);
	AUI_edit_SetTextStr(editor_info, TxtSoftOK, TxtDelete, TitleId, TitleString);
	AUI_edit_SetAltTextStr(editor_info, 0, NULL, TRUE, TxtSoftBack);
    /* create the dialog handler */
    return AUI_edit_Start(win, editor_info);  /* start the common editor */ 
}


/*******************************************************************************

 $Function:		AUI_edit_Create

 $Description:	Create the editor.
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	parent	-	The parent window.

*******************************************************************************/

static T_MFW_HND AUI_edit_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data)
{
    T_AUI_EDITOR_INFO	*data = (T_AUI_EDITOR_INFO *)ALLOC_MEMORY (sizeof (T_AUI_EDITOR_INFO));
    T_MFW_WIN     		*win_data;

    TRACE_FUNCTION ("AUI_edit_Create()");

	/* Create window handler */

    data->win = win_create(parent, 0, E_WIN_VISIBLE, (T_MFW_CB)AUI_edit_WinCb);	// Create window

    if (data->win==NULL)														// Make sure window exists
    {
		return NULL;
    }

	/* Connect the dialog data to the MFW-window */
     
    data->mmi_control.dialog	= (T_DIALOG_FUNC)AUI_edit_ExecCb;				/* Setup the destination for events */
    data->mmi_control.data		= data;
    data->parent				= parent;
    win_data					= ((T_MFW_HDR *)data->win)->data;
    win_data->user				= (void *)data;

 	data->kbd					= kbd_create(data->win, KEY_ALL,(T_MFW_CB)AUI_edit_KbdCb);
	data->kbd_long				= kbd_create(data->win, KEY_ALL|KEY_LONG,(T_MFW_CB)AUI_edit_KbdLongCb);
	data->editor				= ATB_edit_Create(&data->editor_data.editor_attr,0);

	data->editor_data = *editor_data;
	
	 // Jul 22,2004 CRR:21605 xrashmic - SASKEN
	 // The initial value of this variable should be false. i.e the user has still not changed the case.
	CaseChanged=FALSE;

	AUI_edit_Init(data->win);

	SEND_EVENT(data->win, E_ED_INIT, 0, 0);
	
    /* Return window handle */

    return data->win;
}


/*******************************************************************************

 $Function:		AUI_edit_Init

 $Description:	Initialise the editor.
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	win - The editor window

*******************************************************************************/

static void AUI_edit_Init(T_MFW_HND win)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;
    T_ATB_TEXT			text={0,0,NULL};
	
	TRACE_FUNCTION("AUI_edit_Init()");

	data->editor->update = ED_UPDATE_FULL;
	data->predText.len = 0;					/* Blank out predictive text buffer */

	/* Set up title information */

	data->hasTitle	= FALSE;
	data->title.len = 0;						/* Blank out title buffer */
	
	if (data->editor_data.TitleId!=NULL)
	{
		data->hasTitle = TRUE;
		text.data = (UBYTE *)GET_TEXT(data->editor_data.TitleId);
	}
	else if (data->editor_data.TitleString!=NULL)
	{
		data->hasTitle = TRUE;
		text.data = data->editor_data.TitleString;
	}

	/* If title exists, get its dcs and length */
	
	if (data->hasTitle)
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
		data->title.data = (UBYTE *)ALLOC_MEMORY((text.len + 1)*ATB_string_Size(&text));
		
		ATB_string_Copy(&data->title, &text);
	}
	
    /* Set up correct parameters for easytext */
    
#ifdef EASY_TEXT_ENABLED
	if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA))
	{
		/* Allocate word buffer memory */

		data->predText.data = (UBYTE *)ALLOC_MEMORY((ED_PREDTEXT_MAX+1)*sizeof(UBYTE));
		data->predText.dcs = ATB_DCS_ASCII;
		data->predText.len = 0;
	
		FFS_flashData.PredTextAvailable = TRUE;
		Initialize_Dict(Mmi_getCurrentLanguage()/*SPR1508*/, 0);
		ResetDictSearch();
	}
#endif

	/* Initialise editor */
	
	ATB_edit_Init(data->editor);

	/* Initialise text entry */
	
	data->entry_data = AUI_entry_Create(data->win, data->editor, E_ED_UPDATE);
		
	/* If there's a timeout, create the timer */
	
    if (data->editor_data.timeout && (data->editor_data.timeout != FOREVER))
    {
        data->timer = tim_create(data->win, data->editor_data.timeout, (T_MFW_CB)AUI_edit_TimerCb);
        tim_start(data->timer);
    }
    else
    {
        data->timer = NULL;
    }

    /* Title timer, if required, is set up in WinCb */

	data->title_timer = NULL;
    
    return;
}


/*******************************************************************************

 $Function:		AUI_edit_Destroy

 $Description:	Destroy the editor.

 $Returns:		None.

 $Arguments:	window	-	The editor window.

*******************************************************************************/

void AUI_edit_Destroy(T_MFW_HND win)
{
	T_MFW_WIN			*win_data;
    T_AUI_EDITOR_INFO	*data;

#ifdef TRACE_AUIEditor
    TRACE_FUNCTION ("AUI_edit_Destroy()");
#endif

	if (win == NULL)
	{
#ifdef TRACE_AUIEditor
		TRACE_EVENT ("Error : Called with NULL Pointer");
#endif
		return;
	}
	// xreddymn Dec-21-2004 MMI-SPR-27382: Added NULL window handle check for AUI_edit_Destroy
	win_data	= ((T_MFW_HDR *)win)->data;
	if(win_data == NULL) return;
	data		= (T_AUI_EDITOR_INFO *)win_data->user;
    if (data)
    {
    	/* Free memory allocated for easytext buffer */
    	
#ifdef EASY_TEXT_ENABLED
		if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA) && data->predText.data )
		{
			TRACE_EVENT("Freeing predictive text buffer");
			FREE_MEMORY((void *)data->predText.data, (ED_PREDTEXT_MAX+1)*sizeof(UBYTE));
		}
#endif

		/* Free memory allocated for title */

		if (data->hasTitle)
		{
			FREE_MEMORY(data->title.data, (data->title.len + 1)*ATB_string_Size(&data->title));
		}

		/* Delete timers */

		if (data->timer)
		{
			tim_delete(data->timer);
			data->timer = NULL;
		}
		if (data->title_timer)
		{
			tim_delete(data->title_timer);
			data->title_timer = NULL;
		}
		
        win_delete (data->win);

		/* Free editor memory */
		
		AUI_entry_Destroy(data->entry_data);
		ATB_edit_Destroy(data->editor);
		FREE_MEMORY ((void *)data, sizeof (T_AUI_EDITOR_INFO));
    }
    
    return;
}


/*******************************************************************************

 $Function:    	AUI_edit_ExecCb

 $Description:	Dialog function for editor.
 
 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

static void AUI_edit_ExecCb(T_MFW_HND win, USHORT event, USHORT value, void *parameter)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_EDITOR_INFO	*data      = (T_AUI_EDITOR_INFO *)win_data->user;

    T_MFW_HND			parent_win	= data->parent;
    USHORT				alphachar;
 	/* Store these in case editor is destroyed on callback */
    USHORT				Identifier	= data->editor_data.Identifier;
    T_AUI_EDIT_CB		Callback	= data->editor_data.Callback;
    UBYTE				destroyEditor = data->editor_data.destroyEditor;

/* Mar 08, 2006    REF:ER OMAPS00065168  */
#ifdef NEPTUNE_BOARD
    UBYTE ringer;
#endif

#ifdef TRACE_AUIEditor
    TRACE_FUNCTION ("AUI_edit_ExecCb()");
#endif

    switch (event)
    {
    	/* Initialise */
    
        case E_ED_INIT:
        	TRACE_EVENT("E_ED_INIT");
			win_show(data->win);
			break;

      	/* Destroy the editor */
      	
      	case E_ED_DESTROY:
			AUI_edit_Destroy(data->win);   
      		break;
      		
      	/* De-initialise editor */
        case E_ED_DEINIT:
         	TRACE_EVENT("E_ED_DEINIT");

/* Mar 08, 2006    REF:ER OMAPS00065168  */
/* Stop playing ring tone on key press */
    #ifdef NEPTUNE_BOARD
            ringer = getcurrentSMSTone();
            audio_StopSoundbyID( AUDIO_BUZZER, ringer );

            vsi_t_sleep(0,10); /* small delay */
    #endif 

	        if (Callback)
    	    	(Callback) (parent_win, Identifier, value);

			if(destroyEditor)
            	AUI_edit_Destroy(data->win);   
            break;

		/* Insert a character */
		
		case E_ED_INSERT:
			alphachar = (USHORT)value;
			/* If value is set, insert character */
			if (alphachar!=NULL)
			{
	            ATB_edit_Char(data->editor, alphachar, TRUE);
#ifdef EASY_TEXT_ENABLED
				if(FFS_flashData.PredTextAvailable && FFS_flashData.PredTextSelected && ATB_edit_Mode(data->editor, ED_MODE_PREDTEXT))
				{
		            if ((alphachar == UNICODE_FULLSTOP) ||(alphachar == UNICODE_EXCLAMATION)
		            	|| (alphachar == UNICODE_QUESTION))
		        	{
		             	ATB_edit_Char(data->editor, UNICODE_SPACE, TRUE);
		        	}
				}
#endif
			}
			else
			/* If parameter is set, insert string */
			if (parameter!=NULL)
			{
				ATB_edit_InsertString(data->editor, (T_ATB_TEXT *)parameter);
			}
			win_show(data->win);
			break;

		/* Update word wrap & redraw editor window */
		
		case E_ED_UPDATE:
			//ATB_edit_Refresh(data->editor); /* SPR#2342 - SH */
			win_show(data->win);
   			break;

        default:
		break;
    }
    return;
}


/*******************************************************************************

 $Function:		AUI_edit_WinCb

 $Description:	Editor window event handler.

 $Returns:		None.

 $Arguments:	event	- the event
 				win		- the editor window

*******************************************************************************/

static int AUI_edit_WinCb(T_MFW_EVENT event, T_MFW_WIN *win_data)
{
    T_AUI_EDITOR_INFO	*data			= (T_AUI_EDITOR_INFO *)win_data->user;
    T_ED_DATA			*editor			= data->editor;
	USHORT				scrollBarSize;
	USHORT				scrollBarPos;
	SHORT				editX			= editor->attr->win_size.px;
	SHORT				editY			= editor->attr->win_size.py;
	SHORT				editWidth		= editor->attr->win_size.sx;
	SHORT				editHeight		= editor->attr->win_size.sy;
	char				lengthIndC[12];

	/* Strings for the header */
	T_ATB_TEXT			string;
	USHORT				stringWidth;
	USHORT				stringHeight;
	T_ATB_TEXT			title;
	USHORT				titleWidth;
	USHORT				titleHeight;

	USHORT				textLen;
	
	

	
	T_DS_TEXTFORMAT  	format;
	BOOL				showIcons;				/* Flag that stores whether icons (e.g. alignment icons) should be shown */
	
	USHORT				leftSK;
	USHORT				rightSK;
	USHORT				textIndex;
	T_ATB_TEXT			newTitle;
	USHORT				lastSpace;
	USHORT				character;
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	SHORT				titleIconWidth = 0;
	SHORT				iconX;
	SHORT				iconY;
#endif
	
#ifdef TRACE_AUIEditor
    TRACE_FUNCTION ("AUI_edit_WinCb()");
#endif

    if (!data)
		return MFW_EVENT_CONSUMED;
  
    switch(event)
    {
        case E_WIN_VISIBLE: 											/* window is visible */
            if (win_data->flags & E_WIN_VISIBLE)
			{
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
				if (editor->attr->TitleIcon.data != NULL)
				{
					if (editor->attr->TitleIcon.width > TITLE_ICON_WIDTH) 
					{
						titleIconWidth = TITLE_ICON_WIDTH;
					}
					else
					{
						titleIconWidth = editor->attr->TitleIcon.width;
					}
				}			
				else
				{
					titleIconWidth = 0;
				}
#endif

				textLen = data->editor->attr->text.len;
				ATB_edit_LineGet(data->editor, data->editor->winStartLine);
				

				if (editor->update==ED_UPDATE_TRIVIAL)
                                editor->update = ED_UPDATE_DEFAULT;

				TRACE_EVENT("Updating whole screen.");
				
				
				

				/* Show the edited text */
				/* Don't update editor text if in ED_UPDATE_PARTIAL */
				
				if (editor->update!=ED_UPDATE_PARTIAL)
				{
					ATB_edit_Show(data->editor);
				}
				
				/* Set the colour for drawing title and scrollbar */

				resources_setTitleColour(COLOUR_EDITOR);

				/* TITLE */

				if (data->editor_data.zone_id & ZONE_TITLE)
				{
					dspl_Clear(0,0, SCREEN_SIZE_X-1, editY-1);
				
					/* Display the title, if it exists */

					string.len = 0;
					string.dcs = ATB_DCS_ASCII;
					title.len = 0;
					title.dcs = ATB_DCS_ASCII;
		
					if (data->hasTitle)
					{
						title.data = data->title.data;
						title.len = data->title.len;
						title.dcs = data->title.dcs;
					}

					/* Uppercase/lowercase indicator */

					if (!ATB_edit_Mode(editor, ED_MODE_READONLY))
					{
						switch(ATB_edit_GetCasePref(editor))
						{
							case ED_CASEPREF_ALPHA_UC:
							case ED_CASEPREF_ALPHANUM_UC:
								string.data = (UBYTE *)ED_IND_UPPER;
								break;

							case ED_CASEPREF_ALPHA_LC:
							case ED_CASEPREF_ALPHANUM_LC:
								string.data = (UBYTE *)ED_IND_LOWER;
								break;
							
							case ED_CASEPREF_ALPHANUM:
								if (ATB_edit_GetCase(editor)==ED_CASE_UPPER)
									string.data = (UBYTE *)ED_IND_UPPER;
								else if (ATB_edit_GetCase(editor)==ED_CASE_LOWER)
									string.data = (UBYTE *)ED_IND_LOWER;
								else if (ATB_edit_GetCase(editor)==ED_CASE_CAPS)
									string.data = (UBYTE *)ED_IND_CAPS;
								else
									string.data = (UBYTE *)ED_IND_NUM;
								break;
								
							case ED_CASEPREF_NUM:
								string.data = (UBYTE *)ED_IND_NUM;
								break;

							default:
								string.data = (UBYTE *)ED_IND_NONE;
								break;
						}

						string.len = ATB_string_Length(&string);
						
#ifdef EASY_TEXT_ENABLED
						/* Display predicted word in header */

						if(ATB_edit_Mode(editor, ED_MODE_PREDTEXT) && FFS_flashData.PredTextSelected)
						{
							if (ATB_edit_CapitaliseWord(data->editor))	/* Word is capitalised */
								string.data = (UBYTE *)ED_IND_CAPS;
							else
								string.data = (UBYTE *)ED_IND_LOWER;
							string.len = ATB_string_Length(&string);
						
							if(data->predText.len!=0)
							{
								title.data = data->predText.data;
								title.len = data->predText.len;
								title.dcs = data->predText.dcs;
		                    }
						}
#endif
						if (ATB_edit_Mode(editor, ED_MODE_ALPHA) && title.len==0)
						{
							/* SPR#1995 - SH - 'size' includes null terminator, so
							 * remaining characters is size-1 */
#ifdef LSCREEN
							sprintf(lengthIndC, "%d/%d", textLen, editor->attr->size-1);
#else
							sprintf(lengthIndC, "%d", editor->attr->size-textLen-1);
#endif
								
							title.data = (UBYTE *)lengthIndC;
							title.len = ATB_string_Length(&title);
						}
					}

					/* Draw the string to the right of the editor - the case/format indicator, and icons (if appropriate) */

					stringWidth = 0;
					stringHeight = 0;
					showIcons = FALSE;
					
					if (string.len)
					{
					TRACE_EVENT("Displaying string on right");
						ATB_display_SetFormatAttr(&format, 0, FALSE);	/* Set format to format at cursor */						
						stringWidth = ATB_display_StringWidth(&string, &format)+2;
						stringHeight = ATB_display_StringHeight(&string, &format);
						ATB_display_Text(SCREEN_SIZE_X-stringWidth+2,0, &format, &string);
						showIcons = TRUE;				/* If showing case, show icons */
					}

#ifdef EASY_TEXT_ENABLED
					/* Display ezitext icon to the left of the case indicator */

					if(ATB_edit_Mode(editor, ED_MODE_PREDTEXT) && FFS_flashData.PredTextSelected == TRUE)
					{
						if (data->predText.len==0)		// Show icons only if there's no predicted text entry currently
							showIcons = TRUE;
						else
							showIcons = FALSE;
					}
#endif

					/* Show icons to the left of the case indicator */
					
					if (showIcons)
					{
						
#ifdef EASY_TEXT_ENABLED
						/*  Display ezitext icon to the left of the case indicator */
						if(ATB_edit_Mode(editor, ED_MODE_PREDTEXT) && FFS_flashData.PredTextSelected == TRUE)
						{
							stringWidth += 8;
							dspl_BitBlt(SCREEN_SIZE_X-stringWidth+2,
								0,8,8,0,(char*)TextIconeZiTextSmall,0);
						}
#endif

						if (stringHeight<Mmi_layout_line_height())
							stringHeight = Mmi_layout_line_height();
					}
					

					/* Draw the string to the left of the screen - the editor title, or the number of characters
					 * remaining free in the buffer, or the predicted text word. */

					titleWidth = 0;
					titleHeight = 0;
					
					if (title.len)
					{
						ATB_display_SetFormatAttr(&format, 0, FALSE);				/* Clear format */

						titleWidth = ATB_display_StringWidth(&title, &format)+2;
						titleHeight = ATB_display_StringHeight(&title, &format);

						/* Check to see if title fits into top line - if it doesn't,
						 * we will need display a section of it. */

						if (titleWidth>(SCREEN_SIZE_X-stringWidth-2))
						{
							/* Create timer if it doesn't exist */

							if (!data->title_timer)
							{
								data->title_timer = tim_create(data->win, 3000, (T_MFW_CB)AUI_edit_TitleTimerCb);
								data->title_pos = 0;
								data->title_next_pos = 0;
						        tim_start(data->title_timer);
						    }
						    
							titleWidth = 0;
							textIndex = data->title_pos;
							lastSpace = 0;

							while (titleWidth<(SCREEN_SIZE_X-stringWidth-2) && textIndex<title.len)
							{
								character = ATB_string_GetChar(&title, textIndex);
								titleWidth+=ATB_display_GetCharWidth(character, &format);
								textIndex++;
								if (character==UNICODE_SPACE)
									lastSpace = textIndex;
							}
								
							TRACE_EVENT_P1("Characters that fit: %d", textIndex);

							/* Calculate next start position */

							if (textIndex==title.len)
								data->title_next_pos = 0;
							else
							{
								/* Word wrap to last space, if there was one */
								
								if (lastSpace>0)
									textIndex = lastSpace;
								data->title_next_pos = textIndex;
							}

							TRACE_EVENT_P1("Next position: %d", data->title_next_pos);
								
							newTitle.len = textIndex-data->title_pos;
							newTitle.data = (UBYTE *)ALLOC_MEMORY((newTitle.len+1)*ATB_string_Size(&title));
							newTitle.dcs = title.dcs;

							for (textIndex=0; textIndex<newTitle.len; textIndex++)
							{
								ATB_string_SetChar(&newTitle, textIndex, ATB_string_GetChar(&title, data->title_pos+textIndex));
							}
							ATB_string_SetChar(&newTitle, newTitle.len, UNICODE_EOLN);
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
							/* Check if the icon is self-explanatory, GSM 11.14, Sec 6.5.4, 12.32.
								If the icon is to displayed, move the text by icon width. */
							if (editor->attr->TitleIcon.selfExplanatory == FALSE)
								ATB_display_Text(titleIconWidth+1,0,&format, &newTitle);
#else
							ATB_display_Text(0,0,&format, &newTitle);
#endif
							FREE_MEMORY((UBYTE *)newTitle.data, (newTitle.len+1)*ATB_string_Size(&title));
						}
						else
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
						{
							/* Check if the icon is self-explanatory, GSM 11.14, Sec 6.5.4, 12.32.
								If the icon is to displayed, move the text by icon width. */
							if (editor->attr->TitleIcon.selfExplanatory == FALSE)
								ATB_display_Text(titleIconWidth+1,0,&format, &title);
						}
#else
							ATB_display_Text(0,0,&format, &title);
#endif

#ifdef EASY_TEXT_ENABLED
						/* Provides a cursor in the predicted word, so the user can tell which of the characters
						 * entered are being used to select the word. */
						 
						if(ATB_edit_Mode(editor, ED_MODE_PREDTEXT) && FFS_flashData.PredTextSelected && data->predText.len!=0)
						{
							ATB_display_Cursor(&data->predText, data->predTextChar.pos, ED_CURSOR_BLOCK,
								data->predTextChar.lineWidth, 0, data->predTextChar.width, data->predTextChar.height);
	                    }
#endif
					}

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
					if(editor->attr->TitleIcon.data != NULL)
					{

						if (titleHeight == 0)
							titleHeight = TITLE_ICON_HEIGHT;

						/* Display '?' icon if the size is bigger, 
							else display the icon*/
						if ((editor->attr->TitleIcon.width > TITLE_ICON_WIDTH) || (editor->attr->TitleIcon.height > TITLE_ICON_HEIGHT))
						{
							/* iconY calculates place hoilder to place the icon 
								at the center of the screen. */
							   iconX = 1;
							   iconY = 1+ ((titleHeight-2) / 2) - (10 / 2);
							   dspl_BitBlt2(iconX, iconY, 8, 
									10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);
						}
						else
						{
							/* iconY calculates place hoilder to place the icon 
								at the center of the screen. */
							iconX = 1;
							iconY = 1+ ((titleHeight-2) / 2) - (editor->attr->TitleIcon.height / 2);
							dspl_BitBlt2(iconX, iconY, editor->attr->TitleIcon.width, editor->attr->TitleIcon.height, 
								(void*)editor->attr->TitleIcon.data, 0, BMP_FORMAT_256_COLOUR);
						}
					}
#endif					

					/* Draw line on top */
					
					if (stringHeight>titleHeight)
						titleHeight = stringHeight;

	/* Non colour display - draw line between title texts*/
#ifndef LSCREEN
					if (titleHeight>0)
					{
						if (stringWidth==0)
							stringWidth = 1;					/* Make sure end of line is on-screen */
						dspl_DrawLine(titleWidth, titleHeight/2, SCREEN_SIZE_X-stringWidth, titleHeight/2); /* Line between title & indicator */
					}
#endif
				}

				/* SCROLLBAR */
				
				if (data->editor_data.zone_id & ZONE_SCROLLBAR)
				{
					dspl_Clear(editX+editWidth, editY, editX+editWidth+ED_SCROLLBAR_WIDTH-1, editY+editHeight-1);
				
					/* Vertical scrollbar - only show if editor is taller than view size */

					if (editor->totalHeight > 0 && editor->viewHeight<editor->totalHeight)
					{
						dspl_DrawLine(editX+editWidth, editY, editX+editWidth, editY+editHeight-1);
						scrollBarSize = editor->viewHeight * editHeight / editor->totalHeight;
						if (scrollBarSize>editHeight)
							scrollBarSize = editHeight;
						scrollBarPos = editor->viewStartPos * editHeight / editor->totalHeight;

						dspl_DrawLine(editX+editWidth+1, editY+scrollBarPos, editX+editWidth+1,
							editY+scrollBarPos+scrollBarSize);
						dspl_DrawLine(editX+editWidth+2, editY+scrollBarPos, editX+editWidth+2,
							editY+scrollBarPos+scrollBarSize);
					}
				}

				/* SOFTKEYS */

				if (data->editor_data.zone_id & ZONE_SOFTKEYS)
				{
#ifdef EASY_TEXT_ENABLED
					/* If a word is being entered, display OK and Delete */

					if (ATB_edit_Mode(data->editor, ED_MODE_PREDTEXT) && data->predText.len!=0 && FFS_flashData.PredTextSelected == TRUE)
					{
						leftSK = TxtSoftOK;
						rightSK = TxtDelete;
					}
					else
#endif
					{
					/* Entered less than the required number of chars: Alternate Left Softkey appears */
						if (textLen < data->editor_data.min_enter)
						{
							leftSK = data->editor_data.AltLeftSoftKey;
						}
						else
						{
							leftSK = data->editor_data.LeftSoftKey;
						}
					/* Buffer is empty and alternative rsk enabled: alternative rsk appears */
						if (textLen==0 && data->editor_data.change_rsk_on_empty)
						{
							rightSK = data->editor_data.AltRightSoftKey;
						}
						else
						{
							rightSK = data->editor_data.RightSoftKey;
						}
	                }
	                displaySoftKeys(leftSK, rightSK);
	        	}

	        	/* Finished drawing screen */
			}
            break;
            
        default:
            return MFW_EVENT_PASSED;
//            break;  // RAVI
    }

	data->editor->update = ED_UPDATE_DEFAULT;
	
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		AUI_edit_KbdCb

 $Description:	Editor keyboard event handler

 $Returns:		None.

 $Arguments:	event		- the keyboard event
 				keyboard 

*******************************************************************************/

static int AUI_edit_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;
#if (BOARD != 61)	&& defined(EASY_TEXT_ENABLED)
	T_ATB_TEXT			*PredText	= &data->predText;
	USHORT				character;
#endif	
	ED_RES				result;
  	
	TRACE_FUNCTION("AUI_edit_KbdCb()");
	
    /* Suppress unwanted long keypresses */

	data->doNextLongPress = TRUE;         		/* next Key_long event is correct */

	/* Restart the timer */
	
    if (data->timer !=NULL)
	{
    	tim_stop(data->timer);
    	tim_start(data->timer);
	}

#ifdef EASY_TEXT_ENABLED
	if (FFS_flashData.PredTextSelected==FALSE || !ATB_edit_Mode(data->editor, ED_MODE_PREDTEXT))
	{
	/* Predictive text is off */
#endif

	switch (keyboard->code)
	{
		/* UP key */
		
		case KCD_MNUUP:
			/* For DSample, up and down move cursor up and down a line in all modes */
#ifdef LSCREEN
			ATB_edit_MoveCursor(data->editor, ctrlUp, TRUE);

			/* For other samples, up and down move left and right when not in read only mode */
#else
			if (ATB_edit_Mode(data->editor, ED_MODE_READONLY))		/* Acts as up key in read-only mode */
			{
				ATB_edit_MoveCursor(data->editor, ctrlUp, TRUE);
			}
      		else
    		{
				ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
			}
#endif
			data->editor->update = ED_UPDATE_TRIVIAL;		/* This flag speeds up update */
			win_show(data->win);
			break;

		/* DOWN key */
		
		case KCD_MNUDOWN:
			/* For DSample, up and down move cursor up and down a line in all modes */
#ifdef LSCREEN
			ATB_edit_MoveCursor(data->editor, ctrlDown, TRUE);

			/* For other samples, up and down move left and right when not in read only mode */
#else
			if (ATB_edit_Mode(data->editor, ED_MODE_READONLY))		/* Act as down key in read-only mode */
			{
				ATB_edit_MoveCursor(data->editor, ctrlDown, TRUE);
			}
      		else
    		{
        		ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);		/* Otherwise, move cursor left */
      		}
#endif
			data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
			win_show(data->win);
			break;

		/* For DSample, left and right keys move cursor left and right */
#ifdef LSCREEN

		/* MENU LEFT */
		
		case KCD_MNULEFT:
			ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);		/* Move cursor left */
			data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
			win_show(data->win);
			break;

		/* MENU RIGHT */
		
		case KCD_MNURIGHT:
			ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);		/* Move cursor right */
			data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
			win_show(data->win);
			break;
#endif

		/* LEFT SOFT KEY */
		
		case KCD_LEFT:
			if (data->editor_data.editor_attr.text.len < data->editor_data.min_enter)
			{
				/* Entered less than the required number of chars */
				if (data->editor_data.AltLeftSoftKey!=TxtNull)
				{
					/* an alternate softkey is defined: execute it */
					SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_ALTERNATELEFT, NULL);
				}
			}
			else if (data->editor_data.LeftSoftKey!=TxtNull)
			{
				/* Left Softkey is enabled (and entered sufficient number of chars): execute it
				 * get the orginal text back into the editor instead of the hide buffer ( only filled with '*') */
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))
				{
					ATB_edit_HiddenExit(data->editor);
				}
				SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_LEFT, NULL);
			}
			break;

		/* HANGUP key */

        case KCD_HUP:
            if (data->editor_data.RightSoftKey NEQ TxtNull)						/* Right softkey is enabled: execute it */
			{
				SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_HUP, NULL);		/* Deinitialise the editor */
            }
			break;

		/* SEND KEY */

		case KCD_CALL:
			if (data->editor_data.editor_attr.text.len >= data->editor_data.min_enter)
			{
				/* Send this event to generate an MO call in SMS editor */
				SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_CALL, NULL);
			}
			break;
		
		/* RIGHT SOFT KEY */

	    case KCD_RIGHT:
	    TRACE_EVENT("KCD_RIGHT");
	        if (ATB_edit_Mode(data->editor, ED_MODE_READONLY)) 					/* in read only mode, */
	        {
	        	TRACE_EVENT("SENDING DEINIT EVENT");
	        	SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_RIGHT, NULL);		/* just deinitialise editor */
	        }
	        else															/* otherwise, RSK acts as clear key */
            {
            	/* If multitap is in progress, delete current character
				 * rather than previous character */

				if (data->editor->multitap)
				{
					AUI_entry_StopMultitap(data->entry_data); /* Cancel multitap */
					result = ATB_edit_DeleteRight(data->editor, TRUE); /* SPR#2342 - SH */
				}
				else
				{
	            	result = ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
				}
				
	            /* If we delete from first character in the editor, exit editor */
	            if (result==ED_DONE)
	            {
					SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_RIGHT, NULL);
	            }
	            else
	            {
		            win_show(data->win);
	            }
            }
			break;

		/* Switch text mode: uppercase, lowercase */
		case KCD_HASH:
			if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA))
			{				
				 // Jul 22,2004 CRR:21605 xrashmic - SASKEN
				 // The user has explicitly changed the case. When
				 // the editor becomes empty, we should not reset the 
				 // case to sentence case
				CaseChanged=TRUE;
				switch(ATB_edit_GetCase(data->editor))
				{
					case ED_CASE_LOWER:
						 // Jul 22,2004 CRR:21605 xrashmic - SASKEN
						 // The user has changed the case to sentence case.
						 // when the editor becomes empty, we have to reset 
						 // the case to sentence case.
						CaseChanged=FALSE;
						ATB_edit_SetCase(data->editor, ED_CASE_CAPS);
						break;
					case ED_CASE_CAPS:
						ATB_edit_SetCase(data->editor, ED_CASE_UPPER);
						break;
					case ED_CASE_UPPER:
						ATB_edit_SetCase(data->editor, ED_CASE_LOWER);

						/* If easy text selected, switch to it */
#ifdef EASY_TEXT_ENABLED
  						if (ATB_edit_Mode(data->editor, ED_MODE_PREDTEXT) && FFS_flashData.PredTextAvailable)
						{
							FFS_flashData.PredTextSelected = TRUE;
						}
#endif
						break;
					case ED_CASE_NUM:
						ATB_edit_SetCase(data->editor, ED_CASE_LOWER);
						break;
				}
			}
			else
			{
				/* SPR#1788 - SH - Forbid '#' entry in read only mode */
			 if (!ATB_edit_Mode(data->editor, ED_MODE_READONLY))
				AUI_entry_EventKey(data->entry_data, event, keyboard);
			}

			win_show(data->win);
			break;

		case (KCD_STAR):
			/* If we're in apha mode and not in read-only mode, display the symbol screen */	
			if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA) && !ATB_edit_Mode(data->editor, ED_MODE_READONLY))
			{
				AUI_symbol_Start (data->win, E_ED_INSERT);
			}
			else
			{
			/* SPR#1788 - SH - Allow '*' to be entered in numeric mode */
			 if (!ATB_edit_Mode(data->editor, ED_MODE_READONLY))
				AUI_entry_EventKey(data->entry_data, event, keyboard);
			}

			win_show(data->win);
			break;

		/* KEY ENTRY 0 - 9 */
		
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
            if (!ATB_edit_Mode(data->editor, ED_MODE_READONLY))
			{
                AUI_entry_EventKey(data->entry_data, event, keyboard);
                win_show(data->win);
        	}
			break;
		}

#ifdef EASY_TEXT_ENABLED
		}
	else
	
	/* Short key press in EasyText */

		{
		if(FFS_flashData.PredTextAvailable == TRUE)
		{
	        switch (keyboard->code)
            {
				case KCD_MNUUP:
					if(PredText->len==0)
					{
#ifdef LSCREEN
						ATB_edit_MoveCursor(data->editor, ctrlUp, TRUE);

					/* For other samples, up and down move left and right when not in read only mode */
#else
						ATB_edit_MoveCursor(data->editor,ctrlRight, TRUE);
#endif
						data->editor->update = ED_UPDATE_TRIVIAL;		/* This flag speeds up update */
						win_show(data->win);
					}	
					break;

				case KCD_MNUDOWN:
					if(PredText->len==0)
					{
#ifdef LSCREEN
						ATB_edit_MoveCursor(data->editor, ctrlDown, TRUE);

					/* For other samples, up and down move left and right when not in read only mode */
#else
						ATB_edit_MoveCursor(data->editor,ctrlLeft, TRUE);
#endif
						data->editor->update = ED_UPDATE_TRIVIAL;		/* This flag speeds up update */
						win_show(data->win);
					}
					break;

#ifdef LSCREEN
				case KCD_MNURIGHT:
					if(PredText->len==0)
					{
						ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
						data->editor->update = ED_UPDATE_TRIVIAL;		/* This flag speeds up update */
						win_show(data->win);
					}
					break;

				case KCD_MNULEFT:
					if(PredText->len==0)
					{
						ATB_edit_MoveCursor(data->editor,ctrlLeft, TRUE);
						data->editor->update = ED_UPDATE_TRIVIAL;		/* This flag speeds up update */
						win_show(data->win);
					}
					break;
#endif

				case KCD_LEFT:
					if(PredText->len==0)
					{
						if (data->editor_data.LeftSoftKey NEQ TxtNull)
						{
							SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_LEFT, NULL);
						}
					}
					else
					{
						if ((data->editor_data.editor_attr.text.len + PredText->len) < data->editor_data.editor_attr.size)
						{
							if (ATB_string_GetChar(PredText,0)!=UNICODE_STAR)
							{
								/* Capitalise first letter of words and the letter 'I' */
								
								if (ATB_edit_CapitaliseWord(data->editor) || (PredText->len==1 && ATB_string_GetChar(PredText, 0)==ATB_char_Unicode('i')))
								{
									ATB_string_SetChar(PredText, 0,  ATB_edit_FindCapital(ATB_string_GetChar(PredText, 0)));/*SPR1508*/
								}
								character = ATB_edit_GetCursorChar(data->editor, -1);
								
								if ( character != UNICODE_SPACE && character != UNICODE_EOLN)
								{
									ATB_string_MoveRight(PredText,0,1,ED_PREDTEXT_MAX);
									ATB_string_SetChar(PredText, 0, UNICODE_SPACE);
								}
								ATB_edit_InsertString(data->editor,PredText);
							}
						}
						ResetDictSearch();
						AUI_edit_ClearPredText(data);
						ATB_edit_SetCase(data->editor, ED_CASE_LOWER);
						win_show(data->win);
					}
					break;

				case KCD_HUP:
					if (data->editor_data.RightSoftKey NEQ TxtNull)			/* Right softkey is enabled: execute it */
					{
						SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_HUP, NULL);
					}
					break;

				case KCD_RIGHT:
					if(PredText->len==0)
					{
						if(ATB_string_GetChar(&data->editor_data.editor_attr.text, 0)!=UNICODE_EOLN)
						{
							ATB_edit_Char(data->editor,ctrlBack,TRUE);
							win_show(data->win);
						}
						else
						{
							/* leave editor if buffer already empty */
							SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_RIGHT, NULL);
						}
					}
					else
					{
						if (PredText->len==1)
						{
							DictBackSpace((char *)data->predText.data);
							AUI_edit_ClearPredText(data);
							ATB_edit_SetCase(data->editor, ED_CASE_LOWER);
						}
						else
						{
							DictBackSpace((char *)data->predText.data);
							data->predTextChar.pos--;						/* char deleted, move cursor pos back */
							AUI_edit_CalcPredText(data);
						}
						win_show(data->win);
					}
					break;

				case KCD_2:
				case KCD_3:
				case KCD_4:
				case KCD_5:
				case KCD_6:
				case KCD_7:
				case KCD_8:
				case KCD_9:
					if (PredText->len==0)
					{
						AUI_edit_ClearPredText(data);
					}
					if (DictAlphaKeyPress(keyboard->code,(char *)PredText->data))
					{	
						PredText->len = ATB_string_Length(PredText);
						data->predTextChar.pos++;			/* The keypress was used to find a word, advance cursor */
						AUI_edit_CalcPredText(data);
					}
					win_show(data->win);
					break;

				case KCD_HASH:
					switch(ATB_edit_GetCase(data->editor))
					{
						/* Press # once to switch into capitalise mode */
						case ED_CASE_CAPS:
							ATB_edit_SetCase(data->editor, ED_CASE_LOWER);
							/* Switch out of easy text mode, if we're not entering a word */
							if (PredText->len==0)
							{
								FFS_flashData.PredTextSelected = FALSE;
							}
							break;
						default:
							ATB_edit_SetCase(data->editor, ED_CASE_CAPS);
							break;
					}
					win_show(data->win);
					break;

				case KCD_STAR:
					if (PredText->len==0)
					{
						AUI_symbol_Start (data->win, E_ED_INSERT);
					}
					else
					{
						int i;
						MoreCandidates((char *)PredText->data, &i);
						AUI_edit_CalcPredText(data);
						win_show(data->win);
					}
					break;

				case KCD_0:
				case KCD_1:
					if (PredText->len!=0)
					{	
						if ((data->editor_data.editor_attr.text.len + PredText->len) < MAX_MSG_LEN)
						{
							/* If char b4 cursor full stop/question/exclamation (or last char space and char b4 full stop) */
							if (ATB_edit_CapitaliseWord(data->editor) || (PredText->len==1 && ATB_string_GetChar(PredText,0)==ATB_char_Unicode('i')))
								ATB_string_SetChar(PredText, 0, ATB_edit_FindCapital(ATB_string_GetChar(PredText, 0)));/*SPR1508*/

							/* Capitalise first letter of char. If char before cursor not space, insert space */
							character = ATB_edit_GetCursorChar(data->editor, -1);

							if (ATB_string_GetChar(PredText, 0) != UNICODE_STAR)
							{								
								if (keyboard->code == KCD_1)
								{
									/* Insert space at end of string */
									ATB_string_SetChar(PredText, PredText->len, UNICODE_SPACE);
									PredText->len++;
									ATB_string_SetChar(PredText, PredText->len, UNICODE_EOLN);
									
									if ( character!=UNICODE_SPACE && character!=UNICODE_EOLN)
									{
										/* Insert space at start of string */
										ATB_string_MoveRight(PredText, 0, 1, ED_PREDTEXT_MAX);
										ATB_string_SetChar(PredText, 0, UNICODE_SPACE);
									}
								}
								else
								{
									/* Insert space and full stop at end of string */
									ATB_string_SetChar(PredText, PredText->len, UNICODE_FULLSTOP);
									PredText->len++;
									ATB_string_SetChar(PredText, PredText->len, UNICODE_SPACE);
									PredText->len++;
									ATB_string_SetChar(PredText, PredText->len, UNICODE_EOLN);
									
									if ( character!=UNICODE_SPACE && character!=UNICODE_EOLN)
									{
										/* Insert space at start of string */
										ATB_string_MoveRight(PredText, 0, 1, ED_PREDTEXT_MAX);
										ATB_string_SetChar(PredText, 0, UNICODE_SPACE);
									}
								}
								ATB_edit_InsertString(data->editor, PredText);
							}
						}
						AUI_edit_ClearPredText(data);
						ResetDictSearch();
						ATB_edit_SetCase(data->editor, ED_CASE_LOWER);
						win_show(data->win);
					}
				break;

				/* Any key */
				
				default:
					if (!ATB_edit_Mode(data->editor, ED_MODE_READONLY)) /* ignore normal keys if in read-only mode */
					{
						AUI_entry_EventKey(data->entry_data, event, keyboard);
						ATB_edit_Char(data->editor,ctrlBack,TRUE);
						win_show(data->win);
					}
				break;
				}
			}
		}
#endif
	
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		AUI_edit_KbdLongCb

 $Description:	Editor keyboard event handler for long keypresses

 $Returns:		None.

 $Arguments:	event		- the keyboard event
 				keyboard 

*******************************************************************************/

static int AUI_edit_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;

#if (BOARD != 61)	&& defined(EASY_TEXT_ENABLED)
	T_ATB_TEXT			*PredText	= &data->predText;
#endif

	ED_RES				result;
  
#ifdef TRACE_AUIEditor
	TRACE_FUNCTION("AUI_edit_KbdLongCb()");
#endif

    /* Suppress unwanted long keypresses */

	if ( data->doNextLongPress )
		data->doNextLongPress = FALSE;       			/* mark Key_long event as consumed but do current long press */
 	// Jul 29, 2004         REF: CRR:20892 xrashmic - SASKEN
	// Description: By Long press on Left/Right key at 'Message Editor' screen, 
	// cursor not scrolling continuously
	// Fix: The continuous long press of a key was being ignored for all keys. 
	// Now only for left and right keys this is handled.
	else if(!(keyboard->code==KCD_MNULEFT ||keyboard->code== KCD_MNURIGHT))
	{
		return MFW_EVENT_CONSUMED;      		/* don't do current long press */
	}

	/* Restart the timer */
	
    if (data->timer !=NULL)
	{
    	tim_stop(data->timer);
    	tim_start(data->timer);
	}

#ifdef EASY_TEXT_ENABLED
	if (FFS_flashData.PredTextSelected == FALSE || !ATB_edit_Mode(data->editor, ED_MODE_PREDTEXT)) {
	/* Predictive text is off */
#endif

	switch (keyboard->code)
	{
	 	// Jul 29, 2004         REF: CRR:20892 xrashmic - SASKEN
		// Description: By Long press on Left/Right key at 'Message Editor' screen, 
		// cursor not scrolling continuously
		// Fix: The Long press of left/right key is now handled in AUI_edit_KbdLongCB.
		case KCD_MNULEFT:
			ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);		/* Move cursor left */
			data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
			win_show(data->win);
			break;
		case KCD_MNURIGHT:
			ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);		/* Move cursor right */
			data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
			win_show(data->win);
			break;

		case KCD_RIGHT:
			result = ATB_edit_ClearAll(data->editor);						/* Otherwise, clear the editor */	

			/* If buffer is empty, right soft key causes exit from editor */
			if (result==ED_DONE)
			{
				SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_RIGHT, NULL);
			}
			else
			{
				win_show(data->win);
			}
			break;

		/* KEY ENTRY 0 */
		
		case KCD_0:
			/* Alpha mode: insert a '0' */
			if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA))
			{
				if (keyboard->code<=KCD_HASH)
				{
					ATB_edit_DeleteLeft(data->editor, FALSE);  /* SPR#2342 - SH */
					AUI_entry_EditDigit(data->entry_data, keyboard->code);	// enter the character, not multi-tap
					win_show(data->win);
				}
			}

			/* Number mode: insert a '+'*/
			else if (!ATB_edit_Mode(data->editor, ED_MODE_READONLY))			/* ignore normal keys if in read-only mode */
			{
				if (ATB_edit_Mode(data->editor, ED_MODE_HIDDEN))								/* in hidden mode do not allow entering of '+' */
				{
					ATB_edit_DeleteLeft(data->editor, TRUE); /* remove the '0'. SPR#2342 - SH */
				}
				else
				{
					ATB_edit_Char(data->editor,ctrlBack,FALSE); 				/* remove the '0' */
					ATB_edit_Char(data->editor,UNICODE_PLUS,TRUE);    			/* insert the '+' */
				}
				win_show(data->win);
			}
			break;
		case (KCD_STAR):
			/* MZ cq11414 add Pause Character */
			ATB_edit_DeleteLeft(data->editor, FALSE);

			//Sep 2, 2004    REF: CRR 21370 xkundadu
			//Changed the long press of '*' key to display 'w'
    			AUI_entry_EditChar(data->entry_data, ATB_char_Unicode('w'), FALSE);
			win_show(data->win);
			break;

		case (KCD_HASH):

			/* In alphanumeric mode, a long hold of # selects number case */
			if (ATB_edit_GetCasePref(data->editor)==ED_CASEPREF_ALPHANUM)
				ATB_edit_SetCase(data->editor, ED_CASE_NUM);
			else 
			{ 
				/* MZ cq11414 add Wait  Character */			
				ATB_edit_DeleteLeft(data->editor, FALSE);

				//Sep 2, 2004    REF: CRR 21370 xkundadu
				//Changed the long press of '#' key to display 'p'
	    			AUI_entry_EditChar(data->entry_data, ATB_char_Unicode('p'), FALSE);
			}
			win_show(data->win);
			break;

		/* KEY ENTRY 1 - 9 */
		
		case KCD_1:
		case KCD_2:
		case KCD_3:
		case KCD_4:
		case KCD_5:
		case KCD_6:
		case KCD_7:
		case KCD_8:
		case KCD_9:
			/* Insert equivalent numeric character, only possible in Alpha modes */
			if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA))
			{
				if (keyboard->code<=KCD_HASH)
				{
					ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
					AUI_entry_EditDigit(data->entry_data, keyboard->code);	// enter the character, not multi-tap
					win_show(data->win);
				}
			}
			break;

		}
	
#ifdef EASY_TEXT_ENABLED
	}
	else
	{
		if(FFS_flashData.PredTextAvailable == TRUE)
		{
			switch (keyboard->code)
			{
			 	// Jul 29, 2004         REF: CRR:20892 xrashmic - SASKEN
				// Description: By Long press on Left/Right key at 'Message Editor' 
				// screen,  cursor not scrolling continuously
				// Fix: The Long press of left/right key is now handled in
				// AUI_edit_KbdLongCB.
				case KCD_MNULEFT:
					ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);		/* Move cursor left */
					data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
					win_show(data->win);
					break;		
				case KCD_MNURIGHT:
					ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);		/* Move cursor right */
					data->editor->update = ED_UPDATE_TRIVIAL;				/* This flag speeds up update - no change to text */
					win_show(data->win);
					break;

				case KCD_RIGHT: 								/*empty predictive text string */
					if(PredText->len!=0)
					{
						PredText->len = 0;
						ATB_string_SetChar(PredText, 0, UNICODE_EOLN);
						ResetDictSearch();
						win_show(data->win);
					}
					else 										/*empty whole editor */
					{
						if(data->editor_data.editor_attr.text.len > 0 )
						{
							ATB_edit_ClearAll(data->editor);
							win_show(data->win);
						}
						else
						{
							/* leave editor if buffer already empty */
							SEND_EVENT (win, E_ED_DEINIT, INFO_KCD_RIGHT, NULL);
						}
					}
					break;

				case(KCD_STAR):
					AUI_symbol_Start(data->win, E_ED_INSERT);
					break;

				case (KCD_HASH):
					break;

				/* KEY ENTRY 1 - 9 */

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
					if(PredText->len==0)
					{
						/* Insert equivalent numeric character, only possible in Alpha modes */
						if (ATB_edit_Mode(data->editor, ED_MODE_ALPHA))
						{
							if (keyboard->code<=KCD_HASH)
							{
								ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
								AUI_entry_EditDigit(data->entry_data, keyboard->code);	// enter the character, not multi-tap
							}
						}
						win_show(data->win);
					}
					break;
			}
		}
	}
	#endif

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	AUI_edit_TimerCb

 $Description:	Callback function for the editor info timer.
 
 $Returns:		None.

 $Arguments:	event	- the event type
 				timer	- the timer
 
*******************************************************************************/

static int AUI_edit_TimerCb (T_MFW_EVENT event, T_MFW_TIM *timer)
{
    T_MFW_HND			win			= mfw_parent (mfw_header());



//    T_MFW_HND			parent_win	= data->parent;  // RAVI
//    USHORT				Identifier	= data->editor_data.Identifier;  // RAVI
//    T_AUI_EDIT_CB			Callback	= data->editor_data.Callback;  // RAVI

	TRACE_FUNCTION("AUI_edit_TimerCb");

#if 0
    AUI_edit_Destroy(data->win);

    if (Callback)
        (Callback) (parent_win, Identifier, INFO_TIMEOUT);
#else
	SEND_EVENT (win, E_ED_DEINIT, INFO_TIMEOUT, NULL);
#endif

    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	AUI_edit_TitleTimerCb

 $Description:	Callback function for the editor title timer.  Recalculates title_pos so that
 				the next part of the string is displayed.
 
 $Returns:		None.

 $Arguments:	event	- the event type
 				timer	- the timer
 
*******************************************************************************/

static int AUI_edit_TitleTimerCb (T_MFW_EVENT event, T_MFW_TIM *timer)
{
   	/* Use the mfwHeader property of the timer to get the window */
    T_MFW_HND			win			= mfw_parent(timer->mfwHeader); 
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_EDITOR_INFO 	*data		= (T_AUI_EDITOR_INFO *)win_data->user;

	TRACE_FUNCTION("AUI_edit_TitleTimerCb");

	/* SPR#1597 - SH - Since there may be multiple timers running at once, only
	call win_show if the window is focussed. */
	
	if (winIsFocussed(win))
	{
		if (data->title_next_pos!=data->title_pos)
		{
			data->title_pos = data->title_next_pos;
		}

		data->editor->update = ED_UPDATE_PARTIAL;  /* Don't update editor text */
		win_show(data->win);

		tim_start(data->title_timer);
	}
	
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:		AUI_edit_CalcPredText

 $Description:	Calculate properties of predicted text string.

 $Returns:		None.

 $Arguments:	data	- The AUI editor data
 
*******************************************************************************/

static void AUI_edit_CalcPredText(T_AUI_EDITOR_INFO *data)
{
	USHORT character;
	T_ATB_TEXT dummy;

	data->predText.len = ATB_string_Length(&data->predText);

	/* Get position of current entry */
	dummy.data = data->predText.data;
	dummy.dcs = data->predText.dcs;
	dummy.len = data->predTextChar.pos;
	data->predTextChar.lineWidth = ATB_display_StringWidth(&dummy, &data->predTextChar.format);
 	data->predTextChar.lineHeight = ATB_display_StringHeight(&dummy, &data->predTextChar.format);
 
	/* Get width and height of current character */
	character = ATB_string_GetChar(&data->predText, data->predTextChar.pos);
	data->predTextChar.width = ATB_display_GetCharWidth(character, &data->predTextChar.format);
	data->predTextChar.height = ATB_display_GetCharHeight(character, &data->predTextChar.format);

	return;
}


/*******************************************************************************

 $Function:		AUI_edit_ClearPredText

 $Description:	Clear the predictive text buffer

 $Returns:		None.

 $Arguments:	data	- The AUI editor data
 
*******************************************************************************/

static void AUI_edit_ClearPredText(T_AUI_EDITOR_INFO *data)
{
	data->predText.len = 0;
	ATB_string_SetChar(&data->predText,0,UNICODE_EOLN);
	memset(&data->predTextChar, 0, sizeof(T_ED_CHAR));
	return;
}


/*******************************************************************************

 $Function:		AUI_edit_InsertString

 $Description:	Insert a string into the editor
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	win		- The editor window
 				string	- The string to insert
				dcs		- The dcs of the string
	
*******************************************************************************/

void AUI_edit_InsertString(T_MFW_HND win, UBYTE *string, T_ATB_DCS dcs)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;
	T_ATB_TEXT			text;

	text.data = string;
	text.dcs = dcs;
	text.len = ATB_string_Length(&text);
	
    ATB_edit_InsertString(data->editor, &text);

	return;
}


/*******************************************************************************

 $Function:		AUI_edit_InsertChar

 $Description:	Insert a character into the editor
 
 $Returns:		Pointer to the editor's window.

 $Arguments:	win			- The editor window
 				character	- The character to insert
	
*******************************************************************************/

void AUI_edit_InsertChar(T_MFW_HND win, USHORT character)
{
    T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
    T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;

    ATB_edit_Char(data->editor, character, TRUE);

	return;
}


/*******************************************************************************

 $Function:		AUI_edit_GetSpecificWindowSIze
 
 $Description:	Sets up window size

 $Returns:		None.

 $Arguments:	attr	- The ATB editor attributes
 				zone_id	- Specifies the window size (See ZONE_... in header file)

*******************************************************************************/
#ifndef LSCREEN
//Sizes defined for CSAMPLE editore
#define BOOKDETAILS_EDITOR_SIZE 		0, 0, SCREEN_SIZE_X-4, Mmi_layout_line(LAST_LINE_TOP)
#define NUMBER_EDITOR_SIZE				0, 12, SCREEN_SIZE_X , MNU3LINE
#define PHONEBOOK_DLG_EDITOR_SIZE		4, Mmi_layout_line(3)+2, SCREEN_SIZE_X-4, SCREEN_SIZE_Y-(Mmi_layout_line(3)+2)
#define BOOKSDN_EDITOR_SIZE 			34, Mmi_layout_line(3)+2, 50, Mmi_layout_line(1)+2

//MMIbooksearchwindow
//#define BOOKSEARCH_EDITOR_SIZE 			0, Mmi_layout_line(SECOND_LAST_LINE_TOP), SCREEN_SIZE_X, Mmi_layout_line(1)

//MMICall
#define CALL_EDITOR_SIZE				4,28,SCREEN_SIZE_X-4, 20
#define CALLSCRATCHPAD_EDITOR_SIZE		6, Mmi_layout_line(2),SCREEN_SIZE_X-6, 16

//MMIServices
#define SERVICE_CREATE_EDITOR_SIZE		4,Mmi_layout_line(1)+2,SCREEN_SIZE_X-4, MNU3LINE
#define PASSWORD_EDITOR_SIZE			0, 12, SCREEN_SIZE_X , 13
#define EM_EDITOR_SIZE					0, 0, SCREEN_SIZE_X-4, Mmi_layout_line(LAST_LINE_TOP)

//From MMIIdle
//GW Changed window height from 30 (27?) to 24.
#define WIN_DIALLING_SIZE				0,Mmi_layout_line_icons(2), SCREEN_SIZE_X,24
#define WIN_DIALLING_C_SIZE 			0,Mmi_layout_line_icons(1),SCREEN_SIZE_X,32

//Positions for editor windows in mmismssend
#define PHNO_EDITOR_SIZE 				0, (Mmi_layout_line(1)+2), SCREEN_SIZE_X, 20

//NB defines below used twice, second time START/SIZE_Y was same as START/SIZE_Y above 
#define CENTRE_EDITOR_SIZE				0, Mmi_layout_line(1), SCREEN_SIZE_X, MNU3LINE+3

//And for MMIServices - moved here from mmiservices.c
#define SAT_EDITOR_SIZE	 				6,30,SCREEN_SIZE_X-6, 20

#define RPWD_EDITOR_SIZE				0,12,SCREEN_SIZE_X,13

//MMIBookCallDetails
#define PHB_EDITOR_SIZE  				4,0,SCREEN_SIZE_X-4, ALLBUT2LINE

//MMIPins
#define PIN_EDIT_RESET_SIZE 			12,20,84-12,16
//MMITimeDate
#define TIMEDATE_EDITOR_AREA_SIZE 0,10,70,10
#endif
/* GW SPR#1840 26/03/03 - Copied from MMieditor.c*/
#ifndef LSCREEN
static void setSize(T_ATB_WIN_SIZE *win, int px, int py, int sx, int sy)
{
	win->px = px;	win->py = py;
	win->sx = sx;	win->sy = sy;
}
#endif
void AUI_edit_GetSpecificWindowSize(T_ED_ATTR* attr, int zone_id )
{
	switch (zone_id)
	{
		case ZONE_SMALL_EDITOR:
			attr->win_size.px = 6;
			attr->win_size.py = Mmi_layout_IconHeight()*2;
			attr->win_size.sx = SCREEN_SIZE_X-attr->win_size.px;
			attr->win_size.sy = Mmi_layout_line_height()*2+4;
			break;
			
		case ZONE_BOTTOM_LINE:
			attr->win_size.px = 0;
			attr->win_size.py = SCREEN_SIZE_Y - Mmi_layout_line_height() - Mmi_layout_softkeyHeight();
			attr->win_size.sx = SCREEN_SIZE_X-attr->win_size.px;
			attr->win_size.sy = Mmi_layout_line_height();
			break;
			
		case ZONE_BOTTOM_LINE_FIND: //used for phonebook search
			attr->win_size.px = dspl_GetTextExtent(MmiRsrcGetText(TxtFind),0); 
			attr->win_size.py = SCREEN_SIZE_Y - Mmi_layout_line_height() - Mmi_layout_softkeyHeight();
			attr->win_size.sx = SCREEN_SIZE_X-attr->win_size.px;
			attr->win_size.sy = Mmi_layout_line_height();
			break;
		
#ifndef LSCREEN
		case BOOKDETAILS_EDITOR: 		setSize(&attr->win_size,BOOKDETAILS_EDITOR_SIZE);		break;
		case NUMBER_EDITOR:			 	setSize(&attr->win_size,NUMBER_EDITOR_SIZE);				break;
		case PHONEBOOK_DLG_EDITOR:		setSize(&attr->win_size,PHONEBOOK_DLG_EDITOR_SIZE);		break;
		case BOOKSDN_EDITOR:			setSize(&attr->win_size,BOOKSDN_EDITOR_SIZE);			break;
	//	case BOOKSEARCH_EDITOR:		 	setSize(&attr->win_size,BOOKSEARCH_EDITOR_SIZE);			break;
		case CALL_EDITOR: 			 	setSize(&attr->win_size,CALL_EDITOR_SIZE);				break;
		case CALLSCRATCHPAD_EDITOR:		setSize(&attr->win_size,CALLSCRATCHPAD_EDITOR_SIZE);		break;
		case SERVICE_CREATE_EDITOR:		setSize(&attr->win_size,SERVICE_CREATE_EDITOR_SIZE);		break;
		case PASSWORD_EDITOR:			setSize(&attr->win_size,PASSWORD_EDITOR_SIZE);			break;
		case EM_CREATE_EDITOR:			setSize(&attr->win_size,EM_EDITOR_SIZE);				break;/*SPR#1840*/
		
		case WIN_DIALLING:				
		case WIN_DIALLING_CB:				
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
					setSize(&attr->win_size,WIN_DIALLING_C_SIZE);
				else
					setSize(&attr->win_size,WIN_DIALLING_SIZE);
				break;
		case WIN_DIALLING_SMS:		 	
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{
					setSize(&attr->win_size,0,Mmi_layout_line_icons(1),SCREEN_SIZE_X,24);/*SPR#1840*/
				}
				else
					setSize(&attr->win_size,WIN_DIALLING_SIZE);
				break;
		case WIN_DIALLING_SAT:		 	
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
					setSize(&attr->win_size,WIN_DIALLING_C_SIZE);
				else
					setSize(&attr->win_size,WIN_DIALLING_SIZE);
				break;
		
		case PHNO_EDITOR:			 	setSize(&attr->win_size,PHNO_EDITOR_SIZE);				break;
		case CENTRE_EDITOR:				setSize(&attr->win_size,CENTRE_EDITOR_SIZE);				break;
		case SAT_EDITOR	:			 	setSize(&attr->win_size,SAT_EDITOR_SIZE);				break;
		case RPWD_EDITOR:			 	setSize(&attr->win_size,RPWD_EDITOR_SIZE);				break;
		case PHB_EDITOR: 			 	setSize(&attr->win_size,PHB_EDITOR_SIZE);				break;
		case PIN_EDIT_RESET:			setSize(&attr->win_size,PIN_EDIT_RESET_SIZE);			break;
		case TIMEDATE_EDITOR_AREA:		setSize(&attr->win_size,TIMEDATE_EDITOR_AREA_SIZE);		break;
#endif	
		default:
			break;
	}
}

/*******************************************************************************

 $Function:		AUI_edit_CalculateWindow
 
 $Description:	Sets up window size

 $Returns:		None.

 $Arguments:	attr	- The ATB editor attributes
 				zone_id	- Specifies the window size (See ZONE_... in header file)
 				SPR#1559 - SH - this is now a pointer to the zone_id, allowing
 							the zone_id to be modified by this function

*******************************************************************************/

void AUI_edit_CalculateWindow(T_ED_ATTR* attr, int *zone_id_pointer )
{
	int zone_id = *zone_id_pointer;
	
	attr->win_size.px = 0;
	attr->win_size.py = 0;
	attr->win_size.sx = SCREEN_SIZE_X;
	attr->win_size.sy = SCREEN_SIZE_Y;
	
	if (zone_id == ZONE_BORDER_EDITOR)
	{	//Leave border on left/right of screen.
		attr->win_size.px = 4;
		attr->win_size.sx = attr->win_size.sx - 2*attr->win_size.px;
		zone_id = ZONE_FULLSOFTKEYS;
		*zone_id_pointer = zone_id;
	}
	else if ((zone_id != 0x0000) && ((zone_id &0xF000)==0xF000))/*SPR#1840*/
	{
		AUI_edit_GetSpecificWindowSize(attr,zone_id);
	    return;
	}
	else if (zone_id == 0)
	{
		zone_id = ZONE_FULLSOFTKEYS;
		*zone_id_pointer = zone_id;
	}
	
	if (zone_id & ZONE_ICONS)
		attr->win_size.py = attr->win_size.py + Mmi_layout_IconHeight();

	if (zone_id & ZONE_SOFTKEYS)
		attr->win_size.sy = attr->win_size.sy - Mmi_layout_softkeyHeight();

	if (zone_id & ZONE_TITLE)
		attr->win_size.py = attr->win_size.py + Mmi_layout_TitleHeight();
	
	if (zone_id & ZONE_CASE_ABC)
		attr->win_size.sy = attr->win_size.sy - Mmi_layout_line_height();

	if (zone_id & ZONE_SCROLLBAR)
		attr->win_size.sx = attr->win_size.sx - ED_SCROLLBAR_WIDTH;

	attr->win_size.sy = attr->win_size.sy - attr->win_size.py;

	if ((zone_id & ZONE_FULLSCREEN) == ZONE_FULLSCREEN)
		return;
	
	/*Else window is not entire screen */
	
	switch (zone_id & ZONE_FULL_HEIGHT)
	{
		case ZONE_FULL_HEIGHT:	
			break;
		case ZONE_TOPHALF_HEIGHT:
			attr->win_size.sy = attr->win_size.sy/2;		
			break;
		case ZONE_MIDDLE_HEIGHT:			
			attr->win_size.sy = attr->win_size.sy/2;		
			attr->win_size.py = attr->win_size.py+attr->win_size.sy/2;		
			break;
		case ZONE_BOTTOMHALF_HEIGHT:		
			attr->win_size.sy = attr->win_size.sy/2;		
			attr->win_size.py = attr->win_size.py+attr->win_size.sy;		
			break;
		case ZONE_BOTTOMTWOLINES_HEIGHT:		
			attr->win_size.py = attr->win_size.py+attr->win_size.sy-Mmi_layout_line_height()*2;
			attr->win_size.sy = Mmi_layout_line_height()*2;				
			break;
		/* xreddymn OMAPS00075852 May-15-2006
		 * This display zone is located one line above the softkeys.
		 */
		case ZONE_MIDDLETWOLINES_HEIGHT:		
			attr->win_size.py = attr->win_size.py+attr->win_size.sy-Mmi_layout_line_height()*3;
			attr->win_size.sy = Mmi_layout_line_height()*2;				
			break;
		default:
			//No action
			break;
	}
	
	switch (zone_id & ZONE_FULL_WIDTH)
	{
		case ZONE_FULL_WIDTH:	
			break;
		case ZONE_LEFTHALF_WIDTH:		
			attr->win_size.sx = attr->win_size.sx/2;		
			break;
		case ZONE_MIDDLE_WIDTH	:		
			attr->win_size.sx = attr->win_size.sx/2;		
			attr->win_size.px = attr->win_size.px+attr->win_size.sx/2;		
			break;
		case ZONE_RIGHTHALF_WIDTH:		
			attr->win_size.sx = attr->win_size.sx/2;		
			attr->win_size.px = attr->win_size.px+attr->win_size.sx;		
			break;
		default:
			//No action
			break;
	}
}


/*******************************************************************************

 $Function:		AUI_edit_SetDefault

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
 
*******************************************************************************/

void AUI_edit_SetDefault(T_AUI_EDITOR_DATA *editor_data)
{
	/* Set everything to zero... */
	/* SPR#1559 - Move CalculateWindow to below */
	
	memset(editor_data, 0, sizeof(T_AUI_EDITOR_DATA));

	/* Except these things... */
	
	editor_data->editor_attr.colour	= COLOUR_EDITOR;
	editor_data->editor_attr.font	= EDITOR_FONT;
	editor_data->editor_attr.cursor	= ED_CURSOR_BAR;
	editor_data->editor_attr.mode	= ED_MODE_ALPHA;
	editor_data->editor_attr.text.dcs    = ATB_DCS_ASCII;
	editor_data->timeout			= FOREVER;
	editor_data->destroyEditor		= TRUE;
	editor_data->zone_id			= ZONE_FULL_SK_TITLE;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	editor_data->editor_attr.TitleIcon.data = NULL;
	editor_data->editor_attr.TitleIcon.width = 0;
	editor_data->editor_attr.TitleIcon.height = 0;
	editor_data->editor_attr.TitleIcon.isTitle = FALSE;
//x0035544 Mar 14, 2006 DR:OMAPS00061468
//set the default value for display_type to SAT_ICON_NONE
	editor_data->editor_attr.TitleIcon.display_type = SAT_ICON_NONE;
	editor_data->editor_attr.TitleIcon.selfExplanatory = FALSE;
#endif	

	/* SPR#1559 - SH - CalculateWindow moved here; pointer to zone_id used */
	AUI_edit_CalculateWindow(&editor_data->editor_attr, &editor_data->zone_id);
	
	ATB_display_ClearFormat(&editor_data->editor_attr.startFormat);
	return;
}


/*******************************************************************************

 $Function:		AUI_edit_SetDisplay

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
 				zone_id		- Type of editor screen (see header file for ZONE_...)
 				colour		- Colour of the editor
 
*******************************************************************************/

void AUI_edit_SetDisplay(T_AUI_EDITOR_DATA *editor_data, int zone_id, int colour, UBYTE font )
{
	/* SPR#1559 - SH - Use pointer to zone_id */
	AUI_edit_CalculateWindow(&editor_data->editor_attr, &zone_id);
	editor_data->zone_id 			= zone_id;
	editor_data->editor_attr.colour	= colour;
	editor_data->editor_attr.font	= font;
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetEvents

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.

*******************************************************************************/

void AUI_edit_SetEvents(T_AUI_EDITOR_DATA *editor_data, USHORT Identifier, BOOL destroyEditor, ULONG timeout, T_AUI_EDIT_CB Callback)
{
	if (editor_data)
	{
		editor_data->Identifier			= Identifier;
		editor_data->destroyEditor		= destroyEditor;
		editor_data->timeout			= timeout;
		editor_data->Callback			= Callback;
	}	
	return;
}


/*******************************************************************************

 $Function:		AUI_edit_SetMode

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
 				mode		- Editor mode (see header file for ED_MODE_...)
 				cursor		- Editor cursor (see header file for ED_CURSOR_...)
 
*******************************************************************************/

void AUI_edit_SetMode(T_AUI_EDITOR_DATA *editor_data, USHORT mode, USHORT cursor )
{
	if (editor_data)
	{
		editor_data->editor_attr.mode		= mode;
		editor_data->editor_attr.cursor		= cursor;
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetBuffer

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
 				dcs			- Data coding scheme of text (see ATB_DCS_...)
 				text		- The text in the appropriate coding scheme
 				size		- Maximum size of the edit buffer in characters
 
*******************************************************************************/

void AUI_edit_SetBuffer(T_AUI_EDITOR_DATA *editor_data, UBYTE dcs, UBYTE *text, USHORT size)
{
	if (editor_data)
	{
		editor_data->editor_attr.text.dcs = dcs;
		editor_data->editor_attr.text.data = text;
		editor_data->editor_attr.text.len = ATB_string_Length(&editor_data->editor_attr.text);
		editor_data->editor_attr.size = size;
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetTextStr

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
 				LeftSoftKey, RightSoftKey - Text IDs of the softkeys
 				TitleId		- Text ID for the title
 				TextString	- Text string for the title, used if TitleId is 0
 
*******************************************************************************/

void AUI_edit_SetTextStr(T_AUI_EDITOR_DATA *editor_data, USHORT LeftSoftKey, USHORT RightSoftKey, USHORT TitleId, UBYTE *TitleString)
{
	if (editor_data)
	{
		editor_data->LeftSoftKey = LeftSoftKey;
		editor_data->RightSoftKey = RightSoftKey;
		editor_data->AltRightSoftKey = RightSoftKey;
		editor_data->TitleId = TitleId;
		editor_data->TitleString = TitleString;
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetAltTextStr

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
  				min_enter	- No of characters needed to be entered
 				AltLeftSoftKey - Text ID of alternate left softkey
 				change_rsk_on_empty - TRUE if alternative right soft key is used for
 				              empty buffer
 				AltRightSoftKey - Text ID of alternative right softkey
 
*******************************************************************************/

void AUI_edit_SetAltTextStr(T_AUI_EDITOR_DATA *editor_data, UBYTE min_enter, USHORT AltLeftSoftKey, BOOL change_rsk_on_empty, USHORT AltRightSoftKey)
{
	if (editor_data)
	{
		editor_data->AltLeftSoftKey = AltLeftSoftKey;
		editor_data->min_enter = min_enter;
		editor_data->AltRightSoftKey = AltRightSoftKey;
		editor_data->change_rsk_on_empty = change_rsk_on_empty;
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetFormatStr

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
  				formatstr	- The formatted input string
  				preformat	- TRUE if format codes are to be inserted into text
  							  before editing
  				filler		- If preformat is true, this character will be used to
  							  pad out the provided string
 
*******************************************************************************/

void AUI_edit_SetFormatStr(T_AUI_EDITOR_DATA *editor_data, char *formatstr, BOOL preformat, char filler)
{
	USHORT textIndex;
	USHORT formatIndex;
//	USHORT character;  // RAVI

	TRACE_FUNCTION("AUI_edit_SetFormatStr()");
	
	if (editor_data)
	{
		editor_data->editor_attr.FormatString = formatstr;

		/* SPR#1983 - SH - *m and *M strings are freeform, no preformatting required */
		if (preformat && strcmp(formatstr, "*m")!=0 && strcmp(formatstr, "*M")!=0)
		{
			formatIndex = 0;
			textIndex = 0;

			/* SPR#1983 - SH - Ensure formatting doesn't write over null terminator */
			while (formatstr[formatIndex]!=0 && 
				!strchr("123456789*", formatstr[formatIndex]) && /* Exit at delimited field */
				textIndex<(editor_data->editor_attr.size-1))
			{
				/* Encountered fixed character */
				if (formatstr[formatIndex]=='\\')
				{
					formatIndex++;
					ATB_string_SetChar(&editor_data->editor_attr.text, textIndex, ATB_char_Unicode(formatstr[formatIndex]));
				}
				/* If editor text has run out, fill the rest with our filler character */
				else if (textIndex>editor_data->editor_attr.text.len)
				{
					ATB_string_SetChar(&editor_data->editor_attr.text, textIndex, ATB_char_Unicode(filler));
				}
				if(formatstr[formatIndex]!=0)       /*a0393213 lint warnings removal - 'Possible access of out-of-bounds pointer'*/
					{
					formatIndex++;
					textIndex++;
					}			
			}
		}
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetFormatAttr

 $Description:	Sets up standard editor attributes.

 $Returns:		None.

 $Arguments:	editor_data	- The MMI editor data that is to be modified.
  				startFormat	- Pointer to text formatting data structure
 
*******************************************************************************/

void AUI_edit_SetFormatAttr(T_AUI_EDITOR_DATA *editor_data, UBYTE attr)
{
	if (editor_data)
	{
		editor_data->editor_attr.startFormat.attr = attr;
	}
	
	return;
}

/*******************************************************************************

 $Function:		AUI_edit_SetAttr

 $Description:	Set some standard editor attributes

 $Returns:    	None.

 $Arguments:  	attr	- The editor attr

*******************************************************************************/
void AUI_edit_SetAttr(T_ED_ATTR *attr, int zone_id, ULONG colour, UBYTE font, USHORT mode, USHORT cursor, T_ATB_DCS dcs, UBYTE *text, USHORT size)
{
	/* Populate the structure with the parameters
	*/
	/* SPR#1559 - SH - Use pointer to zone_id */
	AUI_edit_CalculateWindow(attr, &zone_id );
	attr->colour = colour;
	attr->font = font;
	attr->mode = mode;
	attr->cursor = cursor;
	attr->text.dcs = dcs;
	attr->text.data = text;
	attr->text.len = ATB_string_Length(&attr->text);
	attr->size = size;

	return;
}

/* API - 08/07/03 - CQ10728 - Query your own number with a D1 SIM*/
/*******************************************************************************

 $Function:		AUI_Edit_Display_Only

 $Description:	Sets up the display requirements for the Editor & also shows it

 $Returns:    	None.

 $Arguments:  	win - parent window, LSK - Left Soft Key, RSK - Right Soft Key
 				string1 - String details, string2 - String details, Title - Editor title string

*******************************************************************************/
void AUI_Edit_Display_Only(T_MFW_HND win, int LSK, int RSK, char *string1, char *string2, int Title)
{	
	 
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)win)->data;
	T_AUI_EDITOR_INFO	*data		= (T_AUI_EDITOR_INFO *)win_data->user;

	T_AUI_EDITOR_DATA display_editor_info;

	int size = MAX_RO_EDITOR_LEN;

	AUI_edit_SetDefault(&display_editor_info);
			
			/* Include title for colour display */
#ifdef COLOURDISPLAY
	AUI_edit_SetDisplay(&display_editor_info, ZONE_FULL_SK_TITLE_SCROLL, COLOUR_EDITOR, EDITOR_FONT);
#else /* COLOURDISPLAY */
	AUI_edit_SetDisplay(&display_editor_info, ZONE_FULL_SK_SCROLL, COLOUR_EDITOR, EDITOR_FONT);
#endif /* COLOURDISPLAY */
	AUI_edit_SetMode(&display_editor_info, ED_MODE_ALPHA | ED_MODE_READONLY, ED_CURSOR_NONE);
	AUI_edit_SetEvents(&display_editor_info, 0, FALSE, FOREVER, (T_AUI_EDIT_CB) AUI_Edit_Display_Only_CB);
	AUI_edit_SetTextStr(&display_editor_info, LSK, RSK, Title, NULL);

	/* SPR#2672 - SH - Allocate buffer dynamically */

	data->displayBuffer = (char *)ALLOC_MEMORY(size);
	
	strcpy(data->displayBuffer, string1);
		
	if(string2 != NULL)
	{
		strcat(data->displayBuffer, "\n");
		strcat(data->displayBuffer, string2);
	}
	
	if (data->displayBuffer[0]==(char)0x80)
	{
		AUI_edit_SetBuffer(&display_editor_info, ATB_DCS_UNICODE, (UBYTE *)&data->displayBuffer[2], size/2);
	}
	else
	{
		AUI_edit_SetBuffer(&display_editor_info, ATB_DCS_ASCII, (UBYTE *)&data->displayBuffer[0], size);
	}

	data->win = AUI_edit_Start(win,&display_editor_info);  /* start the editor */
	
}

/*******************************************************************************

 $Function:		AUI_Edit_Display_Only_CB

 $Description:	Handles the key call back from the Read only Editor

 $Returns:    	None.

 $Arguments:  	

*******************************************************************************/
static void AUI_Edit_Display_Only_CB(T_MFW_HND win, USHORT Identifier, SHORT reason)
{ 
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_AUI_EDITOR_INFO     * data = (T_AUI_EDITOR_INFO *)win_data->user;

TRACE_EVENT("AUI_Edit_Display_Only_CB");

	switch(reason)
	{
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
			TRACE_EVENT("INFO_KCD_RIGHT/HUP");
			AUI_Edit_Display_Only_Destroy(data->win);
		break;

		default:
			TRACE_EVENT("Err: Default");
			break;
		
	}
}

/*******************************************************************************

 $Function:    	AUI_Edit_Display_Only_Destroy

 $Description:	Destroy the Read only information Editor
 
 $Returns:		none

 $Arguments:	own_window - current window handler
 				
*******************************************************************************/

static void AUI_Edit_Display_Only_Destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_AUI_EDITOR_INFO     * data;

//	USHORT i;  // RAVI

    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_AUI_EDITOR_INFO *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 

			/* SPR#2672 - SH - Delete buffer */

			if (data->displayBuffer!=NULL)
			{
				FREE_MEMORY((void *)data->displayBuffer, MAX_RO_EDITOR_LEN);
			}
			
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->win);
#else /* NEW_EDITOR */
			editor_destroy(data->win);
#endif /* NEW_EDITOR */

		    win_delete (data->win);

		    FREE_MEMORY ((void *)data, sizeof (T_AUI_EDITOR_INFO));
		
		
		}
	    else
		{
		    TRACE_EVENT ("SmsRead_R_TEXTEDT2_destroy() called twice");
		}
	}
}
/* API - 08/07/03 - CQ10728 END*/
