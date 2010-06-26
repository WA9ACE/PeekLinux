/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_mgf.h
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG  
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for sndcp_mgf.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/

#ifndef SNDCP_MGF_H
#define SNDCP_MGF_H


#define NAS_HCOMP_BOTH_DIRECT 3

EXTERN void mg_check_cnf_xid (UBYTE* ret, UBYTE sapi);

EXTERN void mg_check_ind_xid (UBYTE* ret, UBYTE sapi);

#ifndef NCONFIG
EXTERN void mg_config_delay (USHORT millis);
#endif

EXTERN void mg_delete_npdus (UBYTE nsapi);

EXTERN void mg_del_comp_pdus_ack (UBYTE sapi);

EXTERN void mg_reset_compressors (UBYTE nsapi);

EXTERN void mg_xid_cnf_ok_res(UBYTE sapi);

EXTERN void mg_dti_close (UBYTE nsapi);

EXTERN void mg_dti_open (UBYTE nsapi);

EXTERN void mg_decode_xid (T_sdu* sdu,
                           T_XID_BLOCK* xid_block,
                           UBYTE* ret,
                           UBYTE sapi);

EXTERN void mg_clean_xid (UBYTE sapi);

EXTERN void mg_init (void);

EXTERN void mg_is_ack (T_snsm_qos snsm_qos,
                       BOOL* spec,
                       BOOL* b);

EXTERN void mg_is_rel_comp_nec (UBYTE nsapi, BOOL* nec);

EXTERN void mg_rel_nsapi_nec (UBYTE nsapi);

EXTERN void mg_re_negotiate(UBYTE sapi);  

EXTERN void mg_re_negotiate_ack(UBYTE sapi,
                                U16 cause);  

EXTERN void mg_resend_xid_if_nec(UBYTE sapi);

EXTERN void mg_reset_comp_ack (UBYTE sapi);

EXTERN void mg_reset_states_n_rej (UBYTE sapi_index);

EXTERN void mg_resume_affected_nus(UBYTE sapi);

#ifndef SNDCP_UPM_INCLUDED
EXTERN void mg_send_modification_if_nec (UBYTE sapi);
#endif

EXTERN void mg_send_snsm_activate_res (UBYTE nsapi);

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_send_empty_xid_req (T_SN_ACTIVATE_REQ* snsm_activate_ind);
#else
EXTERN void mg_send_empty_xid_req (T_SNSM_ACTIVATE_IND* snsm_activate_ind);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_send_xid_req (T_SN_ACTIVATE_REQ* snsm_activate_ind);
#else
EXTERN void mg_send_xid_req (T_SNSM_ACTIVATE_IND* snsm_activate_ind);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

EXTERN void mg_send_xid_req_del (UBYTE sapi);

EXTERN void mg_set_cur_xid_block (UBYTE sapi);

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_set_new_xid_block (T_SN_ACTIVATE_REQ* snsm_activate_ind);
#else
EXTERN void mg_set_new_xid_block (T_SNSM_ACTIVATE_IND* snsm_activate_ind);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_set_req_xid_block (T_SN_ACTIVATE_REQ* snsm_activate_ind);
#else
EXTERN void mg_set_req_xid_block (T_SNSM_ACTIVATE_IND* snsm_activate_ind);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

EXTERN void mg_set_res_cur_xid_block (UBYTE sapi, USHORT* res_sdu_len);

EXTERN void mg_set_xid_params (UBYTE sapi, T_sdu* sdu, T_XID_BLOCK block);

EXTERN void mg_set_res_xid_params (T_sdu* sdu, UBYTE sapi);

EXTERN void mg_suspend_affected_nus(UBYTE sapi);

EXTERN void mg_no_col_no_re(T_LL_ESTABLISH_IND* ll_establish_ind);

EXTERN void mg_no_col_re(T_LL_ESTABLISH_IND* ll_establish_ind);

EXTERN void mg_col_no_re(T_LL_ESTABLISH_IND* ll_establish_ind);

EXTERN void mg_col_re(T_LL_ESTABLISH_IND* ll_establish_ind);

EXTERN void mg_get_sapi_dntt_rej (UBYTE sapi, UBYTE dntt, BOOL* rej);

EXTERN void mg_get_sapi_pntt_rej (UBYTE sapi, UBYTE pntt, BOOL* rej);

EXTERN void mg_get_sapi_pntt_nsapi (UBYTE sapi, UBYTE pntt, UBYTE nsapi, BOOL* used);

EXTERN void mg_get_sapi_pcomp_pntt (UBYTE sapi, UBYTE pcomp, UBYTE* pntt);

#ifdef TI_DUAL_MODE
EXTERN T_SN_GET_PENDING_PDU_CNF* mg_get_unsent_unconfirmed_npdus(U8 nsapi, T_SN_GET_PENDING_PDU_CNF* sn_get_pending_pdu_cnf);

EXTERN void mg_clean_ack_npdu_queues_leave_data(U8 nsapi);

EXTERN void mg_clean_unack_npdu_queues_including_data(U8 nsapi);
EXTERN void sm_make_test_pending_pdu_cnf(T_SN_GET_PENDING_PDU_CNF* sn_get_pending_pdu_cnf,
                                         T_SN_TEST_GET_PENDING_PDU_CNF* sn_test_get_pending_pdu_cnf);

#endif



#endif /* !SNDCP_MGF_H */

