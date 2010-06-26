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
|  Purpose :  Definitions for llc_uirxs.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UIRXS_H
#define LLC_UIRXS_H


EXTERN void sig_llme_uirx_assign_req (void);

EXTERN void sig_llme_uirx_unassign_req (void);

EXTERN void sig_llme_uirx_reset_req (void);

EXTERN void sig_rx_uirx_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                  T_FRAME_NUM nu);


#endif /* !LLC_UIRXS_H */
