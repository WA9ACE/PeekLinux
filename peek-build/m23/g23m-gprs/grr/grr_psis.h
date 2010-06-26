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
|  Purpose :  Definitions for service PSI.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PSIS_H
#define GRR_PSIS_H

EXTERN void sig_ctrl_psi_si13_received ( T_RRGRR_GPRS_SI13_IND *rrgrr_si13_ind,
                                         BOOL                   ba_bcch_changed );

EXTERN void sig_ctrl_psi_pim ( void );

EXTERN void sig_ctrl_psi_pam ( void );

EXTERN void sig_ctrl_psi_ptm ( void );

EXTERN void sig_ctrl_psi_suspend ( void );

EXTERN void sig_ctrl_psi_resumpt ( void );

EXTERN void sig_ctrl_psi_resumpt_cc ( void );

EXTERN void sig_ctrl_psi_stop ( void );

EXTERN void sig_ctrl_psi_sync_ok (void);

EXTERN void sig_ctrl_psi_read_full_psi_in_new_cell(void);

EXTERN void sig_ctrl_psi_rel_state(T_RELEASE_STATE rel_state);

EXTERN void sig_ctrl_psi_access_disabled ( T_PSI_DISABLE_CAUSE dc );

/* GFP PTM*/
EXTERN void sig_gfp_psi_1_ptm     ( void ); 
EXTERN void sig_gfp_psi_2_ptm     ( void ); 
EXTERN void sig_gfp_psi_3_ptm     ( void ); 
EXTERN void sig_gfp_psi_3_bis_ptm ( void );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void sig_gfp_psi_3_ter_ptm ( void );
#endif
EXTERN void sig_gfp_psi_4_ptm     ( void );
EXTERN void sig_gfp_psi_13_ptm    ( void ); 
#ifdef REL99
EXTERN void sig_gfp_psi_8_ptm     ( void ); 
#endif
/* GFP*/
EXTERN void sig_gfp_psi_1 ( void ) ;
EXTERN void sig_gfp_psi_2 ( UBYTE rel_pos_i) ;
EXTERN void sig_gfp_psi_3 ( UBYTE rel_pos_i) ;
EXTERN void sig_gfp_psi_3_bis ( UBYTE rel_pos_i );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
EXTERN void sig_gfp_psi_3_ter ( UBYTE rel_pos_i );
#endif
EXTERN void sig_gfp_psi_4 ( UBYTE rel_pos_i ) ;
EXTERN void sig_gfp_psi_5 ( UBYTE rel_pos_i ) ;
EXTERN void sig_gfp_psi_prach ( void ) ;
EXTERN void sig_gfp_psi_scell_pbcch_stop_con ( T_MPHP_SCELL_PBCCH_STOP_CON *mphp_scell_pbcch_stop_con) ;
EXTERN void sig_gfp_psi_ncell_pbcch_ind ( T_MPHP_NCELL_PBCCH_IND *mphp_ncell_pbcch_ind) ;
EXTERN void sig_gfp_psi_save_persistence_level ( T_pers_lev * ptr2persistence_level_i  ); /*MODIF*/
#ifdef REL99
EXTERN void sig_gfp_psi_8 ( UBYTE rel_pos_i) ;
#endif

#endif /* !GRR_PSIS_H */

