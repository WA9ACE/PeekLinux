/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Settings
 $File:		    MmiSettings.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

	
********************************************************************************
 $History: MmiSettings.c
 
	Mar 28, 2007  DR: OMAPS00122762 x0039928
	Description: MM: Deleting a PCM Voice Memo message in one particular memory, 
	delete them in all memories
	Solution: voice memo position and pcm voice memo position is provided for all the devices.
	
      Oct 30 2006, OMAPS00098881 x0039928(sumanth)
      Removal of power variant

 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Updating function "setUserSettings2Default ()" to memset Line1 and line2
				fields of FFS_flashData
	
	Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
   	Description: Need to reduce flash foot-print for Locosto Lite 
   	Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
   	out voice memo feature if the above flag is enabled.
   	
     	May 04, 2005  REF: CRR 30285  x0021334
    	Description: SMS Status Request in idle and dedicated mode (using Menu) setting does not work
    	Solution: status of sms status request is read and focus is set accrodingly.

       March 2, 2005    REF: CRR 11536 x0018858
       Description: Reply path not supported while sending a reply.
       Solution: Added menu option for the reply path.

	Aug 25, 2004    REF: CRR 24904   Deepa M.D
	Bug:cleanup compile switches used for Go-lite
	Fix:COLOURDISPLAY compiler switch has been added to the functions which are 
	used only for the normal color build.

        xrashmic 7 Dec, 2004 MMI-SPR-23965
        Description: Not able to extract the objects in EMS message
        Solution: Extract and store the objects in the Object Manager

        xrashmic 29 Nov, 2004 MMI-SPR-26161
        Description: The downloaded object through wap was not visible to the MMI
        Solution: The downloaded wap object is available in the object manager 
        and can be used by the MMS
 
 	Jul 28, 2004 REF: CRR 20899 xkundadu
	Description: No identification for user whether 'Any-key-Answer' is On/Off
	Solution: The menu focus will be on the 'On' menu  if the 'Any key 
			answer' is enabled, otherwise focus will be on 'Off' menu item.
			Changed "submenu" to "menuItem" to call a function, to set focus to
			the selected option.Added  function settingsFocusCurSelAnyKeyAns()
			to set the focus to selected item.
 

	Jul 29, 2004 REF: CRR 20898 xkundadu
	Description: No identification for user whether 'Auto-Answer' is On/Off
	Solution: The menu focus will be on the 'On' menu  if the 'Auto 
			answer' is enabled, otherwise focus will be on 'Off' menu item.
			Changed 'submenu' to 'menuItem' to call a function, which sets the
			focus to the selected option. Added the function 
			settingsFocusCurSelAutoAns() to set the focus to selected item.
 

	 Jul 22,2004 CRR:20896 xrashmic - SASKEN
	 Description: The current time format is not highlighted
	 Fix: Instead of providing the submenu directly we call a function 
	 where we set the menu and also higlight the previously selected menu item
	 
//  Issue Number : SPR#12822 on 31/03/04 by vvadiraj
	May 10, 2004    REF: CRR 15753  Deepa M.D 
	Menuitem provided to set the validity period to maximum.
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
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_cphs.h"
#include "mfw_sat.h"
#include "mfw_td.h"

#include "mfw_mme.h"//xashmic 27 Sep 2006, OMAPS00096389 

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiIdle.h"
#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiSounds.h"
#include "MmiMenu.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiSettings.h"

#ifndef FF_NO_VOICE_MEMO
#include "MmiVoiceMemo.h"
#endif

#include "mfw_ffs.h"
#include "MmiTimers.h"
#include "MmiResources.h"

#include "cus_aci.h"


#include "mmiColours.h"

#define CLEAR_BOTTOM_ZONE dspl_Clear( KEY_AREA );
T_MFW_SMS_INFO SmsConfigData;

//ADDED BY RAVI - 28-11-2005
extern UBYTE SmsSend_get_config_data (T_MFW_SMS_INFO *config_data);
// END ADD BY RAVI

static int winEvent (MfwEvt e, MfwWin *w);

