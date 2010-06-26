/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookCallListWindow.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the call list window handling for the phone book.
                        
********************************************************************************
 $History: MmiBookCallListWindow.c
       Feb 22, 2007 OMAPS00115777 a0393213(R.Prabakar)
       Description : Details of any Missed call number shows Invalid information
       Fix            : Data was not initialized properly. Now it's initialized properly and 
       the fix done as part of CRR 22638 reverted back as it isn't clean
       
       July 21,2006 REF:DR OMAPS00085695 x0047075
	Description:Fail to delete the correct entry in LDN, LRN and LMN with Locosto.
	Solution:After deleting any entry in the LDN,LRN,LMN book,we highlight the first entry as the 
	current selected entry.

    Apr 05, 2006 REF: OMAPS00073906 a0393213 (Prabakar R)
    Bug : Phone will crash when check the Missed calls list and press the direction key
    Fix : unnecessary function calls are removed
    
       Aug 24, 2004        REF: CRR  22638 xrashmic
	Bug:  E-Sample switches off when you attempt to look at details of a number 
		  in call list. 
	Fix:   Storing the data in a global variable to be used later.


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
#include "MmiTimers.h"

#include "MmiBookShared.h"



/*******************************************************************************
                                                                              
                      Local Data Structure Definitions
                                                                              
*******************************************************************************/

#define NO_FAILURE (-1)
int 			idCounter = 0;
static char		missedCalls[40];
char			calls[10];


/*******************************************************************************
                                                                              
                               Private Methods
                                                                              
*******************************************************************************/


/*******************************************************************************

 $Function:    	bookCallListSetupEntries

 $Description:	Populates the call list display with the current values
 
 $Returns:		None

 $Arguments:	Current, pointer to the entry in question
 
*******************************************************************************/

static void bookCallListSetupEntries( tMmiPhbData *Current )
{
    T_MFW_PHB_ENTRY	p_pEntry;
	int				index;

	/* search the phone book(s)
	*/
	for ( index = 0; index < Current->list.num_entries; index++ )
	{
		//GW Copy name text (for NO_ASCII set/clear) and length
		//GW but only if there is an entry!
		if ( bookFindNameInPhonebook( (const char *) Current->entry[ index ].number, &p_pEntry ) )
			bookCopyPhbName(&Current->entry[ index ], &p_pEntry, PHONEBOOK_ITEM_LENGTH/*SPR2123*/);
	}
}




/*******************************************************************************

 $Function:    	bookCallListDialog

 $Description:	Dialog functino for the phone book call list window
 
 $Returns:		None

 $Arguments:	win, window handle
                event, event to be handled
				value, not used
				parameter, not used
 
*******************************************************************************/

