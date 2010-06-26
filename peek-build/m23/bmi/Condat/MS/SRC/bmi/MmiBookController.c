/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   PhoneBook
 $File:       MmiBookController.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

  The book controller module provides the external
  interface to the phone book. It provides ALL external
  entry points to the phone book system.

********************************************************************************

 $History: MmiBookController.c

	Nov 10, 2006   DRT: OMAPS000103354  x0039928
	Description: Delete all failure in LDN Phonebook
	Solution: phb_delete_all is called with the proper phone book as input.

	Aug 18, 2006   ER: OMAPS00089840  x0039928
	Description: Changes to be made in MMI for the functionality "Delete all" ADN entries
	Solution: A new menu item "Delete All" is added in the phone book menu.

	July 21,2006 REF:DR OMAPS00085695 x0047075
	Description:Fail to delete the correct entry in LDN, LRN and LMN with Locosto.
	Solution:After deleting any entry in the LDN,LRN,LMN book,we highlight the first entry as the 
	current selected entry.

       Jun 13, 2005	REF: SPR 31705 x0021308
       Description: Once  maximum limit of the My Number is exceeded, display info : Phonebook full, 
       expected is MyNumber Full
       Solution: Replaced the Return tag in function Protected Menu Handler(booksetownNumber) from 
       Phonebookfull with MyNumberfull.

       Jun 13, 2005	REF: SPR 31710 x0021308
 	Description: Once  maximum limit of the FDN Number  is exceeded, display info : Phonebook full, 
       expected is FDNList Full
       Solution: Replaced the Return tag in function Protected Menu Handler(booksetownNumber) from 
       Phonebookfull with FDNListfull While checking whether in FDNMenu is active using MenuFDN global 
       variable.  

        
   	Sep 22, 2004        REF: CRR 21392  xvilliva  
	Bug:		PIN2 Screen is not displaying, While Adding FDN Number thru 
			Recent call Menu.
	Fix:	 	We add the "bookRepRedNameEnter" to "ProtectedActions" array.
			If FDN is enabled, Pin2 will be asked. 

	June 26, 2006	     REF : OMAPS00083148  x0023848
	Bug:		SMS send is failed when trying through missed call list
	Fix:		Assignemnt of SmsData.called_from_reply_item to FALSE is Added
	
  25/10/00      Original Condat(UK) BMI version.



 $End

 // Issue Number : MMI-FIX-12161 on 07/04/04 by Nishikant Kulkarni/Ajith K.P

*******************************************************************************/


/*******************************************************************************

                                Include Files

*******************************************************************************/

/* MMI phone book specific include files
*/
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


#include "MmiBookController.h"
#include "MmiBookUtils.h"

/* MMI specific includes, outside scope of phone book
*/
#include "MmiPins.h"
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "mmiCall.h"
#include "MmiIdle.h"
#include "MmiCPHS.h"


/*******************************************************************************

                                Definitions

*******************************************************************************/

