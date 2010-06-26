/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_suas.c
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
|             described in the SDL-documentation (SUA-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include <string.h>    /* to get memcpy() */
#include "dti.h"
#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/
#include "sndcp_suaf.h"     /* to get internal functions of service sua */
#include "sndcp_nus.h"     /* to get signal functions to service nu */
#include "sndcp_cias.h"     /* to get signal functions to service cia */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sua_add_ll_desc_req_params
+------------------------------------------------------------------------------
| Description : sets the parameters of the given ll_desc_req
|
| Parameters  : pdu_ref, ll_desc_req*
|
+------------------------------------------------------------------------------
*/
LOCAL void sua_add_ll_desc_req_params (T_pdu_ref pdu_ref,
                                       T_LL_DESC_REQ* ll_desc_req)
{
  T_snsm_qos snsm_qos; /* initialized later with getter */

  TRACE_FUNCTION( "sua_add_ll_desc_req_params" );

  /*
   * Set quality of service.
   */
  sndcp_get_nsapi_qos(pdu_ref.ref_nsapi, &snsm_qos);
  sndcp_snsm_qos_to_ll_qos(snsm_qos, &ll_desc_req->ll_qos);
  /*
   * Set radio prio.
   */
  sndcp_get_nsapi_prio(pdu_ref.ref_nsapi,
                       &ll_desc_req->radio_prio);
#ifdef REL99
  sndcp_get_nsapi_pktflowid(pdu_ref.ref_nsapi,
                            (U16 *)&ll_desc_req->pkt_flow_id);
#endif /*REL99*/
  /*
   * Set pdu_ref.
   */
  ll_desc_req->reference1.ref_nsapi = pdu_ref.ref_nsapi;
  ll_desc_req->reference1.ref_npdu_num = (UBYTE)pdu_ref.ref_npdu_num;
  ll_desc_req->reference1.ref_seg_num = pdu_ref.ref_seg_num;

} /* sua_add_ll_desc_req_params() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sua_add_ll_data_req_params
+------------------------------------------------------------------------------
| Description : sets the parameters of the given ll_data_req
|
| Parameters  : pdu_ref, ll_data_req*
|
+------------------------------------------------------------------------------
*/
LOCAL void sua_add_ll_data_req_params (T_pdu_ref pdu_ref,
                                       T_LL_DATA_REQ* ll_data_req)
{
  T_snsm_qos snsm_qos; /* initialized later with getter */

  TRACE_FUNCTION( "sua_add_ll_data_req_params" );

  /*
   * Set quality of service.
   */
  sndcp_get_nsapi_qos(pdu_ref.ref_nsapi, &snsm_qos);
  sndcp_snsm_qos_to_ll_qos(snsm_qos, &ll_data_req->ll_qos);
  /*
   * Set radio prio.
   */
  sndcp_get_nsapi_prio(pdu_ref.ref_nsapi,
                       &ll_data_req->radio_prio);
#ifdef REL99
  sndcp_get_nsapi_pktflowid(pdu_ref.ref_nsapi,
                            &ll_data_req->pkt_flow_id);
#endif /*REL99*/

  /*
   * Set pdu_ref.
   */
  ll_data_req->reference1.ref_nsapi = pdu_ref.ref_nsapi;
  ll_data_req->reference1.ref_npdu_num = (UBYTE)pdu_ref.ref_npdu_num;
  ll_data_req->reference1.ref_seg_num = pdu_ref.ref_seg_num;

} /* sua_add_ll_data_req_params() */
#endif /* _SNDCP_DTI_2_ */
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sua_add_sn_header
+------------------------------------------------------------------------------
| Description : sets the segment header in the given ll_desc_req
|
| Parameters  : first and/or last segment , ll_desc_req*, packet type.
|
+------------------------------------------------------------------------------
*/
LOCAL void sua_add_sn_header (U8 seg_pos,
                              T_pdu_ref pdu_ref,
                              T_LL_DESC_REQ* ll_desc_req,
                              U8 packet_type)
{
  USHORT offset = ENCODE_OFFSET_BYTE;
  BOOL compressed = FALSE;
  T_desc3* desc3 = NULL;
  U8* sndcp_header = NULL;
  U8 sapi_index = 0;

  TRACE_FUNCTION( "sua_add_sn_header" );

  /* Get the descriptor describing the memory area with the sndcp header */
  desc3 = (T_desc3*)ll_desc_req->desc_list3.first;
  sndcp_header = (U8*)desc3->buffer;
  sndcp_header[offset] = 0;

  sndcp_get_sapi_index(ll_desc_req->sapi, &sapi_index); 
  /*
   * Octet 1, F bit.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0)
  {
    sndcp_header[offset] += (1 << 6);
  }
  /*
   * Octet 1, 0 for SN_DATA, i.e. nothing.
   */

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
  if ((seg_pos & SEG_POS_FIRST) > 0)
  {
    sndcp_header[offset] = 0;
    /*
     * This is the first bit.
     * Find d/pcomp values and set them.
     */
    /*
     * Octet 2, dcomp
     */
    sndcp_header[offset] +=
      (U8)(sndcp_data->mg.cur_xid_block[sapi_index].v42.dcomp << 4);
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
      }
      else if (packet_type == TYPE_IP)
      {
        /*
         * IP.
         */
        sndcp_header[offset] = 0;
      }
      else
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
   * Octet 3, N-PDU number, if F bit set to 1.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0)
  {
    sndcp_header[offset] = (U8)pdu_ref.ref_npdu_num;
  }


} /* sua_add_sn_header() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sua_add_sn_header
+------------------------------------------------------------------------------
| Description : sets the segment header in the given ll_data_req
|
| Parameters  : first and/or last segment , ll_data_req*, packet type.
|
+------------------------------------------------------------------------------
*/
LOCAL void sua_add_sn_header (UBYTE seg_pos,
                              T_pdu_ref pdu_ref,
                              T_LL_DATA_REQ* ll_data_req,
                              UBYTE packet_type)
{
  USHORT offset = ll_data_req->sdu.o_buf / 8;
  BOOL compressed = FALSE;

  TRACE_FUNCTION( "sua_add_sn_header" );

  ll_data_req->sdu.buf[offset] = 0;
  /*
   * Octet 1, F bit.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0) {
    ll_data_req->sdu.buf[offset] += (1 << 6);
  }
  /*
   * Octet 1, 0 for SN_DATA, i.e. nothing.
   */

  /*
   * Octet 1, M bit.
   */
  if ((seg_pos & SEG_POS_LAST) == 0) {
    ll_data_req->sdu.buf[offset] += (1 << 4);
  }
  /*
   * Octet 1, NSAPI number.
   */
  ll_data_req->sdu.buf[offset] += pdu_ref.ref_nsapi;

  offset ++;
  if ((seg_pos & SEG_POS_FIRST) > 0) {
    ll_data_req->sdu.buf[offset] = 0;
    /*
     * This is the first bit.
     * Find d/pcomp values and set them.
     */
    /*
     * Octet 2, dcomp
     */
    ll_data_req->sdu.buf[offset] +=
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
        ll_data_req->sdu.buf[offset] =
          sndcp_data->sua->cur_xid_block.vj.pcomp1;
      } else if (packet_type == TYPE_IP) {
        /*
         * IP.
         */
        ll_data_req->sdu.buf[offset] = 0;
      } else {
        /*
         * Compressed TCP.
         */
        ll_data_req->sdu.buf[offset] =
          sndcp_data->sua->cur_xid_block.vj.pcomp2;
      }
    }

    offset ++;
  }

  /*
   * Octet 3, N-PDU number, if F bit set to 1.
   */
  if ((seg_pos & SEG_POS_FIRST) > 0) {
    ll_data_req->sdu.buf[offset] = (UBYTE)pdu_ref.ref_npdu_num;
  }


} /* sua_add_sn_header() */
#endif /* _SNDCP_DTI_2_ */

