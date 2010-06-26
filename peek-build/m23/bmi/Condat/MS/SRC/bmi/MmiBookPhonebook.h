#ifndef _MMI_BOOK_PHONEBOOK_H_
#define _MMI_BOOK_PHONEBOOK_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookPhoneBook.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description

    This module provides the basic phone book functionality

  
********************************************************************************
 $History: MmiBookPhoneBook.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/


/* entry point for the phone book window
*/
T_MFW_HND bookPhonebookStart( MfwHnd parent, int reason );


/* Provide the destroy function for subordinate windows to clean up
   when necessary
*/
void bookPhonebookDestroy( MfwHnd window );



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
