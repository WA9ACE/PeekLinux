/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookSDNWindow.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the service numbers window for the phone book
                        
********************************************************************************
 $History: MmiBookSDNWindow.c

    CRR: 25302 - xpradipg 10 Nov 2004
    Description: Should be able to select the number fro the phonebook while
    sending MMS/EMS.
    Solution: The new feature to select the number from phonebook while sending
    MMS/EMS is added
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

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
#include "mfw_cnvt.h"	//GW Added for 'mfw_SIM2GsmStr'

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

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

#include "mmiColours.h"




/*******************************************************************************
                                                                              
                                Private methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookSDN

 $Description:	window dialog function
 
 $Returns:		none.

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

static void bookSDN( T_MFW_HND win, USHORT event, SHORT value, void * parameter )
{
    T_MFW_WIN       *win_data = ((T_MFW_HDR *) win)->data;
    tBookStandard   *data = (tBookStandard *)win_data->user;
	MfwMnu          *mnu;
	tMmiPhbData     *my;

    TRACE_FUNCTION ("bookSDN()");

	/* Guard against bad incoming data
	*/
	if ( ! data )
		return;

	/* Grab the current phonebook data element, we use this a lot
	*/
	my = &data->phbk->current;

	/* simple event handler, dealing with the messages we know about
	*/
    switch (event)
	{
		case SEARCH_INIT:
		{
  			/* Search initialise, no action required
			*/
		}
		break;

		case SEARCH_SCROLL_UP:
		{
			/* Scroll up event we need to deal with correctly
			*/
			if ( ( my->index==1 ) && ( my->selectedName == 0 ) )
			{
				/* select the correct boundary for the list
				*/
				my->selectedName = my->status.used_entries;
				if ( my->selectedName > MAX_SEARCH_NAME ) 
					my->selectedName = MAX_SEARCH_NAME;

				/* and subtract one cos we are dealing with an array starting
				   at zero
				*/
				my->selectedName--;

				/* and calculate the correct index value
				*/
				my->index = my->status.used_entries - my->selectedName;

			}
			else
			{
				if ( my->selectedName == 0 )
					my->index--;
				else
					my->selectedName--;
			}

			/* update menu structures
			*/
			mnu = (MfwMnu *)mfwControl( data->menu );
			mnu->lCursor[ mnu->level ] = my->selectedName;

			/* and go find the name
			*/
			bookFindName( MAX_SEARCH_NAME, &data->phbk->current );
		}
		break;

		case SEARCH_SCROLL_DOWN:
		{
			/* scroll down event is a bit easier than the scroll up
			*/
			if ( (my->index + my->selectedName) == my->status.used_entries )
			{
				my->index = 1;
				my->selectedName = 0;
			}
			else
			{
				if ( my->selectedName == (MAX_SEARCH_NAME - 1) )
					my->index++;
				else
					my->selectedName++;
			}

			/* update menu structures
			*/
			mnu = (MfwMnu *)mfwControl( data->menu );
			mnu->lCursor[ mnu->level ] = my->selectedName;

			/* and go find the name
			*/
			bookFindName( MAX_SEARCH_NAME, &data->phbk->current );
		}
		break;

		case SEARCH_STRING:
		{
			/* search for the current element selected in our buffer
			*/
			my->index = my->index + my->selectedName;

			if ( bookSearchName( data->edtBuf, MAX_SEARCH_NAME, &data->phbk->current ) == MFW_PHB_OK )
			{
				if ( my->index > (my->status.used_entries - MAX_SEARCH_NAME + 1) )
				{
					/* need to reorganise a bit here
					*/
					if ( my->status.used_entries > MAX_SEARCH_NAME )
					{
						my->selectedName  = my->index - my->status.used_entries;
						my->selectedName += MAX_SEARCH_NAME - 1;
						my->index        -= my->selectedName;
					}
					else
					{
						my->selectedName = my->index - 1;
						my->index        = 1;
					}

					/* find the appropriate name
					*/
					bookFindName( MAX_SEARCH_NAME, &data->phbk->current );
				}
				else
					data->phbk->current.selectedName = 0;

				/* update menu structures
				*/
				mnu = (MfwMnu *)mfwControl(data->menu);
				mnu->lCursor[mnu->level] = data->phbk->current.selectedName;
			}
		}
		break;

		case SEARCH_UPDATE:
		{
			/* Update search message
			*/
			my->index = 1;
			my->selectedName = 0;
			memset( data->edtBuf, '\0', sizeof( data->edtBuf ) );
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_Reset( data->editor );
#else /* NEW_EDITOR */
			edtReset( data->edt );
#endif /* NEW_EDITOR */

			/* deal with the new search using the method above
			*/
			SEND_EVENT( win, SEARCH_STRING, 0, 0 );
		}
		break;

/* SPR#1428 - SH - New Editor: added event so we can
 * prompt a redraw when required. */
#ifdef NEW_EDITOR
		case SEARCH_REDRAW:
			win_show(data->win);
			break;
#endif

		default:
		{
			/* no default handler, just ignore any other messages
			*/
		}
		break;

	}

}




