/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sus.c
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
|  Purpose :  This modul is part of the entity SNDCP and implements all
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (SU-statemachine)
+-----------------------------------------------------------------------------
*/

/*---- HISTORY --------------------------------------------------------------*/
#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"         /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include <string.h>    /* to get memcpy() */
#include "dti.h"
#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/
#include "sndcp_suf.h"     /* to get internal functions of service su */
#include "sndcp_cias.h"     /* to get signal functions to service cia */
#include "sndcp_nus.h"     /* to get signal functions to service nu */
#include "sndcp_sus.h"



/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : su_add_ll_unitdesc_req_params
+------------------------------------------------------------------------------
| Description : sets the parameters of the given ll_unitdata_req
|
| Parameters  : nsapi, ll_unitdata_req*
|
+------------------------------------------------------------------------------
*/
LOCAL void su_add_ll_unitdesc_req_params (UBYTE nsapi,
                                           T_LL_UNITDESC_REQ* ll_unitdesc_req)
{
  T_snsm_qos snsm_qos; /* Initialized with getter later */

  TRACE_FUNCTION( "su_add_ll_unitdesc_req_params" );

  /*
   * Set quality of service.
   */
  sndcp_get_nsapi_qos(nsapi, &snsm_qos);
  sndcp_snsm_qos_to_ll_qos(snsm_qos, &ll_unitdesc_req->ll_qos);
  /*
   * Set radio prio.
   */
  sndcp_get_nsapi_prio(nsapi,
                       &ll_unitdesc_req->radio_prio);
#ifdef REL99
  sndcp_get_nsapi_pktflowid(nsapi,
                            (U16*)&ll_unitdesc_req->pkt_flow_id);
#endif /*REL99*/


} /* su_add_ll_unitdesc_req_params() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : su_add_ll_unitdata_req_params
+------------------------------------------------------------------------------
| Description : sets the parameters of the given ll_unitdata_req
|
| Parameters  : nsapi, ll_unitdata_req*
|
+------------------------------------------------------------------------------
*/
LOCAL void su_add_ll_unitdata_req_params (UBYTE nsapi,
                                           T_LL_UNITDATA_REQ* ll_unitdata_req)
{
  T_snsm_qos snsm_qos; /* Initialized with getter later */

  TRACE_FUNCTION( "su_add_ll_unitdata_req_params" );

  /*
   * Set quality of service.
   */
  sndcp_get_nsapi_qos(nsapi, &snsm_qos);
  sndcp_snsm_qos_to_ll_qos(snsm_qos, &ll_unitdata_req->ll_qos);
  /*
   * Set radio prio.
   */
  sndcp_get_nsapi_prio(nsapi,
                       &ll_unitdata_req->radio_prio);
#ifdef REL99	
  sndcp_get_nsapi_pktflowid(nsapi,
                            &ll_unitdata_req->pkt_flow_id);
#endif /*REL99*/

} /* su_add_ll_unitdata_req_params() */
#endif /* _SNDCP_DTI_2_ */
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : su_add_sn_header
+------------------------------------------------------------------------------
| Description : sets the segment header in the given ll_unitdesc_req
|
| Parameters  : first and/or last segment , ll_unitdesc_req*, packet type
|
+------------------------------------------------------------------------------
*/
LOCAL void su_add_sn_header (UBYTE seg_pos,
                              T_pdu_ref pdu_ref,
                              T_LL_UNITDESC_REQ* ll_unitdesc_req,
                              UBYTE packet_type)
{
  USHORT offset = ENCODE_OFFSET_BYTE;
  BOOL compressed = FALSE;
  UBYTE sapi_index = 0;
  T_desc3* desc3 = NULL;
  U8* sndcp_header = NULL;

  TRACE_FUNCTION( "su_add_sn_header" );

  sndcp_get_sapi_index(ll_unitdesc_req->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  /* Get the descriptor describing the memory area with the sndcp header */
  desc3 = (T_desc3*)ll_unitdesc_req->desc_list3.first;
  sndcp_header = (U8*)desc3->buffer;
  sndcp_header[offset] = 0;
  /*
   * Octet 1, F bit.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0)
  {
    sndcp_header[offset] += (1 << 6);
  }
  /*
   * Octet 1, 1 for SN_UNITDATA.
   */
  sndcp_header[offset] += (1 << 5);
  /*
   * Octet 1, M bit.
   */
  if ((seg_pos & SEG_POS_LAST) == 0)
  {
    sndcp_header[offset] += (1 << 4);
  }
  /*
   * Octet 1, NSAPI number.
   */
  sndcp_header[offset] += pdu_ref.ref_nsapi;

  offset ++;
  sndcp_header[offset] = 0;

  if ((seg_pos & SEG_POS_FIRST) > 0) {
    /*
     * This is the first bit.
     * Find d/pcomp values and set them.
     */
    /*
     * Octet 2, dcomp
     */
    sndcp_header[offset] +=
      (UBYTE)(sndcp_data->mg.cur_xid_block[sapi_index].v42.dcomp << 4);
    /*
     * Octet 2, pcomp
     */
    sndcp_is_nsapi_header_compressed(pdu_ref.ref_nsapi, &compressed);
    if (compressed) {
      if (packet_type == TYPE_UNCOMPRESSED_TCP)
      {
        /*
         * Uncompressed TCP.
         */
        sndcp_header[offset] =
          sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp1;
      } else if (packet_type == TYPE_IP)
      {
        /*
         * IP.
         */
        sndcp_header[offset] = 0;
      } else
      {
        /*
         * Compressed TCP.
         */
        sndcp_header[offset] =
          sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp2;
      }
    }

    offset ++;
  }
  /*
   * Octet 2 or 3, segment number
   */
  sndcp_header[offset] = 0;
  sndcp_header[offset] += (UBYTE)(pdu_ref.ref_seg_num << 4);
  /*
   * Octet 2 or 3, N-PDU number, MSB.
   */
  sndcp_header[offset] += ((pdu_ref.ref_npdu_num >> 8) & 0xf);

  offset ++;

  /*
   * Octet 3 or 4, N-PDU number, LSB.
   */
  sndcp_header[offset] = (pdu_ref.ref_npdu_num & 0xff);

} /* su_add_sn_header() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : su_add_sn_header
+------------------------------------------------------------------------------
| Description : sets the segment header in the given ll_unitdata_req
|
| Parameters  : first and/or last segment , ll_unitdata_req*, packet type
|
+------------------------------------------------------------------------------
*/
LOCAL void su_add_sn_header (UBYTE seg_pos,
                              T_pdu_ref pdu_ref,
                              T_LL_UNITDATA_REQ* ll_unitdata_req,
                              UBYTE packet_type)
{
  USHORT offset = ll_unitdata_req->sdu.o_buf / 8;
  BOOL compressed = FALSE;
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "su_add_sn_header" );

  sndcp_get_sapi_index(ll_unitdata_req->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  ll_unitdata_req->sdu.buf[offset] = 0;
  /*
   * Octet 1, F bit.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0) {
    ll_unitdata_req->sdu.buf[offset] += (1 << 6);
  }
  /*
   * Octet 1, 1 for SN_UNITDATA.
   */
  ll_unitdata_req->sdu.buf[offset] += (1 << 5);
  /*
   * Octet 1, M bit.
   */
  if ((seg_pos & SEG_POS_LAST) == 0) {
    ll_unitdata_req->sdu.buf[offset] += (1 << 4);
  }
  /*
   * Octet 1, NSAPI number.
   */
  ll_unitdata_req->sdu.buf[offset] += pdu_ref.ref_nsapi;

  offset ++;
  ll_unitdata_req->sdu.buf[offset] = 0;

  if ((seg_pos & SEG_POS_FIRST) > 0) {
    /*
     * This is the first bit.
     * Find d/pcomp values and set them.
     */
    /*
     * Octet 2, dcomp
     */
    ll_unitdata_req->sdu.buf[offset] +=
      (UBYTE)(sndcp_data->su->cur_xid_block.v42.dcomp << 4);
    /*
     * Octet 2, pcomp
     */
    sndcp_is_nsapi_header_compressed(pdu_ref.ref_nsapi, &compressed);
    if (compressed) {
      if (packet_type == TYPE_UNCOMPRESSED_TCP) {
        /*
         * Uncompressed TCP.
         */
        ll_unitdata_req->sdu.buf[offset] =
          sndcp_data->su->cur_xid_block.vj.pcomp1;
      } else if (packet_type == TYPE_IP) {
        /*
         * IP.
         */
        ll_unitdata_req->sdu.buf[offset] = 0;
      } else {
        /*
         * Compressed TCP.
         */
        ll_unitdata_req->sdu.buf[offset] =
          sndcp_data->su->cur_xid_block.vj.pcomp2;
      }
    }

    offset ++;
  }
  /*
   * Octet 2 or 3, segment number
   */
  ll_unitdata_req->sdu.buf[offset] = 0;
  ll_unitdata_req->sdu.buf[offset] += (UBYTE)(pdu_ref.ref_seg_num << 4);
  /*
   * Octet 2 or 3, N-PDU number, MSB.
   */
  ll_unitdata_req->sdu.buf[offset] += ((pdu_ref.ref_npdu_num >> 8) & 0xf);

  offset ++;

  /*
   * Octet 3 or 4, N-PDU number, LSB.
   */
  ll_unitdata_req->sdu.buf[offset] = (pdu_ref.ref_npdu_num & 0xff);

} /* su_add_sn_header() */
#endif /* _SNDCP_DTI_2_ */


/*==== PUBLIC FUNCTIONS =====================================================*/
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sig_cia_su_cia_comp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_su_CIA_COMP_IND
|
| Parameters  : cia_comp_ind*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_su_cia_comp_ind (T_CIA_COMP_IND* cia_comp_ind)
{
  UBYTE sapi_index = 0;


  TRACE_ISIG( "sig_cia_su_cia_comp_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(cia_comp_ind->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      {

        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;
        UBYTE nsapi = cia_comp_ind->pdu_ref.ref_nsapi;
        UBYTE packet_type = cia_comp_ind->packet_type;

        PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);
        ll_unitdesc_req->sapi = cia_comp_ind->sapi;
        ll_unitdesc_req->tlli = cia_comp_ind->tlli;
        ll_unitdesc_req->ll_qos.delay      = cia_comp_ind->cia_qos.delay;
        ll_unitdesc_req->ll_qos.relclass   = cia_comp_ind->cia_qos.relclass;
        ll_unitdesc_req->ll_qos.peak       = cia_comp_ind->cia_qos.peak;
        ll_unitdesc_req->ll_qos.preced     = cia_comp_ind->cia_qos.preced;
        ll_unitdesc_req->ll_qos.mean       = cia_comp_ind->cia_qos.mean;
        /* attached_counter not copied */
        /* cipher not copied */
        ll_unitdesc_req->reserved_unitdata_req1.ref_nsapi    = cia_comp_ind->pdu_ref.ref_nsapi;
        ll_unitdesc_req->reserved_unitdata_req1.ref_npdu_num = cia_comp_ind->pdu_ref.ref_npdu_num;
        ll_unitdesc_req->reserved_unitdata_req1.ref_seg_num  = cia_comp_ind->pdu_ref.ref_seg_num;
        ll_unitdesc_req->seg_pos = cia_comp_ind->seg_pos;
        ll_unitdesc_req->desc_list3.list_len = cia_comp_ind->desc_list3.list_len;
        ll_unitdesc_req->desc_list3.first    = cia_comp_ind->desc_list3.first;

        su_add_ll_unitdesc_req_params(nsapi,
                                      ll_unitdesc_req);
        su_add_sn_header(seg_pos, pdu_ref,
                         ll_unitdesc_req,
                         packet_type);

        /* All information has been transferred remove the compression primitive */
        MFREE(cia_comp_ind);

        /*
         * Write the ll_unitdesc_req prim to ll_unitdesc_q,
         * increment write pointer.
         */
        sndcp_data->su->ll_unitdesc_q[sndcp_data->su->ll_unitdesc_q_write] =
          ll_unitdesc_req;
        sndcp_data->su->ll_unitdesc_q_write =
          (sndcp_data->su->ll_unitdesc_q_write + 1) % SNDCP_SEGMENT_NUMBERS_UNACK;

      }
      break;
    case SU_LLC_RECEPTIVE:
      {
        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE ll_unitdesc_sapi;
        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;
        UBYTE nsapi = cia_comp_ind->pdu_ref.ref_nsapi;
        UBYTE packet_type = cia_comp_ind->packet_type;

        PALLOC(ll_unitdesc_req, LL_UNITDESC_REQ);
        ll_unitdesc_req->sapi = cia_comp_ind->sapi;
        ll_unitdesc_req->tlli = cia_comp_ind->tlli;
        ll_unitdesc_req->ll_qos.delay      = cia_comp_ind->cia_qos.delay;
        ll_unitdesc_req->ll_qos.relclass   = cia_comp_ind->cia_qos.relclass;
        ll_unitdesc_req->ll_qos.peak       = cia_comp_ind->cia_qos.peak;
        ll_unitdesc_req->ll_qos.preced     = cia_comp_ind->cia_qos.preced;
        ll_unitdesc_req->ll_qos.mean       = cia_comp_ind->cia_qos.mean;
        /* attached_counter not copied */
        /* cipher not copied */
        ll_unitdesc_req->reserved_unitdata_req1.ref_nsapi    = cia_comp_ind->pdu_ref.ref_nsapi;
        ll_unitdesc_req->reserved_unitdata_req1.ref_npdu_num = cia_comp_ind->pdu_ref.ref_npdu_num;
        ll_unitdesc_req->reserved_unitdata_req1.ref_seg_num  = cia_comp_ind->pdu_ref.ref_seg_num;
        ll_unitdesc_req->seg_pos = cia_comp_ind->seg_pos;
        ll_unitdesc_req->desc_list3.list_len = cia_comp_ind->desc_list3.list_len;
        ll_unitdesc_req->desc_list3.first    = cia_comp_ind->desc_list3.first;

        /* All information has been transferred remove the compression primitive */
        MFREE(cia_comp_ind);

        su_add_ll_unitdesc_req_params(nsapi,
                                      ll_unitdesc_req);
        su_add_sn_header(seg_pos, pdu_ref,
                         ll_unitdesc_req,
                         packet_type);


#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
            ll_unitdesc_req->desc_list3.list_len);

        sndcp_trace_desc_list3_content(ll_unitdesc_req->desc_list3);
#endif

#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP,
                                 FLOW_TRACE_UP,
                                 FLOW_TRACE_BOTTOM,
                                 FALSE);
