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
|  Purpose :  Definitions for service MEAS.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_MEASS_H
#define GRR_MEASS_H

/* Constants */

#define NO_PAGING               0xFFFFFFFF

/* CTRL Signals */

EXTERN void sig_ctrl_meas_int_list_invalid   ( void );
  
EXTERN void sig_ctrl_meas_int_list_valid     ( void );

EXTERN void sig_ctrl_meas_ext_invalid_psi5   ( void );
 
EXTERN void sig_ctrl_meas_ext_valid_psi5     ( void );

EXTERN void sig_ctrl_meas_pim                ( void );

EXTERN void sig_ctrl_meas_pam                ( void );

EXTERN void sig_ctrl_meas_ptm                ( void );

EXTERN void sig_ctrl_meas_start              ( void );

EXTERN void sig_ctrl_meas_stop               ( void );

EXTERN void sig_ctrl_meas_suspend            ( void );

EXTERN void sig_ctrl_meas_resume             ( void );

EXTERN void sig_ctrl_meas_pmr_accept         ( void );

EXTERN void sig_ctrl_meas_pmr_reject         ( void );

EXTERN void sig_ctrl_meas_ext_meas_cnf       ( T_RRGRR_EXT_MEAS_CNF *rrgrr_ext_meas_cnf );

EXTERN void sig_ctrl_meas_ba_changed         ( void );

/* CS Signals */

EXTERN void sig_cs_meas_start                ( void );

EXTERN void sig_cs_meas_stop                 ( void );

/* GFP Signals */

EXTERN void sig_gfp_meas_order               ( void );

EXTERN void sig_gfp_meas_int_meas_stop_con   ( T_MPHP_INT_MEAS_STOP_CON * dummy );

EXTERN void sig_gfp_meas_int_meas_ind        ( T_MPHP_INT_MEAS_IND * mphp_int_meas_ind );

EXTERN void sig_gfp_meas_rxlev_pccch_ind     ( UBYTE pccch_lev );

EXTERN void sig_gfp_meas_ctrl_pwr_ta_ptm     ( void );

/* TC Signals */

EXTERN void sig_tc_meas_update_pch           ( void );

/* Access Functions */

EXTERN void   meas_init                 ( T_MEAS_IM_MODE        im_mode_i );
  
EXTERN void   meas_im_get_rel_i_level   ( T_MEAS_IM_CARRIER    *ma,
                                          T_ilev               *i_level   );

EXTERN UBYTE  meas_im_get_abs_i_level   ( T_MEAS_IM_CARRIER    *ma,
                                          T_ilev_abs           *i_level   );

EXTERN void   meas_im_set_carrier       ( T_MEAS_IM_CARRIER    *carrier,
                                          T_p_frequency_par    *freq_par  );

EXTERN ULONG  meas_im_get_drx_period_seconds
                                        ( void                            );

EXTERN USHORT meas_im_get_drx_period_frames
                                        ( void                            );

EXTERN UBYTE  meas_im_get_permit        ( void                            );

EXTERN UBYTE  meas_c_get_value          ( void                            );

EXTERN void   meas_c_get_c_value        ( T_CGRLC_c_value      *c_value   );

EXTERN void   meas_c_set_c_value        ( T_CGRLC_c_value      *c_value   );

#endif /* !GRR_MEASS_H */

