/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_cm.h        $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 3.11.98                      $Modtime:: 21.03.00 19:26   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_CM

   PURPOSE : This modul contains the definition for call management.


   $History:: mfw_cm.h                                              $

   Jan 09, 2007   REF: OMAPS00110541   x0039928
   Description: In N12.15 release, Ram usage of BMI plus MFW got increased by 2.5 k
   Solution: Changed the #defines of MFW_NUM_LEN and MAX_LEN
   
   Dec 20, 2006 DR:OMAPS00108192 a0393213(R.Prabakar)
   Description : Continous DTMF tones for simultanous keypress
   Solution     : Nesting of START-STOP sequence was not taken care of. Now they are taken care of.

   Dec 13, 2006 DR:OMAPS00107103 a0393213(R.Prabakar)
   Description : No DTMF tones for simultanous keypress
   Solution     : Once there was some problem in sending DTMF tones, the problem persisted till the next reset.
                     This was because the queue was not getting flushed once the call was ended. 
                     Now the queue is flushed once the call is ended.
                     
	Nov 17, 2006   REF:DR:OMAPS00104580  x0039928
	Description:   CT GCF - TC27.22.4.13.1 Seq 1.10 - PROACTIVE SIM COMMANDS: 
	SET UP CALL (NORMAL) - Call Set-up Failed
	Solution:  Removed the Redial flag enabling code and changed the number length.
	
   Feb 03, 200   REF: CRR 25960   x0012850
   Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible
   Solution: swapping action results in one of the calls getting muted. We attach user connection again.
                 
    Oct 04, 2004    REF: CRR 25519     Deepa M.D
	Bug:Re-align structure members in MFW
	Fix:Structure elements have been  realigned to avoid the structure padding

//  Jul 2, 2004        REF: CRR 21421  xvilliva
//  Bug:  ALS:Action on the other line when change is locked. 
//  Fix:	A new error code is defined for handling error returned by ACI.


 * 
 * *****************  Version 42  *****************
 * User: Vo           Date: 24.03.00   Time: 10:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * chang define value
 * 
 * *****************  Version 41  *****************
 * User: Vo           Date: 15.03.00   Time: 18:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: Remote call control and Abort CM command
 * 
 * *****************  Version 40  *****************
 * User: Vo           Date: 22.02.00   Time: 14:03
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 39  *****************
 * User: Vo           Date: 17.02.00   Time: 19:06
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 38  *****************
 * User: Vo           Date: 20.01.00   Time: 13:01
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New: Manage fax/data/combinated call
 * New: New event CM_REDIAL_STOP to indicate that the reason of
 * failed connection is outside of defined category.
 * 
 * *****************  Version 37  *****************
 * User: Le           Date: 13.12.99   Time: 8:50
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 36  *****************
 * User: Vo           Date: 11.11.99   Time: 14:22
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New event and command define for DTMF tone
 * New prototype for requesting of autoredial mode
 * 
 * *****************  Version 35  *****************
 * User: Vo           Date: 10.09.99   Time: 18:36
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new prototype
 * 
 * *****************  Version 34  *****************
 * User: Vo           Date: 30.08.99   Time: 13:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new event, parameter for setting of AoC value
 * 
 * *****************  Version 33  *****************
 * User: Vo           Date: 9.08.99    Time: 12:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * change definition (length of number (161 to 41))
 *  new return value "CM_CTRL_STR"
 *  new cm command: "CM_SWAP_MULTIPARTY"
 * 
 * *****************  Version 32  *****************
 * User: Vo           Date: 21.07.99   Time: 14:39
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 31  *****************
 * User: Vo           Date: 16.07.99   Time: 10:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 30  *****************
 * User: Vo           Date: 9.07.99    Time: 14:31
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 29  *****************
 * User: Vo           Date: 2.07.99    Time: 18:25
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 28  *****************
 * User: Vo           Date: 22.06.99   Time: 8:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 *
 * *****************  Version 1  *****************
 * User: Vo           Date: 20.11.98   Time: 13:40
 * Created in $/GSM/DEV/MS/SRC/MFW
 * Call Management

    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY

*/

#ifndef DEF_MFW_CM_HEADER
#define DEF_MFW_CM_HEADER



/* #include "phb.h" RSA 20/12/01 - Removed */
#include"mfw_phb.h"

