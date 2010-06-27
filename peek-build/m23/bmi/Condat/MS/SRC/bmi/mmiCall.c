/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   Call
 $File:       MmiCall.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description

    This module provides the call functionality


********************************************************************************
 $History: MmiCall.c

	August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat)
 	Description:  COMBO PLUS: Voice can not be heard in a call when MP3 tone was set
 	                                         Have an MP3 tone set as Ringer. Make a call to the target.
 	                                         Accept the same, no voice can be heard from the calling 
 	                                         device or from the board that received 
       Issue:	      Timing Synchronization Issue between BMI-ACI-L1.
 	                    BMI calls sAT_A to accept the call. ACI then sends the RING_OFF signal
 	                    to BMI, wherein BMI does a Ringer Stop. After this, ACI calls Vocoder
 	                    Enable. However, the Ringer Stop goes to L1 much later after Vocoder
 	                    Enable reaches L1 due to which the Vocoder eventually gets disabled.
 	Solution:       The Vocoder Enable Implementation is removed from ACI and
 	                    shall be called by BMI on receipt of AS_STOP_IND
 	                    (AS_STOP_IND is the message sent by AS to BMI when BMI invokes
 	                     as_stop for ringer_stop). This way, it is assured that the Vocoder
 	                     Enable Request reaches L1 only after Ringer Stop is completely done

	July 06, 2007 DR: OMAPS00137334 x0062172(Syed Sirajudeen)
 	Description:[MPY]Wrong indication when add party if conference full
 	Solution: Pop up the "Conference Full" string if user tries to add (MAX_CALLS+1)th user
 			to the conference.
 	
	 	
 
	May 28, 2007 DR: OMAPS00123948 x0066692
	Description: MMI doesn't display specific dialogs for enabling TTY_VCO and TTY_HCO
	Solution : In call_tty_statuswin() Dialog data is initialized specific to TTY_VCO and 
	TTY_HCO and TTY_ALL. Also conditions CALL_TTY_ALWAYSON and CALL_TTY_ONNEXTCALL
	are applied while displaying hte dialog.
	
	Mar 21, 2007 DR: OMAPS00121645 x0066814
	Description: Bip ringer are always heard during calling and even during the voice call
	Solution : In E_CM_CONNECT_ACK event, the code to stop the ringer is added.
	
 	
	May 16, 2007 DR: OMAPS00117598 x0066692
	Description: Ringer instabilities during incoming call when headset is plugged.
	Solution : Function mfw_hook_register() is called after starting the ringer during 
			incoming call.

	May 10 2007 DR:OMAPS00127983 x066814
	Description: PSTN caller name not displayed on MS
	Solution: In incomingCall() funtion, added a case to check whether 
	the name is updated by rAT_PercentCNAP.
	
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 
 
	Mar 22, 2007,DR: OMAPS00121671 x0066814(Geetha) 
	Description: Bip ringer are always heard during calling and even during the voice call
	Solution: In E_CM_CONNECT_ACK event, the code to stop the ringer is added
 	
	Feb 16, 2007 DR: OMAPS00114834 x0pleela
	Description: When accepting the call through AT commands with MP3 ringer, the corrupted 
				MP3 play continues
	Solution: Made changes in the function:
			1) call_incoming_cb(): In KCD_RIGHT and KCD_LEFT keypad events, the code to 
				stop the ringer is commented as MMI should stop the ringer when it recieves
				the RING_OFF event from ACI rather than stopping the ringer when user 
				accepts or rejects the call using keypad 
			2) callcmevent(): In E_CM_CONNECT_ACK event, the code to stop the ringer is 
				commented as this code is moved and handled in call event E_CM_RING_OFF
	
	Jan 10, 2007 DR: OMAPS00110568 x0039928	
	Description: Remove duplication of FFS.Flashdata from GDI
	Solution: Added a new function get_call_data_tty().
	
	Dec 22, 2006 REF:OMAPS00107042  x0039928
       Description : Tagrget hangs or crashes when making MO call with Agilent
       Solution     : cm status variable is otpimized to get memory from heap
       to avoid stack overflow.
       
 
 	Dec 13, 2006 DR:OMAPS00107103 a0393213(R.Prabakar)
 	Description : No DTMF tones for simultanous keypress
 	Solution     : Once there was some problem in sending DTMF tones, the problem persisted till the next reset.
 	                  This was because the queue was not getting flushed once the call was ended. 
 	                  Now the queue is flushed once the call is ended.
 	
      Nov 03, 2006  DR: OMAPS000101158 x0pleela 
      Description : Board crashes after sending atd command through HyperTerminal
      Solution    : Avoid peforming phonebook search operation if the source id is not LOCAL
      			   callCmEvent: Check for the value of uOthersrc and if set donot perform phonebook 
    				search operation. Else perform phonebook search operation
      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI

	Sep 01, 2006 REF:OMAPS00090555  x0039928
       Description : When make a MO call,MFW can not get call number from ACI.
       Solution     : The called number is extracted from the parameter of callCmEvent() 
       for event E_CM_MO_RES and is displayed. Also stack memory optimization is done to 
       resolve MMI stack overflow problem.

	xrashmic 21 Aug, 2005 OMAPS00090198
	The mmeSetVolume is skipped when in a TTY call, as TTY has it own profile 
	and when this profile is loaded, the volume would also be set.

	Aug 17, 2006 REF:OMAPS00090196  x0039928
       Description : HOOk Register / Unregister shall not be called when TTY is ON
       Solution     : TTY ON condition is checked before registering / unregistering for the hook.
       
   	Aug 31, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	- Removed the new code added in callnumber() : MFW_SS_USSD
	switch case to send the USSD string through at+cusd command. Instead sending through ATDn command
	
 	Aug 30, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	- Calling function GLOBAL USHORT get_nm_status(void) as per the review 
				comments to avoid the usage of the global variable gnm_status
				
	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	callnumber(): MFW_SS_USSD is handled separately where send_ss_ussd() is called to 
			initaite USSD transaction
	
	Aug 03, 2006 REF:OMAPS00088329  x0039928
       Description : TTY> When TTY is ON Headset insertion detection / profile download should not happen
       Solution     : TTY ON condition is checked before downloading the headset profile.

 	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Added new enum "CALL_ACTIVE_LINE", to display current active line along 
				with the called/calling number
			b) Modified function "inCall_displayData", "call_win_cb" to display current active 
				line along with the called/calling number
		
	Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
	Description: Triton PG2.2 impacts on Locosto program
	Solution: Software workaround is done to register headset-hook dynamically 
	during/before call is established and unregister it after the call is ended/disconnected.
	
	May 18, 2006    REF: DRT OMAPS00076438  xdeepadh	
	Description: Scrolling not implemented in Imageviewer
	Solution: The support for scrolling has been provided.


 	May 15, 2006    DR: OMAPS00075901 x0021334
	Descripton: Call Waiting - Release and Accept  - incompatible usage of MMI and AT interpreter
	Solution: The call waiting tone is stopped on receipt of E_CM_RING_OFF event.

	May 9, 2006    REF:DR OMAPS00074884  xrashmic
	Description:   Pressing Red key in scratch pad does not release calls but deletes the characters
	Solution: Pressing Red key in scratch pad releses all calls. If there are no calls, it would destroy
			the scratch pad.

    Apr 26, 2006 OMAPS00074886 a0393213 (Prabakar R)
    Description : No option in MMI to release the held call. 
    Solution    : No option was available. Added a menu option.
    
	Mar 31, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added prototype for voice_buffering_stop_recording
			Added check before calling play/record stop
		
 	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Function: callCmEvent
			Changes: 1. E_CM_DISCONNECT: Added code for voice buffering to stop playing 
						and recording if PTT call is disconnected.
					2. E_CM_CONNECT_ACK: Added code for voice buffering to start playing 
						if PTT call is connected.

			Function: call_kbd_cb
			Changes: Added code to update voice_buffering_data when END key is pressed 
					while in PTT call

			Function: incomingCall
			Changes: Added code to reject incoming call while in buffering phase 

			Function: call_calling_cb
			Changes: Added code to update voice_buffering_data when END key is pressed 
					before PTT call setup

	Mar 28, 2006    REF:DR OMAPS00072407  x0039928
	Description:   Phone hangs when tried to make an outgoing call
	Solution: Reverting back the changes of issue OMAPS00048894 to stop from stack overflow.

	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: During an event CALL_OUTGOING_SAT in the function Call() Copied sim icon data in to the 
	structure call_data inorder to display it on the  dialog screen.
	
	Feb 13, 2006    REF:DR OMAPS00067943  x0039928
	Description:   MMI gives access to Application during active call in Plus but not in Lite
	Solution: The Menu function M_exeViewfinder is renamed to M_exeApplication and moved out of
	FF_MMI_TEST_CAMERA flag.

	Jan 27, 2006    REF:DR OMAPS00048894  x0039928
	Description:   Outgoing Call: The called number is not displayed while an outgoing call is made
	Solution: The called number is extracted from the parameter of callCmEvent() for event E_CM_MO_RES and is displayed.

	Jan 05, 2006    REF:DR OMAPS00050454  x0039928
	Description:   Phone power cycles while deflecting a call to another number
	Solution: The calling window dialog is destroyed if there are no more calls.

 	Nov 09, 2005 REF: OMAPS00043190 - x0018858
	Description: PTCRB - 27.22.4.1.4 (DISPLAY TEXT (Sustained text) Fails in 1900 DISPLAY TEXT
			    (Sustained text) 
	Solution: Have added a new global variable to store the handle of the window and then release
			it when a higher priority call event comes in. 

 	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.

 	Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
	Description: Locosto: MIgration to New LCD APIs
	Solution: Migrated to New APIs by replacing the old driver APIs with
	corresponding New LCD APIs

 	Aug 22 2005, xpradipg - LOCOSTO-ENH-31154
 	Description:	Application to test camera
 	Solution:	Implemented the camera application with following functionalities
 			preview, snapshot and image saving.
 
 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 	Description:	Support for various audio profiles
 	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 			The audio device is enabled as per the user actions.	
 			
   Apr 06, 2005	REF: ENH 30011 xdeepadh
   Description: Replacing the Test Application compilation flags with new flags. 
   Solution:  The existing flags for Camera and MP3 test application have  beeen replaced with the 
   new compilation flags,FF_MMI_TEST_CAMERA and FF_MMI_TEST_MP3 respectively.

   April 25, 200   REF: MMI-FIX-30125   x0018858
   Description: Unable to make a DTMF call with one call on hold.
   Solution: Existing inmplemention allowed DTMF for only the first call that was being made. Modfified the
   condition to allow DTMF for the second call also.

   Apr 05, 2005    REF: ENH 29994 xdeepadh
   Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
   Solution: Generic Midi Ringer and Midi Test Application were implemented.
 

   Feb 03, 200   REF: CRR 25960   x0012850
   Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible
   Solution: swapping action results in one of the calls getting muted. We attach user connection again.

    Jan 19, 2005 REF: CRR MMI-SPR-27455 xnkulkar
    Description: TC 29.3.2.6.7 failed on 900 on R&S
    Solution: function "mfw_aud_l1_enable_vocoder ()" should be called only when the 
    call mode is not DATA or FAX

    Nov 29, 2004    REF: CRR 25051 xkundadu
    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
    Fix: Added volume level list linked to up/down keys. 
          User can select the speaker volume among those levels.

    Nov 16, 2004 REF: CRR MMI-SPR-26120 xnkulkar
    Description: After disconnecting MOC cannot return to STK menu 
    Solution: When the user ends a call, check if it is a STK call and send SAT_RETURN event to 
    		    return the control to STK menu.

    Oct 27, 2004 REF: CRR MMI-SPR-15632 xnkulkar
    Description: SMS : class 2 and 3 SMSMT : icon not displayed when 2 active calls
    Solution: In callCmEvent() under case case E_CM_TIMER_EVERY_SECOND, 
    		    condition "call_data.calls.mode == CallSingle" is removed to make the sms icon visible
	           even if there are more than one calls present. 

    October 07, 2004 REF: CRR MMI-FIX-24440 xnkulkar
    Description: Bug in mmicall.c callID related Error
    Solution: In holdExec() function "callIndex()" function is used instead of directly 
	          using status[call_data.calls.selected]

	Bug Id 14 & 21 : 		12 Aug, 2004 - xrashmic
	Description: Presing up/Down scroll key displays alert mode optins, 
	which overlaps with incoming call screen
	Solution:	Up and down scroll keys have been disabled for incoming call screen

 	Sep 2, 2004    REF: CRR 21370 xkundadu
	Description: Giving wrong expected result by long press on "#" key
	Fix:	When user presses long on '#' key it will display 'p'.
		Changed the long press of '*' key to display 'w'

//  Jul 2, 2004        REF: CRR 21421  xvilliva
//	Bug:  ALS:Action on the other line when change is locked. 
//	Fix:  The error code returned by ACI is processed and an alert 
//		  is flashed to the user.

 	Jul 14, 2004 REF=CRR 13847 xkundadu
	Description: Have one active call and one held call. 
			    If you end the active call using MMI, the Held call is still on 
			    hold and UNHOLD has to be selected to retrieve the call 
			    If you end the active call using BPM then the held call 
			    automatically beocmes an active call, shouldn't there be
			    consistency between the two?
	Solution:	    After all the disconnection, if there is only one call and
			    that call is HELD, retrieve that call to make it as ACTIVE.
			    Added the boolean variable bhostDisconnection to test 
			    whether the disconnection process started from host side or not
			    If disconenction is from remote side, after the 
			    disconnection if there is only one call and that is HELD, dont
			    change its status to ACTIVE.
			    

//  Jun 30, 2004        REF: CRR 13698  xvilliva
//	Bug: No possibility to make an emergency call when active call and held 
//		 call are existing. 
//	Fix:   If the call being attempted is an emergency call in the presence 
//		 of a held call and an active call, we release all current calls by
//		 calling cm_end_all() and initate the emergency call on receiving the
//		 E_CM_DISCONNECT_ALL event.

//  Jun 10, 2004    REF=CRR 13602	Sasken/xreddymn
//		Added * and # key handling, to allow the input of 
//		* and # characters in the scratchpad screen

//  June 08, 2004 REF: CRR MMI-SPR-13876 xkundadu(Sasken)
//  Issue description: CHW : AUDIO : No difference between CW and incoming call
//  If there is one Active call and one wiating call and if the Active call is disconnected 
//  from the remote side/network side, the waiting tone should change to ringing tone.

    Jun 30, 2004  REF: CRR 15685 xkundadu(Sasken)
    Description: FT - TC5.3 MOC - No busy tone upon MOC to busy 
    			 subscriber/GOLite.
    Solution:	 Added code to play beep sound when the called party  is busy.
			 When the remote party is busy, earlier there was only dialog 
			 box indication that 'Number busy'
 
    Fix	:	"Setting" menu has been provided to set the SMS settings, during the active call

	25/10/00      Original Condat(UK) BMI version.



    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY

 $End

*******************************************************************************/


/******************************************************************************

                                Include Files

*******************************************************************************/

#define ENTITY_MFW
/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"

#include "mfw_sim.h"
#include "mfw_cm.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"
#include "mfw_cm.h"
// Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif //FF_MIDI_RINGER 

//Apr 06, 2005	REF: ENH 30011 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
//e-armanetsaid 19/08/04
#include "mfw_cam.h"
//e-armanetsaid 19/08/04
#endif //FF_MMI_TEST_CAMERA

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#include "MmiEditor_i.h"
#endif


#include "MmiMenu.h"
#include "MmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"
#include "MmiResources.h"	/* sbh */
#include "MmiCPHS.h"

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"


#include "mfw_ffs.h"
#include "MmiTimers.h"
#include "mmiSmsMenu.h"
#include "mmiColours.h"


// 	xpradipg - LOCOSTO-ENH-31154 : 22 Aug 2005
#ifdef FF_MMI_TEST_CAMERA
#include "r2d/r2d.h"
#endif

//x0pleela 09 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB 
#include "mfw_aud.h"
#endif
//  June 08, 2004 REF: CRR MMI-SPR-13877 Ajith K P
//  Issue description: CHW : AUDIO : CW tone not stopped
//  1 call is active
//  1 call is waiting call
//  Waiting tone is heard
//  The user press 2+SEND => 1 CH and 1 active call
//  The waiting tone is not stopped.

//  June 08, 2004 REF: CRR MMI-SPR-13876 xkundadu(Sasken)
//  Issue description: CHW : AUDIO : No difference between CW and incoming call
//  If there is one Active call and one wiating call and if the Active call is disconnected 
//  from the remote side/network side, the waiting tone should change to ringing tone.



/* SPR#1983 - SH - Required for WAP disconnect event */
#ifdef FF_WAP
#include "AUIWapext.h"
#endif

//x0pleela 07 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "MmiPins.h"
#endif //FF_PHONE_LOCK


//x0pleela 23 Aug, 2006  DR: OMAPS00083503
//EXTERN USHORT get_nm_status(void); 

/* Marcus: Issue 1057: 21/01/2003: Added the following two external prototypes */
extern T_MFW_HND sat_get_setup_menu_win (void);
extern T_MFW_HND sat_get_call_setup_win (void);

USHORT item_flag_hold( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT item_flag_swap( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
// Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
// prototype of the function which determines whether the menu option is to be displayed or not.
USHORT item_flag_endheld( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );

/*******************************************************************************

                              External Function Prototypes.

*******************************************************************************/

extern T_MFW_HND satDisplayTextWin; // Nov 09, 2005 REF: OMAPS00043190 - x0018858
					    // Externed this sat display text handle to make it accessible here.
					    
extern UBYTE getCurrentRingerSettings(void );
extern int M_exeSendNew(MfwMnu* m, MfwMnuItem* i); // MZ
extern MfwMnuAttr *settingMenuAttributes(void);//May 27, 2004    REF: CRR 19186  Deepa M.D 
extern MfwMnuAttr *applicationsMenuAttributes(void);//August 24, 2004    e-armanetsaid
void deflect_destroy(BOOL delete_call_windows); //GW
/*******************************************************************************
                End
*******************************************************************************/
//  Jun 30, 2004  REF: CRR 15685 xkundadu(Sasken)
//  Added this AUDIO_PLAY_THRICE define to play the beep sound three times
#define AUDIO_PLAY_THRICE  (AUDIO_PLAY_ONCE *3)

#define TRACE_MMI_CALL

#ifndef TRACE_MMI_CALL
//#define TRACE_EVENT(a)
#endif

#if defined (WIN32)
#define TONES_AUTH_NUM  3
#define TONES_BUSY      4
#define TONES_DROPPED   5
#define TONES_CONGEST   6
#endif



#ifndef MFW_EVENT_PASSED
#define MFW_EVENT_PASSED 0
#endif

#define MMI_OUTGOING_CALL_DIALOG_NORMAL        (1)
#define MMI_OUTGOING_CALL_DIALOG_SAT           (2)

#define NUMERIC_KEYS 		0x8FF	/*MC for setting up dialogue key events*/

#define MAX_SOUND 255

/* Marcus: CCBS: 14/11/2002: Start */
/* CCBS related constants */
#define MAX_CCBS_LIST_ITEMS 5
#define MIN_CCBS_IDX        1
#define MAX_CCBS_IDX        (MAX_CCBS_LIST_ITEMS)


/* CCBS list structure type definitions */
typedef struct
{
    SHORT   idx;                    /* ACI's index to this callback */
    UBYTE   number[MFW_NUM_LEN];    /* Phone number of callback */
    /* Name in phonebook corresponding to phone number of callback, if any */
#ifdef NO_ASCIIZ
    T_MFW_PHB_TEXT   name;
#else
    UBYTE            name[PHB_MAX_LEN];
#endif
} T_CCBS_LIST_ITEM_STR;

typedef struct
{
    T_MFW_HND               win;    /* Parent window of active callbacks list */
    ListMenuData            *list_menu_data;    /* allocated list menu memory */
    UBYTE                   count;  /* Number of valid items 0..MAX_CCBS_LIST_ITEMS */
    UBYTE                   sel;    /* Currently selected item 0..MAX_CCBS_LIST_ITEMS-1 */
    T_CCBS_LIST_ITEM_STR    item[MAX_CCBS_LIST_ITEMS];
} T_CCBS_LIST_STR;

/* CCBS recall type definitions */
typedef struct
{
    T_MFW_HND           win;    /* Parent window of active callbacks list */
    T_MFW_CM_CALL_TYPE  type;                      /* type of this call    */
    UBYTE               number[MFW_NUM_LEN];    /* Phone number of callback */
    /* Name in phonebook corresponding to phone number of callback, if any */
#ifdef NO_ASCIIZ
    T_MFW_PHB_TEXT      name;
#else
    UBYTE               name[PHB_MAX_LEN];
#endif
} T_CCBS_RECALL_STR;


/* CCBS related local module global data */
static T_CCBS_LIST_STR ccbs_list;   // CCBS list information structure
static T_MFW_HND ccbs_registration_win = NULL;  // Callback registration window handle
static T_CCBS_RECALL_STR ccbs_recall;  // Callback recall information structure
static char ccbs_recall_text_buffer[80];

/* Data for the list of registered CCBS */
static MfwRect ccbs_list_menuArea      = {0,0,10,10};
static MfwMnuAttr ccbs_list_menuAttrib =
{
    &ccbs_list_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_NETWORK, TxtNull, NULL, MNUATTRSPARE
};

//Daisy tang added for Real Resume feature 20071107
//start
BOOL IsRealPaused=FALSE;
extern BOOL IsMP3Playing;
static MfwHnd  mp3playTimer;
//end
/* CCBS related local function prototypes */
static void call_ccbs_show_status(void);
static void call_initialise_ccbs_list(T_MFW_HND win); // Marcus: CCBS: 14/11/2002
static int call_ccbs_callback(T_MFW_HND win, USHORT identifier, UBYTE reason);
static void call_cm_ccbs_event(T_MFW_CM_CCBS_INFO *para);
static void call_add_to_ccbs_list(T_MFW_CM_CCBS_INFO *para);
static void call_delete_from_ccbs_list(void);
static void call_create_ccbs_menu_list (void);
static void call_ccbs_menu_list_cb(T_MFW_HND win, ListMenuData * ListData);
static void call_show_ccbs_menu_list(void);
static int call_offer_ccbs_registration_callback(T_MFW_HND win, USHORT identifier, UBYTE reason);
static void call_offer_ccbs_registration(T_MFW_CM_CCBS_INFO *para);
static void call_withdraw_ccbs_registration(LangTxt TxtId);
static void call_offer_ccbs_recall(T_MFW_CM_CCBS_INFO *para);
static int call_offer_ccbs_recall_callback(T_MFW_HND win, USHORT identifier, UBYTE reason);
static void call_withdraw_ccbs_recall(LangTxt TxtId);
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)*/
EXTERN T_MFW_PHB_TEXT *get_cnap_name();
//CALL_NAME_NETWORK = TRUE implies that name is got from rAT_%CNAP
//CALL_NAME_NETWORK = FALSE implies that either the name is from phonebook or
//there is no name(only number is displayed)
EXTERN BOOL CALL_NAME_NETWORK;
//  MMI-SPR 13847 xkundadu
//  Added this variable to identify whether the disconnection is intiated from
//  local mobile station or not.
BOOL bhostDisconnection = FALSE;

/* Marcus: CCBS: 14/11/2002: End */

static UBYTE LocalRing=FALSE;
UBYTE  call_SATCall = FALSE;    // Marcus: Issue 1057: 21/01/2003: Made public
static UBYTE InCallTimer = TRUE; //This ought to be stored in ffs

// Default "dialogDisp" should be TRUE! This has been added so that removeAllCalls doesn't display a dialog for each call
//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
// When removing calls one by one from removeAllCalls, rebuild calls should not be called
// Added a new paramter to the removeCall function to indicate the same
int removeCall (SHORT cid, BOOL dialogDisp,BOOL allCallsEnded);
int removeAllCalls(void);
int addCall (SHORT cid);
int callCmEvent (MfwEvt e, void *para);

static int InformationTimerEvent(MfwEvt e, MfwTim *t);

/* x0039928 - Commented for lint warning removal 
static int callWaitingAnswerKeyEvent (MfwEvt e, MfwKbd *k);   */
void callAutoRedialKeyEvent(T_MFW_HND win, USHORT identifier, USHORT reason);

// Default "dialogDisp" will be True, and affects only the "ViewEnd" mode.
static void callSetMode(int mode, BOOL dialogDisp);

static int contactsExec(MfwMnu* m, MfwMnuItem* i);
static int M_exeSetting(MfwMnu* m, MfwMnuItem* i);//May 27, 2004    REF: CRR 19186  Deepa M.D 
// Feb 13, 2006    REF:DR OMAPS00067943  x0039928
// Fix : The Menu function M_exeViewfinder is renamed to M_exeApplication
static int M_exeApplication(MfwMnu* m, MfwMnuItem* i);//August 24 e-armanetsaid
static int holdExec(MfwMnu* m, MfwMnuItem* i);
static int muteExec(MfwMnu* m, MfwMnuItem* i);
static int scratchPadExec(MfwMnu* m, MfwMnuItem* i);
static int mainMenuExec(MfwMnu* m, MfwMnuItem* i);
static int startConferenceExec(MfwMnu* m, MfwMnuItem* i);
static int swapExec(MfwMnu* m, MfwMnuItem* i);
static int endAllExec(MfwMnu* m, MfwMnuItem* i);
// Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
// Prototype of the handler for "End held" menu option
static int endHeldExec(MfwMnu* m, MfwMnuItem* i); 
static int endConferenceExec(MfwMnu* m, MfwMnuItem* i);
/* x0039928 - Commented for lint warning removal
static int transferExec(MfwMnu* m, MfwMnuItem* i);*/

static int privateStartAddConferenceExec(MfwMnu* m, MfwMnuItem* i);
static int exeSendMessage(MfwMnu* m, MfwMnuItem* i);
static int callExeNameEnter(MfwMnu* m, MfwMnuItem* i);
static void showCCNotify (T_MFW_HND win, T_MFW_CM_NOTIFY  * cc_notify);

/* x0039928 - Commented for lint warning removal 
static int call_incoming_animation_cb(MfwEvt e, MfwTim *t); */


static void showSelectPrivateCall(void);
static void showWaitingAnswer(void);
static void showAutoRedial(void);

static void endingCall(void);
static void rebuildCalls(void);
static int getCallStatus(SHORT call_number,T_MFW_CM_STATUS* call);
void incomingCall(const T_MFW_CM_CW_INFO* p_pCwInfo);
void callConnect(SHORT cid);
void getTimeString(long time,char *timeString);
static void refreshCallTimer(void);  // RAVI

static void callFailureCause(UBYTE cause);
static void sendDTMFString(void);
/* x0039928 - Commented for lint warning removal 
static void callCalcCost(char* line);    */


void call_show_ending_all(void); // RAVI


char* callInformation;
//xvilliva SPR13698, strECC used to store the emergency number. 
static char strECC[EDITOR_SIZE];
//xvilliva SPR13698, "makeECC" indicating the need to intiate emergency call.
static BOOL makeECC = false;	
int callIndex(SHORT cid);
char line[MAX_LINE];


static SHORT singleCall;

int idwithheld;

//	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
extern UBYTE carkitInserted;
extern UBYTE headsetInserted;
#endif
/*x0039928 OMAPS00097714 HCO/VCO option - ttypPfType is used to store the type of TTY (normal/HCO/VCO)*/
#ifdef FF_TTY_HCO_VCO
UBYTE call_data_tty;
extern UBYTE ttyPfType;
#endif

/* SPR#1985 - SH - TTY - If TRUE, updates will display a window*/

static UBYTE TTYDisplayDialog = FALSE;

//18/12/2001  MC added sms message option
static  T_MFW_MNU_ITEM menuCallSingleItems[] =
{
    {0,0,0, (char*) TxtSoftContacts,  0,(MenuFunc)contactsExec,  item_flag_none},
	{0,0,0, (char*) TxtHold,         0,(MenuFunc)holdExec,      item_flag_none},
    {0,0,0, (char*) TxtMute,         0,(MenuFunc)muteExec,      item_flag_none},
//	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
//	new menu items added for all the four different profiles, only one of the 
//	menu item will be available at any given point fo time
#ifdef FF_MMI_AUDIO_PROFILE
    {0,0,0, (char*) TxtLoudspeaker,	 0,        (MenuFunc)M_exeLoudspeaker, item_loudspeaker_status },
    {0,0,0, (char*) TxtHandheld,	 0,        (MenuFunc)M_exeHandheld, item_handheld_status },
    {0,0,0, (char*) TxtHeadset,	0,	(MenuFunc)M_exeHeadset, item_headset_status},
    {0,0,0,(char*) TxtCarkit,	0,	(MenuFunc)M_exeCarkit, item_carkit_status},
#endif
    {0,0,0, (char*) TxtSendMessage,	 0,	(MenuFunc)M_exeSendNew,	item_flag_none },
    {0,0,0, (char*) TxtMessages,	 0,	(MenuFunc)M_exeRead,	item_flag_none },/*SPR2132*/
    {0,0,0, (char*) TxtSettings,	 0,	(MenuFunc)M_exeSetting,	item_flag_none }, //May 27, 2004    REF: CRR 19186  Deepa M.D .//Added Menu item for SMS Setting
    // Feb 13, 2006    REF:DR OMAPS00067943  x0039928
    // Fix : The Menu function M_exeViewfinder is renamed to M_exeApplication 
    // and moved out of FF_MMI_TEST_CAMERA flag.
    {0,0,0, (char*) TxtApplications,	 0,	(MenuFunc)M_exeApplication,	item_flag_none }//August 23, 2004    e-armanetsaid.//Added Menu item for Camera test purposes while call ongoing
};

static const MfwMnuAttr menuCallSingle =
{
    &menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE,               /* centered page menu       */
    (U8)-1,                                                /* use default font         */
    (T_MFW_MNU_ITEM*)menuCallSingleItems,                               /* with these items         */
    sizeof(menuCallSingleItems)/sizeof(T_MFW_MNU_ITEM), /* number of items          */
	COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE

};

//18/12/2001  MC added sms message option
static T_MFW_MNU_ITEM menuCallTwoItems[] =
{
	{0,0,0, (char*) TxtMultiparty,	0,(MenuFunc)startConferenceExec,	item_flag_none},
    {0,0,0, (char*) TxtContacts,        0,(MenuFunc)contactsExec,         item_flag_none},
    {0,0,0, (char*) TxtSwap,            0,(MenuFunc)swapExec,             item_flag_swap},
    {0,0,0, (char*) TxtEndAll,          0,(MenuFunc)endAllExec,           item_flag_none},
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Code which adds "End held" to the menu which appears when user presses Option soft key when there are atleast two calls
    // This menu item is displayed only when some call is in hold state
	{0,0,0, (char*) TxtEndHeld,          0,(MenuFunc)endHeldExec,           item_flag_endheld},	
    {0,0,0, (char*) TxtMute,            0,(MenuFunc)muteExec,             item_flag_none},
    {0,0,0, (char*) TxtHold,         0,(MenuFunc)holdExec,      item_flag_hold},//MC
//	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
//	new menu items added for all the four different profiles, only one of the 
//	menu item will be available at any given point fo time
#ifdef FF_MMI_AUDIO_PROFILE
    {0,0,0, (char*) TxtLoudspeaker,	 0,        (MenuFunc)M_exeLoudspeaker, item_loudspeaker_status },
    {0,0,0, (char*) TxtHandheld,	 0,        (MenuFunc)M_exeHandheld, item_handheld_status },
    {0,0,0, (char*) TxtHeadset,	0,	(MenuFunc)M_exeHeadset, item_headset_status},
    {0,0,0,(char*) TxtCarkit,	0,	(MenuFunc)M_exeCarkit, item_carkit_status},
#endif
    {0,0,0, (char*) TxtSendMessage,	 0,	(MenuFunc)M_exeSendNew,	item_flag_none },
    {0,0,0, (char*) TxtMessages,	 0,	(MenuFunc)M_exeRead,	item_flag_none },/*SPR2132*/
    {0,0,0, (char*) TxtSettings,	 0,	(MenuFunc)M_exeSetting,	item_flag_none }//May 27, 2004    REF: CRR 19186  Deepa M.D //Added Menu item for SMS Setting
};

static const MfwMnuAttr menuCallTwo =
{
    &menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    menuCallTwoItems,                /* with these items         */
    sizeof(menuCallTwoItems)/sizeof(T_MFW_MNU_ITEM), /* number of items     */
	COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE

};

//18/12/2001  MC added sms message option
static T_MFW_MNU_ITEM menuCallConferenceItems[] =
{
    {0,0,0, (char*) TxtPrivateCall,  0,(MenuFunc)privateStartAddConferenceExec,  item_flag_none},
    {0,0,0, (char*) TxtSwap,         0,(MenuFunc)swapExec,                       item_flag_swap},
    {0,0,0, (char*) TxtEndAll,       0,(MenuFunc)endAllExec,                     item_flag_none},
    {0,0,0, (char*) TxtEndConference, 0,(MenuFunc)endConferenceExec,               item_flag_none},
    {0,0,0, (char*) TxtContacts,     0,(MenuFunc)contactsExec,                   item_flag_none},
    {0,0,0, (char*) TxtHold,         0,(MenuFunc)holdExec,                        item_flag_hold},
    {0,0,0, (char*) TxtMute,         0,(MenuFunc)muteExec,                       item_flag_none},
//	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
//	new menu items added for all the four different profiles, only one of the 
//	menu item will be available at any given point fo time
#ifdef FF_MMI_AUDIO_PROFILE
    {0,0,0, (char*) TxtLoudspeaker,	 0,        (MenuFunc)M_exeLoudspeaker, item_loudspeaker_status },
    {0,0,0, (char*) TxtHandheld,	 0,        (MenuFunc)M_exeHandheld, item_handheld_status },
    {0,0,0, (char*) TxtHeadset,	0,	(MenuFunc)M_exeHeadset, item_headset_status},
    {0,0,0,(char*) TxtCarkit,	0,	(MenuFunc)M_exeCarkit, item_carkit_status},
#endif

    {0,0,0, (char*) TxtScratchPad,   0,(MenuFunc)scratchPadExec,                 item_flag_none},
    {0,0,0, (char*) TxtMainMenu,     0,(MenuFunc)mainMenuExec,                   item_flag_none},
    {0,0,0, (char*) TxtSendMessage,	 0,	(MenuFunc)M_exeSendNew,	item_flag_none },
    {0,0,0, (char*) TxtMessages,	 0,	(MenuFunc)M_exeRead,	item_flag_none },/*SPR2132*/
	{0,0,0, (char*) TxtSettings,	 0,	(MenuFunc)M_exeSetting,	item_flag_none }//May 27, 2004    REF: CRR 19186  Deepa M.D //Added Menu item for SMS Setting
};

static const MfwMnuAttr menuCallConference =
{
    &menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    (T_MFW_MNU_ITEM *)menuCallConferenceItems,                /* with these items         */
    sizeof(menuCallConferenceItems)/sizeof(T_MFW_MNU_ITEM), /* number of items     */
	COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE

};

#if(0)  /* x0039928 - Commented for lint warning removal */
static const T_MFW_MNU_ITEM menuDTMFItems [] =
{
    {0,0,0, (char*) TxtSendMessage,  0,(MenuFunc)exeSendMessage,    item_flag_none},
    {0,0,0, (char*) TxtMessages,	 0,	(MenuFunc)M_exeRead,	item_flag_none },/*SPR2132*/
    {0,0,0, (char*) TxtAddNew,       0,(MenuFunc)callExeNameEnter,  item_flag_none}
};

static const T_MFW_MNU_ATTR menuDTMF =
{
    &menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    (T_MFW_MNU_ITEM *)menuDTMFItems,                      /* with these items         */
    sizeof(menuDTMFItems)/sizeof(T_MFW_MNU_ITEM), /* number of items     */
	COLOUR_LIST_XX, TxtNull,NULL,  MNUATTRSPARE

};
#endif

enum {
	CALL_STATUS_CALL,
	CALL_TIMER_CALL,
	CALL_ID_NAME_CALL,
	CALL_ID_NUM_CALL
//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
,
CALL_ACTIVE_LINE
#endif
};

/*********************************************************************
**********************************************************************

          CALL WINDOW. DECLARATION

*********************************************************************
**********************************************************************/
//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
extern T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
extern T_MFW_HND	phlock_kbd_handle;			/* to hold the kbd handle of phone unlock editor window 	*/
extern T_MFW_HND	phlock_mtc_win_handle;		/* to hold the win handle of MT call window 			*/
#endif //FF_PHONE_LOCK

  T_call call_data;

//x0pleela 09 Mar, 2006  ER:OMAPS00067709
#ifdef FF_PCM_VM_VB
EXTERN T_voice_buffering voice_buffering_data;
#endif
void call(T_MFW_HND win, USHORT event, SHORT value, void * parameter);


static int call_win_cb(MfwEvt e, MfwWin *w);
static int call_kbd_cb (MfwEvt e, MfwKbd *k);
static int call_kbd_long_cb (MfwEvt e, MfwKbd *k);


/*********************************************************************
**********************************************************************

          MENU WINDOW. DECLARATION

*********************************************************************
**********************************************************************/

typedef struct
{
  T_MMI_CONTROL   mmi_control;
  T_MFW_HND       win;
  T_MFW_HND       kbd;
  T_MFW_HND       kbd_long;
  T_MFW_HND       menu;
  T_VOID_FUNC     func_cb;
  /*
   * internal data
   */
} T_call_menu;


/* Added to remove warning Aug-11*/
EXTERN void mfw_aud_l1_enable_vocoder ( void );
EXTERN void mfw_aud_l1_disable_vocoder ( void );
#ifdef FF_CPHS
           EXTERN T_MFW mfw_get_ALS_type(void);
#endif /* FF_CPHS */
EXTERN BOOL smsidle_get_unread_sms_available(void);
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
      EXTERN void mmi_em_start_eng_mode_menus(T_MFW_HND parent);
#endif /* (TI_PS_FF_EM) || (MMI_EM_ENABLED) */
void   cm_end_held(void);
EXTERN void info_screen (T_MFW_HND win, USHORT TextId, USHORT TextId2,T_VOID_FUNC  Callback);
EXTERN void dspl_show_bitmap(int x, int y,t_font_bitmap* current_bitmap,U32 attr );
EXTERN void sat_release_the_editor (void);
/* End - Added to remove warning Aug-11 */

static T_MFW_HND call_menu_create(MfwHnd parent);
static void call_menu_destroy(MfwHnd window);

T_MFW_HND call_menu_start(MfwHnd parent,MfwMnuAttr* menuAttr,T_VOID_FUNC func_cb);

void call_menu (T_MFW_HND win, USHORT event, SHORT value, void * parameter);


static int call_menu_win_cb (MfwEvt e, MfwWin *w);
static int call_menu_kbd_cb (MfwEvt e, MfwKbd *k);
static int call_menu_kbd_long_cb (MfwEvt e, MfwKbd *k);
static int call_menu_mnu_cb (MfwEvt e, MfwMnu *m);



/*********************************************************************
**********************************************************************

          CALL FUNCTIONS. DECLARATION

*********************************************************************
**********************************************************************/
/*SPR 1392, call deflection function prototypes*/
static int deflPhbExec(void); // RAVI
/* SPR#1428 - SH - change parameters slightly */
EXTERN void      cm_mt_deflect            (T_MFW_HND win, USHORT identifier, SHORT reason);
void call_deflection_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);//jgg
/*SPR 1392 end*/
void call_incoming_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void call_calling_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void call_ending_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void call_waiting_answer_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void call_sending_dtmf_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void call_menu_end_cb(void);
void call_failure_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void call_calling_show (USHORT Identifier);
void call_info_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
/* Marcus: Issue 1057: 21/01/2003: Added the following function prototype */
void call_acm_max_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);

/*********************************************************************
**********************************************************************

          DIALLING WINDOW. DECLARATION

*********************************************************************
**********************************************************************/



typedef struct
{
	T_MMI_CONTROL   mmi_control;
	T_MFW_HND win;     // window handle
	T_MFW_HND kbd;
	T_MFW_HND kbd_long;
#ifdef NEW_EDITOR
	T_ED_DATA	*editor;	/* SPR#1428 - SH - New Editor changes */
#else
	T_MFW_HND edt;
#endif
	UBYTE mode;
	UBYTE callstatus;
} T_call_scratchpad;




T_MFW_HND call_scratchpad_create(MfwHnd parent);
void call_scratchpad_exit (void);
static int call_scratchpad_kbd_cb (MfwEvt e, MfwKbd *k);
static int call_scratchpad_kbd_long_cb (MfwEvt e, MfwKbd *k);
static int call_scratchpad_win_cb (MfwEvt e, MfwWin *w);

void call_scratchpad (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
T_MFW_HND call_scratchpad_start (T_MFW_HND win_parent,char character,UBYTE mode, UBYTE callstatus);

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
int iIncomingCall = 0;   /* Variable for Incoming Call */
#endif 
#endif

//x0pleela 03 Nov, 2006  DR: OMAPS000101158
GLOBAL void mfw_set_OtherSrc_Value(UBYTE otherSrc);
GLOBAL UBYTE mfw_get_OtherSrc_Value(void);


/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

static E_IN_CALL_RINGER_STATUS iIncomingCallRingerStop = 0;

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End  */

/*******************************************************************************

                                Private Methods

*******************************************************************************/
//Daisy tang added for Real Resume feature 20071107 
//start
static void mp3_test_real_resume(void)
{
	IsRealPaused = FALSE;
	IsMP3Playing = TRUE;
	mfw_fm_audRealResume();				
}


static int mp3_test_play_Timer_cb( MfwEvt e, MfwTim *tc )
{
    if (mp3playTimer != NULL )
    {
        timStop(mp3playTimer);
        timDelete (mp3playTimer);
    }
    mp3_test_real_resume();
    return MFW_EVENT_CONSUMED;
}

//end
/*******************************************************************************

 $Function:     callInit

 $Description:  initiates the window

 $Returns:    none

 $Arguments:  parent window

*******************************************************************************/

void callInit (MfwHnd parent)
{

  TRACE_FUNCTION("callInit");

  cm_init();
  call_data.cm = cm_create(0, E_CM_ALL_SERVICES, (MfwCb)callCmEvent);
  call_data.win = 0;
  call_data.callStatus = CallInactive;

  call_data.vocoderState = CALL_VOCODER_IDLE;
}


/*******************************************************************************

 $Function:     callExit

 $Description:  Exits the window

 $Returns:    none

 $Arguments:  none

*******************************************************************************/


void callExit (void)
{

  TRACE_FUNCTION("callExit");
    cm_delete(call_data.win);
    timDelete(call_data.timer);
    cm_exit();
}




/*******************************************************************************

 $Function:     callAutoRedialKeyEvent

 $Description:  keyboard event handler

 $Returns:    none

 $Arguments:  typical

*******************************************************************************/


void callAutoRedialKeyEvent(T_MFW_HND win, USHORT identifier, USHORT reason)
{

  TRACE_FUNCTION("callAutoRedialKeyEvent");

	call_data.win_redial = 0;
	switch (reason)
	{
		case INFO_KCD_LEFT:
		/*
			callSetMode(ViewAutoRedial);
			cm_redial();
		*/
		break;
		case INFO_KCD_HUP:
		case INFO_KCD_RIGHT:
      case INFO_KCD_CLEAR:
      cm_redial_abort();
      callSetMode(ViewConnect, TRUE);
    break;
    default:
    break;
    }
}



/*******************************************************************************

 $Function:     callCmEvent


 $Description:  call management event handler


 $Returns:    1, processed, 0 ignored


 $Arguments:  event and parameter


*******************************************************************************/



int callCmEvent(MfwEvt e, void *para)
{

/* a0393213 (WR - "currentRinger" was set but never used) */
#ifndef FF_MIDI_RINGER 
#ifndef FF_MMI_RINGTONE
  UBYTE currentRinger;
#endif
#endif

  int i;
  SHORT       *cid;
  T_MFW_CM_DISCONNECT *dc;
  T_MFW_CM_COMMAND  *comm;
//  SHORT       callId;   // RAVI
  T_MFW_CM_REDIAL     *pRedial;
  T_MFW_CM_CPI        *cmCpi;
  /* SPR#1352 - SH - TTY */
  #ifdef MMI_TTY_ENABLED
  T_MFW_CM_CTYI			*cmCTYI;
  #endif
  /* end SH */

 //  June 08, 2004 REF: CRR MMI-SPR-13876 xkundadu(Sasken)
//  Added this variable to check whether there is a waiting call
SHORT waiting_callnumber;

//  MMI-SPR 13847 xkundadu
//  heldCall_Id gets the index of a HELD call.
  SHORT heldCall_Id;

//  Aug 24, 2004 REF: CRR MMI-SPR-15750 xkundadu 
// activeCall_Id gets the index of a ACTIVE call. 
  SHORT activeCall_Id;

//x0pleela 09 Feb, 2007  DR: OMAPS00114834
//This definition is required to get the current ringer settings
#ifndef FF_MIDI_RINGER 
  UBYTE currentRinger;
#endif

#ifndef FF_MIDI_RINGER 
  currentRinger = getCurrentRingerSettings();
#endif

	TRACE_EVENT_P2("%s %d","callCmEvent  ",e);

    switch(e)
    {

      /*
       * Handle call notifications
       */
      case E_CM_NOTIFY:
      	TRACE_EVENT("E_CM_NOTIFY");
        SEND_EVENT (call_data.win, E_CM_NOTIFY, 0, para);
        break;

	  case E_CM_COLP:
	      	TRACE_EVENT("E_CM_COLP");
        	SEND_EVENT (call_data.win, E_CM_COLP,0, para);
        break;

  case E_CM_CW:
//Apr 06, 2005	REF: ENH 30011 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
#ifdef RVM_CAMA_SWE
#ifdef RVM_CAMD_SWE
    //e-armanetsaid 18/08/04 -enable R2D back when incoming call -might have been disabled by camera test module
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//replaced the flag with the function call, to migrate to the new Lcd APIs
	if(dspl_control(DSPL_ACCESS_QUERY) == DSPL_ACCESS_DISABLE)
    {
//Sept 15, 2005 REF: LOCOSTO-ENH-34257 - xpradipg
//replaced the flag with the function call, to migrate to the new Lcd APIs  
	dspl_control(DSPL_ACCESS_ENABLE);
	mfw_cam_disable_sensor();
     }
    //e-armanetsaid 18/08/04 -enable R2D back when incoming call -might have been disabled by camera test module
#endif
#endif
#endif //FF_MMI_TEST_CAMERA

    /* Set the Incoming Call Variable */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
    iIncomingCall = 1;
#endif
#endif

    if(para)
        incomingCall((T_MFW_CM_CW_INFO*)para);
// Apr 05, 2005    REF: ENH 29994 xdeepadh				
//Commented mmesetvolume.since the speaker volume will be set, 
//once the call is acknowledged.
//  mmeSetVolume (1, FFS_flashData.output_volume);

    break;

  case E_CM_RING_OFF:
  	TRACE_EVENT("E_CM_RING_OFF");
//Below original code is commented by x0pleela 09 Feb, 2007 DR: OMAPS00114834
    // x0021334 15 May, 2006 DR: OMAPS00075901
    // This event comes before incoming call is accepted . Stop call waiting tone.
   // audio_StopSoundbyID (AUDIO_MICROPHONE, TONES_CW);

//x0pleela 15 Feb, 2007  DR: OMAPS00114834
/*	Above original code is commented and the following condition is added to stop 
	the ringer when MMI recieves the RING_OFF event from ACI rather than stopping 
	the ringer when user accepts or rejects the call using keypad
*/

	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */
	
	/* Set the Incoming Call Ringer Status as "Stopped" */
	bmi_incomingcall_set_ringer_status(BMI_INCOMING_CALL_NONE);
	
	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End */

	if (LocalRing)
      {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
        audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
        LocalRing = FALSE;

	  	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

		/* Vocoder Enable */
		mfw_aud_l1_enable_vocoder();
	  
	  	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End */
      }
	else
     	{     	
  	// Checking whether call is present or not
		if(call_data.calls.numCalls)
	   	{   		
	   	  	// Checking whether the call is mobile terminated.
 		      if(call_data.call_direction EQ  MFW_CM_MTC)
	 		{
	 			// The call is incoming call, So stop the call waiting tone.
				audio_StopSoundbyID( AUDIO_MICROPHONE, TONES_CW);

				/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

				/* Vocoder Enable */
				mfw_aud_l1_enable_vocoder();
				
				/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End */
				
	 		}
		   	
	   	}
		else
		{
// stop this ringing tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);

	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

	/* Set the Incoming Call Ringer Status as "Stopped" */
	bmi_incomingcall_set_ringer_status(BMI_INCOMING_CALL_RINGERSTOPPED);

	/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End */

#else
#ifdef FF_MMI_RINGTONE  /* Stop MSL */
                     audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT);
#else
			// This is to stop the buzzer  
			audio_StopSoundbyID( AUDIO_BUZZER, getCurrentRingerSettings() );
#endif
#endif
		}
     	}	
    break;

  case E_CM_DISCONNECT_ALL:
	TRACE_EVENT(" E_CM_DISCONNECT_ALL : Call ended  remove All Calls!");
	
	call_show_ending_all();

	removeAllCalls();

	if (call_data.win)
		call_destroy(call_data.win);
	
// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is unregistered if the calls are disconnected.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif

	mfw_hook_unregister();


#endif
	
	//xvilliva SPR13698, After receiving the "E_CM_DISCONNECT_ALL"
	//we initiate the MO ECC. We reset the flag and then initiate the call.
	if(makeECC == TRUE && strlen(strECC)>0)
	{
		makeECC = FALSE;
		cm_mo_call((UBYTE *)strECC, VOICE_CALL);	
	}

//x0pleela 24 Mar, 2006  ER:OMAPS00067709
//stop voice recording 
#ifdef FF_PCM_VM_VB
  	  if( (get_voice_memo_type() EQ VOICE_BUFFERING) AND 
  	  	(!voice_buffering_data.incoming_call_discon) )
  	  { 	    
  	    if(voice_buffering_data.call_active AND (!voice_buffering_data.play_stopped ))
  	      voice_buffering_stop_playing(); 
	    if( !voice_buffering_data.recording_possible) 
		voice_buffering_stop_recording();
  	  }
#endif
	//Daisy tang added for Real Resume feature 20071107	
	//start
	if(IsRealPaused)
	 	timStart( mp3playTimer );

	//end
	break;

  case E_CM_DISCONNECT:
      TRACE_EVENT("E_CM_DISCONNECT");
//EF	assign dc now 29/01/2003
      dc = (T_MFW_CM_DISCONNECT *)para;

      if (strlen((char*)call_data.colp_number) > 0)
	    memset(call_data.colp_number,'\0',sizeof(call_data.colp_number));

	/* SPR 1392, work out whether we need to remove the call deflection windows*/
	/*if the call to be disconnected is the current incoming call*/
	 	if ((dc->call_number==call_data.incCall))
		{	/*if the deflection editor exists*/
			if (call_data.win_deflectEdit)
			{ 	
				// Jan 05, 2006    REF:DR OMAPS00050454  x0039928
				// Delete the calling window also
				deflect_destroy(TRUE);
	 		}
		}

#ifdef FF_WAP
	  /* SPR#1983 - SH - Inform WAP of a disconnection */
	  AUI_wap_call_disconnect(dc->call_number);
#endif

	/* MZ cq11080 25-07-03  Check to see if call is muted then Unmute.*/
	if (call_data.calls.muted)
	{
       	if (audio_SetMute(AUDIO_MICROPHONE, AUDIO_MUTING_OFF) == DRV_OK)
        	{
            		call_data.calls.muted = false;
            		menuCallSingleItems[2].str = (char*)TxtMute;
        	}
    	}

      if (call_data.ignore_disconnect)
      	{
 	//x0pleela 15 Mar, 2006  ER:OMAPS00067709
	//stop voice recording 
#ifdef FF_PCM_VM_VB
	if( get_voice_memo_type() EQ VOICE_BUFFERING )
  	{ 	    
  	    if(voice_buffering_data.call_active AND (!voice_buffering_data.play_stopped ))
  	      voice_buffering_stop_playing(); 
	    if( !voice_buffering_data.recording_possible) 
		voice_buffering_stop_recording();
  	}
#endif
	//Daisy tang added for Real Resume feature 20071107	
	//start
	if(IsRealPaused)
	 	timStart( mp3playTimer );

	//end
       	return 1;
      	}


      if (LocalRing)
      {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
        audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
        LocalRing = FALSE;
      }
 
	  	
     
      TRACE_EVENT("Disconnect Event");

      cm_get_aoc_value(CM_AOC_CCM,&call_data.aocInfo);

      if (!dc->call_number)       // Delete multiparty
      {

// 	Oct 6, 2004    REF: CRR 23948 xkundadu
//	Description: FT-Brazil-TC 9.2 multiparty using BPM -After multiparty
//	'call ended', mmi doesnot  return to idle,but indicates that a call is still..
//	Fix:	Call the removeCall() function for all the calls in the multiparty
//		in the case of multiparty disconnection. 

			// Temporary  variable to store the call table entries.
/* Dec 22, 2006 REF:OMAPS00107042  x0039928 */			
#ifdef FF_MMI_PB_OPTIM
                     T_MFW_CM_STATUS *currentstatus = NULL;
#else
			T_MFW_CM_STATUS currentstatus[MAX_CALLS] = {-1};
#endif			
			UBYTE noOfCalls = 0;//Number of calls 
			int j;
			BOOL bCallEndedShown = FALSE;//To check 'Call ended' screen displayed or not
		       
#ifdef FF_MMI_PB_OPTIM
                     currentstatus = (T_MFW_CM_STATUS *)ALLOC_MEMORY(MAX_CALLS * sizeof(T_MFW_CM_STATUS));
#endif
			noOfCalls = call_data.calls.numCalls;

			// Storing the call table to a temperory table.
			for( j=0;j<call_data.calls.numCalls;j++)
			{
				currentstatus[j].call_number = call_data.calls.status[j].call_number;
				currentstatus[j].stat = call_data.calls.status[j].stat;
				currentstatus[j].mtpy = call_data.calls.status[j].mtpy;
			}

			// Check whether the call in the call table is multiparty member or not.
			// If it is multiparty member, remove the call table entry for that.
			for( j=0;j<noOfCalls;j++)
			{
				if(currentstatus[j].mtpy == MFW_MTPY_MEMBER)
				{
					 call_data.accessEnd = TRUE;
					 // Avoid display of 'Call ended' screen multiple times.
					 if(bCallEndedShown == FALSE)
					 {
						bCallEndedShown = TRUE;
						//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
						removeCall(currentstatus[j].call_number, TRUE,FALSE);
					 }
					 else
					 {
						//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
						removeCall(currentstatus[j].call_number, FALSE,FALSE);
					 }
					 	
				}
			}
/* Dec 22, 2006 REF:OMAPS00107042  x0039928 */				
#ifdef FF_MMI_PB_OPTIM
    if(currentstatus != NULL)
		{	FREE_MEMORY((UBYTE*)currentstatus, MAX_CALLS * sizeof(T_MFW_CM_STATUS));
		       currentstatus = NULL;
		}
#endif
      }

else
{
      if (callIndex(dc->call_number)<MAX_CALLS)
      	{
		//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
		removeCall(dc->call_number, TRUE,FALSE);  // Delete active call

//  June 08, 2004 REF: CRR MMI-SPR-13876 xkundadu(Sasken)
//  Issue description: CHW : AUDIO : No difference between CW and incoming call
//  If there is one Active call and one wiating call and if the Active call is 
//  disconnected  from the remote side/network side, the waiting tone should 
//  change to ringing tone.
//  Solution: If there is a waiting call and all other calls has been disconnected,
//		     change the waiting tone to ringing tone.

		// Check whether a waiting call is exisiting 
		if (cm_search_callId(CAL_STAT_Wait, &waiting_callnumber) EQ CM_OK)
		{
		// Checking whether there is no other calls
		   if(!call_data.calls.numCalls)
			{
				// If there is no other calls, stop the waiting tone and start ringing tone.
				audio_StopSoundbyID( AUDIO_MICROPHONE, TONES_CW);
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_IC,true,sounds_midi_ringer_start_cb);
#else
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_RINGTONE /* MSL Ring Tone */
                           audio_PlaySoundID( AUDIO_BUZZER, CALLTONE_SELECT, 0 , AUDIO_PLAY_INFINITE);
#else
       audio_PlaySoundID( AUDIO_BUZZER, getCurrentRingerSettings(), 
                                         getCurrentVoulmeSettings(), AUDIO_PLAY_INFINITE);
#endif
#else
	audio_PlaySoundID( AUDIO_BUZZER, getCurrentRingerSettings(), 0 , AUDIO_PLAY_INFINITE);
#endif
/* END RAVI */
#endif
			}
		}
      	}
      else                // Delete not executed call
      {
        if ((dc->call_number==call_data.incCall))
        {
          if (call_data.win_incoming)
          {

		//  Jun 21, 2004 REF: CRR MMI-SPR-17981 xkundadu
		//  Issue description: RE: S60/Symbian: ACI reports RING messages after an 
		//  incoming call was closed using at+CHLD=1x or ATH
		//  Solution: If E_CM_DISCONNECT event comes and if incoming call alert is there
		//                stop the ringing tone.

// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE /* Stop MSL */
            audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT);
#else
	     audio_StopSoundbyID( AUDIO_BUZZER, getCurrentRingerSettings());
#endif
 #endif
            TRACE_EVENT("Dialog Destroy No 1");
            if(idwithheld == TRUE)
      			idCounter = idCounter + 1;

            SEND_EVENT(call_data.win_incoming,DIALOG_DESTROY,0,0);
              call_data.win_incoming = 0;
          }
          TRACE_EVENT_P2("GET_CAUSE_VALUE(dc->cause %x) EQ %x", dc->cause, GET_CAUSE_VALUE(dc->cause));
		  
          if (GET_CAUSE_VALUE (dc->cause)!=NOT_PRESENT_8BIT)
            {
              //Calling side has terminated the call, turn of ringer MZ.
	  	if(call_data.calls.numCalls)
	   	{
	   	   TRACE_EVENT("NDH >>> Stopping TONES_CW in the Microphone");
		  audio_StopSoundbyID( AUDIO_MICROPHONE, TONES_CW);
		}
		else
	  	{
	   	   	TRACE_EVENT("NDH >>> Stopping current Rigner on the Buzzer");
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
			// Stopping the current ring tone.
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
			/* x0045876, 14-Aug-2006 (WR - "currentRinger" was set but never used) */
              	/* currentRinger = getCurrentRingerSettings(); */

#ifdef FF_MMI_RINGTONE /*Stop MSL */
                     audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT);
#else
			/* x0045876, 14-Aug-2006 (WR - "currentRinger" was set but never used) */
              	currentRinger = getCurrentRingerSettings();

              	audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif
	  	}
              //Check if incoming call has been rejected by user, call is not regarded as a missed call. MZ
#ifdef FF_2TO1_PS
              if(dc->cause != M_CC_CAUSE_USER_BUSY ) /*SPR#2217 - DS - Changed MNCC_CAUSE_USER_BUSY to CAUSE_USER_BUSY */
#else
              if(dc->cause != M_CC_CAUSE_USER_BUSY )
#endif
                {
                  TRACE_EVENT("Add missed call");
                  if (call_data.globalCWInfo.type EQ VOICE_CALL)
                  addMissedCall();
                }


            }
            call_data.incCall = 0;
            if (!call_data.calls.numCalls)
              if (call_data.win)
              {
                call_destroy(call_data.win);
                /* SPR#1985 - SH - Revert to initial TTY setting
		      	 * Do not access dc-> after this call, as it will generate
		      	 * an event here which will overwrite the previous data */
#ifdef MMI_TTY_ENABLED
				call_tty_revert();
#endif

//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//stop voice recording 
#ifdef FF_PCM_VM_VB
  	  if( (get_voice_memo_type() EQ VOICE_BUFFERING) AND 
  	  	(!voice_buffering_data.incoming_call_discon) )
  	  { 	    
  	    if(voice_buffering_data.call_active AND (!voice_buffering_data.play_stopped ))
  	      voice_buffering_stop_playing(); 
	    if( !voice_buffering_data.recording_possible) 
		voice_buffering_stop_recording();
		voice_buffering_data.call_active = FALSE;
		vocoder_mute_ul(0);
  	  }

#endif


	//Daisy tang added for Real Resume feature 20071107	
	//start
	if(IsRealPaused)
	 	timStart( mp3playTimer );
	//end
                return 1;
              }
        }
        if ((dc->call_number==call_data.outCall))
        {
          if (call_data.win_calling){
            TRACE_EVENT("Dialog Destroy No 2");
            SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
            call_data.win_calling = 0;
          }
          call_data.outCall = 0;
          callFailureCause(dc->cause);
        }
      }
  }
      if (call_data.callsToEnd)
      {
        call_data.callsToEnd=call_data.callsToEnd-1;
        cm_disconnect(call_data.calls.status[0].call_number);
      }

     	/* SPR#1985 - SH - Revert to initial TTY setting
      	 * Do not access dc-> after this call, as it will generate
      	 * an event here which will overwrite the previous data */
#ifdef MMI_TTY_ENABLED
		call_tty_revert();
#endif

		//GW-SPR#1035 - Added SAT changed
			/*NM, 110702
			 in case that the SAT initiate the setup call (open a new editor);
			 so we want to make sure that the editor disappear after disconnect
			*/
			sat_release_the_editor ();
			/*NM, 110702 END*/

		rebuildCalls(); // Marcus: Issue 1795: 12/03/2003

		// MMI-SPR 13847 xkundadu
		// Solution: If there is only one call and if that call is HELD, 
		// change its status to ACTIVE.
		
		// Checking whether the disconnection indication is initiated from host
		if(bhostDisconnection == TRUE)
		{
			bhostDisconnection = FALSE;

			// Aug 24, 2004 REF: CRR MMI-SPR-15750 xkundadu
			// Bug:"END" function is not implemented for mutiple call
			// Solution: In the case of multiparty HELD call or single HELD call
			//               change its state to ACTIVE.
			
			if(call_data.calls.numCalls)  
			{
 				//If HELD call is there and no ACTIVE call, change the HELD
 				//call to ACTIVE.
				if((cm_search_callId(CAL_STAT_Held, &heldCall_Id) == CM_OK) AND
				    (cm_search_callId(CAL_STAT_Active, &activeCall_Id) != CM_OK))
				{
					// Change the HELD call to ACTIVE call
					cm_command(heldCall_Id, CM_RETRIEVE, '\0');
		        		call_data.current_command = CM_RETRIEVE;
				}
			}
		}
//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//stop voice recording 
#ifdef FF_PCM_VM_VB
  	  if( (get_voice_memo_type() EQ VOICE_BUFFERING) AND 
  	  	(!voice_buffering_data.incoming_call_discon) )
  	  { 	    
  	    if(voice_buffering_data.call_active AND (!voice_buffering_data.play_stopped ))
      		voice_buffering_stop_playing(); 
		if( !voice_buffering_data.recording_possible) 
		voice_buffering_stop_recording();
  	       voice_buffering_data.call_active = FALSE;
		vocoder_mute_ul(0);
 	  }

#endif

// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is unregistered for the last call.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928

#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif
		if(call_data.calls.numCalls == 0)  
			mfw_hook_unregister();
#endif
	//Daisy tang added for Real Resume feature 20071107	
	//start
	if(IsRealPaused)
	 	timStart( mp3playTimer );
	//end
        break;

        case E_CM_CONNECT_ACK:
      TRACE_EVENT("E_CM_CONNECT_ACK");
      cid = (SHORT*)para;
      TRACE_EVENT_P5("CId %d . outCall %d . win_calling %d . incCall %d win_incoming %d",
      *cid,call_data.outCall,call_data.win_calling,call_data.incCall,
      call_data.win_incoming);

// Jan 19, 2005 REF: CRR MMI-SPR-27455 xnkulkar
// Description: TC 29.3.2.6.7 failed on 900 on R&S
// Solution: function "mfw_aud_l1_enable_vocoder ()" should be called only when the 
// call mode is not DATA or FAX 
	if (cm_search_callMode(*cid) != CAL_MODE_Data AND cm_search_callMode(*cid) != CAL_MODE_Fax)
	{
		if (call_data.vocoderState == CALL_VOCODER_DISABLED)
		{
			/*
			** If the Vocoder is disabled, now is the time to re-enable it.
			*/
			TRACE_EVENT("BMI E_CM_CONNECT_ACK: enable vocoder");
			mfw_aud_l1_enable_vocoder ();			
		}

		call_data.vocoderState = CALL_VOCODER_INCALL;
	}
	else
	{
		TRACE_EVENT("BMI E_CM_CONNECT_ACK: not voice call");
	}

      if (LocalRing)
      {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
        audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
        LocalRing = FALSE;
      }
	else
     	{
     	
//  Jun 08 2004 REF: CRR MMI-SPR-13877 Ajith K P
//  Issue description: CHW : AUDIO : CW tone not stopped
//  Solution: When MMI receives the event E_CM_CONNECT_ACK,  if number of
//  calls is greater than 1 and if the call is MTC, Stop the waiting call beep, otherwise stop the,
//  normal ringing tone.

	  	// Checking whether call is present or not
		if(call_data.calls.numCalls)
	   	{
   		
	   	  	// Checking whether the call is mobile terminated.
 		      if(call_data.call_direction EQ  MFW_CM_MTC)
	 		{
	 			// The call is incoming call, So stop the call waiting tone.
				audio_StopSoundbyID( AUDIO_MICROPHONE, TONES_CW);
	 		}
		   	
	   	}
		else
		{
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Since there is no other call the ringing tone will be Buzzer,
// So stop this ringing tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /* Stop MSL */
                     audio_StopSoundbyID( AUDIO_BUZZER, CALLTONE_SELECT);
#else
			// This is to stop the buzzer  
			audio_StopSoundbyID( AUDIO_BUZZER, getCurrentRingerSettings() );
#endif
#endif
		}
     	}



			if ((*cid==call_data.incCall)&&(call_data.win_incoming))
			{
					SEND_EVENT(call_data.win_incoming,DIALOG_DESTROY,0,0);
					call_data.win_incoming = 0;
			}

			if ((*cid==call_data.outCall)&&(call_data.win_calling)){
					SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
					call_data.win_calling = 0;
			}
			// Aug 17, 2004        REF: CRR 6401  xvilliva
			// Recitified 'for' loop, Changed from MAX_CALL_NR to MAX_CALLS 
			// as it is going out of bounds of call_data.calls.status[i] 
			// which is of size MAX_CALLS only.
			//Quick check to see if we're duplicating call table
			for (i =0; i<MAX_CALLS; i++)
			{	
				if (*cid == call_data.calls.status[i].call_number)
				{
					return 1; /*a0393213 lint warning removal - 1 returned*/
				}
			}

			call_data.outCall = 0;
			call_data.incCall = 0;
          callConnect(*cid);
	//xrashmic 21 Aug, 2005 OMAPS00090198
	//The mmeSetVolume is skippen when in a TTY call
	#ifdef MMI_TTY_ENABLED
	if( call_data.tty == CALL_TTY_OFF )
    #endif		
	{
		mmeSetVolume (1, FFS_flashData.output_volume);
	}
#ifdef MMI_TTY_ENABLED
	else
	{
		audio_set_tty(3);
	}
   #endif	
      //x0pleela 09 Mar, 2006   ER:OMAPS00067709
      //Playing the recorded voice for PTT
#ifdef FF_PCM_VM_VB
        if(  get_voice_memo_type() EQ VOICE_BUFFERING )
        {
           voice_buffering_data.call_active = TRUE;
           voice_buffering_playback_start(); 
        }
#endif
        break;
        case E_CM_CONNECT:
      TRACE_EVENT("E_CM_CONNECT !!");

      cid = (SHORT*)para;
      if (LocalRing)
      {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
        audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
        LocalRing = FALSE;
      }

      if ((*cid==call_data.incCall)&&(call_data.win_incoming)){
          SEND_EVENT(call_data.win_incoming,DIALOG_DESTROY,0,0);
          call_data.win_incoming = 0;
      }
      if ((*cid==call_data.outCall)&&(call_data.win_calling)){
          SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
          call_data.win_calling = 0;
      }
      call_data.outCall = 0;
      call_data.incCall = 0;
          callConnect(*cid);
	//xrashmic 21 Aug, 2005 OMAPS00090198
	//The mmeSetVolume is skippen when in a TTY call
	#ifdef MMI_TTY_ENABLED
	if( call_data.tty == CALL_TTY_OFF )
	#endif
	{
		mmeSetVolume (1, FFS_flashData.output_volume);
	}

    break;
        case E_CM_WAP_CALL_CONNECTED:
    break;

        case E_CM_COMMAND:
            comm = (T_MFW_CM_COMMAND*)para;
        if (comm->result == Present) {
        TRACE_EVENT_P2("comm.result = %d comm.command = %d",comm->result,comm->command);
         switch (comm->command) {

                    case CM_RETRIEVE:
                        TRACE_EVENT("call CM_RETRIEVED received");
          break;
                    case CM_HOLD:
                        TRACE_EVENT("call CM_HOLD received");
            break;
          case CM_BUILD_MULTIPARTY:
            TRACE_EVENT("call CM_BUILDMULTIPARTY received");
            call_data.calls.mode = CallConference;
          break;
          case CM_SPLIT_MULTIPARTY:
            TRACE_EVENT("call CM_SPLIT_MULTIPARTY received");
            call_data.singleCall = comm->call_number;
            call_data.calls.mode = CallSingleMultiparty;
          break;
          case CM_HOLD_MULTIPARTY:
            TRACE_EVENT("call CM_HOLD_MULTIPARTY received");
			//Mar 26,2008, OMAPS00164785, x0091220(Daniel)
			if(call_data.calls.numCalls>1)
			{
				call_data.calls.mode = CallConference;
			}
			else
			{
				call_data.calls.mode = CallSingle;
			}
          break;
          case CM_RETRIEVE_MULTIPARTY:
              TRACE_EVENT("call CM_RETRIEVE_MULTIPARTY received");
			  
		// Aug 24, 2004 REF: CRR MMI-SPR-15750 xkundadu
 		//  If number of calls greater than one, set the mode to CallConference.
 		

              if(call_data.calls.numCalls >1)
	       {
        		call_data.calls.mode = CallConference;

	       }
   
	
          break;
          case CM_SWAP:
            TRACE_EVENT("call CM_SWAP received");
          break;
          case CM_SWAP_MULTIPARTY:
            TRACE_EVENT("call CM_SWAP MULTIPARTYreceived");
            if (call_data.calls.mode == CallSingleMultiparty)
              call_data.calls.mode = CallMultipartySingle;
            else
              call_data.calls.mode = CallSingleMultiparty;
          break;
          case CM_ETC:
            TRACE_EVENT("call CM_ETC received");
            i=0;
            while (i<call_data.calls.numCalls)
            {
              if (call_data.calls.status[i].call_number!=singleCall)
              {
			//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
                if (!removeCall(call_data.calls.status[i].call_number, TRUE,FALSE))
                i++;
              }
              else
                i++;
            }
          break;
          case CM_DTMF_AUTO:
          break;
          default:
          break;
                }
        rebuildCalls();

				if (call_data.win_waiting)
				{
					SEND_EVENT(call_data.win_waiting,DIALOG_DESTROY,0,0);
					call_data.win_waiting = 0;
					// Feb 03, 200   REF: CRR 25960   x0012850
                                   // Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible
                                   // Solution: Call cm_setSpeechMode() to attach user connection again.
					cm_setSpeechMode();
					TRACE_EVENT("destroy call win h");
				}
				if (call_data.win_calling)
				{
					SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
					call_data.win_calling = 0;
					TRACE_EVENT("destroy call win i");
				}

            }
			if (call_data.accessEnd)
			{
             callSetMode(ViewConnect, TRUE);
            }

	/* indicates if a command has failed e.g. ECT MC 12/04/02*/
	if (comm->result == NotPresent)
	{	
	      //06 July, 2007 DR: OMAPS00137334 x0062172 Syed
		if(call_data.calls.numCalls >= MAX_CALLS)
		{
		bookShowInformation(call_data.win, TxtConferenceFull,NULL, NULL); 
		}
		else
		{
		bookShowInformation(call_data.win, TxtFailed,NULL,  NULL); /*a0393213 lint warning removal*/
		}
		
		if (call_data.win_waiting) /*If we're waiting for current command*/
			{	/*Get rid of waiting window*/
				SEND_EVENT(call_data.win_waiting,DIALOG_DESTROY,0,0);
				call_data.win_waiting = 0;
				TRACE_FUNCTION("destroy call win h");
			}
//TISH, patch for OMAPS00129157
//start
				if (call_data.win_calling && comm->command==CM_HOLD)
				{
					SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
					call_data.win_calling = 0;
					TRACE_EVENT("destroy call win i");
				}
//end

	}
      break;
    case E_CM_DTMF:
      if (call_data.sendingDTMF)
      {
          call_data.charSent++;
          sendDTMFString();
      }
    break;
    case E_CM_REDIAL:
      TRACE_EVENT("E_CM_REDIAL");
          pRedial = (T_MFW_CM_REDIAL*)para;
      if(pRedial == NULL) /*a0393213 lint warnings removal - check for null added*/
      {
      		TRACE_ERROR("callCmEvent():E_CM_REDIAL null parameter");
      }
      else
      {	  	
		switch(pRedial->redial_mode)
		{

		case CM_REDIAL_BLACKLIST:
			TRACE_EVENT("CM_REDIAL_BLACKLIST");
			TRACE_EVENT("the blacklist is now full");
			/*
			GSM 2.07
			when the blacklist is full the MT
			shall prohibit further automatic call
			attempts to any one number until
			the blacklist is manually cleared at the MT

			The blacklist is cleared after switch off/on ?!
			*/
			break;
		case CM_REDIAL_NO_SERVICE:
			TRACE_EVENT("CM_REDIAL_NO_SERVICE");
			break;
		case CM_REDIAL_OUTGOING:
			TRACE_EVENT("CM_REDIAL_OUTGOING");
		case CM_REDIAL_INCOMING:
			TRACE_EVENT("CM_REDIAL_INCOMING");
		case CM_REDIAL_ABORTED:
			TRACE_EVENT("CM_REDIAL_ABORTED");
		case CM_REDIAL_BLACKLISTED :
			TRACE_EVENT("CM_REDIAL_BLACKLISTED");

			if (call_data.win_redial)
			{
			SEND_EVENT(call_data.win_redial,DIALOG_DESTROY,0,0);
			call_data.win_redial = 0;
			TRACE_EVENT("destroy redial win ");
			}

			break;
		case CM_REDIAL_SUCCESS:
			TRACE_EVENT("CM_REDIAL_SUCCESS");

			if (call_data.win_redial)
			{
			SEND_EVENT(call_data.win_redial,DIALOG_DESTROY,0,0);
			call_data.win_redial = 0;
			TRACE_EVENT("destroy redial win ");
			}
			rebuildCalls();
			break;

		case CM_REDIAL_STARTED:
			TRACE_EVENT("CM_REDIAL_STARTED");
			/*  */




			if (call_data.win_calling)
			{
			SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
			call_data.win_calling = 0;
			TRACE_EVENT("destroy call call_win");
			}

			if (pRedial->left_attempts EQ 0 )		// Not yet started redial attempts
			{
			/* start the 1st repeat attempt */
			cm_autoredial_start();
			}
			callSetMode(ViewAutoRedial, TRUE);
			break;

		case CM_REDIAL_STOP:
			TRACE_EVENT("E_CM_REDIAL_STOP");
			if (call_data.win_redial)
			{
			SEND_EVENT(call_data.win_redial,DIALOG_DESTROY,0,0);
			call_data.win_redial = 0;
			}
			rebuildCalls();
			break;
		case CM_REDIAL_ATTEMPT:
			TRACE_EVENT("E_CM_REDIAL ATTEMPT");
			break;
		default:
			break;
		}
      	}
        break;
    case E_CM_MO_RES:
    {
      T_MFW_PHB_ENTRY phb_entry;
// Sep 01, 2006 REF:OMAPS00090555  x0039928
// Fix : Get the outgoing call data/number from the parameter passed to callCmEvent
      T_MFW_CM_MO_INFO *outgoing_call_data	= (T_MFW_CM_MO_INFO*) para;
      T_MFW_CM_AOC_INFO   aoc_info;
      char * v_Number = NULL;
      cid = &(outgoing_call_data->call_id);

      // this is used for the "last charge"
      cm_get_aoc_value(CM_AOC_ACM, &aoc_info);

      acm_last_call = aoc_info.acm;

      TRACE_EVENT ("E_CM_MO_RES");

      TRACE_EVENT_P1("Call Id %d",*cid);

      /* If we have no active call window, create one */
      if (!call_data.win)
      {
        call_create(0);
      }

      if (call_data.calls.numCalls == 0)
      {
        SEND_EVENT(call_data.win,CALL_INIT,0,0);
      }

      /* end any locally generated tones, this is for late assignment
       * and should be managed within ACI
       */
      if (LocalRing)
      {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
        audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
	 LocalRing = FALSE;
      }

    /* SPR#1983 - SH - Notify WAP if a data call is set up */

#ifdef FF_WAP
	{
	    T_MFW_CM_CALL_STAT	stat;
	    UBYTE		ton;
	    UBYTE		mode;

		cm_search_callStat(*cid, &stat, &ton, &mode);

	    if (mode == DATA_CALL)
	    {
		  AUI_wap_call_connect(*cid);
		}
	}
#endif

        /* set the outcall ID */
        call_data.outCall = *cid;
        call_data.call_direction = MFW_CM_MOC;

        /* set local pointer to number from outgoing call data */
// Sep 01, 2006 REF:OMAPS00090555  x0039928		
        v_Number = (char*)(outgoing_call_data->number);

        TRACE_EVENT_P1("(char*)outgoing_call_data.number: %s", outgoing_call_data->number);

        /* Check whether the dialled number appears in the phonebook and set the
         * status information if it does
         */
        if (cm_check_emergency((UBYTE*)v_Number) == TRUE)
        {
            call_data.emergencyCall = TRUE;
        }

        /* SPR#1700 - DS - If call is a SAT call and the SIM has supplied an alpha identifier then
         * do not overwrite with name/number. Label already setup for SAT calls in function call().
         */
         if (call_SATCall == TRUE)
         {
            //Ensure calling string is NULL terminated
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
         }
    	//x0pleela 03 Nov, 2006 DR: OMAPS000101158
    	//check the value of uOthersrc
    	//If the value is not set then do the phone book search operation
    	//Else do not perform  phone book search operation
    	//this is to avoid stack overflow probelm when the source id is not LOCAL
	if(!mfw_get_OtherSrc_Value()) 
	{
#ifdef NO_ASCIIZ
	// else  //x0pleela 03 Nov, 2006  DR: OMAPS000101158
	if ((bookFindNameInSDNPhonebook(v_Number, &phb_entry) == 1) && (phb_entry.name.len != 0) )
        {
            memcpy(call_data.phbNameNumber, (char*)phb_entry.name.data, sizeof(call_data.phbNameNumber)-1);
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
        }
        else if(bookFindNameInPhonebook(v_Number, &phb_entry) == 1 && (phb_entry.name.len != 0) )
        {
            memcpy(call_data.phbNameNumber, (char*)phb_entry.name.data, sizeof(call_data.phbNameNumber)-1);
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
        }
#else
        else if ( (bookFindNameInSDNPhonebook(v_Number, &phb_entry) == 1) && (phb_entry.name[0] != 0) )
        {
            memcpy(call_data.phbNameNumber, (char*)phb_entry.name, sizeof(call_data.phbNameNumber)-1);
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
        }
        else if(bookFindNameInPhonebook(v_Number, &phb_entry) == 1 && (phb_entry.name[0] != 0) )
        {
            memcpy(call_data.phbNameNumber, (char*)phb_entry.name, sizeof(call_data.phbNameNumber)-1);
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
        }
#endif
        else
        {
            memcpy(call_data.phbNameNumber,v_Number, sizeof(call_data.phbNameNumber)-1);
            /*MC, SPR 1257, merge from 3.3.3*/
            /* Marcus: Issue 987: 18/09/2002: Ensured call_data.phbNameNumber null terminated */
            call_data.phbNameNumber[sizeof(call_data.phbNameNumber)-1] = 0;
            /*MC end*/
// Sep 01, 2006 REF:OMAPS00090555  x0039928			
            if ( (outgoing_call_data->ton == MFW_TON_INTERNATIONAL) && (
                outgoing_call_data->number[0] != '+') )
            {
                char InterNat_Plus[EDITOR_SIZE];		/* NDH : This should NOT be static!& the size is determined by calldata.phbNameNumber */

                TRACE_EVENT("International Call Required!");
                // API - 826 - Create a buffer and a plus to the start of the buffer then add the out going number
                strcpy(InterNat_Plus, "+");
                mfwStrncpy(&InterNat_Plus[1], call_data.phbNameNumber, sizeof(InterNat_Plus)-1);

                memcpy(call_data.phbNameNumber, (char*)InterNat_Plus, sizeof(call_data.phbNameNumber));
            }
        }
        }

    	//x0pleela 03 Nov, 2006 DR: OMAPS000101158
    	//check the value of uOthersrc
    	//If the value is set then reset it to FALSE
	if(mfw_get_OtherSrc_Value())
		mfw_set_OtherSrc_Value(FALSE);

        if (call_SATCall == TRUE)
        {
            TRACE_EVENT ("SAT_CALL_DIALOG");
            // Marcus: Issue 1057: 21/01/2003: Removed: call_SATCall = FALSE;

            /* create the outgoing call dialog */
            call_calling_show(MMI_OUTGOING_CALL_DIALOG_SAT);

        }
        else
        {
            TRACE_EVENT ("NORMAL_CALL_DIALOG");

            /* create the outgoing call dialog */
            call_calling_show(MMI_OUTGOING_CALL_DIALOG_NORMAL);
        }
	}
       break;
    case E_CM_TIMER_EVERY_SECOND:
      TRACE_EVENT("EVERY_SECOND");
      refreshCallTimer();
      cm_get_aoc_value(CM_AOC_CCM,&call_data.aocInfo);

	  TRACE_EVENT_P1("winIsFocussed(call_data.win) %d",winIsFocussed(call_data.win));

	  /* SPR#1602 - SH - Uncomment check to see if window is focussed */
	 // Oct 27, 2004 REF: CRR MMI-SPR-15632 xnkulkar
	 // Description: SMS : class 2 and 3 SMSMT : icon not displayed when 2 active calls
	 // Fix: condition "call_data.calls.mode == CallSingle" is removed to make the sms icon visible
	 // even if there are more than one calls present. 
	  if (winIsFocussed(call_data.win)
	  	//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
			|| ((!phlock_win_handle) || (mmiPinsEmergencyCall()))
#endif //FF_PHONE_LOCK
	  	)
        	winShow(call_data.win);
    break;
    case E_CM_CPI:
	{
	  T_DISPLAY_DATA display_info; /* cq11676: User Busy indication for E plus. 10-02-04 MZ */	
	  cmCpi = (T_MFW_CM_CPI*)para;	  
	  TRACE_EVENT_P5("callCmEvent - CPI : Call No %d, type %d, InBand %d, TCh %d, Cause 0x%4x ",
	  					cmCpi->call_number,
	  					cmCpi->type,
	  					cmCpi->inband,
	  					cmCpi->tch,
	  					cmCpi ->cause);

	  if (cmCpi->type == CPI_TYPE_ALERT)/*a0393213 lint warnings removal - CPI_MSG_Alert changed to CPI_TYPE_ALERT*/
      {
          if (!cmCpi->inband)
          {

/* RAVI - 20-1-2006 */          
/* Silent Implementation */          
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_RINGTONE
    audio_PlaySoundID(AUDIO_MICROPHONE, CALLTONE_SELECT, 0, AUDIO_PLAY_INFINITE);
#else
  	audio_PlaySoundID( AUDIO_MICROPHONE, TONES_RINGING_TONE,
                                         getCurrentVoulmeSettings(), AUDIO_PLAY_INFINITE);
#endif
#else
	/* Testcase 26.8.x - generated internal alert */
	audio_PlaySoundID(AUDIO_MICROPHONE, TONES_RINGING_TONE, 0, AUDIO_PLAY_INFINITE);
#endif
	       LocalRing = TRUE;
          }
         else if (LocalRing)
          {
#ifdef FF_MMI_RINGTONE /* Stop MSL */
             audio_StopSoundbyID(AUDIO_MICROPHONE , CALLTONE_SELECT);
#else
            audio_StopSoundbyID(AUDIO_MICROPHONE ,TONES_RINGING_TONE);
#endif
            LocalRing = FALSE;
          }

      }
      else if ((cmCpi->type == CPI_TYPE_SYNC) && (cmCpi->tch)) /*a0393213 lint warnings removal - CPI_MSG_Sync changed to CPI_TYPE_SYNC*/
      {
      	 /*
      	 ** check whether this is an incoming call from Idle, as the Vocoder should not be disabled if
      	 ** we are already in a call, or if Late Traffic Channel assignment has caused the channel to be
      	 ** assigned after the call has been accepted.
      	 */
	 if (call_data.vocoderState == CALL_VOCODER_IDLE)
	 {
		 /*
		 ** A the Traffic Channel is assigned the Vocoder will have been turned on by Layer1.
		 ** Disable the Vocoder, so that an E2 Melody can be played
		 */
		 mfw_aud_l1_disable_vocoder();

		 call_data.vocoderState = CALL_VOCODER_DISABLED;
	 }
	 
      }
       else if((cmCpi->type == CPI_TYPE_DISC) && (cmCpi->inband)) /*a0393213 lint warnings removal - CPI_MSG_Disc changed to CPI_TYPE_DISC*/
	{
		/* cq11676: Display the Number busy indication on receiving the Disconnect cause and allow user to stay in calling screen 
		    untill the final Traffic channel Release indication has been received. Specific for E+ network. 17-02-04 MZ */
		
#ifdef FF_2TO1_PS
		 if ( cmCpi->cause ==  CAUSE_NWCC_USER_BUSY)
#else
		 if ( cmCpi->cause ==  M_CC_CAUSE_USER_BUSY)
#endif
         {
	       	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNumberBusy, TxtNull, COLOUR_STATUS);
	       	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
	 		info_dialog(idle_get_window(), &display_info);
		 } 

	}
   }
    break;
    /*MC SPR 1392, call deflection status*/
	case E_CM_CALL_DEFLECT_STATUS:
	{	BOOL* result = para;
		T_DISPLAY_DATA display_info;


		if (*result == TRUE)/*If call deflection was successful*/
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallDeflection, TxtDone, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
		}
		else/*if call deflection failed*/
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallDeflection, TxtFailed, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
		}
		/*Show dialog*/
		info_dialog(idle_get_window(), &display_info);
		/*destroy the call deflection windows and the call windows if no active call*/
		deflect_destroy(TRUE);

	}
	break;
    /* Marcus: CCBS: 14/11/2002: Start */
    case E_CM_CCBS:
        TRACE_EVENT("E_CM_CCBS");
        call_cm_ccbs_event((T_MFW_CM_CCBS_INFO *) para);
        break;
    /* Marcus: CCBS: 14/11/2002: End */

    /* SPR#1352 - SH - TTY */
#ifdef MMI_TTY_ENABLED
    case E_CM_CTYI:
      cmCTYI  = (T_MFW_CM_CTYI*)para;
      switch (cmCTYI->neg)
      {
      	case CTYI_REJECT:
      		TRACE_EVENT("CTYI REJECT");
      		/* TTY rejected, switch it off */

  			call_tty_set(CALL_TTY_OFF);
   			call_tty_statuswin();
      		break;
      	case CTYI_GRANT:
      		TRACE_EVENT("CTYI GRANT");
      		/* If user has set "always on", then set that status.
      		 * Otherwise, we're just switching on TTY for the next
      		 * call */

  			if (FFS_flashData.ttyAlwaysOn)
  				call_tty_set(CALL_TTY_ALWAYSON);
  			else
  				call_tty_set(CALL_TTY_ONNEXTCALL);

  			call_tty_statuswin();
      		break;
      }
      break;
#endif
    /* end SH */

        default:
    return 0;
    }
    return 1;
}



#if(0) /* x0039928 - Commented for lint warning removal */
/*******************************************************************************

 $Function:     callWaitingAnswerKeyEvent

 $Description:

 $Returns:    1, processed, 0 ignored

 $Arguments:  event and parameter

*******************************************************************************/

static int callWaitingAnswerKeyEvent (MfwEvt e, MfwKbd *k)
{
  TRACE_FUNCTION("callWaitingAnswerKeyEvent");
  switch (k->code)
  {

    case KCD_LEFT:
      cm_command_abort(call_data.current_command);
    break;
    default:
    break;
  }
  return 1;
}

#endif



/*******************************************************************************

 $Function:     showDefault

 $Description:  common display content

 $Returns:

 $Arguments:

*******************************************************************************/



static void showDefault (void)
{
  dspl_ClearAll();
  GlobalIconStatus = GlobalSignalIconFlag | GlobalBatteryIconFlag
						| GlobalSMSIconFlag| GlobalVoiceMailIconFlag
						| GlobalCallForwardingIconFlag| GlobalKeyplockIconFlag
						| GlobalRingerIconFlag | GlobalRingVibrIconFlag
						| GlobalVibratorIconFlag | GlobalAlarmIconFlag
						| GlobalSilentRingerIconFlag|GlobalCipheringActivIconFlag
						| GlobalCipheringDeactivIconFlag
 /* SH 18/01/02. Flag for GPRS On icon.
    Note: Homezone icon not displayed during call. */
		#ifdef MMI_GPRS_ENABLED
			  | GlobalGPRSOnIconFlag
		#endif
	/* SPR#1352 - SH - Flag for TTY icon */
		#ifdef MMI_TTY_ENABLED
				| GlobalTTYIconFlag
		#endif
	/* SH end */
				;
  iconsShow();
}







#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:     showSelectPrivateCall

 $Description:  multiparty private call handling

 $Returns:

 $Arguments:

*******************************************************************************/
// define position we will display number/name of caller
#define CALLID_NUM_XPOS (35)
// define number of chars that can be displayed (at this position)
#define STR_DISPLAY_NCHAR ((SCREEN_SIZE_X-CALLID_NUM_XPOS)/6)


static void showSelectPrivateCall(void){


    int i = 0;
    char stat;
    char line[MAX_LINE];
  char tag[MAX_CHAR];
  int topLine;
  TRACE_FUNCTION("showSelectPrivateCall");

  showDefault();
  memset(tag,'\0',sizeof(tag));
  if (call_data.calls.numCalls<MAX_CALLS)
		topLine = Mmi_layout_line(2);
  else
		topLine = Mmi_layout_line(1);
  for (i=0;i<call_data.calls.numCalls;i++)
    {
      memset(line,'\0',sizeof(line));
      memset(tag,'\0',sizeof(tag));
      sprintf(line,"%d",i+1);
      if (call_data.calls.status[i].stat == CALL_ACTIVE)
            	stat = 'A';
               else
        		stat = 'H';
	  		sprintf(line,"%d.%c.",i+1,stat);
      if (i==callIndex(call_data.calls.selected))
			dspl_TextOut(CALLINFO_X_AH, (USHORT)(topLine+Mmi_layout_line_height()*i), DSPL_TXTATTR_INVERS, line);
      else
			dspl_TextOut(CALLINFO_X_AH, (USHORT)(topLine+Mmi_layout_line_height()*i), 0, line);

#ifdef NO_ASCIIZ
 /*MC, SPR 1257, merge from 3.3.3*/
      if (call_data.calls.status[i].name.len > 0)
         {
			resources_truncate_to_screen_width((char*)call_data.calls.status[i].name.data,call_data.calls.status[i].name.len ,(char*)tag,MAX_CHAR-2, SCREEN_SIZE_X,FALSE);
         }
/*MC end*/
#else
            if (strlen((char*)call_data.calls.status[i].name) > 0)
            {
            	resources_truncate_to_screen_width(call_data.calls.status[i].name,0 ,(char*)tag,MAX_CHAR-2, SCREEN_SIZE_X,FALSE);
            }
#endif
           	else
          if (strlen((char*)call_data.calls.status[i].number) > 0)
          {
			resources_truncate_to_screen_width((char*)call_data.calls.status[i].number,0 ,(char*)tag,MAX_CHAR-2, SCREEN_SIZE_X,FALSE);
          }
          else
          {
			resources_truncate_to_screen_width(GET_TEXT(TxtSoftCall),0 ,(char*)tag,MAX_CHAR-2, SCREEN_SIZE_X,TRUE);
          }
      if (i==callIndex(call_data.calls.selected))
			dspl_TextOut(CALLINFO_X_TAGPOS,(USHORT)(topLine+Mmi_layout_line_height()*i), DSPL_TXTATTR_INVERS, tag);
      else
			dspl_TextOut(CALLINFO_X_TAGPOS,(USHORT)(topLine+Mmi_layout_line_height()*i), 0, tag);
  }
  displaySoftKeys(TxtSoftSelect,TxtEnd);
}



/*******************************************************************************

 $Function:     showWaitingAnswer

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static void showWaitingAnswer(void){


  TRACE_FUNCTION("showWaitingAnswer");
    showDefault();
	PROMPT(TXTMESSAGE_X, Mmi_layout_line(2), 0, TxtPleaseWait);
  displaySoftKeys(TxtCancel,'\0');
}

#endif

/*******************************************************************************

 $Function:     showSendingDTMF

 $Description:  displays sending DTMF

 $Returns:

 $Arguments:

*******************************************************************************/


void showSendingDTMF(void){


  TRACE_FUNCTION("showSendingDTMF");
    showDefault();
	PROMPT(TXTMESSAGE_X, Mmi_layout_line(2), 0, TxtSendingDTMF);
  displaySoftKeys(TxtCancel,'\0');
}





/*******************************************************************************

 $Function:     showInformation

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void showInformation(void){


  TRACE_FUNCTION("showInformation");
    showDefault();
  dspl_TextOut(24, 16, 0, callInformation);
}





/*******************************************************************************

 $Function:     callSetMode

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static void callSetMode (int mode, BOOL dialogDisp){

    T_DISPLAY_DATA   display_info;



  switch (mode){
        case ViewConnect:
      if (call_data.calls.numCalls)
      {
        call_data.refreshConnect = 1;
        call_data.calls.view =  ViewConnect;
      }
      else
      { /*SPR 1392, check for valid incoming call ID rather than existence of incoming
        call window.  During the setting up of call deflection, the incoming call screen doesn't exist*/
        if ((!call_data.incCall) && (!call_data.win_calling)&&(call_data.win))
        	call_destroy(call_data.win);
        return;
      }
        break;
        case ViewMenu:
      call_data.calls.view = ViewMenu;
        break;
        case ViewEnd:
          {
      T_MFW_CM_AOC_INFO   aoc_info;

      TRACE_EVENT(" Call ended  read the AOC again");

      // this is used for the "last charge"
      cm_get_aoc_value(CM_AOC_ACM, &aoc_info);

      if ( aoc_info.acm > acm_last_call )
      {
        //for the last charge
        acm_last_call = aoc_info.acm - acm_last_call;
      }
      else
      {
        TRACE_EVENT(" ACM value didnt change");
      }


      memset(line,'\0',sizeof(line));
      if (InCallTimer == TRUE)
        getTimeString(call_data.timeEnded,(char*)line);
      setLastCallDuration(call_data.timeEnded, call_data.call_direction);

	if (dialogDisp)
	{
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallEnded, TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_ending_cb, THREE_SECS, KEY_CLEAR );

		/*
		* Call Info Screen
		*/
		call_data.win_ending = info_dialog (call_data.win, &display_info);
	}
        }
    break;
        case ViewAutoRedial:
      		memset(line,'\0',sizeof(line));
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtCancel, TxtAutoredial,  TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)callAutoRedialKeyEvent, FOREVER, KEY_RIGHT|KEY_CLEAR|KEY_HUP );
		    display_info.TextString2  = (char*)call_data.phbNameNumber;

			call_data.win_redial = info_dialog (call_data.win, &display_info);
        break;
    default:
    break;
  }


	switch (mode)
	{
	case ViewDTMF:
		call_data.calls.view = ViewDTMF;
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		call_data.editorAttrDTMF.cursor = ED_CURSOR_BAR;
#else /* NEW_EDITOR */
		call_data.edtAttrDTMF.mode = edtCurBar1;
#endif /* NEW_EDITOR */
		break;

	case ViewScratchPad:
		call_data.calls.view = ViewScratchPad;
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		call_data.editorAttrDTMF.cursor = ED_CURSOR_BAR;
#else /* NEW_EDITOR */
		call_data.edtAttrDTMF.mode = edtCurBar1;
#endif /* NEW_EDITOR */
        break;

    case ViewSelectPrivateCall:
    	call_data.calls.view = ViewSelectPrivateCall;
        break;


        //GW Added
	case ViewShortWait:
		dlg_initDisplayData_TextId( &display_info, TxtCancel, TxtNull, TxtPleaseWait,  TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_waiting_answer_cb, ONE_SECS, KEY_LEFT );
		display_info.TextString2   = (char*)line;
		call_data.win_waiting = info_dialog (call_data.win, &display_info);
        break;

	case ViewWaitingAnswer:
		dlg_initDisplayData_TextId( &display_info, TxtCancel, TxtNull, TxtPleaseWait,  TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_waiting_answer_cb, FOREVER, KEY_LEFT );
	    display_info.TextString2   = NULL;
		call_data.win_waiting = info_dialog (call_data.win, &display_info);
    break;

    case ViewSendingDTMF:
		dlg_initDisplayData_TextId( &display_info, TxtCancel, TxtNull, TxtSendingDTMF,  TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_sending_dtmf_cb, FOREVER, KEY_LEFT );
	    display_info.TextString2   = (char*)line;
    	call_data.win_sending_dtmf = info_dialog (call_data.win, &display_info);
        break;
    default:
    break;
  }
  winShow(call_data.win);
}






/*******************************************************************************

 $Function:     contactsExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int contactsExec(MfwMnu* m, MfwMnuItem* i)
{

 // T_MFW_HND       win  = mfwParent(mfw_header());    // RAVI
//  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;   // RAVI
  //  T_call_menu * data = (T_call_menu *)win_data->user;   // RAVI


  bookPhonebookStart(call_data.win,PhbkNormal);
  return 1;

}

//May 27, 2004    REF: CRR 19186  Deepa M.D 
/*******************************************************************************

  $Function:	  M_exeSetting
  
  $Description:	  This function is called when the user press "Settings"  in the
				  Active call "options" screen.This function creates and displays the 
				  SMS Settings menu in  an active call screen.
	
  $Returns:		  Execution status
	  
  $Arguments:	  m - menu handler
				  i - Menu item selected
		
*******************************************************************************/

static int M_exeSetting(MfwMnu* m, MfwMnuItem* i)
{
	
//  T_MFW_HND       win  = mfwParent(mfw_header());  // RAVI
//    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data; // RAVI
//     T_call_menu * data = (T_call_menu *)win_data->user;  // RAVI
	TRACE_FUNCTION("M_exeSetting");
	
	/* Create the  SMS Setting menu*/
	bookMenuStart(call_data.win, settingMenuAttributes(),0);
	
	return 1;
}


//August 24 2004 e-armanetsaid added for camera driver test purposes
// Feb 13, 2006    REF:DR OMAPS00067943  x0039928
// Fix : The Menu function M_exeViewfinder is renamed to M_exeApplication 
/*******************************************************************************

  $Function:	  M_exeApplication
  
  $Description:	  This function is called when the user press "Application"  in the
				  Active call "options" screen.This function creates and displays the 
				  Application menu in  an active call screen.
	
  $Returns:		  Execution status
	  
  $Arguments:	  m - menu handler
				  i - Menu item selected
		
*******************************************************************************/

static int M_exeApplication(MfwMnu* m, MfwMnuItem* i)
{
	
 // T_MFW_HND       win  = mfwParent(mfw_header());     // RAVI
 // T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;   // RAVI
//  T_call_menu * data = (T_call_menu *)win_data->user;    // RAVI
	TRACE_FUNCTION("M_exeSetting");
	
	/* Create the  Application menu*/
	bookMenuStart(call_data.win, applicationsMenuAttributes(),0);
	
	return 1;
}

/*******************************************************************************

 $Function:     holdExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int holdExec(MfwMnu* m, MfwMnuItem* i){

  TRACE_FUNCTION("holdExec");
  switch (call_data.calls.mode){
    case CallSingle:
    case CallTwo:
	 // Oct 07, 2004 REF: CRR MMI-FIX-24440 xnkulkar
	 // callIndex function is used instead of directly using status[call_data.calls.selected]
      if (call_data.calls.status[callIndex(call_data.calls.selected)].stat == CALL_HELD)
      {
        cm_command(call_data.calls.selected, CM_RETRIEVE, '\0');
        call_data.current_command = CM_RETRIEVE;
      }
      else
      {
          cm_command(call_data.calls.selected, CM_HOLD,'\0' );
         call_data.current_command = CM_HOLD;
      }
      callSetMode(ViewWaitingAnswer, TRUE);
    break;
    case CallConference:
	 // Oct 07, 2004 REF: CRR MMI-FIX-24440 xnkulkar
	 // callIndex function is used instead of directly using status[call_data.calls.selected]
      if (call_data.calls.status[callIndex(call_data.calls.selected)].stat == CALL_HELD)
      {
        cm_command(call_data.calls.selected, CM_RETRIEVE_MULTIPARTY, '\0');
        call_data.current_command = CM_RETRIEVE_MULTIPARTY;
      }
      else
      {
        cm_command(call_data.calls.selected, CM_HOLD_MULTIPARTY, '\0');
        call_data.current_command = CM_HOLD_MULTIPARTY;
      }
      callSetMode(ViewWaitingAnswer, TRUE);
    break;
    case CallMultipartySingle:
    case CallSingleMultiparty:
    default:
    break;
  }
  return 1;
}

/*******************************************************************************

 $Function:     muteExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int muteExec(MfwMnu* m, MfwMnuItem* i){

    T_MFW_HND       win  = mfwParent(mfw_header());
//  T_MFW_WIN       *win_data = ((T_MFW_HDR *)win)->data;  //RAVI
//  T_call_menu     *data = (T_call_menu *)win_data->user;    // RAVI
 
    if (call_data.calls.muted)
    {
        if (audio_SetMute(AUDIO_MICROPHONE, AUDIO_MUTING_OFF) == DRV_OK)
        {
            call_data.calls.muted = false;
            menuCallSingleItems[2].str = (char*)TxtMute;
        }
    }
    else
    {
        if (audio_SetMute(AUDIO_MICROPHONE, AUDIO_MUTING_ON) == DRV_OK)
        {
            call_data.calls.muted = true;
            menuCallSingleItems[2].str = (char*)TxtUnmute;
        }
    }

    call_menu_destroy(win);
    call_data.win_menu = 0;
    return 1;
}

/*******************************************************************************

 $Function:     scratchPadExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int scratchPadExec(MfwMnu* m, MfwMnuItem* i){

    T_MFW_HND       win  = mfwParent(mfw_header());
//  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
//  T_call_menu * data = (T_call_menu *)win_data->user;

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	call_scratchpad_start(win,ctrlBack,ViewScratchPad, 0);
#else
	call_scratchpad_start(win,ecBack,ViewScratchPad, 0);
#endif
  return 1;
}

/*******************************************************************************

 $Function:     mainMenuExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int mainMenuExec(MfwMnu* m, MfwMnuItem* i){

//JVJE  menuEnter();
  return 1;

}

/*******************************************************************************

 $Function:     startConferenceExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int startConferenceExec(MfwMnu* m, MfwMnuItem* i){

  cm_command(0,CM_BUILD_MULTIPARTY,'\0');
  call_data.current_command = CM_BUILD_MULTIPARTY;
  callSetMode(ViewWaitingAnswer, TRUE);
  return 0;

}

/*******************************************************************************

 $Function:     privateStartAddConferenceExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int privateStartAddConferenceExec(MfwMnu* m, MfwMnuItem* i){

  switch(call_data.calls.mode){
		case CallConference:
			cm_command(call_data.calls.selected,CM_SPLIT_MULTIPARTY,'\0');
			callSetMode(ViewShortWait, TRUE);
		break;
	case CallMultipartySingle :
	case CallSingleMultiparty :
	case CallTwo:
			TRACE_EVENT("call to BUILD_MULTIPARTY------------------------------------");
      cm_command(0,CM_BUILD_MULTIPARTY,'\0');
      call_data.current_command = CM_BUILD_MULTIPARTY;
      callSetMode(ViewWaitingAnswer, TRUE);
    break;
    default:
    break;
  }
  return 1;

}




/*******************************************************************************

 $Function:     swapExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int swapExec(MfwMnu* m, MfwMnuItem* i){

  switch(call_data.calls.mode){
    case CallSingleMultiparty :
		case CallMultipartySingle :

			cm_command(0,CM_SWAP_MULTIPARTY, '\0');
      call_data.current_command = CM_SWAP_MULTIPARTY;
      callSetMode(ViewWaitingAnswer, TRUE);
    break;
    case CallTwo:
        cm_command(call_data.calls.selected,CM_SWAP, '\0');
      call_data.current_command = CM_SWAP;
      callSetMode(ViewWaitingAnswer, TRUE);
    break;
    default:
    break;
  }
  return 1;
}

/*******************************************************************************

 $Function:     endAllExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int endAllExec(MfwMnu* m, MfwMnuItem* i){

  cm_end_all();

  return 1;

}

// Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
// Handler for "End held" menu item added

/*******************************************************************************

 $Function:     endHeldExec

 $Description: when "End held" menu option is pressed, this function is called

 $Returns: 1 for success 0 for failure

 $Arguments: not used

*******************************************************************************/

static int endHeldExec(MfwMnu* m, MfwMnuItem* i){

  cm_end_held();

  return 1;

}


/*******************************************************************************

 $Function:     endConferenceExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int endConferenceExec(MfwMnu* m, MfwMnuItem* i){

  cm_disconnect(0);

  return 1;

}



/*******************************************************************************

 $Function:     transferExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/
/* x0039928 - Commented for lint warning removal */
/*
static int transferExec(MfwMnu* m, MfwMnuItem* i){

  cm_command(0,CM_ETC,'\0');
  call_data.current_command = CM_ETC;
  callSetMode(ViewWaitingAnswer, TRUE);
  return 1;

}
*/ 



/*******************************************************************************

 $Function:     removeCall

 $Description:  removes a managed call

 $Returns:

 $Arguments:  call id

*******************************************************************************/



//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
int removeCall(SHORT cid, BOOL dialogDisp, BOOL allCallsEnded)
{
    int i;
  TYPEQUEUE* first;

  TRACE_EVENT_P1("Remove call %d",cid);
  if (!call_data.accessEnd){
    TRACE_EVENT("not access end");
    first = call_data.endQueue;
    while (call_data.endQueue){
      call_data.endQueue = call_data.endQueue->next;
    }
      call_data.endQueue = (TYPEQUEUE*)ALLOC_MEMORY(sizeof(TYPEQUEUE));
      call_data.endQueue->call_number = cid;
      call_data.endQueue->next = 0;
    if (first) call_data.endQueue = first;
    return 0;
  }
  if (call_data.endQueue){
    first = call_data.endQueue->next;
    FREE_MEMORY( (void *)call_data.endQueue, sizeof(TYPEQUEUE));
    call_data.endQueue = first;
  }
  call_data.accessEnd = FALSE;
  call_data.timeEnded = call_data.calls.time[callIndex(cid)];

  call_data.call_direction = call_data.calls.status[callIndex(cid)].call_direction;
  call_data.calls.status[callIndex(cid)].call_direction = MFW_CM_INV_DIR;
  
    for (i = 0; i < MAX_CALLS; i++)
    {
        if (call_data.calls.status[i].call_number == cid)
            call_data.calls.status[i].call_number = 0;

        if (i < (MAX_CALLS -1))
        {
            if (call_data.calls.status[i].call_number == 0 && call_data.calls.status[i+1].call_number != 0)
            {
                call_data.calls.status[i] = call_data.calls.status[i+1];
                call_data.calls.time[i] = call_data.calls.time[i+1];
                call_data.calls.status[i+1].call_number = 0;
                call_data.calls.status[i+1].call_direction = MFW_CM_INV_DIR;
            }
        }
    }

	if (call_data.calls.numCalls > 0)
		call_data.calls.numCalls--;

	if (call_data.calls.selected == cid)
		call_data.calls.selected = 0;

	if (call_data.calls.numCalls ==1)
		call_data.calls.mode = CallSingle;

	if (call_data.calls.mode == CallSingleMultiparty)
		//Mar 26,2008, OMAPS00164785, x0091220(Daniel)
		call_data.calls.mode = CallSingle;

	if (call_data.singleCall == cid)
		call_data.singleCall = 0;
	//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
	// When removing calls one by one from removeAllCalls, rebuild calls should not be called
	if (call_data.calls.numCalls && (allCallsEnded==FALSE))
		rebuildCalls();
	else
	{
		resetDTMFQueue(); /*OMAPS00107103 a0393213(R.Prabakar) - The DTMF queue is flushed once the call is ended*/
		call_data.vocoderState = CALL_VOCODER_IDLE;
	}

	callSetMode(ViewEnd, dialogDisp);
	return 1;
}


/*******************************************************************************

 $Function:     removeAllCalls

 $Description:  removes a managed call

 $Returns:

 $Arguments:  call id

*******************************************************************************/




int removeAllCalls(void)
{
	int i;
	int j = 0;

	TRACE_FUNCTION("removeAllCalls()");

	for (i=0; i<MAX_CALLS; i++)
	{
		/*
		** Because removecall() shifts the data down each time a call is removed, the index to use should remain as 0
		*/
		if (call_data.calls.status[j].call_direction != MFW_CM_INV_DIR)
		{
			call_data.accessEnd = TRUE;
			//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
			// When removing calls one by one from removeAllCalls, rebuild calls should not be called
			// Added a new paramter to the removeCall function to indicate the same
			removeCall(call_data.calls.status[j].call_number, FALSE,TRUE);
		}
		else
			j++;
	}

	return 1;
}

/*******************************************************************************

 $Function:     addCall

 $Description:

 $Returns:

 $Arguments:  call id

*******************************************************************************/


int addCall(SHORT cid) {


  MmiModuleDel(ModuleInCall);
  if (!call_data.calls.numCalls){
    MmiModuleSet(ModuleCall);
    call_data.callStatus = CallActive;

        TRACE_EVENT("active call!!");
  }
    if (call_data.calls.numCalls >= MAX_CALLS)
        return 0;
    if (call_data.calls.status[call_data.calls.numCalls].call_number != 0)
        return 0;
#ifdef NO_ASCIIZ
{
    /*a0393213 lint warning:access beyond array soln:memcpy changed to mfwStrncpy*/
    mfwStrncpy((char*)call_data.calls.status[call_data.calls.numCalls].name.data,
           (char*)call_data.phbNameNumber,
           sizeof(call_data.calls.status[call_data.calls.numCalls].name.data));
    call_data.calls.status[call_data.calls.numCalls].name.len =
        dspl_str_length(call_data.phbNameNumber);
    /* Marcus: Issue 987: 18/09/2002: End */
    /*MC end*/
}
#else

  strncpy((char*)call_data.calls.status[call_data.calls.numCalls].name,
      (char*)call_data.phbNameNumber,sizeof(call_data.calls.status[call_data.calls.numCalls].name));
#endif
    getCallStatus(cid, &call_data.calls.status[call_data.calls.numCalls]);
  call_data.calls.time[call_data.calls.numCalls] = 0;
  call_data.calls.numCalls++;
  call_data.calls.selected = cid;
  switch (call_data.calls.mode){
    case CallSingle:
      if (call_data.calls.numCalls > 1)
        call_data.calls.mode = CallTwo;
    break;
    case CallConference:
      call_data.singleCall = cid;
      call_data.calls.mode = CallSingleMultiparty;
    break;
    default:
      if (call_data.calls.numCalls == 1)
        call_data.calls.mode = CallSingle;
    break;
  }
  rebuildCalls();
    return 1;
}

/*******************************************************************************

 $Function:     callIndex

 $Description:

 $Returns:    returns the cm call id

 $Arguments:  call id

*******************************************************************************/


int callIndex(SHORT cid) {

int i;

    for (i = 0; i < MAX_CALLS; i++) {
        if (call_data.calls.status[i].call_number == cid)
            return i;
    }
    return MAX_CALLS;
}

/*******************************************************************************

 $Function:     callConnect

 $Description:

 $Returns:

 $Arguments:  call id

*******************************************************************************/



void callConnect(SHORT cid){

  TRACE_FUNCTION("Call Connect");
  addCall(cid);
  callSetMode(ViewConnect, TRUE);

}

/*******************************************************************************

 $Function:     callNumber

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



void callNumber(UBYTE *number)
{
	T_MFW     res;
// 	T_MFW_PHB_ENTRY phb_entry; // RAVI
	T_MFW_SS_RETURN ssRes;
	char edt_buf_prov[EDITOR_SIZE];

TRACE_FUNCTION("callnumber()");
// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is registered  for the first call when an outgoing call is done.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED
if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)
#endif	
		mfw_hook_register();
#endif
  
      call_data.emergencyCall = FALSE;

      mfwStrncpy((char*)edt_buf_prov,(char*)number, EDITOR_SIZE);
      ssRes = ss_check_ss_string((UBYTE*)edt_buf_prov);
	  
      switch (ssRes)
      {
        case MFW_SS_USSD:
        case MFW_SS_SIM_LOCK:
        case MFW_SS_REG_PW:
        case MFW_SS_CF:
        case MFW_SS_CB:
        case MFW_SS_CLIR:
        case MFW_SS_CLIP:
        case MFW_SS_COLR:
        case MFW_SS_COLP:
        case MFW_SS_WAIT:
        case MFW_SS_HOLD:

        case MFW_SS_SIM_REG_PW:
        case MFW_SS_SIM_UNBLCK_PIN:
        case MFW_SS_MMI:
       	res =   cm_mo_call((UBYTE*)number, VOICE_CALL);
		switch(res)	
		{
			//xvilliva 21421 - We flash an alert "Invalid Request" to user as Line1
			//is locked and CF interrogation is done for line2.
			case CM_ALS_ERR: 
				info_screen(0, TxtNotImplemented, TxtNull, NULL);
				break;
		}
        return;           /* Handled on # */
        /*MC 12/04/02*/
        case MFW_SS_ECT:
        	TRACE_EVENT("Recognised ECT string");
        	cm_mo_call((UBYTE*)number, VOICE_CALL);
        return;           /* Handled on # */
        case MFW_SS_CCBS:
        	TRACE_EVENT("Recognised CCBS string");
        	cm_mo_call((UBYTE*)number, VOICE_CALL);
          return;
        case MFW_SS_DIAL:
        case MFW_SS_DIAL_IDX:
        case MFW_SS_DTMF:
        case MFW_SS_UNKNOWN:
        default:
        break;            /* Remove #, it will be added later on again */
      }

      res = cm_mo_call((UBYTE*)number, VOICE_CALL);
      switch (res){
        case CM_OK :
          TRACE_EVENT("call OK");
        break;
        case CM_EC:
          call_data.emergencyCall = TRUE;
        break;
				case CM_BLACKLIST:
					TRACE_EVENT("number blacklisted now");
					/*NM, p018*/
					if (call_data.win)
						bookShowInformation(call_data.win,Txtblacklisted,NULL,(T_VOID_FUNC)call_failure_cb);
				break;
        case CM_SIM:
        case CM_SS :
        case CM_MMI:
        case CM_USSD:
        return;
//      break; //RAVI
		//GW-SPR#1035 - Added from SAT changes baseline
		case CM_CFDN_ERR:
		case CM_CBDN_ERR:
				TRACE_EVENT("MO-Call blocked by FDN or BDN");
				info_screen(0, TxtCallBlocked, TxtNull, NULL);
				break;
        default:
					TRACE_FUNCTION("call not OK");
  			//check if call ended because ACM>=ACMMAX
				{
// Sep 01, 2006 REF:OMAPS00090555  x0039928
// Fix: Memory for aocInfo is allocated fron heap instead of stack if flag FF_MMI_PB_OPTIM is defined
			#ifdef FF_MMI_PB_OPTIM
				T_MFW_CM_AOC_INFO* aocInfo;
				aocInfo = (T_MFW_CM_AOC_INFO*)ALLOC_MEMORY(sizeof(T_MFW_CM_AOC_INFO));
			#else
				T_MFW_CM_AOC_INFO aocInfo;
			#endif

			#ifdef FF_MMI_PB_OPTIM
				cm_get_aoc_value(CM_AOC_ACMMAX,aocInfo);
  				cm_get_aoc_value(CM_AOC_ACM,aocInfo);
				if((aocInfo->acm >= aocInfo->acm_max) &&
					( aocInfo->acm_max > 0))
			#else
					cm_get_aoc_value(CM_AOC_ACMMAX,&aocInfo);

					//Get ACM and ACMMAX
				    cm_get_aoc_value(CM_AOC_ACM,&aocInfo);

					//display "No Credit" to user
					if((aocInfo.acm >= aocInfo.acm_max) &&
						( aocInfo.acm_max > 0))
			#endif
					{    	T_DISPLAY_DATA display_info;
						/* SPR2500, initialise info dialogue correctly*/
							dlg_initDisplayData_TextId(&display_info,  TxtSoftOK, NULL, TxtNo, TxtCredit, COLOUR_STATUS);
							dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT | KEY_RIGHT);

							TRACE_EVENT("acm_max exceeded");
							info_dialog (call_data.win, &display_info);
// Sep 01, 2006 REF:OMAPS00090555  x0039928
// Fix: if flag FF_MMI_PB_OPTIM is defined then free the allocated memory from heap for aocInfo
						#ifdef FF_MMI_PB_OPTIM
							if(aocInfo)
								FREE_MEMORY((UBYTE*)aocInfo,sizeof(T_MFW_CM_AOC_INFO));
						#endif

							return;

					}
					else
					{
						TRACE_FUNCTION("call not OK to show error");
						/*API - 917 - Added code so that when trying to generate an MO call when FDN is
									  activated an entry is not in the FDN phonebook this screen should be displayed
						*/
						/*NM, p018*/
						if (call_data.win)
						{
							bookShowInformation(call_data.win,TxtOperationNotAvail,NULL,(T_VOID_FUNC)call_failure_cb);
						}
						else
						{
							/* to show any notification even in the case  "call_data.win == 0" */
							info_screen(0, TxtOperationNotAvail, TxtNull, NULL);
						}
					}
// Sep 01, 2006 REF:OMAPS00090555  x0039928
// Fix: if flag FF_MMI_PB_OPTIM is defined then free the allocated memory from heap for aocInfo					
				#ifdef FF_MMI_PB_OPTIM
					if(aocInfo)
						FREE_MEMORY((UBYTE*)aocInfo,sizeof(T_MFW_CM_AOC_INFO));
				#endif
				}
				return;
			}

}

/*******************************************************************************

 $Function:     getTimeString

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



void getTimeString(long time,char *timeString){

  int hour,min,sec;

  hour = time/3600;
  min = (time-hour*3600)/60;
  sec = (time-hour*3600-min*60);
  sprintf(timeString,"%02d:%02d:%02d",hour,min,sec);
  return;
}


/*******************************************************************************

 $Function:     refreshCallTimer

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/




static void refreshCallTimer(){

  int i;

  TRACE_FUNCTION("refreshCallTimer");
    for (i = 0; i < call_data.calls.numCalls; i++)
    call_data.calls.time[i]++;
  return;
}



/*******************************************************************************

 $Function:     call_HeadsetKeyDetection

 $Description:

 $Returns:    Return TRUE if call or incoming call or outgoing call in progress

 $Arguments:  None
*******************************************************************************/



UBYTE call_HeadsetKeyDetection  (void)
{
  if (call_data.incCall || call_data.outCall || call_data.calls.numCalls)
    return 1;
  else
    return 0;
}




/*******************************************************************************

 $Function:     endingCall

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static void endingCall(void){

  TRACE_FUNCTION("Ending call");
  call_data.accessEnd = TRUE;
  
// Nov 16, 2004 REF: CRR MMI-SPR-26120 xnkulkar
// Description: After disconnecting MOC cannot return to STK menu 
// Solution: When the user ends a call, check if it is a STK call and send SAT_RETURN event to 
//   		     return the control to STK menu.
  if (call_SATCall == TRUE)
  {
  	SEND_EVENT(sat_get_setup_menu_win(), SAT_RETURN, 0, NULL);
  }   
  if (call_data.endQueue){
	//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
    removeCall(call_data.endQueue->call_number, TRUE,FALSE);
    return;
  }
  if (!call_data.calls.numCalls)
  { /*SPR 1392, check for valid incoming call ID rather than existence of incoming
        call window.  During the setting up of call deflection, the incoming call screen doesn't exist*/
   if ((!call_data.incCall/*.win_incoming*/)&&(!call_data.win_calling))
   {
    MmiModuleDel(ModuleCall);
    call_data.callStatus = CallInactive;

        if (call_data.win) call_destroy(call_data.win);
   }
  }
  callSetMode(ViewConnect, TRUE);
}



/*******************************************************************************

 $Function:     rebuildCalls

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static void rebuildCalls(void){

  int i;

  TRACE_FUNCTION("rebuildCalls");
  if (call_data.calls.selected == 0)
    call_data.calls.selected = call_data.calls.status[0].call_number;
  if (call_data.calls.mode != CallConference){
    for (i=0;i < call_data.calls.numCalls;i++){
    	TRACE_EVENT_P1("Getting call number %d", i);
      getCallStatus(call_data.calls.status[i].call_number, &call_data.calls.status[i]);
      if (call_data.calls.status[i].stat == CALL_ACTIVE)
        call_data.calls.selected = call_data.calls.status[i].call_number;
    }
  }
  else
    for (i=0;i < call_data.calls.numCalls;i++){
    TRACE_EVENT_P1("Getting call number %d", i);
      getCallStatus(call_data.calls.status[i].call_number, &call_data.calls.status[i]);
    }
    if (call_data.win_menu){
      call_menu_destroy(call_data.win_menu);
      call_data.win_menu = 0;
    }
}


/*******************************************************************************

 $Function:     getCallStatus

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/
//Sandip - 18261  . Setup call Reset fix

static int getCallStatus(SHORT call_number,T_MFW_CM_STATUS* call){

  char prov[MFW_TAG_LEN];
  int p;
  int i;
#ifdef NO_ASCIIZ
  //For future compatibility with unicode
  memset(prov, '\0', MFW_TAG_LEN);
//Changed here for CQ-18261
  memcpy((char*)prov,(char*)call->name.data,MFW_TAG_LEN );
  p = cm_status(call_number,call, 1);
  i=0;
  while ((i <call->name.len) && (call->number[i]==prov[i]))
  {
  	i++;
  }
  if (i == call->name.len)
   	{
  	memset(call->name.data,'\0',PHB_MAX_LEN);
	/*a0393213 lint warning:access beyond array soln:memcpy changed to mfwStrncpy*/
  	mfwStrncpy((char*)call->name.data, (char*)prov, PHB_MAX_LEN);  	
  	}
#else
	//copy current name to temp var
  mfwStrncpy((char*)prov,(char*)call->name, MFW_TAG_LEN);
	//read status from MFW
  p = cm_status(call_number,call, 1);
  TRACE_EVENT_P4("CID:%d,orig name:%s new name: %s, Call number: %s", prov, call->name, prov, call->number);
	//if number alphabetically  ahead of orig name
  if (strcmp((char*)call->number,(char*)prov))
  mfwStrncpy((char*)call->name,(char*)prov, MFW_TAG_LEN);	//copy orig name to structure
#endif
  return p;

}

#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:     exeSendMessage

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


int exeSendMessage(MfwMnu* m, MfwMnuItem* i)
{
  return 0;
}


/*******************************************************************************

 $Function:     callExeNameEnter

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static int callExeNameEnter(MfwMnu* m, MfwMnuItem* i){
  return 0;
}

#endif

/*******************************************************************************

 $Function:     callFailureCause

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

#ifdef FF_2TO1_PS
static void callFailureCause(UBYTE cause)
{

  TRACE_FUNCTION("callFailureCause()");

  switch(cause)
  {
    case M_CC_CAUSE_UNASSIGN:
    case M_CC_CAUSE_BARRED:
    case M_CC_CAUSE_NUM_FORMAT:
    case M_CC_CAUSE_BARRED_IN_CUG:
    case M_CC_CAUSE_USER_NOT_IN_CUG:
/* RAVI - 20-1-2006 */		
/* Silent Implementation */		
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(), 
	                                  AUDIO_PLAY_ONCE);
#else		
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to above causes. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
#endif
/* END RAVI */
		bookShowInformation(idle_get_window(),TxtCheckNumber,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_USER_BUSY:
/* RAVI - 20-1-2006 */		
/* Silent Implementation */		
#ifdef NEPTUNE_BOARD
     audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(),
                                       AUDIO_PLAY_THRICE);
#else
/*  Jun 30, 2004  REF: CRR 15685 xkundadu(Sasken) */
/*  Play the beep sound if the called party is busy. */
     audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_THRICE);
#endif
/* END RAVI */
      bookShowInformation(idle_get_window(),TxtNumberBusy,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_NO_RESPONSE:
    case M_CC_CAUSE_ALERT_NO_ANSWER:
    case M_CC_CAUSE_CALL_REJECT:
      bookShowInformation(idle_get_window(),TxtNoAnswer,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_NUM_CHANGED:
      bookShowInformation(idle_get_window(),TxtNumberChanged,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_FACILITY_REJECT:
    case M_CC_CAUSE_STATUS_ENQUIRY:
    case M_CC_CAUSE_QOS_UNAVAIL:

    case M_CC_CAUSE_BEARER_CAP_UNAVAIL:
    case M_CC_CAUSE_SERVICE_UNAVAIL:
    case M_CC_CAUSE_BEARER_NOT_IMPLEM:
    case M_CC_CAUSE_ACM_MAX:
    case M_CC_CAUSE_FACILITY_NOT_IMPLEM:
    case M_CC_CAUSE_SERVICE_NOT_IMPLEM:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE);	
#else		
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to above causes. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
#endif
/* END RAVI */
		bookShowInformation(idle_get_window(),TxtOperationNotAvail,NULL,(T_VOID_FUNC)call_failure_cb);
      
    break;
    case M_CC_CAUSE_NO_CHAN_AVAIL:
    case M_CC_CAUSE_TEMP_FAIL:
    case M_CC_CAUSE_SWITCH_CONGEST:
    case M_CC_CAUSE_REQ_CHAN_UNAVAIL:
		
      bookShowInformation(idle_get_window(),TxtNetworkBusy,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_FACILITY_UNSUBSCRIB:
    case M_CC_CAUSE_BEARER_CAP_AUTHORIZ:
      bookShowInformation(idle_get_window(),TxtNotSubscribed,NULL,(T_VOID_FUNC)call_failure_cb);
    break;

		//GW-SPR#1035 - Added SAT changed
		/*NM, 110702
		  the "202" stands for timer 303 expired;
		  unfortunately there is no define for that in the SAP/MNCC */
		case 202:
			bookShowInformation(idle_get_window(),TxtCallTimeout,NULL,(T_VOID_FUNC)call_failure_cb);
		break;
		/*NM, 110702 END*/
    default:
    /* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(),
                                                AUDIO_PLAY_ONCE);
#else
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to any other reason. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
#endif
/* END RAVI */
		bookShowInformation(idle_get_window(),TxtOperationNotAvail,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
  }
  return;
}

#else /* FF_2TO1_PS */
static void callFailureCause(UBYTE cause)
{

  TRACE_FUNCTION("callFailureCause()");

  switch(cause)
  {
    case M_CC_CAUSE_UNASSIGN:
    case M_CC_CAUSE_BARRED:
    case M_CC_CAUSE_NUM_FORMAT:
    case M_CC_CAUSE_BARRED_IN_CUG:
    case M_CC_CAUSE_USER_NOT_IN_CUG:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(),
                                                AUDIO_PLAY_ONCE);
#else
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to above causes. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
#endif
/* END RAVI */
		bookShowInformation(idle_get_window(),TxtCheckNumber,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_USER_BUSY:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(), AUDIO_PLAY_THRICE);
#else
/*   Jun 30, 2004  REF: CRR 15685 xkundadu(Sasken) */
/*  Play the beep sound if the called party is busy. */
     audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_THRICE);
#endif
/* END RAVI*/
      bookShowInformation(idle_get_window(),TxtNumberBusy,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_NO_RESPONSE:
    case M_CC_CAUSE_ALERT_NO_ANSWER:
    case M_CC_CAUSE_CALL_REJECT:
      bookShowInformation(idle_get_window(),TxtNoAnswer,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_NUM_CHANGED:
      bookShowInformation(idle_get_window(),TxtNumberChanged,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_FACILITY_REJECT:
    case M_CC_CAUSE_STATUS_ENQUIRY:
    case M_CC_CAUSE_QOS_UNAVAIL:

    case M_CC_CAUSE_BEARER_CAP_UNAVAIL:
    case M_CC_CAUSE_SERVICE_UNAVAIL:
    case M_CC_CAUSE_BEARER_NOT_IMPLEM:
    case M_CC_CAUSE_ACM_MAX:
    case M_CC_CAUSE_FACILITY_NOT_IMPLEM:
    case M_CC_CAUSE_SERVICE_NOT_IMPLEM:
/* RAVI - 20-1-2006 */		
/* Silent Implementation */		
#ifdef NEPTUNE_BOARD
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(),
                                                AUDIO_PLAY_ONCE);
#else
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to above causes. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
#endif
/* END RAVI */
		bookShowInformation(idle_get_window(),TxtOperationNotAvail,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_NO_CHAN_AVAIL:
    case M_CC_CAUSE_TEMP_FAIL:
    case M_CC_CAUSE_SWITCH_CONGEST:
    case M_CC_CAUSE_REQ_CHAN_UNAVAIL:
      bookShowInformation(idle_get_window(),TxtNetworkBusy,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
    case M_CC_CAUSE_FACILITY_UNSUBSCRIB:
    case M_CC_CAUSE_BEARER_CAP_AUTHORIZ:
      bookShowInformation(idle_get_window(),TxtNotSubscribed,NULL,(T_VOID_FUNC)call_failure_cb);
    break;

		//GW-SPR#1035 - Added SAT changed
		/*NM, 110702
		  the "202" stands for timer 303 expired;
		  unfortunately there is no define for that in the SAP/MNCC */
		case 202:
			bookShowInformation(idle_get_window(),TxtCallTimeout,NULL,(T_VOID_FUNC)call_failure_cb);
		break;
		/*NM, 110702 END*/
    default:
    /* RAVI - 20-1-2006 */
    /* Silent Implementation */
 #ifdef NEPTUNE_BOARD
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, getCurrentVoulmeSettings(),
                                                AUDIO_PLAY_ONCE);
 #else
		/*  Oct 07, 2004  REF: CRR 23873 xnkulkar(Sasken) */
		/*  Play the Error tone if the call failure is due to any other reason. */
		audio_PlaySoundID( AUDIO_SPEAKER, TONES_BUSY, 0 , AUDIO_PLAY_ONCE);
 #endif
 /* END RAVI */
		bookShowInformation(idle_get_window(),TxtOperationNotAvail,NULL,(T_VOID_FUNC)call_failure_cb);
    break;
  }
  return;
}

#endif /* !FF_2TO1_PS */

/*******************************************************************************

 $Function:     sendDTMFString

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

static void sendDTMFString(void)
{


  call_data.sendingDTMF = TRUE;
  callSetMode(ViewSendingDTMF, TRUE);
  if (call_data.charSent<strlen(call_data.edtBufDTMF)){
    cm_command(call_data.calls.selected,CM_DTMF_AUTO,(UBYTE)call_data.edtBufDTMF[call_data.charSent]);
    call_data.current_command = CM_DTMF_AUTO;
  }
  else
  {
    call_data.charSent = 0;
    call_data.sendingDTMF = FALSE;
    callSetMode(ViewConnect, TRUE);
  }
}


#if(0) /* x0039928 - Commented for lint warning removal */
/*******************************************************************************

 $Function:     callCalcCost

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


static void callCalcCost (char* line)
{
    char line2[MAX_LINE];
    char *pdest,*src,*dst,*POINT;
    LONG result;
    int  ch = '.';
    UBYTE counter=0;

    memset(line2,'\0',sizeof(line2));

    cm_get_aoc_value(CM_AOC_CCM,&call_data.aocInfo);  /* Get CCM  */
    cm_get_aoc_value(CM_AOC_PUCT,&call_data.aocInfo); /* Get currency and price per unit */

    /* Search for '.' */
    pdest = strchr((char*) call_data.aocInfo.ppu, ch );
    if( pdest EQ NULL )                              /* '.' not founded  */
    {
        result = atol((char*)call_data.aocInfo.ppu );          /* STRING => LONG   */
        result= result *  call_data.aocInfo.ccm;               /* CCM * PPU        */
        sprintf(line, "%ld %s", result,call_data.aocInfo.cur); /* LONG => STRING   */
    }
    else                                             /* '.' found  */
    {
       /*
        * '.' found, *pdest EQ '.'
        */
        counter = strlen(pdest + 1); /* store the decimal place */

        src = (char*)call_data.aocInfo.ppu;
        dst = line;
        do                           /* remove the '.'   */
        {
            if (*src NEQ '.')
            {
                *dst++ = *src;
            }
        } while (*src++ NEQ '\0');
                                             /* STRING => LONG   */
        result= atol(line) *  call_data.aocInfo.ccm;   /*  CCM * PPU       */
        sprintf(line, "%ld", result);        /* LONG => STRING   */

        if(strlen(line) < counter)
        {
            src = line;
            dst = line2;
            *dst++ = '0';
            *dst++ = '.';
            counter = counter - strlen(line);
            do                               /* fill up with '0'   */
            {
                *dst++ = '0';
            } while (--counter NEQ 0);

            memcpy (dst,src,sizeof(src));
            sprintf(line, "%s %s", line2,call_data.aocInfo.cur);     /* add the currency */
            return;
        }
        src = line;
        dst = line2;
        POINT= src + strlen(src) - counter;
        do                                  /* set the '.'   */
        {
            if (src EQ POINT)
            {
                *dst++ = '.';
            }
            *dst++ = *src++;

        } while (*src NEQ '\0');
        if (line2[0] EQ '.')
            sprintf(line, "0%s %s", line2,call_data.aocInfo.cur);    /* add the currency */
        if (line2[0] NEQ '.')
            sprintf(line, "%s %s", line2,call_data.aocInfo.cur);     /* add the currency */
    }
    return;
}
#endif

/*******************************************************************************

 $Function:     call_create

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/




T_MFW_HND call_create (T_MFW_HND parent_window){

    T_call        * data = &call_data;
    T_MFW_WIN     * win;

  TRACE_FUNCTION("call_create()");

  data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)call_win_cb);

    if (data->win EQ 0)
      return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)call;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
    * Create any other handler
    */
  memset(data->phbNameNumber,'\0',sizeof(data->phbNameNumber));

    data->kbd      = kbd_create (data->win,KEY_ALL,(T_MFW_CB)call_kbd_cb);
    data->kbd_long = kbd_create (data->win,KEY_ALL|KEY_LONG,(T_MFW_CB)call_kbd_long_cb);

//x0pleela 07 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
  	if( phlock_win_handle && ( call_data.call_direction == MFW_CM_MTC  ) )
	{
		phlock_mtc_win_handle = data->win;
		
		kbd_delete(data->kbd);
		data->kbd = phlock_kbd_handle; //set the kbd handle of unlock screen
	}
#endif //FF_PHONE_LOCK

  data->cm = cm_create(data->win, E_CM_ALL_SERVICES, (MfwCb)callCmEvent);

  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr(&data->editorAttrCall,CALL_EDITOR,COLOUR_EDITOR_XX,EDITOR_FONT, ED_MODE_ALPHA, ED_CURSOR_BAR,ATB_DCS_ASCII, (UBYTE*)data->phbNameNumber,EDITOR_SIZE);
    data->editorCall = ATB_edit_Create(&data->editorAttrCall,0);
#else /*NEW_EDITOR*/
	bookSetEditAttributes(CALL_EDITOR,COLOUR_EDITOR_XX,0,edtCurBar1,0,(char*)data->phbNameNumber,EDITOR_SIZE,&data->edtAttrCall);
    data->editCall = edtCreate(data->win,&data->edtAttrCall,0,0);
#endif /* NEW_EDITOR */
  SEND_EVENT(data->win,CALL_INIT,0,0);
//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
if( ( !phlock_win_handle) || (mmiPinsEmergencyCall()) )
#endif //FF_PHONE_LOCK	
	winShow(data->win);
//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
else
{
	//show phone unlock window		
	winShow(phlock_win_handle );
}
#endif //FF_PHONE_LOCK

  /*
   * return window handle
   */
  return data->win;



}


/*******************************************************************************

 $Function:     call_destroy

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_destroy (T_MFW_HND own_window){

  T_MFW_WIN     * win;
  T_call        * data;

  TRACE_FUNCTION("call_destroy()");


  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_call *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */

#ifdef FF_MMI_AUDIO_PROFILE
	if( mfw_get_current_audioDevice() == MFW_AUD_LOUDSPEAKER)
	{
		mfw_audio_set_device(MFW_AUD_HANDHELD);
	}
#endif
	
    if (call_data.win_calling){
      SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
      call_data.win_calling = 0;
    }
    if (call_data.win_ending){
      SEND_EVENT(call_data.win_ending,DIALOG_DESTROY,0,0);
      call_data.win_ending = 0;
    }
    if (call_data.win_menu){
      call_menu_destroy(call_data.win_menu);
      call_data.win_menu = 0;
    }
    if (call_data.win_waiting){
      SEND_EVENT(call_data.win_waiting,DIALOG_DESTROY,0,0);
      call_data.win_waiting = 0;
    }
    if (call_data.win_sending_dtmf){
      SEND_EVENT(call_data.win_sending_dtmf,DIALOG_DESTROY,0,0);
      call_data.win_sending_dtmf = 0;
    }

	if (call_data.win_incoming)
	{
		return;
	}

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	/* New editor not destroyed automatically by win_delete,
	 * so do so here */
	if (call_data.editorCall)
	{
		ATB_edit_Destroy(call_data.editorCall);
		call_data.editorCall = 0;
	}
#endif /* NEW_EDITOR */

/*mc, spr 1392, destroy all call deflection windows*/

	if (call_data.win_deflectSearch)
	{
		bookPhonebookDestroy(call_data.win_deflectSearch);
		call_data.win_deflectSearch = 0;
	}

	if (call_data.win_deflectEdit)
	{
		/* SPR#1428 - SH - New editor */
	#ifdef NEW_EDITOR
		AUI_edit_Destroy(call_data.win_deflectEdit);
	#else /* NEW_EDITOR */
		editor_destroy(call_data.win_deflectEdit);
	#endif /* NEW_EDITOR */

		call_data.win_deflectEdit = 0;
	}

	if (call_data.win_deflecting)
	{
		SEND_EVENT(call_data.win_deflecting, DIALOG_DESTROY, NULL, NULL);
		call_data.win_deflecting = 0;
	}

    win_delete (data->win);
    data->win=0;

	//x0pleela 07 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	if((  call_data.call_direction == MFW_CM_MTC )&&  phlock_mtc_win_handle )
	{
		 phlock_mtc_win_handle = NULL;
	}
#endif //FF_PHONE_LOCK
    }

    idleEvent(IdleUpdate);/* NDH SPR 1584, update idle screen*/

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
  iIncomingCall = 0;
#endif
#endif
  }
}

/*******************************************************************************

 $Function:     	deflect_destroy

 $Description:  	destroys call deflection windows and, depending upon the value of the
 					parameter delete_call_windows, deletes the call windows if there is no
 					currently active call.

 $Returns:			nothing

 $Arguments:		Boolean delete_call_windows, should be set to TRUE if call winodws
 					ought to be deleted too.
*******************************************************************************/
void deflect_destroy(BOOL delete_call_windows)
{
/*destroy all call deflection windows*/

	if (call_data.win_deflectSearch)
	{
		bookPhonebookDestroy(call_data.win_deflectSearch);
		call_data.win_deflectSearch = 0;
	}

	if (call_data.win_deflectEdit)
	{
		/* SPR#1428 - SH - New editor */
	#ifdef NEW_EDITOR
		AUI_edit_Destroy(call_data.win_deflectEdit);
	#else /* NEW_EDITOR */
		editor_destroy(call_data.win_deflectEdit);
	#endif /* NEW_EDITOR */

		call_data.win_deflectEdit = 0;
	}

	if (call_data.win_deflecting)
	{
		SEND_EVENT(call_data.win_deflecting, DIALOG_DESTROY, NULL, NULL);
		call_data.win_deflecting = 0;
	}

	/*set incoming call number to 0, as call deflection has either failed or succeeded*/
	call_data.incCall = 0;

 /*destroy call window if no active call and flag TRUE*/
  if (!call_data.calls.numCalls && delete_call_windows == TRUE)
  {        	if (call_data.win)
            {
                call_destroy(call_data.win);
             }
  }

}


/*******************************************************************************

 $Function:     call

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_call         * data = (T_call *)win_data->user;
    int i;


    TRACE_FUNCTION ("call()");

    switch (event)
  {
    case CALL_INIT:
    {


      memset(&data->calls,0,sizeof(data->calls));
	  memset(call_data.colp_number,'\0',sizeof(call_data.colp_number));
      phb_set_auto_off(PHB_AUTO_ON);
      data->win_incoming=0;
      data->win_calling=0;
      data->win_menu=0;
      data->win_waiting=0;
      data->calls.numCalls = 0;
      data->accessEnd = TRUE;
      data->callWaitingEnd = 0;
      data->emergencyCall = FALSE;
      data->callsToEnd = 0;
      data->incCall = 0;
      data->callStatus = CallInactive;
      data->outCall=0;
      data->call_direction=MFW_CM_INV_DIR;
      data->refreshConnect = 0;
      data->firstChar=FIRST_TYPED;
      data->currentVolume=MAX_SOUND;
      data->endQueue = 0;
      data->singleCall = 0;
      data->charSent=0;
      data->sendingDTMF=FALSE;
      data->ignore_disconnect = FALSE;

	for (i=0; i<MAX_CALLS; i++)	call_data.calls.status[i].call_direction = MFW_CM_INV_DIR;

	  //GW Ensure a valid (non-zero) number for currentVolume
	  /*if (FFS_flashData.output_volume > MAX_SOUND)
	 	FFS_flashData.output_volume = MAX_SOUND;
	  else if (FFS_flashData.output_volume <= 0)
		FFS_flashData.output_volume = MAX_SOUND;*//*a0393213 lint warning removal - commented as the statement as no effect*/
// Apr 05, 2005    REF: ENH 29994 xdeepadh					  
//Commented mmesetvolume.since the speaker volume will be set, 
//once the call is acknowledged.
//mmeSetVolume (1, FFS_flashData.output_volume);
//x0pleela 27 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
if ( (!phlock_win_handle) || (mmiPinsEmergencyCall()) )
#endif //FF_PHONE_LOCK
      winUnhide(call_data.win);
    }
    break;

    case CALL_DESTROY_WAITING:
      if (call_data.win_waiting){
        SEND_EVENT(call_data.win_waiting,DIALOG_DESTROY,0,0);
        call_data.win_waiting = 0;
        if (!call_data.calls.numCalls)
          call_destroy(call_data.win);
      }
    break;
    case CALL_DESTROY_CALLING_WINDOW:
    	/* SPR#1983 - SH - This is a special WAP event.
    	 * It destroys the "Calling" dialog if WAP connection
    	 * is cancelled. */

	    TRACE_EVENT("CALL_DESTROY_CALLING_WINDOW");

	    if (call_data.win_calling)
	    {
	        SEND_EVENT(call_data.win_calling,DIALOG_DESTROY,0,0);
	        call_data.win_calling = 0;
	    }

	    if (call_data.outCall)
	    {
	        cm_disconnect(call_data.outCall);
			call_data.outCall = 0;
	        if (!call_data.calls.numCalls)
	        {
	          call_data.ignore_disconnect = TRUE;
	          call_destroy(call_data.win);
	          return;
	      	}
	    	endingCall();
	    }
    	break;

        case E_CM_NOTIFY:
      /*
       * Show all cc related notifications
       */
      showCCNotify( win, (T_MFW_CM_NOTIFY*) parameter );
      break;
	    case E_CM_COLP:
	    {
			/* NM 056, FTA Testcase 31.1.3.1
			 * add COLP only for the singlecall and have
			 * to be done for multicall as well !
			 */

	    	T_MFW_CM_MO_INFO *colp	= (T_MFW_CM_MO_INFO*) parameter ;

      		TRACE_EVENT ("E_CM_COLP");

			if (strlen((char*)colp->number) > 0)
			{
				memset(call_data.colp_number,'\0',sizeof(call_data.colp_number));
				strncpy ((char*)call_data.colp_number , (char*)colp->number, sizeof(call_data.colp_number)-1);
			}
	    }
	    break;
    case CALL_OUTGOING_SAT:
                    {
      /* this means the SIM toolkit is about to start a call - this means we will
       * shortly receive E_CM_MO_RES from MFW, so we do some initialisation of    data
       * parameters before we receive this event
       */

         T_SAT_call_setup_parameter * call_setup_parameter = (
    T_SAT_call_setup_parameter *)parameter;

        char* tempString; /* Temporary string holder */

            TRACE_EVENT ("CALL_OUTGOING_SAT");

      /* if this is the first call then initialise the window data */
                        if (call_data.calls.numCalls == 0)
                            SEND_EVENT(call_data.win,CALL_INIT,0,0);

      /* set the outcall ID */
      call_data.outCall = call_setup_parameter->callId;
      call_data.call_direction = MFW_CM_MOC;

      /* set the display string from the SIM toolkit label */

      /* SPR#1700 - DS - Use TextString2 (second alpha id) if available. Else, use TextString (first alpha id).
       * This conforms with GSM SAT spec 11.14 section 6.4.13
       */

     if (call_setup_parameter->TextString2 NEQ NULL) /* Use second alpha id */
     {
        TRACE_EVENT("Use second alpha id: TextString2");
        tempString = call_setup_parameter->TextString2;
  //x0035544 Mar 11, 2006, DR:OMAPS00061467
  #ifdef FF_MMI_SAT_ICON
	if(call_setup_parameter->IconInfo2.dst != NULL)
	{
	//Copy the icon data for the second alpah ID
		call_data.IconData.width 	=  call_setup_parameter->IconInfo2.width;
	  	call_data.IconData.height	=  call_setup_parameter->IconInfo2.height;		
		call_data.IconData.dst = call_setup_parameter->IconInfo2.dst;
		call_data.IconData.selfExplanatory 	=  call_setup_parameter->IconInfo2.selfExplanatory;
		TRACE_EVENT_P2("icon width=%d, icon height= %d---sudha",call_data.IconData.width,call_data.IconData.height);
	}
	else
	{
		call_data.IconData.dst = NULL;
		call_data.IconData.width = 0;
		call_data.IconData.height = 0;
		call_data.IconData.selfExplanatory = FALSE;
	}
   #endif	
     }
     else /* Second alpha id not set therefore use first alpha id */
     {
        TRACE_EVENT("Use first alpha id: TextString");
        tempString = call_setup_parameter->TextString;
  //x0035544 Mar 11, 2006, DR:OMAPS00061467
  #ifdef FF_MMI_SAT_ICON	
	if(call_setup_parameter->IconInfo.dst != NULL)
	{
	//copy the icon data of first alpha ID
		call_data.IconData.width 	=  call_setup_parameter->IconInfo.width;
	  	call_data.IconData.height	=  call_setup_parameter->IconInfo.height;		
		call_data.IconData.dst =  call_setup_parameter->IconInfo.dst;
		call_data.IconData.selfExplanatory 	=  call_setup_parameter->IconInfo.selfExplanatory;
	}
	else
	{
		call_data.IconData.dst = NULL;
		call_data.IconData.width = 0;
		call_data.IconData.height = 0;
		call_data.IconData.selfExplanatory = FALSE;
	}
   #endif	
     } 

     if (tempString NEQ NULL)
    {
        uint32 len; /*a0393213 lint warning - type changed from int to uint32*/

        /*
        * SPR#1700 - DS - Copy string according to string encoding
        */
        if ( *(tempString) == (char) 0x80) /* Unicode */ /*a0393213 lint warnings removal - typecasting done*/
        {
            /* Count number of bytes in SAT label */
            len = 2*ATB_string_UCLength((USHORT*)tempString);

            if (sizeof(call_data.phbNameNumber) < len)
            {
                len = 2*(sizeof(call_data.phbNameNumber)/2);/*a0393213 lint warnings removal - to make len an even number*/
            }

            TRACE_EVENT_P1("Unicode SAT label is %d bytes", len);

            memcpy( (void *) call_data.phbNameNumber, (void *)tempString, len-2);/*a0393213 lint warnings removal - changed len to len-2*/

            /* Make sure the string is null terminated */
            call_data.phbNameNumber[len-2]   = 0x00;/*a0393213 lint warnings removal - changed len to len-2*/

            /* Unicode strings need two zero bytes to terminate */
            call_data.phbNameNumber[len-1] = 0x00;/*a0393213 lint warnings removal - changed len to len-1*/
        }
        else /* Ascii */
        {
        len = strlen(tempString);

        if (sizeof(call_data.phbNameNumber) < len)
        {
            len = sizeof(call_data.phbNameNumber);
        }

        TRACE_EVENT_P1("Ascii SAT label is %d bytes", len);

        strncpy(call_data.phbNameNumber, tempString, len-1);/*a0393213 lint warnings removal - changed len to len-1*/
	  call_data.phbNameNumber[len-1]=0x00;/*a0393213 lint warnings removal - added this statement*/
        }  		
    }
    else
    {
        TRACE_EVENT("tempString is NULL - No SAT label!");
        call_data.phbNameNumber[0] = '\0';
    }

      call_SATCall = TRUE;

         }
  	  break;
    /* SPR#1428 - SH - Insert phone number into new editor */
	/*MC, SPR 1392, phone number selected for call deflection*/
	case DEFLECT_PHBK_NUMBER:
	#ifdef NEW_EDITOR
		{
			T_ATB_TEXT text;
			text.dcs = ATB_DCS_ASCII;

			/*API - 25/08/03 - SPR2144 - Remove the call to pass call_data.editor_buffer, instead pass down parameter*/
			text.data = (UBYTE *)parameter;
			/*API - 25/08/03 - SPR 2144 - END*/

			text.len = ATB_string_Length(&text);
			SEND_EVENT(call_data.win_deflectEdit, E_ED_INSERT, 0, (void *)&text);
		}
	#else /* NEW_EDITOR */
		/*copy number to editor*/
		memcpy (call_data.editor_buffer , (char *)parameter, sizeof(call_data.editor_buffer));
		SEND_EVENT(call_data.win_deflectEdit, E_EDITOR_CURSOR_END, NULL, NULL);
	#endif /* NEW_EDITOR */

		/*Set search window pointer to NULL as we've just exited that window*/
		call_data.win_deflectSearch = 0;
		break;

    default:
    break;
  }

}

int lines_inBetween_Display(int noCalls, int callNo)
{
	switch(noCalls)
		{
			case 1:
			case 2:
				return (1+(callNo*5));
		//	break;   // RAVI

			case 3:
				return (0+(callNo*4));
		//	break;  // RAVI

			case 4:
				return (0+(callNo*3));
		//	break;  // RAVI

			case 5:
			case 6:
				return (0+(callNo*2));
		//	break;  // RAVI

			default:
				/* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function) */
				/* break; */
				return 0;
		}
}

// API - SPR 1775 - Conference call, added to place a arrow next to the call looking at!
int multiCall_ActiveCall (int bitmap_pos, int CurrentCallNo)
{
	t_font_bitmap  multicall_pointer = { 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)multi_pointer};
	int xPos, yPos;
	int noActCalls, nLines;

	noActCalls = call_data.calls.numCalls;

	TRACE_EVENT("multiCall_ActiveCall");
	TRACE_EVENT_P2("CurrentCallNo = %d, noActCalls = %d", CurrentCallNo, noActCalls);

	xPos = 5;
	nLines = lines_inBetween_Display(noActCalls, CurrentCallNo);

	TRACE_EVENT_P1("nLines = %d", nLines);

	switch(bitmap_pos)
	{
		case CALL_STATUS_CALL :
			if(noActCalls <= 2)
				nLines = nLines;
			else if(noActCalls == 3)
				nLines = nLines;
			else if(noActCalls >= 4)
					nLines = nLines;
		break;

		default:
			xPos = 5;
			yPos = nLines;
		break;
	}
	yPos = Mmi_layout_IconHeight() + (nLines * Mmi_layout_line_height());

	TRACE_EVENT_P2("xPos = %d, yPos = %d", xPos, yPos);

	dspl_show_bitmap(xPos, yPos, &multicall_pointer, 0 );

	/* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function) */
	return TRUE;
}

/*MC, please note: it is assumed that txtStr will have a unicode tag at the beginning
if it ought to be displayed in Unicode, otherwise, it *will* be displayed in ASCII*/
int inCall_displayData( int dataId , int txtId, char* txtStr, int CurrentCallNo)
{
	int xPos, yPos;
	int txtFormat;
	int lenStr, lenId, txtWidth, nLines;
	int noActCalls;   // statusWidth;  // RAVI
	char  idStr[PHB_MAX_LEN];
	/*MC, SPR 1319*/
	UBYTE unicode_string =0;/*Unicode flag*/
	UBYTE current_display_type = dspl_get_char_type();/*store current char type*/
	noActCalls = call_data.calls.numCalls;
	//noActCalls = 4;


TRACE_EVENT_P4("inCall_displayData %d %d %s %d",dataId,txtId,txtStr,CurrentCallNo);

	if (txtStr != NULL)
	{
/*MC SPR 1319*/
#ifdef NO_ASCIIZ
		/*set char type so correct text extent calculated*/
		if (txtStr[0] ==(char)0x80)/*if unicode tag*//*a0393213 lint warnings removal - typecasting done*/
		{
			dspl_set_char_type(DSPL_TYPE_UNICODE);
		}
		else
		{
			dspl_set_char_type(DSPL_TYPE_ASCII);
		}
#endif
/*mc end*/
		lenStr = dspl_GetTextExtent( txtStr, 0);
	}
	else
		lenStr = 0;

	if(txtId != TxtNull)
	{
		memcpy(idStr, MmiRsrcGetText(txtId), PHB_MAX_LEN);
		lenId = dspl_GetTextExtent( idStr, 0);
		/*MC, unicode string if in chinese*/
		if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			unicode_string = 1;
	}
	else
	{
		lenId = 0;
/*MC SPR 1319*/
	#ifdef NO_ASCIIZ
		/*MC*/
		if ((txtStr != NULL) && (txtStr[0] ==(char)0x80))/*if unicode tag*//*a0393213 lint warnings removal - typecasting done*/
		{	char debug[25];
			int i;

			dspl_set_char_type(DSPL_TYPE_UNICODE);
			//idStr = txtStr;
			 resources_truncate_to_screen_width(txtStr,0, idStr, PHB_MAX_LEN,
	SCREEN_SIZE_X, TRUE);
			unicode_string = 1;/*set unicode flag*/

			for(i=0; i< 25;i++)
			{ if (txtStr[i] == NULL)
					debug[i] = '0';
				else
					debug[i] = txtStr[i];
			}


			TRACE_EVENT_P1("Call Name string:%s", debug);
		}
		else
#endif
/*MC end*/
		{	//idStr = txtStr;
		resources_truncate_to_screen_width(txtStr,0, idStr, PHB_MAX_LEN,
	SCREEN_SIZE_X, FALSE);
			dspl_set_char_type(DSPL_TYPE_ASCII);
		}
	}

	txtWidth = lenStr+lenId;

#ifndef LSCREEN
	xPos = 0;
	yPos = 0;
	txtFormat = 0;

	switch (dataId)
	{
		case CALL_STATUS_CALL :
			xPos = 8;
			if (CurrentCallNo == 0)
				yPos = Mmi_layout_line(1);
			else
				yPos = (Mmi_layout_line(1) + LINE_HEIGHT*CurrentCallNo);
		break;

		case CALL_TIMER_CALL :
			xPos = INCALLTIMER_X;
			if (CurrentCallNo == 0)
				yPos = Mmi_layout_line(2);
			else
				yPos = (Mmi_layout_line(1) + LINE_HEIGHT*CurrentCallNo);
		break;

		case CALL_ID_NAME_CALL :
			xPos = CALLID_NUM_XPOS;
			if (CurrentCallNo == 0)
				yPos = Mmi_layout_line(1);
			else
				yPos = (Mmi_layout_line(1) + LINE_HEIGHT*CurrentCallNo);
		break;

		case CALL_ID_NUM_CALL :
			xPos = CALLID_NUM_XPOS;
			if (CurrentCallNo  == 0)
				yPos = Mmi_layout_line(1);
			else
				yPos = (Mmi_layout_line(1) + LINE_HEIGHT*CurrentCallNo);
		break;

		default:
		break;
	}
#else

	txtFormat = 0;
	xPos = ((SCREEN_SIZE_X - txtWidth)/2);
	nLines = lines_inBetween_Display(noActCalls, CurrentCallNo);

	switch (dataId)
	{
		case CALL_STATUS_CALL :
			if(noActCalls <= 2)
				nLines = nLines;
			else if(noActCalls == 3)
				nLines = nLines;
			else if(noActCalls >= 4)
				{
					xPos = 5;
					nLines = nLines;
				}
		break;

		case CALL_ID_NAME_CALL :
			if(noActCalls <= 2)
			{
			#ifdef FF_CPHS
				nLines=nLines+2;
			#else
				nLines = nLines + 1;
			#endif
			}
			else if(noActCalls == 3)
			{
			#ifdef FF_CPHS
				nLines=nLines+1;
			#else
				nLines = nLines;
			#endif
			}
			else if(noActCalls >= 4)
			{
			#ifdef FF_CPHS
				nLines= nLines+2;
			#else
				nLines = nLines + 1;
			#endif
			}
		break;

		case CALL_ID_NUM_CALL :
			if(noActCalls <= 2)
			{
			#ifdef FF_CPHS
				nLines = nLines + 3;
			#else
				nLines = nLines + 2;
			#endif
			}
			else if(noActCalls == 3)
			{
			#ifdef FF_CPHS
				nLines = nLines + 2;
			#else
				nLines = nLines + 1;
			#endif
			}
			else if(noActCalls >= 4)
			{
			#ifdef FF_CPHS	
				nLines = nLines + 2;
			#else
				nLines = nLines + 1;
			#endif
			}
		break;

		case CALL_TIMER_CALL :
			if(noActCalls <= 2)
			{
			#ifdef FF_CPHS
				nLines = nLines + 4;
			#else
				nLines = nLines + 3;
			#endif
			}
			else if(noActCalls == 3)
			{
			#ifdef FF_CPHS
				nLines = nLines + 3;
			#else
				nLines = nLines + 2;
			#endif
			}
			else if(noActCalls >= 4)
			{
				xPos = (SCREEN_SIZE_X - 5) - dspl_GetTextExtent( idStr, 0);
			#ifdef FF_CPHS	
				nLines = nLines + 1;
			#else
				nLines = nLines;
			#endif
			}
		break;

#ifdef FF_CPHS
		case CALL_ACTIVE_LINE:
			nLines = nLines + 1;
			break;
#endif
		default:
		break;
	}
yPos = Mmi_layout_IconHeight() + (nLines * Mmi_layout_line_height());

#endif
/*MC SPR 1319*/
#ifdef NO_ASCIIZ

if (unicode_string==1)
	dspl_TextOut(xPos, yPos, DSPL_TXTATTR_UNICODE, idStr);
else
#endif
	dspl_ScrText(xPos,	yPos,	idStr,	txtFormat);
/*MC SPR 1319, restore original char type*/
dspl_set_char_type(current_display_type);

   return 0; /* Added to remove warning Aug - 11 */
}


/*******************************************************************************

 $Function:     call_win_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/
//GW-SPR#???-Truncate phone numbers at the beginning (lose first x chars)

static int call_win_cb(MfwEvt e, MfwWin *w){
//	int j; // RAVI
    int i = 0;
    char line[/*MAX_LINE*/PHB_MAX_LEN];
	char tag[MAX_CHAR];
	char name[MAX_CHAR];
	int topLine;
	int alphaUsed=0;   /*a0393213 lint warnings removal - initialization done*/
	/* x0045876, 14-Aug-2006 (WR - "currentRinger" was set but never used) */
	char len;
	TRACE_FUNCTION("call_win_cb");
	
	switch (e)
	{
	case MfwWinVisible:
		memset(tag,'\0',sizeof(tag));
		memset(name,'\0',sizeof(name));
		TRACE_EVENT_P1("icon SMS is = %d", iconsGetState(iconIdSMS));
		//  Jun 11, 2004    REF: CRR 15634  Deepa M.D 
		//Check for unread SMS here and display the SMS icon accordingly.
		if (smsidle_get_unread_sms_available())
			iconsSetState(iconIdSMS);//Make the SMS Icon visible.
		else
			iconsDeleteState(iconIdSMS);//Make the SMS Icon invisible.
		/*iconsShow();*/  /* SPR#1699 - SH - Undo this change for now */
		showDefault();
		
		if (!call_data.calls.numCalls){
			displaySoftKeys('\0',TxtEnd);
			return 1;
		}

#ifdef FF_PHONE_LOCK		
			if( ( call_data.call_direction == MFW_CM_MTC ) && ( phlock_mtc_win_handle ) )
			{
				TRACE_EVENT("dialog_info_win_resize_cb: creating mtc kbd handler");
				call_data.kbd= kbd_create (phlock_mtc_win_handle, KEY_ALL, (T_MFW_CB)call_kbd_cb);
			}
#endif //FF_PHONE_LOCK				
		
		switch(call_data.calls.mode){			
		case CallSingle :
			if (call_data.calls.muted)
			{
				inCall_displayData( CALL_STATUS_CALL ,TxtMuted, NULL, 0);
#ifndef LSCREEN
				TRACE_EVENT("Call is Muted!");
#else
				inCall_displayData(CALL_ID_NAME_CALL, 0, name, 0);
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, 0);
//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
				if(call_data.call_direction==MFW_CM_MOC)
				{
  				  if(mmi_get_als_selectedLine()==MFW_SERV_LINE1)
  				  {
  					inCall_displayData(CALL_ACTIVE_LINE,TxtLine1,NULL, 0);
  				  }
  				  else if(mmi_get_als_selectedLine()==MFW_SERV_LINE2)
  				  {
  					inCall_displayData(CALL_ACTIVE_LINE,TxtLine2,NULL, 0);		
  				  }
				}
#endif /* FF_CPHS*/
#endif				
			}
			else
			{
				i = callIndex(call_data.calls.selected);
				/*a0393213 lint warnings removal - check for callIndex done*/
				if(i<0 || i>=MAX_CALLS)
				{
					TRACE_ERROR("call_win_cb(): Call index out of bounds");
					break;
				}
				alphaUsed = 0;
				
				if (call_data.calls.status[i].stat == CALL_ACTIVE)
				{
#ifndef LSCREEN
#ifdef MMI_TTY_ENABLED
					if (call_tty_get())
						inCall_displayData( CALL_STATUS_CALL, 0, "1TTY", 0);
					else
#endif
						inCall_displayData( CALL_STATUS_CALL ,0, "1.A.", 0);
#else
					/* SPR#1352 - SH - Make this a text ID, and check for TTY*/
#ifdef MMI_TTY_ENABLED
					if (call_tty_get())
						inCall_displayData( CALL_STATUS_CALL, TxtActiveTTYCall, 0, 0);
					else
#endif
						inCall_displayData( CALL_STATUS_CALL, TxtActiveCall, 0, 0);
#endif
				}
				else
				{
#ifndef LSCREEN
					inCall_displayData( CALL_STATUS_CALL ,0, "1.H.", 0);
#else
					/* SPR#1352 - SH - Make this a text ID, and check for TTY*/
#ifdef MMI_TTY_ENABLED
					if (call_tty_get())
						inCall_displayData( CALL_STATUS_CALL, TxtHeldTTYCall, 0, 0);
					else
#endif
						inCall_displayData( CALL_STATUS_CALL, TxtHeldCall, 0, 0);
#endif
				}
				/*mc SPR 1319*/
#ifdef NO_ASCIIZ
				if (call_data.calls.status[i].name.len > 0)
				{
					alphaUsed = TRUE;
					if	(call_data.calls.status[i].name.data[0] == 0x80)
					{	/*convert string from on-sim unicode to display format*/
						memset(name, 0, MAX_CHAR);
						/*make sure we don't go over the end of the name string*/
						if (call_data.calls.status[i].name.len-1 > (MAX_CHAR -2))
							len = MAX_CHAR-2;
						else
							len = call_data.calls.status[i].name.len-1;
						memcpy(&name[2], &call_data.calls.status[i].name.data[1], len);
						name[0] = (char)0x80;/*a0393213 lint warnings removal - char* removed*/
						name[1] = 0x7f;
					}
					else
						strncpy(name, (char*)call_data.calls.status[i].name.data,MAX_CHAR-1);
				}
#else
				if (strlen((char*)call_data.calls.status[i].name) > 0)
				{
					alphaUsed = TRUE;
					strncpy(name, &call_data.calls.status[i].name,MAX_CHAR-1 );
				}
				
#endif
				if (strlen((char*)call_data.calls.status[i].number) > 0)
				{
					
					TRACE_EVENT("output COLP number");
					/* NM,  Testcase 31.1.3.1 COLP,
					show the real connected number,
					for single call
					*/
					if (strlen((char*)call_data.colp_number) > 0)
					{
						strncpy(tag, (char*)call_data.colp_number, MAX_CHAR-1);
					}
					else
					{
						strncpy(tag, (char*)call_data.calls.status[i].number, MAX_CHAR-1);
					}
				}
				else
				{/*MC SPR 1319*/
#ifdef NO_ASCIIZ
					if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
					{ 	/*prepend string with unicode tag so that we know to display it as unicode*/
						memset(tag, 0, MAX_CHAR);
						tag[0] = (char)0x80;/*a0393213 lint warnings removal - typecasting done*/
						tag[1] = 0x7f;
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
						memcpy(&tag[2], GET_TEXT(TxtSoftCall), ATB_string_UCLength((USHORT*)GET_TEXT(TxtSoftCall))*sizeof(USHORT));
#else /* NEW_EDITOR */
						memcpy(&tag[2], GET_TEXT(TxtSoftCall), strlenUnicode((U16*)GET_TEXT(TxtSoftCall))-2);
#endif /* NEW_EDITOR */
						TRACE_EVENT_P1("TxtSoftCall: %s", tag);
					}
					else
#endif
						/*mc end*/				{
						memcpy(tag, GET_TEXT(TxtSoftCall), MAX_CHAR);
					}
				}
			}
			
			if(alphaUsed == TRUE)
			{
				alphaUsed = FALSE;
				inCall_displayData(CALL_ID_NAME_CALL, 0, name, 0);
#ifdef LSCREEN
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, 0);
#endif
			}
			else
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, 0);

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
			if(CphsPresent() && mmi_cphs_get_als_value())
			{
				
				TRACE_FUNCTION_P1("call_data.call_direction: %d", call_data.call_direction);
				if(call_data.call_direction==MFW_CM_MOC)
				{
					if(mmi_get_als_selectedLine()==MFW_SERV_LINE1)
						{
							inCall_displayData(CALL_ACTIVE_LINE,TxtLine1,NULL, 0);
						}
					else if(mmi_get_als_selectedLine()==MFW_SERV_LINE2)
						{
							inCall_displayData(CALL_ACTIVE_LINE,TxtLine2,NULL, 0);	
						}
					else
						inCall_displayData(CALL_ACTIVE_LINE,TxtNull,NULL, 0);	
				}
				else if(call_data.call_direction==MFW_CM_MTC)
				{	TRACE_FUNCTION_P1("call_data.call_direction: %d", call_data.call_direction);						 
						if( mfw_get_ALS_type() EQ VOICE_CALL)
							{
								inCall_displayData(CALL_ACTIVE_LINE,TxtLine1,NULL, 0);
							}
						else if(mfw_get_ALS_type() EQ AuxVOICE_CALL)
							{
								inCall_displayData(CALL_ACTIVE_LINE,TxtLine2,NULL, 0);			
							}
						else
							inCall_displayData(CALL_ACTIVE_LINE,TxtNull,NULL, 0);	
				}
			}
#endif   /* FF_CPHS */

			
			memset(line,'\0',sizeof(line));
			
			getTimeString(call_data.calls.time[i],(char*)line);
			
			if (InCallTimer == TRUE)
			{
				inCall_displayData(CALL_TIMER_CALL, 0, line, 0);
			}
			break;
	case CallTwo :
	case CallConference :
	default:
		
		if (call_data.calls.numCalls<MAX_CALLS)
			topLine = 2;
        else
			topLine = 1;
		
		for (i=0;i<call_data.calls.numCalls;i++)
        {
			
			/* x0045876, 14-Aug-2006 (WR - "currentRinger" was set but never used) */
			/* posY = Mmi_layout_line(topLine+i); */
			Mmi_layout_line(topLine+i);
			
			memset(line,'\0',sizeof(line));
			memset(name,'\0',sizeof(name));
			memset(tag,'\0',sizeof(tag));
			sprintf(line,"%d",i+1);
			
			if (call_data.calls.status[i].stat == CALL_ACTIVE)
			{
#ifndef LSCREEN
				/* SPR#1352 - SH - Check for TTY*/
#ifdef MMI_TTY_ENABLED
				if (call_tty_get())
					sprintf(&line[1],"%s","1TTY");//GW Extra ';' removed.
				else
#endif
					sprintf(&line[1],"%s","1.A.");
#else
				/* SPR#1352 - SH - Make this a text ID, and check for TTY*/
#ifdef MMI_TTY_ENABLED
				if (call_tty_get())
					inCall_displayData( CALL_STATUS_CALL, TxtActiveTTYCall, 0, i);
				else
#endif
					inCall_displayData( CALL_STATUS_CALL, TxtActiveCall, 0, i);

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
				if(call_data.call_direction==MFW_CM_MOC)
				{
					if(mmi_get_als_selectedLine()==MFW_SERV_LINE1)
						{
							inCall_displayData(CALL_ACTIVE_LINE, TxtLine1,NULL, i);
						}
					else if(mmi_get_als_selectedLine()==MFW_SERV_LINE2)
						{
							inCall_displayData(CALL_ACTIVE_LINE,TxtLine2,NULL, i);		
						}
				}	
#endif /* FF_CPHS */

#endif
			}
			else
			{
#ifndef LSCREEN
				sprintf(&line[1],"%s","1.H.");
#else
#ifdef MMI_TTY_ENABLED
				if (call_tty_get())
					inCall_displayData( CALL_STATUS_CALL, TxtHeldTTYCall, 0, i);
				else
#endif
					inCall_displayData( CALL_STATUS_CALL, TxtHeldCall, 0, i);
#endif
			}
			
#ifndef LSCREEN
			inCall_displayData( CALL_STATUS_CALL ,0, line, i);
#endif
			
			
			
#ifdef NO_ASCIIZ
			if (call_data.calls.status[i].name.len > 0)
			{
				if	(call_data.calls.status[i].name.data[0] == 0x80)
				{	/*convert string from on-sim unicode to display format*/
					memset(name, 0, MAX_CHAR);
					/*make sure we don't go over the end of the name string*/
					if (call_data.calls.status[i].name.len-1 > (MAX_CHAR -2))
						len = MAX_CHAR-2;
					else
						len = call_data.calls.status[i].name.len-1;
					memcpy(&name[2], &call_data.calls.status[i].name.data[1], len);
					name[0] = (char)0x80;/*a0393213 lint warnings removal - type casting done*/
					name[1] = 0x7f;
				}
				else
				{
					strncpy(name, (char*)call_data.calls.status[i].name.data,MAX_CHAR-1);
				}
			}
#else
			if (strlen((char*)call_data.calls.status[i].name) > 0)
			{
				alphaUsed = TRUE;
				strncpy(name, &call_data.calls.status[i].name,MAX_CHAR-1 );
			}
#endif
			
			if (strlen((char*)call_data.calls.status[i].number) > 0)
			{
				TRACE_EVENT("output COLP number");
				/*
				*COLP,  show the real connected number,
				for multi call
				*/
				if (strlen((char*)call_data.colp_number) > 0)
				{	strncpy(tag, (char*)call_data.colp_number, MAX_CHAR-1);
				}
				else
				{	strncpy(tag, (char*)call_data.calls.status[i].number, MAX_CHAR-1);
				}
			}
			else
			{
				/*MC SPR 1319*/
#ifdef NO_ASCIIZ
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{ 	/*prepend string with unicode tag so that we know to display it as unicode*/
					memset(tag, 0, MAX_CHAR);
					tag[0] = (char)0x80;/*a0393213 lint warnings removal - typecasting done*/
					tag[1] = 0x7f;
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
					memcpy(&tag[2], GET_TEXT(TxtSoftCall), ATB_string_UCLength((USHORT*)GET_TEXT(TxtSoftCall))*sizeof(USHORT));
#else /* NEW_EDITOR */
					memcpy(&tag[2], GET_TEXT(TxtSoftCall), strlenUnicode((U16*)GET_TEXT(TxtSoftCall))-2);
#endif /* NEW_EDITOR */
					TRACE_EVENT_P1("TxtSoftCall: %s", tag);
				}
				else
#endif
					/*mc end*/
					memcpy(tag, GET_TEXT(TxtSoftCall), MAX_CHAR);
			}
			
			if(alphaUsed == TRUE)
			{
				alphaUsed = FALSE;
				inCall_displayData(CALL_ID_NAME_CALL, 0, name, i);
#ifdef LSCREEN
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, i);
#endif
			}
			else
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, i);
			
			if (((call_data.calls.mode== CallConference)||(call_data.calls.mode== CallMultipartySingle))
				&& (i==callIndex(call_data.calls.selected)))
			{
				multiCall_ActiveCall(CALL_STATUS_CALL, i);
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, i);
			}
			else
			{
				inCall_displayData(CALL_ID_NUM_CALL, 0, tag, i);
			}
#ifdef LSCREEN
			memset(line,'\0',sizeof(line));
			
			getTimeString(call_data.calls.time[i],(char*)line);
			
			/* JVJ Call timer removed, it makes the display unreadable */
#endif
		}
		break;
	}
	displaySoftKeys(TxtSoftOptions,TxtEnd);
    break;
    default:
		break;
   }
   
   return 1;
}




/*******************************************************************************

 $Function:     call_kbd_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


static int call_kbd_cb (MfwEvt e, MfwKbd *k){

    T_MFW_HND       win  = mfwParent(mfw_header()); //GW Added
	int index;

	/* a0393213 (WR - "volStep" was set but never used) */
	/* int volStep;  */

  TRACE_FUNCTION("call_kbd_cb");

	/* a0393213 (WR - "volStep" was set but never used) */
	/*
	//GW Change volume in large steps if sound has a large range
	if (MAX_SOUND < 8)
		volStep = 1;
	else
		volStep = MAX_SOUND / 4;
	*/

	switch (k->code)
	{
		case KCD_MNUUP:
			if ((call_data.calls.mode== CallConference)||(call_data.calls.mode== CallMultipartySingle))
			{
				index = callIndex(call_data.calls.selected);
				if (index ==0)
					index = call_data.calls.numCalls;
				index = index - 1;
        		call_data.calls.selected = call_data.calls.status[index].call_number;
        		winShow(win);
  			}
	        	else
	        	{
//    Nov 29, 2004    REF: CRR 25051 xkundadu
//    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//    Fix: Added volume level list linked to up/down keys. 
//          User can select the speaker volume among those levels.        		

				T_MFW_HND Volmenu_handle; // Menu Handle
				UBYTE	     currentLevel = 0; // Index of the menuitem where the focus should be.
        		 	
				// Setting the reason why the volume settings is going to be changed.
			 	setSoundsReason(SettingVolume);				
				// Display the menu, if the user presses the MENU key up
        			Volmenu_handle = bookMenuStart(call_data.win, ringerVolumeLevels(), SettingVolume);
			
				switch(FFS_flashData.output_volume)
				{
					case OutVolumeLevel1: 
						{
							// If the current speaker volume is OutVolumeLevel1,
							// focus the first menu item. I.e Level 1.
							currentLevel = 0;
						}
						break;
					case OutVolumeLevel2:
						{
							// Level 2
							currentLevel = 1;
						}
						break;
					case OutVolumeLevel3: 
						{
							// Level 3
							currentLevel = 2;
						}
						break;
					case OutVolumeLevel4: 
						{
							// Level 4
							currentLevel = 3;
						}
						break;
					case OutVolumeLevel5: 
						{
							// Level 5
							currentLevel = 4;
						}
						break;
					default:
						{
							FFS_flashData.output_volume = OutVolumeLevel3;
							currentLevel = 2;
						}
				}
				
				// Set the focus to the currently selected volume level menu item.
				SEND_EVENT(Volmenu_handle, DEFAULT_OPTION, NULL, &currentLevel);	
				
	        	}
			break;

		case KCD_MNUDOWN:
			if ((call_data.calls.mode== CallConference)||(call_data.calls.mode== CallMultipartySingle))
			{
				index = callIndex(call_data.calls.selected);
				index = index + 1;
				if (index >= call_data.calls.numCalls)
					index = 0;
        		call_data.calls.selected = call_data.calls.status[index].call_number;
				winShow(win);
			}
			else
			{
//    Nov 29, 2004    REF: CRR 25051 xkundadu
//    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//    Fix: Added volume level list linked to up/down keys. 
//          User can select the speaker volume among those levels.      	       		 
				T_MFW_HND 		Volmenu_handle;// Menu Handle
				UBYTE			currentLevel = 0;// Index of the menuitem where the focus should me.

				// Setting the reason why the volume settings is going to be changed.
				setSoundsReason(SettingVolume);
				// Display the menu, if the user presses the MENU key DOWN
	        		Volmenu_handle = bookMenuStart(call_data.win, ringerVolumeLevels(), SettingVolume);
				switch(FFS_flashData.output_volume)
				{
					case OutVolumeLevel1: 
						{
							// If the current speaker volume is OutVolumeLevel1,
							// focus the first menu item. I.e Level 1.							
							currentLevel = 0;
						}
						break;
					case OutVolumeLevel2: 
						{
							currentLevel = 1;
						}
						break;
					case OutVolumeLevel3:
						{
							currentLevel = 2;
						}
						break;
					case OutVolumeLevel4: 
						{
							currentLevel = 3;
						}
						break;
					case OutVolumeLevel5: 
						{
							currentLevel = 4;
						}
						break;
					default:
						{
							FFS_flashData.output_volume = OutVolumeLevel3;
							currentLevel = 2;
						}
				}
				// Set the focus to the currently selected volume level menu item.
				SEND_EVENT(Volmenu_handle, DEFAULT_OPTION, NULL, &currentLevel);					
			}
		break;
		case KCD_LEFT:
		{	
			/*a0393213 lint warning removal - call index bound check done*/
			int call_index=callIndex(call_data.calls.selected);
			if(call_index<0 || call_index>=MAX_CALLS)
				{
				TRACE_ERROR("call_kbd_cb(): call index out of bound");
				break;
				}
			switch (call_data.calls.mode){
			case CallSingle:
				if (call_data.calls.status[call_index].stat == CALL_ACTIVE)
					menuCallSingleItems[1].str = (char*)TxtHold;
				else
					menuCallSingleItems[1].str = (char*)TxtUnhold;
				call_data.win_menu = call_menu_start(call_data.win,(T_MFW_MNU_ATTR *)&menuCallSingle,(T_VOID_FUNC)call_menu_end_cb);
				break;
			case CallTwo:
				if (call_data.calls.status[call_index].stat == CALL_ACTIVE)
					menuCallTwoItems[5].str = (char*)TxtHold;
				else
					menuCallTwoItems[5].str = (char*)TxtUnhold;
				call_data.win_menu = call_menu_start(call_data.win,(T_MFW_MNU_ATTR *)&menuCallTwo,(T_VOID_FUNC)call_menu_end_cb);
				break;
			case CallConference:
				if (call_data.calls.status[call_index].stat == CALL_ACTIVE)
					menuCallConferenceItems[5].str = (char*)TxtHold;
				else
					menuCallConferenceItems[5].str = (char*)TxtUnhold;
				menuCallConferenceItems[0].str = (char*)TxtPrivateCall;
				call_data.win_menu = call_menu_start(call_data.win,(T_MFW_MNU_ATTR *)&menuCallConference,(T_VOID_FUNC)call_menu_end_cb);
				break;
			case CallSingleMultiparty:
			case CallMultipartySingle:
				menuCallConferenceItems[0].str = (char*)TxtAddToConference;
				call_data.win_menu = call_menu_start(call_data.win,(T_MFW_MNU_ATTR *)&menuCallConference,(T_VOID_FUNC)call_menu_end_cb);
				break;
			default:
				break;
			}		
    			break;
		}
    case KCD_CALL:
      holdExec(0, 0);
      break;

    case KCD_HUP:
//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//updating voice buffering data
#ifdef FF_PCM_VM_VB
     if(  get_voice_memo_type() EQ VOICE_BUFFERING )
      {
        voice_buffering_data.rec_stop_reason = CALLING_PARTY_END_CALL;
      }
#endif     
        cm_end_all();
        break;
    case KCD_RIGHT:
//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//updating voice buffering data
#ifdef FF_PCM_VM_VB
        if(  get_voice_memo_type() EQ VOICE_BUFFERING )
        {
          voice_buffering_data.rec_stop_reason = CALLING_PARTY_END_CALL;
        }
#endif    	
			if (!call_data.calls.numCalls)
			{
     			cm_force_disconnect();
      			call_data.ignore_disconnect = TRUE;
// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is unregistered when the call is disconnected through keypad or hook.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif
			mfw_hook_unregister();
#endif			
      			/* SPR#1985 - SH - Revert to initial TTY setting*/
#ifdef MMI_TTY_ENABLED
				call_tty_revert();
#endif
        /* end SH */
				call_destroy(call_data.win);
				return 1;
			}
			// MMI-SPR 13847 xkundadu
			// Assign this variable to TRUE to indicate that, host pary started
			// the disconnection. 
			bhostDisconnection = TRUE;

			
			switch (call_data.calls.mode)
			{
        case CallConference:
        case CallMultipartySingle:
          cm_disconnect(call_data.calls.selected);
        break;
        default:
          {
            TRACE_EVENT_P1("call Id %d",call_data.calls.selected);
          }
          cm_disconnect(call_data.calls.selected);
          if (call_data.calls.numCalls==1){
            call_data.ignore_disconnect = TRUE;
          			/* SPR#1985 - SH - Revert to initial TTY setting*/
#ifdef MMI_TTY_ENABLED
		call_tty_revert();
#endif
        /* end SH */
		//	May 9, 2006    REF:DR OMAPS00074884  xrashmic
            removeCall(call_data.calls.selected, TRUE,FALSE);
          }
        break;
      }
    break;
      default:

	  	//April 25, 2005   REF: MMI-FIX-30125   x0018858
	  	//The condition has been modified to support the second call DTMF input.
    		/* use the DTMF only for single call */
			if (call_data.calls.mode EQ CallNormal ||call_data.calls.mode EQ CallDone ||
    			call_data.calls.mode EQ CallNew  ||call_data.calls.mode EQ CallSingle || call_data.calls.mode EQ CallTwo)
			{
				TRACE_EVENT("send DTMF");
				call_scratchpad_start(call_data.win,editControls[k->code],ViewDTMF, 0);
    		}
		// if (call_data.calls.mode EQ CallTwo ||call_data.calls.mode EQ CallConference ||
    			//call_data.calls.mode EQ CallSingleMultiparty ||call_data.calls.mode EQ CallMultipartySingle)
    		if (call_data.calls.mode EQ CallConference ||call_data.calls.mode EQ CallSingleMultiparty ||call_data.calls.mode EQ CallMultipartySingle)
			{
    			TRACE_EVENT("send no DTMF and the keys are able for multicall controll");
				call_scratchpad_start(call_data.win,editControls[k->code],ViewMulticallControl, ViewMulticallControlCallWaiting);
			}

    break;

    }
    return 1;


}

/*******************************************************************************

 $Function:     call_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static int call_kbd_long_cb (MfwEvt e, MfwKbd *k){

  TRACE_FUNCTION("callConnectKeyEvent");
    return MFW_EVENT_CONSUMED;


}




/*******************************************************************************

 $Function:     call_incoming_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_incoming_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
//  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//  T_call * data     = (T_call *)win_data->user;                      // RAVI

  /* a0393213 (WR - "currentRinger" was set but never used) */
#ifndef FF_MIDI_RINGER 
#ifndef FF_MMI_RINGTONE
  UBYTE currentRinger;
#endif
#endif
  /*
   * special case: Identifier is used only for the Multicall Control
   */

  /* API - 10/11/2003 - SPR 2472 - Remove the call to turn backlight on from here and place in the incomingCall() function */
  /* API - 10/11/2003 - SPR 2472 - END */

  TRACE_FUNCTION ("call_incoming_cb()");

  /*SPR 2646, don't reset window variable till window is destroyed*/
  /* a0393213 (WR - "currentRinger" was set but never used) */
#ifndef FF_MIDI_RINGER 
#ifndef FF_MMI_RINGTONE
  currentRinger = getCurrentRingerSettings();
#endif
#endif

  switch (reason)
  {
    case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:

          // terminate the ringing
       /* MZ 1/08/03 issue 2359, terminated the call waiting tone when user rejects the waiting call from the
           active call state.*/
    call_data.win_incoming = 0;/*SPR 2646, the dialogue is destroyed*/

// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is unregistered when a call is rejected.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif

	if (!call_data.calls.numCalls)
			mfw_hook_unregister();
#endif
      cm_mt_reject();

//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Check to see if the window has not been deinitialized and then de-init 
// it on rejecting the call.
//begin
	if (satDisplayTextWin)
	{
		SEND_EVENT (satDisplayTextWin, E_ED_DEINIT, INFO_KCD_LEFT, NULL);
	}
//end
    break;
    case INFO_KCD_OFFHOOK:
        case INFO_KCD_LEFT:
			
       call_data.win_incoming = 0;/*SPR 2646, the dialog is destroyed here too*/
	/* SPR#1983 - SH - Destroy WAP if WAP call is active */
#ifdef FF_WAP
    if (AUI_wap_in_call())
    {
  	  AUI_wap_browser_disconnect();
    }
    else
    {
      	cm_mt_accept();
#else
	{
	    cm_mt_accept();
#endif
	/* MZ cq11080  25/07/03 Remove mute, when incoming call is accepted. */
		if (call_data.calls.muted)
		{
       		if (audio_SetMute(AUDIO_MICROPHONE, AUDIO_MUTING_OFF) == DRV_OK)
        		{
            			call_data.calls.muted = false;
            			menuCallSingleItems[2].str = (char*)TxtMute;
        		}
    		}
	}

        break;
        case INFO_KCD_UP:
        case INFO_KCD_DOWN:

      // MZ 3/4/01set the sound reason to volume setting from incomingcall.
      /*SPR 2646, don't create a volume setting menu more than once*/
      setSoundsReason(SettingVolume);
		if (!call_data.win_menu)
		{
      // MZ 3/4/01 display the volume settings menu.
          call_data.win_menu=bookMenuStart(call_data.win, ringerVolumeSetting(), SettingVolume);
		}

          break;
    //this is for the auto answer
        case INFO_TIMEOUT:
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*MSL Stop */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
        audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif
      cm_mt_accept();
       call_data.win_incoming = 0;/*SPR 2646, this is the only point where this variable should be reset*/
        break;

        case INFO_KCD_ALL:

        call_data.win_incoming = 0;/*SPR 2646, the dialog is destroyed here too*/
    // only allowed if Keypadlock -> off and the AnyKeyAnswer -> on
      if ((FFS_flashData.settings_status & SettingsAnyKeyAnswerOn) AND
            !(FFS_flashData.settings_status & SettingsKeyPadLockOn))
      {
                TRACE_EVENT ("call_incoming_cb() with Anykeyanswer");
        mmeAudioTone(0, 0, TONE_SWT_OFF);
// Apr 05, 2005    REF: ENH 29994 xdeepadh		
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
        audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif
        cm_mt_accept();
	}
	else /* open the scratchpad for the Multicall Control */
	{
		call_scratchpad_start(call_data.win, editControls[identifier], ViewMulticallControl, ViewMulticallControlIncomingCall);

	}

        break;

    default:

    break;
  }
}

#if(0) /* x0039928 - Commented for lint warning removal */
/*******************************************************************************

 $Function: 	call_incoming_animation_cb

 $Description:	callback for the in-call animation timer expiring

 $Returns:		1

 $Arguments:	e - event (unused)
 				t - timer control block (unused)

 $ History
	GW #1555 15/01/03 - Disabled animation code for GOLite b+w display
	GW #1555 15/01/03 - Changed code to use data from the icon structure instead of hard
						coded values.

*******************************************************************************/
static int call_incoming_animation_cb (MfwEvt e, MfwTim *t)
{
#ifdef COLOURDISPLAY
	MfwIcnAttr *anim;
	call_data.animationScreen = (call_data.animationScreen + 1) % CALLANIMATION ;

	if (call_data.incCall != 0)
	{
		anim = (MfwIcnAttr *)&incoming_call_animate[call_data.animationScreen];
		dspl_BitBlt2(anim->area.px,anim->area.py,anim->area.sx,anim->area.sy,(void*)anim->icons,0,anim->icnType);
		timStart(call_data.timer);
	}
#endif
	return 1;
}
#endif

/*******************************************************************************

 $Function:     incomingCall

 $Description:

 $Returns:

 $Arguments:

 //SPR#2106 - DS - Function restructured to check return value from bookFindNameInPhonebook().
    Also checks if supplied number is empty string and, if empty, sets TextId2 to TxtNumberWithheld.
*******************************************************************************/

void incomingCall(const T_MFW_CM_CW_INFO* p_pCwInfo)
{
   
    int phbMatch=0;
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)*/
    T_MFW_PHB_TEXT *new_cnap_name;

    TRACE_FUNCTION("incomingCall()");

//x0pleela 14 Mar, 2006  ER: OMAPS00067709
#ifdef FF_PCM_VM_VB
 //Reject the incoming call if in buffering phase
  if( (get_voice_memo_type() EQ VOICE_BUFFERING) AND voice_buffering_data.buffering_phase)
  {
  	voice_buffering_data.incoming_call_discon = TRUE;
  	cm_mt_reject();  
	return;	  
  }
  
#endif

	/* API - 10/11/2003 - SPR 2472 - Place the call to turn on the backlight here so the backlight comes on for all calls */
    mme_backlightEvent(BL_INCOMING_CALL);
	/* API - 10/11/2003 - SPR 2472 - END */

    /*SPR 1392, replaced local variable display_info with call_data.inc_call_data, so incoming calls dialog
    can be easily resurrected*/

    if(p_pCwInfo == 0)
        return;

	//x0pleela 03 Apr, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	call_data.call_direction = MFW_CM_MTC;
#endif //FF_PHONE_LOCK

    if (!call_data.win)
        call_create(0);

    if (call_data.calls.numCalls == 0)
	{

// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is registered when an incoming call comes.

//May 16, 2007 DRT OMAPS00116526 x0066692(Asha)
//Moved down the code. mfw_hook_register() is called after starting the Ringer.

	        SEND_EVENT(call_data.win,CALL_INIT,0,0);
	}

    //set the global status
    MmiModuleSet(ModuleInCall);

    memcpy(&call_data.globalCWInfo, p_pCwInfo, sizeof(call_data.globalCWInfo));
    call_data.incCall = p_pCwInfo->call_number;
    call_data.call_direction = MFW_CM_MTC;


    /* SPR#1983 - SH - Detect if WAP call is in progress */
#ifdef FF_WAP
    if (AUI_wap_in_call())
    {
        dlg_initDisplayData_TextId( &call_data.inc_call_data, TxtEndWap, TxtReject, TxtNull, TxtNull , COLOUR_STATUS_INCOMINGCALL);
    }
  else
#endif
    {
        dlg_initDisplayData_TextId( &call_data.inc_call_data, TxtAccept, TxtReject, TxtNull, TxtNull , COLOUR_STATUS_INCOMINGCALL);
    }
    /* end SPR#1983 */

  //Daisy tang added for Real Resume feature 20071107 
 //start
  if(IsMP3Playing)
  {	
  	UINT i;
  
	IsRealPaused = TRUE;
	mfw_fm_audRealPause();
	mp3playTimer = timCreate( 0, 800, (MfwCb)mp3_test_play_Timer_cb);
#if 1   
	for(i = 0;i<50;i++) 
	{
	      /*
	      #define AS_STATE_STARTING  3
		#define AS_STATE_STOPPING  5
	      */
		if((as_get_state() EQ 3) OR (as_get_state() EQ 5))
	 		vsi_t_sleep(0,10); /* small delay */ 
		else
			break;
	}
#endif
  }
 //end
 /* MZ 1/08/03 issue 2359, generate a call waiting tone/indication in the ear piece. */
 if(call_data.calls.numCalls)
 /* RAVI - 20-1-2006 */	
 /* Silent Implementation */	
#ifdef NEPTUNE_BOARD
	audio_PlaySoundID( AUDIO_MICROPHONE, TONES_CW, getCurrentVoulmeSettings(), 
                                         AUDIO_PLAY_INFINITE);
#else
	 audio_PlaySoundID( AUDIO_MICROPHONE, TONES_CW, 0, AUDIO_PLAY_INFINITE);
#endif
/* END RAVI */
  else
// Apr 05, 2005    REF: ENH 29994 xdeepadh		
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_IC,true,sounds_midi_ringer_start_cb);

//May 16, 2007 DRT OMAPS00116526 x0066692(Asha)
//mfw_hook_register() is called after starting the Ringer.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif
		 mfw_hook_register();
#endif

#else
/* RAVI - 20-1-2006 */
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_RINGTONE /* MSL Ring Tone */
        audio_PlaySoundID( AUDIO_BUZZER, CALLTONE_SELECT, 0 , AUDIO_PLAY_INFINITE);
#else
	{
		 UBYTE currentRinger;
		 currentRinger=getCurrentRingerSettings();
	 audio_PlaySoundID( AUDIO_BUZZER, currentRinger, getCurrentVoulmeSettings(), 
                                          AUDIO_PLAY_INFINITE);
	}
#endif
#else
	{
		UBYTE currentRinger;
		 currentRinger=getCurrentRingerSettings();
  	 audio_PlaySoundID( AUDIO_BUZZER, currentRinger, 0 , AUDIO_PLAY_INFINITE);
	}
#endif
/* END RAVI */

#endif
    call_data.inc_call_data.TextId2      = 0;
    call_data.inc_call_data.TextString2 = NULL;/*a0393213 lint warnings removal - '\0' changed to NULL*/

    TRACE_EVENT_P1("number %s",call_data.globalCWInfo.number);

    /*SPR#2106 - DS - Check if supplied number string is empty */
    if (call_data.globalCWInfo.number[0] == 0x00)
    {
        /* No number supplied */
        idwithheld = TRUE;
        call_data.inc_call_data.TextId2  = TxtNumberWithheld;
    }
    else
    {
        /* Number supplied */
        idwithheld = FALSE;

        /* Try to find number in PB entry */
        phbMatch = bookFindNameInPhonebook((char*)call_data.globalCWInfo.number,&call_data.entry);

#ifdef NO_ASCIIZ
        TRACE_EVENT_P1("name %s",call_data.entry.name.data);
#else /* NO_ASCIIZ */
        TRACE_EVENT_P1("name %s",call_data.entry.name);
#endif /* NO_ASCIIZ */

        /* Set up alpha tag */

        if (phbMatch == TRUE)
        {
            /* Match found */
#ifdef NO_ASCIIZ
            if (call_data.entry.name.len > 0)
            {/* MC, SPR 1257, merge from 3.3.3*/
                memset(call_data.phbNameNumber, 0, EDITOR_SIZE);
                memcpy(call_data.phbNameNumber,(char*)call_data.entry.name.data, sizeof(call_data.phbNameNumber));
                /*MC end*/

                //GW-SPR#762 - String may not be null terminated - add null at end.
                if (call_data.entry.name.len < EDITOR_SIZE)
                    call_data.phbNameNumber[call_data.entry.name.len] = 0x00;

                call_data.inc_call_data.TextString2  = call_data.phbNameNumber;
            }
#else /* NO_ASCIIZ */
            //checking for the number from caller
            if (strlen((char*)call_data.entry.name) > 0)
            {
                bookGsm2Alpha((UBYTE*)call_data.entry.name);
                strncpy((char*)call_data.phbNameNumber,(char*)call_data.entry.name,sizeof(call_data.phbNameNumber));
                call_data.inc_call_data.TextString2  = (char*)call_data.phbNameNumber;
            }
#endif /* NO_ASCIIZ */
        }
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)-Begin*/
else if (CALL_NAME_NETWORK)
		{
		/*Name is from Network (rAT_%CNAP)*/
		CALL_NAME_NETWORK=FALSE;
		new_cnap_name=get_cnap_name();
		call_data.globalCWInfo.name.len=new_cnap_name->len;
#ifdef NO_ASCIIZ	
		memcpy(call_data.globalCWInfo.name.data,new_cnap_name->data,sizeof(call_data.globalCWInfo.name.data));
		if (call_data.globalCWInfo.name.len > 0)
              {     
              	memset(call_data.phbNameNumber, 0, EDITOR_SIZE);
              	memcpy(call_data.phbNameNumber,(char*)call_data.globalCWInfo.name.data, sizeof(call_data.phbNameNumber));
                
                //String may not be null terminated - add null at end.
              	if (call_data.entry.name.len < EDITOR_SIZE)
              	call_data.phbNameNumber[call_data.globalCWInfo.name.len] = 0x00;

              	call_data.inc_call_data.TextString2  = call_data.phbNameNumber;
            	}
#else /* NO_ASCIIZ */
            //checking for the number from caller
            strcpy(call_data.globalCWInfo.name.len,new_cnap_name->len);
            strcpy(call_data.globalCWInfo.name.data,new_cnap_name->data);
            if (strlen((char*)call_data.globalCWInfo.name) > 0)
            	{            			
             		bookGsm2Alpha((UBYTE*)call_data.globalCWInfo.name);
             		strncpy((char*)call_data.phbNameNumber,(char*)call_data.globalCWInfo.name,sizeof(call_data.phbNameNumber));
             		call_data.inc_call_data.TextString2  = (char*)call_data.phbNameNumber;
            	}
#endif /* NO_ASCIIZ */
		}
/*May 10,2007 DR:OMAP00127983 x0066814(Geetha)- End*/
        else
        {
            /* No match */
            if (strlen((char*)call_data.globalCWInfo.number) > 0)
            {
                int max_size = sizeof(call_data.phbNameNumber);
		  memset((char*)call_data.phbNameNumber, 0x00, max_size);
                /* SH 12/02/02.  Ensure that '+' is included before international numbers. */
                if ((call_data.globalCWInfo.ton == MFW_TON_INTERNATIONAL) && (call_data.globalCWInfo.number[0] != (UBYTE)'+') )
                {
                    strcpy((char*)call_data.phbNameNumber,"+");
			max_size--;
                }

                /* sbh end */

                strncat((char*)call_data.phbNameNumber,(char*)call_data.globalCWInfo.number, max_size);

                call_data.inc_call_data.TextString2  = (char*)call_data.phbNameNumber;
            }
        }
    }
    //GW-SPR#762 - Wrap number/name onto 2nd line (if too long)
    call_data.inc_call_data.WrapStrings = WRAP_STRING_2;

    if (!ALSPresent(NULL, NULL, NULL))
    {
        if (call_data.globalCWInfo.type EQ  VOICE_CALL)
    		call_data.inc_call_data.TextId       = TxtLine1;
        if (call_data.globalCWInfo.type EQ AuxVOICE_CALL)
    		call_data.inc_call_data.TextId       = TxtLine2;
    }
    else
    {
    	if (call_data.globalCWInfo.type EQ  VOICE_CALL)
    	{
            /* SPR#1352 - SH - Modify dialog if this is a TTY call */
            /* API - 10-01-03 - 1543 - change the pointer from display_info to call_data.inc_call_data */
#ifdef MMI_TTY_ENABLED
    		if (call_tty_get())
    		{
    			call_data.inc_call_data.TextId       = TxtIncomingTTYCall;
    		}
    		else
    		{

    			call_data.inc_call_data.TextId       = TxtIncomingCall;
    		}
#else /* MMI_TTY_ENABLED */
    		call_data.inc_call_data.TextId       = TxtIncomingCall;
#endif /* MMI_TTY_ENABLED */
    	}
    	/* end SH */

    }

    if (call_data.globalCWInfo.type EQ  DATA_CALL)
        call_data.inc_call_data.TextId       = TxtIncomingData; // Only to be able to compile.

    if (call_data.globalCWInfo.type EQ  FAX_CALL)
        call_data.inc_call_data.TextId       = TxtIncomingFax; //SPR#1147 - DS - changed text id from "Fax" to "Incoming fax".

#ifndef COLOURDISPLAY
    call_data.inc_call_data.dlgType = 0;
    call_data.inc_call_data.bgdBitmap = NULL;
#else /* COLOURDISPLAY */
    call_data.inc_call_data.dlgType = DLG_BORDER_VERTICAL_1|DLG_BORDER_HORIZONTAL_1|DLG_INFO_LAYOUT_CENTRE|DLG_INFO_LAYOUT_BOTTOM;
    call_data.inc_call_data.bgdBitmap = icon_getBgdBitmap(BGD_INCALL);
#endif /* COLOURDISPLAY */

    if (FFS_flashData.settings_status & SettingsAutoAnswerOn)
    {
        //Assume we cannot do call deflection if auto-answer is on (not enough time)
	// xrashmic 12 Aug, 2004  Bug: 14, 21
	//Disabled the KEY_MNUUP|KEY_MNUDOWN for this dialog. It used to display the alert mode screen in the incoming call screen
        dlg_initDisplayData_events( &call_data.inc_call_data, (T_VOID_FUNC)call_incoming_cb, THREE_SECS, KEY_RIGHT|KEY_LEFT|KEY_HUP|KEY_CALL|INFO_KCD_ALL );
    }
    /*SPR 1392, if call deflection is off*/
    else if (FFS_flashData.call_deflection!= TRUE)
    {
	// xrashmic 12 Aug, 2004 Bug: 14, 21
	//Disabled the KEY_MNUUP|KEY_MNUDOWN for this dialog. It used to display the alert mode screen in the incoming call screen
        dlg_initDisplayData_events( &call_data.inc_call_data, (T_VOID_FUNC)call_incoming_cb, FOREVER, KEY_RIGHT|KEY_LEFT|KEY_HUP|KEY_CALL|INFO_KCD_ALL );
    }
    else //Call deflection active
    {
        call_data.inc_call_data.LeftSoftKey  = TxtDeflect;
	// xrashmic 12 Aug, 2004 Bug: 14, 21
	//Disabled the KEY_MNUUP|KEY_MNUDOWN for this dialog. It used to display the alert mode screen in the incoming call screen
        dlg_initDisplayData_events( &call_data.inc_call_data, (T_VOID_FUNC)call_deflection_cb, FOREVER, KEY_RIGHT|KEY_LEFT|KEY_HUP|KEY_CALL|INFO_KCD_ALL );
    }

    /*
    ** Turn on the backlight
    */
    mme_backlightEvent(BL_INCOMING_CALL);
    /*
    * Call Info Screen
    */
#ifdef COLOURDISPLAY
	mmi_dialogs_insert_animation_new(&call_data.inc_call_data,250,(MfwIcnAttr*)incoming_call_animate, CALLANIMATION);
#endif
    call_data.win_incoming = info_dialog (call_data.win, &call_data.inc_call_data);

}




/*******************************************************************************

 $Function:     call_calling_show

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/




static void call_calling_show (USHORT Identifier)
{

    T_DISPLAY_DATA   display_info;
  TRACE_FUNCTION("call_calling_show");

	/* SPR#1352 - SH - Modify dialog if this is a TTY call */
#ifdef MMI_TTY_ENABLED
	if (call_tty_get())
	{
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtEnd, TxtTTYCalling, TxtNull , COLOUR_STATUS_CALLING);
	}
	else
#endif
	{
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtEnd, TxtCalling, TxtNull , COLOUR_STATUS_CALLING);
	}
	/* end SH */

	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_calling_cb, FOREVER, KEY_RIGHT | KEY_HUP | NUMERIC_KEYS );
	display_info.Identifier   = Identifier;
	//GW-SPR#762 - Wrap number/name onto 2nd line (if too long)
	display_info.WrapStrings = WRAP_STRING_2;

  	if (call_data.emergencyCall)
     	display_info.TextId2  = TxtSOS;
  	else
  		display_info.TextString2  = (char*)call_data.phbNameNumber;
	//x0035544 Mar 09, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
	if(call_data.IconData.dst != NULL)
	{	
		//copy the icon data to be displayed on to the screen.
		display_info.IconData.width 	=  call_data.IconData.width;
	  	display_info.IconData.height	= call_data.IconData.height;		
		display_info.IconData.dst = call_data.IconData.dst;
		display_info.IconData.selfExplanatory 	= call_data.IconData.selfExplanatory;
	}      
#endif	

  	/* set the user defined identifier */
    call_data.win_calling= info_dialog (call_data.win, &display_info);
}


/*******************************************************************************

 $Function:     call_calling_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_calling_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
 // T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//  T_call * data     = (T_call *)win_data->user;                      // RAVI
  /*
   * Who has initiated the information screen
   */
  TRACE_FUNCTION ("call_calling_cb()");
  call_data.win_calling = 0;
  switch (reason)
  {
        case INFO_TIMEOUT:
        break;
        case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:

//x0pleela 09 Mar, 2006  ER:OMAPS00067709
//updating voice buffering data
#ifdef FF_PCM_VM_VB
          if(  get_voice_memo_type() EQ VOICE_BUFFERING )
          {
            voice_buffering_data.rec_stop_reason = CALL_END_BEF_SETUP;
          }
#endif  
			/* destroy the redial windows if exit*/
			cm_redial_abort();

       if (identifier == MMI_OUTGOING_CALL_DIALOG_SAT)
        /* in the SAT case, MFW does not know about the call, we therefore
         * need to disconnect the call without using MFW's call table
         */
		{
        satDisc();
        /* Marcus: Issue 1057: 21/01/2003: Start */
        SEND_EVENT(sat_get_call_setup_win(), SAT_CALL_END, 0, NULL);

        /* SPR#1784 - DS - Not needed. SAT_RETURN sent by sim_toolkit_exec() SAT_SESSION_END.
         */
        //SEND_EVENT(sat_get_setup_menu_win(), SAT_RETURN, 0, NULL); /* recreate the SETUP MENU */
        call_SATCall = FALSE;
        /* Marcus: Issue 1057: 21/01/2003: End */

         call_data.ignore_disconnect = TRUE;
  		if (!call_data.calls.numCalls)
		{
          call_destroy(call_data.win);
          return;
  	    }
  	}
     else
      {
        TRACE_EVENT_P1("disconnecting call %d",call_data.outCall);
        cm_disconnect(call_data.outCall);

				if (!call_data.calls.numCalls)
				{
          call_data.ignore_disconnect = TRUE;
          call_destroy(call_data.win);
          return;
      }
      endingCall();

// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
// Fix: Hook is unregistered when an outgoing call is ended in calling window.
#ifdef FF_MMI_AUDIO_PROFILE
// Aug 17, 2006 REF:OMAPS00090196  x0039928
#ifdef MMI_TTY_ENABLED

if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif

		if (!call_data.calls.numCalls)
			mfw_hook_unregister();
#endif

        break;
        case INFO_KCD_LEFT:
        break;
    case INFO_KCD_ALL:
    	/*MC 12.04.02  allows scratchpad to be started when call not yet connected*/
    		/* use the DTMF only for single call */
			if (call_data.calls.mode EQ CallNormal ||call_data.calls.mode EQ CallDone ||
    			call_data.calls.mode EQ CallNew  ||call_data.calls.mode EQ CallSingle || call_data.calls.mode EQ NULL)
			{
				TRACE_FUNCTION("send DTMF");

				call_scratchpad_start(call_data.win,editControls[identifier],ViewDTMF, 0);
			}

			if (call_data.calls.mode EQ CallTwo ||call_data.calls.mode EQ CallConference ||
    			call_data.calls.mode EQ CallSingleMultiparty ||call_data.calls.mode EQ CallMultipartySingle)
			{
    			TRACE_FUNCTION("send no DTMF and the keys are able for multicall controll");
				call_scratchpad_start(call_data.win,editControls[identifier],ViewMulticallControl, ViewMulticallControlCallWaiting);
			}

    break;
      }
  }
}

/*******************************************************************************

 $Function:     call_menu_end_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_menu_end_cb(void)
{
  call_data.win_menu=0;
}


/*******************************************************************************

 $Function:     call_waiting_answer_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_waiting_answer_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
 // T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//  T_call * data     = (T_call *)win_data->user;                      // RAVI
  /*
   * Who has initiated the information screen
   */
  TRACE_FUNCTION ("call_waiting_answer_cb()");


  call_data.win_waiting=0;

  switch (reason)
  {
//        call_data.win_waiting=0;   // RAVI
        case INFO_KCD_LEFT:
      if (call_data.current_command)
        cm_command_abort(call_data.current_command);
      call_data.current_command = 0;
      callSetMode(ViewConnect, TRUE);
        break;
      default:
    break;
  }
}

/*******************************************************************************

 $Function:     call_sending_dtmf_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_sending_dtmf_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
//  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//  T_call * data     = (T_call *)win_data->user;                      // RAVI
  /*
   * Who has initiated the information screen
   */
  TRACE_EVENT ("call_sending_dtmf_cb()");
  call_data.win_sending_dtmf=0;
  switch (reason)
  {
        case INFO_KCD_LEFT:
        case INFO_KCD_HUP:
      call_data.charSent = 0;
      call_data.sendingDTMF = FALSE;
      cm_command_abort(CM_DTMF_AUTO);
      call_data.current_command = CM_DTMF_AUTO;
        break;
    default:
    break;
  }
}



/*******************************************************************************

 $Function:     call_ending_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

/*
 * Marcus: Issue 1057: 21/01/2003: Modified this function to use call_acm_max_cb
 * in the call to dlg_initDisplayData_events, and to handle the end of a SAT
 * call is not in the "No credit" situation
 */



void call_ending_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
//  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//  T_call * data     = (T_call *)win_data->user;                      // RAVI
//  T_MFW_CM_AOC_INFO aocInfo;  // RAVI

  /*
   * Who has initiated the information screen
   */
  TRACE_FUNCTION ("call_ending_cb()");
  call_data.win_ending=0;
  switch (reason)
  {
      default:
      endingCall();
    break;
  }
	//MC 17/12/2001
  //check if call ended because ACM>=ACMMAX
{T_MFW_CM_AOC_INFO aocInfo;

	cm_get_aoc_value(CM_AOC_ACMMAX,&aocInfo);

	//Get ACM and ACMMAX
    cm_get_aoc_value(CM_AOC_ACM,&aocInfo);

	//display "No Credit" to user
	if((aocInfo.acm >= aocInfo.acm_max) &&
	 	( aocInfo.acm_max > 0))
	{    	T_DISPLAY_DATA display_info;

			TRACE_EVENT("acm_max exceeded");

		dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtNo, TxtCredit , COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_acm_max_cb, THREE_SECS, KEY_RIGHT | KEY_LEFT );
		info_dialog (call_data.win, &display_info);
		return;
//			return;   // RAVI
	}
	else if (call_SATCall == TRUE)
    {
        call_SATCall = FALSE;
#if 0 /* CQ16437 : Do not need to return to the SAT menu as it was not exited on Call Setup */
        SEND_EVENT(sat_get_call_setup_win(), SAT_CALL_END, 0, NULL);
        SEND_EVENT(sat_get_setup_menu_win(), SAT_RETURN, 0, NULL); /* SPR#1784 - DS - recreate the SETUP MENU */
#endif
    }

}

//Nov 09, 2005 REF: OMAPS00043190 - x0018858
// Check to see if the window has not been deinitialized and then de-init 
// it on ending the call after answering it.
//Begin
	if (satDisplayTextWin)
	{
		SEND_EVENT (satDisplayTextWin, E_ED_DEINIT, INFO_KCD_LEFT, NULL);
	}
//end

}

/*
 * Marcus: Issue 1057: 21/01/2003: Added the call_acm_max_cb function to handle
 * the end of a SAT call in the "No credit" situation
 */
/*******************************************************************************

 $Function:     call_acm_max_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_acm_max_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    TRACE_FUNCTION ("call_acm_max_cb()");
    if (call_SATCall == TRUE)
    {
        call_SATCall = FALSE;
        SEND_EVENT(sat_get_call_setup_win(), SAT_CALL_END, 0, NULL);
        SEND_EVENT (sat_get_setup_menu_win(), SAT_RETURN, 0, NULL); /* SPR#1784 - DS - recreate the SETUP MENU */

    }
}




/*******************************************************************************

 $Function:     call_menu_create

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


static T_MFW_HND call_menu_create(MfwHnd parent){

  T_MFW_WIN     * win_data;
  T_call_menu *  data = (T_call_menu *)ALLOC_MEMORY (sizeof (T_call_menu));


  TRACE_FUNCTION ("call_menu_create()");

  data->win = win_create (parent, 0, E_WIN_VISIBLE, (T_MFW_CB)call_menu_win_cb);

  if (data->win EQ 0)
    return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)call_menu;
  data->mmi_control.data     = data;
    win_data                   = ((T_MFW_HDR *)data->win)->data;
  win_data->user             = (void *)data;

   /*
    * return window handle
    */

  data->kbd = kbdCreate(data->win,KEY_ALL,(MfwCb)call_menu_kbd_cb);
    data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)call_menu_kbd_long_cb);


    return data->win;



}

/*******************************************************************************

 $Function:     call_menu_destroy

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static void call_menu_destroy(MfwHnd window){

  T_MFW_WIN     * win_data  = ((T_MFW_HDR *)window)->data;
  T_call_menu * data = (T_call_menu *)win_data->user;

  TRACE_FUNCTION ("call_menu_destroy()");

	if (window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
		return;
	}

  if (data)
  {
    call_data.win_menu = 0;
    winDelete (data->win);
    /*     * Free Memory
       */
    FREE_MEMORY ((void *)data, sizeof (T_call_menu));
  }

}


/*******************************************************************************

 $Function:     call_menu_start

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



T_MFW_HND call_menu_start(MfwHnd parent,MfwMnuAttr* menuAttr,T_VOID_FUNC func_cb){


    T_MFW_HND win;

      win = call_menu_create (parent);

      if (win NEQ NULL)
    {
        SEND_EVENT (win, CALL_INIT, 0, (MfwMnuAttr*) menuAttr);
    }
      return win;



}



/*******************************************************************************

 $Function:     call_menu

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


void call_menu (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_call_menu      * data = (T_call_menu *)win_data->user;

    TRACE_FUNCTION ("T_call_menu()");

    switch (event)
  {
    case CALL_INIT:
      data->menu = mnuCreate(data->win,(MfwMnuAttr*)parameter,E_MNU_ESCAPE,(MfwCb)call_menu_mnu_cb);
      mnuLang(data->menu,mainMmiLng);
      mnuUnhide(data->menu);
      winShow(data->win);
    break;
    default:
    return;
  }

}

/*******************************************************************************

 $Function:     call_menu_win_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/




static int call_menu_win_cb (MfwEvt e, MfwWin *w)
{
	T_call_menu * data = (T_call_menu *)w->user;

	MfwMnu * mnu;

	TRACE_FUNCTION ("call_menu_win_cb()");
	switch (e)
	{
    	case MfwWinVisible:
      		mnu = (MfwMnu *)mfwControl(data->menu);
      		showDefault();
			softKeys_displayId(TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour );
			break;
        default:
        	return 0;
    }
    return 1;

}


/*******************************************************************************

 $Function:     call_menu_kbd_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static int call_menu_kbd_cb (MfwEvt e, MfwKbd *k){


    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_call_menu * data = (T_call_menu *)win_data->user;

  TRACE_FUNCTION ("call_menu_kbd_cb()");


  switch (k->code)
    {
        case KCD_MNUUP:
            mnuUp(data->menu);
      winShow(win);
        break;
        case KCD_MNUDOWN:
            mnuDown(data->menu);
      winShow(win);
        break;
        case KCD_LEFT:
      mnuSelect(data->menu);
      	{
      		MfwMnu *mnu;
      		mnu = (MfwMnu *) mfwControl( data->menu );
			if (mnu != NULL)
			{
				if (mnu->curAttr != NULL)
					softKeys_displayId( TxtSoftSelect, TxtSoftBack, 0, mnu->curAttr->mnuColour);
			}
      	}
			break;
        case KCD_RIGHT:
        case KCD_HUP:
            mnuEscape(data->menu);
        break;
        default:
            return MFW_EVENT_CONSUMED;
  }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:     call_menu_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/




static int call_menu_kbd_long_cb (MfwEvt e, MfwKbd *k){

    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_call_menu * data = (T_call_menu *)win_data->user;


    if ((e & KEY_CLEAR) && (e & KEY_LONG))
    {
      mnuEscape(data->menu);
      return MFW_EVENT_CONSUMED;
    }
    return  MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:     call_menu_mnu_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



static int call_menu_mnu_cb (MfwEvt e, MfwMnu *m){

    T_MFW_HND       win  = mfwParent(mfw_header());
 //   T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
//    T_call_menu * data = (T_call_menu *)win_data->user;      // RAVI

  TRACE_FUNCTION ("call_menu_mnu_cb()");

  switch (e)
        {
            case E_MNU_ESCAPE:
        call_menu_destroy(win);
        call_data.win_menu = 0;
      break;
            default:
                return 0;
        }
  return 1;
}




/*******************************************************************************

 $Function:     call_scratchpad_create

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


T_MFW_HND call_scratchpad_create (T_MFW_HND parent_window)
{
    T_MFW_WIN     * win;
    T_call_scratchpad *  data = (T_call_scratchpad *)ALLOC_MEMORY (sizeof (T_call_scratchpad));



    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)call_scratchpad_win_cb);

    if (data->win EQ 0)
      return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)call_scratchpad;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
    * Create any other handler
    */

  memset( (void *) call_data.edtBufDTMF,'\0',sizeof(call_data.edtBufDTMF));
  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr(&call_data.editorAttrDTMF,CALLSCRATCHPAD_EDITOR,COLOUR_EDITOR_XX,EDITOR_FONT, ED_MODE_ALPHA, ED_CURSOR_BAR,ATB_DCS_ASCII, (UBYTE*)call_data.edtBufDTMF,EDITOR_SIZE);
    data->editor = ATB_edit_Create(&call_data.editorAttrDTMF,0);
#else /*NEW_EDITOR*/
	bookSetEditAttributes(CALLSCRATCHPAD_EDITOR, COLOUR_EDITOR_XX,0,edtCurBar1,0,(char*)call_data.edtBufDTMF,EDITOR_SIZE,&call_data.edtAttrDTMF);
	data->edt      = edtCreate  (data->win,&call_data.edtAttrDTMF,MfwEdtVisible,0);
#endif /*NEW_EDITOR*/

    // xnkulkar SPR-18262 : Added KEY_MAKE flag to detect the key release event.
    data->kbd      = kbd_create (data->win,KEY_ALL |KEY_MAKE,(T_MFW_CB)call_scratchpad_kbd_cb);
    data->kbd_long = kbd_create (data->win,KEY_ALL|KEY_LONG,(T_MFW_CB)call_scratchpad_kbd_long_cb);

  /*
   * return window handle
   */
  return data->win;
}





/*******************************************************************************

 $Function:     call_scratchpad_destroy

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_scratchpad_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_call_scratchpad        * data;

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_call_scratchpad *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */
       /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	  /* New editor not destroyed automatically by win_delete,
	   * so do so here */
	  if (data->editor)
	  {
	  	ATB_edit_Destroy(data->editor);
	  }
#endif /* NEW_EDITOR */

      win_delete (data->win);
    }
    FREE_MEMORY ((void *)data, sizeof (T_call_scratchpad));
  }
}


/*******************************************************************************

 $Function:     call_scratchpad_start

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

T_MFW_HND call_scratchpad_start (T_MFW_HND win_parent,char character,UBYTE mode, UBYTE callstatus)
{
  T_MFW_WIN       *win_data;
  T_call_scratchpad         * data;


  T_MFW_HND win = call_scratchpad_create (win_parent);


  if (win NEQ NULL)
  {
      win_data = ((T_MFW_HDR *) win)->data;
        data = (T_call_scratchpad *)win_data->user;
    data->mode = mode;
		data->callstatus = callstatus;
    SEND_EVENT(win,CALL_SCRATCHPAD_INIT,0,&character);/*a0393213 warnings removal-address of character sent*/
  }
  return win;

}



/*******************************************************************************

 $Function:     call_scratchpad

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_scratchpad (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_call_scratchpad         * data = (T_call_scratchpad *)win_data->user;

    TRACE_FUNCTION ("call_scratchpad()");

    switch (event)
  {
    case CALL_SCRATCHPAD_INIT:
      memset(call_data.edtBufDTMF,'\0',sizeof(call_data.edtBufDTMF));
      /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
      ATB_edit_Unhide(data->editor);
      ATB_edit_AsciiChar(data->editor, *((char*)parameter), FALSE); /*a0393213 warnings removal-parameter properly typecasted and used*/
      ATB_edit_AsciiChar(data->editor, ctrlBottom, TRUE);
#else /* NEW_EDITOR */
      edtUnhide(data->edt);
      edtChar(data->edt,(char)parameter);    // RAVI - Introduced *
      edtChar(data->edt,ecBottom);
#endif /* NEW_EDITOR */

	#if 1//TISH patch  for OMAPS00115335
	  if (data->mode EQ ViewDTMF || data->mode EQ ViewMulticallControl)
	#else
	  if (data->mode EQ ViewDTMF)
	#endif
	  {
	  	cm_command(call_data.calls.selected,CM_DTMF_ACTIVE,(UBYTE)call_data.edtBufDTMF[0]);
	  	call_data.current_command = CM_DTMF_ACTIVE;
	  }
      winShow(data->win);
    break;
      default:
    return;
  }
}




/*******************************************************************************

 $Function:     call_scratchpad_win_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

static int call_scratchpad_win_cb (MfwEvt e, MfwWin *w)
{
    T_call_scratchpad         * data = (T_call_scratchpad *)w->user;

    TRACE_FUNCTION ("call_scratchpad_win_cb()");
  switch (e)
    {
        case MfwWinVisible:
      dspl_ClearAll();
   	  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	  ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
      edtShow(data->edt);
#endif /* NEW_EDITOR */
      displaySoftKeys(TxtSoftCall,TxtDelete);//MC
        break;
        default:
        return 0;
    }

    return 1;
}


#if(0) /* x0039928 - Commented for lint warning removal */
static void call_display_call_ended(void)
{
	T_DISPLAY_DATA display_info;

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallEnded, TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_CLEAR );

	/*
	* Call Info Screen
	*/
	info_dialog (0, &display_info);


	return;
}
#endif
/*******************************************************************************

 $Function:     call_scratchpad_kbd_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/


static int call_scratchpad_kbd_cb (MfwEvt e, MfwKbd *k)
{
	T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_call_scratchpad         * data = (T_call_scratchpad *)win_data->user;
    // xnkulkar SPR-18262: This BOOL variable is added to detect the key press event in order to call winShow()/windowsUpdate() function
    BOOL key_pressed_dtmf = FALSE;
	//xvilliva SPR13698 - The following two variables are used to find a 
	//HELD and an active call.	
	SHORT heldID = -1;
	SHORT activeID = -1;  

//	int position=0;   // RAVI
	UBYTE callstatus = data->callstatus;

    TRACE_FUNCTION ("call_scratchpad_kbd_cb()");

    // xnkulkar SPR-18262:  Detect the key release event (using !(e & KEY_MAKE)). This event is used to send the 
    // DTMF stop command. 
    if (!(e & KEY_MAKE))
    {
     	switch (k->code)
     	{
		case KCD_0:
	      	case KCD_1:
	     	case KCD_2:
	      	case KCD_3:
	      	case KCD_4:
	      	case KCD_5:
	      	case KCD_6:
	      	case KCD_7:
	      	case KCD_8:
	       case KCD_9:
	      	case KCD_STAR:
	      	case KCD_HASH:
			// xnkulkar SPR-18262: A key has been released. Call cm_command() function with  CM_DTMF_INACTIVE
			// mode to stop sending the DTMF tone.
		#if 1 // for OMAPS00115335
                       if(call_data.current_command EQ CM_DTMF_ACTIVE)
                        {
                               cm_command(call_data.calls.selected,CM_DTMF_INACTIVE,(UBYTE)editControls[k->code]);
                               call_data.current_command = CM_DTMF_INACTIVE;
                        }
		#else
	      		cm_command(call_data.calls.selected,CM_DTMF_INACTIVE,(UBYTE)editControls[k->code]);
		     	call_data.current_command = CM_DTMF_INACTIVE;
		#endif

		default:
	             	return MFW_EVENT_CONSUMED;
//			break;   // RAVI
	   }
	}
			
    switch(data->mode){
        case ViewDTMF:
    switch (k->code)
    {
      case KCD_0:
      case KCD_1:
      case KCD_2:
      case KCD_3:
      case KCD_4:
      case KCD_5:
      case KCD_6:
      case KCD_7:
      case KCD_8:
        case KCD_9:
      case KCD_STAR:
      case KCD_HASH:


      /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
        ATB_edit_AsciiChar(data->editor, editControls[k->code], FALSE);
        ATB_edit_AsciiChar(data->editor,ctrlBottom, TRUE);
#else /* NEW_EDITOR */
        edtChar(data->edt, editControls[k->code]);
        edtChar(data->edt,ecBottom);
#endif /* NEW_EDITOR */

#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
		/*MC, SPR2469 check for engineering mode code*/
		if (strcmp((char*)call_data.edtBufDTMF, "*36446337464#") == 0)
		{
			TRACE_EVENT("Engineering mode string");
			/*create menu*/
			mmi_em_start_eng_mode_menus(call_data.win);
		}
		else
#endif
		{
			// xnkulkar SPR-18262: A key has been pressed. Call cm_command() function with  CM_DTMF_ACTIVE
			// mode to start sending the DTMF tone.
	        	cm_command(call_data.calls.selected,CM_DTMF_ACTIVE,(UBYTE)editControls[k->code]);
	        	call_data.current_command = CM_DTMF_ACTIVE;
	  	 	key_pressed_dtmf = TRUE;
			// xnkulkar SPR-18262: Call winShow() to update the character on screen.
			// Calling windowsUpdate() slows down the process of sending DTMF commands and induces a delay.
			win_show(data->win);		
		}
          break;

        /*API - 23/07/03 - 11076 - Add the call to KCD_RIGHT here */
        /*API - 06/10/03 - 2585 - Re-work of the above issue */
        case KCD_RIGHT:

        if (strlen(call_data.edtBufDTMF) > 0)
        {
        /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
       		ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
        	edtChar(data->edt, ecBack);
#endif /* NEW_EDITOR */
		
        }
		else
		call_scratchpad_destroy(data->win);
            
      break;
      case KCD_HUP:
	//	May 9, 2006    REF:DR OMAPS00074884  xrashmic        
	// Pressing Red key in scratch pad releses all calls. If there are no calls, 
	// it would destroy the scratch pad.
		{
			if (call_data.calls.numCalls==0)
			{
				call_scratchpad_destroy(data->win);
			}
			else
			{
				cm_end_all();
			}
		}
      break;
      case KCD_CALL:
        case KCD_LEFT:

        callNumber((UBYTE*)call_data.edtBufDTMF);
        call_scratchpad_destroy(win);
		/* MZ cq11080  25/07/03 Remove mute when activating new call */
	if (call_data.calls.muted)
	{
       	if (audio_SetMute(AUDIO_MICROPHONE, AUDIO_MUTING_OFF) == DRV_OK)
        	{
            		call_data.calls.muted = false;
            		menuCallSingleItems[2].str = (char*)TxtMute;
        	}
    	}
        return 1;
//      break;  // RAVI

	  /*API - 23/07/03 - 11076 - Remove duplication of code for The handeling of KCD_RIGHT
		  	  					 problem was also this returned 1 at end of case and stopped
								 further editor update etc*/

      default:
      break;
    }
        break;
    case ViewScratchPad:
    switch (k->code)
    {
      case KCD_0:
      case KCD_1:
      case KCD_2:
      case KCD_3:
      case KCD_4:
      case KCD_5:
      case KCD_6:
      case KCD_7:
      case KCD_8:
        case KCD_9:
      case KCD_STAR:
      case KCD_HASH:
       /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
        ATB_edit_AsciiChar(data->editor, editControls[k->code], TRUE);
#else /* NEW_EDITOR */
        edtChar(data->edt, editControls[k->code]);
#endif /*NEW_EDITOR */
          break;
        case KCD_HUP:
          call_scratchpad_destroy(data->win);
      break;
        case KCD_LEFT:
        sendDTMFString();
      break;
        case KCD_RIGHT:
          call_scratchpad_destroy(data->win);
      break;
      default:
      break;
    }
    break;
		case ViewMulticallControl:
		{
			TRACE_FUNCTION ("ViewMulticallControl()");
			switch (k->code)
			{
			case KCD_0:
			case KCD_1:
			case KCD_2:
			case KCD_3:
			case KCD_4:
			case KCD_5:
			case KCD_6:
			case KCD_7:
			case KCD_8:
    		case KCD_9:
// 
//  Jun 10, 2004    REF=CRR 13602	Sasken/xreddymn
//		Added * and # key handling, to allow the input of 
//		* and # characters in the scratchpad screen
			case KCD_STAR:
			case KCD_HASH:

    		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		        ATB_edit_AsciiChar(data->editor, editControls[k->code], TRUE);
#else /* NEW_EDITOR */
        		edtChar(data->edt, editControls[k->code]);
#endif /* NEW_EDITOR */
	        break;

			case KCD_MNUUP:
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		        ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
#else /* NEW_EDITOR */
        		edtChar(data->edt, ecRight);
#endif /* NEW_EDITOR */
			break;

     	    case KCD_MNUDOWN:
     	    /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		        ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);
#else /* NEW_EDITOR */
        		edtChar(data->edt, ecLeft);
#endif /* NEW_EDITOR */
     	    break;
     		case KCD_CALL:
	  		case KCD_LEFT:
	  			 /*MC 12/04/02 Check for ECT string  and display appropriate waiting screen*/
		        if (call_data.edtBufDTMF[0]== '4' && strlen(call_data.edtBufDTMF) == 1)
		        { 	call_data.current_command = CM_ETC;
		  			callSetMode(ViewWaitingAnswer, TRUE);
		        }
				// xvilliva SPR13698
				// We find out if the current call is an emergency call, we check if there is an 
				// active call and a held call currently, we release all the calls to make way to 
				// establish an emergency call.
				// When we try to disconnect the active+held calls using cm_disconnect() and 
				// establish an emergency call using cm_mo_call(), ECC doesnot get through
				// as we do not get E_CM_DISCONNECT immediately. This is varying with 
				// service providers. To avoid this we take the approach of ending all calls
				// and after we receive the E_CM_DISCONNECT_ALL event we initiate the 
				// emergency call.
				makeECC = FALSE;
				strcpy(strECC,"");
				if (cm_check_emergency((UBYTE*)call_data.edtBufDTMF) EQ TRUE)
				{
					if ((cm_search_callId(CAL_STAT_Held, &heldID) == CM_OK)
						AND (cm_search_callId(CAL_STAT_Active, &activeID) == CM_OK))
					{
						strcpy(strECC,call_data.edtBufDTMF);// Store the emergency number.
						makeECC = TRUE;						// Set the flag for making ECC.
						cm_end_all();						// We end all other calls.
						if (call_data.win) 
							call_destroy(call_data.win); 	// Clear the call status window.				       	
					}			
				}
				if(makeECC == FALSE)						// We make call only if it is not
				{											// handled in the above "if".
	  				cm_mo_call((UBYTE*)call_data.edtBufDTMF, VOICE_CALL);
				}
				call_scratchpad_destroy(data->win);

			// June 22, 2004 MMI-SPR 16573 CW+CLIP : blanck screen
			// Issue description: 1 active call + 1 Call Waiting (calling number is correctly 
			//displayed)
			// The user releases the active call (11+SEND)
			// Call Waiting but the screen is empty (yellow screen !)
			// The user answers the call (yellow screen..)
			// Solution: After the scratchpad window destruction, if there is an incoming call, 
			// show the incoming call window

			// Checking whether the there is an incoming call is present or not
			if(call_data.incCall) 
			{
				// Checking the callstatus. If there is a call is waiting and if the user presses 
				// any of the key betweem 0-9, from the function call_incoming_cb(), 
				// call_scratchpad_start() will be called with the status parameter ViewMulticallControlIncomingCall
				// here it's checking whethere the status variable is ViewMulticallControlIncomingCall
				if(callstatus EQ ViewMulticallControlIncomingCall)
				{
				// Checking whether the incoming call window is exisiting
				// IF window is not there, display it.
					if (call_data.win_incoming == 0)
					{	
						call_data.win_incoming = info_dialog(call_data.win, &call_data.inc_call_data);
					}
				}
			}


				
			break;
    		case KCD_RIGHT:
    			TRACE_EVENT ("press right");
 				if (strlen(call_data.edtBufDTMF) > 0)
 				{
 				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
       				ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
        			edtChar(data->edt, ecBack);
#endif /* NEW_EDITOR */
 				}
 				else
 				{
 					TRACE_EVENT ("now destroy");
					call_scratchpad_destroy(data->win);
					// June 22, 2004 MMI-SPR 16573: CW+CLIP : blanck screen
					// Issue description: 1 active call + 1 Call Waiting (calling number is correctly 
					//displayed)
					// Solution: Here also retain the incoming call window.
					// Checking whether the there is an incoming call is present or not
					if(call_data.incCall) 
					{
						// Checking the callstatus. If there is a call is waiting and if the user presses 
						// any of the key betweem 0-9, from the function call_incoming_cb(), 
						// call_scratchpad_start() will be called with the status parameter ViewMulticallControlIncomingCall
						// here it's checking whethere the status variable is ViewMulticallControlIncomingCall
						if(callstatus EQ ViewMulticallControlIncomingCall)
						{
						// Checking whether the incoming call window is exisiting
						// IF window is not there, display it.
							if (call_data.win_incoming == 0)
							{	
								call_data.win_incoming = info_dialog(call_data.win, &call_data.inc_call_data);
							}
						}
					}
 				}
    			break;
    		case KCD_HUP:
			//	May 9, 2006    REF:DR OMAPS00074884  xrashmic        
			// Pressing Red key in scratch pad releses all calls. If there are no calls, 
			//it would destroy the scratch pad.
			if (call_data.calls.numCalls==0)
			{
				call_scratchpad_destroy(data->win);
			}
			else
			{
				cm_end_all();
			}
			break;
    default:
    break;
    }


			/* if true, call the incoming call screen back on the screen because it
			  was destroyed by the scratchpad function */
			if (callstatus EQ ViewMulticallControlIncomingCall && data EQ NULL)
			{
				TRACE_EVENT ("call the incoming call screen back on the screen");
				incomingCall(&call_data.globalCWInfo);
			}
			}
		break;
		default:
		break;
    }
    /* API - 30/10/2003 - SPR 2667 - Remove the call to winShow(call_data.win); and replace with this call
    							  	 this means that all windows will be updated from idle up */

   // xnkulkar SPR-18262: As windowsUpdate() keeps the system busy for more time and hinders the process of
   // sending DTMF commands, it's called when no key is pressed.
    if(key_pressed_dtmf == FALSE)
    {
    windowsUpdate();
    }
    /* API - 30/10/2003 - SPR 2667 - END */
    return MFW_EVENT_CONSUMED;
}





/*******************************************************************************

 $Function:     call_scratchpad_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

static int call_scratchpad_kbd_long_cb (MfwEvt e, MfwKbd *k)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_call_scratchpad         * data = (T_call_scratchpad *)win_data->user;

        if (e & KEY_LONG)
        {
          {
            if (e & KEY_CLEAR)
            {
            call_scratchpad_destroy(data->win);
            return MFW_EVENT_CONSUMED;
            }
            if (e & KEY_0)
            {
              call_data.edtBufDTMF[0] = '+';
              winUpdate((MfwWin*)mfwControl(data->win));
              return MFW_EVENT_CONSUMED;
            }
  	     else
  	     {

		//Sep 2, 2004    REF: CRR 21370 xkundadu
		//Added  the long press of '*' key to display 'w'
			if(e & KEY_STAR)
			{
				#ifdef NEW_EDITOR
					ATB_edit_DeleteLeft(data->editor, FALSE);
					ATB_edit_AsciiChar(data->editor, 'w', TRUE);
				#else
			             	edtChar(data->edt, ecBack);
		        		edtChar(data->edt,'w');
				#endif 

				return MFW_EVENT_CONSUMED;
			}
  	     	}

	     //Sep 2, 2004    REF: CRR 21370 xkundadu
	    //Changed  the long press of '#' key to display 'p'
	    
            if (e & KEY_HASH)
            {
            /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
       		ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
       		ATB_edit_AsciiChar(data->editor, 'p', TRUE);
#else /* NEW_EDITOR */
        	edtChar(data->edt, ecBack);
        	edtChar(data->edt,'p');
#endif /* NEW_EDITOR */

              return MFW_EVENT_CONSUMED;
            }
          }
        }
        return MFW_EVENT_PASSED;
}

UBYTE call_status(void){
  return call_data.callStatus;
}


/*******************************************************************************

 $Function:     call_failure_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_failure_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
 // T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;   // RAVI
//  T_call * data     = (T_call *)win_data->user;                       // RAVI 
  /*
   * Who has initiated the information screen
   */
  TRACE_FUNCTION ("call_failure_cb()");
  switch (reason)
  {
        case INFO_TIMEOUT:
      if (!call_data.calls.numCalls)
     {
        if (call_data.win)
            call_destroy(call_data.win);
     }
    break;
        case INFO_KCD_RIGHT:
          break;
        case INFO_KCD_LEFT:
          break;
        case INFO_KCD_HUP:
        break;
    default:
    break;
  }

  /* SPR#1784 - DS - If failed call was a SAT call, clean up SAT */
  if (call_SATCall == TRUE)
  {
        call_SATCall = FALSE;
        SEND_EVENT(sat_get_call_setup_win(), SAT_CALL_END, 0, NULL);

        /* May be necessary to reinstate if there is no call result editor sent from SAT */
        //SEND_EVENT(sat_get_setup_menu_win(), SAT_RETURN, 0, NULL); /* recreate the SETUP MENU */
  }

}

/*******************************************************************************

 $Function:     call_get_window

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/



MfwHnd call_get_window(void){
  return call_data.win;
}



/*******************************************************************************

 $Function:     showCCNotify

 $Description:  Show result from E_CC_NOTIFY being returned. Short cc-related
            notification window.

 $Returns:

 $Arguments:
*******************************************************************************/


static void showCCNotify (T_MFW_HND win, T_MFW_CM_NOTIFY  * cc_notify)
{
	T_DISPLAY_DATA   display_info;
	USHORT txtId2 = TxtNull;

  TRACE_FUNCTION("++ showCCNotify");


    switch (cc_notify->notification)
    {
      case MFW_CM_HOLD:
        txtId2      = TxtHold;
        break;

      case MFW_CM_RETRIEVED:
        txtId2      = TxtUnhold;
        break;

      default:
        break;
    }

    if (txtId2 != TxtNull )
    {
		dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull,TxtSoftCall, txtId2 , COLOUR_STATUS_CCNOTIFY);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_info_cb, THREE_SECS, KEY_CLEAR | KEY_LEFT );
        display_info.Identifier   = E_CM_NOTIFY;

        /*
         * Call Info Screen
         */

        info_dialog (win, &display_info);
    }
}

/*******************************************************************************

 $Function:     call_info_cb

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void call_info_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
  TRACE_FUNCTION ("call_info_cb()");
}

/*******************************************************************************

 $Function:     StartsendDTMFString

 $Description:

 $Returns:

 $Arguments:
*******************************************************************************/

void StartsendDTMFString(char * String)
{

  strncpy (call_data.edtBufDTMF, String, sizeof(call_data.edtBufDTMF));
  call_data.charSent = 0;
  call_data.sendingDTMF = TRUE;
  callSetMode(ViewSendingDTMF, TRUE);
  sendDTMFString();

}

/*******************************************************************************

 $Function:  menuInCallTimerOn

 $Description: Handles user selection of the "on" option in the In-call Timer menu

 $Returns:  status int

 $Arguments:  not used
*******************************************************************************/
int menuInCallTimerOn(MfwMnu* m, MfwMnuItem* i)
{T_MFW_HND win =  mfwParent( mfw_header() );
  T_DISPLAY_DATA display_info;


    InCallTimer = TRUE;
    flash_write();

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtActivated, TxtInCallTimer, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
    /* Call Info Screen
    */
    info_dialog( win, &display_info );

    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:  menuInCallTimerOff

 $Description: Handles user selection of the "off" option in the In-call Timer menu

 $Returns:  status int

 $Arguments:  not used
*******************************************************************************/
int menuInCallTimerOff(MfwMnu* m, MfwMnuItem* i)
{T_MFW_HND win =  mfwParent( mfw_header() );
  T_DISPLAY_DATA display_info;


    InCallTimer = FALSE;
    flash_write();

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtDeActivated, TxtInCallTimer, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */
    info_dialog( win, &display_info );

    return MFW_EVENT_CONSUMED;

}

/* Marcus: CCBS: 13/11/2002: Start */
/*******************************************************************************

 $Function:     call_initialise_ccbs_list

 $Description:  Initialises the ccbs_list structure

 $Returns:      Nothing

 $Arguments:    T_MFW_HND win   - handle to parent window

*******************************************************************************/
static void call_initialise_ccbs_list(T_MFW_HND win)
{
    UBYTE index;

    TRACE_FUNCTION("call_initialise_ccbs_list()");
    TRACE_EVENT_P1("win = %p", win);

    ccbs_list.win = win;
    ccbs_list.list_menu_data = NULL;
    ccbs_list.count = 0;
    ccbs_list.sel = MAX_CCBS_LIST_ITEMS;    // Deliberately set to an invalid value
    for (index = 0; index < MAX_CCBS_LIST_ITEMS; index++)
    {
        ccbs_list.item[index].idx = 0;      // Deliberately set to an invalid value
        ccbs_list.item[index].number[0] = '\0';
#ifdef NO_ASCIIZ
        ccbs_list.item[index].name.len = 0;
        ccbs_list.item[index].name.data[0] = '\0';
#else
        ccbs_list.item[index].name[0] = '\0';
#endif
    }
}

/*******************************************************************************

 $Function:     call_cm_ccbs_event

 $Description:  Handles E_CM_CCBS events

 $Returns:      Nothing

 $Arguments:    T_MFW_CM_CCBS_INFO *para    - pointer to E_CM_CCBS event data

*******************************************************************************/
static void call_cm_ccbs_event(T_MFW_CM_CCBS_INFO *para)
{
    TRACE_FUNCTION("call_cm_ccbs_event()");

    if ((FFS_flashData.ccbs_status != FALSE) &&
        (FFS_flashData.ccbs_status != TRUE))
    {
        /*
         * CCBS status is not something sensible,
         * so set CCBS status to default
         */
        effs_t ffs_res; // For the result returned by flash_write
        FFS_flashData.ccbs_status = FALSE;
        if ((ffs_res = flash_write()) != EFFS_OK)
        {
             TRACE_EVENT_P1("flash_write returned %d", ffs_res);
        }
        TRACE_EVENT("CCBS status set to default (off)");
    }

    switch (para->operation)
    {
        case CM_CCBS_DELETE_ALL:
            TRACE_EVENT("CM_CCBS_DELETE_ALL");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO unexpected");
                    // No further action required
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK");
                    // No further action required
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    // No further action required
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    break;
            }
            break;

        case CM_CCBS_DELETE:
            TRACE_EVENT("CM_CCBS_DELETE");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO unexpected");
                    // No further action required
                    break;

                case CM_CCBS_OK:    // Completed deleting a specific CCBS
                    TRACE_EVENT("CM_CCBS_OK");
                    call_delete_from_ccbs_list();

                    if (ccbs_list.count == 0)
                    {
                        bookShowInformation(ccbs_list.win, TxtEmptyList, NULL, NULL);
                    }
                    else
                    {
                        call_show_ccbs_menu_list();
                    }
                    break;

                case CM_CCBS_ERROR: // Failed to delete a specific CCBS
                    TRACE_EVENT("CM_CCBS_ERROR");
                    call_show_ccbs_menu_list();
                    bookShowInformation(ccbs_list.win, TxtFailed, NULL, NULL);
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    // No further action required
                    break;
            }
            break;

        case CM_CCBS_LIST:
            TRACE_EVENT("CM_CCBS_LIST");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO");
                    call_add_to_ccbs_list(para);
                    break;

                case CM_CCBS_OK:    // Completed obtaining the list of CCBS
                    TRACE_EVENT("CM_CCBS_OK");
                    if (ccbs_list.count == 0)
                    {
                        call_ccbs_show_status();
                        bookShowInformation(ccbs_list.win, TxtEmptyList, NULL, NULL);
                    }
                    else
                    {
                        call_show_ccbs_menu_list();
                    }
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    call_ccbs_show_status();
                    bookShowInformation(ccbs_list.win, TxtFailed, NULL, NULL);
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    // No further action required
                    break;
            }
            break;

        case CM_CCBS_ASYNC:
            TRACE_EVENT("CM_CCBS_ASYNC");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO");

                    if (FFS_flashData.ccbs_status != TRUE)
                    {
                        TRACE_EVENT("CCBS deactivated");
                        break;
                    }

                    switch(para->indct)
                    {
                        case CCBS_IND_Possible:
                            TRACE_EVENT("CCBS_IND_Possible");
                            call_offer_ccbs_registration(para);
                            break;

                        case CCBS_IND_PossibilityTimedOut:
                            TRACE_EVENT("CCBS_IND_PossibilityTimedOut");
                            call_withdraw_ccbs_registration(TxtTimedOut);
                            break;

                        case CCBS_IND_Recall:
                            TRACE_EVENT("CCBS_IND_Recall");
                            call_offer_ccbs_recall(para);
                            break;

                        case CCBS_IND_RecallTimedOut:
                            TRACE_EVENT("CCBS_IND_RecallTimedOut");
                            call_withdraw_ccbs_recall(TxtTimedOut);
                            break;

                        default:
                            TRACE_EVENT_P1("Unexpected indct %d", para->indct);
                            // No further action required
                            break;
                    }
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK unexpected");
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR unexpected");
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    break;
            }
            break;

        case CM_CCBS_ACCEPT_REGISTRATION:
            TRACE_EVENT("CM_CCBS_ACCEPT_REGISTRATION");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO");
                    // No further action required - CCBS_IND_Registered ignored
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK");
                    call_withdraw_ccbs_registration(TxtAcc);
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    call_withdraw_ccbs_registration(TxtFailed);
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    break;
            }
            break;

        case CM_CCBS_REJECT_REGISTRATION:
            TRACE_EVENT("CM_CCBS_REJECT_REGISTRATION");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO unexpected");
                    // No further action required
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK");
                    // No further action required
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    // No further action required
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    // No further action required
                    break;
            }
            break;

        case CM_CCBS_ACCEPT_RECALL:
            TRACE_EVENT("CM_CCBS_ACCEPT_RECALL");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO unexpected");
                    // No further action required
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK");
                    call_withdraw_ccbs_recall(TxtAcc);
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    call_withdraw_ccbs_recall(TxtFailed);
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    // No further action required
                    break;
            }
            break;

        case CM_CCBS_REJECT_RECALL:
            TRACE_EVENT("CM_CCBS_REJECT_RECALL");

            switch(para->type)
            {
                case CM_CCBS_INFO:
                    TRACE_EVENT("CM_CCBS_INFO unexpected");
                    // No further action required
                    break;

                case CM_CCBS_OK:
                    TRACE_EVENT("CM_CCBS_OK");
                    call_withdraw_ccbs_recall(TxtRejected);
                    break;

                case CM_CCBS_ERROR:
                    TRACE_EVENT("CM_CCBS_ERROR");
                    call_withdraw_ccbs_recall(TxtRejected);
                    break;

                default:
                    TRACE_EVENT_P1("Unexpected type %d", para->type);
                    // No further action required
                    break;
            }
            break;

        case CM_CCBS_NONE:
            TRACE_EVENT("CM_CCBS_NONE unexpected");
            // No further action required
            break;

        default:
            TRACE_EVENT_P1("Unexpected operation %d", para->operation);
            break;
    }
}

/*******************************************************************************

 $Function:     call_add_to_ccbs_list

 $Description:  Adds an entry to a CCBS list structure

 $Returns:      Nothing

 $Arguments:    T_MFW_CM_CCBS_INFO *para    - pointer to E_CM_CCBS event data

*******************************************************************************/
static void call_add_to_ccbs_list(T_MFW_CM_CCBS_INFO *para)
{
    TRACE_FUNCTION("call_add_to_ccbs_list()");

    if (para->indct != CCBS_IND_IrgtResult)
    {
        TRACE_EVENT_P1("Unexpected indct %d", para->indct);
    }
    else if (para->status != CCBS_STAT_Active)
    {
        TRACE_EVENT_P1("Unexpected status %d", para->status);
    }
    else if ((para->idx < MIN_CCBS_IDX) || (para->idx > MAX_CCBS_IDX))
    {
        TRACE_EVENT_P1("Unexpected idx %d", para->idx);
    }
    else if (ccbs_list.count >= MAX_CCBS_LIST_ITEMS)
    {
        TRACE_EVENT_P1("ccbs_list.count = %d", ccbs_list.count);
    }
    else
    {
        /* Define a pointer for efficient access to the item to be filled */
        // T_CCBS_LIST_ITEM_STR *item = &ccbs_list.item[ccbs_list.count];
        T_MFW_PHB_ENTRY entry;  // Structure to pass to bookFindNameInPhonebook
        int max_num_size = sizeof(ccbs_list.item[ccbs_list.count].number);
		
        TRACE_EVENT_P1("ccbs_list.count = %d", ccbs_list.count);
        // TRACE_EVENT_P1("item: %p", item);

        memset((char *)ccbs_list.item[ccbs_list.count].number, 0x00, max_num_size);

        ccbs_list.item[ccbs_list.count].idx = para->idx;
        if ((para->ton == MFW_TON_INTERNATIONAL) && (para->number[0] != '+'))
        {
            strcpy((char *)ccbs_list.item[ccbs_list.count].number, "+");
	     max_num_size--;
        }

        strncat((char *)ccbs_list.item[ccbs_list.count].number, (char *)para->number, max_num_size);
        TRACE_EVENT_P1("number %s", ccbs_list.item[ccbs_list.count].number);
        if (bookFindNameInPhonebook((char*)para->number, &entry))
        {
            /* The number corresponds to a name in the phonebook */
#ifdef NO_ASCIIZ
            memcpy(&ccbs_list.item[ccbs_list.count].name, &entry.name, sizeof(ccbs_list.item[ccbs_list.count].name));
            if ((dspl_get_char_type() == DSPL_TYPE_UNICODE) &&
                (ccbs_list.item[ccbs_list.count].name.data[0] == 0x80))
            {
                /* UniCode */
                TRACE_EVENT("Prefixed unicode name");
                if (ccbs_list.item[ccbs_list.count].name.len < (sizeof(ccbs_list.item[ccbs_list.count].name.data)-2))
                {
                    ccbs_list.item[ccbs_list.count].name.data[ccbs_list.item[ccbs_list.count].name.len] = '\0';
                    ccbs_list.item[ccbs_list.count].name.data[ccbs_list.item[ccbs_list.count].name.len+1] = '\0';
                }
                else
                {
                    ccbs_list.item[ccbs_list.count].name.data[sizeof(ccbs_list.item[ccbs_list.count].name.data)-2] = '\0';
                    ccbs_list.item[ccbs_list.count].name.data[sizeof(ccbs_list.item[ccbs_list.count].name.data)-1] = '\0';
                }
            }
            else    /* not UniCode */
            {
                TRACE_EVENT("Not prefixed unicode: ASCII name");
                if (ccbs_list.item[ccbs_list.count].name.len < (sizeof(ccbs_list.item[ccbs_list.count].name.data)-1))
                {
                    ccbs_list.item[ccbs_list.count].name.data[ccbs_list.item[ccbs_list.count].name.len] = '\0';
                }
                else
                {
                    ccbs_list.item[ccbs_list.count].name.data[sizeof(ccbs_list.item[ccbs_list.count].name.data)-1] = '\0';
                }
                bookGsm2Alpha(ccbs_list.item[ccbs_list.count].name.data);
                TRACE_EVENT_P2("name.data %p %s", ccbs_list.item[ccbs_list.count].name.data, ccbs_list.item[ccbs_list.count].name.data);
            }
            TRACE_EVENT_P1("name.len %d", ccbs_list.item[ccbs_list.count].name.len);
#else
            strncpy(ccbs_list.item[ccbs_list.count].name, entry.name, sizeof(ccbs_list.item[ccbs_list.count].name)-1);
            ccbs_list.item[ccbs_list.count].name[sizeof(ccbs_list.item[ccbs_list.count].name)-1] = '\0';
            bookGsm2Alpha(ccbs_list.item[ccbs_list.count].name);
            TRACE_EVENT_P1("name %s", ccbs_list.item[ccbs_list.count].name);
#endif
        }
        ccbs_list.count++;
    }
}

/*******************************************************************************

 $Function:     call_delete_from_ccbs_list

 $Description:  Deletes an entry from a CCBS list structure

 $Returns:      Nothing

 $Arguments:    None

*******************************************************************************/
static void call_delete_from_ccbs_list(void)
{
    TRACE_FUNCTION("call_delete_from_ccbs_list()");

    if (ccbs_list.count == 0)
    {
        TRACE_EVENT("List empty");
    }
    else if (ccbs_list.sel >= ccbs_list.count)
    {
        TRACE_EVENT_P2("Invalid item %d selected from %d", ccbs_list.sel, ccbs_list.count);
    }
    else
    {
        TRACE_EVENT_P2("Valid item %d selected from %d", ccbs_list.sel, ccbs_list.count);
        if (ccbs_list.sel < (ccbs_list.count-1))
        {
            UBYTE index;
            TRACE_EVENT("Not the last item selected");
            for (index = ccbs_list.sel; index < (ccbs_list.count-1); index++)
            {
                TRACE_EVENT_P2("Moving item %d to item %d", index+1, index);
                memcpy(&ccbs_list.item[index], &ccbs_list.item[index+1],
                    sizeof(ccbs_list.item[index]));
            }
        }
        ccbs_list.count--;
        TRACE_EVENT_P1("%d items remain", ccbs_list.count);
    }
}

/*******************************************************************************

 $Function:     call_create_ccbs_menu_list

 $Description:	Creates a list of registered CCBS for selection.

 $Returns:		Nothing

 $Arguments:	None

*******************************************************************************/
static void call_create_ccbs_menu_list(void)
{
    UBYTE index;

    TRACE_FUNCTION("call_create_ccbs_menu_list()");

    if (ccbs_list.count == 0)
    {
        TRACE_EVENT("Should not be an empty list");
        return;
    }

    ccbs_list.list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof (ListMenuData));

    TRACE_EVENT_P2("ALLOC_MEMORY for ccbs_list.list_menu_data: %d, %p", sizeof (ListMenuData), ccbs_list.list_menu_data);
    if (ccbs_list.list_menu_data == NULL)
    {
        return;
    }

    ccbs_list.list_menu_data->List =
        (T_MFW_MNU_ITEM *) ALLOC_MEMORY (ccbs_list.count * sizeof (T_MFW_MNU_ITEM));

    TRACE_EVENT_P2("ALLOC_MEMORY for ccbs_list.list_menu_data->List: %d, %p",
        ccbs_list.count * sizeof (T_MFW_MNU_ITEM), ccbs_list.list_menu_data->List);

    if (ccbs_list.list_menu_data->List == NULL)
    {
        TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data: %d, %p", sizeof (ListMenuData), ccbs_list.list_menu_data);
        FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data, sizeof (ListMenuData));
        ccbs_list.list_menu_data = NULL;
        return;
    }

    /*
     * Fill in the items in the menu list
     */
    TRACE_EVENT_P1("ccbs_list.count: %d", ccbs_list.count);

    for (index = 0; index < ccbs_list.count; index++)
    {
        TRACE_EVENT_P1("index: %d", index);
        mnuInitDataItem(&ccbs_list.list_menu_data->List[index]);

#ifdef NO_ASCIIZ
        if (ccbs_list.item[index].name.len != 0 )
        {
            ccbs_list.list_menu_data->List[index].str = (char *)ccbs_list.item[index].name.data;
            TRACE_EVENT_P2("name.data: %p %s", ccbs_list.item[index].name.data, ccbs_list.item[index].name.data);
	    }
#else
        if (strlen((char *)ccbs_list.item[index].name) != 0)
        {
            ccbs_list.list_menu_data->List[index].str = (char *)ccbs_list.item[index].name;
	    }
#endif
        else
        {
            ccbs_list.list_menu_data->List[index].str = (char *)ccbs_list.item[index].number;
            TRACE_EVENT_P2("number: %p %s", ccbs_list.item[index].number, ccbs_list.item[index].number);
        }

        TRACE_EVENT_P3("[%d] %p %s", index, ccbs_list.list_menu_data->List[index].str, ccbs_list.list_menu_data->List[index].str);
        ccbs_list.list_menu_data->List[index].flagFunc = item_flag_none;
    }

    TRACE_EVENT_P1("index: %d", index);
    /*
     * Fill common parameter for list handling
     */
    ccbs_list.list_menu_data->ListLength      = index;
    ccbs_list.list_menu_data->ListPosition    = 1;
    ccbs_list.list_menu_data->CursorPosition  = 1;
    ccbs_list.list_menu_data->SnapshotSize    = index;
    ccbs_list.list_menu_data->Font            = 0;
    ccbs_list.list_menu_data->LeftSoftKey     = TxtDelete;
    ccbs_list.list_menu_data->RightSoftKey    = TxtSoftBack;
    ccbs_list.list_menu_data->KeyEvents       =
        KCD_HUP | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;
    ccbs_list.list_menu_data->Reason          = 0;
    ccbs_list.list_menu_data->Strings         = TRUE;
    ccbs_list.list_menu_data->Attr            = &ccbs_list_menuAttrib;
    ccbs_list.list_menu_data->autoDestroy     = TRUE;
    ccbs_list.list_menu_data->listCol         = COLOUR_LIST_XX;
    Mmi_set_area_attr(&ccbs_list_menuArea,0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());

}

/*******************************************************************************

 $Function:     call_ccbs_menu_list_cb

 $Description:	Callback function for the registered CCBS list

 $Returns:      Nothing

 $Arguments:	T_MFW_HND win   - the list's window handle
                ListMenuData *  - pointer to list data

*******************************************************************************/
static void call_ccbs_menu_list_cb(T_MFW_HND win, ListMenuData * ListData)
{
    T_MFW res;    // The value returned fron cm_ccbs_delete

    TRACE_FUNCTION("call_ccbs_menu_list_cb()");

    /* Make record of the selected item for deletion if OK */
    ccbs_list.sel = ListData->ListPosition;

    switch (ListData->Reason)
    {
        case LISTS_REASON_SELECT:

            TRACE_EVENT("LISTS_REASON_SELECT");
#ifdef NO_ASCIIZ
            if (ccbs_list.item[ccbs_list.sel].name.len != 0 )
            {
    		    TRACE_EVENT_P3("selected item %d idx %d %s",
    		        ccbs_list.sel,
    		        ccbs_list.item[ccbs_list.sel].idx,
    		        (char *)ccbs_list.item[ccbs_list.sel].name.data);
    	    }
#else
            if (strlen((char *)ccbs_list.item[ccbs_list.sel].name) != 0 )
            {
    	        TRACE_EVENT_P3("selected item %d idx %d %s",
    	            ccbs_list.sel,
    	            ccbs_list.item[ccbs_list.sel].idx,
    	            (char *)ccbs_list.item[ccbs_list.sel].name);
    	    }
#endif
            else
            {
                TRACE_EVENT_P3("selected item %d idx %d %s",
                    ccbs_list.sel,
                    ccbs_list.item[ccbs_list.sel].idx,
                    (char *)ccbs_list.item[ccbs_list.sel].number);
            }

            if ((res = cm_ccbs_delete(ccbs_list.item[ccbs_list.sel].idx)) != CM_OK)
            {
                TRACE_EVENT_P1("cm_ccbs_delete returned %d", res);
                TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data->List: %d, %p",
                    ccbs_list.count * sizeof (T_MFW_MNU_ITEM),
                    ccbs_list.list_menu_data->List);
                FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data->List,
                    ccbs_list.count * sizeof (T_MFW_MNU_ITEM));
                TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data: %d, %p",
                    sizeof (ListMenuData),
                    ccbs_list.list_menu_data);
                FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data,
                    sizeof (ListMenuData));
                /* Redisplay the registered CCBS list */
                call_show_ccbs_menu_list();
                bookShowInformation(ccbs_list.win, TxtFailed, NULL, NULL);
                return;
            }
            break;

        case LISTS_REASON_BACK:
        case LISTS_REASON_HANGUP:

            TRACE_EVENT("LISTS_REASON_BACK/LISTS_REASON_HANGUP");

            /* Redisplay the Callback Status */
            call_ccbs_show_status();
            break;

        default:
            TRACE_EVENT_P1("Unexpected ListData->Reason %d", ListData->Reason);
            break;
    }

    TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data->List: %d, %p",
        ccbs_list.count * sizeof (T_MFW_MNU_ITEM),
        ccbs_list.list_menu_data->List);
    FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data->List,
        ccbs_list.count * sizeof (T_MFW_MNU_ITEM));
    TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data: %d, %p",
        sizeof (ListMenuData),
        ccbs_list.list_menu_data);
    FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data,
        sizeof (ListMenuData));
    ccbs_list.list_menu_data = NULL;

}

/*******************************************************************************

 $Function:     call_show_ccbs_menu_list

 $Description:  Shows the list of registered CCBS

 $Returns:      Nothing

 $Arguments:    None

*******************************************************************************/
static void call_show_ccbs_menu_list(void)
{
    T_MFW res;
    TRACE_FUNCTION("call_show_ccbs_menu_list()");
    call_create_ccbs_menu_list();
    if ((res = listDisplayListMenu(
        ccbs_list.win, ccbs_list.list_menu_data,
        (ListCbFunc)call_ccbs_menu_list_cb, 0)) != LISTS_OK)
    {
        TRACE_EVENT_P1("listDisplayListMenu returned %d", res);
        TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data->List: %d, %p",
            ccbs_list.count * sizeof (T_MFW_MNU_ITEM),
            ccbs_list.list_menu_data->List);
        FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data->List,
            ccbs_list.count * sizeof (T_MFW_MNU_ITEM));
        TRACE_EVENT_P2("FREE_MEMORY for ccbs_list.list_menu_data: %d, %p",
            sizeof (ListMenuData),
            ccbs_list.list_menu_data);
        FREE_MEMORY ((UBYTE *)ccbs_list.list_menu_data,
            sizeof (ListMenuData));
        ccbs_list.list_menu_data = NULL;
        bookShowInformation(ccbs_list.win, TxtFailed, NULL, NULL);
    }
}

/*******************************************************************************

 $Function:     call_offer_ccbs_registration

 $Description:  Offers an opportunity for CCBS registration to the user

 $Returns:      Nothing

 $Arguments:    T_MFW_CM_CCBS_INFO *para    - pointer to E_CM_CCBS event data

*******************************************************************************/
static void call_offer_ccbs_registration(T_MFW_CM_CCBS_INFO *para)
{
    TRACE_FUNCTION("call_offer_ccbs_registration()");

    if (para->indct != CCBS_IND_Possible)
    {
        TRACE_EVENT_P1("Unexpected indct %d", para->indct);
    }
    else
    {
        T_DISPLAY_DATA  display_info ;  // Structure for info_dialog

        /* Marcus: Issue 1630: 24/01/2003: Start */
        dlg_initDisplayData_TextId( &display_info, TxtYes, TxtNo, TxtCallBackQuery, TxtNull, COLOUR_STATUS);
        dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_offer_ccbs_registration_callback, FOREVER, KEY_HUP | KEY_LEFT | KEY_RIGHT);
        /* Marcus: Issue 1630: 24/01/2003: End */

        ccbs_registration_win = info_dialog(NULL, &display_info);
        TRACE_EVENT_P3("info_dialog(%p, %p): %p",
            NULL, &display_info, ccbs_registration_win);
    }
}
/*******************************************************************************

 $Function:    	call_offer_ccbs_registration_callback

 $Description:	Handle the Callback Registration Query callback

 $Returns:		int: MFW_EVENT_CONSUMED	or MFW_EVENT_REJECTED

 $Arguments:    T_MFW_HND win       - pointer to the parent window
                USHORT identifier   - Identifier supplied in T_DISPLAY_DATA
                UBYTE reason        - The reason for the call

 $Note:         The current Callback Registration Query window is about to be destroyed

*******************************************************************************/

static int call_offer_ccbs_registration_callback(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    int     res = MFW_EVENT_CONSUMED;   // Result code to be returned by this function
    T_MFW   mfw_res;    // Result code returned by MFW

    TRACE_FUNCTION("call_offer_ccbs_registration_callback()");
    TRACE_EVENT_P3("win %p: identifier %d: reason %d", win, identifier, reason);

    switch(reason)
    {
        case INFO_KCD_RIGHT:    // Reject offer of CCBS registration
            TRACE_EVENT("INFO_KCD_RIGHT");
            if ((mfw_res = cm_ccbs_reject_registration()) != CM_OK)
            {
                TRACE_EVENT_P1("cm_ccbs_reject_registration returned %d", mfw_res);
                bookShowInformation(ccbs_registration_win, TxtFailed, NULL, NULL);
            }
            break;

        case INFO_KCD_LEFT: // Accept the offer of CCBS registration
            TRACE_EVENT("INFO_KCD_LEFT");
            if ((mfw_res = cm_ccbs_accept_registration()) != CM_OK)
            {
                TRACE_EVENT_P1("cm_ccbs_accept_registration returned %d", mfw_res);
                bookShowInformation(ccbs_registration_win, TxtFailed, NULL, NULL);
            }
            break;

        case INFO_KCD_HUP:  // Ignore the offer of CCBS registration
            TRACE_EVENT("INFO_KCD_HUP");
            // No further action required
            break;

        default:
            TRACE_EVENT_P1("Unexpected reason: %d", reason);
            res = MFW_EVENT_REJECTED;
            break;
    }

    /*
     * The current Callback Registration Query window is about to be destroyed
     * so the window handle is no longer valid.
     */
    ccbs_registration_win = NULL;

    return res;
}


/*******************************************************************************

 $Function:     call_withdraw_ccbs_registration

 $Description:  Withdraws an opportunity for CCBS registration to the user

 $Returns:      Nothing

 $Arguments:    T_MFW_CM_CCBS_INFO *para    - pointer to E_CM_CCBS event data

*******************************************************************************/
static void call_withdraw_ccbs_registration(LangTxt TxtId)
{
    T_DISPLAY_DATA  display_info ;  // Structure for info_dialog

    TRACE_FUNCTION("call_withdraw_ccbs_registration()");

    if (ccbs_registration_win != NULL)
    {
        TRACE_EVENT_P1("Destroying dialog %p", ccbs_registration_win);
        SEND_EVENT(ccbs_registration_win, DIALOG_DESTROY, 0, 0);
        ccbs_registration_win = NULL;
    }

    /* Marcus: Issue 1630: 24/01/2003: Start */
    dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallBack, TxtId, COLOUR_STATUS);
    dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_HUP | KEY_LEFT | KEY_RIGHT);
    /* Marcus: Issue 1630: 24/01/2003: End */

    info_dialog(NULL, &display_info);
    TRACE_EVENT_P2("info_dialog(%p, %p)", NULL, &display_info);
}

/*******************************************************************************

 $Function:     unicode_strcpy

 $Description:  UniCode equivalent of ASCII strcpy

 $Returns:      Nothing

 $Arguments:    pointer to destination unicode string
                pointer to source unicode string

*******************************************************************************/
static void unicode_strcpy(USHORT *dst, USHORT *src)
{
    UCHAR *first = (UCHAR *)src;

    if (*first == 0x80)
        src++;

    while (*dst++ = *src++);
}

/*******************************************************************************

 $Function:     unicode_strcat

 $Description:  UniCode equivalent of ASCII strcat

 $Returns:      Nothing

 $Arguments:    pointer to destination unicode string
                pointer to source unicode string

*******************************************************************************/
static void unicode_strcat(USHORT *dst, USHORT *src)
{
    UCHAR *first = (UCHAR *)src;

    if (*first == 0x80)
        src++;

    while (*dst)
        dst++;

    while (*dst++ = *src++);
}

/*******************************************************************************

 $Function:     unicode_strlen

 $Description:  UniCode equivalent of ASCII strlen

 $Returns:      size_t - length of UniCode string (including and 0x80 prefix)

 $Arguments:    pointer to destination unicode string
                pointer to source unicode string

*******************************************************************************/
static size_t unicode_strlen(USHORT *src)
{
    size_t count = 0;

    while (*src++)
        count += sizeof(USHORT);

    return count;
}

/*******************************************************************************

 $Function:     ascii_unicode_strcpy

 $Description:  UniCode equivalent of ASCII strcpy with ASCII source

 $Returns:      Nothing

 $Arguments:    pointer to destination unicode string
                pointer to source unicode string

*******************************************************************************/
static void ascii_unicode_strcpy(USHORT *dst, UCHAR *src)
{
    UBYTE *udst = (UBYTE *)dst;
    do
    {
        *udst = 0;
         udst++;
    }
    while (*udst++ = *src++);
}

/*******************************************************************************

 $Function:     ascii_unicode_strcat

 $Description:  UniCode equivalent of ASCII strcat with ASCII source

 $Returns:      Nothing

 $Arguments:    pointer to destination unicode string
                pointer to source unicode string

*******************************************************************************/
static void ascii_unicode_strcat(USHORT *dst, UCHAR *src)
{
    UBYTE *udst = (UBYTE *)dst + unicode_strlen(dst);
    ascii_unicode_strcpy((USHORT *)udst, src);
}

/*******************************************************************************

 $Function:     fill_ccbs_recall_text_buffer

 $Description:  Fills the ccbs_recall_text_buffer

 $Returns:      Length (in bytes) of the contents of the ccbs_recall_text_buffer

 $Arguments:    LangTxt TxtId - text identifier of the final line

*******************************************************************************/
static size_t fill_ccbs_recall_text_buffer(LangTxt TxtId)
{
    size_t size;

    TRACE_FUNCTION("fill_ccbs_recall_text_buffer");

    if ((dspl_get_char_type() == DSPL_TYPE_UNICODE) &&
        (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE))
    {
        /* Chinese UniCode - MmiRsrcGetText returns UniCode */
        TRACE_EVENT("Chinese UniCode - UniCode TxtIds");

        /*MC, SPR1526, don't need to add unicode tag any more*/
        unicode_strcpy((USHORT *)&ccbs_recall_text_buffer[0],
            (USHORT *)MmiRsrcGetText(TxtCallBack));
        ascii_unicode_strcat((USHORT *)ccbs_recall_text_buffer, (UCHAR *)"\n");
#ifdef NO_ASCIIZ
        if (ccbs_recall.name.len > 0)
        {
            if (ccbs_recall.name.data[0] == 0x80)
            {
                TRACE_EVENT("NO_ASCIIZ UniCode name");
                unicode_strcat((USHORT *)ccbs_recall_text_buffer,
                    (USHORT *)ccbs_recall.name.data);
            }
            else
            {
                TRACE_EVENT("NO_ASCIIZ ASCII name");
                ascii_unicode_strcat((USHORT *)ccbs_recall_text_buffer,
                    ccbs_recall.name.data);
            }
        }
#else
        if (ccbs_recall.name[0] == 0x80)
        {
            TRACE_EVENT("ASCIIZ UniCode name");
            unicode_strcat((USHORT *)ccbs_recall_text_buffer,
                (USHORT *)ccbs_recall.name);
        }
        else if (strlen((char *)ccbs_recall.name) > 0)
        {
            TRACE_EVENT("ASCIIZ ASCII name");
            ascii_unicode_strcat((USHORT *)ccbs_recall_text_buffer,
                (char *)ccbs_recall.name);
        }
#endif
        else
        {
            TRACE_EVENT("ASCII number");
            ascii_unicode_strcat((USHORT *)ccbs_recall_text_buffer,
                (UCHAR *)ccbs_recall.number);
        }
        ascii_unicode_strcat((USHORT *)ccbs_recall_text_buffer, (UCHAR *)"\n");
        unicode_strcat((USHORT *)ccbs_recall_text_buffer,
            (USHORT *)MmiRsrcGetText(TxtId));

    	size = unicode_strlen((USHORT *)ccbs_recall_text_buffer);
    }
    else    /* Not Chinese UniCode */
    {
        int max_recallbuf_size = sizeof(ccbs_recall_text_buffer);
	 int cur_recallbuf_size = 0;
        TRACE_EVENT("Not Chinese UniCode - ASCII TxtIds");
        strcpy(ccbs_recall_text_buffer, MmiRsrcGetText(TxtCallBack));
        strcat(ccbs_recall_text_buffer, "\n");

	 cur_recallbuf_size = strlen(ccbs_recall_text_buffer);
	 max_recallbuf_size -= cur_recallbuf_size;
	
#ifdef NO_ASCIIZ
        if (ccbs_recall.name.len > 0)
        {
            TRACE_EVENT("NO_ASCIIZ ASCII name");
            strncat(ccbs_recall_text_buffer, (char *)ccbs_recall.name.data, max_recallbuf_size);
	     max_recallbuf_size -= strlen((char *)ccbs_recall.name.data);
        }
#else
        if (strlen((char *)ccbs_recall.name) > 0)
        {
            TRACE_EVENT("ASCIIZ ASCII name");
            strncat(ccbs_recall_text_buffer, (char *)ccbs_recall.name, max_recallbuf_size);
	     max_recallbuf_size -= strlen((char *)ccbs_recall.name);
        }
#endif
        else
        {
            TRACE_EVENT("ASCII number");
            strncat(ccbs_recall_text_buffer, (char *)ccbs_recall.number, max_recallbuf_size);
	     max_recallbuf_size -= strlen((char *)ccbs_recall.number);
        }

	if (max_recallbuf_size > 1)
	{
	        strcat(ccbs_recall_text_buffer, "\n");
	        strncat(ccbs_recall_text_buffer, MmiRsrcGetText(TxtId), max_recallbuf_size-1);
	}

    	size = strlen(ccbs_recall_text_buffer);
    }

    TRACE_EVENT_P1("size = %d", size);

    return size;
}

/*******************************************************************************

 $Function:     call_offer_ccbs_recall

 $Description:  Offers an opportunity for CCBS recall to the user

 $Returns:      Nothing

 $Arguments:    T_MFW_CM_CCBS_INFO *para    - pointer to E_CM_CCBS event data

*******************************************************************************/
static void call_offer_ccbs_recall(T_MFW_CM_CCBS_INFO *para)
{
    size_t size;   /* The length of the string to be displayed */
    LangTxt TxtId;

    TRACE_FUNCTION("call_offer_ccbs_recall()");

    if (para->indct != CCBS_IND_Recall)
    {
        TRACE_EVENT_P1("Unexpected indct %d", para->indct);
    }
    else
    {
    	/* SPR#1428 - SH - New editor changes */
    	#ifdef NEW_EDITOR
    	T_AUI_EDITOR_DATA editor_data;
   		#else /* NEW_EDITOR */
        T_EDITOR_DATA editor_data;  // Structure for (read-only) editor
    	#endif /* NEW_EDITOR */

        T_MFW_PHB_ENTRY entry;  // Structure to pass to bookFindNameInPhonebook

	 int max_recall_size = sizeof(ccbs_recall.number);

        /* Clear the ccbs_recall structure for a new CCBS recall offering */
        memset((void *)&ccbs_recall, 0, sizeof(ccbs_recall));

        /* Fill in the fields of the ccbs_recall structure */
        ccbs_recall.type = para->call_type;

        if ((para->ton == MFW_TON_INTERNATIONAL) && (para->number[0] != '+'))
        {
            strcpy((char *)ccbs_recall.number, "+");
	     max_recall_size--;
        }

        strncat((char *)ccbs_recall.number, (char *)para->number, max_recall_size);
        TRACE_EVENT_P1("number %s", ccbs_recall.number);
        if (bookFindNameInPhonebook((char*)para->number, &entry))
        {
            /* The number corresponds to a name in the phonebook */
#ifdef NO_ASCIIZ
            memcpy(&ccbs_recall.name, &entry.name, sizeof(ccbs_recall.name));
            if ((dspl_get_char_type() == DSPL_TYPE_UNICODE) &&
                (ccbs_recall.name.data[0] == 0x80))
            {
                /* UniCode */
                TRACE_EVENT("Prefixed unicode name");
                if (ccbs_recall.name.len < (sizeof(ccbs_recall.name.data)-2))
                {
                    ccbs_recall.name.data[ccbs_recall.name.len] = '\0';
                    ccbs_recall.name.data[ccbs_recall.name.len+1] = '\0';
                }
                else
                {
                    ccbs_recall.name.data[sizeof(ccbs_recall.name.data)-2] = '\0';
                    ccbs_recall.name.data[sizeof(ccbs_recall.name.data)-1] = '\0';
                }
            }
            else    /* Not UniCode */
            {
                TRACE_EVENT("Not prefixed unicode: ASCII name");
                if (ccbs_recall.name.len < (sizeof(ccbs_recall.name.data)-1))
                {
                    ccbs_recall.name.data[ccbs_recall.name.len] = '\0';
                }
                else
                {
                    ccbs_recall.name.data[sizeof(ccbs_recall.name.data)-1] = '\0';
                }
                bookGsm2Alpha(ccbs_recall.name.data);
                TRACE_EVENT_P1("name %s", ccbs_recall.name.data);
            }
            TRACE_EVENT_P1("name len %d", ccbs_recall.name.len);
#else
            strncpy(ccbs_recall.name, entry.name, sizeof(ccbs_recall.name)-1);
            ccbs_recall.name[sizeof(ccbs_recall.name)-1] = '\0';
            bookGsm2Alpha(ccbs_recall.name);
            TRACE_EVENT_P1("name %s", ccbs_recall.name);
#endif
        }

       	switch (ccbs_recall.type)
       	{
       	    case VOICE_CALL:
     		    TxtId = TxtVoice;
       		    break;

       	    case DATA_CALL:
       		    TxtId = TxtData;
       		    break;

       	    case FAX_CALL:
      		    TxtId = TxtFax;
      		    break;

       	    default:
      		    TxtId = TxtNull;
       		    TRACE_EVENT_P1("Unexpected ccbs_recall.type %d", ccbs_recall.type);
       		    break;
        }

        size = fill_ccbs_recall_text_buffer(TxtId);

        /* SPR#1428 - SH - New editor changes */
#ifdef NEW_EDITOR
    	AUI_edit_SetDefault(&editor_data);
    	AUI_edit_SetDisplay(&editor_data, ZONE_FULLSOFTKEYS, COLOUR_EDITOR_XX, EDITOR_FONT);
    	AUI_edit_SetTextStr(&editor_data, TxtAccept, TxtReject, TxtNull, 0);
    	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)call_offer_ccbs_recall_callback);
    	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ccbs_recall_text_buffer, size);
    	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
        if ((ccbs_recall.win = AUI_edit_Start(NULL, &editor_data)) != NULL)
        {
            TRACE_EVENT_P3("AUI_edit_Start(%p, %p): %p", NULL, &editor_data, ccbs_recall.win);
// Apr 05, 2005    REF: ENH 29994 xdeepadh		
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_IC,true,sounds_midi_ringer_start_cb);
#else
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_RINGTONE /* MSL Ring tone */
           audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0 , AUDIO_PLAY_ONCE );
#else
     audio_PlaySoundID(AUDIO_BUZZER, getCurrentRingerSettings(), 
                                      getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
#else
            audio_PlaySoundID(AUDIO_BUZZER, getCurrentRingerSettings(), 0 , AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */

#endif
        }
        else
        {
            TRACE_EVENT("AUI_edit_Start failed");
        }
#else /* NEW_EDITOR */
        editor_attr_init(&editor_data.editor_attr, ZONE_FULLSOFTKEYS,
            edtCurNone, 0, ccbs_recall_text_buffer, size, COLOUR_EDITOR_XX);
        editor_data_init(&editor_data, (T_EDIT_CB)call_offer_ccbs_recall_callback,
            TxtAccept, TxtReject, TxtNull, 0, READ_ONLY_MODE, FOREVER);
   		if ((ccbs_recall.win = editor_start(NULL, &editor_data)) != NULL)
        {
            TRACE_EVENT_P3("editor_start(%p, %p): %p",
                NULL, &editor_data, ccbs_recall.win);
// Apr 05, 2005    REF: ENH 29994 xdeepadh		
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_IC,true,sounds_midi_ringer_start_cb);
#else
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_RINGTONE /* MSL Ring Tone */
           audio_PlaySoundID(AUDIO_BUZZER, CALLTONE_SELECT, 0 , AUDIO_PLAY_ONCE );
#else
	audio_PlaySoundID(AUDIO_BUZZER, getCurrentRingerSettings(), 
                                        getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
#else
            audio_PlaySoundID(AUDIO_BUZZER, getCurrentRingerSettings(), 0 , AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
#endif
        }
        else
        {
            TRACE_EVENT("editor_start failed");
        }
#endif

    }
}

/*******************************************************************************

 $Function:    	call_offer_ccbs_recall_callback

 $Description:	Handle the Callback Registration Query callback

 $Returns:		int: MFW_EVENT_CONSUMED	or MFW_EVENT_REJECTED

 $Arguments:    T_MFW_HND win       - pointer to the parent window
                USHORT identifier   - Identifier supplied in T_DISPLAY_DATA
                UBYTE reason        - The reason for the call

 $Note:         The current Callback Registration Query window is about to be destroyed

*******************************************************************************/

static int call_offer_ccbs_recall_callback(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    int     res = MFW_EVENT_CONSUMED;   // Result code to be returned by this function
    T_MFW   mfw_res;    // Result code returned by MFW

    TRACE_FUNCTION("call_offer_ccbs_recall_callback()");
    TRACE_EVENT_P3("win %p: identifier %d: reason %d", win, identifier, reason);

    switch(reason)
    {
        case INFO_KCD_RIGHT:    // Reject offer of CCBS recall
            TRACE_EVENT("INFO_KCD_RIGHT");
            if ((mfw_res = cm_ccbs_reject_recall()) != CM_OK)
            {
                TRACE_EVENT_P1("cm_ccbs_reject_recall returned %d", mfw_res);
                bookShowInformation(ccbs_recall.win, TxtFailed, NULL, NULL);
            }
            break;

        case INFO_KCD_LEFT: // Accept the offer of CCBS recall
        case INFO_KCD_CALL:
            TRACE_EVENT("INFO_KCD_LEFT");
            if ((mfw_res = cm_ccbs_accept_recall(
                ccbs_recall.number, ccbs_recall.type)) != CM_OK)
            {
                TRACE_EVENT_P1("cm_ccbs_accept_recall returned %d", mfw_res);
                if (mfw_res == CM_BLACKLIST)
                {
                    bookShowInformation(ccbs_recall.win, Txtblacklisted, NULL, NULL);
                }
                else
                {
                    bookShowInformation(ccbs_recall.win, TxtFailed, NULL, NULL);
                }
            }
            break;

        case INFO_KCD_HUP:  // Ignore the offer of CCBS recall
            TRACE_EVENT("INFO_KCD_HUP");
            // No further action required
            break;

        default:
            TRACE_EVENT_P1("Unexpected reason: %d", reason);
            res = MFW_EVENT_REJECTED;
            break;
    }

    /*
     * The current Callback Registration Query window is about to be destroyed
     * so the window handle is no longer valid.
     */
    ccbs_recall.win = NULL;
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
    audio_StopSoundbyID(AUDIO_BUZZER,getCurrentRingerSettings());
#endif
#endif
    return res;
}

/*******************************************************************************

 $Function:     call_withdraw_ccbs_recall

 $Description:  Withdraws an opportunity for CCBS registration to the user

 $Returns:      Nothing

 $Arguments:    None

*******************************************************************************/
static void call_withdraw_ccbs_recall(LangTxt TxtId)
{
    size_t size;   /* The length of the string to be displayed */
    ULONG duration; /* The duration of this display - 3s or forever */
    LangTxt lsk;    /* The LSK label - OK if forever else none */
	/* SPR#1428 - SH - New editor data */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;
#else /* NEW_EDITOR */
    T_EDITOR_DATA editor_data;  // Structure for (read-only) editor
#endif /* NEW_EDITOR */
    int zoneId; // Variable for "zone" of editor

    TRACE_FUNCTION("call_withdraw_ccbs_recall()");

    if (ccbs_recall.win != NULL)
    {
        TRACE_EVENT_P1("Destroying editor %p", ccbs_recall.win);
	/* SPR#1428 - SH - New editor changes */
#ifdef NEW_EDITOR
		AUI_edit_Destroy(ccbs_recall.win);
#else /* NEW_EDITOR */
        editor_destroy(ccbs_recall.win);
#endif /* NEW_EDITOR */
        ccbs_recall.win = NULL;
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
        audio_StopSoundbyID(AUDIO_BUZZER,getCurrentRingerSettings());
#endif
#endif
    }

    size = fill_ccbs_recall_text_buffer(TxtId);

    if (TxtId == TxtTimedOut)
    {
        TRACE_EVENT("Timed Out");
        duration = FOREVER;
        lsk = TxtSoftOK;
        zoneId = ZONE_FULLSOFTKEYS;
    }
    else
    {
        TRACE_EVENT("Not Timed Out");
        duration = THREE_SECS;
        lsk = TxtNull;
        zoneId = ZONE_FULLSCREEN;
    }

   	/* SPR#1428 - SH - New editor changes */
#ifdef NEW_EDITOR
    	AUI_edit_SetDefault(&editor_data);
    	AUI_edit_SetDisplay(&editor_data, zoneId, COLOUR_EDITOR_XX, EDITOR_FONT);
    	AUI_edit_SetTextStr(&editor_data, lsk, TxtNull, TxtNull, 0);
    	AUI_edit_SetEvents(&editor_data, 0, TRUE, duration, NULL);
    	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ccbs_recall_text_buffer, size);
    	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
        AUI_edit_Start(NULL, &editor_data);
#else /* NEW_EDITOR */
    editor_attr_init(&editor_data.editor_attr, zoneId, edtCurNone, 0,
        ccbs_recall_text_buffer, size, COLOUR_EDITOR_XX);
    editor_data_init(&editor_data, NULL,
        lsk, TxtNull, TxtNull, 0, READ_ONLY_MODE, duration);

    editor_start(NULL, &editor_data);
#endif /* NEW_EDITOR */
    TRACE_EVENT_P2("editor_start(%p, %p)", NULL, &editor_data);
}

/*******************************************************************************

 $Function:    	call_ccbs_show_status

 $Description:	Display the CCBS Status Display

 $Returns:		Nothing

 $Arguments:    None

*******************************************************************************/
static void call_ccbs_show_status(void)
{
    T_DISPLAY_DATA  display_info ;  // Structure for info_dialog
    TRACE_FUNCTION("call_ccbs_show_status()");

    /* Marcus: Issue 1630: 24/01/2003: Start */
    if (FFS_flashData.ccbs_status == FALSE)
    {
        /* CCBS is off, so no RSK "List" option */
        TRACE_EVENT("FFS_flashData.ccbs_status == FALSE");
        dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtCallBack, TxtOff, COLOUR_STATUS_NETWORKS);
        dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_ccbs_callback, FOREVER, KEY_HUP | KEY_LEFT );
    }
    else
    {
        /* CCBS is on, so RSK "List" option */
        TRACE_EVENT("FFS_flashData.ccbs_status != FALSE");
        dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtList, TxtCallBack, TxtOn, COLOUR_STATUS_NETWORKS);
        dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)call_ccbs_callback, FOREVER, KEY_HUP | KEY_LEFT | KEY_RIGHT );
    }
    /* Marcus: Issue 1630: 24/01/2003: End */

    TRACE_EVENT_P2("info_dialog(%p, %p)", NULL, &display_info);
    info_dialog(NULL, &display_info);
}

/*******************************************************************************

 $Function:    	call_ccbs_callback

 $Description:	Handle the CCBS Status Display callback

 $Returns:		int: MFW_EVENT_CONSUMED	or MFW_EVENT_REJECTED

 $Arguments:    T_MFW_HND win       - pointer to the parent window
                USHORT identifier   - Identifier supplied in T_DISPLAY_DATA
                UBYTE reason        - The reason for the call

 $Note:         The current CCBS Status Display window is about to be destroyed

*******************************************************************************/

static int call_ccbs_callback(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    int res = MFW_EVENT_CONSUMED;   // Result code to be returned by this function

    TRACE_FUNCTION("call_ccbs_callback()");
    TRACE_EVENT_P3("win %p: identifier %d: reason %d", win, identifier, reason);

    switch(reason)
    {
        case INFO_KCD_RIGHT:    // List CCBS
            TRACE_EVENT("INFO_KCD_RIGHT");
            if (FFS_flashData.ccbs_status == TRUE)
            {
                T_MFW cm_res; // Result returned by cm_ccbs_list

                /* Set up the structure that is to contain the CCBS list */
                call_initialise_ccbs_list(win);

                /* Request the CCBS list from ACI via MFW */
                if ((cm_res = cm_ccbs_list()) != CM_OK)
                {
                    /* Redisplay the CCBS Status Display replacing that being destroyed */
                    call_ccbs_show_status();
                    bookShowInformation(NULL, TxtFailed, NULL, NULL);
                    TRACE_EVENT_P1("cm_ccbs_list returned %d", cm_res);
                }
            }
            else
            {
                TRACE_EVENT("List selected when CCBS off");
            }
            break;

        case INFO_KCD_LEFT: // Cancel back
            TRACE_EVENT("INFO_KCD_LEFT");
            break;

        case INFO_KCD_HUP:  // Cancel back
            TRACE_EVENT("INFO_KCD_HUP");
            break;

        default:
            TRACE_EVENT_P1("Unexpected reason: %d", reason);
            res = MFW_EVENT_REJECTED;
            break;
    }

    return res;
}

/*******************************************************************************

 $Function:    	call_ccbs

 $Description:	Handle the CCBS sub-menu

 $Returns:		int: MFW_EVENT_CONSUMED	or MFW_EVENT_REJECTED

 $Arguments:    MfwMnu *menu       - pointer to current menu
                MfwMnuItem *item   - pointer to current menu item

*******************************************************************************/

int call_ccbs(MfwMnu* menu, MfwMnuItem* item)
{
    int res = MFW_EVENT_CONSUMED;   // Result code to be returned by this function
    effs_t ffs_res; // For result returned by flash_write

    TRACE_FUNCTION("call_ccbs()");

    if ((FFS_flashData.ccbs_status != FALSE) &&
        (FFS_flashData.ccbs_status != TRUE))
    {
        /*
         * CCBS status is not something sensible,
         * so set CCBS status to default
         */
        FFS_flashData.ccbs_status = FALSE;
        if ((ffs_res = flash_write()) != EFFS_OK)
        {
             TRACE_EVENT_P1("flash_write returned %d", ffs_res);
        }
        TRACE_EVENT("CCBS status set to default (off)");
    }

    switch(menu->lCursor[menu->level])
    {
        case 0:
            TRACE_EVENT("Menu option 0: Check Status");
            break;

        case 1:
            TRACE_EVENT("Menu option 1: Activate");
            if (FFS_flashData.ccbs_status != TRUE)
            {
                TRACE_EVENT("FFS_flashData.ccbs_status != TRUE");
                FFS_flashData.ccbs_status = TRUE;
                if ((ffs_res = flash_write()) != EFFS_OK)
                {
                    TRACE_EVENT_P1("flash_write returned %d", ffs_res);
                }
            }
            break;

        case 2:
            TRACE_EVENT("Menu option 2: Deactivate");
            if (FFS_flashData.ccbs_status != FALSE)
            {
                T_MFW cm_res; // Result returned by cm_ccbs_delete_all
                TRACE_EVENT("FFS_flashData.ccbs_status != FALSE");
                FFS_flashData.ccbs_status = FALSE;
                if ((ffs_res = flash_write()) != EFFS_OK)
                {
                    TRACE_EVENT_P1("flash_write returned %d", ffs_res);
                }
                if ((cm_res = cm_ccbs_delete_all()) != CM_OK)
                {
                    TRACE_EVENT_P1("cm_ccbs_delete_all returned %d", cm_res);
                    bookShowInformation(NULL, TxtFailed, NULL, NULL);
                }
                else
                {
                    TRACE_EVENT("cm_ccbs_delete_all successful");
                }
            }
            break;

        default:
            TRACE_EVENT_P1("Unexpected menu->lCursor[menu->level]: %d",
                menu->lCursor[menu->level]);
            res = MFW_EVENT_REJECTED;
            break;
    }

    if (res == MFW_EVENT_CONSUMED)
    {
        /* Valid, so display the CCBS Status Display */
        call_ccbs_show_status();
    }
    TRACE_EVENT_P1("call_ccbs returning %d", res);
    return res;
}
/* Marcus: CCBS: 13/11/2002: End */

/*******************************************************************************

 $Function:  call_deflection_on

 $Description: Handles user selection of the "on" option in the Call Deflection menu

 $Returns:  status int

 $Arguments:  not used
 Function added for SPR 1392
*******************************************************************************/
int call_deflection_on(MfwMnu* m, MfwMnuItem* i)
{T_MFW_HND win =  mfwParent( mfw_header() );
  T_DISPLAY_DATA display_info;


  FFS_flashData.call_deflection = TRUE;
    flash_write();

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallDeflection, TxtActivated, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
    /* Call Info Screen
    */
    info_dialog( win, &display_info );

    return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:  call_deflection_off

 $Description: Handles user selection of the "off" option in the Call deflection menu

 $Returns:  status int

 $Arguments:  not used
  Function added for SPR 1392
*******************************************************************************/
int call_deflection_off(MfwMnu* m, MfwMnuItem* i)
{T_MFW_HND win =  mfwParent( mfw_header() );
  T_DISPLAY_DATA display_info;

  FFS_flashData.call_deflection = FALSE;
    flash_write();

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallDeflection, TxtDeActivated, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );

    /* Call Info Screen
    */
    info_dialog( win, &display_info );

    return MFW_EVENT_CONSUMED;

}





/*******************************************************************************

 $Function:     cm_mt_deflect
 $Description:  Handles events from call deflection editor
 $Returns:    nothing
 $Arguments:  win, identifier(not used), reason(which key was pressed)
*******************************************************************************/
void cm_mt_deflect(T_MFW_HND win, USHORT identifier, SHORT reason)
{
	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  	T_call * data     = (T_call *)win_data->user;
	T_DISPLAY_DATA display_info;
	BOOL result;

    TRACE_FUNCTION("cm_mt_deflect");
    TRACE_EVENT_P1("cm_mt_deflect reason=%d",reason);



	switch (reason)
	{
	case INFO_KCD_LEFT:

		TRACE_EVENT("cm_mt_deflect-left");
		/*destroy the editor, we don't need it anymore*/
		/* SPR#1428 - SH - New editor */
	#ifdef NEW_EDITOR
		AUI_edit_Destroy(call_data.win_deflectEdit);
	#else /* NEW_EDITOR */
		editor_destroy(call_data.win_deflectEdit);
	#endif /* NEW_EDITOR */

		call_data.win_deflectEdit = 0;
		/*deflect to number in editor*/
		result = cm_deflect_incoming_call(data->editor_buffer);

		if (result == TRUE)
		{
			// Jan 05, 2006    REF:DR OMAPS00050454  x0039928
			// Fix: Destroy the calling window
			 if (!call_data.calls.numCalls)
  			{       if (call_data.win)
            			{
			                call_destroy(call_data.win);
             			}
  			}

			/*Show that we're attempting to deflect the  call*/
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtDeflectingCallTo, TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
			display_info.TextString2   =data->editor_buffer;
			call_data.win_deflecting = info_dialog(idle_get_window(), &display_info);
		}
		else
		{	/*Show that deflection of call failed*/
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtDeflect, TxtFailed, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
			info_dialog(idle_get_window(), &display_info);

			/*delete all deflection windows if deflection has failed*/
			deflect_destroy(TRUE);
		}
		break;
case INFO_KCD_ALTERNATELEFT:
		/*start the phonebook serach window to select a number*/
		deflPhbExec();

		break;
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:	// (Exit from Editor)
		TRACE_EVENT("cm_mt_deflect-right");
		/*destroy the editor*/
		/* SPR#1428 - SH - New editor */
	#ifdef NEW_EDITOR
		AUI_edit_Destroy(call_data.win_deflectEdit);
	#else /* NEW_EDITOR */
		editor_destroy(call_data.win_deflectEdit);
	#endif /* NEW_EDITOR */
		call_data.win_deflectEdit = 0;
		/*resurrect the incoming call screen*/
		if (call_data.win_incoming == 0)
		{	call_data.win_incoming=info_dialog(call_data.win, &call_data.inc_call_data);
		}
		else
		{ winShow(call_data.win_incoming);
		}

		break;

	default:

		break;

	}

	return;
}



/*******************************************************************************

 $Function:     call_deflection_cb
 $Description:  Handles keyboard input from incoming call screen
 $Returns:    nothing
 $Arguments:  win, identifier(not used), reason(which key was pressed)
*******************************************************************************/
void call_deflection_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_call * data     = (T_call *)win_data->user;

/* a0393213 (WR - "currentRinger" was set but never used) */
#ifndef FF_MIDI_RINGER 
#ifndef FF_MMI_RINGTONE
  UBYTE currentRinger;
#endif
#endif

#ifdef NEW_EDITOR
  T_AUI_EDITOR_DATA editor_data;
#else /* NEW_EDITOR */
  T_EDITOR_DATA		editor_data;
#endif /* NEW_EDITOR */

  TRACE_FUNCTION("call_deflection_cb");
  TRACE_EVENT("call_deflection_cb");

  TRACE_EVENT_P1("number %s",call_data.globalCWInfo.number);

#ifndef WIN32
// EF 2nd call causes windows crash
/*a0393213 lint warnings:structure passed instead of char* */
#ifdef NO_ASCIIZ
  TRACE_EVENT_P1("name %s",call_data.entry.name.data);
#else
  TRACE_EVENT_P1("name %s",call_data.entry.name);
#endif
#endif

  /*special case: Identifier is used only for the Multicall Control */
    call_data.win_incoming = 0;
    // Jan 05, 2006    REF:DR OMAPS00050454  x0039928
    // Fix: Reset the deflecting window.
    call_data.win_deflecting = 0;

  /* a0393213 (WR - "currentRinger" was set but never used) */
#ifndef FF_MIDI_RINGER 
#ifndef FF_MMI_RINGTONE
  currentRinger = getCurrentRingerSettings();
#endif
#endif

  switch (reason)
  {
    //reject
    case INFO_KCD_HUP:
    case INFO_KCD_RIGHT:
		 TRACE_EVENT("reject deflection");
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// terminate the ringing
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
        audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif
      	cm_mt_reject();
    	break;
	//deflect
	case INFO_KCD_LEFT:
		TRACE_EVENT("deflect call");
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
               audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
	        audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif
		/* SPR#1428 - SH - New editor */
	#ifdef NEW_EDITOR
		AUI_edit_SetDefault(&editor_data);
		AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
		editor_data.editor_attr.win_size.py += Mmi_layout_line_height() ;
		editor_data.editor_attr.win_size.sy -= Mmi_layout_line_height() ;
		AUI_edit_SetTextStr(&editor_data, TxtDeflect, TxtSoftBack, TxtDeflectCallTo, NULL);
		AUI_edit_SetEvents(&editor_data, identifier, FALSE, FOREVER, (T_AUI_EDIT_CB)cm_mt_deflect);
		AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)data->editor_buffer, 20);
		AUI_edit_SetAltTextStr(&editor_data, 1, TxtSoftContacts, FALSE, TxtNull);
		AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_BAR);
		/* create the editor handler */
		call_data.win_deflectEdit =  AUI_edit_Start(win, &editor_data);  /* start the editor */
	#else /* NEW_EDITOR */
		editor_attr_init(&editor_data.editor_attr, ZONE_FULL_SK_TITLE, edtCurBar1, 0, data->editor_buffer, 20, COLOUR_EDITOR_XX );
		editor_data.editor_attr.win.py += Mmi_layout_line_height() ;
		editor_data.editor_attr.win.sy -= Mmi_layout_line_height() ;
		editor_data.AlternateLeftSoftKey = TxtSoftContacts;/*get phonebook list*/
		editor_data.LeftSoftKey 		= TxtDeflect; /*deflect to entered number*/
		editor_data.RightSoftKey		= TxtSoftBack;
		editor_data.TextId				= TxtDeflectCallTo;
		editor_data.Identifier			= identifier;
		editor_data.min_enter		    = 1;
		editor_data.hide			    = FALSE;
		editor_data.timeout				=FOREVER;
		editor_data.Callback     		= (T_EDIT_CB)cm_mt_deflect;
		editor_data.destroyEditor		= FALSE;
		editor_data.mode = DIGITS_MODE;
		/* create the editor handler */
		call_data.win_deflectEdit =  editor_start(win, &editor_data);  /* start the editor */
#endif /* NEW_EDITOR */

		break;

    //Accept call
   	case INFO_KCD_OFFHOOK:

     	TRACE_EVENT("accept call -deflection");
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
      	audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif

      	cm_mt_accept();
        break;
//    break;    // RAVI

    //this is for the auto answer
        case INFO_TIMEOUT:
// Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// Stopping the current ring tone.
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
              audio_StopSoundbyID( AUDIO_BUZZER,  CALLTONE_SELECT);
#else
        	audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif

      		cm_mt_accept();
        break;

    default:

    break;
  }

}




/*******************************************************************************

 $Function:     deflPhbExec

 $Description: Starts phonebook serach screen for call deflection

 $Returns: status int

 $Arguments:none

*******************************************************************************/



static int deflPhbExec()
{

 //   T_MFW_HND       win  = mfwParent(mfw_header());                // RAVI
 //   T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;       // RAVI 
//    T_call_menu * data = (T_call_menu *)win_data->user;            // RAVI

    call_data.win_deflectSearch = bookPhonebookStart(call_data.win,PhbkFromDeflect);
  	return 1;
}


/* SPR#1352 - SH */
#ifdef MMI_TTY_ENABLED


/*******************************************************************************

 $Function:		call_tty_init

 $Description:	Initialise TTY

 $Returns		None

 $Arguments:	None

*******************************************************************************/

void call_tty_init(void)
{
	/* SPR#1985 - SH - By default, don't display update dialogs */

	TTYDisplayDialog = FALSE;

	if (FFS_flashData.ttyAlwaysOn==TRUE)
	{
		cm_tty_enable(TRUE);
	}
	else
	{
		FFS_flashData.ttyAlwaysOn = FALSE;	/* Ensures it's either TRUE or FALSE */
		call_tty_set(CALL_TTY_OFF);
	}

	return;
}

/*******************************************************************************

 $Function:		call_tty_get

 $Description:	Obtain status of TTY

 $Returns		Status byte

 $Arguments:	None

*******************************************************************************/

UBYTE call_tty_get(void)
{
	return call_data.tty;
}

/*******************************************************************************

 $Function:		call_tty_set

 $Description:	Change status of TTY in RAM.  Called when an appropriate response
 				is received from ACI.

 $Returns		None

 $Arguments:	Status byte, one of: CALL_TTY_OFF, CALL_TTY_ALWAYSON,
 				CALL_TTY_ONNEXTCALL

*******************************************************************************/

void call_tty_set(UBYTE tty)
{
	call_data.tty = tty;
#ifdef FF_TTY_HCO_VCO	
	call_data_tty = call_data.tty;
#endif

	if (tty)
		iconsSetState(iconIdTTY);
	else
		iconsDeleteState(iconIdTTY);

	return;
}


/*******************************************************************************

 $Function:		call_tty_menu

 $Description:	Initialise menu for TTY

 $Returns		None

 $Arguments:	None

*******************************************************************************/

int call_tty_menu(MfwMnu* menu, MfwMnuItem* item)
{
	T_MFW_HND	win  = mfwParent(mfw_header());
	T_MFW_HND	menuWin;
	UBYTE selection;

	/* SPR#1985 - SH - Query TTY status, to update menu */
	TTYDisplayDialog = FALSE;
	cm_tty_query();

	selection = call_data.tty;
	menuWin = bookMenuStart(win, TTYAttributes(), 0);
	SEND_EVENT(menuWin, DEFAULT_OPTION, NULL, &selection);  /* Set the default option to the value of tty */

	return 1;
}
/*x0039928 OMAPS00097714 HCO/VCO option - menu handlers added*/
#ifdef FF_TTY_HCO_VCO
/*******************************************************************************

 $Function:		call_tty_co_menuselect_nextcall

 $Description:	when an item is selected in TTY --> On next call menu

 $Returns		None

 $Arguments:	None

*******************************************************************************/
int call_tty_co_menuselect_nextcall(MfwMnu* menu, MfwMnuItem* item)
{
	UBYTE tty_co;

	tty_co = menu->lCursor[menu->level];

	ttyPfType = tty_co;

	if (FFS_flashData.ttyAlwaysOn!=FALSE)
	{
		FFS_flashData.ttyAlwaysOn = FALSE;
		flash_write();
	}

       TTYDisplayDialog = TRUE;
	cm_tty_enable(TRUE);
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:		call_tty_co_menuselect_always

 $Description:	when an item is selected in TTY --> Always menu

 $Returns		None

 $Arguments:	None

*******************************************************************************/
int call_tty_co_menuselect_always(MfwMnu* menu, MfwMnuItem* item)
{
	UBYTE tty_co;

	tty_co = menu->lCursor[menu->level];

	if (tty_co == TTY_ONLY_PF)
	{
		if (FFS_flashData.ttyAlwaysOn!= TRUE)
		{
			FFS_flashData.ttyAlwaysOn = TRUE;
			flash_write();
		}

			FFS_flashData.ttyPfType = TTY_ONLY_PF;
			flash_write();
	}
	else if(tty_co == TTY_VCO_PF)
	{
		if (FFS_flashData.ttyAlwaysOn!=TRUE)
		{
			FFS_flashData.ttyAlwaysOn = TRUE;
			flash_write();
		}
			
			FFS_flashData.ttyPfType = TTY_VCO_PF;
			flash_write();
	}
	else if(tty_co == TTY_HCO_PF)
	{
		if (FFS_flashData.ttyAlwaysOn!=TRUE)
		{
			FFS_flashData.ttyAlwaysOn = TRUE;
			flash_write();
		}
			
			FFS_flashData.ttyPfType = TTY_HCO_PF;
			flash_write();
	}

       TTYDisplayDialog = TRUE;
	cm_tty_enable(TRUE);
	
	return 1;
}
#endif
/*******************************************************************************

 $Function:		call_tty_menuselect

 $Description:	Select menu option for TTY.  Switches "always on" on or off in the
 				flash, then requests the ACI appropriately.

 $Returns		None

 $Arguments:	None

*******************************************************************************/


int call_tty_menuselect(MfwMnu* menu, MfwMnuItem* item)
{
	T_MFW_HND	win  = mfwParent(mfw_header());
	T_DISPLAY_DATA display_info;
	UBYTE tty;

	tty = menu->lCursor[menu->level];
	/* SPR#1985 - SH - Remove duplicate call */

// Aug 03, 2006 REF:OMAPS00088329  x0039928
// Fix: If headset is already connected then tty is not enabled.
	if(tty != CALL_TTY_OFF)
	{
#ifdef FF_MMI_AUDIO_PROFILE		
		if(headsetInserted == TRUE)
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNotAllowed, NULL, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT );
			info_dialog(idle_get_window(), &display_info);
			
			return 1;
		}
#endif		
	}

	/* Check if "Always on" status has changed */

	if (tty==CALL_TTY_ALWAYSON)
	{
#ifdef FF_TTY_HCO_VCO /*x0039928 OMAPS00097714 HCO/VCO option - Added to provide HCO/VCO option*/
		bookMenuStart(win, TTYCOAlwaysAttributes(), 0);
#else
		if (FFS_flashData.ttyAlwaysOn!=TRUE)
		{
			FFS_flashData.ttyAlwaysOn = TRUE;
			flash_write();
		}
#endif
	}
	else
	{

#ifdef FF_TTY_HCO_VCO /*x0039928 OMAPS00097714 HCO/VCO option - Added to provide HCO/VCO option*/
		if (tty==CALL_TTY_ONNEXTCALL)
			bookMenuStart(win, TTYCONextCallAttributes(), 0);
		else
#endif		
		if (FFS_flashData.ttyAlwaysOn!=FALSE)
		{
			FFS_flashData.ttyAlwaysOn = FALSE;
			flash_write();
		}
	}

	/* Send request to MFW->ACI
	 * SPR#1985 - SH - Don't show status dialog */


	TTYDisplayDialog = TRUE;

#ifdef FF_TTY_HCO_VCO	
	if (!tty)
		cm_tty_enable(FALSE);
#else
	if (tty)
		cm_tty_enable(TRUE);
	else
		cm_tty_enable(FALSE);
#endif	

	return 1;
}

#ifdef NEPTUNE_BOARD /* Mar 27, 2006    REF:ER OMAPS00071798 */

/*******************************************************************************

 $Function:		ctty_audio_mode_text

 $Description:   sets the audio mode then requests the ACI appropriately.

 $Returns		None

 $Arguments:	None

*******************************************************************************/
int ctty_audio_mode_text(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win =  mfwParent( mfw_header() );
    T_DISPLAY_DATA display_info;

    FFS_flashData.tty_audio_mode = AUDIO_MODE_TEXT;
    flash_write();

    dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtAudioModeText,  TxtActivated, COLOUR_STATUS);
    dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT);
    
    info_dialog( win, &display_info );

}


/*******************************************************************************

 $Function:		ctty_audio_mode_text

 $Description:   sets the audio mode then requests the ACI appropriately.

 $Returns		None

 $Arguments:	None

*******************************************************************************/

int ctty_audio_mode_vco(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win =  mfwParent( mfw_header() );
    T_DISPLAY_DATA display_info;

    FFS_flashData.tty_audio_mode = AUDIO_MODE_VCO;
    flash_write();

    dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtAudioModeVco,  TxtActivated, COLOUR_STATUS);
    dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT);
    
    info_dialog( win, &display_info );
}


/*******************************************************************************

 $Function:		ctty_audio_mode_text

 $Description:   sets the audio mode then requests the ACI appropriately.

 $Returns		None

 $Arguments:	None

*******************************************************************************/

int ctty_audio_mode_unknown(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win =  mfwParent( mfw_header() );
    T_DISPLAY_DATA display_info;

    FFS_flashData.tty_audio_mode = AUDIO_MODE_Unknown;
    flash_write();

    dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtAudioModeUnknown,  TxtActivated, COLOUR_STATUS);
    dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT);
        
    info_dialog( win, &display_info );

}

#endif

/*******************************************************************************

 $Function:		call_tty_statuswin

 $Description:	If TTY status changes, inform the user with a window

 $Returns		None

 $Arguments:	None

*******************************************************************************/

void call_tty_statuswin(void)
{
	T_MFW_HND	win  = mfwParent(mfw_header());
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("call_tty_statuswin()");

	/* SPR#1985 - SH - Now checks to see if dialog should be displayed */

	if (TTYDisplayDialog)
	{
//	May 28, 2007 DR: OMAPS00123948 x0066692
//Added Specific Dailogs when TTy_VCO, TTY_HCO are selected
		if (call_data.tty == CALL_TTY_ALWAYSON)
		{
			if(flash_read() >= EFFS_OK)
			  {
				if( FFS_flashData.ttyPfType == TTY_ONLY_PF) 
					dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTY,  TxtActivated, COLOUR_STATUS);
				else if(FFS_flashData.ttyPfType == TTY_VCO_PF)
					dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTYVco,  TxtActivated, COLOUR_STATUS);
				
				else if(FFS_flashData.ttyPfType == TTY_HCO_PF )
					dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTYHco,  TxtActivated, COLOUR_STATUS);
			}
		}
		else if( call_data.tty == CALL_TTY_ONNEXTCALL )
		{
			if( ttyPfType == TTY_ONLY_PF )
				dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTY,  TxtActivated, COLOUR_STATUS);
			else if (ttyPfType == TTY_VCO_PF )
				dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTYVco,  TxtActivated, COLOUR_STATUS);
			else if(ttyPfType == TTY_HCO_PF )
				dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTYHco,  TxtActivated, COLOUR_STATUS);
			
		}
		else
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtTTY,  TxtDeActivated, COLOUR_STATUS);
		}

		dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, NULL );
		info_dialog (win, &display_info);
		TTYDisplayDialog = FALSE;
	}

	return;
}

/*******************************************************************************

 $Function:		call_tty_revert

 $Description:	Called when call terminates, to revert TTY settings back to their original
 				values (when appropriate)

 $Returns		None

 $Arguments:	None

*******************************************************************************/

void call_tty_revert(void)
{

	/* If "on next call" status is true, and stored status isn't "always on",
	 * then TTY has been temporarily switched on from the "off" state,
	 * and must be switched off again. */

	if (call_tty_get()==CALL_TTY_ONNEXTCALL)
	{
		if (FFS_flashData.ttyAlwaysOn)	/* TTY must stay on */
			cm_tty_enable(TRUE);
		else
			cm_tty_enable(FALSE);
	}

	/* If we're in "off" status but the stored status is "always on", then
	 * TTY has been temporarily switched off - switch it on again */

	else if (call_tty_get()==CALL_TTY_OFF && FFS_flashData.ttyAlwaysOn)
	{
		cm_tty_enable(TRUE);
	}

	return;
}

#endif

static void call_ending_all_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	/*
	** Send an Idle Update to trigger the idle screen to be updated if required,
	** it will also trigger the Pin Entry screen to be reactivated if an emergency call
	** was made before the Pin/Puk was entered.
	*/
	idleEvent(IdleUpdate);

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
       iIncomingCall = 0;    
#endif
#endif

	return;
}


void call_show_ending_all()
{
        T_DISPLAY_DATA   display_info;

        TRACE_EVENT("call_show_ending_all()");

		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCallEnded, TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info,(T_VOID_FUNC)call_ending_all_cb, THREE_SECS, KEY_CLEAR );
          /*
           * Call Info Screen
           */
        info_dialog (idle_get_window(), &display_info);
}

/*******************************************************************************

 $Function:     item_flag_hold



 $Arguments:  m, ma, mi - not used

*******************************************************************************/

USHORT item_flag_hold( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
    switch (call_data.calls.mode)
    {
        case CallSingle:
        case CallConference:
        return 0;
        default:
        return 1;
    }

}

// Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 

/*******************************************************************************
 
 $Function:     item_flag_endheld
 $Description:	 This function decides if the 'End held' menu item needs to be displayed or not. 
 			 The return value 0 indicates to display and 1 to hide 

 $Arguments:  m, ma, mi - not used

 

*******************************************************************************/

USHORT item_flag_endheld( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
    SHORT id;
    if (cm_search_callId(CAL_STAT_Held, &id) == CM_OK)
    {
        //some call is in hold state, so display 'End held' menu option
        return 0;
    }
	else
	{
	    //there is no call in hold state, so don't display 'End held' menu option
	    return 1;
	}
}


/*******************************************************************************

 $Function:     item_flag_swap



 $Arguments:  m, ma, mi - not used

*******************************************************************************/

USHORT item_flag_swap( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
    switch (call_data.calls.mode)
    {
        case CallTwo:
        case CallSingleMultiparty:
        case CallMultipartySingle:
        return 0;
        default:
        return 1;
    }

}

 /* cq15633 Access status function 23-03-04. MZ   */

/*******************************************************************************

 $Function:	 call_IsAlertScreenActive

 $Description:	 Returns the status of the incoming call window. TRUE- If the incoming call window is active
                      

 $Returns		TRUE/FALSE 

 $Arguments:	None

*******************************************************************************/

BOOL  call_IsAlertScreenActive( void )
{
	if(call_data.win_incoming != NULL)
        	return TRUE;
    
	return FALSE;
}
//	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
/*******************************************************************************

 $Function:	 item_loudspeaker_status

 $Description:	 This function decides if the loudspeaker menu item needs to be displayed or not. 
 			 The return value 0 indicates to display and 1 to hide                  

 $Returns		TRUE/FALSE 

 $Arguments:	None

*******************************************************************************/
USHORT item_loudspeaker_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	int device;
	TRACE_FUNCTION("item_loudspeaker_status");
	device = mfw_get_current_audioDevice( );
	if ( device == MFW_AUD_CARKIT || carkitInserted == TRUE|| device == MFW_AUD_LOUDSPEAKER)
		return TRUE;
	return FALSE;
}
/*******************************************************************************

 $Function:	 item_headset_status

 $Description:	 The menu item "Headset" is displayed when the function returns ''0", the required 
 			 condition is the current device is not headset or and the headset is not inserted

 $Returns		TRUE/FALSE 

 $Arguments:	None

*******************************************************************************/
USHORT item_headset_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	int device;
	TRACE_FUNCTION("item_headset_status");
	device = mfw_get_current_audioDevice( );
	if ( device == MFW_AUD_HEADSET || headsetInserted == FALSE)
		return TRUE;
	return FALSE;
}
/*******************************************************************************

 $Function:	 item_carkit_status

 $Description:	 The menu item "carkit" is displayed when the function returns ''0", the required 
 			 condition is the current device is not handheld or not carkit or the headset is not 
 			 inserted
 $Returns		TRUE/FALSE 

 $Arguments:	None

*******************************************************************************/
USHORT item_carkit_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	int device;
	TRACE_FUNCTION("item_carkit_status");
	device = mfw_get_current_audioDevice( );
	if ( device == MFW_AUD_CARKIT || carkitInserted == FALSE)
		return TRUE;
	return FALSE;
}
/*******************************************************************************

 $Function:	 item_handheld_status

 $Description:	 The menu item "Handheld" is displayed when the function returns ''0"
 
 $Returns		TRUE/FALSE 

 $Arguments:	None

*******************************************************************************/
USHORT item_handheld_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	int device;
	TRACE_FUNCTION("item_handheld_status");
	device = mfw_get_current_audioDevice( );
	if ( device == MFW_AUD_CARKIT || headsetInserted == TRUE || device == MFW_AUD_HANDHELD)
		return TRUE;
	return FALSE;
}
/*******************************************************************************

 $Function:	M_exeHandheld

 $Description:	Set the audio device to handheld
 
 $Returns		none

 $Arguments: m and i are not used

*******************************************************************************/
int M_exeHandheld (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());


	TRACE_FUNCTION("M_exeHandheld");
	mfw_audio_set_device(MFW_AUD_HANDHELD);
    call_menu_destroy(win);
    call_data.win_menu = 0;
	return 1; /*a0393213 warnings removal-added return statement*/
}
/*******************************************************************************

 $Function:	 M_exeHeadset

 $Description:	 Set the audio device to Headset
 
 $Returns		none

 $Arguments: m and i are not used

*******************************************************************************/
int M_exeHeadset (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());


	TRACE_FUNCTION("M_exeHeadset");
    mfw_audio_set_device(MFW_AUD_HEADSET);
    call_menu_destroy(win);
    call_data.win_menu = 0;
	return 1; /*a0393213 warnings removal-added return statement*/
}
/*******************************************************************************

 $Function:	 M_exeLoudspeaker

 $Description:	 Set the audio device to Loudspeaker
 
 $Returns		none

 $Arguments: m and i are not used

*******************************************************************************/
int M_exeLoudspeaker (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());


	TRACE_FUNCTION("M_exeLoudspeaker");
	mfw_audio_set_device(MFW_AUD_LOUDSPEAKER);
    call_menu_destroy(win);
    call_data.win_menu = 0;
	return 1; /*a0393213 warnings removal-added return statement*/
}
/*******************************************************************************

 $Function:	 M_exeCarkit

 $Description:	 Set the audio device to Carkit
 
 $Returns		none

 $Arguments: m and i are not used

*******************************************************************************/
int M_exeCarkit (MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());


	TRACE_FUNCTION("M_exeCarkit");
	mfw_audio_set_device(MFW_AUD_CARKIT);
    call_menu_destroy(win);
    call_data.win_menu = 0;
	return 1; /*a0393213 warnings removal-added return statement*/	
}
/*******************************************************************************

 $Function:	 mmi_hook_handle

 $Description:	 Performs the action of the right softkey in connected call screen
 
 $Returns		none

 $Arguments:

*******************************************************************************/
void mmi_hook_handle()
{

	MfwKbd kbdvar;

	TRACE_FUNCTION("mmi_hook_handle");
	kbdvar.code = KCD_RIGHT;
	call_kbd_cb(0,&kbdvar);
}
#endif

/*******************************************************************************

 $Function:	 get_call_data_tty

 $Description:	 Gives the tty status 
 
 $Returns		tty status
 
 $Arguments: None

*******************************************************************************/
#ifdef FF_TTY_HCO_VCO
UBYTE get_call_data_tty(void)
{
	return call_data_tty;
}
#endif

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

/*******************************************************************************

 $Function:	 bmi_incomingcall_set_ringer_status

 $Description:	 Sets the Ringer Status during Incoming Call
 
 $Returns		 None
 
 $Arguments:   Ringer Status Value (Input)

*******************************************************************************/
void bmi_incomingcall_set_ringer_status(E_IN_CALL_RINGER_STATUS eStatus)
{
	if (  ( eStatus < BMI_INCOMING_CALL_NONE ) ||
		  ( eStatus > BMI_INCOMING_CALL_RINGERSTOPPED ) )
	{
		return;
	}
	
	iIncomingCallRingerStop = eStatus;
}

/*******************************************************************************

 $Function:	 bmi_incomingcall_get_ringer_status

 $Description:	 Gives the Ringer Status during Incoming Call
 
 $Returns		 None
 
 $Arguments:   Ringer Status Value (Output)

*******************************************************************************/
void bmi_incomingcall_get_ringer_status(E_IN_CALL_RINGER_STATUS *peStatus)
{
	if (!peStatus)
	{
		return;
	}

	*peStatus = iIncomingCallRingerStop;
}

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End  */

