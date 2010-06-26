/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdf.h
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
|  Purpose :  Definitions for sndcp_sdf.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_sdf.h                             $
 * 
 * 16    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 15    15.05.00 16:16 Hk
 * backup.
 * 
 * 14    9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 13    18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 12    30.03.00 16:04 Hk
 * 
 * 11    17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 10    10.03.00 17:49 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 9     7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 8     25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 7     17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 6     11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 5     9.02.00 15:02 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 4     7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 3     3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 2     26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 1     25.01.00 17:31 Hk
 * Initial.

#endif

#ifndef SNDCP_SDF_H
#define SNDCP_SDF_H

/*
 * Convenience functions, not in SDL
 */
EXTERN BOOL sd_f_bit (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN UBYTE sd_get_pcomp (T_LL_UNITDATA_IND* ll_unitdata_ind);
EXTERN BOOL sd_m_bit (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_is_nsapi_rec (UBYTE nsapi, BOOL* b);

EXTERN void sd_is_seg_valid(T_LL_UNITDATA_IND* ll_unitdata_ind,
                         BOOL* valid);

EXTERN void sd_set_nsapi_rec(UBYTE nsapi, BOOL b);

EXTERN void sd_un_d_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_d_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_d_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_d_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_f_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_f_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_f_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_f_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_s_f0_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_s_f0_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_s_f1_m0 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_un_s_f1_m1 (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sd_delete_cur_sn_unitdata_ind (U8 nsapi);

EXTERN void sd_get_unitdata_if_nec (UBYTE sapi);

EXTERN void sd_init (void);

EXTERN void sd_get_nsapi (T_LL_UNITDATA_IND* ll_unitdata_ind, UBYTE* nsapi);

#endif /* !SNDCP_SDF_H */

