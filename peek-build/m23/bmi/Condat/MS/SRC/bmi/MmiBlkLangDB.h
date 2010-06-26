#ifndef _AUTOGEN_RESOURCE_HEADER_H_
#define _AUTOGEN_RESOURCE_HEADER_H_ 1
//
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Resource Manager
 $File:		    MmiBlkLanfDB.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This file contains the basic resource manager information relating
	to the textual entries managed.

    There are two routines provided by this module, one returning the
	number of resource tables supported, the other giving the base address
	of the resource tables.

    This module is derived from an auto generated version of the file with
	additional commenting added.
                        
********************************************************************************
 $History: MmiBlkLangDB.h
	
 	Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat)
 	Description: MP3 - AAC cannot be disabled in Locosto
 	Solution: MP3 AAC Flags Usage have been corrected

 	July 06, 2007 DR: OMAPS00137334 x0062172(Syed Sirajudeen)
 	Description:[MPY]Wrong indication when add party if conference full
 	Solution: Added strings 	TxtConferenceFull to notify that the conference reached the MAX_CALLS 
 
 	
 			
   April 03 2007 ER:OMAPS00116772 x0061088 (Prachi)
   Description:Support of 1.3 Mpixel camera on Locosto Plus
   Solution: Defined New strings TxtAutoSave,TxtAutoSaveOn,TxtAutoSaveOff,TxtSoftSave 
   and TxtSoftCancel. 
    
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation

    Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    Description:SMS alphanumeric address
    Solution: Defined New string TxtTo for SMS alphanumeric support
 
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
 
 	Jan 30, 2007 DR: OMAPS00108892 x0pleela
 	Description: TCMIDITEST.23(Apps) - Play All File feature fails when different Configurations
 				like Mono/Stereo, Loop On/Off are selected
 	Solution: 	Defined new strings TxtLoopOn, TxtCantPlayAllFiles, TxtCantBeActivated to display an
 			error message if we try to enable Loop on while playing all files
 	Feb 27, 2007 ER: OMAPS00113891 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on BAT/ATI level
 	Solution: Added strings TxtFwdIncomingTTYCall( for Fwd. TTY call ) and 
 			TxtFwdIncomingCall (Fwd. Inc. Call)to display for any forwarded incoming call
 			
      Oct 30 2006 ER:OMAPS00091029 x0039928(sumanth)
      Boot time measurement
      
      Oct 30, 2006 ER:OMAPS00098881 Prabakar
      Removing power variant
      
 	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Added strings TxtBlkNetwork( for category Blocked Network) and 
 			TxtEnterPbLock (for editor title "Enter Blocked NW code")

      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      
 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xrashmic 9 Sep 2006, OMAPS00092732
	USBMS ER: Added strings for USB MS

  	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution: Added string TxtNoMEPD to display MEPD not present 	

	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	Added new strings TxtUSSDMsgRxd and TxtFontNotSupported to display if the characters 
			are UCS2 in USSD transactions 

	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security
	Solution:Made the following changes
			Added following new strings:
				TxtBusy			: Displays "Busy" if timer is enabled for wrong entry of password
				TxtMasterUnlock	: For menu option Master unlock
				TxtEnterMasterKey	: Text for the editor to enter master unlock key
				TxtUnblockME		: For menu option Unblock ME


    June 02, 2006    DR: OMAPS00078005 x0021334
    Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
    Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
    function. This function has now been implemented to pass on the received values to MMI
    for user confirmation and RTC updation.

       Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
       Bug:MP3 RWD and FWD function - MOT
       Fix: Forward and Rewind functionalities are handled. New strings
       Forward, Rewind, Forwarding and Rewinding are added.
       
 	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			Defined a new string "TxtEditLineName" for the menu item display

	May 10, 2006    DR: OMAPS00075379 x0021334
	Descripton: SMS  - Status Request in idle mode - using Menu - no status in pending state
	Solution: The display of pending status is network dependent. In some networks this
	status is not given. When available, it used to display 'Temp Error'. This has now been
	replaced with 'Pending'.

    Apr 26, 2006 OMAPS00074886 a0393213 (Prabakar R)
    Description : No option in MMI to release the held call. 
    Solution    : No option was available. Added a menu option.
    
	Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
   	Description: Need to reduce flash foot-print for Locosto Lite 
   	Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
   	out voice memo feature if the above flag is enabled.

	Feb 07, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Renamed "TxtPendReport" to "TxtReport"
   	
       Feb 01, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added "TxtPendReport" to display when status report status is pending
   	
 	Mar 14, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new strings for menu items and dialog displays for PCM voice memo and voice buffering 
	
	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new strings which are used in menu items to support PCM Voice memo feature
	
	Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh
 	Description:In file viewer, the user is not allowed to retain the old name once he/she goes to the rename file option.
 	Solution:While renaming the file,if the name already exists,  display a proper error message
 	

	Dec 07, 2005    REF: OMAPS00050087 x0039928
	Description: Deactivated is displayed while activate Hide Id option is selected
	Solution: String "Not Activated" is added.
	
	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!

 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
	Description: Implementation of Test Menu for AAC
	Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
 	Aug 31, 2005    REF: SPR 34050 xdeepadh
	Description: The application hangs if while renaming a saved file a special character such as '?', '!' etc is used
	Solution: String for error message have been added

	Jul 18, 2005    REF: SPR 31695   xdeepadh
	Bug:Problems with MP3 test application
	Fix:The window handling of MP3 Test Application has been done properly.



	 Aug 22, 2005    REF: ENH 31154 xdeepadh
   	Description: Application to Test Camera
   	Solution: Implemeted the Fileviewer to view the jpeg images.Camera Application to 
   	preview,capture and save image has been implemented.
   	Strings for Fileviewer and Camera  have been added.

 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 	Description:	Support for various audio profiles
 	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 			The audio device is enabled as per the user actions.

	July 19, 2005 REF: CRR LOCOSTO-ENH-28173 xpradipg
  	Description: To provide MMI Support to enable/disable/change password and 
  				query all the Personalization locks
  	Solution: Integration of the changes for the same provided by the soldel 
  			  team

   Jun 13, 2005	REF: SPR 31705 x0021308
   Description: After the maximum limit of the My Number is exceeded, display info : Phonebook full 
   expected is MyNumber Full
   Solution: Added Strings "MyNumber Full" string in Enum IndexTag.

   Jun 13, 2005	REF: SPR 31710 x0021308
   Description: After the maximum limit of the FDN Number  is exceeded, display info : Phonebook full 
   expected is FDNList Full
   Solution: Added Strings "FDNList Full" string in Enum IndexTag.
    May 11 2005  REF:  MMI-SPR-29887  x0012849
    To Implement the deferred MMS retrieval.
   
   Apr 06, 2005	REF: ENH 30011 xdeepadh
   Description: Replacing the Test Application compilation flags with new flags. 
   Solution:  The existing flags for Camera and MP3 test application have  beeen replaced with the 
   new compilation flags,FF_MMI_TEST_CAMERA and FF_MMI_TEST_MP3 respectively.

   x0018858 24 Mar, 2005 MMI-FIX-11321
   Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
   Fix: Added support for handling indication when message box is full and
   there is a pending message.

   Apr 05, 2005    REF: ENH 29994 xdeepadh
   Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
   Solution: Generic Midi Ringer and Midi Test Application were implemented.
 

   March 2, 2005    REF: CRR 11536 x0018858
   Description: Reply path not supported while sending a reply.
   Solution: Added menu option for the reply path.

    xrashmic 7 Dec, 2004 MMI-SPR-23965
    Nov 29, 2004    REF: CRR 25051 xkundadu
    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
    Fix: Added volume level list linked to up/down keys. 
          User can select the speaker volume among those levels.

        xrashmic 29 Nov, 2004 MMI-SPR-26161
        xrashmic 14 Oct, 2004 MMI-SPR-25291

        xrashmic 26 Aug, 2004 MMI-SPR-23931
       
