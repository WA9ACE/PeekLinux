/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Start
 $File:		    MmiStart.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the start code for the MMI
                        
********************************************************************************
 $History: MmiStart.c

	Jan 05, 2007 OMAPS00084665  x0039928(sumanth)
 	Description: Boot time exceed UCSD estimations
 	
 	xashmic 21 Sep 2006, OMAPS00095831
 	Description: USB enumeration does not happen in PIN/PUK/Insert SIM scenarios
 	Solution: When booting up without SIM card, or prompting for PIN/PUK/SIMP entry, 
 	enumerate all the USB ports automatically 

       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation
       Solution     : As part of the implemention, in this file, divert status is got immediately after boot-up
       
	Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
   	Description: Need to reduce flash foot-print for Locosto Lite 
   	Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
   	out voice memo feature if the above flag is enabled.

 	Oct 11, 2005	REF: LOCOSTO-SPR-34629	-	xpradipg
 	Description:	Temporary removal of animation logo and replacing with 
 				static image
 	Solution:		The Animation at bootup is removed and replaced with a static
 				image also the display of logo is done before initiating the 
 				AT_CMD_CFUN full functionality

    	Jun 23, 2004        	REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
  	Solution:			The CF state is maintained in flash. During bootup the CF icon in the idle screen
  					 is displayed depending upon its state in the ffs

	25/10/00			Original Condat(UK) BMI version.	
	
	Dec 23, 2005 REF: SR13873 - x0020906
	Description : Store the FFS value into current sound structure
	Fix : Call setDefaultSound () to assign FFS value into current sound structure.	
	   
 $End

*******************************************************************************/




/*******************************************************************************
                                                                              
                                Include files
                                                                              
*******************************************************************************/
#define ENTITY_MFW

//#define CO_UDP_IP temp till xml file ready

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
#include "pwr.h" /*OMAPS00091029 x0039928(sumanth)*/

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"

#include "mfw_lng.h"
#include "mfw_nm.h"
#include "mfw_icn.h"
#include "mfw_sim.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_tim.h"
#include "mfw_mme.h" 	// SH - so we can call mmeInit in show_welcome_cb

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiCall.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiIdle.h"
#include "Mmiicons.h"
#include "MmiSettings.h"
#include "MmiMenu.h"
#include "MmiNetwork.h"
#include "MmiServices.h"
#include "MmiBookController.h"
#include "MmiBookServices.h"
#include "MmiNetwork.h"
#include "MmiSimToolkit.h"

#ifndef FF_NO_VOICE_MEMO
#include "MmiVoiceMemo.h"
#endif

#include "MmiCPHS.h"
#include "mmiSmsIdle.h"
#include "mmiSmsMenu.h"

/* Set Default FFS value to sound structure - RAVI - 23-12-2005 */
#ifdef NEPTUNE_BOARD
#include "MmiSounds.h"
#endif
/* END RAVI */

#ifdef BTE_MOBILE
#include "mmiBluetooth.h"
#endif

#ifdef MMI_GPRS_ENABLED
#include "MmiGprs.h"
#endif /* GPRS */

/* SPR759 - SH*/
#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"
#endif

/* SPR1983 - SH */
#ifdef FF_WAP
#include "MmiWapFfs.h"

/* SPR2086 - SH */
#include "AUIWapext.h"
#endif

#ifndef NEPTUNE_BOARD
#ifdef FF_EM_MODE
#include "MmiEm.h"
#endif 
#else
#ifdef TI_PS_FF_EM
#include "MmiEm.h"
#endif
#endif

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

#ifndef MFW_EVENT_PASSED
#define MFW_EVENT_PASSED 0
#endif

// Jun 23 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
#include "mfw_ffs.h"

#ifdef FF_MMI_MULTIMEDIA
#include "mfw_MultimediaApp.h"
#endif


extern void sms_cb_init (void);

/*******************************************************************************
                                                                              
                                Static data
                                                                              
*******************************************************************************/

static MfwHnd win;
static MfwHnd kbd;

