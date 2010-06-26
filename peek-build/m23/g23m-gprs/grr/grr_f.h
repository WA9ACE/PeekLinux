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
| Purpose:     Definitions for grr_f.c .
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_F_H
#define GRR_F_H

 
#define END_OF_LIST 0xff

#define DELETE_MESSAGE 0xff


#define COMPUTE_DATA   0x00
#define INITIALIZE     0x01

#define DB_MODE_INIT      0 /* initialisation                      */
#define DB_MODE_CC_REQ    1 /* cell change request                 */
#define DB_MODE_CC_ACC    2 /* confirmation of cell change request */
                            /* (acceptance of new cell)            */
#define DB_MODE_CC_REJ    3 /* rejection of cell change request    */
                            /* (back to old cell)                  */
#define DB_MODE_SWAP      4 /* swap between the two databases      */

#define PLEV_RND_INDEX    0 
#define S_T_RND_INDEX     1 
#define PCR_RND_INDEX     2 

typedef UBYTE T_DB_MODE;

EXTERN void grr_set_pg_nmo(void);
EXTERN BOOL grr_check_tmsi( BUF_tmsi_field * tmsi_i);              
EXTERN BOOL grr_check_ptmsi( BUF_ptmsi * ptmsi_i);                
EXTERN ULONG grr_buffer2ulong ( BUF_ptmsi * ptmsi);
EXTERN BOOL grr_check_all_tlli ( BUF_tlli_value * tlli_i );
EXTERN void grr_init ( void );
EXTERN UBYTE grr_decode_grr ( T_MSGBUF *  msg_ptr_i );
EXTERN UBYTE grr_decode_rr ( T_MSGBUF *  msg_ptr_i );

EXTERN void grr_encode_ctrl ( UBYTE * ptr_in_i, 
                              T_MSGBUF * ptr_out_i, 
                              UBYTE r_bit_i);
EXTERN BOOL grr_check_address ( UBYTE msg_type_i,UBYTE tn_i );
EXTERN void grr_save_persistence_level ( T_pers_lev * ptr2persistence_level_i );
EXTERN ULONG grr_calc_new_poll_pos ( ULONG fn_i, UBYTE rrbp_i );
EXTERN ULONG grr_random_value (UBYTE index, ULONG max_i);
EXTERN BOOL grr_is_pbcch_present (void);
EXTERN ULONG grr_decode_tbf_start_abs(T_abs *ptr2tbf_abs);
EXTERN ULONG grr_decode_tbf_start_rel(ULONG start_fn, USHORT rel_pos);

EXTERN UBYTE grr_calc_nr_of_set_bits (  UBYTE value  );

EXTERN T_DB_MODE grr_get_db_mode( void );
EXTERN void grr_set_db_ptr( T_DB_MODE mode );
EXTERN void grr_init_db( T_SC_DATABASE *db );

EXTERN BOOL grr_check_dist ( ULONG high_i, ULONG low_i, ULONG dist_i);
EXTERN BOOL handle_ms_cap (UBYTE msg_type );

/* the following functions are used by service MEAS, CS and PSI */
EXTERN void grr_prcs_xmeas_struct    ( T_XMEAS         *db_xmeas,
                                       T_xmeas_par     *air_xmeas,
                                       BOOL             cpy_prm_set,
                                       UBYTE            new_inst_idx,
                                       UBYTE           *start_ext_lst_idx,
                                       UBYTE           *stop_ext_lst_idx   );
EXTERN void grr_init_xmeas_struct    ( T_XMEAS         *xmeas              );
EXTERN BOOL grr_prcs_nc_freq_list    ( T_NC_LIST       *db_list,
                                       T_NC_RFREQ_LIST *rfreq_list,
                                       BOOL             v_air_list,
                                       T_nc_freq_list  *air_list,
                                       T_INFO_TYPE      type,
                                       UBYTE            instance           );
EXTERN void grr_prcs_nc_freq_final   ( T_NC_LIST       *dest_list,
                                       T_NC_RFREQ_LIST *dest_rfreq,
                                       BOOL            *v_src_list_rfreq,
                                       T_NC_LIST       *src_list,
                                       T_NC_RFREQ_LIST *src_rfreq          );
EXTERN void grr_prcs_nc_param_struct ( T_NC_PARAM      *db_nc,
                                       T_nc_meas_par   *ext_nc,
                                       UBYTE            new_idx            );
EXTERN void grr_prcs_nc_param_final  ( T_NC_PARAM      *dest_nc,
                                       BOOL            *v_src_nc,
                                       T_NC_PARAM      *src_nc             );
