/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_f.h
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
|  Purpose :  Definitions for sndcp_f.c
+-----------------------------------------------------------------------------
*/

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_f.h                                $
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

#ifndef SNDCP_F_H
#define SNDCP_F_H


#define NAS_DCOMP_BOTH_DIRECT 3
EXTERN void sndcp_get_nsapi_state(UBYTE nsapi,
                                  USHORT* stat);

#ifdef _SNDCP_DTI_2_
EXTERN void sndcp_get_nsapi_direction(UBYTE nsapi,
                                      U8* direction);
#else /*_SNDCP_DTI_2_*/
EXTERN void sndcp_get_nsapi_direction(UBYTE nsapi,
                                      BOOL* direction);
#endif /*_SNDCP_DTI_2_*/

EXTERN void sndcp_get_nsapi_interface(UBYTE nsapi,
                                      U8* interfac);

EXTERN void sndcp_get_nsapi_linkid(UBYTE nsapi,
                                   U32* linkid);

EXTERN void sndcp_get_nsapi_neighbor(UBYTE nsapi,
                                     U8** neighbor);


EXTERN void sndcp_get_sapi_index(UBYTE sapi,
                                 UBYTE* index);

EXTERN void sndcp_is_nsapi_data_compressed(UBYTE nsapi,
                                           BOOL* compressed);

EXTERN void sndcp_is_nsapi_header_compressed(UBYTE nsapi,
                                             BOOL* compressed);

EXTERN void sndcp_mean_trace(UBYTE nsapi, UBYTE direction, UBYTE ack_mode, USHORT len);

EXTERN void sndcp_reset_xid_block(T_XID_BLOCK* xid_block);


#ifdef _SIMULATION_
#ifdef _SNDCP_DTI_2_
EXTERN void sndcp_sdu_to_desc_list(T_sdu*, T_desc_list2*);
EXTERN void su_send_ll_unitdata_req_test(T_LL_UNITDESC_REQ* ll_unitdesc_req);
EXTERN void su_send_ll_data_req_test(T_LL_DESC_REQ* ll_desc_req);
#else /*_SNDCP_DTI_2_*/
EXTERN void sndcp_sdu_to_desc_list(T_sdu*, T_desc_list*);
#endif /*_SNDCP_DTI_2_*/
#endif /* _SIMULATION_ */


EXTERN void sndcp_set_nsapi_direction(UBYTE nsapi,
                                      BOOL direction);

EXTERN void sndcp_set_nsapi_interface(UBYTE nsapi,
                                      U8 interfac);


EXTERN void sndcp_set_nsapi_linkid(UBYTE nsapi,
                                   U32 linkid);

EXTERN void sndcp_set_nsapi_neighbor(UBYTE nsapi,
                                     U8* neighbor);

EXTERN void sndcp_set_nsapi_state(UBYTE nsapi,
                                  USHORT stat);

EXTERN void sndcp_get_sapi_state(UBYTE sapi,
                                 USHORT* state);

EXTERN void sndcp_set_sapi_state(UBYTE sapi,
                                 USHORT stat);

EXTERN void sndcp_get_nsapi_ack(UBYTE nsapi,
                                BOOL* b);

EXTERN void sndcp_set_nsapi_ack(UBYTE nsapi,
                                BOOL b);

EXTERN void sndcp_get_sapi_ack(UBYTE sapi,
                               BOOL* b);

EXTERN void sndcp_set_sapi_ack(UBYTE sapi,
                               BOOL b);

EXTERN void sndcp_get_nsapi_prio(UBYTE nsapi,
                                 UBYTE* prio);
#ifdef REL99 
EXTERN void sndcp_get_nsapi_pktflowid(U8 nsapi,  U16* pkt_flow_id);
EXTERN void sndcp_set_nsapi_pktflowid(UBYTE nsapi, 
                                      U16 packet_flow_identifier);
#endif /*REL99*/

EXTERN void sndcp_no_xid(void);

EXTERN void sndcp_set_nsapi_prio(UBYTE nsapi,
                                 UBYTE prio);

