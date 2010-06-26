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
|  Purpose :  Definitions for ppp_chaps.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_CAPS_H
#define PPP_CAPS_H


EXTERN void sig_arb_chap_packet_req (T_desc2* packet);

EXTERN void sig_arb_chap_open_req (void);

EXTERN void sig_arb_chap_down_req (void);

EXTERN void sig_arb_chap_stop_req (void);

EXTERN void sig_arb_chap_to_req (void);

#endif /* !PPP_CAPS_H */
