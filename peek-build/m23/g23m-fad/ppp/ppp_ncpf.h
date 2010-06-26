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
|  Purpose :  Definitions for ppp_ncpf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_NCPF_H
#define PPP_NCPF_H


EXTERN void ncp_init (void);

EXTERN void ncp_get_values (UBYTE* ptr_hc, 
                            UBYTE* ptr_msid, 
                            ULONG* ptr_ip,
                            ULONG* ptr_pdns, 
                            ULONG* ptr_sdns);

EXTERN void ncp_get_scr (T_desc2** ptr_packet);

EXTERN void ncp_get_str (T_desc2** ptr_packet);

EXTERN void ncp_analyze_first_ipcp(T_desc2* packet, 
                                   UBYTE*   ptr_result, 
                                   UBYTE*   ptr_hc, 
                                   UBYTE*   ptr_msid);

EXTERN void ncp_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos);

EXTERN void ncp_analyze_pco (UBYTE pco_buf[], 
                             USHORT pos, 
                             ULONG* ptr_dns1, 
                             ULONG* ptr_dns2,
                             ULONG* ptr_gateway);

EXTERN void ncp_rcr (T_desc2** ptr_packet, UBYTE* isnew, UBYTE* forward);

EXTERN void ncp_rca (T_desc2* packet, UBYTE* forward);

EXTERN void ncp_rcn (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void ncp_rcj (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void ncp_rtr (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void ncp_rta (T_desc2* packet, UBYTE* forward);

EXTERN void ncp_rxj (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void ncp_ruc (T_desc2** ptr_packet, UBYTE* forward);

#endif /* !PPP_NCPF_H */
