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
         
#ifndef GRR_CSF_H
#define GRR_CSF_H

#define CRDM_T_RESELECT 0 /* timer T_RESELECT has expired            */
#define CRDM_CR_CTRL    1 /* cell re-selection request by CTRL       */
#define CRDM_CR_INITIAL 2 /* initial cell re-selection request by CS */
#define CRDM_CR_CONT    3 /* any cell re-selection request after     */
                          /* the initial one initiated by CS         */
typedef UBYTE T_CRDM;     /* cell re-selection decision mode */

EXTERN void  cs_grr_init          ( void                );

EXTERN void  cs_process_cc_order  ( T_nc_meas_par  *nc_meas_par,
                                    UBYTE           v_nc_freq_list,
                                    T_nc_freq_list *nc_freq_list  
#if defined (REL99) AND defined (TI_PS_FF_EMR) 
                                    , UBYTE v_enh_meas_param_pcco,  
                                    T_enh_meas_param_pcco   *enh_meas_param_pcco
#endif
                                  );

EXTERN void  cs_calc_params       ( void                );

EXTERN void  cs_store_meas_values ( void                );

EXTERN void  cs_store_rxlev_values( void                );

EXTERN BOOL  cs_find_strongest    ( ULONG fn_offset     );

EXTERN void  cs_update_bsic       ( void                );


EXTERN T_NC_ORDER cs_get_network_ctrl_order
                                  ( BOOL consider_gmm_state );

EXTERN void  cs_reset_nc_change_mark
                                  ( T_SC_DATABASE *db   );

EXTERN void  cs_reset_meas_rep_params
                                  ( T_SC_DATABASE *db   );

EXTERN void  cs_compare_bsic      ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind );

EXTERN void  cs_calc_cr_criterion ( T_CR_CRITERIONS  *cr_crit,
                                    USHORT            arfcn, 
                                    T_rla_p          *rla_p,
                                    T_CR_PAR_1       *cr_par_1 );

EXTERN void  cs_get_best_cell     ( T_NC_SIX_STRGST **nc,
                                    T_scell_info    **sc       );

EXTERN UBYTE cs_get_cr_meas_mode  ( void                );

EXTERN void  cs_send_cr_meas_req  ( UBYTE del_meas_rslt );

EXTERN void  cs_send_cr_meas_stop_req
                                  ( void                );

EXTERN void  cs_send_update_ba_req
                                  ( T_SC_DATABASE *db   );

EXTERN void  cs_send_meas_rep_req ( UBYTE cause         );

EXTERN void  cs_store_meas_rep_cnf 
                                  ( T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf );

EXTERN void  cs_build_strongest   ( void                );

EXTERN BOOL  cs_send_meas_rpt     ( BOOL   perfrom_init );

EXTERN void  cs_start_t_reselect  ( void                );

EXTERN void  cs_stop_t_reselect   ( void                );

EXTERN void  cs_process_t3158     ( void                );

EXTERN UBYTE cs_is_meas_reporting ( void                );

EXTERN void  cs_cancel_meas_report
                                  ( void                );

EXTERN void  cs_build_nc_ref_list ( T_SC_DATABASE *db,
                                    BOOL           is_dsf );

EXTERN void  cs_init_nc_ref_list  ( T_NC_REF_LIST *list );

EXTERN BOOL  cs_find_candidate    ( void                );

EXTERN BOOL  cs_cr_decision       ( T_CRDM         mode );

EXTERN void  cs_init_nc_mval_lst  ( void                );

EXTERN void cs_stop_cr_meas       ( void                );

EXTERN void cs_req_cr_meas        ( void                );

EXTERN void cs_fill_cr_meas_req   ( T_MPHP_CR_MEAS_REQ *prim, UBYTE id );

EXTERN void cs_reset_all_cell_results ( void            );

EXTERN void cs_reset_all_rxlev_results( void                       );

EXTERN void cs_reset_meas_result      ( T_CELL       *cell         );

EXTERN void cs_fill_meas_rslt         ( T_MPHP_CR_MEAS_IND  *mphp_cr_meas_ind,
                                        T_MPHP_TCR_MEAS_IND *mphp_tcr_meas_ind );

EXTERN BOOL cs_get_rla_p              ( T_RLA_P_DATA *rla_p_data,
                                        UBYTE        *rla_p        );

EXTERN T_RXLEV_AVG cs_get_rxlev_avg   ( T_RXLEV_DATA *rxlev_data   );

EXTERN void cs_tcr_meas_ind_to_pl     ( T_MPHP_TCR_MEAS_IND * prim );

EXTERN T_NC_MVAL* cs_create_cr_cell   ( USHORT arfcn, UBYTE bsic   );

EXTERN void cs_stop_t3158 ( void );

EXTERN void cs_check_nc_sync_timer(USHORT reporting_pd);

#if defined (REL99) AND defined (TI_PS_FF_EMR) 
EXTERN BOOL  cs_send_enh_meas_rpt ( void                );
EXTERN BOOL  cs_is_enh_meas_reporting    ( void                );
EXTERN void  cs_init_ba_bcch_nc_ref_list   ( void                );
#endif

#endif /* !GRR_CSF_H */

