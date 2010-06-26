/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       ATBDisplay.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description:



********************************************************************************

 $History: ATBDisplay.h

  15/03/02      Original Condat(UK) BMI version.

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : Added definitions for height & width of the title icon.


  $End

*******************************************************************************/

#ifndef ATB_DISPLAY_H
#define ATB_DISPLAY_H

// Shashi Shekar B.S., a0876501, 16-Jan-2006, OMAPS00061460
#ifdef FF_MMI_SAT_ICON
#define TITLE_ICON_WIDTH			10
#define TITLE_ICON_HEIGHT			16
extern const unsigned char SATIconQuestionMark[];
#endif

/*
 * T_DS_TEXTFORMAT
 * Data type to contain formatting attributes
 */
 
typedef struct
{
	UBYTE		attr;				/* Standard attributes */
	BOOL		highlight;			/* TRUE if highlighting is on */
 } T_DS_TEXTFORMAT;

#define	WHOLE_STRING	0xFFFF		// Used to indicate that the whole string is to be used for a particular function

/* Cursor types */

typedef enum
{
	DS_CURSOR_NONE = 0,
	DS_CURSOR_BAR,
	DS_CURSOR_UNDERLINE,
	DS_CURSOR_BLOCK
}
T_DS_CURSOR;

/* Alignments */

typedef enum
{
	DS_ALIGN_LEFT = 0,
	DS_ALIGN_RIGHT = 1,
	DS_ALIGN_CENTRE = 2
}
T_DS_ALIGN;

/* Local function prototypes */

void		ATB_display_Cursor (T_ATB_TEXT *text, USHORT textIndex, UBYTE type, SHORT x, SHORT y, USHORT width, USHORT height);
void		ATB_display_Text(SHORT x, SHORT y, T_DS_TEXTFORMAT *format, T_ATB_TEXT *text);
void		ATB_display_SetFormatAttr(T_DS_TEXTFORMAT *format, USHORT attr, BOOL highlight);
void		ATB_display_ClearFormat(T_DS_TEXTFORMAT *format);
void		ATB_display_CopyFormat(T_DS_TEXTFORMAT *dest, T_DS_TEXTFORMAT *src);
int			ATB_display_GetCharWidth (USHORT character, T_DS_TEXTFORMAT *format);
int			ATB_display_GetMaxCharWidth (T_DS_TEXTFORMAT *format);
int			ATB_display_GetCharHeight (USHORT character, T_DS_TEXTFORMAT *format);
USHORT		ATB_display_StringWidth(T_ATB_TEXT *text, T_DS_TEXTFORMAT *format);
USHORT		ATB_display_StringHeight(T_ATB_TEXT *text, T_DS_TEXTFORMAT *format);
#endif
