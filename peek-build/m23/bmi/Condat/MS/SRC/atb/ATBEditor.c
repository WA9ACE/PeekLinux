/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    ATBEditor.c
 $Revision:		                                                      
                                                                              
 $Author:		SH - Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description: ATB Editor component.
    
                        
********************************************************************************

 $History: ATBEditor.c

 	Mar 31, 2008 DR:OMAPS00164541 - x0082844
 	Description: OTA ATT - GSM-BTR-1-5618 - MS wrongly displays long number stored in ADN
 	Solution:No need to check for the current chararcter, if its a space.
 
       Feb 02, 2006 DR: OMAPS00061468 - x0035544.
       Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
       Solution : Modifications for displaying idle mode text and icon are done in 
                           ATB_edit_OutTextLines. 
       
	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : Modifications done in ATB_edit_OutTextLines 
				for display of the title icon.

  CRR:25542 - xpradipg - 26 Oct 2004
 Description: The last character is not deleted from the text entry screen in
 any WAP page.
 Solution: The formatIndex was set to zero on reaching the last character which
 skips the removal of the last character. This assignment is moved out.
	
	 Jul 22,2004 CRR:21605 xrashmic - SASKEN
	 Description: After deleting all the characters in the editor the case does not change to 
	 sentence case.
	 Fix: After deleting a character, check if editor is empty and then set the case to
	 sentence case only if the user has not modified the case.

	
 $End

*******************************************************************************/

#ifndef ATB_EDITOR_C
#define ATB_EDITOR_C
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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "dspl.h"
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "font_bitmaps.h"

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"

#undef	TRACE_ATBEDITOR

extern UBYTE CaseChanged; // SPR 21605

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
const unsigned char SATIconQuestionMark[] =
{
	0x25,0x25,0x00,0x00,0x00,0x00,0x25,0x25,
	0x25,0x00,0xE0,0xE0,0xE0,0xE0,0x00,0x25,
	0x00,0xE0,0xE0,0xFF,0xFF,0xE0,0xE0,0x00,
	0x00,0xE0,0xFF,0xE0,0xE0,0xFF,0xE0,0x00,
	0x00,0xE0,0xE0,0xE0,0xE0,0xFF,0xE0,0x00,
	0x00,0xE0,0xE0,0xFF,0xFF,0xE0,0xE0,0x00,
	0x00,0xE0,0xE0,0xFF,0xE0,0xE0,0xE0,0x00,
	0x00,0xE0,0xE0,0xFF,0xE0,0xE0,0xE0,0x00,
	0x25,0x00,0xE0,0xE0,0xE0,0xE0,0x00,0x25,
	0x25,0x25,0x00,0x00,0x00,0x00,0x25,0x25,
};
#endif


/*******************************************************************************

  LOCAL FUNCTION PROTOTYPES

*******************************************************************************/

/* SPR#1983 - SH - Add 'text' parameter */
static int		ATB_edit_Insert (T_ED_DATA *editor, T_ATB_TEXT *text, USHORT character);
static void		ATB_edit_FindNext(T_ED_DATA *editor);
static USHORT	ATB_edit_FindPrev(T_ED_DATA *editor);
static void		ATB_edit_OutTextLines (T_ED_DATA *editor);
static void		ATB_edit_WordWrap(T_ED_DATA *editor);
static void		ATB_edit_UpdateCursorPos(T_ED_DATA *editor);
static ED_RES	ATB_edit_Update (T_ED_DATA *editor, int  dy);
static void 	ATB_edit_LineDestroyAll(T_ED_DATA *editor);


/*******************************************************************************

 $Function:		ATB_edit_Create

 $Description:	Create the editor.  Allocate memory for the editor data and set up
 				some default parameters.
 
 $Returns:		Pointer to the editor data, or NULL if memory allocation failed.

 $Arguments:	editAttr	-	The editor attributes (caller allocated)
 				callback	-	Callback function

*******************************************************************************/

T_ED_DATA* ATB_edit_Create (T_ED_ATTR *editAttr, T_ED_CB callback)
{
    T_ED_DATA *editor;

	TRACE_FUNCTION("ATB_edit_Create()");

	/* Allocate memory for editor data */

	editor = (T_ED_DATA *) ATB_mem_Alloc(sizeof(T_ED_DATA));
	
    if (!editor)
        return NULL;
    
    /* Reset editor data */

    memset(editor, 0, sizeof(T_ED_DATA));
    editor->display = FALSE;
    editor->handler = callback;
    editor->attr = editAttr;
	editor->cursor.width = 8;
	editor->initialised = FALSE;
	editor->line = NULL;
	
    return editor;
}


/*******************************************************************************

 $Function:		ATB_edit_Destroy

 $Description:	Delete the editor.  Free the allocated memory.
 
 $Returns:		ED_OK			- OK
 				ED_BAD_HANDLE	- Editor data is null pointer

 $Arguments:	editor			-	The editor data

*******************************************************************************/

ED_RES ATB_edit_Destroy (T_ED_DATA *editor)
{
	ED_RES		result = ED_OK;
	
	TRACE_FUNCTION("ATB_edit_Destroy()");

	if (editor)
	{
		ATB_edit_LineDestroyAll(editor);
		
		if (editor->hiddenText)
		{
			ATB_edit_HiddenExit(editor);
		}
		
    	ATB_mem_Free((void *)editor, sizeof(T_ED_DATA));
	}
	else
		result = ED_BAD_HANDLE;
		
    return result;
}


