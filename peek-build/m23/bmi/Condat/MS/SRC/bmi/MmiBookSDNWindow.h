#ifndef _MMI_BOOK_SDN_WINDOW_H_
#define _MMI_BOOK_SDN_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookSDNWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module provides the service numbers window for the phone book
                        
********************************************************************************
 $History: MmiBookSDNWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/



/*******************************************************************************
                                                                              
                         Methods provided by module
                                                                              
*******************************************************************************/


/* We need to provide an entry point for the
   window, everything else can be derived from here
*/
T_MFW_HND bookSDNStart( MfwHnd parent );


/* Destruction point
*/
void bookSDNDestroy( MfwHnd window );


/* include a tuned search facility to find a name in
   the service numbers phone book
*/
int bookFindNameInSDNPhonebook( const char* p_pszNumber, T_MFW_PHB_ENTRY* p_pEntry );


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
