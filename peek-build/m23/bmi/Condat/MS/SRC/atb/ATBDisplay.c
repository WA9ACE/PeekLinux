/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		ATB
 $File:			ATBDisplay.c
 $Revision:		1.0

 $Author:		SH - Condat(UK)
 $Date:			11/11/02

********************************************************************************

 Description: ATB/Display interface.



********************************************************************************

 $History: ATBDisplay.c

  15/03/02      Original Condat(UK) BMI version.
 $End

*******************************************************************************/

#ifndef ATB_DISPLAY_C
#define ATB_DISPLAY_C

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

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "gdi.h"
#include "font_bitmaps.h"
#include "dspl.h"
#include "ATBCommon.h"
#include "ATBDisplay.h"

/*
+--------------------------------------------------------------------+
| LOCAL FUNCTION PROTOTYPES                                         |
+--------------------------------------------------------------------+
*/

extern t_font_bitmap* get_bitmap(USHORT selected_code);
t_font_bitmap *ATB_display_GetBitmap(USHORT character);
void		ATB_display_ShowBitmap(int x,int y,t_font_bitmap* current_bitmap,T_DS_TEXTFORMAT *format,
			int xstart, int ystart, int width, int height);

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay            |
| STATE   : code                        ROUTINE : ATB_display_Cursor          |
+--------------------------------------------------------------------+

  PURPOSE : set cursor at specified position

*/

