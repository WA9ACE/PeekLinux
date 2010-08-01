#ifndef _MMIBOOKSHARED_
#define _MMIBOOKSHARED_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookShared.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This module provides definitions of the types and
    constants which are shared across the phone book
    application modules.
   
********************************************************************************
 $History: MmiBookShared.h
      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      
 	Sep 11, 2006 DR: OMAPS00094182 xrashmic
 	Description: midi and mp3 files is not being populated on accessing corresponding menu, but only during bootup
 	Solution: Populate files on accessing the "browse files" list
	xrashmic 9 Sep 2006, OMAPS00092732
	USBMS ER: Added menu in phone setting to set or unset PS shutdown while enumeration of USB MS
	On USB connect event, a list of possible ports that can be enumerated would be displayed.	

	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security 
	Solution:Made the following changes
			Added prototype for function SIMPCategoryListAttributes

	Apr 24, 2006    ERT: OMAPS00067603 x0043642
   	Description: Engineering mode
   	Solution:  Added new engineering mode function prototypes.

	Jan 22, 2006 REF: OMAPS00061930  x0039928  
	Bug:PhoneBook -Change entry and view entry through Change option-failed
 	Fix: Phonebook is populated with phone numbers after some time and not immediately while saving the changed entries.

 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
	Description: Implementation of Test Menu for AAC
	Solution: The existing MP3 test Application has been enhanced to support the AAC file testing.
	Sep 27,2005 REF: SPR 34402 xdeepadh  
	Bug:Mono option is always highlighted even when Stereo is selected
	Fix:The channel setting will be saved in the FFS, which will be retrieved 
	later and the selected channel will be higlighted.

 	Aug 22, 2005    REF: ENH 31154 xdeepadh
   	Description: Application to Test Camera
   	Solution: Implemeted the Fileviewer to view the jpeg images.Camera Application to 
   	preview,capture and save image has been implemented.
   	
    May 04, 2005  REF: CRR 30285  x0021334
    Description: SMS Status Request in idle and dedicated mode (using Menu) setting does not work
    Solution: status of sms status request is read and focus is set accrodingly.

    Nov 29, 2004    REF: CRR 25051 xkundadu
    Description: INCALL SCREEN ?ADJUSTING THE VOLUME
    Fix: Added volume level list linked to up/down keys. 
          User can select the speaker volume among those levels.
 
    CRR: 25302 - xpradipg 10 Nov 2004
    Description: Should be able to select the number fro the phonebook while
    sending MMS/EMS.
    Solution: The new feature to select the number from phonebook while sending
    MMS/EMS is added
  

 	Jul 29, 2004 REF: CRR 20898 xkundadu
	Description: No identification for user whether 'Auto-Answer' is On/Off
	Solution: Added the  function GetAutoAnswerMenu() prototype.
			 


	Jul 22,2004 CRR:20896 xrashmic - SASKEN
	Description: The current time format is not highlighted
	Fix: Instead of providing the submenu directly we call a function 
	where we set the menu and also higlight the previously selected menu item

	May 27, 2004    REF: CRR 19186  Deepa M.D 
	Bug	:	"Setting" menu not available during a call
    	Fix	:	"Setting" menu has been provided to set the SMS settings, during the active call
	   
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/



/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

/* The following include files provide the basic type definitions
   required by the phone book application. In order to minimise
   the maintenance of these includes, we will define them all
   here and include this file in each of the public headers. In
   this way any changes are localised to a single module.
*/
#include "prim.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_sim.h"
#include "mfw_sat.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH -New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_phb.h"
#include "phb.h"
#include "mfw_sys.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_nm.h"
#include "mfw_str.h"
#include "mfw_sms.h"
#include "mfw_ss.h"

#include "dspl.h"

/* SPR#1428 - SH -New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUITextEntry.h"
#include "AUIEditor.h"
#endif

#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiSoftKeys.h"
#include "MmiMain.h"
#include "MmiMenu.h"
#include "MmiPins.h"
#include "MmiCall.h"
#include "MmiIdle.h"



#include "MmiBlkLangDB.h"
#include "MmiResources.h"


#ifndef MFW_EVENT_PASSED
#define MFW_EVENT_PASSED 0
#endif



/*******************************************************************************
                                                                              
                                Shared Definitions
                                                                              
*******************************************************************************/

/* Define standard error reports from the phone book
*/
#define BOOK_FAILURE    (-1)
#define BOOK_SUCCESS    (0)
#define BOOK_WINDOW_KEY (0x00BABE04L)



/* Standard definitions which are shared across the phonebook
   modules
*/
#define MAX_CALL_LISTS			15
/*SPR 1455, */
#ifndef LSCREEN
#define NAME_SCREEN_MAX			13
#else
#define NAME_SCREEN_MAX			41
#endif 

/*SPR 1456*/
#ifdef LSCREEN
#define NAME_LIST_SIZE			10
#define MAX_SEARCHED			10
//
#define MAX_SEARCH_NAME			10
#define MAX_SEARCH_CALL_LIST	10
#else
#define NAME_LIST_SIZE			3
#define MAX_SEARCHED			3

#define MAX_SEARCH_NAME			3
#define MAX_SEARCH_CALL_LIST	3
#endif
//#define EDITOR_SIZE				32
//#define EDITOR_SIZE				96 defined in mmimain.h
/*SPR 2102, should be long enough to cope with displaying a name and number with time and date in unicode*/
#define STANDARD_EDITOR_SIZE	(PHB_MAX_LEN*2+50)

//GW-SPR#762- 40 digit phonebook size.
#ifdef TI_PS_FFS_PHB
#define NUMBER_LENGTH			41
#else
#ifdef PHONEBOOK_EXTENSION
#define NUMBER_LENGTH			41
#else
#define NUMBER_LENGTH			20
#endif /* else, #ifdef PHONEBOOK_EXTENSION */
#endif /* else, #ifdef TI_PS_FFS_PHB */
#define MAX_SEARCH_CHAR			8

#define PHONEBOOK_ITEM_LENGTH 41 /*SPR2123, length of phonebook entry*/
//#define PHB_UPN					MAX_PHB_TYPE+1
//This definition conflicts with the MFW definition



/* Define a default menu structure
*/




/*******************************************************************************
                                                                              
                        Shared Structure Definitions
                                                                              
*******************************************************************************/


/* Define the type of searching we can perform, these can be
   on name, number or SIM location
*/
typedef enum {
    SEARCH_BY_NAME, 
    SEARCH_BY_NUMBER,
    SEARCH_BY_LOCATION
} tSearchTypes;

typedef enum {
    CREATE_ENTRY,
    MODIFY_EXISTING,
    ADD_FROM_IDLE
} tUpdateTypes;



typedef struct
{
    T_MFW_PHB_ENTRY     entry[MAX_SEARCHED];
    T_MFW_PHB_LIST      list;
    T_MFW_PHB_STATUS    status;
    UBYTE               result;
	UBYTE               selectedName;
    SHORT               index;
    UBYTE				missedCallsOffset;
	int                 mode;
    tSearchTypes        KindOfSearch;
} tMmiPhbData, *pMmiPhbData;



/* Define the basic control record for the phonebook itself,
   this structure will be used by all phonebook application
   modules.
*/
typedef struct T_phbk_data
{
    T_MMI_CONTROL		mmi_control;
    T_MFW_HND			win;

    /* internal data
    */
    T_MFW_HND			search_win;
	T_MFW_HND			root_win;
    T_MFW_HND			menu_main_win;
	T_MFW_HND			calls_list_win;
	T_MFW_HND			menu_options_win;
	T_MFW_HND			menu_options_win_2;
	T_MFW_HND		    menu_call_options_win;
	T_MFW_HND			menu_call_options_win_2;
	T_MFW_HND			name_details_win;
	T_MFW_HND		    input_number_win;
	T_MFW_HND			input_name_win;
	T_MFW_HND			parent_win;

	char				fromSMS;
	char				fromSMSSC;
	char				fromDivert;
    //CRR: 25302 - xpradipg 10 Nov 2004
    //Is set to True if invoked from MMS/EMS
       char                      fromMmsEms;
	char				fromDeflect;/*SPR 1392, call deflection*/
	tMmiPhbData			current;
    T_MFW_PHB_ENTRY		newEntry;
    T_MFW_UPN_LIST		*upnList;
	struct T_phbk_data	*phbk;
	UBYTE				edt_buf_name[MAX_ALPHA_LEN];
	/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
	char				edt_buf_number[PHB_MAX_LEN];
	/* SPR#1428 - SH - Not required for new editor */
#ifndef NEW_EDITOR
	MfwEdtAttr			edt_attr_name;
    MfwEdtAttr			edt_attr_number;
#endif /* NEW_EDITOR */
	T_VOID_FUNC			pin2_next;
	struct MfwMnuTag 			*menu;		//Added for FDN -- MC
	struct MfwMnuItemTag		*item;		//Added for FDN -- MC
    tUpdateTypes        UpdateAction;

} T_phbk;


/* SPR#1428 - SH - Not required for new editor */
#ifndef NEW_EDITOR
/* Each window uses a standard message block containing references
   to the keyboards, editors, menues etc.
*/
typedef struct _tInputSpecifics_
{
    int 		text;
    MfwEdtAttr	*edt_attr_input;
    int         left_soft_key;
    int         right_soft_key;
	short		abc;
    void		(* callback)( T_MFW_HND win,UBYTE reason );
} tInputSpecifics, *pInputSpecifics;
#endif /* NEW_EDITOR */

typedef struct _tBookStandard_
{
	/* Standard elements which are consistent across all phonebook
	   window control blocks
	*/
    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
    T_MFW_HND       menu_tim;
    T_MFW_HND       menu;
    T_MFW_HND	  tim;    //Jan 22, 2006 REF: OMAPS00061930  x0039928, Add a timer handle
#ifdef NEW_EDITOR
	T_ED_DATA		*editor;
	T_AUI_ENTRY_DATA *entry_data;
#else /* NEW_EDITOR */
	T_MFW_HND       edt;
#endif /* NEW_EDITOR */
    T_MFW_HND       parent_win;
    T_phbk			*phbk;
    T_VOID_FUNC     Callback;

    /* internal data
    */
    UBYTE           status_of_timer; /* use for the keypadlock */
#ifdef NEW_EDITOR	/* SPR#1428 - SH - New Editor changes */
	T_ED_ATTR		editor_attr;
#else /* NEW_EDITOR */
    MfwEdtAttr      attr;
#endif /* NEW_EDITOR */
    char            edtBuf[STANDARD_EDITOR_SIZE*sizeof(USHORT)];
#ifndef NEW_EDITOR	/* SPR#1428 - SH - New Editor changes */
	tInputSpecifics properties;	/* No longer required with NEW_EDITOR */
#endif /* NEW_EDITOR */
} tBookStandard, *pBookStandard;



/* In order to encapsulate the behaviour of the phonebook in
   it's own terms of reference, we will provide the following
   type definition overrides.
*/
#define tBookStatus			int
#define tBookPtr			void *
#define tBookDialogData		T_DISPLAY_DATA
#define tBookMfwMenu		struct MfwMnuTag
#define tBookMfwMenuItem	struct MfwMnuItemTag
#define tBookMfwHnd			MfwHnd
#define tBookMfwHdr			MfwHdr
#define tBookMfwWin         MfwWin
#define tBookMfwEvent       MfwEvt
#define tBookMfwBook        int
#define tBookPhonebook		T_phbk

#define pBookPhonebook      tBookPhonebook *
#define pBookStandard       tBookStandard *
#define pBookMfwWin         tBookMfwWin *
#define THREE_SECS          0x0BB8             /* 3000 milliseconds.                  */


/* The following definitions are inherited from the Phbk.h header
   and will be adjusted as required to provide the phone book
   functionality.
*/
typedef enum
{
    PhbkNone=PhbkDummy,
    PhbkNormal,
    PhbkMainMenu,
    PhbkDone,
    PhbkSearchNormal,
    PhbkDetailsListDone,
    PhbkNameEnterNormal,
    PhbkNameEnterIdle,
    PhbkNumberEnterNormal,
    PhbkMissedCallsListNormal,
    PhbkReplyListNormal,
    PhbkRepRedOptionsNormal,
    PhbkRedialListNormal,
	PhbkDetailListToIdle,
	PhbkFromSms,
	PhbkFromSmsSC,	/*SH*/
	PhbkFromDivert,
	PhbkSendAsDTMF,
	//CRR: 25302 - xpradipg 10 Nov 2004
	PhbkFromMms,
	PhbkFromDeflect/*SPR 1392, call deflection*/
} tBookActionIdentifiers;


typedef enum
{
	INPUT_INIT,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_CLEAR,
	INPUT_DESTROY,
	MENU_INIT,
	ADD_CALLBACK,
	DEFAULT_OPTION,
	SEARCH_INIT,
	SEARCH_SCROLL_UP,
	SEARCH_SCROLL_DOWN,
	SEARCH_STRING,
	SEARCH_UPDATE,
	//CRR: 25302 - xpradipg 10 Nov 2004
	MMS_EMS_PHBK_NUMBER,
#ifdef NEW_EDITOR	/* SPR#1428 - SH - New Editor: So we can redraw screen when required */
	SEARCH_REDRAW,
#endif /* NEW_EDITOR */
	CALL_DETAILS_INIT,
	CALLS_LIST_INIT,
	PHBK_INIT,
	PHBK_SEND_NUMBER,
	SMS_PHBK_NUMBER,
	SMSSC_PHBK_NUMBER, /* SH - Service centre number */
	DIVERT_PHBK_NUMBER,  //MZ 26/02/01 definition for set divert.
	DEFLECT_PHBK_NUMBER	/*SPR 1392, call deflection*/
} tBookMessageEvents;




/*******************************************************************************
                                                                              
                        Define common interface definitions
                                                                              
*******************************************************************************/

/* Define shared macros for the event and dialog handlers. This macro
   definition ensures that all handlers are given the same interface.
*/
#define EVENT_HANDLER( Name ) \
static int Name( tBookMfwEvent Event, tBookMfwWin *Window )

#define DIALOG_FUNCTION( Name ) \
static void Name( tBookMfwHnd win, USHORT event, SHORT value, void *parameter )




/*******************************************************************************
                                                                              
                                Function Prototypes
                                                                              
*******************************************************************************/


//Nov 29, 2004    REF: CRR 25051 xkundadu
//Description: INCALL SCREEN ?ADJUSTING THE VOLUME
//Fix: Added these prototype for the menu display function.
MfwMnuAttr *ringerVolumeLevels( void ); 

//  Jul 29, 2004 REF:  CRR 20898 xkundadu
//  Added this function to return the 'AnyKeyAnswer' menu to display.
MfwMnuAttr *GetAutoAnswerMenu(void); // RAVI

#ifdef FF_MMI_MULTIMEDIA
/* Function Prototype for getting the Configuration Parameter Menu. */
MfwMnuAttr *GetScreenModeMenu(void);
MfwMnuAttr *GetAudioFormatMenu(void);
MfwMnuAttr *GetVideoFormatMenu(void);
MfwMnuAttr *GetVideoSizeMenu(void);
MfwMnuAttr *GetFrameRateMenu(void);
MfwMnuAttr *GetVideoBitRateMenu(void);
MfwMnuAttr *GetVideoRecordDurationMenu(void);
MfwMnuAttr *GetAACBitRateMenu(void);
MfwMnuAttr *GetAMRBitRateMenu(void);
MfwMnuAttr *GetAudioPathMenu(void);
MfwMnuAttr *GetVolumeConfigMenu(void);
MfwMnuAttr *GetPreferredStorageMenu(void);
MfwMnuAttr *GetImageCaptureQualityMenu(void);
MfwMnuAttr *GetPcmChannelModeMenu(void);
MfwMnuAttr *GetPcmRateSelectionMenu(void);
MfwMnuAttr *GetPcmSampleRateMenu(void); /* PCM Decode Sample Rate */
MfwMnuAttr *GetVmPcmSampleRateMenu(void); /* Voice Memo PCM Decode Sample Rate */
MfwMnuAttr *GetPcmDecodeChannelModeMenu(void);
MfwMnuAttr *GetVmPcmDecodeChannelModeMenu(void);
MfwMnuAttr *GetVoiceMemoMenu(void);
MfwMnuAttr *GetAudioVideoSyncOptionMenu(void);
MfwMnuAttr *GetAudioVideoRecordOptionMenu(void);  /* Video Only Option - x0043641 */
#ifdef FF_MMI_NEPTUNE_IMAGE_VIEWER
MfwMnuAttr *GetmenuImageViewerOptions(void);
MfwMnuAttr *GetmenuImgEditOptions(void);
#endif
#endif

#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST
/* Gets the power management options*/
MfwMnuAttr *GetPowerManagementMenu(void);
#endif
#endif
/* We have a pair of menu area definitions which must be used in
   the phone book, these are defined once in this module and then
   shared between all of the phone book modules.
*/
MfwRect MmiBookMenuArea( void );
MfwRect MmiBookNamesArea( void );

/* Similarly for the shared call list buffer
*/
void *MmiBookCallList( int index );
int MmiBookCallListSize( int index );

/* and menu attributes
*/
MfwMnuAttr *MmiBookMenuDetailsList( void );


/* Helper functions
*/
void MmiBookShowDefault( void );

/* Text message handling functions
*/
int MmiBookCurrentText( void );
void MmiBookSetCurrentText( int pstr );


/* Menu structure access routines
*/
#ifdef SIM_PERS
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
MfwMnuAttr *SIMPCategoryListAttributes( void );
#endif
#if defined (TI_PS_FF_EM) || defined (MMI_EM_ENABLED)
MfwMnuAttr *EngModeAttributes( void );/*MC, SPR1209 engineering mode menu*/
#endif
MfwMnuAttr *bookOwnNumberMenuAttributes( void );
MfwMnuItem *bookOwnNumberMenuItems( void );
MfwMnuAttr *bookPhonebookMenuAttributes( void );
MfwMnuItem *bookPhonebookMenuItems( void );
MfwMnuAttr *bookRepRedOptionsMenuAttributes( void );
MfwMnuItem *bookRepRedOptionsMenuItems( void );
MfwMnuAttr *bookNameMenuAttributes( void );
MfwMnuAttr *bookNameMenuFDNAttributes( void );// MMI SPR 18555
MfwMnuItem *bookNameMenuItems( void );
MfwMnuAttr *bookUPNMenuAttributes( void );/*MC SPR 1327*/
MfwMnuAttr *bookMainMenuAttributes( void );
MfwMnuAttr *bookCallBarringMenuAttributes( void );
MfwMnuItem *bookMainMenuItems( void );
MfwMnuAttr *calcOptionMenuAttributes( void );
MfwMnuAttr *settingMenuAttributes(void);//May 27, 2004    REF: CRR 19186  Deepa M.D 
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_TEST_CAMERA
MfwMnuAttr * qualityMenuAttributes(void);
MfwMnuAttr * colorMenuAttributes(void);
#endif
#ifdef FF_MMI_CAMERA_APP
MfwMnuAttr *CameraMenuAttributes(void);
#endif
//xrashmic 9 Sep 2006, OMAPS00092732
#ifdef FF_MMI_USBMS
MfwMnuAttr *USBMenuAttributes(void);
#endif
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
MfwMnuAttr *rotateMenuAttributes(void);
MfwMnuAttr *zoomMenuAttributes(void);
#endif
MfwMnuAttr *applicationsMenuAttributes(void);//August 24, 2004    e-armanetsaid
MfwMnuItem *calcOptionMenuItems( void );
MfwMnuAttr *MelgenOptionMenuAttributes( void );
MfwMnuItem *MelgenOptionMenuItems( void );
MfwMnuAttr *ringerVolumeSetting( void );
MfwMnuAttr * create_mmi_phonebook_names_list(void);/*SPR2123*/ // RAVI
BOOL  destroy_mmi_phonebook_names_list();/*SPR2123*/
#ifdef FF_WAP
MfwMnuAttr *WAPMenuAttributes(void);
MfwMnuItem *WAPMenuItems(void);
MfwMnuAttr *WAPBookOptMenuAttributes(void);
MfwMnuItem *WAPBookOptMenuItems(void);
MfwMnuAttr *WAPHistOptMenuAttributes(void);
MfwMnuItem *WAPHistOptMenuItems(void);
MfwMnuAttr *WAPProfOptionsAttributes(void);
MfwMnuItem *WAPProfOptionsItems(void);
MfwMnuAttr *WAPGoToAttributes(void);
MfwMnuItem *WAPGoToItems(void);
//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
MfwMnuAttr *WAPPPGAuthenticationAttributes(void);
MfwMnuAttr *WAPWirelessProfiledHTTPAttributes(void);
#endif
MfwMnuAttr *WAPConnectionTypeAttributes(void);
MfwMnuItem *WAPConnectionTypeItems(void);
MfwMnuAttr *WAPConnectionSpeedAttributes(void); /* SPR#1983 - SH */
MfwMnuItem *WAPConnectionSpeedItems(void); /* SPR#1983 - SH */
/* SPR#2324 - SH - "access type" menu is now GPRS/Dialup menu */
MfwMnuAttr *WAPGPRSDialupAttributes(void); /* SPR#2324 */
MfwMnuItem *WAPGPRSDialupItems(void); /* SPR#2324 */
MfwMnuAttr *WAPGPRSProfileAttributes(void); /* SPR#2324 */
MfwMnuItem *WAPGPRSProfileItems(void); /* SPR#2324 */
MfwMnuAttr *WAPDialupProfileAttributes(void); /* SPR#2324 */
MfwMnuItem *WAPDialupProfileItems(void); /* SPR#2324 */
MfwMnuAttr *WAPAdvancedProfileAttributes(void); /* SPR#2324 */
MfwMnuItem *WAPAdvancedProfileItems(void); /* SPR#2324 */
MfwMnuAttr *WAPSecurityOptionAttributes(void);
MfwMnuItem *WAPSecurityOptionItems(void);
MfwMnuAttr *WAPSaveHistoryAttributes(void);
MfwMnuItem *WAPSaveHistoryItems(void);
MfwMnuAttr *WAPScaleImagesAttributes(void);
MfwMnuItem *WAPScaleImagesItems(void);
MfwMnuAttr *WAPNewBookmarkAttributes(void); /* SPR#1983 - SH */
MfwMnuItem *WAPNewBookmarkItems(void); /* SPR#1983 - SH */
#endif // WAP

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
MfwMnuAttr *WAPPushListOptionsAttributes(void); /* SPR#2086 - SH */
MfwMnuItem *WAPPushListOptionsItems(void); /* SPR#2086 - SH */
MfwMnuAttr *WAPPushMessagesAttributes(void);
MfwMnuItem *WAPPushMessagesItems(void);
#endif /* PUSH */


#ifdef MMI_GPRS_ENABLED
MfwMnuAttr *SMSServiceAttributes(void);
MfwMnuItem *SMSServiceItems(void);
#endif // GPRS

/* SPR#1112 - Internal phonebook menus */
#ifdef INT_PHONEBOOK
MfwMnuAttr *SelectBookAttributes(void);
MfwMnuItem *SelectBookItems(void);
#endif //INT_PHONEBOOK

/* SPR#1352 - SH - TTY */
#ifdef MMI_TTY_ENABLED
MfwMnuAttr *TTYAttributes(void);
MfwMnuItem *TTYItems(void);
/*x0039928 OMAPS00097714 HCO/VCO option - menu items added*/
#ifdef FF_TTY_HCO_VCO
MfwMnuAttr *TTYCOAlwaysAttributes(void);
MfwMnuAttr *TTYCONextCallAttributes(void);
#endif
#endif
/* end SH */

int MmiBook_getStoredOption( void);
//SPR12822
MfwMnuAttr *NWIconAttributes(void);

// Jul 22,2004 CRR:20896 xrashmic - SASKEN
MfwMnuAttr *TimeFormatAttributes(void);

#ifdef FF_MMI_FILEMANAGER
MfwMnuAttr *FMMenuOptionAttributes(void);
MfwMnuAttr *FMRootFolderAttributes(void);
MfwMnuAttr *FMDrivesOptionsListAttributes(void);
MfwMnuAttr *FMDestDrivesListAttributes(void);
MfwMnuAttr *FMDestRootFolderAttributes(void);
#endif
// Sep 27,2005 REF: SPR 34402 xdeepadh  
// 	Nov 14, 2005    REF: OMAPS00044445 xdeepadh
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
MfwMnuAttr *PlayerChannelAttributes(void);
#endif

//May 04, 2005 CRR:30285 x0021334
MfwMnuAttr *SmsStatusReportAttributes(void);

MfwMnuAttr *GetAnyKeyAnswerMenu (void);
//Aug 22, 2005    REF: ENH 31154 xdeepadh
#ifdef FF_MMI_FILE_VIEWER
MfwMnuAttr *fileviewerMenuAttributes( void );//file viewer
MfwMnuAttr *ViewOptionsMenuAttributes( void );//file viewer
#endif

//Sep 11, 2006 DR: OMAPS00094182 xrashmic
#ifdef FF_MMI_TEST_MIDI 
MfwMnuAttr *MidiTestAppMenuAttributes(void);
#endif
#if defined(FF_MMI_TEST_MP3) ||  defined(FF_MMI_TEST_AAC) 
MfwMnuAttr *AudioPlayerAppMenuAttributes(void);
#endif

/*******************************************************************************
                                                                              
                    Include Files For Phone Book Functionality
                                                                              
*******************************************************************************/

/* These includes need to be here as they depend on definitions
   defined above.
*/
#include "MmiBookUtils.h"
#include "MmiBookServices.h"
#include "MmiBookSearchWindow.h"
#include "MmiEmoMenuWindow.h"
#include "MmiBookDetailsWindow.h"
/* SPR#1428 - SH - New Editor changes: BookInputWindow is no longer required */
#ifndef NEW_EDITOR
#include "MmiBookInputWindow.h"
#endif /* NEW_EDITOR */
#include "MmiBookCallListWindow.h"
#include "MmiBookCallDetailsWindow.h"
#include "MmiBookSDNWindow.h"
#include "MmiBookPhonebook.h"
#include "MmiMenu.h"



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/


#endif
