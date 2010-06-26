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
|  Purpose :  Definitions for grr_tcp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_TCP_H
#define GRR_TCP_H





EXTERN void tc_t3162 ( void );
EXTERN void tc_t3168 ( void );
EXTERN void tc_t3170 ( void );
EXTERN void tc_t3172 ( UBYTE );
EXTERN void tc_t3186 ( void );

EXTERN void tc_cgrlc_ul_tbf_ind        ( T_CGRLC_UL_TBF_IND * cgrlc_ul_tbf_ind );

EXTERN void tc_cgrlc_ctrl_msg_sent_ind ( T_CGRLC_CTRL_MSG_SENT_IND * cgrlc_ctrl_msg_sent_ind );

EXTERN void tc_cgrlc_starting_time_ind ( T_CGRLC_STARTING_TIME_IND * cgrlc_starting_time_ind );

EXTERN void tc_cgrlc_t3192_started_ind ( T_CGRLC_T3192_STARTED_IND * cgrlc_t3192_started_ind );

EXTERN void tc_cgrlc_ta_value_ind      ( T_CGRLC_TA_VALUE_IND * cgrlc_ta_value_ind );


EXTERN void tc_cgrlc_tbf_rel_ind       ( T_CGRLC_TBF_REL_IND * cgrlc_tbf_rel_ind);

EXTERN void tc_cgrlc_cont_res_done_ind ( T_CGRLC_CONT_RES_DONE_IND * cgrlc_cont_res_done_ind);

EXTERN void tc_cgrlc_test_mode_ind     ( T_CGRLC_TEST_MODE_IND * cgrlc_test_mode_ind );

#endif /* !GRR_TCP_H */