#endif
        ll_unitdesc_sapi = ll_unitdesc_req->sapi;


#ifdef _SIMULATION_
#ifndef MULTI_LAYER
        su_send_ll_unitdata_req_test(ll_unitdesc_req);
#else
        PSEND(hCommLLC, ll_unitdesc_req);
#endif /* MULTI_LAYER */
#else /* _SIMULATION_ */
        PSEND(hCommLLC, ll_unitdesc_req);
#endif /* _SIMULATION_ */

        SET_STATE(SU, SU_LLC_NOT_RECEPTIVE);
        /*
         * If segment is the last of the N-PDU then send ready_ind to nsapi
         * and get next entry from sn q..
         */
        if ((seg_pos & SEG_POS_LAST) > 0) {
          sndcp_data->su->cia_state = CIA_IDLE;
          su_next_sn_unitdata_req( ll_unitdesc_sapi );
          sig_su_nu_ready_ind(nsapi);
        }

      }
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SU_CIA_COMP_IND unexpected" );
      sndcp_cl_desc3_free((T_desc3*)cia_comp_ind->desc_list3.first);
      MFREE(cia_comp_ind);
      break;
  }
} /* sig_cia_su_cia_comp_ind() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_cia_su_cia_comp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_su_CIA_COMP_IND
|
| Parameters  : cia_comp_ind*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_su_cia_comp_ind (T_CIA_COMP_IND* cia_comp_ind)
{
  UBYTE sapi_index = 0;


  TRACE_ISIG( "sig_cia_su_cia_comp_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(cia_comp_ind->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      {

        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;
        UBYTE nsapi = cia_comp_ind->pdu_ref.ref_nsapi;
        UBYTE packet_type = cia_comp_ind->packet_type;

        PPASS(cia_comp_ind, ll_unitdata_req, LL_UNITDATA_REQ);

        su_add_ll_unitdata_req_params(nsapi,
                                      ll_unitdata_req);
        su_add_sn_header(seg_pos, pdu_ref,
                         ll_unitdata_req,
                         packet_type);
        /*
         * Write the ll_unitdata_req prim to ll_unitdata_q,
         * increment write pointer.
         */
        sndcp_data->su->ll_unitdata_q[sndcp_data->su->ll_unitdata_q_write] =
          ll_unitdata_req;
        sndcp_data->su->ll_unitdata_q_write =
          (sndcp_data->su->ll_unitdata_q_write + 1) % SNDCP_SEGMENT_NUMBERS_UNACK;

      }
      break;
    case SU_LLC_RECEPTIVE:
      {
        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE ll_unitdata_sapi;
        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;
        UBYTE nsapi = cia_comp_ind->pdu_ref.ref_nsapi;
        UBYTE packet_type = cia_comp_ind->packet_type;

        PPASS(cia_comp_ind, ll_unitdata_req, LL_UNITDATA_REQ);

        su_add_ll_unitdata_req_params(nsapi,
                                      ll_unitdata_req);
        su_add_sn_header(seg_pos, pdu_ref,
                         ll_unitdata_req,
                         packet_type);


#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
                       (ll_unitdata_req->sdu.l_buf + 7) / 8);

        sndcp_trace_sdu(& ll_unitdata_req->sdu);
