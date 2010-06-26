#ifndef _MMI_BOOK_SERVICES_H_
#define _MMI_BOOK_SERVICES_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookServices.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the basic phone book services required by
	the other phone book modules
                        
********************************************************************************
 $History: MmiMsgs.c

	June 07,  2005   REF: CRR MMI-FIX-31545 x0021334
    	Issue description: SIM: wrong behavior when FDN is not allocated in SST
    	Solution: Before displaying the FDN options, a check is made if FDN is allocated. Only if 
                  	FDN is allocated, the options are displayed.

	Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
	Description: Current state of FDN not highlighted
	Fix: Instead of showing the submenu directly we call this function 
	where we set the menu and also higlight the previously selected menu item

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/
#define READ 3
#define WRITE 6

/* initialise and exit the phone book application
*/
void bookInitialise( MfwHnd parent );

/* SPR#1112 - Internal phonebook */
#ifdef INT_PHONEBOOK
void bookInitInternal( void);
UBYTE bookGetBookSelected(void); // RAVI
void bookSetBookSelected(UBYTE bookSelected);
UBYTE bookGetBookAvailable(void); // RAVI
void bookSetBookAvailable(UBYTE bookAvailable);
#endif

void bookExit( void );

/* The following services are provided by this module
*/
int bookFindNameInPhonebook( const char* p_pszNumber, T_MFW_PHB_ENTRY* p_pEntry );
/* SPR#1112 - SH - Add this function */
int bookFindNameInSpecificPhonebook( const char *p_pszNumber, T_MFW_PHB_ENTRY *p_pEntry );

/* Find a number given a position in the book
*/
UBYTE bookFindNumberByPosition( UBYTE index, T_MFW_PHB_ENTRY* p_pEntry );

/* Show an information dialog
*/
void bookShowInformation( MfwHnd parent, int txtId, char *text, T_VOID_FUNC cb_function );

/* SPR1112 - SH - Show an info screen
*/
T_MFW_HND bookShowInfoScreen( MfwHnd parent, int txtId1, int txtId2, T_VOID_FUNC cb_function,
	USHORT Identifier, ULONG Time);

/* Add name when the number is already known
*/
void bookAddName(T_MFW_HND win,char* number);

/* Call index
*/
int bookCallIndex(UBYTE index);

/* Convert GSM strings to their alphanumeric equivalent
*/
void bookGsm2Alpha( UBYTE *alpha );

/* Determine type of current phone book, FDN/ADN
*/
UBYTE bookActiveBook( int process );

/* Default call backs for name and number entry
*/

/* SPR#1428 - SH - Moved these from MmiBookInputWindow.h */
#ifdef NEW_EDITOR
MfwHnd bookInputStartNameEditor(   MfwHnd parent, void *buffer );
MfwHnd bookInputStartNumberEditor( MfwHnd parent, void *buffer );
void bookInputDestroy( MfwHnd window );
void bookAddNameCB( T_MFW_HND win, USHORT Identifier, SHORT reason );
void bookAddNumberCB( T_MFW_HND win, USHORT Identifier, SHORT reason );
#else /* NEW_EDITOR */
void bookAddNameCB( T_MFW_HND win, UBYTE reason );
void bookAddNumberCB( T_MFW_HND win, UBYTE reason );
#endif /* NEW_EDITOR */

/* Memory view
*/
void bookMemoryView( void );


/* Delete entry from phone book
*/
void bookDeleteEntry(T_MFW_HND Window);
void bookActivateFDN( T_MFW_HND window );
int bookFDNMenu(MfwMnu* menu, MfwMnuItem* item);//MMI SPR 18555

// Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
void settingsFocusFDNCurSel(void);

// June 07,  2005         REF: CRR MMI-FIX-31545 x0021334
// Issue description: SIM: wrong behavior when FDN is not allocated in SST
// Added prototype for function to check if FDN is allocated
void checkFDNSupport(void); 

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
