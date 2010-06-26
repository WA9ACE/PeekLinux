#ifndef _MMI_BOOK_MENU_WINDOW_H_
#define _MMI_BOOK_MENU_WINDOW_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookMenuWindow.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    Menu handling for the phone book
                        
********************************************************************************
 $History: MmiBookMenuWindow.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/


/* Entry point for the menu windows
*/
T_MFW_HND bookMenuStart( MfwHnd parent, MfwMnuAttr *menuAttr,SHORT reason );


/* Destruction point
*/
void bookMenuDestroy( MfwHnd window );



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
