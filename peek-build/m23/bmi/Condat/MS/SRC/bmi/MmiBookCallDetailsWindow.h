#ifndef _MMI_BOOK_CALL_DETAILS_WINDOW_H_
#define _MMI_BOOK_CALL_DETAILS_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookCallDetailsWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the implementation of the call details window for the
	phone book module
                        
********************************************************************************
 $History: MmiBookCallDetailsWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/



/* entry point for the call details window
*/
T_MFW_HND bookCallDetailsStart( MfwHnd parent );

/* Destruction point
*/
void bookCallDetailsDestroy( MfwHnd window );

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
