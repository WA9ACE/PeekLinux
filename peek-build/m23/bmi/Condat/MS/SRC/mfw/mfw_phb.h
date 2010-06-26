/*
+--------------------------------------------------------------------+
| PROJECT:  GSM-MFW (8417)              $Workfile:: mfw_phb.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED:  7.1.99                      $Modtime:: 24.03.00 11:47   $|
| STATE  :  code                                                     |
+--------------------------------------------------------------------+

	PURPOSE :  Types defination for phonebook management
    . 
*/ 

#ifndef DEF_MFW_PHB
#define DEF_MFW_PHB

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#ifdef FF_2TO1_PS
#include "p_8010_127_sim_sap.h"	//GW 22/11/01 - Added 
#else
#include "prim.h"
#endif
#include "phb.h"	//GW 22/11/01 - Added
#ifdef NEPTUNE_BOARD
#include "Ffs.h" 
#endif 
/*
 * events are organised as bitmaps
 */
#define E_PHB_STATUS              0x0001
#define E_PHB_ERROR               0x0002
#define E_PHB_UPN_LIST            0x0004
#define E_PHB_READY               0x0008
#define E_PHB_BUSY                0x0010

#define PHB_MAX_DIGITS     2
#define PHB_MAX_ENTRY_NUM  5

/* SPR#1112 - Internal phonebook */
#ifdef INT_PHONEBOOK
#define DEFAULT_PB		0
#endif

#ifdef TI_PS_FFS_PHB
//#define PHB_MAX_LEN        MAX_PHB_NUM_LEN /* Phonebook number length + '\0' */
#define PHB_MAX_LEN        (44+1) /* Phonebook number length + '\0' */
#else
//GW-SPR#762 For 44 digit phonebook, see aci_cmh.h
#ifdef PHONEBOOK_EXTENSION
#define PHB_MAX_LEN        (44+1) /* Phonebook number length + '\0' */
#else
#define PHB_MAX_LEN        (20+1) /* Phonebook number length + '\0' */
#endif /* else, #ifdef PHONEBOOK_EXTENSION */
#endif /* else, #ifdef TI_PS_FFS_PHB */

#define MAX_UPN_NUM        6

#define PHB_UNRESTRICTED   0
#define PHB_RESTRICTED     1
#define PHB_LOADING        2
#define PHB_NOT_ALLOCATED  3

#define PHB_AUTO_OFF       0
#define PHB_AUTO_ON        1


/* global constants */

typedef enum     /* phonebook type */
{
    PHB_ECC = 0,         /* Emergency call numbers        */
    PHB_ADN,             /* Abbreviated dialing number    */
    PHB_FDN,             /* Fixed dialing number          */
    PHB_BDN,             /* Barred dialing number         */
    PHB_LDN,             /* Last dialing number           */
    PHB_LRN,             /* Last received number          */
    PHB_SDN,             /* Service dialing number        */
    PHB_LMN,
    PHB_ADN_FDN,
    PHB_UPN,
#ifdef INT_PHONEBOOK	/* SPR#1112 - Internal phonebook */
    PHB_IPB,
#endif
    PHB_NONE,
    MAX_PHB_TYPE
} T_MFW_PHB_TYPE;

typedef enum            /* type of number */
{
  MFW_TON_UNKNOWN,
  MFW_TON_INTERNATIONAL,
  MFW_TON_NATIONAL,
  MFW_TON_NET_SPECIFIC,
  MFW_TON_DED_ACCESS,
  MFW_TON_ALPHA_NUMERIC,
  MFW_TON_ABBREVIATED,
  MFW_TON_EXTENDED
} T_MFW_PHB_TON;

typedef enum              /* numbering plan identifier */
{
  MFW_NPI_UNKNOWN,
  MFW_NPI_ISDN      = 1,
  MFW_NPI_DATA      = 3,
  MFW_NPI_TELEX     = 4,
  MFW_NPI_NATIONAL  = 8,
  MFW_NPI_PRIVATE   = 9,
  MFW_NPI_M_ERMES   = 10,  
  MFW_NPI_M_CTS     = 11   
} T_MFW_PHB_NPI;

typedef enum            /* service related to the phone number */
{
  SERVICE_UNKNOWN,
  SERVICE_ASYNCH,           /* asynchronous modem          */
  SERVICE_SYNCH,            /* synchronous modem           */
  SERVICE_PAD_ASYNCH,       /* PAD access (asynchronous)   */
  SERVICE_PACKET_SYNCH,     /* packet access (synchronous) */
  SERVICE_VOICE,            /* voice                       */
  SERVICE_FAX               /* fax                         */
} T_MFW_PHB_SERVICE;

typedef enum    /* Result of operation */
{
    MFW_NO_ENTRY,
    MFW_ENTRY_EXIST
} T_MFW_PHB_RESULT;


typedef enum    /* Return values */
{
    MFW_PHB_OK,     /* success           */
    MFW_PHB_FULL,   /* phonebook is full */
    MFW_PHB_PIN2_REQ,
    MFW_PHB_PUK2_REQ,
    MFW_PHB_FAIL ,
    //x0061560 Date May16,07 (sateeshg) OMAPS00133241
    MFW_PHB_EXCT
} T_MFW_PHB_RETURN;

typedef enum    /* mode of sorted phonebook */
{
    MFW_PHB_ALPHA,
    MFW_PHB_NUMBER,
    MFW_PHB_PHYSICAL,
    MFW_PHB_INDEX
} T_MFW_PHB_MODE;

/* Structures */
typedef struct  /* Character */
{
  UBYTE dcs;
  UBYTE len;                    /* length of string data        */
  UBYTE data[PHB_MAX_LEN];      /* string data                  */
  UBYTE dummy;
} T_MFW_PHB_TEXT;

typedef struct
{
  UBYTE year     [PHB_MAX_DIGITS];
  UBYTE month    [PHB_MAX_DIGITS];
  UBYTE day      [PHB_MAX_DIGITS];
  UBYTE dummy1;
  UBYTE dummy2;
} T_MFW_PHB_DATE;

typedef struct
{
  UBYTE hour     [PHB_MAX_DIGITS];
  UBYTE minute   [PHB_MAX_DIGITS];
  UBYTE second   [PHB_MAX_DIGITS];
  UBYTE dummy1;
  UBYTE dummy2;
} T_MFW_PHB_TIME;


typedef struct  /* Phonebook entry */
{
    UBYTE            book;                  /* phonebook                   */
    UBYTE			 dummy1;
    UBYTE			 dummy2;
    UBYTE			 dummy3;
#ifdef NO_ASCIIZ
    T_MFW_PHB_TEXT   name;                  /* length of name              */
#else
   UBYTE            name[/*PHB_MAX_LEN*/MAX_ALPHA_LEN];     /* alpha identifier            */
#endif
    UBYTE            number[PHB_MAX_LEN+1];  /* telephone number, +1 for '+' in intl. numbers */
    SHORT            index;             /* index in phonebook table    */
    UBYTE            ton;               /* type of number              */
    UBYTE            npi;               /* numbering plan identifier   */
    T_MFW_PHB_DATE   date;              /* date of entry               */
    T_MFW_PHB_TIME   time;              /* time of entry               */
    UBYTE 				line;
} T_MFW_PHB_ENTRY;


typedef struct  /* phonebook read operation */
{
    UBYTE             result;                   /* result of operation         */
    UBYTE             book;                     /* phonebook                   */
    SHORT             num_entries;              /* number of phonebook entries */
    T_MFW_PHB_ENTRY   *entry;                   /* phonebook entries           */
} T_MFW_PHB_LIST;


typedef struct  /* phonebook status indication */
{
    UBYTE book;         /* phonebook                     */
    SHORT max_entries;  /* max entries in the phonebook  */
    UBYTE tag_len;      /*length of alpha identifier     */
    SHORT used_entries; /* used entries in the phonebook */
    SHORT avail_entries;
} T_MFW_PHB_STATUS;