/*******************************************************************************

 $Function:    	bookSDNWinCB

 $Description:	Window call back function
 
 $Returns:		status indicating if event handled or not

 $Arguments:	e, event, w, window handle
 
*******************************************************************************/

static int bookSDNWinCB( MfwEvt e, MfwWin *w )
{
    tBookStandard *data = (tBookStandard *)w->user;

	char *ElemPtr, *NumPtr;

	/* x0045876, 14-Aug-2006 (WR - "Position" was set but never used) */
	/* int  leftKey, rightKey, ElemSize, i, Position;  */
	int  leftKey, rightKey, ElemSize, i;

    TRACE_FUNCTION ("phbk_sdn_win_cb()");

	switch (e)
    {
        case MfwWinVisible:
		{
			/* Paint function, clear the screen and paint in each
			   of our controls
			*/
			dspl_ClearAll();

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
			edtShow(data->edt);
#endif /* NEW_EDITOR */

			/* Populate the names screen buffer
			*/
			for (i=0;i<MAX_SEARCH_NAME;i++)
			{
				/* Grab info for this element
				*/
				ElemPtr  = MmiBookCallList(i);
				ElemSize = MmiBookCallListSize(i);

				/* Decide on whether to use number or name
				*/
#ifdef NO_ASCIIZ				
				NumPtr   = (char*)data->phbk->current.entry[i].name.data;
#else
				NumPtr   = (char*)data->phbk->current.entry[i].name;
#endif
				
				if ( *NumPtr == '\0' )
				{
					NumPtr = (char*)data->phbk->current.entry[i].number;
					/* x0045876, 14-Aug-2006 (WR - "Position" was set but never used) */
					/* Position = POS_END; */
				}
				/* BEGIN: x0045876, 14-Aug-2006 (WR - "Position" was set but never used) */
				/*
				else
					Position = POS_BEGIN;
				*/
				/* END: x0045876, 14-Aug-2006 (WR - "Position" was set but never used) */

				/* Clear the buffer, then fill it with the required value
				*/
				memset( ElemPtr, '\0', ElemSize );
				/*mc, SPR 1442, replaced old truncation function with new one*/ 
				resources_truncate_to_screen_width(NumPtr, 0, ElemPtr, ElemSize,SCREEN_SIZE_X, FALSE);
			}

			/* Show the menu and stick up the find prompt
			*/
			mnuUnhide(data->menu);
			PROMPT( MmiBookMenuArea().px, Mmi_layout_line(SECOND_LAST_LINE_TOP), 0, TxtFind );

			/* The softkeys are dependant on how we got here
			*/
			//CRR: 25302 - xpradipg 10 Nov 2004
			//check also if phonebook is invoked by MMS/EMS
			if ( ( data->phbk->fromSMS ) || ( data->phbk->fromSMSSC )|| (data->phbk->fromMmsEms) ||( data->phbk->current.status.book == PHB_SDN ) )
			{
			    leftKey  = TxtSoftSelect;
			    rightKey = TxtSoftBack;
			}
			else
			{
			    leftKey  = TxtSoftCall;
			    rightKey = TxtSoftOptions;
			}
			displaySoftKeys( leftKey, rightKey );
		}
		break;

		default:
		{
			/* Default handler, just return zero and let the next handler in the
			   chain deal with the event if required
			*/
		    return 0;
		}
    }

	/* return non-zero status indicating we have dealt with the event
	*/
	return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookSDNKbdCB

 $Description:	Keyboard handler
 
 $Returns:		stats indicating if event handled or not
                (Always MFW_EVENT_CONSUMED)

 $Arguments:	e, event, k, keyboard handle
 
*******************************************************************************/

static int bookSDNKbdCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND          win       = mfwParent(mfw_header());
    T_MFW_WIN          *win_data = ((T_MFW_HDR *)win)->data;
    tBookStandard      *data     = (tBookStandard *)win_data->user;
	tBookMessageEvents MyEvent   = SEARCH_STRING;
	char               *Number;

//	int i = 0;  // RAVI

    TRACE_FUNCTION ("bookSDNKbdCB()");

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_entry_EventKey(data->entry_data, e, k);
#else /* NEW_EDITOR */
	/* Make sure we have the correct editor enabled
	* (unnecessary for NEW_EDITOR) */
	if ( activeEditor() != data->edt )
		editActivate( data->edt, TRUE );
	editEventKey( e, k );
#endif /* NEW_EDITOR */

	/* Handle the events we need to deal with here
	*/
	switch (k->code)
    {
        case KCD_MNUUP:
		{
			/* Scroll up event, just mark our event as such and it'll
			   be dealt with in due course
			*/
			MyEvent = SEARCH_SCROLL_UP;
		}
		break;

        case KCD_MNUDOWN:
		{
			/* Scroll down event, just mark our event as such and it'll
			   be dealt with in due course
			*/
			MyEvent = SEARCH_SCROLL_DOWN;
		}
		break;

		case KCD_CALL:
        case KCD_LEFT:
		{
			/* left key is a select, we either need to send the number
			   back to the parent window, or call it
			*/
			Number = (char *) data->phbk->current.entry[ data->phbk->current.selectedName ].number;

			if ( data->phbk->fromSMS )
				SEND_EVENT( data->phbk->parent_win, SMS_PHBK_NUMBER, 0, (UBYTE *) Number );
			if ( data->phbk->fromSMSSC )
				SEND_EVENT( data->phbk->parent_win, SMSSC_PHBK_NUMBER, 0, (UBYTE *) Number );
			else if(data->phbk->fromDivert )
				SEND_EVENT( data->phbk->parent_win, DIVERT_PHBK_NUMBER, 0, (UBYTE *) Number );
			else if(data->phbk->fromDeflect )/*SPR 1392, send event to call dialogue*/
				SEND_EVENT( data->phbk->parent_win, DEFLECT_PHBK_NUMBER, 0, (UBYTE *) Number );
                    //CRR: 25302 - xpradipg 10 Nov 2004
                    //send even if invoked by MMS/EMS
                     else if(data->phbk->fromMmsEms)
                        SEND_EVENT( data->phbk->parent_win, MMS_EMS_PHBK_NUMBER, 0, (UBYTE *) Number );
			else
				callNumber( (UBYTE *) Number );

			/* Having dealt with the number, we destroy our phone book
			   context and exit the handler early
			*/
			bookPhonebookDestroy( data->phbk->win );
			return MFW_EVENT_CONSUMED;
		}
		//break; // RAVI

        case KCD_RIGHT:
		{
			/* Right key is a cancel
			*/
			bookSDNDestroy( data->win );
			data->phbk->search_win = 0;
			return MFW_EVENT_CONSUMED;
		}
//		break;   // RAVI

        case KCD_HUP:
		{
			/* Clear key deletes the last character, or if none left will
			   take us out of here
			*/
            if ( data->edtBuf[0] == '\0' )
			{
				bookSDNDestroy( data->win );
				data->phbk->search_win = 0;
				return MFW_EVENT_CONSUMED;
			}
            else
			{
				/* Delete last character entered
				*/
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 */
#else /* NEW_EDITOR */
                edtChar( data->edt, ecBack );
#endif /* NEW_EDITOR */
			}
		}
        break;

        default:
		{
			/* No default processing required
			*/
		}
        break;
    }

	/* Initiate a new search based on the current settings, MyEvent
	   will have been set accordingly
	*/
    SEND_EVENT( data->win, MyEvent, 0, 0 );
	winShow( data->win );

	/* And make sure no calling routines try to do anything with the
	   event we have just dealt with
	*/
    return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookSDNKeyLongCB

 $Description:	long keyboard event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event to handle, k, keyboard handle
 
*******************************************************************************/

static int bookSDNKeyLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND       win       = mfwParent(mfw_header());
    T_MFW_WIN		*win_data = ((T_MFW_HDR *)win)->data;
    tBookStandard   *data     = (tBookStandard *)win_data->user;

	/* Just checks for clear and long being set, if so it will
	   destroy the window
	*/
	if ( (e & KEY_CLEAR) && (e & KEY_LONG) )
	{
		bookSDNDestroy( win );
		data->phbk->search_win = 0;
	}
	
	/* Force event consumed always, prevents default behaviour
	   kicking in
	*/
	return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	bookSDNCreate

 $Description:	Creates the Service numbers widow
 
 $Returns:		Window Handle, or NULL if unsuccessfull

 $Arguments:	Parent, handle of the parent window
 
*******************************************************************************/

static T_MFW_HND bookSDNCreate( MfwHnd parent )
{
	T_MFW_WIN       *win_data;
	tBookStandard   *data            = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) );
	T_MFW_WIN       *parent_win_data = ((T_MFW_HDR *)parent)->data;
	T_phbk*         phbk             = (T_phbk *)parent_win_data->user;

	TRACE_FUNCTION( "bookSDNCreate()" );

	/* Create the window
	*/
	if ( ( data->win = win_create (parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookSDNWinCB ) ) == NULL )
		return NULL;
	
	/*SPR 2123, MC allocate memory for phonebook list*/
	if (create_mmi_phonebook_names_list() == NULL)
		return NULL;
	
	/* set up the basic window elements, dialog and user data pointers
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC)bookSDN;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
 	win_data->user				= (void *)data;
	data->parent_win			= parent;

	/* Create keyboard and menu handlers to be associated with
	   this window
	*/
	data->kbd      = kbdCreate( data->win, KEY_ALL, (MfwCb) bookSDNKbdCB );
    data->kbd_long = kbdCreate( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookSDNKeyLongCB );
	data->menu     = mnuCreate( data->win, MmiBookMenuDetailsList(), 0, 0 );

	/* Set basic edit attributes structure and create an editor
	   associated with these attributes
	*/
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr( &data->editor_attr, BOOKSDN_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_ALPHA, ED_CURSOR_BAR, ATB_DCS_ASCII, (UBYTE*) data->edtBuf, MAX_SEARCH_CHAR);
	data->editor = ATB_edit_Create(&data->editor_attr,0);
	data->entry_data = AUI_entry_Create(data->win, data->editor, SEARCH_REDRAW);
	ATB_edit_Init(data->editor);
