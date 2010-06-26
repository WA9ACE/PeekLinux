/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiSimToolkit.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       22/02/01

********************************************************************************

 Description:



********************************************************************************

 $History: MmiSimToolkit.c
 
      Apr 18, 2006 Ref:OMAPS00116309 R.Prabakar (a0393213)
      CT-GCF[27.22.4.7.1]- MMI crashes on REFERSH seq 1.2
      Solution : Data type mismatch between ACI and MMI sorted out


    Jan 19, 2007    REF:DR OMAPS00110946    x0043642
    Description:   CT GCF / PTCRB - TC27.22.4.12.2 SEND USSD (Icon support) - Unexpected TERMINAL RESPONSE performed
    Solution:        Send unknown command terminal response for invalid ss length

    16 Jan 2006, Ref OMAPS00110997, X0043642
    Description: CT PTCRB - TC27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) - Unexpected TERMINAL RESPONSE performed
    Solution: Set satResult as unknownData icon is not present.

 
	Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
	Description:   CT GCF - TC27.22.4.13.1 Seq 1.10 - PROACTIVE SIM COMMANDS: 
	SET UP CALL (NORMAL) - Call Set-up Failed
	Solution:  Removed the Redial flag enabling code.

 	Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
 	Description : CT-GCF[27.22.4.22.1]-Incorrect string display
 	Solution     : The DCS attribute is set dynamically

    06 Oct 2006, Ref OMAPS00095263, Archana(x0047075)
    Description : CT-GCF[27.22.4.24.1]-MS  Resets
    Solution     : Editor dialog is destroyed on destroy event instead of timeout.
    			   
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
	
    10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
    Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
    Solution     : UCS2 display added to terminal profile. Made some changes for long idle screen text display and
    			   UCS2 display for SAT icon
  	 
    Jun 06, 2006 DR: OMAPS00080543 - xreddymn
    Changes to display long SAT messages correctly on idle screen.

 	29 May 2006 REF: OMAPS00071366, x0045876
 	Description: IT3 verdict: FAIL, [Step 13] The ME did not display idle screen without the message 
 	"Idle Mode Text" as expected.

      xrashmic 16 Feb, 2006 OMAPS00064413
      When a SIM RESET occurs, while recreating the simtoolkit list view, the list win is also recreated.  

       May 26, 2006 REF: OMAPS00076094, x0045876
       Description: MMI's Terminal Profile (TP) setup is not in line with the SAT class e implementation
       Solution: TP is updated based on the current support by MMI.

       May 24, 2006 REF:  In SET UP CALL RESULT, Editor is not initialized, x0045876
       Solution: Initialized Editor

	May 09, 2006 REF: OMAPS00064076, x0045876
       Description: SAT IDLE TEXT - UCS2 Display
       Solution: Added UCS2 support to display UCS2 characters in IDLE MODE.
 
       xrashmic 16 Feb, 2006 OMAPS00064413
       When a SIM RESET occurs, while recreating the simtoolkit list view, the list win is also recreated.  

 	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: In the function sim_toolkit_exec() for the event SAT_SETUP_CALL copied the icon data which is 
	got through the sat command on to the structures data->call_setup_parameter.iconInfo and to the data->call_setup_parameter.iconInfo2
	appropriately.And  For the event SAT_CALL_END freed the memory allocated for the icon data.
	on to the 
	
  	Feb 02, 2006 DR: OMAPS00061468 - x0035544.
   	Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
   	Solution : SAT SET UP IDLE MODE TEXT (Icon) support added.

   Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
   Description: SAT Icon support
   Solution : SAT icon support added.

    Dec 23, 2005    REF: OMAPS00062166     x0039928
    Bug: CT-TIB-GCF[27.22.4.24.1]-sequence 1.2 crashes
    Fix: Initialize the editor buffer. Initialization is taken care for DTMF and also for SS.

	
    xreddymn Sep-06-2005 MMI-SPR-33876:
    Initialize editor buffer when displaying message box
    in case of SAT_SEND_SMS

	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

  25/10/00      Original Condat(UK) BMI version.
21/02/03		removed all calls to function sat_add_unicode_tag_if_needed() and the
				function itself
    26-05-04      CRR 19656    Sandip and Deepa M.D
      	Fix for The Dsample doesn't display lines ( <CR> or <LF> ) in the SETUP IDLE 
	TEXT command.
	Fix for The idle screen is empty when a SETUP IDLE TEXT command is sent with 
	a long text ( 239 characters ).
    
    
 $End

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

#include "gdi.h"
#include "audio.h"

#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_aci.h"
#include "cnf_aci.h"
#include "mon_aci.h"

#include "tok.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mfw_mfw.h"
#include "mfw_kbd.h"
#include "mfw_tim.h"
#include "mfw_lng.h"
#include "mfw_win.h"
#include "mfw_icn.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h" // x0021334 : To test SAT session end - CQ33597
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sms.h"

#include "dspl.h"


#include "p_mmi.h"
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "mfw_mme.h"

#include "MmiMmi.h"

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
#include "MmiTimeDate.h"
#include "mmiSat_i.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiMenu.h" // SH - 20/11/01 - included to provide symbol SimMenuFunc
#include "mmiSmsRead.h" /*MC SPR 940/2*/

#include "cus_aci.h"
#include "prim.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "mmiSatClassE.h"

#include "mmiColours.h" /* SPR#2321 - DS */
#include "MmiDialogs.h" /* SPR#2321 - DS */


#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"
#endif

#ifdef FF_WAP
#include "AUIWapext.h"
#endif

#if defined(FF_WAP)
#define MMI_TEST_SAT_LAUNCH_BROWSER
#endif

#ifdef MMI_TEST_SAT_LAUNCH_BROWSER
void mmiOpenDummyBrowser(T_MFW_HND win,char* information);
#define INFORMATION_SIZE 100
char information[INFORMATION_SIZE];
#endif
/*
 * Local definitions
 */
typedef struct
{
    T_MMI_CONTROL mmi_control;
    T_MFW_HND     win;
    T_MFW_HND     sat_handle;
    T_SAT_CMD   * sat_command; /* mfw storage of actual command */
    T_SAT_CMD   * sat_setup_menu_command; /* dynamic storage of setup menu */
    T_MFW_HND     sat_setup_menu_win; /* c030 rsa window associated with the setup menu command */
    T_MFW_HND     sat_setup_menu_listmnu_win; /* NDH : Window associated with the SAT ListMenu */
    T_MFW_HND     info_win;           /* window used for simple info screens */
    T_MFW_HND     sat_call_setup_win; /* window associated with the call setup command */
    T_SAT_call_setup_parameter * call_setup_parameter; /* parameter psace of call setup to be passed to call() */
    T_SAT_EVENTS  active_sat_session; /* event which caused the currently active session */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
    T_EDITOR_DATA editor_data;
#endif
/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/
    UBYTE     sat_available_on_SIM;
} T_sim_toolkit;

static T_sim_toolkit * g_sim_toolkit_data;

/*MC SPR 1257, merged in from b-sample version of this file*/
char *			sat_mainmenu_label = NULL;
UBYTE 			header_len;
/*MC end*/

BOOL sat_call_active = FALSE;   // Marcus: Issue 1812: 13/03/2003

/* PROTOTYPES */
static T_MFW_HND sim_toolkit_create     (T_MFW_HND parent_window);
static void      sim_toolkit_destroy    (T_MFW_HND own_window);
//sandip 16292 . added this new function
unsigned char DecodeGSM7ToASCII(U8 * encoded, U8 * plain, U8 Length);
/*
 * Marcus: Issue 1057: 21/01/2003: Removed "static" from the following two
 * function prototypes for their use in MmiCall.c
 */
T_MFW_HND sat_get_setup_menu_win (void);
T_MFW_HND sat_get_call_setup_win (void);

static void      sat_setup_menu_proc    (T_SAT_CMD * sat_command);
static void      sat_info_cb            (T_MFW_HND win, UBYTE identifier, UBYTE reason);

extern UBYTE  call_SATCall; // Marcus: Issue 1057: 21/01/2003


/*******************************************************************************

 $Function:     sim_toolkit_check

 $Description:   If there is no support for SAT on SIM card
         the item SIM TOOLKIT will not appear in the menulist

 $Returns:

 $Arguments:

*******************************************************************************/
USHORT sim_toolkit_check (struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi)
{

  T_sim_toolkit * data = g_sim_toolkit_data;

  TRACE_FUNCTION("sim_toolkit_check");

  if (data->sat_available_on_SIM EQ TRUE)
  {
    //show the SAT item in the menulist
    return 0;
  }
  else
  {
    return 1;
  }

}




/*******************************************************************************

 $Function:  simToolkitInit

 $Description:   This is the start-up time initialisation routine.
         For compatibility reasons the functions is still there.


 $Returns:    none.

 $Arguments:

*******************************************************************************/
void simToolkitInit (T_MFW_HND parent_window)
{
  TRACE_FUNCTION("simToolkitInit");

    g_sim_toolkit_data = (T_sim_toolkit *)ALLOC_MEMORY (sizeof (T_sim_toolkit));

    winAutoFocus (TRUE);
    sat_set_setup_menu_win(NULL);
    sat_set_call_setup_win(NULL);
    sim_toolkit_create (parent_window);
}
/*******************************************************************************

 $Function:  simToolkitExit

 $Description:  This is the shutdown time clean-up routine.

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void simToolkitExit(void)
{
  TRACE_FUNCTION("simTookitExit");
    sim_toolkit_destroy (g_sim_toolkit_data->win);
//    FREE_MEMORY ((U8 *)g_sim_toolkit_data, sizeof (T_sim_toolkit));
	if (sat_mainmenu_label != NULL)
	{
		sat_destroy_TEXT_ASCIIZ (sat_mainmenu_label);
		sat_mainmenu_label = NULL;
	}
}


/********************************************************************
 *
 * SIM Toolkit Dialog
 *
 * Window - Structure
 *
 * sim_toolkit   ---->   sat_play_tone
 *               ---->   sat_display_text
 *               ---->   sat_get_key
 *               ---->   sat_setup_call
 *               ---->   sat_get_string
 *               ---->   sat_setup_menu
 *               ---->   sat_select_item
 *               ---->   sat_send_sms
 *               ---->   sat_send_ss
 *
 ********************************************************************/
static const UBYTE sat_terminal_profile [13] =
{
#ifdef SAT_TP1_PRF_DNL

    (SAT_TP1_PRF_DNL | SAT_TP1_MENU_SEL),
    
     /* 26-05-2006, x0045876 (OMAPS00076094 - Removed SAT_TP2_UCS2_ENTRY, SAT_TP2_UCS2_DSPL,
        SAT_TP2_DSPL_EXT) */
    /*10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
       Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
       Solution     : UCS2 display added to terminal profile*/
    (SAT_TP2_CMD_RES|SAT_TP2_CC|SAT_TP2_ALPHA_ID|
     SAT_TP2_UCS2_DSPL), 

    (SAT_TP3_DSPL_TXT|SAT_TP3_GET_INKEY|SAT_TP3_GET_INPUT|
     SAT_TP3_PLAY_TONE|SAT_TP3_REFRESH),

    (SAT_TP4_SEL_ITEM|SAT_TP4_SEND_SMS|SAT_TP4_SEND_SS|
     SAT_TP4_SEND_USSD|SAT_TP4_SETUP_CALL|SAT_TP4_SETUP_MENU),

    (SAT_TP5_EVENT_LIST | SAT_TP5_USER_ACT | SAT_TP5_SCR_AVAIL),

#ifdef FF_MMI_R99_SAT_LANG
    (SAT_TP6_LANG_SEL),  /*OMAPS00070661 R.Prabakar(a0393213) Language Selection Enabled */
#else
    0x0,
#endif    

    0x0,
    
    (SAT_TP8_BIN_GET_INKEY | SAT_TP8_IDLE_TXT | SAT_TP8_AI2_SETUP_CALL),

#if defined(FF_WAP)
#ifdef FF_MMI_R99_SAT_LANG
    /*OMAPS00070661 R.Prabakar(a0393213) Language Notification and Local Information(language setting)*/
    (SAT_TP9_SUST_DSPL_TXT|SAT_TP9_LAUNCH_BROWSER | SAT_TP9_PLI_LANG | SAT_TP9_LANG_NOTIFY),
#else
    (SAT_TP9_SUST_DSPL_TXT|SAT_TP9_LAUNCH_BROWSER), 
#endif
#else
#ifdef FF_MMI_R99_SAT_LANG
    /*OMAPS00070661 R.Prabakar(a0393213) Language Notification and Local Information(language setting)*/
    (SAT_TP9_SUST_DSPL_TXT | SAT_TP9_PLI_LANG | SAT_TP9_LANG_NOTIFY),
#else
    SAT_TP9_SUST_DSPL_TXT, 
#endif
#endif

    /* 26-05-2006, x0045876 (OMAPS00076094 - Added Softkeys) */
    0x03, 

    0x0,

    (SAT_TP12_OPEN_CHANNEL | SAT_TP12_CLOSE_CHANNEL | SAT_TP12_RECEIVE_DATA |
    SAT_TP12_SEND_DATA),

    (SAT_TP13_CSD_SUPP_BY_ME | SAT_TP13_GPRS_SUPP_BY_ME)

#else
/* START: 11-05-2006, x0045876 (OMAPS00076094) */
    /* 0x09, 0x73, 0x17, 0x37,0x00 // c032 rsa */
#ifdef FF_MMI_R99_SAT_LANG
/*OMAPS00070661 R.Prabakar(a0393213) Language Notification, Local Information(language setting) and Language Selection Enabled*/
    0x09, 0x7B, 0x97, 0x3F, 0x61, 0x01, 0x00, 0x58, 0x29, 0x03, 0x00 /*, 0x0F, 0x03*/
#else
    0x09, 0x7B, 0x97, 0x3F, 0x61, 0x00, 0x00, 0x58, 0x01, 0x03, 0x00 /*, 0x0F, 0x03*/
#endif
 /* END: 11-05-2006, x0045876 (OMAPS00076094) */  
#endif
};

