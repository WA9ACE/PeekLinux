/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txs.h
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
|  Purpose :  Definitions for gmm_txs.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_TXS_H
#define GMM_TXS_H




EXTERN void sig_kern_tx_data_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type );
EXTERN void sig_rx_tx_data_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type );


#endif /* !GMM_TXS_H */

