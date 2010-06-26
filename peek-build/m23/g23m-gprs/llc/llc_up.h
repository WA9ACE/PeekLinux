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
|  Purpose :  Definitions for llc_up.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UP_H
#define LLC_UP_H




EXTERN void u_ll_establish_req ( T_LL_ESTABLISH_REQ *ll_establish_req );

EXTERN void u_ll_establish_res ( T_LL_ESTABLISH_RES *ll_establish_res );

EXTERN void u_ll_release_req ( T_LL_RELEASE_REQ *ll_release_req );

EXTERN void u_ll_xid_req ( T_LL_XID_REQ *ll_xid_req );

EXTERN void u_ll_xid_res ( T_LL_XID_RES *ll_xid_res );



#endif /* !LLC_US_H */

