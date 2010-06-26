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
|  Purpose :  Definitions for llc_itxf.c 
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXF_H
#define LLC_ITXF_H


EXTERN void itx_init               (void);

EXTERN void itx_init_sapi          (void);

EXTERN void itx_send_next_frame    (T_ABIT_REQ_TYPE req);

EXTERN void itx_i_queue_clean      (void);

EXTERN void itx_i_queue_store      
(
#ifdef LL_DESC
 T_LL_DESC_REQ*, BOOL *store_failed
#else
 T_LL_DATA_REQ*, BOOL *store_failed
#endif
);
EXTERN void itx_i_queue_set_status (T_IQ_STATUS status, T_FRAME_NUM num);

#ifdef LL_2to1
EXTERN void itx_i_queue_get_ready  (BOOL* found, T_LL_reference1* reference, UBYTE state);
#else
EXTERN void itx_i_queue_get_ready  (BOOL* found, T_reference1* reference, UBYTE state);
#endif

#ifdef LL_2to1
EXTERN void itx_send_ll_data_cnf   (T_LL_reference1* reference);
#else
EXTERN void itx_send_ll_data_cnf   (T_reference1* reference);
#endif

EXTERN void itx_handle_ll_ready_ind(BOOL data_send);

EXTERN void itx_s_queue_store      (T_COMMAND, T_ABIT_REQ_TYPE, T_FRAME_NUM nr, 
                                    T_SACK_BITMAP*);

EXTERN void itx_s_queue_clean      (void);


#endif /* !LLC_ITXF_H */

