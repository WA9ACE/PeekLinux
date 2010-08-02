/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Network
 $File:		    MmiNetwork.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    22/02/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

  

********************************************************************************

 $Hist       

   Oct 09, 2007 OMAPS00148258 x0056422
   Description: Memory leak problem
   Solution: Modified the freed memory size to solve the memory leak problem

   Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)
   Description:HPLMN Manual Selection at power on -unable to perform manual selection at power ON
   Solution: In network_nm_cb(), added a new case E_NM_PLMN_LIST_BOOTUP, to list the PLMNs during bootup.
   May 31, 2007 DR: OMAPS00133201 x0pleela
   Description: MS keeps searching networks and doesn't list available PLMNs after performing 
   			manual selection
   Solution : Added new function nm_abort_network() to destroy the dialog saying "Searching for 
   			Networks" when CME_ERR_AbortedByNetwork error is recieved from ACI

	May 24, 2007  DR: OMAPS00132483 x0pleela
	Description: MM: While playing an AAC, unplug USB freezes the audio
	Solution: In network() function, setting or resetting the phonelock and auto phone lock status bits
				
 	May 09, 2007  DR: OMAPS00129014 x0pleela
	Description: COMBO: unable to send SMS during midi playing
	Solution: Added phone lock status code in network() function.			
 	May 09, 2007  DR: OMAPS00129014
	Description: COMBO: unable to send SMS during midi playing
	Solution: Added phone lock status code in network() function.			
    Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)
    Description: With Orange SIM card, once attached (at+cgatt=1), "5201" is displayed instead of "Orange"
    Solution : In network function, under the NETWORK_FULL_SERVICE, 
    added a contion to check whether plmn_name is present.

    Mar 30,2007  DRT: OMAPS00120157 x00061088(Prachi)
    Description : MMI displays network forbidden
    Solution: Added check for Manual selection when the user selects network
    manually for network list .Now network() checking for for Manual selection 
    by checking data->network_list_status is NETWORK_LIST_DISPLAY_FIRST or not.

 	Feb 16, 2007 OMAPS00098351 x0066814(Geetha)
 	Description: NITZ support is not working
       Solution: Added the FF_TIMEZONE flag.Time is calculated according to GMT, if the FF_TIMEZONE flag is enabled. 
       		Added settingsNITZ function to write the menu option selected onto the Flash.

    Feb 08,2007    DRT: OMAPS00115060 x00061088(Prachi)
    Description : To check for Forbidden network (for denied services),it takes long time.
    after including fix for issue OMAPS00110741.
    Solution: To solve this issue ,condition for forbidden network(i.e. network_data.forbidden == TRUE) 
    is moved out of condition if (parameter != Null).
       


    Jan 10, 2007 REF: OMAPS00110741  x0039928(sumanth)
    Description: Sample resets after network deregistration during Robustness Test
    Solution: A condition is added to check if the parameter passed to the network() is NULL.
    
    Jan 05, 2007 OMAPS00084665  x0039928(sumanth)
    Description: Boot time exceed UCSD estimations
 	
   Dec 13, 2006    DRT: OMAPS00105640 x0039928(Sumanth)
   Description : SIMP: The board reboots when removing SIM card.
   Solution : A condition is added in network() to check if SIM is active. 
   
    Oct 30,2006 ER:OMAPS00091029 x0039928(sumanth)
    Bootup time measurement
    
    Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
    Description:Enhance RSSI to 3 parameters: Strength, quality, min-access-level
    Solution: If there is NO NETWORK , Network() is calling globalSignalUpdate() 
    to show the Signal Strength.
   Sep 18,2006    DRT: OMAPS0090268 x00061088(Prachi)
   Description : To check for Forbidden network (for denied services),it takes long time.
   Solution : Function network_nm_cb() has now be implemented to handle E_NM_FORBIDDEN_NETWORK,which sends event
   NETWORK_FORBIDDEN_NETWORK to network(),by setting flags network_data.aborted  =False 
   and network_data.forbidden = Ture.

	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution:	a) Removed the usage of global variable gEons_alloc_active
			b) Added new check for CNIV request
			c) Memset the data->plmn_name and data->network_name
			d) Added check to display short name if long name len is 0. or display numeric name if short name len is 0
			   	
    June 02, 2006    DR: OMAPS00078005 x0021334
    Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
    Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
    function. This function has now been implemented to pass on the received values to MMI
    for user confirmation and RTC updation.

	May 26, 2006 ER: OMAPS00079607 x0012849 :Jagannatha M
	Description: CPHS: To support AoC and information numbers.
	Solution: Made the following changes
		       Added a function that returns the roaming status.

	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
   			- Included flag NEPTUNE_BOARD to retain old code for operator get name and 
				added new code for this ER also
			- Removed the global variable gOper_sel_done and have introduced 2 functions which will 
				set and get the status of the operator selection

	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: In function network(), check if EONS is allocated and activated. If not, get operator name from other sources
			Removed CPHS check code as prioritized operator name is already updated to network_data
   	
	xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
	Added a please wait screen so that the sim is updated with the new code before 
	populating the list again when deleting or when entering a newtwork code
       
    Nov 02, 2005	REF: omaps00044855   Sumanth Kumar.C
    Description:	Phone crashes on invoking Manual option in Network Selection Menu.
    Solution:		Memory free of network_data.pplmn is done when the editor exits instead of 
    list exit by option selection so that the pplmn data is available when the list is invoked once again.

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.

    Sep 23, 2004        REF: CRR 24562  xvilliva
    Bug:	Enable/disable the manual network selection option depending
			on ENS.
    Fix:	If the manual network selection is disabled we check if an 
			error is returned and flash an alert to the user.

	Sep 8, 2004        REF: CRR 16062  xvilliva
    Bug:	If a PLMN is selected (by using the middle key) from the PLMN list 
    		presented upon a manual search the D-Sample may freeze.
    Fix:	The "KEY_MNUSELECT" event is also monitered which was not
    		done earlier and was resulting in the PLMN list shown again.

    Jul 21, 2004        REF: CRR 13348  xvilliva
    Bug:	After selecting HPLMN manually, the idle screen is not shown.
    Fix:	After the "Network Selected" alert is flashed, we destroy the 
    		phonebook menu to return to the idle screen.


	 //  Apr 14, 2004        REF: CRR 12641  xpradipg
	 //		The network when selected using "Automatic" is not showing an alert after selection.
	 //		A flag has been used to show the alert when ever network is selected using 
	 //		"Automatic" method. As the flag is present it will not flash this alert when the
	 //		device is restarted.
 
	25/10/00			Original Condat(UK) BMI version.	

	05/04/06 REF:OMAPS00061046 x0043641
	BUG:Possible to cancel the searching with right soft key although the option is not available.
	FIX: In the searching mode cancel option is provided.
	
	08-03-2006   REF: OMAPS00061049    x0020906
	
	Bug: The sample crashed many times while trying to camp onto a FPLMN. PCO stucked when tracing this TC, 
	therefore no PCO traces available; only L1 traces (TestHarness).

	Fix: During manual selection from the indication will get the plmn list count and used the count for allocating 
	plmn_list pointer and then destorying the list and free the pointer allocation when particular network is selected
	or when back softkey is pressed.

	   
 $End
 // Issue Number : SPR#12654 on 07/04/04 by Pradeep/Vadiraj
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
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_cm.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "mfw_sms.h"

// June 02, 2006    DR: OMAPS00078005 x0021334
//Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the Flag
#ifdef FF_TIMEZONE
#include "mfw_td.h"
#endif

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif

#include "MmiMenu.h"
#include "MmiIdle.h"
#include "MmiCall.h"
#include "MmiBookController.h"
#include "MmiNetwork.h"
#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiPcm.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

#include "mfw_ffs.h"
#include "MmiCPHS.h"

#ifdef MMI_HOMEZONE_ENABLED
#include "MmiHomezone.h"
#endif

#include "MmiNetwork.h"

#include "cus_aci.h"

#include "mmiColours.h"

/* BEGIN ADD: Neptune Alignment */
#ifdef NEPTUNE_BOARD
#include "bmi_integ_misc.h"
#endif 
/* END ADD: Neptune Alignment */
#ifdef NEPTUNE_BOARD
/* PLMN list count obtained from indication, stored in the global static variable*/
/*OMAPS00061049 - 08-03-2006 */
static int Plmn_list_count = 0;
#endif


/* Enum containing GMT   */
typedef enum { 
MMI_GMT_NT_1200 = -48, /* Read as GMT -12:00 hrs */
MMI_GMT_NT_1145 = -47, /* Read as GMT -11:45 hrs */
MMI_GMT_NT_1130 = -46,
MMI_GMT_NT_1115 = -45,
MMI_GMT_NT_1100 = -44,
MMI_GMT_NT_1045 = -43,
MMI_GMT_NT_1030 = -42,
MMI_GMT_NT_1015 = -41,
MMI_GMT_NT_1000 = -40,
MMI_GMT_NT_0945 = -39,
MMI_GMT_NT_0930 = -38,
MMI_GMT_NT_0915 = -37,
MMI_GMT_NT_0900 = -36,
MMI_GMT_NT_0845 = -35,
MMI_GMT_NT_0830 = -34,
MMI_GMT_NT_0815 = -33,
MMI_GMT_NT_0800 = -32,
MMI_GMT_NT_0745 = -31,
MMI_GMT_NT_0730 = -30,
MMI_GMT_NT_0715 = -29,
MMI_GMT_NT_0700 = -28,
MMI_GMT_NT_0645 = -27,
MMI_GMT_NT_0630 = -26,
MMI_GMT_NT_0615 = -25,
MMI_GMT_NT_0600 = -24,
MMI_GMT_NT_0545 = -23,
MMI_GMT_NT_0530 = -22,
MMI_GMT_NT_0515 = -21,
MMI_GMT_NT_0500 = -20,
MMI_GMT_NT_0445 = -19,
MMI_GMT_NT_0430 = -18,
MMI_GMT_NT_0415 = -17,
MMI_GMT_NT_0400 = -16,
MMI_GMT_NT_0345 = -15,
MMI_GMT_NT_0330 = -14,
MMI_GMT_NT_0315 = -13,
MMI_GMT_NT_0300 = -12,
MMI_GMT_NT_0245 = -11,
MMI_GMT_NT_0230 = -10,
MMI_GMT_NT_0215 = -9,
MMI_GMT_NT_0200 = -8,
MMI_GMT_NT_0145 = -7,
MMI_GMT_NT_0130 = -6,
MMI_GMT_NT_0115 = -5,
MMI_GMT_NT_0100 = -4,
MMI_GMT_NT_0045 = -3,
MMI_GMT_NT_0030 = -2,
MMI_GMT_NT_0015 = -1,

MMI_GMT_PT_0000 = 0, /* Read as GMT +0:00 */


MMI_GMT_PT_0015 = 1,
MMI_GMT_PT_0030 = 2,
MMI_GMT_PT_0045 = 3,
MMI_GMT_PT_0100 = 4,
MMI_GMT_PT_0115 = 5,
MMI_GMT_PT_0130 = 6,
MMI_GMT_PT_0145 = 7,
MMI_GMT_PT_0200 = 8,
MMI_GMT_PT_0215 = 9,
MMI_GMT_PT_0230 = 10,
MMI_GMT_PT_0245 = 11,
MMI_GMT_PT_0300 = 12,
MMI_GMT_PT_0315 = 13,
MMI_GMT_PT_0330 = 14,
MMI_GMT_PT_0345 = 15,
MMI_GMT_PT_0400 = 16,
MMI_GMT_PT_0415 = 17,
MMI_GMT_PT_0430 = 18,
MMI_GMT_PT_0445 = 19,
MMI_GMT_PT_0500 = 20,
MMI_GMT_PT_0515 = 21,
MMI_GMT_PT_0530 = 22,
MMI_GMT_PT_0545 = 23,
MMI_GMT_PT_0600 = 24,
MMI_GMT_PT_0615 = 25,
MMI_GMT_PT_0630 = 26,
MMI_GMT_PT_0645 = 27,
MMI_GMT_PT_0700 = 28,
MMI_GMT_PT_0715 = 29,
MMI_GMT_PT_0730 = 30,
MMI_GMT_PT_0745 = 31,
MMI_GMT_PT_0800 = 32,
MMI_GMT_PT_0815 = 33,
MMI_GMT_PT_0830 = 34,
MMI_GMT_PT_0845 = 35,
MMI_GMT_PT_0900 = 36,
MMI_GMT_PT_0915 = 37,
MMI_GMT_PT_0930 = 38,
MMI_GMT_PT_0945 = 39,
MMI_GMT_PT_1000 = 40,
MMI_GMT_PT_1015 = 41,
MMI_GMT_PT_1030 = 42,
MMI_GMT_PT_1045 = 43,
MMI_GMT_PT_1100 = 44,
MMI_GMT_PT_1115 = 45,
MMI_GMT_PT_1130 = 46,
MMI_GMT_PT_1145 = 47,
MMI_GMT_PT_1200 = 48, /* Read as GMT +12:00 */



MMI_TIME_ERROR = -100
} T_MMI_TZ;


extern void emo_printf(const char *fmt, ...);

int  mmi_timezone_value(int mmi_rtc_timezone);

/*
 * Data for top window
 */
typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        network_win;        /* MFW win handler      */
  T_MFW_HND        nm_handle;          /* MFW network handler  */
  UBYTE			   aborted;
  UBYTE			forbidden;
  UBYTE			   keep_plmn_list;
  T_MFW_NETWORK_STRUCT  *pplmn;              /* pref. PLMN list */
  T_MFW_PPLMN_MEM  pplmn_mem;
  UBYTE            plmn_name[SIZE_PLMN_NAME];      /* textual network name */
  UBYTE            network_name[SIZE_NETWORK_NAME];    /* numeric network name */
  USHORT           status;             /* current status       */
  UBYTE            roaming_indicator;  /* Roaming Indicator    */
  ListMenuData   * network_list;       /* PLMN available List  */
  UBYTE            network_list_status;/* Network List Status */
  UBYTE            network_search_name[MAX_PLMN][6]; /* numeric network name */
  T_MFW_HND        network_search_win;
  UBYTE			    Listposition;
} T_network;

T_network network_data;

typedef struct
{
    T_MMI_CONTROL    mmi_control;
    T_MFW_HND        win;
    SHORT            id;
    T_MFW_HND        parent;
    T_MFW_HND        kbd;
    T_MFW_HND        kbd_long;
    T_MFW_HND        menu;
    T_MFW_HND        dialog_win;
	ListMenuData     *network_list;
/*SPR 2635, replace pplmn data structure pointer with pointer to array of strings*/
	char**		pplmn_longnames;
    CHAR 			   buffer[6];
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_info;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA     editor_info; /* storage for editor attributes */ 
#endif
} T_network_OPT;


typedef enum
{
    E_INIT,
    E_BACK,
    E_RETURN,
    E_ABORT,
    E_EXIT
} e_lists_events;

//  Apr 14, 2004        REF: CRR 12641  xpradipg
static BOOLEAN	Auto_Network_Select=false;
extern T_MFW_HND gPhbkMenu;//    Jul 21, 2004        REF: CRR 13348  xvilliva
//    Sep 23, 2004        REF: CRR 24562  xvilliva
extern int g_ensErr;


/*
 * Local prototypes
 */
static int            network_win_cb                 (T_MFW_EVENT event, T_MFW_WIN * win);
static int            network_nm_cb                  (T_MFW_EVENT event, T_MFW_HND para);
static void           network_remove_network_log     (void);
static UBYTE          network_log_enabled            (void);
static void           network_list_cb                (T_MFW_HND win, ListMenuData * ListData);
static void           network_pref_list_cb           (T_MFW_HND win, ListMenuData * ListData);
static ListMenuData * network_create_list            (T_network * data, T_MFW_PLMN_LIST * plmn_list);
static ListMenuData * network_create_pref_list       (T_network * data);
static ListMenuData * network_create_mobile_list     (T_network_OPT * data);
static void 		network_start_animation_plmn_selection	(int plmn_value);
static void           network_start_animation        (void);
static void           network_stop_animation         (void);
static void           network_selected				         (void);
static void           network_forbidden_plmn         (void);
static void           network_no_plmns               (void);
       void           network_animation_cb           (T_MFW_HND win, UBYTE identifier, UBYTE reason);
//Jul 21, 2004        REF: CRR 13348  xvilliva
//We use a seperate callback for "Network Selected".
void           network_select_animation_cb           (T_MFW_HND win, UBYTE identifier, UBYTE reason);
     T_MFW_HND      network_MNU_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
static T_MFW_HND      network_MNU_create(MfwHnd parent_window);
static void           network_MNU_destroy(MfwHnd own_window);
     void           network_MNU_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int            network_MNU_mnu_cb (MfwEvt e, MfwMnu *m);
static int            network_MNU_win_cb (MfwEvt e, MfwWin *w);
static int            network_MNU_kbd_cb (MfwEvt e, MfwKbd *k);
static int		   network_request_plnm_list (MfwMnu* m, MfwMnuItem* i);
void                network_delete_pref_plnm (T_MFW_HND win , USHORT identifier, UBYTE reason);
void                network_info_screen (T_MFW_HND win, USHORT MessageId, T_VOID_FUNC funt_ptr );// xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
static int            network_delete(MfwMnu* m, MfwMnuItem* i);
	T_MFW_HND network_EDT_start (MfwMnu* m, MfwMnuItem* i);
static T_MFW_HND network_EDT_create (T_MFW_HND parent);
static void network_EDT_destroy (T_MFW_HND window);
static void network_EDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static void network_EDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event);
/* SPR#2354 - SH -Added two new callbacks */
static void network_request_plmn_list_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);
void network_mobile_list_changed_cb(T_MFW_HND win , USHORT identifier, UBYTE reason);

//x0pleela 30 May, 2007  DR: OMAPS00133201
void nm_abort_network( void );

// June 02, 2006    DR: OMAPS00078005 x0021334
// Call back function of info dialog for getting user confirmation for time update
//Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
static int nw_time_update_callback(T_MFW_HND win, USHORT identifier, UBYTE reason);

// June 02, 2006    DR: OMAPS00078005 x0021334
// To update RTC with new time and date info.
static T_MFW_TIME newTime;
static T_MFW_DATE newDate;
#endif

/*
 * Local Definitions
 */
#define NETWORK_LIMITED_SERVICE   420
#define NETWORK_NO_NETWORK        421
#define NETWORK_PLMN_LIST         422
#define NETWORK_PREFERRED_LIST    423
#define NETWORK_PPLMN_MEM_LIST    424
/* SPR759 - SH - Cell reselection */
#define NETWORK_CELL_SELECT        425

#define NETWORK_LIST_DISPLAY      430
#define NETWORK_LIST_SELECT_PLMN  432

// June 02, 2006    DR: OMAPS00078005 x0021334
//Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
#define NETWORK_TIME_UPDATE       445
#endif
//Sep 18 2006 DRT :OMAPS0009268 x0061088
#define NETWORK_FORBIDDEN_NETWORK 446 

#ifdef NEPTUNE_BOARD
/* List destory not done - added - OMAPS00061049 - 08-03-2006 */
#define NETWORK_LIST_DESTORY          440   
#endif

/*
 * Local Network List Status
 */
#define NETWORK_LIST_DISPLAY_FIRST   1
#define NETWORK_LIST_SELECTED        2
#define NETWORK_LIST_REGISTRATION    3
#define NETWORK_PREF_LIST_SELECTED   4
#define NETWORK_MENU_ITEM            2

//nm 5.3.01 this number will be change
#define MAX_SHOW_OF_PLMN 			99

static MfwMnuItem network_OPTItems [] =
{
    {0,0,0,(char *)TxtSelectFromList,    0,  (MenuFunc)network_request_plnm_list, item_flag_none},
    {0,0,0,(char *)TxtEnterNetworkCode, 0,  (MenuFunc)network_EDT_start,   item_flag_none},
    {0,0,0,(char *)TxtDelete,           0,  (MenuFunc)network_delete,          item_flag_none}
};

static MfwMnuAttr network_OPTAttrib =
{
    &network_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    network_OPTItems,                      /* with these items         */
    sizeof(network_OPTItems)/sizeof(MfwMnuItem), /* number of items     */
	COLOUR_LIST_NETWORK, TxtNull, NULL, MNUATTRSPARE


};



static MfwMnuAttr network_menuAttrib =
{
    &network_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_NETWORK, TxtNull, NULL, MNUATTRSPARE

};

//x0pleela 09 May, 2007  DR: OMAPS00129014
#ifdef FF_PHONE_LOCK
extern T_MFW_HND	phlock_win_handle;			/* to hold the win handle of phone unlock editor window 	*/
GLOBAL UBYTE phlock_auto;
//x0pleela 29 May, 2007 DR: OMAPS00132483
EXTERN int mfw_get_Phlock_status(void);
EXTERN void mfw_set_Phlock_status( int phlock_status);
EXTERN int mfw_get_Auto_Phlock_status(void);
EXTERN void mfw_set_Auto_Phlock_status( int phlock_status);
#endif //F_PHONE_LOCK
/*******************************************************************************

 $Function:    	networkInit

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND networkInit (T_MFW_HND parent_window)
{
  return (network_create (parent_window));
}


/*******************************************************************************

 $Function:    	networkExit

 $Description:	exit network handling (Backward Compatibility interface)

 $Returns:		

 $Arguments:	
 *********************************************************************/


void networkExit (void)
{
  network_destroy (network_data.network_win);
}

/*********************************************************************
 *
 * Network Dialog
 *
 * Window - Structure
 *
 * network    -->  network_list
 *            -->  network_search_animation
 *
 *********************************************************************/

/*******************************************************************************

 $Function:    	network_create

 $Description:	create network top window

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND network_create (T_MFW_HND parent_window)
{
  T_network * data = &network_data;
  T_MFW_WIN * win;

  emo_printf ("network_create()");

  data->network_win = win_create (parent_window, 0, 0, (T_MFW_CB)network_win_cb);

  if (data->network_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog = (T_DIALOG_FUNC)network;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->network_win)->data;
  win->user                = (void *) data;

  /*
   * Initialise MFW
   */
  nm_init ();

  /*
   * Create any other handler
   */
    data->nm_handle = nm_create (data->network_win, E_NM_ALL_SERVICES, (T_MFW_CB)network_nm_cb);

  memset (data->plmn_name, 0, SIZE_PLMN_NAME);
  memset (data->network_name, 0, SIZE_NETWORK_NAME);

  data->aborted = FALSE;
  data->keep_plmn_list = FALSE;
  data->forbidden = FALSE;

  data->status   = NETWORK_NO_SERVICE;
  data->network_list = NULL;
  data->network_search_win = 0;
  data->Listposition = 0;



  winShow(data->network_win);
  /*
   * return window handle
   */
  return data->network_win;
}

/*******************************************************************************

 $Function:    	network_destroy

 $Description:	destroy network top window

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN * win;
  T_network * data;

  emo_printf ("network_destroy()");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_network *)win->user;

    if (data)
    {
      /*
       * Exit NM and delete NM handler
       */
      nm_exit ();
      nm_delete (data->nm_handle);

      /*
       * Delete WIN handler
       */
      win_delete (data->network_win);
    }
  }
}