static void sim_toolkit_destroy (T_MFW_HND own_window);
static void sim_toolkit_exec    (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command);
static int  sim_toolkit_sat_cb  (T_MFW_EVENT event, T_MFW_SAT * sat_bits);
/*******************************************************************************

 $Function:  sim_toolkit_create

 $Description:   Creation of an instance for the SIM Toolkit dialog.
             Type of dialog : SINGLE_STATIC
              Top Window must be available at any time, only one instance.


 $Returns:    none.

 $Arguments:

*******************************************************************************/
static T_MFW_HND sim_toolkit_create (T_MFW_HND parent_window)
{
    T_sim_toolkit * data = g_sim_toolkit_data;
    T_MFW_WIN     * win;

    TRACE_FUNCTION("sim_toolkit_create");

    data->win = win_create (parent_window, 0, 0, NULL); // c013 rsa

    if (data->win EQ NULL)
        return NULL;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)sim_toolkit_exec;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (MfwUserDataPtr)data;

    data->sat_command = (T_SAT_CMD *)ALLOC_MEMORY (sizeof (T_SAT_CMD)); // c022 rsa

    /*
     * Create any other handler
     */
    data->sat_handle = sat_create (
        data->win,                    // parent window
        /* possible SAT events */
        MfwSatSessionEnd |     /* end of session           */
        MfwSatRefresh |     /* refresh SIM fields           */
		MfwSatDataRefreshed | /*data Refreshed */
		MfwSatTextOut|         /* display string           */
        MfwSatGetKey|          /* get user keystroke       */
        MfwSatGetString|       /* get user input           */
        MfwSatPlayTone|        /* play audio tone          */
        MfwSatSetupMenu|       /* setup toolkit menu       */
        MfwSatSelectItem|      /* select menu item         */
        MfwSatSendSMS|         /* send short message       */
        MfwSatSendSS|          /* send service command     */
        MfwSatSendUSSD|          /* send USSD command     */
        MfwSatSendDTMF|          /* send dtmf command     */
        MfwSatCall|            /* setup a call             */
		MfwSatSetEvents|      /* setup event lists 		*/
		MfwSatIdleText|      /* set idle text 		*/
		MfwSatCcRes|           /* call control result      */
        MfwSatCcAlert           /* call control alerting    */
#if defined(FF_WAP)
        |MfwSatLaunchBrowser
#else
#endif
        |MfwSatOpenChannel|
		 MfwSatCloseChannel|
		 MfwSatSendData|
		 MfwSatReceiveData|
		 MfwSatErrBusy            /* SAT error handling */  
		 #ifdef FF_MMI_R99_SAT_LANG
		 |MfwSatLangNotify  |         /*OMAPS00070661 (SAT-lang notify and local info(lang setting)) a0393213(R.Prabakar)*/
		 MfwSatLocalInfo
		 #endif
		 ,
        (T_MFW_CB)sim_toolkit_sat_cb,  // event callback
        data->sat_command);            // control block for MFW

    /*
     * initialize global variables
     */
    data->sat_setup_menu_command = NULL;
    data->sat_command = NULL;
    data->info_win = NULL;
    data->call_setup_parameter = NULL;
    data->active_sat_session = SAT_SESSION_END;
    data->sat_available_on_SIM = FALSE;
 /* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/
    sat_mainmenu_label = NULL;

    /*
     * Initialise MFW
     */
    sat_init ((UBYTE *)sat_terminal_profile, (UBYTE)sizeof (sat_terminal_profile));

    /*
     * return window handle
     */
    win_show(data->win);
    return data->win;
}
/*******************************************************************************

 $Function:  sim_toolkit_destroy

 $Description:  Destroy the sim toolkit dialog.

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static void sim_toolkit_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN     * win;
    T_sim_toolkit * data;

    if (own_window)
        {
            win  = ((T_MFW_HDR *)own_window)->data;
            data = (T_sim_toolkit *)win->user;

            if (data)
                {
                    /*
                     * Exit SAT and Delete SAT Handler
                     */
                    sat_exit ();
                    sat_delete (data->sat_handle);

                    /*
                     * Delete WIN Handler
                     */
                    win_delete (data->win);
                    /*
                     * Free Memory
                     */
                    if (data->sat_setup_menu_command NEQ NULL)
                    {
                        FREE_MEMORY ((U8 *)data->sat_setup_menu_command, sizeof (T_SAT_CMD));
                        data->sat_setup_menu_command = NULL;
                    }
                    if (data->sat_command != NULL)
                    {
                    	FREE_MEMORY ((U8 *)data->sat_command, sizeof (T_SAT_CMD)); // c022 rsa
                    	data->sat_command = NULL;
                    }
                    FREE_MEMORY ((U8 *)data, sizeof (T_sim_toolkit));
                }
        }
}

/*******************************************************************************

 $Function:  sim_toolkit_exec

 $Description:   Dialog function for sim toolkit top window.
        Handle the SAT commands delivered by MFW<-ACI<-SIM

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static void sim_toolkit_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command)
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *) win)->data;
    T_sim_toolkit    * data     = (T_sim_toolkit *)win_data->user;
    T_MFW_HND         sat_win;
    T_SAT_RES sat_res;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
#endif
#ifdef FF_MMI_R99_SAT_LANG
     T_DISPLAY_DATA display_info;
#endif

    TRACE_FUNCTION ("sim_toolkit_exec()");

    if (event NEQ SAT_SESSION_END)
        data->active_sat_session = (T_SAT_EVENTS) event;

	if (data->info_win != NULL)
	{
		SEND_EVENT (data->info_win, E_ED_DEINIT, 0, NULL);
		data->info_win = NULL;
	}


    switch (event)
        {        
        #ifdef FF_MMI_R99_SAT_LANG
        case SAT_LOCAL_INFO:
		/*OMAPS00070661(SAT-Local Info(lang setting)) a0393213(R.Prabakar)*/
		TRACE_EVENT ("SAT Local Information ");
		if(sat_command->qual==LOCAL_INFO_LANG_SETTING_QUALIFIER)/*local information - language setting*/
		{
			UBYTE lang[2]; 

			switch(Mmi_getCurrentLanguage())
				{
				case ENGLISH_LANGUAGE:
					lang [0] = ASCII_FOR_E;  /* 'e' */
					lang [1] = ASCII_FOR_N; /* 'n' */
					sat_res[SAT_ERR_INDEX] = SAT_RES_SUCCESS;
					sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
					break;
				case CHINESE_LANGUAGE:
					lang [0] = ASCII_FOR_Z;  /* 'z' */
					lang [1] = ASCII_FOR_H;  /* 'h' */
					sat_res[SAT_ERR_INDEX] = SAT_RES_SUCCESS;
					sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
					break;
				case GERMAN_LANGUAGE:
					lang [0] = ASCII_FOR_D;  /* 'd' */
					lang [1] = ASCII_FOR_E;  /* 'e' */ 
					sat_res[SAT_ERR_INDEX] = SAT_RES_SUCCESS;
					sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
					break;					
				default:
					sat_res[SAT_ERR_INDEX]=SAT_RES_MISSING;
					sat_res[SAT_AI_INDEX]  = SatResAiNoService;
				}
			/* Terminal Response - Specifying language set in ME */
			satLocalInfo(sat_command, sat_res, sizeof(sat_res), lang, sizeof(lang));
		}
            break;
        case SAT_LANGUAGE_NOTIFY: 
		/*OMAPS00070661(SAT-Lang notify) a0393213(R.Prabakar)*/
             TRACE_EVENT ("Language Notification ");
		
		/* Terminal Response to SIM - OK for getting the notification of language */
		sat_res[SAT_ERR_INDEX] = SAT_RES_SUCCESS;
		sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		
		if(sat_command->qual == LANG_NOTIFY_SPEC_LANG_QUALIFIER) /*specified language notification*/
		{
			/* Check for language and set the respective language */
			if ((sat_command->c.language.lang[0] == ASCII_FOR_E) && (sat_command->c.language.lang[1] == ASCII_FOR_N))   /* English */
			{
			/* set as English */
			Mmi_Set2Language(ENGLISH_LANGUAGE);
			}
			else if ((sat_command->c.language.lang[0] == ASCII_FOR_D) && (sat_command->c.language.lang[1] == ASCII_FOR_E))   /* German */    
			{
			/* Set as German */
			Mmi_Set2Language(GERMAN_LANGUAGE);                 

			}
			else if ((sat_command->c.language.lang[0] == ASCII_FOR_Z) && (sat_command->c.language.lang[1] == ASCII_FOR_H))   /* Chineese */    
			{
			/* Set as Chineese */
			Mmi_Set2Language (CHINESE_LANGUAGE);
			}
			else
			{
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNotAllowed, TxtNull , COLOUR_STATUS_PINS);
			dlg_initDisplayData_events( &display_info, NULL, THREE_SECS, KEY_CLEAR | KEY_RIGHT );
			info_dialog (0, &display_info);
			/* Terminal Response to SIM - OK for getting the notification of language */
			sat_res[SAT_ERR_INDEX] = SAT_RES_REJECT;/*error - language not set*/
			sat_res[SAT_AI_INDEX]  = SatResAiNoService;
			}
		}
		else if(sat_command->qual == LANG_NOTIFY_NON_SPEC_LANG_QUALIFIER) /* non-specified language notification*/
		{
			Mmi_Set2Language(ENGLISH_LANGUAGE);
		}
		sat_done (sat_command, sat_res);      
            break;  
	  #endif /*FF_MMI_R99_SAT_LANG*/
        case SAT_DISPLAY_TEXT:
            TRACE_EVENT("SAT_DISPLAY_TEXT");
            /*
             * Text with normal priority will be displayed
             * only in idle screen and in SAT Menu Item
             */
            if ((sat_command->qual & SAT_M_TEXT_HIGH_PRIO) EQ 0)
            {
            TRACE_EVENT("NOT SAT_M_TEXT_HIGH_PRIO");
        /* Check if Idle AND Sat Session are inactiv    */
          if ((!g_SATsession_is_active)&&(!idleIsFocussed()))
        {
                TRACE_EVENT("BUSY SCREEN");
                    /* idle is not in focus and not currently in a SAT session: we are busy elsewhere */
            sat_res[SAT_ERR_INDEX] = SAT_RES_BUSY_ME;
            sat_res[SAT_AI_INDEX]  = SatResAiBusyScreen;
            sat_done (sat_command, sat_res);
                    return;
                }
            }

            sat_win = sat_display_text_create (data->win);
            if (sat_win)
                {
                    data->sat_command = sat_command;
                    SEND_EVENT (sat_win, event, 0, data->sat_command);
                }
            else
    {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
    }
            break;

        case SAT_PLAY_TONE:
            TRACE_EVENT("SAT_PLAY_TONE");
            /*
             * Create and initiate Play Tone Window
             */
            sat_win = sat_play_tone_create (data->win);
            if (sat_win)
                {
                    data->sat_command = sat_command;
                    SEND_EVENT (sat_win, event, 0, data->sat_command);
                }
            else
    {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
    }
            break;

        case SAT_GET_KEY:
            TRACE_EVENT("SAT_GET_KEY");
            /*
             * Create and initiate Get Key Window
             */
            sat_win = sat_get_key_create (data->win);
            if (sat_win)
                {
                    data->sat_command = sat_command;
                    SEND_EVENT (sat_win, event, 0, data->sat_command);
                }
            else
    {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
    }
            break;
        case SAT_GET_STRING:
            TRACE_EVENT("SAT_GET_STRING");
            /*
             * Create and initiate Get String Window
             */
            sat_win = sat_get_string_create (data->win);
            if (sat_win)
                {
                    data->sat_command = sat_command;
                    SEND_EVENT (sat_win, event, 0, data->sat_command);
                }
            else
    {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
    }
            break;

            // c016 rsa
        case SAT_SETUP_MENU:
            TRACE_EVENT("SAT_SETUP_MENU");

            sat_call_active = FALSE;   // Marcus: Issue 1812: 13/03/2003
      //SAT is on the SIM available
      data->sat_available_on_SIM = TRUE;

            /*
             * initiate Setup Menu
             */
            sat_setup_menu_proc(sat_command);
      sat_res[SAT_ERR_INDEX] = SatResSuccess;
      sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
            sat_done (sat_command, sat_res);
            break;

        case SAT_SELECT_ITEM:
            TRACE_EVENT("SAT_SELECT_ITEM");
            /*
             * Create and initiate select item Window
             */
            sat_win = sat_select_item_create (data->win);
            if (sat_win)
                {
                    data->sat_command = sat_command;
                    SEND_EVENT (sat_win, event, 0, data->sat_command);
                }
            else
    {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
    }
            break;
            // end c016 rsa

            // c001 xas
        case SAT_SEND_SMS:
            TRACE_EVENT("SAT_SEND_SMS");
            /*
             * --> display alpha identifier if available
             */
            if ((sat_command->c.sms.info.len NEQ 0) &&
                (sat_command->c.sms.info.len NEQ (U8)-1))
                {
                	char *TextString = (char *)sat_create_TEXT_ASCIIZ (&sat_command->c.sms.info);
                   /*
                    * Setup an editor to display a string
                    */



			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					AUI_edit_SetDefault(&data->editor_data);
					AUI_edit_SetTextStr(&data->editor_data, TxtNull, TxtNull, TxtNull, (UBYTE *)TextString);
					AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
					AUI_edit_SetEvents(&data->editor_data, event, TRUE, FOREVER, (T_AUI_EDIT_CB)sat_info_cb);
					// xreddymn Sep-06-2005 MMI-SPR-33876: Initialize editor buffer
					AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE*)"", 1);
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.sms.info.icon.qual != 0xFF)
		{
			/* Copy the icon data to the editor attributes*/
			data->editor_data.editor_attr.TitleIcon.width = sat_command->c.sms.info.iconInfo.width;  
			data->editor_data.editor_attr.TitleIcon.height = sat_command->c.sms.info.iconInfo.height; 

			icon_length = sat_command->c.sms.info.iconInfo.width * sat_command->c.sms.info.iconInfo.height;
										
			data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.sms.info.iconInfo.dst, icon_length);
			data->editor_data.editor_attr.TitleIcon.isTitle = TRUE;
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.sms.info.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
			     	}

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.sms.info.iconInfo.dst, icon_length);
				sat_command->c.sms.info.iconInfo.dst = NULL;
			 
		}
