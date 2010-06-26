/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookCallDetailsWindow.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the implementation of the call details window for the
	phone book module
                        
********************************************************************************
 $History: MmiBookCallDetailsWindow.c
       Feb 22, 2007 OMAPS00115777 a0393213(R.Prabakar)
       Description : Details of any Missed call number shows Invalid information
       Fix            : Data was not initialized properly. Now it's initialized properly and 
       the fix done as part of CRR 22638 reverted back as it isn't clean
       
       Aug 24, 2004        REF: CRR  22638 xrashmic
	Bug:  E-Sample switches off when you attempt to look at details of a number 
		  in call list. 
	Fix:   Using the data stored in global variable to avoid corrupt data.

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End
  // Issue Number : SPR#12874 on 31/03/04 by Rashmi.C.N.
*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include Files
                                                                              
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
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"

#include "mfw_sim.h"
#include "mfw_cm.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#endif

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMenu.h"
#include "mmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"
#include "mfw_ffs.h"
#include "MmiTimers.h"

#include "MmiBookShared.h"
#include "MmiCPHS.h"
#include "mmiColours.h"


#define CALLDETAILS_INFO_LEN 32


/*******************************************************************************
                                                                              
                                Private methods
                                                                              
*******************************************************************************/



/*******************************************************************************

 $Function:    	bookCallDetails

 $Description:	dialog function for the call details windows
 
 $Returns:		None.

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

static void bookCallDetails( T_MFW_HND win, USHORT event, SHORT value, void * parameter )
{
    T_MFW_WIN      *win_data = ((T_MFW_HDR *) win)->data;
    tBookStandard  *data     = (tBookStandard *)win_data->user;
#ifdef NEW_EDITOR
	T_MFW_PHB_ENTRY p_pEntry;
	tMmiPhbData     current = data->phbk->current;
	T_MFW_PHB_ENTRY *my = &current.entry[ current.selectedName - current.missedCallsOffset ];
	T_ATB_TEXT		nameText;
	T_ATB_TEXT		numberText;
	T_ATB_TEXT		otherText;
	char            *TempBuffer = (char *)ALLOC_MEMORY(CALLDETAILS_INFO_LEN);
	char			*time;
	char			*date;
    TRACE_FUNCTION( "bookCallDetails()" );

	/* SPR#1428 - SH - New Editor changes
	 * Moved all this stuff here from the WinCb - it shouldn't be there! */
 
	/* try to get the name from the phone book, null the structure out
	   if we can't find it at all
	*/

	if ( bookFindNameInPhonebook( (const char *) my->number, &p_pEntry ) )
	{
		bookCopyPhbName(my, &p_pEntry,PHB_MAX_LEN);
			my->name.len = p_pEntry.name.len;
	}
	else
	{
		my->name.len = 0;
	}

	numberText.dcs = ATB_DCS_ASCII;
	numberText.data = my->number;
	ATB_string_Length(&numberText);

#ifdef EASY_TEXT_ENABLED	
	if ( my->name.data[0] ==0x80)
	{	
		/* UNICODE */
		nameText.dcs = ATB_DCS_UNICODE;
		nameText.data = &my->name.data[2];
	}
	else
