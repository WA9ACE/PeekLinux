/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Call
 $File:		    MmiIdle.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
                        
********************************************************************************

 $History: MmiIdle.h
       Mar 30, 2007 OMAPS00123019 a0393213(R.Prabakar0
       Description : Reboot of the target when you unplug/plug USB cable using camera application
       
 	Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
 	Description : CT-GCF[27.22.4.22.1]-Incorrect string display
 	Solution     : The DCS attribute is set dynamically

	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option
 	
	xrashmic 9 Sep 2006, OMAPS00092732
	USBMS ER

       July 12, 2006    REF:DR OMAPS00081477   x0047075
	Description:   SMS (with SMS Report Indicator +CDS Enabled) Reset Issue.
	Solution: Added a window handler for the sms status editor.
	              If sms status editor does not exist, create it.If it already exists Update the same.
 
    Jun 06, 2006 DR: OMAPS00080543 - xreddymn
    Changes to display long SAT messages correctly on idle screen.

    May 15, 2006 DR: OMAPS00075852 - xreddymn
    Do not display messages on Idle screen when inside the dialer.

 	Mar 14, 2006 DR: OMAPS00061468 - x0035544.
   	Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
   	Solution : Added additional parameter to the prototype of  addSatMessage() to hold the display type
   	for  SAT idle text with icon support.

 	Sept 15, 2005    REF: CRR 32410  x0021334
	Description: The keypad unlocks even though the unlocking sequence is not executed in quick succession.
 	Fix:	A new timer expiry event is added and handled.

        xrashmic 26 Aug, 2004 MMI-SPR-23931
        
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#else
#include "mfw_edt.h"
#endif
#include "Mmiicons.h" //for T_BITMAP

#ifndef _DEF_MMI_IDLE_H_
#define _DEF_MMI_IDLE_H_

#define EDITOR_SIZE 41					/* SPR761 - SH - Moved from MmiIdle.c, changed to 41 */

/* xreddymn OMAPS00080543 Jun-06-2006
 * Maximum length allowed for SAT messages on Idle Screen.
 */
#define MAX_SATMSG_LEN                  296

typedef enum
{										/* REASONS                  */
 IdleNone = IdleDummy,					/* no reason                */
 IdleNormal,							/* normal idle mode         */
 IdleUpdate,
 IdleSearchNetwork,
 IdleNewSmsMessage,
 IdleActiveKeyPadLocked,
 IdleNewCBImmediateMessage,
 IdleNewCBNormalMessage
} E_IDLE_TYPE; /*a0393213 warnings removal-typedef name given*/

//	Sept 15, 2005    REF: CRR 32410  x0021334
//	Description: The keypad unlocks even though the unlocking sequence is not executed in quick succession.
// 	Added a new enum member IDLE_KEYPAD_UNLOCK_TIMER_EXPIRED
typedef enum
{										/* REASONS                  */
	IDLE_NO_NETWORK,
	IDLE_UPDATE,
	IDLE_NEW_SMS,
	IDLE_ACTIVE_KEY_PAD_LOCKED,
	IDLE_DETAILS_NUMBER,
	IDLE_DIALLING_INIT,
	IDLE_INFO_ENDED,
	IDLE_KEYPAD_LOCKED_UPDATE,
	IDLE_START_DIALLING_NUMBER,
	IDLE_KEYPAD_UNLOCK_TIMER_EXPIRED 
} E_IDLE_EVENTS; /*a0393213 warnings removal-typedef name given*/

typedef enum
{
    SHOWVERSION_INIT
} Version_events;


#define SHOW_CPRS_DEACTIV   	0
#define SHOW_CPRS_ACITV			1		
#define DONT_SHOW_CPRS			2 //not allowed to show any indication

/*
*  The information related to every window must be encapsulated in such an structure
*/
typedef struct
{
    T_MMI_CONTROL   mmi_control;		// this a common parameter in every structure
	T_MFW_HND win;						// windows handler, another common parameter
	T_MFW_HND kbd;						// To control all of the events, 2 handler are needed kbd that handles the key press
    T_MFW_HND kbd_long;					// event and kbd_long that handles the release and long key event.
#ifdef NEW_EDITOR						/* SPR#1428 - SH - New Editor changes */
	T_ED_DATA *editor;
	T_ED_DATA *editor_sat;
	T_MFW_HND sms_status_editor;    //July 12, 2006    REF:DR OMAPS00081477  x0047075 Fix:Handler for the sms status editor
#else /* NEW_EDITOR */
    T_MFW_HND edt;
    T_MFW_HND edt_sat;					/* SPR759 - SH*/
    T_MFW_HND sms_status_edt;          //July 12,2006 REF:DR OMAPS00081477 x0047075 Fix:Handler for the sms status editor
#endif /* NEW_EDITOR */
	T_MFW_HND info_win;
	T_MFW_HND klck_win;				// Keypad Locked Window Handler
	int missedCalls;
	int ems; //xrashmic 26 Aug, 2004 MMI-SPR-23931
	UBYTE search;
	UBYTE flagIdle;
    USHORT nm_status;
    	char edt_buf[EDITOR_SIZE];			/* SPR761 - SH - Changed from a number to a constant */
#ifdef NEW_EDITOR						/* SPR#1428 - SH - New Editor changes */
	T_ED_ATTR editor_attr;				/* Dialling editor */
	T_ED_ATTR editor_attr2;				/* CB editor */
	T_ED_ATTR editor_attr3;				/* CLASS 0 SMS editor */
	T_ED_ATTR editor_attr_sat;			/* SAT & Homezone editor */
#else /* NEW_EDITOR */
	MfwEdtAttr edt_attr;				//this one for the dialling editor
	MfwEdtAttr edt_attr2;				//this is for the CB editor
	MfwEdtAttr edt_attr3;				//this is for the (only CLASS 0) SMS editor	
	MfwEdtAttr edt_attr_sat;			/* SPR759 - SH*/
#endif /* NEW_EDITOR */
	char imei[16];
    /* xreddymn OMAPS00075852 May-15-2006
     * This will be set to TRUE when dialer is being displayed.
     */
    UBYTE dialer_visible;
    UBYTE new_sms;
    UBYTE new_cbch;
	CHAR incoming_cb_msg[MAX_CBMSG_LEN]; 
	/*NM, p019*/
	CHAR incoming_sms_msg[MAX_MSG_LEN_SGL]; 	
    UBYTE right_key_pressed;
    UBYTE silent_mode;
    UBYTE clean_screen;
    UBYTE ciphering;
    UBYTE starting_up;                  /*SPR#1662 - NH - New field for checking the start up status */
    /* xreddymn OMAPS00080543 Jun-06-2006 */
    CHAR incoming_sat_msg[MAX_SATMSG_LEN];		/* SPR759 - SH*/
    T_BITMAP* idleNetLogo;				//Background for idle screen - NULL if no background.
	T_BITMAP * idleBgd;				//Background image on the idle Screen    
    int defTextOffset;				//start point for text (may vary with bitmap ?)
} T_idle;

extern UBYTE KeyPadLock;
extern T_idle idle_data;/*Oct 13 2006, OMAPS00095266, a0393213(R.Prabakar)*/
#ifdef FF_MMI_USBMS
extern T_MFW_HND usb_opt_win; /*Mar 30, 2007 OMAPS00123019 a0393213(R.Prabakar)*/
#endif
                                        /* PROTOTYPES               */
extern void wake_up_rr(void);

void idleInit (MfwHnd parent);
void idleExit (void);
void idleExec (int reason, MmiState next);
void idleEvent(int reason);
void idleDialBuffer(char* dialBuffer);

MfwHnd idle_get_window(void);

int idleIsFocussed(void);

void idle_set_starting_up(UBYTE set);

void windowsUpdate(void);

void addMissedCall(void);
void addNewEMS(void);//xrashmic 26 Aug, 2004 MMI-SPR-23931
void addCBCH(char* cbch_string, UBYTE type);
void statusCBCH(UBYTE status);

// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
//x0035544 Mar 14, 2006 DR:OMAPS00061468
/*
**Sending additional parameter display_type to hold the display type for SAT Idle text with icon display
*/
#ifdef FF_MMI_SAT_ICON
void addSatMessage(char *satMessage, UBYTE width, UBYTE height,
						char *dst, UBYTE selfExplanatory, T_DISPLAY_TYPE display_type);
#else
void addSatMessage(char *satMessage);		/* SPR759 - SH*/
#endif
extern void showIMEI (T_MFW_HND win, char* IMEI);
void idle_setBgdBitmap(int bmp);

/* Added to remove warning Aug - 11 */
EXTERN U32 dspl_GetBorderColour ( void );
EXTERN U32 dspl_GetFgdColour ( void );
EXTERN U32 dspl_SetBorderColour (U32 inColour);
EXTERN int dspl_GetContrastColour( int ipCol);
EXTERN UBYTE sms_check_message_pending(void);
EXTERN int M_callVoice(MfwMnu* m, MfwMnuItem* i);
/* End - Remove warning Aug - 11 */

#endif

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
/* Callback function for powermanagement timer. */
int powerManagementEvent( MfwEvt e, MfwTim *tc );

/*function for updating PM timer.*/
void mmi_update_pm_timer_duration(void);

/*function to enable/disable Power Management .*/
void mmi_pm_enable( int enable_pm);
#endif
#endif
//xashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
void mmi_usb_mode_connect(void);
void mmi_usb_mode_disconnect(void);
GLOBAL int mmi_usb_enumeration (MfwMnu* m, MfwMnuItem* i);
GLOBAL int setUSBMSPS (MfwMnu* m, MfwMnuItem* i);//xashmic 27 Sep 2006, OMAPS00096389 
GLOBAL int setUSBMSPopup (MfwMnu* m, MfwMnuItem* i);//xashmic 27 Sep 2006, OMAPS00096389 
UBYTE getUSBMSFlags(void);//xashmic 27 Sep 2006, OMAPS00096389 

#endif
