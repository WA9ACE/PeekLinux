/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  $
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
|  Purpose :  Definitions for sndcp_cias.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log::                                                                   $

#endif

#ifndef SNDCP_CIAS_H
#define SNDCP_CIAS_H



EXTERN void sig_mg_cia_new_xid(T_XID_BLOCK* new_xid);

EXTERN void sig_mg_cia_delete_npdus(UBYTE nsapi);

EXTERN void sig_su_cia_cia_comp_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                    USHORT npdu_number,
                                    UBYTE nsapi,
                                    UBYTE sapi
                                    );
EXTERN void sig_sua_cia_cia_comp_req (T_SN_DATA_REQ* sn_data_req,
                                    UBYTE npdu_number,
                                    UBYTE nsapi,
                                    UBYTE sapi
                                    );
EXTERN void sig_mg_cia_reset_ind(void);

EXTERN void sig_sd_cia_cia_decomp_req (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sig_sda_cia_cia_decomp_req (T_LL_DATA_IND* ll_data_ind);

#endif /* !SNDCP_CIAS_H */

