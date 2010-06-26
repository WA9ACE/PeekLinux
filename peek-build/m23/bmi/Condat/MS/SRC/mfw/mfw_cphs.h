/*
+--------------------------------------------------------------------+
| PROJECT:  GSM-MFW (8417)              $Workfile:: mfw_cphs.h      $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED:  14.7.00                     $Modtime::                  $|
| STATE  :  code                                                     |
+--------------------------------------------------------------------+

	PURPOSE :  Types defination for CPHS management

	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a)Added function prototype "mfw_cphs_get_als_info()" which returns
				the current active line
	
	Oct 04, 2004    REF: CRR 25519     Deepa M.D
	Bug:Re-align structure members in MFW
	Fix:Structure elements have been  realigned to avoid the structure padding
*/ 

#ifndef DEF_MFW_CPHS
#define DEF_MFW_CPHS
#include <string.h>

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

#include "mfw_mfw.h"
#include "mfw_nm.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_phb.h"
#include "mfw_phbi.h"
#include "mfw_cm.h"
#include "mfw_cmi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"

#include "drv_key.h"


#include "ksd.h"

#if defined (FAX_AND_DATA)
#include "aci_fd.h"
#endif


#include "phb.h"


#include "mfw_ss.h"
#include "mfw_ssi.h"


#include "mfw_sys.h"

#ifdef NEPTUNE_BOARD
#include "Ffs.h" 
#endif 


/* Constants define */
#define MFW_INVALID_SIM_DATA	0xFF
#define MAX_CPHS_ENTRY          4
#define MAX_PCM_MAILBOX_LEN     10

typedef enum              /* CPHS status */
{
  CPHS_NotPresent = 10,
  CPHS_ERR,
  	//x0pleela 11 June, 2006  DR:OMAPS00079692
	 //changed from CPHS_OK to MFW_CPHS_OK
  MFW_CPHS_OK
} T_MFW_CPHS_STATUS;

typedef enum              /* SIM operation status */
{
  MFW_SIMOP_UNKNOWN = 20,
  MFW_SIMOP_WRITE_ERR,
  MFW_SIMOP_WRITE_OK,
  MFW_SIMOP_READ_ERR,
  MFW_SIMOP_READ_OK
} T_MFW_SIMOP_STATUS;

typedef enum              /* CPHS service */
{
  MFW_CPHS_CSP = 1,             /* customer service profile */
  MFW_CPHS_SST,                 /* service string table     */
  MFW_CPHS_MB_NUMBER,           /* mailbox numbers          */
  MFW_CHPS_NAME_SHORT,          /* operator name shortform  */
  MFW_CHPS_INFO_NUMBERS         /* information numbers      */
} T_MFW_CPHS_SERVICE;

typedef enum              /* index of mailbox-number/alternate-line service */
{
  MFW_SERV_NotPresent = -1,
  MFW_SERV_LINE1      = 0,
  MFW_SERV_LINE2,
  MFW_SERV_DATA,
  MFW_SERV_FAX
} T_MFW_LINE_INDEX;

typedef enum              /* status of alternate line service */
{
  MFW_LINE_UNLOCKED = 0,
  MFW_LINE_LOCKED
} T_MFW_LINE_STATUS;

typedef enum              /* voice message waiting flag and
                             call forwarding flags           */
{
  MFW_CFLAG_NotPresent = 0,
  MFW_CFLAG_SET        = 0x0A,
  MFW_CFLAG_NOTSet     = 0x05
} T_MFW_CFLAG_STATUS;

/* events are organised as bitmaps */
#define E_CPHS_IND              0x0001
#define E_CPHS_OP_NAME          0x0002
#define E_CPHS_GET_VC_NUM       0x0004
#define E_CPHS_SET_VC_NUM       0x0008
#define E_CPHS_GET_VC_STAT      0x0010
#define E_CPHS_SET_VC_STAT      0x0020
#define E_CPHS_GET_DV_STAT      0x0040
#define E_CPHS_SET_DV_STAT      0x0080
#define E_CPHS_GET_ALS_STATUS   0x0100
#define E_CPHS_SET_LINE                 0x0200
#define E_CPHS_SET_ALS_STATUS   0x0400
#define E_CPHS_GET_CSP          0x0800
#define E_CPHS_GET_INFO_NUM     0x1000
#define E_CPHS_GET_INFO_LIST    0x2000
#define E_CPHS_GET_SIM_FIELD	0x4000 //MC for generic sim reads
#define E_CPHS_SET_SIM_FIELD	0x8000 //sbh for generic sim writes
#define E_CPHS_MSP_IND	0x10000
#ifdef FF_CPHS_REL4
#define E_CPHS_REL4_GET_MBI 0x20000
#define E_CPHS_REL4_GET_MBDN 0x40000
#endif

