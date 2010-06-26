#ifndef _MMIMP3TEST_H_
#define _MMIMP3TEST_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MP3Test
 $File:		    MmiMP3Test.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    18/05/04                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

	MMI Application to test the different Audio file formats.
                        
********************************************************************************
 $History: MmiMP3Test.h

	18/05/04			Texas Instruments	   

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

       Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
       Bug:MP3 RWD and FWD function - MOT
       Fix: Forward and Rewind functionalities are handled.

	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
	Description: Implementation of Test Menu for AAC
	Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
	The interfaces have been made generic to support both MP3 and AAC files.

	Sep 27,2005 REF: SPR 34402 xdeepadh  
	Bug:Mono option is always highlighted even when Stereo is selected
	Fix:The channel setting will be saved in the FFS, which will be retrieved later
	and the selected channel will be higlighted.
	
	Jul 18, 2005    REF: SPR 31695   xdeepadh
	Bug:Problems with MP3 test application
	Fix:The window handling of MP3 Test Application has been done properly.

 $End

*******************************************************************************/
EXTERN int	mmi_audio_player_set_channel_mono		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	mmi_audio_player_set_channel_stereo		(MfwMnu* m, MfwMnuItem* i);
//Jul 18, 2005    REF: SPR 31695   xdeepadh
EXTERN int  mmi_audio_player_files_browse 	(MfwMnu* m, MfwMnuItem* i);
GLOBAL int  mmi_audio_player_play (MfwMnu* m, MfwMnuItem* i);
//Sep 27,2005 REF: SPR 34402 xdeepadh  
 void mmi_audio_player_set_cur_selected_channels(void);
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
GLOBAL int  mmi_audio_player_Application (MfwMnu* m, MfwMnuItem* i);

#if (BOARD == 71)
//  Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
int M_exePause (MfwMnu* m, MfwMnuItem* i);
int M_exeForward (MfwMnu* m, MfwMnuItem* i);
int M_exeRewind (MfwMnu* m, MfwMnuItem* i);
#endif

/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/

extern USHORT record_position;



/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/




/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/* pretty standard initialise, execute and exit methods
*/


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif

