/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Supplementary services 
 $File:		    MmiServices.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description

    This module provides the supplementary services functionality

  
********************************************************************************
 $History: MmiServices.c

	May 03, 2007 DR: OMAPS00129383 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on BAT/ATI level
 	Solution: Replaced BASIC_SERVICE_CODE with TELETELE_BEARER_SERVICE_CODE to activate 
 			and de-activate both Tele and Bearere services
 	
  	Feb 27, 2007 ER: OMAPS00113891 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on BAT/ATI level
 	Solution: Did the following changes:
	 		- Externed call_data variable
			- In function showssNotify(), 
				>> handling the case MFW_SS_MT_FORWARDED: where  
					incoming call dialog will be displayed with proper string for TTY/normal forwarded 
					incoming call.
				>>New case MFW_SS_MT_CALL_FORWARDED is added to display a dialog "Incoming
					call forwarde" to the subscriber who has forwarded his all incoming calls
 	
 	Nov 27, 2006 OMAPS00098359 a0393213(R.Prabakar)
 	Description: CPHS 4.2 : Call Divert doesn't work properly for Line 2

	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	 Dynamically changing the dcs value in editor settings in functions showUSSDInfo() and showUSSDCNF()

	Aug 28, 2006  DR: OMAPS00083503 x0pleela
	Description: Result of ss_check_ss_string may be ignored
	Solution:	Defined new enumeration T_SS_APLHA for DCS
			services(): Checking for dcs and if UCS2 character then display a dialog 
						saying font not supported. Else proceed with the transaction
			showUSSDError():This function displays a dialog saying USSD message received, 
					Font not supported for UCS2 characters
 

 	Aug 23, 2006 DR: OMAPS00090419 x0pleela
 	Descroption:FT Nice/France, Orange (wk32/06) - Call Barring - misleading message:
 	Solution:Commented  the code (function getCBService()) which appends the ss string with *11 
 			or *89 as it is not a valid SS string for checking status, activation and deactivation of
 			call barring feature in functions servicesCallBarringCheck, servicesCallBarringDeactivate,
 			servicesCallBarringActivate 
 			
       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation
       Solution     : As part of implementation, in this file, call to cphs_set_divert_status is called whenever necessary.
       The current code is put under the else part of #ifdef FF_CPHS. Some clean-up of existing code is also done.

	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
 			a) Defined new enumeration "SERVICE_CB_SERVICE" for SS similar to Line 1 and Line 2
 			b) Defined new function "getCBService()" to update psstring based on the current 
 				line activated while processing SS

	Apr 10, 2006    REF: OMAPS00052691 x0039928
	Description: FT USA [Cingular(WK42)] - T 8.6 Call Waiting Accept - 
	                  ATD *43# returns "Unknown status"
	Solution: Proper Text is displyed when network returns status STAT_NONE( 0 ) for 
	              call waiting activation.

	Dec 07, 2005    REF: OMAPS00050087 x0039928
	Description: Deactivated is displayed while activate Hide Id option is selected
	Solution: String "Not Activated" is displayed if the Hide Id activation fails.
	
	July 09, 2005   REF : MMI-22565 - a0876501
	Description:	IMEI CD byte calculation added
	Solution:	Increased the IMEI_LEN from 15 to 17 for displaying the Cd byte.

 	Jun 16, 2005    REF: CRR 31543   x0021334
	Issue: SATK: cannot get TR for send ussd command
	Solution: When the user presses "Hang up" key when the SAT USSD session is in progress,  
	              a call is made to function 'sAT_end_ussd()' to end the USSD session.

 	May 12, 2005 REF: CRR 30892  x0021334
 	Bug: MMI doesn't handle 2nd Interogation of "CW for basic services" using USSD *#43#.
 	Fix:  Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification

	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
	Description:	IMEI retreival  by posting a call to ACI sAT_xx API
	Solution:		The direct call to cl_get_imeisv() is replaced with the
					call to the mmi_imei_store() which inturn calls sAT_Dn() and
					retrieves the IMEI info and stores it onto the global 
					variable "imei"
					
	Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
	Bug : Unable to compose a one or two digit number
	Fix :  This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled.
		The read only editor with the service provider message that comes up when a one or two digit is dialled
		was not handled properly. So altered the editor handling to cater for the long message.
		
	Oct 20, 2004    REF: CRR 25772  Deepa M.D
	Bug:	Not displaying proper RSK name in call divert edit screens
	Fix:	The RSK name will be changed based on whether the editor content is empty or not.

    Jul 23, 2004        REF: CRR 16107  xvilliva
    Bug:  (Call Barring) - MMI response overlayes partially background images.
    Fix:    Alert being displayed is altered.

        Jun 21,2004 CRR:13650 xpradipg - SASKEN

    Jul 16, 2004        REF: CRR 18409  xvilliva
    Bug:  CHW : message "Deactivated" when it should be "Activated". 
    Fix:  The alert displayed depends on the status code received. If we 
	      receive the status as zero or 0xFF, we now show an alert saying 
	      "unknown status" instead of "Deactivated".


    Jun 21,2004 CRR:13650 xpradipg - SASKEN
 
    	Jun 23, 2004        	REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
  	Solution:			The CF state is maintained in flash. During bootup the CF icon in the idle screen
  					 is displayed depending upon its state in the ffs
  					
    	Jun 10, 2004        	REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
  	Solution:			The condition for which Call Forward is activated/deactivated or its staus is checked, 
					is displayed in the message along with the activation/deactivation message

Jun 21,2004 CRR:13650 xpradipg - SASKEN
Description: CBA : no basic service sent when using the menu
Fix: Basic service code 10(All teleservices) is sent for activation and deactivation thru menus

				

	Jun 04,2004 CRR:13601 xpradipg - SASKEN
	Description: CFO interrogation display : Timer to long
	Fix: Timer is enabled for the information dialog. It times out after 5 seconds from the last key
	     press.
	        
	Jun 03, 2004 CRR:13649 - xpradipg-SASKEN
	subject: CBA : Management of password
  	solution: password is promted on selecting activation/deactivation option as against the previous
  	case where it was shown up on selecting CallBarring option in the Network Services


 	Issue Number : SPR#12882 on 29/03/04 by VadiRaj
 	
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End
 
*******************************************************************************/


/******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#define BUGS 0

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
#include "prim.h"


#include "mfw_sys.h"
#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif /* NEW_EDITOR */
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_tim.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sat.h"
#include "mfw_ffs.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_cm.h"
#include "mfw_ss.h"
#include "dspl.h"
#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiIdle.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiCall.h"
#include "MmiServices.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUITextEntry.h"
#else /* NEW_EDITOR */
#include "MmiEditor.h"
#endif /* NEW_EDITOR */

#include "MmiCPHS.h"


#include "MmiBookUtils.h"
#include "MmiBookShared.h" // MZ 26/02/01
#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"
#include "aci_cmh.h"    // Marcus: Issue 1494: 18/12/2002: for MAX_USSD_LEN

//gprads CQ-16432
#include "AUIEditor_i.h"
extern void msgsGetVoiceAddress(CHAR *Buffer);
extern void InfoDialogTimRestart(T_MFW_HND win);

/* x0039928 - Lint warning removal 
static void (*show) (void); */

static void showWait (T_MFW_HND win);
static void showOk (T_MFW_HND win);
static void showFailed (T_MFW_HND win);
static void showError (T_MFW_HND win, T_MFW_SS_ERROR ss_error);
static void showCLIStatus (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf );
static void showSSNotify (T_MFW_HND win, T_MFW_SS_NOTIFY  * ss_notify);



static void showIMEI2 (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf);
static void showUSSDREQ (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf);
static void showUSSDCNF (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf);
static void showUSSDEnter (void);
static void showUSSDFull (T_MFW_HND win);
static void sendUSSD (T_MFW_SS_PARA *ussd_senddata);
static int servicesSendCallDivert(char * divert_number);
int servicesUSSDEdit(struct MfwMnuTag* m, struct MfwMnuItemTag* i);
int servicesUSSDDelete(struct MfwMnuTag* m, struct MfwMnuItemTag* i);
static void servicesShowUSSDMenu(void);
static void servicesStoreUSSD(void);

static void ServicesGetOtherNumber(T_MFW_HND win);
static void servicesStoreVoiceMailNumber( void );
#ifdef NEW_EDITOR
static void services_get_voice_number_cb(T_MFW_HND win, USHORT identifier, SHORT reason);
#else /* NEW_EDITOR */
static void services_get_voice_number_cb(T_MFW_HND win, USHORT reason);
#endif /* NEW_EDITOR */

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
static void getCBService(char * pSSstring);
#endif

//x0pleela 24 Aug, 2006  DR: OMAPS00083503
//Function to display a dialog saying font not suported for UCS2 characters
#ifdef MMI_LITE
static void showUSSDError (T_MFW_HND win);
#endif

#ifdef FF_CPHS_REL4
extern BOOL interrogate;
#endif

//x0pleela 27 eb, 2007  ER: OAMPS00113891
//get the call_data structure
extern T_call call_data;

typedef struct
{
  T_MMI_CONTROL   mmi_control;
  T_MFW_HND       parent_win;
  T_MFW_HND       info_win;
  T_MFW_HND       info_tim;
  T_MFW_HND       info_kbd;
  T_MFW_HND       info_kbd_long;
  USHORT          LeftSoftKey;     /* used for Ok */
  USHORT          RightSoftKey;    /* Used for Back */
  ULONG           Time;
  T_MFW_EVENT     KeyEvents;
  USHORT          TextId1;          /* Line 1 */
  USHORT          TextId2;          /* Line 2 */
  char *          TextString4;
  USHORT          Identifier;
  T_VOID_FUNC     Callback;
} T_dialog_ext_info;

//x0pleela 24 Aug, 2006  DR: OMAPS00083503

typedef enum          /* see DCS in Rec. GSM 03.38 */
{
  SS_ALPHA_7_Bit    = 0,
  SS_ALPHA_8_Bit    = 1,
  SS_ALPHA_16_Bit   = 2,
  SS_ALPHA_Reserved = 3
}
T_SS_ALPHA;


T_MFW_HND dialog_ext_info_dialog (T_MFW_HND            parent_win,
                                  T_dialog_ext_info  * display_info);

static MfwHnd lastFocus;				/* store old focus			*/
static MfwHnd win;                      /* our window               */
 /*x0039928- Lint warning removal */
/* static MfwHnd kbd;                      // our keyboard handler     */
static MfwHnd times;					/* timeout				    */

/* x0039928 - Lint warning removal
static MmiState nextState;              // next state when finished */
static int servicesState;				/* current state of module	*/


static T_MFW_SS_PARA *ss_cnf;           /* union container for all SS confirmations */

                                         /* decode service type - tele/bearer */
static unsigned int serviceGetServiceType( UBYTE ss_telecom_type, UBYTE ss_telecom_service);
static unsigned int serviceGetCategory( T_MFW_SS_CATEGORY cat );
static unsigned int serviceGetType( T_MFW_SS_CODES type );
static unsigned int ServiceGetStatus( UBYTE pStatus);
static unsigned int serviceGetError( T_MFW_SS_ERROR ss_error);



#define  SS_STAT_NONE  0    //xvilliva 18409, not registered, not active, 
							//not provisioned,not quiescent

//x0pleela 03 May, 2007 DR: OMAPS00129383
//defined new service code for both Teleservices and Bearer services
#define TELE_BEARER_SERVICE_CODE "09"

//June 17, 2004 - CRR:13650 - CBA : no basic service sent when using the menu
//basic service code for "all teleservices"
#define BASIC_SERVICE_CODE "10"
int ss_Code;
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
// ss_Code-- This holds the current CF code for which the activation/deactivation or the staus is being checked
// This would be used while displaying the status or activation/deactivation message


typedef enum
{
	CFBusy = 0,
	CFNoReach,
	CFNoReply,
	CFAllConditional,
	CFUnconditional,
	CFAllCalls
} SERVICE_CF_TYPE;

typedef enum
{
	CBAllOutgoing = 0,
	CBInternational,
	CBInternationalHome,
	CBAllIncoming,
	CBIncomingRoaming
} SERVICE_CB_TYPE;



typedef enum
{
	CFVoice = 0,
	CFAuxVoice, /*MC, ALS line 2*/ 
	CFFax,
	CFData,
	CFAll
} SERVICE_CF_SERVICE;

//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
typedef enum
{
	CBVoice = 0,
	CBAuxVoice /*MC, ALS line 2*/ 
	
}SERVICE_CB_SERVICE;
#endif

typedef enum
{
	CFActivate = 0,
	CFDeactivate,
	CFCheck
} SERVICE_CF_ACTION;

typedef enum
{
	CBActivate = 0,
	CBDeactivate,
	CBCheck,
	CBCheckPassword,
	CBNewPassword,
	CBConfirmNewPassword,
	//Jun 02, 2004 CRR:13649 - xpradipg-SASKEN
	//added for holding the state for callbarring service
	//CQ-13649:start
       CBCancelAll,
	CBActionNone,
	//CQ-13649:end
        //CQ-16432 start
	CBEnterSatPassword
	//CQ-16432 end
} SERVICE_CB_ACTION;



/* NOTE that these are needed in new architecture */
static SERVICE_CF_TYPE       CFType;        /* Type of Call Forwarding */
static SERVICE_CF_SERVICE    CFService;     /* basic service for Call Forwarding */
//x0pleela 25 May, 2006  DR: OMAPS00070657
/* x0039928 - Lint warning removal 
#ifdef FF_CPHS
static SERVICE_CB_SERVICE    CBService;     // basic service for Call Barring 
#endif
*/
static SERVICE_CF_ACTION     CFAction;      /* action for Call Forwarding */
static SERVICE_CB_TYPE       CBType;		/* Type of Call Barring */
static SERVICE_CB_ACTION     CBAction;		/* action for Call Barring */
//Jun 02, 2004 CRR:13649 - xpradipg-SASKEN
//new enum added to indicate the action to be taken in the callback function for password
static SERVICE_CB_ACTION	   CBFlag;	/*to indicate the action after retriving the password*/
//CRR:13649
/* x0039928 - Lint warning removal 
static char                 CFNumber[MAX_LEN];  number to forward calls to */



/* end of bits for USSD */



#define CF_ACTIVE_STATUS        1
#define CF_INACTIVE_STATUS      2
/* Marcus: Issue 1652: 03/02/2003 */
#define CF_PROVISIONED_STATUS   3
#define CF_UNKNOWN_STATUS       4


/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
    Description : CPHS Call Forwarding feature implementation
    Solution     : As part of this implementation, some #defines and struct definitions are moved to MmiServices.h*/

#define MFW_CFU_VOICE_ACTIVE	0x01
#define MFW_CFU_FAX_ACTIVE	0x02
#define MFW_CFU_DATA_ACTIVE	0x04
#define MFW_CFU_SMS_ACTIVE	0x08

#define MFW_CFU_ALL_SERVICES_ACTIVE (MFW_CFU_VOICE_ACTIVE | MFW_CFU_FAX_ACTIVE | MFW_CFU_DATA_ACTIVE | MFW_CFU_SMS_ACTIVE)
#define MFW_CFU_ALL_BASIC_SERVICES_ACTIVE (MFW_CFU_VOICE_ACTIVE | MFW_CFU_FAX_ACTIVE | MFW_CFU_DATA_ACTIVE)

T_ss ss_data = {0};
char satPassword[10]={0};//CQ-16432


static void srv_empty_list_result (UBYTE ss_category, USHORT event, UBYTE ss_status, UBYTE ss_code); /* Marcus: Issue 1652: 03/02/2002 */
static void showCFStatus (T_MFW_HND win , T_ss  * data );
static UBYTE srv_copy_feature_list_cf (T_ss_feature_cf * out_list, T_MFW_SS_CF_CNF * in_list);
static void srv_get_forwarded_to_number ( char * textString,
                                          char * forwarded_to_number,
                                          UBYTE max_chars);
static UBYTE srv_get_service_type ( USHORT * textId,
                                    UBYTE ss_telecom_type, UBYTE ss_telecom_service);
static void srv_get_status (USHORT *textId, UBYTE pStatus);
static UBYTE srv_check_CF_telephony_status (T_MFW_SS_CF_CNF * in_list);

static void showCWStatus (T_MFW_HND win, T_ss  * data );
static UBYTE srv_copy_feature_list_cw (T_ss_feature_cw * out_list, T_MFW_SS_CW_CNF * in_list);

static void showCBStatus (T_MFW_HND win, T_ss  * data );
static UBYTE srv_copy_feature_list_cb (T_ss_feature_cb * out_list, T_MFW_SS_CB_CNF * in_list);


static int services_win_cb(T_MFW_EVENT e, T_MFW_SS_PARA * para);
static int services_ss_cb(T_MFW_EVENT e, T_MFW_HND para);

static int services_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reason);


static void srv_display_status (UBYTE ss_status);



typedef struct
{
    int 		   text;
#ifdef NEW_EDITOR		/* SPR#1428 - SH - New Editor changes */
	T_ED_ATTR	  *editor_attr_input;
#else /* NEW_EDITOR */
    MfwEdtAttr*    edt_attr_input;
#endif /* NEW_EDITOR */
    int            left_soft_key;
    int            right_soft_key;
	short		   abc;   /* numeric or alpha? */
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes */
	T_AUI_EDIT_CB callback;
#else  /* NEW_EDITOR */
    void (* callback)(T_MFW_HND win,USHORT reason);
#endif  /* NEW_EDITOR */
} T_input_properties;




typedef struct
{
    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       parent_win;
    T_MFW_HND       win;
    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes */
	T_MFW_HND		 edit_win;
#else /* NEW_EDITOR */
    T_MFW_HND       edt_input;
#endif /* NEW_EDITOR */
	T_input_properties properties;
} T_services_input;

/* SPR#1428 - SH - New Editor changes - no longer required*/
#ifndef NEW_EDITOR
void services_number_input_cb(T_MFW_HND win, UBYTE identifier, USHORT reason);
#endif



static T_MFW_HND services_input_create (MfwHnd parent);
static void services_input_destroy (MfwHnd window);
static MfwHnd services_input_start(MfwHnd parent,T_input_properties *properties);
void services_input (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int services_input_win_cb(MfwEvt e, MfwWin *w);

/* SPR#1428 - SH - New Editor changes - no longer required*/
#ifndef NEW_EDITOR
static int services_input_kbd_cb(MfwEvt e, MfwKbd *k);
static int services_input_kbd_long_cb(MfwEvt e, MfwKbd *k);
#endif



/* editor   */
//GW Moved editor definitions to MmiResources.h
#define SAT_EDITOR_COLOR            0
#define SAT_EDITOR_FONT             0
#define SAT_EDITOR_CONTROLS         0
#define SAT_EDITOR_BUFFER_SIZE      32
/*    */

//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
#ifdef FF_MMI_ATC_MIGRATION
#define IMEI_LEN 17 /*a0393213 warnings removal - changed 15 to 17*/
#endif

#ifdef FF_CPHS_REL4
extern UBYTE msp_reg_profid;
extern BOOL msp_register;
#endif


/*******************************************************************************

 $Function:    	servicesInit

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/



T_MFW_HND servicesInit (T_MFW_HND parent)
{

	TRACE_FUNCTION("servicesInit function");

  /*
   * Initialise MFW
   */
    ss_mfw_init();
    /*
    * Create any other handler
    */
//16432
//Changed ss_create(0,0xFFFF,....)
    ss_data.ss_handle = ss_create (0,
                                0xFFFF,
                                (T_MFW_CB)services_ss_cb);

    ss_data.win = 0;
    return (services_create (0));


}


/*******************************************************************************

 $Function:    	servicesExit

 $Description:	exit services handling

 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesExit (void)
{
    TRACE_FUNCTION("servicesExit");

    ss_delete(ss_data.win);
    ss_exit();
}


/*******************************************************************************

 $Function:    	services_create

 $Description:	Creation of an instance for the Services dialog.

 $Returns:		

 $Arguments:	
 
*******************************************************************************/


T_MFW_HND services_create (T_MFW_HND parent_window)
{
    T_ss     * data = &ss_data;
    T_MFW_WIN     * win;

    TRACE_FUNCTION("services_create");

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)services_win_cb);

    if (data->win EQ 0)
      return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog    = (T_DIALOG_FUNC)services;
    data->mmi_control.data      = data;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
     * Create any other handler
     */
    data->info_win = 0;
    data->info_wait = 0;
    data->cb_menu_win = 0;

	/* SPR#1428 - SH - New Editor changes - this editor never used */
#if 0
    data->ussd_num_win = 0;
    data->ussd_name_win = 0;
#endif

    data->input_number_win = 0;
	memset(data->edt_buf_name,'\0',sizeof(data->edt_buf_name));
	memset(data->edt_buf_number,'\0',sizeof(data->edt_buf_number));
	memset(data->edt_buf_cb_password,'\0',sizeof(data->edt_buf_cb_password));  /* x0039928 - Lint warning fix */
	/* SPR#1428 - SH - New Editor changes - no longer required*/
#ifndef NEW_EDITOR
	bookSetEditAttributes(SERVICE_CREATE_EDITOR,COLOUR_EDITOR_XX,0,edtCurBar1,0,(char*)data->edt_buf_name,PHB_MAX_LEN,&data->edt_attr_name);
	bookSetEditAttributes(SERVICE_CREATE_EDITOR,COLOUR_EDITOR_XX,0,edtCurBar1,0,(char*)data->edt_buf_number,PHB_MAX_LEN,&data->edt_attr_number);
#endif /* ifndef NEW_EDITOR */

	/* NDH : 28/08/2003 : Intialise cfu_services to "ALL Inactive" */
	data->cfu_services = 0;

    winShow (data->win);

  /*
   * return window handle
   */
  return data->win;
}


/*******************************************************************************

 $Function:    	services_destroy

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void services_destroy (T_MFW_HND own_window)
{
//  T_MFW_WIN     * win;    // RAVI
  T_ss     * data = &ss_data;

  TRACE_FUNCTION("=== services_destroy");

  if (data)
  {
    /*
     * Delete WIN Handler
     */

    if (data->info_win)
    {
      SEND_EVENT(data->info_win, DIALOG_DESTROY, 0, 0);
      data->info_win=0;
    }
    if (ss_data.info_wait)
    {
    	TRACE_FUNCTION("destroying the wait screen b");
        SEND_EVENT(ss_data.info_wait, DIALOG_DESTROY, 0, 0);
        ss_data.info_wait = 0;
    }
    

    if (data->input_number_win)
    {
       TRACE_FUNCTION("killing input number win");
       services_input_destroy( data->input_number_win );
       data->input_number_win=0;
    }
  }
}

/*******************************************************************************

 $Function:    	services_win_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static int services_win_cb(T_MFW_EVENT e, T_MFW_SS_PARA * para)
{
  /*
   * Top Window has no output
   */
    TRACE_FUNCTION ("services_win_cb()");
    switch (e)
    {
        case MfwWinVisible:
        break;
        default:
        return 0;
    }
  return 1;

}