/*******************************************************************************

 $Function:    	network

 $Description:	network top window dialog function

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
 
  T_MFW_WIN         * win_data = ((T_MFW_HDR *) win)->data;
  T_network         * data     = (T_network *)win_data->user;
  T_MFW_NETWORK_STRUCT  * plmn_ident;
  UBYTE signal_value;

  // June 02, 2006    DR: OMAPS00078005 x0021334
  //Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
  #ifdef FF_TIMEZONE
  T_DISPLAY_DATA  display_info ;  // Structure for info_dialog
  T_MFW_TIME_IND *network_time; // Time/date info from network
  T_MFW_TIME *time_format; // To get the current time format set in MS
  #endif

  	
  emo_printf(">>> network()");

//x0pleela 09 Mar, 2007  DR: OMAPS00129014
#ifdef FF_PHONE_LOCK
	if( mfw_phlock_get_lock_status(MFW_PH_LOCK) == MFW_PH_LOCK_ENABLE ) 
	{
		emo_printf("idle_draw: MFW_PH_LOCK_ENABLE");

		//x0pleela 23 May, 2007   DR: OMAPS00132483
		//Set the phone lock status to TRUE
		mfw_set_Phlock_status(TRUE);
		
		if( !phlock_win_handle )
			phLock_UnLock();
	}
	else //for Automatic Phone lock check
	{
		//x0pleela 23 May, 2007   DR: OMAPS00132483
		//Set the phone lock status to FALSE
		mfw_set_Phlock_status(FALSE);
		
		if( mfw_phlock_get_autolock_status(MFW_AUTO_PH_LOCK) == MFW_PH_LOCK_ENABLE )
		{
			phlock_auto = TRUE;

			//x0pleela 23 May, 2007   DR: OMAPS00132483
			//Set the auto phone lock status to TRUE
			mfw_set_Auto_Phlock_status(TRUE);
		}		
		//x0pleela 23 May, 2007   DR: OMAPS00132483
		//Set the auto phone lock status to FALSE
		mfw_set_Auto_Phlock_status(FALSE);
	}
#endif /* FF_PHONE_LOCK */

  switch (event)
  {
    case NETWORK_NO_SERVICE:
    case NETWORK_LIMITED_SERVICE:
      emo_printf ("NETWORK NO/LIMITED SERVICE");
      /*
       * Set Status
       */
      data->status = NETWORK_NO_SERVICE;

      // June 02, 2006    DR: OMAPS00078005 x0021334
      // Set PCTZV mode on
      //Dec 13,2006       DR:OMAPS00098351  x0066814-Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
      nm_get_network_time();      
#endif

      /*
       * Check if network log is enabled
       */
      if (network_log_enabled ())
      {
        SEND_EVENT (idle_get_window(), NETWORK_NO_PLMN_XXX_AVAIL, 0, 0);
      }
      else
      {
        SEND_EVENT (idle_get_window(), NETWORK_NO_SERVICE, 0, 0);
        if (data->network_list NEQ NULL)
        {
          switch (data->network_list_status)
          {
            case NETWORK_LIST_REGISTRATION:
		emo_printf ("NETWORK_LIST_REGISTRATION");
              /*
               * Present list of available PLMNs again
               *
		 * Start the available plmn search
		 */
		nm_avail_plmn_list();
		//Sep 23, 2004        REF: CRR 24562  xvilliva
		//After retrieving the list we see if an error occured 
		//and flash an alert to the user accordingly.
		if(g_ensErr == 1)
		{
			g_ensErr = 0;
			info_screen(0, TxtNotAllowed, TxtNull, NULL);
		}

		/*
		 * remove network log if needed
		 */
		network_remove_network_log ();

		network_data.forbidden = TRUE;
              break;

            case NETWORK_LIST_SELECTED:
		emo_printf ("NETWORK_LIST_SELECTED");
              /*
               * After registration start, MFW returns the current status
               */
              data->network_list_status= NETWORK_LIST_REGISTRATION;
              data->keep_plmn_list = TRUE;
              break;
          }
        }
      }
 //Oct 05, 2006    ER: OMAPS00094496 x0061088(Prachi)
	 signal_value = 0 ;
        globalSignalUpdate(signal_value);       
#ifdef MMI_HOMEZONE_ENABLED
	/* sh - tell homezone there is no network */
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to incorporate the third argument.
	homezoneUpdate(0,0,0);
#endif
      break;

    case NETWORK_SEARCH_NETWORK:
      emo_printf ("NETWORK SEARCH NETWORK");
      SEND_EVENT (idle_get_window(), NETWORK_SEARCH_NETWORK, 0, 0);
      break;

    case NETWORK_FULL_SERVICE:
      emo_printf("NETWORK_FULL_SERVICE");
      /*
       * Set Status
       */
      data->status = NETWORK_FULL_SERVICE;
#ifdef FF_CPHS_REL4  
	mfw_cphs_get_msp_info();
#endif

      // June 02, 2006    DR: OMAPS00078005 x0021334
      // Set PCTZV mode on
      //Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
      nm_get_network_time();       
#endif
      /*
       * Copy Network Name in textual and numerical format
       *
       * NOTE: Service Provider Name is not used
       *       Operator Name from SIM needs CPHS support by MFW
       */
      plmn_ident = (T_MFW_NETWORK_STRUCT *)parameter;

	//x0pleela 15 Feb, 2006  ER: OMAPs00065203
	//Removed the global variable gOper_sel_done and have introduced 2 functions which will 
	//set and get the status of the operator selection
	
       //x0pleela 9 Feb, 2006  ER:OMAPS00065203
	//Check if EONS is allocated and activated. If not,  and if operator name is not yet obtained from other sources
	//get operator name from other sources
	if( ( ( plmn_ident->opn_read_from EQ NM_CPHS ) AND ( plmn_ident->roaming_indicator)  )
  	OR (plmn_ident->opn_read_from EQ NM_ROM)  ) 
      {
        //get operator name from other sources CPHS or NITZ or ROM
         nm_get_opername(plmn_ident);
      }
memset(data->plmn_name, 0, SIZE_PLMN_NAME);
memset(data->network_name, 0, SIZE_NETWORK_NAME);  /* x0039928 - Lint warning removal */
#ifdef NO_ASCIIZ
      memcpy (data->plmn_name, &plmn_ident->network_long.data, SIZE_PLMN_NAME);
#else
      memcpy (data->plmn_name, plmn_ident->network_long, SIZE_PLMN_NAME);
#endif      
      memcpy (data->network_name, plmn_ident->network_numeric, SIZE_NETWORK_NAME);

     /*
       * If network name is not known use numeric representation as default.
       */
  
#ifdef NO_ASCIIZ
//Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)     
if(!(strlen((char*)data->plmn_name))) 
{
     
      	  if( plmn_ident->network_short.len EQ 0 )      	  	
      	   strcpy ((char *)data->plmn_name, (char *)data->network_name);
      	  else
      	    memcpy (data->plmn_name, &plmn_ident->network_short.data, SHORT_NAME); /* x0039928 -Lint warning removal */
      	
}
#else    
//Apr 13, 2007 DRT:OMAPS00122739 x0066814(Geetha)     
if(!(strlen((char*)data->plmn_name))) 
{
      
      		 if (strlen ((char *)(plmn_ident->network_short)) EQ 0)
       		strcpy ((char *)data->plmn_name, (char *)data->network_name);
      		else
      			memcpy (data->plmn_name, &plmn_ident->network_short, SIZE_PLMN_NAME);
  	
}
#endif
      /*
       * Store Roaming Indicator
       */
      data->roaming_indicator = plmn_ident->roaming_indicator;

      network_stop_animation ();
  
      /*
       * Inform Idle screen
       */
	  SEND_EVENT (idle_get_window(), NETWORK_FULL_SERVICE, 0, 0);

      /*
       * If network list is available
       */
      if (data->network_list NEQ NULL)
      {
        switch (data->network_list_status)
        {
          case NETWORK_LIST_REGISTRATION:
            if(data->keep_plmn_list EQ FALSE)
            {
	          emo_printf("NETWORK_LIST_REGISTRATION No 1");
				
#ifdef FF_2TO1_PS
#ifndef NEPTUNE_BOARD
                 FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                        MMREG_MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
#else
                /* Free the allocated memory for PLMN list window - 
                    OMAPS00061049 - 08-03-2006 */
		  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                       Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif

#else
               
#ifndef NEPTUNE_BOARD
		  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                        MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
#else
                 /* Free the allocated memory for PLMN list window - 
                     OMAPS00061049 - 08-03-2006 */
		  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                        Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif

#endif
                FREE_MEMORY ((UBYTE *)data->network_list, sizeof (ListMenuData));
                data->network_list = NULL;
            }
            data->keep_plmn_list = FALSE;
            break;
          case NETWORK_LIST_SELECTED:
            /*
             * After registration start, MFW returns the current status
             */
            emo_printf("NETWORK_LIST_SELECTED");
            data->network_list_status= NETWORK_LIST_REGISTRATION;
			/* Commented out and moved
       		network_selected(); */
            break;
        }
        // API - Moved from previous position
		network_selected(); 
      }

	  //  Apr 14, 2004        REF: CRR 12641  xpradipg
	  if(Auto_Network_Select==true)
	  {
	  	network_selected();
		Auto_Network_Select=false;
	  }
	  	
      break;

    case NETWORK_NO_NETWORK:
      emo_printf ("NETWORK NO NETWORK");
      /*
       * No PLMN found after search for available PLMNs
       */
      network_stop_animation ();
      if (network_log_enabled ())
      {
        data->status = NETWORK_NO_SERVICE;
        SEND_EVENT (idle_get_window(), NETWORK_NO_PLMN_XXX_AVAIL, 0, 0);
      }
      else
      {
        /*
         * No Networks found.
         */
        data->status = NETWORK_NO_SERVICE;
        SEND_EVENT (idle_get_window(), NETWORK_NO_SERVICE, 0, 0);
      }
#ifdef MMI_HOMEZONE_ENABLED
	/* sh - tell homezone there is no network */
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to incorporate the third argument.
	homezoneUpdate(0,0,0);
#endif
      break;
//sep 18 2006 OMPAS00090268 x0061088(Prachi)
    case NETWORK_FORBIDDEN_NETWORK:
    case NETWORK_PLMN_LIST:
      emo_printf ("NETWORK PLMN LIST");
       emo_printf (">>NETWORK PLMN LIST");
      /*
       * PLMN list found
       */
	  if (network_data.aborted != TRUE)
	  {
		  if (network_log_enabled ())
		  {
			data->status = NETWORK_NO_SERVICE;
			SEND_EVENT (idle_get_window(), NETWORK_NO_PLMN_XXX_AVAIL, 0, 0);
		  }
		  else
		  {
			T_MFW_PLMN_LIST * plmn_list;
			/*
			 * Networks founds.
			 */
			data->status = NETWORK_NO_SERVICE;
			/*
			 * If network list is available, destroy
			 */
			if (data->network_list NEQ NULL)
			{

#ifdef FF_2TO1_PS                       
#ifndef NEPTUNE_BOARD
			  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                               MMREG_MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
#else
                       /* Free the allocated memory for PLMN list window - 
                           OMAPS00061049 - 08-03-2006 */
			  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                              Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif

#else
#ifndef NEPTUNE_BOARD
			  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                               MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
#else
                       /* Free the allocated memory for PLMN list window - 
                           OMAPS00061049 - 08-03-2006 */  
			  FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                              Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif

#endif
			  
			  FREE_MEMORY ((UBYTE *)data->network_list, sizeof (ListMenuData));
			  data->network_list = NULL;
			}

/* Jan 10, 2007 REF: OMAPS00110741  x0039928(sumanth) */
/* Soln : check if parameter passed is NULL                          */
	if(parameter != NULL)
	{
			/*
			 * If list is empty, show info screen
			 */
			plmn_list = (T_MFW_PLMN_LIST *)parameter;

#ifdef NEPTUNE_BOARD
			/* Count assigned to global static variable - OMAPS00061049 - 08-03-2006 */
			Plmn_list_count = plmn_list->count;
#endif
/* Dec 13, 2006    DRT: OMAPS00105640 x0039928(Sumanth)
     Fix: Condition added to check if sim is active                      */
	if(SIM_NOT_ACTIVE != sim_status_check())
	{			
			if (plmn_list->count EQ 0)
			{
			  network_stop_animation ();
			  network_no_plmns ();
			}
			else
			{
			  /*
			   * Create and present list of available PLMNs
			   *
			   */

			  /* Pass the plmn_list which already hold the parameter data - OMAPS00061049 - 08-03-2006 */
#ifndef NEPTUNE_BOARD			  
			  data->network_list = network_create_list (data, parameter);
#else
			  data->network_list = network_create_list (data, plmn_list );
#endif
			  data->network_list_status = NETWORK_LIST_DISPLAY_FIRST;
			  network_stop_animation ();
			  listDisplayListMenu (win, data->network_list,  (ListCbFunc)network_list_cb,0);
	 
	               // change by Sasken ( Pradeep/Vadiraj) on Apr 07th 2004
			// Issue Number : SPR#12654 on 07/04/04 by Pradeep/Vadiraj
			// Bug : Pressing "search" button does not trigger anything
			// Solution: When the ME is already camped on to a network the NoNetwork event is not sent.
			// Commented the following line
			  // SEND_EVENT (idle_get_window(), NETWORK_NO_SERVICE, 0, 0);

			
			}
		}
      }
      }
		  //Mar 30,2007  DRT: OMAPS00120157 x00061088(Prachi)
		     if(data->network_list_status == NETWORK_LIST_DISPLAY_FIRST)
		    {
        // Feb 08,2007    DRT: OMAPS00115060 x00061088(Prachi)		  
			  if (network_data.forbidden == TRUE)
			  {
				network_forbidden_plmn();
				network_data.forbidden = FALSE;
			  }
		    }
			  
			}
	  else
	  {
		  network_data.aborted = FALSE;
	  }
#ifdef MMI_HOMEZONE_ENABLED
	/* sh - tell homezone there is no network */
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to incorporate the third argument.
	homezoneUpdate(0,0,0);
#endif
      break;

    case NETWORK_LIST_SELECT_PLMN:

      emo_printf ("NETWORK LIST_SELECT");
       emo_printf (">> NETWORK LIST_SELECT");
      /*
       * A network has been selected from the PLMN available list
       * Start manual registration and start Search animation.
       */
      network_start_animation_plmn_selection (value);
      data->network_list_status = NETWORK_LIST_SELECTED;
      nm_select_plmn ((U8 *)network_data.network_search_name[value]);
		//Sep 23, 2004        REF: CRR 24562  xvilliva
		//After retrieving the list we see if an error occured 
		//and flash an alert to the user accordingly.
		if(g_ensErr == 1)
		{
			g_ensErr = 0;
			info_screen(0, TxtNotAllowed, TxtNull, NULL);
		}

		/* x0056422, OMAPS00148258 */

		if (data->network_list NEQ NULL)
   		{
			#ifdef FF_2TO1_PS                       
			#ifndef NEPTUNE_BOARD
     			FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                               MMREG_MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
			#else
                /* Free the allocated memory for PLMN list window - 
                OMAPS00061049 - 08-03-2006 */
    	 		FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                              Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
			#endif /* NEPTUNE_BOARD */
 
			#else
				#ifndef NEPTUNE_BOARD
     				FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                             MAX_PLMN_ID * sizeof (T_MFW_MNU_ITEM)); 
				#else
					/* Free the allocated memory for PLMN list window - 
                        OMAPS00061049 - 08-03-2006 */  
     				FREE_MEMORY ((UBYTE *)data->network_list->List, 
                                              Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
				#endif /* NEPTUNE_BOARD */
 
			#endif /* FF_2TO1_PS */
     
			FREE_MEMORY ((UBYTE *)data->network_list, sizeof (ListMenuData));
			data->network_list = NULL;
   	} 
	 //Modify by hou peng 2007/09/19 end 

	/* x0056422 OMAPS00148258 */

      break;
   case NETWORK_PPLMN_MEM_LIST:
	  {
      T_MFW_PPLMN_MEM *pplmn_mem;
	  UBYTE i;

      emo_printf (">> NETWORK PPLMN MEM LIST");
      /*  Save the number of PLMN records:
       *  maxRcd  = maximum preferred PLMN records on the SIM card
       *  usedRcd = used preferred PLMN records on the SIM card
       */
      pplmn_mem = (T_MFW_PPLMN_MEM *)parameter; 

	  network_data.pplmn_mem.maxRcd  = pplmn_mem->maxRcd;
	  network_data.pplmn_mem.usedRcd = pplmn_mem->usedRcd;

      /*
       * Request preferred PLMN list from the SIM
       */

      	
	    network_data.pplmn = (T_MFW_NETWORK_STRUCT *)ALLOC_MEMORY ((pplmn_mem->maxRcd) * sizeof (T_MFW_NETWORK_STRUCT));
      	
		if (network_data.pplmn EQ 0)
		{
			return;
		}

	 	if (pplmn_mem->usedRcd > 0)
		 {
		 	 // request MFW to fill up the pref. PLMN list
		     nm_pref_plmn_list(network_data.pplmn, pplmn_mem->maxRcd, 1);
		 }
	 	else
	 	{
	 		// doesnt make sense to request the MFW to fill up the list
	 		// when the "pplmn_mem->usedRcd" is  0

	 		 emo_printf(">> NETWORK PPLMN MEM LIST - ELSE Loop");
			 for (i=0; i < (network_data.pplmn_mem.maxRcd); i++)
			 {
#ifdef NO_ASCIIZ
				network_data.pplmn[i].network_long.len = 0;
				network_data.pplmn[i].network_short.len = 0;
#else			 	
				memset(network_data.pplmn[i].network_long,   '\0', LONG_NAME);
				memset(network_data.pplmn[i].network_short,   '\0', SHORT_NAME);
#endif
				memset(network_data.pplmn[i].network_numeric, '\0', NUMERIC_LENGTH);
			 }
			
		    data->network_list = network_create_pref_list (data);
		    listDisplayListMenu (win, data->network_list,(ListCbFunc)network_pref_list_cb,0);
	 	}

      }
      break;

   case NETWORK_PREFERRED_LIST:
	  {			

	    /* UBYTE i; */ /* Warning Correction */

	  
      emo_printf (">>NETWORK PREFERRED LIST");
      /*
       *  Create and present the preferred list of PLMNs
       * 
       */

      //nm      network_stop_animation ();

    
    data->network_list = network_create_pref_list (data);
	
    /* cq12719 Check if prefered list info resource has been created.16-03-04 MZ */
    if(data->network_list  != NULL)
    		listDisplayListMenu (win, data->network_list,(ListCbFunc)network_pref_list_cb,0);
      }   
      break;

  case NETWORK_CELL_SELECT:
  	{
		
	/* x0045876, 14-Aug-2006 (WR -"cell_data" was set but never used) */
#ifdef MMI_HOMEZONE_ENABLED
  	/* If Homezone is active, inform it of the new cell data */
  	T_MFW_CELL_STRUCT *cell_data;
#endif
  	
  	emo_printf(">>NETWORK CELL SELECT");

  	
       /* x0045876, 14-Aug-2006 (WR -"cell_data" was set but never used) */
	/* cell_data = (T_MFW_CELL_STRUCT *)parameter; */

#ifdef MMI_HOMEZONE_ENABLED

	/* x0045876, 14-Aug-2006 (WR -"cell_data" was set but never used) */
	cell_data = (T_MFW_CELL_STRUCT *)parameter;

	/* SPR759 - SH - send cell reselection info to homezone */
	
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to incorporate the third argument.
	homezoneUpdate(cell_data->lid, cell_data->ci, 0);
#endif
	break;
#ifdef NEPTUNE_BOARD
    /* Destory the list when select softkey is pressed / back softkey is pressed */
   /* OMAPS00061049 - 08-03-2006 - BEGIN*/	
   case  NETWORK_LIST_DESTORY:
              emo_printf ("Inside NETWORK_LIST_DESTORY Event");
			  
		/* Destory the list window */	  
	       listsDestroy(data->network_list->win);

		emo_printf ("Destoryed list window successfully");
		
		/*
		 * If network list is available, destroy
		 */
		if (data->network_list NEQ NULL)
		{

#ifdef FF_2TO1_PS
                /* Free the allocated memory for PLMN list window - OMAPS00061049 - 08-03-2006 */ 
		  FREE_MEMORY ((UBYTE *)data->network_list->List, Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#else
                /* Free the allocated memory for PLMN list window - OMAPS00061049 - 08-03-2006 */ 
		  FREE_MEMORY ((UBYTE *)data->network_list->List, Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif
		  
		  FREE_MEMORY ((UBYTE *)data->network_list, sizeof (ListMenuData));
		  data->network_list = NULL;
		}

		emo_printf ("List window memory freed");
	  break;
	/* OMAPS00061049 - 08-03-2006 - END */ 	
#endif	
		

  }

// June 02, 2006    DR: OMAPS00078005 x0021334
// Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
// Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
// function. The same is received and handled in case NETWORK_TIME_UPDATE in MMI for user 
// confirmation and RTC updation.

//Dec 13,2006       DR:OMAPS00098351  x0066814 - Begin
#ifdef FF_TIMEZONE
  case NETWORK_TIME_UPDATE:
  		{
  BOOL modify_date_negative = FALSE;
  BOOL modify_date_positive = FALSE;    
  int actual_TD = 0;
  int absolute_value;
    
  emo_printf ("Inside NETWORK_TIME_UPDATE Event");

if(FFS_flashData.nitz_option!=NitzOff)
{

emo_printf("NITZ- not OFF");

  // Copy time and date info received from network into 'time'
  network_time = (T_MFW_TIME_IND *)parameter;
  actual_TD = network_time->timezone;
  absolute_value = (10*(actual_TD & 0x7))+((actual_TD >> 4) & 0xf);

emo_printf("actual_TD: %d", actual_TD);
emo_printf("Absolute value: %d", absolute_value);
emo_printf("IN HEX");
emo_printf("actual_TD: %x", actual_TD);
emo_printf("Absolute value: %x", absolute_value);
  if((actual_TD & 0x08))
  {
    absolute_value = ~absolute_value+1;
  }
  
actual_TD = absolute_value;

if(actual_TD >0)
{

                     network_time->hour = network_time->hour + actual_TD/4 ;
                     
                     network_time->minute = network_time->minute + (actual_TD%4)*15;
                        if(network_time->minute > 60)
                            {
                            network_time->minute = network_time->minute - 60 ;
                            network_time->hour = network_time->hour +1;
                            }
                        if(network_time->minute == 60)
                            {
                            network_time->minute = 0;
                            network_time->hour = network_time->hour +1;
                            }
                        if(network_time->hour >= 24)
                            modify_date_positive = TRUE;


}
else if(actual_TD<0)
{

                        if((network_time->minute - abs((actual_TD%4)*15)) < 0)
                            {
                            network_time->minute = 60 - (abs((actual_TD%4)*15)  - network_time->minute );

                            network_time->hour = network_time->hour - 1;
                            }
                        else if((network_time->minute - abs((actual_TD%4)*15)) == 0)
                            {
                            network_time->minute = 0;
                            network_time->hour = network_time->hour - 1;
                           
                            }
                        else
                            network_time->minute = network_time->minute - abs((actual_TD%4)*15);
                            
                        if((network_time->hour - abs(actual_TD/4 ))  < 0)
                            modify_date_negative = TRUE;
                        else
                            network_time->hour = network_time->hour - abs(actual_TD/4 );  

}

//Assume the date is 1/07/2005 5 Am in the morning 
//After time zone calculation it needs to become 30/06/2005 and not 31 st June 2005 since there are only 30 days
//in all the odd months before July and in all the even months after july(7th Month)
//Similarly a problem arises if the year too needs to be adjusted after the timezone calculations.


if(modify_date_negative)
 {

			emo_printf("modify date negative");
	           	network_time->hour  = 24 - abs(network_time->hour - abs(actual_TD/4 ));

	              network_time->day = network_time->day -1;
				   
                         if(network_time->day == 0)
                            {
                                if(((network_time->month)%2 != 0) && network_time->month <= 7)
                                    {
                                    if( network_time->month == 1)
                                        {
                                            network_time->month = network_time->month -1;  
                                            network_time->day = 31;
                                        }
					     else{
                                        network_time->month = network_time->month -1;  
                                        network_time->day = 30;
					     	}
                                    }
                                else if(((network_time->month)%2 == 0) && network_time->month < 7)
                                    {
                                        network_time->month = network_time->month -1;  
                                        network_time->day = 31;
                                    }
                                if(((network_time->month)%2 == 0) && network_time->month > 7)
                                    {
                                        if( network_time->month == 8)
                                        {
                                            network_time->month = network_time->month -1;  
                                            network_time->day = 31;
                                        }
					     else
                                        {
                                            network_time->month = network_time->month -1;  
                                            network_time->day = 30;
                                        }
										
                                	}
                                if(network_time->month == 2)
                                    {
                                        if(((network_time->year+2000)%4 == 0) && ((network_time->year+2000)%100 != 0))
                                            network_time->day = 29;
                                        else
                                            network_time->day = 28;
                                    }
                                if(network_time->month <= 0)
                                    {
                                        network_time->year = network_time->year -1;
                                        network_time->month = 12;
                                    }
                            }
}

else if(modify_date_positive)
{
                    	emo_printf("modify date positive");
                         network_time->hour =  network_time->hour -24;
       
                         network_time->day = network_time->day + 1;

                        if(network_time->month ==  2)
                            {
                                 if(network_time->day ==  30)
                                    {
                                        network_time->day =  1;
                                        network_time->month =  3;                                        
                                    }
                                 if(network_time ->day == 29)
                                    {
                                                if(((network_time->year+2000)%4 == 0) && ((network_time->year+2000)%100 != 0))
                                                	   {
                                                       network_time->day = 29;
                                                	   }
                                                else
                                                    {
                                                        network_time->day = 1;
                                                        network_time->month = 3;
                                                    }
                                    }
                            }
                         	else
				{
					if(network_time->day>=31)
					{
					if(network_time->day == 31)  
                            	{
                                	if(((network_time->month)%2 != 0) && network_time->month <= 7)
                                    {
                                        network_time->day = 31;
                                    }
                                else if(((network_time->month)%2 == 0) && network_time->month > 7)
                                    {
                                        network_time->day = 31;
                                    }
						
                                else
                                    {
                                        network_time->day = 1;
                                        network_time->month = network_time->month +1;
                                    }
                         		}
					else
                                    {
                                        network_time->day = 1;
                                        network_time->month = network_time->month +1;
                                    }
                         		}
                           	}	
                                if(network_time->month > 12)
                                    {
                                        network_time->year = network_time->year + 1;
                                        network_time->month = 1;
                                    }
                    }

  // Get the current time and format info set in the MS 
  time_format = mfw_td_get_time();

  // Copy this info in the structure that will be used to do RTC upation.
  // This ensures that the format set remains the same even after RTC updation.
  newTime = *time_format;
    
  emo_printf("------------------------------------");
  emo_printf("Current format set in MS : %d", time_format->format);

  // The time info received from network is in 24Hrs format. Depeneding on the format
  // curently set in the MS, we need to do conversion and then update the RTC.
  if (time_format->format == RTC_TIME_FORMAT_12HOUR)
  {
      emo_printf("12 Hrs format");
        
      if (network_time->hour == 0) // If hour info from network is 0, set hour as 12 
      {                    // and the AM/PM flag off to indicate AM. 
          newTime.PM_flag = 0;
          newTime.hour = 12;
      }
      else if (network_time->hour < 12) // If hour info from network is less than 12,
	  {                         // set the hour info as it is and AM flag to off.
            newTime.PM_flag = 0;
            newTime.hour = network_time->hour;
	  }
	  else
	  {					
		   newTime.PM_flag = 1;  // If hour info from network is greater than 12,
		   newTime.hour = network_time->hour - 12; // set hour = received hour - 12 and AM/PM
	  }                                     // flag to ON indicate PM.
   
  newTime.minute = network_time->minute; // Set minute info
  newTime.second = network_time->second; // Set second info

  // Set the date related info. Year information is decoded in the following way:
  // 0 - 2000 - default
  // 1 - 2001
  // 2 - 2002 etc.
  // Hence we need to add 2000 to the year value received from network to 
  // display the actual year.
  
  newDate.day = network_time->day; // Day of the month
  newDate.month = network_time->month; // Month
  newDate.year = network_time->year + 2000; // year 
  }
  else
  {
      emo_printf("24 Hrs format");        
      newTime.hour = network_time->hour; // In 24 Hrs format, set the received hour info as it is.
  
     newTime.minute = network_time->minute; // Set minute info
  newTime.second = network_time->second; // Set second info

  // Set the date related info. Year information is decoded in the following way:
  // 0 - 2000 - default
  // 1 - 2001
  // 2 - 2002 etc.
  // Hence we need to add 2000 to the year value received from network to 
  // display the actual year.
  
  newDate.day = network_time->day; // Day of the month
  newDate.month = network_time->month; // Month
  newDate.year = network_time->year + 2000; // year 
    }
  
//if(FFS_flashData.nitz_option==NitzAutomatic)
//{
	
   /* Lets assume get RTC automatically */
   // Update RTC with new time info
   mfw_td_set_time(&newTime);

   // Update RTC with new date info            
   mfw_td_set_date(&newDate);

   // Send IDLE_UPDATE event to force idle screen refresh             
   SEND_EVENT(idle_get_window(), IDLE_UPDATE, 0, 0);

	       
/*
}
else //if(FFS_flashData.nitz_option==NITZOn)
{
  // Show a dialog to the user to get confirmation for Time/Date updation.
  dlg_initDisplayData_TextId( &display_info, TxtYes, TxtNo, TxtTimeUpdate, TxtTimeUpdateConfirm, COLOUR_STATUS);
  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)nw_time_update_callback, FOREVER, KEY_HUP | KEY_LEFT | KEY_RIGHT);
  info_dialog (NULL, &display_info);
}
*/
}
  }
  break;
 #endif

  }
}
//Dec 13,2006       DR:OMAPS00098351  x0066814 - End
/*******************************************************************************

 $Function:    	network_win_cb

 $Description:	network top window

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static int network_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}


/*******************************************************************************

 $Function:    	network_nm_cb

 $Description:	network handler (mfw callback handler)

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static int network_nm_cb (T_MFW_EVENT event, T_MFW_HND para)
{
  emo_printf("network_nm_cb");

  switch(event)
  {
    case E_NM_FULL_SERVICE:
//x0pleela 9 Feb, 2006  ER:OMAPS00065203
//Handling E_NM_OPN for operator name display
    	case E_NM_OPN:
			
       SEND_EVENT (network_data.network_win, NETWORK_FULL_SERVICE, 0, para);
      break;
   //Sep 18,2006    DRT: OMAPS00090268 x0061088(Prachi)
   //Description : To check for Forbidden network (for denied services),it takes long time.
   //Solution : Function network_nm_cb() has now be implemented to handle E_NM_FORBIDDEN_NETWORK,which sends event
   //NETWORK_FORBIDDEN_NETWORK to network(),by setting flags network_data.aborted  =False 
   //and network_data.forbidden = Ture
    case E_NM_FORBIDDEN_NETWORK:
	  	  network_data.forbidden = TRUE;
	 	  network_data.aborted = FALSE;
  		  /* Jan 10, 2007 REF: OMAPS00110741  x0039928(sumanth) */
		  /* Fix : Pass NULL as parameter                                         */
	  	 SEND_EVENT (network_data.network_win, NETWORK_FORBIDDEN_NETWORK, 0, 0);
      break;
   	
    case E_NM_LIMITED_SERVICE:
      SEND_EVENT (network_data.network_win, NETWORK_LIMITED_SERVICE, 0, para);
      break;

    case E_NM_NO_SERVICE:
      SEND_EVENT (network_data.network_win, NETWORK_NO_SERVICE, 0, para);
   	  break;

    case E_NM_SEARCH_NETWORK:
	  SEND_EVENT (network_data.network_win, NETWORK_SEARCH_NETWORK, 0, para);
   	  break;

    case E_NM_NO_NETWORK:
	  SEND_EVENT (network_data.network_win, NETWORK_NO_NETWORK, 0, para);
  	  break;

	case E_NM_PLMN_LIST:
	  SEND_EVENT (network_data.network_win, NETWORK_PLMN_LIST, 0, para);
      break;

	case E_NM_DEREGISTRATION:
		emo_printf("MmiNetwork:E_NM_DEREGISTRATION");
		mainShutdown();
		break;
	case E_NM_PREF_PLMN_LIST:
		emo_printf("MmiNetwork:E_NM_PREF_PLMN_LIST");
        SEND_EVENT (network_data.network_win, NETWORK_PREFERRED_LIST, 0, para);
		break;
	case E_NM_PPLMN_MEM:
		emo_printf("MmiNetwork:E_NM_PPLMN_MEM");
        SEND_EVENT (network_data.network_win, NETWORK_PPLMN_MEM_LIST, 0, para);
		break;
	/* SH 29-04-02 - Cell reselection */
	case E_NM_CELL_SELECT:
		emo_printf("MmiNetwork:E_NM_CELL_SELECT");
		SEND_EVENT (network_data.network_win, NETWORK_CELL_SELECT, 0, para);
		break;
	/* SPR919 - SH - Band selection */
	case E_NM_BAND:
		emo_printf("MmiNetwork:E_NM_BAND");
		nm_registration (nm_get_mode (),0, 0);
		//Sep 23, 2004        REF: CRR 24562  xvilliva
		//After retrieving the list we see if an error occured 
		//and flash an alert to the user accordingly.
		if(g_ensErr == 1)
		{
			g_ensErr = 0;
			info_screen(0, TxtNotAllowed, TxtNull, NULL);
		}
		break;

	//x0pleela 30 May, 2007 DR: OMAPS00133201
	case E_NM_ABORT_NETWORK:
		emo_printf("MmiNetwork:E_NM_ABORT_NETWORK");
		nm_abort_network();
		break;
/*Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)*/
	case E_NM_PLMN_LIST_BOOTUP:
		emo_printf("MmiNetwork:E_NM_PLMN_LIST_BOOTUP");
		 	/*Start the available plmn search*/
  	nm_avail_plmn_list();
		/*After retrieving the list we see if an error occured 
		and flash an alert to the user accordingly.*/
		if(g_ensErr == 1)
		{
			g_ensErr = 0;
			info_screen(0, TxtNotAllowed, TxtNull, NULL);
		 }
	 	/* remove network log if needed */
  		network_remove_network_log ();
   		network_start_animation ();
	    	network_data.aborted = FALSE;
 
		break;

    // June 02, 2006    DR: OMAPS00078005 x0021334
    // Send NETWORK_TIME_UPDATE event to update the new network time/date
    //Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
    case E_MFW_TIME_IND:
        emo_printf("E_MFW_TIME_IND");
		SEND_EVENT (network_data.network_win, NETWORK_TIME_UPDATE, 0, para);
        break;        
#endif
    default:
  	  return 0;
  }
  return 1;
}

