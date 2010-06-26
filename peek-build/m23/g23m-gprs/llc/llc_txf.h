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
|  Purpose :  Definitions for llc_txf.c 
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_TXF_H
#define LLC_TXF_H


EXTERN void llc_tx_init (void);

EXTERN void tx_clear_buffer (void);

EXTERN void tx_send_cipher_req 
  (
#ifdef LL_DESC
  T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req,
#else
  T_CCI_CIPHER_REQ *cci_cipher_desc_req,
#endif
  T_PDU_TYPE frame_type,
  UBYTE protected_mode,
  T_FRAME_NUM ns,
  UBYTE cipher,
  ULONG oc
  );

GLOBAL void tx_reserve_buffer 
  (
#ifdef LL_DESC                               
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  T_PRIM_TYPE prim_type,
  UBYTE cause,
  T_SERVICE rx_service,
  ULONG *reservation_no,
  BOOL *buffer_available
  );


GLOBAL void tx_store_buffer (T_GRLC_DATA_REQ *grlc_data_req);

EXTERN void tx_get_next_frame (T_GRLC_DATA_REQ **grlc_data_req,
                               T_GRLC_UNITDATA_REQ **grlc_unitdata_req,
                               T_PRIM_TYPE *prim_type,
                               T_SERVICE *rx_service,
                               UBYTE *sapi);

EXTERN void tx_get_first_data_frame (T_TX_QUEUE **elem);

EXTERN void tx_remove_data_frames (T_SERVICE service, T_SAPI sapi);


#endif /* !LLC_TXS_H */