void confirm (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void confirm_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
static void confirm_dialog(void);
static int confirm_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
void confirm_destroy (T_MFW_HND own_window);
T_MFW_HND confirm_create (T_MFW_HND parent_window);
void confirm_Exit (T_MFW_HND own_window);
T_MFW_HND confirm_Init (T_MFW_HND parent_window);
void setUserSettings2Default (void);


static void (*show) (void);

static MfwHnd win;                      /* our window               */
/* static MmiState nextState;              // next state when finished   x0039928 - Lint warning fix */
/* static DisplayData DisplayInfo;   x0039928 - Lint warning fix */



#define SHOW_CONFIRM           3
#define ACTIVATE_CONFIRM       4
#define DEACTIVATE_CONFIRM     5
#define FACTORY_RESET          6

UBYTE reply_path_bit =0;

typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        confirm_win;        /* MFW win handler      */
  UBYTE 		   local_status;
} T_confirm;

T_confirm confirm_data;


/*
 * Settings menu subs:
 */




#define ON  	0
#define OFF		1
#define CLEAR	2

	
typedef struct 
{
    UBYTE setting;
    UBYTE status;
} T_settings;

//GW Added for settings not stored in FFS
static unsigned int BMIsettings=0;  /* x0039928 - Lint warning fix */

typedef enum 
{
    ENGLISH = 0,
    GERMAN,
    AUTOREDIAL,
    AUTOANSWER,
    ANYKEYANSWER,
    CALLTIMERS,
    KEYPAD_CLICK,
 	KEYPAD_DTMF,
    KEYPAD_SILENT,
#ifndef FF_NO_VOICE_MEMO
    VOICE_MEMO,
#endif
#ifdef FF_POWER_MANAGEMENT
    LCD_REFRESH,   /*OMAPS00098881 (removing power variant) a0393213(Prabakar)*/
#endif
    MAX
}T_FACTORY_RESET_LIST;

  

T_settings DEFAULT_SETTINGS[] = 
{ 
	{ENGLISH,		ON},		//select the Language:English as a default
	{GERMAN,		OFF},   
	{AUTOREDIAL,	OFF},		
	{AUTOANSWER,	OFF},		
	{ANYKEYANSWER,	OFF},		
	{CALLTIMERS,	CLEAR},		
	{KEYPAD_CLICK,	ON},		//only one of the Keypads are eligible
	{KEYPAD_DTMF,	OFF},		
	{KEYPAD_SILENT,	OFF},
#ifndef FF_NO_VOICE_MEMO
	{VOICE_MEMO,	CLEAR},		
#endif
#ifdef FF_POWER_MANAGEMENT
	{LCD_REFRESH,           ON}, /*OMAPS00098881 (removing power variant) a0393213(Prabakar)*/
#endif
	{MAX,			0xFF}   
};

/*******************************************************************************

 $Function:    	SmsValidityPeriod

 $Description:	Store the validity period information, selected by the user.
			
 
 $Returns:		Execution status
 
 $Arguments:	m - menu handler
 				i - Menu item selected
 
*******************************************************************************/
int SmsValidityPeriod(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_SMS_INFO	sms_parameter;
	UBYTE index = 0;  // RAVI - Assigned 0.
	T_DISPLAY_DATA DisplayInfo;
	int txtId = 0;  // RAVI - Assigned 0.
		
    T_MFW_HND win  = mfwParent(mfw_header());
	
    TRACE_FUNCTION(">>>> SmsValidityPeriod()");

      /*SPR#1952 - DS - Show the new period setting on pop-up dialog */
	switch (m->lCursor[m->level])
	{
	case 0:
		txtId = Txt1Hour;
		index = VP_REL_1_HOUR;
		break;
	case 1:
		txtId = Txt12Hours;
		index = VP_REL_12_HOURS;
		break;
	case 2:
		txtId = Txt24Hours;
		index = VP_REL_24_HOURS;
		break;
	case 3:
		txtId = Txt1Week;
		index = VP_REL_1_WEEK;
		break;
	case 4:
		txtId = TxtMaxTime;
		index = VP_REL_MAXIMUM;    //  May 10, 2004    REF: CRR 15753  Deepa M.D 
		break;
		
	}

	/* API - 28/07/03 - CQ10203 -  Read the Data and then set the REL VP identifier*/
	SmsSend_get_config_data(&sms_parameter);

	FFS_flashData.vp_rel = index;
	flash_write();
	
	sms_parameter.vp_rel = FFS_flashData.vp_rel;
	TRACE_EVENT_P1("sms_parameter.vp_rel = %d", sms_parameter.vp_rel);
	/*API - CQ10203 END*/

	if (sms_set_val_period(&sms_parameter) == MFW_SMS_OK)
		dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtCancel,TxtSmsValidityPeriodSet, txtId /*SPR#1952*/, COLOUR_STATUS);
	else
		dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtCancel,TxtFailed, TxtNull, COLOUR_STATUS);

	dlg_initDisplayData_events( &DisplayInfo, (T_VOID_FUNC)NULL, THREE_SECS, KEY_RIGHT|KEY_CLEAR );
	
	// Display confirmation screen.
	info_dialog(win,&DisplayInfo);
	
	return 1;
}