static void bookCallListDialog( T_MFW_HND win, USHORT event, SHORT value, void *parameter )
{
    T_MFW_WIN		*win_data = ((T_MFW_HDR *) win)->data;
    tBookStandard	*data = (tBookStandard *)win_data->user;
	tMmiPhbData		*Current    = &data->phbk->current;
	MfwMnu *mnu;
	
	int Failure;
    TRACE_FUNCTION ("bookCallListDialog()");
    TRACE_EVENT ("jgg-bookCallListDialog()");

	/* Handle the dialog events
	*/
    switch (event)
	{
		case CALLS_LIST_INIT:
		{
			/* Initialise message
			*/			
              //July 21,2006 REF:DR OMAPS00085695 x0047075
             //Fix: After deleting an entry in the phonebook,we make First entry as the current selected entry .
			Current->selectedName = 0;
			data->phbk->current.index		= 1;
			data->phbk->current.status.book = (int)parameter;

			/* If we are currently loading the phone book we can't go any
			   further, so indicate a failure condition
			*/
			Failure = ( phb_get_mode() == PHB_LOADING ) ? TxtPleaseWait : NO_FAILURE;

			/* grab the current status of the phone book, and
			   check that it's okay
			*/
			if ( Failure == NO_FAILURE )
			{
				bookGetCurrentStatus( &data->phbk->current.status );

				/* Issue 2538, MZ 25/09/03 Only read the number of used records from the Service Table
				    when initialising the calls list. */
	
				Failure = ( bookFindName(data->phbk->current.status.used_entries, &data->phbk->current ) != MFW_PHB_OK )
					? TxtReadError : NO_FAILURE;
			}

			/* Check for an empty list
			*/
			if ( Failure == NO_FAILURE )
				Failure = ( data->phbk->current.index == 0 ) ? TxtEmptyList : NO_FAILURE;

			/* api patch 03-04-02
			*/
			if ((Failure != NO_FAILURE) &&  (idCounter > 0))
				Failure = NO_FAILURE;
			
			/* If everything is okay we can try to find the name in the
			   phone book, otherwise Failure will contain an error message
			*/
			if ( Failure == NO_FAILURE )
			{
				bookCallListSetupEntries( &data->phbk->current );
                       //July 21,2006 REF:DR OMAPS00085695 x0047075
                      // Fix: After deleting an entry in the phonebook,we make First entry as the Current selected entry.
			     mnu = (MfwMnu *) mfwControl( data->menu );
			     mnu->lCursor[mnu->level] = Current->selectedName;
				winShow( data->win );
			}
			else
			{
				/* If we get to here and the failure flag isn't NO_FAILURE then
				   it'll be set to the id of the message we need to present, so
				   show the information dialog and destroy the call list window
				*/
				bookShowInformation( data->phbk->win, Failure, NULL, NULL );
				bookCallListDestroy( data->win );
			}
		}
		break;
		case SEARCH_SCROLL_UP:
		{
			/* Scroll up
			*/

			if(( Current->selectedName == 1) && (Current->status.used_entries == 1))
				Current->selectedName = Current->selectedName;
							
			else if ( ( Current->index == 1 ) && ( Current->selectedName == Current->missedCallsOffset ) )
			{
				if ( (Current->status.used_entries + Current->missedCallsOffset) < MAX_SEARCH_CALL_LIST )
					Current->selectedName = Current->status.used_entries -1;
				else
					Current->selectedName =  MAX_SEARCH_CALL_LIST - 1;
				Current->index = Current->status.used_entries + Current->missedCallsOffset - Current->selectedName;
			}	
			else
			{
				if(Current->selectedName == Current->missedCallsOffset)
				{
					if(Current->index == 1 + Current->missedCallsOffset)
						Current->selectedName = Current->missedCallsOffset;
					Current->index--;
				}
				else
					Current->selectedName--;
			}
			mnu = (MfwMnu *) mfwControl( data->menu );
			mnu->lCursor[mnu->level] = Current->selectedName;

			//Apr 05, 2006 REF: OMAPS00073906 a0393213 (Prabakar R)
            //Bug : Phone will crash when check the Missed calls list and press the direction key
            //Fix : unnecessary function calls(bookFindName and bookCallListSetupEntries) are removed
		}
		break;

		case SEARCH_SCROLL_DOWN:
		{
			/* Scroll Down
			*/		

			if ( ( Current->index + Current->selectedName ) == Current->status.used_entries + Current->missedCallsOffset)
			{
				Current->index = 1;
				Current->selectedName = 0 + Current->missedCallsOffset;
			}
			else
			{
				if ( Current->selectedName == MAX_SEARCH_CALL_LIST - 1 )
					Current->index++;
				else
					Current->selectedName++;
			}
			mnu = (MfwMnu *) mfwControl( data->menu );
			mnu->lCursor[mnu->level] = Current->selectedName;

            //Apr 05, 2006 REF: OMAPS00073906 a0393213 (Prabakar R)
            //Bug : Phone will crash when check the Missed calls list and press the direction key
            //Fix : unnecessary function calls(bookFindName and bookCallListSetupEntries) are removed
		}
		break;

		default:
		{

			/* No need to deal with any other events
			*/
		}
		break;
	}
}


/*******************************************************************************

 $Function:    	bookCallListWinCB

 $Description:	Window Event Handler
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise MFW_EVENT_PASSED

 $Arguments:	standard window event handler, e, event, w, window handle
 
*******************************************************************************/

