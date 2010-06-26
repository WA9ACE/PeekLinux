/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_ss.h        $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 25.1.99                      $Modtime:: 21.02.00 14:19   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SS

   PURPOSE : This modul contains the definition for supplementary management.


   $History:: mfw_ss.h                                              $

	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation

     	Feb 27, 2007 ER: OMAPS00113891 x0pleela
 	Description:[ACI] MTC related call forwarded SS notification not differentiable on 
 				BAT/ATI level
 	Solution: new enum value  MFW_SS_MT_CALL_FORWARDED (MT call is forwarded 
 				to another subscriber) is added for  T_MFW_SS_NOTIFY_CODES    

	Nov 13, 2006 DR: OMAPS00103356  x0pleela
 	Description: SIMP:Master unlock failed through MMI
 	Solution: Defined new enums:
		 	MFW_WRONG_PWD: will get returned if Master unlocking password is wrong
 			MFW_MASTER_OK: will get returned if Master unlocking operation is successful
 	
	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
	Description : Locosto: SIMP - integration of issues submitted by solDel on 
			     ME Personalization - changes as per the new interfaces
	Solution	:   The return value of sAT_PlusCLCK of AT_EXCT is handled and 
			    a please wait screen is displayed until the response is recieved
   
 * 
 * *****************  Version 14  *****************
 * User: Vo           Date: 22.02.00   Time: 14:09
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 13  *****************
 * User: Vo           Date: 10.01.00   Time: 16:09
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 12  *****************
 * User: Ak           Date: 10.01.00   Time: 13:14
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 11  *****************
 * User: Ak           Date: 21.10.99   Time: 11:56
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Why: Improvement
 * What: Passing of network basic service codes instead of GSM 2.30
 * codes for SS control string results and invocations
 * 
 * *****************  Version 10  *****************
 * User: Vo           Date: 12.09.99   Time: 13:38
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Bug fix: delete 'SS guidance'
 * New events, new structure and new prototype for USSD
 * 
 * *****************  Version 9  *****************
 * User: Vo           Date: 30.08.99   Time: 11:46
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new return value MFW_SS_DIAL_IDX
 * 
 * *****************  Version 8  *****************
 * User: Vo           Date: 9.08.99    Time: 16:48
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * change structure for SS results
 * add structure for IMEI
 * 
 * *****************  Version 7  *****************
 * User: Vo           Date: 21.07.99   Time: 14:45
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 6  *****************
 * User: Vo           Date: 9.07.99    Time: 14:57
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 9.07.99    Time: 14:37
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 4  *****************
 * User: Vo           Date: 2.07.99    Time: 15:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 3  *****************
 * User: Vo           Date: 20.05.99   Time: 17:49
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Vo           Date: 10.02.99   Time: 12:41
 * Created in $/GSM/DEV/MS/SRC/MFW

*/

#ifndef DEF_MFW_SS_HEADER
#define DEF_MFW_SS_HEADER

#include "mfw_nm.h"
#include "mfw_str.h"
#include "ksd.h"

#ifdef MAX_PWD_LEN /*a0393213 warnings removal-to prevent redefinition of MAX_PWD_LEN in some files*/
#undef MAX_PWD_LEN 
#endif
#define MAX_PWD_LEN     8

#define MFW_SS_NUM_LEN      21
#define MFW_SS_SUBADR_LEN   21
#define MFW_SS_PWD_LEN       5
 
#define MFW_MAX_FEAT_NR      5
#define MFW_MAX_TELE_NR      5
#define MFW_MAX_SRV_NR       5

/* events are organised as bitmaps */
#define E_SS_OK        0x0001
#define E_SS_CF_CNF    0x0002
#define E_SS_CB_CNF    0x0004
#define E_SS_GET_PW    0x0008
#define E_SS_CLI_CNF   0x0010
#define E_SS_CW_CNF    0x0020
#define E_SS_NOTIFY    0x0040
#define E_SS_RES       0x0080
#define E_SS_IMEI      0x0100
#define E_SS_USSD_REQ  0x0200
#define E_SS_USSD_CNF  0x0400
#define E_SS_CF_ICN_UPD 0x0800
//CQ-16432 start
#define E_SS_SAT_CPWD 0x1000
//CQ-16432 end

