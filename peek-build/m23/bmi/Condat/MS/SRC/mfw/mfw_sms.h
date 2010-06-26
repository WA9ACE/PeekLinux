/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sms.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 21.12.98                      $Modtime:: 21.03.00 10:48  $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SMS

   PURPOSE : This modul contains the definition for SMS management.


   $History:: mfw_sms.h                                              $

      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

	Feb 07, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Renamed "MFW_SMS_STATRPT_PENDG" to MFW_SMS_STATRPT in T_MFW_SMS_STAT
   			Removed T_MFW_SMS_MT_TYPE enumeration added earlier
   			
      	Feb 01, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added new enumeration T_MFW_SMS_MSG_TYPE to suport message type MT, MO or STATUS REPORT. 
   			Changed the name from T_MFW_SMS_MSG_TYPE to T_MFW_SMS_MT_TYPE
   			Changed the type of new field  sms_msg_type ( of type T_MFW_SMS_MT_TYPE) in the structure T_MFW_SMS_MT.
   			Added new enumeration type "MFW_SMS_STATRPT_PENDG" into T_MFW_SMS_STAT
   			
      	Jan 30, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added new enumeration T_MFW_SMS_MSG_TYPE to suport message type either TEXT or STATUS REPORT. 
   			Added new field  sms_msg_type to the structure T_MFW_SMS_MT.

   	Jan 20, 2006    REF: DRT OMAPS00053377 xdeepadh
   	Description: SMS Validity Period - using MMI - Failure, MS not able to set SMS validity
   	Solution: Store the first octet too while querying.

       Dec 15, 2005   REF: MMI-FIX-OMAPS00058684 x0035544
       Description: OMAPS00045726 / MMI-FIX-30439 Follow-Up : ifdef for FF Home Zone forgotten.
       Solution: Added Feature flag FF_HOMEZONE for the function sms_homezoneCBToggle(U8 Activate_CB)
       in this file as it has got the prototype for this function
       
	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.

	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.
	
      	Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
  	Issue description:  BMI: using the tool nowSMS the voicemail indication via sms does not work
  	Solution: Instead of identifying voice using addr[0] and pid, we use dcs. Also, messages belonging
  	              to group "1100" are deleted when voicemail icon is displayed.

	Oct 04, 2004    REF: CRR 25519     Deepa M.D
	Bug:Re-align structure members in MFW
	Fix:Structure elements have been  realigned to avoid the structure padding

  * *****************  Version 27  ***************** 
   SPR 2260:  Merged in bugfixes 1833,  and 1830 
   fom the 1.6.3 branch into this file.
 * 
 * *****************  Version 26  *****************
 * User: Vo           Date: 22.03.00   Time: 17:44
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * check voice mail status
 * 
 * *****************  Version 24  *****************
 * User: Vo           Date: 14.01.00   Time: 20:15
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: Constant  define:  MAX_CBMSG_LEN
 * New:change parameter "data" to "data + len" in function
 * sms_submit(), sms_store() and sms_change() 
 * 
 * *****************  Version 23  *****************
 * User: Vo           Date: 2.12.99    Time: 13:15
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 22  *****************
 * User: Vo           Date: 17.11.99   Time: 12:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * change define NON_STRING_CHARACTER to NO_ASCIIZ
 * 
 * *****************  Version 21  *****************
 * User: Vo           Date: 17.11.99   Time: 11:00
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 1. Change constants: T_MFW_SMS_MEM
 * 2. New constants: T_MFW_SMS_MO_TYPE  for submit message type
 * 3. New constants: T_MFW_SMS_CMD_IND for command indication
 * 4. #ifdef NON_STRING_CHARACTER  - alpha uses data + len
 * 5. Change interface: sms_submit()
 * 6. New interface: sms_change_read_status()
 * 
 * *****************  Version 20  *****************
 * User: Vo           Date: 21.10.99   Time: 13:35
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * change constant define
 * 
 * *****************  Version 19  *****************
 * User: Es           Date: 16.09.99   Time: 17:23
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * added TON/NPI fields to T_MFW_SMS_ADDR
 * 
 * *****************  Version 18  *****************
 * User: Vo           Date: 12.09.99   Time: 12:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * change parameter 'dcs' in T_MFW_SMS_CB_INFO (adaptation to ACI)
 * 
 * *****************  Version 17  *****************
 * User: Vo           Date: 10.09.99   Time: 19:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * move sms_save_pref_lang() to sim_save_pref_lang()
 * 
 * *****************  Version 16  *****************
 * User: Vo           Date: 30.08.99   Time: 15:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new events
 * 
 * *****************  Version 15  *****************
 * User: Vo           Date: 26.07.99   Time: 14:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * Add new definitions
 * 
 * *****************  Version 14  *****************
 * User: Vo           Date: 16.07.99   Time: 13:50
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 13  *****************
 * User: Vo           Date: 16.07.99   Time: 10:39
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 12  *****************
 * User: Vo           Date: 22.06.99   Time: 8:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 11  *****************
 * User: Vo           Date: 1.06.99    Time: 14:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 10  *****************
 * User: Vo           Date: 28.05.99   Time: 9:52
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 9  *****************
 * User: Vo           Date: 21.05.99   Time: 13:51
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 8  *****************
 * User: Vo           Date: 18.05.99   Time: 14:35
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 7  *****************
 * User: Vo           Date: 25.03.99   Time: 16:34
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 6  *****************
 * User: Vo           Date: 25.02.99   Time: 16:47
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 4  *****************
 * User: Vo           Date: 11.02.99   Time: 15:43
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 3  *****************
 * User: Vo           Date: 11.02.99   Time: 12:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Vo           Date: 7.01.99    Time: 12:38
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Vo           Date: 30.12.98   Time: 11:49
 * Created in $/GSM/DEV/MS/SRC/MFW

*/

#ifndef DEF_MFW_SMS_HEADER
#define DEF_MFW_SMS_HEADER

#include "mfw_cm.h"
#include "aci_lst.h"
#include "conc_sms.h"

#ifndef NEPTUNE_BOARD
//#define MAX_IDENTS          0x14
#else
#define MAX_IDENTS          0x14 /* NEPTUNE Integration: Uncommented.*/
#endif

#define MAX_SCTP_DIGITS        2
#define BASIC_MAX_MSG_LEN   160
#define MAX_CBMSG_LEN         94
//1227 - API - 05/10/02 - Create new Define for Single SMS and increase original for Conc_SMS
#define MAX_MSG_LEN_SGL		   161 //Max message length for disabled Conc SMS API

/*JVJ-SPR-1298. Provisional until a new option in the .inf file is inserted.*/

#ifdef MMI_LITE
/*SPR 2350, concat messagse of up to 500 chars should be allowed*/
#define MAX_MSG_LEN_LITE		(MAX_MSG_LEN_SGL*3)				
#define MAX_MSG_LEN          MAX_MSG_LEN_LITE
#else
#define MAX_MSG_LEN          1601 //Max size for a SMS	
#endif

#define MAX_MSG_LEN_ARRAY   (MAX_MSG_LEN+1) //Array size for SMS


/*MC, chinese message lengths*/
#define MAX_MSG_LEN_SGL_CHI	   140 //Max message length for single chinese message
#define MAX_MSG_LEN_CHI				(MAX_MSG_LEN_SGL_CHI*10)			 


/*
 * events are organised as bitmaps
 */
#define E_SMS_MO_AVAIL         0x0001
#define E_SMS_CMD_AVAIL        0x0002
#define E_SMS_SAVE_AVAIL       0x0004
#define E_SMS_MT               0x0008
#define E_SMS_MO               0x0010
#define E_SMS_CB               0x0020
#define E_SMS_CB_RECEIVED      0x0040
#define E_SMS_MT_RECEIVED      0x0080
#define E_SMS_STATUS           0x0100
#define E_SMS_MEM              0x0200
#define E_SMS_OK               0x0400
#define E_SMS_ERR              0x0800
#define E_SMS_BUSY             0x1000
#define E_SMS_READY            0x2000
#define E_SMS_MEM_FULL         0x4000
#define E_SMS_MEM_FREE         0x8000

#define E_SMS_RECEIVED          0x0600
#define E_SMS_ALL_SERVICE       0xffff

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
//liuyu
/* SPR#2086 */
#define E_SMS_WAP_MT_RECEIVED	0x10000
#define E_SMS_WAP_MO_SENT     0x20000
#define E_SMS_WAP_ERR        	 0x40000
#endif

