/*******************************************************************************

	  CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiMain.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description:



********************************************************************************

 $History: MmiMain.c
 
    Nov 06, 2007 ER: OMAPS00151698 x0056422
    Description: 
    Solution 1) Need to suppress key beep sound for the commands that originated from the 
                BT headset and were simulated as key events. This is done by adding a check
                for BMI_BT_HEADSET_COMMAND in keyAction function, and do nothing inside if 
                it is true. 

 	May 24, 2007  DR: OMAPS00132483 x0pleela
	Description: MM: While playing an AAC, unplug USB freezes the audio
	Solution: 1) Changed the function names to get and set the status of headset, usb insertion 
				and removal
			2) In function  globalmme(), removed the function call to get the phonelock status during 
				USB plug/unplug and headset plug/unplug event and inturn using a static variable 
				which gives the phonelock status. Replaced the function call mfw_get_phlock_status 
				with mfw_get_Phlock_status to get the phone lock status

	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 

    Mar 22, 2007  DVT: OMAPS00121916  x0039928(sumanth)
    Description: T-Flash hot swap.
    
    Jan 05, 2007 OMAPS00084665  x0039928(sumanth)
    Description: Boot time exceed UCSD estimations
 	
    Oct 30, 2006 ER:OMAPS00098881 x0039928(sumanth)
    Removal of power variant
    
    Oct 30, 2006 ER:OMAPS00091029 x0039928(sumanth)
    Bootup time measurement

    Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
    Description:Enhance RSSI to 3 parameters: Strength, quality, min-access-level
    Solution: NAPTUNE flag is removed from globalSignalUpdate().
    
 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER: Handles the connect and disconnect event.

	Aug 03, 2006 REF:OMAPS00088329  x0039928
       Description : TTY> When TTY is ON Headset insertion detection / profile download should not happen
       Solution     : TTY ON condition is checked before downloading the headset profile.

       Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : As part of the implementation, in this file, type of GlobalIconStatus is changed       

	Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
	Description: Triton PG2.2 impacts on Locosto program
	Solution: Software workaround is done to register headset-hook dynamically 
	during/before call is established and unregister it after the call is ended/disconnected.

 	Apr 17, 2006    REF: ER OMAPS00075178 x0pleela
   	Description: When the charger connect to phone(i-sample), the charger driver works, but the MMI and App don't work
   	Solution:  Defined new functions "charging_start_cb()": Callback after dialog "Charging" is displayed
			and "charging_start_info()" : function to display the dialog "charging" when charger is connected
			Handling battery/charger events in globalmme()

 	Nov 09, 2005    REF: DVT OMAPS00056873 xdeepadh
   	Description: Deactivating Bright Light on I-Sample
   	Solution: On Isample boards the backlight is set off, since the bright LEDS are very disturbing.
 
  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 	Description:	Support for various audio profiles
 	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 			The audio device is enabled as per the user actions.	
	Jul 05, 2005    REF: SPR 31894 xdeepadh
   	Description: Proposed chnages for Locosto-live sign off
   	Solution: UICC SIM API will be used for Locosto.

 	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.
 	Apr 05, 2005    REF: ENH 29994 xdeepadh
   	Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
   	Solution: Generic Midi Ringer and Midi Test Application were implemented.
   	
	Jan 07, 2005 REF: CRR 26394 xkundadu 
	Issue description:Logo of the operator not displayed
	Solution: Set the network operator logo display option to enbled state. 
 
 	Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
	Issue description:  Golite booting problem.
  	Solution: If the FFS is not formatted, prevent writing into the FFS.
  	
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

 	Jul 22,2004 CRR:20896 xrashmic - SASKEN
 	Description: The current time format is not highlighted
 	Fix: The time format is written into flash. Instead of providing the submenu 
 	directly we call a function where we set the menu and also higlight the 
 	previously selected menu item


    	May 10, 2004    REF: CRR 15753  Deepa M.D 
	Menuitem provided to set the validity period to maximum.

    	25/10/00	Original Condat(UK) BMI version.
    	
  Dec 23, 2005    REF: SR13873 x0020906
	Description: To stop playing ringtone when new sms arrives - when user is in any menu screen.
	Fix : Fix is done in keyAction ().	
    
 $End

*******************************************************************************/

#include <stdio.h>
#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include "gdi.h"
#include "audio.h"

#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_lng.h"
#include "mfw_win.h"
#include "mfw_icn.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_ffs.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "Mfw_td.h"/*SPR 1725*/
#ifdef SIM_TOOLKIT
#include "mfw_sat.h"
#endif
#include "dspl.h"


#include "p_mmi.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "mfw_mme.h"

#ifndef _SIMULATION_
/* BEGIN ADD: Neptune Alignment */
#ifndef NEPTUNE_BOARD
/* END ADD: Neptune Alignment */
#include "sys_types.h"
/* BEGIN ADD: Neptune Alignment */
#else 
#include "bmi_integ_misc.h"
#endif
/* END ADD: Neptune Alignment */
#endif

#include "MmiMmi.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#endif

#include "Mmiicons.h"
#include "MmiPins.h"
#include "MmiMain.h"
#include "MmiDialogs.h"
#include "MmiIdle.h"
#include "MmiStart.h"
#include "mmiCall.h"
#include "MmiSimToolkit.h"

#include "MmiLists.h"
#include "MmiSounds.h"
#include "MmiResources.h"
#include "MmiSettings.h"
#include "MmiTimeDate.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

/* OMAPS00151698, x0056422 */

#ifdef FF_MMI_A2DP_AVRCP
#include "mfw_bt_api.h"
#include "mfw_bt_private.h"
#endif

//x0pleela 21 Apr, 2006 DR: OMAPS00075178
//Removing the AUDIO_PROFILE flag as Mmicolours.h is also required for Battery/charger indication fix
//#ifdef FF_MMI_AUDIO_PROFILE
#include "MmiColours.h"
//#endif
static MfwHnd times;	      /* clock via software timer */
static MfwHnd times_switchoff;		/* timer switch off (GB)*/

					/* GLOBAL CONTROL DATA	    */
LngInfo mainLngInfo;			/* language control block   */
MfwHnd mainMmiLng;			/* global language handler  */


//x0pleela 11 Apr, 2006 ER: OMAPS00075178
 //Timer for battery charging animation
extern MfwHnd battery_charging_animate; 

/* Taking idle data - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
extern T_idle idle_data;
#endif
/* END RAVI - 23-12-2005 */

#ifndef _SIMULATION_
extern void AI_Power(SYS_UWORD8 power);
#endif

#ifdef NEPTUNE_BOARD  /* Removal of linker errors */
void SIM_PowerOff (void);
#endif

static int keyAction (MfwEvt e, MfwKbd *res);
/* x0039928 - Lint warning removal
static int key (MfwEvt e, MfwKbd *kc); */
static int timer (MfwEvt e, MfwTim *t);
/* x0039928 - Lint warning removal
static void backlight (void);  */
static int timesEvent (MfwEvt e, MfwTim *t);
static void showGoodBye (T_MFW_HND win);
static void goodbye_cb (void);


static MfwHnd mme;				/* our MME manager	     */

/* Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
    Description : CPHS Roaming indication feature implementation
    Solution     : As part of the implementation, type of GlobalIconStatus is changed from U16 to U32 as number of
    bits in the variable is not sufficient to store details about all the icons*/    
U32 GlobalIconStatus;
UBYTE globalMobileMode;
int MmiModule;

// ADDED BY RAVI - 28-11-2005
EXTERN int flash_formatted(void);
EXTERN void time_date_init(void);
EXTERN void sms_cb_exit (void);
EXTERN void time_date_delete(void);
EXTERN UBYTE getCurrentRingerSettings(void);
EXTERN UBYTE CphsPresent(void);
EXTERN void  cphs_support_check(void);
// END RAVI

/* batterie and signal	  start  */
int globalBatteryUpdate(U8 value);
int globalSignalUpdate(U8 value);

//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
extern void mfw_audio_device_indication(U8 notify);
extern 	 void mmi_hook_handle();
void mmi_device_status_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback);
#endif


void globalFlashBattery (void);

UBYTE globalBatteryPicNumber;
UBYTE globalSignalPicNumber;

UBYTE globalBatteryMode; /*  */
int globalMme(MfwEvt e, void *para);
/* batterie and signal	   end	*/

/*MC SPR 1725 removed current clock and date variables*/

/* x0039928 - Lint warning removal
static LONG second; */
static U8 deregis = FALSE;	      /* status deregistration	     */

// SH 6/03/02 - HUPKeyOrigin is 1 if POWER_KEY is pressed in idle screen, 0 otherwise
// Means that long press of HUP key will go to idle screen, not just shut down
UBYTE HUPKeyOrigin;
int off_switch;	/* distinguish between on- and off switching to display a goodbye message*/
/* SPR877 - SH - set to TRUE if welcome animation has finished */
extern UBYTE animation_complete;

T_MFW_CFLAG_STATUS DivertStatus = MFW_CFLAG_NOTSet;
#define TIME_MINUTE	60000
#define TIMEDOWN	120000L 	/* time for powerdown 2min  */
#define TIME_SWITCH_OFF 3000	     /* switch off time delay */
//Jul 05, 2005    REF: SPR 31894 xdeepadh
//The reader_id  BSP_UICC_DEFAULT_READER_ID will be 0x01. 
#ifdef TI_PS_UICC_CHIPSET_15
#define BSP_UICC_DEFAULT_READER_ID 0x01
#endif

static UBYTE buffer[100];
/* Amounts to: 128 (format mode) + 8 (overwrite mode) */


extern BMI_BT_STRUCTTYPE tGlobalBmiBtStruct; 

/*******************************************************************************

 $Function:

 $Description:	general editor control

 $Returns:    none.

 $Arguments:

*******************************************************************************/

/* SPR#1428 - SH - New Editor changes - this version of editControls used
 * by new editor.  The rest is now in AUITextEntry */

#ifdef NEW_EDITOR

