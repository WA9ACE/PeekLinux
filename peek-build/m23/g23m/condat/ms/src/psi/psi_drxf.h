/*
+-----------------------------------------------------------------------------
|  File     : psi_drxf.h
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
|  Purpose :  Definitions for psi_drxf.c 
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#ifndef PSI_DRXF_H
#define PSI_DRXF_H 

EXTERN void psi_drx_init (void);
EXTERN void psi_drx_dti_reason_data_received(T_DTI2_DATA_IND *dti2_data_ind);

#endif /* !PSI_DRXF_H */

