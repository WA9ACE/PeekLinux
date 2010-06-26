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

#ifndef GRR_MEASF_H
#define GRR_MEASF_H

/*==== CONST ================================================================*/

#define BL_FOR_PAGE_PERIOD_CALC 2

#define BLOCK_PER_MF            12          /* blocks per multiframe         */
#define Z_MSEC_PER_FRAME        120
#define N_MSEC_PER_FRAME        26

#define FRAME_PER_MF            (ULONG)52   /* frames per GPRS multiframe    */
#define FRAME_PER_CS_MF         (ULONG)51   /* frames per GSM multiframe     */

#define USEC_PER_FRAME          (ULONG)((MEAS_ACRCY*Z_MSEC_PER_FRAME)/N_MSEC_PER_FRAME)
                                            /* microseconds per frame        */
#define USEC_PER_MF             (ULONG)(FRAME_PER_MF*USEC_PER_FRAME)
                                            /* microseconds per multiframe   */
#define USEC_PER_CS_MF          (ULONG)(FRAME_PER_CS_MF*USEC_PER_FRAME)
                             /* microseconds per circuit switched multiframe */

#define PAGING_CYCLE             64

#define C_VALUE_IDX_DEFAULT  0
#define C_VALUE_DEFAULT      0

/*==== FUNCTION TYPES =======================================================*/

EXTERN void   meas_im_init           ( T_MEAS_IM_MODE        im_mode       );

EXTERN T_MEAS_IM_FILTER*  
              meas_im_get_filter     ( T_MEAS_IM_CARRIER    *ma            );

EXTERN UBYTE  meas_im_fill_abs_iLevel( UBYTE                *v_ilev,
                                       UBYTE                *ilev,
                                       T_MEAS_IM_FILTER     *filter,
                                       UBYTE                 idx           );

EXTERN void   meas_im_fill_rel_iLevel( UBYTE                *v_ilev,
                                       UBYTE                *ilev,
                                       T_MEAS_IM_FILTER     *filter,
                                       UBYTE                 idx           );

EXTERN void   meas_im_new_value      ( T_MPHP_INT_MEAS_IND  *mphp_int_meas_ind,
                                       UBYTE                 state         );

EXTERN void   meas_im_cgrlc_int_level_req
                                     ( BOOL                  init          );

EXTERN void   meas_im_stop_trns      ( void                                );

EXTERN void   meas_im_stop_idle      ( void                                );

EXTERN void   meas_im_start_trns     ( void                                );

EXTERN BOOL   meas_im_start_idle     ( void                                );

EXTERN void   meas_im_prepare_trns   ( void                                );

EXTERN void   meas_im_prepare_idle   ( void                                );

EXTERN void   meas_im_timer_elapsed  ( void                                );

EXTERN void   meas_im_state_changed  ( UBYTE                 state         );

EXTERN void   meas_em_init           ( void                                );


EXTERN UBYTE  meas_em_prcs_meas_order( T_D_MEAS_ORDER       *d_meas_order,
                                       T_NC_ORDER           *ctrl_order    );

EXTERN BOOL   meas_em_is_valid_pmo_present 
                                     ( void                                );

EXTERN void   meas_em_valid_pmo      ( void                                );

EXTERN void   meas_em_valid_psi5     ( void                                );


EXTERN void   meas_em_process_t3178  ( void                                );

EXTERN void   meas_em_start          ( void                                );

EXTERN void   meas_em_stop_req       ( void                                );

EXTERN BOOL   meas_em_store_rslt     ( T_RRGRR_EXT_MEAS_CNF *rrgrr_em_cnf  );

EXTERN BOOL   meas_em_send_meas_rpt  ( BOOL                  perfrom_init  );

EXTERN BOOL   meas_is_spgc_ccch_supported 
                                     ( void                                );

EXTERN void   meas_handle_int_meas_values 
                                     ( T_MPHP_INT_MEAS_IND  *mphp_int_meas_ind,
                                       UBYTE                *rx_lev        );

EXTERN void   meas_c_init            ( void                                );

EXTERN void   meas_c_val_update_pim  ( UBYTE                 rx_lev        );

EXTERN void   meas_handle_pwr_par    ( void                                );

#endif /* !GRR_MEASF_H */

