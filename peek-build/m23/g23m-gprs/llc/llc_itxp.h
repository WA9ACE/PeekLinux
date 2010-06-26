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
|  Purpose :  Definitions for llc_itxp.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXP_H
#define LLC_ITXP_H


#ifdef LL_DESC
EXTERN void itx_ll_data_req ( T_LL_DATA_REQ *ll_data_req );
#endif

EXTERN void itx_ll_desc_req
(
#ifdef LL_DESC
  T_LL_DESC_REQ *ll_desc_req
#else
  T_LL_DATA_REQ *ll_desc_req
#endif
);

#endif /* !LLC_ITXP_H */

