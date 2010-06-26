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
|  Purpose :  Definitions for service GFF.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_GFFF_H
#define GRLC_GFFF_H




EXTERN void gff_init ( void );

EXTERN void gff_tbf_init ( void );

EXTERN void gff_send_ctrl_block (ULONG fn_i, UBYTE tn_i, UBYTE * ptr_ctrl_block_i );

EXTERN BOOL gff_analyse_dl_data ( ULONG fn_i, T_dl_data * ptr_dl_data_i );

EXTERN void gff_handle_continious_ta ( void );

EXTERN void gff_clip_rxlev ( UBYTE *clipp, UBYTE *rxlev, UBYTE number );

#endif /* !GRLC_GFFF_H */