UBYTE editControls [KCD_MAX] =	    /* edit control keys:	*/
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '*', '#', ctrlNone, ctrlNone, ctrlUp, ctrlDown, ctrlLeft, ctrlRight,
    ctrlNone, ctrlNone, ctrlEnter, ctrlNone, ctrlNone, ctrlNone,
    ctrlNone, ctrlNone, ctrlEnter
};

#else /* NEW_EDITOR */

static MfwHnd myHandle = 0;		/* editors handle	    */
static MfwHnd editTim = 0;		/* the editor timer	    */
static char *formatHandle;	  // SH - pointer to format string, so that editEventKey can see it
static int *formatIndex;	// SH - pointer to format index
static int *fieldIndex; 	// SH - pointer to field position
static char *tmpBuf;	      // SH - pointer to temporary buffer for hidden alphanumeric entry

#define KEY_PAD_MAX 5

UBYTE editControls [KCD_MAX] =	    /* edit control keys:	*/
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '*', '#', ecNone, ecNone, ecUp, ecDown, ecLeft, ecRight,
    ecNone, ecNone, ecEnter, ecNone, ecNone, ecNone,
    ecNone, ecNone, ecEnter
};

static UBYTE editAlphaMode;		   /* alpha mode flag	       */
static UBYTE editCalcMode;		  /* calculator mode flag	   */
static UBYTE editFormatMode;	  /* format mode flag */
static UBYTE editHiddenMode;	  /* hidden alphanumeric mode */
static UBYTE editAlphaLevel;		   /* alpha level selected     */
static UBYTE editAlphaKey;		   /* alpha mode current key   */



static UBYTE editAlpha [12][KEY_PAD_MAX] =	     /* alpha mode characters	 */
{
    {'.','+','-','=','0'},		//0 key
    {' ','?','!','1',':'}, {'a','b','c','2',')'}, {'d','e','f','3','('},     // 1 2 3
    {'g','h','i','4','$'}, {'j','k','l','5',' '}, {'m','n','o','6',','},     // 4 5 6
  {'p','q','r','s','7'}, {'t','u','v','8',';'}, {'w','x','y','z','9'},	  // 7 8 9
  {'*','#','%','&',' '}, {' ','\'','\"','@','/'}  // star and hash keys
};

static UBYTE editAlphaU [12][KEY_PAD_MAX] =	  /* upCase alpha mode characters*/
{
  {'.','+','-','=','0'},	      //0 key
    {' ','?','!','1',':'}, {'A','B','C','2',')'}, {'D','E','F','3','('},    // 1 2 3
    {'G','H','I','4','$'}, {'J','K','L','5',' '}, {'M','N','O','6',','},   // 4 5 6
  {'P','Q','R','S','7'}, {'T','U','V','8',';'}, {'W','X','Y','Z','9'}, // 7 8 9
  {'*','#','%','&',' '}, {' ','\'','\"','@','/'}		// star and hash keys
};


int upCase= TRUE;


static int editEventTim (MfwEvt e, MfwTim *t);
static void editShowHeader (void);

#endif /* NEW_EDITOR */

#ifdef FF_MMI_AUDIO_PROFILE
extern T_call call_data;
#endif

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
#define Bool unsigned char 
EXTERN unsigned char Screen_Update;
EXTERN void SVC_SCN_EnablePM(Bool enable );
#endif
#endif
EXTERN MfwHnd timer_handle;

//x0pleela 19 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
static int ph_lock_status;

#define ResetFlag	0x00 //toset any bit to zero
#define UsbMsEnum 	0x01 //for USB MS enum/de-enum
#define HeadsetReg	0x02	//For Headset Plug/Unplug

//x0pleela 29 May, 2007 DR: OMAPS00132483
EXTERN int mfw_get_Phlock_status(void);
EXTERN void mfw_set_Phlock_status( int phlock_status);
EXTERN int mfw_get_Auto_Phlock_status(void);
EXTERN void mfw_set_Auto_Phlock_status( int phlock_status);

#endif

//x0pleela 19 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
/*******************************************************************************

 $Function:  mmi_set_Headset_Reg

 $Description:	 set /resets the HeadsetReg bit in ph_lock_status 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 23 May, 2007  DR: OMAPS00132483
GLOBAL void mmi_set_Headset_Reg( int headset_reg)
{
	TRACE_FUNCTION("mmi_set_Headset_Reg()");
	if( headset_reg )
		ph_lock_status |=  HeadsetReg;
	else
		ph_lock_status &= ResetFlag;
	return;
}

/*******************************************************************************

 $Function:  mmi_get_Headset_Reg

 $Description:	 returns the HeadsetReg status from ph_lock_status 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 23 May, 2007  DR: OMAPS00132483
GLOBAL int mmi_get_Headset_Reg(void)
{
	TRACE_FUNCTION("mmi_get_Headset_Reg()");
	if( (ph_lock_status & HeadsetReg) == HeadsetReg)
		return TRUE;
	else
		return FALSE;
}


/*******************************************************************************

 $Function:  mmi_set_usbms_enum

 $Description:	 set /resets the  UsbMsEnum bit in ph_lock_status 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 23 May, 2007  DR: OMAPS00132483
GLOBAL void mmi_set_usbms_enum( int usb_enum)
{
	TRACE_FUNCTION("mmi_set_usbms_enum()");
	if( usb_enum )
		ph_lock_status |=  UsbMsEnum;
	else
		ph_lock_status &= ResetFlag;
	return;
}

/*******************************************************************************

 $Function:  mmi_get_usbms_enum

 $Description:	 returns the UsbMsEnum status from ph_lock_status 

 $Returns:    none.

 $Arguments:

*******************************************************************************/
//x0pleela 23 May, 2007  DR: OMAPS00132483
GLOBAL int mmi_get_usbms_enum(void)
{
	TRACE_FUNCTION("mmi_get_usbms_enum()");
	if(( ph_lock_status & UsbMsEnum ) == UsbMsEnum)
		return TRUE;
	else
		return FALSE;
}
#endif /*FF_PHONE_LOCK*/

/*******************************************************************************

 $Function:  mainInit

 $Description:	 init MMI main module

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void mainInit (UBYTE InitCause)
{
	int i;
	T_MFW_SMS_INFO sms_settings;
	//x0018858 24 Mar, 2005 MMI-FIX-11321
	//added structures for storing the indication settings and CMER settings.
	T_ACI_MM_CIND_VAL_TYPE cindSettings;
	T_ACI_MM_CMER_VAL_TYPE cmerSettings;
    TRACE_FUNCTION("maininit");

    mfwSetSignallingMethod(1);
    winAutoFocus(TRUE);
    mfwSetFocus(0);			/* reset focus		    */


    /*MC SPR 1111*/
		/*MC check to see if  language already selected and saved to FFS*/

//  Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
//  Issue description:  Golite booting problem.
//  Solution: If the FFS is not formatted, dont write into the FFS.
//                In that case just set the langauge to English and do the mmi
//		     layout settings.
	if(flash_formatted() == TRUE)
	{
		if (flash_read() >= EFFS_OK)
		{	TRACE_EVENT_P1("FFS_flashData.language: %d", FFS_flashData.language);
			
			/*MC SPR 1150 if language is not 0 and not greater than the maximum language code*/
			if (FFS_flashData.language != NULL && FFS_flashData.language<= NO_OF_LANGUAGES)
				MmiRsrcSetLang(FFS_flashData.language);/*MC, change to stored language*/
			else
				MmiRsrcSetLang(ENGLISH_LANGUAGE);
			
			/*API - 28/07/03 - CQ10203 - If the flash value not set set to default VP_REL_24_HOURS*/
			if((FFS_flashData.vp_rel != VP_REL_1_HOUR)   && 
				(FFS_flashData.vp_rel != VP_REL_12_HOURS) &&				  
				(FFS_flashData.vp_rel != VP_REL_24_HOURS) && 
				(FFS_flashData.vp_rel != VP_REL_1_WEEK)&&
				(FFS_flashData.vp_rel != VP_REL_MAXIMUM))   //  May 10, 2004    REF: CRR 15753  Deepa M.D 
			{
				FFS_flashData.vp_rel = VP_REL_24_HOURS;
			}
			/*API - 28/07/03 - CQ10203 - END */

			/*NDH - 13/01/04 - CQ16753 - Propogate the Validity Period Settings to the ACI */
			sms_settings.vp_rel = FFS_flashData.vp_rel;
			(void)sms_set_val_period(&sms_settings);
			/*NDH - 13/01/04 - CQ16753 - END */
		}
		else
		{	
			/* GW If flash read fails set up default settings. */
			/* This will also save the settings to flash */
			memset(&FFS_flashData, 0x00, sizeof(FFS_flashData));
			/*default language is english*/
			setUserSettings2Default();
		}
	}
//  Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
//  Issue description:  Golite booting problem.
//  Solution: If the FFS is not formatted, dont write into the FFS.
//                In that case just set the langauge to English and mmi
//		     layout settings.
	else
	{
		 // Set the default language to English.
		 MmiRsrcSetLang(ENGLISH_LANGUAGE);

		//  Jan 07, 2005 REF: CRR 26394 xkundadu 
		//  Issue description:Logo of the operator not displayed
		//  Solution: Set the network operator logo display option to enbled state. 
		//  Set  other user settings also to default values.

		 #ifndef MMI_LITE
			memset(&FFS_flashData, 0x00, sizeof(FFS_flashData));
			setUserSettings2Default();
		 #endif
	}
/*mc end*/
	HUPKeyOrigin = 0;  // sbh
	off_switch=0;	/* 0 is stands for mobile is swiched on */
	animation_complete = FALSE;

    kbdAlways((MfwCb)keyAction);	/* global keyboard access   */
    kbdTime(2000,1000,400);		/* long / repeat timeouts   */

    mainMmiLng = lngCreate(0,0,&mainLngInfo,0); /* language handler */

	// times clock event

    /*OMAPS00098881 (removing power variant) - a0393213(Prabakar)
	 Removed #ifndef FF_POWER_MANAGEMENT.
	 Timer is created irrespective of LCD refresh state.
	 It would be started when LCD refresh is needed else it would be stopped*/
    times = timCreate(0,TIME_MINUTE,(MfwCb)timesEvent);

	
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
//	additional events addes for carkit and headset 
//xashmic 9 Sep 2006, OMAPS00092732
//Added MfwMmeUSBMS for handling USB MS events
#if defined(FF_MMI_AUDIO_PROFILE) && defined( FF_MMI_USBMS)
	 mme = mmeCreate(0, MfwMmeSignal|MfwMmeBattery|MfwMmeBaState|MfwMmeHeadset|MfwMmeCarkit|MfwMmeUSBMS, (MfwCb)globalMme);