#endif
					
                    data->info_win = AUI_edit_Start(win, &data->editor_data);
#else /* NEW_EDITOR */
                    data->editor_data.LeftSoftKey          = TxtNull;
                    data->editor_data.AlternateLeftSoftKey = TxtNull;
                    data->editor_data.RightSoftKey         = TxtNull;
                    data->editor_data.hide                 = FALSE;
                    data->editor_data.mode                 = E_EDIT_READ_ONLY_MODE;
                    data->editor_data.timeout              = FOREVER;
                    data->editor_data.Identifier           = event;
                    data->editor_data.destroyEditor        =  TRUE;
                    data->editor_data.Callback   = (T_EDIT_CB)sat_info_cb;
                     data->editor_data.TextString       = (char *)TextString;
                    data->editor_data.min_enter  = 0;
                    data->info_win = editor_start(win, &data->editor_data);
#endif /* NEW_EDITOR */
                }
            break;

        case SAT_SEND_SS:
            TRACE_EVENT("SAT_SEND_SS");
            TRACE_EVENT_P1("sat_command->c.ss.info.len = %d", sat_command->c.ss.info.len);
            /*
             * --> display alpha identifier if available
             */
            if ((sat_command->c.ss.info.len NEQ 0) &&
                (sat_command->c.ss.info.len NEQ (U8)-1))
                {
                	char *TextString = (char *)sat_create_TEXT_ASCIIZ (&sat_command->c.ss.info);


                   /*
                    * Setup an editor to display a string
                    */
                    /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					AUI_edit_SetDefault(&data->editor_data);
					AUI_edit_SetTextStr(&data->editor_data, TxtNull, TxtNull, TxtNull, (UBYTE *)TextString);
					AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
					AUI_edit_SetEvents(&data->editor_data, event, TRUE, FOREVER, (T_AUI_EDIT_CB)sat_info_cb);
					// Dec 23, 2005    REF: OMAPS00062166     x0039928
					// Fix: Initialize the editor buffer
					AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE*)"", 1);
				
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.ss.info.icon.qual != 0xFF)
		{
			/* Copy the icon data to the editor attributes*/
			data->editor_data.editor_attr.TitleIcon.width = sat_command->c.ss.info.iconInfo.width;  
			data->editor_data.editor_attr.TitleIcon.height = sat_command->c.ss.info.iconInfo.height; 

			icon_length = sat_command->c.ss.info.iconInfo.width * sat_command->c.ss.info.iconInfo.height;
										
			data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.ss.info.iconInfo.dst, icon_length);
			data->editor_data.editor_attr.TitleIcon.isTitle = TRUE;
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.ss.info.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
			     	}

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.ss.info.iconInfo.dst, icon_length);
				sat_command->c.ss.info.iconInfo.dst = NULL;
			 
		}
#endif
			
		                    data->info_win = AUI_edit_Start(win, &data->editor_data);

#else /* NEW_EDITOR */
                    data->editor_data.LeftSoftKey          = TxtNull;
                    data->editor_data.AlternateLeftSoftKey = TxtNull;
                    data->editor_data.RightSoftKey         = TxtNull;
                    data->editor_data.hide                 = FALSE;
                    data->editor_data.mode                 = E_EDIT_READ_ONLY_MODE;
                    data->editor_data.timeout              = FOREVER;
                    data->editor_data.Identifier           = event;
                    data->editor_data.Callback   = (T_EDIT_CB)sat_info_cb;
                    data->editor_data.destroyEditor        =  TRUE;
                    data->editor_data.TextString       	= TextString;

                    data->editor_data.min_enter  = 0;
                    data->info_win = editor_start(win, &data->editor_data);
#endif /* NEW_EDITOR */
                }

/* 19 -Jan -2007 x0043642 OMAPS00110997 */
            else
            {

#ifdef FF_MMI_SAT_ICON
    /*Check if the SAT command has icon*/
                if(sat_command->c.ss.info.icon.qual != 0xFF)
                {
                        /* Free the memory of icon data that we got through the SATK command*/
                        mfwFree((U8 *)sat_command->c.ss.info.iconInfo.dst, sat_command->c.ss.info.iconInfo.width * 
                            sat_command->c.ss.info.iconInfo.height);
                        sat_command->c.ss.info.iconInfo.dst = NULL;
                }
#endif
                sat_res[SAT_ERR_INDEX] = SatResUnknownData;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done(sat_command, sat_res);
            }
            break;
            // end c001 xas


        case SAT_SEND_DTMF:
            TRACE_EVENT("SAT_SEND_DTMF");
            /*
             * --> display alpha identifier if available
             */
            if ((sat_command->c.cmd.info.len NEQ 0) &&
                (sat_command->c.cmd.info.len NEQ (U8)-1))
                {
                	char *TextString = (char *)sat_create_TEXT_ASCIIZ (&sat_command->c.cmd.info);


                   /*
                    * Setup an editor to display a string
                    */
                    /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					AUI_edit_SetDefault(&data->editor_data);
					AUI_edit_SetTextStr(&data->editor_data, TxtNull, TxtNull, TxtNull, (UBYTE *)TextString);
					AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
                                   /*06 OCT 2006, Ref OMAPS00095263, Archana(x0047075)
                                    Description : CT-GCF[27.22.4.24.1]-MS  Resets
                                    Solution     : Editor dialog is destroyed on destroy event instead of timeout. */
                                   AUI_edit_SetEvents(&data->editor_data, event, TRUE, FOREVER, (T_AUI_EDIT_CB)sat_info_cb);
					// Dec 23, 2005    REF: OMAPS00062166     x0039928
					// Fix: Initialize the editor buffer
					AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE*)"", 1);
				
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
		if(sat_command->c.cmd.info.icon.qual != 0xFF)
		{
			/* Copy the icon data to the editor attributes*/
			data->editor_data.editor_attr.TitleIcon.width = sat_command->c.cmd.info.iconInfo.width;  
			data->editor_data.editor_attr.TitleIcon.height = sat_command->c.cmd.info.iconInfo.height; 

			icon_length = sat_command->c.cmd.info.iconInfo.width * sat_command->c.cmd.info.iconInfo.height;
										
			data->editor_data.editor_attr.TitleIcon.data = (char *)ALLOC_MEMORY (icon_length);

			memcpy(data->editor_data.editor_attr.TitleIcon.data, sat_command->c.cmd.info.iconInfo.dst, icon_length);
			data->editor_data.editor_attr.TitleIcon.isTitle = TRUE;
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(sat_command->c.cmd.info.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					data->editor_data.editor_attr.TitleIcon.selfExplanatory = TRUE;
			     	}

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)sat_command->c.cmd.info.iconInfo.dst, icon_length);
				sat_command->c.cmd.info.iconInfo.dst = NULL;
			 
		}
#endif
			
                    			data->info_win = AUI_edit_Start(win, &data->editor_data);

#else /* NEW_EDITOR */
                    data->editor_data.LeftSoftKey          = TxtNull;
                    data->editor_data.AlternateLeftSoftKey = TxtNull;
                    data->editor_data.RightSoftKey         = TxtNull;
                    data->editor_data.hide                 = FALSE;
                    data->editor_data.mode                 = E_EDIT_READ_ONLY_MODE;
                    /*06 OCT 2006, Ref OMAPS00095263, Archana(x0047075)
                    Description : CT-GCF[27.22.4.24.1]-MS  Resets
                    Solution     : Editor dialog is destroyed on destroy event instead of timeout. */
                    data->editor_data.timeout              = FOREVER;
                    data->editor_data.Identifier           = event;
                    data->editor_data.Callback   = (T_EDIT_CB)sat_info_cb;
                    data->editor_data.destroyEditor        =  TRUE;
                    data->editor_data.TextString       	= TextString;

                    data->editor_data.min_enter  = 0;
                    data->info_win = editor_start(win, &data->editor_data);
