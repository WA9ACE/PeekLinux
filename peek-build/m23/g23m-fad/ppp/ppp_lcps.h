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
|  Purpose :  Definitions for ppp_lcps.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_LCPS_H
#define PPP_LCPS_H


EXTERN void sig_arb_lcp_parameters_req (USHORT mru, UBYTE ap, ULONG accm);

EXTERN void sig_arb_lcp_modify_ap_req (UBYTE ap);

EXTERN void sig_arb_lcp_packet_req (T_desc2* packet);

EXTERN void sig_arb_lcp_open_req (void);

EXTERN void sig_arb_lcp_close_req (void);

EXTERN void sig_arb_lcp_down_req (void);

EXTERN void sig_arb_lcp_to_req (void);

EXTERN void sig_ona_lcp_tld_req(void);

EXTERN void sig_ona_lcp_tlf_req(void);

EXTERN void sig_ona_lcp_str_req(T_desc2** ptr_packet);

EXTERN void sig_ona_lcp_scr_req(T_desc2** ptr_packet);

EXTERN void sig_ona_lcp_tlu_req(void);

#endif /* !PPP_LCPS_H */

