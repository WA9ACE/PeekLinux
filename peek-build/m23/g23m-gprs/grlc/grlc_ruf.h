/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  Definitions for service RU.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RUF_H
#define GRLC_RUF_H




EXTERN  void   ru_init ( void );

EXTERN  void   ru_get_next_sdu ( void );

EXTERN  USHORT ru_calc_rlc_data_size ( T_CODING_SCHEME  cs_type_i, UBYTE ti_bit_i );

EXTERN  USHORT ru_set_block_status ( T_CODING_SCHEME  cs_type_i );

EXTERN T_CODING_SCHEME  ru_get_cs_type ( USHORT  bs_i );

EXTERN  void   ru_tbf_init ( void );

EXTERN  void   ru_send_mac_data_req ( UBYTE bsn_i );

EXTERN  void   ru_set_prim_queue ( BOOL cd_state_i );

EXTERN  UBYTE  ru_countdown_procedure ( UBYTE ret_blocks );

EXTERN  void   ru_update_vb ( void );

EXTERN  void   ru_calc_rlc_data_block ( UBYTE bsn_i );

EXTERN  void   ru_handle_n3102 ( T_PAN_CNT  pan_cnt_i );

EXTERN  UBYTE  ru_calc_va ( void );

EXTERN  void   ru_ret_bsn ( void );

EXTERN  void   ru_change_of_cs ( T_CODING_SCHEME  cs_type_i );

EXTERN  USHORT ru_recalc_rlc_oct_cnt ( void );

EXTERN  BOOL   ru_contention_resolution ( void );

EXTERN  void   ru_delete_prims ( UBYTE last_bsn_i );

EXTERN  BOOL   ru_handle_n3104 ( void );

EXTERN void    ru_send_control_block ( void );

EXTERN void    ru_stall_ind ( void );

EXTERN void    ru_new_data ( void );

EXTERN BOOL    ru_within_window ( UBYTE bsn_i, UBYTE high_value_i,UBYTE low_value_i );

EXTERN void    ru_check_pl_ret ( UBYTE rlc_blocks_sent_i );

EXTERN void    ru_set_T3198 ( UBYTE bsn_i );

EXTERN void    ru_send_ul_dummy_block ( void );

EXTERN void    ru_handle_stall_ind ( void );

EXTERN void    ru_reorg_l1 ( UBYTE sent_blks_i );

EXTERN void    ru_del_prim_in_uack_mode (UBYTE rlc_blocks_sent_i);

EXTERN void    ru_handle_timers ( UBYTE rlc_blocks_sent_i);

EXTERN UBYTE   ru_set_next_bsn_ret ( void);

EXTERN void    ru_handle_nts (UBYTE rlc_blocks_sent_i);

EXTERN void    ru_handle_tbf_start_in_ptm ( UBYTE rlc_blocks_sent_i );

EXTERN void    ru_switch_ul_buffer  ( UBYTE rlc_blocks_sent_i );

EXTERN void ru_cgrlc_st_time_ind  ( void );

EXTERN BOOL ru_ctrl_blk_selection_allowed();

EXTERN BOOL ru_peek_next_sdu(USHORT *sdu_len,UBYTE *active_prim,UBYTE *next_prim);

EXTERN UBYTE ru_peek_for_ctrl_blk();

#endif /* !GRLC_RUF_H */