/* SS status is organised as bitmaps */
#define SS_STAT_ACTIVE      0x01
#define SS_STAT_REGISTERED  0x02
#define SS_STAT_PROVISIONED 0x04
#define SS_STAT_QUIESCENT  	0x08
#define SS_STAT_UNKNOWN		0xFF

/* SS codes */
typedef enum       
{
    SS_CODES_UNKNOWN,
    SS_CF_ALL,
    SS_CF_ALL_COND,
    SS_CF_CFU,
    SS_CF_CFNRy,
    SS_CF_CFNR,
    SS_CF_CFB,
    SS_CB_BAOC,
    SS_CB_BAOIC,
    SS_CB_BAOICexH,
    SS_CB_BAIC,
    SS_CB_BAICroam,
    SS_CB_ALL,
    SS_CB_ALL_BAOC,
    SS_CB_ALL_BAIC,
    SS_CW,
    SS_CCBS,
    SS_CLIP,
    SS_CLIR,
    SS_COLP,
    SS_COLR,
    SS_PIN1,
    SS_PIN2,
    SS_USSD
} T_MFW_SS_CODES;

/* SS category */
typedef enum
{
    SS_CTG_UNKNOWN,
    SS_REGISTRATION,
    SS_ERASURE,
    SS_ACTIVATION,
    SS_DEACTIVATION,
    SS_INTERROGATION
} T_MFW_SS_CATEGORY;

/* return values for type of key sequence */
typedef enum       
{
//x0pleela 23 Mar, 2007  ER : OMAPS00122561
#ifdef FF_PHONE_LOCK
	MFW_SS_ALRDY_DIS = 101, 
#endif /*FF_PHONE_LOCK*/
#ifdef SIM_PERS
//	Nov 03, 2005 DR: OMAPS00052032 - xpradipg
//	added new enum for execution in progress
	MFW_EXCT = 100,
	//x0pleela 13 Nov, 2006 DR: OMAPS00103356 
	//This will get returned if Master unlocking operation is successful
	MFW_MASTER_OK = 200,
	//x0pleela 13 Nov, 2006 DR: OMAPS00103356 
	//This will get returned if Master unlocking password is wrong
	MFW_WRONG_PWD = -2, 
	MFW_FAILURE = -1,
#endif
    MFW_SS_OK,
    MFW_SS_DIAL,
    MFW_SS_DIAL_IDX,
    MFW_SS_DTMF,
    MFW_SS_USSD,
    MFW_SS_SIM_LOCK,
    MFW_SS_SIM_REG_PW,
    MFW_SS_REG_PW,
    MFW_SS_SIM_UNBLCK_PIN,
    MFW_SS_REGISTER,
    MFW_SS_CF,
    MFW_SS_CB,  
    MFW_SS_CLIR,
    MFW_SS_CLIP,
    MFW_SS_COLR,
    MFW_SS_COLP,
    MFW_SS_WAIT,
    MFW_SS_MMI,    /* present IMEI */
    MFW_SS_HOLD,
    MFW_SS_ECT,/*MC*/
    MFW_SS_CCBS,
    MFW_SS_MULTIPARTY,
    MFW_SS_LANG_RESET, /*MC, SPR 1111*/
    MFW_SS_TTY_NEXTCALL_ON,	/* SPR#1352 - SH */
    MFW_SS_TTY_NEXTCALL_OFF,
    MFW_SS_UNKNOWN,
    MFW_SS_FAIL
} T_MFW_SS_RETURN;

/* data coding scheme */
typedef enum       
{
    MFW_DCS_7bits = 0x00,
    MFW_DCS_8bits = 0x04,
    MFW_DCS_UCS2 = 0x08,
    MFW_DCS_SIM,
    MFW_ASCII
}T_MFW_DCS;   

typedef enum
{
    MFW_CL_UNKNOWN,
    MFW_CL_PERM,    			/* permanent */
	MFW_CL_TEMP_DEF_REST,       /* temporary default restricted */
	MFW_CL_TEMP_DEF_ALLOWED    /* temporary default allowed */	
} T_MFW_CL_STATUS;
 
