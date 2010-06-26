/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_cmi.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 3.11.98                      $Modtime:: 14.03.00 10:03   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_CMI

   PURPOSE : Internal definitions for call management of MMI framework


   $History:: mfw_cmi.h                                              $
 * 
 * *****************  Version 33  *****************
 * User: Vo           Date: 15.03.00   Time: 18:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: Remote call control and Abort CM command
 * 
 * *****************  Version 32  *****************
 * User: Vo           Date: 17.02.00   Time: 19:07
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 31  *****************
 * User: Vo           Date: 20.01.00   Time: 12:34
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 30  *****************
 * User: Vo           Date: 22.12.99   Time: 15:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * change prototypes
 * 
 * *****************  Version 29  *****************
 * User: Le           Date: 13.12.99   Time: 8:50
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 28  *****************
 * User: Vo           Date: 11.11.99   Time: 14:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New prototypes define
 * 
 * *****************  Version 27  *****************
 * User: Le           Date: 11.10.99   Time: 16:27
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 26  *****************
 * User: Vo           Date: 30.08.99   Time: 13:30
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new prototypes 
 * 
 * *****************  Version 25  *****************
 * User: Vo           Date: 26.07.99   Time: 14:28
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * delete unused definitions
 * 
 * *****************  Version 24  *****************
 * User: Vo           Date: 21.07.99   Time: 14:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 23  *****************
 * User: Vo           Date: 16.07.99   Time: 10:33
 * Updated in $/GSM/DEV/MS/SRC/MFW

*/

#ifndef DEF_MFW_CMI_HEADER
#define DEF_MFW_CMI_HEADER

#define MAX_REDIAL_ATTEMPT 10
#define MAX_BLACKLIST_NUM  10

/* definition of commands for call related supplementary service */
#define CM_RELEASE       255

typedef enum     /* Call type definition */
{
  MFW_CALL_FREE = 0,
  MFW_NORMAL_CALL,     /* normal voice call */
  MFW_ECC_CALL         /* emergency call */
} T_MFW_CM_TYPE;

/* Flag for phone call status */
#define INCOMING_CALL  1
#define OUTGOING_CALL  2
#define REDIALING_CALL 3

#define MFW_CM_NO_FLAG         0
#define MFW_CM_MT_REJECT       1   /* mobile rejects a MT call */
#define MFW_CM_TERMINATED      2   /* mobile terminates a call */
#define MFW_CM_MPTY_TERMINATED 3   /* mobile terminates a call */
#define MFW_CM_MT_ACCEPT       4   /* mobile accepts a MT call */
#define MFW_CM_1_SEND          5
#define MFW_CM_HOLD_AND_DIAL   6   /* active to hold and dial  */
/* Marcus: CCBS: 14/11/2002: Start */
#define MFW_CM_CCBS_REGISTER   7   /* CCBS registration */
#define MFW_CM_CCBS_RECALL     8   /* CCBS recall */
/* Marcus: CCBS: 14/11/2002: END */

#define MFW_CM_DISCONNECT_ALL 9

/*
 *  CM Control Block
 */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_CM_PARA  para;
} T_MFW_CM;

typedef struct
{
	UBYTE count;			
	UBYTE number[MAX_BLACKLIST_NUM][MFW_NUM_LEN];
} T_MFW_CM_BLACKLIST_LIST;


/* Current call info */
typedef struct
{
    SHORT   call_number;
    UBYTE   number[MFW_NUM_LEN];
#ifdef NO_ASCIIZ  /* RM 8.9.2000 */
    T_MFW_PHB_TEXT name;              /* alpha identfier      */
#else
    UBYTE   name[MFW_TAG_LEN];        /* alpha identfier      */
#endif       
    UBYTE   subaddr[MFW_SUBADR_LEN];    
    UBYTE   type;             /* call type: OUTGOING_CALL ... */
    UBYTE   ton;
    UBYTE   prefix[MAX_LEN];
    T_MFW_CM_CALL_TYPE mode;  /* call mode                    */
} T_MFW_CM_CC_STAT;

T_MFW_HND cm_install              (T_MFW_HDR             * w, 
                                   T_MFW_HDR             * h, 
                                   T_MFW_CM              * cm);
T_MFW_RES cm_remove               (T_MFW_HDR             * h);
BOOL      cm_sign_exec            (T_MFW_HDR             * hdr,
                                   T_MFW_EVENT             event, 
                                   T_MFW_CM_PARA         * para);
void      cm_error_connect        (SHORT call_number);
void      cm_rmt_error_connect    (SHORT call_number);
void      cm_ok_connect           (void);
void      cm_ok_aoc               (T_ACI_AT_CMD cmdId);
void      cm_error_aoc            (T_ACI_AT_CMD cmdId, T_ACI_CME_ERR reason);
void      cm_rmt_ok_connect       (void);
void      cm_ok_disconnect        (T_ACI_CMD_SRC src, SHORT call_id);
void      cm_error_disconnect     (void);
void      cm_stop_redialTim       (void);
void      cm_result_cmd           (T_ACI_CMD_SRC src, UBYTE result);
void      cm_notify               (T_ACI_CSSU_CODE code,
                                   SHORT           index,
                                   UBYTE          *number);
T_MFW     cm_search_callStat      (SHORT call_number, T_MFW_CM_CALL_STAT *stat,
                                   UBYTE *ton, UBYTE *mode);
void      cm_AT_D                 (T_ACI_CLOG *cmd);
void      cm_AT_CHLD              (T_ACI_CLOG *cmd);
int       cm_search_callId        (T_ACI_CAL_STAT type, SHORT *id);
UBYTE     cm_error_code           (T_ACI_AT_CMD cmd);
void      cm_ok_dtmf              (void);
void      cm_error_dtmf           (void);
T_MFW_CM_CALL_TYPE cm_cvt_call_mode( T_ACI_CAL_MODE type );
void cm_set_call_mode             (T_MFW_CM_CALL_TYPE mode);
/*a0393213 warnings removal - parameters of cm_bc_parameter() and cm_build_callType() changed because of ACI enum change*/
void cm_bc_parameter              (T_ACI_CRING_SERV_TYP type);
void cm_build_callType            (T_ACI_CRING_SERV_TYP type1, T_ACI_CRING_SERV_TYP type2);
T_ACI_CAL_MODE cm_search_callMode (SHORT id);
void      cm_error_dial           (T_ACI_CME_ERR err);
void      cm_data_connect         (SHORT callId);
void      cm_wap_data_connected       (void);

#endif
