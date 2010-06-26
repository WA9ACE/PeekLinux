/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  Definitions for llc_itxs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXS_H
#define LLC_ITXS_H


EXTERN void sig_llme_itx_assign_req (void);

EXTERN void sig_llme_itx_unassign_req (void);

EXTERN void sig_llme_itx_reset_req (void);

EXTERN void sig_llme_itx_abmest_req (void);

EXTERN void sig_llme_itx_abmrel_req (void);

EXTERN void sig_llme_itx_suspend_req (void);

EXTERN void sig_llme_itx_resume_req (void);

EXTERN void sig_tx_itx_ready_ind (void);

EXTERN void sig_irx_itx_peer_busy_ind (void);

EXTERN void sig_irx_itx_peer_ready_ind (void);

EXTERN void sig_irx_itx_trigger_ind (void);

EXTERN void sig_irx_itx_ack_ind (BOOL is_ack, T_FRAME_NUM num);

EXTERN void sig_irx_itx_cnf_l3data_req (void);

EXTERN void sig_irx_itx_send_rr_req (T_ABIT_REQ_TYPE);

EXTERN void sig_irx_itx_send_rnr_req (T_ABIT_REQ_TYPE);

EXTERN void sig_irx_itx_send_ack_req (T_ABIT_REQ_TYPE);

EXTERN void sig_irx_itx_send_sack_req (T_ABIT_REQ_TYPE, T_SACK_BITMAP* );

#endif /* !LLC_ITXS_H */
