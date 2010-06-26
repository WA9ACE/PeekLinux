/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_txp.h
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
|  Purpose :  Definitions for gmm_txp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_TXP_H
#define GMM_TXP_H



#ifdef GMM_TCS4
EXTERN void tx_gmmsm_unitdata_req ( T_MMPM_UNITDATA_REQ *gmmsm_unitdata_req );
#else
EXTERN void tx_gmmsm_unitdata_req ( T_GMMSM_UNITDATA_REQ *gmmsm_unitdata_req );
#endif



#endif /* !GMM_TXP_H */

