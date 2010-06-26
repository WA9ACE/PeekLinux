#ifndef _MMISTART_H_
#define _MMISTART_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Start
 $File:		    MmiStart.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the start code for the MMI
                        
********************************************************************************
 $History: MmiStart.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/

#include "mfw_win.h"




/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/

typedef enum
{
	PhoneInit = StartDummy,
	FirstTimeNMFound,
	NmSearching
}E_PHONE_EVENTS;



/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/* pretty standard initialise, execute and exit methods
*/
void startInit( void );
void startExec( int reason, MmiState next );
void startExit( void );


/* returns the handle of the currently focussed window
*/
MfwHnd startWhoIsFocused(void);

/* SPR877 - SH - moved here from .c file */
void showwelcome(T_MFW_HND win);

int mmiStart_animationComplete( void );

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif

