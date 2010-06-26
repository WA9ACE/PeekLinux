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
|  Purpose :  Definitions for service CTRL.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CTRLS_H
#define GRR_CTRLS_H

/*  TC-CTRL*/
EXTERN void sig_tc_ctrl_set_pckt_mode ( T_GLBL_PCKT_MODE final_pckt_mode, UBYTE param );
EXTERN void sig_tc_ctrl_cr_started_rsp( void );
EXTERN void sig_tc_ctrl_tbf_release_cnf( void );

EXTERN void sig_tc_ctrl_prepare_abnorm_rel_with_cr( void );
EXTERN void sig_tc_ctrl_abnorm_rel_with_cr( void );

EXTERN void sig_tc_ctrl_t3172_running ( void );

EXTERN void sig_tc_ctrl_dcch_data_req(T_RRGRR_DATA_REQ * rrgrr_data_req_i );
EXTERN void sig_tc_ctrl_suspend_cnf(void);
EXTERN void sig_tc_ctrl_channel_req ( UBYTE channel_req );
EXTERN void sig_tc_ctrl_gprs_data_req(T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req_i);
EXTERN void sig_tc_ctrl_suspend_dedi_chan(void);
EXTERN void sig_tc_ctrl_resume_dedi_chan(void);
EXTERN void sig_tc_ctrl_access_reject_on_new_cell(void);
EXTERN void sig_tc_ctrl_contention_ok(void);
EXTERN void sig_tc_ctrl_contention_failure(void);
EXTERN void sig_tc_ctrl_rel_state(T_RELEASE_STATE rel_state);
EXTERN void sig_tc_ctrl_send_stop_task_req(UBYTE v_stop_ccch);

EXTERN void sig_tc_ctrl_control_block_result(T_BLK_OWNER blk_owner,BOOL is_success);

/*  PG-CTRL*/
EXTERN void sig_pg_ctrl_downlink_transfer ( UBYTE page_id );
EXTERN void sig_pg_ctrl_rr_est_req ( PG_CHANNEL type);
EXTERN void sig_pg_ctrl_stop_mon_ccch ( void );
EXTERN void sig_pg_ctrl_start_mon_ccch ( UBYTE pg_type );

/*  PSI-CTRL*/
EXTERN void sig_psi_ctrl_si13_processed ( void );
EXTERN void sig_psi_ctrl_access_enabled ( void );
EXTERN void sig_psi_ctrl_access_disabled ( T_PSI_DISABLE_CAUSE dc );
EXTERN void sig_psi_ctrl_psi1or_psi13_receiption_failure(void);
EXTERN void sig_psi_ctrl_read_si ( UBYTE si_type);
EXTERN void sig_psi_ctrl_access_changed ( void  ) ;
EXTERN void sig_psi_ctrl_access_barred(void);
EXTERN void sig_psi_ctrl_ncell_param_valid ( void );
EXTERN void sig_psi_ctrl_ncell_param_invalid ( void );
EXTERN void sig_psi_ctrl_ncell_psi_read( BOOL read_successfully);
EXTERN void sig_psi_ctrl_int_list_invalid(void);
EXTERN void sig_psi_ctrl_int_list_valid( void );
EXTERN void sig_psi_ctrl_meas_param_invalid_psi5(void);
EXTERN void sig_psi_ctrl_meas_param_valid_psi5(void);
EXTERN void sig_psi_ctrl_meas_param_invalid_si13(void);
EXTERN void sig_psi_ctrl_meas_param_valid_si13(BOOL ba_bcch_changed);

EXTERN void sig_psi_ctrl_new_pccch(void);
#ifdef REL99
EXTERN void sig_psi_ctrl_cbch_info_ind(void);
#endif

/*  MEAS-CTRL*/
EXTERN void sig_meas_ctrl_meas_report ( T_U_MEAS_REPORT *meas_report );

/*  CS-CTRL*/
EXTERN void sig_cs_ctrl_meas_report ( T_U_MEAS_REPORT *meas_report );
EXTERN void sig_cs_ctrl_cancel_meas_report ( void );
EXTERN void sig_cs_ctrl_new_candidate ( T_CS_RESELECT_CAUSE reselect_cause );
EXTERN void sig_cs_ctrl_no_more_candidate ( T_CS_RESELECT_CAUSE reselect_cause );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void sig_cs_ctrl_enh_meas_report ( T_U_ENHNC_MEAS_REPORT *enh_meas_report );
#endif

/*  GFP-CTRL*/
#ifdef REL99
EXTERN void sig_gfp_ctrl_cc_order(T_TIME time_to_poll);
#else
EXTERN void sig_gfp_ctrl_cc_order( void );
#endif

EXTERN void sig_gfp_ctrl_dsf_ind ( void );
EXTERN void sig_tc_ctrl_test_mode_ind(void);
#endif /* !GRR_CTRLS_H */