#endif /* NEW_EDITOR */
                }
            break;
        case SAT_SETUP_CALL:
            TRACE_EVENT("SAT_SETUP_CALL");

            TRACE_EVENT_P2("Redial durUnit %d durValue %d",
				sat_command->c.call.durUnit,sat_command->c.call.durValue);
            data->sat_command = sat_command;

            /* create parameter space for call, or reuse old */
            if (data->call_setup_parameter EQ NULL)
            {
                data->call_setup_parameter =
                    (T_SAT_call_setup_parameter *)ALLOC_MEMORY (sizeof(T_SAT_call_setup_parameter));
            }

            /* SPR#1784 - DS - If command qualifier is set to 'with redial' (01, 03 or 05) set a global redial flag */
            if ( (sat_command->qual & 0x01) == TRUE )
            {
                if (sat_command->qual > 0x05)
                {
                    TRACE_EVENT("Invalid call command qualifier!");
                }
                else
                {
/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/
                }
            }


            /* gather the name of the called party (if any) */
            if ((sat_command->c.call.info.len NEQ 0) &&
                (sat_command->c.call.info.len NEQ (U8)-1))
            {
                TRACE_EVENT("call.info contains data.");
                data->call_setup_parameter->TextString = sat_create_TEXT_ASCIIZ(&sat_command->c.call.info);
                //x0035544 Feb 07, 2006 DR:OMAPS00061467
	//copy the icon data for  the first alpha identifier
	#ifdef FF_MMI_SAT_ICON
    		if (sat_command->c.call.info.icon.qual != 0xFF)
         		{		
			data->call_setup_parameter->IconInfo.width = sat_command->c.call.info.iconInfo.width;
			data->call_setup_parameter->IconInfo.height= sat_command->c.call.info.iconInfo.height;
			icon_length = sat_command->c.call.info.iconInfo.width * sat_command->c.call.info.iconInfo.height;	
			if(sat_command->c.call.info.iconInfo.dst != NULL)
				{
			data->call_setup_parameter->IconInfo.dst = (char *)ALLOC_MEMORY (icon_length);
			memcpy(data->call_setup_parameter->IconInfo.dst, sat_command->c.call.info.iconInfo.dst, icon_length);
				}
			     if(sat_command->c.call.info.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					data->call_setup_parameter->IconInfo.selfExplanatory = TRUE;
										
			     	}
			     else
					data->call_setup_parameter->IconInfo.selfExplanatory = FALSE;
				 
		  	/* Free the memory of icon data that we got through the SATK command*/
			if(sat_command->c.call.info.iconInfo.dst != NULL)
				{
				 mfwFree((U8 *)sat_command->c.call.info.iconInfo.dst, icon_length);
				sat_command->c.call.info.iconInfo.dst = NULL;		 
				}
         	}
	else
		{
		data->call_setup_parameter->IconInfo.dst = NULL;
		data->call_setup_parameter->IconInfo.width = 0;
		data->call_setup_parameter->IconInfo.height = 0;
		data->call_setup_parameter->IconInfo.selfExplanatory = FALSE;
		}
			
       #endif		
            }
            else
            {
                /* provide an empty string; NULL would crash in info_dialog() */
                TRACE_EVENT("call.info contains no data!");
                sat_command->c.call.info.len = 0;

                /* SPR#1700 - DS - Set string to NULL */
                data->call_setup_parameter->TextString = NULL;

                /* SPR#1700 - DS - Commented out code below. Checks in call() in mmiCall.c ensure
                 * a NULL string is not sent to info_dialog
                 */
#if 0
                data->call_setup_parameter->TextString = sat_create_TEXT_ASCIIZ(&sat_command->c.call.info);
#endif /* 0 */
            }

            /* SPR#1700 - DS - Save info2 (if available). This should be used for the "calling" dialog.
             * If not available, info (above) can be used.
             */

             /* gather the name of the called party (if any) */
            if ((sat_command->c.call.info2.len NEQ 0) &&
                (sat_command->c.call.info2.len NEQ (U8)-1))
            {
                TRACE_EVENT("call.info2 contains data.");
                data->call_setup_parameter->TextString2 = sat_create_TEXT_ASCIIZ(&sat_command->c.call.info2);
        //x0035544 Feb 07, 2006 DR:OMAPS00061467
 	//copy the icon data for  the second alpha identifier 
    #ifdef FF_MMI_SAT_ICON
	if (sat_command->c.call.info2.icon.qual != 0xFF)
         	{         	
			data->call_setup_parameter->IconInfo2.width= sat_command->c.call.info2.iconInfo.width;
			data->call_setup_parameter->IconInfo2.height = sat_command->c.call.info2.iconInfo.height;
			data->call_setup_parameter->IconInfo2.dst = data->call_setup_parameter->IconInfo.dst;
			if(sat_command->c.call.info2.icon.qual == 0x00)
			     	{
					// Icon is self-explanatory. Do not display the text
					data->call_setup_parameter->IconInfo2.selfExplanatory = TRUE;
			     	}
			else
			data->call_setup_parameter->IconInfo2.selfExplanatory = FALSE;			
         	}
	else
		{
		data->call_setup_parameter->IconInfo2.dst = NULL;
		data->call_setup_parameter->IconInfo2.width = 0;
		data->call_setup_parameter->IconInfo2.height = 0;
		data->call_setup_parameter->IconInfo2.selfExplanatory = FALSE;
		}

 #endif	
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialogue displays them correctly*/
			 sat_add_unicode_tag_if_needed(data->call_setup_parameter->TextString2);
#endif
            }
            else
            {
                /* provide an empty string; NULL would crash in info_dialog() */
                TRACE_EVENT("call.info2 contains no data!");
                sat_command->c.call.info2.len = 0;

                 /* SPR#1700 - DS - Set string to NULL */
                data->call_setup_parameter->TextString2 = NULL;

                 /* SPR#1700 - DS - Commented out code below. Checks in call() in mmiCall.c ensure
                  *  a NULL string is not sent to info_dialog
                  */
#if 0
                data->call_setup_parameter->TextString2 = sat_create_TEXT_ASCIIZ(&sat_command->c.call.info2);
#ifdef NO_ASCIIZ/*MC SPR 940/2 Add tag to Unicode strings so info dialogue displays them correctly*/
			 sat_add_unicode_tag_if_needed(data->call_setup_parameter->TextString2);
#endif
#endif /* 0 */
            }
            break;

        case SAT_CALL_RESULT:
            if (data->call_setup_parameter EQ NULL)
            {
        /* we are NOT in a SETUP CALL session;this is a "normal" Call Result */
        TRACE_EVENT("SAT_CALL_RESULT a");
        /*
        * --> display alpha identifier if available
        */

		TRACE_EVENT_P1("Redial Time %d",sat_command->c.ccres.redialTime);

        if ((sat_command->c.ccres.info.len NEQ 0) &&
          (sat_command->c.ccres.info.len NEQ (U8)-1))
        {
        	char *TextString = (char *)sat_create_TEXT_ASCIIZ (&sat_command->c.ccres.info);


        /*
        * Setup an editor to display a string
          */
          /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&data->editor_data);
			AUI_edit_SetTextStr(&data->editor_data, TxtNull, TxtNull, TxtNull, (UBYTE *)TextString);
			AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
			AUI_edit_SetEvents(&data->editor_data, event, TRUE, FOREVER, (T_AUI_EDIT_CB)sat_info_cb);
			/* 01-06-2006, x0045876 (OMAPS00070741) */
			AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE*)"", 1);

	        data->info_win = AUI_edit_Start(win, &data->editor_data);
#else /* NEW_EDITOR */
			SmsSend_loadEditDefault(&data->editor_data);
			data->editor_data.LeftSoftKey          = TxtNull;
			data->editor_data.AlternateLeftSoftKey = TxtNull;
			data->editor_data.RightSoftKey         = TxtNull;
			data->editor_data.hide                 = FALSE;
			data->editor_data.mode                 = E_EDIT_READ_ONLY_MODE;
			data->editor_data.timeout              = FOREVER;
			data->editor_data.Identifier           = event;
			data->editor_data.Callback   = (T_EDIT_CB)sat_info_cb;
			data->editor_data.destroyEditor        =  TRUE;
			data->editor_data.TextId      		= TxtNull;
			data->editor_data.TextString       = TextString;
			data->editor_data.min_enter  = 0;
			data->info_win = editor_start(win, &data->editor_data);
#endif /* NEW_EDITOR */

        }
      }
      else
      {
        /* we are in a SETUP CALL session;this is a SATN return */
        U8 general_result;
        U8 additional_info;

        TRACE_EVENT("SAT_CALL_RESULT b");

        /* process result of a call setup */
        general_result = (U8)(sat_command->c.ccres.result); /*SPR#2042 - DS - Changed from .redialTime */
        additional_info = (U8)(sat_command->c.ccres.callId);

        if (((general_result & (U8)0xF0) EQ (U8)0x00) ||
          ((general_result & (U8)0xF0) EQ (U8)0x10))
        {
          /* command has been performed */
          /*
          * destroying the display of SETUP MENU will focus the idle screen (in order to show the incall info)
          * furthermore it's necessary to keep track of the used resources
          */
#if 0 /* CQ16437 : Do not need to exit menu in order to handle the call correctly */
          TRACE_EVENT("destroy SETUP menu");
          SEND_EVENT (sat_get_setup_menu_win(), SAT_EXIT, 0, NULL);
#endif
          return;
        }
        else if ((general_result & (U8)0xF0) EQ (U8)0x20)
        {
          /* command failed: may be worth to retry */
          switch (general_result)
          {
          case 0x20: /* ME currently unable to process command */
            TRACE_EVENT("SAT: ME unable");
            break;
          case 0x21: /* NW currently unable to process command */
            TRACE_EVENT("SAT: NW unable");
#ifdef FF_2TO1_PS
            if ((additional_info EQ (U8)CAUSE_NWCC_USER_BUSY) &&
#else
            if ((additional_info EQ (U8)M_CC_CAUSE_USER_BUSY) &&
#endif
              (data->call_setup_parameter NEQ NULL))
            {
              SEND_EVENT (sat_get_call_setup_win(), SAT_CALL_REDIAL, 0, data->call_setup_parameter); /* redial */
              return; /* do NOT release the call setup parameter */
            }
            break;
          case 0x22: /* user rejected call setup */
            TRACE_EVENT("user rejected call setup");
          case 0x23: /* user cleared down call */
            TRACE_EVENT("user cleared down call");
          default:
            TRACE_EVENT("default reason");
            break;
          }
        }
        else if ((general_result & (U8)0xF0) EQ (U8)0x30)
        {
          /* command failed: not worth to retry with identical command */
          TRACE_EVENT("SAT_CALL_RESULT permanent problem");
          /* await SAT_SESSION_END in order to react */
        }
        else
        {
          TRACE_EVENT("SAT_CALL_RESULT unexp. result code");
        }
        SEND_EVENT (win, SAT_CALL_END, 0, NULL); /* release the call setup parameter */
      }
            break;

        case SAT_CALL_ALERT:
            TRACE_EVENT("SAT_CALL_ALERT");
            if (data->call_setup_parameter EQ NULL)
            {
                /* in case we are called withoud previous CALL_SETUP (which shouldn't happen!) */
                data->call_setup_parameter =
                    (T_SAT_call_setup_parameter *)ALLOC_MEMORY (sizeof(T_SAT_call_setup_parameter));

                /* provide an empty string; NULL would crash in info_dialog() */
                sat_command->c.ccres.info.len = 0;
                data->call_setup_parameter->TextString = sat_create_TEXT_ASCIIZ(&sat_command->c.ccres.info);

            }

            /* gather callId and redialTime */
            data->call_setup_parameter->callId = sat_command->c.ccres.callId;
            data->call_setup_parameter->redialTime = sat_command->c.ccres.redialTime;

            /* start a dialog to accept or reject the call */
      sat_win = sat_call_setup_create(data->win);
            if (sat_win)
      {
        data->sat_command = sat_command;
        SEND_EVENT (sat_win, event,0,data->call_setup_parameter);
      }
            else
      {
        sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
        sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
        sat_done (sat_command, sat_res);
      }
            break;

        case SAT_SESSION_END:
            ETRACE(sprintf(buf, "SAT_SESSION_END %d", data->active_sat_session));
      data->active_sat_session = (T_SAT_EVENTS)event;

            if (data->info_win NEQ NULL)
      {
        /* destroy any infoscreen */
        /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		SEND_EVENT (data->info_win, E_ED_DEINIT, 0, NULL);
#else /* NEW_EDITOR */
        SEND_EVENT (data->info_win, E_EDITOR_DEINIT, 0, NULL);
#endif /* NEW_EDITOR */
        data->info_win = NULL;
      }


      /*
       * Marcus: Issue 1057: 21/01/2003: Made SEND_EVENT conditional on
       * call_SATCall being FALSE.
       */
      if (call_SATCall == FALSE)
      {
          /* Marcus: Issue 1812: 13/03/2003: Start */
          if (sat_call_active)
          {
            sat_call_active = FALSE;
            SEND_EVENT (sat_get_setup_menu_win(), SAT_EXIT, 0, NULL);
          }
          else
          {
            SEND_EVENT (sat_get_setup_menu_win(), SAT_RETURN, 0, NULL); /* recreate the SETUP MENU */
          }
          /* Marcus: Issue 1812: 13/03/2003: End */
      }
        break;

        case SAT_CALL_END:
            TRACE_EVENT("SAT_CALL_END");
            if (data->call_setup_parameter NEQ NULL)
            {
                /* release the call setup parameters */
                sat_destroy_TEXT_ASCIIZ(data->call_setup_parameter->TextString);

	   //x0035544 Feb 07, 2006 DR:OMAPS00061467  ARUNKS
          //Release the icon data.
         #ifdef FF_MMI_SAT_ICON
	      if (data->call_setup_parameter->IconInfo.dst != NULL)
		   {
			FREE_MEMORY((U8 *)data->call_setup_parameter->IconInfo.dst, data->call_setup_parameter->IconInfo.width * 
								data->call_setup_parameter->IconInfo.height);
			data->call_setup_parameter->IconInfo.dst = NULL;
		   }
	   
         #endif
                FREE_MEMORY ((U8 *)data->call_setup_parameter, sizeof (T_SAT_call_setup_parameter));
                data->call_setup_parameter = NULL;

                SEND_EVENT (sat_get_call_setup_win(), event, 0, NULL); /* destroy the window */


            }

             /* SPR#1784 - DS - Destroy any information screen.
             */
              if (data->info_win NEQ NULL)
              {
                TRACE_EVENT("Destroy the info screen");

                /* destroy any infoscreen */
#ifdef NEW_EDITOR
                SEND_EVENT (data->info_win, E_ED_DEINIT, 0, NULL);
#else /* NEW_EDITOR */
                SEND_EVENT (data->info_win, E_EDITOR_DEINIT, 0, NULL);
#endif /* NEW_EDITOR */
                data->info_win = NULL;
              }

            break;

        case SAT_UNKNOWN:
            TRACE_EVENT("SAT_UNKNOWN");
            /*
             * unknown SAT commands are rejected
             */
      sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
      sat_res[SAT_AI_INDEX]  = SatResAiNoService;
            sat_done (sat_command, sat_res);
            break;

       case SAT_REFRESH:
            TRACE_EVENT("SAT_REFRESH");
#ifdef MMI_HOMEZONE_ENABLED
{
int i;
    for (i=0;i<sat_command->c.refFiles.files_nr;i++)
    {
      /*a0393213 OMAPS00116309 Data type of 'files' changed from U16* to T_file_info*
	because of R99 change in ACI*/
      switch(sat_command->c.refFiles.files[i].datafield)
      {
         case HZ_SIM_PARAMETERS:
         case HZ_SIM_CELL_CACHE:
         case HZ_SIM_CELL_CACHE+1:
         case HZ_SIM_CELL_CACHE+2:
         case HZ_SIM_CELL_CACHE+3:
         	/* Read new sim fields into memory.  SPR877 - SH - Parameter specifies that SIM fields
         	 * should be read in, even if they were already read at startup. */
         	homezoneReadSim(HZ_READ_SIM_AGAIN);
         	return;
         default:
         break;
      }
    }
}
#endif
            break;

	   case SAT_SET_EVENTS:
			break;
	   case SAT_IDLE_TEXT:
			{
				/* xreddymn OMAPS00080543 Jun-06-2006 */
				SatTxt  *inText;
				unsigned char    *txtBuf; /* a0393213 LINT warning removal - char* changed to unsigned char* */
				UBYTE   outDCS;
				U16     length;

				inText = &sat_command->c.text;

		   		if (inText->len == 0)
	   			{
					// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
				       addSatMessage(NULL, 0, 0, NULL, FALSE, SAT_ICON_IDLEMODE_TEXT);
					/* Free the memory of icon data that we got through the SATK command*/
					if(sat_command->c.text.iconInfo.dst != NULL)
					{
						 mfwFree((U8 *)sat_command->c.text.iconInfo.dst, sat_command->c.text.iconInfo.width *
						 	sat_command->c.text.iconInfo.height);
						sat_command->c.text.iconInfo.dst  = NULL;
					}
					
#else
					addSatMessage(NULL);
#endif

					/* 09-May-2006, x0045876 (OMAPS00064076 - By sending SatResUnKnownData, 
					    TC 27.22.4.22.1 SEQ 1.3 was failing. It was changed due to integration problem) */

/* 16 -Jan -2007 x0043642 OMAPS00110997 */
#ifdef FF_MMI_SAT_ICON
                                if(sat_command->c.text.icon.qual != 0xFF)
                                {
                                    sat_res[SAT_ERR_INDEX] = SatResUnknownData;
                                    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                                    sat_done(sat_command, sat_res);
                                
                                }

                                else
#endif
                                {
                
                                    sat_res[SAT_ERR_INDEX] = SatResSuccess;
                                    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                                    sat_done(sat_command, sat_res);
                                }
	   			}
				else
				{
					/* xreddymn OMAPS00080543 Jun-06-2006 */
					/* GSM 7-bit DCS */
					if ((inText->code == 0x00) || ((inText->code & 0x0c) == 0x00))
					{
						length = (((inText->len * 8) + 7) / 7) + 1;
					}
					/* 8-bit DCS (ASCII) or UCS2 DCS */
					else
					{
						length = inText->len + 1;
					}
					txtBuf = (unsigned char *)ALLOC_MEMORY(length);

					if (!txtBuf)
					{
						sat_res[SAT_ERR_INDEX] = SatResImpossible; 
						sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
						sat_done(sat_command, sat_res);
					}
					else
					{
						/* xreddymn OMAPS00080543 Jun-06-2006 */
						memset(txtBuf, 0x00, length);

						/* START: 09-May-2006, x0045876 (OMAPS00064076) */
						if (inText->code==MFW_DCS_UCS2)
							{
							outDCS = MFW_DCS_UCS2;
							/* 	Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
							 	Description : CT-GCF[27.22.4.22.1]-Incorrect string display
							 	Solution     : Editor DCS attribute set dynamically based upon the DCS of the incoming text*/
							#ifdef NEW_EDITOR
								idle_data.editor_attr_sat.text.dcs=ATB_DCS_UNICODE;
							#endif
							}
						else
							{
							outDCS = MFW_ASCII;
							/* 	Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
							 	Description : CT-GCF[27.22.4.22.1]-Incorrect string display
							 	Solution     : Editor DCS attribute set dynamically based upon the DCS of the incoming text*/
							#ifdef NEW_EDITOR
								idle_data.editor_attr_sat.text.dcs=ATB_DCS_ASCII;
							#endif
							}
						/* END: 09-May-2006, x0045876 (OMAPS00064076) */

						TRACE_EVENT_P3("NDH >>> inDcs : %02x, outDCS : %02x, Text Len : %d", inText->code,
										   outDCS, inText->len);
						
						if ((inText->code == 0x00) || ((inText->code & 0x0c) == 0x00)) // GSM 7-Bit
						{
							ATB_convert_String((char *)inText + inText->text,
												MFW_DCS_7bits, inText->len,
												(char*)txtBuf,
												/* xreddymn OMAPS00080543 Jun-06-2006
												 * Removed hardcoded length.
												 */
												outDCS, length-1,
												TRUE);
						}
						else if ((inText->code & 0x0c) == 0x04)	// ASCII 8-Bit
						{
/* xreddymn OMAPS00080543 Jun-06-2006 
 * Since long idle screen text display is now supported, we no longer
 * need to check length of the message.
 */						
#if 0
							if (outDCS == MFW_ASCII)
							{
								if (inText->len > length-1)
								{
									//CQ - 19656 Sandip Start
									//For Input DCS 0x00, we are handling long strings , so also for input DCS 0x04 we should handle long strings
									#if 0
									/*
									** Display Text too long ... Send failure TR to SIM
									*/
									sat_res[SAT_ERR_INDEX] = SatResImpossible; 
									sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
									sat_done(sat_command, sat_res);

									FREE_MEMORY((U8 *)txtBuf, MAX_CBMSG_LEN);
									return;
                                                               #endif
                                                               //CQ - 19656 Sandip
                                                                ATB_convert_String((char *)inText + inText->text,
												MFW_DCS_8bits, inText->len,
												txtBuf,
												outDCS, MAX_CBMSG_LEN-1,
												TRUE);
									//CQ - 19656 Sandip End
								}
								else
								{
									//MMI_FIX-19656   Using 8 bit table for dispay start
									ATB_convert_String((char *)inText + inText->text,
													MFW_DCS_8bits, inText->len,
													txtBuf,
													outDCS, MAX_CBMSG_LEN-1,
													TRUE);
									//MMI_FIX-19656   Using 8 bit table for dispay end
									//memcpy(txtBuf, (char *)inText + inText->text, inText->len);
								}
							}
							else
#endif
							{
								ATB_convert_String((char *)inText + inText->text,
													MFW_DCS_8bits, inText->len,
													(char*)txtBuf,
													/* xreddymn OMAPS00080543 Jun-06-2006
													 * Removed hardcoded length.
													 */
													outDCS, length-1,
													TRUE);
							}
						}
						else // UCS2
/* xreddymn OMAPS00080543 Jun-06-2006 
 * Since long idle screen text display is now supported, we no longer
 * need to check length of the message.
 */			
/*10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
    Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
    Solution     : #if 0 added for OMAPS00080543 moved*/
    			   
 						{

							if (outDCS == MFW_DCS_UCS2)
							{
								#if 0
								if (inText->len > MAX_CBMSG_LEN-1)
								{
									/*
									** Display Text too long ... Send failure TR to SIM
									*/
									sat_res[SAT_ERR_INDEX] = SatResImpossible; 
									sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
									sat_done(sat_command, sat_res);

									FREE_MEMORY((U8 *)txtBuf, MAX_CBMSG_LEN);
									return;
								}
								else
								#endif
								{
									/*Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
									   Solution : Changes made as part of OMAPS00064076 undone. 
									   Before we were appending 0x80 and 0x7F to the original string to describe that the following text is UNICODE.
									   Now since the attribute of the editor is already set to UNICODE, no need to append 0x80 and 0x7F*/
    									memcpy(txtBuf, (UBYTE*)inText+inText->text, inText->len);

								}
							}
							else

							{
								ATB_convert_String((char *)inText + inText->text,
													MFW_DCS_UCS2, inText->len,
													(char*)txtBuf,
													/* xreddymn OMAPS00080543 Jun-06-2006
													 * Removed hardcoded length.
													 */
													outDCS, length-1,
													TRUE);
							}
						}

						TRACE_EVENT_P1("NDH >>> txtBuf is : %s", txtBuf);
						
						// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
/*Check if the SAT command has icon*/
				if(sat_command->c.text.icon.qual != 0xFF)
				{
					/* Icon is self-explanatory. No need to display text for this case.*/
					     if(sat_command->c.text.icon.qual == 0x00)
					     	{
							/* Icon is self-explanatory. Do not display the text.
								Send a TRUE to the Idle window. */
							/*Oct 13, 2006 OMAPS00095266 a0393213(R.Prabakar)
							   Solution : Changes made as part of OMAPS00077654 undone.
							   Before we were setting the first two bytes to 0x80 and 0x7F and sending the text from the third byte.
							   Now we are not appending any string to the original string. So the buffer is sent right from the first byte*/
							       addSatMessage((char*)txtBuf, sat_command->c.text.iconInfo.width, sat_command->c.text.iconInfo.height,
								(char *)sat_command->c.text.iconInfo.dst, TRUE, SAT_ICON_IDLEMODE_TEXT); 
					     	}
					     else
					     	{
								addSatMessage((char*)txtBuf, sat_command->c.text.iconInfo.width, sat_command->c.text.iconInfo.height,
								(char *)sat_command->c.text.iconInfo.dst, FALSE, SAT_ICON_IDLEMODE_TEXT); 
					     	}
				}
				else
				{
						addSatMessage((char*)txtBuf, 0, 0, NULL, FALSE, SAT_ICON_NONE); 
				}

#else
						/* START: 09-May-2006, x0045876 (OMAPS00064076) */
							addSatMessage((char*)txtBuf);
						/* END: 09-May-2006, x0045876 (OMAPS00064076) */
#endif	

						sat_res[SAT_ERR_INDEX] = SatResSuccess; 
						sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
						sat_done(sat_command, sat_res);
						// 09-Dec-2005, Shashi Shekar B.S., a0876501, START
#ifdef FF_MMI_SAT_ICON
						/* Free the memory of icon data that we got through the SATK command*/
						if(sat_command->c.text.iconInfo.dst != NULL)
						{
							 mfwFree((U8 *)sat_command->c.text.iconInfo.dst, sat_command->c.text.iconInfo.width *
							 	sat_command->c.text.iconInfo.height);
							sat_command->c.text.iconInfo.dst  = NULL;
						}
#endif
						/* xreddymn OMAPS00080543 Jun-06-2006 */
						FREE_MEMORY((U8 *)txtBuf, length);
					}
				}
	   		}
			break;

#ifdef FF_WAP
       case SAT_LAUNCH_BROWSER:

		break;
#endif
        case SAT_OPEN_CHANNEL:
            TRACE_EVENT("SAT_OPEN_CHANNEL");

             /*
             * Create and initiate Open Channel Window
             */
            sat_win = sat_class_e_create (data->win);

            if (sat_win)
            {
                data->sat_command = sat_command;
                SEND_EVENT (sat_win, event, 0, data->sat_command);
            }
            else
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
            }
            break;
        case SAT_CLOSE_CHANNEL:
            TRACE_EVENT("SAT_CLOSE_CHANNEL");

             /*
             * Create and initiate Close Channel window
             */
            sat_win = sat_class_e_create (data->win);

            if (sat_win)
            {
                data->sat_command = sat_command;
                SEND_EVENT (sat_win, event, 0, data->sat_command);
            }
            else
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
            }
            break;
        case SAT_SEND_DATA:
            TRACE_EVENT("SAT_SEND_DATA");

             /*
             * Create and initiate Send Data window
             */
            sat_win = sat_class_e_create (data->win);

            if (sat_win)
            {
                data->sat_command = sat_command;
                SEND_EVENT (sat_win, event, 0, data->sat_command);
            }
            else
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
            }
            break;
        case SAT_RECEIVE_DATA:

            TRACE_EVENT("SAT_RECEIVE_DATA");

             /*
             * Create and initiate Receive Data window
             */
            sat_win = sat_class_e_create (data->win);

            if (sat_win)
            {
                data->sat_command = sat_command;
                SEND_EVENT (sat_win, event, 0, data->sat_command);
            }
            else
            {
                sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
                sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
                sat_done (sat_command, sat_res);
            }
            break;
        default:
            TRACE_EVENT("sim_toolkit_exec() unexp. event");
            break;
        }
}
/*******************************************************************************

 $Function:  sim_toolkit_sat_cb

 $Description:    Callback function for SAT handler of top window

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static int sim_toolkit_sat_cb ( T_MFW_EVENT event, T_MFW_SAT * sat_bits)
{
    USHORT sat_event = 0;   // RAVI
 
    TRACE_EVENT("sim_toolkit_sat_cb");
    /*
     * check whether a decoded message is available
     */
    if ((sat_bits EQ NULL) || (g_sim_toolkit_data->win EQ NULL))
    {
        TRACE_EVENT("sim_toolkit_sat_cb() unexp. event");
        return MFW_EVENT_CONSUMED;
    }

    /* convert the flag into an event */
    if (event == MfwSatSessionEnd)
    {
        sat_event = SAT_SESSION_END;
    }
    else
    {
        switch (event)
        {
        case MfwSatTextOut:         /* display string           */
            sat_event = SAT_DISPLAY_TEXT;
            break;
        case MfwSatGetKey:          /* get user keystroke       */
            sat_event = SAT_GET_KEY;
            break;
        case MfwSatGetString:       /* get user input           */
            sat_event = SAT_GET_STRING;
            break;
        case MfwSatPlayTone:        /* play audio tone          */
            sat_event = SAT_PLAY_TONE;
            break;
        case MfwSatSetupMenu:       /* setup toolkit menu       */
            sat_event = SAT_SETUP_MENU;
            break;
        case MfwSatSelectItem:      /* select menu item         */
            sat_event = SAT_SELECT_ITEM;
            break;
        case MfwSatSendSMS:         /* send short message       */
            sat_event = SAT_SEND_SMS;
            break;
        case MfwSatSendUSSD:
        case MfwSatSendSS:          /* send service command     */
            sat_event = SAT_SEND_SS;
            break;
        case MfwSatSendDTMF:          /* send service command     */
            sat_event = SAT_SEND_DTMF;
            break;
        case MfwSatCall:            /* setup a call             */
            sat_event = SAT_SETUP_CALL;
            break;
        case MfwSatCcRes:           /* call control result      */
            sat_event = SAT_CALL_RESULT;
            break;
        case MfwSatCcAlert:         /* call control alerting    */
            sat_event = SAT_CALL_ALERT;
            break;
        case MfwSatRefresh:         /* refresh SIM fields     */
            sat_event = SAT_REFRESH;
            break;
        case MfwSatSetEvents:         /* Sat Set Events updated   */
            sat_event = SAT_SET_EVENTS;
            break;
        case MfwSatIdleText:         /* Sat Set Events updated   */
            sat_event = SAT_IDLE_TEXT;
            break;
        case MfwSatDataRefreshed:         /* Data refreshed     */
				TRACE_EVENT("MfwSatDataRefreshed event sent");
            break;
        case MfwSatOpenChannel:         /* Open channel     */
	      sat_event = SAT_OPEN_CHANNEL;
            break;
        case MfwSatCloseChannel:         /* Close channel    */
	      sat_event = SAT_CLOSE_CHANNEL;
            break;
        case MfwSatSendData:         /* Send data     */
	      sat_event = SAT_SEND_DATA;
            break;
        case MfwSatReceiveData:         /* Receive data     */
	      sat_event = SAT_RECEIVE_DATA;
            break;
#ifdef FF_WAP /*Disable for testing without WAP*/
        case MfwSatLaunchBrowser:
				TRACE_EVENT("SAT_LAUNCH_BROWSER got to MMI");
#ifdef MMI_TEST_SAT_LAUNCH_BROWSER
           {
             #define LENGTH_URL 40
			 T_SAT_RES sat_res;
			 SatLaunchBrowser laun_brow;
			 char prov_url[LENGTH_URL];
             short pos = 0;
             int i;

             sat_res[SAT_ERR_INDEX] = SAT_RES_SUCCESS;
		     sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		     sat_done (sat_bits->cmd, sat_res);
            sat_event = SAT_LAUNCH_BROWSER;
             laun_brow = sat_bits->cmd->c.browser;
             memset(information,'\0',INFORMATION_SIZE);
			 TRACE_EVENT_P1("Browser identity -> %d",laun_brow.identity);
             memcpy(&prov_url,laun_brow.url,LENGTH_URL);
             prov_url[LENGTH_URL-1] = '\0';
			 TRACE_EVENT_P1("Browser url -> %s",prov_url);
             pos += sprintf(information+pos,
             	"BROWSER\n-Identity=%d\n-Url=%s\n-Bearers(%d)=",
             	laun_brow.identity,
             	prov_url,
             	laun_brow.n_bearer);
             for (i=0;i<5;i++)
                 pos+=sprintf(information+pos," %d",*laun_brow.bearer++);
             mmiOpenDummyBrowser(g_sim_toolkit_data->win,information);
		   }
			break;
#endif /*FF_WAP*/
#endif /* MMI_TEST_SAT_LAUNCH_BROWSER */
#ifdef FF_MMI_R99_SAT_LANG
        case MfwSatLangNotify: 		/*OMAPS00070661(SAT-Lang notify) a0393213(R.Prabakar)*/
            sat_event = SAT_LANGUAGE_NOTIFY;
            break;
        case MfwSatLocalInfo:		/*OMAPS00070661(SAT-Local Info) a0393213(R.Prabakar)*/
            sat_event = SAT_LOCAL_INFO;
            break;
#endif /*FF_MMI_R99_SAT_LANG*/
	case MfwSatErrBusy:
		TRACE_EVENT("MfwSatError reached BMI");
		{
		T_DISPLAY_DATA display_info;

		/* SPR#2321 - DS - Display "SAT Busy" dialog */

		dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, "SAT Busy", "Reset App" , COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_RIGHT | KEY_LEFT );
		info_dialog (g_sim_toolkit_data->win, &display_info);
		}

		/* No SEND_EVENT */

		return MFW_EVENT_CONSUMED;
        default:
            sat_event = SAT_UNKNOWN;
            break;
        }
    }
    SEND_EVENT (g_sim_toolkit_data->win, sat_event, 0, sat_bits->cmd);
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:  sat_setup_menu_proc

 $Description:   process SAT_SETUP_MENU event

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static void sat_setup_menu_proc(T_SAT_CMD * sat_command)
{
  TRACE_FUNCTION("sat_setup_menu_proc");

    if (sat_command->c.menu.items[0].len EQ 0)
    {
       /*
        * "Item data object for item 1" is a null data object:
        * hide the SIM toolkit menu item in the main menu
        */
        menuDisableSimMenu();
    g_sim_toolkit_data->sat_available_on_SIM = FALSE;
		//release the labe of the mainmenu
		//GW-SPR#1035 Free memory using sat_destroy_TEXT_ASCIIZ (as it is allocated using corresponding procedure)
		if (sat_mainmenu_label NEQ NULL)
		{
			sat_destroy_TEXT_ASCIIZ (sat_mainmenu_label);
			sat_mainmenu_label = NULL;
		}

        if (sat_get_setup_menu_win() NEQ NULL)
        {
            /* there's an active SETUP MENU */
            SEND_EVENT (sat_get_setup_menu_win(), SAT_EXIT, 0, NULL); /* SETUP_MENU shall selfdestroy */
        }
        if (g_sim_toolkit_data->sat_setup_menu_command NEQ NULL)
        {
            FREE_MEMORY ((U8 *)(g_sim_toolkit_data->sat_setup_menu_command), sizeof (T_SAT_CMD));
            g_sim_toolkit_data->sat_setup_menu_command = NULL;
        }
    }
    else
    {
        /* install new setup menu */
        if (sat_get_setup_menu_win() NEQ NULL)
        {
            /* there's already an active SETUP MENU */
            SEND_EVENT (sat_get_setup_menu_win(), SAT_EXIT, 0, NULL); /* SETUP_MENU shall selfdestroy */
            //xrashmic 16 Feb, 2006 OMAPS00064413 
	     //List win should be set to null, when destroying the setup menu.
            sat_set_setup_menu_listmnu_win(NULL);
        }

        /* store command contents for later use in sat_setup_menu_start() */
        if (g_sim_toolkit_data->sat_setup_menu_command EQ NULL)
        {
            /* if not already allocated, get storage for the SETUP MENU command */
            g_sim_toolkit_data->sat_setup_menu_command =
                (T_SAT_CMD *)ALLOC_MEMORY (sizeof (T_SAT_CMD));
        }
        *(g_sim_toolkit_data->sat_setup_menu_command) = *sat_command; /* copy contents */

/*#ifdef CHINESE_MMI*/
		/*
		  The label for the main menu

		*/
		TRACE_EVENT("SAT: Creating main menu label");
 		{
#ifdef NO_ASCIIZ
			/*MC,  SPR 940/2if we're in chinese, header may be converted from ASCII to Unicode*/
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				header_len = g_sim_toolkit_data->sat_setup_menu_command->c.menu.header.len *2;
			else
#endif
				/* taking the label from menu command file and convert to ascii */
				 header_len = g_sim_toolkit_data->sat_setup_menu_command->c.menu.header.len;


			sat_mainmenu_label = sat_create_TEXT_ASCIIZ (&g_sim_toolkit_data->sat_setup_menu_command->c.menu.header);
                  if(sat_mainmenu_label)
		  {
		    sat_mainmenu_label[header_len] = '\0';
                    /*JVJ #1576 The SimToolkit Title string is set here */
                    res_set_SIMToolkit_title_string(sat_mainmenu_label);
                  }                  
		}
        /* unhide the SIM toolkit menu item in the main menu */
        menuEnableSimMenu
            (
            "",  /* label */
            (SimMenuFunc)sat_setup_menu_start, /* callback when menu entered */
            g_sim_toolkit_data->sat_setup_menu_command /* parameter to callback */
            );
    }
    sat_set_setup_menu_win(NULL); // avoid notifications to a non-existent window
}


/*******************************************************************************

 $Function:  sat_win_cb

 $Description:   Default sat window event handler. Avoid intermediate visibility of lower menus


 $Returns:    none.

 $Arguments:

*******************************************************************************/
int sat_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  TRACE_FUNCTION("sat_win_cb");

    if (win EQ NULL)
  return MFW_EVENT_CONSUMED;

    switch (event)
        {
        case E_WIN_VISIBLE: /* window changed visibility */
            if (win->flags & E_WIN_VISIBLE)
                {
                    /* window becomes visible */
                    dspl_ClearAll(); /* overwrite redraw of lower menus with cleared screen */
                    dspl_TextOut(0,12,/*DSPL_TXTATTR_NORMAL*/DSPL_TXTATTR_CURRENT_MODE/*MC*/,GET_TEXT(TxtPleaseWait)); /* sbh - so we don't get blank screen */
                }
            break;
        default:
            return MFW_EVENT_REJECTED;
        }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:  sat_kbd_cb

 $Description:   Default sat window keyboard handler


 $Returns:    none.

 $Arguments:

*******************************************************************************/
int sat_kbd_cb (T_MFW_EVENT event, T_MFW_KBD * key)
{
  T_MFW_HND   win     = mfwParent( mfw_header() );

  TRACE_FUNCTION("sat_kbd_cb");

  /* make sure we handle null keyboards
  */
  if ( key == NULL )
    return MFW_EVENT_CONSUMED;

  /* deal with the incoming key code
  */
    switch( key->code )
    {

      /* Destroy the window on either right softkey or hangup keypresses */
    case KCD_HUP:
    case KCD_RIGHT:
    {
      SEND_EVENT(win, SAT_DESTROY_WINDOW, 0, 0);
      break;
    }
    }

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:  sat_info_cb

 $Description:  Callback function information dialog.

 $Returns:    none.

 $Arguments:

*******************************************************************************/
static void sat_info_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
    /* INFO DISPLAY */
    T_MFW_WIN       * info_win_data   = ((T_MFW_HDR *)win)->data;
    T_sim_toolkit   * info_data       = (T_sim_toolkit *)info_win_data->user;

    TRACE_FUNCTION("sat_info_cb");

    if (win EQ NULL)
        return;

        /*
        * Who has initiated the information screen
    */
    switch (identifier)
    {
      //x0035544 added fix done by x0021334 for CQ-33597 21-11-2005
        case SAT_SEND_DTMF: // x0021334 : To test SAT session end - CQ33597
    {
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		if (info_data->editor_data.editor_attr.TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)info_data->editor_data.editor_attr.TitleIcon.data, info_data->editor_data.editor_attr.TitleIcon.width * 
								info_data->editor_data.editor_attr.TitleIcon.height);
			info_data->editor_data.editor_attr.TitleIcon.data = NULL;
		}
#endif	

    	switch(reason)
    	{
       	case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
              	// Call mfw function to end the session
              	Mfw_SAT_DTMF_EndSession();
	       	#ifdef NEW_EDITOR
			AUI_edit_Destroy(info_data->info_win);
              	#else /* NEW_EDITOR */
			editor_destroy(info_data->info_win);
               	#endif /* NEW_EDITOR */
        	 	info_data->info_win = NULL;
      	        break;
             	 default:
             	 	TRACE_EVENT("sat_info_cb(): unexp. event");
             	  break;
    	   	}
    	}
        break;

    case SAT_SEND_SMS:
    case SAT_SEND_SS:
    //case SAT_SEND_DTMF: x0035544 CQ-33597 21-11-2005
    case SAT_CALL_RESULT:
        if ((info_win_data EQ NULL) || (info_data EQ NULL))
            return;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		if (info_data->editor_data.editor_attr.TitleIcon.data != NULL)
		{
			FREE_MEMORY((U8 *)info_data->editor_data.editor_attr.TitleIcon.data, info_data->editor_data.editor_attr.TitleIcon.width * 
								info_data->editor_data.editor_attr.TitleIcon.height);
			info_data->editor_data.editor_attr.TitleIcon.data = NULL;
		}
#endif		
        /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		sat_destroy_TEXT_ASCIIZ((char*)info_data->editor_data.TitleString); /* displayed text */
#else /* NEW_EDITOR */
        sat_destroy_TEXT_ASCIIZ((char*)info_data->editor_data.TextString); /* displayed text */
#endif /* NEW_EDITOR */
        break;
    default:
        TRACE_EVENT("sat_info_cb(): unexp. event");
        break;
    }
}


/*******************************************************************************

 $Function:  sat_set_setup_menu_win

 $Description:   set the window associated with the setup menu command

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_set_setup_menu_win(T_MFW_HND win)
{
  TRACE_FUNCTION("sat_set_setup_menu_win");

    g_sim_toolkit_data->sat_setup_menu_win = win;
}


/*******************************************************************************

 $Function:  sat_get_setup_menu_win

 $Description:   get the window associated with the setup menu command

 $Returns:    none.

 $Arguments:

*******************************************************************************/
T_MFW_HND sat_get_setup_menu_win(void)
{
  TRACE_EVENT("sat_get_setup_menu_win");
    return g_sim_toolkit_data->sat_setup_menu_win;
}


/*******************************************************************************

 $Function:  sat_set_setup_menu_listmnu_win

 $Description:   set the window associated with the sat list menu

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_set_setup_menu_listmnu_win(T_MFW_HND win)
{
  TRACE_FUNCTION("sat_set_setup_menu_listmnu_win");

    g_sim_toolkit_data->sat_setup_menu_listmnu_win= win;
}


/*******************************************************************************

 $Function:  sat_get_setup_menu_listmnu_win

 $Description:   get the window associated with the setup list menu

 $Returns:    none.

 $Arguments:

*******************************************************************************/
T_MFW_HND sat_get_setup_menu_listmnu_win(void)
{
  TRACE_EVENT("sat_get_setup_menu_listmnu_win");
    return g_sim_toolkit_data->sat_setup_menu_listmnu_win;
}


