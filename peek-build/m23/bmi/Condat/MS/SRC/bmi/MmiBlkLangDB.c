/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Resource Manager
 $File:		    MmiBlkLangDB.c
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
 $History: MmiBlkLangDB.c

  Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat)
  Description: MP3 - AAC cannot be disabled in Locosto
  Solution: MP3 AAC Flags Usage have been corrected
  
  Jun 20, 2007 DR: OMAPS00139535 x0080426 Sinara
  Description: Typo in MmiBlkLangDB.c
  Solution: Mismatch in the braces for TxtCamNot string for German language 

   May 03 2007 ER: x0061088 (Prachi)


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


	Jan 30, 2007 DR: OMAPS00108892 x0pleela
 	Description: TCMIDITEST.23(Apps) - Play All File feature fails when different Configurations
 				like Mono/Stereo, Loop On/Off are selected
 	Solution: 	Defined new strings TxtLoopOn, TxtCantPlayAllFiles, TxtCantBeActivated to display an
 			error message if we try to enable Loop on while playing all files
	Feb 27, 2007 ER: OMAPS00113891 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on BAT/ATI level
 	Solution: Added strings TxtFwdIncomingTTYCall( for Fwd. TTY call ) and 
 			TxtFwdIncomingCall (Fwd. Inc. Call)to display for any forwarded incoming call
 
      Oct 30, 2006 ER: OMAPS00091029 x0039928(sumanth)
      Bootup time measurement

      Oct 30, 2006 ER:OMAPS00098881 a0393213(Prabakar)
	Removing power variant
	      
	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Added strings TxtBlkNetwork( for category Blocked Network) and 
 			TxtEnterPbLock (for editor title "Enter Blocked NW code")
 

 	Oct 07, 2006 DR: OMAPS00098180 x0039928
 	Description:Undefined symbols in BMI
 	Solution: Removed TxtGarbageColloect from flags FF_MMI_FILE_VIEWER and FF_MMI_IMAGE_VIEWER


      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      
 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xrashmic 9 Sep 2006, OMAPS00092732 
	USBMS ER: Added strings related to USB MS 

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

 	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new strings for menu items and dialog displays for PCM voice memo and voice buffering 
	
	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new strings which are used in menu items to support PCM Voice memo feature
	
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
    
       May 03, 2006    DRT: OMAPS00076856 xdeepadh
   	Description:  Syntax error in MMI file
   	Solution: The syntax error has been corrected.

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

 	Sep 14, 2005    REF: SPR 34020 xdeepadh
   	Description:  Certain Sub menu items are not displayed in Chinese
   	Solution: The chinese strings have been rearranged as per the English language.
 	Aug 29, 2005    REF: SPR 31712 xdeepadh
	Description: GPRS Preferred Activation message screen style is displayed inside a rectangle box
	Solution: GPRS Preferreed and CSD Preferred string have been shortened.

  	Aug 22, 2005    REF: ENH 31154 xdeepadh
   	Description: Application to Test Camera
   	Solution: Implemeted the Fileviewer to view the jpeg images.Camera Application to 
   	preview,capture and save image has been implemented.
   	Strings for Fileviewer and Camera  have been added.
18/05/04			Texas Instruments	   

        Jul 18, 2005    REF: SPR 31695   xdeepadh
	Bug:Problems with MP3 test application
	Fix:The window handling of MP3 Test Application has been done properly.


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
   Solution: Added Strings "MyNumber Full" appropriately for English, Chinese and German string in 
   Enum IndexTag.

   Jun 13, 2005	REF: SPR 31710 x0021308
   Description: After the maximum limit of the FDN Number  is exceeded, display info : Phonebook full 
   expected is FDNList Full
   Solution: Added Strings "FDNList Full" appropriately for English, Chinese and German string in Enum
   IndexTag. 
     May 11 2005  REF:  MMI-SPR-29887  x0012849
    To Implement the deferred MMS retrieval.

   May 02, 2005    REF: SPR 30755 xdeepadh
   Description: Add the missing strings in chinese string database.
   Solution: Added the missing Chinese Strings
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

    xrashmic 28 Jan, 2005 MMI-SPR-28166
    Added support for saving Plugin Images
    
   Jan 12, 2004    REF: CRR 28078 xnkulkar
   Description: Compilation error in MmiBlkLangDB.c with FF_WAP && FF_TCPIP
   Solution: TxtPPGAuthenication changed to TxtPPGAuthentication

    Nov 29, 2004    REF: CRR 25051 xkundadu
    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
    Fix: Added volume level list linked to up/down keys. 
          User can select the speaker volume among those levels.
 xrashmic 7 Dec, 2004 MMI-SPR-23965
    Description: Not able to extract the objects in EMS message
    Solution: Extract and store the objects in the Object Manager

    xrashmic 29 Nov, 2004 MMI-SPR-26161
    Description: The downloaded object through wap was not visible to the MMI
    Solution: The downloaded wap object is available in the object manager 
    and can be used by the MMS
    CRR: 25291 - xrashmic 14 Oct 2004
    Description: The sent EMS is stored in unsent folder.
    Solution: The Outbox has been removed, instead we now have Sent folder for the sent EMS 
        xrashmic 26 Aug, 2004 MMI-SPR-23931
        To display the indication of new EMS in the idle screen
        
	xrashmic 19 Aug, 2004 Bug: 2, 3, 36 and 42
	All the status screen, .i.e sending, receiving etc. are displayed separatly for the MMS module.
	Previously the WAP screens were being used. 

	25/10/00			Original Condat(UK) BMI version.
	
    Jul 23, 2004        REF: CRR 16107  xvilliva
    Bug:  (Call Barring) - MMI response overlayes partially background images.
    Fix:    Modified entries to string array to show appropriate alert.

//  Jun 25, 2004        REF: CRR 21547  xvilliva  (Added TxtInvalidInput.)


	May 10, 2004    REF: CRR 15753  Deepa M.D 
	Menuitem provided to set the validity period to maximum.
	   	   
	Apr 14, 2004        REF: CRR 12653  xvilliva  (Added "Limited Service".)

	Issue Number : SPR#15568 on 24/03/04 by Nishikant Kulkarni
    Issue Number : SPR#12883 on 29/03/04 by VadiRaj
    Issue Number : SPR#12240 on 7/04/04 by Deepa M D


//    Jun 10, 2004        	REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
//  	Added  Strings "All Divert", "Divert when not reachable", "Divert if Busy", "Divert when no answer"
	"Divert for Cond. Forward" , "Divert for All calls"


    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode.

    June 09, 2006 REF:ER OMAPS00081264  x0020906
    Added menu string for 12k and 24k PCM rate.    
 $End

 
*******************************************************************************/


/*******************************************************************************
                                                                              
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
#include "MmiBlkLangDB.h"

/*******************************************************************************
                                                                              
                                Local structures                                 
                                                                              
*******************************************************************************/


/*******************************************************************************
                                                                              
                                Local data tables                                 
                                                                              
*******************************************************************************/

/* Text table 1, autogenerated to provide English text associations.
*/

//DON'T TOUCH THE FOLLOWING LINE...
//Language = "English"

static const tPointerStructure LanguageIndex_1[LEN_LANGUAGE1+1] = {
    { TxtNull, "" },
#ifdef FF_CPHS
  { TxtEditLineName, "Edit Line Name"}, //x0pleela 23 May, 2006  DR: OMAPS00070657
#endif
//x0pleela 24 Feb, 2006  ER OMAPS00067709
//Added TxtPcmVoiceMemo strings used in menu items to support PCM Voice memo feature
//x0pleela 09 Mar, 2006   ER: OMAPS00067709
//Added the following strings used in menu items to support Voice buffering feature
#ifdef FF_PCM_VM_VB
{TxtStopFailed, "Stop Failed"},
{TxtAlready, "Already"},
{TxtPcmVoiceMemo, "PCM Voice Memo"},
{TxtVoiceBuffering, "Voice Buffering"},
{TxtEditNum, "Edit number"},
{TxtStartRec, "Start Recording"},
{TxtStopRec, "Stop Recording"},
#endif
#ifdef SIM_PERS 
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
{TxtBlkNetwork, "Blocked Network"},	//For new category "Blocked Network" 
{ TxtEnterPbLock,"Enter Blocked NW Code"}, //Editor string to display "Enter Blocked Network password"
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
{TxtBusy,"Busy"}, 						//Displays "Busy" if timer is enabled for wrong entry of password
{TxtMasterUnlock,"Master Key Unlock"}, 	//For menu option Master unlock
{TxtEnterMasterKey, "Enter Master Key"},	//Text for the editor to enter master unlock key
{TxtUnblockME, "Unblock ME"},			//For menu option Unblock ME
{TxtUnblockCode, "Enter Unblock Code"}, //x0pleela 16 May, 2006  DR: OMAPS00067919
{TxtTimerFlag, "Unlock Timer Flag"},
{TxtETSIFlag, "ETSI Flag"},
{TxtAirtelFlag, "AirTel Ind. Flag"},
{ TxtEnterOldPsLock,"Enter Old LockCode"},
{ TxtEnterPsLockNew,"Enter New LockCode"},
{ TxtEnterPsLockconf,"Confirm New LockCode"},
 { TxtLockEnabled, "Lock Enabled"},
    { TxtLockDisabled,"Lock Disabled"},
    { TxtPersonalization, "Personalization"},
    { TxtSupplementaryInfo, "Supplementary Info"},
    { TxtPersonalize, "Personalize"},
    { TxtUnlockCategory, "Unlock Category"},
    { TxtLockCategory, "Lock Category"},
    { TxtResetFC, "Reset Failure counter"},
    { TxtGetFC, "Get Failure Counter"},
    { TxtMaxFC,"Max Failure Count"},
    { TxtCorporate, "Corporate"},
    { TxtServiceProvider, "Service Provider"},
    { TxtNetworkSubset, "Network Subset"},
    { TxtLockActivated,"Lock Activated"},
    { TxtLockDeactivated,"Lock Deactivated"},
    { TxtEnterNLock,"Enter Network Password"},
    { TxtEnterNsLock,"Enter Netw. Subset Password"},
    { TxtEnterSpLock,"Enter Serv. Provider Password"},
    { TxtEnterCLock,"Enter Corporate Password"},
    { TxtEnterPsLock,"Enter SIM Password"},
     { TxtPsLockConfWrong,"Password does"},
    { TxtPsLockConfwrong2,"not match"},
    { TxtPsLockChanged,"Code changed"},
    { TxtCounterReset,"Counter Reset"},
     { TxtPassfailed,"Change failed"}, 
     {TxtUnblock,"Unblocking Sucess"},
    { TxtAlreadyLocked,"Category Already Locked"},
    { TxtAlreadyUnlocked,"Category Already Unlocked"},
    {  TxtPhoneUnblocked,"Phone Unblocked"},
    { TxtPhoneBlocked, "Phone Blocked" },
    { TxtGetFailFC,"Reset FC Fail Attempts left"},
	{ TxtMaxFailFC,"Max Reset FC Fail Attempts"},
	{ TxtGetSuccFC,"Reset FC Succ Attempts left"},
	 { TxtMaxSucFC,"Max Reset FC Succ Attempts"},
#endif    
#ifdef FF_MMI_CPHS
    { TxtViewAll,"View All"},
    { TxtEmergencyNumbers,"Emergency Numbers"},
#endif    
    { TxtReport, "Report"}, //x0pleela 01 Feb, 2006 DR: OMAPS00059261
    { TxtNotImplemented, "Inval. Request" },
    { TxtManu, "CONDAT" },
    { TxtEnterPin1, "Enter PIN1" },
    { TxtEnterPin2, "Enter PIN2" },
    { TxtEnterPuk1, "Enter PUK1" },
    { TxtEnterPuk2, "Enter PUK2" },
    { TxtInvalidCard, "SIM Invalid" },
    { TxtNoCard, "Insert SIM" },
    { TxtInvalidIMEI, "Invalid IMEI" }, // Nov 24, 2005, a0876501, DR: OMAPS00045909
    { TxtSoftCall, " Call" },
    { TxtSoftOK, "OK" },
    { TxtSoftMenu, "Menu" },
    { TxtHelp, "Help" },
    { TxtSoftContacts, "Contacts" },
    { TxtPlay, "Play" },
    { TxtMessages, "Messages" },
    { TxtAlarm, "Alarm" },
    { TxtSettings, "Settings" },
    { TxtExtra, "Extra" },
    { TxtSend, "Send" },
    { TxtSent, "Sent" },
    { TxtNotSent, "Not sent" },
    { TxtServiceCentre, "Service Cent." },
    { TxtPleaseWait, "Please wait" },
    { TxtSmsEnterNumber, "Enter number" },
    { TxtRead, "Read" },
    { TxtRetrieve, "Retrieve" },   // x0012849 11 May 205 MMI-SPR-29887
    { TxtDeleteAll, "Delete all" },
    { TxtSave, "Save" },
    { TxtVoicemail, "VoiceMail" },
    { TxtBroadcast, "Broadcast" },
    { TxtNew, "New" },
    { TxtDigits, "123" },
    { TxtLowercase, "abc" },
    { TxtUppercase, "ABC" },
    { TxtApplications, "Applications" },
#ifndef FF_NO_VOICE_MEMO
    { TxtVoiceMemo, "Voice Memo" },
#endif
    { TxtSimNot, "SIM not " },
    { TxtReady, "ready !" },
    { TxtSmsListFull, "Messages: full" },
    { TxtNewCbMessage, "New CB" },
    { TxtSaved, "Saved" },
    { TxtDelete, "Delete" },
    { TxtReply, "Reply" },
    { TxtStoreNumber, "Store Number" },
#ifdef FF_CPHS_REL4    
    { TxtMSP, "MSP Profile" },
    { TxtMessageStatus, "Message Status" }, 
    { TxtMailBox, "MailBox"},
    { TxtElectronic, "Electronic"},
#endif    
    { TxtForward, "Forward" },
    { TxtActivate, "Activate" },
    { TxtTopics, "Topics" },
    { TxtConsult, "Consult" },
    { TxtAddNew, "Add New Entry" },
    { TxtContacts, "Contacts" },
    { TxtSearchName, "Search" },
    { TxtMemory, "Memory Stat." },
    { TxtMyNumber, "My Number" },
    {TxtMyNumberFull, "My NumberFull"}, //SPR 31705 x0021308:RamG
    { TxtRinger, "Ringer" },
    { TxtVolume, "Volume" },
    { TxtKeypadTones, "Keypad Tones" },
    { TxtNewMessage, "New Message" },
    { TxtNewBroadcast, "New Broadcast" },
    { TxtAutoAnswer, "Auto-Answer" },
    { TxtTimeDate, "Time/Date" },
    { TxtLanguages, "Languages" },
    { TxtVoiceMail, "VoiceMail ?" },
    { TxtPinCode, "Pin Code" },
    { TxtAutoredial, "AutoRedial" },
    { TxtDTMF, "DTMF" },
    { TxtOn, "On" },
    { TxtOff, "Off" },
    { TxtNetworkSel, "Network Sel." },
    { TxtSendID, "Hide ID" },
    { TxtChargeTimers, "Charge/Timers" },
    { TxtCallDivert, "Divert" },
    { TxtCallWaiting, "Call Waiting" },
    { TxtIfBusy, "If Busy" },
    { TxtIfNoAnswer, "If No Answer" },
    { TxtIfNotReachable, "Conditional Forward" }, /*SPR#15568 - Changed from "If Not Reach" to "Conditional Forward" on 24/03/04 SASKEN*/
    { TxtAllCalls, "All Calls" },
    { TxtCancelAll, "Cancel All" },
    { TxtAll, "All" },
    { TxtVoice, "Voice" },
    { TxtFax, "Fax" },
    { TxtData, "Data" },
    { TxtStatus, "Status" },
    { TxtCancel, "Cancel" },
    { TxtCalculator, "Calculator" },
    { TxtEnterName, "Enter Name:" },
    { TxtSoftBack, "Back" },
    { TxtSoftSelect, "Select   " },
    { TxtSoftOptions, "Options" },
    { TxtEmptyList, "Empty List" },
    { TxtSendMessage, "Send message" },
    { TxtChange, "Change" },
    { TxtEnterNumber, "Enter number:" },
    { TxtPhbkFull, "Phonebook full" },
    { TxtWriteError, "Write error" },
    { TxtEntryDeleted, "Entry deleted" },
    { TxtDetails, "Details" },
    { TxtHold, "Hold" },
    { TxtUnhold, "Unhold" },
    { TxtMute, "Mute" },
    { TxtSendDTMF, "Send DTMF" },
    { TxtMainMenu, "Main menu" },
    { TxtSwap, "Swap" },
    { TxtEndAll, "End all" },
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Added a menu option. "End held" text is added to the language index
    { TxtEndHeld, "End held" }, 
    { TxtPrivateCall, "Private call" },
    { TxtStartConference, "Start conference" },
    { TxtReStartConference, "Re-start conference" },
    { TxtEndConference, "End conference" },
    { TxtFind, "Find:" },
    { TxtServiceNumbers, "Service no." },
    { TxtOutofRange, "Out of Range" },
    { TxtDone, "Done" },
    { TxtFailed, "Failed" },
#ifdef FF_MMI_MULTIMEDIA
    { TxtFileNotSupported, "File Not Supported" },
#endif    
    { TxtEnterNewPin, "Enter New PIN" },
    { TxtConfPin, "Confirm PIN" },
    { TxtChangPin, "PIN Changed" },
    { TxtEnd, "End" },
    { TxtPINFail, "PIN Failed" },
    { TxtSOS, "SOS" },//Change by Sasken ( VadiRaj) on March 29th 2004, Issue Number : MMI-SPR-12883, Solution: Changing the string "Calling SOS" to "SOS" 
    { TxtAutomatic, "Automatic" },
    { TxtManual, "Manual" },
    { TxtFixedNames, "Fixed Names" },
    { TxtFactoryReset, "Factory Reset" },
    { TxtSearching, "Searching" },
    { TxtChangPIN2, "PIN2 Changed" },
    { TxtOldPIN, "Enter Old PIN" },
    { TxtOldPIN2, "Enter Old PIN2?" },
    { TxtNotAcc, "Not Accepted" },
    { TxtEnabled, "PIN Enabled" },
    { TxtDisabled, "PIN Disabled" },
    { TxtPINOK, "PIN OK" },
    { TxtSimBlocked, "SIM Blocked" },
    { TxtNewPIN, "New PIN" },
    { TxtAcc, "Accepted" },
    { TxtNoNetwork, "No Network" },
    { TxtNo, "No" },
    { TxtEnterDate, "Enter Date" },
    { TxtEnterTime, "Enter Time" },
    { TxtScratchPad, "Scratchpad" },
    { TxtDeleted, "Deleted" },
    { TxtActivated, "Activated" },
    { TxtDeActivated, "DeActivated" },
    { TxtDeactivate, "Deactivate" },
    { TxtNotActivated, "Not Activated" }, // Dec 07, 2005  REF: OMAPS00050087 x0039928 - Added string to lang table
    { TxtCheck, "Check" },
    { TxtYes, "Yes" },
    { TxtCallNumber, "Call Number" },
    { TxtEnterMid, "Enter MID" },
    { TxtReadError, "Read Error" },
    { TxtUsed, "Used" },
    { TxtOutgoing, "Outgoing" },
    { TxtMuted, "Muted" },
    { TxtCallEnded, "Call ended" },
    { TxtIncomingCall, "Incoming call" },
    { TxtAnsweredCalls, "Answered Calls" }, // API - APR #1629 - Added string to lang table.
    { TxtIncomingData, "Incoming data" },
    { TxtIncomingFax, "Incoming fax" }, //SPR#1147 - DS - Added string to lang table.
    { TxtNumberWithheld, "Num. withheld" }, //SPR#1147 - DS - Modified to fit on screen.
    { TxtAccept, "Accept" },
    { TxtReject, "Reject" },
    { TxtCalling, "Calling" },
    { TxtMissedCalls, "Missed Calls" },
    { TxtMissedCall, "Missed Call" },
    { TxtLocked, "Keys Locked" },
    { TxtUnmute, "Unmute" },
    { TxtCharging, "Charging..." },
    { TxtChargComplete, "Charging\nComplete" },
    { TxtOperationNotAvail, "Not available" },
    { TxtAddToConference, "Add to conference" },
    { TxtEmpty, "Empty" },
    { TxtSelectFromList, "Replace with..." }, /* SPR#2354 - SH */
    { TxtEnterNetworkCode, "Network Code" },
    { TxtMCCMNC, "MCC / MNC ?" },
#ifndef FF_NO_VOICE_MEMO
    { TxtRecord, "Record" },
#endif
    { TxtSecurity, "Security" },
    { TxtEnglish, "English" },
    { TxtGerman, "German" },
    { TxtValidity,"Validity" }, 
    { TxtMessageTyp,"Message Typ" }, 
    { TxtSetVoiceMail,"Set VoiceMail" }, 
    { TxtCallTimers,"Call Timers" }, 
    { TxtLastCharge,"Last Charge" },
    { TxtTotalCharge,"Total Charge" },
    { TxtResetCharge,"Reset Charge" },
    { TxtIfNoService,"Not Reachable" }, /*SPR#15568 - Changed from "If No Service" to "Not Reachable" on 24/03/04 SASKEN*/
    { TxtDiscard, "Discard" }, 
    { TxtEDIT, "Edit" }, 
    { TxtExtractNumber, "Extr.Number." },
    { TxtWrite, "Write" },
    { TxtCheckStatus,"Check Status" },
    { TxtSetDivert,"Set Divert" },
    { TxtCancelDivert,"Cancel Divert" },
    { TxtHideID,"Hide ID" }, 
    { TxtTones,"Tones" },
    { TxtClock,"Clock" },
    { TxtPhone, "Phone" },
    { TxtStop, "Stop" },
#ifndef FF_NO_VOICE_MEMO
    { TxtRecording, "Recording" },
    { TxtPlayback, "Playback" },
#endif
    { TxtNoNumber, "No number" },
    { TxtOkToDelete, "Delete all ?" },
    { TxtBalance, "Balance" },
    { TxtLimit, "Limit" },
    { TxtLimitEq, "Limit=" },
    { TxtRate, "Rate" },
    { TxtMinutes, "Minutes=" },
    { TxtCost, "Cost=" },
    { TxtCredit, "Credit" },
    { TxtPressOk, "Press Ok to" },
    { TxtReset, "Reset" },
    { TxtIncoming, "Incoming" },
    { TxtCounter, "counter" },
    { TxtOutgoingCalls, "Outgoing calls" },
    { TxtCostCounter, "cost counter" },
    { TxtEnterCurrency, "Enter currency" },
    { Txt1Unit, "1 Unit=..." },
    { TxtChanged, "Changed" },
    { TxtEnterLimit, "Enter limit:" },
    { TxtCancelled, "Cancelled" },
    { TxtView, "View" },
    { TxtEnterPCK, "Enter PCK:" },
    { TxtEnterCCK, "Enter CCK:" },
    { TxtEnterNCK, "Enter NCK:" },
    { TxtEnterNSCK, "Enter NSCK:" },
    { TxtEnterSPCK, "Enter SPCK:" },
    { TxtCodeInc, "Password incorrect" },
    { TxtBlckPerm, "Blocked permanently" },
    { TxtDealer, "contact your dealer" },
    { TxtSendingDTMF, "Sending DTMF" },
    { TxtCheckNumber, "Check number" },
    { TxtNumberBusy, "Number busy" },
    { TxtNoAnswer, "No answer" },
    { TxtNumberChanged, "Number changed" },
    { TxtNetworkBusy, "Network busy" },
    { TxtNotSubscribed, "Not subscribed" },
    { TxtAnyKeyAnswer, "Any-key answer" },
    { TxtPINBlock, "PIN Blocked" },
    { TxtEdit, "Edit" },
    { TxtChangePassword,"Change Passw." },
    { TxtSetBar,"Set Bar" },
    { TxtSmsTone,"SMS Tone" },
    { TxtMelody,"Melody" },
    { TxtSilent,"Silent" },
    { TxtClick,"Click" },
    { TxtClearAlarm,"Clear Alarm" },
    { TxtTimeFormat,"Time Format" },	
    { TxtTwelveHour,"12 Hour Clock"},
	{ TxtTwentyfourHour,"24 Hour Clock"},
    { TxtInCallTimer,"In-callTimer" },
    { TxtStore, "Store" },
    { TxtNone, "None" },
    { TxtPIN, "PIN" },
    { TxtPIN2, "PIN2" },
    { TxtConfPin2, "Confirm PIN2" },
    { TxtNewPIN2, "Enter New PIN2" },
    { TxtUnlockOK, "Unlock OK" },
    { TxtIMEI, "IMEI" },
    { TxtList, "List" },
    { TxtFull, "Full" },
    { TxtNotAvailable, "NotAvailable" },
    { TxtCallBAOC, "CallBAOC" },
    { TxtCallBAOIC, "CallBAOIC" },
    { TxtCallBAOICexH, "CallBAOICexH" },
    { TxtCallBAIC, "CallBAIC" },
    { TxtCallBAICroam, "CallBAICroam" },
    { TxtCallBarringAll, "Call Barring" },
    { TxtCallBAOCAll, "CallBAOCAll" },
    { TxtCallBAICAll, "CallBAICAll" },
    { TxtCLIP, "CLIP" },
    { TxtCOLP, "COLP" },
    { TxtCOLR, "COLR" },
    { TxtSimToolkit, "SIM Toolkit" },
    { TxtPhonebook, "PhoneBook" },
    { TxtRecentCalls, "Recent Calls" },
    { TxtNames, "Names" },
    { TxtInterCalls,"Internat.Calls" },
    { TxtInterHome,"Internat.but Home" },
    { TxtWhenRoaming,"when Roaming" },
    { TxtCancelAllBars,"Cancel All B." },
    { TxtWaiting, "Waiting" },
    { TxtBarred, "Barred" },
    { TxtForwarded, "Forwarded" },
    { TxtCallDivertNoReach, "Divert No Reach" },
    { TxtCallDivertNoReply, "Divert No Reply" },
    { TxtCallDivertIfBusy, "Divert If Busy" },
    { TxtPressMenu, "Press Menu, * " },
    {TxtPlus, "Plus"},
    {TxtMinus, "Minus"},
    {TxtDivideBy, "Divide By"},
    {TxtMultiplyBy, "Multiply By"},
    {TxtEquals, "Equals"},
    {TxtSending, "Sending"},
    {TxtMessage, "Message"},
    {TxtSoftNext,"Next"},  
    {TxtLastCall,"Last Call"},
    {TxtIncomingCalls,"Incoming Calls"},
    {TxtLow,"Low"},
    {TxtMedium,"Medium"},
    {TxtHigh,"High"},
    {TxtIncreasing,"Increasing"},
    {TxtTimerReset, "Timer Reset"}, 
    {TxtClockSetTo, "Clock Set to"},
    {TxtSilentMode, "Silent Mode"},
    {TxtSmsValidityPeriodSet,"Period Set"}, /*SPR#1952 - Shortened to fit on pop-up*/
    // change by Sasken (Deepa M D) on Aprinl 7th 2004
    // Issue Number : MMI-SPR-12240
    // Bug: When selecting a message validity period, only a portion of the text is displayed.
    // Solution: Added space to display the string properly on the screen 
    {Txt1Hour,"1 Hour"},
    {Txt12Hours,"12 Hours"},
    {Txt24Hours,"24 Hours"},
    {Txt1Week,"1 Week"},
    {TxtMaxTime,"Maximum"},    ////  May 10, 2004    REF: CRR 15753  Deepa M.D 
    {TxtPin2Code,"Pin2 Code"},
    {TxtEnablePIN,"Activate PIN"},
    {TxtRequesting,"Requesting"},
    {TxtNotAllowed,"Not Allowed"},
    {TxtEmergency,"Emergency ?"},
	{TxtChinese,"Chinese"},
    {TxtPassword,"Password"},
    {TxtConfirmPassword,"Confirm Password"},
    {TxtNewPassword,"New Password"},
    {TxtNoMessages,"List Empty"},
    {TxtTwoAttemptsLeft, "2 tries left" },
    {TxtOneAttemptLeft, "1 try left"},
    {Txtblacklisted, "Num.blacklisted"},
    {TxtErrUnknown, "Unknown Error"},
    {TxtSSErr,"SS error"},
    {TxtUnknownSub,"Unknown subscr"},
    {TxtIllSub,"Illegal subscr"},
    {TxtIllEqip,"Illegal device"},
    {TxtIllOp, "Illegal Op"},
    {TxtSubVil,"Subs Violation"},
    {TxtInComp,"Incompatible"},
    {TxtFacNoSup,"Facility unsup"},
    {TxtAbsSub,"Subs absent"},
    {TxtSysFail,"System failure"},
    {TxtMissDta,"Data missing"},
    {TxtUnexDta,"Unexp. data"},
    {TxtPwdErr,"Input Error"},
    {TxtUnkAlph,"Unknown letter"},
    {TxtMaxMpty,"Max part. conf"},
    {TxtResNoAvai,"Res not avail"},
    {TxtUnRec,"Unrecognized"},
    {TxtMisTyp,"Mistyped"},
    {TxtBadStruct,"Bad structure"},
    {TxtDupInv,"Duplicated use"},
    {TxtResLim,"No Resource"},
    {TxtIniRel,"Initial Rel."},
    {TxtFatRes,"Fatal Result"},
    {TxtEntAnw,"Answer:"},
    {TxtSsMsg,"SS Message"},
    {TxtOld,"Old"},
    {TxtInsertnote, "Insert Note"},
   	{TxtMelodygenerator, "Melodygenerator"},
   	{TxtOctaveHigher, "Octave higher"},
   	{TxtOctaveLower, "Octave lower"},
    {TxtLoadMelody, "Load melody"},
    {TxtEasyText, "eZiText"},
   	{TxtDataCount, "Data Count"},
   	{TxtQoS, "QoS"},
   	{TxtTypeofConnection, "Connection"},
   	{TxtTotalCount, "Total Count"},
   	{TxtResetCount, "Reset Count"},
   	{TxtDataCounter, "Data Counter"},
   	{TxtV42bis, "v.42bis"},
   	{TxtNormal, "Normal"},
   	{TxtCSD, "CSD"},
   	{TxtClassB, "Class B"},
   	{TxtClassC, "Class C"},
    {TxtIPAddress, "IP Address"},
/* SPR#2324 - SH - Removed some strings */
    {TxtIPAddress2, "IP Address 2"},
    {TxtPort1, "Port"},
    {TxtPort2, "Port 2"},
    {TxtAPN, "APN"},
    {TxtConnectionType, "Session Type"}, /* SPR#2324 - SH - Better description */
//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
    {TxtNameServer1, "Primary DNS"},
    {TxtNameServer2, "Secondary DNS"},
    {TxtPPGAuthentication, "Authentication"},
    {TxtWirelessProfiledHTTP, "Proxy"},
#endif
    {TxtGPRS, "GPRS"},
    {TxtOutgoingData, "Outgoing Data"},
    {TxtGame, "Game"},
    { TxtHomepage, "Homepage" },
    { TxtBookmarks, "Bookmarks" },
    { TxtGoTo, "Go to URL" },
    { TxtHistory, "History" },
   	{ TxtSetup, "Settings" },
   	{ TxtNewBookmark, "New Bookmark" },
   	{ TxtWWWdot, "http://www." },
   	{ TxtWAPdot, "http://wap." },
   	{ TxtSMS, "SMS" },
   	{ TxtEmail, "Email" },
   	{ TxtClear, "Clear" },
   	{ TxtBookmark, "Add Bookmark" },
/* SPR#2324 - SH - Removed some strings */
   	{ TxtSaveHistory, "Save History" },
   	{ TxtEditName, "Edit Name" },
   	{ TxtEditURL, "Edit URL" },
/* SPR#2324 - SH - Removed some strings */
   	{ TxtResponseTimer, "Response timer" },
   	{ TxtDialupNumber, "Dialup Number" },
   	{ TxtTemporary, "Temporary" },
   	{ TxtContinuous, "Continuous" },
/* SPR#2324 - SH - Removed some strings */
    { TxtWAP, "WAP" },
    { TxtExit, "Exit" },
    { TxtReload, "Reload" },
    { TxtStored, "Stored"},
    { TxtOther, "Other"},
    { TxtConnecting, "Connecting..." },
    { TxtDownloading, "Loading..." },
    { TxtUpdating, "Updating..." },
    { TxtEnterURL, "Enter URL:" },
    { TxtProfileName, "Profile Name: " },
    { TxtISPUsername, "Username" },
    { TxtISPPassword, "Password" },
    { TxtGoToLink, "Go to Link" },
    { TxtSelect, "Select" },
    { TxtNotConnected, "Not connected" },
    { TxtScaleImages, "Image scaling"},
    { TxtLoggingIn, "Logging in to" },
	{ TxtPageNotFound, "Page not found"},	/* SPR#1983 - SH - Modified WAP error messages */
	{ TxtInvalidURL, "Invalid URL"},
	{ TxtNoResponse, "No Response"},
	{ TxtAccessDenied, "Access Denied"},
	{ TxtErrorInPage, "Error in Page"},
	{ TxtScriptError, "Script Error"},
	{ TxtServerError,  "Server Error"},
	{ TxtGatewayError, "Gateway Error"},
	{ TxtUnexpectedErr, "Unexpected Err."},
	{ TxtCharacterSet, "Character Set"},
	{ TxtWBXMLError, "WBXML Error"},
	{ TxtFatalError, "Fatal Error"},
	{ TxtGatewayBusy, "Gateway Busy"},
	{ TxtSecurityFail, "Secur. Fail"},
	{ TxtErrorOutOfMem, "Out of memory"},
	{ TxtErrorMemLow, "Memory low"},
	{ TxtError, "Error"},
	{ TxtErrorHTTP, "HTTP"},
	{ TxtErrorWSP, "WSP"},
	{ TxtErrorWAE, "WAE"},
	{ TxtErrorWSPCL, "WSPCL"},
	{ TxtErrorWSPCM, "WSPCM"},
	{ TxtErrorWTP, "WTP"},
	{ TxtErrorWTLS, "WTLS"},
	{ TxtErrorWDP, "WDP"},
	{ TxtErrorUDCP, "UDCP"},
	{ TxtNotSupported, "Not Supported"},
    { TxtSelectLine, "Select Line"},
    { TxtLine1, "Line 1"},
    { TxtLine2, "Line 2"},
    {TxtInfoNumbers, "Info Numbers"},
	{ TxtAttach, "Attach"},
    { TxtDetach, "Detach"},
    { TxtDetaching, "Detaching"},
    { TxtDefineContext, "Define Ctxt" },
   	{ TxtActivateContext, "Activate Ctxt"},
    { TxtDeactivateContext, "Deactiv. Ctxt"},
    // 	Aug 29, 2005    REF: SPR 31712 xdeepadh
    { TxtGPRSPreferred, "GPRS Prefer"}, // SH 25/01/02
    { TxtCSDPreferred, "CSD Prefer"}, // SH 25/01/02
    { TxtNotAttached, "Not Attached"}, // SH 28/01/02
    { TxtAlreadyAttached, "Already"}, // SH 8/2/02 MZ cq10952
    { TxtSelected, "Selected"}, /*ap 06/02/02*/
    { TxtNetwork, "Network"}, /*ap 06/02/02*/
    { TxtForbidden, "Forbidden"}, /*ap 06/02/02*/
    { TxtActivateFDN, "Activate FDN"}, /*ap 06/02/02*/
    { TxtPlease, "Please"},	/*ap 06/02/02*/
	{ TxtTimedOut, "Timed Out"}, // SH 13/02/02
	{ TxtWillChangeOn, "Will Change On"}, // SH 13/02/02
    { TxtAttached, "Attached"}, // SH 14/02/02
    { TxtDetached, "Detached"}, // SH 14/02/02
    { TxtUnknown, "Unknown"}, //api 13/04/02
    { TxtCallTransfer, "Transfer Calls"},//MC
	{ TxtConnected, "Connected"}, // SH
    { TxtGSM_900,"GSM 900"},	// SH
    { TxtDCS_1800,"DCS 1800"},
	{ TxtPCS_1900,"PCS 1900"},
    { TxtE_GSM, "E GSM"},
	{ TxtGSM_850, "GSM 850"},
    { TxtCallTimeout, "Call timeout" },    
	{ TxtBand, "Band"},
    { TxtSatRefresh, "SAT refresh" }, 
    { TxtCallBlocked, "Call blocked" },
    { TxtSRR, "Status Report" }, // Marcus: Issue 1170: 30/09/2002
      //March 2, 2005    REF: CRR 11536 x0018858
     { TxtRP, "Reply Path" },// Added the string for reply path.
    { TxtDelivered, "Delivered" }, // Marcus: Issue 1170: 07/10/2002
    { TxtTempError, "Temp. Error" }, // Marcus: Issue 1170: 07/10/2002
	{ TxtUnknownCode,"Code Unknown"},/*MC SPR 1111*/
    { TxtPhoneSettings, "Phone Settings" }, //SPR#1113 - DS - Added text id for renamed main menu item "Phone Settings"
    { TxtNetworkServices, "Netw. Services" }, //SPR#1113 - DS - Added text id for new main menu item "Netw. Services"
    { TxtCallDeflection, "Call Deflect." },  //SPR#1113 - DS - Added text id for new menu item "Call Deflection" (CD)
    { TxtCallBack,  "Call Back" }, //SPR#1113 - DS - Added text id for new menu item "Call Back" (CCBS)
    { TxtMobileInfo,  "Mobile Info"},   /*MC, SPR1209*/
  	{ TxtServingCell,  "Serving Cell"}, /*MC, SPR1209*/

#ifdef TI_PS_FF_EM 
/*EngMode */
    { TxtServingCellRF, "Serving cell RF"},
    { TxtServingChannel, "Serving Channel"},
    { TxtAmrInfo, "AMR Information"},
    { TxtSNDCPInfo, "SNDCP Information"},
    { TxtUserLevelInfo, "User Level Information"},
    { TxtLLCTracing, "LLC Tracing info"},
    { TxtRlcMacInfo, "RLC/MAC Information"},
    { TxtGMMInfo, "GMM Information"},
    { TxtCallStatus, "Call status"},
    { TxtLayerMessages, "Layer Messages"},
    { TxtWCDMALayerMessages, "WCDMA Layer messages"},
    { Txt3GInfo, "Additional 3G information"},
    { TxtPacketDataParameter, "Packet data parameters"},
#endif

 	{ TxtNeighbourCell, "Neighbour Cell"},   /*MC, SPR1209*/
 	{ TxtLocation, "Location"},    /*MC, SPR1209*/
	{ TxtCiphHopDTX, "Ciph Hop DTX"}, /*MC, SPR1209*/
	{ TxtConcat, "Concatenate" },
    { TxtColourMenu,		"Colours"},	
    { TxtColourBlack,		"Black"},	
	{ TxtColourBlue,		"Blue"},
	{ TxtColourCyan,		"Cyan"},
	{ TxtColourGreen, 		"Green"}, 
	{ TxtColourMagenta, 	"Magenta"}, 
	{ TxtColourYellow,		"Yellow"},
	{ TxtColourRed, 		"Red"}, 
	{ TxtColourWhite,		"White"},	
	{ TxtColourPink,		"Pink"},
	{ TxtColourOrange,		"Orange"},
	{ TxtColourLightBlue,	"LightBlue"},
	{ TxtColourLightGreen, 	"LightGreen"}, 
	{ TxtColourPaleYellow, 	"Pale Yellow"},
    { TxtColourTransparent,	"Transparent"},	
	{ TxtIdleScreenBackground,		"Idle Screen Bgd"},
	{ TxtIdleScreenSoftKey,			"Idle Screen Softkey"},
	{ TxtIdleScreenText,			"Idle Screen Text"},
	{ TxtIdleScreenNetworkName,  	"Idle Screen Network"},
	{ TxtGeneralBackground ,  		"General Bgd"},
	{ TxtMenuSoftKeyForeground , 	"Menu Softkey Fgd"},
	{ TxtMenuSoftKeyBackground ,  	"Menu SoftKey Bgd"},
	{ TxtSubMenuHeadingText ,  		"Sub-Menu Heading Text"},
	{ TxtSubMenuHeadingBackground,	"Sub-Menu Heading Bgd"},
	{ TxtSubMenuTextForeground ,  	"Sub-Menu Text Fgd"},
	{ TxtSubMenuTextBackground,  	"Sub-Menu Text Bgd"},
	{ TxtSubMenuSelectionBar,  		"Sub-Menu Selection Bar"},
	{ TxtPopUpTextForeground,  		"Pop-Up text Fgd"},
	{ TxtPopUpTextBackground,		"Pop-Up text Bgd"},
    { TxtColour,					"Colour" },
    { TxtLoser,						"You Lost" },
    { TxtPlayAgain,					"Play Again?" },
    { TxtWinner,					"You Win" },		
	{ TxtSelectBook, 				"Select Book" },	/* SPR1112 - SH */
    { TxtSIM, 						"SIM" },
    { TxtMove, 						"Move" },
    { TxtCopy, 						"Copy" },
    { TxtThisEntry, 				"This Entry"},
    { TxtBackground,				"Background"},	/* for selecting backgrounds*/
    { TxtProviderIcon,				"Network Icon"}, /*AP - 29-01-03 - Add to language table */
    { TxtIdleScreen,				"Idle screen"},	
	{ TxtMenuScreen,				"Main menu screen"},	
	{ TxtBgdSquares,				"Squares"},
	{ TxtBgdO2,						"O2"},
	{ TxtBgdTree,					"Tree"},
    { TxtBgdTILogo,					"TI Logo"},
    { TxtBgdSunset,					"Sunset"},
    { TxtBgdEdinburgh1,				"Edinburgh 1"},
    { TxtBgdEdinburgh2,				"Edinburgh 2"},
    { TxtBgdEdinburgh3,				"Edinburgh 3"},
    { TxtBgdEdinburgh4,				"Edinburgh 4"},
    { TxtDeflectCall,"Deflect Call:"}, //SPR 1392 calldeflection
    { TxtDeflectCallTo,"Deflect To:"}, //SPR 1392  calldeflection
    { TxtDeflect,"Deflect"}, //SPR 1392  calldeflection
    { TxtDeflectingCallTo,"Deflecting To:"}, //SPR 1392  calldeflection
    { TxtCallBackQuery,  "Call Back?" }, // Marcus: CCBS: 15/11/2002
    { TxtRejected,  "Rejected" },       // Marcus: CCBS: 15/11/2002
	{ TxtActiveCall,			"Active Call" },
	{ TxtHeldCall,				"Held Call" },
	{ TxtActiveTTYCall,			"Active TTY Call" },		/* SPR#1352 - SH - TTY */
	{ TxtHeldTTYCall,			"Held TTY Call" },
    { TxtTTY,				"TTY" },
#ifdef NEPTUNE_BOARD    /* Mar 27, 2006    REF:ER OMAPS00071798 */
    {TxtAudioMode, "Audio mode"},
    {TxtAudioModeText, "Text mode"},
    {TxtAudioModeVco, "VCO mode"},
    {TxtAudioModeUnknown, "Unknown"},
#endif    
    { TxtBootTime, "Boot Time Measurement"}, /*OMAPS0091029 x0039928(sumanth)*/
/*x0039928 OMAPS00097714 - HCO/VCO implementation - added strings*/
#ifdef FF_TTY_HCO_VCO
    { TxtTTYAll,			"TTY All" },
    { TxtTTYVco,			"TTY Vco" },
    { TxtTTYHco,			"TTY Hco" },
#endif    
    { TxtOnNextCall,		"On Next Call" },
    { TxtAlwaysOn,			"Always On" },
    { TxtTTYCalling,		"TTY Calling..." },
    { TxtIncomingTTYCall,	"Inc. TTY Call" },
    { TxtProvisioned, "Provisioned"}, // Marcus: Issue 1652: 03/02/2003
    { TxtExecuted, "Executed"}, // Marcus: Issue 1652: 03/02/2003
    { TxtCommand, "Command"}, // NDH : SPR#1869 : 08/04/2003
    { TxtCompleted, "Completed"}, // NDH : SPR#1869 : 08/04/2003
    { TxtMultiparty, "Conference"}, // AP: Issue 1749: 06/03/03
    	{ TxtAttaching,			"Attaching"},	/* SPR#1983 - SH - Various WAP texts */
	{ TxtConnection,		"Connection"},
    	{ TxtUsing, 			"Using"},
    { TxtConnectionSpeed,	"Connect. Speed"},
    { TxtAnalogue,		"Analogue"},
    { TxtISDN9600,		"ISDN 9600"},
    { TxtISDN14400,		"ISDN 14400"},
    { TxtEndWap,		"End WAP"},
	{ TxtUntitled,		"Untitled"},
	{ TxtChooseReplacement1, "Choose"}, /* SPR#2354 */
	{ TxtChooseReplacement2, "Replacement"}, /* SPR#2354 */
	{ TxtDialup, "Dialup" }, /* SPR#2324 - SH - Added */
   	{ TxtAdvanced, "Advanced" },
   	{ TxtGPRSDialupGPRS, "(GPRS)/Dialup" },
   	{ TxtGPRSDialupDialup, "GPRS/(Dialup)" },
   	{ TxtProfiles, "Profiles" },
   	{ TxtNewProfile, "New Profile" },
   	{ TxtResetProfiles, "Reset Profiles" },
	{ TxtPasswords, "Passwords"}, 	//SPR16107	
	{ TxtMismatch, "donot match!"}, 	//SPR16107	
	{ TxtDataCounterReset, "Reset Counter?"}, /* SPR#2346 - SH */
	{ TxtCallDate, "Date : "}, /* API - 05/09/03 - SPR2357 - Added */
	{ TxtCallTime, "Time : "}, /* API - 05/09/03 - SPR2357 - Added */   	   	
	{ TxtInvalidMessage, "Invalid Message"},/*MC SPR 2530*/
	{ TxtIncompleteMessage, "Incomplete Message"},/*MC SPR 2530*/
        //yanbin add MMS string
          //TISHMMS Project
        { TxtDownload,					"Download"},
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)  //added by ellen
    { TxtShowMessages,				"Show Push Messages"},
    { TxtPushMessages,				"Push Messages"},
#endif
#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS)//MMI-SPR 49811 - x0035544 07 nov 2005
        {TxtMMS,            "MMS"}, 
        {TxtEMS,            "EMS"},         
        {TxtMMSCreate,"Create MMS"},
        {TxtEMSCreate,"Create EMS"},        
        {TxtMMSInbox,"Inbox"},
        {TxtMMSUnsent,"Unsent"},
        {TxtMMSSent,"Sent"}, //CRR: 25291 - xrashmic 14 Oct 2004
        {TxtMMSSettings,"Settings"},
        {TxtRetrieveCondition,"Rertieval Type"},        
        {TxtMMSInsertPicture,"Ins Picture"},
        {TxtMMSInsertSound,"Ins Sound"},
        {TxtMMSInsertText,"Ins Text"},
        {TxtMMSInsertSlide,"Ins Slide"},
        {TxtMMSDelete,"Delete"},
        {TxtMMSPreview,"Preview"},
        {TxtMMSDeleteSlide,"Del Slide"},
        {TxtMMSDeletePicture,"Del Picture"},
        {TxtMMSDeleteSound,"Del Sound"},
        {TxtMMSDeleteText,"Del Text"},
        {TxtMMSInsert, "Insert"},
//xrashmic 19 Aug, 2004 Bug: 2, 3, 36 and 42
        {TxtMMSSendSucess,"Sent"},
        {TxtMMSSendFail, "Send Failed"},
        {TxtMMSSendng, "Sending"},
        {TxtMMSDownloading,"Receiving"},
        {TxtNewMMS,"New MMS"},
        {TxtMMSRcvFail,"Receive Failed"},
        {TxtMMSDots," MMS ..."},
        {TxtExtract, "Extract" },
        {TxtMMSNotification, "MMS Notification" },//x0012849 May 11 2005 MMI-SPR-29887
        {TxtImmediate, "Immediate" },//x0012849 May 11 2005 MMI-SPR-29887
        {TxtDeferred, "Deferred" },//x0012849 May 11 2005 MMI-SPR-29887    
        {TxtMMSRetrieving, "MMS Retrieving" },//x0012849 May 11 2005 MMI-SPR-29887
        {TxtMMSRetrieved, "MMS Retrieved" },//x0012849 May 11 2005 MMI-SPR-29887           
        {TxtEMSNew," New EMS"}, //xrashmic 26 Aug, 2004 MMI-SPR-23931
        {TxtType, "Type"},
        {TxtObject,"Object"},//xrashmic 29 Nov, 2004 MMI-SPR-26161
        {TxtLarge," Too Large"},//xrashmic 29 Nov, 2004 MMI-SPR-26161
        //xrashmic 7 Dec, 2004 MMI-SPR-23965
        {TxtMMSInsertPrePicture,"Ins Pre Picture"},     
        {TxtMMSInsertPreSound, "Ins Pre Sound"},
        {TxtAllObjects, "All Objects"},
        {TxtExtracted, "Extracted"},
        {TxtExtracting,"Extracting"},
        {TxtNoObjects, "No Objects"},
        {TxtExtractable, "Extractable"},
 #endif  //MMI-SPR 49811 - x0035544 07 nov 2005    
	{ TxtLockAls, "Lock ALS"}, /* NDH : CQ16317 */
	{ TxtUnlockAls, "Unlock ALS"}, /* NDH : CQ16317 */
	{ TxtAlsLocked, "ALS Locked"}, /* NDH : CQ16317 */
	{ TxtAlsUnlocked, "ALS Unlocked"}, /* NDH : CQ16317 */
	{ TxtLimService, "Limited Service"}, /* SPR12653 */
	
//   Apr 06, 2005	REF: ENH 30011 xdeepadh
//Strings for Camera, Mp3 and midi test application are 
//under the respective flags.
//Nov 14, 2005    REF: OMAPS00044445 xdeepadh
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */
// #if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
	{ TxtPlayerMonoChannel, "Mono"}, /* SPR12653 */
	{ TxtPlayerStereoChannel, "Stereo"}, /* SPR12653 */
	{ TxtPlayerSetChannels, "Channels settings"}, /* SPR12653 */
	{ TxtPlayerPlay, "Play file"}, /* SPR12653 */
	{ TxtPlayerTest, "Audio Player"}, /* SPR12653 */
	{ TxtPlayerFileSelected, "File Selected"}, /* SPR12653 */
	{ TxtPlayerSelectFile, "Select a File"}, /* SPR12653 */
	{ TxtPlayerPause, "Pause"}, /* SPR12653 */
	{ TxtPlayerResume, "Resume"}, /* SPR12653 */
	{ TxtPlayerStop, "Stop"}, /* SPR12653 */
	{ TxtPlayerPlaying, "Playing"}, /* SPR12653 */
	{ TxtPlayerForward, "Forward"},  /* OMAPS00070659  x0039928  */
	{ TxtPlayerRewind, "Rewind"},
	{ TxtPlayerForwarding, "Forwarding"},
	{ TxtPlayerRewinding, "Rewinding"},  /* OMAPS00070659  x0039928  */
//	Jul 18, 2005    REF: SPR 31695   xdeepadh
	{ TxtPlayerFileNotLoaded, "No Files loaded"},
	{ TxtPlayerOptionNotImplemented, "Option not yet implement"}, 
// #endif //FF_MMI_TEST_MP3
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */

//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
	{ TxtCameraTest, "Camera Application"}, /* NDH : CQ16317 */
	{ TxtViewFinder, "View Finder"}, /* NDH : CQ16317 */	
	{ TxtSnapshot, "Take Snapshot"}, /* NDH : CQ16317 */		
	{ TxtSaveSnapshot, "Save Snapshot"}, /* NDH : CQ16317 */	
	{ TxtSnapshotSaved, "Snapshot Saved"}, /* NDH : CQ16317 */	
	{ TxtSoftSnap, "Snapshot"}, /* NDH : CQ16317 */		
	{ TxtSoftSnapSave, "Save"}, /* NDH : CQ16317 */		
	{ TxtSnapSaving, "Saving"},
	{ TxtQuality, "Quality"},
	{ TxtEconomy, "Economy"},
	{ TxtStandard, "Standard"},
	{ TxtColor, "Color"},
	{ TxtSepia, "Sepia"},
	{ TxtBlackandWhite, "Black&White"},
	{ TxtNegative, "Negative"},
	{ TxtFilename, "Filename"},
	{ TxtDigiZoom, "Zoom"},
	{ TxtFrame, "Border Frame"},
	{ TxtFrame1, "Border Frame1"},
	{ TxtFrame2, "Border Frame2"},
	{ TxtNoFrame, "No frame"},	
	{ TxtShootingMode, "Shooting Mode"},
	{ TxtSingleshot, "Single Shot"},
	{ Txt2Shots, "2 Shots"},
	{ Txt4Shots, "4 Shots"},
	{ Txt8Shots, "8 Shots"},
#endif //FF_MMI_TEST_CAMERA
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Midi Test Application Strings were put under the flag FF_MMI_TEST_MIDI
#ifdef FF_MMI_TEST_MIDI
       { TxtMidiTest, "Midi Application"}, 
       { TxtMidiChangeConfig, "Change Config"}, 
       { TxtMidiPlay, "Play"},
       { TxtMidiPlayAllFiles, "Play all files"},
       { TxtMidiExit, "Stop playing"},
       { TxtChConfigFile, "Browse Midi Files"},
       { TxtChConfigVoiceLimit, "Voice Limit"},      
       { TxtChConfigLoop, "Loop On/Off"},  
       { TxtChConfigChannels, "Channels"}, 
       { TxtChConfigBankLocation, "Bank Location"},  
       { TxtMidiEnterVoiceNumber, "Enter number of voices"},
    	{ TxtMidiSelectFile,  "Select file to be played"},
	{ TxtMidiOptionNotImplemented, "Option not yet implement"}, 
       { TxtMidiLoopOn, "On"},       
       { TxtMidiLoopOff, "Off"},       
       { TxtMidiChannels, "Channels"},       
       { TxtMidiMonoChannel, "Mono"}, 
       { TxtMidiStereoChannel, "Stereo"},
       { TxtMidiBankLocation, "Bank"},//MMI-SPR 49811 - x0035544 07 nov 2005
	{ TxtMidiFastBankLocation, "Fast"},
	{ TxtMidiSlowBankLocation, "Slow"},
    	{ TxtMidiFileTitle, "Select File to be played"},
    	{ TxtMidiFileNotLoaded, "No Files loaded"},
    	{ TxtMidiFileSelected, "File Selected"},
       { TxtChConfigAudioOutput, "Audio Output"}, 
	{ TxtMidiSpeaker, "Speaker"},
	{ TxtMidiHeadset, "Headset"},
//x0pleela 30 Jan, 2007  DR: OMAPS00108892
//Adding the following strings to display error message
//when loop on is enabled while playing all files
	{ TxtLoopOn, "Loop ON"},
	{ TxtCantPlayAllFiles, "Cannot Play All Files"},
	{ TxtCantBeActivated, "Cannot be Activated"},
#endif//FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
	/*
	** Start of Bluetooth Related Strings
	*/
    	{TxtBluetooth, "Bluetooth"},
    	{TxtBluetoothEnable, "Enable"},
    	{TxtBluetoothDisable, "Disable"},
    	{TxtBtEnterDeviceName, "Enter Device Name:"},
    	{TxtBtEnterPassKey, "Enter Pass Key:"},
    	{TxtBtPassKey, "Pass Key"},
    	{TxtBtTimeout, "Timeout"},
    	{TxtBtPairing, "Pairing"},
    	{TxtBtPaired, "Paired"},
    	{TxtBtAuthorize, "Authorize"},
    	{TxtBtUnAuthorize, "Un-Authorize"},
	{TxtBtInfo, "Info"},
	{TxtBtYesOnce, "Yes (Once)"},
	{TxtBtYesAlways, "Yes (Always)"},
	{TxtBtPairDevice, "Pair Device"},
	{TxtBtChangePassKey, "Change PassKey"},
	{TxtBtShowServices, "Show Services"},
	{TxtBtAddToKnown,	 "Add To Known"},
	{TxtBtRemoveDevice, "Remove Device"},
	{TxtBtDeviceName, "Device Name"},
	{TxtBtSet, "Set"},
	{TxtBtServices, "Services"},
	{TxtBtNoServices, "No Services"},
	{TxtBtSupported, "Supported"},
	{TxtBtSerialPort, "Serial Port"},
	{TxtBtDialUpNetworking, "Dial Up Networking"},
	{TxtBtHeadSet, "Head Set"},
	{TxtBtHandsFree, "Hands Free"},
	{TxtBtObjectPush, "Object Push"},
	{TxtBtFileTransfer, "File Transfer"},
	{TxtBtLanAccess, "Lan Access"},
	{TxtBtCordlessTelephony, "Cordless Telephony"},
	{TxtBtIntercom, "Intercom"},
	{TxtBtUnknownService, "Unknown Service"},
	{TxtBtRequiresAuthorizationForService, "%s requires Authorization for Service : %s"},
	{TxtBtDevices, "Devices"},
	{TxtBtDevice, "Device,"},
	{TxtBtHidden, "Hidden"},
	{TxtBtTimed, "Timed"},
	{TxtBtKnownDevices, "Known Devices"},
	{TxtBtDiscoverable, "Discoverable"},
	{TxtBtNoDevices, "No Devices"},
	{TxtBtFound, "Found"},
    	{TxtBtSetLocalName, "Set Local Name"},
	{TxtBtClearStoredDevices, "Clear all Stored Devices"},
    	/*
    	** End of Bluetooth Related Strings
    	*/
#endif
	{ TxtFDNName, "FDN Numbers" },// MMI SPR 18555
	{ TxtAllDivert, "All Divert"}, 
	{ TxtNoReachDivert, "Divert when not reachable"}, 
	{ TxtBusyDivert, "Divert if Busy"}, 
	{ TxtNoAnsDivert, "Divert when no answer"}, 
	{ TxtCondDivert, "Divert for Cond. Forward"}, 
	{ TxtAllCallsDivert, "Divert for All calls"}, 
	{ TxtInvalidInput, "Invalid input"}, //SPR21547
	{ TxtFDNListFull, "FDN List Full"}, //SPR 31710 x0021308:RamG

	
	//Nov 29, 2004    REF: CRR 25051 xkundadu
    	//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
	//Fix: Added these strings to display the various speaker volume levels.
	{TxtLevel1 ," Level 1"},  
	{TxtLevel2 ," Level 2"},
	{TxtLevel3 ," Level 3"},
	{TxtLevel4 ," Level 4"},
	{TxtLevel5 ," Level 5"},
        {TxtImage,"Image"}, //xrashmic 28 Jan, 2005 MMI-SPR-28166
       { TxtSmsWaiting, "Message waiting"}, //x0018858 24 Mar, 2005 MMI-FIX-11321        

/* Power management Menu*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        { Txt10Second, "10 Sec" },
        { Txt15Second, "15 Sec" },
        { Txt20Second, "20 Sec" },
	 { TxtSetParam1, "Set Param"},      
#endif
#endif
#ifdef FF_MMI_MULTIMEDIA

	/* String required for Multimedia Menu. */
	{TxtMultimediaApp, "Multimedia Test Application"},    
	{TxtConfigurationParameters, "Configuration Parameters"},     	
	{TxtAudioVideoPlayback, "Audio/Video Playback"},     	
	{TxtMidiPlayback, "Midi Playback"},     	
	{TxtImageViewer, "Image Viewer"},     	
	{TxtCameraCapture, "Camera Capture"},    	
	{TxtVideoRecording, "Video Recording"},
	{TxtAudioRecording, "Audio Recording"},
	{TxtPlayRecording, "Play Recording"},
      	{TxtFileListType, "FileLists"},        
	{TxtNoFileAvail, "No Files"}, 
	/* Configuration Parameter. */
	{TxtOutputScreenSize, "Output Screen Size"},      			
	{TxtAudioEncodingFormat, "Audio Encoding Format"},       			
	{TxtVideoEncodingFormat, "Video Encoding Format"},      			
	{TxtVideoCaptureSize, "Video Capture Size"},       			
	{TxtVideoFrameRateSelection, "Video Frame Rate Selection"},       			
	{TxtAudioBitRate, "Audio Bitrate"},       			
	{TxtVideoBitRate, "Video Bitrate"},    
	{TxtMidiPlayerSelect, "Midi Player Enabled"},   
	/* Screen mode */
	{TxtHalfScreenMode, "Half-Screen Mode" },	
	{TxtFullScreenMode, "Full-Screen Mode" },		
	/* Audio Format */
	{TxtAAC, "AAC" },		
	{TxtAMR, "AMR-NB" },		
	{TxtPCM, "PCM" },			
	{TxtMP3, "MP3" },		
	/* Video Format */
	{TxtMpeg4, "MPEG4" },		
	{TxtH263, "H263" },		
	/* Screen Size */
	{TxtQcif, "QCIF" },		
	{TxtCif, "SQCIF" },		
	/* Frame Rate */
	{Txt5fps, "5 FPS"},    			
	{Txt10fps, "10 FPS"},			
	{Txt15fps, "15 FPS"},  			
	{Txt20fps, "20 FPS"}, 
	{Txt25fps, "25 FPS"},     			
	{Txt30fps, "30 FPS"}, 
	/* Bit Rate*/
	{TxtAACBitRate, "AAC BitRate" },		
	{AMRNBBitRate, "AMR-NB BitRate" },		
	/* AAC Bit Rate */
	{Txt48Kbps, "48 KBPS"},     			
	{Txt66p15Kbps, "66.15 KBPS"}, 			
	{Txt72Kbps, "72 KBPS"}, 				
	{Txt96pKbps, " 96  KBPS"},  			
	{Txt132p3Kbps, "32.3 KBPS"}, 
	{Txt144Kbps, "144 KBPS"},  			
	{Txt192Kbps, "192 KBPS"}, 
	{Txt264p6Kbps, "264.6 KBPS"},   			
	{Txt288Kbps, "288 KBPS"}, 
	/* AMR Bit Rate */
       {Txt1p80Kbps, "1.80 KBPS"},
	{Txt4p75Kbps, "4.75 KBPS"},     			
	{Txt5p15Kbps, "5.15 KBPS"}, 			
	{Txt5p9Kbps, "5.9 KBPS"}, 			
	{Txt6p7Kbps, "6.7 KBPS"}, 
	{Txt7p4Kbps, "7.4 KBPS"},    			
	{Txt7p95Kbps, "7.95 KBPS"}, 
	{Txt10p2Kbps, "10.2 KBPS"},     			
	{Txt12p2Kbps, "12.2 KBPS"}, 
	/* Video Bit Rate */
	{Txt64Kbps, "64 KBPS"}, 
	{Txt128Kbps, "128 KBPS"},     			
	{Txt384Kbps, "384 KBPS"}, 
	/* Softkey for Audio / Video display. */
	{TxtPauseKey, "Pause"},
	{TxtResumeKey, "Resume"},
	{TxtEncodeKey, "Encode"}, 
	{TxtSoftIVT, "IVT"},
	{TxtPictureDisp, "Picture Display"},	
	{TxtSetParam, "Set Param"},      
	/* Audio Path Configuration Parameter */
	{TxtAudioPath, "Audio Path Configuration"},  
	{TxtHeadset, "Head Set"},				  
	{TxtHeadphone, "Stereo Head Phone"},	  
	/* Audio / Video Recording. */
	{TxtSoftStart, "Start"}, 						
	{TxtAudvidRecord, "Audio / Video Recording"},  
	{TxtQQVga, "QQVGA"},			
	{TxtEncdTo, "Encode To"},         
	{TxtJPEGFile, "JPEG File"},          
	/* For FTP Application */
	{ TxtFTPApp, "FTP" },
	{ TxtPut, "Put" },
	{ TxtGet, "Get" },	
	/* Configuration Parameters - Volume and Preferred Storage. */
       { TxtVolumeConfig, "Volume Configuration"},
       { TxtPreferredStorage, "Preferred Storage"},
       { TxtAudioDecodeConfig, "Audio Decode Configuration"},       
       { TxtAudioEncodeConfig, "Audio Encode Configuration"},
       { TxtVideoEncodeConfig, "Video Encode Configuration"},       
       { TxtSpeaker, "Handset"},                      
       { TxtEnterPath, "Enter File Path:"},
       {TxtInvalidPath, "Invalid"},
	{ TxtVMPlayback, "Voice Memo Playback" },
	{ TxtVMRecord, "Voice Memo Recording" },
	{ TxtVMConfigParam, "Configuration Parameters" },
	{ TxtStartRecord, "Start Voice Memo Recording" },
	{ TxtPlayingfile, "Playing Selected Fie " },
	{ TxtPauseFile, "Pause Selected File" },
       { TxtRecordVoice, "Recording Voice" },
      	{ TxtSoftMore, "More" },
      	{ TxtUseCase, "Use Cases" },
	{ TxtRepeateForEver, "Repeate For Ever" },
       { TxtPlayonce, "Play Once" },
	{ TxtCamera , "Camera"},
	{ TxtCapture, "Capture" },
	{ TxtCrop, "Crop"},
	{ TxtRescale, "Rescale"},
	{ TxtRotate, "Rotate"},
	{TxtDummyStr, "Dummy String"}, 
	{TxtDummyStr1, "Dummy String1"}, 
	/*PCM Decode sampling Rate*/
       {Txt8kRate, "8k Sample Rate"}, 
       {Txt11kRate, "11k Sample Rate"}, 
       {Txt12kRate, "12k Sample Rate"},         /*OMAPS00081264 - 12k Menu addition */
       {Txt16kRate, "16k Sample Rate"}, 
       {Txt22kRate, "22k Sample Rate"}, 
       {Txt24kRate, "24k Sample Rate"},        /*OMAPS00081264 - 12k Menu addition */
       {Txt32kRate, "32k Sample Rate"}, 
       {Txt44kRate, "44k Sample Rate"}, 
       {Txt48kRate, "48k Sample Rate"}, 
       {TxtPcmRate, "PCM Sample Rate"}, 
       {TxtPcmChannelMode, "PCM Channel Mode"}, 
        /* Image Quality Setting */
       {TxtImgCaptureQuality, "Image Capture Quality"}, 
       {Txt0Set, "0"}, 
       {Txt10Set, "10"}, 
       {Txt20Set, "20"}, 
       {Txt30Set, "30"}, 
       {Txt40Set, "40"}, 
       {Txt50Set, "50"}, 
       {Txt60Set, "60"}, 
       {Txt70Set, "70"}, 
       {Txt80Set, "80"}, 
       {Txt90Set, "90"}, 
       {Txt100Set, "100"}, 
       {TxtStreoMode, "Stereo Mode"}, 
       {TxtMonoMode, "Mono Mode"}, 
       {TxtVideoPlayOption, "Video Playback Option"},
       {TxtAudioVideoPlay, "Audio & Video Play"},
       {TxtVideoPlay, "Video Play Only"},
       /* Video Recording Duration Specification */
       {TxtVideoRecDuration, "Video Recording Duration"},      
       {Txt10sec, "10 Sec Recording"},
       {Txt15sec, "15 Sec Recording"},
       {Txt20sec, "20 Sec Recording"},
       {TxtNonStop, "Non Stop Recording"},
  /* Video Only Option - x0043641*/
       {TxtVideoRecordOption, "Video Record Option"},
       {TxtAudioVideoRecord, "Video & Audio Recording"},
       {TxtVideoRecord, "Video Recording Only"},
#endif
// 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005       
#ifdef FF_MMI_AUDIO_PROFILE
	{TxtLoudspeaker, "Loudspeaker"},
	{TxtHandheld, "Handheld"},
	{TxtCarkit, "Carkit"},
	{TxtHeadset,"Headset"},
	{TxtInserted, "inserted"},
	{TxtRemoved, "removed"},
#endif	
	//Aug 22, 2005    REF: ENH 31154 xdeepadh

#ifdef FF_MMI_FILE_VIEWER
	{ TxtFileNotLoaded,"No Files loaded"},
	{ TxtFileViewer, "FileViewer"}, 
	{ TxtRename, "Rename"}, 
	{TxtDeleteConfirm,"Delete?"},
	{TxtFileTooBig,"File Too Big"},
	{TxtFileExists,"File exists"},//Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh .
	{TxtWrongFilename,"Wrong Name"}, //Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,"Wrong extn"},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
#endif //FF_MMI_FILE_VIEWER
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        {TxtPowerMgnt,"Power Management"},
#endif
#endif
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
	{ TxtRotate, "Rotate"}, 
	{ TxtRotate90Degree, "90 Degree"}, 
	{ TxtRotate180Degree, "180 Degree"}, 
	{ TxtRotate270Degree, "270 Degree"}, 
	{ TxtZoom, "Zoom"}, 
	{ TxtZoomIn, "Zoom In"}, 
	{ TxtZoomOut, "Zoom Out "}, 
	{ TxtZoomInMax, "Zoom In Max "}, 
	{ TxtZoomOutMax, "Zoom Out Max"}, 
	
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
{TxtUSB, "USB"},
	{TxtUSBEnumeration, "USB Enumeration"},
	 {  TxtUSBMode, "USB mode"},
 	 {  TxtContinue, "Continue ?"},	
	{ TxtUSBDisconnect ,"Disconnecting"},
	{TxtUSBMS,"USB MS"},
	{TxtUSBTrace,"USB Trace"},
	{TxtUSBFax,"USB Fax"}, 
{TxtPSShutdown,"PS Shutdown"},
{TxtNoPSShutdown,"No PS shutdown"},
	//xashmic 27 Sep 2006, OMAPS00096389 
	{TxtUSBFaxnTrace,"USB Fax and Trace"},
	{TxtUSBMSnTrace,"USB MS and Trace"},
	{TxtUSBFaxnMS,"USB Fax and MS"},
	{TxtUSBMSPorts,"USB Ports"},
	{TxtPS,"PS"},
	{TxtPopupMenu,"Pop up menu"},
#endif
    // x0021334 10 May, 2006 DR: OMAPS00075379
	{ TxtPending, "Pending"},

    // x0021334 02 June, 2006 DR: OMAPS00078005 
#ifdef FF_TIMEZONE
	{ TxtTimeUpdate, "New Time/Date received."},
	{ TxtTimeUpdateConfirm, "Want to update?"},		
	{ TxtNitzTimeUpdate, "Network Time Update"},
#endif
	//x0pleela 30 Aug, 2006 DR: OMAPS00091250
	{ TxtNoMEPD, "MEPD not present"},

//x0pleela 24 Aug, 2006  DR:OMAPS00083503
//Adding new strings to display if the characters are UCS2 in USSD transactions
#ifdef MMI_LITE
	{TxtUSSDMsgRxd, "USSD Message received" },
	{TxtFontNotSupported, "Font Not Supported"},
#endif
//x0pleela 19 Feb, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	{TxtPhoneLock, "Phone Lock"},
	{TxtPhLockEnable, "Lock Enable"},
	{TxtPhLockAutomatic, "Automatic"},
	{TxtPhLockAutoOn, "On"},
	{TxtPhLockAutoOff, "Off"},
	{TxtPhLockChangeCode,"Change Unlock Code"},
	{TxtEnterPhUnlockCode, "Enter Ph. Unlock Code"},
	{TxtEnterOldUnlockCode, "Enter Old Unlock Code"},
	{TxtEnterNewUnlockCode, "Enter New Unlock Code"},
	{TxtAlreadyEnabled,"Already enabled"},
	{TxtAlreadyDisabled, "Already disabled"},
	{TxtAutoLockDisable, "Auto Ph Lock Disable"},	
	{TxtAutoPhone, "Automatic Phone"},	
	{TxtSuccess, "Success"},	
	{TxtNotLocked, "Phone can't be locked"},
#endif /* FF_PHONE_LOCK */
/*OMAPS00098881 (Removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
	{TxtScreenUpdate,"Screen Update"},
#endif
//Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
#ifdef FF_MMI_CAMERA_APP
	{TxtNotEnough,"Not Enough"},
	{TxtCamMemory,"Memory"},
#endif
	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
	{TxtFwdIncomingTTYCall, "Fwd. Inc. TTY Call"}, /* String for Fwd Incoming TTY call */
	{TxtFwdIncomingCall, "Fwd. Inc. Call"}, /* String for Fwd Incoming  call */
#ifdef FF_MMI_FILEMANAGER 
	/* File Management related strings */
	{TxtFileMgmt, "File Management"},   
	{TxtFileList, "FileLists"},
	{TxtFFS, "NOR" },	
	{TxtNORMS, "NOR-MS" },	
	{TxtNAND, "NAND" },	
	{TxtTflash, "T-Flash" },	
	{TxtOpen,"Open"},
	{TxtFormat,"Format"},
	{TxtProperties,"Properties"},
	{TxtImageFiles, "Image"},
	{TxtAudioFiles,"Audio"},
	{TxtFile,"File"},
	{TxtDirectory,"Directory"},
	{TxtFreeSpace,"Free Space"},
	{TxtUsedSpace, "Used Space"},
	{TxtThumbnail, "Thumbnail"},
	{TxtExists, "Exists"},
	{TxtCreateDir, "Create Directory"},
	{TxtCreated, "Created"},
	{TxtNotEmpty, "Not Empty"},
	{TxtMemorystat,"Memory Status"},
	{TxtSourceDest, "Source and Destination"},
	{TxtSame, "Same"},
	{TxtAudNotAllowed,"Not Allowed"},
	{TxtAudAudioError,"Audio"},
	{TxtAudDenied,"Denied"},
	{TxtAudPlayerError,"Player"},
	{TxtAudFFSError," FFS"},
	{TxtAudMessagingError,"Messaging"},
	{TxtAudNotReady,"Not Ready"},
	{TxtAudInvalidParam,"Invalid Param"},
	{TxtAudInternalError,"Internal"},
	{TxtAudMemoryError,"Memory"},
	{TxtAudNotSupported,"Not Supported"},	
	{TxtCopying," Copying ..."},
	{ TxtFileNotLoaded,"No Files loaded"},
	{ TxtFileViewer, "FileViewer"}, 
	{ TxtRename, "Rename"}, 
	{TxtDeleteConfirm,"Delete?"},
	{TxtFileTooBig,"File Too Big"},
	{TxtFileExists,"File exists"},//Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh .
	{TxtWrongFilename,"Wrong Name"}, //Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,"Wrong extn"},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtStorageDevice,"Storage Device"},
	{TxtPBar,"Progress Bar On/Off"},
#endif
//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    {TxtTo,"To"},

/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
#ifdef FF_MMI_CAMERA_APP
    {TxtAutoSave,"Auto Save"},
    {TxtAutoSaveOn,"On"},
    {TxtAutoSaveOff,"Off"},
    {TxtSoftSave,"Save"},
    {TxtSoftCancel,"Cancel"},
#endif /* FF_MMI_CAMERA_APP */
//May 03 2007 ER: x0061088 (Prachi)
#if CAM_SENSOR == 1
    {TxtCamMenuResolution,"Change Resolution"},
    {TxtCamVga,"VGA"},
    {TxtCamQcif,"QCIF"},
    {TxtCamSxga,"SXGA"},
    {TxtCamNot,"Not"},
    {TxtCamSupported,"Supported"},
#endif
	 //06 July, 2007 DR: OMAPS00137334 x0062172 Syed
	{TxtConferenceFull, "Conference Full"}, /* String for Conference Full notification */
	{TxtEmobiixApps, "Emobiix Applications"}, 
    { 0, 0 }
};

#ifndef CHINESE_MMI 

/* Text table 2, autogenerated to provide German text associations.
*/

//DON'T TOUCH THE FOLLOWING LINE...
//Language = "German"

static const tPointerStructure LanguageIndex_2[LEN_LANGUAGE2+1] = {
    { TxtNull, "" },
#ifdef FF_CPHS
  { TxtEditLineName, "Edit Line Name"}, //x0pleela 14 June, 2006  DR: OMAPS00070657
#endif
//x0pleela 20 Mar, 2006   ER: OMAPS00067709
//Added the following strings used in menu items to support Voice buffering feature
#ifdef FF_PCM_VM_VB
{TxtStopFailed, "Stop Failed"},
{TxtAlready, "Already"},
{TxtPcmVoiceMemo, "PCM Voice Memo"},
{TxtVoiceBuffering, "Voice Buffering"},
{TxtEditNum, "Edit number"},
{TxtStartRec, "Start Recording"},
{TxtStopRec, "Stop Recording"},
#endif
#ifdef SIM_PERS    //x0035544 07-11-2005
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
{TxtBlkNetwork, "Blocked Network"},	//For new category "Blocked Network" 
{ TxtEnterPbLock,"Enter Blocked NW Code"}, //Editor string to display "Enter Blocked Network password"
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
{TxtBusy,"Busy"}, 						//Displays "Busy" if timer is enabled for wrong entry of password
{TxtMasterUnlock,"Master Key Unlock"}, 	//For menu option Master unlock
{TxtEnterMasterKey, "Enter Master Key"},	//Text for the editor to enter master unlock key
{TxtUnblockME, "Unblock ME"},			//For menu option Unblock ME
{TxtUnblockCode, "Enter Unblock Code"}, //x0pleela 16 May, 2006  DR: OMAPS00067919
    {TxtTimerFlag, "Unlock Timer Flag"},   //x0035544 07-11-2005
    {TxtETSIFlag, "ETSI Flag"},   //x0035544 07-11-2005
    {TxtAirtelFlag, "AirTel Ind. Flag"},  //x0035544 07-11-2005
    { TxtEnterOldPsLock,"Enter Old LockCode"},  //x0035544 07-11-2005
    { TxtEnterPsLockNew,"Enter New LockCode"},  //x0035544 07-11-2005
    { TxtEnterPsLockconf,"Confirm New LockCode"},   //x0035544 07-11-2005
    { TxtLockEnabled, "Lock Enabled"},  //x0035544 07-11-2005
    { TxtLockDisabled,"Lock Disabled"},  //x0035544 07-11-2005
    { TxtPersonalization, "Personalization"},  //x0035544 07-11-2005
    { TxtSupplementaryInfo, "Supplementary Info"},  //x0035544 07-11-2005
    { TxtPersonalize, "Personalize"},  //x0035544 07-11-2005
    { TxtUnlockCategory, "Unlock Category"},  //x0035544 07-11-2005
    { TxtLockCategory, "Lock Category"},  //x0035544 07-11-2005
    { TxtResetFC, "Reset Failure counter"},  //x0035544 07-11-2005
         { TxtGetFC, "Get Failure Counter"},  //x0035544 07-11-2005 
    { TxtMaxFC,"Max Failure Count"},  //x0035544 07-11-2005
    { TxtCorporate, "Corporate"},  //x0035544 07-11-2005
    { TxtServiceProvider, "Service Provider"}, //x0035544 07-11-2005
    { TxtNetworkSubset, "Network Subset"}, //x0035544 07-11-2005
    { TxtLockActivated,"Lock Activated"}, //x0035544 07-11-2005
    { TxtLockDeactivated,"Lock Deactivated"}, //x0035544 07-11-2005
    { TxtEnterNLock,"Enter Network Password"}, //x0035544 07-11-2005
    { TxtEnterNsLock,"Enter Netw. Subset Password"}, //x0035544 07-11-2005
    { TxtEnterSpLock,"Enter Serv. Provider Password"}, //x0035544 07-11-2005
    { TxtEnterCLock,"Enter Corporate Password"}, //x0035544 07-11-2005
    { TxtEnterPsLock,"Enter SIM Password"},  //x0035544 07-11-2005
     { TxtPsLockConfWrong,"Password does"},  //x0035544 07-11-2005
    { TxtPsLockConfwrong2,"not match"},  //x0035544 07-11-2005
    { TxtPsLockChanged,"Code changed"},  //x0035544 07-11-2005
    { TxtCounterReset,"Counter Reset"}, //x0035544 07-11-2005
     { TxtPassfailed,"Change failed"},  //x0035544 07-11-2005
     {TxtUnblock,"Unblocking Sucess"}, //x0035544 07-11-2005
         { TxtAlreadyLocked,"Category Already Locked"},  //x0035544 07-11-2005
    { TxtAlreadyUnlocked,"Category Already Unlocked"}, //x0035544 07-11-2005
    {  TxtPhoneUnblocked,"Phone Unblocked"}, //x0035544 07-11-2005
        { TxtPhoneBlocked, "Phone Blocked" },  //x0035544 07-11-2005 
    { TxtGetFailFC,"Reset FC Fail Attempts left"}, //x0035544 07-11-2005
    { TxtMaxFailFC,"Max Reset FC Fail Attempts"}, //x0035544 07-11-2005
    { TxtGetSuccFC,"Reset FC Succ Attempts left"}, //x0035544 07-11-2005
    { TxtMaxSucFC,"Max Reset FC Succ Attempts"}, //x0035544 07-11-2005
#endif    //x0035544 07-11-2005
#ifdef FF_MMI_CPHS //x0035544 07-11-2005
    { TxtViewAll,"View All"}, //x0035544 07-11-2005
    { TxtEmergencyNumbers,"Emergency Numbers"},  //x0035544 07-11-2005
#endif   	//x0035544 07-11-2005	
    { TxtNotImplemented, "Nicht unterstuetzt" },
    { TxtManu, "Customer" },
    { TxtEnterPin1, "PIN Eingabe" },
    { TxtEnterPin2, "PIN 2 eingeben" },
    { TxtEnterPuk1, "PUK Eingabe" },
    { TxtEnterPuk2, "PUK 2 eingeben" },
	{ TxtInvalidCard, "Ungueltige SIM Karte" },
    { TxtNoCard, "Keine SIM Karte" },
    { TxtInvalidIMEI, "Invalid IMEI" }, // Nov 24, 2005, a0876501, DR: OMAPS00045909
    { TxtSoftCall, "Ruf" },
    { TxtSoftOK, "Ok" },
    { TxtSoftMenu, "Menue" },
    { TxtHelp, "Hilfe" },
    { TxtSoftContacts, "Kontakte" },
    { TxtPlay, "Abspielen" },
    { TxtMessages, "Nachrichten" },
    { TxtAlarm, "Alarm" },
    { TxtSettings, "Einstellungen" },
	{ TxtExtra, "Zusaetzliches" },
    { TxtSend, "Senden" },
	{ TxtSent, "Gesendet" },
	{ TxtNotSent, "Nicht gesendet" },
    { TxtServiceCentre, "Dienstcenter" },
    { TxtPleaseWait, "Bitte warten" },
	{ TxtSmsEnterNumber, "Nummer eingeben" },
    { TxtRead, "Lesen" },
       { TxtRetrieve, "Retrieve" },  //x0035544 07-11-2005
	{ TxtDeleteAll, "Alles loeschen" },
    { TxtSave, "Speichern" },
    { TxtVoicemail, "Sprachnachrichten" },
    { TxtBroadcast, "Rundrufe" },
    { TxtNew, "Neu" },
	{ TxtDigits, "123" },
	{ TxtLowercase, "abc" },
	{ TxtUppercase, "ABC" },
    { TxtApplications, "Anwendungen" },
#ifndef FF_NO_VOICE_MEMO
    { TxtVoiceMemo, "Sprachaufz." },
#endif
    { TxtSimNot, "SIM nicht " },
	{ TxtReady, "fertig !" },
    { TxtSmsListFull, "Nachr.liste voll" },
	{ TxtNewCbMessage, "Neue CB" },
    { TxtSaved, "Gespeicherte" },
    { TxtDelete, "Loeschen" },
    { TxtReply, "Antworten" },
    { TxtStoreNumber, "Nummer speichern" },
#ifdef FF_CPHS_REL4      
    { TxtMSP, "MSP Profile" },
    { TxtMessageStatus, "Message Status" }, 
    { TxtMailBox, "MailBox"},
    { TxtElectronic, "Electronic"};
#endif    
    { TxtForward, "Weiterleiten" },
    { TxtActivate, "Aktivieren" },
    { TxtTopics, "Inhalte" },
    { TxtConsult, "Nachsehen" },
    { TxtAddNew, "Neuer Eintrag" },
    { TxtContacts, "Kontakte" },
    { TxtSearchName, "Name suchen" },
    { TxtMemory, "Speicher Stat." },
    { TxtMyNumber, "Eigene Nummer" },
    {TxtMyNumberFull, "Eigene Nummervoll" },//SPR 31705 x0021308:RamG
    { TxtRinger, "Signale" },
    { TxtVolume, "Lautstaerke" },
    { TxtKeypadTones, "Tastatur Toene" },
    { TxtNewMessage, "Neue Nachricht" },
    { TxtNewBroadcast, "Neuer Rundruf" },
    { TxtAutoAnswer, "autom. Antwort" },
    { TxtTimeDate, "Uhrzeit/Datum" },
    { TxtLanguages, "Sprachen" },
    { TxtVoiceMail, "Sprach-Nachrichten ?" },
    { TxtPinCode, "Pin Code" },
    { TxtAutoredial, "Auto Wiederwahl" },
    { TxtDTMF, "DTMF senden" },
    { TxtOn, "An" },
    { TxtOff, "Aus" },
    { TxtNetworkSel, "Netzwerk Waehlen" },
    { TxtSendID, "ID verbergen" },
    { TxtChargeTimers, "Gebuehren /Zeiten" },
    { TxtCallDivert, "Ruf umleiten" },
    { TxtCallWaiting, "Wartender Anruf" },
    { TxtIfBusy, "Wenn besetzt" },
    { TxtIfNoAnswer, "Wenn k. Antwort" },
     { TxtIfNotReachable, "Conditional Forward" },  //x0035544 07-11-2005
     { TxtAllCalls, "All Calls" },
    { TxtCancelAll, "Alle abbrechen" },
    { TxtAll, "Alle" },
    { TxtVoice, "Sprache" },
    { TxtFax, "Fax" },
    { TxtData, "Daten" },
    { TxtStatus, "Status" },
    { TxtCancel, "Abbruch" },
    { TxtCalculator, "Rechner" },
    { TxtEnterName, "Name eintragen:" },
    { TxtSoftBack, "Zurueck" },
	{ TxtSoftSelect, "Auswahl" },
    { TxtSoftOptions, "Optionen" },
    { TxtEmptyList, "Leere Liste" },
    { TxtSendMessage, "Nachricht abschicken" },
    { TxtChange, "Aendern" },
    { TxtEnterNumber, "Nummer eintragen:" },
    { TxtPhbkFull, "Telefonbuch voll" },
    { TxtWriteError, "Schreibfehler" },
    { TxtEntryDeleted, "Eintrag geloescht" },
    { TxtDetails, "Details" },
    { TxtHold, "Halten" },
    { TxtUnhold, "Aktivieren" },
    { TxtMute, "Stumm" },
    { TxtSendDTMF, "DTMF abschicken" },
    { TxtMainMenu, "Hauptmenue" },
    { TxtSwap, "Tauschen" },
    { TxtEndAll, "Alle beenden" },
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Added a menu option. "End held" text is added to the language index
    { TxtEndHeld, "End held" }, 
    { TxtPrivateCall, "Privater Anruf" },
    { TxtStartConference, "Konferenz starten" },
    { TxtReStartConference, "Konferenz wiederstarten" },
    { TxtEndConference, "Konferenz beenden" },
    { TxtFind, "Suche" },
    { TxtServiceNumbers, "Dienstnummern" },
    { TxtOutofRange, "Ausser Bereich" },
    { TxtDone, "Erfolgreich" },
    { TxtFailed, "Fehlschlag" },
#ifdef FF_MMI_MULTIMEDIA
    { TxtFileNotSupported, "File Not Supported" },
#endif
    { TxtEnterNewPin, "Neue PIN:" },
    { TxtConfPin, "Neue PIN bestaetigen" },
	{ TxtChangPin, "PIN geaendert" },
    { TxtEnd, "Ende" },
    { TxtPINFail, "Falscher Code" },
    { TxtSOS, "SOS" }, //x0035544 07-11-2005
    { TxtAutomatic, "Automatisch" },
    { TxtManual, "Manuell" },
    { TxtFixedNames, "Namen festgelegt" },
    { TxtFactoryReset, "urspruengliche Werkseinstellung" },
    { TxtSearching, "Suche" },
    { TxtChangPIN2, "PIN2 geaendert" },
    { TxtOldPIN, "Alte PIN:" },
    { TxtOldPIN2, "Alte PIN2:" },
    { TxtNotAcc, "Nicht akzeptiert" },
    { TxtEnabled, "PIN Aktiviert" },
    { TxtDisabled, "PIN Deaktiviert" },
    { TxtPINOK, "PIN akzeptiert" },
    { TxtSimBlocked, "SIM gesperrt" },
    { TxtNewPIN, "Neue PIN" },
    { TxtAcc, "Akzeptiert" },
    { TxtNoNetwork, "Kein Netzwerk" },
    { TxtNo, "Nein" },
    { TxtEnterDate, "Datum" },
    { TxtEnterTime, "Zeit" },
    { TxtScratchPad, "Notizen" },
    { TxtDeleted, "Geloescht" },
    { TxtActivated, "Aktiviert" },
    { TxtDeActivated, "Deaktiviert" },
    { TxtDeactivate, "Deaktiveren" },
    { TxtNotActivated, "Not Activated" }, // REF: OMAPS00050087 - Need to translate
    { TxtCheck, "Ueberpruefen" },
    { TxtYes, "Ja" },
    { TxtCallNumber, "Nummer anrufen" },
    { TxtEnterMid, "MID eingeben" },
    { TxtReadError, "Lesefehler" },
    { TxtUsed, "Benutzt" },
    { TxtOutgoing, "Ausgehend" },
    { TxtMuted, "Stumm" },
    { TxtCallEnded, "Ruf beendet" },
    { TxtIncomingCall, "Ankommender Anruf" },
    { TxtAnsweredCalls, "Anrufe beantwortet" }, // API - APR #1629 - Added string to lang table.    
   	{ TxtIncomingData, "Ankommende Daten" },
    { TxtIncomingFax, "Eingehendes Fax" }, //SPR#1147 - DS - Added string to lang table for incoming fax call.
    { TxtNumberWithheld, "Nummer zurueckgehalten" }, //SPR#1147 - DS - Removed extra m from Nummer.
    { TxtAccept, "Annehmen" },
    { TxtReject, "Ablehnen" },
    { TxtCalling, "Waehle" },
    { TxtMissedCalls, "Versaeumte Anrufe" },
    { TxtMissedCall, "Versaeumter Anruf" },
    { TxtLocked, "Tasten gesperrt" },
    { TxtUnmute, "Stumm aus" },
    { TxtCharging, "Aufladen ..." },
    { TxtChargComplete, "Aufladen abgeschlossen" },
    { TxtOperationNotAvail, "Operation nicht verfuegbar" },
    { TxtAddToConference, "Zur Konferenz hinzufeugen" },
    { TxtEmpty, "leer" },
    { TxtSelectFromList, "Auswahl v. Liste" },
    { TxtEnterNetworkCode, "Netzwerk Code eingeben" },
    { TxtMCCMNC, "MCC / MNC ?" },
#ifndef FF_NO_VOICE_MEMO
    { TxtRecord, "Aufzeichnung" },
#endif
	{ TxtSecurity, "Sicherheit" },
	{ TxtEnglish, "Englisch" },
	{ TxtGerman, "Deutsch" },
    { TxtValidity,"Gueltigkeit" }, 
    { TxtMessageTyp, "Art der Nachricht" /*SPR#2633 - DS - German string modified: "Art d.Message"*/ }, 
    { TxtSetVoiceMail, "Sprachnachricht aufzeichnen" /*SPR#2633 - DS - German string modified: "VoiceMail eingeben"*/ }, 
    { TxtCallTimers,"Rufdauer" },
    { TxtLastCharge,"letzte Kosten" },
    { TxtTotalCharge,"gesamte Kosten" },
    { TxtResetCharge,"Kosten loeschen" },
    { TxtIfNoService,"Not Reachable" }, //x0035544 07-11-2005
    { TxtDiscard, "Verwerfen" },
    { TxtEDIT, "Editieren" }, 
    { TxtExtractNumber, "Nummer extrahieren" },
    { TxtWrite,  "Schreiben" },
    { TxtCheckStatus,"Status checken" },
    { TxtSetDivert,"Rufumleit.setzen" },
    { TxtCancelDivert,"Rufumleit.loeschen" },
    { TxtHideID,"ID verbergen" }, 
    { TxtTones,"Toene" },
    { TxtClock,"Uhr" },
	{ TxtPhone, "Telefon" },
    { TxtStop, "Stop" },
#ifndef FF_NO_VOICE_MEMO
    { TxtRecording, "Aufnahme" },
    { TxtPlayback, "Abspielen" },
#endif
    { TxtNoNumber, "Keine Nummer" },
    { TxtOkToDelete, "OK zum Loeschen" },
    { TxtBalance, "Balance" },
    { TxtLimit, "Limit" },
    { TxtLimitEq, "Limit=" },
    { TxtRate, "Rate" },
    { TxtMinutes, "Minuten=" },
    { TxtCost, "Kosten=" },
    { TxtCredit, "Kredit" },
    { TxtPressOk, "Drueck Ok zum" },
    { TxtReset, "Zuruecksetzen" },
    { TxtIncoming, "Eingehend" },
    { TxtCounter, "Zaehler" },
    { TxtOutgoingCalls, "Abgehende Anrufe" },
    { TxtCostCounter, "Kostenzaehler" },
    { TxtEnterCurrency, "Waehrung eingeben" },
    { Txt1Unit, "1 Einheit=..." },
    { TxtChanged, "Geaendert" },
    { TxtEnterLimit, "Limit eingeben:" },
    { TxtCancelled, "Abgebrochen" },
    { TxtView, "Einsehen" },
    { TxtEnterPCK, "PCK eingeben:" },
    { TxtEnterCCK, "CCK eingeben:" },
    { TxtEnterNCK, "NCK eingeben:" },
    { TxtEnterNSCK, "NSCK eingeben:" },
    { TxtEnterSPCK, "SPCK eingeben:" },
    { TxtCodeInc, "Passwort falsch" },
    { TxtBlckPerm, "Permanent blockiert" },
    { TxtDealer, "Hersteller kontaktieren" },
    { TxtSendingDTMF, "Sende DTMF" },
    { TxtCheckNumber, "Checke Nummer" },
    { TxtNumberBusy, "Netzwerk ausgelastet" },
    { TxtNoAnswer, "Keine Antwort" },
    { TxtNumberChanged, "Nummer geaendert" },
    { TxtNetworkBusy, "Netz ueberlastet" },
    { TxtNotSubscribed, "Nicht registriert" },
    { TxtAnyKeyAnswer, "beliebige Taste Antwort" },
    { TxtPINBlock, "PIN Blockiert" },
    { TxtEdit, "Editieren" },
    { TxtChangePassword,"Password aendern" },
    { TxtSetBar,"Rufsperre einstellen" },
    { TxtSmsTone,"SMS Ton" },
    { TxtMelody,"Melodie" },
    { TxtSilent,"Stumm" },
    { TxtClick,"Klick" },
    { TxtClearAlarm,"Alarm loeschen" },
    { TxtTimeFormat,"Zeit Format" },  
    { TxtTwelveHour,"12 Hour Clock"},
	{ TxtTwentyfourHour,"24 Hour Clock"},
    { TxtInCallTimer,"Anrufzeit" },
    { TxtStore, "speichern" },
    { TxtNone, "Kein" },
    { TxtPIN, "PIN" },
    { TxtPIN2, "PIN2" },
    { TxtConfPin2, "PIN2 bestaetigen" },
    { TxtNewPIN2, "Neue PIN2" },
    { TxtUnlockOK, "Freischalten OK" },
	{ TxtIMEI, "IMEI" },
	{ TxtList, "Liste" },
	{ TxtFull, "Voll" },
    { TxtNotAvailable, "Nicht verfuegbar" },
    { TxtCallBAOC, "CallBAOC" },
    { TxtCallBAOIC, "CallBAOIC" },
    { TxtCallBAOICexH, "CallBAOICexH" },
    { TxtCallBAIC, "CallBAIC" },
    { TxtCallBAICroam, "CallBAICroam" },
	{ TxtCallBarringAll, "Anrufsperre" },
    { TxtCallBAOCAll, "CallBAOCAll" },
    { TxtCallBAICAll, "CallBAICAll" },
    { TxtCLIP, "CLIP" },
    { TxtCOLP, "COLP" },
    { TxtCOLR, "COLR" },
    { TxtSimToolkit, "SIM Toolkit" },
	{ TxtPhonebook, "Telefonbuch" },
    { TxtRecentCalls, "kuerzliche Anrufe" },
    { TxtNames, "Namen" },
    { TxtInterCalls,"Internat.Anrufe" },
    { TxtInterHome, "Ausland, auﬂer Heimatland" /*SPR#2633 - DS - German string modified: "Internat.but Home "*/ },
    { TxtWhenRoaming,"wenn Roaming" },
    { TxtCancelAllBars,"Alle Rufsperren loeschen" },
	{ TxtWaiting, "Warten" },
	{ TxtBarred, "gesperrt" },
	{ TxtForwarded, "Weitergelietet" },
    { TxtCallDivertNoReach, "Weiterleiten wenn nicht erreichbar" },
    { TxtCallDivertNoReply, "Weiterleiten wenn keine Antwort" },
    { TxtCallDivertIfBusy, "Weiterleiten wenn besetzt" },
    { TxtPressMenu, "Druecke Menue, *" },
    {TxtPlus, "Plus"},
    {TxtMinus, "Minus"},
    {TxtDivideBy, "Dividiere mit"},
    {TxtMultiplyBy, "Multipliziere mit"},
    {TxtEquals, "gleich"},
    {TxtSending, "sende"}, 
    {TxtMessage, "Nachricht"},
    {TxtSoftNext,"Naechste"},
    {TxtLastCall,"Letzter Anruf"},
    {TxtIncomingCalls,"Eingehende Anrufe"},
    {TxtLow,"leise"},
    {TxtMedium,"Mittel"},
    {TxtHigh,"laut"},
    {TxtIncreasing,"erhoehen"}, 
    {TxtTimerReset, "Timer zuruecksetzen"},
    {TxtClockSetTo, "Uhrzeit stellen auf"},
    {TxtSilentMode, "leise Mode"},
    {TxtSmsValidityPeriodSet,"Intervall einstellen"},
    {Txt1Hour,"1 Stunde"},
    {Txt12Hours,"12 Stunden"},
    {Txt24Hours,"24 Stunden"},
    {Txt1Week,"1 Woche"},
    {TxtMaxTime,"Maximum"}, //  May 10, 2004    REF: CRR 15753  Deepa M.D .Localization required
    {TxtPin2Code,"Pin2 Code"},
    {TxtEnablePIN,"PIN aktivieren"},
    {TxtRequesting,"Anfragen"},
    {TxtNotAllowed,"Nicht erlaubt"},
    {TxtEmergency,"Notruf ?"},   	
	{TxtChinese,"Chinesisch"},
	{TxtPassword,"Passwort"},
    {TxtConfirmPassword,"Passwort bestaetigen"},
    {TxtNewPassword,"Neues Passwort"},
    {TxtNoMessages,"Liste leer"},
    {TxtTwoAttemptsLeft, "2 Versuche uebrig " },
	{TxtOneAttemptLeft, "1 Versuch uebrig "},
	{Txtblacklisted, "schwarze Liste"},
    {TxtErrUnknown, "Fehler unbek."},
    {TxtSSErr,"SS Fehler"},
    {TxtUnknownSub,"Kunde unbek."},
    {TxtIllSub,"Kunde illegal"},
    {TxtIllEqip,"Mobile illegal"},
    {TxtIllOp, "Handlung illegal"},
    {TxtSubVil,"Unmoeglich"},
    {TxtInComp,"Inkompatibel"},
    {TxtFacNoSup,"Nicht machbar"},
    {TxtAbsSub,"Unverfuegbar"},
    {TxtSysFail,"System Fehler"},
    {TxtMissDta,"Fehlende Daten"},
    {TxtUnexDta,"Unerwartet"},
    {TxtPwdErr,"Eingabefehler"},
    {TxtUnkAlph,"Unbek. Zeichen"},
    {TxtMaxMpty,"Max Teiln Konf"},
    {TxtResNoAvai,"keine Antwort"},    
    {TxtUnRec,"Nicht erkannt"},
    {TxtMisTyp,"Vertippt"},
    {TxtBadStruct,"Strukt. falsch"},
    {TxtDupInv,"Mehrfach verw."},
    {TxtResLim,"Keine Mittel"},
    {TxtIniRel, "Start Problem" /*SPR#2633 - DS - German string modified: "Start problem"*/ },
    {TxtFatRes,"Fatales Ergeb."},
    {TxtEntAnw,"Antwort:"},
    {TxtSsMsg,"SS Nachricht"},
    {TxtOld,"Alt"},
    {TxtInsertnote, "Note einfuegen"},
   	{TxtMelodygenerator, "Melodiegenerator"},
   	{TxtOctaveHigher, "Oktave hoeher"},
   	{TxtOctaveLower, "Oktave niedriger"},
   	{TxtLoadMelody, "Lade Melodie"},
   	{TxtEasyText, "eZiText"},
   	{TxtDataCount, "Daten zaehlen"},
   	{TxtQoS, "QoS"},
   	{TxtTypeofConnection, "Verbindungsart"},
   	{TxtTotalCount, "Zaehler Total"},
   	{TxtResetCount, "Zaehler zurucksetzen"},
   	{TxtDataCounter, "Datenzaehler"},
   	{TxtV42bis, "v.42bis"},
   	{TxtNormal, "Normal"},
   	{TxtCSD, "CSD"},
   	{TxtClassB, "Class B"},
   	{TxtClassC, "Class C"},
    {TxtIPAddress, "IP Addresse"},
/* SPR#2324 - SH - Removed some strings */
    {TxtIPAddress2, "IP Addresse 2"},
    {TxtPort1, "Port 1"},
    {TxtPort2, "Port 2"},
    {TxtAPN, "APN"},
    {TxtConnectionType, "Verbindungdsart"},
//xpradipg - Aug 4:changes for WAP2.0 Menu
 //Localisation is not done   
 #if defined (FF_WAP) && defined (FF_GPF_TCPIP)
    {TxtNameServer1, "Primary DNS"},
    {TxtNameServer2, "Secondary DNS"},
    {TxtPPGAuthentication, "Authentication"},
    {TxtWirelessProfiledHTTP, "Proxy"},
 #endif
    {TxtGPRS, "GPRS"},
    {TxtOutgoingData, "ausgehende Daten"},
    {TxtGame, "Spiel"},
    { TxtHomepage, "Homepage" },
    { TxtBookmarks, "Bookmarks" },
    { TxtGoTo, "Gehe zu URL" },
    { TxtHistory, "History" },
   	{ TxtSetup, "Eigenschaften" },
   	{ TxtNewBookmark, "Neuer Bookmark" },
   	{ TxtWWWdot, "www." },
   	{ TxtWAPdot, "wap." },
   	{ TxtSMS, "SMS" },
   	{ TxtEmail, "Email" },
   	{ TxtClear, "loeschen" },
   	{ TxtBookmark, "Bookmark hinzufuegen" },
/* SPR#2324 - SH - Removed some strings */
   	{ TxtSaveHistory, "History speichern"},
   	{ TxtEditName, "Name editieren"},
   	{ TxtEditURL, "URL editieren"},
/* SPR#2324 - SH - Removed some strings */
   	{ TxtResponseTimer, "Antwort Zeituhr"},
   	{ TxtDialupNumber, "Dial-up Nummer" },
   	{ TxtTemporary, "voruebergehend" },
   	{ TxtContinuous, "Fortlaufend" },
   	/* SPR#2324 - SH - Removed some strings */
    { TxtWAP, "WAP" },
    { TxtExit, "Exit" },
    { TxtReload, "Neuladen" },
    { TxtStored, "gespeichert"},
    { TxtOther, "Andere"},
    { TxtConnecting, "Verbinde..." },
    { TxtDownloading, "Lade..." },
    { TxtUpdating, "Updating..." },
    { TxtEnterURL, "URL eingeben:" },
    { TxtProfileName, "Profilname: " },
    { TxtISPUsername, "Benutzername" },
    { TxtISPPassword, "Passwort" },
    { TxtGoToLink, "Gehe zu Link" },
    { TxtSelect, "auswaehlen" },
    { TxtNotConnected, "Nicht verbunden" },
    { TxtScaleImages, "Bildgroesse einstellen"},
    { TxtLoggingIn, "Einloggen in" },
	{ TxtPageNotFound, "Page not found"},	/* SPR#1983 - SH - Modified WAP error messages */
	{ TxtInvalidURL, "Invalid URL"},
	{ TxtNoResponse, "No Response"},
	{ TxtAccessDenied, "Access Denied"},
	{ TxtErrorInPage, "Error in Page"},
	{ TxtScriptError, "Script Error"},
	{ TxtServerError,  "Server Error"},
	{ TxtGatewayError, "Gateway Error"},
	{ TxtUnexpectedErr, "Unexpected Err."},
	{ TxtCharacterSet, "Character Set"},
	{ TxtWBXMLError, "WBXML Error"},
	{ TxtFatalError, "Fatal Error"},
	{ TxtGatewayBusy, "Gateway Busy"},
	{ TxtSecurityFail, "Secur. Fail"},
	{ TxtErrorOutOfMem, "Out of memory"},
	{ TxtErrorMemLow, "Memory low"},
	{ TxtError, "Error"},
	{ TxtErrorHTTP, "HTTP"},
	{ TxtErrorWSP, "WSP"},
	{ TxtErrorWAE, "WAE"},
	{ TxtErrorWSPCL, "WSPCL"},
	{ TxtErrorWSPCM, "WSPCM"},
	{ TxtErrorWTP, "WTP"},
	{ TxtErrorWTLS, "WTLS"},
	{ TxtErrorWDP, "WDP"},
	{ TxtErrorUDCP, "UDCP"},
	{ TxtNotSupported, "Nicht unterstuetzt"},
    { TxtSelectLine, "Waehle Zeile"},
    { TxtLine1, "Line 1"},
    { TxtLine2, "Line 2"},
    {TxtInfoNumbers, "Infonummern"},
    { TxtAttach, "Anheften"},
    { TxtDetach, "Loesen"},
    { TxtDetaching, "Abloesend"},
    { TxtDefineContext, "Definiere Kontext" },
    { TxtActivateContext, "Aktiviere Kontext"},
    {TxtDeactivateContext, "Deaktiviere Kontext"},
    { TxtGPRSPreferred, "GPRS bevorzugt"}, // SH 25/01/02
    { TxtCSDPreferred, "CSD bevorzugt"}, // SH 25/01/02
    { TxtNotAttached, "Nicht angeheftet"}, // SH 28/01/02
    { TxtAlreadyAttached, "Schon angeheftet"}, // SH 8/2/02
    { TxtSelected, "ausgewaehlt"}, /*ap 06/02/02*/
    { TxtNetwork, "Netzwerk"}, /*ap 06/02/02*/
    { TxtForbidden, "Verboten"}, /*ap 06/02/02*/
    { TxtActivateFDN, "FDN aktivieren "}, /*ap 06/02/02*/
    { TxtPlease, "Bitte"},	/*ap 06/02/02*/
   	{ TxtTimedOut, "Zeitueberschreitung"}, // SH 13/02/02
	{ TxtWillChangeOn, "Werde wechseln bei"}, // SH 13/02/02
    { TxtAttached, "Angeheftet"}, // SH 14/02/02
    { TxtDetached, "geloest"}, // SH 14/02/02
    { TxtUnknown, "Unbekannt"},
    { TxtCallTransfer, "Verbinde Anruf"},//MC
    { TxtConnected, "Verbunden"}, // SH
    { TxtGSM_900,"GSM 900"},	// SH
    { TxtDCS_1800,"DCS 1800"},
	{ TxtPCS_1900,"PCS 1900"},
    { TxtE_GSM, "E GSM"},
	{ TxtGSM_850, "GSM 850"},
    { TxtCallTimeout, "Anruf Zeitueberschreitung" },     
	{ TxtBand, "Band"},
    { TxtSatRefresh, "SAT neu laden" },    
    { TxtCallBlocked, "Anruf blockiert" },
    { TxtSRR, "Status report" }, // Marcus: Issue 1170: 30/09/2002
     //March 2, 2005    REF: CRR 11536 x0018858
         { TxtRP, "Reply Path" },// added the string for the reply path.
    { TxtDelivered, "Delivered" }, // Marcus: Issue 1170: 07/10/2002
    { TxtTempError, "Temp. Error" }, // Marcus: Issue 1170: 07/10/2002
    {	TxtUnknownCode, "Unbekannter Code" /*SPR#2633 - DS - German string modified: "Unknown Code"*/ },/*MC SPR 1111*/
    { TxtPhoneSettings, "Einstellungen" }, //SPR#1113 - DS - Added text id for renamed main menu item "Phone Settings"
    { TxtNetworkServices, "Netzwerk Services" }, //SPR#1113 - DS - Added text id for new main menu item "Netw. Services"
    { TxtCallDeflection, "Anruf abgelenkt" },  //SPR#1113 - DS - Added text id for new menu item "Call Deflection" (CD)
    { TxtCallBack,  "Zurueckrufen" }, //SPR#1113 - DS - Added text id for new menu item "Call Back" (CCBS)
       { TxtMobileInfo,  "Mobile Info"},  /*MC, SPR1209*/
  	{ TxtServingCell,  "Serving Cell"}, /*MC, SPR1209*/

#ifdef TI_PS_FF_EM 
/*EngMode */
    { TxtServingCellRF, "Serving cell RF"},
    { TxtServingChannel, "Serving Channel"},
    { TxtAmrInfo, "AMR Information"},
    { TxtSNDCPInfo, "SNDCP Information"},
    { TxtUserLevelInfo, "User Level Information"},
    { TxtLLCTracing, "LLC Tracing info"},
    { TxtRlcMacInfo, "RLC/MAC Information"},
    { TxtGMMInfo, "GMM Information"},
    { TxtCallStatus, "Call status"},
    { TxtLayerMessages, "Layer Messages"},
    { TxtWCDMALayerMessages, "WCDMA Layer messages"},
    { Txt3GInfo, "Additional 3G information"},
    { TxtPacketDataParameter, "Packet data parameters"},
#endif
 
 	{ TxtNeighbourCell, "Neighbour Cell"},   /*MC, SPR1209*/
 	{ TxtLocation, "Ortsbestimmung"},    /*MC, SPR1209*/
	{ TxtCiphHopDTX, "Ciph Hop DTX"}, /*MC, SPR1209*/
    { TxtConcat, "verkette" },    
   { TxtColourMenu,		"Farben"},	
    { TxtColourBlack,		"Schwarz"},	
	{ TxtColourBlue,		"Blau"},
	{ TxtColourCyan,		"Zyan"},
	{ TxtColourGreen, 		"Gruen"}, 
	{ TxtColourMagenta, 	"Magenta"}, 
	{ TxtColourYellow,		"Gelb"},
	{ TxtColourRed, 		"Rot"}, 
	{ TxtColourWhite,		"Weiss"},	
	{ TxtColourPink,		"Pink"},
	{ TxtColourOrange,		"Orange"},
	{ TxtColourLightBlue,	"Hellblau"},
	{ TxtColourLightGreen, 	"Hellgruen"}, 
	{ TxtColourPaleYellow, 	"leicht Gelb"},
    { TxtColourTransparent,	"Transparent"},	
	{ TxtIdleScreenBackground,		"Untaetiges Display Hintergrund"},
	{ TxtIdleScreenSoftKey,			"Untaetiges Display Softkey"},
	{ TxtIdleScreenText,			"Untaetiges Display Text"},
	{ TxtIdleScreenNetworkName,  	"Untaetiges Display Netzwerk"},
	{ TxtGeneralBackground ,  		"Genereller Hintergrund"},
	{ TxtMenuSoftKeyForeground , 	"Menue Softkey Vordergrund"},
	{ TxtMenuSoftKeyBackground ,  	"Menue Softkey Hintergrund"},
	{ TxtSubMenuHeadingText ,  		"Untermenue Ueberschrift Text"},
	{ TxtSubMenuHeadingBackground,	"Untermenue Ueberschrift  Hintergrund"},
	{ TxtSubMenuTextForeground ,  	"Untermenue Text Vordergrund"},
	{ TxtSubMenuTextBackground,  	"Untermenue Text Hintergrund"},
	{ TxtSubMenuSelectionBar,  		"Untermenue Ueberschrift Auswahlliste"},
	{ TxtPopUpTextForeground,  		"Popup Text Vordergrund"},
	{ TxtPopUpTextBackground,		"Popup Text Hintergrund"},

	{ TxtColour,			"Farben", },	
    { TxtLoser,					"Verloren" },
    { TxtPlayAgain,					"Nochmal spielen?" },
    { TxtWinner,					"Gewonnen" },		    
	{ TxtSelectBook, 		"Waehle Buch" },	/* SPR1112 - SH */
    	{ TxtSIM, 			"SIM" },
    	{ TxtMove, 			"Bewege" },
    	{ TxtCopy, 			"Kopiere" },
    	{ TxtThisEntry, 		"Dieser Eintrag"},
    { TxtBackground,	"Hintergrund"},	/* for selecting backgrounds*/
    { TxtProviderIcon,	"Netzwerk Icon"}, /*AP - 29-01-03 - Add to language table */    
    { TxtIdleScreen,	"Untaetiges Display Bild"},	
	{ TxtMenuScreen,	"Haupt Men? /*SPR#2633 - DS - German string removed: "Main menu screen"*/ },	

	{ TxtBgdSquares,	"Rechtecke"},
	{ TxtBgdO2,			"O2"},
	{ TxtBgdTree,		"Baum"},
    { TxtBgdTILogo,		"TI Logo"},
    { TxtBgdSunset,		"Sonnenuntergang"},
    { TxtBgdEdinburgh1,				"Edinburgh 1"},
    { TxtBgdEdinburgh2,				"Edinburgh 2"},
    { TxtBgdEdinburgh3,				"Edinburgh 3"},
    { TxtBgdEdinburgh4,				"Edinburgh 4"},
    { TxtDeflectCall,"Anruf ablenken"}, //SPR 1392 calldeflection
    { TxtDeflectCallTo,"ablenken nach"}, //SPR 1392  calldeflection
    { TxtDeflect,"ablenken "}, //SPR 1392  calldeflection
    { TxtDeflectingCallTo,"lenke ab nach"}, //SPR 1392  calldeflection
    { TxtCallBackQuery,  "Zurueckrufen Anfrage" }, // Marcus: CCBS: 15/11/2002
    { TxtRejected,  "abgelehnte" },       // Marcus: CCBS: 15/11/2002
	{ TxtActiveCall,			"Aktive Anrufe" },	/* SPR#1352 - SH*/
	{ TxtHeldCall,				"gehaltener Anruf" },
	{ TxtActiveTTYCall,			"Aktive TTY Anrufe" },
	{ TxtHeldTTYCall,			"gehaltene TTY Anrufe" },
	{ TxtTTY,				"TTY" },		
    #ifdef NEPTUNE_BOARD    /* Mar 27, 2006    REF:ER OMAPS00071798 */
    {TxtAudioMode, "Audio mode"},
    {TxtAudioModeText, "Text mode"},
    {TxtAudioModeVco, "VCO mode"},
    {TxtAudioModeUnknown, "Unknown"},
    #endif    
    { TxtBootTime, "Boot Time Measurement"}, /*OMAPS0091029 x0039928(sumanth)*/
	/*x0039928 OMAPS00097714 - HCO/VCO implementation - added strings*/
#ifdef FF_TTY_HCO_VCO	
    { TxtTTYAll,			"TTY All" },
    { TxtTTYVco,			"TTY Vco" },
    { TxtTTYHco,			"TTY Hco" },
#endif    
    { TxtOnNextCall,		"Beim naechsten Anruf" },
    { TxtAlwaysOn,			"Immer an" },
    { TxtTTYCalling,		"TTY anrufen" },
	{ TxtIncomingTTYCall,	"Eingehender TTY Anruf"},
    { TxtProvisioned, 		"bereitgestellt"}, // Marcus: Issue 1651: 30/01/2003
    { TxtExecuted, 			"ausgefuehrt"}, 	// Marcus: Issue 1651
    { TxtCommand, "Befehl" /*SPR#2633 - DS - German string modified: "Command"*/ }, // NDH : SPR#1869 : 08/04/2003
    { TxtCompleted, "Fertig" /*SPR#2633 - DS - German string modified: "Completed"*/}, // NDH : SPR#1869 : 08/04/2003
    { TxtMultiparty, "Konferenz" /*SPR#2633 - DS - German string modified: "Conference"*/}, // AP: Issue 1749: 06/03/03  
	{ TxtAttaching,			"Attaching"},	/* SPR#1983 - SH - Various WAP texts */
	{ TxtConnection,		"Connection"},	
    	{ TxtUsing, 			"Using"},
        { TxtConnectionSpeed,	"Connect. Speed"},
    	{ TxtAnalogue,		"Analogue"},
    	{ TxtISDN9600,		"ISDN 9600"},
    	{ TxtISDN14400,		"ISDN 14400"},
    	{ TxtEndWap,		"End WAP"},
	{ TxtUntitled,		"Untitled"},
	{ TxtChooseReplacement1, "Choose"}, /* SPR#2354 */
	{ TxtChooseReplacement2, "Replacement"}, /* SPR#2354 */
	{ TxtDialup, "Dialup" }, /* SPR#2324 - SH - Added */
   	{ TxtAdvanced, "Advanced" },
   	{ TxtGPRSDialupGPRS, "(GPRS)/Dialup" },
   	{ TxtGPRSDialupDialup, "GPRS/(Dialup)" },
   	{ TxtProfiles, "Profiles" },
   	{ TxtNewProfile, "New Profile" },
   	{ TxtResetProfiles, "Reset Profiles" },
	{ TxtPasswords, "Passwords"}, 	//SPR16107	
	{ TxtMismatch, "donot match!"}, 	//SPR16107	
	{ TxtDataCounterReset, "Reset Counter?"}, /* SPR#2346 - SH */
	{ TxtCallDate, "Date : "}, /* API - 05/09/03 - SPR2357 - Added */
	{ TxtCallTime, "Time : "}, /* API - 05/09/03 - SPR2357 - Added */   	
	{ TxtInvalidMessage, "Invalid Message"},/*MC SPR 2530*/
	{ TxtIncompleteMessage, "Incomplete Message"},/*MC SPR 2530*/
	/* SUJATHA MMS Integration */
	           { TxtDownload,                  "Download"},
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)  //added by ellen
        { TxtShowMessages,              "Show Push Messages"}, //x0035544 07-11-2005
        { TxtPushMessages,              "Push Messages"}, //x0035544 07-11-2005
        #endif

#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS) //x0035544 07-11-2005
        {TxtMMS,            "MMS"}, 
        {TxtEMS,            "EMS"},         
        {TxtMMSCreate,"MMS erstellen"},
        {TxtEMSCreate,"EMS erstellen"},        
        {TxtMMSInbox,"Eingang"},
        {TxtMMSUnsent,"Ungesendet"},
        {TxtMMSSent,"Sent"}, //CRR: 25291 - xrashmic 14 Oct 2004
        {TxtMMSSettings,"Einstellung"},
        {TxtRetrieveCondition,"Rertieval Type"},  //x0035544 07-11-2005      
        {TxtMMSInsertPicture,"Bild einfuegen"},
        {TxtMMSInsertSound,"Ton einfuegen"},
        {TxtMMSInsertText,"Text einfuegen"},
        {TxtMMSInsertSlide,"Slide einfuegen"},
        {TxtMMSDelete,"Loeschen"},
        {TxtMMSPreview,"Vorschau"},
        {TxtMMSDeleteSlide,"Slide loeschen"},
        {TxtMMSDeletePicture,"Bild loeschen"},
        {TxtMMSDeleteSound,"Ton loeschen"},
        {TxtMMSDeleteText,"Text loeschen"},
        {TxtMMSInsert, "Insert"},
	//xrashmic 19 Aug, 2004 Bug: 2, 3, 36 and 42
        {TxtMMSSendSucess,"Sent"},
        {TxtMMSSendFail, "Send Failed"},
        {TxtMMSSendng, "Sending"},
        {TxtMMSDownloading,"Receiving"},
        {TxtNewMMS,"New MMS"},
        {TxtMMSRcvFail,"Receive Failed"},
        {TxtMMSDots," MMS ..."},
        
        {TxtExtract, "Extract" },
	 {TxtMMSNotification, "MMS Notification" }, //x0035544 07-11-2005
        {TxtImmediate, "Immediate" }, //x0035544 07-11-2005
        {TxtDeferred, "Deferred" }, //x0035544 07-11-2005
        {TxtMMSRetrieving, "MMS Retrieving" }, //x0035544 07-11-2005
        {TxtMMSRetrieved, "MMS Retrieved" }, //x0035544 07-11-2005
        {TxtEMSNew," New EMS"},  //x0035544 07-11-2005
        {TxtType, "Type"},  //x0035544 07-11-2005
        {TxtObject,"Object"}, //x0035544 07-11-2005
        {TxtLarge," Too Large"},  //x0035544 07-11-2005
        
        {TxtMMSInsertPrePicture,"Ins Pre Picture"},  //x0035544 07-11-2005    
        {TxtMMSInsertPreSound, "Ins Pre Sound"},  //x0035544 07-11-2005
        {TxtAllObjects, "All Objects"},  //x0035544 07-11-2005
        {TxtExtracted, "Extracted"},  //x0035544 07-11-2005
        {TxtExtracting,"Extracting"}, //x0035544 07-11-2005
        {TxtNoObjects, "No Objects"},  //x0035544 07-11-2005
        {TxtExtractable, "Extractable"},  //x0035544 07-11-2005
#endif//x0035544 07-11-2005
	{ TxtLockAls, "ALS Sperren"}, /* NDH : CQ16317 */
	{ TxtUnlockAls, "ALS Entsperren"}, /* NDH : CQ16317 */
	{ TxtAlsLocked, "ALS Gesperrt"}, /* NDH : CQ16317 */
	{ TxtAlsUnlocked, "ALS Entsperrt"}, /* NDH : CQ16317 */
	{ TxtLimService, "Limited Service"}, /* SPR12653 */  //localization required.
	

//   Apr 06, 2005	REF: ENH 30011 xdeepadh
//Strings for Camera, Mp3 and midi test application are 
//under the respective flags.        
//Nov 14, 2005    REF: OMAPS00044445 xdeepadh

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */
// #if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
	{ TxtPlayerMonoChannel, "Set Channels to Mono"}, /* SPR12653 */
	{ TxtPlayerStereoChannel, "Set Channels to Stereo"}, /* SPR12653 */
	{ TxtPlayerSetChannels, "Channels settings"}, /* SPR12653 */
	{ TxtPlayerPlay, "Datei spielen"}, /* SPR12653 */
	{ TxtPlayerTest, "Audio Player"}, /* SPR12653 */
	{ TxtPlayerFileSelected, "Datei gewaehlt"}, /* SPR12653 */
	{ TxtPlayerSelectFile, " Datei waehlen"}, /* SPR12653 */
	{ TxtPlayerPause, "Pause"}, /* SPR12653 */
	{ TxtPlayerResume, "Neuladen"}, /* SPR12653 */
	{ TxtPlayerStop, "Stop"}, /* SPR12653 */
	{ TxtPlayerPlaying, "Datei gespielt"}, /* SPR12653 */
	{ TxtPlayerForward, "Forward"},  /* OMAPS00070659  x0039928  */
	{ TxtPlayerRewind, "Rewind"},
	{ TxtPlayerForwarding, "Forwarding"},
	{ TxtPlayerRewinding, "Rewinding"},  /* OMAPS00070659  x0039928  */
//	Jul 18, 2005    REF: SPR 31695   xdeepadh	
	{ TxtPlayerFileNotLoaded, "No Files loaded"},
	{ TxtPlayerOptionNotImplemented, "Option not yet implement"}, 
// #endif //FF_MMI_TEST_MP3

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */

//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
	{ TxtCameraTest, "Camera Application"}, /* NDH : CQ16317 */
	{ TxtViewFinder, "View Finder"}, /* NDH : CQ16317 */	
	{ TxtSnapshot, "Snapshot nehmen"}, /* NDH : CQ16317 */		
	{ TxtSaveSnapshot, "Snapshot speichern"}, /* NDH : CQ16317 */	
	{ TxtSnapshotSaved, "Snapshot gespeichert"}, /* NDH : CQ16317 */	
	{ TxtSoftSnap, "Snapshot"}, /* NDH : CQ16317 */		
	{ TxtSoftSnapSave, "speichern"}, /* NDH : CQ16317 */		
	{ TxtSnapSaving, "Snapshot gespeichert"},
	{ TxtQuality, "Quality"},
	{ TxtEconomy, "Economy"},
	{ TxtStandard, "Standard"},
	{ TxtColor, "Color"},
	{ TxtSepia, "Sepia"},
	{ TxtBlackandWhite, "Black&White"},
	{ TxtNegative, "Negative"},
	{ TxtFilename, "Filename"},
	{ TxtDigiZoom, "Zoom"},
	{ TxtFrame, "Border Frame"},
	{ TxtFrame1, "Border Frame1"},
	{ TxtFrame2, "Border Frame2"},
	{ TxtNoFrame, "No frame"},
	{ TxtShootingMode, "Shooting Mode"},
	{ TxtSingleshot, "Single Shot"},
	{ Txt2Shots, "2 Shots"},
	{ Txt4Shots, "4 Shots"},
	{ Txt8Shots, "8 Shots"},
#endif //FF_MMI_TEST_CAMERA
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Midi Test Application Strings were put under the flag FF_MMI_TEST_MIDI
#ifdef FF_MMI_TEST_MIDI
       { TxtMidiTest, "Midi Application"}, 
       { TxtMidiChangeConfig, "ALS Entsperrt"}, 
       { TxtMidiPlay, "ALS Entsperrt"},
       { TxtMidiPlayAllFiles, "ALS Entsperrt"},
       { TxtMidiExit, "ALS Entsperrt"},
       { TxtChConfigFile, "ALS Entsperrt"},
       { TxtChConfigVoiceLimit, "ALS Entsperrt"},      
       { TxtChConfigLoop, "ALS Entsperrt"},       
       { TxtChConfigChannels, "ALS Entsperrt"},       
       { TxtChConfigBankLocation, "ALS Entsperrt"},     
       { TxtMidiEnterVoiceNumber, "Enter number of voices"},
    	{ TxtMidiSelectFile,  "Select file to be played"},
	{ TxtMidiOptionNotImplemented, "Option not yet implement"}, 
       { TxtMidiLoopOn, "On"},  //x0035544 07-11-2005     
       { TxtMidiLoopOff, "Off"},    //x0035544 07-11-2005   
       { TxtMidiChannels, "Channels"},       
       { TxtMidiMonoChannel, "Mono"},  //x0035544 07-11-2005
       { TxtMidiStereoChannel, "Stereo"},  //x0035544 07-11-2005
       { TxtMidiBankLocation, "Bank"},
	{ TxtMidiFastBankLocation, "Fast"}, //x0035544 07-11-2005
	{ TxtMidiSlowBankLocation, "Slow"}, //x0035544 07-11-2005
    	{ TxtMidiFileTitle, "Select File to be played"},
    	{ TxtMidiFileNotLoaded, "No Files loaded"},
    	{ TxtMidiFileSelected, "File Selected"},
       { TxtChConfigAudioOutput, "Audio Output"}, 
	{ TxtMidiSpeaker, "Speaker"},
	{ TxtMidiHeadset, "Headset"},
//x0pleela 30 Jan, 2007  DR: OMAPS00108892
//Adding the following strings to display error message
//when loop on is enabled while playing all files	
	{ TxtLoopOn, "Loop ON"},
	{ TxtCantPlayAllFiles, "Cannot Play All Files"},	
	{ TxtCantBeActivated, "Cannot be Activated"},
#endif//#ifdef FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
	/*
	** Start of Bluetooth Related Strings
	*/
    	{TxtBluetooth, "Bluetooth"},
    	{TxtBluetoothEnable, "Enable"},
    	{TxtBluetoothDisable, "Disable"},
    	{TxtBtEnterDeviceName, "Enter Device Name:"},
    	{TxtBtEnterPassKey, "Enter Pass Key:"},
    	{TxtBtPassKey, "Pass Key"},
    	{TxtBtTimeout, "Timeout"},
    	{TxtBtPairing, "Pairing"},
    	{TxtBtPaired, "Paired"},
    	{TxtBtAuthorize, "Authorize"},
    	{TxtBtUnAuthorize, "Un-Authorize"},
	{TxtBtInfo, "Info"},
	{TxtBtYesOnce, "Yes (Once)"},
	{TxtBtYesAlways, "Yes (Always)"},
	{TxtBtPairDevice, "Pair Device"},
	{TxtBtChangePassKey, "Change PassKey"},
	{TxtBtShowServices, "Show Services"},
	{TxtBtAddToKnown,	 "Add To Known"},
	{TxtBtRemoveDevice, "Remove Device"},
	{TxtBtDeviceName, "Device Name"},
	{TxtBtSet, "Set"},
	{TxtBtServices, "Services"},
	{TxtBtNoServices, "No Services"},
	{TxtBtSupported, "Supported"},
	{TxtBtSerialPort, "Serial Port"},
	{TxtBtDialUpNetworking, "Dial Up Networking"},
	{TxtBtHeadSet, "Head Set"},
	{TxtBtHandsFree, "Hands Free"},
	{TxtBtObjectPush, "Object Push"},
	{TxtBtFileTransfer, "File Transfer"},
	{TxtBtLanAccess, "Lan Access"},
	{TxtBtCordlessTelephony, "Cordless Telephony"},
	{TxtBtIntercom, "Intercom"},
	{TxtBtUnknownService, "Unknown Service"},
	{TxtBtRequiresAuthorizationForService, "%s requires Authorization for Service : %s"},
	{TxtBtDevices, "Devices"},
	{TxtBtDevice, "Device,"},
	{TxtBtHidden, "Hidden"},
	{TxtBtTimed, "Timed"},
	{TxtBtKnownDevices, "Known Devices"},
	{TxtBtDiscoverable, "Discoverable"},
	{TxtBtNoDevices, "No Devices"},
	{TxtBtFound, "Found"},
    	{TxtBtSetLocalName, "Set Local Name"},
	{TxtBtClearStoredDevices, "Clear all Stored Devices"}, /* To be translated */
    	/*
    	** End of Bluetooth Related Strings
    	*/
#endif 
	{ TxtFDNName, "FDN Numbers" }, //x0035544 07-11-2005
	{ TxtAllDivert, "All Divert"}, /* SPR13614 */  //localization required.
	{ TxtNoReachDivert, "Divert when not reachable"}, /* SPR13614 */  //localization required.
	{ TxtBusyDivert, "Divert if Busy"}, /* SPR13614 */  //localization required.
	{ TxtNoAnsDivert, "Divert when no answer"}, /* SPR13614 */  //localization required.
	{ TxtCondDivert, "Divert for Cond. Forward"}, /* SPR13614 */  //localization required.
	{ TxtAllCallsDivert, "Divert for All calls"}, /* SPR13614 */  //localization required.
	 	{ TxtInvalidInput, "Invalid input"}, //x0035544 07-11-2005
	{ TxtFDNListFull, "FDN List Full"}, //x0035544 07-11-2005

	
	////x0035544 Nov 07,2005 copied the lines here
    	//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
	//Fix: Added these strings to display the various speaker volume levels.
	{TxtLevel1 ," Level 1"},  //x0035544 07-11-2005
	{TxtLevel2 ," Level 2"},  //x0035544 07-11-2005
	{TxtLevel3 ," Level 3"},  //x0035544 07-11-2005
	{TxtLevel4 ," Level 4"},  //x0035544 07-11-2005
	{TxtLevel5 ," Level 5"},  //x0035544 07-11-2005
        {TxtImage,"Image"}, //x0035544 07-11-2005
       { TxtSmsWaiting, "Message waiting"}, //x0035544 07-11-2005        

/* Power management Menu*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        { Txt10Second, "10 Sec" },
        { Txt15Second, "15 Sec" },
        { Txt20Second, "20 Sec" },
	 { TxtSetParam1, "Set Param"},    
#endif
#endif

#ifdef FF_MMI_MULTIMEDIA
      /* Multimedia Menu RElated String.  */
      	{TxtMultimediaApp, "Multimedia Test Application"},     
      	{TxtConfigurationParameters, "Configuration Parameters"},          	
      	{TxtAudioVideoPlayback, "Audio/Video Playback"},          	      	
      	{TxtMidiPlayback, "Midi Playback"},          	      	
      	{TxtImageViewer, "Image Viewer"},           	      	
      	{TxtCameraCapture, "Camera Capture"},         	      	
      	{TxtVideoRecording, "Video Recording"}, 
      	{TxtAudioRecording, "Audio Recording"}, 
      	{TxtPlayRecording, "Play Recording"}, 
      	{TxtFileListType, "File Listing"},          	      	        
	{TxtNoFileAvail, "No Files Available"},     	
	/* Configuration Parameter menu. */
	{TxtOutputScreenSize, "Output Screen Size"},       		
	{TxtAudioEncodingFormat, "Audio Encoding Format"},       			
	{TxtVideoEncodingFormat, "Video Encoding Format"},      			
	{TxtVideoCaptureSize, "Video Capture Size"},      			
	{TxtVideoFrameRateSelection, "Video Frame Rate Selection"},       			
	{TxtAudioBitRate, "Audio Bitrate"},       			
	{TxtVideoBitRate, "Video Bitrate"}, 
	{TxtMidiPlayerSelect, "Midi Player Enabled"}, 
        /* Screen Mode. */
	{TxtHalfScreenMode, "Half-Screen Mode" },	
	{TxtFullScreenMode, "Full-Screen Mode" },		
	/* Audio Format */
	{TxtAAC, "AAC" },		
	{TxtAMR, "AMR-NB" },		
	{TxtPCM, "PCM" },			
	{TxtMP3, "MP3" },			
	/* Video Format */
	{TxtMpeg4, "MPEG4" },		
	{TxtH263, "H263" },		
        /* Screen Size */
        {TxtQcif, "QCIF" },		
	{TxtCif, "SQCIF" },		
       /* Frame Rate */
	{Txt5fps, "5 FPS"},    			
	{Txt10fps, "10 FPS"},			
	{Txt15fps, "15 FPS"}, 			
	{Txt20fps, "20 FPS"},
	{Txt25fps, "25 FPS"},   			
	{Txt30fps, "30 FPS"}, 
        /* Bit Rate  */
	{TxtAACBitRate, "AAC BitRate" },		
	{AMRNBBitRate, "AMR-NB BitRate" },		
	 /* AAC Bit Rate */
	{Txt48Kbps, "48 KBPS"},     			
	{Txt66p15Kbps, "66.15 KBPS"}, 			
	{Txt72Kbps, "72 KBPS"}, 				
	{Txt96pKbps, "96  KBPS"},			
	{Txt132p3Kbps, "32.3 KBPS"}, 
	{Txt144Kbps, "144 KBPS"},    			
	{Txt192Kbps, "192 KBPS"}, 
	{Txt264p6Kbps, "264.6 KBPS"},   			
	{Txt288Kbps, "288 KBPS"}, 
        /* AMR Bit Rate */
       {Txt1p80Kbps, "1.80 KBPS"},
	{Txt4p75Kbps, "4.75 KBPS"},     			
	{Txt5p15Kbps, "5.15 KBPS"}, 		
	{Txt5p9Kbps, "5.9 KBPS"}, 			
	{Txt6p7Kbps, "6.7 KBPS"}, 
	{Txt7p4Kbps, "7.4 KBPS"},    			
	{Txt7p95Kbps, "7.95 KBPS"}, 
	{Txt10p2Kbps, "10.2 KBPS"},    			
	{Txt12p2Kbps, "12.2 KBPS"}, 
        /* Video Bit Rate */
	{Txt64Kbps, "64 KBPS"}, 
	{Txt128Kbps, "128 KBPS"},    			
	{Txt384Kbps, "384 KBPS"},
	/* Softkey related string */
	{TxtPauseKey, "Pause"},
	{TxtResumeKey, "Resume"},
	{TxtEncodeKey, "Encode"}, 	
	{TxtSoftIVT, "IVT"},	
	{TxtPictureDisp, "Picture Display"},	
	{TxtSetParam, "Set Param"},       	
	 /* Audio Path Configuration Parameter */
	{TxtAudioPath, "Audio Path Configuration"},  
	{TxtHeadset, "Head Set"},				  
	{TxtHeadphone, "Stereo Head Phone"},	
	/* Softkey / Title related sting. */
	{TxtSoftStart, "Start"}, 						
	{TxtAudvidRecord, "Audio / Video Recording"},  
	{TxtQQVga, "QQVGA"},                                   
	{TxtEncdTo, "Encode To"},         
	{TxtJPEGFile, "JPEG File"},          
	/* File Management Releated String. */
	{TxtFileMgmt, "File Management"},   
	{TxtFileOprns, "File Operations" },	
	{TxtFileOprnsTest, "File Operation Test" },	
	{TxtFFS, "Flash File System" },	
	{TxtMMC, "MMC / SD Card" },
	/*{TxtFMMMC, "MMC Card" },
	{TxtSDC, "SD Card" },	
	{TxtTestCase1, " Folder Not Empty"  },	
	{TxtTestCase2, "Operation List" },	
 	{TxtTestCase3, "Destination" },	*/
 	{TxtOperSuccess, "Operation Success" },
	{TxtOperFailed, "Operation Failed" },
	{TxtNotYetDone, "Not Yet Implemented" },
	/*For FTP Application */
	{ TxtFTPApp, "FTP" },
	{ TxtPut, "Put" },
	{ TxtGet, "Get" },	
	 /* Configuration Parameter - Volume / Preferred Storage. */
       { TxtVolumeConfig, "Volume Configuration"},
       { TxtPreferredStorage, "Preferred Storage"},
       { TxtAudioDecodeConfig, "Audio Decode Configuration"},
       { TxtAudioEncodeConfig, "Audio Encode Configuration"},
       { TxtVideoEncodeConfig, "Video Encode Configuration"},       
       { TxtSpeaker, "Handset"},
       { TxtEnterPath, "Enter File Path:"},
       {TxtInvalidPath, "Invalid"},
	{ TxtRename, "Rename" },
       { TxtCreateFolder, "CreateFolder" },
       { TxtEnterFileName, "Enter File Name : " },
	{ TxtFolderNotEmpty, " Folder Not Empty" },
       /*{ TxtOperationList, "Operation List" },*/
       { TxtDestination, "Destination" },
      	{ TxtVMPlayback, "Voice Memo Playback" },
	{ TxtVMRecord, "Voice Memo Recording" },
	{ TxtVMConfigParam, "Configuration Parameters" },
	{ TxtStartRecord, "Start Voice Memo Recording" },
	{ TxtPlayingfile, "Playing Selected Fie " },
	{ TxtPauseFile, "Pause Selected File" },
       { TxtRecordVoice, "Recording Voice" },
	{ TxtSoftMore, "More" },
       { TxtUseCase, "Use Cases" },
      	{ TxtRepeateForEver, "Repeate For Ever" },
       { TxtPlayonce, "Play Once" },
       { TxtCamera , "Camera"},
	{ TxtCapture, "Capture" },
	{ TxtCrop, "Crop"},
	{ TxtRescale, "Rescale"},
	{ TxtRotate, "Rotate"},
	{TxtDummyStr, "Dummy String"}, 
	{TxtDummyStr1, "Dummy String1"}, 
	/*PCM Decode sampling Rate*/
       {Txt8kRate, "8k Sample Rate"}, 
       {Txt11kRate, "11k Sample Rate"}, 
       {Txt12kRate, "12k Sample Rate"},         /*OMAPS00081264 - 12k Menu addition */
       {Txt16kRate, "16k Sample Rate"}, 
       {Txt22kRate, "22k Sample Rate"},
       {Txt24kRate, "24k Sample Rate"},       /*OMAPS00081264 - 12k Menu addition */
       {Txt32kRate, "32k Sample Rate"}, 
       {Txt44kRate, "44k Sample Rate"}, 
       {Txt48kRate, "48k Sample Rate"}, 
       {TxtPcmRate, "PCM Sample Rate"}, 
       {TxtPcmChannelMode, "PCM Channel Mode"}, 
       /* Image Quality Setting */
       {TxtImgCaptureQuality, "Image Capture Quality"}, 
       {Txt0Set, "0"}, 
       {Txt10Set, "10"}, 
       {Txt20Set, "20"}, 
       {Txt30Set, "30"}, 
       {Txt40Set, "40"}, 
       {Txt50Set, "50"}, 
       {Txt60Set, "60"}, 
       {Txt70Set, "70"}, 
       {Txt80Set, "80"}, 
       {Txt90Set, "90"}, 
       {Txt100Set, "100"}, 
       {TxtStreoMode, "Stereo Mode"},
       {TxtMonoMode, "Mono Mode"},
       {TxtVideoPlayOption, "Video Playback Option"},
       {TxtAudioVideoPlay, "Audio & Video Play"},
       {TxtVideoPlay, "Video Play Only"}, 
        /* Video Recording Duration Specification */
       {TxtVideoRecDuration, "Video Recording Duration"},      
       {Txt10sec, "10 Sec Recording"},
       {Txt15sec, "15 Sec Recording"},
       {Txt20sec, "20 Sec Recording"},
       {TxtNonStop, "Non Stop Recording"},
   /* Video Only Option - x0043641 */
       {TxtVideoRecordOption, "Video Record Option"},
       {TxtAudioVideoRecord, "Video & Audio Recording"},
       {TxtVideoRecord, "Video Recording Only"},
#endif

// 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005        
#ifdef FF_MMI_AUDIO_PROFILE
	{TxtLoudspeaker, "Loudspeaker"},//xpradipg - 31895 : localization required
	{TxtHandheld, "Handheld"},//xpradipg - 31895 : localization required
	{TxtCarkit, "Carkit"},//xpradipg - 31895 : localization required
	{TxtHeadset,"Headset"},//xpradipg - 31895 : localization required
	{TxtInserted, "inserted"},//xpradipg - 31895 : localization required
	{TxtRemoved, "removed"},//xpradipg - 31895 : localization required
#endif	
	//Aug 22, 2005    REF: ENH 31154 xdeepadh
	
#ifdef (FF_MMI_FILE_VIEWER)
	{ TxtFileNotLoaded,"No Files loaded"},
	{ TxtFileViewer, "FileViewer"}, 
	{ TxtRename, "Rename"}, 
	{TxtDeleteConfirm,"Delete?"},
	{TxtFileTooBig,"File Too Big"},
	{TxtFileExists,"File exists"}, //Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh
	{TxtWrongFilename,"Wrong Name"},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,"Wrong extn"},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
#endif //FF_MMI_FILE_VIEWER
	
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        {TxtPowerMgnt,"Power Management"},
#endif
#endif
#if defined(FF_MMI_TEST_CAMERA)||defined(FF_MMI_CAMERA_APP)
	{ TxtRotate, "Rotate"}, 
	{ TxtRotate90Degree, "90 Degree"}, 
	{ TxtRotate180Degree, "180 Degree"}, 
	{ TxtRotate270Degree, "270 Degree"}, 
	{ TxtZoom, "Zoom"}, 
	//May 03, 2006    DRT: OMAPS00076856 xdeepadh
	{ TxtZoomIn, "Zoom In"}, 
	{ TxtZoomOut,"Zoom Out"}, 
	{ TxtZoomInMax, "Zoom In Max "}, 
	{ TxtZoomOutMax, "Zoom Out Max"}, 
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
{TxtUSB, "USB"},
	{TxtUSBEnumeration, "USB Enumeration"},
	 {  TxtUSBMode, "USB mode"},
 	 {  TxtContinue, "Continue ?"},	
	{ TxtUSBDisconnect ,"Disconnecting"},
	{TxtUSBMS,"USB MS"},
	{TxtUSBTrace,"USB Trace"},
	{TxtUSBFax,"USB Fax"}, 
{TxtPSShutdown,"PS Shutdown"},
{TxtNoPSShutdown,"No PS shutdown"},
	//xashmic 27 Sep 2006, OMAPS00096389 
	{TxtUSBFaxnTrace,"USB Fax and Trace"},
	{TxtUSBMSnTrace,"USB MS and Trace"},
	{TxtUSBFaxnMS,"USB Fax and MS"},
	{TxtUSBMSPorts,"USB Ports"},
	{TxtPS,"PS"},
	{TxtPopupMenu,"Pop up menu"},
#endif

    // x0021334 10 May, 2006 DR: OMAPS00075379
	{ TxtPending, "Pending"},

    // x0021334 02 June, 2006 DR: OMAPS00078005 
#ifdef FF_TIMEZONE
	{ TxtTimeUpdate, "New Time/Date received."},
	{ TxtTimeUpdateConfirm, "Want to update?"},
	{ TxtNitzTimeUpdate, "Network Time Update"},
#endif
	//x0pleela 30 Aug, 2006 DR: OMAPS00091250
	{ TxtNoMEPD, "MEPD not present"},

//x0pleela 24 Aug, 2006  DR:OMAPS00083503
//Adding new strings to display if the characters are UCS2 in USSD transactions
#ifdef MMI_LITE
	{TxtUSSDMsgRxd, "USSD Message received" },
	{TxtFontNotSupported, "Font Not Supported"},
#endif
//x0pleela 05 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	{TxtPhoneLock, "Phone Lock"},
	{TxtPhLockEnable, "Lock Enable"},
	{TxtPhLockAutomatic, "Automatic"},
	{TxtPhLockAutoOn, "On"},
	{TxtPhLockAutoOff, "Off"},
	{TxtPhLockChangeCode,"Change Unlock Code"},
	{TxtEnterPhUnlockCode, "Enter Phone Unlock Code"},
	{TxtEnterOldUnlockCode, "Enter Old Unlock Code"},
	{TxtEnterNewUnlockCode, "Enter New Unlock Code"},
	{TxtAlreadyEnabled,"Already enabled"},
	{TxtAlreadyDisabled, "Already disabled"},
	{TxtAutoLockDisable, "Auto Ph Lock Disable"},	
	{TxtAutoPhone, "Automatic Phone"},	
	{TxtSuccess, "Success"},	
	{TxtNotLocked, "Phone can't be locked"},
#endif /* FF_PHONE_LOCK */
/*OMAPS00098881 (Removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
	{TxtScreenUpdate,"Screen Update"},
#endif
//Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
#ifdef FF_MMI_CAMERA_APP
	{TxtNotEnough,"Not Enough"},
	{TxtCamMemory,"Memory"},
#endif
	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
	{TxtFwdIncomingTTYCall, "Fwd. Inc. TTY Call"},/* String for Fwd Incoming TTY  call */
	{TxtFwdIncomingCall, "Fwd. Inc. Call"},/* String for Fwd Incoming  call */
	
	//06 July, 2007 DR: OMAPS00137334 x0062172 Syed
	{TxtConferenceFull, "Conference Full"},  /* String for Conference Full notification */
	
#ifdef FF_MMI_FILEMANAGER 
	/* File Management related strings */
	{TxtFileMgmt, "File Management"},   
	{TxtFileList, "FileLists"},
	{TxtFFS, "NOR" },	
	{TxtNORMS, "NOR-MS" },	
	{TxtNAND, "NAND" },	
	{TxtTflash, "T-Flash" },	
	{TxtOpen,"Open"},
	{TxtFormat,"Format"},
	{TxtProperties,"Properties"},
	{TxtImageFiles, "Image"},
	{TxtAudioFiles,"Audio"},
	{TxtFile,"File"},
	{TxtDirectory,"Directory"},
	{TxtFreeSpace,"Free Space"},
	{TxtUsedSpace, "Used Space"},
	{TxtThumbnail, "Thumbnail"},
	{TxtExists, "Exists"},
	{TxtCreateDir, "Create Directory"},
	{TxtCreated, "Created"},
	{TxtNotEmpty, "Not Empty"},
	{TxtMemorystat,"Memory Status"},
	{TxtSourceDest, "Source and Destination"},
	{TxtSame, "Same"},
	{TxtAudNotAllowed,"Not Allowed"},
	{TxtAudAudioError,"Audio"},
	{TxtAudDenied,"Denied"},
	{TxtAudPlayerError,"Player"},
	{TxtAudFFSError," FFS"},
	{TxtAudMessagingError,"Messaging"},
	{TxtAudNotReady,"Not Ready"},
	{TxtAudInvalidParam,"Invalid Param"},
	{TxtAudInternalError,"Internal"},
	{TxtAudMemoryError,"Memory"},
	{TxtAudNotSupported,"Not Supported"},	
	{TxtCopying," Copying ..."},
	{ TxtFileNotLoaded,"No Files loaded"},
	{ TxtFileViewer, "FileViewer"}, 
	{ TxtRename, "Rename"}, 
	{TxtDeleteConfirm,"Delete?"},
	{TxtFileTooBig,"File Too Big"},
	{TxtFileExists,"File exists"},//Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh .
	{TxtWrongFilename,"Wrong Name"}, //Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,"Wrong extn"},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtStorageDevice,"Storage Device"},
	{TxtPBar,"Progress Bar On/Off"},
#endif
//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)
    {TxtTo,"To"},
/* Mar 26 2007 ER:OMAPS00116772 x0061088(Prachi)*/
#ifdef FF_MMI_CAMERA_APP
    {TxtAutoSave,"Auto Save"},
    {TxtAutoSaveOn,"On"},
    {TxtAutoSaveOff,"Off"},
    {TxtSoftSave,"Save"},
    {TxtSoftCancel,"Cancel"},
#endif /* FF_MMI_CAMERA_APP */

//added by prachi 
#if CAM_SENSOR == 1
    {TxtCamMenuResolution,"Change Resolution"},
    {TxtCamVga,"VGA"},
    {TxtCamQcif,"QCIF"},
    {TxtCamSxga,"SXGA"},

    //19 Jul, 2007  DR: OMAPS00139535 x0080426 Sinara
    //Braces were mismatching
    {TxtCamNot,"Not"},
    {TxtCamSupported,"Supported"},
#endif  
        { 0, 0 }
};

#else

//DON'T TOUCH THE FOLLOWING LINE...
//Language = "Chinese"

/*MC, SPR 1526, prepended chinese string with Unicode tag*/
	const unsigned char stringTest[] = {0x00,0x48,0x00,0x49,0x4E,0x33};
    const unsigned char chineseTxtNull[] = {0x80, 0x00, 0x00,0x00};

//x0pleela 24 Aug, 2006  DR:OMAPS00083503
//Adding new strings to display if the characters are UCS2 in USSD transactions
#ifdef MMI_LITE
const unsigned char chineseTxtUSSDMsgRxd[] = {0x80, 0x00, 0x00,'U',0x00,'S',0x00,'S',0x00,'D',0x00,' ',0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,'g',0x00,'e',0x00,' ',0x00,'r',0x00,'e',0x00,'c',0x00,'e',0x00,'i',0x00,'v',0x00,'e',0x00,'d',0x00,0x00}; 
const unsigned char chineseTxtFontNotSupported[] = {0x80, 0x00, 0x00,'F',0x00,'o',0x00,'n',0x00,'t',0x00,' ',0x00,'N',0x00,'o',0x00,'t',0x00,' ',0x00,'S',0x00,'u',0x00,'p',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,'e',0x00,'d',0x00,0x00}; 
#endif


//x0pleela 14 June, 2006  DR: OMAPS00070657
//Edit Line for ALS feature of CPHS
#ifdef FF_CPHS
	const unsigned char chineseTxtEditLineName[] = {0x80, 0x00, 0x00,'E',0x00,'d',0x00,'i',0x00,'t',0x00,' ',0x00,'L',0x00,'i',0x00,'n',0x00,'e',0x00,' ',0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,0x00}; 
#endif
//x0pleela 30 Aug, 2006 DR: OMAPS00091250
//to display string MEPD not present
const unsigned char chineseTxtNoMEPD[] = {0x80, 0x00, 0x00,'M',0x00,'E',0x00,'P',0x00,'D',0x00,' ',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'p',0x00,'r',0x00,'e',0x00,'s',0x00,'e',0x00,'n',0x00,'t',0x00,0x00}; 

//x0pleela 20 Mar, 2006   ER: OMAPS00067709
//Added the following strings used in menu items to support Voice buffering feature
#ifdef FF_PCM_VM_VB
    const unsigned char chineseTxtStopFailed[] = {0x80, 0x00, 0x00,'S',0x00,'t',0x00,'o',0x00,'p',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'e',0x00,'d',0x00, 0x00}; 
    const unsigned char chineseTxtAlready[] = {0x80, 0x00, 0x00,'A',0x00,'l',0x00,'r',0x00,'e',0x00,'a',0x00,'d',0x00,'y',0x00, 0x00}; 
    const unsigned char chineseTxtPcmVoiceMemo[] = {0x80, 0x00, 0x00,'P',0x00,'C',0x00,'M',0x00,' ',0x00,'V',0x00,'o',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,'M',0x00,'e',0x00,'m',0x00,'o',0x00, 0x00}; 
    const unsigned char chineseTxtVoiceBuffering[] = {0x80, 0x00, 0x00,'V',0x00,'o',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,'B',0x00,'u',0x00,'f',0x00,'f',0x00,'e',0x00, 'r',0x00,'i',0x00,'n',0x00,'g',0x00,0x00}; 
    const unsigned char chineseTxtEditNum[] = {0x80, 0x00, 0x00,'E',0x00,'d',0x00,'i',0x00,'t',0x00,' ',0x00,'n',0x00,'u',0x00,'m',0x00,'b',0x00,'e',0x00,'r',0x00, 0x00}; 
    const unsigned char chineseTxtStartRec[] = {0x80, 0x00, 0x00,'S',0x00,'t',0x00,'a',0x00,'r',0x00,'t',0x00,' ',0x00,'R',0x00,'e',0x00,'c',0x00,'o',0x00,'r',0x00, 'd',0x00, 'i',0x00, 'n',0x00, 'g',0x00, 0x00}; 
    const unsigned char chineseTxtStopRec[] = {0x80, 0x00, 0x00,'S',0x00,'t',0x00,'o',0x00,'p',0x00,' ',0x00,'R',0x00,'e',0x00,'c',0x00,'o',0x00,'r',0x00, 'd',0x00, 'i',0x00, 'n',0x00, 'g',0x00, 0x00}; 
 #endif

#ifdef SIM_PERS    
	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//For new category "Blocked Network" 
	 const unsigned char chineseTxtBlkNetwork[] = {0x80, 0x00, 0x00,'B',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,' ', 0x00,'N',0x00,'e',0x00,'t',0x00,'w',0x00,'o',0x00,'r',0x00,'k',0x00, 0x00}; 
	//string to display "Enter Blocked Network password"
	 const unsigned char chineseTxtEnterPbLock[] = {0x80, 0x00, 0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'B',0x00,'l', 0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,' ',0x00,'N',0x00,'W',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00, 0x00}; 

	//x0pleela 20 July, 2006    ER: OMAPS00087586, OMAPS00087587
       const unsigned char chineseTxtBusy[] = {0x80, 0x00, 0x00,'B',0x00,'u',0x00,'s',0x00,'y',0x00,0x00};  //Displays "Busy" if timer is enabled for wrong entry of password
       const unsigned char chineseTxtMasterUnlock[] = {0x80, 0x00, 0x00,'M',0x00,'a',0x00,'s',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'K', 0x00,'e',0x00,'y',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00, 0x00}; //For menu option Master unlock
       const unsigned char chineseTxtEnterMasterKey[] = {0x80, 0x00, 0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'M',0x00,'a',0x00,'s',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'K', 0x00,'e',0x00,'y',0x00, 0x00};    //Text for the editor to enter master unlock key
       const unsigned char chineseTxtUnblockME[] = {0x80, 0x00, 0x00,'U',0x00,'n',0x00,'b',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'M',0x00,'E',0x00, 0x00};    //For menu option Unblock ME
	
    const unsigned char chineseTxtUnblockCode[] = {0x80, 0x00, 0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'U',0x00,'n',0x00,'b',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00, 0x00}; //x0pleela 16 May, 2006  DR: OMAPS00067919
#endif    
    const unsigned char chineseTxtTimerFlag[] = {0x80, 0x00, 0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,'r',0x00,'F',0x00,'l',0x00,'a',0x00,'g',0x00, 0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtETSIFlag[]= {0x80,0x00,0x00,'E',0x00,'T',0x00,'S',0x00,'I',0x00,' ',0x00,'F',0x00,'l',0x00,'a',0x00,'g',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtAirtelFlag[]= {0x80,0x00,0x00,'A',0x00,'i',0x00,'r',0x00,'T',0x00,'e',0x00,'l',0x00,' ',0x00,'I',0x00,'n',0x00,'d',0x00,'.',0x00,' ',0x00,'F',0x00,'l',0x00,'a',0x00,'g',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterOldPsLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'O',0x00,'l',0x00,'d',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterPsLockNew[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterPsLockconf[]= {0x80,0x00,0x00,'C',0x00,'o',0x00,'n',0x00,'f',0x00,'i',0x00,'r',0x00,'m',0x00,' ',0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtLockEnabled[]= {0x80,0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'E',0x00,'n',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtLockDisabled[]= {0x80,0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'D',0x00,'i',0x00,'s',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtPersonalization[]= {0x80,0x00,0x00,'P',0x00,'e',0x00,'r',0x00,'s',0x00,'o',0x00,'n',0x00,'a',0x00,'l',0x00,'i',0x00,'z',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtSupplementaryInfo[]= {0x80,0x00,0x00,'S',0x00,'u',0x00,'p',0x00,'p',0x00,'l',0x00,'e',0x00,'m',0x00,'e',0x00,'n',0x00,'t',0x00,'a',0x00,'r',0x00,'y',0x00,' ',0x00,'I',0x00,'n',0x00,'f',0x00,'o',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtPersonalize[]= {0x80,0x00,0x00,'P',0x00,'e',0x00,'r',0x00,'s',0x00,'o',0x00,'n',0x00,'a',0x00,'l',0x00,'i',0x00,'z',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtUnlockCategory[]= {0x80,0x00,0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'a',0x00,'t',0x00,'e',0x00,'g',0x00,'o',0x00,'r',0x00,'y',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtLockCategory[]= {0x80,0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'a',0x00,'t',0x00,'e',0x00,'g',0x00,'o',0x00,'r',0x00,'y',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtResetFC[]= {0x80,0x00,0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'u',0x00,'r',0x00,'e',0x00,' ',0x00,'c',0x00,'o',0x00,'u',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtGetFC[]= {0x80,0x00,0x00,'G',0x00,'e',0x00,'t',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'u',0x00,'r',0x00,'e',0x00,' ',0x00,'C',0x00,'o',0x00,'u',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtMaxFC[]= {0x80,0x00,0x00,'M',0x00,'a',0x00,'x',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'u',0x00,'r',0x00,'e',0x00,' ',0x00,'C',0x00,'o',0x00,'u',0x00,'n',0x00,'t',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtCorporate[]= {0x80,0x00,0x00,'C',0x00,'o',0x00,'r',0x00,'p',0x00,'o',0x00,'r',0x00,'a',0x00,'t',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtServiceProvider[]= {0x80,0x00,0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,'P',0x00,'r',0x00,'o',0x00,'v',0x00,'i',0x00,'d',0x00,'e',0x00,'r',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtNetworkSubset[]= {0x80,0x00,0x00,'N',0x00,'e',0x00,'t',0x00,'w',0x00,'o',0x00,'k',0x00,' ',0x00,'S',0x00,'u',0x00,'b',0x00,'s',0x00,'e',0x00,'t',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtLockActivated[]= {0x80,0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'A',0x00,'c',0x00,'t',0x00,'i',0x00,'v',0x00,'a',0x00,'t',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtLockDeactivated[]= {0x80,0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'D',0x00,'e',0x00,'A',0x00,'c',0x00,'t',0x00,'i',0x00,'v',0x00,'a',0x00,'t',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterNLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'N',0x00,'e',0x00,'t',0x00,'w',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'o',0x00,'r',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
	const unsigned char chineseTxtEnterNsLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'N',0x00,'e',0x00,'t',0x00,'w',0x00,' ',0x00,'S',0x00,'u',0x00,'b',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterSpLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'.',0x00,'P',0x00,'r',0x00,'o',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterCLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'C',0x00,'o',0x00,'r',0x00,'p',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEnterPsLock[]= {0x80,0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'S',0x00,'I',0x00,'M',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'o',0x00,'r',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
     const unsigned char chineseTxtPsLockConfWrong[]= {0x80,0x00,0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'o',0x00,'r',0x00,'d',0x00,' ',0x00,'d',0x00,'o',0x00,'e',0x00,'s',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtPsLockConfwrong2[]= {0x80,0x00,0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'M',0x00,'a',0x00,'t',0x00,'c',0x00,'h',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtPsLockChanged[]= {0x80,0x00,0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,' ',0x00,'C',0x00,'h',0x00,'a',0x00,'n',0x00,'g',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtCounterReset[]= {0x80,0x00,0x00,'C',0x00,'o',0x00,'u',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,0x00}; //x0035544 07-11-2005
     const unsigned char chineseTxtPassfailed[]= {0x80,0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n',0x00,'g',0x00,'e',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'e',0x00,'d',0x00,0x00};  //x0035544 07-11-2005
     const unsigned char chineseTxtUnblock[]= {0x80,0x00,0x00,'U',0x00,'n',0x00,'b',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'i',0x00,'n',0x00,'g',0x00,' ',0x00,'S',0x00,'u',0x00,'c',0x00,'c',0x00,'e',0x00,'s',0x00,'s',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtAlreadyLocked[]= {0x80,0x00,0x00,'C',0x00,'a',0x00,'t',0x00,'e',0x00,'g',0x00,'o',0x00,'r',0x00,'y',0x00,' ',0x00,'A',0x00,'l',0x00,'r',0x00,'e',0x00,'d',0x00,'y',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtAlreadyUnlocked[]= {0x80,0x00,0x00,'C',0x00,'a',0x00,'t',0x00,'e',0x00,'g',0x00,'o',0x00,'r',0x00,'y',0x00,' ',0x00,'A',0x00,'l',0x00,'r',0x00,'e',0x00,'d',0x00,'y',0x00,' ',0x00,'U',0x00,'L',0x00,'c',0x00,'k',0x00,'d',0x00,0x00};//x0035544 07-11-2005
    const unsigned char chineseTxtPhoneUnblocked[]= {0x80,0x00,0x00,'P',0x00,'h',0x00,'o',0x00,'n',0x00,'e',0x00,' ',0x00,'U',0x00,'n',0x00,'b',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtPhoneBlocked[]= {0x80,0x00,0x00,'P',0x00,'h',0x00,'o',0x00,'n',0x00,'e',0x00,' ',0x00,'B',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtGetFailFC[]= {0x80,0x00,0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'F',0x00,'C',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,' ',0x00,'A',0x00,'t',0x00,'t',0x00,'L',0x00,'f',0x00,'t',0x00,0x00}; //x0035544 07-11-2005
	const unsigned char chineseTxtMaxFailFC[]= {0x80,0x00,0x00,'M',0x00,'a',0x00,'x',0x00,' ',0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'F',0x00,'C',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,' ',0x00,'A',0x00,'t',0x00,'t',0x00,0x00};//{0x80,0x00,0x00,Max Reset FC Fail Attempts0x00,0x00};//x0035544 07-11-2005
	const unsigned char chineseTxtGetSuccFC[]= {0x80,0x00,0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'F',0x00,'C',0x00,' ',0x00,'S',0x00,'u',0x00,'c',0x00,'c',0x00,' ',0x00,'A',0x00,'t',0x00,'t',0x00,'e',0x00,'m',0x00,'p',0x00,'t',0x00,0x00};//{0x80,0x00,0x00,Reset FC Succ Attempts left0x00,0x00};//x0035544 07-11-2005
	const unsigned char chineseTxtMaxSucFC[]= {0x80,0x00,0x00,'M',0x00,'a',0x00,'x',0x00,'R',0x00,'S',0x00,' ',0x00,'F',0x00,'C',0x00,' ',0x00,'S',0x00,'u',0x00,'c',0x00,'c',0x00,' ',0x00,'A',0x00,'t',0x00,'t',0x00,'e',0x00,'m',0x00,'p',0x00,'t',0x00,0x00};//{0x80,0x00,0x00,Max Reset FC Succ Attempts0x00,0x00};//x0035544 07-11-2005
    const unsigned char chineseTxtViewAll[]= {0x80,0x00,0x00,'V',0x00,'i',0x00,'e',0x00,'w',0x00,' ',0x00,'c',0x00,'l',0x00,'l',0x00,0x00};//{0x80,0x00,0x00,View All0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtEmergencyNumbers[]= {0x80,0x00,0x00,'E',0x00,'m',0x00,'e',0x00,'r',0x00,'g',0x00,'e',0x00,'n',0x00,'c',0x00,'y',0x00,' ',0x00,'N',0x00,'u',0x00,'m',0x00,'b',0x00,'e',0x00,'r',0x00,'s',0x00,0x00,0x00};//x80,0x00,0x00,Emergency Numbers0x00,0x00};//x0035544 07-11-2005
    const unsigned char chineseTxtNotImplemented[] = {0x80, 0x00, 0x4E,0x0D,0x53,0xEF,0x4F,0x98,0x67,0x0D,0x52,0xA1,0x00,0x00};
    const unsigned char chineseTxtManu[] = {0x80, 0x00, 0x00,0x43,0x00,0x4F,0x00,0x4E,0x00,0x44,0x00,0x41,0x00,0x54,0x00,0x00};
    const unsigned char chineseTxtNetSearch[] = {0x80, 0x00, 0x67,0xE5,0x5B,0xFB,0x00,0x00};
    const unsigned char chineseTxtEnterPin1[] = {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x31,0x00,0x00};
    const unsigned char chineseTxtEnterPin2[]= {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00}; 
    const unsigned char chineseTxtEnterPuk1[] = {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x50,0x00,0x55,0x00,0x4B,0x00,0x31,0x00,0x00}; 
    const unsigned char chineseTxtEnterPuk2[] = {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x50,0x00,0x55,0x00,0x4B,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtInvalidCard[] = {0x80, 0x00, 0x00,0x49,0x00,0x6E,0x00,0x76,0x00,0x61,0x00,0x6C,0x00,0x69,0x00,0x64,0x00,0x2E,0x00,0x43,0x00,0x61,0x00,0x72,0x00,0x64,0x00,0x00}; //English
    const unsigned char chineseTxtNoCard[] = {0x80, 0x00, 0x65,0xE0,0x00,0x53,0x00,0x49,0x00,0x4D,0x00,0x00};
// Nov 24, 2005, a0876501, DR: OMAPS00045909
    const unsigned char chineseTxtInvalidIMEI[] = {0x80, 0x00, 0x00,'I',0x00,'n',0x00,'v',0x00,'a',0x00,'l',0x00,'i',0x00,'d',0x00,'I',0x00,'M',0x00,'E',0x00,'I',0x00,0x00};
    //const unsigned char chineseTxtSoftCall[] = {0x80, 0x00, 0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtSoftCall[] = {0x80, 0x00, 0x54,0x7c,0x53, 0xeb, 0x00,0x00};
    const unsigned char chineseTxtSoftOK[] = {0x80, 0x00, 0x78,0x6E,0x8B,0xA4,0x00,0x00};
    const unsigned char chineseTxtSoftMenu[] = {0x80, 0x00, 0x83,0xDC,0x53,0x55,0x00,0x00};
    const unsigned char chineseTxtHelp[] = {0x80, 0x00, 0x5E,0x2E,0x52,0xA9,0x00,0x00};
    const unsigned char chineseTxtSoftContacts[] = {0x80, 0x00, 0x80,0x54,0x7C,0xFB,0x00,0x00};
    const unsigned char chineseTxtDownload[] = {0x80, 0x00, 0x50,0xA8,0x5B,0x58,0x00,0x00}; 
    const unsigned char chineseTxtPlay[] = {0x80, 0x00, 0x62,0xE8,0x65,0x3E,0x00,0x00};
    const unsigned char chineseTxtNotEnough[] = {0x80, 0x00, 0x4E,0x0D,0x59,0x1F,0x00,0x00};
    const unsigned char chineseTxtCharacter[] = {0x80, 0x00, 0x65,0x87,0x5B,0x57,0x00,0x00}; 
    const unsigned char chineseTxtMemUsage[] = {0x80, 0x00, 0x8B,0xB0,0x5F,0xC6,0x7A,0x7A,0x95,0xF4,0x4E,0x58,0x00,0x00};
    const unsigned char chineseTxtOperator[] =  {0x80, 0x00, 0x64,0xCD,0x4F,0x5C,0x54,0x58,0x00,0x00};
    const unsigned char chineseTxtMessages[] = {0x80, 0x00, 0x77,0xED,0x4F,0xE1,0x00,0x00}; 
    const unsigned char chineseTxtOrganiser[] = {0x80, 0x00, 0x8B,0XB0,0X4E,0X8B,0X67,0X2C,0x00,0x00}; 
    const unsigned char chineseTxtAlarm[] = {0x80, 0x00, 0x95,0xF9,0x94,0xC3,0x00,0x00};
    const unsigned char chineseTxtSounds[] = {0x80, 0x00, 0x58,0xF0,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtSettings[] = {0x80, 0x00, 0x8B,0xBE,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtServices[] = {0x80, 0x00, 0x67,0x0D,0x52,0xA1,0x00,0x00};
    const unsigned char chineseTxtExtra[] = {0x80, 0x00, 0x98,0x9D,0x59,0x16,0x00,0x00};
    const unsigned char chineseTxtSend[] = {0x80, 0x00, 0x53,0xD1,0x78,0x01,0x90,0x01,0x00,0x00}; 
    const unsigned char chineseTxtSent[] = {0x80, 0x00, 0x5D,0xF1,0x53,0xD1,0x90,0x01,0x00,0x00};
    const unsigned char chineseTxtNotSent[] = {0x80, 0x00, 0x67,0x2A,0x53,0xD1,0x90,0x01,0x00,0x00};
    const unsigned char chineseTxtNoMessages[] = {0x80, 0x00, 0x65,0xE0,0x77,0xED,0x4F,0xE1,0x00,0x00}; 
    const unsigned char chineseTxtEnterMessage[] = {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x00,0x2C,0x00,0x4D,0x00,0x65,0x00,0x73,0x00,0x73,0x00,0x61,0x00,0x67,0x00,0x65,0x00,0x00};
    const unsigned char chineseTxtServiceCentre[] = {0x80, 0x00, 0x67,0x0D,0x52,0xA1,0x4E,0x2D,0x5F,0xC3,0x00,0x00};
    const unsigned char chineseTxtPleaseWait[] = {0x80, 0x00, 0x8B,0xF7,0x7B,0x49,0x50,0x19,0x00,0x00};
    const unsigned char chineseTxtSmsReceived[] =  {0x80, 0x00, 0x65,0x36,0x52,0x30,0x75,0x59,0x8A,0x00,0x00,0x00};
    const unsigned char chineseTxtSmsEnterNumber[] = {0x80, 0x00, 0x8F,0x93,0x51,0x65,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtRead[] =  {0x80, 0x00, 0x96,0x05,0x8B,0xFB,0x00,0x00};
     const unsigned char chineseTxtRetrieve[]= {0x80,0x00,0x00,'R',0x00,'e',0x00,'t',0x00,'r',0x00,'i',0x00,'e',0x00,'v',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
    const unsigned char chineseTxtDeleteAll[] = {0x80, 0x00, 0x51,0x68,0x90,0xE8,0x52,0x20,0x96,0x64,0x00,0x00};
    const unsigned char chineseTxtSave[] = {0x80, 0x00, 0x50,0xA8,0x5B,0x58,0x00,0x00};
    const unsigned char chineseTxtVoicemail[] = {0x80, 0x00, 0x8B,0xED,0x97,0xF3,0x4f,0xe1,0x7b,0xb1,0x00,0x00};
    const unsigned char chineseTxtBroadcast[] =  {0x80, 0x00,0x4F,0xE1,0x60,0x6F,0x53,0xF0,0x00,0x00}; 
    const unsigned char chineseTxtNew[] = {0x80, 0x00,0x65,0xB0,0x4f,0xe1,0x60,0x6f,0x00,0x00};
    const unsigned char chineseTxtApplications[] = {0x80, 0x00,0xCF,0x5E,0xCF,0x5D,0x00,0x00};
#ifndef FF_NO_VOICE_MEMO
    const unsigned char chineseTxtVoiceMemo[] = {0x80, 0x00,0x8B,0xED,0x97,0xF3,0x59,0x07,0x5f,0xD8,0x00,0x00};
#endif
    const unsigned char chineseTxtCbEnterMid[] = {0x80, 0x00,0x00,0x43,0x00,0x42,0x00,0x20,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x4D,0x00,0x49,0x00,0x44,0x00,0x00}; // No Translation
    const unsigned char chineseTxtSmsListFull[] = {0x80, 0x00,0x4F,0xE1,0x7B,0xB1,0x5D,0xF1,0xBE,0xE1,0x00,0x00}; 
    const unsigned char chineseTxtNewCbMessage[] = {0x80, 0x00,0x00,0x31,0x65,0xB0,0x51,0x6C,0x51,0x71,0x8B,0xAF,0x60,0x6F,0x00,0x00};
    const unsigned char chineseTxtSaved[] = {0x80, 0x00,0x5D,0xF1,0x50,0xA8,0x5B,0x58,0x00,0x00};
    const unsigned char chineseTxtNotSaved[] = {0x80, 0x00,0x67,0x2A,0x50,0xA8,0x5B,0x58,0x00,0x00};
    const unsigned char chineseTxtDelete[] = {0x80, 0x00,0x52,0x20,0x96,0x64,0x00,0x00};
    const unsigned char chineseTxtReply[] = {0x80, 0x00,0x56,0xDE,0x4F,0xE1,0x00,0x00};
    const unsigned char chineseTxtStoreNumber[] = {0x80, 0x00,0x50,0xA8,0x5B,0x58,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtTimeReceived[] = {0x80, 0x00,0x63,0xA5,0x65,0x36,0x65,0xF6,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtForward[] = {0x80, 0x00,0x8F,0x6C,0x90,0x01,0x00,0x00};
    const unsigned char chineseTxtActivate[] = {0x80, 0x00,0x5F,0x00,0x8D,0x77,0x00,0x00};
    const unsigned char chineseTxtTopics[] = {0x80, 0x00,0x98,0x79,0x76,0xEE,0x00,0x00};
    const unsigned char chineseTxtConsult[] = {0x80, 0x00,0x8C,0x18,0x8B,0xE2,0x00,0x00};
    const unsigned char chineseTxtAddNew[] = {0x80, 0x00,0x65,0xB0,0x58,0x9E,0x00,0x00};
    const unsigned char chineseTxtAddNewEvent[] = {0x80, 0x00,0x65,0xB0,0x58,0x9E,0x98,0x79,0x76,0xEE,0x00,0x00};
    const unsigned char chineseTxtModifyEvent[] = {0x80, 0x00,0x4F,0xEE,0x65,0x39,0x98,0x79,0x76,0xEE,0x00,0x00};
    const unsigned char chineseTxtDeleteEvent[] = {0x80, 0x00,0x52,0x20,0x96,0x64,0x98,0x79,0x76,0xEE,0x00,0x00};
    const unsigned char chineseTxtShowEvent[] = {0x80, 0x00,0x66,0x3E,0x79,0x3A,0x98,0x79,0x76,0xEE}; 
    const unsigned char chineseTxtContacts[] = {0x80, 0x00,0x62,0xE8,0x54,0x7C,0x00,0x00};
    const unsigned char chineseTxtSearchName[] = {0x80, 0x00,0x67,0xE5,0x5B,0xFB,0x00,0x00};
    const unsigned char chineseTxtMemory[] = {0x80, 0x00,0x8B,0xB0,0x61,0xB6,0x72,0xb6,0x60,0x01,0x00,0x00};
    const unsigned char chineseTxtMyNumber[] = {0x80, 0x00,0x67,0x2C,0x67,0x3A,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtMyNumberFull[] = {0x80, 0x00,0x67,0x2C,0x67,0x3A,0x53,0xF7,0x78,0x01,0x00,0x00,0x80, 0x00,0x6E,0xE1,0x00,0x00};//SPR 31705 x0021308:RamG
    const unsigned char chineseTxtRinger[] = {0x80, 0x00,0x54,0xCD,0x94,0xC3,0x00,0x00};
    const unsigned char chineseTxtVolume[] = {0x80, 0x00,0x97,0xF3,0x91,0xCF,0x00,0x00};
    const unsigned char chineseTxtKeypadTones[] = {0x80, 0x00,0x63,0x09,0x95,0x2E,0x58,0xF0,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtNewMessage[] = {0x80, 0x00,0x65,0xB0,0x75,0x59,0x8A,0x00,0x00,0x00};
    const unsigned char chineseTxtNewBroadcast[] = {0x80, 0x00,0x65,0xB0,0x65,0xB0,0x51,0x6C,0x51,0x71,0x8B,0xAF,0x60,0x6F,0x00,0x00};
    const unsigned char chineseTxtBatteryLow[] = {0x80, 0x00,0x75,0x35,0x6C,0x60,0x4E,0x0D,0x8D,0xC3,0x00,0x00};
    const unsigned char chineseTxtCreditLow[] = {0x80, 0x00,0x5B,0x58,0x6B,0x3E,0x4E,0x0D,0x8D,0xC3,0x00,0x00};
    const unsigned char chineseTxtAlarmClock[] = {0x80, 0x00,0x95,0xF9,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtKeypadLock[] = {0x80, 0x00,0x63,0x09,0x95,0x2E,0x95,0x01,0x00,0x00};
    const unsigned char chineseTxtAutoAnswer[] = {0x80, 0x00,0x81,0xEA,0x52,0xA8,0x56,0xDE,0x7B,0x54,0x00,0x00};
    const unsigned char chineseTxtTimeDate[] = {0x80, 0x00,0x65,0xF6,0x95,0xF4,0x65,0xE5,0x67,0x1F,0x00,0x00};
    const unsigned char chineseTxtLanguages[] = {0x80, 0x00,0x8B,0xED,0x8A,0x00,0x00,0x00};
    const unsigned char chineseTxtVoiceMail[] = {0x80, 0x00,0x8B,0xED,0x97,0xF3,0x4F,0xE1,0x7B,0xB1,0x00,0x00};
    const unsigned char chineseTxtPinCode[] = {0x80, 0x00,0x00,0x50,0x00,0x69,0x00,0x6E,0x00,0x20,0x00,0x43,0x00,0x6F,0x00,0x64,0x00,0x65,0x00,0x00};
    const unsigned char chineseTxtPin2Code[] = {0x80, 0x00,0x00,0x50,0x00,0x69,0x00,0x6E,0x00,0x32,0x00,0x20,0x00,0x43,0x00,0x6F,0x00,0x64,0x00,0x65,0x00,0x00};
    const unsigned char chineseTxtAutoredial[] = {0x80, 0x00,0x81,0xEA,0x52,0xA8,0x62,0xE8,0x53,0xF7,0x00,0x00};
    const unsigned char chineseTxtDTMF[] = {0x80, 0x00,0x00,0x44,0x00,0x54,0x00,0x4D,0x00,0x46,0x00,0x00};
    const unsigned char chineseTxtVibrator[] = {0x80, 0x00,0x63,0x2F,0x52,0xA8,0x00,0x00};
    const unsigned char chineseTxtOn[] = {0x80, 0x00,0x5F,0x00,0x00,0x00};
    const unsigned char chineseTxtOff[] = {0x80, 0x00,0x51,0x73,0x00,0x00};
    const unsigned char chineseTxtVibraThenRing[] = {0x80, 0x00,0x63,0x2F,0x52,0xA8,0x51,0x8D,0x54,0xCD,0x94,0xC3,0x00,0x00};
    const unsigned char chineseTxtCommands[] = {0x80, 0x00,0x63,0x07,0x4E,0xCA,0x00,0x00};
    const unsigned char chineseTxtNumbers[] = {0x80, 0x00,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtNetworkSel[] = {0x80, 0x00,0x7F,0x51,0x7E,0xDC,0x90,0x09,0x62,0xE9,0x00,0x00};
    const unsigned char chineseTxtSendID[] = {0x80, 0x00,0x96,0x90,0x85,0xCF,0x8E,0xAB,0x4E,0xFD,0x00,0x00};
    const unsigned char chineseTxtChargeTimers[] = {0x80, 0x00,0x65,0x36,0x8D,0x39,0x8B,0xB0,0x65,0xF6,0x00,0x00};
    const unsigned char chineseTxtCallDivert[] = {0x80, 0x00,0x54,0x7C,0x53,0xEB,0x8F,0x6C,0x63,0xA5,0x00,0x00};
    const unsigned char chineseTxtInfraRed[] = {0x80, 0x00,0x7E,0xA2,0x59,0x15,0x7E,0xBF,0x00,0x00};
    const unsigned char chineseTxtCallWaiting[] = {0x80, 0x00,0x90,0x1A,0x8B,0xDD,0x7B,0x49,0x5F,0x85,0x00,0x00}; 
    const unsigned char chineseTxtIfBusy[] = {0x80, 0x00,0x5F,0xD9,0x00,0x00};
    const unsigned char chineseTxtIfNoAnswer[] = {0x80, 0x00,0x65,0xE0,0x5E,0x94,0x7B,0x54,0x00,0x00};
    const unsigned char chineseTxtAllCalls[] = {0x80, 0x00,0x51,0x68,0x90,0xE8,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtCancelAll[] = {0x80, 0x00,0x51,0x68,0x90,0xE8,0x53,0xD6,0x6D,0x88,0x00,0x00};
    const unsigned char chineseTxtAll[] = {0x80, 0x00,0x51,0x68,0x90,0xE8,0x00,0x00};
    const unsigned char chineseTxtVoice[] = {0x80, 0x00,0x8B,0xED,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtFax[] = {0x80, 0x00,0x4F,0x20,0x77,0x1F,0x00,0x00};
    const unsigned char chineseTxtData[] = {0x80, 0x00,0x8D,0x44,0x65,0x99,0x00,0x00};
    const unsigned char chineseTxtStatus[] = {0x80, 0x00,0x72,0xB6,0x60,0x01,0x00,0x00};
    const unsigned char chineseTxtCancel[] = {0x80, 0x00,0x53,0xD6,0x6D,0x88,0x00,0x00};
    const unsigned char chineseTxtCalculator[] = {0x80, 0x00,0x8B,0xA1,0x7B,0x97,0x56,0x68,0x00,0x00};
    const unsigned char chineseTxtStopwatch[] = {0x80, 0x00,0x8B,0xA1,0x65,0xF6,0x56,0x68,0x00,0x00};
    const unsigned char chineseTxtEnterName[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x59,0xD3,0x54,0x0D,0x00,0x00};
    const unsigned char chineseTxtSoftBack[] = {0x80, 0x00,0x8F,0xD4,0x56,0xDE,0x00,0x00};
    const unsigned char chineseTxtSoftSelect[] = {0x80, 0x00,0x90,0x09,0x62,0xE9,0x00,0x00};
    const unsigned char chineseTxtSoftOptions[] = {0x80, 0x00,0x90,0x09,0x62,0xE9,0x98,0x79,0x76,0xEE,0x00,0x00}; 
    const unsigned char chineseTxtEmptyList[] = {0x80, 0x00,0x65,0xE0,0x8D,0x44,0x65,0x99,0x00,0x00}; 
    const unsigned char chineseTxtSendMessage[] = {0x80, 0x00,0x53,0xD1,0x90,0x01,0x77,0xED,0x4F,0xE1,0x00,0x00};
    const unsigned char chineseTxtChange[] = {0x80, 0x00,0x65,0x39,0x53,0xD8,0x00,0x00};
    const unsigned char chineseTxtEnterNumber[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtPhbkFull[] = {0x80, 0x00,0x75,0x35,0x8B,0xDD,0x67,0x2C,0x5D,0xF1,0x6E,0xE1,0x00,0x00};
    const unsigned char chineseTxtWriteError[] = {0x80, 0x00,0x4E,0x66,0x51,0x99,0x95,0x19,0x8B,0xEF,0x00,0x00};
    const unsigned char chineseTxtEntryDeleted[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x52,0x20,0x96,0x64,0x00,0x00}; 
    const unsigned char chineseTxtDetails[] = {0x80, 0x00,0x7E,0xC6,0x82,0x82,0x00,0x00}; 
    const unsigned char chineseTxtHold[] = {0x80, 0x00,0x4F,0xDD,0x75,0x59,0x00,0x00};
    const unsigned char chineseTxtUnhold[] = {0x80, 0x00,0x4E,0x0D,0x4F,0x0D,0xF5,0x59,0x00,0x00};
    const unsigned char chineseTxtMute[] = {0x80, 0x00,0x97,0x59,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtSendDTMF[] = {0x80, 0x00,0x53,0xD1,0x90,0x01,0x00,0x44,0x00,0x54,0x00,0x4D,0x00,0x46,0x00,0x00};
    const unsigned char chineseTxtMainMenu[] = {0x80, 0x00,0x4E,0x3B,0x76,0xEE,0x5F,0x55,0x00,0x00};
    const unsigned char chineseTxtStartConference[] = {0x80, 0x00,0x4F,0x1A,0x8B,0xAE,0x5F,0x00,0x59,0xCB,0x00,0x00};
    const unsigned char chineseTxtSwap[] = {0x80, 0x00,0x4E,0xA4,0x63,0x62,0x00,0x00}; 
    const unsigned char chineseTxtEndAll[] = {0x80, 0x00,0x51,0x68,0x90,0xE8,0x7E,0xD3,0x67,0x5F,0x00,0x00};
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Added a menu option. "End held" text is added to the language index
	const unsigned char chineseTxtEndHeld[] = {0x80, 0x00, 0x00,'E',0x00,'n',0x00,'d',0x00,' ',0x00,'h',0x00,'e',0x00,'l',0x00,'d',0x00,0x00};
    const unsigned char chineseTxtTransfer[] = {0x80, 0x00,0x8F,0x6C,0x79,0xFB,0x00,0x00};
    const unsigned char chineseTxtPrivateCall[] = {0x80, 0x00,0x79,0xC1,0x4E,0xBA,0x75,0x35,0x8B,0xDD,0x00,0x00};
    		const unsigned char chineseTxtReStartConference[] = {0x80, 0x00,0x91,0xcd,0x65,0xb0,0x5f,0x00,0x59,0xcb,0x4f,0x1a,0x8b,0xae,0x00,0x00};
    const unsigned char chineseTxtFind[] = {0x80, 0x00,0x5B,0xFB,0x62,0x7E,0x00,0x00};
    const unsigned char chineseTxtServiceNumbers[] = {0x80, 0x00,0x67,0x0D,0x52,0xA1,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtDone[] = {0x80, 0x00,0x5B,0x8C,0x62,0x10,0x00,0x00};
    const unsigned char chineseTxtFailed[] = {0x80, 0x00,0x59,0x31,0x8D,0x25,0x00,0x00};
#ifdef FF_MMI_MULTIMEDIA
    const unsigned char chineseTxtFileNotSupported[] = {0x80, 0x00,0x59,0x31,0x8D,0x25,0x00,0x00};
#endif
    const unsigned char chineseTxtEnterNewPin[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xB0,0x5B,0xC6,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtConfPin[] = {0x80, 0x00,0x78,0x6E,0x5B,0x9A,0x5B,0xC6,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtChangPin[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x5D,0xF1,0x65,0x39,0x53,0xD8,0x00,0x00};
    const unsigned char chineseTxtEnd[] = {0x80, 0x00,0x7E,0xD3,0x67,0x5E,0x00,0x00};
    const unsigned char chineseTxtPINFail[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x59,0x31,0x8D,0x25,0x00,0x00};
    const unsigned char chineseTxtCorrect[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x6B,0x63,0x78,0x6E,0x00,0x00};
    const unsigned char chineseTxtAutomatic[] = {0x80, 0x00,0x81,0xEA,0x52,0xA8,0x00,0x00};
    const unsigned char chineseTxtManual[] = {0x80, 0x00,0x4E,0xBA,0x5D,0xE5,0x00,0x00}; 
    const unsigned char chineseTxtFixedNames[] = {0x80, 0x00,0x56,0xFA,0x5B,0x9A,0x59,0xD3,0x54,0x0D,0x00,0x00};
    const unsigned char chineseTxtFactoryReset[] = {0x80, 0x00,0x5C,0x80,0x53,0x82,0x8B,0xBE,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtReceiveSMS[] = {0x80, 0x00,0x63,0xA5,0x65,0x36,0x00,0x53,0x00,0x4D,0x00,0x53,0x00,0x00};
    const unsigned char chineseTxtCenterNumber[] = {0x80, 0x00,0x4E,0x2D,0x5F,0xC3,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtChangePIN[] = {0x80, 0x00,0x65,0x39,0x53,0xD8,0x00,0x20,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x00};
    const unsigned char chineseTxtDisablePIN[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x51,0x73,0x95,0xED,0x00,0x00};
    const unsigned char chineseTxtEnablePIN[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x5F,0x00,0x8D,0xFF,0x00,0x00};
    const unsigned char chineseTxtChangePIN2[] = {0x80, 0x00,0x65,0x39,0x53,0xD8,0x00,0x20,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtPINincorrect[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x95,0x19,0x8B,0xEF,0x00,0x00};
    const unsigned char chineseTxtManualSelected[] = {0x80, 0x00,0x4E,0xBA,0x5D,0xE5,0x90,0x09,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtAutomaticSelected[] = {0x80, 0x00,0x81,0xEA,0x52,0xA8,0x90,0x09,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtSearching[] = {0x80, 0x00,0x67,0xE5,0x5B,0xFB,0x00,0x00}; 
    const unsigned char chineseTxtNoAccess[] = {0x80, 0x00,0x65,0xE0,0x6C,0xD5,0x8F,0xDB,0x51,0x65,0x00,0x00};
    const unsigned char chineseTxtNoNetworksAvailable[] = {0x80, 0x00,0x65,0xE0,0x7F,0x51,0x7E,0xDC,0x00,0x00};
    const unsigned char chineseTxtOldPIN[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xE7,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x31,0x00,0x00};
    const unsigned char chineseTxtOldPIN2[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xE7,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtEnterNewPIN2[] =  {0x80, 0x00,0x65,0xB0,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtNotAcc[] = {0x80, 0x00,0x65,0xE0,0x6C,0xD5,0x63,0xA5,0x53,0xD7,0x00,0x00}; 
    const unsigned char chineseTxtChPIN[] = {0x80, 0x00,0x4F,0xEE,0x65,0x39,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x00};
    const unsigned char chineseTxtChPIN2[] = {0x80, 0x00,0x4F,0xEE,0x65,0x39,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtAttempts[] = {0x80, 0x00,0x65,0x3E,0x5F,0x03,0x5C,0x1D,0x8B,0xD5,0x00,0x00};
    const unsigned char chineseTxtEnabled[] = {0x80, 0x00,0x00,0x50,0x00,0x49,0x00,0x4E,0x67,0x09,0x65,0x48,0x00,0x00};
    const unsigned char chineseTxtDisabled[] = {0x80, 0x00,0x00,0x50,0x00,0x49,0x00,0x4E,0x65,0xE0,0x65,0x48,0x00,0x00};
    const unsigned char chineseTxtPINOK[] = {0x80, 0x00,0x00,0x50,0x00,0x49,0x00,0x4E,0x6B,0x63,0x78,0x6E,0x00,0x00};
    const unsigned char chineseTxtSimBlocked[] =  {0x80, 0x00,0x00,0x53,0x00,0x4D,0x00,0x53,0x5D,0xF1,0x5C,0x01,0x95,0x01,0x00,0x00};
    const unsigned char chineseTxtNewPIN[] =  {0x80, 0x00,0x65,0xB0,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x00};
    const unsigned char chineseTxtAcc[] = {0x80, 0x00,0x5D,0xF1,0x63,0xA5,0x53,0xD7,0x00,0x00};
    const unsigned char chineseTxtNoNetwork[] = {0x80, 0x00,0x65,0xE0,0x7F,0x51,0x7E,0xDC,0x00,0x00};
    const unsigned char chineseTxtNo[] = {0x80, 0x00,0x54,0x26,0x00,0x00};
    const unsigned char chineseTxtEnterDate[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xE5,0x67,0x1F,0x00,0x00};
    const unsigned char chineseTxtEnterTime[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xF6,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtScratchPad[] = {0x80, 0x00,0x7B,0x14,0x8B,0xB0,0x67,0x2C,0x00,0x00}; 
    const unsigned char chineseTxtDeleted[] = {0x80, 0x00,0x5D,0xF1,0x52,0x20,0x96,0x64,0x00,0x00};
    const unsigned char chineseTxtActivated[] = {0x80, 0x00,0x5D,0xF2,0x5F,0x00,0x8D,0x77,0x00,0x00}; 
    const unsigned char chineseTxtDeActivated[] = {0x80, 0x00,0x5D,0xF2,0x51,0x73,0x95,0xED,0x00,0x00};
    const unsigned char chineseTxtDeactivate[] = {0x80, 0x00,0x51,0x73,0x95,0xED,0x00,0x00};
    const unsigned char chineseTxtNotActivated[] = {0x80, 0x00, 0x00, 'N', 0x00, 'o', 0x00, 't', 0x00, ' ', 0x00, 'A', 0x00, 'c', 0x00, 't', 0x00, 'i', 0x00, 'v', 0x00, 'a', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, 0x00, 0x00};//Dec 07, 2005  REF: OMAPS00050087 x0039928
    const unsigned char chineseTxtCheck[] = {0x80, 0x00,0x68,0xC0,0x67,0xE5,0x00,0x00};
    const unsigned char chineseTxtOther[] = {0x80, 0x00,0x51,0x76,0x5B,0x83,0x00,0x00};
    const unsigned char chineseTxtMidSaved[] = {0x80, 0x00,0x5D,0xF1,0x50,0xA8,0x5B,0x58,0x00,0x00};
    const unsigned char chineseTxtYes[] = {0x80, 0x00,0x66,0x2F,0x00,0x00};
    const unsigned char chineseTxtCallNumber[] = {0x80, 0x00,0x62,0xE8,0x53,0xD1,0x53,0xF7,0x78,0x01,0x00,0x00}; 
    const unsigned char chineseTxtEnterMid[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x4D,0x00,0x49,0x00,0x44,0x00,0x00};
    const unsigned char chineseTxtReadError[] = {0x80, 0x00,0x96,0x05,0x8B,0xFB,0x95,0x19,0x8B,0xEF,0x00,0x00};
    const unsigned char chineseTxtPressCToConfirm[] = {0x80, 0x00,0x63,0x09,0x00,0x43,0x95,0x2E,0x78,0x6E,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtUsed[] = {0x80, 0x00,0x5D,0xF2,0xCF,0x5D,0x7A,0x7A,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtOutgoing[] = {0x80, 0x00,0x62,0xE8,0x5C,0x80,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtMuted[] = {0x80, 0x00,0x65,0xE0,0x58,0xF0,0x00,0x00}; 
    const unsigned char chineseTxtCallEnded[] = {0x80, 0x00,0x90,0x1A,0x8B,0xDD,0x5D,0xF1,0x7E,0xD3,0x67,0x5F,0x00,0x00}; 
    const unsigned char chineseTxtIncomingCall[] = {0x80, 0x00,0x5D,0xF2,0x63,0xA5,0x75,0x35,0x8B,0xDD,0x00,0x00}; 
	//const unsigned char chineseTxtAnsweredCalls[] = {0x80, 0x00, 0x00, 'A', 0x00,'n', 0x00,'s', 0x00,'w', 0x00,'e', 0x00,'r', 0x00,'e', 0x00,'d', 0x00,' ', 0x00,'C', 0x00,'a', 0x00,'l', 0x00,'l', 0x00,'s', 0x00, 0x00,0x00 };
	const unsigned char chineseTxtAnsweredCalls[] = {0x80, 0x00,0x5d,0xf2,0x5e,0x94,0x7b,0x54,0x54,0x7c,0x53,0xeb,0x00, 0x00};
    const unsigned char chineseTxtNumberWithheld[] = {0x80, 0x00,0x4F,0xDD,0x75,0x59,0x76,0x84,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtAccept[] = {0x80, 0x00,0x63,0xA5,0x53,0xD7,0x00,0x00};
    const unsigned char chineseTxtReject[] = {0x80, 0x00,0x62,0xD2,0x7E,0xDD,0x00,0x00};
    const unsigned char chineseTxtCalling[] = {0x80, 0x00,0x6B,0x63,0x57,0x28,0x63,0xA5,0x90,0x1A,0x00,0x00};
    const unsigned char chineseTxtMissedCalls[] = {0x80, 0x00,0x67,0x2A,0x63,0xA5,0x75,0x35,0x8B,0xDD,0x00,0x00}; 
    const unsigned char chineseTxtMissedCall[] = {0x80, 0x00,0x67,0x2A,0x63,0xA5,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtCallFailed[] = {0x80, 0x00,0x62,0xE8,0x53,0xD1,0x59,0x31,0x8D,0x25,0x00,0x00};
    const unsigned char chineseTxtLocked[] = {0x80, 0x00,0x63,0x09,0x95,0x2E,0x5D,0xF1,0x95,0x01,0x00,0x00};
    const unsigned char chineseTxtUnmute[] = {0x80, 0x00,0x59,0x0D,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtCharging[] = {0x80, 0x00,0x51,0x45,0x75,0x35,0x8F,0xC7,0x7A,0x0B,0x4E,0x2D,0x00,0x00};
    const unsigned char chineseTxtChargComplete[] = {0x80, 0x00,0x51,0x45,0x75,0x35,0x5B,0x8C,0x62,0x10,0x00,0x00};
    const unsigned char chineseTxtOperationNotAvail[] = {0x80, 0x00,0x4E,0x0D,0x53,0xEF,0x4F,0x9B,0x67,0x0D,0x52,0xA1,0x00,0x00};
    const unsigned char chineseTxtAddToConference[] = {0x80, 0x00,0x52,0xA0,0x51,0x65,0x4F,0x1A,0x8B,0xAE,0x00,0x00};
    const unsigned char chineseTxtEmpty[] = {0x80, 0x00,0x65,0xE0,0x8D,0x44,0x65,0x99,0x00,0x00};
    const unsigned char chineseTxtSelectFromList[] = {0x80, 0x00,0x4E,0xCE,0x54,0x0D,0x53,0x55,0x91,0xCC,0x90,0x09,0x00,0x00};
    const unsigned char chineseTxtEnterNetworkCode[] = {0x80, 0x00,0x7F,0x51,0x7E,0xDC,0x53,0xF7,0x78,0x01,0x00,0x00};
    /* Marcus: Issue 1135: 12/09/2002: Start */
    const unsigned char chineseTxtMCCMNC[] = {0x80, 0x00,0x56,0xFD,0x5B,0xB6,0x78,0x01,0x00,0x2F,0x7F,0x51,0x7E,0xDC,0x78,0x01,0x00,0x00};
    /* Marcus: Issue 1135: 12/09/2002: End */
#ifndef FF_NO_VOICE_MEMO
    const unsigned char chineseTxtRecord[] = {0x80, 0x00,0x5F,0x55,0x97,0xF3,0x00,0x00};
#endif
    const unsigned char chineseTxtTune1[] = {0x80, 0x00,0x8C,0x03,0x65,0x74,0x00,0x20,0x00,0x31,0x00,0x00};
    const unsigned char chineseTxtTune2[] = {0x80, 0x00,0x8C,0x03,0x65,0x74,0x00,0x20,0x00,0x32,0x00,0x00}; 
    const unsigned char chineseTxtTune3[] = {0x80, 0x00,0x8C,0x03,0x65,0x74,0x00,0x20,0x00,0x33,0x00,0x00}; 
    const unsigned char chineseTxtTune4[] = {0x80, 0x00,0x8C,0x03,0x65,0x74,0x00,0x20,0x00,0x34,0x00,0x00}; 
    const unsigned char chineseTxtTune5[] = {0x80, 0x00,0x8C,0x03,0x65,0x74,0x00,0x20,0x00,0x35,0x00,0x00};
    const unsigned char chineseTxtPressMenu[] = {0x80, 0x00,0x63,0x09,0x67,0xEE,0x5F,0x55,0x95,0x2E,0x00,0x00};
    const unsigned char chineseTxtSecurity[] = {0x80, 0x00,0x5B,0x89,0x51,0x68,0x00,0x00};
    const unsigned char chineseTxtEnglish[] = {0x80, 0x00,0x82,0xF1,0x8B,0xED,0x00,0x00};
    const unsigned char chineseTxtGerman[] = {0x80, 0x00,0x5F,0xB7,0x8B,0xED,0x00,0x00};
    const unsigned char chineseTxtValidity[] = {0x80, 0x00,0x67,0x09,0x65,0x48,0x67,0x1F,0x00,0x00};
    const unsigned char chineseTxtMessageTyp[] = {0x80, 0x00,0x75,0x59,0x8A,0x00,0x79,0xCD,0x7C,0x7B,0x00,0x00};
    const unsigned char chineseTxtSetVoiceMail[] = {0x80, 0x00,0x8B,0xBE,0x5B,0x9A,0x8B,0xED,0x97,0xF3,0x4F,0xE1,0x7B,0xB1,0x00,0x00}; 
    const unsigned char chineseTxtCallTimers[] = {0x80, 0x00,0x75,0x35,0x8B,0xDD,0x8B,0xB0,0x65,0xF6,0x00,0x00};
    const unsigned char chineseTxtLastCharge[] = {0x80, 0x00,0x67,0x00,0x54,0x0E,0x65,0x36,0x8D,0x39,0x00,0x00};
    const unsigned char chineseTxtTotalCharge[] = {0x80, 0x00,0x60,0x3B,0x51,0x71,0x65,0x36,0x8D,0x39,0x00,0x00};
    const unsigned char chineseTxtResetCharge[] = {0x80, 0x00,0x59,0x0D,0x8B,0xBE,0x65,0x36,0x8D,0x39,0x00,0x00};
    const unsigned char chineseTxtDiscard[] = {0x80, 0x00,0x65,0x3E,0x5F,0x03,0x00,0x00};
    const unsigned char chineseTxtEDIT[] = {0x80, 0x00,0x7F,0x16,0x8F,0x91,0x00,0x00};
    const unsigned char chineseTxtExtractNumber[] = {0x80, 0x00,0x4F,0x7F,0x00,0x20,0x00,0x20,0x76,0x84,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtWrite[] = {0x80, 0x00,0x7F,0x16,0x8F,0x91,0x00,0x00};
    const unsigned char chineseTxtCheckStatus[] = {0x80, 0x00,0x68,0xC0,0x67,0xE5,0x72,0xB6,0x60,0x01,0x00,0x00};
    const unsigned char chineseTxtSetDivert[] = {0x80, 0x00,0x8B,0xBE,0x5B,0x9A,0x54,0x7C,0x53,0xEB,0x8F,0x6C,0x63,0xA5,0x00,0x00};
    const unsigned char chineseTxtCancelDivert[] = {0x80, 0x00,0x53,0xD6,0x6D,0x88,0x54,0x7C,0x53,0xEB,0x8F,0x6C,0x63,0xA5,0x00,0x00};
    const unsigned char chineseTxtHideID[] = {0x80, 0x00,0x96,0x90,0x85,0xCF,0x8E,0xAB,0x4E,0xFD,0x00,0x00};
    const unsigned char chineseTxtTones[] = {0x80, 0x00,0x97,0xF3,0x8C,0x03,0x00,0x00};
    const unsigned char chineseTxtClock[] = {0x80, 0x00,0x95,0xF9,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtSelectLevel[] = {0x80, 0x00,0x7E,0xA7,0x52,0x2B,0x90,0x09,0x62,0xE9,0x00,0x00};
    const unsigned char chineseTxtNewGame[] = {0x80, 0x00,0x65,0xB0,0x6E,0x38,0x62,0x0F,0x00,0x00};
    const unsigned char chineseTxtPhone[] = {0x80, 0x00,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtFriend[] = {0x80, 0x00,0x67,0x0B,0x53,0xCB,0x00,0x00};
    const unsigned char chineseTxtDemo[] = {0x80, 0x00,0x6F,0x14,0x79,0x3A,0x00,0x00};
    const unsigned char chineseTxtSelectMode[] = {0x80, 0x00,0x5F,0x62,0x5F,0x0F,0x00,0x00};
    const unsigned char chineseTxtStop[] = {0x80, 0x00,0x50,0x5C,0x00,0x00};
#ifndef FF_NO_VOICE_MEMO
    const unsigned char chineseTxtRecording[] = {0x80, 0x00,0x6B,0x63,0x57,0x28,0x5F,0x55,0x97,0xF3,0x00,0x00};
    const unsigned char chineseTxtPlayback[] = {0x80, 0x00,0x56,0xDE,0x65,0x3E,0x00,0x00};
#endif
    const unsigned char chineseTxtNoNumber[] = {0x80, 0x00,0x65,0xE0,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtNotAllowed[] = {0x80, 0x00,0x4E,0x0D,0x51,0x41,0x8B,0xB8,0x00,0x00};
    const unsigned char chineseTxtOkToDelete[] = {0x80, 0x00,0x51,0x68,0x90,0xE8,0x52,0x20,0x96,0x64,0x00,0x00};
    const unsigned char chineseTxtBattery[] = {0x80, 0x00,0x75,0x35,0x6C,0x60,0x00,0x00};
    const unsigned char chineseTxtFully[] = {0x80, 0x00,0x6E,0xE1,0x00,0x00};
    const unsigned char chineseTxtCharged[] = {0x80, 0x00,0x51,0x45,0x75,0x35,0x00,0x00};
    const unsigned char chineseTxtBalance[] = {0x80, 0x00,0x5B,0x58,0x6B,0x3E,0x00,0x00};
    const unsigned char chineseTxtLimit[] = {0x80, 0x00,0x96,0x50,0x5E,0xA6,0x00,0x00};
    const unsigned char chineseTxtLimitEq[] = {0x80, 0x00,0x96,0x50,0x5E,0xA6,0x00,0x00}; 
    //const unsigned char chineseTxtRate[] = {0x80, 0x00,0x65,0x36,0x8D,0x39,0x00,0x00};
    const unsigned char chineseTxtRate[] = {0x80, 0x00,0x8d, 0x39, 0x73, 0x87, 0x00, 0x00};

    const unsigned char chineseTxtMinutes[] = {0x80, 0x00,0x52,0x06,0x94,0x9F,0x00,0x00}; 
    const unsigned char chineseTxtCost[] = {0x80, 0x00,0x4E,0xF7,0x68,0x3C,0x00,0x00};
    const unsigned char chineseTxtCredit[] = {0x80, 0x00,0x5B,0x58,0x6B,0x3E,0x00,0x00};
    const unsigned char chineseTxtRemaining[] = {0x80, 0x00,0x52,0x69,0x4F,0x59,0x00,0x00};
    const unsigned char chineseTxtPressOk[] = {0x80, 0x00,0x63,0x09,0x78,0x6E,0x8B,0xA4,0x00,0x00};
    const unsigned char chineseTxtReset[] = {0x80, 0x00,0x91,0xCD,0x65,0xB0,0x8B,0xBE,0x7F,0x6E,0x00,0x00};
    const unsigned char chineseTxtIngoingMinutes[] = {0x80, 0x00,0x59,0x16,0x63,0x02,0x52,0x06,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtIncoming[] = {0x80, 0x00,0x63,0xA5,0x8F,0xDB,0x00,0x00};
    const unsigned char chineseTxtCounter[] = {0x80, 0x00,0x8B,0xB0,0x65,0xF6,0x67,0x3A,0x00,0x00};
    const unsigned char chineseTxtOutgoingCalls[] = {0x80, 0x00,0x5D,0xF2,0x62,0xE8,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtCostCounter[] = {0x80, 0x00,0x8B,0xB0,0x8D,0x39,0x67,0x3A,0x00,0x00};
    const unsigned char chineseTxtEnterCurrency[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x8D,0x27,0x5E,0x01,0x00,0x00};
    const unsigned char chineseTxt1Unit[] = {0x80, 0x00,0x00,0x31,0x00,0x20,0x53,0x55,0x51,0x43,0x00,0x00};
    const unsigned char chineseTxtChanged[] = {0x80, 0x00,0x5D,0xF1,0x65,0x39,0x53,0xD8,0x00,0x00};
    const unsigned char chineseTxtEnterLimit[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x96,0x50,0x5E,0xA6,0x00,0x00}; 
    const unsigned char chineseTxtCreditLimit[] = {0x80, 0x00,0x5B,0x58,0x6B,0x3E,0x96,0x50,0x5E,0xA6,0x00,0x00};
    const unsigned char chineseTxtCancelled[] = {0x80, 0x00,0x5D,0xF1,0x53,0xD6,0x6D,0x88,0x00,0x00};
    const unsigned char chineseTxtView[] = {0x80, 0x00,0x96,0x05,0x89,0xC8,0x00,0x00};
    const unsigned char chineseTxtSendTo[] = {0x80, 0x00,0x53,0xD1,0x90,0x01,0x52,0x30,0x00,0x00};
    const unsigned char chineseTxtEnterPCK[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x50,0x00,0x43,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtEnterCCK[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x43,0x00,0x43,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtEnterNCK[] =  {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x4E,0x00,0x43,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtEnterNSCK[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x4E,0x00,0x53,0x00,0x43,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtEnterSPCK[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x20,0x00,0x53,0x00,0x50,0x00,0x43,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtCodeInc[] = {0x80, 0x00,0x53,0xF7,0x78,0x01,0x95,0x19,0x8B,0xEE,0x00,0x00};
    const unsigned char chineseTxtBlckPerm[] = {0x80, 0x00,0x6C,0x38,0x8F,0xDC,0x5C,0x01,0x95,0x01,0x00,0x00};
    const unsigned char chineseTxtDealer[] =  {0x80, 0x00,0x80,0x54,0x7C,0xFB,0x4E,0xE3,0x74,0x06,0x4E,0xBA,0x00,0x00};
    const unsigned char chineseTxtSendingDTMF[] = {0x80, 0x00,0x53,0xD1,0x90,0x01,0x00,0x44,0x00,0x54,0x00,0x4D,0x00,0x46,0x00,0x00}; 
    const unsigned char chineseTxtCheckNumber[] = {0x80, 0x00,0x68,0xC0,0x67,0xE5,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtNumberBusy[] = {0x80, 0x00,0x53,0xF7,0x78,0x01,0x5F,0xD9,0x00,0x00};
    const unsigned char chineseTxtNoAnswer[] = {0x80, 0x00,0x65,0xE0,0x5E,0x94,0x7B,0x54,0x00,0x00};
    const unsigned char chineseTxtNumberChanged[] = {0x80, 0x00,0x53,0xF7,0x78,0x01,0x5D,0xF1,0x65,0x39,0x53,0xD8,0x00,0x00};
    const unsigned char chineseTxtNetworkBusy[] = {0x80, 0x00,0x7F,0x51,0x7E,0xDC,0x5F,0xD9,0x00,0x00};
    const unsigned char chineseTxtNotSubscribed[] = {0x80, 0x00,0x67,0x2A,0x76,0x7B,0x8B,0xB0,0x00,0x00};
    //const unsigned char chineseTxtAnyKeyAnswer[] = {0x80, 0x00,0x63,0x09,0x4E,0xFB,0x4F,0x55,0x95,0x2E,0x56,0xDE,0x7B,0x54,0x00,0x00};
     const unsigned char chineseTxtAnyKeyAnswer[] = {0x80, 0x00,0x63,0x09,0x4e,0xfb,0x4f,0x55,0x95,0x2e,0x5e,0x94,0x7b,0x54,0x00,0x00};
	const unsigned char chineseTxtPINBlock[] = {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x5D,0xF1,0x5C,0x01,0x95,0x01,0x00,0x00};
    const unsigned char chineseTxtOnce[] = {0x80, 0x00,0x4E,0x00,0x6B,0x21,0x00,0x00};
    const unsigned char chineseTxtDaily[] = {0x80, 0x00,0x6B,0xCF,0x65,0xE5,0x00,0x00};
    const unsigned char chineseTxtWeekdays[] = {0x80, 0x00,0x5D,0xE5,0xCF,0x5C,0x65,0xE5,0x00,0x00};
    const unsigned char chineseTxtWeekly[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x67,0x1F,0x00,0x00};
    const unsigned char chineseTxtMonthly[] = {0x80, 0x00,0x6B,0xCF,0x67,0x08,0x00,0x00};
    const unsigned char chineseTxtYearly[] = {0x80, 0x00,0x6B,0xCF,0x5E,0x74,0x5E,0x74,0x00,0x00};
    const unsigned char chineseTxtEnterText[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0x87,0x5B,0x57,0x00,0x00};
    const unsigned char chineseTxtStartTime[] = {0x80, 0x00,0x5F,0x00,0x59,0xCB,0x65,0xF6,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtEndTime[] = {0x80, 0x00,0x7E,0xD3,0x67,0x5F,0x65,0xF6,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtAlarmOffset[] = {0x80, 0x00,0x00,0x55,0x00,0x6E,0x00,0x6B,0x00,0x6E,0x00,0x6F,0x00,0x77,0x00,0x6E,0x00,0x00};
    const unsigned char chineseTxtAlarmRepeat[] = {0x80, 0x00,0x91,0xCD,0x59,0x0D,0x95,0xF9,0x94,0xC3,0x00,0x00};
    const unsigned char chineseTxtIcon[] = {0x80, 0x00,0x63,0xD2,0x75,0x3B,0x00,0x00};
    const unsigned char chineseTxtDeleteEntry[] = {0x80, 0x00,0x52,0x20,0x96,0x94,0x8F,0x93,0x51,0x65,0x00,0x00};
    const unsigned char chineseTxtMonday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x4E,0x00,0x00,0x00};
    const unsigned char chineseTxtTuesday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x4E,0x8C,0x00,0x00};
    const unsigned char chineseTxtWednesday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x4E,0x09,0x00,0x00};
    const unsigned char chineseTxtThursday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x56,0xDB,0x00,0x00};
    const unsigned char chineseTxtFriday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x4E,0x94,0x00,0x00};
    const unsigned char chineseTxtSaturday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x51,0x6D,0x00,0x00};
    const unsigned char chineseTxtSunday[] = {0x80, 0x00,0x6B,0xCF,0x66,0xEF,0x65,0xE5,0x00,0x00};
    const unsigned char chineseTxtGoto[] = {0x80, 0x00,0x53,0xBB,0x00,0x00};
    const unsigned char chineseTxtListAll[] = {0x80, 0x00,0x5C,0x55,0x79,0x3A,0x51,0x68,0x90,0xE8,0x00,0x00};
    const unsigned char chineseTxtSetDateTime[] = {0x80, 0x00,0x8B,0xBE,0x5B,0x9A,0x65,0xE5,0x67,0x1F,0x65,0xF6,0x95,0xF4,0x00,0x00};
    const unsigned char chineseTxtEdit[] = {0x80, 0x00,0x7F,0x16,0x8F,0x91,0x00,0x00};
    const unsigned char chineseTxtChangePassword[] = {0x80, 0x00,0x65,0x39,0x53,0xD8,0x5B,0xC6,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtSetBar[] = {0x80, 0x00,0x5C,0x01,0x95,0x01,0x8B,0xBE,0x7F,0x6E,0x00,0x00};
    const unsigned char chineseTxtSmsTone[] = {0x80, 0x00,0x00,0x53,0x00,0x4D,0x00,0x53,0x97,0xF3,0x8C,0x03,0x00,0x00};
    const unsigned char chineseTxtMelody[] = {0x80, 0x00,0x65,0xCB,0x5F,0x8B,0x00,0x00};
    /* Marcus: Issues 1122 & 1137: 12/09/2002: Start */
    const unsigned char chineseTxtSilent[] = {0x80, 0x00,0x5b,0x89,0x97,0x59,0x00,0x00};
    const unsigned char chineseTxtSilentMode[] = {0x80, 0x00,0x5b,0x89,0x97,0x59,0x6a,0x21,0x5f,0x0f,0x00,0x00};
    /* Marcus: Issues 1122 & 1137: 12/09/2002: End */
    const unsigned char chineseTxtClick[] = {0x80, 0x00,0x63,0x09,0x00,0x00};
    const unsigned char chineseTxtClearAlarm[] = {0x80, 0x00,0x53,0xD6,0x6D,0x88,0x95,0xF9,0x94,0xC3,0x00,0x00};
    const unsigned char chineseTxtTimeFormat[] = {0x80, 0x00,0x65,0xF6,0x95,0xF4,0x66,0x3E,0x79,0x3A,0x5F,0x62,0x5F,0x0F,0x00,0x00};
    const unsigned char chineseTxtTwelveHour[] = {0x80, 0x00,0x00,0x31,0x00,0x32,0x00,0x20,0x5C,0x0F,0x65,0xF6,0x00,0x00};
	const unsigned char chineseTxtTwentyfourHour[] = {0x80, 0x00,0x00,0x32,0x00,0x34,0x00,0x20,0x5C,0x0F,0x65,0xF6,0x00,0x00};
    const unsigned char chineseTxtAlarm_5[] = {0x80, 0x00,0x63,0xD0,0x52,0x4D,0x00,0x35,0x52,0x06,0x94,0x9F,0x00,0x00}; 
    const unsigned char chineseTxtAlarm_10[] = {0x80, 0x00,0x63,0xD0,0x52,0x4D,0x00,0x31,0x00,0x30,0x52,0x06,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtAlarm_15[] = {0x80, 0x00,0x63,0xD0,0x52,0x4D,0x00,0x31,0x00,0x35,0x52,0x06,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtAlarm_60[] = {0x80, 0x00,0x63,0xD0,0x52,0xD4,0x00,0x36,0x00,0x30,0x52,0x06,0x94,0x9F,0x00,0x00};
    const unsigned char chineseTxtStore[] = {0x80, 0x00,0x50,0xA8,0x5B,0x58,0x00,0x00}; 
    const unsigned char chineseTxtNone[] = {0x80, 0x00,0x5D,0xF1,0x00,0x00};
    const unsigned char chineseTxtBeep[] = {0x80, 0x00,0x8B,0x66,0x7B,0x1B,0x58,0xF0,0x00,0x00};
    const unsigned char chineseTxtPressOKToConfirm[] = {0x80, 0x00,0x63,0x09,0x00,0x4F,0x00,0x4B,0x78,0x6E,0x5B,0x9A,0x00,0x00};
    const unsigned char chineseTxtNwLogOff[] = {0x80, 0x00,0x90,0x00,0x5C,0x80,0x57,0x51,0x7E,0xDC,0x00,0x00};
    const unsigned char chineseTxtNwLogOn[] = {0x80, 0x00,0x8F,0xDB,0x51,0x65,0x57,0x51,0x7E,0xDC,0x00,0x00};
    const unsigned char chineseTxtIncorrect[] = {0x80, 0x00,0x95,0x19,0x8B,0xEF,0x00,0x00};
    const unsigned char chineseTxtPIN[] =  {0x80, 0x00,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x31,0x00,0x00};
    const unsigned char chineseTxtPUK[] =  {0x80, 0x00,0x00,0x50,0x00,0x55,0x00,0x4B,0x00,0x31,0x00,0x00};
    const unsigned char chineseTxtPIN2[] =  {0x80, 0x00,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtPUK2[] = {0x80, 0x00,0x00,0x50,0x00,0x55,0x00,0x4B,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtConfPin2[] = {0x80, 0x00,0x6B,0x63,0x78,0x6E,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtNewPIN2[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xB0,0x00,0x50,0x00,0x49,0x00,0x4E,0x00,0x32,0x00,0x00};
    const unsigned char chineseTxtUnlockOK[] = {0x80, 0x00,0x5B,0xC6,0x95,0x01,0x00,0x4F,0x00,0x4B,0x00,0x00};
    const unsigned char chineseTxtPause[] = {0x80, 0x00,0x66,0x82,0x50,0x5C,0x00,0x00};
    const unsigned char chineseTxtMore[] = {0x80, 0x00,0x59,0x1A,0x00,0x00};
    const unsigned char chineseTxtNotOk[] = {0x80, 0x00,0x4E,0x0D,0x00,0x00};
    const unsigned char chineseTxtIMEI[] = {0x80, 0x00,0x00,0x49,0x00,0x4D,0x00,0x45,0x00,0x49,0x00,0x00}; 
    const unsigned char chineseTxtList[] = {0x80, 0x00,0x54,0x0D,0x53,0x55,0x00,0x00};
    const unsigned char chineseTxtFull[] = {0x80, 0x00,0x6E,0xE1,0x00,0x00};
    const unsigned char chineseTxtUSSD[] = {0x80, 0x00,0x00,0x55,0x00,0x53,0x00,0x53,0x00,0x44,0x00,0x00};
    const unsigned char chineseTxtNotAvailable[] = {0x80, 0x00,0x4E,0x0D,0x53,0xEF,0x4F,0x9B,0x67,0x0D,0x52,0xA1,0x00,0x00};
    const unsigned char chineseTxtCallBarringAll[] = {0x80, 0x00,0x54,0x7C,0x53,0xEB,0x96,0x50,0x52,0x36,0x00,0x00};
    const unsigned char chineseTxtSimToolkit[] = {0x80, 0x00,0x00,0x53,0x00,0x4D,0x00,0x53,0x5D,0xE5,0x51,0x77,0x7B,0xB1,0x00,0x00};
    const unsigned char chineseTxtPhonebook[] = {0x80, 0x00,0x75,0x35,0x8B,0xDD,0x67,0x2C,0x00,0x00};
    //const unsigned char chineseTxtRecentCalls[] = {0x80, 0x00,0x8F,0xD1,0x67,0x1F,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtRecentCalls[] = {0x80, 0x00,0x90,0x1a,0x8b,0xdd,0x8b,0xb0,0x5f,0x55, 0x00, 0x00};

    const unsigned char chineseTxtNames[] = {0x80, 0x00,0x59,0xD3,0x54,0x0D,0x00,0x00};
    const unsigned char chineseTxtSearchNumber[] = {0x80, 0x00,0x67,0xE5,0x5B,0xFB,0x53,0xF7,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtSearchLocation[] = {0x80, 0x00,0x67,0xE5,0x5B,0xFB,0x57,0x30,0x57,0x40,0x00,0x00};
    const unsigned char chineseTxtInterCalls[] =  {0x80, 0x00,0x56,0xFD,0x96,0x45,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtInterHome[] =  {0x80, 0x00,0x56,0xFD,0x96,0x45,0x00,0x28,0x65,0xE0,0x5B,0xB6,0x00,0x29,0x75,0x35,0x8B,0xDD,0x00,0x00}; 
    const unsigned char chineseTxtWhenRoaming[] = {0x80, 0x00,0x6F,0x2B,0x6E,0x38,0x00,0x00};
    //const unsigned char chineseTxtCancelAllBars[] =  {0x80, 0x00,0x52,0x20,0x96,0x64,0x51,0x68,0x90,0xE8,0x00,0x00};
    const unsigned char chineseTxtCancelAllBars[] =  {0x80, 0x00,0x52,0x20,0x96,0x64,0x51,0x68,0x90,0xE8,0x54,0x7c,0x53,0xeb,0x96,0x50, 0x52, 0x36,0x00,0x00};
    const unsigned char chineseTxtVMCenter[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x00,0x56,0x00,0x4D,0x4E,0x2D,0x5F,0xC3,0x00,0x00};
    const unsigned char chineseTxtWaiting[] = {0x80, 0x00,0x7B,0x49,0x50,0x19,0x00,0x00};
    const unsigned char chineseTxtBarred[] =   {0x80, 0x00,0x58,0x35,0x58,0x5E,0x00,0x00};
    const unsigned char chineseTxtForwarded[] = {0x80, 0x00,0x5D,0xF1,0x8F,0x6C,0x90,0x01,0x00,0x00};
    const unsigned char chineseTxtCallDivertNoReach[] = {0x80, 0x00,0x8F,0x6C,0x79,0xFB,0x67,0x2A,0x90,0x1A,0x00,0x00};
    const unsigned char chineseTxtCallDivertNoReply[] = {0x80, 0x00,0x8F,0x6C,0x79,0xFB,0x65,0xE0,0x5E,0x94,0x7B,0x54,0x00,0x00};
    const unsigned char chineseTxtCallDivertIfBusy[] = {0x80, 0x00,0x8F,0x6C,0x79,0xFB,0x82,0xE5,0x5F,0xD9,0x00,0x00};
    const unsigned char chinesexTxtPUK2[] = {0x80, 0x00,0x00,0x50,0x00,0x55,0x00,0x4B,0x00,0x32,0x00,0x00};
	const unsigned char chineseTxtMax[] = {0x80, 0x00,0x67,0x00,0x59,0x1A,0x00,0x00};
    const unsigned char chineseTxtPlus[] = {0x80, 0x00,0x52,0xA0,0x00,0x00};
    const unsigned char chineseTxtMinus[] = {0x80, 0x00,0x51,0xCF,0x00,0x00};
    const unsigned char chineseTxtDivideBy[] = {0x80, 0x00,0x96,0x64,0x4E,0xE5,0x00,0x00};
    const unsigned char chineseTxtMultiplyBy[] = {0x80, 0x00,0x4E,0x58,0x4E,0xE5,0x00,0x00};
    const unsigned char chineseTxtEquals[] = {0x80, 0x00,0x7B,0x49,0x4E,0x8E,0x00,0x00};
    const unsigned char chineseTxtSending[] = {0x80, 0x00,0x53,0xD1,0x90,0x01, 0x00, 0x00};
    const unsigned char chineseTxtMessage[] = {0x80, 0x00,0x57,0x79,0x8A,0x00,0x00,0x00}; 
    const unsigned char chineseTxtSoftNext[] = {0x80, 0x00,0x4E,0x0B,0x4E,0x00,0x4E,0x2A,0x00,0x00};
    const unsigned char chineseTxtLastCall[] = {0x80, 0x00,0x67,0x00,0x54,0x0E,0x75,0x35,0x8B,0xDD,0x00,0x00};
  	const unsigned char chineseTxtIncomingCalls[] = {0x80, 0x00,0x63,0xA5,0x8F,0xDB,0x75,0x35,0x8B,0xDD,0x00,0x00};
    const unsigned char chineseTxtLow[] = {0x80, 0x00,0x4F,0x4E,0x00,0x00};
    const unsigned char chineseTxtMedium[] = {0x80, 0x00,0x4E,0x2D,0x00,0x00};
    const unsigned char chineseTxtHigh[] = {0x80, 0x00,0x9A,0xD8,0x00,0x00};
    const unsigned char chineseTxtIncreasing[] =  {0x80, 0x00,0x6E,0x10,0x58,0x9E,0x00,0x00};
    const unsigned char chineseTxtTimerReset[] =  {0x80, 0x00,0x91,0xCD,0x8B,0xBE,0x8B,0xA1,0x65,0xF6,0x00,0x00};
    const unsigned char chineseTxtClockSetTo[] =   {0x80, 0x00,0x95,0xF9,0x94,0x9F,0x8B,0xBE,0x5B,0x9A,0x4E,0x3A,0x00,0x00};
    const unsigned char chineseTxtChinese[] =   {0x80, 0x00,0x4E,0x2D,0x65,0x87,0x00,0x00};
    const unsigned char chineseTxtPassword[] =  {0x80, 0x00,0x5B,0xC6,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtConfirmPassword[] = {0x80, 0x00,0x78,0x6E,0x5B,0x9A,0x5B,0xC6,0x78,0x01,0x00,0x00};
    const unsigned char chineseTxtNewPassword[] = {0x80, 0x00,0x65,0xB0,0x5B,0xC6,0x78,0x01,0x00,0x00}; 
    const unsigned char chineseTxtRequesting[]= {0x80, 0x00,0x6B,0x63,0x57,0x28,0x89,0x81,0x6C,0x42,0x00,0x00};
    const unsigned char chineseTxt24Hours[] = {0x80, 0x00,0x00,0x32,0x00,0x34,0x00,0x20,0x5C,0x0F,0x65,0xF6,0x00,0x00};
    const unsigned char chineseTxt12Hour[] = {0x80, 0x00,0x00,0x31,0x00,0x32,0x00,0x20,0x5C,0x0F,0x65,0xF6,0x00,0x00};
	const unsigned char chineseTxtInCallTimer[]= {0x80, 0x00,0x8B,0xA1,0x65,0xF6,0x00,0x00};
	const unsigned char chineseTxt1Hour[] = {0x80, 0x00,0x00,0x31,0x00,0x20,0x5c,0x0f,0x65,0xF6,0x00,0x00};
	const unsigned char chineseTxt1Week[] = {0x80, 0x00,0x00,0x31,0x00,0x20,0x66,0x1f,0x67,0x1f,0x00,0x00};
	const unsigned char chineseTxtMaxTime[] = { 0x80, 0x00,0x00,'M',0x00,'A',0x00,'X',0x00,'I',0x00,'M',0x00,'U',0x00,'M',0x00,0x00,0x00 };//  May 10, 2004    REF: CRR 15753  Deepa M.D .Localization required
	const unsigned char chineseTxtValiditySet[] = {0x80, 0x00,0x67,0x09,0x65,0x48,0x67,0x1F,0x8B,0xBE,0x5B,0x91,0x00,0x00};
	const unsigned char chineseTxtEmergency[] = {0x80, 0x00,0x7D,0x27,0x60,0x25,0x00,0x00};
	const unsigned char chineseTxtOutofRange[] = {0x80, 0x00,0x5C,0x80,0x83,0xD3,0x56,0xF4,0x00,0x00};
	const unsigned char chineseTxtUppercase[] = {0x80, 0x00,0x00,0x41,0x00,0x42,0x00,0x43,0x00,0x00};
	const unsigned char chineseTxtLowercase[] = {0x80, 0x00,0x00,0x61,0x00,0x62,0x00,0x63,0x00,0x00};
	const unsigned char chineseTxtDigits[] = {0x80, 0x00,0x00,0x31,0x00,0x32,0x00,0x33,0x00,0x00};
	const unsigned char chineseTxtSimNot[] = {0x80, 0x00,0x00,0x53,0x00,0x49,0x00,0x4D,0x00,0x4E,0xED,0x00,0x00};
	const unsigned char chineseTxtReady[] = {0x80, 0x00,0x00,0x52,0x00,0x65,0x00,0x61,0x00,0x64,0x00,0x79,0x00,0x00};
    const unsigned char chineseTxtCallBAOC[]= {0x80, 0x00,0x54,0x7c,0x51,0xFA,0x96,0x50,0x52,0x36,0x00,0x00};
    const unsigned char chineseTxtCallBAOIC[] = {0x80, 0x00,0x79,0x81,0x62,0xe8,0x56,0xfd,0x96,0x45,0x75,0x35,0x8b,0xdd,0x00,0x00};
   	const unsigned char chineseTxtCallBAOICexH[] = {0x80, 0x00,0x53,0xea,0x62,0xe8,0x56,0xde,0x67,0x2c,0x56,0xfd,0x00,0x00,0x00};
    const unsigned char chineseTxtCallBAIC[] = {0x80, 0x00,0x54,0x7c,0x51,0x65,0x96,0x50,0x52,0x36,0x00,0x00};
    const unsigned char chineseTxtCallBAICroam[] = {0x80, 0x00,0x6f,0x2b,0x6e,0x38,0x65,0xf6,0x62,0xd2,0x63,0xa5,0x00,0x00};
    const unsigned char chineseTxtCallBAOCAll[] = {0x80, 0x00,0x79,0x81,0x62,0xe8,0x62,0x40,0x67,0x09,0x75,0x35,0x8b,0xdd,0x00,0x00};
	const unsigned char chineseTxtCallBAICAll[] = {0x80, 0x00,0x62,0xD2,0x63,0xa5,0x62,0x40,0x67,0x09,0x67,0x65,0x75,0x35,0x00,0x00};
	const unsigned char chineseTxtCLIP[] = {0x80, 0x00,0x51,0x41,0x8b,0xb8,0x5b,0xf9,0x65,0xb9,0x66,0x3e,0x79,0x3a,0x67,0x2c,0x67,0x3a,0x53,0xf7,0x78,0x01,0x00,0x00};
	const unsigned char chineseTxtCOLP[] = {0x80, 0x00,0x66,0x3e,0x79,0x3a,0x67,0x65,0x75,0x35,0x53,0xf7,0x78,0x01,0x00,0x00};
	const unsigned char chineseTxtCOLR[] = {0x80, 0x00,0x4e,0x0d,0x66,0x3e,0x79,0x3a,0x67,0x65,0x75,0x35,0x53,0xf7,0x78,0x01,0x00,0x00};
	const unsigned char chineseTxtTwoAttemptsLeft[] = {0x80, 0x00,0x00,0x32,0x65,0x3E,0x5F,0x03,0x5C,0x1D,0x8B,0xD5,0x00,0x00};
	const unsigned char chineseTxtOneAttemptLeft[] = {0x80, 0x00,0x00,0x31,0x65,0x3E,0x5F,0x03,0x5C,0x1D,0x8B,0xD5,0x00,0x00};
	const unsigned char chineseTxtOld[] = {0x80, 0x00,0x8F,0x93,0x51,0x65,0x65,0xE7,0x00,0x00};
	const unsigned char chineseTxtInsertnote[] = {0x80, 0x00, 0x63,0xd2,0x51,0x65,0x6c,0xe8,0x89,0xe3,0x00,0x00};
   	const unsigned char chineseTxtMelodygenerator[] = {0x80, 0x00, 0x54,0x8c,0x5f,0x26,0x4e,0xa7,0x75,0x1f,0x56,0x68,0x00,0x00};
   	const unsigned char chineseTxtOctaveHigher[] = {0x80, 0x00, 0x97,0xf3,0x96,0x36,0x9a,0xd8,0x00,0x00};
   	const unsigned char chineseTxtOctaveLower[] = {0x80, 0x00, 0x97,0xf3,0x96,0x36,0x4f,0x4e,0x00,0x00};
   	const unsigned char chineseTxtLoadMelody[] = {0x80, 0x00, 0x8f,0x7d,0x51,0x65,0x54,0x8c,0x5f,0x26,0x00,0x00};

 const unsigned char chineseTxtEasyText[] = { 0x80, 0x00,0x4e,0x2d,0x65,0x87,0x8f,0x93,0x51,0x65,0x6c,0xd5, 0x00,0x00 };

const unsigned char chineseTxtDataCount[] = {0x80, 0x00,0x65,0x70,0x63,0x6e,0x8b,0xa1,0x65,0x70, 0x00, 0x00};
const unsigned char chineseTxtQoS[] = {0x80, 0x00,0x67,0x0d,0x52,0xa1,0x8d,0x28,0x91,0xcf, 0x00, 0x00};
const unsigned char chineseTxtTypeofConnection[] = {0x80, 0x00,0x8f,0xde,0x63,0xa5, 0x00, 0x00};
const unsigned char chineseTxtTotalCount[] = { 0x80, 0x00,0x51,0x68,0x90,0xe8,0x8b,0xa1,0x65,0x70,0x00, 0x00};
const unsigned char chineseTxtResetCount[] = {0x80, 0x00,0x8b,0xa1,0x65,0x70,0x6e,0x05,0x96,0xf6,0x00,0x00};
const unsigned char chineseTxtDataCounter[] = {0x80, 0x00,0x65,0x70,0x63,0x6e,0x8b,0xa1,0x65,0x70,0x56,0x68,0x00,0x00};
const unsigned char chineseTxtV42bis[] = { 0x80, 0x00,0x00,'v',0x00,'.',0x00,'4',0x00,'2',0x00,'b',0x00,'i',0x00,'s',0x00,0x00,0x00 };
const unsigned char chineseTxtNormal[] = {0x80, 0x00,0x66,0x6e,0x90,0x1a, 0x00,0x00};
const unsigned char chineseTxtCSD[] = { 0x80, 0x00,0x00,0x43,0x00,0x53,0x00,0x44, 0x00,0x00};
const unsigned char chineseTxtClassB[] = { 0x80, 0x00,0x00,0x42,0x7c,0x7b,0x00,0x00};
const unsigned char chineseTxtClassC[] = { 0x80, 0x00,0x00,0x43,0x7c,0x7b,0x00,0x00};
/* SPR#2324 - SH - Removed some strings */
const unsigned char chineseTxtIPAddress[] = {0x80, 0x00,0x00,0x49,0x00,0x50,0x57,0x30,0x57,0x40,0x00,0x31,0x00,0x00};
const unsigned char chineseTxtIPAddress2[] = { 0x80, 0x00,0x00,0x49,0x00,0x50,0x57,0x30,0x57,0x40,0x00,0x32,0x00};
const unsigned char chineseTxtPort1[] = { 0x80, 0x00,0x7a,0xef,0x53,0xe3,0x00,0x31,0x00};
const unsigned char chineseTxtPort2[] = { 0x80, 0x00, 0x7a,0xef,0x53,0xe3,0x00,0x32,0x00,0x00};
const unsigned char chineseTxtAPN[] = { 0x80, 0x00,0x00,'A',0x00,'P',0x00,'N',0x00,0x00,0x00 };
const unsigned char chineseTxtConnectionType[] = { 0x80, 0x00,0x8f,0xde,0x63,0xa5,0x7c,0x7b,0x57,0x8b,0x00, 0x00};

//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
const unsigned char chineseTxtNameServer1[] = {0x80, 0x00,0x00,'P',0x00,'r',0x00,'i',0x00,'m',0x00,'a',0x00,'r',0x00,'y',0x00,' ',0x00,'D',0x00e,'N',0x00,'S',0x00,0x00,0x00};
const unsigned char chineseTxtNameServer2[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'c',0x00,'o',0x00,'n',0x00,'d',0x00,'a',0x00,'r',0x00,'y',0x00e,' ',0x00,'D',0x00,'N',0x00,'S',0x00,0x00,0x00};
const unsigned char chineseTxtPPGAuthentication[] = {0x80, 0x00,0x00,'A',0x00,'u',0x00,'t',0x00,'h',0x00,'e',0x00e,'n',0x00,'t',0x00,'i',0x00,'c',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00,0x00};
const unsigned char chineseTxtWirelessProfiledHTTP[] = {0x80, 0x00,0x00,'P',0x00,'r',0x00,'o',0x00,'x',0x00,'y',0x00,0x00,0x00};
#endif



const unsigned char chineseTxtGPRS[] = { 0x80, 0x00,0x00,'G',0x00,'P',0x00,'R',0x00,'S',0x00,0x00,0x00 };
const unsigned char chineseTxtOutgoingData[] = { 0x80, 0x00,0x53,0xd1,0x90,0x01,0x65,0x70,0x63,0x6e,0x00,0x00};

		const unsigned char chineseTxtGame[] = {0x80, 0x00, 0x6e,0x38,0x62,0x0f,0x00,0x00};
		const unsigned char chineseTxtHomepage[] = {0x80, 0x00, 0x4e,0x3b,0x98,0x75,0x00,0x00};
		const unsigned char chineseTxtBookmarks[] = {0x80, 0x00, 0x4e,0x66,0x7b,0x7e,0x00,0x00};
		const unsigned char chineseTxtGoTo[] = {0x80, 0x00, 0x80,0x54,0x52,0x30,0x00,0x55,0x00,0x52,0x00,0x4c,0x00,0x00};
		const unsigned char chineseTxtHistory[] = {0x80, 0x00, 0x53,0x86,0x53,0xf2,0x00,0x00};
   		const unsigned char chineseTxtSetup[] = {0x80, 0x00, 0x8b,0xbe,0x5b,0x9a,0x00,0x00};
   		const unsigned char chineseTxtNewBookmark[] = {0x80, 0x00, 0x65,0xb0,0x4e,0x66,0x7b,0x7e,0x00,0x00};

   		const unsigned char chineseTxtWWWdot[] = {0x80, 0x00, 0x00,0x68,0x00,0x74,0x00,0x74,0x00,0x70,0x00,0x3a,0x00,0x2f,0x00,0x2f,0x00,0x77,0x00,0x77,0x00,0x77,0x00,0x2e,0x00,0x00};
   		const unsigned char chineseTxtWAPdot[] = {0x80, 0x00, 0x00,0x68,0x00,0x74,0x00,0x74,0x00,0x70,0x00,0x3a,0x00,0x2f,0x00,0x2f,0x00,0x77,0x00,0x61,0x00,0x70,0x00,0x2e,0x00,0x00};
   		const unsigned char chineseTxtSMS[] = {0x80, 0x00, 0x77,0xed,0x6d,0x88,0x60,0x6f,0x00,0x00};
   		const unsigned char chineseTxtEmail[] = {0x80, 0x00, 0x75,0x35,0x5b,0x50,0x90,0xae,0x4e,0xf6,0x00,0x00};
   		const unsigned char chineseTxtClear[] = {0x80, 0x00, 0x6e,0x05,0x96,0x64,0x00,0x00};
   		const unsigned char chineseTxtBookmark[] = {0x80, 0x00, 0x52,0xa0,0x51,0x65,0x4e,0x66,0x7b,0x7e,0x00,0x00};
/* SPR#2324 - SH - Removed some strings */
   		const unsigned char chineseTxtSaveHistory[] = {0x80, 0x00, 0x4f,0xdd,0x5b,0x58,0x53,0x86,0x53,0xf2,0x00,0x00};
   		const unsigned char chineseTxtEditName[] = {0x80, 0x00, 0x7f,0x16,0x8f,0x91,0x54,0x0d,0x5b,0x57,0x00,0x00};
   		const unsigned char chineseTxtEditURL[] = {0x80, 0x00, 0x7f,0x16,0x8f,0x91,0x00,0x55,0x00,0x52,0x00,0x4c,0x00,0x00};

   		const unsigned char chineseTxtAccessType[] = {0x80, 0x00, 0x63,0xa5,0x51,0x65,0x7c,0x7b,0x57,0x8b,0x00,0x00};
   		const unsigned char chineseTxtResponseTimer[] = {0x80, 0x00, 0x54,0xcd,0x5e,0x94,0x8b,0xa1,0x65,0xf6,0x56,0x68,0x00,0x00};
   		const unsigned char chineseTxtDialupNumber[] = {0x80, 0x00, 0x62,0xe8,0x53,0xf7,0x53,0xf7,0x78,0x01,0x00,0x00};
   		const unsigned char chineseTxtTemporary[] = {0x80, 0x00, 0x4e,0x34,0x65,0xf6,0x00,0x00};
   		const unsigned char chineseTxtContinuous[] = {0x80, 0x00, 0x7e,0xe7,0x7e,0xed,0x00,0x00};
   		const unsigned char chineseTxtGPRSData[] = {0x80, 0x00, 0x00,0x47,0x00,0x50,0x00,0x52,0x00,0x53,0x65,0x70,0x63,0x6e,0x00,0x00};
   		const unsigned char chineseTxtCSData[] = {0x80, 0x00, 0x00,0x43,0x00,0x53,0x65,0x70,0x63,0x6e,0x00,0x00};
    	const unsigned char chineseTxtWAP[] = {0x80, 0x00, 0x00,0x57,0x00,0x41,0x00,0x50,0x00,0x00};

const unsigned char chineseTxtExit[] = { 0x80, 0x00,0x90,0x00,0x51,0xfa,0x00,0x00 };
const unsigned char chineseTxtUpdating[] = { 0x80, 0x00,0x66,0xf4,0x65,0xb0,0x4e,0x2d,0x00,0x2e,0x00,0x2e,0x00,0x2e,0x00,0x00};
const unsigned char chineseTxtReload[] = {0x80, 0x00, 0x91,0xcd,0x65,0xb0,0x8f,0x7d,0x51,0x65,0x00,0x00};
const unsigned char chineseTxtStored[] = {0x80, 0x00, 0x5d,0xf2,0x5b,0x58,0x50,0xa8,0x00,0x00};
const unsigned char chineseTxtConnecting[] = {0x80, 0x00, 0x8f,0xde,0x63,0xa5,0x4e,0x2d,0x00,0x2e,0x00,0x2e,0x00,0x2e,0x00,0x00};
const unsigned char chineseTxtDownloading[] = {0x80, 0x00, 0x8f,0x7d,0x51,0x65,0x4e,0x2d,0x00,0x2e,0x00,0x2e,0x00,0x2e,0x00,0x00};
const unsigned char chineseTxtEnterURL[] = {0x80, 0x00, 0x8f,0x93,0x51,0x65,0x00,0x55,0x00,0x52,0x00,0x4c,0x00,0x3a,0x00,0x00};
const unsigned char chineseTxtProfileName[] = {0x80, 0x00, 0x60,0xc5,0x66,0x6f,0x6a,0x21,0x5f,0x0f,0x54,0x0d,0x79,0xf0,0x00,0x3a,0x00,0x00};
const unsigned char chineseTxtISPUsername[] = {0x80, 0x00, 0x75,0x28,0x62,0x37,0x54,0x0d,0x00,0x00};
const unsigned char chineseTxtISPPassword[] = {0x80, 0x00, 0x5b,0xc6,0x78,0x01,0x00,0x00};
const unsigned char chineseTxtGoToLink[] = {0x80, 0x00, 0x8f,0x6c,0x52,0x30,0x8f,0xde,0x63,0xa5,0x00,0x00};
const unsigned char chineseTxtSelect[] = {0x80, 0x00, 0x90,0x09,0x62,0xe9,0x00,0x00};
const unsigned char chineseTxtNotConnected[] = {0x80, 0x00, 0x6c,0xa1,0x67,0x09,0x8f,0xde,0x63,0xa5,0x00,0x00};
const unsigned char chineseTxtScaleImages[] = {0x80, 0x00, 0x56,0xfe,0x50,0xcf,0x7f,0x29,0x65,0x3e,0x00,0x00};

/* SPR#1983 - SH - New WAP error messages */
const unsigned char chineseTxtLoggingIn[] = { 0x80, 0x00,0x76,0x7b,0x96,0x46,0x00,0x00};
const unsigned char chineseTxtPageNotFound[] = { 0x00,'P',0x00,'a',0x00,'g',0x00,'e',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'f',0x00,'o',0x00,'u',0x00,'n',0x00,'d',0x00,0x00};
const unsigned char chineseTxtInvalidURL[] = { 0x00,'I',0x00,'n',0x00,'v',0x00,'a',0x00,'l',0x00,'i',0x00,'d',0x00,' ',0x00,'U',0x00,'R',0x00,'L',0x00,0x00};
const unsigned char chineseTxtNoResponse[] = { 0x00,'N',0x00,'o',0x00,' ',0x00,'R',0x00,'e',0x00,'s',0x00,'p',0x00,'o',0x00,'n',0x00,'s',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAccessDenied[] = { 0x00,'A',0x00,'c',0x00,'c',0x00,'e',0x00,'s',0x00,'s',0x00,' ',0x00,'D',0x00,'e',0x00,'n',0x00,'i',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtErrorInPage[] = { 0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,' ',0x00,'i',0x00,'n',0x00,' ',0x00,'P',0x00,'a',0x00,'g',0x00,'e',0x00,0x00};
const unsigned char chineseTxtScriptError[] = { 0x00,'S',0x00,'c',0x00,'r',0x00,'i',0x00,'p',0x00,'t',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00};
const unsigned char chineseTxtServerError[] = { 0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'e',0x00,'r',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00};
const unsigned char chineseTxtGatewayError[] = { 0x00,'G',0x00,'a',0x00,'t',0x00,'e',0x00,'w',0x00,'a',0x00,'y',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00};
const unsigned char chineseTxtUnexpectedErr[] = { 0x00,'U',0x00,'n',0x00,'e',0x00,'x',0x00,'p',0x00,'e',0x00,'c',0x00,'t',0x00,'e',0x00,'d',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'.',0x00,0x00};
const unsigned char chineseTxtCharacterSet[] = { 0x00,'C',0x00,'h',0x00,'a',0x00,'r',0x00,'a',0x00,'c',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'S',0x00,'e',0x00,'t',0x00,0x00};
const unsigned char chineseTxtWBXMLError[] = { 0x00,'W',0x00,'B',0x00,'X',0x00,'M',0x00,'L',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00};
const unsigned char chineseTxtFatalError[] = { 0x00,'F',0x00,'a',0x00,'t',0x00,'a',0x00,'l',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00};
const unsigned char chineseTxtGatewayBusy[] = { 0x00,'G',0x00,'a',0x00,'t',0x00,'e',0x00,'w',0x00,'a',0x00,'y',0x00,' ',0x00,'B',0x00,'u',0x00,'s',0x00,'y',0x00,0x00};
const unsigned char chineseTxtSecurityFail[] = { 0x00,'S',0x00,'e',0x00,'c',0x00,'u',0x00,'r',0x00,'.',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,0x00};
const unsigned char chineseTxtErrorOutOfMem[] = { 0x00, 'O',0x00,'u',0x00,'t',0x00,' ',0x00,'o',0x00,'f',0x00,' ',0x00,'m',0x00,'e',0x00,'m',0x00,'o',0x00,'r',0x00,'y',0x00,0x00};
const unsigned char chineseTxtErrorMemLow[] = { 0x00, 'M',0x00,'e',0x00,'m',0x00,'o',0x00,'r',0x00,'y',0x00,' ',0x00,'l',0x00,'o',0x00,'w',0x00,0x00};
const unsigned char chineseTxtError[] = { 0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorHTTP[] = { 0x00,'H',0x00,'T',0x00,'T',0x00,'P',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWSP[] = { 0x00,'W',0x00,'S',0x00,'P',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWAE[] = { 0x00,'W',0x00,'A',0x00,'E',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWSPCL[] = { 0x00,'W',0x00,'S',0x00,'P',0x00,'C',0x00,'L',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWSPCM[] = { 0x00,'W',0x00,'S',0x00,'P',0x00,'C',0x00,'M',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWTP[] = { 0x00,'W',0x00,'T',0x00,'P',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWTLS[] = { 0x00,'W',0x00,'T',0x00,'L',0x00,'S',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorWDP[] = { 0x00,'W',0x00,'D',0x00,'P',0x00,0x00,0x00 };
const unsigned char chineseTxtErrorUDCP[] = { 0x00,'U',0x00,'D',0x00,'C',0x00,'P',0x00,0x00,0x00 };
const unsigned char chineseTxtNotSupported[] = {0x80, 0x00, 0x4e,0x0d,0x65,0x2f,0x63,0x01,0x00,0x00};

/* SPR#1983 - SH - More WAP strings */
const unsigned char chineseTxtAttaching[] = { 0x00,'A',0x00,'t',0x00,'t',0x00,'a',0x00,'c',0x00,'h',0x00,'i',0x00,'n',0x00,'g',0x00, 0x00 };
const unsigned char chineseTxtConnection[] = { 0x00,'C',0x00,'o',0x00,'n',0x00,'n',0x00,'e',0x00,'c',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00, 0x00 };
const unsigned char chineseTxtUsing[] = { 0x00,'U',0x00,'s',0x00,'i',0x00,'n',0x00,'g',0x00, 0x00 };
const unsigned char chineseTxtConnectionSpeed[] = { 0x00,'C',0x00,'o',0x00,'n',0x00,'n',0x00,'e',0x00,'c',0x00,'t',0x00,'.',0x00,' ',0x00,'S',0x00,'p',0x00,'e',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtAnalogue[] = { 0x00,'A',0x00,'n',0x00,'a',0x00,'l',0x00,'o',0x00,'g',0x00,'u',0x00,'e',0x00,0x00};
const unsigned char chineseTxtISDN9600[] = { 0x00,'I',0x00,'S',0x00,'D',0x00,'N',0x00,' ',0x00,'9',0x00,'6',0x00,'0',0x00,'0',0x00,0x00};
const unsigned char chineseTxtISDN14400[] = { 0x00,'I',0x00,'S',0x00,'D',0x00,'N',0x00,' ',0x00,'1',0x00,'4',0x00,'4',0x00,'0',0x00,'0',0x00,0x00};
const unsigned char chineseTxtRoaming[] = {0x00, 'R', 0x00, 'O', 0x00, 'A', 0x00, 'M', 0x00, 'I', 0x00, 'N', 0x00, 'G', 0x00, 0x00};
const unsigned char chineseTxtEndWap[] = { 0x00, 'E',0x00,'n',0x00,'d',0x00,' ',0x00,'W',0x00,'A',0x00,'P',0x00,0x00};
const unsigned char chineseTxtUntitled[] = {0x00,'U',0x00,'n',0x00,'t',0x00,'i',0x00,'t',0x00,'l',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtSelectLine[] = {0x80, 0x00,0x90,0x09,0x62,0xe9,0x7e,0xbf,0x8d,0xef, 0x00, 0x00};
const unsigned char chineseTxtLine1[] = {0x80, 0x00,0x7e,0xbf,0x8d,0xef,0x00,0x20,0x00,0x31, 0x00, 0x00};
const unsigned char chineseTxtLine2[] = {0x80, 0x00,0x7e,0xbf,0x8d,0xef,0x00,0x20,0x00,0x32, 0x00, 0x00};
const unsigned char chineseTxtInfoNumbers[] = { 0x80, 0x00,0x4f,0xe1,0x60,0x6f,0x53,0xf7,0x78,0x01,0x00,0x00 };

const unsigned char chineseTxtAttach[] = {0x80, 0x00,0x6f,0xc0,0x6d,0x3b, 0x00, 0x00};
const unsigned char chineseTxtDetach[] = {0x80, 0x00,0x65,0xad,0x5f,0x00, 0x00, 0x00};
const unsigned char chineseTxtDetaching[] = {0x80, 0x00,0x65,0xad,0x5f,0x00, 0x00, 0x00};
const unsigned char chineseTxtDefineContext[] = {0x80, 0x00,0x5b,0x9a,0x4e,0x49,0x4e,0x0a,0x4e,0x0b,0x65,0x87, 0x00, 0x00};

const unsigned char chineseTxtActivateContext[] = {0x80, 0x00,0x6f,0xc0,0x6d,0x3b,0x4e,0x0a,0x4e,0x0b,0x65,0x87,0x00, 0x00};
const unsigned char chineseTxtDeactivateContext[] = {0x80, 0x00,0x65,0xad,0x5f,0x00,0x4e,0x0a,0x4e,0x0b,0x65,0x87,0x00, 0x00};
const unsigned char chineseTxtGPRSPreferred[] = {0x80, 0x00,0x00,0x47,0x00,0x50,0x00,0x52,0x00,0x53,0x4f,0x18,0x51,0x48, 0x00, 0x00};
const unsigned char chineseTxtCSDPreferred[] = {0x80, 0x00,0x00,0x43,0x00,0x53,0x00,0x44,0x4f,0x18,0x51,0x48, 0x00, 0x00};
const unsigned char chineseTxtNotAttached[] = {0x80, 0x00,0x67,0x2a,0x6f,0xc0,0x6d,0x3b, 0x00, 0x00};
const unsigned char chineseTxtAlreadyAttached[] = {0x80, 0x00,0x5d,0xf2,0x6f,0xc0,0x6d,0x3b, 0x00, 0x00};
const unsigned char chineseTxtSelected[] = {0x80, 0x00,0x5d,0xf2,0x90,0x09,0x62,0xe9, 0x00, 0x00};

const unsigned char chineseTxtNetwork[] = {0x80, 0x00,0x7f,0x51,0x7e,0xdc, 0x00, 0x00};
const unsigned char chineseTxtForbidden[] = {0x80, 0x00,0x79,0x81,0x6b,0x62, 0x00, 0x00};

const unsigned char chineseTxtActivateFDN[] = {0x80, 0x00,0x6f,0xc0,0x6d,0x3b,0x00,0x46,0x00,0x44,0x00,0x4e, 0x00, 0x00};
const unsigned char chineseTxtPlease[] = {0x80, 0x00,0x8b,0xf7,0x00,0x00};	/*ap 06/02/02*/
const unsigned char chineseTxtTimedOut[] = {0x80, 0x00,0x8d,0x85,0x65,0xf6, 0x00, 0x00};
const unsigned char chineseTxtWillChangeOn[] = {0x80, 0x00,0x5c,0x06,0x65,0x39,0x53,0xd8,0x4e,0x3a, 0x00, 0x00};
const unsigned char chineseTxtAttached[] = {0x80, 0x00,0x5d,0xf2,0x6f,0xc0,0x6d,0x3b, 0x00, 0x00};
const unsigned char chineseTxtDetached[] = {0x80, 0x00,0x5d,0xf2,0x65,0xad,0x5f,0x00, 0x00, 0x00};
const unsigned char chineseTxtUnknown[]= {0x80, 0x00,0x00, 'U',0x00,'n',0x00,'k',0x00,'n',0x00,'o',0x00,'w',0x00,'n',0x00,0x00,0x00}; //x0035544 07-11-2005
const unsigned char chineseTxtIncomingFax[] = {0x80, 0x00,0x63,0xa5,0x65,0x36,0x4f,0x20,0x77,0x1f, 0x00, 0x00};

/*MC SPR 1242, added string defns*/
const unsigned char chineseTxtCallTransfer[] = {0x80, 0x00, 0x8f,0x6c,0x79,0xfb,0x75,0x35,0x8b,0xdd,0x00,0x00};
const unsigned char chineseTxtConnected[] = {0x80, 0x00,0x5d,0xf2,0x8f,0xde,0x63,0xa5, 0x00, 0x00};
const unsigned char chineseTxtGSM_900[] = {0x80, 0x00,0x00, 'G',0x00,'S',0x00,'M',0x00,' ',0x00,'9',0x00,'0',0x00,'0',0x00,0x00,0x00};
const unsigned char chineseTxtDCS_1800[] = {0x80, 0x00,0x00, 'D',0x00,'C',0x00,'S',0x00,' ',0x00,'1',0x00,'8',0x00,'0',0x00,'0',0x00,0x00,0x00};
const unsigned char chineseTxtPCS_1900[] = {0x80, 0x00,0x00, 'P',0x00,'C',0x00,'S',0x00,' ',0x00,'1',0x00,'9',0x00,'0',0x00,'0',0x00,0x00,0x00};
const unsigned char chineseTxtE_GSM[] = {0x80, 0x00,0x00, 'E',0x00,' ',0x00,'G',0x00,'S',0x00,'M',0x00,0x00,0x00};
const unsigned char chineseTxtGSM_850[] = {0x80, 0x00,0x00, 'G',0x00,'S',0x00,'M',0x00,' ',0x00,'8',0x00,'5',0x00,'0',0x00,0x00,0x00};
const unsigned char chineseTxtCallTimeout[] = {0x80, 0x00,0x54,0x7c,0x53,0xeb,0x8d,0x85,0x65,0xf6, 0x00, 0x00};
const unsigned char chineseTxtBand[] = {0x80, 0x00,0x98,0x91,0x6b,0xb5, 0x00, 0x00};
const unsigned char chineseTxtSatRefresh[] = {0x80, 0x00,0x52,0x37,0x65,0xb0,0x00,0x53,0x00,0x41,0x00,0x54, 0x00, 0x00};
const unsigned char chineseTxtCallBlocked[] = {0x80, 0x00,0x54,0x7c,0x53,0xeb,0x96,0x3b,0x58,0x5e, 0x00, 0x00};
		const unsigned char chineseTxtblacklisted[] = {0x80, 0x00,0x75,0x35,0x8b,0xdd,0x53,0xf7,0x78,0x01,0x9e,0xd1,0x54,0x0d,0x53,0x55,0x00,0x00};
		const unsigned char chineseTxtErrUnknown[] = {0x80, 0x00,0x67,0x2a,0x77,0xe5,0x95,0x19,0x8b,0xef,0x00,0x00};
		const unsigned char chineseTxtSSErr[] = {0x80, 0x00,0x00,0x53,0x00,0x53,0x95,0x19,0x8b,0xef,0x00,0x00};
		const unsigned char chineseTxtUnknownSub[] = {0x80, 0x00,0x67,0x2a,0x77,0xe5,0x75,0x28,0x62,0x37,0x00,0x00};
		const unsigned char chineseTxtIllSub[] = {0x80, 0x00,0x97,0x5e,0x6c,0xd5,0x75,0x28,0x62,0x37,0x00,0x00};
		const unsigned char chineseTxtIllEqip[] = {0x80, 0x00,0x97,0x5e,0x6c,0xd5,0x5e,0xfa,0x8b,0xae,0x00,0x00};
		const unsigned char chineseTxtIllOp[] = {0x80, 0x00,0x97,0x5e,0x6c,0xd5,0x64,0xcd,0x4f,0x5c,0x00,0x00};
		const unsigned char chineseTxtSubVil[] = {0x80, 0x00,0x00, '#', 0x00, ' ', 0x00, '5', 0x00, ' ', 0x00, '4', 0x00,0x00};
		const unsigned char chineseTxtInComp[] = {0x80, 0x00,0x4e,0x0d,0x51,0x7c,0x5b,0xb9,0x00,0x00};
		const unsigned char chineseTxtFacNoSup[] = {0x80, 0x00,0x8b,0xbe,0x59,0x07,0x4e,0x0d,0x63,0xd0,0x4f,0x9b,0x00,0x00};
		const unsigned char chineseTxtAbsSub[] = {0x80, 0x00,0x00, '#', 0x00, ' ', 0x00, '5', 0x00, ' ', 0x00, '7', 0x00,0x00};
		const unsigned char chineseTxtSysFail[] = {0x80, 0x00,0x7c,0xfb,0x7e,0xdf,0x95,0x19,0x8b,0xef,0x00,0x00};
		const unsigned char chineseTxtMissDta[] = {0x80, 0x00,0x65,0x70,0x63,0x6e,0x4e,0x22,0x59,0x31,0x00,0x00};
		const unsigned char chineseTxtUnexDta[] = {0x80, 0x00,0x4e,0x0d,0x67,0x1f,0x67,0x1b,0x65,0x70,0x63,0x6e,0x00,0x00};
		const unsigned char chineseTxtPwdErr[] = {0x80, 0x00,0x8f,0x93,0x51,0x65,0x95,0x19,0x8b,0xef,0x00,0x00};
		const unsigned char chineseTxtUnkAlph[] = {0x80, 0x00,0x67,0x2a,0x77,0xe5,0x5b,0x57,0x6b,0xcd,0x00,0x00};
		const unsigned char chineseTxtMaxMpty[] = {0x80, 0x00,0x67,0x00,0x59,0x1a,0x4f,0x1a,0x8b,0xae,0x53,0xc2,0x52,0xa0,0x65,0xb9,0x00,0x00};
		const unsigned char chineseTxtResNoAvai[] = {0x80, 0x00,0x00, '#', 0x00, ' ', 0x00, '6', 0x00, ' ', 0x00, '4', 0x00,0x00};
		const unsigned char chineseTxtUnRec[] = {0x80, 0x00,0x65,0xe0,0x6c,0xd5,0x8b,0xc6,0x52,0x2b,0x00,0x00};
		const unsigned char chineseTxtMisTyp[] = {0x80, 0x00,0x95,0x19,0x8b,0xef,0x7c,0x7b,0x57,0x8b,0x00,0x00};
		const unsigned char chineseTxtBadStruct[] = {0x80, 0x00,0x57,0x4f,0x7e,0xd3,0x67,0x84,0x00,0x00};
		const unsigned char chineseTxtDupInv[] = {0x80, 0x00,0x91,0xcd,0x59,0x0d,0x4f,0x7f,0x75,0x28,0x00,0x00};
		const unsigned char chineseTxtResLim[] = {0x80, 0x00,0x6c,0xa1,0x67,0x09,0x8d,0x44,0x6e,0x90,0x00,0x00};
		const unsigned char chineseTxtIniRel[] = {0x80, 0x00,0x00, '#', 0x00, ' ', 0x00, '7',0x00, ' ',  0x00, '0', 0x00,0x00};
		const unsigned char chineseTxtFatRes[] = {0x80, 0x00,0x00, '#', 0x00, ' ', 0x00, '7', 0x00, ' ', 0x00, '1', 0x00,0x00};
		const unsigned char chineseTxtEntAnw[] = {0x80, 0x00,0x56,0xde,0x7b,0x54,0x00,0x3a,0x00,0x00};
		const unsigned char chineseTxtSsMsg[] = {0x80, 0x00,0x00,0x53,0x00,0x53,0x6d,0x88,0x60,0x6f,0x00,0x00};
/*MC end*/		
const unsigned char chineseUnknownCode[] = {0x80, 0x00,0x67,0x2a,0x77,0xe5,0x78,0x01, 0x00, 0x00};
const unsigned char chineseTxtMobileInfo[] = {0x80, 0x00,0x62,0x4b,0x67,0x3a,0x4f,0xe1,0x60,0x6f, 0x00, 0x00};
const unsigned char chineseTxtServingCell[]= {0x80, 0x00,0x67,0x0d,0x52,0xa1,0x5c,0x0f,0x53,0x3a, 0x00, 0x00};
const unsigned char chineseTxtNeighbourCell[] = {0x80, 0x00,0x4e,0x34,0x8f,0xd1,0x5c,0x0f,0x53,0x3a, 0x00, 0x00};
const unsigned char chineseTxtLocation[] = {0x80, 0x00,0x4f,0x4d,0x7f,0x6e, 0x00, 0x00};

//const unsigned char chineseTxtCiphHopDTX[] = {0x80, 0x00,0x00, 'C',0x00,'i',0x00,'p',0x00,'h',0x00,' ',0x00,'H',0x00,'o',0x00,'p',0x00,' ',0x00,'D',0x00,'T',0x00,'X',0x00,0x00, 0x00}; /*MC, SPR1209*/
const unsigned char chineseTxtCiphHopDTX[] = {0x80, 0x00,0x00,0x43,0x00,0x69,0x00,0x70,0x00,0x68,0x00,0x20,0x00,0x48,0x00,0x6f,0x00,0x70,0x00,0x20,0x00,0x44,0x00,0x54,0x00,0x58, 0x00, 0x00};
const unsigned char chineseTxtPhoneSettings[] =  {0x80, 0x00,0x62,0x4b,0x67,0x3a,0x8b,0xbe,0x7f,0x6e,0x00,0x00}; //SPR#1113 - DS - Added text id for renamed main menu item "Phone Settings"
const unsigned char chineseTxtNetworkServices[] =  {0x80, 0x00,0x7f,0x51,0x7e,0xdc,0x67,0x0d,0x52,0xa1,0x00,0x00};  //SPR#1113 - DS - Added text id for new main menu item "Netw. Services"
const unsigned char chineseTxtCallDeflection[] = {0x80, 0x00,0x54,0x7c,0x53,0xeb,0x8f,0x6c,0x79,0xfb,0x00,0x00};   //SPR#1113 - DS - Added text id for new menu item "Call Deflection" (CD)
const unsigned char chineseTxtConcat[]= {0x80, 0x00,0x94,0xfe,0x63,0xa5, 0x00, 0x00};

/*MC SPR 1319, created new unicode strings for colour menu*/
const unsigned char chineseTxtColourMenu[] = {0x80, 0x00,0x5f,0x69,0x82,0x72,0x76,0x84, 0x00, 0x00};
const unsigned char chineseTxtColourBlack[] = {0x80, 0x00,0x9e,0xd1,0x82,0x72, 0x00, 0x00};

const unsigned char chineseTxtColourBlue[] = {0x80, 0x00,0x84,0xdd,0x82,0x72,0x00,0x00};
const unsigned char chineseTxtColourCyan[] =  {0x80, 0x00,0x97,0x52,0x82,0x72,0x00,0x00};
const unsigned char chineseTxtColourGreen[] =  {0x80, 0x00,0x7e,0xff,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourMagenta[] =  {0x80, 0x00,0x6d,0x0b,0x7e,0xa2,0x00,0x00};
const unsigned char chineseTxtColourYellow[] =  {0x80, 0x00,0x9e,0xc4,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourRed[] =  {0x80, 0x00,0x7e,0xa2,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourWhite[] =  {0x80, 0x00,0x76,0x7d,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourPink[] =  {0x80, 0x00,0x7c,0x89,0x7e,0xa2, 0x00, 0x00};
const unsigned char chineseTxtColourOrange[] = {0x80, 0x00,0x6a,0x59,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtTxtColourLightBlue[] =  {0x80, 0x00,0x6d,0x45,0x84,0xdd,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourLightGreen[] = {0x80, 0x00, 0x6d,0x45,0x7e,0xff,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtTxtColourPaleYellow[] = {0x80, 0x00,0x6d,0x45,0x9e,0xc4,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtColourTransparent[] =  {0x80, 0x00,0x90,0x0f,0x66,0x0e, 0x00, 0x00};
const unsigned char chineseTxtIdleScreenBackground[] =  {0x80, 0x00,0x5f,0x85,0x67,0x3a,0x75,0x4c,0x97,0x62,0x80,0xcc,0x66,0x6f,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtIdleScreenSoftKey[] =  {0x80, 0x00,0x5f,0x85,0x67,0x3a,0x75,0x4c,0x97,0x62,0x8f,0x6f,0x95,0x2e, 0x00, 0x00};
const unsigned char chineseTxtIdleScreenText[] =  {0x80, 0x00,0x5f,0x85,0x67,0x3a,0x75,0x4c,0x97,0x62,0x65,0x87,0x67,0x2c, 0x00, 0x00};
const unsigned char chineseTxtIdleScreenNetworkName[] =  {0x80, 0x00,0x5f,0x85,0x67,0x3a,0x75,0x4c,0x97,0x62,0x7f,0x51,0x7e,0xdc,0x68,0x07,0x8b,0xc6,0x00, 0x00};
const unsigned char chineseTxtGeneralBackground[] =  {0x80, 0x00,0x80,0xcc,0x66,0x6f, 0x00, 0x00};
const unsigned char chineseTxtMenuSoftKeyForeground[] =  {0x80, 0x00,0x83,0xdc,0x53,0x55,0x8f,0x6f,0x95,0x2e,0x52,0x4d,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseTxtMenuSoftKeyBackground[] =  {0x80, 0x00,0x83,0xdc,0x53,0x55,0x8f,0x6f,0x95,0x2e,0x80,0xcc,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseTxtSubMenuHeadingText[] =  {0x80, 0x00,0x5b,0x50,0x83,0xdc,0x53,0x55,0x68,0x07,0x98,0x98,0x65,0x87,0x5b,0x57,0x00, 0x00};
const unsigned char chineseTxtTxtSubMenuHeadingBackground[] =  {0x80, 0x00,0x5b,0x50,0x83,0xdc,0x53,0x55,0x68,0x07,0x98,0x98,0x80,0xcc,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseSubMenuTextForeground[] =  {0x80, 0x00,0x5b,0x50,0x83,0xdc,0x53,0x55,0x52,0x4d,0x66,0x6f,0x82,0x72, 0x00, 0x00};
const unsigned char chineseTxtTxtSubMenuTextBackground[] =  {0x80, 0x00,0x5b,0x50,0x83,0xdc,0x53,0x55,0x80,0xcc,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseTxtSubMenuSelectionBar[]= {0x80, 0x00,0x5b,0x50,0x83,0xdc,0x53,0x55,0x90,0x09,0x62,0xe9,0x98,0x79,0x00, 0x00};
const unsigned char chineseTxtPopUpTextForeground[] =  {0x80, 0x00,0x5f,0x39,0x51,0xfa,0x65,0x87,0x67,0x2c,0x52,0x4d,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseTxtPopUpTextBackground[] =  {0x80, 0x00,0x5f,0x39,0x51,0xfa,0x65,0x87,0x67,0x2c,0x80,0xcc,0x66,0x6f,0x82,0x72,0x00, 0x00};
const unsigned char chineseTxtSelectBook[] =  {0x80, 0x00,0x90,0x09,0x62,0xe9,0x75,0x35,0x8b,0xdd,0x67,0x2c,0x00,0x00};	/* SPR1112 - SH */
const unsigned char chineseTxtSim[] = {0x80, 0x00,0x00,0x53,0x00,0x49,0x00,0x4d,0x00,  0x00};
const unsigned char chineseTxtMove[] = {0x80, 0x00,0x79,0xfb,0x52,0xa8,0x00, 0x00 };
const unsigned char chineseTxtCopy[] = {0x80, 0x00,0x59,0x0d,0x52,0x36,0x00, 0x00 };
const unsigned char chineseTxtThisEntry[]= {0x80, 0x00,0x5f,0x53,0x52,0x4d,0x98,0x79,0x00, 0x00};
/*mc end*/
const unsigned char chineseTxtColour[] 		= { 0x80, 0x00,0x98,0x9c,0x82,0x72,0x00,0x00};
const unsigned char chineseTxtLoser[] 		= { 0x80, 0x00,0x4f,0x60,0x8f,0x93,0x4e,0x86,0x00,0x00};
const unsigned char chineseTxtWinner[] 		= { 0x80, 0x00,0x4f,0x60,0x8d,0x62,0x4e,0x86,0x00,0x00};
const unsigned char chineseTxtPlayAgain[] 	= { 0x80, 0x00,0x51,0x8d,0x73,0xa9,0x4e,0x00,0x6b,0x21,0x00,0x3f,0x00,0x00};
const unsigned char chineseTxtBackground[] = { 0x80, 0x00,0x80,0xcc,0x66,0x6f,0x00,0x00};
const unsigned char chineseTxtNetworkIcon[] = { 0x80, 0x00,0x7f,0x51,0x7e,0xdc,0x68,0x07,0x8b,0xc6,0x00,0x00}; /*AP - 29-01-03 - Add to language table */        
const unsigned char chineseTxtIdle [] 	= {0x80, 0x00,0x5f,0x85,0x67,0x3a,0x75,0x4c,0x97,0x62,0x00,0x00 };
const unsigned char chineseTxtSquares[] 	= {0x80, 0x00,0x65,0xb9,0x57,0x57,0x00,0x00 };
const unsigned char chineseTxtO2[] 		= {0x80, 0x00,0x00,0x4f,0x00,0x32,0x00,0x00 };
const unsigned char chineseTxtTree[] 	= {0x80, 0x00,0x68,0x11,0x00,0x00 };
const unsigned char chineseTxtTILogo[] 	= {0x80, 0x00, 0x00,0x54,0x00,0x49,0x68,0x07,0x8b,0xc6,0x00,0x00 };
const unsigned char chineseTxtSunset[] 	= {0x80, 0x00,0x65,0xe5,0x84,0x3d,0x00,0x00 };
const unsigned char chineseTxtEdinburgh[] = {0x80, 0x00, 0x72,0x31,0x4e,0x01,0x58,0x21,0x00,0x00 };
const unsigned char chineseTxtCallBack[] = {0x80, 0x00,0x56,0xde,0x62,0xe8,0x00,0x00};   //SPR#1113 - DS - Added text id for new menu item "Call Back" (CCBS)
const unsigned char chineseTxtCallBackQuery[] = {0x80, 0x00,0x56,0xde,0x62,0xe8,0x96,0x1f,0x52,0x17,0x00,0x00};   // Marcus: CCBS: 15/11/2002
const unsigned char chineseTxtRejected[] = {0x80, 0x00,0x88,0xab,0x62,0xd2,0x7e,0xdd,0x00,0x00};   // Marcus: CCBS: 15/11/2002
const unsigned char chineseTxtDeflectCall[] = {0x80, 0x00,0x8f,0x6c,0x79,0xfb,0x54,0x7c,0x53,0xeb,0x00,0x3a,0x00,0x00}; //SPR 1392 calldeflection
const unsigned char chineseTxtDeflectCallTo[] = {0x80, 0x00,0x8f,0x6c,0x79,0xfb,0x52,0x30,0x00,0x3a,0x00,0x00}; //SPR 1392  calldeflection
const unsigned char chineseTxtDeflect[] = {0x80, 0x00,0x8f,0x6c,0x79,0xfb,0x00,0x00}; //SPR 1392  calldeflection
const unsigned char chineseTxtDeflectingCallTo[]= {0x80, 0x00,0x6b,0x63,0x57,0x28,0x8f,0x6c,0x79,0xfb,0x52,0x30,0x00,0x3a,0x00,0x00}; //SPR 1392  calldeflection
const unsigned char chineseTxtActiveCall[] = {0x80, 0x00,0x6f,0xc0,0x6d,0x3b,0x54,0x7c,0x53,0xeb,0x00,0x00};	/* SPR#1352 - SH*/
const unsigned char chineseTxtHeldCall[] = {0x80, 0x00,0x4f,0xdd,0x63,0x01,0x54,0x7c,0x53,0xeb,0x00,0x00};
const unsigned char chineseTxtActiveTTYCall[] = {0x80, 0x00,0x6f,0xc0,0x6d,0x3b,0x75,0x35,0x4f,0x20,0x54,0x7c,0x53,0xeb,0x00,0x00};
const unsigned char chineseTxtHeldTTYCall[] = {0x80, 0x00,0x4f,0xdd,0x63,0x01,0x75,0x35,0x4f,0x20,0x54,0x7c,0x53,0xeb,0x00,0x00};
const unsigned char chineseTxtTTY[] = {0x80, 0x00,0x75,0x35,0x4f,0x20,0x00,0x00};	
/*x0039928 OMAPS00097714 - HCO/VCO implementation - added strings*/
#ifdef FF_TTY_HCO_VCO
const unsigned char chineseTxtTTYAll[] = {0x80, 0x00, 0x00, 'T', 0x00, 'T', 0x00, 'Y', 0x00, ' ', 0x00,'A', 0x00, 'l', 0x00, 'l', 0x00, 0x00 };
const unsigned char chineseTxtTTYVco[] = {0x80, 0x00, 0x00, 'T', 0x00, 'T', 0x00, 'Y', 0x00, ' ', 0x00,'V', 0x00, 'c', 0x00, 'o', 0x00, 0x00 };
const unsigned char chineseTxtTTYHco[] = {0x80, 0x00, 0x00, 'T', 0x00, 'T', 0x00, 'Y', 0x00, ' ', 0x00,'H', 0x00, 'c', 0x00, 'o', 0x00, 0x00 };
#endif
const unsigned char chineseTxtOnNextCall[] = {0x80, 0x00,0x5f,0x53,0x4e,0x0b,0x4e,0x00,0x4e,0x2a,0x54,0x7c,0x53,0xeb,0x00,0x00};
const unsigned char chineseTxtAlwaysOn[] = {0x80, 0x00,0x60,0x3b,0x57,0x28,0x00,0x00};
const unsigned char chineseTxtTTYCalling[] = {0x80, 0x00,0x60,0x3b,0x57,0x28,0x00,0x00};
const unsigned char chineseTxtIncomingTTYCall[] = {0x80, 0x00,0x75,0x35,0x4f,0x20,0x54,0x7c,0x53,0xeb,0x4e,0x2d,0x00,0x2e,0x00,0x2e,0x00,0x2e,0x00,0x00};
const unsigned char chineseTxtProvisioned[] = { 0x80, 0x00,0x5d,0xf2,0x4f,0x9b,0x5e,0x94,0x00, 0x00 }; // Marcus: Issue 1652: 03/02/2003
const unsigned char chineseTxtExecuted[] = { 0x80, 0x00, 0x5d,0xf2,0x62,0x67,0x88,0x4c,0x00, 0x00 }; // Marcus: Issue 1652: 03/02/2003
const unsigned char chineseTxtMultiparty[] = { 0x80, 0x00,0x00, 'M', 0x00, 'u', 0x00, 'l', 0x00, 't', 0x00, 'i', 0x00, 'p', 0x00, 'a', 0x00, 'r', 0x00, 't', 0x00, 'y', 0x00, 0x00 }; // AP: Issue 1749: 06/03/03
const unsigned char chineseTxtBootTime[] = {0x80, 0x00, 0x00,'B', 0x00,'o',0x00,'o',0x00,'t',0x00,' ',0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,0x00}; /*OMAPS0091029 x0039928(sumanth)*/
/*MC, SPR 1526, end*/
const unsigned char chineseTxtCommand[] = { 0x80, 0x00,0x00,'C', 0x00, 'o', 0x00, 'm', 0x00, 'm', 0x00, 'a', 0x00, 'n', 0x00, 'd', 0x00, 0x00 }; // NDH : SPR#1869 : 08/04/2003
const unsigned char chineseTxtCompleted[] = {0x80, 0x00, 0x00,'C', 0x00, 'o', 0x00, 'm', 0x00, 'p', 0x00, 'l', 0x00, 'e', 0x00, 't', 0x00, 'e', 0x00, 'd', 0x00, 0x00 }; // NDH : SPR#1869 : 08/04/2003
const unsigned char chineseTxtDialup[] = {0x80, 0x00,0x00,'D',0x00,'i',0x00,'a',0x00,'l',0x00,'u',0x00,'p',0x00,0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtAdvanced[] = {0x80, 0x00,0x00,'A',0x00,'d',0x00,'v',0x00,'a',0x00,'n',0x00,'c',0x00,'e',0x00,'d',0x00,0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtGPRSDialupGPRS[] = {0x80, 0x00,0x00,'(',0x00,'G',0x00,'P',0x00,'R',0x00,'S',0x00,')',0x00,'/',0x00,'D',0x00,'i',0x00,'a',0x00,'l',0x00,'u',0x00,'p',0x00,0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtGPRSDialupDialup[] = {0x80, 0x00,0x00,'G',0x00,'P',0x00,'R',0x00,'S',0x00,'/',0x00,'(',0x00,'D',0x00,'i',0x00,'a',0x00,'l',0x00,'u',0x00,'p',0x00,')',0x00,0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtProfiles[] = {0x80, 0x00,0x00,'P',0x00,'r',0x00,'o',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,'s', 0x00, 0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtNewProfile[] = {0x80, 0x00,0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'P',0x00,'r',0x00,'o',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00, 0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtResetProfiles[] = {0x80, 0x00, 0x00,'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'P',0x00,'r',0x00,'o',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,0x00}; /* SPR#2324 - SH */
const unsigned char chineseTxtChooseReplacement1[] = { 0x80, 0x00,0x00,'C', 0x00, 'h', 0x00, 'o', 0x00, 'o', 0x00, 's', 0x00, 'e', 0x00, 0x00 }; /* SPR#2354 - SH */
const unsigned char chineseTxtChooseReplacement2[] = { 0x80, 0x00,0x00,'R', 0x00, 'e', 0x00, 'p', 0x00, 'l', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'm', 0x00, 'e', 0x00, 'n', 0x00, 't', 0x00, 0x00 }; /* SPR#2354 - SH */
const unsigned char chineseTxtNewPinAndConfirm[] = {0x80, 0x00, 0x00,'N', 0x00, 'e', 0x00, 'w', 0x00, ' ', 0x00, 'P', 0x00, 'I', 0x00, 'N', 0x00, ' ', 0x00, '&', 0x00, ' ', 0x00, 'C', 0x00, 'o', 0x00, 'n', 0x00, 'f', 0x00, '.', 0x00, 0x00 }; // NDH : CQ11260 : 12/08/2003
const unsigned char chineseTxtAreDifferent[] = { 0x80, 0x00,0x00,'a', 0x00, 'r', 0x00, 'e', 0x00, ' ', 0x00, 'd', 0x00, 'i', 0x00, 'f', 0x00, 'f', 0x00, 'e', 0x00, 'r', 0x00, 'e', 0x00, 'n', 0x00, 't', 0x00, 0x00 }; // NDH : CQ11260 : 12/08/2003
const unsigned char chineseTxtDataCounterReset[] = {0x80, 0x00,0x00, 'R',0x00,'e',0x00,'s',0x00,'e',0x00,'t',0x00,' ',0x00,'C',0x00,'o',0x00,'u',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,'?',0x00,0x00}; /* SPR#2346 - SH */
const unsigned char chineseTxtCallDate[] = { 0x80, 0x00,0x00, 'D', 0x00, 'a', 0x00, 't', 0x00, 'e', 0x00, ' ', 0x00, ':', 0x00, ' ', 0x00, 0x00 }; /* API - 05/09/03 - SPR2357 - Added */
const unsigned char chineseTxtCallTime[] = { 0x80, 0x00,0x00, 'T', 0x00, 'i', 0x00, 'm', 0x00, 'e', 0x00, ' ', 0x00, ':', 0x00, ' ', 0x00, 0x00 }; /* API - 05/09/03 - SPR2357 - Added */
const unsigned char chineseTxtInvalidMessage[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'v',0x00,'a',0x00,'l',0x00,'i',0x00,'d',0x00,' ',0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,'g',0x00,'e',0x00,0x00};/*MC SPR 2530*/
const unsigned char chineseTxtIncompleteMessage[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'c',0x00,'o',0x00,'m',0x00,'p',0x00,'l',0x00,'e',0x00,'t',0x00,'e',0x00,' ',0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,'g',0x00,'e',0x00,0x00};/*MC SPR 2530*/
const unsigned char chineseTxtSRR[] = { 0x00,'S',0x00,'t',0x00,'a',0x00,'t',0x00,'u',0x00,'s',0x00,' ',0x00,'R',0x00,'e',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,0x00 }; // Marcus: Issue 1170: 30/09/2002
//March 2, 2005    REF: CRR 11536 x0018858
/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtRP[] = { 0x00,'R',0x00,'e',0x00,'p',0x00,'l',0x00,'y',0x00,' ',0x00,'P',0x00,'a',0x00,'t',0x00,'h',0x00,0x00 }; // added this string.for the reply path.
const unsigned char chineseTxtDelivered[] = { 0x00,'D',0x00,'e',0x00,'l',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'e',0x00,'d',0x00,0x00 }; // Marcus: Issue 1170: 07/10/2002
const unsigned char chineseTxtTempError[] = { 0x00,'T',0x00,'e',0x00,'m',0x00,'p',0x00,'.',0x00,' ',0x00,'E',0x00,'r',0x00,'r',0x00,'o',0x00,'r',0x00, 0x00 }; // Marcus: Issue 1170: 07/10/2002
/* SUJATHA MMS integration */
//const unsigned char chineseTxtDownload[] = {0x00,'C',0x00,'D',0x00,'o',0x00,'w',0x00,'n',0x00,'l',0x00,'o',0x00,'a',0x00,'d',0x00,0x00};
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)  //added by ellen
const unsigned char chineseTxtPushMessages[] = {0x00,'P',0x00,'u',0x00,'s',0x00,'h',0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,0x00,'g',0x00,'e',0x00,'s',0x00,0x00};
const unsigned char chineseTxtShowMessages[] = {0x00,'S',0x00,'h',0x00,'o',0x00,'w',0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,0x00,'g',0x00,'e',0x00,'s',0x00,0x00};
#endif
const unsigned char chineseTxtMMS[] = {0x80, 0x00,0x00,'M', 0x00,'M', 0x00,'S', 0x00,0x00}; //x0035544 07-11-2005
const unsigned char chineseTxtEMS[] = {0x80, 0x00,0x00, 'E', 0x00,'M', 0x00,'S', 0x00,0x00};
const unsigned char chineseTxtMMSCreate[] = {0x80, 0x00,0x00,'C',0x00,'r',0x00,'e',0x00,'a',0x00,'t',0x00,'e',0x00,' ',0x00,'M',0x00,'M',0x00,'S', 0x00,0x00};
const unsigned char chineseTxtEMSCreate[] = {0x80, 0x00,0x00,'C',0x00,'r',0x00,'e',0x00,'a',0x00,'t',0x00,'e',0x00,' ',0x00,'E',0x00,'M',0x00,'S', 0x00,0x00};
const unsigned char chineseTxtMMSInbox[] = {0x80, 0x00,0x00,'I',0x00,'n',0x00,'b',0x00,'o',0x00,'x', 0x00,0x00};
const unsigned char chineseTxtMMSUnsent[] = {0x80, 0x00,0x00,'U',0x00,'n',0x00,'s',0x00,'e',0x00,'n',0x00,'t', 0x00,0x00};
//CRR: 25291 - xrashmic 14 Oct 2004
const unsigned char chineseTxtMMSSent[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'n',0x00,'t', 0x00,0x00};
const unsigned char chineseTxtMMSSettings[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'t',0x00,'t',0x00,'i',0x00,'n',0x00,'g',0x00,'s', 0x00,0x00}; 
const unsigned char chineseTxtRetrieveCondition[] = {0x80, 0x00,0x00,'R',0x00,'e',0x00,'t',0x00,'r',0x00,'i',0x00,'e',0x00,'v',0x00,'a',0x00,'l',0x00,' ',0x00,'T',0x00,'y',0x00,'p',0x00,'e',0x00,0x00}; //x0035544 07-11-2005
const unsigned char chineseTxtMMSInsertPicture[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'s',0x00,' ',0x00,'P',0x00,'i',0x00,'c',0x00,'t',0x00,'u',0x00,'r',0x00,'e',0x00,0x00};
const unsigned char chineseTxtMMSInsertSound[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'s',0x00,' ',0x00,'S',0x00,'o',0x00,'u',0x00,'n',0x00,'d',0x00,0x00};
/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtMMSInsertText[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'s',0x00,' ',0x00,'T',0x00,'e',0x00,'x',0x00,'t',0x00,0x00};
const unsigned char chineseTxtMMSInsertSlide[] = {0x80, 0x00,0x00,'I',0x00,'n',0x00,'s',0x00,' ',0x00,'S',0x00,'l',0x00,'i',0x00,'d',0x00,'e', 0x00,0x00};
const unsigned char chineseTxtMMSDelete[] = {0x80, 0x00,0x00,'D',0x00,'e',0x00,'l',0x00,'e',0x00,'t',0x00,'e', 0x00,0x00};
const unsigned char chineseTxtMMSPreview[] = {0x80, 0x00,0x00,'P',0x00,'r',0x00,'e',0x00,'v',0x00,'i',0x00,'e',0x00,'w', 0x00,0x00};
const unsigned char chineseTxtMMSDeleteSlide[] = {0x80, 0x00,0x00,'D',0x00,'e',0x00,'l',0x00,' ',0x00,'S',0x00,'l',0x00,'i',0x00,'d',0x00,'e', 0x00,0x00};
const unsigned char chineseTxtMMSDeletePicture[] = {0x80, 0x00,0x00,'D',0x00,'e',0x00,'l',0x00,' ',0x00,'P',0x00,'i',0x00,'c',0x00,'t',0x00,'u',0x00,'r',0x00,'e', 0x00,0x00};
const unsigned char chineseTxtMMSDeleteSound[] = {0x80, 0x00,0x00, 'D',0x00,'e',0x00,'l',0x00,' ',0x00,'S',0x00,'o',0x00,'u',0x00,'n',0x00,'d',0x00,0x00};
const unsigned char chineseTxtMMSDeleteText[] = {0x80, 0x00,0x00,'D',0x00,'e',0x00,'l',0x00,' ',0x00,'T',0x00,'e',0x00,'x',0x00,'t', 0x00,0x00};
const unsigned char chineseTxtMMSInsert[] = {0x80, 0x00,0x00, 'I',0x00,'n',0x00,'s',0x00,'e',0x00,'r',0x00,'t',0x00,0x00};
const unsigned char chineseTxtMMSSendSucess[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'n',0x00,'t', 0x00,0x00};
/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtMMSSendFail[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'n',0x00,'d',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'e',0x00,'d', 0x00,0x00};
const unsigned char chineseTxtMMSSendng[] = {0x80, 0x00,0x00, 'S',0x00,'e',0x00,'n',0x00,'d',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtMMSDownloading[] = {0x80, 0x00,0x00, 'R',0x00,'e',0x00,'c',0x00,'e',0x00,'i',0x00,'v',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtNewMMS[] = {0x80, 0x00,0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'M',0x00,'M',0x00,'S', 0x00,0x00};
const unsigned char chineseTxtMMSRcvFail[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'n',0x00,'d',0x00,' ',0x00,'F',0x00,'a',0x00,'i',0x00,'l',0x00,'e',0x00,'d', 0x00,0x00};
const unsigned char chineseTxtMMSDots[] = {0x80, 0x00,0x00, 'M',0x00,'M',0x00,'S',0x00,'.',0x00,'.',0x00,'.',0x00,0x00};
const unsigned char chineseTxtExtract[] = {0x80, 0x00,0x00, 'E',0x00,'x',0x00,'t',0x00,'r',0x00,'a',0x00,'c',0x00,'t',0x00,0x00};
        const unsigned char chineseTxtMMSNotification[]= {0x80, 0x00,0x00, 'M',0x00,'M',0x00,'S',0x00,' ',0x00,'N',0x00,'o',0x00,'t',0x00,'i',0x00,'f',0x00,'i',0x00,'c',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00};  //x0035544 07-11-2005
const unsigned char chineseTxtImmediate[]= {0x80, 0x00,0x00, 'I',0x00,'m',0x00,'m',0x00,'e',0x00,'d',0x00,'i',0x00,'a',0x00,'t',0x00,'e',0x00,0x00};  //x0035544 07-11-2005
        const unsigned char chineseTxtDeferred[]= {0x80, 0x00,0x00, 'D',0x00,'e',0x00,'f',0x00,'e',0x00,'r',0x00,'r',0x00,'e',0x00,'d',0x00,0x00};  //x0035544 07-11-2005
        const unsigned char chineseTxtMMSRetrieving[]= {0x80, 0x00,0x00, 'M',0x00,'M',0x00,'S',0x00,' ',0x00,'R',0x00,'e',0x00,'t',0x00,'r',0x00,'i',0x00,'e',0x00,'v',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};  //x0035544 07-11-2005 
        const unsigned char chineseTxtMMSRetrieved[]= {0x80, 0x00,0x00, 'M',0x00,'M',0x00,'S',0x00,' ',0x00,'R',0x00,'e',0x00,'t',0x00,'r',0x00,'i',0x00,'e',0x00,'v',0x00,'e',0x00,'d',0x00,0x00};   //x0035544 07-11-2005
const unsigned char chineseTxtEMSNew[] = {0x80, 0x00, 0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'E',0x00,'M',0x00,'S',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtType[] = {0x80, 0x00, 0x00,'T',0x00,'y',0x00,'p',0x00,'e',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtObject[] = {0x80, 0x00, 0x00,'O',0x00,'b',0x00,'j',0x00,'e',0x00,'c',0x00,'t',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtLarge[] = {0x80, 0x00, 0x00,'T',0x00,'o',0x00,'o',0x00,' ',0x00,'L',0x00,'a',0x00,'r',0x00,'g',0x00,'e',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtInsertPic[] = {0x80, 0x00, 0x00,'I',0x00,'n',0x00,'s',0x00,' ',0x00,'p',0x00,'r',0x00,'e',0x00,' ',0x00,'p',0x00,'i',0x00,'c',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtInsertSound[] = {0x80, 0x00, 0x00,'I',0x00,'n',0x00,'s',0x00,' ',0x00,'p',0x00,'r',0x00,'e',0x00,' ',0x00,'s',0x00,'o',0x00,'u',0x00,'n',0x00,'d',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtAllObjects[] = {0x80, 0x00, 0x00,'O',0x00,'b',0x00,'j',0x00,'e',0x00,'c',0x00,'t',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtExtracted[] = {0x80, 0x00, 0x00,'E',0x00,'x',0x00,'t',0x00,'r',0x00,'a',0x00,'c',0x00,'t',0x00,'e',0x00,'d',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtExtracting[] = {0x80, 0x00, 0x00,'E',0x00,'x',0x00,'t',0x00,'r',0x00,'a',0x00,'c',0x00,'t',0x00,'i',0x00,'n',0x00,'g',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtNoObjects[] = {0x80, 0x00, 0x00,'N',0x00,'o',0x00,' ',0x00,'O',0x00,'b',0x00,'j',0x00,'e',0x00,'c',0x00,'t',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtExtractable[] = {0x80, 0x00, 0x00,'E',0x00,'x',0x00,'t',0x00,'r',0x00,'a',0x00,'c',0x00,'t',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00, 0x00}; //xrashmic 26 Aug, 2004 MMI-SPR-23931
const unsigned char chineseTxtLockAls[] = { 0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'A',0x00,'L',0x00,'S',0x00,0x00 }; // NDH :CQ16317
const unsigned char chineseTxtUnlockAls[] = { 0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'A',0x00,'L',0x00,'S',0x00, 0x00 }; // NDH :CQ16317
const unsigned char chineseTxtAlsLocked[] = { 0x00,'A',0x00,'L',0x00,'S',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,0x00 }; // NDH :CQ16317
const unsigned char chineseTxtAlsUnlocked[] = { 0x00,'A',0x00,'L',0x00,'S',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,0x00 }; // NDH :CQ16317
/* x0045876, 14-Aug-2006 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtLimService[] = { 0x80,0x00,0x00,'L',0x00,'i',0x00,'m',0x00,'i',0x00,'t',0x00,'e',0x00,'d',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00 }; // SPR12653
const unsigned char chineseTxtFDNName[] = {0x80, 0x00,0x00, 'F',0x00,'D',0x00,'N',0x00,' ',0x00,'N',0x00,'u',0x00,'m',0x00,'b',0x00,'e',0x00,'r',0x00,'s',0x00,0x00}; //MMI SPR 18555 //localization required.
const unsigned char chineseTxtFDNListFull[] = {0x80, 0x00,0x00, 'F',0x00,'D',0x00,'N',0x00,' ',0x00,'L',0x00,'i',0x00,'s',0x00,'t',0x00,'F',0x00,'u',0x00,'l',0x00,'l',0x00,0x00};//SPR 31710 x0021308:RamG//localization required. 
#ifdef FF_CPHS_REL4  
const unsigned char chineseTxtMSP[] = {0x80, 0x00, 0x00,'M',0x00,'S',0x00,'P',0x00,' ',0x00,'P',0x00,'r',0x00,'o',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtMessageStatus[] = {0x80, 0x00, 0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,'g',0x00,'e',0x00,' ',0x00,'S',0x00,'t',0x00,'a',0x00,'t',0x00,'u',0x00,'s',0x00,0x00};
const unsigned char chineseTxtMailBox[] = {0x80, 0x00, 0x00,'M',0x00,'a',0x00,'i',0x00,'l',0x00,'B',0x00,'o',0x00,'x',0x00,0x00};
const unsigned char chineseTxtElectronic[] = {0x80, 0x00, 0x00,'E',0x00,'l',0x00,'e',0x00,'c',0x00,'t',0x00,'r',0x00,'o',0x00,'n',0x00,'i',0x00,'c',0x00,0x00};
#endif
//   Apr 06, 2005	REF: ENH 30011 xdeepadh
//Strings for Camera, Mp3 and midi test application are 
//under the respective flags.
//Nov 14, 2005    REF: OMAPS00044445 xdeepadh

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */
// #if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
const unsigned char chineseTxtPlayerMonoChannel[] = {0x80, 0x00,0x00,'M',0x00,'o',0x00,'n',0x00,'o', 0x00,0x00};
const unsigned char chineseTxtPlayerStereoChannel[] = {0x80, 0x00,0x00,'S',0x00,'t',0x00,'e',0x00,'r', 0x00,'e',0x00,'o',0x00,0x00};
const unsigned char chineseTxtPlayerSetChannels[] = {0x80, 0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n', 0x00,'n',0x00,'e',0x00,'l',0x00,'s',0x00,' ',0x00,'s',0x00,'e', 0x00,'t',0x00,'t',0x00,'i',0x00,'n', 0x00,'g',0x00,'s',0x00,0x00};
const unsigned char chineseTxtPlayerPlay[] = {0x80, 0x00,0x00,'P',0x00,'l',0x00,'a',0x00,'y', 0x00,' ',0x00,' ',0x00,' ',0x00,'f', 0x00,'i',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtPlayerTest[] = {0x80, 0x00,0x00,'A',0x00,'u',0x00,'d',0x00,'i', 0x00,'o',0x00,' ',0x00,'P',0x00,'l',0x00,'a',0x00,'y', 0x00,'e',0x00,'r',0x00,0x00};
const unsigned char chineseTxtPlayerFileSelected[] = {0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00,' ',0x00,'S',0x00,'e',0x00,'l',0x00,'e',0x00,'c',0x00,'t', 0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtPlayerSelectFile[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'l',0x00,'e', 0x00,'c',0x00,'t',0x00,' ',0x00,' ', 0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtPlayerPause[] = {0x80, 0x00,0x00,'P',0x00,'a',0x00,'u',0x00,'s', 0x00,'e',0x00,0x00};
const unsigned char chineseTxtPlayerResume[] = {0x80, 0x00,0x00,'R',0x00,'e',0x00,'s',0x00,'u',0x00,'m', 0x00,'e',0x00,0x00};
const unsigned char chineseTxtPlayerStop[] = {0x80, 0x00,0x00,'S',0x00,'t',0x00,'o',0x00,'p',0x00,0x00};
const unsigned char chineseTxtPlayerPlaying[] = {0x80, 0x00,0x00,'P',0x00,'l',0x00,'a',0x00,'y',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtPlayerForward[] = {0x80, 0x00, 0x00,'F',0x00,'o',0x00,'r',0x00,'w',0x00,'a',0x00,'r',0x00,'d',0x00,0x00};  /* OMAPS00070659  x0039928  */
const unsigned char chineseTxtPlayerRewind[] = {0x80, 0x00,0x00,'R',0x00,'e',0x00,'w',0x00,'i',0x00,'n',0x00,'d',0x00,0x00};
const unsigned char chineseTxtPlayerForwarding[] = {0x80, 0x00, 0x00,'F',0x00,'o',0x00,'r',0x00,'w',0x00,'a',0x00,'r',0x00,'d',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtPlayerRewinding[] = {0x80, 0x00,0x00,'R',0x00,'e',0x00,'w',0x00,'i',0x00,'n',0x00,'d',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};  /* OMAPS00070659  x0039928  */
//	Jul 18, 2005    REF: SPR 31695   xdeepadh
const unsigned char chineseTxtPlayerNoFile[] = {0x80, 0x00,0x00,'N',0x00,'o',0x00,' ',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,' ',0x00,'l',0x00,'o',0x00,'a',0x00,'d',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtPlayerOptionNotImplemented[] = {0x80, 0x00,0x00,'N',0x00,'o',0x00,'t',0x00,' ',0x00,'I',0x00,'m',0x00,'p',0x00,'l',0x00,'e',0x00,'m',0x00,'e',0x00,'n',0x00,'t',0x00,'e',0x00,'d',0x00,0x00};
// #endif  //FF_MMI_TEST_MP3
/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */

//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
/* a0393213 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtCameraTest[] = {0x80, 0x00,0x00,'C',0x00,'a',0x00,'m',0x00,'e', 0x00,'r',0x00,'a',0x00,' ',0x00,'A', 0x00,'p',0x00,'p',0x00,0x00};
const unsigned char chineseTxtViewFinder[] = {0x80, 0x00,0x00,'V',0x00,'i',0x00,'e',0x00,'w', 0x00,'f',0x00,'i',0x00,'n',0x00,'d',0x00,'e',0x00,'r',0x00,0x00};
const unsigned char chineseTxtSnapshot[] = {0x80, 0x00,0x00,'T',0x00,'a',0x00,'k',0x00,'e',0x00,' ',0x00,'S',0x00,'n',0x00,'a',0x00,'p',0x00,'s',0x00,'h',0x00,'o',0x00,'t',0x00,0x00};
const unsigned char chineseTxtSaveSnapshot[] = {0x80, 0x00,0x00,'S',0x00,'a',0x00,'v',0x00,'e',0x00,' ',0x00,'S',0x00,'n',0x00,'a',0x00,'p',0x00,'s',0x00,'h',0x00,'o',0x00,'t',0x00,0x00};
const unsigned char chineseTxtSnapshotSaved[] = {0x80, 0x00,0x00,'S',0x00,'n',0x00,'a',0x00,'p',0x00,'s',0x00,'h',0x00,'o',0x00,'t',0x00,' ',0x00,'S',0x00,'a',0x00,'v',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtSoftSnap[] = {0x80, 0x00,0x00,'S',0x00,'n',0x00,'a',0x00,'p',0x00,'s',0x00,'h',0x00,'o',0x00,'t',0x00,0x00};
const unsigned char chineseTxtSoftSnapSave[] = {0x80, 0x00,0x00,'S',0x00,'a',0x00,'v',0x00,'e',0x00,0x00};
const unsigned char chineseTxtSnapSaving[] = {0x80, 0x00,0x00,'S',0x00,'a',0x00,'v',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtQuality[]  =  {0x80, 0x00,0x00,'Q',0x00,'u',0x00,'a',0x00,'l', 0x00,'i',0x00,'t',0x00,'y',0x00,0x00};
const unsigned char chineseTxtEconomy[] = {0x80, 0x00,0x00,'E',0x00,'c',0x00,'o',0x00,'n', 0x00,'o',0x00,'m',0x00,'y',0x00,0x00};
/* a0393213 (WR - integer conversion resulted in truncation) */
const unsigned char chineseTxtStandard[] = {0x80, 0x00,0x00,'S',0x00,'t',0x00,'a',0x00,'n', 0x00,'d',0x00,'a',0x00,'r',0x00,'d',0x00,0x00};
const unsigned char chineseTxtColor[] = {0x80, 0x00,0x00,'C',0x00,'o',0x00,'l',0x00,'o', 0x00,'r',0x00,0x00};
const unsigned char chineseTxtSepia[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'p',0x00,'i', 0x00,'a',0x00,0x00};
const unsigned char chineseTxtB_W[] = {0x80, 0x00,0x00,'B',0x00,'&',0x00,'W',0x00,0x00};
const unsigned char chineseTxtNegative[] = {0x80, 0x00,0x00,'N',0x00,'e',0x00,'g',0x00,'a', 0x00,'t',0x00,'i',0x00,'v',0x00,'e',0x00,0x00};
const unsigned char chineseTxtFilename[] = {0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtDigiZoom[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxtFrame[]= {0x80, 0x00,0x00,'F',0x00,'r',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtFrame1[]= {0x80, 0x00,0x00,'F',0x00,'r',0x00,'a',0x00,'m',0x00,'e',0x00,'1', 0x00,0x00};
const unsigned char chineseTxtFrame2[]= {0x80, 0x00,0x00,'F',0x00,'r',0x00,'a',0x00,'m',0x00,'e',0x00,'2', 0x00,0x00};
const unsigned char chineseTxtNoFrame[]= {0x80, 0x00,0x00,'N',0x00,'o',0x00,'F',0x00,'r',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtShootingMode[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxtSingleShot[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxt2Shots[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxt4Shots[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxt8Shots[] = {0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
#endif //FF_MMI_TEST_CAMERA
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Midi Test Application Strings were put under the flag FF_MMI_TEST_MIDI
#ifdef FF_MMI_TEST_MIDI
const unsigned char chineseTxtMidiTest[] = {0x80, 0x00,0x00,'M',0x00,'i',0x00,'d',0x00,'i', 0x00,' ',0x00,'A',0x00,'p',0x00,'p',0x00,0x00};
const unsigned char chineseTxtMidiChangeConfig[] = {0x80, 0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n', 0x00,'g',0x00,'e',0x00,' ',0x00,'C',0x00,'o',0x00,'n',0x00,'f', 0x00,'i',0x00,'g',0x00,0x00};
const unsigned char chineseTxtMidiPlay[] = {0x80, 0x00,0x00,'P',0x00,'l',0x00,'a',0x00,'y', 0x00,0x00};
const unsigned char chineseTxtMidiPlayAllFiles[] = {0x80, 0x00,0x00,'P',0x00,'l',0x00,'a',0x00,'y', 0x00,' ',0x00,'a',0x00,'l',0x00,'l',0x00,0x00};
const unsigned char chineseTxtMidiExit[] = {0x80, 0x00,0x00,'S',0x00,'t',0x00,'o',0x00,'p', 0x00,' ',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtChConfigFile[] = {0x80, 0x00,0x00,'B',0x00,'r',0x00,'o',0x00,'w', 0x00,'s',0x00,'e',0x00,' ',0x00,'f',0x00,'i',0x00,'l',0x00,'e', 0x00,'s',0x00,0x00};
const unsigned char chineseTxtChConfigVoiceLimit[] = {0x80, 0x00,0x00,'V',0x00,'o',0x00,'i',0x00,'c', 0x00,'e',0x00,' ',0x00,'l',0x00,'i',0x00,'m', 0x00,'i',0x00,'t',0x00,0x00};
const unsigned char chineseTxtChConfigLoop[] = {0x80, 0x00,0x00,'L',0x00,'o',0x00,'o',0x00,'p', 0x00,0x00,0x00};
const unsigned char chineseTxtChConfigChannels[] = {0x80, 0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n', 0x00,'n',0x00,'e',0x00,'l',0x00,'s',0x00,0x00};
const unsigned char chineseTxtMidiBankLocation[] = {0x80, 0x00,0x00,'B',0x00,'a',0x00,'n',0x00,'k', 0x00,0x00};
const unsigned char chineseTxtChConfigBankLocation[] = {0x80, 0x00,0x00,'C',0x00,'o',0x00,'n',0x00,'f', 0x00,'i',0x00,'g',0x00,' ',0x00,'B',0x00,'a',0x00,'n',0x00,'k', 0x00,0x00};
const unsigned char chineseTxtMidiEnterVoiceNumber[] = {0x80, 0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e', 0x00,'r',0x00,' ',0x00,'v',0x00,'o',0x00,'i',0x00,'c', 0x00,'e',0x00,0x00};
const unsigned char chineseTxtMidiSelectFile[] = {0x80,0x00,0x00,'S',0x00,'e',0x00,'l',0x00,'e', 0x00,'c',0x00,'t',0x00,' ',0x00,'f',0x00,'i', 0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtMidiOptionNotImplemented[] = {0x80, 0x00,0x00,'N',0x00,'o',0x00,'t',0x00,' ',0x00,'I',0x00,'m',0x00,'p',0x00,'l',0x00,'e',0x00,'m',0x00,'e',0x00,'n',0x00,'t',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtMidiLoopOn[] = {0x80, 0x00,0x00,'O',0x00,'n',0x00,0x00};
const unsigned char chineseTxtMidiLoopOff[] = {0x80, 0x00,0x00,'O',0x00,'f',0x00,'f',0x00,0x00};
const unsigned char chineseTxtMidiChannels[] = {0x80, 0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n', 0x00,'n',0x00,'e',0x00,'l',0x00,'s',0x00,0x00};
const unsigned char chineseTxtMidiMonoChannel[] = {0x80, 0x00,0x00,'M',0x00,'o',0x00,'n',0x00,'o', 0x00,0x00};
const unsigned char chineseTxtMidiStereoChannel[] = {0x80, 0x00,0x00,'S',0x00,'t',0x00,'e',0x00,'r', 0x00,'e',0x00,'o',0x00,0x00};
const unsigned char chineseTxtMidiFastBankLocation[] = {0x80, 0x00,0x00,'F',0x00,'a',0x00,'s',0x00,'t', 0x00,0x00};
const unsigned char chineseTxtMidiSlowBankLocation[] = {0x80, 0x00,0x00,'S',0x00,'l',0x00,'o',0x00,'w', 0x00,0x00};
const unsigned char chineseTxtMidiFileTitle[] = {0x80, 0x00,0x00,'S',0x00,'e',0x00,'l',0x00,'e', 0x00,'c',0x00,'t',0x00,' ',0x00,'f',0x00,'i', 0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtMidiFileNotLoaded[] = {0x80, 0x00,0x00,'N',0x00,'o',0x00,' ',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,' ',0x00,'l',0x00,'o',0x00,'a',0x00,'d',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtMidiFileSelected[] = {0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00,' ',0x00,'S',0x00,'e',0x00,'l',0x00,'e',0x00,'c',0x00,'t', 0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtChConfigAudioOutput[] = {0x80, 0x00,0x00,'A',0x00,'u',0x00,'d',0x00,'i', 0x00,'o',0x00,' ',0x00,'o',0x00,'u',0x00,'t',0x00,'p', 0x00,'u',0x00,'t',0x00,0x00};
const unsigned char chineseTxtMidiSpeaker[] = {0x80,0x00,0x00,'S',0x00,'p',0x00,'e',0x00,'a',0x00,'k',0x00,'e',0x00,'r',0x00,0x00 };
const unsigned char chineseTxtMidiHeadset[] =  {0x80,0x00,0x00,'H',0x00,'e',0x00,'a',0x00,'d',0x00,'s',0x00,'e',0x00,'t',0x00,0x00 };
#endif//#ifdef FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
/*
** Start of Bluetooth Related Strings
*/
const unsigned char chineseTxtBluetooth[] = { 0x00,'B',0x00,'l',0x00,'u',0x00,'e',0x00,'t',0x00,'o',0x00,'o',0x00,'t',0x00,'h',0x00,0x00 };
const unsigned char chineseTxtBluetoothEnable[] = { 0x00,'E',0x00,'n',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBluetoothDisable[] = { 0x00,'D',0x00,'i',0x00,'s',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtEnterDeviceName[] = { 0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,':',0x00,0x00 };
const unsigned char chineseTxtBtEnterPassKey[] = { 0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,' ',0x00,'K',0x00,'e',0x00,'y',0x00,':',0x00,0x00 };
const unsigned char chineseTxtBtPassKey[] = { 0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,' ',0x00,'K',0x00,'e',0x00,'y',0x00,0x00 };
const unsigned char chineseTxtBtTimeout[] = { 0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,'o',0x00,'u',0x00,'t',0x00,0x00 };
const unsigned char chineseTxtBtPairing[] = { 0x00,'P',0x00,'a',0x00,'i',0x00,'r',0x00,'i',0x00,'n',0x00,'g',0x00,0x00 };
const unsigned char chineseTxtBtPaired[] = { 0x00,'P',0x00,'a',0x00,'i',0x00,'r',0x00,'e',0x00,'d',0x00,0x00 };
const unsigned char chineseTxtBtAuthorize[] = { 0x00,'A',0x00,'u',0x00,'t',0x00,'h',0x00,'o',0x00,'r',0x00,'i',0x00,'z',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtUnAuthorize[] = { 0x00,'U',0x00,'n',0x00,'-',0x00,'A',0x00,'u',0x00,'t',0x00,'h',0x00,'o',0x00,'r',0x00,'i',0x00,'z',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtInfo[] = { 0x00,'I',0x00,'n',0x00,'f',0x00,'o',0x00,0x00 };
const unsigned char chineseTxtBtYesOnce[] = { 0x00,'Y',0x00,'e',0x00,'s',0x00,' ',0x00,'(',0x00,'O',0x00,'n',0x00,'c',0x00,'e',0x00,')',0x00,0x00 };
const unsigned char chineseTxtBtYesAlways[] = { 0x00,'Y',0x00,'e',0x00,'s',0x00,' ',0x00,'(',0x00,'A',0x00,'l',0x00,'w',0x00,'a',0x00,'y',0x00,'s',0x00,')',0x00,0x00 };
const unsigned char chineseTxtBtPairDevice[] = { 0x00,'P',0x00,'a',0x00,'i',0x00,'r',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtChangePassKey[] = { 0x00,'C',0x00,'h',0x00,'a',0x00,'n',0x00,'g',0x00,'e',0x00,' ',0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'K',0x00,'e',0x00,'y',0x00,0x00 };
const unsigned char chineseTxtBtShowServices[] = { 0x00,'S',0x00,'h',0x00,'o',0x00,'w',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtAddToKnown[] = { 0x00,'A',0x00,'d',0x00,'d',0x00,' ',0x00,'T',0x00,'o',0x00,' ',0x00,'K',0x00,'n',0x00,'o',0x00,'w',0x00,'n',0x00,0x00 };
const unsigned char chineseTxtBtRemoveDevice[] = { 0x00,'R',0x00,'e',0x00,'m',0x00,'o',0x00,'v',0x00,'e',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtDeviceName[] = { 0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtSet[] = { 0x00,'S',0x00,'e',0x00,'t',0x00,0x00 };
const unsigned char chineseTxtBtServices[] = { 0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtNoServices[] = { 0x00,'N',0x00,'o',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtSupported[] = { 0x00,'S',0x00,'u',0x00,'p',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,'e',0x00,'d',0x00,0x00 };
const unsigned char chineseTxtBtSerialPort[] = { 0x00,'S',0x00,'e',0x00,'r',0x00,'i',0x00,'a',0x00,'l',0x00,' ',0x00,'P',0x00,'o',0x00,'r',0x00,'t',0x00,0x00 };
const unsigned char chineseTxtBtDialUpNetworking[] = { 0x00,'D',0x00,'i',0x00,'a',0x00,'l',0x00,' ',0x00,'U',0x00,'p',0x00,' ',0x00,'N',0x00,'e',0x00,'t',0x00,'w',0x00,'o',0x00,'r',0x00,'k',0x00,'i',0x00,'n',0x00,'g',0x00,0x00 };
const unsigned char chineseTxtBtHeadSet[] = { 0x00,'H',0x00,'e',0x00,'a',0x00,'d',0x00,' ',0x00,'S',0x00,'e',0x00,'t',0x00,0x00 };
const unsigned char chineseTxtBtHandsFree[] = { 0x00,'H',0x00,'a',0x00,'n',0x00,'d',0x00,'s',0x00,' ',0x00,'F',0x00,'r',0x00,'e',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtObjectPush[] = { 0x00,'O',0x00,'b',0x00,'j',0x00,'e',0x00,'c',0x00,'t',0x00,' ',0x00,'P',0x00,'u',0x00,'s',0x00,'h',0x00,0x00 };
const unsigned char chineseTxtBtFileTransfer[] = { 0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,' ',0x00,'T',0x00,'r',0x00,'a',0x00,'n',0x00,'s',0x00,'f',0x00,'e',0x00,'r',0x00,0x00 };
const unsigned char chineseTxtBtLanAccess[] = { 0x00,'L',0x00,'a',0x00,'n',0x00,' ',0x00,'A',0x00,'c',0x00,'c',0x00,'e',0x00,'s',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtCordlessTelephony[] = { 0x00,'C',0x00,'o',0x00,'r',0x00,'d',0x00,'l',0x00,'e',0x00,'s',0x00,'s',0x00,' ',0x00,'T',0x00,'e',0x00,'l',0x00,'e',0x00,'p',0x00,'h',0x00,'o',0x00,'n',0x00,'y',0x00,0x00 };
const unsigned char chineseTxtBtIntercom[] = { 0x00,'I',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,'c',0x00,'o',0x00,'m',0x00,0x00 };
const unsigned char chineseTxtBtUnknownService[] = { 0x00,'U',0x00,'n',0x00,'k',0x00,'n',0x00,'o',0x00,'w',0x00,'n',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtRequiresAuthorizationForService[] = { 0x00,'%',0x00,'s',0x00,' ',0x00,'r',0x00,'e',0x00,'q',0x00,'u',0x00,'i',0x00,'r',0x00,'e',0x00,'s',0x00,' ',0x00,'A',0x00,'u',0x00,'t',0x00,'h',0x00,'o',0x00,'r',0x00,'i',0x00,'z',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,' ',0x00,'f',0x00,'o',0x00,'r',0x00,' ',0x00,'S',0x00,'e',0x00,'r',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,' ',0x00,':',0x00,' ',0x00,'%',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtDevices[] = { 0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtDevice[] = { 0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtHidden[] = { 0x00,'H',0x00,'i',0x00,'d',0x00,'d',0x00,'e',0x00,'n',0x00,0x00 };
const unsigned char chineseTxtBtTimed[] = { 0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,'d',0x00,0x00 };
const unsigned char chineseTxtBtKnownDevices[] = { 0x00,'K',0x00,'n',0x00,'o',0x00,'w',0x00,'n',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtDiscoverable[] = { 0x00,'D',0x00,'i',0x00,'s',0x00,'c',0x00,'o',0x00,'v',0x00,'e',0x00,'r',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtNoDevices[] = { 0x00,'N',0x00,'o',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtBtFound[] = { 0x00,'F',0x00,'o',0x00,'u',0x00,'n',0x00,'d',0x00,0x00 };
const unsigned char chineseTxtBtSetLocalName[] = { 0x00,'S',0x00,'e',0x00,'t',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'a',0x00,'l',0x00,' ',0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,0x00 };
const unsigned char chineseTxtBtClearStoredDevices[] = { 0x00,'C',0x00,'l',0x00,'e',0x00,'a',0x00,'r',0x00,' ',0x00,'a',0x00,'l',0x00,'l',0x00,' ',0x00,'S',0x00,'t',0x00,'o',0x00,'r',0x00,'e',0x00,'d',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,'s',0x00,0x00 }; /* To be translated */
/*
** End of Bluetooth Related Strings
*/
#endif
const unsigned char chineseTxtAllDivert[] = { 0x80, 0x00,0x00,'A',0x00,'l',0x00,'l',0x00,' ',0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtBusyDivert[] = { 0x80, 0x00,0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,' ',0x00,'i',0x00,'f',0x00,' ',0x00,'b',0x00,'u',0x00,'s',0x00,'y',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtNoAnsDivert[] = { 0x80, 0x00,0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,' ',0x00,'w',0x00,'h',0x00,'e',0x00,'n',0x00,'n',0x00,'o',0x00,' ',0x00,'a',0x00,'n',0x00,'s',0x00,'w',0x00,'e',0x00,'r',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtAllCallsDivert[] = { 0x80, 0x00,0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,' ',0x00,'f',0x00,'o',0x00,'r',0x00,' ',0x00,'a',0x00,'l',0x00,'l',0x00,' ',0x00,'c',0x00,'a',0x00,'l',0x00,'l',0x00,'s',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtIfNotReachable[] = { 0x80, 0x00,0x00,'c',0x00,'o',0x00,'n',0x00,'d',0x00,'.',0x00,' ',0x00,'f',0x00,'o',0x00,'r',0x00,'w',0x00,'a',0x00,'r',0x00,'d',0x00,0x00 }; /* SPR15568 */  //localization required.
const unsigned char chineseTxtIfNoService[] ={ 0x80, 0x00,0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'r',0x00,'e',0x00,'a',0x00,'c',0x00,'h',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00 }; /* SPR15568 */  //localization required.
const unsigned char chineseTxtSOS[] = { 0x80, 0x00,0x00,'S',0x00,'O',0x00,'S',0x00,0x00 }; /* SPR12883 */  //localization required.
const unsigned char chineseTxtNoReachDivert[] = { 0x80, 0x00,0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,' ',0x00,'w',0x00,'h',0x00,'e',0x00,'n',0x00,' ',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'r',0x00,'e',0x00,'a',0x00,'c',0x00,'h',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtCondDivert[] = { 0x80, 0x00,0x00,'D',0x00,'i',0x00,'v',0x00,'e',0x00,'r',0x00,'t',0x00,' ',0x00,'f',0x00,'o',0x00,'r',0x00,' ',0x00,'c',0x00,'o',0x00,'n',0x00,'d',0x00,'.',0x00,' ',0x00,'f',0x00,'o',0x00,'r',0x00,'w',0x00,'a',0x00,'r',0x00,'d',0x00,0x00 }; /* SPR13614 */  //localization required.
const unsigned char chineseTxtInvalidInput[] = { 0x80, 0x00,0x00,'I',0x00,'n',0x00,'v',0x00,'a',0x00,'l',0x00,'i',0x00,'d',0x00,' ',0x00,'I',0x00,'n',0x00,'p',0x00,'u',0x00,'t',0x00,0x00 }; /* SPR21547 */  //localization required.
const unsigned char chineseTxtPasswords[] = { 0x80, 0x00,0x00,'P',0x00,'a',0x00,'s',0x00,'s',0x00,'w',0x00,'o',0x00,'r',0x00,'d',0x00,'s',0x00,0x00};/* SPR16107 */  //localization required.	
const unsigned char chineseTxtMismatch[] = {0x80, 0x00,0x00,'d',0x00,'o',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'m',0x00,'a',0x00,'t',0x00,'c',0x00,'h',0x00,'!',0x00,0x00 }; /* SPR16107 */  //localization required.
//x0pleela 30 Jan, 2007  DR: OMAPS00108892
//Adding the following strings to display error message when loop on is enabled while playing all files
const unsigned char chineseTxtLoopOn[] =  {0x80,0x00,0x00,'L',0x00,'o',0x00,'o',0x00,'p',0x00,' ',0x00,'O',0x00,'N',0x00,0x00 };
const unsigned char chineseTxtCantPlayAllFiles[] =  {0x80,0x00,0x00,'C',0x00,'a',0x00,'n',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'P',0x00,'l',0x00,'a',0x00,'y',0x00,' ',0x00,'A',0x00,'l',0x00,'l',0x00,' ',0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,0x00 };
const unsigned char chineseTxtCantBeActivated[] =  {0x80,0x00,0x00,'C',0x00,'a',0x00,'n',0x00,'n',0x00,'o',0x00,'t',0x00,' ',0x00,'b',0x00,'e',0x00,' ',0x00,'A',0x00,'c',0x00,'t',0x00,'i',0x00,'v',0x00,'a',0x00,'t',0x00,'e',0x00,'d',0x00,0x00 };


//Nov 29, 2004    REF: CRR 25051 xkundadu
//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//Fix: Added these Chinees strings to display the various speaker volume 
//      levels.
const unsigned char chineesTxtLevel1[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'1',0x00,0x00};
const unsigned char chineesTxtLevel2[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'2',0x00,0x00};
const unsigned char chineesTxtLevel3[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'3',0x00,0x00};
const unsigned char chineesTxtLevel4[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'4',0x00,0x00};
const unsigned char chineesTxtLevel5[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtImage[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtSMSWait[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
// 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
/* a0393213 (WR - integer conversion resulted in truncation) */
const unsigned char chineesTxtLoudspeaker[] = {0x80, 0x00,0x00,'L',0x00,'o',0x00,'u',0x00,'d', 0x00,'s',0x00,'p',0x00,'e',0x00,'a', 0x00,'k',0x00,'e',0x00,'r',0x00,0x00};
/* a0393213 (WR - integer conversion resulted in truncation) */
const unsigned char chineesTxtHandheld[] = {0x80, 0x00,0x00,'H',0x00,'a',0x00,'n',0x00,'d', 0x00,'h',0x00,'e',0x00,'l',0x00,'d',0x00,0x00};
const unsigned char chineesTxtCarkit[] = {0x80, 0x00,0x00,'C',0x00,'a',0x00,'r',0x00,'k', 0x00,'i',0x00,'t',0x00,0x00};
const unsigned char chineesTxtHeadset[] = {0x80, 0x00,0x00,'H',0x00,'e',0x00,'a',0x00,'d', 0x00,'s',0x00,'e',0x00,'t',0x00,0x00};
/* a0393213 (WR - integer conversion resulted in truncation) */
const unsigned char chineesTxtInserted[] = {0x80, 0x00,0x00,'i',0x00,'n',0x00,'s',0x00,'e', 0x00,'r',0x00,'t',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineesTxtRemoved[] = {0x80, 0x00,0x00,'r',0x00,'e',0x00,'m',0x00,'o', 0x00,'v',0x00,'e',0x00,'d',0x00,0x00};
#endif
#ifdef FF_MMI_MULTIMEDIA
/* Multimedia related Strings. */
const unsigned char chineesTxtMultimediaApp[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtConfigurationParameters[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioVideoPlayback[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtMidiPlayback[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtImageViewer[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtCameraCapture[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioVideoRecording[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFileListType[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtNoFilesAvail[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Configuration Parameters related string.  */
const unsigned char chineesTxtOutputScreenSize[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioEncodingFormat[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoEncodingFormat[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoCaptureSize[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoFrameRateSelection[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioBitRate[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoBitRate[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtMidiPlayerSelect[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Screen Mode */
const unsigned char chineesTxtHalfScreenMode [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFullScreenMode[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Audio Format	 */
const unsigned char  chineesTxtAAC [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAMR [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtPCM [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtMP3 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Video Format */
const unsigned char chineesTxtMpeg4 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtH263 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Frame Size */
const unsigned char chineesTxtQcif [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtCif  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};

/* Frame Rate */
const unsigned char chineesTxt5fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt10fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt15fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt20fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt25fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt30fps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Bit Rate */
const unsigned char chineesTxtAACBitRate  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesAMRNBBitRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};

/* AAC Bit Rate */
const unsigned char chineesTxt48Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt66p15Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt72Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt96pKbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt132p3Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt144Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};    			
const unsigned char chineesTxt192Kbps[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt264p6Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};    			
const unsigned char chineesTxt288Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* AMR Bit Rate */
const unsigned char chineesTxt1p80Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};    			
const unsigned char chineesTxt4p75Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};    			
const unsigned char chineesTxt5p15Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};			
const unsigned char chineesTxt5p9Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00}; 			
const unsigned char chineesTxt6p7Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt7p4Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};    			
const unsigned char chineesTxt7p95Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt10p2Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt12p2Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Video Bit Rate */
const unsigned char chineesTxt64Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt128Kbps [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt384Kbps[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Softkey for Multimedia Application. */
const unsigned char chineesTxtPauseKey [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtResumeKey [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtEncodeKey[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};	
const unsigned char chineesTxtSoftIVT [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};	
const unsigned char chineesTxtPictureDisp[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};	
const unsigned char chineesTxtSetParam[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Audio Path Configuration Parameter. */
const unsigned char chineesTxtAudioPath [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtHeadset[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtHeadphone [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtSoftStart [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudvidRecord[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Added the string for Audio / video Recording */
const unsigned char chineesTxtAudioRecording[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoRecording[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* End Add */
const unsigned char chineesTxtQQVga[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtEncdTo[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtJPEGFile [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* File Management related strings. */
const unsigned char chineesTxtFileMgmt [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFileOprns [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFileOprnsTest  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFFS[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtMMC [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtSDC[] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtTestCase1 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtTestCase2 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtTestCase3 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtOperSuccess [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtOperFailed  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtNotYetDone [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
	/*For FTP Application */
const unsigned char chineesTxtFTPApp [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtPut [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtGet [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Configuration Parameters - Volume Configuration and Preferred Storage. */
const unsigned char chineesTxtVolumeConfig [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtPreferredStorage [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioDecodeConfig [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioEncodeConfig [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoEncodeConfig [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtSpeaker [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtEnterPath [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtInvalidPath [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtDummyStr [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtRename [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtCreateFolder [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtEnterFileName [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtFolderNotEmpty [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/*
const unsigned char chineesTxtOperationList  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
*/
const unsigned char chineesTxtDestination [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};

/* x0045876, 02-08-2006, (UCS2 Support - When CHINESE_MMI flag enabled, got error chineesTxtDummyStr already initialized)*/
/*
const unsigned char chineesTxtDummyStr [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
*/
const unsigned char chineesTxtDummyStr1 [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt8kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt11kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt12kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};      /*OMAPS00081264 - 12k Menu addition */
const unsigned char chineesTxt16kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt22kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt24kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};     /*OMAPS00081264 - 12k Menu addition */
const unsigned char chineesTxt32kRate  [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt44kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt48kRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtPcmRate [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};

/* x0045876, 02-Aug-2006 (UCS2 Support - chineesTxtPcmChannelMode undefined) */
const unsigned char chineesTxtPcmChannelMode [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtImgCaptureQuality [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt0Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt10Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt20Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt30Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt40Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt50Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt60Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt70Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt80Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt90Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt100Set [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtStreoMode [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtMonoMode [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoPlayOption [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* x0045876, 02-Aug-2006 (UCS2 Support - When CHINESE_MMI flag enabled, got error chineesTxtAudioVideoPlayback 
						already initialized, so changed to chineesTxtAudioVideoPlay) */
const unsigned char chineesTxtAudioVideoPlay [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoPlay [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
       /* Video Recording Duration Specification */
const unsigned char chineesTxtVideoRecDuration [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxt10sec [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char  chineesTxt15sec [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char  chineesTxt20sec [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char  chineesTxtNonStop [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
/* Video Only Option - x0043641 */
const unsigned char chineesTxtVideoRecordOption [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtAudioVideoRecord [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineesTxtVideoRecord [] = {0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const chineseTxtGarbageCollect[]={0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
#endif
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef F_MMI_FILE_VIEWER
const unsigned char chineseTxtFileNotLoaded[]={0x80, 0x00,0x00,'N',0x00,'o',0x00,' ',0x00,'f',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,' ',0x00,'l',0x00,'o',0x00,'a',0x00,'d',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtFileViewer[]={0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'v', 0x00,'i',0x00,'e',0x00,'w',0x00,'e',0x00,'r',0x00,0x00};
const unsigned char chineseTxtRename[]={0x80, 0x00,0x00,'R',0x00,'e',0x00,'n',0x00,'a', 0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtDeleteConfirm[]={0x80, 0x00,0x00,'D',0x00,'e',0x00,'l',0x00,'e', 0x00,'t',0x00,'e',0x00,'?',0x00,0x00};
const unsigned char chineseTxtFileTooBig[]={0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00,' ',0x00,'b',0x00,'i',0x00,'g',0x00,0x00};
//Aug 31, 2005    REF: SPR 34050 xdeepadh	
const unsigned char chineseTxtWrongFile[]={0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineseTxtNoExtension[]={0x80, 0x00,0x00,'L',0x00,'e',0x00,'v',0x00,'e', 0x00,'l',0x00,' ',0x00,'5',0x00,0x00};
const unsigned char chineseTxtFileExists[]={0x80, 0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00,'E',0x00,'x',0x00,'i',0x00,'s',0x00,'t',0x00,'s',0x00,0x00}; //Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh
#endif //FF_MMI_FILE_VIEWER

#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
const unsigned char chineseTxtRotate[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e',0x00,0x00};
const unsigned char chineseTxtRotate90Degree[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e', 0x00,' ',0x00,'9', 0x00,'0',0x00,0x00};
const unsigned char chineseTxtRotate180Degree[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e', 0x00,' ',0x00,'1', 0x00,'8', 0x00,'0',0x00,0x00};
const unsigned char chineseTxtRotate270Degree[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e', 0x00,' ',0x00,'2', 0x00,'7', 0x00,'0',0x00,0x00};
const unsigned char chineseTxtZoom[]={0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,0x00};
const unsigned char chineseTxtZoomIn[]={0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,' ',0x00,'I',0x00,'n',0x00,0x00};
const unsigned char chineseTxtZoomOut[]={0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,' ',0x00,'O',0x00,'u',0x00,'t',0x00,0x00};
const unsigned char chineseTxtZoomInMax[]={0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,' ',0x00,'I',0x00,'n',0x00,'m',0x00,'a',0x00,'x',0x00,0x00};
const unsigned char chineseTxtZoomOutMax[]={0x80, 0x00,0x00,'Z',0x00,'o',0x00,'o',0x00,'m', 0x00,' ',0x00,'O',0x00,'u',0x00,'t',0x00,'m',0x00,'a',0x00,'x',0x00,0x00};

#endif
/* Power management Menu*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
const unsigned char chineesTxt10Second[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e',0x00,0x00};
const unsigned char chineesTxt15Second[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e',0x00,0x00};
const unsigned char chineesTxt20Sec[]={0x80, 0x00,0x00,'R',0x00,'o',0x00,'t',0x00,'a', 0x00,'t',0x00,'e',0x00,0x00};
#endif
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
const unsigned char chineseTxtUSBEnumeration[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'E',0x00,'n',0x00,'u',0x00,'m',0x00,'e',0x00,'r',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00};
const unsigned char chineseTxtUSBMode[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'m',0x00,'o',0x00,'d',0x00,'e',0x00,0x00};
const unsigned char chineseTxtContinue[]={0x80, 0x00,0x00,'C',0x00,'o',0x00,'n',0x00,'t',0x00,'i',0x00,'u',0x00,'e',0x00,'?',0x00,0x00};
const unsigned char chineseTxtUSBDisconnect[]={0x80, 0x00,0x00,'D',0x00,'i',0x00,'s',0x00,'c',0x00,'o',0x00,'n',0x00,'n',0x00,'e',0x00,'c',0x00,'t',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtUSBMS[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'M',0x00,'S',0x00,0x00};
const unsigned char chineseTxtUSBTrace[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'T',0x00,'r',0x00,'a',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtUSBFax[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'F',0x00,'a',0x00,'x',0x00,0x00};
const unsigned char chineseTxtPSShutdown[]={0x80, 0x00,0x00,'P',0x00,'S',0x00,'S',0x00,'h',0x00,'u',0x00,'t',0x00,'d',0x00,'o',0x00,'w',0x00,'n',0x00,0x00};
const unsigned char chineseTxtNoPSShutdown[]={0x80, 0x00,0x00,'N',0x00,'o',0x00,' ',0x00,'P',0x00,'S',0x00,'S',0x00,'h',0x00,'u',0x00,'t',0x00,'d',0x00,'o',0x00,'w',0x00,'n',0x00,0x00};
//xashmic 27 Sep 2006, OMAPS00096389 
const unsigned char chineseTxtUSBFaxnTrace[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'F',0x00,'a',0x00,'x',0x00,' ',0x00,'a',0x00,'n',0x00,'d',0x00,' ',0x00,'T',0x00,'r',0x00,'a',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtUSBMSnTrace[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'M',0x00,'S',0x00,' ',0x00,'a',0x00,'n',0x00,'d',0x00,' ',0x00,'T',0x00,'r',0x00,'a',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtUSBFaxnMS[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'a',0x00,'n',0x00,'d',0x00,' ',0x00,'F',0x00,'a',0x00,'x',0x00,0x00};
const unsigned char chineseTxtUSBMSPorts[]={0x80, 0x00,0x00,'U',0x00,'S',0x00,'B',0x00,' ',0x00,'P',0x00,'o',0x00,'r',0x00,'t',0x00,'s',0x00,0x00};
const unsigned char chineseTxtPS[]={0x80, 0x00,0x00,'P',0x00,'S',0x00,0x00};
const unsigned char chineseTxtPopupMenu[]={0x80, 0x00,0x00,'P',0x00,'o',0x00,'p',0x00,'U',0x00,'p',0x00,' ',0x00,'M',0x00,'e',0x00,'n',0x00,'u',0x00,0x00};
#endif
// x0021334 10 May, 2006 DR: OMAPS00075379
const unsigned char chineseTxtPending[]={0x80, 0x00,0x00,'P',0x00,'e',0x00,'n',0x00,'d',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};

// x0021334 02 June, 2006 DR: OMAPS00078005 
#ifdef FF_TIMEZONE
const unsigned char chineseTxtTimeUpdate[]={0x80,
0x00,0x00,'N', 0x00,'e',0x00,'w',0x00,' ',0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,'/',0x00,'D',0x00,'a',0x00,'t',0x00,'e',0x00,' ',0x00,'r',0x00,'e',0x00,'c',0x00,'e',0x00,'i',0x00,'v',0x00,'e',0x00,'d',0x00,'.',0x00,0x00};
const unsigned char chineseTxtTimeUpdateConfirm[]={0x80, 0x00,0x00,'W',0x00,'a',0x00,'n',0x00,'t',0x00,' ',0x00,'t',0x00,'o',0x00,' ',0x00,'u',0x00,'p',0x00,'d',0x00,'a',0x00,'t',0x00,'e',0x00,'?',0x00,0x00};
const unsigned char chineseTxtNitzTimeUpdate[]={0x80,
0x00,0x00,'N', 0x00,'e',0x00,'t',0x00,'w',0x00,'o',0x00,'r',0x00,'k',0x00,' ',0x00,'T',0x00,'i',0x00,'m',0x00,'e',0x00,' ',0x00,'U',0x00,'p',0x00,'d',0x00,'a',0x00,'t',0x00,'e',0x00,0x00};
#endif
/*OMAPS00098881 (Removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
/* x0039928 - Lint warning removal */
const unsigned char chineseTxtScreenUpdate[]={0x80, 0x00,0x00,'S',0x00,'c',0x00,'r',0x00,'e',0x00,'e',0x00,'n',0x00,' ',0x00,'U',0x00,'p',0x00,'d',0x00,'a',0x00,'t',0x00,'e',0x00,0x00};
#endif
#ifdef FF_MMI_CAMERA_APP
const unsigned char chineseTxtCamNotEnough[] = {0x80, 0x00,0x00,'N',0x00,'o',0x00,'t',0x00,' ', 0x00,'E',0x00,'n',0x00,'o',0x00,'u', 0x00,'g',0x00,'h',0x00,0x00};
const unsigned char chineseTxtCamMemory[] = {0x80, 0x00,0x00,'M',0x00,'e',0x00,'m',0x00,'o', 0x00,'r',0x00,'y',0x00,0x00};
#endif
	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
const unsigned char chineseTxtFwdIncomingTTYCall[]={0x80, 0x00,0x00,'F',0x00,'w',0x00,'d',0x00,'.',0x00,' ',0x00,'I',0x00,'n',0x00,'c',0x00,'.',0x00,' ',0x00,'T',0x00,'T',0x00,'Y',0x00,' ',0x00,'c',0x00,'a',0x00,'l',0x00,'l',0x00,0x00};
const unsigned char chineseTxtFwdIncomingCall[]={0x80, 0x00,0x00,'F',0x00,'w',0x00,'d',0x00,'.',0x00,' ',0x00,'I',0x00,'n',0x00,'c',0x00,'.',0x00,' ',0x00,' ',0x00,'c',0x00,'a',0x00,'l',0x00,'l',0x00,0x00};
	
//06 July, 2007 DR: OMAPS00137334 x0062172 Syed
/* String for Conference Full notification */
const unsigned char chineseTxtConferenceFull[]={0x80, 0x00,0x00,'C', 0x00,'o',0x00,'n',0x00,'f ',0x00,'e',0x00,'r',0x00,'e',0x00,'n',0x00,'c',0x00,'e',0x00,' ',0x00,'F',0x00,'u',0x00,'l',0x00,'l',0x00,0x00};
	

//x0pleela 05 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
const unsigned char chineseTxtPhoneLock[]={0x80, 0x00,0x00,'P',0x00,'h',0x00,'o',0x00,'n',0x00,'e',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,0x00};
const unsigned char chineseTxtPhLockEnable[]={0x80, 0x00,0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'E',0x00,'n',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtPhLockAutomatic[]={0x80, 0x00,0x00,'A',0x00,'u',0x00,'t',0x00,'o',0x00,'m',0x00,'a',0x00,'t',0x00,'i',0x00,'c',0x00,0x00};
const unsigned char chineseTxtPhLockAutoOn[]={0x80, 0x00,0x00,'O',0x00,'n',0x00,0x00};
const unsigned char chineseTxtPhLockAutoOff[]={0x80, 0x00,0x00,'O',0x00,'f',0x00,'f',0x00,0x00};
const unsigned char chineseTxtPhLockChangeCode[]={0x80, 0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n',0x00,'g',0x00,'e',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00};
const unsigned char chineseTxtEnterPhUnlockCode[]={0x80, 0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'P',0x00,'h',0x00,'.',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00};
const unsigned char chineseTxtEnterOldUnlockCode[]={0x80, 0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'O',0x00,'l',0x00,'d',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00};
const unsigned char chineseTxtEnterNewUnlockCode[]={0x80, 0x00,0x00,'E',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,' ',0x00,'N',0x00,'e',0x00,'w',0x00,' ',0x00,'U',0x00,'n',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'C',0x00,'o',0x00,'d',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAlreadyEnabled[]={0x80, 0x00,0x00,'A',0x00,'l',0x00,'r',0x00,'e',0x00,'a',0x00,'d',0x00,'y',0x00,' ',0x00,'E',0x00,'n',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtAlreadyDisabled[]={0x80, 0x00,0x00,'A',0x00,'l',0x00,'r',0x00,'e',0x00,'a',0x00,'d',0x00,'y',0x00,' ',0x00,'D',0x00,'i',0x00,'s',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtAutoLockDisable[]={0x80, 0x00,0x00,'A',0x00,'u',0x00,'t',0x00,'o',0x00,' ',0x00,'P',0x00,'h',0x00,' ',0x00,'L',0x00,'o',0x00,'c',0x00,'k',0x00,' ',0x00,'D',0x00,'i',0x00,'s',0x00,'a',0x00,'b',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAutoPhone[]={0x80, 0x00,0x00,'A',0x00,'u',0x00,'t',0x00,'o',0x00,'m',0x00,'a',0x00,'t',0x00,'i',0x00,'c',0x00,' ',0x00,'P',0x00,'h',0x00,'o',0x00,'n',0x00,'e',0x00,0x00};
const unsigned char chineseTxtSuccess[]={0x80, 0x00,0x00,'S',0x00,'u',0x00,'c',0x00,'c',0x00,'e',0x00,'s',0x00,'s',0x00,0x00}; 
const unsigned char chineseTxtNotLocked[]={0x80, 0x00,0x00,'P',0x00,'h',0x00,'o',0x00,'n',0x00,'e',0x00,' ',0x00,'c',0x00,'a',0x00,'n',0x00,'t',0x00,' ',0x00,'b',0x00,'e',0x00,' ',0x00,'l',0x00,'o',0x00,'c',0x00,'k',0x00,'e',0x00,'d',0x00,0x00}; 
#endif /* FF_PHONE_LOCK */


//Sep 14, 2005    REF: SPR 34020 xdeepadh
//The chinese strings have been rearranged.
//DON'T TOUCH THE FOLLOWING LINE...
//Language = "Chinese Lookup"

#ifdef FF_MMI_FILEMANAGER
const unsigned char chineseTxtFileMgmt [] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'M',0x00,'a',0x00,'n',0x00,'a',0x00,'g',0x00,'e',0x00,'m',0x00,'e',0x00,'n',0x00,'t',0x00,0x00};
const unsigned char chineseTxtFileList[] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'l',0x00,'i',0x00,'s',0x00,'t',0x00,'s',0x00,0x00};
const unsigned char chineseTxtFFS[] = {0x80,0x00,0x00,'N',0x00,'O',0x00,'R',0x00,0x00};
const unsigned char chineseTxtNORMS[] = {0x80,0x00,0x00,'N',0x00,'O',0x00,'R',0x00,'-',0x00,'M',0x00,'S',0x00,0x00};
const unsigned char chineseTxtNAND[] = {0x80,0x00,0x00,'M',0x00,'M',0x00,'C',0x00,'C',0x00,'a',0x00,'r',0x00,'d',0x00,0x00};
const unsigned char chineseTxtTflash[] = {0x80,0x00,0x00,'S',0x00,'D',0x00,'C',0x00,'a',0x00,'r',0x00,'d',0x00,0x00};
const unsigned char chineseTxtOpen[] = {0x80,0x00,0x00,'O',0x00,'p',0x00,'e',0x00,'n',0x00,0x00};
const unsigned char chineseTxtFormat[] = {0x80,0x00,0x00,'F',0x00,'o',0x00,'r',0x00,'m',0x00,'a',0x00,'t',0x00,0x00};
const unsigned char chineseTxtProperties[] = {0x80,0x00,0x00,'P',0x00,'r',0x00,'o',0x00,'p',0x00,'e',0x00,'r',0x00,'t',0x00,'i',0x00,'e',0x00,'s',0x00,0x00};
const unsigned char chineseTxtImageFiles[]= {0x80,0x00,0x00,'i',0x00,'m',0x00,'a',0x00,'g',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAudioFiles[] = {0x80,0x00,0x00,'A',0x00,'u',0x00,'d',0x00,'i',0x00,'o',0x00,0x00};
const unsigned char chineseTxtFile [] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,0x00};
const unsigned char chineseTxtDirectory[] = {0x80,0x00,0x00,'D',0x00,'i',0x00,'r',0x00,'e',0x00,'c',0x00,'t',0x00,'o',0x00,'r',0x00,'y',0x00,0x00};
const unsigned char chineseTxtFreeSpace[] = {0x80,0x00,0x00,'F',0x00,'r',0x00,'e',0x00,'e',0x00,'S',0x00,'p',0x00,'a',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtUsedSpace[] = {0x80,0x00,0x00,'U',0x00,'s',0x00,'e',0x00,'d',0x00,'S',0x00,'p',0x00,'a',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtThumbnail[] = {0x80,0x00,0x00,'T',0x00,'h',0x00,'u',0x00,'m',0x00,'b',0x00,'n',0x00,'a',0x00,'i',0x00,'l',0x00,0x00};
const unsigned char chineseTxtExists[] = {0x80,0x00,0x00,'e', 0x00,'x',0x00,'i',0x00,'s',0x00,'t',0x00,'s',0x00,0x00};
const unsigned char chineseTxtCreateDir[] = {0x80,0x00,0x00,'C',0x00,'r',0x00,'e',0x00,'a',0x00,'t',0x00,'e',0x00,'D',0x00,'i',0x00,'r',0x00,'e',0x00,'c',0x00,'t',0x00,'o',0x00,'r',0x00,'y',0x00,0x00};
const unsigned char chineseTxtCreated [] = {0x80,0x00,0x00,'C',0x00,'r',0x00,'e',0x00,'a',0x00,'t',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtNotEmpty [] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'t',0x00,'e',0x00,'m',0x00,'p',0x00,'t',0x00,'y',0x00,0x00};
const unsigned char chineseTxtMemorystat [] = {0x80,0x00,0x00,'M',0x00,'e',0x00,'m',0x00,'o',0x00,'r',0x00,'y',0x00,'S',0x00,'t',0x00,'a',0x00,'t',0x00,'u',0x00,'s',0x00,0x00};
const unsigned char chineseTxtSourceDest [] = {0x80,0x00,0x00,'S',0x00,'o',0x00,'u',0x00,'r',0x00,'c',0x00,'e',0x00,'a',0x00,'n',0x00,'d',0x00,'D',0x00,'e',0x00,'s',0x00,'t',0x00,'i',0x00,'n',0x00,'a',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00};
const unsigned char chineseTxtSame[] = {0x80,0x00,0x00,'S',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAudNotAllowed [] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'t',0x00,'A',0x00,'l',0x00,'l',0x00,'o', 0x00, 'w' , 0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtAudAudioError[] = {0x80,0x00,0x00,'A',0x00,'u',0x00,'d',0x00,'i',0x00,'o',0x00,0x00};
const unsigned char chineseTxtAudDenied[] = {0x80,0x00,0x00,'D',0x00,'e',0x00,'n',0x00,'i',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtAudPlayerError [] = {0x80,0x00,0x00,'P',0x00,'l',0x00,'a',0x00,'y',0x00,'e',0x00,'r',0x00,0x00};
const unsigned char chineseTxtAudFFSError [] = {0x80,0x00,0x00,'F',0x00,'F',0x00,'S',0x00,0x00};
const unsigned char chineseTxtAudMessagingError[] = {0x80,0x00,0x00,'M',0x00,'e',0x00,'s',0x00,'s',0x00,'a',0x00,'g',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtAudNotReady [] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'t',0x00,'R',0x00,'e',0x00,'a',0x00,'d',0x00,'y',0x00,0x00};
const unsigned char chineseTxtAudInvalidParam [] = {0x80,0x00,0x00,'i',0x00,'n',0x00,'v', 0x00,'a',0x00,'l',0x00,'i',0x00,'d',0x00,'P',0x00,'a',0x00,'r',0x00,'a',0x00,'m',0x00,0x00};
const unsigned char chineseTxtAudInternalError [] = {0x80,0x00,0x00,'i',0x00,'n',0x00,'t',0x00,'e',0x00,'r',0x00,'n',0x00,'a',0x00,'l',0x00,0x00};
const unsigned char chineseTxtAudMemoryError [] = {0x80,0x00,0x00,'M',0x00,'e',0x00,'m',0x00,'o',0x00,'r',0x00,'y',0x00,0x00};
const unsigned char chineseTxtAudNotSupported [] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'t',0x00,'S',0x00,'u',0x00,'p',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,'e',0x00,'d', 0x00,0x00};
const unsigned char chineseTxtCopying [] = {0x80,0x00,0x00,'C',0x00,'o',0x00,'p',0x00,'y',0x00,'i',0x00,'n',0x00,'g',0x00,0x00};
const unsigned char chineseTxtFileNotLoaded [] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'s',0x00,'l',0x00,'o',0x00,'a',0x00,'d',0x00,'e',0x00,'d',0x00,0x00};
const unsigned char chineseTxtFileViewer [] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e', 0x00, 'V' , 0x00,'i',0x00,'e', 0x00, 'w' , 0x00,'e',0x00,'r',0x00,0x00};
const unsigned char chineseTxtRename[]	= {0x80,0x00,0x00,'R',0x00,'e',0x00,'n',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtDeleteConfirm[] = {0x80,0x00,0x00,'D',0x00,'e',0x00,'l',0x00,'e',0x00,'t',0x00,'e',0x00,0x00};
const unsigned char chineseTxtFileTooBig [] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'T',0x00,'o',0x00,'o',0x00,'B',0x00,'i',0x00,'g',0x00,0x00};
const unsigned char chineseTxtFileExists[] = {0x80,0x00,0x00,'F',0x00,'i',0x00,'l',0x00,'e',0x00,'e',  0x00,'x',0x00,'i',0x00,'s',0x00,'t',0x00,'s',0x00,0x00};
const unsigned char chineseTxtWrongFilename [] = {0x80,0x00,0x00,'W',0x00,'r',0x00,'o',0x00,'n',0x00,'g',0x00,'N',0x00,'a',0x00,'m',0x00,'e',0x00,0x00};
const unsigned char chineseTxtNoExtension[] = {0x80,0x00,0x00,'W',0x00,'r',0x00,'o',0x00,'n',0x00,'g',0x00,'e',  0x00,'x',0x00,'t',0x00,'n',0x00,0x00};
const unsigned char chineseTxtStorageDevice[] = {0x80,0x00,0x00,'S',0x00,'t',0x00,'o',0x00,'r',0x00,'a',0x00,'g',  0x00,'e',0x00,' ',0x00,'D',0x00,'e',0x00,'v',0x00,'i',0x00,'c',0x00,'e',0x00,0x00};
const unsigned char chineseTxtPBar[] = {0x80,0x00,0x00,'P',0x00,'r',0x00,'o',0x00,'g',0x00,'r',0x00,'e',  0x00,'s',0x00,'s',0x00,' ',0x00,'B',0x00,'a',0x00,'r',0x00,' ',0x00,'O',0x00,'n',0x00,'/',0x00,'O',0x00,'f',0x00,'f',0x00,0x00};
#endif

//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)    
const unsigned char chineseTxtTo[]={0x80, 0x00,0x00,'T',0x00,'o',0x00,0x00};

/*April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
#ifdef FF_MMI_CAMERA_APP
const unsigned char chineseTxtAutoSave[] = {0x80,0x00,0x00,'A',0x00,'u',0x00,'t',0x00,'o',0x00,' ' ,0x00,'S',  0x00,'a',0x00,'v',0x00,'e',0x00,0x00};
const unsigned char chineseTxtAutoSaveOn[] = {0x80,0x00,0x00,'O',0x00,'n',0x00,0x00};
const unsigned char chineseTxtAutoSaveOff[] = {0x80,0x00,0x00,'O',0x00,'f',0x00,'f',0x00,0x00};
const unsigned char chineseTxtSoftSave[] = {0x80,0x00,0x00,'S',0x00,'a',0x00,'v',0x00,'e',0x00,0x00};
const unsigned char chineseTxtSoftCancel[] = {0x80,0x00,0x00,'C',0x00,'a',0x00,'n',0x00,'c',0x00,'e',0x00,'l',0x00,0x00};
#endif /*FF_MMI_CAMERA_APP */

//added by prachi

#if CAM_SENSOR == 1
const unsigned char chineseTxtCamResolution[] = {0x80,0x00,0x00,'C',0x00,'h',0x00,'a',0x00,'n',0x00,'g',0x00,'e',0x00,' ' ,0x00,'R',0x00,'e', 0x00,'s',0x00,'o',0x00,'l',0x00,'u',0x00,'t',0x00,'i',0x00,'o',0x00,'n',0x00,0x00};
const unsigned char chineseTxtCamVga[] = {0x80,0x00,0x00,'V',0x00,'G',0x00,'A',0x00,0x00};
const unsigned char chineseTxtCamQcif[] = {0x80,0x00,0x00,'Q',0x00,'C',0x00,'I',0x00,'F',0x00,0x00};
const unsigned char chineseTxtCamSxga[] = {0x80,0x00,0x00,'S',0x00,'X',0x00,'G',0x00,'A',0x00,0x00};
const unsigned char chineseTxtCamNot[] = {0x80,0x00,0x00,'N',0x00,'o',0x00,'t',0x00,0x00};
const unsigned char chineseTxtCamSupported[] = {0x80,0x00,0x00,'S',0x00,'u',0x00,'p',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,'e',0x00,'d',0x00,0x00};
#endif 

static const tPointerStructure LanguageIndex_2[LEN_LANGUAGE2+1] = {
 { TxtNull,(char*)   chineseTxtNull},
//x0pleela 14 June, 2006  DR: OMAPS00070657
//Edit Line name defined for ALS feature of CPHS
#ifdef FF_CPHS
 {TxtEditLineName,  (char*)  	 chineseTxtEditLineName},
#endif
//x0pleela 20 Mar, 2006   ER: OMAPS00067709
//Added the following strings used in menu items to support Voice buffering feature
#ifdef FF_PCM_VM_VB
    {TxtStopFailed,  (char*)   chineseTxtStopFailed}, 
    {TxtAlready, (char*) chineseTxtAlready},
    {TxtPcmVoiceMemo, (char*) chineseTxtPcmVoiceMemo},
    {TxtVoiceBuffering, (char*) chineseTxtVoiceBuffering},
    {TxtEditNum, (char*) TxtEditNum},
    {TxtStartRec, (char*) chineseTxtStartRec},
    {TxtStopRec, (char*) chineseTxtStopRec},
#endif
#ifdef SIM_PERS  //x0035544 07-11-2005
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
{TxtBlkNetwork, (char*) chineseTxtBlkNetwork},	//For new category "Blocked Network" 
{ TxtEnterPbLock,(char*) chineseTxtEnterPbLock}, //Editor string to display "Enter Blocked Network password"
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
{TxtBusy, (char*)   chineseTxtBusy}, 	//Displays "Busy" if timer is enabled for wrong entry of password
{TxtMasterUnlock, (char*)   chineseTxtMasterUnlock}, //For menu option Master unlock
{TxtEnterMasterKey, (char*)   chineseTxtEnterMasterKey}, //Text for the editor to enter master unlock key
{TxtUnblockME, (char*)   chineseTxtUnblockME}, //For menu option Unblock ME
{TxtUnblockCode, (char*)   chineseTxtUnblockCode}, //x0pleela 16 May, 2006  DR: OMAPS00067919
	{TxtTimerFlag, (char*)   chineseTxtTimerFlag}, //x0035544 07-11-2005
{TxtETSIFlag,(char*)   chineseTxtETSIFlag}, //x0035544 07-11-2005
{TxtAirtelFlag,(char*)   chineseTxtAirtelFlag}, //x0035544 07-11-2005
{ TxtEnterOldPsLock,(char*)   chineseTxtEnterOldPsLock}, //x0035544 07-11-2005
{ TxtEnterPsLockNew,(char*)   chineseTxtEnterPsLockNew}, //x0035544 07-11-2005
{ TxtEnterPsLockconf,(char*)   chineseTxtEnterPsLockconf}, //x0035544 07-11-2005
 { TxtLockEnabled, (char*)   chineseTxtLockEnabled}, //x0035544 07-11-2005
    { TxtLockDisabled,(char*)   chineseTxtLockDisabled}, //x0035544 07-11-2005
    { TxtPersonalization, (char*)   chineseTxtPersonalization}, //x0035544 07-11-2005
    { TxtSupplementaryInfo, (char*)   chineseTxtSupplementaryInfo}, //x0035544 07-11-2005
    { TxtPersonalize, (char*)   chineseTxtPersonalize}, //x0035544 07-11-2005
    { TxtUnlockCategory, (char*)   chineseTxtUnlockCategory}, //x0035544 07-11-2005
    { TxtLockCategory, (char*)   chineseTxtResetFC}, //x0035544 07-11-2005
    { TxtResetFC, (char*)   chineseTxtResetFC}, //x0035544 07-11-2005
    { TxtGetFC,(char*)   chineseTxtGetFC}, //x0035544 07-11-2005
    { TxtMaxFC,(char*)   chineseTxtMaxFC}, //x0035544 07-11-2005
    { TxtCorporate, (char*)   chineseTxtCorporate}, //x0035544 07-11-2005
    { TxtServiceProvider, (char*)   chineseTxtServiceProvider}, //x0035544 07-11-2005
    { TxtNetworkSubset, (char*)   chineseTxtNetworkSubset}, //x0035544 07-11-2005
    { TxtLockActivated,(char*)   chineseTxtLockActivated}, //x0035544 07-11-2005
    { TxtLockDeactivated,(char*)   chineseTxtLockDeactivated}, //x0035544 07-11-2005
    { TxtEnterNLock,(char*)   chineseTxtEnterNLock}, //x0035544 07-11-2005
    { TxtEnterNsLock,(char*)   chineseTxtEnterNsLock}, //x0035544 07-11-2005
    { TxtEnterSpLock,(char*)   chineseTxtEnterSpLock}, //x0035544 07-11-2005
    { TxtEnterCLock,(char*)   chineseTxtEnterCLock}, //x0035544 07-11-2005
    { TxtEnterPsLock,(char*)   chineseTxtEnterPsLock}, //x0035544 07-11-2005
     { TxtPsLockConfWrong,(char*)   chineseTxtPsLockConfWrong}, //x0035544 07-11-2005
    { TxtPsLockConfwrong2,(char*)   chineseTxtPsLockConfwrong2}, //x0035544 07-11-2005
    { TxtPsLockChanged,(char*)   chineseTxtPsLockChanged}, //x0035544 07-11-2005
    { TxtCounterReset,(char*)   chineseTxtCounterReset}, //x0035544 07-11-2005
     { TxtPassfailed,(char*)   chineseTxtPassfailed},  //x0035544 07-11-2005
     {TxtUnblock,(char*)   chineseTxtUnblock}, //x0035544 07-11-2005
    { TxtAlreadyLocked,(char*)   chineseTxtAlreadyLocked}, //x0035544 07-11-2005
    { TxtAlreadyUnlocked,(char*)   chineseTxtAlreadyUnlocked}, //x0035544 07-11-2005
    {  TxtPhoneUnblocked,(char*)   chineseTxtPhoneUnblocked}, //x0035544 07-11-2005
    { TxtPhoneBlocked,(char*)   chineseTxtPhoneBlocked }, //x0035544 07-11-2005
    { TxtGetFailFC,(char*)   chineseTxtGetFailFC}, //x0035544 07-11-2005
	{ TxtMaxFailFC,(char*)   chineseTxtMaxFailFC}, //x0035544 07-11-2005
	{ TxtGetSuccFC,(char*)   chineseTxtGetSuccFC}, //x0035544 07-11-2005
	 { TxtMaxSucFC,(char*)   chineseTxtMaxSucFC}, //x0035544 07-11-2005
#endif     //x0035544 07-11-2005
#ifdef FF_MMI_CPHS //x0035544 07-11-2005
    { TxtViewAll,(char*)   chineseTxtViewAll}, //x0035544 07-11-2005
    { TxtEmergencyNumbers,(char*)   chineseTxtEmergencyNumbers}, //x0035544 07-11-2005
#endif   //x0035544 07-11-2005 

	{ TxtNotImplemented,(char*)   chineseTxtNotImplemented},
    { TxtManu,(char*) chineseTxtManu},
    { TxtEnterPin1,(char*)   chineseTxtEnterPin1},
    { TxtEnterPin2,(char*)   chineseTxtEnterPin2},
    { TxtEnterPuk1,(char*)   chineseTxtEnterPuk1},
    { TxtEnterPuk2,(char*)   chineseTxtEnterPuk2},
	{ TxtInvalidCard,(char*)   chineseTxtInvalidCard},
    { TxtNoCard,(char*)   chineseTxtNoCard},
    { TxtInvalidIMEI,(char*)   chineseTxtInvalidIMEI}, // Nov 24, 2005, a0876501, DR: OMAPS00045909
    { TxtSoftCall,(char*)   chineseTxtSoftCall},
    { TxtSoftOK,(char*)   chineseTxtSoftOK},
    { TxtSoftMenu,(char*)   chineseTxtSoftMenu},
    { TxtHelp,(char*)   chineseTxtHelp},
    { TxtSoftContacts,(char*)   chineseTxtSoftContacts},
    { TxtPlay,(char*)   chineseTxtPlay},
    { TxtMessages,(char*)   chineseTxtMessages},
    { TxtAlarm,(char*)   chineseTxtAlarm},
    { TxtSettings,(char*)   chineseTxtSettings},
 	{ TxtExtra ,(char*)    chineseTxtExtra},
    { TxtSend ,(char*)    chineseTxtSend},
	{ TxtSent ,(char*)    chineseTxtSent},
	{ TxtNotSent ,(char*)    chineseTxtNotSent},
    { TxtServiceCentre,(char*)    chineseTxtServiceCentre},
    { TxtPleaseWait,(char*)    chineseTxtPleaseWait},
 	{ TxtSmsEnterNumber,(char*)    chineseTxtSmsEnterNumber},
    { TxtRead,(char*)    chineseTxtRead},
        { TxtRetrieve, (char*)    chineseTxtRetrieve },   //x0035544 07-11-2005
	{ TxtDeleteAll,(char*)    chineseTxtDeleteAll},
    { TxtSave,(char*)    chineseTxtSave},
    { TxtVoicemail,(char*)    chineseTxtVoicemail},
    { TxtBroadcast,(char*)    chineseTxtBroadcast},
    { TxtNew,(char*)    chineseTxtNew},
    { TxtDigits,(char*) chineseTxtDigits },
    { TxtLowercase,(char*) chineseTxtLowercase },
    { TxtUppercase,(char*) chineseTxtUppercase },
    { TxtApplications,(char*)    chineseTxtApplications},
#ifndef FF_NO_VOICE_MEMO
    { TxtVoiceMemo,(char*)    chineseTxtVoiceMemo},
#endif
    { TxtSimNot,(char*) chineseTxtSimNot },
    { TxtReady,(char*) chineseTxtReady },
 	{ TxtSmsListFull,(char*)    chineseTxtSmsListFull},
    { TxtNewCbMessage,(char*)    chineseTxtNewCbMessage},
    { TxtSaved,(char*)    chineseTxtSaved},
    { TxtDelete,(char*)    chineseTxtDelete},
    { TxtReply,(char*)    chineseTxtReply},
     { TxtStoreNumber,(char*)    chineseTxtStoreNumber},
#ifdef FF_CPHS_REL4       
    { TxtMSP,(char*) chineseTxtMSP},
    { TxtMessageStatus,(char*) chineseTxtMessageStatus},
    { TxtMailBox,(char*) chineseTxtMailBox},
    { TxtElectronic,(char*) chineseTxtElectronic},
#endif    
    { TxtForward,(char*)    chineseTxtForward},
    { TxtActivate,(char*)    chineseTxtActivate},
    { TxtTopics,(char*)    chineseTxtTopics},
    { TxtConsult,(char*)    chineseTxtConsult},
    { TxtAddNew,(char*)    chineseTxtAddNew},
     { TxtContacts,(char*)    chineseTxtContacts},
    { TxtSearchName,(char*)    chineseTxtSearchName},
    { TxtMemory,(char*)    chineseTxtMemory},
    { TxtMyNumber,(char*)    chineseTxtMyNumber},
    {TxtMyNumberFull,(char*) chineseTxtMyNumberFull}, //added by RanG :31705 13/06/05
    { TxtRinger,(char*)    chineseTxtRinger},
    { TxtVolume,(char*)    chineseTxtVolume},
    { TxtKeypadTones,(char*)    chineseTxtKeypadTones},
    { TxtNewMessage,(char*)    chineseTxtNewMessage},
    { TxtNewBroadcast,(char*)    chineseTxtNewBroadcast},
      { TxtAutoAnswer,(char*)    chineseTxtAutoAnswer},
    { TxtTimeDate,(char*)    chineseTxtTimeDate},
    { TxtLanguages,(char*)    chineseTxtLanguages},
    { TxtVoiceMail,(char*)    chineseTxtVoiceMail},
    { TxtPinCode,(char*)    chineseTxtPinCode},
    { TxtAutoredial,(char*)    chineseTxtAutoredial},
    { TxtDTMF,(char*)    chineseTxtDTMF},
     { TxtOn,(char*)    chineseTxtOn},
    { TxtOff,(char*)    chineseTxtOff},
     { TxtNetworkSel,(char*)    chineseTxtNetworkSel},
    { TxtSendID,(char*)    chineseTxtSendID},
    { TxtChargeTimers,(char*)    chineseTxtChargeTimers},
    { TxtCallDivert,(char*)    chineseTxtCallDivert},
     { TxtCallWaiting,(char*)    chineseTxtCallWaiting},
    { TxtIfBusy,(char*)    chineseTxtIfBusy},
    { TxtIfNoAnswer,(char*)    chineseTxtIfNoAnswer},
    { TxtIfNotReachable,(char*)    chineseTxtIfNotReachable},
    { TxtAllCalls,(char*)    chineseTxtAllCalls},
    { TxtCancelAll,(char*)    chineseTxtCancelAll},
    { TxtAll,(char*)    chineseTxtAll},
    { TxtVoice,(char*)    chineseTxtVoice},
    { TxtFax,(char*)    chineseTxtFax},
    { TxtData,(char*)    chineseTxtData},
    { TxtStatus,(char*)    chineseTxtStatus},
    { TxtCancel,(char*)    chineseTxtCancel},
    { TxtCalculator,(char*)    chineseTxtCalculator},
     { TxtEnterName,(char*)    chineseTxtEnterName},
    { TxtSoftBack,(char*)    chineseTxtSoftBack},
	{ TxtSoftSelect,(char*)    chineseTxtSoftSelect},
    { TxtSoftOptions,(char*)    chineseTxtSoftOptions},
    { TxtEmptyList,(char*)    chineseTxtEmptyList},
    { TxtSendMessage,(char*)    chineseTxtSendMessage},
    { TxtChange,(char*)    chineseTxtChange},
    { TxtEnterNumber,(char*)    chineseTxtEnterNumber},
    { TxtPhbkFull,(char*)    chineseTxtPhbkFull},
    { TxtWriteError,(char*)    chineseTxtWriteError},
    { TxtEntryDeleted,(char*)    chineseTxtEntryDeleted},
    { TxtDetails,(char*)    chineseTxtDetails},
    { TxtHold,(char*)    chineseTxtHold},
    { TxtUnhold,(char*)    chineseTxtUnhold},
    { TxtMute,(char*)    chineseTxtMute},
    { TxtSendDTMF,(char*)    chineseTxtSendDTMF},
    { TxtMainMenu,(char*)    chineseTxtMainMenu},
     { TxtSwap,(char*)    chineseTxtSwap},
    { TxtEndAll,(char*)    chineseTxtEndAll},
    // Apr 26 2006, a0393213 (Prabakar R), OMAPS00074886 : No option in MMI to release the held call. 
    // Added a menu option. "End held" text is added to the language index
    { TxtEndHeld,(char*)    chineseTxtEndHeld}, 
     { TxtPrivateCall,(char*)    chineseTxtPrivateCall},
    { TxtStartConference,(char*)    chineseTxtStartConference},
    { TxtReStartConference,(char*)    chineseTxtReStartConference},
    { TxtEndConference, (char*) chineseTxtReStartConference },
    { TxtFind,(char*)    chineseTxtFind},
    { TxtServiceNumbers,(char*)    chineseTxtServiceNumbers},
    { TxtOutofRange,(char*) chineseTxtOutofRange},
      { TxtDone,(char*)    chineseTxtDone},
    { TxtFailed,(char*)    chineseTxtFailed},
#ifdef FF_MMI_MULTIMEDIA    
    { TxtFileNotSupported, (char *)chineseTxtFileNotSupported},
#endif    
    { TxtEnterNewPin,(char*)    chineseTxtEnterNewPin},
    { TxtConfPin,(char*)    chineseTxtConfPin},
	{ TxtChangPin,(char*)    chineseTxtChangPin},
    { TxtEnd,(char*)    chineseTxtEnd},
    { TxtPINFail,(char*)    chineseTxtPINFail},
     { TxtSOS,(char*)    chineseTxtSOS},
    { TxtAutomatic,(char*)    chineseTxtAutomatic},
    { TxtManual,(char*)    chineseTxtManual},
    { TxtFixedNames,(char*)    chineseTxtFixedNames},
    { TxtFactoryReset,(char*)    chineseTxtFactoryReset},
     { TxtSearching,(char*)    chineseTxtSearching },
     { TxtChangPIN2,(char*)    chineseTxtChangePIN2 },
    { TxtOldPIN,(char*)    chineseTxtOldPIN },
    { TxtOldPIN2,(char*)    chineseTxtOldPIN2 },
     { TxtNotAcc,(char*)    chineseTxtNotAcc },
     { TxtEnabled,(char*)    chineseTxtEnabled },
    { TxtDisabled,(char*)    chineseTxtDisabled },
    { TxtPINOK,(char*)    chineseTxtPINOK },
    { TxtSimBlocked,(char*)    chineseTxtSimBlocked },
    { TxtNewPIN,(char*)    chineseTxtNewPIN },
    { TxtAcc,(char*)    chineseTxtAcc },
    { TxtNoNetwork,(char*)    chineseTxtNoNetwork },
    { TxtNo,(char*)    chineseTxtNo },
    { TxtEnterDate,(char*)    chineseTxtEnterDate },
    { TxtEnterTime,(char*)    chineseTxtEnterTime },
    { TxtScratchPad,(char*)    chineseTxtScratchPad },
    { TxtDeleted,(char*)    chineseTxtDeleted },
    { TxtActivated,(char*)    chineseTxtActivated },
    { TxtDeActivated,(char*)    chineseTxtDeActivated },
    { TxtDeactivate,(char*)    chineseTxtDeactivate },
    { TxtNotActivated,(char*)    chineseTxtNotActivated }, // Dec 07, 2005  REF: OMAPS00050087 x0039928 - Added string to lang table
    { TxtCheck,(char*)    chineseTxtCheck },
     { TxtYes,(char*)    chineseTxtYes },
    { TxtCallNumber,(char*)    chineseTxtCallNumber },
    { TxtEnterMid,(char*)    chineseTxtEnterMid },
    { TxtReadError,(char*)    chineseTxtReadError },
     { TxtUsed,(char*)    chineseTxtUsed },
    { TxtOutgoing,(char*)    chineseTxtOutgoing },
    { TxtMuted,(char*)    chineseTxtMuted },
    { TxtCallEnded,(char*)    chineseTxtCallEnded },
    { TxtAnsweredCalls, (char*) chineseTxtAnsweredCalls }, // API - APR #1629 - Added string to lang table.    
    { TxtIncomingCall,(char*)    chineseTxtIncomingCall },
	{ TxtIncomingData,(char*)    chineseTxtIncomingCall }, //s.b should translate the "incoming data" in chinese
    { TxtIncomingFax,(char*)    chineseTxtIncomingFax }, //SPR#1147 - DS - Added string for incoming fax.
    { TxtNumberWithheld,(char*)    chineseTxtNumberWithheld },
    { TxtAccept,(char*)    chineseTxtAccept },
    { TxtReject,(char*)    chineseTxtReject },
    { TxtCalling,(char*)    chineseTxtCalling },
    { TxtMissedCalls,(char*)    chineseTxtMissedCalls },
    { TxtMissedCall,(char*)    chineseTxtMissedCall },
     { TxtLocked,(char*)    chineseTxtLocked },
    { TxtUnmute,(char*)    chineseTxtUnmute },
    { TxtCharging,(char*)    chineseTxtCharging },
    { TxtChargComplete,(char*)    chineseTxtChargComplete },
    { TxtOperationNotAvail,(char*)    chineseTxtOperationNotAvail },
    { TxtAddToConference,(char*)    chineseTxtAddToConference },
    { TxtEmpty,(char*)    chineseTxtEmpty },
    { TxtSelectFromList,(char*)    chineseTxtSelectFromList },
    { TxtEnterNetworkCode,(char*)    chineseTxtEnterNetworkCode },
    { TxtMCCMNC,(char*)    chineseTxtMCCMNC },
#ifndef FF_NO_VOICE_MEMO
     { TxtRecord,(char*)    chineseTxtRecord },
#endif
	{ TxtSecurity,(char*)    chineseTxtSecurity },
	{ TxtEnglish,(char*)    chineseTxtEnglish },
	{ TxtGerman,(char*)    chineseTxtGerman },
    { TxtValidity,(char*)    chineseTxtValidity },
    { TxtMessageTyp,(char*)    chineseTxtMessageTyp },
    { TxtSetVoiceMail,(char*)    chineseTxtSetVoiceMail },
    { TxtCallTimers,(char*)    chineseTxtCallTimers },
    { TxtLastCharge,(char*)    chineseTxtLastCharge },
    { TxtTotalCharge,(char*)    chineseTxtTotalCharge },
    { TxtResetCharge,(char*)    chineseTxtResetCharge },
    { TxtIfNoService,(char*)    chineseTxtIfNoService },
    { TxtDiscard,(char*)    chineseTxtDiscard },
    { TxtEDIT,(char*)    chineseTxtEDIT },
    { TxtExtractNumber,(char*)    chineseTxtExtractNumber },
    { TxtWrite,(char*)    chineseTxtWrite },
    { TxtCheckStatus,(char*)    chineseTxtCheckStatus },
    { TxtSetDivert,(char*)    chineseTxtSetDivert },
    { TxtCancelDivert,(char*)    chineseTxtCancelDivert },
    { TxtHideID,(char*)    chineseTxtHideID },
    { TxtTones,(char*)    chineseTxtTones },
    { TxtClock,(char*)    chineseTxtClock },
 	{ TxtPhone,(char*)    chineseTxtPhone },
     { TxtStop,(char*)    chineseTxtStop },
#ifndef FF_NO_VOICE_MEMO
    { TxtRecording,(char*)    chineseTxtRecording },
    { TxtPlayback,(char*)    chineseTxtPlayback },
#endif
    { TxtNoNumber,(char*)    chineseTxtNoNumber },
    { TxtOkToDelete,(char*)    chineseTxtOkToDelete },
    { TxtBalance,(char*)    chineseTxtBalance },
    { TxtLimit,(char*)    chineseTxtLimit },
    { TxtLimitEq,(char*)    chineseTxtLimitEq },
    { TxtRate,(char*)    chineseTxtRate },
    { TxtMinutes,(char*)    chineseTxtMinutes },
    { TxtCost,(char*)    chineseTxtCost },
    { TxtCredit,(char*)    chineseTxtCredit },
     { TxtPressOk,(char*)    chineseTxtPressOk },
    { TxtReset,(char*)    chineseTxtReset },
     { TxtIncoming,(char*)    chineseTxtIncoming },
    { TxtCounter,(char*)    chineseTxtCounter },
    { TxtOutgoingCalls,(char*)    chineseTxtOutgoingCalls },
    { TxtCostCounter,(char*)    chineseTxtCostCounter },
    { TxtEnterCurrency,(char*)    chineseTxtEnterCurrency },
    { Txt1Unit,(char*)    chineseTxt1Unit },
    { TxtChanged,(char*)    chineseTxtChanged },
    { TxtEnterLimit,(char*)    chineseTxtEnterLimit },
     { TxtCancelled,(char*)    chineseTxtCancelled },
    { TxtView,(char*)    chineseTxtView },
     { TxtEnterPCK,(char*)    chineseTxtEnterPCK },
    { TxtEnterCCK,(char*)    chineseTxtEnterCCK },
    { TxtEnterNCK,(char*)    chineseTxtEnterNCK },
    { TxtEnterNSCK,(char*)    chineseTxtEnterNSCK },
    { TxtEnterSPCK,(char*)    chineseTxtEnterSPCK },
    { TxtCodeInc,(char*)    chineseTxtCodeInc },
    { TxtBlckPerm,(char*)    chineseTxtBlckPerm },
    { TxtDealer,(char*)    chineseTxtDealer },
    { TxtSendingDTMF,(char*)    chineseTxtSendingDTMF },
    { TxtCheckNumber,(char*)    chineseTxtCheckNumber },
    { TxtNumberBusy,(char*)    chineseTxtNumberBusy },
    { TxtNoAnswer,(char*)    chineseTxtNoAnswer }, 
    { TxtNumberChanged,(char*)    chineseTxtNumberChanged },
    { TxtNetworkBusy,(char*)    chineseTxtNetworkBusy },
    { TxtNotSubscribed,(char*)    chineseTxtNotSubscribed },
    { TxtAnyKeyAnswer,(char*)    chineseTxtAnyKeyAnswer },
    { TxtPINBlock,(char*)    chineseTxtPINBlock },
     { TxtEdit,(char*)    chineseTxtEdit },
    { TxtChangePassword,(char*)    chineseTxtChangePassword },
    { TxtSetBar,(char*)    chineseTxtSetBar },
    { TxtSmsTone,(char*)    chineseTxtSmsTone },
    { TxtMelody,(char*)    chineseTxtMelody },
    { TxtSilent,(char*)    chineseTxtSilent },
    { TxtClick,(char*)    chineseTxtClick },
    { TxtClearAlarm,(char*)    chineseTxtClearAlarm },
    { TxtTimeFormat,(char*)    chineseTxtTimeFormat },
    { TxtTwelveHour,(char*)    chineseTxtTwelveHour },
	{ TxtTwentyfourHour,(char*)    chineseTxtTwentyfourHour }, 
    { TxtInCallTimer,(char*)    chineseTxtInCallTimer },
     { TxtStore,(char*)    chineseTxtStore },
    { TxtNone,(char*)    chineseTxtNone }, 
     { TxtPIN,(char*)    chineseTxtPIN },
     { TxtPIN2,(char*)    chineseTxtPIN2 },
     { TxtConfPin2,(char*)    chineseTxtConfPin2 },
    { TxtNewPIN2,(char*)    chineseTxtNewPIN2 }, 
    { TxtUnlockOK,(char*)    chineseTxtUnlockOK },
 	{ TxtIMEI,(char*)    chineseTxtIMEI },
	{ TxtList,(char*)    chineseTxtList },
	{ TxtFull,(char*)    chineseTxtFull },
     { TxtNotAvailable,(char*)    chineseTxtNotAvailable },
    { TxtCallBAOC,(char*) chineseTxtCallBAOC},
    { TxtCallBAOIC,(char*) chineseTxtCallBAOIC },
    { TxtCallBAOICexH,(char*) chineseTxtCallBAOICexH },
    { TxtCallBAIC,(char*) chineseTxtCallBAIC },
    { TxtCallBAICroam,(char*) chineseTxtCallBAICroam},
    { TxtCallBarringAll,(char*) chineseTxtCallBarringAll},
    { TxtCallBAOCAll,(char*) chineseTxtCallBAOCAll },
    { TxtCallBAICAll,(char*) chineseTxtCallBAICAll},
    { TxtCLIP,(char*) chineseTxtCLIP},
    { TxtCOLP,(char*) chineseTxtCOLP},
    { TxtCOLR,(char*) chineseTxtCOLR},
    { TxtSimToolkit,(char*)    chineseTxtSimToolkit },
	{ TxtPhonebook,(char*)    chineseTxtPhonebook },
    { TxtRecentCalls,(char*)    chineseTxtRecentCalls },
    { TxtNames,(char*)    chineseTxtNames }, 
     { TxtInterCalls,(char*)    chineseTxtInterCalls },
    { TxtInterHome,(char*)    chineseTxtInterHome }, 
    { TxtWhenRoaming,(char*)    chineseTxtWhenRoaming },
    { TxtCancelAllBars,(char*)    chineseTxtCancelAllBars },
 	{ TxtWaiting,(char*)    chineseTxtWaiting }, 
	{ TxtBarred,(char*)    chineseTxtBarred }, 
	{ TxtForwarded,(char*)    chineseTxtForwarded },
    { TxtCallDivertNoReach,(char*)    chineseTxtCallDivertNoReach },
    { TxtCallDivertNoReply,(char*)    chineseTxtCallDivertNoReply },
    { TxtCallDivertIfBusy,(char*)    chineseTxtCallDivertIfBusy }, 
    { TxtPressMenu,(char*) chineseTxtPressMenu},
    {TxtPlus,(char*)    chineseTxtPlus },
    {TxtMinus,(char*)    chineseTxtMinus },
    {TxtDivideBy,(char*)    chineseTxtDivideBy },
    {TxtMultiplyBy,(char*)    chineseTxtMultiplyBy },
    {TxtEquals,(char*)    chineseTxtEquals },
    {TxtSending,(char*)	chineseTxtSending },
    {TxtMessage,(char*)    chineseTxtMessage },
    {TxtSoftNext,(char*)    chineseTxtSoftNext },
    {TxtLastCall,(char*)    chineseTxtLastCall },
    {TxtIncomingCalls,(char*)    chineseTxtIncomingCall },
    {TxtLow,(char*)    chineseTxtLow },
    {TxtMedium,(char*)    chineseTxtMedium },
    {TxtHigh,(char*)    chineseTxtHigh },
    {TxtIncreasing,(char*)    chineseTxtIncreasing },
    {TxtTimerReset,(char*)    chineseTxtTimerReset },
    {TxtClockSetTo,(char*)    chineseTxtClockSetTo }, 
    /* Marcus: Issues 1122 & 1137: 12/09/2002: Start */
    {TxtSilentMode,(char*) chineseTxtSilentMode},
    /* Marcus: Issues 1122 & 1137: 12/09/2002: End */
    {TxtSmsValidityPeriodSet,(char*) chineseTxtValiditySet},
    {Txt1Hour,(char*) chineseTxt1Hour},
    {Txt12Hours,(char*) chineseTxt12Hour},
    {Txt24Hours,(char*) chineseTxt24Hours},
    {Txt1Week,(char*) chineseTxt1Week},
    {TxtMaxTime,(char*) chineseTxtMaxTime},////  May 10, 2004    REF: CRR 15753  Deepa M.D 
    {TxtPin2Code,(char*) chineseTxtPIN2},
    {TxtEnablePIN,(char*) chineseTxtEnablePIN},
    {TxtRequesting,(char*) chineseTxtRequesting},
    {TxtNotAllowed,(char*) chineseTxtNotAllowed},
    {TxtEmergency,(char*) chineseTxtEmergency},
    {TxtChinese,(char*)  chineseTxtChinese },
	{TxtPassword,(char*) chineseTxtPassword },
    {TxtConfirmPassword,(char*) chineseTxtConfirmPassword},
    {TxtNewPassword,(char*) chineseTxtNewPassword},
    {TxtNoMessages,(char*) chineseTxtNoMessages},
    {TxtTwoAttemptsLeft,(char*) chineseTxtTwoAttemptsLeft},
    {TxtOneAttemptLeft,(char*) chineseTxtOneAttemptLeft},
		{Txtblacklisted,	(char*) chineseTxtblacklisted},/*MC SPR 1242, added table entries*/
		{TxtErrUnknown,		(char*) chineseTxtErrUnknown},
		{TxtSSErr,			(char*) chineseTxtSSErr},
		{TxtUnknownSub,		(char*) chineseTxtUnknownSub},
		{TxtIllSub,			(char*) chineseTxtIllSub},
		{TxtIllEqip,		(char*) chineseTxtIllEqip},
		{TxtIllOp,			(char*) chineseTxtIllOp},
		{TxtSubVil,			(char*) chineseTxtSubVil},
		{TxtInComp,			(char*) chineseTxtInComp},
		{TxtFacNoSup,		(char*) chineseTxtFacNoSup},
		{TxtAbsSub,			(char*) chineseTxtAbsSub},
		{TxtSysFail,		(char*) chineseTxtSysFail},
		{TxtMissDta,		(char*) chineseTxtMissDta},
		{TxtUnexDta,		(char*) chineseTxtUnexDta},
		{TxtPwdErr,			(char*) chineseTxtPwdErr},
		{TxtUnkAlph,		(char*) chineseTxtUnkAlph},
		{TxtMaxMpty,		(char*) chineseTxtMaxMpty},
		{TxtResNoAvai,		(char*) chineseTxtResNoAvai},
		{TxtUnRec,			(char*) chineseTxtUnRec},
		{TxtMisTyp,			(char*) chineseTxtMisTyp},
		{TxtBadStruct,		(char*) chineseTxtBadStruct},
		{TxtDupInv,			(char*) chineseTxtDupInv},
		{TxtResLim,			(char*) chineseTxtResLim},
		{TxtIniRel,			(char*) chineseTxtIniRel},
		{TxtFatRes,			(char*) chineseTxtFatRes},
		{TxtEntAnw,			(char*) chineseTxtEntAnw},
		{TxtSsMsg,			(char*) chineseTxtSsMsg},/*MC end*/
	{TxtOld,(char*) chineseTxtOld},
 { TxtInsertnote, (char*)chineseTxtInsertnote },
 { TxtMelodygenerator, (char*)chineseTxtMelodygenerator },
 { TxtOctaveHigher, (char*)chineseTxtOctaveHigher },
 { TxtOctaveLower, (char*)chineseTxtOctaveLower },
 { TxtLoadMelody, (char*)chineseTxtLoadMelody },
 { TxtEasyText, (char*)chineseTxtEasyText },
 { TxtDataCount, (char*)chineseTxtDataCount },
 { TxtQoS, (char*)chineseTxtQoS },
 { TxtTypeofConnection, (char*)chineseTxtTypeofConnection },
 { TxtTotalCount, (char*)chineseTxtTotalCount },
 { TxtResetCount, (char*)chineseTxtResetCount },
 { TxtDataCounter, (char*)chineseTxtDataCounter },
 { TxtV42bis, (char*)chineseTxtV42bis },
 { TxtNormal, (char*)chineseTxtNormal },
 { TxtCSD, (char*)chineseTxtCSD },
 { TxtClassB, (char*)chineseTxtClassB },
 { TxtClassC, (char*)chineseTxtClassC },
 { TxtIPAddress, (char*)chineseTxtIPAddress },
/* SPR#2324 - SH - Removed some strings */
 { TxtIPAddress2, (char*)chineseTxtIPAddress2 },
 { TxtPort1, (char*)chineseTxtPort1 },
 { TxtPort2, (char*)chineseTxtPort2 },
 { TxtAPN, (char*)chineseTxtAPN },
 { TxtConnectionType, (char*)chineseTxtConnectionType },
//xpradipg - Aug 4:changes for WAP2.0 Menu
// Jan 12, 2004    REF: CRR 28078 xnkulkar
// Description: Compilation error in MmiBlkLangDB.c with FF_WAP && FF_TCPIP
// Solution: TxtPPGAuthenication changed to TxtPPGAuthentication
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
 {TxtNameServer1, (char*)chineseTxtNameServer1},
 {TxtNameServer2, (char*)chineseTxtNameServer2},
 {TxtPPGAuthentication, (char*)chineseTxtPPGAuthentication},
 {TxtWirelessProfiledHTTP, (char*)chineseTxtWirelessProfiledHTTP},
#endif

 { TxtGPRS, (char*)chineseTxtGPRS },
 { TxtOutgoingData, (char*)chineseTxtOutgoingData },
 { TxtGame, (char*)chineseTxtGame },
 { TxtHomepage, (char*)chineseTxtHomepage },
 { TxtBookmarks, (char*)chineseTxtBookmarks },
 { TxtGoTo, (char*)chineseTxtGoTo },
 { TxtHistory, (char*)chineseTxtHistory },
 { TxtSetup, (char*)chineseTxtSetup },
 { TxtNewBookmark, (char*)chineseTxtNewBookmark },
 { TxtWWWdot, (char*)chineseTxtWWWdot },
 { TxtWAPdot, (char*)chineseTxtWAPdot },
 { TxtSMS, (char*)chineseTxtSMS },
 { TxtEmail, (char*)chineseTxtEmail },
 { TxtClear, (char*)chineseTxtClear },
 { TxtBookmark, (char*)chineseTxtBookmark },
/* SPR#2324 - SH - Removed some strings */
 { TxtSaveHistory, (char*)chineseTxtSaveHistory },
 { TxtEditName, (char*)chineseTxtEditName },
 { TxtEditURL, (char*)chineseTxtEditURL },
/* SPR#2324 - SH - Removed some strings */
 { TxtResponseTimer, (char*)chineseTxtResponseTimer },
 { TxtDialupNumber, (char*)chineseTxtDialupNumber },
 { TxtTemporary, (char*)chineseTxtTemporary },
 { TxtContinuous, (char*)chineseTxtContinuous },
/* SPR#2324 - SH - Removed some strings */
 { TxtWAP, (char*)chineseTxtWAP },
 { TxtExit, (char*)chineseTxtExit },
 { TxtReload, (char*)chineseTxtReload },
 { TxtStored, (char*)chineseTxtStored },
 { TxtOther, (char*)chineseTxtOther },
 { TxtConnecting, (char*)chineseTxtConnecting },
 { TxtDownloading, (char*)chineseTxtDownloading },
 { TxtUpdating, (char*)chineseTxtUpdating },
 { TxtEnterURL, (char*)chineseTxtEnterURL },
 { TxtProfileName, (char*)chineseTxtProfileName },
 { TxtISPUsername, (char*)chineseTxtISPUsername },
 { TxtISPPassword, (char*)chineseTxtISPPassword },
 { TxtGoToLink, (char*)chineseTxtGoToLink },
 { TxtSelect, (char*)chineseTxtSelect },
 { TxtNotConnected, (char*)chineseTxtNotConnected },
 { TxtScaleImages, (char*)chineseTxtScaleImages },
 { TxtLoggingIn, (char*)chineseTxtLoggingIn },
 { TxtPageNotFound, (char*)chineseTxtPageNotFound},
 { TxtInvalidURL, (char*)chineseTxtInvalidURL},
 { TxtNoResponse, (char*)chineseTxtNoResponse},
 { TxtAccessDenied, (char*)chineseTxtAccessDenied},
 { TxtErrorInPage, (char*)chineseTxtErrorInPage},
 { TxtScriptError, (char*)chineseTxtScriptError},
 { TxtServerError,  (char*)chineseTxtServerError},
 { TxtGatewayError, (char*)chineseTxtGatewayError},
 { TxtUnexpectedErr, (char*)chineseTxtUnexpectedErr},
 { TxtCharacterSet, (char*)chineseTxtCharacterSet},
 { TxtWBXMLError, (char*)chineseTxtWBXMLError},
 { TxtFatalError, (char*)chineseTxtFatalError},
 { TxtGatewayBusy, (char*)chineseTxtGatewayBusy},
 { TxtSecurityFail, (char*)chineseTxtSecurityFail},
 { TxtErrorOutOfMem, (char*)chineseTxtErrorOutOfMem},
 { TxtErrorMemLow, (char*)chineseTxtErrorMemLow},
 { TxtError, (char*)chineseTxtError },
 { TxtErrorHTTP, (char*)chineseTxtErrorHTTP },
 { TxtErrorWSP, (char*)chineseTxtErrorWSP },
 { TxtErrorWAE, (char*)chineseTxtErrorWAE },
 { TxtErrorWSPCL, (char*)chineseTxtErrorWSPCL },
 { TxtErrorWSPCM, (char*)chineseTxtErrorWSPCM },
 { TxtErrorWTP, (char*)chineseTxtErrorWTP },
 { TxtErrorWTLS, (char*)chineseTxtErrorWTLS },
 { TxtErrorWDP, (char*)chineseTxtErrorWDP },
 { TxtErrorUDCP, (char*)chineseTxtErrorUDCP },
  { TxtNotSupported, (char*)chineseTxtNotSupported },
 { TxtSelectLine, (char*)chineseTxtSelectLine},/*MC SPR 1242, added table entries*/
  { TxtLine1, (char*)chineseTxtLine1},
  { TxtLine2, (char*)chineseTxtLine2},
 { TxtInfoNumbers, (char*)chineseTxtInfoNumbers},/*MC end*/
 { TxtAttach, (char *)chineseTxtAttach }, /* SH 18/01/02 */
 { TxtDetach, (char *)chineseTxtDetach }, /* SH 18/01/02 */
 { TxtDetaching, (char *)chineseTxtDetaching },/* MZ 15/07/03 */
 { TxtDefineContext, (char *)chineseTxtDefineContext}, /*SH 22/01/02 */
 { TxtActivateContext, (char *)chineseTxtActivateContext}, /*SH 22/01/02 */
 { TxtDeactivateContext, (char *)chineseTxtDeactivateContext}, /*SH 22/01/02 */
 { TxtGPRSPreferred, (char *)chineseTxtGPRSPreferred}, /*SH 25/01/02 */
 { TxtCSDPreferred, (char *)chineseTxtCSDPreferred},/*SH 22/01/02 */
 { TxtNotAttached, (char *)chineseTxtNotAttached},/*SH 22/01/02 */
 { TxtAlreadyAttached, (char *)chineseTxtAlreadyAttached},/*SH 8/02/02 */
 { TxtSelected, (char *)chineseTxtSelected}, /*ap 06/02/02*/
 { TxtNetwork, (char *)chineseTxtNetwork}, /*ap 06/02/02*/
 { TxtForbidden, (char *)chineseTxtForbidden}, /*ap 06/02/02*/
 { TxtActivateFDN, (char *)chineseTxtActivateFDN}, /*ap 06/02/02*/
 { TxtPlease, (char *)chineseTxtPlease},	/*ap 06/02/02*/
 { TxtTimedOut, (char *)chineseTxtTimedOut},	// sh 13/02/02
 { TxtWillChangeOn, (char *)chineseTxtWillChangeOn},	// sh 13/02/02
 { TxtAttached, (char *)chineseTxtAttached},
 { TxtDetached, (char *)chineseTxtDetached},
  { TxtUnknown, (char *)chineseTxtUnknown}, //x0035544 07-11-2005 
 {	TxtCallTransfer,(char*)chineseTxtCallTransfer},/*MC SPR 1242, added table entries*/
 {	TxtConnected,(char*)chineseTxtConnected},
 {  TxtGSM_900,	(char*)chineseTxtGSM_900},
 {   TxtDCS_1800,(char*)chineseTxtDCS_1800},
{	TxtPCS_1900,(char*)chineseTxtPCS_1900},
 {   TxtE_GSM,(char*)chineseTxtE_GSM},
{	TxtGSM_850,(char*)chineseTxtGSM_850},
{	TxtCallTimeout,(char*)chineseTxtCallTimeout},
{	TxtBand,(char*)chineseTxtBand},
{	TxtSatRefresh,(char*)chineseTxtSatRefresh},
{	TxtCallBlocked,(char*)chineseTxtCallBlocked},/*MC end*/
   { TxtSRR, (char *)chineseTxtSRR}, // Marcus: Issue 1170: 30/09/2002
//March 2, 2005    REF: CRR 11536 x0018858
        { TxtRP, (char *)chineseTxtRP },//added this string for the reply path.
   { TxtDelivered, (char *)chineseTxtDelivered }, // Marcus: Issue 1170: 07/10/2002
   { TxtTempError, (char *)chineseTxtTempError }, // Marcus: Issue 1170: 07/10/2002 
 {	TxtUnknownCode,(char*)chineseUnknownCode},/*MC SPR 1111*/
   { TxtPhoneSettings, (char *)chineseTxtPhoneSettings},  //SPR#1113 - DS - Added text id for renamed main menu item "Phone Settings"
    { TxtNetworkServices, (char *) chineseTxtNetworkServices},  //SPR#1113 - DS - Added text id for new main menu item "Netw. Services"
    { TxtCallDeflection, (char *) chineseTxtCallDeflection},  //SPR#1113 - DS - Added text id for new menu item "Call Deflection" (CD)
    { TxtCallBack, (char *) chineseTxtCallBack},  //SPR#1113 - DS - Added text id for new menu item "Call Back" (CCBS)
  { TxtMobileInfo,  (char *)chineseTxtMobileInfo},  /*MC, SPR1209*/
 { TxtServingCell,  (char *)chineseTxtServingCell}, /*MC, SPR1209*/

/*EngMode */
#ifdef TI_PS_FF_EM 
    { TxtServingCellRF, "Serving cell RF"},
    { TxtServingChannel, "Serving Channel"},
    { TxtAmrInfo, "AMR Information"},
    { TxtSNDCPInfo, "SNDCP Information"},
    { TxtUserLevelInfo, "User Level Information"},
    { TxtLLCTracing, "LLC Tracing info"},
    { TxtRlcMacInfo, "RLC/MAC Information"},
    { TxtGMMInfo, "GMM Information"},
    { TxtCallStatus, "Call status"},
    { TxtLayerMessages, "Layer Messages"},
    { TxtWCDMALayerMessages, "WCDMA Layer messages"},
    { Txt3GInfo, "Additional 3G information"},
    { TxtPacketDataParameter, "Packet data parameters"},
#endif


 { TxtNeighbourCell, (char *)chineseTxtNeighbourCell},   /*MC, SPR1209*/
 { TxtLocation, (char *)chineseTxtLocation},    /*MC, SPR1209*/
{ TxtCiphHopDTX, (char *)chineseTxtCiphHopDTX}, /*MC, SPR1209*/
 { TxtConcat, (char *)chineseTxtConcat},
 /*MC SPR 1319, added reference to unicode strings*/
    { TxtColourMenu,		(char *)chineseTxtColourMenu},	
    { TxtColourBlack,		(char *)chineseTxtColourBlack},	
	{ TxtColourBlue,		(char *)chineseTxtColourBlue},
	{ TxtColourCyan,		(char *)chineseTxtColourCyan},
	{ TxtColourGreen, 		(char *)chineseTxtColourGreen}, 
	{ TxtColourMagenta, 	(char *)chineseTxtColourMagenta}, 
	{ TxtColourYellow,		(char *)chineseTxtColourYellow},
	{ TxtColourRed, 		(char *)chineseTxtColourRed}, 
	{ TxtColourWhite,		(char *)chineseTxtColourWhite},	
	{ TxtColourPink,		(char *)chineseTxtColourPink},
	{ TxtColourOrange,		(char *)chineseTxtColourOrange},
	{ TxtColourLightBlue,	(char *)chineseTxtTxtColourLightBlue},
	{ TxtColourLightGreen, 	(char *)chineseTxtColourLightGreen}, 
	{ TxtColourPaleYellow, 	(char *)chineseTxtTxtColourPaleYellow},
	{ TxtColourTransparent,	(char *)chineseTxtColourTransparent},
	{ TxtIdleScreenBackground,		(char *)chineseTxtIdleScreenBackground},
	{ TxtIdleScreenSoftKey,			(char *)chineseTxtIdleScreenSoftKey},
	{ TxtIdleScreenText,			(char *)chineseTxtIdleScreenText},
	{ TxtIdleScreenNetworkName,  	(char *)chineseTxtIdleScreenNetworkName},
	{ TxtGeneralBackground ,  		(char *)chineseTxtGeneralBackground},
	{ TxtMenuSoftKeyForeground , 	(char *)chineseTxtMenuSoftKeyForeground},
	{ TxtMenuSoftKeyBackground ,  	(char *)chineseTxtMenuSoftKeyBackground},
	{ TxtSubMenuHeadingText ,  		(char *)chineseTxtSubMenuHeadingText},
	{ TxtSubMenuHeadingBackground,	(char *)chineseTxtTxtSubMenuHeadingBackground},
	{ TxtSubMenuTextForeground ,  	(char *)chineseSubMenuTextForeground },
	{ TxtSubMenuTextBackground,  	(char *)chineseTxtTxtSubMenuTextBackground},
	{ TxtSubMenuSelectionBar,  		(char *)chineseTxtSubMenuSelectionBar},
	{ TxtPopUpTextForeground,  		(char *)chineseTxtPopUpTextForeground},
	{ TxtPopUpTextBackground,		(char *)chineseTxtPopUpTextBackground},
    { TxtColour,					(char *)chineseTxtColour},
    { TxtLoser,						(char *)chineseTxtLoser},
    { TxtPlayAgain,					(char *)chineseTxtPlayAgain },
    { TxtWinner,					(char *)chineseTxtWinner },		    
     { TxtSelectBook,	(char*)chineseTxtSelectBook },	/* SPR1112 - SH */
    { TxtSIM, 			(char*)chineseTxtSim },
    { TxtMove,			(char*) chineseTxtMove},
    { TxtCopy, 			(char*)chineseTxtCopy},
    { TxtThisEntry, 	(char*)chineseTxtThisEntry},
    { TxtBackground,	(char*)chineseTxtBackground},	/* for selecting backgrounds*/
    { TxtProviderIcon,	(char*)chineseTxtNetworkIcon }, /*AP - 29-01-03 - Add to language table */        
    { TxtIdleScreen,	(char*)chineseTxtIdle},	
	{ TxtMenuScreen,	(char*)chineseTxtMainMenu},	
	{ TxtBgdSquares,	(char*)chineseTxtSquares},
	{ TxtBgdO2,			(char*)chineseTxtO2},
	{ TxtBgdTree,		(char*)chineseTxtTree},
    { TxtBgdTILogo,		(char*)chineseTxtTILogo},
    { TxtBgdSunset,		(char*)chineseTxtSunset},
    { TxtBgdEdinburgh1,	(char*)chineseTxtEdinburgh},
    { TxtBgdEdinburgh2,	(char*)chineseTxtEdinburgh},
    { TxtBgdEdinburgh3,	(char*)chineseTxtEdinburgh},
    { TxtBgdEdinburgh4,	(char*)chineseTxtEdinburgh},
	{ TxtDeflectCall, (char*)chineseTxtDeflectCall}, //SPR 1392 calldeflection
    { TxtDeflectCallTo, (char*)chineseTxtDeflectCallTo}, //SPR 1392  calldeflection
    { TxtDeflect, (char*)chineseTxtDeflect}, //SPR 1392  calldeflection
    { TxtDeflectingCallTo,(char*)chineseTxtDeflect}, //SPR 1392  calldeflection
    { TxtCallBackQuery, (char *) chineseTxtCallBackQuery },  // Marcus: CCBS: 15/11/2002
    { TxtRejected,(char*)    chineseTxtRejected },          // Marcus: CCBS: 15/11/2002
    { TxtActiveCall, (char*) chineseTxtActiveCall},	/* SPR#1352 - SH*/
	{ TxtHeldCall,	(char*) chineseTxtHeldCall			},
	{ TxtActiveTTYCall,(char*) chineseTxtActiveTTYCall},
	{ TxtHeldTTYCall,(char*) chineseTxtHeldTTYCall		},
	{ TxtTTY,		(char*) chineseTxtTTY	},		

#ifdef NEPTUNE_BOARD    /* Mar 27, 2006    REF:ER OMAPS00071798 */
    {TxtAudioMode, "Audio mode"},
    {TxtAudioModeText, "Text mode"},
    {TxtAudioModeVco, "VCO mode"},
    {TxtAudioModeUnknown, "Unknown"},
#endif    
    { TxtBootTime, (char*) chineseTxtBootTime}, /*OMAPS00091029 x0039928(sumanth)*/
/*x0039928 OMAPS00097714 - HCO/VCO implementation - added strings*/
#ifdef FF_TTY_HCO_VCO
    { TxtTTYAll,		(char*) chineseTxtTTYAll},
    { TxtTTYVco,		(char*) chineseTxtTTYVco},
    { TxtTTYHco,		(char*) chineseTxtTTYHco },
#endif    
    { TxtOnNextCall,	(char*) chineseTxtOnNextCall	},
    { TxtAlwaysOn,		(char*) chineseTxtAlwaysOn	},
    { TxtTTYCalling,	(char*) chineseTxtTTYCalling	 },
	{ TxtIncomingTTYCall,	(char*) chineseTxtIncomingTTYCall	},
    { TxtProvisioned, (char *)chineseTxtProvisioned }, // Marcus: Issue 1652: 03/02/2003
    { TxtExecuted, (char *)chineseTxtExecuted }, // Marcus: Issue 1652: 03/02/2003
    { TxtCommand, (char *)chineseTxtCommand }, // NDH : SPR#1869 : 08/04/2003
    { TxtCompleted, (char *)chineseTxtCompleted }, // NDH : SPR#1869 : 08/04/2003
    { TxtMultiparty, (char*)chineseTxtMultiparty }, // AP: Issue 1749: 06/03/03
	{ TxtAttaching, (char*)chineseTxtAttaching},		/* SPR#1983 - SH - Various WAP texts*/
	{ TxtConnection, (char*)chineseTxtConnection},
    { TxtUsing, (char *)chineseTxtUsing },
	{  TxtConnectionSpeed, (char *)chineseTxtConnectionSpeed },
	{  TxtAnalogue, (char *)chineseTxtAnalogue },
	{  TxtISDN9600, (char *)chineseTxtISDN9600 },
	{  TxtISDN14400,  (char *)chineseTxtISDN14400 },
	{ TxtEndWap, (char *)chineseTxtEndWap},
	{ TxtUntitled, (char *)chineseTxtUntitled },
	{ TxtChooseReplacement1, (char *)chineseTxtChooseReplacement1}, /* SPR#2354 */
	{ TxtChooseReplacement2, (char *)chineseTxtChooseReplacement2}, /* SPR#2354 */
	{ TxtDialup,		(char *)chineseTxtDialup }, /* SPR#2324 - SH - Added */
   	{ TxtAdvanced,		(char *)chineseTxtAdvanced },
   	{ TxtGPRSDialupGPRS,	(char *)chineseTxtGPRSDialupGPRS },
   	{ TxtGPRSDialupDialup,	(char *)chineseTxtGPRSDialupDialup },
   	{ TxtProfiles, 		(char *)chineseTxtProfiles },
  	{ TxtNewProfile,	(char *)chineseTxtNewProfile },
  	{ TxtResetProfiles, (char *)chineseTxtResetProfiles },
	{ TxtPasswords,(char *)chineseTxtPasswords}, //SPR16107 localization required. 
	{ TxtMismatch,(char *)chineseTxtMismatch}, //SPR16107 localization required.
	{ TxtDataCounterReset, (char *)chineseTxtDataCounterReset }, /* SPR#2346 - SH */
	{ TxtCallDate, (char *)chineseTxtCallDate }, /* API - 05/09/03 - SPR2357 - Added */
	{ TxtCallTime, (char *)chineseTxtCallTime }, /* API - 05/09/03 - SPR2357 - Added */   	   	
	{ TxtInvalidMessage, (char*)chineseTxtInvalidMessage},/*MC SPR 2530*/
	{ TxtIncompleteMessage,(char*)chineseTxtIncompleteMessage},/*MC SPR 2530*/

	/* SUJATHA MMS Integration */
	{ TxtDownload,      (char *)chineseTxtDownload},
    #if defined (FF_WAP) && defined (FF_GPF_TCPIP)  //added by ellen
	     { TxtShowMessages,  (char *)chineseTxtShowMessages}, //x0035544 07-11-2005
            { TxtPushMessages,  (char *)chineseTxtPushMessages}, //x0035544 07-11-2005
    #endif
	
    #if defined (FF_MMI_MMS) || defined (FF_MMI_EMS) //x0035544 07-11-2005
	  {TxtMMS,(char *)chineseTxtMMS}, 
        {TxtEMS,(char *)chineseTxtEMS},         
        {TxtMMSCreate,(char *)chineseTxtMMSCreate},
        {TxtEMSCreate,(char *)chineseTxtEMSCreate},        
        {TxtMMSInbox,(char *)chineseTxtMMSInbox},
        {TxtMMSUnsent,(char *)chineseTxtMMSUnsent},
        {TxtMMSSent,(char *)chineseTxtMMSSent},//CRR: 25291 - xrashmic 14 Oct 2004
        {TxtMMSSettings,(char *)chineseTxtMMSSettings},
        {TxtRetrieveCondition,(char *)chineseTxtRetrieveCondition}, //x0035544 07-11-2005       
        {TxtMMSInsertPicture,(char *)chineseTxtMMSInsertPicture},
        {TxtMMSInsertSound,(char *)chineseTxtMMSInsertSound},
        {TxtMMSInsertText,(char *)chineseTxtMMSInsertText},
        {TxtMMSInsertSlide,(char *)chineseTxtMMSInsertSlide},
        {TxtMMSDelete,(char *)chineseTxtMMSDelete},
        {TxtMMSPreview,(char *)chineseTxtMMSPreview},
        {TxtMMSDeleteSlide,(char *)chineseTxtMMSDeleteSlide},
        {TxtMMSDeletePicture,(char *)chineseTxtMMSDeletePicture},
        {TxtMMSDeleteSound,(char *)chineseTxtMMSDeleteSound},
        {TxtMMSDeleteText,(char *)chineseTxtMMSDeleteText},
        {TxtMMSInsert,(char *)chineseTxtMMSInsert},
//xrashmic 19 Aug, 2004 Bug: 2, 3, 36 and 42
        {TxtMMSSendSucess,(char *)chineseTxtMMSSendSucess},
        {TxtMMSSendFail,(char *)chineseTxtMMSSendFail},
        {TxtMMSSendng,(char *)chineseTxtMMSSendng},
        {TxtMMSDownloading,(char *)chineseTxtMMSDownloading},
        {TxtNewMMS,(char *)chineseTxtNewMMS},
        {TxtMMSRcvFail,(char *)chineseTxtMMSRcvFail},
        {TxtMMSDots,(char *)chineseTxtMMSDots},
	{TxtExtract,(char *)chineseTxtExtract}, //x0035544 07-11-2005
	 {TxtMMSNotification, (char *)chineseTxtMMSNotification },//x0035544 07-11-2005
        {TxtImmediate,(char *)chineseTxtImmediate },//x0035544 07-11-2005
        {TxtDeferred, (char *)chineseTxtDeferred },//x0035544 07-11-2005
        {TxtMMSRetrieving, (char *)chineseTxtMMSRetrieving },//x0035544 07-11-2005
        {TxtMMSRetrieved, (char *)chineseTxtMMSRetrieved },//x0035544 07-11-2005

        {TxtEMSNew,(char *)chineseTxtEMSNew}, //xrashmic 26 Aug, 2004 MMI-SPR-23931
        {TxtType, (char *)chineseTxtType},
        {TxtObject,(char *)chineseTxtObject},//xrashmic 29 Nov, 2004 MMI-SPR-26161
        {TxtLarge,(char *)chineseTxtLarge},//xrashmic 29 Nov, 2004 MMI-SPR-26161
        //xrashmic 7 Dec, 2004 MMI-SPR-23965
        {TxtMMSInsertPrePicture,(char *)chineseTxtInsertPic},     
        {TxtMMSInsertPreSound, (char *)chineseTxtInsertSound},
        {TxtAllObjects, (char *)chineseTxtAllObjects},
        {TxtExtracted, (char *)chineseTxtExtracted},
        {TxtExtracting,(char *)chineseTxtExtracting},
        {TxtNoObjects, (char *)chineseTxtNoObjects},
        {TxtExtractable, (char *)chineseTxtExtractable},
   #endif//x0035544 07-11-2005
	 { TxtLockAls, (char *)chineseTxtLockAls }, /* NDH : CQ16317 */
	 { TxtUnlockAls, (char *)chineseTxtUnlockAls }, /* NDH : CQ16317 */
	 { TxtAlsLocked, (char *)chineseTxtAlsLocked }, /* NDH : CQ16317 */
	 { TxtAlsUnlocked, (char *)chineseTxtAlsUnlocked }, /* NDH : CQ16317 */
	 { TxtLimService, (char *)chineseTxtLimService}, /* SPR12653 */  //localization required.

	// Apr 06, 2005	REF: ENH 30011	
//Strings for Camera, Mp3 and midi test application are 
//under the respective flags.
//Nov 14, 2005    REF: OMAPS00044445 xdeepadh

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #if defined */

// #if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
	{ TxtPlayerMonoChannel, (char *)chineseTxtPlayerMonoChannel}, /* NDH : CQ16317 */
	{ TxtPlayerStereoChannel, (char *)chineseTxtPlayerStereoChannel}, /* NDH : CQ16317 */	
	{ TxtPlayerSetChannels, (char *)chineseTxtPlayerSetChannels}, /* NDH : CQ16317 */		
	{ TxtPlayerPlay, (char *)chineseTxtPlayerPlay}, /* NDH : CQ16317 */	
	{ TxtPlayerTest, (char *)chineseTxtPlayerTest}, /* NDH : CQ16317 */	
	{ TxtPlayerFileSelected, (char *)chineseTxtPlayerFileSelected}, /* NDH : CQ16317 */		
	{ TxtPlayerSelectFile, (char *)chineseTxtPlayerSelectFile}, /* NDH : CQ16317 */		
	{ TxtPlayerPause, (char *)chineseTxtPlayerPause},
	{ TxtPlayerResume, (char *)chineseTxtPlayerResume }, /* NDH : CQ16317 */
	{ TxtPlayerStop, (char *)chineseTxtPlayerStop }, /* NDH : CQ16317 */
	{ TxtPlayerPlaying, (char *)chineseTxtPlayerPlaying }, /* NDH : CQ16317 */
	{ TxtPlayerForward, (char *)chineseTxtPlayerForward },  /* OMAPS00070659  x0039928  */
	{ TxtPlayerRewind, (char *)chineseTxtPlayerRewind },
	{ TxtPlayerForwarding, (char *)chineseTxtPlayerForwarding },
	{ TxtPlayerRewinding, (char *)chineseTxtPlayerRewinding },  /* OMAPS00070659  x0039928  */
//	Jul 18, 2005    REF: SPR 31695   xdeepadh	
	{ TxtPlayerFileNotLoaded, (char *)chineseTxtPlayerNoFile},
	{ TxtPlayerOptionNotImplemented, (char*)chineseTxtPlayerOptionNotImplemented}, 
// #endif //FF_MMI_TEST_MP3

/* Nov 22, 2007 DR: OMAPS00152315 x0080701(Bharat) -- Commented #endif */

//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)	
	{ TxtCameraTest, (char *)chineseTxtCameraTest}, /* NDH : CQ16317 */
	{ TxtViewFinder, (char *)chineseTxtViewFinder}, /* NDH : CQ16317 */	
	{ TxtSnapshot, (char *)chineseTxtSnapshot}, /* NDH : CQ16317 */		
	{ TxtSaveSnapshot, (char *)chineseTxtSaveSnapshot}, /* NDH : CQ16317 */	
	{ TxtSnapshotSaved, (char *)chineseTxtSnapshotSaved}, /* NDH : CQ16317 */	
	{ TxtSoftSnap, (char *)chineseTxtSoftSnap}, /* NDH : CQ16317 */		
	{ TxtSoftSnapSave, (char *)chineseTxtSoftSnapSave}, /* NDH : CQ16317 */		
	{ TxtSnapSaving, (char *)chineseTxtSnapSaving},
	{ TxtQuality, (char *)chineseTxtQuality},
	{ TxtEconomy, (char *)chineseTxtEconomy},
	{ TxtStandard, (char *)chineseTxtStandard},
	{ TxtColor, (char *)chineseTxtColor},
	{ TxtSepia, (char *)chineseTxtSepia},
	{ TxtBlackandWhite, (char *)chineseTxtB_W},
	{ TxtNegative, (char *)chineseTxtNegative},
	{ TxtFilename, (char *)chineseTxtFilename},
	{ TxtDigiZoom, (char *)chineseTxtDigiZoom},
	{ TxtFrame, (char *)chineseTxtFrame},	
	{ TxtFrame1, (char *)chineseTxtFrame1},
	{ TxtFrame2, (char *)chineseTxtFrame2},	
	{ TxtNoFrame, (char *)chineseTxtNoFrame},	
	{ TxtShootingMode, (char *)chineseTxtShootingMode},
	{ TxtSingleshot, (char *)chineseTxtSingleShot},
	{ Txt2Shots, (char *)chineseTxt2Shots},
	{ Txt4Shots, (char *)chineseTxt4Shots},
	{ Txt8Shots, (char *)chineseTxt8Shots},
#endif //FF_MMI_TEST_CAMERA
// Apr 05, 2005    REF: ENH 29994 xdeepadh
//Midi Test Application Strings were put under the flag FF_MMI_TEST_MIDI
#ifdef FF_MMI_TEST_MIDI
	{ TxtMidiTest, (char *)chineseTxtMidiTest }, /* NDH : CQ16317 */
	{ TxtMidiChangeConfig, (char *)chineseTxtMidiChangeConfig }, /* NDH : CQ16317 */
	{ TxtMidiPlay, (char *)chineseTxtMidiPlay }, /* NDH : CQ16317 */
	{ TxtMidiPlayAllFiles, (char *)chineseTxtMidiPlayAllFiles }, /* NDH : CQ16317 */
	{ TxtMidiExit, (char *)chineseTxtMidiExit }, /* NDH : CQ16317 */
	{ TxtChConfigFile, (char *)chineseTxtChConfigFile }, /* NDH : CQ16317 */
	{ TxtChConfigVoiceLimit, (char *)chineseTxtChConfigVoiceLimit }, /* NDH : CQ16317 */
	{ TxtChConfigLoop, (char *)chineseTxtChConfigLoop }, /* NDH : CQ16317 */
	{ TxtChConfigChannels, (char *)chineseTxtChConfigChannels }, /* NDH : CQ16317 */
	{ TxtChConfigBankLocation, (char *)chineseTxtMidiBankLocation }, /* NDH : CQ16317 */
	{ TxtMidiEnterVoiceNumber, (char *)chineseTxtMidiEnterVoiceNumber }, /* NDH : CQ16317 */
	{ TxtMidiSelectFile, (char *)chineseTxtMidiSelectFile }, /* NDH : CQ16317 */
	{ TxtMidiOptionNotImplemented, (char *)chineseTxtMidiOptionNotImplemented }, /* NDH : CQ16317 */
	{ TxtMidiLoopOn, (char *)chineseTxtMidiLoopOn }, /* NDH : CQ16317 */
	{ TxtMidiLoopOff, (char *)chineseTxtMidiLoopOff }, /* NDH : CQ16317 */
	{ TxtMidiChannels, (char *)chineseTxtMidiChannels }, /* NDH : CQ16317 */
	{ TxtMidiMonoChannel, (char *)chineseTxtMidiMonoChannel }, /* NDH : CQ16317 */
	{ TxtMidiStereoChannel, (char *)chineseTxtMidiStereoChannel}, /* NDH : CQ16317 */
	{ TxtMidiBankLocation, (char *)chineseTxtMidiBankLocation}, //x0035544 07-11-2005
	{ TxtMidiFastBankLocation, (char *)chineseTxtMidiFastBankLocation }, /* NDH : CQ16317 */
	{ TxtMidiSlowBankLocation, (char *)chineseTxtMidiSlowBankLocation }, /* NDH : CQ16317 */
	{ TxtMidiFileTitle, (char *)chineseTxtMidiFileTitle }, /* NDH : CQ16317 */
	{ TxtMidiFileNotLoaded, (char *)chineseTxtMidiFileNotLoaded }, /* NDH : CQ16317 */
	{ TxtMidiFileSelected, (char *)chineseTxtMidiFileSelected }, /* NDH : CQ16317 */
	{ TxtChConfigAudioOutput, (char *)chineseTxtChConfigAudioOutput}, /* NDH : CQ16317 */
	{ TxtMidiSpeaker, (char *)chineseTxtMidiSpeaker}, /* NDH : CQ16317 */
	{ TxtMidiHeadset, (char *)chineseTxtMidiHeadset}, /* NDH : CQ16317 */
//x0pleela 30 Jan, 2007  DR: OMAPS00108892
//Adding the following strings to display error message
//when loop on is enabled while playing all files	
	{ TxtLoopOn, (char *)chineseTxtLoopOn},
	{ TxtCantPlayAllFiles, (char *)chineseTxtMidiHeadset},
	{ TxtCantBeActivated, (char*)chineseTxtCantBeActivated},
#endif//#ifdef FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
	/*
	** Start of Bluetooth Related Strings
	*/
    	{TxtBluetooth, (char *)chineseTxtBluetooth },
    	{TxtBluetoothEnable, (char *)chineseTxtBluetoothEnable },
    	{TxtBluetoothDisable, (char *)chineseTxtBluetoothDisable },
    	{TxtBtEnterDeviceName, (char *)chineseTxtBtEnterDeviceName },
    	{TxtBtEnterPassKey, (char *)chineseTxtBtEnterPassKey },
    	{TxtBtPassKey, (char *)chineseTxtBtPassKey },
    	{TxtBtTimeout, (char *)chineseTxtBtTimeout },
    	{TxtBtPairing, (char *)chineseTxtBtPairing },
    	{TxtBtPaired, (char *)chineseTxtBtPaired },
    	{TxtBtAuthorize, (char *)chineseTxtBtAuthorize },
    	{TxtBtUnAuthorize, (char *)chineseTxtBtUnAuthorize },
	{TxtBtInfo, (char *)chineseTxtBtInfo },
	{TxtBtYesOnce, (char *)chineseTxtBtYesOnce },
	{TxtBtYesAlways, (char *)chineseTxtBtYesAlways },
	{TxtBtPairDevice, (char *)chineseTxtBtPairDevice },
	{TxtBtChangePassKey, (char *)chineseTxtBtChangePassKey },
	{TxtBtShowServices, (char *)chineseTxtBtShowServices },
	{TxtBtAddToKnown,	 (char *)chineseTxtBtAddToKnown },
	{TxtBtRemoveDevice, (char *)chineseTxtBtRemoveDevice },
	{TxtBtDeviceName, (char *)chineseTxtBtDeviceName },
	{TxtBtSet, (char *)chineseTxtBtSet },
	{TxtBtServices, (char *)chineseTxtBtServices },
	{TxtBtNoServices, (char *)chineseTxtBtNoServices },
	{TxtBtSupported, (char *)chineseTxtBtSupported },
	{TxtBtSerialPort, (char *)chineseTxtBtSerialPort },
	{TxtBtDialUpNetworking, (char *)chineseTxtBtDialUpNetworking },
	{TxtBtHeadSet, (char *)chineseTxtBtHeadSet },
	{TxtBtHandsFree, (char *)chineseTxtBtHandsFree },
	{TxtBtObjectPush, (char *)chineseTxtBtObjectPush },
	{TxtBtFileTransfer, (char *)chineseTxtBtFileTransfer },
	{TxtBtLanAccess, (char *)chineseTxtBtLanAccess },
	{TxtBtCordlessTelephony, (char *)chineseTxtBtCordlessTelephony },
	{TxtBtIntercom, (char *)chineseTxtBtIntercom },
	{TxtBtUnknownService, (char *)chineseTxtBtUnknownService },
	{TxtBtRequiresAuthorizationForService, (char *)chineseTxtBtRequiresAuthorizationForService },
	{TxtBtDevices, (char *)chineseTxtBtDevices },
	{TxtBtDevice, (char *)chineseTxtBtDevice },
	{TxtBtHidden, (char *)chineseTxtBtHidden },
	{TxtBtTimed, (char *)chineseTxtBtTimed },
	{TxtBtKnownDevices, (char *)chineseTxtBtKnownDevices },
	{TxtBtDiscoverable, (char *)chineseTxtBtDiscoverable },
	{TxtBtNoDevices, (char *)chineseTxtBtNoDevices },
	{TxtBtFound, (char *)chineseTxtBtFound },
    	{TxtBtSetLocalName, (char *)chineseTxtBtSetLocalName },
	{TxtBtClearStoredDevices, (char *)chineseTxtBtClearStoredDevices },
    	/*
    	** End of Bluetooth Related Strings
    	*/
#endif
	{ TxtFDNName, (char *)chineseTxtFDNName }, //MMI SPR 18555 	 
	{ TxtAllDivert, (char *)chineseTxtAllDivert},/* SPR13614 */  //localization required.
	{ TxtNoReachDivert, (char *)chineseTxtNoReachDivert},/* SPR13614 */  //localization required.
	{ TxtBusyDivert, (char *)chineseTxtBusyDivert},/* SPR13614 */  //localization required.
	{ TxtNoAnsDivert, (char *)chineseTxtNoAnsDivert},/* SPR13614 */  //localization required.
	{ TxtCondDivert, (char *)chineseTxtCondDivert},/* SPR13614 */  //localization required.
	{ TxtAllCallsDivert, (char *)chineseTxtAllCallsDivert},/* SPR13614 */  //localization required.
	{ TxtInvalidInput, (char *)chineseTxtInvalidInput}, //SPR21547 localization required.
	{ TxtFDNListFull,(char *)chineseTxtFDNListFull},//SPR 31710 x0021308:RamG.
        
//Nov 29, 2004    REF: CRR 25051 xkundadu
//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//Fix: Added these Chinees strings to display the various speaker volume 
//      level strings. 
	{TxtLevel1, (char *)chineesTxtLevel1},
	{TxtLevel2, (char *)chineesTxtLevel2},
	{TxtLevel3, (char *)chineesTxtLevel3},
	{TxtLevel4, (char *)chineesTxtLevel4},
	{TxtLevel5, (char *)chineesTxtLevel5},
	 {TxtImage,(char *)chineesTxtImage}, //xrashmic 28 Jan, 2005 MMI-SPR-28166
       { TxtSmsWaiting, (char*)chineesTxtSMSWait}, //x0018858 24 Mar, 2005 MMI-FIX-11321        
/* Power management Menu*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
        { Txt10Second, (char *)chineesTxt10Second },
        { Txt15Second, (char *)chineesTxt15Second },
        { Txt20Second, (char *)chineesTxt20Sec },
#endif
#endif
#ifdef FF_MMI_MULTIMEDIA
        /* Multimedia related strings. */
	{TxtMultimediaApp, (char *)chineesTxtMultimediaApp},   
	{TxtConfigurationParameters, (char *)chineesTxtConfigurationParameters},   
	{TxtAudioVideoPlayback, (char *)chineesTxtAudioVideoPlayback},   	
	{TxtMidiPlayback, (char *)chineesTxtMidiPlayback},   
	{TxtImageViewer, (char *)chineesTxtImageViewer},   	
	{TxtCameraCapture, (char *)chineesTxtCameraCapture},   	
	{TxtVideoRecording, (char *)chineesTxtAudioVideoRecording},
	{TxtAudioRecording, (char *)chineesTxtAudioVideoRecording},
	{TxtPlayRecording, (char *)chineesTxtAudioVideoRecording},
	{TxtFileListType, (char *)chineesTxtFileListType},   		
	{TxtNoFileAvail, (char *)chineesTxtNoFilesAvail},   			
	/* Configuration Parameter. */
	{TxtOutputScreenSize, (char *)chineesTxtOutputScreenSize},   					
	{TxtAudioEncodingFormat, (char *)chineesTxtAudioEncodingFormat},   					
	{TxtVideoEncodingFormat, (char *)chineesTxtVideoEncodingFormat},   
	{TxtVideoCaptureSize, (char *)chineesTxtVideoCaptureSize},  				
	{TxtVideoFrameRateSelection, (char *)chineesTxtVideoFrameRateSelection},  					
	{TxtAudioBitRate, (char *)chineesTxtAudioBitRate},   					
	{TxtVideoBitRate, (char *)chineesTxtVideoBitRate},   					
	{TxtMidiPlayerSelect, (char *)chineesTxtMidiPlayerSelect},   						
        /* Screen Mode. */
	{TxtHalfScreenMode,(char *) chineesTxtHalfScreenMode },	
	{TxtFullScreenMode, (char *) chineesTxtFullScreenMode},		
	/* Audio Format */
	{TxtAAC, (char *) chineesTxtAAC },		
	{TxtAMR,(char *) chineesTxtAMR },		
	{TxtPCM,(char *) chineesTxtPCM },		
	{TxtMP3,(char *) chineesTxtMP3 },			
	/* Video Format */
	{TxtMpeg4, (char *) chineesTxtMpeg4 },		
	{TxtH263, (char *) chineesTxtH263 },		
        /* Frame Size */
	{TxtQcif, (char *) chineesTxtQcif },		
	{TxtCif, (char *) chineesTxtCif  },		
       /* Frame Rate */
	{Txt5fps, (char *) chineesTxt5fps },    			
	{Txt10fps, (char *) chineesTxt10fps }, 			
	{Txt15fps, (char *) chineesTxt15fps },  			
	{Txt20fps, (char *) chineesTxt20fps },
	{Txt25fps, (char *) chineesTxt25fps },     			
	{Txt30fps, (char *) chineesTxt30fps },
        /* Bit Rate */
	{TxtAACBitRate, (char *) chineesTxtAACBitRate  },		
	{AMRNBBitRate, (char *) chineesAMRNBBitRate  },		
        /* AAC Bit Rate */
	{Txt48Kbps, (char *) chineesTxt48Kbps },     			
	{Txt66p15Kbps,(char *) chineesTxt66p15Kbps }, 			
	{Txt72Kbps, (char *) chineesTxt72Kbps }, 				
	{Txt96pKbps, (char *) chineesTxt96pKbps },			
	{Txt132p3Kbps, (char *) chineesTxt132p3Kbps }, 
	{Txt144Kbps, (char *) chineesTxt144Kbps },    			
	{Txt192Kbps,(char *) chineesTxt192Kbps}, 
	{Txt264p6Kbps, (char *) chineesTxt264p6Kbps },     			
	{Txt288Kbps, (char *) chineesTxt288Kbps }, 
        /* AMR Bit Rate  */
       {Txt1p80Kbps, (char *) chineesTxt1p80Kbps }, 
	{Txt4p75Kbps, (char *) chineesTxt4p75Kbps },     			
	{Txt5p15Kbps, (char *) chineesTxt5p15Kbps }, 			
	{Txt5p9Kbps, (char *) chineesTxt5p9Kbps },  			
	{Txt6p7Kbps, (char *) chineesTxt6p7Kbps }, 
	{Txt7p4Kbps, (char *) chineesTxt7p4Kbps },   			
	{Txt7p95Kbps, (char *) chineesTxt7p95Kbps }, 
	{Txt10p2Kbps, (char *) chineesTxt10p2Kbps },   			
	{Txt12p2Kbps, (char *) chineesTxt12p2Kbps },
       /* Video Bit Rate */
	{Txt64Kbps, (char *) chineesTxt64Kbps },
	{Txt128Kbps, (char *) chineesTxt128Kbps },    			
	{Txt384Kbps, (char *) chineesTxt384Kbps}, 
	/* Softkey display string in Multimedia Application.*/
	{TxtPauseKey, (char *) chineesTxtPauseKey },
	{TxtResumeKey, (char *) chineesTxtResumeKey },
	{TxtEncodeKey, (char *) chineesTxtEncodeKey }, 	
	{TxtSoftIVT, (char *) chineesTxtSoftIVT },	
	{TxtPictureDisp, (char *) chineesTxtPictureDisp },	
	{TxtSetParam, (char *) chineesTxtSetParam},       	
	/* Audio Path Configuration Parameter. */
	{TxtAudioPath, (char *) chineesTxtAudioPath }, 
	{TxtHeadset, (char *) chineesTxtHeadset},				 
	{TxtHeadphone, (char *) chineesTxtHeadphone },	  
	/* Audio / Video Recording related strings.*/
	{TxtSoftStart,(char *) chineesTxtSoftStart }, 						
	{TxtAudvidRecord,(char *) chineesTxtAudvidRecord},  
	{TxtQQVga, (char *) chineesTxtQQVga},                                    
	{TxtEncdTo, (char *) chineesTxtEncdTo},         
	{TxtJPEGFile, (char *) chineesTxtJPEGFile },        
	/* File Management related strings. */
	{TxtFileMgmt,(char *) chineesTxtFileMgmt },    
	{TxtFileOprns, (char *) chineesTxtFileOprns },	
	{TxtFileOprnsTest, (char *) chineesTxtFileOprnsTest  },	
	{TxtFFS, (char *) chineesTxtFFS},	
	{TxtMMC, (char *) chineesTxtMMC  },	
	/* Begin: x0045876, 02-Aug-2006 (UCS2 support - Not defined) */
	/*

	{TxtSDC, (char *) chineesTxtSDC},	
	{TxtTestCase1, (char *) chineesTxtTestCase1 },	
	{TxtTestCase2, (char *) chineesTxtTestCase2 },	
 	{TxtTestCase3, (char *) chineesTxtTestCase3 },	
	*/
 	/* End: x0045876, 02-Aug-2006 (UCS2 support - Not defined) */
 	{TxtOperSuccess, (char *) chineesTxtOperSuccess },
	{TxtOperFailed, (char *) chineesTxtOperFailed  },
	{TxtNotYetDone, (char *) chineesTxtNotYetDone },
	/*For FTP Application */
	{ TxtFTPApp,(char *) chineesTxtFTPApp  },
	{ TxtPut, (char *) chineesTxtPut },
	{ TxtGet, (char *) chineesTxtGet },
	/* Configuration Parameter - Volume Configuration and Preferred Storage.*/
	{ TxtVolumeConfig, (char *) chineesTxtVolumeConfig},
	{ TxtPreferredStorage, (char *) chineesTxtPreferredStorage},	
       { TxtAudioDecodeConfig, (char *) chineesTxtAudioDecodeConfig},
       { TxtAudioEncodeConfig, (char *) chineesTxtAudioEncodeConfig},
       { TxtVideoEncodeConfig, (char *) chineesTxtVideoEncodeConfig},	
	{ TxtSpeaker, (char *) chineesTxtSpeaker},	 // Configuration Parameter.
	 { TxtEnterPath, (char *) chineesTxtEnterPath},
       {TxtInvalidPath, (char *) chineesTxtInvalidPath},
	{ TxtRename, (char *) chineesTxtRename },
       { TxtCreateFolder, (char *) chineesTxtCreateFolder },
       { TxtEnterFileName, (char *) chineesTxtEnterFileName },
	{ TxtFolderNotEmpty, (char *) chineesTxtFolderNotEmpty },
       { TxtGarbageCollect, (char *) chineseTxtGarbageCollect },
	/* x0045876, 02-Aug-2006 (UCS2 Support - Parameter undefined) */
	/*
       { TxtOperationList, (char *) chineesTxtOperationList  },
       */
       { TxtDestination, (char *) chineesTxtDestination },
	{TxtDummyStr, (char *) chineesTxtDummyStr }, 
	{TxtDummyStr1, (char *) chineesTxtDummyStr1 }, 
	{Txt8kRate, (char *) chineesTxt8kRate}, 	
	{Txt11kRate, (char *) chineesTxt11kRate }, 
	{Txt12kRate, (char *)chineesTxt12kRate},       /*OMAPS00081264 - 12k Menu addition */
	{Txt16kRate, (char *) chineesTxt16kRate }, 
	{Txt22kRate, (char *) chineesTxt22kRate}, 
       {Txt24kRate, (char *)chineesTxt24kRate},     /*OMAPS00081264 - 24k Menu addition */
	{Txt32kRate, (char *) chineesTxt32kRate }, 	
	{Txt44kRate, (char *) chineesTxt44kRate}, 
	{Txt48kRate, (char *) chineesTxt48kRate}, 
	{TxtPcmRate, (char *) chineesTxtPcmRate}, 
	{TxtPcmChannelMode, (char *) chineesTxtPcmChannelMode }, 
       /* Image Quality Setting */
       {TxtImgCaptureQuality, (char *) chineesTxtImgCaptureQuality}, 
       {Txt0Set, (char *) chineesTxt0Set}, 
       {Txt10Set, (char *) chineesTxt10Set}, 
       {Txt20Set, (char *) chineesTxt20Set}, 
       {Txt30Set, (char *) chineesTxt30Set}, 
       {Txt40Set, (char *) chineesTxt40Set}, 
       {Txt50Set, (char *) chineesTxt50Set}, 
       {Txt60Set, (char *) chineesTxt60Set}, 
       {Txt70Set, (char *) chineesTxt70Set}, 
       {Txt80Set, (char *) chineesTxt80Set}, 
       {Txt90Set, (char *) chineesTxt90Set}, 
       {Txt100Set, (char *) chineesTxt100Set}, 
       {TxtStreoMode, (char *) chineesTxtStreoMode},
       {TxtMonoMode, (char *) chineesTxtMonoMode}, 
       {TxtVideoPlayOption, (char *) chineesTxtVideoPlayOption},
/* x0045876 - 02-Aug-2006 (UCS2 support - Changed from chineesTxtAudioVideoPlayback to chineesTxtAudioVideoPlay) */
       {TxtAudioVideoPlay, (char *) chineesTxtAudioVideoPlay},
       {TxtVideoPlay, (char *) chineesTxtVideoPlay},      
        /* Video Recording Duration Specification */
       {TxtVideoRecDuration, (char *)chineesTxtVideoRecDuration},      
       {Txt10sec, (char *)chineesTxt10sec},
       {Txt15sec, (char *)chineesTxt15sec},
       {Txt20sec, (char *)chineesTxt20sec},
       {TxtNonStop, (char *)chineesTxtNonStop},       
/* Video Only Option - x0043641 */
       {TxtVideoRecordOption, (char *) chineesTxtVideoRecordOption},
       {TxtAudioVideoRecord, (char *) chineesTxtAudioVideoRecord},
       {TxtVideoRecord, (char *) chineesTxtVideoRecord},
#endif
// 	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005	
#ifdef FF_MMI_AUDIO_PROFILE
	{TxtLoudspeaker, (char *)chineesTxtLoudspeaker},
	{TxtHandheld, (char *)chineesTxtHandheld},
	{TxtCarkit, (char *)chineesTxtCarkit},
	{TxtHeadset, (char *)chineesTxtHeadset},
	{TxtInserted, (char*)chineesTxtInserted},
	{TxtRemoved, (char*)chineesTxtRemoved},
#endif	
#ifdef FF_MMI_FILE_VIEWER
	{TxtFileNotLoaded,(char *)chineseTxtFileNotLoaded},
	{ TxtFileViewer, (char *)chineseTxtFileViewer}, //May 02, 2005 REF:Camera App xdeepadh
	{ TxtRename, (char *)chineseTxtRename},
	{TxtDeleteConfirm,(char *)chineseTxtDeleteConfirm},	
	{TxtFileTooBig,(char *)chineseTxtFileTooBig},
	{TxtFileExists,(char *)chineseTxtFileExists},//Jan 03, 2006    REF: SPR OMAPS00060036  xdeepadh translation needed
	{TxtWrongFilename,(char *)chineseTxtWrongFile},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,(char *)chineseTxtNoExtension},//Aug 31, 2005    REF: SPR 34050 xdeepadh	
	#endif // FF_MMI_FILE_VIEWER

//Aug 22, 2005    REF: ENH 31154 xdeepadh
#if defined(FF_MMI_TEST_CAMERA) ||defined(FF_MMI_CAMERA_APP)
	{ TxtRotate,(char *)chineseTxtRotate}, 
	{ TxtRotate90Degree, (char *)chineseTxtRotate90Degree}, 
	{ TxtRotate180Degree, (char *)chineseTxtRotate180Degree}, 
	{ TxtRotate270Degree, (char *)chineseTxtRotate270Degree}, 
	{ TxtZoom, (char *)chineseTxtZoom},
	{ TxtZoomIn, (char *)chineseTxtZoomIn}, 
	{ TxtZoomOut, (char *)chineseTxtZoomOut}, 
	{ TxtZoomInMax, (char *)chineseTxtZoomInMax}, 
	{ TxtZoomOutMax, (char *)chineseTxtZoomOutMax}, 
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
{TxtUSB, "USB"},
	{TxtUSBEnumeration, (char *)chineseTxtUSBEnumeration},
	 {  TxtUSBMode, (char *)chineseTxtUSBMode},
 	 {  TxtContinue, (char *)chineseTxtContinue},	
	{ TxtUSBDisconnect ,(char *)chineseTxtUSBDisconnect},
	{TxtUSBMS,(char *)chineseTxtUSBMS},
	{TxtUSBTrace,(char *)chineseTxtUSBTrace},
	{TxtUSBFax,(char *)chineseTxtUSBFax}, 
{TxtPSShutdown,(char *)chineseTxtPSShutdown},
{TxtNoPSShutdown,(char *)chineseTxtNoPSShutdown},
//xashmic 27 Sep 2006, OMAPS00096389 
{TxtUSBFaxnTrace,(char *)chineseTxtUSBFaxnTrace},
{TxtUSBMSnTrace,(char *)chineseTxtUSBMSnTrace},
{TxtUSBFaxnMS,(char *)chineseTxtUSBFaxnMS},
{TxtUSBMSPorts,(char *)chineseTxtUSBMSPorts},
{TxtPS,(char *)chineseTxtPS},
{TxtPopupMenu,(char *)chineseTxtPopupMenu},

#endif

    // x0021334 10 May, 2006 DR: OMAPS00075379
    { TxtPending, (char *)chineseTxtPending},

    // x0021334 02 June, 2006 DR: OMAPS00078005 
#ifdef FF_TIMEZONE
	{ TxtTimeUpdate, (char *)chineseTxtTimeUpdate},
	{ TxtTimeUpdateConfirm, (char *)chineseTxtTimeUpdateConfirm},	
	{ TxtNitzTimeUpdate, (char *)chineseTxtNitzTimeUpdate},
#endif
	//x0pleela 30 Aug, 2006 DR: OMAPS00091250
	{ TxtNoMEPD, (char *)chineseTxtNoMEPD},

//x0pleela 24 Aug, 2006  DR:OMAPS00083503
//Adding new strings to display if the characters are UCS2 in USSD transactions
#ifdef MMI_LITE
	{ TxtUSSDMsgRxd, (char *)chineseTxtUSSDMsgRxd},
	{ TxtFontNotSupported, (char *)chineseTxtFontNotSupported},	
#endif
//x0pleela 05 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	{ TxtPhoneLock, (char *)chineseTxtPhoneLock},
	{ TxtPhLockEnable, (char *)chineseTxtPhLockEnable},		
	{ TxtPhLockAutomatic, (char *)chineseTxtPhLockAutomatic},
	{ TxtPhLockAutoOn, (char *)chineseTxtPhLockAutoOn},		
	{ TxtPhLockAutoOff, (char *)chineseTxtPhLockAutoOff},
	{ TxtPhLockChangeCode, (char *)chineseTxtPhLockChangeCode},		
	{ TxtEnterPhUnlockCode, (char *)chineseTxtEnterPhUnlockCode},
	{ TxtEnterOldUnlockCode, (char *)chineseTxtEnterOldUnlockCode},		
	{ TxtEnterNewUnlockCode, (char *)chineseTxtEnterNewUnlockCode},
	{ TxtAlreadyEnabled, (char *)chineseTxtAlreadyEnabled},		
	{ TxtAlreadyDisabled, (char *)chineseTxtAlreadyDisabled},
	{ TxtAutoLockDisable, (char *)chineseTxtAutoLockDisable},
	{ TxtAutoPhone, (char *)chineseTxtAutoPhone},
	{ TxtSuccess, (char *)chineseTxtSuccess},
	{TxtNotLocked, (char *)chineseTxtNotLocked},	
#endif /*FF_PHONE_LOCK*/

/*OMAPS00098881 (Removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
	{TxtScreenUpdate,"Screen Update"},
#endif
//Mar 09 2007 DRT :OMAPS00118881 x0061088(Prachi)
#ifdef FF_MMI_CAMERA_APP
	{TxtNotEnough,(char *)chineseTxtCamNotEnough},
	{TxtCamMemory,(char *)chineseTxtCamMemory},
#endif
	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
	{ TxtFwdIncomingTTYCall, (char *)chineseTxtFwdIncomingTTYCall},	/* String for Fwd Incoming TTY call */
	
	{ TxtFwdIncomingCall, (char *)chineseTxtFwdIncomingCall}, /* String for Fwd Incoming  call */
	//06 July, 2007 DR: OMAPS00137334 x0062172 Syed
	{TxtConferenceFull, (char *)chineseTxtConferenceFull}, 	 /* String for Conference Full notification  */
#ifdef FF_MMI_FILEMANAGER 
	/* File Management related strings */
	{TxtFileMgmt,(char *)chineseTxtFileMgmt},  
	{TxtFileList,(char *)chineseTxtFileList},
	{TxtFFS,(char *)chineseTxtFFS },	
	{TxtNORMS,(char *)chineseTxtNORMS },
	{TxtNAND,(char *)chineseTxtNAND},	
	{TxtTflash,(char *)chineseTxtTflash},	
	{TxtOpen,(char *)chineseTxtOpen},
	{TxtFormat,(char *)chineseTxtFormat},
	{TxtProperties,(char *)chineseTxtProperties},
	{TxtImageFiles,(char *)chineseTxtImageFiles},
	{TxtAudioFiles,(char *)chineseTxtAudioFiles},
	{TxtFile,(char *)chineseTxtFile},
	{TxtDirectory,(char *)chineseTxtDirectory},
	{TxtFreeSpace,(char *)chineseTxtFreeSpace},
	{TxtUsedSpace,(char *)chineseTxtUsedSpace},
	{TxtThumbnail,(char *)chineseTxtThumbnail},
	{TxtExists,(char *)chineseTxtExists},
	{TxtCreateDir,(char *)chineseTxtCreateDir},
	{TxtCreated,(char *)chineseTxtCreated},
	{TxtNotEmpty,(char *)chineseTxtNotEmpty},
	{TxtMemorystat,(char *)chineseTxtMemorystat},
	{TxtSourceDest,(char *)chineseTxtSourceDest},
	{TxtSame,(char *)chineseTxtSame},
	{TxtAudNotAllowed,(char *)chineseTxtAudNotAllowed},
	{TxtAudAudioError,(char *)chineseTxtAudAudioError},
	{TxtAudDenied,(char *)chineseTxtAudDenied},
	{TxtAudPlayerError,(char *)chineseTxtAudPlayerError},
	{TxtAudFFSError,(char *)chineseTxtAudFFSError},
	{TxtAudMessagingError,(char *)chineseTxtAudMessagingError},
	{TxtAudNotReady,(char *)chineseTxtAudNotReady},
	{TxtAudInvalidParam,(char *)chineseTxtAudInvalidParam},
	{TxtAudInternalError,(char *)chineseTxtAudInternalError},
	{TxtAudMemoryError,(char *)chineseTxtAudMemoryError},
	{TxtAudNotSupported,(char *)chineseTxtAudNotSupported},	
	{TxtCopying,(char *)chineseTxtCopying},
	{TxtFileNotLoaded,(char *)chineseTxtFileNotLoaded},
	{TxtFileViewer,(char *)chineseTxtFileViewer},
	{TxtRename,(char *)chineseTxtRename},
	{TxtDeleteConfirm,(char *)chineseTxtDeleteConfirm},
	{TxtFileTooBig,(char *)chineseTxtFileTooBig},
	{TxtFileExists,(char *)chineseTxtFileExists},//Jan 03,2006    REF: SPR OMAPS00060036  xdeepadh .
	{TxtWrongFilename,(char *)chineseTxtWrongFilename},//Aug 31,2005    REF: SPR 34050 xdeepadh	
	{TxtNoExtension,(char *)chineseTxtNoExtension},//Aug 31,2005    REF: SPR 34050 xdeepadh	
	{TxtStorageDevice,(char *)chineseTxtStorageDevice},
	{TxtPBar,(char *)chineseTxtPBar},
#endif

//Mar 22 2007 ER:OMAPS00113882 x0061088 (Prachi)  
    {TxtTo,(char *)chineseTxtTo},

/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
#ifdef FF_MMI_CAMERA_APP
    {TxtAutoSave,(char *)chineseTxtAutoSave},
    {TxtAutoSaveOn,(char *)chineseTxtAutoSaveOn},
    {TxtAutoSaveOff,(char *)chineseTxtAutoSaveOff},
    {TxtSoftSave,(char *)chineseTxtSoftSave},
    {TxtSoftCancel,(char *)chineseTxtSoftCancel},
#endif /* FF_MMI_CAMERA_APP */
#if CAM_SENSOR == 1
    {TxtCamMenuResolution,(char *)chineseTxtCamResolution},
    {TxtCamVga,(char *)chineseTxtCamVga},
    {TxtCamQcif,(char *)chineseTxtCamQcif},
    {TxtCamSxga,(char *)chineseTxtCamSxga},
    {TxtCamNot,(char *)chineseTxtCamNot},
    {TxtCamSupported,(char *)chineseTxtCamSupported},
#endif 
	
	{ 0, 0 }
};

#endif


 char* MelodyTable[]={

		"The_Ale_is_dear",
		"Danza_de_Astureses",
		"rising_chromatic",
		"rising_cscale",
		"MELODY_ONE",
		"MELODY_HELLO",
		"bugle",
		"tb1",
		0

	
/*		{"The_Ale_is_dear"},
		{"Danza_de_Astureses"},
		{"rising_chromatic"},
		{"rising_cscale"},
		{"MELODY_ONE"},
		{"MELODY_HELLO"},
		{"bugle"},
		{"tb1"},
		{0}*/
};


/* Master Index Table, autogenerated to create the relationships
   between the two tables given above.
*/
static const long int MasterIndexTable[][3] = {

    {   1, LEN_LANGUAGE1, (long int) LanguageIndex_1 },
#ifndef CHINESE_MMI
    {   2, LEN_LANGUAGE2, (long int) LanguageIndex_2 },
#else
    {   2, LEN_LANGUAGE2,(long int) LanguageIndex_2 },
#endif
 	{   3,  8, (long int) MelodyTable },
    { 0, 0, 0 }
};


/*******************************************************************************
                                                                              
                                Public routines
                                                                              
*******************************************************************************/


/*******************************************************************************

 $Function:    	Mmir_BaseAddress

 $Description:	returns the base address of the Master Index Table

 $Returns:		As above

 $Arguments:	none.
 
*******************************************************************************/

void *Mmir_BaseAddress()
{
    return (char *) MasterIndexTable;
}






/*******************************************************************************
 $Function:    	Mmir_NumberOfEntries

 $Description:	returns the number of entries in the master index table.

 $Returns:		As above

 $Arguments:	none.
 
*******************************************************************************/

int Mmir_NumberOfEntries()
{
/*
    return 2;
*/
/*MC, SPR 1150 got rid of magic number*/
	return NO_OF_LANGUAGES;
}


/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/