#else 
#ifdef FF_MMI_AUDIO_PROFILE
	 mme = mmeCreate(0, MfwMmeSignal|MfwMmeBattery|MfwMmeBaState|MfwMmeHeadset|MfwMmeCarkit, (MfwCb)globalMme);
#else
		#ifdef FF_MMI_USBMS
	 		mme = mmeCreate(0, MfwMmeSignal|MfwMmeBattery|MfwMmeBaState|MfwMmeUSBMS, (MfwCb)globalMme);
		#else
	 mme = mmeCreate(0, MfwMmeSignal|MfwMmeBattery|MfwMmeBaState, (MfwCb)globalMme);
#endif
	#endif	
#endif

    /*Setup when we want the backlight to come on*/
// 	Nov 09, 2005    REF: DVT OMAPS00056873 xdeepadh
//On Isample boards the backlight is set off, since the bright LEDS are very disturbing.
    
	for (i=0;i<BL_LAST_OPTION;i++)
#if (BOARD == 71)
	mme_setBacklightEvent(i,BL_NO_LIGHT);
#else
	mme_setBacklightEvent(i,BL_MAX_LIGHT);
#endif

#if (BOARD == 71)
	mme_setBacklightEvent(BL_IDLE_TIMER,BL_NO_LIGHT);
#else
	mme_setBacklightEvent(BL_IDLE_TIMER,BL_SET_IDLE);
#endif

	mme_setBacklightEvent(BL_KEY_PRESS,BL_MAX_LIGHT);
	mme_setBacklightEvent(BL_INCOMING_CALL,BL_MAX_LIGHT);
	mme_setBacklightEvent(BL_EXIT,BL_NO_LIGHT);
	
	/*and tell the backlight that the init event has happened */
	mme_backlightEvent(BL_INIT);

	//x0018858 24 Mar, 2005 MMI-FIX-11321
	//Added two functions to set the indication and CMER values.
	//begin
	sms_set_cind_values(&cindSettings);
	sms_set_cmer_values(&cmerSettings);
	//end.

//RM test 14-07 second = 0;
   /*SPR 1725, replace FFS clock data with RTC*/

     /*OMAPS00098881 (removing power variant) a0393213(R.Prabakar)
	  Removed #ifndef FF_POWER_MANAGEMENT flag*/
	time_date_init();

    deregis = FALSE;

    /*OMAPS00098881 (removing power variant) a0393213(R.Prabakar)
	If FF_POWER_MANAGEMENT is not enabled the timer is started only when the lcd refresh is needed*/
#ifndef FF_POWER_MANAGEMENT 
      timStart(times);		       /*      start clock	    */
#else
      if(FFS_flashData.refresh==1)
	  	timStart(times);
#endif

    startExec(PhoneInit,0);		 /* get it rolling	    */

}

/*******************************************************************************

 $Function:  mainExit

 $Description:	 exit MMI main module

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void mainExit (UBYTE InitCause)
{
  TRACE_FUNCTION("mainExit");

  startExit();
  sim_exit();			      /* finit SIM handler	  */
    lngDelete(mainMmiLng); mainMmiLng = 0;
    timDelete(times);
    sms_cb_exit ();/*MC SPR1920 clean up cell broadcast*/
    mmeDelete(mme); mme = 0;
    kbdAlways(0);
    dspl_ClearAll();
	time_date_delete();/*SPR 1725*/
}

/*******************************************************************************

 $Function:  keyAction

 $Description:	 handler called on every keyboard action

 $Returns:

 $Arguments:

*******************************************************************************/
static int keyAction (MfwEvt e, MfwKbd *res)
{
  /* this function "keyAction" is called 4*times from MFW per each keypress */
  /* twice for keypress and twice for keyrelease */
  /* the variable "toggle" is using to avoid to play the keypadtone twice ! */
    static UBYTE toggle = TRUE;

	/* OMAPS00151698, x0056422 */
	#ifdef FF_MMI_A2DP_AVRCP
	if(tGlobalBmiBtStruct.bConnected == BMI_BT_CONNECTED && tGlobalBmiBtStruct.tCmdSrc == BMI_BT_HEADSET_COMMAND)
	{}
	else
	{
	#endif

#ifdef NEPTUNE_BOARD    /*  OMAPS00033660   */
    static unsigned char cKeySound = 0;
#endif

  /* this mask is use to filter the keyevent "e" */
  /* want to use KEY_0, KEY_1,......  KEY_HASH */
  USHORT  mask = 0x0FFF;
	
	/* Initialize - RAVI - 23-12-2005 */
	/*a0393213 warnings removal-variable conditionally removed*/
#if ((!defined(FF_MIDI_RINGER)) || defined(NEPTUNE_BOARD))
  UBYTE currentRinger = 0;
#endif 


  /* terminate the ringing in all situation (it doesnt matter where the user is */
  if ( (e & KEY_HUP) && (e & KEY_MAKE) )
  {
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
/* terminate the ringing */
#ifndef FF_MIDI_RINGER  
	currentRinger = getCurrentRingerSettings();
	audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );
#endif

  }

  /* the power-off-key ! */
  if ( (e & KEY_POWER) && (e & KEY_MAKE))
  {	
    TRACE_EVENT("KEY_POWER pressed !!!!");

	if (pinsIsFocussed() EQ FOCUSSED_PINS)
    {
    	pin_exit();
    	HUPKeyOrigin=1;
    }
    else if (idleIsFocussed())
	{
    	HUPKeyOrigin=1;
	}
    
    if ((HUPKeyOrigin==1) && (off_switch == 0))
	{
		/*
		** Only pass through this once, off_switch must be 0 because Key_Action is called twice
		** for each key press.
		*/
		TRACE_EVENT("In idle window.");
	    showGoodBye ( idle_get_window () ); 
	    times_switchoff = timCreate(0,THREE_SECS,(MfwCb)goodbye_cb);
	    timStart(times_switchoff);
	    off_switch++;
	    deregis = TRUE;
    }
  }

  HUPKeyOrigin = 0;


  /* every other calls */
  if(toggle)
  {
#ifdef SIM_TOOLKIT
       /*SPR#2121 - DS - Only download event to SAT if the event has been registered
        * by the SAT Setup Event List command
        */
      if (satEvtRegistered(SatEvtUserActionActivated) == TRUE)
      {
        satEvtDownload(SatEvtUserAction);
      }
#endif

/* Stop Playing ring tone - RAVI - 23-12-2005 */
 #ifdef NEPTUNE_BOARD    
	if(idle_data.new_sms == TRUE)
	{
		currentRinger = getcurrentSMSTone();
		audio_StopSoundbyID( AUDIO_BUZZER, currentRinger );

		/* Till this audio is stopped and next is played. */
		vsi_t_sleep( 0, 10 );
	}

	cKeySound = 1;  /*  OMAPS00033660   */
	if( idle_data.edt_buf[0] != '\0' )
	{
		if( idle_data.edt_buf[0] == '*'  ||  idle_data.edt_buf[0] == '#' )
      		{
      			cKeySound = 0;
		}
	}
	else
	{
		if ( (e & KEY_HASH) || ( e & KEY_STAR ) )
		{
			cKeySound = 0;
		}
	}

	if( cKeySound == 1 )
	{
      
 #endif   /*  OMAPS00033660   */
 /* END RAVI - 23-12-2005 */

    if ((FFS_flashData.settings_status & SettingsKeypadClick) && (e & KEY_MAKE))  
    {
      TRACE_EVENT ("Keypad Click activ");
/* Start Playing key tones on key press - RAVI 23-12-2005 */
#ifndef NEPTUNE_BOARD
     audio_PlaySoundID(0, TONES_KEYBEEP, 200, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */ 
#else
        /* RAVI - 20-1-2006 */
	/* Silent Implementation */
       /* Changed 0 to getCurrentVolumeSetting () */
    audio_PlaySoundID(0, (TONES_KEYBEEP), getCurrentVoulmeSettings(),
                                     AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
    }
    else if ((FFS_flashData.settings_status & SettingsKeypadDTMF) && (e & KEY_MAKE))
    {
      TRACE_EVENT ("Keypad DTMF activ");
      switch (e &= mask)
	{
	case KEY_0:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_0), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else
	  audio_PlaySoundID(0, (TONES_DTMF_0), 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_1:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_1), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else
	  audio_PlaySoundID(0, TONES_DTMF_1, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_2:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_2), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else		
	  audio_PlaySoundID(0, TONES_DTMF_2, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_3:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_3), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else				
	  audio_PlaySoundID(0, TONES_DTMF_3, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_4:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_4), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_4, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_5:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_5), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_5, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_6:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_6), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_6, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_7:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_7), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_7, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_8:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_8), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_8, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_9:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_9), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
 #else				
	  audio_PlaySoundID(0, TONES_DTMF_9, 0,AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_STAR:
/* RAVI  - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_STAR), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_STAR, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	case KEY_HASH:
/* RAVI - 20-1-2006 */		
#ifdef NEPTUNE_BOARD
       audio_PlaySoundID(0, (TONES_DTMF_HASH), getCurrentVoulmeSettings(), AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#else						
	  audio_PlaySoundID(0, TONES_DTMF_HASH, 0, AUDIO_PLAY_ONCE ); /* GW#2355 /CQ11341 */
#endif
/* END RAVI */
	break;
	default:
	  break;

      }
    }

#ifdef NEPTUNE_BOARD
      	}
 #endif

  }

	/* OMAPS00151698, x0056422 */
	#ifdef FF_MMI_A2DP_AVRCP
	}
	#endif



  /* toggle the variable */
  if(toggle)
  {
    toggle = FALSE;
  }
  else
  {
    toggle = TRUE;
  }


  return 0;
}

