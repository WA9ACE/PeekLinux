#ifndef _MMI_BOOK_CALL_LIST_WINDOW_H_
#define _MMI_BOOK_CALL_LIST_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookCallListWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the call list window handling for the phone book.
                        
********************************************************************************
 $History: MmiBookCallListWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/* entry point for the call details window
*/
T_MFW_HND bookCallListStart( MfwHnd parent,int parameter );

/* Destruction point
*/
void bookCallListDestroy( MfwHnd window );

extern int idCounter;


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/


#endif
