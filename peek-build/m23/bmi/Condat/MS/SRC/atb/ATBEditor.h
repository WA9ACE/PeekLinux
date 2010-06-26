/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    ATBEditor.h
 $Revision:		                                                      
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description:  Header file for ATB Editor component.

 REQUIRES: ATBCommon.h, ATBDisplay.h
                        
********************************************************************************

 $History: ATBEditor.h
       Feb 02, 2006 DR: OMAPS00061468 - x0035544.
       Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
       Solution:Enum is defined to support mode of SAT info display 
       
	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : Structure definition displaying title icon through editor.
  
 $End

*******************************************************************************/


#ifndef ATB_EDITOR_H
#define ATB_EDITOR_H

#ifndef DSPL_H
#include "dspl.h"
#endif

/**********************************************
 * CONSTANTS
 *
 *********************************************/

enum
{
	ED_CURSOR_NONE,			            /* No cursor, read only */
	ED_CURSOR_BAR,			           /* Vertical bar thin */
	ED_CURSOR_UNDERLINE,				/* Underline-style cursor */
	ED_CURSOR_BLOCK						/* Highlights current character */
};

enum
{
	ED_MODE_ALPHA			= 0x01,				/* Alphanumeric Multi-tap mode */
	ED_MODE_OVERWRITE		= 0x02,				/* Overwrite mode */
	ED_MODE_PREDTEXT		= 0x04,				/* Predictive text mode */
	ED_MODE_FORMATTED		= 0x08,				/* Formatted input mode for WAP */
	ED_MODE_HIDDEN			= 0x10,				/* Hidden mode - *'s appear in place */
	ED_MODE_READONLY		= 0x20,				/* Read only mode */
	ED_MODE_ALL				= 0xFFFF			/* Allows switching off of all modes */
};

/* Editor visibility */

enum
{
	ED_INVISIBLE			= 0,				/* Editor is not displayed */
	ED_VISIBLE				= 1					/* Editor is displayed */
};

/* Editor updates */

#define ED_UPDATE_DEFAULT		0				/* Update everything by default */
#define ED_UPDATE_TRIVIAL		1				/* Trivial change - no scrolling or reformatting req.*/
#define ED_UPDATE_PARTIAL		2				/* Partial change */
#define ED_UPDATE_FULL			3				/* Full update required */


/**********************************************
 * ED_RES
 *
 * Result of editor functions
 *********************************************/
 
typedef enum
{
	ED_OK,
	ED_BAD_HANDLE,
	ED_ERROR,
	ED_DONE
}
ED_RES;


/**********************************************
 * T_ED_CB
 *
 * Callback type
 *********************************************/
 
typedef int (*T_ED_CB) (ULONG, void*);


/**********************************************
 * EDITOR CONTROL CHARACTERS
 *
 *********************************************/
 
typedef enum                           
{
    ctrlNone = 0,							/* no valid edit control */
    ctrlLeft,							/* cursor left */
    ctrlRight,							/* cursor right */
    ctrlUp,								/* cursor up */
    ctrlDown,							/* cursor down */
    ctrlTop,							/* cursor to top of text */
    ctrlBottom,							/* cursor to end of text */
    ctrlBack,							/* backspace */
    ctrlDel,							/* delete */
    ctrlEnter,							/* return key */
    ctrlEscape,							/* escape key */
    ctrlMax								/* Highest control character */
} T_ED_CONTROLS;


/* This type indicates whether or not the cursor is within a fixed field */

enum
{
	ENTRY_NOT_IN_FIELD = 0,
	ENTRY_ENTERING_FIELD,
	ENTRY_IN_FIELD
};


/* Output types for ATB_edit_FindPrev() */

enum
{
	FINDPREV_NO_CHANGE,
	FINDPREV_PREV_FOUND,
	FINDPREV_FIRST_CHAR,
	FINDPREV_LAST_CHAR
};

//Sudha.V., x0035544, Feb 02, 2006 DR: OMAPS00061468
#ifdef FF_MMI_SAT_ICON
typedef enum
{
	SAT_ICON_NONE = -1,
	SAT_ICON_DISPLAY_TEXT,
	SAT_ICON_IDLEMODE_TEXT
} T_DISPLAY_TYPE;


// Shashi Shekar B.S., a0876501, 16-Jan-2006, OMAPS00061460
/* T_ATB_EDITOR_TITLE_ICON */

typedef struct
{
	UBYTE		width;
	UBYTE		height;
	char*		data;
	BOOL		isTitle;
	T_DISPLAY_TYPE display_type;
	BOOL 		selfExplanatory;

} T_ATB_EDITOR_TITLE_ICON;
#endif

