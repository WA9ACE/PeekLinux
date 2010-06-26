/*******************************************************************************

	CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       AUITextEntry.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description:
  Multi-tap and other text entry methods for AUI Editor.


********************************************************************************

 $History: AUITextEntry.c
 
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

#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiMain.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUITextEntry.h"

/*******************************************************************************

 STATIC DATA

********************************************************************************/
/*SPR 2175, increased KEY_PAD_MAX and changed tables to allow entry of some 
accented characters*/
#define KEY_PAD_MAX 6


/* API - 01/09/03 - SPR2371 - Move the new character to the end of the multitap sequence */
/* Alpha mode characters for multi-tap - lower case */

static UBYTE editAlpha [12][KEY_PAD_MAX] =          
{
    {'.',	',',		'+',		'-',		'=',		'0'},	/*Key 0*/				
    
    {' '	,'?',		'!',		';',		':',		'1'},	/*Key 1*/
    /*    										a grave		a umlaut*/
    {'a', 	'b',		'c',		'2',		'\x85', 	'\x84'}, 	/*Key 2*/
    /*          								e-grave		e-acute*/
    {'d',	'e',		'f',		'3',		'\x8a',		'\x83'}, 	/*Key 3*/
    /* 											i-grave*/
    {'g',	'h',		'i',		'4',		'\x8d',		'@'},	/*Key 4*/
    /**/
    {'j',	'k',		'l',		'5',		'&', 		'*'}, 	/*Key 5*/
    /*											o-grave		o-umlaut*/
    {'m',	'n',		'o', 		'6',		'\x95',		'\x94'},    /*Key 6*/ 
    /*														SS*/
	{'p',	'q',		'r',		's',		'7',		'\xe1'}, 	/*Key 7*/
	/*											u-grave		u-umlaut*/
	{'t',	'u',		'v',		'8',		'\x97',		'\x82'}, 	/*Key 8*/
	/**/
	{'w',	'x',		'y',		'z',		'9',		'#'},    /*Key 9*/
	
	{'*',	'*',		'*',		'*',		'*',		'*'}, 	/*Key star, not actually used for multitap*/
	
	{'#',	'#',		'#',		'#',		'#',		'#'}	/*Key hash, not actually used for multitap*/
};

/* Alpha mode characters for multi-tap - upper case */
 
static UBYTE editAlphaU [12][KEY_PAD_MAX] =      
{
	{'.',	',',		'+',		'-',		'=',		'0'}, /*Key 0*/
		
    {' ',	'?',		'!',		';',		':', 		'1'}, /*Key 1*/
/*												A-umlaut	cedilla*/
    {'A',	'B',		'C',		'2',		'\x8e', 	'\x81'}, /*Key 2*/
/*												E-acute*/
    {'D',	'E',		'F',		'3',		'\x90',		' '}, /*Key 3*/
    
    {'G',	'H',		'I',		'4',		'@', 		' '}, /*Key 4*/
    
    {'J',	'K',		'L',		'5',		'&',		'*'}, /*Key 5*/
/*												O-umlaut */
    {'M',	'N',		'O',		'6',		'\x99',		' '},  /*Key 6*/  
     /*														SS*/
	{'P',	'Q',		'R',		'S',		'7',		'\xe1'}, /*Key 7*/
/*												U-umlaut*/
	{'T',	'U',		'V',		'8',		'\x9A',		' '}, /*Key 8*/
	
	{'W',	'X',		'Y',		'Z',		'9', 		'#'}, /*Key 9*/
	
	{'*',	'*',		'*'			,'*',		'*', 		'*'}, /*Key star, not actually used for multitap*/
	
	{'#',	'#',		'#',		'#',		'#',		'#'}  /*Key hash, not actually used for multitap*/
};

/* API - 01/09/03 - SPR2371 - END*/

static void	*MMIdata;		/* Temporary storage for MMI pointer */

/*******************************************************************************

 LOCAL FUNCTION PROTOTYPES

********************************************************************************/