#endif

#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP,
                                 FLOW_TRACE_UP,
                                 FLOW_TRACE_BOTTOM,
                                 FALSE);
#endif
        ll_unitdata_sapi = ll_unitdata_req->sapi;

        PSEND(hCommLLC, ll_unitdata_req);

        SET_STATE(SU, SU_LLC_NOT_RECEPTIVE);
        /*
         * If segment is the last of the N-PDU then send ready_ind to nsapi
         * and get next entry from sn q..
         */
        if ((seg_pos & SEG_POS_LAST) > 0) {
          sndcp_data->su->cia_state = CIA_IDLE;
          su_next_sn_unitdata_req( ll_unitdata_sapi );
          sig_su_nu_ready_ind(nsapi);
        }

      }
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SU_CIA_COMP_IND unexpected" );
      break;
  }
} /* sig_cia_su_cia_comp_ind() */
#endif /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_delete_pdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_DELETE_PDUS
|               The same as the one in service sua, but everything is in the
|               unacknowledged version here.
|
| Parameters  : UBYTE nsapi the affected nsapi, UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_delete_pdus (UBYTE nsapi, UBYTE sapi)
{
  UBYTE sapi_index = 0;
  BOOL ll_q_affected = FALSE;

  TRACE_ISIG( "sig_mg_su_delete_pdus" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];
  sndcp_data->su->cia_state = CIA_IDLE;

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_RECEPTIVE:
      {
        /*
         * Delete affected N-PDUS from sn_unitdata_q.
         */
        BOOL still = TRUE;
        UBYTE index = sndcp_data->su->sn_unitdata_q_read;

        while (still) {
          if (index == sndcp_data->su->sn_unitdata_q_write) {
            break;
          }
          if (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            UBYTE i = 0;

            if (sndcp_data->su->sn_unitdata_q[index] != NULL) {
#ifdef _SNDCP_DTI_2_
              MFREE_PRIM(sndcp_data->su->sn_unitdata_q[index]);
#else /*_SNDCP_DTI_2_*/ 
              PFREE_DESC(sndcp_data->su->sn_unitdata_q[index]);
#endif /*_SNDCP_DTI_2_*/
              sndcp_data->su->sn_unitdata_q[index] = NULL;
            }

            for (i = index;
                 i != sndcp_data->su->sn_unitdata_q_write;
                 i = (i + 1) % SN_UNITDATA_Q_LEN) {


              sndcp_data->su->sn_unitdata_q[i] =
                sndcp_data->su->sn_unitdata_q[(i + 1) % SN_UNITDATA_Q_LEN];

              sndcp_data->su->sn_unitdata_q[(i + 1) % SN_UNITDATA_Q_LEN] =
                NULL;

            }
            sndcp_data->su->sn_unitdata_q_write =
              (sndcp_data->su->sn_unitdata_q_write - 1
               + SN_UNITDATA_Q_LEN) % SN_UNITDATA_Q_LEN;
          } else {
            index = (index + 1) % SN_UNITDATA_Q_LEN;
          } /* else (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) */

        }
#ifdef _SNDCP_DTI_2_
        /*
         * Delete affected SN-PDUS from ll_unitdata_q.
         */
        still = TRUE;
        index = sndcp_data->su->ll_unitdesc_q_read;
        while (still) {
          if (index == sndcp_data->su->ll_unitdesc_q_write) {
            break;
          }
          if (su_get_nsapi(sndcp_data->su->ll_unitdesc_q[index]) == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            UBYTE i = 0;

            ll_q_affected = TRUE;
            /*
             * Free the deleted primitive.
             */
            if (sndcp_data->su->ll_unitdesc_q[index] != NULL) {
              T_desc3 *free_help, *help =
             (T_desc3 *)sndcp_data->su->ll_unitdesc_q[index]->desc_list3.first;
              while(help != NULL)
              {
                 sndcp_cl_desc3_free(help);
                 free_help = help;
                 help = (T_desc3 *)free_help->next;
                 MFREE(free_help);
              }
              sndcp_data->su->ll_unitdesc_q[index]->desc_list3.first = NULL;
              PFREE(sndcp_data->su->ll_unitdesc_q[index]);
              sndcp_data->su->ll_unitdesc_q[index] = NULL;
            }

            for (i = index;
                 i != sndcp_data->su->ll_unitdesc_q_write;
                 i = (i + 1) % SNDCP_SEGMENT_NUMBERS_UNACK) {

              sndcp_data->su->ll_unitdesc_q[i] =
                sndcp_data->su->ll_unitdesc_q
                  [(i + 1) % SNDCP_SEGMENT_NUMBERS_UNACK];
            }
            sndcp_data->su->ll_unitdesc_q_write =
              (sndcp_data->su->ll_unitdesc_q_write - 1
               + SNDCP_SEGMENT_NUMBERS_UNACK) % SNDCP_SEGMENT_NUMBERS_UNACK;

          } else {
            index = (index + 1) % SNDCP_SEGMENT_NUMBERS_UNACK;
          } /* else (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) { */


        }
#else /* _SNDCP_DTI_2_ */
        /*
         * Delete affected SN-PDUS from ll_unitdata_q.
         */
        still = TRUE;
        index = sndcp_data->su->ll_unitdata_q_read;
        while (still) {
          if (index == sndcp_data->su->ll_unitdata_q_write) {
            break;
          }
          if (su_get_nsapi(sndcp_data->su->ll_unitdata_q[index]) == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            UBYTE i = 0;

            ll_q_affected = TRUE;
            /*
             * Free the deleted primitive.
             */
            if (sndcp_data->su->ll_unitdata_q[index] != NULL) {
              PFREE(sndcp_data->su->ll_unitdata_q[index]);
              sndcp_data->su->ll_unitdata_q[index] = NULL;
            }

            for (i = index;
                 i != sndcp_data->su->ll_unitdata_q_write;
                 i = (i + 1) % SNDCP_SEGMENT_NUMBERS_UNACK) {

              sndcp_data->su->ll_unitdata_q[i] =
                sndcp_data->su->ll_unitdata_q
                  [(i + 1) % SNDCP_SEGMENT_NUMBERS_UNACK];
            }
            sndcp_data->su->ll_unitdata_q_write =
              (sndcp_data->su->ll_unitdata_q_write - 1
               + SNDCP_SEGMENT_NUMBERS_UNACK) % SNDCP_SEGMENT_NUMBERS_UNACK;

          } else {
            index = (index + 1) % SNDCP_SEGMENT_NUMBERS_UNACK;
          } /* else (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) { */


        }
#endif /* _SNDCP_DTI_2_ */
      }
      /*
       * if the segments in LL queue were from the affected nsapi then the
       * next N-PDU is directed to cia.
       */
      if (ll_q_affected) {
        su_next_sn_unitdata_req (sapi);
      }
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_DELETE_PDUS unexpected" );
      break;
  }
} /* sig_mg_su_delete_pdus() */



