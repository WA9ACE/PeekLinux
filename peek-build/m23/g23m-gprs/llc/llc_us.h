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
|  Purpose :  Definitions for llc_us.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_US_H
#define LLC_US_H




EXTERN void sig_llme_u_assign_req (void);

EXTERN void sig_llme_u_unassign_req (void);

EXTERN void sig_rx_u_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                               T_COMMAND command, T_BIT cr_bit, T_BIT pf_bit);

EXTERN void sig_rx_u_frmr_ind (T_LL_UNITDATA_IND *ll_unitdata_ind,
                               T_PDU_TYPE pdu_type,
                               USHORT frmr_ctrl_length,
                               T_BIT cr_bit,
                               UBYTE frmr_reason);

EXTERN void sig_t200_u_expired_ind 
(
#ifndef LL_DESC 
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#endif
  UBYTE cause,
  T_EXPIRY_MODE_TYPE mode
);

EXTERN void sig_llme_u_reest_req (void);

EXTERN void sig_irx_u_no_frame_expected_ind (void);

#endif /* !LLC_US_H */