/*******************************************************************************

 $Function:  sat_set_call_setup_win

 $Description:   set the window associated with the call setup command

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_set_call_setup_win(T_MFW_HND win)
{
  TRACE_FUNCTION("sat_set_call_setup_win");
    g_sim_toolkit_data->sat_call_setup_win = win;
}


/*******************************************************************************

 $Function:  sat_get_call_setup_win

 $Description:    get the window associated with the call setup command

 $Returns:    none.

 $Arguments:

*******************************************************************************/
/* Marcus: Issue 1057: 21/01/2003: Made public */
T_MFW_HND sat_get_call_setup_win(void)
{
  TRACE_FUNCTION("sat_get_call_setup_win");
    return g_sim_toolkit_data->sat_call_setup_win;
}

/*******************************************************************************

 $Function:  g_ascii_gsm_table

 $Description:  convert a string coded in ASCIIZ into GSM alphabet

 $Returns:    none.

 $Arguments:

*******************************************************************************/
    static const char g_ascii_gsm_table[256] = {
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //000-007
       0x20,0x20,  10,0x20,0x20,  13,0x20,0x20, //008-015
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //016-023
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //024-031
         32,  33,  34,  35,   2,  37,  38,  39, //032-039
         40,  41,  42,  43,  44,  45,  46,  47, //040-047
         48,  49,  50,  51,  52,  53,  54,  55, //048-055
         56,  57,  58,  59,  60,  61,  62,  63, //056-063
          0,  65,  66,  67,  68,  69,  70,  71, //064-071
         72,  73,  74,  75,  76,  77,  78,  79, //072-079
         80,  81,  82,  83,  84,  85,  86,  87, //080-087
         88,  89,  90,0x20,0x20,0x20,0x20,0x20, //088-095
       0x20,  97,  98,  99, 100, 101, 102, 103, //096-103
        104, 105, 106, 107, 108, 109, 110, 111, //104-111
        112, 113, 114, 115, 116, 117, 118, 119, //112-119
        120, 121, 122,0x20,0x20,0x20,0x20,0x20, //120-127
          9, 126,   5,0x20, 123, 127,  15,0x20, //128-135
       0x20,0x20,   4,0x20,0x20,   7,  91,  14, //136-143
         31,  29,  28,0x20, 124,   8,0x20,   6, //144-151
       0x20,  92,  94,0x20,   1,   3,0x20,0x20, //152-159
       0x20,0x20,0x20,0x20, 125,  93,0x20,0x20, //160-167
         96,  17,0x20,0x20,0x20,  64,0x20,0x20, //168-175
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //176-183
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //184-191
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //192-199
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //200-207
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //208-215
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //216-223
       0x20,  30,  19,  22,  24,0x20,0x20,0x20, //224-231
         18,  25,  21,0x20,0x20,0x20,0x20,  20, //232-239
         26,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //240-247
       0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //248-255
    };

void sat_ascii_to_gsm (char * gsm_string, char * ascii_string, U16 length)
{
    U16 i;
  TRACE_FUNCTION("sat_ascii_to_gsm");

    for (i = 0; i < length; i++)
        gsm_string[i] = g_ascii_gsm_table[ascii_string[i]];
}

/*******************************************************************************

 $Function:  sat_ascii_to_ucode

 $Description:  convert a string coded in ASCIIZ into UCS2 alphabet

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_ascii_to_ucode (wchar_t * UCS2_chars, char * ascii_string, U16 length)
{
    U16 i;
  TRACE_FUNCTION("sat_ascii_to_ucode");
    for (i = 0; i < length; i++)
        UCS2_chars[i] = (wchar_t)(int)(ascii_string[i]);/*a0393213 extra casting done to avoid lint warning*/
}

/*******************************************************************************

 $Function:  sat_ucode_to_ascii

 $Description:   convert a string coded in UCS2 into ASCII alphabet

 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_ucode_to_ascii (char * ascii_string, wchar_t * UCS2_chars, U16 length)
{
    U16 i;
  TRACE_FUNCTION("sat_ucode_to_asci");

    for (i = 0; i < length; i++)
        ascii_string[i] = (char)UCS2_chars[i];
    ascii_string[i] = '\0';
}
/*******************************************************************************

 $Function:  g_gsm_ascii_table

 $Description:    convert a string coded in GSM alphabet into an ASCIIZ string

 $Returns:    none.

 $Arguments:

*******************************************************************************/

    static const unsigned char g_gsm_ascii_table[128] = {
         64, 156,  36, 157, 138, 130, 151, 141,
        149, 128,  10,0x20 , 32,  13, 143, 134,
       0x20, 169, 232, 226, 239, 234, 227,0x20,
        228, 233, 240,0x20, 146, 145, 225, 144,
         32,  33,  34,  35,0x20,  37,  38,  39,
         40,  41,  42,  43,  44,  45,  46,  47,
         48,  49,  50,  51,  52,  53,  54,  55,
         56,  57,  58,  59,  60,  61,  62,  63,
        173,  65,  66,  67,  68,  69,  70,  71,
         72,  73,  74,  75,  76,  77,  78,  79,
         80,  81,  82,  83,  84,  85,  86,  87,
         88,  89,  90, 142, 153, 165, 154,0x20,
        168,  97,  98,  99, 100, 101, 102, 103,
        104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 132, 148, 164, 129, 133
    };

void sat_gsm_to_ascii (char * ascii_string, char * gsm_string, U16 length)
{
    U16 i;
  TRACE_FUNCTION("sat_gsm_to_ascii");
  //  Jun 02, 2004 REF: CRR 15747  xpradipg-SASKEN 
  //  Fix: The string is terminated if the length is reached or if it encounters '0xFF'
  //  gsm_string[i]!=0xFF is introduced to check if the string is terminated prior to the specified length
   for (i = 0; i < length && gsm_string[i]!= (char)0xFF; i++)
        ascii_string[i] = (char)g_gsm_ascii_table[(gsm_string[i] & (U8)0x7F)];
    ascii_string[i] = '\0';
}


/*******************************************************************************

 $Function:  sat_TEXT_to_ASCIIZ

 $Description:   This routine converts a MFW SAT text descriptor into a
         conventional ASCIIZ string
 $Returns:    none.

 $Arguments:

*******************************************************************************/
void sat_TEXT_to_ASCIIZ (char * destination, T_SAT_TXT * txt)
{
    UBYTE * temp_buffer;
    char* txt_start;

    TRACE_FUNCTION("sat_TEXT_to_ASCIIZ");

    TRACE_EVENT_P1("T_SAT_TXT txt->code=%x", txt->code);

    switch (txt->code)
    {
    case MFW_DCS_7bits:
  temp_buffer = (UBYTE *)ALLOC_MEMORY(((txt->len*8)/7)+1);
  utl_cvt7To8 ((UBYTE *)txt + txt->text, txt->len, temp_buffer,0);
  
    //Sandip CQ 16292 . commented 
  /*sat_gsm_to_ascii (destination, (char *)temp_buffer, len);*/
    /*Sandip CQ 16292 . added this new function . Problem with current TI GSM 7-bit default function*/
 DecodeGSM7ToASCII((unsigned char*)txt+txt->text, (unsigned char*) destination, txt->len);
  FREE_MEMORY ((U8 *)temp_buffer, ((txt->len*8)/7)+1);
  break;
    case MFW_DCS_8bits:
     /* SPR#1700 - DS - Modified to handle 0x80 style unicode */
    txt_start = (char*)txt + txt->text;
    if ((*txt_start == (char)0x80) && (txt->len & 1))
    {
    	TRACE_EVENT("0x80 style unicode");
        memset(destination,0,txt->len+2);
        memcpy(destination,txt_start+1,txt->len-1);
        txt->code = MFW_DCS_UCS2;
    }
    else
    {
        sat_gsm_to_ascii (destination, (char *)txt + txt->text, (UBYTE)txt->len);
    }
  break;
    case MFW_DCS_UCS2:
        sat_ucode_to_ascii (destination, (wchar_t *)((char *)txt + txt->text), txt->len);
  break;
    default:
      sat_gsm_to_ascii (destination, (char *)txt + txt->text, (UBYTE)txt->len); /* SH - got txt->code=244, so need generic response to this*/
    TRACE_EVENT("sat_TEXT_to_ASCIIZ() unexp. DCS");
    }
}

//sandip 16292
// Converts a given 8-bit encoded string into original 7-bit GSM data string according to TS 31.102
/*******************************************************************************

 $Function:  DecodeGSM7ToASCII

 $Description:   This function decodes a GSM 7-bit string into ASCII equivalent

 $Returns:    length of decoded string

 $Arguments: encoded - GSM string pointer
 		       plain      - Buffer to hold the decoded ASCII string
 		       Length   -  Length of the encoded GSM string 

				
 *******************************************************************************/
unsigned char DecodeGSM7ToASCII(U8 * encoded, U8 * plain, U8 Length)
{
	U8 bits, i, j;
	U8 c;

	/* Extract the ascii characters from the bytes,
	*/

	for (i = j = bits = 0; j < Length; i++, j++)
	{
		if (bits > 0)
			c = encoded[i-1] >> (8-bits);
		else
			c=0;

		if (bits < 7)
			c |= encoded[i] << bits;

		*plain++ = c & 0x7f;
		bits = (bits + 1)%8;  /* x0039928 - Lint warning fix */
		if (bits == 0)
		{
			i -= 1;
			Length++;
		}
	}

	*plain = '\0';
	return (Length);
}
/*******************************************************************************

 $Function:  sat_create_TEXT_ASCIIZ

 $Description:   This routine creates out of an MFW SAT text descriptor an
        dynamically allocated ASCIIZ string pointer

 $Returns:    none.

 $Arguments:

				MC, Note: text returned by this function has no Unicode tag at beginning
 $Returns:		none.

 $Arguments:

*******************************************************************************/
/*MC, SPR 940/2 uncommented function back in, and then rewrote it :)*/
void sat_TEXT_to_UCODE (char*  destination, int size, T_SAT_TXT * txt)
{

    UBYTE * temp_buffer;
    
    char debug[50];


	TRACE_EVENT("sat_TEXT_to_UCODE()");

    switch (txt->code) /*DCS of Text*/
    {
    case MFW_DCS_7bits:
	temp_buffer = (UBYTE *)ALLOC_MEMORY(((txt->len*8)/7)+1);
			/*convert from 7 to 8 bit*/
	utl_cvt7To8 ((UBYTE *)txt + txt->text, txt->len, temp_buffer, 0);

        /*MC, convert from 8 bit to UCS2*/
    /*SPR2175, use new function for conversion*/
    ATB_convert_String((char*)temp_buffer, MFW_DCS_8bits, (((txt->len*8)/7)+1), (char*)destination, MFW_DCS_UCS2, /*txt->len*2*/size, FALSE);
	FREE_MEMORY ((U8 *)temp_buffer, ((txt->len*8)/7)+1);
	break;
    case MFW_DCS_8bits:/*DCS tends to come out as 8 bit whatever it is*/
    if (*((UBYTE*)txt+txt->text) !=0x80)/*MC, if ASCII, convert to Unicode*/
    {
#ifdef SAT_TEXT_TRACING
    	//string_GSM_to_UCS2((UBYTE)txt->len, (UBYTE *)txt + txt->text, (USHORT)(txt->len), destination);
    	sprintf(debug, "Length of MENU HEADER:%d", txt->len);
    	TRACE_EVENT(debug);
#endif
		/*MC, convert from 8 bit to UCS2*/
		/*SPR2175, use new function for conversion*/
        ATB_convert_String((char *)txt +txt->text, MFW_DCS_8bits, txt->len, (char*)destination, MFW_DCS_UCS2, /*txt->len*2*/size, FALSE);
    }
    else
    {
		/*MC, unicode string, convert to our display format*/
    	destination[0] = (char)0x80;/*SPR 1728, add unicode tag*/
    	destination[1] = (char)0x7f;

    	memcpy(&destination[2], (UBYTE*)txt+txt->text+1, txt->len-1);

    }
	break;
    case MFW_DCS_UCS2:/*MC, unicode string, convert to our display format*/
	{	destination[0] = (char)0x80;/*SPR 1728, add unicode tag*/
    	destination[1] = (char)0x7f;

    	//memcpy(&destination[2], (UBYTE*)txt+txt->text+1, txt->len-1);
    	memcpy(&destination[2], (UBYTE*)txt+txt->text, txt->len); /* SPR#2340 - DS - Corrected len */

    }

	break;
    default:
	TRACE_EVENT("sat_TEXT_to_UCODE() unexp. DCS");
    }
    /*MC, SPR 1086 Commented out traces here, as long buffers cause a crash*/
    {int i;

    	for (i=0; i <size && i<20; i++)
    	{	if (destination[i] == 0)
    			debug[i] ='0';
    		else
    			debug[i] = destination[i];
    	}
	debug[i+1] = 0x00; /* SPR#2321 - DS - Terminate debug buffer after UCS2 string */
    	TRACE_EVENT(debug);
    }
}
/*JVJE*/

/*******************************************************************************

 $Function:  sat_create_TEXT_ASCIIZ

 $Description:	 This routine creates out of an MFW SAT text descriptor an
				dynamically allocated ASCIIZ string pointer

 $Returns:		none.

 $Arguments:

*******************************************************************************/
#define SAT_LEN_SIZE (U16)sizeof(U16)         /* used to hold the allocation size */
#define SAT_TERMINATOR_SIZE (U16)sizeof(U16)  /* used to hold terminating '\0' in U8 and U16 */

char * sat_create_TEXT_ASCIIZ (T_SAT_TXT * txt)
{

    char * res;
    U16 size;
	UBYTE first_char= *((UBYTE*)txt+txt->text);/*SPR 1728*/
    TRACE_EVENT("sat_create_TEXT_ASCIIZ ()");
#ifdef  NO_ASCIIZ
	TRACE_EVENT_P3("DCS:%d  first_char:%d len: %d", txt->code, first_char, txt->len);
	/*SPR 1728, if unicode string format it for display*/
      if ( first_char==0x80 ||txt->code==MFW_DCS_UCS2)
    {	return (char*)sat_create_TEXT_UCODE (txt);}
#endif

    /* SPR#2321 - DS - Format not Unicode therefore return NULL if string length is zero */
    if (txt->len == 0)
	return NULL;

    /* SPR#2321 - DS - SAT has sent a NULL string. Set first char to ASCII code for <space> */
    if (txt->len == 1 && first_char == 0x00)
    {
	*((UBYTE*)txt+txt->text) = 0x20;
    }

    /* SPR#2321 - DS - Check if SAT has supplied a NULL string (first_char 0x00, dcs not UCS2) */
    if (first_char == 0x00 && txt->code != MFW_DCS_UCS2)
    	return NULL;

    /* allocate sufficient space for the converted string */
    if (txt->code == MFW_DCS_7bits)
	size = SAT_LEN_SIZE + ((txt->len*8)/7)*sizeof(char) + SAT_TERMINATOR_SIZE;
    else
	size = SAT_LEN_SIZE + txt->len*sizeof(char) + SAT_TERMINATOR_SIZE;
    res = (char *)ALLOC_MEMORY (size);
    *(U16 *) res = size; /* store allocated size in first 2 bytes (for deallocation purposes) */
    res += SAT_LEN_SIZE,  /* set the pointer to the string, not to the alloc size */

    /* convert the string */
    sat_TEXT_to_ASCIIZ (res, txt);


    return res;
}

/*******************************************************************************

 $Function:  sat_create_TEXT_UCODE

 $Description:	 This routine creates out of an MFW SAT text descriptor an
				 dynamically allocated UCS2 string pointer

 $Returns:		none.

 $Arguments:

*******************************************************************************/
/*JVJE*/
/*MC, SPR 940/2 uncommneted and then re-written function, pointers to wide chars now are pointers to chars*/
char* sat_create_TEXT_UCODE (T_SAT_TXT * txt)
{
    char * res;
    U16 size;

    TRACE_EVENT("sat_create_TEXT_UCODE()");

    if (txt->code == MFW_DCS_7bits)					/*MC SPR 1086, add 2 extra chars in case UCS2 tag is to be added later*/
        size = SAT_LEN_SIZE + sizeof(char) + ((txt->len*8)/7) * /*sizeof(wchar_t)*/2+SAT_TERMINATOR_SIZE*2;
    else											/*MC SPR 1086, add 2 extra chars in case UCS2 tag is to be added later*/
        //size = SAT_LEN_SIZE + sizeof(char) + txt->len * /*sizeof(wchar_t)*/2+SAT_TERMINATOR_SIZE*2;
        size = SAT_LEN_SIZE + (sizeof(U16) * txt->len) + SAT_TERMINATOR_SIZE*2; /* SPR#2321 - DS - Improve clarity of mem alloc */
    res = (char *)ALLOC_MEMORY (size);
    *(U16 *)res = size;
    res = ((char *)res + SAT_LEN_SIZE);

    TRACE_EVENT_P1("size: %d", size);

    sat_TEXT_to_UCODE (res,size,  txt);

    return res;
}


/*******************************************************************************

 $Function:  sat_create_ITEM_ASCIIZ

 $Description:	 This routine converts a MFW SAT Item descriptor into a
				conventional String pointer
				MC, note: this function will add a Unicode tag to the beginning of Unicode
				strings returned

 $Returns:		none.

 $Arguments:

*******************************************************************************/
/*MC, SPR 940/2 changed this func to output Unicode string when appropriate*/
char * sat_create_ITEM_ASCIIZ (SatItem * item)
{
    char * res;
    U16 size;
    UBYTE first_char= *((UBYTE*)item+item->text);

    TRACE_FUNCTION("sat_create_ITEM_ASCIIZ()");

    TRACE_EVENT_P2("first_char:%d len: %d", first_char, item->len);

    /* SPR#2321 - DS - Return NULL if string length is zero */
    if (item->len == 0)
    {
	//return NULL;//sandip-CQ 16306
	*((UBYTE*)item+item->text) = 0x20;
    }

    /* SPR#2321 - DS - SAT has sent a NULL string. Set first char to ASCII code for <space> */
    if ((item->len == 1) && (first_char == 0x00))
    {
        *((UBYTE*)item+item->text) = 0x20;
    }

    /* allocate sufficient space for the converted string */
    size = SAT_LEN_SIZE + item->len/**sizeof(wchar_t)*/*2 + 3*SAT_TERMINATOR_SIZE;
    res = (char *)ALLOC_MEMORY (size);
    *(U16 *) res = size; /* store allocated size in first 2 bytes (for deallocation purposes) */
    res += SAT_LEN_SIZE; /* set the pointer to the string, not to the alloc size */

    /*
     * item->text is offset of string from start of item
     */
#ifdef NO_ASCIIZ /*MC SPR 940/2 check if we're in Chinese or string is unicode and deal with it*/

     if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)/*if chinese*/
     	{
#ifdef SAT_TEXT_TRACING
		 /***************************Go-lite Optimization changes Start***********************/
		 //Aug 16, 2004    REF: CRR 24323   Deepa M.D
		 TRACE_EVENT_P1("Length of menu item:%d", item->len);
		 /***************************Go-lite Optimization changes end***********************/
#endif
     		if (*((char*)item+item->text) != (char)0x80)/*MC, if item ASCII*/
     		{ //	int i;  // RAVI
				/*convert to unicode*/
     			TRACE_EVENT("Adding menu item in ASCII->UCS2");
     	//		SmsRead_convertSMSmsg((char *)item + item->text, MFW_DCS_8bits, item->len, &res[2], MFW_DCS_UCS2, size-4, FALSE);
		//		res[0] = 0x80;/*add tag to beginning of string*/
		//		res[1] =0x7F;
			/*MC, SPR 1086 8 bit to UCS2 conversion causes menu update problems*/
     		/*Thought it might be due to memory leaks/overwrites, but can't find any*/
     		/*Easy solution is to convert to ASCII rather than UCS2*/
			sat_gsm_to_ascii (res, (char *)item + item->text, item->len);
     		}
     		else /*if Unicode tag*/
     		{	TRACE_EVENT("Adding menu item in UCS2");
     			/*Shift string up one byte*/
    			memcpy(res+2, (char *)item + item->text+1, item->len-1);
     			res[0] = (char)0x80;/*add tag at beginning of string*/
     			res[1] = (char)0x7f;
     		}
     	}
     else /*if not chinese BUT unicode tag, convert to display Unicode format*/
     {	 if (*((char*)item+item->text) == (char)0x80)
     		{	memcpy(res+2, (char *)item + item->text+1, item->len-1);
     			res[0] = (char)0x80;
     			res[1] = (char)0x7f;
     		}

     	else /*if not chinese and ascii string, convert from GSM to ASCII*/
#endif
    		sat_gsm_to_ascii (res, (char *)item + item->text, item->len);
#ifdef NO_ASCIIZ
     }
#endif


    return res;
}

/*******************************************************************************

 $Function:  sat_destroy_TEXT_ASCIIZ

 $Description:	 destroy an ASCIIZ string previously created with
				 sat_create_TEXT_ASCIIZ() or sat_create_ITEM_ASCIIZ()

 $Returns:		none.

 $Arguments:

*******************************************************************************/
void sat_destroy_TEXT_ASCIIZ (char * str)
{
/*MC, SPR 940/2 if we're in chinese, use Unicode string destuctor*/
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{	sat_destroy_TEXT_UCODE(str);
		return;
	}

  TRACE_FUNCTION("sat_destroy_TEXT_ASCIIZ");

    if (str NEQ NULL)
        {
            /* the two bytes before the string are holding the alloc size info */
            FREE_MEMORY ((U8 *)(str-SAT_LEN_SIZE), *(U16 *) (str-SAT_LEN_SIZE));
            str = NULL;
        }
}

#ifdef MMI_TEST_SAT_LAUNCH_BROWSER
void mmiOpenDummyBrowser(T_MFW_HND win,char* information)
{
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA	editor_data;
#endif

	TRACE_FUNCTION("mmiOpenDummyBrowser");

	/* SPR#1428 - SH - New Editor changes */

#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetTextStr(&editor_data, TxtNull, TxtNull, TxtNull, NULL);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)information, INFORMATION_SIZE);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);

	AUI_edit_Start(win, &editor_data);
#else /* NEW_EDITOR */

#ifdef DBG1
	editor_data_init();
#endif
	editor_data.TextString			= "WAP";
	editor_data.LeftSoftKey			= TxtSoftOK;
	editor_data.RightSoftKey		= NULL;
	editor_data.AlternateLeftSoftKey	= NULL;
	editor_data.Callback			= NULL;
	editor_data.Identifier			= 0;
	editor_data.hide				= FALSE;
	editor_data.mode				= READ_ONLY_MODE;
	editor_data.timeout           	= FOREVER;
	editor_data.min_enter			= 0;
	editor_data.destroyEditor		= TRUE;
	editor_data.editor_attr.win.px	= 0;
	editor_data.editor_attr.win.py	= 8;
	editor_data.editor_attr.win.sx	= 84;
	editor_data.editor_attr.win.sy	= 24;
	editor_data.editor_attr.font	= 0;
	editor_data.editor_attr.mode	= edtCurNone;
	editor_data.editor_attr.controls	= 0;
    editor_data.editor_attr.size	= INFORMATION_SIZE;

	editor_data.editor_attr.text = information;

	/* create the dialog handler */
	editor_start(win, &editor_data);  /* start the editor */
#endif

	return;
}

#endif
//GW SPR#1035 - Added SAT changes
/*******************************************************************************

 $Function:  sat_release_the_editor

 $Description: The SAT-application initiate a setup call and
             the MMI opens for this call status a new editor.
             Normally the event "SAT_SESSION_END" release the editor
             but for some reason this event doesnt come up !
             This function allows from the MMICall.c in the case of "disconnect"
             to release the editor as well.

 $Returns:		none.

 $Arguments:

*******************************************************************************/
void sat_release_the_editor (void)
{
      TRACE_FUNCTION("sat_release_the_editor()");

	if (g_sim_toolkit_data->info_win NEQ NULL)
	{
		/* destroy any infoscreen if exist */
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		SEND_EVENT (g_sim_toolkit_data->info_win, E_ED_DEINIT, 0, NULL);
#else /* NEW_EDITOR */
		SEND_EVENT (g_sim_toolkit_data->info_win, E_EDITOR_DEINIT, 0, NULL);
#endif /* NEW_EDITOR */
		g_sim_toolkit_data->info_win = NULL;
	}
}

/*MC, SPR 940/2 uncommented function and changed param to char*  */
void sat_destroy_TEXT_UCODE (/*cp_wstring_t*/char* str)
{
    if (str NEQ NULL)
        {
            FREE_MEMORY ((U8 *)((char *)str-SAT_LEN_SIZE), *(U16 *) ((char *)str-SAT_LEN_SIZE));
            str = NULL;
        }
}
/*******************************************************************************

 $Function:  sat_add_unicode_tag_if_needed

 $Description: Checks if string ought to be pre-pended with a unicode tag and if it ought to,
 				adds the tag (note, the string has to already have 2 "extra" chars allocated
 				to it; this normally happens in the create_TEXT_UCODE function)

 $Returns:		none.

 $Arguments:	string

*******************************************************************************/
/*SPR1257, new function*/
void sat_add_unicode_tag_if_needed(char* string)
{/*Add tag to Unicode strings so info dialogue displays them correctly*/
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE&& string[0]!= (char)0x80 )
			{
				char TempString[MAX_MSG_LEN_SGL];/*MC, SPR 1292, more sensible length*/
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
				memcpy(&TempString[2], GET_TEXT(TxtSoftCall), ATB_string_UCLength((USHORT*)string)*sizeof(USHORT));
#else /* NEW_EDITOR */
				memcpy(&TempString[2],  string, strlenUnicode((U16*) string));
#endif /* NEW_EDITOR */

				TempString[0] = (char)0x80;
				TempString[1] = (char)0x7f;
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
				memcpy(string, TempString, (ATB_string_UCLength((USHORT*)TempString)+1)*sizeof(USHORT));
#else /* NEW_EDITOR */
				memcpy( string, TempString, strlenUnicode((U16*)TempString));
#endif /* NEW_EDITOR */

			}
}

/* Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
    -Deleted Redial flag enabling/disabling code*/