EXTERN void grr_init_ncmeas_extd_struct
                                     ( T_NCMEAS_EXTENDED *extd,
                                       BOOL               is_cw            );
EXTERN void grr_init_nc_list         ( T_NC_LIST       *list               );

EXTERN void grr_init_nc_param        ( T_NC_PARAM      *param,
                                       BOOL             is_cw              );
EXTERN void grr_sort_ext_lst_freq    ( T_EXT_FREQ_LIST  *list,
                                       UBYTE             max_number,
                                       T_EXT_START_STOP *start_stop        );

/* the above functions are used by service MEAS, CS and PSI */

/*
 * Refrence frequency list, hopping frequencies etc.
 */
EXTERN void grr_create_channel_list(T_rfl* rfl, UBYTE* list);
EXTERN BOOL grr_create_freq_list(UBYTE ma_num, UBYTE maio, T_p_chan_sel *chan_sel, T_p_freq_list *freq_list);
EXTERN void grr_ma_filter_list( 
                               USHORT* list1, /* input*/
                               USHORT* list2, /* output*/
                               const T_gprs_ms_alloc_ie* gp_ma
                               );



/*
 * Cell Reselection functions
 */
EXTERN void grr_prepare_db_for_new_cell (void );

/*
 * NON DRX Functions
 */
EXTERN BOOL grr_is_non_drx_mode( void );
EXTERN BOOL grr_is_non_drx_period(void);
EXTERN void grr_handle_non_drx_period ( T_NON_DRX_TYPE non_drx_type,
                                        BOOL           non_drx_timer_running );

EXTERN void grr_store_g_pwr_par ( T_g_pwr_par      *g_pwr_par );
EXTERN void grr_store_type_pwr_par
                                ( T_pwr_par        *pwr_par,
                                  BOOL              use_prev  );
EXTERN void grr_store_type_tn_alloc_pwr
                                ( T_tn_alloc_pwr   *pwr_par   );
EXTERN void grr_store_type_dyn_alloc
                                ( T_dyn_alloc      *pwr_par   );
EXTERN void grr_store_type_alf_gam
                                ( T_alf_gam        *pwr_par,
                                  UBYTE             tn        );
EXTERN void grr_store_type_pck_upl_ass_ia
                                ( T_tfi_ass_alloc  *pwr_par,
                                  UBYTE             tn        );
EXTERN void grr_store_type_pck_snbl_ass_ia 
                                ( T_sngl_block_alloc *pwr_par,
                                  UBYTE             tn        );

EXTERN void grr_store_type_tfi_ass_rlc
                                ( T_tfi_ass_rlc    *pwr_par,
                                  UBYTE             tn        );

EXTERN void grr_store_type_alpha_gamma
                                ( T_alpha_gamma    *pwr_par,
                                  UBYTE             tn        );

EXTERN void grr_store_type_pwr_ctrl
                                ( T_pwr_ctrl       *pwr_par   );


EXTERN void grr_set_tbf_cfg_req_param
                                ( T_MPHP_ASSIGNMENT_REQ *tbf_cfg_req );

EXTERN void grr_update_pacch( void );

EXTERN USHORT grr_imsi_mod ( void );

EXTERN ULONG grr_get_tlli ( void );

EXTERN void grr_set_buf_tlli ( BUF_tlli_value *buf_tlli_o, ULONG tlli_i );


EXTERN void  grr_set_tlli           ( USHORT * l_tlli,
                                     USHORT * o_tlli,
                                     UBYTE  * b_tlli,
                                     ULONG    tlli          );

EXTERN UBYTE grr_calculate_Ttb (UBYTE ul_usage,UBYTE dl_usage);

EXTERN UBYTE grr_calculate_Tra (UBYTE ul_usage,UBYTE dl_usage);

EXTERN UBYTE grr_reconstruct_dl_usage(UBYTE dl_usage);

EXTERN void grr_encode_ul_ctrl_block ( UBYTE *ul_ctrl_block,
                                       UBYTE *ul_ctrl_data );

EXTERN UBYTE grr_get_ul_ctrl_block_header ( UBYTE r_bit );

EXTERN BOOL grr_check_if_tbf_start_is_elapsed ( ULONG start_fn, ULONG current_fn);

EXTERN UBYTE grr_get_gprs_ms_class( void );

