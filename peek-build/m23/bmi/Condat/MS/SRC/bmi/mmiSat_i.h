
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		   	MmiSat_I.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    MMI SIM toolkit internal/private interface

                        
********************************************************************************

 $History: MmiSat_I.h

 	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event

 	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: created and defined the new structure called T_SAT_IconInfo at the structure 
	T_SAT_call_setup_parameter to support SAT icon data.
	

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

#ifndef _DEF_MMI_SAT_I_H_
#define _DEF_MMI_SAT_I_H_

#include "mfw_mfw.h"
#include "mfw_sat.h"
#include "MmiDummy.h"


#define THREE_SECS  0x0BB8             /* 3000 milliseconds.                  */
#define FIVE_SECS   0x1388             /* 5000 milliseconds.                  */
#define TEN_SECS    0x2710             /* 10000 milliseconds.                 */
#define TWENTY_SECS 0x4E20             /* 20000 milliseconds.                 */
#define TWO_MIN     0x1D4C0            /* 2 min                               */
#define FOREVER     0xFFFF             /* Infinite time period.               */
//x0035544 Feb 06, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
typedef struct
{							/* SAT ICON DATA   */
	U8 width;				/* SAT Icon width  */
	U8 height;				/* SAT Icon height */
	char* dst;                           /* Icon data	   */
	BOOL selfExplanatory;
} T_SAT_IconInfo;
#endif

typedef struct
{
    S16 callId;                         /* id of new call           */
    S32 redialTime;                     /* maximal redial time      */
    char * TextString;                  /* name of called party     */
    char * TextString2;                 /* alpha id 2 - use on "calling" screen */
 //x0035544 Feb 06, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON	
    T_SAT_IconInfo IconInfo;       /*Icon data  */
    T_SAT_IconInfo IconInfo2;
#endif

} T_SAT_call_setup_parameter;

/*
 * SAT Command qualifier
 */
#define SAT_F_CALL_IF_NOT_BUSY        (U8)0x00
#define SAT_F_CALL_IF_NOT_BUSY_REDIAL (U8)0x01
#define SAT_F_CALL_PUT_ON_HOLD        (U8)0x02
#define SAT_F_CALL_PUT_ON_HOLD_REDIAL (U8)0x03
#define SAT_F_CALL_DISCONNECT         (U8)0x04
#define SAT_F_CALL_DISCONNECT_REDIAL  (U8)0x05

#define SAT_M_SMS_PACKING             (U8)0x01
  
#define SAT_M_TEXT_HIGH_PRIO          (U8)0x01
#define SAT_M_TEXT_WAIT               (U8)0x80

#define SAT_M_INKEY_ALPHA             (U8)0x01
#define SAT_M_INKEY_UCS2              (U8)0x02
#define SAT_M_INKEY_YES_NO            (U8)0x04
#define SAT_M_INKEY_HELP_AVAIL        (U8)0x80

#define SAT_M_INPUT_ALPHA             (U8)0x01
#define SAT_M_INPUT_UCS2              (U8)0x02
#define SAT_M_INPUT_NOECHO            (U8)0x04
#define SAT_M_INPUT_PACKING           (U8)0x08
#define SAT_M_INPUT_HELP_AVAIL        (U8)0x80

#define SAT_M_SELECT_TYPE_ENA         (U8)0x01
#define SAT_M_SELECT_PRESENT_NAV      (U8)0x02
#define SAT_M_SELECT_HELP_AVAIL       (U8)0x80

#define SAT_M_SETUP_HELP_AVAIL        (U8)0x80

/*
 * SAT Event definitions (NOTE: corresponding MfwSat* is a flag bitfield)
 */
