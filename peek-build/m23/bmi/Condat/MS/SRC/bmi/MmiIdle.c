/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		Idle
 $File:		    MmiIdle.c
 $Revision:		1.0

 $Author:		Condat(UK)
 $Date:		    25/10/00

********************************************************************************

 Description:

   Handling for the idle screen

********************************************************************************
 $History: MmiIdle.c

	May 09, 2007  DR: OMAPS00129014
	Description: COMBO: unable to send SMS during midi playing
	Solution: Removed the phlock check status code in idle_draw_main_idle. 
			Added a flag to identify whether autophonelock is enabled or not and start the timer 
				based on the flag status
	
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 
 
      Mar 30, 2007 OMAPS00123019 a0393213(R.Prabakar)
      Description :  Reboot of the target when you unplug/plug USB cable using camera application
      
       28th Mar 2007 OMAPS00121870 a0393213(R.Prabakar)
	 Description : In Limited service mode unable to access idle screen options menu and Names/Read
       
	Jan 24, 2007 OMAPS00112312 a0393213(R.Prabakar)
	Description:MMI causes system freeze

 	Jan 05, 2007 OMAPS00084665  x0039928(sumanth)
 	Description: Boot time exceed UCSD estimations
 	
       Dec 08, 2006 OMAPS00106744  a0393213(R.Prabakar)
       Description : Wrong string is displayed when pressing long [1]
       
 	Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
 	Description : CT-GCF[27.22.4.22.1]-Incorrect string display
 	Solution     : The DCS attribute is set dynamically
 	

       Oct 30, 2006 ER:OMAPS00091029 x0039928(sumanth)
       Bootup time measurement
        
       Oct 11, 2006 REF:OMAPS00098287  x0039928
       Description : CPHS 4.2: MMI variable als_status is not updated proplery with the SIM contents.
       Solution     : als_status update is deffered until the idle screen is displayed.
       
	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option
	

 	xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
 	Ports are automatically enumerated if user does not select any option with in 5 seconds.
 
	xrashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

  	Aug 31, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	- Removed the definition of new function GLOBAL USHORT get_nm_status(void).
	this function is not requierd as we are removing the new code added in callnumber() : MFW_SS_USSD
	switch case to send the USSD string through at+cusd command. Instead sending through ATDn command
	
				
 	Aug 30, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	- Defined a new function GLOBAL USHORT get_nm_status(void) as per the review 
				comments to avoid the usage of the global variable gnm_status

			
 	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	- Defined a new global variable gnm_status to hold the network status which will be 
			used in mmicall.c
			- idle_dialling_kbd_cb(): Copy the nm_status which is used during initiating the 
			USSD transaction


       July 12, 2006    REF:DR OMAPS00081477   x0047075
	Description:   SMS (with SMS Report Indicator +CDS Enabled) Reset Issue.
	Solution: Added a window handler for the sms status editor.
	              If sms status editor does not exist, create it.If it already exists Update the same.
    Jun 30, 2006     REF : OMAPS00083503    R.Prabakar
    Description : When USSD string is sent, MMI shows requesting screen forever
    Solution     : We will send USSD string or call number only when we are in full service, unless it's an emergency number

    10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
    Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
    Solution     : The attribute of ATB editor is set to unicode
    			   
   July 04, 2006 REF:OMAPS00083759 a0393213(R.Prabakar)
   Description : Roaming indicator icon shown during searching
   Solution     : Roaming icon display condition will be checked only when we are in full network
   
	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution:	a) Removed the usage of global variable gEons_alloc_active
			b) In func, idle_draw_main_idle(), getting network logo statement is remoevd and replaced by NULL to pass the EONs test cases
			c) Added a check to display SPN if PLMN anme and numeric names are NULL
			   	
       Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : As part of the implementation, in this file, Roaming icon is displayed on idle screen based on a condition

    Jun 06, 2006 DR: OMAPS00080543 - xreddymn
    Changes to display long SAT messages correctly on idle screen.

 	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Added "IDLE_ALS_DATA" to display Current line indicator on the idle screen
			b) Modified the function "idle_displayData()", "idle_draw_main_idle()" to display 
				Current active line indicator on the idle screen

    Jun 02, 2006 DR: OMAPS00079699 - xreddymn
    Description: Long CB messages are not displayed completely.
    Solution: Allow user to navigate up / down to scroll through the message.

    May 15, 2006 DR: OMAPS00075852 - xreddymn
    Description: New SMS and Message Full indications will not be seen by user
    when CB is enabled, because CB messages have higher priority.
    Solution: CB messages and New SMS or Message Full indications are displayed
    in two different zones on the Idle screen.

	Apr 17, 2006    REF: ER OMAPS00075178 x0pleela
   	Description: When the charger connect to phone(i-sample), the charger driver works, but the MMI and App don't work
   	Solution: Created Timer handler for Battery charging indication

	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
   			Removed the code added in earlier version from function idle_kbd_cb()
   			Registering for MfwWinResume event in idle_create()
   			Handling MfwWinResume event in idle_win_cb()
   	
	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: "idle_opername_timer_cb" callback for timer to display opername on idle screen
   			Display of the operator name on idle screen based on the following:
			a. If SPN and plmn name are not NULL, SPN is displayed for 5 seconds and then plmn name
			    For the above scenario, display_condition of SPN is not checked as per Cingular's requirement
			    In spite of display_condition being FALSE, we go ahead and display SPN
	  	       b. If SPN is NULL and plmn name is not NULL, then plmn name is displayed			
   	
       Feb 02, 2006 DR: OMAPS00061468 - x0035544.
       Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
       Solution : SAT SET UP IDLE MODE TEXT (Icon) support added.
       
	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!

	Nov 17, 2005 DR: OMAPS00050447 - nekkareb
	Description: Even after removing the SIM , Hutch or Airtel network is seen
	Solution : Sim removal event is now being handled and appropriate display shown.
	
 	Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
	Description:   Mp3 Ringer Implementation
	Solution: The Midi Ringer application has been enhanced to support mp3 as ringer.
	The interface  function names have been modified.

       xrashmic 22 Aug, 2004 MMI-SPR-32798
       Adding the support for screen capture using a dynamically assigned key.

	Sept 15, 2005    REF: CRR 32410  x0021334 
	Description: The keypad unlocks even though the unlocking sequence is not executed in quick succession.
	Fix:	A new timer expiry event is added and handled.

	Jul 08, 2005    REF: ENH 32642 x0018858
   	Description: The date time string should at all times be displayed on the idle scree.
	Solution: Code to display the date time if already not displayed, has been added.

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.


	Jun 24, 2005 MMI-FIX-30973 x0018858
	Issue Description :- The homezone/cityzone tag disappears when the HUP button is pressed or after
				         "new CB" arrives indication is received and processed.
	Fix: Modified the code to display the homezone/cityzone tag after the HUP is processed.

	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
	Description:	IMEI retreival  by posting a call to ACI sAT_xx API
	Solution:		The direct call to cl_get_imeisv() is replaced with the
					call to the mmi_imei_store() which inturn calls sAT_Dn() and
					retrieves the IMEI info and stores it onto the global 
					variable "imei"
					
	Apr 05, 2005    REF: ENH 29994 xdeepadh
   	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
	Solution: Generic Midi Ringer and Midi Test Application were implemented.
   
	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.
	
 	Nov 3, 2004    REF: CRR 25887 xkundadu
	Description: Unable to make an emergency call.
	Fix: Changed the code to make the emergency call when the screen is locked.
		Also, a check is done to confirm whether the keypad buffer contains any ECC.
 
    CRR 25268: 13 Oct 2004 - xpradipg
    Description: Unable to see the Number / name and the status read/unread of
    the received EMS/MMS
    Solution: The status of each message is updated as soon as the Message is 
    read, and "old"/"new" is appended based on the status flag
    
    CRR 25270: 13 Oct 2004 - xpradipg
    Description: EMS: Titles such as "no name" is added to EMSs with no titles
    Solution: The corresponding name is retrieved from the phone book for the
    number from which EMS is received, if name is not present number is 
    displayed in Inbox

	Sep 22, 2004    REF: CRR 23871 xkundadu
	Description: Screen Locking: Unlocking screen is happening by other keys
	Fix: handled the other key events except '*' not to unlock the keypad.

	Sep 2, 2004    REF: CRR 21370 xkundadu
	Description: Giving wrong expected result by long press on "#" key
	Fix:	When user presses long on '#' key it will display 'p'.
		Changed the long press of '*' key to display 'w'
 	
	May 21, 2004    REF: CRR 16084  Deepa M.D 
	Fix:	SMSFull message string is  displayed,in the clean Idle Screen .
 
	Apr 27, 2004    REF: ENH 12646  Deepa M.D 
	Fix:		When the SMS memory is full,the SMS icon is made to blink.	
	Apr 14, 2004        REF: CRR 12653  xvilliva  
	Fix:	The FPLMN selection shows "No Service" on idle screen.
			The current mode of "limited service" is queried and the screen is updated
			accordingly. The LSK and RSK also work in sync.
	May 26, 2004    REF: CRR 19656  Sandip and Deepa M.D
	Fix:	when a SETUP IDLE TEXT command is sent with a long text ( 239 characters ), instead of discarding the message
			the message will be displayed.The CR is handled properly by converting it to Space.
	
	Jul 23,2004 CRR:20895 xrashmic - SASKEN
	Description: The keypad should not be unlocked by SEND and *, it should be
       unlocked only by Menu and *
	Fix: SEND key is now handled the same way as rest of the number keys. 
	Keypad unlock can be done only through MENU and *.

	Apr 14, 2004        REF: CRR 12653  xvilliva  
	The FPLMN selection shows "No Service" on idle screen.
	The current mode of "limited service" is queried and the screen is updated
	accordingly. The LSK and RSK also work in sync.
        xrashmic 26 Aug, 2004 MMI-SPR-23931
        To display the indication of new EMS in the idle screen
        
	25/10/00			Original Condat(UK) BMI version.
	
	Dec 23, 2005  REF:SR13873   x0020906
	Description: SMS Play tones to be stopped during button press in idle	
	FIX : Called for audio_stopSoundById () in the key press event.	
	
	Apr 05,2006 REF:OMAPS00061046 x0043641
	Description: On pressing the KCD_HUP key in searching state the menu displays
       'No Network' with 'Name' and 'Menu' as softkeys instead of 'Searching'.
       FIX: called network_start_full_service() in the key press of KCD_HUP.

    Apr 24, 2006    ERT: OMAPS00067603 x0043642
    Description: Engineering mode
    Solution:  Added functionalities to handle the magic sequence to access engineering mode
       
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


#include "prim.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_cm.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_simi.h"    // NH : put after mfw_nm.h as it contains a definition required by mfw_simi.h
#include "mfw_sat.h"
//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif


#include "dspl.h"
#include "MmiMmi.h"

#include "ksd.h"
#include "psa.h"
#include "pwr.h" /*OMAPS00091029 x0039928(sumanth)*/

#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"

#include "MmiMenu.h"

#include "MmiBookController.h"
#include "MmiBookUtils.h"

#include "mmiSmsRead.h"

#include "MmiCall.h"
#include "MmiNetwork.h"
#include "Mmiicons.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#if (BOARD != 71) && !defined(NEPTUNE_BOARD) /*a0393213 warnings removal-Don't include for locosto and neptune*/
#include "mmimmsmenu.h" //xrashmic 26 Aug, 2004 MMI-SPR-23931
#endif
#include "MmiSounds.h"
#include "MmiUserData.h"
#include "MmiSoftKeys.h"
#include "MmiPins.h"

#include "MmiDialogs.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "MmiSettings.h"
#include "mmiSmsIdle.h"
#include "mfw_ffs.h"
#include "mmiSmsBroadcast.h"
#include "MmiCPHS.h"

#include "mfw_td.h"/*SPR 1725*/
#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"	/* SPR759 - SH*/
#endif
#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"

#include "MmiResources.h" //for LINE_HEIGHT
#include "mmiColours.h"

#undef JVJ_USSD_TEST
#include "MmiBookShared.h"
#include "MmiBookUtils.h"

#include "mfw_mme.h"//xashmic 9 Sep 2006, OMAPS00092732
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
#include "MmiEm.h"
#endif
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
int pm_on = 2;  /*Neither PM is ON nor PM is OFF */
#endif
#endif

//x0pleela 16 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "usb/usb_api.h"
#endif /* FF_PHONE_LOCK */

#include "mfw_camapp.h"
#include "MmiCameraApp.h"
T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER *wefilename=NULL;
T_WCHAR me_uc[40];

//extern void rfs_test_01(void);
extern T_AUDIO_RET audio_mms_play_from_ffs_start (T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER *p_play_parameter, T_RV_RETURN return_path);

static UBYTE modem_boot_done; /*OMAPS00091029 x0039928(sumanth) - flag to indicate modem boot has happenned*/
static UBYTE network_sync_done; /*OMAPS00091029 x0039928(sumanth) - flag to indicate network sync has happenned*/

/*********************************************************************
**********************************************************************

					IDLE WINDOW. DECLARATION

*********************************************************************
**********************************************************************/

/*
*  This is a static window (also called dialog), it remains always, so that we dont have to worry a memory allocation
*/
/*
	The typedef for T_idle has been moved into the MmiIdle.h file.
*/

T_idle idle_data;						// This is a static window, and the related data are also static.

/*
*  These are common functions xxx_create and xxx_destroy
*/
T_MFW_HND idle_create (T_MFW_HND parent_window);
void idle_destroy (T_MFW_HND own_window);

// ADDED BY RAVI - 28-11-2005
EXTERN void icon_setMainBgdBitmap (int bmpId);
extern T_MFW_HND SmsRead_R_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
extern UBYTE getcurrentVoiceTone(void);
// END RAVI - 28-11-2005

/*
*  This dialog function (the same name as the window)
* is used to handle the comunication between different windows. The global macro SEND_EVENT can be used with parameter win
* and the corresponding events to send from one mmi dialog to another.
*/

void idle (T_MFW_HND win, USHORT event, SHORT value, void * parameter);


/*
*  These are common optional functions handler
*/


static int idle_win_cb(MfwEvt e, MfwWin *w);
static int idle_kbd_cb (MfwEvt e, MfwKbd *k);
static int idle_kbd_long_cb (MfwEvt e, MfwKbd *k);


static int idleLine( int lineNo );



static int idleExeSendMessage(MfwMnu* m, MfwMnuItem* i);
MfwHnd VOS_Idle_Win;                    /* our window               */

extern UBYTE getCurrentRingerSettings(void );

void idle_Ciphering (UBYTE gsm_ciph, UBYTE gprs_ciph);

U16 AlarmStatus = 0;

extern UBYTE HUPKeyOrigin; // SH - set to 1 if POWER_KEY is pressed in idle screen
UBYTE test_sim; // SH
static UBYTE LimitedService;//  Apr 14, 2004        REF: CRR 12653  xvilliva
//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
extern MfwHnd sms_animate;  //Timer for blinking SMS Icon
/* Power management Changes*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
MfwHnd Pm_timer;  /* Timer power management */
#endif
#endif

//x0pleela 11 Apr, 2006 ER: OMAPS00075178
 //Timer for battery charging animation
extern MfwHnd battery_charging_animate; 
static int  homezoneFlag = 0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858
							  // Added a flag.

//Jun 24, 2005 MMI-FIX-30973 x0018858
//Global to copy the New CB msg tag.
char tag_zone[94];

//x0pleela 09 Feb, 2006 ER: OMAPS00065203
#define SPN 		0	/* SPN to be displayed on Idle screen*/
#define PLMN 	1	/* PLMN name to be displayed on Idle screen*/

static MfwHnd plmn_display;				/* timer handle */
static UBYTE timer_start_flag = FALSE;		/* Timer start flag */	
static UBYTE opername_disp_flag=SPN;		/* Opertor name display flag : SPN or PLMN*/

// Oct 11, 2006 REF:OMAPS00098287  x0039928
extern T_MFW_CPHS_ALS_STATUS als_status;
extern BOOL als_bootup;
//x0pleela 23 Aug, 2006  DR: OMAPS00083503
GLOBAL USHORT get_nm_status(void); //returns the nm_status

//x0pleela 23 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
static MfwHnd autoPhLock_timer;						/* timer handle */
static UBYTE autoPhLock_timer_flag = FALSE;		/* Timer start flag */
extern T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
#endif /*FF_PHONE_LOCK*/
/*********************************************************************
**********************************************************************

					DIALLING WINDOW. DECLARATION

*********************************************************************
**********************************************************************/

/*
*  This is a dinamic dialog, it must be created only when dialling in idle screen, and destroyed automatically after calling
* or clearing
*/

//	Nov 3, 2004    REF: CRR 25887 xkundadu
//	Description: Unable to make an emergency call.
//	Fix: Check whether the keypad buffer contains any ECC.

//    This is the maximum length of ECC number.
#define ECC_LENGTH 3
#define WIN_DIALLING_ORG_X    0
#define WIN_CENTRALIZE_ORG_X 30

#define XOFFSET 1
#define YOFFSET 1


#ifndef LSCREEN
#define YLINE 9
#else
#define YLINE (12*2+2)
#endif


#define KEYPADLOCK_BUF_SIZE	4

/*
*  The information related to every window must be encapsulated in such an structure
*/

typedef struct
{
    T_MMI_CONTROL   mmi_control;		// common control parameter
	T_MFW_HND win;
	T_MFW_HND kbd;
    T_MFW_HND kbd_long;
#ifdef NEW_EDITOR
	T_ED_DATA	*editor;	/* SPR#1428 - SH - New Editor changes */
#else /* NEW_EDITOR */
	T_MFW_HND edt;
#endif /* NEW_EDITOR */
} T_idle_dialling;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL     mmi_control;
    T_MFW_HND         win;
    T_MFW_HND         parent_win;
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA   editor_data;
#endif

    /* internal data */
    char        buffer[80];
    UBYTE       status;

} tShowVerion;


EXTERN T_MFW_HND USB_CAMERA_win;

/*
*  These are common functions xxx_create and xxx_destroy
*/

T_MFW_HND idle_dialling_create(MfwHnd parent);
void idle_dialling_exit (void);


/*
*  This dialog function (the same name as the window)
* is used to handle the comunication between different windows. The global macro SEND_EVENT can be used with parameter win
* and the corresponding events to send from one mmi dialog to another.
*/

void idle_dialling (T_MFW_HND win, USHORT event, SHORT value, void * parameter);

/*
*  These are common optional functions handler
*/


static int idle_dialling_kbd_cb (MfwEvt e, MfwKbd *k);
static int idle_dialling_kbd_long_cb (MfwEvt e, MfwKbd *k);
static int idle_dialling_win_cb (MfwEvt e, MfwWin *w);

/*
*  This an optional function, used often to call, create and init a new dialog, with different parameters depending
* on the context
*/


T_MFW_HND idle_dialling_start (T_MFW_HND win_parent,char *character);



/*********************************************************************
**********************************************************************

					OPTIONS DIALLING WINDOW. DECLARATION

*********************************************************************
**********************************************************************/



#define WIN_OPTIONS_DIALLING_ORG_X 0
#define WIN_OPTIONS_DIALLING_ORG_Y SECOND_LINE

#define WIN_OPTIONS_DIALLING_X_OFFSET 1
#define WIN_OPTIONS_DIALLING_Y_OFFSET 1



T_MFW_HND idle_dialling_options_create(MfwHnd parent);
void idle_dialling_options_destroy (T_MFW_HND own_window);

static int idle_dialling_options_kbd_cb (MfwEvt e, MfwKbd *k);
static int idle_dialling_options_kbd_long_cb (MfwEvt e, MfwKbd *k);
static int idle_dialling_options_win_cb (MfwEvt e, MfwWin *w);
static int idle_dialling_options_mnu_cb (MfwEvt e, MfwMnu *m);

typedef struct
{
    T_MMI_CONTROL   mmi_control;
	T_MFW_HND win;     // window handle
	T_MFW_HND kbd;
    T_MFW_HND kbd_long;
	T_MFW_HND menu;
} T_idle_dialling_options;





/*********************************************************************
**********************************************************************

					KEYPADLOCKED WINDOW. DECLARATION

*********************************************************************
**********************************************************************/


T_MFW_HND idle_key_pad_locked_create(MfwHnd parent);
void idle_key_pad_locked_destroy (T_MFW_HND own_window);

static int idle_key_pad_locked_kbd_cb (MfwEvt e, MfwKbd *k);
static int idle_key_pad_locked_kbd_long_cb (MfwEvt e, MfwKbd *k);
static int idle_key_pad_locked_win_cb (MfwEvt e, MfwWin *w);

typedef struct
{
    T_MMI_CONTROL   mmi_control;
	T_MFW_HND win;     // window handle
	T_MFW_HND kbd;
    T_MFW_HND kbd_long;
#ifdef NEW_EDITOR		/* SPR#1428 - SH - New Editor changes */
	T_ED_DATA	*editor;
#else
    T_MFW_HND edt;
#endif
	T_MFW_HND win_info;     // window handle
	UBYTE menuPressed;
	UBYTE keyPadLockBuf[KEYPADLOCK_BUF_SIZE];
	UBYTE TempkeyPadLockBuf[KEYPADLOCK_BUF_SIZE];
} T_idle_key_pad_locked;

void idle_key_pad_locked (T_MFW_HND win, USHORT event, SHORT value, void * parameter);



static void (*showKeyPadLocked) (void);
static void showSOSCallKeyPadLocked(void);
void idle_info_destroy_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);


/*********************************************************************
**********************************************************************

					Information WINDOW. DECLARATION

*********************************************************************
**********************************************************************/



static int lockTimerEvent(MfwEvt e, MfwTim *t);
static void showPressMenuStar(void);
/* x0039928 -Lint warning removal 
static void showSMSNewMessage(void); */
static void showNothing(void);



T_MFW_HND idle_information_create(MfwHnd parent);
void idle_information_destroy (T_MFW_HND own_window);

static int idle_information_kbd_cb (MfwEvt e, MfwKbd *k);
/* x0039928 - lint warning removal
static int idle_information_kbd_long_cb (MfwEvt e, MfwKbd *k); */
static int idle_information_win_cb (MfwEvt e, MfwWin *w);