/*==== PUBLIC FUNCTIONS =====================================================*/
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sua_cia_comp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SUA_CIA_COMP_IND
|
| Parameters  : cia_comp_ind*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sua_cia_comp_ind (T_CIA_COMP_IND* cia_comp_ind)
{
  U8 sapi_index = 0;

  TRACE_ISIG( "sig_cia_cia_sua_comp_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(cia_comp_ind->sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];
  /*
   * Mark segment as transmitted for later acknowledgement.
   */
  sua_mark_segment((U8)cia_comp_ind->pdu_ref.ref_npdu_num,
                   cia_comp_ind->pdu_ref.ref_nsapi);

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      {

        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        U8 seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;

        PALLOC(ll_desc_req, LL_DESC_REQ);
        ll_desc_req->sapi = cia_comp_ind->sapi;
        ll_desc_req->tlli = cia_comp_ind->tlli;
        /* Single parameter copy qos needed due to different SAPs */
        ll_desc_req->ll_qos.delay      = cia_comp_ind->cia_qos.delay;
        ll_desc_req->ll_qos.relclass   = cia_comp_ind->cia_qos.relclass;
        ll_desc_req->ll_qos.peak       = cia_comp_ind->cia_qos.peak;
        ll_desc_req->ll_qos.preced     = cia_comp_ind->cia_qos.preced;
        ll_desc_req->ll_qos.mean       = cia_comp_ind->cia_qos.mean;
        /* radio_prio not defined, handled in separate function later */
        ll_desc_req->reserved_data_req1 = cia_comp_ind->comp_inst;
        ll_desc_req->reference1.ref_nsapi    = cia_comp_ind->pdu_ref.ref_nsapi;
        ll_desc_req->reference1.ref_npdu_num = cia_comp_ind->pdu_ref.ref_npdu_num;
        ll_desc_req->reference1.ref_seg_num  = cia_comp_ind->pdu_ref.ref_seg_num;
        ll_desc_req->seg_pos = cia_comp_ind->seg_pos;
        /* attached_counter not copied */
        /* cipher not copied */
        ll_desc_req->desc_list3.list_len = cia_comp_ind->desc_list3.list_len;
        ll_desc_req->desc_list3.first    = cia_comp_ind->desc_list3.first;

        sua_add_ll_desc_req_params(cia_comp_ind->pdu_ref,
                                   ll_desc_req);
        sua_add_sn_header(seg_pos, pdu_ref, ll_desc_req, cia_comp_ind->packet_type);

        /* All information has been transferred remove the compression primitive */
        MFREE(cia_comp_ind);

        /*
         * Write the ll_desc_req prim to ll_desc_q,
         * increment write pointer.
         */
        sndcp_data->sua->ll_desc_q[sndcp_data->sua->ll_desc_q_write] =
          ll_desc_req;
        sndcp_data->sua->ll_desc_q_write =
          (sndcp_data->sua->ll_desc_q_write + 1) % SNDCP_SEGMENT_NUMBERS_ACK;
      }
      break;
    case SUA_LLC_RECEPTIVE:
      {
        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        U8 ll_desc_seg_pos;
        U8 ll_desc_nsapi;
        U8 ll_desc_sapi;

        U8 seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;

        PALLOC(ll_desc_req, LL_DESC_REQ);
        ll_desc_req->sapi = cia_comp_ind->sapi;
        ll_desc_req->tlli = cia_comp_ind->tlli;
        ll_desc_req->ll_qos.delay      = cia_comp_ind->cia_qos.delay;
        ll_desc_req->ll_qos.relclass   = cia_comp_ind->cia_qos.relclass;
        ll_desc_req->ll_qos.peak       = cia_comp_ind->cia_qos.peak;
        ll_desc_req->ll_qos.preced     = cia_comp_ind->cia_qos.preced;
        ll_desc_req->ll_qos.mean       = cia_comp_ind->cia_qos.mean;
        /* radio_prio not defined, handled in separate function later */
        ll_desc_req->reserved_data_req1 = cia_comp_ind->comp_inst;
        ll_desc_req->reference1.ref_nsapi    = cia_comp_ind->pdu_ref.ref_nsapi;
        ll_desc_req->reference1.ref_npdu_num = cia_comp_ind->pdu_ref.ref_npdu_num;
        ll_desc_req->reference1.ref_seg_num  = cia_comp_ind->pdu_ref.ref_seg_num;
        ll_desc_req->seg_pos = cia_comp_ind->seg_pos;
        /* attached_counter not copied */
        /* cipher not copied */
        ll_desc_req->desc_list3.list_len = cia_comp_ind->desc_list3.list_len;
        ll_desc_req->desc_list3.first    = cia_comp_ind->desc_list3.first;

        sua_add_ll_desc_req_params(cia_comp_ind->pdu_ref,
                                   ll_desc_req);
        sua_add_sn_header(seg_pos, pdu_ref, ll_desc_req, cia_comp_ind->packet_type);

        /* All information has been transferred remove the compression primitive */
        MFREE(cia_comp_ind);

        ll_desc_seg_pos = ll_desc_req->seg_pos;
        ll_desc_sapi    = ll_desc_req->sapi;
        ll_desc_nsapi   = sua_get_nsapi(ll_desc_req);

#ifdef _SIMULATION_

#ifndef MULTI_LAYER
        su_send_ll_data_req_test(ll_desc_req);
#else
        PSEND(hCommLLC, ll_desc_req);
#endif /* MULTI_LAYER */
#else /* _SIMULATION_ */
        PSEND(hCommLLC, ll_desc_req);
#endif /* _SIMULATION_ */

        SET_STATE(SUA, SUA_LLC_NOT_RECEPTIVE);

        if ((ll_desc_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_desc_nsapi );
          sua_next_sn_data_req( ll_desc_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }
      }
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SUA_CIA_COMP_IND unexpected" );
      sndcp_cl_desc3_free((T_desc3 *)cia_comp_ind->desc_list3.first);
      MFREE(cia_comp_ind);
      break;
  }
} /* sig_cia_sua_cia_comp_ind() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sua_cia_comp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SUA_CIA_COMP_IND
|
| Parameters  : cia_comp_ind*
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sua_cia_comp_ind (T_CIA_COMP_IND* cia_comp_ind)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_cia_cia_sua_comp_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(cia_comp_ind->sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];
  /*
   * Mark segment as transmitted for later acknowledgement.
   */
  sua_mark_segment((UBYTE)cia_comp_ind->pdu_ref.ref_npdu_num,
                   cia_comp_ind->pdu_ref.ref_nsapi);

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      {

        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;

        PPASS(cia_comp_ind, ll_data_req, LL_DATA_REQ);

        sua_add_ll_data_req_params(cia_comp_ind->pdu_ref,
                                   ll_data_req);
        sua_add_sn_header(seg_pos, pdu_ref, ll_data_req, cia_comp_ind->packet_type);
        /*
         * Write the ll_data_req prim to ll_data_q,
         * increment write pointer.
         */
        sndcp_data->sua->ll_data_q[sndcp_data->sua->ll_data_q_write] =
          ll_data_req;
        sndcp_data->sua->ll_data_q_write =
          (sndcp_data->sua->ll_data_q_write + 1) % SNDCP_SEGMENT_NUMBERS_ACK;
      }
      break;
    case SUA_LLC_RECEPTIVE:
      {
        /*
         * Save seg_pos and pdu_ref from cia_comp_ind.
         */
        UBYTE ll_data_seg_pos;
        UBYTE ll_data_nsapi;
        UBYTE ll_data_sapi;

        UBYTE seg_pos = cia_comp_ind->seg_pos;
        T_pdu_ref pdu_ref = cia_comp_ind->pdu_ref;

        PPASS(cia_comp_ind, ll_data_req, LL_DATA_REQ);

        sua_add_ll_data_req_params(cia_comp_ind->pdu_ref,
                                      ll_data_req);
        sua_add_sn_header(seg_pos, pdu_ref, ll_data_req, cia_comp_ind->packet_type);

        ll_data_seg_pos = ll_data_req->seg_pos;
        ll_data_sapi    = ll_data_req->sapi;
        ll_data_nsapi   = sua_get_nsapi(ll_data_req);

        PSEND(hCommLLC, ll_data_req);

        SET_STATE(SUA, SUA_LLC_NOT_RECEPTIVE);

        if ((ll_data_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_data_nsapi );
          sua_next_sn_data_req( ll_data_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }
      }
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SUA_CIA_COMP_IND unexpected" );
      break;
  }
} /* sig_cia_sua_cia_comp_ind() */
#endif /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_delete_pdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SUA_DELETE_PDUS
|               The same as the one in service su, but everything is in the
|               acknowledged version here.
|               cia_state is set to CIA_IDLE.
|
| Parameters  : UBYTE nsapi the affected nsapi, UBYTE sapi the affected sapi,
|               BOOL destroy defines if data pay load must be freed.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_delete_pdus (U8 nsapi, U8 sapi, BOOL destroy)
{
  U8 sapi_index;
  BOOL ll_q_affected = FALSE;

  TRACE_ISIG( "sig_mg_sua_delete_pdus" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  sndcp_data->sua->cia_state = CIA_IDLE;

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_RECEPTIVE:
      {
        /*
         * Delete affected N-PDUS from sn_data_q.
         */
        BOOL still = TRUE;
        U8 index = sndcp_data->sua->sn_data_q_read;

        while (still) {
          if (index == sndcp_data->sua->sn_data_q_write) {
            break;
          }
          if (sndcp_data->sua->sn_data_q[index]->nsapi == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            U8 i = 0;
            if (destroy) {
#ifdef _SNDCP_DTI_2_
              MFREE_PRIM(sndcp_data->sua->sn_data_q[index]);
#else /*_SNDCP_DTI_2_*/ 
              PFREE_DESC(sndcp_data->sua->sn_data_q[index]);
#endif /*_SNDCP_DTI_2_*/
            }
            sndcp_data->sua->sn_data_q[index] = NULL;

            for (i = index;
                 i != sndcp_data->sua->sn_data_q_write;
                 i = (i + 1) % SN_DATA_Q_LEN) {


              sndcp_data->sua->sn_data_q[i] =
                sndcp_data->sua->sn_data_q[(i + 1) % SN_DATA_Q_LEN];
              sndcp_data->sua->sn_data_q[(i + 1) % SN_DATA_Q_LEN] = NULL;

            }
            sndcp_data->sua->sn_data_q_write =
              (sndcp_data->sua->sn_data_q_write - 1
                + SN_DATA_Q_LEN) % SN_DATA_Q_LEN;
            if (index == sndcp_data->sua->sn_data_q_write) {
              break;
            }
          } else {
            index = (index + 1) % SN_DATA_Q_LEN;
          } /* if (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi)  */

        }
#ifdef _SNDCP_DTI_2_
        /*
         * Delete affected SN-PDUS from ll_desc_q.
         */
        still = TRUE;
        index = sndcp_data->sua->ll_desc_q_read;
        while (still) {
          if (index == sndcp_data->sua->ll_desc_q_write) {
            break;
          }
          if (sua_get_nsapi(sndcp_data->sua->ll_desc_q[index]) == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            U8 i = 0;

            ll_q_affected = TRUE;
            /*
             * Free the deleted primitive.
             */
            if (sndcp_data->sua->ll_desc_q[index] != NULL) {
              T_desc3 *free_help, *help =
             (T_desc3 *)sndcp_data->sua->ll_desc_q[index]->desc_list3.first;
              while(help != NULL)
              {
                if (destroy) {
                  sndcp_cl_desc3_free(help);/* free help->buffer */
                }
                free_help = help;
                help = (T_desc3 *)free_help->next;
                MFREE(free_help);
              }

              PFREE(sndcp_data->sua->ll_desc_q[index]);
              sndcp_data->sua->ll_desc_q[index] = NULL;
            }

            for (i = index;
                 i != sndcp_data->sua->ll_desc_q_write;
                 i = (i + 1) % SNDCP_SEGMENT_NUMBERS_ACK) {

              sndcp_data->sua->ll_desc_q[i] =
                sndcp_data->sua->ll_desc_q
                  [(i + 1) % SNDCP_SEGMENT_NUMBERS_ACK];
              sndcp_data->sua->ll_desc_q
                [(i + 1) % SNDCP_SEGMENT_NUMBERS_ACK] = NULL;

            }
            sndcp_data->sua->ll_desc_q_write =
              (sndcp_data->sua->ll_desc_q_write - 1
               + SNDCP_SEGMENT_NUMBERS_ACK) % SNDCP_SEGMENT_NUMBERS_ACK;
            if (index == sndcp_data->sua->ll_desc_q_write) {
              break;
            }
          } else {
            index = (index + 1) % SNDCP_SEGMENT_NUMBERS_ACK;
          } /* if (sndcp_data->sua->sn_data_q[index]->nsapi == nsapi) { */

        }
#else /* _SNDCP_DTI_2_ */
        /*
         * Delete affected SN-PDUS from ll_data_q.
         */
        still = TRUE;
        index = sndcp_data->sua->ll_data_q_read;
        while (still)
        {
          if (index == sndcp_data->sua->ll_data_q_write) {
            break;
          }
          if (sua_get_nsapi(sndcp_data->sua->ll_data_q[index]) == nsapi) {
            /*
             * The index for the prims to be shifted when 1 entry is deleted.
             */
            UBYTE i = 0;

            ll_q_affected = TRUE;
            /*
             * Free the deleted primitive.
             */
            if (sndcp_data->sua->ll_data_q[index] != NULL) {
              PFREE(sndcp_data->sua->ll_data_q[index]);
              sndcp_data->sua->ll_data_q[index] = NULL;
            }

            for (i = index;
                 i != sndcp_data->sua->ll_data_q_write;
                 i = (i + 1) % SNDCP_SEGMENT_NUMBERS_ACK) {

              sndcp_data->sua->ll_data_q[i] =
                sndcp_data->sua->ll_data_q
                  [(i + 1) % SNDCP_SEGMENT_NUMBERS_ACK];
              sndcp_data->sua->ll_data_q
                [(i + 1) % SNDCP_SEGMENT_NUMBERS_ACK] = NULL;

            }
            sndcp_data->sua->ll_data_q_write =
              (sndcp_data->sua->ll_data_q_write - 1
               + SNDCP_SEGMENT_NUMBERS_ACK) % SNDCP_SEGMENT_NUMBERS_ACK;
            if (index == sndcp_data->sua->ll_data_q_write) {
              break;
            }
          } else {
            index = (index + 1) % SNDCP_SEGMENT_NUMBERS_ACK;
          } /* if (sndcp_data->sua->sn_data_q[index]->nsapi == nsapi) { */

        }
#endif /* _SNDCP_DTI_2_ */
      }
      /*
       * if the segments in LL queue were from the affected nsapi then the
       * next N-PDU is directed to cia.
       */
      if (ll_q_affected) {
        sua_next_sn_data_req (sapi);
      }

      /*
       * Delete the segment informations.
       */
      {
        T_SEG_INFO* help = sndcp_data->sua->first_seg_info;
        while (help != NULL) {
          T_SEG_INFO* help_next = help->next;
          if (help->nsapi == nsapi) {
            if(sndcp_data->sua->first_seg_info == help)
              sndcp_data->sua->first_seg_info = help_next;
            MFREE(help);
          }
          help = help_next;
        }
      }

      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_DELETE_PDUS unexpected" );
      break;
  }
} /* sig_mg_sua_delete_pdus() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_n201
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SUA_N201
|
| Parameters  : UBYTE sapi the affected sapi, USHORT n201
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_n201 (UBYTE sapi,
                             USHORT n201)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sua_n201" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_RECEPTIVE:
      sndcp_data->sua->n201_i = n201;
      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_N201 unexpected" );
      break;
  }
} /* sig_mg_sua_n201() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SUA_RESET_IND
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_reset_ind (UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sua_reset_ind" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_RECEPTIVE:
      /*
       * This is only here:
       */
      SET_STATE(SUA, SUA_LLC_NOT_RECEPTIVE);

      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_sua_reset_ind() */

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_RESUME
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_resume (U8 sapi)
{
  U8 sapi_index = 0;

  TRACE_ISIG( "sig_mg_sua_resume" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      SET_STATE (SUA, SUA_LLC_NOT_RECEPTIVE);
      break;
    case SUA_LLC_RECEPTIVE_SUSPEND:
      /*
       * Is queue with LL_DESC_REQ empty?
       */
      if (sndcp_data->sua->ll_desc_q_write ==
          sndcp_data->sua->ll_desc_q_read) {
        SET_STATE(SUA, SUA_LLC_RECEPTIVE);
      } else {
        /*
         * Get next segment from queue and send it.
         */
        T_LL_DESC_REQ* ll_desc_req =
          sndcp_data->sua->ll_desc_q[sndcp_data->sua->ll_desc_q_read];

        U8 ll_desc_seg_pos = ll_desc_req->seg_pos;
        U8 ll_desc_nsapi   = sua_get_nsapi(ll_desc_req);
        U8 ll_desc_sapi    = ll_desc_req->sapi;

        sndcp_data->sua->ll_desc_q[sndcp_data->sua->ll_desc_q_read] = NULL;

        sndcp_data->sua->ll_desc_q_read =
            (sndcp_data->sua->ll_desc_q_read + 1) % SNDCP_SEGMENT_NUMBERS_ACK;

#ifdef _SIMULATION_
        su_send_ll_data_req_test(ll_desc_req);
#else /* _SIMULATION_ */
        PSEND(hCommLLC, ll_desc_req);
#endif /* _SIMULATION_ */

        SET_STATE(SUA, SUA_LLC_NOT_RECEPTIVE);
        if ((ll_desc_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_desc_nsapi );
          sua_next_sn_data_req( ll_desc_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }
      }
      break;
    case SUA_LLC_NOT_RECEPTIVE:
      break;
    case SUA_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_RESUME unexpected" );
      break;
  }
} /* sig_mg_sua_resume() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SU_RESUME
|
| Parameters  : UBYTE sapi the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_resume (UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sua_resume" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      SET_STATE (SUA, SUA_LLC_NOT_RECEPTIVE);
      break;
    case SUA_LLC_RECEPTIVE_SUSPEND:
      /*
       * Is queue with LL_DATA_REQ empty?
       */
      if (sndcp_data->sua->ll_data_q_write ==
          sndcp_data->sua->ll_data_q_read) {
        SET_STATE(SUA, SUA_LLC_RECEPTIVE);
      } else {
        /*
         * Get next segment from queue and send it.
         */
        T_LL_DATA_REQ* ll_data_req =
          sndcp_data->sua->ll_data_q[sndcp_data->sua->ll_data_q_read];

        UBYTE ll_data_seg_pos = ll_data_req->seg_pos;
        UBYTE ll_data_nsapi   = sua_get_nsapi(ll_data_req);
        UBYTE ll_data_sapi    = ll_data_req->sapi;

        sndcp_data->sua->ll_data_q[sndcp_data->sua->ll_data_q_read] = NULL;

        sndcp_data->sua->ll_data_q_read =
            (sndcp_data->sua->ll_data_q_read + 1) % SNDCP_SEGMENT_NUMBERS_ACK;

        PSEND(hCommLLC, ll_data_req);

        SET_STATE(SUA, SUA_LLC_NOT_RECEPTIVE);
        if ((ll_data_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_data_nsapi );
          sua_next_sn_data_req( ll_data_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }
      }
      break;
    case SUA_LLC_NOT_RECEPTIVE:
      break;
    case SUA_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_RESUME unexpected" );
      break;
  }
} /* sig_mg_sua_resume() */
#endif /* _SNDCP_DTI_2_ */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sua_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_NU_SUA_SUSPEND
|
| Parameters  : SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sua_suspend (UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sua_suspend" );

  /*
   * Set service instance according to sapi in signal.
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      break;
    case SUA_LLC_RECEPTIVE_SUSPEND:
      break;
    case SUA_LLC_NOT_RECEPTIVE:
      SET_STATE (SUA, SUA_LLC_NOT_RECEPTIVE_SUSPEND);
      break;
    case SUA_LLC_RECEPTIVE:
      SET_STATE (SUA, SUA_LLC_RECEPTIVE_SUSPEND);
      break;
    default:
      TRACE_ERROR( "SIG_MG_SUA_SUSPEND unexpected" );
      break;
  }
} /* sig_mg_sua_suspend() */


/*
+------------------------------------------------------------------------------
| Function    : sig_nu_sua_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_NU_SUA_DATA_REQ
|
| Parameters  : SN_DATA_REQ primitive, N-PDU number, NSAPI, SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_nu_sua_data_req (T_SN_DATA_REQ* sn_data_req,
                                 UBYTE npdu_number,
                                 UBYTE nsapi,
                                 UBYTE sapi
                                )
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_nu_sua_data_req" );

#ifdef _SNDCP_MEAN_TRACE_
#ifdef _SNDCP_DTI_2_
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_UP,
                   SNDCP_MEAN_ACK,
                   sn_data_req->desc_list2.list_len);
#else /*_SNDCP_DTI_2_*/
  sndcp_mean_trace(nsapi,
                   SNDCP_MEAN_UP,
                   SNDCP_MEAN_ACK,
                   sn_data_req->desc_list.list_len);
#endif /*_SNDCP_DTI_2_*/
#endif /* _SNDCP_MEAN_TRACE_ */

  /*
   * set service instance according to sapi in signal
   */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_RECEPTIVE:
      /*
       * If queue with NPDUs is empty, and cia is not busy,
       * send the NPDU to cia, else
       * add it to the queue.
       */
      if ((sndcp_data->sua->sn_data_q_read ==
           sndcp_data->sua->sn_data_q_write) &&
          (sndcp_data->sua->ll_desc_q_read ==
           sndcp_data->sua->ll_desc_q_write) &&
          sndcp_data->sua->cia_state == CIA_IDLE) {
        /*
         * Send the NPDU to cia.
         */
        sndcp_data->sua->cia_state = CIA_BUSY;
        sig_sua_cia_cia_comp_req(sn_data_req,
                            npdu_number,
                            nsapi,
                            sapi);
      } else { /* Queue with NPDUs is not empty or cia is busy. */
        /*
         * Write the sn_data_req prim to sn_data_q,
         * increment write pointer.
         */
        sndcp_data->sua->sn_data_q[sndcp_data->sua->sn_data_q_write] =
          sn_data_req;
        sndcp_data->sua->npdu_number_q[sndcp_data->sua->sn_data_q_write] =
          npdu_number;
        sndcp_data->sua->sn_data_q_write =
          (sndcp_data->sua->sn_data_q_write + 1) % SN_DATA_Q_LEN;
      } /* Queue with NPDUs is not empty or cia is busy. */
      break;
    default:
      TRACE_ERROR( "SIG_NU_SUA_DATA_REQ unexpected" );
#ifdef _SNDCP_DTI_2_
      MFREE_PRIM(sn_data_req);
#else /*_SNDCP_DTI_2_*/
      PFREE_DESC(sn_data_req);
#endif /*_SNDCP_DTI_2_*/
      break;
  }
} /* sig_nu_sua_data_req() */