/* The following information controls the welcome animation
*/
#define WELCOME     1
#define NO_WELCOME  0

/* static MfwHnd win_animation;  x0039928 - Lint warning fix */
/* static USHORT animation_index; x0039928 - Lint warning fix */
/* static U8 welcome_status; x0039928 - Lint warning fix */
/* SPR877 - SH - set to TRUE if welcome animation has finished */
UBYTE animation_complete = FALSE;
									   
/*******************************************************************************
                                                                              
                                Local prototypes
                                                                              
*******************************************************************************/

static int winEvent( MfwEvt e, MfwWin *w );
static int keyEvent( MfwEvt e, MfwKbd *k );


static void showDefault( void );

/* SH - moved showwelcome primitive to header file */
static int show_welcome_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);
// 	Oct 11, 2005	REF: LOCOSTO-SPR-34629	-	xpradipg
#ifdef TI_PS_UICC_CHIPSET_15
//extern unsigned char TIlogColour[];
#endif
/*******************************************************************************
                                                                              
                                Private methods
                                                                              
*******************************************************************************/

#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:    	winEvent

 $Description:	Window event handler, handles window visible events and
				ignores all others
 
 $Returns:		MFW_EVENT_CONSUMED or MFW_EVENT_PASSED

 $Arguments:	e, event to be handles, w, window handle
 
*******************************************************************************/