/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_n201
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_N201
|
| Parameters  : UBYTE sapi the affected sapi, USHORT n201
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_n201 (UBYTE sapi,
                            USHORT n201)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_su_n201" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_RECEPTIVE:
      sndcp_data->su->n201_u = n201;
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_N201 unexpected" );
      break;
  }
} /* sig_mg_su_n201() */



/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_SUSPEND
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_suspend (UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_su_suspend" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      break;
    case SU_LLC_RECEPTIVE_SUSPEND:
      break;
    case SU_LLC_NOT_RECEPTIVE:
      SET_STATE (SU, SU_LLC_NOT_RECEPTIVE_SUSPEND);
      break;
    case SU_LLC_RECEPTIVE:
      SET_STATE (SU, SU_LLC_RECEPTIVE_SUSPEND);
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_SUSPEND unexpected" );
      break;
  }
} /* sig_mg_su_suspend() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_RESET_IND
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_reset_ind (UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_su_reset_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_su_reset_ind() */

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_RESUME
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_resume (UBYTE sapi)
{
  UBYTE sapi_index = 0;


  TRACE_ISIG( "sig_mg_su_resume" );
  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      SET_STATE (SU, SU_LLC_NOT_RECEPTIVE);
      break;
    case SU_LLC_RECEPTIVE_SUSPEND:
      /*
       * Is queue with LL_UNITDESC_REQ empty?
       */
      if (sndcp_data->su->ll_unitdesc_q_write ==
          sndcp_data->su->ll_unitdesc_q_read) {
        SET_STATE(SU, SU_LLC_RECEPTIVE);
      } else {
        /*
         * Get next segment from queue and send it.
         */
        U8 ll_unitdesc_seg_pos;
        U8 ll_unitdesc_nsapi;
        U8 ll_unitdesc_sapi;

        T_LL_UNITDESC_REQ* ll_unitdesc_req =
          sndcp_data->su->ll_unitdesc_q[sndcp_data->su->ll_unitdesc_q_read];

        sndcp_data->su->ll_unitdesc_q[sndcp_data->su->ll_unitdesc_q_read] =
          NULL;

        sndcp_data->su->ll_unitdesc_q_read =
            (sndcp_data->su->ll_unitdesc_q_read + 1) %
              SNDCP_SEGMENT_NUMBERS_UNACK;


        /*
         * trace output
         */
#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
                       ll_unitdesc_req->desc_list3.list_len);

        sndcp_trace_desc_list3_content(ll_unitdesc_req->desc_list3);
#endif
#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP,
                                 FLOW_TRACE_UP,
                                 FLOW_TRACE_BOTTOM,
                                 FALSE);