static int bookCallListWinCB( MfwEvt e, MfwWin *w )
{
	tBookStandard	*data = (tBookStandard *)w->user;
	tMmiPhbData		*Current    = &data->phbk->current;
	char			*ptr;
//	MfwMnu *mnu;	    // RAVI
	int				index, i;
//	int				Withheld;   // RAVI

       /* x0045876, 14-Aug-2006 (WR - "missedOutput" was set but never used) */
	/*int				displayStart, missedOutput; */
       int				displayStart;


	TRACE_FUNCTION("bookCallListWinCB");

	/* x0045876, 14-Aug-2006 (WR - "missedOutput" was set but never used) */
	/* missedOutput = FALSE; */

	/* api introduction to display withheld numbers in recent calls list
	   03-04-02
	*/
	switch( e )
    {
        case MfwWinVisible:
		{	
			/* Handle the window visible event, show default screen,
			   populate the call list menu and put up default soft keys
			*/
		      TRACE_EVENT_P1("<<<<idCounter Value is %d", idCounter);	
		    MmiBookShowDefault();

			if(idCounter > 0)
			{
				/* x0045876, 14-Aug-2006 (WR - "missedOutput" was set but never used) */
				/* missedOutput = TRUE; */
				
				displayStart = 1;
				strcpy(missedCalls,MmiRsrcGetText(TxtUnknown));
				strcat(missedCalls,": ");
				sprintf(calls,"%d",idCounter);
				strcat(missedCalls,calls);
				data->phbk->current.missedCallsOffset = 1;
				strcpy( (char*)MmiBookCallList( 0 ), (char*)missedCalls);
			}
			else
				displayStart = 0;
			
			index = 0;
			
			for ( i = displayStart; i < NAME_LIST_SIZE; i++ )
			{
#ifdef NO_ASCIIZ
				if ( data->phbk->current.entry[index].name.len > 0 )
					ptr = (char *) data->phbk->current.entry[index].name.data;
				else if(strlen((char *) data->phbk->current.entry[index].number) > 0)
					ptr = (char *) data->phbk->current.entry[index].number;
				else
					ptr = "";

#else
				if ( strlen( (char *) data->phbk->current.entry[index].name ) > 0 )
					ptr = (char *) data->phbk->current.entry[index].name;
				else if (strlen((char *) data->phbk->current.entry[index].number) > 0)
					ptr = (char *) data->phbk->current.entry[index].number;
				else
					ptr = "";
#endif				

					strcpy( (char*)MmiBookCallList( i ), ptr);
					TRACE_FUNCTION((char*)MmiBookCallList( i ));
					index ++;
					strcpy( (char*)MmiBookCallList( index ), (char*)ptr);
					TRACE_EVENT((char*)MmiBookCallList( index ));
			}

			mnuUnhide(data->menu);
                     
			
			if((idCounter > 0) && (Current->status.used_entries == 0))
				displaySoftKeys( TxtNull, TxtSoftBack );
			else if((idCounter > 0) && (Current->status.used_entries > 0))
				displaySoftKeys( TxtSoftOptions, TxtSoftBack );
			else
				displaySoftKeys( TxtSoftOptions, TxtSoftBack );
		}
		break;

        default:
		{
			/* If it's not a window visible event we ignore it and
			   pass it back up the tree to be handled
			*/
			return MFW_EVENT_PASSED;
		}
//		break;   // RAVI
    }

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	bookCallListKbdCB

 $Description:	Keyboard event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, keyboard handle
 
*******************************************************************************/

static int bookCallListKbdCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND		win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;
    tMmiPhbData		*Current    = &data->phbk->current;


	/* Handle the events provided by the key
	*/
    switch (k->code)
    {
        case KCD_MNUUP:
        {
	        SEND_EVENT( data->win, SEARCH_SCROLL_UP, 0, 0 );
            winShow(win);
		}
        break;

        case KCD_MNUDOWN:
		{
	        SEND_EVENT( data->win, SEARCH_SCROLL_DOWN, 0, 0 );
            winShow(win);
		}
        break;
        case KCD_RIGHT:
			bookPhonebookDestroy( data->phbk->win );
		break;
		case KCD_CALL:
		{
			/* selected to call the number
			*/
			mnuHide( data->menu );
			callNumber( data->phbk->current.entry[data->phbk->current.selectedName - data->phbk->current.missedCallsOffset].number );
			bookPhonebookDestroy( data->phbk->win );
		}
        break;

		case KCD_MNUSELECT:
        case KCD_LEFT:
		{
			/* Selected the options menu so display them
			*/
			if(Current->status.used_entries > 0)
				data->phbk->menu_call_options_win = bookMenuStart( data->phbk->win, bookRepRedOptionsMenuAttributes(),0 );
			else
				break;
		}
        break;

        case KCD_HUP:
		{
			/* Selected to get out of here, so destroy the window
			*/
			bookCallListDestroy( data->win );
		}
		break;

		default:
		{
			/* No other handling required here
			*/
		}
		break;
    }

	/* Always consume the event
	*/
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	bookCallListKbdLongCB

 $Description:	Keyboard Long Press event handler
 
 $Returns:		MFW_EVENT_CONSUMED always

 $Arguments:	e, event, k, keyboard handle
 
*******************************************************************************/

static int bookCallListKbdLongCB( MfwEvt e, MfwKbd *k )
{
    T_MFW_HND		win			= mfwParent( mfw_header() );
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *)win)->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;

	/* hand a long clear event only
	*/
	if ( ( e & KEY_CLEAR ) && ( e & KEY_LONG ) )
		bookCallListDestroy( data->win );

	return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:		bookCallListCreate

 $Description:	Create the call list window
 
 $Returns:		Handle of the window, or NULL if there is an error

 $Arguments:	parent, handle of the parent window
 
