
#ifndef _DEF_MMI_DUMMY_H_
#define _DEF_MMI_DUMMY_H_


/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		
 $File:		    MmiDummy.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
 *******************************************************************************                                                                             
  $History: MmiServices.c

    xreddymn Jun-29-2005 MMI-ENH-32467:
    Handling of animated GIF images. Added E_WIN_SUSPEND to
    provide MfwWinSuspend event to WAP window

	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
	Description:	IMEI retreival  by posting a call to ACI sAT_xx API
	Solution:		The direct call to cl_get_imeisv() is replaced with the
					call to the callNUmber() which inturn calls sAT_Dn() and
					retrieves the IMEI info                                                                            
 
*******************************************************************************/



/****************************************************************************************
 *
 * Common Definitions for template and MFW compatibility
 *
 ****************************************************************************************
 */
/*
 * compatibility definitions
 * MFW - GUI uses definitions which are not compatible with the
 * interface !!!
 */

#define mfw_parent mfwParent
#define mfw_header mfwHeader

#define win_create winCreate
#define win_delete winDelete
#define win_show   winShow
#define win_hide   winHide
#define win_unhide winUnhide
#define E_WIN_VISIBLE MfwWinVisible
#define E_WIN_RESUME MfwWinResume
#define E_WIN_SUSPEND MfwWinSuspend
#define E_WIN_DELETE MfwWinDelete

// xreddymn Jun-28-2005 MMI-SPR-32467
#define E_WIN_SUSPEND MfwWinSuspend

#define kbd_create kbdCreate
#define kbd_delete kbdDelete
/* SPR#1428 - SH - New Editor: no longer required */
#ifndef NEW_EDITOR
#define edt_create edtCreate
#define edt_delete edtDelete
#define edt_reset  edtReset
#endif

#define tim_delete   timDelete
#define tim_create   timCreate
#define tim_start    timStart
#define tim_stop     timStop
#define tim_set_time timSetTime

#define sat_create satCreate
#define sat_init   satInit
#define sat_delete satDelete
#define sat_exit   satExit

#define sat_done( a, b )    satDone( a, (U8 *) b, (int) sizeof( b ) )

typedef MfwSat T_MFW_SAT;
typedef MfwTim T_MFW_TIM;
typedef MfwWin T_MFW_WIN;

typedef SatCmd   T_SAT_CMD;
typedef SatTxt   T_SAT_TXT;
typedef SatInput T_SAT_INPUT;

typedef MfwMnuItem T_MFW_MNU_ITEM;
typedef MfwMnuAttr T_MFW_MNU_ATTR;
typedef MfwMnu     T_MFW_MNU;
typedef MfwKbd     T_MFW_KBD;
#ifndef NEW_EDITOR	/* SPR#1428 - SH - New Editor: no longer required */
typedef MfwEdtAttr T_MFW_EDT_ATTR;
#endif

#define MFW_SAT_REFRESH         MfwSatRefresh
#define MFW_SAT_TEXT_OUT        MfwSatTextOut
#define MFW_SAT_GET_KEY         MfwSatGetKey
#define MFW_SAT_GET_STRING      MfwSatGetString
#define MFW_SAT_PLAY_TONE       MfwSatPlayTone
#define MFW_SAT_SETUP_MENU      MfwSatSetupMenu
#define MFW_SAT_SELECT_ITEM     MfwSatSelectItem
#define MFW_SAT_SEND_SMS        MfwSatSendSMS
#define MFW_SAT_SEND_SS         MfwSatSendSS
#define MFW_SAT_CALL            MfwSatCall
#define MFW_SAT_SESSION_END     MfwSatSessionEnd

#define SAT_RES_SUCCESS         SatResSuccess
#define SAT_RES_PARTIAL         SatResPartial
#define SAT_RES_MISSING         SatResMissing
#define SAT_RES_ADDITIONAL      SatResAdditional
#define SAT_RES_MODIFIED        SatResModified
#define SAT_RES_USER_ABORT      SatResUserAbort
#define SAT_RES_USER_BACK       SatResUserBack
#define SAT_RES_USER_NO_RESP    SatResUserNoResp
#define SAT_RES_USER_HELP       SatResUserHelp
#define SAT_RES_BUSY_ME         SatResBusyME
#define SAT_RES_BUSY_NET        SatResBusyNet
#define SAT_RES_REJECT          SatResReject
#define SAT_RES_CLEARED         SatResCleared
#define SAT_RES_CC_TEMP         SatResCCTemp
#define SAT_RES_IMPOSSIBLE      SatResImpossible
#define SAT_RES_UNKNOWN_TYPE    SatResUnknownType
#define SAT_RES_UNKNOWN_DATA    SatResUnknownData
#define SAT_RES_UNKNOWN_NUMBER  SatResUnknownNumber
#define SAT_RES_SS_ERROR        SatResSSerror
#define SAT_RES_SMS_ERROR       SatResSMSerror
#define SAT_RES_NO_VALUES       SatResNoValues
#define SAT_RES_USSD_ERROR      SatResUSSDerror
#define SAT_RES_CC_PERMANENT    SatResCCPermanent

/*
 * end of compatibility definitions
 */
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg 
#ifdef FF_MMI_ATC_MIGRATION
EXTERN char imei[];
#endif
char* get_build_user(void); // RAVI
char* get_build_date(void); // RAVI
char* get_build_time(void); // RAVI
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
#ifdef FF_MMI_ATC_MIGRATION
void mmi_imei_retrieve();
#else
int get_imei(char *imei);
#endif
extern T_MFW_HND idle_get_window (void);

/* Added to remove warning Aug - 11 */
#ifdef PCM_2_FFS
      extern  S32 ffs_file_read ( const S8 * pathname, void * buf, S32 nbytes );
#endif  /* PCM_2_FFS */
/* End - remove warning Aug - 11 */

#endif

