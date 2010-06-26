/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernp.h
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
|  Purpose :  Definitions for gmm_kernp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_KERNP_H
#define GMM_KERNP_H



EXTERN void kern_t3302 (void);
EXTERN void kern_t3310 (void);
EXTERN void kern_t3311 (void);
EXTERN void kern_t3312 (void);
EXTERN void kern_t3321 (void);
EXTERN void kern_tpower_off (void);

EXTERN void kern_gmmreg_plmn_res ( T_GMMREG_PLMN_RES *gmmreg_plmn_res );

EXTERN void kern_gmmreg_plmn_mode_req ( T_GMMREG_PLMN_MODE_REQ *gmmreg_plmn_mode_req );

EXTERN void kern_gmmreg_config_req ( T_GMMREG_CONFIG_REQ *gmmreg_config_req );

EXTERN void kern_gmmreg_net_req ( T_GMMREG_NET_REQ *gmmreg_net_req );

EXTERN void kern_gmmreg_detach_req ( T_GMMREG_DETACH_REQ *gmmreg_detach_req );

#ifdef GMM_TCS4
/*No Establish request will come from SM in new TI DK implementation*/
#else
EXTERN void kern_gmmsm_establish_req ( T_GMMSM_ESTABLISH_REQ 
                                       *gmmsm_establish_req );
#endif

#ifdef GMM_TCS4 
EXTERN void kern_gmmsm_sequence_res ( T_MMPM_SEQUENCE_RES *gmmsm_sequence_res );
#else
EXTERN void kern_gmmsm_sequence_res ( T_GMMSM_SEQUENCE_RES *gmmsm_sequence_res );
#endif 

EXTERN void kern_gmmsms_reg_state_req ( T_GMMSMS_REG_STATE_REQ *gmmsms_reg_state_req );


#ifdef REL99
EXTERN void kern_gmmsm_pdp_status_req ( T_MMPM_PDP_CONTEXT_STATUS_REQ *gmmsm_pdp_status_req );
#endif 


EXTERN void kern_gmmrr_page_ind ( T_GMMRR_PAGE_IND *gmmrr_page_ind );

EXTERN void kern_gmmrr_cs_page_ind ( T_GMMRR_CS_PAGE_IND *gmmrr_cs_page_ind );

EXTERN void kern_gmmrr_suspend_cnf ( T_GMMRR_SUSPEND_CNF *gmmrr_suspend_cnf );

EXTERN void kern_cgrlc_status_ind ( T_CGRLC_STATUS_IND *cgrlc_status_ind ); /* TCS 2.1 */

EXTERN void kern_cgrlc_test_mode_cnf ( T_CGRLC_TEST_MODE_CNF *cgrlc_test_mode_cnf ); /* TCS 2.1 */


EXTERN void kern_llgmm_status_ind ( T_LLGMM_STATUS_IND *llgmm_status_ind );

EXTERN void kern_llgmm_tlli_ind ( T_LLGMM_TLLI_IND *llgmm_tlli_ind );

EXTERN void kern_sim_gmm_insert_ind ( T_SIM_GMM_INSERT_IND *sim_mm_insert_ind );

EXTERN void kern_sim_remove_ind ( T_SIM_REMOVE_IND *sim_remove_ind );

EXTERN void kern_mmgmm_auth_rej_ind ( T_MMGMM_AUTH_REJ_IND *mmgmm_auth_rej_ind );

EXTERN void kern_mmgmm_nreg_cnf ( T_MMGMM_NREG_CNF *mmgmm_nreg_cnf );

EXTERN void kern_mmgmm_plmn_ind ( T_MMGMM_PLMN_IND *mmgmm_plmn_ind );

EXTERN void kern_mmgmm_t3212_val_ind ( T_MMGMM_T3212_VAL_IND 
                                          *mmgmm_t3212_val_ind );
EXTERN void kern_mmgmm_cm_release_ind ( T_MMGMM_CM_RELEASE_IND 
                                          *mmgmm_cm_release_ind );
EXTERN void kern_mmgmm_cm_establish_ind ( T_MMGMM_CM_ESTABLISH_IND 
                                          *mmgmm_cm_establish_ind );
EXTERN void kern_mmgmm_cm_emergency_ind ( T_MMGMM_CM_EMERGENCY_IND 
                                          *mmgmm_cm_emergency_ind );
EXTERN void kern_mmgmm_lup_accept_ind ( T_MMGMM_LUP_ACCEPT_IND 
                                          *mmgmm_lup_accept_ind );
EXTERN void kern_mmgmm_lup_needed_ind ( T_MMGMM_LUP_NEEDED_IND *mmgmm_lup_needed_ind );
EXTERN void kern_mmgmm_info_ind ( T_MMGMM_INFO_IND *mmgmm_info_ind );

EXTERN void kern_gmmreg_attach_req ( T_GMMREG_ATTACH_REQ *gmmreq_attach_req );
EXTERN void kern_sim_authentication_cnf ( T_SIM_AUTHENTICATION_CNF *sim_authentication_cnf );
 
EXTERN void kern_mmgmm_ciphering_ind ( T_MMGMM_CIPHERING_IND *mmgmm_ciphering_ind );
EXTERN void kern_mmgmm_tmsi_ind ( T_MMGMM_TMSI_IND *mmgmm_tmsi_ind );
EXTERN void kern_gmmrr_cr_ind ( T_GMMRR_CR_IND *gmmrr_cr_ind );
EXTERN void kern_tlocal_detach (void);
EXTERN void kern_mmgmm_ahplmn_ind ( T_MMGMM_AHPLMN_IND *mmgmm_ahplmn_ind );

#endif /* !GMM_KERNP_H */

