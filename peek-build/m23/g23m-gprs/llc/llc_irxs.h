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
|  Purpose :  Definitions for llc_irxs.c 
|             Logical Link Control (LLC)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXS_H
#define LLC_IRXS_H


EXTERN void sig_llme_irx_assign_req (void);

EXTERN void sig_llme_irx_unassign_req (void);

EXTERN void sig_llme_irx_reset_req (void);

EXTERN void sig_llme_irx_abmest_req (void);

EXTERN void sig_llme_irx_abmrel_req (void);

EXTERN void sig_itx_irx_s_frame_req (T_ABIT_REQ_TYPE req);

EXTERN void sig_rx_irx_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                 T_COMMAND         command, 
                                 T_PDU_TYPE        frame_type,
                                 T_BIT             cr_bit,
                                 T_BIT             a_bit,
                                 T_FRAME_NUM       ns, 
                                 T_FRAME_NUM       nr);


#endif /* !LLC_IRXS_H */
