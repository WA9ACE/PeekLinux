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
|  Purpose :  Definitions for llc_txl.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_TXL_H
#define LLC_TXL_H


EXTERN void tx_label_s_data (T_SERVICE rx_service, 
                             T_GRLC_DATA_REQ *grlc_data_req);

EXTERN void tx_label_s_unitdata (T_SERVICE rx_service, 
                                 T_GRLC_UNITDATA_REQ *grlc_unitdata_req);


EXTERN void tx_label_s_removed (T_SERVICE rx_service, UBYTE sapi);

#endif /* !LLC_TXL_H */