typedef enum
{
    MFW_OV_UNKNOWN,
    MFW_OV_ENABLED,   /* enabled */
  	MFW_OV_DISABLED   /* disabled */
} T_MFW_OV_CTGRY;

/* SS notification result code */
typedef enum
{
    MFW_SS_NOTIFY_UNKNOWN,
    MFW_SS_MO_unconFORWARD,  /* unconditional call forwaiting */
    MFW_SS_MO_conFORWARD,    /* conditional call forwaitings  */
    MFW_SS_MO_FORWARDED,     /* call forwarded                */
    MFW_SS_MO_WAITING,       /* waiting indicator             */
    MFW_SS_MO_CUG_CALL,      /* a CUG call                    */
    MFW_SS_MO_moBARRED,      /* outgoing calls are barred     */
    MFW_SS_MO_mtBARRED,      /* incoming calls are barred     */
    MFW_SS_MO_CLIR,          /* CLIR suppression reject       */
    MFW_SS_MO_DEFLECTED,     /* call deflected                */
    MFW_SS_MT_FORWARDED,     /* a forwarded call              */
    MFW_SS_MT_CUG_CALL,      /* a CUG call                    */
    MFW_SS_NOTIFY_MULTIPARTY,   /* multiparty call entered        */
    MFW_SS_NOTIFY_holdRELEASED, /* on hold call released          */
    MFW_SS_NOTIFY_checkSS,   /* forward check SS message received */
    MFW_SS_NOTIFY_ectALERT,
    MFW_SS_NOTIFY_ectCONNECT,
    MFW_SS_MT_DEFLECTED,      /* call deflected                */
    //x0pleela 22 Feb, 2007 ER: OMAPS00113891
    MFW_SS_MT_CALL_FORWARDED    /* MT call is forwarded to another subscriber */
} T_MFW_SS_NOTIFY_CODES;

typedef enum
{
    MFW_SS_ERR_UNKNOWN,
    MFW_SS_NO_ERROR,
  	MFW_SS_ERROR,
    MFW_SS_ERR_UNKNOWN_SUBSCRIBER,
    MFW_SS_ERR_ILLEGAL_SUBSCRIBER,
    MFW_SS_ERR_BEARER_SVC_NOT_PROV,
    MFW_SS_ERR_TELE_SVC_NOT_PROV,
    MFW_SS_ERR_ILLEGAL_EQUIPMENT,
    MFW_SS_ERR_CALL_BARRED,      
    MFW_SS_ERR_ILLEGAL_SS_OPERATION,
    MFW_SS_ERR_SS_ERR_STATUS,       
    MFW_SS_ERR_SS_NOT_AVAIL,
    MFW_SS_ERR_SS_SUBS_VIOLATION,
    MFW_SS_ERR_SS_INCOMP,        
    MFW_SS_ERR_FAC_NOT_SUPPORTED,
    MFW_SS_ERR_ABSENT_SUBS,      
    MFW_SS_ERR_SYSTEM_FAIL,
    MFW_SS_ERR_DATA_MISSING,
    MFW_SS_ERR_UNEXPECT_DATA,
    MFW_SS_ERR_PWD_REG_FAIL,
    MFW_SS_ERR_NEG_PWD_CHECK,
    MFW_SS_ERR_NUM_PWD_VIOLATION,
    MFW_SS_ERR_UNKNOWN_ALPHA,
    MFW_SS_ERR_USSD_BUSY,
    MFW_SS_ERR_MAX_NUM_MPTY_EXCEED,
    MFW_SS_ERR_RESOURCE_NOT_AVAIL,
    MFW_SS_GEN_PROB_UNRECOG_CMP,
    MFW_SS_GEN_PROB_MISTYPED_CMP,
    MFW_SS_GEN_PROB_BAD_STRUCT_CMP,
    MFW_SS_INV_PROB_DUPL_INV_ID,
    MFW_SS_INV_PROB_UNRECOG_OP,
    MFW_SS_INV_PROB_MISTYPED_PAR,
    MFW_SS_INV_PROB_RESOURCE_LIM,
    MFW_SS_INV_PROB_INIT_RELEASE,
    MFW_SS_INV_PROB_UNRECOG_LNK_ID,
    MFW_SS_INV_PROB_LNK_RES_UNEXP,
    MFW_SS_INV_PROB_UNEXP_LNK_OP,
    MFW_SS_RES_PROB_UNRECOG_INV_ID,
    MFW_SS_RES_PROB_RET_RES_UNEXP,
    MFW_SS_RES_PROB_MISTYPED_PAR,
    MFW_SS_ERR_PROB_UNRECOG_INV_ID,
    MFW_SS_ERR_PROB_RET_ERR_UNEXP,
    MFW_SS_ERR_PROB_UNRECOG_ERR,
    MFW_SS_ERR_PROB_UNEXP_ERR,
    MFW_SS_ERR_PROB_MISTYPED_PAR,
    MFW_SS_ERR_FATAL_INV_RESULT,
    MFW_SS_ERR_FATAL_CCD_DEC,
    MFW_SS_ERR_FATAL_SS_ENT
} T_MFW_SS_ERROR;

