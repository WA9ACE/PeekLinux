/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  cnf_sndcp.h
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
|  Purpose :  Dynamic Configuration for SNDCP.
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/cnf_sndcp.h                              $
 * 
 * 23    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 22    15.05.00 16:16 Hk
 * backup.
 * 
 * 21    9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 20    18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 19    30.03.00 16:04 Hk
 * 
 * 18    17.03.00 18:02 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 17    10.03.00 17:48 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 16    7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 15    25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 14    17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 13    11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 12    9.02.00 15:02 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 11    7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 10    3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 9     26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 8     13.01.00 18:07 Hk
 * Backup. Test case SNDCP008 successful.
 * 
 * 7     7.01.00 18:09 Hk
 * Backup. Data transfer in service su in case of LL_READY_IND, test case
 * SNDCP005.
 * 
 * 6     4.01.00 18:56 Hk
 * Work on uplink data transfer, test case SNDCP003 successful.
 * 
 * 5     30.12.99 17:16 Hk
 * Last backup before millennium crash. Continued work on service su, data
 * transfer.
 * 
 * 4     29.12.99 17:21 Hk
 * Started to rework service su, queue of inoming sn_nuitdata_req prims will
 * be designed in more detail in SDL.
 * 
 * 3     28.12.99 18:18 Hk
 * Added test primitives with sdu instead of desc_list and started work on
 * data transfer.
 * 
 * 2     27.12.99 18:33 Hk
 * Implemented and tested SNDCP000, SNDCP001.
 * 
 * 1     21.12.99 18:39 Hk
 * Initial.
 
 
#endif

#ifndef CNF_SNDCP_H
#define CNF_SNDCP_H


/*==== CONST ================================================================*/
/*
 * CONFIGURATION PARAMETER
 *
 * Description :  The constants define the commands for dynamic
 *                configuration proposals.
 */


#endif /* !CNF_SNDCP_H */