//  xrashmic 19 Aug, 2004 Bug: 2, 3, 36 and 42


    Jul 23, 2004        REF: CRR 16107  xvilliva
    Bug:  (Call Barring) - MMI response overlayes partially background images.
    Fix:    Modfied enum and array in source file to display a better alert.

//  Jun 25, 2004        REF: CRR 21547  xvilliva  (Added TxtInvalidInput.)
 

//    Jun 10, 2004        	REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
//  	Added  Strings "All Divert", "Divert when not reachable", "Divert if Busy", "Divert when no answer"
	"Divert for Cond. Forward" , "Divert for All calls"	
May 10, 2004    REF: CRR 15753  Deepa M.D 
	Menuitem provided to set the validity period to maximum.		


//  Apr 14, 2004        REF: CRR 12653  xvilliva  (Added "Limited Service".)

	25/10/00			Original Condat(UK) BMI version.	


    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode.

    June 09, 2006 REF:ER OMAPS00081264  x0020906
    Added menu string for 12k and 24k PCM rate.

    Aug 1, 2006 REF:ER OMAPS000xxxxx  x0020906
    Added menu string for enable / disable audio & video sync.

 $End

*******************************************************************************/



/*******************************************************************************
                                                                              
                                Required Include Files
                                                                              
*******************************************************************************/

#include "mfw_lng.h"


/* The tPointerStructure provides the basic entry type for the resource
   tables indicated below.

   The structure provides a simple one-to-one mapping between the identifier
   and it's associated text string.
*/
typedef struct _tPointerStructure_
{
    long int Id;
    char *Text;
} tPointerStructure, *pPointerStructure;

/*OMAPS00070661 (SAT - lang setting, lang notification and local info) R.Prabakar
    language macros moved to mfw_lng.h if  FF_MMI_R99_SAT_LANG is defined*/
#ifndef FF_MMI_R99_SAT_LANG
//#define LANG_CHANGE_TEST

/*MC SPR 1150, moved language macros from MmiResources.h*/
/*There should be language definitions for each number from 1 to NO_OF_LANGUAGES*/
#define ENGLISH_LANGUAGE 1
#define GERMAN_LANGUAGE  2
#ifdef CHINESE_MMI
#define CHINESE_LANGUAGE  2
#else
#define CHINESE_LANGUAGE  3
#endif

#ifdef LANG_CHANGE_TEST /*MC test code*/
#define MARTIAN_LANGUAGE 2
#define CHINESE_LANGUAGE  3
#endif

/*MC, SPR  1150, allows easier addition of languages, just increment macro*/
#ifndef CHINESE_MMI
#define NO_OF_LANGUAGES 2
#else
#define NO_OF_LANGUAGES 2
#endif

#endif


/*******************************************************************************
                                                                              
                                Public Definitions
                                                                              
*******************************************************************************/