#if(0)/* x0039928 -Lint warning removal */
/*******************************************************************************

 $Function:  key

 $Description:	 keyboard event handler

 $Returns:    none.

 $Arguments:

*******************************************************************************/
// switching the mobile on
static int key (MfwEvt e, MfwKbd *kc)
{

TRACE_FUNCTION("Initial key");
//---------------start : Mobile On-------------------
    TRACE_EVENT("	      Mobile ON");
    /* set the state of mobile on   */
    globalMobileMode = (globalMobileMode | GlobalMobileOn);


//startExec(PhoneInit,0);	      /* get it rolling 	  */

    return 1;
}

#endif
//Callback after dialog "Charging" is displayed
//x0pleela 14 Apr, 2006 ER: OMAPS00075178
static void charging_start_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION("charging_start_cb()");
	/*
	** Send an Idle Update to trigger the idle screen to be updated if required,
	*/
	idleEvent(IdleUpdate);

	return;
}

//function to display the dialog "charging" when charger is connected
//x0pleela 14 Apr, 2006  ER: OMAPS00075178
static void charging_start_info(void)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("charging_start_info()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtCharging, TxtNull, COLOUR_STATUS);
	/*a0393213 warnings removal-callback fn typecasted*/
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)charging_start_cb, TWO_SECS, KEY_CLEAR );
	/*
	* Call Info Screen
	*/
	info_dialog (idle_get_window(), &display_info);


	return;
}
/*******************************************************************************

 $Function:  globalMme

 $Description:	MME event handler

 $Returns:    none.

 $Arguments:

*******************************************************************************/
int globalMme(MfwEvt e, MfwHnd para)
{
    MfwMme* mc = (MfwMme*)para;

    U8	value;
#ifdef FF_MMI_AUDIO_PROFILE
	 MfwHnd       win  = call_data.win_incoming;
	 MfwWin     * win_data ;
	 T_dialog_info * data;
	 T_MFW_HND       parent_win = NULL;
	 USHORT          Identifier = 0;
	 void (* Callback)() = NULL;

	if( win != NULL)
	{
		win_data = (MfwWin*)((T_MFW_HDR *)win)->data;
	 	data = (T_dialog_info *)win_data->user;
	 	parent_win = data->parent_win;
	 	Identifier = data->Identifier;
	 	Callback= data->Callback;
	}
#endif	
 
	TRACE_FUNCTION("globalMme");
	
    switch(e) {
    case MfwMmeSignal :
	TRACE_EVENT("Mme Event: Signal");
	value = (U8)mc->value;//JVJE
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1 ("Event MfwMmeSignal  %d", value);
	/***************************Go-lite Optimization changes end***********************/
	globalSignalUpdate(value); /*Update the value of the fieldstrength  */

	break;

    case MfwMmeBattery:
       TRACE_EVENT("Mme Event: Battery....");
	 TRACE_EVENT_P1("MfwMmeBattery:mc->value: %d", mc->value);
	globalBatteryUpdate(mc->value); //x0pleela 10 Apr, 2006	ER: OMAPS00075178
	break;


    case MfwMmeBaState:
	TRACE_EVENT("Mme Event: State....");

	value = (U8)mc->value;
	TRACE_EVENT_P1("MfwMmeBaState:mc->value: %d", mc->value);
	switch(value)
	{
//x0pleela 10 Apr, 2006 ER: OMAPS00075178
	case PWR_CHG_PLUG:
		TRACE_EVENT("Mme Event:PWR_CHG_PLUG");
		charging_start_info();
		break;
	/* External Power Plugged Off */
    //case 0: //x0pleela 10 Apr, 2006 
	case PWR_CHG_UNPLUG://x0pleela 10 Apr, 2006 ER: OMAPS00075178
      		TRACE_EVENT("Mme Event:Discharging");
	    	globalMobileMode = globalMobileMode & (~GlobalCharging);
	    	globalMobileMode = globalMobileMode & (~GlobalBatteryPower);
		globalBatteryMode = GlobalBatteryGreater5; //x0pleela 10 Apr, 2006
		timStop(battery_charging_animate ); //x0pleela 11 Apr, 2006
		idleEvent(IdleUpdate); //x0pleela 10 Apr, 2006
	break;

	/* Mme Event: External Power Connected or End of Charge */
 //      case 1: //x0pleela 10 Apr, 2006 
	case PWR_CHG_STOP: //x0pleela 10 Apr, 2006 ER: OMAPS00075178
      		TRACE_EVENT("Mme Event:GlobalBatteryPower");
//		globalMobileMode = globalMobileMode & (~GlobalCharging);
	    	globalMobileMode = (globalMobileMode | GlobalBatteryPower);
	    /* indicate that the battery is full*/
	  	globalBatteryMode = GlobalBatteryFull;
		timStop(battery_charging_animate ); //x0pleela 11 Apr, 2006
		idleEvent(IdleUpdate); //x0pleela 10 Apr, 2006
	 	 TRACE_EVENT("globalBatteryPicNumber = GlobalBatteryFull");
	break;
	/* Mme Event: Start of Charge */
//	case 2: //x0pleela 10 Apr, 2006
	case PWR_CHG_BEGIN: //x0pleela 10 Apr, 2006 ER: OMAPS00075178
	    TRACE_EVENT("Mme Event:Charging");	   
	    globalMobileMode = globalMobileMode & (~GlobalBatteryPower);
	    globalMobileMode = (globalMobileMode | GlobalCharging);
		globalBatteryMode = GlobalBatteryGreater5; //x0pleela 10 Apr, 2006
		idleEvent(IdleUpdate); //x0pleela 10 Apr, 2006
		//globalFlashBattery (); //x0pleela 10 Apr, 2006 : included to show animation of battery charging
	break;
	default:
	    break;
    }
    break;
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE	
//indicates the events related to the headset
	case MfwMmeHeadset:
// Aug 03, 2006 REF:OMAPS00088329  x0039928		
#ifdef MMI_TTY_ENABLED

	if(FFS_flashData.ttyAlwaysOn == FALSE && call_data.tty == CALL_TTY_OFF)

#endif	
	{
		value = (U8)mc->value;
		switch(value)
		{
			case AUDIO_HEADSET_HOOK_DETECT:
			{
				TRACE_EVENT("AUDIO_HEADSET_HOOK_DETECT");
				if( winIsFocussed(call_data.win_incoming))
				{	
					dialog_info_destroy(win);
					if(Callback != NULL) /* x0039928 - Lint warning fix */
					(Callback)(parent_win,Identifier, INFO_KCD_LEFT);
				}
				else if(call_data.win)
					mmi_hook_handle();
			}
			break;
			case AUDIO_HEADSET_UNPLUGGED:
				TRACE_EVENT("AUDIO_HEADSET_UNPLUGGED");
			//x0pleela 20 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK			
				//Set headset connect to FALSE
				mmi_set_Headset_Reg(FALSE);

				//x0pleela 23 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( !mfw_get_Phlock_status() )
				{
			#endif /*FF_PHONE_LOCK*/

				mmi_device_status_info(NULL,TxtHeadset,TxtRemoved,NULL);
				// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
				// Fix: Hook is unregistered when headset is plugged off.
				mfw_hook_unregister();
				
			//x0pleela 20 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK				
				}
			#endif /*FF_PHONE_LOCK*/

				mfw_audio_device_indication(value);
				break;

			case AUDIO_HEADSET_PLUGGED:
				TRACE_EVENT("AUDIO_HEADSET_PLUGGED");
			//x0pleela 20 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK				
				//set headset connect to TRUE
				mmi_set_Headset_Reg(TRUE);
			
				//x0pleela 23 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( !mfw_get_Phlock_status() )
			#endif /*FF_PHONE_LOCK*/	
			
				mmi_device_status_info(NULL,TxtHeadset,TxtInserted,NULL);

				mfw_audio_device_indication(value);

			//x0pleela 20 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK				
				//x0pleela 23 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( !mfw_get_Phlock_status() )
				{	
			#endif /*FF_PHONE_LOCK*/
				// Jun 02, 2006  DRT OMAPS00079213   x0039928(sumanth)
				// Fix: Hook is registered when headset is plugged in during call.
				if(call_data.win_calling OR call_data.win_incoming OR call_data.calls.numCalls)
					mfw_hook_register();
				
			//x0pleela 20 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK				
				}
			#endif /*FF_PHONE_LOCK*/
				break;
		}
	}
	
		break;
//indicates the events related to the carkit		
	case MfwMmeCarkit:
		value = (U8) mc->value;
		switch(value)
		{
			case AUDIO_CARKIT_UNPLUGGED:	
				TRACE_EVENT("AUDIO_CARKIT_UNPLUGGED");
				mmi_device_status_info(NULL,TxtCarkit,TxtRemoved,NULL);
				mfw_audio_device_indication(value);
				break;
			case AUDIO_CARKIT_PLUGGED:
				TRACE_EVENT("AUDIO_CARKIT_PLUGGED");
				mmi_device_status_info(NULL,TxtCarkit,TxtInserted,NULL);
				mfw_audio_device_indication(value);
				break;
		}
		break;
#endif
//Handler for connect and disconnect event of the USB cable.
#ifdef FF_MMI_USBMS		
	case MfwMmeUSBMS:
		{
			value = (U8) mc->value;
			switch(value)
			{
				case MFW_USB_CABLE_CONNECT_EVENT :
//x0pleela 16 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK
				//set usb_ms_enum to TRUE
				mmi_set_usbms_enum( TRUE);
				//x0pleela 23 May, 2007  DR: OMAPS00132483
				//get the phone lock status 			
				if( !mfw_get_Phlock_status() )
			#endif /*FF_PHONE_LOCK*/

				mmi_usb_enumeration_options();
				break;

				case MFW_USB_CABLE_DISCONNECT_EVENT :
					//x0pleela 16 Mar, 2007 ER: OMAPS00122561
			#ifdef FF_PHONE_LOCK
				//set usb_ms_enum to FALSE
				mmi_set_usbms_enum( FALSE );				
				//x0pleela 23 May, 2007  DR: OMAPS00132483
				//get the phone lock status 				
				if( !mfw_get_Phlock_status() )
			#endif /*FF_PHONE_LOCK*/

				mmi_usbms_mode_disconnect();
				break;
/* Mar 22, 2007  DVT: OMAPS00121916  x0039928 */	
#ifdef FF_MMI_FILEMANAGER
				case MFW_TFLASH_INSERTED_EVENT:
					mmi_device_status_info(NULL,TxtTflash,TxtInserted,NULL);
					break;
				case MFW_TFLASH_REMOVED_EVENT:
					mmi_device_status_info(NULL,TxtTflash,TxtRemoved,NULL);
					break;
#endif					
			}
				
		}
		break;
#endif

    default:
	return 1;
    }
    return 1;
}