#define MFW_STRING_LEN	   161
#define MFW_NUM_LEN         41      /*  Jan 09, 2007   REF: OMAPS00110541   x0039928 */
#define MFW_TAG_LEN         21
#define MFW_SUBADR_LEN      21
#define MAX_LEN             21           /*  Jan 09, 2007   REF: OMAPS00110541   x0039928 */
#define MAX_CURRENCY         5

/*
 * events are organised as bitmaps
 */
#define E_CM_OK                 0x0001
#define E_CM_CW                 0x0002
#define E_CM_CONNECT            0x0004
#define E_CM_CONNECT_ACK        0x0008
#define E_CM_DISCONNECT         0x0010
#define E_CM_COMMAND            0x0020
#define E_CM_NOTIFY             0x0040
#define E_CM_REDIAL             0x0080
#define E_CM_COLP               0x0100
#define E_CM_RING_OFF           0x0200
#define E_CM_MO_RES             0x0400
#define E_CM_CPI                0x0800
#define E_CM_TIMER_EVERY_SECOND 0x1000
#define E_CM_AOC                0x2000
#define E_CM_AOC_CNF            0x4000
#define E_CM_DTMF               0x8000
#define E_CM_WAP_CALL_CONNECTED   0x10000
#define E_CM_CTYI				0x20000	/* SPR#1352 - SH */
/* Marcus: CCBS: 14/11/2002: Start */
#define E_CM_CCBS               0x40000
/* Marcus: CCBS: 14/11/2002: End */
#define E_CM_CALL_DEFLECT_STATUS   0x80000 /*SPR 1392*/
#define E_CM_DISCONNECT_ALL 0x100000
#define E_CM_ALL_SERVICES    0xFFFFFF

/* Possible service code for a call number */
#define CM_OK          1        /* A mobile originated call attempt has been started.   */
#define CM_EC          2        /* A mobile originated emergency call has been started. */
#define CM_SIM         3        /* The number has started a SIM control procedure.      */
#define CM_SS          4        /* The number has started a supplementary service.      */
#define CM_USSD        5        /* The number has started an unstructured SS.           */
#define CM_MMI         6        /* The number define a MMI specific procedure.          */
#define CM_ERROR       7        /* The number can not procedured.                       */
#define CM_BLACKLIST   8        /* The number is blacklisted.                           */
#define CM_NO_REG      9        /* The network is not registered.                       */
#define CM_NO_SERVICE 10        /* The service type of network is NO_SERVICE.           */
#define CM_NO_CARRIER 11        /* No connection because of no carrier                  */
#define CM_NO_AOC     12        /* NO advice of charge information available            */
#define CM_BUSY       13        /* No connection because of user busy                   */
#define CM_NO_ANSWER  14        /* No connection because of no answering                */
#define CM_CTRL_STR   15        /* control string									    */
#define CM_ACM_MAX    16        /* ACM reachs maximal value                             */
#define CM_CFDN_ERR   17        /* check FDN number error                               */
#define CM_CBDN_ERR   18        /* check BDN number error                               */
#define CM_2CALLS_ALREADY_ERR 19/* 2 calls already, we can not perform a third outgoing call */
#define CM_CLI		    20        /* CLI command complete */
#define CM_ALS_ERR		    21//xvilliva 21421 - Handling err code returned by ACI.

/* Definition for advice of charge */
#define CM_AOC_UNKNOWN   0
#define CM_AOC_INFO     20      /* AoC information from the network.  */
#define CM_AOC_PUCT     21      /* PUCT is requested by MMI logic.    */
#define CM_AOC_ACMMAX   22      /* ACM max is requested by MMI logic. */
#define CM_AOC_ACM      23      /* ACM is requested by MMI logic.     */
#define CM_AOC_CCM      24      /* CCM is requested by MMI logic.     */
#define CM_AOC_TIMERS   25      /* AoC timer values are requested.    */
#define CM_AOC_CCM_PUCT 26      /* CCM using PUCT                     */
#define CM_AOC_ACM_PUCT 27      /* ACM using PUCT                     */
#define CM_AOC_RAW_PUCT 28      /* RAW PUCT                           */

/* Definition for redialing */
#define CM_REDIAL_OFF        0  /* no redialing                       */
#define CM_REDIAL_AUTO       1  /* automatic redialing                */
#define CM_REDIAL_MANU       2  /* manual redialing                   */

