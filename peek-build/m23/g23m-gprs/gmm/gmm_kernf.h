/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernf.h
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
|  Purpose :  Definitions for gmm_kernf.c 
+----------------------------------------------------------------------------- 
*/ 



#ifndef GMM_KERNF_H
#define GMM_KERNF_H



EXTERN void kern_sim_gmm_update (void );
EXTERN void kern_gmmreg_info_ind ( void );
EXTERN void kern_gmmreg_detach ( UBYTE detach_type, 
                                 USHORT service,
                                 UBYTE search_running,
                                 USHORT cause);
EXTERN void kern_local_detach_open_proc (void );
EXTERN void kern_local_detach ( USHORT error_cause, BOOL det_acc_sent, T_LOCAL_DETACH_PROC local_detach_open_proc );

EXTERN void kern_init ( void );



EXTERN void kern_attach_reset ( void );


EXTERN void kern_mm_imsi_detach ( void );
EXTERN void kern_mm_imsi_detach_ind ( USHORT error_cause, BOOL perform_detach, UBYTE detach_type );
EXTERN void kern_mm_cm_establish_res ( UBYTE cm_establish_res  );
EXTERN void kern_mm_cm_emergency_res ( UBYTE cm_emergency_res  );

EXTERN void kern_mm_lau ( void );

EXTERN void kern_mm_set_state ( UBYTE state);
EXTERN void kern_set_rau_timer ( T_rau_timer rau_timer );

EXTERN void kern_mm_start_t3212 ( void );

EXTERN void kern_tmsi_negotiated ( BOOL v_tmsi, 
                                   T_mobile_identity *tmsi,
                                   BOOL v_ptmsi, 
                                   T_gmobile_identity *ptmsi,
                                   BOOL v_ptmsi_signature,
                                   T_p_tmsi_signature *p_tmsi_signature
                                 );

EXTERN void kern_local_attach ( T_TLLI_TYPE new_tlli_type,
                                  T_TLLI_TYPE old_tlli_type );
EXTERN void kern_gmmrr_assign ( void );


EXTERN ULONG kern_get_tmsi (T_mobile_identity * mobile_identity);

EXTERN void kern_get_mobile_identity ( 
  UBYTE type_of_identity,
  T_gmobile_identity* gmobile_identity );
EXTERN void kern_ulong2mobile_identity ( ULONG ptmsi,
                                  T_gmobile_identity* gmobile_identity );

EXTERN BOOL kern_lau_needed ( void );
EXTERN BOOL kern_cell_changed ( void );
EXTERN BOOL kern_lai_changed ( void );
EXTERN BOOL kern_rai_changed ( void );
EXTERN BOOL kern_ra_crossed ( void );
EXTERN BOOL kern_plmn_changed ( void );
EXTERN void kern_set_rai ( T_routing_area_identification  * rai);
EXTERN void kern_get_imeisv (T_gmobile_identity *imei);
EXTERN void kern_read_imsi (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind);
EXTERN void kern_read_pcm ( void );
EXTERN void kern_gmmrr_suspend ( UBYTE susp_gprs, UBYTE gmmrr_susp_cause, UBYTE susp_type);
EXTERN void kern_mm_activate_mm ( void );
EXTERN void kern_gmmreg_attach_cnf ( UBYTE attach_type);
EXTERN void kern_gmmreg_attach_cnf_sr ( UBYTE attach_type, UBYTE search_running);
EXTERN void kern_mmgmmreg_attach_cnf ( UBYTE attach_type, UBYTE search_running,
                      T_MMGMM_REG_CNF * mmgmm_reg_cnf );
EXTERN void kern_disable (void);
EXTERN void kern_llgmm_unassign (void );
EXTERN void kern_llgmm_suspend (UBYTE susp_cause);

EXTERN void kern_gmmsm_establich_rej ( void );
EXTERN void kern_llgmm_assign ( void );
EXTERN void kern_llgmm_resume ( void );
EXTERN void kern_llgmm_assign_tlli ( T_TLLI_TYPE new_tlli_type,
                                  T_TLLI_TYPE old_tlli_type );
EXTERN void kern_mm_activate_rr ( void );
EXTERN void kern_mm_reg_req ( U8 reg_type, U8 bootup_act );

EXTERN void kern_gmmrr_disable ( void );
EXTERN void kern_gmmrr_enable ( void ); 

EXTERN void kern_mm_net_req ( void );
EXTERN void kern_mm_detach_started ( void );
EXTERN void kern_mm_attach_started ( void );
EXTERN void kern_mm_auth_rej ( void );
EXTERN void kern_mm_attach_rej ( USHORT error_cause );
EXTERN void kern_mm_attach_acc ( BOOL v_mobile_identity, T_mobile_identity * mobile_identity, /* TCS 2.1 */
                                 BOOL v_eqv_plmn_list,   T_eqv_plmn_list   * eqv_plmn_list ); /* TCS 2.1 */
EXTERN void kern_sim_authentication_req ( UBYTE rand[MAX_RAND], UBYTE cksn );
EXTERN void kern_sim_del_locigprs ( void ); 
EXTERN void kern_set_loc_info (T_SIM_GMM_UPDATE_REQ *sim_gmm_update_req);
EXTERN void kern_set_kc_cksn (T_SIM_GMM_UPDATE_REQ *sim_gmm_update_req);
EXTERN void kern_read_loc_info (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind);
EXTERN void kern_read_kc_cksn (T_SIM_GMM_INSERT_IND *sim_gmm_insert_ind);
EXTERN void kern_gmmrr_stop_waiting_for_transmission (void);
EXTERN void kern_gmmrr_attach_started (void);
EXTERN void kern_gmmrr_attach_finished (void);
EXTERN BOOL kern_is_cell_forbidden (void);
EXTERN void kern_remove_plmn_from_forbidden_list (T_plmn plmn);
EXTERN void kern_send_gmmreg_cell_ind (void);
EXTERN void kern_send_llgmm_trigger_req(void);
EXTERN void kern_attach_rau_init (void);
EXTERN void kern_build_ms_network_capability (T_ms_network_capability *ms_network_capability);

EXTERN USHORT kern_make_cause (BOOL cause_valid, UBYTE network_cause);
EXTERN void kern_call_undone_mm_proc_der ( void );
EXTERN void kern_call_undone_mm_proc_reg ( void );
#ifdef GMM_TCS4
EXTERN T_CAUSE_ps_cause kern_make_new_cause ( void );
#endif
EXTERN void kern_reset_cipher ( void );
#endif /* !GMM_KERNF_H */

