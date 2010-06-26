/*
+-----------------------------------------------------------------------------
|  File     : psi_rxf.h
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
|  Purpose  : Definitions for psi_rxf.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/ 

#ifndef PSI_RXF_H
#define PSI_RXF_H

EXTERN void psi_rx_init (void);
EXTERN void psi_rx_read (void);
EXTERN void psi_rx_send_data_to_dtx (T_dio_buffer* buffer,T_DIO_CTRL* control_info);
EXTERN void psi_check_control_info(U32 device,T_DIO_CTRL_LINES *control_info_new);
EXTERN void psi_rx_reconf_pkt (void);
EXTERN void psi_rx_send_data_to_dtx_pkt (T_dio_buffer* buffer);


#endif /* !PSI_RXF_H */

