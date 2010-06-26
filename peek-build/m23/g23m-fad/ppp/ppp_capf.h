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
|  Purpose :  Definitions for ppp_chapf.c
+-----------------------------------------------------------------------------
*/

#ifndef PPP_CAPF_H
#define PPP_CAPF_H


EXTERN void chap_init (void);

EXTERN void chap_fill_out_packet (UBYTE pco_buf[], USHORT* ptr_pos);

EXTERN void chap_get_sc (T_desc2** ptr_packet);

EXTERN void chap_get_ss (T_desc2** ptr_packet);

EXTERN void chap_get_sr (T_desc2** ptr_packet);

EXTERN void chap_rc (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void chap_rr (T_desc2** ptr_packet, UBYTE* forward);

EXTERN void chap_rs (T_desc2* packet, UBYTE* forward);

EXTERN void chap_rf (T_desc2* packet, UBYTE* forward);

#ifdef _SIMULATION_
EXTERN void ppp_trace_desc (T_desc2* packet);
#endif

#endif /* CL_RIBU_H */
