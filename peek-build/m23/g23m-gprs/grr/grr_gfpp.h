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
|  Purpose :  Definitions for grr_gfpp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_GFPP_H
#define GRR_GFPP_H




EXTERN void gfp_mphp_data_ind ( T_MPHP_DATA_IND *mphp_data_ind );



EXTERN void gfp_mphp_scell_pbcch_stop_con ( T_MPHP_SCELL_PBCCH_STOP_CON *mphp_scell_pbcch_stop_con );



EXTERN void gfp_mphp_stop_pccch_con ( T_MPHP_STOP_PCCCH_CON *mphp_stop_pccch_con );



EXTERN void gfp_mphp_cr_meas_ind ( T_MPHP_CR_MEAS_IND *mphp_cr_meas_ind );



EXTERN void gfp_mphp_cr_meas_stop_con ( T_MPHP_CR_MEAS_STOP_CON *mphp_cr_meas_stop_con );



EXTERN void gfp_mphp_int_meas_ind ( T_MPHP_INT_MEAS_IND *mphp_int_meas_ind );



EXTERN void gfp_mphp_tint_meas_ind ( T_MPHP_TINT_MEAS_IND *mphp_tint_meas_ind );



EXTERN void gfp_mphp_int_meas_stop_con ( T_MPHP_INT_MEAS_STOP_CON *mphp_int_meas_stop_con );



EXTERN void gfp_mphp_ncell_pbcch_ind ( T_MPHP_NCELL_PBCCH_IND *mphp_ncell_pbcch_ind );



EXTERN void gfp_mphp_ra_con ( T_MPHP_RA_CON *mphp_ra_con );



EXTERN void gfp_mphp_ra_stop_con ( T_MPHP_RA_STOP_CON *mphp_ra_stop_con );



EXTERN void gfp_mphp_polling_ind ( T_MPHP_POLLING_IND *mphp_polling_ind );



EXTERN void gfp_mphp_assignment_con ( T_MPHP_ASSIGNMENT_CON *mphp_assignment_con );



EXTERN void gfp_mphp_repeat_ul_fixed_alloc_con ( T_MPHP_REPEAT_UL_FIXED_ALLOC_CON *mphp_repeat_ul_fixed_alloc_con );



EXTERN void gfp_mphp_single_block_con ( T_MPHP_SINGLE_BLOCK_CON *mphp_single_block_con );



EXTERN void gfp_mphp_tbf_release_con ( T_MPHP_TBF_RELEASE_CON *mphp_tbf_release_con );



EXTERN void gfp_mphp_pdch_release_con ( T_MPHP_PDCH_RELEASE_CON *mphp_pdch_release_con );



EXTERN void gfp_mphp_timing_advance_con ( T_MPHP_TIMING_ADVANCE_CON *mphp_timing_advance_con );



EXTERN void gfp_mphp_update_psi_param_con ( T_MPHP_UPDATE_PSI_PARAM_CON *mphp_update_psi_param_con );



EXTERN void gfp_mphp_tcr_meas_ind ( T_MPHP_TCR_MEAS_IND *mphp_tcr_meas_ind );



EXTERN void gfp_mphp_stop_single_block_con ( T_MPHP_STOP_SINGLE_BLOCK_CON *mphp_stop_single_block_con );



EXTERN void gfp_mphp_ncell_pbcch_stop_con ( T_MPHP_NCELL_PBCCH_STOP_CON *mphp_ncell_pbcch_stop_con );


EXTERN void gfp_cgrlc_data_ind ( T_CGRLC_DATA_IND * cgrlc_data_ind );

#endif /* !GRR_GFPP_H */