/* Since we want to make sure all of the menu handlers get
   treated the same way, and have a protected handler, define
   a new macro, in terms of the MENU_HANDLER, which will
   invoke the protected function correctly (See below for
   further details)
*/
#define PROTECTED_MENU_HANDLER( Name ) \
static MENU_HANDLER( Protected##Name ); \
MENU_HANDLER( Name ) \
{ \
    return Protected( Name, Protected##Name, Menu, Item ); \
} \
static MENU_HANDLER( Protected##Name )

/*
** Since PROTECTED_MENU_HANDLER handles only FDN protection, the following is a menu handler to ensure
** that PIN2 entry is required for any associated menu action
*/
#define PIN2RQD_MENU_HANDLER( Name ) \
static MENU_HANDLER( Pin2Rqd##Name ); \
MENU_HANDLER( Name ) \
{ \
    return Pin2Rqd( Name, Pin2Rqd##Name, Menu, Item ); \
} \
static MENU_HANDLER( Pin2Rqd##Name )

/*Added For Fix:  MMI-FIX-12161 on 07/04/04 by Nishikant Kulkarni/Ajith K.P */
extern T_idle idle_data;               


/*******************************************************************************

                                Local structures

*******************************************************************************/

/* A number of the actions the phone book is required to perform
   can only be activated when the user enters a security PIN number,
   the PIN2 code. In order to make the operation of the module
   clear, we will build a list of the protected actions, and then
   drive all menu operations through a single clearing process.

   The module will use the following action list perform the
   protection, note that the last entry in this list must be
   NULL
*/
typedef tBookStatus (*tProtectFunction)( tBookMfwMenu *Menu, tBookMfwMenuItem *Item );

tProtectFunction ProtectedActions[] = {
  bookNameEnter,
  bookNameEdit,
  bookNameDelete,
  bookNameDeleteAll,
  // Sep 22, 2004        REF: CRR 21392  xvilliva  
  // A function, if added to this array would ask Pin2 when called.
  // Storing a number from recent call list would ask for Pin2, if FDN is enabled.
  bookRepRedNameEnter, 
  NULL };

//    May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	This flag will indicate when we are in FDN menu
int menuFDN; 
extern T_call call_data;


/*******************************************************************************

                                Private methods

*******************************************************************************/

/*******************************************************************************

 $Function:     Mmir_BaseAddress

 $Description:  returns the base address of the Master Index Table

 $Returns:    As above

 $Arguments:  none.

*******************************************************************************/

/* Provide a local routine which will perform the protection
   method. This will scan the list of protected actions, looking
   for the provided function, if it isn't a protected function
   then we will just call it directly, otherwise we will guard
   it with the PIN2 security code if we have a protected book
*/
static int Protected( tProtectFunction FunTag, tProtectFunction FunPtr, tBookMfwMenu *Menu, tBookMfwMenuItem *Item )
{
    pBookMfwWin    win_data = (pBookMfwWin) bookWindowData();
  pBookPhonebook book     = ((pBookStandard)win_data->user)->phbk;
  int i;

  /* Check for a protected book being used (details to be sorted
     out at a later date)
  */

   // Sep 22, 2004        REF: CRR 21388  xvilliva
   // We check if the call is not from FDN menu and if the current storage
   // for phonebook is internal phone book, we avoid asking Pin2 accordingly.
   if(menuFDN == 0 && bookActiveBook(WRITE) == PHB_IPB)
   {
   	//Do nothing.
   } 
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	Pin2 should be prompted when user does FDN related operations in FDN menu
//   In FDN menu, receie an active call and FDN is disabled, pin2 should not be prompted for in call addressbook operations
  else if ( phb_get_mode() == PHB_RESTRICTED || (menuFDN && (call_data.calls.numCalls==0)))
  {
    /* Is the requested function one of the ones we need
       to protect ?
    */
    TRACE_FUNCTION("Phone Book in FDN mode:ask for pin 2");
    for ( i = 0; ProtectedActions[i]; i++ )
    {
      if ( FunTag == ProtectedActions[i] )
      {
        /* Yup, so we need to store the pending action for
           this window, this will be invoked by the checking
           operation, indirectly, if the check is successful
        */
        book->pin2_next = (T_VOID_FUNC) FunPtr;

        //We're cheating and passing the current window to the
        //handler via the unused menu parameter -- MC
        book->menu = (tBookMfwMenu*) bookCurrentWindow();
        book->item = Item;

        /* Call out to get the pin2 number checked, this will
           respond with a message back to this object, which
           we will deal with on receipt
        */
        return pin2_check( bookCurrentWindow());
      }
    }
  }

  /* we've come through the list of functions and either don't
     have a protected action to perform, or the book itself
     is not protected, so just invoke the requested action
  */
  return (FunPtr)( (tBookMfwMenu*) bookCurrentWindow()/*Menu*/, Item );
}


/*
** This function will apply PIN2 protection to the associated menu functions at ALL times
**
*/
static int Pin2Rqd( tProtectFunction FunTag, tProtectFunction FunPtr, tBookMfwMenu *Menu, tBookMfwMenuItem *Item )
{
	pBookMfwWin    win_data = (pBookMfwWin) bookWindowData();
	pBookPhonebook book     = ((pBookStandard)win_data->user)->phbk;
//	int i;   // RAVI

	/* Yup, so we need to store the pending action for
	this window, this will be invoked by the checking
	operation, indirectly, if the check is successful
	*/
	book->pin2_next = (T_VOID_FUNC) FunPtr;

	//We're cheating and passing the current window to the
	//handler via the unused menu parameter -- MC
	book->menu = (tBookMfwMenu*) bookCurrentWindow();
	book->item = Item;

	/* Call out to get the pin2 number checked, this will
	respond with a message back to this object, which
	we will deal with on receipt
	*/
	return pin2_check( bookCurrentWindow());
}

/*******************************************************************************

                                Public methods

*******************************************************************************/

/* Menu Handlers

   We are routing all of the phone book functionality through
   this module to allow the functional modules to be developed
   independantly where possible.

   This gives a single interface point for the rest of the
   MMI subsystem.

   All of the following functions use the MENU_HANDLER macro,
   as such they all have a common interface, this is not therefore
   documented in each case. The following information is common
   for each menu handler


   $Returns:  Status from worker routine

   $Arguments:  Menu, the menu from which the function was activated
                Item, the item associated with the call

*/


/*******************************************************************************

 $Function:     bookNameEnter

 $Description:

    Add new name to the phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameEnter )
{
  //recast the menu parameter as the current window -- MC
    T_MFW_HND     win         = (T_MFW_HND)Menu;
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

  TRACE_FUNCTION("bookNameEnter");
    /* Clear the input editor structures before we go any further
    */

  if (Phbk->UpdateAction!=ADD_FROM_IDLE)
      memset( Phbk->phbk->edt_buf_number, '\0', PHB_MAX_LEN );
  /*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
  	  memset( Phbk->phbk->edt_buf_name,   '\0', MAX_ALPHA_LEN );
    /* Indicate this is an additional entry being created
    */
    Phbk->UpdateAction = CREATE_ENTRY;

	// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
	//	As the book is being changed for FDN menu operations, we need to 
	//    get back the current book for the normal phonbook operations
    Phbk->current.status.book = bookActiveBook(READ);

    /* Determine if we have room to add any new entries
    */
  bookGetCurrentStatus( &Phbk->phbk->current.status );
  if ( Phbk->phbk->current.status.avail_entries )
  {
        /* We are okay to input the number, use a worker routine
           from the input handler module to actually deal with this
        */
    Phbk->input_number_win = bookInputStartNumberEditor( win, Phbk->edt_buf_number );
  }
  else
  {
        /* We have no room available to add the name, so check if
           we have filled the namespace or not
        */
        
        /*If FDN menu active and no room for entries, then display FDN list full */
	tIndexTagNames Tag ;	
        if(menuFDN)  
        	{
        	 Tag = ( Phbk->phbk->current.status.used_entries )
        	    ? TxtFDNListFull : TxtOperationNotAvail;          //SPR 31710 x0021308:RamG	
        	}
         else
		{
		 Tag = ( Phbk->phbk->current.status.used_entries )
            ? TxtPhbkFull : TxtOperationNotAvail;
	      	} 
        bookShowInformation( win, Tag,NULL, NULL );
  
	
  }

    /* Always return event consumed
    */
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookNameEdit

 $Description:

    Edit the name in the phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameEdit )
{
  //recast the menu parameter as the current window -- MC
    T_MFW_HND     win         = /*mfwParent( mfw_header() )*/(T_MFW_HND)Menu;
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

    /* Indicate this is an edit of a current entry
    */
    Phbk->UpdateAction = MODIFY_EXISTING;
	/*MC SPR 1327, if UPN don't change phonebook type*/
    if (data->phbk->current.status.book != PHB_UPN)
		data->phbk->current.status.book = bookActiveBook(READ);

	/*api - clear buffers before edit
	*/
	memset( Phbk->phbk->edt_buf_number, '\0', PHB_MAX_LEN );
  	memset( Phbk->phbk->edt_buf_name,   '\0', MAX_ALPHA_LEN );

TRACE_EVENT_P2("current phbk is = %d and current entry is = %d", data->phbk->current.status.book,data->phbk->current.entry[ data->phbk->current.selectedName ].book);
	
	if(data->phbk->current.status.book == PHB_ADN_FDN)
	{
		if(data->phbk->current.entry[ data->phbk->current.selectedName ].book==PHB_FDN)
		{
			bookActivateFDN((T_MFW_HND)Menu);
		}	
		else
		{
		    	/* And populate the buffers with the information to be modified
    			*/
				#ifdef NO_ASCIIZ
				/*MC , SPR 1242 copy whole name, merged from b-sample build*/
				  memcpy( Phbk->phbk->edt_buf_name,   Phbk->current.entry[ Phbk->current.selectedName ].name.data, MAX_ALPHA_LEN /*Phbk->current.entry[ Phbk->current.selectedName ].name.len*/);
				#else
				  memcpy( Phbk->phbk->edt_buf_name,   Phbk->current.entry[ Phbk->current.selectedName ].name, MAX_ALPHA_LEN );
				#endif
			    memcpy( Phbk->phbk->edt_buf_number, Phbk->current.entry[ Phbk->current.selectedName ].number, PHB_MAX_LEN );

			    /* And edit the entry we currently have active
			    */
			    Phbk->input_number_win = bookInputStartNumberEditor( win, Phbk->edt_buf_number );
		}
	}
    else
    	{
		    /* And populate the buffers with the information to be modified
    		*/
			#ifdef NO_ASCIIZ
			/*MC , SPR 1242 copy whole name, merged from b-sample build*/
			  memcpy( Phbk->phbk->edt_buf_name,   Phbk->current.entry[ Phbk->current.selectedName ].name.data, MAX_ALPHA_LEN/*Phbk->current.entry[ Phbk->current.selectedName ].name.len*/);
			#else
			  memcpy( Phbk->phbk->edt_buf_name,   Phbk->current.entry[ Phbk->current.selectedName ].name, MAX_ALPHA_LEN );
			#endif
		    memcpy( Phbk->phbk->edt_buf_number, Phbk->current.entry[ Phbk->current.selectedName ].number, PHB_MAX_LEN );
	
		    /* And edit the entry we currently have active
		    */
		   Phbk->input_number_win = bookInputStartNumberEditor( win, Phbk->edt_buf_number );
    	}
    /* Always return event consumed
    */
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookNameDelete

 $Description:

    Delete name in phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameDelete )
{

	//recast the menu parameter as the current window -- MC
	//and pass it on 
	//when ADN_FDN phonebook in use check if current entry is FDN
	//is so then call information screen or allow entry to be deleted. --- AP 23/01/02
	//Issue 1944 MZ  Allow an entry in FDN list to be deleted.
    T_MFW_HND       win         = (T_MFW_HND)Menu;
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data         = (tBookStandard *) win_data->user;
	
	 /*MC SPR 1327, if UPN don't change phonebook type*/
     if (data->phbk->current.status.book != PHB_UPN)
			data->phbk->current.status.book = bookActiveBook(READ);
	if(data->phbk->current.status.book == PHB_ADN_FDN)
		if(data->phbk->current.entry[ data->phbk->current.selectedName ].book==PHB_FDN)
			bookActivateFDN((T_MFW_HND)Menu);
		else	
			bookDeleteEntry((T_MFW_HND)Menu);
	else
		bookDeleteEntry((T_MFW_HND)Menu);	

    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookNameCall

 $Description:

    Call name in phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameCall )
{   T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

  callNumber( Phbk->current.entry[ Phbk->current.selectedName ].number );
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookDeleteAll

 $Description:

    Delete name in phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookDeleteAll )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

// Aug 18, 2006   ER: OMAPS00089840  x0039928   
    bookDeleteBook(Phbk->win);

    return MFW_EVENT_CONSUMED;
}

// Nov 10, 2006   DRT: OMAPS000103354  x0039928
// Fix: phb_delete_book() is called with proper book information  instead of bookDeleteBook().
/*******************************************************************************

 $Function:     bookDeleteRec

 $Description:

    Delete name in phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookDeleteRec )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

  if (phb_delete_book(Phbk->current.status.book)==MFW_PHB_OK)
  {
      MmiBookSetCurrentText(  TxtEntryDeleted);
      bookShowInformation( Phbk->win, MmiBookCurrentText(),NULL, NULL );
      bookPhonebookDestroy(Phbk->win);
  }
  else
  {
         MmiBookSetCurrentText(TxtFailed);
         bookShowInformation( Phbk->win, MmiBookCurrentText(),NULL, NULL );
  	  bookPhonebookDestroy(Phbk->win);
  }

    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookNameSearch

 $Description:

    Search for name in phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameSearch )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;

    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
  data->phbk->current.status.book = bookActiveBook(READ);
  data->phbk->search_win = bookSearchStart( data->phbk->win );

    return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:     bookNumberSearch

 $Description:

    Search for name in phone book, order the elements by the phone book number

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNumberSearch )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;

    data->phbk->current.KindOfSearch = SEARCH_BY_NUMBER;
  data->phbk->current.status.book = bookActiveBook(READ);
  data->phbk->search_win = bookSearchStart( data->phbk->win );

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookLocationSearch

 $Description:

    Search for number in phone book, using SIM location to order search
    window output

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookLocationSearch )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;

    data->phbk->current.KindOfSearch = SEARCH_BY_LOCATION;
  data->phbk->current.status.book = bookActiveBook(READ);
  data->phbk->search_win = bookSearchStart( data->phbk->win );

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookNameDeleteAll

 $Description:

    Delete all names associated with a phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookNameDeleteAll )
{
return NULL;
}




/*******************************************************************************

 $Function:     bookNameMessage

 $Description:

    Send message to the number selected

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookSendMessage )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;
    T_SmsUserData   SmsData;

    /* Copy the number and name information to the SMS buffer
    */
  /* API - 13-01-03 - 1409 - Minus the Phbk->current.missedCallsOffset to point at the correct number in the missed calls */ 
  memcpy( SmsData.NumberBuffer, (UBYTE *) Phbk->current.entry[ Phbk->current.selectedName - Phbk->current.missedCallsOffset ].number,
        sizeof( SmsData.NumberBuffer ) );
    /*MC SPR 1257, zero whole buffer for unicode*/
    memset(SmsData.TextBuffer, '\0', MAX_MSG_LEN_ARRAY);
   /* OMAPS00083148 - Assignement called_from_reply_item to FALSE is added*/
    SmsData.called_from_reply_item = FALSE;

    /* Create a dynamic menu window to perform the SMS sending
    */
  SmsSend_SEND_start( win, (void *) &SmsData );

  return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:     bookGetOwnNumber

 $Description:

    Get own number

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookGetOwnNumber )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;


    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
  data->phbk->current.status.book = PHB_UPN;
  data->phbk->search_win = bookSearchStart( data->phbk->win );

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookSetOwnNumber

 $Description:

    Set own number

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookSetOwnNumber )
{  T_MFW_HND      win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

    /* Clear the input editor structures before we go any further
    */
    /*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
  memset( Phbk->phbk->edt_buf_name,   '\0', MAX_ALPHA_LEN );
    memset( Phbk->phbk->edt_buf_number, '\0', PHB_MAX_LEN );

    /* Indicate this is an additional entry being created
    */
    Phbk->UpdateAction = CREATE_ENTRY;
  data->phbk->current.status.book = PHB_UPN;
    /* Determine if we have room to add any new entries
    */
  bookGetCurrentStatus( &Phbk->phbk->current.status );
  if ( Phbk->phbk->current.status.avail_entries )
  {
        /* We are okay to input the number, use a worker routine
           from the input handler module to actually deal with this
        */
    Phbk->input_number_win = bookInputStartNumberEditor( win, Phbk->edt_buf_number );
  }
else
  {
        /* We have no room available to add the name, so check if
           we have filled the namespace or not
        */
        tIndexTagNames Tag = ( Phbk->phbk->current.status.used_entries )
            ? TxtMyNumberFull : TxtOperationNotAvail;    //SPR 31705 x0021308:RamG.
        bookShowInformation( win, Tag, NULL, NULL );
  }

    /* Always return event consumed
    */
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookRepRedNameEnter

 $Description:

    Enter name associated with Reply-Redial number

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookRepRedNameEnter )
{
    // Sep 22, 2004        REF: CRR 21392  xvilliva  
    // We use the variable "Menu", similar to bookNameEnter() to avoid a crash. 
    T_MFW_HND     win         = (T_MFW_HND)Menu;
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;
    char*     selected_number;

  selected_number = (char*)data->phbk->current.entry[data->phbk->current.selectedName].number;
	// Sep 22, 2004        REF: CRR 21392  xvilliva  
	// We get the exact book that is selected.
	data->phbk->current.status.book  = bookActiveBook(WRITE);
	
  Phbk->UpdateAction = CREATE_ENTRY;
  bookGetCurrentStatus( &Phbk->phbk->current.status );
  if ( Phbk->phbk->current.status.avail_entries )
  {
        /* We are okay to input the number, use a worker routine
           from the input handler module to actually deal with this
        */

    bookAddName(win,selected_number);
  }
  else
  {
        /* We have no room available to add the name, so check if
           we have filled the namespace or not
        */
        tIndexTagNames Tag = ( Phbk->phbk->current.status.used_entries )
            ? TxtPhbkFull : TxtOperationNotAvail;
        bookShowInformation( win, Tag, NULL, NULL );
  }
/*   free( selected_number);   x0039928 - Lint warning removal */
  return 1;
}


/*******************************************************************************

 $Function:     bookRepRedDelete

 $Description:

    Delete name associated with Reply-Redial Number

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookRepRedDelete )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

	

  deleteFromFile(Phbk,Phbk->current.status.book);
  bookShowInformation( Phbk->win, MmiBookCurrentText(),NULL, NULL );
  bookMenuDestroy(Phbk->menu_call_options_win);
// July 21, 2006    REF:DR OMAPS00085695  x0047075
    // Fix : Always sending the CALLS_LIST_INIT event whenever we delete an entry from LDN ,LRN,LMN phonebook.
//  if(Phbk->current.status.used_entries > 0)
//	  SEND_EVENT(Phbk->calls_list_win, SEARCH_SCROLL_UP, 0, 0 );
  SEND_EVENT(Phbk->calls_list_win, CALLS_LIST_INIT, 0, (void*)Phbk->current.status.book );
  return 1;
}


/*******************************************************************************

 $Function:     bookCallListDetails

 $Description:

    Retrieve details for call list

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookCallListDetails )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

  bookCallDetailsStart( Phbk->win);
  return 1;

}



/*******************************************************************************

 $Function:     bookServiceNumbers

 $Description:

    Get service numbers

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookServiceNumbers )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;

    data->phbk->current.KindOfSearch = SEARCH_BY_NAME;
  data->phbk->current.status.book = PHB_SDN;
  data->phbk->search_win = bookSearchStart( data->phbk->win );

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookMemoryStatus

 $Description:

    Show the status of the phone book memory.

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookMemoryStatus )
{
    /* This performs the memory view functions, and returns MFW_EVENT_CONSUMED
       in all cases.
    */
  bookMemoryView();
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookFDNActivate

 $Description:

   The FDN Activate routine is simply a wrapper for the real
   FDN Activate handler which resides in mmiPins.c

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookFDNActivate )
{
  return fdnActivate( Menu, Item );
}


/*******************************************************************************

 $Function:     bookFDNDeactivate

 $Description:

   The FDN Deactivate routine is simply a wrapper for the real
   FDN Deactivate handler which resides in mmiPins.c

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookFDNDeactivate )
{
  return fdnDeactivate( Menu, Item );
}

/*******************************************************************************

 $Function:     bookEnterLDN

 $Description:

    Enters de LDN List

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookEnterLDN )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );

  bookPhonebookStart(win,PhbkRedialListNormal);
    return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:     bookEnterLRN

 $Description:

    Enters de LRN List

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookEnterLRN )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );

  bookPhonebookStart(win,PhbkReplyListNormal);
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookEnterLMN

 $Description:

    Enters de LMN List

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookEnterLMN )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );

  /* Issue Number : MMI-FIX-1216 on 07/04/04 by Nishikant Kulkarni and Ajith K.P
  Bug : missed call message is displayed after deleting all missed calls from the list
  Solution:  idle_data.missedcalls flag was not updated if the user ckecks/deletes the missed call list
              We rectified the same.*/

  idle_data.missedCalls = 0;	

  bookPhonebookStart(win,PhbkMissedCallsListNormal);
    return MFW_EVENT_CONSUMED;
}

