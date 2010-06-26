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
| Purpose:     Definitions for grlc_f.c .
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_F_H
#define GRLC_F_H

 
#define END_OF_LIST 0xff     /* grlc_delete_prim                          */  
#define DELETE_MESSAGE 0xff  /* grlc_ccd_error_handling grlc_decode_grlc  */
#define COMPUTE_DATA   0x00  /* grlc_prbs  testmode                       */
#define INITIALIZE     0x01  /* grlc_prbs  testmode                       */

/*****************************************************************************************************************************/

EXTERN ULONG    grlc_buffer2ulong                   ( BUF_cr_tlli *tlli                                                           );  /*  BOTH_ENT  */
EXTERN void     grlc_init                           ( void                                                                        );
EXTERN void     grlc_delete_prim                    ( void                                                                        ); 
EXTERN UBYTE    grlc_decode_grlc                    ( T_MSGBUF *  msg_ptr_i                                                       );
EXTERN void     grlc_encode_ctrl                    ( UBYTE * ptr_in_i, T_MSGBUF * ptr_out_i, UBYTE r_bit_i                       );
EXTERN void     grlc_prim_put                       ( UBYTE * liststart_i, UBYTE object_i, UBYTE pos_i                            );
EXTERN UBYTE    grlc_prim_get_first                 ( UBYTE * liststart_i                                                         );
EXTERN ULONG    grlc_calc_new_poll_pos              ( ULONG fn_i, UBYTE rrbp_i                                                    );  /*  BOTH_ENT   */
EXTERN UBYTE  * grlc_set_packet_ctrl_ack            ( void                                                                        );  /*  BOTH_ENT?  */
EXTERN void     grlc_send_access_burst              ( UBYTE tn_i                                                                  );
EXTERN void     grlc_send_normal_burst              ( UBYTE * struct_data, UBYTE * encoded_data, UBYTE tn_i                       );
EXTERN void     grlc_del_sent_poll                  ( void                                                                        );
EXTERN void     grlc_save_poll_pos                  ( ULONG fn_i, USHORT tn_i, UBYTE rrbp_i, UBYTE poll_type_i, UBYTE pctrl_ack_i );
EXTERN ULONG    grlc_decode_tbf_start_rel           ( ULONG start_fn, USHORT rel_pos                                              );
EXTERN void     grlc_get_sdu_len_and_used_ts        ( T_RLC_VALUES * values                                                       );
EXTERN BOOL     grlc_check_dist                     ( ULONG high_i, ULONG low_i, ULONG dist_i                                     );
EXTERN void     grlc_handle_poll_pos                ( ULONG current_fn                                                            );
EXTERN void     grlc_send_rem_poll_pos              ( ULONG current_fn                                                            );
EXTERN UBYTE    grlc_test_mode_active               ( void                                                                        );
EXTERN void     grlc_prbs                           ( UBYTE action_i, UBYTE lenght_i, UBYTE * ptr_i                               );  /*  Testmode  */
EXTERN void     grlc_trace_tbf_par                  ( UBYTE tbf_index                                                             );
EXTERN void     grlc_set_buf_tlli                   ( BUF_tlli_value *buf_tlli_o, ULONG tlli_i                                    );  /*  BOTH_ENT  */
EXTERN void     grlc_set_tlli                       ( USHORT * l_tlli, USHORT * o_tlli, UBYTE  * b_tlli, ULONG    tlli            );  /*  BOTH_ENT  */
EXTERN void     grlc_encode_ul_ctrl_block           ( UBYTE *ul_ctrl_block, UBYTE *ul_ctrl_data                                   );  /*  BOTH_ENT  */
EXTERN BOOL     grlc_check_if_tbf_start_is_elapsed  ( ULONG start_fn, ULONG current_fn                                            );
EXTERN void     grlc_activate_tfi                   ( ULONG fn_i                                                                  );
EXTERN T_TIME   grlc_t_status                       ( USHORT t_index                                                              );
EXTERN void     grlc_enter_standby_state            ( void                                                                        );
EXTERN void     grlc_enter_ready_state              ( void                                                                        );

#endif /* !GRLC_F_H */

