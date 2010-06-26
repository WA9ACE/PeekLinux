#ifndef _MMIBOOKUTILS_H_
#define _MMIBO0KUTILS_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookUtils.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This module provides utility functions for the phone book.

                        
********************************************************************************

 $History: MmiBookUtils.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/* Include the phonebook types package, it will probably be
   required to get the basic types which are shared by the
   phonebook modules.
*/
#include "MmiBookShared.h"
#include "mfw_phb.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "MmiEditor.h" //for editor size definitions
#endif
/* bookCurrentWindow

   This returns the window handle associated with the current
   MFW element
*/
tBookMfwHnd bookCurrentWindow( void );


/* bookDefaultCallBack

   This is a simple call back function which is invoked when
   a not implemented dialog is displayed. We don't want anyone
   outside of this module using it so define it as static,
   this will still be callable from the info dialogs as it
   will use the reference we set up.
*/
static tBookStatus bookDefaultCallBack( tBookMfwHnd win, UBYTE identifier, UBYTE reason );


/* bookWindowData

   This routine is designed to be invoked from within a menu
   handler or call back function and will return a pointer to
   the user data associated with the current MFW element.

   The return value is a pointer to void, therefore the caller
   is responsible for casting this to the correct type prior
   to use.
*/
void *bookWindowData( void );


/* bookMenuSelectedNumber

   This will return the currently selected entry in the phone
   book, when the routine is being called from a menu handler
*/
char *bookMenuSelectedNumber( void );


/* bookNotImplemented

   This will present an information dialog indicating the current
   feature is still under development. This routine will use the
   current MFW element to detect the handle of the parent window
   if the incoming window handle is NULL.
*/
tBookStatus bookNotImplemented( tBookMfwHnd ParentWindowHandle );
tBookStatus bookMenuNotImplemented( tBookMfwMenu *Menu, tBookMfwMenuItem *Item );




/* Utility function to determine if the phone book is
   still in the loading state
*/
int bookPhoneBookLoading( void );



/* Utility functions just to throw a string onto the screen
   primarily for debug purposes, the modal version will wait
   for a user input before removing the message, the non-modal
   will clear after three seconds
*/
tBookStatus bookInfoDialog( char *String1, char *String2 );
tBookStatus bookInfoDialogModal( char *String, char *String2 );


/* SPR#1428 - SH - New Editor: No longer required */
#ifndef NEW_EDITOR
/* Set editing attributes
   This constructs a edit attribute block from the provided
   parameters
*/
// GW - Changed colour from 8 bits (foreground) to 2x32bits fgd and bgd
void bookSetEditAttributes(
    int editZone,
	U32  colindex,
	U8  font,  U8  mode,  U8  *controls,
	char *text,   U16 size,
	MfwEdtAttr*   attr);
#endif
    
/* Memory allocation and freeing functions

   These routines provided guarded access to the malloc and free
   routines used in the MMI application. The wrapper functions
   provided here allow debugging and tracking information to be
   easily derived, it also allows the memory allocation and
   deallocation to be changed if required.

   Note that the free routine takes a pointer to the pointer
   to the memory being freed. This allows the actual pointer
   value to be set to NULL as part of the free operation, this
   helps protect against reusing the pointer after the memory
   has been deallocated.

   Sample usage:

		tStructure *ptr;

        if ( ( ptr = (tStructure *) bookMemAlloc( sizeof(tStructure) ) ) != NULL )
		{
			// use the allocated memory
			//
			...

			// free the memory
			//
			bookMemFree( &ptr );
		}
*/
void *bookMemAlloc( long int NumBytes );
void bookMemFree( void **Pointer );



/* Name handling utility functions
*/
UBYTE bookFindName(   UBYTE number, tMmiPhbData *current );
//UBYTE changeName( UBYTE book,   T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS *currentStatus );
//UBYTE deleteName( UBYTE book,   UBYTE index,            T_MFW_PHB_STATUS *currentStatus );
//UBYTE addName(    UBYTE book,   T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS *currentStatus );

/* File handling functions
*/
void changeInFile(   T_phbk *phbk );
void deleteFromFile( T_phbk *phbk, UBYTE book );
void storeInFile(    T_phbk *phbk );

/* SPR#1112 - SH - Internal phonebook use */
#ifdef INT_PHONEBOOK
void bookChangePB(T_MFW_HND win, UBYTE identifier, UBYTE reason);
UBYTE bookCopy( T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS	*destStatus, T_MFW_PHB_STATUS	*srcStatus, UBYTE move);
void bookCopySingle(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void bookCopyMultiple(T_MFW_HND win, UBYTE move, UBYTE reason);
#endif

UBYTE bookGetCurrentStatus( T_MFW_PHB_STATUS *currentStatus );
int bookEvent( MfwEvt e, void *para );

/* Copy name in phonebook structure
*/
void bookCopyPhbName( T_MFW_PHB_ENTRY* destEntry, T_MFW_PHB_ENTRY* srcEntry, int maxLen );


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
