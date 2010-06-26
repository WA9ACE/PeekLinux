/*
+-----------------------------------------------------------------------------
|  File     : psi_txs.h
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
|  Purpose  : Definitions for psi_txs.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/ 

#ifndef PSI_TXS_H
#define PSI_TXS_H

EXTERN void psi_drx_tx_data (T_desc_list2* list, T_parameters* dtx_control_info);
EXTERN void psi_drx_tx_data_pkt (T_desc_list2* list, U8 protocol_id);
EXTERN void psi_ker_tx_flush (void);
EXTERN void psi_ker_tx_open(void);
EXTERN void psi_ker_tx_close (void);

#endif /* !PSI_TXS_H */