typedef struct
{
    T_MMI_CONTROL   mmi_control;
	T_MFW_HND win;     // window handle
    T_MFW_HND parent_win;
    T_MFW_HND timer;
	T_VOID_FUNC info_cb;
	T_MFW_HND kbd;
} T_idle_information;



void idle_information (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
T_MFW_HND idle_information_start(MfwHnd win_parent,S32 time,void (*show)(void),T_VOID_FUNC info_cb);





static void (*showInformation)(void);


static int idle_information_tim_cb(MfwEvt e, MfwTim *t);
static int idle_imei_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);


T_MFW_HND ShowVersion(T_MFW_HND parent_window);
static T_MFW_HND ShowVersion_create(MfwHnd parent_window);
static void ShowVersion_DialogCB(T_MFW_HND win, USHORT e,  SHORT identifier, void *parameter);
void ShowVersionSetEditor(T_MFW_HND win);
static void ShowVerionCB( T_MFW_HND win, USHORT Identifier,UBYTE reason);
void showVersion_destroy(MfwHnd own_window);
const char * date = "20080812";
const char * bmi  = "01.06.01";
const char * gprs = "XX.XX.XX";
const char * ti   = "N5.24M18V1.9";

//API - 13-01-03 - 1310 - Create a variable for idle instead of using global pin_emergency_call
UBYTE pin_idle_emerg_call;
//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
extern int smsFullEventTim( MfwEvt e, MfwTim *tc );

#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
#ifndef NEPTUNE_BOARD
//xrashmic 22 Aug, 2004 MMI-SPR-32798
U8 screenCaptureKey=KCD_NONE;
#endif
T_MFW_HND       screen_capture_win;
#endif
//xashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
USHORT USB_Mode = FALSE;
T_MFW_HND usb_opt_win;
T_MFW_HND usb_dialog_win;
MfwHnd usb_auto_enum_timer;//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
#define AUTO_USB_ENUMERATION_TIMER 5000 //xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
#endif

//x0pleela 09 Feb, 2006  ER: OMAPS00065203
//callback for timer to display opername on idle screen
int idle_opername_timer_cb(  MfwEvt e, MfwTim *tc );

//x0pleela 23 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
//callback for timer to display phone unlock screen
int idle_autoPhoneLock_timer_cb(  MfwEvt e, MfwTim *tc );
EXTERN int phLock_UnLock (void);
//x0pleela 09 May, 2007  DR: OMAPS00129014
EXTERN UBYTE phlock_auto;
#endif /*FF_PHONE_LOCK*/
/*********************************************************************
**********************************************************************

					IDLE WINDOW. IMPLEMENTATION

*********************************************************************/
/*******************************************************************************

 $Function:    	idleInit

 $Description:	init MMI idle handling

 $Returns:

 $Arguments:

*******************************************************************************/




void idleInit (MfwHnd parent)
{
	/* x0045876, 14-Aug-2006 (WR - "hnd" was set but never used) */
	/* T_MFW_HND hnd; */
	
    winAutoFocus(TRUE);
    idle_data.nm_status = 0;
	//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
	//Timer created to blink the SMS icon , when SMS  memory is full
	sms_animate= timCreate(0,500,(MfwCb) smsFullEventTim);

	//  x0pleela 11 Apr, 2006 ER: OMAPS00075178
	//Timer created to animate battery charging
	battery_charging_animate= timCreate(0,1000,(MfwCb) BattChargeEventTim);
	
	//x0pleela 10 Feb, 2006  ER: OMAPS00065203
	//Create timer to display SPN for 5 secs and then display PLMN name
	plmn_display = timCreate(0, 5000, (MfwCb)idle_opername_timer_cb);

//x0pleela 23 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	//Create timer to display Phone lock unlock screen - timer is 10 secs duration
	autoPhLock_timer = timCreate(0, 10000, (MfwCb)idle_autoPhoneLock_timer_cb);
#endif /*FF_PHONE_LOCK*/
	
	/* x0045876, 14-Aug-2006 (WR - "hnd" was set but never used) */
    	/* hnd = idle_create (parent); */
	idle_create (parent);
	
    TRACE_EVENT_P1("FFS_flashData.IdleScreenBgd = %d", FFS_flashData.IdleScreenBgd);
	
    if( FFS_flashData.ProviderNetworkShow != TRUE )
		FFS_flashData.ProviderNetworkShow = FFS_flashData.ProviderNetworkShow;
    else
		FFS_flashData.ProviderNetworkShow = TRUE;
	
    /* x0045876, 14-Aug-2006 (WR - pointless comparison of unsigned integer with zero) */
    /* if((FFS_flashData.IdleScreenBgd >= BGD_NONE) && (FFS_flashData.IdleScreenBgd < BGD_LAST)) */
    if (FFS_flashData.IdleScreenBgd < BGD_LAST)
		idle_setBgdBitmap(FFS_flashData.IdleScreenBgd);
    else
		idle_setBgdBitmap(BGD_TI_LOGO);
	
	/* x0045876, 14-Aug-2006 (WR - pointless comparison of unsigned integer with zero) */
	/* if((FFS_flashData.MainMenuBgd >= BGD_NONE) && (FFS_flashData.MainMenuBgd < BGD_LAST)) */
	if (FFS_flashData.MainMenuBgd < BGD_LAST)
		icon_setMainBgdBitmap(FFS_flashData.MainMenuBgd);
    else
		icon_setMainBgdBitmap(BGD_SQUARE);


/* Power management */
/* create power management timer*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
     ; /* Don't do anything */
   /* Pm_timer = timCreate(0, (FFS_flashData.pm_duration * 1000),(MfwCb) powerManagementEvent);     */
  /* mmi_update_pm_timer_duration (); */  /* Update the global variable pm_on depending on the value set in flash */
#endif
#endif
}


/*******************************************************************************

 $Function:    	idleExit

 $Description:	 exit MMI idle handling

 $Returns:

 $Arguments:

*******************************************************************************/


void idleExit (void)
{
    TRACE_EVENT("IdleExit");
	//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
	//Delete the timer for blinking SMS Icon ,when memory is full.
    if (sms_animate)
	{
		timStop(sms_animate);
		timDelete(sms_animate);
	}
	sms_animate = 0;

	//x0pleela 11 Apr, 2006  ER: OMAPS00075178
	//Delete the timer of battery charging 
      if (battery_charging_animate)
	{
		timStop(battery_charging_animate);
		timDelete(battery_charging_animate);
	}
	battery_charging_animate = 0;
	
	//x0pleela 14 Feb, 2006  ER: OMAPS00065203
	//Delete the timer of operator name display
	if( plmn_display)
	{
		timDelete(plmn_display);
		plmn_display = 0;
	}

	//x0pleela 23 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	//Delete the timer for Phone unlock screen
	if( autoPhLock_timer)
	{
		timDelete(autoPhLock_timer);
		autoPhLock_timer = 0;
	}
#endif /*FF_PHONE_LOCK*/
    mfwSetSatIdle(0);
	
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

/*Delete timer*/
    if(Pm_timer)
    {
        timStop(Pm_timer);
        timDelete(Pm_timer);
    }
#endif
#endif
    idle_destroy (idle_data.win);
}



/*******************************************************************************

 $Function:    	idle_create

 $Description:	 Creation of an instance for the Idle dialog.

 $Returns:

 $Arguments:

*******************************************************************************/


T_MFW_HND idle_create (T_MFW_HND parent_window)
{
    T_idle        * data = &idle_data;
    T_MFW_WIN     * win;
	UBYTE result;

	TRACE_FUNCTION ("idle_create");

    memset(&idle_data,'\0',sizeof(idle_data));
    idle_data.defTextOffset= SCREEN_SIZE_Y-Mmi_layout_softkeyHeight()-6*Mmi_layout_line_height();

    /* xreddymn OMAPS00075852 May-15-2006
     * Set the default value for dialer_visible.
     */
    idle_data.dialer_visible = FALSE;

	/*
     * Create window handler
     */

	//x0pleela 05 Apr, 2007  ER: OMAPS00122561
	//register for MfwWinSuspend also for Automatic phone lock
     //x0pleela 15 Feb, 2006 ER:OMAPs00065203
     //Register MfwWinResume event also
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE|MfwWinResume
						#ifdef FF_PHONE_LOCK
							|MfwWinSuspend
						#endif //FF_PHONE_LOCK
				, (T_MFW_CB)idle_win_cb);

    if (data->win EQ 0)
    {
       mfwSetSatIdle(0);
       return 0;
    }

	VOS_Idle_Win = data->win;


	/*
     * These assignments are necessary to attach the data to the window, and to handle the mmi event communication.
     */


	data->mmi_control.dialog    = (T_DIALOG_FUNC)idle;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
    * Create any other handler
    */

    data->kbd      = kbd_create (data->win,KEY_ALL|KEY_MAKE,(T_MFW_CB)idle_kbd_cb);
    data->kbd_long = kbd_create (data->win,KEY_ALL|KEY_LONG,(T_MFW_CB)idle_kbd_long_cb);
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	data->editor = 0;
	data->editor_sat = 0;
	AUI_edit_SetAttr(&data->editor_attr2, WIN_DIALLING_CB, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_ASCII, (UBYTE*)data->incoming_cb_msg,	MAX_CBMSG_LEN);
	AUI_edit_SetAttr(&data->editor_attr3, WIN_DIALLING_SMS, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_ASCII, (UBYTE*)data->incoming_sms_msg,	MAX_MSG_LEN_SGL);
	
	/* 09-May-2006, x0045876 (OMAPS00064076) */
   /*10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
       Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
       Solution     : The attribute of ATB editor is set to unicode*/
       /*13 Oct 2006 OMAPS00095266 a0393213(R.Prabakar)
	    Description : CT-GCF[27.22.4.22.1]-Incorrect string display
	    Solution     : changes done as part of OMAPS00077654 reverted back. The attribute of ATB editor is set to ASCII itself*/
	AUI_edit_SetAttr(&data->editor_attr_sat, WIN_DIALLING_SAT, COLOUR_EDITOR_XX, EDITOR_FONT, ED_MODE_READONLY, ED_CURSOR_NONE, ATB_DCS_ASCII, (UBYTE*)data->incoming_sat_msg,	MAX_SATMSG_LEN);
	
        // July 12, 2006    REF:DR OMAPS00081477  x0047075
        //Fix:Assigning the sms_status_editor window handle to NULL
	   idle_data.sms_status_editor = NULL; 
#else /* NEW_EDITOR */
	data->edt = 0;
	data->edt_sat = 0; /* SPR759 - SH*/
	bookSetEditAttributes(WIN_DIALLING_CB, 	COLOUR_EDITOR_XX,0,edtCurNone,0,(char*)data->incoming_cb_msg,	MAX_CBMSG_LEN,&data->edt_attr2);
	bookSetEditAttributes(WIN_DIALLING_SMS,	COLOUR_EDITOR_XX,0,edtCurNone,0,(char*)data->incoming_sms_msg,	MAX_MSG_LEN_SGL,&data->edt_attr3);
    /* xreddymn OMAPS00080543 Jun-06-2006 */
	bookSetEditAttributes(WIN_DIALLING_SAT,	COLOUR_EDITOR_XX,0,edtCurNone,0,(char*)data->incoming_sat_msg,	MAX_SATMSG_LEN,&data->edt_attr_sat);
        // July 12, 2006    REF:DR OMAPS00081477  x0047075
        //Fix:Assigning the sms_status_editor window handle to NULL
	idle_data.sms_status_edt = NULL; 
#endif /* NEW_EDITOR */

/*
    * In the new design the are created and automatically shown.
    */

	winShow(data->win);
	data->search=FALSE;
	data->missedCalls = 0;
	//API - 23/09/02 - Emergency Call Define
	pin_idle_emerg_call = FALSE;
	data->flagIdle=FALSE;
	data->right_key_pressed = FALSE;
	addCBCH(NULL, IdleNewCBImmediateMessage);
	data->new_cbch= FALSE;
	data->clean_screen = FALSE; //for clean the DISPLAY ZONE 3
	data->ciphering = DONT_SHOW_CPRS; //use as a default
    data->starting_up = TRUE; /*SPR#1662 JVJ New flag for the start up procedure */


	//init the Ciphering indication
	result = sim_init_CPRS ((CallbackCPRI) idle_Ciphering, MFW_SIM_CPRI_SHOW );

	if (result == FALSE)
		TRACE_EVENT("Cant show Ciphering");

	/*
	** CQ16435 : Set the Mfw Sat window to this value.
	*/
	mfwSetSatIdle(data->win);

	/*
   * return window handle
   */
  return data->win;
}


/*******************************************************************************

 $Function:    	idle_destroy

 $Description:	 Destroy the idle dialog.

 $Returns:

 $Arguments:

*******************************************************************************/


void idle_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_idle        * data;

	/*
     * The destruction of this dialog is trivial, because  the information contained is not dynamic. A better example
	 * of dialog destruction can be found in dialling_destroy
	*/
	TRACE_FUNCTION ("idle_destroy");


  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_idle *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */
      win_delete (data->win);
	  data->win=0;
    }
  }
}


/*******************************************************************************

 $Function:    	idle

 $Description:	 Dialog function for idle top window.

 $Returns:

 $Arguments:

*******************************************************************************/


void idle (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	U8 * key_code;
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_idle         * data = (T_idle *)win_data->user;


	/*
     * This function is called by the SEND_EVENT macro, when the parameter win is the idle window. the different events
	 * come from another mmi modules.
 	*/


	TRACE_FUNCTION ("idle()");

    switch (event)
	{
      case NETWORK_NO_SERVICE:
      case NETWORK_NO_PLMN_XXX_AVAIL:
        /*
         * Network management indicates no or limited service
         */
        data->nm_status  = event;
		data->search=TRUE;

		/*NM, p020*/
		if(idleIsFocussed())
		windowsUpdate();
        break;
      case NETWORK_SEARCH_NETWORK:
      case NETWORK_FULL_SERVICE:
        /*
         * Network will be searched or is available
         */

#ifdef MMI_HOMEZONE_ENABLED
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified the conditional statement to read only once.Rest of the code will be performed everytime.
	//begin 30439
  if (event==NETWORK_FULL_SERVICE && !homezoneFlag)
  {
	homezoneFlag = 1; /* only read one time from SIM for FULL SERVICE */
  	homezoneReadSim(HZ_READ_ONCE_ONLY);		// Read in homezone data. Will do nothing on repeated calls.
  }
  //end 30439
#endif
		data->search=FALSE;
		data->nm_status  = event;

		/*NM, p020*/
		if(idleIsFocussed())
        windowsUpdate();
        break;

  	  case IDLE_UPDATE:
		/*NM, p020*/
		if ((idleIsFocussed()) || (idle_data.klck_win != NULL))
		{
			if ((mmiStart_animationComplete()!=TRUE) && (mmiPinsEmergencyCall() == TRUE))
			{
				mmiPinsResetEmergencyCall();
				mfwSimRestartPinEntry();
			}
			else
	 			windowsUpdate();
		}
  	break;

	  case IDLE_NEW_SMS:
		  // show the info about incoming sms on the idle screen
	      idle_data.clean_screen = FALSE;
		/*NM, p020*/
		if(idleIsFocussed())
		  windowsUpdate();
		else if (idle_data.klck_win != NULL)
			SEND_EVENT(idle_data.klck_win,IDLE_NEW_SMS,0,0);
	  break;
      case IDLE_ACTIVE_KEY_PAD_LOCKED:
		idle_data.klck_win = idle_key_pad_locked_create(win);
		windowsUpdate();
		break;

      case IDLE_DETAILS_NUMBER:
		idle_dialling_create(win);
		windowsUpdate();
		break;

	 case IDLE_START_DIALLING_NUMBER:
		key_code = (U8*)parameter;
		memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));
		idle_data.edt_buf[0]=editControls[*key_code];
		idle_dialling_start(idle_data.win,NULL);
		break;

      default:
		return;
	}

}

/*******************************************************************************

 $Function:    	idleExec

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idleExec (int reason, MmiState next)
{
    MmiModuleSet (ModuleIdle); /* Set the global status of Idle */
	switch (reason)
    {
        case IdleNormal:                /* enter normal idle mode   */
			winShow(idle_data.win);
        break;
		default:
        break;
    }
}


/*******************************************************************************

 $Function:    	idleEvent

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idleEvent (int reason)
{
	TRACE_EVENT_P1("IDLE event reason: %d",reason);

    switch (reason)
    {
		case IdleSearchNetwork:
		    if (idle_data.win)
			    SEND_EVENT(idle_data.win,IDLE_NO_NETWORK,0,0);
	    break;
    	case IdleUpdate:
    		if (idle_data.win)
	    		//SEND_EVENT(idle_data.win,IDLE_UPDATE,0,0);
    	break;
	    case IdleNewSmsMessage:
		    if (idle_data.win)
			    SEND_EVENT(idle_data.win,IDLE_NEW_SMS,0,0);
    	break;
	    case IdleActiveKeyPadLocked:
		   	if (idle_data.win)
		    	SEND_EVENT(idle_data.win,IDLE_ACTIVE_KEY_PAD_LOCKED,0,0);
    	break;
	    case PhbkDetailListToIdle:
		   	if (idle_data.win)
		    	SEND_EVENT(idle_data.win,IDLE_DETAILS_NUMBER,0,0);
    	break;
	    default:
        break;
    }
}



/*******************************************************************************

 $Function:    	idle_setBgdBitmap

 $Description:	This procedure is called when the idle background bitmap is to be changed

 $Returns:		None

 $Arguments:	bmp - pointer to the bitmap structure.

*******************************************************************************/
void idle_setBgdBitmap(int bmpId)
{
	idle_data.idleBgd = icon_getBitmapFromId(bmpId, NULL);

}


/*******************************************************************************

 $Function:    	idle_initDisplayData

 $Description:	initialises the display data

 $Returns:

 $Arguments:

 $History
  GW 09/10/02 - Created

*******************************************************************************/
enum {
	IDLE_CHARGING_MODE,
	IDLE_NETWORK_NAME,
#ifdef FF_CPHS	
	IDLE_ALS_DATA,  //x0pleela 25 May, 2006  DR: OMAPS00070657
#endif
	IDLE_CALL_SMS_INFO,
	IDLE_CLOCK_STR,
	IDLE_DATE_STR,
	IDLE_LOCKED_ECC,
	IDLE_LOCKED,
	IDLE_MENU_PRESS,
	IDLE_CB_INFO,
	IDLE_END
};

static int idlePosition[IDLE_END];
/*******************************************************************************

 $Function:

 $Description:

 $Returns:

 $Arguments:

 $History
  GW 09/10/02 - Created

*******************************************************************************/
int idle_initDisplayData( void )
{
	int i;
	for (i=0;i<IDLE_END;i++)
		idlePosition[i] = 0;

	// ADDED BY RAVI - 28-11-2005
	return 0;
	// END RAVI - 28-11-2005
}


