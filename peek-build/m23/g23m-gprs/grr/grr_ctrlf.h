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

#ifndef GRR_CTRLF_H
#define GRR_CTRLF_H

typedef enum
{
  CTRL_CR_TYPE_CS,
  CTRL_CR_TYPE_NEW,
  CTRL_CR_TYPE_NEW_NOT_SYNCED,
  CTRL_CR_TYPE_NEW_PBCCH_INFO,
  CTRL_CR_TYPE_CONT
} T_CTRL_CR_TYPE;

typedef enum
{
  CTRL_DC_OTHER,
  CTRL_DC_SUSPEND_NORMAL,
  CTRL_DC_SUSPEND_IMM_REL,
  CTRL_DC_CR_NW_RR,
  CTRL_DC_CR_NW_GRR,
  CTRL_DC_CR_MS_RR_NORMAL,
  CTRL_DC_CR_MS_RR_IMM_REL,
  CTRL_DC_CR_MS_GRR_NORMAL,
  CTRL_DC_CR_MS_GRR_ABNORMAL,
  CTRL_DC_PSI_MANAGEMENT
} T_CTRL_DISABLE_CAUSE;

typedef enum
{
  CCO_TYPE_RR,
  CCO_TYPE_GRR
} T_CTRL_CCO_TYPE;

#define SET_STATE_FAILED_CR( state )           \
  if( state EQ CTRL_CR )                       \
  {                                            \
    SET_STATE( CTRL, CTRL_FAILED_CR );         \
  }                                            \
  else                                         \
  {                                            \
    SET_STATE( CTRL, CTRL_FAILED_CR_IN_SUSP ); \
  }


typedef BOOL ( * T_TC_ACCESS_ENABLE_FUNC )( BOOL cell_has_changed );


EXTERN void ctrl_stop_T3174 ( void );

EXTERN void ctrl_start_T3174 ( void );

EXTERN void ctrl_stop_all_activities ( T_CTRL_DISABLE_CAUSE dc, void *data );

EXTERN void ctrl_init ( void );

EXTERN void ctrl_init_params ( void );

EXTERN void ctrl_start_monitoring_bcch ( UBYTE si_to_read );

EXTERN void ctrl_stop_monitoring_ccch ( void );

EXTERN void ctrl_start_monitoring_ccch ( UBYTE pag_mode );

EXTERN void ctrl_send_rr_est_req ( UBYTE susp_req );

EXTERN void ctrl_send_rr_est_rsp ( UBYTE susp_req, UBYTE rsp );

EXTERN void ctrl_suspend_services ( UBYTE susp_cause );

EXTERN void ctrl_copy_non_gprs_opt ( T_non_gprs *p_non_gprs_opt);

EXTERN BOOL ctrl_is_gprs_suspension_request_needed ( void );

EXTERN UBYTE ctrl_get_number_of_digits ( UBYTE *digits );

EXTERN void ctrl_send_cell_reselection_req ( UBYTE cr_type );

EXTERN void ctrl_start_cell_reselection ( T_CTRL_CR_TYPE ctrl_cr_type, 
                                          BOOL           initial );

EXTERN BOOL ctrl_cell_has_changed ( void );

EXTERN void ctrl_send_cell_ind ( void );

EXTERN void ctrl_enable_serving_cell ( UBYTE final );

EXTERN void ctrl_install_new_scell ( void );

EXTERN void ctrl_reinstall_old_scell_req ( BOOL is_pcco, UBYTE cause );

EXTERN void ctrl_reinstall_old_scell_cnf ( BOOL is_success );

EXTERN void ctrl_copy_pbcch_des_from_ncell ( T_SC_DATABASE *db );

EXTERN void ctrl_sync_to_given_cell ( void );

EXTERN void ctrl_send_access_disable_if_needed ( T_TC_DISABLE_CAUSE         disable_cause,
                                                 T_TC_DISABLE_CALLBACK_FUNC callback_func );

EXTERN BOOL ctrl_copy_scell_params ( T_serving_cell_info *sc_inf );

EXTERN void ctrl_copy_cell_env( T_cell_env          *cell_env,
                                T_serving_cell_info *scell_info );

EXTERN void ctrl_cell_change_order ( T_CTRL_CCO_TYPE type, void *data );

EXTERN void ctrl_send_reconnect_dcch_req ( UBYTE cause );

EXTERN void ctrl_pcco_failure ( void );

EXTERN void ctrl_build_pcco_failure_msg ( USHORT arfcn, UBYTE bsic );

EXTERN void ctrl_si13_received ( T_RRGRR_GPRS_SI13_IND * rrgrr_si13_ind );

EXTERN void ctrl_cc_rejected ( void );

EXTERN void ctrl_failure_processing ( T_FAILURE_SIGNAL fail_sig );

EXTERN void ctrl_send_control_block ( T_BLK_OWNER   blk_owner, 
                                      void        * blk_struct );

EXTERN void ctrl_cancel_control_block ( T_BLK_OWNER blk_owner );

EXTERN void ctrl_send_control_block_result ( T_BLK_OWNER blk_owner,
                                             BOOL        is_success );

EXTERN void ctrl_resp_resume ( void );

EXTERN void ctrl_handle_new_candidate ( BOOL initial );

EXTERN void ctrl_handle_no_more_candidate ( T_CTRL_CR_TYPE ctrl_cr_type, BOOL initial );

EXTERN void ctrl_start_rr_task ( UBYTE state );

EXTERN void ctrl_stop_rr_task_req ( T_GLBL_PCKT_MODE final_pckt_mode, UBYTE param );

EXTERN void ctrl_stop_rr_task_cnf ( BOOL change_pckt_mode );

EXTERN void ctrl_send_rrgrr_stop_task_req ( T_CTRL_TASK ctrl_task, UBYTE param ); 

EXTERN void ctrl_set_old_scell ( BOOL service );

EXTERN void ctrl_send_gmmrr_cr_ind ( void );

EXTERN void ctrl_tc_access_enable ( T_TC_ACCESS_ENABLE_FUNC access_enable_func );

EXTERN T_GPRS_SERVICE ctrl_get_gprs_service ( void                       ); 

EXTERN void ctrl_set_gprs_service ( UBYTE cause, UBYTE limited );

EXTERN void ctrl_send_gmmrr_suspend_cnf ( void );

EXTERN void ctrl_handle_gmmrr_cs_page_ind( UBYTE state );

EXTERN void ctrl_handle_parked_rrgrr_cr_ind( UBYTE state );

EXTERN void ctrl_read_pcm ( void );

EXTERN void ctrl_enter_standby_state( void );

EXTERN void ctrl_enter_ready_state( void );

#ifdef REL99
EXTERN BOOL ctrl_fill_cbch_hopping_params(T_cbch *cbch_req,const T_freq_par *freq_par);
#endif

#endif /* !GRR_CTRLF_H */