#endif /* EASY_TEXT_ENABLED */
	{
		/* ASCII */
		nameText.dcs = ATB_DCS_ASCII;
		nameText.data = my->name.data;
	}

	ATB_string_Length(&nameText);

	ATB_edit_ResetMode(data->editor, ED_MODE_READONLY);		/* Switch off read only to add text */
	ATB_edit_ClearAll(data->editor);						/* Blank the editor */
	ATB_edit_InsertString(data->editor, &nameText);
	ATB_edit_Char(data->editor, ctrlEnter, FALSE);
	ATB_edit_InsertString(data->editor, &numberText);
	ATB_edit_Char(data->editor, ctrlEnter, FALSE);
	
	if (CphsPresent() == TRUE)
	{
		sprintf(TempBuffer, "Line %d", my->line);
		otherText.dcs = ATB_DCS_ASCII;
		otherText.data = (UBYTE *)TempBuffer;
		ATB_string_Length(&otherText);
		ATB_edit_InsertString(data->editor, &otherText);
		ATB_edit_Char(data->editor, ctrlEnter, FALSE);
	}
	
	/* API - 10/09/03 - SPR2513 - Add support to display chinese Data & Time */
	/* API - 05/09/03 - SPR2357 - Get the character string for the date */
	
	date = MmiRsrcGetText(TxtCallDate);

	if(Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{
		otherText.dcs = ATB_DCS_UNICODE;
		otherText.data = (UBYTE *)date;
	}
	else
	{
		otherText.dcs = ATB_DCS_ASCII;
		otherText.data = (UBYTE *)date;
	}
	ATB_string_Length(&otherText);
	ATB_edit_InsertString(data->editor, &otherText);	

	/* API - 05/09/03 - SPR2357 - END */
	/* API - 10/09/03 - SPR2513 - END */	
	
	// API - 21-01-03 - 1566 - Date and Time displayed for Recent calls

	// change by Sasken ( Rashmi C N) on March 31th 2004
	// Issue Number : MMI-SPR-12874
	// Bug: Wrong date and time in the Missed call details.
	// Solution: converting the date and time into decimal format used in the 
    //           phonebook management.
		
	sprintf((char*)TempBuffer, "%.2d/%.2d/%.2d",(UBYTE)((my->date.day[0])<<4)+(UBYTE) my->date.day[1],
										  (UBYTE) ((my->date.month[0])<<4)+(UBYTE) my->date.month[1],
										(UBYTE) ((my->date.year[0])<<4)+(UBYTE) my->date.year[1]);
	otherText.dcs = ATB_DCS_ASCII;
	otherText.data = (UBYTE *)TempBuffer;
	ATB_string_Length(&otherText);
	ATB_edit_InsertString(data->editor, &otherText);
	ATB_edit_Char(data->editor, ctrlEnter, FALSE);

	

	/* API - 10/09/03 - SPR2513 - Add support to display chinese Data & Time */
	/* API - 05/09/03 - SPR2357 - Get the character string for the Time */
	
	time = MmiRsrcGetText(TxtCallTime);

	if(Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{
		otherText.dcs = ATB_DCS_UNICODE;
		otherText.data = (UBYTE *)time;
	}
	else
	{
		otherText.dcs = ATB_DCS_ASCII;
		otherText.data = (UBYTE *)time;
	}

	ATB_edit_InsertString(data->editor, &otherText);	
	
	/* API - 05/09/03 - SPR2357 - END */
	/* API - 10/09/03 - SPR2513 - END */	

	// API - 21-01-03 - 1566 - Date and Time displayed for Recent calls

	// change by Sasken ( Rashmi C N) on March 31th 2004
	// Issue Number : MMI-SPR-12874
	// Bug: Wrong date and time in the Missed call details.
	// Solution: converting the date and time into decimal format used in the 
    //           phonebook management.
	sprintf((char*)TempBuffer, "%.2d:%.2d", (UBYTE) ((my->time.hour[0])<<4)+(UBYTE) my->time.hour[1], 
										 (UBYTE) ((my->time.minute[0])<<4)+(UBYTE) my->time.minute[1]);

	otherText.dcs = ATB_DCS_ASCII;
	otherText.data = (UBYTE *)TempBuffer;
	ATB_string_Length(&otherText);
	ATB_edit_InsertString(data->editor, &otherText);

	ATB_edit_SetMode(data->editor, ED_MODE_READONLY);		/* Switch on read only again */


	FREE_MEMORY((void *)TempBuffer, CALLDETAILS_INFO_LEN);
	
	/* show the editor and update the soft keys
	*/
	/* SH - the string has changed, so we need to refresh the word-wrap */
	ATB_edit_Refresh(data->editor);
	win_show(data->win);

#else /* NEW_EDITOR */

	if ( event == CALL_DETAILS_INIT )
		winShow( data->win );

#endif /* NEW_EDITOR */
  }




/*******************************************************************************

 $Function:    	bookCallDetailsWinCB

 $Description:	Window call back function
 
 $Returns:		MFW_EVENT_CONSUMED

 $Arguments:	e, event to be handles, w, window handle
 
*******************************************************************************/

static int bookCallDetailsWinCB( MfwEvt e, MfwWin *w )
{
	tBookStandard * data = (tBookStandard *)w->user;
//	char		*Time;     // RAVI
//	char		*Date;     // RAVI

	TRACE_FUNCTION("bookCallDetailsWinCB");
	switch( e )
    {
        case MfwWinVisible:
		{
/* SPR#1428 - SH - New Editor changes - Just show editor, calculate text
 * string to display in bookCallDetails() */
#ifdef NEW_EDITOR
			ATB_edit_Show(data->editor);
			resources_setTitleColour(COLOUR_EDITOR);

			/* TITLE */
		
			dspl_Clear(0,0, SCREEN_SIZE_X-1, data->editor->attr->win_size.py-1);
			PROMPT(0,0,0,TxtDetails);
#else /* NEW_EDITOR */
        	char            TempBuffer[32];
			T_MFW_PHB_ENTRY p_pEntry;
			tMmiPhbData     current = data->phbk->current;
			T_MFW_PHB_ENTRY *my = &current.entry[ current.selectedName - current.missedCallsOffset ];

			/* default screen update prior to painting below
			*/
		    dspl_ClearAll();

			/* try to get the name from the phone book, null the structure out
			   if we can't find it at all
			*/

#ifdef NO_ASCIIZ
			{
			UBYTE result;

			if ( bookFindNameInPhonebook( (const char *) my->number, &p_pEntry ) )
			{


                //GW Copy name and length 
				bookCopyPhbName(my, &p_pEntry,PHB_MAX_LEN);
					my->name.len = p_pEntry.name.len;
			}
			else
				{
					TRACE_EVENT(">>>>Length set to zero");
					my->name.len = 0;
				}

			}
#else /* NO_ASCIIZ */

			if ( bookFindNameInPhonebook( (const char *) my->number, &p_pEntry ) )
				bookCopyPhbName(my, &p_pEntry, NAME_SCREEN_MAX); //GW Copy name
			else
				memset( (char*) my->name, '\0', NAME_SCREEN_MAX );
#endif /* NO_ASCIIZ */

			memset((char*)TempBuffer ,'\0',32);

			
			/* API - 1203 - 30/09/02 - Add the Data and the Time to the TempBuffer to be shown when the details of a
									   LDN, LMN etc is looked at. Currently commented out as the ACI does not store the
									   numbers at present and garbage would be displayed.
			*/

			/* API - 1542 - 10/01/03 - Update from issue above Added the values written to the structure by ACI to 
									   display them to the user.
			*/

			/* API - 05/09/03 - SPR2357 - Get the character string for the date and time */
			Time = MmiRsrcGetText(TxtCallTime);  
			Date = MmiRsrcGetText(TxtCallDate); 
			
			sprintf((char*)TempBuffer,"%s%d%d%s%d%d%s %s%d%d%s%d%d%s%d%d%s", 
				Time, (UBYTE) my->time.hour[0], (UBYTE) my->time.hour[1], ":", 
						(UBYTE) my->time.minute[0], (UBYTE) my->time.minute[1], "\n",
				Date, (UBYTE) my->date.day[0], (UBYTE) my->date.day[1],  "/", 
						(UBYTE) my->date.month[0], (UBYTE) my->date.month[1], "/",
						(UBYTE) my->date.year[0], (UBYTE) my->date.year[1], "\n");
			/* API - 05/09/03 - SPR2357 - END */

			

#ifdef NO_ASCIIZ
#ifdef EASY_TEXT_ENABLED	
/*MC SPR 1242, changed macros to function calls e.g FIRST_LINE_CHINESE->Mmi_layout_first_line()*/
				if (my->name.data[0] ==0x80)//if unicode
				{	//display name as unicode
					dspl_TextOut(0,0, DSPL_TXTATTR_UNICODE, (char *)my->name.data);
					if ((my->name.data[MAX_LINE+2] != 0) && (my->name.data[MAX_LINE+3] != 0))
						dspl_TextOut(0, Mmi_layout_first_line(), 0,  (char *) &my->name.data[MAX_LINE+2]);
					//display number
					dspl_TextOut(0, Mmi_layout_second_line(), 0,  (char *) my->number);
					if (strlen((char *) my->number) > MAX_LINE)
/*MC end*/						dspl_TextOut(0, Mmi_layout_second_line() + 8, 0,  (char *) &my->number[MAX_LINE]);
				}
				else
#endif /* EASY_TEXT_ENABLED */
				{
					if (CphsPresent() == TRUE)
					{
						sprintf( (char *) data->edtBuf, "%s%s%s%s%d%s%s",
						(char *) my->name.data,   "\n",
						(char *) my->number,"\n Line ",  my->line,"\n",
						TempBuffer );
					}
					else
						sprintf( (char *) data->edtBuf, "%s%s%s%s%s",
						(char *) my->name.data,   "\n",
						(char *) my->number,"\n",
						TempBuffer );
				
					/* show the editor and update the soft keys
					*/
					edtShow( data->edt );
				}

#else /* NO_ASCIIZ */
				if (CphsPresent() == TRUE)
					{
						sprintf( (char *) data->edtBuf, "%s%s%s%s%d%s%s",
						(char *) my->name,   "\n",
						(char *) my->number,"\n Line ",  my->line,"\n",
						TempBuffer );
					}
					else
						sprintf( (char *) data->edtBuf, "%s%s%s%s%s",
						(char *) my->name,   "\n",
						(char *) my->number,"\n",
						TempBuffer );
				
			/* show the editor and update the soft keys
			*/
					edtShow( data->edt );
#endif /* NO_ASCIIZ */
#endif /* NEW_EDITOR */

			displaySoftKeys( TxtNull, TxtSoftBack );
		}
		break;

        default:
		{
			/* default processing, we return IGNORED here and let the
			   default handler pick up the ball
			*/
			return MFW_EVENT_PASSED;
		}
    }

    return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookCallDetailsKeyCB

 $Description:	normal keyboard handler
 
 $Returns:		MFW_EVENT_CONSUMED in all cases

 $Arguments:	e, event to be handled, k keyboard handle
 
*******************************************************************************/

static int bookCallDetailsKeyCB( MfwEvt e, MfwKbd *k )
{
	T_MFW_HND     win       = mfwParent( mfw_header() );
    T_MFW_WIN     *win_data = ((T_MFW_HDR *)win)->data;
    tBookStandard *data     = (tBookStandard *) win_data->user;

	/* make sure we handle null keyboards
	*/
	if ( k == NULL )
		return MFW_EVENT_CONSUMED;

	/* deal with the incoming key code
	*/
    switch( k->code )
    {
		case KCD_MNUDOWN:
		{
			/* scroll down event
			*/

			if ( data->phbk->current.selectedName == data->phbk->current.status.used_entries-1 )
			{
	            data->phbk->current.selectedName = 0;
	        }
	        else
	        	data->phbk->current.selectedName++;
			
		}
        break;

        case KCD_MNUUP:
		{				
			/* scroll up event
			*/
	        if ( data->phbk->current.selectedName == 0 )
	        {
				data->phbk->current.selectedName = data->phbk->current.status.used_entries-1;
			}
			else
				data->phbk->current.selectedName--;
		}
        break;

        case KCD_CALL:
		{
			/* left, implies make call
			*/
			callNumber( data->phbk->current.entry[data->phbk->current.selectedName].number );
			bookPhonebookDestroy( data->phbk->win );
		}
		break;

        case KCD_RIGHT:
		{
			bookCallDetailsDestroy(data->win);
		}
        break;

        case KCD_HUP:
		{
			/* clear, destroy the window
			*/
			bookCallDetailsDestroy( data->win );
		}
		break;

        default:
		{
			/* no default behaviour required
			*/
		}
		break;
    }

	/* if we have scrolled then the behaviour needs to be to perform another
	   search to get the window updated correctly
	*/
	if ( ( k->code == KCD_MNUUP ) || ( k->code == KCD_MNUDOWN ) )
	{
		SEND_EVENT(data->win, 0, 0, 0);
        winShow( win );
	}

	/* and always return consumed
	*/
    return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:    	bookCallDetailsKbdLongCB

 $Description:	Keyboard Long Press event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, keyboard handle
 
*******************************************************************************/

static int bookCallDetailsKeyLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND		win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;

	/* hand a long clear event only
	*/
	if ( ( e & KEY_CLEAR ) && ( e & KEY_LONG ) )
		bookCallDetailsDestroy( data->win );

	return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookCallDetailsCreate

 $Description:	Creates the window and associates editors and keyboards
                with it.
 
 $Returns:		handle of the newly created window, or NULL if failure

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

static T_MFW_HND bookCallDetailsCreate( MfwHnd parent )
{
	T_MFW_WIN     *win_data;
	tBookStandard *data             = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) );
	T_MFW_WIN     *parent_win_data  = ((T_MFW_HDR *)parent)->data;
	T_phbk        *phbk             = (T_phbk *)parent_win_data->user;

	TRACE_FUNCTION ("phbk_call_details_create()");

	/* Deal with allocation failure on the data block
	*/
	if ( ! data )
		return NULL;

	/* create the window
	*/
	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookCallDetailsWinCB ) ) == NULL )
		return NULL;

	/* Associate the dialog and user data areas
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC) bookCallDetails;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
    win_data->user				= (void *) data;
	data->parent_win			= parent;

    /* Create the keyboards and editor objects, and associate a phone book handler
    */
	data->kbd      = kbdCreate( data->win, KEY_ALL,            (MfwCb) bookCallDetailsKeyCB      );
    data->kbd_long = kbdCreate( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookCallDetailsKeyLongCB );
	/*OMAPS00115777 - a0393213(R.Prabakar) - Details of any Missed call number shows Invalid information
	    The phonebook should be set before sending any event to 'call details window'*/
	data->phbk     = phbk;
	/* 1945 MZ Initialise the edtBuf   */
	memset(data->edtBuf,'\0', STANDARD_EDITOR_SIZE );

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr( &data->editor_attr, PHB_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT,
		ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_UNICODE/*SPR2102*/, 	(UBYTE*) data->edtBuf, STANDARD_EDITOR_SIZE);
	data->editor = ATB_edit_Create(&data->editor_attr,0);
	SEND_EVENT(data->win, 0, 0, 0);
	ATB_edit_Init(data->editor);