/* SPR#1112 - Internal phonebook menu options*/
#ifdef INT_PHONEBOOK

/*******************************************************************************

 $Function:     bookSelectBook

 $Description:  Open the select book menu, preselecting the appropriate option

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookSelectBook )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
	T_MFW_WIN     *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;
	T_MFW_HND		menu_win;
	UBYTE			selectedBook;
	
	TRACE_FUNCTION("bookSelectBook");

	if (bookGetBookAvailable())
	{
		selectedBook = (UBYTE)bookGetBookSelected();
		menu_win = bookMenuStart(Phbk->win, SelectBookAttributes(), 0);
		SEND_EVENT(menu_win, DEFAULT_OPTION, NULL, (void *)&selectedBook); // Add our own callback
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable, NULL, NULL);
	}
	
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookSelectBookSIM

 $Description:  Select the SIM phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookSelectBookSIM )
{
    T_MFW_HND     win         = mfwParent( mfw_header() );
	T_MFW_WIN     *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

	if (bookGetBookAvailable())
	{
		bookSetBookSelected(FALSE);
		bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, (T_VOID_FUNC)bookChangePB, (USHORT)bookGetBookSelected(), 100);
	}
	else
		bookShowInfoScreen( Phbk->win, TxtPhonebook, TxtChanged, NULL, NULL, 3000);
	
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     bookSelectBookSIM

 $Description:  Select the SIM phone book

 $Returns:    Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookSelectBookPhone )
{
    T_MFW_HND		win			= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ( (T_MFW_HDR *) win )->data;
    tBookStandard	*data		= (tBookStandard *) win_data->user;
    T_phbk          *Phbk		= data->phbk;

	if (bookGetBookAvailable())
	{
		bookSetBookSelected(TRUE);
		bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, (T_VOID_FUNC)bookChangePB, (USHORT)bookGetBookSelected(), 100);
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable,NULL, NULL );
	}
	
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookCopyEntry

 $Description:  Copy a single entry from/to internal memory

 $Returns:     Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookCopyEntry )
{
	T_MFW_HND			win         = mfwParent( mfw_header() );
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard		*data       = (tBookStandard *) win_data->user;
    T_phbk				*Phbk       = data->phbk;
	
  	TRACE_FUNCTION( "bookCopyEntry()" );

	if (bookGetBookAvailable())
	{
	    /* Show Please Wait dialog briefly, then call copy function
	    */
	    bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, (T_VOID_FUNC)bookCopySingle, (USHORT)FALSE, 100);
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable, NULL, NULL);
	}
	
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookCopyAll

 $Description:  Copy all entries from/to internal memory

 $Returns:     Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookCopyAll )
{
	T_MFW_HND			win         = mfwParent( mfw_header() );
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard		*data       = (tBookStandard *) win_data->user;
    T_phbk				*Phbk       = data->phbk;
	
  	TRACE_FUNCTION( "bookCopyAll()" );

	if (bookGetBookAvailable())
	{
	    /* Copy
	    */
	    bookCopyMultiple(Phbk->win, FALSE, 0);
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable, NULL, NULL);
	}
	
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookMoveEntry

 $Description:  Move a single entry from/to internal memory

 $Returns:     Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookMoveEntry )
{
  	T_MFW_HND			win         = mfwParent( mfw_header() );
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard		*data       = (tBookStandard *) win_data->user;
    T_phbk				*Phbk       = data->phbk;
	
  	TRACE_FUNCTION( "bookMoveEntry()" );

	if (bookGetBookAvailable())
	{
	    /* Show Please Wait dialog briefly, then call move function
	    */
	    bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, (T_VOID_FUNC)bookCopySingle, (USHORT)TRUE, 100);
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable, NULL, NULL);
	}
	
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     bookMoveAll

 $Description:  Move all entries from/to internal memory

 $Returns:     Refer Menu Handlers Definition Block Above

 $Arguments:  Refer Menu Handlers Definition Block Above

*******************************************************************************/

PROTECTED_MENU_HANDLER( bookMoveAll )
{
	T_MFW_HND			win         = mfwParent( mfw_header() );
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard		*data       = (tBookStandard *) win_data->user;
    T_phbk				*Phbk       = data->phbk;
	
  	TRACE_FUNCTION( "bookMoveAll()" );

	if (bookGetBookAvailable())
	{
	    /* Show Please Wait dialog briefly, then call copy function
	    */
	    bookCopyMultiple(Phbk->win, TRUE, 0);
	}
	else
	{
		bookShowInformation( Phbk->win, TxtNotAvailable, NULL, NULL);
	}
	
    return MFW_EVENT_CONSUMED;
}

#endif

PIN2RQD_MENU_HANDLER( CphsAlsLock )
{
	return cphs_lock_als( Menu, Item );
}

PIN2RQD_MENU_HANDLER( CphsAlsUnlock )
{
	return cphs_unlock_als( Menu, Item );
}
/*******************************************************************************

                                End of File

*******************************************************************************/