/**********************************************
 * T_ED_ATTR
 *
 * Editor attributes, provided by user
 *********************************************/

typedef struct
{
    T_ATB_WIN_SIZE	win_size;				/* Editor position and size */
    ULONG			colour;					/* Foreground and background colour */
    UBYTE			font;					/* Character font */
    USHORT			cursor;					/* cursor type */
    USHORT			mode;					/* Edit Mode */
    T_ATB_TEXT		text;					/* Edit buffer */
    USHORT			size;              		/* Buffer size */
	char			*FormatString;			/* Formatting string for WAP formatted input */
	T_DS_TEXTFORMAT	startFormat;			/* Text format to be applied to the start of text */
// Shashi Shekar B.S., a0876501, 16-Jan-2006, OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	T_ATB_EDITOR_TITLE_ICON		TitleIcon;
#endif
} T_ED_ATTR;

/**********************************************
 * T_ED_LINE
 *
 * Information about a line of text in the editor
 *********************************************/

typedef struct T_ED_LINE_TAG
{
	UBYTE 					height;			/* The height of the line */
	USHORT					pos;			/* The position of the start of the line in the buffer */
	T_DS_TEXTFORMAT			format;			/* The format attributes at the start of the line */
	struct T_ED_LINE_TAG	*next;			/* The succeeding line */
} T_ED_LINE;


/**********************************************
 * T_ED_CHAR
 *
 * Information about a character in the editor
 *********************************************/

typedef struct
{
	USHORT 					width;			// The width of the character
	USHORT 					height;			// The height of the character
	USHORT					pos;			// The position of the character
	T_DS_TEXTFORMAT			format;				// The format attributes of the character
	USHORT					lineHeight;			// Height of the line, so far
	USHORT					lineWidth;			// Width of the current line (in pixels)
} T_ED_CHAR;


/**********************************************
 * T_ED_CURSOR
 *
 * Information about cursor
 *********************************************/

typedef struct
{
    SHORT					x;			// Cursor X position, relative to top left corner of editor
    SHORT					y;			// Cursor Y position, relative to top left corner of editor
    SHORT					width;		// Size of cursor
    SHORT					height;		// Height of cursor
    USHORT					attr;		// Format attributes under the cursor
    USHORT					pos;         /* cursor position          */
	SHORT					line;		// The line the cursor is on (with respect to the whole text)
} T_ED_CURSOR;


/**********************************************
 * T_ED_DATA
 *
 * Main editor information
 *********************************************/
 
typedef struct                
{
    T_ED_CB					handler;            /* event handler            */
    T_ED_ATTR				*attr;               /* editor attributes        */

	/* Various flags */
	
	BOOL					initialised;		// TRUE if important values have been set up
    BOOL					display;			/* Whether editor is hidden or not. */
    UBYTE					update;				/* Which portions of the editor to update */
	UBYTE					multitap;			/* Stores status of multi-tap */
	UBYTE					textcase;			/* Case of text entry (upper, lower, numeric etc) */

	/* Word wrap information */
	
    T_ED_CURSOR				cursor;				/* Properties of cursor */
	T_ED_CHAR				thischar;			/* Properties of current character */
	T_ED_CHAR				space;				/* Properties of the last space */
	T_ED_LINE				*line;				/* Properties of lines in editor */

	BOOL					startOfLine;		/* TRUE indicates the start of a text line */
	BOOL 					endOfLine;			/* TRUE indicates end of a text line */
	BOOL					endOfText;			/* TRUE indicates no more chars in text */
	BOOL					precedingEOL;		/* Character preceding current character is an End Of Line */
	BOOL					precedingSpace;		/* Character preceding current character is a space */

	USHORT					startPos;			/* Character position, or start of current block */
	USHORT					numLines;			/* Total number of lines in the editor */
	SHORT					winStartLine;		/* The first line to be displayed in the current window */
	USHORT					linesPerScreen;		/* No of lines that fit on current screen */

	/* Display properties */

    USHORT					totalHeight;		/* Total height (in pixels) of the whole text string */
    USHORT					viewHeight;			/* The height visible in the editor */
    USHORT					viewStartPos;		/* Start position of view in pixels */

	/* WAP delimited input information */

	SHORT					formatIndex;		// Position in formatting string
    SHORT					fieldIndex;			// Position within a delimited field

    /* Hidden input information */

    T_ATB_TEXT				*hiddenText;		/* Hidden text is stored in this buffer */ 
} T_ED_DATA;