#else /* NEW_EDITOR */
	bookSetEditAttributes( BOOKSDN_EDITOR, COLOUR_EDITOR_XX , 0, edtCurBar1, 0, (char*) data->edtBuf, MAX_SEARCH_CHAR, &data->attr );
	data->edt = edtCreate( data->win, &data->attr, 0, 0 );
#endif /* NEW_EDITOR */

	/* Show the menu
	*/
	mnuUnhide(data->menu);

	/* associate the phonebook handler
	*/
	data->phbk = phbk;

	/* and return the newly created window handle
	*/
    return data->win;
}





/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookSDNStart

 $Description:	This is the entry point for the service numbers window handler
 
 $Returns:		handle of window we are creating

 $Arguments:	Parent, handle of the parent window
 
*******************************************************************************/

T_MFW_HND bookSDNStart( MfwHnd parent )
{
	T_MFW_HND       win;
    T_MFW_WIN       *win_data;
    tBookStandard *data;
	MfwMnu          *mnu;

  	TRACE_FUNCTION( "bookSDNCreate()" );

	/* We can't actually create the window if the phone book is
	   still loading, so we will deal with this up front
	*/
	if ( phb_get_mode() == PHB_LOADING )
	{
		bookShowInformation( idle_get_window(), TxtPleaseWait ,NULL, NULL );
		return NULL;
	}

	/* Create the basic window, dealing with errors here and just terminating
	   if we can't create the window correctly.
	*/
    if ( ( win = bookSDNCreate( parent ) ) == NULL )
		return NULL;

	/* Okay, we have created the window, so set up our internal
	   working pointers and check if the current phone book has
	   some entries in it
	*/
	win_data = ((T_MFW_HDR *) win)->data;
    data = (tBookStandard *)win_data->user;
	data->phbk->current.status.book = PHB_SDN;

	/* we need to determine if there are any entries in the phone book
	   before we allow the service number windows to be displayed, we
	   do this by searching for an empty name
	*/
	bookGetCurrentStatus( &data->phbk->current.status );
	memset( data->edtBuf, '\0' , sizeof( data->edtBuf ) );
	data->phbk->current.index = 1;
	data->phbk->current.selectedName = 0;
	bookFindName( MAX_SEARCH_NAME, &data->phbk->current );

	/* If the current index is still zero then we have been unable to
	   find an entry, in this case we need to shutdown the service
	   numbers window, since we can't do anything with it.
	*/
	if ( data->phbk->current.index == 0 )
	{
		bookSDNDestroy( win );
		data->phbk->search_win = 0;
		bookShowInformation( idle_get_window(),  TxtEmptyList ,NULL, NULL );
		return 0;
	}

	/* We are still running, so set up the menu and edit structures
	   accordingly, and display the window
	*/
	mnu = (MfwMnu *) mfwControl( data->menu );
	mnu->lCursor[ mnu->level ] = data->phbk->current.selectedName;
	
/* SPR#1428 - SH - New Editor: editActivate no longer needed */
#ifndef NEW_EDITOR
	editActivate( data->edt, 1 );
#endif
	winShow( data->win );

	/* return the pointer to the window
	*/
    return win;
}