/* Marcus: Issue 1170: 30/09/2002: Start */
/*******************************************************************************

 $Function:    	SmsStatusReportRequest

 $Description:	Activate/Deactivate SRR, selected by the user.
			
 $Returns:		Execution status
 
 $Arguments:	m - menu handler
 				i - Menu item selected
 
*******************************************************************************/
int SmsStatusReportRequest(MfwMnu* m, MfwMnuItem* i)
{
	int index = m->lCursor[m->level];
	T_DISPLAY_DATA DisplayInfo;
    T_MFW_SMS_INFO sms_info;
    T_MFW_HND win  = mfwParent(mfw_header());
	int TextId2;
    TRACE_FUNCTION(">>>> SmsStatusReportRequest()");

    if (sms_parameter(&sms_info, TRUE) == MFW_SMS_OK)
    {
        if (index == 0)
        	sms_info.srr = SMS_NOT_REQUESTED_SRR;
        else
        	sms_info.srr = SMS_REQUESTED_SRR;
        
        if (sms_parameter(&sms_info, FALSE) == MFW_SMS_OK)
        {
   	
            if (index == 0)
            	TextId2      = TxtOff;
            else
            	TextId2      = TxtOn;

		dlg_initDisplayData_TextId( &DisplayInfo, TxtSoftOK, TxtNull,TxtSRR, TextId2, COLOUR_POPUP);
		dlg_initDisplayData_events( &DisplayInfo, (T_VOID_FUNC)NULL, THREE_SECS, KEY_RIGHT | KEY_LEFT );
		
        	info_dialog(win,&DisplayInfo);
        }
    }
	return 1;
}
/* Marcus: Issue 1170: 30/09/2002: End */

/* 
March 2, 2005    REF: CRR 11536 x0018858
Added a function to set the Reply path  
*/
//begin -11536
/*******************************************************************************

 $Function:    	SmsReplyPathRequest

 $Description:	Activate/Deactivate  reply path, selected by the user.
			
 $Returns:		Execution status
 
 $Arguments:	m - menu handler
 				i - Menu item selected


*******************************************************************************/

int SmsReplyPathRequest(MfwMnu* m, MfwMnuItem* i)
{
	int index = m->lCursor[m->level];
	T_DISPLAY_DATA DisplayInfo;
    T_MFW_SMS_INFO sms_info;
    T_MFW_HND win  = mfwParent(mfw_header());
	int TextId2;
    TRACE_FUNCTION(">>>> SmsReplyPathRequest()");

    if (sms_parameter(&sms_info, TRUE) == MFW_SMS_OK)
    {
        if (index == 0)
            reply_path_bit= SMS_UNSET_RP;
        else
            reply_path_bit= SMS_SET_RP;

        TRACE_EVENT_P1("The reply paTH BIT IS  = %d", reply_path_bit);
   	
            if (index == 0)
            	TextId2      = TxtOff;
            else
            	TextId2      = TxtOn;

		dlg_initDisplayData_TextId( &DisplayInfo, TxtSoftOK, TxtNull,TxtRP, TextId2, COLOUR_POPUP);
		dlg_initDisplayData_events( &DisplayInfo, (T_VOID_FUNC)NULL, THREE_SECS, KEY_RIGHT | KEY_LEFT );
		
        	info_dialog(win,&DisplayInfo);
        
    }
	return 1;
}
// End --11536


/*******************************************************************************

 $Function:    	settingsVoiceMail

 $Description:	menu entry function

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsVoiceMail(MfwMnu* m, MfwMnuItem* i)

{

	cphs_get_mailbox ();


    return 0;
}

/*******************************************************************************

 $Function:    	settingsVoiceMail

 $Description:	menu entry function

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsScNumber(MfwMnu* m, MfwMnuItem* i)
{

    return 0;
}
/*******************************************************************************

 $Function:    	settingsInit

 $Description:	init settings handling

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void settingsInit (void)
{

    TRACE_EVENT ("settingsInit()");

   /*  nextState = 0;  x0039928 - Lint warning fix */

    win = winCreate(0,0,MfwWinVisible,(MfwCb)winEvent);
    winShow(win);
}

/*******************************************************************************

 $Function:    	settingsExit

 $Description:	exit settings handling

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void settingsExit (void)
{
    TRACE_FUNCTION ("settingsExit()");

 /*    nextState = 0;  x0039928 - Lint warning fix */
    winDelete(win); win = 0;
}


/*******************************************************************************

 $Function:    	winEvent

 $Description:	window event handler

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static int winEvent (MfwEvt e, MfwWin *w)
{
    TRACE_EVENT ("MmiSettings :: winEvent()");

    switch (e)
    {
        case MfwWinVisible:
            if (w->flags & MfwWinVisible)
                show();
            break;
        default:
            return 0;
    }

    return 1;
}
/*******************************************************************************

 $Function:    	settingsGetStatus

 $Description:	Function to return the status of various BMI settings

 $Returns:		Non-zero number if setting is ON
 				Zero if the setting is OFF

 $Arguments:	flagBit - value indicating which setting is to be changed.
 
*******************************************************************************/
int settingsGetStatus(int flagBit)
{
	if (flagBit & SettingsKeyPadLockOn)
	{
		return(BMIsettings & flagBit);
	}
	else
	{
		return(FFS_flashData.settings_status & flagBit);	
	}
}
/*******************************************************************************

 $Function:    	settingsSetStatus

 $Description:	Function to set or clear the status of various BMI settings

 $Returns:		None.

 $Arguments:	flagBit	- value indicating which setting is to be changed.
 				state 	- SET_SETTING - set the value
 						- CLEAR_SETTING - clear the value. 
GW SPR#2470 Created
*******************************************************************************/
void settingsSetStatus(unsigned int flagBit, int value)
{
	if (flagBit & SettingsKeyPadLockOn)
	{
		if (value == SET_SETTING)
			BMIsettings = BMIsettings | flagBit;
		else
			BMIsettings = BMIsettings & (~flagBit);
	}
	else
	{
		if (value == SET_SETTING)
			FFS_flashData.settings_status = FFS_flashData.settings_status | flagBit;
		else
			FFS_flashData.settings_status = FFS_flashData.settings_status & (~flagBit);
	}
	flash_write();
}
/*******************************************************************************

 $Function:    	settingsAutoRedialOn

 $Description:	auto redial switch on

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAutoRedialOn(MfwMnu* m, MfwMnuItem* i)
{

   T_MFW_HND win = mfw_parent(mfw_header());

	confirm_Init(win);

	cm_set_redial(CM_REDIAL_AUTO);


    confirm_data.local_status = ACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);

	return 0;
}
/*******************************************************************************

 $Function:    	settingsAutoRedialOff

 $Description:	auto redial switch off

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAutoRedialOff(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());
 	confirm_Init(win);

	cm_set_redial(CM_REDIAL_OFF);

    confirm_data.local_status = DEACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);



	return 0;
}
/*******************************************************************************

 $Function:    	settingsSilentModeOn

 $Description:    Activate the silent Mode

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsSilentModeOn(void)
{
	TRACE_FUNCTION(">> settingsSilentModeOn()");
	FFS_flashData.settings_status |= SettingsSilentMode;
	flash_write();
	setSilentModeVolume();
	iconsSetState(iconIdSilRin);
	return 0;
}
/*******************************************************************************

 $Function:    	settingsSilentModeOff

 $Description:	Deactivate the silent Mode

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsSilentModeOff(void)
{
	TRACE_EVENT(">> settingsSilentModeOff()");
	FFS_flashData.settings_status &= ~SettingsSilentMode;
	flash_write();
	restoreSilentModeVolume();
	iconsDeleteState(iconIdSilRin);
	return 0;
}

/*******************************************************************************

 $Function:    	settingsKeyPadLockOn

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsKeyPadLockOn(void)
{
	BMIsettings = BMIsettings | SettingsKeyPadLockOn;
	iconsSetState(iconIdKeyplock);
	idleEvent(IdleActiveKeyPadLocked);
	return 0;
}
/*******************************************************************************

 $Function:    	settingsKeyPadLockOff

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsKeyPadLockOff(void)
{
	BMIsettings = BMIsettings & ~SettingsKeyPadLockOn;
	iconsDeleteState(iconIdKeyplock);

	return 0;
}
/*******************************************************************************

 $Function:    	settingsAutoAnswerOn

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAutoAnswerOn(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	confirm_Init(win);

	FFS_flashData.settings_status |= SettingsAutoAnswerOn;
	flash_write();
    confirm_data.local_status = ACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);

	return 0;
}
/*******************************************************************************

 $Function:    	settingsAutoAnswerOff

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAutoAnswerOff(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	confirm_Init(win);

	FFS_flashData.settings_status &= ~SettingsAutoAnswerOn;
	flash_write();
    confirm_data.local_status = DEACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);



	return 0;
}


/*******************************************************************************

 $Function:    	settingsAnyKeyAnswerOn

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAnyKeyAnswerOn(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND win = mfw_parent(mfw_header());

	confirm_Init(win);

	FFS_flashData.settings_status |= SettingsAnyKeyAnswerOn;
	flash_write();
    confirm_data.local_status = ACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);
	return 0;
}
/*******************************************************************************

 $Function:    	settingsAnyKeyAnswerOff

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int settingsAnyKeyAnswerOff(MfwMnu* m, MfwMnuItem* i)
{

    T_MFW_HND win = mfw_parent(mfw_header());

	confirm_Init(win);
	
	FFS_flashData.settings_status &= ~SettingsAnyKeyAnswerOn;
	flash_write();
    confirm_data.local_status = DEACTIVATE_CONFIRM;

	SEND_EVENT (confirm_data.confirm_win, SHOW_CONFIRM, 0, 0);

	return 0;
}

/*******************************************************************************

 $Function:    	factoryReset

 $Description:	Reset all the user-settings, 
 				At first it show up a confirmation screen 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int factoryReset(MfwMnu* m, MfwMnuItem* i)
{

    T_MFW_HND win = mfw_parent(mfw_header());
	
	confirm_Init(win);

	confirm_data.local_status = FACTORY_RESET;

	SEND_EVENT (confirm_data.confirm_win, FACTORY_RESET, 0, 0);
	return 0;
}



/*******************************************************************************

 $Function:    	setUserSettings2Default

 $Description:	The user confirm to delete the user-settings. 

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void setUserSettings2Default (void)
{
	//char buf[30];  // RAVI



	if (DEFAULT_SETTINGS[ENGLISH].status EQ ON)
	{
		// set the language :ENGLISH
		MmiRsrcSetLang(ENGLISH_LANGUAGE);/*MC, SPR 1242 - changed to new function*/
	}


	if (DEFAULT_SETTINGS[GERMAN].status EQ ON)
	{
		// set the language :GERMAN
		MmiRsrcSetLang(GERMAN_LANGUAGE);	/*MC, SPR 1242 - changed to new function*/
	}


	if (DEFAULT_SETTINGS[AUTOREDIAL].status EQ ON)
	{
		// set the Auto-redial
		cm_set_redial(CM_REDIAL_AUTO);	
	}
	else
	{
		cm_set_redial(CM_REDIAL_OFF);
	}


	if (DEFAULT_SETTINGS[AUTOANSWER].status EQ ON)
	{
		// set the Autoanswer
		FFS_flashData.settings_status |= SettingsAutoAnswerOn;
	}
	else
	{
		//switch off the Autoanswer
		FFS_flashData.settings_status &= ~SettingsAutoAnswerOn;
	}


	if (DEFAULT_SETTINGS[ANYKEYANSWER].status EQ ON)
	{
		// set the Anykeyanswer
		FFS_flashData.settings_status |= SettingsAnyKeyAnswerOn;
	}
	else 
	{
		//switch off the Anykeyanswer
		FFS_flashData.settings_status &= ~SettingsAnyKeyAnswerOn;
	}

	if (DEFAULT_SETTINGS[CALLTIMERS].status EQ CLEAR)
	{
		//reset here the call timers
		//incoming, outgoing & last calls
	}


	if (DEFAULT_SETTINGS[KEYPAD_CLICK].status EQ ON)
	{
		// set the Keypad click

		if (FFS_flashData.settings_status & SettingsKeypadDTMF)
		{
			FFS_flashData.settings_status &= ~SettingsKeypadDTMF;
		}

		FFS_flashData.settings_status |= SettingsKeypadClick;
	}


	if (DEFAULT_SETTINGS[KEYPAD_DTMF].status EQ ON)
	{
		// set the Keypad DTMF

		if (FFS_flashData.settings_status & SettingsKeypadClick)
		{
			FFS_flashData.settings_status &= ~SettingsKeypadClick;
		}

		FFS_flashData.settings_status |= SettingsKeypadDTMF;
	}


	if (DEFAULT_SETTINGS[KEYPAD_SILENT].status EQ ON)
	{
		// set the Keypad silence

		if (FFS_flashData.settings_status & SettingsKeypadClick)
		{
			FFS_flashData.settings_status &= ~SettingsKeypadClick;
		}
		else if (FFS_flashData.settings_status & SettingsKeypadDTMF)
		{
			FFS_flashData.settings_status &= ~SettingsKeypadDTMF;
		}
	
	}

	//GW-SPR#1018- Trace is too big for the buffer and is causing a crash.
	//  sprintf (buf, " FFS_flashData.settings_status %d", FFS_flashData.settings_status);
    //  TRACE_EVENT (buf);

       //xrashmic 7 Dec, 2004 MMI-SPR-26161 and MMI-SPR-23965
       // Resetting the downloaded and extracted wap objects
       FFS_flashData.image_usr_obj=0;
	FFS_flashData.audio_usr_obj =0;