EXTERN BOOL grr_store_cs_param   ( T_NC_LIST    *nc_list,
                                   BOOL          v_cs_par,
                                   T_cs_par     *cs_par,
                                   T_INFO_TYPE   type,
                                   UBYTE         instance,
                                   UBYTE        *number,
                                   USHORT       *last_arfcn,
                                   USHORT        arfcn,
                                   UBYTE         bsic          );
EXTERN void grr_store_si13_pbcch_location
                                 ( T_CR_PAR     *cr_par,
                                   BOOL          v_si13_pbcch,
                                   T_si13_pbcch *si13_pbcch    );
EXTERN void grr_restore_cs_param ( T_ncell_info *curr_info,
                                   T_ncell_info *prev_info,
                                   UBYTE         curr_idx      );
EXTERN T_ncell_info* grr_get_next_ncell_param 
                                 ( UBYTE         max_number,
                                   T_NC_LIST    *list,
                                   T_INFO_SRC    info_src      );
EXTERN T_ncell_info* grr_get_next_bigger_ncell_param 
                                 ( T_NC_LIST    *list,
                                   T_INFO_SRC    info_src      );

EXTERN void grr_handle_ta ( UBYTE v_tav,
                            UBYTE tav,
                            UBYTE v_ul_tai,
                            UBYTE ul_tai,
                            UBYTE ul_tatn, 
                            UBYTE v_dl_tai,
                            UBYTE dl_tai,
                            UBYTE dl_tatn,
                            T_p_timing_advance *ptr2ta);
/*NEWFILES*/
EXTERN void grr_decode_rlcmac ( UBYTE * ptr_msg_i, T_D_HEADER * ptr_d_header_i );
EXTERN T_MSGBUF * grr_handle_rlcmac_header ( T_PACKET_MODE   mode_i, 
                                             T_D_HEADER    * ptr_header_i,
                                             ULONG           fn_i );
EXTERN void grr_increase_dsc ( void );
EXTERN BOOL grr_decrease_dsc ( void );

#define grr_g23_arfcn_to_l1(arfcn)  convert_arfcn_to_l1(arfcn,0)
#define grr_l1_arfcn_to_g23(arfcn)  convert_arfcn_to_g23(arfcn,0)

EXTERN USHORT convert_arfcn_to_l1 (USHORT arfcn, UBYTE std);
EXTERN USHORT convert_arfcn_to_g23 (USHORT arfcn, UBYTE std);

EXTERN T_TIME grr_t_status( USHORT t_index );

#define SET_GPRS_RXLEV_ACCESS_MIN(a,v_b,b) (a=(v_b?b:GPRS_RXLEV_ACCESS_MIN_INVALID))
#define SET_GPRS_MS_TXPWR_MAX_CCH(a,v_b,b) (a=(v_b?b:GPRS_MS_TXPWR_MAX_CCH_INVALID))
#define SET_GPRS_TEMP_OFFSET(a,v_b,b)      (a=(v_b?b:GPRS_TEMPORARY_OFFSET_INVALID))
#define SET_GPRS_PENALTY_TIME(a,v_b,b)     (a=(v_b?b:GPRS_PENALTY_TIME_INVALID    ))
#define SET_GPRS_RESEL_OFF(a,v_b,b)                                                  \
  (a=(v_b?b:(grr_is_pbcch_present()?GPRS_RESEL_OFF_DEFAULT:GPRS_RESEL_OFF_INVALID)))
#define SET_GPRS_HCS_THR(a,v_b,b)          (a=(v_b?b:GPRS_HCS_THR_INVALID         ))
#define SET_GPRS_PRIO_CLASS(a,v_b,b)       (a=(v_b?b:GPRS_PRIORITY_CLASS_INVALID  ))

EXTERN T_NC_MVAL*    grr_get_nc_mval    ( USHORT arfcn, UBYTE bsic, UBYTE *idx );
EXTERN T_ncell_info* grr_get_ncell_info ( USHORT arfcn, UBYTE bsic );

EXTERN void grr_set_pbcch         ( BOOL               pbcch_presence );
EXTERN void grr_set_freq_par      ( T_p_frequency_par *freq_par       );
EXTERN BOOL grr_get_pccch_freq_par( UBYTE              pccch_group, 
                                    T_p_chan_sel      *chan_sel,
                                    T_p_freq_list     *freq_list );

EXTERN void  grr_cgrlc_pwr_ctrl_cnf  ( T_CGRLC_PWR_CTRL_CNF *cgrlc_pwr_ctrl_cnf     );
EXTERN void  grr_cgrlc_pwr_ctrl_req  ( BOOL                  v_c_value              );
EXTERN void  grr_prcs_pwr_ctrl       ( T_CGRLC_pwr_ctrl     *pwr_ctrl,
                                       BOOL                  v_c_value              );