/*******************************************************************************

 $Function:    	bookFindNameInSDNPhonebook

 $Description:	This routine performs the FindNameInPhoneBook functionality,
				tuned to the particular requirements of the service numbers
				phonebook.
 
 $Returns:		handle of window we are creating

 $Arguments:	Parent, handle of the parent window
 
*******************************************************************************/

int bookFindNameInSDNPhonebook( const char* p_pszNumber, T_MFW_PHB_ENTRY* p_pEntry )
{
	T_MFW_PHB_LIST	phb_list;
#ifdef NO_ASCIIZ
	T_MFW_PHB_TEXT p_pszNumberText;
#endif
	SHORT			phb_index=0;
/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
	UBYTE			l_name[MAX_ALPHA_LEN];
	int status;
char bfr[80];	
	TRACE_FUNCTION("bookFindNameInSDNPhonebook()");

	/* guard against dodgy input data
	*/
	if( ( p_pszNumber == NULL ) || ( p_pEntry == NULL ) )
		return 0;
	if ( strlen(p_pszNumber) < 1)
		return (0);
	/* initialise the search structures
	*/
	memset(p_pEntry, 0, sizeof(T_MFW_PHB_ENTRY));
	memset(&phb_list, 0, sizeof(phb_list));
	phb_list.entry = p_pEntry;
	phb_list.num_entries = 1;
	phb_list.book = PHB_SDN;
	
	//GW Set up data structure for NO_ASCIIZ
#ifdef NO_ASCIIZ
	p_pszNumberText.dcs = MFW_DCS_8bits;
	p_pszNumberText.len = strlen(p_pszNumber);
	strcpy((char*)p_pszNumberText.data, p_pszNumber);
#endif

	/* see what we can find using the standard search routine for
	   any phone book, giving the service numbers book as a parameter
	*/
	
#ifdef NO_ASCIIZ
	status = phb_find_entries( (UBYTE)PHB_SDN,  &phb_index, (UBYTE)MFW_PHB_NUMBER, 1, &p_pszNumberText,	&phb_list );
#else
	status = phb_find_entries( PHB_SDN, &phb_index, MFW_PHB_NUMBER, 1, (char *) p_pszNumber,	&phb_list );
#endif
	sprintf(bfr,"len:%2d , name: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",p_pEntry->name.len);
	memcpy(&bfr[16], p_pEntry->name.data, PHB_MAX_LEN);
	TRACE_FUNCTION(bfr);
	sprintf(bfr,"number:'%s'",p_pEntry->number);
	TRACE_FUNCTION(bfr);
	
	if (status != MFW_PHB_OK)
	{
		phb_list.result = MFW_NO_ENTRY;
		TRACE_FUNCTION("bookFindNameInSDNPhonebook()- status error");
	}

	/* convert the returned data to manageable information
	*/

#ifdef NO_ASCIIZ
	//GW Check that conversion returned a valid string
	//GW Testing for 0 is stupid but since the macros PASSED and FAILED that are used by the procedure are not in the header (duh!) we can't use them.
	/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
	if (mfw_SIM2GsmStr( p_pEntry->name.len, p_pEntry->name.data, MAX_ALPHA_LEN, l_name ) == 0)
		memcpy(p_pEntry->name.data, l_name, MAX_ALPHA_LEN);

	p_pEntry->name.data[0] = 0x00;

#else
	bookGsm2Alpha( p_pEntry->name );
		
#endif

    /* if we couldn't find any information, null out the return
	   structure, this will stop people who don't check the return
	   status from getting sensible information
	*/
	if( phb_list.result != MFW_ENTRY_EXIST )
		memset(p_pEntry, 0, sizeof(T_MFW_PHB_ENTRY));

	/* return status depends on result of search
	*/
	return ( phb_list.result == MFW_ENTRY_EXIST );
}