/* basic service type */
typedef enum              
{
  	MFW_BST_UNKNOWN = 0,
    MFW_BST_BEARER  = 0x82,
    MFW_BST_TELE    = 0x83
} T_MFW_BASIC_SERVICE_TYPE;

#define MFW_BS_TS_UNKNOWN (0xFF)  /* unknown bearer and teleservice */

/* teleservice */ 
typedef enum              
{
    MFW_TS_ALL_TS        =0x0,         /* all teleservices               */
    MFW_TS_ALL_SPCH      =0x10,        /* All speech transmission services */
    MFW_TS_TLPHNY        =0x11,        /* telephony                      */
    MFW_TS_EMRGNCY       =0x12,        /* emergency calls                */
    MFW_TS_ALL_SMS       =0x20,        /* all SMS services               */
    MFW_TS_SMS_MT        =0x21,        /* SMS MT PP                      */
    MFW_TS_SMS_MO        =0x22,        /* SMS MO PP                      */
    MFW_TS_ALL_FAX       =0x60,        /* all FAX transmission services  */
    MFW_TS_FAX3_ALT_SPCH =0x61,        /* FAX group 3 alter. speech      */
    MFW_TS_FAX3_AUTO     =0x62,        /* FAX group 3 automatic          */
    MFW_TS_FAX4          =0x63,        /* FAX group 4                    */
    MFW_TS_ALL_DATA      =0x70,        /* all FAX and SMS services       */
    MFW_TS_ALL_XCPT_SMS  =0x80,        /* all FAX and speech services    */
    MFW_TS_ALL_PSSS      =0xd0,        /* all PLMN specific TS           */
    MFW_TS_PLMN1         =0xd1,        /* PLMN specific TS 1             */
    MFW_TS_PLMN2         =0xd2,        /* PLMN specific TS 2             */
    MFW_TS_PLMN3         =0xd3,        /* PLMN specific TS 3             */
    MFW_TS_PLMN4         =0xd4,        /* PLMN specific TS 4             */
    MFW_TS_PLMN5         =0xd5,        /* PLMN specific TS 5             */
    MFW_TS_PLMN6         =0xd6,        /* PLMN specific TS 6             */
    MFW_TS_PLMN7         =0xd7,        /* PLMN specific TS 7             */
    MFW_TS_PLMN8         =0xd8,        /* PLMN specific TS 8             */
    MFW_TS_PLMN9         =0xd9,        /* PLMN specific TS 9             */
    MFW_TS_PLMNA         =0xda,        /* PLMN specific TS A             */
    MFW_TS_PLMNB         =0xdb,        /* PLMN specific TS B             */
    MFW_TS_PLMNC         =0xdc,        /* PLMN specific TS C             */
    MFW_TS_PLMND         =0xdd,        /* PLMN specific TS D             */
    MFW_TS_PLMNE         =0xde,        /* PLMN specific TS E             */
    MFW_TS_PLMNF         =0xdf        /* PLMN specific TS F             */

} T_MFW_TELE_SERVICE;

