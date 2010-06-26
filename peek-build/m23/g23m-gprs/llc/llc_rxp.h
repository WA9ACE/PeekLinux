/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  Definitions for llc_rxp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_RXP_H
#define LLC_RXP_H


EXTERN void rx_grlc_data_ind ( T_GRLC_DATA_IND *grlc_data_ind );

EXTERN void rx_grlc_unitdata_ind ( T_GRLC_UNITDATA_IND *grlc_unitdata_ind );

EXTERN void rx_cci_decipher_cnf ( T_LL_UNITDATA_IND *ll_unitdata_ind );

#ifdef _SIMULATION_
EXTERN void rx_grlc_data_ind_test ( T_GRLC_DATA_IND_TEST *grlc_data_ind_test );

EXTERN void rx_grlc_unitdata_ind_test ( T_GRLC_UNITDATA_IND_TEST 
                                      *grlc_unitdata_ind_test );
#endif /* _SIMULATION_ */


#endif /* !LLC_RXS_H */
