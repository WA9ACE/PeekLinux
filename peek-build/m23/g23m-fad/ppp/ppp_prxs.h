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
|  Purpose :  Definitions for ppp_prxs.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PRXS_H
#define PPP_PRXS_H

EXTERN void sig_ftx_prx_start_flow_req (void);

EXTERN void sig_ftx_prx_stop_flow_req (void);

EXTERN void sig_ftx_prx_dead_mode_req (void);

EXTERN void sig_ftx_prx_ready_mode_req (void);

EXTERN void sig_arb_prx_dti_connected_req (void);

EXTERN void sig_dti_prx_data_received_ind (T_DTI2_DATA_IND *dti_data_ind);

#endif /* !PPP_PRXS_H */