#else/*NEW_EDITOR*/
	bookSetEditAttributes( PHB_EDITOR, COLOUR_EDITOR_XX , 0, edtCurBar1, 0,
		(char*) data->edtBuf, STANDARD_EDITOR_SIZE, &data->attr );
	data->edt      = edtCreate( data->win, &data->attr, 0, 0 );
#endif /*NEW_EDITOR*/



	/* return the handle to the newly created window
	*/
    return data->win;
}


/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/



/*******************************************************************************

 $Function:    	bookCallDetailsStart

 $Description:	entry point for the call details window
 
 $Returns:		handle of newly created window, or NULL

 $Arguments:	parent, handle of the parent window
 
*******************************************************************************/

T_MFW_HND bookCallDetailsStart( MfwHnd parent )
{
	T_MFW_HND win;

	TRACE_FUNCTION( "bookCallDetailsStart()" );

	/* create and initialise the window
	*/
    if ( ( win = bookCallDetailsCreate( parent ) ) != NULL )
        SEND_EVENT( win, CALL_DETAILS_INIT, 0, 0 );

	/* return the handle, or NULL if it failed to be created correctly
	*/
    return win;
}



/*******************************************************************************

 $Function:    	bookCallDetailsDestroy

 $Description:	destroys the call details window
 
 $Returns:		None.

 $Arguments:	window, hande of the window to be destroyed
 
*******************************************************************************/

void bookCallDetailsDestroy( MfwHnd window )
{
	T_MFW_WIN     *win  = ((T_MFW_HDR *)window)->data;
	tBookStandard *data = (tBookStandard *)win->user;

	TRACE_FUNCTION ("bookCallDetailsDestroy()");

	if ( data )
	{
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		/* New editor isn't automatically destroyed with winDelete,
		 * so we destroy it here */
		if (data->editor)
		{
			ATB_edit_Destroy(data->editor);
		}
#endif
		winDelete( data->win );
		FREE_MEMORY( (unsigned char *) data, sizeof( tBookStandard ) );
	}
}






/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/