/*******************************************************************************

 $Function:    	nm_flag_log_off

 $Description:	 Attribut function for menu entries.
    	       It defines whether the network log off function shall
	            be displayed or not.

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
USHORT nm_flag_log_off (T_MFW_MNU * m, T_MFW_MNU_ATTR * ma, T_MFW_MNU_ITEM * mi)
{
  if (FFS_flashData.net_log_status EQ NM_LOG_AVAIL_USED)
    return 0;

  return MNU_ITEM_HIDE;
}
/*******************************************************************************

 $Function:    	nm_flag_log_off

 $Description:	 Attribut function for menu entries.
    	       It defines whether the network log off function shall
	            be displayed or not.

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
USHORT nm_flag_log_on (T_MFW_MNU * m, T_MFW_MNU_ATTR * ma, T_MFW_MNU_ITEM * mi)
{
  if (FFS_flashData.network_name == NULL)
  {
    return MNU_ITEM_HIDE;
  }
  if (FFS_flashData.net_log_status EQ NM_LOG_AVAIL_NOT_USED)
    return 0;

  return MNU_ITEM_HIDE;
}

/*******************************************************************************

 $Function:    	network_set_mode_auto

 $Description:	 Start a automatic registration
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int network_set_mode_auto(T_MFW_MNU *m, T_MFW_MNU_ITEM *i)
{
  emo_printf ("network_set_mode_auto()");
  
  /*
   * Save the automatic mode in PCM for next power on
   */
  nm_set_mode(NM_AUTOMATIC);

  /*
   * Start the registration of full service
   */
  nm_registration (NM_AUTOMATIC,0, FALSE);

  /*
   * remove network log if needed
   */
  network_remove_network_log ();

  /*
   * Remove Main Menu
   */
  //JVJEmenuExit();

  network_start_animation ();
