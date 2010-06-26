/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_simi.h      $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 14.10.98                     $Modtime:: 20.03.00 11:48   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

  PURPOSE : Internal definitions for SIM management of MMI framework

  $History:: mfw_simi.h                                       $

	May 15, 2006 DR: OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution: Added prototype of funtion sim_simlock_cpin_code()
*/

#ifndef DEF_MFW_SIMI_HEADER
#define DEF_MFW_SIMI_HEADER

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"

#define NO_ALLOCATED              0
#define ALLOCATED_AND_DEACTIVATED 2
#define ALLOCATED_AND_ACTIVATED   3

#define SIM_UBLK_ERROR            0
#define SIM_UBLK_OK               1

#define SIM_PIN2_FAILURE          17
#define SIM_PUK2_FAILURE          18

#define MFW_LEN_PLMN_NTRY         3

/* SIM configuration information */
typedef struct
{
    UBYTE deper_key [16];
    UBYTE phase;
    UBYTE oper_mode;       /* SIM card functionality   */
    UBYTE pref_lang[5];
    UBYTE access_acm;
    UBYTE access_acmmax;
    UBYTE access_puct;
    UBYTE sim_gidl1[5];
    UBYTE sim_gidl2[5];
} T_MFW_SIM_CONFIG;

/*
 *  SIM Control Block
 */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_SIM_PARA  para;
} T_MFW_SIM;



EXTERN T_MFW_HND sim_install              (T_MFW_HDR              *w,
                                           T_MFW_HDR              *h,
                                           T_MFW_SIM              *sim);
EXTERN T_MFW_RES sim_remove               (T_MFW_HDR              *h);
EXTERN int       sim_sign_exec            (T_MFW_HDR * hdr,
                                           T_MFW_EVENT event,
                                           T_MFW_SIM_PARA *para);



EXTERN  void            sim_spn_req           (void);
EXTERN  void            sim_ok_cfun           (void);
/* OMAPS00075177 - 17,Apr-2006 */
/* Signature of the function has been changed from U8 to T_ACI_CME_ERR. 
   because, when calling this function, it is converting from T_ACI_CME_ERR to U8. 
   It is loosing the negative values( the enum T_ACI_CME_ERR has negative values )
*/
EXTERN  void            sim_error_cfun        (T_ACI_CME_ERR reason);
EXTERN  void            sim_ok_cpin           (void);
EXTERN  void            sim_error_cpin        (T_ACI_CME_ERR reason);
EXTERN  void            sim_ok_cpinc          (void);
EXTERN  void            sim_error_cpinc       (void);
EXTERN  void            sim_ok_cpind          (void);
EXTERN  void            sim_error_cpind       (void);
EXTERN  void            sim_ok_cpine          (void);
EXTERN  void            sim_error_cpine       (void);
EXTERN  UBYTE           sim_check_service     (UBYTE nr, UBYTE * serv_table);
EXTERN  T_MFW           sim_status_check      (void);
EXTERN  void            decode_imsi           (UBYTE simlck[],
                                               UBYTE imsi_id[LONG_NAME]);
EXTERN  void            sim_mmi_parameter     (T_SIM_ACTIVATE_CNF *sim_activate_cnf);
EXTERN  void            sim_ss_unblock        (UBYTE result, T_ACI_CME_ERR reason);
EXTERN  void            sim_simlock_timeout   (void);
EXTERN  T_MFW           sim_simlock_status    (UBYTE status);
EXTERN  UBYTE           sim_cvtPINstatus      (T_ACI_PVRF_STAT ps, UBYTE type);
EXTERN  void            sim_gid1_cnf          (USHORT error, UBYTE *data);
EXTERN  void            sim_gid2_cnf          (USHORT error, UBYTE *data);
EXTERN  void            sim_pin_ident         (UBYTE id);
EXTERN  T_MFW           sim_check_imsi_digit  (UBYTE pcm_imsi, UBYTE sim_imsi);
EXTERN  void            sim_mmi_update_parameter(T_SIM_ACTIVATE_IND *sim_act_ind);
EXTERN  void            sim_sat_file_update   (USHORT dataId);
EXTERN  void            sim_read_sst_cnf      (USHORT error, UBYTE *data);
EXTERN void 			sim_read_lp_cnf (USHORT error, T_ACI_LAN_SUP  *CLang);

//x0pleela 21 Apr, 2006  DR: OMAPS00067919
#ifdef SIM_PERS
EXTERN  void sim_simlock_cpin_code (T_ACI_CPIN_RSLT code);
#endif
#endif
