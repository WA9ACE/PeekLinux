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
|  Purpose :  Definitions for llc_uitxp.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UITXP_H
#define LLC_UITXP_H



#ifdef LL_DESC
EXTERN void uitx_ll_unitdata_req ( T_LL_UNITDATA_REQ *ll_unitdata_req );
#endif

EXTERN void uitx_ll_unitdesc_req
( 
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req
#endif
);
#endif /* !LLC_UITXS_H */