typedef struct  /* Character */
{
  UBYTE len;                    /* length of string data        */
  UBYTE data[PHB_MAX_LEN];      /* string data                  */
} T_MFW_STRING;

typedef struct	/* Information numbers */
{
  UBYTE          index;
  UBYTE          entryStat;    /* entry status     */
  T_MFW_PHB_TEXT alpha;        /* alpha identifier */
} T_MFW_CPHS_INFO_NUMBER;

typedef struct	/* List of information numbers */
{
  UBYTE count;                /* number of read entries */
  UBYTE level;
  T_MFW_CPHS_INFO_NUMBER entry[MAX_CPHS_ENTRY];  /* entries          */
} T_MFW_CPHS_INFONUM_LIST;

typedef struct /* CPHS information */
{
  UBYTE phase;
  UBYTE opNS;       /* operator name shortform  */
  UBYTE mbNum;      /* mailbox numbers          */
  UBYTE sst;        /* service string table     */
  UBYTE csp;        /* customer service profile */      
  UBYTE iNum;       /* information numbers      */
} T_MFW_CPHS_INFO;

typedef struct  /* Operator name */
{
  T_MFW_LNAME  longName;       /* operator name string    */
  T_MFW_SNAME  shortName;      /* operator name shortform */
} T_MFW_CPHS_OP_NAME;

#define MFW_CPHS_VM_NUMBER_MAXLEN ( PHB_MAX_LEN + 20 )

typedef struct  /* Mailbox number */
{
  UBYTE          index;                /* index in phonebook table       */
  T_MFW_PHB_TEXT alpha;                /* alpha identifier               */  
  UBYTE          number[MFW_CPHS_VM_NUMBER_MAXLEN];  /* telephone number               */
  UBYTE          ton;                  /* type of number                 */ 
  UBYTE          npi;                  /* numbering plan identifier      */ 
  UBYTE          service;              /* indicates the line or fax/data */
  UBYTE			 ext_rec_number;	   /* Number of EXT Records */
  UBYTE          ext_rec_idx[4]; 
  UBYTE          datalen;  
  UBYTE          list_index;           /* Index / Position in the List */
} T_MFW_CPHS_ENTRY;

typedef struct  /* List of mailbox numbers */
{
  UBYTE count;                      /* numbers of the list */
  T_MFW_CPHS_ENTRY entries[MAX_CPHS_ENTRY];
} T_MFW_CPHS_VC_NUM;

/*Data structure to store MBI of registered profile*/
#ifdef FF_CPHS_REL4
typedef struct
{
  UBYTE voice; /*stores record id of voice*/
  UBYTE fax; /*stores record id of fax*/
  UBYTE electronic; /*stores record id of electronic*/
  UBYTE other;  /*stores record id of other*/
  UBYTE valid; /*1 - the info stored is valid
                       0 - the info stored is invalid*/
} T_MFW_CPHS_REL4_MBI;

typedef struct
{
  CHAR    alpha[MAX_ALPHA_LEN];
  CHAR    number[MAX_MB_NUM_LEN];
  T_ACI_TOA    type;
} T_MFW_MBDN;

typedef struct
{
  T_MFW_MBDN mailbox_numbers[4];/*mailbox numbers of voice,fax,electronic and other*/
  UBYTE valid; /* 1st bit-voicemail, 2nd bit-fax,3rd bit-electronic,4th bit-other. bit set if valid, bit unset if invalid*/                      
} T_MFW_CPHS_REL4_MBDN;