/* definition for validity period mode */
#define SMS_VPF_NOTPRESENT 0
#define SMS_VPF_NUMERICAL   2  // Corresponds to relative period
#define SMS_VPF_STRING      3  // Corresponds to absolute period

/* definition for reply path */
#define SMS_UNSET_RP        0
#define SMS_SET_RP          1

/* definition for reject duplicates */
#define SMS_ACCEPT_RD        0
#define SMS_REJECT_RD        1

/* definition for status report request */
#define SMS_NOT_REQUESTED_SRR 0
#define SMS_REQUESTED_SRR     1

/* SMS management functional return codes */
typedef enum              
{
  MFW_SMS_FAIL = -1,         /* execution of command failed    */
  MFW_SMS_OK                 /* execution of command completed */
} T_MFW_SMS_RETURN;

/* definition for message type */
typedef enum              
{
  MFW_NO_MESSAGE,
  MFW_SMS_MESSAGE,           /* SMS message            */
  MFW_CB_MESSAGE             /* cell broadcast message */
} T_MFW_SMS_TYPE;

/* definition for submit message type */
typedef enum              
{
  MFW_SMS_SUBMIT,
  MFW_SMS_REPLY,
  MFW_SMS_SUBMIT_SENT,
  MFW_SMS_REPLY_SENT
} T_MFW_SMS_MO_TYPE;

/* Memory which messages are stored */
typedef enum
{
  MFW_MEM_NOTPRESENT=-1,  /*a0393213 OMAPS00109953*/
  MFW_MEM_ME,
  MFW_MEM_SIM
} T_MFW_SMS_MEM;

// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar
// Added MFW_SMS_VOICEMSG_DELETE to enum T_MFW_SMS_STAT

/* Memory which messages are stored */
typedef enum
{
  MFW_SMS_UNKNOWN,
  MFW_SMS_UNREAD,       /* received unread message */
  MFW_SMS_READ,         /* received read message   */
  MFW_SMS_STOR_UNSENT,  /* stored unsent messgae   */
  MFW_SMS_STOR_SENT,    /* stored sent message     */
  MFW_SMS_MO,           /* MO SMS message          */
  MFW_SMS_MT,           /* MT SMS message          */
  MFW_SMS_ALL,
  MFW_SMS_NotVOICE,
  MFW_SMS_VOICE,        /* voice mail message */
  MFW_SMS_VOICEMSG_DELETE,  // To mark the voice mail sms for deletion (group 1100)
  MFW_SMS_STATRPT, ////x0pleela 01 Feb, 2006 DR: OMAPS00059261 /* Status report status*/ //x0pleela 06 Feb, 2006 DR: OMAPS00059261 Removed pending from name
  MFW_SMS_INVALID   /*SPR2260*/
} T_MFW_SMS_STAT;

/*SPR 2260, eneumerated type for concat status of message in list*/
typedef enum
{
	MFW_SMS_NO_CONC,
	MFW_SMS_CONC_PART,
	MFW_SMS_CONC_WHOLE
}
T_MFW_SMS_CONC_STAT	;
	
typedef struct
{
  UBYTE year     [MAX_SCTP_DIGITS];
  UBYTE month    [MAX_SCTP_DIGITS];
  UBYTE day      [MAX_SCTP_DIGITS];
  UBYTE hour     [MAX_SCTP_DIGITS];
  UBYTE minute   [MAX_SCTP_DIGITS];
  UBYTE second   [MAX_SCTP_DIGITS];
  SHORT timezone;
} T_MFW_SMS_SCTP;

/* SMS command */
typedef enum              
{
  SMS_ENQ_PREV_SMS,         /* Starts an enquiry to previously sended short message */
  SMS_CAN_STAT_REQ,         /* Cancels status report request                        */
  SMS_DEL_PREV_SMS,         /* Deletion of previous sended short message            */
  SMS_REQ_STAT_REP          /* Requests status report for SMS message               */
} T_MFW_SMS_COMMAND;

