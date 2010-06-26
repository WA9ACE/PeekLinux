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
|  Purpose :  Definitions for llc_txp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_TXP_H
#define LLC_TXP_H


EXTERN void tx_grlc_ready_ind ( T_GRLC_READY_IND *grlc_ready_ind );

EXTERN void tx_grlc_suspend_ready_ind ( T_GRLC_SUSPEND_READY_IND *grlc_suspend_ready_ind );

EXTERN void tx_cci_cipher_cnf ( T_GRLC_DATA_REQ *grlc_data_req );


#endif /* !LLC_TXS_H */