#define CM_REDIAL_BLACKLIST   33  /* reset blacklist                  */
#define CM_REDIAL_NO_SERVICE  34  /* mobile out the cell              */
#define CM_REDIAL_INCOMING    35  /* redial abort because of incoming call */
#define CM_REDIAL_ABORTED     36  /* abort redial fromm MMI           */
#define CM_REDIAL_STARTED     37  /* start autoredial                 */
#define CM_REDIAL_ATTEMPT     38  /* start redial call                */
#define CM_REDIAL_BLACKLISTED 39  /* Redialing number is blacklisted  */
#define CM_REDIAL_OUTGOING    40  /* redial abort because of outgoing call */
#define CM_REDIAL_SUCCESS     41  /* Redialing is successful          */
#define CM_REDIAL_STOP        42  /* Stop redialing (outside of category) */

/* definition of commands for call related supplementary service */
#define CM_COMMAND_UNKNOWN        0
#define CM_HOLD                   1  /* initiate call hold                 */
#define CM_RETRIEVE               2  /* initiate call retrieve             */
#define CM_BUILD_MULTIPARTY       3  /* build a multiparty                 */
#define CM_HOLD_MULTIPARTY        4  /* set multiparty on hold             */
#define CM_RETRIEVE_MULTIPARTY    5  /* set multiparty on active           */
#define CM_SPLIT_MULTIPARTY       6  /* split a multiparty                 */
#define CM_ETC                    7  /* enable explicit call transfer      */
#define CM_CALL_DEFLECTION        8  /* redirection of incomming call      */
#define CM_CCBS                   9  /* completion on call busy subscriber */
#define CM_MODIFY                10  /* initiate in-call modification      */
#define CM_DTMF_ACTIVE           11  /* enable DTMF signalling             */
#define CM_DTMF_INACTIVE         12  /* disable DTMF signalling            */
#define CM_SWAP                  13  /* swap of two calls                  */
#define CM_SWAP_MULTIPARTY       14
#define CM_DTMF_AUTO             15  /* DTMF signalling with defined length of tones */


#define CM_DISCONNECT_ALL_RESULT_ERROR 0
#define CM_DISCONNECT_ALL_RESULT_OK 1



#define MIN_DURATION_1   	7000		/* time of the 1th redialing        */
#define MIN_DURATION_2_4	70000		/* time of the 2th to 4st redialing  */
#define MIN_DURATION_5  	200000		/* time of the 5th or over redialing */

/* Mar 27, 2006    REF:ER OMAPS00071798 */

#ifdef NEPTUNE_BOARD
typedef enum              /* %CTTY parameter (audio mode) */
{
  AUDIO_MODE_TEXT = 0,
  AUDIO_MODE_VCO,
  AUDIO_MODE_Unknown
}
T_MMI_CTTY_AUDIO_MOD;
#endif


typedef enum              /* definition of the result */
{
  CM_AOC_CONF_UNKNOWN,
  CM_AOC_CONF_OK,
  CM_AOC_CONF_SIMPIN2REQ,
  CM_AOC_CONF_WRONGPASSWD,
  CM_AOC_CONF_OPNOTALLOW,
  CM_AOC_CONF_ACMRESETNEEDED
} T_MFW_AOC_CNF_RES;

typedef enum              /* type of call progress information */
{
  CPI_TYPE_UNKNOWN = -1,
  CPI_TYPE_SETUP,       /* call setup      */
  CPI_TYPE_DISC,        /* call disconnect */
  CPI_TYPE_ALERT,       /* call alerting   */
  CPI_TYPE_PROC,        /* call proceeding */
  CPI_TYPE_SYNC,         /* TCH assigment   */
  CPI_TYPE_CONN,
  CPI_TYPE_RELEASE,    /* Call released  cq111676 17-02-04 */
  CPI_TYPE_REJECT       /* Call rejected   cq11676 17-02-04   */
} T_MFW_CPI_TYPE;

typedef enum              /* inband tone */
{
  CPI_IBT_UNKNOWN = -1,
  CPI_IBT_FALSE,
  CPI_IBT_TRUE
} T_MFW_CPI_IBT;