#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:  backlight

 $Description:	switch on backlight for certain time

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static void backlight (void)
{


}
#endif
/*******************************************************************************

 $Function:  timesEvent

 $Description:	 times event  clock

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static int timesEvent (MfwEvt e, MfwTim *t)
{

		      //RM only for PC testing
/* x0039928 -Lint warning removal
			  second += 60; 			  */

 
   idleEvent(IdleUpdate);

   /*MC CONQUEST 5999->6007,21/05/02 if CPHS reading didn't work on switch-on, try again*/
   if (CphsPresent() == CPHS_ERR)
		cphs_support_check();
	

  mme_backlightEvent(BL_IDLE_TIMER);
	
  timStart(times);

  return 1;
}


/*******************************************************************************

 $Function:  showCLOCK

 $Description:	display clock

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void showCLOCK(void)
{	/*SPR 1725, use new function to get clock string*/
    dspl_TextOut(30,24,0,(char *)mfw_td_get_clock_str());
}
/*******************************************************************************

 $Function:  setClockFormat

 $Description:	used to switch between 24 hour and 12 hour display format

 $Returns:    none.

 $Arguments:  format enum

*******************************************************************************/
void setClockFormat(UBYTE format)
{/*SPR 1725, use new function to set clock format*/
  if (format == MFW_TIME_FORMAT_12HOUR)
    FFS_flashData.time_format = MFW_TIME_FORMAT_12HOUR;
  else
    FFS_flashData.time_format = MFW_TIME_FORMAT_24HOUR;
// Jul 22,2004 CRR:20896 xrashmic - SASKEN
// The timeformat was not being written into the flash.
  flash_write();
  
  mfw_td_set_time_format((T_MFW_TIME_FORMAT)format);
}
// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
// Bug : The AM PM for 12 hour fomrat was not being shown
// Solution : Added the following function to get the current time format from flash
UBYTE getClockFormat()
{
	  return    FFS_flashData.time_format;
}

/*SPR 1725, removed ClockSet and DateSet functions*/

/*******************************************************************************

 $Function:  showDATE

 $Description:	 display date

 $Returns:

 $Arguments:

*******************************************************************************/
void showDATE(void)
{/*SPR 1725, use new function to get date string*/
    dspl_TextOut(0,10,0,mfw_td_get_date_str());

}


/*******************************************************************************

 $Function:  displayAlignedText

 $Description:	Display text horizontally aligned [left|right|center] in specified row.


 $Returns:    none.

 $Arguments:

*******************************************************************************/
void displayAlignedText( T_MMI_ALIGN x_alignment, USHORT y_lineNum, UBYTE attrib, char* string )
{
    int stringLength = 0;
    USHORT startXPos = 0;      // starting X pos for display

    dspl_DevCaps deviceCaps;

    dspl_GetDeviceCaps(&deviceCaps);

	/* SPR#1983 - SH - Shouldn't use strlen for unicode string.
	 * 0 means whole string will be used. */
	 
    stringLength = (int)dspl_GetTextExtent(string, 0);

	TRACE_EVENT_P2("aligned %d, extent = %d", x_alignment, stringLength);
	
    switch ( x_alignment )
    {
	case LEFT:

	break;

	case RIGHT:
	    startXPos = (USHORT) deviceCaps.Width - stringLength /*- OneCharLen*/ ;
	break;

	case CENTER:
	    startXPos = (USHORT) (deviceCaps.Width - stringLength) / 2;
	break;

    }

  dspl_TextOut(startXPos, y_lineNum, attrib, (char*)string );
}
/*******************************************************************************

 $Function:  displayAlignedPrompt

 $Description:	Display prompt horizontally aligned [left|right|center] in specified row.

 $Returns:

 $Arguments:

*******************************************************************************/
void displayAlignedPrompt( T_MMI_ALIGN x_alignment, USHORT y_lineNum, UBYTE attrib, int string_id )
{
    int stringLength = 0;
    USHORT startXPos = 0;      // starting X pos for display

    dspl_DevCaps deviceCaps;

    dspl_GetDeviceCaps(&deviceCaps);


    /* Marcus: Issue 1123: 12/09/2002: Replaced use of strlen with dspl_str_length for Unicode */
    stringLength=dspl_str_length((char*)MmiRsrcGetText( string_id ));
     switch ( x_alignment )
    {
        case LEFT:

        break;

        case RIGHT:
            startXPos = (USHORT) deviceCaps.Width - stringLength /*- OneCharLen*/ ;
        break;

        case CENTER:
            startXPos = (USHORT) (deviceCaps.Width - stringLength) / 2;
        break;

    }

	PROMPT(startXPos, y_lineNum, attrib, string_id );
}
/*******************************************************************************

 $Function:  mainShutdown  

 $Description:	 

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void mainShutdown(void)
{

    TRACE_FUNCTION("mainShutdown ()");

    if(deregis)
    {
   //---------------start: Mobile OFF-------------------
	TRACE_EVENT("		     Mobile OFF");
	/* delete the state of mobile on   */
	globalMobileMode = globalMobileMode & (~GlobalMobileOn);

	deregis = FALSE;

    mmiExit();/* RM 20-07 */

//Jul 05, 2005    REF: SPR 31894 xdeepadh	
// SIM functionality has been replaced by UICC in Locosto.
#ifdef TI_PS_UICC_CHIPSET_15
	simdrv_poweroff(BSP_UICC_DEFAULT_READER_ID);
#else
	SIM_PowerOff();
#endif
#ifndef _SIMULATION_
    AI_Power(0);
#endif

    }
}
/*******************************************************************************

 $Function:  globalBatteryUpdate

 $Description:	Update the value of the battery after the MME event

 $Returns:

 $Arguments:

*******************************************************************************/
int globalBatteryUpdate (U8 value)
{

    MmiModuleDel (ModuleBattLow);  /* delete the state */

    globalBatteryMode = GlobalBatteryGreater5;


  switch (value)
  {
    case 0:	      /* battery low */
	  globalBatteryPicNumber = 0;
	  globalBatteryMode = GlobalBatteryLesser5;

	  /* is just a global status */
	  MmiModuleSet (ModuleBattLow);  /* set the state */
    break;

    case 1:
	  globalBatteryPicNumber = 1;
    break;

    case 2:
	  globalBatteryPicNumber = 2;
    break;

    case 3:
	  globalBatteryPicNumber = 3;
    break;

    case 4:
	  globalBatteryPicNumber = 4;
    break;

    default:
	  globalBatteryPicNumber = 2;
    break;
  }

    idleEvent(IdleUpdate);

    return 0;
}


/*******************************************************************************

 $Function:  globalSignalUpdate

 $Description:Update the value of the fieldstrength after the MME event

 $Returns:

 $Arguments:

*******************************************************************************/
int globalSignalUpdate (U8 value)
{

  globalSignalPicNumber = value;
  
  idleEvent(IdleUpdate);
#ifdef NEPTUNE_BOARD
  dspl_Enable(1);
#endif
  return 0;
}



/*******************************************************************************

 $Function:  globalFlashBattery

 $Description:	 exit MMI main module

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void globalFlashBattery (void)
{
    TRACE_EVENT(" globalFlashBattery ");

    globalBatteryPicNumber++;

    if(globalBatteryPicNumber > 4)
	globalBatteryPicNumber = 0;
   
  idleEvent(IdleUpdate);
    TRACE_EVENT(" start the timer for Blink ");


}

/* SPR#1428 - SH - New Editor: this is all obsolete with new editor.  Equivalent
 * functionality can be found in AUITextEntry.c */
 
#ifndef NEW_EDITOR
/*******************************************************************************

 $Function:  editShowHeader

 $Description:	 displays the new character over the cursor

 $Returns:    none.

 $Arguments:

*******************************************************************************/
/*MC SPR 1242 merged in b-sample version of this function */
static void editShowHeader (void)
{
	int	alphachar;
	MfwEdt *edit = ((MfwHdr *) myHandle)->data;

	TRACE_FUNCTION("editShowHeader");

	edtChar(myHandle,ecDel);
    if (upCase)
    	alphachar = editAlphaU[editAlphaKey][editAlphaLevel];
    else
        alphachar = editAlpha[editAlphaKey][editAlphaLevel];

    if (!(editAlphaMode && editAlphaKey != KCD_MAX))
    	alphachar= ' ';
    else if (editHiddenMode)													// SH - in hidden mode...
    	tmpBuf[edit->cp] = alphachar;											// store character in buffer
#ifdef NO_ASCIIZ
	/*MC, SPR 940/2 01/08/02, don't need to check for language, tag should be enough*/
	if (edit->attr->text[0] == 0x80/*MC 22/07/02*/)
		alphachar = alphachar << 8;
#endif
{/*NM p025*/
	edit->attr->alphaMode = TRUE;
}
	
	edtChar(myHandle,alphachar);

	/* 
	is this actually useless after insert a 
	character to call "ecRight" and then "ecLeft"
	For the multitap we need only two steps:
	"ecDel" plus insert the character
	
	edtChar(myHandle,ecLeft);*/


	{edit->attr->alphaMode = FALSE;}

	/*p025 end*/

}

