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
|  Purpose :  Definitions for llc_txs.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_TXS_H
#define LLC_TXS_H


EXTERN void sig_llme_tx_assign_req (void);

EXTERN void sig_llme_tx_unassign_req (void);

EXTERN void sig_llme_tx_reset_req (void);

EXTERN void sig_llme_tx_ready_req (void);

EXTERN void sig_u_tx_data_req 
(
#ifdef LL_DESC 
 T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
 T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
 UBYTE cause
);

EXTERN void sig_uitx_tx_data_req
  (
#ifdef LL_DESC                                  
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  UBYTE             cipher,
  UBYTE             cause,
  T_FRAME_NUM       nu,
  ULONG             oc
  );

EXTERN void sig_uitx_tx_unitdata_req 
  (
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  UBYTE             cipher,
  T_FRAME_NUM       nu,
  ULONG             oc
  );

EXTERN void sig_itx_tx_data_req 
(
#ifdef LL_DESC
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  T_PDU_TYPE        frame_type,
  T_FRAME_NUM       ns,
  UBYTE             cause,
  USHORT            header_size,
  ULONG             oc
);

EXTERN void sig_llme_tx_trigger_req (UBYTE cause);

EXTERN void sig_llme_tx_resume_req (BOOL grlc_was_suspened);

EXTERN void sig_llme_tx_suspend_req (void);

EXTERN void sig_llme_tx_flush_req (T_SERVICE service);


#endif /* !LLC_TXS_H */