#ifndef FF_NO_VOICE_MEMO
	if (DEFAULT_SETTINGS[VOICE_MEMO].status EQ CLEAR)
	{
	/* Mar 28, 2007  DR: OMAPS00122762 x0039928 */
#ifndef FF_MMI_FILEMANAGER	
		// clear the voice memo
		FFS_flashData.voice_memo_position = 0;
#endif
	
	}
#endif

/*OMAPS00098881(removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
      if(DEFAULT_SETTINGS[LCD_REFRESH].status EQ ON)
      	{
      		FFS_flashData.refresh=1; /*lcd refresh needed*/
      	}
	else
	{
		FFS_flashData.refresh=0; /*lcd refresh not needed*/
	}
#endif

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
	memset(FFS_flashData.line1,'\0', sizeof(FFS_flashData.line1));
	memset(FFS_flashData.line2,'\0', sizeof(FFS_flashData.line2));
#endif

	//reset the call timers
	setLastCallDuration(0, MFW_CM_INV_DIR);
	resetIncomingCallsDuration();
	resetOutgoingCallsDuration();
	//set clock format to twenty-four hour
	setClockFormat(MFW_TIME_FORMAT_24HOUR);/*SPR 1725, changed macro*/

	// API/GW - On factory reset save the original colours to the flash.
	resetFactoryColour();
	//API - 1550 - Added code to reset the backgrounds to defaults
	resetFactoryBackground();

	/*API - 28/07/03 - CQ10203 - Add a call to set the validity period to default on Factory Reset*/
	resetSMSValidityPeriod();
	/*API - CQ10203 - END*/
	
	// PCM
	// here should also update the user-data-settings !!
	// in PCM
