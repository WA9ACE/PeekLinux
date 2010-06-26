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
|  Purpose :  Definitions for llc_ul.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_UL_H
#define LLC_UL_H


EXTERN void   u_label_xid              (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                        T_COMMAND command,
                                        T_BIT cr_bit,
                                        T_BIT pf_bit);

EXTERN void   u_label_xidres           (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                        T_COMMAND command,
                                        T_BIT cr_bit,
                                        T_BIT pf_bit);

EXTERN void   u_label_xid_sabm_send    (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                        T_BIT cr_bit,
                                        T_BIT pf_bit);

EXTERN void   u_label_xid_valid        (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                        BOOL reset_received,
                                        BOOL xid_ind);

EXTERN void   u_label_frmr_cond        (T_LL_UNITDATA_IND *ll_unitdata_ind,
                                        T_BIT cr_bit,
                                        UBYTE frmr_reason);

EXTERN void   u_label_frmr_cond_reest  (T_LL_UNITDATA_IND *ll_unitdata_ind,
                                        T_BIT cr_bit,
                                        UBYTE frmr_reason);

EXTERN void   u_label_s_est_ind        (T_BIT pf_bit);


#endif /* !LLC_UL_H */