*******************************************************************************/

static T_MFW_HND bookCallListCreate( MfwHnd parent )
{
	T_MFW_WIN		*parent_win_data = ( (T_MFW_HDR *) parent )->data;
	T_phbk			*phbk = (T_phbk *)parent_win_data->user;	
	T_MFW_WIN		*win_data;
	tBookStandard	*data;	
	MfwMnu *mnu;

	TRACE_FUNCTION ("bookCallListCreate()");

	/* allocate memory for our control block
	*/
	if ( ( data = (tBookStandard *) ALLOC_MEMORY( sizeof( tBookStandard ) ) ) == NULL )
		return NULL;

	/* Create the window if we can
	*/
	if ( ( data->win = win_create( parent, 0, E_WIN_VISIBLE, (T_MFW_CB) bookCallListWinCB ) ) == NULL )
	{
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
		return NULL;
	}
	/*SPR 2123, MC allocate memory for phonebook list*/
	if (create_mmi_phonebook_names_list() == NULL)
		return NULL;
	/* Okay, we have created the control block and the window, so
	   we now need to configure the dialog and data pointers
	*/
    data->mmi_control.dialog	= (T_DIALOG_FUNC) bookCallListDialog;
    data->mmi_control.data		= data;
    win_data					= ((T_MFW_HDR *)data->win)->data;
 	win_data->user				= (void *) data;
	data->phbk					= phbk;
	data->phbk->root_win        = data->win;
	data->phbk->calls_list_win  = data->win;
	data->parent_win			= parent;

	/* create keyboards and menus for our window
	*/
	data->kbd		= kbdCreate( data->win, KEY_ALL,            (MfwCb) bookCallListKbdCB );
    data->kbd_long	= kbdCreate( data->win, KEY_ALL | KEY_LONG, (MfwCb) bookCallListKbdLongCB );
	data->menu		= mnuCreate( data->win, (MfwMnuAttr *)MmiBookMenuDetailsList(), 0, 0 );

	mnu = (MfwMnu *) mfwControl( data->menu );
	
	/* api 19-04-02 - path to move cursor down one 
	in missed calls list. */

	if(idCounter > 0)
		mnu->lCursor[mnu->level] = 1;
	else
		mnu->lCursor[mnu->level] = 0;
	
	mnuScrollMode(data->menu,0);

	/* allow the window and menu to be created
	*/
	mnuUnhide(data->menu);
	winShow(data->win);

	/* And return the handle of the newly created window
	*/
    return data->win;
}


/*******************************************************************************
                                                                              
                               Public Methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookCallListStart

 $Description:	Entry point for the call list window
 
 $Returns:		Handle of window

 $Arguments:	parent, handle of parent window
 
*******************************************************************************/

T_MFW_HND bookCallListStart( MfwHnd parent,int parameter )
{
	T_MFW_HND win;

	/* create and initialise the window
	*/
    if ( ( win = bookCallListCreate( parent ) ) != NULL )
		SEND_EVENT( win, CALLS_LIST_INIT, 0, (void*)parameter );

	/* return the handle, or NULL if we have had an error
	*/
    return win;
}


/*******************************************************************************

 $Function:    	bookCallListDestroy

 $Description:	destroys the call list window
 
 $Returns:		None

 $Arguments:	window, the window to be killed
 
*******************************************************************************/

void bookCallListDestroy( MfwHnd window )
{
	T_MFW_WIN		*win  = ((T_MFW_HDR *)window)->data;
	tBookStandard	*data = (tBookStandard *)win->user;

	TRACE_FUNCTION ("bookCallListDestroy()");
	/* Remove the Withheld counter from the list
	*/
	idCounter = 0;
	memset (missedCalls, '\0', 40);
	memset(calls, '\0', 10);
	
	/* Only actually do something if the data pointer is valid
	*/
	if ( ! data )
		return;


	/* If we are the root window then we need to deal with the
	   phonebook destruction
	*/
	if (  data->phbk->root_win == window )
		bookPhonebookDestroy(data->phbk->win);
	else
	{
		/* delete our window and free the allocated memory
		*/
		data->phbk->calls_list_win = 0;
		winDelete ( data->win );
		FREE_MEMORY( (void *)data, sizeof( tBookStandard ) );
	}
	/*SPR2123, MC deallocate memory for phonebook list*/
	destroy_mmi_phonebook_names_list(MmiBookMenuDetailsList());
}



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

