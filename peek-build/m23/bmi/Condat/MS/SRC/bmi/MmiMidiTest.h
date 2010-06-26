#ifndef _MMIMIDITEST_H_
#define _MMIMIDITEST_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MidiTest
 $File:		    MmiMidiTest.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    18/05/04                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the code for testing Riviera Midi driver
                        
********************************************************************************
 $History: MmiMidiTest.h

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

    June 19, 2006    REF: DRT OMAPS00076378  x0012849 : Jagannatha
    Description: MIDI files should play while browsing
    Solution: Added protoype of a function which will return a value based on 
     whether list of MIDI files is showed or not.
    
	Apr 05, 2005    REF: ENH 29994 xdeepadh
	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.
	
	Dec 08, 2004	REF: CRR MMI-SPR-27284
	Description: MIDI Player: Back Key malfunctions in MIDI application.
	Solution: A window is created to hold list window. When the user presses Back key, the newly 
		    created window is deleted along with list window, bringing the control back to Midi options
		    screen.
	
	18/05/04			Texas Instruments	   
 $End

*******************************************************************************/
// June 19, 2006 REF: DRT OMAPS00076378  x0012849
EXTERN BOOL MIDIstatus();
EXTERN int 	midi_test_cc_file 		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_cc_voice_limit		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_cc_channels		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_set_loop_on		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_set_loop_off		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_set_channel_mono		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_set_channel_stereo		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_play		(MfwMnu* m, MfwMnuItem* i);
EXTERN int	midi_test_play_all	(MfwMnu* m, MfwMnuItem* i);

// Dec 08, 2004	REF: CRR MMI-SPR-27284
// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Prototype for function midi_test_files_browse()
EXTERN int  midi_test_files_browse 	(MfwMnu* m, MfwMnuItem* i);
EXTERN int  midi_stop_player (MfwMnu* m, MfwMnuItem* i);
EXTERN int midi_test_set_speaker (MfwMnu* m, MfwMnuItem* i);
EXTERN int midi_test_set_headset (MfwMnu* m, MfwMnuItem* i);
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
GLOBAL int  mmi_midi_test_Application (MfwMnu* m, MfwMnuItem* i);

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