static int AUI_entry_Timer (MfwEvt event, MfwTim *timer);
static USHORT AUI_entry_Keypress(T_AUI_ENTRY_DATA *entry_data, MfwEvt event, MfwKbd *key);

/*******************************************************************************

 $Function:		AUI_entry_Create  

 $Description:	Initialises the text entry data.  Allocates memory.  Creates the timer.

 $Returns:		Pointer to the data.

 $Arguments:	win		- The window in which the editor is displayed
 				editor	- The editor
 				update	- The event to be sent to the window in order to update it
 
*******************************************************************************/

T_AUI_ENTRY_DATA *AUI_entry_Create(T_MFW_HND win, T_ED_DATA *editor, U32 update)
{
	T_AUI_ENTRY_DATA *entry_data = (T_AUI_ENTRY_DATA *)mfwAlloc(sizeof(T_AUI_ENTRY_DATA));
	
	entry_data->win			= win;
	entry_data->update		= update;
	entry_data->editor		= editor;
	/* SPR#1597 - SH - Always create this timer */
    entry_data->editTim		= tim_create(win, 1000, (MfwCb) AUI_entry_Timer);
    entry_data->Level		= KEY_PAD_MAX-1;
    entry_data->Key			= KCD_MAX;

    return entry_data;
}


/*******************************************************************************

 $Function:		AUI_entry_Destroy
 
 $Description:	Frees memory associated with text entry.

 $Returns:		none.

 $Arguments:	entry_data	- The text entry data
 
*******************************************************************************/

void AUI_entry_Destroy(T_AUI_ENTRY_DATA *entry_data)
{
    if (entry_data->editTim)
    {
        timDelete(entry_data->editTim);
    }

    mfwFree((void *)entry_data, sizeof(T_AUI_ENTRY_DATA));

    return;
}

/*******************************************************************************

 $Function:		AUI_entry_EditChar 

 $Description:	Sends a character to the text editor.

 $Returns:		None.

 $Arguments:	data		- The editor data
 				alphachar	- Character to be inserted, or NULL if key tables are to
 							be used
 				multitap	- TRUE if multitap is in progress
 
*******************************************************************************/

USHORT AUI_entry_EditChar (T_AUI_ENTRY_DATA *entry_data, USHORT alphachar, BOOL multitap)
{
	T_ED_DATA		*editor		= entry_data->editor;

	TRACE_FUNCTION("AUITextEntry: AUI_entry_EditChar()");

	/* If character is not provided, get it from the key table */

	if (alphachar==NULL)
	{
	    alphachar = AUI_entry_GetKeyChar(entry_data, ATB_edit_GetCasePref(editor));

		if (!(ATB_edit_Mode(entry_data->editor, ED_MODE_ALPHA) && entry_data->Key!=KCD_MAX))
	    	alphachar = UNICODE_SPACE;
	}

	ATB_edit_MultiTap(editor, alphachar, multitap);

    return alphachar;
}


/*******************************************************************************

 $Function:		AUI_entry_EditDigit

 $Description:	Sends a numeric digit/symbol to the editor

 $Returns:		None.

 $Arguments:	data		- The editor data
 				code		- The key code of the digit to be inserted
 
*******************************************************************************/

USHORT AUI_entry_EditDigit (T_AUI_ENTRY_DATA *entry_data, UBYTE code)
{
	T_ED_DATA		*editor		= entry_data->editor;
	USHORT			alphachar;

	TRACE_FUNCTION("AUITextEntry: AUI_entry_EditDigit()");

	alphachar = ATB_char_Unicode(editControls[code]);
	
	ATB_edit_MultiTap(editor, alphachar, FALSE);

    return alphachar;
}