#endif
        ll_unitdesc_seg_pos = ll_unitdesc_req->seg_pos;
        ll_unitdesc_nsapi   = su_get_nsapi(ll_unitdesc_req);
        ll_unitdesc_sapi    = ll_unitdesc_req->sapi;

#ifdef _SIMULATION_
        su_send_ll_unitdata_req_test(ll_unitdesc_req);
#else /* _SIMULATION_ */
        PSEND(hCommLLC, ll_unitdesc_req);
#endif /* _SIMULATION_ */

        SET_STATE(SU, SU_LLC_NOT_RECEPTIVE);
        /*
         * If segment is the last of the N-PDU then send ready_ind to nsapi
         * and get next entry from sn q..
         */
        if ((ll_unitdesc_seg_pos & SEG_POS_LAST) > 0) {
          sndcp_data->su->cia_state = CIA_IDLE;
          su_next_sn_unitdata_req( ll_unitdesc_sapi );
          sig_su_nu_ready_ind( ll_unitdesc_nsapi );
        }
      }

      break;
    case SU_LLC_NOT_RECEPTIVE:
      break;
    case SU_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_RESUME unexpected" );
      break;
  }
} /* sig_mg_su_resume() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_su_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_RESUME
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_su_resume (UBYTE sapi)
{
  UBYTE sapi_index = 0;


  TRACE_ISIG( "sig_mg_su_resume" );
  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      SET_STATE (SU, SU_LLC_NOT_RECEPTIVE);
      break;
    case SU_LLC_RECEPTIVE_SUSPEND:
      /*
       * Is queue with LL_UNITDATA_REQ empty?
       */
      if (sndcp_data->su->ll_unitdata_q_write ==
          sndcp_data->su->ll_unitdata_q_read) {
        SET_STATE(SU, SU_LLC_RECEPTIVE);
      } else {
        /*
         * Get next segment from queue and send it.
         */
        UBYTE ll_unitdata_seg_pos;
        UBYTE ll_unitdata_nsapi;
        UBYTE ll_unitdata_sapi;

        T_LL_UNITDATA_REQ* ll_unitdata_req =
          sndcp_data->su->ll_unitdata_q[sndcp_data->su->ll_unitdata_q_read];

        sndcp_data->su->ll_unitdata_q[sndcp_data->su->ll_unitdata_q_read] =
          NULL;

        sndcp_data->su->ll_unitdata_q_read =
            (sndcp_data->su->ll_unitdata_q_read + 1) %
              SNDCP_SEGMENT_NUMBERS_UNACK;


        /*
         * trace output
         */
#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
                       (ll_unitdata_req->sdu.l_buf + 7) / 8);
        sndcp_trace_sdu(& ll_unitdata_req->sdu);