typedef enum {
    TxtNull = 0,
#ifdef FF_CPHS
  TxtEditLineName, //x0pleela 23 May, 2006  DR: OMAPS00070657
#endif
//x0pleela 24 Feb, 2006  ER OMAPS00067709
//Added TxtPcmVoiceMemo strings which are used in menu items to support PCM Voice memo feature
//x0pleela 09 Mar, 2006   ER: OMAPS00067709
//Added the following strings used in menu items to support Voice buffering feature
#ifdef FF_PCM_VM_VB
TxtStopFailed,
TxtAlready,
TxtPcmVoiceMemo,
TxtVoiceBuffering,
TxtEditNum,
TxtStartRec,
TxtStopRec,
#endif
#ifdef SIM_PERS 
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
TxtBlkNetwork,  //For new category "Blocked Network" 
TxtEnterPbLock, //Editor string to display "Enter Blocked Network password"
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
TxtBusy,
TxtMasterUnlock,
TxtEnterMasterKey,
TxtUnblockME,
TxtUnblockCode, //x0pleela 18 Apr, 2006  DR: OMAPS00067912
TxtTimerFlag,
TxtETSIFlag,
TxtAirtelFlag,
 TxtEnterOldPsLock,
 TxtEnterPsLockNew,
 TxtEnterPsLockconf,
TxtLockEnabled,
    TxtLockDisabled,
    TxtPersonalization,
    TxtSupplementaryInfo,
    TxtPersonalize,
    TxtUnlockCategory,
    TxtLockCategory,
    TxtResetFC,
    TxtGetFC,
    TxtMaxFC,
    TxtCorporate,
    TxtServiceProvider,
    TxtNetworkSubset,
    TxtLockActivated,
    TxtLockDeactivated,
    TxtEnterNLock,
    TxtEnterNsLock,
    TxtEnterSpLock,
    TxtEnterCLock,
    TxtEnterPsLock,
    
    
     TxtPsLockConfWrong,
    TxtPsLockConfwrong2,
    
    
    TxtPsLockChanged,
    TxtCounterReset,
    TxtPassfailed,
    TxtUnblock, 
    TxtAlreadyLocked,
    TxtAlreadyUnlocked,
    TxtPhoneUnblocked,
    TxtPhoneBlocked,
    TxtGetFailFC,
	TxtMaxFailFC,
	TxtGetSuccFC,
	TxtMaxSucFC,
#endif
#ifdef FF_MMI_CPHS
    TxtViewAll,
    TxtEmergencyNumbers,
#endif    
   TxtReport,	//x0pleela 01 Feb, 2006 DR: OMAPS00059261 
   TxtNotImplemented,
    TxtManu,
    TxtEnterPin1,
    TxtEnterPin2,
    TxtEnterPuk1,
    TxtEnterPuk2,
    TxtInvalidCard, 
    TxtNoCard,
	TxtInvalidIMEI, // Nov 24, 2005, a0876501, DR: OMAPS00045909
    TxtSoftCall,
    TxtSoftOK,
    TxtSoftMenu,
    TxtHelp,
    TxtSoftContacts,
    TxtPlay,
    TxtMessages,
    TxtAlarm,
    TxtSettings,
    TxtExtra, 
    TxtSend, 
    TxtSent,
    TxtNotSent, 
    TxtServiceCentre, 
    TxtPleaseWait,
    TxtSmsEnterNumber, 
    TxtRead, 
    TxtRetrieve,   // x0012849 11 May 205 MMI-SPR-29887
    TxtDeleteAll, 
    TxtSave,
    TxtVoicemail,
    TxtBroadcast, 
    TxtNew, 
    TxtDigits, 
    TxtLowercase, 
    TxtUppercase, 
    TxtApplications, 
#ifndef FF_NO_VOICE_MEMO
    TxtVoiceMemo, 
#endif
    TxtSimNot, 
    TxtReady, 
    TxtSmsListFull,
    TxtNewCbMessage, 
    TxtSaved,
    TxtDelete, 
    TxtReply, 
    TxtStoreNumber,
#ifdef FF_CPHS_REL4      
    TxtMSP,
    TxtMessageStatus,
    TxtMailBox,
    TxtElectronic,
#endif    
    TxtForward,
    TxtActivate, 
    TxtTopics,
    TxtConsult,
    TxtAddNew, 
    TxtContacts, 
    TxtSearchName, 
    TxtMemory, 
    TxtMyNumber, 
    TxtMyNumberFull, //SPR 31705 x0021308: RamG
    TxtRinger,
    TxtVolume,
    TxtKeypadTones,
    TxtNewMessage, 
    TxtNewBroadcast, 
    TxtAutoAnswer, 
    TxtTimeDate, 
    TxtLanguages, 
    TxtVoiceMail, 
    TxtPinCode, 
    TxtAutoredial,
    TxtDTMF, 
    TxtOn,
    TxtOff,
    TxtNetworkSel, 
    TxtSendID,
    TxtChargeTimers,
    TxtCallDivert,
    TxtCallWaiting,
    TxtIfBusy,
    TxtIfNoAnswer,
    TxtIfNotReachable, 
    TxtAllCalls,
    TxtCancelAll,
    TxtAll,
    TxtVoice,
    TxtFax,
    TxtData,
    TxtStatus,
    TxtCancel,
    TxtCalculator,
    TxtEnterName,
    TxtSoftBack,
    TxtSoftSelect,
    TxtSoftOptions,
    TxtEmptyList,
    TxtSendMessage,
    TxtChange,
    TxtEnterNumber,
    TxtPhbkFull,
    TxtWriteError,
    TxtEntryDeleted,
    TxtDetails,
    TxtHold,
    TxtUnhold,
    TxtMute,
    TxtSendDTMF,
    TxtMainMenu,
    TxtSwap,
    TxtEndAll,
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Added a menu option. Added "End held" text.
    TxtEndHeld, 
    TxtPrivateCall,
    TxtStartConference,
    TxtReStartConference,
    TxtEndConference,
    TxtFind,
    TxtServiceNumbers,
    TxtOutofRange,
    TxtDone,
    TxtFailed,
#ifdef FF_MMI_MULTIMEDIA    
    TxtFileNotSupported,
#endif    
    TxtEnterNewPin,
    TxtConfPin,
    TxtChangPin,
    TxtEnd,
    TxtPINFail,
    TxtSOS,
    TxtAutomatic,
    TxtManual,
    TxtFixedNames,
    TxtFactoryReset,
    TxtSearching,
    TxtChangPIN2, 
    TxtOldPIN,
    TxtOldPIN2, 
    TxtNotAcc, 
    TxtEnabled,
    TxtDisabled, 
    TxtPINOK,
    TxtSimBlocked,
    TxtNewPIN,
    TxtAcc,
    TxtNoNetwork,
    TxtNo,
    TxtEnterDate,
    TxtEnterTime,
    TxtScratchPad,
    TxtDeleted,
    TxtActivated, 
    TxtDeActivated,
    TxtDeactivate,
    TxtNotActivated, // Dec 07, 2005    REF: OMAPS00050087 x0039928 - Added string to lang table
    TxtCheck,
    TxtYes, 
    TxtCallNumber,
    TxtEnterMid,
    TxtReadError,
    TxtUsed,
    TxtOutgoing,
    TxtMuted,
    TxtCallEnded,
    TxtIncomingCall,
    TxtAnsweredCalls, // API - APR #1629 - Added string to lang table.
	TxtIncomingData,
	TxtIncomingFax, //SPR#1147 - DS - Added string to lang table.
    TxtNumberWithheld,
    TxtAccept,
    TxtReject,
    TxtCalling,
    TxtMissedCalls,
    TxtMissedCall,
    TxtLocked,
    TxtUnmute,
    TxtCharging,
    TxtChargComplete,
    TxtOperationNotAvail,
    TxtAddToConference,
    TxtEmpty,
    TxtSelectFromList,
    TxtEnterNetworkCode,
    TxtMCCMNC,
#ifndef FF_NO_VOICE_MEMO
    TxtRecord,
#endif
    TxtSecurity,
    TxtEnglish,
    TxtGerman,
    TxtValidity,
    TxtMessageTyp,
    TxtSetVoiceMail,
    TxtCallTimers,
    TxtLastCharge,
    TxtTotalCharge,
    TxtResetCharge,
    TxtIfNoService,
    TxtDiscard,
    TxtEDIT,  
    TxtExtractNumber,
    TxtWrite, 
    TxtCheckStatus,
    TxtSetDivert,
    TxtCancelDivert,
    TxtHideID,
    TxtTones,
    TxtClock,
    TxtPhone, 
    TxtStop,
#ifndef FF_NO_VOICE_MEMO
    TxtRecording,
    TxtPlayback,
#endif
    TxtNoNumber,
    TxtOkToDelete,
    TxtBalance,
    TxtLimit, 
    TxtLimitEq,
    TxtRate,
    TxtMinutes,
    TxtCost,
    TxtCredit, 
    TxtPressOk, 
    TxtReset,
    TxtIncoming,
    TxtCounter,
    TxtOutgoingCalls,
    TxtCostCounter,
    TxtEnterCurrency, 
    Txt1Unit,
    TxtChanged,
    TxtEnterLimit,
    TxtCancelled, 
    TxtView,
    TxtEnterPCK,
    TxtEnterCCK,
    TxtEnterNCK,
    TxtEnterNSCK,
    TxtEnterSPCK, 
    TxtCodeInc,
    TxtBlckPerm,
    TxtDealer,
    TxtSendingDTMF,
    TxtCheckNumber,
    TxtNumberBusy,
    TxtNoAnswer, 
    TxtNumberChanged,
    TxtNetworkBusy,
    TxtNotSubscribed, 
    TxtAnyKeyAnswer,
    TxtPINBlock,
    TxtEdit,
    TxtChangePassword,
    TxtSetBar,
    TxtSmsTone,
    TxtMelody,
    TxtSilent,
    TxtClick,
    TxtClearAlarm,
    TxtTimeFormat,	
    TxtTwelveHour,
	TxtTwentyfourHour,
    TxtInCallTimer,
    TxtStore,
    TxtNone,
    TxtPIN,
    TxtPIN2, 
    TxtConfPin2,
    TxtNewPIN2, 
    TxtUnlockOK, 
    TxtIMEI, 
    TxtList, 
    TxtFull, 
    TxtNotAvailable, 
    TxtCallBAOC, 
    TxtCallBAOIC, 
    TxtCallBAOICexH, 
    TxtCallBAIC,
    TxtCallBAICroam, 
    TxtCallBarringAll,
    TxtCallBAOCAll,
    TxtCallBAICAll, 
    TxtCLIP, 
    TxtCOLP, 
    TxtCOLR, 
    TxtSimToolkit, 
    TxtPhonebook, 
    TxtRecentCalls, 
    TxtNames,
    TxtInterCalls,
    TxtInterHome,
    TxtWhenRoaming,
    TxtCancelAllBars,
    TxtWaiting,
    TxtBarred, 
    TxtForwarded, 
    TxtCallDivertNoReach, 
    TxtCallDivertNoReply, 
    TxtCallDivertIfBusy, 
    TxtPressMenu, 
    TxtPlus, 
    TxtMinus,
    TxtDivideBy, 
    TxtMultiplyBy, 
    TxtEquals, 
    TxtSending,
    TxtMessage,
    TxtSoftNext,
    TxtLastCall,
    TxtIncomingCalls,
    TxtLow,
    TxtMedium,
    TxtHigh,
    TxtIncreasing,
    TxtTimerReset, 
    TxtClockSetTo, 
    TxtSilentMode,
    TxtSmsValidityPeriodSet,
    Txt1Hour,
    Txt12Hours,
    Txt24Hours,
    Txt1Week,
    TxtMaxTime,  //  May 10, 2004    REF: CRR 15753  Deepa M.D 
    TxtPin2Code,
    TxtEnablePIN,
    TxtRequesting,
    TxtNotAllowed,
    TxtEmergency,
	TxtChinese,
	TxtPassword,
    TxtConfirmPassword,
    TxtNewPassword,
    TxtNoMessages,
    TxtTwoAttemptsLeft,
    TxtOneAttemptLeft,
    Txtblacklisted,
    TxtErrUnknown,
    TxtSSErr,
    TxtUnknownSub,
    TxtIllSub,
    TxtIllEqip,
    TxtIllOp,
    TxtSubVil,
    TxtInComp,
    TxtFacNoSup,
    TxtAbsSub,
    TxtSysFail,
    TxtMissDta,
    TxtUnexDta,
    TxtPwdErr,
    TxtUnkAlph,
    TxtMaxMpty,
    TxtResNoAvai,
    TxtUnRec,
    TxtMisTyp,
    TxtBadStruct,
    TxtDupInv,
    TxtResLim,
    TxtIniRel,
    TxtFatRes,
    TxtEntAnw,
    TxtSsMsg,
    TxtOld,
    TxtInsertnote, 
   	TxtMelodygenerator,
   	TxtOctaveHigher,
   	TxtOctaveLower,
   	TxtLoadMelody,
   	TxtEasyText,
   	TxtDataCount,
   	TxtQoS,
   	TxtTypeofConnection,
   	TxtTotalCount,
   	TxtResetCount,
   	TxtDataCounter,
   	TxtV42bis,
   	TxtNormal,
   	TxtCSD,
   	TxtClassB,
   	TxtClassC,
   	TxtIPAddress,
/* SPR#2324 - SH - Removed some strings */
   	TxtIPAddress2,
   	TxtPort1,
   	TxtPort2,
   	TxtAPN,
    TxtConnectionType, 
//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
    TxtNameServer1, 
    TxtNameServer2,
    TxtPPGAuthentication,
    TxtWirelessProfiledHTTP,
#endif
    TxtGPRS, 
    TxtOutgoingData,
	TxtGame,
    TxtHomepage,
	TxtBookmarks,
    TxtGoTo,
    TxtHistory,
   	TxtSetup,
   	TxtNewBookmark,
   	TxtWWWdot,
   	TxtWAPdot,
   	TxtSMS,
   	TxtEmail,
   	TxtClear,
   	TxtBookmark,
   	/* 07-11-05 removed text Ids TxtSelectProfile,	TxtSetupProfiles*/
   	TxtSaveHistory,
   	TxtEditName,
   	TxtEditURL,
/* SPR#2324 - SH - Removed some strings */
   	TxtResponseTimer,
   	TxtDialupNumber,
   	TxtTemporary,
   	TxtContinuous,
/* SPR#2324 - SH - Removed some strings */
   	TxtWAP,
   	TxtExit,
   	TxtReload,
   	TxtStored,
	TxtOther,
	TxtConnecting,
	TxtDownloading,
	TxtUpdating,
	TxtEnterURL,
	TxtProfileName,
    TxtISPUsername,
    TxtISPPassword,
    TxtGoToLink,
    TxtSelect,
	TxtNotConnected,
	TxtScaleImages,
	TxtLoggingIn,
	TxtPageNotFound,
	TxtInvalidURL,
	TxtNoResponse,
	TxtAccessDenied,
	TxtErrorInPage,
	TxtScriptError,
	TxtServerError,
	TxtGatewayError,
	TxtUnexpectedErr,
	TxtCharacterSet,
	TxtWBXMLError,
	TxtFatalError,
	TxtGatewayBusy,
	TxtSecurityFail,
	TxtErrorOutOfMem,
	TxtErrorMemLow,
	TxtError,
	TxtErrorHTTP,
	TxtErrorWSP,
	TxtErrorWAE,
	TxtErrorWSPCL,
	TxtErrorWSPCM,
	TxtErrorWTP,
	TxtErrorWTLS,
	TxtErrorWDP,
	TxtErrorUDCP,
	TxtNotSupported,
	TxtSelectLine,
   	TxtLine1,
   	TxtLine2,
   	TxtInfoNumbers,
	TxtAttach,	/*SH 18/01/02*/
	TxtDetach,	/*SH 18/01/02*/
	TxtDetaching, // MZ cq10952 15/07/0
	TxtDefineContext, /*SH 22/01/02*/
	TxtActivateContext, /*SH 22/01/02*/
	TxtDeactivateContext, /*SH 22/01/02*/
	TxtGPRSPreferred,
	TxtCSDPreferred,
	TxtNotAttached,
	TxtAlreadyAttached,
	TxtSelected, /*ap 06/02/02*/
	TxtNetwork, /*ap 06/02/02*/
	TxtForbidden, /*ap 06/02/02*/
	TxtActivateFDN, /*ap 06/02/02*/
	TxtPlease, /*ap 06/02/02*/
	TxtTimedOut, // SH 13/02/02
	TxtWillChangeOn, // SH 13/02/02
	TxtAttached, // SH 14/02/02
	TxtDetached, // SH 14/02/02
	TxtUnknown, // api 13/14/02
	TxtCallTransfer,//MC
	TxtConnected, // SH
    TxtGSM_900,	// SH
    TxtDCS_1800,
	TxtPCS_1900,
    TxtE_GSM,
	TxtGSM_850,
	TxtCallTimeout,
	TxtBand,
	TxtSatRefresh,
	TxtCallBlocked,
	TxtSRR,  // Marcus: Issue 1170: 30/09/2002
        //March 2, 2005    REF: CRR 11536 x0018858
	TxtRP,//added for reply path 
	TxtDelivered,  // Marcus: Issue 1170: 07/10/2002
	TxtTempError,  // Marcus: Issue 1170: 07/10/2002
	TxtUnknownCode,/*MC SPR 1111*/
	TxtPhoneSettings, //SPR#1113 - DS - Added text id for renamed main menu item "Phone Settings"
	TxtNetworkServices, //SPR#1113 - DS - Added text id for new main menu item "Netw. Services"
	TxtCallDeflection, //SPR#1113 - DS - Added text id for new menu item "Call Deflection" (CD)
	TxtCallBack, //SPR#1113 - DS - Added text id for new menu item "Call Back" (CCBS)
	TxtMobileInfo, /*SPR1209*/
  	TxtServingCell,/*SPR1209*/

#ifdef TI_PS_FF_EM 
/*EngMode */
    TxtServingCellRF,
    TxtServingChannel,
    TxtAmrInfo,
    TxtSNDCPInfo,
    TxtUserLevelInfo,
    TxtLLCTracing,
    TxtRlcMacInfo,
    TxtGMMInfo,
    TxtCallStatus,
    TxtLayerMessages,
    TxtWCDMALayerMessages,
    Txt3GInfo,
    TxtPacketDataParameter,
#endif
 	TxtNeighbourCell, /*SPR1209*/
 	TxtLocation, /*SPR1209*/
	TxtCiphHopDTX,/*SPR1209*/
	TxtConcat,
	TxtColourMenu,	
	TxtColourBlack,	
	TxtColourBlue,
	TxtColourCyan,
	TxtColourGreen, 
	TxtColourMagenta, 
	TxtColourYellow,
	TxtColourRed, 
	TxtColourWhite,	
	TxtColourPink,
	TxtColourOrange,
	TxtColourLightBlue,
	TxtColourLightGreen, 
	TxtColourPaleYellow, 
	TxtColourTransparent,
    TxtIdleScreenBackground,
	TxtIdleScreenSoftKey,
	TxtIdleScreenText,
	TxtIdleScreenNetworkName,  
	TxtGeneralBackground ,  		
	TxtMenuSoftKeyForeground , 
	TxtMenuSoftKeyBackground ,  	
	TxtSubMenuHeadingText ,  	
	TxtSubMenuHeadingBackground,  
	TxtSubMenuTextForeground ,  	
	TxtSubMenuTextBackground,  	
	TxtSubMenuSelectionBar,  	
	TxtPopUpTextForeground,  	
	TxtPopUpTextBackground,
	TxtColour,
    TxtLoser,
    TxtPlayAgain,
    TxtWinner,
	TxtSelectBook,	/* SPR1112 - SH */
    TxtSIM,
    TxtMove,
    TxtCopy,
    TxtThisEntry,
    TxtBackground,	/* for selecting backgrounds*/
    TxtProviderIcon,
    TxtIdleScreen,		
	TxtMenuScreen,		
	TxtBgdSquares,
	TxtBgdO2,
	TxtBgdTree,
	TxtBgdTILogo,
	TxtBgdSunset,
	TxtBgdEdinburgh1,
	TxtBgdEdinburgh2,
	TxtBgdEdinburgh3,
	TxtBgdEdinburgh4,			
	TxtDeflectCall, //SPR 1392 call-deflection
	TxtDeflectCallTo, //SPR 1392 call deflection
	TxtDeflect, //SPR 1392 call deflection
	TxtDeflectingCallTo, //SPR 1392 call deflection
	TxtCallBackQuery, // Marcus: CCBS: 15/11/2002
	TxtRejected, // Marcus: CCBS: 15/11/2002
	TxtActiveCall,				/* SPR#1352 - SH */
	TxtHeldCall,
	TxtActiveTTYCall,
	TxtHeldTTYCall,
	TxtTTY,							
#ifdef NEPTUNE_BOARD /* Mar 27, 2006    REF:ER OMAPS00071798 */
        TxtAudioMode,
	TxtAudioModeText,
	TxtAudioModeVco,
	TxtAudioModeUnknown,
#endif
	TxtBootTime,  /*OMAPS00091029 x0039928(sumanth)*/
#ifdef FF_TTY_HCO_VCO/*x0039928 OMAPS00097714 - HCO/VCO implementation - added strings*/
	TxtTTYAll,
	TxtTTYVco,
	TxtTTYHco,
#endif	
	TxtOnNextCall,
	TxtAlwaysOn,
	TxtTTYCalling,
	TxtIncomingTTYCall,
	TxtProvisioned, // Marcus: Issue 1652: 03/02/2003
	TxtExecuted,  // Marcus: Issue 1652: 03/02/2003
	TxtCommand, // NDH : SPR#1869 : 08/04/2003
	TxtCompleted, // NDH : SPR#1869 : 08/04/2003
	TxtMultiparty, // AP: Issue 1749: 06/03/03
	TxtAttaching,		/* SPR#1983 - SH - Various WAP texts*/
	TxtConnection,
	TxtUsing,
	TxtConnectionSpeed,
	TxtAnalogue,
	TxtISDN9600,
	TxtISDN14400,
	TxtEndWap,
	TxtUntitled,
	TxtChooseReplacement1,
	TxtChooseReplacement2,
	TxtDialup, /* SPR#2324 - SH - Added */
	TxtAdvanced, /* SPR#2324 - SH - Added */
	TxtGPRSDialupGPRS,  /* SPR#2324 - SH - Added */
	TxtGPRSDialupDialup,  /* SPR#2324 - SH - Added */
	TxtProfiles, /* SPR#2324 - SH - Added */
	TxtNewProfile, /* SPR#2324 - SH - Added */
	TxtResetProfiles, /* SPR#2324 - SH - Added */
	TxtPasswords,// MMI SPR 16107
	TxtMismatch,// MMI SPR 16107
	TxtDataCounterReset, /* SPR#2346 - SH */
	TxtCallDate, /* API - 05/09/03 - SPR2357 - Added */
	TxtCallTime, /* API - 05/09/03 - SPR2357 - Added */
	TxtInvalidMessage,/*MC SPR 2530*/
	TxtIncompleteMessage,/*MC SPR 2530*/
	//yanbin add MMS string	
       //TISHMMS Project
    TxtDownload,
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
	TxtShowMessages, /*added by ellen*/
	TxtPushMessages, /*added by ellen*/
#endif

#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS)//MMI-SPR 49811 - x0035544 07 nov 2005
	TxtMMS,
	TxtEMS,
	TxtMMSCreate,
	TxtEMSCreate,
	TxtMMSInbox,
	TxtMMSUnsent,
	TxtMMSSent,//CRR: 25291 - xrashmic 14 Oct 2004
	TxtMMSSettings,
	TxtRetrieveCondition,   //x0012849 May 11 2005 MMI-SPR-29887
	TxtMMSInsertPicture,
	TxtMMSInsertSound,
	TxtMMSInsertText,
	TxtMMSInsertSlide,
	TxtMMSDelete,
	TxtMMSPreview,
	TxtMMSDeleteSlide,
	TxtMMSDeletePicture,
	TxtMMSDeleteSound,
	TxtMMSDeleteText,
        TxtMMSInsert,
        TxtMMSSendSucess,
        TxtMMSSendFail,
        TxtMMSSendng,    
        TxtMMSDownloading,
        TxtNewMMS,
        TxtMMSRcvFail,
        TxtMMSDots,
        TxtExtract,
        TxtMMSNotification,//x0012849 May 11 2005 MMI-SPR-29887
        TxtImmediate,        //x0012849 May 11 2005 MMI-SPR-29887
        TxtDeferred,           //x0012849 May 11 2005 MMI-SPR-29887
        TxtMMSRetrieving,  //x0012849 May 11 2005 MMI-SPR-29887    // MMS Retrieving 
        TxtMMSRetrieved,        //x0012849 May 11 2005 MMI-SPR-29887      // MMS Retrieved
	TxtEMSNew, //xrashmic 26 Aug, 2004 MMI-SPR-23931
	TxtType,//xrashmic 29 Nov, 2004 MMI-SPR-26161
        TxtObject,//xrashmic 29 Nov, 2004 MMI-SPR-26161
        TxtLarge,//xrashmic 29 Nov, 2004 MMI-SPR-26161
        //xrashmic 7 Dec, 2004 MMI-SPR-23965
        TxtMMSInsertPrePicture,	
        TxtMMSInsertPreSound,
        TxtAllObjects, 
        TxtExtracted,        
        TxtExtracting,
        TxtNoObjects, 
        TxtExtractable,
#endif//MMI-SPR 49811 - x0035544 07 nov 2005       
	TxtLockAls, /* NDH : CQ16317 */
	TxtUnlockAls, /* NDH : CQ16317 */
	TxtAlsLocked, /* NDH : CQ16317 */
	TxtAlsUnlocked, /* NDH : CQ16317 */
	TxtLimService, /* SPR12653 */
	
//   Apr 06, 2005	REF: ENH 30011 xdeepadh
//Strings for Camera, Mp3 and midi test application are 
//under the respective flags.	
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */
//#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
	TxtPlayerMonoChannel,
	TxtPlayerStereoChannel,
	TxtPlayerSetChannels,
	TxtPlayerPlay,
	TxtPlayerTest,
	TxtPlayerFileSelected,
	TxtPlayerSelectFile,
	TxtPlayerPause,
	TxtPlayerResume,
	TxtPlayerStop,
	TxtPlayerPlaying,
	TxtPlayerForward,  //  Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
	TxtPlayerRewind,
	TxtPlayerForwarding,
	TxtPlayerRewinding,   //  Jun 06, 2006 REF: ERT OMAPS00070659  x0039928  
	TxtPlayerFileNotLoaded, //Jul 18, 2005    REF: SPR 31695   xdeepadh
	TxtPlayerOptionNotImplemented,//Jul 18, 2005    REF: SPR 31695   xdeepadh
//#endif //FF_MMI_TEST_MP3
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)	
	TxtCameraTest,
	TxtViewFinder, 
	TxtSnapshot, 
	TxtSaveSnapshot, 
	TxtSnapshotSaved,
	TxtSoftSnap,
	TxtSoftSnapSave,
	TxtSnapSaving,
	TxtQuality,
	TxtEconomy,
	TxtStandard, 
	 TxtColor,
	 TxtSepia, 
	 TxtBlackandWhite, 
	 TxtNegative, 
	 TxtFilename, 
	 TxtDigiZoom,
 	 TxtFrame,
	 TxtFrame1,
	 TxtFrame2,	 
	 TxtNoFrame,	 	 
	 TxtShootingMode,
	 TxtSingleshot,
	 Txt2Shots,
	Txt4Shots,
	Txt8Shots,
