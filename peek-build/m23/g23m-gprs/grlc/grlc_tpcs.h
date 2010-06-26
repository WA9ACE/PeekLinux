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

#ifndef GRLC_TPCS_H
#define GRLC_TPCS_H

/* TPC Signals */

EXTERN void sig_gff_tpc_pwr_ctrl_ind ( T_MAC_PWR_CTRL_IND * mac_pwr_ctrl_ind );

EXTERN void sig_tm_tpc_update_pch    ( void                                  );

/* PEI_CONFIG Signals */

EXTERN void sig_pei_config_tpc_set_pwr_par( UBYTE alpha, UBYTE gamma );

EXTERN void sig_pei_config_tpc_fix_pcl    ( UBYTE pcl );

/* Access Functions */

EXTERN void tpc_grlc_init  ( void );

EXTERN void tpc_get_pch    ( UBYTE *pch_dest );

EXTERN void tpc_set_pwr_ctrl_param
                           ( T_CGRLC_pwr_ctrl_param *pwr_ctrl_param );

EXTERN void tpc_set_glbl_pwr_ctrl_param
                           ( T_CGRLC_glbl_pwr_ctrl_param *glbl_pwr_ctrl_param );

#endif /* #ifndef GRLC_TPCS_H */