typedef enum SAT_EVENTS {
        SAT_UNKNOWN = 100,    /* event not handled by MMI */
        SAT_DISPLAY_TEXT,     /* display string, destroy after timeout  */
        SAT_DISPLAY_TEXT_WAIT,/* display string, wait for user response */
        SAT_GET_KEY,          /* get user keystroke       */
        SAT_GET_STRING,       /* get user input           */
        SAT_PLAY_TONE,        /* play audio tone          */
        SAT_SETUP_MENU,       /* setup toolkit menu       */
        SAT_SELECT_ITEM,      /* select menu item         */
        SAT_SEND_SMS,         /* send short message       */
        SAT_SEND_SS,          /* send service command     */
        SAT_SETUP_CALL,       /* setup a call             */
        SAT_CALL_RESULT,      /* call control result      */
        SAT_CALL_ALERT,       /* call control alerting    */
        SAT_SESSION_END,      /* session terminated       */
        SAT_RETURN,           /* SETUP_MENU shall recreate */
        SAT_EXIT,             /* SETUP_MENU shall selfdestroy */
        SAT_CALL_REDIAL,      /* attempt a redial         */
        SAT_REFRESH,           /* refresh SIM fields     */        
        SAT_CALL_END,          /* clean up after call end  */
        SAT_DESTROY_WINDOW,	/* sbh - destroy the current window */
        SAT_IDLE_TEXT,			/* sbh - display text on idle screen */
        SAT_SET_EVENTS,			/* sbh - request when events will happen */
        SAT_SEND_DTMF,          /* send service command     */

        /* SPR#1786 - DS - SAT Class E Events */
        SAT_OPEN_CHANNEL,
        SAT_CLOSE_CHANNEL,
        SAT_SEND_DATA,
        SAT_RECEIVE_DATA
#ifdef FF_MMI_R99_SAT_LANG
        ,SAT_LANGUAGE_NOTIFY, /*OMAPS00070661(SAT-Lang Notifiy) a0393213(R.Prabakar)*/
        SAT_LOCAL_INFO            /*OMAPS00070661(SAT-Local Info)   a0393213(R.Prabakar)*/
#endif
#ifdef FF_WAP
        ,SAT_LAUNCH_BROWSER  /*Class C Launch Browser event */
#endif
} T_SAT_EVENTS;

/*
* SAT defintions for returning results with sat_done()
*/
#define SAT_ERR_INDEX 0
#define SAT_AI_INDEX  1
typedef U8 T_SAT_RES[2];

/*
* SAT internal prototypes
*/
extern T_MFW_HND sat_display_text_create (T_MFW_HND parent_window);
extern T_MFW_HND sat_play_tone_create (T_MFW_HND parent_window);
extern T_MFW_HND sat_get_string_create (T_MFW_HND parent_window);
extern T_MFW_HND sat_get_key_create (T_MFW_HND parent_window);
extern T_MFW_HND sat_setup_menu_start(T_SAT_CMD * sat_command);
extern T_MFW_HND sat_select_item_create (T_MFW_HND parent);
extern T_MFW_HND sat_call_setup_create(T_MFW_HND parent_window);
extern int sat_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
extern int sat_kbd_cb (T_MFW_EVENT event, T_MFW_KBD * key);
extern void sat_set_setup_menu_win(T_MFW_HND win);
extern void sat_set_call_setup_win(T_MFW_HND win);

extern void sat_set_setup_menu_listmnu_win(T_MFW_HND win);
extern T_MFW_HND sat_get_setup_menu_listmnu_win(void);

extern BOOL g_SATsession_is_active;

/*
* SAT string conversion routines
*/
extern char * sat_create_TEXT_ASCIIZ (T_SAT_TXT * txt);
extern char * sat_create_ITEM_ASCIIZ (SatItem * item);
extern void sat_destroy_TEXT_ASCIIZ (char * str);
#define sat_destroy_ITEM_ASCIIZ sat_destroy_TEXT_ASCIIZ

/*MC SPR 940/2uncommented unicode function, and changed return type*/
extern char* sat_create_TEXT_UCODE (T_SAT_TXT * txt);
/*MC end*/
/*MC, SPR 940/2 uncommented unicode function and changed parameter type*/
extern void sat_destroy_TEXT_UCODE (char* str);
/*MC end */
#ifdef INTEGRATION_SEPT00
#else /* INTEGRATION_SEPT00 */

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned short wchar_t; // wide unicaode character type
#endif /* _WCHAR_T */
#include "mfw_ss.h" /* T_MFW_DCS */

typedef ListMenuData ListMenuData_t;

#define E_EDIT_READ_ONLY_MODE  READ_ONLY_MODE
#define E_EDIT_DIGITS_MODE     DIGITS_MODE
#define E_EDIT_ALPHA_MODE      ALPHA_MODE
#endif /* INTEGRATION_SEPT00 */

extern void sat_gsm_to_ascii (char * ascii_string, char * gsm_string, U16 length);
extern void sat_ascii_to_gsm (char * gsm_string, char * ascii_string, U16 length);
extern void sat_ascii_to_ucode (wchar_t * UCS2_chars, char * ascii_string, U16 length);
extern void sat_ucode_to_ascii (char * ascii_string, wchar_t * UCS2_chars, U16 length);
extern void sat_TEXT_to_ASCIIZ (char * destination, T_SAT_TXT * txt);

#endif  /* _DEF_MMI_SAT_I_H_   */
