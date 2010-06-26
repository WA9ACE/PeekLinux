#ifndef _MMI_BOOK_DETAILS_WINDOW_H_
#define _MMI_BOOK_DETAILS_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookDetailsWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

	Provides the details handlig for the phone book
 
                        
********************************************************************************
 $History: MmiBookDetailsWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/* Entry point for the details window
*/
void bookDetailsStart( MfwMnu* m, MfwMnuItem* i );

/* Destruction point
*/
void bookDetailsDestroy( MfwHnd window );


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