void ATB_display_Cursor (T_ATB_TEXT *text, USHORT textIndex, UBYTE type, SHORT x, SHORT y, USHORT width, USHORT height)
{
	T_ATB_TEXT charTxt;
	UBYTE charData[4];
	USHORT character;
	T_DS_TEXTFORMAT format;
	
	switch(type)
	{
		// bar cursor
		case DS_CURSOR_BAR:
		    dspl_DrawLine(x,y,x,y+height-1);
		break;

		// underscore cursor
		case DS_CURSOR_UNDERLINE:
			dspl_DrawLine(x,y+height-1, x+width-1, y+height-1);
		break;

		// block cursor
		case DS_CURSOR_BLOCK:
			charTxt.data = charData;
			charTxt.dcs = text->dcs;
			charTxt.len = 1;

			character = ATB_string_GetChar(text, textIndex);
			if (!ATB_char_IsVisible(character))
				character = UNICODE_SPACE;

			ATB_string_SetChar(&charTxt, 0, character);
			ATB_string_SetChar(&charTxt, 1, UNICODE_EOLN);

			ATB_display_SetFormatAttr(&format, 0, TRUE);
			ATB_display_Text(x,y, &format, &charTxt);
			break;
	}
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_Text        |
+--------------------------------------------------------------------+

  PURPOSE : draw 'length' characters of string 'text' at x,y in format 'format'.

*/

void ATB_display_Text(SHORT x, SHORT y, T_DS_TEXTFORMAT *format, T_ATB_TEXT *text)
{

	USHORT charStore;
	UBYTE attr;

	attr = 0;
		
	if (text->dcs==ATB_DCS_UNICODE)
		attr |= DSPL_TXTATTR_UNICODE;
	if (format->highlight)
		attr |= DSPL_TXTATTR_INVERS;

	charStore = ATB_string_GetChar(text, text->len);
	ATB_string_SetChar(text, text->len, UNICODE_EOLN);
	dspl_TextOut((USHORT)x, (USHORT)y, attr, (char *)text->data);
	ATB_string_SetChar(text, text->len, charStore);

	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_GetCharWidth       |
+--------------------------------------------------------------------+

  PURPOSE : get height of character in current font

*/

int ATB_display_GetCharWidth (USHORT character, T_DS_TEXTFORMAT *format)
{
	t_font_bitmap	*current_bitmap;
	int				width;

	if (character==UNICODE_WIDEST_CHAR)
	{
		width = ATB_display_GetMaxCharWidth(format);
	}
	else
	{
		current_bitmap = ATB_display_GetBitmap(character);
		if (!current_bitmap)
		{
			current_bitmap = ATB_display_GetBitmap(UNICODE_SPACE);
		}
		width = (int)current_bitmap->width;
	}


	//TRACE_EVENT_P2("Width of char %d is %d", character, width);
	
	return width;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_GetMaxCharWidth       |
+--------------------------------------------------------------------+

  PURPOSE : Get maximum width of a character in the current font and style

*/

int ATB_display_GetMaxCharWidth (T_DS_TEXTFORMAT *format)
{
	int width;
	
	width = 16;

	return width;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay            |
| STATE   : code                        ROUTINE : ATB_display_GetCharHeight       |
+--------------------------------------------------------------------+

  PURPOSE : get height of character in current font

*/

int ATB_display_GetCharHeight (USHORT character, T_DS_TEXTFORMAT *format)
{

	/*t_font_bitmap	*current_bitmap;
	int				height;

	current_bitmap = ATB_display_GetBitmap(character);
	if (!current_bitmap)
	{
		current_bitmap = ATB_display_GetBitmap(UNICODE_SPACE);
	}
	height = (int)current_bitmap->height;

	return height;*/

	return (int)dspl_GetFontHeight();
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATB Display            |
| STATE   : code                        ROUTINE :          |
+--------------------------------------------------------------------+

  PURPOSE : Return width of string in pixels.  NOTE: End of line characters
  will be included in the width - same width as a space - if the "length"
  parameter includes them.

  text - the text whose width is to be calculated
  length - the length of text to be considered
  format - pointer to the format attributes at the start of the string
*/

USHORT ATB_display_StringWidth(T_ATB_TEXT *text, T_DS_TEXTFORMAT *format)
{
	USHORT				width;
	USHORT				textIndex;
	USHORT				character;
	T_DS_TEXTFORMAT	tempFormat;									// Temporary storage for format attributes
	BOOL			endFound;

#ifdef TRACE_ATBEDITOR
	TRACE_FUNCTION("ATB Editor: ATB_display_StringWidth()");
#endif

	width = 0;
	textIndex = 0;
	endFound = FALSE;
	ATB_display_CopyFormat(&tempFormat, format);			// Make a copy of the original format

	while (!endFound)
	{
		character	=	ATB_string_GetChar(text, textIndex);	// Get the next character
		width		+=	ATB_display_GetCharWidth(character, &tempFormat);
		textIndex++;

		if(!(textIndex<text->len))
			endFound = TRUE;
	}

	return (width);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATB Display            |
| STATE   : code                        ROUTINE :          |
+--------------------------------------------------------------------+

  PURPOSE : Return height of tallest character in next 'length' characters of string
*/

USHORT ATB_display_StringHeight(T_ATB_TEXT *text, T_DS_TEXTFORMAT *format)
{
	USHORT				height;
	USHORT				currentHeight;
	USHORT				textIndex;
	USHORT				character;
	BOOL				endFound;
	T_DS_TEXTFORMAT		tempFormat;									// Temporary storage for format attributes

	TRACE_FUNCTION("ATB_display_StringHeight()");

	height = 0;
	textIndex = 0;
	endFound = FALSE;
	ATB_display_CopyFormat(&tempFormat, format);		// Store the original format
	
	while (!endFound)
	{
		character = ATB_string_GetChar(text, textIndex);		// Get the next character
		currentHeight = ATB_display_GetCharHeight(character, &tempFormat);
		if (currentHeight>height)
			height = currentHeight;
		textIndex++;

		if(!(textIndex<text->len))
			endFound = TRUE;
	}

	return (height);
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_SetFormatAttr  |
+--------------------------------------------------------------------+

  PURPOSE : Sets format attributes and highlighting
	attr	- the rich text formatting
	highlight - TRUE if highlighting is to be switched on for the whole string
*/

void ATB_display_SetFormatAttr(T_DS_TEXTFORMAT *format, USHORT attr, BOOL highlight)
{
	format->attr = attr;
	format->highlight = highlight;
	
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_ClearFormat   |
+--------------------------------------------------------------------+

  PURPOSE : Clears the formatting attributes
  format - pointer to the current formatting attributes
*/

void ATB_display_ClearFormat(T_DS_TEXTFORMAT *format)
{
	format->attr = 0;
	format->highlight = FALSE;
	return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c            |
| STATE   : code                        ROUTINE : ATB_display_ClearFormat   |
+--------------------------------------------------------------------+

  PURPOSE : Duplicates format attributes
  dest	- pointer to the destination format structure (caller allocated)
  src	- pointer to the source format structure (caller allocated)
*/

void ATB_display_CopyFormat(T_DS_TEXTFORMAT *dest, T_DS_TEXTFORMAT *src)
{
	memcpy((void *)dest, (void *)src, sizeof(T_DS_TEXTFORMAT));
	return;
}


/*
+------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ATBDisplay.c                |
| STATE   : code                        ROUTINE : ATB_display_GetBitmap  |
+------------------------------------------------------------------------+

  PURPOSE : Returns address of bitmap based on unicode number (our coding scheme)
*/

t_font_bitmap *ATB_display_GetBitmap(USHORT character)
{
	USHORT current_code;
	
	current_code	= (USHORT)(character<<8)+(USHORT)(character>>8);	// Swap bytes round for correct unicode
	return (t_font_bitmap *)get_bitmap(current_code);
}

#endif
