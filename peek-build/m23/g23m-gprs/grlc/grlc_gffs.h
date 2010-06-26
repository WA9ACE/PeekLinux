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

#ifndef GRLC_GFFS_H
#define GRLC_GFFS_H



#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
EXTERN void sig_tm_gff_ul_activate ( UBYTE activate_cause );
#else
EXTERN void sig_tm_gff_ul_activate ( void );
#endif

EXTERN void sig_tm_gff_ul_deactivate ( void );

EXTERN void sig_tm_gff_dl_activate ( void );

EXTERN void sig_tm_gff_dl_deactivate ( void );



#endif /* !GRLC_GFFS_H */

