/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  Definitions for grr_measp.c.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_MEASP_H
#define GRR_MEASP_H

EXTERN void meas_t_im_sync ( void );

EXTERN void meas_t3178 ( void );

EXTERN void meas_p_meas_order_ind ( T_D_MEAS_ORDER *d_meas_order );

EXTERN void meas_tb_rxlev_sc_req ( T_TB_RXLEV_SC_REQ *tb_rxlev_sc_req );

#endif /* !GRR_MEASP_H */

