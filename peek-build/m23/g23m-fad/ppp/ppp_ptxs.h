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
|  Purpose :  Definitions for ppp_ptxs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PTXS_H
#define PPP_PTXS_H


EXTERN void sig_arb_ptx_dead_mode_req (void);

EXTERN void sig_arb_ptx_ready_mode_req (void);

EXTERN void sig_arb_ptx_blocked_mode_req (void);
 
EXTERN void sig_arb_ptx_transparent_mode_req (void);
 
EXTERN void sig_frx_ptx_packet_ind (USHORT ptype, 
                                    USHORT packet_len, 
                                    T_desc2* packet);

EXTERN void sig_dti_ptx_tx_buffer_full_ind (void);

EXTERN void sig_dti_ptx_tx_buffer_ready_ind (void);

EXTERN void sig_ftx_ptx_buffer_full_ind (void);

EXTERN void sig_ftx_ptx_buffer_ready_ind (void);


#endif /* !PPP_PTXS_H */
