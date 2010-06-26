/*
+-----------------------------------------------------------------------------
|  File     : psi_dtxs.h
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
|  Purpose  : Definitions for psi_dtxs.c
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_DTXS_H
#define PSI_DTXS_H

EXTERN void psi_ker_dtx_close (void);
EXTERN void psi_ker_dtx_open (void);
EXTERN void psi_rx_dtx_data(T_desc2* buffer, U16 len,T_DIO_CTRL* control_info);
EXTERN void psi_rx_dtx_data_pkt(T_desc2* buffer, U16 len, U8 p_id);

#endif /* !PSI_DTXS_H */

