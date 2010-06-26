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
|  Purpose :  Definitions for ppp_ftxs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_FTXS_H
#define PPP_FTXS_H

EXTERN void sig_arb_ftx_parameters_req (ULONG accm, UBYTE pfc, UBYTE acfc);

EXTERN void sig_arb_ftx_dead_mode_req (void);

EXTERN void sig_arb_ftx_blocked_mode_req (void);

EXTERN void sig_arb_ftx_ready_mode_req (void);

EXTERN void sig_arb_ftx_transparent_mode_req (void);

EXTERN void sig_any_ftx_packet_req (USHORT ptype, T_desc2* packet);

EXTERN void sig_dti_ftx_tx_buffer_ready_ind (void);

EXTERN void sig_dti_ftx_tx_buffer_full_ind (void);

#endif /* !PPP_FTXS_H */
