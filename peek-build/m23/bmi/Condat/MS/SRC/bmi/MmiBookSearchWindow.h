#ifndef _MMI_BOOK_SEARCH_WINDOW_H_
#define _MMI_BOOK_SEARCH_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiSearchWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
	This module provides the phone book search window functionality for the
	phone book module of the basic MMI
                        
********************************************************************************
 $History: MmiSearchWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/



/* Entry point to the search window functions
*/
T_MFW_HND bookSearchStart( MfwHnd parent );

/* Destruction point
*/
void bookSearchDestroy( MfwHnd window );



/* Additional helper functions provided by this module
*/
UBYTE bookSearchName( char *pattern, UBYTE number, tMmiPhbData *current );



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
