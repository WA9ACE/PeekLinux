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
|  Purpose :  Definitions for grlc_gffp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_GFFP_H
#define GRLC_GFFP_H


EXTERN void gff_mac_pwr_ctrl_ind ( T_MAC_PWR_CTRL_IND *mac_pwr_ctrl_ind );


EXTERN void gff_mac_data_ind ( T_MAC_DATA_IND *mac_data_ind );


EXTERN void gff_mac_ready_ind ( T_MAC_READY_IND *mac_ready_ind );


#ifdef _SIMULATION_

EXTERN void gff_l1test_call_mphp_power_control ( T_L1TEST_CALL_MPHP_POWER_CONTROL *return_mphp_power_control );

#endif /* #ifdef _SIMULATION_ */


#endif /* !GRLC_GFFP_H */