//xashmic 27 Sep 2006, OMAPS00096389	
//xashmic 9 Sep 2006, OMAPS00092732	
//By default PS shutdown is disabled
#ifdef FF_MMI_USBMS
	FFS_flashData.usb_ms_flags=MFW_USBMS_DEFAULT;
#endif

	flash_write();


}

/*******************************************************************************

 $Function:    	confirm_Init

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND confirm_Init (T_MFW_HND parent_window)
{
  return (confirm_create (parent_window));
}
/*******************************************************************************

 $Function:    	confirm_Exit

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void confirm_Exit (T_MFW_HND own_window)
{

    T_MFW_WIN   * win_data = ((T_MFW_HDR *)own_window)->data;
    T_confirm * data     = (T_confirm *)win_data->user;

  confirm_destroy (data->confirm_win);
}

/*******************************************************************************

 $Function:    	confirm_create

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND confirm_create (T_MFW_HND parent_window)
{
  T_confirm * data = &confirm_data;
  T_MFW_WIN   * win;

  TRACE_FUNCTION ("confirm_create()");

  data->confirm_win = win_create (parent_window, 0, 0, (T_MFW_CB)confirm_win_cb);

  if (data->confirm_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog = (T_DIALOG_FUNC)confirm;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->confirm_win)->data;
  win->user                = (void *) data;

 
  winShow(data->confirm_win);
  /*
   * return window handle
   */
  return data->confirm_win;
}

