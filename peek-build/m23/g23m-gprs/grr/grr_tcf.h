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

#ifndef GRR_TCF_H
#define GRR_TCF_H

/*
 * used in tc_gen_rand
 */
#define S_VALUE_RESERVED 0xff

typedef enum
{
  R_BUILD_2PHASE_ACCESS 
} T_REASON_BUILD;

typedef enum
{
  SRC_TBF_INFO_RE_ASSIGN,  /* channel reassignment */
  SRC_TBF_INFO_IMM_ASSIGN, /* immediate assignment */
  SRC_TBF_INFO_UL_ASSIGN   /* uplink assignment    */
} T_SRC_TBF_INFO;

typedef enum
{
  E_ACCESS_REJ_NULL,      /* unforeseen mesage  content*/
  E_ACCESS_REJ_IGNORE,    /* wrong address, faulty message */
  E_ACCESS_REJ_WAIT,      /* valid reject with wait indication */
  E_ACCESS_REJ_NO_WAIT,   /* valid reject without wait indication */
  E_ACCESS_REJ_ERROR_DUMMY
} T_EVAL_ACCESS_REJ;

typedef enum
{
  E_UL_ASSIGN_NULL,             /* unforeseen mesage content */
  E_UL_ASSIGN_SB_WITHOUT_TBF,   /* Single Block without TBF Establishment */
  E_UL_ASSIGN_SB_2PHASE_ACCESS, /* Allocation for 2 Phase of 2_Phase-Access */
  E_UL_ASSIGN_DYNAMIC,          /* Dynamic Allocation */
  E_UL_ASSIGN_FIXED,            /* Fixed Allocation */
  E_UL_ASSIGN_RE_ASSIGN,
  E_UL_ASSIGN_ERROR_RA,         /* because of to many PDCH assigned */
  E_UL_ASSIGN_IGNORE            /*Ignore UL Assignment*/ 
} T_EVAL_UL_ASSIGN;

typedef enum
{
  H_RA_CON_STOP_REQ,    /* stop sending of packet access request */
  H_RA_CON_CONTINUE   /* continue sending of packet access request */
} T_HANDLE_RA_CON;

typedef enum
{
  E_DL_ASSIGN_IGNORE,    /* no effect on current tbf */
  E_DL_ASSIGN_ERROR_RA,  /* release and start random access procedure*/ 
  E_DL_ASSIGN,           /* valid message */
  E_DL_ASSIGN_ABORT_DL
} T_EVAL_DL_ASSIGN;

typedef enum
{
  E_TS_IGNORE,                  /* no effect on current tbf(s) */
  E_TS_UL_REASSIG_NEW_DL,       /* reassignment of uplink  allocation and new downlink allocation*/
  E_TS_UL_REASSIG_DL_REASSIG,   /* reassignment of uplink  allocation and  reassignment downlink allocation*/
  E_TS_NEW_UL_DL_REASSIG,       /* new assignment of uplink  allocation and reassignment downlink allocation*/
  E_TS_RECONFIG_ERROR_RA        /* error in packet timeslot reconfigure */
} T_EVAL_TS_RECONFIG;

typedef enum
{
  E_PDCH_REL_NULL,        /* unforeseen mesage content, corrupt message */
  E_PDCH_REL_IGNORE,      /* no effect to current tbf */
  E_PDCH_REL_RELEASE_DL,  /* all timeslots for this tbf removed */
  E_PDCH_REL_RELEASE_UL,  /* all timeslots for this tbf removed */
  E_PDCH_REL_RELEASE_BOTH,/* all timeslots for this tbf removed */
  E_PDCH_REL_RECONF,      /* valid message */
  E_PDCH_REL_RELEASE_DL_RECONF_UL,  /* all timeslots for DL tbf removed, UL reconfigured */
  E_PDCH_REL_RELEASE_UL_RECONF_DL,  /* all timeslots for UL tbf removed  DL reconfigured*/
  E_PDCH_REL_ERROR_DUMMY
} T_EVAL_PDCH_REL;




typedef enum
{
  C_P_LEVEL_NULL,     
  C_P_LEVEL_SEND,   
  C_P_LEVEL_DO_NOT_SEND
} T_CHECK_P_LEVEL;



typedef enum
{
  CAC_OTHER,
  CAC_T3170_EXPIRED
} T_CHECK_ACCESS_CAUSE;


#define SET_STATE_TC_PIM_AFTER_ACCESS_DISABLED( cell_has_changed,   \
                                                enable_cause      ) \
  {                                                                 \
    tc_handle_new_cell( cell_has_changed, enable_cause );           \
                                                                    \
    SET_STATE( TC, TC_PIM );                                        \
    grr_data->tc.disable_class = CGRLC_DISABLE_CLASS_NULL;          \
  }