typedef enum              /* TCH */
{
  CPI_TCH_UNKNOWN = -1,
  CPI_TCH_FALSE,
  CPI_TCH_TRUE
} T_MFW_CPI_TCH;

typedef enum            /* type of call    */
{
  NONE_TYPE,
  VOICE_CALL,
  AuxVOICE_CALL,
  DATA_CALL,
  FAX_CALL,
  VFD_VOICE,       /* voice followed data, voice mode    */
  VAD_VOICE,       /* voice alternating data, voice mode */
  VAF_VOICE,       /* voice alternating fax, voice mode  */
  VFD_DATA,        /* voice followed data, data mode     */
  VAD_DATA,        /* voice alternating data, data mode  */
  VAF_FAX          /* voice alternating fax, fax mode    */
} T_MFW_CM_CALL_TYPE;

typedef enum            /* direction of call    */
{
	MFW_CM_INV_DIR = -1,
	MFW_CM_MOC,
	MFW_CM_MTC
} T_MFW_CM_CALL_DIR;

/* Result of operation */
typedef enum
{
  NotPresent,
  Present
} T_MFW_CM_RESULT;

/* Call status */
typedef enum
{
  NOT_PRESENT,
  CALL_HELD,
  CALL_ACTIVE,
  CALL_DEACTIVE,
  CALL_WAIT,
  CALL_DIAL,
  CALL_ALERT
} T_MFW_CM_CALL_STAT;

/* Bearer parameter */
typedef enum    /* data rate */
{
  SPEED_300,
  SPEED_1200,
  SPEED_2400,
  SPEED_4800,
  SPEED_9600,
  SPEED_14400
} T_MFW_CM_DATA_SPEED;

typedef enum   /* bearer service */
{
  Asynch,
  Synch
} T_MFW_CM_BS;

typedef enum   /* connection element */
{
  Transparent,
  NonTransparent
} T_MFW_CM_CE;

typedef enum   /* connection element */
{
  Linear,
  Circular
} T_MFW_CM_RI;

typedef enum  /* SS notification result code */
{
    MFW_CM_NOTIFY_UNKNOWN,
    MFW_CM_HOLD,         /* call has been put on hold      */
    MFW_CM_RETRIEVED     /* call has been retrieved        */
} T_MFW_CM_NOTIFY_CODES;

typedef enum   /* connection element */
{
  MFW_MPTY_NotPresent = -1,
  MFW_NO_MTPY_MEMBER,
  MFW_MTPY_MEMBER
} T_MFW_MTPY_MEMBER;

typedef UBYTE T_CM_CLI;

typedef struct
{
  SHORT  call_number;
  UBYTE  cause;
} T_MFW_CM_DISCONNECT;

typedef struct
{
  UBYTE result;
  UBYTE cause;
} T_MFW_CM_DISCONNECT_ALL;


typedef struct
{
  UBYTE rate;                   /* used baud rate       */
  UBYTE bearer_serv;            /* used bearer service  */
  UBYTE conn_elem;              /* used connect_element */
  UBYTE stop_bits;              /* used stop bits       */
  UBYTE data_bits;              /* used data bits       */
  UBYTE parity;                 /* used parity          */
  UBYTE flow_control;           /* used flow control    */
} T_MFW_CM_BC_PARA;


#ifdef NO_ASCIIZ 
typedef struct {
  SHORT             call_id;                   /* call identify        */
  UBYTE             number[MFW_NUM_LEN];       /* call number          */
  UBYTE             ton;                       /* type of number       */
  UBYTE             type;                 
  T_MFW_PHB_TEXT    name;                      /* alpha identfier      */
  UBYTE             subaddr[MFW_SUBADR_LEN];   /* subaddress           */
} T_MFW_CM_MO_INFO; /* type definition for E_CM_COLP event */
#else
typedef struct {
  SHORT             call_id;                   /* call identify        */
  UBYTE             number[MFW_NUM_LEN];       /* call number          */
  UBYTE             ton;                       /* type of number       */
  UBYTE             type;                
  UBYTE             name[MFW_TAG_LEN];         /* alpha identfier      */
  UBYTE             subaddr[MFW_SUBADR_LEN];   /* subaddress           */
} T_MFW_CM_MO_INFO; /* type definition for E_CM_COLP event */
#endif