static int winEvent( MfwEvt e, MfwWin *w )
{
	TRACE_FUNCTION("WinEvent(Start)");
    switch( e )
    {
        case MfwWinVisible:
		{
            if (w->flags & MfwWinVisible)
                show();
		}
        break;

        default:
		{
            return MFW_EVENT_PASSED;
		}
    }

    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	winanimEvent

 $Description:	Handle events during the window animation

 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

static int winanimEvent( MfwEvt e, MfwWin *w )
{

	TRACE_FUNCTION("winanimEvent");

    switch( e )
    {
        case MfwWinVisible:
	        dspl_ClearAll();
			
		break;
        default:
		{
            return MFW_EVENT_PASSED;
		}
    }

	return MFW_EVENT_CONSUMED;
}

#endif

/*******************************************************************************

 $Function:    	startregis

 $Description:	All we need to do here is start the SIM

 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

static void startregis( void )
{
	TRACE_FUNCTION("start registration");
// 	Oct 11, 2005	REF: LOCOSTO-SPR-34629	-	xpradipg
//	the static image is displayed just before intiating the full functionality
#ifdef TI_PS_UICC_CHIPSET_15
//	dspl_BitBlt2(WELCOME_ANI_POS_X, WELCOME_ANI_POS_Y,176,60,(char *)&TIlogColour[0],0,ICON_TYPE_256_COL);
#endif
#ifndef EMO_SIM
	sim_activate();	
#endif
	


}

/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	startInit

 $Description:	Initialises the MMI startup functions

 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

void startInit( void )
{
	//void UIInit();

#ifdef FF_MMI_MMS
    /* TISHMMS Project */
    extern void mms_wap_data_init(void);
    extern  void mms_mms_data_init(void);
#endif

//    T_MFW_HND hnd;   // RAVI

    /* Assume the welcome status is welcome
    */
    /* welcome_status = WELCOME;  x0039928 - Lint warning fix */
    
	animation_complete = FALSE;

	/* unset the flag for ready state at the beginning*/
	smsidle_unset_ready_state();

    /* setup signalling and AUTOFOCUS
    */
    mfwSetSignallingMethod( 1 );
    winAutoFocus( TRUE );

    /* API/GW - Set Colour of displays */
    colour_initial_colours();

	/* GW-SPR#1035-Added SAT changes by Nebi (changed initialisation order). */
	/* initialise network services */
	simToolkitInit( 0 );
	bookInitialise( 0 );
	callInit( 0 );
	iconsInit();

#ifdef BTE_MOBILE
	mmi_bt_init();
#endif
	
	/* Handlers created on windows */
	pin_init( 0 );
	networkInit( 0 );	
	smsidle_start(0, NULL); 
	servicesInit( 0 );

	idleInit( 0 );  

#ifndef FF_NO_VOICE_MEMO
	voice_memo_init ();
#endif
/*
	voice_mail_init ();
	sms_cb_init ();
	
	mmi_cphs_init();	
*/
	/* SPR#2346 - SH - GPRS */
#ifndef EMO_SIM
#ifdef MMI_GPRS_ENABLED
	GPRS_Init();
#endif /* GPRS */
#endif

#ifndef NEPTUNE_BOARD
#ifndef EMO_SIM
#ifdef FF_EM_MODE
	mmi_em_init();/*MC SPR1209 engineering mode*/
#endif
#endif
        /* SPR#2086 - SH - Startup WAP */
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
        AUI_wap_init();
#endif

#ifdef FF_MMI_MMS    
            /* TISHMMS Project */
            /*init mms wap profile*/
            mms_wap_data_init();
            mms_mms_data_init();
#endif
#else /* NEPTUNE_BOARD */
/* NEPTUNE: Sumit*/
#if 0
	/* SPR#2346 - SH - GPRS */
#ifdef MMI_GPRS_ENABLED
	GPRS_Init();
#endif /* GPRS */

#ifdef TI_PS_FF_EM
	mmi_em_init();/*MC SPR1209 engineering mode*/
#endif

        /* SPR#2086 - SH - Startup WAP */
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
        AUI_wap_init();
#endif

#ifdef FF_MMI_MMS    
            /* TISHMMS Project */
            /*init mms wap profile*/
            mms_wap_data_init();
            mms_mms_data_init();
#endif
#endif /* if 0 */
#endif /* NEPTUNE_BOARD */

/* Set FFS value into sound structure - RAVI - 21-12-2005 */
#ifdef NEPTUNE_BOARD
	setDefaultSound();
#endif
/* END RAVI - 21-12-2005 */

/* Retrive the configurable ring tone stored */
#ifdef NEPTUNE_BOARD
#ifdef FF_MMI_MULTIMEDIA
       Store_Default_Parameter ();
       setDefaultMSLTone ();
#endif
#endif
/* End */
	//xashmic 21 Sep 2006, OMAPS00095831
	//During bootup, PIN, PUK, SIMP, Insert SIM stages, 
	//Audio module, USB module needs to be initialized for
	//Emergency call , USB enumeration respectively
#ifndef EMO_SIM
	mmeInit();  //fieldstrength driver
#endif
	/* SH - PIN screen now comes before welcome screen */
	showwelcome(idle_get_window());

        while(animation_complete == FALSE)
                TCCE_Task_Sleep(1);

	//UIInit();

	startregis();	
}

/*******************************************************************************

 $Function:    	startExit

 $Description:	Stops the MMI start windows, basically reverses the effect
				of the startInit above

 $Returns:		None.

 $Arguments:	None.
 
*******************************************************************************/

void startExit( void )
{
	/* remove keyboard handlers
	*/
    kbdDelete(kbd);
	kbd = 0;

	/* SPR#2346 - SH - GPRS */
	#ifdef MMI_GPRS_ENABLED
    GPRS_Exit();
	#endif /* GPRS */
	
	
	/* shut down the sim toolkit
	*/
    simToolkitExit();

	/* close the phonebook, calls, idle, services, icons, sounds
	   and network handlers
	*/
	bookExit();

#ifdef BTE_MOBILE
	mmi_bt_exit();
#endif

	callExit();
	idleExit();
	smsidle_destroy ();
    iconsExit();
    cphs_exit(); //MC
#ifdef NEPTUNE_BOARD
#ifdef TI_PS_FF_EM
    mmi_em_exit();/*MC, SPR1209 engineering mode */
#endif
#else
#ifdef FF_EM_MODE
    mmi_em_exit();/*MC, SPR1209 engineering mode */
#endif
#endif
}