/*******************************************************************************

 $Function:    	idle_displayData

 $Description:	Procedure to allow different layouts of the idle screen data depending on
 				hardware configuration.
 				Each bit of text that can be displayed is given a value in the enumerated type.
 				For some h/w configurations, different enumerated type values are displayed in
 				the same position. By checking the 'idlePosition' array we can tell if we can display
 				a particular bit of text. This allows a C-sample to display either the SMS information
 				or the clock and for the D-sample to always display the clock (and the date).

 $Returns:		None.

 $Arguments:	dataId - value form the enumerated type that indicates what the data is
 				txtId - text identifier (can be set to TxtNull if not used)
 				strId - pointer to a char array to be displayed. Array is assumed to be ascii.
					NB If both txtId and strId are used, the string is displayed first followed by
					1 or 2 spaces, then the text id allowing display of "1 New Message" etc.
 $History
  GW 09/10/02 - Created

*******************************************************************************/
int idle_displayData( int dataId , int txtId, char* txtStr)
{
	int xPos, yPos;
	int lenStr,lenId,lenSpace;
	int txtWidth = 0;  // RAVI
	char *txtSpace = "";	//EF must initialise. 03/02/2003
	char *idStr;

	/* x0045876, 14-Aug-2006 (WR - "txtFormat" was set but never used) */
	/* int txtFormat = 0; // RAVI */
	
	int noRoom = 0;

	if (txtStr != NULL)
		lenStr = dspl_GetTextExtent( txtStr, 0);
	else
		lenStr = 0;
	if (txtId!=TxtNull)
	{
		idStr = (char*)MmiRsrcGetText(txtId);
		lenId = dspl_GetTextExtent( idStr, 0);
	}
	else
	{
		lenId = 0;
		idStr = "";
	}
	if ((lenId > 0 ) && (lenStr >0))
	{
#ifndef LSCREEN

			txtSpace=" ";

#else
			txtSpace="  ";

#endif
		lenSpace = dspl_GetTextExtent( txtSpace, 0);
	}
	else
	{
		lenSpace = 0;
	}
	txtWidth = lenStr+lenId+lenSpace;
	if (idlePosition[dataId] == 0)
		idlePosition[dataId] = 1;
	else
	{
		//We have already output something here - do not overwrite it
		noRoom = 1;
	}
#ifndef LSCREEN
	xPos = 0;
	yPos = 0;

	/* x0045876, 14-Aug-2006 (WR - "txtFormat" was set but never used) */
	/* txtFormat = 0; */

	switch (dataId)
	{
		case IDLE_NETWORK_NAME:
			xPos = 16;
			yPos = Mmi_layout_line_icons(1);
			break;

		case IDLE_CHARGING_MODE:
			xPos = 16;
			yPos = Mmi_layout_line_icons(2);
			break;

		case IDLE_CALL_SMS_INFO:
			xPos = 0;
			yPos = Mmi_layout_line_icons(3);
			break;

		case IDLE_CLOCK_STR:
            /* Marcus: Issue 1614: 23/01/2003: Start */
#ifdef NEW_EDITOR
			if ((idlePosition[IDLE_CALL_SMS_INFO] == 0) &&
			    (idle_data.editor == NULL))
#else
			if ((idlePosition[IDLE_CALL_SMS_INFO] == 0) &&
			    (idle_data.edt == NULL))
#endif
            /* Marcus: Issue 1614: 23/01/2003: End */
			{
				xPos = 30;
				yPos = 24;
			}
			else
			{	//no room to display the time
				noRoom = 1;
			}
			break;
		case IDLE_DATE_STR:
			noRoom = 1;	 //date not displayed in B-sample/C-sample
			break;
		case IDLE_LOCKED_ECC:
		case IDLE_LOCKED:
			xPos = 0;
			yPos = idleLine(2);
			/* xreddymn OMAPS00075852 May-15-2006
			 * Set the color values for keypad locked message.
			 */
			dspl_SetBgdColour( COL_White );
			dspl_SetFgdColour( COL_Black );
			dspl_Clear(0, yPos, SCREEN_SIZE_X, yPos + Mmi_layout_line_height());
			break;
		case IDLE_MENU_PRESS:
			xPos = 0;
			yPos = idleLine(3);
			/* xreddymn OMAPS00075852 May-15-2006
			 * Set the color values for unlock key-press sequence message.
			 */
			dspl_SetBgdColour( COL_White );
			dspl_SetFgdColour( COL_Black );
			dspl_Clear(0, yPos, SCREEN_SIZE_X, yPos + Mmi_layout_line_height());
			break;

		/* xreddymn OMAPS00075852 May-15-2006
		 * Calculate the display position.
		 */
		case IDLE_CB_INFO:
			yPos = idleLine(4);
			break;

		default:
			break;
	}
#else
	xPos = (SCREEN_SIZE_X - txtWidth)/2;
	yPos = idle_data.defTextOffset;

	/* x0045876, 14-Aug-2006 (WR - "txtFormat" was set but never used) */
	/* txtFormat = 0; */
	
	switch (dataId)
	{
		case IDLE_NETWORK_NAME:
			if (idle_data.idleNetLogo == NULL)
				yPos = Mmi_layout_line_height()*4;
			else
				yPos = yPos + Mmi_layout_line_height()*3;
			break;
			
//x0pleela 25 May, 2006  DR:OMAPS00070657
#ifdef FF_CPHS
		case IDLE_ALS_DATA:
			yPos = Mmi_layout_line_height()*6;
			break;
#endif			

		case IDLE_CHARGING_MODE:
			yPos = yPos + Mmi_layout_line_height()*4;
			break;

		case IDLE_CALL_SMS_INFO:
			yPos = yPos + Mmi_layout_line_height()*5;
			break;

		case IDLE_CLOCK_STR:
            /* Marcus: Issue 1614: 23/01/2003: Start */
#ifdef NEW_EDITOR
			if (idle_data.editor == NULL)
#else
			if (idle_data.edt == NULL)
#endif
            /* Marcus: Issue 1614: 23/01/2003: End */
			{
    			xPos = 10;
    			yPos = yPos + Mmi_layout_line_height();
			}
			else
			{
				noRoom = 1;
			}
			break;
		case IDLE_DATE_STR:
            /* Marcus: Issue 1614: 23/01/2003: Start */
#ifdef NEW_EDITOR
			if (idle_data.editor == NULL)
#else
			if (idle_data.edt == NULL)
#endif
			{
    			xPos = SCREEN_SIZE_X - txtWidth - 10;
    			yPos = yPos + Mmi_layout_line_height();
			}
			else
			{
				noRoom = 1;
			}
            /* Marcus: Issue 1614: 23/01/2003: End */
			break;
		case IDLE_LOCKED_ECC:
			xPos = 0; /*SPR#2026 - DS - Changed from 10 */
			yPos = yPos + Mmi_layout_line_height()*4;
			dspl_Clear(xPos,yPos,mmiScrX,mmiScrY);
			dspl_Clear(xPos,yPos+ Mmi_layout_line_height()*5,mmiScrX,mmiScrY);
			break;
		case IDLE_LOCKED:
			xPos = 10;
			yPos = yPos + Mmi_layout_line_height()*3;
			/* xreddymn OMAPS00075852 May-15-2006
			 * Set the color values for keypad locked message.
			 */
			dspl_SetBgdColour( COL_White );
			dspl_SetFgdColour( COL_Black );
			dspl_Clear(0, yPos, SCREEN_SIZE_X, yPos + Mmi_layout_line_height());
			break;
		case IDLE_MENU_PRESS:
			xPos = 10;
			yPos = yPos + Mmi_layout_line_height()*4;
			/* xreddymn OMAPS00075852 May-15-2006
			 * Set the color values for unlock key-press sequence message.
			 */
			dspl_SetBgdColour( COL_White );
			dspl_SetFgdColour( COL_Black );
			dspl_Clear(0, yPos, SCREEN_SIZE_X, yPos + Mmi_layout_line_height());
			break;

		/* xreddymn OMAPS00075852 May-15-2006
		 * Calculate the display position.
		 */
		case IDLE_CB_INFO:
			yPos = yPos + Mmi_layout_line_height()*4;
			break;

		default:
			break;

	}
#endif

	if (!noRoom)
	{
		//GW 28/11/02 - Only display a non-null string

		/*MC SPR 1526, using dspl_TextOut rather than dspl_ScrText*/
		if (txtStr != NULL)
			dspl_TextOut(xPos,				yPos,	0, txtStr);
		if (lenSpace!= NULL)
			dspl_TextOut(xPos+lenStr,		yPos,	0, txtSpace);
		dspl_TextOut(xPos+lenStr+lenSpace,yPos,	0, idStr);
	}

	// ADDED BY RAVI - 28-11-2005
	return 0;
	//END RAVI - 28-11-2005
}

#ifndef LSCREEN
/*******************************************************************************

 $Function:

 $Description:	Returns a pixel position for a line number on the idle screen

 $Returns:		y pixel position for the line

 $Arguments:	lineNo - line number

 $History
  GW 09/10/02 - Created

*******************************************************************************/
static int idleLine( int lineNo )
{
//	int yPos;  // RAVI
#ifndef LSCREEN
	return (Mmi_layout_line_icons(lineNo));
#else
	return (Mmi_layout_line(lineNo)*3/2+Mmi_layout_IconHeight());
#endif
}
#endif
/*SPR 1725, removed getClockString() and getDateString() functions*/

/*******************************************************************************

 $Function:

 $Description:

 $Returns:

 $Arguments:

 $History
  GW 09/10/02 - Created

*******************************************************************************/
//Fix for 19656
void idle_show_cb_editor_scrollbar(T_ED_DATA *e)
{
	int editX, editY, editWidth, editHeight;
	int scrollBarSize, scrollBarPos;

	if(e->viewHeight>=e->totalHeight) 
		return;

	editX=e->attr->win_size.px;
	editY=e->attr->win_size.py;
	editWidth=e->attr->win_size.sx-3;
	editHeight=e->attr->win_size.sy;

	dspl_DrawLine(editX+editWidth, editY, editX+editWidth, editY+editHeight-1);
	scrollBarSize = e->viewHeight * editHeight / e->totalHeight;
	if (scrollBarSize>editHeight)
		scrollBarSize = editHeight;
	scrollBarPos = e->viewStartPos * editHeight / e->totalHeight;

	/* xreddymn OMAPS00079699 Jun-02-2006 */
	dspl_DrawLine(editX+editWidth+1, editY+scrollBarPos, editX+editWidth+1,
		editY+scrollBarPos+scrollBarSize-1);
	dspl_DrawLine(editX+editWidth+2, editY+scrollBarPos, editX+editWidth+2,
		editY+scrollBarPos+scrollBarSize-1);
}

void idle_draw_main_idle( void )
{
T_CURRENT_NETWORK current_network;
char text[20];
int txtStrId;	//Id of text string to be displayed
int txtNo;		//Numeric value to be displayed (0=no value)
//int xOfs;		//used to offset text id when a number is to be displayed // RAVI
// int lenStr, txtFormat;  // RAVI
// static int nTimes=0;  // RAVI
int oldborderSize;
int localborderSize;
// Jul 08, 2005    REF: ENH 32642 x0018858
char dt_str[20];		//date string
char time_str[20];	//time string
/* x0045876, 14-Aug-2006 (WR - "xPos" was set but never used) */
/* int xPos,yPos;		//x and y position on screen */
int yPos;

// Nov 24, 2005, a0876501, DR: OMAPS00045909
int sim_status;

//  Apr 14, 2004        REF: CRR 12653  xvilliva
  CHAR            oper[MAX_ALPHA_OPER_LEN]; /* operator name */

  /* x0045876, 14-Aug-2006 (WR - "mode" was declared but never referenced) */
  /* T_ACI_COPS_MOD  mode;     */                /* COPS mode     */ 

  /* x0045876, 14-Aug-2006 (WR - "mode" was declared but never referenced) */
  /* T_ACI_COPS_FRMT frmt;       */              /* COPS format   */
  T_ACI_NRG_RGMD  regMode;
  T_ACI_NRG_SVMD  srvMode;
  T_ACI_NRG_FRMT  oprFrmt;
  T_ACI_NRG_SVMD  srvStat;
LimitedService = 0;//end of crr12653


// Nov 24, 2005, a0876501, DR: OMAPS00045909
	sim_status = sim_status_check();
	memset(&current_network,'\0',sizeof(T_CURRENT_NETWORK)); /*x0039928 - Lint warning fix */
	memset(&srvStat, '\0',sizeof(srvStat));
		TRACE_FUNCTION("idle_draw_main_idle()");

		/* If we have not finished animation, do not draw any of the idle screen */
		if (mmiStart_animationComplete()!=TRUE)
			return;

TRACE_FUNCTION("idle_draw_main_idle");

		resources_setColour( COLOUR_IDLE );
		dspl_ClearAll();
		idle_initDisplayData();

	    if (idle_data.starting_up) /*SPR#1662 - NH Show please wait since the phone is not ready yet*/
	    {
			idle_displayData(IDLE_NETWORK_NAME,TxtPleaseWait,NULL);
	        return;
	    }

	    /* GW SPR#1721 - Add a border round all the text on the idle screen */
#ifdef COLOURDISPLAY
	oldborderSize = dspl_setBorderWidth(1);
#endif
		if (dspl_GetFgdColour()==dspl_GetBorderColour())
		{
			//Get a colour that contrasts with the foreground colour
			dspl_SetBorderColour(dspl_GetContrastColour(dspl_GetFgdColour()));
			//If this is the same as the background colour... too bad
		}

		if(idle_data.idleBgd != NULL)
			dspl_BitBlt2(idle_data.idleBgd->area.px,idle_data.idleBgd->area.py,idle_data.idleBgd->area.sx,idle_data.idleBgd->area.sy,(void *)idle_data.idleBgd->icons,0,idle_data.idleBgd->icnType);

		// Jul 08, 2005    REF: ENH 32642 x0018858
		// Set the back ground and foreground colour.
     		dspl_SetFgdColour( COL_BLK );
      		dspl_SetBgdColour( COL_TRANSPARENT );     

       	if (idle_data.nm_status == NETWORK_FULL_SERVICE) 
		{
			network_get_name (&current_network);
#ifdef COLOURDISPLAY
//x0pleela 22 june, 2006  DR: OMAPs00082374
//commented this as it is not required for EONs test cases
			idle_data.idleNetLogo = NULL; //icon_getNetworkIcon(current_network.network_name);
#else
			idle_data.idleNetLogo = NULL;
#endif
		}
		else
			idle_data.idleNetLogo = NULL;

//The animation complete will provide a better

		if (idle_data.idleNetLogo)
		{
			int xOfs,yOfs;
			T_BITMAP* icn=idle_data.idleNetLogo;

			if (icn->area.sy < SCREEN_SIZE_Y - Mmi_layout_IconHeight())
			{
				yOfs = Mmi_layout_IconHeight();
				if (icn->area.sy + yOfs < idle_data.defTextOffset)
					yOfs = yOfs + (idle_data.defTextOffset - icn->area.sy)/2;
			}
			else
				yOfs = 0;

			if (icn->area.sx < SCREEN_SIZE_X)
				xOfs = (SCREEN_SIZE_X - icn->area.sx)/2;
			else
				xOfs = 0;
#ifdef COLOURDISPLAY
	        dspl_BitBlt2(xOfs,yOfs,icn->area.sx,icn->area.sy,icn->icons,0,icn->icnType);
#endif
		}

		if (smsidle_get_unread_sms_available())
			iconsSetState(iconIdSMS);/* Indicate SMS delivery to MS-User. */
		else
			iconsDeleteState(iconIdSMS);
/*   Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : If current_network.roaming_indicator is set, the roaming icon is set*/
/*   July 04, 2006 REF:OMAPS00083759 a0393213(R.Prabakar)
  	 Description : Roaming indicator icon shown during searching
   	Solution     : Roaming icon display condition will be checked only when we are in full network*/
		if(idle_data.nm_status == NETWORK_FULL_SERVICE && current_network.roaming_indicator)
			{
				iconsSetState(iconIdRoaming);
			}
		else
			{
				iconsDeleteState(iconIdRoaming);
			}
		/*
		DISPLAY ZONE 1
		*/
		GlobalIconStatus = GlobalSignalIconFlag | GlobalBatteryIconFlag
						| GlobalSMSIconFlag| GlobalVoiceMailIconFlag
						| GlobalCallForwardingIconFlag| GlobalKeyplockIconFlag
						| GlobalRingerIconFlag | GlobalRingVibrIconFlag
						| GlobalVibratorIconFlag | GlobalAlarmIconFlag
						| GlobalSilentRingerIconFlag|GlobalCipheringActivIconFlag
						| GlobalCipheringDeactivIconFlag
		/* SH 18/01/02. Flag for GPRS On icon. */
#ifdef MMI_GPRS_ENABLED
						| GlobalGPRSOnIconFlag
#endif
		/* SPR759 - SH - Flag for Homezone icon */
#ifdef MMI_HOMEZONE_ENABLED
						| GlobalHomezoneIconFlag
#endif
		/* SPR#1352 - SH - Flag for TTY icon */
#ifdef MMI_TTY_ENABLED
						| GlobalTTYIconFlag
#endif
		/* SH end */

						| GlobalRoamingIconFlag    /*Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)*/
						;

              /* OMAPS00050447: a0393130, handle removal of sim card */
              /* In case of no network service icon display is handled in   */
              /* switch case below                                                         */
             if(idle_data.nm_status == NETWORK_NO_SERVICE && sim_status == SIM_NOT_ACTIVE)
              /* OMAPS00050447: a0393130, handle removal of sim card */			  	
             {
                TRACE_EVENT("ICON display handled below");
              }			  
              else
              {
                 TRACE_EVENT("Normal ICON display handled");
                 iconsShow();
              }

		/*
		DISPLAY ZONE 2
		*/
		switch (idle_data.nm_status)
		{
			case NETWORK_FULL_SERVICE :
				if (globalMobileMode == (GlobalMobileOn|GlobalCharging))
				{
					if(globalBatteryMode == GlobalBatteryFull)
					{
						idle_displayData(IDLE_CHARGING_MODE,TxtChargComplete,NULL);

					}
					else
					{
						idle_displayData(IDLE_CHARGING_MODE,TxtCharging,NULL);
					}
					break;
				}
				else
				{
					if (DivertStatus == MFW_CFLAG_SET)
					{
						idle_displayData(IDLE_CHARGING_MODE,TxtCallDivert,NULL);
					}
				}
				/*NM p032
				set the flag when the TEST SIM is inserted*/
				if (!strcmp((char *)current_network.network_name,"00101"))
				{
					test_sim = TRUE;
				}
				else
				{
					test_sim = FALSE;
				}
				/*p032 end */

			    //x0pleela 9 Feb, 2006  ER:OMAPS00065203
			    
			    //If SPN and plmn name are not NULL, display SPN for 5 seconds and the plmn name
			    //For the above scenario, display_condition of SPN is not checked as per Cingular's requirement
			    //In spite of display_condition being FALSE, we go ahead and display SPN

			    //If SPN is NULL and plmn name is not NULL, then display plmn name
				
                		/* Marcus: Issue 1618: 24/01/2003: Start *
				 *
				 * If we have a service provider name, show that,
				 * else show the PLMN name
				 */
				
	  		         if(!timer_start_flag)
	  		         {
	  		           timStart(plmn_display);
	  		           timer_start_flag = TRUE;
	  		         }

				  if( ( opername_disp_flag EQ SPN ) AND 
				  	(strlen((char*)current_network.service_provider_name)) ) //SPN name
				 {
		  		   idle_displayData(IDLE_NETWORK_NAME,TxtNull,(char*)current_network.service_provider_name);
				  }
				else
				{
				  if(strlen((char*)current_network.plmn_name)) //PLMN name
				    idle_displayData(IDLE_NETWORK_NAME,TxtNull,(char*)current_network.plmn_name);
				  else  if(strlen((char*)current_network.network_name))	//Numeric name
				    idle_displayData(IDLE_NETWORK_NAME,TxtNull,(char*)current_network.network_name);
				  else //SPN name
				  	 idle_displayData(IDLE_NETWORK_NAME,TxtNull,(char*)current_network.service_provider_name);
				}

			if(network_sync_done==0)
	  		{
	  		/*OMAPS00091029 x0039928(sumanth) - to mark the end of network sync event*/
 				boot_time_snapshot(ENetworkSync); 
			       TRACE_EVENT("Boot Time Snapshot - ENetworkSync");
				network_sync_done=1;
	  		}
                /* Marcus: Issue 1618: 24/01/2003: End */

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
			if (mmi_cphs_get_als_value() )
			{
			 switch( FFS_flashData.als_selLine)
       	         {
       	         	case MFW_SERV_LINE1:
       	         		{
       	         			TRACE_EVENT("als_status: Line1");
       	         		idle_displayData(IDLE_ALS_DATA, TxtLine1, NULL);
// Oct 11, 2006 REF:OMAPS00098287  x0039928	
// Fix: Update the als_status variable
						if(als_bootup == TRUE)
 						{
 							als_status.selectedLine = MFW_SERV_LINE1;
							sAT_PercentALS(CMD_SRC_LCL, ALS_MOD_SPEECH);
						}
       	         		}
       	         		break;
       	         		
       	         	case MFW_SERV_LINE2:
       	         			{
       	         				TRACE_EVENT("als_status: Line2");
							idle_displayData(IDLE_ALS_DATA, TxtLine2, NULL);
// Oct 11, 2006 REF:OMAPS00098287  x0039928	
// Fix: Update the als_status variable		
						if(als_bootup == TRUE)
 						{
							als_status.selectedLine = MFW_SERV_LINE2;
							sAT_PercentALS(CMD_SRC_LCL, ALS_MOD_AUX_SPEECH);
						}
       	         		}
       	         		break;
       	         		
       	         	default:
       	         		idle_displayData(IDLE_ALS_DATA, TxtNull, NULL);
       	         		break;
       	         }
// Oct 11, 2006 REF:OMAPS00098287  x0039928	
// Fix: Update the als_status variable			
			    if(als_bootup == TRUE)
 			    {
   				 als_status.status = (T_MFW_LINE_STATUS)FFS_flashData.als_statLine;
  				 als_bootup = FALSE;
			    }
			}
                	
#endif
				
				break;

			case NETWORK_SEARCH_NETWORK :
				idle_displayData(IDLE_NETWORK_NAME,TxtSearching,NULL);
				 if(modem_boot_done==0)
	  			{
	  			/*OMAPS00091029 x0039928(sumanth) - to mark the end of modem boot event*/
   					boot_time_snapshot(EModemBoot); 
					TRACE_EVENT("Boot Time Snapshot - EModemBoot");
					modem_boot_done=1;
	  			}
				break;
			case NETWORK_NO_SERVICE :
				if (sim_status == SIM_NOT_ACTIVE)
				{
                                /* OMAPS00050447: a0393130, handle removal of sim card */
                                globalSignalPicNumber   =0; /* updating signal strength */
                                iconsShow();		               
                                /* OMAPS00050447: a0393130, handle removal of sim card */					
                                idle_displayData(IDLE_NETWORK_NAME,TxtNoCard,NULL);
                            }
// Nov 24, 2005, a0876501, DR: OMAPS00045909
				else if (sim_status == IMEI_NOT_VALID)
					idle_displayData(IDLE_NETWORK_NAME,TxtInvalidIMEI,NULL);
				else
				{//  Apr 14, 2004        REF: CRR 12653  xvilliva
				/*a0393213 warnings removal - srvStat initialized with NRG_SVMD_NotPresent(-1) instead of -2*/					
						srvStat = NRG_SVMD_NotPresent;
						  qAT_PercentNRG( CMD_SRC_LCL,
						                  &regMode,
						                  &srvMode,
						                  &oprFrmt,
						                  &srvStat,
						                  oper);		
						if(srvStat == NRG_SVMD_Limited && globalSignalPicNumber > iconSigStat49)
						{//xvilliva checking signal and  limited service status.
							idle_displayData(IDLE_NETWORK_NAME,TxtLimService,NULL);
							LimitedService = 1;
						}
						else
					idle_displayData(IDLE_NETWORK_NAME,TxtNoNetwork,NULL);
				}
				break;
			case NETWORK_NO_PLMN_XXX_AVAIL:
				if (sim_status == SIM_NOT_ACTIVE)
					idle_displayData(IDLE_NETWORK_NAME,TxtNoCard,NULL);
// Nov 24, 2005, a0876501, DR: OMAPS00045909
				else if (sim_status == IMEI_NOT_VALID)
					idle_displayData(IDLE_NETWORK_NAME,TxtInvalidIMEI,NULL);
				else
					idle_displayData(IDLE_NETWORK_NAME,TxtNoNetwork,NULL);
			default:
				idle_displayData(IDLE_NETWORK_NAME,TxtNoNetwork,NULL);
				break;
		}

		/*
		DISPLAY ZONE 3
		*/

		TRACE_EVENT("DISPLAY ZONE 3");
		txtStrId = TxtNull;
		txtNo= 0;
		// xOfs= 0; // RAVI
		if (!idle_data.clean_screen) //for clean the DISPLAY ZONE 3
		//dont show the following information when the user pressed
		// the HUP key once in the idle screen
		{
			TRACE_EVENT("Not clean screen");
			idle_data.new_sms = FALSE;

			/* xreddymn OMAPS00075852 May-15-2006
			 * Display messages on idle screen only if dialer is not visible.
			 */
			if(idle_data.dialer_visible == FALSE)
			{
		/*
		** NDH : 15/05/2003 : Change the priority for the idle display, such that
		** CB, SAT Idle Display Text or Class 0 SMS are displayed with
		** the highest priority
		*/
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			/* Normal incoming cell broadcast */
			if (idle_data.new_cbch)
			{
				if (idle_data.editor)
				{
					ATB_edit_Hide(idle_data.editor);
				}

				/* SPR759 - SH. Hide SAT idle mode text if present */
				if (idle_data.editor_sat)
				{
					ATB_edit_Hide(idle_data.editor_sat);
				}
				txtStrId = TxtNewCbMessage;
				TRACE_EVENT("new_cbch");
			}

			/* SPR759 - SH - Display SAT idle mode text */
			else if (idle_data.editor_sat)
			{
				/* xreddymn OMAPS00080543 Jun-06-2006 */
				localborderSize = dspl_setBorderWidth(0);
				ATB_edit_Unhide(idle_data.editor_sat);
				ATB_edit_Show(idle_data.editor_sat);
				idle_show_cb_editor_scrollbar(idle_data.editor_sat); //Fix for 19656
				dspl_setBorderWidth(localborderSize);
				TRACE_EVENT("editor_sat");
			}

			/* Dialling editor, Immediate incoming cell broadcast */
			else if (idle_data.editor)
			{
				TRACE_EVENT("*** Displaying number editor *");
				/* MZ issue 1948, Modify the text boarder settings before displaying the message. */
				localborderSize = dspl_setBorderWidth(0);
				ATB_edit_Unhide(idle_data.editor);
				ATB_edit_Show(idle_data.editor);
				/* xreddymn OMAPS00079699 Jun-02-2006
				 * Display scrollbar for the Cell Broadcast message viewer.
				 */
				idle_show_cb_editor_scrollbar(idle_data.editor);
				dspl_setBorderWidth(localborderSize);
			}
#else /* NEW_EDITOR */
			/* Normal incoming cell broadcast */
			if (idle_data.new_cbch)
			{
				if (idle_data.edt)
				{
					edtHide(idle_data.edt);
				}

				/* SPR759 - SH. Hide SAT idle mode text if present */
				if (idle_data.edt_sat)
				{
					edtHide(idle_data.edt_sat);
				}
				txtStrId = TxtNewCbMessage;
			}

			/* SPR759 - SH - Display SAT idle mode text */
			else if (idle_data.edt_sat)
			{
				edtUnhide(idle_data.edt_sat);
				edtShow(idle_data.edt_sat);
			}

			/* Immediate incoming cell broadcast */
			else if (idle_data.edt)
			{
				edtUnhide(idle_data.edt);
				edtShow(idle_data.edt);
			}
#endif /* NEW_EDITOR */
		    }/* end: if(idle_data.dialer_visible == FALSE) */

			/* xreddymn OMAPS00075852 May-15-2006
			 * Inform user the arrival of a new CB message.
			 */
			if (txtStrId != TxtNull)
			{
			    if (idle_data.new_cbch)
			    {
			        dspl_SetFgdColour( COL_BLK );
			        dspl_SetBgdColour( COL_TRANSPARENT );
			        idle_displayData(IDLE_CB_INFO, txtStrId, NULL);
			    }
			    txtStrId = TxtNull;
			}

			/* xreddymn OMAPS00075852 May-15-2006
			 * Display Missed Call, New SMS, Message full --etc messages in a
			 * different zone from CB or SAT messages.
			 */
			if (idle_data.missedCalls)
			{
				txtNo = idle_data.missedCalls;
				if(txtNo <= 1)
				{
					txtStrId = TxtMissedCall;
				}
				else
				{
					txtStrId = TxtMissedCalls;
				}
			TRACE_EVENT("missed calls");
			}
			//xrashmic 26 Aug, 2004 MMI-SPR-23931
			// To display the indication of new EMS in the idle screen
			else if (idle_data.ems)
			{
				#if defined (FF_MMI_MMS) || defined (FF_MMI_EMS)//MMI-SPR 49811 - x0035544 07 nov 2005
				txtStrId = TxtEMSNew;
				#endif  //MMI-SPR 49811 - x0035544 07 nov 2005
			}
			else if (sms_message_count(MFW_SMS_UNREAD) > 0 )
			{
				txtStrId = smsidle_display_sms_status(&txtNo );
				idle_data.new_sms = TRUE;
			}
			else if(FFS_flashData.settings_status & SettingsSilentMode)
			{
				txtStrId = TxtSilentMode;
				TRACE_EVENT("Silent mode");
			}
			/* Marcus: Issue 1604: 23/01/2003: Start */
			else if (smsidle_get_ready_state() && smsidle_get_memory_full())
			{
				/* x0018858 24 Mar, 2005 MMI-FIX-11321
				Added a condition to check for any pending message on the server.
				*/
				//begin -x0018858 24 Mar, 2005 MMI-FIX-11321
			    	if(TRUE == sms_check_message_pending())
				{
					TRACE_EVENT("Pending SMS, but list is full");
					txtStrId = TxtSmsWaiting;
				}//end -x0018858 24 Mar, 2005 MMI-FIX-11321
				else
				{
					TRACE_EVENT("No unread SMS, but list is full");
					txtStrId = TxtSmsListFull;
				}
			}
			/* Marcus: Issue 1604: 23/01/2003: End */
            else
			{
				TRACE_EVENT("None of the above!");
			}
		}
		else //(!idle_data.clean_screen)
		{
		TRACE_EVENT("Clean screen");

			/* xreddymn OMAPS00075852 May-15-2006
			 * Display messages on idle screen only if dialer is not visible.
			 */
			if(idle_data.dialer_visible == FALSE)
			{

		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			//this is for the immediate incoming cell broadcast
			if (idle_data.editor)
			{	/* MZ  Issue 1948, modify the text border settings before displaying the message.*/
				localborderSize = dspl_setBorderWidth(0);
				ATB_edit_Unhide(idle_data.editor);
				ATB_edit_Show(idle_data.editor);
				/* xreddymn OMAPS00079699 Jun-02-2006
				 * Display scrollbar for the Cell Broadcast message viewer.
				 */
				idle_show_cb_editor_scrollbar(idle_data.editor);
				dspl_setBorderWidth(localborderSize);
			}
#else /* NEW_EDITOR */
			//this is for the immediate incoming cell broadcast
			if (idle_data.edt)
			{
				edtUnhide(idle_data.edt);
				edtShow(idle_data.edt);
			}
#endif /* NEW_EDITOR */
            }/* end: if(idle_data.dialer_visible == FALSE) */

			//20/4/01 MZ check for silent_mode status.
			if(FFS_flashData.settings_status & SettingsSilentMode)
			{
				txtStrId = TxtSilentMode;
			}
			//  May 21, 2004    REF: CRR 16084  Deepa M.D 
			//Display the  Message Full String, when the Idle screen is clean.
			else if (smsidle_get_ready_state() && smsidle_get_memory_full())
			{
				/* x0018858 24 Mar, 2005 MMI-FIX-11321
				Added a condition to check for any pending message on the server.
				*/
				//begin -x0018858 24 Mar, 2005 MMI-FIX-11321
				if(TRUE == sms_check_message_pending())
				{
					TRACE_EVENT("Pending SMS, but list is full");
					txtStrId = TxtSmsWaiting;
				}//end -x0018858 24 Mar, 2005 MMI-FIX-11321
				else
				{
					TRACE_EVENT("No unread SMS, but list is full");
					txtStrId = TxtSmsListFull;
				}
			}
			else
			{
				TRACE_EVENT("None of the above 2");
			}
		}
		//If data has been set up, then show it
		if (txtStrId != TxtNull)
		{
			/* xreddymn OMAPS00075852 May-15-2006
			 * Set the color values to display the text.
			 */
			dspl_SetFgdColour( COL_BLK );
			dspl_SetBgdColour( COL_TRANSPARENT );
			if (txtNo != 0)
			{
				sprintf((char*)text,"%d",txtNo);
				idle_displayData(IDLE_CALL_SMS_INFO,txtStrId,text);
			}
			else
				idle_displayData(IDLE_CALL_SMS_INFO,txtStrId,NULL);
		}

		idle_displayData(IDLE_CLOCK_STR,	TxtNull, mfw_td_get_clock_str());/*SPR 1725*/
		idle_displayData(IDLE_DATE_STR,		TxtNull, mfw_td_get_date_str()); /*SPR 1725*///Only displayed on D-sample

		/*
		DISPLAY ZONE 4
		*/

		TRACE_EVENT("DISPLAY ZONE 4");
		resources_setSKColour( COLOUR_IDLE );

	    /* GW SPR#1721 - Changed from a case statement so we restore colour/border info   */
		if ((idle_data.search) && ((idle_data.nm_status==NETWORK_NO_SERVICE) ||
								(idle_data.nm_status==NETWORK_NO_PLMN_XXX_AVAIL)))
		{
			TRACE_EVENT("idle_win_cb()-no network yet");
			//  Apr 14, 2004        REF: CRR 12653  xvilliva
			//		Here we are setting the softkey names if there are missedCalls/SMS/CBCH
			//		we set the left key as "Read" else it is "Names".
			/*28th Mar 2007 OMAPS00121870 a0393213(R.Prabakar)
			    Network signal indicator is also taken into account to show the soft keys*/
			if(srvStat == NRG_SVMD_Limited  && globalSignalPicNumber > iconSigStat49)
			{
				if(idle_data.new_sms || idle_data.missedCalls > 0 || idle_data.new_cbch)
					softKeys_displayId(TxtRead,TxtSoftMenu,0,COLOUR_IDLE);
				else
					softKeys_displayId(TxtNames,TxtSoftMenu,0,COLOUR_IDLE);
			}
			else
			{
				/*28th Mar 2007 OMAPS00121870 a0393213(R.Prabakar)
				    "Menu" is mapped to right softkey*/
				softKeys_displayId(TxtSearchName,TxtSoftMenu,0,COLOUR_IDLE);
			}
		}
		//xrashmic 26 Aug, 2004 MMI-SPR-23931
		// To display the 'read' softkey for the indication of new EMS in the idle screen
		//CRR 25268 & 25270: 14 Oct 2004 - xpradipg
		//display the read softkey only if there are no new cb message or sat messsage on idle screen
		else if (idle_data.ems && !idle_data.missedCalls && !idle_data.clean_screen)
		{
			softKeys_displayId(TxtRead,TxtSoftMenu,0,COLOUR_IDLE);
		}
		else if ( smsidle_get_unread_sms_available()
			&& (!idle_data.clean_screen)
			&& idle_data.new_sms )
		{
			TRACE_EVENT("idle_win_cb()-unread SMS");
			softKeys_displayId(TxtRead,TxtSoftMenu,0,COLOUR_IDLE);
		}
		else if ((!idle_data.clean_screen) && idle_data.new_cbch)
		{
			TRACE_EVENT("idle_win_cb()-new CBCH");
			softKeys_displayId(TxtRead,TxtSoftMenu,0,COLOUR_IDLE);
		}
		else if ((!idle_data.clean_screen) && (idle_data.missedCalls > 0))
		{
			TRACE_EVENT("idle_win_cb()-missed calls");
			softKeys_displayId(TxtRead,TxtSoftMenu,0,COLOUR_IDLE);
		}
		else
		{
			TRACE_EVENT("idle_win_cb()-phonebook");
			softKeys_displayId(TxtNames,TxtSoftMenu,0,COLOUR_IDLE);
		}

// Jul 08, 2005    REF: ENH 32642 x0018858
//Code to display the date/time even if the idle_data editor is being displayed.
//Begin 32642
#ifdef NEW_EDITOR
		if (idle_data.editor != NULL)
#else
		if (idle_data.edt != NULL)
#endif
		{
			dspl_SetFgdColour( COL_BLK );
			dspl_SetBgdColour( COL_TRANSPARENT );     

			strcpy((char *)dt_str,  mfw_td_get_date_str());
			strcpy((char *)time_str,  mfw_td_get_clock_str());


			/* x0045876, 14-Aug-2006 (WR - "xPos" was set but never used) */
			/* xPos = 10; */
			
			yPos = Mmi_layout_line_height()*7;

			dspl_Clear(10, Mmi_layout_line_height()*7,mmiScrX,mmiScrY);
			dspl_TextOut (10,Mmi_layout_line_height()*7, 0, time_str);

			dspl_Clear(125, Mmi_layout_line_height()*7,mmiScrX,mmiScrY);		
			//x0pleela 09 Nov, 2006 ER: OMAPS00099966
			//resizing for Endurance Font support
		#ifdef FF_ENDURANCE_FONT
			dspl_TextOut( 65, yPos, 0, dt_str );
		#else
			dspl_TextOut( 100, yPos, 0, dt_str );
		#endif

		}
//End 32642
			
		resources_restoreMnuColour();//required only if other menu items are to be drawn afterwards

		resources_restoreColour();

#ifdef COLOURDISPLAY
	dspl_setBorderWidth(oldborderSize);
#endif

		//Debug - output how much memory is left.
		mfwCheckMemoryLeft();

//x0pleela 09 May, 2007  DR: OMAPS00129014
#ifdef FF_PHONE_LOCK
		//for Automatic Phone lock check	
		if( phlock_auto )
		{
			//start timer to display unlock screen
			if( (!autoPhLock_timer_flag) &&  idleIsFocussed()  )
			{
				timStart(autoPhLock_timer);
				autoPhLock_timer_flag= TRUE;
			}
		}
#endif //FF_PHONE_LOCK		
	TRACE_FUNCTION("end of idle_draw_main_idle()");
}

