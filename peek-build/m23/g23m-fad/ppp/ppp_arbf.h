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
|  Purpose :  Definitions for ppp_arbf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ARBF_H
#define PPP_ARBF_H


EXTERN void arb_init (void);

EXTERN void arb_discard_packet (T_desc2* packet);

EXTERN void arb_get_protocol_reject (USHORT ptype, 
                                     T_desc2*  packet, 
                                     T_desc2** ptr_packet);

EXTERN void arb_get_pco_list (T_sdu* ptr_pco_list);

EXTERN void arb_analyze_pco_list (T_sdu*  pco_list, 
                                  ULONG*  ptr_dns1,
                                  ULONG*  ptr_dns2,
                                  ULONG*  ptr_gateway);

EXTERN void arb_dti_close_peer_ind (void);

EXTERN void arb_dti_close_prot_ind (void);

#endif /* !PPP_ARBF_H */