/*******************************************************************************

 $Function:    	destroyInfoWindow

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void destroyInfoWindow( void)
{
    TRACE_FUNCTION("destroyInfoWindow");

    /* kill info win if there */
    if (ss_data.info_wait)
    {
        TRACE_FUNCTION("destroying the wait screen a");
        SEND_EVENT(ss_data.info_wait, DIALOG_DESTROY, 0, 0);
        ss_data.info_wait = 0;
    }
    if (ss_data.input_number_win)
    {
        TRACE_FUNCTION("destroying input window!!!");
        SEND_EVENT(ss_data.input_number_win, SERVICES_INPUT_DESTROY, 0, 0);
        ss_data.input_number_win = 0;
    }

}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         MODULE: MFW_DMY              |
| STATE  : code                         ROUTINE: ss_getAlphabetCb        |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to extract the used alphabet out
            of the data coding scheme.
*/
UBYTE ss_getAlphabetCb ( UBYTE dcs )
{
  UBYTE alphabet = 0;  /* means 7 bit default alphabet */

  switch (dcs & 0xF0)
  {
    case( 0x10 ):
      if (dcs EQ 0x11)
        alphabet = 2;
      break;

    case( 0x70 ):
    case( 0x60 ):
    case( 0x50 ):
    case( 0x40 ):
      alphabet = (dcs & 0x0C) >> 2;
      /* According to 03.38, "Any reserved codings shall be assumed 
       * to be the GSM 7 bit default alphabet by a receiving entity".
       */
      if (alphabet EQ 3)
        alphabet = 0;
      break;

    case( 0xF0 ):
      alphabet = (dcs & 0x04) >> 2;
      break;
  }

  return alphabet;
}
/*******************************************************************************

 $Function:    	services_ss_cb

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static int services_ss_cb(T_MFW_EVENT e, T_MFW_HND para)
{

    TRACE_FUNCTION ("services_ss_cb()");

    {
   //   T_MFW_WIN * win;   // RAVI
   //   win = (T_MFW_WIN *) ((T_MFW_HDR *)ss_data.win)->data;  // RAVI
    }

    switch (e)
	{
	case E_SS_RES:
        if (!ss_data.info_wait) 
        {
			TRACE_FUNCTION ("show the wait screen");
#ifdef FF_CPHS_REL4  
		if(interrogate != 1)
		{
#endif		
			showWait( ss_data.win );
#ifdef FF_CPHS_REL4  
		}
#endif		
        }
        /*
         * ignore the event
         */
        break;

	case E_SS_NOTIFY:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_NOTIFY,0, para);

        destroyInfoWindow();
        break;

     case E_SS_CF_CNF:

		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_CF_CNF,0, para);

        destroyInfoWindow();
        break;

     case E_SS_CF_ICN_UPD:

		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_CF_ICN_UPD,0, para);

        destroyInfoWindow();
        break;

     case E_SS_CW_CNF:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_CW_CNF,0, para);

        destroyInfoWindow();
        break;

     case E_SS_CLI_CNF:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_CLI_CNF,0, para);

        destroyInfoWindow();
        break;

     case E_SS_CB_CNF:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_CB_CNF,0, para);

	    destroyInfoWindow();
        break;

    /* we must display the ussd info and then edit and send a user ussd */
    case E_SS_USSD_REQ:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_USSD_REQ,0, para);

        destroyInfoWindow();		
        break;

    /* just display the ussd info */
    case E_SS_USSD_CNF:
		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_USSD_CNF,0, para);

        destroyInfoWindow();
        break;

    case E_SS_IMEI:

		if(!ss_data.info_win)
	        SEND_EVENT(ss_data.win,E_SS_IMEI,0, para);

        destroyInfoWindow();
        break;


	case E_SS_GET_PW:

		if(!ss_data.info_win)
			SEND_EVENT(ss_data.win,E_SS_GET_PW,0, para);

        destroyInfoWindow();
	break;
	//CQ-16432 - start
	case E_SS_SAT_CPWD:
	TRACE_EVENT("Received event E_SS_SAT_CPWD in BMI");	
		if(!ss_data.info_win)
			SEND_EVENT(ss_data.win,E_SS_SAT_CPWD,0, para);
        destroyInfoWindow();
	break;
	//CQ-16432 - end
     default:
        break;
    }

    return 1;  /* consumed */
}


/*******************************************************************************

 $Function:    	services

 $Description:	

 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void services (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_ss           * data = (T_ss *)win_data->user;

    T_MFW_SS_PARA  * ss_cnf = (T_MFW_SS_PARA *)parameter;

//    unsigned int i = 0;  // RAVI

//    char line[MAX_LINE];  // RAVI

    TRACE_FUNCTION ("services()");

    switch (event)
	{


        case ServicesError:
            TRACE_FUNCTION ("ServicesError");
            showError(win, ss_cnf->cb_cnf.ss_error);
            break;

        case ServicesFailed:
            TRACE_FUNCTION ("ServicesFailed");
            showFailed(win);
            break;

        case ServicesOtherNumber:
            TRACE_FUNCTION("ServicesOtherNumber");
            ServicesGetOtherNumber(win);
            break;

        case ServicesOk:
            TRACE_FUNCTION ("ServicesOk");
            showOk(win);
            break;

        case ServicesUSSDEnter:
        {
#ifdef NEW_EDITOR	
				T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
        		T_EDITOR_DATA	editor_data;
#endif /* NEW_EDITOR */

				TRACE_FUNCTION("ServicesUSSDEnter");

				memset(ss_data.edt_buf_number,'\0',PHB_MAX_LEN);

				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR /* NEW_EDITOR */
				AUI_edit_SetDefault(&editor_data);
				AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
				AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_number, MAX_USSD_LEN);
				AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtEntAnw, NULL);
				AUI_edit_SetAltTextStr(&editor_data, 1, TxtNull, FALSE, TxtNull);
				AUI_edit_SetEvents(&editor_data, ServicesUSSDEnter, TRUE, FOREVER, (T_AUI_EDIT_CB)services_info_cb);

				AUI_edit_Start(ss_data.win, &editor_data);  /* start the editor */
#else /* NEW_EDITOR */
				editor_data.editor_attr.win.px   = 6;
				editor_data.editor_attr.win.py   = 12;
				editor_data.editor_attr.win.sx   = 80;
				editor_data.editor_attr.win.sy   = 20;
				editor_data.editor_attr.edtCol	= COLOUR_EDITOR_XX,
				editor_data.editor_attr.font     = 0;
				editor_data.editor_attr.mode    = edtCurBar1;
				editor_data.editor_attr.controls   = 0;
				editor_data.hide			    = FALSE;

				editor_data.editor_attr.text	= (char*)ss_data.edt_buf_number;

				editor_data.editor_attr.size	= sizeof(ss_data.edt_buf_number);
				editor_data.LeftSoftKey			= TxtSoftOK;
				editor_data.RightSoftKey		= TxtSoftBack;
				editor_data.AlternateLeftSoftKey     = TxtNull;
				editor_data.TextString			= '\0';
				editor_data.TextId				= TxtEntAnw;
				editor_data.Identifier			= ServicesUSSDEnter;
				editor_data.mode				= DIGITS_MODE;
				editor_data.timeout		        = FOREVER;
				editor_data.min_enter		    = 1;
				editor_data.Callback		    = (T_EDIT_CB)services_info_cb;
				editor_data.destroyEditor	    = TRUE;


				/* create the dialog handler */
				editor_start(ss_data.win, &editor_data);  /* start the editor */
#endif /* NEW_EDITOR */
        	}
            break;

        case SERVICES_DO_CALL_VOICE_DIVERT:
            servicesStoreVoiceMailNumber();
            servicesSendCallDivert(0);
            break;
        case ServicesDoCallDivert:
            servicesSendCallDivert(0);
            break;

	case E_SS_RES:
        /*
         * event is ignored
         */
        break;
	case E_SS_NOTIFY:
        TRACE_EVENT(" E_SS_NOTIFY");
        showSSNotify( win, (T_MFW_SS_NOTIFY*) parameter );  /* all short ss-related notfications */
        break;

	case E_SS_CF_ICN_UPD:
		TRACE_FUNCTION("E_SS_CF_ICN_UPD");
		
		if ((ss_cnf->cf_cnf.ss_code == SS_CF_CFU) ||
		     (ss_cnf->cf_cnf.ss_code == SS_CF_ALL))
		{
			/*
			* Update icons for idle screen if CFU and telephony
			*/
			switch (srv_check_CF_telephony_status ( &ss_cnf->cf_cnf ))
			{
				case CF_ACTIVE_STATUS:
					iconsSetState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Setting the Current state of CF in Flash						
					FFS_flashData.CFState=TRUE;
					 break;

				case CF_INACTIVE_STATUS:
					iconsDeleteState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Setting the Current state of CF in Flash						
					FFS_flashData.CFState=FALSE;
					break;
			}
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Writng the Current state of CF into ffs						
			flash_write();
			idleEvent(IdleUpdate);
		}
		break;
    /*
     * modified handling for results taking lists in account
     */
	case E_SS_CF_CNF:
      /*
       * check whether we received data what was requested / consistent
       */
      
      switch(ss_cnf->cf_cnf.ss_code)
      {
        case SS_CODES_UNKNOWN:
          /* Marcus: Issue 1652: 03/02/2003: Start */
          TRACE_EVENT("SS_CODES_UNKNOWN");
          {
            T_DISPLAY_DATA   display_info;
        	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtNull,  TxtExecuted, COLOUR_STATUS);
        	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_CLEAR | KEY_LEFT );
            display_info.Identifier   = E_SS_CF_CNF;  // can be used also for CF and CW

            /*
             * Call Info Screen
             */

            ss_data.info_win = info_dialog (ss_data.win, &display_info);
          }
          /* Marcus: Issue 1652: 03/02/2003: End */
          break;
		case SS_CF_CFU:
		case SS_CF_ALL:

			/*
			* Update icons for idle screen if CFU and telephony
			*/
			
      		/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
                  Description : CPHS Call Forwarding feature implementation
                  Solution     : As part of the implemention, cphs_set_divert_status() is called whenever the divert status
                                    is changed*/
                                                
			TRACE_EVENT("services():E_SS_CF_CNF SS_CF_CFU");
			switch (srv_check_CF_telephony_status ( &ss_cnf->cf_cnf ))
			{			    
				case CF_ACTIVE_STATUS:
					#ifdef FF_CPHS
					switch (CFService)
					{
						case (CFVoice):	
							    /*MC SPR 742, handle line 2, auxiliary telephony*/
								#ifdef FF_CPHS_REL4
								if(isCFISFilePresent() )
									{	/*call_forwarding_status_rel4.voice updated*/
										if(call_forwarding_status_rel4.voice != MFW_CFLAG_SET || 
											memcmp(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN)!=0)
											{
												call_forwarding_status_rel4.voice=MFW_CFLAG_SET;
												memcpy(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN);
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
										break;
									}
								
								#endif
								
		    			case CFAuxVoice:
#ifdef FF_CPHS
							if (GetAlsLine() == MFW_SERV_LINE2) 
								{
								TRACE_EVENT("services():call forward activation successful for line2");
									/*30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
									    check to find whether we are doing unnecessary sim write*/
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_SET)  
									cphs_set_divert_status(MFW_CFLAG_NotPresent, MFW_CFLAG_SET, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
						
							else
								{
								TRACE_EVENT("services():call forward activation successful for line1");
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_SET, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
#endif
						break;
						case (CFData): 	
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent())
								{
							#endif
							if(GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET)
								cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_SET);
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.data!= MFW_CFLAG_SET)
											{
												call_forwarding_status_rel4.data=MFW_CFLAG_SET;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
							break;
						case (CFFax):
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
							if(GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
								cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_NotPresent);
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.fax!= MFW_CFLAG_SET)
											{
												call_forwarding_status_rel4.fax=MFW_CFLAG_SET;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
							
							break;
						case (CFAll):	
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							if (GetAlsLine() == MFW_SERV_LINE2)
								{
								TRACE_EVENT("services():call forward activation successful for all");
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_SET || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_SET,MFW_CFLAG_SET);
								}
							else
								{
								TRACE_EVENT("services():call forward activation successful for all");
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_SET || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_SET,MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_SET);
								}
								}
							else
								{
										if(call_forwarding_status_rel4.voice!= MFW_CFLAG_SET ||
										   call_forwarding_status_rel4.fax!= MFW_CFLAG_SET ||	
										   call_forwarding_status_rel4.data!= MFW_CFLAG_SET || 
											memcmp(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN)!=0 )
											{
												call_forwarding_status_rel4.voice= MFW_CFLAG_SET;
										  	 	call_forwarding_status_rel4.fax= MFW_CFLAG_SET;		
										   		call_forwarding_status_rel4.data= MFW_CFLAG_SET;
												memcpy(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN);

												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
						break;
					}
					#else
						iconsSetState(iconIdCallFor);
	//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
	//              Setting the Current state of CF in Flash						
						FFS_flashData.CFState=TRUE;	
						flash_write();
					#endif
					break;

				case CF_INACTIVE_STATUS:
				      #ifdef FF_CPHS	
					switch (CFService)
					{
						case (CFVoice):	
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
								if(isCFISFilePresent() )
									{	/*call_forwarding_status_rel4.voice updated*/
										if(call_forwarding_status_rel4.voice != MFW_CFLAG_NOTSet)
											{
												call_forwarding_status_rel4.voice=MFW_CFLAG_NOTSet;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
										break;
									}
								
								#endif
							    /*MC SPR 742, handle line 2, auxiliary telephony*/
		    			case CFAuxVoice:

							if (GetAlsLine() == MFW_SERV_LINE2)
								{
								TRACE_EVENT("services():call forward deactivation successful for line2");
									/*30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
									    check to find whether we are doing unnecessary sim write*/
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_NOTSet)  
									cphs_set_divert_status(MFW_CFLAG_NotPresent, MFW_CFLAG_NOTSet, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
						
							else
								{
								TRACE_EVENT("services():call forward deactivation successful for line1");
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_NOTSet)
									cphs_set_divert_status(MFW_CFLAG_NOTSet, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
						break;
						case (CFData): 	
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
								if(GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_NOTSet)
									cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_NOTSet);
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.data!= MFW_CFLAG_NOTSet)
											{
												call_forwarding_status_rel4.data=MFW_CFLAG_NOTSet;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
							break;
						case (CFFax):	
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
								if(GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_NOTSet)
									cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_NOTSet,MFW_CFLAG_NotPresent);
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.fax!= MFW_CFLAG_NOTSet)
											{
												call_forwarding_status_rel4.fax=MFW_CFLAG_NOTSet;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
								break;
						case (CFAll):	
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							if (GetAlsLine() == MFW_SERV_LINE2)
								{
								TRACE_EVENT("services():call forward deactivation successful for all");
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_NOTSet || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_NOTSet ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_NOTSet)
									cphs_set_divert_status(MFW_CFLAG_NotPresent, MFW_CFLAG_NOTSet, MFW_CFLAG_NOTSet, MFW_CFLAG_NOTSet);
								}
							else
								{
								TRACE_EVENT("services():call forward deactivation successful for all");
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_NOTSet || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_NOTSet ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_NOTSet)
									cphs_set_divert_status(MFW_CFLAG_NOTSet, MFW_CFLAG_NotPresent, MFW_CFLAG_NOTSet, MFW_CFLAG_NOTSet);
								}
								}
							else
								{
										if(call_forwarding_status_rel4.voice!= MFW_CFLAG_NOTSet ||
										   call_forwarding_status_rel4.fax!= MFW_CFLAG_NOTSet ||		
										   call_forwarding_status_rel4.data!= MFW_CFLAG_NOTSet )
											{
												call_forwarding_status_rel4.voice= MFW_CFLAG_NOTSet;
										  	 	call_forwarding_status_rel4.fax= MFW_CFLAG_NOTSet;		
										   		call_forwarding_status_rel4.data= MFW_CFLAG_NOTSet;
										
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
						break;
					}
					   #else
						 iconsDeleteState(iconIdCallFor);
	//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
	//              Setting the Current state of CF in Flash						
						FFS_flashData.CFState=FALSE;
						flash_write();
					   #endif
					break;
			}
		/*
		* no break
		*/
    	case SS_CLIR:
		case SS_CF_ALL_COND:
		case SS_CF_CFNRy:
		case SS_CF_CFNR:
		case SS_CF_CFB:
          switch(ss_cnf->cf_cnf.ss_error)
          {
            case MFW_SS_NO_ERROR:
              /*
               * now store the MFW data under the SS window
               */
               TRACE_EVENT("services():no ss error");
              data->mfw_ss_cf = (T_ss_feature_cf *)ALLOC_MEMORY (sizeof (T_ss_feature_cf));

              /*
               * copy all interesting feature entries
               */
              data->max_feature = srv_copy_feature_list_cf (data->mfw_ss_cf, &ss_cnf->cf_cnf);
              data->current_feature = /*CFService*/0;
             

              if (data->max_feature EQ 0)
              {
                /* Marcus: Issue 1652: 03/02/2003: Start */
                if (ss_cnf->cf_cnf.ss_feature_count)
                {
                    T_MFW_FEATURE *temp = ss_cnf->cf_cnf.ss_feature_list;
                    srv_empty_list_result (ss_cnf->cf_cnf.ss_category, event, temp->ss_status, ss_cnf->cf_cnf.ss_code);
                }
                else
                {
                    srv_empty_list_result (ss_cnf->cf_cnf.ss_category, event, 0, ss_cnf->cf_cnf.ss_code);
                }
                /* Marcus: Issue 1651: 03/02/2003: End */
                FREE_MEMORY ((void *)data->mfw_ss_cf, sizeof (T_ss_feature_cf));
                data->mfw_ss_cf = NULL;
              }
              else
              {
                /*
                 * now display status
                 */
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
// ss_Code-- This holds the current CF code for which the activation/deactivation or the staus is being checked
// This would be used while displaying the status or activation/deactivation message
                 ss_Code=ss_cnf->cf_cnf.ss_code;
                showCFStatus(win, data);
              }
              break;

            default:
		TRACE_EVENT_P1("services():ss error %d", ss_cnf->cf_cnf.ss_error);
              TRACE_FUNCTION("ERROR in CFU,INTERROGATION");
              showError(win, ss_cnf->cf_cnf.ss_error);
              break;
          }
          break;

        default:
          break;
      } /* end switch(ss_cnf->cf_cnf.ss_code) */
      break;


	case E_SS_CW_CNF:
 	  if (ss_cnf->cw_cnf.ss_error == MFW_SS_NO_ERROR)
      {
        /*
         * now store the MFW data under the SS window
         */
        data->mfw_ss_cw = (T_ss_feature_cw *)ALLOC_MEMORY (sizeof (T_ss_feature_cw));

        /*
         * copy all interesting feature entries
         */
        data->max_feature = srv_copy_feature_list_cw (data->mfw_ss_cw, &ss_cnf->cw_cnf);
        data->current_feature = 0;

        if (data->max_feature EQ 0)
        {


  		  if (ss_cnf->cw_cnf.ss_status NEQ 0xFF AND ss_cnf->cw_cnf.ss_status NEQ 0x00)
			srv_display_status (ss_cnf->cw_cnf.ss_status);
		  else
  		    srv_empty_list_result (ss_cnf->cw_cnf.ss_category, event, ss_cnf->cw_cnf.ss_status, ss_cnf->cf_cnf.ss_code);  /* Marcus: Issue 1588: 23/01/2003 */
          FREE_MEMORY ((void *)data->mfw_ss_cw, sizeof (T_ss_feature_cw));
          data->mfw_ss_cw = NULL;
        }
        else
        {
          /*
           * now display status
           */
			if (ss_cnf->cw_cnf.ss_status EQ SS_STAT_UNKNOWN) 
			{
				//SPR18409 xvilliva, If we receive the status as 0xFF,
				//we show an alert saying "unknown status".  
				info_screen(win, TxtUnknown, TxtStatus, NULL);
			}
			else
			{
				// Apr 10, 2006    REF: OMAPS00052691 x0039928
				// Fix: Proper text id is assigned for display if network returns 
				// status STAT_NONE for call waiting activation.
				if (ss_cnf->cw_cnf.ss_status EQ SS_STAT_NONE)
				{
					if ((ss_cnf->cw_cnf.ss_category EQ SS_REGISTRATION) OR 
						(ss_cnf->cw_cnf.ss_category EQ SS_ACTIVATION))
							data->mfw_ss_cw->status  = TxtActivated;
				}
				showCWStatus(win, data);
			}
	}
    }
	  else
      {
	    showError(win, ss_cnf->cw_cnf.ss_error);
      }
	  break;

	case E_SS_CLI_CNF:
        TRACE_FUNCTION("  E_SS_CLI_CNF");

			if (ss_cnf->cli_cnf.ss_error == MFW_SS_NO_ERROR)
            {
                showCLIStatus(win, ss_cnf);
            }
            else
            {
                showError(win, ss_cnf->cli_cnf.ss_error);
            }
		break;


    case E_SS_CB_CNF:
      switch(ss_cnf->cb_cnf.ss_error)
      {
        case MFW_SS_NO_ERROR:
          /*
           * now store the MFW data under the SS window
           */
          data->mfw_ss_cb = (T_ss_feature_cb *)ALLOC_MEMORY (sizeof (T_ss_feature_cb));

          /*
           * copy all interesting feature entries
           */
          data->max_feature = srv_copy_feature_list_cb (data->mfw_ss_cb, &ss_cnf->cb_cnf);
          data->current_feature = 0;

          if (data->max_feature EQ 0)
          {
            srv_empty_list_result (ss_cnf->cb_cnf.ss_category, event, 0, ss_cnf->cf_cnf.ss_code);  /* Marcus: Issue 1588: 23/01/2003 */
            FREE_MEMORY ((void *)data->mfw_ss_cb, sizeof (T_ss_feature_cb));
            data->mfw_ss_cb = NULL;
          }
          else
          {
            /*
             * now display status
             */
            showCBStatus(win, data);
          }
          break;

        default:
          showError(win, ss_cnf->cb_cnf.ss_error);
          break;
      }
      break;

    case E_SS_IMEI:   /* present IMEI */
       /* T_MFW_IMEI imei_cnf;*/
        TRACE_FUNCTION("  E_SS_IMEI");

        switch(ss_cnf->imei_info.error)
        {
        case MFW_SS_NO_ERROR:
            TRACE_FUNCTION( "Handling IMEI no error" );
//	May 02, 2005   REF : LOCOSTO-ENH-30818 - xpradipg
//	copy the IMEI onto the global variable and display the information
#ifdef FF_MMI_ATC_MIGRATION
			memset(imei,0x00,IMEI_LEN+1);   
    		memcpy(imei, ss_cnf->imei_info.imei_number, IMEI_LEN);
    		showIMEI(win,(char*)imei);
#else    		
    		showIMEI2(win, ss_cnf);
#endif     
			break;

        default:
            TRACE_FUNCTION("ERROR in IMEI");
            showError(win, (T_MFW_SS_ERROR)ss_cnf->imei_info.error);
            break;
        }
        break;

            /* we must display the ussd info and then edit and send a user ussd */
    case E_SS_USSD_REQ:
	{
	
	//x0pleela 24 Aug, 2006  DR: OMAPS00083503
	//Checking dcs 
#ifdef MMI_LITE
	UBYTE alphabet;
	alphabet = ss_getAlphabetCb ((UBYTE) ss_cnf->ussd_info.dcs );

	//if it is UCS2 character then display a message for the user
	if( alphabet EQ SS_ALPHA_16_Bit)
	{
		showUSSDError(win);
	}
	else
	{
	
#endif
        switch(ss_cnf->ussd_info.error)
        {
        case MFW_SS_NO_ERROR:
            TRACE_FUNCTION( "Handling USSD_REQ no error" );
            showUSSDREQ(win, ss_cnf );
            break;

        default:
             TRACE_FUNCTION( "Handling USSD_REQ no error" );
            showError(win, (T_MFW_SS_ERROR)ss_cnf->ussd_info.error);
            break;
        }
#ifdef MMI_LITE
	}
#endif
    	}
        break;


    /* just display the ussd info */
    case E_SS_USSD_CNF:
        TRACE_FUNCTION(" E_SS_USSD_CNF");
        switch(ss_cnf->ussd_info.error)
        {
        
        case MFW_SS_NO_ERROR:
            TRACE_FUNCTION( "Handling USSD_CNF no error" );

#ifdef FF_CPHS_REL4  
	if(interrogate)
	{
	//	mfw_cphs_set_default_profile();
	//	mfw_cphs_set_reg_profile();
		interrogate = 0;
		TRACE_EVENT_P1("ussd string   %s", ss_cnf->ussd_info.ussd);
		mmi_msp_interrogate_done();
	}
	else
	if(msp_register)
	{
		msp_register = FALSE;
		TRACE_EVENT_P1("msp profile id registered %d", msp_reg_profid);
		mfw_cphs_set_reg_profile(msp_reg_profid);
		info_screen(win, TxtActivated, NULL, NULL);
		mmi_msp_interrogate_done();
	}
	else
	{
#endif
	showUSSDCNF(win, ss_cnf );
#ifdef FF_CPHS_REL4  
		}
#endif
            break;

        default:
            TRACE_FUNCTION("ERROR in USSD_CNF");
#ifdef FF_CPHS_REL4  
	if(interrogate)
	{
		mfw_cphs_set_default_profile(1);
		mfw_cphs_set_reg_profile(1);
		interrogate = 0;
		TRACE_EVENT_P1("ussd string   %s", ss_cnf->ussd_info.ussd);
		mmi_msp_interrogate_done();
	}
	if(msp_register)
	{
		msp_register = FALSE;
		TRACE_EVENT("msp profile id registeration failed");
	}
#endif				
            		showError(win, (T_MFW_SS_ERROR)ss_cnf->ussd_info.error);
            break;
        }
        break;
	case E_SS_GET_PW:
		if (ss_cnf->pw_cnf.ss_error == MFW_SS_NO_ERROR)
		{
            showOk(win);
            strncpy((char*)ss_data.edt_buf_cb_password,
            (char*)ss_cnf->pw_cnf.ss_new_pwd,CB_PSW_LEN);
		}
		else
		{
            showError(win, ss_cnf->pw_cnf.ss_error);
		}
	break;
	//CQ-16432 start
	case E_SS_SAT_CPWD:
		TRACE_EVENT("E_SS_SAT_CPWD");
		requestSatCBPassword();
	break;
	//CQ-16432 end
     default:
		return;
	}

}


/*******************************************************************************

 $Function:    	srv_check_CF_telephony_status

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

/*
** NDH : 28/08/2003 : I am modifying the functionality of this routine so that will simply return
** CF_ACTIVE_STATUS if there is an active Call Forwarding Service and CF_INACTIVE_STATUS
** otherwise. This function is currently used in only one place and this change would be in line
** with the required functionality at that point.
*/

static UBYTE srv_check_CF_telephony_status (T_MFW_SS_CF_CNF * in_list)
{
	UBYTE i;
	UBYTE retStat = CF_INACTIVE_STATUS;

       TRACE_EVENT("srv_check_CF_telephony_status");

	if (in_list->ss_error == MFW_SS_NO_ERROR)
	{
		if (in_list->ss_feature_count)
		{
			for (i=0;i<in_list->ss_feature_count;i++)
			{
				T_MFW_FEATURE * pFeature = &in_list->ss_feature_list[i];

				if (pFeature->ss_telecom_type == MFW_BST_TELE)
				{
					switch(pFeature->ss_telecom_service)
					{
						case MFW_TS_ALL_SPCH:
						case MFW_TS_TLPHNY:
						case MFW_TS_EMRGNCY:

		                                   TRACE_EVENT("Speech Tele Service");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= MFW_CFU_VOICE_ACTIVE;
							else
								ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_VOICE_ACTIVE);
							break;

						case MFW_TS_ALL_FAX:
						case MFW_TS_FAX3_ALT_SPCH:
						case MFW_TS_FAX3_AUTO:
						case MFW_TS_FAX4:

		                                   TRACE_EVENT("Fax Tele Service");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= MFW_CFU_FAX_ACTIVE;
							else
								ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_FAX_ACTIVE);
							break;

						case MFW_TS_ALL_DATA:

		                                   TRACE_EVENT("Data Tele Service");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= MFW_CFU_DATA_ACTIVE;
							else
								ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_DATA_ACTIVE);
							break;

						case MFW_TS_ALL_SMS:
						case MFW_TS_SMS_MO:
						case MFW_TS_SMS_MT:

		                                   TRACE_EVENT("SMS Tele Service");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= MFW_CFU_SMS_ACTIVE;
							else
								ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_SMS_ACTIVE);
							break;

						case MFW_TS_ALL_XCPT_SMS:

		                                   TRACE_EVENT("All Except SMS Tele Service");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= (MFW_CFU_VOICE_ACTIVE |
													    MFW_CFU_FAX_ACTIVE |
													    MFW_CFU_DATA_ACTIVE);
							else
								ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE -
														(MFW_CFU_VOICE_ACTIVE |
														 MFW_CFU_FAX_ACTIVE |
														 MFW_CFU_DATA_ACTIVE));
							break;

						case MFW_TS_ALL_TS:

		                                   TRACE_EVENT("All Tele Services");

							if (pFeature->ss_status & SS_STAT_ACTIVE)
								ss_data.cfu_services |= MFW_CFU_ALL_SERVICES_ACTIVE;
							else
								ss_data.cfu_services = 0;
							break;

						default:
							break;
					}
				}
				else if (pFeature->ss_telecom_type == MFW_BST_BEARER)
				{

		                       TRACE_EVENT("Bearer Service Type");

					if (pFeature->ss_status & SS_STAT_ACTIVE)
						ss_data.cfu_services |= MFW_CFU_DATA_ACTIVE;
					else
						ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_DATA_ACTIVE);
					break;
				}
				else
				{
					/* MFW_BST_UNKNOWN */

		                       TRACE_EVENT("Unknown Service Type");

					if (pFeature->ss_status & SS_STAT_ACTIVE)
						ss_data.cfu_services |= MFW_CFU_ALL_BASIC_SERVICES_ACTIVE;
					else
						ss_data.cfu_services = 0;
					break;
				}
				
			}
		}
		else if ((in_list->ss_category == SS_REGISTRATION) ||
			     (in_list->ss_category == SS_ACTIVATION))
		{
			ss_data.cfu_services |= MFW_CFU_ALL_BASIC_SERVICES_ACTIVE;
		}
		else if ((in_list->ss_category == SS_ERASURE) ||
			     (in_list->ss_category == SS_DEACTIVATION))
		{
			ss_data.cfu_services &= (MFW_CFU_ALL_SERVICES_ACTIVE - MFW_CFU_ALL_BASIC_SERVICES_ACTIVE);
		}
	}

       TRACE_EVENT_P1("cfu_services flag is : %8lx",  ss_data.cfu_services);

	if (ss_data.cfu_services)
		retStat = CF_ACTIVE_STATUS;

	return retStat;
}


/*******************************************************************************

 $Function:    	srv_check_CF_telephony_status

 $Description:	copies only the interesting parts of the call forwarding
            feature list.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static UBYTE srv_copy_feature_list_cf (T_ss_feature_cf * out_list, T_MFW_SS_CF_CNF * in_list)
{
  UBYTE found_entries = 0;
  UBYTE i, j;
 
  TRACE_EVENT_P1("ss_feature_count %d",in_list->ss_feature_count);

  if (in_list->ss_feature_count == 0)
  	out_list->ss[0].service = TxtCallDivert;

  for (i=0;i<in_list->ss_feature_count;i++)
  {	//MC got rid of pointer arithmetic
  	
      T_MFW_FEATURE * pFeature = &in_list->ss_feature_list[i]; //SPR#1081 - DS - Removed mem allocation. Uses assignment instead.
	T_ss_feature_cf_data * cf_data = &out_list->ss[found_entries];
    	UBYTE srvFound;
    
	TRACE_EVENT_P2("Telecom type:%d, service: %d ",pFeature->ss_telecom_type, pFeature->ss_telecom_service);		
    /*
     * first check teleservice
     */
    if (srv_get_service_type (&cf_data->service, pFeature->ss_telecom_type/*CFService*/,
                              pFeature->ss_telecom_service))
    {

	if (cf_data->service == TxtNull)
		cf_data->service = TxtCallDivert;

	/*
	** If there is more than one entry of a given type, ensure that an information
	** screen is displayed only once
	*/
	srvFound = FALSE;

	for (j=0; j<found_entries && !srvFound; j++)
	{
		if (cf_data->service == out_list->ss[j].service)
			srvFound = TRUE;
	}		

	if (!srvFound)
	{
	      /*
	       * second set status
	       */
	      srv_get_status (&cf_data->status, pFeature->ss_status);

		if ((ss_cnf->cf_cnf.ss_code==SS_CF_CFU)||(ss_cnf->cf_cnf.ss_code==SS_CF_ALL)){
			if (pFeature->ss_status & SS_STAT_ACTIVE)
			{
				iconsSetState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Setting the Current state of CF in Flash						
				FFS_flashData.CFState=TRUE;
			}
	        /* Marcus: Issue 1652: 03/02/2003: Start */
	        else if (pFeature->ss_status & SS_STAT_PROVISIONED)
			{
				iconsSetState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Setting the Current state of CF in Flash						
				FFS_flashData.CFState=TRUE;
		        }
	        /* Marcus: Issue 1652: 03/02/2003: End */
	        else
		        {
	            iconsDeleteState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Setting the Current state of CF in Flash						
				FFS_flashData.CFState=FALSE;
		        }
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Writng the Current state of CF into ffs						
			flash_write();
			
		}			

	      /*
	       * third set forwarded to number
	       * If the TON is International, add a '+' to the beginning of the number
	       */
	       if ((pFeature->ss_numbering_type == TON_International) &&
		    (pFeature->ss_forwarded_to_number[0] != '+'))
	       {
		      cf_data->forwarded_to_number[0] = '+';
		      srv_get_forwarded_to_number (&cf_data->forwarded_to_number[1],
										   (char *)pFeature->ss_forwarded_to_number,
										   MAX_SS_CF_NUM_LEN-1);
	       }
		else
		{
		      srv_get_forwarded_to_number ((char *)cf_data->forwarded_to_number,
					                                      (char *)pFeature->ss_forwarded_to_number,
					                                      MAX_SS_CF_NUM_LEN);
		}

		TRACE_EVENT_P1("ss_forwarded_to_number %s", (CHAR *)pFeature->ss_forwarded_to_number);
	      found_entries++;
	}
    }

  }

  /*
   * return the number of interesting entries
   */
  return found_entries;
}


/*******************************************************************************

 $Function:    	srv_get_forwarded_to_number

 $Description:	sets the forwarded to number if available.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static void srv_get_forwarded_to_number ( char * textString,
                                          char * forwarded_to_number,
                                          UBYTE max_chars)
{
  TRACE_FUNCTION("srv_get_forwarded_to_number");

  memset (textString, 0, max_chars);
  strncpy (textString, forwarded_to_number, max_chars-1);
}


/*******************************************************************************

 $Function:    	srv_get_service_type

 $Description:	checks if the service is interesting or not.
            
 $Returns:		

 $Arguments:	

 $History:

 //SPR#0776 - DS - Added default value for textId. Changed references from TxtNotImplemented
 				    to TxtNull.
 
*******************************************************************************/


static UBYTE srv_get_service_type ( USHORT * textId,
                                    UBYTE ss_telecom_type, UBYTE ss_telecom_service)
{
  TRACE_FUNCTION("srv_get_service_type");

  TRACE_EVENT_P1("ss_telecom_type: %x", ss_telecom_type);
  TRACE_EVENT_P1("ss_telecom_service: %x", ss_telecom_service);

  *textId = TxtNull; //Set to TxtNull in case no match found below.

  switch ( ss_telecom_type )
  {
    case MFW_BST_UNKNOWN:
      break;

    case MFW_BST_TELE:
      switch ( ss_telecom_service )
      {
        case MFW_TS_ALL_TS:
          *textId = TxtAllCalls;
          return TRUE;

        case MFW_TS_ALL_SPCH:       /* All speech transmission services */
        case MFW_TS_TLPHNY:         /* telephony    */
        case MFW_TS_PLMN1: /*MC, this is returned for auxiliary telephony status, 742*/
          *textId = TxtVoice;
          return TRUE;
	 
	 case MFW_TS_ALL_SMS:
	 case MFW_TS_SMS_MO:
	 case MFW_TS_SMS_MT:
	   *textId = TxtSMS;
	   return TRUE;

	 case MFW_TS_EMRGNCY:
	 case MFW_TS_ALL_XCPT_SMS:
        case MFW_TS_ALL_PSSS:     /* all PLMN specific TS           */
        case MFW_TS_PLMN2:          /* PLMN specific TS 2             */
        case MFW_TS_PLMN3:          /* PLMN specific TS 3             */
        case MFW_TS_PLMN4:          /* PLMN specific TS 4             */
        case MFW_TS_PLMN5:          /* PLMN specific TS 5             */
        case MFW_TS_PLMN6:          /* PLMN specific TS 6             */
        case MFW_TS_PLMN7:          /* PLMN specific TS 7             */
        case MFW_TS_PLMN8:          /* PLMN specific TS 8             */
        case MFW_TS_PLMN9:          /* PLMN specific TS 9             */
        case MFW_TS_PLMNA:          /* PLMN specific TS A             */
        case MFW_TS_PLMNB:          /* PLMN specific TS B             */
        case MFW_TS_PLMNC:          /* PLMN specific TS C             */
        case MFW_TS_PLMND:          /* PLMN specific TS D             */
        case MFW_TS_PLMNE:          /* PLMN specific TS E             */
        case MFW_TS_PLMNF:          /* PLMN specific TS F             */
          *textId = TxtNull;
          return TRUE;

        case MFW_TS_ALL_DATA:
          *textId = TxtData;
          return TRUE;

        case MFW_TS_ALL_FAX:
	 case MFW_TS_FAX3_ALT_SPCH:
	 case MFW_TS_FAX3_AUTO:
	 case MFW_TS_FAX4:
          *textId = TxtFax;
          return TRUE;
      }

      break;

    case MFW_BST_BEARER:
      switch ( ss_telecom_service )
      {
        case MFW_BS_ALL_BS:
          *textId = TxtData; //SPR#933 - DS - Changed TxtNull to TxtData.
          return TRUE;

		case MFW_BS_ALL_DATA_CDA: //SPR#933 - DS - Added so MMI displays correct info to user.
        	*textId = TxtData;
        	return TRUE;

        default:
          break;
      }
      break;
  }

  return FALSE;
}

/*******************************************************************************

 $Function:    	srv_get_status

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void srv_get_status (USHORT *textId, UBYTE pStatus)
{
  TRACE_FUNCTION("srv_get_status");
  TRACE_EVENT_P1("Call Forwarding status: %d", pStatus);

  if  (pStatus & SS_STAT_ACTIVE)    // handles also the case of invalid status
    *textId = TxtActivated;
  /* Marcus: Issue 1652: 03/02/2003: Removed NDH 10/06/03 */
  else
    *textId = TxtDeActivated;
}


/*******************************************************************************

 $Function:    	srv_copy_feature_list_cw

 $Description:	 copies only the interesting parts of the call waiting
            service list.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static UBYTE srv_copy_feature_list_cw (T_ss_feature_cw * out_list, T_MFW_SS_CW_CNF * in_list)
{
  UBYTE found_entries = 0;
  UBYTE i, j;

  if (in_list->ss_service_count == 0)
  	out_list->ss[0].service = TxtCallWaiting;

  for (i=0;i<in_list->ss_service_count;i++)
  {
    T_MFW_SRV_GROUP      * pFeature = &in_list->ss_service_list[i];
    T_ss_feature_cw_data * cw_data = &out_list->ss[found_entries];
    UBYTE srvFound;
    /*
     * first check teleservice
     */
    if (srv_get_service_type (&cw_data->service, pFeature->ss_telecom_type,
                              pFeature->ss_telecom_service))
    {

	if (cw_data->service == TxtNull)
		cw_data->service = TxtCallWaiting;

	/*
	** If there is more than one entry of a given type, ensure that an information
	** screen is displayed only once
	*/
	srvFound = FALSE;

	for (j=0; j<found_entries && !srvFound; j++)
	{
		if (cw_data->service == out_list->ss[j].service)
			srvFound = TRUE;
	}		

	if (!srvFound)
	{
	      /*
	       * second set status
	       */
	      srv_get_status (&out_list->status, in_list->ss_status);
	      found_entries++;
	}
    }
  }

  /*
   * return the number of interesting entries
   */
  return found_entries;
}

/*******************************************************************************

 $Function:    	srv_copy_feature_list_cb

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static UBYTE srv_copy_feature_list_cb (T_ss_feature_cb * out_list, T_MFW_SS_CB_CNF * in_list)
{
  UBYTE found_entries = 0;
  UBYTE i, j;

  USHORT textId = 0;   // RAVI

  /*
   * SPR #1878 NDH - Determine which text is to be displayed depending on what request
   *                was initially sent.
   */
  switch(in_list->ss_category)
  {
  	case SS_REGISTRATION:
  	case SS_ACTIVATION:
  	case SS_INTERROGATION:
      /*
       * Interrogation returns only those services which are active
       */
  		textId = TxtActivated;
  		break;

  	case SS_ERASURE:
  	case SS_DEACTIVATION:
  		textId = TxtDeActivated;
  		break;
  }

  if (in_list->ss_telecom_count == 0)
  	out_list->ss[0].service = TxtCallBarringAll;

  for (i=0;i<in_list->ss_telecom_count;i++)
  {
    T_MFW_TELECOM * pFeature = &in_list->ss_telecom_list[i];
    T_ss_feature_cb_data * cb_data = &out_list->ss[found_entries];
    UBYTE srvFound;
    /*
     * first check teleservice
     */
    if (srv_get_service_type (&cb_data->service, pFeature->ss_telecom_type,
                              pFeature->ss_telecom_service))
    {
	if (cb_data->service == TxtNull)
		cb_data->service = TxtCallBarringAll;

      cb_data->status = textId;

	/*
	** If there is more than one entry of a given type, ensure that an information
	** screen is displayed only once
	*/
	srvFound = FALSE;

	for (j=0; j<found_entries && !srvFound; j++)
	{
		if (cb_data->service == out_list->ss[j].service)
			srvFound = TRUE;
	}		

	if (!srvFound)
		found_entries++;
    }
  }

  /*
   * return the number of interesting entries
   */
  return found_entries;
}



/*******************************************************************************

 $Function:    	services_info_cb

 $Description:	Callback function from information dialog.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static int services_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reason)
{

    T_DISPLAY_DATA   display_info; 	// Jun 16, 2005 REF: CRR 31543  x0021334
//    UBYTE PWD;  // RAVI
    T_MFW_SS_RETURN sat_result_hangup; 	// Jun 16, 2005 REF: CRR 31543  x0021334

     //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
     //Bug : Unable to compose a one or two digit number
     //Fix : This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled. 
     //The read only editor with the service provider message that comes up when a one or two digit is dialled
     //was not handled properly. So altered the editor handling to cater for the long message.
    T_MFW_WIN 	*win_data;
    T_ss 		  	*data = NULL;  // RAVI - Assigned NULL.

    TRACE_EVENT_P1("Service Info CB %d", win);

  ss_data.info_wait = 0;

    //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
    //Bug : Unable to compose a one or two digit number
    //Fix : This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled. 
    //The read only editor with the service provider message that comes up when a one or two digit is dialled
    //was not handled properly. So altered the editor handling to cater for the long message.	
    if(win)
    {
         win_data   = ((T_MFW_HDR *)win)->data;
         if(win_data)
	      data         = (T_ss * )win_data->user;
     }  

  /*
   * Who has initiated the information screen
   */
  switch (identifier)
  {
    case SS_WAIT:

    	break;
    case E_SS_IMEI:/* present IMEI */
    case ServicesOk:
    case ServicesFailed:
    case ServicesCheckCLI_OK:
    case ServicesCheckCW_OK:
    case ServicesIMEI_OK:
    case ServicesError:
   	case ServicesUSSD_CNF_OK:
#if 0
    case ServicesUSSDAlreadyFull:   /* already full message screen */
#endif
    case ServicesCFU_OK:
    case ServicesCB_OK:

    case E_SS_NOTIFY:
    case E_SS_CLI_CNF:
    case E_SS_USSD_CNF:
      switch (reason)
      {
        case INFO_TIMEOUT:
          /* info dialog timed out, where to now? */
          /* no break; */
        case INFO_KCD_HUP:
          /* clear key in info dialog, where to now? */
          /* no break;*/
        case INFO_KCD_LEFT:
          /* left soft key in info dialog, where to now? */
          /* no break;*/
        case INFO_KCD_RIGHT:
          /* right soft key in info dialog, where to now? */
	     //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
	     //Bug : Unable to compose a one or two digit number
    	    //Fix :   This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled.
    	    //The read only editor with the service provider message that comes up when a one or two digit is dialled
           //was not handled properly. So altered the editor handling to cater for the long message.
	    if(data)
	    {
	          if(data->info_win)
		   {
		        AUI_edit_Destroy(data->info_win);
		        data->info_win=NULL;
		    }
	     }			  
            services_destroy(win);
          break;
      }
	// May 12, 2005 REF: CRR 30892  x0021334	
	// Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification
      ss_data.info_win = NULL;
      break;

    case E_SS_CB_CNF:
      {
        T_MFW_WIN * win_data;
        T_ss      * data;

	if(win != NULL)  /* x0039928 - Lint warning removal */
       {
       	win_data = ((T_MFW_HDR *)win)->data;
        	data     = (T_ss *)win_data->user;
	

        switch (reason)
        {
          case INFO_TIMEOUT:
	   //Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Timer is introduced for the information dialog, we are handling the time_out event
          case INFO_KCD_HUP:
          case INFO_KCD_LEFT:
            /*
             * stop the display of the results
             */
            if (data->mfw_ss_cb)
            {
              FREE_MEMORY ((void *)data->mfw_ss_cb, sizeof (T_ss_feature_cb));
              data->mfw_ss_cb = NULL;
              data->current_feature = 0;

              TRACE_EVENT_P2("SS_data.win %d %d",ss_data.win, data);
            }
            break;

          case INFO_KCD_RIGHT:
            if ((data->mfw_ss_cb) && (data->max_feature > 1))
            {
              if (data->max_feature > data->current_feature + 1)
                data->current_feature++;
              else
              	data->current_feature = 0;
              
              showCBStatus (win, data);
		//Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Restart the timer if the key was pressed. We handle only right key since left key destroys
		//the information dialog
	       InfoDialogTimRestart(win);
            }
	
            break;

          default:
            break;
        }
	// May 12, 2005 REF: CRR 30892  x0021334	
	// Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification
	 ss_data.info_win = NULL;
	}
      }
	break;

    case E_SS_CW_CNF:
      {
        T_MFW_WIN * win_data;
        T_ss      * data;

	if(win != NULL)   /* x0039928 - Lint warning removal */
	{
		win_data = ((T_MFW_HDR *)win)->data;
        	data     = (T_ss *)win_data->user;
	

        switch (reason)
        {
          case INFO_TIMEOUT:
	   //Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Timer is introduced for the information dialog, we are handling the time_out event
          case INFO_KCD_HUP:
          case INFO_KCD_LEFT:
            /*
             * stop the display of the results
             */
            if (data->mfw_ss_cw)
            {
              FREE_MEMORY ((void *)data->mfw_ss_cw, sizeof (T_ss_feature_cw));
              data->mfw_ss_cw = NULL;
              data->current_feature = 0;
            }
            break;

          case INFO_KCD_RIGHT:
            if ((data->mfw_ss_cw) && (data->max_feature > 1))
            {
              if (data->max_feature > data->current_feature + 1)
                data->current_feature++;
              else
              	data->current_feature = 0;
              
              showCWStatus (win, data);
		//Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Restart the timer if the key was pressed. We handle only right key since left key destroys
		//the information dialog
		 InfoDialogTimRestart(win);
		
            }
            break;

          default:
            break;
        }
	// May 12, 2005 REF: CRR 30892  x0021334	
	// Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification	
	 ss_data.info_win = NULL;
      }
	}
	break;
    case E_SS_CF_CNF:
      {
        T_MFW_WIN * win_data;
        T_ss      * data;

	 if(win != NULL)
	 {
		win_data = ((T_MFW_HDR *)win)->data;
        	data     = (T_ss *)win_data->user;
	 

        switch (reason)
        {
          case INFO_TIMEOUT:
            //Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Timer is introduced for the information dialog, we are handling the time_out event
          case INFO_KCD_HUP:
          case INFO_KCD_LEFT:
            /*
             * stop the display of the results
             */
            if (data->mfw_ss_cf)
            {
              FREE_MEMORY ((void *)data->mfw_ss_cf, sizeof (T_ss_feature_cf));
              data->mfw_ss_cf = NULL;
              data->current_feature = 0;
            }
            break;

          case INFO_KCD_RIGHT:
            if ((data->mfw_ss_cf) && (data->max_feature > 1))
            {
              if (data->max_feature > data->current_feature + 1)
                data->current_feature++;
              else
              	data->current_feature = 0;
              
              showCFStatus (win, data);
		//Jun 04,2004 CRR:13601 xpradipg - SASKEN
		//Restart the timer if the key was pressed. We handle only right key since left key destroys
		//the information dialog
              InfoDialogTimRestart(win);
            }
            break;

          default:
            break;
        }
	// May 12, 2005 REF: CRR 30892  x0021334	
	// Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification
	 ss_data.info_win = NULL;
	}
        break;
      }
    case ServicesUSSD_REQ_OK:
      switch (reason)
      {
        case INFO_KCD_HUP:
		// Jun 16, 2005 REF: CRR 31543  x0021334
		// Hangup key is pressed. End the USSD session.
		sat_result_hangup = satHangup (CMD_SRC_LCL);
		if (sat_result_hangup == MFW_SS_OK)
		{
			dlg_initDisplayData_TextId(&display_info,  NULL, NULL, TxtCancelled, TxtNull, COLOUR_STATUS);
			dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT | KEY_RIGHT);
			info_dialog (NULL, &display_info);	
		}
		else
		{
			TRACE_ERROR("Error in SAT USSD abort");
		}
         // no break;
        case INFO_TIMEOUT:
          /* info dialog timed out, where to now? */
         // no break;
        case INFO_KCD_RIGHT:
          /* right soft key in info dialog, where to now?
             all above mean cancelled so go back to whatever was before (idle?)*/
            services_destroy(win);
            break;

        case INFO_KCD_LEFT:
          /* left soft key in info dialog, where to now?
            This means confirm, so now go to USSD entry screen */
             SEND_EVENT(ss_data.win,ServicesUSSDEnter,0,0);
             break;
        default:
            break;
      }
	// May 12, 2005 REF: CRR 30892  x0021334	
	// Set ss_data.info_win to NULL to pass the check in services_ss_cb() for notification	
	ss_data.info_win = NULL;
      break;

	case ServicesUSSDEnter:
			{
			T_MFW_SS_PARA ussd_data;

		    ussd_data.ussd_info.len = strlen((char*)ss_data.edt_buf_number);
  		 	ussd_data.ussd_info.dcs = (T_MFW_DCS)ss_data.dcs;  /* x0020906 - 24-08-2006 */
  			ussd_data.ussd_info.error = MFW_SS_NO_ERROR;

			memcpy(ussd_data.ussd_info.ussd, ss_data.edt_buf_number, ussd_data.ussd_info.len);
			sendUSSD (&ussd_data);
		}
		break;

  }
  return 1;
}


/*******************************************************************************

 $Function:    	servicesNumbers

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesNumbers(MfwMnu* m, MfwMnuItem* i){
    CFAction = CFActivate;
	servicesDoOtherNumber();	

	// ADDED BY RAVI - 28-11-2005
	return 1;
	//END RAVI - 28-11-2005
}


/*******************************************************************************

 $Function:    	servicesInfraRed

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesInfraRed(void){return 11;}


/*******************************************************************************

 $Function:    	servicesExec

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void servicesExec (int reason, MmiState next)
{
//    UBYTE PWD;   // RAVI
    T_MFW_HND win = mfw_parent(mfw_header());
   // T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI


   /* nextState = next; x0039928 - Lint warning removal */


	if (!winIsFocussed(win))
		lastFocus = winFocus(win);

	servicesState = reason;						/* save reason now !! */

	switch (reason)
    {
	case ServicesIdle:							/* giving back control to last module*/
		winFocus(lastFocus);
		winShow(lastFocus);
		lastFocus = 0;
		return;									/* return from module */

	case ServicesOk:
		TRACE_FUNCTION("Service OK");
        winShow(win);
        timStart(times);  /* start timer to go back in the submenu */
		break;

	case ServicesFailed:
		TRACE_FUNCTION("Service Failed");
        winShow(win);
        timStart(times);  /* start timer to go back in the submenu */
		break;

   	case ServicesError:
		TRACE_FUNCTION("Service Error");
        winShow(win);
        timStart(times);  /* start timer to go back in the submenu */
		break;

    case ServicesExecCF:          /* all Call forwarding from menu */
    case ServicesActCF:             /* show "Please Wait" screen/animation */
    case ServicesDeactCF:
    case ServicesCheckCF:
	case ServicesActCW:
	case ServicesDeactCW:
	case ServicesCheckCW:
	case ServicesActCLIR:
	case ServicesDeactCLIR:
    case ServicesCheckCLIR:
        winShow(win);
        timStart(times);  /* start timer to go back in the submenu */
		break;

    case ServicesCheckCW_OK:
        TRACE_FUNCTION(" ServicesCheckCW_OK");
        winShow(win);
        timStart(times);  /* start timer to go back in the submenu */
		break;

	case ServicesCheckCLI_OK:
		TRACE_FUNCTION("ServicesCheckCLI_OK");
        winShow(win);
        timStart(times);
		break;

    case ServicesIMEI_OK:
		TRACE_FUNCTION("ServicesIMEI_OK");
        winShow(win);
        timStart(times);
        break;
    case ServicesResShow:
		TRACE_FUNCTION("ServicesResShow");
        winShow(win);
        timStart(times);
        break;

    case ServicesCB_OK:
		TRACE_FUNCTION("ServicesCB_OK");
        lastFocus = winFocus(win);
        winShow(win);
        timStart(times);
        break;

    case ServicesCFU_OK:
		TRACE_FUNCTION("ServicesCFUShow");
        lastFocus = winFocus(win);
        winShow(win);
        timStart(times);
        break;

    case ServicesDoCallDivert:
        servicesSendCallDivert(0);
        break;

    case PIN2_ABORT:
          TRACE_FUNCTION("ServicesPin2_Abort");
        break;
    default:
		break;
    }
}

#ifdef MMI_LITE
/*******************************************************************************

 $Function:    	showUSSDError

 $Description:	This function displays a dialog saying USSD message received, Font not supported 
 			for UCS2 characters
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

//x0pleela 24 Aug, 2004  DR: OMAPS00083503

static void showUSSDError (T_MFW_HND win)
{

    T_DISPLAY_DATA   display_info;

    TRACE_FUNCTION("showUSSDError");
	

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtUSSDMsgRxd,  TxtFontNotSupported, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT );
    display_info.Identifier   = ServicesError;

      /*
       * Call Info Screen
       */

    ss_data.info_win = info_dialog (ss_data.win, &display_info);

}
#endif

/*******************************************************************************

 $Function:    	showWait

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showWait (T_MFW_HND win)
{

    T_DISPLAY_DATA   display_info;

		TRACE_FUNCTION("showWait");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtRequesting,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT );
    display_info.Identifier   = SS_WAIT;

      /*
       * Call Info Screen
       */

    ss_data.info_wait = info_dialog (win, &display_info);
}


/*******************************************************************************

 $Function:    	showOk

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showOk (T_MFW_HND win)
{

    T_DISPLAY_DATA   display_info;

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtDone,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT );
    display_info.Identifier   = ServicesOk;

      /*
       * Call Info Screen
       */

    ss_data.info_win = info_dialog (ss_data.win, &display_info);

}

/*******************************************************************************

 $Function:    	showFailed

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showFailed (T_MFW_HND win)
{

    T_DISPLAY_DATA   display_info;

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtFailed,  TxtNull, COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT );
    display_info.Identifier   = ServicesFailed;

      /*
       * Call Info Screen
       */
    ss_data.info_win = info_dialog (ss_data.win, &display_info);
}

/*******************************************************************************

 $Function:    	showError

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showError (T_MFW_HND win, T_MFW_SS_ERROR ss_error)
{

    T_DISPLAY_DATA   display_info;

    TRACE_FUNCTION("showError");

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtFailed,  serviceGetError(ss_error), COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT );
    display_info.Identifier   = ServicesError;

      /*
       * Call Info Screen
       */

    ss_data.info_win = info_dialog (ss_data.win, &display_info);

}


/*******************************************************************************

 $Function:    	showSSNotify

 $Description:	Show result from E_SS_NOTIFY being returned. Short ss-related
            notification window.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showSSNotify (T_MFW_HND win, T_MFW_SS_NOTIFY  * ss_notify)
{

    T_DISPLAY_DATA   display_info;
	USHORT TxtId1,TxtId2;

    TRACE_FUNCTION("++ showSSNotify");

    TxtId1	= TxtNull;
    TxtId2	= TxtNull;

    switch (ss_notify->code)
    {
    case MFW_SS_MO_unconFORWARD:	/* CFU activated */
		{
			TRACE_EVENT("showSSNotify() MFW_SS_MO_unconFORWARD");

      		/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
                  Description : CPHS Call Forwarding feature implementation
                  Solution     : As part of the implemention, cphs_set_divert_status() is called whenever the network notifies
                                    about unconditional forward*/
                                    
			#ifdef FF_CPHS
					switch (CFService)
					{
						case (CFVoice):	
						    /*MC SPR 742, handle line 2, auxiliary telephony*/
							    /*MC SPR 742, handle line 2, auxiliary telephony*/
								#ifdef FF_CPHS_REL4
								if(isCFISFilePresent() )
									{	/*call_forwarding_status_rel4.voice updated*/
										if(call_forwarding_status_rel4.voice != MFW_CFLAG_SET || 
											memcmp(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN)!=0)
											{
												call_forwarding_status_rel4.voice=MFW_CFLAG_SET;
												memcpy(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN);
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
										break;
									}
								
								#endif
		    				case CFAuxVoice:

							if (GetAlsLine() == MFW_SERV_LINE2)
								{
								/*30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
								   check to find whether we are doing unnecessary sim write*/
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_SET)  
									cphs_set_divert_status(MFW_CFLAG_NotPresent, MFW_CFLAG_SET, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
						
							else
								{
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_SET, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent, MFW_CFLAG_NotPresent);
								}
						break;
						case (CFData):
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
							if(GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET)
								cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_SET);
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.data!= MFW_CFLAG_SET)
											{
												call_forwarding_status_rel4.data=MFW_CFLAG_SET;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
							break;
						case (CFFax):
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
							if(GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
								cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_NotPresent);
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.fax!= MFW_CFLAG_SET)
											{
												call_forwarding_status_rel4.fax=MFW_CFLAG_SET;
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
							break;
						case (CFAll):
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
							if(!isCFISFilePresent() )
								{
							#endif
							if (GetAlsLine() == MFW_SERV_LINE2)
								{
								if(GetCphsDivertStatus(MFW_SERV_LINE2)!=MFW_CFLAG_SET || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_SET,MFW_CFLAG_SET);
								}
							else
								{
								if(GetCphsDivertStatus(MFW_SERV_LINE1)!=MFW_CFLAG_SET || 
								   GetCphsDivertStatus(MFW_SERV_DATA)!=MFW_CFLAG_SET ||
								   GetCphsDivertStatus(MFW_SERV_FAX)!=MFW_CFLAG_SET)
									cphs_set_divert_status(MFW_CFLAG_SET,MFW_CFLAG_NotPresent,MFW_CFLAG_SET,MFW_CFLAG_SET);
								}
							/*a0393213 cphs rel4*/
							#ifdef FF_CPHS_REL4
								}
							else
								{
										if(call_forwarding_status_rel4.voice!= MFW_CFLAG_SET ||
										   call_forwarding_status_rel4.fax!= MFW_CFLAG_SET ||		
										   call_forwarding_status_rel4.data!= MFW_CFLAG_SET  || 
											memcmp(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN)!=0)
											{
												call_forwarding_status_rel4.voice= MFW_CFLAG_SET;
										  	 	call_forwarding_status_rel4.fax= MFW_CFLAG_SET;		
										   		call_forwarding_status_rel4.data= MFW_CFLAG_SET;
												memcpy(call_forwarding_status_rel4.voice_dialling_number,ss_data.edt_buf_number,PHB_MAX_LEN);
										
												cphs_rel4_set_divert_status(	call_forwarding_status_rel4.voice, 
																			call_forwarding_status_rel4.fax,
																			call_forwarding_status_rel4.data,
																			call_forwarding_status_rel4.voice_dialling_number);									
											}
								}
							#endif
						break;
					}

			#else
				 iconsSetState(iconIdCallFor);
		//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
		//              Storing the Current state of CF in Flash						
					FFS_flashData.CFState=TRUE;
					 flash_write();
			 #endif
    		}
		 
								/* Activate the CFU icon */
		/* break; */ /***** NO BREAK ******/
	case MFW_SS_MO_conFORWARD:		/* Conditional CF active */
        TxtId1      = TxtForward;
        TxtId2      = TxtActivated;
		break;

    case MFW_SS_MT_FORWARDED:			/* incoming call fowarded           */
	//x0pleela 27 Feb, 2007  ER: OMAPS00113891
	TRACE_EVENT("showSSNotify: MFW_SS_MT_FORWARDED");

	//set TextId of call data accordingly
#ifdef MMI_TTY_ENABLED
    		if (call_tty_get())
	   		call_data.inc_call_data.TextId   = TxtFwdIncomingTTYCall;
		else
			call_data.inc_call_data.TextId       = TxtFwdIncomingCall;	
#else  /* MMI_TTY_ENABLED */
	call_data.inc_call_data.TextId  = TxtFwdIncomingCall;
#endif /* MMI_TTY_ENABLED */			

	//if incoming call window is not NULL, then destroy the same
 	if( call_data.win_incoming != NULL )
		dialog_info_destroy(call_data.win_incoming);
	
	//call the incoming call dialog with new strings based on call forward condition
	 call_data.win_incoming = info_dialog (call_data.win, &call_data.inc_call_data);
	 
		break;

	//x0pleela 22 Feb, 2007  ER: OMAPS00113891
	//Message displayed to the subscriber who has forwarded the call
    case MFW_SS_MT_CALL_FORWARDED:			/* incoming call fowarded           */
        TxtId1      = TxtIncomingCall;
        TxtId2      = TxtForwarded;
		break;

    case MFW_SS_MO_FORWARDED:       /* our call is being forwarded */
        TxtId1      = TxtSoftCall;
        TxtId2      = TxtForwarded;

        break;

    case MFW_SS_MO_WAITING:     /* outgoing call is waiting signal received */
        TxtId1      = TxtSoftCall;
        TxtId2      = TxtWaiting;
        break;

    case MFW_SS_MO_moBARRED:  /* outgoing calls barred */
        TxtId1      = TxtOutgoingCalls;
        TxtId2      = TxtBarred;
        break;

    case MFW_SS_MO_mtBARRED:  /* incoming calls barred */
        TxtId1      = TxtIncomingCall;
        TxtId2      = TxtBarred;
        break;

    case MFW_SS_MO_DEFLECTED:  /* incoming call diverted  */
        TxtId1      = TxtIncomingCall;
        TxtId2      = TxtCallDivert;
        break;

    default:
        break;
    }

    if (TxtId1 != TxtNull )
    {

		dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtId1,  TxtId2, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, THREE_SECS, KEY_CLEAR| KEY_LEFT );
        display_info.Identifier   = E_SS_NOTIFY;
        
          /*
           * Call Info Screen
           */

        ss_data.info_win = info_dialog (ss_data.win, &display_info);
    }
}

/*******************************************************************************

 $Function:    	srv_display_status

 $Description:	Indicate a sensefull response to the user if only the .
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void srv_display_status (UBYTE ss_status)
{
  T_DISPLAY_DATA   display_info;
	USHORT TxtId2;
  TRACE_FUNCTION("srv_display_status()");


    //GW Swapped text strings over
    if (ss_status & SS_STAT_ACTIVE)
		TxtId2      = TxtActivated;
    /* Marcus: Issue 1652: 03/02/2003: Removed NDH 10/06/03 */
    else
        TxtId2      = TxtDeActivated;

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtNull,  TxtId2, COLOUR_STATUS);
	//Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//Timer value changed from FOREVER to FIVE_SECS
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT );
    display_info.Identifier   = E_SS_CB_CNF;  // can be used also for CF and CW

    /*
     * Call Info Screen
     */

    ss_data.info_win = info_dialog (ss_data.win, &display_info);
  }

/*******************************************************************************

 $Function:    	ysrv_empty_list_result

 $Description:	Indicate a sensefull response to the user if an
            activation / deactivation or interrogation leads to an
            empty list.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void srv_empty_list_result (UBYTE ss_category, USHORT event, UBYTE ss_status, UBYTE ss_code) /* Marcus: Issue 1652: 03/02/2002 */
{
  T_DISPLAY_DATA   display_info;
	USHORT TxtId1;
	USHORT TxtId2 = TxtNull;
  TRACE_FUNCTION("srv_empty_list_result");

  switch (event)
  {
  	case E_SS_CB_CNF:
	  	TxtId1 = TxtCallBarringAll;
		break;
		
  	case E_SS_CW_CNF:
	  	TxtId1 = TxtCallWaiting;
		break;
		
  	case E_SS_CF_CNF:
	  	TxtId1 = TxtCallDivert;
		break;

	default:
		TxtId1 = TxtNull;
  }

  /* Marcus: Issue 1652: 03/02/2002: Start */
  TRACE_EVENT_P3("ss_category %d, event %d, ss_status %d", ss_category, event, ss_status);
  switch (ss_category)
  {
    case SS_ACTIVATION:
    case SS_REGISTRATION:
      TRACE_FUNCTION ("ACTIVATED !");
      TxtId2      = TxtActivated;
      break;
    /* Marcus: Issue 1588: 23/01/2003: Start */
    case SS_INTERROGATION:
      if ((event == E_SS_CW_CNF) || (event == E_SS_CF_CNF))
      {
          if (ss_status & SS_STAT_ACTIVE)
          {
            TRACE_EVENT ("INTERROGATION:ACTIVATED !");
            TxtId2      = TxtActivated;
          }
          else  /* Marcus: Issue 1651: 30/01/2003: Removed NDH 10/06/03 */
          {
            TRACE_EVENT ("INTERROGATION:DEACTIVATED !");
            TxtId2      = TxtDeActivated;
          }
      }
      else
      {
          TRACE_EVENT ("DEACTIVATED !");
          TxtId2      = TxtDeActivated;
      }
      break;

    case SS_ERASURE:
    case SS_DEACTIVATION:
      TRACE_FUNCTION ("DEACTIVATED !");
      TxtId2      = TxtDeActivated;
      break;
    default:
      TxtId2      = TxtNull;
      break;
  }
  /* Marcus: Issue 1652: 03/02/2002: End */

  if (TxtId2 != TxtNull )
  {
	/*CQ10690 - API - 23/06/03 - Set the icon state to show on this return */
  	if ( (TxtId2 == TxtActivated) && 
  	( (ss_code == SS_CF_CFU) || (ss_code == SS_CF_ALL) ) )
  	{
      		iconsSetState(iconIdCallFor);
//  Jun 23, 2004        REF: CRR MMI-SPR-13599  Rashmi C N(Sasken) 
//              Storing the Current state of CF in Flash						
		FFS_flashData.CFState=TRUE;
		 flash_write();
  	}
	/*CQ10690 - END*/
	if(event==E_SS_CF_CNF)
	{
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
//	We are now passing the type of CF as the first stringID for the dialog info screen 
// serviceGetType(ss_Code) this function would return the string id for the ss_code being passed

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, serviceGetType((T_MFW_SS_CODES)ss_code),  TxtId2, COLOUR_STATUS);
	}
	else
	{
	        dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtId1,  TxtId2, COLOUR_STATUS);
        }
	//Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//Timer value changed from FOREVER to FIVE_SECS
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT );
       display_info.Identifier   = E_SS_CB_CNF;  // can be used also for CF and CW

    /*
     * Call Info Screen
     */

    ss_data.info_win = info_dialog (ss_data.win, &display_info);
  }
}


/*******************************************************************************

 $Function:    showCWState

 $Description:	display CW state
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void showCWStatus (T_MFW_HND win , T_ss * data)

{
	T_DISPLAY_DATA   display_info;
  	T_ss_feature_cw_data * pFeature;

  	TRACE_FUNCTION("showCWStatus");

	//MC got rid of dodgy pointer arithmetic
  pFeature = &data->mfw_ss_cw->ss[data->current_feature];

  if (data->max_feature == 1)
  {
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, pFeature->service,  data->mfw_ss_cw->status, COLOUR_STATUS);
	 //Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//Timer value changed from FOREVER to FIVE_SECS
	  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT);
  }
  else
  {
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtSoftNext, pFeature->service,  data->mfw_ss_cw->status, COLOUR_STATUS);
	  //Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//Timer value changed from FOREVER to FIVE_SECS
	  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT | KEY_RIGHT );
  }

  display_info.Identifier   = E_SS_CW_CNF; 

  /*
   * Call Info Screen
   */

  ss_data.info_win = info_dialog (ss_data.win, &display_info);

  if (ss_data.info_win EQ NULL)
  {
    TRACE_FUNCTION("NOT ENOUGH RESOURCES TO CREATE EXT INFO DIALOG");
  }
}

/*******************************************************************************

 $Function:    	showCLIStatus 

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void  showCLIStatus(T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf)
{

	T_DISPLAY_DATA   display_info;

	TRACE_FUNCTION("showCLIStatus()");

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, serviceGetType(ss_cnf->cli_cnf.ss_code), TxtNull , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_CLEAR | KEY_LEFT );
	display_info.Identifier   = ServicesCheckCLI_OK;

	if (ss_cnf->cli_cnf.ss_status == SS_STAT_UNKNOWN)
	{
		// Unknown status. By defaut say that the id is sent
		display_info.TextId2 = TxtActivated;
	}
	else if (ss_cnf->cli_cnf.ss_status & SS_STAT_ACTIVE)
		{
			switch (ss_cnf->cli_cnf.ss_clir_option)
			{
				// By defaut the caller id is not sent
				case MFW_CL_TEMP_DEF_REST:
					switch (CFAction)
					{
						// ActivateCLIR, so deactivate the sending of caller id
						case CFActivate:
							// By defaut the caller id is not sent
							display_info.TextId2 = TxtActivated;
							// No CLIR suppress or invoke
							cm_mo_prefix(0x04);
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
							FFS_flashData.CLIR=TRUE;
  							flash_write();
							break;						
						// Deactivate CLIR, so activate the sending of caller id
						case CFDeactivate:
							// By defaut the caller id is not sent
							display_info.TextId2 = TxtDeActivated;
							// CLIR suppress needed
							cm_mo_prefix(0x08);
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
							FFS_flashData.CLIR=FALSE;
  							flash_write();
							break;
						case CFCheck:
						default:
							/* // Check if CLIR suppress is activated: caller id sent
							if ((cm_mo_prefix(0) & 0x0c) == 0x08)
								display_info.TextId2 = TxtDeActivated;
							// By defaut the caller id is not sent
							else
								display_info.TextId2 = TxtActivated;*/
							// Check if CLIR suppress is activated: caller id sent
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Using the CLIR status maintained in the flash, when the status is checked
							if (!FFS_flashData.CLIR)
							{
								display_info.TextId2 = TxtDeActivated;
							}
							// By defaut the caller id is not sent
							else
							{
								display_info.TextId2 = TxtActivated;
							}
								
							break;
					}
					break;
				// By defaut the caller id is sent
				case MFW_CL_TEMP_DEF_ALLOWED:
					switch (CFAction)
					{
						// ActivateCLIR, so deactivate the sending of caller id
						case CFActivate:
							// By defaut the caller id is sent
							display_info.TextId2 =TxtActivated;
							// CLIR invoke needed
							cm_mo_prefix(0x0c);
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
							FFS_flashData.CLIR=TRUE;
  							flash_write();
							break;
						// Deactivate CLIR, so activate the sending of caller id
						case CFDeactivate:
							// By defaut the caller id is sent
							display_info.TextId2 = TxtDeActivated;
							// No CLIR suppress or invoke
							cm_mo_prefix(0x04);
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
							FFS_flashData.CLIR=FALSE;
 	 						flash_write();
							break;
						case CFCheck:
						default:
							/* // Check if CLIR invoke is activated: caller id not sent
							if ((cm_mo_prefix(0) & 0x0c) == 0x0c)
								display_info.TextId2 = TxtActivated;
							// By defaut the caller id is sent
							else
								display_info.TextId2 = TxtDeActivated;*/
							// Check if CLIR invoke is activated: caller id not sent
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Using the CLIR status maintained in the flash, when the status is checked
							if (FFS_flashData.CLIR)
							{
								display_info.TextId2 = TxtActivated;
							}
							// By defaut the caller id is sent
							else
							{
								display_info.TextId2 = TxtDeActivated;
					}
								
					}
					break;
				case MFW_CL_PERM:
					// No CLIR suppress or invoke
					cm_mo_prefix(0x04);
					// No break
				case MFW_CL_UNKNOWN:
				default:
					// The CLIR is active and permanent the caller id is not sent
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
					FFS_flashData.CLIR=TRUE;
					flash_write();
					display_info.TextId2 = TxtActivated;
					break;
			}			
		}
		// CLIR service is not activated: The caller id is sent
	else
	{
		// Dec 07, 2005    REF: OMAPS00050087 x0039928
		// Fix: String "Not Activated" is displayed if the network doesn't permit CLIR
		switch(CFAction)
		{
			case CFActivate:
				display_info.TextId2 = TxtNotActivated;
				break;
			case CFDeactivate:
			case CFCheck:
			default:
				display_info.TextId2 = TxtDeActivated;
				break;
		}
		// No CLIR suppress or invoke
//  Apr 22, 2004        REF: CRR MMI-SPR-16873  Rashmi C N(Sasken) 
//		Maintaing the CLIR status in the flash. 
		FFS_flashData.CLIR=FALSE;
		flash_write();
		cm_mo_prefix(0x04);
	}

	// Call Info Screen
  ss_data.info_win = info_dialog (ss_data.win, &display_info);

}


#ifndef FF_MMI_ATC_MIGRATION   /* x0039928 -Lint warning fix */
/*******************************************************************************

 $Function:    	showIMEI2 

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void showIMEI2 (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf)
{
    T_DISPLAY_DATA   display_info;
//	July 09, 2005   REF : MMI-22565 - a0876501
//	Increased the IMEI_LEN from 15 to 17 for displaying the Cd byte & SW version.
#ifndef FF_MMI_ATC_MIGRATION
#define IMEI_LEN 17         /* IMEI + check digit = 15, IMEI + SW version = 16, IMEI + check digit + SW version = 17*/
#endif
    static char imei_array[IMEI_LEN+1];
    //GW IMEI number is 26 chars long!
	memset(imei_array,0x00,IMEI_LEN+1);   
    memcpy(imei_array, ss_cnf->imei_info.imei_number, IMEI_LEN);

	dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull,TxtIMEI, TxtNull , COLOUR_STATUS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_CLEAR | KEY_LEFT );
    display_info.TextString2   = imei_array;
      /*
       * Call Info Screen
       */
    ss_data.info_win = info_dialog (ss_data.win, &display_info);

}
#endif

/*******************************************************************************

 $Function:    	showUSSDInfo 

 $Description:	this is used for both USSD types (confirm and request).
 				The USSD request is keep going after "showUSSDInfo"
 				function and open the editor to enter the string.

 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static void showUSSDInfo (T_MFW_HND win, USHORT ident, T_MFW_SS_PARA  * ss_cnf, USHORT sk1, USHORT sk2)
{
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA	editor_data;
#endif

	TRACE_FUNCTION("showUSSDInfo");

	/* keep the dcs; need it later for sending */
	ss_data.dcs = ss_cnf->ussd_info.dcs;

	/* SPR#1428 - SH - New Editor changes */
	
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetMode(&editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
	
	//x0pleela 29 Aug, 2006  DR: OMAPS00083503
	//Dynamically changing the dcs value in editor settings
	if( ss_getAlphabetCb ( ss_data.dcs EQ SS_ALPHA_16_Bit) )
		AUI_edit_SetBuffer(&editor_data, ATB_DCS_UNICODE, (UBYTE *)ss_cnf->ussd_info.ussd, MAX_USSD_LEN);
	else
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_cnf->ussd_info.ussd, MAX_USSD_LEN);
	if (ident EQ ServicesUSSD_REQ_OK)
	{
		AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtSsMsg, NULL);
	}
	else
	{
		AUI_edit_SetTextStr(&editor_data, TxtNull, TxtSoftBack, TxtSsMsg, NULL);
	}
	AUI_edit_SetEvents(&editor_data, ident, TRUE, FOREVER, (T_AUI_EDIT_CB)services_info_cb);

	AUI_edit_Start(ss_data.win, &editor_data);  /* start the editor */
#else /* NEW_EDITOR */
	editor_data.editor_attr.win.px   = 6;
	editor_data.editor_attr.win.py   = 12;
	editor_data.editor_attr.win.sx   = 80;
	editor_data.editor_attr.win.sy   = 20;
	editor_data.editor_attr.edtCol	= COLOUR_EDITOR_XX,
	editor_data.editor_attr.font     = 0;
	editor_data.editor_attr.mode    = edtCurNone;
	editor_data.editor_attr.controls   = 0;

	editor_data.hide			    = FALSE;		    
	editor_data.editor_attr.text	= (char*)ss_cnf->ussd_info.ussd;
	editor_data.editor_attr.size	= sizeof(ss_cnf->ussd_info.ussd);

	if (ident EQ ServicesUSSD_REQ_OK)
	{
		editor_data.LeftSoftKey			= TxtSoftOK;
	editor_data.RightSoftKey		= TxtSoftBack;
	}
	else
	{
		editor_data.LeftSoftKey			= TxtNull;
		editor_data.RightSoftKey		= TxtSoftBack;
	}

	editor_data.AlternateLeftSoftKey     = TxtNull;
	editor_data.TextString			= TxtNull;
	editor_data.TextId				= TxtSsMsg;
	editor_data.Identifier			= ident;
	editor_data.mode				= READ_ONLY_MODE;
	editor_data.timeout		        = FOREVER;
	editor_data.min_enter		    = 1;
	editor_data.Callback		    = (T_EDIT_CB)services_info_cb;
	editor_data.destroyEditor	    = TRUE;

	/* create the dialog handler */
	editor_start(ss_data.win, &editor_data);  /* start the editor */ 
#endif /* NEW_EDITOR */

}


/*******************************************************************************

 $Function:    	 showUSSDREQ

 $Description:	show USSD info from Request, and then allow user to enter a response.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void showUSSDREQ (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf)
{
    TRACE_FUNCTION("showUSSDREQ");
    showUSSDInfo(win, ServicesUSSD_REQ_OK, ss_cnf, TxtSoftOK, TxtSoftBack);

}



/*******************************************************************************

 $Function:    	 showUSSDCNF

 $Description:	display USSD message
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void showUSSDCNF (T_MFW_HND win, T_MFW_SS_PARA  * ss_cnf)
{

    T_DISPLAY_DATA   display_info;
    int nPixels = 0; 
    int nchar = 0;

    //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
    //Bug : Unable to compose a one or two digit number
    //Fix :  This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled.
    //The read only editor with the service provider message that comes up when a one or two digit is dialled
    //was not handled properly. So altered the editor handling to cater for the long message.
    T_MFW_WIN 	 *win_data;
    T_ss 	               *data = NULL;
    T_AUI_EDITOR_DATA 		display_editor_info;	

    TRACE_FUNCTION("showUSSDCNF");
	/* API - 09-07-03 - CQ10728 - Add this check to use either the editor or the bubble depending
								   on the size of the returned string */
	/* API - 27-08-03 - CQ10728 - Update, calculate the length of the string in number of pixels */

    //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
    //Bug : Unable to compose a one or two digit number
    //Fix :  This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled.
    //The read only editor with the service provider message that comes up when a one or two digit is dialled
    //was not handled properly. So altered the editor handling to cater for the long message.
    if(win)
    {
         win_data   = ((T_MFW_HDR *)win)->data;
	  if(win_data)	
    	      data         = (T_ss * )win_data->user;
    }	
	
	nchar = strlen((char *)ss_cnf->ussd_info.ussd);
	nPixels = dspl_GetTextExtent( (char *)ss_cnf->ussd_info.ussd, nchar);
	/* API - 27-08-03 - CQ10728 - END */
	
    if (nPixels >= SCREEN_SIZE_X && (data != NULL))   /* x0039928 - Lint warning removal */
    {
	    //Feb 17, 2005   REF : CRR 17657 x0012850 Divya.P.V
	    //Bug : Unable to compose a one or two digit number
           //Fix : This bug is reproducible when a very long message is sent by the service provider when an invalid one or two digit number is dialled.
	    //The read only editor with the service provider message that comes up when a one or two digit is dialled
	    //was not handled properly. So altered the editor handling to cater for the long message.
	   AUI_edit_SetDefault(&display_editor_info);
          AUI_edit_SetMode(&display_editor_info, ED_MODE_READONLY, ED_CURSOR_UNDERLINE);

	//x0pleela 29 Aug, 2006  DR: OMAPS00083503
	//Dynamically changing the dcs value in editor settings
	if( ss_getAlphabetCb ( (UBYTE)ss_cnf->ussd_info.dcs EQ SS_ALPHA_16_Bit))
		AUI_edit_SetBuffer(&display_editor_info, ATB_DCS_UNICODE, (UBYTE*)ss_cnf->ussd_info.ussd, (USHORT)ss_cnf->ussd_info.len);
	else
	   AUI_edit_SetBuffer(&display_editor_info, ATB_DCS_ASCII, (UBYTE*)ss_cnf->ussd_info.ussd, (USHORT)ss_cnf->ussd_info.len);

	   AUI_edit_SetTextStr(&display_editor_info, TxtNull, TxtSoftBack,TxtNull,NULL);
	   AUI_edit_SetEvents(&display_editor_info, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)services_info_cb);  
          display_editor_info.Identifier = ServicesUSSD_CNF_OK;
	   data->info_win = AUI_edit_Start(data->win, &display_editor_info);
    }
	else
	{
		if (strlen((char *)ss_cnf->ussd_info.ussd) == 0)
		{
			dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, TxtCommand,  TxtCompleted, COLOUR_STATUS);
		}
		else
		{
			dlg_initDisplayData_TextStr(&display_info, TxtSoftOK, TxtNull, (char *)ss_cnf->ussd_info.ussd, NULL, COLOUR_STATUS);
		}

		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FOREVER, KEY_HUP| KEY_LEFT);
    	display_info.Identifier   = ServicesUSSD_CNF_OK;

     	/*
       	* Display Dialog Info Screen
       	*/

    	ss_data.info_win = info_dialog (ss_data.win, &display_info);
	}
	/*API - 09-07-03 CQ10728- END*/
}


/*******************************************************************************

 $Function:    	 sendUSSD

 $Description:	Send USSD message
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void sendUSSD (T_MFW_SS_PARA *ussd_senddata)
{

 //   unsigned int tLen = ussd_senddata->ussd_info.len;  // RAVI

	ss_cnf = ussd_senddata;

	if (ss_send_ussd( (T_MFW_SS_USSD *) ss_cnf ) == MFW_SS_FAIL)
    {
		/*   ussd data is wrong */
        showFailed( ss_data.win );
    }
	else
        if (servicesState == ServicesUSSDSendEdit)
        {
            servicesExec(ServicesUSSDSendEdit_Ok, 0);
        }
        else
        {
		    servicesExec(ServicesUSSDEnter_Send, 0);
}
}


/*******************************************************************************

 $Function:    	 showCFStatus

 $Description:	display CFU state
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void showCFStatus (T_MFW_HND win , T_ss * data)
{
  T_DISPLAY_DATA   display_info;
  T_ss_feature_cf_data * pFeature;
	static char str[50];
								

  TRACE_FUNCTION("showCFStatus");

	//MC got rid of dodgy pointer arithmetic
  pFeature = &data->mfw_ss_cf->ss[data->current_feature];

  if (data->max_feature == 1)
  {
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
//	We are now passing the type of CF as the first stringID for the dialog info screen 
// serviceGetType(ss_Code) this function would return the string id for the ss_code being passed
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, serviceGetType((T_MFW_SS_CODES)ss_Code),  TxtNull, COLOUR_STATUS);

// Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//the time parameter is changed from FOREVER to FIVE_SECS
	  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT);
  }
  else
  {
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
 //	We are now passing the type of CF as the first stringID for the dialog info screen 
 // serviceGetType(ss_Code) this function would return the string id for the ss_code being passed
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtSoftNext, serviceGetType((T_MFW_SS_CODES)ss_Code),  TxtNull, COLOUR_STATUS);
	//Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//the time parameter is changed from FOREVER to FIVE_SECS
	   dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT | KEY_RIGHT );
  }

  display_info.Identifier   = E_SS_CF_CNF;
  memset(str, 0, 50);
  //Only show the number when activating call diverts
  if (pFeature->status == TxtActivated)
  {
//    Jun 10, 2004        REF: CRR MMI-SPR-13614  Rashmi C N(Sasken) 
// When call forward is activated, the service(i.e voice, fax or data) and the number to which it is forwarded
// is passed as the second string for the dialog display. Only two strings or string IDs can be given for a 
//dialog info screen. Hence, we are adding the service and the number into one string.	  
	strcat(str,(char*)MmiRsrcGetText(pFeature->service));
	strcat(str,"   ");
	strcat(str,pFeature->forwarded_to_number);
	display_info.TextString2=str;
  }
  else /* SPR#2252 - DS - Show status */
  {
	display_info.TextId2 = pFeature->status;
  }

  /*
   * Call Info Screen
   */

  ss_data.info_win = info_dialog (ss_data.win, &display_info);

  if (ss_data.info_win EQ NULL)
  {
    TRACE_FUNCTION("NOT ENOUGH RESOURCES TO CREATE EXT INFO DIALOG");
  }
}


/*******************************************************************************

 $Function:    	 showCBStatus

 $Description:	display CB state
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static void showCBStatus (T_MFW_HND win , T_ss * data)
{
  T_DISPLAY_DATA   display_info;
  T_ss_feature_cb_data * pFeature;

  TRACE_FUNCTION("showCBStatus");

	//MC got rid of dodgy pointer arithmetic
  pFeature = &data->mfw_ss_cb->ss[data->current_feature];

  if (data->max_feature == 1)
  {
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtNull, pFeature->service,  pFeature->status, COLOUR_STATUS);
	  //Jun 04,2004 CRR:13601 xpradipg - SASKEN
	//Timer value changed from FOREVER to FIVE_SECS
	  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT);
  }
  else
  {
	  dlg_initDisplayData_TextId( &display_info, TxtSoftOK, TxtSoftNext, pFeature->service,  pFeature->status, COLOUR_STATUS);
	  //Jun 04,2004 CRR:13601 xpradipg - SASKEN
	  //Timer value changed from FOREVER to FIVE_SECS
	  dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, FIVE_SECS, KEY_CLEAR| KEY_LEFT | KEY_RIGHT );
  }
  display_info.Identifier   = E_SS_CB_CNF;  // can be used also for CF and CW

  /*
   * Call Info Screen
   */

  ss_data.info_win = info_dialog (ss_data.win, &display_info);

  if (ss_data.info_win EQ NULL)
  {
    TRACE_FUNCTION("NOT ENOUGH RESOURCES TO CREATE EXT INFO DIALOG");
  }
}

/*******************************************************************************

 $Function:    	 getCFAction

 $Description:	To return a string corresponding to call forwarding action.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCFAction(char * pSSstring)
{ 
    switch (CFAction)
    {
    case CFActivate:
        strcpy(pSSstring, "**");
        break;
    case CFDeactivate:
        strcpy(pSSstring, "##");
        break;
    case CFCheck:
        strcpy(pSSstring, "*#");
        break;
    default:
        break;
    }
   
}


/*******************************************************************************

 $Function:    	 getCFType

 $Description:	To return a string corresponding to call forwarding type:
            CFU, CFNRy, CFU, etc.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCFType(char* pSSstring)
{
    switch (CFType)
    {
    case CFBusy:
        strcat(pSSstring, "67*");
        break;
    case CFNoReply:
        strcat(pSSstring, "61*");
        break;
    case CFNoReach:
        strcat(pSSstring, "62*");
        break;
    case CFUnconditional:
        strcat(pSSstring, "21*");
        break;
    case CFAllConditional:
        strcat(pSSstring, "004*");
        break;
    case CFAllCalls:
        strcat(pSSstring, "002*");
        break;
    default:
        break;
    }
}




/*******************************************************************************

 $Function:    	 getCFNumber

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void getCFNumber(char * pSSstring)
{

    strcat(pSSstring, ss_data.edt_buf_number); 
}



/*******************************************************************************

 $Function:    	 getCFService

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCFService(char * pSSstring)
{

    switch (CFService)
    {
    case CFVoice:
        strcat(pSSstring, "*11"); //SPR#0776 - DS -changed from 10 to 11
        break;
    /*MC SPR 742, handle line 2, auxiliary telephony*/
    case CFAuxVoice:
		strcat(pSSstring, "*89"); 
        break;
    case CFFax:
        strcat(pSSstring, "*13");
        break;
    case CFData:
        strcat(pSSstring, "*12"); /*a0393213 cphs rel4 - changed service code from 20 to 12 as per 51.011 spec*/
        break;
    default:
        break;
    }
}



/*******************************************************************************

 $Function:    	 servicesSendCallDivert

 $Description:	Send out Call Divert String to MFW according to static variables.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static int servicesSendCallDivert( char * divert_number)
{	
    char SS_string[60] = {'\0'};

    /* build up SS string according to menu selections */
	getCFAction(SS_string);
	getCFType(SS_string);
    if (divert_number)
        strcat(SS_string, divert_number);
    else
	    getCFNumber(SS_string);
	getCFService(SS_string);
	strcat(SS_string, "#");

	/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
      Description : CPHS Call Forwarding feature implementation
      Solution     : previous code is deleted because it was setting CF flag in flash before getting network confirmation*/
		
	
    TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
	{
        showFailed( ss_data.win );
	}
	return 0;
}



/*******************************************************************************

 $Function:    	 servicesActivateCLIR

 $Description:	activate CLIR
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

int servicesActivateCLIR(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];

	CFAction = CFActivate;
	strcpy(actstr, "#31#");  

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
		showFailed( ss_data.win );
	else
	{
	strcpy(actstr, "*#31#");   

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );
	}


	return 0;
}


/*******************************************************************************

 $Function:    	 servicesSuppressCLIR

 $Description:	suppress CLIR
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesDeactivateCLIR(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];

	CFAction = CFDeactivate;
	strcpy(actstr, "*31#");    

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
		showFailed( ss_data.win );
	else
	{
	strcpy(actstr, "*#31#");    

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );
	}

	return 0;
}

/*******************************************************************************

 $Function:    	 servicesCheckCLIR

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



int servicesCheckCLIR(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];

	CFAction = CFCheck;

	strcpy(actstr, "*#31#");

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

	return 0;
}


/*******************************************************************************

 $Function:    	 servicesCheckCWaiting

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

int servicesCheckCWaiting(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];

	strcpy(actstr, "*#43#");

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

	return 0;
}


/*******************************************************************************

 $Function:    	 servicesActivateCWaiting

 $Description:	activate CallWaiting functionality
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesActivateCWaiting(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];


/* JVJ #1586 Changed *43*10# to *43*#  */
	strcpy(actstr, "*43*#");

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

	return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	 servicesDeactivateCWaiting

 $Description:	deactivate CallWaiting functionality
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



int servicesDeactivateCWaiting(MfwMnu* m, MfwMnuItem* i) {
	char actstr[8];

/* JVJ #1586 Changed #43*10# to #43# */
	strcpy(actstr, "#43#");

	if (ss_execute_transaction((UBYTE*)actstr, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

	return  MFW_EVENT_CONSUMED;
}




/*******************************************************************************

 $Function:    	servicesInterCFU

 $Description:	Interrogate the CFU functionality
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/




int servicesInterCFU(void)
{
	char actstr[8];

	strcpy(actstr, "*#21#");

    ss_execute_transaction((UBYTE*)actstr, 0);

	return 0;
}









#if(0) /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:    	ServiceGetStatus

 $Description:	return the code for the MMI text string for the service status.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


unsigned int ServiceGetStatus( UBYTE pStatus)
{
    unsigned int strStatusCode = 0;

	TRACE_FUNCTION("++ ServiceGetStatus");

    if  (pStatus & SS_STAT_ACTIVE)
        strStatusCode = TxtActivated;
    /* Marcus: Issue 1652: 03/02/2003: Removed NDH 10/06/03 */
    else
        strStatusCode = TxtDeActivated;
    return strStatusCode;
}


/*******************************************************************************

 $Function:    	serviceGetServiceType

 $Description:	return the code for the MMI text string for the service type.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


unsigned int serviceGetServiceType( UBYTE ss_telecom_type, UBYTE ss_telecom_service)
{
    unsigned int strServCode = 0;

	TRACE_FUNCTION("++ serviceGetServiceType");
    switch ( ss_telecom_type )
    {
        case MFW_BST_UNKNOWN:
            break;

        case MFW_BST_TELE:
            switch ( ss_telecom_service )
            {
                case MFW_TS_ALL_TS:         strServCode = TxtAllCalls; break;/* all teleservices               */
                case MFW_TS_ALL_SPCH:       /* All speech transmission services */
                case MFW_TS_TLPHNY:         /* telephony                      */
                case MFW_TS_EMRGNCY:        /* emergency calls                */
                    strServCode = TxtVoice;
                    break;
                case MFW_TS_ALL_SMS:        /* all SMS services               */
                case MFW_TS_SMS_MT:         /* SMS MT PP                      */
                case MFW_TS_SMS_MO:         /* SMS MO PP                      */
                    /*strServCode = TxtSMS;*/
                    break;
                case MFW_TS_ALL_FAX:        /* all FAX transmission services  */
                case MFW_TS_FAX3_ALT_SPCH:  /* FAX group 3 alter. speech      */
                case MFW_TS_FAX3_AUTO:      /* FAX group 3 automatic          */
                case MFW_TS_FAX4:           /* FAX group 4                    */
                    strServCode = TxtFax;
                    break;
                case MFW_TS_ALL_DATA:       strServCode = TxtData; break;/* all FAX and SMS services       */
                case MFW_TS_ALL_XCPT_SMS:   strServCode = TxtNotImplemented;
                    TRACE_FUNCTION("all x SMS"); break;/* all FAX and speech services    */
                case MFW_TS_ALL_PSSS:       strServCode = 0; break;/* all PLMN specific TS           */
                case MFW_TS_PLMN1:          /* PLMN specific TS 1             */
                case MFW_TS_PLMN2:          /* PLMN specific TS 2             */
                case MFW_TS_PLMN3:          /* PLMN specific TS 3             */
                case MFW_TS_PLMN4:          /* PLMN specific TS 4             */
                case MFW_TS_PLMN5:          /* PLMN specific TS 5             */
                case MFW_TS_PLMN6:          /* PLMN specific TS 6             */
                case MFW_TS_PLMN7:          /* PLMN specific TS 7             */
                case MFW_TS_PLMN8:          /* PLMN specific TS 8             */
                case MFW_TS_PLMN9:          /* PLMN specific TS 9             */
                case MFW_TS_PLMNA:          /* PLMN specific TS A             */
                case MFW_TS_PLMNB:          /* PLMN specific TS B             */
                case MFW_TS_PLMNC:          /* PLMN specific TS C             */
                case MFW_TS_PLMND:          /* PLMN specific TS D             */
                case MFW_TS_PLMNE:          /* PLMN specific TS E             */
                case MFW_TS_PLMNF:          /* PLMN specific TS F             */
                    strServCode = 0;
                    break;

                default: break;

            }
            break;

        /* NOTE that we are returning "Data" for all Bearer services until otherwise spec'ed */
        case MFW_BST_BEARER:
            switch ( ss_telecom_service )
            {
                case MFW_BS_ALL_BS       : strServCode = TxtNotImplemented; /* strServCode = TxtAllServices; */ break; /* all bearer services            */
                case MFW_BS_ALL_DATA_CDA : /* all data CDA services          */
                case MFW_BS_CDA_300      : /* data CDA  300 bps              */
                case MFW_BS_CDA_1200     : /* data CDA 1200 bps              */
                case MFW_BS_CDA_1200_75  : /* data CDA 1200/75 bps           */
                case MFW_BS_CDA_2400     : /* data CDA 2400 bps              */
                case MFW_BS_CDA_4800     : /* data CDA 4800 bps              */
                case MFW_BS_CDA_9600     : /* data CDA 9600 bps              */
                case MFW_BS_ALL_DATA_CDS : /* all data CDS services          */
                case MFW_BS_CDS_1200     : /* data CDS 1200 bps              */
                case MFW_BS_CDS_2400     : /* data CDS 2400 bps              */
                case MFW_BS_CDS_4800     : /* data CDS 4800 bps              */
                case MFW_BS_CDS_9600     : /* data CDS 9600 bps              */
                case MFW_BS_ALL_DATA_PAD : /* all data PAD services          */
                case MFW_BS_PAD_300      : /* data PAD  300 bps              */
                case MFW_BS_PAD_1200     : /* data PAD 1200 bps              */
                case MFW_BS_PAD_1200_75  : /* data PAD 1200/75 bps           */
                case MFW_BS_PAD_2400     : /* data PAD 2400 bps              */
                case MFW_BS_PAD_4800     : /* data PAD 4800 bps              */
                case MFW_BS_PAD_9600     : /* data PAD 9600 bps              */
                case MFW_BS_ALL_DATA_PDS : /* all data PDS services          */
                case MFW_BS_PDS_2400     : /* data PDS 2400 bps              */
                case MFW_BS_PDS_4800     : /* data PDS 4800 bps              */
                case MFW_BS_PDS_9600     : /* data PDS 9600 bps              */
                case MFW_BS_SPCH_ALT_CDA : /* all data CDA alter. speech     */
                case MFW_BS_SPCH_ALT_CDS : /* all data CDS alter. speech     */
                case MFW_BS_SPCH_FLD_CDA : /* all data speech followed CDA   */
                case MFW_BS_SPCH_FLD_CDS : /* all data speech followed CDA   */
                case MFW_BS_ALL_DC_ASYN  : /* all data circuit asynchronous  */
                case MFW_BS_ALL_ASYN     : /* all asynchronous services      */
                case MFW_BS_ALL_DC_SYN   : /* all data circuit synchronous   */
                case MFW_BS_ALL_SYN      : /* all synchronous services       */
                case MFW_BS_ALL_PSSS     : /* all PLMN specific BS           */
                case MFW_BS_PLMN1        : /* PLMN specific 1                */
                case MFW_BS_PLMN2        : /* PLMN specific 2                */
                case MFW_BS_PLMN3        : /* PLMN specific 3                */
                case MFW_BS_PLMN4        : /* PLMN specific 4                */
                case MFW_BS_PLMN5        : /* PLMN specific 5                */
                case MFW_BS_PLMN6        : /* PLMN specific 6                */
                case MFW_BS_PLMN7        : /* PLMN specific 7                */
                case MFW_BS_PLMN8        : /* PLMN specific 8                */
                case MFW_BS_PLMN9        : /* PLMN specific 9                */
                case MFW_BS_PLMNA        : /* PLMN specific A                */
                case MFW_BS_PLMNB        : /* PLMN specific B                */
                case MFW_BS_PLMNC        : /* PLMN specific C                */
                case MFW_BS_PLMND        : /* PLMN specific D                */
                case MFW_BS_PLMNE        : /* PLMN specific E                */
                case MFW_BS_PLMNF        : /* PLMN specific F                */
                    strServCode = 0; break;

                default: break;

            }
            break;

        default:
            break;
    }
    return strServCode;
}

#endif
/*******************************************************************************

 $Function:    	serviceGetError

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static unsigned int serviceGetError( T_MFW_SS_ERROR ss_error )
{
    unsigned int errStrCode = TxtNotAvailable;
    TRACE_FUNCTION("serviceGetError");

    switch (ss_error)
    {
      case MFW_SS_ERR_UNKNOWN:
        errStrCode=TxtErrUnknown;
        break;
      case MFW_SS_ERROR:
        errStrCode=TxtSSErr;
        break;
      case MFW_SS_ERR_UNKNOWN_SUBSCRIBER:
        errStrCode=TxtUnknownSub;
        break;
      case MFW_SS_ERR_ILLEGAL_SUBSCRIBER:
        errStrCode=TxtIllSub;
        break;
      case MFW_SS_ERR_BEARER_SVC_NOT_PROV:
      case MFW_SS_ERR_TELE_SVC_NOT_PROV:
        errStrCode = TxtNotSubscribed;
        break;
      case MFW_SS_ERR_ILLEGAL_EQUIPMENT:
        errStrCode = TxtIllEqip;
        break;
      case MFW_SS_ERR_CALL_BARRED:
        errStrCode = TxtBarred;
        break;
      case MFW_SS_ERR_ILLEGAL_SS_OPERATION:
        errStrCode = TxtIllOp;
        break;
      case MFW_SS_ERR_SS_ERR_STATUS:
        errStrCode = TxtCheckStatus;
        break;
      case MFW_SS_ERR_SS_NOT_AVAIL:
        errStrCode = TxtNotAvailable;
        break;
      case MFW_SS_ERR_SS_SUBS_VIOLATION:
        errStrCode = TxtSubVil;
        break;
      case MFW_SS_ERR_SS_INCOMP:
        errStrCode = TxtInComp;
        break;
      case MFW_SS_ERR_FAC_NOT_SUPPORTED:
        errStrCode = TxtFacNoSup;
        break;
      case MFW_SS_ERR_ABSENT_SUBS:
        errStrCode = TxtAbsSub;
        break;
      case MFW_SS_ERR_SYSTEM_FAIL:
        errStrCode = TxtSysFail;
        break;
      case MFW_SS_ERR_DATA_MISSING:
        errStrCode = TxtMissDta;
        break;
      case MFW_SS_ERR_UNEXPECT_DATA:
      case MFW_SS_ERR_PROB_UNEXP_ERR:
      case MFW_SS_RES_PROB_RET_RES_UNEXP:
      case MFW_SS_ERR_PROB_RET_ERR_UNEXP:
      case MFW_SS_INV_PROB_LNK_RES_UNEXP:
      case MFW_SS_INV_PROB_UNEXP_LNK_OP:
        errStrCode = TxtUnexDta;
        break;
      case MFW_SS_ERR_PWD_REG_FAIL:
        errStrCode = TxtPwdErr;
        break;
      case MFW_SS_ERR_NEG_PWD_CHECK:
        errStrCode = TxtCodeInc;
        break;
      case MFW_SS_ERR_NUM_PWD_VIOLATION:
        errStrCode = TxtBlckPerm;
        break;
      case MFW_SS_ERR_UNKNOWN_ALPHA:
        errStrCode = TxtUnkAlph;
        break;
      case MFW_SS_ERR_USSD_BUSY:
        errStrCode = TxtNetworkBusy;
        break;
      case MFW_SS_ERR_MAX_NUM_MPTY_EXCEED:
        errStrCode = TxtMaxMpty;
        break;
      case MFW_SS_ERR_RESOURCE_NOT_AVAIL:
        errStrCode = TxtOperationNotAvail;
        break;
      case MFW_SS_RES_PROB_UNRECOG_INV_ID:
      case MFW_SS_INV_PROB_UNRECOG_LNK_ID:
      case MFW_SS_ERR_PROB_UNRECOG_INV_ID:
      case MFW_SS_GEN_PROB_UNRECOG_CMP:
      case MFW_SS_INV_PROB_UNRECOG_OP:
      case MFW_SS_ERR_PROB_UNRECOG_ERR:
        errStrCode = TxtUnRec;
        break;
      case MFW_SS_RES_PROB_MISTYPED_PAR:
      case MFW_SS_GEN_PROB_MISTYPED_CMP:
      case MFW_SS_INV_PROB_MISTYPED_PAR:
      case MFW_SS_ERR_PROB_MISTYPED_PAR:
        errStrCode = TxtMisTyp;
        break;
      case MFW_SS_GEN_PROB_BAD_STRUCT_CMP:
        errStrCode = TxtBadStruct;
        break;
      case MFW_SS_INV_PROB_DUPL_INV_ID:
        errStrCode = TxtDupInv;
        break;
      case MFW_SS_INV_PROB_RESOURCE_LIM:
        errStrCode = TxtResLim;
        break;
      case MFW_SS_INV_PROB_INIT_RELEASE:
        errStrCode = TxtIniRel;
        break;
      case MFW_SS_ERR_FATAL_INV_RESULT:
      case MFW_SS_ERR_FATAL_CCD_DEC:
      case MFW_SS_ERR_FATAL_SS_ENT:
        errStrCode = TxtFatRes;
        break;
default:
        break;
    }
    return errStrCode;
}

/*******************************************************************************

 $Function:    	serviceGetType

 $Description:	Return code of text string that indicates the SS Type
            CW, CF, CFU, etc
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


unsigned int serviceGetType( T_MFW_SS_CODES type )
{
    unsigned int strTypeCode = 0;

	TRACE_FUNCTION("++ serviceGetType");

    switch (type)
    {
        case SS_CODES_UNKNOWN:
            /* Marcus: Issue 1652: 03/02/2003: Start */
            TRACE_EVENT("SS_CODES_UNKNOWN");
            strTypeCode =  TxtUnknown;
            /* Marcus: Issue 1652: 03/02/2003: End */
            break;

        case SS_CF_ALL:
            strTypeCode =  TxtAllDivert;
            break;

        case SS_CF_ALL_COND:
            strTypeCode =  TxtCondDivert;
            break;

        case SS_CF_CFU:
            strTypeCode =  TxtAllCallsDivert;
            break;

        case SS_CF_CFNRy:
            strTypeCode = TxtNoAnsDivert ;
            break;

        case SS_CF_CFNR:
            strTypeCode =  TxtNoReachDivert;
            break;

        case SS_CF_CFB:
            strTypeCode =  TxtBusyDivert;
            break;

        case SS_CB_BAOC:
            strTypeCode =  TxtCallBAOC;
            break;
        case SS_CB_BAOIC:
            strTypeCode =  TxtCallBAOIC;
            break;
        case SS_CB_BAOICexH:
            strTypeCode =  TxtCallBAOICexH;
            break;
        case SS_CB_BAIC:
            strTypeCode =  TxtCallBAIC;
            break;
        case SS_CB_BAICroam:
            strTypeCode =  TxtCallBAICroam;
            break;
        case SS_CB_ALL:
            strTypeCode =  TxtCallBarringAll;
            break;
        case SS_CB_ALL_BAOC:
            strTypeCode =  TxtCallBAOCAll;
            break;
        case SS_CB_ALL_BAIC:
            strTypeCode =  TxtCallBAICAll;
            break;

        case SS_CW:
            strTypeCode = TxtCallWaiting;
            break;

        case SS_CCBS:
            break;

        case SS_CLIP:
            strTypeCode = TxtCLIP;
            break;
        case SS_CLIR:
            strTypeCode =  TxtSendID;
            break;

        case SS_COLP:
            strTypeCode = TxtCOLP;
            break;
        case SS_COLR:
            strTypeCode = TxtCOLR;
            break;

        case SS_PIN1:
        case SS_PIN2:
            break;

        default:
            break;
    }
    return strTypeCode;
}

#if(0) /* x0039928 -Lint warning removal */
/*******************************************************************************

 $Function:    	serviceGetCategory

 $Description:	return the code for the MMI text string for the service category.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

unsigned int serviceGetCategory( T_MFW_SS_CATEGORY cat )
{
    unsigned int strCatCode = 0;

	TRACE_FUNCTION("++ serviceGetCategory");

    switch ( cat )
    {
        case SS_CTG_UNKNOWN:
            break;

        case SS_REGISTRATION:
        case SS_ACTIVATION:
            strCatCode = TxtActivated;
            break;

        case SS_ERASURE:
        case SS_DEACTIVATION:
            strCatCode = TxtDeActivated;
            break;

        case SS_INTERROGATION:
            TRACE_FUNCTION("SS Interrogation");
            break;

        default:
            break;
    }
    return strCatCode;
}
#endif

/*******************************************************************************

 $Function:    	servicesDivertActivate

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesDivertActivate(void)
{
    TRACE_FUNCTION("servicesDivertActivate");
    CFAction = CFActivate;
}

/*******************************************************************************

 $Function:    	servicesDivertCheck

 $Description:	
            
 $Returns:		

 $Arguments:	

 $History:

 //SPR#0776 - DS -Now takes service type into account by calling getCFServiceCheck.
 
*******************************************************************************/


void servicesDivertCheck(void)
{
    char SS_string[60] = {'\0'};
    /*
     * Interrogate Call Divert
     */
    TRACE_FUNCTION("servicesDivertCheck");

    CFAction = CFCheck;

	getCFAction(SS_string);
	getCFType(SS_string);
	getCFService(SS_string); //SPR#929 - DS - Changed from getCFServiceCheck.
	strcat(SS_string, "#");

    	TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

}

/*******************************************************************************

 $Function:    	servicesDivertDeactivate

 $Description:	
            
 $Returns:		

 $Arguments:	

 $History

 //SPR#0776 - DS -Now takes service type into account by calling getCFServiceCheck.
 
*******************************************************************************/



void servicesDivertDeactivate(void)
{
    char SS_string[60] = {'\0'};
    /*
     * Deactivate Call Divert
     */
    TRACE_FUNCTION("servicesDivertDeactivate");

    CFAction = CFDeactivate;

	getCFAction(SS_string);
	getCFType(SS_string);
    	getCFService(SS_string); //SPR#929 - DS - Changed from getCFServiceCheck.
    	SS_string[strlen(SS_string)] = '#';

    TRACE_FUNCTION(SS_string);
	/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
      Description : CPHS Call Forwarding feature implementation
      Solution     : previous code is deleted because it was setting CF flag in flash before getting network confirmation*/
	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

}

/*******************************************************************************

 $Function:    	servicesDivertVoice

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesDivertVoice(void)
{
    TRACE_FUNCTION("servicesDivertVoice");
 /*mc SPR 742 check if line2 is the current line*/
#ifdef FF_CPHS
   if (!ALSPresent(NULL, NULL, NULL) && GetAlsLine() == MFW_SERV_LINE2/*FFS_flashData.als_selLine == MFW_SERV_LINE2*/)
    	CFService = CFAuxVoice;
    else /*mc end*/
   	CFService = CFVoice;
#endif
}

/*******************************************************************************

 $Function:    	servicesDivertFax

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesDivertFax(void)
{
    TRACE_FUNCTION("servicesDivertFax");
    CFService = CFFax;
}


/*******************************************************************************

 $Function:    	servicesDivertData

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertData(void)
{
    TRACE_FUNCTION("servicesDivertData");
    CFService = CFData;
}

/*******************************************************************************

 $Function:    	servicesDivertActionAll

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesDivertActionAll(void)
{
    TRACE_FUNCTION("servicesDivertActionAll");
    CFService = CFAll;
}


/*******************************************************************************

 $Function:    	servicesDivertBusy

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertBusy(void)
{
  TRACE_FUNCTION("servicesDivertBusy");
  CFType = CFBusy;
}

/*******************************************************************************

 $Function:    	servicesDivertNoAnswer

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertNoAnswer(void)
{
    TRACE_FUNCTION("servicesDivertNoAnswer");
    CFType = CFNoReply;
}

/*******************************************************************************

 $Function:    	servicesDivertNotReachable

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertNotReachable(void)
{
    TRACE_FUNCTION("servicesDivertNotReachable");
    CFType = CFAllConditional;
}

/*******************************************************************************

 $Function:    	servicesDivertNoService

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertNoService(void)
{
    TRACE_FUNCTION("servicesDivertNoService");
    CFType = CFNoReach;
}


/*******************************************************************************

 $Function:    	servicesDivertAllCalls

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertAllCalls(void)
{
    TRACE_FUNCTION("servicesDivertAllCalls");
    CFType = CFUnconditional;
}

/*******************************************************************************

 $Function:    	servicesDivertCancelAll

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDivertCancelAll(void)
{
    TRACE_FUNCTION("servicesDivertCancelAll");
    CFType = CFAllCalls;
    CFAction = CFDeactivate;
    CFService = CFAll;
    servicesSendCallDivert(0);
}

/*******************************************************************************

 $Function:    	servicesBarringAllOutgoing

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesBarringAllOutgoing(void)
{
    TRACE_FUNCTION("servicesBarringAllOutgoing");
    CBType = CBAllOutgoing;
}

/*******************************************************************************

 $Function:    	servicesBarringInternational

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesBarringInternational(void)
{
    TRACE_FUNCTION("servicesBarringInternational");
    CBType = CBInternational;
}

/*******************************************************************************

 $Function:    	servicesBarringInternationalHome

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesBarringInternationalHome(void)
{
    TRACE_FUNCTION("servicesBarringInternationalHome");
    CBType = CBInternationalHome;
}


/*******************************************************************************

 $Function:    	servicesBarringAllIncoming

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesBarringAllIncoming(void)
{
    TRACE_FUNCTION("servicesBarringAllIncoming");
    CBType = CBAllIncoming;
}

/*******************************************************************************

 $Function:    	servicesBarringIncomingRoaming

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesBarringIncomingRoaming(void)
{
    TRACE_FUNCTION("servicesBarringIncomingRoaming");
    CBType = CBIncomingRoaming;
}


/*******************************************************************************

 $Function:    	services_get_voice_number_cb

 $Description:	callback function for the enter voicemail number window
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


#ifdef NEW_EDITOR
static void services_get_voice_number_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_services_input * data = (T_services_input *)win_data->user;

	TRACE_FUNCTION("services_get_voice_number_cb");
    
	switch (reason)
	{
		case INFO_KCD_LEFT:
            TRACE_FUNCTION("SERVICES_INPUT_LEFT");
            services_input_destroy(ss_data.input_number_win);
            /* send number if one has been entered */
            if (strlen(ss_data.edt_buf_number) > 0 )
                SEND_EVENT(data->parent_win, SERVICES_DO_CALL_VOICE_DIVERT, 0, 0);
            break;

		case INFO_KCD_ALTERNATELEFT:
            TRACE_FUNCTION("SERVICES_INPUT_ALTERNATELEFT");
			/* Go to phone book */
			if (bookPhoneBookLoading()==BOOK_SUCCESS)
			{
				// T_MFW_HND phbk_win;  // RAVI
				bookPhonebookStart(win,PhbkFromDivert);  // RAVI - Removed Assignment phbk_win = .
			}
				
            break;

        case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:
        	TRACE_FUNCTION("SERVICES_INPUT_RIGHT/HUP");
            services_input_destroy(ss_data.input_number_win);
            break;
		default:
		break;
	}
}

#else /* NEW_EDITOR */
static void services_get_voice_number_cb(T_MFW_HND win, USHORT reason)
{
    TRACE_FUNCTION("services_get_voice_number_cb");
    
	switch (reason){
		case SERVICES_INPUT_LEFT:
            TRACE_FUNCTION("SERVICES_INPUT_LEFT");
            /* send number if one has been entered */
            if (strlen(ss_data.edt_buf_number) > 0 )
                SEND_EVENT(win, SERVICES_DO_CALL_VOICE_DIVERT, 0, 0);
            break;
		default:
		break;
	}
}
#endif /* NEW_EDITOR */


/*******************************************************************************

 $Function:    	servicesDoVoiceMailNumber

 $Description:	this function should access voicemail number if it is already
            entered, otherwise allow access to voicemail editing window
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/




void servicesDoVoiceMailNumber(void)
{
    char voiceNum[30] = {'\0'};

    TRACE_FUNCTION("servicesDoVoiceMailNumber");

    //JVJEmsgsGetVoiceAddress(voiceNum);

    /* instead of the above we should use something like:  */
    if ( strlen(voiceNum) > 0)
    {
        servicesSendCallDivert(voiceNum);
    }
    else
    {
        T_ss * data = &ss_data;
        T_input_properties input;
        
        memset(data->edt_buf_number,'\0',PHB_MAX_LEN);
		input.abc = FALSE;
		input.text = TxtEnterNumber;
		/* SPR#1428 - SH - New Editor changes - no longer required */
#ifndef NEW_EDITOR
		input.edt_attr_input = &data->edt_attr_number;
#endif /* ifndef NEW_EDITOR */
		input.left_soft_key = TxtSoftOK;
		input.right_soft_key = TxtSoftBack;
		input.callback = services_get_voice_number_cb;
		data->input_number_win = services_input_start(data->win,&input);

    }

}


/*******************************************************************************

 $Function:    	services_get_number_cb

 $Description:	callback function for the enter other number window
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

#ifdef NEW_EDITOR
static void services_get_number_cb(T_MFW_HND win, USHORT identifier, SHORT reason)
{
//	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;  // RAVI
  //  T_services_input * data = (T_services_input *)win_data->user;  // RAVI

	TRACE_FUNCTION("services_get_voice_number_cb");
    
	switch (reason)
	{
		case INFO_KCD_LEFT:
            TRACE_FUNCTION("SERVICES_INPUT_LEFT");
            /* send number if one has been entered */
			SEND_EVENT(ss_data.win, ServicesDoCallDivert, 0, 0);
            services_input_destroy(ss_data.input_number_win);
            ss_data.input_number_win = NULL;
            break;

		case INFO_KCD_ALTERNATELEFT:
            TRACE_FUNCTION("SERVICES_INPUT_ALTERNATELEFT");
			/* Go to phone book */
			if (bookPhoneBookLoading()==BOOK_SUCCESS)
			{
				//T_MFW_HND phbk_win;  // RAVI
				bookPhonebookStart(win,PhbkFromDivert);  // RAVI - Removed assignment phbk_win = 
			}
				
            break;

        case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:
        	TRACE_FUNCTION("SERVICES_INPUT_RIGHT/HUP");
            services_input_destroy(ss_data.input_number_win);
            ss_data.input_number_win = NULL;
            break;
		default:
		break;
	}
}

#else /* NEW_EDITOR */
static void services_get_number_cb(T_MFW_HND win,USHORT reason)
{
    TRACE_FUNCTION("services_get_number_cb");
    
	switch (reason){
		case SERVICES_INPUT_LEFT:
            TRACE_FUNCTION("SERVICES_INPUT_LEFT");
            SEND_EVENT(win, ServicesDoCallDivert, 0, 0);
            break;
		case SERVICES_INPUT_RIGHT:
            TRACE_FUNCTION("SERVICES_INPUT_RIGHT");
            SEND_EVENT(win, ServicesDoCallDivert, 0, 0);
		break;
		default:
		break;
	}
}
#endif /* NEW_EDITOR */

/*******************************************************************************

 $Function:    	servicesGetOtherNumber

 $Description:	Sets up an editor for entering another number.
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/




static void ServicesGetOtherNumber(T_MFW_HND win)
{
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_ss * data = (T_ss *)win_data->user;

    T_input_properties input;

	TRACE_EVENT("ServicesGetOtherNumber");
	
    memset(data->edt_buf_number,'\0',PHB_MAX_LEN);
	input.abc = FALSE;
	input.text = TxtEnterNumber;
	/* SPR#1428 - SH - New Editor changes - no longer required */
#ifndef NEW_EDITOR
	input.edt_attr_input = &data->edt_attr_number;
#endif /* ifndef NEW_EDITOR */
	input.left_soft_key = TxtSoftOK;
	//Oct 20, 2004    REF: CRR 25772  Deepa M.D
	//The RSK name has been set to Delete.
	input.right_soft_key = TxtDelete;
	input.callback = services_get_number_cb;
	data->input_number_win = services_input_start(win,&input);
}

/*******************************************************************************

 $Function:    	servicesDoOtherNumber

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesDoOtherNumber(void)
{

    TRACE_FUNCTION("servicesDoOtherNumber");

    SEND_EVENT(ss_data.win, ServicesOtherNumber, 0, 0);


    /* this is function called from menu, it will also send a signal to start
       window being displayed for editing or calling phonebook */
    /* set up and show edit field */
}


/*******************************************************************************

 $Function:    	servicesStoreVoiceMailNumber

 $Description:	Store number to Voicemail number (currently in SMS, should be in PCM?)
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static void servicesStoreVoiceMailNumber( void )
{
    char voiceNum[30] = {'\0'};
    TRACE_FUNCTION("servicesStoreVoiceMailNumber");

    getCFNumber(voiceNum);

}


#if(0) /* x0039928 -Lint warning removal */
/*******************************************************************************

 $Function:    	servicesStoreUSSD

 $Description:	Store selected USSD to NVM.
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static void servicesStoreUSSD(void)
{
    TRACE_FUNCTION("servicesStoreUSSD");
}
#endif

/*******************************************************************************

 $Function:    	servicesActivateIRDA

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesActivateIRDA(MfwMnu* m, MfwMnuItem* i)
{
	// ADDED BY RAVI - 28-11-2005
	return 1;
	// END RAVI - 28-11-2005
}

/*******************************************************************************

 $Function:    	servicesDeactivateIRDA

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


int servicesDeactivateIRDA(MfwMnu* m, MfwMnuItem* i)
{
	// ADDED BY RAVI - 28-11-2005
	return 1;
	// END RAVI - 28-11-2005
}


static T_MFW_HND   dialog_ext_info_create      (T_MFW_HND  parent_win);
static void        dialog_ext_info_destroy     (T_MFW_HND  own_window);
       void        dialog_ext_info             (T_MFW_HND  win,
                                                USHORT event, SHORT value, void * parameter);
static int         dialog_ext_info_win_cb      (T_MFW_EVENT event, T_MFW_WIN * win);
static T_MFW_CB    dialog_ext_info_tim_cb      (T_MFW_EVENT event, T_MFW_TIM * tc);
static int         dialog_ext_info_kbd_cb      (T_MFW_EVENT event, T_MFW_KBD * keyboard);
static int         dialog_ext_info_kbd_long_cb (T_MFW_EVENT e, T_MFW_KBD *k);


/*******************************************************************************

 $Function:    	dialog_ext_info_dialog

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


T_MFW_HND dialog_ext_info_dialog (T_MFW_HND           parent_win,
                                  T_dialog_ext_info * display_info)
{
  T_MFW_HND win;

  win = dialog_ext_info_create (parent_win);

  if (win NEQ NULL)
  {
    SEND_EVENT (win, DIALOG_INIT, 0, display_info);
  }
  return win;
}


/*******************************************************************************

 $Function:    	dialog_ext_info_create

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static T_MFW_HND dialog_ext_info_create (T_MFW_HND parent_win)
{
  T_MFW_WIN         * win_data;
  T_dialog_ext_info *  data = (T_dialog_ext_info *)ALLOC_MEMORY (sizeof (T_dialog_ext_info));

  TRACE_FUNCTION ("dialog_ext_info_create()");

  if (data EQ 0)
  {
      TRACE_FUNCTION("NOT ENOUGH RESOURCES FOR DATA");
      return 0;
  }
  data->info_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)dialog_ext_info_win_cb);

  if (data->info_win EQ 0)
  {
      TRACE_FUNCTION("NOT ENOUGH RESOURCES FOR 4 line INFO WIN");
      return 0;
  }

  /*
   * Create window handler
   */
  data->mmi_control.dialog   = (T_DIALOG_FUNC)dialog_ext_info;
  data->mmi_control.data     = data;
  data->parent_win           = parent_win;
  win_data                   = ((T_MFW_HDR *)data->info_win)->data;
  win_data->user             = (void *)data;


  win_show(data->info_win);
  /*
   * return window handle
   */
  return data->info_win;
}

/*******************************************************************************

 $Function:    	dialog_ext_info_destroy

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void dialog_ext_info_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  T_dialog_ext_info * data = (T_dialog_ext_info *)win->user;

  TRACE_FUNCTION ("dialog_ext_info_destroy()");

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
		return;
	}

  if (data)
  {
    /*
     * Exit TIMER & KEYBOARD Handle
     */
    kbd_delete (data->info_kbd);
    kbd_delete (data->info_kbd_long);
    tim_delete (data->info_tim);

    /*
     * Delete WIN Handler
     */
    win_delete (data->info_win);
    /*
     * Free Memory
     */
    FREE_MEMORY ((void *)data, sizeof (T_dialog_ext_info));
  }
}

/*******************************************************************************

 $Function:    	dialog_ext_info

 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void dialog_ext_info (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
  T_MFW_WIN      * win_data     = ((T_MFW_HDR *)win)->data;
  T_dialog_ext_info  * data     = (T_dialog_ext_info *)win_data->user;
  T_dialog_ext_info  * display_info = (T_dialog_ext_info *)parameter;

  TRACE_FUNCTION ("dialog_ext_info()");

  switch (event)
  {
    case DIALOG_INIT:
      /*
       * Initialize dialog
       */
      /*
       * Store any other data
       */
      data->LeftSoftKey  = display_info->LeftSoftKey;
      data->RightSoftKey = display_info->RightSoftKey;
      data->Time         = display_info->Time;
      data->KeyEvents    = display_info->KeyEvents;
      data->TextId1      = display_info->TextId1;
      data->TextId2      = display_info->TextId2;
      data->TextString4  = display_info->TextString4;
      data->Identifier   = display_info->Identifier;
      data->Callback     = display_info->Callback;
      /*
       * Create timer and keyboard handler
       */
      data->info_tim             = tim_create (win, display_info->Time, (T_MFW_CB)dialog_ext_info_tim_cb);
      data->info_kbd             = kbd_create (win, KEY_ALL, (T_MFW_CB)dialog_ext_info_kbd_cb);
      data->info_kbd_long        = kbd_create (win, KEY_ALL|KEY_LONG, (T_MFW_CB)dialog_ext_info_kbd_long_cb);

      if (display_info->Time NEQ FOREVER)
        tim_start (data->info_tim);
      win_show(win);
      break;

    case DIALOG_DESTROY:
		dialog_ext_info_destroy(win);
	break;
  }
}

/*******************************************************************************

 $Function:    	dialog_ext_info_win_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static int dialog_ext_info_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  T_dialog_ext_info * data = (T_dialog_ext_info *)win->user;

  TRACE_FUNCTION ("dialog_ext_info_win_cb()");

  if (data EQ 0)
    return 1;

  switch (event)
  {
    case E_WIN_VISIBLE:
      /*
       * Print the information dialog
       */

      /*
       * Clear Screen
       */
      dspl_ClearAll();
      /*
       * Print softkeys
       */
      displaySoftKeys(data->LeftSoftKey, data->RightSoftKey);
      /*
       * Print information
       */
	  if (data->TextId1 NEQ 0)
      {
        displayAlignedText(CENTER, Mmi_layout_line(1), 0, GET_TEXT(data->TextId1));
      }

      if (data->TextId2 NEQ 0)
      {
        displayAlignedText(CENTER, Mmi_layout_line(2), 0, GET_TEXT(data->TextId2));
      }

      if (data->TextString4)
      {
        displayAlignedText(CENTER, Mmi_layout_line(3), 0, data->TextString4);
      }
      break;

    default:
      return 0;
  }
  return 1;
}


/*******************************************************************************

 $Function:    	dialog_ext_info_tim_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static T_MFW_CB dialog_ext_info_tim_cb (T_MFW_EVENT event, T_MFW_TIM *tc)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_ext_info * data = (T_dialog_ext_info *)win_data->user;

  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

  dialog_ext_info_destroy (win);

  if (Callback)
     (Callback) (parent_win, Identifier, INFO_TIMEOUT);

  return 0;
}

/*******************************************************************************

 $Function:    	dialog_ext_info_kbd_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static int dialog_ext_info_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_ext_info * data = (T_dialog_ext_info *)win_data->user;
  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

  TRACE_FUNCTION("dialog_ext_info_kbd_cb");

  switch (keyboard->code)
  {
    case KCD_LEFT:
      if (data->KeyEvents & KEY_LEFT)
      {
	    dialog_ext_info_destroy (win);
	    if (Callback)
		  (Callback) (parent_win, Identifier, INFO_KCD_LEFT);
      }
      break;

    case KCD_RIGHT:
	  if (data->KeyEvents & KEY_RIGHT)
      {
	   	dialog_ext_info_destroy (win);
	    if (Callback)
	      (Callback) (parent_win, Identifier, INFO_KCD_RIGHT);
      }
      break;

    case KCD_HUP:
	  if (data->KeyEvents & KEY_CLEAR)
      {
	    dialog_ext_info_destroy (win);
	    if (Callback)
	  	 (Callback) (parent_win, Identifier, INFO_KCD_HUP);
      }
      break;
     /* dialog_ext_info_destroy (win);  */ /* Unreachable Code - x0020906 - 24-08-2006 */

    case KCD_MNUUP:
       dialog_ext_info_destroy (win);
       if (Callback)
         (Callback) (parent_win, Identifier, INFO_KCD_UP);
       break;

    case KCD_MNUDOWN:
       dialog_ext_info_destroy (win);
       if (Callback)
         (Callback) (parent_win, Identifier, INFO_KCD_DOWN);
       break;

    default:
      return MFW_EVENT_PASSED;
  }
  return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	dialog_ext_info_kbd_long_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/






static int dialog_ext_info_kbd_long_cb (T_MFW_EVENT e, T_MFW_KBD *k)
{
  T_MFW_HND       win  = mfw_parent (mfw_header());
  T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
  T_dialog_ext_info * data = (T_dialog_ext_info *)win_data->user;
  T_MFW_HND       parent_win = data->parent_win;
  USHORT          Identifier = data->Identifier;
  void (* Callback)() = data->Callback;

  TRACE_FUNCTION("dialog_ext_info_kbd_long_cb");

  switch (k->code)
  {
    case KCD_MNUUP:
       dialog_ext_info_destroy (win);
       if (Callback)
         (Callback) (parent_win, Identifier, INFO_KCD_UP);
       break;

    case KCD_MNUDOWN:
       dialog_ext_info_destroy (win);
       if (Callback)
         (Callback) (parent_win, Identifier, INFO_KCD_DOWN);
       break;
  }
  /*
   * Consume all long key presses
   */
  return MFW_EVENT_CONSUMED;

}

/*******************************************************************************

 $Function:    	services_input_create
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/




static T_MFW_HND services_input_create(MfwHnd parent){


	T_MFW_WIN     * win_data;
	T_services_input *  data = (T_services_input *)ALLOC_MEMORY (sizeof (T_services_input));



	TRACE_FUNCTION ("services_input_create()");

	data->win = win_create (parent, 0, E_WIN_VISIBLE, (T_MFW_CB)services_input_win_cb);

	if (data->win EQ 0)
		return 0;

    /*
     * Create window handler
     */
    data->mmi_control.dialog   = (T_DIALOG_FUNC)services_input;
    data->mmi_control.data     = data;
    win_data                   = ((T_MFW_HDR *)data->win)->data;
 	win_data->user             = (void *)data;

   /*
    * return window handle
    */

	/* SPR#1428 - SH - New Editor changes - no longer required */
#ifndef NEW_EDITOR
	data->kbd = kbdCreate(data->win,KEY_ALL,(MfwCb)services_input_kbd_cb);
    data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)services_input_kbd_long_cb);
#endif /* ifndef NEW_EDITOR */

	data->parent_win = parent;

    return data->win;

}


/*******************************************************************************

 $Function:    	services_input_destroy
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/





static void services_input_destroy(MfwHnd window){
	T_MFW_WIN     * win  = ((T_MFW_HDR *)window)->data;
	T_services_input * data = (T_services_input *)win->user;

	TRACE_FUNCTION ("services_input_destroy()");

	if (window == NULL)
	{
		TRACE_EVENT ("Error : Called with NULL Pointer");
		return;
	}

	if (data)
	{
	/* SPR#1428 - SH - Destroy new editor */
#ifdef NEW_EDITOR
		AUI_edit_Destroy(data->edit_win);
#else /* NEW_EDITOR */
        editDeactivate();
#endif /* NEW_EDITOR */

		winDelete (data->win);
		/*     * Free Memory
	     */
		FREE_MEMORY ((void *)data, sizeof (T_services_input));
	}
}


/*******************************************************************************

 $Function:    	services_input_start
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static MfwHnd services_input_start(MfwHnd parent,T_input_properties *properties)
{

  T_MFW_HND win;
  TRACE_FUNCTION ("services_input_start()");

  win = services_input_create (parent);

  if (win NEQ NULL)
  {
    SEND_EVENT (win, SERVICES_INPUT_INIT, 0, properties);
  }
  return win;
}


/*******************************************************************************

 $Function:    	services_input
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void services_input (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_services_input     * data = (T_services_input *)win_data->user;

	T_input_properties* properties = (T_input_properties*) parameter;
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;
#endif /* NEW_EDITOR */

    TRACE_FUNCTION ("T_services_input()");

    switch (event)
	{
		case SERVICES_INPUT_INIT:
			/* SPR#1428 - SH - New Editor changes - replace old custom editor
			 * with a standard AUI Editor */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&editor_data);
			AUI_edit_SetTextStr(&editor_data, properties->left_soft_key, properties->right_soft_key, properties->text, NULL);
			AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, properties->callback);
			//Oct 20, 2004  REF: CRR 25772  Deepa M.D
			//The alternate text string when the editor content is empty is set and
			//the change_rsk_on_empty flag is set to true.
			AUI_edit_SetAltTextStr(&editor_data, 1, TxtNames, TRUE, TxtSoftBack);
			AUI_edit_SetDisplay(&editor_data, SERVICE_CREATE_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
		
			if (properties->abc)
			{
				AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_name, sizeof(ss_data.edt_buf_name)); 
			}
			else
			{
				AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_number, sizeof(ss_data.edt_buf_number)); 

			}
			TRACE_EVENT("SERVICES_INPUT_INIT");

			data->edit_win =  AUI_edit_Start(data->win, &editor_data);

#else /*NEW_EDITOR*/

			data->properties.abc	= properties->abc;
			data->properties.text	= properties->text;
			data->properties.edt_attr_input	= properties->edt_attr_input;
			data->properties.left_soft_key = properties->left_soft_key;
			data->properties.right_soft_key = properties->right_soft_key;
			data->properties.callback = properties->callback;
			data->edt_input = edtCreate(data->win,data->properties.edt_attr_input,0,0);
			editDeactivate();
			editActivate (data->edt_input,properties->abc);
			edtUnhide(data->edt_input);
			winShow(data->win);
#endif /* NEW_EDITOR */
			break;
		
		case SERVICES_INPUT_DESTROY:
            TRACE_FUNCTION("SERVICES_INPUT_DESTROY");
			services_input_destroy(data->win);
			break;

		case DIVERT_PHBK_NUMBER:
			/* SPR#1428 - SH - New Editor changes - buffer changed, send event to
			 * update editors */
#ifdef NEW_EDITOR
			{
				T_ATB_TEXT text;
				text.dcs = ATB_DCS_ASCII;
				text.data = (UBYTE *)parameter;
				text.len = ATB_string_Length(&text);
				SEND_EVENT(data->edit_win, E_ED_INSERT, 0, (void *)&text);
			}
#else /* NEW_EDITOR */
		    strncpy(ss_data.edt_buf_number, (char *)parameter, strlen((char *)parameter)+1);
#endif /* NEW_EDITOR */
			break;
		default:
		return;
	}

}


/*******************************************************************************

 $Function:    	services_input_win_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static int services_input_win_cb (MfwEvt e, MfwWin *w){
/* Warning Removal - x0020906 - 24-08-2006 */
#ifndef NEW_EDITOR
	T_services_input * data = (T_services_input *)w->user;
#endif

	//int i =0;   // RAVI

    TRACE_FUNCTION ("services_input_win_cb()");

    /* SPR#1428 - SH - New Editor changes - this window is now
     * invisible, AUI editor sits on top*/
#ifdef NEW_EDITOR
	return 0;
#else /* NEW_EDITOR */
	switch (e)
    {
        case MfwWinVisible:
			edtShow(data->edt_input);
	        PROMPT(menuArea.px,menuArea.py,0,data->properties.text);
			displaySoftKeys(data->properties.left_soft_key,data->properties.right_soft_key);
		break;
        default:
		return 0;
    }
#endif /* NEW_EDITOR */

   /* return 1; */ /* Unreachable Code - x0020906 - 24-08-2006 */

}


/* SPR#1428 - SH - New Editor changes - no longer required*/
#ifndef NEW_EDITOR
/*******************************************************************************

 $Function:    	services_input_kbd_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

static int services_input_kbd_cb (MfwEvt e, MfwKbd *k){


    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_services_input * data = (T_services_input *)win_data->user;

    TRACE_FUNCTION ("services_input_kbd_cb()");
    
	if (activeEditor() NEQ data->edt_input)
		editActivate(data->edt_input,data->properties.abc);

    switch (k->code)
    {
        case KCD_MNUUP:
            edtChar(data->edt_input,ecRight);
        break;
        
        case KCD_MNUDOWN:
            edtChar(data->edt_input,ecLeft);
		break;
		
        case KCD_LEFT:
            TRACE_FUNCTION("left key pressed");
			data->properties.callback(data->parent_win,SERVICES_INPUT_LEFT);
		break;
		
        case KCD_HUP:
            /* if edit field is empty then exit, else delete one char */
			data->properties.callback(data->parent_win,SERVICES_INPUT_CLEAR);

            if (strlen(data->properties.edt_attr_input->text) > 0)
            {
        		edtChar(data->edt_input,ecBack);
    			if(strlen(data->properties.edt_attr_input->text) == 0)
					data->properties.right_soft_key=TxtContacts;
        		else
        			data->properties.right_soft_key=TxtDelete;
        		winShow(data->win);
        	}
            else
                services_input_destroy(data->win);
		break;
		
	    case KCD_RIGHT:
	    	// Check if there is a digit in the edit buffer softkey should be delete.
            if (strlen(data->properties.edt_attr_input->text) > 0)
            {
        		edtChar(data->edt_input,ecBack);
    			if(strlen(data->properties.edt_attr_input->text) == 0)
    			{
					data->properties.right_soft_key=TxtContacts;
        			winShow(data->win);
        		}
        	}
	    	else
	    	{
	    		// MZ 26/02/01 services_input_destroy(data->win);
				// Activate phone book .
				if (bookPhoneBookLoading()==BOOK_SUCCESS)
				{
					T_MFW_HND phbk_win;
					phbk_win = bookPhonebookStart(data->win,PhbkFromDivert);
				}
				
	    	}
            
        break;
        default:
	        editEventKey (e,k);
            if(strlen(data->properties.edt_attr_input->text) > 0)
				data->properties.right_soft_key=TxtDelete;
            else
            	data->properties.right_soft_key=TxtContacts;
            winShow(data->win);

        break;
    }
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	services_input_kbd_long_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/




static int services_input_kbd_long_cb (MfwEvt e, MfwKbd *k){

    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_services_input * data = (T_services_input *)win_data->user;


			if ((e & KEY_CLEAR) && (e & KEY_LONG))
			{
				edtClear(data->edt_input);
				services_input_destroy(data->win);
				return MFW_EVENT_CONSUMED;
			}
		return  MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	services_number_input_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void services_number_input_cb(T_MFW_HND win, UBYTE identifier, USHORT reason){

	switch(reason){

	case SERVICES_INPUT_LEFT:
		break;
	case SERVICES_INPUT_RIGHT:
		break;
	case SERVICES_INPUT_CLEAR:
		break;
	default:
	return;
	}
}

#endif /* ifndef NEW_EDITOR */


/*******************************************************************************

 $Function:    	servicesCallBarringCheck
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesCallBarringCheck(void)
{
    char SS_string[60] = {'\0'};

    TRACE_FUNCTION("servicesCallBarringCheck");

    CBAction = CBCheck;

	getCBAction(SS_string);
	getCBType(SS_string);

//x0pleela 23 Aug, 2006 DR: OMAPS00090419
//Removing the below code as it doesn't result in a valid SS.
//x0pleela 25 May, 2006  DR: OMAPS00070657
//#ifdef FF_CPHS
//	getCBService(SS_string);
//#endif
    SS_string[strlen(SS_string)-1] = '#';

    TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

}

/*******************************************************************************

 $Function:    	servicesCallBarringDeactivate
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void servicesCallBarringDeactivate(void)
{
    char SS_string[60] = {'\0'};

    TRACE_FUNCTION("servicesCallBarringDeactivate");

    CBAction = CBDeactivate;

	getCBAction(SS_string);
	getCBType(SS_string);
	getCBPassword(SS_string);

//x0pleela 23 Aug, 2006 DR: OMAPS00090419
//Removing the below code as it doesn't result in a valid SS.
//x0pleela 25 May, 2006  DR: OMAPS00070657
//#ifdef FF_CPHS
//	getCBService(SS_string);
//#endif

	//x0pleela 03 May, 2007  DR: OMAPs00129383
	//Replaced BASIC_SERVICE_CODE with TELETELE_BEARER_SERVICE_CODE
	//to deactivate both Tele and Bearere services
	
	//June 21, 2004 - CRR:13650 - CBA : no basic service sent when using the menu
	//Adding the basic service code 10(all teleservices) to the SS string during deactivation
	strcat(SS_string,TELE_BEARER_SERVICE_CODE);
	SS_string[strlen(SS_string)] = '#';

    TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );


}

//Jun 02,2004 CRR:13649 xpradipg-SASKEN
/*******************************************************************************

 $Function:    	servicesCallBarringDeactivatePassword
 
 $Description:	Sets the flag for deactivating the selected barring service and prompts for the password
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void servicesCallBarringDeactivatePassword(void)
{

	CBFlag=CBDeactivate;
	requestCBPassword();
}

/*******************************************************************************

 $Function:    	servicesCallBarringActivate
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesCallBarringActivate(void)
{
    char SS_string[60] = {'\0'};

    TRACE_FUNCTION("servicesCallBarringActivate"); //SPR#1038 - DS - Changed from DeActivate to Activate.

    CBAction = CBActivate;

	getCBAction(SS_string);
	getCBType(SS_string);
	getCBPassword(SS_string);
//x0pleela 23 Aug, 2006 DR: OMAPS00090419
//Removing the below code as it doesn't result in a valid SS.
//x0pleela 25 May, 2006  DR: OMAPS00070657
//#ifdef FF_CPHS
//	getCBService(SS_string);
//#endif

	//x0pleela 03 May, 2007  DR: OMAPs00129383
	//Replaced BASIC_SERVICE_CODE with TELETELE_BEARER_SERVICE_CODE
	//to activate both Tele and Bearere services

	//June 21, 2004 - CRR:13650 - CBA : no basic service sent when using the menu
	//Adding the basic service code 10(all teleservices) to the SS string during activation
	strcat(SS_string,TELE_BEARER_SERVICE_CODE);
	SS_string[strlen(SS_string)] = '#';

    TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

}

//Jun 02,2004 CRR:13649 xpradipg-SASKEN
/*******************************************************************************

 $Function:    	servicesCallBarringActivatePassword
 
 $Description:	Sets the flag for activating the selected barring service and prompts for the password
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/

void servicesCallBarringActivatePassword(void)
{

	CBFlag=CBActivate;
	requestCBPassword();
}


/*******************************************************************************

 $Function:    	servicesCallBarringCancelAll
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void servicesCallBarringCancelAll(void)
{
    char SS_string[60] = {'\0'};

    TRACE_FUNCTION("servicesCallBarringCancelAll");

	
    strcat(SS_string, "#330*");
	getCBPassword(SS_string);
    SS_string[strlen(SS_string)-1] = '#';

    TRACE_FUNCTION(SS_string);

	if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
        showFailed( ss_data.win );

}

//Jun 02,2004 CRR:13649 xpradipg-SASKEN
/*******************************************************************************

 $Function:    	servicesCallBarringCancelAllPassword
 
 $Description:	Sets the flag for cancelling all barring service and prompts for the password
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
void servicesCallBarringCancelAllPassword(void)
{
	CBFlag=CBCancelAll;
	requestCBPassword();
}


/*******************************************************************************

 $Function:    	getCBAction
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCBAction(char * pSSstring)
{
    switch (CBAction)
    {
    case CBActivate:
        strcpy(pSSstring, "*");
        break;
    case CBDeactivate:
        strcpy(pSSstring, "#");
        break;
    case CBCheck:
        strcpy(pSSstring, "*#");
        break;
    default:
        break;
    }
}

/*******************************************************************************

 $Function:    	getCBType
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCBType(char* pSSstring)
{


    switch (CBType)
    {
    case CBAllOutgoing:
        strcat(pSSstring, "33*");
        break;
    case CBInternational:
        strcat(pSSstring, "331*");
        break;
    case CBAllIncoming:
        strcat(pSSstring, "35*");
        break;
    case CBInternationalHome:
        strcat(pSSstring, "332*");
        break;
    case CBIncomingRoaming:
        strcat(pSSstring, "351*");
        break;
    default:
        break;
    }
}

/*******************************************************************************

 $Function:    	getCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


static void getCBPassword(char* pSSstring)
{
	TRACE_FUNCTION("getCBPassword");
        strcat(pSSstring, (char*)ss_data.edt_buf_cb_password);
        strcat(pSSstring, "*");
}

#if(0)  /* x0039928 - Lint warning removal */
/*******************************************************************************

 $Function:    	 getCBService

 $Description:	
            
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
//x0pleela 25 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
static void getCBService(char * pSSstring)
{
	int nservice;
	nservice =GetAlsLine();
    switch (nservice)
    {
    case CBVoice:
        strcat(pSSstring, "*11"); //SPR#0776 - DS -changed from 10 to 11
        break;
    /*MC SPR 742, handle line 2, auxiliary telephony*/
    case CBAuxVoice:
		strcat(pSSstring, "*89"); 
        break;
    default:
        break;
    }
}
#endif
#endif

//CQ-16432 start
/*******************************************************************************

 $Function:    	requestSatCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void requestSatCBPassword(void)
{

   
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data; /* SPR#1428 - SH - New Editor changes */
#else /* NEW_EDITOR */
	T_EDITOR_DATA editor_data;
#endif /* NEW_EDITOR */
	 T_MFW_HND 		win = mfw_parent(mfw_header());
	 /* T_MFW_HND		satPswdWin; */ /* Not Used - warning removal */ /*x0020906 - 24-08-2006 */
	TRACE_FUNCTION("requestSatCBPassword");
	//SPR#1014 - DS - Reset editor data
	memset(&editor_data, '\0', sizeof(editor_data));
	
    /*               
     * Setup an editor to display a string                
     */
     
	memset(satPassword,'\0',sizeof(satPassword));

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	TRACE_FUNCTION("New editor is defined");
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, RPWD_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetMode(&editor_data, ED_MODE_HIDDEN, ED_CURSOR_UNDERLINE);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)satPassword, CB_PSW_LEN+1);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtPassword, NULL);
	AUI_edit_SetAltTextStr(&editor_data, CB_PSW_LEN, TxtNull, FALSE, TxtNull);
	AUI_edit_SetEvents(&editor_data, CBEnterSatPassword, TRUE, FOREVER, (T_AUI_EDIT_CB)CB_password_cb);
	/* satPswdWin = AUI_edit_Start(win, &editor_data);  */ /* start the editor */  /* Warning Removal - x0020906 - 24-08-2006*/
       AUI_edit_Start(win, &editor_data);  /* start the editor */
	TRACE_EVENT_P1("satPassword contents: %s",satPassword);
#else /* NEW_EDITOR */
	TRACE_FUNCTION("New editor is not defined");
	bookSetEditAttributes( RPWD_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
		(char*)ss_data.edt_buf_cb_password,CB_PSW_LEN+1, &editor_data.editor_attr );
     editor_data.LeftSoftKey          = TxtSoftOK;               
     editor_data.AlternateLeftSoftKey = TxtNull;                
     editor_data.RightSoftKey         = TxtSoftBack;               
     editor_data.hide                 = TRUE;               
     editor_data.mode                 = DIGITS_MODE;                
     editor_data.timeout              = FOREVER;                
     editor_data.Identifier           = CBCheckPassword;   
     editor_data.destroyEditor        = TRUE;
     editor_data.Callback   = (T_EDIT_CB)CB_password_cb;               
     editor_data.TextId = TxtPassword;
     editor_data.TextString       =   NULL;
     editor_data.min_enter  = CB_PSW_LEN; 							 	               					  
     editor_data.destroyEditor = TRUE;
     ss_data.info_win = editor_start(ss_data.win, &editor_data);               
#endif /* NEW_EDITOR */

}
//CQ-16432 end

/*******************************************************************************

 $Function:    	requestCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



void requestCBPassword(void)
{
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data; /* SPR#1428 - SH - New Editor changes */
#else /* NEW_EDITOR */
	T_EDITOR_DATA editor_data;
#endif /* NEW_EDITOR */
TRACE_FUNCTION("requestCBPassword");
	//SPR#1014 - DS - Reset editor data
	memset(&editor_data, '\0', sizeof(editor_data));
	
    /*               
     * Setup an editor to display a string                
     */
     
	memset(ss_data.edt_buf_cb_password,'\0',sizeof(ss_data.edt_buf_cb_password));

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
TRACE_FUNCTION("NEW_EDITOR");
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, RPWD_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetMode(&editor_data, ED_MODE_HIDDEN, ED_CURSOR_UNDERLINE);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_cb_password, CB_PSW_LEN+1);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtPassword, NULL);
	AUI_edit_SetAltTextStr(&editor_data, CB_PSW_LEN, TxtNull, FALSE, TxtNull);
	AUI_edit_SetEvents(&editor_data, CBCheckPassword, TRUE, FOREVER, (T_AUI_EDIT_CB)CB_password_cb);

	ss_data.info_win = AUI_edit_Start(ss_data.win, &editor_data);  /* start the editor */
#else /* NEW_EDITOR */
TRACE_FUNCTION("Old_editor");
	bookSetEditAttributes( RPWD_EDITOR, COLOUR_EDITOR_XX, 0, edtCurBar1, 0, 
		(char*)ss_data.edt_buf_cb_password,CB_PSW_LEN+1, &editor_data.editor_attr );
     editor_data.LeftSoftKey          = TxtSoftOK;               
     editor_data.AlternateLeftSoftKey = TxtNull;                
     editor_data.RightSoftKey         = TxtSoftBack;               
     editor_data.hide                 = TRUE;               
     editor_data.mode                 = DIGITS_MODE;                
     editor_data.timeout              = FOREVER;                
     editor_data.Identifier           = CBCheckPassword;   
     editor_data.destroyEditor        = TRUE;
     editor_data.Callback   = (T_EDIT_CB)CB_password_cb;               
     editor_data.TextId = TxtPassword;
     editor_data.TextString       =   NULL;
     editor_data.min_enter  = CB_PSW_LEN; 							 	               					  
     editor_data.destroyEditor = TRUE;
     ss_data.info_win = editor_start(ss_data.win, &editor_data);               
#endif /* NEW_EDITOR */
}

/*******************************************************************************

 $Function:    	CB_password_cb
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/



static void CB_password_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
//16432
	TRACE_FUNCTION("CB_password_cb");
        switch (identifier)
        {
			case CBCheckPassword:
				//Jun 02,2004 CRR:13649 xpradipg-SASKEN
				//keyevents right and hangup are handled initially
				switch (reason)				
				{
				    case INFO_KCD_HUP:
				// Change by Sasken ( VadiRaj) on March 29th 2004
				// Issue Number : MMI-SPR-12882
				// Subject: No functionality for the right softkey.
				// Bug : No functionality for the right softkey
				// Solution : Added functionality for the right "BACK" softeky
					case INFO_KCD_RIGHT: 
						ss_data.info_win = 0;
						CBFlag=CBActionNone;;
				    break;
				    default:
				    break;
				}
				//Jun 02,2004 CRR:13649 xpradipg-SASKEN
				//depending on the action selected by the user the respective functions are invoked
				switch(CBFlag)
				{
					case CBDeactivate:
						CBFlag=CBActionNone;				//reset the flag 
						servicesCallBarringDeactivate();	//initiates the callbarring deactivation for the selected service
						break;
					case CBActivate:
						CBFlag=CBActionNone;				//reset the flag
						servicesCallBarringActivate();	//initiates the callbarring activation for the selected service
						break;
					case CBCancelAll:
						CBFlag=CBActionNone;				//reset the flag
						servicesCallBarringCancelAll();	//initiates the cancelling of all barring services
						break;
					case CBNewPassword:
						CBFlag=CBActionNone;				//reset the flag
						enterNewCBPassword();			//prompt for the new password 
				    break;
				    default:
						CBFlag=CBActionNone;;
				    break;
				}
				ss_data.info_win = 0;
			break;				
			case CBNewPassword:
				ss_data.info_win = 0;
				switch (reason)				
				{
				    case INFO_KCD_LEFT:
						confirmNewCBPassword();
				    break;
				    default:
				    break;
				 }
			break;
			case CBConfirmNewPassword:
				ss_data.info_win = 0;
				switch (reason)				
				{
				    case INFO_KCD_LEFT:
						changeCBPassword();
				    break;
				    default:
				    break;
				}
			break;
		//CQ-16432 start
	     case CBEnterSatPassword:
		/*a0393213 warnings removal- CLCK_FAC_Ox changed to FAC_Ox as a result of ACI interface change*/
		ss_set_clck(FAC_Ox, CLCK_MOD_NotPresent, satPassword, CLASS_NotPresent, MFW_MODUL_UNKNOWN);
 	      //CQ-16432 end
 	      break;
            default:
            break;
        }
}

//Jun 02,2004 CRR:13649 xpradipg-SASKEN
/*******************************************************************************
 $Function:    	enterOldCBPassword
 
 $Description:	sets flag for changing the barring password and prompts to enter the current 
 			barring password
  $Returns:		

 $Arguments:	
 
*******************************************************************************/

void enterOldCBPassword(void)
{
	CBFlag=CBNewPassword;
	requestCBPassword();
}

/*******************************************************************************

 $Function:    	enterNewCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void enterNewCBPassword(void)
{
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	//SPR#1014 - DS - Reset editor data
	memset(&editor_data, '\0', sizeof(editor_data));
	memset(ss_data.edt_buf_cb_new_password,'\0',sizeof(ss_data.edt_buf_cb_new_password));

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, RPWD_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetMode(&editor_data, ED_MODE_HIDDEN, ED_CURSOR_UNDERLINE);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_cb_new_password, CB_PSW_LEN+1);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtNewPassword, NULL);
	AUI_edit_SetAltTextStr(&editor_data, CB_PSW_LEN, TxtNull, FALSE, TxtNull);
	AUI_edit_SetEvents(&editor_data, CBNewPassword, TRUE, FOREVER, (T_AUI_EDIT_CB)CB_password_cb);

	ss_data.info_win = AUI_edit_Start(ss_data.win, &editor_data);  /* start the editor */
#else /* NEW_EDITOR */
	bookSetEditAttributes( RPWD_EDITOR, 0, 0, edtCurBar1, 0, 
		(char*)ss_data.edt_buf_cb_new_password,CB_PSW_LEN+1, &editor_data.editor_attr );
     editor_data.LeftSoftKey          = TxtSoftOK;               
     editor_data.AlternateLeftSoftKey = TxtNull;                
     editor_data.RightSoftKey         = TxtDelete;               
     editor_data.hide                 = TRUE;               
     editor_data.mode                 = DIGITS_MODE;                
     editor_data.timeout              = FOREVER;                
     editor_data.Identifier           = CBNewPassword;  
     editor_data.destroyEditor        = TRUE;
     editor_data.Callback   = (T_EDIT_CB)CB_password_cb;               
     editor_data.TextId = TxtNewPassword;
     editor_data.TextString       =   NULL;
     editor_data.min_enter  = CB_PSW_LEN; 							 	               					  
     editor_data.destroyEditor = TRUE;
     ss_data.info_win = editor_start(ss_data.win, &editor_data);      
#endif /* NEW_EDITOR */
}

/*******************************************************************************

 $Function:    	confirmNewCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/


void confirmNewCBPassword(void)
{
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;	/* SPR#1428 - SH - New Editor data */
#else
	T_EDITOR_DATA editor_data;
#endif

	//SPR#1014 - DS - Reset editor data.
	memset(&editor_data, '\0', sizeof(editor_data));

	memset(ss_data.edt_buf_cb_conf_new_password,'\0',sizeof(ss_data.edt_buf_cb_conf_new_password));

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, RPWD_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetMode(&editor_data, ED_MODE_HIDDEN, ED_CURSOR_UNDERLINE);
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)ss_data.edt_buf_cb_conf_new_password, CB_PSW_LEN+1);
	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtSoftBack, TxtConfirmPassword, NULL);
	AUI_edit_SetAltTextStr(&editor_data, CB_PSW_LEN, TxtNull, FALSE, TxtNull);
	AUI_edit_SetEvents(&editor_data, CBConfirmNewPassword, TRUE, FOREVER, (T_AUI_EDIT_CB)CB_password_cb);

	ss_data.info_win = AUI_edit_Start(ss_data.win, &editor_data);  /* start the editor */
#else /* NEW_EDITOR */
	bookSetEditAttributes(  RPWD_EDITOR, 0, 0, edtCurBar1, 0, 
		(char*)ss_data.edt_buf_cb_conf_new_password,CB_PSW_LEN+1, &editor_data.editor_attr );
	editor_data.LeftSoftKey          = TxtSoftOK;               
     editor_data.AlternateLeftSoftKey = TxtNull;                
     editor_data.RightSoftKey         = TxtDelete;               
     editor_data.hide                 = TRUE;               
     editor_data.mode                 = DIGITS_MODE;                
     editor_data.timeout              = FOREVER;                
     editor_data.Identifier           = CBConfirmNewPassword;  
     editor_data.destroyEditor        = TRUE;
     editor_data.Callback   = (T_EDIT_CB)CB_password_cb;               
     editor_data.TextId = TxtConfirmPassword;
     editor_data.TextString       =   NULL;
     editor_data.min_enter  = CB_PSW_LEN; 							 	               					  
     editor_data.destroyEditor = TRUE;    
     ss_data.info_win = editor_start(ss_data.win, &editor_data);       
#endif /* NEW_EDITOR */

}

/*******************************************************************************

 $Function:    	changeCBPassword
 
 $Description:	
 
 $Returns:		

 $Arguments:	

*******************************************************************************/


void changeCBPassword(void){

    char SS_string[60] = {'\0'};
    T_DISPLAY_DATA   display_info;

    TRACE_FUNCTION("changeCBPassword");

	if (strcmp((char*)ss_data.edt_buf_cb_new_password, (char*)ss_data.edt_buf_cb_conf_new_password))
	{
		/*
		** The New and Confirm Passwords do not match, restart the process and display a dialog to
		** explain. (The dialog is done second so that it will display on top of the new prompt)
		*/
		enterNewCBPassword();

		// Jul 23, 2004        REF: CRR 16107  xvilliva
		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtPasswords,  TxtMismatch, COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)services_info_cb, THREE_SECS, 0);
		display_info.Identifier   = ServicesFailed;

		/*
		* Call Info Screen
		*/
		ss_data.info_win = info_dialog (ss_data.win, &display_info);
	}
	else
	{
		strcat(SS_string, "**03*330*");
		strcat(SS_string,(char*)ss_data.edt_buf_cb_password);
		strcat(SS_string,"*");
		strcat(SS_string,(char*)ss_data.edt_buf_cb_new_password);
		strcat(SS_string,"*");
		strcat(SS_string,(char*)ss_data.edt_buf_cb_conf_new_password);
		strcat(SS_string,"#");

		TRACE_FUNCTION(SS_string);

		if (ss_execute_transaction((UBYTE*)SS_string, 0) == MFW_SS_FAIL)
			showFailed( ss_data.win );
	}
	

}


/*******************************************************************************

 $Function:    	featurePlaceholder
 
 $Description:		Display screen dialog for not-yet-implemented features
 
 $Returns:		None	

 $Arguments:		None

 //SPR#1113 - DS - Created function.

*******************************************************************************/

void featurePlaceholder(void){

  T_DISPLAY_DATA      display_info ;

  TRACE_FUNCTION("featurePlaceholder");

  display_info.LeftSoftKey  = TxtSoftOK;
  display_info.RightSoftKey = 0;
  display_info.TextId      = 0;
  display_info.TextId2      = 0;
  display_info.TextString = "Feature";
  display_info.TextString2 = "Not Available";
  display_info.Time         = THREE_SECS;
  display_info.Identifier   = 0;
  display_info.Callback     = NULL;
  display_info.KeyEvents    = KEY_CLEAR | KEY_LEFT | KEY_MNUUP | KEY_MNUDOWN;

  /*
   * Call Info Screen
   */

  info_dialog(win, &display_info);
  
}