/*******************************************************************************

 $Function:		ATB_edit_Init

 $Description:	Initialise the editor.  Ensures that valid combinations of editing modes
 				are set.  Sets uppercase/lowercase appropriately.  Moves the cursor
 				to the correct place in the text.  Performs a word-wrap.
 
 $Returns:		ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Init (T_ED_DATA *editor)
{
	TRACE_FUNCTION("ATB_edit_Init()");


   if (!editor)											/* If editor handle doesn't exist, return error */
    	return ED_BAD_HANDLE;

	/* Get the length of the supplied string */

	ATB_string_Length(&editor->attr->text);

	/* SPR#1983 - SH - Set CAPS preference after moving cursor,
	 * as any cursor moves now reset CAPS to LOWER case. */
	 
	ATB_edit_SetCase(editor, ED_CASE_LOWER);
	 
	/* Non read-only modes*/

	if (!ATB_edit_Mode(editor, ED_MODE_READONLY))
	{
		/* Initialise hidden mode */
		
		if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
		{
			ATB_edit_HiddenInit(editor);
			ATB_edit_SetMode(editor, ED_MODE_OVERWRITE);
		}

		/* Initialise formatted mode */

		if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))
		{
			ATB_edit_SetMode(editor, ED_MODE_OVERWRITE | ED_MODE_ALPHA);
			ATB_edit_MoveCursor(editor,ctrlTop,FALSE);	    /* Send cursor to start of string */
			
			/* Check for "M" & "m" formats; these set the case to upper/lower by default. */
			/* SPR#1983 - SH - Also, overwrite mode can be switched off for these
			 * cases, free entry allowed.*/
			
			ATB_edit_SetCase(editor, ED_CASE_CAPS);			/* Caps is on by default */

			if (strcmp(editor->attr->FormatString, "*M")==0)
			{
				ATB_edit_SetCase(editor, ED_CASE_UPPER);
				ATB_edit_ResetMode(editor, ED_MODE_OVERWRITE);
			}
			
			if (strcmp(editor->attr->FormatString, "*m")==0)
			{
				ATB_edit_SetCase(editor, ED_CASE_LOWER);
				ATB_edit_ResetMode(editor, ED_MODE_OVERWRITE);
			}

		}
		/* Of the cursor modes, only formatted starts at the top, others start at the bottom */
		else
		{
			ATB_edit_MoveCursor(editor, ctrlBottom, FALSE);

			/* SPR#1983 - SH - If the buffer is empty, first character will be capitalised.
			 * Otherwise, lowercase is the default. */
			 
			if (editor->attr->text.len==0)
			{
				ATB_edit_SetCase(editor, ED_CASE_CAPS);			/* Caps is on if first character */
			}
		}
	}

	/* No cursor modes */
	else
	{
		ATB_edit_MoveCursor(editor, ctrlTop, FALSE);
	}

	/* Format text */
	
	ATB_edit_Update(editor, 0);

	/* Make editor visible */
	
	editor->display = TRUE;                        		/* really show it           */

	return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Reset

 $Description:	Reset the editor - move the cursor to the start.
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Reset (T_ED_DATA *editor)
{

	TRACE_FUNCTION("ATB_edit_Reset()");

    if (!editor)
        return ED_BAD_HANDLE;            	/* Editor does not exist */

    editor->cursor.pos = 0;					/* Reset cursor position */
	editor->initialised = FALSE;			/* Fully wrap all of text */
	ATB_edit_Refresh(editor);				/* Refresh word wrap */
    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Show

 $Description:	Show the editor, if it is visible.
 
 $Returns:		ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Show (T_ED_DATA *editor)
{
	/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
	UBYTE		previousFont = (UBYTE) -1;								// store previous font
	USHORT		editX		= editor->attr->win_size.px;				// pos. of left of edit window
	USHORT		editY		= editor->attr->win_size.py;				// pos. of top of edit window
    USHORT		editWidth    = editor->attr->win_size.sx;       	  	// width of edit window
    USHORT		editHeight	= editor->attr->win_size.sy;				// height of edit window

	TRACE_FUNCTION("ATB_edit_Show()");
	
    if (!editor)
        return ED_BAD_HANDLE;									// Editor doesn't exist

    if (editor->display)
    {
    	resources_setColour(editor->attr->colour);

		if (editor->attr->font != (UBYTE) -1)
        	previousFont = dspl_SelectFontbyID(editor->attr->font);		// setup font
    
        dspl_Clear(editX,editY,editX+editWidth-1,editY+editHeight-1);	// Clear the editor window
	
		ATB_edit_OutTextLines(editor); 										// Show text

		/* Display cursor, if it's switched on and we're not in multitap */
		
	    if (editor->attr->cursor!=ED_CURSOR_NONE && !editor->multitap)
	    {
	        ATB_display_Cursor(&editor->attr->text, editor->cursor.pos, editor->attr->cursor, editX+editor->cursor.x,editY+editor->cursor.y,
	        	editor->cursor.width,editor->cursor.height);
	    }
    }

    if (previousFont != (UBYTE) -1)
        dspl_SelectFontbyID(previousFont);   							// Restore previous font

    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Refresh

 $Description:	Refresh the editor word wrap etc.
 
 $Returns:		ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Refresh (T_ED_DATA *editor)
{
	TRACE_FUNCTION("ATB_edit_Refresh()");

    if (!editor)
        return ED_BAD_HANDLE;        		    /* editor does not exist    */

    /* Get the length of the supplied string */

	ATB_string_Length(&editor->attr->text);

	/* Update word wrap */
	
 	ATB_edit_Update(editor, 0);

    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Hide

 $Description:	Hide the editor
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Hide (T_ED_DATA *editor)
{
	TRACE_FUNCTION("ATB_edit_Hide()");

    if (!editor)
        return ED_BAD_HANDLE;            /* element does not exist   */
    
    editor->display = FALSE;       /* editor is not visible    */
    
    if (editor->handler)                   /* call event handler       */
    {
		(void)(editor->handler(ED_INVISIBLE,editor));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    }
    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Unhide

 $Description:	Unhide the editor
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_Unhide (T_ED_DATA *editor)
{
	TRACE_FUNCTION("ATB_edit_Unhide()");

    if (!editor)
        return ED_BAD_HANDLE;            /* element does not exist   */

    editor->display = TRUE;
    
    if (editor->handler)                   /* call event handler       */
    {
		(void)(editor->handler(ED_VISIBLE,editor));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/
    }
    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_MoveCursor

 $Description:	Move the cursor
 
 $Returns:		ED_ERROR	- Unexpected cursor movement
 				ED_OK		- OK

 $Arguments:	editor		- The editor data
 				character	- The control character (ctrlLeft etc)
 				update		- TRUE if word wrap is to be carried out after move

*******************************************************************************/

ED_RES ATB_edit_MoveCursor (T_ED_DATA *editor, USHORT control, UBYTE update)
{
	USHORT altCursorPos;
	SHORT dy;
	
	TRACE_FUNCTION("ATB_edit_MoveCursor");
	
    if (!editor)
        return ED_BAD_HANDLE;            /* element does not exist   */

	/* Can't move cursor in hidden mode, except to start and end */

	dy = 0;
		
	switch(control)
	{
		case ctrlLeft:
		 	if (!ATB_edit_Mode(editor, ED_MODE_READONLY))			/* Not in read only mode*/
		 	{	
				if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))  		/* formatted input */
				{
	        		ATB_edit_FindPrev(editor);            			 /* find previous non-fixed character */
	        	}
	        	else
	        	{
	        		altCursorPos = editor->cursor.pos;					/* Store original cursor position */
	        		
	        		editor->cursor.pos = ATB_string_IndexMove(&editor->attr->text, editor->cursor.pos, -1);
	        			
		        	if (editor->cursor.pos==altCursorPos) 				/* If we're at the start of text */
					{
		        		ATB_edit_MoveCursor(editor, ctrlBottom, FALSE);	/* .. then go to the end of text! */
					}
				}
			}
			break;

		case ctrlRight:
			if (!ATB_edit_Mode(editor, ED_MODE_READONLY))			/* Not in read only mode*/
		 	{
				if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))  		/* formatted input */
				{
					ATB_edit_FindNext(editor);						/* Move cursor forward once */
				}
				else
				{
	        		altCursorPos = editor->cursor.pos;					/* Store original cursor position */
	        	
					editor->cursor.pos = ATB_string_IndexMove(&editor->attr->text, editor->cursor.pos, 1);
					if (editor->cursor.pos==altCursorPos) 				/* If we're at the end of text */
	        		{
	        			ATB_edit_MoveCursor(editor, ctrlTop, FALSE);	/* .. then go to the start of text! */
	        		}
	        	}
			}
			break;

		case ctrlUp:
			if (editor->cursor.line > 0)
	  			dy = -1;
	  		break;
	  		
		case ctrlDown:
			if (editor->cursor.line < (editor->numLines-1))
	  			dy = 1;
			break;

		case ctrlTop:
			editor->cursor.pos = 0;
			editor->initialised = FALSE;						/* We need to recalculate whole editor */

			/* For formatted mode, skip over any fixed characters at the start */
			
			if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))
			{
				editor->formatIndex = 0;
				editor->fieldIndex = 0;
				ATB_edit_FindNext(editor);
				ATB_edit_FindPrev(editor);
			}
			break;
			
		case ctrlBottom:
			/* Search until we find end of text */
			while (ATB_string_GetChar(&editor->attr->text, editor->cursor.pos)!=UNICODE_EOLN)
			{
	            editor->cursor.pos++;
			}
			editor->initialised = FALSE;						/* We need to recalculate whole editor */
			break;

		default:
			TRACE_EVENT("** ERROR ** Unexpected cursor movement.");
			return ED_ERROR;
			/*break;*/
	}

	/* SPR#1983 - SH - In caps mode, any keypress switches to lower case */

	if (editor->textcase==ED_CASE_CAPS)
		editor->textcase = ED_CASE_LOWER;

	if (update || dy!=0)
	{
		ATB_edit_Update(editor, dy);
	}
	
	return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_DeleteLeft

 $Description:	Delete the character to the left of the cursor
 				SPR#2342 - SH - Added flag 'update'
 
 $Returns:		ED_OK		- OK

 $Arguments:	editor		- The editor data
				update		- TRUE if word wrap is to be carried out after deletion

*******************************************************************************/

ED_RES ATB_edit_DeleteLeft (T_ED_DATA *editor, UBYTE update)
{
	ED_RES	result = ED_OK;
	USHORT	altCursorPos;
	char formatchar;
	
	TRACE_FUNCTION("ATB_edit_DeleteLeft");
	
    if (!editor)
        return ED_BAD_HANDLE;            /* element does not exist   */

	/* SPR#1983 - SH - Hidden text changes now mirror normal text changes. */

	/* Formatted mode */
	
	if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))			/* Formatted input */
    {
		if (ATB_edit_FindPrev(editor)!=FINDPREV_FIRST_CHAR)                 			/* Skip over fixed characters */
		{
			if (editor->formatIndex>0)													/* So we don't look back beyond start of string */
			{
				/* If we're in a delimited field, do normal delete, otherwise just cursor back */
				formatchar = editor->attr->FormatString[editor->formatIndex-1];
				if ((formatchar>'0' && formatchar<='9') || formatchar=='*')				/* If it's a number between 1 and 9, or a * */
				{
					ATB_string_MoveLeft(&editor->attr->text, editor->cursor.pos, 1);
					if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
					{
						ATB_string_MoveLeft(editor->hiddenText, editor->cursor.pos, 1);
					}
                                   //CRR:25546 - xpradipg - 26 Oct 2004
                                   //the if block is moved out from the ATB_edit_FindPrev()
                                   if (editor->fieldIndex==0)													// If we've reached the beginning of the field
   				            editor->formatIndex--;	
				}
			}
			result = ED_OK;
		}
		else
		{
			result = ED_DONE;  			/* Delete with empty buffer - escape from editor */
		}
	}

	/* Overwrite mode */
	
	else if (ATB_edit_Mode(editor, ED_MODE_OVERWRITE))
	{
		/* If we're at the end of the string, shift the '0' back */
		if (editor->cursor.pos>0)
		{
			if (editor->cursor.pos == editor->attr->text.len)
			{
				result = ATB_edit_MoveCursor(editor, ctrlLeft, FALSE);
				ATB_string_SetChar(&editor->attr->text, editor->cursor.pos, UNICODE_EOLN);
				editor->attr->text.len--;
				if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
				{
					ATB_string_SetChar(editor->hiddenText, editor->cursor.pos, UNICODE_EOLN);
					editor->hiddenText->len--;
				}
			}
			/* Otherwise, overwrite with a space */
			else
			{
				result = ATB_edit_MoveCursor(editor, ctrlLeft, FALSE);
				ATB_string_SetChar(&editor->attr->text, editor->cursor.pos, UNICODE_SPACE);
				if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
				{
					ATB_string_SetChar(editor->hiddenText, editor->cursor.pos, UNICODE_SPACE);
				}
			}
		}
		else
		{
			result = ED_DONE;				/* Delete with empty buffer - escape from editor */
		}
	}
	else
	{
		/* Otherwise, just perform normal delete operation */
    	altCursorPos = ATB_string_IndexMove(&editor->attr->text, editor->cursor.pos,-1);
        if (altCursorPos!=editor->cursor.pos)
        {
            ATB_string_MoveLeft(&editor->attr->text, altCursorPos, editor->cursor.pos-altCursorPos);	// Delete the difference!
	
			if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
			{
				ATB_string_MoveLeft(editor->hiddenText, altCursorPos, editor->cursor.pos-altCursorPos);	// Delete the difference!
			}
				
			editor->cursor.pos = altCursorPos;
			result = ED_OK;
        }
        else
        	result = ED_DONE;
	}
		
	/* SPR#1983 - SH - In caps mode, any keypress switches to lower case */

	if (editor->textcase==ED_CASE_CAPS)
		editor->textcase = ED_CASE_LOWER;

	 // Jul 22,2004 CRR:21605 xrashmic - SASKEN
	 // When deleting a character, if editor is empty, then set the case to 
	 // sentence case only if the user has not modified the case explicitly.
	if(editor->cursor.pos==0 && CaseChanged == FALSE)
	{
		editor->textcase = ED_CASE_CAPS;		
	}

	if (update)
	{
		ATB_edit_Update(editor,0);
	}
	
	return result;
}


