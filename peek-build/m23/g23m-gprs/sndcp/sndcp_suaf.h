/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_suaf.h
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
|  Purpose :  Definitions for sndcp_suaf.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_suaf.h                            $
 * 
 * 10    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 9     15.05.00 16:16 Hk
 * backup.
 * 
 * 8     9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 7     18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 6     30.03.00 16:04 Hk
 * 
 * 5     17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 4     10.03.00 17:49 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 3     7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 2     25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 1     17.02.00 16:06 Hk
 * Initial.
 * 
 * 12    17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 11    11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 10    9.02.00 15:03 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 9     7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 8     3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 7     26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 6     13.01.00 18:07 Hk
 * Backup. Test case SNDCP008 successful.
 * 
 * 5     7.01.00 18:09 Hk
 * Backup. Data transfer in service su in case of LL_READY_IND, test case
 * SNDCP005.
 * 
 * 4     4.01.00 18:56 Hk
 * Work on uplink data transfer, test case SNDCP003 successful.
 * 
 * 3     30.12.99 17:16 Hk
 * Last backup before millennium crash. Continued work on service su, data
 * transfer.
 * 
 * 2     29.12.99 17:21 Hk
 * Started to rework service su, queue of inoming sn_nuitdata_req prims will
 * be designed in more detail in SDL.
 * 
 * 1     29.12.99 10:54 Hk
 * Initial.

#endif

#ifndef SNDCP_SUAF_H
#define SNDCP_SUAF_H

#ifdef _SNDCP_DTI_2_
EXTERN U8 sua_get_nsapi (T_LL_DESC_REQ* ll_desc_req);
#else /* _SNDCP_DTI_2_ */
EXTERN UBYTE sua_get_nsapi (T_LL_DATA_REQ* ll_data_req);
#endif /* _SNDCP_DTI_2_ */
 
EXTERN void sua_init (void);

EXTERN void sua_mark_segment (UBYTE npdu_number,
                              UBYTE nsapi);

EXTERN void sua_next_sn_data_req(UBYTE nsapi);
 


#endif /* !SNDCP_SUAF_H */

