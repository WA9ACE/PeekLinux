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
|  Purpose :  Definitions for ppp_arbs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_ARBS_H
#define PPP_ARBS_H


EXTERN void sig_ptx_arb_packet_ind (USHORT ptype, T_desc2* packet);

EXTERN void sig_lcp_arb_rpj_ind (USHORT ptype);

EXTERN void sig_rt_arb_to_ind (void);

EXTERN void sig_any_arb_tlf_ind (void);

EXTERN void sig_any_arb_tlu_ind (void);

EXTERN void sig_lcp_arb_tld_ind (void);

EXTERN void sig_ncp_arb_tld_ind (void);

EXTERN void sig_frx_arb_escape_ind (void);

EXTERN void sig_dti_arb_connection_opened_ind (
  DTI_HANDLE hDTI,
  U8 instance,
  U8 inter_face,
  U8 channel
  );

EXTERN void sig_dti_arb_connection_closed_ind (
  DTI_HANDLE hDTI,
  U8 instance,
  U8 inter_face,
  U8 channel
  );

#endif /* !PPP_ARBS_H */
