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
|  Purpose :  Definitions for llc_irxf.c
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXF_H
#define LLC_IRXF_H


EXTERN void irx_init            (void);
EXTERN void irx_init_sapi       (void);
EXTERN void irx_init_abm        (void);

EXTERN void irx_queue_store     (T_LL_UNITDATA_IND *ll_unitdata_ind,  
                                 T_FRAME_NUM       ns,
                                 BOOL              *is_busy);
EXTERN void irx_queue_retrieve  (T_LL_UNITDATA_IND **ll_unitdata_ind, BOOL* found);
EXTERN void irx_queue_clean     (void);
EXTERN void irx_get_last_queued_ns (BOOL *found, T_FRAME_NUM *num);

EXTERN void irx_update_vr       (void);
EXTERN void irx_ack_all_to      (T_FRAME_NUM n);
EXTERN void irx_handle_sack     (T_FRAME_NUM nr, T_LL_UNITDATA_IND*, T_PDU_TYPE);
EXTERN void irx_send_ack        (T_BIT a_bit);
EXTERN void irx_send_rnr        (void);

EXTERN void irx_build_sack_bitmap (T_SACK_BITMAP *bitmap);

#endif /* !LLC_IRXF_H */

