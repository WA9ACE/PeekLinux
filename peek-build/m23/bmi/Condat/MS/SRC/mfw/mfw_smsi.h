/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_smsi.h      $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 21.12.98                      $Modtime:: 21.03.00 11:51  $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SMSI

   PURPOSE : This modul contains the definition for SMS management.


   $History:: mfw_smsi.h                                              $

      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

 *	May 27, 2005    MMI-FIX-29869 x0018858
 * 	Description: There is a limitation on the number of messages that can be downloaded from 
 *  server even though there is space on the sim.
 * 	Solution: The limitation has been removed and the number of messages that can be dowloaded
 * 	has been modified to the masimum number that can be stored.
 * 
 * *****************  Version 15  *****************
 * User: Vo           Date: 22.03.00   Time: 17:44
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * check voice mail status
 * 
 * *****************  Version 14  *****************
 * User: Vo           Date: 15.03.00   Time: 14:37
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Bug fix: receiving MT indication conflict with perform SMS command
 * Bug fix: read SMS parameter
 * 
 * *****************  Version 13  *****************
 * User: Vo           Date: 28.01.00   Time: 15:36
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 12  *****************
 * User: Vo           Date: 20.01.00   Time: 12:28
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 11  *****************
 * User: Vo           Date: 14.01.00   Time: 20:17
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 10  *****************
 * User: Vo           Date: 17.11.99   Time: 11:01
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * 1. change constants
 * 2. new prototype define
 * 
 * *****************  Version 9  *****************
 * User: Vo           Date: 21.10.99   Time: 13:36
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * new constants define
 * new prototyp
 * 
 * *****************  Version 8  *****************
 * User: Vo           Date: 30.08.99   Time: 15:18
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * new prototypes
 * 
 * *****************  Version 7  *****************
 * User: Vo           Date: 26.07.99   Time: 14:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * defines moved to mfw_sms.h
 * 
 * *****************  Version 6  *****************
 * User: Vo           Date: 16.07.99   Time: 13:50
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 22.06.99   Time: 8:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 4  *****************
 * User: Vo           Date: 20.05.99   Time: 17:48
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 3  *****************
 * User: Vo           Date: 31.03.99   Time: 14:09
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Vo           Date: 12.02.99   Time: 18:59
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Vo           Date: 30.12.98   Time: 11:49
 * Created in $/GSM/DEV/MS/SRC/MFW

*/

#ifndef DEF_MFW_SMSI_HEADER
#define DEF_MFW_SMSI_HEADER

#define MFW_SMS_FREE        0
#define MFW_SMS_NEW         1
#define MFW_SMS_UPDATE      2
#define MFW_SMS_WAIT        3
//May 27, 2005    MMI-FIX-29869 x0018858 -Changed the #define to a global variable
#ifdef FF_MMI_SMS_DYNAMIC
extern int g_max_messages;
#else
#define MAX_MESSAGES        30
#endif
/* message type */
#define MFW_MT_MESSAGE      10
#define MFW_WR_NEW_MESSAGE  20
#define MFW_WR_CHG_MESSAGE  30

/*
 *  SMS Control Block
 */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_SMS_PARA  para;
} T_MFW_SMS;


/* SMS parameters */
typedef struct	
{
  UBYTE  tag_len;             /* length of Alpha identifier */
  UBYTE  tag[MFW_TAG_LEN];    /* Alpha identifier           */
  UBYTE  para_id;             /* Parameter identifier       */
  UBYTE  dest_addr[12];       /* TP-destination address     */
  UBYTE  sc_addr[12];	      /* service centre address     */
  UBYTE	 prot_id;	          /* protocol identifier        */
  UBYTE	 dcs;                 /* data coding scheme         */
  UBYTE	 srr;	              /* status report request      */
  UBYTE	 rp;	              /* reply path                 */
  UBYTE	 rd;	              /* reject duplicates          */
  UBYTE	 vp_mode;	          /* validity period mode       */
  UBYTE	 vp_rel;	          /* validity period relative   */
  UBYTE *vp_abs; 	          /* validity period absolute   */
} T_MFW_SMS_PARAMETER;

/* Cell broadcast massage parameters */
typedef struct	
{
  UBYTE  cbm_id[10];      /* cell broadcast message identifier */
} T_MFW_SMS_CB_PARA;

typedef struct
{
  UBYTE mem;
  UBYTE used;
  UBYTE total;
} T_MFW_MEM_INFO;


EXTERN T_MFW_HND sms_install              (T_MFW_HDR      *w, 
                                           T_MFW_HDR      *h, 
                                           T_MFW_SMS      *sms);
EXTERN T_MFW_RES sms_remove               (T_MFW_HDR      *h);
EXTERN BOOL      sms_sign_exec            (T_MFW_HDR      *hdr,
                                           T_MFW_EVENT     event, 
                                           T_MFW_SMS_PARA *para);

void sms_signal                  (T_MFW_EVENT event, void * para);
void sms_ok_delete               (void);
void sms_ok_change               (void);
void sms_busy_ind                (void); // bugfix for SIM refresh
T_MFW_SMS_STAT sms_decodeStatus  (T_ACI_SMS_STAT stat);
T_ACI_SMS_STAT sms_codeStatus    (T_MFW_SMS_STAT stat);
void sms_get_cbm_id              (USHORT *cbmid, UBYTE len);
void sms_save_cbm_id             (USHORT *cbmid, UBYTE len);
void sms_read_msg_info           (void);
void sms_update_ready            (void);
void sms_copy_msg_list           (int num, T_MFW_SMS_MSG *msg_list);
// PATCH JPS 29.09: add function equivalent of sms_message_list bu more economic
void sms_copy_idx_list           (int num, T_MFW_SMS_IDX *msg_list);
//MC 
/*a0393213 OMAPS00109953 - memory added as parameter*/
void sms_copy_idx(UBYTE index, T_MFW_SMS_IDX *msg_list, T_MFW_SMS_MEM sms_memory);

// PATCH JPS 29.09 end
void sms_check_mt_update         (void);
void sms_ok_csas                 (void);
void sms_phbReady_ind            (void);
T_MFW sms_check_voice_mail       (UBYTE pid, UBYTE dcs, char *addr,
                                  T_ACI_TOA *toa, T_ACI_UDH_DATA *udh);
void sms_sat_file_update         (USHORT dataId);
void sms_sat_update_cnf          (void);
void sms_ok_CMGL (void);

#endif
