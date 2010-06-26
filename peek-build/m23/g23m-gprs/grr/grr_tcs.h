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
|  Purpose :  Definitions for service TC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_TCS_H
#define GRR_TCS_H

/*
 *  interface for service CTRL
 */
EXTERN void sig_ctrl_tc_control_block        ( T_BLK_OWNER  blk_owner_i,
                                               void        *blk_struct_i     );
EXTERN void sig_ctrl_tc_cancel_control_block ( T_BLK_OWNER  blk_owner_i      );

EXTERN void sig_ctrl_tc_access_disable       ( T_TC_DISABLE_CAUSE         disable_cause,
                                               T_TC_DISABLE_CALLBACK_FUNC callback_func );


EXTERN void sig_ctrl_tc_enable_grlc          ( UBYTE queue_mode, UBYTE cell_update_cause );
EXTERN BOOL sig_ctrl_tc_disable_prim_queue   ( BOOL         cell_has_changed );
EXTERN BOOL sig_ctrl_tc_abnorm_rel_with_cr_failed
                                             ( BOOL         cell_has_changed );


EXTERN void sig_ctrl_tc_ia_received (T_RRGRR_IA_IND *rrgrr_ia_ind);
EXTERN void sig_ctrl_tc_pdch_assign_received (T_RRGRR_DATA_IND *rrgrr_data_ind);
EXTERN void sig_ctrl_tc_iaext_received (T_RRGRR_IAEXT_IND *rrgrr_iaext_ind);
EXTERN void sig_ctrl_tc_iarej_received(T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind);
EXTERN void sig_ctrl_tc_ia_downlink_received(T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_in );
EXTERN void sig_ctrl_tc_stop_task_cnf_received(void);
EXTERN void sig_ctrl_tc_suspend_dcch_cnf(void);
EXTERN void sig_ctrl_tc_reconnect_dcch_cnf(void);
EXTERN void sig_ctrl_tc_cr_timerout_cnf(void);

EXTERN void sig_ctrl_tc_enter_pam(void);
EXTERN void sig_ctrl_tc_leave_to_ptm(void);
EXTERN void sig_ctrl_tc_leave_to_pim(void);





/*
 *  interface for service CPAP
 */
typedef enum
{
  IA_UL,            /*  */
  IA_DL,            /*  */
  PDCH_UL_ASS_CMD,  /*  */
  PDCH_DL_ASS_CMD   /*  */
} T_PDCH_ASS_CAUSE;


EXTERN void sig_cpap_tc_channel_req( UBYTE );
EXTERN void sig_cpap_tc_dcch_data_req( T_RRGRR_DATA_REQ * rrgrr_data_req_i );
EXTERN void sig_cpap_tc_assign_tbf( T_TBF_TYPE tbf_type );
EXTERN void sig_cpap_tc_assign_sb(void *ptr2prim, UBYTE purpose);
EXTERN void sig_cpap_tc_error_pim( void );
EXTERN void sig_cpap_tc_tbf_created( T_TBF_TYPE tbf_type );
EXTERN void sig_cpap_tc_assign_pdch(T_MPHP_ASSIGNMENT_REQ *ptr2prim, T_PDCH_ASS_CAUSE cause);
EXTERN void sig_cpap_tc_resume_dedi_chan(void );
EXTERN void sig_cpap_tc_suspend_dedi_chan(void );
EXTERN void sig_cpap_tc_send_gprs_data_req(T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req);
EXTERN void sig_cpap_tc_build_res_req(T_U_RESOURCE_REQ *ptr2res_req);
EXTERN void sig_cpap_tc_ctrl_ptm(void);
EXTERN void sig_cpap_tc_start_T3142(ULONG wait);
EXTERN void sig_cpap_tc_stop_T3142(void);
EXTERN void sig_cpap_tc_send_stop_task_req(UBYTE v_stop_ccch);

EXTERN void sig_meas_tc_update_ta_req(void);



/* Access Functions */

EXTERN UBYTE* tc_get_ctrl_blk(UBYTE *index, BOOL unacknowledged);
EXTERN BOOL   tc_is_ctrl_blk_rdy(UBYTE ack_cnt_meas_rpt, UBYTE ack_cnt_other);
EXTERN UBYTE  tc_get_num_ctrl_blck(void);
EXTERN UBYTE  tc_cpy_ctrl_blk_to_buffer(UBYTE *buffer);

/*GFP*/
/*GFP*/
/* functional interface */
EXTERN void sig_gfp_tc_access_rej_ptm        ( void ); 
EXTERN void sig_gfp_tc_ts_reconfig_ptm       ( void ); 
EXTERN void sig_gfp_tc_ul_assign_ptm         ( void ); 
EXTERN void sig_gfp_tc_dl_assign_ptm         ( void ); 
EXTERN void sig_gfp_tc_pdch_release_ptm      ( UBYTE timeslot_i ); 
EXTERN void sig_gfp_tc_packet_tbf_rel_ptm    (ULONG fn, UBYTE rrbp, UBYTE sp  );

EXTERN void sig_gfp_tc_fix_alloc_ack_ptm     ( void );
EXTERN void sig_gfp_tc_update_ta_req_ptm     ( void );


/* mphp interface*/
EXTERN void sig_gfp_tc_queuing_not           ( void ); 
EXTERN void sig_gfp_tc_ul_assign             ( void ); 
EXTERN void sig_gfp_tc_dl_assign             ( void ); 
EXTERN void sig_gfp_tc_access_rej            ( void ); 
EXTERN void sig_gfp_tc_ra_con                ( T_MPHP_RA_CON                * mphp_ra_con_i ); 
EXTERN void sig_gfp_tc_single_block_cnf      ( T_MPHP_SINGLE_BLOCK_CON      * mphp_single_block_con        );
EXTERN void sig_gfp_tc_ra_stop_con           ( T_MPHP_RA_STOP_CON           * mphp_ra_stop_con_i           ); 
EXTERN void sig_gfp_tc_assignment_con        ( void ); 
EXTERN void sig_gfp_tc_tbf_release_con       ( T_MPHP_TBF_RELEASE_CON       * mphp_tbf_release_con_i       ); 
EXTERN void sig_gfp_tc_stop_single_block_con ( T_MPHP_STOP_SINGLE_BLOCK_CON * mphp_stop_single_block_con_i ); 
EXTERN void sig_gfp_tc_poll_ind              (ULONG fn,UBYTE rrbp,UBYTE poll_type,UBYTE pctrl_ack          );
EXTERN void sig_ctrl_tc_delete_cs_msg        ( void );
#endif /* !GRR_TCS_H */