#endif //FF_MMI_TEST_CAMERA
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Midi Test Application Strings were put under the flag FF_MMI_TEST_MIDI
#ifdef FF_MMI_TEST_MIDI
	TxtMidiTest,
	TxtMidiChangeConfig,
	TxtMidiPlay,
	TxtMidiPlayAllFiles,
	TxtMidiExit,   
	TxtChConfigFile,
	TxtChConfigVoiceLimit,
	TxtChConfigLoop,
	TxtChConfigChannels,
	TxtChConfigBankLocation,    	
	TxtMidiEnterVoiceNumber,
	TxtMidiSelectFile,
	TxtMidiOptionNotImplemented,
	TxtMidiLoopOn,
	TxtMidiLoopOff,       
	TxtMidiChannels,
	TxtMidiMonoChannel,
	TxtMidiStereoChannel,
	TxtMidiBankLocation,
	TxtMidiFastBankLocation,
	TxtMidiSlowBankLocation,
	TxtMidiFileTitle,
	TxtMidiFileNotLoaded,
	TxtMidiFileSelected,
	TxtChConfigAudioOutput,
	TxtMidiSpeaker, 
	TxtMidiHeadset, 
//x0pleela 30 Jan, 2007  DR: OMAPS00108892
//Adding the following strings to display error message
//when loop on is enabled while playing all files
	TxtLoopOn,
	TxtCantPlayAllFiles,
	TxtCantBeActivated,
 #endif //#ifdef FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
	/*
	** Start of Bluetooth Related Strings
	*/
    	TxtBluetooth,
    	TxtBluetoothEnable,
    	TxtBluetoothDisable,
    	TxtBtEnterDeviceName,
    	TxtBtEnterPassKey,
    	TxtBtPassKey,
    	TxtBtTimeout,
    	TxtBtPairing,
    	TxtBtPaired,
    	TxtBtAuthorize,
    	TxtBtUnAuthorize,
	TxtBtInfo,
	TxtBtYesOnce,
	TxtBtYesAlways,
	TxtBtPairDevice,
	TxtBtChangePassKey,
	TxtBtShowServices,
	TxtBtAddToKnown,	
	TxtBtRemoveDevice,
	TxtBtDeviceName,
	TxtBtSet,
	TxtBtServices,
	TxtBtNoServices,
	TxtBtSupported,
	TxtBtSerialPort,
	TxtBtDialUpNetworking,
	TxtBtHeadSet,
	TxtBtHandsFree,
	TxtBtObjectPush,
	TxtBtFileTransfer,
	TxtBtLanAccess,
	TxtBtCordlessTelephony,
	TxtBtIntercom,
	TxtBtUnknownService,
	TxtBtRequiresAuthorizationForService,
	TxtBtDevices,
	TxtBtDevice,
	TxtBtHidden,
	TxtBtTimed,
	TxtBtKnownDevices,
	TxtBtDiscoverable,
	TxtBtNoDevices,
	TxtBtFound,
	TxtBtSetLocalName,
	TxtBtClearStoredDevices,
