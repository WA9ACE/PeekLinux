/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   PhoneBook
 $File:       MmiBookShared.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    Shared definitions of data elements are made in here
    and accessed via the primitive operations supplied

********************************************************************************  

 $History: MmiBookShared.c

 
	Sep 26, 2007    Configuration of Drives for SBuild 2.3.0 Migration  x0080701 (Bharat)
	Description:   Support from MMI to configure drives (NOR, NORMS, NAND, TFLASH)
	                    is added as part of SBuild 2.3.0 Migration
	
    April 03 2007 ER:OMAPS00116772 x0061088(Prachi)
    Description:Support of 1.3 Mpixel camera on Locosto Plus
    Solution: Auto Save option is added for the Camera Application
 
	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 
	
	Mar 28, 2007   ER: OMAPS00106188  x0039928
	Description: Align Midi application menu choices and audio Player menu choices 
	
	Nov 10, 2006   DRT: OMAPS000103354  x0039928
	Description: Delete all failure in LDN Phonebook
	Solution: Delete all for Recent list is mapped to bookDeleteRec instead of bookDeleteAll.

  	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Added new category "Blocked Network" as menu item in menuCategory and menuBootupCatList
 
      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      
 	xashmic 27 Sep 2006, OMAPS00096389 
	ENH - a) Providing static menu for re-selection of enum options for enumeration
	b) Popup menu displayed on connect event, can be enabled or disabled via menu option

 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list
 	xrashmic 9 Sep 2006, OMAPS00092732
	USBMS ER: Added menu in phone setting to set or unset PS shutdown while enumeration of USB MS
	On USB connect event, a list of possible ports that can be enumerated would be displayed.
 
	Aug 18, 2006   ER: OMAPS00089840  x0039928
	Description: Changes to be made in MMI for the functionality "Delete all" ADN entries
	Solution: A new menu item "Delete All" is added in the phone book menu.
 
	Mar 15, 2006   ER: OMAPS00067709  x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Defined new sub menu items for PCM voice memo and Voice buffering
			Added newly created sub menu items as menu items into "Application" menu
 
	Feb 24, 2006    ER: OMAPS00067709 x0pleela
	Description: Voice Buffering implementation on C+ for PTT via PCM API 
	Solution: Added new menu items for PCM voice memo and Voice buffering
			Added PCM Voice memo and Voice buffering menu items into "Application" menu
			Defined functions GetPcmVoiceMemoMenu, GetVoiceBufferingMenu to populate the 
				PCM Voice memo and Voice buffering menus

	Jun 19, 2006  DR: OMAPS00070657 xdeepadh
	Description: MMI allows to MP3 and MIDI play during an active call
	Solution: From the active call screen, only selected applications can be accessed now.
	
	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security 
	Solution:Made the following changes
			1) Included the header file "MmiPins.h"
			2) Added new menu item "Master unlock" for menu menuMEPersonalization
			3) Defined new menu "menuBootupCatList" to display during bootup for unlocking categories or unblocking ME
			4) New function SIMPCategoryListAttributes(): function to call the new menu "menuBootupCatList" from any file
		
	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Defined a new menu item for the user to customize the strings (line1 and line2)
	
 	Apr 24, 2006    ERT: OMAPS00067603 x0043642
   	Description: Engineering mode
   	Solution:  Added new engineering mode menus to call respective functions.

	Apr 06, 2006    ERT: OMAPS00070660 x0039928(sumanth)
   	Description: Need to reduce flash foot-print for Locosto Lite 
   	Solution: Voice Memo feature is put under the flag #ifndef FF_NO_VOICE_MEMO to compile 
   	out voice memo feature if the above flag is enabled.

 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
	Description: Implementation of Test Menu for AAC
	Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.

	Sep 27,2005 REF: SPR 34402 xdeepadh  
	Bug:Mono option is always highlighted even when Stereo is selected
	Fix:The channel setting will be saved in the FFS, which will be retrieved later
	and the selected channel will be higlighted.
	
	Jul 18, 2005    REF: SPR 31695   xdeepadh
	Bug:Problems with MP3 test application
	Fix:The window handling of MP3 Test Application has been done properly.


	 Aug 22, 2005    REF: ENH 31154 xdeepadh
   	Description: Application to Test Camera
   	Solution: Implemeted the Fileviewer to view the jpeg images.Camera Application to 
   	preview,capture and save image has been implemented.

	July 19, 2005 REF: CRR LOCOSTO-ENH-28173 xpradipg
  	Description: To provide MMI Support to enable/disable/change password and 
  				query all the Personalization locks
  	Solution: Integration of the changes for the same provided by the soldel 
  			  team

    June 07,  2005   REF: CRR MMI-FIX-31545 x0021334
    Issue description: SIM: wrong behavior when FDN is not allocated in SST
    Solution: Before displaying the FDN options, a check is made if FDN is allocated. Only if 
                  FDN is allocated, the options are displayed.
    May 11 2005  REF:  MMI-SPR-29887  x0012849
   To Implement the deferred MMS retrieval.

    May 04, 2005  REF: CRR 30285  x0021334
    Description: SMS Status Request in idle and dedicated mode (using Menu) setting does not work
    Solution: status of sms status request is read and focus is set accrodingly.

   Apr 06, 2005	REF: ENH 30011 xdeepadh
   Description: Replacing the Test Application compilation flags with new flags. 
   Solution:  The existing flags for Camera and MP3 test application have  beeen replaced with the 
   new compilation flags,FF_MMI_TEST_CAMERA and FF_MMI_TEST_MP3 respectively.

   Apr 05, 2005    REF: ENH 29994 xdeepadh
   Description: Implementation of Generic Midi Ringer and Modification of existing Midi Test Application
   Solution: Generic Midi Ringer and Midi Test Application were implemented.
 
   March 2, 2005    REF: CRR 11536 x0018858
   Description: Reply path not supported while sending a reply.
   Solution: Added menu option for the reply path.


   xreddymn Jan-06-2004 MMI-SPR-27618: Display list of WAP push messages before displaying the Options list.

   Dec 08, 2004	REF: CRR MMI-SPR-27284
   Description: MIDI Player: Back Key malfunctions in MIDI application.
   Solution: A window is created to hold list window. When the user presses Back key, the newly 
		    created window is deleted along with list window, bringing the control back to Midi options
		    screen.
 
    Nov 29, 2004    REF: CRR 25051 xkundadu
    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
    Fix: Added volume level list linked to up/down keys. 
          User can select the speaker volume among those levels.

    MMI-SPR-25333 - xreddymn Nov-09-2004
    Modified Messaging menu to seperate SMS and MMS/EMS related features

    CRR: 25291 - xrashmic 14 Oct 2004
    Description: The sent EMS is stored in unsent folder.
    Solution: The Outbox has been removed, instead we now have Sent folder for the sent EMS 

	Jul 29, 2004 REF: CRR 20898 xkundadu
	Description: No identification for user whether 'Auto-Answer' is On/Off
	Solution: The menu focus will be on the 'On' menu  if the 'Auto-Answer'
			is enabled, otherwise focus will be on 'Off' menu item.
			Changed 'submenu' to 'menuItem' to call a function,which set the
			focus to the selected item.
 
  Jun 02, 2004 CRR:13649 - xpradipg-SASKEN
  subject: CBA : Management of password
  solution: password is promted on selecting activation/deactivation option as against the previous
  case where it was shown up on selecting Call
  May 27, 2004    REF: CRR 19186  Deepa M.D 
	Bug	:	"Setting" menu not available during a call
	Fix	:	"Setting" menu has been provided to set the SMS settings, during the active call
   May 10, 2004    REF: CRR 15753  Deepa M.D 
   Menuitem provided to set the validity period to maximum.
	Issue Number : SPR#12822 on 31/03/04 by v vadiraj
	25/10/00      Original Condat(UK) BMI version.


    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY

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
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/
#include "rv_swe.h"
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#endif
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH -New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"

#include "mfw_sim.h"
#include "mfw_cm.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"
#ifdef FF_MMI_TEST_MIDI 
#include "mfw_midi.h"
#endif  //FF_MMI_TEST_MIDI 
//   Apr 06, 2005	REF: ENH 30011 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
#include "mfw_cam.h"
#endif //FF_MMI_TEST_CAMERA

#ifdef FF_MMI_CAMERA_APP
#include "mfw_camapp.h"
#endif //FF_MMI_TEST_CAMERA

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */
#include "mfw_fm.h"
/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"
#include "MmiTimers.h"

/* SPR#1428 - SH -New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif

#include "MmiBookShared.h"
#include "MmiBookController.h"
#include "MmiResources.h"

#include "MmiNetwork.h"
#include "MmiSettings.h"
#include "MmiServices.h"
#include "MmiPins.h"
#include "MmiTimeDate.h"
#include "mmiSmsBroadcast.h"
#include "MmiSimToolkit.h"
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "MmiCalculatorMain.h"


#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
EXTERN int mmi_PowerManagement(MfwMnu* m, MfwMnuItem* i);
#endif
#endif

#include "AUIWapext.h"
#include "MmiSounds.h"
#include "MmiLists.h"
#include "MmiTimers.h"

#ifndef FF_NO_VOICE_MEMO
#include "MmiVoiceMemo.h"
#endif

//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MMI_TEST_MIDI
#include "MmiMidiTest.h"
#endif//#ifdef FF_MMI_TEST_MIDI
//Apr 06, 2005	REF: ENH 30011 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
#include "MmiCameraTest.h"
#endif //FF_MMI_TEST_CAMERA
#ifdef FF_MMI_CAMERA_APP
#include "MmiCameraApp.h"
#endif //FF_MMI_TEST_CAMERA
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
#include "MmiMP3Test.h"
#endif//FF_MMI_TEST_MP3
// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
#include "MmiFileViewer.h"
#endif

#include "Mmiicons.h"
#include "MmiAoc.h"

#if defined (MMI_EM_ENABLED) || defined (TI_PS_FF_EM)
#include "MmiEm.h" /*SPR1209*/
#endif


#include "MmiMenu.h"

/*BEGIN ADD: RAVI*/
#ifdef FF_MMI_MULTIMEDIA
#include "MmiMultimediaApp.h"
#endif
/* END ADD: RAVI*/


#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
#include "mmimmsmenu.h"
#endif

#ifdef MMIGAME
#include "Mmigame.h"
#endif
/* SPR890 - SH - Test mode menu */
#ifdef MMI_TEST_MODE
#include "MmiTest.h"
#endif

#include "MmiCPHS.h"
#include "MmiBand.h"
#ifdef MMI_GPRS_ENABLED
#include "MmiGprs.h"
#endif

#ifdef BTE_MOBILE
#include "MmiBluetooth.h"
#endif

extern int M_exeSendNew(MfwMnu* m, MfwMnuItem* i); // MZ
extern int M_exeRead(MfwMnu* m, MfwMnuItem* i);    // MZ 6/2/01
/*OMAPS00098881(removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
extern int screenUpdateOn(MfwMnu* m, MfwMnuItem* i);
extern int screenUpdateOff(MfwMnu* m, MfwMnuItem* i);
#endif
#include "mmiColours.h"
#include "mmiColours.h" 

#ifdef FF_MMI_FILEMANAGER
#include "mmifilemanager.h"
#endif



extern void soundsSetVolume(void);
extern int SmsSend_R_OPTExeCentreEdit(MfwMnu* m, MfwMnuItem* i);

#ifdef SIM_PERS
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
#include "MmiPins.h"
#endif

/*******************************************************************************

                                Static members

*******************************************************************************/
/*GW Changed all submenus to use the same colour definitions 'COLOUR_LIST_SUBMENU' */



/* menu buffers
*/

/*This should cause an error if the size is changed
  if NAME_LIST_SIZE is not 3 or 10 then the structure is incorrect and must be updated*/
#ifndef LSCREEN
static const int zz_errCatcher[6-NAME_LIST_SIZE] = {1,2,3};
#else
/* x0039928 - Lint warnings removal 
static const int zz_errCatcher[20-NAME_LIST_SIZE] = {1,2,3,4,5,6,7,8,9,10}; */
#endif

/*SPR2123, MC removed static phonebook list menu data structure
and replaced with pointers which are dynamically allocated*/

static MfwMnuAttr*  phonebook_list = NULL;
static MfwMnuItem* items_in_phonebook_list = NULL;
static char** menu_item_strings = NULL;
/*SPR2123, end*/

/* Global text message structure
*/
static int textMessage;

#ifdef MMI_TEST_MODE
EXTERN UBYTE testSelected;
#endif

extern MfwIcnAttr mainIconFrame1	;


/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

static USHORT mmi_check_tflash( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
static USHORT mmi_check_nand( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
static USHORT mmi_check_nor( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
static USHORT mmi_check_norms( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */


/******************************************************************************

                                Menu Definitions

*******************************************************************************/

#define DEF_MENU_STYLE          	(MNU_LEFT | MNU_LIST | MNU_CUR_LINE)
#define DEF_COLOUR_MENU_STYLE   	(MNU_LEFT | MNU_LIST_COLOUR | MNU_CUR_LINE)
#define DEF_MENU_ICON_LIST_STYLE	(MNU_LEFT | MNU_LIST_ICONS | MNU_CUR_LINE)
#define BEGIN_MENU( name ) \
static const MfwMnuItem name##Items [] = {

#define MENU_ITEM_ICON( icon, msg, handler, attrFunction ) \
    { icon,0,0, (char*) msg, 0,                       (MenuFunc)handler, attrFunction }

#define MENU_ITEM( msg, handler, attrFunction ) \
    { 0,0,0,  (char*) msg, 0,                       (MenuFunc)handler, attrFunction }

#define MENU_ITEM_SUB_MENU( msg, sub_menu,handler, attrFunction ) \
    { 0,0,0,  (char*) msg, (MfwMnuAttr*) &sub_menu,(MenuFunc)handler, attrFunction }


#define SUB_MENU( msg, sub_menu, attrFunction ) \
    { 0,0,0, (char*) msg, (MfwMnuAttr*) &sub_menu, 0,                 attrFunction }

#define SUB_MENU_ICON( icon, msg, sub_menu, attrFunction ) \
    { icon,0,0, (char*) msg, (MfwMnuAttr*) &sub_menu, 0,                 attrFunction }

#define END_MENU( name, area, col ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, DEF_MENU_STYLE, (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ),  col, TxtNull, NULL, MNUATTRSPARE \
};

#define END_MENU_MULTICOLOUR( name, area, col, hdrId ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, DEF_COLOUR_MENU_STYLE, (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ),  col, hdrId, NULL, MNUATTRSPARE \
};

#define END_MENU_HEADER( name, area, col, hdrId  ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, DEF_MENU_STYLE, (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ),  col, hdrId , NULL, MNUATTRSPARE\
};

#define END_MENU_ICON_LIST( name, area, col, bitmap ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, DEF_MENU_ICON_LIST_STYLE, (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ),  col, TxtNull, bitmap, MNUATTRSPARE \
};

#define DEF_MENU_PAGED_STYLE          (MNU_LEFT | MNU_HDR_CENTER | MNU_PAGED | MNU_CUR_LINE)
#define BEGIN_ICON_MENU( name ) \
static const MfwMnuItem name##Items [] = {

#define MENU_ICON_ITEM( icon, msg, handler, attrFunction ) \
    { (MfwIcnAttr *)icon,0,0, (char*) msg, 0,                       (MenuFunc)handler, attrFunction }


#define SUB_ICON_MENU( icon,msg, sub_menu, attrFunction ) \
    { (MfwIcnAttr *)icon,0,0, (char*) msg, (MfwMnuAttr*) &sub_menu, 0,                 attrFunction }

#define END_ICON_MENU( name, area, col, bitmap ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, DEF_MENU_PAGED_STYLE, (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ), col, TxtNull , bitmap, MNUATTRSPARE\
};
#define END_MULTIPLE_ICON_MENU( name, area, fCol, bCol ) \
}; \
\
static const MfwMnuAttr name = { \
    (MfwRect *) &area, (MNU_LEFT | MNU_PAGE_ICONS | MNU_CUR_LINE), (UBYTE) -1, (MfwMnuItem *) name##Items, sizeof( name##Items ) / sizeof( MfwMnuItem ), col, TxtNull, NULL, MNUATTRSPARE \
};


/*SPR949 - SH - new macros.  If the first item in a menu is a _SELECT, then the selected
value of this menu will be stored in the provided UBYTE variable 'var'.*/
#define MENU_ITEM_SELECT( msg, handler, attrFunction, var ) \
    { 0,(void *)&var,0, (char *) msg, 0, (MenuFunc)handler, attrFunction }

#define MENU_ITEM_SUB_MENU_SELECT( msg, sub_menu,handler, attrFunction, var ) \
    { 0,(void *)&var,0, (char *) msg, (MfwMnuAttr*) &sub_menu,(MenuFunc)handler, attrFunction }

#define SUB_MENU_SELECT( msg, sub_menu, attrFunction, var ) \
    { 0,(void *)&var,0, (char *) msg, (MfwMnuAttr*) &sub_menu, 0, attrFunction }
/* end SPR949 */

#ifdef MMI_TTY_ENABLED
/* SPR#1352 - SH - TTY menu */
BEGIN_MENU( menuTTY )
	MENU_ITEM( TxtOff, call_tty_menuselect, item_flag_none),
	MENU_ITEM( TxtAlwaysOn, call_tty_menuselect, item_flag_none),
	MENU_ITEM( TxtOnNextCall, call_tty_menuselect, item_flag_none)
END_MENU( menuTTY, menuListArea, COLOUR_LIST_SUBMENU1 )
/*x0039928 OMAPS00097714 HCO/VCO option - menu items added*/
#ifdef FF_TTY_HCO_VCO
BEGIN_MENU( menucoTTYAlways )
	MENU_ITEM( TxtTTYAll, call_tty_co_menuselect_always, item_flag_none),
	MENU_ITEM( TxtTTYVco, call_tty_co_menuselect_always, item_flag_none),
	MENU_ITEM( TxtTTYHco, call_tty_co_menuselect_always, item_flag_none)
END_MENU( menucoTTYAlways, menuListArea, COLOUR_LIST_SUBMENU1 )

BEGIN_MENU( menucoTTYNextCall )
	MENU_ITEM( TxtTTYAll, call_tty_co_menuselect_nextcall, item_flag_none),
	MENU_ITEM( TxtTTYVco, call_tty_co_menuselect_nextcall, item_flag_none),
	MENU_ITEM( TxtTTYHco, call_tty_co_menuselect_nextcall, item_flag_none)
END_MENU( menucoTTYNextCall, menuListArea, COLOUR_LIST_SUBMENU1 )
#endif
#endif


#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
/*Engineering mode menus - MC SPR1209*/
BEGIN_MENU( menuEngMode )
#ifndef NEPTUNE_BOARD
    MENU_ITEM( TxtMobileInfo,    Mmi_em_display_mobile_info_data,   item_flag_none ),
  	MENU_ITEM( TxtServingCell,   Mmi_em_display_serving_cell_params,    item_flag_none ),
    MENU_ITEM( TxtNeighbourCell,    Mmi_em_display_neighbour_cell_params,    item_flag_none ),
    MENU_ITEM( TxtLocation,    Mmi_em_display_location_params,    item_flag_none ),
    MENU_ITEM( TxtCiphHopDTX,    Mmi_em_display_ciph_hop_DTX_params,    item_flag_none ),
     MENU_ITEM( TxtGPRS,    Mmi_em_display_GPRS_params,    item_flag_none )
#else   /* NEPTUNE BOARD */    
    MENU_ITEM( TxtServingCell,   Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtServingCellRF,   Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtServingChannel,   Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtAmrInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtSNDCPInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtUserLevelInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtLLCTracing,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtRlcMacInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtGMMInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtCallStatus,    Mmi_em_start,    item_flag_none ),    
    MENU_ITEM( TxtLayerMessages,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtNeighbourCell,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtWCDMALayerMessages,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( Txt3GInfo,    Mmi_em_start,    item_flag_none ),
    MENU_ITEM( TxtPacketDataParameter,    Mmi_em_start,    item_flag_none ),

#endif /* ifdef NEPTUNE BOARD */
END_MENU( menuEngMode, menuListArea, COLOUR_LIST_SUBMENU )
/*MC end*/
#endif


/* SPR#1112 - SH - Internal phonebook menus */

#ifdef INT_PHONEBOOK

BEGIN_MENU( menuSelectBook )
    MENU_ITEM( TxtSIM, 		bookSelectBookSIM,     	item_flag_none ),
    MENU_ITEM( TxtPhone,     bookSelectBookPhone,        item_flag_none ),
END_MENU( menuSelectBook, menuListArea, COLOUR_LIST_SUBMENU  )

BEGIN_MENU( menuCopy )
    MENU_ITEM( TxtThisEntry,  bookCopyEntry,     	item_flag_none ),
    MENU_ITEM( TxtAll,    	bookCopyAll,     		item_flag_none ),
END_MENU( menuCopy, menuListArea, COLOUR_LIST_SUBMENU  )

BEGIN_MENU( menuMove )
    MENU_ITEM( TxtThisEntry,  bookMoveEntry,     	item_flag_none ),
    MENU_ITEM( TxtAll,    	bookMoveAll,     		item_flag_none ),
END_MENU( menuMove, menuListArea, COLOUR_LIST_SUBMENU  )

#endif

#ifdef FF_WAP

/*
 *  WAP menus
 */
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
    /* Pushed message options sub-menu */
    
    BEGIN_MENU( menuPushListOptions )
      MENU_ITEM( TxtRead,        (MenuFunc)AUI_wap_push_message_read,  item_flag_none ),
      MENU_ITEM( TxtDelete,       (MenuFunc)AUI_wap_push_message_delete,  item_flag_none )
    END_MENU( menuPushListOptions, menuListArea, COLOUR_LIST_SUBMENU1 )
    
    /* Show push messages sub-menu */
    
    BEGIN_MENU( menuPushMessages )
      MENU_ITEM( TxtOff,        (MenuFunc)AUI_wap_push_showmessages_set,  item_flag_none ),
      MENU_ITEM( TxtOn,       (MenuFunc)AUI_wap_push_showmessages_set,  item_flag_none )
    END_MENU( menuPushMessages, menuListArea, COLOUR_LIST_SUBMENU1 )
#endif

// Scale Images sub-menu

BEGIN_MENU( menuScaleImages )
  MENU_ITEM( TxtOff,        (MenuFunc)AUI_scaleimages_set,  item_flag_none ),
  MENU_ITEM( TxtOn,       (MenuFunc)AUI_scaleimages_set,  item_flag_none )
END_MENU( menuScaleImages, menuListArea, COLOUR_LIST_SUBMENU )

// Save History sub-menu

BEGIN_MENU( menuSaveHistory )
  MENU_ITEM( TxtOff,        (MenuFunc)AUI_savehist_set,   item_flag_none ),
  MENU_ITEM( TxtOn,       (MenuFunc)AUI_savehist_set,   item_flag_none )
END_MENU( menuSaveHistory, menuListArea, COLOUR_LIST_SUBMENU )

/* SPR#2324 - SH - GPRS/Dialup submenu */

BEGIN_MENU( menuWAPGPRSDialup )
  MENU_ITEM( TxtGPRS,     (MenuFunc)AUI_access_set, item_flag_none ),
  MENU_ITEM( TxtDialup,     (MenuFunc)AUI_access_set, item_flag_none ),
END_MENU( menuWAPGPRSDialup, menuListArea, COLOUR_LIST_SUBMENU )

// Security sub-menu

BEGIN_MENU( menuSecurityOption )
  MENU_ITEM( TxtOff,        (MenuFunc)AUI_security_set, item_flag_none ),
  MENU_ITEM( TxtOn,       (MenuFunc)AUI_security_set, item_flag_none )
END_MENU( menuSecurityOption, menuListArea, COLOUR_LIST_SUBMENU )

// Connection Type sub-menu

BEGIN_MENU( menuWAPConnectionType )
  MENU_ITEM( TxtTemporary,    (MenuFunc)AUI_connectionType_set,item_flag_none ),
  MENU_ITEM( TxtContinuous,   (MenuFunc)AUI_connectionType_set,item_flag_none )
END_MENU( menuWAPConnectionType, menuListArea, COLOUR_LIST_SUBMENU )

/* SPR#1983 - SH - Connection Speed sub-menu */

BEGIN_MENU( menuWAPConnectionSpeed )
  MENU_ITEM( TxtAnalogue,    (MenuFunc)AUI_connectionSpeed_set,item_flag_none ),
  MENU_ITEM( TxtISDN9600,   (MenuFunc)AUI_connectionSpeed_set,item_flag_none ),
  MENU_ITEM( TxtISDN14400,   (MenuFunc)AUI_connectionSpeed_set,item_flag_none )
END_MENU( menuWAPConnectionSpeed, menuListArea, COLOUR_LIST_SUBMENU1 )

//xpradipg - Aug 4: Changes for WAP 2.0 menus
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
BEGIN_MENU( MenuPPGAuthenticationOptions)
 MENU_ITEM( TxtOff,        (MenuFunc)AUI_PPGAuthentication_set, item_flag_none ),
  MENU_ITEM( TxtOn,       (MenuFunc)AUI_PPGAuthentication_set, item_flag_none )
END_MENU( MenuPPGAuthenticationOptions, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( MenuWirelessProfiledHTTPOptions)
 MENU_ITEM( TxtOff,        (MenuFunc)AUI_WirelessProfiledHTTP_set, item_flag_none ),
  MENU_ITEM( TxtOn,       (MenuFunc)AUI_WirelessProfiledHTTP_set, item_flag_none )
END_MENU( MenuWirelessProfiledHTTPOptions, menuListArea, COLOUR_LIST_SUBMENU )
#endif


/* SPR#2324 - SH - Advanced Profile Options */

BEGIN_MENU( menuAdvancedProfileOptions )
  MENU_ITEM( TxtConnectionType, (MenuFunc)AUI_connectionType,     item_flag_none ),
  MENU_ITEM( TxtSecurity,     (MenuFunc)AUI_security,       item_flag_none ),
  MENU_ITEM( TxtResponseTimer,  (MenuFunc)AUI_response_timer, item_flag_none ),
  MENU_ITEM( TxtIPAddress2,   (MenuFunc)AUI_IPAddress2, item_flag_none ),
  MENU_ITEM( TxtPort1,      (MenuFunc)AUI_Port1,    item_flag_none ),
  MENU_ITEM( TxtPort2,      (MenuFunc)AUI_Port2,    item_flag_none )
 //xpradipg - Aug 4, 2004: changes for WAP 2.0 menus
 #if defined (FF_WAP) && defined (FF_GPF_TCPIP)
  ,
  MENU_ITEM( TxtNameServer1, (MenuFunc)AUI_NameServer1, item_flag_WAP_2_0),
  MENU_ITEM( TxtNameServer2, (MenuFunc)AUI_NameServer2, item_flag_WAP_2_0),
  MENU_ITEM( TxtPPGAuthentication, (MenuFunc)AUI_PPGAuthentication, item_flag_none),
  MENU_ITEM( TxtWirelessProfiledHTTP,(MenuFunc)AUI_WirelessProfiledHTTP,item_flag_WAP_2_0)

 #endif
END_MENU( menuAdvancedProfileOptions, menuListArea, COLOUR_LIST_SUBMENU )

/* SPR#2324 - SH - GPRS Profile Options */

BEGIN_MENU( menuGPRSProfileOptions )
  MENU_ITEM( TxtGPRSDialupGPRS, (MenuFunc)AUI_access, item_flag_none),
  MENU_ITEM( TxtEditName, (MenuFunc)AUI_profile_name_edit, item_flag_none ),
  MENU_ITEM( TxtHomepage, (MenuFunc)AUI_homepage_edit, item_flag_none ),
  MENU_ITEM( TxtISPUsername, (MenuFunc)AUI_username, item_flag_none ),
  MENU_ITEM( TxtISPPassword, (MenuFunc)AUI_password, item_flag_none),
  MENU_ITEM( TxtIPAddress,   (MenuFunc)AUI_IPAddress1, item_flag_none ),
  MENU_ITEM( TxtAPN,        (MenuFunc)AUI_APN,      item_flag_none ),
  SUB_MENU( TxtAdvanced, menuAdvancedProfileOptions, item_flag_none),
END_MENU( menuGPRSProfileOptions, menuListArea, COLOUR_LIST_SUBMENU )

/* SPR#2324 - SH - Dialup Profile Options */

BEGIN_MENU( menuDialupProfileOptions )
#ifdef GPRS /* Only provide option if GPRS is switched on */
  MENU_ITEM( TxtGPRSDialupDialup, (MenuFunc)AUI_access, item_flag_none),
#endif
  MENU_ITEM( TxtEditName, (MenuFunc)AUI_profile_name_edit, item_flag_none ),
  MENU_ITEM( TxtHomepage, (MenuFunc)AUI_homepage_edit, item_flag_none ),
  MENU_ITEM( TxtISPUsername, (MenuFunc)AUI_username, item_flag_none ),
  MENU_ITEM( TxtISPPassword, (MenuFunc)AUI_password, item_flag_none),
  MENU_ITEM( TxtIPAddress,   (MenuFunc)AUI_IPAddress1, item_flag_none ),
  MENU_ITEM( TxtDialupNumber,   (MenuFunc)AUI_dialupNumber, item_flag_none ),
  MENU_ITEM( TxtConnectionSpeed, (MenuFunc)AUI_connectionSpeed,     item_flag_none ), /* SPR#1827 - SH - Connection Speed sub-menu */
  SUB_MENU( TxtAdvanced, menuAdvancedProfileOptions, item_flag_none),
END_MENU( menuDialupProfileOptions, menuListArea, COLOUR_LIST_SUBMENU )

/* SPR#2324 - SH - Profile options menu */

BEGIN_MENU( menuProfileOptions )
  MENU_ITEM( TxtSoftSelect, (MenuFunc)AUI_profile_select, item_flag_none ),
  MENU_ITEM( TxtChange, (MenuFunc)AUI_profile_change, item_flag_none),
  MENU_ITEM( TxtDelete, (MenuFunc)AUI_profile_delete, item_flag_none),
  MENU_ITEM( TxtNewProfile, (MenuFunc)AUI_profile_new, item_flag_none),
  MENU_ITEM( TxtResetProfiles, (MenuFunc)AUI_profile_reset, item_flag_none),
END_MENU( menuProfileOptions, menuListArea, COLOUR_LIST_SUBMENU )

// Setup sub-menu

BEGIN_MENU( menuWAPSettings )
  MENU_ITEM( TxtSaveHistory,    AUI_savehist,       item_flag_none ),
  MENU_ITEM( TxtScaleImages,    AUI_scaleimages,      item_flag_none ),
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
        MENU_ITEM( TxtShowMessages,    AUI_wap_push_showmessages,      item_flag_none )/*added by ellen*/
#endif  
END_MENU( menuWAPSettings, menuListArea, COLOUR_LIST_SUBMENU )

// History Options sub-menu

BEGIN_MENU( menuHistoryOptions )
  MENU_ITEM( TxtGoTo,       AUI_history_goto,     item_flag_none ),
  MENU_ITEM( TxtEditName,     AUI_history_edit,     item_flag_none ),
  MENU_ITEM( TxtEditURL,      AUI_history_edit_url,   item_flag_none ),
  MENU_ITEM( TxtDelete,     AUI_history_delete,     item_flag_none ),
  MENU_ITEM( TxtSend,       AUI_history_sendSMS,    item_flag_none ),
  MENU_ITEM( TxtBookmark,     AUI_history_addbook,    item_flag_none )
END_MENU( menuHistoryOptions, menuListArea, COLOUR_LIST_SUBMENU ) //SPR#1347 - GW - added parameter.

// History sub-menu

BEGIN_MENU( menuHistory )
  MENU_ITEM( TxtList,       AUI_history_list,     item_flag_none ),
  MENU_ITEM( TxtClear,      AUI_history_clear,      item_flag_none )
END_MENU( menuHistory, menuListArea, COLOUR_LIST_SUBMENU )

// GoTo sub-menu

BEGIN_MENU( menuGoTo )
  MENU_ITEM( TxtWWWdot,     AUI_goto_www,       item_flag_none ),
  MENU_ITEM( TxtWAPdot,     AUI_goto_wap,       item_flag_none ),
  MENU_ITEM( TxtOther,      AUI_goto_other,       item_flag_none )
END_MENU( menuGoTo, menuListArea, COLOUR_LIST_SUBMENU )

// Bookmark Options sub-menu

BEGIN_MENU( menuBookmarkOptions )
  MENU_ITEM( TxtGoTo,       AUI_bookmarks_goto,     item_flag_none ),
  MENU_ITEM( TxtEditName,     AUI_bookmarks_edit,     item_flag_none ),
  MENU_ITEM( TxtEditURL,      AUI_bookmarks_edit_url,   item_flag_none ),
  MENU_ITEM( TxtDelete,     AUI_bookmarks_delete,   item_flag_none ),
  MENU_ITEM( TxtSend,       AUI_bookmarks_sendSMS,    item_flag_none )
END_MENU( menuBookmarkOptions, menuListArea, COLOUR_LIST_SUBMENU )

// New Bookmark sub-menu

BEGIN_MENU( menuNewBookmark )
  MENU_ITEM( TxtWWWdot,     AUI_bookmarks_add_www,    item_flag_none ),
  MENU_ITEM( TxtWAPdot,     AUI_bookmarks_add_wap,    item_flag_none ),
  MENU_ITEM( TxtOther,      AUI_bookmarks_add_other,  item_flag_none )
END_MENU( menuNewBookmark, menuListArea, COLOUR_LIST_SUBMENU )

// Bookmarks sub-menu

BEGIN_MENU( menuBookmarks )
  MENU_ITEM( TxtList,       AUI_bookmarks_list,     item_flag_none ),
  MENU_ITEM( TxtNewBookmark,   AUI_bookmarks_add,      item_flag_none )	/* SPR#1983 - SH */
END_MENU( menuBookmarks, menuListArea, COLOUR_LIST_SUBMENU )

/* WAP Menu
 * SPR#2324 - SH - Added "Profiles" menu */

BEGIN_MENU( menuWAP )
  MENU_ITEM( TxtHomepage,     AUI_homepage,       item_flag_none ),
  SUB_MENU( TxtBookmarks,     menuBookmarks,        item_flag_none ),
  SUB_MENU( TxtGoTo,        menuGoTo,         item_flag_none ),
  SUB_MENU( TxtHistory,     menuHistory,        item_flag_none ),
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
  MENU_ITEM( TxtDownload,     AUI_download,        item_flag_none ),    /* SPR#2086 - TEST CODE */
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
//SUB_MENU( TxtPushMessages,      menuPushListOptions,      item_flag_none ), /*added by ellen*/
// xreddymn Jan-06-2004 MMI-SPR-27618: Display list of WAP push messages before displaying the Options list.
  MENU_ITEM( TxtPushMessages,      (MenuFunc)AUI_wap_push_list,      item_flag_none ), 
#endif    
#endif
  SUB_MENU( TxtSettings,      menuWAPSettings,      item_flag_none ),
  MENU_ITEM( TxtProfiles,  (MenuFunc)AUI_profiles_setup,  item_flag_none )
END_MENU( menuWAP, menuListArea, COLOUR_LIST_SUBMENU )

#endif

/*---------------------------------------------------------------------------*/

#ifdef EASY_TEXT_ENABLED
//Sub Menu of Easy Text
BEGIN_MENU( menuEasyText )
  MENU_ITEM( TxtActivate,   Easy_Activate,  item_flag_none ),
  MENU_ITEM( TxtDeactivate, Easy_DeActivate, item_flag_none)
END_MENU_HEADER( menuEasyText, menuListArea, COLOUR_LIST_SUBMENU, TxtEasyText)
#endif

//Sub-Menu of Volume Settings
BEGIN_MENU( menuSetSmsValidityPeriod )
    MENU_ITEM( Txt1Hour,  SmsValidityPeriod,    item_flag_none ),
    MENU_ITEM( Txt12Hours,  SmsValidityPeriod,  item_flag_none ),
    MENU_ITEM( Txt24Hours,  SmsValidityPeriod,    item_flag_none ),
	MENU_ITEM( Txt1Week,  SmsValidityPeriod,  item_flag_none ),
	MENU_ITEM( TxtMaxTime,  SmsValidityPeriod,  item_flag_none ),  //  May 10, 2004    REF: CRR 15753  Deepa M.D .Menuitem to set the validity period to Maximum.
END_MENU( menuSetSmsValidityPeriod, menuListArea, COLOUR_LIST_SUBMENU )

 //Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
//Sub-Menu of Quality Settings
BEGIN_MENU( menuSetQuality )
    MENU_ITEM( TxtEconomy,  mmi_camera_test_set_quality,    item_flag_none ),
    MENU_ITEM( TxtStandard,  mmi_camera_test_set_quality,  item_flag_none ),
    MENU_ITEM( TxtHigh,  mmi_camera_test_set_quality,    item_flag_none ),
END_MENU( menuSetQuality, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuSetColor )
    MENU_ITEM( TxtColor,  mmi_camera_test_set_color,    item_flag_none ),
    MENU_ITEM( TxtSepia,  mmi_camera_test_set_color,    item_flag_none ),
    MENU_ITEM( TxtBlackandWhite,  mmi_camera_test_set_color,    item_flag_none ),
    MENU_ITEM( TxtNegative,  mmi_camera_test_set_color,  item_flag_none )
END_MENU( menuSetColor, menuListArea, COLOUR_LIST_SUBMENU )
#endif //FF_MMI_TEST_CAMERA

#ifdef FF_MMI_CAMERA_APP
//Sub-Menu of Quality Settings
BEGIN_MENU( menuSetQuality )
    MENU_ITEM( TxtEconomy,  mmi_camera_set_quality,    item_flag_none ),
    MENU_ITEM( TxtStandard,  mmi_camera_set_quality,  item_flag_none ),
    MENU_ITEM( TxtHigh,  mmi_camera_set_quality,    item_flag_none ),
END_MENU( menuSetQuality, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

BEGIN_MENU( menuStorage )
    MENU_ITEM( TxtFFS,  mmi_camera_set_storage,    mmi_check_nor ),
    MENU_ITEM( TxtNORMS,  mmi_camera_set_storage,    mmi_check_norms ),
    MENU_ITEM( TxtNAND,  mmi_camera_set_storage,  mmi_check_nand ),
    MENU_ITEM( TxtTflash,  mmi_camera_set_storage,    mmi_check_tflash ),
END_MENU( menuStorage, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */


/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
BEGIN_MENU( menuAutoSave )
    MENU_ITEM( TxtAutoSaveOn,  mmi_camera_set_auto_save, item_flag_none ),
    MENU_ITEM( TxtAutoSaveOff,  mmi_camera_set_auto_save, item_flag_none ),
END_MENU( menuAutoSave, menuListArea, COLOUR_LIST_SUBMENU )

//added by prachi 

#if CAM_SENSOR == 1
BEGIN_MENU( menuResolution )
    MENU_ITEM( TxtCamVga, mmi_camera_set_resolution, item_flag_none ),
    MENU_ITEM( TxtCamQcif, mmi_camera_set_resolution, item_flag_none ),
    MENU_ITEM( TxtCamSxga, mmi_camera_set_resolution, item_flag_none ),
END_MENU( menuResolution, menuListArea, COLOUR_LIST_SUBMENU )
#endif
//Sub-Menu of Rotate Settings
BEGIN_MENU( menuSetRotate )
    MENU_ITEM( TxtRotate90Degree,  mmi_camera_set_rotation,    item_flag_none ),
    MENU_ITEM( TxtRotate180Degree,  mmi_camera_set_rotation,  item_flag_none ),
    MENU_ITEM( TxtRotate270Degree,  mmi_camera_set_rotation,    item_flag_none ),
END_MENU( menuSetRotate, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Shooting mode Settings
BEGIN_MENU( menuSetBurstmode )
    MENU_ITEM( TxtOn,  mmi_camera_set_burstmode,    item_flag_none ),
    MENU_ITEM( TxtOff,  mmi_camera_set_burstmode,  item_flag_none ),
END_MENU( menuSetBurstmode, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Border frames
BEGIN_ICON_MENU( menuFrameMenu )
    MENU_ICON_ITEM( &mainIconFrame1,TxtFrame1,  mmi_camera_set_borderframe,     item_flag_none ),
    MENU_ICON_ITEM( NULL,TxtNoFrame,  mmi_camera_set_borderframe,     item_flag_none ),
END_ICON_MENU( menuFrameMenu, menuListArea, COLOUR_LIST_MAIN, NULL )

//Sub-Menu of Camera Settings
BEGIN_MENU( menuSetCameraparams )
    SUB_MENU( TxtQuality,  menuSetQuality,     item_flag_none ),
    SUB_MENU( TxtFrame, menuFrameMenu ,    item_flag_none ),
    SUB_MENU( TxtRotate,      menuSetRotate,     item_flag_none ),
    SUB_MENU( TxtShootingMode,		menuSetBurstmode,	item_flag_none),
    SUB_MENU( TxtStorageDevice,	menuStorage,	item_flag_none),
/* April 03 2007 ER:OMAPS00116772 x0061088(Prachi)*/
    SUB_MENU( TxtAutoSave,	menuAutoSave,	item_flag_none),
 //added by prachi add flag here    
 #if CAM_SENSOR == 1
    SUB_MENU( TxtCamMenuResolution,	menuResolution,	item_flag_none),
 #endif    
    MENU_ITEM( TxtFilename,		mmi_camera_edit_filename,	item_flag_none),
END_MENU( menuSetCameraparams, menuListArea, COLOUR_LIST_SUBMENU )


#endif //FF_MMI_CAMERA_APP

#ifdef TI_PS_FF_CONC_SMS
//1227 - 01/10/02 - API - Sub Menu for the Concatenate Switch menu option
//Sub Menu of Concatenate Switch
BEGIN_MENU( menuConcatenate )
	MENU_ITEM( TxtActivate,	Concatenate_Activate,	item_flag_none),
	MENU_ITEM( TxtDeactivate,	Concatenate_DeActivate,	item_flag_none),
	MENU_ITEM( TxtStatus,	Concatenate_Status,	item_flag_none)
END_MENU( menuConcatenate, menuListArea, COLOUR_LIST_SUBMENU )
#endif /*TI_PS_FF_CONC_SMS*/

/* Marcus: Issue 1170: 30/09/2002: Start */
BEGIN_MENU( menuSetSmsStatusReportRequest )
    MENU_ITEM( TxtOff,  SmsStatusReportRequest,    item_flag_none ),
    MENU_ITEM( TxtOn,  SmsStatusReportRequest,  item_flag_none )
END_MENU( menuSetSmsStatusReportRequest, menuListArea, COLOUR_LIST_SUBMENU )
/* Marcus: Issue 1170: 30/09/2002: End */

// March 2, 2005    REF: CRR 11536 x0018858
// Added this to create a menu option for setting
// and unsetting the reply path.
// begin -11536
BEGIN_MENU( menuSetSmsReplyPath)
    MENU_ITEM( TxtOff,  SmsReplyPathRequest,    item_flag_none ),
    MENU_ITEM( TxtOn,  SmsReplyPathRequest,  item_flag_none )
END_MENU( menuSetSmsReplyPath, menuListArea, COLOUR_LIST_SUBMENU )
// end -11536

// Sub-Menu of Message Settings
// 1227 - 05/10/02 - API - Add the Menu option Concatenate
BEGIN_MENU( menuSetMsg )
#ifndef EASY_TEXT_ENABLED
    MENU_ITEM( TxtServiceCentre,  SmsSend_R_OPTExeCentreEdit,     item_flag_none ),
    SUB_MENU( TxtValidity,      menuSetSmsValidityPeriod,     item_flag_none ),
#ifdef TI_PS_FF_CONC_SMS    
    SUB_MENU( TxtConcat,		menuConcatenate,	ConcatenateDisplay),
#endif /*TI_PS_FF_CONC_SMS*/
	
// May04, 2005    REF: CRR 30285 x0021334
// Changed SUB_MENU to MENU_ITEM to call function settingsStatusRequestCurSel()
    MENU_ITEM( TxtSRR,  (MenuFunc)settingsSmsStatusReportCurSel,    item_flag_none ),
 //SUB_MENU( TxtSRR,      menuSetSmsStatusReportRequest,     item_flag_none ), // Marcus: Issue 1170: 30/09/2002
    //March 2, 2005    REF: CRR 11536 x0018858
    SUB_MENU( TxtRP,      menuSetSmsReplyPath,     item_flag_none )// Added for setting reply Path
#else
    MENU_ITEM( TxtServiceCentre,  SmsSend_R_OPTExeCentreEdit,     item_flag_none ),
    SUB_MENU( TxtValidity,      menuSetSmsValidityPeriod,     item_flag_none ),
    SUB_MENU( TxtEasyText,      menuEasyText,       item_flag_none),
#ifdef TI_PS_FF_CONC_SMS    
     SUB_MENU( TxtConcat,		menuConcatenate,	ConcatenateDisplay),
#endif /*TI_PS_FF_CONC_SMS*/     
// May04, 2005    REF: CRR 30285 x0021334
// Changed SUB_MENU to MENU_ITEM to call function settingsStatusRequestCurSel()
    MENU_ITEM( TxtSRR,  (MenuFunc)settingsSmsStatusReportCurSel,    item_flag_none ),
//    SUB_MENU( TxtSRR,      menuSetSmsStatusReportRequest,     item_flag_none ), // Marcus: Issue 1170: 30/09/2002
    //March 2, 2005    REF: CRR 11536 x0018858
    SUB_MENU( TxtRP,      menuSetSmsReplyPath,     item_flag_none ) // Added for setting reply Path.
//    MENU_ITEM( TxtMessageTyp,   bookMenuNotImplemented,     item_flag_none )
#endif
END_MENU( menuSetMsg, menuListArea, COLOUR_LIST_SUBMENU )


//// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
BEGIN_MENU(menuRotateActivate)
    MENU_ITEM( TxtRotate90Degree,   mmi_fv_set_rotate_90degree,    item_flag_none ),
    MENU_ITEM( TxtRotate180Degree,    mmi_fv_set_rotate_180degree,     item_flag_none ),
    MENU_ITEM( TxtRotate270Degree,    mmi_fv_set_rotate_270degree,     item_flag_none )
END_MENU( menuRotateActivate, menuListArea, COLOUR_LIST_SUBMENU1 )

BEGIN_MENU( menuFileViewer )
	MENU_ITEM( TxtView , mmi_fv_view_image_start,     item_flag_none ),
	MENU_ITEM( TxtRename,      mmi_fv_reame_edit,     item_flag_none ),
	MENU_ITEM( TxtDelete,		mmi_fv_delete_start,	item_flag_none),
END_MENU( menuFileViewer, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuViewOptions )
	SUB_MENU( TxtRotate,   menuRotateActivate,     item_flag_none ),
END_MENU( menuViewOptions, menuListArea, COLOUR_LIST_SUBMENU )
#endif

/*MC  SPR 1920 add brodcast activate menu*/
BEGIN_MENU(menuBroadMsgActivate)
    MENU_ITEM ( TxtCheckStatus,   settingsShowStateCBDisable,     item_flag_none ),
    MENU_ITEM( TxtOff,   smscb_M_CB_ACT_ExeNo,    item_flag_none ),
    MENU_ITEM( TxtOn,     smscb_M_CB_ACT_ExeYes,     item_flag_none )
END_MENU( menuBroadMsgActivate, menuListArea, COLOUR_LIST_SUBMENU1 )
/*MC  SPR 1920 re-instate broadcast SMS menu*/
// Sub-Menu of Broadcast Messages
BEGIN_MENU( menuBroadMsg )
    SUB_MENU( TxtActivate,   menuBroadMsgActivate,     item_flag_none ),
    MENU_ITEM( TxtRead,     sms_cb_select_read,     item_flag_none ),
    MENU_ITEM( TxtNewBroadcast, smscb_M_CB_TOP_ExeAddNew,     item_flag_none ),
    MENU_ITEM( TxtList,     smscb_M_CB_TOP_ExeConsult,     item_flag_none )
END_MENU( menuBroadMsg, menuListArea, COLOUR_LIST_SUBMENU1 )

// Sub-Menu of VoiceMail
BEGIN_MENU( menuVoiceMsg )
    MENU_ITEM( TxtVoicemail,  menu_cphs_call_mailbox_number,           item_flag_none ),
    MENU_ITEM( TxtSetVoiceMail, menu_cphs_edit_mailbox_number,     item_flag_none/*CphsMailboxPresent*/ )
END_MENU( menuVoiceMsg, menuListArea, COLOUR_LIST_SUBMENU )


//

#ifdef FF_CPHS_REL4
/*sub menu of MailBox*/
BEGIN_MENU( menuMailBox )
    MENU_ITEM( TxtVoicemail,  menu_cphs_rel4_show_mailbox_number,           item_flag_none ),
    MENU_ITEM( TxtFax, menu_cphs_rel4_show_mailbox_number,     item_flag_none ),
    MENU_ITEM( TxtElectronic, menu_cphs_rel4_show_mailbox_number,     item_flag_none ),
    MENU_ITEM( TxtOther, menu_cphs_rel4_show_mailbox_number,     item_flag_none )
END_MENU_HEADER( menuMailBox, menuListArea, COLOUR_LIST_SUBMENU, TxtMailBox )

#endif

/****************************************************/

#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
//yanbin MMS menu
//TISHMMS Project
// May 11 2005  REF:  MMI-SPR-29887  x0012849
// New Menu for MMS Retrieval condition
BEGIN_MENU( menuSetMmsRetrievalType ) 
    MENU_ITEM( TxtImmediate,  MmsRetrievalTypeRequest,    item_flag_none ),   
    MENU_ITEM( TxtDeferred,  MmsRetrievalTypeRequest,  item_flag_none ),   
    MENU_ITEM( TxtStatus,  MmsRetrievalTypeRequest,  item_flag_none )   
END_MENU(menuSetMmsRetrievalType, menuListArea, COLOUR_LIST_SUBMENU)
// Sub-Menu of MMS
BEGIN_MENU( menuMMS )
#ifdef FF_MMI_MMS
    MENU_ITEM( TxtMMSCreate,  M_exeMMSCreate,           item_flag_none ),
#endif    
#ifdef FF_MMI_EMS
    MENU_ITEM( TxtEMSCreate,  M_exeEMSCreate,           item_flag_none ),
#endif    
#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
    MENU_ITEM( TxtMMSInbox,  M_exeMMSInbox,           item_flag_none ),
    MENU_ITEM( TxtMMSUnsent,  M_exeMMSUnsent,           item_flag_none ),
//CRR: 25291 - xrashmic 14 Oct 2004
    MENU_ITEM( TxtMMSSent,  M_exeMMSSent,           item_flag_none ),
#endif
#ifdef FF_MMI_MMS
    MENU_ITEM( TxtMMSSettings,  M_exeMMSSettings,           item_flag_none ),
   // May 11 2005  REF:  MMI-SPR-29887  
    SUB_MENU ( TxtRetrieveCondition,  menuSetMmsRetrievalType ,           item_flag_none )
#endif
END_MENU( menuMMS, menuListArea, COLOUR_LIST_SUBMENU1 )
#endif

/****************************************************/
// Menu of Messages
// xreddymn Nov-09-2004, MMI-SPR-25333: Modified Messaging sub-menu to distinguish between SMS and MMS/EMS
#if(0)

BEGIN_MENU( menuMsg )
        //yanbin add MMS menu
        //TISHMMS Project

// xreddymn Aug-09-2004
#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
        SUB_MENU ( TxtMMS,      menuMMS,         item_flag_none ),
#endif

    MENU_ITEM ( TxtRead,      M_exeRead,         item_flag_none ),
    // MZ Start the Text editor.
/* API - 03/09/03 - SPR2383 - Remove the call to show the Editor and call the eZiText window if active*/
/*MC SPR 1498*/
#if defined (EASY_TEXT_ENABLED)
#  if defined (WIN32) // EF windows simulation does not use ezitext(yet)
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#  else
    MENU_ITEM( TxtWrite,    showeZiTextIcon,      item_flag_none ),
#  endif
#else
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#endif
	/*MC end*/
/* API - 03/09/03 - SPR2383 - END*/
    MENU_ITEM( TxtDeleteAll,    M_exeDelAll,  item_flag_none ),
    SUB_MENU ( TxtSettings,   menuSetMsg,        item_flag_none ),
    SUB_MENU ( TxtBroadcast,    menuBroadMsg/*MC SPR 1920*/,      item_flag_none ),
    SUB_MENU ( TxtVoicemail,    menuVoiceMsg,        item_flag_none )
END_MENU( menuMsg, menuListArea, COLOUR_LIST_SUBMENU )

#else

/* xreddymn Nov-09-2004, MMI-SPR-25333: Seperate sub menu for SMS features */
#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
BEGIN_MENU( menuSMS )
    MENU_ITEM ( TxtRead,      M_exeRead,         item_flag_none ),
    // MZ Start the Text editor.
/* API - 03/09/03 - SPR2383 - Remove the call to show the Editor and call the eZiText window if active*/
/*MC SPR 1498*/
#if defined (EASY_TEXT_ENABLED)
#  if defined (WIN32) // EF windows simulation does not use ezitext(yet)
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#  else
    MENU_ITEM( TxtWrite,    showeZiTextIcon,      item_flag_none ),
#  endif
#else
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#endif
	/*MC end*/
/* API - 03/09/03 - SPR2383 - END*/
    MENU_ITEM( TxtDeleteAll,    M_exeDelAll,  item_flag_none ),
    SUB_MENU ( TxtSettings,   menuSetMsg,        item_flag_none ),
    SUB_MENU ( TxtBroadcast,    menuBroadMsg/*MC SPR 1920*/,      item_flag_none ),
    SUB_MENU ( TxtVoicemail,    menuVoiceMsg,        item_flag_none )
END_MENU( menuSMS, menuListArea, COLOUR_LIST_SUBMENU )
#endif

/* xreddymn Nov-09-2004, MMI-SPR-25333: New Messaging Sub-menu.
 * If MMS/EMS feature is available, move SMS related features into a new sub-menu,
 * Otherwise we retain SMS features in the Messaging sub-menu
 */
BEGIN_MENU( menuMsg )
#if defined (FF_MMI_EMS) || defined (FF_MMI_MMS)
	//yanbin add MMS menu
    //TISHMMS Project
    SUB_MENU ( TxtMMS,      menuMMS,         item_flag_none ),
    SUB_MENU ( TxtSMS,      menuSMS,         item_flag_none ),
#else
    MENU_ITEM ( TxtRead,      M_exeRead,         item_flag_none ),
    // MZ Start the Text editor.
/* API - 03/09/03 - SPR2383 - Remove the call to show the Editor and call the eZiText window if active*/
/*MC SPR 1498*/
#if defined (EASY_TEXT_ENABLED)
#  if defined (WIN32) // EF windows simulation does not use ezitext(yet)
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#  else
    MENU_ITEM( TxtWrite,    showeZiTextIcon,      item_flag_none ),
#  endif
#else
    MENU_ITEM( TxtWrite,    M_exeSendNew,      item_flag_none ),
#endif
	/*MC end*/
/* API - 03/09/03 - SPR2383 - END*/
    MENU_ITEM( TxtDeleteAll,    M_exeDelAll,  item_flag_none ),
    SUB_MENU ( TxtSettings,   menuSetMsg,        item_flag_none ),
    SUB_MENU ( TxtBroadcast,    menuBroadMsg/*MC SPR 1920*/,      item_flag_none ),
#ifdef FF_CPHS_REL4      
    MENU_ITEM ( TxtMessageStatus, build_profile_list1, item_flag_isMWISFilePresent ),
    /*a0393213 cphs rel4*/
/*VoiceMail menu is not shown when MBDN file is present.
    Note that the menu item is not shown when the flag function returns 1 and shown when it returns 0*/
    SUB_MENU ( TxtVoicemail,    menuVoiceMsg,        item_flag_isMBDNFileNotPresent ),
    SUB_MENU (TxtMailBox, menuMailBox, item_flag_isMBDNFilePresent),    
#else  
    SUB_MENU ( TxtVoicemail,    menuVoiceMsg,        item_flag_none )
#endif    
#endif
END_MENU( menuMsg, menuListArea, COLOUR_LIST_SUBMENU )

#endif

/*---------------------------------------------------------------------------*/



/* Phone book menus
 *
 */
BEGIN_MENU( menuOwnNumber )
    MENU_ITEM( TxtConsult, bookGetOwnNumber,    item_flag_none ),
    MENU_ITEM( TxtAddNew,  bookSetOwnNumber,    item_flag_none )
END_MENU( menuOwnNumber, menuListArea, COLOUR_LIST_SUBMENU )

// Sub-Menu of
BEGIN_MENU( menuRepRedOptions )
    MENU_ITEM( TxtStoreNumber, bookRepRedNameEnter, item_flag_none ),
    MENU_ITEM( TxtSendMessage, bookSendMessage,     item_flag_none ),
    MENU_ITEM( TxtDetails,     bookCallListDetails, item_flag_none ),
    MENU_ITEM( TxtDelete,      bookRepRedDelete,    item_flag_none ),
    // Nov 10, 2006   DRT: OMAPS000103354  x0039928
    // Fix : Delete All is mapped to bookDeleteRec instead of bookDeleteAll
    MENU_ITEM( TxtDeleteAll,   bookDeleteRec,      item_flag_none )
END_MENU( menuRepRedOptions, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuNameOptions )
    MENU_ITEM( TxtSendMessage, bookSendMessage,     item_flag_none ),
    MENU_ITEM( TxtChange,      bookNameEdit,        item_flag_none ),
    MENU_ITEM( TxtDetails,     bookDetailsStart,    item_flag_none ),
      //added "call" option - MC
    MENU_ITEM( TxtCallNumber,  bookNameCall,    item_flag_none ),

/* SPR#1112 - SH - Options for when internal phonebook present */
#ifdef INT_PHONEBOOK
	SUB_MENU( TxtCopy, menuCopy, item_flag_none),
	SUB_MENU( TxtMove, menuMove, item_flag_none),
#endif
	MENU_ITEM( TxtDelete,      bookNameDelete,    item_flag_none ),
END_MENU( menuNameOptions, menuListArea, COLOUR_LIST_SUBMENU )

/*SPR 1327, UPN number menu*/
BEGIN_MENU( menuUPNOptions )
    MENU_ITEM( TxtChange,      bookNameEdit,        item_flag_none ),
    MENU_ITEM( TxtDetails,     bookDetailsStart,    item_flag_none ),
    MENU_ITEM( TxtDelete,      bookNameDelete,    item_flag_none ),
END_MENU( menuUPNOptions, menuListArea, COLOUR_LIST_SUBMENU )
#ifdef FF_MMI_CPHS
BEGIN_MENU( menuEmergOptions )
    MENU_ITEM( TxtEdit, bookEmergEdit,     item_flag_none ),
    MENU_ITEM( TxtDelete,      bookEmergDelete,        item_flag_none ),
END_MENU( menuEmergOptions, menuListArea, COLOUR_LIST_SUBMENU )
// Emergency Numbers menu
BEGIN_MENU( menuEmergency )
    MENU_ITEM( TxtAddNew,      bookEmergencyAdd,        item_flag_none ),
    MENU_ITEM( TxtViewAll,     bookEmergencyView,    item_flag_none ),
END_MENU( menuEmergency, menuListArea, COLOUR_LIST_SUBMENU )
#endif
// Menu of Phonebook
BEGIN_MENU( menuPhbk )
    MENU_ITEM( TxtSearchName,     bookNameSearch,       item_flag_none ),
#ifdef FF_CPHS
    MENU_ITEM(TxtInfoNumbers, InfoNumbers,        item_flag_none),
#endif
    MENU_ITEM( TxtAddNew,         bookNameEnter,        item_flag_none ),
    MENU_ITEM( TxtServiceNumbers, bookServiceNumbers,   item_flag_none ),
    SUB_MENU(  TxtMyNumber,       menuOwnNumber,        item_flag_none ),
    MENU_ITEM( TxtMemory,         bookMemoryStatus,     item_flag_none ),
// Aug 18, 2006   ER: OMAPS00089840  x0039928    
    MENU_ITEM( TxtDeleteAll,     bookDeleteAll,       item_flag_none ),
/* SPR#1112 - SH - Internal Phonebook new menu option */
#ifdef INT_PHONEBOOK
	MENU_ITEM( TxtSelectBook,	bookSelectBook,			item_flag_none ),
#endif
#ifdef FF_MMI_CPHS
	SUB_MENU( TxtEmergencyNumbers,	menuEmergency,			item_flag_none ),
#endif	
END_MENU( menuPhbk, menuListArea, COLOUR_LIST_SUBMENU )



/*---------------------------------------------------------------------------*/



//Sub-Menu of Call Timers
BEGIN_MENU(menuCallTimers)
  MENU_ITEM( TxtLastCall,   timerLastCall,          item_flag_none),
  MENU_ITEM( TxtIncomingCalls,    timerIncomingCalls,       item_flag_none),
  MENU_ITEM( TxtOutgoingCalls,    timerOutgoingCalls,       item_flag_none),
END_MENU(menuCallTimers, menuListArea, COLOUR_LIST_SUBMENU)



//Sub-Menu of Charge Limit / AOC
BEGIN_MENU( menuLimitAOC )
    MENU_ITEM( TxtView,     aoc_view_limit,         item_flag_none ),
    MENU_ITEM( TxtChange,   aoc_change_limit,       item_flag_none ),
    MENU_ITEM( TxtCancel,   aoc_cancel_limit,       item_flag_none )
END_MENU( menuLimitAOC, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-Menu of AOC
BEGIN_MENU( menuAOCRecent )
    MENU_ITEM( TxtCredit,   aoc_credit_remaining,   item_flag_none ),
    MENU_ITEM( TxtRate,     aoc_charge_rate,        item_flag_none ),
    SUB_MENU ( TxtLimit,    menuLimitAOC,       item_flag_none ),
    MENU_ITEM( TxtLastCharge, aoc_last_charge,      item_flag_none ),
    MENU_ITEM( TxtTotalCharge,  aoc_total_charge,     item_flag_none ),
    MENU_ITEM( TxtResetCharge,  aoc_reset_charge,     item_flag_none )
END_MENU( menuAOCRecent, menuListArea, COLOUR_LIST_SUBMENU )


// Menu of Recent Calls
BEGIN_MENU( menuRecent )
    MENU_ITEM( TxtMissedCalls,  bookEnterLMN,     item_flag_none ),
    MENU_ITEM( TxtAnsweredCalls, bookEnterLRN,     item_flag_none ), //API - 28-01-03 - 1628 - Change TxtIncomingCall to TxtAnsweredCalls
    MENU_ITEM( TxtOutgoingCalls,bookEnterLDN,     item_flag_none ),
    SUB_MENU ( TxtCallTimers, menuCallTimers,     item_flag_none ),
    SUB_MENU ( TxtChargeTimers, menuAOCRecent,   service_check_aoc )
END_MENU( menuRecent, menuListArea, COLOUR_LIST_SUBMENU )
/*---------------------------------------------------------------------------*/




//Sub-Menu
BEGIN_MENU( menuCallDivert )
    MENU_ITEM ( TxtCheckStatus,   servicesDivertCheck,     item_flag_none ),
    MENU_ITEM ( TxtSetDivert,   servicesNumbers,       item_flag_none ),
    MENU_ITEM ( TxtCancelDivert,  servicesDivertDeactivate,    item_flag_none )
END_MENU( menuCallDivert, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu
BEGIN_MENU( menuCallDivertNR )
    MENU_ITEM ( TxtSetDivert,   servicesNumbers,       item_flag_none ),
    MENU_ITEM ( TxtCancelDivert,  servicesDivertDeactivate,    item_flag_none )
END_MENU( menuCallDivertNR, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu
BEGIN_MENU( menutypDivert )
    MENU_ITEM_SUB_MENU ( TxtVoice,menuCallDivert,servicesDivertVoice,item_flag_none ),
    MENU_ITEM_SUB_MENU ( TxtFax,  menuCallDivert,servicesDivertFax,  item_flag_none ),
    MENU_ITEM_SUB_MENU ( TxtData, menuCallDivert,servicesDivertData, item_flag_none )
END_MENU( menutypDivert, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu
BEGIN_MENU( menutypDivertNR )
    MENU_ITEM_SUB_MENU ( TxtVoice,menuCallDivertNR,servicesDivertVoice,item_flag_none ),
    MENU_ITEM_SUB_MENU ( TxtFax,  menuCallDivertNR,servicesDivertFax,  item_flag_none ),
    MENU_ITEM_SUB_MENU ( TxtData, menuCallDivertNR,servicesDivertData, item_flag_none )
END_MENU( menutypDivertNR, menuListArea, COLOUR_LIST_SUBMENU )

// Menu of Divert
//SPR#1113 - DS - Added sub-menus for "If Not Reach." divert. The NR menus do not allow status checking.
BEGIN_MENU( menuDivert )
    MENU_ITEM_SUB_MENU ( TxtIfNoService,  menutypDivert,  servicesDivertNoService,  item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtIfBusy,     menutypDivert,  servicesDivertBusy,     item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtIfNoAnswer,   menutypDivert,  servicesDivertNoAnswer,   item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtIfNotReachable, menutypDivertNR,  servicesDivertNotReachable, item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtAllCalls,   menutypDivert,  servicesDivertAllCalls,   item_flag_none),
  MENU_ITEM( TxtCancelAll,    servicesDivertCancelAll, item_flag_none )
END_MENU( menuDivert, menuListArea, COLOUR_LIST_SUBMENU )

/*---------------------------------------------------------------------------*/
/*
 *  Security menus
 */

//Sub-Menu of Pin Code
BEGIN_MENU( menuPinCodeSet )
    MENU_ITEM( TxtStatus,   pin_status,        item_flag_none ),
    MENU_ITEM( TxtActivate,   pin_enable,        item_flag_none ),
    MENU_ITEM( TxtDeactivate, pin_disable,       item_flag_none ),
    MENU_ITEM( TxtChange,   pin_change,        item_flag_none )
END_MENU( menuPinCodeSet, menuListArea, COLOUR_LIST_SUBMENU )
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//sub menu of FDN numbers
BEGIN_MENU( bookFDNNumbers )
    MENU_ITEM( TxtSearchName,   bookNameSearch,   item_flag_none ),
    MENU_ITEM( TxtAddNew,  bookNameEnter   ,  item_flag_none ),
END_MENU( bookFDNNumbers, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Fixed Dial
BEGIN_MENU( menuFixedDialSet )
    MENU_ITEM( TxtYes,    bookFDNActivate,   item_flag_none ),
    MENU_ITEM( TxtNo,   bookFDNDeactivate,   item_flag_none ),
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	Added FDN Numbers menu
    MENU_ITEM( TxtFDNName, bookFDNMenu, item_flag_none ),
END_MENU( menuFixedDialSet, menuListArea, COLOUR_LIST_SUBMENU )

#ifdef SIM_PERS 
//Sub-menu of ME Personalization categories
BEGIN_MENU( menuCategory )
    MENU_ITEM ( TxtNetwork,  setLockTypeNw,  item_flag_none),
    MENU_ITEM ( TxtNetworkSubset,  setLockTypeNs,  item_flag_none),
    MENU_ITEM ( TxtServiceProvider,  setLockTypeSp,  item_flag_none),
    MENU_ITEM ( TxtCorporate,  setLockTypeCp,  item_flag_none),
    MENU_ITEM ( TxtSIM,  setLockTypePs,  item_flag_none),
    //x0pleela 25 Sep, 2006 ER: OMAPS00095524
    //Adding a new category type "Blocked Network"
    MENU_ITEM ( TxtBlkNetwork,  setLockTypePb,  item_flag_none)
END_MENU( menuCategory, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-menu of ME Personalization Supplementary Info
BEGIN_MENU( menuSupplementaryInfo )
    MENU_ITEM ( TxtGetFC ,  get_fc_attempts_left ,  item_flag_none),
    MENU_ITEM( TxtMaxFC, get_fc_max_value, item_flag_none),
    MENU_ITEM( TxtGetFailFC, get_fail_reset_fc_attempts_left, item_flag_none),
    MENU_ITEM( TxtMaxFailFC, get_fail_reset_fc_max_value, item_flag_none),
    MENU_ITEM( TxtGetSuccFC, get_succ_reset_fc_attempts_left, item_flag_none),
    MENU_ITEM( TxtMaxSucFC, get_succ_reset_fc_max_value, item_flag_none),
    MENU_ITEM( TxtTimerFlag, get_timer_flag_value, item_flag_none),
    MENU_ITEM( TxtETSIFlag, get_etsi_flag_value, item_flag_none),
    MENU_ITEM( TxtAirtelFlag, get_airtel_ind_flag_value, item_flag_none)    
END_MENU( menuSupplementaryInfo, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-menu of ME Personalization
BEGIN_MENU( menuMEPersonalization )
    MENU_ITEM_SUB_MENU ( TxtLockCategory,  menuCategory,  setLockActionLock,  item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtUnlockCategory,  menuCategory,  setLockActionUnlock,  item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtChangePassword,  menuCategory, setLockActionChPwd,  item_flag_none),
    MENU_ITEM_SUB_MENU ( TxtCheckStatus,  menuCategory,  setLockActionChkStat,  item_flag_none),
    MENU_ITEM ( TxtResetFC,  SIMLock_resetFC,  item_flag_none),
    //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
    //New menu item "Master unlock"
    MENU_ITEM ( TxtMasterUnlock,  SIMLock_MasterKeyUnlock,  simlock_check_masterkey),
    SUB_MENU ( TxtSupplementaryInfo,  menuSupplementaryInfo,  item_flag_none)
END_MENU( menuMEPersonalization, menuListArea, COLOUR_LIST_SUBMENU )

//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
//New menu to display during bootup for unlocking categories or unblocking ME
BEGIN_MENU( menuBootupCatList )
    MENU_ITEM ( TxtNetwork,  simp_unlock_Category,  simlock_check_NWCatLock),
    MENU_ITEM ( TxtNetworkSubset,  simp_unlock_Category, simlock_check_NSCatLock),
    MENU_ITEM ( TxtServiceProvider,  simp_unlock_Category, simlock_check_SPCatLock),
    MENU_ITEM ( TxtCorporate,  simp_unlock_Category, simlock_check_CPCatLock),
    MENU_ITEM ( TxtSIM,  simp_unlock_Category, simlock_check_SIMCatLock),
    //x0pleela 25 Sep, 2006  ER: OMAPS00095524
    //Adding "Blocked Network" category
    MENU_ITEM ( TxtBlkNetwork,  simp_unlock_Category, simlock_check_PBCatLock),
    MENU_ITEM ( TxtUnblockME,  SIMLock_unblock_ME,  simlock_check_CatBlock),
    MENU_ITEM ( TxtMasterUnlock,  SIMLock_MasterKeyUnlock_bootup,  simlock_check_masterkey)  
END_MENU( menuBootupCatList, menuListArea, COLOUR_LIST_SUBMENU )

#endif

//x0pleela 19 Feb, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
//Sub-menu of Automatic Phone Lock
BEGIN_MENU( menuPhLockAutomatic )
    MENU_ITEM ( TxtPhLockAutoOn,  phLock_Automatic_On,  item_flag_none),
    MENU_ITEM ( TxtPhLockAutoOff,  phLock_Automatic_Off, item_flag_none)
 END_MENU( menuPhLockAutomatic, menuListArea, COLOUR_LIST_SUBMENU )

//x0pleela 19 Feb, 2007 ER: OMAPS00122561
//Sub-menu of Phone Lock
BEGIN_MENU( menuPhoneLock )
    MENU_ITEM ( TxtPhLockEnable,  phLock_Enable_Lock,  item_flag_none),
    SUB_MENU ( TxtPhLockAutomatic,  menuPhLockAutomatic,  item_flag_none),
    MENU_ITEM ( TxtPhLockChangeCode,  phLock_Change_UnlockCode,  item_flag_none)
END_MENU( menuPhoneLock, menuListArea, COLOUR_LIST_SUBMENU )
#endif /* FF_PHONE_LOCK*/

// Menu of Security
BEGIN_MENU( menuSecurity )

// Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
// Instead of providing the submenu directly we call a function 
// where we set the menu and also higlight the previously selected menu item
//    MENU_ITEM( TxtFixedNames, settingsFocusFDNCurSel,    item_flag_none ),

// June 07,  2005         REF: CRR MMI-FIX-31545 x0021334
// Issue description: SIM: wrong behavior when FDN is not allocated in SST
// Added this function to check if FDN is allocated
    MENU_ITEM( TxtFixedNames, checkFDNSupport,    item_flag_none ),

    SUB_MENU ( TxtPIN,        menuPinCodeSet,       item_flag_none ),
    MENU_ITEM( TxtPIN2,       pin2_change,          item_flag_none ),
#ifdef SIM_PERS
    SUB_MENU( TxtPersonalization, menuMEPersonalization, item_flag_none )
#endif
    //x0pleela 19 Feb, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
    ,SUB_MENU ( TxtPhoneLock,        menuPhoneLock,       item_flag_none ) 
#endif /* FF_PHONE_LOCK */
END_MENU( menuSecurity, menuListArea, COLOUR_LIST_SUBMENU )

/*---------------------------------------------------------------------------*/
/*
 *  Applications menus
 */

//Options-Menu of Calculator
BEGIN_MENU( menuCalcOptions )
  MENU_ITEM( TxtEquals,       calcEquals,             item_flag_none ),
    MENU_ITEM( TxtPlus,         calcPlus,               item_flag_none ),
    MENU_ITEM( TxtMinus,        calcMinus,              item_flag_none ),
    MENU_ITEM( TxtDivideBy,     calcDivide,           item_flag_none ),
    MENU_ITEM( TxtMultiplyBy,     calcMultiply,             item_flag_none )
END_MENU( menuCalcOptions, menuListArea, COLOUR_LIST_SUBMENU )

//Options-Menu of Melodie Generator
#ifdef MMI_MELODYGENERATOR
BEGIN_MENU( menuMelgenOptions )
  MENU_ITEM( TxtOctaveHigher, octave_higher,           item_flag_none ),
  MENU_ITEM( TxtOctaveLower,  octave_lower,          item_flag_none ),
  MENU_ITEM( TxtPlay,     melody_generator,        item_flag_none ),
  MENU_ITEM( TxtInsertnote, insert_new_note,         item_flag_none ),
  MENU_ITEM( TxtSave,     melody_generator,          item_flag_none ),
  MENU_ITEM( TxtLoadMelody, melody_generator,          item_flag_none ),
  MENU_ITEM( TxtDelete,   delete_current_note,       item_flag_none )
END_MENU( menuMelgenOptions, menuListArea, COLOUR_LIST_SUBMENU )
#endif //MMI_MELODYGENERATOR

//x0pleela 24 Feb, 2006  ER OMAPS00067709
#ifdef FF_PCM_VM_VB

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

BEGIN_MENU( menuPCMVoiceMemoStorage )
    MENU_ITEM( TxtFFS,  PCM_voice_memo_storeage,    mmi_check_nor ),
    MENU_ITEM( TxtNORMS,  PCM_voice_memo_storeage,    mmi_check_norms ),
    MENU_ITEM( TxtNAND,  PCM_voice_memo_storeage,  mmi_check_nand ),
    MENU_ITEM( TxtTflash,  PCM_voice_memo_storeage,    mmi_check_tflash ),
END_MENU( menuPCMVoiceMemoStorage, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuVoicebufferStorage )
    MENU_ITEM( TxtFFS,  voice_buffer_storeage,    mmi_check_nor ),
    MENU_ITEM( TxtNORMS,  voice_buffer_storeage,    mmi_check_norms ),
    MENU_ITEM( TxtNAND,  voice_buffer_storeage,  mmi_check_nand ),
    MENU_ITEM( TxtTflash,  voice_buffer_storeage,    mmi_check_tflash ),
END_MENU( menuVoicebufferStorage, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

//Sub-Menu of PCM Voice Memo
BEGIN_MENU( menuPcmVoiceMemo )
    MENU_ITEM( TxtPlay,     pcm_voice_memo_play,         item_flag_none ),
    MENU_ITEM( TxtRecord, pcm_voice_memo_record,     item_flag_none ),
    SUB_MENU( TxtStorageDevice,   menuPCMVoiceMemoStorage,         item_flag_none )
END_MENU( menuPcmVoiceMemo, menuListArea, COLOUR_LIST_SUBMENU )

//x0pleela 09 Mar, 2006   ER: OMAPS00067709
//Sub-Menu of Voice Buffering
BEGIN_MENU( menuVoiceBuffering )
    MENU_ITEM( TxtEditNum,     voice_buffering_edit_num,         item_flag_none ),
    MENU_ITEM( TxtStartRec,   voice_buffering_record_start,         item_flag_none ),
    MENU_ITEM( TxtStopRec,   voice_buffering_record_stop,         item_flag_none ),
    SUB_MENU( TxtStorageDevice,   menuVoicebufferStorage,         item_flag_none )
END_MENU( menuVoiceBuffering, menuListArea, COLOUR_LIST_SUBMENU )
#endif
#ifndef FF_NO_VOICE_MEMO

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

BEGIN_MENU( menuVoiceMemoStorage )
    MENU_ITEM( TxtFFS,  voice_memo_storeage,    mmi_check_nor ),
    MENU_ITEM( TxtNORMS,  voice_memo_storeage,    mmi_check_norms ),
    MENU_ITEM( TxtNAND,  voice_memo_storeage,  mmi_check_nand ),
    MENU_ITEM( TxtTflash,  voice_memo_storeage,    mmi_check_tflash ),
END_MENU( menuVoiceMemoStorage, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */


//Sub-Menu of Voice Memo
BEGIN_MENU( menuVoiceMemo )
    MENU_ITEM( TxtPlay,     voice_memo_play,         item_flag_none ),
    MENU_ITEM( TxtRecord,   voice_memo_record,         item_flag_none ),
    SUB_MENU( TxtStorageDevice,   menuVoiceMemoStorage,         item_flag_none )
END_MENU( menuVoiceMemo, menuListArea, COLOUR_LIST_SUBMENU )
#endif

//e-armanetsaid 18/05/04 Add menu for Midi driver test
//Sub-Menu of Loop On/Off
//   Apr 06, 2005	REF: ENH 30011 xdeepadh
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
BEGIN_MENU(menuPlayerChannels)
    MENU_ITEM( TxtPlayerMonoChannel,     mmi_audio_player_set_channel_mono,         item_flag_none ),
    MENU_ITEM( TxtPlayerStereoChannel,     mmi_audio_player_set_channel_stereo,         item_flag_none )
END_MENU( menuPlayerChannels, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuPlayerTest )
	MENU_ITEM(TxtPlayerSelectFile, mmi_audio_player_files_browse, item_flag_none),
	// Sep 27,2005 REF: SPR 34402 xdeepadh  
	// Instead of providing the submenu directly, we call a function 
	// where we set the menu and also higlight the previously selected menu item
	MENU_ITEM( TxtPlayerSetChannels, mmi_audio_player_set_cur_selected_channels,    item_flag_none ),
	//Jul 18, 2005    REF: SPR 31695   xdeepadh
	MENU_ITEM( TxtPlayerPlay,   mmi_audio_player_play,         item_flag_none )
END_MENU( menuPlayerTest, menuListArea, COLOUR_LIST_SUBMENU )
#endif //FF_MMI_TEST_MP3

#ifdef FF_MMI_FILEMANAGER
BEGIN_MENU(menuPB)
    MENU_ITEM( TxtMidiLoopOn,     mmi_pb_on_off,         item_flag_none ),
    MENU_ITEM( TxtMidiLoopOff,     mmi_pb_on_off,         item_flag_none )
END_MENU( menuPB, menuListArea, COLOUR_LIST_SUBMENU )
#endif

//Apr 05, 2005    REF: ENH 29994 xdeepadh
#ifdef FF_MMI_TEST_MIDI
BEGIN_MENU(menuMidiLoop)
    MENU_ITEM( TxtMidiLoopOn,     midi_test_set_loop_on,         item_flag_none ),
    MENU_ITEM( TxtMidiLoopOff,     midi_test_set_loop_off,         item_flag_none )
END_MENU( menuMidiLoop, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Channels
BEGIN_MENU(menuMidiChannels)
    MENU_ITEM( TxtMidiMonoChannel,     midi_test_set_channel_mono,         item_flag_none ),
    MENU_ITEM( TxtMidiStereoChannel,     midi_test_set_channel_stereo,         item_flag_none )
END_MENU( menuMidiChannels, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Audio Output 
BEGIN_MENU(menuMidiAudioOutput) 
    MENU_ITEM( TxtMidiSpeaker,     midi_test_set_speaker,         item_flag_none ),
    MENU_ITEM( TxtMidiHeadset,     midi_test_set_headset,         item_flag_none ),
 END_MENU( menuMidiAudioOutput, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Midi Change Config
#if 0
BEGIN_MENU(menuMidiChangeConfig)
    MENU_ITEM( TxtChConfigVoiceLimit,     midi_test_cc_voice_limit,         item_flag_none ),
    SUB_MENU( TxtChConfigLoop,     menuMidiLoop,         item_flag_none ),
    SUB_MENU( TxtChConfigChannels,     menuMidiChannels,         item_flag_none ),
    SUB_MENU( TxtChConfigAudioOutput,    menuMidiAudioOutput,         item_flag_none )
#ifdef FF_MMI_FILEMANAGER
    ,SUB_MENU( TxtPBar,    menuPB,         item_flag_none )
#endif
END_MENU( menuMidiChangeConfig, menuListArea, COLOUR_LIST_SUBMENU )
#endif
//Sub-Menu of Midi Test
// Dec 08, 2004	REF: CRR MMI-SPR-27284
// Description: MIDI Player: Back Key malfunctions in MIDI application.
// Solution: Calling function midi_test_files_browse() when user select "Browse Midi Files"
#if 0
BEGIN_MENU( menuMidiTest )
    MENU_ITEM( TxtChConfigFile,     midi_test_files_browse,         item_flag_none ), 
    SUB_MENU( TxtMidiChangeConfig,    menuMidiChangeConfig,         item_flag_none ),
    MENU_ITEM( TxtMidiPlay,   midi_test_play,         item_flag_none ),
    MENU_ITEM( TxtMidiPlayAllFiles,   midi_test_play_all,         item_flag_none ),
    MENU_ITEM( TxtMidiExit,   midi_stop_player,         item_flag_none )
END_MENU( menuMidiTest, menuListArea, COLOUR_LIST_SUBMENU )
#endif
#endif//#ifdef FF_MMI_TEST_MIDI
#ifdef BTE_MOBILE
//Sub-Menu of Bluetooth Discoverable
BEGIN_MENU( menuBluetoothDiscoverable )
    MENU_ITEM( TxtBtDiscoverable,	mmi_bluetooth_set_discoverable_on,        item_flag_none ),
    MENU_ITEM( TxtBtHidden,			mmi_bluetooth_set_discoverable_off,        item_flag_none ),
    MENU_ITEM( TxtBtTimed,			mmi_bluetooth_set_discoverable_timed,    item_flag_none )
END_MENU( menuBluetoothDiscoverable, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Bluetooth Devices
/*
** Note:
** The menu display functions isBluetoothActive and isBluetoothInactive seem to be used in the wrong context
** ie. isBluetoothInactive for options which should only be displayed if Bluetooth is active. The reason for this is
** that these functions return TRUE if their state is met, where the menu is displayed if the value returned is FALSE.
**
** If still not clear, see the function item_flag_none, which ALWAYS causes the menu item to be diaplayed.
*/
BEGIN_MENU( menuBluetoothDevices )
    MENU_ITEM( TxtSearchName,                    mmi_bluetooth_device_search,        isBluetoothInactive ),
    MENU_ITEM( TxtBtKnownDevices,    mmi_bluetooth_list_known_devices, item_flag_none ),
    MENU_ITEM( TxtBtClearStoredDevices,    mmi_bluetooth_clear_all_stored_devices, item_flag_none ),
END_MENU( menuBluetoothDevices, menuListArea, COLOUR_LIST_SUBMENU )

//Sub-Menu of Bluetooth
BEGIN_MENU( menuBluetooth )
    MENU_ITEM( TxtBluetoothEnable,	mmi_bluetooth_on,				isBluetoothActive ),
    MENU_ITEM( TxtAutomatic,     mmi_bluetooth_automatic,  isBluetoothActive ),
    MENU_ITEM( TxtBluetoothDisable,	mmi_bluetooth_off,				isBluetoothInactive ),
    MENU_ITEM( TxtBtSetLocalName, mmi_bluetooth_change_name, item_flag_none ),
    SUB_MENU( TxtBtDiscoverable, menuBluetoothDiscoverable, item_flag_none),
    SUB_MENU(TxtBtDevices, menuBluetoothDevices, item_flag_none)
END_MENU( menuBluetooth, menuListArea, COLOUR_LIST_SUBMENU )
#endif

#ifdef FF_MMI_MULTIMEDIA
/* Configuration Parameter Menu Items */
BEGIN_MENU( menuSelectOutputScreemode )
	MENU_ITEM( TxtHalfScreenMode, mmi_Set_ScreenMode, item_flag_none),
	MENU_ITEM( TxtFullScreenMode, mmi_Set_ScreenMode , item_flag_none )
END_MENU( menuSelectOutputScreemode, menuListArea, COLOUR_LIST_SUBMENU )


/* PCM Encode Configuration */
BEGIN_MENU( menuPcmEncodeConfig )
    MENU_ITEM( TxtPcmChannelMode,(MenuFunc)FocusCurSel_PcmChannelMode, item_flag_none ),
    MENU_ITEM( TxtPcmRate,(MenuFunc)FocusCurSel_PcmRateSelection, item_flag_none ), 
END_MENU( menuPcmEncodeConfig, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectAudioEncodingFormat )
    SUB_MENU ( TxtPCM, menuPcmEncodeConfig, item_flag_none),
  	MENU_ITEM( TxtAMR, mmi_Set_AudioFormat, item_flag_none ),	
    MENU_ITEM( TxtAAC, mmi_Set_AudioFormat, item_flag_none ),
	MENU_ITEM( TxtMP3, mmi_Set_AudioFormat, item_flag_none )
END_MENU( menuSelectAudioEncodingFormat, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectVideoEncodingFormat )
	MENU_ITEM( TxtMpeg4, mmi_Set_VideoFormat , item_flag_none ),
	MENU_ITEM( TxtH263, mmi_Set_VideoFormat , item_flag_none ),
	MENU_ITEM( TxtNone, mmi_Set_VideoFormat , item_flag_none )
END_MENU( menuSelectVideoEncodingFormat, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectVideoCaptureSize )
	MENU_ITEM( TxtQcif, mmi_Set_VideoSize , item_flag_none ),
	MENU_ITEM( TxtCif, mmi_Set_VideoSize , item_flag_none ),
	MENU_ITEM( TxtQQVga, mmi_Set_VideoSize , item_flag_none )
END_MENU( menuSelectVideoCaptureSize, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuSelectFrameRate )
	MENU_ITEM( Txt5fps, mmi_Set_FrameRate , item_flag_none ),
	MENU_ITEM( Txt10fps, mmi_Set_FrameRate , item_flag_none ),
	MENU_ITEM( Txt15fps, mmi_Set_FrameRate , item_flag_none ),
	MENU_ITEM( Txt30fps, mmi_Set_FrameRate , item_flag_none )
END_MENU( menuSelectFrameRate, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuSelectAACBitRate )
	MENU_ITEM( Txt48Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt66p15Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt72Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt96pKbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt132p3Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt144Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt192Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt264p6Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
	MENU_ITEM( Txt288Kbps, mmi_Set_AAC_BitRate , item_flag_none ),
END_MENU( menuSelectAACBitRate, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectAMRBitRate )
	MENU_ITEM( Txt1p80Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt4p75Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt5p15Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt5p9Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt6p7Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt7p4Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt7p95Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt10p2Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
	MENU_ITEM( Txt12p2Kbps, mmi_Set_AMR_BitRate , item_flag_none ),
END_MENU( menuSelectAMRBitRate, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectAudioRate )
    MENU_ITEM( TxtAACBitRate,(MenuFunc)FocusCurSel_AACBitRate, item_flag_none ),
    MENU_ITEM( AMRNBBitRate,(MenuFunc)FocusCurSel_AMRBitRate, item_flag_none ),
END_MENU( menuSelectAudioRate, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuSelectVideoRate )
	MENU_ITEM( Txt64Kbps, mmi_Set_VideoBitRate , item_flag_none ),
	MENU_ITEM( Txt128Kbps, mmi_Set_VideoBitRate , item_flag_none ),
END_MENU( menuSelectVideoRate, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuAudioPath )
	MENU_ITEM( TxtHeadset, mmi_Set_AudioPath , item_flag_none ),
	MENU_ITEM( TxtSpeaker, mmi_Set_AudioPath, item_flag_none ),
END_MENU( menuAudioPath, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuPreferredStorage )
	MENU_ITEM( TxtFFS, mmi_Set_PreferredStorage , item_flag_none ),
	MENU_ITEM( TxtMMC, mmi_Set_PreferredStorage , item_flag_none)
END_MENU( menuPreferredStorage, menuListArea, COLOUR_LIST_SUBMENU )

/* PCM Decode Sampling Rate */
BEGIN_MENU( menuPcmSampleRate )
	MENU_ITEM( Txt8kRate, mmi_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt11kRate, mmi_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt12kRate, mmi_set_pcm_samplerate, item_flag_none ),       /*OMAPS00081264 - 12k Menu addition */
	MENU_ITEM( Txt16kRate, mmi_set_pcm_samplerate, item_flag_none ), 
	MENU_ITEM( Txt22kRate, mmi_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt24kRate, mmi_set_pcm_samplerate, item_flag_none ),      /*OMAPS00081264 - 24k Menu addition */
	MENU_ITEM( Txt32kRate, mmi_set_pcm_samplerate, item_flag_none ),      
	MENU_ITEM( Txt44kRate, mmi_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt48kRate, mmi_set_pcm_samplerate, item_flag_none ),
END_MENU( menuPcmSampleRate, menuListArea, COLOUR_LIST_SUBMENU )

/*Voice Memo PCM Sampling Rate */
BEGIN_MENU( menuVmPcmSampleRate )
	MENU_ITEM( Txt8kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt11kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),	
	MENU_ITEM( Txt12kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),  /*OMAPS00081264 - 12k Menu addition */
	MENU_ITEM( Txt16kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt22kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt24kRate, mmi_vm_set_pcm_samplerate, item_flag_none ), /*OMAPS00081264 - 24k Menu addition */
	MENU_ITEM( Txt32kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt44kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
	MENU_ITEM( Txt48kRate, mmi_vm_set_pcm_samplerate, item_flag_none ),
END_MENU( menuVmPcmSampleRate, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuPcmDecodeChanMode )
	MENU_ITEM( TxtMonoMode, mmi_Set_PcmDecodeChanMode , item_flag_none ),
	MENU_ITEM( TxtStreoMode, mmi_Set_PcmDecodeChanMode, item_flag_none ),       
END_MENU( menuPcmDecodeChanMode, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuVmPcmDecodeChanMode )
	MENU_ITEM( TxtMonoMode, mmi_Set_VmPcmDecodeChanMode , item_flag_none ),
	MENU_ITEM( TxtStreoMode, mmi_Set_VmPcmDecodeChanMode, item_flag_none ),       
END_MENU( menuVmPcmDecodeChanMode, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuVolumeConfig )
	MENU_ITEM( TxtLevel1, mmi_Set_VolumeConfig , item_flag_none ),
	MENU_ITEM( TxtLevel2, mmi_Set_VolumeConfig , item_flag_none ),
	MENU_ITEM( TxtLevel3, mmi_Set_VolumeConfig , item_flag_none ),
	MENU_ITEM( TxtLevel4, mmi_Set_VolumeConfig , item_flag_none ),
	MENU_ITEM( TxtLevel5, mmi_Set_VolumeConfig , item_flag_none ),
END_MENU( menuVolumeConfig, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuCamera )
    MENU_ITEM( TxtCameraCapture,		mmi_Select_CameraCapture,    item_flag_none ),
    MENU_ITEM( TxtVideoRecording,			mmi_Select_AudioVideo_Recording,    item_flag_none ),
    MENU_ITEM( TxtAudioRecording,			mmi_Select_Audio_Recording,    item_flag_none )
END_MENU( menuCamera, menuListArea, COLOUR_LIST_SUBMENU )


#ifdef FF_MMI_NEPTUNE_IMAGE_VIEWER
BEGIN_MENU( menuImageEdit )
    MENU_ITEM( TxtCrop,	mmi_ImageEdit_Crop,    item_flag_none ),
    MENU_ITEM( TxtRescale,	mmi_ImageEdit_Rescale,    item_flag_none ),
    MENU_ITEM( TxtRotate,	mmi_ImageEdit_Rotate,    item_flag_none ),
END_MENU( menuImageEdit, menuListArea, COLOUR_LIST_SUBMENU )
#endif

/* Image Capture Quality */
BEGIN_MENU( menuImageCaptureQuality )
	MENU_ITEM( Txt0Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt10Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt20Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt30Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt40Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt50Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt60Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt70Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt80Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt90Set, mmi_Set_ImageCaptureQuality , item_flag_none ),
	MENU_ITEM( Txt100Set, mmi_Set_ImageCaptureQuality , item_flag_none )	
END_MENU( menuImageCaptureQuality, menuListArea, COLOUR_LIST_SUBMENU )


/* Power management menus*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
BEGIN_MENU( menuPowerManagement )
        MENU_ITEM( TxtOff, mmi_PMDuration, item_flag_none ),
        MENU_ITEM( Txt10Second, mmi_PMDuration, item_flag_none ),
        MENU_ITEM( Txt15Second, mmi_PMDuration, item_flag_none ),
        MENU_ITEM( Txt20Second, mmi_PMDuration, item_flag_none ),
END_MENU( menuPowerManagement, menuListArea, COLOUR_LIST_SUBMENU )
#endif
#endif
/* Pcm Stero / Mono Setting Menu for Recording */
BEGIN_MENU( menuPcmChannelMode )
	MENU_ITEM( TxtMonoMode, mmi_Set_PcmChannelMode , item_flag_none ),
	MENU_ITEM( TxtStreoMode, mmi_Set_PcmChannelMode, item_flag_none ),
END_MENU( menuPcmChannelMode, menuListArea, COLOUR_LIST_SUBMENU )


/* Pcm Rate Selection for Recording */
BEGIN_MENU( menuPcmRateSelection )
	MENU_ITEM( Txt8kRate, mmi_Set_PcmRateSelection , item_flag_none ),
	MENU_ITEM( Txt16kRate, mmi_Set_PcmRateSelection, item_flag_none ),
END_MENU( menuPcmRateSelection, menuListArea, COLOUR_LIST_SUBMENU )

/* To Specify whether file access is from FFS / MMC */
BEGIN_MENU( menuMMStorageDevice )
	MENU_ITEM( TxtFFS, mmi_Select_AudioVideo_Files , item_flag_none ),
	MENU_ITEM( TxtMMC, mmi_Select_AudioVideo_Files , item_flag_none )
END_MENU( menuMMStorageDevice, menuListArea, COLOUR_LIST_SUBMENU )

/* To Specify whether file access is from FFS / MMC for Image Viewer */
BEGIN_MENU( menuMMImageStorageDevice )
	MENU_ITEM( TxtFFS, mmi_Select_Image_Files , item_flag_none ),
	MENU_ITEM( TxtMMC, mmi_Select_Image_Files , item_flag_none )
END_MENU( menuMMImageStorageDevice, menuListArea, COLOUR_LIST_SUBMENU )

/*Audio Decode configuration */
BEGIN_MENU( menuAudioDecodeConfig )
	MENU_ITEM( TxtAudioPath,(MenuFunc)FocusCurSel_AudioPath, item_flag_none ),
END_MENU( menuAudioDecodeConfig, menuListArea, COLOUR_LIST_SUBMENU )

/*Audio Encode Configuration */
BEGIN_MENU( menuAudioEncodeConfig )
	MENU_ITEM( TxtAudioEncodingFormat,(MenuFunc)FocusCurSel_AudioFormat, item_flag_none ),
	SUB_MENU( TxtAudioBitRate,			menuSelectAudioRate,    item_flag_none ),
END_MENU( menuAudioEncodeConfig, menuListArea, COLOUR_LIST_SUBMENU )


/*Audio & Video Sync Option */
BEGIN_MENU( menuAudioVideoSyncOption )
	MENU_ITEM( TxtAudioVideoPlay, mmi_Audio_Video_Play_Option, item_flag_none ),
	MENU_ITEM( TxtVideoPlay, mmi_Audio_Video_Play_Option,    item_flag_none ),
END_MENU( menuAudioVideoSyncOption, menuListArea, COLOUR_LIST_SUBMENU )


/*Audio & Video Sync Option */
BEGIN_MENU( menuVideoRecordDuration )
       MENU_ITEM( TxtNonStop, mmi_Video_Record_Duration,    item_flag_none ),
	MENU_ITEM( Txt10sec, mmi_Video_Record_Duration, item_flag_none ),
	MENU_ITEM( Txt15sec, mmi_Video_Record_Duration,    item_flag_none ),
	MENU_ITEM( Txt20sec, mmi_Video_Record_Duration,    item_flag_none ),			
END_MENU( menuVideoRecordDuration, menuListArea, COLOUR_LIST_SUBMENU )


/*Audio & Video Record option  */
BEGIN_MENU( menuAudioVideoRecordOption )
	MENU_ITEM( TxtAudioVideoRecord, mmi_Audio_Video_Record_Option, item_flag_none ),
	MENU_ITEM( TxtVideoRecord, mmi_Audio_Video_Record_Option,    item_flag_none ),
END_MENU( menuAudioVideoRecordOption, menuListArea, COLOUR_LIST_SUBMENU )


/* Video Encode Configuration */
BEGIN_MENU( menuVideoEncodeConfig )
	MENU_ITEM( TxtVideoEncodingFormat,(MenuFunc)FocusCurSel_VideoFormat, item_flag_none ),
	MENU_ITEM( TxtVideoCaptureSize,(MenuFunc)FocusCurSel_VideoSize, item_flag_none ),
	MENU_ITEM( TxtVideoFrameRateSelection,(MenuFunc)FocusCurSel_FrameRate, item_flag_none ),
	MENU_ITEM( TxtVideoBitRate,(MenuFunc)FocusCurSel_VideoBitRate, item_flag_none )
END_MENU( menuVideoEncodeConfig, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU( menuConfigParam )
    MENU_ITEM( TxtOutputScreenSize,(MenuFunc)FocusCurSel_ScreenMode, item_flag_none ),
    SUB_MENU( TxtAudioEncodeConfig,menuAudioEncodeConfig, item_flag_none ),
    SUB_MENU( TxtVideoEncodeConfig,menuVideoEncodeConfig, item_flag_none ),
    MENU_ITEM( TxtAudioPath,(MenuFunc)FocusCurSel_AudioPath, item_flag_none ),
    MENU_ITEM( TxtPreferredStorage,(MenuFunc)FocusCurSel_PreferredStorage, item_flag_none ),
    MENU_ITEM( TxtImgCaptureQuality,(MenuFunc)FocusCurSel_ImageCaptureQuality, item_flag_none ), 
    MENU_ITEM( TxtVideoPlayOption, (MenuFunc)FocusCurSel_AudioVideoPlay_Option, item_flag_none ),
    MENU_ITEM( TxtVideoRecordOption, (MenuFunc)FocusCurSel_AudioVideoRecord_Option, item_flag_none ),
    MENU_ITEM( TxtVideoRecDuration, (MenuFunc)FocusCurSel_VideoRecord_Duration, item_flag_none )
END_MENU( menuConfigParam, menuListArea, COLOUR_LIST_SUBMENU )

/* Multimedia Application Menu. */
BEGIN_MENU( menuMultimediaApp )
    SUB_MENU( TxtConfigurationParameters,	menuConfigParam,      				 item_flag_none ),
/* If MMC is available then show the menu, otherwise access directly from FFS */    
    SUB_MENU( TxtAudioVideoPlayback,	 	menuMMStorageDevice,       item_flag_none ),    
    SUB_MENU( TxtImageViewer,				menuMMImageStorageDevice,    		 item_flag_none ),
    SUB_MENU( TxtCamera,                        	menuCamera,                          		 item_flag_none),  
    MENU_ITEM( TxtAudioRecording,			mmi_Select_Audio_Recording,    item_flag_none )
END_MENU( menuMultimediaApp, menuListArea, COLOUR_LIST_SUBMENU )

/* Voice Meo Implemention */
#ifdef FF_MMI_VOICEMEMO
BEGIN_MENU( menuStartRecord )
    MENU_ITEM( TxtStartRecord,	mmi_Select_VM_Audio_Recording,    item_flag_none )
END_MENU( menuStartRecord, menuListArea, COLOUR_LIST_SUBMENU )

/* To Specify whether file access is from FFS / MMC for Image Viewer */
BEGIN_MENU( menuVMStorageDevice )
	MENU_ITEM( TxtFFS, mmi_VM_Select_AMR_Files , item_flag_none ),
	MENU_ITEM( TxtMMC, mmi_VM_Select_AMR_Files , item_flag_none )
END_MENU( menuVMStorageDevice, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuVoiceMemoUC )
    SUB_MENU( TxtVMPlayback, menuVMStorageDevice,  item_flag_none ),
    SUB_MENU( TxtVMRecord, menuStartRecord,    item_flag_none ),
#ifdef FUNCTIONALITY_NOT_IMPLEMENTED    
    MENU_ITEM( TxtVMConfigParam,  mmi_VM_Select_AMR_Files,    item_flag_none )
#endif
END_MENU( menuVoiceMemoUC, menuListArea, COLOUR_LIST_SUBMENU )
#endif
#endif

#ifdef FF_MMI_FILEMANAGER
BEGIN_MENU( menuFMListOptions )
    MENU_ITEM( TxtRename,     mmi_fm_rename ,    item_flag_none ),
    MENU_ITEM( TxtCopy,         mmi_fm_copy,         showCopyMove ),
    MENU_ITEM( TxtMove,         mmi_fm_move,        showCopyMove ),
    MENU_ITEM( TxtDelete,        mmi_fm_remove,       item_flag_none),
    MENU_ITEM( TxtProperties,  mmi_fm_properties,  item_flag_none), 
    MENU_ITEM( TxtCreateDir,  mmi_fm_newdir,  showCreate),
#if 0
    SUB_MENU( TxtSettings,    menuMidiChangeConfig, check_source_setting),
#endif
END_MENU( menuFMListOptions, menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuRootFolders )
    MENU_ITEM( TxtImageFiles, mmi_fm_display, item_flag_none),
    MENU_ITEM( TxtAudioFiles, mmi_fm_display, item_flag_none),
END_MENU( menuRootFolders, menuListArea, COLOUR_LIST_SUBMENU )
/* Sub-Menu For File Manager */
/* SD Card menu has been removed -- OMAPS00074650 */
BEGIN_MENU( menuFMDrivesOptions )
    MENU_ITEM( TxtOpen, mmi_fm_root_folders, item_flag_none),
    MENU_ITEM( TxtMemorystat, mmi_fm_memStat, item_flag_none),
    MENU_ITEM(TxtFormat, mmi_fm_format, format_check_dev) ,   
END_MENU( menuFMDrivesOptions, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

BEGIN_MENU( menuFMDrives )
    MENU_ITEM( TxtFFS, mmi_fm_drives, mmi_check_nor),
    MENU_ITEM( TxtNORMS, mmi_fm_drives, mmi_check_norms),
    MENU_ITEM( TxtNAND, mmi_fm_drives, mmi_check_nand),
    MENU_ITEM(TxtTflash, mmi_fm_drives, mmi_check_tflash),    
END_MENU( menuFMDrives, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

BEGIN_MENU( menuDestRootFolders )
    MENU_ITEM( TxtImageFiles, mmi_fm_dest_display, check_source_image),
    MENU_ITEM( TxtAudioFiles, mmi_fm_dest_display, check_source_audio),
END_MENU( menuDestRootFolders, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

BEGIN_MENU( menuDestFMDrives )
    MENU_ITEM( TxtFFS, mmi_fm_dest_root_folders, mmi_check_nor),
    MENU_ITEM( TxtNORMS, mmi_fm_dest_root_folders, mmi_check_norms),
    MENU_ITEM( TxtNAND, mmi_fm_dest_root_folders, mmi_check_nand),
    MENU_ITEM(TxtTflash, mmi_fm_dest_root_folders, mmi_check_tflash),    
END_MENU( menuDestFMDrives, menuListArea, COLOUR_LIST_SUBMENU )

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

#endif


BEGIN_MENU( menuApplications )
  MENU_ITEM( TxtCalculator,     (MenuFunc)calculator, item_flag_none ),
  #ifdef MMI_MELODYGENERATOR
  MENU_ITEM( TxtMelodygenerator,  (MenuFunc)melody_generator_aktivate,  item_flag_none ),
  #endif /* MMI_MELODYGENERATOR */
#ifdef MMIGAME
  MENU_ITEM( TxtGame,       (MenuFunc)four_in_a_row_aktivate, item_flag_none),
  #endif
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
  MENU_ITEM( TxtPowerMgnt,     (MenuFunc)mmi_PowerManagement, item_flag_none ),
#endif


  #else
  /* GW 15/09/01 removed debug code*/
  #endif /* MMIGAME */
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifndef FF_MMI_FILEMANAGER  
#ifdef FF_MMI_TEST_MIDI
   MENU_ITEM( TxtMidiTest,       mmi_midi_test_Application,        item_flag_none ),//Sep 11, 2006 DR: OMAPS00094182 xrashmic
#endif/*#ifdef FF_MMI_TEST_MIDI */
#endif /*FF_MMI_FILEMANAGER*/
	/*   Apr 06, 2005	REF: ENH 30011 xdeepadh */
#ifdef FF_MMI_CAMERA_APP
    MENU_ITEM(TxtCameraTest, mmi_camera_launch_view_finder,         item_flag_none ),
#endif //FF_MMI_CAMERA_APP

#ifdef FF_MMI_TEST_CAMERA
    MENU_ITEM(TxtCameraTest, mmi_camera_test_launch_view_finder,         item_flag_none ),
#endif /*FF_MMI_TEST_CAMERA */
// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
MENU_ITEM(TxtFileViewer, LaunchFileViewer,         item_flag_none ),
#endif //FF_MMI_FILE_VIEWER
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
#ifndef FF_MMI_FILEMANAGER
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
    MENU_ITEM( TxtPlayerTest,       mmi_audio_player_Application,        item_flag_none ),//Sep 11, 2006 DR: OMAPS00094182 xrashmic
#endif/* FF_MMI_TEST_MP3 */
#endif
#ifdef BTE_MOBILE
    SUB_MENU( TxtBluetooth,       menuBluetooth,        item_flag_none ),
#endif

#ifndef FF_NO_VOICE_MEMO
/* Voice memo Menu Display */
#ifdef FF_MMI_VOICEMEMO
   SUB_MENU( TxtVoiceMemo,       menuVoiceMemoUC,        item_flag_none ),  
#else   
    SUB_MENU( TxtVoiceMemo,       menuVoiceMemo,        item_flag_none ),
#endif
#endif

//x0pleela 24 Feb, 2006  ER OMAPS00067709
#ifdef FF_PCM_VM_VB
	SUB_MENU( TxtPcmVoiceMemo,       menuPcmVoiceMemo,        item_flag_none ),

	//x0pleela 09 Mar, 2006   ER: OMAPS00067709
	//menu item of Voice Buffering
	SUB_MENU( TxtVoiceBuffering,       menuVoiceBuffering,        item_flag_none ),
#endif
#ifdef FF_MMI_MULTIMEDIA
     /* Multimedia Application initial menu specification. */
    SUB_MENU( TxtMultimediaApp, menuMultimediaApp, item_flag_none ),
#endif
#ifdef FF_MMI_MULTIMEDIA
     /* Multimedia Application initial menu specification. */
    SUB_MENU( TxtMultimediaApp, menuMultimediaApp, item_flag_none ),
#endif
    MENU_ITEM( TxtBootTime, menuBootTimeMsmt, item_flag_none ),  /*OMAPS00091029 x0039928(sumanth)*/
#ifdef FF_MMI_FILEMANAGER    
    SUB_MENU( TxtFileMgmt, menuFMDrives, item_flag_none ), 
#endif
END_MENU( menuApplications, menuListArea, COLOUR_LIST_SUBMENU )


//Jun 19, 2006  DR: OMAPS00070657 xdeepadh
//Only selected applications can be accessed from active call screen
/*
BEGIN_MENU( activecallApps )
  #ifdef MMI_MELODYGENERATOR
  MENU_ITEM( TxtMelodygenerator,  (MenuFunc)melody_generator_aktivate,  item_flag_none ),
  #endif
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
  MENU_ITEM( TxtPowerMgnt,     (MenuFunc)mmi_PowerManagement, item_flag_none ),
#endif
#else
#endif 

// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
MENU_ITEM(TxtFileViewer, LaunchFileViewer,         item_flag_none ),
#endif //FF_MMI_FILE_VIEWER

#ifdef BTE_MOBILE
    SUB_MENU( TxtBluetooth,       menuBluetooth,        item_flag_none ),
#endif

#ifndef FF_NO_VOICE_MEMO
#ifdef FF_MMI_VOICEMEMO
   SUB_MENU( TxtVoiceMemo,       menuVoiceMemoUC,        item_flag_none ),  
#else   
    SUB_MENU( TxtVoiceMemo,       menuVoiceMemo,        item_flag_none ),
#endif
#endif

#ifdef FF_PCM_VM_VB
	SUB_MENU( TxtPcmVoiceMemo,       menuPcmVoiceMemo,        item_flag_none ),

	//x0pleela 09 Mar, 2006   ER: OMAPS00067709
	//menu item of Voice Buffering
	SUB_MENU( TxtVoiceBuffering,       menuVoiceBuffering,        item_flag_none ),
#endif

#ifdef FF_MMI_MULTIMEDIA
    SUB_MENU( TxtMultimediaApp, menuMultimediaApp, item_flag_none ),
#endif
END_MENU( activecallApps, menuListArea, COLOUR_LIST_SUBMENU )
*/
/*---------------------------------------------------------------------------*/

/*
 *  Settings menus
 */
/* Sub-Menu of Language */
BEGIN_MENU( menuLanguage )
    MENU_ITEM( TxtEnglish,    Mmi_Set2English,   item_flag_none ),
#ifndef CHINESE_MMI
  MENU_ITEM( TxtGerman,   Mmi_Set2German,    item_flag_none )
#else
    MENU_ITEM( TxtChinese,    Mmi_Set2Chinese,    item_flag_none )
#endif
END_MENU( menuLanguage, menuListArea, COLOUR_LIST_SUBMENU )


/* Sub-Menu of AnyKey */
BEGIN_MENU( menuAnyKeySet )
    MENU_ITEM( TxtOn,   settingsAnyKeyAnswerOn,  item_flag_none ),
    MENU_ITEM( TxtOff,    settingsAnyKeyAnswerOff, item_flag_none )
END_MENU( menuAnyKeySet, menuListArea, COLOUR_LIST_SUBMENU )


/* Sub-Menu of AutoAnswer */
BEGIN_MENU( menuAutoAnswerSet )
    MENU_ITEM( TxtOn,   settingsAutoAnswerOn,  item_flag_none ),
    MENU_ITEM( TxtOff,    settingsAutoAnswerOff,   item_flag_none )
END_MENU( menuAutoAnswerSet, menuListArea, COLOUR_LIST_SUBMENU )


/* Sub-Menu of AutoRedial */
BEGIN_MENU( menuAutoRedialSet )
    MENU_ITEM( TxtOn,   settingsAutoRedialOn,  item_flag_none ),
    MENU_ITEM( TxtOff,    settingsAutoRedialOff,   item_flag_none )
END_MENU( menuAutoRedialSet, menuListArea, COLOUR_LIST_SUBMENU )

/* Sub-menu of time Format */
BEGIN_MENU( menuTimeFormat )
    MENU_ITEM( TxtTwelveHour,   twelve_hour_clock,    item_flag_none ),
    MENU_ITEM( TxtTwentyfourHour, twentyfour_hour_clock,  item_flag_none ),

END_MENU( menuTimeFormat, menuListArea, COLOUR_LIST_SUBMENU )
/* Sub-menu of in-call timer */
BEGIN_MENU( menuInCallTimer )
    MENU_ITEM( TxtOn,   menuInCallTimerOn,    item_flag_none ),
    MENU_ITEM( TxtOff,    menuInCallTimerOff, item_flag_none ),

END_MENU( menuInCallTimer, menuListArea, COLOUR_LIST_SUBMENU )
/*Sub-Menu of Clock */
/* GW 15/09/01 Removed debug menu items */
BEGIN_MENU( menuTypclock )
    MENU_ITEM( TxtTimeDate,   settingsTimeDate,   item_flag_none ),
    MENU_ITEM( TxtAlarm,    alarmTimeDate,      item_flag_none ),
    MENU_ITEM( TxtClearAlarm, clear_alarm,    item_flag_none ),
/* Jul 22,2004 CRR:20896 xrashmic - SASKEN     */
/* Instead of providing the submenu directly we call a function  */
/* where we set the menu and also higlight the previously selected menu item */
    MENU_ITEM( TxtTimeFormat, settingsFocusTimeFormatCurSel,    item_flag_none ),
    SUB_MENU( TxtInCallTimer, menuInCallTimer,   item_flag_none )
END_MENU( menuTypclock, menuListArea, COLOUR_LIST_SUBMENU )

#ifdef FF_TIMEZONE
BEGIN_MENU( menuNITZ )
	MENU_ITEM( TxtAutomatic,  settingsNITZ,   item_flag_none ),
	MENU_ITEM( TxtOn, settingsNITZ,   item_flag_none ),
	MENU_ITEM( TxtOff,   settingsNITZ,  item_flag_none )
END_MENU( menuNITZ, menuListArea, COLOUR_LIST_SUBMENU )
#endif
/* Sub-Menu of KeypadTones */
BEGIN_MENU( menuSetTones2 )
    MENU_ITEM( TxtSilent, soundsKeypadSilent,  item_flag_none ),
    MENU_ITEM( TxtClick,  soundsKeypadClick,   item_flag_none ),
    MENU_ITEM( TxtDTMF,   soundsKeypadDTMF,  item_flag_none )
END_MENU( menuSetTones2, menuListArea, COLOUR_LIST_SUBMENU )

/* Sub-Menu of Volume Settings */
BEGIN_MENU( menuSetVolume )
    MENU_ITEM( TxtSilent, soundsVolSilent,    item_flag_none ),
    MENU_ITEM( TxtLow,    soundsVolLow,  item_flag_none ),
    MENU_ITEM( TxtMedium, soundsVolMedium,  item_flag_none ),
    MENU_ITEM( TxtHigh,   soundsVolHigh,   item_flag_none ),
/* Apr 05, 2005    REF: ENH 29994 xdeepadh */
#ifndef FF_MIDI_RINGER   
   /* Increasing option is not present for  Generic Midi Ringer */
    MENU_ITEM( TxtIncreasing, soundsVolInc,      item_flag_none )
#endif
END_MENU( menuSetVolume, menuListArea, COLOUR_LIST_SUBMENU )

/* Nov 29, 2004    REF: CRR 25051 xkundadu */
/* Description: INCALL SCREEN ?ADJUSTING THE VOLUME */
/* Fix: Added these menu items. If the user presses MENU_UP/DOWN */
/*       this menu will come up to allow the user to select the speaker volume  */
BEGIN_MENU( menuSelectVolume )
    MENU_ITEM( TxtLevel1,SetVolumeLevel1,    item_flag_none ),
    MENU_ITEM( TxtLevel2, SetVolumeLevel2,  item_flag_none ),
    MENU_ITEM( TxtLevel3,SetVolumeLevel3,  item_flag_none ),
    MENU_ITEM( TxtLevel4, SetVolumeLevel4,   item_flag_none ),
    MENU_ITEM( TxtLevel5, SetVolumeLevel5,      item_flag_none )
END_MENU( menuSelectVolume, menuListArea, COLOUR_LIST_SUBMENU )

/* Sub-Menu For PCM / MP3 / AMR file selection for ring tones */ 
#ifdef FF_MMI_RINGTONE
BEGIN_MENU( menuRingerPcmFileSelection )
    MENU_ITEM( TxtFFS, mmi_PCMRinger_Files, item_flag_none),
    MENU_ITEM( TxtMMC, mmi_PCMRinger_Files, item_flag_none),
END_MENU( menuRingerPcmFileSelection, menuListArea, COLOUR_LIST_SUBMENU )    

BEGIN_MENU( menuRingerMp3FileSelection )
    MENU_ITEM( TxtFFS, mmi_MP3Ringer_Files, item_flag_none),
    MENU_ITEM( TxtMMC, mmi_MP3Ringer_Files, item_flag_none),
END_MENU( menuRingerMp3FileSelection, menuListArea, COLOUR_LIST_SUBMENU ) 

BEGIN_MENU( menuRingerAmrFileSelection )
    MENU_ITEM( TxtFFS, mmi_AMRNBRinger_Files, item_flag_none),
    MENU_ITEM( TxtMMC, mmi_AMRNBRinger_Files, item_flag_none),
END_MENU( menuRingerAmrFileSelection, menuListArea, COLOUR_LIST_SUBMENU ) 
BEGIN_MENU( menuSelectTones )
    SUB_MENU( TxtPCM,   menuRingerPcmFileSelection,  item_flag_none ),
    SUB_MENU( TxtMP3,    menuRingerMp3FileSelection,  item_flag_none ),
    SUB_MENU( TxtAMR,   menuRingerAmrFileSelection,  item_flag_none )
END_MENU( menuSelectTones, menuListArea, COLOUR_LIST_SUBMENU )
#endif

/* Sub-Menu of Ringer,SMS Tone,Alarm */
BEGIN_MENU( menuSetTones )
#ifdef FF_MMI_RINGTONE
     SUB_MENU( TxtMelody,   menuSelectTones,  item_flag_none ),
#else
    MENU_ITEM( TxtMelody,   soundsRinger,  item_flag_none ),
#endif    
    MENU_ITEM_SUB_MENU( TxtVolume,    menuSetVolume,soundsSetVolume, item_flag_none )
END_MENU( menuSetTones, menuListArea, COLOUR_LIST_SUBMENU )

/* Sub-Menu of Tones */
BEGIN_MENU( menuTonesSet )
    MENU_ITEM_SUB_MENU( TxtRinger,    menuSetTones,soundReasonRinger,  item_flag_none ),
    MENU_ITEM_SUB_MENU( TxtSmsTone,   menuSetTones,soundReasonSmsTone,   item_flag_none ),
    MENU_ITEM_SUB_MENU( TxtAlarm,     menuSetTones,soundReasonAlarm,   item_flag_none ),
    SUB_MENU( TxtKeypadTones, menuSetTones2,   item_flag_none )
END_MENU( menuTonesSet, menuListArea, COLOUR_LIST_SUBMENU )


/* Sub-Menu of Network Selection */
BEGIN_MENU( menuNetworkSet )
    MENU_ITEM( TxtAutomatic,(MenuFunc)network_set_mode_auto, item_flag_none ),
    MENU_ITEM( TxtManual, (MenuFunc)network_set_mode_man,  item_flag_none ),
    MENU_ITEM( TxtList,   (MenuFunc)network_pref_list,   item_flag_none )
END_MENU( menuNetworkSet, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-Menu of Hide ID
BEGIN_MENU( menuHideIDSet )
    MENU_ITEM( TxtCheckStatus,  servicesCheckCLIR   ,    item_flag_none ),
    MENU_ITEM( TxtActivate   ,  servicesActivateCLIR  ,    item_flag_none ),
    MENU_ITEM( TxtDeactivate ,  servicesDeactivateCLIR  ,  item_flag_none )
END_MENU( menuHideIDSet, menuListArea, COLOUR_LIST_SUBMENU )

/* Marcus: CCBS: 13/11/2002: Start */
//Sub-Menu of CCBS
BEGIN_MENU( menuCCBS )
    MENU_ITEM( TxtCheckStatus,  call_ccbs,  item_flag_none ),
    MENU_ITEM( TxtActivate   ,  call_ccbs,  item_flag_none ),
    MENU_ITEM( TxtDeactivate ,  call_ccbs,  item_flag_none )
END_MENU( menuCCBS, menuListArea, COLOUR_LIST_SUBMENU1 )
/* Marcus: CCBS: 13/11/2002: End */

//Sub-Menu of Call Waiting
BEGIN_MENU( menuCallWaitingSet )
    MENU_ITEM( TxtCheckStatus,  servicesCheckCWaiting,     item_flag_none ),
    MENU_ITEM( TxtActivate,   servicesActivateCWaiting,  item_flag_none ),
    MENU_ITEM( TxtDeactivate, servicesDeactivateCWaiting,  item_flag_none )
END_MENU( menuCallWaitingSet, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-Menu
BEGIN_MENU( menuCallBarring)
    MENU_ITEM ( TxtCheckStatus, servicesCallBarringCheck,  item_flag_none ),
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: start
//Selecting this menuItem calls serivcesCallBarringActivatePassword() - sets a flag indicating activation
    MENU_ITEM ( TxtSetBar,    servicesCallBarringActivatePassword, item_flag_none ),
 //Selecting this menuItem calls serivcesCallBarringDeactivatePassword() - sets a flag indicating deactivation
    MENU_ITEM ( TxtCancel,  servicesCallBarringDeactivatePassword,item_flag_none )
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: end
END_MENU( menuCallBarring, menuListArea, COLOUR_LIST_SUBMENU )


//Sub-Menu of Call Barring
BEGIN_MENU( menuBarringSet )
    MENU_ITEM_SUB_MENU ( TxtOutgoing,   menuCallBarring,   servicesBarringAllOutgoing,item_flag_none  ),
    MENU_ITEM_SUB_MENU ( TxtInterCalls,   menuCallBarring,   servicesBarringInternational,item_flag_none  ),
    MENU_ITEM_SUB_MENU ( TxtInterHome,    menuCallBarring,   servicesBarringInternationalHome,item_flag_none  ),
    MENU_ITEM_SUB_MENU ( TxtIncomingCall, menuCallBarring,   servicesBarringAllIncoming ,item_flag_none ),
    MENU_ITEM_SUB_MENU ( TxtWhenRoaming,  menuCallBarring,   servicesBarringIncomingRoaming,item_flag_none  ),
    
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: start
//Selecting this menuItem calls servicesCallBarringCancelAllPassword - sets a flag to cancel all barring
//and also prompts for the password
    MENU_ITEM( TxtCancelAllBars,  servicesCallBarringCancelAllPassword,  item_flag_none ),

//Jun 02,2004 CRR:13649 xpradipg-SASKEN: start    
//Selecting this menuItem calls enterOldPassword - sets a flag for change of password and also 
//prompts for the password
    MENU_ITEM( TxtChangePassword, enterOldCBPassword,  item_flag_none )
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: end
END_MENU( menuBarringSet, menuListArea, COLOUR_LIST_SUBMENU )


BEGIN_MENU(menuBandSelect)
 MENU_ITEM(TxtStatus,          mmi_band_radio_mode_info,              item_flag_none),
  MENU_ITEM(TxtAutomatic,      mmi_band_radio_mode_change_auto,       item_flag_none),
  MENU_ITEM(TxtManual,        mmi_band_radio_mode_change_manual,     item_flag_none ),

END_MENU(menuBandSelect, menuListArea, COLOUR_LIST_SUBMENU)

/*SPR 1392, call deflection menu*/
BEGIN_MENU(menuCallDeflection)
  MENU_ITEM(TxtOn,          call_deflection_on,              item_flag_none),
  MENU_ITEM(TxtOff,      call_deflection_off,       item_flag_none)
END_MENU(menuCallDeflection, menuListArea, COLOUR_LIST_SUBMENU1)


//x0pleela 23 May, 2006  DR: OMAPS00070657
//Adding submenu for the user to select line 1 or line 2
#ifdef FF_CPHS
  BEGIN_MENU(cphs_edit_line)
    MENU_ITEM(TxtLine1, cphs_edit_line1, item_flag_none),
    MENU_ITEM(TxtLine2, cphs_edit_line2,item_flag_none)
  END_MENU(cphs_edit_line,menuListArea,COLOUR_LIST_SUBMENU)
#endif

//x0pleela 07 june, 2006  DR: OMAPS00070657
//the following menu items should be displayed for both flags MMI_GPRS_ENABLED and FF_CPHS
#if defined(MMI_GPRS_ENABLED) || defined(FF_CPHS)
BEGIN_MENU(menuLineSelect)
  MENU_ITEM(TxtStatus,      cphs_als_status,      item_flag_none),
  MENU_ITEM(TxtLine1,       cphs_line1_select,      CphsALSUnlocked),
  MENU_ITEM(TxtLine2,       cphs_line2_select,      CphsALSUnlocked ),
  MENU_ITEM(TxtLockAls,   CphsAlsLock,   CphsALSUnlocked ),
  MENU_ITEM(TxtUnlockAls, CphsAlsUnlock, CphsALSLocked )
#ifdef FF_CPHS
	,
  SUB_MENU(TxtEditLineName,   cphs_edit_line,      item_flag_none) 
#endif
  END_MENU(menuLineSelect, menuListArea,COLOUR_LIST_SUBMENU )
#endif


#ifdef MMI_GPRS_ENABLED

BEGIN_MENU( menuSMSService )
    MENU_ITEM( TxtGPRS,       GPRS_MenuSelectSms_set,     item_flag_none  ),
    MENU_ITEM( TxtCSD,        GPRS_MenuSelectSms_set,     item_flag_none  ),
    MENU_ITEM( TxtGPRSPreferred,  GPRS_MenuSelectSms_set,     item_flag_none  ),
    MENU_ITEM( TxtCSDPreferred,   GPRS_MenuSelectSms_set,     item_flag_none  )
END_MENU( menuSMSService , menuListArea, COLOUR_LIST_SUBMENU )

BEGIN_MENU( menuDataCount )
    MENU_ITEM ( TxtIncomingData,    GPRS_MenuIncomingDataCount,   item_flag_none  ),
    MENU_ITEM ( TxtOutgoingData,    GPRS_MenuOutgoingDataCount,   item_flag_none  )
END_MENU( menuDataCount, menuListArea, COLOUR_LIST_SUBMENU )


#ifdef NEPTUNE_BOARD  /* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef MMI_TTY_ENABLED
BEGIN_MENU(menuTtyAudioMode)
  MENU_ITEM(TxtAudioModeText,       ctty_audio_mode_text,      item_flag_none),
  MENU_ITEM(TxtAudioModeVco,       ctty_audio_mode_vco,      item_flag_none ),
  MENU_ITEM(TxtAudioModeUnknown,   ctty_audio_mode_unknown,   item_flag_none ),
END_MENU(menuTtyAudioMode, menuListArea,COLOUR_LIST_SUBMENU )
#endif
#endif

/* GPRS Submenu */

BEGIN_MENU( menuDataSettings )
  MENU_ITEM( TxtAttach,     GPRS_MenuAttach,         item_flag_none ),
  MENU_ITEM( TxtDetach,     GPRS_MenuDetach,         item_flag_none ),
  SUB_MENU ( TxtDataCount,    menuDataCount,        item_flag_none ),
  MENU_ITEM ( TxtSMS,       GPRS_MenuSelectSms,        item_flag_none ),
/* SPR#2346 - SH - Removed GPRS test options */
END_MENU( menuDataSettings,			menuListArea, COLOUR_LIST_SUBMENU )
#endif  //MMI_GPRS_ENABLED

//SPR#1113 - DS - New menu "Netw. Related" added
//SPR#1347 - GW - Moved network menu outside of '#ifdef MMI_GPRS_ENABLED'
//					Disabled 'Band selection' when GPRS is disabled.
//Menu of Network Related services
BEGIN_MENU( menuNetwork )
//x0pleela 25 May, 2006  DR: OMAPS00070657
#if defined(MMI_GPRS_ENABLED) || defined(FF_CPHS)
    SUB_MENU( TxtSelectLine,      menuLineSelect,      ALSPresent),
#endif
#ifdef FF_CPHS_REL4  
    MENU_ITEM(TxtMSP, build_profile_list, MSPPresent),
#endif    
    SUB_MENU ( TxtCallDivert,  menuDivert,  item_flag_none),
    SUB_MENU( TxtNetworkSel,    menuNetworkSet,         item_flag_none ),
    SUB_MENU(TxtBand, 			menuBandSelect, 	item_flag_none),
    
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: start
//points to the menu structure of differnt callBarring services  instead of calling the password entry screen
    SUB_MENU(TxtCallBarringAll,	menuBarringSet, 	item_flag_none),
//Jun 02,2004 CRR:13649 xpradipg-SASKEN: end  

    SUB_MENU( TxtCallWaiting,   menuCallWaitingSet,       item_flag_none ),
    SUB_MENU( TxtHideID,      menuHideIDSet,          item_flag_none ),
    SUB_MENU(TxtCallBack, menuCCBS, item_flag_none ), // Marcus: CCBS: 13/11/2002
    SUB_MENU(TxtCallDeflection, menuCallDeflection, item_flag_none ) //SPR 1392
#ifdef MMI_TTY_ENABLED
	,	/* Comma here so previous item won't report error when flag switched off */
	MENU_ITEM( TxtTTY,			call_tty_menu,				item_flag_none)	/* SPR#1352 - SH - TTY menu */
#ifdef NEPTUNE_BOARD  /* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef MMI_TTY_ENABLED
       ,  /* Comma here so previous item won't report error when flag switched off */
       SUB_MENU( TxtAudioMode,   menuTtyAudioMode,       item_flag_none )
#endif
#endif

#endif

END_MENU( menuNetwork, menuListArea, COLOUR_LIST_SUBMENU )


// Menu of Settings
#ifdef _TESTING__
BEGIN_MENU(menuCphsTesting)
	MENU_ITEM (TxtRead, 			Cphs_reading_test, 			item_flag_none),
	MENU_ITEM(TxtWrite, 			Cphs_writing_test, 			item_flag_none)
END_MENU(menuCphsTesting, menuListArea,COLOUR_LIST_SUBMENU)
#endif

#ifdef COLOURDISPLAY
int storeMenuOption(MfwMnu* mnu, MfwMnuItem* mnuItem);
int colMenuSelected(MfwMnu* mnu, MfwMnuItem* mnuItem);
int icon_bgdMenuSelected(MfwMnu* mnu, MfwMnuItem* mnuItem);

BEGIN_MENU(menuColours)
	MENU_ITEM (TxtColourBlack,	 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourBlue, 			colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourCyan, 			colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourGreen, 			colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourMagenta, 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourYellow,	 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourRed, 			colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourWhite,	 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourPink,	 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourOrange,		 	colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourLightBlue, 		colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourLightGreen, 	colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourPaleYellow, 	colMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtColourTransparent, 	colMenuSelected, 			item_flag_none),
END_MENU_MULTICOLOUR(menuColours, menuListArea,COLOUR_LIST_COLOUR,TxtNull)

BEGIN_MENU(menuColourObjects)
	MENU_ITEM_SUB_MENU (TxtIdleScreenBackground,		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtIdleScreenSoftKey,  			menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtIdleScreenText,  			menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtIdleScreenNetworkName,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtGeneralBackground ,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtMenuSoftKeyForeground ,  	menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtMenuSoftKeyBackground ,  	menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtSubMenuHeadingText ,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtSubMenuHeadingBackground,  	menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtSubMenuTextForeground ,  	menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtSubMenuTextBackground,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtSubMenuSelectionBar,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtPopUpTextForeground,  		menuColours, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtPopUpTextBackground ,  		menuColours, storeMenuOption,	item_flag_none),
END_MENU_MULTICOLOUR(menuColourObjects, menuListArea,COLOUR_LIST_COLOUR,TxtColourMenu)

BEGIN_MENU(menuBackdrops)
	MENU_ITEM (TxtNone, 		icon_bgdMenuSelected, 			item_flag_none),
//	MENU_ITEM (TxtBgdSunset, 	icon_bgdMenuSelected, 			item_flag_none),
//	MENU_ITEM (TxtBgdSquares, 	icon_bgdMenuSelected, 			item_flag_none),
//	MENU_ITEM (TxtBgdTree, 		icon_bgdMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtBgdEdinburgh1, icon_bgdMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtBgdEdinburgh2, icon_bgdMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtBgdEdinburgh3, icon_bgdMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtBgdEdinburgh4, icon_bgdMenuSelected, 			item_flag_none),
	MENU_ITEM (TxtBgdTILogo, 	icon_bgdMenuSelected, 			item_flag_none)
END_MENU(menuBackdrops, menuListArea,COLOUR_LIST_SUBMENU)

BEGIN_MENU(menuBackgroundScrSelect)
	MENU_ITEM_SUB_MENU (TxtIdleScreen,		menuBackdrops, storeMenuOption, 	item_flag_none),
	MENU_ITEM_SUB_MENU (TxtMenuScreen,		menuBackdrops, storeMenuOption, 	item_flag_none),
END_MENU(menuBackgroundScrSelect, menuListArea,COLOUR_LIST_SUBMENU)

BEGIN_MENU(menuProviderIconSet)
	MENU_ITEM (TxtYes,		icon_networkShow, 	item_flag_none),
	MENU_ITEM (TxtNo,		icon_networkDontShow, 	item_flag_none),
END_MENU(menuProviderIconSet, menuListArea,COLOUR_LIST_SUBMENU)

#endif

/*
  * SPR#1113 - DS - Removed network related services from "Phone Settings" menu
  */
  //xrashmic 9 Sep 2006, OMAPS00092732
//xashmic 27 Sep 2006, OMAPS00096389
#ifdef FF_MMI_USBMS
BEGIN_MENU(menuPS)
	MENU_ITEM (TxtPSShutdown,		setUSBMSPS, 	item_flag_none),
	MENU_ITEM (TxtNoPSShutdown,	setUSBMSPS,  	item_flag_none),
END_MENU(menuPS, menuListArea,COLOUR_LIST_SUBMENU)
BEGIN_MENU(menuPopup)
	MENU_ITEM (TxtYes,		setUSBMSPopup, 	item_flag_none),
	MENU_ITEM (TxtNo,	setUSBMSPopup,  	item_flag_none),
END_MENU(menuPopup, menuListArea,COLOUR_LIST_SUBMENU)
BEGIN_MENU( menuUSBPortOptions )
    MENU_ITEM( TxtUSBMS,  mmi_usb_enumeration,    item_flag_none ),
    MENU_ITEM( TxtUSBTrace,  mmi_usb_enumeration,  item_flag_none ),
    MENU_ITEM( TxtUSBFax,  mmi_usb_enumeration,    item_flag_none ),
    MENU_ITEM( TxtUSBFaxnTrace,  mmi_usb_enumeration,    item_flag_none ),
    MENU_ITEM( TxtUSBMSnTrace,  mmi_usb_enumeration,  item_flag_none ),
    MENU_ITEM( TxtUSBFaxnMS,  mmi_usb_enumeration,    item_flag_none ),
    MENU_ITEM( TxtAll,  mmi_usb_enumeration,    item_flag_none ),
END_MENU( menuUSBPortOptions, menuListArea, COLOUR_LIST_SUBMENU )
BEGIN_MENU(menuUSBMS)
	SUB_MENU (TxtPS,		menuPS, 	item_flag_none),
	SUB_MENU (TxtPopupMenu,	menuPopup,  	item_flag_none),
	SUB_MENU(TxtUSBMSPorts, menuUSBPortOptions, item_flag_none),
END_MENU(menuUSBMS, menuListArea,COLOUR_LIST_SUBMENU)
#endif

/*OMAPS00098881(removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
BEGIN_MENU(menuScreenUpdate)
	MENU_ITEM(TxtOn, screenUpdateOn, item_flag_none),
	MENU_ITEM(TxtOff, screenUpdateOff,item_flag_none)	
END_MENU(menuScreenUpdate, menuListArea, COLOUR_LIST_SUBMENU)
#endif


BEGIN_MENU( menuSet )
#ifdef _TESTING__
    SUB_MENU(TxtExtra, 				menuCphsTesting, 			item_flag_none),
#endif
    SUB_MENU( TxtLanguages,       menuLanguage,               item_flag_none ),
    #ifdef MMI_GPRS_ENABLED
    SUB_MENU( TxtGPRS,        menuDataSettings,     item_flag_none ),
    #endif
  SUB_MENU( TxtTones,       menuTonesSet,         item_flag_none ),
    SUB_MENU( TxtClock,       menuTypclock,       item_flag_none ),
    SUB_MENU( TxtAutoredial,    menuAutoRedialSet,      item_flag_none ),
    #ifdef FF_TIMEZONE
    SUB_MENU( TxtNitzTimeUpdate,       menuNITZ,       item_flag_none ),
    #endif
//Jul 28, 2004 REF:CRR 20899 xkundadu
//Changed submenu to menuItem to call a function.
//whenever the user presses the 'AutoAnswer' menu, the function 
//settingsFocusCurSelAnyKeyAns() will be called, which sets the focus to the
//  'On/Off' menu item depending on whether 'AutoAnswer' is enabld or not.
     MENU_ITEM( TxtAutoAnswer,(MenuFunc)settingsFocusCurSelAutoAns, item_flag_none ),

//Jul 28, 2004 REF: CRR 20899 xkundadu
//Changed submenu to menuItem to call a function.
//whenever the user presses the 'Any key Answer' menu, the function 
//settingsFocusCurSelAnyKeyAns() will be called, which sets the focus to the
//  'On/Off' menu item depending on whether 'Any Key answer is enabled or not.
  MENU_ITEM( TxtAnyKeyAnswer,(MenuFunc)settingsFocusCurSelAnyKeyAns, item_flag_none ),
  MENU_ITEM( TxtFactoryReset,   factoryReset,         item_flag_none ),
#ifdef COLOURDISPLAY
    SUB_MENU( TxtColourMenu, 	menuColourObjects,			item_flag_none ),
    SUB_MENU( TxtBackground,	menuBackgroundScrSelect,	item_flag_none ),
// change by Sasken (VVADIRAJ) on March 31th 2004
// Issue Number : MMI-SPR-12822
// changed submenu to menuitem to call a function   
//    SUB_MENU( TxtProviderIcon,	menuProviderIconSet,		item_flag_none ),
    MENU_ITEM( TxtProviderIcon,	    (MenuFunc)settingsFocusNwIconCurSel,	item_flag_none ),
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
    SUB_MENU( TxtUSBMS, 	menuUSBMS,			item_flag_none ),
#endif
/*OMAPS00098881(removing power variant) a0393213(Prabakar)*/
#ifdef FF_POWER_MANAGEMENT
    SUB_MENU(TxtScreenUpdate, menuScreenUpdate, item_flag_none)
#endif
END_MENU_HEADER( menuSet, menuListArea,COLOUR_LIST_SUBMENU, TxtSettings )
/*---------------------------------------------------------------------------*/


/* SPR890 - SH - Test mode menu */
#ifdef MMI_TEST_MODE
BEGIN_MENU( menuTestMode )
	MENU_ITEM_SELECT( TxtTest,	test_function, item_flag_none, testSelected),
	MENU_ITEM( TxtTest,	test_function,			item_flag_none ),
	MENU_ITEM( TxtTest,		test_function,			item_flag_none ),
	MENU_ITEM( TxtTest,		test_function,			item_flag_none )
END_MENU( menuTestMode, menuListArea )
#endif
/*---------------------------------------------------------------------------*/

/* SPR890 - SH - Test mode menu */
#ifdef MMI_TEST_MODE
BEGIN_MENU( menuTestMode )
	MENU_ITEM_SELECT( TxtTest,	test_function, item_flag_none, testSelected),
	MENU_ITEM( TxtTest,	test_function,			item_flag_none ),
	MENU_ITEM( TxtTest,		test_function,			item_flag_none ),
	MENU_ITEM( TxtTest,		test_function,			item_flag_none )
END_MENU( menuTestMode, menuListArea )
#endif
/*---------------------------------------------------------------------------*/

extern int menuEmobiixItemCallback(MfwMnu* m, MfwMnuItem* i);
extern int menuEmobiixItemCallback2(MfwMnu* m, MfwMnuItem* i);
extern int menuEmobiixItemCallback3(MfwMnu* m, MfwMnuItem* i);
extern int menuEmobiixItemCallback4(MfwMnu* m, MfwMnuItem* i);
extern int menuEmobiixItemCallback5(MfwMnu* m, MfwMnuItem* i);

/* Main system menu, keep this here until we get the real one implemented
*/
BEGIN_ICON_MENU( menuMainMenu )
    MENU_ICON_ITEM( &mainIconWAP, TxtEmobiixApps,  menuEmobiixItemCallback,     item_flag_none ),
    MENU_ICON_ITEM( &mainIconMessages, TxtEmobiixApps,  menuEmobiixItemCallback2,     item_flag_none ),
    MENU_ICON_ITEM( &mainIconPhonebook, TxtEmobiixApps,  menuEmobiixItemCallback3,     item_flag_none ),
    MENU_ICON_ITEM( &mainIconRecentCalls, TxtEmobiixApps,  menuEmobiixItemCallback4,     item_flag_none ),
    MENU_ICON_ITEM( &mainIconCallDivert, TxtEmobiixApps,  menuEmobiixItemCallback5,     item_flag_none ),
    //SUB_ICON_MENU ( &mainIconMessages,TxtMessages,    menuMsg,  				item_flag_none ),
    //SUB_ICON_MENU ( &mainIconPhonebook,TxtPhonebook,   menuPhbk,                item_flag_none ),
    //SUB_ICON_MENU ( &mainIconRecentCalls,TxtRecentCalls, menuRecent,        item_flag_none ),
    //SUB_ICON_MENU ( &mainIconCallDivert,TxtNetworkServices,  menuNetwork,       item_flag_none ),
    //SUB_ICON_MENU ( &mainIconSecurity,TxtSecurity,    menuSecurity,     item_flag_none ),
    //SUB_ICON_MENU ( &mainIconCalculator,TxtApplications, menuApplications,    item_flag_none ),
    //SUB_ICON_MENU ( &mainIconSettings,TxtPhoneSettings,    menuSet,          item_flag_none ),
END_ICON_MENU( menuMainMenu, menuListArea, COLOUR_LIST_MAIN, &mainIconBackground )



/*******************************************************************************

                           Public Menu Access Routines

*******************************************************************************/

#ifdef SIM_PERS
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
//function to call the new menu "menuBootupCatList" from any file
MfwMnuAttr *SIMPCategoryListAttributes( void )
{
    return (MfwMnuAttr *) &menuBootupCatList;
}

#endif


#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
/*MC, SPR1209 return engineering mode menu*/
MfwMnuAttr *EngModeAttributes( void )
{
    return (MfwMnuAttr *) &menuEngMode;
}
#endif

MfwMnuAttr *bookOwnNumberMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuOwnNumber;
}
MfwMnuItem *bookOwnNumberMenuItems( void )
{
    return (MfwMnuItem *) menuOwnNumberItems;
}

MfwMnuAttr *bookPhonebookMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuPhbk;
}
MfwMnuItem *bookPhonebookMenuItems( void )
{
    return (MfwMnuItem *) menuPhbkItems;
}

MfwMnuAttr *bookRepRedOptionsMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuRepRedOptions;
}
MfwMnuItem *bookRepRedOptionsMenuItems( void )
{
    return (MfwMnuItem *) menuRepRedOptionsItems;
}
#ifdef FF_MMI_CPHS
MfwMnuAttr *bookEmergMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuEmergOptions;
}
#endif
MfwMnuAttr *bookNameMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuNameOptions;
}
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	FDN Numbers menu
MfwMnuAttr *bookNameMenuFDNAttributes( void )
{
    return (MfwMnuAttr *) &bookFDNNumbers;
}

MfwMnuItem *bookNameMenuItems( void )
{
    return (MfwMnuItem *) menuNameOptionsItems;
}

//Nov 29, 2004    REF: CRR 25051 xkundadu
//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//Fix: Display the menu items.

MfwMnuAttr *ringerVolumeLevels( void )
{
	return (MfwMnuAttr *) &menuSelectVolume;
}

/*MC, SPR 1327, access UPN options menu*/
MfwMnuAttr *bookUPNMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuUPNOptions;
}
MfwMnuAttr *bookMainMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuMainMenu;
}
MfwMnuItem *bookMainMenuItems( void )
{
    return (MfwMnuItem *) menuMainMenuItems;
}

MfwMnuAttr *bookCallBarringMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuBarringSet;
}

MfwMnuAttr *calcOptionMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuCalcOptions;
}
MfwMnuItem *calcOptionMenuItems( void )
{
    return (MfwMnuItem *) menuCalcOptionsItems;
}
//May 27, 2004    REF: CRR 19186  Deepa M.D 
//returns the pointer to the SMS Settings Menu.
MfwMnuAttr *settingMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuSetMsg;
}
// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
//Camera Qulaity menu
MfwMnuAttr *qualityMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuSetQuality;
}

MfwMnuAttr *colorMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuSetColor;
}
#endif //FF_MMI_TEST_CAMERA
#ifdef FF_MMI_CAMERA_APP
MfwMnuAttr *CameraMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuSetCameraparams;
}
#endif //FF_MMI_CAMERA_APP
//xashmic 27 Sep 2006, OMAPS00096389 
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
MfwMnuAttr *USBMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuUSBPortOptions;
} 
#endif

// Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
MfwMnuAttr *rotateMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuRotateActivate;
}


MfwMnuAttr *fileviewerMenuAttributes( void )//file viewer
{
	return (MfwMnuAttr *) &menuFileViewer;
}
#endif

#ifdef FF_MMI_FILE_VIEWER
MfwMnuAttr *ViewOptionsMenuAttributes( void )//file viewer
{
	return (MfwMnuAttr *) &menuViewOptions;
}
#endif

//Jun 19, 2006  DR: OMAPS00070657 xdeepadh
//Only selected applications can be accessed from active call screen
//returns the pointer to the Applications  Menu.
/*
MfwMnuAttr *applicationsMenuAttributes(void)
{
  return (MfwMnuAttr *) &activecallApps;
}
*/
#ifdef MMI_MELODYGENERATOR
MfwMnuAttr *MelgenOptionMenuAttributes( void )
{
    return (MfwMnuAttr *) &menuMelgenOptions;
}
MfwMnuItem *MelgenOptionMenuItems( void )
{
    return (MfwMnuItem *) menuMelgenOptionsItems;
}
#endif // MMI_MELODYGENERATOR

MfwMnuAttr *ringerVolumeSetting( void )
{
    return (MfwMnuAttr *) &menuSetVolume;
}


#ifdef FF_WAP

MfwMnuAttr *WAPMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuWAP;
}

MfwMnuItem *WAPMenuItems(void)
{
  return (MfwMnuItem *) menuWAPItems;
}

MfwMnuAttr *WAPBookOptMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuBookmarkOptions;
}

MfwMnuItem *WAPBookOptMenuItems(void)
{
  return (MfwMnuItem *) menuBookmarkOptionsItems;
}

MfwMnuAttr *WAPHistOptMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuHistoryOptions;
}

MfwMnuItem *WAPHistOptMenuItems(void)
{
  return (MfwMnuItem *) menuHistoryOptionsItems;
}

MfwMnuAttr *WAPProfOptionsAttributes(void)
{
  return (MfwMnuAttr *) &menuProfileOptions; /* SPR#2324 - SH */
}

MfwMnuItem *WAPProfOptionsItems(void)
{
  return (MfwMnuItem *) menuProfileOptionsItems; /* SPR#2324 - SH */
}

MfwMnuAttr *WAPGoToAttributes(void)
{
  return (MfwMnuAttr *) &menuGoTo;
}

MfwMnuItem *WAPGoToItems(void)
{
  return (MfwMnuItem *) menuGoToItems;
}

MfwMnuAttr *WAPConnectionTypeAttributes(void)
{
  return (MfwMnuAttr *) &menuWAPConnectionType;
}

MfwMnuItem *WAPConnectionTypeItems(void)
{
	return (MfwMnuItem *) menuWAPConnectionTypeItems;
}

/* SPR#1983 - SH */

MfwMnuAttr *WAPConnectionSpeedAttributes(void)
{
  return (MfwMnuAttr *) &menuWAPConnectionSpeed;
}

MfwMnuItem *WAPConnectionSpeedItems(void)
{
	return (MfwMnuItem *) menuWAPConnectionSpeedItems;
}

/* SPR#2324 - SH - "access type" menu is now GPRS/Dialup menu */

MfwMnuAttr *WAPGPRSDialupAttributes(void)
{
  return (MfwMnuAttr *) &menuWAPGPRSDialup;
}

MfwMnuItem *WAPGPRSDialupItems(void)
{
	return (MfwMnuItem *) menuWAPGPRSDialupItems;
}

MfwMnuAttr *WAPGPRSProfileAttributes(void)
{
  return (MfwMnuAttr *) &menuGPRSProfileOptions;
}

MfwMnuItem *WAPGPRSProfileItems(void)
{
  return (MfwMnuItem *) menuGPRSProfileOptionsItems;
}

MfwMnuAttr *WAPDialupProfileAttributes(void)
{
  return (MfwMnuAttr *) &menuDialupProfileOptions;
}

MfwMnuItem *WAPDialupProfileItems(void)
{
  return (MfwMnuItem *) menuDialupProfileOptionsItems;
}

MfwMnuAttr *WAPAdvancedProfileAttributes(void)
{
  return (MfwMnuAttr *) &menuAdvancedProfileOptions;
}

MfwMnuItem *WAPAdvancedProfileItems(void)
{
  return (MfwMnuItem *) menuAdvancedProfileOptionsItems;
}

/* end SPR#2324 */

MfwMnuAttr *WAPSecurityOptionAttributes(void)
{
  return (MfwMnuAttr *) &menuSecurityOption;
}

MfwMnuItem *WAPSecurityOptionItems(void)
{
  return (MfwMnuItem *) menuSecurityOptionItems;
}

MfwMnuAttr *WAPSaveHistoryAttributes(void)
{
  return (MfwMnuAttr *) &menuSaveHistory;
}

MfwMnuItem *WAPSaveHistoryItems(void)
{
  return (MfwMnuItem *) menuSaveHistoryItems;
}

MfwMnuAttr *WAPScaleImagesAttributes(void)
{
  return (MfwMnuAttr *) &menuScaleImages;
}

MfwMnuItem *WAPScaleImagesItems(void)
{
  return (MfwMnuItem *) menuScaleImagesItems;
}

/* SPR#1983 - SH */

MfwMnuAttr *WAPNewBookmarkAttributes(void)
{
  return (MfwMnuAttr *) &menuNewBookmark;
}

MfwMnuItem *WAPNewBookmarkItems(void)
{
  return (MfwMnuItem *) menuNewBookmarkItems;
}
/* SPR#2086 - SH */

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)

MfwMnuAttr *WAPPushListOptionsAttributes(void)
{
  return (MfwMnuAttr *) &menuPushListOptions;
}

MfwMnuItem *WAPPushListOptionsItems(void)
{
  return (MfwMnuItem *) menuPushListOptionsItems;
}

MfwMnuAttr *WAPPushMessagesAttributes(void)
{
  return (MfwMnuAttr *) &menuPushMessages;
}

MfwMnuItem *WAPPushMessagesItems(void)
{
  return (MfwMnuItem *) menuPushMessagesItems;
}
#endif /* PUSH */

#endif // WAP

#ifdef MMI_GPRS_ENABLED

MfwMnuAttr *SMSServiceAttributes(void)
{
  return (MfwMnuAttr *) &menuSMSService;
}

MfwMnuItem *SMSServiceItems(void)
{
  return (MfwMnuItem *) menuSMSServiceItems;
}

#endif //GPRS

//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
MfwMnuAttr *WAPPPGAuthenticationAttributes(void)
{
	return (MfwMnuAttr*)&MenuPPGAuthenticationOptions;
}

MfwMnuAttr *WAPWirelessProfiledHTTPAttributes(void)
{
	return (MfwMnuAttr*)&MenuWirelessProfiledHTTPOptions;
}
#endif
/* SPR#1112 - SH - Menus for internal phonebook */
#ifdef INT_PHONEBOOK
MfwMnuAttr *SelectBookAttributes(void)
{
  return (MfwMnuAttr *) &menuSelectBook;
}

MfwMnuItem *SelectBookItems(void)
{
  return (MfwMnuItem *) menuSelectBookItems;
}
#endif

/* SPR#1352 - SH - TTY menu */
#ifdef MMI_TTY_ENABLED
MfwMnuAttr *TTYAttributes(void)
{
  return (MfwMnuAttr *) &menuTTY;
}

MfwMnuItem *TTYItems(void)
{
  return (MfwMnuItem *) menuTTYItems;
}
/*x0039928 OMAPS00097714 HCO/VCO option - menu items added*/
#ifdef FF_TTY_HCO_VCO
MfwMnuAttr *TTYCOAlwaysAttributes(void)
{
  return (MfwMnuAttr *) &menucoTTYAlways;
}

MfwMnuAttr *TTYCONextCallAttributes(void)
{
  return (MfwMnuAttr *) &menucoTTYNextCall;
}
#endif
#endif
#ifdef FF_MMI_FILEMANAGER
MfwMnuAttr *FMMenuOptionAttributes(void)
{
  return (MfwMnuAttr *) &menuFMListOptions;
}

MfwMnuAttr *FMRootFolderAttributes(void)
{
  return (MfwMnuAttr *) &menuRootFolders;
}

MfwMnuAttr *FMDrivesOptionsListAttributes(void)
{
  return (MfwMnuAttr *) &menuFMDrivesOptions;
}
MfwMnuAttr *FMDestDrivesListAttributes(void)
{
  return (MfwMnuAttr *) &menuDestFMDrives;
}
MfwMnuAttr *FMDestRootFolderAttributes(void)
{
  return (MfwMnuAttr *) &menuDestRootFolders;
}
#endif
/* end SH */

/*******************************************************************************

                                Public Routines

*******************************************************************************/

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- Start */

/*******************************************************************************

 $Function:     mmi_check_tflash

 $Description: Routine to check if TFlash exists or not

 $Returns:    	0                       :  Enable TFlash in all Menus; 
                     MNU_ITEM_HIDE :  Hide TFlash in all Menus

 $Arguments:  MfwMnuTag* , MfwMnuAttrTag*, MfwMnuItemTag*

*******************************************************************************/
USHORT mmi_check_tflash( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{

/********************************************************/
/* If the TFlash Feature Flag is enabled, then check if TFlash Card   */
/*                            is inserted or not                                          */
/********************************************************/
#ifdef FF_MMI_TFLASH

	char curDir[10];
	UINT16 curDir_uc[10];
	UINT16 objName_uc[10];
	char objName[10];
	T_RFS_DIR f_dir;
			
	strcpy(curDir, "/");
	convert_u8_to_unicode(curDir, curDir_uc);

	if (rfs_opendir (curDir_uc, &f_dir) < 0)
	{
		TRACE_EVENT_P1("Opening dir %s Failed",curDir);
		return MNU_ITEM_HIDE; 
	}

	while(rfs_readdir (&f_dir, objName_uc, FM_MAX_OBJ_NAME_LENGTH) > 0x0 )
	{
		if (objName_uc[0]  == '\0')
		{
			return MNU_ITEM_HIDE;
		}

		convert_unicode_to_u8(objName_uc, objName);
				
		if(strcmp(objName,"MMC")==0)
		{
			rfs_closedir(&f_dir);
			return 0;
		}
	}    

	rfs_closedir(&f_dir);

	return MNU_ITEM_HIDE;

/********************************************************/
/* If the TFlash Feature Flag is disabled, then hide TFlash Item        */
/********************************************************/

#else

	return MNU_ITEM_HIDE;

#endif	

}


/*******************************************************************************

 $Function:     mmi_check_nand

 $Description: Routine to check if NAND exists or not

 $Returns:    	0                       :  Enable NAND in all Menus; 
                     MNU_ITEM_HIDE :  Hide NAND in all Menus

 $Arguments:  MfwMnuTag* , MfwMnuAttrTag*, MfwMnuItemTag*

*******************************************************************************/
USHORT mmi_check_nand( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{

/********************************************************/
/* If the NAND Feature Flag is enabled, then enable the Menu Item  */
/********************************************************/
#ifdef FF_MMI_NAND

	return 0;

/********************************************************/
/* If the NAND Feature Flag is enabled, then hide the Menu Item     */
/********************************************************/

#else

	return MNU_ITEM_HIDE;

#endif	

}

/*******************************************************************************

 $Function:     mmi_check_nor

 $Description: Routine to check if NOR exists or not

 $Returns:    	0                       :  Enable NOR in all Menus; 
                     MNU_ITEM_HIDE :  Hide NOR in all Menus

 $Arguments:  MfwMnuTag* , MfwMnuAttrTag*, MfwMnuItemTag*

*******************************************************************************/
USHORT mmi_check_nor( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{

/********************************************************/
/* If the NOR Feature Flag is enabled, then enable the Menu Item    */
/********************************************************/
#ifdef FF_MMI_NOR

	return 0;

/********************************************************/
/* If the NOR Feature Flag is enabled, then hide the Menu Item       */
/********************************************************/

#else

	return MNU_ITEM_HIDE;

#endif	

}


/*******************************************************************************

 $Function:     mmi_check_norms

 $Description: Routine to check if NORMS exists or not

 $Returns:    	0                       :  Enable NORMS in all Menus; 
                     MNU_ITEM_HIDE :  Hide NORMS in all Menus

 $Arguments:  MfwMnuTag* , MfwMnuAttrTag*, MfwMnuItemTag*

*******************************************************************************/
USHORT mmi_check_norms( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{

/********************************************************/
/* If the NORMS Feature Flag is enabled, then enable Menu Item     */
/********************************************************/
#ifdef FF_MMI_NORMS

	return 0;

/********************************************************/
/* If the NORMS Feature Flag is enabled, then hide the Menu Item   */
/********************************************************/

#else

	return MNU_ITEM_HIDE;

#endif	

}

/*  Sep 26, 2007	Configuration of Drives for SBuild 2.3.0 Migration x0080701 (Bharat) -- End */

/*******************************************************************************

 $Function:     MmiBookMenuArea

 $Description:  access routine for the local static menu area

 $Returns:    pointer to the menu area

 $Arguments:  none.

*******************************************************************************/

MfwRect MmiBookMenuArea( void )
{
  return menuArea;
}

/*******************************************************************************

 $Function:     MmiBookNamesArea

 $Description:  access routine for the local static names menu area

 $Returns:    pointer to the names menu area

 $Arguments:  none.

*******************************************************************************/

MfwRect MmiBookNamesArea( void )
{
  return menuNamesArea;
}
/*******************************************************************************

 $Function:     create_mmi_phonebook_names_list. Added for SPR2123

 $Description:  access routine for the phonebook names list.  Allocates memory for the
 list if memory has not already been allocated.

 $Returns:    pointer to the phonebook name list

 $Arguments:  none.

*******************************************************************************/
MfwMnuAttr* create_mmi_phonebook_names_list()
{	int i;

	TRACE_EVENT("create_mmi_phonebook_names_list()");
	if (phonebook_list != NULL)
		return phonebook_list;

/*allocate memory for list of phonenumbers*/
	phonebook_list = (MfwMnuAttr*)ALLOC_MEMORY(sizeof(MfwMnuAttr));
	if (phonebook_list == NULL)
	{	TRACE_EVENT("Phonebook list alloc failed");
		return NULL;
	}

/*allocate memory for the array of items in the phonebook list*/
	items_in_phonebook_list = (MfwMnuItem*)ALLOC_MEMORY(sizeof(MfwMnuItem)*NAME_LIST_SIZE);
	if (items_in_phonebook_list == NULL)
	{	TRACE_EVENT("Phonebook list items alloc failed");
		return NULL;
	}

/*allocate memory for the array of pointers to strings holding phonebook list text*/
	menu_item_strings = (char**)ALLOC_MEMORY(sizeof(char*)*NAME_LIST_SIZE);
	if (menu_item_strings == NULL)
	{	TRACE_EVENT("menu item string list alloc failed");
		return NULL;
	}

/*allocate memory for each text string in turn*/
	for (i=0;i<NAME_LIST_SIZE;i++)
	{	menu_item_strings[i] = (char*)ALLOC_MEMORY(sizeof(char)*PHONEBOOK_ITEM_LENGTH);
		if (menu_item_strings[i] == NULL)
		{	TRACE_EVENT(" menu item string alloc failed");
			return NULL;
		}

	}

/*specify phonebook list menu configuration*/
	phonebook_list->area = &menuNamesArea;
	phonebook_list->mode = MNU_LEFT | MNU_LIST | MNU_CUR_LINE;
	phonebook_list->font = (U8)-1;
	phonebook_list->items = items_in_phonebook_list;
	phonebook_list->nItems =   NAME_LIST_SIZE;
   	phonebook_list->mnuColour = COLOUR_LIST_SUBMENU;
   	phonebook_list->hdrId = TxtNull;
   	phonebook_list->bgdBitmap= NULL;
   	phonebook_list->unused = MNUATTRSPARE;

/*specify configuration of each meni item*/
	for(i=0; i<NAME_LIST_SIZE; i++)
	{
		items_in_phonebook_list[i].icon =0;
		items_in_phonebook_list[i].exta =0;
		items_in_phonebook_list[i].x =0;
		items_in_phonebook_list[i].str =menu_item_strings[i];
		items_in_phonebook_list[i].menu =0;
		items_in_phonebook_list[i].func =0;
		items_in_phonebook_list[i].flagFunc =item_flag_none;
	}

 	return phonebook_list;
}

/*******************************************************************************

 $Function:     destroy_mmi_phonebook_names_list. Added for SPR2123

 $Description:  frees memory for phonebook names list

 $Returns:    TRUE if sucessful, FALSE if failed

 $Arguments:  pointer to phonebook names list

*******************************************************************************/
BOOL  destroy_mmi_phonebook_names_list(MfwMnuAttr* pointer)
{	int i;


	TRACE_EVENT("destroy_mmi_phonebook_names_list()");
	/*if memory not allocated for phonebook list OR the pointer passed to the function is
	NOT that of the phonebook list, stop here*/
	if ((phonebook_list == NULL) || (pointer != phonebook_list))
		return FALSE;

	/*dealloacte memory for each item's text string*/
	for(i=0; i<NAME_LIST_SIZE; i++)
	{	FREE_MEMORY((U8*)menu_item_strings[i], sizeof(char)*PHONEBOOK_ITEM_LENGTH);
			menu_item_strings[i] = NULL;

	}
	/*deallocate memory for the array of pointer to text strings*/
	FREE_MEMORY((U8*)menu_item_strings, sizeof(char*)*NAME_LIST_SIZE);

	/*deallocate memory for the array of items in the phonebook list*/
	FREE_MEMORY((U8*)items_in_phonebook_list, sizeof(MfwMnuItem)*NAME_LIST_SIZE);
	items_in_phonebook_list = NULL;

	/*deallocate the phonebook list menu attributes*/
	FREE_MEMORY((U8*)phonebook_list, sizeof(MfwMnuAttr));
	phonebook_list = NULL;

	return TRUE;
}
/*******************************************************************************

 $Function:     MmiBookCallList

 $Description:  access routine for the local static call list buffer

 $Returns:    pointer to the call list buffer

 $Arguments:  index, of the element required

*******************************************************************************/

void *MmiBookCallList( int index )
{
  /* Make sure that the index is reasonable before using it
  */
  if ( index < 0 )
    index = 0;

  if ( index >= NAME_LIST_SIZE )
    index = NAME_LIST_SIZE - 1;

  /* return address of requested element in array
  */
  /*SPR 2123, if memory hasn't been allocated for menu items strings return NULL*/
  if (menu_item_strings != NULL)
  	return menu_item_strings[index];
  else
  	return NULL;
}

/*******************************************************************************

 $Function:     MmiBookCallListSize

 $Description:  access routine for the size of the local static call list buffer

 $Returns:    PHONEBOOK_ITEM_LENGTH

 $Arguments:  index, provided for compatibility with call list access routine

*******************************************************************************/

int MmiBookCallListSize( int index )
{/*SPR2123, MC changed MAX_SCREEN_LENGTH to PHONEBOOK_ITEM_LENGTH*/
  return PHONEBOOK_ITEM_LENGTH;
}


/*******************************************************************************

 $Function:     MmiBookMenuDetailsList

 $Description:  access routine for the menu details list attribute structure

 $Returns:    address of attribute structure

 $Arguments:  none.

*******************************************************************************/

MfwMnuAttr *MmiBookMenuDetailsList( void )
{
	/*SPR2123, return the pointer to the phonebook list attributes, it'll be NULL
	if memory has not been alloacted to it*/
	return phonebook_list;



}


/*******************************************************************************

 $Function:     MmiBookShowDefault

 $Description:  Default screen update routine, this will be called prior to
        any other screen painting in the phone book. For now we just
        clear the display as a default action.

 $Returns:    None.

 $Arguments:  None.

*******************************************************************************/

void MmiBookShowDefault( void )
{
    dspl_ClearAll();
}





/*******************************************************************************

 $Function:     MmiBookCurrentText

 $Description:  Returns a handle to the current global text message.

 $Returns:    Returns a handle to the current global text message.

 $Arguments:  None.

*******************************************************************************/

int MmiBookCurrentText( void )
{
    return textMessage;
}

void MmiBookSetCurrentText( int pstr )
{
  textMessage = pstr;
}

/*******************************************************************************

 $Function:		storeMenuOption

 $Description:	Stores the ID of the text string for the currently selected menu item

 $Returns:		0

 $Arguments:	mnu - unused
 				numItem - selected menu item

GW 28/11/02 Added a general purpose storage/retrieval procedure
*******************************************************************************/

static int storedMenuId = TxtNull;
int storeMenuOption(MfwMnu* mnu, MfwMnuItem* mnuItem)
{
	T_MFW_HND win = mfw_parent(mfw_header());

	storedMenuId = (int)mnuItem->str;
	return(0);
}

/*******************************************************************************

 $Function:		MmiBook_getStoredOption

 $Description:		Returns the ID of the text string stored on a menu selection

 $Returns:		storedMenuId - string Id of the selected menu item

 $Arguments:	none.

GW 28/11/02 Added a general purpose storage/retrieval procedure
*******************************************************************************/
int MmiBook_getStoredOption( void)
{
	return (storedMenuId);

}
//SPR12822 - vvadiraj@sasken.com
MfwMnuAttr *NWIconAttributes(void)
{
//added a compiler switch. - xvilliva
#ifdef COLOURDISPLAY
  return (MfwMnuAttr *) &menuProviderIconSet;  
#endif

}
//  Jul 28, 2004 REF: CRR 20899 xkundadu
//  Returns the 'Any Key Answer' menu to display.
/*******************************************************************************
 $Function:		GetAnyKeyAnswerMenu

 $Description:		 Returns the 'Any Key Answer' menu

 $Returns:		menuAnyKeySet - Menu of the 'AnyKeyAnser'.

 $Arguments:	none.

*******************************************************************************/
MfwMnuAttr *GetAnyKeyAnswerMenu()
{
	return (MfwMnuAttr *) &menuAnyKeySet;  
}
//  Jul 29, 2004 REF: CRR 20898 xkundadu
//  Returns the 'AutAnswer' menu to display.
/*******************************************************************************
 $Function:		GetAutoAnswerMenu

 $Description:		 Returns the 'AutoAnswer' menu

 $Returns:		menuAutoAnswerSet - Menu of the 'AutoKeyAnswer'.

 $Arguments:	none.

*******************************************************************************/
MfwMnuAttr *GetAutoAnswerMenu()
{
 	return (MfwMnuAttr *) &menuAutoAnswerSet;  
}


// Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
// Returning the submenu for FDN submenu
MfwMnuAttr *FDNMenu(void)
{
  return (MfwMnuAttr *) &menuFixedDialSet;  
}

// Jul 22,2004 CRR:20896 xrashmic - SASKEN
// Returning the submenu for time format.
MfwMnuAttr *TimeFormatAttributes(void)
{
  return (MfwMnuAttr *) &menuTimeFormat;  
}
// Sep 27,2005 REF: SPR 34402 xdeepadh  
// Returning the submenu for channels
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
MfwMnuAttr *PlayerChannelAttributes(void)
{
  return (MfwMnuAttr *) &menuPlayerChannels;  
}
#endif //endif FF_MMI_TEST_MP3

// May 04, 2005       	REF: CRR:30285 x0021334
// Returning the submenu for SMS Status Report submenu
MfwMnuAttr *SmsStatusReportAttributes(void)
{
  return (MfwMnuAttr *) &menuSetSmsStatusReportRequest;  
}

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
/* Gets the power management options*/
MfwMnuAttr *GetPowerManagementMenu(void)
{
  return (MfwMnuAttr *) &menuPowerManagement;
}
#endif
#endif
#ifdef FF_MMI_MULTIMEDIA
/* PCM decode channel mode menu */
MfwMnuAttr *GetPcmDecodeChannelModeMenu(void)
{
  return (MfwMnuAttr *) &menuPcmDecodeChanMode;
}

/*Voice Memo PCM decode channel mode menu */
MfwMnuAttr *GetVmPcmDecodeChannelModeMenu(void)
{
  return (MfwMnuAttr *) &menuVmPcmDecodeChanMode;
}

/* Configuration Parameter Menu item obtained from these functions. */
MfwMnuAttr *GetScreenModeMenu(void)
{
  return (MfwMnuAttr *) &menuSelectOutputScreemode;
}

MfwMnuAttr *GetAudioFormatMenu(void)
{
  return (MfwMnuAttr *) &menuSelectAudioEncodingFormat;
}

MfwMnuAttr *GetVideoFormatMenu(void)
{
  return (MfwMnuAttr *) &menuSelectVideoEncodingFormat;
}

MfwMnuAttr *GetVideoSizeMenu(void)
{
  return (MfwMnuAttr *) &menuSelectVideoCaptureSize;
}

MfwMnuAttr *GetFrameRateMenu(void)
{
  return (MfwMnuAttr *) &menuSelectFrameRate;
}

MfwMnuAttr *GetVideoBitRateMenu(void)
{
  return (MfwMnuAttr *) &menuSelectVideoRate;
}

MfwMnuAttr *GetAACBitRateMenu(void)
{
  return (MfwMnuAttr *) &menuSelectAACBitRate;
}

MfwMnuAttr *GetAMRBitRateMenu(void)
{
  return (MfwMnuAttr *) &menuSelectAMRBitRate;
}

MfwMnuAttr *GetAudioPathMenu(void)
{
  return (MfwMnuAttr *) &menuAudioPath;
}

MfwMnuAttr *GetVideoRecordDurationMenu(void)
{
  return (MfwMnuAttr *) &menuVideoRecordDuration;
}

MfwMnuAttr *GetVolumeConfigMenu(void)
{
  return (MfwMnuAttr *) &menuVolumeConfig;
}

MfwMnuAttr *GetPreferredStorageMenu(void)
{
  return (MfwMnuAttr *) &menuPreferredStorage;
}

/* Image Capture Quality Menu */
MfwMnuAttr *GetImageCaptureQualityMenu(void)
{
  return (MfwMnuAttr *) &menuImageCaptureQuality;
}

/* PCM Channel Mode Menu */
MfwMnuAttr *GetPcmChannelModeMenu(void)
{
  return (MfwMnuAttr *) &menuPcmChannelMode;
}


/* PCM Rate Selection Menu */
MfwMnuAttr *GetPcmRateSelectionMenu(void)
{
  return (MfwMnuAttr *) &menuPcmRateSelection;
}


/* PCM Decode Sample Rate */
MfwMnuAttr *GetPcmSampleRateMenu(void)
{
  return (MfwMnuAttr *) &menuPcmSampleRate;
}


/* Voice Memo PCM Decode Sample Rate */
MfwMnuAttr *GetVmPcmSampleRateMenu(void)
{
  return (MfwMnuAttr *) &menuVmPcmSampleRate;
}


/* Audio & Video Play option */
MfwMnuAttr *GetAudioVideoSyncOptionMenu(void)
{
  return (MfwMnuAttr *) &menuAudioVideoSyncOption;
}

/* Audio & Video Record option */
MfwMnuAttr *GetAudioVideoRecordOptionMenu(void)
{
  return (MfwMnuAttr *) &menuAudioVideoRecordOption;
}
MfwMnuAttr *GetVoiceMemoMenu(void)
{
#ifdef FF_MMI_VOICEMEMO
     return (MfwMnuAttr *) &menuVoiceMemoUC;
#else
     return (MfwMnuAttr *) &menuVoiceMemo;
#endif
}


#ifdef FF_MMI_NEPTUNE_IMAGE_VIEWER

MfwMnuAttr *GetmenuImageViewerOptions(void)
{
  return (MfwMnuAttr *) &menuMMImageStorageDevice;
}


MfwMnuAttr *GetmenuImgEditOptions(void)
{
  return (MfwMnuAttr *) &menuImageEdit;
}
#endif
#endif
//Sep 11, 2006 DR: OMAPS00094182 xrashmic
#ifdef FF_MMI_TEST_MIDI 
#if 0
MfwMnuAttr *MidiTestAppMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuMidiTest;
}
#endif
#endif
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
MfwMnuAttr *AudioPlayerAppMenuAttributes(void)
{
  return (MfwMnuAttr *) &menuPlayerTest;
}
#endif

/*******************************************************************************

                                End of File

*******************************************************************************/