typedef struct {
  UBYTE             result;                    /* result of operation  */
  SHORT             call_number;               /* call index           */
  UBYTE             number[MFW_NUM_LEN];       /* call number          */
  UBYTE             ton;                       /* type of number       */
  T_MFW_CM_CALL_TYPE  type;                      /* type of this call    */
#ifdef NO_ASCIIZ  
  T_MFW_PHB_TEXT    name;                      /* alpha identfier      */
#else
  UBYTE             name[MFW_TAG_LEN];         /* alpha identfier      */
#endif
  UBYTE             subaddr[MFW_SUBADR_LEN];   /* subaddress           */
  UBYTE             ri;                        /* repeat indicator     */
  T_MFW_CM_BC_PARA  bc1;                       /* bearer parameter 1   */
  T_MFW_CM_BC_PARA  bc2;                       /* bearer parameter 2   */
} T_MFW_CM_CW_INFO; /* type definition for E_CM_CW (indicator a new incomming call) event */

/* Marcus: CCBS: 14/11/2002: Start */
/* Type definitions for E_CM_CCBS events */
typedef enum
{
    CM_CCBS_NONE,                   /* Used to indicate no CCBS operation in progress */
    CM_CCBS_DELETE_ALL,             /* Response to cm_ccbs_delete_all */
    CM_CCBS_DELETE,                 /* Response to cm_ccbs_delete */
    CM_CCBS_LIST,                   /* Response to cm_ccbs_list */
    CM_CCBS_ACCEPT_REGISTRATION,    /* Response to cm_ccbs_accept_registration */
    CM_CCBS_REJECT_REGISTRATION,    /* Response to cm_ccbs_reject_registration */
    CM_CCBS_ACCEPT_RECALL,          /* Response to cm_ccbs_accept_recall */
    CM_CCBS_REJECT_RECALL,          /* Response to cm_ccbs_reject_recall */
    CM_CCBS_ASYNC                   /* An asynchronous event not related to an operation */
} T_MFW_CM_CCBS_OP; /* Type definition for the operation associated E_CM_CCBS event */

typedef enum
{
    CM_CCBS_INFO,                   /* rAT_PercentCCBS information */
    CM_CCBS_OK,                     /* OK completion of a CCBS operation */
    CM_CCBS_ERROR                   /* ERROR completion of a CCBS operation */
} T_MFW_CM_CCBS_TYPE;   /* Type definition for the type of E_CM_CCBS event */

typedef struct
{
    T_MFW_CM_CCBS_OP    operation;  /* CCBS operation in progress */
    T_MFW_CM_CCBS_TYPE  type;       /* type of E_CM_CCBS event */
    T_ACI_CCBS_IND      indct;      /* Type of CM_CCBS_INFO indication */
    T_ACI_CCBS_STAT     status;     /* Status of CM_CCBS_INFO indication */
    SHORT               idx;        /* Index to specific CCBS (1..5), else 0 */
    UBYTE               number[MFW_NUM_LEN];       /* call number          */
    UBYTE               ton;                       /* type of number       */
    T_MFW_CM_CALL_TYPE  call_type;                 /* type of this call    */
    UBYTE               subaddr[MFW_SUBADR_LEN];   /* subaddress           */
    T_ACI_ALRT_PTRN     alrtPtn;                   /* Alerting pattern     */
} T_MFW_CM_CCBS_INFO; /* type definition for E_CM_CCBS event */
/* Marcus: CCBS: 14/11/2002: End */

typedef struct
{
  SHORT  call_number;           /* number of call      */
  UBYTE  command;               /* requested command   */
  UBYTE  result;                /* result of operation */
} T_MFW_CM_COMMAND; /* type definition for E_CM_COMMAND (ss command) event */

