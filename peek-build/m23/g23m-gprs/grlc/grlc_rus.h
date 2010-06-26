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

#ifndef GRLC_RUS_H
#define GRLC_RUS_H




EXTERN void sig_tm_ru_assign ( void );

EXTERN void sig_tm_ru_abrel ( ULONG fn,BOOL poll_i );

EXTERN void sig_tm_ru_queue_status ( void );

EXTERN void sig_gff_ru_ul_ack (ULONG fn , UBYTE tn, UBYTE rrbp, UBYTE sp);

EXTERN void sig_gff_ru_mac_ready_ind ( T_MAC_READY_IND * mac_ul_ready_ind);

EXTERN void sig_tm_ru_reset_poll_array();

#endif /* !GRLC_RUS_H */

