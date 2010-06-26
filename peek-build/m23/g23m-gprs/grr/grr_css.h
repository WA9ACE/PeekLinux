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
|  Purpose :  Definitions for service CS.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CSS_H
#define GRR_CSS_H

/* CTRL Signals */

EXTERN void sig_ctrl_cs_check_bsic_ind ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind );

EXTERN void sig_ctrl_cs_reselect       ( T_CS_RESELECT_CAUSE reselect_cause );

EXTERN void sig_ctrl_cs_start          ( void );

EXTERN void sig_ctrl_cs_stop           ( T_CS_DC_STATUS dc_stat );

EXTERN void sig_ctrl_cs_gmm_state      ( T_GMM_STATES state );

EXTERN void sig_ctrl_cs_cc_result      ( BOOL cc_status );

EXTERN void sig_ctrl_cs_ptm            ( void );

EXTERN void sig_ctrl_cs_leave_to_pam   ( void );

EXTERN void sig_ctrl_cs_enter_pam      ( void );

EXTERN void sig_ctrl_cs_pim            ( void ); 

EXTERN void sig_ctrl_cs_meas_rep_cnf   ( T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf );

EXTERN void sig_ctrl_cs_install_new_scell 
                                       ( void );

EXTERN BOOL sig_ctrl_cs_check_c1_new_scell 
                                       ( BOOL  cell_change_order,
                                         UBYTE gprs_rxlev_access_min,
                                         UBYTE gprs_ms_txpwr_max_cch );

EXTERN void sig_ctrl_cs_start_cell_selection 
                                       ( UBYTE cr_type );

EXTERN void sig_ctrl_cs_cc_order_ind   ( T_D_CELL_CHAN_ORDER *d_cell_chan_order,
                                         T_D_CHANGE_ORDER    *d_change_order );

EXTERN void sig_ctrl_cs_pmr_accept     ( void );

EXTERN void sig_ctrl_cs_pmr_reject     ( void );

EXTERN void sig_ctrl_cs_check_cell_location ( USHORT arfcn, UBYTE bsic );

#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void sig_ctrl_cs_si2quater_ind   ( T_RRGRR_SI2QUATER_IND *rrgrr_si2quater_ind );
#endif

EXTERN void sig_ctrl_cs_simu_timer_stop ( void );

EXTERN void sig_ctrl_cs_nc_param_invalid ( void );
 
EXTERN void sig_ctrl_cs_nc_param_valid   ( void );

EXTERN void sig_ctrl_cs_ba_changed       ( void );

EXTERN void sig_ctrl_cs_reset_meas_rep_params       ( void );

/* MEAS Signals */

EXTERN void sig_meas_cs_meas_order     ( T_NC_ORDER prev_ctrl_order );

/* PG Signals */

EXTERN void sig_pg_cs_pause            ( void );

EXTERN void sig_pg_cs_resume           ( void );

/* GFP Signals */

EXTERN void sig_gfp_cs_cr_meas_ind     ( T_MPHP_CR_MEAS_IND * mphp_cr_meas_ind );

EXTERN void sig_gfp_cs_tcr_meas_ind    ( T_MPHP_TCR_MEAS_IND * mphp_tcr_meas_ind );

EXTERN void sig_gfp_cs_cr_meas_stop_con( T_MPHP_CR_MEAS_STOP_CON * dummy       );

#endif /* !GRR_CSS_H */

