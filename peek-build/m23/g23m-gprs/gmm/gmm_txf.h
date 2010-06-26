/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txf.h
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
|  Purpose :  Definitions for gmm_txf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_TXF_H
#define GMM_TXF_H




EXTERN void txgmm_init ( void );



EXTERN void tx_unitdata_req ( T_TLLI_TYPE tlli_type, UBYTE msg_type );

EXTERN void tx_fill_unitdata_req ( T_LL_UNITDATA_REQ *ll_unitdata_req_p, T_TLLI_TYPE tlli_type, BOOL cipher );



#endif /* !GMM_TXF_H */

