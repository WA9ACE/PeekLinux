/*
+-----------------------------------------------------------------------------
|  File     : psi_dtxf.h
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
|  Purpose  : Definitions for psi_dtxf.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/
#ifndef PSI_DTXF_H
#define PSI_DTXF_H

EXTERN void psi_dtx_dti_reason_tx_buffer_ready (void);
EXTERN void psi_dtx_dti_reason_tx_buffer_full (void);
EXTERN void psi_dtx_init (void);

#endif /* !PSI_DTXF_H */