/*******************************************************************************

 $Function:    	idle_win_cb

 $Description:

 $Returns:

 $Arguments:

 $History
  GW 09/10/02 - Removed display code to separate procedure.

*******************************************************************************/
static int idle_win_cb (MfwEvt e, MfwWin *w)
{
TRACE_FUNCTION("idle_win_cb");
switch (e)
{
	case MfwWinVisible:

/* Mar 08, 2006    REF:ER OMAPS00065168  */
/* Destroy the editor to make the messages like 'new message' visible */
    #ifdef NEPTUNE_BOARD
    /* June 23, 2006 Ref: OMAPS00079445 */
    /* Destroy the editor if there is no cell broadcast message */
              if(tag_zone == NULL)
                  statusCBCH(FALSE);
    #endif
		idle_draw_main_idle();
		break;
//x0pleela 15 Feb, 2006  ER:OMAPS00065203
//Handling MfwWinResume event
	case MfwWinResume: 	/* window is resumed */
		opername_disp_flag = SPN;
		timer_start_flag = FALSE;

		break;
//x0pleela 05 Apr, 2007  ER: OMAPS00122561
//Handling Win suspend event
#ifdef FF_PHONE_LOCK
		case MfwWinSuspend :
			timStop(autoPhLock_timer);
			autoPhLock_timer_flag= FALSE;
			break;

#endif /*FF_PHONE_LOCK*/
		
	case MfwWinFocussed: /* input focus / selected*/
	case MfwWinDelete:/* window will be deleted*/
	default:
		return MFW_EVENT_REJECTED;
}

return MFW_EVENT_CONSUMED;

}


/*******************************************************************************

 $Function:    	keyEvent

 $Description:	 keyboard event handler

 $Returns:

 $Arguments:

*******************************************************************************/

static int idle_kbd_cb (MfwEvt e, MfwKbd *k)
{

	// static UBYTE right_key_pressed=FALSE;  // RAVI
	UBYTE currentRinger = getCurrentRingerSettings();

	T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;
// 	UBYTE Temp_edt_buf[KEYPADLOCK_BUF_SIZE];  // RAVI

//	char debug[50];  // RAVI - Not Used.

	TRACE_FUNCTION("idle_kbd_cb");

	TRACE_EVENT_P1("Key: %d", k->code);
#ifdef NEPTUNE_BOARD
    TRACE_EVENT_P1("pm_duration = %d ", FFS_flashData.pm_duration);
/* Power management */


#ifdef MMI_POWER_MANAGEMENT_TEST

    mmi_pm_enable(0); /*First, disable PM*/
    
    mmi_pm_enable(1); /*then enable it.*/
#endif
#endif
    if (idle_data.starting_up) /*NH Do nothing when starting up */
    	{
		return MFW_EVENT_CONSUMED;
	}

	if (!(e & KEY_MAKE))
	{
    TRACE_EVENT_P1("!(e & KEY_MAKE):::  k->code = %d", k->code);
		switch (k->code)
			{
			case KCD_RIGHT:
				pin_idle_emerg_call = FALSE;
			break;

			// sbh - power key pressed in idle screen, we can shut down
			case KCD_POWER:
				pin_idle_emerg_call = FALSE;
				HUPKeyOrigin = 1;
			break;

			case KCD_LEFT:
			break;

			default:
			break;
			}
		idle_data.right_key_pressed = FALSE;
		
		return MFW_EVENT_CONSUMED;
	}

	idle_data.right_key_pressed = FALSE;

	if(idle_data.missedCalls > 0)
    {
    TRACE_EVENT_P1("idle_data.missedCalls > 0:::  k->code = %d", k->code);
	  switch (k->code)
      {
      	case KCD_MNUSELECT:
	    case KCD_LEFT:
	      idle_data.missedCalls = 0;
	      bookPhonebookStart(idle_data.win,PhbkMissedCallsListNormal);
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

                    /*    mmi_pm_enable(0); */ /* Disable PM*/
#endif
#endif
	    return MFW_EVENT_CONSUMED;
				default:
				break;
			}
	}

#if defined (FF_MMI_EMS) && defined (FF_GPF_TCPIP)
//xrashmic 26 Aug, 2004 MMI-SPR-23931
       // To launch the EMS inbox on pressing the "read" left softkey in idle 
       // screen after receving the new EMS indication in the idle screen.
       if(idle_data.ems)
       {
     TRACE_EVENT_P1("idle_data.ems:::  k->code = %d", k->code);
          switch (k->code)
          {
               case KCD_MNUSELECT:
               case KCD_LEFT:
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

                       /*  mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
                       MMSBox_start(idle_data.win, (MfwMnuAttr*)MMS_INBOX);
                       return MFW_EVENT_CONSUMED;
               default:
                       break;
           }
       }
#endif


	if(LimitedService == 0 && idle_data.search)//  Apr 14, 2004        REF: CRR 12653  xvilliva
	//if (idle_data.search)
    {
    TRACE_EVENT_P1("LimitedService == 0 && idle_data.search:::  idle_data.nm_status = %d", idle_data.nm_status);
	switch (idle_data.nm_status)
    {
		case NETWORK_NO_SERVICE :
        case NETWORK_NO_PLMN_XXX_AVAIL:
                TRACE_EVENT_P1("k->code = %d", k->code);
			switch (k->code)
			{
				case KCD_MNUSELECT:
		        	case KCD_LEFT:
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                               /*     mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
					network_start_full_service();
					winShow(idle_data.win);
				case KCD_HUP:
					idle_data.search=FALSE;
                                   /* On pressing the KCD_HUP key in searching state the menu displays
                                   *'No Network' with 'Name' and 'Menu' as softkeys instead of 
                                   *'Searching'.
                                   */
                                   #ifdef NEPTUNE_BOARD
                                   network_start_full_service();
                                   #endif
					winShow(idle_data.win);
				break;
				/*28th Mar 2007 OMAPS00121870 a0393213(R.Prabakar)
				   "Menu" is mapped to right softkey*/	
 				case KCD_RIGHT:
					bookPhonebookStart(idle_data.win,PhbkMainMenu);	
					break;
				/*CONQ 6436, MC allow entry of emergency number*/
				/*API - 13-09-02 -Add all KCD_0 and KCD_8 */
				/*CQ10659 - API - 20/06/03 - Added KCD_STAR, KCD_HASH, KCD_6*/
				case KCD_0:
				case KCD_1:
				case KCD_2:
				case KCD_6:
				case KCD_8:
				case KCD_9:
				case KCD_STAR:
				case KCD_HASH:
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

                               /*  mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
					memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));
					idle_data.edt_buf[0]=editControls[k->code];
					TRACE_EVENT_P2("Idle_data.edt_buf[0] = %d, [1] = %d", idle_data.edt_buf[0], idle_data.edt_buf[1]);
					idle_dialling_start(idle_data.win,NULL);
					break;

				default:
				break;
			}
			return MFW_EVENT_CONSUMED;

		default:
		break;
	}
    }

	 if (smsidle_get_unread_sms_available()
	 	&& (!idle_data.clean_screen)
	 	&& idle_data.new_sms  )

    {
    TRACE_EVENT_P1(" smsidle_get_unread_sms_available:::  k->code = %d", k->code);

	  switch (k->code)
      {
      	case KCD_MNUSELECT:
	    case KCD_LEFT:
//Apr 05, 2005    REF: ENH 29994 xdeepadh
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// terminate the ringing
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
/* Stop playing ring tone on key press - RAVI  -23-12-2005 */
#ifdef NEPTUNE_BOARD
        currentRinger = getcurrentSMSTone();
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
	      audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif

	 	    vsi_t_sleep(0,10); /* small delay */
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
		audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif /*END RAVI */
#endif

          idle_data.new_sms    = FALSE;
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                      /*  mmi_pm_enable(0);  */ /*Disable PM*/
#endif
#endif
		  /*SPR 2512, removed setting of clean_screen flag here*/
		  SmsRead_R_start(idle_data.win, (MfwMnuAttr *)&sms_list_type);

          return MFW_EVENT_CONSUMED;
				default:
				break;
			}
	}

	//this is for cell broadcast
	if ((!idle_data.clean_screen) && idle_data.new_cbch )
	{
        TRACE_EVENT_P1("if ((!idle_data.clean_screen) && idle_data.new_cbch ) k->code = %d", k->code);    
	  switch (k->code)
      {
      	case KCD_MNUSELECT:
	    case KCD_LEFT:
//Apr 05, 2005    REF: ENH 29994 xdeepadh
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
// terminate the ringing
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
/* Stop Playing Ring tone on Key press - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
             currentRinger = getcurrentSMSTone();
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
  	      audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif

		   vsi_t_sleep(0,10); /* small delay */ 
#else
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
        audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
		audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif
#endif /* END RAVI */
#endif

		  idle_data.new_cbch = FALSE;
          idle_data.clean_screen = TRUE;
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                      /*   mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
		  sms_cb_read_msg (ALL_MESSAGE);
          return MFW_EVENT_CONSUMED;
				default:
				break;
			}
	}



    switch (k->code)
    {
        
				case KCD_MNUUP:
/* xreddymn OMAPS00080543 Jun-06-2006 */
//Fix for 19656
#ifdef NEW_EDITOR
					if((idle_data.editor_sat) && (!idle_data.new_cbch))
					{
						TRACE_EVENT("sat menu up");
						ATB_edit_MoveCursor(idle_data.editor_sat,ctrlUp, TRUE);
						ATB_edit_Show(idle_data.editor_sat);
						idle_show_cb_editor_scrollbar(idle_data.editor_sat);
					}
#else /* NEW_EDITOR */
					if((idle_data.edt_sat) && (!idle_data.new_cbch))
					{
						TRACE_EVENT("CB menu up");
						edtChar(idle_data.edt,ecUp);
					}
#endif /*NEW_EDITOR */
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					else if(idle_data.editor)
					{
						TRACE_EVENT("CB menu up");
						ATB_edit_MoveCursor(idle_data.editor,ctrlUp, TRUE);
						/* xreddymn OMAPS00079699 Jun-02-2006
						 * Display Cell Broadcast message viewer and its scrollbar.
						 */
						ATB_edit_Show(idle_data.editor);
						idle_show_cb_editor_scrollbar(idle_data.editor);
					}
#else /* NEW_EDITOR */
					else if(idle_data.edt)
					{
						TRACE_EVENT("CB menu up");
						edtChar(idle_data.edt,ecUp);
					}
#endif /*NEW_EDITOR */
					else
					{
						/*
							Indicate that the volume settings menu has been invoked from the
							incoming call screen.
//Apr 05, 2005    REF: ENH 29994 xdeepadh						*/
#ifdef FF_MIDI_RINGER  
						IdleScreenVolumeTimer();
#endif
						setSoundsReason(SettingVolume);
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                       /* mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
						idle_data.info_win=bookMenuStart(idle_data.win, ringerVolumeSetting(), SettingVolume);
					}
					break;

				case KCD_MNUDOWN:
/* xreddymn OMAPS00080543 Jun-06-2006 */
//Fix for 19656
#ifdef NEW_EDITOR
					if((idle_data.editor_sat) && (!idle_data.new_cbch))
					{
						TRACE_EVENT("sat menu up");
						ATB_edit_MoveCursor(idle_data.editor_sat,ctrlDown, TRUE);
						ATB_edit_Show(idle_data.editor_sat);
						idle_show_cb_editor_scrollbar(idle_data.editor_sat);
					}
#else /* NEW_EDITOR */
					if((idle_data.edt_sat) && (!idle_data.new_cbch))
					{
						TRACE_EVENT("CB menu up");
						edtChar(idle_data.edt,ecUp);
					}
#endif /*NEW_EDITOR */
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					else if(idle_data.editor)
					{
						TRACE_EVENT("CB menu down");
						ATB_edit_MoveCursor(idle_data.editor,ctrlDown, TRUE);
						/* xreddymn OMAPS00079699 Jun-02-2006
						 * Display Cell Broadcast message viewer and its scrollbar.
						 */
						ATB_edit_Show(idle_data.editor);
						idle_show_cb_editor_scrollbar(idle_data.editor);
					}
#else /* NEW_EDITOR */
					else if(idle_data.edt)
					{
						TRACE_EVENT("CB menu down");
						edtChar(idle_data.edt,ecDown);
					}
#endif /*NEW_EDITOR */
					else
					{
						/*
							Indicate that the volume settings menu has been invoked from the
							incoming call screen.
						*/
//Apr 05, 2005    REF: ENH 29994 xdeepadh						
#ifdef FF_MIDI_RINGER  
						IdleScreenVolumeTimer();
#endif
						setSoundsReason(SettingVolume);
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                      /*  mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
						idle_data.info_win=bookMenuStart(idle_data.win, ringerVolumeSetting(), SettingVolume);
					}
					break;
				// break;  // RAVI
				case KCD_MNUSELECT:
				case KCD_LEFT:                  /* main menu                */
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                        /* mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
/* Stop Playing Audio on Key press - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
				  if (idle_data.new_sms == TRUE)
				  {
				/*Apr 05, 2005    REF: ENH 29994 xdeepadh	*/
				/*Terminate ringing */
#ifdef FF_MIDI_RINGER  
					mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
			              currentRinger = getcurrentSMSTone();
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
                                  audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
					audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif

					vsi_t_sleep(0,10);  /* Small Delay */
#endif					
				  }
#else 
/* Apr 05, 2005    REF: ENH 29994 xdeepadh	 */
/* Terminate ringing */
#ifdef FF_MIDI_RINGER  
					mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#endif
#endif
/* END RAVI - 23-12-2005 */

					
					bookPhonebookStart(idle_data.win,PhbkNormal);
				break;
				case KCD_CALL:
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                      /*  mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
					bookPhonebookStart(idle_data.win,PhbkRedialListNormal);
				break;
				case KCD_RIGHT:                 /* contacts (ADN)           */
           			/* SPR#1449 - SH - Remove, as terminates GPRS connection.
            		 * Seems to be unnecessary. */
             		/*cm_force_disconnect();
					 terminate the ringing */
/*				     audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
*/			  
/* Stop playing audio on key press - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
				  if (idle_data.new_sms == TRUE)
				  {
				/* Apr 05, 2005    REF: ENH 29994 xdeepadh	*/
				/* Terminate ringing */
#ifdef FF_MIDI_RINGER  
					mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#else
			             currentRinger = getcurrentSMSTone();
#ifdef FF_MMI_RINGTONE  /*Stop MSL */
                                   audio_StopSoundbyID( AUDIO_BUZZER,  SMSTONE_SELECT);
#else
					audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif

					vsi_t_sleep(0,10);  /* Small delay */
#endif					
				  }
#else 
/* Apr 05, 2005    REF: ENH 29994 xdeepadh	*/
/* Terminate ringing */
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#endif
#endif
/* END RAVI - 23-12-2005 */
/* Power management */
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
                      /*  mmi_pm_enable(0); */ /*Disable PM*/
#endif
#endif
					 bookPhonebookStart(idle_data.win,PhbkMainMenu);
				break;
				case KCD_HUP:
//Apr 05, 2005    REF: ENH 29994 xdeepadh
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//Terminate ringing
#ifdef FF_MIDI_RINGER  
	mfw_ringer_stop(sounds_midi_ringer_stop_cb);
#endif

					/*
					** NDH : 15/05/2003 : If the Idle editor is being displayed the only way to
					** remove it is by pressing the Hangup Key, if it is active, do not set
					** "Clean Screen" so that the previous "Missed Calls" etc will be displayed
					*/
					if (!idle_data.editor)
					{
					  //for clean the DISPLAY ZONE 3
					  //information like "missed calls" and "receive messages"
					  //disappear if the user press the HUP key
						  idle_data.clean_screen = TRUE;
					}

					
 				       /* delete the message buffer  plus the editor */
  					statusCBCH(FALSE);
					windowsUpdate();
			break;

			// sbh - power key pressed in idle screen, we can shut down
			case KCD_POWER:
				HUPKeyOrigin = 1;
			break;

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
					memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));
					idle_data.edt_buf[0]=editControls[k->code];
					idle_dialling_start(idle_data.win,NULL);
					break;

				default:
				break;
		}