/*******************************************************************************

 $Function:    	confirm_destroy

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void confirm_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN   * win;
  T_confirm * data;

  TRACE_FUNCTION ("confirm_destroy()");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_confirm *)win->user;

    if (data)
    {

      /*
       * Delete WIN handler
       */
      win_delete (data->confirm_win);
      data->confirm_win = 0;
    }
  }
}
/*******************************************************************************

 $Function:    	confirm_win_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static int confirm_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}


/*******************************************************************************

 $Function:    	confirm_dialog

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void confirm_dialog(void)
{
  T_DISPLAY_DATA display_info;

  TRACE_FUNCTION ("confirm_dialog()");


	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull,TxtNull, TxtNull , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)confirm_dialog_cb, THREE_SECS, KEY_CLEAR | KEY_RIGHT | KEY_LEFT );

  if (confirm_data.local_status EQ ACTIVATE_CONFIRM)
  {
	  display_info.TextId   = TxtActivated;
  }
  else if (confirm_data.local_status EQ DEACTIVATE_CONFIRM)
  {
     display_info.TextId    = TxtDeActivated;
  }
  else if (confirm_data.local_status EQ FACTORY_RESET)
  {
     display_info.TextId      = TxtPressOk;
	 display_info.TextId2     = TxtReset;
	 display_info.Time       = TEN_SECS;
     display_info.LeftSoftKey  = TxtSoftOK;
     display_info.RightSoftKey = TxtSoftBack;

  }

  /*
   * Call Info Screen
   */
  info_dialog (confirm_data.confirm_win, &display_info);

}
/*******************************************************************************

 $Function:    	confirm_dialog_cb

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void confirm_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION ("confirm_dialog_cb()");

      switch (reason) 
      {
        case INFO_KCD_LEFT:
			// user confirm to reset all the user settings !
			setUserSettings2Default ();
            confirm_Exit(win);
          break;          

        case INFO_TIMEOUT:
          /* no break; */
        case INFO_KCD_HUP:
          /* no break; */
        case INFO_KCD_RIGHT:
          /* no break; */
        case INFO_KCD_CLEAR:
            confirm_Exit(win);
          break;
      }
}

/*******************************************************************************

 $Function:    	confirm

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void confirm (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
 // T_MFW_WIN    * win_data = ((T_MFW_HDR *) win)->data;  // RAVI
 // T_confirm      * data     = (T_confirm *)win_data->user;    // RAVI

 TRACE_FUNCTION("confirm()");

 switch (event)
  {
    case SHOW_CONFIRM:
		confirm_dialog();
    break;
    case FACTORY_RESET:
		confirm_dialog();
    break;

  }

}
static void showSet (void)
{

    T_DISPLAY_DATA   display_info;

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtActivated,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_HUP| KEY_LEFT );

	info_dialog ( NULL, &display_info);
}

static void showClear (void)
{

    T_DISPLAY_DATA   display_info;

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtDeActivated,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_HUP| KEY_LEFT );

	info_dialog ( NULL, &display_info);
}

//    MENU_ITEM ( TxtCheckStatus,   servicesDivertCheck,     item_flag_none ),
void settingsShowStateAnykeyAnswer (void)
{
	if (settingsGetStatus(SettingsAnyKeyAnswerOn))
		showSet();
	else
		showClear();
}
void settingsShowStateAutoAnswer (void)
{
	if (settingsGetStatus(SettingsAutoAnswerOn))
		showSet();
	else
		showClear();
}
void settingsShowStateAutoRedial(void)
{
	if (FFS_flashData.redial_mode == CM_REDIAL_AUTO)
		showSet();
	else
		showClear();
}
/*GW SPR#2470 Created*/
void settingsShowStateCBDisable(void)
{
	if (settingsGetStatus(SettingsCBDisable))
		showClear();
	else
		showSet();
}

// change by Sasken (VVADIRAJ) on March 31th 2004
// Issue Number : MMI-SPR-12822
// Subject: Highlighting the previously selected menu item.
// Bug : Not highlighting the previously selected menu item.Selecting first item always.
// Solution: Instead of providing the submenu directly we call this function
//			 where we set the menu and also higlight the previously selected menu item.

/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24904   Deepa M.D
//This function is used only for the Color build.Hence it is put under the 
//COLOURDISPLAY compiler switch
#ifdef COLOURDISPLAY 
void settingsFocusNwIconCurSel(void)
{
	UBYTE			SaveNwIcon;
	T_MFW_HND 		setting_win;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	
	setting_win= bookMenuStart(win, NWIconAttributes(), 0);
	SaveNwIcon=(FFS_flashData.ProviderNetworkShow == TRUE)?0:1;
	SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &SaveNwIcon);
}
#endif 
/***************************Go-lite Optimization changes Start***********************/
//SPR 12822:VVADIRAJ