/*******************************************************************************

 $Function:		ATB_edit_DeleteRight

 $Description:	Delete the character to the right of the cursor
 				SPR#2342 - SH - Added flag 'update'
 
 $Returns:		ED_OK		- OK

 $Arguments:	editor		- The editor data
				update		- TRUE if word wrap is to be carried out after deletion

*******************************************************************************/

ED_RES ATB_edit_DeleteRight (T_ED_DATA *editor, UBYTE update)
{
	char formatchar;

	TRACE_FUNCTION("ATB_edit_DeleteRight");
	
    if (!editor)
        return ED_BAD_HANDLE;            /* element does not exist   */
	
	/* SPR#1983 - SH - Hidden text changes now mirror normal text changes. */

	/* Formatted mode */
	
	if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))			/* Formatted input */
    {
		/* If we're in a delimited field, do normal delete right, otherwise ignore */
		if (editor->formatIndex>0)
		{
			formatchar = editor->attr->FormatString[editor->formatIndex-1];
			if ((formatchar>'0' && formatchar<='9') || formatchar=='*')				/* If it's a number between 1 and 9, or a * */
			{
				ATB_string_MoveLeft(&editor->attr->text, editor->cursor.pos, 1);
				if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
				{
					ATB_string_MoveLeft(editor->hiddenText, editor->cursor.pos, 1);
				}
			}
		}
	}

	/* Overwrite mode */
	
	else if (ATB_edit_Mode(editor, ED_MODE_OVERWRITE))
	{
		/* Make sure we're not at the end of the string */
		if (editor->cursor.pos<editor->attr->text.len)
		{
			if (!ATB_edit_Mode(editor, ED_MODE_HIDDEN))
			{
				ATB_string_SetChar(&editor->attr->text, editor->cursor.pos, UNICODE_SPACE);
			}
		}
	}
	else
	{
		ATB_string_MoveLeft(&editor->attr->text, editor->cursor.pos, 1);		/* Otherwise, just perform normal delete operation */
		if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
		{
			ATB_string_MoveLeft(&editor->attr->text, editor->cursor.pos, 1);		/* Otherwise, just perform normal delete operation */
		}
	}

	/* SPR#1983 - SH - In caps mode, any keypress switches to lower case, if we're
	 * not in multi-tap */

	if (!editor->multitap && editor->textcase==ED_CASE_CAPS)
		editor->textcase = ED_CASE_LOWER;

	if (update)
	{
		ATB_edit_Update(editor,0);
	}
	
	return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_ClearAll

 $Description:	Clear all text from the editor, move cursor to the top
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_DONE			- Deleted from start of editor, exit editor
 				ED_OK			- OK

 $Arguments:	editor		- The editor data

*******************************************************************************/

ED_RES ATB_edit_ClearAll (T_ED_DATA *editor)
{
	/* SPR#2275 - SH - No longer use variable 'textlength' */
	/* x0045876, 14-Aug-2006 (WR - variable "result" was set but never used) */
	/* ED_RES result; */
	
	TRACE_FUNCTION("ATB_edit_ClearAll()");

    if (!editor)
        return ED_BAD_HANDLE;            			// element does not exist
	
	/* FORMATTED MODE */
	
	if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))
	{
  		/* Find first non-fixed character.  SPR#2275 - Simplified */
		ATB_edit_MoveCursor(editor, ctrlTop, FALSE);													// starting from the top.
	}

	/* NORMAL MODE */
	
	else
	{
		if (editor->attr->text.len==0)
		{
			/* x0045876, 14-Aug-2006 (WR - variable "result" was set but never used) */
			/* result = ED_DONE; */
		}
		else
		{
	        memset(editor->attr->text.data,'\0',editor->attr->size*ATB_string_Size(&editor->attr->text));        /* Clear buffer */
			editor->attr->text.len = 0;
			if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))
			{
	        	memset(editor->hiddenText->data,'\0',editor->attr->size*ATB_string_Size(&editor->attr->text));
	        	editor->hiddenText->len = 0;
			}
			ATB_edit_Reset(editor);										/* Send cursor to start */

			/* x0045876, 14-Aug-2006 (WR - variable "result" was set but never used) */
		 	/* result = ED_OK; */
		}
	}

	ATB_edit_Update(editor,0);                 			/* Update word wrap & cursor */
	        
    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_Char

 $Description:	Insert a character into the editor text, or execute a control code
 
 $Returns:		ED_OK			- OK

 $Arguments:	editor		- The editor data
 				character	- The character - in unicode representation
 				update		- TRUE if word wrap is to be carried out after insertion

*******************************************************************************/

