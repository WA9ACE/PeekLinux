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
|  Purpose :  Definitions for service TC.
+----------------------------------------------------------------------------- 
*/ 



#ifndef GRLC_TCF_H
#define GRLC_TCF_H


typedef enum
{
  C_PRIM_CONTINUE,            /*  primitive has lower prio, throughput and so on */
  C_PRIM_PUT,                 /*  queue at that possition */
  C_PRIM_REALLOC_START,       /*  primitive needs to start re-allocation immedatly */
  C_PRIM_REALLOC_MARK,        /*  primitive needs to start re-allocation 
                                  at next llc bondary */
  C_PRIM_NEW_TBF,             /*  mark begin of an new tbf in the prim_queue */
  C_PRIM_REALLOC_SHORT,       /*  primitive needs to start re-allocation if previous on is short access*/
  C_PRIM_RLC_MODE_CHANGE      /*  prim whose rlc mode changes and had greater priority*/
} T_COMPARE_PRIM;


typedef enum
{
  R_BUILD_2PHASE_ACCESS, 
  R_RE_ALLOC,
  R_FIX_RE_ALLOC
} T_REASON_BUILD;



EXTERN BOOL           tm_access_allowed             (  UBYTE  radio_prio  );

EXTERN void           tm_get_gmm_prim_queue         ( void );
EXTERN void           tm_handle_grlc_ready_ind      ( void );
EXTERN void           tm_get_llc_prim_queue         ( void );
EXTERN void           tm_ul_tbf_ind                 ( void );
EXTERN UBYTE          tm_prim_queue_get_free_count  ( void );
EXTERN void           tm_abort_tbf                  ( T_TBF_TYPE tbf_i );
EXTERN void           tm_data_req                   ( T_PRIM_TYPE prime_tpye_i,T_GRLC_DATA_REQ * ptr2prim_i);                                                                                
EXTERN void           tm_build_chan_req_des         ( T_chan_req_des * out_i,T_PRIM_QUEUE * p_ptr_i );
EXTERN void           tm_handle_error_ra            ( void );
EXTERN void           tm_build_res_req              ( T_U_GRLC_RESOURCE_REQ *ptr2res_req,
                                                      T_REASON_BUILD    reason_i);
EXTERN BOOL           tm_store_ctrl_blk             ( T_BLK_OWNER blk_owner, void *blk_struct );
EXTERN void           tm_delete_prim_queue          ( void);
EXTERN void           tm_queue_test_mode_prim       ( UBYTE pdu_num_i );
EXTERN void           tm_send_tbf_rel               ( T_TBF_TYPE );
EXTERN T_COMPARE_PRIM tm_compare_prim               ( UBYTE new_prim_i,UBYTE position_i,BOOL *new_qos);
EXTERN void           tm_tfi_handling               ( ULONG tbf_start,
                                                      UBYTE tbf_type, 
                                                      UBYTE ul_tfi, 
                                                      BYTE dl_tfi);                    
EXTERN void           tm_activate_tbf               ( T_TBF_TYPE );
EXTERN void           tm_deactivate_tbf             ( T_TBF_TYPE );
EXTERN void           tm_ini_realloc                ( UBYTE start_of_new_tbf_i );
EXTERN void           tm_init_prim                  ( void );                     
EXTERN void           tm_start_access               ( void );
EXTERN void           tm_grlc_init                  ( void );


EXTERN void           tm_cgrlc_status_ind           ( UBYTE cause ); 

EXTERN void           tm_handle_polling_bit         ( ULONG st_fn, UBYTE tn );

EXTERN void           tm_store_fa_bitmap            (T_CGRLC_fix_alloc_struct * ptr2_fix_alloc);
EXTERN void           tm_handle_final_alloc         (UBYTE final_allocation);
EXTERN USHORT         tm_set_fa_bitmap              ( UBYTE ts_mask, T_FA_ALLOC* ptr_alloc);
EXTERN void           tm_handle_test_mode_cnf       ( BOOL v_test_mode_cnf );

/*********************************************************************************/

EXTERN void           tm_prcs_pwr_ctrl              ( T_CGRLC_pwr_ctrl *pwr_ctrl );

EXTERN BOOL           tm_cancel_ctrl_blk            ( T_BLK_OWNER blk_owner );
EXTERN UBYTE*         tm_set_start_ctrl_blk         ( UBYTE *index );
EXTERN T_BLK_INDEX    tm_set_stop_ctrl_blk          ( BOOL        is_tx_success,
                                                      T_BLK_OWNER srch_owner,
                                                      T_BLK_INDEX start_idx );
EXTERN void           tm_set_stop_tc_ctrl_blk       ( void );
EXTERN void           tm_set_stop_all_ctrl_blk      ( void );

#endif /* !GRLC_TCF_H */
