#ifndef _MMI_GAME_H_
#define _MMI_GAME_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Calculator
 $File:		    Mmigame.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    03/07/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

   	 This provides the main game functionality
                        
********************************************************************************

*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/


extern T_MFW_HND four_in_a_row_create(MfwHnd parent);
//extern void melody_generator_destroy (void);
extern void four_in_a_row (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
extern int four_in_a_row_kbd_cb (MfwEvt e, MfwKbd *k);
extern int four_in_a_row_win_cb (MfwEvt e, MfwWin *w);
extern T_MFW_HND four_in_a_row_start (T_MFW_HND win_parent,char *character);

/*******************************************************************************
                                                                              
                                Interface types
                                                                              
*******************************************************************************/
typedef enum
{
    SHOWGAMEINFO_INIT
} GameInfo_events;

/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/* pretty standard initialise, execute and exit methods
*/
int four_in_a_row_aktivate(MfwMnu* m, MfwMnuItem* i);

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif

