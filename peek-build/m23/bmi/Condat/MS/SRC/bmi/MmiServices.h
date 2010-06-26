/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Services
 $File:		    MmiServices.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
                        
********************************************************************************

 $History: MmiServices.h
       May 30, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Call Forwarding feature implementation
       Solution     : As part of implementation, in this file, some #defines and structure definitions are moved from
                         MmiServices.c to MmiServices.h
	Jun 02,2004 CRR:13649 xpradipg-SASKEN
	subject: CBA : Management of password
  	solution: password is promted on selecting activation/deactivation option as against the previous
  	case where it was shown up on selecting CallBarring option in the Network Services menu.
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

#ifndef _DEF_MMI_SERVICES_H_
#define _DEF_MMI_SERVICES_H_

/*
 * External Interfaces
 */



/*
 * Top Window Defintions
 */
T_MFW_HND   services_create        (T_MFW_HND parent_window);
void        services_destroy       (T_MFW_HND own_window);
void        services               (T_MFW_HND win, USHORT event, SHORT value, void * parameter);




/*
 * Event definitions
 */
#define     SS_OK                   100
#define     SS_FAILED               101
#define     SS_ERROR                102
#define     SS_WAIT                 103
#define     SS_DO_CALL_DIVERT       104
#define     SS_USSD_ENTER_NUMBER    105
#define     SS_USSD_LIST_FULL       106

#define     SS_CB_CNF               110

                                        /* REASONS                  */
typedef enum
{
	ServicesIdle = ServicesDummy,
	ServicesOk,
	ServicesFailed,
	ServicesActCW,
	ServicesDeactCW,
	ServicesCheckCW,
	ServicesCheckCW_OK,
    ServicesActCF,
    ServicesDeactCF,
    ServicesCheckCF,
	ServicesCFU_OK,
    ServicesCB_OK,
    ServicesExecCF,
    ServicesDoCallDivert,
	ServicesResShow,
    ServicesActCLIR,
	ServicesDeactCLIR,
	ServicesCheckCLIR,
	ServicesCheckCLI_OK,
    ServicesIMEI_OK,
    ServicesUSSD_REQ_OK,
    ServicesUSSD_CNF_OK,
    ServicesUSSDEnter,
    ServicesUSSDEnter_Send,
    ServicesUSSDNew,
    ServicesUSSDSendEdit,
    ServicesUSSDSendEdit_Ok,
    ServicesUSSDAlreadyFull,
    ServicesUSSDListMenu,
    ServicesUSSDOptions,
    ServicesUSSDChange,
    ServicesUSSDChange_OK,
    ServicesUSSDDelete,
    ServicesOtherNumber,
    SERVICES_INPUT_INIT,
    SERVICES_INPUT_LEFT,
    SERVICES_INPUT_RIGHT,
    SERVICES_INPUT_CLEAR,
    SERVICES_INPUT_DESTROY,
    SERVICES_DO_CALL_VOICE_DIVERT,
    ServicesError

}E_SERVICES;

/* 30th May,2006     OMAPS00079650     a0393213 (R.Prabakar)
    Description : CPHS Call Forwarding feature implementation
    Solution     : As part of this implementation, some #defines and struct definitions are moved from MmiServices.c*/
    
#define MAX_SS_CF_NUM_LEN 16
#define CB_PSW_LEN  4

typedef struct
{
  USHORT service;
  USHORT status;
  char   forwarded_to_number[MAX_SS_CF_NUM_LEN];
} T_ss_feature_cf_data;

typedef struct
{
  T_ss_feature_cf_data ss[MFW_MAX_FEAT_NR];
} T_ss_feature_cf;


typedef struct
{
  USHORT service;
} T_ss_feature_cw_data;

typedef struct
{
  USHORT               status;
  T_ss_feature_cw_data ss[MFW_MAX_FEAT_NR];
} T_ss_feature_cw;

typedef struct
{
  USHORT service;
  USHORT status;
} T_ss_feature_cb_data;

typedef struct
{
  T_ss_feature_cb_data ss[MFW_MAX_FEAT_NR];
} T_ss_feature_cb;

typedef struct
{
    T_MMI_CONTROL   mmi_control;
	T_MFW_HND       win;     // window handle
    T_MFW_HND       ss_handle;   /* MFW services handler  */
	T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
//    T_MFW_CM_AOC_INFO aocInfo;
    T_MFW_HND       info_win;  /* handle to info dialog */
    T_MFW_HND       info_wait;  /* handle to wait dialog */
    T_MFW_HND       cb_menu_win;  /* handle to ussd number dialog */
    T_MFW_HND       input_number_win;  /* handle to call forward dialog */

    USHORT            event;
  	char              edt_buf_name[MAX_ALPHA_LEN];/*MC SPR 1257*/
	char              edt_buf_number[PHB_MAX_LEN];
  	char              edt_buf_cb_password[CB_PSW_LEN+1];
  	char              edt_buf_cb_new_password[CB_PSW_LEN+1];
  	char              edt_buf_cb_conf_new_password[CB_PSW_LEN+1];
#ifndef NEW_EDITOR			/* SPR#1428 - SH - New Editor changes - no longer required */
	MfwEdtAttr        edt_attr_name;
    MfwEdtAttr        edt_attr_number;
#endif /* ifndef NEW_EDITOR */
    T_ss_feature_cf * mfw_ss_cf;
    T_ss_feature_cw * mfw_ss_cw;
    T_ss_feature_cb * mfw_ss_cb;
    UBYTE             current_feature;
    UBYTE             max_feature;
    UBYTE             dcs;  /* need it for sending ussd to mfw */

	/*
	** NDH : 28/08/2003
	**
	** cfu_services is a binary flag to determine which (if any) of the Call Frwd Uncond
	** services are active. The map is as follows :
	**		bit 0 : Voice Calls	( & 0x01)
	**		bit 1 : Fax Calls	( & 0x02)
	**		bit 2 : Data Calls	( & 0x04)
	**		bit 3 : SMS		( & 0x08)
	**		bit 4 : unused - for future expansion
	**		bit 5 : unused - for future expansion
	**		bit 6 : unused - for future expansion
	**		bit 7 : unused - for future expansion
	*/
	UBYTE             cfu_services;
} T_ss;

EXTERN T_ss ss_data;
#define SERVICESIDLE                    ServicesIdle
#define SERVICESOK                      ServicesOk
#define SERVICESFAILED                  ServicesFailed
#define SERVICESACTCW                   ServicesActCW
#define SERVICESDEACTCW                 ServicesDeactCW
#define SERVICESCHECKCW                 ServicesCheckCW
#define SERVICESCHECKCW_OK              ServicesCheckCW_OK
#define SERVICESACTCF                   ServicesActCF
#define SERVICESDEACTCF                 ServicesDeactCF
#define SERVICESCHECKCF                 ServicesCheckCF
#define SERVICESCFU_OK                  ServicesCFU_OK
#define SERVICESCB_OK                   ServicesCB_OK
#define SERVICESEXECCF                  ServicesExecCF
#define SERVICESDOCALLDIVERT            ServicesDoCallDivert
#define SERVICESRESSHOW                 ServicesResShow
#define SERVICESACTCLIR                 ServicesActCLIR
#define SERVICESDEACTCLIR               ServicesDeactCLIR
#define SERVICESCHECKCLIR               ServicesCheckCLIR
#define SERVICESCHECKCLI_OK             ServicesCheckCLI_OK
#define SERVICESIMEI_OK                 ServicesIMEI_OK
#define SERVICESUSSD_REQ_OK             ServicesUSSD_REQ_OK
#define SERVICESUSSD_CNF_OK             ServicesUSSD_CNF_OK
#define SERVICESUSSDENTER               ServicesUSSDEnter
#define SERVICESUSSDENTER_SEND          ServicesUSSDEnter_Send
#define SERVICESUSSDNEW                 ServicesUSSDNew
#define SERVICESUSSDSENDEDIT            ServicesUSSDSendEdit
#define SERVICESUSSDSENDEDIT_OK         ServicesUSSDSendEdit_Ok
#define SERVICESUSSDALREADYFULL         ServicesUSSDAlreadyFull
#define SERVICESUSSDLISTMENU            ServicesUSSDListMenu
#define SERVICESUSSDOPTIONS             ServicesUSSDOptions
#define SERVICESUSSDCHANGE              ServicesUSSDChange
#define SERVICESUSSDCHANGE_OK           ServicesUSSDChange_OK
#define SERVICESUSSDDELETE              ServicesUSSDDelete
#define SERVICESERROR                   ServicesError


int servicesNumbers(MfwMnu* m, MfwMnuItem* i);
int servicesInfraRed(void);

int servicesActivateCLIR(MfwMnu* m, MfwMnuItem* i);
int servicesDeactivateCLIR(MfwMnu* m, MfwMnuItem* i);
int servicesCheckCLIR(MfwMnu* m, MfwMnuItem* i);

int servicesCheckCWaiting(MfwMnu* m, MfwMnuItem* i);
int servicesActivateCWaiting(MfwMnu* m, MfwMnuItem* i);
int servicesDeactivateCWaiting(MfwMnu* m, MfwMnuItem* i);
int servicesInterCFU(void);

int servicesActivateIRDA(MfwMnu* m, MfwMnuItem* i);
int servicesDeactivateIRDA(MfwMnu* m, MfwMnuItem* i);

T_MFW_HND servicesInit(T_MFW_HND parentWindow);
void servicesExit(void);
void services (T_MFW_HND win, USHORT event, SHORT value, void * parameter);

void servicesExec (int reason, MmiState next);




void servicesDivertActivate(void);
void servicesDivertDeactivate(void);
void servicesDivertCheck(void);
void servicesDivertDeactivate(void);
void servicesDivertVoice(void);
void servicesDivertFax(void);
void servicesDivertData(void);
void servicesDivertActionAll(void);
void servicesDivertBusy(void);
void servicesDivertNoAnswer(void);
void servicesDivertNotReachable(void);
void servicesDivertAllCalls(void);
void servicesDivertNoService(void);
void servicesDivertCancelAll(void);
void servicesDoVoiceMailNumber(void);
void servicesDoOtherNumber(void);

void servicesBarringAllOutgoing(void);
void servicesBarringInternational(void);
void servicesBarringInternationalHome(void);
void servicesBarringAllIncoming(void);
void servicesBarringIncomingRoaming(void);

void servicesCallBarringActivate(void);
void servicesCallBarringDeactivate(void);
void servicesCallBarringCheck(void);
void servicesCallBarringCancelAll(void);
//Jun 02,2004 CRR:13649 xpradipg-SASKEN - start
//Declaration for the functions added
void servicesCallBarringActivatePassword(void);
void servicesCallBarringDeactivatePassword(void);
void servicesCallBarringCancelAllPassword(void);
void enterOldCBPassword(void);
//Jun 02,2004 CRR:13649 xpradipg-SASKEN - end
void requestCBPassword(void);
//CQ-16432 start
void requestSatCBPassword(void);
//CQ-16432 end
void changeCBPassword(void);
void confirmNewCBPassword(void);
void enterNewCBPassword(void);


static void getCBPassword(char* pSSstring);
static void getCBType(char* pSSstring);
static void getCBAction(char * pSSstring);
static void CB_password_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);

#if 0
/* SH - This whole editor appears to be redundant */
void servicesUSSDDisplayList(void);
void servicesUSSDSend(void);
void servicesUSSDAddNew(void);
#endif

void featurePlaceholder(void); //SPR#1113 - DS - Added prototype for placeholder function for not-yet-implemented features.
#endif