//Jun 24, 2005 MMI-FIX-30973 x0018858
//Added code to handle the display of homezone/cityzone tag.
//Begin 30973
//Check to see if there are any new sms/ missed calls and confirm that the screen is clean.
	if ( ((smsidle_get_unread_sms_available() == 0)
	 		 	&& ( idle_data.new_sms <= 0) && ( idle_data.missedCalls == 0)) || (idle_data.clean_screen))
		{
    TRACE_EVENT_P1(" smsidle_get_unread_sms_available() == 0:::  tag_zone = %d", tag_zone );
			if (tag_zone != NULL)
			{

				addCBCH((char*)tag_zone, IdleNewCBImmediateMessage);
				winShow(idle_data.win);

			}
		}
//End 30973	

		return MFW_EVENT_CONSUMED;
}





/*******************************************************************************

 $Function:    	idle_kbd_long_cb

 $Description:	 Creation of an instance for the Idle dialog.

 $Returns:

 $Arguments:

*******************************************************************************/


static int  idle_kbd_long_cb(MfwEvt e, MfwKbd *k)
{

TRACE_FUNCTION ("idle_kbd_long_cb");
	if ((e & KEY_RIGHT) && (e & KEY_LONG) && (idle_data.right_key_pressed == TRUE))
	{
	}

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	idle_get_window

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




MfwHnd idle_get_window (void)
{
	return idle_data.win;
}

/*******************************************************************************

 $Function:    	idle_set_starting_up

 $Description: Sets a flag that indicates to Idle that the phone is starting up

 $Returns:

 $Arguments:

*******************************************************************************/




void idle_set_starting_up (UBYTE set)
{
	idle_data.starting_up = set;
}


/*******************************************************************************

 $Function:    	idleIsFocussed

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




int idleIsFocussed(void)
{

	if (winIsFocussed(idle_data.win))
	{
		TRACE_EVENT("idleIsFocussed");
	    return 1;
	}
	else
		return 0;


}

/*******************************************************************************

 $Function:    	addMissedCall

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




void addMissedCall(void)
{
	idle_data.missedCalls++;

    // show the info about missed calls on the idle screen
	idle_data.clean_screen = FALSE;
}
/*******************************************************************************

 $Function:    	class0_editor_cb  Added for issue 2512

 $Description:	 CAllback for the class 0 editor.  frees memory

 $Returns:		none

 $Arguments:	window(cast to string pointer), id, reason

*******************************************************************************/
void class0_editor_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{	U8* message_buffer;
	/*cast the window pointer to string buffer*/
	message_buffer = (U8*)win;
	/*free the memory*/
	FREE_MEMORY(message_buffer, MAX_EDITOR_LEN);
	message_buffer = NULL;
	 // July 12, 2006    REF:DR OMAPS00081477  x0047075
        //Fix:Assigning the sms_status_editor window handle to NULL
#ifdef NEW_EDITOR
	idle_data.sms_status_editor = NULL; 
#else/* NEW_EDITOR */
      idle_data.sms_status_edt = NULL; 
#endif /* NEW_EDITOR */
	/*redraw the windows*/
	windowsUpdate();


}
/*******************************************************************************

 $Function:    	addCBCH

 $Description:	 this indicates the incoming cell-info cellbroadcast
 				or incoming sms class 0 message
                The Status tells about to show or not on the Idle screen

 $Returns:

 $Arguments:

*******************************************************************************/
void addCBCH(char* cbch_string, UBYTE type)
{
/*SPR 2512*/
#ifdef NEW_EDITOR
		T_AUI_EDITOR_DATA editor_data;
#else /* NEW_EDITOR */
		T_EDITOR_DATA editor_data;
#endif /* NEW_EDITOR */
/*SPR 2512, end*/
	switch(type)
	{
		case IdleNewCBImmediateMessage:
			if(cbch_string NEQ NULL)
			{
				memset(idle_data.incoming_cb_msg, '\0',MAX_CBMSG_LEN);
				strncpy((char *)idle_data.incoming_cb_msg,  (char *)cbch_string, MAX_CBMSG_LEN);

//Jun 24, 2005 MMI-FIX-30973 x0018858
// Copy the tag string on to a global variable for display later.
//Begin 30973
				memset(tag_zone, '\0',MAX_CBMSG_LEN);
				strncpy((char *)tag_zone,  (char *)idle_data.incoming_cb_msg, MAX_CBMSG_LEN);
				tag_zone[MAX_CBMSG_LEN -1]= '\0';
//End 30973
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				if (!idle_data.editor)
				{
			       	idle_data.editor = ATB_edit_Create (&idle_data.editor_attr2,0);
				}
				/* xreddymn OMAPS00079699 Jun-02-2006
				 * Do not re-initialize editor if it already exists.
				 */ 
				else break;
#else /* NEW_EDITOR */
				if (!idle_data.edt)
				{
			       	idle_data.edt = edtCreate  (idle_data.win,&idle_data.edt_attr2,0,0);
				}
				/* xreddymn OMAPS00079699 Jun-02-2006
				 * Do not re-initialize editor if it already exists.
				 */ 
				else break;
#endif /* NEW_EDITOR */

				//this is centralize the cb message if it's a short one
				//12 cos CB messages have some space character after the string
				//it is just a estimate
				/* SPR#1428 - SH - New Editor changes.  Use centralise
				 * editor feature. */
#ifdef NEW_EDITOR
				if ((strlen(cbch_string)) < 12)
					idle_data.editor_attr2.startFormat.attr |= DS_ALIGN_CENTRE;
				ATB_edit_Init(idle_data.editor);
#else /* NEW_EDITOR */
				if ((strlen(cbch_string)) < 12)
					idle_data.edt_attr2.win.px = WIN_CENTRALIZE_ORG_X;
				else
					idle_data.edt_attr2.win.px = WIN_DIALLING_ORG_X;
#endif /*NEW_EDITOR*/
			}
			else
			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				if (idle_data.editor)
				{
					ATB_edit_Destroy(idle_data.editor);
				}

				idle_data.editor_attr2.startFormat.attr = 0;

				idle_data.editor = 0;
#else /* NEW_EDITOR */
				if (idle_data.edt)
				{
					edtDelete(idle_data.edt);
				}

				//set the centralize for the cb message to default
				idle_data.edt_attr2.win.px = WIN_DIALLING_ORG_X;

				idle_data.edt = 0;
#endif /*NEW_EDITOR */
				memset(idle_data.incoming_cb_msg, '\0',MAX_CBMSG_LEN);
			}
			break;

		case IdleNewCBNormalMessage:

			//set back to default
	        idle_data.clean_screen = FALSE;

			idle_data.new_cbch = TRUE;
			break;

		/*NM p019*/
		case IdleNewSmsMessage:
/*SPR 2512, show class 0 message as a pop-up*/
#ifdef NEW_EDITOR
         // July 12, 2006    REF:DR OMAPS00081477  x0047075
        //Fix:Creating a new sms status editor, if it already exists just update the editor
		if(idle_data.sms_status_editor == NULL) 
        	{
                            AUI_edit_SetDefault(&editor_data);
				AUI_edit_SetDisplay(&editor_data, ZONE_FULLSOFTKEYS, COLOUR_EDITOR_XX, EDITOR_FONT);
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)cbch_string, strlen(cbch_string));
				AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
				AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtNull, TxtNull, NULL);
				AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)class0_editor_cb);
				/*this casting of the message buffer allows it to be passed back via the editor callback function
				so we can free the memory when it's no longer needed.  Yes, I know it's dodgy.*/

        // July 12, 2006    REF:DR OMAPS00081477  x0047075
        //Fix:Creating a new sms status editor, if it already exists just update the editor
				idle_data.sms_status_editor = AUI_edit_Start((T_MFW_HND)cbch_string, &editor_data);
        	}
	      else
	      	{
	          winShow (idle_data.sms_status_editor); 
	      	}	  
		

#else
       // July 12, 2006    REF:DR OMAPS00081477  x0047075
       //Fix:Creating a new sms status editor, if it already exists just update the editor
		if(idle_data.sms_status_edt == NULL) 
		{
                            editor_data.editor_attr.win.px = 0;
				editor_data.editor_attr.win.py = 0;
				editor_data.editor_attr.win.sx =
				    SCREEN_SIZE_X-editor_data.editor_attr.win.px;
				editor_data.editor_attr.win.sy = SCREEN_SIZE_Y-2*Mmi_layout_first_line();
				editor_data.editor_attr.edtCol	= COLOUR_EDITOR_XX,
				editor_data.editor_attr.font     = 0;
				editor_data.editor_attr.mode    = edtCurNone;
				editor_data.editor_attr.controls   = 0;
				editor_data.hide			    = FALSE;

				editor_data.editor_attr.text	= (char*)cbch_string;

				editor_data.editor_attr.size	= strlen(cbch_string);
				editor_data.LeftSoftKey			= TxtSoftOK;
				editor_data.RightSoftKey		= TxtNull;
				editor_data.AlternateLeftSoftKey     = TxtNull;
				editor_data.TextString			= '\0';
				editor_data.TextId				= TxtNull;
				editor_data.Identifier			= NULL;
				editor_data.mode				= READ_ONLY_MODE;
				editor_data.timeout		        = FOREVER;
				editor_data.min_enter		    = 1;
				editor_data.Callback		    = (T_EDIT_CB)NULL;
				editor_data.destroyEditor	    = TRUE;
                        // July 12, 2006    REF:DR OMAPS00081477  x0047075
                       //Fix:Creating a new sms status editor, if it already exists just update the editor
			      idle_data.sms_status_edt = editor_start(idle_get_window(), &editor_data);
		}
		else
		{
			winShow (idle_data.sms_status_edt); 
		}

 #endif
			break;

	}


}

/*******************************************************************************

 $Function:    	statusCBCH

 $Description:	if True -> keep it on the idle screen
                if FALSE -> remove the message (CB or SMS)
                           and destroy the editor

 $Returns:

 $Arguments:

*******************************************************************************/

void statusCBCH(UBYTE status)
{
	if(status)
	{

		windowsUpdate();

	}
	else
	{

		//Dont show the CB on the Idle screen

		idle_data.new_cbch = FALSE;
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		if (idle_data.editor)
			ATB_edit_Destroy(idle_data.editor);
		idle_data.editor = 0;

		//set the centralize for the cb message to default
		idle_data.editor_attr2.startFormat.attr = 0;

#else /* NEW_EDITOR */
		if (idle_data.edt)
			edtDelete(idle_data.edt);
		idle_data.edt = 0;

		//set the centralize for the cb message to default
		idle_data.edt_attr2.win.px = WIN_DIALLING_ORG_X;
#endif /*NEW_EDITOR */
	}


}


//xashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
/*******************************************************************************

 $Function:    	mmi_usb_display_info

 $Description:	 information dialog

 $Returns:		none

 $Arguments:	parent_win 	-	parent window
			lsk			-	left softkey lable
			rsk			-	Right softkey lable
			str1			-	String ID1
			str1			-	String ID2
*******************************************************************************/
T_MFW_HND mmi_usb_display_info(T_MFW_HND parent_win,int lsk, int rsk, int str1, int str2, int timer)
{
	T_MFW_HND win;
	T_DISPLAY_DATA   display_info;
	TRACE_FUNCTION("mmi_usb_display_info()");
	dlg_initDisplayData_TextId( &display_info, lsk, rsk, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, NULL, timer, 0 );
	win=info_dialog (parent_win, &display_info);			
	return win;
}	

/*******************************************************************************

 $Function:    	mmi_usbms_mode_disconnect

 $Description:	 Disconnect event handler

 $Returns:		none

 $Arguments:	 none

*******************************************************************************/
void mmi_usbms_mode_disconnect(void)
{	
	T_MFW_HND parent_win = mfwParent(mfw_header());
	TRACE_FUNCTION("mmi_usbms_mode_disconnect()");

	//xashmic 27 Sep 2006, OMAPS00096389 
	//xashmic 21 Sep 2006, OMAPS00095831
	//Skip the disconnect dialog display when ports are enumerated automatically
	if( getBootUpState() > 0 && (getUSBMSFlags() & MFW_USBMS_POPUP_ENABLE))
		mmi_usb_display_info(parent_win,TxtNull,TxtNull,TxtUSBDisconnect,TxtUSB,THREE_SECS);
	if(usb_opt_win)
	{
		//User has not selected any option, destroy the menu
		bookMenuDestroy(usb_opt_win);
		usb_opt_win=NULL;
		//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
		//Stop the timer and delete it. 
		if(usb_auto_enum_timer)
		{
			timStop(usb_auto_enum_timer);
			timDelete(usb_auto_enum_timer);
			usb_auto_enum_timer=NULL;
		}
	}
	//If PS was shutdown, enable it
	if(USB_Mode==TRUE)
	{
		sim_enable();
		SEND_EVENT(usb_dialog_win,DIALOG_DESTROY,0,0);
	}
	USB_Mode= FALSE;
}

/*******************************************************************************

 $Function:    	mmi_usbms_mode_connect

 $Description:	 Connect event handler

 $Returns:		none

 $Arguments:	usbms_enum		-	contains information as to proceed with enumeration or to 
 								discontinue enumeration: MFW_USB_ENUM or MFW_USB_NO_ENUM
			usb_enum_type	-	Contains information about which all port to be enumerated
*******************************************************************************/
void mmi_usbms_mode_connect(T_MFW_USB_ENUM_CONTROL usbms_enum,T_MFW_USB_ENUM_TYPE usb_enum_type)
{	
	T_MFW_HND parent_win = mfwParent(mfw_header());
	TRACE_FUNCTION("mmi_usbms_mode_connect()");
	
	//xashmic 27 Sep 2006, OMAPS00096389
	//When enumerating USB MS individually or with a combination of other ports, 
	if( (usbms_enum == MFW_USB_ENUM) && 
	( (  usb_enum_type == MFW_USB_TYPE_MS) || 	(usb_enum_type == MFW_USB_TYPE_AT_MS) ||
	( usb_enum_type == MFW_USB_TYPE_MS_TRACE) || (usb_enum_type == MFW_USB_TYPE_ALL) ) )
	{
		//For USBMS enumeration, if PS Shutdown option is set to TRUE, disable the PS
		if(FFS_flashData.usb_ms_flags & MFW_USBMS_PS_SHUTDOWN)
		{
			sim_disable();
		   	USB_Mode= TRUE;
			idle_set_starting_up(TRUE);
	     	      //When PS shutdown is enabled, on enumerating USB MS, user is retricted for accessing any menu
	   		usb_dialog_win=mmi_usb_display_info( parent_win, TxtNull , TxtNull, TxtUSBMode,  TxtNull,FOREVER );
	}
		else
			mmi_usb_display_info( parent_win, TxtNull , TxtNull, TxtUSBEnumeration,  TxtPleaseWait,THREE_SECS );
	}
	else
		mmi_usb_display_info( parent_win, TxtNull , TxtNull, TxtUSBEnumeration,  TxtPleaseWait,THREE_SECS );
	mfw_usbms_enum_control( usbms_enum,usb_enum_type );
}