typedef enum
{
   T_MFW_CPHS_REL4_MAILBOX_VOICE=0,
   T_MFW_CPHS_REL4_MAILBOX_FAX,
   T_MFW_CPHS_REL4_MAILBOX_ELECTRONIC,
   T_MFW_CPHS_REL4_MAILBOX_OTHER   
} T_MFW_CPHS_REL4_MAILBOX_TYPE;

#endif

typedef struct  /* Voice message waiting information */
{
  UBYTE result;             /* read error/no error      */
  T_MFW_CFLAG_STATUS line1; /* waiting status of line 1 */
  T_MFW_CFLAG_STATUS line2; /* waiting status of line 2 */
  T_MFW_CFLAG_STATUS fax;   /* waiting status of fax    */
  T_MFW_CFLAG_STATUS data;  /* waiting status of data   */
} T_MFW_CPHS_VC_STAT;

/*Data structure to store MWIS of a profile*/
#ifdef FF_CPHS_REL4
typedef struct
{
  UBYTE mwisStat;
  UBYTE voicemail;
  UBYTE fax;
  UBYTE electronic;
  UBYTE other;  
} T_MFW_CPHS_REL4_MWIS;
#endif

typedef struct  /* Status of diverted call indicators */
{
  UBYTE result;             /* read error/no error                            */
  T_MFW_CFLAG_STATUS line1; /* call forwarding unconditional status of line 1 */
  T_MFW_CFLAG_STATUS line2; /* call forwarding unconditional status of line 2 */
  T_MFW_CFLAG_STATUS fax;   /* call forwarding unconditional status of fax    */
  T_MFW_CFLAG_STATUS data;  /* call forwarding unconditional status of data   */
} T_MFW_CPHS_DV_STAT;

/*a0393213 cphs rel4 - added a structure for divert status*/
#ifdef FF_CPHS_REL4
typedef struct  /* Status of diverted call indicators */
{
  T_MFW_CFLAG_STATUS voice; /* call forwarding unconditional status of voice */
  T_MFW_CFLAG_STATUS fax;   /* call forwarding unconditional status of fax    */
  T_MFW_CFLAG_STATUS data;  /* call forwarding unconditional status of data   */
  char voice_dialling_number[PHB_MAX_LEN]; /*number to which voice has been forwarded*/  
} T_MFW_CPHS_REL4_DV_STAT;
#endif

typedef struct /* Information about alternate line service */
{
  T_MFW_LINE_INDEX  selectedLine;       
  T_MFW_LINE_STATUS status;         /* ALS is locked or unlocked */
} T_MFW_CPHS_ALS_STATUS;


//Code 15:
typedef struct  /* Customer service profile */
{
  UBYTE result;            /* read error/no error       */
  UBYTE csp[20];           /* size of MFW_CPHS_CSP_SIZE */
} T_MFW_CPHS_CSP;
//Code 15 END



typedef struct  /* Information number entry */
{
  UBYTE          result;         /* read error/no error (only used for read information number) */
  UBYTE          index;
  UBYTE          entryStat;
  T_MFW_PHB_TEXT alpha;          /* alpha identifier           */
  UBYTE          number[4];
  UBYTE          ton;
  UBYTE          npi;
} T_MFW_CPHS_INFONUM_ENTRY;

#ifdef FF_CPHS_REL4
typedef struct
{
	char alpha[MAX_ALPHA_LEN];
	char number[MAX_PHB_NUM_LEN];
	T_ACI_TOA         type;
	T_ACI_BS_SPEED    speed;
	T_ACI_CNUM_SERV   service;
	T_ACI_CNUM_ITC    itc;
}T_MFW_CPHS_MSP_REC;

typedef struct
{
	UBYTE count;
	T_MFW_CPHS_MSP_REC profile[4];
	UBYTE default_profile;
	UBYTE registered_profile;
}T_MFW_CPHS_MSP_INFO;
#endif

/***************************Go-lite Optimization changes Start***********************/
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_CPHS_READ_SIM_ENTRY)are realigned to avoid the structure padding
typedef struct  /* SIM read data */
{
	
	UBYTE*          	exchange_data;
	USHORT          	sim_result;         /* read error/no error (only used for read information number) */
	USHORT				requested_field;
	UBYTE				data_len;
	UBYTE				record;				/* sh */ 
} T_MFW_CPHS_READ_SIM_ENTRY;
/***************************Go-lite Optimization changes end***********************/