EXTERN void sndcp_get_nsapi_sapi(UBYTE nsapi,
                                 UBYTE* sapi);

EXTERN void sndcp_set_nsapi_sapi(UBYTE nsapi,
                                 UBYTE sapi);

EXTERN void sndcp_is_nsapi_used(UBYTE nsapi,
                                BOOL* b);

EXTERN void sndcp_set_nsapi_used(UBYTE nsapi,
                                 BOOL sapi);

EXTERN void sndcp_get_nsapi_qos(UBYTE nsapi,
                                T_snsm_qos* qos);

EXTERN void sndcp_set_nsapi_qos(UBYTE nsapi,
                                T_snsm_qos qos);

EXTERN void sndcp_snsm_qos_to_ll_qos(T_snsm_qos snsm_qos,
                                     T_ll_qos* ll_qos);
#ifdef _SNDCP_DTI_2_
EXTERN void sndcp_sig_callback(U8 instance,
                               U8 interfac,
                               U8 channel,
                               U8 reason,
                               T_DTI2_DATA_IND *dti_data_ind);

EXTERN void sndcp_cl_desc2_attach(T_desc2* p_desc2);
EXTERN void sndcp_cl_desc3_free(T_desc3* p_desc3);
#else
EXTERN void sndcp_sig_callback(U8 instance,
                               U8 interfac,
                               U8 channel,
                               U8 reason,
                               T_DTI_DATA_IND *dti_data_ind);
#endif /*_SNDCP_DTI_2_*/

#ifdef SNDCP_TRACE_ALL

EXTERN void sndcp_trace_sdu(T_sdu* sdu);

#ifdef _SNDCP_DTI_2_
EXTERN void sndcp_trace_desc_list3_content(T_desc_list3 desc_list3);
#endif /*_SNDCP_DTI_2_*/

#endif /* SNDCP_TRACE_ALL */

#ifdef SNDCP_TRACE_BUFFER
#ifdef _SNDCP_DTI_2_
EXTERN void sndcp_trace_desc_list(T_desc_list2* desc_list);
#else /*_SNDCP_DTI_2_*/
EXTERN void sndcp_trace_desc_list(T_desc_list* desc_list);
#endif /*_SNDCP_DTI_2_*/
#endif /* SNDCP_TRACE_BUFFER */

#ifdef SNDCP_TRACE_IP_DATAGRAM

#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_trace_ip_datagram(T_desc_list2* desc_list);
GLOBAL void sndcp_bin_trace_ip(T_desc_list2* desc_list, U8 direction);
GLOBAL void sndcp_default_ip_trace(T_desc_list2* desc_list, U8 direction);
#else /*_SNDCP_DTI_2_*/
GLOBAL void sndcp_trace_ip_datagram(T_desc_list* desc_list);
GLOBAL void sndcp_bin_trace_ip(T_desc_list* desc_list, U8 direction);
GLOBAL void sndcp_default_ip_trace(T_desc_list* desc_list, U8 direction);
#endif /*_SNDCP_DTI_2_*/

#endif /* SNDCP_TRACE_IP_DATAGRAM */


EXTERN void sndcp_unset_nsapi_state(UBYTE nsapi,
                                  USHORT stat);

EXTERN void sndcp_unset_sapi_state(UBYTE nsapi,
                                  USHORT stat);

EXTERN void sndcp_sn_count_req(T_SN_COUNT_REQ * sn_count_req);

EXTERN USHORT sndcp_swap2(USHORT n);

EXTERN ULONG sndcp_swap4(ULONG n);

EXTERN U8 sndcp_get_nsapi_rec_state (U8 nsapi);

EXTERN void sndcp_set_nsapi_rec_state (U8 nsapi, U8 state);

EXTERN void sndcp_set_unack_transfer_params (T_LL_UNITDATA_IND* ll_unitdata_ind);

EXTERN void sndcp_set_ack_transfer_params (T_LL_DATA_IND* ll_data_ind);

#endif /* !SNDCP_F_H */