/*******************************************************************************

 $Function:    	mmi_usb_enumeration_options_cb

 $Description:	 Event handler for the option screen ( contains list of ports for enumeration on connect event)

 $Returns:		none

 $Arguments:	parent_win	-	Parent Window
 			Identifier		-	
 			Reason		-	

*******************************************************************************/
void mmi_usb_enumeration_options_cb(T_MFW_HND parent_win, USHORT identifier, SHORT reason)
{
	TRACE_FUNCTION("mmi_usb_enumeration_options_cb()");

	mfw_usbms_enum_control(MFW_USB_NO_ENUM,MFW_USB_TYPE_ALL);
	usb_opt_win=NULL;

	//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
	//Stop the timer and delete it. User has pressed back in the options menu
	if(usb_auto_enum_timer)
	{
		timStop(usb_auto_enum_timer);
		timDelete(usb_auto_enum_timer);
		usb_auto_enum_timer=NULL;
	}
}
/*******************************************************************************

 $Function:    	mmi_usb_enumeration_timer_cb

 $Description:	 Event handler for the timer in option screen 

 $Returns:		none

 $Arguments:	parent_win	-	Parent Window
 			Identifier		-	
 			Reason		-	
//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
*******************************************************************************/
static void mmi_usb_enumeration_timer_cb(T_MFW_HND parent_win, USHORT identifier, SHORT reason)
{

	T_MFW_HND      window		= mfwParent( mfw_header() );
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *)window)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;
	MfwMnu			*mnu;
	
	TRACE_FUNCTION("mmi_usb_enumeration_timer_cb()");
    if(usb_opt_win && data->menu==((tBookStandard*)(((MfwWin*)(((T_MFW_HDR *)usb_opt_win)->data))->user))->menu) // OMAPS00149237 Added by Sameer & Geetha
    {
    	mnuSelect( data->menu ); //OMAPS00149237
		bookMenuDestroy(usb_opt_win);
		usb_opt_win=NULL;			
	}
	
	//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
	//Timer has expired, need to enumerate all ports automatically
	if(usb_auto_enum_timer)
	{
		timDelete(usb_auto_enum_timer);
		usb_auto_enum_timer=NULL;
	}
	mmi_usbms_mode_connect(MFW_USB_ENUM,MFW_USB_TYPE_ALL);
}

/*******************************************************************************

 $Function:    	mmi_usb_enumeration_options

 $Description:	 Displays a list of ports that can be enumerated

 $Returns:		none

 $Arguments:		none

*******************************************************************************/
void mmi_usb_enumeration_options(void)
{
	TRACE_FUNCTION("mmi_usb_enumeration_options()");
	
	usb_opt_win=bookMenuStart(idle_data.win, USBMenuAttributes(),0);
	//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
	//Enumerate all the ports if user does not select any option/exit this menu within 5 Sec
	usb_auto_enum_timer=timCreate(usb_opt_win, AUTO_USB_ENUMERATION_TIMER, (MfwCb)mmi_usb_enumeration_timer_cb);
	timStart(usb_auto_enum_timer);
	
	SEND_EVENT(usb_opt_win, ADD_CALLBACK, NULL, (void *)mmi_usb_enumeration_options_cb); 
}

/*******************************************************************************

 $Function:    	setUSBMS

 $Description:	 Updates the FFs with user selected option for PS shutdwon on / off

 $Returns:		none

 $Arguments:		m 	- pointer to current menu
 				i 	- pointer to current menu item

xashmic 27 Sep 2006, OMAPS00096389 
*******************************************************************************/
GLOBAL int setUSBMSPS (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	
	TRACE_FUNCTION("setUSBMSPS()");
	switch (m->lCursor[m->level])
	{
		case 0:
			//Turn on the last bit in the byte
			FFS_flashData.usb_ms_flags = FFS_flashData.usb_ms_flags|MFW_USBMS_PS_SHUTDOWN;
			break;
		case 1:
		default:
			//Turn off the last bit in the byte
			FFS_flashData.usb_ms_flags = FFS_flashData.usb_ms_flags & MFW_USBMS_PS_NO_SHUTDOWN;

	}
	TRACE_EVENT_P1(" %x",FFS_flashData.usb_ms_flags);
	mmi_usb_display_info(parent_win,TxtNull, TxtNull, TxtSaved, TxtNull,THREE_SECS);
	flash_write();
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	setUSBMSPopup

 $Description:	 Updates the FFs with user selected option for poping up menu on connect event

 $Returns:		none

 $Arguments:		m 	- pointer to current menu
 				i 	- pointer to current menu item
xashmic 27 Sep 2006, OMAPS00096389 
*******************************************************************************/
GLOBAL int setUSBMSPopup (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	
	TRACE_FUNCTION("setUSBMSPopup()");
	switch (m->lCursor[m->level])
	{
		case 0:
			FFS_flashData.usb_ms_flags = FFS_flashData.usb_ms_flags | MFW_USBMS_POPUP_ENABLE;
			break;
		case 1:
		default:
			FFS_flashData.usb_ms_flags = FFS_flashData.usb_ms_flags & MFW_USBMS_POPUP_DISABLE;	

	}
	TRACE_EVENT_P1(" %x",FFS_flashData.usb_ms_flags);
	mmi_usb_display_info(parent_win,TxtNull, TxtNull, TxtSaved, TxtNull,THREE_SECS);
	flash_write();
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_usb_enumeration

 $Description:	  Based on the user selection the USB ports are enumerated

 $Returns:		none

 $Arguments:		m 	- pointer to current menu
 				i 	- pointer to current menu item
*******************************************************************************/
GLOBAL int mmi_usb_enumeration (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND parent_win = mfwParent(mfw_header());
	T_MFW_USB_ENUM_CONTROL usbms_enum=MFW_USB_NO_ENUM;
	T_MFW_USB_ENUM_TYPE usb_enum_type=MFW_USB_TYPE_ALL;
	TRACE_FUNCTION("mmi_usb_enumeration()");
	//xashmic 14 Sep 2006, OMAPS00092732 HOT_FIX
	//Stop the timer and delete it. User has selected a option
	if(usb_auto_enum_timer)
	{
		timStop(usb_auto_enum_timer);
		timDelete(usb_auto_enum_timer);
		usb_auto_enum_timer=NULL;
	}
	
	usbms_enum=MFW_USB_ENUM;
	switch (m->lCursor[m->level])
	{
		case 0:
			TRACE_EVENT("USB MS");
			usb_enum_type=MFW_USB_TYPE_MS;
			break;
		case 1:
			TRACE_EVENT("USB TRACE");
			usb_enum_type=MFW_USB_TYPE_TRACE;
			break;
		case 2:
			TRACE_EVENT("USB FAX");
			usb_enum_type=MFW_USB_TYPE_AT;
			break;
		case 3://xashmic 27 Sep 2006, OMAPS00096389 
			TRACE_EVENT("USB MS FAX n TRACE");
			usb_enum_type=MFW_USB_TYPE_AT_TRACE;
			break;			
		case 4://xashmic 27 Sep 2006, OMAPS00096389 
			TRACE_EVENT("USB MS n TRACE");
			usb_enum_type=MFW_USB_TYPE_MS_TRACE;
			break;			
		case 5://xashmic 27 Sep 2006, OMAPS00096389 
			TRACE_EVENT("USB FAX n MS");
			usb_enum_type=MFW_USB_TYPE_AT_MS;
			break;			
		case 6://xashmic 27 Sep 2006, OMAPS00096389 
			TRACE_EVENT("USB ALL");
			usb_enum_type=MFW_USB_TYPE_ALL;
			break;			
	}
	/*OMAPS00123019 a0393213(R.Prabakar)
	   The menu is not destroyed here. It would be destroyed in the key board handler*/
			mmi_usbms_mode_connect(usbms_enum,usb_enum_type);
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	getUSBMSFlags

 $Description:	  return the current USB flags

 $Returns:	USB MS flags

 $Arguments:		None
//xashmic 27 Sep 2006, OMAPS00096389 
*******************************************************************************/
UBYTE getUSBMSFlags(void)
{
	TRACE_EVENT_P1("getUSBMSFlags %x",FFS_flashData.usb_ms_flags);
	return FFS_flashData.usb_ms_flags;
}
#endif	 


/*******************************************************************************
 $Function:    	addSatMessage

 $Description:	Store a SAT message to display on the idle screen.
 				SPR759 - SH.

 $Returns:

 $Arguments:	satMessage	- the text of the message

*******************************************************************************/

// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
void addSatMessage(char *satMessage, UBYTE width, UBYTE height,
						char *dst, UBYTE selfExplanatory, T_DISPLAY_TYPE display_type)
#else						
void addSatMessage(char *satMessage)
#endif
{
// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
#endif

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	if(satMessage NEQ NULL)
	{
		if(!idle_data.editor_sat)
		{
			idle_data.editor_sat = ATB_edit_Create(&idle_data.editor_attr_sat,0);  /* Create the editor to hold it */
		}

// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
		if(selfExplanatory == FALSE)
		{
		    /* xreddymn OMAPS00080543 Jun-06-2006 */
			memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
			strncpy((char *)idle_data.incoming_sat_msg, (char *)satMessage, MAX_SATMSG_LEN);

			/* Centralize sat message if below 12 characters (so it doesn't look odd on the screen) */

			if ((strlen(satMessage)) < 12)
				idle_data.editor_attr_sat.startFormat.attr |= DS_ALIGN_CENTRE;
		}
		else
		{
			/* Icon is self-explanatory. No need to display text for this case.
			    Fill the idle text buffer with NULL*/
            /* xreddymn OMAPS00080543 Jun-06-2006 */
			memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
		}

		/* Copy the data to the Editor title field & ATB editor will 
 		    take care of displaying the Icon */
		if(dst != NULL)
		{
			idle_data.editor_sat->attr->TitleIcon.width = width;
			idle_data.editor_sat->attr->TitleIcon.height = height;

			icon_length = width * height;
										
			idle_data.editor_sat->attr->TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(idle_data.editor_sat->attr->TitleIcon.data, dst, icon_length);
			idle_data.editor_sat->attr->TitleIcon.display_type = display_type; //x0035544 02-Feb-2006 DR:61468
			idle_data.editor_sat->attr->TitleIcon.isTitle = FALSE;
			
		}		
#else
            /* xreddymn OMAPS00080543 Jun-06-2006 */
			memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
			strncpy((char *)idle_data.incoming_sat_msg, (char *)satMessage, MAX_SATMSG_LEN);

			/* Centralize sat message if below 12 characters (so it doesn't look odd on the screen) */

			if ((strlen(satMessage)) < 12)
				idle_data.editor_attr_sat.startFormat.attr |= DS_ALIGN_CENTRE;
	
#endif		

		ATB_edit_Init(idle_data.editor_sat);

	}
	else		/* If a null string is passed, get rid of the message */
	{
		if (idle_data.editor_sat)
		{ 
		    // 02-Feb-2006, Sudha.V., x0035544
   #ifdef FF_MMI_SAT_ICON
		/* Check if a Idle mode text malloc was done for icon support, if Yes, free it*/
		if (idle_data.editor_sat->attr->TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)idle_data.editor_sat->attr->TitleIcon.data, idle_data.editor_sat->attr->TitleIcon.width * 
								idle_data.editor_sat->attr->TitleIcon.height);
			idle_data.editor_sat->attr->TitleIcon.data = NULL;
		}
   #endif
			ATB_edit_Destroy(idle_data.editor_sat);
		}

		//set the centralize for the cb message to default
		idle_data.editor_attr_sat.startFormat.attr = 0;

		idle_data.editor_sat = NULL;
        /* xreddymn OMAPS00080543 Jun-06-2006 */
		memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
	}
#else /* NEW_EDITOR */
	if(satMessage NEQ NULL)
	{
		if(!idle_data.edt_sat)
		{
			idle_data.edt_sat = edtCreate(idle_data.win,&idle_data.edt_attr_sat,0,0);  // Create the editor to hold it
		}
        /* xreddymn OMAPS00080543 Jun-06-2006 */
		memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
		strncpy((char *)idle_data.incoming_sat_msg, (char *)satMessage, MAX_SATMSG_LEN);

		//Centralize sat message if below 12 characters (so it doesn't look odd on the screen)

		if ((strlen(satMessage)) < 12)
			idle_data.edt_attr_sat.win.px = (SCREEN_SIZE_X-strlen(idle_data.edt_attr_sat.text)*6)/2;
		else
			idle_data.edt_attr_sat.win.px = WIN_DIALLING_ORG_X;
	}
	else		// If a null string is passed, get rid of the message
	{
		if (idle_data.edt_sat)
		{
			edtDelete(idle_data.edt_sat);
		}

		//set the centralize for the cb message to default
		idle_data.edt_attr_sat.win.px = WIN_DIALLING_ORG_X;

		idle_data.edt_sat = NULL;
        /* xreddymn OMAPS00080543 Jun-06-2006 */
		memset(idle_data.incoming_sat_msg, '\0', MAX_SATMSG_LEN);
	}
#endif /* NEW_EDITOR */
	return;
}

/*******************************************************************************

 $Function:    	idleDialBuffer

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idleDialBuffer(char* dialBuffer)
{
	memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));
	strncpy((char*)idle_data.edt_buf,(char*)dialBuffer,sizeof(idle_data.edt_buf)-1);
	idle_dialling_start(idle_data.win,NULL);
}


/*******************************************************************************

 $Function:    	windowsUpdate

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void windowsUpdate(void)
{
	TRACE_FUNCTION("Idle_update()");
	winShow(idle_data.win);


}



/*******************************************************************************

 $Function:    	idle_dialling_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


T_MFW_HND idle_dialling_create (T_MFW_HND parent_window)
{
    T_MFW_WIN     * win;

    /*
     * This window is dynamic, for that reason the associated data are allocated in the mfw heap
     */

	T_idle_dialling *  data = (T_idle_dialling *)ALLOC_MEMORY (sizeof (T_idle_dialling));

	TRACE_FUNCTION ("idle_dialling_create");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)idle_dialling_win_cb);

    if (data->win EQ 0)
      return 0;

	 /*
     * These assignments are necessary to attach the data to the window, and to handle the mmi event communication.
     */

	data->mmi_control.dialog    = (T_DIALOG_FUNC)idle_dialling;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
    * Create any other handler
    */
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	data->editor	= ATB_edit_Create(&idle_data.editor_attr, 0);
#else /* NEW_EDITOR */
	data->edt      = edtCreate  (data->win,&idle_data.edt_attr,0,0);
#endif /* NEW_EDITOR */
    data->kbd      = kbd_create (data->win,KEY_ALL,(T_MFW_CB)idle_dialling_kbd_cb);
    data->kbd_long = kbd_create (data->win,KEY_ALL|KEY_LONG,(T_MFW_CB)idle_dialling_kbd_long_cb);


  //winShow(data->win);
  /*
   * return window handle
   */
  return data->win;
}



/*******************************************************************************

 $Function:    	idle_dialling_destroy

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



void idle_dialling_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_idle_dialling        * data;

	TRACE_FUNCTION("idle_dialling_destroy");

	/*SPR 2500*/
  win  = ((T_MFW_HDR *)own_window)->data;
  data = (T_idle_dialling *)win->user;

#ifdef NEPTUNE_BOARD
  memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));   /*  OMAPS00033660   */
#endif

  if ((own_window) &&
  	(win) &&
  	(data))
    {    
        /* xreddymn OMAPS00075852 May-15-2006
         * When destroying the dialer, set the dialer_visible flag to FALSE.
         */
        idle_data.dialer_visible = FALSE;

      /*
       * Exit Keyboard Handler
       */
      /*NM, 26.3.02*/
      /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
      ATB_edit_Destroy(data->editor);
#else/* NEW_EDITOR */
	  edt_delete(data->edt);
#endif /* NEW_EDITOR */
      kbd_delete(data->kbd);
      kbd_delete(data->kbd_long);
      /*
       * Delete WIN Handler
       */
      win_delete (data->win);

      /*
      *  In this case the data attached to window must be also deleted.
      */
   	FREE_MEMORY ((void *)data, sizeof (T_idle_dialling));
    }
    else
    {/*SPR2500*/
        TRACE_ERROR("idle_dialling_destory : Invalid pointer");
    }

}


/*******************************************************************************

 $Function:    	idle_dialling_start

 $Description:	 This function just creates and inits the new dialog

 $Returns:

 $Arguments:

*******************************************************************************/




T_MFW_HND idle_dialling_start (T_MFW_HND win_parent,char *character)
{


  T_MFW_HND win = idle_dialling_create (win_parent);

  TRACE_FUNCTION("idle_dialling_start");

  /* xreddymn OMAPS00075852 May-15-2006
   * When creating the dialer, set the dialer_visible flag to TRUE.
   */
  idle_data.dialer_visible = TRUE;

  if (win NEQ NULL)
  {
		SEND_EVENT(win,IDLE_DIALLING_INIT,0,(char*)character);
  }
  return win;

}



/*******************************************************************************

 $Function:    	idle_dialling

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



void idle_dialling (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_idle_dialling         * data = (T_idle_dialling *)win_data->user;

    TRACE_FUNCTION ("idle_dialling()");
   /*
   *  In this case the communication is very simple (only one intern event)
   */

	//GW Use macros for screen size
    switch (event)
	{
		case IDLE_DIALLING_INIT:
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetAttr(&idle_data.editor_attr, WIN_DIALLING, COLOUR_EDITOR_XX, EDITOR_FONT, 0, ED_CURSOR_UNDERLINE, ATB_DCS_ASCII, (UBYTE*)idle_data.edt_buf,EDITOR_SIZE);

			ATB_edit_Init(data->editor);

			ATB_edit_Unhide(data->editor);
			ATB_edit_Char(data->editor, ctrlBottom, TRUE);
#else /* NEW_EDITOR */
			bookSetEditAttributes(WIN_DIALLING,COLOUR_EDITOR_XX,0,edtCurBar1,0,(char*)idle_data.edt_buf,EDITOR_SIZE,&idle_data.edt_attr);

			/* NM 13.03 fast-edit */
			editActivate (data->edt,FALSE);

			edtUnhide(data->edt);
			edtChar(data->edt,ecBottom);
#endif /* NEW_EDITOR */
			winShow(data->win);
		break;
	    default:
		return;
	}

}



/*******************************************************************************

 $Function:    	idle_dialling_win_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static int idle_dialling_win_cb (MfwEvt evt, MfwWin *win)
{
    T_idle_dialling         * data = (T_idle_dialling *)win->user;

    TRACE_FUNCTION ("idle_dialling_win_cb()");
	switch (evt)
    {
        case MfwWinVisible:
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			/* If update is trivial, don't update what's underneath */
			if (data->editor->update!=ED_UPDATE_TRIVIAL)
			{
				idle_draw_main_idle();
				displaySoftKeys(TxtNames,TxtDelete);
			}
			ATB_edit_Show(data->editor);

			data->editor->update = ED_UPDATE_DEFAULT;
#else /* NEW_EDITOR */
			idle_draw_main_idle();
			edtShow(data->edt);
			displaySoftKeys(TxtNames,TxtDelete);
#endif /* NEW_EDITOR */
        break;
        default:
        return 0;
    }

    return 1;
}


//#if defined (MMI_EM_ENABLED)
#ifdef MMI_EM_ENABLED
//#ifndef (NEPTUNE_BOARD)
#ifndef NEPTUNE_BOARD
/* This is not valid for Neptune Engineering Mode, hence*/

/*******************************************************************************

 $Function:    	screen_capture_dialog_cb

 $Description: Call back for the screen  capture key assignment dialog
                     xrashmic 22 Aug, 2004 MMI-SPR-32798

 $Returns:

 $Arguments:

*******************************************************************************/
 static void screen_capture_dialog_cb( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
       switch(reason)
       {
            case INFO_KCD_LEFT:
                screenCaptureKey=KCD_LEFT;
                break;
            case INFO_KCD_RIGHT:
                screenCaptureKey=KCD_RIGHT;
                break;
	     case INFO_KCD_HUP:
                screenCaptureKey=KCD_HUP;
                break;             
            case INFO_KCD_UP:
                 dialog_info_destroy (screen_capture_win);
                screenCaptureKey=KCD_MNUUP;
                break;                
            case INFO_KCD_DOWN:
                 dialog_info_destroy (screen_capture_win);
                screenCaptureKey=KCD_MNUDOWN;
                break;                
            case INFO_KCD_OFFHOOK:
                screenCaptureKey=KCD_CALL;
                break;                
            case INFO_KCD_ALL:
                screenCaptureKey=Identifier;
                break;
            default:
                screenCaptureKey=KCD_NONE;
       }
       
}

/*******************************************************************************

 $Function:    	screen_capture_dialog

 $Description:  Dialog to set a key for screen capture
                      xrashmic 22 Aug, 2004 MMI-SPR-32798


 $Returns:

 $Arguments:

*******************************************************************************/

void screen_capture_dialog(void)
{
    T_DISPLAY_DATA  display_info;
    T_MFW_HND       parent_win  = mfwParent(mfw_header());

    dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull, COLOUR_WAP_POPUP);
    display_info.TextString="Press a key";
    display_info.TextString2="For screen capture";
    dlg_initDisplayData_events( &display_info,(T_VOID_FUNC)screen_capture_dialog_cb, FOREVER,KEY_ALL );
    display_info.Identifier  = 0; 
    screen_capture_win=info_dialog(parent_win, &display_info);
    dspl_Enable(0);
    win_show(screen_capture_win);
    dspl_Enable(1);
} 
#endif /* NEPTUNE_BOARD */
#endif

/*******************************************************************************

 $Function:    	idle_dialling_kbd_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static int idle_dialling_kbd_cb (MfwEvt e, MfwKbd *k)
{
	/*
     * The following lines describe the common method to extract the data from the window. This mfw_parent(mfw_header())
	 * construction captures the current mfw handled window.
     */
	T_MFW_HND			win				= mfw_parent (mfw_header());
    T_MFW_WIN			*win_data		= ((T_MFW_HDR *)win)->data;
    T_idle_dialling		*data			= (T_idle_dialling *)win_data->user;
    T_MFW_SS_RETURN		ssRes;
//	T_MFW_PHB_ENTRY		p_pEntry;    // RAVI
	tMmiPhbData			*current		= (tMmiPhbData *)ALLOC_MEMORY (sizeof (tMmiPhbData ));
	U8 					finishedHere	= FALSE;	/* SH- used to skip out part of function */
	int					position		= 0;
	char				edt_buf_prov[EDITOR_SIZE];
	/* try to establish if we have information in the phone book
	*/
     T_RV_RETURN_PATH            return_path = {0, NULL};
	 T_AUDIO_RET  ret;
	int wei;

      char *str1 = "/FFS/mmi/tones/a.amr";
	
	TRACE_FUNCTION ("idle_dialling_kbd_cb()");

	/* SPR#1428 - SH - New Editor changes - By default, set this flag
	 * so that only editor portion of idle screen is updated when a key is pressed. */