typedef enum
{
  MT_IND_IDX,               /* message will be stored to memory and index will 
                                be indicated (E_SMS_MT_RECEIVED) */
  MT_IND_NO_IDX,            /* message will be stored to memory and index will 
                                not be indicated */
  MT_IND_MSG,               /* message will be indicated to mmi (E_SMS_MT)
                                but not stored to memory */
  MT_CB_IDX,                /* CB message will be stored to memory and index will 
                                be indicated (E_SMS_CB_RECEIVED) */
  MT_CB_NO_IDX,             /* CB message will be stored to memory and index will 
                                not be indicated */
   MT_CB_MSG,                /* CB message will be indicated to mmi (E_SMS_CB)
                                but not stored to memory */
  /* Marcus: Issue 1170: 07/10/2002: Start */
  MT_IND_SRR_ON,            /* Status reports are to be notified */
  MT_IND_SRR_OFF            /* Status reports are not to be notified */
  /* Marcus: Issue 1170: 07/10/2002: End */

  } T_MFW_SMS_MT_IND;

typedef enum
{
  CMD_SMS_NO_CMD,
  CMD_SMS_SUBMIT,           /* send message */
  CMD_SMS_MEMORY,           /* set/get preferred message store */
  CMD_SMS_READ,             /* read message */
  CMD_SMS_STORE,            /* save message */
  CMD_SMS_DELETE,           /* delete message */
  CMD_SMS_COMMAND,          /* send command */
  CMD_SMS_SET_PARA,         /* set SMS parameter */
  CMD_SMS_SET_CB_PARA,      /* set cell broadcast parameter */
  CMD_SMS_SET_IND           /* set new message indication */
  } T_MFW_SMS_CMD_IND;

//x0pleela 01 Feb, 2006 DR: OMAPS00059261
/* SMS type: MT, MO, STAT RPT */

typedef enum
{
  MFW_MSG_DELIVER,
  MFW_MSG_SUBMIT,
  MFW_MSG_STATUS_REPORT
}T_MFW_SMS_MSG_TYPE;

typedef struct
{
  UBYTE mem;
  UBYTE used;
  UBYTE total;
  UBYTE dummy;
} T_MFW_SMS_MEM_INFO;

/* SMS parameters */
typedef struct	
{
  CHAR              sc_addr[MAX_LEN]; /* service centre address   */
  SHORT	            prot_id;	      /* protocol identifier      */
  SHORT	            dcs;              /* data coding scheme       */
  UBYTE	            srr;              /* status report request    */
  UBYTE	            rp;               /* reply path               */
  UBYTE	            rd;               /* reject duplicates        */
  UBYTE	            vp_mode;	      /* validity period mode     */
  UBYTE	            vp_rel;	          /* validity period relative */
  T_MFW_SMS_SCTP    vp_abs;           /* validity period absolute */
   SHORT            first_oct;     		/* First Octet*/
} T_MFW_SMS_INFO;

/* Identification to incomming message */
typedef struct
{
  UBYTE mem;             /* indicate the memory for stored message */
  UBYTE index;           /* index of message                       */
  UBYTE concat_status;	  /* status of concatenated */
  UBYTE received_sms_type;  /* type of sms */
} T_MFW_SMS_ID;

/* Information about mobile terminated short message */
typedef struct
{
  CHAR  tag[MFW_TAG_LEN];
#ifdef NO_ASCIIZ
  UBYTE len;	/* length of name              */
  UBYTE dummy1;
  UBYTE dummy2;
  UBYTE dummy3;
#endif
  CHAR  number[MFW_NUM_LEN];
  T_MFW_PHB_TON ton;
  T_MFW_PHB_NPI npi;
} T_MFW_SMS_ADDR;


typedef struct
{
  UBYTE             index;                /* index of message           */
  T_MFW_SMS_ADDR    orig_addr;            /* call party address         */
  CHAR              sc_addr[MAX_LEN];     /* service centre address     */
  UBYTE             prot_id;              /* protocol identifier        */
  SHORT             dcs;                  /* data coding scheme         */
  UBYTE				      rp;					          /* reply path					*/
  UBYTE					ti;								/* type indicator */
  T_MFW_SMS_SCTP    sctp;                 /* service centre timestamp   */
  USHORT/*UBYTE*/       msg_len;              /* length of short message    */
  CHAR              sms_msg[MAX_MSG_LEN_ARRAY]; /* short message              */
  UBYTE             udh_len;              /* length of user data header */
  UBYTE             udh[BASIC_MAX_MSG_LEN];     /* user data header           */
  T_MFW_SMS_STAT		sms_msg_type;	//x0pleela 01 Feb, 2006 DR: OMAPS00059261 /* sms message type: MT text or MT status report*/
  BOOL				voicemail;/*SPR#1408 - DS - TRUE if a voicemail message. Port of 1669*/
  T_MFW_SMS_MEM memory;	/*a0393213 OMAPS00109953 - memory (SIM/ME) where the message is stored*/
} T_MFW_SMS_MT;