/*******************************************************************************

 $Function:  editEventTim  

 $Description:	 executes the timer event from the edit component

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
/*MC SPR 1242 merged in b-sample version of this function */
static int editEventTim (MfwEvt e, MfwTim *t)
{
  char	*format = formatHandle; 		      // SH - temporary format template

  TRACE_FUNCTION("editEventTim");

    if (!(editAlphaMode || editFormatMode))
        return 1;


/*MC, SPR925 23/07/02, removed variable assignment for chinese */
	
	// SH - modifications for format mode & hidden mode
	if (editFormatMode)															// SH - advance format pointer when
	{
		if (editHiddenMode)
		{
			editCharFindNext('*');
		}
		else
			editCharFindNext(ecRight);
	}
	else if (editHiddenMode)
	{
		edtChar(myHandle,'*');
	}
	// end of modifications
	
	else
    	edtChar(myHandle,ecRight);
	
    editAlphaLevel = KEY_PAD_MAX-1;
    editAlphaKey = KCD_MAX;

    return 1;
}


/*******************************************************************************

 $Function:  editEventKey  

 $Description:	 executes the timer event form the edit component

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
int editEventKey (MfwEvt e, MfwKbd *k)
{
/*MC SPR 1242 merged in b-sample version of this function */
	MfwEdt *edit;
	char *tb;
    UBYTE  ch = '.';

    char		*format = formatHandle;											// SH - temporary format template
    char		formatchar;														// SH - Current format character
    char		alphachar;														// SH - Current multi-tap character
    int			safetycounter;													// SH - Stops searching forever to find compatible character
    BOOL		validCharacter;													// SH - flag to indicate whether character is accepted

	BOOL		unicode;														/* SPR957 - SH - TRUE if string is unicode */
	
	/*NM p026*/
	if (!myHandle)													// If we've reached the end of the format string, don't allow input 
		return 1;

	/* Over the editor handle "e" we have now the access to 
	 * the editor buffer "tb". We need it e.g "editCalcMode" 
	 * to check the buffer */
	 
    edit = ((MfwHdr *) myHandle)->data;
    tb = edit->attr->text;
	/*p026 end*/
	
	/* SPR957 - SH - TRUE if string is unicode */
    if (tb[0]==0x80)
    	unicode = TRUE;
    else
    	unicode = FALSE;
	
	/* SH - formatted input mode (for WAP) */
	
	if (editFormatMode)
	{
		if (k->code==KCD_HUP)													// Escape from edit screen
            return 0;

		if (edit->cp >= (edit->attr->size))									// If at last character of editor, don't
		{
			return 1;															// print character
		}
		
		formatchar = format[*formatIndex];
		
       	if (formatchar == NULL)													// If we've reached the end of the format string, don't allow input 
       		return 1;
       		
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')				// Delimiter for format field
    	{
        	(*formatIndex)++;
        	*fieldIndex		= 0;
        	formatchar 		= format[*formatIndex];							// Next character is the format for the field
		}
		        
        /* Multi-tap input
		 *
         * The user presses a different key, meaning the previous character must be entered */
        
    	if (editAlphaKey != KCD_MAX && editAlphaKey != k->code)
	{
	        timStop(editTim);

	        alphachar = NULL;
	        
	        switch(formatchar)
		{
    			case 'X':														// Uppercase alphabetic only
    			case 'A':														// Uppercase alphabetic or punctuation
    				alphachar = editAlphaU[editAlphaKey][editAlphaLevel];
					break;
					
    			case 'x':														// Lowercase alphabetic only
				case 'a':														// Lowercase alphabetic or punctuation
					alphachar = editAlpha[editAlphaKey][editAlphaLevel];
				break;

				case 'M':
				case 'm':
					if (upCase)
						alphachar = editAlphaU[editAlphaKey][editAlphaLevel];
					else
						alphachar = editAlpha[editAlphaKey][editAlphaLevel];
				break;
        	}

			if (alphachar!=NULL)
			{
				if (editHiddenMode)									// SH - in hidden mode...
            	{
	            	tmpBuf[edit->cp] = alphachar;							// store character in buffer
	            	alphachar = '*';								// ...and show star
            	}
				editCharFindNext(alphachar);									// Add number to buffer & advance cursor
				editAlphaLevel = KEY_PAD_MAX-1;
		        editAlphaKey = KCD_MAX;
		        if (edit->cp >= (edit->attr->size))							// If at last character of editor, don't
					return 1;													// allow another character
			}
        }

		formatchar = format[*formatIndex];
		
		if (k->code <= KCD_9)													// Key pressed is 0 - 9.
			{

			/* Numeric */
			
			if (formatchar == 'N')												// A numeric digit is expected
			{
				if (editHiddenMode)
            	{
					tmpBuf[edit->cp] = editControls[k->code];
					editCharFindNext('*');
            	}
				else
	        		editCharFindNext(editControls[k->code]);						// Add number to buffer & advance cursor
		        editAlphaLevel = KEY_PAD_MAX-1;									// Reset multi-tap settings
				editAlphaKey = KCD_MAX;
				return 1;
	        }

			/* Multi-tap alphanumeric */

			alphachar = NULL;

  			if (!strchr("XxAaMm",formatchar))									// If not a valid format code, exit
  				return 1;
  			
          	if (strchr("Xx",formatchar) && k->code<KCD_2)						// 0 and 1 keys not valid in these modes
        		return 1;

    		editAlphaKey = k->code;												// Store the key pressed
			editAlphaLevel++;													// Increment through list of symbols
			if (editAlphaLevel > (KEY_PAD_MAX-1))								// List wraps round
				editAlphaLevel = 0;
			
    		safetycounter = 0;
    		while (alphachar==NULL && safetycounter<KEY_PAD_MAX)
			{
    			validCharacter = FALSE;
    			
    			switch(formatchar)
				{
    				case 'X':
    					alphachar = editAlphaU[editAlphaKey][editAlphaLevel];	// Uppercase...
    					validCharacter = (alphachar>='A' && alphachar<='Z');	// ...alphabetic only
					break;
					
					case 'x':
    					alphachar = editAlpha[editAlphaKey][editAlphaLevel];	// Lowercase
    					validCharacter = (alphachar>='a' && alphachar<='z');	// ...alphabetic only
    				break;

    				case 'A':
    					alphachar = editAlphaU[editAlphaKey][editAlphaLevel];	// Uppercase...
    					validCharacter = (alphachar<'0' || alphachar>'9');		// ..non-numeric
    				break;

    				case 'a':
    					alphachar = editAlpha[editAlphaKey][editAlphaLevel];	// Lowercase...
    					validCharacter = (alphachar<'0' || alphachar>'9');		// ..non-numeric
    				break;

    				case 'M':
    				case 'm':
    					if (upCase)												// User-selected case...
    						alphachar = editAlphaU[editAlphaKey][editAlphaLevel];
    					else
    						alphachar = editAlpha[editAlphaKey][editAlphaLevel];
    					validCharacter = TRUE;									// ...any character accepted
    				break;
				}
    				
    			if (!validCharacter)											// If character isn't uppercase alphabetic,
				{															// find the next one that is,
					alphachar = NULL;
    				safetycounter++;											// (but don't search forever)
    				editAlphaLevel++;
    				if (editAlphaLevel > (KEY_PAD_MAX-1))
						editAlphaLevel = 0;
				}
			}

			if (alphachar!=NULL)				// If we've found a character, and string isn't max size
			{
				if (editHiddenMode)
            	{
					tmpBuf[edit->cp] = alphachar;
            	}
				edtChar(myHandle,alphachar);
				edtChar(myHandle,ecLeft);
				timStart(editTim);
			}
			else
				editAlphaKey = KEY_MAX;
			return 1;
			}

		if (k->code==KCD_HASH)													// Hash key
		{
			if (strchr("AaMm",formatchar))
			{
				if (editHiddenMode)
            	{
					tmpBuf[edit->cp] = ' ';
					editCharFindNext('*');
            	}
				else
					editCharFindNext(' ');										// Prints a space in this mode
				}
			return 1;
			}

		if (k->code==KCD_STAR)													// Star key
		{
			if (strchr("AaMm",formatchar))
			{
				if (editHiddenMode)
            	{
					tmpBuf[edit->cp] = '*';
            	}
				editCharFindNext('*');
			}
			return 1;
		}
		
        return 1;
	}
	
	/************************/
	/* Normal multi-tap mode */
	/************************/

    if (editAlphaMode)
    {
        if (editAlphaKey != KCD_MAX && editAlphaKey != k->code)
        {
            timStop(editTim);

/*MC SPR 925, 23/07/02, removed editshowheader for chinese*/
           	edtChar(myHandle,ecRight);
/*MC SPR 925, 23/07/02,removed shifting of alphachar, not needed here, merged from ealier code*/
        			
            editAlphaLevel = KEY_PAD_MAX-1;
            editAlphaKey = KCD_MAX;
        }
	
        if (k->code <= KCD_HASH)
        {
            editAlphaLevel++;
            if (editAlphaLevel > (KEY_PAD_MAX-1))
                editAlphaLevel = 0;
            if (editAlphaKey == KCD_MAX)						// Inserting a new character
        	{
	        	TRACE_EVENT("Inserting character.");
				/*MC, SPR 925, 23/07/02 removed insert space and ecLeft if Chinese*/
			
	            /* NM p01306
	               the patch p027 to reduce calling the edtChar as less as possible 
	               was not working for inserting characters.
	               This additional change should work now:
	            */
				if (unicode)	/*SPR957 - SH - use new unicode flag */
				{
		            moveRightUnicode((U16 *)&edit->attr->text[edit->cp*2],1);					
				}
				else
				{
		            moveRight(&edit->attr->text[edit->cp],strlen(&edit->attr->text[edit->cp]),1);
		        }
				/* NM p01306 END */
			}
		}
    	editAlphaKey = k->code;
    	editShowHeader();
        timStart(editTim);
        return 1;
    }

	alphachar = NULL;
	
    switch (k->code)
    {
        case KCD_HUP:
            return 0;
        case KCD_HASH:
	        /* KGT: If not in an alpha mode insert blanc. 
	                    But in numeric mode insert '#'. */
		    if (editAlphaMode)
		    {
		    	alphachar = ' ';
				editShowHeader();
		    }
		    else
		    {
		    	if (editCalcMode)
		    	{
					if(!(strchr(tb, ch ))) /* set '.' only once ! */
						alphachar = '.';
		    	}
		    	else
		    		alphachar = '#';
		    }
		    /* end KGT */

		    /* SH - hidden mode */
			if (editHiddenMode)
			{
				tmpBuf[edit->cp] = alphachar;
				alphachar = '*';
			}
			/* end of hidden mode */

			/* SPR957 - In unicode mode, character is shifted to second byte */
			if (unicode)
				alphachar = alphachar << 8;
			
		    edtChar(myHandle,alphachar);
	        return 1;
	    		
        case KCD_STAR:

	    	if (editCalcMode)
	    	{
				if(!(strchr(tb, ch ))) /* set '.' only once ! */
					alphachar = '.';		/* SPR957 - SH*/
		    		/*edtChar(myHandle,'.');*/
	    	}
	    	else
	    	{
	    		/*if (!editAlphaMode)
                	edtChar(myHandle,'*');
	    		else     */
	    		if (editAlphaMode)
	    		{	
	    			editShowHeader();
	    		}

	    		alphachar = '*';
	    		
	    	    /* SH - hidden mode */
                if (editHiddenMode)
            	{
					tmpBuf[edit->cp] = '*';
            	}
				/* end of hidden mode */

				/* SPR957 - SH - In unicode mode, character is shifted to second byte */
				if (unicode)
					alphachar = alphachar << 8;
				edtChar(myHandle,alphachar);
	    	}
            return 2;									// SH -  2 indicates that character WAS printed
            
        case KCD_ABC:
            editAlphaMode = (UBYTE) !editAlphaMode;
            if (editAlphaMode)
            {
                editAlphaLevel = KEY_PAD_MAX-1;
                editAlphaKey = KCD_MAX;
            }
            editShowHeader();
            return 1;
        default:
		if (editAlphaMode){
			timStop(editTim);
			edtChar(myHandle,ecRight);
			return 1;
		}
		break;
    }

    /* SH - hidden mode */
    if (editHiddenMode)
	{
    	tmpBuf[edit->cp] = editControls[k->code];
    	alphachar = '*';	/* SPR957 - SH*/
    	/* SPR957 - In unicode mode, character is shifted to second byte */
		if (unicode)
			alphachar = alphachar << 8;
			
    	edtChar(myHandle,alphachar);
	}
     /* end of hidden mode */
    else
    {	/*MC SPR 964, sets alphamode to FALSE when in digits mode, so that cursor moves right*/
    	edit->attr->alphaMode = editAlphaMode;
    	edtChar(myHandle,editControls[k->code]);
    }

    return 1;
}


