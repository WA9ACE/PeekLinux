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
|  Purpose :  Definitions for service TM.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_TMS_H
#define GRLC_TMS_H



/*
 *  interface for service RU
 */
EXTERN void sig_ru_tm_error_ra( void );
EXTERN void sig_ru_tm_cs( void );
EXTERN void sig_ru_tm_end_of_tbf( void );
EXTERN void sig_ru_tm_end_of_pdu ( UBYTE );
EXTERN void sig_ru_tm_prim_delete ( void );
EXTERN void sig_ru_tm_ctrl_blk_sent( UBYTE index );
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
EXTERN void sig_ru_tm_ul_re_est_tbf(void);
#endif

/*
 *  interface for service RD
 */
EXTERN void sig_rd_tm_end_of_tbf( BOOL rel_ul );
EXTERN void sig_rd_tm_ul_req_cnf( void );


EXTERN UBYTE* tm_get_ctrl_blk(UBYTE *index, BOOL unacknowledged);
EXTERN BOOL   tm_is_ctrl_blk_rdy(UBYTE ack_cnt_meas_rpt, UBYTE ack_cnt_other);
EXTERN UBYTE  tm_get_num_ctrl_blck(void);
EXTERN UBYTE  tm_cpy_ctrl_blk_to_buffer(UBYTE *buffer);

EXTERN void sig_ru_tm_repeat_alloc( void );
EXTERN void   sig_ru_tm_end_of_fix_alloc(void);

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
EXTERN void sig_gff_tm_mac_ready_ind_two_phase  (T_MAC_READY_IND *mac_ready_ind);
EXTERN void tm_send_prr_2p_ptm ( void );
#endif


#endif /* !GRLC_TMS_H */

