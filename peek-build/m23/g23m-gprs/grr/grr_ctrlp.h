/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  Definitions for grr_ctrlp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CTRLP_H
#define GRR_CTRLP_H

/* TIMER */
EXTERN void ctrl_t3134 ( void );

EXTERN void ctrl_t3174 ( void );

EXTERN void ctrl_t3176 ( void );

#ifdef REL99
EXTERN void ctrl_t_poll_timer(void);
#endif

/* GMMRR */
EXTERN void ctrl_gmmrr_enable_req ( T_GMMRR_ENABLE_REQ *gmmrr_enable_req );

EXTERN void ctrl_gmmrr_assign_req ( T_GMMRR_ASSIGN_REQ *gmmrr_assign_req );

EXTERN void ctrl_gmmrr_disable_req ( T_GMMRR_DISABLE_REQ *gmmrr_disable_req );

EXTERN void ctrl_gmmrr_suspend_req ( T_GMMRR_SUSPEND_REQ *gmmrr_suspend_req );

EXTERN void ctrl_gmmrr_resume_req ( T_GMMRR_RESUME_REQ *gmmrr_resume_req );

EXTERN void ctrl_gmmrr_cs_page_res ( T_GMMRR_CS_PAGE_RES *gmmrr_cs_page_res );

EXTERN void ctrl_gmmrr_standby_req ( T_GMMRR_STANDBY_REQ *gmmrr_standby_req );

EXTERN void ctrl_gmmrr_ready_req ( T_GMMRR_READY_REQ *gmmrr_ready_req );


EXTERN void ctrl_gmmrr_attach_started_req ( T_GMMRR_ATTACH_STARTED_REQ *gmmrr_attach_started_req );

EXTERN void ctrl_gmmrr_attach_finished_req ( T_GMMRR_ATTACH_FINISHED_REQ *gmmrr_attach_finished_req );

EXTERN void ctrl_gmmrr_cell_res ( T_GMMRR_CELL_RES *gmmrr_cell_res );

/* RRGRR */
EXTERN void ctrl_rrgrr_gprs_si13_ind ( T_RRGRR_GPRS_SI13_IND *rrgrr_gprs_si13_ind );

EXTERN void ctrl_rrgrr_packet_paging_ind ( T_RRGRR_PACKET_PAGING_IND *rrgrr_packet_paging_ind );

EXTERN void ctrl_rrgrr_ia_ind ( T_RRGRR_IA_IND *rrgrr_ia_ind );

EXTERN void ctrl_rrgrr_iaext_ind ( T_RRGRR_IAEXT_IND *rrgrr_iaext_ind );

EXTERN void ctrl_rrgrr_assignment_rej_ind ( T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind );

EXTERN void ctrl_rrgrr_data_ind ( T_RRGRR_DATA_IND *rrgrr_data_ind );

EXTERN void ctrl_rrgrr_ia_downlink_ind ( T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_ind );

EXTERN void ctrl_rrgrr_stop_task_cnf ( T_RRGRR_STOP_TASK_CNF *rrgrr_stop_task_cnf );

EXTERN void ctrl_rrgrr_rr_est_ind ( T_RRGRR_RR_EST_IND *rrgrr_rr_est_ind);

EXTERN void ctrl_rrgrr_suspend_dcch_cnf ( T_RRGRR_SUSPEND_DCCH_CNF *rrgrr_suspend_dcch_cnf);

EXTERN void ctrl_rrgrr_reconnect_dcch_cnf ( T_RRGRR_RECONNECT_DCCH_CNF *rrgrr_reconnect_dcch_cnf);

EXTERN void ctrl_rrgrr_stop_dcch_ind(T_RRGRR_STOP_DCCH_IND* rrgrr_stop_dcch_ind);

EXTERN void ctrl_rrgrr_cr_ind( T_RRGRR_CR_IND *cr_ind);

EXTERN void ctrl_rrgrr_check_bsic_ind ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind );

EXTERN void ctrl_rrgrr_sync_ind ( T_RRGRR_SYNC_IND *rrgrr_sync_ind );

EXTERN void ctrl_rrgrr_meas_rep_cnf ( T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf );

EXTERN void ctrl_rrgrr_ext_meas_cnf ( T_RRGRR_EXT_MEAS_CNF *rrgrr_ext_meas_cnf );

EXTERN void ctrl_rrgrr_ms_id_ind(T_RRGRR_MS_ID_IND *ms_id);

/* CGRLC */
EXTERN void ctrl_cgrlc_standby_state_ind ( T_CGRLC_STANDBY_STATE_IND *cgrlc_standby_state_ind );

EXTERN void ctrl_cgrlc_ready_state_ind ( T_CGRLC_READY_STATE_IND *cgrlc_ready_state_ind );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void ctrl_rrgrr_si2quater_ind(T_RRGRR_SI2QUATER_IND *rrgrr_enh_para_ind);
#endif

#endif /* !GRR_CTRLP_H */