/*******************************************************************************

 $Function:  editClear

 $Description:	SH - For formatted input, clears everything except fixed characters

 $Returns:

 $Arguments:

*******************************************************************************/
/*MC SPR 1242 merged in b-sample version of this function */
void editClear()
{
	int editIndex = 0;
	MfwEdt *edit = ((MfwHdr *) myHandle)->data;
    char *tb = edit->attr->text;
    
	*formatIndex = -1;															// Find first non-fixed character,
	edtChar(myHandle,ecTop);													// starting from the top.
	editCharFindNext(NULL);

	while (edit->cp <strlen(tb))
	{
		editCharFindNext(' ');														// Overwrite everything with spaces
	}
	edtChar(myHandle,ecTop);
	*formatIndex = -1;
	editCharFindNext(NULL);															// Return to the first non-fixed character
	
	return;
}


/*******************************************************************************

 $Function:  editCharFindNext 

 $Description:	SH - For formatted input, adds a character to the input buffer then finds
 				the next non-fixed character space for the cursor to occupy

 $Returns:		

 $Arguments:	character - the character (or code) to print
 
*******************************************************************************/
/*MC SPR 1242 merged in b-sample version of this function */
void editCharFindNext(char character)
{
	char	*format = formatHandle;												// SH - temporary format template
	char	formatchar;
	MfwEdt	*edit = ((MfwHdr *) myHandle)->data;
	UBYTE	inField = 0;								// =1 if entering field, =2 if in field 

	TRACE_EVENT("editCharFindNext");

	// Check for delimited field
	
	if (*formatIndex>0)
	{
		formatchar = format[*formatIndex-1];
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
			inField = 2;
	}
	
	formatchar = format[*formatIndex];
	if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
		inField = 1;
	
	// Check for cursor right at end of string - don't allow
	
	if (character == ecRight && edit->cp >= strlen(edit->attr->text) && *formatIndex>-1 && inField==0)
	{
		TRACE_EVENT("End of string.");
		return;
	}
	
	if (character!=NULL)														// First add the required character
	{
		edtChar(myHandle, character);											// to the buffer
	}

	// Check for start of fixed input field

	if (inField==1)
	{
		TRACE_EVENT("Entering field.");
		(*formatIndex)++;														// Get us into the field...
		*fieldIndex = 0;														// ...and reset the field index
		formatchar = *(format+*formatIndex);
		if (formatchar=='M')
			upCase = TRUE;
		if (formatchar=='m')
			upCase = FALSE;
		inField = 2;
	}
	
	// Check whether we're in a fixed input field, e.g. "4N" or "8X"

	if (inField==2)															// So we don't look back beyond start of string
	{
		TRACE_EVENT("Move on in field.");
		(*fieldIndex)++;													// Increment the position in the field
		if (*fieldIndex==(int)(formatchar-'0'))								// If we've entered the number of characters specified (note- will never happen for the '*' !)
		{
			TRACE_EVENT("Exiting field.");
			(*formatIndex)++;												// point to NULL at end of string (no more input)
		}
		return;
	}

	// If not, just look at next format character as usual
	
	(*formatIndex)++;															// Point to next character
	
	while (*formatIndex<strlen(format) && *(format+*formatIndex) == '\\')		// Fixed characters encountered
	{
		edtChar(myHandle,ecRight);												// Skip over them
		(*formatIndex)+=2;
	}

	if (*formatIndex>(strlen(format)))											// Don't look beyond end of string
		*formatIndex = strlen(format);
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
	/***************************Go-lite Optimization changes end***********************/
	return;
}

/*******************************************************************************

 $Function:  editFindPrev 

 $Description:	SH - For formatted input, finds the previous non-fixed character and
 				moves the cursor there if possible

 $Returns:		0 if the cursor position is not changed (nowhere to go)
 				1 if the previous character has been found
 				2 if the cursor was over the first non-fixed character

 $Arguments:	
 
*******************************************************************************/
/*MC SPR 1242 merged in b-sample version of this function */
int editFindPrev()
{
	char	*format		= formatHandle;											// SH - temporary format template
	int		editIndex;
	char	formatchar;
	MfwEdt	*edit = ((MfwHdr *) myHandle)->data;

	TRACE_EVENT("editFindPrev");
	
	if (edit->cp == 0)															// If cursor is at start of string, return 2
	{
		TRACE_EVENT("Exit - start of string found");
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
		/***************************Go-lite Optimization changes end***********************/
		return 2;
	}
	// First check whether we're in a fixed input field, e.g. "4N" or "8X"
			
	if (*formatIndex>0)															// So we don't look back beyond start of string
	{
		formatchar = *(format+*formatIndex-1);
		if ((formatchar>'0' && formatchar<='9') || formatchar=='*')				// If it's a number between 1 and 9, or a *
		{
			TRACE_EVENT("In delimited field.");
			edtChar(myHandle,ecLeft);
			if (edit->cp < edit->attr->size-1)									// (Don't decrement if at last char in string)
				(*fieldIndex)--;												// Decrement the position in the field
			
			if (*fieldIndex==0)													// If we've reached the beginning of the field
			{
				TRACE_EVENT("Getting out of field.");
				(*formatIndex)--;												// Get out of the field
			}

			
			TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
			
			if (edit->cp==(strlen(edit->attr->text)-1))		// Special case if last character - tell editor to shorten the string
			{
				TRACE_EVENT("Shorten string");
				return 3;
			}
			
			return 1;															// then we're done
		}
	}

	// If not (or if we've just come out of one) just look at next format character as usual

	editIndex	= *formatIndex-1;												// Make copy of format position, starting off to left
	 	
	while (editIndex>0)
	{
		if (*(format+editIndex-1)=='\\')										// If there's a fixed char
			editIndex -=2;														// Look back a further 2 characters
		else																	// If there's a non-fixed character
			break;																// then exit loop
   	}

	if (editIndex==-1)															// Go back from 1st character in editor
	{
		TRACE_EVENT("Exit - skipped over fixed character");
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
		/***************************Go-lite Optimization changes end***********************/
		return 2;
	}
	
	formatchar = format[editIndex-1];											
	if ((formatchar>'0' && formatchar<='9') || formatchar=='*')
		(*fieldIndex)--;
		
	if (editIndex>-1)															// Provided there is somewhere to go....
		{
		while(*formatIndex>editIndex)
		{
			if (edtChar(myHandle,ecLeft)==MfwResOk)								// move cursor there
				(*formatIndex)--;
			if (format[*formatIndex]=='\\')
				(*formatIndex)--;
		}
		TRACE_EVENT("Found new position.");
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
		/***************************Go-lite Optimization changes end***********************/
		return 1;																// Found new position
	}
	TRACE_EVENT("Position unchanged.");
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P2("formatIndex, fieldIndex: %d, %d", *formatIndex, *fieldIndex);
	/***************************Go-lite Optimization changes end***********************/
	return 0;																	// Position unchanged
}


/*******************************************************************************

 $Function:  editActivate  

 $Description:	 Activates the edit component

 $Returns:		none.

 $Arguments:	
 
*******************************************************************************/
void editActivate (MfwHnd e, int AlphaMode)
{
  TRACE_FUNCTION("editActivate");

    myHandle = e;
    /*NM p028*/
    if (!editTim)
        editTim = timCreate(0,1000,(MfwCb) editEventTim);
    editAlphaMode = ((AlphaMode != 0) && (AlphaMode != 4) && (AlphaMode != 5));
    editCalcMode  = (AlphaMode == 4);
    editFormatMode = (AlphaMode == 6);
    editHiddenMode = FALSE;
    editAlphaLevel = KEY_PAD_MAX-1;
    editAlphaKey = KCD_MAX;
}


