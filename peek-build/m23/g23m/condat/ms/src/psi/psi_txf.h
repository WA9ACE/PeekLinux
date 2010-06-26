/*
+-----------------------------------------------------------------------------
|  File     : psi_txf.h
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
|  Purpose  : Definitions for psi_txf.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/ 

#ifndef PSI_TXF_H
#define PSI_TXF_H

EXTERN void psi_tx_init (void);
EXTERN void psi_create_send_buffer(T_dio_buffer** buffer, T_desc_list2 *list, T_desc2** pid_buffer, U8 p_id);
EXTERN void psi_converts_control_info_data(T_parameters *dtx_ctrl_info);

#endif /* !PSI_TXF_H */

