#ifndef _MMIMMI_H_
#define _MMIMMI_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiMmi.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the root mofule for the basic MMI
                        
********************************************************************************
 $History: MmiMmi.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/
#include "MmiResources.h" //for screen size
#include "mfw_win.h"	//for MfwWinAttr definition


/*******************************************************************************
                                                                              
                    Type definitions
                                                                              
*******************************************************************************/


typedef void ( *MmiState )( int, void( * ) ( ) );


										

/*******************************************************************************
                                                                              
                    Definitions provided by the root module
                                                                              
*******************************************************************************/

/* Identifier ranges for applications. These are used to isolate
   particular message numbers for particular applications, hence
   preventing a rogue message being handled incorrectly.
*/
#define MenuDummy		  0
#define ExtrasDummy		 50
#define StartDummy		100
#define IdleDummy		200
#define GlobalDummy		300
#define ServicesDummy	400
#define SoundsDummy		500
#define TimeDateDummy	600
#define PhbkDummy		700
#define OrganiserDummy	800
#define CallDummy		900
#define PinsDummy		1000
#define SatDummy		1100
#define DialogsDummy	1200
#define MsgsDummy		1300



/* Define window areas
*/
//The following values are incorrect - but do I fix them?
#define LCD_SX SCREEN_SIZE_X
#define LCD_SY SCREEN_SIZE_Y



/* Define charging states
*/
#define GlobalDefault            0x00
#define GlobalCharging           0x01
#define GlobalBatteryPower       0x02
#define GlobalMobileOn           0x04
#define GlobalDisplayFlag        0x08




/*******************************************************************************
                                                                              
							Global data elements
                                                                              
*******************************************************************************/

/* Root module attribute structures
*/
extern MfwWinAttr g_MainWin;
extern MfwWinAttr g_IconWin;
extern MfwWinAttr g_KeyWin;

/* Display screen sizes
*/
extern U16 mmiScrX, mmiScrY;

/* Mode for the phone
*/
extern UBYTE globalMobileMode;



/*******************************************************************************
                                                                              
							Public Prototypes
                                                                              
*******************************************************************************/

void mmi_main (void);                   /* initial entry point      */
void mmiInit (void);                    /* init MMI                 */
void mmiExit (void);                    /* exit MMI                 */


										
/*******************************************************************************
                                                                              
                                End Of File
                                                                              
*******************************************************************************/

#endif