//  Apr 14, 2004        REF: CRR 12641  xpradipg
	Auto_Network_Select=true;
  return 1;
}
/*******************************************************************************

 $Function:    	network_set_mode_man

 $Description:	 Start a manual registration
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int network_set_mode_man(T_MFW_MNU *m, T_MFW_MNU_ITEM *i)
{
  emo_printf ("network_set_mode_man()");

  /*
   * Save the automatic mode in PCM for next power on
   */
  nm_set_mode(NM_MANUAL);

  /*
   * Start the available plmn search
   */
  nm_avail_plmn_list();
	//Sep 23, 2004        REF: CRR 24562  xvilliva
	//After retrieving the list we see if an error occured 
	//and flash an alert to the user accordingly.
	if(g_ensErr == 1)
	{
		g_ensErr = 0;
		info_screen(0, TxtNotAllowed, TxtNull, NULL);
		  return 1;
	}

  /*
   * remove network log if needed
   */
  network_remove_network_log ();

  /*
   * Remove Main Menu
   */
  //nm menuExit();

  network_start_animation ();

  network_data.aborted = FALSE;
  return 1;
}

 /*
+--------------------------------------------------------------------+
| PROJECT : MMI       (6366)	     MODULE  : MmiNetwork            |
| STATE   : code                     ROUTINE : network_pref_list     |
+--------------------------------------------------------------------+

  PURPOSE : Show the preferred list

*/
int network_pref_list(T_MFW_MNU *m, T_MFW_MNU_ITEM *mi)
{
//emo_printf ("network_pref_list()");

  /*
   * Request the maximum number of preferred PLMN records 
   * on the SIM card
   */
	nm_pplmn_mem_req();


  
  return 1;
}


/*******************************************************************************

 $Function:    	network_remove_network_log

 $Description:	Reset network log if set.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_remove_network_log (void)
{

  if (FFS_flashData.net_log_status EQ NM_LOG_AVAIL_USED)
  {
    /*
     * Until now the network log has been used.
     * Now write to FFS
     */
    FFS_flashData.net_log_status = NM_LOG_AVAIL_NOT_USED;
    flash_write();
   
  }
}

/*******************************************************************************

 $Function:    	network_log_off

 $Description:	Removes a network log and returns to automatic registration.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int network_log_off(T_MFW_MNU *m, T_MFW_MNU_ITEM *i)
{
  emo_printf ("network_log_off()");

  /*
   * Same behaviour as automatic registration
   */
  network_set_mode_auto (m,i);


  network_start_animation ();

  return 1;
}

/*******************************************************************************

 $Function:    	network_log_on

 $Description:	 Sets a network log.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
int network_log_on(T_MFW_MNU *m, T_MFW_MNU_ITEM *i)
{
 

  emo_printf ("network_log_on()");


  if (FFS_flashData.net_log_status EQ NM_LOG_AVAIL_NOT_USED OR
     FFS_flashData.net_log_status EQ NM_LOG_AVAIL_USED)
  {
    /*
     * Until now the network log has been not used.
     * Now set the PCM.
     */
    FFS_flashData.net_log_status = NM_LOG_AVAIL_USED;
    memcpy (FFS_flashData.plmn_name, network_data.plmn_name, SIZE_PLMN_NAME);
    memcpy (FFS_flashData.network_name, network_data.network_name, SIZE_NETWORK_NAME);

    
	flash_write();
    /*
     * Save the manual mode in PCM for next power on
     */
    nm_set_mode(NM_MANUAL);

    /*
     * Start the registration of the logged network
     */
    nm_select_plmn (network_data.network_name);
	//Sep 23, 2004        REF: CRR 24562  xvilliva
	//After retrieving the list we see if an error occured 
	//and flash an alert to the user accordingly.
  	if(g_ensErr == 1)
	{
		g_ensErr = 0;
		info_screen(0, TxtNotAllowed, TxtNull, NULL);
	}
  }


  network_start_animation ();

  return 1;
}

/*******************************************************************************

 $Function:    	network_start_full_service

 $Description: Starts a full service registration.
              a) manual selection
              b) automatic selection
              c) network log selection
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_start_full_service(void)
{
 
  emo_printf("network_start_full_service()");

  network_data.status = NETWORK_SEARCH_NETWORK;

 if(FFS_flashData.plmn_name == NULL) //if no network data
 {   /*
     * Start normal Registration
     */
//TISH modified for MSIM
#if defined (_WIN32_)
    EXTERN UBYTE mmi_reg_mode;

    if (mmi_reg_mode EQ 0)
      nm_registration (NM_AUTOMATIC,0, 0);
    else
      nm_registration (NM_MANUAL,0, 0);
#else
    if (nm_get_mode() == NM_MANUAL)
         SEND_EVENT (idle_get_window(), NETWORK_SEARCH_NETWORK, 0, 0);
                 nm_registration (nm_get_mode (),0, 0);
#endif
  }
  else
  {
    switch (FFS_flashData.net_log_status)
    {
      case NM_LOG_AVAIL_USED:
        memcpy (network_data.plmn_name, FFS_flashData.plmn_name, SIZE_PLMN_NAME);
        memcpy (network_data.network_name, FFS_flashData.network_name, SIZE_NETWORK_NAME);
        nm_select_plmn (network_data.network_name);
        break;
      default:
	    if (nm_get_mode() == NM_MANUAL)
	         SEND_EVENT (idle_get_window(), NETWORK_SEARCH_NETWORK, 0, 0);
        nm_registration (nm_get_mode (),0, 0);
        break;
    }
  }
	//Sep 23, 2004        REF: CRR 24562  xvilliva
	//After retrieving the list we see if an error occured 
	//and flash an alert to the user accordingly.
	if(g_ensErr == 1)
	{
		g_ensErr = 0;
		info_screen(0, TxtNotAllowed, TxtNull, NULL);
	}
}

/*******************************************************************************

 $Function:    	network_get_name

 $Description:	 Returns the textual and numeric representation of the
    	         network name.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_get_name(T_CURRENT_NETWORK * nm)
{
#ifdef NO_ASCIIZ
    T_MFW_SP_NAME service_provider_name;
#else
    U8 service_provider_name[LONG_NAME];
#endif
  emo_printf ("network_get_name()");

//x0pleela 15 Feb, 2006   ER: OMAPS00065203
//Included flag NEPTUNE_BOARD to retain old code for operator get name and 
//added new code for this ER also 

#ifndef NEPTUNE_BOARD

  //x0pleela 09 Feb, 2006 ER: OMAPS00065203
  //Removed the code as prioritized operator name is already updated to network_data
  // and CPHS check is not required

  //x0pleela 09 Feb, 2006 ER: OMAPS00065203
  //Updating prioritized operator name 
  memset(  nm->plmn_name, 0, SIZE_PLMN_NAME );
  memset(  nm->network_name, 0, SIZE_NETWORK_NAME );
  memcpy (nm->plmn_name, network_data.plmn_name, SIZE_PLMN_NAME);
  memcpy (nm->network_name, network_data.network_name, SIZE_NETWORK_NAME);

  memset(  nm->service_provider_name, 0, SP_NAME );
 #else
   if (CphsPresent() )//if CPHS supported
  {	
      T_MFW_CPHS_OP_NAME* operator_name;
      operator_name = CphsRequestOperatorName(); //get operator name
  		
      if (operator_name->longName.data[0] != NULL)//if name not blank
      {	
        memcpy(nm->plmn_name, operator_name->longName.data, SIZE_PLMN_NAME);
        emo_printf("Reading PLMN from SIM");    	
      }
      else
        memcpy (nm->plmn_name, network_data.plmn_name, SIZE_PLMN_NAME);
      
      if (operator_name->shortName.data[0] != NULL)//if name not blank
      {
        memcpy(nm->network_name, operator_name->shortName.data, SIZE_NETWORK_NAME);
      }
      else
        memcpy (nm->network_name, network_data.network_name, SIZE_NETWORK_NAME);
    }
    else
    {
        
      	memcpy (nm->plmn_name, network_data.plmn_name, SIZE_PLMN_NAME);
      	memcpy (nm->network_name, network_data.network_name, SIZE_NETWORK_NAME);
    
    }
	  /* Marcus: Issue 1618: 24/01/2003: Start */
    /* Obtain the service provider name (from the SIM EF-SPN file) if any */
    nm->service_provider_name[0] = '\0';
#endif
#ifdef NO_ASCIIZ
    nm_get_spn(&service_provider_name);
 	if ((service_provider_name.len != 0) &&
 	    (service_provider_name.data[0] != '\0'))
  	{
   	  	if (service_provider_name.len > sizeof(nm->service_provider_name)-1)
   	  	    service_provider_name.len = sizeof(nm->service_provider_name)-1;
   	  	
   	  	memcpy(nm->service_provider_name,
   	  	    service_provider_name.data,
   	  	    service_provider_name.len);
   	  	
  	    nm->service_provider_name[service_provider_name.len] = '\0';
  	}
#else
    nm_get_spn(service_provider_name);
  	if (strlen(service_provider_name))
  	{
   	  	strncpy(nm->service_provider_name,
   	  	    service_provider_name,
   	  	    sizeof(nm->service_provider_name)-1);
   	  	
  	   	nm->service_provider_name[sizeof(nm->service_provider_name)-1] = '\0';
    }
#endif
    /* Marcus: Issue 1618: 24/01/2003: End */
    nm->roaming_indicator = network_data.roaming_indicator;
}

/*******************************************************************************

 $Function:    	network_log_enabled

 $Description:	 Checks whether a network log is active or not.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static UBYTE network_log_enabled (void)
{

  emo_printf ("network_log_enabled()");

  if (FFS_flashData.net_log_status EQ NM_LOG_AVAIL_USED)
    return TRUE;

  return FALSE;
}

/*******************************************************************************

 $Function:    	ListMenuData

 $Description:	 Creates a list of available PLMNs for selection.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static ListMenuData * network_create_list (T_network * data, T_MFW_PLMN_LIST * plmn_list)
{
  UBYTE i;

  ListMenuData *list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof (ListMenuData));

  if (list_menu_data EQ 0)
    return NULL;

  /* Allocated memory only for availble number of plmn list count */
  /* OMAPS00061049 - 08-03-2006 */
#ifndef NEPTUNE_BOARD  
  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY (MAX_PLMN * sizeof (T_MFW_MNU_ITEM));  
#else
  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY (plmn_list->count * sizeof (T_MFW_MNU_ITEM));
#endif

  if (list_menu_data->List EQ 0)
    return NULL;

  /*
   * Fill Menu List
   */
#ifdef FF_2TO1_PS
  for (i=0;i<MMREG_MAX_PLMN_ID;i++)
#else
  for (i=0;i<MAX_PLMN_ID;i++)
#endif
  {
#ifndef NEPTUNE_BOARD  
       /* Taken this statement inside the condition */
	/* OMAPS00061049 - 08-03-2006 */   
       mnuInitDataItem(&list_menu_data->List[i]); 
#endif

    if (i < plmn_list->count)
    {
#ifdef NEPTUNE_BOARD
       mnuInitDataItem(&list_menu_data->List[i]);
#endif
	
      /*
       * set network name
       */
#ifdef NO_ASCIIZ
      if (plmn_list->plmn[i].network_long.len NEQ 0)
      {
            list_menu_data->List[i].str = (char *)plmn_list->plmn[i].network_long.data;
      }
#else       
      if (strlen((char *)plmn_list->plmn[i].network_long) NEQ 0)
      {
            list_menu_data->List[i].str = (char *)plmn_list->plmn[i].network_long;
      }
#endif            
      else
      {
         list_menu_data->List[i].str = (char *)plmn_list->plmn[i].network_numeric;
      }

      /*
       * Store numeric network name in any case
       */
      strncpy ((char *)data->network_search_name[i], (char *)plmn_list->plmn[i].network_numeric, 6);


      /*
       * NOTE : add forbidden indicator
       */
      list_menu_data->List[i].flagFunc = item_flag_none;
    }
    else
    {
    }
  }

  /*
   * Fill common parameter for list handling
   */

  list_menu_data->ListLength     = plmn_list->count;
  list_menu_data->ListPosition   = 1;
  list_menu_data->CursorPosition = 1;
  list_menu_data->SnapshotSize   = plmn_list->count;
  list_menu_data->Font           = 0;
  list_menu_data->LeftSoftKey    = TxtSoftSelect;
  list_menu_data->RightSoftKey   = TxtSoftBack;
  // Sep 8, 2004        REF: CRR 16062  xvilliva
  //Though all the key events are monitered, the listsKeyEvent()
  //responds to a few keys which includes KCD_MNUSELECT.
  list_menu_data->KeyEvents      =  KEY_ALL;
  list_menu_data->Reason         = 0;
  list_menu_data->Strings        = TRUE;
  list_menu_data->Attr           = &network_menuAttrib;
  list_menu_data->autoDestroy    = TRUE;
  list_menu_data->listCol = COLOUR_LIST_XX;
  	

  return list_menu_data;
}

/*******************************************************************************

 $Function:    	network_create_pref_list

 $Description:	 Creates a list of preferred PLMNs for selection.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static ListMenuData * network_create_pref_list (T_network * data)
{
  UBYTE i;

  ListMenuData *list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof (ListMenuData));

  if (list_menu_data EQ 0)
  {
    return NULL;
  }

  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY ((data->pplmn_mem.maxRcd) * sizeof (T_MFW_MNU_ITEM));

  if (list_menu_data->List EQ 0)
  {
  
    /* cq12719 If second memory alloc fails Free the memory allocated for  list menu data. 16-03-04 MZ */
	if(NULL != list_menu_data)
	  	  FREE_MEMORY ((void *)list_menu_data, sizeof (ListMenuData));
	
    return NULL;
   }

  emo_printf ("network_create_pref_list ()");



  /*
   * Fill Menu List
   */
  for (i=0; i < (network_data.pplmn_mem.maxRcd); i++)
  {
	mnuInitDataItem(&list_menu_data->List[i]);
 
   /*
    * set network name
    */

#ifdef NO_ASCIIZ
    if (network_data.pplmn[i].network_long.len NEQ 0 )
    {
		list_menu_data->List[i].str = (char *)network_data.pplmn[i].network_long.data;
	}
#else       
    if (strlen((char *)network_data.pplmn[i].network_long) NEQ 0 )
    {
		list_menu_data->List[i].str = (char *)network_data.pplmn[i].network_long;
	}
#endif
    else
    {
      list_menu_data->List[i].str = GET_TEXT(TxtEmpty);
      emo_printf ("network_long == 0");
    }

      list_menu_data->List[i].flagFunc = item_flag_none;

  }
  /*
   * Fill common parameter for list handling
   */


  list_menu_data->ListLength     = network_data.pplmn_mem.maxRcd;
  list_menu_data->ListPosition    = 1;
  list_menu_data->CursorPosition   = 1;
  list_menu_data->SnapshotSize	= network_data.pplmn_mem.maxRcd;
  list_menu_data->Font          = 0;
  list_menu_data->LeftSoftKey   = TxtSoftOptions; /* SPR#2354 - SH - Change 'Select' to 'Options' */
  list_menu_data->RightSoftKey  = TxtSoftBack;
  list_menu_data->KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;
  list_menu_data->Reason         = 0;
  list_menu_data->Strings        = TRUE;
  list_menu_data->Attr           = &network_menuAttrib;
  list_menu_data->autoDestroy    = TRUE; 
  list_menu_data->listCol = COLOUR_LIST_XX;

  return list_menu_data;
}

/*******************************************************************************

 $Function:    	network_create_mobile_list

 $Description:	 
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static ListMenuData * network_create_mobile_list (T_network_OPT * data)
{
  UBYTE i;

  ListMenuData *list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof(ListMenuData));

  if (list_menu_data EQ 0)
  {
  	emo_printf ("failed to create the list");
    return NULL;
  }

  mfwCheckMemoryLeft();
  
  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY (MAX_SHOW_OF_PLMN * sizeof (T_MFW_MNU_ITEM));

  mfwCheckMemoryLeft();

  if (list_menu_data->List EQ 0)
  {
	emo_printf ("failed to create the list");
    return NULL;
  }

  emo_printf ("network_create_mobile_list ()");  



  /*
   * Fill Menu List
   */
  for (i=0; i < MAX_SHOW_OF_PLMN; i++)
  {
	mnuInitDataItem(&list_menu_data->List[i]);
 
   /*
    * set network name
    */

/*SPR 2635 copy set list strings pointing at longname strings*/
 	if (data->pplmn_longnames[i][0] NEQ 0 )
    {
		list_menu_data->List[i].str =data->pplmn_longnames[i];
	}
	else
	    
    {
      list_menu_data->List[i].str = GET_TEXT(TxtEmpty);
	}	

/*SPR 2635, end*/

      list_menu_data->List[i].flagFunc = item_flag_none;

  }
  /*
   * Fill common parameter for list handling
   */


  list_menu_data->ListLength     = MAX_SHOW_OF_PLMN;
  list_menu_data->ListPosition     = 1;
  list_menu_data->CursorPosition  = 1;
  list_menu_data->SnapshotSize   = MAX_SHOW_OF_PLMN;
  list_menu_data->Font           = 0;
  list_menu_data->LeftSoftKey     = TxtSoftSelect;
  list_menu_data->RightSoftKey   = TxtSoftBack;
  list_menu_data->KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;
  list_menu_data->Reason         = 0;
  list_menu_data->Strings        = TRUE;
  list_menu_data->Attr           = &network_menuAttrib;
  list_menu_data->autoDestroy    = TRUE;  /*destroy the menulist after press any sofkey*/
  list_menu_data->listCol = COLOUR_LIST_XX;

  return list_menu_data;
}

#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:    	network_submenu

 $Description:	Creates a submenu for the preferred list. 
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static ListMenuData * network_submenu (T_network * data, T_MFW_NETWORK_STRUCT * plmn_list)
{
  UBYTE i;
 

  ListMenuData *list_menu_data = (ListMenuData *)ALLOC_MEMORY (sizeof (ListMenuData));

  if (list_menu_data EQ 0)
    return NULL;

  list_menu_data->List = (T_MFW_MNU_ITEM *) ALLOC_MEMORY ((NETWORK_MENU_ITEM) * sizeof (T_MFW_MNU_ITEM));
  if (list_menu_data->List EQ 0)
    return NULL;

  /*
   * Fill Menu List
   */
  for (i=0;i<(NETWORK_MENU_ITEM);i++)
  {
	mnuInitDataItem(&list_menu_data->List[i]);
    
   /*
    * set network name
    */
#ifdef NO_ASCIIZ
    if (plmn_list->network_long.len NEQ 0)
		list_menu_data->List[i].str = (char *)plmn_list->network_long.data;
#else 
    if (strlen((char *)plmn_list->network_long) NEQ 0)
		list_menu_data->List[i].str = (char *)plmn_list->network_long;
#endif

    else
    ;
		//strncpy ((char *)list_menu_data->List[i].str,(char *)empty, 6);


    plmn_list ++;

  }
  /*
   * Fill common parameter for list handling
   */
  list_menu_data->ListLength     = data->pplmn_mem.maxRcd;
  list_menu_data->ListPosition   = 1;
  list_menu_data->CursorPosition = 1;
  list_menu_data->SnapshotSize   = data->pplmn_mem.maxRcd;
  list_menu_data->Font           = 0;
  list_menu_data->LeftSoftKey    = TxtSoftSelect;
  list_menu_data->RightSoftKey   = TxtSoftBack;
  list_menu_data->KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;
  list_menu_data->Reason         = NETWORK_PREF_LIST_SELECTED;
  list_menu_data->Strings        = TRUE;
  list_menu_data->Attr           = &network_menuAttrib;
  list_menu_data->listCol = COLOUR_LIST_XX;

  return list_menu_data;
}

#endif

/*******************************************************************************

 $Function:    	network_list_cb

 $Description:	Callback function list dialog.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_list_cb(T_MFW_HND win, ListMenuData * ListData)
{
  emo_printf ("network_list_cb()");

#ifndef NEPTUNE_BOARD
  if (ListData->Reason != LISTS_REASON_BACK)
  {
	SEND_EVENT (win, NETWORK_LIST_SELECT_PLMN, ListData->CursorPosition, NULL);
  }

#else
  if (ListData->Reason EQ LISTS_REASON_SELECT)
  {
	SEND_EVENT (win, NETWORK_LIST_SELECT_PLMN, ListData->CursorPosition, NULL);
	SEND_EVENT (win, NETWORK_LIST_DESTORY, NULL, NULL);
  }

  if ((ListData->Reason EQ LISTS_REASON_BACK) || 
    (ListData->Reason EQ LISTS_REASON_CLEAR)||
    (ListData->Reason EQ LISTS_REASON_HANGUP))

  {
        SEND_EVENT (win, NETWORK_LIST_DESTORY, NULL, NULL);
  }
#endif

}

/*******************************************************************************

 $Function:    	network_pref_list_cb

 $Description:	Callback function pref. list dialog.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_pref_list_cb(T_MFW_HND win, ListMenuData * ListData)
{
  emo_printf ("network_pref_list_cb()");

  if (ListData->Reason EQ LISTS_REASON_SELECT)
  {
     
// call here the MenuOptions with the items "Select from the list"

      FREE_MEMORY ((UBYTE *)network_data.network_list->List, (network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_MNU_ITEM));

      FREE_MEMORY ((UBYTE *)network_data.network_list, sizeof (ListMenuData));
// xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
//Commented FREE_MEMORY for network_data.pplmn as this is required when we enter the option menu.
      //    Nov 02, 2005	REF: omaps00044855   Sumanth Kumar.C
      //    Solution:		Comment network_data.pplmn memory free.
/* 
    // cq12719 Stop the memory leak 17-03-04 MZ.
      FREE_MEMORY ((UBYTE *)network_data.pplmn,(network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_NETWORK_STRUCT));
*/
	//keep the position of the selected PLMN, need it later to update the list
	network_data.Listposition = ListData->ListPosition;

	network_MNU_start(win,&network_OPTAttrib);
  }
  else
  {
	  
	  //user pressed Exit to go back to the network submenu
	  FREE_MEMORY ((UBYTE *)network_data.network_list->List, (network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_MNU_ITEM));

	  FREE_MEMORY ((UBYTE *)network_data.network_list, sizeof (ListMenuData));
	  
	  FREE_MEMORY ((UBYTE *)network_data.pplmn,(network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_NETWORK_STRUCT));

  }

  
}

/*******************************************************************************

 $Function:    	network_mobile_list_cb

 $Description:	Callback function international list dialog.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_mobile_list_cb(T_MFW_HND win, ListMenuData * ListData)
{

	/* UBYTE i;*/ /* Warning Correction */

	T_MFW_PPLMN_MODE mode;

    T_MFW_WIN       * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT    * data     = (T_network_OPT *)win_data->user;
    T_DISPLAY_DATA display_info;
    
	emo_printf ("network_mobile_list_cb()");


    switch (ListData->Reason)
    {
		case LISTS_REASON_SELECT: /*  */
			
			mode =   REPLACE_PREF_PLMN;    /* Overwrite an existing entry  at a given index */
			/*SPR 2635, use new function to replace preferred PLMN using the longname*/
			nm_change_pref_plmn_longname(mode,(network_data.Listposition) +1 , 0, data->pplmn_longnames[ListData->ListPosition]);

			/* SPR#2354 - SH - Now display "Changed".  On callback, redraw network list */
			dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtChanged, TxtNull, COLOUR_STATUS_NETWORKS);
			dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_mobile_list_changed_cb, THREE_SECS, 0 );

		    info_dialog (win, &display_info); 

	    break;
		case LISTS_REASON_BACK: /* back to previous menu */

			emo_printf ("pressed Back-key");

			//API - SPR 2025 - 22/05/03 - De Allocate memory to avoid memory leak and also Crash
			if ( network_data.pplmn != NULL )
			  FREE_MEMORY ((UBYTE *)network_data.pplmn,(network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_NETWORK_STRUCT));

			network_MNU_destroy(data->win);

	    break;
		default:
	    return;
    }


}

/*******************************************************************************

 $Function:    	network_mobile_list_changed_cb

 $Description:	Callback from "Changed" message.  Destroys the list and recreates the
 				updated network list.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void network_mobile_list_changed_cb(T_MFW_HND win , USHORT identifier, UBYTE reason)
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT    * data     = (T_network_OPT *)win_data->user;
    /* T_DISPLAY_DATA display_info; */ /* Warning Correction */
    
	/* destroy the Menu */
    network_MNU_destroy(data->win);

    /* request the pref. PLMN list */
    nm_pref_plmn_list(network_data.pplmn, network_data.pplmn_mem.maxRcd, 1);

    return;
}


/*******************************************************************************

 $Function:    	network_start_animation

 $Description:	Starts the network animation.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_start_animation(void)
{
  T_DISPLAY_DATA display_info;

  emo_printf ("network_start_animation()");

	if (network_data.network_search_win != NULL)
	{
		network_stop_animation();
	}
       /* REF: OMAPS00061046 possible to cancel the searching with right soft key although
        *  the option is not available.
        */
#ifdef NEPTUNE_BOARD
	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtCancel, "Searching for", "Networks", COLOUR_STATUS_NETWORKS);
#else
       dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, "Searching for", "Networks", COLOUR_STATUS_NETWORKS);
#endif
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_animation_cb, FOREVER, KEY_CLEAR | KEY_RIGHT);
  /*
   * Call Info Screen
   */

  network_data.network_search_win = info_dialog (network_data.network_win, &display_info);
}


/*******************************************************************************

 $Function:    	network_start_animation_plmn_selection

 $Description:	Starts the plmn selection animation.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_start_animation_plmn_selection(int plmn_value)
{
  T_DISPLAY_DATA display_info;
/*  T_MFW_PLMN_LIST * plmn_list; */  /* Warning Correction */

  emo_printf ("network_start_animation()");

	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, "Requesting....", NULL, COLOUR_STATUS_NETWORKS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_animation_cb, FOREVER, KEY_CLEAR| KEY_RIGHT );
	
  /*
   * Call Info Screen
   */

  network_data.network_search_win = info_dialog (network_data.network_win, &display_info);
}


/*******************************************************************************

 $Function:    	network_forbidden_plmn

 $Description:	Starts the forbidden plmn data screen.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_forbidden_plmn(void)
{
  T_DISPLAY_DATA display_info;

  emo_printf ("network_forbidden_plmn()");

	if (network_data.network_search_win != NULL)
	{
		network_stop_animation();
	}

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNetwork, TxtForbidden, COLOUR_STATUS_NETWORKS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_animation_cb, 3500, KEY_CLEAR| KEY_RIGHT );
  /*
   * Call Info Screen
   */
//Feb 08,2007    DRT: OMAPS00115060 x00061088(Prachi)
  network_data.network_search_win = info_dialog (0, &display_info);
}


/*******************************************************************************

 $Function:    	network_selected

 $Description:	displays info screen on network selected
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_selected(void)
{
  T_DISPLAY_DATA display_info;

  emo_printf ("network_start_animation()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtNetwork, TxtSelected, COLOUR_STATUS_NETWORKS);
	// Jul 21, 2004        REF: CRR 13348  xvilliva
	// We use a new callback to return to idle screen after the alert is flashed.
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_select_animation_cb, 3500, KEY_CLEAR| KEY_RIGHT );

  /*
   * Call Info Screen
   */

  network_data.network_search_win = info_dialog (network_data.network_win, &display_info);
}

/*******************************************************************************

 $Function:    	network_no_plmns

 $Description:	 Indicate Info Screen that the network list is empty.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_no_plmns(void)
{
  T_DISPLAY_DATA display_info;

  emo_printf ("network_no_plmns()");

	dlg_initDisplayData_TextStr( &display_info, TxtNull, TxtNull, "No networks", "available", COLOUR_STATUS_NETWORKS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_animation_cb, THREE_SECS, KEY_CLEAR| KEY_RIGHT );
  /*
   * Call Info Screen
   */
  info_dialog (0, &display_info);

}
/*******************************************************************************

 $Function:    	network_stop_animation

 $Description:	 Stops the network animation.
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_stop_animation(void)
{
  emo_printf ("network_stop_animation()");

  if (network_data.network_search_win)
  {
	SEND_EVENT(network_data.network_search_win,DIALOG_DESTROY,0,0);
    network_data.network_search_win = 0;
  }
}
/*******************************************************************************

 $Function:    	network_animation_cb

 $Description:	Callback for information dialog (not used)n
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_animation_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
  emo_printf ("network_animation_cb()");

  network_data.network_search_win = 0;
  network_data.aborted = TRUE;
}

/*******************************************************************************

 $Function:    	network_select_animation_cb

 $Description:	Callback for information dialog "Network Selected".
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_select_animation_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	emo_printf ("network_select_animation_cb()");

	network_data.network_search_win = 0;
	network_data.aborted = TRUE;
	// Jul 21, 2004        REF: CRR 13348  xvilliva
	// If "gPhbkMenu" is not NULL, we destroy phonebook menu 
	// to return to idle screen.
	if(gPhbkMenu != NULL)
	{
		bookPhonebookDestroy( gPhbkMenu );
	}
}

/*******************************************************************************

 $Function:    	network_MNU_start

 $Description:	called when the user press the Edit Softkey to enter the 
                Option Menu "Select from the list" & "Enter the Network Code" &
                            "Delete"
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

T_MFW_HND network_MNU_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
    T_MFW_HND win;

    emo_printf ("network_MNU_start()");

    win = network_MNU_create (parent_window);
    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, NULL, (void *)menuAttr);
	}
    return win;
}

/*******************************************************************************

 $Function:    	network_MNU_create

 $Description:	Create the Option window
 
 $Returns:		mfw window handle

 $Arguments:	parent_window -parent window handle
 			
*******************************************************************************/

static T_MFW_HND network_MNU_create(MfwHnd parent_window)
{
    T_network_OPT   * data = (T_network_OPT *)ALLOC_MEMORY (sizeof (T_network_OPT));
    T_MFW_WIN       * win;

    emo_printf ("network_MNU_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)network_MNU_win_cb);
    if (data->win EQ NULL)
	{
	    return NULL;
	}
  
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)network_MNU_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    return data->win;
}

/*******************************************************************************

 $Function:    	network_MNU_destroy

 $Description:	Destroy the Option window
 
 $Returns:		mfw window handle

 $Arguments:	own_window - current window
 			
*******************************************************************************/