/*******************************************************************************

 $Function:    	bookSDNDestroy

 $Description:	destroys the Service Numbers window
 
 $Returns:		none.

 $Arguments:	window, handle of the window to be destroyed
 
*******************************************************************************/

void bookSDNDestroy( MfwHnd window )
{
	T_MFW_WIN       *win  = ((T_MFW_HDR *)window)->data;
	tBookStandard   *data = (tBookStandard *)win->user;

	TRACE_FUNCTION( "bookSDNDestroy" );

	/* Guard against bad incoming data
	*/
	if ( ! data )
		return;
		
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	/* Editor is no longer destroyed with winDelete,
	 * so destroy it here. */
	if (data->editor)
	{
		ATB_edit_Destroy(data->editor);
		data->editor = 0;
	}
	if (data->entry_data)
	{
		AUI_entry_Destroy(data->entry_data);
		data->entry_data = 0;
	}
#endif /* NEW_EDITOR */

	/* Destroy the window using the appropriate method
	*/
	if ( data->phbk->root_win == window )
		bookPhonebookDestroy( data->phbk->win );
	else
	{
		/* deleting the window, so make sure we free the memory
		*/
		data->phbk->search_win = 0;
		winDelete( data->win );
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
	}
	/*SPR2123, MC deallocate memory for phonebook list*/
	destroy_mmi_phonebook_names_list(MmiBookMenuDetailsList());
}


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