#endif
    	/*
    	** End of Bluetooth Related Strings
    	*/
    	
 	TxtFDNName,// MMI SPR 18555
	TxtAllDivert,// MMI SPR 13614
	TxtNoReachDivert,  // MMI SPR 13614
	TxtBusyDivert, // MMI SPR 13614
	TxtNoAnsDivert, // MMI SPR 13614
	TxtCondDivert, // MMI SPR 13614
	TxtAllCallsDivert,// MMI SPR 13614
	TxtInvalidInput,// MMI SPR 21547
	TxtFDNListFull, // SPR 31710 x0021308 :RamG



//Nov 29, 2004    REF: CRR 25051 xkundadu
//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//Fix: Enums corresponding to various speaker volume 
//      level strings. 
	TxtLevel1,  
	TxtLevel2,
	TxtLevel3,
	TxtLevel4,
	TxtLevel5,
	TxtImage, //xrashmic 28 Jan, 2005 MMI-SPR-28166
	TxtSmsWaiting,//x0018858 24 Mar, 2005 MMI-FIX-11321
/*Power management menu*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        Txt10Second,
        Txt15Second,
        Txt20Second,
	 TxtSetParam1,     
#endif
#endif
#ifdef FF_MMI_MULTIMEDIA
	/* String to be displayed in BMI. Menu Strings. */
	TxtMultimediaApp,                      
	TxtConfigurationParameters,     
	TxtAudioVideoPlayback,             
	TxtMidiPlayback,                         
	TxtImageViewer,                         
	TxtCameraCapture,                     
	TxtVideoRecording,            
	TxtAudioRecording,
	TxtPlayRecording,
	TxtFileListType,                          
	TxtNoFileAvail,                           
	TxtOutputScreenSize,                 
	TxtAudioEncodingFormat,          
	TxtVideoEncodingFormat,          
	TxtVideoCaptureSize,                 
	TxtVideoFrameRateSelection,   
	TxtAudioBitRate,                        
	TxtVideoBitRate,                        
	TxtMidiPlayerSelect,                  
	/* Configuration Parameter Strings. */
	TxtHalfScreenMode,                   
	TxtFullScreenMode,
	/* Audio Format */
	TxtAAC,
	TxtAMR,
	TxtPCM,
	TxtMP3,
	/* Video Format */
	TxtMpeg4,
	TxtH263,
	/* Frame Size */
       TxtQcif,
	TxtCif,
	/* Video Frame Rate */
	Txt5fps,
	Txt10fps,
	Txt15fps,
	Txt20fps,
	Txt25fps,
	Txt30fps,
	/* Bit Rate */
	TxtAACBitRate,
	AMRNBBitRate,
	/* AAC Bit Rate */
	Txt48Kbps,
	Txt66p15Kbps,
	Txt72Kbps,
	Txt96pKbps,
	Txt132p3Kbps,
	Txt144Kbps,
	Txt192Kbps,
	Txt264p6Kbps,
	Txt288Kbps,
	/* AMR Bit Rate */
       Txt1p80Kbps,
	Txt4p75Kbps,
	Txt5p15Kbps,
	Txt5p9Kbps,
	Txt6p7Kbps,
	Txt7p4Kbps,
	Txt7p95Kbps,
	Txt10p2Kbps,
	Txt12p2Kbps,
       /* Video Bit Rate */
	Txt64Kbps,
	Txt128Kbps,
	Txt384Kbps,
	/* Softkey display strings */
	TxtPauseKey,
	TxtResumeKey,
	TxtEncodeKey,     
	TxtSoftIVT,            
	TxtPictureDisp, 
	TxtSetParam,     
	/* Audio Path Configuration Parameter */
	TxtAudioPath,
	TxtHeadset,
	TxtHeadphone,
	/* Audio / Video Recording related strings. */
	TxtSoftStart,
	TxtAudvidRecord,
	TxtQQVga,           
	TxtEncdTo,          
	TxtJPEGFile,       
	TxtFTPApp,
	TxtPut,
	TxtGet,
	/* Configuration parameter, Volume and Preferred storage.*/
	TxtVolumeConfig,
	TxtPreferredStorage,
	TxtAudioDecodeConfig,
	TxtAudioEncodeConfig,
	TxtVideoEncodeConfig,
	TxtSpeaker,             
	TxtEnterPath,
	TxtInvalidPath,
   	TxtVMPlayback,
	TxtVMRecord,
      TxtVMConfigParam,
      TxtStartRecord,
      TxtPlayingfile,
      TxtPauseFile,
      TxtRecordVoice,
      TxtSoftMore,
      TxtUseCase,
      TxtRepeateForEver,
      TxtPlayonce,
      TxtCamera, 
      TxtCapture,
      TxtCrop,
      TxtRescale,
      TxtRotate,
	TxtDummyStr,
	TxtDummyStr1,
       /* PCM Sample Rate */
       Txt8kRate,
       Txt11kRate,
       Txt12kRate,     /*OMAPS00081264 - 12k Menu addition */
       Txt16kRate,
       Txt22kRate,
       Txt24kRate,     /*OMAPS00081264 - 24k Menu addition */
       Txt32kRate,
       Txt44kRate,
       Txt48kRate,
       TxtPcmRate,
       TxtPcmChannelMode, 
       /* Image Capture Quality Setting */
      TxtImgCaptureQuality,
      Txt0Set,
      Txt10Set,
      Txt20Set,
      Txt30Set,
      Txt40Set,
      Txt50Set,
      Txt60Set,
      Txt70Set,
      Txt80Set,
      Txt90Set,
      Txt100Set,      
      TxtStreoMode,
      TxtMonoMode,
      TxtVideoPlayOption,
      TxtAudioVideoPlay,
      TxtVideoPlay,
      /* Video Encoding Time Setting */
      TxtVideoRecDuration,
      Txt10sec,
      Txt15sec,
      Txt20sec,
      TxtNonStop,