#endif
#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP,
                                 FLOW_TRACE_UP,
                                 FLOW_TRACE_BOTTOM,
                                 FALSE);
#endif
        ll_unitdata_seg_pos = ll_unitdata_req->seg_pos;
        ll_unitdata_nsapi   = su_get_nsapi(ll_unitdata_req);
        ll_unitdata_sapi    = ll_unitdata_req->sapi;

        PSEND(hCommLLC, ll_unitdata_req);

        SET_STATE(SU, SU_LLC_NOT_RECEPTIVE);
        /*
         * If segment is the last of the N-PDU then send ready_ind to nsapi
         * and get next entry from sn q..
         */
        if ((ll_unitdata_seg_pos & SEG_POS_LAST) > 0) {
          sndcp_data->su->cia_state = CIA_IDLE;
          su_next_sn_unitdata_req( ll_unitdata_sapi );
          sig_su_nu_ready_ind( ll_unitdata_nsapi );
        }
      }

      break;
    case SU_LLC_NOT_RECEPTIVE:
      break;
    case SU_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_SU_RESUME unexpected" );
      break;
  }
} /* sig_mg_su_resume() */
#endif /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_nu_su_unitdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_NU_SU_UNITDATA_REQ
|
| Parameters  : T_SN_UNITDATA_REQ ,  N-PDU to be sent to sapiup in form of
|                                    prim struct
|                                    USHORT,  the send N-PDU number of this N-PDU
|                                    UBYTE, the number of the NSAPI who sent the N-PDU
|                                    UBYTE), the SAPI that is supposed to compute the N-PDU
|
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_nu_su_unitdata_req (T_SN_UNITDATA_REQ* sn_unitdata_req,
                                    USHORT npdu_number,
                                    UBYTE nsapi,
                                    UBYTE sapi
                                    )
{
  UBYTE sapi_index = 0;

#ifdef _SNDCP_MEAN_TRACE_
#ifdef _SNDCP_DTI_2_
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_UP,
                   SNDCP_MEAN_UNACK,
                   sn_unitdata_req->desc_list2.list_len);