ED_RES ATB_edit_Char (T_ED_DATA *editor, USHORT character, UBYTE update)
{
	TRACE_FUNCTION("ATB_edit_Char()");

    if (!editor)
        return ED_BAD_HANDLE;            			// element does not exist

    switch (character)
    {
    	/* Quit editor */
    	
        case ctrlEscape:                  							             
            return ED_DONE;
        	/*break;*/

        /* Cursor movements*/
        
        case ctrlLeft:
        case ctrlRight:
        case ctrlUp:
        case ctrlDown:
        case ctrlTop:
        case ctrlBottom:
        	ATB_edit_MoveCursor(editor, character, update);
			break;

        /* Backspace */ 
        
        case ctrlBack:
        	ATB_edit_DeleteLeft(editor, update); /* SPR#2342 - SH */
       		break;

		/* Delete character under cursor */
		
        case ctrlDel:
			ATB_edit_DeleteRight(editor, update); /* SPR#2342 - SH */
			break;

		/* CR/LF */

        case ctrlEnter:
            character = UNICODE_LINEFEED;
            /* SPR#1983 - SH - Insert into normal buffer */
            if (ATB_edit_Insert(editor, &editor->attr->text, character))
            {
            	ATB_edit_MoveCursor(editor,ctrlRight,update);
            }
			break;
        
		/* Normal character */ 
            
        default:
	        /* SPR#1983 - SH - Insert into normal buffer */
            if (ATB_edit_Insert(editor, &editor->attr->text, character))
            {
				/* Character inserted OK.  Move cursor to right if we're not in multitap */
	            if (!editor->multitap)
	            {
		            ATB_edit_MoveCursor(editor,ctrlRight,FALSE);
		        }

	        	if (update)
				{
				  	ATB_edit_Update(editor, 0);
				}
			}
			break;
	}        

	/* SPR#1983 - SH - In caps mode, any keypress switches to lower case */
	
	if (!editor->multitap && editor->textcase==ED_CASE_CAPS)
		editor->textcase = ED_CASE_LOWER;
	
    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_AsciiChar

 $Description:	Insert an ascii character into the editor text, or execute a control code
 
 $Returns:		ED_OK			- OK

 $Arguments:	editor		- The editor data
 				character	- The ascii character
 				update		- TRUE if word wrap is to be carried out after insertion

*******************************************************************************/

ED_RES ATB_edit_AsciiChar (T_ED_DATA *editor, char character, UBYTE update)
{
	USHORT unicodeChar;
	
	if (character<ctrlMax)
		unicodeChar = (USHORT)(unsigned char)character;
	else
		unicodeChar = ATB_char_Unicode(character);
		
	return ATB_edit_Char(editor, unicodeChar, update);
}


/*******************************************************************************

 $Function:		ATB_edit_Insert

 $Description:	Insert a character into the editor text
 				SPR#1983 - SH - Added 'text' parameter.
 
 $Returns:		TRUE if character was inserted

 $Arguments:	editor		- The editor data
 				text		- The text string (normal or hidden buffer)
 				character	- The character - in unicode representation

*******************************************************************************/

static int ATB_edit_Insert (T_ED_DATA *editor, T_ATB_TEXT *text, USHORT character)
{
	int result = FALSE;
	
	TRACE_FUNCTION("ATB_edit_Insert()");

    if (!ATB_edit_Mode(editor, ED_MODE_READONLY))				// Can't insert into read only mode
    {
	    if (ATB_edit_Mode(editor, ED_MODE_OVERWRITE))									
	    {
	    	if (editor->cursor.pos < (editor->attr->size-1))	/* Make sure character will fit */
	    	{
	    		result = TRUE;									/* We can write the cahracter */
	    		
		    	/* If overwriting end of line, we need to increment length of string */
		    	if (ATB_string_GetChar(text, editor->cursor.pos)==UNICODE_EOLN)
		    	{
		    		
	    			/* Ensure string ends with end of line character */
	    			ATB_string_SetChar(text, editor->cursor.pos+1, UNICODE_EOLN);
	    			text->len++;
	    		}
		    }
	    }
	   	else /* For insert mode, check if we have space */
	    {
	        result = ATB_string_MoveRight(text, editor->cursor.pos, 1, editor->attr->size);	// move rest of text right to leave space
	    }

        if (result)
	    {
	    	ATB_string_SetChar(text, editor->cursor.pos, character);	// Insert the character
	    }
    }
    else
    {
    	result = FALSE;
    }
    
    return result;
}


/*******************************************************************************

 $Function:		ATB_edit_MultiTap 

 $Description:	Displays the specified character over the cursor

 $Returns:		None.

 $Arguments:	editor		- The editor data
 				character	- The character to display
 				multitap	- TRUE if multi-tap is currently in progress
 
*******************************************************************************/

ED_RES ATB_edit_MultiTap(T_ED_DATA *editor, USHORT character, BOOL multitap)
{
	UBYTE oldmultitap;
	ED_RES result = ED_OK;
	
	TRACE_FUNCTION("ATB_edit_MultiTap()");

	if (!editor)
        return ED_BAD_HANDLE;            			// element does not exist
        
	oldmultitap = editor->multitap;
	editor->multitap = multitap;
	
	/* If we were previously in multitap, and in insert mode, delete character under cursor.
	 * Since this deletes current character in both visible and
	 * hidden buffer, do this before inserting character to either buffer. */

	if (oldmultitap && !multitap && !ATB_edit_Mode(editor, ED_MODE_HIDDEN))
	{
		result = ATB_edit_MoveCursor(editor, ctrlRight, TRUE);
	}
	else
	{
		if (oldmultitap && !ATB_edit_Mode(editor, ED_MODE_OVERWRITE))
		{
			ATB_edit_DeleteRight(editor, FALSE); /* SPR#2342 - SH */
		}
		
		/* Hidden mode */

		if (ATB_edit_Mode(editor, ED_MODE_HIDDEN))							/* In hidden mode... */
	    {
		    ATB_edit_Insert(editor, editor->hiddenText, character);
		    
	    	if (!multitap)													/* n multi-tap, show character...*/
	    		character = UNICODE_STAR;									/* ...otherwise show star */
	    }

		result = ATB_edit_Char(editor,character,TRUE);
	}
		
    return result;
}


/*******************************************************************************

 $Function:		ATB_edit_AsciiMultiTap 

 $Description:	Displays the specified ascii character over the cursor

 $Returns:		None.

 $Arguments:	editor		- The editor data
 				character	- The ascii character to display
 				multitap	- TRUE if multi-tap is currently in progress
 
*******************************************************************************/

ED_RES ATB_edit_AsciiMultiTap(T_ED_DATA *editor, char character, BOOL multitap)
{
	USHORT unicodeChar;
	
	if (character<ctrlMax)
		unicodeChar = (USHORT)(unsigned char)character;
	else
		unicodeChar = ATB_char_Unicode(character);
		
	return ATB_edit_MultiTap(editor, unicodeChar, multitap);
}


/*******************************************************************************

 $Function:		ATB_edit_FindNext 

 $Description:	For formatted input, adds a character to the input buffer then finds
 				the next non-fixed character space for the cursor to occupy

 $Returns:		None

 $Arguments:	editor		- The editor data
 				character	- The character (or code) to insert
 
*******************************************************************************/

static void ATB_edit_FindNext(T_ED_DATA *editor)
{
    char			*format 		= editor->attr->FormatString;
   	char			formatchar;
	UBYTE			inField			= ENTRY_NOT_IN_FIELD;

	TRACE_FUNCTION("ATB_edit_FindNext()");

	// xreddymn - Included xrashmic's fix for 12470
	if(editor->attr->text.len==0)
	{
		return;
	}

	/* Check for delimited field */
	
	if (editor->formatIndex>0)
	{
		formatchar = format[editor->formatIndex-1];
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
			inField = ENTRY_IN_FIELD;
	}
	
	formatchar = format[editor->formatIndex];
	if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
		inField = ENTRY_ENTERING_FIELD;
	
	/* Check for cursor right at end of string - don't allow */
	
	if (editor->cursor.pos>=editor->attr->text.len
		&& editor->formatIndex>-1 && inField==ENTRY_NOT_IN_FIELD)
	{
		return;
	}

	/* Move cursor position right */

	editor->cursor.pos = ATB_string_IndexMove(&editor->attr->text, editor->cursor.pos, 1);

	/* Check for start of fixed input field */

	if (inField==ENTRY_ENTERING_FIELD)
	{
		editor->formatIndex++;														// Get us into the field...
		editor->fieldIndex = 0;														// ...and reset the field index
		formatchar = format[editor->formatIndex];
		if (formatchar=='M')
			editor->textcase = ED_CASE_UPPER;
		if (formatchar=='m')
			editor->textcase = ED_CASE_LOWER;
		inField = ENTRY_IN_FIELD;
	}

	/* Check whether we're in a fixed input field, e.g. "4N" or "8X" */

	if (inField==ENTRY_IN_FIELD)															// So we don't look back beyond start of string
	{
		formatchar = format[editor->formatIndex-1];
		editor->fieldIndex++;													// Increment the position in the field
		if (editor->fieldIndex==(int)(formatchar-'0'))								// If we've entered the number of characters specified (note- will never happen for the '*' !)
		{
			editor->formatIndex++;												// point to NULL at end of string (no more input)
		}
		return;
	}

	/* If not, just look at next format character as usual */
	
	editor->formatIndex++;															// Point to next character
	
	while (editor->formatIndex<(SHORT)strlen(format) && format[editor->formatIndex]=='\\')		// Fixed characters encountered
	{
		editor->cursor.pos = ATB_string_IndexMove(&editor->attr->text, editor->cursor.pos, 1);				// Skip over them
		editor->formatIndex+=2;
	}

	if (editor->formatIndex>(SHORT)(strlen(format)))											// Don't look beyond end of string
		editor->formatIndex = strlen(format);

	return;
}


/*******************************************************************************

 $Function:  ATB_edit_FindPrev 

 $Description:	For formatted input, finds the previous non-fixed character and
 				moves the cursor there if possible

 $Returns:		FINDPREV_NO_CHANGE if the cursor position is not changed (nowhere to go)
 				FINDPREV_PREV_FOUND if the previous character has been found
 				FINDPREV_FIRST_CHAR if the cursor was over the first non-fixed character
 				FINDPREV_LAST_CHAR if the cursor is at the last non-fixed character

 $Arguments:	editor	- The editor data
 
*******************************************************************************/

static USHORT ATB_edit_FindPrev(T_ED_DATA *editor)
{
    char			*format 		= editor->attr->FormatString;
   	char			formatchar;
   	SHORT			editIndex;

	TRACE_FUNCTION("ATB_edit_FindPrev()");

	/* Check if cursor is at start of string, return 2 */
	
	if (editor->cursor.pos == 0)
	{
		return FINDPREV_FIRST_CHAR;
	}
	
	/* Check whether we're in a fixed input field, e.g. "4N" or "8X" */
			
	if (editor->formatIndex>0)															// So we don't look back beyond start of string
	{
		formatchar = format[editor->formatIndex-1];
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')				// If it's a number between 1 and 9, or a *
		{
			if (editor->cursor.pos > 0)
				editor->cursor.pos--;
				
			if (editor->cursor.pos < editor->attr->size-1)									// (Don't decrement if at last char in string)
				editor->fieldIndex--;												// Decrement the position in the field
			
			if (editor->cursor.pos==editor->attr->text.len-1)		// Special case if last character - tell editor to shorten the string
				return FINDPREV_LAST_CHAR;
			
			return FINDPREV_PREV_FOUND;											// then we're done
		}
	}

	/* If not (or if we've just come out of one) just look at next format character as usual */

	editIndex	= editor->formatIndex-1;												// Make copy of format position, starting off to left
	 	
	while (editIndex>0)
	{
		if (format[editIndex-1]=='\\')										// If there's a fixed char
			editIndex -=2;														// Look back a further 2 characters
		else																	// If there's a non-fixed character
			break;																// then exit loop
   }

	if (editIndex==-1)															// Go back from 1st character in editor
	{
		return FINDPREV_FIRST_CHAR;
	}
	
	formatchar = format[editIndex-1];											
	if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
		editor->fieldIndex--;
		
	if (editIndex>-1)															// Provided there is somewhere to go....
	{
		while(editor->formatIndex>editIndex)
		{
			if (editor->cursor.pos > 0)
			{
				editor->cursor.pos--;					// move cursor there
				editor->formatIndex--;
			}
			if (format[editor->formatIndex]=='\\')
				editor->formatIndex--;
		}
		return FINDPREV_PREV_FOUND;												// Found new position
	}
	
	return FINDPREV_NO_CHANGE;													// Position unchanged
}


/*******************************************************************************

 $Function:		ATB_edit_GetCasePref 

 $Description:	Returns the preferred case for the current position in the editor

 $Returns:		ED_CASEPREF_NUM					- Any numeric character
 				ED_CASEPREF_ALPHA_UC			- Any symbolic or alphabetic uppercase
 											  character
 				ED_CASEPREF_ALPHA_LC			- Any symbolic or alphabetic lowercase
 											  character
   				ED_CASEPREF_ALPHANUM			- Any symbolic, numeric, or alphabetic
 											  character
 				ED_CASEPREF_ALPHANUM_UC			- Any symbolic, numeric, or alphabetic
 											  uppercase character
				ED_CASEPREF_ALPHANUM_LC			- Any symbolic, numeric, or alphabetic
 											  lowercase character

 $Arguments:	editor		- The editor data
 
*******************************************************************************/

T_ED_CASE_PREF ATB_edit_GetCasePref(T_ED_DATA *editor)
{
	T_ED_CASE_PREF	casePref;
	char			formatchar;
	char			*format;

	/* FORMATTED MODE */
	
	if (ATB_edit_Mode(editor, ED_MODE_FORMATTED))
	{
		format = editor->attr->FormatString;
		formatchar = format[editor->formatIndex];
	  
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')			// Delimiter for format field
		{
	       	editor->formatIndex++;
	       	editor->fieldIndex = 0;
	       	formatchar = format[editor->formatIndex];						// Next character is the format for the field
		}

	    switch(formatchar)
		{
			case 'X':														/* Uppercase alphanumeric */
				casePref = ED_CASEPREF_ALPHANUM_UC;
				break;
			case 'x':
				casePref = ED_CASEPREF_ALPHANUM_LC;								/* Lowercase alphanumeric */
				break;
			case 'A':														/* Uppercase alphabetic */
				casePref = ED_CASEPREF_ALPHA_UC;
				break;
			case 'a':														/* Lowercase alphabetic */
				casePref = ED_CASEPREF_ALPHA_LC;
				break;
			case 'M':
				casePref = ED_CASEPREF_ALPHANUM;
				break;
			case 'm':
				casePref = ED_CASEPREF_ALPHANUM;
				break;
			case 'N':
				casePref = ED_CASEPREF_NUM;
				break;
				
			default:
				casePref = ED_CASEPREF_NONE;
				break;
	    }
	}

	/* NORMAL MODE */
	else
	{
		casePref = ED_CASEPREF_NUM;	/* SPR#2342 - SH - Default to numeric mode */
				
		if (ATB_edit_Mode(editor, ED_MODE_ALPHA)
			&& ATB_edit_GetCase(editor)!=ED_CASE_NUM)
		{
			casePref = ED_CASEPREF_ALPHANUM;
		}
	}
	
	return casePref;
}


/*******************************************************************************

 $Function:		ATB_edit_OutTextLines

 $Description:	Draw the visible lines of text onto the screen
 
 $Returns:		None

 $Arguments:	editor		- The editor data

*******************************************************************************/

static void ATB_edit_OutTextLines (T_ED_DATA *editor)
{
	USHORT		editX			= editor->attr->win_size.px;				/* X position in editor */
	USHORT		editY			= editor->attr->win_size.py;				/* Y position in editor */
	USHORT		editWidth		= editor->attr->win_size.sx;				/* Height of the editor */
	USHORT		editHeight		= editor->attr->win_size.sy;				/* Height of the editor */
	USHORT		lineNo;
	USHORT		heightOnScreen;										/* Height of lines shown so far */
	USHORT		offsetX=0;											/* X offset of line */
	T_DS_TEXTFORMAT	tempFormat;										/* Temporary storage for format attributes */
	T_ED_LINE	*line;												/* Current line attributes */
	T_ATB_TEXT	currentLine;										/* Current line */
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	USHORT		titleIconWidth = 0;
	USHORT 		iconX, iconY;
	USHORT		titleHeight = 0;
#endif

	TRACE_FUNCTION("ATB_edit_OutTextLines()");

	if (editor == NULL)
		return;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
       if (editor->attr->TitleIcon.data != NULL && !editor->attr->TitleIcon.isTitle)
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

	if(titleIconWidth)
		editX = editX + titleIconWidth + 1;
#endif

	heightOnScreen = 0;
	line = ATB_edit_LineGet(editor, editor->winStartLine);

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	if(line != NULL)
		titleHeight = line->height;
#endif
	/*a0393213 lint warning removal - check 'line!=NULL' added*/
	for (lineNo = editor->winStartLine; lineNo < editor->numLines && heightOnScreen<=editHeight && line!=NULL; lineNo++)
	{
    	heightOnScreen 	+= line->height;							/* Add this height to the total height so far... */

		if (editor->display && heightOnScreen <= editHeight)		/* and make sure this fits onto the screen */
		{
			if (line->next!=NULL)
			currentLine.len = line->next->pos - line->pos;  		/* End of line is the first character of the next line */
			else  /*a0393213 lint warnings removal*/
				{
				currentLine.len=0;
				TRACE_ERROR("ATB_edit_OutTextLines():line missing");
				}
			currentLine.dcs = editor->attr->text.dcs;
			currentLine.data = &editor->attr->text.data[line->pos*ATB_string_Size(&currentLine)];

			offsetX = 0;
			if (line->format.attr & DS_ALIGN_RIGHT)
			{
				offsetX = editWidth-ATB_display_StringWidth(&currentLine, &line->format);
			}
			else if (line->format.attr & DS_ALIGN_CENTRE)
			{
				offsetX = (editWidth-ATB_display_StringWidth(&currentLine, &line->format))/2;
			}
			ATB_display_CopyFormat(&tempFormat, &line->format);		/* So format of lines doesn't change */
			ATB_display_Text(offsetX+editX, editY, &tempFormat, &currentLine);
		}

		editY += line->height;										/* Move down by line height, ready for the next one */
		line = line->next;											/* Get next line */
	}
//Sudha.V., x0035544, Feb 02, 2006, DR: OMAPS00061468
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	if(editor->attr->TitleIcon.data != NULL && !editor->attr->TitleIcon.isTitle)
	{
	switch(editor->attr->TitleIcon.display_type)
		{
		case SAT_ICON_NONE:
			break;
		case SAT_ICON_IDLEMODE_TEXT:
			
		        if ((editor->attr->TitleIcon.width > TITLE_ICON_WIDTH) || (editor->
attr->TitleIcon.height > TITLE_ICON_HEIGHT))
		        {
			         iconX = editX+offsetX - editor->attr->TitleIcon.width-2;
		              iconY = editor->attr->win_size.py+ ((titleHeight-2)/2);
			         dspl_BitBlt2(iconX, iconY, 8, 
					    10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);

		         }
		     else
		     {
			       iconX = editX+offsetX - editor->attr->TitleIcon.width-2;
		              iconY = editor->attr->win_size.py + ((titleHeight-2)/2) - (
editor->attr->TitleIcon.height / 2);
		              dspl_BitBlt2(iconX, iconY, editor->attr->TitleIcon.width, 
				        editor->attr->TitleIcon.height, (void*)editor->attr->TitleIcon.
data, 0, BMP_FORMAT_256_COLOUR);
		      }
		 break;

		 case SAT_ICON_DISPLAY_TEXT:
			
		        if ((editor->attr->TitleIcon.width > TITLE_ICON_WIDTH) || (editor->
attr->TitleIcon.height > TITLE_ICON_HEIGHT))
		       {
			      iconX = 1;
			      iconY = 1+ ((titleHeight-2) / 2) - (10 / 2);
			     dspl_BitBlt2(iconX, iconY, 8, 
					   10, (void*)SATIconQuestionMark, 0, BMP_FORMAT_256_COLOUR);

		       }
		    else
		    {
		           iconX = 1;
		           iconY = 1+ ((titleHeight-2) / 2) - (editor->attr->TitleIcon.
height / 2);
		          dspl_BitBlt2(iconX, iconY, editor->attr->TitleIcon.width, 
				    editor->attr->TitleIcon.height, (void*)editor->attr->TitleIcon.data, 0
, BMP_FORMAT_256_COLOUR);
		     }
		break;
	    }
		}
#endif

    return;
}


/*******************************************************************************

 $Function:		ATB_edit_Update

 $Description:	Update editor (without displaying), moving cursor up or down by 'dy' lines.

  This function goes through the text, word-wraps it with the help of ATB_edit_WordWrap,
  and works out the start position of text on-screen and the X and Y pixel positions
  of the cursor (with the help of ATB_edit_UpdateCursorPos).  The character position of
  the start of each line within the string is stored, so that ATB_edit_OutTextLines can
  quickly display the editor contents without further calculation.
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_OK			- OK

 $Arguments:	editor		- The editor data
 				dy			- number of lines (+ or -) that the cursor must scroll
 								up or down.

*******************************************************************************/

static ED_RES ATB_edit_Update (T_ED_DATA *editor, int dy)
{
	USHORT				cursorCharPos=0;			/* New cursor character position */
	USHORT				linePos;				/* Char pos in string of current line. */
	USHORT				lineNo;					/* Line being considered */
	USHORT				editComplete;			/* Flag indicating cursor position found/updated or end of line reached. */
	USHORT				character;				/* Current unicode char. */
	T_ED_LINE			*line;					/* Pointer to current entry in line chain */
	USHORT				cursorColumn;			/* Column cursor is in - always multiple of 8 pixels */
	SHORT				charMaxWidth;
	
	TRACE_FUNCTION("ATB_edit_Update()");

	/* Make sure the editor exists and has text in it */
	
	if (!editor)
		return ED_BAD_HANDLE;

	/* For non read-only modes, or on first process, perform word wrap */

	if (!ATB_edit_Mode(editor, ED_MODE_READONLY) || editor->initialised==FALSE)
	{
		editor->viewStartPos = 0;
		editor->viewHeight = 0;
		editor->totalHeight = 0;
		editor->startOfLine = TRUE;								/* We're at the start of a line */	
		editor->endOfText = FALSE;
		editor->precedingEOL = FALSE;							/* Used to detect if preceding character was CR/LF */
		editor->thischar.lineWidth = 0;							/* Width of current line */
		editor->cursor.line = 0;								/* Line that the cursor is on */
		editor->space.pos = 0;									/* Possible position for soft linebreak - none as yet */

		/* Set up data if this is the first time... */
		
		if (!editor->initialised)
		{
			editor->thischar.pos = 0;
			editor->numLines = 0;								/* total number of lines in editor, start at 0 */

			line = ATB_edit_LineGet(editor, 0);
			line->pos = 0;										/* first line starts at start of buffer */
			line->height = 0;									/* Height of first line */
			ATB_display_CopyFormat(&line->format, &editor->attr->startFormat);	/* Start with this text formatting */
			ATB_display_CopyFormat(&editor->thischar.format, &editor->attr->startFormat); // Start with this text formatting
			
			editor->winStartLine = 0;							/* First line to be displayed in window */

			editor->initialised = TRUE;
		}
		
		/* Set up data if this is NOT the first time */

		else
		{
			/* We only need to word wrap text that might have been changed by user text entry.
			 * The user can affect the previous line (by inserting a space or deleting, so the word is
			 * wrapped to a previous line) and all subsequent lines.  Therefore if we start word wrapping
			 * on the line previous to where the cursor is, we can encompass all changes. */
			line = ATB_edit_LineGet(editor, 0);
			for (lineNo = 0; lineNo<editor->numLines && editor->cursor.pos>=line->pos; lineNo++)
			{
				line = line->next;
			}
			if (lineNo>0)
				lineNo--;
			if (lineNo>0)
				lineNo--;

			line = ATB_edit_LineGet(editor, lineNo);
			editor->thischar.pos = line->pos;							/* Start looking at this line */						
			editor->numLines = lineNo;									/* This no. of lines so far */								
			ATB_display_CopyFormat(&editor->thischar.format, &line->format); /* Start with this text formatting */
			line->height = 0;											/* Height of first line */
		}


		/* Set up some values */
		
		cursorCharPos	= editor->cursor.pos;						/* Cursor position in the string */
		linePos			= 0;										// Position on the current line
			
		/* Word wrap the text into separate lines, storing the start position and height of each.
		 * Also, find the cursor and work out its X position. */
		
		while(!editor->endOfText)									// Go through from first character to last character
		{
			ATB_edit_WordWrap(editor);      						// What is the character?  What's its width?

			if (editor->endOfLine)									// Newline, or current char will not fit on previous line.
			{
				editor->numLines++;								// We've got another line
				editor->startOfLine = TRUE;						// New line is starting
			}
							
			if (editor->startOfLine)
			{
				line = ATB_edit_LineGet(editor, editor->numLines);
				line->pos = editor->startPos;
				line->height = editor->thischar.height; // Height of line set to that of first character
				ATB_display_CopyFormat(&line->format,&editor->thischar.format);
																	/* Formatting at start of line to that of 1st character */
				editor->thischar.lineWidth = 0;

				line = ATB_edit_LineGet(editor, editor->winStartLine);
				if (editor->startPos <= line->pos)	/* Is this the first line to be displayed in the editor? */
				{
					editor->winStartLine = editor->numLines;					/* If so, set this line to the start window line */
				}
				editor->startOfLine = FALSE;
			}

			if (editor->endOfText)												/* If it's a unicode terminator... */
			{
				if (cursorCharPos > editor->thischar.pos)						/* Cursor is past end of text - move to char before end of line. */
					cursorCharPos = editor->thischar.pos;
			}

			if (editor->startPos == cursorCharPos)					// We've found the cursor
			{
				editor->cursor.line 	= editor->numLines;				// Store the line it's on
				editor->cursor.width	= editor->thischar.width;			// Set the width of the cursor
				editor->cursor.x		= editor->thischar.lineWidth;			// And its position
				editor->cursor.attr		= editor->thischar.format.attr;			// Save the format attribute
			}

			editor->thischar.lineWidth += editor->thischar.width;
			editor->thischar.pos++;
		}					// End while

		editor->numLines++;										// Number of lines is one more than the last line
		line = ATB_edit_LineGet(editor, editor->numLines);
		line->pos = editor->thischar.pos;		// End of last line
		
		ATB_edit_UpdateCursorPos(editor);								// Update, but if dy!=0 we may have to change this
	}
	
	/* We now have the start position of each line and its height stored in an array.
	 * We also have the cursor's current X position on its line (but its line may be offscreen) */

	if (dy)														// If we're sending the cursor up/down some lines...
	{
		editor->cursor.line = editor->cursor.line+dy;	// Change cursor line
	
		if (editor->cursor.line >= editor->numLines )		// Make sure line cursor is on doesn't go beyond...
			editor->cursor.line = editor->numLines-1;		// ...last line of editor...

		if (editor->cursor.line < 0)					// ...or above...
			editor->cursor.line = 0;							// ...first line of editor

		/* In read-only mode, stop scrolling down when the bottom line of the text
		 * is visible at the bottom of the window */
		 
		if (ATB_edit_Mode(editor,ED_MODE_READONLY))
		{
			if (editor->numLines>=editor->linesPerScreen
				&& editor->cursor.line >= (editor->numLines - editor->linesPerScreen))
			{
				editor->cursor.line = (editor->numLines - editor->linesPerScreen);
			}
			
			editor->winStartLine = editor->cursor.line;
		}	
	}

	/* Reset all our horizontal variables */
	
	editor->thischar.pos		= 0;
	editor->thischar.lineWidth	= 0;
	editComplete				= TRUE;
	editor->endOfText			= FALSE;
	editor->space.pos			= 0;

	/* Work out how many lines fit on the current screen */

	ATB_edit_UpdateCursorPos(editor); 
	lineNo = 0;

	/* Update the line where we start showing the text on the window */
	
	if (editor->cursor.line < editor->winStartLine) 	//cursor is on a line before current window screen
	{
		editor->winStartLine = editor->cursor.line;
		editComplete = FALSE;
	}
	else if (editor->cursor.line >= (editor->winStartLine+editor->linesPerScreen)) //cursor is below the bottom of the screen
	{
		editor->winStartLine = editor->cursor.line-(editor->linesPerScreen-1);
		editComplete = FALSE;
	}
	
	if (dy!= 0)														/* If we're moving up or down */
	{
		editComplete = FALSE;
	}

	if (!editComplete)												/* Calculate new cursor X and Y positions */
	{
		if (dy!=0)													/* If we've changed line, find new X position */
		{
			line = ATB_edit_LineGet(editor, editor->cursor.line);
			editor->thischar.lineWidth = 0;
			linePos = line->pos;		// Start of current line
			ATB_display_CopyFormat(&editor->thischar.format, &line->format); // Format attributes of 1st character
			/* Get column - is always a multiple of the maximum character width.  Makes sure cursor doesn't wander
			 * left or right too much when moving up or down by lines */
			charMaxWidth = (SHORT)ATB_display_GetMaxCharWidth(&editor->thischar.format);
			cursorColumn = editor->cursor.x - (editor->cursor.x % charMaxWidth);

			linePos--;
			editor->thischar.width = 0;
			
			/* SPR#2342 - SH - Improved finding cursor position on adjacent lines.
			 * First search until we're in the column or at the end of the line */

			do
			{
				linePos++;
				editor->thischar.lineWidth += editor->thischar.width;
				character = ATB_string_GetChar(&editor->attr->text, linePos);
				editor->thischar.width = ATB_display_GetCharWidth(character, &editor->thischar.format);
			}
			while (editor->thischar.lineWidth<cursorColumn && linePos < (line->next->pos-1));

			/* Is there a character also in the column, but closer to our original character? */
			
			while ((editor->thischar.lineWidth+editor->thischar.width) < (cursorColumn+charMaxWidth)
				&& linePos< (line->next->pos-1)
				&& (editor->cursor.x - editor->thischar.lineWidth)> editor->thischar.width)
			{
				linePos++;
				editor->thischar.lineWidth += editor->thischar.width;
				character = ATB_string_GetChar(&editor->attr->text, linePos);
				editor->thischar.width = ATB_display_GetCharWidth(character, &editor->thischar.format);
			}
			
			/* Set the new cursor X position */
			cursorCharPos			= linePos;									// New cursor position in buffer
			editor->cursor.width	= editor->thischar.width;					// Set the width of the cursor
			editor->cursor.x		= editor->thischar.lineWidth;				// And its position
			editor->cursor.attr		= editor->thischar.format.attr;				// Save the format attribute
		}
		ATB_edit_UpdateCursorPos(editor);
	}

	/* Change cursor position */
	
	editor->cursor.pos = cursorCharPos;

    return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_WordWrap

 $Description:	Main word wrap function.  Takes in the characters of the string
				one by one, calculating the total width displayed on screen and setting flags
				when a string should be wrapped, a carriage return is encountered, the end
				of string has been reached.
				
				Tracks the last space character and goes back there when a word runs over
				the edge of the screen.  Also works out the height of the current line, based on
				the maximum character height found.
 
 $Returns:		None

 $Arguments:	editor		- The editor data

*******************************************************************************/

static void ATB_edit_WordWrap(T_ED_DATA *editor)
{
	USHORT		character;
	int			punctuation;
	T_ED_LINE	*line;

#ifdef TRACE_ATBEDITOR
	TRACE_FUNCTION("ATB_edit_WordWrap()");
#endif

	editor->endOfLine = FALSE;
	line = ATB_edit_LineGet(editor, editor->numLines);

	/* Get the character from the buffer */

	editor->startPos = editor->thischar.pos;
	character = ATB_string_GetChar(&editor->attr->text, editor->thischar.pos);
	
	/* Find the character's dimensions */

	/* If we're multi-tapping a character, or in fixed-width mode, it has the maximum width */
	
	if (editor->multitap && editor->thischar.pos==editor->cursor.pos)
		editor->thischar.width = ATB_display_GetCharWidth(UNICODE_WIDEST_CHAR, &editor->thischar.format);
	else
		editor->thischar.width = ATB_display_GetCharWidth(character, &editor->thischar.format);						// Character width
	
	editor->thischar.height = ATB_display_GetCharHeight(character, &editor->thischar.format);

	/* Check to see if last character was a CR/LF */

	if (editor->precedingEOL)
	{
		editor->endOfLine = TRUE;
		editor->precedingEOL = FALSE;
	}
	else										// otherwise, character included on line
	{
		if (editor->thischar.height > line->height)		// if height>height so far...
			line->height = editor->thischar.height;		// ...adjust the height so far
	}
			
	/* Set flags appropriate for the character */
	
	switch(character)
	{
		case UNICODE_EOLN:
			editor->endOfText = TRUE;							// We're at the end of the editor text
		break;

		case UNICODE_LINEFEED:
		case UNICODE_CR:
			editor->precedingEOL = TRUE;							// This is an end of line
		break;
		
		default:
		break;
	}

	/* Check if wrapping required */
	
	if ( (editor->thischar.lineWidth+editor->thischar.width)>editor->attr->win_size.sx )		// Current character will go off edge of editor
	{
		editor->endOfLine = TRUE;
//Mar 31, 2008 DR:OMAPS00164541 Shubhro (x0082844) -->Start 	//No need to check for current character, if its a space.
#if 0
		// If we've found a space, and it's a word wrapping mode */
		
		if (editor->space.pos > 0 && editor->precedingSpace==FALSE
			&& !ATB_edit_Mode(editor, ED_MODE_OVERWRITE)) 
		{
			editor->thischar.pos			= editor->space.pos;	// reset character position back to here
			editor->startPos				= editor->space.pos;	// character is space, so start of block=character pos
			editor->thischar.width			= editor->space.width;
			editor->thischar.height			= editor->space.height;
			ATB_display_CopyFormat(&editor->thischar.format,&editor->space.format);
			line->height = editor->space.lineHeight;
			editor->endOfText			= FALSE;					// If we're wrapping on an EOLN, we've gone back
		}
		editor->space.pos = 0;								// Reset space position to start of line
		editor->precedingSpace = FALSE;
#endif
	}
#if 0
	else
	{
		if (editor->precedingSpace)	// Remember enough info so that this point can be restored
		{
			editor->space.pos			= editor->startPos;				// Store position of start of current block, or just character pos.	
			editor->space.width			= editor->thischar.width;
			editor->space.height		= editor->thischar.height;
			ATB_display_CopyFormat(&editor->space.format,&editor->thischar.format);
			editor->space.lineHeight	= line->height;
			editor->precedingSpace		= FALSE;
		}
		
		punctuation = FALSE;
		if (character==UNICODE_SPACE)  								 // Wrap only on spaces
		{
			punctuation = TRUE;
		}
	
		if ((punctuation) && (editor->thischar.lineWidth > 0)) 					//A space is a good point for a soft break
		{
			editor->precedingSpace = TRUE;
		}
	}
#endif
//Mar 31, 2008 DR:OMAPS00164541 Shubhro (x0082844) -->End

	return;
}


/*******************************************************************************

 $Function:		ATB_edit_UpdateCursorPos

 $Description:	Update the cursor's vertical position, based on its position within
				the string.
 
 $Returns:		None

 $Arguments:	editor		- The editor data

*******************************************************************************/

static void ATB_edit_UpdateCursorPos(T_ED_DATA *editor)
{
	USHORT lineNo;
	USHORT	lineHeight;
	USHORT	editHeight = editor->attr->win_size.sy;
	T_ED_LINE *line;

#ifdef TRACE_ATBEDITOR
	TRACE_FUNCTION("ATB_edit_UpdateCursorPos()");
#endif

	editor->cursor.y = 0;										// Recalculate cursor Y position...
	editor->viewHeight = 0;										// ...and height of viewable screen...
	editor->totalHeight = 0;									// ...and total height of screen...
	editor->viewStartPos = 0;									// ...and the start pixel position of the view...
	editor->linesPerScreen = 0;								// ...and number of lines to the screen
	lineNo = 0;
		
	while (lineNo<editor->numLines)
	{
		line = ATB_edit_LineGet(editor, lineNo);

		lineHeight = line->height;
	
		if (lineNo==editor->cursor.line)
			editor->cursor.y = editor->viewHeight;							// Y position of cursor

		if (lineNo==editor->winStartLine)									// starting posn rel to start of editor text
			editor->viewStartPos = editor->totalHeight;
		
		if (lineNo>=editor->winStartLine && (editor->viewHeight+lineHeight)<=editHeight)
		{
			editor->viewHeight += lineHeight;
			editor->linesPerScreen++;
		}

		editor->totalHeight += lineHeight;
		lineNo++;
	}

	line = ATB_edit_LineGet(editor, editor->cursor.line);
	editor->cursor.height  = line->height;	// Change its height

	return;
}


/************************************/  
/* GLOBAL PROCEDURES				*/
/* Add/removing words in the editor    */
/************************************/

/*******************************************************************************

 $Function:		ATB_edit_InsertString

 $Description:	Insert a string at the cursor.
 
 $Returns:		ED_BAD_HANDLE	- Editor data pointer is null
 				ED_OK			- OK

 $Arguments:	editor		- The editor data
 				insText		- The text to insert

*******************************************************************************/

ED_RES ATB_edit_InsertString (T_ED_DATA *editor, T_ATB_TEXT *insText)
{
    T_ATB_TEXT	*text;
	int 		textIndex;
	USHORT		character;

	TRACE_FUNCTION("ATB_edit_InsertString()");

    if (!editor)													// Make sure editor exists
        return ED_BAD_HANDLE;
	
	if (insText==NULL || insText->len==0)
		return ED_OK;												/* No string to insert - trivial operation. */

	if (ATB_edit_Mode(editor, ED_MODE_READONLY)	)					/* Don't insert in read-only mode*/
		return ED_ERROR;
		
	text = &editor->attr->text;

	if ((text->len+insText->len) >= editor->attr->size)
		return ED_ERROR; 											/* String too long */ 

	/* Move text up by the length of insText */

	ATB_string_MoveRight(text, editor->cursor.pos, insText->len, editor->attr->size); 
	
	/* Copy string into buffer */
	
	for (textIndex=0; textIndex<insText->len; textIndex++)
	{
		character = ATB_string_GetChar(insText, textIndex);
		ATB_string_SetChar(text, editor->cursor.pos+textIndex, character);
	}

	editor->cursor.pos = editor->cursor.pos+insText->len;
	
	/* Reformat updated text */
	
	ATB_edit_Update(editor, 0);

    return ED_OK;
   }


/*******************************************************************************

 $Function:		ATB_edit_GetCursorChar

 $Description:	Return the character at a position offset from the current cursor
				position by the value supplied.
 
 $Returns:		The character, or UNICODE_EOLN if goes beyond bounds of string.

 $Arguments:	editor		- The editor data
 				offset		- The offset from the current cursor position from which
 								to get the character

*******************************************************************************/

USHORT ATB_edit_GetCursorChar(T_ED_DATA *editor, int offset)
{
	USHORT	textIndex;
	USHORT character;

	TRACE_FUNCTION("ATB_edit_GetCursorChar");
        
	textIndex = editor->cursor.pos+offset;

    /*a0393213 warnings removal-pointless comparison of unsigned integer with zero*/
	if (textIndex > editor->attr->text.len)
		character = UNICODE_EOLN;
	else
		character = ATB_string_GetChar(&editor->attr->text, textIndex);

	return character;
}


/*******************************************************************************

 $Function:		ATB_edit_CapitaliseWord

 $Description:	Returns TRUE if next word after cursor ought to be capitalised

 $Returns:		None.

 $Arguments:	editor	- The editor data

*******************************************************************************/

BOOL ATB_edit_CapitaliseWord(T_ED_DATA *editor)
{
	USHORT LastChar;
	USHORT CharBefore;

	/* First check to see if first word is to be capitalised */
	
	if (editor->textcase==ED_CASE_CAPS)
		return TRUE;

	/* If not, look at preceding characters */
	
	LastChar = ATB_edit_GetCursorChar(editor, -1);
	CharBefore = ATB_edit_GetCursorChar(editor, -2);

	if (LastChar==UNICODE_FULLSTOP || LastChar==UNICODE_EXCLAMATION
		|| LastChar==UNICODE_QUESTION || LastChar==UNICODE_EOLN)
		return TRUE;
		
	if (LastChar==UNICODE_SPACE)
		if(CharBefore==UNICODE_FULLSTOP || CharBefore==UNICODE_EXCLAMATION || CharBefore==UNICODE_QUESTION)
			return TRUE;

	return FALSE;
}

/*******************************************************************************

 $Function:		ATB_edit_FindCapital

 $Description:	returns the code of the input char converted to a capital. If char has no 
 				upper case equivalent returns original char.
 				Added for issue 1508
 
 $Returns:		UBYTE

 $Arguments:	char
 
*******************************************************************************/
	
USHORT ATB_edit_FindCapital(USHORT small_char)
{	char ascii_code= ATB_char_Ascii(small_char);	
		

		/*if "normal" character*/
		if (ascii_code>96 && ascii_code< 123)
			return (ATB_char_Unicode(ascii_code - 0x20));
		
		switch ((UCHAR)ascii_code) /*a0393213 lint warnings removal - typecast done*/
		{
			/* x0045876, 14-Aug-2006 (WR - statment unreachable) */
			case (130): return ATB_char_Unicode((char)154); /* break; */ /*U with umlaut*/
			case (132): return ATB_char_Unicode((char)142); /* break; */ /*A with umlaut*/
			case (148): return ATB_char_Unicode((char)153); /* break; */ /*O with umlaut*/
			default: return ATB_char_Unicode(ascii_code);
		}

}
/*******************************************************************************

 $Function:		ATB_edit_HiddenInit

 $Description:	Initialize editor for hidden mode.

 $Returns:    	None.

 $Arguments:  	editor	- The editor data

*******************************************************************************/

ED_RES ATB_edit_HiddenInit(T_ED_DATA *editor)
{
	USHORT  len = editor->attr->text.len;

	TRACE_FUNCTION("ATB_edit_HiddenInit()");

	if (!editor)
        return ED_BAD_HANDLE;            			// element does not exist
    if (editor->hiddenText)
    	return ED_ERROR;
    	
	/* get memory for the temporary buffer */
	editor->hiddenText = (T_ATB_TEXT *) ATB_mem_Alloc(sizeof(T_ATB_TEXT));
	editor->hiddenText->len = 0;
	editor->hiddenText->data = (UBYTE *)ATB_mem_Alloc(editor->attr->size*ATB_string_Size(&editor->attr->text));	

	/* copy text to the temporary buffer */
	ATB_string_Copy(editor->hiddenText, &editor->attr->text);

	/* overwrite the string in the editor buffer with stars */
    memset(editor->attr->text.data,'\0',editor->attr->size*ATB_string_Size(&editor->attr->text));        /* Clear buffer */
	editor->attr->text.len = 0;
	ATB_edit_Reset(editor);										/* Send cursor to start */

	while (editor->attr->text.len < len)
		ATB_edit_AsciiChar(editor,'*',FALSE);
	
 	return ED_OK;
}


/*******************************************************************************

 $Function:		ATB_edit_HiddenExit

 $Description:	Deinitialize editor for hidden mode.

 $Returns:    	None.

 $Arguments:  	editor	- The editor data

*******************************************************************************/

ED_RES ATB_edit_HiddenExit(T_ED_DATA *editor)
{
	TRACE_FUNCTION("ATB_edit_HiddenExit()");
	
	if (!editor)
        return ED_BAD_HANDLE;            			// element does not exist
    if (!editor->hiddenText)
    	return ED_ERROR;
    	
	/* For hidden mode, copy the hidden text into the buffer & free memory */	
    ATB_string_Copy(&editor->attr->text, editor->hiddenText);
    ATB_mem_Free ((void *)editor->hiddenText->data, editor->attr->size*ATB_string_Size(editor->hiddenText));
    ATB_mem_Free ((void *)editor->hiddenText, sizeof(T_ATB_TEXT));
    editor->hiddenText = NULL;

    /* x0045876, 14-Aug-2006 (WR - "ATB_edit_HiddenExit" should return a value) */
    return ED_DONE;
}


/*******************************************************************************

 $Function:		ATB_edit_Mode

 $Description:	Returns TRUE if the appropriate bits are set in the editor mode

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				mode	- The mode bits to check

*******************************************************************************/

void ATB_edit_SetAttr(T_ED_DATA *editor, T_ATB_WIN_SIZE *win_size, ULONG colour, UBYTE font, USHORT mode, USHORT cursor, T_ATB_TEXT *text, USHORT size)
{
	memcpy(&editor->attr->win_size, win_size, sizeof(T_ATB_WIN_SIZE));
	editor->attr->colour = colour;
	editor->attr->font = font;
	editor->attr->cursor = cursor;
	editor->attr->mode = mode;
	memcpy(&editor->attr->text, text, sizeof(T_ATB_TEXT));
	editor->attr->size = size;

    return;
}


/*******************************************************************************

 $Function:		ATB_edit_Mode

 $Description:	Returns TRUE if the appropriate bits are set in the editor mode

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				mode	- The mode bits to check

*******************************************************************************/

UBYTE ATB_edit_Mode(T_ED_DATA *editor, USHORT mode)
{
	UBYTE result;

	if (editor->attr->mode & mode)
		result = TRUE;
	else
		result = FALSE;

    return result;
}


/*******************************************************************************

 $Function:		ATB_edit_SetMode

 $Description:	Sets the appropriate bits in the editor mode

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				mode	- The mode bits to set

*******************************************************************************/

void ATB_edit_SetMode(T_ED_DATA *editor, USHORT mode)
{
	editor->attr->mode |= mode;

    return;
}


/*******************************************************************************

 $Function:		ATB_edit_ResetMode

 $Description:	Resets the appropriate bits in the editor mode

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				mode	- The mode bits to reset

*******************************************************************************/

void ATB_edit_ResetMode(T_ED_DATA *editor, USHORT mode)
{
	editor->attr->mode &= (~mode);

    return;
}


/*******************************************************************************

 $Function:		ATB_edit_SetStyle

 $Description:	Sets the appropriate bits in the editor style

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				format	- The format bits to set

*******************************************************************************/

void ATB_edit_SetStyle(T_ED_DATA *editor, USHORT style)
{
	UBYTE mask;

	mask = 0;
	
	switch(style)
	{
		case DS_ALIGN_LEFT:
		case DS_ALIGN_RIGHT:
		case DS_ALIGN_CENTRE:
			mask = DS_ALIGN_RIGHT | DS_ALIGN_CENTRE;
			break;
	}

	/* Switch off previous format */
	editor->attr->startFormat.attr &= (~mask);
	/* Switch on new format */
	editor->attr->startFormat.attr |= style;

    return;
}

/*******************************************************************************

 $Function:		ATB_edit_ResetFormat

 $Description:	Resets the appropriate bits in the editor format

 $Returns:    	None.

 $Arguments:  	editor	- The editor data
 				format	- The format bits to reset

*******************************************************************************/

void ATB_edit_ResetFormat(T_ED_DATA *editor, USHORT format)
{
	editor->attr->startFormat.attr &= (~format);

    return;
}


/*******************************************************************************

 $Function:		ATB_edit_GetCase

 $Description:	Returns the currently selected text case

 $Returns:    	ED_CASE_UPPER
 				ED_CASE_LOWER
 				ED_CASE_CAPS
 				ED_CASE_NUM
 				ED_CASE_NONE

 $Arguments:  	editor	- The editor data

*******************************************************************************/

UBYTE ATB_edit_GetCase(T_ED_DATA *editor)
{
	return (UBYTE)editor->textcase;
}


/*******************************************************************************

 $Function:		ATB_edit_SetCase

 $Description:	Changes the currently selected text case

 $Returns:    	None

 $Arguments:  	editor		- The editor data
 				textcase	- Case to select.  One of:
 				 			  ED_CASE_UPPER
 							  ED_CASE_LOWER
 							  ED_CASE_CAPS
 							  ED_CASE_NUM
 							  ED_CASE_NONE

*******************************************************************************/

void ATB_edit_SetCase(T_ED_DATA *editor, UBYTE textcase)
{
	editor->textcase = textcase;
	return;
}


/*******************************************************************************

 $Function:		ATB_edit_LineGet

 $Description:	Get the pointer to the requested line in the linked list
 				
 $Returns:		The required line structure

 $Arguments:	editor		- The text editor
 				lineNo		- The line required
				
*******************************************************************************/

T_ED_LINE *ATB_edit_LineGet(T_ED_DATA *editor, SHORT lineNo)
{
	T_ED_LINE *line = editor->line;
	SHORT thisLineNo;

	thisLineNo = 0;
	if (lineNo<0)
		lineNo = 0;

	/* If the first line doesn't exist... */
	
	if (line==NULL)
	{
		editor->line = (T_ED_LINE *)ATB_mem_Alloc(sizeof(T_ED_LINE));
		memset(editor->line, 0, sizeof(T_ED_LINE));
		editor->line->next = NULL;
		line = editor->line;
	}

	/* Search for the line required */
	
	while (thisLineNo<lineNo)
	{
		if (line->next==NULL)
		{
			line->next = (T_ED_LINE *)ATB_mem_Alloc(sizeof(T_ED_LINE));
			memset(line->next, 0, sizeof(T_ED_LINE));
			line->next->next = NULL;
		}
		line = line->next;
		lineNo--;
	}
	
	return line;
}


/*******************************************************************************

 $Function:		ATB_edit_LineDestroyAll

 $Description:	Destroy all entries in the line linked list
 				
 $Returns:		None

 $Arguments:	editor		- The text editor
				
*******************************************************************************/

static void ATB_edit_LineDestroyAll(T_ED_DATA *editor)
{
	T_ED_LINE *line = editor->line;
	T_ED_LINE *newLine;
		
	while (line!=NULL)
	{
		newLine = line->next;
		ATB_mem_Free((void *)line, sizeof(T_ED_LINE));
		line = newLine;
	}

	return;
}
#endif
