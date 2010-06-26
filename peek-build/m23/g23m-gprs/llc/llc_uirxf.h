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
|  Purpose :  Definitions for llc_uirxf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UIRXF_H
#define LLC_UIRXF_H



EXTERN void uirx_init (void);

EXTERN void uirx_init_sapi (void);

EXTERN void uirx_store_nu (T_FRAME_NUM nu, T_FRAME_NUM vur);

EXTERN BOOL uirx_check_nu (T_FRAME_NUM nu, T_FRAME_NUM vur);

EXTERN void uirx_set_new_vur (T_FRAME_NUM new_vur);

EXTERN void uirx_queue_clean (void);

EXTERN void uirx_queue_store (T_LL_UNITDATA_IND *ll_unitdata_ind);

EXTERN BOOL uirx_queue_retrieve (T_LL_UNITDATA_IND **ll_unitdata_ind);


#endif /* !LLC_UIRXS_H */