/* bearer service */
typedef enum              
{
    MFW_BS_ALL_BS       =0x0,         /* all bearer services            */
    MFW_BS_ALL_DATA_CDA =0x10,        /* all data CDA services          */
    MFW_BS_CDA_300      =0x11,        /* data CDA  300 bps              */
    MFW_BS_CDA_1200     =0x12,        /* data CDA 1200 bps              */
    MFW_BS_CDA_1200_75  =0x13,        /* data CDA 1200/75 bps           */
    MFW_BS_CDA_2400     =0x14,        /* data CDA 2400 bps              */
    MFW_BS_CDA_4800     =0x15,        /* data CDA 4800 bps              */
    MFW_BS_CDA_9600     =0x16,        /* data CDA 9600 bps              */
    MFW_BS_ALL_DATA_CDS =0x18,        /* all data CDS services          */
    MFW_BS_CDS_1200     =0x1a,        /* data CDS 1200 bps              */
    MFW_BS_CDS_2400     =0x1c,        /* data CDS 2400 bps              */
    MFW_BS_CDS_4800     =0x1d,        /* data CDS 4800 bps              */
    MFW_BS_CDS_9600     =0x1e,        /* data CDS 9600 bps              */
    MFW_BS_ALL_DATA_PAD =0x20,        /* all data PAD services          */
    MFW_BS_PAD_300      =0x21,        /* data PAD  300 bps              */
    MFW_BS_PAD_1200     =0x22,        /* data PAD 1200 bps              */
    MFW_BS_PAD_1200_75  =0x23,        /* data PAD 1200/75 bps           */
    MFW_BS_PAD_2400     =0x24,        /* data PAD 2400 bps              */
    MFW_BS_PAD_4800     =0x25,        /* data PAD 4800 bps              */
    MFW_BS_PAD_9600     =0x26,        /* data PAD 9600 bps              */
    MFW_BS_ALL_DATA_PDS =0x28,        /* all data PDS services          */
    MFW_BS_PDS_2400     =0x2c,        /* data PDS 2400 bps              */
    MFW_BS_PDS_4800     =0x2d,        /* data PDS 4800 bps              */
    MFW_BS_PDS_9600     =0x2e,        /* data PDS 9600 bps              */
    MFW_BS_SPCH_ALT_CDA =0x30,        /* all data CDA alter. speech     */
    MFW_BS_SPCH_ALT_CDS =0x38,        /* all data CDS alter. speech     */
    MFW_BS_SPCH_FLD_CDA =0x40,        /* all data speech followed CDA   */
    MFW_BS_SPCH_FLD_CDS =0x48,        /* all data speech followed CDA   */
    MFW_BS_ALL_DC_ASYN  =0x50,        /* all data circuit asynchronous  */
    MFW_BS_ALL_ASYN     =0x60,        /* all asynchronous services      */
    MFW_BS_ALL_DC_SYN   =0x58,        /* all data circuit synchronous   */
    MFW_BS_ALL_SYN      =0x68,        /* all synchronous services       */
    MFW_BS_ALL_PSSS     =0xd0,        /* all PLMN specific BS           */
    MFW_BS_PLMN1        =0xd1,        /* PLMN specific 1                */
    MFW_BS_PLMN2        =0xd2,        /* PLMN specific 2                */
    MFW_BS_PLMN3        =0xd3,        /* PLMN specific 3                */
    MFW_BS_PLMN4        =0xd4,        /* PLMN specific 4                */
    MFW_BS_PLMN5        =0xd5,        /* PLMN specific 5                */
    MFW_BS_PLMN6        =0xd6,        /* PLMN specific 6                */
    MFW_BS_PLMN7        =0xd7,        /* PLMN specific 7                */
    MFW_BS_PLMN8        =0xd8,        /* PLMN specific 8                */
    MFW_BS_PLMN9        =0xd9,        /* PLMN specific 9                */
    MFW_BS_PLMNA        =0xda,        /* PLMN specific A                */
    MFW_BS_PLMNB        =0xdb,        /* PLMN specific B                */
    MFW_BS_PLMNC        =0xdc,        /* PLMN specific C                */
    MFW_BS_PLMND        =0xdd,        /* PLMN specific D                */
    MFW_BS_PLMNE        =0xde,        /* PLMN specific E                */
    MFW_BS_PLMNF        =0xdf         /* PLMN specific F                */

} T_MFW_BEARER_SERVICE;


/* mode of close user group (cug) */
typedef enum
{
  MFW_SS_CUG_MODE_Not_Present = -1,
  MFW_SS_CUG_TMP_DISABLE,
  MFW_SS_CUG_TMP_ENABLE
}T_MFW_SS_CUG_MODE;