/*******************************************************************************

 $Function:  AUI_entry_Timer  

 $Description:	 executes the timer event from the edit component

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/

static int AUI_entry_Timer (MfwEvt event, MfwTim *timer)
{
	T_AUI_ENTRY_DATA *entry_data		= (T_AUI_ENTRY_DATA *)MMIdata;

	TRACE_FUNCTION("AUIEditor: AUI_entry_Timer()");

    /* SPR#2174 - SH - Only display character if we're still in multi-tap */

    if (entry_data->editor->multitap)
    {
	    if (ATB_edit_Mode(entry_data->editor, ED_MODE_ALPHA))
	    {
		    AUI_entry_EditChar(entry_data, NULL, FALSE);
			
		    entry_data->Level = KEY_PAD_MAX-1;
		    entry_data->Key = KCD_MAX;
	    }

	    SEND_EVENT(entry_data->win, entry_data->update,0,0);
	}
	
    return 1;
}


/*******************************************************************************

 $Function:  	AUI_entry_StopMultitap  

 $Description:	Stops a multitap operation going on, cancelling the timer
				SPR#2174 - SH - Added
				
 $Returns:		none.

 $Arguments:	entry_data - The entry data
 
*******************************************************************************/

void AUI_entry_StopMultitap(T_AUI_ENTRY_DATA *entry_data)
{
	TRACE_FUNCTION("AUIEditor: AUI_entry_StopMultitap()");

	timStop(entry_data->editTim);

    entry_data->editor->multitap = FALSE;
    
    entry_data->Level = KEY_PAD_MAX-1;
	entry_data->Key = KCD_MAX; 
	
    return;
}


/*******************************************************************************

 $Function:		AUI_entry_EventKey  

 $Description:	Process a keypress event.

 $Returns:		The character that is entered, or NULL.

 $Arguments:	
 
*******************************************************************************/

USHORT AUI_entry_EventKey (T_AUI_ENTRY_DATA *entry_data, MfwEvt event, MfwKbd *key)
{
	T_ED_DATA		*editor		= entry_data->editor;
	USHORT			alphachar;												// The character entered

	TRACE_FUNCTION("AUIEditor: AUI_entry_EventKey");
	
	alphachar = AUI_entry_Keypress(entry_data, event, key);

	ATB_edit_Refresh(editor);
	
	return alphachar;
}


/*******************************************************************************

 $Function:		AUI_entry_Keypress  

 $Description:	Process a keypress event

 $Returns:		The character that is entered, or NULL.

 $Arguments:	
 
*******************************************************************************/

static USHORT AUI_entry_Keypress(T_AUI_ENTRY_DATA *entry_data, MfwEvt event, MfwKbd *key)
{
	T_ED_DATA		*editor			= entry_data->editor;
    USHORT			alphachar;										/* Current multi-tap character */
	T_ED_CASE_PREF	casePref;

	TRACE_FUNCTION("AUIEditor: AUI_entry_KeypressFormat()");
	
	alphachar = NULL;
			
	if (key->code!=KCD_HUP && editor->cursor.pos<(editor->attr->size-1)) /* SPR#1995 - SH */
	{		       
		/* Multi-tap: The user presses a different key, meaning the previous character must be entered */

    	if (entry_data->Key != KCD_MAX && entry_data->Key != key->code)
    	{
	        timStop(entry_data->editTim);

			AUI_entry_EditChar(entry_data, alphachar, FALSE);					// Show character, not multi-tap
			entry_data->Level = KEY_PAD_MAX-1;
	        entry_data->Key = KCD_MAX;
		}

		casePref = ATB_edit_GetCasePref(editor);	/* Work out the format character that will govern the format of this keypress */
			
        if (editor->cursor.pos < (editor->attr->size-1) && casePref!=ED_CASEPREF_NONE) /* SPR#1995 - SH */					// Make sure we can still show a character
        { 	
			if (key->code <= KCD_HASH)						/* SPR#1788 - SH - Now accept 0-9, * and # */
			{
				entry_data->Key = key->code;										// Store the key pressed
				
				/* Numeric */
				
				if (casePref==ED_CASEPREF_NUM)												// A numeric digit is expected
				{
					alphachar = AUI_entry_GetKeyChar(entry_data, casePref);
			        entry_data->Level = KEY_PAD_MAX-1;								// Reset multi-tap settings
					entry_data->Key = KCD_MAX;
			        AUI_entry_EditChar(entry_data, alphachar, FALSE);					// Show character, not multi-tap#
				}

				/* Multi-tap alphanumeric */

	  			else										// Valid format codes
	  			{
		          	alphachar = NULL;
		          	
					entry_data->Level++;												// Increment through list of symbols
					if (entry_data->Level > (KEY_PAD_MAX-1))							// List wraps round
						entry_data->Level = 0;
					
		    		alphachar = AUI_entry_GetKeyChar(entry_data, casePref);

					if (alphachar!=NULL)				// If we've found a character, and string isn't max size
					{
						AUI_entry_EditChar(entry_data, alphachar, TRUE);					// Add character, multi-tap
						/* Need to store this handle, so timer can get access to
		            	 * entry_data on callback */
		            	MMIdata = (void *)entry_data;
						timStart(entry_data->editTim);
					}
					else
						entry_data->Key = KCD_MAX;/*a0393213 warnings removal-KEY_MAX changed to KCD_MAX*/
				}
			}
		}
	}
	return alphachar;
}


