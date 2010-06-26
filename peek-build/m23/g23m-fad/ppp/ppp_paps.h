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
|  Purpose :  Definitions for ppp_paps.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PAPS_H
#define PPP_PAPS_H

EXTERN void sig_arb_pap_packet_req(T_desc2* packet);

EXTERN void sig_arb_pap_open_req (void);

EXTERN void sig_arb_pap_down_req (void);

EXTERN void sig_arb_pap_stop_req (void);

EXTERN void sig_arb_pap_to_req (void);

#endif /* !PPP_PAPS_H */