#ifdef NEW_EDITOR
	data->editor->update = ED_UPDATE_TRIVIAL;
#endif

	switch (k->code)
	{
		case KCD_MNURIGHT:
		case KCD_MNUUP:
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
	        edtChar(data->edt,ecRight);
#endif /* NEW_EDITOR */
		break;

		case KCD_MNULEFT:
        case KCD_MNUDOWN:
        	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
	        edtChar(data->edt,ecLeft);
#endif /* NEW_EDITOR */
        break;

		case KCD_CALL:
#ifdef JVJ_USSD_TEST
			{
			  T_MFW_SS_USSD prov_ussd;

			  prov_ussd.len = strlen((char*)idle_data.edt_buf);
			  memcpy(prov_ussd.ussd,(char*)idle_data.edt_buf,prov_ussd.len);
			  prov_ussd.dcs = MFW_DCS_7bits;
			  /*    Jun 30, 2006     REF : OMAPS00083503    R.Prabakar
				    Description : When USSD string is sent, MMI shows requesting screen forever
				    Solution     : We will send USSD string or call number only when we are in full service, unless it's an emergency number*/
  		if(idle_data.nm_status == NETWORK_FULL_SERVICE || cm_check_emergency((UBYTE*)idle_data.edt_buf))
						ss_send_ussd(&prov_ussd);
		else
			bookInfoDialog("No network", NULL);
		
			}
#else
			  /*    Jun 30, 2006     REF : OMAPS00083503    R.Prabakar
				    Description : When USSD string is sent, MMI shows requesting screen forever
				    Solution     : We will send USSD string or call number only when we are in full service, unless it's an emergency number*/
		if(idle_data.nm_status == NETWORK_FULL_SERVICE || cm_check_emergency((UBYTE*)idle_data.edt_buf))
			{
			callNumber((UBYTE*)idle_data.edt_buf);
			}
		else
			bookInfoDialog("No network", NULL);
#endif
			idle_dialling_destroy(data->win);
		break;

       	case KCD_LEFT:						/* call establishment	*/
			/* Ensure next update is a full update */
#ifdef NEW_EDITOR
			data->editor->update = ED_UPDATE_DEFAULT;
#endif
			if (bookPhoneBookLoading()==BOOK_SUCCESS)
			{
				T_MFW_HND phbk_win;
				phbk_win = bookPhonebookStart(idle_data.win,PhbkNameEnterIdle);
		        SEND_EVENT( phbk_win, PHBK_SEND_NUMBER, 0,(char*)idle_data.edt_buf );
			}
		break;

		case KCD_RIGHT:
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(data->editor, TRUE); /* SPR#2342 - SH */
			win_show(data->win);
#else /* NEW_EDITOR */
	        edtChar(data->edt,ecBack);
#endif /* NEW_EDITOR */

			if (strlen(idle_data.edt_buf) <= 0)
				idle_dialling_destroy(data->win);
		break;

		/* SPR#2342 - SH - HUP detroys editor */
		case KCD_HUP:
			idle_dialling_destroy(data->win);
			
			break;

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
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_AsciiChar(data->editor, editControls[k->code], TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
	        edtChar(data->edt, editControls[k->code]);
#endif /* NEW_EDITOR */
		break;

		case KCD_HASH:
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_AsciiChar(data->editor, editControls[k->code], TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
	        edtChar(data->edt, editControls[k->code]);
#endif /* NEW_EDITOR */
            memset(edt_buf_prov,'\0',sizeof(edt_buf_prov));
			strcpy((char*)edt_buf_prov,(char*)idle_data.edt_buf);

//hou peng test begin
           if(strcmp((char*)idle_data.edt_buf, "#1#") == 0)
			{

		//		rfs_test_01();
			}
		     if(strcmp((char*)idle_data.edt_buf, "#2#") == 0)
			{

	//			rfs_test_02();
			}
		if(strcmp((char*)idle_data.edt_buf, "#3#") == 0)
			{
			int i=0;
			wefilename = (T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER*)mfwAlloc(sizeof(T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER)*80);
			  convert_u8_to_unicode(str1, me_uc);
		
		           while (me_uc[i] != '\0')
                       {
                             wefilename->memo_name[i] = me_uc[i];
                                i++;
                          }
                         wefilename->memo_name[i] = '\0';  // add end of string 	
                      
	
			ret=audio_mms_play_from_ffs_start(wefilename,return_path);
			mfwFree((U8 *)wefilename,sizeof(T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER)*80);
			}
   if(strcmp((char*)idle_data.edt_buf, "#4#") == 0)
			{

				audio_mms_play_from_ffs_stop();
			}
		
//hou peng test end
			if(strcmp((char*)idle_data.edt_buf, "###520#") == 0)
			{
				ShowVersion(idle_data.win);
				idle_dialling_destroy(data->win);
				finishedHere = TRUE; /*sbh*/
			}

//            #ifdef TI_PS_FF_EM
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)					

                    //xrashmic 22 Aug, 2004 MMI-SPR-32798
                    //Using a dialog to take the input from the user for key to be used for screen capture.
                    else if(strcmp((char*)idle_data.edt_buf, "###1234#") == 0)
                    {
						#ifndef NEPTUNE_BOARD
                            screen_capture_dialog();
                            idle_dialling_destroy(data->win);
						#endif
                    }
		      else
		      {	/*MC, SPR1209 check for engineering mode code*/
				if (strcmp((char*)idle_data.edt_buf, "*36446337464#") == 0)
				{
					TRACE_EVENT("Engineering mode string");
					/*create menu*/
					mmi_em_start_eng_mode_menus(idle_data.win);
					idle_dialling_destroy(data->win);
					finishedHere = TRUE;
				}
#endif
				else
				{
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
						case MFW_SS_CCBS:
							finishedHere = TRUE;					/* Handled on # */
							break;
						case MFW_SS_SIM_REG_PW:
						case MFW_SS_SIM_UNBLCK_PIN:
							gsm_idle(0,(char *)idle_data.edt_buf);
							idle_dialling_destroy(data->win);
							finishedHere = TRUE;					/* Handled on # */
							break;
						case MFW_SS_MMI:
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
//	guarding the code with the flag						
#ifndef FF_MMI_ATC_MIGRATION	
							get_imei((char*)idle_data.imei);
							showIMEI(idle_data.win,(char*)idle_data.imei);
#endif
							idle_dialling_destroy(data->win);
							finishedHere = TRUE;					/* Handled on # */
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
//	replaced the get_imei() with the mmi_imei_retrieve() - sAT_Dn()							
#ifdef FF_MMI_ATC_MIGRATION							
							mmi_imei_retrieve();
#endif							
							break;
					  /*MC, SPR 1111 handle language reset*/
			        case MFW_SS_LANG_RESET:
			        	/*if language has been reset*/
			        	if (resources_reset_language((char*) idle_data.edt_buf) == TRUE)
			        	{
									bookShowInformation(idle_data.win,TxtDone,NULL,NULL);
			        	}
			        	else	/*if current language English, don't show second Line in English*/
			        	{T_DISPLAY_DATA display_info;

			        	/*SPR 1619, use new info dialog configuration functions*/
						if (Mmi_getCurrentLanguage() == ENGLISH_LANGUAGE)
							dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, MmiRsrcGetText(TxtUnknownCode), NULL, COLOUR_POPUP);
						else
							dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, MmiRsrcGetText(TxtUnknownCode), "Code Unknown", COLOUR_POPUP);
							dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, SIX_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT );
							/* Call Info Screen
							 */
							info_dialog( idle_data.win, &display_info );
			        	}
			        /*MC end*/
			        	idle_dialling_destroy(data->win);
			        	finishedHere = TRUE;
			        break;

						case MFW_SS_DIAL:
						case MFW_SS_DIAL_IDX:
						case MFW_SS_DTMF:
						case MFW_SS_UNKNOWN:
						default:
							/* CQ16651  - NDH - Lines Deleted, Do not delete the entered character */
							break;
					}
				}

//#ifdef TI_PS_FF_EM
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)					
			}
#endif
/* SH - skip out this section if we recognised SS code */

		if (!finishedHere)
		{
//nm 450
//  this functions looks for shortcut
			sscanf(idle_data.edt_buf, "%d", &position);
			current->missedCallsOffset = 0;
			current->index = position;
			current->selectedName = 0;
			current->KindOfSearch = SEARCH_BY_LOCATION;
			current->status.book = bookActiveBook(READ);
			bookGetCurrentStatus( &(current->status) );

	        if((idle_data.edt_buf[0]!='*')&&(bookFindName( 1, current )== MFW_PHB_OK))
	        {  memset(idle_data.edt_buf,'\0',sizeof(idle_data.edt_buf));

				strncpy((char*)idle_data.edt_buf,(char*)current->entry[0].number,sizeof(idle_data.edt_buf)-1);
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Char(data->editor, ctrlBottom, TRUE);
				win_show(data->win);
#else /* NEW_EDITOR */
				edtChar(data->edt,ecBottom);
#endif /* NEW_EDITOR */
	        }
		}
          break;
		default:
		break;
            	}
	FREE_MEMORY ((void *)current, sizeof (tMmiPhbData));
    return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:    	idle_dialling_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int idle_dialling_kbd_long_cb (MfwEvt e, MfwKbd *k)
{
	T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_idle_dialling         * data = (T_idle_dialling *)win_data->user;

	TRACE_FUNCTION ("idle_dialling_kbd_long_cb");

	if (e & KEY_LONG)
	{
	/* SPR#1428 - SH - New Editor - Long right soft keypress clears buffer */
#ifdef NEW_EDITOR
		if ((e & KEY_RIGHT) || (e & KEY_CLEAR))
#else /* NEW_EDITOR */
		if (e & KEY_CLEAR)
#endif /* NEW_EDITOR */
		{
			idle_dialling_destroy(data->win);
			return MFW_EVENT_CONSUMED;
		}
		if (e & KEY_0)
		{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */
			ATB_edit_AsciiChar(data->editor, '+', TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
            edtChar(data->edt,ecBack);
            edtChar(data->edt,'+');
#endif /* NEW_EDITOR */

			return MFW_EVENT_CONSUMED;
		}
		if (e & KEY_1)
		{
			/*OMAPS00106744 Dec 08,2006 a0393213(R.Prabakar)
			   Description : Wrong string is displayed when pressing long [1]
			   Solution     : M_callVoice() was called which checks the voice mail number stored in flash
			   Now menu_cphs_call_mailbox_number() is called which checks cphs4.2 file if it's present else it would call M_callVoice()*/
			if(strcmp(idle_data.edt_buf,"1")==0)
				{
			//long press on key 1 start to dial the voicemail
				menu_cphs_call_mailbox_number(NULL,NULL);
			return MFW_EVENT_CONSUMED;
				}
		}
		/* MZ cq11414 support for Pause seperator for DTMF strings. */
		if(e & KEY_STAR)
		{
		#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(data->editor, FALSE);

			//Sep 2, 2004    REF: CRR 21370 xkundadu
			// Changed the long press of '*' to display 'w'.
			
			ATB_edit_AsciiChar(data->editor, 'w', TRUE);
			win_show(data->win);
		#else /* NEW_EDITOR */
			/* MZ cq11414 add Pause Character */
	       	MfwEdt 	*edit =	((MfwHdr*) data->edt)->data;
			if((edit->cp) > 0) && (edit->attr->text[edit->cp-1] == '*'))
			{
				edtChar(data->edt,ecBack);
			}

			//Sep 2, 2004    REF: CRR 21370 xkundadu
			//Changed the long press of '*' key to display 'w'.
			edtChar(data->edt,'w');

		#endif /* NEW_EDITOR */
			return MFW_EVENT_CONSUMED;
		}
		if (e & KEY_HASH)
		{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(data->editor, FALSE); /* SPR#2342 - SH */

			//Sep 2, 2004    REF: CRR 21370 xkundadu
			// Changed the long press of '#' key to display 'p'.
			ATB_edit_AsciiChar(data->editor, 'p', TRUE);
			win_show(data->win);
#else /* NEW_EDITOR */
		/* MZ cq11414 add Wait  Character */
	       MfwEdt 	*edit =	((MfwHdr*) data->edt)->data;
		if((edit->cp) > 0) && (edit->attr->text[edit->cp-1] == '#'))
		{
			edtChar(data->edt,ecBack);
		}

		//Sep 2, 2004    REF: CRR 21370 xkundadu
	      // Changed the long press of '#' to display 'p'.
	      
		edtChar(data->edt,'p');

#endif /* NEW_EDITOR */
			return MFW_EVENT_CONSUMED;
		}

	}
	return MFW_EVENT_CONSUMED;
}











/*********************************************************************
**********************************************************************

					KEYPAD LOCKED EVENT WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/


/*******************************************************************************

 $Function:    	idle_key_pad_locked_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



T_MFW_HND idle_key_pad_locked_create(MfwHnd parent_window){

    T_MFW_WIN     * win;
    T_idle_key_pad_locked *  data = (T_idle_key_pad_locked *)ALLOC_MEMORY (sizeof (T_idle_key_pad_locked));

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)idle_key_pad_locked_win_cb);

    if (data->win EQ 0)
      return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)idle_key_pad_locked;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;
    /*
    * Create any other handler
    */



    data->kbd      = kbd_create (data->win,KEY_ALL,(T_MFW_CB)idle_key_pad_locked_kbd_cb);
    data->kbd_long = kbd_create (data->win,KEY_ALL|KEY_LONG,(T_MFW_CB)idle_key_pad_locked_kbd_long_cb);
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetAttr(&idle_data.editor_attr, WIN_DIALLING, COLOUR_EDITOR_XX, EDITOR_FONT, 0, ED_CURSOR_UNDERLINE, ATB_DCS_ASCII, (UBYTE*)idle_data.edt_buf,EDITOR_SIZE);
	memset (idle_data.edt_buf,'\0',EDITOR_SIZE);
	data->editor      = ATB_edit_Create(&idle_data.editor_attr,0);
	ATB_edit_Init(data->editor);
#else /* NEW_EDITOR */
	data->edt      = edtCreate(data->win,&idle_data.edt_attr,0,0);
#endif /* NEW_EDITOR */

	data->win_info				= 0;

	data->menuPressed = FALSE;

   /*
    * return window handle
    */
	memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
	edtReset(data->edt);
#endif /* NEW_EDITOR */
	showKeyPadLocked = showNothing;
    winShow(data->win);
    return data->win;
}


/*******************************************************************************

 $Function:    	idle_key_pad_locked_destroy

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idle_key_pad_locked_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_idle_key_pad_locked        * data;

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_idle_key_pad_locked *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */
      win_delete (data->win);
    }
    FREE_MEMORY ((void *)data, sizeof (T_idle_key_pad_locked));
    idle_data.klck_win = NULL;
  }
}


/*******************************************************************************

 $Function:    	idle_key_pad_locked

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idle_key_pad_locked (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_idle_key_pad_locked      * data = (T_idle_key_pad_locked *)win_data->user;
	U8* key_code;
//	UBYTE j;  // RAVI
	// Nov 3, 2004 REF: CRR 25887 xkundadu
	//Description: Unable to make an emergency call.
	// Added this variable to check whether the entered digit is of length 2.
	BOOL bTwodigitECC = FALSE;


    TRACE_FUNCTION ("idle_key_pad_locked()");

    switch (event)
	{
	case IDLE_KEYPAD_LOCKED_UPDATE:
	key_code = (U8*)parameter;
	switch (*key_code)
	{
		
		case KCD_RIGHT:
			/* API - 10/10/02 - 1162 - Call the emergency Number if this is true
			*/
			if (pin_idle_emerg_call == TRUE)
			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Hide(data->editor);
#else /* NEW_EDITOR */
				edtHide(data->edt);
#endif /* NEW_EDITOR */
				callNumber((UBYTE*)idle_data.edt_buf);

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
				edtReset(data->edt);
#endif /* NEW_EDITOR */
				memset(idle_data.edt_buf,'\0',EDITOR_SIZE);	/* sbh */
			    memset(data->TempkeyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
			    memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
				showKeyPadLocked = showNothing;
				pin_idle_emerg_call = FALSE;
			}
			else
			{
				data->menuPressed = TRUE;
				data->win_info = idle_information_start(win,2000,showPressMenuStar,(T_VOID_FUNC)idle_info_destroy_cb);
			}
		break;
		case KCD_STAR:
			if (data->menuPressed)
			{
                TRACE_EVENT ("dactivate the KEYpad");
				settingsKeyPadLockOff();
				data->menuPressed = FALSE;
				idle_key_pad_locked_destroy(win);
				return;
			}
			else
			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Hide(data->editor);
				ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
				edtHide(data->edt);
				edtReset(data->edt);
#endif /* NEW_EDITOR */
			    memset(data->TempkeyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
			    memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
				memset(idle_data.edt_buf,'\0',EDITOR_SIZE);
				showKeyPadLocked = showNothing;
				data->win_info = idle_information_start(win,2000,showPressMenuStar,(T_VOID_FUNC)idle_info_destroy_cb);
			}
		break;
		// Jul 23,2004 CRR:20895 xrashmic - SASKEN
		// Description: The keypad should not be unlocked by SEND and *, it 
       	// should be unlocked only by Menu and *
		// Fix: SEND key is now handled the same way as rest of the number  
		// keys. Keypad unlock can be done only through MENU and *.		
		case KCD_CALL:
			
		case KCD_0:
		case KCD_1:
		case KCD_2:
		case KCD_8:
		case KCD_9:
			/* API - 10/10/02 - 1162 - Read the Idle KeyPadLock Buffer to see if the inputed valu
									   is that of an emergency Number
			*/
			data->menuPressed = FALSE;
			data->TempkeyPadLockBuf[0] = editControls[*key_code];

			strcat((char*)data->keyPadLockBuf, (char*)data->TempkeyPadLockBuf);
                     TRACE_EVENT_P1("data->keyPadLockBuf: %s",(char*)data->keyPadLockBuf);

			if((strlen((char*)data->keyPadLockBuf) > 1) && (strlen((char*)data->keyPadLockBuf) < 4))
			{
				TRACE_EVENT("cm_check_emergency() TRACE IDLE 1");

				// Nov 3, 2004 REF: CRR 25887 xkundadu
				//Description: Unable to make an emergency call.
				//Fix: Changed the code to make the emergency call when the 
				//	   screen is locked.


				// Check whether the buffer contains  any emergency number or not.
				// If the number is ECC then allow the user to make emergency call.
				
				if ((cm_check_emergency((UBYTE*)data->keyPadLockBuf)==TRUE) ||
					(( strlen((char*)data->keyPadLockBuf) == ECC_LENGTH) && 
					(cm_check_emergency((UBYTE*)&data->keyPadLockBuf[1])==TRUE) &&
					(bTwodigitECC == TRUE)))  // RAVI - Changed = to == in condition.
					{
						if(bTwodigitECC == TRUE)
						{
							// If the last  digits form ECC, copy the digits to editor buffer.
							memcpy(idle_data.edt_buf, &data->keyPadLockBuf[1], KEYPADLOCK_BUF_SIZE-1);
						}
						else
						{
						      	strcpy((char*)idle_data.edt_buf,(char*)data->keyPadLockBuf);
						}
						showKeyPadLocked = showSOSCallKeyPadLocked;
						pin_idle_emerg_call = TRUE;
					    	memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
						winUpdate((MfwWin*)mfwControl(win));
				}
				else
				{
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					ATB_edit_Hide(data->editor);
					ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
					edtHide(data->edt);
					edtReset(data->edt);
#endif /* NEW_EDITOR */

				// Nov 3, 2004 REF: CRR 25887 xkundadu
				//Description: Unable to make an emergency call.
				//Fix: Changed the code to make the emergency call when the 
				//	   screen is locked.
					if(strlen((char *)data->keyPadLockBuf) >= 3)
					{
				   		//If the buffer length is greater than or equal to 3 ( i.e ECC_LENGTH)
				   		// remove the first character from the buffer. This is to check 
				   		// whether the remaining characters are part of ECC.
						memcpy(&data->keyPadLockBuf[0], &data->keyPadLockBuf[1], KEYPADLOCK_BUF_SIZE-1);
					}
					memset(idle_data.edt_buf,'\0',EDITOR_SIZE);
					showKeyPadLocked = showNothing;
					pin_idle_emerg_call = FALSE;
					data->win_info = idle_information_start(win,2000,showPressMenuStar,(T_VOID_FUNC)idle_info_destroy_cb);
					winUpdate((MfwWin*)mfwControl(win));
				}
			}
			else
			{
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Hide(data->editor);
				ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
				edtHide(data->edt);
				edtReset(data->edt);
#endif /* NEW_EDITOR */
				if(strlen((char *)data->keyPadLockBuf) >= 3)
					memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
				memset(idle_data.edt_buf,'\0',EDITOR_SIZE);
				showKeyPadLocked = showNothing;
				data->win_info = idle_information_start(win,2000,showPressMenuStar,(T_VOID_FUNC)idle_info_destroy_cb);
				pin_idle_emerg_call = FALSE;
				winUpdate((MfwWin*)mfwControl(win));
			}

		break;

//	Sep 22, 2004    REF: CRR 23871 xkundadu
//	Description: Screen Locking: Unlocking screen is happening by other keys
//	Fix: handled the other key events except '*' not to unlock the keypad.

		case KCD_HASH:
		case KCD_MNUDOWN:
		case KCD_MNUUP:
		case KCD_MNULEFT:
		case KCD_MNURIGHT:
		case KCD_MNUSELECT:
		case KCD_LEFT:
		case KCD_3:
		case KCD_4:
		case KCD_5:
		case KCD_6:
		case KCD_7:
		case KCD_HUP:	
						// If the user presses key, other than MENU/RSK set the 
						// menuPressed key to FALSE. This is done to avoid the 
						// unlocking by keys except MENU and  '*'.
						data->menuPressed = FALSE;

		
		default:
						data->menuPressed = FALSE;

		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Hide(data->editor);
				ATB_edit_Reset(data->editor);
#else /* NEW_EDITOR */
				edtHide(data->edt);
				edtReset(data->edt);
