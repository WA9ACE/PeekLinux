
/*******************************************************************************

					CONDAT (UK) 

********************************************************************************									      

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.				   
									      
********************************************************************************

 $Project name: Basic MMI						       
 $Project code: BMI (6349)							     
 $Module:		MMI
 $File: 	    MmiPins.c
 $Revision:		1.0							  
									      
 $Author:		Condat(UK)							   
 $Date: 	    25/10/00							  
									       
********************************************************************************
									      
 Description:
 
  MMI PIN / PUK entry handling
			
********************************************************************************

 $History: MmiPins.c

	May 24, 2007  DR: OMAPS00132483 x0pleela
	Description: MM: While playing an AAC, unplug USB freezes the audio
	Solution: If any Audio file is playing, then stopping them before enabling/disabling phone lock,
				auto phone lock and changing unlock password.
			Changed the name of fucntions for setting and restting the headset/usb bit in 
				ph_lock status variable
	
 	May 23, 2007    DR: OMAPS00127483  x0pleela
	Description:   Phonelock - Unable to unlock the phone during incoming call with ringer...
	Solution: 	When ringer for either incoming call or alarm is playing in phone locked state, Audio 
			usues the DMA channels which will be active. The DMA channels will be available only 
			after the audio file is fully played. This will cause that process to be blocked until the 
			DMA channel becomes in-active and in turn if we try to query or access secure driver 
			we are blocked until the audio file is completely played. Since this is a hardware 
			constraint, we found a workaround for this issue and are stopping the ringer  
			either for incoming call or alarm before accessing secure driver for enabling or 
			disbaling the phone lock.

	May 09, 2007  DR: OMAPS00129014
	Description: COMBO: unable to send SMS during midi playing
	Solution: Setting the auto phone lock flag to TRUE / FALSE when automatic phonelock is 
				on / off respectively	
	
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 

      Feb 19, 2006 ER:OMAPS00115179 a0393213
      Description : support SIM insertion without powering down
      Solution	   : Unnecessary windows cleared during SIM insertion

 	Nov 13, 2006 DR: OMAPS00103356  x0pleela
 	Description: SIMP:Master unlock failed through MMI
 	Solution: 1) In function pins_editor_cb() for INPUT_MASTER_KEY_REQ, 
			- An event SMLK_SHOW_WRONG_PWD is sent for wrong Master unlock password
			- An event SMLK_SHOW_DEP_CATS_UNLOCKED is sent which says all the dependent 
				categories are unlocked
			2) In function pin_messages(): 
 			- An event SMLK_SHOW_WRONG_PWD is handled to display a dialog saying Master 
 				Unblock Password is wrong
			- An event SMLK_SHOW_DEP_CATS_UNLOCKED to display a dialog saying Master 
				unlocking operation is successful

    	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Made the following changes
			1) sim_event_cb_main()
				- Handling the event MFW_SIM_PBLOCK_REQ for requesting unlocking password 
					of "Blocked Network" category
			2) simp_busy_cb()
				- Added a check for CME class and Ext class and handled all CME errors 
					and EXT errors separately
			3) pin_main()
				- Handling the event INPUT_PBLOCK_PIN_REQ to prompt the user to enter the 
					Blocked Network Password (PIN or PUK)
			4) pin_info_cb()
				- Added a check for CME class and Ext class and handled all CME errors and EXT 
					errors separately for event SMLK_MASTERKEY and SMLK_PUK
			5) simlock_check_PBCatLock()
				- Added new function which gets the lock status of Blocked Network category 
					and accordingly display the menu item
			6) simp_unlock_Category()
				- Handling "Blocked Network" category and send INPUT_PBLOCK_PIN_REQ event
			7) setLockTypePb()
				- Added new function which sets the global flag to the Personalisation lock
			8) set_pin_messages()
				- Updating the variable which prompts the user to enter Blocked Network password
			9) pins_editor_cb()
				- Handling the event INPUT_PBLOCK_PIN_REQ fr Blocked Network category
				- Added a check for CME class and Ext class and handled all CME errors and EXT 
					errors separately
 	
 	Sep 26, 2006 DR: OMAPS00096565  x0pleela
 	Description: After blocking PIN, when ISAMPLE is reset, instead of prompting for PUK, master 
 				unlock key is asked
 	Solution: Added a check for C_KEY_REQ in functions sim_event_cb_main(), pin_info_cb() .
 			If this variable is set the SIMP menu will be isplayed 
 			else user wil be prompted to enter PUK
 

 	xashmic 21 Sep 2006, OMAPS00095831
 	Description: USB enumeration does not happen in PIN/PUK/Insert SIM scenarios
 	Solution: When booting up without SIM card, or prompting for PIN/PUK/SIMP entry, 
 	enumerate all the USB ports automatically 
 	
 	Sep 11, 2006 DR: OMAPS00094215  x0pleela
 	Description: MMI compilation failed as CME_ErrBusy undefined
 	Solution: Handling new Ext_ERR_Busy sent by ACI

 	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution:  - Handling MFW_MEPD_INVALID event in function sim_event_cb_main()
			- Handling NO_MEPD event in functions pin_main(), pin_edt_kbd_cb(), check_pins(), 
				pin_editor(), pin_messages(), check_set_pins(), pins_editor_cb(), show_confirmation_cb()

	July 31, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security
	Solution:Made the following changes
	1) Added new element mk_pin to store Master Unlock key to T_Pin structure
	2) pin_main(), pin_messages(), setting_pin_main(),set_pin_messages(): Added new event SIM_LOCK_BUSY_UNBLOCK 
		to display a dialog "Busy" when wrong unlock password is entered after 1st attempt if timer is running
	3) pin_info_cb(): Handling CME_ERR_Busy 
	4) mmi_simlock_reset_fc_value(), pins_editor_cb(): Handling CME_ERR_Busy error and sending new event 
		SIM_LOCK_BUSY_UNBLOCK 	to display a dialog "Busy" when wrong unlock password 
		is entered after 1st attempt if timer is running
	
	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security
				
	Solution:Made the following changes
	1) sim_event_cb_main(): check for Master Unlock option enabled. If so, then display the menu 
				  with options Unblock ME and Master Unlock. Similar for all the categories also.
	2) pin_main(): 	
		a) Handling SIM_LOCK_BUSY_BOOTUP event which displays a dialog "Busy" for the user 
			if timer is enabled and ACI returns Busy error when user had entered wrong unlocking 
			password
		b) Handling events SMLK_SHOW_MK_FAIL for wrong master unlock key, 
			SMLK_SHOW_MK_SUCC for correct master unlock key

	3) pin_messages(): 
		a) Handling event SMLK_SHOW_MK_FAIL to show a dialog saying "Master Unlock Failed"
		b) Handling event SMLK_SHOW_MK_SUCC to show a dialog saying "Master Unlock Success"
		c) For event SMLK_SHOW_FC_SUCC, changed the event from PIN_OK to SMLK_PUK bcoz 
			after unblocking ME, for event PIN_OK it would go to idle screen without displaying 
			menu for unlocking any categories if any
		d) Delete the simp_list window handler after displaying the dialog immaterial of whether 
			the unlock/unblock passwords are correct or not

	4) pin_info_cb():	
		a) Handling PUK1_REQ separately as a menu should be displayed for Unblocking ME and 
			Master Unlock
		b) Handling SMLK_MASTERKEYto display menu for Unblocking ME or unlocking categories 
			if any and Master Unlock
		c) Made changes in the code for event SMLK_PUK to display menu Unblocking ME or 
			unlocking categories if any and Master Unlock

	5) setting_pin_main():
		a) Handling event SIM_LOCK_REQ_MKPWD for Master unlock ket request
		b) Handling event SIM_LOCK_MKPWD_FAIL for Master unlocking failed
		c) Handling event SIM_LOCK_MKPWDSUCC for Master unlocking success
		d) Handling new event SIM_LOCK_BUSY to display a dialog Busy for wrong entry of 
			unlocking pwd and timer been enable

	6) mmi_display_result(): Handling CME error busy while unlocking a locked category and send 
		SIM_LOCK_BUSY event

	7) mmi_simlock_en_dis_verify(): Handling CME error busy while unlocking a locked category 
		and send SIM_LOCK_BUSY event

	8) set_pin_editor(): Handling event INPUT_SIM_MKPWD

	9) set_pin_kbd_cb(): Handling event INPUT_SIM_MKPWD

	10) set_pin_messages(): Handling events SIM_LOCK_MKPWD_FAIL for Master unlock failed, 
		SIM_LOCK_MKPWDSUCC for Master unlock success, SIM_LOCK_BUSY for category unlock 
		return with Busy error from ACI

	11) pins_editor_cb(): 
		a) Update mmi_simlock_aciErrDesc with the latest CME error
		b) Event INPUT_MASTER_KEY_REQ performs master unlock and sends corresponding 
			events during bootup
		c) Delete the simp list window handler immaterial of whether the unlock/unblock
			passwords are correct or not
		d) Get the busy state "Simp_Busy_State" and send event to display a dialog Busy 
			for the user
		e) Check for Master Unlock option enabled. If so, then display the menu with options 
			<Category> unlock and Master Unlock

	12) Added following new functions:
		a) simp_busy_cb: This function checks for the ACI CME error and display menu for 
			unlocking  categories or unblocking ME
		b) mmi_simlock_category_menus: Starts the simlock category list menu
		c) SIMLock_unblock_ME: This function sends an event to display an editor for the user to 
			enter unblocking code
		d) SIMLock_MasterKeyUnlock_bootup: This functioh gets called to unlock category /unblock 
			ME during bootup sequence
		e) SIMLock_MasterKeyUnlock: This functioh gets called to unlock category /unblock ME 
			through SIMP menu
		f) simlock_check_masterkey: If there is no support for Master Unlocking the item "Master unlock"
			will not appear on the menulist
		g) simlock_get_masterkey_status: This function gets the master key status
		h) simlock_check_NWCatLock: This function gets the lock status of Network category and 
			accordingly display the menu item
		i) simlock_check_NSCatLock: This function gets the lock status of Network subset category and 
			accordingly display the menu item
		j) simlock_check_SPCatLock: This function gets the lock status of Service Provider category and 
			accordingly display the menu item
		k) simlock_check_CPCatLock: This function gets the lock status of Corporate category and 
			accordingly display the menu item
		l) simlock_check_SIMCatLock: This function gets the lock status of SIM category and accordingly 
			display the menu item
		m) simlock_check_CatBlock: This function gets  the ME status and accordingly display the menu item
		n) mmi_simlock_master_unlock: This function unlocks category/unblocks ME and sends event 
			accordingly through SIMP menu
		o) simp_unlock_Category: This function sends corresponding event to unlock a locked category 
			through dynamic menu

       July 11, 2006    DR: OMAPS00084081   x0039928
       Descripton: CT-GCF-LL[27.22.4.7.2]-No IMSI attched request after SIM reset
       Solution: Network registration is initiated on sim reset.

	July 05, 2006    DR: OMAPS00084642   x0pleela
       Descripton: SIMP: we can unlock category with invalid password (more than 16 digit)
       Solution: Changed the size of simlock password enty from MAX_DIG to MAX_PIN+1
       
    xreddymn 4, Jul, 2006 OMAPS00083495
    Resolved issues reported in PIN and PUK input through BPM.

       June 07, 2006    DR: OMAPS00080701   x0021334
       Descripton: Phone hangs while unblocking SIMP during bootup.
       Solution: The assignment of  'SIMP_BOOTUP' to 'sim_unlock_in_prog' to indicate that
       the operation is happeneing during boot-up sequence was not taking place since the 
       respective function was not being called when SIMP was blocked and re-boot was done.
       This has now been corrected.

	xrashmic 6 Jul, 2006 OMAPS00080708
	The pin window is necessary for displaying the error message for MFW_SS_FAIL

	xrashmic 1 Jul, 2006 OMAPS00075784
       Fixed the blue screen issue in pin, pin2 and simp scenarios
	
	May 15, 2006 DR:OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution: Made the following changes
			Function: pin_main()
					1. Change the text display based on the PUK1 or Unblock code request
					2. Added code to close the editor on PIN_OK_END event
			Function: pins_editor_cb()
					1. Modified the code to check SIMP during bootup sequence

 	Apr 19, 2006 DR:OMAPS00067912 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution: Made changes in the functions sim_event_cb_main(), pin_main(), pin_messages(), 
				pin_info_cb(), setting_pin_main(), pins_editor_cb()to handle SIMP unblocking code

	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!
 
	Nov 17, 2005 DR: OMAPS00050447 - nekkareb
	Description: Even after removing the SIM , Hutch or Airtel network is seen
	Solution : Sim removal event is now being handled and appropriate display shown.
    Nov 15,2005 	DR : OMAPS00057280	-	x0034700
 	Description:	Compilation Errors in Neptune build while integrating  latest MMI Changes
 	Solution:		Included the compilation FLAG "NEPTUNE_BOARD" for un wanted code for Neptune build


	Nov 03, 2005 DR: OMAPS00050595 - xpradipg
	Description: SIMP: If the SIM is blocked (by entering invalid PIN1), the user is 
			    not able to unblock it even after entering the valid PUK1
	Solution : The global flag for SIM Personlaization PUK request was set even
			for the SIM PUK request. This setting is removed. Also the condition
			check has been done first for the global flag and then the rest of the
			checks.
 
	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
	Description : Locosto: SIMP - integration of issues submitted by solDel on 
			     ME Personalization - changes as per the new interfaces
	Solution	:   The return value of sAT_PlusCLCK of AT_EXCT is handled and 
			    a please wait screen is displayed until the response is recieved 
			    
	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
  	Description: Integration changes required with new aci labe
  	Solution: changes to adopt to the new ACI label   

	July 19, 2005 REF: CRR LOCOSTO-ENH-28173 xpradipg
  	Description: To provide MMI Support to enable/disable/change password and 
  				query all the Personalization locks
  	Solution: Integration of the changes for the same provided by the soldel 
  			  team

 	June 16, 2005  REF: CRR 31267  x0021334
	Description: Handset ignore the initializtion of the PIN1/PIN2
	Fix:	Cheking is done to ascertain if PIN1/PIN2 are initialised. If not, appropriate
	       message is displayed to the user.

//  May 31, 2004        REF: CRR 17291  xvilliva
//	Bug:	After power cycle the setting of the used line which have been made 
//			from the BMI is lost.
//	Fix:	The Pin2 which is entered in the editor and sent for verification is 
//			stored in a global variable for re-use in sAT_PlusCLCK().

//  May 13, 2004        REF: CRR 13632  xvilliva
//			The PUK1 screen is not brought up even if user enters wrong Pin1 thrice
//			from Idle screen -"**04*OLD_CHV1*NEW_CHV1*NEW_CHV1#".
//			If the Pin1 fails for 2 times - "Not accepted" screen is displayed.
//			but if the Pin1 fails 3rd consecutive time - "Pin Blocked" screen is displayed 
//			and the user is taken to PUK1 screen.
//  May 13, 2004        REF: CRR 13623  xvilliva  
//			The PUK1 screen can be exited by pressing HangUP or Back key.
//			The PUK1 screen if contains less than 1 character then back key is disabled.
//			Hangup key is disabled permenantly to avoid exiting the screen.


	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

  // Issue Number : SPR#15692 on 25/03/04 by Rashmi.C.N.

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
#include "mfw_sys.h"

#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_cm.h"
#include "mfw_icn.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_ss.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_tim.h"
#include "mfw_mnu.h"
#include "mfw_sms.h"

//x0pleela 15 May, 2007 DR: OMAPS00127483
#ifdef FF_PHONE_LOCK
#ifdef FF_MIDI_RINGER  
#include "mfw_midi.h"
#endif //FF_MIDI_RINGER 
#endif /* FF_PHONE_LOCK */

#include "dspl.h"


#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUIPinEditor.h"
#else
#include "MmiEditor.h"
#endif

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiIdle.h"
#include "MmiCall.h"
#include "MmiNetwork.h"
#include "MmiSounds.h"
#include "MmiSettings.h"
#include "MmiServices.h"
#include "Mmiicons.h"
#include "MmiDialogs.h"
#include "MmiPins.h"
#include "MmiCPHS.h"
#include "cus_aci.h"
#ifndef NEPTUNE_BOARD
#include "p_sim.h"
#endif
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"
#include "mmiSmsIdle.h" //GW SPR#1035 - For smsidle_unset_ready_state(void) definition.
#ifdef SIM_PERS
#include "aci_cmh.h"
#include "cmh.h"
#endif
//#define CLEAR_PIN_EDIT_AREA dspl_Clear(0,30,LCD_X,LCD_Y);


//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#include "usb/usb_api.h"
#include "mfw_mme.h"
//x0pleela 15 May, 2007 DR: OMAPS00127483
#include "MmiSounds.h"
#endif /* FF_PHONE_LOCK */

char g_pin2[MAX_PIN+1];

#ifdef SIM_PERS
//x0pleela 25 Apr, 2006  DR: OMAPS00067919
//Global variable to store the code result of qAT_PlusCPIN() result
extern T_ACI_CPIN_RSLT simp_cpin_code; 
extern UBYTE simp_cpin_flag;
#endif


/* PROTOTYPS */

static int pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static int pin_edt_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static int pin_edt_kbd_cb (T_MFW_EVENT event, T_MFW_KBD * kc);
static void check_pins (T_MFW_HND win,void * edt_pin);
static void gsm_sec_execute(T_MFW_HND win,void *string);
static void pin_edt_tim_out_cb (T_MFW_EVENT event,T_MFW_TIM * t);
static int pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void pin_icons(void);
static void clear_edit_array (void * parameter);
static void emerg_pin(void * string);
static int sim_rem_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int gsm_test(T_MFW_HND win,void * edt_pin);
static void pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void sim_rem_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc);
static void main_call_edit(T_MFW_HND win, USHORT event);
static void main_call_mess(T_MFW_HND win,USHORT event);
static int setting_pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static int set_pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void sett_pin_mess(T_MFW_HND win,USHORT event);
static void check_set_pins (T_MFW_HND win,void * edt_pin);
static int set_pin_kbd_cb (T_MFW_EVENT event,T_MFW_KBD *  kc);
static void set_pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int set_pin_win_cb (T_MFW_EVENT event,T_MFW_WIN * win);
static void sett_pin_edit(T_MFW_HND win,USHORT event);
static int gsm_set_test(T_MFW_HND win,void * edt_pin);
static void set_mode_fdn_adn(T_MFW_HND win,void * string);
static void check_plock_to_clock(T_MFW_HND win,USHORT event);
static void check_nlock_to_clock(T_MFW_HND win,USHORT event);
static void check_splock_to_clock(T_MFW_HND win,USHORT event);
static void check_nslock_to_clock(T_MFW_HND win,USHORT event);
static void check_sim_clock(T_MFW_HND win,USHORT event);
static void pin_ch_end_or_abort (T_MFW_HND win);
static void pin_verif_or_check2_end(T_MFW_HND win);
static void pin1_en_dis_verify(T_MFW_HND win);
static int set_pin_edt_win_cb (T_MFW_EVENT event,T_MFW_WIN * win);
static int set_pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc);
static void gsm_sec_execute_set(T_MFW_HND win,void *string);
void pin2_not_available_screen(void);  /* Warning Correction */
static void not_avail_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void pins_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
void pin1_not_enabled_screen(void);  /* Warning Correction */

// June 16, 2005  REF: CRR 31267  x0021334
// Call back function for handling uninitialised PIN1 condition
void pin1_cb_function (T_MFW_HND win, UBYTE identifier, UBYTE reason);

static void pins_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason);
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
void pins_loadEditDefault (T_AUI_EDITOR_DATA *editor_data, USHORT TitleId, USHORT Idenfitier);
#else /* NEW_EDITOR */
void pins_loadEditDefault (T_EDITOR_DATA *editor_data);
#endif /* NEW_EDITOR */
/* SPR#1746 - SH - Add identifier parameter */
void show_confirmation (T_MFW_HND win, USHORT Identifier);
static int show_confirmation_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);

void pin_skClear( void );
void pin_rectClear( MfwRect *win );


#ifdef SIM_PERS
//this global variable is used to track which lock was selected
S16 mmi_simlock_locktype =0xFF;
//	Nov 03, 2005 DR: OMAPS00052032 - xpradip
//	holds the info display handle
static T_MFW_HND info_disp = NULL;
//this global variable is used to track the action to be performed on the selected category
S16 mmi_simlock_lockaction = 0xFF;
static T_ACI_ERR_DESC mmi_simlock_aciErrDesc = 0;
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	added prototypes require for new compiler 2.54
static void mmi_simlock_reset_fc_value(T_MFW_HND win);
static void mmi_simlock_en_dis_verify(T_MFW_HND win);
#endif

extern T_MFW_SS_RETURN mfw_simlock_enable_lock(U8 *pin, S16 lcktyp);
extern T_MFW_SS_RETURN mfw_simlock_disable_lock(U8 *pin, S16 lcktyp);

#define NUM_OF_PLOCK	 6
// the coordinate of the TxtId 
//moved info text defines to MmiResources.h
#define PIN_ENTRY_MAX	3
#define STAR		0x2a
#define HASH		0x23
#define FIRST_DIGIT	  1
//#define NUM_OF_PLOCK	 6
#define IDENT_GSM_1	 5
#define IDENT_GSM_2	 6
#define TIM_LOCK	 0x01
#define TIM_LOCK_JUMP	 1000
typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   parent_win;
  T_MFW_HND	   pin_win;
  T_MFW_HND	   sim_handle;
#ifdef SIM_PERS
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
//New window handler for the new menu "menuBootupCatList"
  T_MFW_HND	   simp_list_win;
  UBYTE 	   mk_pin[MAX_PIN+1];	    /* Master Unlock Key*/
#endif
  int		   pin_retries; 	/* number of  pin attempts  */
  UBYTE 	   puk_request;
  USHORT		   display_id1;
  USHORT		   display_id2;
  UBYTE 	   gsm_state;		/* status gsm in security   */
  SET_CASE	   set_state;		/* setting state	    */
  UBYTE 	   pin[MAX_DIG+1];	    /* number string pin	*/
  UBYTE 	   puk[MAX_PIN+1];	/* number string puk	    */
  UBYTE 	   new_pin[MAX_PIN+1];	/* store new pin	    */
  UBYTE 	   old_pin[MAX_PIN+1];	/* store old pin	    */
  PIN_CASE	   pin_case;		/* editor state 	    */
  char				edtbuf[MAX_DIG];  
} T_pin;
typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   parent_win;
  T_MFW_HND	   pin_mess_win;
  USHORT		   display_id1;
  USHORT		   display_id2;
  PIN_CASE	   pin_case;
}T_pin_mess;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   parent_win;
  T_MFW_HND	   pin_edt_win;
  T_MFW_HND	   kbd_handle;
  T_MFW_HND	   kbd_long_handle;
#ifdef NEW_EDITOR		/* SPR#1428 - SH - New Editor changes */
  T_ED_DATA *	editor;
#else /* NEW_EDITOR */
  T_MFW_HND	   editor_handle;
#endif /* NEW_EDITOR */
  T_MFW_HND	   tim_out_handle;
  UBYTE 	   pin[MAX_PIN+1];	    /* number string pin	*/
  UBYTE 	   puk[MAX_PIN+1];	/* number string puk	    */
  UBYTE 	   new_pin[MAX_PIN+1];	/* store new pin	    */
  UBYTE 	   old_pin[MAX_PIN+1];	/* store old pin	    */
  UBYTE 	   editor_index;
  PIN_CASE	   pin_case_edit;
  UBYTE 	   gsm_state;		/* status gsm in security   */
  UBYTE 	   emergency_call;
  char edtbuf[MAX_DIG];
#ifdef NEW_EDITOR		/* SPR#1428 - SH - New Editor changes */
  T_ED_ATTR editorPinAttr;
#else /* NEW_EDITOR */
  MfwEdtAttr editpinAttr;
#endif /* NEW_EDITOR */
}T_pin_edt;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND	   parent_win;
  T_MFW_HND	   sim_rem_win;
  T_MFW_HND	   sim_handle;
  PIN_CASE	   pin_case;
  USHORT		   display_id1;
} T_sim_rem;

LOCAL T_sim_rem sim_rem_data;

static T_MFW_HND pin_windows;
static T_MFW_HND pin_edit_windows;
static T_MFW_HND pin_editor_window;
static T_MFW_HND pin_mess_windows;
static T_MFW_HND set_pin_windows;
/* static T_MFW_HND sim_rem_handle;   x0039928-Lint warning removal */

//API define for a flag for PIN Emergency Entry
UBYTE pin_emergency_call;
char pin_emerg_call[MIN_PIN];

#ifdef SIM_PERS
UBYTE FCUnlock_flag;
UBYTE perm_blocked;
UBYTE gsim_status;
/*a0393213 warnings removal-sim_unlock_in_prog made to be of type T_MFW_SIMP_CLCK_FLAG*/
EXTERN T_MFW_SIMP_CLCK_FLAG sim_unlock_in_prog; // June 07, 2006    DR: OMAPS00080701   x0021334
#endif
//June 16, 2005  REF: CRR 31267  x0021334
BOOL pin1Flag = FALSE; 

// June 16, 2005  REF: CRR 31267  x0021334
extern BOOL pin2Flag; // This variable will be needed here

//xashmic 21 Sep 2006, OMAPS00095831
//To track the various stages during bootup
T_BOOTUP_STATE BootUpState = BOOTUP_STATE_NONE;

//x0pleela 08 ep, 2006  DR: OMAPS00091250
//Adding the prototype
void mfw_flash_write(char *LogMsg,...);

//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
EXTERN T_call call_data; 
static int autoPhLock_menu;
T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
T_MFW_HND	phlock_kbd_handle;			/* to hold the kbd handle of phone unlock editor window 	*/
T_MFW_HND	phlock_alarm_win_handle; 	/* to hold the win handle of alarm window 				*/
T_MFW_HND	phlock_mtc_win_handle;		/* to hold the win handle of MT call window 			*/
T_MFW_HND   phlock_dialog_mtc_win_handle;/* to hold the win handle of MT call dialog window 		*/
int 			phlock_alarm;				/* flag to check whether alarm event has occured or not 	*/ 
//x0pleela 09 Mar, 2007  DR: OMAPS00129014
EXTERN UBYTE phlock_auto;				/* flag to check whether auo phone lock is enabled or not */

//x0pleela 15 May, 2007 DR: OMAPS00127483
#ifdef FF_MMI_AUDIO_PROFILE
extern UBYTE mfwAudPlay;//flag for audio 
#endif

void mmi_phlock_change_pin(T_MFW_HND win,int type,char* oldpsw,char* newpsw);
static void mmi_phlock_en_dis_verify(T_MFW_HND win);

//x0pleela 19 Mar, 2007 ER: OMAPS00122561
EXTERN void mmi_set_usbms_enum( int usb_enum);
EXTERN int mmi_get_usbms_enum(void);
EXTERN void mmi_set_Headset_Reg( int headset_reg);
EXTERN int mmi_get_Headset_Reg(void);
static MfwHnd mmi_phlock_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback);
T_MFW_HND phlock_setting_pin_create (T_MFW_HND parent_window);
static int phlock_setting_pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
void phlock_setting_pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void phlock_sett_pin_mess(T_MFW_HND win,USHORT event);
T_MFW_HND phlock_set_pin_mess_create(T_MFW_HND parent_window);
void phlock_set_pin_mess_destroy  (T_MFW_HND own_window);
static int phlock_set_pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
void phlock_set_pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void phlock_set_pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons);
static void phlock_sett_pin_edit(T_MFW_HND win, USHORT event);
T_MFW_HND phlock_set_pin_edt_create (T_MFW_HND parent_window);
static int phlock_set_pin_edt_win_cb (T_MFW_EVENT event,T_MFW_WIN * win);
static void phlock_set_pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int phlock_set_pin_kbd_cb (T_MFW_EVENT event,T_MFW_KBD *  kc);
static int phlock_set_pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc);
static void phlock_pin_edt_tim_out_cb (T_MFW_EVENT event,T_MFW_TIM * t);
void phlock_setting_pin_destroy  (T_MFW_HND own_window);
void phlock_set_pin_edt_destroy  (T_MFW_HND own_window);
T_MFW_SS_RETURN mfw_phlock_enable_lock(U8 *pin, S16 lcktyp);
T_MFW_SS_RETURN mfw_phlock_disable_lock(U8 *pin, S16 lcktyp);

//x0pleela 29 May, 2007 DR: OMAPS00132483
EXTERN int mfw_get_Phlock_status(void);
EXTERN void mfw_set_Phlock_status( int phlock_status);
EXTERN int mfw_get_Auto_Phlock_status(void);
EXTERN void mfw_set_Auto_Phlock_status( int phlock_status);
#endif /* FF_PHONE_LOCK */

/*******************************************************************************

 $Function:	pin_init 

 $Description:	This is the start-up time initialisation routine.
		call from init routine in the idle
 
 $Returns:		None

 $Arguments:	parent window
 
*******************************************************************************/
void pin_init (T_MFW_HND parent_window)
{
	 pin_create (parent_window);
  BootUpState = BOOTUP_STATE_NONE;//xashmic 21 Sep 2006, OMAPS00095831
}


/*******************************************************************************

 $Function:	pin_exit 

 $Description:	power down in security screen
 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/
void pin_exit (void)
{
 if(pin_mess_windows)
	 pin_mess_destroy(pin_mess_windows);

 if (pin_editor_window)
 {
#ifdef NEW_EDITOR
	AUI_pin_Destroy(pin_editor_window);
#else
	editor_destroy(pin_editor_window);
#endif
	pin_editor_window = NULL;
 } 
 
 if (pin_edit_windows)
	 pin_edt_destroy(pin_edit_windows);

 if(pin_windows)
 {
	TRACE_FUNCTION("pin_destroy 1");
	pin_destroy (pin_windows);

 }
}


/*******************************************************************************

 $Function:	pin_create

 $Description:	Creation of an instance for the PIN dialog (sim activation)

		Type of dialog : SINGLE_DYNAMIC
 $Returns:		Window

 $Arguments:	Parent window
 
*******************************************************************************/
T_MFW_HND pin_create (T_MFW_HND parent_window)
{

  T_MFW_WIN	* win;
  T_pin * data = (T_pin *)ALLOC_MEMORY (sizeof (T_pin));

  data->pin_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)pin_win_cb);

  pin_windows = data->pin_win;
  pin_edit_windows = NULL;
  pin_editor_window = NULL;
  pin_mess_windows = NULL;

  TRACE_FUNCTION("MmiPins:pin_create");

  if (data->pin_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)pin_main;/* dialog main function    */
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_win)->data;
  win->user		      = (void *) data;


  /*
   * Create any other handler
   */

  data->sim_handle = sim_create(data->pin_win,E_SIM_ALL_SERVICES,(MfwCb)sim_event_cb_main);

  /*
   * return window handle
   */

	mfwSetSignallingMethod(1);//mfw focus handling
	winShow(data->pin_win);


  return data->pin_win;
}


/*******************************************************************************

 $Function:	pin_destroy

 $Description:	Destroy the pin dialog.
 $Returns:		None

 $Arguments:	window
 
*******************************************************************************/
void pin_destroy  (T_MFW_HND own_window)
{

  T_pin *      data;
  T_MFW_WIN *	win;

  TRACE_FUNCTION("MmiPins:pin_destroy");

  if (own_window)
  {
    win = ((T_MFW_HDR *)own_window)->data;
    data = (T_pin *)win->user;

	 if(data)
	 {
      /*
       * Exit SIM and Delete SIM Handler
       */
      sim_delete (data->sim_handle);

      /*
       * Delete WIN Handler
       */
      win_delete (data->pin_win);
	  pin_windows = 0;
	  FREE_MEMORY((void *)data,(sizeof(T_pin)));
	 }
  }
}


/*******************************************************************************

 $Function:	pin_win_cb

 $Description:	Callback function for windows
 $Returns:		Status int
 $Arguments:	window handler event, window
 
*******************************************************************************/
static int pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  TRACE_EVENT_P1("pin_win_cb: %d", event);

  if (event EQ MfwWinVisible)
  {
    dspl_ClearAll();
	return 1;
  }
  return 0;
}





/*******************************************************************************

 $Function:	sim_event_cb_main

 $Description:	PURPOSE : SIM event handler for Pins
 $Returns:		Status int
 $Arguments:	window handler event, sim status
 
*******************************************************************************/
int sim_event_cb_main (T_MFW_EVENT event, T_MFW_HND para)
{
/**********************/
// #define NO_ACTION	0  /* Warning Correction */

// BYTE reconf_handling = NO_ACTION;  /* Warning Correction */
// BYTE reconf_defreeze_display = 1;  /* Warning Correction */
/*********************/

	T_MFW_SIM_STATUS * status;
	UBYTE limited;

    T_MFW_HND win = mfw_parent(mfw_header());

#ifdef SIM_PERS
	int status1, max1, curr1;
#endif
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;

	T_pin * pin_data = (T_pin *)win_data->user;

	status = (T_MFW_SIM_STATUS *)para;

    limited = TRUE;			/* initial limited mode     */

  TRACE_EVENT_P3("sim event handler,event:%d, status:%d, proc:%d",event, status->sim_status, status->sim_procedure);
// June 16, 2005  REF: CRR 31267  x0021334
// If PIN1 and PUK1 are uninitialised, post SIM_LOCKED event
    if ((sim_pin_count(MFW_SIM_PIN1) EQ 0) AND (sim_pin_count(MFW_SIM_PUK1) EQ 0))
    {	
    	SEND_EVENT(pin_data->pin_win, SIM_LOCKED, 0, pin_data);
    }
    else // June 16, 2005  REF: CRR 31267  x0021334
    {
 	switch(event)
 	{
		case E_SIM_INSERTED:	/*  there not handled */

		BootUpState = BOOTUP_STATE_COMPLETED;//xashmic 21 Sep 2006, OMAPS00095831
		
		/* 14-Mar-2006, Monika.B.J., x0045876, START (Bug OMAPS00071413) */
#ifdef FF_MMI_SAT_ICON
				    addSatMessage(NULL, 0, 0, NULL, FALSE, SAT_ICON_IDLEMODE_TEXT);

#else
					addSatMessage(NULL);
#endif
// July 11, 2006    DR: OMAPS00084081   x0039928
// Fix: Registers to network on sim reset
		 nm_registration (NM_AUTOMATIC, 0, FALSE);
       		/*OMAPS00115179 - support SIM insertion without powering down - a0393213(R.Prabakar)
       		   If pin_editor_window for the removed SIM("Insert SIM" window or "PIN" entry window) is present 
       		   destroy it when a new SIM is inserted. 
       		   If "PIN" entry window is necessary even for the newly inserted SIM, it 
       		   would be created once again at a different point. */
           		if(pin_editor_window)
      			{
          			AUI_pin_Destroy(pin_editor_window);
            			pin_editor_window=NULL;
      			}
         		smsidle_unset_ready_state ();
			
		/* 14-Mar-2006, Monika.B.J., x0045876, END */
					return 0;		/* (handled in sim_rem dialog, commented out currently */


		//GW-SPR#1035-Added STK Changes
		case E_SIM_RESET:	
	    	TRACE_FUNCTION("E_SIM_RESET");
		/*NM, 110702
			the MM-entity initated the de-registration already (MMI dont have to do it); 
			after this event we should avoid any cases to access the SIM
			(e.g. reading phonebook, reading sms....)
			
			- how to do it ???
			->  unset the "sms_initialised" flag to FALSE

			- when does it set back to TRUE ???
			-> the flag will be set after the event "E_SMS_READY"
		*/	
			smsidle_unset_ready_state ();
			info_screen(0, TxtSimNot, TxtReady, NULL); 
		/*NM, 110702 END*/
	    return 0;		
	    

		case E_SIM_STATUS:
	  	/*
	   	* check whether decoded message is available
	   	*/
		if( para EQ (T_MFW_SIM_STATUS *)NULL )
		   return MFW_RES_ILL_HND;

		status = (T_MFW_SIM_STATUS *)para;

 TRACE_FUNCTION_P1("sim_status: %d", status->sim_status);

/********************************/
#ifdef SIM_PERS		

			  //x0pleela 18 Oct, 2006 DR: OMAPS00099660
			  //Added the following code (MFW_MEPD_INVALID)inside the SIM_PERS flag
			  //x0pleela 08 Sep, 2006  DR: OMAPS00091250
			  //Write No MEPD, NO SIM string into a file
			  if(  status->sim_procedure == MFW_SIM_ACTIVATION )
			  {
			  	if(status->sim_status ==  MFW_MEPD_INVALID)
					mfw_flash_write("No MEPD");
				if(status->sim_status == MFW_SIM_NO_SIM_CARD)
					mfw_flash_write("No SIM");
			  }
					

				//x0pleela 29 Aug, 2006 DR: OMAPS00091250
				//Added this check so that MMI doesn't hang at boot up as 
				//MEPD data is not present
				if( ( status->sim_procedure == MFW_SIM_ACTIVATION )AND 
					( ( status->sim_status !=  MFW_MEPD_INVALID) OR
					(status->sim_status != MFW_SIM_NO_SIM_CARD) ) )
				{
				
						status1 =mfw_simlock_check_status(0,&max1, &curr1);
						if(status1== MFW_SIM_PERM_BLOCKED)
						{
							perm_blocked = 1;
						 	status->sim_status=MFW_SIM_PUK_REQ;
						}
				}
				
#endif
			BootUpState = BOOTUP_STATE_INPROGRESS;//xashmic 21 Sep 2006, OMAPS00095831

		 switch (status->sim_procedure)
		 {
		/*
		 * Initial activation of the SIM card
		 */
			case MFW_SIM_ACTIVATION:
		  /*
		   * check the result of activation
		   */
			  switch (status->sim_status)
			  {
				  //x0pleela 18 Oct, 2006 DR: OMAPS00099660
				  //Added the following code (MFW_MEPD_INVALID)inside the SIM_PERS flag
#ifdef SIM_PERS			  
			  //x0pleela 29 aug, 2006  DR: OMAPS00091250
			  	//Send event No MEPD data
			  	case MFW_MEPD_INVALID:
					//limited = TRUE;
					TRACE_FUNCTION("sim_event_cb_main:no MEPD data");
					SEND_EVENT(pin_data->pin_win,NO_MEPD, 0, pin_data);
					break;
#endif					
			/*
			 * PIN 1 must be entered
			 */
					case MFW_SIM_PIN_REQ:
						if(status->sim_pin_retries < PIN_ENTRY_MAX)
						{
							TRACE_FUNCTION("sim_event_cb_main:PIN1 req-retr");
							pin_data->pin_retries = status->sim_pin_retries;/* number of retries */
							SEND_EVENT(pin_data->pin_win,PIN1_REQ_ATT,0,pin_data);
						}
						else
						{

/********************************/

							TRACE_FUNCTION("sim_event_cb_main:PIN1 req");
							SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
						}
						break;

			/*
			 * SIM card is blocked, PUK 1 is needed
			 */
					case MFW_SIM_PUK_REQ:
					{
#ifdef SIM_PERS
						int smlk_status, max, curr; //x0pleela 19 Apr, 2006 DR:OMAPS00067912   changed the name from stastus to smlk_status
#endif
							TRACE_FUNCTION("sim_event_cb_main:PUK1 req");
#ifdef SIM_PERS
						//x0pleela 19 Apr, 2006 DR:OMAPS00067912   
						//changed the name from stastus to smlk_status
						smlk_status =mfw_simlock_check_status(0,&max, &curr);
						if( (smlk_status== MFW_SIM_BLOCKED) OR (smlk_status == MFW_SIM_NOT_BLOCKED) )
						{
							TRACE_EVENT("BMI: MFW_SIM_BLOCKED");
							perm_blocked = 0;
								TRACE_EVENT("BMI: MFW_SIM_PUK1");
							//x0pleela 26 Sep, 2006 DR: OMAPS00096565
							//Adding a check for C_KEY_REQ. If set, display SIMP menu
							//Else prompt the user for PUK entry
							if( C_KEY_REQ )
							{
								//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
								//check for Master Unlock option enabled. 
								//If so, then display the menu with options Unblock ME and Master Unlock
								if( simlock_get_masterkey_status())
						  		{
							  		pin_data->simp_list_win = mmi_simlock_category_menus(win);
							  	}
						  	}
							else
#endif
								SEND_EVENT(pin_data->pin_win,PUK1_REQ_ACT_INFO,0,pin_data);

#ifdef SIM_PERS
						}
						//x0pleela 19 Apr, 2006 DR:OMAPS00067912   
						//changed the name from stastus to smlk_status
						else if (smlk_status == MFW_SIM_PERM_BLOCKED)
						{
							perm_blocked =1;
							SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
						}
#endif
         				 }
					break;

			/*
			 * no PIN is needed
			 */
					case MFW_SIM_NO_PIN:
						
						BootUpState = BOOTUP_STATE_COMPLETED;//xashmic 21 Sep 2006, OMAPS00095831

						limited = FALSE;
						TRACE_FUNCTION("sim_event_cb_main:no Pin");
						//mmi_cphs_refresh();
						SEND_EVENT(pin_data->pin_win,PIN_OK, 0, NULL);//back to idle
						break;

			/*
			 * SIM card is broken
			 */
					case MFW_SIM_INVALID_CARD:
						TRACE_FUNCTION("sim_event_cb_main: invalid card");
						SEND_EVENT(pin_data->pin_win,INVALID_CARD, 0, pin_data);
						break;

			/*
			 * No SIM card is inserted
			 */
					case MFW_SIM_NO_SIM_CARD:
						
						TRACE_FUNCTION("sim_event_cb_main: no SIM Card");
						SEND_EVENT(pin_data->pin_win,NO_SIM_CARD, 0, pin_data);
						break;

// Nov 24, 2005, a0876501, DR: OMAPS00045909
					case MFW_IMEI_NOT_VALID:
						limited = FALSE;
						TRACE_FUNCTION("sim_event_cb_main: IMEI is not valid");
						SEND_EVENT(pin_data->pin_win,INVALID_IMEI, 0, pin_data);
						break;

#ifdef SIM_PERS 

					case MFW_SIM_PLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_PLOCK_REQ");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options SIM unlock and Master Unlock
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_PLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ, 0, pin_data);
						break;
						
					case MFW_SIM_NLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_NLOCK_REQ");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options Network unlock and Master Unlock					  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ, 0, pin_data);
						break;
						
					case MFW_SIM_NSLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_NSLOCK_REQ");
  						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options Network subset unlock and Master Unlock
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NSLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ, 0, pin_data);
						break;
						
					case MFW_SIM_SPLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_SPLOCK_REQ");
  						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options Service Provider unlock and Master Unlock					  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_SPLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ, 0, pin_data);
						break;
						
					case MFW_SIM_CLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_CLOCK_REQ");
  						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options Corporate unlock and Master Unlock					  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_CLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
    						    SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ, 0, pin_data);
						break;

					//x0pleela 25 Sep, 2006 ER: OMAPS00095524
					//Request unlocking password of "Blocked Network" category
					case MFW_SIM_PBLOCK_REQ:
						TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_PBLOCK_REQ");
						//check for Master Unlock option enabled. 
						//If so, then display the menu with options Network unlock and Master Unlock					  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_PBLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ, 0, pin_data);
						break;
#endif
					default:
						return 0;
			  }
		  /*
		   * start limited or full service */

			  if (limited)
			  {
			/*
			 * limited shall be the requested service
			 * if no service is reached, the limited
			 * service is requested*/
			 	TRACE_FUNCTION("DEBUG");

					if (nm_reg_status() EQ NOT_PRESENT_8BIT
						AND
						status->sim_procedure EQ MFW_SIM_ACTIVATION)
						{
							TRACE_FUNCTION("registration in limited mode ");
							nm_registration (NM_AUTOMATIC, 0, TRUE);
						}
			  }
			break;
			case MFW_SIM_VERIFY:
		  /*
		   * check the result of verify
		   */
			  switch (status->sim_status)
			  {
			/*
			 * PIN 1 must be entered
			 */
					case MFW_SIM_PIN_REQ:
					if(status->sim_pin_retries < PIN_ENTRY_MAX)
					{
							TRACE_FUNCTION("sim_event_cb_main:Ver:PIN1 req-retr");
							pin_data->pin_retries = status->sim_pin_retries;
							SEND_EVENT(pin_data->pin_win,PIN1_REQ_ATT,0,pin_data);
					}
					else
					{
							TRACE_FUNCTION("SimEventPins:V:PIN1 req");
							SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
					}
					break;
			/*
			 * SIM card is blocked, PUK 1 is needed
			 */
					case MFW_SIM_PUK_REQ:
						TRACE_FUNCTION("SimEventPins:V:PUK1 req");
						SEND_EVENT(pin_data->pin_win,PUK1_REQ_VER_INFO,0,pin_data);
						break;

			/*
			 * PIN entering successfull
			 */
					case MFW_SIM_NO_PIN:
						TRACE_FUNCTION("sim_event_cb_main:PIN ok without sim unlock");
						//mmi_cphs_refresh();
						pin_data->pin_case = PIN_OK_INFO;
						if (pin_data->set_state == PIN2_CHECK)
						{
							set_mode_fdn_adn(win,pin_data);
						}
						SEND_EVENT(pin_data->pin_win,PIN_OK_INFO,0,pin_data);
						break;
			/*
			 * SIM card is broken
			 */
					case MFW_SIM_INVALID_CARD:
						TRACE_FUNCTION("sim_event_cb_main: invalid card");
						SEND_EVENT(pin_data->pin_win,INVALID_CARD, 0, pin_data);
						break;

			/*
			 * No SIM card is inserted
			 */
					case MFW_SIM_NO_SIM_CARD:
						TRACE_FUNCTION("sim_event_cb_main: no SIM Card");
						SEND_EVENT(pin_data->pin_win,NO_SIM_CARD, 0, pin_data);
						break;
					default:
						return 0;
			  }
			break;
			/*
			 * response to PUK entering
			 */
			case MFW_SIM_UNBLOCK:
			  switch (status->sim_status)
			  {
				case MFW_SIM_PUK_REQ:
					TRACE_FUNCTION("sim_event_cb_main:unblock failure");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FAIL_INFO,0,pin_data);
					break;
				case MFW_SIM_SUCCESS:
					TRACE_FUNCTION("sim_event_cb_main:unblock success");
					//mmi_cphs_refresh();
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_SUCC,0,pin_data);
					break;
				case MFW_SIM_FAILURE:
					TRACE_FUNCTION("sim_event_cb_main:unblock fatal error");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FATAL_ERROR,0,pin_data);
					break;				
				case MFW_SIM_INVALID_CARD:
					TRACE_FUNCTION("sim_event_cb_main:unblock failed, no more retries");
					SEND_EVENT(pin_data->pin_win,INVALID_CARD,0,pin_data);
					break;
				default:
					return 0;
			  }
			break;

			case MFW_SIM_REMOVED:/*  Handle the removal of SIM card */
                            /* OMAPS00050447: a0393130, handle removal of sim card */
                            switch (status->sim_status)
                            {
                                 case MFW_SIM_NO_SIM_CARD: 
                                      TRACE_FUNCTION("sim_event_cb_main: SIM Card removed");
						/*OMAPS00115179 - support SIM insertion without powering down - a0393213
						    SMS state should be unset immediately after removing the SIM card. This ensures
						    icons/display related to messages are NOT shown in the idle screen after the removal of SIM card*/						
					       smsidle_unset_ready_state ();									  
                                       SEND_EVENT(idle_get_window(),NETWORK_NO_SERVICE, 0, 0);
                                       break;
					
                                 default:
                                       return 0;
                            }
			 break;
			      /* OMAPS00050447: a0393130, handle removal of sim card */
			  
			default:
				return 0;
		 }

	return 1;
	#ifdef BMI_TEST_MC_SIM_EVENT
	break;
	case BMI_TEST_MC_SIM_EVENT:
	{
		T_MFW_READ_CALLBACK* sim_read_data;

		sim_read_data = (T_MFW_READ_CALLBACK*)para;

		TRACE_EVENT_P3("SIM READ Error: %d, %x%x", sim_read_data->error_code,
			sim_read_data->read_buffer[0], sim_read_data->read_buffer[1]);
	}
	break;
	#endif
  	}
    } // June 16, 2005  REF: CRR 31267  x0021334
    
    /* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function ) */
    return 0;
}
#ifdef SIM_PERS
/*******************************************************************************

 $Function:    	simp_busy_cb

 $Description:   This function checks for the ACI CME error and display menu for unlocking
 			  categories or unblocking ME

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
void simp_busy_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons)
{
  T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;

  T_pin * pin_data = (T_pin *)win_data->user;//pin main data
  
    //x0pleela 25 Sep, 2006 ER: OMAPS00095524	
    //check for CME class and handle all CME errors
  if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Cme) 
  {
  switch(aciErrDesc & 0x0000FFFF)
  {
    case CME_ERR_NetworkPersPinReq:
      TRACE_EVENT("CME_ERR_NetworkPersPinReq");
      if( simlock_get_masterkey_status())
      {
        mmi_simlock_locktype = MFW_SIM_NLOCK;
        pin_data->simp_list_win = mmi_simlock_category_menus(win);
      }
      else
        SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ,0,pin_data);
  break;
  
    case CME_ERR_NetworkSubsetPersPinReq:
      TRACE_EVENT("CME_ERR_NetworkSubsetPersPinReq");
      if( simlock_get_masterkey_status())
      {
        mmi_simlock_locktype = MFW_SIM_NSLOCK;
        pin_data->simp_list_win = mmi_simlock_category_menus(win);
      }
      else
        SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ,0,pin_data);
    break;
    
    case CME_ERR_ProviderPersPinReq:
      TRACE_EVENT("CME_ERR_ProviderPersPinReq");
      if( simlock_get_masterkey_status())
      {
        mmi_simlock_locktype = MFW_SIM_SPLOCK;
        pin_data->simp_list_win = mmi_simlock_category_menus(win);
      }
      else
        SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ,0,pin_data);
    break;
    
    case CME_ERR_CorporatePersPinReq:
      TRACE_EVENT("CME_ERR_CorporatePersPinReq");
      if( simlock_get_masterkey_status())
      {
        mmi_simlock_locktype = MFW_SIM_CLOCK;
        pin_data->simp_list_win = mmi_simlock_category_menus(win);
      }
      else
        SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ,0,pin_data);
    break;
    
    case CME_ERR_PhSimPinReq:
      TRACE_EVENT("CME_ERR_PhSimPinReq");
      if( simlock_get_masterkey_status())
      {
        mmi_simlock_locktype = MFW_SIM_PLOCK;
        pin_data->simp_list_win = mmi_simlock_category_menus(win);
      }
      else
        SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ,0,pin_data);
    break;
    
    case CME_ERR_PhoneFail:
    case CME_ERR_NetworkPersPukReq:
    case CME_ERR_NetworkSubsetPersPukReq:
    case CME_ERR_ProviderPersPukReq:
    case CME_ERR_CorporatePersPukReq:
    {
      int status,curr_fail_reset, curr_succ_reset;
      TRACE_EVENT("PUK Req");
      status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
      
		if( status == MFW_SIM_BLOCKED)
      {
        C_KEY_REQ = 1;
        if( simlock_get_masterkey_status())
        {
          pin_data->simp_list_win = mmi_simlock_category_menus(win);
        }
        else
          SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
      }
      else if (status == MFW_SIM_PERM_BLOCKED)
      {
        perm_blocked =1;
        SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
      }	
    }
    break;	
    
    case CME_ERR_WrongPasswd:
      TRACE_EVENT("CME_ERR_WrongPasswd");
      SEND_EVENT(pin_data->pin_win,identifier,0,pin_data);
    break;
    
    default:
      TRACE_EVENT("Undefined error");
  }
}

    //x0pleela 25 Sep, 2006 ER: OMAPS00095524	
    //check for Ext class and handle all Extension errors
	else if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) 
	{
		switch(aciErrDesc & 0x0000FFFF)
		{
			//x0pleela 25 Sep, 2006 ER: OMAPS00095524
			//Request for Blocked Network Password 
			case EXT_ERR_BlockedNetworkPersPinReq:
				TRACE_EVENT("CME_ERR_NetworkPersPinReq");
				if( simlock_get_masterkey_status())
				{
					mmi_simlock_locktype = MFW_SIM_PBLOCK;
					pin_data->simp_list_win = mmi_simlock_category_menus(win);
				}
				else
					SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ,0,pin_data);
			break;

			//x0pleela 25 Sep, 2006 ER: OMAPS00095524
			//Request for Blocked Network Unblock code 
			case EXT_ERR_BlockedNetworkPersPukReq:
			{
				int status,curr_fail_reset, curr_succ_reset;
				TRACE_EVENT("PUK Req");

				status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

				if( status==MFW_SIM_BLOCKED)
				{
					C_KEY_REQ = 1;
					if( simlock_get_masterkey_status())
					{
						pin_data->simp_list_win = mmi_simlock_category_menus(win);
					}
					else
						SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
				}
				else if (status == MFW_SIM_PERM_BLOCKED)
				{
					perm_blocked =1;
					SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
				}	
			}
			break;	

			default:
				break;
		}
	}
  
}

#endif

/*******************************************************************************

 $Function:	pin_main

 $Description:	PIN MAIN Dialog Handling function
 $Returns:		void
 $Arguments:	window, window handler event, value, parameters
 
*******************************************************************************/
static void pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;//pin main data
// 	T_pin_mess * mess_data = (T_pin_mess *) parameter;//pin message data  /* Warning Correction */
//	T_pin_edt * edt_data = (T_pin_edt *) parameter;//pin editor data  /* Warning Correction */

	// June 16, 2005  REF: CRR 31267  x0021334
	// Added the following two variables
	static int pin1_info_count = 0; 
	T_DISPLAY_DATA display_info; 
	
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA	editor_data;
#endif

	TRACE_EVENT_P1("MmiPins:pin_main: %d", event);

	 switch(event)
	 {
#ifdef SIM_PERS
		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//This event displays a dialog "Busy" for the user if timer is enabled and 
		//ACI returns Busy error when user had entered wrong unlocking password
		case SIM_LOCK_BUSY_BOOTUP:
			mfw_simlock_set_busy_state(FALSE);
			dlg_initDisplayData_TextId(&display_info,  NULL, NULL, TxtBusy, TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)simp_busy_cb, THREE_SECS, KEY_LEFT | KEY_RIGHT);
			info_dialog (pin_data->pin_win, &display_info);
			break;
#endif
	 
		// June 16, 2005  REF: CRR 31267  x0021334
		// This case is added to handle PIN1 and PUK1 EQ 0 condition
	 	 case SIM_LOCKED:
		 if(pin1_info_count EQ 0) // To avoid getting the sim blocked dialog during emergency call set up
		 {
			pin1Flag = TRUE; // set this flag to TRUE to indicate that PIN1 and PUK1 are 0
			// Display info dialog that the sim is blocked for 3 seconds, after which call back function is called.
			dlg_initDisplayData_TextId(&display_info,  NULL, NULL, TxtSimBlocked, TxtDealer, COLOUR_STATUS);
			dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)pin1_cb_function, THREE_SECS, KEY_LEFT | KEY_RIGHT);
			info_dialog (pin_data->pin_win, &display_info);
			pin1_info_count++;
		 }
		 else // post SIM_LOCKED_EMR_EDITOR event to create the editor for entering emergency numbers
		 {
			SEND_EVENT(pin_data->pin_win, SIM_LOCKED_EMR_EDITOR, 0, 0);
		 }
		 break;

		 // June 16, 2005  REF: CRR 31267  x0021334
		// This case is added to handle response from call back function 'pin1_cb_function'
		 case SIM_LOCKED_EMR_EDITOR:
		 memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));
		 pins_loadEditDefault (&editor_data, TxtSimBlocked, SIM_LOCKED_EMR_EDITOR);
		 AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_DIG);
		 pin_editor_window = AUI_pin_Start(win, &editor_data); // start the editor
		 break;

		  case PIN1_STATUS:		    //Text "Pin disabled" or "enabled"
		  case PIN1_REQ_ATT:		    //info screen "xx attempts "
		  case PUK1_REQ_ACT_INFO:	    //info screen "SIM blocked "
		  
		  case GSM_FAIL:		    // gsm not allowed
		  case NEW_PIN_FAIL:		    // new pin failed
		  case PUK1_REQ_VER_INFO:	    // Text "Pin blocked "
		  case PUK1_UNBL_FAIL_INFO:	    // unblock error - new puk requ
		  case PUK1_UNBL_FATAL_ERROR:
		  
		 
		  case SIM_UNLOCK_OK:		    //Text "Unlock ok "
		  case SIM_LOCK_ERR:
#ifdef SIM_PERS
        case SIM_LOCK_PERS_CHK_OK:
		    case SMLK_SHOW_FC_FAIL:
		    case SMLK_SHOW_FC_SUCC:			
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		    case SMLK_SHOW_MK_FAIL:		//wrong master unlock key
		    case SMLK_SHOW_MK_SUCC:		//correct master unlock key
		    case SIM_LOCK_BUSY_UNBLOCK: //wrong unblock code
#endif
				   main_call_mess(win,event);
				   break; 

			case PUK1_REQ:			  //Text "Enter Puk "
#ifdef SIM_PERS
    case INPUT_SIM_PIN_REQ:
		case INPUT_NLOCK_PIN_REQ:
		case INPUT_NSLOCK_PIN_REQ:
		case INPUT_SPLOCK_PIN_REQ:
		case INPUT_CLOCK_PIN_REQ:
		case PERM_BLK:
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		case INPUT_MASTER_KEY_REQ: 		//Master Unlock Key request
		case INPUT_PBLOCK_PIN_REQ:		//x0pleela 25 Sep, 2006 ER: OMAPS00095524
#endif
				//clear the editor-buffer
				memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#ifdef SIM_PERS
		switch(event)
				{
				case PERM_BLK:
					pins_loadEditDefault (&editor_data, TxtPhoneBlocked, PERM_BLK);
					break;

				case PUK1_REQ:	
				if( C_KEY_REQ )
					pins_loadEditDefault (&editor_data, TxtUnblockCode, PUK1_REQ);
				else					
					pins_loadEditDefault (&editor_data, TxtEnterPuk1, PUK1_REQ);
					break;
				
				case INPUT_NLOCK_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterNLock, INPUT_NLOCK_PIN_REQ);
					break;
					
				case INPUT_NSLOCK_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterNsLock, INPUT_NSLOCK_PIN_REQ);
					break;
					
				case INPUT_SPLOCK_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterSpLock, INPUT_SPLOCK_PIN_REQ);
					break;
					
				case INPUT_CLOCK_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterCLock, INPUT_CLOCK_PIN_REQ);
					break;

				case INPUT_SIM_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterPsLock, INPUT_SIM_PIN_REQ);
					break;

				//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
				case INPUT_MASTER_KEY_REQ:
					pins_loadEditDefault (&editor_data, TxtMasterUnlock, INPUT_MASTER_KEY_REQ);
					break;					
					
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524	
				case INPUT_PBLOCK_PIN_REQ:
					pins_loadEditDefault (&editor_data, TxtEnterPbLock, INPUT_PBLOCK_PIN_REQ);
					break;					
					
				}
				TRACE_FUNCTION_P1("sim_status: %d", gsim_status);
#else
			        pins_loadEditDefault (&editor_data, TxtEnterPuk1, PUK1_REQ);
#endif
				//x0pleela 05 Jul, 2006  DR: OMAPS00084642
				//Changed the size of simlock password enty from MAX_DIG to MAX_PIN+1
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_PIN+1);
				pin_editor_window = AUI_pin_Start(win, &editor_data);
#else /*NEW_EDITOR */
				pins_loadEditDefault (&editor_data);

				editor_data.editor_attr.text	= pin_data->edtbuf;  /* buffer to be edited */
				editor_data.editor_attr.size	= MAX_PIN + 1;	 /* limit to 8 digits */
				editor_data.LeftSoftKey 		= TxtSoftOK;	    
				editor_data.RightSoftKey	= TxtDelete;
				editor_data.TextId			= TxtEnterPuk1;
				editor_data.Identifier		= PUK1_REQ;  
				editor_data.min_enter		= 4;
				editor_data.hide			= TRUE;
#ifdef SIM_PERS
		switch(event)
				{
				case PERM_BLK:
					editor_data.TextId	= TxtPhoneBlocked;
					editor_data.Identifier			= PERM_BLK;  
					break;

				case PUK1_REQ:
				if( C_KEY_REQ )
					editor_data.TextId	= TxtUnblockCode;
				else
					editor_data.TextId	= TxtEnterPuk1;
					editor_data.Identifier = PUK1_REQ;  
					break;

				case INPUT_NLOCK_PIN_REQ:
					editor_data.TextId	=TxtEnterNLock;
					editor_data.Identifier			= INPUT_NLOCK_PIN_REQ;  
					break;
					
				case INPUT_NSLOCK_PIN_REQ:
					editor_data.TextId	=TxtEnterNsLock;
					editor_data.Identifier			= INPUT_NSLOCK_PIN_REQ;  
					break;
					
				case INPUT_SPLOCK_PIN_REQ:
					editor_data.TextId	=TxtEnterSpLock;
					editor_data.Identifier			= INPUT_SPLOCK_PIN_REQ;  
					break;
					
				case INPUT_CLOCK_PIN_REQ:
					editor_data.TextId	=TxtEnterCLock;
					editor_data.Identifier			= INPUT_CLOCK_PIN_REQ;  
					break;

				case INPUT_SIM_PIN_REQ:
					editor_data.TextId	=TxtEnterPsLock;
					editor_data.Identifier			= INPUT_SIM_PIN_REQ;  
					break;

				//x0pleela 20 july, 2006 ER: OMAPS00087586, OMAPS00087587
				case INPUT_MASTER_KEY_REQ:
					editor_data.TextId	=TxtMasterUnlock;
					editor_data.Identifier			= INPUT_MASTER_KEY_REQ;  
					break;

				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				case INPUT_PBLOCK_PIN_REQ:
					editor_data.TextId	=TxtEnterPbLock;
					editor_data.Identifier			= INPUT_PBLOCK_PIN_REQ;  
					break;					
				}
#endif		
				/* create the dialog handler */
				pin_editor_window = editor_start(win, &editor_data);  /* start the editor */
#endif	/*NEW_EDITOR */
			break;	

			case INPUT_NEW_PIN:
				//clear the editor-buffer
				memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				pins_loadEditDefault (&editor_data, TxtEnterNewPin, INPUT_NEW_PIN);
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_PIN+1);
				pin_editor_window = AUI_pin_Start(win, &editor_data);
#else /*NEW_EDITOR */
				pins_loadEditDefault (&editor_data);

				editor_data.editor_attr.text	= pin_data->edtbuf;  /* buffer to be edited */
				editor_data.editor_attr.size	= MAX_PIN + 1;	 /* limit to 8 digits */
				editor_data.LeftSoftKey 		= TxtSoftOK;	    
				editor_data.RightSoftKey		= TxtDelete;
				editor_data.TextId				= TxtEnterNewPin;
				editor_data.Identifier			= INPUT_NEW_PIN;  
				editor_data.min_enter		    = 4;
				editor_data.hide			    = TRUE;

				/* create the dialog handler */
				pin_editor_window = editor_start(win, &editor_data);  /* start the editor */ 
#endif /*NEW_EDITOR */
			break;			

			case INPUT_NEW_PIN_AGAIN:
				//clear the editor-buffer
				memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				pins_loadEditDefault (&editor_data, TxtConfPin, INPUT_NEW_PIN_AGAIN);

				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_PIN+1);
				pin_editor_window = AUI_pin_Start(win, &editor_data);
#else /*NEW_EDITOR */
				pins_loadEditDefault (&editor_data);
				
				editor_data.editor_attr.text	= pin_data->edtbuf;  /* buffer to be edited */
				editor_data.editor_attr.size	= MAX_PIN + 1;	 /* limit to 8 digits */
				editor_data.LeftSoftKey 		= TxtSoftOK;	    
				editor_data.RightSoftKey		= TxtDelete;
				editor_data.TextId				= TxtConfPin;
				editor_data.Identifier			= INPUT_NEW_PIN_AGAIN;	
				editor_data.min_enter		    = 4;
				editor_data.hide			    = TRUE;

				/* create the dialog handler */
				pin_editor_window = editor_start(win, &editor_data);  /* start the editor */ 
#endif /* NEW_EDITOR */
				break;
				
		  case PIN1_REQ:		    //Text "Enter Pin "
				TRACE_FUNCTION("PIN1_REQ");
				//clear the editor-buffer
				memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				pins_loadEditDefault (&editor_data, TxtEnterPin1, PIN1_REQ);

				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_DIG);
				pin_editor_window = AUI_pin_Start(win, &editor_data);
#else /*NEW_EDITOR */
				pins_loadEditDefault (&editor_data);
				
				editor_data.editor_attr.text	= pin_data->edtbuf;  /* buffer to be edited */
				editor_data.editor_attr.size	= MAX_PIN + 1;	 /* limit to 8 digits */
				editor_data.LeftSoftKey 		= TxtSoftOK;	    
				editor_data.RightSoftKey		= TxtDelete;
				editor_data.TextId				= TxtEnterPin1;
				editor_data.Identifier			= PIN1_REQ;  
				editor_data.min_enter		    = 4;
				editor_data.hide			    = TRUE;

				/* create the dialog handler */ 
				pin_editor_window = editor_start(win, &editor_data);  /* start the editor */ 
#endif /* NEW_EDITOR */
			break;

		  case NO_SIM_CARD:		    //Text "No Sim Card "
		  case INVALID_CARD:		    //Text "Invalid SIM card "
// Nov 24, 2005, a0876501, DR: OMAPS00045909
		  case INVALID_IMEI:		    //Text "Invalid IMEI "
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS		  
  		  case NO_MEPD: //x0pleela 30 Aug, 2006 DR: OMAPS00091250
#endif

		  		TRACE_EVENT("NO_SIM OR INVALID_SIM OR INVALID IMEI");

// May 15, 2006 DR: OMAPS00077825 - x0039928
// Fix : TTY is initialized.
#ifdef MMI_TTY_ENABLED
				call_tty_init();
#endif

				/* API - Created the one call to generate the editor as there were two duplicate calls */
				//clear the editor-buffer
				memset(pin_data->edtbuf,'\0',sizeof(pin_data->edtbuf));
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				pins_loadEditDefault (&editor_data, TxtNull, 0);
				
				if(event == NO_SIM_CARD)
				{
					AUI_edit_SetTextStr(&editor_data, TxtNull, TxtDelete, TxtNoCard, NULL);
					AUI_edit_SetEvents(&editor_data, INSERT_CARD, FALSE, FOREVER, (T_AUI_EDIT_CB)pins_editor_cb);
				}
				else if (event == INVALID_IMEI) // Nov 24, 2005, a0876501, DR: OMAPS00045909
				{
					AUI_edit_SetTextStr(&editor_data, TxtNull, TxtDelete, TxtInvalidIMEI, NULL);
					AUI_edit_SetEvents(&editor_data, SHOW_IMEI_INVALID, FALSE, FOREVER, (T_AUI_EDIT_CB)pins_editor_cb);

				}
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS					
				//x0pleela 30 Aug, 2006 DR: OMAPS00091250
				else if ( event == NO_MEPD )
				{
					AUI_edit_SetTextStr(&editor_data, TxtNull, TxtDelete, TxtNoMEPD, NULL);
					AUI_edit_SetEvents(&editor_data, NO_MEPD_DATA, FALSE, FOREVER, (T_AUI_EDIT_CB)pins_editor_cb);

				}
#endif				
				else
				{
					AUI_edit_SetTextStr(&editor_data, TxtNull, TxtDelete, TxtInvalidCard, NULL);
					AUI_edit_SetEvents(&editor_data, CARD_REJECTED, FALSE, FOREVER, (T_AUI_EDIT_CB)pins_editor_cb);

				}
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)pin_data->edtbuf, MAX_DIG);
				AUI_edit_SetAltTextStr(&editor_data, 3, TxtNull, TRUE, TxtNull);

				/*SPR#2235 - DS - Overwrite default 'hidden' PIN entry mode */
				AUI_edit_SetMode(&editor_data, 0 /*Numeric Mode */, ED_CURSOR_UNDERLINE);
				
				pin_editor_window = AUI_pin_Start(win, &editor_data);
#else /*NEW_EDITOR */
				pins_loadEditDefault (&editor_data);

				if(event == NO_SIM_CARD)
				{
					editor_data.TextId = TxtNoCard;
					editor_data.Identifier = INSERT_CARD;	
				}
				else if(event == INVALID_IMEI) // Nov 24, 2005, a0876501, DR: OMAPS00045909
				{
					editor_data.TextId = TxtInvalidIMEI;
					editor_data.Identifier = SHOW_IMEI_INVALID;	
				}
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS					
				//x0pleela 30 Aug, 2006 DR: OMAPS00091250
				else if (event == NO_MEPD )
				{
					editor_data.TextId = TxtNoMEPD;
					editor_data.Identifier = NO_MEPD_DATA;	
				}
#endif				
				else
				{
					editor_data.TextId = TxtInvalidCard;
					editor_data.Identifier = CARD_REJECTED;	
				}

				editor_data.editor_attr.text	= pin_data->edtbuf;  /* buffer to be edited */
				editor_data.editor_attr.size	= MAX_DIG;   /* Don't limit to 3 digits (as that prevents "*#06#") */
				editor_data.LeftSoftKey 		= TxtNull;	    
				editor_data.RightSoftKey		= TxtDelete;
				editor_data.min_enter		    = 3;
				editor_data.destroyEditor	    = FALSE;

				/* create the dialog handler */
				pin_editor_window = editor_start(win, &editor_data);  /* start the editor */
#endif /* NEW_EDITOR */

		  break;
		 
		  case TXT_SIM_PLOCK:
				   pin_data->pin_case = SIM_PLOCK;
				   main_call_edit(win,SIM_PLOCK);		      // Start: enter P unlock
				   break;
		  case TXT_SIM_SPLOCK:
				   pin_data->pin_case = SIM_SPLOCK;
				   main_call_edit(win,SIM_SPLOCK);		      // Start: enter SP unlock
				   break;
		  case TXT_SIM_NLOCK:
				   pin_data->pin_case = SIM_NLOCK;
				   main_call_edit(win,SIM_NLOCK);		      // Start: enter N unlock
				   break;
		  case TXT_SIM_CLOCK:
				   pin_data->pin_case = SIM_CLOCK;
				   main_call_edit(win,SIM_CLOCK);		      // Start: enter C unlock
				   break;
		  case TXT_SIM_NSLOCK:
				   pin_data->pin_case = SIM_NSLOCK;
				   main_call_edit(win,SIM_NSLOCK);		      // Start: enter NS unlock
				   break;



	 }

	switch(event)
	{
	  case PIN_OK_INFO: // PIN was needed
	  case PIN_OK:	    // PIN wasn't needed
				check_plock_to_clock(win,event);/* start sim lock procedure   */
			   break;
	  case PIN_OK_END:
		{
			BootUpState = BOOTUP_STATE_COMPLETED;//xashmic 21 Sep 2006, OMAPS00095831

//		   U8 uMode;  /* Warning Correction */
			/*
			*** registration in full service mode case: PIN was  necessary
			*/
				/* SH - show welcome screen */
			//	showwelcome(idle_get_window());

			    network_start_full_service ();
//GW-SPR#1035-Added STK Changes
				TRACE_FUNCTION("pin_destroy 2");
	  }			
		       break;
//nm insert new one
	  case PIN1_STATUS_END:
			   pin_destroy(pin_data->pin_win);// finish main pin dialog
			   TRACE_FUNCTION("pin_destroy 3");
		       break;
//nm		       
	  case TXT_SIM_LOCK_ALL:
	  		//ShowMessage(idle_get_window(), pin_data->display_id1, pin_data->display_id2);
			   pin_destroy(pin_data->pin_win);// finish main pin dialog
			   TRACE_FUNCTION("pin_destroy 4");
		       break;
	  case PUK1_UNBL_SUCC:
			if((pin_data->pin_case EQ PUK1_END) || (pin_data->gsm_state && (pin_data->pin_case EQ INPUT_PUK1)))
						/*	 sim lock check only after power on and puk verified */
				   /* by PUK (normal way) or by gsm unblock string */
			{
				check_plock_to_clock(win,event);
			}
			else
			{

				main_call_mess(win,event);		   
	 
			}
			break;
	}

	switch(event)
	{


	  case SIM_NLOCK_REQ:
	  case SIM_NSLOCK_REQ:
	  case SIM_SPLOCK_REQ:
	  case SIM_CLOCK_REQ:
			main_call_mess(win,event);
			break;
	  case SIM_PLOCK:
			check_nlock_to_clock(win,event);
			break;
	  case SIM_NLOCK:
			check_splock_to_clock(win,event);
			break;
	  case SIM_SPLOCK:
			check_nslock_to_clock(win,event);
			break;
	  case SIM_NSLOCK:
			check_sim_clock(win,event);
			break;
	  case SIM_CLOCK:
			if(sim_unlock_sim_lock(MFW_SIM_CLOCK,(UBYTE *)pin_data->pin)EQ MFW_SIM_CLOCK)/* unlock code  */
			{
					main_call_mess(win,SIM_UNLOCK_OK);	      /* finish sim unlock procedure */
			}
			else if(sim_unlock_sim_lock(MFW_SIM_CLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_UNLOCK_ERR)
			{
				main_call_mess(win,SIM_LOCK_ERR);
			}
			break;
	  default:
		  break;
	}
}


/*******************************************************************************

 $Function:	check_plock_to_clock 

 $Description:	sim lock check from PLOCK to CLOCK
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void check_plock_to_clock(T_MFW_HND win,USHORT event)
{
	/* x0045876, 14-Aug-2006 (WR - "win_data" was declared but never referenced) */
	/* T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data; */
	
//	T_pin * pin_data = (T_pin *)win_data->user; // pin main data  /* Warning Correction */

	TRACE_FUNCTION("MmiPins:check_plock_to_clock");
	/*MC SPR 1351 check all locks other than Plock for being blocked as well as locked.  User can
	then attempt to enter overall PUK to disable lock if need be*/
	if(sim_check_sim_Plock() EQ MFW_SIM_BLOCKED)
	{
		main_call_mess(win,SIM_LOCK_ALL);
	}
	else if(sim_check_sim_Plock() EQ MFW_SIM_LOCKED)
	{
		main_call_mess(win,SIM_PLOCK_REQ);
	}
	else if(sim_check_sim_Nlock() EQ MFW_SIM_LOCKED ||sim_check_sim_Nlock() EQ MFW_SIM_BLOCKED)
	{
		main_call_mess(win,SIM_NLOCK_REQ);
	}
	else if(sim_check_sim_SPlock() EQ MFW_SIM_LOCKED||sim_check_sim_SPlock() EQ MFW_SIM_BLOCKED)
	{
		main_call_mess(win,SIM_SPLOCK_REQ);
	}
	else if(sim_check_sim_NSlock() EQ MFW_SIM_LOCKED ||sim_check_sim_NSlock() EQ MFW_SIM_BLOCKED)
	{
		main_call_mess(win,SIM_NSLOCK_REQ);
	}
	else if(sim_check_sim_Clock() EQ MFW_SIM_LOCKED|| sim_check_sim_Clock() EQ MFW_SIM_BLOCKED)
	{
		main_call_mess(win,SIM_CLOCK_REQ);
	}
	else
	{
		switch(event)
		{
			case PIN_OK:

				/*
				*** registration in full service mode case: PIN was not necessary
				*/
				/* SH - show welcome screen*/
				//showwelcome(idle_get_window());
				/*SPR 1431*/
				network_start_full_service();
//GW-SPR#1035-Added STK Changes
/* NM, 110702
   this destroy the only ONE SIM-handler which we have 
   after switching on !!
   It should be at least one SIM-handler activ all the  time 
*/
				TRACE_FUNCTION("pin_destroy 5");
				break;
			case PIN_OK_INFO:
				main_call_mess(win,PIN_OK_INFO);
				break;
			case PUK1_UNBL_SUCC:
				main_call_mess(win,event);
				break;
			default:
				break;
		}
	}
}


/*******************************************************************************

 $Function:	check_nlock_to_clock
 $Description:	sim lock check from NLOCK to CLOCK
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void check_nlock_to_clock(T_MFW_HND win,USHORT event)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins:check_nlock_to_clock");

	if(sim_unlock_sim_lock(MFW_SIM_PLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_PLOCK)/* P unlock code ok */
	{	/*MC SPR 1351 check locks for being blocked as well as locked.  User can
		then attempt to enter overall PUK to disable lock*/
		if(sim_check_sim_Nlock() EQ MFW_SIM_LOCKED || sim_check_sim_Nlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_NLOCK_REQ);	      //check N unlock code
		}
		else if(sim_check_sim_SPlock() EQ MFW_SIM_LOCKED || sim_check_sim_SPlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_SPLOCK_REQ);	      //check SP unlock code
		}
		else if(sim_check_sim_NSlock() EQ MFW_SIM_LOCKED || sim_check_sim_NSlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_NSLOCK_REQ);	      //check NS unlock code
		}
	else if(sim_check_sim_Clock() EQ MFW_SIM_LOCKED || sim_check_sim_Clock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_CLOCK_REQ);	      //check C unlock code
		}
		else					      /* finish sim unlock procedure */
		{
			main_call_mess(win,SIM_UNLOCK_OK);
		}
	}
	else if(sim_unlock_sim_lock(MFW_SIM_PLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_UNLOCK_ERR)
	{
			main_call_mess(win,SIM_LOCK_ERR);
	}
}


/*******************************************************************************

 $Function:	check_splock_to_clock
 $Description:	sim lock check from SPLOCK to CLOCK
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void check_splock_to_clock(T_MFW_HND win,USHORT event)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins:check_splock_to_clock");

	if(sim_unlock_sim_lock(MFW_SIM_NLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_NLOCK)
	{/*MC SPR 1351 check locks for being blocked as well as locked.  User can
		then attempt to enter overall PUK to disable lock*/
		if(sim_check_sim_SPlock() EQ MFW_SIM_LOCKED || sim_check_sim_SPlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_SPLOCK_REQ);	      //check SP unlock code
		}
		else if(sim_check_sim_NSlock() EQ MFW_SIM_LOCKED|| sim_check_sim_NSlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_NSLOCK_REQ);	      //check NS unlock code
		}
	else if(sim_check_sim_Clock() EQ MFW_SIM_LOCKED || sim_check_sim_Clock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_CLOCK_REQ);	      //check C unlock code
		}
		else					      /* finish sim unlock procedure */
		{
			main_call_mess(win,SIM_UNLOCK_OK);	      /* finish sim unlock procedure */
		}
	}
	else if(sim_unlock_sim_lock(MFW_SIM_NLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_UNLOCK_ERR)
	{
		main_call_mess(win,SIM_LOCK_ERR);
	}
}


/*******************************************************************************

 $Function:	check_nslock_to_clock
 $Description:	sim lock check from NSLOCK to CLOCK
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void check_nslock_to_clock(T_MFW_HND win,USHORT event)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins:check_nslock_to_clock");

	if(sim_unlock_sim_lock(MFW_SIM_SPLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_SPLOCK)/* unlock code ok */
	{/*MC SPR 1351 check locks for being blocked as well as locked.  User can
		then attempt to enter overall PUK to disable lock*/
		if(sim_check_sim_NSlock() EQ MFW_SIM_LOCKED || sim_check_sim_NSlock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_NSLOCK_REQ);	      //check NS unlock code
		}
	else if(sim_check_sim_Clock() EQ MFW_SIM_LOCKED || sim_check_sim_Clock() EQ MFW_SIM_BLOCKED)
		{
			main_call_mess(win,SIM_CLOCK_REQ);	      //check C unlock code
		}
		else					      /* finish sim unlock procedure */
		{
			main_call_mess(win,SIM_UNLOCK_OK);	      /* finish sim unlock procedure */
		}
	}
	else if(sim_unlock_sim_lock(MFW_SIM_SPLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_UNLOCK_ERR)
	{
		main_call_mess(win,SIM_LOCK_ERR);
	}
}


/*******************************************************************************

 $Function:	check_sim_clock
 $Description:	sim lock check	CLOCK
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void check_sim_clock(T_MFW_HND win,USHORT event)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins:check_sim_lock");

	if(sim_unlock_sim_lock(MFW_SIM_NSLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_NSLOCK)/* unlock code ok */
	{/*MC SPR 1351 check locks for being blocked as well as locked.  User can
		then attempt to enter overall PUK to disable lock*/
      if(sim_check_sim_Clock() EQ MFW_SIM_LOCKED || sim_check_sim_Clock() EQ MFW_SIM_BLOCKED)
	  {
		main_call_mess(win,SIM_CLOCK_REQ);		  //check C unlock code
	   }
		else					      /* finish sim unlock procedure */
		{
			main_call_mess(win,SIM_UNLOCK_OK);	      /* finish sim unlock procedure */
		}
	}
	else if(sim_unlock_sim_lock(MFW_SIM_NSLOCK,(UBYTE *)pin_data->pin) EQ MFW_SIM_UNLOCK_ERR)
	{
		main_call_mess(win,SIM_LOCK_ERR);
	}
}

/*******************************************************************************

 $Function:	main_call_mess 
 $Description:	help function for message dialog
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void main_call_mess(T_MFW_HND win, USHORT event)
{

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_mess_win;
	TRACE_FUNCTION("MmiPins: main_call_mess");

	pin_mess_win = pin_mess_create(pin_data->pin_win);
	
	if(pin_mess_win)
	{
		SEND_EVENT(pin_mess_win,event,0,pin_data);
	}
}


/*******************************************************************************

 $Function:	main_call_edit 
 $Description:	help function for editor dialog
 $Returns:		void
 $Arguments:	window, window handler event
 
*******************************************************************************/
static void main_call_edit(T_MFW_HND win,USHORT event)
{

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_edt_win;




	pin_edt_win = pin_edt_create(pin_data->pin_win);

    if(pin_edt_win)
	{
		SEND_EVENT(pin_edt_win,event,0,pin_data);
	}
}


/*******************************************************************************

 $Function:	pin_edt_create 
 $Description:	Creation of an instance  for the PIN Editor dialog.
		Type of dialog : SINGLE_DYNAMIC
 $Returns:		void
 $Arguments:	parent window
 
*******************************************************************************/
T_MFW_HND pin_edt_create (T_MFW_HND parent_window)
{
  T_pin_edt * data = (T_pin_edt *)ALLOC_MEMORY (sizeof (T_pin_edt));

  T_MFW_WIN * win;

  data->pin_edt_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)pin_edt_win_cb);
  TRACE_FUNCTION("pin_edt_create");

  if (data->pin_edt_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  pin_edit_windows = data->pin_edt_win;
  data->mmi_control.dialog    = (T_DIALOG_FUNC)pin_editor;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_edt_win)->data;
  win->user		      = (void *) data;

  /*
   * Create any other handler
   */

  data->tim_out_handle = tim_create(data->pin_edt_win,TIMEOUT,(T_MFW_CB)pin_edt_tim_out_cb);

  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
  data->editor = ATB_edit_Create(&data->editorPinAttr,0);
#else /* NEW_EDITOR */
  data->editor_handle = edt_create(data->pin_edt_win,&data->editpinAttr,0,0);
#endif /* NEW_EDITOR */

  data->kbd_handle = kbd_create(data->pin_edt_win,KEY_ALL,(T_MFW_CB)pin_edt_kbd_cb);
  data->kbd_long_handle = kbd_create(data->pin_edt_win,KEY_ALL | KEY_LONG,(T_MFW_CB)pin_edt_kbd_long_cb);
  /*
   * return window handle
   */
  return data->pin_edt_win;
}


/*******************************************************************************

 $Function:	pin_edt_destroy 
 $Description:	Destroy the pin editor dialog.
 $Returns:		void
 $Arguments:	window
 
*******************************************************************************/
void pin_edt_destroy  (T_MFW_HND own_window)
{
  T_pin_edt * data ;
  T_MFW_WIN * win;


  if (own_window)
  {
	  TRACE_FUNCTION("pin_edt_destroy");
	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_edt *)win->user;

	  if(data)
	  {
      /*
       * Delete WIN Handler and children handler
       */
		  pin_edit_windows = 0;
		  win_delete (data->pin_edt_win);
	      FREE_MEMORY((void *)data,(sizeof(T_pin_edt)));;
	  }
   }
}


/*******************************************************************************

 $Function:	pin_edt_win_cb
 $Description:	Callback function for editor windows
 $Returns:		Status int
 $Arguments:	window handle event, window
 
*******************************************************************************/
static int pin_edt_win_cb (T_MFW_EVENT event,T_MFW_WIN * win)
{
  T_pin_edt 	*edt_data	= (T_pin_edt *)win->user;//pin edt data
  T_MFW_WIN		*win_pin	=((T_MFW_HDR *)edt_data->parent_win)->data;
  T_pin			*pin_data	= (T_pin *)win_pin->user; // pin main data

  TRACE_EVENT_P1("pin_edt_win_cb: %d", event);
  
  if (event EQ MfwWinVisible)
  {
  /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	ATB_edit_Show(edt_data->editor);

	/* ED_UPDATE_TRIVIAL only updates text, not title */
	if (edt_data->editor->update!=ED_UPDATE_TRIVIAL)
	{
		/* Set the colour for drawing title */

		resources_setTitleColour(COLOUR_EDITOR);
		PROMPT(0,0,0,pin_data->display_id1);

		dspl_Clear(0,0, SCREEN_SIZE_X-1, edt_data->editor->attr->win_size.py-1);
	}

	edt_data->editor->update = ED_UPDATE_DEFAULT;
	
#else /* NEW_EDITOR */
    pin_icons();
    PROMPT(INFO_TEXT_X,INFO_TEXT_Y,0,pin_data->display_id1);
#endif /* NEW_EDITOR */

    return 1;
  }
  return 0;
}


/*******************************************************************************

 $Function:	pin_edt_kbd_cb 
 $Description:	Callback function for keyboard
 $Returns:		Status int
 $Arguments:	window handle event, keyborad control block
 
*******************************************************************************/
static int pin_edt_kbd_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data
//	T_sim_rem * sim_rem_data = (T_sim_rem *)win_pin->user; //sim_rem data possible too  /* Warning Correction */

	char timer = TRUE;
	timStop(edt_data->tim_out_handle);/* stop entering timer      */

	TRACE_FUNCTION("MmiPins:pin_edt_kbd_cb");
		switch(kc->code)
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
				if(edt_data->editor_index < MAX_DIG)
				{
					if(kc->code EQ KCD_STAR)
						edt_data->pin[edt_data->editor_index] = STAR;
					else
						edt_data->pin[edt_data->editor_index] = '0' + kc->code;
// Nov 24, 2005, a0876501, DR: OMAPS00045909
		   			if((edt_data->pin_case_edit NEQ CARD_REJECTED) AND (edt_data->pin_case_edit NEQ INSERT_CARD) 
						AND (edt_data->pin_case_edit NEQ SHOW_IMEI_INVALID)
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS							
						//x0pleela 30 Aug, 2006 DR: OMAPS00091250
						AND (edt_data->pin_case_edit NEQ NO_MEPD_DATA) 
#endif
						)
					{
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_AsciiChar(edt_data->editor, '*', TRUE);
#else /* NEW_EDITOR */
						edtChar(edt_data->editor_handle,'*');			       /* hide security code */
#endif /* NEW_EDITOR */
					}
					else
					{
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_AsciiChar(edt_data->editor, edt_data->pin[edt_data->editor_index], TRUE);
#else /* NEW_EDITOR */
						edtChar(edt_data->editor_handle,edt_data->pin[edt_data->editor_index]); /* not hide emergency call */
#endif /* NEW_EDITOR */
					}
						
					edt_data->editor_index++;
					check_pins(win, edt_data);		    /*	digits check depends on status pin_case */
					if(edt_data->gsm_state)
						pin_data->gsm_state = edt_data->gsm_state;

				}
				break;
			case KCD_HASH:
			case KCD_LEFT:
					switch(edt_data->pin_case_edit)
					{

						case INSERT_CARD:
						case CARD_REJECTED:
						case SHOW_IMEI_INVALID: // Nov 24, 2005, a0876501, DR: OMAPS00045909
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS							
						//x0pleela 30 Aug, 2006 DR: OMAPS00091250
						case NO_MEPD_DATA:
#endif							
							if(edt_data->emergency_call)
							{
								emerg_pin(edt_data);				/* start emergency call  */
								timer = FALSE;
							}
							else
							{
								clear_edit_array (edt_data);
								memset(edt_data->pin,'\0', sizeof(edt_data->pin));/* initial string */
								edt_data->editor_index = 0;
							}
							break;
						case SIM_PLOCK:
							timer = FALSE;
							strncpy((char*)pin_data->pin,(char*)edt_data->pin,MAX_PIN);
							pin_edt_destroy(edt_data->pin_edt_win);
			    SEND_EVENT(pin_data->pin_win,SIM_PLOCK,0,pin_data);//check P unlock code
							break;
						case SIM_NLOCK:
							strncpy((char*)pin_data->pin,(char*)edt_data->pin,MAX_PIN);
							pin_edt_destroy(edt_data->pin_edt_win);
							timer = FALSE;
			    SEND_EVENT(pin_data->pin_win,SIM_NLOCK,0,pin_data);
							break;
						case SIM_SPLOCK:
							strncpy((char*)pin_data->pin,(char*)edt_data->pin,MAX_PIN);
							pin_edt_destroy(edt_data->pin_edt_win);
							timer = FALSE;
			    SEND_EVENT(pin_data->pin_win,SIM_SPLOCK,0,pin_data);
							break;
						case SIM_NSLOCK:
							strncpy((char*)pin_data->pin,(char*)edt_data->pin,MAX_PIN);
							pin_edt_destroy(edt_data->pin_edt_win);
							timer = FALSE;
						    SEND_EVENT(pin_data->pin_win,SIM_NSLOCK,0,pin_data);
							break;
						case SIM_CLOCK:
							strncpy((char*)pin_data->pin,(char*)edt_data->pin,MAX_PIN);
							pin_edt_destroy(edt_data->pin_edt_win);
							timer = FALSE;
						    SEND_EVENT(pin_data->pin_win,SIM_CLOCK,0,pin_data);
							break;
						default:
							break;
					}
					break;
			case KCD_HUP:
			case KCD_RIGHT:
				    if(edt_data->editor_index EQ 0)
						;
					else
						edt_data->editor_index--;

					edt_data->pin[edt_data->editor_index] = '\0';
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
					edtChar(edt_data->editor_handle,ecBack);/* delete character    */

					if(strlen((char*)edt_data->pin) < MIN_PIN)
					{
						TRACE_FUNCTION("clear softknr 4");
						pin_rectClear( &edt_data->editpinAttr.win );
					}
#endif /* NEW_EDITOR */
					edt_data->emergency_call = FALSE;
					if(edt_data->pin_case_edit EQ INPUT_PUK1)  /* special case ?? */
						winShow(edt_data->pin_edt_win);
					else
						check_pins(win,edt_data);	   /* check remain for emergency call  */
				    break;
			default:
				break;
		}
	if (timer)
		tim_start(edt_data->tim_out_handle);/* start timer for entering */
	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:	check_pins  
 $Description:	check pins input string
 $Returns:		none
 $Arguments:	window, pin editor attributes
 
*******************************************************************************/
static void check_pins (T_MFW_HND win, void * edt_pin)
{
	T_pin_edt * edt_data = (T_pin_edt *)edt_pin;

	TRACE_FUNCTION("MmiPins:check_pins");

	if(strlen((char*)edt_data->pin)NEQ 0)
    {
		softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);	
	    if (strlen((char*)edt_data->pin) > FIRST_DIGIT)	       /* for every character  */
	    {
			if(strlen((char*)edt_data->pin) >= 3)
			{

				TRACE_FUNCTION("Softkeys:nr 1");
				softKeys_displayId(TxtSoftOK,TxtDelete,0, COLOUR_EDITOR_XX);
			}
			else
				softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);

			gsm_test(win,edt_data); 	  /* test if GSM string or not*/
		}
		
	    TRACE_FUNCTION("Check Length of PIN");
		if (strlen((char*)edt_data->pin) EQ (MIN_PIN - 1) || strlen((char*)edt_data->pin) EQ (MIN_PIN -2))
		{
			TRACE_FUNCTION("cm_check_emergency() TRACE PINS 2");
			if(cm_check_emergency((U8*)edt_data->pin))/* test emergency call*/
			{
				TRACE_FUNCTION("MmiPins:check_pins:emercall");
				edt_data->emergency_call = TRUE;
				softKeys_displayId(TxtSoftCall,TxtNull,0, COLOUR_EDITOR_XX);	
			}
			else
			{
// Nov 24, 2005, a0876501, DR: OMAPS00045909
				if((edt_data->pin_case_edit EQ CARD_REJECTED) OR (edt_data->pin_case_edit EQ INSERT_CARD) 
					OR (edt_data->pin_case_edit EQ SHOW_IMEI_INVALID)
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS	
					//x0pleela 30 Aug, 2006 DR: OMAPS00091250
					OR (edt_data->pin_case_edit EQ NO_MEPD_DATA)
#endif
					)
				{			  /* delete all digits	    */
					clear_edit_array (edt_data);  /*  clear editor array	 */
					memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string */
					edt_data->editor_index = 0;
				}
				edt_data->emergency_call = FALSE;
			}
		}
		if ((strlen((char*)edt_data->pin) >= MIN_PIN) && (strlen((char*)edt_data->pin) < (MAX_PIN + 1)))
		{
// Nov 24, 2005, a0876501, DR: OMAPS00045909
			if(((edt_data->pin_case_edit EQ CARD_REJECTED) OR (edt_data->pin_case_edit EQ INSERT_CARD)
				OR (edt_data->pin_case_edit EQ SHOW_IMEI_INVALID)				
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS					
				//x0pleela 30 Aug, 2006 DR: OMAPS00091250
				OR (edt_data->pin_case_edit EQ NO_MEPD_DATA)
#endif
			) && !(edt_data->gsm_state))
			{
				if(edt_data->editor_index EQ 0)
					;
				else
					edt_data->editor_index--;		  /* only emergency calls, 3 digits */

				edt_data->pin[edt_data->editor_index] = '\0';
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,ecBack);
#endif /* NEW_EDITOR */

			}
			else
			{
				TRACE_FUNCTION("clear softknr 5");

				if(((edt_data->pin_case_edit EQ INPUT_PUK1) ||(edt_data->pin_case_edit EQ INPUT_PUK2)) && !(edt_data->gsm_state))
				{	    /* PUK 1/2 (not by gsm string) max. 8 digits  */
					if(strlen((char*)edt_data->pin) >= MAX_PIN)
					{
						TRACE_FUNCTION("Softkeys:nr 2");
					}
				}
				else if((edt_data->pin_case_edit EQ SIM_CLOCK) ||
					    (edt_data->pin_case_edit EQ SIM_NLOCK) ||
						(edt_data->pin_case_edit EQ SIM_NSLOCK) ||
						(edt_data->pin_case_edit EQ SIM_SPLOCK))
				{	     /* sim lock code  max. 8 digits exept PLOCK  */
					if(strlen((char*)edt_data->pin) >= MAX_PIN)
					{
						TRACE_FUNCTION("Softkeys:nr 3");
					}
				}
				else if (edt_data->pin_case_edit EQ SIM_PLOCK)
				{		   /* sim lock	PLOCK code  max. 6 digits */
					if(strlen((char*)edt_data->pin) >= NUM_OF_PLOCK)
					{
						TRACE_FUNCTION("Softkeys:nr 4");
					}
				}
				else
				{
					if(!(edt_data->gsm_state))	  /* usual way for PIN 4 digits     */
					{
					  TRACE_FUNCTION("softkey 5");
						
					}
					else
					{
						TRACE_FUNCTION("clear softkey 6");
						pin_skClear(  );
//						dspl_Clear( PIN_EDIT_2 ); /* clear button */
					}
				}
				edt_data->emergency_call = FALSE;
			}
		}
		if((strlen((char*)edt_data->pin) > MAX_PIN) && !(edt_data->gsm_state)) /* >8 digits not allowed    */
		{
			if(edt_data->editor_index EQ 0)
				;
			else
				edt_data->editor_index--;

			edt_data->pin[edt_data->editor_index] = '\0';
			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
			edtChar(edt_data->editor_handle,ecBack);
#endif /* NEW_EDITOR */
		}
	}
}


/*******************************************************************************

 $Function:	gsm_test 
 $Description:	test if gsm string
 $Returns:		Status int
 $Arguments:	window, pin editor attributes
 
*******************************************************************************/
static int gsm_test(T_MFW_HND win,void * edt_pin)
{
	int status;
	T_pin_edt * edt_data = (T_pin_edt *)edt_pin;

	//T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data; /* Warning Correction */
	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

	TRACE_FUNCTION("MmiPins:gsm_test");

	status = ss_check_ss_string((UBYTE*)edt_data->pin);/* check GSM-String */
	switch(status)
	{
		case MFW_SS_SIM_UNBLCK_PIN:
			if(!(edt_data->gsm_state))
			{
				switch(edt_data->pin_case_edit)
				{
					case INPUT_PUK1:/* replace * with string id */
						if(!(strncmp((char*)edt_data->pin,"**05*",IDENT_GSM_1)))
						{
							edt_data->gsm_state = TRUE;
							strncpy((char*)edt_data->edtbuf,(char*)edt_data->pin,edt_data->editor_index-1);
							/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
							ATB_edit_Show(edt_data->editor);
#else /* NEW_EDITOR */
							edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */
						}
						else
						{
							edt_data->gsm_state = FALSE;
							TRACE_EVENT("dsplClearAll 2");								
							dspl_ClearAll();
							clear_edit_array (edt_data);
							pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
						}
						break;
					default:
						edt_data->gsm_state = FALSE;
						TRACE_EVENT("dsplClearAll 3");							
						dspl_ClearAll();
						clear_edit_array (edt_data);
						pin_edt_destroy(edt_data->pin_edt_win);
						SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
						break;
				}
			}
			return status;
		case MFW_SS_DIAL:	       /* string not yet detected  */
        case MFW_SS_USSD:  /*JVJ SPR 1040- The short USSD strings should also be 
                              considered in this case, since they can be confused with dialled numbers  
                             */
				edt_data->gsm_state = FALSE;
			return status;
		default:
			TRACE_FUNCTION("MmiPins:default in gsm_test");		
			edt_data->gsm_state = FALSE;
			TRACE_FUNCTION("dsplClearAll 4");							
			dspl_ClearAll();
			clear_edit_array (edt_data);
			pin_edt_destroy(edt_data->pin_edt_win);
			SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
			return status;
	}
}


#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:	gsm_sec_execute
 $Description:	GSM-String will be executed
 $Returns:		None
 $Arguments:	window, string
 
*******************************************************************************/
static void gsm_sec_execute(T_MFW_HND win,void *string)
{

	T_pin_edt * edt_data = (T_pin_edt *)string;

//	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;   /* Warning Correction */
	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

	int result;

	TRACE_FUNCTION("MmiPins:gsm_sec_execute");

	result = ss_execute_transaction((UBYTE*)edt_data->pin,0);/* ussd_man = 0 while SSD */

	TRACE_FUNCTION("dsplClearAll 5"); 
	dspl_ClearAll();
	clear_edit_array (edt_data);
	pin_edt_destroy(edt_data->pin_edt_win);

	switch(result)
	{
		case MFW_SS_SIM_REG_PW: 	/* change PIN1/2	     */
	    TRACE_FUNCTION("MmiPins:gsm_sec_execute:MFW_SS_SIM_REG_PW");
			break;
		case MFW_SS_SIM_UNBLCK_PIN:	/* unblock PIN1/2	    */
	    TRACE_FUNCTION("MmiPins:gsm_sec_execute:MFW_SS_SIM_UNBLCK_PIN");
			break;
		case MFW_SS_FAIL:
	    TRACE_FUNCTION("MmiPins:gsm_sec_execute:MFW_SS_FAIL");
			SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/* new PIN and new PIN again are not the same */
			break;
		default:
			break;
	}
}

#endif

/*******************************************************************************

 $Function:	pin_edt_kbd_long_cb
 $Description:	Callback function for keyboard long
 $Returns:		Status int
 $Arguments:	window handler event, keyboard control block 
 
*******************************************************************************/
static int pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

    TRACE_FUNCTION("MmiPins:pin_edt_kbd_long_cb");

    if ((event & KEY_CLEAR) && (event & KEY_LONG))
	{
			timStop(edt_data->tim_out_handle); /* stop entering timer	*/
			clear_edit_array(edt_data);
			memset(edt_data->pin,'\0',sizeof(edt_data->pin));/* initial string */
			edt_data->editor_index = 0;
			edt_data->emergency_call = FALSE;
			winShow(edt_data->pin_edt_win);
			return MFW_EVENT_CONSUMED;
	}
  return MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:	pin_edt_tim_out_cb
 $Description:	Callback function for timer (watchdog entering)
 $Returns:		void
 $Arguments:	window handler event, timer control block 
 
*******************************************************************************/
static void pin_edt_tim_out_cb (T_MFW_EVENT event,T_MFW_TIM * t)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;

	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

    TRACE_FUNCTION("MmiPins:pin_edt_tim_out_cb");


    TRACE_FUNCTION("clear softkey 7");

	// clear the softkeys
	pin_skClear();
	clear_edit_array(edt_data);
	// clear the input
	
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	ATB_edit_ClearAll(edt_data->editor);
#else /* NEW_EDITOR */
	pin_rectClear( &edt_data->editpinAttr.win );
    memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string  */
#endif /* NEW_EDITOR */

	edt_data->editor_index = 0;
	edt_data->gsm_state = FALSE;
	pin_data->gsm_state = FALSE;

	winShow(edt_data->pin_edt_win);
}



/*******************************************************************************

 $Function:	pin_editor 
 $Description:	Pin editor Dialog Signal Handling function
 $Returns:		void
 $Arguments:	window, window handler event, value, parameter 
 
*******************************************************************************/
static void pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;


    T_pin * pin_data = (T_pin *)parameter;//pin main data

	edt_data->pin_case_edit = pin_data->pin_case;

	TRACE_FUNCTION("MmiPins:pin_editor");

  /*
   * depending on event
   */
  switch (event)
  {
	case INPUT_PIN1:
	case INPUT_PUK1:
	case INPUT_NEW_PIN:
	case INPUT_NEW_PIN_AGAIN:
	case CARD_REJECTED:
	case INSERT_CARD:
	case SHOW_IMEI_INVALID:	// Nov 24, 2005, a0876501, DR: OMAPS00045909
	case SIM_PLOCK:
	case SIM_NLOCK:
	case SIM_NSLOCK:
	case SIM_SPLOCK:
	case SIM_CLOCK:
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS			
	case NO_MEPD_DATA: //x0pleela 30 Aug, 2006 DR: OMAPS00091250
#endif
		
		//SPR#717 - GW - Wrong structures being cleared
		memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string  */
		memset(edt_data->puk, '\0', sizeof(edt_data->puk));
		memset(edt_data->new_pin, '\0', sizeof(edt_data->new_pin));
		memset(edt_data->old_pin, '\0', sizeof(edt_data->old_pin));

		// the coordinate of the editor (enter the pin)
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_SetAttr( &edt_data->editorPinAttr, PIN_EDIT_RESET, COLOUR_EDITOR_XX, EDITOR_FONT, 0, ED_CURSOR_UNDERLINE, ATB_DCS_ASCII, (UBYTE *)edt_data->edtbuf, MAX_DIG);
		clear_edit_array(edt_data);          /*    clear editor buffer      */
		ATB_edit_Init(edt_data->editor);
#else /* NEW_EDITOR */
		editor_attr_init( &edt_data->editpinAttr, PIN_EDIT_RESET, edtCurBar1,0,(char*)edt_data->edtbuf,MAX_DIG,COLOUR_EDITOR_XX);
		clear_edit_array(edt_data);          /*    clear editor buffer      */
#endif /* NEW_EDITOR */

		edt_data->editor_index = 0;
		winShow(edt_data->pin_edt_win);
	break;
	default:
		break;
  }
}


/*******************************************************************************

 $Function:	pin_mess_create 
 $Description:	Creation of an instance  for the PIN Message dialog.
		Type of dialog : SINGLE_DYNAMIC
 $Returns:		window
 $Arguments:	parent win
 
*******************************************************************************/
T_MFW_HND pin_mess_create (T_MFW_HND parent_window)
{
  T_pin_mess * data = (T_pin_mess *)ALLOC_MEMORY (sizeof (T_pin_mess));

  T_MFW_WIN	* win;

  data->pin_mess_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)pin_mess_win_cb);

  TRACE_FUNCTION("MmiPins:pin_mess_create");

  if (data->pin_mess_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  pin_mess_windows = data->pin_mess_win;
  data->mmi_control.dialog    = (T_DIALOG_FUNC)pin_messages;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_mess_win)->data;
  win->user		      = (void *) data;

  /*
   * return window handle
   */
  return data->pin_mess_win;
}


/*******************************************************************************

 $Function:	pin_mess_destroy 
 $Description:	Destroy the pin message dialog.
 $Returns:		none
 $Arguments:	win
 
*******************************************************************************/
void pin_mess_destroy  (T_MFW_HND own_window)
{
  T_pin_mess * data;
  T_MFW_WIN * win;

  if (own_window)
  {
	TRACE_FUNCTION("MmiPins:pin_mess_destroy ");

	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_mess *)win->user;

	  if(data)
	  {

      /*
       * Delete WIN Handler
       */
      
		  pin_mess_windows = 0;
	  win_delete (data->pin_mess_win);
	      FREE_MEMORY((void *)data,(sizeof(T_pin_mess)));
	  }
   }
}

/*******************************************************************************

 $Function:	pin_mess_win_cb 
 $Description:	Callback function for message windows
 $Returns:		Status int
 $Arguments:	event, window
 
*******************************************************************************/
static int pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}




/*******************************************************************************

 $Function:	pin_messages
 $Description:	Pin message Dialog Handling function
 $Returns:		none
 $Arguments:	window, event, value, parameter
 
*******************************************************************************/
void pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
    T_pin_mess * mess_data = (T_pin_mess *)win_data->user;

	T_DISPLAY_DATA display_info;

//	T_MFW_SIM_PIN_STATUS status; //need to check the Pin 1 status  /* Warning Correction */
	//EF_SIMLCK simlck;MC, not needed SPR 1351
//	ULONG timcount = TIM_LOCK;   /* Warning Correction */
//	UBYTE i;        /* Warning Correction */

    T_pin * pin_data = (T_pin *)parameter;
	T_sim_rem * sim_rem_data = (T_sim_rem *)parameter;// sim_rem_data possible too

  TRACE_FUNCTION("Mmi.Pins:pin_messages");
  TRACE_EVENT_P1("Event: %d", event);

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull, COLOUR_STATUS_PINS);

  /*
   * depending on signal
   */
  switch (event)
  {
	case PIN1_REQ_ATT:
	case PUK1_REQ_ACT_INFO:
	case PUK1_REQ_VER_INFO:
	case GSM_FAIL:
    case NEW_PIN_FAIL:
    case PIN_OK_INFO:
    case PUK1_UNBL_FAIL_INFO:
	case PUK1_UNBL_SUCC:
	case PUK1_UNBL_FATAL_ERROR:
	case SIM_UNLOCK_OK:
#ifdef SIM_PERS
	case SIM_LOCK_PERS_CHK_OK:
	case SMLK_SHOW_FC_FAIL:
	case SMLK_SHOW_FC_SUCC:
	//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		    case SMLK_SHOW_MK_FAIL:		//Event to show a dialog saying "Master Unlock Failed"
		    case SMLK_SHOW_MK_SUCC:		//Event to show a dialog saying "Master Unlock Success"
		    case SIM_LOCK_BUSY_UNBLOCK: //wrong unblock code
#endif		    
  case PIN1_STATUS:


	
		switch(event)
		{
	       case PIN1_REQ_ATT:
		      pin_data->pin_case = PIN1_REQ;
		      display_info.TextId = TxtPINFail;
	      display_info.TextId2 = 0;
			  if(pin_data->pin_retries EQ 1)
				display_info.TextId2 = TxtOneAttemptLeft;
			  else
				display_info.TextId2 = TxtTwoAttemptsLeft;
			  break;
  
		case PUK1_REQ_ACT_INFO:		      
			pin_data->pin_case = PUK1_REQ;
#ifdef SIM_PERS 
			if( C_KEY_REQ )
				display_info.TextId = TxtPhoneBlocked;
			else
#endif /*SIM_PERS */
		       	display_info.TextId = TxtSimBlocked;

			display_info.TextId2 = 0;
			display_info.TextString2 = 0;
			break;

		case PUK1_REQ_VER_INFO:
		      pin_data->pin_case = PUK1_REQ;
		      display_info.TextId =TxtPINBlock;
			  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
		   case GSM_FAIL:
			  display_info.TextId = TxtNotAcc;
			  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
		   case PUK1_UNBL_FATAL_ERROR:
			  pin_data->pin_case = PUK1_REQ;
		      display_info.TextId = TxtNotAcc;
			  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
	   case NEW_PIN_FAIL:
		     pin_data->pin_case = INPUT_NEW_PIN;
		     display_info.TextId = TxtNewPIN;
		     display_info.TextId2 = TxtCodeInc;
			 display_info.TextString2 = 0;
			 break;
		   case PIN_OK_INFO:
		      pin_data->pin_case = PIN_OK;
			  display_info.TextId = TxtPINOK;
		  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
			case PUK1_UNBL_FAIL_INFO:
			  pin_data->pin_case = PUK1_REQ;
			  display_info.TextId = TxtCodeInc;
	      display_info.TextId2 = TxtNull;
			  display_info.TextString2 = 0;
			  break;
			case PUK1_UNBL_SUCC:
			  pin_data->pin_case = PIN_OK;
			  display_info.TextId = TxtNewPIN;
		  display_info.TextId2 = TxtChanged;
			  display_info.TextString2 = 0;
			  break;
			case SIM_UNLOCK_OK:
			  pin_data->pin_case = PIN_OK;
			  display_info.TextId = TxtUnlockOK;
	      display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
#ifdef SIM_PERS
			 case SIM_LOCK_PERS_CHK_OK:
			  pin_data->pin_case = PIN_OK;
			  display_info.TextId = TxtPhoneUnblocked;
	      display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;

			  case SMLK_SHOW_FC_FAIL:
			  {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
//				if( curr_fail_reset )
				  	pin_data->pin_case = SMLK_PUK;
//				else
//					pin_data->pin_case = PERM_BLK;
				switch(mmi_simlock_aciErrDesc )
				{
					case CME_ERR_NetworkPersPinReq:
					case CME_ERR_NetworkSubsetPersPinReq:
					case CME_ERR_ProviderPersPinReq:
					case CME_ERR_CorporatePersPinReq:
					case CME_ERR_PhSimPinReq:
						sprintf((char*)buf1,"%d attempts left",curr_succ_reset);
						sprintf((char*)buf2,"%s", "FC Reset-Success");			
						break;

					default:
						sprintf((char*)buf1,"%d attempts left",curr_fail_reset);
						sprintf((char*)buf2,"%s", "FC Reset-Failure");								
						break;
				}
				display_info.TextString= buf1;
				display_info.TextString2 = buf2;
				  break;
			  }

			 case SMLK_SHOW_FC_SUCC:
			 {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
				//if( curr_succ_reset )
				  	// leela: for simp: pin_data->pin_case = PIN_OK;
				//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
				//Changed the event from PIN_OK to SMLK_PUK bcoz after unblocking ME, 
				//for event PIN_OK it would go to idle screen without displaying menu for 
				//unlocking any categories if any
				  pin_data->pin_case = SMLK_PUK;
				//else
					//pin_data->pin_case = PERM_BLK;
				sprintf((char*)buf1,"%d attempts left",curr_succ_reset);
				sprintf((char*)buf2,"%s", "FC Reset-Success");								
				display_info.TextString= buf1;
				display_info.TextString2 = buf2;
				  break;
			 }

			//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
			//Event to display a dialog saying Master Unblock Failed
		    case SMLK_SHOW_MK_FAIL:
		    	{		    		
		    	  char buf[25];
		    	  pin_data->pin_case = SMLK_MASTERKEY;
			  sprintf((char*)buf,"%s", "Master Unlock Failed");
			  display_info.TextString= buf;
		  	  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
		    	}
		    	break;

			//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
			//Event to display a dialog saying Master Unblock Success		    	
		    case SMLK_SHOW_MK_SUCC:		
		    	{
		    	  char buf[25];
		    	  pin_data->pin_case = PIN_OK;
			  sprintf((char*)buf,"%s", "Master Unlock Success");
			  display_info.TextString= buf;
		  	  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
		    	}
		    	break;
 
			//x0pleela 13 Nov, 2006 DR: OMAPS00103356
			//Event to display a dialog saying Master Unblock Password is wrong
		    case SMLK_SHOW_WRONG_PWD:
		    	{		    		
		    	  char buf[25];
		    	  pin_data->pin_case = SMLK_MASTERKEY;
			  sprintf((char*)buf,"%s", "Incorrect Password");
			  display_info.TextString= buf;
		  	  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
		    	}
			
			//x0pleela 13 Nov, 2006 DR: OMAPS00103356
			//Event to display a dialog saying Master unlocking operation is successful
		    case SMLK_SHOW_DEP_CATS_UNLOCKED:
		    	{		    		
		    	  char buf[25];
		    	  pin_data->pin_case = SMLK_MASTERKEY;
			  sprintf((char*)buf,"%s", "Master Unlock Success");
			  display_info.TextString= buf;
		  	  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
		    	}
		    	break;
 
			//x0pleela 26 July, 2006 ER: OMAPS00087586, OMAPS00087587
			//Event to display a dialog saying Busy for wrong entry of unblocking password		    	
			case SIM_LOCK_BUSY_UNBLOCK: //wrong unblock code	
			{
				pin_data->pin_case = SMLK_PUK;
				display_info.TextId = TxtBusy;
			}
			break;
#endif			  
//nm insert new one
			case PIN1_STATUS:
			  pin_data->pin_case = PIN1_STATUS;

			  if(pin_data->set_state EQ ENABLE)
			  {
				display_info.TextId = TxtEnabled;
		      }
			  else
			  {
				display_info.TextId = TxtDisabled;
			  }

		      display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;
//NM
	  
		}

		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)pin_info_cb, THREE_SECS, 0 );
		display_info.Identifier = pin_data->pin_case;
		info_dialog(pin_data->pin_win,&display_info); //information screen
		pin_mess_destroy(mess_data->pin_mess_win);
#ifdef SIM_PERS		
		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//Delete the simp_list window handler  after displaying the dialog
		if(pin_data->simp_list_win)
			win_delete(pin_data->simp_list_win);
#endif
		break;
	case SIM_LOCK_ERR:
		display_info.TextId = TxtCodeInc;
		display_info.TextId2 = TxtPleaseWait;
		display_info.Identifier = pin_data->pin_case;//last pin_case

		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)pin_info_cb, 5*TIM_LOCK_JUMP, 0 );
		pin_mess_destroy(mess_data->pin_mess_win);
		info_dialog(pin_data->pin_win,&display_info); //information screen
		break;

	case SIM_REMOVED:
		pin_data->display_id1 = TxtNoCard;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(sim_rem_data->sim_rem_win,TXT_INSERT_CARD,0,pin_data);
		break;
	case INVALID_CARD:
		pin_data->display_id1 = TxtInvalidCard;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_INVALID_CARD,0,pin_data);
		break;
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS			
//x0pleela 30 Aug, 2006 DR: OMAPS00091250
	case NO_MEPD:
		pin_data->display_id1 = TxtNoMEPD;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_NO_MEPD,0,pin_data);
		break;		
#endif		
	case NO_SIM_CARD:
		pin_data->display_id1 = TxtNoCard;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_NO_CARD,0,pin_data);
		break;
// Nov 24, 2005, a0876501, DR: OMAPS00045909
	case INVALID_IMEI:
		pin_data->display_id1 = TxtInvalidIMEI;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_INVALID_IMEI,0,pin_data);
		break;
	case SIM_LOCK_ALL:
		pin_data->display_id1 = TxtBlckPerm;
		pin_data->display_id2 = TxtDealer;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_LOCK_ALL,0,pin_data);
		break;
	case SIM_PLOCK_REQ:
		pin_data->display_id1 = TxtEnterPCK;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_PLOCK,0,pin_data);
		break;
	case SIM_NLOCK_REQ:
		pin_data->display_id1 = TxtEnterNCK;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_NLOCK,0,pin_data);
		break;
	case SIM_CLOCK_REQ:
		pin_data->display_id1 = TxtEnterCCK;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_CLOCK,0,pin_data);
		break;
	case SIM_NSLOCK_REQ:
		pin_data->display_id1 = TxtEnterNSCK;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_NSLOCK,0,pin_data);
		break;
	case SIM_SPLOCK_REQ:
		pin_data->display_id1 = TxtEnterSPCK;
		pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_SIM_SPLOCK,0,pin_data);
		break;
	default:
		break;
  }
}


#ifndef NEW_EDITOR
/*******************************************************************************

 $Function:	pin_icons 
 $Description:	display back from pins
 $Returns:		none
 $Arguments:	none
 
*******************************************************************************/
static void pin_icons(void)
{

	TRACE_FUNCTION("pin_icons");


	// clear the softkeys
	TRACE_FUNCTION("clear softkey 8");
	pin_skClear();

	TRACE_FUNCTION("dsplClearAll 6");
	dspl_ClearAll();

	GlobalIconStatus = GlobalSignalIconFlag | GlobalBatteryIconFlag |
		       GlobalVoiceMailIconFlag 
		      | GlobalCallForwardingIconFlag| GlobalKeyplockIconFlag
					  | GlobalRingerIconFlag | GlobalRingVibrIconFlag
					  | GlobalVibratorIconFlag | GlobalAlarmIconFlag
					  | GlobalSilentRingerIconFlag
			#ifdef FF_MMI_CPHS
					  | GlobalRoamingIndFlag
			#endif

/* x0045876, 14-Aug-2006 (WR - nested comment is not allowed) */
/* SH 18/01/02. Flag for GPRS icon. */
			
/* SH 18/01/02. Flag for GPRS icon.
   Note: SPR877 - Homezone icon not displayed in PIN screen. */
				#ifdef MMI_GPRS_ENABLED
					  | GlobalGPRSOnIconFlag
				#endif
				;
	iconsShow();		/* representation of the desired icons */
}

#endif

/*******************************************************************************

 $Function:	clearEditArray 
 $Description:	clear EditArray
 $Returns:		none
 $Arguments:	pin editor attributes
*******************************************************************************/
static void clear_edit_array (void * parameter)
{
//	U8 i;     /* Warning Correction */
	T_pin_edt * edt_data = (T_pin_edt *)parameter;

	TRACE_FUNCTION("MmiPins:clearEditArray");

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	memset(edt_data->edtbuf,'\0',sizeof(edt_data->edtbuf));
	ATB_edit_Reset(edt_data->editor);
	ATB_edit_Unhide(edt_data->editor);
#else /* NEW_EDITOR */
	memset(edt_data->edtbuf,'\0',sizeof(edt_data->edtbuf));
	edtReset(edt_data->editor_handle);
	edtUnhide(edt_data->editor_handle);
#endif

}


/*******************************************************************************

 $Function:	pinsIsFocussed
 $Description:	check MmiPins windows focussed
 $Returns:		Status int
 $Arguments:	none
*******************************************************************************/

int pinsIsFocussed(void)
{
	int status;

	if (winIsFocussed(pin_windows) || winIsFocussed(pin_edit_windows) ||
		winIsFocussed(pin_editor_window) ||winIsFocussed(pin_mess_windows))
		status = FOCUSSED_PINS;
	else
		status = NOT_FOCUSSED_PINS;

	return status;
}


/*******************************************************************************

 $Function:	pinsSetIsFocussed
 $Description:	check MmiPins in settings windows focussed
 $Returns:		Status int
 $Arguments:	none
*******************************************************************************/
int pinsSetIsFocussed(void)
{
	int status;

	if (winIsFocussed(set_pin_windows))
		status = FOCUSSED_PINS;
	else
		status = NOT_FOCUSSED_PINS;

	return status;
}


/*******************************************************************************

 $Function:	emerg_pin
 $Description:	emergency call
 $Returns:		none
 $Arguments:	pin editor attributes
*******************************************************************************/
static void emerg_pin(void * string)
{
	T_pin_edt * edt_data = (T_pin_edt *)string;

	edt_data->emergency_call = FALSE;
	clear_edit_array(edt_data);	      /*   clear editor array	*/
	callNumber((UBYTE*)edt_data->pin);	   /* emergency call	*/
	memset(edt_data->pin, '\0', sizeof(edt_data->pin));/*initial string*/
	edt_data->editor_index = 0;
TRACE_FUNCTION("clear softkey 9"); 
	pin_skClear();
}



/*******************************************************************************

 $Function:	pin_info_cb
 $Description:	pin information screen call back
 $Returns:		none
 $Arguments:	win, identifier, reason for callback (not used)
*******************************************************************************/
int pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;

    TRACE_EVENT_P1 ("pin_info_cb() %d", identifier);

	switch(identifier)
	{
		case PIN_OK:
		{
#ifdef SIM_PERS
			int curr_fail_reset1, curr_succ_reset1;
#endif
			TRACE_FUNCTION("pin_info_cb: PIN_OK");

#ifdef SIM_PERS
			mfw_simlock_check_status(0, &curr_fail_reset1, &curr_succ_reset1);

			if (curr_succ_reset1 == 0)
			{
				perm_blocked =1;
				SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
			}		
			else
#endif
				SEND_EVENT(pin_data->pin_win,PIN_OK_END,0,NULL);//PIN was needed
		}
			break;
	
		case PIN1_REQ:
		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//case PUK1_REQ:		//Handling PUK1_REQ separately as a menu should be displayed for Unblocking ME and Master Unlock
		case INPUT_NEW_PIN:
			
			SEND_EVENT(pin_data->pin_win,identifier,0,NULL);//pin_main data !!
			break;


			case PUK1_REQ:
#ifdef SIM_PERS
				//x0pleela 26 Sep, 2006 DR: OMAPS00096565
				//Adding a check for C_KEY_REQ. If set, displaySIMP menu
				//Else prompt the user for PUK entry
				if( C_KEY_REQ ) 
				{
				//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
				//Handling PUK1_REQ separately as a menu should be displayed for Unblocking ME and Master Unlock
				  if( simlock_get_masterkey_status())
				  {
				  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
				  }
				}
				  else
#endif
			  SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,NULL);//pin_main data !!
			break;
#ifdef SIM_PERS
		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//Handling SMLK_MASTERKEYto display menu for Unblocking ME or unlocking categories if any and Master Unlock
		case SMLK_MASTERKEY:
			//x0pleela 25 Sep, 2006 ER: OMAPS00095524	
			//check for CME class and handle all CME errors
			if( ((mmi_simlock_aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Cme) 	
			{
			switch(mmi_simlock_aciErrDesc & 0x0000FFFF)
			{
				case CME_ERR_NetworkPersPinReq:
					TRACE_EVENT("CME_ERR_NetworkPersPinReq");
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_NetworkSubsetPersPinReq:
						TRACE_EVENT("CME_ERR_NetworkSubsetPersPinReq");
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NSLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_ProviderPersPinReq:
						TRACE_EVENT("CME_ERR_ProviderPersPinReq");
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_SPLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_CorporatePersPinReq:
						TRACE_EVENT("CME_ERR_CorporatePersPinReq");
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_CLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_PhSimPinReq:
						TRACE_EVENT("CME_ERR_PhSimPinReq");
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_PLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
							SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ,0,pin_data);
						break;

					case CME_ERR_PhoneFail:
					case CME_ERR_NetworkPersPukReq:
					case CME_ERR_NetworkSubsetPersPukReq:
					case CME_ERR_ProviderPersPukReq:
					case CME_ERR_CorporatePersPukReq:
					case CME_ERR_WrongPasswd:
				
						TRACE_FUNCTION("PUK Req");
						C_KEY_REQ = 1;
						  if( simlock_get_masterkey_status())
						  {
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						{							
							SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
						}
						break;	
						
					default:
						TRACE_EVENT("Undefined error");
						break;
		    			}
				}
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524	
				//check for Ext class and handle all Extension errors
				else if( ((mmi_simlock_aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext)
				{
					switch(mmi_simlock_aciErrDesc & 0x0000FFFF)
					{
						//Request for Blocked Network Password
						case EXT_ERR_BlockedNetworkPersPinReq:
							TRACE_EVENT("EXT_ERR_BlockedNetworkPersPinReq");
							  if( simlock_get_masterkey_status())
							  {
							  	mmi_simlock_locktype = MFW_SIM_PBLOCK;
							  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
							  }
							  else
								SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ,0,pin_data);
						break;

						//Request for Blocked Network Unblock code 
						case EXT_ERR_BlockedNetworkPersPukReq:
							TRACE_EVENT("PUK Req");
							C_KEY_REQ = 1;
							  if( simlock_get_masterkey_status())
							  {
							  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
							  }
							  else
							{							
								SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
							}
						break;	

						default:
							break;
					}
				}
				break;

		case SMLK_PUK:
		{
			int status1,curr_fail_reset1, curr_succ_reset1;

			status1 = mfw_simlock_check_status(0, &curr_fail_reset1, &curr_succ_reset1);

			if (status1 == MFW_SIM_PERM_BLOCKED)
			{
				perm_blocked =1;
				SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
			}
			else
			{
				//x0pleela 11 Sep, 2006  DR: OMASP00094215
				//checking for Class type and the ext busy error
				if( ( ((mmi_simlock_aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
					(mmi_simlock_aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
				{
					C_KEY_REQ = 1;
						  if( simlock_get_masterkey_status())
						  {
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						{							
							SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
						}
				}
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524	
    				//check for Cme class and handle all CME errors
				else if( ((mmi_simlock_aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Cme)
				{
				//end of x0pleela 11 Sep, 2006  DR: OMASP00094215
			   	switch(mmi_simlock_aciErrDesc & 0x0000FFFF)
				{
					case CME_ERR_NetworkPersPinReq:
						TRACE_EVENT("CME_ERR_NetworkPersPinReq");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_NetworkSubsetPersPinReq:
						TRACE_EVENT("CME_ERR_NetworkSubsetPersPinReq");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_NSLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_ProviderPersPinReq:
						TRACE_EVENT("CME_ERR_ProviderPersPinReq");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
					       if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_SPLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						    SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_CorporatePersPinReq:
						TRACE_EVENT("CME_ERR_CorporatePersPinReq");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock				  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_CLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ,0,pin_data);
						break;
						
					case CME_ERR_PhSimPinReq:
						TRACE_EVENT("CME_ERR_PhSimPinReq");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock					  
						  if( simlock_get_masterkey_status())
						  {
						  	mmi_simlock_locktype = MFW_SIM_PLOCK;
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
						SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ,0,pin_data);
						break;

					case CME_ERR_PhoneFail:
					case CME_ERR_NetworkPersPukReq:
					case CME_ERR_NetworkSubsetPersPukReq:
					case CME_ERR_ProviderPersPukReq:
					case CME_ERR_CorporatePersPukReq:
					case CME_ERR_WrongPasswd:
									
							TRACE_FUNCTION("PUK Req");
						C_KEY_REQ = 1;
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
						  if( simlock_get_masterkey_status())
						  {
						  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
						  }
						  else
								SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
						break;	
						
					default:
						TRACE_EVENT("Undefined error");
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						//If no CME error, which implies none of the categories are locked
						//so send PIN_OK_END event for camping on to the network
						SEND_EVENT(pin_data->pin_win,PIN_OK_END,0,NULL);//PIN was needed
						break;
		    			}
				//x0pleela 11 Sep, 2006  DR: OMASP00094215
				}
				//end of x0pleela 11 Sep, 2006  DR: OMASP00094215

				    //x0pleela 25 Sep, 2006 ER: OMAPS00095524	
				    //check for Ext class and handle all Extension errors
				else if( ((mmi_simlock_aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext)
				{
					switch(mmi_simlock_aciErrDesc & 0x0000FFFF)
					{
						//Request for Blocked Network Password
						case EXT_ERR_BlockedNetworkPersPinReq:
							TRACE_EVENT("EXT_ERR_BlockedNetworkPersPinReq");
							//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
							//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
							if( simlock_get_masterkey_status())
							{
								mmi_simlock_locktype = MFW_SIM_PBLOCK;
								pin_data->simp_list_win = mmi_simlock_category_menus(win);
							}
							else
								SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ,0,pin_data);
							break;

						//Request for Blocked Network Unblock code 
						case EXT_ERR_BlockedNetworkPersPukReq:
							TRACE_EVENT("PUK Req");
							C_KEY_REQ = 1;
							//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
							//Displays menu for Unblocking ME or unlocking categories if any and Master Unlock
							if( simlock_get_masterkey_status())
							{
								pin_data->simp_list_win = mmi_simlock_category_menus(win);
							}
							else
							{							
								SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
							}
							break;	

						default:
							break;
					}
				}
			}
		}
		break;
#endif

		case INPUT_PIN1:
			SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,NULL);//pin_main data !!
			break;
		case INPUT_PUK1:
			TRACE_FUNCTION("INPUT_PUK1");
			SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,NULL);//pin_main data !!
			break;
		/*mc SPR 1351, check if simlocks blocked before requesting them again*/
		case SIM_PLOCK:
			if (sim_check_sim_Plock() != MFW_SIM_BLOCKED)
				SEND_EVENT(pin_data->pin_win,SIM_PLOCK_REQ,0,NULL);//pin_main data !!
			else
			{	//ShowMessage(idle_get_window(), TxtBlckPerm, TxtDealer);
				pin_destroy(pin_data->pin_win);
			}
			break;
		case SIM_NLOCK:
			if (sim_check_sim_Nlock() != MFW_SIM_BLOCKED)
				SEND_EVENT(pin_data->pin_win,SIM_NLOCK_REQ,0,NULL);//pin_main data !!
			else
			{	//ShowMessage(idle_get_window(), TxtBlckPerm, TxtDealer);
				pin_destroy(pin_data->pin_win);
			}
			break;
		case SIM_SPLOCK:
			if (sim_check_sim_SPlock() != MFW_SIM_BLOCKED)
				SEND_EVENT(pin_data->pin_win,SIM_SPLOCK_REQ,0,NULL);//pin_main data !!
			else
			{	//ShowMessage(idle_get_window(), TxtBlckPerm, TxtDealer);
				pin_destroy(pin_data->pin_win);
			}
			break;
		case SIM_NSLOCK:
			if (sim_check_sim_NSlock() != MFW_SIM_BLOCKED)
				SEND_EVENT(pin_data->pin_win,SIM_NSLOCK_REQ,0,NULL);//pin_main data !!
			else
			{	//ShowMessage(idle_get_window(), TxtBlckPerm, TxtDealer);
				pin_destroy(pin_data->pin_win);
			}
		    break;
		case SIM_CLOCK:
			if (sim_check_sim_Clock() != MFW_SIM_BLOCKED)
				SEND_EVENT(pin_data->pin_win,SIM_CLOCK_REQ,0,NULL);//pin_main data !!
			else
			{	//ShowMessage(idle_get_window(), TxtBlckPerm, TxtDealer);
				pin_destroy(pin_data->pin_win);
			}
			break;
			/*MC end*/
		case PIN1_STATUS:
				SEND_EVENT(pin_data->pin_win,PIN1_STATUS_END,0,NULL);//pin_main data !!
			break;
		default:
			break;
	}
	return 1;
}

/*******************************************************************************

 $Function:	sim_rem_init
 $Description:	This is the start-up time initialisation routine.
		For compatibility reasons the functions is still there.
 $Returns:		none
 $Arguments:	parent window
*******************************************************************************/
void sim_rem_init (T_MFW_HND parent_window)
{
  sim_rem_create (parent_window);
}




/*******************************************************************************

 $Function:	sim_rem_exit
 $Description:	his is the shutdown time clean-up routine.
 $Returns:		none
 $Arguments:	parent window
*******************************************************************************/
void sim_rem_exit(T_MFW_HND parentWindow)
{
  sim_rem_destroy (sim_rem_data.sim_rem_win);
}



/*******************************************************************************

 $Function:	sim_rem_create 
 $Description:	Creation of an instance for the SIM remove dialog.
				Type of dialog : SINGLE_STATIC
				Top Window must be available at any time, only one instance.
 $Returns:		window
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND sim_rem_create (T_MFW_HND parent_window)
{

  T_sim_rem * data = &sim_rem_data;
  T_MFW_WIN  * win;

  data->sim_rem_win = win_create (parent_window, 0,	MfwWinVisible, (T_MFW_CB)sim_rem_win_cb);

  if (data->sim_rem_win EQ 0)
    return 0;
  else
  {
	  /*
	   * Create window handler
	   */
	  data->mmi_control.dialog    = (T_DIALOG_FUNC)sim_rem_main;
	  data->mmi_control.data      = data;
	  data->parent_win = parent_window;
	  win			      = ((T_MFW_HDR *)data->sim_rem_win)->data;
	  win->user		      = (void *) data;

	  /*
	   * Create any other handler
	   */

	  data->sim_handle = sim_create(0,E_SIM_ALL_SERVICES,(MfwCb)sim_rem_cb_main);
	  winShow(data->sim_rem_win);
	  /*
	   * return window handle
	   */
	  return data->sim_rem_win;
  }
}



/*******************************************************************************

 $Function:	sim_rem_destroy 
 $Description:	Destroy the sim remove dialog.
 $Returns:		none
 $Arguments:	window
*******************************************************************************/
void sim_rem_destroy (T_MFW_HND own_window)
{
  T_sim_rem  * data;
  T_MFW_WIN * win;

  if (own_window)
  {
	 win = ((T_MFW_HDR *)own_window)->data;
     data = (T_sim_rem *)win->user;

	 if(data)
	 {
      /*
       * Exit SIM and Delete SIM Handler
       */
       sim_delete (data->sim_handle);

      /*
       * Delete WIN Handler
       */
      win_delete (data->sim_rem_win);
	 }
  }
}


/*******************************************************************************

 $Function:	sim_rem_win_cb	
 $Description:	Callback function for windows
 $Returns:		Status int
 $Arguments:	event, window
*******************************************************************************/
static int sim_rem_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  if (event EQ MfwWinVisible)
  {
    /*
     * Top Window has no output
     */
	TRACE_FUNCTION("dsplClearAll 7");	   
    dspl_ClearAll();
	return 1;
  }
  return 0;
}



/*******************************************************************************

 $Function:	sim_rem_cb_main 
 $Description:	SIM remove handler
 $Returns:		Status int
 $Arguments:	event, sim status
*******************************************************************************/
int sim_rem_cb_main (T_MFW_EVENT event, T_MFW_HND para)
{

	T_MFW_SIM_STATUS * status;

	T_MFW_HND sim_rem_win;

    status = (T_MFW_SIM_STATUS *)para;
	TRACE_FUNCTION("MmiPins:sim_rem_cb_main");

    switch(event)
	{
	    case E_SIM_STATUS:
		  /*
		   * check whether decoded message is available
		   */
			if( para EQ (T_MFW_SIM_STATUS *)NULL )
				return MFW_RES_ILL_HND;

			status = (T_MFW_SIM_STATUS *)para;
			 switch (status->sim_procedure)
			 {
				case MFW_SIM_REMOVED:
					switch (status->sim_status)
					{
				/*
				 *  SIM card is removed
				 */
						case MFW_SIM_NO_SIM_CARD:
							TRACE_FUNCTION("sim_rem_cb_main:SIM removed");
							sim_rem_win = sim_rem_create(0);
							if(sim_rem_win)
							{
								SEND_EVENT(sim_rem_win,SIM_REMOVED,0,NULL);
							}
				/*
				 * limited shall be the requested service
				 * if no service is reached, the limited
				 * service is requested*/
						nm_registration (NM_AUTOMATIC, 0, TRUE);
							break;
						default:
							return 0;
					}
				break;
				default:
					return 0;
			 }
		 return 1;
	}
	
	/* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function ) */
	return 0;
}



/*******************************************************************************

 $Function:	sim_rem_main 
 $Description:	SIM remove main Dialog Handling function
 $Returns:		None
 $Arguments:	window, event, value, parameter
*******************************************************************************/
static void sim_rem_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_sim_rem * sim_rem_data = (T_sim_rem *)win_data->user;//sim remove main data
//	T_pin_mess * mess_data = (T_pin_mess *) parameter;//pin message data  /* Warning Correction */

	T_MFW_HND pin_mess_win;
	T_MFW_HND pin_edt_win;

	TRACE_FUNCTION("MmiPins:sim_rem_main");

  switch(event)
  {
	  case SIM_REMOVED:					      //Text "Insert Card "
		   pin_mess_win = pin_mess_create(sim_rem_data->sim_rem_win);
		   if(pin_mess_win)
		   {
				SEND_EVENT(pin_mess_win,event,0,sim_rem_data);
		   }
		   break;
      case TXT_INSERT_CARD:
			sim_rem_data->pin_case = INSERT_CARD;
		    pin_edt_win = pin_edt_create(sim_rem_data->sim_rem_win);// Start: enter only emerg call
			if(pin_edt_win)
			{
			    SEND_EVENT(pin_edt_win,INSERT_CARD,0,sim_rem_data);
			}
			break;
	  default:
		  break;
  }

}




/*******************************************************************************

 $Function:	setting_pin_init 
 $Description:	This is the start-up time initialisation routine.
		Start in the Menu
 $Returns:		None
 $Arguments:	parent window
*******************************************************************************/
void setting_pin_init (T_MFW_HND parent_window)
{
  setting_pin_create (parent_window);
}



/*******************************************************************************

 $Function:	setting_pin_create
 $Description:	Creation of an instance for the Settings PIN main dialog.
				Type of dialog : SINGLE_DYNAMIC
				Top Window must be available at any time, only one instance.
 $Returns:		window
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND setting_pin_create (T_MFW_HND parent_window)
{

  T_pin * data = (T_pin *)ALLOC_MEMORY (sizeof (T_pin));
  T_MFW_WIN	* win;

  data->pin_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)setting_pin_win_cb);

  TRACE_FUNCTION("MmiPins:setting_pin_create");

  set_pin_windows = data->pin_win;

  if (data->pin_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)setting_pin_main;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_win)->data;
  win->user		      = (void *) data;

  /*
   * Create any other handler
   */

  data->sim_handle = sim_create(data->pin_win,E_SIM_ALL_SERVICES,(MfwCb)sim_event_cb_setting);

  /*
   * Initialise
   */
 data->set_state = DEFAULT;
 data->puk_request = FALSE;


  mfwSetSignallingMethod(1);//mfw focus handling
  winShow(data->pin_win);//focus on settings:PIN/PIN2 Change/act/Deac


  /*
   * return window handle
   */
  return data->pin_win;
}


/*******************************************************************************

 $Function:	setting_pin_destroy 
 $Description:	Destroy the settings pin main dialog.
 $Returns:		none
 $Arguments:	window
*******************************************************************************/
void setting_pin_destroy  (T_MFW_HND own_window)
{
  T_pin * data;
  T_MFW_WIN * win;

  if (own_window)
  {
	 TRACE_FUNCTION("MmiPins:setting_pin_destroy");

	 win = ((T_MFW_HDR *)own_window)->data;
     data = (T_pin *)win->user;

	 if(data)
	 {
      /*
       * Exit SIM and Delete SIM Handler
       */
	 sim_delete (data->sim_handle);

      /*
       * Delete WIN Handler
       */
	  set_pin_windows = 0;
      win_delete (data->pin_win);
      data->pin_win=0;
	if(data)
	 FREE_MEMORY((void *)data,(sizeof(T_pin)));
	 }
  }
}


/*******************************************************************************

 $Function:	setting_pin_win_cb 
 $Description:	Callback function for main windows in settings
 $Returns:		Status int
 $Arguments:	event, window
*******************************************************************************/
static int setting_pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  TRACE_FUNCTION("setting_pin_win_cb");
  if (event EQ MfwWinVisible)
  {
    /*
     * Top Window has no output
     */
	/* NDH : Removed to prevent Blue Screen on Pin Entry */
	return 1;
  }
  return 0;

}


/*******************************************************************************

 $Function:	sim_event_cb_setting 
 $Description:	SIM event handler for Setttings Pins
 $Returns:		Status int
 $Arguments:	event, sim status
*******************************************************************************/
int sim_event_cb_setting (T_MFW_EVENT event, T_MFW_HND para)
{

	T_MFW_SIM_STATUS * status;

    T_MFW_HND win = mfw_parent(mfw_header());


	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;

	status = (T_MFW_SIM_STATUS *)para;

	TRACE_FUNCTION("Sim Event Handler of Settings Pin"); 
 switch(event)
 {
	case E_SIM_INSERTED://handle in sim_rem_cb_main
	    return 0;
	case E_SIM_STATUS:
	  /*
	   * check whether decoded message is available
	   */
		if( para EQ (T_MFW_SIM_STATUS *)NULL )
		   return MFW_RES_ILL_HND;

		status = (T_MFW_SIM_STATUS *)para;
		 switch (status->sim_procedure)
		 {
		/*
		 * Initial activation of the SIM card
		 */
			case MFW_SIM_VERIFY:
			//xrashmic 1 Jul, 2006 OMAPS00075784
			//When pin is blocked, the user is immediatly prompted with puk 
			// entry screen or else the blue screen is displayed
			case MFW_SIM_ACTIVATION:
		  /*
		   * check the result of verify
		   */
			  switch (status->sim_status)
			  {
			/*
			 * PIN 1 must be entered
			 */
					case MFW_SIM_PIN_REQ:
					if(status->sim_pin_retries < PIN_ENTRY_MAX)
					{
							TRACE_FUNCTION("sim_event_cb_settings V:PIN1 req-retr");
							pin_data->pin_retries = status->sim_pin_retries;
							SEND_EVENT(pin_data->pin_win,PIN1_REQ_ATT,0,pin_data);
					}
					else
					{
							TRACE_FUNCTION("sim_event_cb_setting V:PIN1 req");
							SEND_EVENT(pin_data->pin_win,PIN1_REQ_OLD,0,pin_data);
					}
					break;
			/*
			 * PIN 2 must be entered
			 */
					case MFW_SIM_PIN2_REQ:
					if(status->sim_pin_retries < PIN_ENTRY_MAX)
					{
							TRACE_FUNCTION("sim_event_cb_settings V:PIN2 req-retr");
							pin_data->pin_retries = status->sim_pin_retries;
							SEND_EVENT(pin_data->pin_win,PIN2_REQ_ATT,0,pin_data);
					}
					else
					{
							TRACE_FUNCTION("sim_event_cb_setting V:PIN2 req");
							SEND_EVENT(pin_data->pin_win,PIN2_REQ,0,pin_data);
					}
					break;
			/*
			 * SIM card is blocked, PUK 1 is needed
			 */
					case MFW_SIM_PUK_REQ:
						pin_data->puk_request = TRUE;
						TRACE_FUNCTION("sim_event_cb_setting V:PUK1 req");
						SEND_EVENT(pin_data->pin_win,PUK1_REQ_VER_INFO,0,pin_data);
						break;
			/*
			 * SIM card is blocked, PUK 2 is needed
			 */
					case MFW_SIM_PUK2_REQ:
						pin_data->puk_request = TRUE;
						TRACE_FUNCTION("sim_event_cb_setting V:PUK2 req");
						SEND_EVENT(pin_data->pin_win,PUK1_REQ_VER_INFO,0,pin_data);
						break;

			/*
			 * PIN entering successfull
			 */
					case MFW_SIM_NO_PIN:
						TRACE_FUNCTION("sim_event_cb_setting V:PIN ok ");
						pin_data->pin_case = PIN_OK_INFO;
						SEND_EVENT(pin_data->pin_win,PIN_OK_INFO,0,pin_data);
						break;

                    /* Marcus: Issue 1609: 23/01/2003: Start */
                    case MFW_SIM_FAILURE:
                        /* An attempt to verify a PIN has failed */
                        if (status->sim_status_type == MFW_SIM_PIN2)
                        {
                            /* PIN2 verification not supported by this SIM */
                            SEND_EVENT(pin_data->pin_win,PIN2_SIM_FAILURE,0,pin_data);
                        }
                        else /* Not verifying PIN2, presumably verifying PIN1 */
                            return 0;
                        break;
                    /* Marcus: Issue 1609: 23/01/2003: End */
                    
					default:
						return 0;
			  }
			break;
			/*
			 * response to PUK entering
			 */
			case MFW_SIM_UNBLOCK:
			  switch (status->sim_status)
			  {
				case MFW_SIM_PUK_REQ:
					TRACE_FUNCTION("sim_event_cb_setting:unblock failure");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FAIL_INFO,0,pin_data);
					break;
				case MFW_SIM_PUK2_REQ:
					TRACE_FUNCTION("sim_event_cb_setting:unblock failure");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FAIL_INFO,0,pin_data);
					break;
				case MFW_SIM_SUCCESS:
					TRACE_FUNCTION("sim_event_cb_setting:unblock success");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_SUCC,0,pin_data);
					break;
				case MFW_SIM_FAILURE:
					TRACE_FUNCTION("sim_event_cb_setting:unblock fatal error");
					SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FATAL_ERROR,0,pin_data);
					break;				
				case MFW_SIM_INVALID_CARD:
					TRACE_FUNCTION("sim_event_cb_main:unblock failed, no more retries");
					SEND_EVENT(pin_data->pin_win,INVALID_CARD,0,pin_data);
					break;
				default:
					return 0;
			  }
			break;

			case MFW_SIM_REMOVED:
				return 0;/* there not handled  */
			case MFW_SIM_CHANGE:
			 switch (status->sim_status)
			 {
				case MFW_SIM_SUCCESS:
					TRACE_FUNCTION("sim_event_cb_setting: succ/CH");
					if(pin_data->set_state EQ IDLE_GSM)
					{
						SEND_EVENT(pin_data->pin_win,PIN1_CH_SUCC,0,pin_data);/* change PIN by	gsm string in idle */
					}
					else if(pin_data->pin_case EQ INPUT_NEW_PIN_AGAIN OR (status->sim_status_type EQ MFW_SIM_PIN1))
					{
						SEND_EVENT(pin_data->pin_win,PIN1_CH_SUCC,0,pin_data);/* change PIN, also by gsm string */
					}
					else
					{
						SEND_EVENT(pin_data->pin_win,PIN2_CH_SUCC,0,pin_data);/* change PIN2, also by gsm string */
					}
					break;
				case MFW_SIM_FAILURE:
					TRACE_FUNCTION("sim_event_cb_setting: fail/CH");
					if(!pin_data->puk_request)
					{
						if(pin_data->pin_case EQ INPUT_NEW_PIN_AGAIN)
						{
							SEND_EVENT(pin_data->pin_win,NEW_PIN_FAIL,0,pin_data);/* new Pin failed */
						}
						else if(pin_data->pin_case EQ INPUT_NEW_PIN2_AGAIN)
						{
							SEND_EVENT(pin_data->pin_win,NEW_PIN2_FAIL,0,pin_data);/* new Pin2 failed */
						}
						else if(pin_data->gsm_state)
						{
							if(sim_pin_count(MFW_SIM_PIN1) <= 0 )//  May 13, 2004        REF: CRR 13632  xvilliva
							{
								pin_data->puk_request = TRUE;
								SEND_EVENT(pin_data->pin_win,PUK1_REQ_VER_INFO,0,pin_data);
							}
							else
							SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,pin_data);
						}
					 }
					else
					{
						if(pin_data->pin_case EQ INPUT_NEW_PIN_AGAIN)
						{
							SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
						}
						else
						{
							SEND_EVENT(pin_data->pin_win,PUK2_REQ,0,pin_data);
						}

					}
				break;
			 default:
				return 0;
			}
			break;
		    case MFW_SIM_DISABLE:
		    case MFW_SIM_ENABLE:
		   switch (status->sim_status)
			   {
			     case MFW_SIM_SUCCESS:
		    TRACE_FUNCTION("sim_event_cb_setting: succ/DIS/E");

		    if (status->sim_procedure EQ MFW_SIM_ENABLE)
					{
					  SEND_EVENT(pin_data->pin_win,PIN1_ENAB_END,0,pin_data);
					}
				    else
					{
					  SEND_EVENT(pin_data->pin_win,PIN1_DISAB_END,0,pin_data);
					}
				  break;
			     case MFW_SIM_FAILURE:
				    TRACE_FUNCTION("sim_event_cb_setting: fail/DIS/E");
				    pin_data->pin_retries = sim_pin_count(MFW_SIM_PIN1);
				    SEND_EVENT(pin_data->pin_win,PIN1_ENDIS_FAIL,0,pin_data);
				  break;
			    default:
				  return 0;
			   }
			break;
			default:
				return 0;
		}
	    return 1;
  }
  /* x0045876, 14-Aug-2006 (WR - missing return statement at end of non-void function) */
   return 0;
}



/*******************************************************************************

 $Function:	set_pin_ch_item_flag
 $Description:	decision about show entry "PIN change"
 $Returns:		Status int; 0 if PIN enabled, 1 if disabled
 $Arguments:	menu tag, menu attribute tag, menu item tag
*******************************************************************************/
U16 set_pin_ch_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{
	T_MFW_SIM_PIN_STATUS status;

    TRACE_FUNCTION("MmiPins:set_pin_item_flag_state:pinChange");

	status.type = MFW_SIM_PIN1;
	sim_pin_status(&status);
	if(status.set EQ MFW_SIM_DISABLE) /*if PIN disabled no change PIN */
		return MNU_ITEM_HIDE;
	else
		return 0;		      /* PIN enabled, change PIN possible */
}


/*******************************************************************************

 $Function:	set_pin_en_item_flag
 $Description:	decision about show entry "PIN enable"
 $Returns:		Status int; 0 if show, 1 if don't show
 $Arguments:	menu tag, menu attribute tag, menu item tag
*******************************************************************************/
U16 set_pin_en_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{
	T_MFW_SIM_PIN_STATUS status;

    TRACE_FUNCTION("MmiPins:set_pin_en_item_flag:pinEnable");

	status.type = MFW_SIM_PIN1;
	sim_pin_status(&status);
	if(status.set EQ MFW_SIM_ENABLE) /*if PIN enabled no show menu entry */
		return MNU_ITEM_HIDE;
	else
		return 0;		      /* PIN disabled show menu entry */
}


/*******************************************************************************

 $Function:	set_pin_dis_item_flag 
 $Description:	decision about show entry "PIN enable"
 $Returns:		Status int; 0 if show, 1 if don't show
 $Arguments:	menu tag, menu attribute tag, menu item tag
*******************************************************************************/
U16 set_pin_dis_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{
	T_MFW_SIM_PIN_STATUS status;

	TRACE_FUNCTION("MmiPins:set_pin_en_item_flag:pinDisable");

	status.type = MFW_SIM_PIN1;
	sim_pin_status(&status);
	if(status.set EQ MFW_SIM_DISABLE) /*if PIN disabled no show menu entry */
		return MNU_ITEM_HIDE;
	else
		return 0;		      /* PIN enabled show menu entry */
}


/*******************************************************************************

 $Function:	set_pin2_ch_item_flag
 $Description:	decision about show entry "PIN2 change"
 $Returns:		Status int; 0 if show, 1 if don't show
 $Arguments:	menu tag, menu attribute tag, menu item tag
*******************************************************************************/
//change back to old version
U16 set_pin2_ch_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{
			T_MFW_SIM_PIN_STATUS status;

    TRACE_FUNCTION("MmiPins:set_pin2_ch_item_flag:pin2Change");

	status.type = MFW_SIM_PIN2;
	sim_pin_status(&status);
	/*if(status.set EQ MFW_SIM_DISABLE)*/ /*if PIN disabled no change PIN */
	if(status.set EQ MFW_SIM_UNKNOWN)
		return MNU_ITEM_HIDE;
	else
		return 0;		      /* PIN enabled, change PIN possible */
}

#ifdef SIM_PERS
/*******************************************************************************
 $Function:		mmi_simlock_change_pin
 $Description:	Performs the change password for a given lock type
 $Returns:		success or failure
 $Arguments:	lock type, oldpassword, new password
*******************************************************************************/

void mmi_simlock_change_pin(T_MFW_HND win,int type,char* oldpsw,char* newpsw)
{
    T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
    T_pin * pin_data = (T_pin *)win_data->user;

    TRACE_EVENT_P2(" old paswwrd%s new password%s ",oldpsw,newpsw);
     if(mfw_simlock_change_lock_code(type,oldpsw,newpsw) !=MFW_SS_OK)
       SEND_EVENT(pin_data->pin_win,SIM_LOCK_NEW_ENDISFAIL,0,pin_data);
    else
        SEND_EVENT(pin_data->pin_win,SIM_LOCK_NEW_ENDSUCC,0,pin_data);
}
#endif

/*******************************************************************************

 $Function:	setting_pin_main 
 $Description:	PIN Settings Dialog Handling function
 $Returns:		none
 $Arguments:	win, event, value, parameters
*******************************************************************************/
void setting_pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;//pin main data
//	T_pin_mess * mess_data = (T_pin_mess *) parameter;//pin message data   /* Warning Correction */
//	T_pin_edt * edt_data = (T_pin_edt *) parameter;//pin editor data   /* Warning Correction */
	T_MFW_WIN * par_window;


	 switch(event)
	 {
		  case PIN1_REQ_OLD:					  // Text "Enter Old Pin "
		  case PIN2_REQ_OLD:					  // Text "Enter Old Pin2 "
		  case PIN1_REQ_ATT:					  // info screen "xx attempts "
		  case PIN2_REQ_ATT:					  // info screen "xx attempts "
		  case PUK1_REQ:					  // Text "Enter Puk "
		  case PIN1_REQ:					  // Text "Enter Pin "
		  case PUK2_REQ:					  // Text "Enter Puk2 "
		  case PIN2_REQ:					  // Text "Enter Pin2 "
		  case PIN1_STATUS:					  // Text "Pin enabled"or disabled
		  case INPUT_NEW_PIN:					  // Text "Enter New PIN "
		  case INPUT_NEW_PIN2:					  // Text "Enter New PIN2 "
		  case INPUT_NEW_PIN_AGAIN:				  // Text "Enter New PIN again "
		  case INPUT_NEW_PIN2_AGAIN:				  // Text "Enter New PIN2 again "
		  case GSM_FAIL:					  // gsm not allowed
		  case NEW_PIN_FAIL:					  // new pin2 failed
		  case NEW_PIN2_FAIL:					  // new pin failed
		  case PUK1_REQ_VER_INFO:				  // Text "Pin blocked "
		  case PUK1_UNBL_FAIL_INFO:				  // unblock error - new puk requ
		  case PUK1_UNBL_FATAL_ERROR:
		  case PIN1_CH_SUCC:
		  case PIN2_CH_SUCC:
		  case PIN1_ENAB_END:
		  case PIN1_DISAB_END:
          case PIN2_SIM_FAILURE:    // Marcus: Issue 1609: 23/01/2003 - failure to verify PIN2
		  
		  case FDN_ACTIVATED:
		  case ADN_ACTIVATED:
		  case FDN_FAIL:
		  case FDN_ACT_END:
		  case FDN_DEACT_END:
#ifdef SIM_PERS              
//The different cases of enabling/disabling/quering and changing password
//scenarios are handled		
case SIM_LOCK_NEW_ENDSUCC_CKEY:
    case INPUT_NEW_SIM_PLOCK_AGAIN:	
          case SIM_LOCK_REQ_NEW:
          case TXT_SIM_LOCK_ALL:
          case SIM_LOCK_ENDSUCC:
          case SIM_LOCK_ALREADY_ENDIS:
          case SIM_LOCK_REQ_FCPWD:
          case SIM_LOCK_FCPWD_FAIL:
          case SIM_LOCK_FCPWDSUCC:
          case SIM_LOCK_REQ_OLD:
          case SIM_LOCK_STATUS:
          case NEW_SIM_PLOCK_FAIL:
          case SIM_LOCK_NEW_ENDSUCC:
          case SIM_LOCK_NEW_ENDISFAIL:
          case SIM_LOCK_SIM_REM:
	   case PERM_BLK:
	   case SMLK_SHOW_FC_FAIL1:
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	case SIM_LOCK_REQ_MKPWD:	//Master unlock ket request
	case SIM_LOCK_MKPWD_FAIL:	//Master unlocking failed
	case SIM_LOCK_MKPWDSUCC:	//Master unlocking success
#endif
		  sett_pin_mess(win,event);
				  break;
		  case PUK1_UNBL_SUCC: 
					sett_pin_mess(win,event);
				break;
			
	 }

	 switch(event)
	 {
		  case TXT_ENTER_PIN1:
				   pin_data->pin_case = INPUT_PIN1;//enter PIN1 start
				   sett_pin_edit(win,INPUT_PIN1);
				  break;
  #ifdef SIM_PERS
//events to handle input of new password, password and new password again
                 case TXT_ENTER_PLOCK:
                    		pin_data->pin_case = INPUT_SIM_PLOCK;//enter PIN1 start
				sett_pin_edit(win,INPUT_SIM_PLOCK);
                            break;

                  case TXT_ENTER_FCPWD:
                    		pin_data->pin_case = INPUT_SIM_FCPWD;//enter FC pwd start
				sett_pin_edit(win,INPUT_SIM_FCPWD);
                            break;
//x0pleel 20 July, 2006
                  case TXT_ENTER_MKPWD:
                    		pin_data->pin_case = INPUT_SIM_MKPWD;//enter FC pwd start
				sett_pin_edit(win,INPUT_SIM_MKPWD);
                            break;

                   case TXT_ENTER_NEW_SIMLOCK:
                               pin_data->pin_case = INPUT_NEW_SIM_PLOCK;//enter PIN1 start
				   sett_pin_edit(win,INPUT_NEW_SIM_PLOCK);
                                break;  

                      case TXT_ENTER_NEW_SIMLOCK_AGAIN:
                               pin_data->pin_case = INPUT_NEW_SIM_PLOCK_AGAIN;//enter new PIN2 start
				   sett_pin_edit(win,INPUT_NEW_SIM_PLOCK_AGAIN);
				   break;           
                        case NEW_SIM_PLOCK_END:
                              if(!pin_data->puk_request)
                                {
                                    mmi_simlock_change_pin(win,mmi_simlock_locktype,(char*)pin_data->old_pin,(char*)pin_data->new_pin);
                                }
                              break; 

#endif
		  case TXT_ENTER_PIN2:
				   pin_data->pin_case = INPUT_PIN2;//enter PIN2 start
				   sett_pin_edit(win,INPUT_PIN2);
				  break;
#ifdef SIM_PERS
		case ME_PERM_BLK:
				   TRACE_FUNCTION("ME_PERM_BLK");
				   pin_data->pin_case = PERM_BLK1;//enter PUK1 start
				   sett_pin_edit(win,PERM_BLK1);
				  break;
#endif
		  case TXT_ENTER_PUK1:
		  	TRACE_FUNCTION("TXT_ENTER_PUK1");
				   pin_data->pin_case = INPUT_PUK1;//enter PUK1 start
				   sett_pin_edit(win,INPUT_PUK1);
				  break;
		  case TXT_ENTER_PUK2:
				   pin_data->pin_case = INPUT_PUK2;//enter PUK2 start
				   sett_pin_edit(win,INPUT_PUK2);
				  break;
		  case TXT_ENTER_NEW_PIN:
				   pin_data->pin_case = INPUT_NEW_PIN;//enter new PIN start
				   sett_pin_edit(win,INPUT_NEW_PIN);
				   break;
		  case TXT_ENTER_NEW_PIN2:
				   pin_data->pin_case = INPUT_NEW_PIN2;//enter new PIN2 start
				   sett_pin_edit(win,INPUT_NEW_PIN2);
				   break;
		  case TXT_ENTER_NEW_PIN_AGAIN:
				   pin_data->pin_case = INPUT_NEW_PIN_AGAIN;
				   sett_pin_edit(win,INPUT_NEW_PIN_AGAIN);// Start: enter New PIN1 again
				   break;
		  case TXT_ENTER_NEW_PIN_AGAIN2:
				   pin_data->pin_case = INPUT_NEW_PIN2_AGAIN;
				   sett_pin_edit(win,INPUT_NEW_PIN2_AGAIN);// Start: enter New PIN2 again
				   break;
		  case PIN1_END:	
				   pin1_en_dis_verify(win);	
				   break;
				   
#ifdef SIM_PERS                   
//Events that indicate the input of the lock code and new lock code
                case SIM_PLOCK_END:
                               mmi_simlock_en_dis_verify(win);
                               break;

                 case SIM_FCPWD_END:
                 		mmi_simlock_reset_fc_value(win);
                 		break;

			//x0pleela 20 july, 2006 ER: OMAPS00087586, OMAPS00087587
             	   case SIM_MKPWD_END:
             	   		//Master unlocking through SIMP menu
                 		mmi_simlock_master_unlock(win,  MFW_SIM_MKEYM);
                 		break;
#endif                               
		  case PIN2_END:
				   sim_verify_pin(MFW_SIM_PIN2,(char*)pin_data->old_pin);/* verification pin2 */
				   break;
		  case NEW_PIN_END:
				   if(!pin_data->puk_request)
						sim_change_pin(MFW_SIM_PIN1,(U8*)pin_data->old_pin,(U8*)pin_data->new_pin);/* change PIN1 */
				   else
					{
						sim_unblock_pin(MFW_SIM_PUK1, (char*)pin_data->puk, (char*)pin_data->new_pin);/* unblock pin1 */
					}
				   break;
		  case NEW_PIN2_END:
			   if(!pin_data->puk_request)
						sim_change_pin(MFW_SIM_PIN2,(U8*)pin_data->old_pin,(U8*)pin_data->new_pin);/* change PIN2 */
				   else
						sim_unblock_pin(MFW_SIM_PUK2, (char*)pin_data->puk, (char*)pin_data->new_pin);/* unblock pin2 */
				   break;
		  case PIN_OK_INFO: // PIN was need
					if((pin_data->set_state NEQ FDN_ACTIV) AND (pin_data->set_state NEQ FDN_DEACTIV))
					{
						sett_pin_mess(win,PIN_OK_INFO);// for pin2 change and pin2 check from serv/phoneb
					
					}
					else
					{
						set_mode_fdn_adn(win,pin_data);
					}
				   break;
	 }

	 switch(event)
	 {
		  case PIN_OK_END:
				pin_verif_or_check2_end(win);
				break;
		  case PIN1_CH_END:
		  case PIN2_CH_END:
		  case UNBL_OK:
				pin_ch_end_or_abort(win);
			   break;
			
		  case PIN1_STATUS_END: 		  
		  case IDLE_GSM_FAIL:
				   pin_ch_end_or_abort(win);
			   break;
		  case SETT_ABORT:
				TRACE_FUNCTION("clear softkey 10");		  
				// June 16, 2005  REF: CRR 31267  x0021334
				if (pin2Flag NEQ TRUE)  // To ensure that the screen does not blank out
				{
					dspl_ClearAll();
				}
				if(pin_data->set_state EQ PIN1_CHECK)
				{
					par_window = pin_data->parent_win;
					setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
				}
				else if(pin_data->set_state EQ PIN2_CHECK)
				{
					par_window = pin_data->parent_win;
					setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
					// June 16, 2005  REF: CRR 31267  x0021334
					if (pin2Flag EQ TRUE) // To ensure that failed dialog is not given
					{
						pin2Flag = FALSE;
						winDelete(par_window);
					}
					else
						SEND_EVENT(par_window, PIN2_ABORT, 0, NULL);
				}
				else
					setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
				break;
		  case FDN_UNBL_OK:
				TRACE_FUNCTION("clear softkey 11");		  
				setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
				break;
		  case PIN1_ENDIS_FAIL:
				  if(sim_pin_count(MFW_SIM_PIN1) EQ 0)
				  {
					   pin_data->puk_request = TRUE;
					   sett_pin_mess(win,PUK1_REQ_VER_INFO);
				  }
				  else
				  {
						sett_pin_mess(win,PIN1_ENDIS_FAIL);
				  }
				  break;
		  case FDN_WAIT:
			    pin_data->pin_case = FDN_WAIT;
				winShow(pin_data->pin_win);
				break;	// waiting for call back from phonebook handler
#ifdef SIM_PERS				

//event to handle lock/unlock failure			
		case SIM_LOCK_ENDIS_FAIL:
                            sett_pin_mess(win,SIM_LOCK_ENDIS_FAIL);
                            break;

		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//New event to display a dialog Busy for wrong entry of unlocking pwd and timer been enabled
		case SIM_LOCK_BUSY:
			       sett_pin_mess(win,SIM_LOCK_BUSY);
                            break;

		//x0pleela 26 July,2006 ER: OMAPS00087586, OMAPS00087587
		//new event to display a dialog Busy for wrong entry of unblocking pwd and timer been enabled
		case SIM_LOCK_BUSY_UNBLOCK:
			       sett_pin_mess(win, SIM_LOCK_BUSY_UNBLOCK);
                            break;
                 
#endif                            
		  default:
			  break;
	 }
}



/*******************************************************************************

 $Function:	pin_ch_end_or_abort 
 $Description:	PIN change finished or abort (settings)
 $Returns:		none
 $Arguments:	win
*******************************************************************************/
static void pin_ch_end_or_abort (T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	T_MFW_WIN * par_window;

	TRACE_FUNCTION("MmiPins:pin_ch_end_or_abort");

   if(pin_data->set_state NEQ PIN2_CHECK)
   {
		if((pin_data->set_state EQ FDN_ACTIV) ||(pin_data->set_state EQ FDN_DEACTIV))
		{
			set_mode_fdn_adn(win,pin_data);
		}
		else if(pin_data->set_state EQ PIN1_CHECK)
		{
			par_window = pin_data->parent_win;
			setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
			SEND_EVENT(par_window,PIN1_OK,0,NULL);// pin1 check over PUK1
		}	  
		else
		{
			TRACE_FUNCTION("clear softkey 12");		
			pin_skClear();
			setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog,also idle gsm
		}										/* back to menu tree	  */
   }
   else
   {
		par_window = pin_data->parent_win;
		setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
		SEND_EVENT(par_window,PIN2_OK,0,NULL);// pin2 check over PUK2
   }

}

#ifdef SIM_PERS
//	Nov 03, 2005 DR: OMAPS00052032 - xpradip
/*******************************************************************************
 $Function:		mmi_display_result
 $Description:	Displays appropriate info dialog for the sAT_PlusCLCK operation
 $Returns:		none
 $Arguments:	state for displaying enabled or failed
*******************************************************************************/
void mmi_display_result(U8 state)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)info_disp)->data;
	T_dialog_info * display_info = (T_dialog_info *)win_data->user;
	T_MFW_HND parent = display_info->parent_win;
	T_MFW_WIN * win_data1 = ((T_MFW_HDR *)parent)->data;
	T_pin * pin_data = (T_pin *)win_data1->user;

	if( info_disp)
		dialog_info_destroy(info_disp);
	info_disp = NULL;
	
	if( state)
	{
			SEND_EVENT(parent,SIM_LOCK_ENDSUCC,0,pin_data);
	}
	else
	{
	     if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_SimNotIns)
		     	 SEND_EVENT(parent,SIM_LOCK_SIM_REM,0,pin_data);
	//x0pleela 11 Sep, 2006  DR: OMASP00094215
	//checking for Class type and the ext busy error while unlocking a locked category
	else if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
			(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
 		   	SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY,0,pin_data);
	     else
		        SEND_EVENT(parent,SIM_LOCK_ENDIS_FAIL,0,pin_data);
	}
}

static void mmi_simlock_reset_fc_value(T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	int status,curr_fail_reset, curr_succ_reset;

	TRACE_FUNCTION("mmi_simlock_reset_fc_value()");
	status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
	if( curr_succ_reset )
	{
		status = mfw_simlock_reset_fc_value((U8*)pin_data->old_pin,MFW_SIM_FCM);
		mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

		if (status == MFW_SS_FAIL)
		{	
			if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_SimNotIns)
		     	 SEND_EVENT(pin_data->pin_win,SIM_LOCK_SIM_REM,0,pin_data);

				//x0pleela 11 Sep, 2006  DR: OMASP00094215
				//checking for Class type and the ext busy error while unlocking a locked category
			else if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
					(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
				{
				  SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY_UNBLOCK,0,pin_data);
				}

			else
		     	{
				if( curr_fail_reset )
					{
			     		SEND_EVENT(pin_data->pin_win,SIM_LOCK_FCPWD_FAIL,0,pin_data);
					}
				else
				{
					perm_blocked =1;
					SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
				}
		     	}
		}
         else
             SEND_EVENT(pin_data->pin_win,SIM_LOCK_FCPWDSUCC,0,pin_data);
	}
	else
	{
		perm_blocked =1;
		SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
	}	     
		
   }
/*******************************************************************************
 $Function:		mmi_simlock_en_dis_verify
 $Description:	performs the operation of enabling/disabling and verifying a given
 				lock type after the input of the password
 $Returns:		none
 $Arguments:	none
*******************************************************************************/

static void mmi_simlock_en_dis_verify(T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	T_MFW_SS_RETURN ret;
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_simlock_en_dis_verify()");
  if((pin_data->set_state != ENABLE) && (pin_data->set_state != DISABLE))
	{	
              
	       TRACE_FUNCTION("We're verifying SIMLock");
	     
              SEND_EVENT(pin_data->pin_win,SIM_LOCK_REQ_NEW,0,pin_data);
              
       }
  else if(pin_data->set_state EQ ENABLE)
   {	 
		TRACE_FUNCTION("MmiPins:Enable Lock now");
		//TRACE_FUNCTION_P1("Vidya: SimLock_MMI_Enable Password=%s",pin_data->old_pin);	
		ret = mfw_simlock_enable_lock((U8*)pin_data->old_pin, mmi_simlock_locktype);
		if ( ret == MFW_SS_FAIL)
		{	
		     if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_SimNotIns)
		     	 SEND_EVENT(pin_data->pin_win,SIM_LOCK_SIM_REM,0,pin_data);
		     else
		        SEND_EVENT(pin_data->pin_win,SIM_LOCK_ENDIS_FAIL,0,pin_data);
		}
//	Nov 03, 2005 DR: OMAPS00052032 - xpradip
//	handle the execute state for enabling the lock		
		else if( ret == MFW_EXCT)
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull,TxtPleaseWait,TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, FOREVER, KEY_CLEAR | KEY_LEFT );
			info_disp = info_dialog (pin_data->pin_win, &display_info);
		}
         	else
             		SEND_EVENT(pin_data->pin_win,SIM_LOCK_ENDSUCC,0,pin_data);
   }
   else  if(pin_data->set_state EQ DISABLE)
   { 	
   		ret = mfw_simlock_disable_lock((U8*)pin_data->old_pin,mmi_simlock_locktype); 
		if (ret == MFW_SS_FAIL)
		{	
		      if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_SimNotIns)
		     	 SEND_EVENT(pin_data->pin_win,SIM_LOCK_SIM_REM,0,pin_data);

				//x0pleela 11 Sep, 2006  DR: OMASP00094215
				//checking for Class type and the ext busy error while unlocking a locked category
			else if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
					(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
			  SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY,0,pin_data);
		     else
  			{
  			pin_data->pin_retries = sim_pin_count(MFW_SIM_PIN1);
			SEND_EVENT(pin_data->pin_win,SIM_LOCK_ENDIS_FAIL,0,pin_data);
		     	}
		}
        	else
	       		SEND_EVENT(pin_data->pin_win,SIM_LOCK_ENDSUCC,0,pin_data);
   }

}


/*******************************************************************************
 $Function:		simLock_enable
 $Description:	Initiates the password input screen for enabling a lock	none
 $Arguments:	none
*******************************************************************************/
void SIMLock_enable ()
{
    T_MFW_HND idle_win = mfwParent( mfw_header() );
    T_MFW_HND win = setting_pin_create(idle_win);
    UBYTE slock_status;
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;

    if(win != NULL) /* x0039928 -Lint warning removal */
    {
    	win_data = ((T_MFW_HDR *)win)->data;

    pin_data = (T_pin *)win_data->user;
	
    /*a0393213 warnings removal-status variable set but not used*/	
    /*T_MFW_SIM_PIN_STATUS status; 

    status.type = MFW_SIM_PLOCK;*/
    slock_status= mfw_simlock_get_lock_status(mmi_simlock_locktype);
    pin_data->set_state = ENABLE;

    if(slock_status EQ MFW_SIM_DISABLE) /*if PIN disabled*/
    {	
        SEND_EVENT(pin_data->pin_win,TXT_SIM_LOCK_ALL,0,pin_data);	
     }
    else if (slock_status EQ MFW_SIM_BLOCKED)
    {
    	pin_data->pin_case = SIM_LOCK_ALL;
    	SEND_EVENT(pin_data->pin_win, SIM_LOCK_ENDIS_FAIL,0,pin_data);
     }
    else 
        SEND_EVENT(pin_data->pin_win,SIM_LOCK_ALREADY_ENDIS,0,pin_data);
    	}
}

/*******************************************************************************

 $Function:		SIMLock_disable
 $Description:	Initiates the password input screen for the disabling of a lock
 $Returns:		
 $Arguments:	
*******************************************************************************/
void SIMLock_disable ()
{
		T_MFW_HND idle_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(idle_win);
        	UBYTE slock_status;
		T_MFW_WIN * win_data = NULL;
  //     	T_DISPLAY_DATA display_info;
		T_pin * pin_data = NULL;
		/*T_MFW_SIM_PIN_STATUS status;*//*a0393213 warnings removal-status variable set but not used*/

            TRACE_FUNCTION("SIMLock_disable()");    

		if(win != NULL) /* x0039928 - Lint warning removal */
  		{	win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
		
		/*status.type = MFW_SIM_PLOCK;*/ /*a0393213 warnings removal-status variable set but not used*/
		slock_status= mfw_simlock_get_lock_status(mmi_simlock_locktype);
		pin_data->set_state = DISABLE;
		if(slock_status EQ MFW_SIM_ENABLE) /*if PIN disabled*/
		{	
			SEND_EVENT(pin_data->pin_win,TXT_SIM_LOCK_ALL,0,pin_data);	
		}
		else if (slock_status EQ MFW_SIM_BLOCKED)
	    {
	    	pin_data->pin_case = SIM_LOCK_ALL;
	    	SEND_EVENT(pin_data->pin_win, SIM_LOCK_ENDIS_FAIL,0,pin_data);
		}
		else //PIN already enabled
		/*{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtLockDisabled, TxtNull,  COLOUR_STATUS_PINS);
          	//display_info.TextId = TxtLockEnabled;
          	dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, 0 );
          	info_dialog(0,&display_info);
		}*/
			SEND_EVENT(pin_data->pin_win,SIM_LOCK_ALREADY_ENDIS,0,pin_data);
		}
		return ;
}


/*******************************************************************************
 $Function:		SIMLock_change
 $Description:	Initiates the password input screen for change password
 $Arguments:	none
*******************************************************************************/

 void SIMLock_change ()
{
    T_MFW_HND idle_win = mfwParent( mfw_header());
    T_MFW_HND win = setting_pin_create(idle_win);
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;
    int status;
   
	if(win != NULL)	/* x0039928 - Lint warning removal */
	{
	win_data = ((T_MFW_HDR *)win)->data;
	pin_data = (T_pin *)win_data->user;
	pin_data->set_state = CHANGE_PIN;
	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	
	TRACE_FUNCTION("MmiPins:SIMLock_change");
	
	if(status == MFW_SIM_BLOCKED)
	{
		pin_data->pin_case = SIM_LOCK_ALL;
		SEND_EVENT(pin_data->pin_win,SIM_LOCK_ENDIS_FAIL,0,pin_data);
	}
	else if( status == MFW_SIM_ENABLE){
		SEND_EVENT(pin_data->pin_win,SIM_LOCK_STATUS,0,pin_data);
		}
	else
    {
      /*a0393213 warnings removal - enumerated typed mixed with other type - the statement is not necessary*/
    	/*pin_data->pin_case = DISABLE;*/
    	SEND_EVENT(pin_data->pin_win,SIM_LOCK_REQ_OLD,0,pin_data);
    }
	}
}

/*******************************************************************************

 $Function:		SIMLock_status  
 $Description:	Initiates the query for the lock status.
 $Returns:		
 $Arguments:none
*******************************************************************************/
void SIMLock_status ()
{

		T_MFW_HND idle_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;

	if(win != NULL) /* x0039928 - Lint warning removal */
	{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;


	    TRACE_FUNCTION("SIMLock_status");

	    SEND_EVENT(pin_data->pin_win,SIM_LOCK_STATUS,0,pin_data);
	}
}


/*******************************************************************************

 $Function:    	simlock_doAction

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void simlock_doAction()
{
TRACE_FUNCTION("simlock_doAction()");
	switch(mmi_simlock_lockaction)
		{
		case MFW_SIM_ENABLE:
			SIMLock_enable();
			break;

		case MFW_SIM_DISABLE:
			SIMLock_disable();
			break;

		case MFW_SIM_CHANGE:
			SIMLock_change();
			break;
		case MFW_SIM_VERIFY:
			SIMLock_status();
			break;
			
		default:
			break;
		}
}

int SIMLock_resetFC (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
		T_MFW_HND idle_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL) /* x0039928 -Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;

            	TRACE_FUNCTION("SIMLock_resetFC()");    
            	
	    	SEND_EVENT(pin_data->pin_win, SIM_LOCK_REQ_FCPWD,0,pin_data);
		}
		return 1;
}

/*******************************************************************************

 $Function:    	mmi_simlock_category_menus()

 $Description:	starts the simlock category list menu
 
 $Returns:		None

 $Arguments:	parent window
 
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
T_MFW_HND mmi_simlock_category_menus(T_MFW_HND parent)
{	
	T_MFW_HND simp_cat_list_win;
	TRACE_FUNCTION("mmi_simlock_category_menus");
	
	simp_cat_list_win = bookMenuStart(parent, SIMPCategoryListAttributes(), NULL);
	return  simp_cat_list_win;

}

/*******************************************************************************
 $Function:		SIMLock_unblock_ME
 $Description:	       This function sends an event to display an editor for the user to enter
 				unblocking code
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
int SIMLock_unblock_ME(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
  T_MFW_HND 		pin_win = pin_windows;
  T_MFW_WIN * win_data = ((T_MFW_HDR *)pin_win)->data;
  T_pin * pin_data = (T_pin *)win_data->user; // pin main data

  TRACE_FUNCTION("SIMLock_unblock_ME()");
  
  pin_data->puk_request = TRUE;
  C_KEY_REQ=1;
  FCUnlock_flag=1;
  SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
  return 1;
}

/*******************************************************************************
 $Function:		SIMLock_MasterKeyUnlock_bootup
 $Description:	       This functioh gets called to unlock category /unblock ME during bootup sequence
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
int SIMLock_MasterKeyUnlock_bootup (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
  T_MFW_HND 		pin_win = pin_windows;
  T_MFW_WIN * win_data = ((T_MFW_HDR *)pin_win)->data;
  T_pin * pin_data = (T_pin *)win_data->user; // pin main data
  
  TRACE_FUNCTION("SIMLock_MasterKeyUnlock_bootup()");    
  
  SEND_EVENT(pin_data->pin_win, INPUT_MASTER_KEY_REQ,0,pin_data);
  
  return 1;
}

/*******************************************************************************
 $Function:		SIMLock_MasterKeyUnlock
 $Description:	       This functioh gets called to unlock category /unblock ME through SIMP menu
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
int SIMLock_MasterKeyUnlock (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
  T_MFW_HND idle_win = mfwParent( mfw_header() );
  T_MFW_HND win = setting_pin_create(idle_win);
  T_MFW_WIN * win_data = NULL;
  T_pin * pin_data = NULL;

  if(win != NULL) /* x0039928 - Lint warning removal */
  {
  win_data = ((T_MFW_HDR *)win)->data;
  pin_data = (T_pin *)win_data->user;
  
  TRACE_FUNCTION("SIMLock_MasterKeyUnlock()");    
  SEND_EVENT(pin_data->pin_win, SIM_LOCK_REQ_MKPWD,0,pin_data);
  }
  return 1;
}

/*******************************************************************************

 $Function:    	simlock_check_masterkey

 $Description:   If there is no support for Master Unlocking the item "Master unlock" will not 
 				appear on the menulist

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_masterkey(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	UBYTE status;
	TRACE_FUNCTION("simlock_check_masterkey");

	status = mfw_simlock_check_masterkey(MFW_SIM_MKEYM);
	if( status EQ MFW_MASTER_UNLOCK_ACTIVE)
	  return 0;
	else
	  return 1;
}

/*******************************************************************************

 $Function:    	simlock_get_masterkey_status

 $Description:   Get the master key status

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
UBYTE simlock_get_masterkey_status(void)

{
	UBYTE status;
	TRACE_FUNCTION("simlock_get_masterkey_status");

	status = mfw_simlock_check_masterkey(MFW_SIM_MKEYM);
	return status;	
}

/*******************************************************************************

 $Function:    	simlock_check_NWCatLock

 $Description:   Get the lock status of Network category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_NWCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_NWCatLock");

	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_NLOCK ) && ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;	
	
}

/*******************************************************************************

 $Function:    	simlock_check_NSCatLock

 $Description:   Get the lock status of Network subset category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_NSCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_NSCatLock");


	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_NSLOCK )&& ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;	
}

/*******************************************************************************

 $Function:    	simlock_check_SPCatLock

 $Description:   Get the lock status of Service Provider category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_SPCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_SPCatLock");

	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_SPLOCK )&& ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;		
}

/*******************************************************************************

 $Function:    	simlock_check_CPCatLock

 $Description:   Get the lock status of Corporate category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_CPCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_CPCatLock");

	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_CLOCK )&& ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;		
}

/*******************************************************************************

 $Function:    	simlock_check_SIMCatLock

 $Description:   Get the lock status of SIM category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_SIMCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_SIMCatLock");

	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_PLOCK )&& ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;		
}

/*******************************************************************************

 $Function:    	simlock_check_CatBlock

 $Description:   Get the ME status and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
USHORT	simlock_check_CatBlock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)
{
	T_SUP_INFO_TYPE FLAG;
	int curr_fc;

	TRACE_FUNCTION("simlock_check_CatBlock");

 	FLAG=FCATTEMPTSLEFT;          /* set flag to current */
     	curr_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/	
	if((curr_fc == 0)) //ME blocked
		return 0;
	else
		return 1;
}
/*******************************************************************************

 $Function:    	mmi_simlock_master_unlock

 $Description:   This function unlocks category/unblocks ME and sends event accordingly through SIMP menu

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
static void mmi_simlock_master_unlock(T_MFW_HND win, UBYTE MK_Unblock_flag)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	int status;

	TRACE_FUNCTION("mmi_simlock_master_unlock()");

	status = mfw_simlock_master_unlock((U8*)pin_data->old_pin, MK_Unblock_flag);
	if (status == MFW_SS_FAIL)
		{
		SEND_EVENT(pin_data->pin_win,SIM_LOCK_MKPWD_FAIL,0,pin_data);			
		}
       else
       	{
      	       SEND_EVENT(pin_data->pin_win,SIM_LOCK_MKPWDSUCC,0,pin_data);
       	}
}

/*******************************************************************************

 $Function:    	simlock_check_PBCatLock

 $Description:   Get the lock status of Blocked Network category and accordingly display the menu item

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
USHORT	simlock_check_PBCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi)

{
	int status;
	
	TRACE_FUNCTION("simlock_check_PBCatLock");

	status = mfw_simlock_get_lock_status(mmi_simlock_locktype);
	if( (mmi_simlock_locktype EQ MFW_SIM_PBLOCK ) && ( status NEQ MFW_SIM_BLOCKED) )
		return 0;
	else
		return 1;	
	
}

/*******************************************************************************

 $Function:    	simp_unlock_Category

 $Description:   This function sends corresponding event to unlock a locked category through dynamic menu

 $Returns:

 $Arguments:

*******************************************************************************/
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
int simp_unlock_Category(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
	T_MFW_HND 		pin_win = pin_windows;
	T_MFW_WIN * win_data = ((T_MFW_HDR *)pin_win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	
	TRACE_FUNCTION("simp_unlock_Category()");
	switch((int)i->str)
	{
	case TxtNetwork:
		SEND_EVENT(pin_win,INPUT_NLOCK_PIN_REQ, 0, pin_data);
		break;
	case TxtNetworkSubset:
		SEND_EVENT(pin_win,INPUT_NSLOCK_PIN_REQ, 0, pin_data);
		break;
	case TxtServiceProvider:
		SEND_EVENT(pin_win,INPUT_SPLOCK_PIN_REQ, 0, pin_data);
		break;
	case TxtCorporate:
		SEND_EVENT(pin_win,INPUT_CLOCK_PIN_REQ, 0, pin_data);
		break;
	case TxtSIM:
		SEND_EVENT(pin_win,INPUT_SIM_PIN_REQ, 0, pin_data);
		break;		

	//x0pleela 25 Sep, 2006 ER: OMAPS00095524
	//Handling "Blocked Network" category
	case TxtBlkNetwork:
		SEND_EVENT(pin_win,INPUT_PBLOCK_PIN_REQ, 0, pin_data);
		break;	
	}	
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
 $Function:		get_fc_max_value
 $Description:	       gets the maximum value of FC
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=FCMAX;          /* set flag to current */

     TRACE_FUNCTION("get_fc_max_value");
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     sprintf(buf, "MAX %d tries", max_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}

/*******************************************************************************
 $Function:		get_fc_attempts_left
 $Description:	       gets the current value of FC
 $Returns:		none 
 $Arguments:	       none
*******************************************************************************/
int get_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int curr_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	new ENUM used to adopt to the aci label
     FLAG = FCATTEMPTSLEFT;          /* set flag to current */
     
     curr_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(curr_fc != MFW_FAILURE)
     	{
     sprintf(buf, "%d tries left", curr_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}



/*******************************************************************************
 $Function:		get_fail_reset_fc_max_value
 $Description:	       gets the maximum value of FC
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_fail_reset_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=FCRESETFAILMAX;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     sprintf(buf, "MAX %d tries", max_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}

/*******************************************************************************
 $Function:		get_succ_reset_fc_max_value
 $Description:	       gets the maximum value of FC
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_succ_reset_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=FCRESETSUCCESSMAX;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     sprintf(buf, "MAX %d tries", max_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}
/*******************************************************************************
 $Function:		get_fail_reset_fc_attempts_left
 $Description:	       gets the maximum value of FC
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_fail_reset_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	new ENUM used to adopt to the aci label	 
     FLAG = FCRESETFAILATTEMPTSLEFT;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     sprintf(buf, "%d tries left", max_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}
/*******************************************************************************
 $Function:		get_succ_reset_fc_attempts_left
 $Description:	       gets the maximum value of FC
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_succ_reset_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
//	Sept 24, 2005 REF:  LOCOSTO-ENH-34438, xpradipg
//	new ENUM used to adopt to the aci label	 
     FLAG=FCRESETSUCCESSATTEMPTSLEFT;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     sprintf(buf, "%d tries left", max_fc);
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}
/*******************************************************************************
 $Function:		get_timer_flag_value
 $Description:	       gets the value of timer for successive unlock attempts
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_timer_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=TIMERFLAG;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     	if( max_fc )
	     sprintf(buf, "Flag Enabled");
     	else
     		sprintf(buf,"Flag Disabled");
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}

/*******************************************************************************
 $Function:		get_airtel_ind_flag_value
 $Description:	       gets the value of airtel indication flag
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_airtel_ind_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=AIRTELINDFLAG;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     	if( max_fc )
	     sprintf(buf, "Flag Enabled");
     	else
     		sprintf(buf,"Flag Disabled");
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}

/*******************************************************************************
 $Function:		get_etsi_flag_value
 $Description:	       gets the value of ETSI flag
 $Returns:		none
 $Arguments:	       none
*******************************************************************************/
int get_etsi_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
   
    T_SUP_INFO_TYPE FLAG;
     int max_fc;
     T_MFW_HND       win  = mfwParent(mfw_header());
     T_DISPLAY_DATA display_info;
     char buf[30];
     FLAG=ETSIFLAG;          /* set flag to current */
     
     max_fc=mfw_simlock_get_sup_info(FLAG); /* get the value of the corresponding flag*/
     
     if(max_fc != MFW_FAILURE)
     	{
     	if( max_fc )
	     sprintf(buf, "Flag Enabled");
     	else
     		sprintf(buf,"Flag Disabled");
     dlg_initDisplayData_TextStr( &display_info, TxtNull,  TxtNull, (char *)buf, (char *)NULL, COLOUR_STATUS);
     	}
     else
     dlg_initDisplayData_TextId( &display_info, TxtNull,  TxtNull, TxtFailed, TxtNull, COLOUR_STATUS);
     
     dlg_initDisplayData_events( &display_info, NULL, TWO_SECS, KEY_OK );
    // display_info.TextString2   = NULL; 
     display_info.Identifier   = NULL;
      info_dialog(win,&display_info); //information screen
      
      return 1;
   
}

/*******************************************************************************
 $Function:		setLockTypeNw
 $Description:	sets the global flag to the network lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int setLockTypeNw (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypeNw"); 
	mmi_simlock_locktype=MFW_SIM_NLOCK;
	simlock_doAction();
	return 1;
}
/*******************************************************************************
 $Function:		setLockTypeSp
 $Description:	sets the global flag to the Service provider lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int setLockTypeSp (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypeNw");
	mmi_simlock_locktype=MFW_SIM_SPLOCK;
	simlock_doAction();
	return 1;
}
/*******************************************************************************
 $Function:		setLockTypeNs
 $Description:	sets the global flag to the network subset lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int setLockTypeNs (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypeNs");
	mmi_simlock_locktype=MFW_SIM_NSLOCK;
	simlock_doAction();
	return 1;
}
/*******************************************************************************
 $Function:		setLockTypeCp
 $Description:	sets the global flag to the Corprate lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int setLockTypeCp (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypeCp");
	mmi_simlock_locktype=MFW_SIM_CLOCK;
	simlock_doAction();
	return 1;
}
/*******************************************************************************
 $Function:		setLockTypePs
 $Description:	sets the global flag to the Personalisation lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int setLockTypePs(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypePs");
	mmi_simlock_locktype=MFW_SIM_PLOCK;
	simlock_doAction();
	return 1;
}


/*******************************************************************************
 $Function:		setLockTypePb
 $Description:		sets the global flag to the Personalisation lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
int setLockTypePb(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
       TRACE_FUNCTION("setLockTypePb");
	mmi_simlock_locktype=MFW_SIM_PBLOCK;
	simlock_doAction();
	return 1;
}
/*******************************************************************************

 $Function:    	setLockActionLock

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void setLockActionLock(void)
{
    TRACE_FUNCTION("setLockActionLock");
    mmi_simlock_lockaction= MFW_SIM_ENABLE;
}


/*******************************************************************************

 $Function:    	setLockActionUnlock

 $Description:	sets the global flag to the action unlock
            
 $Returns:		none

 $Arguments:	none
 
*******************************************************************************/


void setLockActionUnlock(void)
{
    TRACE_FUNCTION("setLockActionUnlock");
    mmi_simlock_lockaction= MFW_SIM_DISABLE;
}
/*******************************************************************************

 $Function:    	setLockActionChPwd

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void setLockActionChPwd(void)
{
    TRACE_FUNCTION("setLockActionChPwd");
    mmi_simlock_lockaction= MFW_SIM_CHANGE;
}

/*******************************************************************************

 $Function:    	setLockActionChkStat

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void setLockActionChkStat(void)
{
    TRACE_FUNCTION("setLockActionChkStat");
    mmi_simlock_lockaction= MFW_SIM_VERIFY;
}

#endif



/*******************************************************************************

 $Function:	pin_verif_or_check2_end
 $Description:	PIN change finished or abort (settings)
 $Returns:		none
 $Arguments:	win
****************************************************************************/
static void pin_verif_or_check2_end(T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	T_MFW_WIN * par_window;

	TRACE_FUNCTION("MmiPins:pin_verif_or_check2_end");

  if(pin_data->set_state NEQ PIN2_CHECK)
   {
		if(pin_data->set_state NEQ CHANGE_PIN2)
		{
			TRACE_FUNCTION("MmiPins:changing PIN 2");
			SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN,0,pin_data);//pin change
		}
		else if(pin_data->set_state EQ PIN1_CHECK)
		{
			par_window = pin_data->parent_win;
			setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
			SEND_EVENT(par_window,PIN1_OK,0,NULL);// pin1 check (not over PUK1)
		}
		else
		{
			TRACE_FUNCTION("MmiPins:changing PIN 2");
			SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN2,0,pin_data);// pin2 change
		}

   }
   else
   {	TRACE_FUNCTION("MmiPins:checking PIN 2");
		par_window = pin_data->parent_win;
		setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
		SEND_EVENT(par_window,PIN2_OK,0,NULL);// pin2 check (not over PUK2)
   }

}




/*******************************************************************************

 $Function:	pin1_en_dis_verify 
 $Description:	PIN enab/disab/verify start
 $Returns:		none
 $Arguments:	win
****************************************************************************/
static void pin1_en_dis_verify(T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins:pin1_en_dis_verify");

   if((pin_data->set_state NEQ ENABLE) AND (pin_data->set_state NEQ DISABLE))
	{	TRACE_FUNCTION("We're verifying pin 1");
		sim_verify_pin(MFW_SIM_PIN1,(char*)pin_data->old_pin);/* verification pin1 */
	}
   else if(pin_data->set_state EQ ENABLE)
   {	/* start enabling PIN1 */ 
		TRACE_FUNCTION("MmiPins:Pin1 now enabled");   
		if (sim_enable_pin((U8*)pin_data->old_pin) == MFW_SS_FAIL)
		{	/*SPR 2145, if attempt fails then behave as though a "Pin incorrect"
			event has been sent to MMI*/
 			 pin_data->pin_retries = sim_pin_count(MFW_SIM_PIN1);
		     SEND_EVENT(pin_data->pin_win,PIN1_ENDIS_FAIL,0,pin_data);
		}
			
   }
   else 
   { 	/* start disabling PIN1 */
		TRACE_FUNCTION("MmiPins:Pin1 now disabled");   
		if (sim_disable_pin((U8*)pin_data->old_pin) == MFW_SS_FAIL)
		{	/*SPR 2145, if attempt fails then behave as though a "Pin incorrect"
			event has been sent to MMI*/
  			pin_data->pin_retries = sim_pin_count(MFW_SIM_PIN1);
			SEND_EVENT(pin_data->pin_win,PIN1_ENDIS_FAIL,0,pin_data);
		}
   }

}

/*******************************************************************************

 $Function:	pin_change 
 $Description:	PIN change : call from menu Settings
 $Returns:		Status int
 $Arguments:	menu, item
*******************************************************************************/

int pin_change (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
		T_MFW_HND idle_win = idle_get_window();
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		T_MFW_SIM_PIN_STATUS status;
			
		if(win != NULL) /* x0039928 - Lint warning removal */
		{	win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;

		pin_data->set_state = CHANGE_PIN;

	TRACE_FUNCTION("MmiPins:pin_change");

	status.type = MFW_SIM_PIN1;
		sim_pin_status(&status);
		if(status.set EQ MFW_SIM_ENABLE) /*if PIN enabled no show menu entry */
		{	if(sim_pin_count(MFW_SIM_PIN1) > 0)
				SEND_EVENT(pin_data->pin_win,PIN1_REQ_OLD,0,pin_data);
			else
			{   /* not included the state of the puk counter ! */
				pin_data->puk_request = TRUE;
				SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
			}
		}
		else
		{
			setting_pin_destroy(win);
			pin1_not_enabled_screen();
		}
		}
		return 1;
}


/*******************************************************************************

 $Function:	pin2_change 
 $Description:	PIN2 change : call from menu Settings
 $Returns:		Status int
 $Arguments:	menu, item
*******************************************************************************/
int pin2_change (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
		T_MFW_HND idle_win = idle_get_window();
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL) /* x0039928 - Lint warning removal */
		{	win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;

		pin_data->set_state = CHANGE_PIN2;

	TRACE_FUNCTION("MmiPins:pin2_change");
	      // June 16, 2005  REF: CRR 31267  x0021334
	      // Check if PIN2 and PUK2 are initialised
		if((sim_pin_count(MFW_SIM_PIN2) > 0) AND (sim_pin_count(MFW_SIM_PUK2) NEQ 0))
			SEND_EVENT(pin_data->pin_win,PIN2_REQ_OLD,0,pin_data);
		// If PIN2 is not initialised, ask for PUK 2
		else if ((sim_pin_count(MFW_SIM_PIN2) EQ 0) AND (sim_pin_count(MFW_SIM_PUK2) > 0))
		{
			pin_data->puk_request = TRUE;
			SEND_EVENT(pin_data->pin_win,PUK2_REQ,0,pin_data);
		}
		// Else display that PIN2 is not available
		else
		{
			SEND_EVENT(pin_data->pin_win,PIN2_SIM_FAILURE,0,pin_data);
		}
		}
		return 1;
}


/*******************************************************************************

 $Function:	pin_enable
 $Description:	PIN1 enable : call from menu Settings
 $Returns:		Status int
 $Arguments:	menu, item
*******************************************************************************/
int pin_enable (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
		T_MFW_HND idle_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_SIM_PIN_STATUS status;
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL)  /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
    
		status.type = MFW_SIM_PIN1;//get pin1 status
		sim_pin_status(&status);
		if(status.set EQ MFW_SIM_DISABLE) /*if PIN disabled*/
		{	pin_data->set_state = ENABLE;
			if(sim_pin_count(MFW_SIM_PIN1) > 0) //if PIN1 not blocked
			{	//ask for PIN1
			    SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);	
			}
			else
			{	//ask for PUK1
				pin_data->puk_request = TRUE;
			 SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
			}
		}
		else //PIN already enabled
			SEND_EVENT(pin_data->pin_win,PIN1_ENAB_END,0,pin_data);
		}
		return 1;
}



/*******************************************************************************

 $Function:	pin_disable
 $Description:	PIN1 disable : call from menu Settings
 $Returns:		Status int
 $Arguments:	menu, item
*******************************************************************************/
int pin_disable (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
		T_MFW_HND idle_win =mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_SIM_PIN_STATUS status;
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;

		if(win != NULL) /* x0039928 - Lint warning removal */
		{	win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;


    TRACE_FUNCTION("MmiPins:pin_disable");
		status.type = MFW_SIM_PIN1; //get pin1 status
		sim_pin_status(&status);
		/*MC CONQ 5578, 27/05/02, if pin disabling NOT available*/
		if(status.stat EQ MFW_SIM_NO_DISABLE)
		{	/*tell user and destroy PIN window*/
			//ShowMessage(win, TxtNotAllowed, TxtNull);
			setting_pin_destroy(win);
		}
		else
		{
			/*if PIN enabled */
			if(status.set EQ MFW_SIM_ENABLE )
			{
				pin_data->set_state = DISABLE;
				if(sim_pin_count(MFW_SIM_PIN1) > 0) //if PIN not blocked
				{//ask for PIN
				 SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
				}
				else
				{	//ask for PUK1
				    pin_data->puk_request = TRUE;
					SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
				}
			}
			else //if PIN already disabled no need to ask for PIN
				SEND_EVENT(pin_data->pin_win,PIN1_DISAB_END,0,pin_data);
		}
	}
		return 1;
}


/*******************************************************************************

 $Function:	pin_status  
 $Description:	how if pin request is on or off
 $Returns:		Status int
 $Arguments:	menu, item
*******************************************************************************/
int pin_status (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{

		T_MFW_HND idle_win = idle_get_window();
		T_MFW_HND win = setting_pin_create(idle_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL) /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;


	TRACE_FUNCTION("MmiPins:pin_status");

	    SEND_EVENT(pin_data->pin_win,PIN1_STATUS,0,pin_data);
		}
	    return 1;

}



/*******************************************************************************

 $Function:	pin2_fdn_activate 
 $Description:	PIN2 activate fdn    call from menu Settings
 $Returns:		Status int
 $Arguments:	none
*******************************************************************************/
int pin2_fdn_activate (void)
{		T_MFW_HND parent_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(parent_win);
		T_MFW_SIM_PIN_STATUS status;
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL) /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
	
	
		pin_data->set_state = FDN_ACTIV;

	TRACE_FUNCTION("MmiPins:pin2_check");

	status.type = MFW_SIM_PIN2;
		sim_pin_status(&status);
		SimHasPin(MFW_SIM_PIN2);

		TRACE_EVENT_P1("PIN2 status is: %d",status.stat);
	       // June 16, 2005  REF: CRR 31267  x0021334
	       // Check if PIN2 and PUK2 are initialised
		if ((sim_pin_count(MFW_SIM_PIN2) > 0) AND (sim_pin_count(MFW_SIM_PUK2) NEQ 0))
		{
			SEND_EVENT(pin_data->pin_win,PIN2_REQ,0,pin_data);
		}
		// If PIN2 is not initialised, ask for PUK 2
		else if ((sim_pin_count(MFW_SIM_PIN2) EQ 0) AND (sim_pin_count(MFW_SIM_PUK2) > 0))
		{
			pin_data->puk_request = TRUE;
			SEND_EVENT(pin_data->pin_win,PUK2_REQ,0,pin_data);
		}
		// Else display that PIN2 is not available
		else
		{
			SEND_EVENT(pin_data->pin_win,PIN2_SIM_FAILURE,0,pin_data);
		}
	}
		return 1;
}
void pin1_not_enabled_screen( void )
{		
	T_MFW_HND	    parent_window	 = mfwParent( mfw_header() );
	T_DISPLAY_DATA display_info; 

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtEnablePIN, TxtNull , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)not_avail_cb, THREE_SECS, KEY_RIGHT|KEY_LEFT );
	
	info_dialog( parent_window, &display_info );
}

void pin2_not_available_screen( void )
{		
	T_MFW_HND	    parent_window	 = mfwParent( mfw_header() );
	T_DISPLAY_DATA display_info; 

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtPin2Code, TxtNotAvailable , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)not_avail_cb, THREE_SECS, KEY_RIGHT|KEY_LEFT );
	
	info_dialog( parent_window, &display_info );
}

static void not_avail_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{

		switch (reason)
		{
		case INFO_KCD_LEFT:
		case INFO_KCD_RIGHT:
		
		case INFO_TIMEOUT:
			winShow(win);
			break;
		}
	
}
/*******************************************************************************

 $Function:	pin2_fdn_deactivate
 $Description:	PIN2 deactivate fdn    call from menu Settings
 $Returns:		Status int
 $Arguments:	none
*******************************************************************************/
int pin2_fdn_deactivate (void)
{
		T_MFW_HND parent_win = mfwParent( mfw_header() );
		T_MFW_HND win = setting_pin_create(parent_win);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		
		if(win != NULL) /* x0039928 - Lint warning removal */
		{	win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
//test if sim card has a PIN2
		pin_data->set_state = FDN_DEACTIV;

	TRACE_FUNCTION("MmiPins:pin2_fdn_deactivate");
	       // June 16, 2005  REF: CRR 31267  x0021334
	       // Check if PIN2 and PUK2 are initialised
		if ((sim_pin_count(MFW_SIM_PIN2) > 0) AND (sim_pin_count(MFW_SIM_PUK2) NEQ 0)) //if there is a sim pin2 count
		{
			SEND_EVENT(pin_data->pin_win,PIN2_REQ,0,pin_data);
		}
		//  If PIN2 is not initialised, ask for PUK 2
		else if ((sim_pin_count(MFW_SIM_PIN2) EQ 0) AND (sim_pin_count(MFW_SIM_PUK2) > 0))
		{
			pin_data->puk_request = TRUE;
			SEND_EVENT(pin_data->pin_win,PUK2_REQ,0,pin_data);
		}
		// Else display that PIN2 is not available
		else
		{
			SEND_EVENT(pin_data->pin_win,PIN2_SIM_FAILURE,0,pin_data);
		}
		}
		return 1;
}




/*******************************************************************************

 $Function:	sett_pin_edit
 $Description:	help function for editor dialog
 $Returns:		none
 $Arguments:	window, event
*******************************************************************************/
static void sett_pin_edit(T_MFW_HND win, USHORT event)
{

//open the editor for entering the pin

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_edt_win;



	pin_edt_win = set_pin_edt_create(pin_data->pin_win);



    if(pin_edt_win)
	{
		SEND_EVENT(pin_edt_win,event,0,pin_data);
	}
 


}



/*******************************************************************************

 $Function:	set_pin_edt_create
 $Description:	Creation of an instance  for the PIN Editor dialog settings
				Type of dialog : SINGLE_DYNAMIC
 $Returns:		window
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND set_pin_edt_create (T_MFW_HND parent_window)
{
  T_pin_edt * data = (T_pin_edt *)ALLOC_MEMORY (sizeof (T_pin_edt));
  T_MFW_WIN * win;

  data->pin_edt_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)set_pin_edt_win_cb);

  TRACE_FUNCTION("MmiPins:set_pin_edt_create");

  if (data->pin_edt_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)set_pin_editor;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_edt_win)->data;
  win->user		      = (void *) data;

  /*
   * Create any other handler
   */

  data->tim_out_handle = tim_create(data->pin_edt_win,TIMEOUT,(T_MFW_CB)pin_edt_tim_out_cb);

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
  data->editor = ATB_edit_Create(&data->editorPinAttr,0);
#else /* NEW_EDITOR */
  data->editor_handle = edt_create(data->pin_edt_win,&data->editpinAttr,0,0);
#endif /* NEW_EDITOR */
  data->kbd_handle = kbd_create(data->pin_edt_win,KEY_ALL,(T_MFW_CB)set_pin_kbd_cb);
   data->kbd_long_handle = kbd_create(data->pin_edt_win,KEY_ALL | KEY_LONG,(T_MFW_CB)set_pin_edt_kbd_long_cb);
  /*
   * return window handle
   */
  return data->pin_edt_win;
}


/*******************************************************************************

 $Function:	set_pin_edt_destroy
 $Description:	Destroy the pin editor dialog
 $Returns:		none
 $Arguments:	window
*******************************************************************************/
void set_pin_edt_destroy  (T_MFW_HND own_window)
{
  T_pin_edt * data ;
  T_MFW_WIN * win;

  if (own_window)
  {
	  TRACE_FUNCTION("MmiPins:set_pin_edt_destroy");

	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_edt *)win->user;

	  if(data)
	  {
      /*
       * Delete WIN Handler
       */
	      win_delete (data->pin_edt_win);
	      FREE_MEMORY((void *)data,(sizeof(T_pin_edt)));
	  }
   }
}

#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:	set_pin_win_cb 
 $Description:	Callback function for editor windows
 $Returns:		status int
 $Arguments:	event, window
*******************************************************************************/
static int set_pin_win_cb (T_MFW_EVENT event,T_MFW_WIN * win)
{

 TRACE_FUNCTION("set_pin_win_cb");
 if (event EQ MfwWinVisible)
  {

	dspl_ClearAll();
	return 1;
  }
  return 0;
}
#endif

/*******************************************************************************

 $Function:	set_pin_edt_win_cb
 $Description:	Callback function for editor windows
 $Returns:		status int
 $Arguments:	event, window
*******************************************************************************/
static int set_pin_edt_win_cb (T_MFW_EVENT event,T_MFW_WIN * win)
{
  T_pin_edt * edt_data = (T_pin_edt *)win->user;//pin edt data
  T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
  T_pin     * pin_data = (T_pin *)win_pin->user; // pin main data

  TRACE_FUNCTION("set_pin_edt_win_cb");

  if (event EQ MfwWinVisible)
  {
  /* SPR#1428 - SH - New Editor changes */
  #ifdef NEW_EDITOR
	ATB_edit_Show(edt_data->editor);
	
	if (edt_data->editor->update!=ED_UPDATE_TRIVIAL)
	{
		/* Set the colour for drawing title */

		resources_setTitleColour(COLOUR_EDITOR);

		dspl_Clear(0,0, SCREEN_SIZE_X-1, edt_data->editor->attr->win_size.py-1);

		/* Title */

		if(pin_emergency_call == TRUE)
		{					
			PROMPT(0,0,0,TxtEmergency);
		}
		else
		{
		   	PROMPT(0,0,0,pin_data->display_id1);
#ifdef SIM_PERS
			if( pin_data->display_id2)
				PROMPT(0,20,0,pin_data->display_id2);
#endif
		}

		/* Soft keys */
		
		if ((strlen((char*)edt_data->pin) >= MIN_PIN) && (strlen((char*)edt_data->pin) < (MAX_PIN + 1)))
		{
				if(!(edt_data->gsm_state))	  /* usual way for PIN 4 digits     */
				{
#ifdef SIM_PERS
					if(pin_data->display_id1 == TxtPhoneBlocked)
						softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);
					else
#endif
					softKeys_displayId(TxtSoftOK,TxtDelete,0, COLOUR_EDITOR_XX);
				}
			}
		//xvilliva - Added the below "if" and moved the existing "if" in to "else". "if" avoids "delete" key display.
		if((strlen((char*)edt_data->pin) == 0))//  May 13, 2004        REF: CRR 13623  xvilliva
		{
			TRACE_EVENT("clear softkey 16 - zero length");
			softKeys_displayId(TxtNull,TxtNull,0, COLOUR_EDITOR_XX);												
		}
		else if((strlen((char*)edt_data->pin) < MIN_PIN) && (edt_data->emergency_call != TRUE))
		{
			TRACE_EVENT("clear softkey 16");	
			softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);												
		}
#ifdef SIM_PERS
		else if((strlen((char*)edt_data->pin) < MIN_PIN) && (edt_data->emergency_call == TRUE))
		{
			TRACE_EVENT("clear softkey 16");	
			if(pin_emergency_call == TRUE)
				softKeys_displayId(TxtSoftCall,TxtDelete,0, COLOUR_EDITOR_XX);												
			else
			softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);												
		}
#endif
	}
	
	edt_data->editor->update = ED_UPDATE_DEFAULT;
	
#else /* NEW_EDITOR */
	pin_icons();
   	PROMPT(INFO_TEXT_X,INFO_TEXT_Y,0,pin_data->display_id1);
   	
  TRACE_FUNCTION("softkey 6");
	softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);
#endif /* NEW_EDITOR */
    return 1;
  }
  return 0;
}



/*******************************************************************************

 $Function:	set_pin_editor
 $Description:	Pin editor Dialog Signal Handling function
 $Returns:		void
 $Arguments:	window, event, value, parameters
*******************************************************************************/
static void set_pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;

    T_pin * pin_data = (T_pin *)parameter;//pin main data

	edt_data->pin_case_edit = pin_data->pin_case;

	TRACE_FUNCTION("MmiPins:set_pin_editor");

  /*
   * depending on event
   */
  switch (event)
  {
	case INPUT_PIN1:
	case INPUT_PIN2:
	case INPUT_PUK1:
	case INPUT_PUK2:
	case INPUT_NEW_PIN:
	case INPUT_NEW_PIN_AGAIN:
	case INPUT_NEW_PIN2:
	case INPUT_NEW_PIN2_AGAIN:
#ifdef SIM_PERS
    case INPUT_SIM_PLOCK:
    case INPUT_SIM_FCPWD:
   case INPUT_NEW_SIM_PLOCK:
    case INPUT_NEW_SIM_PLOCK_AGAIN:
	case PERM_BLK1:
	//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	case INPUT_SIM_MKPWD:

#endif
 		//SPR#717 - GW - Wrong structures being cleared
		memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string  */
		memset(edt_data->puk, '\0', sizeof(edt_data->puk));
		memset(edt_data->new_pin, '\0', sizeof(edt_data->new_pin));
		memset(edt_data->old_pin, '\0', sizeof(edt_data->old_pin));

		/* Set up pin entry attributes */
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_SetAttr( &edt_data->editorPinAttr, PIN_EDIT_RESET, COLOUR_EDITOR_XX, EDITOR_FONT, 0, ED_CURSOR_UNDERLINE, ATB_DCS_ASCII, (UBYTE *)edt_data->edtbuf, MAX_DIG);
		clear_edit_array(edt_data);	     /*    clear editor buffer	    */
		edt_data->editor_index = 0;
		ATB_edit_Init(edt_data->editor);
		ATB_edit_Show(edt_data->editor);
#else /* NEW_EDITOR */
		editor_attr_init( &edt_data->editpinAttr, PIN_EDIT_RESET, edtCurBar1,0,(char*)edt_data->edtbuf,MAX_DIG,COLOUR_EDITOR_XX);
		clear_edit_array(edt_data);	     /*    clear editor buffer	    */
		edt_data->editor_index = 0;
		edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */

		winShow(edt_data->pin_edt_win);

		break;

	default:
		break;
  }
}



/*******************************************************************************

 $Function:	set_pin_kbd_cb
 $Description:	Callback function for keyboard settings pin
 $Returns:		status int
 $Arguments:	event, keyboard control block
*******************************************************************************/
static int set_pin_kbd_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;

	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

	char timer = TRUE;

	timStop(edt_data->tim_out_handle);/* stop entering timer      */

	TRACE_FUNCTION("MmiPins:set_pin_kbd_cb");

#ifdef SIM_PERS
	if( !perm_blocked)
#endif
	{
	switch(kc->code)
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
			if(edt_data->editor_index < MAX_DIG)
			{
				/* xreddymn OMAPS00083495 Jul-04-2006 
				 * Limit PIN1, PIN2 entries to 8 digits.
				 */
				if(((edt_data->pin_case_edit==INPUT_PIN1) || 
					(edt_data->pin_case_edit==INPUT_PIN2) ||
					(edt_data->pin_case_edit==INPUT_NEW_PIN) ||
					(edt_data->pin_case_edit==INPUT_NEW_PIN_AGAIN) ||
					(edt_data->pin_case_edit==INPUT_NEW_PIN2) ||
					(edt_data->pin_case_edit==INPUT_NEW_PIN2_AGAIN)) &&
					(edt_data->editor_index>=8) && 
					(!edt_data->gsm_state))

					return MFW_EVENT_CONSUMED;
				else
				{
					if(kc->code EQ KCD_STAR)
						edt_data->pin[edt_data->editor_index] = STAR;
					else
						edt_data->pin[edt_data->editor_index] = '0' + kc->code;

					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					ATB_edit_AsciiChar(edt_data->editor, '*', TRUE);
#else /* NEW_EDITOR */
					edtChar(edt_data->editor_handle,'*');/* hide security code */
#endif /* NEW_EDITOR */

					edt_data->editor_index++;
					check_set_pins(win,edt_data); /* digits check depends on status pin_case */
					if(edt_data->gsm_state)
						pin_data->gsm_state = edt_data->gsm_state;
				}
			}
			winShow(edt_data->pin_edt_win);
			break;

		case KCD_HASH:
		case KCD_LEFT:

			if(pin_emergency_call == TRUE)
			{
				callNumber((UBYTE *)pin_emerg_call);
				pin_emergency_call = FALSE;
			}

			/* If it's a hash, insert it into the editor */
			
			// change by Sasken ( Rashmi C N) on March 25th 2004
			// Issue Number : MMI-SPR-15692
			// Subject: Wrong entry in last dialed number.
			// Bug : While saving PIN, if a hash is entered as a 
			// delimiter/terminater, the hash is also getting stored 
			// as  a part of pin. This makes it mandatory for the user 
			// to enter hash as delimiter/terminater while he is authenticating.
			// Solution: Avoid copying the hash(delimiter/terminater) in to the pin array.
			//			 Commenting the copying part below in the "if" block.
			
			// If it's a hash, insert it into the editor 
			//if(kc->code EQ KCD_HASH)
			//xrashmic 1 Jul, 2006 OMAPS00075784
			//To fix the reset while entering the gsm sequence for unblocking 
			// the pin1 or pin2
			if(!(strncmp((char*)edt_data->pin,"**052*",IDENT_GSM_2))||!(strncmp((char*)edt_data->pin,"**05*",IDENT_GSM_1)))

			{
				edt_data->pin[edt_data->editor_index] = HASH;
				// SPR#1428 - SH - New Editor changes 
#ifdef NEW_EDITOR
				ATB_edit_AsciiChar(edt_data->editor, edt_data->pin[edt_data->editor_index], TRUE);
#else // NEW_EDITOR 
				edtChar(edt_data->editor_handle,edt_data->pin[edt_data->editor_index]);
#endif // NEW_EDITOR 
				edt_data->editor_index++;
			}

			

			// We save the pin2 entered in to this global variable which we
			// use while calling sAT_PlusCLCK(), while locking or unlocking ALS. 					
			memset(g_pin2,0,MAX_PIN+1);
			if(edt_data->pin_case_edit EQ INPUT_PIN2 || edt_data->pin_case_edit EQ INPUT_NEW_PIN2)
				strncpy(g_pin2,(char*)edt_data->pin,MAX_PIN);//xvilliva SPR17291
			switch(edt_data->pin_case_edit)
			{
#ifdef SIM_PERS                
                case INPUT_SIM_PLOCK:
             	  case  INPUT_SIM_FCPWD:
		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		  case INPUT_SIM_MKPWD:
                		
#endif                            
			/* PIN1 and PIN2 */
				case INPUT_PIN1:/* finish of PIN entering */
				case INPUT_PIN2:
					TRACE_EVENT_P1("pin_emergency_call = %d", pin_emergency_call);
					TRACE_EVENT("When 'Call' is pressed!");

					/* PIN is long enough, finish editing */
					
					if ((strlen((char*)edt_data->pin) >= MIN_PIN)  && !(edt_data->gsm_state))
					{
						clear_edit_array(edt_data);	    /*	   clear editor array */
						timer = FALSE;
						strncpy((char*)edt_data->old_pin,(char*)edt_data->pin,MAX_PIN);
						strncpy((char*)pin_data->old_pin, (char*)edt_data->old_pin,MAX_PIN);
						if(edt_data->pin_case_edit EQ INPUT_PIN1)
						{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,PIN1_END,0,pin_data);
						}

#ifdef SIM_PERS                        
                         			else if(edt_data->pin_case_edit EQ INPUT_SIM_PLOCK)
                         			{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,SIM_PLOCK_END,0,pin_data);
						}

		                         	else if(edt_data->pin_case_edit EQ INPUT_SIM_FCPWD)
                         			{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,SIM_FCPWD_END,0,pin_data);
						}
						//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
						else if(edt_data->pin_case_edit EQ INPUT_SIM_MKPWD)
                         			{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,SIM_MKPWD_END,0,pin_data);
						}	  
                         
#endif
						else
						{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,PIN2_END,0,pin_data);
						}
					}
					else
					{
						if(kc->code EQ KCD_HASH)
						{	
							if (!(edt_data->gsm_state))	/* could become gsm */
							{
								check_pins(win,edt_data);
								if(edt_data->gsm_state)
									pin_data->gsm_state = edt_data->gsm_state;
							}
							else /* finish gsm string */
							{
								timer = FALSE;
								if(edt_data->pin_case_edit EQ INPUT_PIN1)
									pin_data->pin_case = PIN1_REQ_OLD;
								else
									pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

							}
						}
					}
					break;

				/* PUK1 and PUK2 */
				
				case INPUT_PUK1:
				case INPUT_PUK2:
					if ((strlen((char*)edt_data->pin) >= MIN_PIN) && !(edt_data->gsm_state)) /* finish of PUK entering */
					{
						clear_edit_array (edt_data);		/* clear editor array */
						timer = FALSE;
						strncpy((char*)edt_data->puk,(char*)edt_data->pin,MAX_PIN);/* store puk  */
						strncpy((char*)pin_data->puk, (char*)edt_data->puk,MAX_PIN);
						if( pin_data->pin_case EQ INPUT_PUK1) 
						{
							    set_pin_edt_destroy(edt_data->pin_edt_win);
								SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN,0,pin_data);/* entering new pin1 start*/
						}
						else
						{
						    set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN2,0,pin_data);/* entering new pin2 start*/
						}
					}
					else
					{
						if(kc->code EQ KCD_HASH)
						{		
							if (!(edt_data->gsm_state))/* could become gsm	    */
							{
								check_pins(win,edt_data);
								if(edt_data->gsm_state)
									pin_data->gsm_state = edt_data->gsm_state;
							}
							else /* finish gsm string  */
							{
								timer = FALSE;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

							}
						}
					}
					break;
					
				case INPUT_NEW_PIN:
				case INPUT_NEW_PIN2:

		#ifdef SIM_PERS
                            case INPUT_NEW_SIM_PLOCK:
                #endif
					if ((strlen((char*)edt_data->pin) >= MIN_PIN)  && !(edt_data->gsm_state))
					{
						clear_edit_array (edt_data);			  /* clear editor array */
						timer = FALSE;
						strncpy((char*)edt_data->new_pin, (char*)edt_data->pin,MAX_PIN);	  /*  store new pin	  */
						strncpy((char*)pin_data->new_pin, (char*)edt_data->new_pin,MAX_PIN);
						if(pin_data->pin_case EQ INPUT_NEW_PIN )
						{
						    set_pin_edt_destroy(edt_data->pin_edt_win);       /* entering new pin1 again*/
							SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN_AGAIN,0,pin_data);
						}
					
				#ifdef SIM_PERS
			             		else if(pin_data->pin_case EQ INPUT_NEW_SIM_PLOCK)
                                       	{
                                           	set_pin_edt_destroy(edt_data->pin_edt_win);
                                           	SEND_EVENT(pin_data->pin_win,INPUT_NEW_SIM_PLOCK_AGAIN,0,pin_data);
                                         }
	                    #endif
						else
						{
						    set_pin_edt_destroy(edt_data->pin_edt_win);       /* entering new pin1 again*/
							SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN2_AGAIN,0,pin_data);
						}
					}
					else
					{
						if(kc->code EQ KCD_HASH)
						{	
							if(!(edt_data->gsm_state)) /* could become gsm	    */
							{
								check_pins(win,edt_data);
								if(edt_data->gsm_state)
						       		pin_data->gsm_state = edt_data->gsm_state;
							}
							else /* finish gsm string  */
							{
								timer = FALSE;

								if(edt_data->pin_case_edit EQ INPUT_NEW_PIN)
									pin_data->pin_case = PIN1_REQ_OLD;
								else
									pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

							}
						}
					}
					break;

	#ifdef SIM_PERS
                            case INPUT_NEW_SIM_PLOCK_AGAIN:
       #endif						
				case INPUT_NEW_PIN_AGAIN:
				case INPUT_NEW_PIN2_AGAIN:
						if ((strlen((char*)edt_data->pin) >= MIN_PIN) && !(edt_data->gsm_state))
						{
							clear_edit_array (edt_data);			  /* clear editor array */
							timer = FALSE;
							if(strcmp((char*)edt_data->pin, (char*)pin_data->new_pin) EQ 0) /* compare PINs successfull*/
							{
								strncpy((char*)pin_data->new_pin, (char*)edt_data->pin,MAX_PIN);
								if(pin_data->pin_case EQ INPUT_NEW_PIN_AGAIN)
								{
								    set_pin_edt_destroy(edt_data->pin_edt_win);
									SEND_EVENT(pin_data->pin_win,NEW_PIN_END,0,pin_data);/* changing PIN1 */
								}
						#ifdef SIM_PERS
                                                		else if (pin_data->pin_case EQ INPUT_NEW_SIM_PLOCK_AGAIN)
                                                   	{
                                                     		set_pin_edt_destroy(edt_data->pin_edt_win);
                                                    		SEND_EVENT(pin_data->pin_win, NEW_SIM_PLOCK_END,0,pin_data);
                                                   	}
          					#endif
								else
								{
								    set_pin_edt_destroy(edt_data->pin_edt_win);
									SEND_EVENT(pin_data->pin_win,NEW_PIN2_END,0,pin_data);/* changing PIN2 */
								}
							}
							else
							{									/* new PIN != new PIN again */

								if(pin_data->pin_case EQ INPUT_NEW_PIN_AGAIN)
								{
									set_pin_edt_destroy(edt_data->pin_edt_win);
									SEND_EVENT(pin_data->pin_win,NEW_PIN_FAIL,0,NULL);
								}
						#ifdef  SIM_PERS  
								else if(pin_data->pin_case EQ INPUT_NEW_SIM_PLOCK_AGAIN)
								{
                                        				set_pin_edt_destroy(edt_data->pin_edt_win);
                                        				SEND_EVENT(pin_data->pin_win,NEW_SIM_PLOCK_FAIL,0,NULL);
                                 				}
                                        #endif		
								else
								{
									set_pin_edt_destroy(edt_data->pin_edt_win);
									SEND_EVENT(pin_data->pin_win,NEW_PIN2_FAIL,0,NULL);
								}
							}
						}
						else
						{
							if(kc->code EQ KCD_HASH)
							{
								if (!(edt_data->gsm_state)) /* could become gsm	    */
								{
									check_pins(win,edt_data);
									if(edt_data->gsm_state)
							       		pin_data->gsm_state = edt_data->gsm_state;
								}
								else /* finish gsm string  */
								{
									timer = FALSE;
									if(edt_data->pin_case_edit EQ INPUT_NEW_PIN_AGAIN)
										pin_data->pin_case = PIN1_REQ_OLD;
									else
										pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

								}
							}
						}
						break;
					
					default:
						break;
				}
				break;
				
/* KCD_HUP */
			case KCD_HUP:
				if( edt_data->pin_case_edit != INPUT_PUK1)//xvilliva SPR13623
				{
				clear_edit_array(edt_data);
				timer = FALSE;
				set_pin_edt_destroy(edt_data->pin_edt_win);
	    		SEND_EVENT(pin_data->pin_win,SETT_ABORT,0,NULL); // abort settings menu
				}
				break;

/* KCD RIGHT */

			case KCD_RIGHT: 	
			//nm, go back to the submenu if there is no character on the screen
				if(strlen((char*)edt_data->pin) == 0 && edt_data->pin_case_edit != INPUT_PUK1)//xvilliva SPR13623
				{
					clear_edit_array(edt_data);
					timer = FALSE;
					set_pin_edt_destroy(edt_data->pin_edt_win);
		    		SEND_EVENT(pin_data->pin_win,SETT_ABORT,0,NULL); // abort settings menu
					return 1;			    
				}

				if(edt_data->editor_index EQ 0)
					;
				else
					edt_data->editor_index--;

				edt_data->pin[edt_data->editor_index] = '\0';
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,ecBack);/* delete character    */
#endif /* NEW_EDITOR */

				if(strlen((char*)edt_data->pin)<MIN_PIN)
				{
				}

				edt_data->emergency_call = FALSE;
				if(edt_data->pin_case_edit EQ INPUT_PUK1)  /* special case ?? */
					winShow(edt_data->pin_edt_win);
				else
				{
					check_set_pins(win,edt_data);	       /* check remain for emergency call  */
					winShow(edt_data->pin_edt_win);
				}
			    break;
			    
			default:
				break;
		}
	}
#ifdef SIM_PERS
	else
	{
		switch(kc->code)
		{
		case KCD_0:
		case KCD_1:
		case KCD_2:
		case KCD_6:
		case KCD_8:
		case KCD_9:
			if(edt_data->editor_index < MAX_DIG)
			{
				edt_data->pin[edt_data->editor_index] = '0' + kc->code;

				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_AsciiChar(edt_data->editor, '*', TRUE);
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,'*');/* hide security code */
#endif /* NEW_EDITOR */

				edt_data->editor_index++;
				check_set_pins(win,edt_data); /* digits check depends on status pin_case */
				if(edt_data->gsm_state)
					pin_data->gsm_state = edt_data->gsm_state;
			}
			winShow(edt_data->pin_edt_win);
			break;
				
	
		case KCD_LEFT:

			if(pin_emergency_call == TRUE)
			{
				callNumber((UBYTE *)pin_emerg_call);
				pin_emergency_call = FALSE;
			}
			break;

		case KCD_RIGHT: 	

//				if( edt_data->edtbuf == '\0' )
//					break;
				if(edt_data->editor_index EQ 0)
					;
				else
					edt_data->editor_index--;

				edt_data->pin[edt_data->editor_index] = '\0';
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,ecBack);/* delete character    */
#endif /* NEW_EDITOR */

				if(strlen((char*)edt_data->pin)<MIN_PIN)
				{
				}

				edt_data->emergency_call = FALSE;
				if(edt_data->pin_case_edit EQ INPUT_PUK1)  /* special case ?? */
					winShow(edt_data->pin_edt_win);
				else
				{
					check_set_pins(win,edt_data);	       /* check remain for emergency call  */
					winShow(edt_data->pin_edt_win);
				}
			    break;

			default:
				break;
			}	
	}
#endif /* SIM_PERS */
	if (timer)
		tim_start(edt_data->tim_out_handle);/* start timer for entering */
	return 1;
}



/*******************************************************************************

 $Function:	set_pin_edt_kbd_long_cb
 $Description:	Callback function for keyboard long
 $Returns:		status int
 $Arguments:	event, keyboard control block
*******************************************************************************/
static int set_pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

    TRACE_FUNCTION("MmiPins:set_pin_edt_kbd_long_cb");

    if ((event & KEY_CLEAR) && (event & KEY_LONG))
	{
			timStop(edt_data->tim_out_handle); /* stop entering timer	*/
			clear_edit_array(edt_data);
			memset(edt_data->pin,'\0',sizeof(edt_data->pin));/* initial string */
			
			/* SPR#1428 - SH - New Editor: string changed, update word-wrap*/
	#ifdef NEW_EDITOR
			ATB_edit_Refresh(edt_data->editor);
	#endif /* NEW_EDITOR */
			edt_data->editor_index = 0;
			edt_data->emergency_call = FALSE;
			winShow(edt_data->pin_edt_win);
	}

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	check_set_pins 
 $Description:	check input string for settings
 $Returns:		void
 $Arguments:	window pin editor attributes
*******************************************************************************/
static void check_set_pins (T_MFW_HND win,void * edt_pin)
{
	T_pin_edt * edt_data = (T_pin_edt *)edt_pin;

	TRACE_FUNCTION("MmiPins: check_set_pins");

	if(strlen((char*)edt_data->pin)NEQ 0)
    {
	    if (strlen((char*)edt_data->pin) > FIRST_DIGIT)	 /* for every character  */
			gsm_set_test(win,edt_data);	/* test if GSM string or not*/
		/*API - 10/10/02 - 1162 - check the value entered into the PIN editor to see if the 
								  value being entered in is an emergency number.
		*/
		if (strlen((char*)edt_data->pin) EQ (MIN_PIN - 1) || strlen((char*)edt_data->pin) EQ (MIN_PIN -2))
		{
			TRACE_FUNCTION("cm_check_emergency() TRACE PINS 1");
			if(cm_check_emergency((U8*)edt_data->pin))/* test emergency call*/
			{
				edt_data->emergency_call = TRUE;
				pin_emergency_call = TRUE;
				memset(pin_emerg_call, '\0',sizeof(pin_emerg_call));
				strcpy((char *)pin_emerg_call, (char *)edt_data->pin);
			}
			else
			{
// Nov 24, 2005, a0876501, DR: OMAPS00045909
				if((edt_data->pin_case_edit EQ CARD_REJECTED) OR (edt_data->pin_case_edit EQ INSERT_CARD)
					OR (edt_data->pin_case_edit EQ SHOW_IMEI_INVALID)				
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS	
					//x0pleela 30 Aug, 2006 DR: OMAPS00091250
					OR(edt_data->pin_case_edit EQ NO_MEPD_DATA)
#endif					
					)
				{			  /* delete all digits	    */
					clear_edit_array (edt_data);  /*  clear editor array	 */
					memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string */
					edt_data->editor_index = 0;
				}
				edt_data->emergency_call = FALSE;
				pin_emergency_call = FALSE;
			}
		}
		else
		{
// Nov 24, 2005, a0876501, DR: OMAPS00045909
			if((edt_data->pin_case_edit EQ CARD_REJECTED) OR (edt_data->pin_case_edit EQ INSERT_CARD)
				OR (edt_data->pin_case_edit EQ SHOW_IMEI_INVALID)				
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS					
				//x0pleela 30 Aug, 2006 DR: OMAPS00091250
				OR (edt_data->pin_case_edit EQ NO_MEPD_DATA)
#endif				
				)
			{			  /* delete all digits	    */
				clear_edit_array (edt_data);  /*  clear editor array	 */
				memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string */
				edt_data->editor_index = 0;
			}
			edt_data->emergency_call = FALSE;
			pin_emergency_call = FALSE;
		}
		/*SPR 2145, if entering PIN1, then make sure no more than 8 characters are entered*/
		if(((strlen((char*)edt_data->pin) > MAX_PIN) || /*no more than 16 digits allowed*/
			(edt_data->pin_case_edit EQ INPUT_PIN1 && strlen((char*)edt_data->pin) > MAX_PIN_EN_DIS ))/*PIN1 should be no more than 8 digits*/
			&& !(edt_data->gsm_state)) /*allowed up to 30 digits if entering GSM string*/
		{

			if(edt_data->editor_index EQ 0)
				;
			else
				edt_data->editor_index--;

			edt_data->pin[edt_data->editor_index] = '\0';

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
			edtChar(edt_data->editor_handle,ecBack);/* delete character    */
#endif /* NEW_EDITOR */
		}
	}
}



/*******************************************************************************

 $Function:	sett_pin_mess 
 $Description:	help function for message dialog
 $Returns:		void
 $Arguments:	window, event
*******************************************************************************/
static void sett_pin_mess(T_MFW_HND win,USHORT event)
{

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_mess_win;
	TRACE_FUNCTION("MmiPins:sett_pin_mess");

	pin_mess_win = set_pin_mess_create(pin_data->pin_win);
	if(pin_mess_win)
	{
		SEND_EVENT(pin_mess_win,event,0,pin_data);
	}
}


/*******************************************************************************

 $Function:	set_pin_mess_create
 $Description:	Creation of an instance  for the PIN Message dialog settings
				Type of dialog : SINGLE_DYNAMIC
 $Returns:		void
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND set_pin_mess_create(T_MFW_HND parent_window)
{
  T_pin_mess * data = (T_pin_mess *)ALLOC_MEMORY (sizeof (T_pin_mess));
  T_MFW_WIN	* win;

  data->pin_mess_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)set_pin_mess_win_cb);

  TRACE_FUNCTION("MmiPins: set_pin_mess_create");

  if (data->pin_mess_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)set_pin_messages;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_mess_win)->data;
  win->user		      = (void *) data;

  /*
   * return window handle
   */
  return data->pin_mess_win;
}


/*******************************************************************************

 $Function:	set pin_mess_destroy
 $Description:	Destroy the pin message dialog settings
 $Returns:		void
 $Arguments:	window
*******************************************************************************/
void set_pin_mess_destroy  (T_MFW_HND own_window)
{
  T_pin_mess * data;
  T_MFW_WIN * win;


  TRACE_FUNCTION("MmiPins: set_pin_mess_destroy");

  if (own_window)
  {
	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_mess *)win->user;

	  if(data)
	  {

      /*
       * Delete WIN Handler
       */
	  win_delete (data->pin_mess_win);
	      FREE_MEMORY((void*)data,(sizeof(T_pin_mess)));
	  }
   }
}


/*******************************************************************************

 $Function:	set_pin_mess_win_cb
 $Description:	Callback function for message windows
 $Returns:		void
 $Arguments:	event, window
*******************************************************************************/
static int set_pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}


/*******************************************************************************

 $Function:	set_pin_messages
 $Description:	Message Dialog for PIN/PUK handling in settings
 $Returns:		void
 $Arguments:	win, event, value, parameter
*******************************************************************************/
void set_pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_mess * mess_data = (T_pin_mess *)win_data->user;

	T_DISPLAY_DATA display_info;

//    U8 uMode;   /* Warning Correction */
	
	T_MFW_SIM_PIN_STATUS status; //need to check the Pin 1 status
	
	T_pin * pin_data = (T_pin *)parameter;
#ifdef SIM_PERS
T_MFW status1 = 0, status2;
  int max=0,max1=0;
	int curr= 0,curr1=0;
#endif
  TRACE_FUNCTION("Mmi.Pins:set_pin_messages");

  /*
   * depending on signal
   */
	
  switch (event)
  {
	case PIN1_REQ_ATT:
	case PIN2_REQ_ATT:
	case PUK1_REQ_VER_INFO:
	case GSM_FAIL:
	case NEW_PIN_FAIL:
	case NEW_PIN2_FAIL:
	case PIN_OK_INFO:
	case PIN1_CH_SUCC:
	case PIN2_CH_SUCC:
	case PUK1_UNBL_FAIL_INFO:
	case PUK1_UNBL_SUCC:
	case PUK1_UNBL_FATAL_ERROR:
	case PIN1_ENAB_END:
	case PIN1_DISAB_END:
	case PIN1_ENDIS_FAIL:
#ifdef SIM_PERS     
    	case SIM_LOCK_ENDIS_FAIL:
      	case SIM_LOCK_ENDSUCC:
       case SIM_LOCK_FCPWD_FAIL:
       case SIM_LOCK_FCPWDSUCC:
	case SIM_LOCK_STATUS:
    	case NEW_SIM_PLOCK_FAIL:
    	case SIM_LOCK_NEW_ENDSUCC:
    	case SIM_LOCK_NEW_ENDISFAIL:
    	case SIM_LOCK_NEW_ENDSUCC_CKEY:	
    	case SIM_LOCK_ALREADY_ENDIS:
    	case SIM_LOCK_SIM_REM:
	case SMLK_SHOW_FC_FAIL1:	
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
       case SIM_LOCK_MKPWD_FAIL:	//Master unlock failed
       case SIM_LOCK_MKPWDSUCC:	//Master unlock success
       case SIM_LOCK_BUSY:			//Category unlock return with Busy error from ACI
       //x0pleel 26 July, 2006 ER: OMAPS00087586, OMAPS00087587
       case SIM_LOCK_BUSY_UNBLOCK: //ME unblock return Busy error from aCI
#endif       
	case FDN_FAIL:
	case FDN_ACT_END:
	case FDN_DEACT_END:
	case PIN1_STATUS:
	case PIN2_SIM_FAILURE:      // Marcus: Issue 1609: 23/01/2003 - failure to verify PIN2


		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull,  COLOUR_STATUS_PINS);
		
		switch(event)
		{
		   case PIN1_REQ_ATT:
			  pin_data->pin_case = PIN1_REQ_OLD;
			  display_info.TextId = TxtPINFail;
			  if(pin_data->pin_retries EQ 1)
				display_info.TextId2 = TxtOneAttemptLeft;
			  else
				display_info.TextId2 = TxtTwoAttemptsLeft;
			  break;
		   case PIN2_REQ_ATT:
			  pin_data->pin_case = PIN2_REQ;
			  display_info.TextId = TxtPINFail;
			  if(pin_data->pin_retries EQ 1)
				display_info.TextId2 = TxtOneAttemptLeft;
			  else
				display_info.TextId2 = TxtTwoAttemptsLeft;
			  break;
		   case PIN1_CH_SUCC:
			  pin_data->pin_case = PIN1_CH_END;
			  display_info.TextId = TxtChangPin;
			  break;
		   case PIN2_CH_SUCC:
			  pin_data->pin_case = PIN2_CH_END;
			  display_info.TextId = TxtChangPIN2;
			  break;
#ifdef SIM_PERS
		  case SIM_LOCK_NEW_ENDSUCC:
           		  pin_data->pin_case = SIM_LOCK_NEW_ENDSUCC;
			  display_info.TextId = TxtPsLockChanged;
			  break;
			  
		  case SIM_LOCK_ENDSUCC:
		  	  pin_data->pin_case = SIM_LOCK_ENDSUCC;
			  if(pin_data->set_state == ENABLE)
			  	display_info.TextId = TxtLockActivated;
			  else if(pin_data->set_state == DISABLE)
			  	display_info.TextId = TxtLockDeactivated;
			  break;	

		  case SIM_LOCK_ALREADY_ENDIS:
		  	  pin_data->pin_case = SIM_LOCK_ALREADY_ENDIS;
			  if(pin_data->set_state == ENABLE)
			  	display_info.TextId = TxtAlreadyLocked;
			  else if(pin_data->set_state == DISABLE)
			  	display_info.TextId = TxtAlreadyUnlocked;
			  break;	
		
		case SIM_LOCK_NEW_ENDSUCC_CKEY:
			 {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				TRACE_FUNCTION("BMI:SIM_LOCK_NEW_ENDSUCC_CKEY");
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
				pin_data->pin_case = SIM_LOCK_NEW_ENDSUCC_CKEY;
				sprintf((char*)buf1,"%d attempts left",curr_succ_reset);
				sprintf((char*)buf2,"%s", "FC Reset-Success");								
				display_info.TextString= buf1;
				display_info.TextString2 = buf2;
			 }
			  break;

		case SMLK_SHOW_FC_FAIL1:
			  {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
				pin_data->pin_case = PUK1_REQ;
				sprintf((char*)buf1,"%d attempts left",curr_fail_reset);
				sprintf((char*)buf2,"%s", "FC Reset-Failure");								
				display_info.TextString= buf1;
				display_info.TextString2 = buf2;
				  break;
			  }

          case SIM_LOCK_FCPWDSUCC:
			 {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
				sprintf((char*)buf1,"%d attempts left",curr_succ_reset);
				sprintf((char*)buf2,"%s", "FC Reset-Success");								
				display_info.TextString= buf2;
				display_info.TextString2 = buf1;
			 }
			  	break;			  	

		//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
		//Displays a dialog saying "master unlock success"
          	case SIM_LOCK_MKPWDSUCC:
			 {
			 	char buf1[25];
				sprintf((char*)buf1,"%s"," Master Unlock Success");
				display_info.TextString= buf1;
				display_info.TextString2 = 0;
			 }
			  	break;	
#endif

		   case PUK1_REQ_VER_INFO:
			  if((pin_data->set_state EQ CHANGE_PIN2) ||
				 (pin_data->set_state EQ PIN2_CHECK) ||
				  (pin_data->set_state EQ FDN_ACTIV) ||
				  (pin_data->set_state EQ FDN_DEACTIV))
			  {
					pin_data->pin_case = PUK2_REQ;
			  }
			  else
			  {
					pin_data->pin_case = PUK1_REQ;
			  }
			  display_info.TextId =TxtPINBlock;
			  break;
			  
			case GSM_FAIL:
			  if(pin_data->set_state EQ IDLE_GSM)
				  pin_data->pin_case = IDLE_GSM_FAIL;
			  display_info.TextId = TxtNotAcc;
			  break;   
		}
		 switch(event)
		 {
		   case PUK1_UNBL_FATAL_ERROR:
			  if((pin_data->set_state EQ CHANGE_PIN2) ||
				 (pin_data->set_state EQ FDN_ACTIV) ||
				 (pin_data->set_state EQ FDN_DEACTIV))
			  {
					pin_data->pin_case = PUK2_REQ;
			  }
			  else if(pin_data->set_state == IDLE_GSM)
				  pin_data->pin_case = IDLE_GSM_FAIL;
			  else
			  {
					pin_data->pin_case = PUK1_REQ;
			  }
			  display_info.TextId = TxtNotAcc;
			  break;
		   case NEW_PIN_FAIL:
			pin_data->pin_case = INPUT_NEW_PIN;
			 display_info.TextId = TxtNewPIN;
			  display_info.TextId2 = TxtCodeInc;
			 break;
		   case NEW_PIN2_FAIL:
			 pin_data->pin_case = INPUT_NEW_PIN2;
			 display_info.TextId = TxtNewPIN2;
			 display_info.TextId2 = TxtCodeInc;
			 break;
		   case PIN_OK_INFO:
			TRACE_FUNCTION("PIN_OK_INFO");
			  pin_data->pin_case = PIN_OK;
			  display_info.TextId = TxtPINOK;
			  break;		   
			case PUK1_UNBL_FAIL_INFO:
			  if((pin_data->set_state EQ CHANGE_PIN2) ||
				 (pin_data->set_state EQ PIN2_CHECK) ||
				  (pin_data->set_state EQ FDN_ACTIV) ||
				  (pin_data->set_state EQ FDN_DEACTIV))
			  {
					pin_data->pin_case = PUK2_REQ;

			  }
			  else if(pin_data->set_state EQ IDLE_GSM)
			  {
					pin_data->pin_case = IDLE_GSM_FAIL;

			  }
			  else
			  {
					pin_data->pin_case = PUK1_REQ;

			  }

			  display_info.TextId2 = TxtCodeInc;

			  break;
			case PUK1_UNBL_SUCC:
			  if((pin_data->set_state EQ CHANGE_PIN2) ||
				 (pin_data->set_state EQ PIN2_CHECK) ||
				  (pin_data->set_state EQ FDN_ACTIV) ||
				  (pin_data->set_state EQ FDN_DEACTIV))
				display_info.TextId = TxtNewPIN2;
			  else
				display_info.TextId = TxtNewPIN;
			  pin_data->pin_case = UNBL_OK;    
			  display_info.TextId2 = TxtChanged;
			  break;
			case PIN1_ENAB_END:
			  pin_data->pin_case = UNBL_OK;
			  pin_data->set_state = DEFAULT;
			display_info.TextId = TxtPinCode;
			  display_info.TextId2 = TxtActivated;
			  break;
			case PIN1_DISAB_END:
			  pin_data->pin_case = UNBL_OK;
			  pin_data->set_state = DEFAULT;
			  display_info.TextId = TxtPinCode;
			  display_info.TextId2 = TxtDeActivated;
			  break;
			case FDN_ACT_END:
			  pin_data->pin_case = FDN_UNBL_OK;
			  display_info.TextId = TxtActivated;
			  break;
			case FDN_DEACT_END:
			  pin_data->pin_case = FDN_UNBL_OK;
			  display_info.TextId = TxtDeActivated;
			  break;
			case FDN_FAIL:
			  pin_data->pin_case = UNBL_OK;
			  display_info.TextId = TxtFailed;
			  break;
			case PIN1_ENDIS_FAIL:
			  pin_data->pin_case = PIN1_REQ;
			  display_info.TextId = TxtPINFail;
			  if(pin_data->pin_retries EQ 1)
				display_info.TextId2 = TxtOneAttemptLeft;
			  else
				display_info.TextId2 = TxtTwoAttemptsLeft;
			  break;
#ifdef SIM_PERS         
 case NEW_SIM_PLOCK_FAIL:
              pin_data->pin_case = INPUT_NEW_SIM_PLOCK;
			  display_info.TextId = TxtPsLockConfWrong;
			  display_info.TextId2 = TxtPsLockConfwrong2;
			  break;
		case SIM_LOCK_NEW_ENDISFAIL:
				{
					 int max=0;
                                   int curr= 0;
			               char buf[20];
			               pin_data->pin_case = FAILURE_PASS_CHG;
			               mfw_simlock_check_status(mmi_simlock_locktype,&max,&curr);
					 if( max != 0xff)
					 {
			               	sprintf((char*)buf,"%d tries left",(curr));
					       display_info.TextId = TxtCodeInc;
			               	display_info.TextString2 = (char*)buf;
			               	display_info.Identifier = pin_data->pin_case;
					 }
					 else
					 	display_info.TextId = TxtCodeInc;
			
			}
			//display_info.TextId=TxtPassword;
			//display_info.TextId2 = TxtPassfailed;
			  break;
	      case SIM_LOCK_STATUS:
              	pin_data->pin_case = SIM_LOCK_STATUS;
          		switch(mfw_simlock_get_lock_status(mmi_simlock_locktype))
          		{
                          case MFW_SIM_DISABLE:display_info.TextId = TxtLockDisabled;break;
                          case  MFW_SIM_ENABLE:display_info.TextId = TxtLockEnabled; break;
                          case MFW_SIM_BLOCKED:display_info.TextId = TxtBlckPerm;break;
                          case  MFW_SIM_FAILURE:display_info.TextId = TxtFailed;break;
               	}
         		 break;

		  case SIM_LOCK_SIM_REM:
                 display_info.TextId=TxtNoCard;
                 break;
		  
            case SIM_LOCK_ENDIS_FAIL:
             if(pin_data->set_state EQ DISABLE)
             {
             		
			char buf[20];
			
	              status1 = mfw_simlock_check_status(mmi_simlock_locktype,&max,&curr);
			  if(status1 != MFW_SIM_BLOCKED && status1 != MFW_SIM_PERM_BLOCKED && ( max != 0xff))
			  {
			  	pin_data->pin_case = TXT_SIM_LOCK_ALL;
				dlg_initDisplayData_TextStr(&display_info,TxtNull,TxtNull,NULL,NULL, COLOUR_STATUS_PINS);
				TRACE_EVENT(" status is not blocked");
				TRACE_EVENT_P2("The current and max value are %d and %d",curr,max);
				sprintf((char*)buf,"%d tries left",(curr));
				display_info.TextId = TxtCodeInc;
				display_info.TextString2 = (char*)buf;
				display_info.Identifier = pin_data->pin_case;
			  }
			  else
			  	display_info.TextId = TxtCodeInc;
		
              //display_info.TextId = TxtPsLockWrong;
              }
             else
             	{
			display_info.TextId = TxtCodeInc;				
             	}
		break;

	//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	//Displays a dialog saying "Busy"
	case SIM_LOCK_BUSY:
		pin_data->pin_case = TXT_SIM_LOCK_ALL;
		display_info.TextId = TxtBusy;
		break;

		
	//x0pleela 26 July, 2006 ER: OMAPS00087586, OMAPS00087587
	//Displays a dialog saying "Busy" for wrong entry of unblocking password
	case SIM_LOCK_BUSY_UNBLOCK:
		pin_data->pin_case =PUK1_REQ ;
		display_info.TextId = TxtBusy;
		break;

	case SIM_LOCK_FCPWD_FAIL:
			  {
				char buf1[20], buf2[20] ;
				int curr_fail_reset, curr_succ_reset;
				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
				sprintf((char*)buf1,"%d attempts left",curr_fail_reset);
				sprintf((char*)buf2,"%s", "FC Reset-Failure");								
				display_info.TextString= buf2;
				display_info.TextString2 = buf1;
				  break;
			  }
			/*	display_info.TextId = TxtCodeInc;
				break;*/
	//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	//Displays a dialog saying "master unlock failed"
	case SIM_LOCK_MKPWD_FAIL:
			  {
				char buf1[25] ;
				sprintf((char*)buf1,"%s" , "Master Unlock Failed");
				display_info.TextString= buf1;
				display_info.TextString2 = 0;
				  break;
			  }
#endif


			case PIN1_STATUS:
			  pin_data->pin_case = PIN1_STATUS;



				TRACE_FUNCTION("set_pin_messages:check the pinrequest status");

				status.type = MFW_SIM_PIN1;//check the PIN1
				sim_pin_status(&status);

				if(status.set EQ MFW_SIM_DISABLE) /*Display the "Disabled Pin1 */
				{
					display_info.TextId = TxtDisabled;
				}
				else
				{
					display_info.TextId = TxtEnabled; /*Display the "Enabled Pin1 */
				}

		

			  display_info.TextId2 = 0;
			  display_info.TextString2 = 0;
			  break;

            /* Marcus: Issue 1609: 23/01/2003: Start */
            case PIN2_SIM_FAILURE:
                /*
                 * Failure to verify PIN2. Display
                 *  PIN2 Code
                 *  Not Available
                 * Then cancel back to the menu.
                 */
                pin_data->pin_case        = SETT_ABORT;
                display_info.TextId       = TxtPin2Code;
                display_info.TextId2      = TxtNotAvailable;
                display_info.TextString2 = 0;
                break;
            /* Marcus: Issue 1609: 23/01/2003: End */
			  
		 	}

#ifdef SIM_PERS 
    TRACE_EVENT_P1(" Status Bef: %d", status1);
    status1 = mfw_simlock_check_status(mmi_simlock_locktype,&max,&curr);
	    status2 = mfw_simlock_check_status(0,&max1,&curr1);
		TRACE_EVENT_P1("Status Aft: %d", status1);
		
  	if(((event==SIM_LOCK_NEW_ENDISFAIL)||(event == SIM_LOCK_ENDIS_FAIL)) && (curr == 0))
    {
      TRACE_EVENT("SIM_LOCK_ENDIS_FAIL,MFW_SIM_BLOCKED");
      pin_data->puk_request = TRUE;
      C_KEY_REQ=1;
		  FCUnlock_flag=1;
      set_pin_mess_destroy(mess_data->pin_mess_win);
     SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
    }
  	else if(status2 ==  MFW_SIM_PERM_BLOCKED)
    {
      TRACE_EVENT("MFW_SIM_PERM_BLOCKED");
      //pin_data->puk_request = TRUE;
      //C_KEY_REQ=1;
		  //FCUnlock_flag=1;
		  perm_blocked =1;
      set_pin_mess_destroy(mess_data->pin_mess_win);
      SEND_EVENT(pin_data->pin_win,PERM_BLK,0,0);
    }
    else
#endif  /*SIM_PERS*/
    {
	if (display_info.TextId2 == TxtOneAttemptLeft)
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)set_pin_info_cb, FIVE_SECS, KEY_LEFT );
	else
	  	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)set_pin_info_cb, THREE_SECS, KEY_LEFT );

	 display_info.Identifier = pin_data->pin_case;
	set_pin_mess_destroy(mess_data->pin_mess_win);

	info_dialog(pin_data->pin_win,&display_info); //information screen
	//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg  
	 dspl_Enable(1);
    }
break;
			
#ifdef SIM_PERS
	case PERM_BLK:
		TRACE_FUNCTION("Enter PERM_BLK");
		pin_data->display_id1 = TxtPhoneBlocked;
		pin_data->display_id2 = TxtDealer;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,ME_PERM_BLK,0,pin_data);
		break;


#endif 
	case PUK1_REQ:
		TRACE_FUNCTION("Enter PUK_REQ");
#ifdef SIM_PERS 
		if( C_KEY_REQ )
			pin_data->display_id1 = TxtUnblockCode;
		else
			
#endif
		pin_data->display_id1 = TxtEnterPuk1;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PUK1,0,pin_data);
		break;
	case PIN1_REQ:
		TRACE_FUNCTION(">>>TEXT ID CALLED");
		pin_data->display_id1 = TxtEnterPin1;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PIN1,0,pin_data);
		break;

#ifdef SIM_PERS        
       case TXT_SIM_LOCK_ALL:
       switch(mmi_simlock_locktype)
       {
       	case  MFW_SIM_NLOCK:
			pin_data->display_id1 = TxtEnterNLock;
       		break;
		case  MFW_SIM_SPLOCK:
			pin_data->display_id1 = TxtEnterSpLock;
			break;
		case  MFW_SIM_NSLOCK:
			pin_data->display_id1 = TxtEnterNsLock;
			break;
		case  MFW_SIM_CLOCK:
			pin_data->display_id1 = TxtEnterCLock;
			break;
		case  MFW_SIM_PLOCK:
			pin_data->display_id1 = TxtEnterPsLock;
			break;
	    //x0pleela 25 Sep, 2006 ER: OMAPS00095524			
	    //Prompting user to enter Blocked Network password
		case MFW_SIM_PBLOCK:
			pin_data->display_id1 = TxtEnterPbLock;
			break;
		}
 		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PLOCK,0,pin_data);
		break;

	case SIM_LOCK_REQ_FCPWD:
			pin_data->display_id1 = TxtPassword;
	 		set_pin_mess_destroy(mess_data->pin_mess_win);
			SEND_EVENT(pin_data->pin_win,TXT_ENTER_FCPWD,0,pin_data);
			break;
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	case SIM_LOCK_REQ_MKPWD:
			pin_data->display_id1 = TxtEnterMasterKey;
	 		set_pin_mess_destroy(mess_data->pin_mess_win);
			SEND_EVENT(pin_data->pin_win,TXT_ENTER_MKPWD,0,pin_data);
			break;
#endif
	case PUK2_REQ:
		pin_data->display_id1 = TxtEnterPuk2;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PUK2,0,pin_data);
		break;
	case PIN2_REQ:
		pin_data->display_id1 = TxtEnterPin2;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PIN2,0,pin_data);
		break;
	case INPUT_NEW_PIN:
#ifdef SIM_PERS
		if(C_KEY_REQ)
		{ 
		
			int sta;
			int curr_fail_reset, curr_succ_reset;
			
			
			mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

			if( curr_succ_reset )
			{
				if( FCUnlock_flag)
				    sta=mfw_simlock_reset_fc_value((U8*)pin_data->puk,MFW_SIM_FCM) ;
			   	else
			   	    sta=mfw_simlock_reset_fc_value((U8*)pin_data->puk,MFW_SIM_FC) ;

				mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

				TRACE_EVENT_P1("Input_new_pin: %d", sta);
				TRACE_EVENT_P1("set_pin_messages: C_KEY_REQ: %d", C_KEY_REQ);
				
				if(sta== MFW_SS_OK) 
				{
					if(FCUnlock_flag)
						FCUnlock_flag=0;

					C_KEY_REQ=0;
					TRACE_EVENT_P1("set_pin_messages: C_KEY_REQ: %d", C_KEY_REQ);
					TRACE_EVENT("set_pin_messages: SIM_LOCK_NEW_ENDSUCC_CKEY: ");
				    	pin_data->puk_request = FALSE;
					set_pin_mess_destroy(mess_data->pin_mess_win);
				    	SEND_EVENT(pin_data->pin_win,SIM_LOCK_NEW_ENDSUCC_CKEY,0,pin_data);
			    	}
			    else if(sta==MFW_SS_FAIL ) //UNBLOCK_FAILURE
			    	{
				    	TRACE_EVENT("set_pin_messages: SMLK_SHOW_FC_FAIL1: ");
			    		set_pin_mess_destroy(mess_data->pin_mess_win);


				//x0pleela 11 Sep, 2006  DR: OMASP00094215
				//Check for Ext_ERR_Busy and send the event accordingly to display a dialog "Busy"
					//for wrong entry of unblock code
					if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
					    	(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
					{
						SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY_UNBLOCK,0,pin_data);
					}
													 
			    		//x0pleela 27 Apr, 2006 DR: OMAPS00067919
			    		//To avoid executing this during bootup sequence
			    		else if( curr_fail_reset != 0)
						SEND_EVENT(pin_data->pin_win,SMLK_SHOW_FC_FAIL1,0,pin_data);

					else 
					{
						perm_blocked =1;
						SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
					}
				}
			}
		}
		else
#endif
    {
		    pin_data->display_id1 = TxtEnterNewPin;
		    set_pin_mess_destroy(mess_data->pin_mess_win);
		    SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PIN,0,pin_data);
			}
		break;
	case INPUT_NEW_PIN2:
		pin_data->display_id1 = TxtNewPIN2;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PIN2,0,pin_data);
		break;
	case INPUT_NEW_PIN_AGAIN:
		pin_data->display_id1 = TxtConfPin;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PIN_AGAIN,0,pin_data);
		break;
	case INPUT_NEW_PIN2_AGAIN:
		pin_data->display_id1 = TxtConfPin2;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PIN_AGAIN2,0,pin_data);
		break;
	case PIN1_REQ_OLD:
		pin_data->display_id1 = TxtOldPIN;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PIN1,0,pin_data);
		break;
	case PIN2_REQ_OLD:
		pin_data->display_id1 = TxtOldPIN2;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PIN2,0,pin_data);
		break;
	case FDN_ACTIVATED:
		pin_data->display_id1 = TxtPleaseWait;//TxtActivated;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,FDN_WAIT,0,pin_data);
		break;
	case ADN_ACTIVATED:
		pin_data->display_id1 = TxtPleaseWait;//TxtDeActivated;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,FDN_WAIT,0,pin_data);
		break;
#ifdef SIM_PERS
		 case SIM_LOCK_REQ_OLD:
              pin_data->display_id1 = TxtEnterOldPsLock;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PLOCK,0,pin_data);
		break;
               case SIM_LOCK_REQ_NEW:
                pin_data->display_id1 = TxtEnterPsLockNew;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_SIMLOCK,0,pin_data);
		break;
		  case INPUT_NEW_SIM_PLOCK_AGAIN:
              pin_data->display_id1 = TxtEnterPsLockconf;
		set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_SIMLOCK_AGAIN,0,pin_data);
              break;
#endif
	default:
		break;
  }
		
}


/*******************************************************************************

 $Function:	set_pin_info_cb
 $Description:	settings pin information screen call back
 $Returns:		void
 $Arguments:	win, identifier, reason
*******************************************************************************/
void set_pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;

	TRACE_FUNCTION("MmiPins: set_pin_info_cb");

	switch(identifier)
	{

		case PIN_OK:
			SEND_EVENT(pin_data->pin_win,PIN_OK_END,0,NULL);//PIN was needed
			break;
		case PIN1_STATUS:
			SEND_EVENT(pin_data->pin_win,PIN1_STATUS_END,0,NULL);
			break;
		case UNBL_OK:
		case PIN1_REQ:
		case PIN2_REQ:
		case PUK1_REQ:
		case PUK2_REQ:
		case INPUT_NEW_PIN:
		case INPUT_NEW_PIN2:
		case PIN1_CH_END:
		case PIN2_CH_END:
		case FDN_UNBL_OK:
		case PIN1_REQ_OLD:
		case PIN2_REQ_OLD:
		case IDLE_GSM_FAIL:
		case SETT_ABORT:        // Marcus: Issue 1609: 23/01/2003
			SEND_EVENT(pin_data->pin_win,identifier,0,NULL);//pin_main data !!
			break;
		default:		
		switch (reasons)
		{
			case INFO_KCD_LEFT:
			case INFO_KCD_RIGHT:
		
			case INFO_TIMEOUT:
#ifdef SIM_PERS
			{
			        pin_skClear();
				setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
			}
		
#else
			dspl_ClearAll();
#endif /* SIM_PERS*/
			break;
		}
			break;
	}

}

/*******************************************************************************

 $Function:	gsm_set_test 
 $Description:	test if gsm string
 $Returns:		status int
 $Arguments:	win, pin editor attributes 
*******************************************************************************/
static int gsm_set_test(T_MFW_HND win,void * edt_pin)
{
	int status;
	T_pin_edt * edt_data = (T_pin_edt *)edt_pin;


//	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;    /* Warning Correction */
	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

	TRACE_FUNCTION("MmiPins:gsm_set_test");

	status = ss_check_ss_string((UBYTE*)edt_data->pin);/* check GSM-String */
	switch(status)
	{
		case MFW_SS_SIM_REG_PW:
				if(!(edt_data->gsm_state))
				{
					switch(pin_data->set_state)
					{
						case CHANGE_PIN:/* replace * with string id */
							if(!(strncmp((char*)edt_data->pin,"**04*",IDENT_GSM_1)))
							{
								edt_data->gsm_state = TRUE;
								strncpy((char*)edt_data->edtbuf,(char*)edt_data->pin,edt_data->editor_index-1);
								/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
								ATB_edit_Refresh(edt_data->editor);  /* String has changed, refresh editor */
								ATB_edit_Show(edt_data->editor);	/* Show the editor */
#else /* NEW_EDITOR */
								edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */

							}
							else
							{
								edt_data->gsm_state = FALSE;
								dspl_ClearAll();
								clear_edit_array (edt_data);
							    set_pin_edt_destroy(edt_data->pin_edt_win);
								if(pin_data->puk_request)
								{
									pin_data->pin_case = PUK1_REQ;
								}
								else
								{
									if((pin_data->set_state EQ ENABLE) || (pin_data->set_state EQ DISABLE))
										pin_data->pin_case = PIN1_REQ;
									else
										pin_data->pin_case = PIN1_REQ_OLD;/* pin1/2 change */
								}
								SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							}
							break;
						case CHANGE_PIN2:/* replace * with string id */
							if(!(strncmp((char*)edt_data->pin,"**042*",IDENT_GSM_2)))
							{
								edt_data->gsm_state = TRUE;
								strncpy((char*)edt_data->edtbuf,(char*)edt_data->pin,edt_data->editor_index-1);
								/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
								ATB_edit_Refresh(edt_data->editor);  /* String has changed, refresh editor */
								ATB_edit_Show(edt_data->editor);	/* Show the editor */
#else /* NEW_EDITOR */
								edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */
							}
							else
							{
								edt_data->gsm_state = FALSE;
								dspl_ClearAll();
								clear_edit_array (edt_data);
							    set_pin_edt_destroy(edt_data->pin_edt_win);
								if(pin_data->puk_request)
								{
									pin_data->pin_case = PUK2_REQ;
								}
								else
								{
									pin_data->pin_case = PIN2_REQ_OLD;
								}
								SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							}
							break;
						default:
							edt_data->gsm_state = FALSE;
							dspl_ClearAll();
							clear_edit_array (edt_data);
							set_pin_edt_destroy(edt_data->pin_edt_win);
							if(pin_data->puk_request)
							{
								if(pin_data->set_state EQ CHANGE_PIN2)
									pin_data->pin_case = PUK2_REQ;
								else
									pin_data->pin_case = PUK1_REQ;
							}
							else
							{
								if(pin_data->set_state EQ CHANGE_PIN)
									pin_data->pin_case = PIN1_REQ_OLD;
								else if(pin_data->set_state EQ CHANGE_PIN2)
									pin_data->pin_case = PIN2_REQ_OLD;
								else
									pin_data->pin_case = PIN1_REQ;/* PIN enable/disable */
							}
							SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							break;
					}
				}
			return status;

		case MFW_SS_SIM_UNBLCK_PIN:
				if(!(edt_data->gsm_state))
				{
					switch(edt_data->pin_case_edit)
					{
						case INPUT_PUK1:/* replace * with string id */
							if(!(strncmp((char*)edt_data->pin,"**05*",IDENT_GSM_1)))
							{
								edt_data->gsm_state = TRUE;
								strncpy((char*)edt_data->edtbuf,(char*)edt_data->pin,edt_data->editor_index-1);
								/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
								ATB_edit_Refresh(edt_data->editor);  /* String has changed, refresh editor */
								ATB_edit_Show(edt_data->editor);	/* Show the editor */
#else /* NEW_EDITOR */
								edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */
							}
							else
							{
								edt_data->gsm_state = FALSE;
								dspl_ClearAll();
								clear_edit_array (edt_data);
							    set_pin_edt_destroy(edt_data->pin_edt_win);
								pin_data->pin_case = PUK1_REQ;
								SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							}
							break;
						case INPUT_PUK2:/* replace * with string id */
							if(!(strncmp((char*)edt_data->pin,"**052*",IDENT_GSM_2)))
							{
								edt_data->gsm_state = TRUE;
								strncpy((char*)edt_data->edtbuf,(char*)edt_data->pin,edt_data->editor_index-1);
								/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
								ATB_edit_Refresh(edt_data->editor);  /* String has changed, refresh editor */
								ATB_edit_Show(edt_data->editor);	/* Show the editor */
#else /* NEW_EDITOR */
								edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */
							}
							else
							{
								edt_data->gsm_state = FALSE;
								dspl_ClearAll();
								clear_edit_array (edt_data);
							    set_pin_edt_destroy(edt_data->pin_edt_win);
								pin_data->pin_case = PUK2_REQ;
								SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							}
							break;
						default:
							edt_data->gsm_state = FALSE;
							dspl_ClearAll();
							clear_edit_array (edt_data);
							set_pin_edt_destroy(edt_data->pin_edt_win);
							if(pin_data->puk_request)
							{
								if(pin_data->set_state EQ CHANGE_PIN2)
									pin_data->pin_case = PUK2_REQ;
								else
									pin_data->pin_case = PUK1_REQ;/* pin1 change,disable,enable */
							}
							else
							{
								if(pin_data->set_state EQ CHANGE_PIN)
									pin_data->pin_case = PIN1_REQ_OLD;
								else if(pin_data->set_state EQ CHANGE_PIN2)
									pin_data->pin_case = PIN2_REQ_OLD;
								else
									pin_data->pin_case = PIN1_REQ; /* pin enable/disable */
							}
							SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
							break;
					}
				}
			return status;
		case MFW_SS_DIAL:	       /* string not yet detected  */
        case MFW_SS_USSD:  /*JVJ SPR 1040- The short USSD strings should also be 
                              considered in this case, since they can be confused with dialled numbers  
                             */
				edt_data->gsm_state = FALSE;
			return status;
		default:
			edt_data->gsm_state = FALSE;
			dspl_ClearAll();
			clear_edit_array (edt_data);
			set_pin_edt_destroy(edt_data->pin_edt_win);
			SEND_EVENT(pin_data->pin_win,GSM_FAIL,0,NULL);/*entering screeen not allowed */
			return status;
	}
}


/*******************************************************************************

 $Function:	set_mode_fdn_adn
 $Description:	activate/deactivate FDN/ADN
 $Returns:		none
 $Arguments:	win, pin editor attributes
*******************************************************************************/
static void set_mode_fdn_adn(T_MFW_HND win, void * string)
{
	T_pin * pin_data = (T_pin *)string;

    TRACE_FUNCTION("MmiPins:set_mode_fdn_adn");

	if((pin_data->set_state EQ FDN_ACTIV) || (pin_data->set_state EQ PIN2_CHECK))
	{
		if(phb_set_mode(PHB_FDN,pin_data->old_pin) EQ MFW_PHB_OK)
		{
			SEND_EVENT(pin_data->pin_win,FDN_ACTIVATED,0,pin_data);/* activate FDN successfull */
		}
		else
		{
			SEND_EVENT(pin_data->pin_win,FDN_FAIL,0,pin_data);/* activate FDN unsuccessfull */
		}
	}
	else
	{
		if(phb_set_mode(PHB_ADN,pin_data->old_pin) EQ MFW_PHB_OK)
		{
			SEND_EVENT(pin_data->pin_win,ADN_ACTIVATED,0,pin_data);/* deactivate FDN  successfull */
		}
		else
		{
			SEND_EVENT(pin_data->pin_win,FDN_FAIL,0,pin_data);/* deactivate FDN unsuccessfull */
		}
	}
}


/*******************************************************************************

 $Function:	backpinFDNactdeact
 $Description:	when callback event is received from phonebook
		(MmiPhbk:phbkEvent)
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
void backpinFDNactdeact(void)
{

	T_MFW_WIN * win_data = ((T_MFW_HDR *)set_pin_windows)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	TRACE_FUNCTION("MmiPins.backpinFDNactdeact");

	if(pin_data->set_state EQ FDN_ACTIV)
	{
		SEND_EVENT(pin_data->pin_win,FDN_ACT_END,0,pin_data);
	}
	else if(pin_data->set_state EQ FDN_DEACTIV)
	{
		SEND_EVENT(pin_data->pin_win,FDN_DEACT_END,0,pin_data);
	}
}



/*******************************************************************************

 $Function:	pin2_check 
 $Description:	PIN2 check :call from menu Services and Phonebook
 $Returns:		status int
 $Arguments:	parent window
*******************************************************************************/
int pin2_check (T_MFW_HND parent_window)
{
		T_MFW_HND win = setting_pin_create(parent_window);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;
		T_MFW_SIM_PIN_STATUS status;

		if(win != NULL) /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
	
//		T_DISPLAY_DATA display_info;
		
	
		pin_data->set_state = PIN2_CHECK;

	TRACE_FUNCTION("MmiPins:pin2_check");

	status.type = MFW_SIM_PIN2;
		sim_pin_status(&status);
	       // June 16, 2005  REF: CRR 31267  x0021334
	       // Check if PIN2 and PUK2 are initialised
		if((sim_pin_count(MFW_SIM_PIN2) > 0) AND (sim_pin_count(MFW_SIM_PUK2) NEQ 0))
		{
			SEND_EVENT(pin_data->pin_win,PIN2_REQ,0,pin_data);
		}
		// If PIN2 is not initialised, ask for PUK 2
		else if ((sim_pin_count(MFW_SIM_PIN2) EQ 0) AND (sim_pin_count(MFW_SIM_PUK2) > 0))
		{
			pin_data->puk_request = TRUE;
			SEND_EVENT(pin_data->pin_win,PUK2_REQ,0,pin_data);
		}
		// Else display that PIN2 is not available
		else
		{
			SEND_EVENT(pin_data->pin_win,PIN2_SIM_FAILURE,0,pin_data);
		}
		}
		return 1;
}



	
/*******************************************************************************

 $Function:	pin1_check 
 $Description:	PIN1 check :call from menu Services (AOC)
 $Returns:		status int
 $Arguments:	parent window
*******************************************************************************/
int pin1_check (T_MFW_HND parent_window)
{
		T_MFW_HND win = setting_pin_create(parent_window);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;

		if(win != NULL) /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
//		T_MFW_SIM_PIN_STATUS status;  /* Warning Correction */

		pin_data->set_state = PIN1_CHECK;

	TRACE_FUNCTION("MmiPins:pin1_check");
		if(sim_pin_count(MFW_SIM_PIN1) > 0)
		{
			SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
		}
		else
		{   /* not included the state of the puk counter ! */
			pin_data->puk_request = TRUE;
			SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
		}
		}
		return 1;
}

/*******************************************************************************

 $Function:	fdnActivate
 $Description:	activate FDN
 $Returns:		status int
 $Arguments:	menu, item
*******************************************************************************/
int fdnActivate(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
	TRACE_FUNCTION("MmiPins:fdnActivate");
	pin2_fdn_activate();/*	check if PIN2 */
    return 1;
}



/*******************************************************************************

 $Function:	fdnDeactivate 
 $Description:	deactivate FDN
 $Returns:		status int
 $Arguments:	menu, item
*******************************************************************************/
int fdnDeactivate(struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{

	TRACE_FUNCTION("MmiPins:fdnDeactivate");
    pin2_fdn_deactivate(); //	check if PIN2
    return 1;
}


/*******************************************************************************

 $Function:	set_fdn_on_item_flag 
 $Description:	decision about menu entry FDN "on"
 $Returns:		0 if show entry, 1 if not
 $Arguments:	menu, menu attributes, item
*******************************************************************************/
U16 set_fdn_on_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{

    TRACE_FUNCTION("MmiPins:set_fdn_on_item_flag");

	if(phb_get_mode()EQ PHB_RESTRICTED) /*if FDN on */
		return MNU_ITEM_HIDE;		/* show no entry "on" */
	else
		return 0;		       /* show entry "on"  */
}


/*******************************************************************************

 $Function:	set_fdn_off_item_flag
 $Description:	decision about menu entry FDN "off"
 $Returns:		0 if show entry off, 1 if not
 $Arguments:	menu, menu attributes, item
*******************************************************************************/
U16 set_fdn_off_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi)
{
   TRACE_FUNCTION("MmiPins:set_fdn_off_item_flag");

   if(phb_get_mode() NEQ PHB_RESTRICTED) /* if FDN off */
		return MNU_ITEM_HIDE;		 /* show no entry "off" */
	else
		return 0;			 /* show entry "off"  */
}



/*******************************************************************************

 $Function:	gsm_idle 
 $Description:	handle gsm string in idle screen (unblock and change pin)
 $Returns:		Status int
 $Arguments:	parent window, string
*******************************************************************************/
int gsm_idle (T_MFW_HND parent_window,char * string)
{
		T_MFW_HND win = setting_pin_create(parent_window);
		T_MFW_WIN * win_data = NULL;
		T_pin * pin_data = NULL;

		if(win != NULL) /* x0039928 - Lint warning removal */
		{
		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;

		pin_data->set_state = IDLE_GSM;
		pin_data->gsm_state = TRUE;

	TRACE_FUNCTION("MmiPins:gsm_idle");

		gsm_sec_execute_set(win,string);
		}
		return 1;
}


/*******************************************************************************

 $Function:	gsm_sec_execute_set
 $Description:	GSM-String will be executed in settings and idle
 $Returns:		none
 $Arguments:	win, pin attributes
*******************************************************************************/
static void gsm_sec_execute_set(T_MFW_HND win,void *string)
{
	    T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
		T_pin * pin_data = (T_pin *)win_data->user;
		int result;

	TRACE_FUNCTION("MmiPins:gsm_sec_execute_set");

		if(pin_data->set_state NEQ IDLE_GSM)
		{
			T_pin_edt * edt_data = (T_pin_edt *)string;

			result = ss_execute_transaction((UBYTE*)edt_data->pin,0);/* ussd_man = 0 while SSD */
			dspl_ClearAll();
			clear_edit_array (edt_data);
			pin_edt_destroy(edt_data->pin_edt_win);
			//xrashmic 1 Jul, 2006 OMAPS00075784
			//After the PUK is verified, security menu has to be displayed. 
			//Hence we need to delete the pin window or else the blue  
			//screen is displayed
			//xrashmic 6 Jul, 2006 OMAPS00080708
			//The pin window is necessary for displaying the error message for MFW_SS_FAIL
			if(result!=MFW_SS_FAIL)
			pin_destroy(pin_data->pin_win);		
		}
		else
			result = ss_execute_transaction((UBYTE*)string,0);/* ussd_man = 0 while SSD */

		switch(result)
		{
			case MFW_SS_SIM_REG_PW:     /* change PIN1/2		*/
				TRACE_FUNCTION("MmiPins:gsm_idle:MFW_SS_SIM_REG_PW");
				break;
			case MFW_SS_SIM_UNBLCK_PIN: /* unblock PIN1/2		*/
				TRACE_FUNCTION("MmiPins:gsm_idle:MFW_SS_SIM_UNBLCK_PIN");
				break;
			case MFW_SS_FAIL:
				TRACE_FUNCTION("MmiPins:gsm_idle:MFW_SS_FAIL");
				sett_pin_mess(win,GSM_FAIL);/* new PIN and new PIN again are not the same */
				break;
			default:
				break;
		}
}


/*******************************************************************************

 $Function:	pins_editor_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void pins_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_pin	      * data	 = (T_pin *)win_data->user;

	T_pin * pin_data = (T_pin *)win_data->user;//pin main data
	//T_DISPLAY_DATA display_info;
//x0pleela 21 Apr, 2006  DR: OMAPS00067919
#ifdef SIM_PERS
	int ret; //used to check the retun type of function mfw_simlock_check_lock_bootup()
#endif
	TRACE_FUNCTION ("pins_editor_cb()");

	/* SPR#1746 - SH - In the case INSERT_CARD or CARD_REJECTED,
	 * the editor has not been destroyed at this point.
	 * BUT in all other cases it has, so we need to set the
	 * editor handle to NULL. */
	 
// Nov 24, 2005, a0876501, DR: OMAPS00045909
	if (Identifier!=INSERT_CARD && Identifier!=CARD_REJECTED 
		&& Identifier!=SHOW_IMEI_INVALID		
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS			
		//x0pleela 30 Aug, 2006 DR: OMAPS00091250
		&& Identifier != NO_MEPD_DATA
#endif
		)
	{
		pin_editor_window = NULL;
	}

	switch (reason)
	{

		case INFO_EMERGENCY:		
			TRACE_EVENT_P1(" call %s", data->edtbuf);	

			/* SPR#1746 - SH - Function modified to provide context */
			show_confirmation (win, Identifier);

			break;
			
		case INFO_KCD_LEFT:
			switch (Identifier)
			{

				case PIN1_REQ:
				{	TRACE_EVENT ("PIN1_REQ");			

		    // verification pin1
					sim_verify_pin(MFW_SIM_PIN1, data->edtbuf);  
				}
					break;
#ifdef SIM_PERS
					case PERM_BLK:
						break;
#endif
				case PUK1_REQ:					
				{
#ifdef SIM_PERS
					int status,curr_fail_reset, curr_succ_reset;
					
					TRACE_FUNCTION ("PUK1-REQ");
					strncpy((char*)pin_data->puk, data->edtbuf,MAX_PIN);  
					status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
//	Nov 03, 2005 DR: OMAPS00050595 - xpradipg
//	the check for C_KEY_REQ is made first to find if the PUK request was for 
//	SIM Personalization or for the SIM PIN1
					
			if(C_KEY_REQ)
			{
				if((curr_fail_reset!=0) && (curr_succ_reset!=0) )
				{
						if( FCUnlock_flag)
						{
							TRACE_FUNCTION("FCUnlock_flag: TRUE");
							status= mfw_simlock_reset_fc_value((U8*)pin_data->puk,MFW_SIM_FCM);
						}
						else
						{
							TRACE_FUNCTION("FCUnlock_flag: FALSE");
                            
						       // June 07, 2006    DR: OMAPS00080701   x0021334
                                                 // Descripton: Phone hangs while unblocking SIMP during bootup.
                                                 // Assign 'SIMP_BOOTUP' to 'sim_unlock_in_prog' to indicate that
                                                 // the operation is happeneing during boot-up sequence. 
						       sim_unlock_in_prog = SIMP_BOOTUP;
							status= mfw_simlock_reset_fc_value((U8*)pin_data->puk,MFW_SIM_FC);
						}

						mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);
						if( status != MFW_SS_OK)
						{
							TRACE_FUNCTION("UNBLOCK_FAILURE");
							mmi_simlock_aciErrDesc = aciErrDesc;

							if((aciErrDesc & 0x0000FFFF) EQ CME_ERR_SimNotIns)
						     	 SEND_EVENT(pin_data->pin_win,SIM_LOCK_SIM_REM,0,pin_data);
	
							//x0pleela 11 Sep, 2006  DR: OMASP00094215
							//Handling CME error busy while unlocking a locked category
							else if( ( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext) AND
									(aciErrDesc & 0x0000FFFF) EQ EXT_ERR_Busy)
							  SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY_UNBLOCK,0,pin_data);

							else
							{
								//x0pleela 27 Apr, 2006 DR: OMAPS00067919
						    		//To avoid executing this during bootup sequence
						    		if(FCUnlock_flag) //x0pleela 27 Apr, 2006 DR: OMAPS00067919
						    		{
									if( curr_fail_reset )
										SEND_EVENT(pin_data->pin_win, SMLK_SHOW_FC_FAIL, 0, pin_data);
									else
									{
										perm_blocked=1;
										SEND_EVENT(pin_data->pin_win, PERM_BLK, 0, pin_data);
									}
								}
							}
						}
					 	else 
						{
							TRACE_FUNCTION("UNBLOCK_SUCESS");
							if(FCUnlock_flag)
								FCUnlock_flag=0;
							//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
							//Update mmi_simlock_aciErrDesc with the latest CME error
							mmi_simlock_aciErrDesc = aciErrDesc;

							if( curr_succ_reset )
								SEND_EVENT(pin_data->pin_win, SMLK_SHOW_FC_SUCC, 0, pin_data);
							else
							{
								perm_blocked=1;
								SEND_EVENT(pin_data->pin_win, PERM_BLK, 0, pin_data);
							}
						}
					}
					else
					{
						perm_blocked = 1;
						SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
					}

				}
			else
			{
#endif			
				strncpy((char*)pin_data->puk, data->edtbuf,MAX_PIN);  
				SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN,0,pin_data);
#ifdef SIM_PERS
			}
#endif				
		}
					break;

#ifdef SIM_PERS
				//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
				//Performs master unlock and sends corresponding events during bootup
				case INPUT_MASTER_KEY_REQ:
				{
					int status;
					strncpy((char*)pin_data->mk_pin, data->edtbuf,MAX_PIN);  
					status = mfw_simlock_master_unlock((U8*)pin_data->mk_pin, MFW_SIM_MKEY);
					if( status EQ MFW_SS_OK )
						SEND_EVENT(pin_data->pin_win, SMLK_SHOW_MK_SUCC, 0, pin_data);

					//x0pleela 13 Nov, 2006 DR: OMAPS00103356
					//send wrong password event 
					else if ( status EQ MFW_WRONG_PWD) 
						SEND_EVENT(pin_data->pin_win, SMLK_SHOW_WRONG_PWD, 0, pin_data);
					//x0pleela 13 Nov, 2006 DR: OMAPS00103356
					//send event which says all the dependent categories are unlocked
					else if ( status EQ MFW_MASTER_OK) 
						SEND_EVENT(pin_data->pin_win, SMLK_SHOW_DEP_CATS_UNLOCKED, 0, pin_data);
					else
						SEND_EVENT(pin_data->pin_win, SMLK_SHOW_MK_FAIL, 0, pin_data);
					
				}
				break;

				case INPUT_SIM_PIN_REQ:
				case INPUT_NLOCK_PIN_REQ:
				case INPUT_NSLOCK_PIN_REQ:
				case INPUT_SPLOCK_PIN_REQ:
				case INPUT_CLOCK_PIN_REQ: 
				//x0pleela 25 Sep, 2006 ER: OMAPS00095524
				case INPUT_PBLOCK_PIN_REQ:
				{
					S16 lcktype = 0;
				int curr_fail_reset1, curr_succ_reset1;

				mfw_simlock_check_status(0, &curr_fail_reset1, &curr_succ_reset1);

					if ((!curr_fail_reset1)||(!curr_succ_reset1))
					{
						perm_blocked =1;
						SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
					}	
					else
					{
					
						strncpy((char*)pin_data->puk, data->edtbuf,MAX_PIN);  
						//for CPIN to CLCK change
						switch(Identifier)
						{
							case INPUT_SIM_PIN_REQ:
								lcktype=MFW_SIM_PLOCK;
								break;
							case INPUT_NLOCK_PIN_REQ:
								lcktype=MFW_SIM_NLOCK;
								break;
							case INPUT_NSLOCK_PIN_REQ:
								lcktype=MFW_SIM_NSLOCK;
								break;
							case INPUT_SPLOCK_PIN_REQ:
								lcktype=MFW_SIM_SPLOCK;
								break;
							case INPUT_CLOCK_PIN_REQ: 
								lcktype=MFW_SIM_CLOCK;
								break;

							//x0pleela 25 Sep, 2006 ER: OMAPS00095524
							case INPUT_PBLOCK_PIN_REQ:
								lcktype=MFW_SIM_PBLOCK;
								break;								
						}
						ret = mfw_simlock_check_lock_bootup((char*)pin_data->puk, lcktype);
						//x0pleela 20 july, 2006 ER: OMAPS00087586, OMAPS00087587
						//delete the simp list window handler
						win_delete(pin_data->simp_list_win);
						if( ret == BOOTUP_LOCK_SUCCESS)
						{
							TRACE_FUNCTION("BOOTUP_LOCK_SUCCESS");
							simp_cpin_flag = FALSE;
							SEND_EVENT(pin_data->pin_win,SIM_LOCK_PERS_CHK_OK,0,pin_data);
						}
						else if (ret ==BOOTUP_OK) 
						{
						  sim_simlock_cpin_code (simp_cpin_code);
						}
						else
						{
							TRACE_FUNCTION("BOOTUP_LOCK_FAILURE");
							TRACE_EVENT_P1("aciErrDesc %d ",aciErrDesc);
							if( simp_cpin_flag )
							 sim_simlock_cpin_code (simp_cpin_code);
							else
							{
								//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
								//get the busy state
								if( mfw_simlock_get_busy_state() )
								{
									//send event to display a dialog Busy for the user
									SEND_EVENT(pin_data->pin_win,SIM_LOCK_BUSY_BOOTUP,0,pin_data);
								}
								    //x0pleela 25 Sep, 2006 ER: OMAPS00095524	
								    //check for CME class and handle all CME errors
								else if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Cme)
								{
									switch(aciErrDesc & 0x0000FFFF)
									{
										case CME_ERR_NetworkPersPinReq:
											TRACE_EVENT("CME_ERR_NetworkPersPinReq");
											//x0pleela 20 July, 2006
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options Network unlock and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												mmi_simlock_locktype = MFW_SIM_NLOCK;
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ,0,pin_data);
										break;
											
										case CME_ERR_NetworkSubsetPersPinReq:
											TRACE_EVENT("CME_ERR_NetworkSubsetPersPinReq");
											//x0pleela 20 July, 2006
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options Network subset unlock and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												mmi_simlock_locktype = MFW_SIM_NSLOCK;
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ,0,pin_data);
										break;
											
										case CME_ERR_ProviderPersPinReq:
											TRACE_EVENT("CME_ERR_ProviderPersPinReq");
											//x0pleela 20 July, 2006
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options service provider unlock and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												mmi_simlock_locktype = MFW_SIM_SPLOCK;
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ,0,pin_data);
										break;
											
										case CME_ERR_CorporatePersPinReq:
											TRACE_EVENT("CME_ERR_CorporatePersPinReq");
											//x0pleela 20 July, 2006
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options Corporate unlock and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												mmi_simlock_locktype = MFW_SIM_CLOCK;
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ,0,pin_data);
										break;
											
										case CME_ERR_PhSimPinReq:
											TRACE_EVENT("CME_ERR_PhSimPinReq");
											//x0pleela 20 July, 2006
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options SIM unlock and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												mmi_simlock_locktype = MFW_SIM_PLOCK;
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ,0,pin_data);
										break;

										case CME_ERR_PhoneFail:
										case CME_ERR_NetworkPersPukReq:
										case CME_ERR_NetworkSubsetPersPukReq:
										case CME_ERR_ProviderPersPukReq:
										case CME_ERR_CorporatePersPukReq:
										{
											int status, curr_fail_reset, curr_succ_reset;
											TRACE_EVENT("PUK Req");
											status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

											if( status==MFW_SIM_BLOCKED)
											{
												C_KEY_REQ = 1;
												//x0pleela 20 July, 2006
												//check for Master Unlock option enabled. 
												//If so, then display the menu with options Unblock ME and Master Unlock	
											if( simlock_get_masterkey_status())
											{
											  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
											  }
									  		else
												SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
											}
											else if (status == MFW_SIM_PERM_BLOCKED)
											{
												perm_blocked =1;
												SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
											}	
										}
										break;	


										case CME_ERR_WrongPasswd:
										default:
											TRACE_EVENT("CME_ERR_WrongPasswd");
											SEND_EVENT(pin_data->pin_win,Identifier,0,pin_data);
											break;
									}
							}
							    //x0pleela 25 Sep, 2006 ER: OMAPS00095524	
							    //check for Ext class and handle all Extension errors
							else if( ((aciErrDesc & 0xFFFF0000) >> 16) EQ ACI_ERR_CLASS_Ext)
							{
								switch(aciErrDesc & 0x0000FFFF)
								{
									//Request for Blocked Network Password
									case EXT_ERR_BlockedNetworkPersPinReq:
										TRACE_EVENT("EXT_ERR_BlockedNetworkPersPinReq");
										  if( simlock_get_masterkey_status())
										  {
										  	mmi_simlock_locktype = MFW_SIM_PBLOCK;
										  	pin_data->simp_list_win = mmi_simlock_category_menus(win);
										  }
										  else
											SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ,0,pin_data);
									break;

									//Request for Blocked Network Unblock code 
									case EXT_ERR_BlockedNetworkPersPukReq:
									{
										int status,curr_fail_reset, curr_succ_reset;
										TRACE_EVENT("PUK Req");
										status = mfw_simlock_check_status(0, &curr_fail_reset, &curr_succ_reset);

										if( status==MFW_SIM_BLOCKED)
										{
											C_KEY_REQ = 1;
											//check for Master Unlock option enabled. 
											//If so, then display the menu with options Unblock ME and Master Unlock	
											if( simlock_get_masterkey_status())
											{
												pin_data->simp_list_win = mmi_simlock_category_menus(win);
											}
											else
												SEND_EVENT(pin_data->pin_win,PUK1_REQ,0,pin_data);
										}
										else if (status == MFW_SIM_PERM_BLOCKED)
										{
											perm_blocked =1;
											SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
										}	
									}
									break;	

									default:
										break;
								}
							}
					   	}	
					}
				}
			}
				break;
#endif
					
				case INPUT_NEW_PIN:	
				{	TRACE_EVENT ("INPUT_NEW_PIN");	
					strncpy((char*)pin_data->new_pin, data->edtbuf,MAX_PIN);  
					SEND_EVENT(pin_data->pin_win,INPUT_NEW_PIN_AGAIN,0,pin_data);
				}
					break;

				case INPUT_NEW_PIN_AGAIN:
				{	//if new pins match
					if(strcmp((char*)pin_data->new_pin, (char*)data->edtbuf) EQ 0)
						sim_unblock_pin(MFW_SIM_PUK1, (char*)pin_data->puk, (char*)pin_data->new_pin);/* unblock pin1 */
					else	//otherwise ask for new pin again
						SEND_EVENT(pin_data->pin_win,NEW_PIN_FAIL,0,NULL);
				}
					break;
				default:
					break;
			}
			break;
					
	    case INFO_KCD_RIGHT:
	    case INFO_KCD_CLEAR:

			TRACE_EVENT ("INFO_KCD_RIGHT pressed");

			switch (Identifier)
			{

				default:
					break;
			}
    default:
			break;
	}
}


/*******************************************************************************

 $Function:	call emergency confirmation

 $Description:	 
 
 $Returns:		

 $Arguments:	
				
*******************************************************************************/

/* SPR#1746 - SH - Add 'Identifier' so previous context is known */

void show_confirmation (T_MFW_HND win, USHORT Identifier)
{
    T_DISPLAY_DATA   display_info;
	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtSoftBack, TxtSoftCall, TxtEmergency , COLOUR_STATUS_PINS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)show_confirmation_cb, FOREVER, KEY_HUP | KEY_LEFT| KEY_RIGHT );

	/* SPR#1746 - SH - Store Identifier in display info, so it can be accessed by callback */
	display_info.Identifier = Identifier;
      /*
       * Call Info Screen
       */

    info_dialog (win, &display_info);

}


int mmiPinsEmergencyCall(void)
{
	return((int)pin_emergency_call);
}

void mmiPinsResetEmergencyCall(void)
{
	pin_emergency_call = FALSE;
	return;
}

/*******************************************************************************

 $Function:	idle_imei_info_cb

 $Description:	 
 
 $Returns:		

 $Arguments:	
				
*******************************************************************************/



static int show_confirmation_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{

	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_pin	      * data	 = (T_pin *)win_data->user;

    switch (reason)	 
   {

	case INFO_KCD_LEFT:
			if (mmiStart_animationComplete() != TRUE)
				pin_emergency_call = TRUE;

			// call emergency number
			callNumber ((UBYTE*)data->edtbuf);			
	  break;
	  
	case INFO_KCD_HUP:
	case INFO_KCD_RIGHT:
		pin_emergency_call = FALSE;

		/* SPR#1746 - SH - Now use identifier to return to previous context.
		 * If the editor handle is not NULL, the editor is still hanging around.
		 * We want to restart the editor, so we'll destroy it first. */
		 
		if (pin_editor_window)
		{
#ifdef NEW_EDITOR
			AUI_pin_Destroy(pin_editor_window);
#else
			editor_destroy(pin_editor_window);
#endif
			pin_editor_window = NULL;
		}

		/* SPR#1746 - SH - For most identifiers, the number can just be passed to pin_main as
		 * an event.  However, INSERT_CARD and CARD_REJECTED actually
		 * correspond to the pin_main events NO_SIM_CARD and INVALID_CARD
		 * respectively.  Make this conversion */
		if (identifier==INSERT_CARD)
			identifier = NO_SIM_CARD;
		else if (identifier==CARD_REJECTED)
			identifier = INVALID_CARD;
// Nov 24, 2005, a0876501, DR: OMAPS00045909
		else if (identifier == SHOW_IMEI_INVALID)
			identifier = INVALID_IMEI;
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Added the following code inside the SIM_PERS flag
#ifdef SIM_PERS			
				//x0pleela 30 Aug, 2006 DR: OMAPS00091250
		else if( identifier == NO_MEPD_DATA )
			identifier = NO_MEPD;
#endif

		/* Restart the appropriate editor */
		SEND_EVENT(win, identifier, 0, data);		
		break;
    }

  return 1;
}
/*******************************************************************************

 $Function:	pins_loadEditDefault

 $Description:	fill up editor-sttribut with default
 
 $Returns:		
 
 $Arguments:		
				
*******************************************************************************/
//GW-SPR#844 - Zero editor data structure to reset all values to a known state.

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
void pins_loadEditDefault (T_AUI_EDITOR_DATA *editor_data, USHORT TitleId, USHORT Identifier)
{
		TRACE_FUNCTION ("pins_loadEditDefault()");
		AUI_edit_SetDefault(editor_data);
		AUI_edit_SetDisplay(editor_data, PASSWORD_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
		AUI_edit_SetMode(editor_data, ED_MODE_HIDDEN, ED_CURSOR_UNDERLINE);
		AUI_edit_SetEvents(editor_data, Identifier, TRUE, FOREVER, (T_AUI_EDIT_CB)pins_editor_cb);
		if (pin1Flag EQ TRUE)   // June 16, 2005  REF: CRR 31267  x0021334
			AUI_edit_SetTextStr(editor_data, TxtSoftOK, TxtDelete, TitleId, NULL); //display RSK as delete
		else																  
			AUI_edit_SetTextStr(editor_data, TxtSoftOK, TxtSoftBack, TitleId, NULL); // display RSK as back
		AUI_edit_SetAltTextStr(editor_data, 4, TxtNull,  TRUE, TxtNull);

		return;
}
#else /* NEW_EDITOR */
void pins_loadEditDefault (T_EDITOR_DATA *editor_data)
{
		TRACE_EVENT ("pins_loadEditDefault()");
		memset(editor_data,0x00,sizeof(T_EDITOR_DATA));
		
		editor_attr_init(&editor_data->editor_attr, PASSWORD_EDITOR, edtCurBar1, NULL, NULL, 0, COLOUR_EDITOR_XX);

		editor_data->hide			    = FALSE;
	    editor_data->Identifier	       = 0;  /* optional */
	    editor_data->mode		    = PIN_SECURITY;
		editor_data->destroyEditor	    = TRUE;
		editor_data->LeftSoftKey	  = TxtSoftSelect;
		editor_data->AlternateLeftSoftKey   = TxtNull;
		editor_data->RightSoftKey	  = TxtSoftBack;
		editor_data->TextId		 = '\0';  
		editor_data->TextString 	  = NULL;  
		editor_data->min_enter		     = 1;  // Avoid to return empty strings
	    editor_data->timeout	      = FOREVER;
		editor_data->Callback		  = (T_EDIT_CB)pins_editor_cb;

}
#endif /* NEW_EDITOR */


//Clear PIN area
void pin_rectClear( MfwRect *win )
{
	
	dspl_Clear(win->px,win->py,win->px+win->sx-1,win->py+win->sy-1);
}
void pin_skClear( void )
{
	MfwRect skRect;
	Mmi_layout_softkeyArea( &skRect );
	pin_rectClear( &skRect );
}

// June 16, 2005  REF: CRR 31267  x0021334
// Call back function to display editor after info dialog
void pin1_cb_function (T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION ("pin1_cb_function");

	// Post SIM_LOCKED_EMR_EDITOR event to start the editor
	SEND_EVENT(win, SIM_LOCKED_EMR_EDITOR, 0, 0);
}
/*******************************************************************************

 $Function:	getBootUpState

 $Description:	 returns the current state during bootup
 
 $Returns:		T_BOOTUP_STATE
 
 $Arguments:		None

//xashmic 21 Sep 2006, OMAPS00095831
*******************************************************************************/
T_BOOTUP_STATE getBootUpState(void)
{
	return BootUpState;
}
//x0pleela 21 Feb, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
/*******************************************************************************
 $Function:		phLock_Enable_Lock
 $Description:	sets the global flag to the network lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_Enable_Lock (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
    T_MFW_HND idle_win = mfwParent( mfw_header() );
    T_MFW_HND win = phlock_setting_pin_create(idle_win);
    UBYTE phlock_status, auto_phlock_status;
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;
	
       TRACE_FUNCTION("phLock_Enable_Lock"); 
	   
    if(win != NULL) 
    {
	win_data = ((T_MFW_HDR *)win)->data;
	pin_data = (T_pin *)win_data->user;

	//x0pleela 23 May, 2007  DR: OMAPS00132483
	//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

	//check the status of Phone lock
    	phlock_status= mfw_phlock_get_lock_status(MFW_PH_LOCK);
    	//check the status of Automatic Phone lock
    	auto_phlock_status = mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK);
    	
	//x0pleela 28 May, 2007  DR: OMAPS132483
	//set the phonelock and auto ph lock status to TRUE if lock is enabled
	if( phlock_status EQ MFW_PH_LOCK_DISABLE)
		mfw_set_Phlock_status(FALSE);
	else
		mfw_set_Phlock_status(TRUE);

	if( auto_phlock_status EQ MFW_PH_LOCK_DISABLE)
		mfw_set_Auto_Phlock_status(FALSE);
	else
		mfw_set_Auto_Phlock_status(TRUE);
	
    	pin_data->set_state = ENABLE;

	if( (phlock_status EQ MFW_PH_LOCK_DISABLE) && 
	     (auto_phlock_status EQ MFW_PH_LOCK_DISABLE))/*if Phone lock is disabled*/
	{	
       	SEND_EVENT(pin_data->pin_win,TXT_PH_LOCK_ALL,0,pin_data);	
     	}
	else 
	        SEND_EVENT(pin_data->pin_win,PH_LOCK_ALREADY_ENDIS,0,pin_data);
    }
	return 1;
}

/*******************************************************************************
 $Function:		phLock_Lock
 $Description:	sets the global flag to the network lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_Lock (void)
{
    T_MFW_HND idle_win = mfwParent( mfw_header() );
    T_MFW_HND win = phlock_setting_pin_create(idle_win);
    UBYTE slock_status;
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;
	T_MFW_SS_RETURN ret;
	T_DISPLAY_DATA display_info;
	
       TRACE_FUNCTION("phLock_Lock"); 

    if(win != NULL) 
    {
	win_data = ((T_MFW_HDR *)win)->data;
	pin_data = (T_pin *)win_data->user;

//x0pleela 23 May, 2007  DR: OMAPS00132483
	//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

    	slock_status= mfw_phlock_get_lock_status(MFW_PH_LOCK);

	if(slock_status EQ MFW_PH_LOCK_DISABLE) /*if Phone lock is enabled*/
      {	
        ret = mfw_phlock_enable_lock(NULL, MFW_PH_LOCK); 

		if ( ret == MFW_SS_FAIL)
		{	
		        SEND_EVENT(pin_data->pin_win,PH_LOCK_EN_FAIL,0,pin_data);
		}
		else if( ret == MFW_EXCT)
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull,TxtPleaseWait,TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, FOREVER, KEY_CLEAR | KEY_LEFT );
			phlock_info_dialog (pin_data->pin_win, &display_info);
		}
         	else
         	{
         		//x0pleela 29 May, 2007  DR: OMAPS00132483
         		//set the phlock status to TRUE
         		mfw_set_Phlock_status( TRUE );
             		SEND_EVENT(pin_data->pin_win,PH_LOCK_SUCC,0,pin_data);
    	}
    	}
	return 1;
    }
	return 0;
}

/*******************************************************************************
 $Function:		phLock_UnLock
 $Description:	sets the global flag to the network lock
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_UnLock (void)
{
    T_MFW_HND idle_win = mfwParent( mfw_header() );
    T_MFW_HND win = phlock_setting_pin_create(idle_win);
    UBYTE slock_status;
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;
	
       TRACE_FUNCTION("phLock_UnLock"); 

    if(win != NULL) 
    {
	win_data = ((T_MFW_HDR *)win)->data;
	pin_data = (T_pin *)win_data->user;

	//x0pleela 23 May, 2007  DR: OMAPS00132483
		//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

    	slock_status= mfw_phlock_get_lock_status(MFW_PH_LOCK);

    	pin_data->set_state = DISABLE;

	if(slock_status EQ MFW_PH_LOCK_ENABLE) /*if Phone lock is disabled*/
    {	
        SEND_EVENT(pin_data->pin_win,TXT_PH_LOCK_ALL,0,pin_data);	
     }
    else 
        SEND_EVENT(pin_data->pin_win,PH_LOCK_ALREADY_ENDIS,0,pin_data);
    	}
	return 1;
}

/*******************************************************************************
 $Function:		phLock_Automatic_On
 $Description:	sets automaticPhLock flag to ON
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_Automatic_On (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
	T_MFW_HND idle_win = mfwParent( mfw_header() );
    	T_MFW_HND win = phlock_setting_pin_create(idle_win);
	UBYTE slock_status;
	T_MFW_WIN * win_data = NULL;
	T_pin * pin_data = NULL;
	
	TRACE_FUNCTION("phLock_Automatic_On");
	autoPhLock_menu = TRUE;
	   
    if(win != NULL) 
    {
	win_data = ((T_MFW_HDR *)win)->data;
	pin_data = (T_pin *)win_data->user;

//x0pleela 23 May, 2007  DR: OMAPS00132483
	//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

	slock_status = mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK);

    	pin_data->set_state = ENABLE;

	if(slock_status EQ MFW_PH_LOCK_DISABLE) /*if Phone lock is disabled*/
    {	
        SEND_EVENT(pin_data->pin_win,TXT_PH_LOCK_ALL,0,pin_data);	
     }
    else 
        SEND_EVENT(pin_data->pin_win,PH_LOCK_ALREADY_ENDIS,0,pin_data);
    	}
	return 1;
}

/*******************************************************************************
 $Function:		phLock_Automatic_Off
 $Description:	sets automaticPhLock flag to OFF
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_Automatic_Off (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
	int status;

	TRACE_FUNCTION("phLock_Automatic_Off"); 

	//x0pleela 23 May, 2007  DR: OMAPS00132483
		//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

	status = mfw_phlock_disable_autoLock(MFW_AUTO_PH_LOCK);
	if( status == MFW_SS_OK )
	{
		//x0pleela 09 Mar, 2007  DR: OMAPS00129014	
		//set the flag to FALSE
		phlock_auto = FALSE;	
		
		//x0pleela 28 May, 2007   DR: OMAPS00132483
		//Set the auto phone lock status to FALSE
		mfw_set_Auto_Phlock_status(FALSE);
		
		mmi_phlock_show_info(0, TxtAutoLockDisable, TxtSuccess, NULL);
	}
	else if ( status == MFW_SS_ALRDY_DIS )
		mmi_phlock_show_info(0, TxtPhoneLock, TxtAlreadyDisabled, NULL);   
	else
		mmi_phlock_show_info(0, TxtAutoLockDisable, TxtFailed, NULL);   

      return 1;
}

/*******************************************************************************
 $Function:		phLock_Change_UnlockCode
 $Description:	
 $Returns:		none
 $Arguments:	none
*******************************************************************************/
int phLock_Change_UnlockCode (struct MfwMnuTag * m, struct MfwMnuItemTag * i)
{
T_MFW_HND idle_win = mfwParent( mfw_header());
    T_MFW_HND win = phlock_setting_pin_create(idle_win);
    T_MFW_WIN * win_data = NULL;
    T_pin * pin_data = NULL;
    int status, auto_lock_status;
   	TRACE_FUNCTION("MmiPins:phLock_Change_UnlockCode");

	if(win != NULL)	/* x0039928 - Lint warning removal */
	{
		//x0pleela 23 May, 2007  DR: OMAPS00132483
			//stop the audio file if playing
#ifdef FF_MMI_AUDIO_PROFILE				
	if( mfwAudPlay)
	{
		// Stopping the current ring tone.
		#ifdef FF_MIDI_RINGER  
			mfw_ringer_stop(sounds_midi_ringer_stop_cb);
		#endif
	}
#endif //FF_MMI_AUDIO_PROFILE

		win_data = ((T_MFW_HDR *)win)->data;
		pin_data = (T_pin *)win_data->user;
		pin_data->set_state = CHANGE_PIN;
		status = mfw_phlock_get_lock_status(MFW_PH_LOCK);
		auto_lock_status = mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK);
		if( ( status == MFW_PH_LOCK_ENABLE) ||
			auto_lock_status == MFW_PH_LOCK_ENABLE) 
		{
			SEND_EVENT(pin_data->pin_win,PH_LOCK_STATUS,0,pin_data);
		}
		else
	    	{
		    	SEND_EVENT(pin_data->pin_win,PH_LOCK_REQ_OLD,0,pin_data);
	    	}
	}
	return 1;
}


/*******************************************************************************
 $Function:		mmi_phlock_en_dis_verify
 $Description:	performs the operation of enabling/disabling and verifying a given
 				lock type after the input of the password
 $Returns:		none
 $Arguments:	none
*******************************************************************************/

static void mmi_phlock_en_dis_verify(T_MFW_HND win)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data
	T_MFW_SS_RETURN ret;
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION("mmi_phlock_en_dis_verify()");
  if((pin_data->set_state != ENABLE) && (pin_data->set_state != DISABLE))
	{	
              
	       TRACE_FUNCTION("We're verifying SIMLock");
	     
              SEND_EVENT(pin_data->pin_win,PH_LOCK_REQ_NEW,0,pin_data);
              
       }
  else if(pin_data->set_state EQ ENABLE)
   {	 
		TRACE_FUNCTION("MmiPins:Enable Lock now");
 		if( autoPhLock_menu )
 		{
			ret = mfw_phlock_enable_lock((U8*)pin_data->old_pin, MFW_AUTO_PH_LOCK); 
 		}
		else
		{
			ret = mfw_phlock_enable_lock((U8*)pin_data->old_pin, MFW_PH_LOCK); 
		}
		if ( ret == MFW_SS_FAIL)
		{	
		        SEND_EVENT(pin_data->pin_win,PH_LOCK_ENDIS_FAIL,0,pin_data);
		}
		else if( ret == MFW_EXCT)
		{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull,TxtPleaseWait,TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, FOREVER, KEY_CLEAR | KEY_LEFT );
			phlock_info_dialog (pin_data->pin_win, &display_info);
		}
         	else
     		{
			if( autoPhLock_menu )
			{
				//x0pleela 09 Mar, 2007  DR: OMAPS00129014
				//set the flag to TRUE
				phlock_auto = TRUE;

				//x0pleela 28 May, 2007   DR: OMAPS00132483
				//Set the auto phone lock status to TRUE
				mfw_set_Auto_Phlock_status(TRUE);
	             		SEND_EVENT(pin_data->pin_win,PH_LOCK_AUTO_SUCC,0,pin_data);
			}
			else
			{
				//x0pleela 28 May, 2007   DR: OMAPS00132483
				//Set the phone lock status to TRUE
				mfw_set_Phlock_status(TRUE);
	             		SEND_EVENT(pin_data->pin_win,PH_LOCK_ENDSUCC,0,pin_data);
			}
     		}
   }
   else  if(pin_data->set_state EQ DISABLE)
   { 	
   TRACE_FUNCTION("MmiPins:Disable Lock now");
   		ret = mfw_phlock_disable_lock((U8*)pin_data->old_pin,MFW_PH_LOCK);
		if (ret == MFW_SS_FAIL)
		{	
			SEND_EVENT(pin_data->pin_win,PH_LOCK_ENDIS_FAIL,0,pin_data);
		}
        	else
        	{
			//x0pleela 28 May, 2007   DR: OMAPS00132483
			//Set the phone lock status to FALSE
			mfw_set_Phlock_status(FALSE);
			
	       	SEND_EVENT(pin_data->pin_win,PH_LOCK_ENDSUCC,0,pin_data);
        	}
   }

}


/*******************************************************************************
 $Function:		mmi_phlock_change_pin
 $Description:	Performs the change password for a given lock type
 $Returns:		success or failure
 $Arguments:	lock type, oldpassword, new password
*******************************************************************************/

void mmi_phlock_change_pin(T_MFW_HND win,int type,char* oldpsw,char* newpsw)
{
    T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
    T_pin * pin_data = (T_pin *)win_data->user;

	TRACE_FUNCTION("mmi_phlock_change_pin()");
    TRACE_EVENT_P2(" old pasword%s new password%s ",oldpsw,newpsw);
     if(mfw_phlock_change_lock_code(type,oldpsw,newpsw) !=MFW_SS_OK)
     	{
     	TRACE_EVENT("mmi_phlock_change_pin: FAILED");
       SEND_EVENT(pin_data->pin_win,PH_LOCK_NEW_ENDISFAIL,0,pin_data);
     	}
    else
    	{
    	     	TRACE_EVENT("mmi_phlock_change_pin: SUCCESS");
        SEND_EVENT(pin_data->pin_win,PH_LOCK_NEW_ENDSUCC,0,pin_data);
    	}
}

/*******************************************************************************

 $Function:     mmi_phlock_show_info

 $Description: Display the Dialog

 $Returns:

 $Arguments:

*******************************************************************************/
static MfwHnd mmi_phlock_show_info(T_MFW_HND parent, int str1, int str2, T_VOID_FUNC callback)
{
	T_DISPLAY_DATA display_info;

	TRACE_FUNCTION ("mmi_phlock_show_info()");

	/*
	** Create a timed dialog to display the Message "Failed"
	*/
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, str1, str2, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, callback, THREE_SECS, KEY_LEFT | KEY_CLEAR | KEY_HUP);
	return phlock_info_dialog(parent, &display_info);
}


/*******************************************************************************

 $Function:	phlock_setting_pin_create
 $Description:	Creation of an instance for the Settings PIN main dialog.
				Type of dialog : SINGLE_DYNAMIC
				Top Window must be available at any time, only one instance.
 $Returns:		window
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND phlock_setting_pin_create (T_MFW_HND parent_window)
{

  T_pin * data = (T_pin *)ALLOC_MEMORY (sizeof (T_pin));
  T_MFW_WIN	* win;

  data->pin_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)phlock_setting_pin_win_cb);

  TRACE_FUNCTION("MmiPins:phlock_setting_pin_create");

  set_pin_windows = data->pin_win;

  if (data->pin_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)phlock_setting_pin_main;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_win)->data;
  win->user		      = (void *) data;

  /*
   * Create any other handler
   */

  data->sim_handle = sim_create(data->pin_win,E_SIM_ALL_SERVICES,(MfwCb)sim_event_cb_setting);

  /*
   * Initialise
   */
 data->set_state = DEFAULT;
 data->puk_request = FALSE;


  mfwSetSignallingMethod(1);//mfw focus handling
  winShow(data->pin_win);//focus on settings:PIN/PIN2 Change/act/Deac


  /*
   * return window handle
   */
  return data->pin_win;
}
/*******************************************************************************

 $Function:	phlock_setting_pin_win_cb 
 $Description:	Callback function for main windows in settings
 $Returns:		Status int
 $Arguments:	event, window
*******************************************************************************/
static int phlock_setting_pin_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  TRACE_FUNCTION("phlock_setting_pin_win_cb()");
  if (event EQ MfwWinVisible)
  {
    /*
     * Top Window has no output
     */
	/* NDH : Removed to prevent Blue Screen on Pin Entry */
	return 1;
  }
  return 0;

}

/*******************************************************************************

 $Function:	phlock_setting_pin_main 
 $Description:	PIN Settings Dialog Handling function
 $Returns:		none
 $Arguments:	win, event, value, parameters
*******************************************************************************/
void phlock_setting_pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;//pin main data

	TRACE_FUNCTION("phlock_setting_pin_main()");

	 switch(event)
	 {
	case PH_LOCK_ALREADY_ENDIS:
	case PH_LOCK_STATUS:
       case PH_LOCK_REQ_OLD:
   	case PH_LOCK_REQ_NEW:
       case INPUT_NEW_PH_LOCK_AGAIN:	
       case NEW_PH_LOCK_FAIL:
       case PH_LOCK_NEW_ENDISFAIL:
       case PH_LOCK_NEW_ENDSUCC:
       case TXT_PH_LOCK_ALL:
       case PH_LOCK_ENDSUCC:
	case PH_UNLOCK_REQ:
	case PH_LOCK_AUTO_SUCC:
	case PH_LOCK_SUCC:
	case PH_LOCK_ENDIS_FAIL:
				  phlock_sett_pin_mess(win,event);
				  break;
	 }

	 switch(event)
	 {
                 case TXT_ENTER_PHLOCK:
	 			TRACE_EVENT("phlock_setting_pin_main: TXT_ENTER_PHLOCK");
                    		pin_data->pin_case = INPUT_PH_LOCK;
				phlock_sett_pin_edit(win,INPUT_PH_LOCK);
                            break;

                   case TXT_ENTER_NEW_PHLOCK:
                               pin_data->pin_case = INPUT_NEW_PH_LOCK;
				   phlock_sett_pin_edit(win,INPUT_NEW_PH_LOCK);
                                break;  

                   case TXT_ENTER_NEW_PHLOCK_AGAIN:
                               pin_data->pin_case = INPUT_NEW_PH_LOCK_AGAIN;
				   phlock_sett_pin_edit(win,INPUT_NEW_PH_LOCK_AGAIN);
				   break;      

                  case NEW_PH_LOCK_END:
                                    mmi_phlock_change_pin(win,MFW_PH_LOCK,(char*)pin_data->old_pin,(char*)pin_data->new_pin);
                              break; 				   

		case PH_LOCK_END:
			TRACE_EVENT("phlock_setting_pin_main:PH_LOCK_END");
			
			//x0pleela 15 May, 2007 DR: OMAPS00127483
			//When ringer for either incoming call or alarm is playing in phone locked state, Audio usues the DMA channels
			//which will be active. The DMA channels will be available only after the audio file is fully played.
			//This will cause that process to be blocked until the DMA channel becomes in-active and in turn if we try to 
			//query or access secure driver we are blocked until the audio file is completely played.
			//Since this is a hardware constraint, we found a workaround for this issue and are stopping the ringer  
			//either for incoming call or alarm before accessing secure driver for enabling or disbaling the phone lock.

			if( (( call_data.call_direction == MFW_CM_MTC ) &&  phlock_dialog_mtc_win_handle)
				|| (phlock_alarm) )
			{
				if( (!phlock_win_handle)		
			#ifdef FF_MMI_AUDIO_PROFILE				
					&& ( mfwAudPlay)
			#endif
					)
				{
				// Stopping the current ring tone.
				#ifdef FF_MIDI_RINGER  
					mfw_ringer_stop(sounds_midi_ringer_stop_cb);
				#endif
				}
			}
			 mmi_phlock_en_dis_verify(win);
                      break;
				   
	 
               case PH_LOCK_SETT_ABORT:
	 			TRACE_EVENT("phlock_setting_pin_main: PH_LOCK_SETT_ABORT");
				phlock_setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
				break;
				
		  default:
			  break;
	 }

}

/*******************************************************************************

 $Function:	phlock_sett_pin_mess 
 $Description:	help function for message dialog
 $Returns:		void
 $Arguments:	window, event
*******************************************************************************/
static void phlock_sett_pin_mess(T_MFW_HND win,USHORT event)
{

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_mess_win;
	TRACE_FUNCTION("MmiPins:phlock_sett_pin_mess()");

	pin_mess_win = phlock_set_pin_mess_create(pin_data->pin_win);
	if(pin_mess_win)
	{
		SEND_EVENT(pin_mess_win,event,0,pin_data);
	}
}


/*******************************************************************************

 $Function:	phlock_set_pin_mess_create
 $Description:	Creation of an instance  for the PIN Message dialog settings
				Type of dialog : SINGLE_DYNAMIC
 $Returns:		void
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND phlock_set_pin_mess_create(T_MFW_HND parent_window)
{
  T_pin_mess * data = (T_pin_mess *)ALLOC_MEMORY (sizeof (T_pin_mess));
  T_MFW_WIN	* win;

  data->pin_mess_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)phlock_set_pin_mess_win_cb);

  TRACE_FUNCTION("MmiPins: phlock_set_pin_mess_create");

  if (data->pin_mess_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)phlock_set_pin_messages;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_mess_win)->data;
  win->user		      = (void *) data;

  /*
   * return window handle
   */
  return data->pin_mess_win;
}


/*******************************************************************************

 $Function:	phlock_set pin_mess_destroy
 $Description:	Destroy the pin message dialog settings
 $Returns:		void
 $Arguments:	window
*******************************************************************************/
void phlock_set_pin_mess_destroy  (T_MFW_HND own_window)
{
  T_pin_mess * data;
  T_MFW_WIN * win;


  TRACE_FUNCTION("MmiPins: phlock_set_pin_mess_destroy()");

  if (own_window)
  {
	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_mess *)win->user;

	  if(data)
	  {

      /*
       * Delete WIN Handler
       */
	  win_delete (data->pin_mess_win);
	      FREE_MEMORY((void*)data,(sizeof(T_pin_mess)));
	  }
   }
}

/*******************************************************************************

 $Function:	phlock_set_pin_mess_win_cb
 $Description:	Callback function for message windows
 $Returns:		void
 $Arguments:	event, window
*******************************************************************************/
static int phlock_set_pin_mess_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
TRACE_FUNCTION("phlock_set_pin_mess_win_cb()");
  /*
   * Top Window has no output
   */
  return 1;
}


/*******************************************************************************

 $Function:	phlock_set_pin_messages
 $Description:	Message Dialog for PIN/PUK handling in settings
 $Returns:		void
 $Arguments:	win, event, value, parameter
*******************************************************************************/
void phlock_set_pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_mess * mess_data = (T_pin_mess *)win_data->user;

	T_DISPLAY_DATA display_info;
	T_pin * pin_data = (T_pin *)parameter;
  TRACE_FUNCTION("Mmi.Pins:phlock_set_pin_messages");

  /*
   * depending on signal
   */
	
  switch (event)
  {
	case PH_LOCK_ALREADY_ENDIS:
	case PH_LOCK_NEW_ENDSUCC:
    	case PH_LOCK_ENDSUCC:
	case PH_LOCK_AUTO_SUCC:
	case PH_LOCK_SUCC:
	case PH_LOCK_STATUS:		
   	case NEW_PH_LOCK_FAIL:
       case PH_LOCK_NEW_ENDISFAIL:
       case PH_LOCK_ENDIS_FAIL:
	case PH_LOCK_EN_FAIL:

		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull,  COLOUR_STATUS_PINS);
		
		switch(event)
		{
		case PH_LOCK_ALREADY_ENDIS:
			pin_data->pin_case = PH_LOCK_ALREADY_ENDIS;
			display_info.TextId = TxtPhoneLock;

		  	if(pin_data->set_state == ENABLE)
				display_info.TextId2 = TxtAlreadyEnabled;
			else if(pin_data->set_state == DISABLE)
			  	display_info.TextId2 = TxtAlreadyDisabled;
			  
			  break;

	         case PH_LOCK_NEW_ENDSUCC:
			if(phlock_win_handle)
				phlock_win_handle = NULL;

			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

          		  pin_data->pin_case = PH_LOCK_NEW_ENDSUCC;
			  display_info.TextId = TxtPhoneLock;
			  display_info.TextId2 = TxtPsLockChanged;
			  break;
			  
		  case PH_LOCK_ENDSUCC:
		  	TRACE_EVENT("phlock_set_pin_messages: PH_LOCK_ENDSUCC");
			if(phlock_win_handle)
				phlock_win_handle = NULL;


			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

				//x0pleela 28 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( mfw_get_Phlock_status() )
      				{
      					TRACE_EVENT("LOCK ENABLED");
	      				//For USB MS
	      				if( mmi_get_usbms_enum() )
	      				{
					#ifdef FF_MMI_USBMS      			
						//unregister USB enumeration
						TRACE_EVENT("USB de-enumeration");
			      			mfw_usbms_enum_control(MFW_USB_NO_ENUM , MFW_USB_TYPE_ALL);
					#endif /*FF_MMI_USBMS*/
	      				}
					//For Headset
					if( mmi_get_Headset_Reg() )
					{
						#ifdef FF_MMI_AUDIO_PROFILE	
							//unregister headset
							//mfw_headset_unregister();
							TRACE_EVENT("headset unregister");
							mfw_hook_unregister();
						#endif /*FF_MMI_AUDIO_PROFILE*/
					}
					pin_data->pin_case = PH_LOCK_ENDSUCC;
				}
				else
				{
					TRACE_EVENT("LOCK DISABLED");
					//For USB MS
					if( mmi_get_usbms_enum() )
					{
					#ifdef FF_MMI_USBMS      
			      			//register USB enumeration
			      			TRACE_EVENT("USB re-enumeration");
		      				mfw_usbms_enum_control(MFW_USB_ENUM , MFW_USB_TYPE_ALL);
					#endif /*FF_MMI_USBMS*/
					}
					//For Headset
					if( mmi_get_Headset_Reg() )
					{
						#ifdef FF_MMI_AUDIO_PROFILE	
							//register headset
							//mfw_headset_register();
						if(call_data.win_calling OR call_data.win_incoming OR call_data.calls.numCalls)
							{
								TRACE_EVENT("headset register");
								mfw_hook_register();
							}
						#endif /*FF_MMI_AUDIO_PROFILE*/
					}	
						pin_data->pin_case = PH_LOCK_SUCCESS;
							
				}
				display_info.TextId = TxtPhone;
				display_info.Identifier = pin_data->pin_case;
	
		 if(pin_data->set_state == ENABLE)
			  {
			  	display_info.TextId2 = TxtLockActivated;
				TRACE_EVENT("phlock_set_pin_messages: PH_LOCK_ENDSUCC: Activated");
			  }
			  else if(pin_data->set_state == DISABLE)
			  {
			  	display_info.TextId2 = TxtLockDeactivated;
				TRACE_EVENT("phlock_set_pin_messages: PH_LOCK_ENDSUCC: Deactivated");
			  }

			  break;				  
		
		case PH_LOCK_AUTO_SUCC:
			TRACE_EVENT("phlock_set_pin_messages: PH_LOCK_AUTO_SUCC");
			if(phlock_win_handle)
				phlock_win_handle = NULL;


			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

			//x0pleela 29 May, 2007  DR: OMAPS00132483
				//get the auto phone lock status 
				if( mfw_get_Auto_Phlock_status() )
			  {
			  	display_info.TextId = TxtAutoPhone;
			    	pin_data->pin_case = PH_LOCK_AUTO_SUCC;
				autoPhLock_menu = FALSE;
			  }
			  if(pin_data->set_state == ENABLE)
			  {
			  	display_info.TextId2 = TxtLockActivated;
			  }
			  else if(pin_data->set_state == DISABLE)
			  {
			  	display_info.TextId2 = TxtLockDeactivated;
			  }
			break;

			case PH_LOCK_SUCC:
		  	TRACE_EVENT("set_pin_msgs: PH_LOCK_SUCC");
			if(phlock_win_handle)
				phlock_win_handle = NULL;

			
			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

				//x0pleela 28 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( mfw_get_Phlock_status() )
      				{
      					TRACE_EVENT("LOCK ENABLED");
	      				//For USB MS
	      				if( mmi_get_usbms_enum() )
	      				{
					#ifdef FF_MMI_USBMS      			
						//unregister USB enumeration
						TRACE_EVENT("USB de-enumeration");
			      			mfw_usbms_enum_control(MFW_USB_NO_ENUM , MFW_USB_TYPE_ALL);
					#endif /*FF_MMI_USBMS*/
	      				}
					//For Headset
					if( mmi_get_Headset_Reg() )
					{
						#ifdef FF_MMI_AUDIO_PROFILE	
							//unregister headset
							//mfw_headset_unregister();
							TRACE_EVENT("headset unregister");
							mfw_hook_unregister();
						#endif /*FF_MMI_AUDIO_PROFILE*/
					}
				}
				else
				{
					TRACE_EVENT("LOCK DISABLED");
					//For USB MS
					if( mmi_get_usbms_enum() )
					{
					#ifdef FF_MMI_USBMS      
			      			//register USB enumeration
			      			TRACE_EVENT("USB re-enumeration");
		      				mfw_usbms_enum_control(MFW_USB_ENUM , MFW_USB_TYPE_ALL);
					#endif /*FF_MMI_USBMS*/
					}
					//For Headset
					if( mmi_get_Headset_Reg() )
					{
						#ifdef FF_MMI_AUDIO_PROFILE	
							//register headset
							//mfw_headset_register();
						if(call_data.win_calling OR call_data.win_incoming OR call_data.calls.numCalls)
							{
								TRACE_EVENT("headset register");
								mfw_hook_register();
							}
						#endif /*FF_MMI_AUDIO_PROFILE*/
					}	
					}
			
			display_info.TextId = TxtLockActivated;
			pin_data->pin_case = PH_LOCK_ENDSUCC;
			 display_info.Identifier = pin_data->pin_case;
			  
			  break;	
  
	case PH_LOCK_STATUS:
		pin_data->pin_case = PH_LOCK_STATUS;
		display_info.TextId = TxtPhone;
      		switch(mfw_phlock_get_lock_status(MFW_PH_LOCK)) 
      		{
                      case MFW_PH_LOCK_DISABLE:
				display_info.TextId2 = TxtLockDisabled;
				break;
                      case  MFW_PH_LOCK_ENABLE:
				display_info.TextId2 = TxtLockEnabled; 
				break;
           	}
     		break;

	case PH_LOCK_NEW_ENDISFAIL:
	{
			if(phlock_win_handle)
				phlock_win_handle = NULL;

			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

	pin_data->pin_case = FAILURE_PASS_CHG;
	display_info.TextId = TxtCodeInc;
	display_info.Identifier = pin_data->pin_case;

	}
			  break;
 	case NEW_PH_LOCK_FAIL:
			if(phlock_win_handle)
				phlock_win_handle = NULL;
			
			if( phlock_kbd_handle)
				phlock_kbd_handle = NULL;

			
              	pin_data->pin_case = INPUT_NEW_PH_LOCK;
			  display_info.TextId = TxtPsLockConfWrong;
			  display_info.TextId2 = TxtPsLockConfwrong2;
			  break;

       case PH_LOCK_ENDIS_FAIL:
	   	TRACE_EVENT("phlock_set_pin_msgs:PH_LOCK_ENDIS_FAIL");
		//x0pleela 28 May, 2007  DR: OMAPS00132483
		//get the phone lock status 
		if ( mfw_get_Phlock_status() ) 
			pin_data->pin_case = PH_LOCK_ENDIS_FAIL;
		else
			pin_data->pin_case = PH_LOCK_FAILURE;
		display_info.Identifier = pin_data->pin_case;

		if (autoPhLock_menu )
			autoPhLock_menu = FALSE;

		  display_info.TextId = TxtCodeInc;
		break;	

	case PH_LOCK_EN_FAIL:
		pin_data->pin_case = PH_LOCK_FAILURE;
		display_info.TextId = TxtNotLocked;
		break;
		
		 	}

    {
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)phlock_set_pin_info_cb, THREE_SECS, KEY_LEFT );
	display_info.Identifier = pin_data->pin_case;
	phlock_set_pin_mess_destroy(mess_data->pin_mess_win);

	phlock_info_dialog(pin_data->pin_win,&display_info); //information screen
	 dspl_Enable(1);
    }
	break;
		
	case TXT_PH_LOCK_ALL:
	case PH_UNLOCK_REQ:
		TRACE_EVENT(":phlock_set_pin_msgs: TXT_PH_LOCK_ALL");
       	pin_data->display_id1 = TxtEnterPhUnlockCode;
 		phlock_set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PHLOCK,0,pin_data);
		break;
	case PH_LOCK_REQ_OLD:
              pin_data->display_id1 = TxtEnterOldPsLock;
		phlock_set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_PHLOCK,0,pin_data);
		break;

	case PH_LOCK_REQ_NEW:
		pin_data->display_id1 = TxtEnterPsLockNew;
		phlock_set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PHLOCK,0,pin_data);
		break;

       case INPUT_NEW_PH_LOCK_AGAIN:
              pin_data->display_id1 = TxtEnterPsLockconf;
		phlock_set_pin_mess_destroy(mess_data->pin_mess_win);
		SEND_EVENT(pin_data->pin_win,TXT_ENTER_NEW_PHLOCK_AGAIN,0,pin_data);
              break;
	default:
		break;
  }
		
}


/*******************************************************************************

 $Function:	phlock_set_pin_info_cb
 $Description:	settings pin information screen call back
 $Returns:		void
 $Arguments:	win, identifier, reason
*******************************************************************************/
void phlock_set_pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user;

	TRACE_FUNCTION("MmiPins: phlock_set_pin_info_cb()");

	switch(identifier)
	{

		case PH_LOCK_ENDSUCC:					
				TRACE_EVENT("PH_LOCK_ENDSUCC");
				
				//x0pleela 28 May, 2007  DR: OMAPS00132483
				//get the phone lock status 
				if( mfw_get_Phlock_status() )
	      			{
	      			TRACE_EVENT("phlock_set_pin_info_cb: LOCK ENABLED");

						//For USB MS
	      				if( mmi_get_usbms_enum() )
	      				{
					#ifdef FF_MMI_USBMS      			
						//unregister USB enumeration
						TRACE_EVENT("USB de-enumeration");
			      			mfw_usbms_enum_control(MFW_USB_NO_ENUM , MFW_USB_TYPE_ALL);
					#endif /*FF_MMI_USBMS*/
	      				}
						
					pin_data->set_state = DISABLE;
					identifier = pin_data->pin_case = PH_UNLOCK_REQ;
					SEND_EVENT(pin_data->pin_win,identifier,0,NULL);				
	           		}
												
			break;

			case PH_LOCK_ENDIS_FAIL:
				TRACE_EVENT("PH_LOCK_ENDIS_FAIL");
				dspl_Enable(1);
				pin_data->set_state = DISABLE;
				identifier = pin_data->pin_case = PH_UNLOCK_REQ;
				
				//x0pleela 15 May, 2007 DR: OMAPS00127483
				//Starting the ringer again for either incoming call or alarm if the user has entered wrong unlocking pasword.
				if( ( ( ( call_data.call_direction == MFW_CM_MTC ) &&  (phlock_dialog_mtc_win_handle) ) || ( phlock_alarm ) )
					&& (phlock_win_handle)
					#ifdef FF_MMI_AUDIO_PROFILE				
						&& ( !mfwAudPlay)
					#endif	
				)
				{
					if( call_data.call_direction == MFW_CM_MTC ) 
					{
					#ifdef FF_MIDI_RINGER  
						mfw_ringer_start(AS_RINGER_MODE_IC, TRUE, sounds_midi_ringer_start_cb);
					#endif
					}
					else if ( phlock_alarm )
					{
					#ifdef FF_MIDI_RINGER  
						mfw_ringer_start(AS_RINGER_MODE_ALARM, TRUE, sounds_midi_ringer_start_cb);
					#endif
					}
					
				}
				SEND_EVENT(pin_data->pin_win,identifier,0,NULL);				
			break;

			case PH_LOCK_SETT_ABORT:      
				SEND_EVENT(pin_data->pin_win,identifier,0,NULL);//pin_main data !!
				break;				
			

		default:		
		switch (reasons)
		{
			case INFO_KCD_LEFT:
			case INFO_KCD_RIGHT:
		
			case INFO_TIMEOUT:
			{
			        pin_skClear();
				phlock_setting_pin_destroy(pin_data->pin_win);// destroy pin settings dialog
				//x0pleela 07 Apr, 2007  ER: OMAPS00122561
				if( phlock_alarm) 
				{
					TRACE_EVENT("phlock_set_pin_info_cb: INFO_TIMEOUT ");
					if( phlock_alarm_win_handle )
						winShow(phlock_alarm_win_handle);
				}	
				if( call_data.call_direction == MFW_CM_MTC )
				{
					if( phlock_mtc_win_handle )
						winShow(phlock_mtc_win_handle);
					
					if( phlock_dialog_mtc_win_handle )
						winShow(phlock_dialog_mtc_win_handle);
				}
			}
		
			break;
		}
			break;
	}
	return;
}

/*******************************************************************************

 $Function:	phlock_sett_pin_edit
 $Description:	help function for editor dialog
 $Returns:		none
 $Arguments:	window, event
*******************************************************************************/
static void phlock_sett_pin_edit(T_MFW_HND win, USHORT event)
{

//open the editor for entering the pin

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin * pin_data = (T_pin *)win_data->user; // pin main data

	T_MFW_HND pin_edt_win;


TRACE_FUNCTION("phlock_sett_pin_edit()");
	pin_edt_win = phlock_set_pin_edt_create(pin_data->pin_win);



    if(pin_edt_win)
	{
		SEND_EVENT(pin_edt_win,event,0,pin_data);
	}
 


}


/*******************************************************************************

 $Function:	phlock_set_pin_edt_create
 $Description:	Creation of an instance  for the PIN Editor dialog settings
				Type of dialog : SINGLE_DYNAMIC
 $Returns:		window
 $Arguments:	parent window
*******************************************************************************/
T_MFW_HND phlock_set_pin_edt_create (T_MFW_HND parent_window)
{
  T_pin_edt * data = (T_pin_edt *)ALLOC_MEMORY (sizeof (T_pin_edt));
  T_MFW_WIN * win;

  data->pin_edt_win = win_create (parent_window, 0, MfwWinVisible, (T_MFW_CB)phlock_set_pin_edt_win_cb);

  TRACE_FUNCTION("MmiPins:phlock_set_pin_edt_create");

  if (data->pin_edt_win EQ 0)
    return 0;
  
	phlock_win_handle = data->pin_edt_win;
  /*
   * Create window handler
   */
  data->mmi_control.dialog    = (T_DIALOG_FUNC)phlock_set_pin_editor;
  data->mmi_control.data      = data;
  data->parent_win = parent_window;
  win			      = ((T_MFW_HDR *)data->pin_edt_win)->data;
  win->user		      = (void *) data;

  /*
   * Create any other handler
   */

  data->tim_out_handle = tim_create(data->pin_edt_win,TIMEOUT,(T_MFW_CB)phlock_pin_edt_tim_out_cb);

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
  data->editor = ATB_edit_Create(&data->editorPinAttr,0);
#else /* NEW_EDITOR */
  data->editor_handle = edt_create(data->pin_edt_win,&data->editpinAttr,0,0);
#endif /* NEW_EDITOR */
  data->kbd_handle = kbd_create(data->pin_edt_win,KEY_ALL,(T_MFW_CB)phlock_set_pin_kbd_cb);
phlock_kbd_handle = data->kbd_handle;

  data->kbd_long_handle = kbd_create(data->pin_edt_win,KEY_ALL | KEY_LONG,(T_MFW_CB)phlock_set_pin_edt_kbd_long_cb);
  /*
   * return window handle
   */
  return data->pin_edt_win;
}

/*******************************************************************************

 $Function:	phlock_set_pin_edt_win_cb
 $Description:	Callback function for editor windows
 $Returns:		status int
 $Arguments:	event, window
*******************************************************************************/
static int phlock_set_pin_edt_win_cb (T_MFW_EVENT event,T_MFW_WIN * win)
{
  T_pin_edt * edt_data = (T_pin_edt *)win->user;//pin edt data
  T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
  T_pin     * pin_data = (T_pin *)win_pin->user; // pin main data

  TRACE_FUNCTION("phlock_set_pin_edt_win_cb");

  if (event EQ MfwWinVisible)
  {

  /* SPR#1428 - SH - New Editor changes */
  #ifdef NEW_EDITOR
	ATB_edit_Show(edt_data->editor);
	
	if (edt_data->editor->update!=ED_UPDATE_TRIVIAL)
	{
		/* Set the colour for drawing title */

		resources_setTitleColour(COLOUR_EDITOR);

		dspl_Clear(0,0, SCREEN_SIZE_X-1, edt_data->editor->attr->win_size.py-1);

		/* Title */

		if(pin_emergency_call == TRUE)
		{					
			PROMPT(0,0,0,TxtEmergency);
		}
		else
		{
		   	PROMPT(0,0,0,pin_data->display_id1);
#ifdef SIM_PERS
			if( pin_data->display_id2)
				PROMPT(0,20,0,pin_data->display_id2);
#endif
		}

		/* Soft keys */
		
		if ((strlen((char*)edt_data->pin) >= MIN_PIN) && (strlen((char*)edt_data->pin) < (MAX_PIN + 1)))
		{
				if(!(edt_data->gsm_state))	  /* usual way for PIN 4 digits     */
				{
#ifdef SIM_PERS
					if(pin_data->display_id1 == TxtPhoneBlocked)
						softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);
					else
#endif
					softKeys_displayId(TxtSoftOK,TxtDelete,0, COLOUR_EDITOR_XX);
				}
			}
		//xvilliva - Added the below "if" and moved the existing "if" in to "else". "if" avoids "delete" key display.
		if((strlen((char*)edt_data->pin) == 0))//  May 13, 2004        REF: CRR 13623  xvilliva
		{
			TRACE_EVENT("clear softkey 16 - zero length");
			softKeys_displayId(TxtNull,TxtNull,0, COLOUR_EDITOR_XX);												
		}
		else if((strlen((char*)edt_data->pin) < MIN_PIN) && (edt_data->emergency_call != TRUE))
		{
			TRACE_EVENT("clear softkey 16");	
			softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);												
		}
#ifdef SIM_PERS
		else if((strlen((char*)edt_data->pin) < MIN_PIN) && (edt_data->emergency_call == TRUE))
		{
			TRACE_EVENT("clear softkey 16");	
			if(pin_emergency_call == TRUE)
				softKeys_displayId(TxtSoftCall,TxtDelete,0, COLOUR_EDITOR_XX);												
			else
			softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);												
		}
#endif
	}
	
	edt_data->editor->update = ED_UPDATE_DEFAULT;
	
#else /* NEW_EDITOR */
	pin_icons();
   	PROMPT(INFO_TEXT_X,INFO_TEXT_Y,0,pin_data->display_id1);
   	
  TRACE_FUNCTION("softkey 6");
	softKeys_displayId(TxtNull,TxtDelete,0, COLOUR_EDITOR_XX);
#endif /* NEW_EDITOR */
 
    return 1;
  }
  return 0;
}



/*******************************************************************************

 $Function:	phlock_set_pin_editor
 $Description:	Pin editor Dialog Signal Handling function
 $Returns:		void
 $Arguments:	window, event, value, parameters
*******************************************************************************/
static void phlock_set_pin_editor(T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;

    T_pin * pin_data = (T_pin *)parameter;//pin main data

	edt_data->pin_case_edit = pin_data->pin_case;

	TRACE_FUNCTION("MmiPins:phlock_set_pin_editor");

  /*
   * depending on event
   */
  switch (event)
  {
	
//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	case INPUT_PH_LOCK:
	case INPUT_NEW_PH_LOCK:
	case INPUT_NEW_PH_LOCK_AGAIN:
#endif /*FF_PHONE_LOCK*/
		//SPR#717 - GW - Wrong structures being cleared
		memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string  */
		memset(edt_data->puk, '\0', sizeof(edt_data->puk));
		memset(edt_data->new_pin, '\0', sizeof(edt_data->new_pin));
		memset(edt_data->old_pin, '\0', sizeof(edt_data->old_pin));

		/* Set up pin entry attributes */
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_SetAttr( &edt_data->editorPinAttr, PIN_EDIT_RESET, COLOUR_EDITOR_XX, EDITOR_FONT, 0, ED_CURSOR_UNDERLINE, ATB_DCS_ASCII, (UBYTE *)edt_data->edtbuf, MAX_DIG);
		clear_edit_array(edt_data);	     /*    clear editor buffer	    */
		edt_data->editor_index = 0;
		ATB_edit_Init(edt_data->editor);
		ATB_edit_Show(edt_data->editor);
#else /* NEW_EDITOR */
		editor_attr_init( &edt_data->editpinAttr, PIN_EDIT_RESET, edtCurBar1,0,(char*)edt_data->edtbuf,MAX_DIG,COLOUR_EDITOR_XX);
		clear_edit_array(edt_data);	     /*    clear editor buffer	    */
		edt_data->editor_index = 0;
		edtShow(edt_data->editor_handle);
#endif /* NEW_EDITOR */

		winShow(edt_data->pin_edt_win);

		break;

	default:
		break;
  }
}



/*******************************************************************************

 $Function:	set_pin_kbd_cb
 $Description:	Callback function for keyboard settings pin
 $Returns:		status int
 $Arguments:	event, keyboard control block
*******************************************************************************/
static int phlock_set_pin_kbd_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());
	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;

	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

	char timer = TRUE;

	timStop(edt_data->tim_out_handle);/* stop entering timer      */

	TRACE_FUNCTION("MmiPins:phlock_set_pin_kbd_cb");

//x0pleela 12 Mar, 2007  ER: OMAPS00122561	
#if defined ( SIM_PERS) || defined (FF_PHONE_LOCK)
 	#if defined (SIM_PERS ) AND !defined (FF_PHONE_LOCK)
		if ( !perm_blocked) 
	#elif defined (FF_PHONE_LOCK ) AND !defined (SIM_PERS)
		if ((mfw_phlock_get_lock_status(MFW_PH_LOCK) == MFW_PH_LOCK_DISABLE) ||
			(mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK) == MFW_PH_LOCK_DISABLE) )
	#elif defined (SIM_PERS ) AND defined (FF_PHONE_LOCK)
		if( ( !perm_blocked) || 
			(mfw_phlock_get_lock_status(MFW_PH_LOCK) == MFW_PH_LOCK_DISABLE) 	||
			(mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK) == MFW_PH_LOCK_DISABLE))
	#endif
	
#endif /* SIM_PERS OR FF_PHONE_LOCK */
	{
	switch(kc->code)
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
//x0pleela 12 Mar, 2007  ER: OMAPS00122561
			if(edt_data->editor_index < PH_MAX_DIG)
			{
				/* xreddymn OMAPS00083495 Jul-04-2006 
				 * Limit PIN1, PIN2 entries to 8 digits.
				 */
				if((
					 (edt_data->pin_case_edit==INPUT_PH_LOCK)  ||
					(edt_data->pin_case_edit==INPUT_NEW_PH_LOCK)  ||
					(edt_data->pin_case_edit==INPUT_NEW_PH_LOCK_AGAIN)
					)
					&&(edt_data->editor_index>=4) 

					&& (!edt_data->gsm_state))
					return MFW_EVENT_CONSUMED;
				else
				{
					if(kc->code EQ KCD_STAR)
						edt_data->pin[edt_data->editor_index] = STAR;
					else
						edt_data->pin[edt_data->editor_index] = '0' + kc->code;

					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					ATB_edit_AsciiChar(edt_data->editor, '*', TRUE);
#else /* NEW_EDITOR */
					edtChar(edt_data->editor_handle,'*');/* hide security code */
#endif /* NEW_EDITOR */

					edt_data->editor_index++;
					check_set_pins(win,edt_data); /* digits check depends on status pin_case */
					if(edt_data->gsm_state)
						pin_data->gsm_state = edt_data->gsm_state;
				}
			}
			winShow(edt_data->pin_edt_win);
			break;

		case KCD_HASH:
		case KCD_LEFT:

			if(pin_emergency_call == TRUE)
			{
				callNumber((UBYTE *)pin_emerg_call);
				pin_emergency_call = FALSE;
#ifdef FF_PHONE_LOCK
				break;
#endif //FF_PHONE_LOCK
			}

			
			if(!(strncmp((char*)edt_data->pin,"**052*",IDENT_GSM_2))||!(strncmp((char*)edt_data->pin,"**05*",IDENT_GSM_1)))

			{
				edt_data->pin[edt_data->editor_index] = HASH;
				// SPR#1428 - SH - New Editor changes 
#ifdef NEW_EDITOR
				ATB_edit_AsciiChar(edt_data->editor, edt_data->pin[edt_data->editor_index], TRUE);
#else // NEW_EDITOR 
				edtChar(edt_data->editor_handle,edt_data->pin[edt_data->editor_index]);
#endif // NEW_EDITOR 
				edt_data->editor_index++;
			}

			

			// We save the pin2 entered in to this global variable which we
			// use while calling sAT_PlusCLCK(), while locking or unlocking ALS. 					
			memset(g_pin2,0,MAX_PIN+1);
			if(edt_data->pin_case_edit EQ INPUT_PIN2 || edt_data->pin_case_edit EQ INPUT_NEW_PIN2)
				strncpy(g_pin2,(char*)edt_data->pin,MAX_PIN);//xvilliva SPR17291
			switch(edt_data->pin_case_edit)
			{
//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
		 case INPUT_PH_LOCK:
#endif /*FF_PHONE_LOCK*/
				
					TRACE_EVENT_P1("pin_emergency_call = %d", pin_emergency_call);
					TRACE_EVENT("When 'Call' is pressed!");

					/* PIN is long enough, finish editing */
					
					if ((strlen((char*)edt_data->pin) >= MIN_PIN)  && !(edt_data->gsm_state))
					{
						clear_edit_array(edt_data);	    /*	   clear editor array */
						timer = FALSE;
						strncpy((char*)edt_data->old_pin,(char*)edt_data->pin,MAX_PIN);
						strncpy((char*)pin_data->old_pin, (char*)edt_data->old_pin,MAX_PIN);
						
     						 if(edt_data->pin_case_edit EQ INPUT_PH_LOCK)
                         			{
	                        			TRACE_EVENT("phlock_set_pin_kbd_cb: INPUT_PH_LOCK");
							if(phlock_win_handle)
								phlock_win_handle = NULL;
							
							if( phlock_kbd_handle)
								phlock_kbd_handle = NULL;

						    phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
							SEND_EVENT(pin_data->pin_win,PH_LOCK_END,0,pin_data);
						}
						
					}
					else
					{
						if(kc->code EQ KCD_HASH)
						{	
							if (!(edt_data->gsm_state))	/* could become gsm */
							{
								check_pins(win,edt_data);
								if(edt_data->gsm_state)
									pin_data->gsm_state = edt_data->gsm_state;
							}
							else /* finish gsm string */
							{
								timer = FALSE;
								if(edt_data->pin_case_edit EQ INPUT_PIN1)
									pin_data->pin_case = PIN1_REQ_OLD;
								else
									pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

							}
						}
					}
					break;

					
//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
				case INPUT_NEW_PH_LOCK:
#endif /*FF_PHONE_LOCK*/					
					if ((strlen((char*)edt_data->pin) >= MIN_PIN)  && !(edt_data->gsm_state))
					{
						clear_edit_array (edt_data);			  /* clear editor array */
						timer = FALSE;
						strncpy((char*)edt_data->new_pin, (char*)edt_data->pin,MAX_PIN);	  /*  store new pin	  */
						strncpy((char*)pin_data->new_pin, (char*)edt_data->new_pin,MAX_PIN);
						
				//x0pleela 06 Mar, 2007  ER: OMAPS00122561
				#ifdef  FF_PHONE_LOCK
					if(pin_data->pin_case EQ INPUT_NEW_PH_LOCK)
					{
                            		phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
                                   	SEND_EVENT(pin_data->pin_win,INPUT_NEW_PH_LOCK_AGAIN,0,NULL);
                                 	}
                             #endif /* FF_PHONE_LOCK */							
						
					}
					else
					{
						if(kc->code EQ KCD_HASH)
						{	
							if(!(edt_data->gsm_state)) /* could become gsm	    */
							{
								check_pins(win,edt_data);
								if(edt_data->gsm_state)
						       		pin_data->gsm_state = edt_data->gsm_state;
							}
							else /* finish gsm string  */
							{
								timer = FALSE;

								if(edt_data->pin_case_edit EQ INPUT_NEW_PIN)
									pin_data->pin_case = PIN1_REQ_OLD;
								else
									pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

							}
						}
					}
					break;
//x0pleela 06 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
				case INPUT_NEW_PH_LOCK_AGAIN:
#endif	/*FF_PHONE_LOCK*/					
				
						if ((strlen((char*)edt_data->pin) >= MIN_PIN) && !(edt_data->gsm_state))
						{
							clear_edit_array (edt_data);			  /* clear editor array */
							timer = FALSE;
							if(strcmp((char*)edt_data->pin, (char*)pin_data->new_pin) EQ 0) /* compare PINs successfull*/
							{
								strncpy((char*)pin_data->new_pin, (char*)edt_data->pin,MAX_PIN);
								
								//x0pleela 06 Mar, 2007 ER: OMAPS00122561
							#ifdef FF_PHONE_LOCK
								if (pin_data->pin_case EQ INPUT_NEW_PH_LOCK_AGAIN)
                                                   	{
                                                     		phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
                                                    		SEND_EVENT(pin_data->pin_win, NEW_PH_LOCK_END,0,pin_data);
                                                   	}
							#endif /*FF_PHONE_LOCK*/								
								
							}
							else
							{									/* new PIN != new PIN again */

								
								//x0pleela 06 Mar, 2007 ER: OMAPS00122561
							#ifdef FF_PHONE_LOCK
								if (pin_data->pin_case EQ INPUT_NEW_PH_LOCK_AGAIN)
                                                   	{
                                                     		phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
                                                    		SEND_EVENT(pin_data->pin_win, NEW_PH_LOCK_FAIL,0,pin_data);
                                                   	}
							#endif /*FF_PHONE_LOCK*/									
								
							}
						}
						else
						{
							if(kc->code EQ KCD_HASH)
							{
								if (!(edt_data->gsm_state)) /* could become gsm	    */
								{
									check_pins(win,edt_data);
									if(edt_data->gsm_state)
							       		pin_data->gsm_state = edt_data->gsm_state;
								}
								else /* finish gsm string  */
								{
									timer = FALSE;
									if(edt_data->pin_case_edit EQ INPUT_NEW_PIN_AGAIN)
										pin_data->pin_case = PIN1_REQ_OLD;
									else
										pin_data->pin_case = PIN2_REQ_OLD;
								//xrashmic 1 Jul, 2006 OMAPS00075784
								//Blue screen fix - Wrong window was being sent
								gsm_sec_execute_set(pin_data->pin_win,edt_data);	    /* execute gsm string */

								}
							}
						}
						break;
					
					default:
						break;
				}
				break;
				
/* KCD_HUP */
			case KCD_HUP:
				if( edt_data->pin_case_edit != INPUT_PUK1)//xvilliva SPR13623
				{
				clear_edit_array(edt_data);
				timer = FALSE;
//x0pleela 30 Mar, 2007  ER OMAPS00122561
#ifdef FF_PHONE_LOCK
			if( (!phlock_win_handle) || (pin_data->set_state != DISABLE) )
				{
#endif //FF_PHONE_LOCK				
				phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
	    		SEND_EVENT(pin_data->pin_win,PH_LOCK_SETT_ABORT,0,NULL); // abort settings menu
#ifdef FF_PHONE_LOCK
				}
#endif //FF_PHONE_LOCK	    		
				}
				break;

/* KCD RIGHT */

			case KCD_RIGHT: 	
			//nm, go back to the submenu if there is no character on the screen
				if(strlen((char*)edt_data->pin) == 0 && edt_data->pin_case_edit != INPUT_PUK1)//xvilliva SPR13623
				{
					clear_edit_array(edt_data);
					timer = FALSE;
//x0pleela 30 Mar, 2007  ER OMAPS00122561
#ifdef FF_PHONE_LOCK
			if( (!phlock_win_handle) || (pin_data->set_state != DISABLE) )
				{
#endif //FF_PHONE_LOCK
					phlock_set_pin_edt_destroy(edt_data->pin_edt_win);
		    		SEND_EVENT(pin_data->pin_win,PH_LOCK_SETT_ABORT,0,NULL); // abort settings menu
					return 1;			    
#ifdef FF_PHONE_LOCK
				}
#endif //FF_PHONE_LOCK					
				}

				if(edt_data->editor_index EQ 0)
					;
				else
					edt_data->editor_index--;

				edt_data->pin[edt_data->editor_index] = '\0';
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,ecBack);/* delete character    */
#endif /* NEW_EDITOR */

				if(strlen((char*)edt_data->pin)<MIN_PIN)
				{
				}

				edt_data->emergency_call = FALSE;
				if(edt_data->pin_case_edit EQ INPUT_PUK1)  /* special case ?? */
					winShow(edt_data->pin_edt_win);
				else
				{
					check_set_pins(win,edt_data);	       /* check remain for emergency call  */
					winShow(edt_data->pin_edt_win);
				}
			    break;
			    
			default:
				break;
		}
	}
//x0pleela 12 Mar, 2007  ER: OMAPS00122561
#if defined (SIM_PERS) || defined ( FF_PHONE_LOCK )
	else
#ifdef FF_PHONE_LOCK
    		if((mfw_phlock_get_lock_status(MFW_PH_LOCK) == MFW_PH_LOCK_ENABLE) || 
			(mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK) == MFW_PH_LOCK_ENABLE) )
#endif /*FF_PHONE_LOCK*/
	{
		switch(kc->code)
		{
		case KCD_0:
		case KCD_1:
		case KCD_2:
		case KCD_6:
		case KCD_8:
		case KCD_9:
			if(edt_data->editor_index < MAX_DIG)
			{
				edt_data->pin[edt_data->editor_index] = '0' + kc->code;

				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_AsciiChar(edt_data->editor, '*', TRUE);
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,'*');/* hide security code */
#endif /* NEW_EDITOR */

				edt_data->editor_index++;
				check_set_pins(win,edt_data); /* digits check depends on status pin_case */
				if(edt_data->gsm_state)
					pin_data->gsm_state = edt_data->gsm_state;
			}
			winShow(edt_data->pin_edt_win);
			break;
				
	
		case KCD_LEFT:

			if(pin_emergency_call == TRUE)
			{
				callNumber((UBYTE *)pin_emerg_call);
				pin_emergency_call = FALSE;
			}
			break;

		case KCD_RIGHT: 	

//				if( edt_data->edtbuf == '\0' )
//					break;
				if(edt_data->editor_index EQ 0)
					;
				else
					edt_data->editor_index--;

				edt_data->pin[edt_data->editor_index] = '\0';
				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_DeleteLeft(edt_data->editor, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
				edtChar(edt_data->editor_handle,ecBack);/* delete character    */
#endif /* NEW_EDITOR */

				if(strlen((char*)edt_data->pin)<MIN_PIN)
				{
				}

				edt_data->emergency_call = FALSE;
				if(edt_data->pin_case_edit EQ INPUT_PUK1)  /* special case ?? */
					winShow(edt_data->pin_edt_win);
				else
				{
					check_set_pins(win,edt_data);	       /* check remain for emergency call  */
					winShow(edt_data->pin_edt_win);
				}
			    break;

			default:
				break;
			}	
	}
#endif /* SIM_PERS  OR FF_PHONE_LOCK*/
	if (timer)
		tim_start(edt_data->tim_out_handle);/* start timer for entering */
	return 1;
}



/*******************************************************************************

 $Function:	phlock_set_pin_edt_kbd_long_cb
 $Description:	Callback function for keyboard long
 $Returns:		status int
 $Arguments:	event, keyboard control block
*******************************************************************************/
static int phlock_set_pin_edt_kbd_long_cb (T_MFW_EVENT event,T_MFW_KBD *  kc)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;//pin edt data

    TRACE_FUNCTION("MmiPins:phlock_set_pin_edt_kbd_long_cb");

    if ((event & KEY_CLEAR) && (event & KEY_LONG))
	{
			timStop(edt_data->tim_out_handle); /* stop entering timer	*/
			clear_edit_array(edt_data);
			memset(edt_data->pin,'\0',sizeof(edt_data->pin));/* initial string */
			
			/* SPR#1428 - SH - New Editor: string changed, update word-wrap*/
	#ifdef NEW_EDITOR
			ATB_edit_Refresh(edt_data->editor);
	#endif /* NEW_EDITOR */
			edt_data->editor_index = 0;
			edt_data->emergency_call = FALSE;
			winShow(edt_data->pin_edt_win);
	}

  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:	phlock_pin_edt_tim_out_cb
 $Description:	Callback function for timer (watchdog entering)
 $Returns:		void
 $Arguments:	window handler event, timer control block 
 
*******************************************************************************/
static void phlock_pin_edt_tim_out_cb (T_MFW_EVENT event,T_MFW_TIM * t)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	T_MFW_WIN * win_data = ((T_MFW_HDR *)win)->data;
	T_pin_edt * edt_data = (T_pin_edt *)win_data->user;

	T_MFW_WIN * win_pin =((T_MFW_HDR *)edt_data->parent_win)->data;
	T_pin * pin_data = (T_pin *)win_pin->user; // pin main data

    TRACE_FUNCTION("MmiPins:phlock_pin_edt_tim_out_cb");


    TRACE_FUNCTION("clear softkey 7");

	// clear the softkeys
	pin_skClear();
	clear_edit_array(edt_data);
	// clear the input
	
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	ATB_edit_ClearAll(edt_data->editor);
#else /* NEW_EDITOR */
	pin_rectClear( &edt_data->editpinAttr.win );
    memset(edt_data->pin, '\0', sizeof(edt_data->pin));/* initial string  */
#endif /* NEW_EDITOR */

	edt_data->editor_index = 0;
	edt_data->gsm_state = FALSE;
	pin_data->gsm_state = FALSE;

	winShow(edt_data->pin_edt_win);
}

/*******************************************************************************

 $Function:	phlock_setting_pin_destroy 
 $Description:	Destroy the settings pin main dialog.
 $Returns:		none
 $Arguments:	window
*******************************************************************************/
void phlock_setting_pin_destroy  (T_MFW_HND own_window)
{
  T_pin * data;
  T_MFW_WIN * win;

  if (own_window)
  {
	 TRACE_FUNCTION("MmiPins:phlock_setting_pin_destroy");

	 win = ((T_MFW_HDR *)own_window)->data;
     data = (T_pin *)win->user;

	 if(data)
	 {
      /*
       * Exit SIM and Delete SIM Handler
       */
	 sim_delete (data->sim_handle);

      /*
       * Delete WIN Handler
       */
	  set_pin_windows = 0;
      win_delete (data->pin_win);
      data->pin_win=0;
	if(data)
	 FREE_MEMORY((void *)data,(sizeof(T_pin)));
	 }
  }
}

/*******************************************************************************

 $Function:	phlock_set_pin_edt_destroy
 $Description:	Destroy the pin editor dialog
 $Returns:		none
 $Arguments:	window
*******************************************************************************/
void phlock_set_pin_edt_destroy(T_MFW_HND own_window)
{
  T_pin_edt * data ;
  T_MFW_WIN * win;

  if (own_window)
  {
	  TRACE_FUNCTION("MmiPins:phlock_set_pin_edt_destroy");

	  win = ((T_MFW_HDR *)own_window)->data;
	  data = (T_pin_edt *)win->user;

	  if(data)
	  {
      /*
       * Delete WIN Handler
       */
	      win_delete (data->pin_edt_win);
	      FREE_MEMORY((void *)data,(sizeof(T_pin_edt)));
	  }
   }
}
#endif /* FF_PHONE_LOCK */