/*******************************************************************************

 $Function:    	startExec

 $Description:	start window execute method

 $Returns:		None.

 $Arguments:	reason, for invoking the execute method,
				next, not used passed for compatibility
 
*******************************************************************************/

void startExec( int reason, MmiState next )
{

    switch (reason)
    {
		case PhoneInit:
		{
		    #ifndef EMO_SIM
		    sim_init();                     /* init SIM handler         */
		    #endif
		    nm_init();                      /* init REG handler         */
	    	    startInit();                        /* init startup module      */
	    	
		}
        	break;

		case FirstTimeNMFound:
		{
			/* first time through setup window and idle handlers
			*/
			TRACE_EVENT("NM Found");

			winFocus( win );
			winShow( win );
			idleExec( IdleNormal, 0 );
			
		}
		break;

		case NmSearching:
		{
			/* just trace on the search event for now
			*/
			TRACE_EVENT("Search");
			
		}
		break;

		default:
		{
			/* no default handling
			*/
			break;
		}
    }
}

/*******************************************************************************

 $Function:    	startWhoIsFocused

 $Description:	provides handle of currently focussed window

 $Returns:		mfwFocus

 $Arguments:	None.
 
*******************************************************************************/

MfwHnd startWhoIsFocused( void )
{
    return( mfwFocus );
}

/*******************************************************************************

 $Function:    	showwelcome

 $Description:	 shows a welcome Message when swiching on the mobile
 
 $Returns:		none

 $Arguments:	
 				
*******************************************************************************/

void showwelcome (T_MFW_HND win)			/* SH - not static, as now externally called */
{
        T_DISPLAY_DATA   display_info;
	T_MFW_HND       win_parent  = mfw_parent (mfw_header());


	TRACE_FUNCTION("showwelcome ()");


    idle_set_starting_up(FALSE); /*SPR#1662-NH Starting is finished for Idle */
// 	Oct 11, 2005	REF: LOCOSTO-SPR-34629	-	xpradipg
//	Since the static image is displayed at bootup and it is preponed, this 
//	function will be called after the sim insert indication is recieved hence
//	we go ahead with the display of the Idle screen by calling show_welcome_cb

 boot_time_snapshot(EAppInit); /*OMAPS00091029 x0039928(sumanth) - to mark the end of app init*/
 TRACE_EVENT("Boot Time Snapshot - EAppInit");
//#ifdef TI_PS_UICC_CHIPSET_15
//	show_welcome_cb(win,0,0);
//#else
//{
	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNull, TxtNull , COLOUR_STATUS_WELCOME);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)show_welcome_cb, FOUR_SECS, 0 );
	
      /*
       * Call Icon
       */
	mmi_dialogs_insert_animation (info_dialog (win, &display_info), 400 ,(MfwIcnAttr*)&welcome_Attr,animwelcome);
//}
//#endif 


}

/*******************************************************************************

 $Function:    	show_welcome_cb

 $Description:	 shows a welcome Message when swiching on the mobile
 
 $Returns:		none

 $Arguments:	
 				
*******************************************************************************/

int show_welcome_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
	TRACE_FUNCTION("show_welcome_cb");

   	/* SH - removed call to startregis, now in startInit */
	//startregis();
    /* SPR759 - SH. Initialise homezone */
#ifdef MMI_HOMEZONE_ENABLED
   	homezoneInit();
#endif /* HOMEZONE */

	/* SPR#1983 - SH - Initialise flash files for WAP */

#if defined (FF_WAP) && defined (CO_UDP_IP)
	flash_wap_init();
#endif /* WAP */

	/* SPR#1112 - SH - Check which phonebook is active.  If FFS is formatted and
	 * FFS phonebook is selected, initialise it. */
#ifndef NEPTUNE_BOARD
#ifdef INT_PHONEBOOK
	bookInitInternal();
#endif
#else
#ifdef INT_PHONEBOOK
	// bookInitInternal(); //SUMIT: Neptune: removed call to bookInitInternal for FFS
#endif
#endif

	/* SPR#1352 - SH - Initialise TTY */
