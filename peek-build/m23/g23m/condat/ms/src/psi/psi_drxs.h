/*
+-----------------------------------------------------------------------------
|  File     : psi_drxs.h
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
|  Purpose  : Definitions for psi_drxs.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/
#ifndef PSI_DRXS_H
#define PSI_DRXS_H

EXTERN void psi_ker_drx_close (void);
EXTERN void psi_ker_drx_open (void);
EXTERN void psi_tx_drx_close (void);
EXTERN void psi_tx_drx_ready (void);

#endif /* !PSI_DRXS_H */