#endif /* NEW_EDITOR */
				memset(idle_data.edt_buf,'\0',EDITOR_SIZE);
			    memset(data->TempkeyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
			    memset(data->keyPadLockBuf,'\0',KEYPADLOCK_BUF_SIZE);
				showKeyPadLocked = showNothing;
				pin_idle_emerg_call = FALSE;
				//winUpdate((MfwWin*)mfwControl(win));
				data->win_info = idle_information_start(win,2000,showPressMenuStar,(T_VOID_FUNC)idle_info_destroy_cb);
		break;
	}
	break;

	case IDLE_NEW_SMS:
		// show the info about incoming sms on the idle screen
		showKeyPadLocked = showNothing;

		/* MZ 1942 remove the additional SMS received indication screen  */

		//data->win_info = idle_information_start(win,2000,showSMSNewMessage,(T_VOID_FUNC)idle_info_destroy_cb);
		windowsUpdate();
	break;

	case IDLE_INFO_ENDED:
		data->win_info = 0;
		win_show(win);
	break;

	// Sept 15, 2005    REF: CRR 32410  x0021334
	// Description: The keypad unlocks even though the unlocking sequence is not executed in quick succession.
	// Handle IDLE_KEYPAD_UNLOCK_TIMER_EXPIRED event.
	case IDLE_KEYPAD_UNLOCK_TIMER_EXPIRED:
		data->menuPressed = FALSE;
	break;
	default:
	return;
	}

}

/*******************************************************************************

 $Function:    	idle_key_pad_locked_win_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static int idle_key_pad_locked_win_cb(MfwEvt e, MfwWin *w)
{

//	T_idle_key_pad_locked * data = (T_idle_key_pad_locked *)w->user;  // RAVI

    TRACE_FUNCTION ("idle_key_pad_locked_win_cb()");

	switch (e)
    {
        case MfwWinVisible:
        	/*NM, p022*/
			showKeyPadLocked();
		break;

        default:
        return 0;
    }

    return 1;
}


/*******************************************************************************

 $Function:    	idle_key_pad_locked_kbd_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/

static int idle_key_pad_locked_kbd_cb (MfwEvt e, MfwKbd *k)
{

	T_MFW_HND       win  = mfw_parent (mfw_header());
	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
	T_idle_key_pad_locked * data = (T_idle_key_pad_locked *)win_data->user;

    TRACE_FUNCTION ("idle_key_pad_locked_kbd_cb()");

	SEND_EVENT(data->win,IDLE_KEYPAD_LOCKED_UPDATE,0,&k->code);
	return MFW_EVENT_CONSUMED;

}



/*******************************************************************************

 $Function:    	idle_key_pad_locked_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static int idle_key_pad_locked_kbd_long_cb (MfwEvt e, MfwKbd *k)
{
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	showSOSCallKeyPadLocked

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




static void showSOSCallKeyPadLocked(void)
{
	TRACE_FUNCTION("showSOSCallKeyPadLocked");
	/*NM p022*/

	/*SPR#2026 - DS - Set up colour for "Emergency?" screen */
	resources_setColour(COLOUR_EDITOR);

	idle_displayData(IDLE_LOCKED_ECC,TxtEmergency,NULL);
	softKeys_displayId(TxtNull,TxtSoftCall,0,COLOUR_EDITOR); /*SPR#2026 - DS - Changed colour from COLOUR_IDLE */
}


/*******************************************************************************

 $Function:    	showNothing

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static void showNothing(void)
{
	TRACE_FUNCTION("showNothing");
}



/*******************************************************************************

 $Function:    	showPressMenuStar

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static void showPressMenuStar(void)
{
	TRACE_FUNCTION("showPressMenuStar");

	idle_displayData(IDLE_LOCKED,TxtLocked,NULL);
	idle_displayData(IDLE_MENU_PRESS,TxtPressMenu,NULL);

}


#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:    	showSMSNewMessage

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static void showSMSNewMessage(void)
{
	char text[20];
	int txtStrId;
	int txtNo;
	int xOfs;		//used to offset text id when a number is to be displayed

	TRACE_FUNCTION("showSMSNewMessage");

	dspl_Clear(WIN_DIALLING_ORG_X,Mmi_layout_line_icons(2),mmiScrX,Mmi_layout_second_line()+YLINE*2);

	txtStrId = smsidle_display_sms_status(&txtNo );

	//If data has been set up, then show it
	if (txtStrId != TxtNull)
	{
		if (txtNo != 0)
		{//we have a number to display - number of new messages/missed calls
			sprintf((char*)text,"%d",txtNo);
			dspl_TextOut(4,24,0,text);//always display numbeer as ascii
			if (txtNo < 10)
				xOfs = 12;
			else
				xOfs = 18;
		}
		else
			xOfs = 0;

		dspl_TextOut(4+xOfs,24,DSPL_TXTATTR_CURRENT_MODE,GET_TEXT(txtStrId));//display as chinese or ascii
	}

	displaySoftKeys(TxtNull,TxtNull);
}
#endif



/*********************************************************************
**********************************************************************

					Information WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/



/*******************************************************************************

 $Function:    	idle_information_create

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




T_MFW_HND idle_information_create(MfwHnd parent_window){

    T_MFW_WIN     * win;
    T_idle_information *  data = (T_idle_information *)ALLOC_MEMORY (sizeof (T_idle_information));

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)idle_information_win_cb);
	data->kbd = kbd_create (data->win,KEY_ALL,(T_MFW_CB)idle_information_kbd_cb);

    if (data->win EQ 0)
      return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)idle_information;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;
	data->parent_win			= parent_window;
    /*
    * Create any other handler
    */



   /*
    * return window handle
    */
    return data->win;

}

/*******************************************************************************

 $Function:    	idle_information_destroy

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void idle_information_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_idle_information        * data;

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_idle_information *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */
		win_delete (data->win);

	 /* x0039928 - Lint warning fix */
    SEND_EVENT(data->parent_win,IDLE_INFO_ENDED,0,0);
	FREE_MEMORY ((void *)data, sizeof (T_idle_information));
  }

  }
}


/*******************************************************************************

 $Function:    	idle_information_start

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



T_MFW_HND idle_information_start(MfwHnd win_parent,S32 time,void (*show)(void),T_VOID_FUNC info_cb){



    T_MFW_HND win = idle_information_create(win_parent);


	T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_idle_information         * data = (T_idle_information *)win_data->user;


	data->timer = timCreate(win,time,(MfwCb)idle_information_tim_cb);
	data->info_cb = (T_VOID_FUNC)info_cb;
	timStart(data->timer);
	showInformation = show;
	winShow(win);
	return win;
}


/*******************************************************************************

 $Function:    	idle_information

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




void idle_information (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
  //  T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;  // RAVI
  //  T_idle_information      * data = (T_idle_information *)win_data->user;  // RAVI

    TRACE_FUNCTION ("idle_information()");

    switch (event)
	{
		default:
		return;
	}

}


/*******************************************************************************

 $Function:    	idle_information_win_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static int idle_information_win_cb(MfwEvt e, MfwWin *w)
{

	TRACE_FUNCTION("idle_information_win_cb");


	switch (e)
    {
        case MfwWinVisible:
			showInformation();
		break;
        default:
        return 0;
    }

    return 1;
}



/*******************************************************************************

 $Function:    	idle_information_kbd_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static int idle_information_kbd_cb(MfwEvt e, MfwKbd *k)
{
	T_MFW_HND       win  = mfw_parent (mfw_header());
	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
	T_idle_information * data = (T_idle_information *)win_data->user;
	T_MFW_HND       keypad_locked_win;
	U8				key_code;


    keypad_locked_win = data->parent_win;
	key_code = k->code;
	TRACE_FUNCTION("idle_information_kbd_cb");
	idle_information_destroy(data->win);
	SEND_EVENT(keypad_locked_win,IDLE_KEYPAD_LOCKED_UPDATE,0,&key_code);
    return MFW_EVENT_CONSUMED;
}

#if (0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:    	idle_information_kbd_long_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static int idle_information_kbd_long_cb(MfwEvt e, MfwKbd *k)
{
    return MFW_EVENT_CONSUMED;
}

#endif


/*******************************************************************************

 $Function:    	idle_information_tim_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static int idle_information_tim_cb(MfwEvt e, MfwTim *t){

	T_MFW_HND       win  = mfw_parent (mfw_header());
	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
	T_idle_information * data = (T_idle_information *)win_data->user;

    	TRACE_FUNCTION("idle_information_tim_cb");

	// Nov 3, 2004 REF: CRR 25887 xkundadu
	//Description: Unable to make an emergency call.
	//Fix: Changed the code to make the emergency call when the 
	//	   screen is locked.
	// If the "keypad locked" message is timed out, reset the key pad buffer to NULL
	if(data != NULL)
	{
	    	T_MFW_WIN      * mywin_data = ((T_MFW_HDR *) data->parent_win)->data;
		if(mywin_data != NULL)
		{
	    		T_idle_key_pad_locked      * mydata = (T_idle_key_pad_locked *)mywin_data->user;
				
			if(mydata->keyPadLockBuf != NULL)
			{
				// Set the buffer to null once the 'key locked' message
				//in the idle screen is timed out.
				memset(mydata->keyPadLockBuf, '\0', KEYPADLOCK_BUF_SIZE); 
			}
		}

	timStop(data->timer);
	data->info_cb(data->parent_win,NULL,NULL);

	// Sept 15, 2005    REF: CRR 32410  x0021334
	// Description: The keypad unlocks even though the unlocking sequence is not executed in quick succession.
	//  Send event to notify that the timer has expired.
	SEND_EVENT(data->parent_win, IDLE_KEYPAD_UNLOCK_TIMER_EXPIRED, 0, 0);
	}
	idle_information_destroy(win);
	windowsUpdate(); /* p303 - sbh */
	return 1;
}


/*******************************************************************************

 $Function:    	idle_info_destroy_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/




void idle_info_destroy_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason){

	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
	T_idle_key_pad_locked * data = (T_idle_key_pad_locked *)win_data->user;

	data->win_info = 0;

}
/*******************************************************************************

 $Function:    	showIMEI

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


void showIMEI (T_MFW_HND win, char* IMEI)
{
    T_DISPLAY_DATA   display_info;

    /* Marcus: Issue 1603: 28/01/2003: Replaced dlg_initDisplayData_TextId with dlg_initDisplayData_TextStr */
	dlg_initDisplayData_TextStr( &display_info, TxtSoftOK, TxtNull, MmiRsrcGetText(TxtIMEI), IMEI, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)idle_imei_info_cb, FOREVER, KEY_CLEAR| KEY_LEFT );

      /*
       * Call Info Screen
       */

    idle_data.info_win = info_dialog (win, &display_info);

}

/*******************************************************************************

 $Function:    	idle_imei_info_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/



static int idle_imei_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{


    switch (reason)      {
        case INFO_KCD_HUP:
        case INFO_KCD_LEFT:
          break;
	}

  return 1;
}

/*******************************************************************************

 $Function:    	Ciphering

 $Description:	Called by mfw
				Now this function doesn t seperate between the GSM and GPRS status

 $Returns:

 $Arguments:

*******************************************************************************/
void idle_Ciphering (UBYTE gsm_ciph, UBYTE gprs_ciph)
{


	if (gsm_ciph == 0 || gprs_ciph == 0)
	{

		TRACE_FUNCTION ("Ciphering: is deactivated");

		idle_data.ciphering = SHOW_CPRS_DEACTIV;
		//show the DEACTIVATED ciphering indication
		iconsSetState(iconIdCipheringDeActiv);
		iconsDeleteState(iconIdCipheringActiv);

	}
	else if (gsm_ciph == 1 || gprs_ciph == 1)
	{
		//show the ACTIVATED ciphering indication

		TRACE_EVENT ("Ciphering: is activated");

		idle_data.ciphering = SHOW_CPRS_ACITV;

		iconsSetState(iconIdCipheringActiv);
		iconsDeleteState(iconIdCipheringDeActiv);
	}
	else if (gsm_ciph == 2 || gprs_ciph == 2)
	{
		//dont show ANY indication !!!

		TRACE_EVENT ("Ciphering: is disabled");

		idle_data.ciphering = DONT_SHOW_CPRS;

		iconsDeleteState(iconIdCipheringActiv);
		iconsDeleteState(iconIdCipheringDeActiv);
	}

	//update the screen
	windowsUpdate();

}

/*******************************************************************************

 $Function:    	showVersion

 $Description:	Displays the version of software and date

 $Returns:

 $Arguments:

*******************************************************************************/
T_MFW_HND ShowVersion(T_MFW_HND parent_window)
{
  	T_MFW_HND       	win           = ShowVersion_create(parent_window);
	TRACE_FUNCTION("ShowVerion()");
    if (win NEQ NULL)
        {
           	SEND_EVENT (win, SHOWVERSION_INIT, 0, 0);
        }

   return win;
}

/*******************************************************************************

 $Function:    	ShowVersion_create

 $Description:	creates the window

 $Returns:

 $Arguments:

*******************************************************************************/
static T_MFW_HND ShowVersion_create(MfwHnd parent_window)
{
	tShowVerion* data = (tShowVerion*)ALLOC_MEMORY (sizeof (tShowVerion));
	T_MFW_WIN  * win;

	if (data EQ NULL)
	{
		return NULL;
	}

	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{

		return NULL;
	}
	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)ShowVersion_DialogCB;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (void *)data;
	data->parent_win         = parent_window;

	return data->win;
}


/*******************************************************************************

 $Function:    	showVersion_DialogCB

 $Description:	CallBack Function

 $Returns:

 $Arguments:

*******************************************************************************/
static void ShowVersion_DialogCB(T_MFW_HND win, USHORT e,  SHORT identifier,
void *parameter)
{
	T_MFW_WIN       	*win_data   = ( (T_MFW_HDR *) win )->data;
	tShowVerion* data = (tShowVerion*) win_data->user;
#ifdef NEW_EDITOR
//	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data */  // RAVI - Not Used.
#else
//	T_EDITOR_DATA		editor_data;   // RAVI  -  Not Used.
#endif

 	TRACE_FUNCTION("ShowVersion_DialogCB()");

	switch( e )
     {	//when window first created
    	case SHOWVERSION_INIT:
    	{

		strcat(data->buffer, "Date:");
		strcat(data->buffer, date);
		strcat(data->buffer,"\n");
		strcat(data->buffer, "BMI:");
		strcat(data->buffer, bmi);
		strcat(data->buffer,"\n");
		strcat(data->buffer,"GPRS:");
		strcat(data->buffer, gprs);
		strcat(data->buffer,"\n");
		strcat(data->buffer,"TI:");
		strcat(data->buffer, ti);

    	ShowVersionSetEditor(win);//set editor with default attribute values

		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_Start(win, &data->editor_data);
#else /* NEW_EDITOR */
		editor_start(win, &(data->editor_data));  /* start the editor */
#endif /* NEW_EDITOR */

		winShow(data->win);
    	}
    	break;

    	default:
    	{
    			TRACE_EVENT("ShowVersion_DialogCB(): Unknown Event");
    	}
    	break;

    }

}

/*******************************************************************************

 $Function:    	ShowVersionSetEditor

 $Description:	Set the editor up

 $Returns:

 $Arguments:

*******************************************************************************/
void ShowVersionSetEditor(T_MFW_HND win)
{
	T_MFW_WIN       	*win_data   = ( (T_MFW_HDR *) win )->data;
	tShowVerion* data = (tShowVerion*) win_data->user;

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&data->editor_data);
	AUI_edit_SetDisplay(&data->editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)ShowVerionCB);
	AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->buffer, 35);
	AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
	AUI_edit_SetTextStr(&data->editor_data, TxtNull, TxtSoftBack, TxtNull, NULL);
#else /* NEW_EDITOR */
	editor_attr_init(&((data->editor_data).editor_attr), NULL, edtCurNone, 0, (char *)data->buffer, 35, COLOUR_EDITOR);
	editor_data_init(&data->editor_data, NULL, TxtSoftBack, TxtNull, 0, 1, READ_ONLY_MODE, FOREVER);
			data->editor_data.hide				= FALSE;
			data->editor_data.Identifier		=  0;
			data->editor_data.TextString 		= NULL;
			data->editor_data.destroyEditor		= TRUE;
		data->editor_data.Callback     		= (T_EDIT_CB)ShowVerionCB;
#endif /* NEW_EDITOR */
}

/*******************************************************************************

 $Function:    	ShowVerionCB

 $Description:	Callback

 $Returns:

 $Arguments:

*******************************************************************************/
static void ShowVerionCB( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tShowVerion*data       = (tShowVerion*) win_data->user;


    TRACE_FUNCTION("ShowVerionCB()");

	switch (reason )
    {
      case INFO_KCD_LEFT:
	  case INFO_KCD_HUP:
      case INFO_KCD_RIGHT:

        default:
		showVersion_destroy(data->win);
		break;
	}
}

/*******************************************************************************

 $Function:    	showVersion_destroy

 $Description:	Destroys the editor

 $Returns:

 $Arguments:

*******************************************************************************/
void showVersion_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;

  tShowVerion* data = NULL;


	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : showVersion_destroy called with NULL Pointer");
		return;
	}


	win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL)
			data = (tShowVerion*)win_data->user;

	  if (data)
		{
		TRACE_EVENT ("calc_destroy()");

			win_delete (data->win);
		  // Free Memory
			FREE_MEMORY ((void *)data, sizeof (tShowVerion));

		}
	  else
		{
			TRACE_EVENT ("calc_destroy() called twice");
		}
	}
//GW 28/11/02 - debug only - to allod display.c to output a trace (for timing only)
void mmi_trace( char* str)
{
	TRACE_EVENT(str);/*a0393213 warnings removal-TRACE_FUNCTION changed to TRACE_EVENT*/
}

//xrashmic 26 Aug, 2004 MMI-SPR-23931
// To display the indication of new EMS in the idle screen
void addNewEMS(void)
{
/* x0045876, 14-Aug-2006 (WR - "currentVoiceTone" was set but never used) */
#ifndef FF_MIDI_RINGER
#ifndef FF_MMI_RINGTONE
    UBYTE currentVoiceTone; 
#endif
#endif

    idle_data.ems=TRUE;
//Apr 05, 2005    REF: ENH 29994 xdeepadh	
//Nov 02, 2005    REF:ER OMAPS00039044  xdeepadh
//Play SMS tone for EMS alert
#ifdef FF_MIDI_RINGER  
	mfw_ringer_start(AS_RINGER_MODE_SMS,false,sounds_midi_ringer_start_cb);
#else

/* x0045876, 14-Aug-2006 (WR - "currentVoiceTone" was set but never used) */
/* currentVoiceTone = getcurrentVoiceTone(); */

#ifdef FF_MMI_RINGTONE /* MSL SMS Tone */
    audio_PlaySoundID( AUDIO_BUZZER, SMSTONE_SELECT, 200, AUDIO_PLAY_ONCE );
#else

 /* x0045876, 14-Aug-2006 (WR - "currentVoiceTone" was set but never used) */
 currentVoiceTone = getcurrentVoiceTone();
 audio_PlaySoundID( AUDIO_BUZZER, currentVoiceTone, 200, AUDIO_PLAY_ONCE );
#endif
#endif
}

/*******************************************************************************

 $Function:    	idle_opername_timer_cb

 $Description:	Callback for timer 

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 14 Feb, 2006  ER: OMAPS00065203
int idle_opername_timer_cb(  MfwEvt e, MfwTim *tc )
{
	TRACE_FUNCTION("idle_opername_timer_cb()");

	//Display text is changed to PLMN name and refresh idle screen
	opername_disp_flag = PLMN;
	idleEvent(IdleUpdate);

	//Stop and delete timer
	if( plmn_display)
	{
	  timStop(plmn_display);
	}
	return MFW_EVENT_CONSUMED;
}

//x0pleela 23 Mar, 2007  ER :OMAPS00122561
#ifdef FF_PHONE_LOCK
/*******************************************************************************

 $Function:    	idle_autoPhoneLock_timer_cb

 $Description:	Callback for timer 

 $Returns:

 $Arguments:

*******************************************************************************/
int idle_autoPhoneLock_timer_cb(  MfwEvt e, MfwTim *tc )
{
	TRACE_FUNCTION("idle_autoPhoneLock_timer_cb()");

	//Stop and delete timer
	if( autoPhLock_timer)
	{
	  timStop(autoPhLock_timer);
	  autoPhLock_timer_flag= FALSE;
	  //display phone unlock screen
	  if( !phlock_win_handle )
		  phLock_Lock();
	}
	return MFW_EVENT_CONSUMED;
}
#endif /*FF_PHONE_LOCK*/

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

/*******************************************************************************

  $Function:    	powerManagementEvent
  
  $Description: Callback function for powermanagement timer..

  $Returns: Event consumed

  $Arguments: unused
*******************************************************************************/
int powerManagementEvent( MfwEvt e, MfwTim *tc )
{
    TRACE_FUNCTION("powerManagementEvent");
    return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

  $Function:    	mmi_update_pm_timer_duration
  
  $Description: function for updating PM timer..

  $Returns: void

  $Arguments: void
*******************************************************************************/
void mmi_update_pm_timer_duration( )
{
    TRACE_FUNCTION("mmi_update_pm_timer_duration");

  /* Depending on the value set in pm duration we can enable / disable the pm variable */
  if(FFS_flashData.pm_duration /*&& idleIsFocussed() */ )
    {
        pm_on = 1;
    }
    else
    {
        pm_on = 0;
        dspl_Enable(1);
    }
}

/*******************************************************************************

  $Function:    	mmi_pm_enable
  
  $Description: function to enable/disable Power Management .

  $Returns: void 

  $Arguments: ENABLE/DISABLE value
*******************************************************************************/
void mmi_pm_enable( int enable_pm)
{
    TRACE_FUNCTION("mmi_pm_enable()");

    if(0 == enable_pm)
    {
        SVC_SCN_EnablePM( 0, 0);
        pm_on = 0;
        dspl_Enable(1);
    }
    else
    {
        if(/*idleIsFocussed() &&*/ FFS_flashData.pm_duration)    
        {
            SVC_SCN_EnablePM( 1, FFS_flashData.pm_duration);   
            pm_on = 1;
           
            /*  timStart(Pm_timer); */
        }
    }

}


#endif
#endif