/* Video Only Option - x0043641 */
      TxtVideoRecordOption,
      TxtAudioVideoRecord,
      TxtVideoRecord,
#endif
// 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
	TxtLoudspeaker,
	TxtHandheld,
	TxtCarkit,
	TxtHeadset,
	TxtInserted,
	TxtRemoved,
#endif	
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef F_MMI_FILE_VIEWER
	TxtFileNotLoaded,//file viewer
	TxtFileViewer,////May 02, 2005 REF:Camera App xdeepadh
	TxtRename, 
	TxtDeleteConfirm,
	TxtFileTooBig,
	TxtFileExists, //Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh 
	TxtWrongFilename, 	//Aug 31, 2005    REF: SPR 34050 xdeepadh
	TxtNoExtension,		// 	Aug 31, 2005    REF: SPR 34050 xdeepadh
#endif // FF_MMI_FILE_VIEWER
#if defined(FF_MMI_FILE_VIEWER) || defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
	TxtRotate,
	TxtRotate90Degree, 
	TxtRotate180Degree, 
	TxtRotate270Degree, 
	TxtZoom, 
	TxtZoomIn, 
	TxtZoomOut,
	TxtZoomInMax, 
	TxtZoomOutMax, 

#endif // FF_MMI_FILE_VIEWER

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
       TxtPowerMgnt,
