/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdaf.h
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
|  Purpose :  Definitions for sndcp_sdaf.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_sdaf.h                            $
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

#ifndef SNDCP_SDAF_H
#define SNDCP_SDAF_H

/*
 * Convenience functions, not in SDL
 */
EXTERN BOOL sda_f_bit (T_LL_DATA_IND* ll_data_ind);
EXTERN UBYTE sda_get_pcomp (T_LL_DATA_IND* ll_data_ind);
EXTERN BOOL sda_m_bit (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_is_seg_valid(T_LL_DATA_IND* ll_data_ind,
                         BOOL* valid);
EXTERN void sda_is_nsapi_rec (UBYTE nsapi, BOOL* b);

EXTERN void sda_set_nsapi_rec(UBYTE nsapi, BOOL b);

EXTERN void sda_ac_f_f0 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_f_f1_m0 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_f_f1_m1 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_s_f0_m0 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_s_f0_m1 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_s_f1_m0 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_ac_s_f1_m1 (T_LL_DATA_IND* ll_data_ind);

EXTERN void sda_delete_cur_sn_data_ind (U8 nsapi);

EXTERN void sda_get_data_if_nec (UBYTE sapi);

EXTERN void sda_init (void);

EXTERN void sda_get_nsapi(T_LL_DATA_IND* ll_data_ind, UBYTE* nsapi);

#endif /* !SNDCP_SDF_H */

