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
|  Purpose :  Definitions for ppp_ncps.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_NCPS_H
#define PPP_NCPS_H


EXTERN void sig_arb_ncp_parameters_req (UBYTE hc,
                                        UBYTE msid,
                                        ULONG ip,
                                        ULONG pdns, 
                                        ULONG sdns,
                                        ULONG gateway);

EXTERN void sig_arb_ncp_modify_hc_req (UBYTE hc, UBYTE msid);

EXTERN void sig_arb_ncp_packet_req (T_desc2* packet);

EXTERN void sig_arb_ncp_open_req (void);

EXTERN void sig_arb_ncp_close_req (void);

EXTERN void sig_arb_ncp_down_req (void);

EXTERN void sig_arb_ncp_to_req (void);

EXTERN void sig_ona_ncp_tld_req (void);

EXTERN void sig_ona_ncp_tlf_req (void);

EXTERN void sig_ona_ncp_str_req (T_desc2** ptr_packet);

EXTERN void sig_ona_ncp_scr_req (T_desc2** ptr_packet);

EXTERN void sig_ona_ncp_tlu_req (void);

#endif /* !PPP_NCPS_H */