#endif
#endif
#ifdef FF_MMI_USBMS
	TxtUSB,
	TxtUSBEnumeration,
	TxtUSBMode,
 	TxtContinue,	
 	TxtUSBDisconnect,
 	TxtUSBMS,
 	TxtUSBTrace,
 	TxtUSBFax, 
 	TxtPSShutdown,
 	TxtNoPSShutdown,
 	//xashmic 27 Sep 2006, OMAPS00096389 
	TxtUSBFaxnTrace,
	TxtUSBMSnTrace,
	TxtUSBFaxnMS,
	TxtUSBMSPorts,
	TxtPS,
	TxtPopupMenu,
#endif
    // x0021334 10 May, 2006 DR: OMAPS00075379
    TxtPending,

    // x0021334 02 June, 2006 DR: OMAPS00078005
#ifdef FF_TIMEZONE
    TxtTimeUpdate,
    TxtTimeUpdateConfirm,
    TxtNitzTimeUpdate,
#endif
    //x0pleela 30 Aug, 2006 DR: OMAPS00091250
	TxtNoMEPD,   
//x0pleela 24 Aug, 2006  DR:OMAPS00083503
//Adding new strings to display if the characters are UCS2 in USSD transactions
#ifdef MMI_LITE
	TxtUSSDMsgRxd, 
	TxtFontNotSupported,