/***************************Go-lite Optimization changes Start***********************/
// PATCH TB Add 10 to acm_puct & acm_puct avoid buffer override
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_CM_AOC_INFO)are realigned to avoid the structure padding
typedef struct
{
	LONG        actual_call_timer;    /* time of actual call                        */
	LONG        ccm;                  /* current call meter                         */
	LONG        acm;                  /* accumulated call meter                     */
    LONG        acm_max;              /* accumulated call meter maximum PUCT        */
	ULONG       eppu;                 /* elementary price per unit                  */
	ULONG       exp;                  /* decimal logarithm                          */
	UBYTE       ccm_puct[MAX_LEN+10];    /* current call meter using PUCT              */
	UBYTE       acm_puct[MAX_LEN+10];    /* accumulated call meter using PUCT          */
	UBYTE       ppu[MAX_LEN];         /* price per unit                             */
	UBYTE       cur[MAX_CURRENCY];    /* currency table                             */
	ULONG       sexp;                 /* sign of decimal logarithm                  */
	UBYTE       requested_type;       /* requested type                             */
} T_MFW_CM_AOC_INFO;  /* type definition for E_CM_AOC event. */
/***************************Go-lite Optimization changes end***********************/

typedef struct
{
  UBYTE              requested_type; 
  T_MFW_AOC_CNF_RES  result;  
} T_MFW_CM_AOC_CNF; /* type definition for E_CM_AOC_CONF event */

/***************************Go-lite Optimization changes Start***********************/
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_CM_REDIAL)are realigned to avoid the structure padding
typedef struct
{
#ifdef NO_ASCIIZ  
	T_MFW_PHB_TEXT name;                  /* alpha identfier      */
#else
	UBYTE       name[MFW_TAG_LEN];        /* alpha identfier             */
#endif
	UBYTE       number[MFW_NUM_LEN];      /* called number               */
	UBYTE       subaddr[MFW_SUBADR_LEN];  /* subaddress                  */
	SHORT       call_id;                  /* call identify               */
	UBYTE       ton;                      /* type of number       */
	UBYTE       type;
	UBYTE       left_attempts;            /* redialing left attempts     */
	UBYTE       redial_mode;              /* redial mode                 */
} T_MFW_CM_REDIAL;   /* type definition for E_CM_REDIAL event. */
/***************************Go-lite Optimization changes end***********************/

/* Call status information */
typedef struct
{
  SHORT               call_number;
  UBYTE               stat;                 /* T_MFW_CM_CALL_STAT   */
  UBYTE               number[MFW_NUM_LEN];  /* called number        */
  UBYTE               ton;                  /* type of number       */
  T_MFW_CM_CALL_TYPE  type;                 /* type of call         */
#ifdef NO_ASCIIZ  

  T_MFW_PHB_TEXT      name;                 /* alpha identfier      */
#else
  UBYTE               name[MFW_TAG_LEN];    /* alpha identfier      */
#endif
  T_MFW_MTPY_MEMBER   mtpy;
  T_MFW_CM_CALL_DIR    call_direction;
} T_MFW_CM_STATUS;

/* Notify message */
typedef struct
{
  SHORT                 call_number;
  T_MFW_CM_NOTIFY_CODES notification;
} T_MFW_CM_NOTIFY;

/***************************Go-lite Optimization changes Start***********************/
/* call progress information */
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_CM_CPI)are realigned to avoid the structure padding
typedef struct
{
	T_MFW_CPI_TYPE type;
	T_MFW_CPI_IBT  inband;
	T_MFW_CPI_TCH  tch;
	SHORT          call_number;
	USHORT          cause;  /* cq11676 Add the CPI cause element 17-02-04 MZ  */
} T_MFW_CM_CPI;
/***************************Go-lite Optimization changes end***********************/

/* SPR#1352 - SH - TTY information */

/* This structure passes TTY status to the MMI */
typedef struct
{
  UBYTE neg;	/* Status of TTY request - possible values below*/
} 
T_MFW_CM_CTYI;



/* Possible values from CTYI callback - indicates TTY status*/
enum
{
	CTYI_NONE,
	CTYI_REQUEST,
	CTYI_REJECT,
	CTYI_GRANT
};
/* end SH */

/*
 * CM Parameter
 */