#else /*_SNDCP_DTI_2_*/
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_UP,
                   SNDCP_MEAN_UNACK,
                   sn_unitdata_req->desc_list.list_len);
#endif /*_SNDCP_DTI_2_*/
#endif /* _SNDCP_MEAN_TRACE_ */

  TRACE_ISIG( "sig_nu_su_unitdata_req" );


  /*
   * set service instance according to sapi in signal
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
    case SU_LLC_NOT_RECEPTIVE:
    case SU_LLC_RECEPTIVE:
      /*
       * If queue with NPDUs is empty, and cia is not busy,
       * send the NPDU to cia, else
       * add it to the queue.
       */
      if ((sndcp_data->su->sn_unitdata_q_read ==
           sndcp_data->su->sn_unitdata_q_write) &&
          (sndcp_data->su->ll_unitdesc_q_read ==
           sndcp_data->su->ll_unitdesc_q_write) &&
          sndcp_data->su->cia_state == CIA_IDLE) {
        /*
         * Send the NPDU to cia.
         */
        sndcp_data->su->cia_state = CIA_BUSY;
        sig_su_cia_cia_comp_req(sn_unitdata_req,
                            npdu_number,
                            nsapi,
                            sapi);
      } else { /* Queue with NPDUs is not empty or cia is busy. */
        /*
         * Write the sn_unitdata_req prim to sn_unitdata_q,
         * increment write pointer.
         */
        sndcp_data->su->sn_unitdata_q[sndcp_data->su->sn_unitdata_q_write] =
          sn_unitdata_req;
        sndcp_data->su->npdu_number_q[sndcp_data->su->sn_unitdata_q_write] =
          npdu_number;
        sndcp_data->su->sn_unitdata_q_write =
          (sndcp_data->su->sn_unitdata_q_write + 1) % SN_UNITDATA_Q_LEN;
      } /* Queue with NPDUs is not empty or cia is busy. */
      break;
    default:
      TRACE_ERROR( "SIG_NU_SU_UNITDATA_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_unitdata_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_unitdata_req);
#endif /*_SNDCP_DTI_2_*/
      break;
  }
} /* sig_nu_su_unitdata_req() */