#ifdef _SIMULATION_
#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : su_send_ll_data_req_test
+------------------------------------------------------------------------------
| Description : Only for test purposes.
|               The given LL_DESC_REQ is copied to an
|               LL_DATA_REQ and sent for the simulated test environment.
|
| Parameters  : T_LL_DESC_REQ* pointer descriptor data.
|
+------------------------------------------------------------------------------
*/
void su_send_ll_data_req_test(T_LL_DESC_REQ* ll_desc_req)
{
  T_desc3* descriptor = NULL;
  U8* p_data = NULL;
  U16 offset = ENCODE_OFFSET_BYTE;

  PALLOC_SDU(ll_data_req, LL_DATA_REQ, (U16)( ll_desc_req->desc_list3.list_len* 8));

  /*
   * Set parameters in ll_data_req primitive
   */
  ll_data_req->sapi = ll_desc_req->sapi;
  ll_data_req->tlli = ll_desc_req->tlli;
  ll_data_req->ll_qos = ll_desc_req->ll_qos;
  ll_data_req->radio_prio = ll_desc_req->radio_prio;
  ll_data_req->reserved_data_req1 = ll_desc_req->reserved_data_req1;
  ll_data_req->reference1 = ll_desc_req->reference1;
  ll_data_req->seg_pos = ll_desc_req->seg_pos;
  ll_data_req->attached_counter = ll_desc_req->attached_counter;
  ll_data_req->reserved_data_req4 = ll_desc_req->reserved_data_req4;
#ifdef REL99
  ll_data_req->pkt_flow_id = ll_desc_req->pkt_flow_id;
#endif /*REL99*/
  /*
   * Set the ENCODE_OFFSET in the uplink sdu to 0, set sdu.l_buf
   */
  ll_data_req->sdu.o_buf = ENCODE_OFFSET;
  ll_data_req->sdu.l_buf = ll_desc_req->desc_list3.list_len* 8;
  /*
   * Copy the data.
   */
  descriptor = (T_desc3*)ll_desc_req->desc_list3.first;
  while (descriptor != NULL)
  {
    T_desc3* help = descriptor;
    p_data = (U8*)descriptor->buffer;
    if (descriptor->len>0)
    {
      memcpy(&ll_data_req->sdu.buf[offset],
             &p_data[descriptor->offset],
             descriptor->len);
    }
    offset += descriptor->len;
    descriptor = (T_desc3*)descriptor->next;
    sndcp_cl_desc3_free(help);
    MFREE(help);
    help = NULL;
  }

  PSEND(hCommLLC, ll_data_req);

  PFREE(ll_desc_req); // MAYBE REMEMBER is this the correct free?

}
#endif /* _SNDCP_DTI_2_ */
#endif /* _SIMULATION_ */