/**********************************************
 * T_ED_CASE
 *
 * The case of the current text
 *********************************************/
typedef enum
{
	ED_CASE_UPPER,						/* Upper case */
	ED_CASE_LOWER,						/* Lower case */
	ED_CASE_CAPS,						/* Capitalise starts of sentences */
	ED_CASE_NUM							/* Numbers */
} T_ED_CASE;

 
/**********************************************
 * T_ED_CASE_PREF
 *
 * Preference for case input
 *********************************************/
 
typedef enum
{
	ED_CASEPREF_NONE,					/* End of string - no case */
	ED_CASEPREF_NUM,					/* Any numeric character */
 	ED_CASEPREF_ALPHA_UC,				/* Any symbolic or alphabetic uppercase character */
	ED_CASEPREF_ALPHA_LC,				/* Any symbolic or alphabetic lowercase character */
	ED_CASEPREF_ALPHANUM,				/* Any symbolic, numeric, or alphabetic character */
	ED_CASEPREF_ALPHANUM_UC,			/* Any symbolic, numeric, or alphabetic uppercase character */
	ED_CASEPREF_ALPHANUM_LC				/* Any symbolic, numeric, or alphabetic lowercase character */
} T_ED_CASE_PREF;


/**********************************************
 * FUNCTION PROTOTYPES
 *
 *********************************************/

ED_RES			ATB_edit_Init (T_ED_DATA *editor);
T_ED_DATA*		ATB_edit_Create (T_ED_ATTR *editAttr, T_ED_CB callback);
ED_RES			ATB_edit_Destroy (T_ED_DATA *editor);
ED_RES			ATB_edit_Reset (T_ED_DATA *editor);
ED_RES			ATB_edit_Show (T_ED_DATA *editor);
ED_RES			ATB_edit_Refresh (T_ED_DATA *editor);
ED_RES			ATB_edit_Hide (T_ED_DATA *editor);
ED_RES			ATB_edit_Unhide (T_ED_DATA *editor);

ED_RES			ATB_edit_Char (T_ED_DATA *editor, USHORT character, UBYTE update);
ED_RES			ATB_edit_AsciiChar (T_ED_DATA *editor, char character, UBYTE update);
ED_RES			ATB_edit_MultiTap(T_ED_DATA *editor, USHORT character, BOOL multitap);
ED_RES			ATB_edit_AsciiMultiTap(T_ED_DATA *editor, char character, BOOL multitap);
ED_RES			ATB_edit_MoveCursor (T_ED_DATA *editor, USHORT control, UBYTE update);
ED_RES			ATB_edit_DeleteLeft (T_ED_DATA *editor, UBYTE update); /* SPR#2342 - SH */
ED_RES			ATB_edit_DeleteRight (T_ED_DATA *editor, UBYTE update); /* SPR#2342 - SH */
ED_RES			ATB_edit_ClearAll (T_ED_DATA *editor);

ED_RES			ATB_edit_HiddenInit(T_ED_DATA *editor);
ED_RES			ATB_edit_HiddenExit(T_ED_DATA *editor);
ED_RES			ATB_edit_InsertString (T_ED_DATA *editor, T_ATB_TEXT *insText);/* Insert a word at the cursor pos */
USHORT			ATB_edit_GetCursorChar(T_ED_DATA *editor, int offset);
BOOL			ATB_edit_CapitaliseWord(T_ED_DATA *editor);
USHORT 			ATB_edit_FindCapital(USHORT small_char);
T_ED_LINE		*ATB_edit_LineGet(T_ED_DATA *editor, SHORT lineNo);

void			ATB_edit_SetAttr(T_ED_DATA *editor, T_ATB_WIN_SIZE *win_size, ULONG colour, UBYTE font, USHORT mode, USHORT cursor, T_ATB_TEXT *text, USHORT size);
UBYTE			ATB_edit_Mode(T_ED_DATA *editor, USHORT mode);
void			ATB_edit_SetMode(T_ED_DATA *editor, USHORT mode);
void			ATB_edit_ResetMode(T_ED_DATA *editor, USHORT mode);
void			ATB_edit_SetStyle(T_ED_DATA *editor, USHORT style);
UBYTE			ATB_edit_GetCase(T_ED_DATA *editor);
void			ATB_edit_SetCase(T_ED_DATA *editor, UBYTE textcase);
T_ED_CASE_PREF	ATB_edit_GetCasePref(T_ED_DATA *editor);
#endif