#ifdef _SIMULATION_
#ifdef  _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : su_send_ll_unitdata_req_test
+------------------------------------------------------------------------------
| Description : Only for test purposes.
|               The given LL_UNITDESC_REQ is copied to an
|               LL_UNITDATA_REQ and sent for the simulated test environment.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
void su_send_ll_unitdata_req_test(T_LL_UNITDESC_REQ* ll_unitdesc_req)
{
  T_desc3* descriptor = NULL;
  U8* p_data = NULL;
  U16 offset = ENCODE_OFFSET_BYTE;

  PALLOC_SDU(ll_unitdata_req, LL_UNITDATA_REQ, (U16)( ll_unitdesc_req->desc_list3.list_len* 8));

  /*
   * Set parameters in ll_unitdata_req primitive
   */
  ll_unitdata_req->sapi = ll_unitdesc_req->sapi;
  ll_unitdata_req->tlli = ll_unitdesc_req->tlli;
  ll_unitdata_req->ll_qos = ll_unitdesc_req->ll_qos;
  ll_unitdata_req->radio_prio = ll_unitdesc_req->radio_prio;
  ll_unitdata_req->cipher = ll_unitdesc_req->cipher;
  ll_unitdata_req->reserved_unitdata_req1 = ll_unitdesc_req->reserved_unitdata_req1;
  ll_unitdata_req->seg_pos = ll_unitdesc_req->seg_pos;
  ll_unitdata_req->attached_counter = ll_unitdesc_req->attached_counter;
  ll_unitdata_req->reserved_unitdata_req4 = ll_unitdesc_req->reserved_unitdata_req4;
#ifdef REL99
  ll_unitdata_req->pkt_flow_id = ll_unitdesc_req->pkt_flow_id;
#endif /*REL99*/
  /*
   * Set the ENCODE_OFFSET in the uplink sdu, set sdu.l_buf
   */
  ll_unitdata_req->sdu.o_buf = ENCODE_OFFSET;
  ll_unitdata_req->sdu.l_buf = ll_unitdesc_req->desc_list3.list_len* 8;
  /*
   * Copy the data.
   */
  descriptor = (T_desc3*)ll_unitdesc_req->desc_list3.first;
  while (descriptor != NULL)
  {
    T_desc3* help = descriptor;
    p_data = (U8*)descriptor->buffer;
    if (descriptor->len>0)
    {
      memcpy(&ll_unitdata_req->sdu.buf[offset],
             &p_data[descriptor->offset],
             descriptor->len);
    }
    offset += descriptor->len;
    descriptor = (T_desc3*)descriptor->next;
    sndcp_cl_desc3_free(help);
    MFREE(help);
    help = NULL;
  }

  PSEND(hCommLLC, ll_unitdata_req);

  PFREE(ll_unitdesc_req); // MAYBE REMEMBER is this the correct free?

} /* su_send_ll_unitdata_req_test */

#endif /* _SNDCP_DTI_2_ */
#endif /* _SIMULATION_ */


