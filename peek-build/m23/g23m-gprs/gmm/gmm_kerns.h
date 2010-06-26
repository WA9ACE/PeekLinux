/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kerns.h
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
|  Purpose :  Definitions for gmm_kerns.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_KERNS_H
#define GMM_KERNS_H


EXTERN void sig_auth_kern_abort_ind ( void );



EXTERN void sig_auth_kern_dereg_ind ( void );



EXTERN void sig_rx_kern_att_acc_ind ( void );



EXTERN void sig_rx_kern_att_rej_ind ( void );



EXTERN void sig_rx_kern_det_req_ind ( void );



EXTERN void sig_rx_kern_det_acc_ind ( void );



EXTERN void sig_rx_kern_rau_acc_ind ( void );



EXTERN void sig_rx_kern_rau_rej_ind ( void );


EXTERN void sig_rx_kern_realloc_cmd_ind ( void );

EXTERN void sig_rx_kern_auth_req_ind ( void );
EXTERN void sig_rx_kern_auth_rej_ind ( void );
EXTERN void sig_rx_kern_id_req_ind ( void );
EXTERN void sig_rx_kern_status_ind ( void );
EXTERN void sig_rx_kern_info_ind ( void );
EXTERN void sig_rx_kern_tst_cmd_ind ( void );
#ifdef GMM_TCS4
  #ifdef FF_EGPRS
    EXTERN void sig_rx_kern_egprs_loopb_cmd_ind ( void );
  #endif /*FF_EGPRS*/
#endif /*GMM_TCS4*/

EXTERN void sig_sync_kern_cell_ind ( T_SIG_CELL_INFO * sig_cell_info );
EXTERN void sig_sync_kern_mmgmm_nreg_ind ( T_MMGMM_NREG_IND *mmgmm_nreg_ind );
EXTERN void sig_sync_kern_mmgmm_reg_rej ( T_MMGMM_REG_REJ *mmgmm_reg_rej );
EXTERN void sig_sync_kern_mmgmm_reg_cnf ( T_MMGMM_REG_CNF *mmgmm_reg_cnf );
EXTERN void sig_rdy_kern_cu_ind ( void );




#endif /* !GMM_KERNS_H */