typedef union
{
  T_MFW_CM_DISCONNECT disconnect;
  T_MFW_CM_DISCONNECT_ALL disconnect_all;  
  T_MFW_CM_CW_INFO    cw_info;
  T_MFW_CM_COMMAND    command;
  T_MFW_CM_AOC_INFO   aoc_info;
  T_MFW_CM_REDIAL     rdl;
  T_MFW_CM_NOTIFY     notify;
  T_MFW_CM_CPI        cm_cpi;
  SHORT               call_id;
  T_MFW_CM_AOC_CNF    aoc_cnf;
  UBYTE               dtmf_tone;
// PATCH - add new parameter
  T_MFW_CM_MO_INFO	  mo_data;
  T_MFW_CM_CCBS_INFO  ccbs; // Marcus: CCBS: 14/11/2002
// END PATCH
/*MC, SPR 1392, added call deflection status param*/
  BOOL call_deflect_status;
  /* SPR#1352 - SH - Add parameter for TTY */
  T_MFW_CM_CTYI			ctyi;
  /* end SH*/
} T_MFW_CM_PARA;
/*OMAPS00108192  a0393213*/
typedef struct
{
	BOOL valid; 				/*entries are valid or not*/
	UBYTE digit;				/*the digit entered*/
	T_ACI_VTS_MOD mode;	/*the mode - manual start, manual stop or automatic*/
} T_MFW_CM_DTMF;

EXTERN void      cm_init                  (void);
EXTERN T_MFW_HND cm_create                (T_MFW_HND          hWin,
                                           T_MFW_EVENT        event,
                                           T_MFW_CB           cbfunc);
EXTERN T_MFW_RES cm_delete                (T_MFW_HND h);
EXTERN T_MFW     cm_check_emergency       (UBYTE *number);
EXTERN T_MFW     cm_mo_call               (UBYTE             *number,
                                           T_MFW_CM_CALL_TYPE type);
EXTERN void      cm_mt_reject             (void);
EXTERN void      cm_mt_accept             (void);
EXTERN void      cm_end_all               (void);
EXTERN T_MFW     cm_disconnect            (SHORT              call_number);
EXTERN T_MFW     cm_command               (SHORT              call_number,
                                           UBYTE              command,
                                           UBYTE              crss_string);
EXTERN T_MFW     cm_get_aoc_value         (UBYTE              request_type,
                                           T_MFW_CM_AOC_INFO *aoc_info);
EXTERN T_MFW     cm_set_aoc_value         (UBYTE              request_type, 
                                           T_MFW_CM_AOC_INFO *aoc_info, 
                                           UBYTE             *pwd);
EXTERN void      cm_set_bc_para           (T_MFW_CM_BC_PARA * bc, UBYTE ti);
EXTERN void      cm_set_redial            (UBYTE redial_mode);
EXTERN void      cm_redial                (void);
EXTERN void      cm_autoredial_start      (void);
EXTERN void      cm_redial_abort          (void);
EXTERN UBYTE    cm_redial_get_count      (void);

EXTERN int       cm_status                (SHORT call_number, 
                                           T_MFW_CM_STATUS *status,
                                           UBYTE num);
EXTERN void      cm_exit                  (void);
EXTERN T_CM_CLI  cm_mo_prefix             (T_CM_CLI prefix);
EXTERN T_MFW     cm_get_redial_mode       (void);
EXTERN T_MFW     cm_command_abort         (UBYTE command);
/*MC, SPR 1392*/
T_MFW cm_deflect_incoming_call(char* number);
void cm_ok_CTFR(void);
void cm_error_CTFR(void);
/* Marcus: CCBS: 13/11/2002: Start */
EXTERN T_MFW    cm_ccbs_delete_all(void);
EXTERN T_MFW    cm_ccbs_delete(SHORT idx);
EXTERN T_MFW    cm_ccbs_list(void);
EXTERN T_MFW    cm_ccbs_accept_registration(void);
EXTERN T_MFW    cm_ccbs_reject_registration(void);
EXTERN T_MFW    cm_ccbs_accept_recall(UBYTE *number, T_MFW_CM_CALL_TYPE type);
EXTERN T_MFW    cm_ccbs_reject_recall(void);
EXTERN void     cm_ccbs_ok(void);
EXTERN void     cm_ccbs_error(void);
/* Marcus: CCBS: 13/11/2002: End */

/* SPR#1985 - SH - TTY */
void cm_tty_enable(UBYTE enable);
void cm_tty_query(void);

// Feb 03, 200   REF: CRR 25960   x0012850
// Description: Call Hold: While one call in Active and other in Held by swapping either of phone is not audible
// Prototype for function cm_setSpeechMode()
EXTERN void cm_setSpeechMode(void);

EXTERN void cm_force_disconnect(void);
/* Added to remove warning */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
EXTERN void resetDTMFQueue();	/*OMAPS00107103 a0393213(R.Prabakar)*/
#endif