typedef struct  /* MSISDN number */
{
#ifdef NO_ASCIIZ
    T_MFW_PHB_TEXT   name;                  /* length of name              */
#else
    UBYTE            name[/*PHB_MAX_LEN*/MAX_ALPHA_LEN/*MC, SPR1257*/];     /* alpha identifier            */
#endif
    UBYTE             number[PHB_MAX_LEN+1]; /* telephone number, +1 for '+' in intl. numbers */
    T_MFW_PHB_SERVICE service;               /* service related to phone number */
    T_MFW_PHB_TON     ton;                   /* type of number                  */
    T_MFW_PHB_NPI     npi;                   /* numbering plan identifier       */
} T_MFW_UPN;

typedef struct
{
    UBYTE count;                 
    T_MFW_UPN upn[MAX_UPN_NUM];  
} T_MFW_UPN_LIST;


/* PHB Parameter */

typedef union
{
  T_MFW_PHB_STATUS  phb_status;
  T_MFW_UPN_LIST    upn_list;
} T_MFW_PHB_PARA;

EXTERN void  phb_init          (void);
EXTERN void  phb_exit          (void);
EXTERN T_MFW_HND phb_create    (T_MFW_HND   hWin, 
                                T_MFW_EVENT event, 
                                T_MFW_CB cbfunc);
EXTERN T_MFW_RES phb_delete    (T_MFW_HND h);
EXTERN T_MFW phb_read_entries  (UBYTE book, 
                                UBYTE index, 
                                UBYTE mode, 
                                UBYTE num_entries, 
                                T_MFW_PHB_LIST *entries);
#ifdef NO_ASCIIZ
EXTERN T_MFW phb_find_entries  (UBYTE book, 
                                SHORT *index, 
                                UBYTE mode, 
                                SHORT num_entries, 
                                T_MFW_PHB_TEXT *search_pattern, 
                                T_MFW_PHB_LIST *entries);
#else
EXTERN T_MFW phb_find_entries  (UBYTE book, 
                                SHORT *index, 
                                UBYTE mode, 
                                SHORT num_entries, 
                                CHAR *search_pattern, 
                                T_MFW_PHB_LIST *entries);
#endif
EXTERN T_MFW phb_store_entry   (UBYTE book, 
                                T_MFW_PHB_ENTRY *entry, 
                                T_MFW_PHB_STATUS *status); 
EXTERN T_MFW phb_delete_entry  (UBYTE book, UBYTE index, 
                                T_MFW_PHB_STATUS *status);
EXTERN T_MFW phb_delete_book   (UBYTE book);
EXTERN T_MFW phb_set_mode      (UBYTE book, UBYTE *passed);
EXTERN T_MFW phb_check_number  (UBYTE *number);
EXTERN T_MFW phb_get_status    (T_MFW_PHB_STATUS *status);
EXTERN T_MFW phb_set_auto_off  (UBYTE mode);
EXTERN T_MFW phb_get_auto_off  (UBYTE *mode);
EXTERN T_MFW_PHB_RETURN phb_switch_entry (T_MFW_PHB_ENTRY *entry, UBYTE *passwd);
EXTERN void  phb_get_upn       (void);
EXTERN T_MFW phb_save_upn      (T_MFW_UPN *upn);
EXTERN void phb_Alpha2Gsm(T_ACI_PB_TEXT *text, UBYTE *alpha);
EXTERN void phb_Gsm2Alpha(UBYTE *alpha, T_ACI_PB_TEXT *text);
EXTERN int phb_get_mode        (void);
EXTERN int numbersMatch(char * num1, char* num2);

/* Added to remove warning Aug - 11 */
#ifdef NEPTUNE_BOARD
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
#ifdef PCM_2_FFS
EXTERN T_FFS_SIZE ffs_ReadRecord(const char *name, void * addr, int size, int index, int recsize);
#endif /* PCM_2_FFS */ 
#endif  /* NEPTUNE_BOARD */
/* End - remove warning Aug - 11 */ 

#endif

