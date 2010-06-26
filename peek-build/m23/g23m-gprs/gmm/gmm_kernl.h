/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_kernl.h
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
|  Purpose :  Definitions for gmm_kernl.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_KERNL_H
#define GMM_KERNL_H



/*EXTERN void kern_enter_reg_no_cell ( void );*/
EXTERN void kern_enter_reg_limited ( void );
EXTERN void kern_enter_reg_normal ( void );
EXTERN void kern_enter_reg_normal_upon_gmmrr_cell_ind ( void );

EXTERN void kern_err_cause ( USHORT error_cause, BOOL det_acc_sent );

EXTERN void kern_attach ( void );


EXTERN void kern_detach ( void );


EXTERN void kern_rau ( void );
EXTERN void kern_norm_rau ( void );
EXTERN void kern_comb_rau ( void );
EXTERN void kern_rau_init ( void );
EXTERN void kern_periodic_rau ( void );

EXTERN void kern_rau_limited ( void );

EXTERN void kern_aac ( USHORT cause );

EXTERN void kern_l_sim_rem_der ( void );
EXTERN void kern_l_sim_rem_reg ( void );
EXTERN void kern_l_sim_rem_reg_lim ( void );

EXTERN void kern_enter_der ( void );

EXTERN void kern_rau_complete ( UBYTE result_value );

EXTERN void kern_attach_accept
( 
  T_result_gmm                 * result,
  T_force_to_standby           * force_to_standby,
  T_rau_timer                  * rau_timer,
  /* attach: radio_priority */
  T_routing_area_identification *routing_area_identification,
  BOOL                           v_p_tmsi_signature,
  T_p_tmsi_signature           * p_tmsi_signature,
  BOOL                           v_ready_timer,
  T_ready_timer                * ready_timer,
  BOOL                           v_gmobile_identity,
  T_gmobile_identity           * gmobile_identity,
  BOOL                           v_mobile_identity,
  T_mobile_identity            * mobile_identity,
  /*
   * only RAU
   */
  BOOL                           v_receive_n_pdu_number_list,
  T_receive_n_pdu_number_list  * receive_n_pdu_number_list,
  USHORT                         cause,
  BOOL                           v_eqv_plmn_list, /* TCS 2.1 */
  T_eqv_plmn_list              * eqv_plmn_list, /* TCS 2.1 */
  BOOL                           v_t3302,
  T_t3302                      * t3302,
  BOOL                           v_cell_notification
);

EXTERN void kern_imsi_detach_reg_susp ( void );
EXTERN void kern_imsi_detach_der_susp ( void );
EXTERN void kern_enter_reg_no_cell_limited ( void );
EXTERN void kern_enter_reg_no_cell_susp ( void );
EXTERN void kern_enter_reg_limited_susp ( void );
EXTERN void kern_rau_susp ( void );
EXTERN void kern_enter_reg_normal_susp ( void );
EXTERN void kern_resume_grr_der ( void );
EXTERN void kern_resume_grr_reg ( void );
#endif /* !GMM_KERNL_H */

