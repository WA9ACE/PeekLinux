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
|  Purpose :  Definitions for service RD.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RDS_H
#define GRLC_RDS_H




EXTERN void sig_tm_rd_assign ( void );

EXTERN void sig_tm_rd_abrel ( ULONG fn, BOOL poll );

EXTERN void sig_tm_rd_ctrl_block ( UBYTE *ptr_ctrl_block_i );

EXTERN void sig_tm_rd_ul_req ( void );

EXTERN void sig_tm_rd_ul_req_stop ( void) ;

EXTERN void sig_tm_rd_nor_rel ( void);

EXTERN void sig_gff_rd_mac_ready_ind ( T_MAC_READY_IND * mac_dl_ready_ind);

EXTERN void sig_gff_rd_data (ULONG fn, UBYTE tn, USHORT block_status,UBYTE rrbp, UBYTE sp,UBYTE bsn,UBYTE fbi,UBYTE e_bit, UBYTE * ptr_dl_block);

EXTERN void sig_ru_rd_get_downlink_release_state( BOOL *release_state);

#endif /* !GRLC_RDS_H */