EXTERN UBYTE grr_get_ms_txpwr_max_cch( USHORT                arfcn,
                                       UBYTE                 pwr_offset,
                                       UBYTE                 gprs_ms_txpwr_max_cch  );
EXTERN UBYTE grr_get_pms_max         ( USHORT                arfcn,
                                       UBYTE                 pwr_offset,
                                       UBYTE                 gprs_ms_txpwr_max_cch,
                                       T_MS_PWR_CAP         *mspc                   );
EXTERN void  grr_get_ms_pwr_cap      ( USHORT                arfcn,
                                       UBYTE                 pow_offset,
                                       T_MS_PWR_CAP         *mspc                   );
EXTERN UBYTE grr_get_pcl             ( UBYTE const          *p_ctrl,
                                       SHORT                 nop );

EXTERN void  grr_get_ms_pwr_cap_gsm900 ( UBYTE         rf_pow_cap,
                                         T_MS_PWR_CAP *mspc        );
EXTERN void  grr_get_ms_pwr_cap_dcs1800( UBYTE         rf_pow_cap,
                                         UBYTE         pow_offset,
                                         T_MS_PWR_CAP *mspc        );
EXTERN void  grr_get_ms_pwr_cap_pcs1900( UBYTE         rf_pow_cap,
                                         UBYTE         pow_offset,
                                         T_MS_PWR_CAP *mspc        );
EXTERN UBYTE grr_get_power_class       ( UBYTE         index       );

EXTERN void grr_set_cell_info_service( void );

EXTERN BOOL grr_is_packet_idle_mode( void );

EXTERN void grr_clip_rxlev ( UBYTE *clipp,
                             UBYTE *rxlev, 
                             UBYTE  number );

EXTERN T_gprs_ms_alloc_ie* grr_get_gprs_ma (UBYTE ma_num);

EXTERN BOOL grr_validate_ma_num_in_freq_par (const T_freq_par *freq_par,UBYTE msg_type);

EXTERN BOOL grr_get_ref_list_or_cell_allocation (T_LIST *list1,
                                                 const T_gprs_ms_alloc_ie *gp_ma,
                                                 UBYTE ma_num);
EXTERN BOOL grr_validate_and_count_frequencies ( USHORT radio_freq_list[MPHP_NUMC_MA],
                                                 USHORT* ptr_cnt);

#ifdef REL99

EXTERN T_TIME grr_get_time_to_send_poll(UBYTE rrbp);

/* 
 * Though this funciton is called only by TC, it is placed in
 * grr_f.c so that the temporary static arrays ussed by this
 * fuction can be shared by other function[grr_cnv_freq_para_in_psi8_direct_enc_2]
 * used by CTRL.
 */
EXTERN BOOL grr_cnv_freq_para_in_assignment_direct_enc_2 (T_p_frequency_par *frequency_par,
                                                          T_freq_par *freq_par);

EXTERN BOOL grr_cnv_freq_para_in_psi8_direct_enc_2 (T_cbch *cbch_req,
                                                    const T_freq_par *freq_par);

#ifdef TI_PS_FF_EMR
EXTERN void grr_init_ba_bcch_nc_list ( T_SC_DATABASE   *db                 );

EXTERN void grr_init_enh_param       ( T_GRR_ENH_PARA      *param, 
                                       BOOL             rep_prio_pres      );

GLOBAL void grr_init_enh_cell_list   ( void                                );

EXTERN void grr_prcs_enh_param_cw_temp ( T_GRR_ENH_PARA  *db_enh,
                                         T_enh_rep_param_struct *air_enh,
                                         UBYTE            new_idx          );

EXTERN void grr_prcs_enh_param_pmo   ( T_GRR_ENH_PARA  *db_enh, 
                                       T_enh_meas_param_pmo *air_enh, 
                                       UBYTE  new_idx, UBYTE *pmo_ind      );
EXTERN void grr_prcs_enh_param_pcco  ( T_SC_DATABASE  *db, 
                                       T_enh_meas_param_pcco *air_enh      );

EXTERN void grr_copy_enh_and_nc_params_from_si2qtr
                                     (T_RRGRR_SI2QUATER_IND 
                                      *rrgrr_si2quater_ind                 );

EXTERN UBYTE grr_get_psi3_cm(void);
#endif

#endif

#endif /* !GRR_F_H */

