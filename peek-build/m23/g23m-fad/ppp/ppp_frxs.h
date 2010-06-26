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
|  Purpose :  Definitions for ppp_frxs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_FRXS_H
#define PPP_FRXS_H


EXTERN void sig_ptx_frx_dead_mode_req (void);

EXTERN void sig_ptx_frx_ready_mode_req (void);

EXTERN void sig_ptx_frx_transparent_mode_req (void);

EXTERN void sig_ptx_frx_start_flow_req (void);

EXTERN void sig_ptx_frx_stop_flow_req (void);

EXTERN void sig_arb_frx_dti_connected_req (void);

EXTERN void sig_dti_frx_data_received_ind(T_DTI2_DATA_IND *dti_data_ind);

#endif /* !PPP_FRXS_H */