/* CPHS Parameter */
typedef union
{
  T_MFW_CPHS_STATUS        stat;
  T_MFW_CPHS_OP_NAME       oper_name;
  T_MFW_CPHS_VC_NUM        vc_entry;
  UBYTE                    result; 
  T_MFW_CPHS_VC_STAT       vc_status;
  T_MFW_CPHS_DV_STAT       dv_status;
  T_MFW_CPHS_ALS_STATUS    als_status;
  T_MFW_CPHS_CSP           csp;
  T_MFW_CPHS_INFONUM_LIST  num_list;
  T_MFW_CPHS_INFONUM_ENTRY info_entry;
  T_MFW_CPHS_READ_SIM_ENTRY read_sim_entry;
} T_MFW_CPHS_PARA;

/* prototypes define */
void    cphs_init               (void);
void    cphs_exit               (void);
MfwHnd  cphs_create             (MfwHnd hWin, MfwEvt event, MfwCb cbfunc);
MfwRes  cphs_delete             (MfwHnd h);
T_MFW   cphs_config             (T_MFW_CPHS_INFO *cphs_info);
T_MFW   cphs_operator_name      (void);
void    cphs_get_mailbox        (void);
T_MFW   cphs_set_mailbox        (T_MFW_CPHS_ENTRY *entry);
T_MFW   cphs_get_mailbox_status (void);
T_MFW   cphs_set_mailbox_status (T_MFW_CFLAG_STATUS line1, 
                                 T_MFW_CFLAG_STATUS line2, 
                                 T_MFW_CFLAG_STATUS fax, 
                                 T_MFW_CFLAG_STATUS data);
T_MFW   cphs_get_divert_status  (void);
T_MFW   cphs_set_divert_status  (T_MFW_CFLAG_STATUS line1, 
                                 T_MFW_CFLAG_STATUS line2, 
                                 T_MFW_CFLAG_STATUS fax, 
                                 T_MFW_CFLAG_STATUS data);
/*a0393213 cphs rel4*/
#ifdef FF_CPHS_REL4
T_MFW cphs_rel4_set_divert_status(T_MFW_CFLAG_STATUS voice, T_MFW_CFLAG_STATUS fax, 
										T_MFW_CFLAG_STATUS data, char number[PHB_MAX_LEN]);
void sendCPHSSignal(UBYTE evt, void* para);
T_MFW_CPHS_MSP_INFO* mfw_cphs_get_msp(void);
T_MFW_CPHS_REL4_MWIS* get_mwis_stat(int);
#endif
void    cphs_get_als_info       (T_MFW_CPHS_ALS_STATUS *info);
void    cphs_select_line        (T_MFW_LINE_INDEX line);
T_MFW   cphs_set_als_status     (T_MFW_LINE_STATUS status);
T_MFW   cphs_get_csp            (void);
T_MFW   cphs_get_info_num       (UBYTE level, UBYTE startIndex);
T_MFW   cphs_select_info_num    (UBYTE index);
BOOL	Read_Sim						(USHORT dat_id, USHORT offset, UBYTE max_len, UBYTE *exDat);
BOOL	Read_Sim_Record					(USHORT dat_id, USHORT record, UBYTE max_len, UBYTE *exDat);
BOOL	Write_Sim						(USHORT dat_id, UBYTE len, UBYTE *exDat);
BOOL	Write_Sim_Record				(USHORT dat_id, USHORT record, UBYTE len, UBYTE *exDat);

//x0pleela 25 May2006  DR: OMAPS00070657
#ifdef FF_CPHS
T_MFW mfw_cphs_get_als_info(void);
#endif

/* Added to remove warning Aug - 11 */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
#ifdef PCM_2_FFS
          EXTERN T_FFS_SIZE ffs_ReadRecord(const char *name, void * addr, int size, int index, int recsize);
#endif  /* PCM_2_FFS */ 
/* End - remove warning Aug - 11 */

/*a0393213 cphs rel4*/
#ifdef FF_CPHS_REL4
UBYTE mfw_cphs_get_reg_profile(void);
#endif

#endif



