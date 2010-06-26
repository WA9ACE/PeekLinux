/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_mgp.h
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
|  Purpose :  Definitions for sndcp_mgp.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_mgp.h                             $
 * 
 * 24    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 23    15.05.00 16:16 Hk
 * backup.
 * 
 * 22    9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 21    18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 20    30.03.00 16:04 Hk
 * 
 * 19    17.03.00 18:02 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 18    10.03.00 17:48 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 17    7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 16    25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 15    17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 14    11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 13    9.02.00 15:02 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 12    7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 11    3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 10    26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 9     13.01.00 18:07 Hk
 * Backup. Test case SNDCP008 successful.
 * 
 * 8     7.01.00 18:09 Hk
 * Backup. Data transfer in service su in case of LL_READY_IND, test case
 * SNDCP005.
 * 
 * 7     4.01.00 18:56 Hk
 * Work on uplink data transfer, test case SNDCP003 successful.
 * 
 * 6     30.12.99 17:16 Hk
 * Last backup before millennium crash. Continued work on service su, data
 * transfer.
 * 
 * 5     29.12.99 17:21 Hk
 * Started to rework service su, queue of inoming sn_nuitdata_req prims will
 * be designed in more detail in SDL.
 * 
 * 4     28.12.99 18:18 Hk
 * Added test primitives with sdu instead of desc_list and started work on
 * data transfer.
 * 
 * 3     27.12.99 18:33 Hk
 * Implemented and tested SNDCP000, SNDCP001.
 * 
 * 2     21.12.99 18:39 Hk
 * First compilation of stubs.
 * 
 * 1     20.12.99 17:41 Hk
 * Initial.

#endif

#ifndef SNDCP_MGP_H
#define SNDCP_MGP_H

#ifndef SNDCP_UPM_INCLUDED
EXTERN void mg_sn_switch_req ( T_SN_SWITCH_REQ *sn_switch_req );
#endif

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_sn_dti_req ( T_SN_DTI_REQ *sn_dti_req);
#endif

EXTERN void mg_ll_reset_ind ( T_LL_RESET_IND *ll_reset_ind );


EXTERN void mg_ll_status_ind ( T_LL_STATUS_IND *ll_status_ind );


EXTERN void mg_ll_establish_cnf ( T_LL_ESTABLISH_CNF *ll_establish_cnf );


EXTERN void mg_exp_re_est_timer (UBYTE sapi_index);


EXTERN void mg_ll_establish_ind ( T_LL_ESTABLISH_IND *ll_establish_ind );



EXTERN void mg_ll_release_cnf ( T_LL_RELEASE_CNF *ll_release_cnf );



EXTERN void mg_ll_release_ind ( T_LL_RELEASE_IND *ll_release_ind );



EXTERN void mg_ll_xid_cnf ( T_LL_XID_CNF *ll_xid_cnf );



EXTERN void mg_ll_xid_ind ( T_LL_XID_IND *ll_xid_ind );



#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_snsm_activate_ind ( T_SN_ACTIVATE_REQ *snsm_activate_ind );
#else
EXTERN void mg_snsm_activate_ind ( T_SNSM_ACTIVATE_IND *snsm_activate_ind );
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_snsm_deactivate_ind ( T_SN_DEACTIVATE_REQ *snsm_deactivate_ind );
#else
EXTERN void mg_snsm_deactivate_ind ( T_SNSM_DEACTIVATE_IND *snsm_deactivate_ind );
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_snsm_sequence_ind ( T_SN_SEQUENCE_REQ *snsm_sequence_ind );
#else
EXTERN void mg_snsm_sequence_ind ( T_SNSM_SEQUENCE_IND *snsm_sequence_ind );
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef SNDCP_UPM_INCLUDED
EXTERN void mg_snsm_modify_ind ( T_SN_MODIFY_REQ *snsm_modify_ind );
#else
EXTERN void mg_snsm_modify_ind ( T_SNSM_MODIFY_IND *snsm_modify_ind );
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

#ifdef TI_DUAL_MODE
EXTERN void mg_get_pending_pdu_req (T_SN_GET_PENDING_PDU_REQ *sn_get_pending_pdu_req);
#endif /*TI_DUAL_MODE*/

#endif /* !SNDCP_MGP_H */