#ifdef MMI_TTY_ENABLED
	call_tty_init();
#endif
	/* end SH */

	animation_complete = TRUE;

    /* SPR#1929 - DS - Check if there is voicemail waiting. Port of 1960 */
	/*a0393213 cphs rel4*/
#if 0
   #ifdef FF_CPHS_REL4    
   if(!isMWISFilePresent())
   #endif	
    GetCphsVoicemailStatus(0);
				
	/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
           Description : CPHS Call Forwarding feature implementation
           Solution     : As part of the implemention, divert status is got during boot-up.
                             The existing code is also maintained*/
   #ifdef FF_CPHS
   /*a0393213 cphs rel4*/
   /* call cphs_get_divert_status() only when EF(CFIS) is not present. when EF(CFIS) is present, the call to
       cphs_get_divert_status() should be made only after fetching MSP information from network*/
   #ifdef FF_CPHS_REL4    
   if(!isCFISFilePresent())
   #endif	
	cphs_get_divert_status ();
   #else
	   //  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
	//        Setting the CF icon state depending on the value in flash. 					
		if(FFS_flashData.CFState==TRUE)
		{
			iconsSetState(iconIdCallFor);
		}
		else
		{
			iconsDeleteState(iconIdCallFor);
		}
    #endif
#endif
    idleEvent(IdleUpdate);
	
	return 1;  // RAVI - Changed Return to Return 1
}

/* GW return when animation is complete (and the idle screen can be displayed*/
int mmiStart_animationComplete( void )
{
	return (animation_complete);
}


#ifdef NEW_KEY_INPUT
#include "kpd_cfg.h"
#include "mfw_kbd.h"
#define MAX_KEY 255
U32 mfw_kbd_map [MAX_KEY] = 
{
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_STAR,
	
};
	
void mfw_keyMap(int mfw_key, int hw_key)
{
	if ((hw_key >0) && (hw_key<MAX_KEY))
		mfw_kbd_map[hw_key] = mfw_key;
}

void start_setKeys( void )
{	
	mfw_keyMap(KEY_0,		KPD_KEY_0);
	mfw_keyMap(KEY_1,		KPD_KEY_1);
	mfw_keyMap(KEY_2,		KPD_KEY_2);
	mfw_keyMap(KEY_3,		KPD_KEY_3);
	mfw_keyMap(KEY_4,		KPD_KEY_4);
	mfw_keyMap(KEY_5,		KPD_KEY_5);
	mfw_keyMap(KEY_6,		KPD_KEY_6);
	mfw_keyMap(KEY_7,		KPD_KEY_7);
	mfw_keyMap(KEY_8,		KPD_KEY_8);
	mfw_keyMap(KEY_9,		KPD_KEY_9);
	mfw_keyMap(KEY_STAR,	KPD_KEY_STAR);
	mfw_keyMap(KEY_HASH,	KPD_KEY_DIESE);
	mfw_keyMap(KEY_LEFT,	KPD_KEY_SOFT_LEFT);
	mfw_keyMap(KEY_RIGHT,	KPD_KEY_SOFT_RIGHT);
	mfw_keyMap(KEY_CALL,	KPD_KEY_CONNECT);
	mfw_keyMap(KEY_HUP,		KPD_KEY_DISCONNECT);
	mfw_keyMap(KEY_POWER,	KPD_PWR);

	mfw_keyMap(KEY_VOLUP,		KPD_KEY_VOL_UP);
	mfw_keyMap(KEY_VOLDOWN,		KPD_KEY_VOL_DOWN);

	mfw_keyMap(KEY_MNUUP,		KPD_KEY_UP);
	mfw_keyMap(KEY_MNUDOWN,		KPD_KEY_DOWN);
	mfw_keyMap(KEY_MNULEFT,		KPD_KEY_LEFT);
	mfw_keyMap(KEY_MNURIGHT,	KPD_KEY_RIGHT);


}

#endif

/*******************************************************************************
                                                                             
                                End of File
                                                                              
*******************************************************************************/