/*******************************************************************************

 $Function:		AUI_entry_GetKeyChar

 $Description:	Return the character to be entered, based on the keypress

 $Returns:		the unicode character to be entered into the text

 $Arguments:	case		- the preference for the case of the output

 
*******************************************************************************/

USHORT AUI_entry_GetKeyChar(T_AUI_ENTRY_DATA *entry_data, T_ED_CASE_PREF casePref)
{
	T_ED_DATA		*editor		= entry_data->editor;
	USHORT			alphachar;
    USHORT			safetycounter;						/* Stops searching forever to find compatible character */
    BOOL			valid;						/* flag to indicate whether character is accepted */
    USHORT			upper;
    USHORT			lower;
    UBYTE			caseSel = ATB_edit_GetCase(editor);	/* The case selected by the user */

	TRACE_FUNCTION("AUI_entry_GetKeyChar");
	
	if (casePref==ED_CASEPREF_NUM || (casePref==ED_CASEPREF_ALPHANUM && caseSel==ED_CASE_NUM))
	{
		alphachar = editControls[entry_data->Key];
		if (entry_data->Key<=KCD_HASH)
			alphachar = alphachar << 8;					// Gives us the ascii in the upper byte of the unicode
	}
	else
	{
		valid = FALSE;
		alphachar = NULL;
		safetycounter = 0;
		
		while (!valid && safetycounter<KEY_PAD_MAX)
		{
			upper = (USHORT)editAlphaU[entry_data->Key][entry_data->Level];
			lower = (USHORT)editAlpha[entry_data->Key][entry_data->Level];

			switch(casePref)
			{
				case ED_CASEPREF_ALPHANUM:
					if (caseSel==ED_CASE_UPPER || caseSel==ED_CASE_CAPS)
						alphachar = upper;
					else
						alphachar = lower;
					valid = TRUE;
					break;
					
				case ED_CASEPREF_ALPHANUM_UC:
					alphachar = upper;
					valid = TRUE;
					break;

				case ED_CASEPREF_ALPHANUM_LC:
					alphachar = lower;
					valid = TRUE;
					break;

				case ED_CASEPREF_ALPHA_UC:
					alphachar = upper;
					valid = !ATB_char_IsNumeric(alphachar);
					break;
					
				case ED_CASEPREF_ALPHA_LC:
					alphachar = lower;
					valid = !ATB_char_IsNumeric(alphachar);
					break;
			}

			if (!valid)
			{															// find the next one that is,
				alphachar = NULL;
				safetycounter++;											// (but don't search forever)
				entry_data->Level++;
				if (entry_data->Level > (KEY_PAD_MAX-1))
					entry_data->Level = 0;
			}
		}

		alphachar = ATB_char_Unicode(alphachar);	/* SPR#2342 - SH - Convert from unicode properly */
	}	

	TRACE_EVENT_P1(">>> alphachar = %d", alphachar);
	
	return alphachar;
}