/* index  - identifier of close user group */
typedef enum
{
  MFW_SS_CUG_INDEX_Not_Present = -1,
  MFW_SS_CUG_INDEX0,
  MFW_SS_CUG_INDEX1,
  MFW_SS_CUG_INDEX2,
  MFW_SS_CUG_INDEX3,
  MFW_SS_CUG_INDEX4,
  MFW_SS_CUG_INDEX5,
  MFW_SS_CUG_INDEX6,
  MFW_SS_CUG_INDEX7,
  MFW_SS_CUG_INDEX8,
  MFW_SS_CUG_INDEX9,
  MFW_SS_CUG_NO_INDEX  /* preferential cug is used by the network */
}T_MFW_SS_CUG_INDEX;

/* info of close user group cug           */
typedef enum 
{
  MFW_SS_CUG_INFO_Not_Present = -1,
  MFW_SS_CUG_INFO_NO,
  MFW_SS_CUG_SUPP_OA,
  MFW_SS_CUG_SUPP_PREF,
  MFW_SS_CUG_SUPP_BOTH
}T_MFW_SS_CUG_INFO;

/* Call forwarding result */
typedef struct
{
    UBYTE ss_telecom_type;   
    UBYTE ss_telecom_service;   
    UBYTE ss_status;
    UBYTE ss_numbering_type;
    UBYTE ss_numbering_plan;
    UBYTE ss_forwarded_to_number[MFW_SS_NUM_LEN];
    UBYTE ss_forwarded_to_subaddress[MFW_SS_SUBADR_LEN];
    UBYTE ss_no_reply_condition_time;
} T_MFW_FEATURE;