//	Jul 29, 2004 REF = CRR 20898 xkundadu
/*******************************************************************************
 $Function:    	settingsFocusCurSelAutoAns
 
 $Description:	This function sets the focus to the On or Off menu item of the 
 			'Auto Answer' menu in the Phone Settings main menu.
 			Check whether the 'Auto Answer' is enabled by reading the data
 			stored in the FFS. Depending on that, set the focus to the menu 
 			item.
 			
 $Returns:	void	
 
 $Arguments:	void
 *******************************************************************************/


void settingsFocusCurSelAutoAns(void)
{
	// Points to the index of the menuitem to be selected
	// If it is 1, 'Off' menuItem will be selected, else 'On' menuitem
	// will be selected.
	UBYTE			menuItemIndex = 1;
	
	T_MFW_HND 		setting_win;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	    
	TRACE_FUNCTION ("settingsFocusCurSelAutoAns() ");

	// Starting the menu corresponding to AutoAnswer.
	setting_win = bookMenuStart(win, GetAutoAnswerMenu(), 0);

	//Checking whether the 'Auto Answer' is enabled or not
	//Reading the value from FFS.
	if( (FFS_flashData.settings_status & SettingsAutoAnswerOn)
		EQ SettingsAutoAnswerOn)
	{
		menuItemIndex =  0;
	}
	
	// Send the event to display the menu item and appropriate menuitem 
	// selected.
	SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &menuItemIndex);	
	
}

// Jul 22,2004 CRR:20896 xrashmic - SASKEN
// Description: The time current time format is not highlighted
// Fix: Instead of showing the submenu directly we call this function 
// where we set the menu and also higlight the previously selected menu item
void settingsFocusTimeFormatCurSel(void)
{
	UBYTE			CurSel;
	T_MFW_HND 		setting_win;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	
	setting_win= bookMenuStart(win, TimeFormatAttributes(), 0);
	CurSel = FFS_flashData.time_format;
	SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &CurSel);
}

/*******************************************************************************
 $Function:    	settingsFocusCurSelAnyKeyAns
 $Description:	This function sets the focus to the On or Off menu item of the 
 			'AnyKey Answer' menu in the Phone Settings main menu.
 			Check whether the 'Any Key Answer' is enabled by reading the data
 			stored in the FFS. Depending on that, set the focus to the menu 
 			item
 $Returns:	void	
 $Arguments:	void
 *******************************************************************************/
//	Jul 28, 2004 REF: CRR 20899 xkundadu
void settingsFocusCurSelAnyKeyAns(void)
{
	// Points to the index of the menuitem to be selected
	// If it is 1, 'Off' menuItem will be selected, else 'On' menuitem
	// will be selected.
	UBYTE			menuItemIndex = 1;
	
	T_MFW_HND 		setting_win;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	    
	TRACE_FUNCTION ("settingsFocusCurSelAnyKeyAns() ");

	// Starting the menu corresponding to AnyKeyanswer.
	setting_win = bookMenuStart(win, GetAnyKeyAnswerMenu(), 0);

	//Checking whether the 'Any key answer is enabled or not
	//Reading the value from FFS.
	if( (FFS_flashData.settings_status & SettingsAnyKeyAnswerOn)
		EQ SettingsAnyKeyAnswerOn)
	{
		//Set the focus to the 'On' menu item.
		menuItemIndex =  0;
	}
	
	// Send the event to display the menu items and set focus to appropriate  
	// menuitem.
	SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &menuItemIndex);	
	
}

// May 04, 2005 REF: CRR 30285 x0021334
/*******************************************************************************
 $Function:    	settingsSmsStatusReportCurSel
 $Description:	This function sets the focus to the On or Off menu item of the 
 			'Status Report' submenu menu in the Messages Settings menu.
 $Returns:	void	
 $Arguments:	void
 *******************************************************************************/
void settingsSmsStatusReportCurSel(void)
{
	T_MFW_HND 		setting_win;
	T_MFW_SMS_INFO sms_info;
	T_MFW_HND 		win = mfw_parent(mfw_header());
	
	setting_win = bookMenuStart(win, SmsStatusReportAttributes(), 0);
	if (sms_parameter(&sms_info, TRUE) == MFW_SMS_OK)
	{
		// Send the event to set focus to appropriate option.
		SEND_EVENT(setting_win, DEFAULT_OPTION, NULL, &sms_info.srr);
	}
	else
	{
		TRACE_FUNCTION("sms info could not be retrieved ");
	}


}