/* Incomming call broadcast message */
typedef struct
{
  SHORT        sn;                  /* serial number in integer format */
  USHORT       msg_id;              /* message identification          */
  UBYTE        dcs;                 /* data coding scheme              */
  UBYTE        page;                /* number of this page             */
  UBYTE        pages;               /* number of total pages           */
  UBYTE        msg_len;             /* length of short message         */
  CHAR         cb_msg[MAX_CBMSG_LEN]; /* cell broadcast message          */
} T_MFW_SMS_CB;

/* Parameter for cell broadcast message */
typedef struct
{
  UBYTE   mode;                         /* acception mode: 0 - accept, 1 - not accept */
#ifdef FF_2TO1_PS
  USHORT  msg_id[CBM_MAX_RANGE_NUMBER]; /* message identifier              */
  UBYTE   dcs[CBM_MAX_RANGE_NUMBER];    /* data coding schemes             */
#else
  USHORT  msg_id[MAX_IDENTS];   /* message identifier              */
  UBYTE   dcs[MAX_IDENTS];      /* data coding schemes             */
#endif
  UBYTE	  dummy;
} T_MFW_SMS_CB_INFO;

/* Information for mobile originated short message */
typedef struct
{
  T_MFW_SMS_ADDR    dest_addr;            /* called party address        */
  CHAR              sc_addr[MAX_LEN];     /* service centre address      */
  UBYTE             prot_id;              /* protocol identifier         */
  SHORT             dcs;                  /* data coding scheme          */
  UBYTE             status;               /* status of sms message       */
  UBYTE             vp_mode;              /* validity period mode        */
  UBYTE             vp_rel;               /* validity period relative    */
  T_MFW_SMS_SCTP    vp_abs;               /* validity period absolute    */
  CHAR              sms_msg[MAX_MSG_LEN_ARRAY]; /* short message               */
  USHORT             msg_len;              /* length of short message     */
  UBYTE             msg_ref;              /* message reference           */
  T_MFW_SMS_MEM memory;		/*a0393213 OMAPS00109953 - memory(SIM/ME) where the message is saved*/
} T_MFW_SMS_MO;

/* Status report for short message */
typedef struct
{
  SHORT           msg_id;              /* message type           */
  UBYTE           msg_ref;             /* message Reference      */
  T_MFW_SMS_ADDR  addr;                /* called party address   */  
  UBYTE           status;              /* status of sms message  */
  T_MFW_SMS_SCTP  rctp;                /* received sms timestamp */
} T_MFW_SMS_STATUS;
/***************************Go-lite Optimization changes Start***********************/
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_SMS_MSG)are realigned to avoid the structure padding
typedef struct
{
	T_MFW_SMS_STAT    stat;         
	T_MFW_SMS_STAT    msg_type;      /* voice mail status */
	T_MFW_SMS_CONC_STAT    concat_status;/*2260*/
	T_MFW_SMS_ADDR    addr;
	T_MFW_SMS_SCTP    rctp;
	SHORT             index;
	SHORT    first_index;/*index of first sms in concat assembly SPR 2260*/
	USHORT             msg_ref;
	T_MFW_SMS_MEM mem; /*a0393213 OMAPS00109953 - memory where the sms is stored (SIM or ME)*/
	T_MFW_SMS_MEM first_mem; /*a0393213 OMAPS00109953 - memory where the first segment in cSMS is stored (SIM or ME)*/
} T_MFW_SMS_MSG;/*structure used in message list*/
/***************************Go-lite Optimization changes end***********************/
// PATCH JPS 29.09: new structure used in mfw_index_list
typedef struct
{
  SHORT             index;
  T_MFW_SMS_STAT    stat;
  T_MFW_SMS_STAT   msg_type;
 T_MFW_SMS_CONC_STAT	concat_status;/*SPR 2260*/
 T_MFW_SMS_MEM memory; /*a0393213 OMAPS00109953 - memory where the sms is stored (SIM or ME)*/
} T_MFW_SMS_IDX;/*structure passed in sms_copy_idx() function,used externally*/
// PATCH JPS 29.09 END