static void network_MNU_destroy(MfwHnd own_window)
{
    T_MFW_WIN       * win_data;
    T_network_OPT   * data;
    /*   T_MFW_PPLMN_MEM *pplmn_mem;  */ /* Warning Correction */

    emo_printf ("network_MNU_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data     = (T_network_OPT *)win_data->user;
		
	    if (data)
		{
			kbd_delete(data->kbd);
			kbd_delete(data->kbd_long);
			mnuDelete (data->menu);
			/*
		     * Delete WIN handler
		     */ 
		       win_delete (data->win); 
			emo_printf("END of WIN DELETE");
			
			if(data->network_list != NULL)
			{    
				emo_printf("START FREE MEMORY"); 
				
				/* Free the allocated memory for PLMN list window - OMAPS00061049 - 08-03-2006 */ 
#ifndef NEPTUNE_BOARD
				FREE_MEMORY ((UBYTE *)data->network_list->List, MAX_SHOW_OF_PLMN* sizeof (T_MFW_MNU_ITEM)); 
#else
				FREE_MEMORY ((UBYTE *)data->network_list->List, Plmn_list_count * sizeof (T_MFW_MNU_ITEM));
#endif

				FREE_MEMORY ((UBYTE *)data->network_list, sizeof (ListMenuData));

				/*SPR 2635, free list of pointers to longname atrings*/
				FREE_MEMORY ((UBYTE *)data->pplmn_longnames,MAX_SHOW_OF_PLMN* sizeof (char*));
				
				emo_printf("END of FREE MEMORY");
			}
	
		    /*     
		     * Free Memory
		     */
		     
		    FREE_MEMORY ((void *)data, sizeof (T_network_OPT));
		}
	    else
		{
		    emo_printf ("network_MNU_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	network_MNU_exec_cb

 $Description:	Exec callback function for the Option window
 
 $Returns:		none

 $Arguments:	win - current window
 				event - event id
 				value - unique id
 				parameter - optional data.
 			
*******************************************************************************/

void network_MNU_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
    T_network_OPT    * data     = (T_network_OPT *)win_data->user;
  /*  T_MFW_HND        * l_parent;  */ /* Warning Correction */
  /*  SHORT            l_id;  */ /* Warning Correction */
  /*  T_MFW_HDR        * parent      =  data->parent;  */ /* Warning Correction */
 /*	T_MFW_EVENT         MfwEvents; */  /* Warning Correction */
	
    emo_printf ("network_MNU_exec_cb()");


    switch (event)
	{
	case E_INIT:
		data->kbd     =   kbdCreate(data->win,KEY_ALL,         (MfwCb)network_MNU_kbd_cb);
    	data->kbd_long =   kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)network_MNU_kbd_cb);
    	data->menu    =   mnuCreate(data->win,&network_OPTAttrib, E_MNU_ESCAPE, (MfwCb)network_MNU_mnu_cb);
    	mnuLang(data->menu,mainMmiLng);

    	/* put the (new) dialog window on top of the window stack */
    	mnuUnhide(data->menu);
	    winShow(win);
	    
	    break;
	default:
	    return;
	}
}

/*******************************************************************************

 $Function:    	network_MNU_mnu_cb

 $Description:	Menu callback function for the Option window
 
 $Returns:		none

 $Arguments:	e - event id
 				m - menu handle
*******************************************************************************/

static int network_MNU_mnu_cb (MfwEvt e, MfwMnu *m)
{
    T_MFW_HND        win      = mfwParent(mfw_header());
    T_MFW_WIN      * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT   * data     = (T_network_OPT *)win_data->user;
  /*  T_MFW_HND      * l_parent;  */ /* Warning Correction */
  /*  SHORT            l_id;  */  /* Warning Correction */

    emo_printf ("network_MNU_mnu_cb()");

    switch (e)
        {
	case E_MNU_ESCAPE: /* back to previous menu "pref. network list"*/
	    network_MNU_destroy(data->win);

	 // request MFW to fill up the pref. PLMN list
	 nm_pref_plmn_list(network_data.pplmn, network_data.pplmn_mem.maxRcd, 1);

	    
	    break;
	default: /* in mnuCreate() only E_MNU_ESCAPE has been enabled! */
	    return MFW_EVENT_REJECTED;
        }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	network_MNU_win_cb

 $Description:	Window callback function for the Option window
 
 $Returns:		execution status

 $Arguments:	e - event id
 				w - window handle 
*******************************************************************************/

static int network_MNU_win_cb (MfwEvt e, MfwWin *w)
{
    emo_printf ("network_MNU_win_cb()");

    switch (e)
	{
        case MfwWinVisible:  /* window is visible  */
	    displaySoftKeys(TxtSoftSelect,TxtSoftBack);
	    break;
	case MfwWinFocussed: /* input focus / selected   */
	case MfwWinDelete:   /* window will be deleted   */
        default:
	    return MFW_EVENT_REJECTED;
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	network_MNU_kbd_cb

 $Description:	Exec callback function for the Option window
 
 $Returns:		execution status

 $Arguments:	e - event id
 				k - keyboard info
*******************************************************************************/

static int network_MNU_kbd_cb (MfwEvt e, MfwKbd *k)
{
    T_MFW_HND            win      = mfwParent(mfw_header());
    T_MFW_WIN          * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT    * data       = (T_network_OPT *)win_data->user;

    emo_printf ("network_MNU_kbd_cb()");

    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_REJECTED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	    switch (k->code)
		{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->menu);
		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->menu);
		    break;
		case KCD_LEFT: /* activate this entry */
		    mnuSelect(data->menu);
		    break;
		case KCD_HUP: /* back to previous menu */
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	network_request_plnm_list

 $Description:	Request existing network list in Mobile
 
 $Returns:		

 $Arguments:	


 *******************************************************************************/
static int network_request_plnm_list(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND          win      = mfwParent(mfw_header());
  /*  T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;  */ /* Warning Correction */
/*    T_network_OPT    * data     = (T_network_OPT *)win_data->user; */  /* Warning Correction */
	T_DISPLAY_DATA display_info;
	
	emo_printf("network_request_plnm_list");

	/* SPR#2354 - SH - Display "Choose Replacement" dialogue,
	 * so the function of the second list is more evident */

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtChooseReplacement1, TxtChooseReplacement2, COLOUR_STATUS_NETWORKS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)network_request_plmn_list_cb, TWO_SECS, 0 );

    info_dialog (win, &display_info); 

    return 1;
}

/*******************************************************************************

 $Function:    	network_request_plnm_list_cb

 $Description:	User has been advised to "choose replacement", now display the list!
 
 $Returns:		None

 $Arguments:	Usual info dialogue callback arguments.

*******************************************************************************/

static void network_request_plmn_list_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT    * data     = (T_network_OPT *)win_data->user;
	/*SPR 2635, just alloc pointers to strings */
 	data->pplmn_longnames = (char**)ALLOC_MEMORY ((MAX_SHOW_OF_PLMN) * sizeof (char*));

	if (data->pplmn_longnames NEQ 0)
	{
		//now fill up the "data->pplmn_longnames" with the PLMN list from ACI
		nm_plnm_list_longnames(data->pplmn_longnames, 1, MAX_SHOW_OF_PLMN);

	}
	/*SPR 2635 end*/
	else
	{
		emo_printf("create the list failed");
		return;
	}

	//show the list on the screen now
	data->network_list = network_create_mobile_list (data);
    listDisplayListMenu (data->win, data->network_list,(ListCbFunc)network_mobile_list_cb,0);

    return;
}


/*******************************************************************************

 $Function:    	network_delete

 $Description:	
 
 $Returns:		

 $Arguments:	


 *******************************************************************************/

static int network_delete(MfwMnu* m, MfwMnuItem* i)
{

    T_MFW_HND win      = mfwParent(mfw_header());

	emo_printf ("network_delete()");

#ifdef NO_ASCIIZ
	if (network_data.pplmn[network_data.Listposition].network_short.len NEQ 0 )
#else 
	if (strlen((char *)network_data.pplmn[network_data.Listposition].network_short) NEQ 0 )
#endif	
	{
		// if the user selected a pref. PLMN
		// xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
		//Made network_info_screen as a generic function as it can be reused to display different messgaes to the user
		//Hence need to pass the callback funtion as a parameter
		network_info_screen (win, TxtDeleted,(T_VOID_FUNC)network_delete_pref_plnm);
	}
	else
	{
		// if the user selected a empty item
		// xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
		//Made network_info_screen as a generic function as it can be reused to display different messgaes to the user
		//Hence need to pass the callback funtion as a parameter
		network_info_screen (win, TxtEmptyList,(T_VOID_FUNC)network_delete_pref_plnm);
	}

	// destroy the Menu 
	network_MNU_destroy(win);

      /* Warning Correction - BEGIN */
	return 1;
      /* END */
	  

}

/*******************************************************************************

 $Function:    	network_delete_pref_plnm

 $Description:	Delete the selected pref. PLMN l
 
 $Returns:		

 $Arguments:	


 *******************************************************************************/
 void network_delete_pref_plnm (T_MFW_HND win , USHORT identifier, UBYTE reason)
{

	T_MFW_PPLMN_MODE mode;
	UBYTE i;
  /*  T_MFW_WIN       * win_data = ((T_MFW_HDR *)win)->data;         */  /* Warning Correction */
  /*  T_network_OPT    * data     = (T_network_OPT *)win_data->user; */ /* Warning Correction */

	emo_printf ("network_delete_pref_plnm()");

  
	mode =   DELETE_PREF_PLMN;      /* Delete an entry from the list  */

	//update now the pref. PLMN list
	//(network_data.Listposition) "+1" because the zero is counted as well
	nm_change_pref_plmn(mode, (network_data.Listposition) +1 , 0, 0);

	// clear all the PLMN and fill up with updated list
	for (i=0; i < network_data.pplmn_mem.maxRcd; i++)
	{

#ifdef NO_ASCIIZ
		network_data.pplmn[i].network_long.len = 0;
		network_data.pplmn[i].network_short.len = 0;
#else 
		memset(network_data.pplmn[i].network_long,   '\0', LONG_NAME);
		memset(network_data.pplmn[i].network_short,   '\0', SHORT_NAME);
#endif		
		memset(network_data.pplmn[i].network_numeric, '\0', NUMERIC_LENGTH);
	}


    // request the updated pref. PLMN list
	nm_pref_plmn_list(network_data.pplmn, network_data.pplmn_mem.maxRcd, 1);

}

/*******************************************************************************

 $Function:    	network_info_screen

 $Description:	confirm the deleted PLMN
 
 $Returns:		

 $Arguments:	

// xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
//Made this as a generic function so that it can be reused to display different messgaes to the user
//Added the call back function pointer as the argument.
 *******************************************************************************/
void network_info_screen (T_MFW_HND win, USHORT MessageId, T_VOID_FUNC funt_ptr )
{
    T_DISPLAY_DATA display_info;

    emo_printf ("network_info_screen()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, MessageId, TxtNull, COLOUR_STATUS_NETWORKS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)funt_ptr, THREE_SECS, 0 );

    info_dialog (win, &display_info); 
}


/*******************************************************************************

 $Function:    	network_OPT_EDT_start

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
T_MFW_HND network_EDT_start (MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND   win;
	T_MFW_HND   parent_window   = mfwParent(mfw_header());

    emo_printf ("network_OPT_EDT_start()");

    win = network_EDT_create (parent_window);

    if (win NEQ NULL)
	{
	    SEND_EVENT (win, E_INIT, 0, 0);
	}
    return win;
}

/*******************************************************************************

 $Function:    	network_OPT_EDT_create

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static T_MFW_HND network_EDT_create(MfwHnd parent_window)
{
    T_network_OPT  * data = (T_network_OPT *)ALLOC_MEMORY (sizeof (T_network_OPT));
    T_MFW_WIN     * win;

    emo_printf ("network_EDT_create()");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, 0, NULL);
    
    if (data->win EQ NULL)
	{
	    return NULL;
	}

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)network_EDT_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */

    winShow(data->win);
    return data->win;
}


/*******************************************************************************

 $Function:    	network_EDT_destroy

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_EDT_destroy(MfwHnd own_window)
{
    T_MFW_WIN        * win_data;
    T_network_OPT     * data;


    emo_printf ("networkT_EDT_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data     = (T_network_OPT *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
		    win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data, sizeof (T_network_OPT));
		}
	    else
		{
		    emo_printf ("network_EDT_destroy() called twice");
		}
	}
}


/*******************************************************************************

 $Function:    	network_EDT_exec_cb

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void network_EDT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *) win)->data;
    T_network_OPT    * data     = (T_network_OPT *)win_data->user;
  /*  T_MFW_HND      * l_parent; */  /* Warning Correction */
  /*  SHORT             l_id; */ /* Warning Correction */
  /*  int                index; */  /* Warning Correction */

    emo_printf ("network_EDT_exec_cb()");

    switch (event)
	{
	case E_INIT:
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		AUI_edit_SetDefault(&data->editor_info);
		AUI_edit_SetDisplay(&data->editor_info, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
		AUI_edit_SetEvents(&data->editor_info, value, TRUE, FOREVER, (T_AUI_EDIT_CB)network_EDT_editor_cb);
		AUI_edit_SetTextStr(&data->editor_info, TxtSoftOK, TxtDelete, TxtMCCMNC, NULL);
		AUI_edit_SetMode(&data->editor_info, 0, ED_CURSOR_UNDERLINE);
		AUI_edit_SetAltTextStr(&data->editor_info, 5, TxtNull, TRUE, TxtSoftBack);
		AUI_edit_SetBuffer(&data->editor_info, ATB_DCS_ASCII, (UBYTE *)data->buffer, 6);

		AUI_edit_Start(data->win, &data->editor_info);
#else /* NEW_EDITOR */
	    data->editor_info.editor_attr.win.px   = 6;
	    data->editor_info.editor_attr.win.py   = /*12*/Mmi_layout_line(2);
	    data->editor_info.editor_attr.win.sx   = /*80*/SCREEN_SIZE_X;
	    data->editor_info.editor_attr.win.sy   = /*20 */Mmi_layout_line_height()*2;
	    data->editor_info.editor_attr.edtCol  = COLOUR_EDITOR_XX;
	    data->editor_info.editor_attr.font     = 0;
	    data->editor_info.editor_attr.mode     = edtCurBar1;
	    data->editor_info.editor_attr.controls = 0;
	    data->editor_info.editor_attr.text     = data->buffer;  /* buffer to be edited */
        data->editor_info.editor_attr.size     = 6;
	    data->editor_info.LeftSoftKey       = TxtSoftOK;	    
	    data->editor_info.RightSoftKey      = TxtDelete;
	    data->editor_info.TextId            = TxtMCCMNC;
	    data->editor_info.Identifier        = value;  /* optional */
	    data->editor_info.mode              = DIGITS_MODE;
	    data->editor_info.Callback          = (T_EDIT_CB)network_EDT_editor_cb;
		data->editor_info.destroyEditor		= TRUE;
	    data->editor_info.hide                 = FALSE;
	    data->editor_info.AlternateLeftSoftKey = TxtNull;
	    data->editor_info.TextString           = NULL;
	    data->editor_info.min_enter            = 5;
	    data->editor_info.timeout              = FOREVER;

	    /* create the dialog handler */
	    editor_start(data->win, &data->editor_info);  /* start the editor */ 
#endif /* NEW_EDITOR */
	   
   
	    break;
	case E_ABORT:
	case E_EXIT: /* defaultreaction */
	    network_EDT_destroy(data->win);
	    break;
	default:
	    return;
	}
}


/*******************************************************************************

 $Function:    	network_change_pref_plmn

 $Description:	Populates the preffered plmn List.

 $Returns:		

 $Arguments:	
 // xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
*******************************************************************************/
 void network_change_pref_plmn (T_MFW_HND win , USHORT identifier, UBYTE reason)
{

	nm_pref_plmn_list(network_data.pplmn, network_data.pplmn_mem.maxRcd, 1);			

}
/*******************************************************************************

 $Function:    	network_EDT_editor_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/
static void network_EDT_editor_cb (T_MFW_HND win, USHORT Identifier, USHORT event)
{
    T_MFW_WIN    * win_data = ((T_MFW_HDR *)win)->data;
    T_network_OPT * data     = (T_network_OPT *)win_data->user;
    T_MFW_HND    * l_parent;
	T_MFW_PPLMN_MODE mode;

    emo_printf ("network_EDT_editor_cb()");


     l_parent = data->parent;

    switch(event)
    	{
			case INFO_KCD_LEFT:
  
			mode =   REPLACE_PREF_PLMN;    /* Overwrite an existing entry  at a given index */

			//update now the pref. PLMN list
			//Overwrite an existing entry
			//(network_data.Listposition) "+1" because the zero is counted as well
			nm_change_pref_plmn(mode, (network_data.Listposition) +1 , 0, (UBYTE*) data->buffer);

			// destroy the Editor 
			network_EDT_destroy(data->win);
			 // xrashmic 10 Nov, 2004 MMI-SPR-OMAPS00044200-OMAPS00044851
			//Added this info screen so that sim is updated by nm_change_pref_plmn with the new code
			//before populating the new list with nm_pref_plmn_list 
			network_info_screen (win, TxtPleaseWait,(T_VOID_FUNC)network_change_pref_plmn);
			// destroy the MENU
			network_MNU_destroy(l_parent);
	    break;
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:
		// destroy the Editor 
	       network_EDT_destroy(data->win);
		// destroy the MENU
		network_MNU_destroy(l_parent);

      //    Nov 02, 2005	REF: omaps00044855   Sumanth Kumar.C
      //    Solution:		Free network_data.pplmn.
		FREE_MEMORY ((UBYTE *)network_data.pplmn,(network_data.pplmn_mem.maxRcd) * sizeof (T_MFW_NETWORK_STRUCT));
		
	    break;
	default:
	    break;
    	}
    return;
}

//x0pleela 30 May, 2007  DR: OMAPS00133201
/*******************************************************************************

 $Function:    	nm_abort_network

 $Description:	remove the searhing window dialog
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void nm_abort_network( void )
{
  emo_printf ("nm_abort_network()");
  //destroy the searching window dialog
  dialog_info_destroy (network_data.network_search_win);
  network_data.network_search_win = 0;
  network_data.aborted = TRUE;
  
  return;
}

 //x0012849: Jagannatha  01 June, 2006  ER: OMAPS00079607 -To Support AcC and Info numbers
/*******************************************************************************

 $Function:    	getRoamingStatus

 $Description: Returns the value of roaming_indicator.

 $Returns:      TRUE or FALSE based on the roaming status.		

 $Arguments:	None.
 
*******************************************************************************/
#ifdef FF_CPHS
UBYTE getRoamingStatus()
{

    return network_data.roaming_indicator;
}  
#endif
 
// June 02, 2006    DR: OMAPS00078005 x0021334
// Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
/*******************************************************************************

 $Function:    	nw_time_update_callback

 $Description:	Call back function for network time update info dialog.

 $Returns:		Result code

 $Arguments:	
 
*******************************************************************************/
//Dec 13,2006       DR:OMAPS00098351  x0066814 - Added the FF_TIMEZONE flag
#ifdef FF_TIMEZONE
static int nw_time_update_callback(T_MFW_HND win, USHORT identifier, UBYTE reason)
{
    int     res = MFW_EVENT_CONSUMED;   // Result code to be returned by this function

    emo_printf("nw_time_update_callback()");

    switch(reason)
    {
        case INFO_KCD_RIGHT:    // Reject offer of network time updation
            emo_printf("INFO_KCD_RIGHT");
            break;

        case INFO_KCD_LEFT: // Accept offer of network time/date updation
            emo_printf("INFO_KCD_LEFT");

            // Update RTC with new time info
            mfw_td_set_time(&newTime);

            // Update RTC with new date info            
            mfw_td_set_date(&newDate);

            // Send IDLE_UPDATE event to force idle screen refresh             
            SEND_EVENT(idle_get_window(), IDLE_UPDATE, 0, 0);
            break;

        case INFO_KCD_HUP:  // Ignore offer of network time updation
            emo_printf("INFO_KCD_HUP");
            // No further action required
            break;

        default:
            emo_printf("Unexpected reason: %d", reason);
            res = MFW_EVENT_REJECTED;
            break;
    }

    return res;
}
#endif

//Feb 28,,2007       DR:OMAPS00098351  x0066814  
 /*******************************************************************************

 $Function:    	settingsNITZ

 $Description:	 Settings the time update to On, Off or Automatic
    	     
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
#ifdef FF_TIMEZONE
int settingsNITZ (T_MFW_MNU *menu, T_MFW_MNU_ITEM *i)
{

 T_MFW_HND parent_win = mfwParent(mfw_header());
 int txtId = TxtAutomatic;

  switch(menu->lCursor[menu->level])
    {
        case 0:
            emo_printf("Menu option 0: Automatic");
		FFS_flashData.nitz_option=NitzAutomatic;
		txtId = TxtAutomatic;
            break;

        case 1:
            emo_printf("Menu option 1: NITZOn");
            FFS_flashData.nitz_option=NitzOn;
		txtId = TxtOn;
            break;

        case 2:
            emo_printf("Menu option 2: NITZOff");
		FFS_flashData.nitz_option=NitzOff;
		txtId = TxtOff;
		break;

       }
  flash_write();
  bookShowInfoScreen(parent_win, txtId, TxtSelected, NULL,NULL,3000);
  return MFW_EVENT_CONSUMED;
}
#endif
