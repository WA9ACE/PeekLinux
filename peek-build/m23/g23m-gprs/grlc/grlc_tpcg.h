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
|  Purpose :  Definitions for service TPC.
+----------------------------------------------------------------------------- 
*/ 

/*
 * !!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!
 * 
 * This file was previously named grlc_tpcf.h.
 *
 * A renaming was necessary due to an ClearCase evil twin problem across the
 * TCS2/TCS3/TCS4 and the TCS5 programs.
 *
 * The same applied for the file grlc_tpcf.c.
 * 
 * !!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!ATTENTION!!!
 */

#ifndef GRLC_TPCG_H
#define GRLC_TPCG_H

EXTERN void  tpc_get_ms_pwr_cap ( UBYTE         band_indicator, 
                                  T_MS_PWR_CAP *ms_pwr_cap      );

EXTERN UBYTE tpc_get_pcl        ( UBYTE const  *p_ctrl,
                                  SHORT         nop             );

#endif /* #ifndef GRLC_TPCG_H */