/*
 * SMS event Parameter 
 */
typedef union
{
  T_MFW_SMS_ID        sms_id;
  T_MFW_SMS_MT        sms_mt;
  T_MFW_SMS_MO        sms_mo;
  T_MFW_SMS_CB        sms_cb;
  T_MFW_SMS_STATUS    sms_status;
  T_MFW_SMS_MEM_INFO  sms_mem;
  UBYTE               index;
  UBYTE               cmd_ind;
} T_MFW_SMS_PARA;

/*a0393213 OMAPS00109953 - SMS indexes are retrieved during bootup. 
   This enum shall be used to maintain the state machine during that process*/
typedef enum
{
   MFW_SMS_INDEX_NOT_RETRIEVED=0,
   MFW_SMS_SIM_INDEX_RETRIEVING,
   MFW_SMS_FFS_INDEX_RETRIEVING,
   MFW_SMS_INDEX_RETRIEVED   
} T_MFW_SMS_INDEX_RETRIEVE_STATE;

/*a0393213 OMAPS00109953 - this function updates the SMS memory details of SIM/ME*/
static void sms_update_memory();

EXTERN void      sms_init                    (void);
EXTERN void      sms_exit                    (void);
EXTERN T_MFW_HND sms_create                  (T_MFW_HND   hWin, 
                                              T_MFW_EVENT event, 
                                              T_MFW_CB cbfunc);
EXTERN T_MFW_RES sms_delete                  (T_MFW_HND h);

#ifdef NO_ASCIIZ

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
EXTERN T_MFW     sms_submit                  (T_MFW_SMS_MO_TYPE type, 
                                              CHAR *dest_addr,
                                              UBYTE *msg, 
                                              USHORT msg_len,
                                              CHAR *sca);
//Dec 14, 2005 REF: MMI-FIX-OMAPS00058684 x0035544
#ifdef FF_HOMEZONE
//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
EXTERN void      sms_homezoneCBToggle(U8 Activate); 
#endif
EXTERN T_MFW sms_submit_wap_sms(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,
                 T_MFW_PHB_TON addr_type,
                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca,
                 T_MFW_PHB_TON sc_type); /* SPR#2086 - SH - Added */
#else

EXTERN T_MFW     sms_submit                  (T_MFW_SMS_MO_TYPE type, 
                                              CHAR *dest_addr, 
                                              UBYTE *msg, 
                                              USHORT msg_len,
                                              CHAR *sca);
#endif
EXTERN T_MFW     sms_store                   (T_MFW_SMS_MO_TYPE type,
											                        CHAR *addr, 
                                              UBYTE *msg, 
                                              USHORT msg_len,
                                              CHAR *sca);
EXTERN T_MFW     sms_change                  (CHAR *addr, 
                                              UBYTE *msg, 
                                              USHORT msg_len,
                                              UBYTE index,
                                              T_MFW_SMS_MEM sms_memory	/*a0393213 OMAPS00109953 - memory added as parameter*/
							);
#else

#if defined (FF_WAP) && defined (FF_GPF_TCPIP)

EXTERN T_MFW     sms_submit                  (T_MFW_SMS_MO_TYPE type, 
                                              CHAR *dest_addr,
                 T_MFW_PHB_TON addr_type,
                                              UBYTE *msg, USHORT msg_len,
                                              CHAR *sca,
                 T_MFW_PHB_TON sc_type); // sbh added length, see function itself
EXTERN T_MFW sms_submit_wap_sms(T_MFW_SMS_MO_TYPE type,
                 CHAR *dest_addr,
                 T_MFW_PHB_TON addr_type,
                 UBYTE *msg,
                 USHORT msg_len,
                 CHAR *sca,
                 T_MFW_PHB_TON sc_type); /* SPR#2086 - SH - Added */
#else
EXTERN T_MFW     sms_submit                  (T_MFW_SMS_MO_TYPE type, 
                                              CHAR *dest_addr, 
                                              UBYTE *msg, USHORT msg_len,
                                              CHAR *sca); // sbh added length, see function itself
