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
|  Purpose :  Definitions for llc_uitxs.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UITXS_H
#define LLC_UITXS_H


EXTERN void sig_llme_uitx_assign_req (void);

EXTERN void sig_llme_uitx_unassign_req (void);

EXTERN void sig_llme_uitx_reset_req (void);

EXTERN void sig_tx_uitx_ready_ind (void);

EXTERN void sig_tx_uitx_trigger_ind (UBYTE cause);


#endif /* !LLC_UITXS_H */
