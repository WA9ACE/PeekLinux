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
|  Purpose :  Definitions for ppp_lcpf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_LCPF_H
#define PPP_LCPF_H


EXTERN void lcp_init (void);

EXTERN void lcp_get_values (UBYTE*  ptr_ap, 
                            USHORT* ptr_mru, 
                            ULONG*  ptr_accm, 
                            UBYTE*  ptr_pfc, 
                            UBYTE* ptr_acfc);

EXTERN void lcp_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos);

EXTERN void lcp_get_scr (T_desc2** ptr_packet);

EXTERN void lcp_get_str (T_desc2** ptr_packet);

EXTERN void lcp_rcr (T_desc2** ptr_packet, UBYTE* isnew, UBYTE* forward);

EXTERN void lcp_rca (T_desc2* packet, UBYTE* forward);

EXTERN void lcp_rcn (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void lcp_rcj (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void lcp_rtr (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void lcp_rta (T_desc2* packet, UBYTE* forward);

EXTERN void lcp_rxj (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void lcp_rpj (T_desc2* packet, UBYTE* forward);

EXTERN void lcp_rer (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void lcp_rep_rdr (T_desc2* packet, UBYTE* forward);

EXTERN void lcp_ruc (T_desc2** ptr_packet, UBYTE* forward);

#endif /* !PPP_LCPF_H */