#endif

EXTERN T_MFW     sms_store                   (T_MFW_SMS_MO_TYPE type,
											                        CHAR *addr, 
                                              UBYTE *msg, USHORT msg_len,
											  CHAR *sca);	// sbh added length, see function itself
EXTERN T_MFW     sms_change                  (CHAR *addr, 
                                              UBYTE *msg, USHORT msg_len,
                                              UBYTE index,
                                              T_MFW_SMS_MEM sms_memory	/*a0393213 OMAPS00109953 - memory added as parameter*/
                                              ); // sbh added length, see function itself
#endif
EXTERN T_MFW     sms_stored_submit           (CHAR *dest_addr, 
                                              UBYTE index);
/*SPR 2260, added concatenated status parameter*/
EXTERN T_MFW sms_store_new_msg_info(UBYTE index, T_ACI_CMGL_SM* sm, UBYTE update_mem, T_CONC_ASSEMBLY_RETURN conc_stat
									   	,T_SMS_TYPE received_sms_type, T_MFW_SMS_MEM mem);   /*a0393213 OMAPS00109953 - memory added as parameter*/
									   	

EXTERN T_MFW sms_send_new_msg_ind(T_ACI_CMGL_SM* sm,UBYTE index, UBYTE mem, UBYTE received_sms_type, UBYTE concatenated_status);

EXTERN T_MFW     sms_set_pref_memory         (UBYTE mem);
EXTERN T_MFW     sms_read                    (UBYTE type, UBYTE index
											,T_MFW_SMS_MEM mem);  /*a0393213 OMAPS00109953 - memory added as parameter*/
											
EXTERN T_MFW     sms_mt_store                (T_MFW_SMS_MT *mt_msg, 
                                              T_MFW_SMS_STAT stat);
EXTERN T_MFW     sms_msg_delete      (UBYTE index
										, T_MFW_SMS_MEM sms_memory); /*a0393213 OMAPS00109953 - memory added as parameter*/
											
EXTERN T_MFW     sms_get_memset              (void);
EXTERN T_MFW     sms_command                 (CHAR *dest_addr, 
                                              UBYTE command, 
                                              UBYTE msg_ref);
EXTERN T_MFW     sms_parameter               (T_MFW_SMS_INFO *sms_info, BOOL fGet);
EXTERN T_MFW	  sms_set_SrvCntr_number	  (T_MFW_SMS_INFO *sms_info);
EXTERN T_MFW	  sms_set_dcs_number		(T_MFW_SMS_INFO *sms_info);//GW Added
EXTERN T_MFW	  sms_set_val_period		(T_MFW_SMS_INFO *sms_info);//GW Added
EXTERN T_MFW     sms_set_cell_broadcast_info (UBYTE mode, 
                                              T_MFW_SMS_CB_INFO *sms_cb_info);
EXTERN T_MFW     sms_get_cell_broadcast_info (T_MFW_SMS_CB_INFO *sms_cb_info);
EXTERN void      sms_set_mt_ind              (T_MFW_SMS_MT_IND sms_ind);
EXTERN SHORT     sms_message_count           (T_MFW_SMS_STAT stat);
EXTERN SHORT     sms_message_list            (T_MFW_SMS_STAT stat, 
                                              T_MFW_SMS_MSG * msg_list,
                                              int max_entries);
// PATCH JPS 29.09: add function equivalent of sms_message_list bu more economic
EXTERN SHORT     sms_index_list            (T_MFW_SMS_STAT stat, 
                                              T_MFW_SMS_IDX * msg_list,
                                              int max_entries);
// PATCH JPS 29.09 END
/*a0393213 OMAPS00109953 - memory added as parameter*/
EXTERN T_MFW     sms_change_read_status      (SHORT index
										, T_MFW_SMS_MEM sms_memory);

//x0018858 24 Mar, 2005 MMI-FIX-11321
//begin -MMI-FIX-11321
EXTERN T_MFW sms_set_cind_values(T_ACI_MM_CIND_VAL_TYPE *cindSettings);
EXTERN T_MFW sms_set_cmer_values(T_ACI_MM_CMER_VAL_TYPE *cmerSettings);
//end -MMI-FIX-11321

/* Added to remove warning */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);

#endif
