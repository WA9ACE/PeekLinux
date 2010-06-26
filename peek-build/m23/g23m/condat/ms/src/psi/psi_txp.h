/*
+-----------------------------------------------------------------------------
|  File     : psi_txp.h
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
|  Purpose  : Definitions for psi_txp.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_TXP_H
#define PSI_TXP_H

EXTERN void psi_tx_sig_flush_ind (U32 device);
EXTERN void psi_tx_sig_write_ind (U32 device);
EXTERN BOOL psi_free_tx_buffer (T_dio_buffer* write_buffer);
EXTERN void psi_fill_tx_buf_list (T_dio_buffer* write_buffer, T_desc2* list, T_desc2* pid_buffer);
EXTERN void psi_fill_tx_buf_pending_list (T_dio_buffer* write_buffer, T_desc2* list, T_desc2* pid_buffer);
EXTERN void psi_fill_tx_buf_flush_pending_list (T_dio_buffer* write_buffer, T_desc2* list);
EXTERN void psi_free_pending_tx_buf (void);
EXTERN void psi_mfree_desc2_chain (T_desc2* dti_data_list);


#endif /* !PSI_TXP_H */
