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
|  Purpose :  Definitions for ppp_arbp.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ARBP_H
#define PPP_ARBP_H


EXTERN void arb_ppp_establish_req ( T_PPP_ESTABLISH_REQ *ppp_establish_req );

EXTERN void arb_ppp_terminate_req ( T_PPP_TERMINATE_REQ *ppp_terminate_req );

EXTERN void arb_ppp_pdp_activate_res ( T_PPP_PDP_ACTIVATE_RES *ppp_pdp_activate_res );

EXTERN void arb_ppp_pdp_activate_rej ( T_PPP_PDP_ACTIVATE_REJ *ppp_pdp_activate_rej );

EXTERN void arb_ppp_modification_req ( T_PPP_MODIFICATION_REQ *ppp_modification_req );

#endif /* !PPP_ARBP_H */