/*******************************************************************************

 $Function:  editHiddenActivate

 $Description:	 SH - Activates the edit component for hidden alphanumeric entry

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void editHiddenActivate (char *buffer)
{
  TRACE_FUNCTION("editHiddenActivate");
    tmpBuf = buffer;
    editHiddenMode = 1;
  return;
}

/*******************************************************************************

 $Function:  editDeactivate

 $Description:	 deactivates the edit component

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void editDeactivate (void)
{
    //myHandle = 0; //SPR#1014 - DS - Commented out.

    if (editTim){
    timStop(editTim);
    timDelete(editTim);
  }
    editTim = 0;
}
/*******************************************************************************

 $Function:  activeEditor

 $Description:	 deactivates the edit component

 $Returns:    none.

 $Arguments:

*******************************************************************************/
MfwHnd activeEditor (void)
{
  return myHandle;
}

/*******************************************************************************

 $Function:	setformatpointers

 $Description:	SH - Sets static variables formatHandle and formatIndex, so thet editEventKey
				can access the format string
 
 $Returns:		

 $Arguments:	Pointer to format string, pointer to format index
				
*******************************************************************************/


void setFormatPointers (char *format, int *index, int *index2)
{

  TRACE_FUNCTION("setFormatPointers");
  formatHandle = format;
  formatIndex = index;
  fieldIndex = index2;
  return;
}

#endif /* NEW_EDITOR */

/*******************************************************************************

 $Function:  MmiModuleSet

 $Description:	 Set the status

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void MmiModuleSet (int module)
{
  switch(module)
  {
  case ModuleIdle:
  MmiModule = MmiModule & ModuleBattLow;
  MmiModule = MmiModule | ModuleIdle;
  break;
  case ModuleInCall:
  MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleInCall;
  break;
  case ModuleCall:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleCall;
  break;
  case ModuleMenu:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleMenu;
  break;
  case ModulePhon:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModulePhon;
  break;
  case ModuleSms:
  MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleSms;
  break;
  case ModuleBattLow:
  MmiModule=MmiModule | ModuleBattLow;
  break;
  case ModuleSAT:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleSAT;
  break;
  case ModuleAlarm:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleAlarm;
  break;
  case ModuleFax:
    MmiModule=MmiModule | ModuleFax;
  break;
  case ModuleDialling:
    MmiModuleDel(ModuleIdle);
  MmiModule=MmiModule | ModuleDialling;
  break;
  default:
  break;
  }

  return;

}

/*******************************************************************************

 $Function:  MmiModuleDel

 $Description:	Delete the status

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void MmiModuleDel (int module)
{
  switch(module)
  {
  case ModuleIdle:

  MmiModule = MmiModule & (~ModuleIdle);
  break;
  case ModuleInCall:
    MmiModule=MmiModule & (~ModuleInCall);
  break;
  case ModuleCall:
    MmiModule=MmiModule & (~ModuleCall);
  break;
  case ModuleMenu:
    MmiModule=MmiModule & (~ModuleMenu);
  break;
  case ModulePhon:
    MmiModule=MmiModule & (~ModulePhon);
  break;
  case ModuleSms:
    MmiModule=MmiModule & (~ModuleSms);
  break;
 case ModuleBattLow:
    MmiModule=MmiModule & (~ModuleBattLow);
  break;
  case ModuleSAT:
    MmiModule=MmiModule & (~ModuleSAT);
  break;
  case ModuleAlarm:
    MmiModule=MmiModule & (~ModuleAlarm);
  break;
  case ModuleFax:
    MmiModule=MmiModule & (~ModuleFax);
  break;
  case ModuleDialling:
  MmiModule=MmiModule & (~ModuleDialling);
  default:
  break;
  }

  return;

}

/*******************************************************************************

 $Function:	showGoodBye

 $Description:	 shows a goodbye Message when swiching off the mobile

 $Returns:    none

 $Arguments:

*******************************************************************************/


static void showGoodBye (T_MFW_HND win)
{
    T_DISPLAY_DATA   display_info;

  TRACE_FUNCTION("showGoodBye ()");
	dlg_zeroDisplayData(&display_info);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)goodbye_cb, FOREVER, 0 );

      /*
       * Call Icon
       */

  mmi_dialogs_insert_animation (info_dialog (win, &display_info), 400 ,(MfwIcnAttr*)&goodbye_Attr,animGoodbye);


}

/*******************************************************************************

 $Function:	goodbye_cb

 $Description:

 $Returns:

 $Arguments:

*******************************************************************************/


static void goodbye_cb (void)
{
  TRACE_FUNCTION("goodbye_cb ()");
  nm_deregistration();	      /* start deregistration procedure   */
}

#ifdef FF_MMI_AUDIO_PROFILE
void mmi_device_status_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_device_status_info");
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1,str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, THREE_SECS, KEY_LEFT | KEY_CLEAR | KEY_HUP);
	info_dialog(parent,&display_info);
}
#endif


/*******************************************************************************

 $Function:	menuBootTimeMsmt

 $Description: menu handler for 'Boot Time' menu. Shows time taken for various boot events

 $Returns:

 $Arguments:

*******************************************************************************/
int menuBootTimeMsmt(MfwMnu* m, MfwMnuItem* i)
{
	int boot_time[4];
	T_AUI_EDITOR_DATA editor_data;
	T_MFW_HND win  = (T_MFW_HND)bookCurrentWindow();
	
	memset(buffer,0,sizeof(buffer));
	get_boot_time(&boot_time);
	TRACE_EVENT_P4("pre boot   %d, app init  %d, ti logo  %d, conn log   %d",boot_time[0],boot_time[1],boot_time[2],boot_time[3]);
       sprintf((char*)buffer,"\nPre boot -%d ms \nApp init - %d ms \nModem Boot - %d ms \nNetwork sync - %d ms",boot_time[0],boot_time[1],boot_time[2],boot_time[3]);
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtBootTime, NULL);
	AUI_edit_SetEvents(&editor_data, 0, TRUE, FOREVER, NULL);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, buffer, 100);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);

	AUI_edit_Start(win, &editor_data);

	return 1;
}
/* ****************************************************************** */
#ifdef NEPTUNE_BOARD
void AI_Power(SYS_UWORD8 power)
{

}

void SIM_PowerOff (void)
{
  TRACE_FUNCTION ("SIM_PowerOff()");
}

#ifdef MMI_POWER_MANAGEMENT_TEST

/*******************************************************************************

 $Function:    	mmi_PowerManagement_screen_forever

 $Description:	This will be executed when closing Power Management Screen
 
 $Returns:		
 $Arguments:	window, Idetifier, reason
 
*******************************************************************************/

void mmi_PowerManagementStatus(T_MFW_HND win, USHORT identifier, SHORT reason)
{
    timStart( timer_handle );
    SVC_SCN_EnablePM( 0 );
    Screen_Update = 1;
}


/*******************************************************************************

 $Function:    	mmi_PowerManagement_screen_forever

 $Description:	Used to display Power Management running Screen
 
 $Returns:		0 - Success 
                1 - Error 


 $Arguments:	window, textId/TestString, color
 
*******************************************************************************/
T_MFW_HND mmi_PowerManagement_screen_forever( MfwHnd win,int TextId, char* TextStr, int colour )
{
	T_DISPLAY_DATA DisplayInfo;
	dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtCancel, TxtNull,  TxtNull, colour);
	dlg_initDisplayData_events( &DisplayInfo,  (T_VOID_FUNC)mmi_PowerManagementStatus, FOREVER, KEY_ALL );
	DisplayInfo.TextString 	= TextStr;
   	return info_dialog(win, &DisplayInfo);
}

/*******************************************************************************

 $Function:    	mmi_PowerManagement

 $Description:	Used to test the power Management
 
 $Returns:		0 - Success 
                1 - Error 


 $Arguments:	menu, menu item
 
*******************************************************************************/
int mmi_PowerManagement(MfwMnu* m, MfwMnuItem* i)
{
    mmi_PowerManagement_screen_forever(0, NULL, "Power Management Running...\n", 1 );
    timStop( timer_handle );   
    dspl_Enable(1);
    Screen_Update = 0;
    SVC_SCN_EnablePM( 1 );
	return 0;
}

#endif


#endif

/*OMAPS00098881(removing power variant) a0393213(prabakar) - screenUpdateOn() and screenUpdateOff() added*/
#ifdef FF_POWER_MANAGEMENT
/*******************************************************************************

 $Function:    	screenUpdateOn

 $Description:	menu handler for Phone settings --> screen update --> on
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int screenUpdateOn(MfwMnu* m, MfwMnuItem* i)
{
	if(FFS_flashData.refresh!=1)
		{
		/*Update the screen, start the timer (which is responsible for time update in idle screen) and save the status*/
		idleEvent(IdleUpdate);		
		timStart(times);
		timStart(timer_handle);/*starting mfw timer also*/
		FFS_flashData.refresh=1;
		flash_write();		
		}
      mmi_dialog_information_screen(0, TxtDone, NULL, NULL, 0);
	return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	mmi_PowerManagement

 $Description:	menu handler for Phone settings --> screen update --> off
 
 $Returns:	

 $Arguments:	menu, menu item
 
*******************************************************************************/
int screenUpdateOff(MfwMnu* m, MfwMnuItem* i)
{
	if(FFS_flashData.refresh!=0)
		{
		/*Stop the timer(so no time update occurs in idle screen) and save the status in flash*/
		timStop(times);
		timStop(timer_handle); /*stoping mfw timer also*/
		FFS_flashData.refresh=0;
		flash_write();
		}
	mmi_dialog_information_screen(0, TxtDone, NULL, NULL, 0);
	return MFW_EVENT_CONSUMED;
}

#endif