EXTERN void tc_cgrlc_ul_tbf_res ( UBYTE tbf_mode, UBYTE prim_status );
EXTERN void tc_cgrlc_dl_tbf_req ( void );
EXTERN void tc_cgrlc_tbf_rel_req(UBYTE tbf_type,UBYTE rel_cause,ULONG rel_fn);
EXTERN void tc_cgrlc_access_status_req  ( void );
EXTERN void tc_send_control_msg_to_grlc ( void );
EXTERN void tc_cgrlc_disable_req        ( UBYTE prim_st );
EXTERN void tc_cgrlc_tbf_rel_res        ( UBYTE tbf_type );

EXTERN void tc_cgrlc_enable_req         ( UBYTE queue_mode,
                                          UBYTE cu_cause,
                                          BOOL  cell_has_changed,
                                          UBYTE enable_cause      );


EXTERN void tc_check_access_is_needed ( T_CHECK_ACCESS_CAUSE cause );
EXTERN void tc_send_tbf_rel           ( T_TBF_TYPE );
EXTERN void tc_send_tbf_release_req   ( T_TBF_TYPE tbf_type, BOOL is_synchron );
EXTERN void tc_deactivate_tbf         ( T_TBF_TYPE );
EXTERN void tc_send_polling_res       (UBYTE poll_type_i,ULONG fn_i, UBYTE rrbp_i,UBYTE ctrl_ack_i );


/*************************************************************+*/





EXTERN USHORT tc_gen_rand ( void );
EXTERN USHORT tc_calc_req ( void );
EXTERN T_CHECK_P_LEVEL tc_check_p_level ( void );
EXTERN void tc_send_assign_req ( T_TBF_TYPE tbf_type_i );
EXTERN void tc_send_pdch_rel ( UBYTE ts_available );
EXTERN void tc_abort_tbf ( T_TBF_TYPE tbf_i );


EXTERN void tc_build_res_req (T_U_RESOURCE_REQ *ptr2res_req,
                              T_REASON_BUILD    reason_i,
                              T_SRC_TBF_INFO    src_info_i );
EXTERN T_EVAL_PDCH_REL tc_eval_pdch_rel ( UBYTE );
EXTERN T_EVAL_UL_ASSIGN tc_eval_ul_assign ( void );
EXTERN T_EVAL_DL_ASSIGN tc_eval_dl_assign ( void );
EXTERN T_EVAL_ACCESS_REJ tc_eval_access_rej ( ULONG * t3172_value_i);
EXTERN void tc_send_ra_req ( void );
EXTERN void tc_send_ra_stop_req ( void );
EXTERN void tc_send_single_block ( void );
EXTERN void tc_send_resource_request_p ( void );
EXTERN T_HANDLE_RA_CON tc_handle_ra_con ( void );
EXTERN void tc_init ( void );
EXTERN void tc_handle_error_pim ( void );
EXTERN void tc_handle_error_ra ( void );
EXTERN void tc_activate_tbf ( T_TBF_TYPE );
EXTERN void tc_start_timer_t3172 ( ULONG t3172_value_i );
EXTERN void tc_handle_tbf_start( T_TBF_TYPE tbf_type );
EXTERN void tc_prepare_handle_tbf_start( T_TBF_TYPE    tbf_type,
                                         UBYTE         new_state, 
                                         UBYTE         old_state );
EXTERN void tc_calc_fa_bitmap (ULONG tbf_start,
                               UBYTE blks_or_blkps, 
                               USHORT len,
                               UBYTE * ptr2_alloc_map, 
                               T_p_fixed_alloc  * ptr2_fix_alloc);

//EXTERN void tc_handle_final_alloc (UBYTE final_allocation);

EXTERN void tc_send_ul_repeat_alloc_req( void);
EXTERN USHORT tc_set_fa_bitmap( UBYTE ts_mask, T_FIX_ALLOC * ptr_alloc);
EXTERN T_EVAL_TS_RECONFIG tc_eval_ts_reconf ( void );
EXTERN BOOL tc_set_hopping_par(T_freq_par *freq_par  );
EXTERN void tc_stop_timer_t3172 ( void );

EXTERN BOOL   tc_store_ctrl_blk ( T_BLK_OWNER blk_owner, void *blk_struct );
EXTERN BOOL   tc_cancel_ctrl_blk ( T_BLK_OWNER blk_owner );
EXTERN UBYTE* tc_set_start_ctrl_blk ( UBYTE *index );
EXTERN T_BLK_INDEX tc_set_stop_ctrl_blk ( BOOL        is_tx_success,
                                          T_BLK_OWNER srch_owner,
                                          T_BLK_INDEX start_idx );
EXTERN void   tc_set_stop_tc_ctrl_blk ( void );
EXTERN void   tc_set_stop_all_ctrl_blk ( void );

EXTERN void tc_stop_normal_burst_req ( void );


EXTERN void tc_malloc_assign ( void );
EXTERN void tc_mfree_assign ( BOOL restore_data );

EXTERN T_TIME tc_get_t3170_value(void);

EXTERN T_MPHP_ASSIGNMENT_REQ * tc_set_freq (void);

EXTERN void tc_call_disable_callback_func ( void );


#endif /* !GRR_TCF_H */
