#ifndef _MMIBOOKCONTROLLER_H_
#define _MMIBOOKCONTROLLER_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookController.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
	The book controller module provides the external
	interface to the phone book. It provides ALL external
	entry points to the phone book system.
                        
********************************************************************************
 $History: MmiBookController.h

	25/10/00			Original Condat(UK) BMI version.	
	   
	Nov 10, 2006   DRT: OMAPS000103354  x0039928
	Description: Delete all failure in LDN Phonebook
	Solution: A new function phbDeleteRec is added
	   
 $End

*******************************************************************************/


/* MMI Phone Book Controller Module

   This module provides the entry points to the phone book
   functionality. The real work of the phonebook is carried
   out in subordinate modules

   Each of the subordinate modules provides a mechanism for
   driving the functionality required by it's own module, but
   the external view outside the phonebook itself will come
   from here.
*/



/* In addition to the primary entry points for the phonebook,
   which are identified below, there are additional entry
   points provided as utilities by the MmiBookUtilities
   module. In order to ensure that these are correctly
   enabled by users of the phonebook, we will include their
   definitions here
*/
#include "MmiResources.h"
#include "MmiBookUtils.h"



/* Define the primary access points which exist in the MMI
   Menu tree, we will have a simple one to one mapping here
   between the menu items in the menu tree and the handler
   functions in the phone book.

   Menu handlers have a standard interface, the only thing
   that changes is the name, so wrap this up cleanly
*/
#define MENU_HANDLER( Name ) tBookStatus Name( tBookMfwMenu *Menu, tBookMfwMenuItem *Item )

MENU_HANDLER( bookNameEnter );
MENU_HANDLER( bookNameEdit );
MENU_HANDLER( bookNameDelete );
MENU_HANDLER( bookNameCall );
MENU_HANDLER( bookDeleteAll );
MENU_HANDLER( bookNameSearch );
MENU_HANDLER( bookNumberSearch );
MENU_HANDLER( bookLocationSearch );
MENU_HANDLER( bookDeleteRec );                     // Nov 10, 2006   DRT: OMAPS000103354  x0039928
MENU_HANDLER( bookNameDeleteAll );

MENU_HANDLER( bookSendAsDTMF );
MENU_HANDLER( bookSendMessage );

MENU_HANDLER( bookGetOwnNumber );
MENU_HANDLER( bookSetOwnNumber );

MENU_HANDLER( bookRepRedNameEnter );
MENU_HANDLER( bookRepRedDelete );

MENU_HANDLER( bookCallListDetails );
MENU_HANDLER( bookDetailsList );

MENU_HANDLER( bookServiceNumbers );
MENU_HANDLER( bookMemoryStatus );
MENU_HANDLER( bookFDNActivate );
MENU_HANDLER( bookFDNDeactivate );

MENU_HANDLER( bookEnterLDN );
MENU_HANDLER( bookEnterLRN) ;
MENU_HANDLER( bookEnterLMN );

/* SPR#1112 - Internal phonebook menu options */
#ifdef INT_PHONEBOOK
MENU_HANDLER( bookSelectBook );
MENU_HANDLER( bookSelectBookSIM );
MENU_HANDLER( bookSelectBookPhone );
MENU_HANDLER( bookCopyEntry );
MENU_HANDLER( bookCopyAll );
MENU_HANDLER( bookMoveEntry );
MENU_HANDLER( bookMoveAll );
#endif

MENU_HANDLER( CphsAlsLock );
MENU_HANDLER( CphsAlsUnlock );

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