#endif   
//x0pleela 19 Feb, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	TxtPhoneLock,
	TxtPhLockEnable,
	TxtPhLockAutomatic,
	TxtPhLockAutoOn,
	TxtPhLockAutoOff,
	TxtPhLockChangeCode,
	TxtEnterPhUnlockCode,
	TxtEnterOldUnlockCode,
	TxtEnterNewUnlockCode,
	TxtAlreadyEnabled,
	TxtAlreadyDisabled,
	TxtAutoLockDisable,
	TxtAutoPhone, 
	TxtSuccess,
	TxtNotLocked,
#endif	/* FF_PHONE_LOCK */
/*OMAPS00098881(removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
       TxtScreenUpdate,
#endif
//Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
#ifdef FF_MMI_CAMERA_APP
	TxtNotEnough,
	TxtCamMemory,
#endif
	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
	TxtFwdIncomingTTYCall, /* String for Fwd Incoming TTY call */
	TxtFwdIncomingCall,  /* String for Fwd Incoming  call */
	 //06 July, 2007 DR: OMAPS00137334 x0062172 Syed
	TxtConferenceFull,  // String for Conference Full notification 

#ifdef FF_MMI_FILEMANAGER
	/* File Management related Strings. */
	TxtFileMgmt,
	TxtFileList,
	TxtFFS,
	TxtNORMS,
	TxtNAND,
	TxtTflash,
	TxtOpen,
	TxtFormat,
	TxtProperties,
	TxtImageFiles,
	TxtAudioFiles,
	TxtFile,
	TxtDirectory,
	TxtFreeSpace,
	TxtUsedSpace,
	TxtThumbnail,
	TxtExists,
	TxtCreateDir,
	TxtCreated,
	TxtNotEmpty,
	TxtMemorystat,
	TxtSourceDest,
	TxtSame,
	TxtAudNotAllowed,
	TxtAudAudioError,
	TxtAudDenied,
	TxtAudPlayerError,
	TxtAudFFSError,
	TxtAudMessagingError,
	TxtAudNotReady,
	TxtAudInvalidParam,
	TxtAudInternalError,
	TxtAudMemoryError,
	TxtAudNotSupported,
	TxtCopying,
	TxtFileNotLoaded,//file viewer
	TxtFileViewer,////May 02, 2005 REF:Camera App xdeepadh
	TxtRename, 
	TxtDeleteConfirm,
	TxtFileTooBig,
	TxtFileExists, //Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh 
	TxtWrongFilename, 	//Aug 31, 2005    REF: SPR 34050 xdeepadh
	TxtNoExtension,		// 	Aug 31, 2005    REF: SPR 34050 xdeepadh
	TxtStorageDevice,
	TxtPBar,
#endif
  //Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    TxtTo,
/* April  03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
#ifdef FF_MMI_CAMERA_APP
    TxtAutoSave,
    TxtAutoSaveOn,
    TxtAutoSaveOff,
    TxtSoftSave,
    TxtSoftCancel,
#endif /* FF_MMI_CAMERA_APP */ 
//May 03 2007 ER: x0061088 (Prachi)
#if CAM_SENSOR == 1
    TxtCamMenuResolution,
    TxtCamVga,
    TxtCamQcif,
    TxtCamSxga,
    TxtCamNot,
    TxtCamSupported,
#endif 
   NO_OF_DEFINED_TEXT_IDS	//GW Leave this as the last element.
} tIndexTagNames, LangTxt;

#if 0
//SPR#1147 - DS - Added one string for incoming fax to lang table.
#define LEN_LANGUAGE1 662 //English language
//SPR#1147 - DS - Added one string for incoming fax to lang table.
#ifndef CHINESE_MMI //German language
#define LEN_LANGUAGE2 662 
#else //Chinese language
#define LEN_LANGUAGE2 662
#endif /* CHINESE_MMI */
#else
/*
** SPR#1869 - NDH - I have moved this so that it is after the ENUM, so that it is possible
**                 to use NO_OF_DEFINED_TEXT_IDS as the size of the Language Array
*/
#define LEN_LANGUAGE1  NO_OF_DEFINED_TEXT_IDS //English language

/*
** SPR#1869 - NDH -  The second language needs to be the same size as the first regardless
**                  of whether it is German, Chinese or Martian
*/
#define LEN_LANGUAGE2  NO_OF_DEFINED_TEXT_IDS
#endif

/*******************************************************************************
                                                                              
                                Public Routine Definitions                                 
                                                                              
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
    void *Mmir_BaseAddress(void);
    int Mmir_NumberOfEntries(void);
#ifdef __cplusplus
}
#endif



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
