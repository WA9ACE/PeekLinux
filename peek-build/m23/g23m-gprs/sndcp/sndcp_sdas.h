/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdas.h
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
|  Purpose :  Definitions for sndcp_sdas.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_sdas.h                            $
 * 
 * 8     15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 7     15.05.00 16:16 Hk
 * backup.
 * 
 * 6     9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 5     18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 4     30.03.00 16:04 Hk
 * 
 * 3     17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 2     10.03.00 17:49 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 1     10.03.00 11:43 Hk
 * Initial.
 * 

#endif

#ifndef SNDCP_SDAS_H
#define SNDCP_SDAS_H


EXTERN void sig_cia_sda_cia_decomp_ind(T_CIA_DECOMP_IND* cia_decomp_ind, UBYTE p_id);

EXTERN void sig_mg_sda_end_est (UBYTE sapi, BOOL success);

EXTERN void sig_mg_sda_getdata (UBYTE sapi, UBYTE nsapi);

EXTERN void sig_cia_sda_getdata (UBYTE sapi, UBYTE nsapi);

EXTERN void sig_mg_sda_delete_npdus (UBYTE nsapi,
                                    UBYTE sapi);

EXTERN void sig_mg_sda_reset_ind (UBYTE nsapi);

EXTERN void sig_mg_sda_start_est(UBYTE sapi);

EXTERN void sig_pda_sda_data_ind (T_LL_DATA_IND* ll_data_ind);


EXTERN void sig_nd_sda_getdata_req (UBYTE sapi,
                                       UBYTE nsapi);


#endif /* !SNDCP_SDAS_H */