typedef struct
{
    T_MFW_SS_CODES    ss_code;            /* Type of call forwarding    */
    T_MFW_SS_CATEGORY ss_category;        /* Category of operation      */
    T_MFW_SS_ERROR    ss_error;           /* Error Value                */
    UBYTE             ss_feature_count;   /* Number of feature elements */
    T_MFW_FEATURE    *ss_feature_list;    /* Feature list               */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_CF_CNF;

/* Call barring info */
typedef struct
{
    UBYTE ss_telecom_type;   
    UBYTE ss_telecom_service;   
    UBYTE ss_status;
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
    UBYTE dummy5;
} T_MFW_TELECOM;

/* Call barring result */
typedef struct
{
    T_MFW_SS_CODES    ss_code;            /* Type of call barring       */
    T_MFW_SS_CATEGORY ss_category;        /* Category of operation      */         
    T_MFW_SS_ERROR    ss_error;           /* Error Value                */
    UBYTE             ss_telecom_count;   /* Number of telecom elements */
    T_MFW_TELECOM    *ss_telecom_list;    /* telecom list               */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_CB_CNF;

/* Call waiting info */
typedef struct
{
    UBYTE ss_telecom_type;   
    UBYTE ss_telecom_service;   
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
    UBYTE dummy5;
    UBYTE dummy6;
} T_MFW_SRV_GROUP;
 
/* Call waiting result */
typedef struct
{
    T_MFW_SS_CODES     ss_code;       /* Type of call waiting      */
    T_MFW_SS_CATEGORY  ss_category;   /* Category of operation     */
    T_MFW_SS_ERROR	   ss_error;      /* Error Value               */
    UBYTE              ss_status;         /* Status                     */
    UBYTE              ss_service_count;  /* Number of telecom elements */
    T_MFW_SRV_GROUP   *ss_service_list;   /* Service list               */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_CW_CNF;

/* Calling line identification/connected line identification result */
typedef struct
{
    T_MFW_SS_CODES      ss_code;       /* Type of line identification   */
    T_MFW_SS_CATEGORY   ss_category;   /* Category of operation         */
    T_MFW_SS_ERROR		ss_error;      /* Error Value                   */
    UBYTE               ss_status;        /* Status                        */
    T_MFW_CL_STATUS     ss_clir_option;   /* CLIR option                   */
    T_MFW_OV_CTGRY      ss_ovrd_ctgry;    /* Override category             */
} T_MFW_SS_CLI_CNF;

/* Get password indication */
typedef struct
{
    T_MFW_SS_CODES   	ss_code;                    /* Type of call barring           */
    T_MFW_SS_CATEGORY   ss_category;      /* Category of operation         */
    T_MFW_SS_ERROR		ss_error;         /* Error Value                   */
    UBYTE               ss_new_pwd[MFW_SS_PWD_LEN];/* New Password         */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_PW_CNF;

/* SS notification */
typedef struct
{
    UBYTE   code;                     /* result copdes             */
    UBYTE   index;                    /* used only for CUG         */
    UBYTE   number[MFW_SS_NUM_LEN];       /* phone number              */
    UBYTE   subaddr[MFW_SS_SUBADR_LEN];   /* subaddress                */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_NOTIFY;

/* SS result */
typedef struct
{
    UBYTE   type;       /* type of service as specified in T_MFW_SS_CODES    */
    UBYTE   category;   /*command category as specified in T_MFW_SS_CATEGORY */
    char    ppn[MAX_PWD_LEN];  /* password, PUK, number depending on SS type */
    char    pwd[MAX_PWD_LEN];  /* password if applicable                     */
    UBYTE   bs;                /* basic service (2.30)                       */
    UBYTE   time;              /* no reply condition time (2.30)             */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_RES;

/* IMEI number */
typedef struct
{
    UBYTE      imei_number[LONG_NAME];
	UBYTE      error;
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
    UBYTE dummy5;
} T_MFW_IMEI;

/* Unstructured supplementary service data */
typedef struct
{
    UBYTE           len;                     /* length of USSD data */
	  UBYTE           ussd[MAX_USSD_LEN];    /* USSD data           */
    T_MFW_DCS       dcs;                     /* data coding scheme  */
    UBYTE           error;
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
} T_MFW_SS_USSD;

typedef struct  /* CUG parameter    */
{
    T_MFW_SS_CUG_MODE mode;            /* cug mode: disable/enable/no mode */
    T_MFW_SS_CUG_INDEX index;          /* cug index: 0 ...10             */
    T_MFW_SS_CUG_INFO info;            /* cug info:suppress OA,pref.CUG,both,no info */
    UBYTE dummy1;
    UBYTE dummy2;
    UBYTE dummy3;
    UBYTE dummy4;
}T_MFW_SS_CUG_PARA;

/* SS Parameter  */
typedef union
{
  T_MFW_SS_CF_CNF   cf_cnf;
  T_MFW_SS_CB_CNF   cb_cnf;
  T_MFW_SS_CW_CNF   cw_cnf;
  T_MFW_SS_CLI_CNF  cli_cnf;
  T_MFW_SS_PW_CNF   pw_cnf;
  T_MFW_SS_RES      ss_res;
  T_MFW_IMEI        imei_info;
  T_MFW_SS_USSD     ussd_info;
  T_MFW_SS_NOTIFY   notify;
} T_MFW_SS_PARA;

EXTERN void            ss_mfw_init             (void);
EXTERN void            ss_exit                 (void);
EXTERN T_MFW_HND       ss_create               (T_MFW_HND hWin, 
                                                T_MFW_EVENT event, 
                                                T_MFW_CB cbfunc);
EXTERN T_MFW_RES       ss_delete               (T_MFW_HND h);
EXTERN T_MFW_SS_RETURN ss_check_ss_string      (UBYTE *string);
EXTERN T_MFW_SS_RETURN ss_execute_transaction  (UBYTE *ss_string, 
                                                UBYTE ussd_mand);
EXTERN T_MFW           ss_send_ussd            (T_MFW_SS_USSD *ussd_info);
EXTERN T_MFW  	       ss_abort_transaction	   (void);
T_MFW_SS_RETURN ss_check_group(T_KSD_SEQGRP grp, T_KSD_SEQPARAM   para);
EXTERN T_MFW_SS_RETURN     ss_set_cug              (T_MFW_SS_CUG_PARA *cug_param);
EXTERN T_MFW_SS_RETURN     ss_get_cug              (T_MFW_SS_CUG_PARA *cug_param);
//CQ-16432 start
EXTERN void ss_sat_passwdReqd(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err);
//CQ-16432 end
#endif
