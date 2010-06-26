/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_f.c
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
|  Purpose :  Contains global functions of SNDCP
+-----------------------------------------------------------------------------
*/

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "macdef.h"

#include "dti.h"
#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_nup.h"      /* to get nu functions that will be called from
                               sndcp_sig_callback() */
#include "sndcp_ndp.h"      /* to get nu functions that will be called from
                               sndcp_sig_callback() */


#include <string.h>     /* to get memcpy() */


/*==== CONST ================================================================*/

#ifdef SNDCP_TRACE_IP_DATAGRAM
#ifndef CF_FAST_EXEC
static U8 bin_trace_ip_buf[1500];
#endif /* CF_FAST_EXEC */
#endif
/*==== LOCAL VARS ===========================================================*/




#ifdef TI_PS_OP_ICUT_SNDCP

/* PDP_TBR add SNDCP terminal loopback capability */
extern U8 SNDCP_LOOPBACK;
extern BOOL bufFull[SNDCP_NUMBER_OF_NSAPIS];
extern T_DTI2_DATA_IND *saveDti2_data_ind[SNDCP_NUMBER_OF_NSAPIS] ;

#endif /* TI_PS_OP_ICUT_SNDCP */





/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_ack
+------------------------------------------------------------------------------
| Description : gets nsapi_ack_ra[nsapi]
|
| Parameters  : UBYTE nsapi, BOOL* ack
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_ack(UBYTE nsapi,
                                  BOOL* ack)
{
  TRACE_FUNCTION("sndcp_get_nsapi_ack");
  {
    *ack = sndcp_data->nsapi_ack_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_direction
+------------------------------------------------------------------------------
| Description : gets nsapi_ack_direction[nsapi]
|               Values: HOME or NEIGHBOR from dti.h.
|
| Parameters  : UBYTE nsapi, BOOL* direction
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_direction(UBYTE nsapi,
                                      U8* direction)
{
  TRACE_FUNCTION("sndcp_get_nsapi_direction");
  {
    *direction = (U8)sndcp_data->nsapi_direction_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_interface
+------------------------------------------------------------------------------
| Description : gets nsapi_interface_ra[nsapi]
|
| Parameters  : UBYTE nsapi, U8 interface
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_interface(UBYTE nsapi,
                                      U8* interfac)
{
  TRACE_FUNCTION("sndcp_get_nsapi_interface");
  {
    *interfac = sndcp_data->nsapi_interface_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_linkid
+------------------------------------------------------------------------------
| Description : gets nsapi_linkid_ra[nsapi]
|
| Parameters  : UBYTE nsapi, ULONG* linkid
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_linkid(UBYTE nsapi,
                                   ULONG* linkid)
{
  TRACE_FUNCTION("sndcp_get_nsapi_linkid");
  {
    *linkid = sndcp_data->nsapi_linkid_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_neighbor
+------------------------------------------------------------------------------
| Description : gets nsapi_neighbor_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE** neighbor
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_neighbor(UBYTE nsapi,
                                     UBYTE** neighbor)
{
  TRACE_FUNCTION("sndcp_get_nsapi_neighbor");
  {
    *neighbor = sndcp_data->nsapi_neighbor_ra[nsapi];
  }
}
#endif /* CF_FAST_EXEC */

#else /*_SNDCP_DTI_2_*/
/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_direction
+------------------------------------------------------------------------------
| Description : gets nsapi_ack_direction[nsapi]
|               Values: HOME or NEIGHBOR from dti.h.
|
| Parameters  : UBYTE nsapi, BOOL* direction
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_direction(UBYTE nsapi,
                                      BOOL* direction)
{
  TRACE_FUNCTION("sndcp_get_nsapi_direction");
  {
    *direction = sndcp_data->nsapi_direction_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_interface
+------------------------------------------------------------------------------
| Description : gets nsapi_interface_ra[nsapi]
|
| Parameters  : UBYTE nsapi, U8 interface
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_interface(UBYTE nsapi,
                                      U8* interfac)
{
  TRACE_FUNCTION("sndcp_get_nsapi_interface");
  {
    *interfac = sndcp_data->nsapi_interface_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_linkid
+------------------------------------------------------------------------------
| Description : gets nsapi_linkid_ra[nsapi]
|
| Parameters  : UBYTE nsapi, ULONG* linkid
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_linkid(UBYTE nsapi,
                                   ULONG* linkid)
{
  TRACE_FUNCTION("sndcp_get_nsapi_linkid");
  {
    *linkid = sndcp_data->nsapi_linkid_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_neighbor
+------------------------------------------------------------------------------
| Description : gets nsapi_neighbor_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE** neighbor
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_neighbor(UBYTE nsapi,
                                     UBYTE** neighbor)
{
  TRACE_FUNCTION("sndcp_get_nsapi_neighbor");
  {
    *neighbor = sndcp_data->nsapi_neighbor_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

#endif /*_SNDCP_DTI_2_*/

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_sapi_ack
+------------------------------------------------------------------------------
| Description : gets sapi_ack_ra["index of sapi"]
|
| Parameters  : UBYTE sapi, BOOL* ack
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_sapi_ack(UBYTE sapi,
                                  BOOL* ack)
{
  TRACE_FUNCTION("sndcp_get_sapi_ack");
  {
    UBYTE sapi_index = 0;

    sndcp_get_sapi_index(sapi, &sapi_index);
    *ack = sndcp_data->sapi_ack_ra[sapi_index];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_sapi_stat
+------------------------------------------------------------------------------
| Description : gets sapi_state_ra[sapi]
|
| Parameters  : UBYTE sapi, UBYTE* stat
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_sapi_state(UBYTE sapi,
                                 USHORT* stat)
{
  TRACE_FUNCTION("sndcp_get_sapi_state");
  {
    UBYTE sapi_index = 0;

    sndcp_get_sapi_index(sapi, &sapi_index);
    *stat = sndcp_data->sapi_state_ra[sapi_index];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_sapi_index
+------------------------------------------------------------------------------
| Description : gets the index (0, 1, 2, 3) for the given sapi (3, 5, 9, 11).
| This function is used when a sapi number serves as a key for one of the 4
| possible instances of services su and sd.
|
| Parameters  : UBYTE sapi, UBYTE* index
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined(SNDCP_2to1) */

GLOBAL void sndcp_get_sapi_index(UBYTE sapi,
                                 UBYTE* index)
{
  TRACE_FUNCTION("sndcp_get_sapi_index");
  {
    if (sapi == 3) {
      *index = 0;
    } else if (sapi == 5) {
      *index = 1;
    } else if (sapi == 9) {
      *index = 2;
    } else if (sapi == 11) {
      *index = 3;
    }
  }
}

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_prio
+------------------------------------------------------------------------------
| Description : sets prio to nsapi_prio_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE* prio
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_prio(UBYTE nsapi,
                                 UBYTE* prio)
{
  TRACE_FUNCTION("sndcp_get_nsapi_prio");
  {
    *prio = sndcp_data->nsapi_prio_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

#ifdef REL99 
/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_pktflowid
+------------------------------------------------------------------------------
| Description : gets packet flow id form nsapi_pktflowid_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE* pkt_flow_id
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_pktflowid(U8 nsapi, U16* pkt_flow_id)

{
  TRACE_FUNCTION("sndcp_get_nsapi_pktflowid");
  {
    *pkt_flow_id = sndcp_data->nsapi_pktflowid_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

#endif /*REL99*/

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_qos
+------------------------------------------------------------------------------
| Description : gets nsapi_qos_ra[nsapi]
|
| Parameters  : UBYTE nsapi, T_snsm_qos* qos
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_qos(UBYTE nsapi,
                                 T_snsm_qos* qos)
{
  TRACE_FUNCTION("sndcp_get_nsapi_qos");
  {
    qos->delay = sndcp_data->nsapi_qos_ra[nsapi].delay;
    qos->relclass = sndcp_data->nsapi_qos_ra[nsapi].relclass;
    qos->peak = sndcp_data->nsapi_qos_ra[nsapi].peak;
    qos->preced = sndcp_data->nsapi_qos_ra[nsapi].preced;
    qos->mean = sndcp_data->nsapi_qos_ra[nsapi].mean;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_sapi
+------------------------------------------------------------------------------
| Description : gets nsapi_sapi_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE* sapi
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_sapi(UBYTE nsapi,
                                 UBYTE* sapi)
{
  TRACE_FUNCTION("sndcp_get_nsapi_sapi");
  {
    *sapi = sndcp_data->nsapi_sapi_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_state
+------------------------------------------------------------------------------
| Description : gets nsapi_state_ra[nsapi]
|
| Parameters  : UBYTE nsapi, USHORT* state
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_get_nsapi_state(UBYTE nsapi,
                                  USHORT* state)
{
  TRACE_FUNCTION("sndcp_get_nsapi_state");
  {
    *state = sndcp_data->nsapi_state_ra[nsapi];
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_is_nsapi_data_compressed
+------------------------------------------------------------------------------
| Description : If nsapi uses data compression then compressed is set to TRUE,
|               else to FALSE.
|
| Parameters  : UBYTE nsapi, BOOL* compressed
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_is_nsapi_data_compressed(UBYTE nsapi,
                                           BOOL* compressed)
{
  USHORT nsapis = sndcp_data->cia.cur_xid_block.v42.nsapis;

  TRACE_FUNCTION("sndcp_is_nsapi_data_compressed");

  *compressed = (nsapis & (1 << nsapi)) > 0;

}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_is_nsapi_used
+------------------------------------------------------------------------------
| Description : sets b to TRUE if the nsapi is already in use, otherwise to
|               FALSE
|
| Parameters  : UBYTE nsapi, BOOL* b
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   defined (SNDCP_2to1) */

GLOBAL void sndcp_is_nsapi_used(UBYTE nsapi,
                                BOOL* b)
{
  TRACE_FUNCTION("sndcp_is_nsapi_used");
  {
    *b = sndcp_data->nsapi_used_ra[nsapi];
  }
}

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ || !REL99 || SNDCP_2to1 */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_is_nsapi_header_compressed
+------------------------------------------------------------------------------
| Description : If nsapi uses header compression then compressed is set to TRUE,
|               else to FALSE.
|
| Parameters  : UBYTE nsapi, BOOL* compressed
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_is_nsapi_header_compressed(UBYTE nsapi,
                                             BOOL* compressed)
{
  USHORT nsapis = sndcp_data->cia.cur_xid_block.vj.nsapis;

  TRACE_FUNCTION("sndcp_is_nsapi_header_compressed");

  *compressed = ((nsapis & (1 << nsapi)) > 0) &&
    sndcp_data->cia.cur_xid_block.vj.is_set;

}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_mean_trace
+------------------------------------------------------------------------------
| Description : traces the mean thruput for given
|
| Parameters  : nsapi, direction, ack_mode
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_mean_trace(UBYTE nsapi,
                             UBYTE direction,
                             UBYTE ack_mode,
                             USHORT len)
{
  T_TIME* start_time_ra = NULL;
  T_TIME* cur_time_ra = NULL;
  ULONG* cur_num_ra = NULL;
  ULONG* cur_pac_ra = NULL;

  TRACE_FUNCTION("sndcp_mean_trace");

  if (direction == SNDCP_MEAN_UP) {
    if (ack_mode == SNDCP_MEAN_ACK) {
      start_time_ra = sndcp_data->start_time_uplink_ack;
      cur_time_ra = sndcp_data->cur_time_uplink_ack;
      cur_num_ra = sndcp_data->cur_num_uplink_ack;
      cur_pac_ra = sndcp_data->cur_pac_uplink_ack;
    } else {
      start_time_ra = sndcp_data->start_time_uplink_unack;
      cur_time_ra = sndcp_data->cur_time_uplink_unack;
      cur_num_ra = sndcp_data->cur_num_uplink_unack;
      cur_pac_ra = sndcp_data->cur_pac_uplink_unack;
    }
  } else {
    if (ack_mode == SNDCP_MEAN_ACK) {
      start_time_ra = sndcp_data->start_time_downlink_ack;
      cur_time_ra = sndcp_data->cur_time_downlink_ack;
      cur_num_ra = sndcp_data->cur_num_downlink_ack;
      cur_pac_ra = sndcp_data->cur_pac_downlink_ack;
    } else {
      start_time_ra = sndcp_data->start_time_downlink_ack;
      cur_time_ra = sndcp_data->cur_time_downlink_ack;
      cur_num_ra = sndcp_data->cur_num_downlink_unack;
      cur_pac_ra = sndcp_data->cur_pac_downlink_unack;
    }
  }

  /*
   * Increment number of packets.
   */
  cur_pac_ra[nsapi] ++;


  if (start_time_ra[nsapi] == 0) {
    vsi_t_time(VSI_CALLER  &start_time_ra[nsapi]);
    cur_num_ra[nsapi] += len;
  } else {

    ULONG mean = 0;
    ULONG delta_millis = 0;

    vsi_t_time(VSI_CALLER  &cur_time_ra[nsapi]);
    cur_num_ra[nsapi] += len;

    delta_millis = cur_time_ra[nsapi] -
                   start_time_ra[nsapi];
    if (direction == SNDCP_MEAN_UP) {
      if (delta_millis > 0) {
        mean = (cur_num_ra[nsapi] * 1000) / delta_millis;
        TRACE_EVENT_P4(
          "nsapi %d  up. %d octets in %d ms. %d octets per sec.",
          nsapi,
          cur_num_ra[nsapi],
          delta_millis,
          mean
        );
      } else {
        TRACE_EVENT_P3(
          "nsapi %d  up. %d octets in %d ms.",
          nsapi,
          cur_num_ra[nsapi],
          delta_millis
        );
      }

    } else { /* not (direction == SNDCP_MEAN_UP) */
      if (delta_millis > 0) {
        mean = (cur_num_ra[nsapi] * 1000) / delta_millis;
        TRACE_EVENT_P4(
          "nsapi %d  down. %d octets in %d ms. %d octets per sec.",
          nsapi,
          cur_num_ra[nsapi],
          delta_millis,
          mean
        );
      } else {
        TRACE_EVENT_P3(
          "nsapi %d  down. %d octets in %d ms.",
          nsapi,
          cur_num_ra[nsapi],
          delta_millis
        );
      }
    } /* not (direction == SNDCP_MEAN_UP) */

  }

}

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sndcp_reset_xid_block
+------------------------------------------------------------------------------
| Description : Resets the given T_XID_BLOCK to default values.
|
| Parameters  : T_XID_BLOCK* xid_block
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_reset_xid_block (T_XID_BLOCK* xid_block)
{
  TRACE_FUNCTION( "sndcp_reset_xid_block" );

  /*
   * Reset all flags for optional compression field parameters.
   */
  xid_block->version_set = FALSE;
  xid_block->v42.is_set = FALSE;
  xid_block->vj.is_set = FALSE;

  xid_block->v42.nsapis_set = FALSE;
  xid_block->v42.p0_set = FALSE;
  xid_block->v42.p1_set = FALSE;
  xid_block->v42.p2_set = FALSE;
  xid_block->vj.nsapis_set = FALSE;
  xid_block->vj.s0_m_1_set = FALSE;
  /*
   * Set fields to default values.
   */
  xid_block->v42.nsapis = SNDCP_NSAPIS_DEFAULT;
  xid_block->v42.p0 = SNDCP_V42_DEFAULT_DIRECTION;
  xid_block->v42.p1 = SNDCP_V42_DEFAULT_P1;
  xid_block->v42.p2 = SNDCP_V42_DEFAULT_P2;

  xid_block->vj.nsapis = SNDCP_NSAPIS_DEFAULT;
  xid_block->vj.s0_m_1 = SNDCP_VJ_DEFAULT_S0_M_1;


} /* sndcp_reset_xid_block() */

#endif /* CF_FAST_EXEC */

#ifdef _SIMULATION_
/*
+------------------------------------------------------------------------------
| Function    : sndcp_sdu_to_desc_list
+------------------------------------------------------------------------------
| Description : copies the given sdu to the given desc_list
|
| Parameters  : T_sdu* sdu,
|               T_desc_list* desc_list
|
+------------------------------------------------------------------------------
*/
#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_sdu_to_desc_list(T_sdu* sdu, T_desc_list2* desc_list) {
  T_desc2           *desc = NULL;
  T_desc2           *last_desc = NULL;
#else /*_SNDCP_DTI_2_*/
GLOBAL void sndcp_sdu_to_desc_list(T_sdu* sdu, T_desc_list* desc_list) {
  T_desc            *desc = NULL;
  T_desc            *last_desc = NULL;
#endif /*_SNDCP_DTI_2_*/
  USHORT               sdu_index;
  USHORT               length;


  /*
   * Begin at the first relevant octet.
   */
  sdu_index = sdu->o_buf/8;

  /*
   * Initialise descriptor list length.
   */
  desc_list->list_len = 0;


  /*
   * Copy complete SDU to descriptor list using descriptors of max. 10 bytes.
   */
  while (sdu_index < sdu->l_buf / 8)
  {
    /*
     * Calculate length of descriptor data (= length of remaining sdu buffer
     * with a maximum of 10)
     */
    length = (sdu_index + 10 < sdu->l_buf / 8) ?
                      10 : (sdu->l_buf / 8 - sdu_index);

    /*
     * Allocate the necessary size for the data descriptor. The size is
     * calculated as follows:
     * - take the size of a descriptor structure
     * - subtract one because of the array buffer[1] to get the size of
     *   descriptor control information
     * - add number of octets of descriptor data
     */
#ifdef _SNDCP_DTI_2_
    MALLOC (desc, (USHORT)(sizeof(T_desc2) - 1 + length));
#else /*_SNDCP_DTI_2_*/
    MALLOC (desc, (USHORT)(sizeof(T_desc) - 1 + length));
#endif /*_SNDCP_DTI_2_*/
    /*
     * Fill descriptor control information.
     */
    desc->next  = (ULONG)NULL;
    desc->len   = length;
#ifdef _SNDCP_DTI_2_
    desc->offset = 0;
    desc->size = desc->len;
#endif
    /*
     * Add length of descriptor data to list length.
     */
    desc_list->list_len += length;

    /*
     * Copy user data from SDU to descriptor.
     */
    if (length>0)
    {
      memcpy (desc->buffer, &sdu->buf[sdu_index], length);
    }
    sdu_index += length;

    if (last_desc)
    {
      /*
       * Add this descriptor (not the first) to the descriptor list.
       */
      last_desc->next = (ULONG)desc;
    }
    else
    {
      /*
       * Insert first descriptor in descriptor list.
       */
      desc_list->first     = (ULONG)desc;
    }

    /*
     * Store this descriptor for later use.
     */
    last_desc = desc;
  }

 }
#endif /* _SIMULATION_ */



/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_ack
+------------------------------------------------------------------------------
| Description : sets nsapi_ack_ra[nsapi] to ack
|
| Parameters  : UBYTE nsapi, BOOL ack
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_ack(UBYTE nsapi,
                                 BOOL ack)
{
  TRACE_FUNCTION(" sndcp_set_nsapi_ack ");
  {
    sndcp_data->nsapi_ack_ra[nsapi] = ack;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_direction
+------------------------------------------------------------------------------
| Description : sets nsapi_direction_ra[nsapi] to direction
|
| Parameters  : UBYTE nsapi, BOOL direction
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_direction(UBYTE nsapi,
                                      BOOL direction)
{
  TRACE_FUNCTION(" sndcp_set_nsapi_direction ");
#ifdef _SNDCP_DTI_2_
  if (direction == DTI_CHANNEL_TO_LOWER_LAYER) {
    sndcp_data->nsapi_direction_ra[nsapi] = DTI_CHANNEL_TO_LOWER_LAYER;
  } else {
    sndcp_data->nsapi_direction_ra[nsapi] = DTI_CHANNEL_TO_HIGHER_LAYER;
#else /*_SNDCP_DTI_2_*/
  if (direction == SN_HOME) {
    sndcp_data->nsapi_direction_ra[nsapi] = HOME;
  } else {
    sndcp_data->nsapi_direction_ra[nsapi] = NEIGHBOR;
#endif /*_SNDCP_DTI_2_*/
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_interface
+------------------------------------------------------------------------------
| Description : sets nsapi_interface_ra[nsapi] to interfac
|
| Parameters  : UBYTE nsapi, U8 interfac
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_interface(UBYTE nsapi,
                                      U8 interfac)
{
  TRACE_FUNCTION(" sndcp_set_nsapi_interface ");
  {
    sndcp_data->nsapi_interface_ra[nsapi] = interfac;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_linkid
+------------------------------------------------------------------------------
| Description : sets nsapi_linkid_ra[nsapi] to linkid
|
| Parameters  : UBYTE nsapi, U32 linkid
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_linkid(UBYTE nsapi,
                                   U32 linkid)
{
  TRACE_FUNCTION(" sndcp_set_nsapi_linkid ");
  {
    sndcp_data->nsapi_linkid_ra[nsapi] = linkid;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_neighbor
+------------------------------------------------------------------------------
| Description : sets nsapi_neighbor_ra[nsapi] to neighbor
|
| Parameters  : UBYTE nsapi, U8* neighbor
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_neighbor(UBYTE nsapi,
                                     U8* neighbor)
{
  TRACE_FUNCTION(" sndcp_set_nsapi_neighbor ");
  {
    sndcp_data->nsapi_neighbor_ra[nsapi] = neighbor;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_no_xid
+------------------------------------------------------------------------------
| Description : XID negotiation during each context (default in our
|               implementation) is switched off.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_no_xid(void)
{
  TRACE_FUNCTION("sndcp_no_xid");
  {
    sndcp_data->always_xid = FALSE;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_prio
+------------------------------------------------------------------------------
| Description : sets nsapi_prio_ra[nsapi] to prio - 1:
|               SNSM prio val is mapped to LL prio val, which is the internal
|               representation.
|
| Parameters  : UBYTE nsapi, UBYTE prio
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_prio(UBYTE nsapi,
                                 UBYTE prio)
{
  TRACE_FUNCTION("sndcp_set_nsapi_prio");
  {
    sndcp_data->nsapi_prio_ra[nsapi] = prio - 1;
  }
}

#endif /* CF_FAST_EXEC */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_pktflowid
+------------------------------------------------------------------------------
| Description : sets pktflowid_ra[nsapi] to packet_flow_identifier:
|               SNSM packet flow identifier is mapped to LL pkt_flow_id, which is the internal
|               representation.
|
| Parameters  : UBYTE nsapi, UBYTE packet_flow_identifier
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_pktflowid(UBYTE nsapi,
                                 U16 packet_flow_identifier)
{
  TRACE_FUNCTION("sndcp_set_nsapi_pktflowid");
  {
    sndcp_data->nsapi_pktflowid_ra[nsapi] = packet_flow_identifier;
  }
}
#endif /* CF_FAST_EXEC */

#endif /*REL99*/

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_qos
+------------------------------------------------------------------------------
| Description : sets nsapi_qos_ra[nsapi] to qos
|
| Parameters  : UBYTE nsapi, T_snsm_qos qos
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_qos(UBYTE nsapi,
                                T_snsm_qos qos)
{
  TRACE_FUNCTION("sndcp_set_nsapi_qos");
  {
    sndcp_data->nsapi_qos_ra[nsapi].delay = qos.delay;
    sndcp_data->nsapi_qos_ra[nsapi].relclass = qos.relclass;
    sndcp_data->nsapi_qos_ra[nsapi].peak = qos.peak;
    sndcp_data->nsapi_qos_ra[nsapi].preced = qos.preced;
    sndcp_data->nsapi_qos_ra[nsapi].mean = qos.mean;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_sapi
+------------------------------------------------------------------------------
| Description : sets nsapi_sapi_ra[nsapi] to sapi
|
| Parameters  : UBYTE nsapi, UBYTE sapi
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_sapi(UBYTE nsapi,
                                 UBYTE sapi)
{
  TRACE_FUNCTION("sndcp_set_nsapi_sapi");
  {
    sndcp_data->nsapi_sapi_ra[nsapi] = sapi;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_state
+------------------------------------------------------------------------------
| Description : adds stat to nsapi_state_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE stat
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
GLOBAL void sndcp_set_nsapi_state(UBYTE nsapi,
                                 USHORT stat)
{
  TRACE_FUNCTION("sndcp_set_nsapi_state");
  {
    sndcp_data->nsapi_state_ra[nsapi] |= stat;
  }
}

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sndcp_ip_packet_filter
+------------------------------------------------------------------------------
| Description : The function filters TCP and UDP packets on certain ports
|
| Parameters  : T_desc_list2 *desc_list, BOOL *discard
|
+------------------------------------------------------------------------------
*/
#ifdef _SNDCP_DTI_2_
LOCAL void sndcp_ip_packet_filter(T_desc_list2 *desc_list, BOOL *discard)
{
  T_desc2 *desc = (T_desc2 *)desc_list->first;
  UBYTE *pack_hdr = &desc->buffer[desc->offset];
  UBYTE *tcp_hdr = NULL, 
        *udp_hdr = NULL;
  USHORT offset = 0;
  USHORT src_port   = 0;
  USHORT dest_port  = 0;

  TRACE_FUNCTION("sndcp_ip_packet_filter");

  if(desc_list == NULL || desc_list->first == NULL || desc_list->list_len == 0){
    *discard = TRUE; /* discard packet */
    return; 
  }

  offset = (pack_hdr[0] & 0x0f) * 4; /* IP header length in bytes*/
  /* Verify, that the desc has a proper length */
  if(desc->len < (offset + 4)){
    return;
  }

  switch(pack_hdr[9]){
    /* ICMP Protocol */
    case 1:
      *discard = FALSE; /* don't discard ICMP packets */
      /* TRACE_EVENT("INFO IP FILTER: ICMP packet is sent"); */
     return;
    /* TCP Protocol */
    case 6:
      tcp_hdr = &pack_hdr[offset];
      src_port  |= tcp_hdr[0] << 8;
      src_port  |= tcp_hdr[1];
      dest_port |= tcp_hdr[2] << 8;
      dest_port |= tcp_hdr[3];
      if(src_port  == 42   || /* Host Name Server */
         dest_port == 42   || 
         src_port  == 111  || /* Remote Procedure Call */
         dest_port == 111  || 
         ((src_port  > 134) && (src_port  < 140))  || /* NetBIOS */
         ((dest_port > 134) && (dest_port < 140))  || /* NetBIOS */
         src_port  == 445   || /* Microsoft-DS */
         dest_port == 445   || 
         src_port  == 1025  || /* network blackjack */
         dest_port == 1025  || 
         src_port  == 1196  || /* listener RFS */
         dest_port == 1196  || 
         src_port  == 1433  || /* Microsoft-SQL-Server */
         dest_port == 1433  || 
         src_port  == 1434  || /* Microsoft-SQL-Monitor */
         dest_port == 1434   
       ){
        *discard = TRUE; /* discard this TCP packet */
        /*TRACE_EVENT_P2("INFO IP FILTER: TCP packet on dest_port %d, src_port %d is discarded",
                                                                   dest_port, src_port);*/
      } else {                                                             
        *discard = FALSE; /* dont't discard this TCP packet */
        /*TRACE_EVENT_P2("INFO IP FILTER: TCP packet on dest_port %d, src_port %d is sent",
                                                                   dest_port, src_port);*/
      }
     return;
    /* UDP Protocol */
    case 17:
      udp_hdr = &pack_hdr[offset];
      src_port  |= udp_hdr[0] << 8;
      src_port  |= udp_hdr[1];
      dest_port |= udp_hdr[2] << 8;
      dest_port |= udp_hdr[3];
      if(src_port  == 42  || /* Host Name Server */
         dest_port == 42  || 
         src_port  == 67  || /* DHCP BOOTP Protocol Server */
         dest_port == 67  || 
         src_port  == 68  || /* DHCP BOOTP Protocol Client */
         dest_port == 68  || 
         src_port  == 69  || /* Trivial FTP */
         dest_port == 69  || 
         src_port  == 111  || /* Remote Procedure Call */
         dest_port == 111  || 
         src_port  == 135  || /* NetBIOS & co */
         dest_port == 135  || 
         src_port  == 137  || /* NetBIOS & co*/
         dest_port == 137  || 
         src_port  == 138  || /* NetBIOS & co*/
         dest_port == 138  || 
         src_port  == 139  || /* NetBIOS & co*/
         dest_port == 139  || 
         src_port  == 389  || /* LDAP */
         dest_port == 389  ||
         src_port  == 445  || /* Microsoft-DS */
         dest_port == 445  || 
         src_port  == 1023 || /* network blackjack & co */
         dest_port == 1023 || 
         src_port  == 1026 || /* network blackjack & co */
         dest_port == 1026 || 
         src_port  == 1027 || /* network blackjack & co */
         dest_port == 1027 || 
         src_port  == 1028 || /* network blackjack & co */
         dest_port == 1028 || 
         src_port  == 1029 || /* network blackjack & co */
         dest_port == 1029 
       ){
        *discard = TRUE; /* discard this UDP packet */
        /*TRACE_EVENT_P2("INFO IP FILTER: UDP packet on dest_port %d, src_port %d is discarded",
                                                                   dest_port, src_port);*/
      } else {                                                             
        *discard = FALSE; /* dont't discard this UDP packet */
        /*TRACE_EVENT_P2("INFO IP FILTER: UDP packet on dest_port %d, src_port %d is sent",
                                                                   dest_port, src_port);*/
    }
    return;
    default:
      *discard = TRUE; /* discard all other packets */
      TRACE_EVENT_P1("INFO IP FILTER: protocol type %d is discarded", pack_hdr[9]);
      return;
  }
}
#endif /* _SNDCP_DTI_2_ */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_sig_callback
+------------------------------------------------------------------------------
| Description : callback function for dti lib.
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifdef _SNDCP_DTI_2_
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_sig_callback(U8 instance,
                               U8 interfac,
                               U8 channel,
                               U8 reason,
                               T_DTI2_DATA_IND *dti2_data_ind)
{
  T_SN_DATA_REQ* sn_data_req = NULL;
  T_SN_UNITDATA_REQ* sn_unitdata_req = NULL;
  BOOL ack = FALSE;

  TRACE_FUNCTION("sndcp_sig_callback");

  
  switch(reason)
  {
    case DTI_REASON_CONNECTION_OPENED:
      /*
       * set rx and tx state machine of the given interface to idle state
       */
      /*
       * The nsapi is given here with the parameter 'channel'.
       */
      nu_connection_state(channel, TRUE);
      break;
    case DTI_REASON_CONNECTION_CLOSED:
      /*
       * set the given interface to closed state
       */
      break;
    case DTI_REASON_DATA_RECEIVED:
      /*
       * process received data
       */
      /*
       * Trace p_id.
       */



#ifdef TI_PS_OP_ICUT_SNDCP

      /* PDP_TBR added SNDCP terminal loopback */
      if ( SNDCP_LOOPBACK == 1)
      {
        if (bufFull[channel] == FALSE)
        {
          TRACE_EVENT_P1("LOOPBACK len=%d", dti2_data_ind->desc_list2.list_len);

          dti_send_data(sndcp_data->hDTI,
                    instance, // U8 instance,
                    SNDCP_INTERFACE_UNACK, // U8 interface,
                    channel,// U8 channel,
                    dti2_data_ind // T_DTI2_DATA_IND *dti_data_ind)
                    );
        }
        else
        {
          saveDti2_data_ind[channel] = dti2_data_ind;
        }
        return;
      }
      


#endif /* TI_PS_OP_ICUT_SNDCP */





#ifdef SNDCP_TRACE_ALL
      switch (dti2_data_ind->parameters.p_id) {
      case DTI_PID_IP:
        TRACE_EVENT_P2("dti2_data_ind->parameters.p_id: %02x (%s)",
                       dti2_data_ind->parameters.p_id,
                       "DTI_PID_IP");
        break;
      case DTI_PID_CTCP:
        TRACE_EVENT_P2("dti2_data_ind->parameters.p_id: %02x (%s)",
                       dti2_data_ind->parameters.p_id,
                       "DTI_PID_CTCP");
        break;
      case DTI_PID_UTCP:
        TRACE_EVENT_P2("dti2_data_ind->parameters.p_id: %02x (%s)",
                       dti2_data_ind->parameters.p_id,
                       "DTI_PID_UTCP");
        break;
      case DTI_PID_UOS:
        TRACE_EVENT_P2("dti2_data_ind->parameters.p_id: %02x (%s)",
                       dti2_data_ind->parameters.p_id,
                       "DTI_PID_UOS");
        break;
      default:
        TRACE_EVENT_P2("dti2_data_ind->parameters.p_id: %02x (%s)",
                       dti2_data_ind->parameters.p_id,
                       "unknown");


      }
#endif /* SNDCP_TRACE_ALL */
      /*
       * Discard DTI primitive if the config prim "DISCARD" was received
       */
      sndcp_data->nu = &sndcp_data->nu_base[channel];
      if(sndcp_data->nu->discard){
        TRACE_EVENT_P1("INFO DISCARD: UL packet with length %d is discarded",
                                        dti2_data_ind->desc_list2.list_len );
        sndcp_data->nu->discarded_data += dti2_data_ind->desc_list2.list_len;
        PFREE_DESC2(dti2_data_ind);
        dti2_data_ind = NULL;
        break;
      }
      /*
       * If the IP filter is enabled (config prim "IP_FILTER_ON" was received), 
       * filter the packets.
       */
      if(sndcp_data->ip_filter){
        BOOL discard;
        /* Statistics Trace */
        TRACE_EVENT_P3("INFO NSAPI[%d]: sent bytes: %d, discarded bytes: %d",
          channel, sndcp_data->nu->sent_data, sndcp_data->nu->discarded_data);
        sndcp_ip_packet_filter(&dti2_data_ind->desc_list2, &discard);
        if(discard){
          sndcp_data->nu->discarded_data += dti2_data_ind->desc_list2.list_len;
          PFREE_DESC2(dti2_data_ind);
          dti2_data_ind = NULL;
          break;
        } else {
          sndcp_data->nu->sent_data += dti2_data_ind->desc_list2.list_len;
        }
      } 
      /*
       * Copy dti_data_ind to sn_[unit]data_ind and enter nu service the
       * old way.
       */
      sndcp_get_nsapi_ack(channel, &ack);
      if (ack) {
        /*
         * Allocate a "normal" SN_DATA_REQ primitive and copy the data
         * of the test primitive to that one
         */
        MALLOC(sn_data_req, sizeof(T_SN_DATA_REQ));
        sn_data_req->nsapi = channel;
        sn_data_req->p_id = dti2_data_ind->parameters.p_id;
        sn_data_req->desc_list2 = dti2_data_ind->desc_list2;

        /*
         * Free the received dti primitive.
         */
        PFREE (dti2_data_ind);
        dti2_data_ind = NULL;

        nu_sn_data_req(sn_data_req);

      } else {
        /*
         * Allocate a "normal" SN_UNITDATA_REQ primitive and copy the data
         * of the test primitive to that one
         */
        MALLOC(sn_unitdata_req, sizeof(T_SN_UNITDATA_REQ));

        sn_unitdata_req->nsapi = channel;
        sn_unitdata_req->p_id = dti2_data_ind->parameters.p_id;
        sn_unitdata_req->desc_list2 = dti2_data_ind->desc_list2;

        /*
         * Free the received dti 2 primitive.
         */
        PFREE (dti2_data_ind);
        dti2_data_ind = NULL;

        nu_sn_unitdata_req(sn_unitdata_req);

      }

      break;
    case DTI_REASON_TX_BUFFER_FULL:
      /*
       * set tx state machine of the given interface to TX_IDLE state
       */



#ifdef TI_PS_OP_ICUT_SNDCP

     /* PDP_TBR added SNDCP terminal loopback */
      if (SNDCP_LOOPBACK == 1)
      {
        dti_stop(sndcp_data->hDTI,
                    instance,
                    interfac,
                    channel);
        bufFull[channel] = TRUE;
        TRACE_EVENT("SNDCP BUFFER IS FULL");
      }
      
#endif /* TI_PS_OP_ICUT_SNDCP */



      /*
       * Will be ignored.
       */
      break;
    case DTI_REASON_TX_BUFFER_READY:
      /*
       * set tx state machine of the given interface to TX_READY state
       */




#ifdef TI_PS_OP_ICUT_SNDCP
    /* PDP_TBR added SNDCP terminal loopback */
      if ( SNDCP_LOOPBACK == 1)
      {
        dti_start(sndcp_data->hDTI,
                    instance,
                    interfac,
                    channel);
        bufFull[channel] = FALSE;
        TRACE_ERROR("SNDCP BUFFER IS READY");
        if (saveDti2_data_ind[channel] != NULL)
        {
            TRACE_EVENT_P1("LOOPBACK len=%d", dti2_data_ind->desc_list2.list_len);

            dti_send_data(sndcp_data->hDTI,
                      instance, // U8 instance,
                      SNDCP_INTERFACE_UNACK, // U8 interface,
                      channel,// U8 channel,
                      saveDti2_data_ind[channel] // T_DTI2_DATA_IND *dti_data_ind)
                      );
            saveDti2_data_ind[channel] = NULL;

        }
        break;
      }
#endif /* TI_PS_OP_ICUT_SNDCP */




      /*
       * Reaction will be like reaction to SN_GET[UNIT]DATA_REQ.
       */
      nd_dti_buffer_ready(channel);
      break;
    default:
      TRACE_ERROR("sndcp_sig_callback called with undefined reason");
  }
}

#endif /* CF_FAST_EXEC */


#else /*_SNDCP_DTI_2_*/

#ifndef CF_FAST_EXEC

GLOBAL void sndcp_sig_callback(U8 instance,
                               U8 interfac,
                               U8 channel,
                               U8 reason,
                               T_DTI_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION("sndcp_sig_callback");

  switch(reason)
  {
    case DTI_REASON_CONNECTION_OPENED:
      /*
       * set rx and tx state machine of the given interface to idle state
       */
      /*
       * The nsapi is given here with the parameter 'channel'.
       */
      nu_connection_state(channel, TRUE);
      break;
    case DTI_REASON_CONNECTION_CLOSED:
      /*
       * set the given interface to closed state
       */
      break;
    case DTI_REASON_DATA_RECEIVED:
      /*
       * process received data
       */
      /*
       * Trace p_id.
       */
#ifdef SNDCP_TRACE_ALL
      switch (dti_data_ind->p_id) {
      case DTI_PID_IP:
        TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                       dti_data_ind->p_id,
                       "DTI_PID_IP");
        break;
      case DTI_PID_CTCP:
        TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                       dti_data_ind->p_id,
                       "DTI_PID_CTCP");
        break;
      case DTI_PID_UTCP:
        TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                       dti_data_ind->p_id,
                       "DTI_PID_UTCP");
        break;
      case DTI_PID_FRAME:
        TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                       dti_data_ind->p_id,
                       "DTI_PID_FRAME");
        break;
      default:
        TRACE_EVENT_P2("dti_data_ind->p_id: %02x (%s)",
                       dti_data_ind->p_id,
                       "unknown");


      }
#endif /* SNDCP_TRACE_ALL */
      /*
       * Copy dti_data_ind to sn_[unit]data_ind and enter nu service the
       * old way.
       */
      if (interfac == SNDCP_INTERFACE_ACK) {
        /*
         * Allocate a "normal" SN_DATA_REQ primitive and copy the data
         * of the test primitive to that one
         */
        PALLOC_DESC (sn_data_req, SN_DATA_REQ);

        sn_data_req->nsapi = channel;
        sn_data_req->p_id = dti_data_ind->p_id;
        sn_data_req->desc_list = dti_data_ind->desc_list;

        /*
         * Free the received dti primitive.
         */
        PFREE (dti_data_ind);
        dti_data_ind = NULL;

        nu_sn_data_req(sn_data_req);

      } else {
        /*
         * Allocate a "normal" SN_UNITDATA_REQ primitive and copy the data
         * of the test primitive to that one
         */
        PALLOC_DESC (sn_unitdata_req, SN_UNITDATA_REQ);

        sn_unitdata_req->nsapi = channel;
        sn_unitdata_req->p_id = dti_data_ind->p_id;
        sn_unitdata_req->desc_list = dti_data_ind->desc_list;

        /*
         * Free the received test primitive.
         */
        PFREE (dti_data_ind);
        dti_data_ind = NULL;

        nu_sn_unitdata_req(sn_unitdata_req);

      }

      break;
    case DTI_REASON_TX_BUFFER_FULL:
      /*
       * set tx state machine of the given interface to TX_IDLE state
       */
      /*
       * Will be ignored.
       */
      break;
    case DTI_REASON_TX_BUFFER_READY:
      /*
       * set tx state machine of the given interface to TX_READY state
       */
      /*
       * Reaction will be like reaction to SN_GET[UNIT]DATA_REQ.
       */
      nd_dti_buffer_ready(channel);
      break;
    default:
      TRACE_ERROR("sndcp_sig_callback called with undefined reason");
  }
}
#endif /* CF_FAST_EXEC */

#endif /*_SNDCP_DTI_2_*/

/*
+------------------------------------------------------------------------------
| Function    : sndcp_unset_nsapi_state
+------------------------------------------------------------------------------
| Description : subtracts stat from nsapi_state_ra[nsapi]
|
| Parameters  : UBYTE nsapi, UBYTE stat
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_unset_nsapi_state(UBYTE nsapi,
                                 USHORT stat)
{
  TRACE_FUNCTION("sndcp_unset_nsapi_state");
  {
    sndcp_data->nsapi_state_ra[nsapi] &= ~ stat;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_used
+------------------------------------------------------------------------------
| Description : sets nsapi_used_ra[nsapi] to b
|
| Parameters  : UBYTE nsapi, BOOL b
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_nsapi_used(UBYTE nsapi,
                                 BOOL b)
{
  TRACE_FUNCTION("sndcp_set_nsapi_used");
  {
    sndcp_data->nsapi_used_ra[nsapi] = b;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_sapi_ack
+------------------------------------------------------------------------------
| Description : sets sapi_ack_ra[sapi] to ack
|
| Parameters  : UBYTE sapi, BOOL ack
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_sapi_ack(UBYTE sapi,
                                 BOOL ack)
{
  TRACE_FUNCTION(" sndcp_set_sapi_ack ");
  {
    UBYTE sapi_index = 0;

    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->sapi_ack_ra[sapi_index] = ack;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_snsm_qos_to_ll_qos
+------------------------------------------------------------------------------
| Description : transfers an snsm_qos to an ll_qos
|
| Parameters  : T_snsm_qos, T_ll_qos*
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_snsm_qos_to_ll_qos(T_snsm_qos snsm_qos,
                                     T_ll_qos* ll_qos)
{
  TRACE_FUNCTION("sndcp_snsm_qos_to_ll_qos");
  {
    ll_qos->delay = snsm_qos.delay;
    ll_qos->relclass = snsm_qos.relclass;
    ll_qos->peak = snsm_qos.peak;
    ll_qos->preced = snsm_qos.preced;
    ll_qos->mean = snsm_qos.mean;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_sapi_state
+------------------------------------------------------------------------------
| Description : adds stat to sapi_state_ra[sapi] bitwise
|
| Parameters  : UBYTE sapi, UBYTE stat
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_set_sapi_state(UBYTE sapi,
                                 USHORT stat)
{
  TRACE_FUNCTION(" sndcp_set_sapi_state ");
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->sapi_state_ra[sapi_index] |= stat;
  }
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_unset_sapi_state
+------------------------------------------------------------------------------
| Description : subtracts stat from sapi_state_ra[sapi] bitwise
|
| Parameters  : UBYTE sapi, UBYTE stat
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_unset_sapi_state(UBYTE sapi,
                                   USHORT stat)
{
  TRACE_FUNCTION(" sndcp_unset_sapi_state ");
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->sapi_state_ra[sapi_index] &= ~ stat;
  }
}

#endif /* CF_FAST_EXEC */

#ifdef FLOW_TRACE
/*
+------------------------------------------------------------------------------
| Function    : sndcp_trace_flow_control
+------------------------------------------------------------------------------
| Description : traces flow control
|
| Parameters  : UBYTE entity, UBYTE uplink, UBYTE sent, BOOL close
|
+------------------------------------------------------------------------------
*/
/*
 * 0 GRR
 * 1 LLC
 * 2 SNDCP
 * 3 PPP
 * 4 UART
 */

/*
#define FLOW_TRACE_GRR    0
#define FLOW_TRACE_LLC    1
#define FLOW_TRACE_SNDCP  2
#define FLOW_TRACE_PPP    3
#define FLOW_TRACE_UART   4

direction of data transfer

#define FLOW_TRACE_UP     0
#define FLOW_TRACE_DOWN   1

is sap sitting on top of entity or is lower entity sap used?

#define FLOW_TRACE_TOP    0
#define FLOW_TRACE_BOTTOM 1
*/


#ifndef CF_FAST_EXEC

GLOBAL void sndcp_trace_flow_control(UBYTE entity, UBYTE transfer, UBYTE position, BOOL opened)
{
  TRACE_FUNCTION(" sndcp_trace_flow_control ");
#ifdef SNDCP_TRACE_ALL
  TRACE_EVENT_P4("sndcp_trace_flow_control[%d][%d][%d] = %d",
                 entity,
                 transfer,
                 position,
                 opened);
#endif
  {
    sndcp_data->flow_control_ra[entity][transfer][position] = opened;
  }
}

#endif /* CF_FAST_EXEC */

#endif /* FLOW_TRACE */


#ifdef _SNDCP_DTI_2_

#ifdef SNDCP_TRACE_ALL

/*
+------------------------------------------------------------------------------
| Function    : sndcp_trace_desc_list3_content
+------------------------------------------------------------------------------
| Description : traces content of a desc3 descriptor list
|
| Parameters  : desc_list3 descriptor list
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_trace_desc_list3_content(T_desc_list3 desc_list3)
{
  U16 current_pos = 0;  /*  */
  U16 data_len = 0;     /* The length of the data to be traced including data offset */
  U8* p_data = NULL;    /* Pointer to byte data element */
  T_desc3* p_desc3 = (T_desc3*)desc_list3.first; /* Pointer to the actual desc3 descriptor element */

  while(p_desc3 != NULL)
  {
    current_pos = p_desc3->offset; /* In case it is the sndcp header allocation, ENCODE_OFFSET has been considered*/
    p_data = (U8*)p_desc3->buffer;
    data_len = current_pos + p_desc3->len; /* p_desc3->len is the total allocation length for sndcp header description */
    while(current_pos <= data_len)
    {
      TRACE_EVENT_P1
        ("%02x",
          p_data[current_pos]
        );
      current_pos ++;
    }

    p_desc3 = (T_desc3*)p_desc3->next;
  }
}

#endif /* CF_FAST_EXEC */

#endif /* SNDCP_TRACE_ALL */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_cl_desc2_attach
+------------------------------------------------------------------------------
| Description : This function attaches a decriptor list to already allocated
|               memory. There is no return value. In case an attempt is made
|               to attach to freed or invalid memory, then the FRAME generates
|               a trace SYSTEM ERROR. This function does not allocate any new
|               memory.
|
| Parameters  : T_desc2* pointer to desc2 descriptor.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

void sndcp_cl_desc2_attach(T_desc2* p_desc2)
{

  MATTACH(p_desc2);

}

#endif /* CF_FAST_EXEC */
/*
+------------------------------------------------------------------------------
| Function    : sndcp_cl_desc3_free
+------------------------------------------------------------------------------
| Description : Frees the descriptor connected to the desc3 descriptor. This
|               free will when applicable cause the frame to decrease a
|               connected counter attached to the allocation or really free
|               the memory in case the counter is zero.
|
| Parameters  : T_desc3* pointer to desc3 descriptor.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

void sndcp_cl_desc3_free(T_desc3* p_desc3)
{

  MFREE(p_desc3->buffer);
  p_desc3->buffer = NULL;
}

#endif /* CF_FAST_EXEC */

#endif /* _SNDCP_DTI_2_ */


#ifdef SNDCP_TRACE_ALL

/*
+------------------------------------------------------------------------------
| Function    : sndcp_trace_sdu
+------------------------------------------------------------------------------
| Description : traces content of one sdu
|
| Parameters  : pointer to sdu
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_trace_sdu(T_sdu* sdu)
{

  USHORT pos = sdu->o_buf >> 3;
  USHORT  frame_len = (sdu->l_buf + 7) / 8;


  TRACE_FUNCTION("sndcp_trace_sdu");

  while(pos < (frame_len + (sdu->o_buf >> 3)))
  {
    if (pos + 8 <= (frame_len + (sdu->o_buf >> 3))) {
      TRACE_EVENT_P8
        ("%02x %02x %02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5],
          sdu->buf[pos + 6],
          sdu->buf[pos + 7]
        );
      pos += 8;
    } else if (pos + 7 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P7
        ("%02x %02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5],
          sdu->buf[pos + 6]
        );
      pos += 7;
    } else if (pos + 6 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P6
        ("%02x %02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4],
          sdu->buf[pos + 5]
        );
      pos += 6;
    } else if (pos + 5 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P5
        ("%02x %02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3],
          sdu->buf[pos + 4]
        );
      pos += 5;
    } else if (pos + 4 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P4
        ("%02x %02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2],
          sdu->buf[pos + 3]
        );
      pos += 4;
    } else if (pos + 3 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P3
        ("%02x %02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1],
          sdu->buf[pos + 2]
        );
      pos += 3;
    } else if (pos + 2 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P2
        ("%02x %02x",
          sdu->buf[pos],
          sdu->buf[pos + 1]
        );
      pos += 2;
    } else if (pos + 1 <= (frame_len + (sdu->o_buf >> 3))){
      TRACE_EVENT_P1
        ("%02x",
          sdu->buf[pos]
        );
      pos++;
    }

  }
}

#endif /* CF_FAST_EXEC */


#endif /* SNDCP_TRACE_ALL */

#ifdef  SNDCP_TRACE_BUFFER

/*
+------------------------------------------------------------------------------
| Function    : sndcp_trace_desc_list
+------------------------------------------------------------------------------
| Description : traces content of one desc_list
|
| Parameters  : pointer to desc_list
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_trace_desc_list(T_desc_list2* desc_list)
{
#else /*_SNDCP_DTI_2_*/
GLOBAL void sndcp_trace_desc_list(T_desc_list* desc_list)
{
#endif /*_SNDCP_DTI_2_*/
  USHORT  frame_len = desc_list->list_len;
#ifdef _SNDCP_DTI_2_
  T_desc2* desc = (T_desc2*)desc_list->first;
  USHORT  desc_pos  = desc->offset;
  USHORT  desc_end  = desc->len + desc->offset;
#else /*_SNDCP_DTI_2_*/
  T_desc* desc = (T_desc*)desc_list->first;
  USHORT  desc_pos = 0;
  USHORT  desc_end  = desc->len;
#endif /*_SNDCP_DTI_2_*/
  USHORT  list_pos = 0;

  TRACE_FUNCTION("sndcp_trace_desc_list");

  while(list_pos < frame_len)
  {
    if (desc != NULL) {
      if (desc_pos >= desc_end) {
#ifdef _SNDCP_DTI_2_
        desc = (T_desc2*)desc->next;
        desc_pos = desc->offset;
        desc_end = desc->len + desc->offset;
#else /*_SNDCP_DTI_2_*/
        desc = (T_desc*)desc->next;
        desc_pos = 0;
        desc_end = desc->len;
#endif /*_SNDCP_DTI_2_*/
      }
    }
    if (desc == NULL) {
      return;
    }
    if (desc_pos + 8 <= desc_end) {
      TRACE_EVENT_P8 ("%02x %02x %02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5],
                      desc->buffer[desc_pos + 6],
                      desc->buffer[desc_pos + 7]
                     );
      list_pos+= 8;
      desc_pos+= 8;
    } else if (desc_pos + 7 <= desc_end) {
      TRACE_EVENT_P7 ("%02x %02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5],
                      desc->buffer[desc_pos + 6]
                     );
      list_pos+= 7;
      desc_pos+= 7;
    } else if (desc_pos + 6 <= desc_end) {
      TRACE_EVENT_P6 ("%02x %02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4],
                      desc->buffer[desc_pos + 5]
                     );
      list_pos+= 6;
      desc_pos+= 6;
    } else if (desc_pos + 5 <= desc_end) {
      TRACE_EVENT_P5 ("%02x %02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3],
                      desc->buffer[desc_pos + 4]
                     );
      list_pos+= 5;
      desc_pos+= 5;
    } else if (desc_pos + 4 <= desc_end) {
      TRACE_EVENT_P4 ("%02x %02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2],
                      desc->buffer[desc_pos + 3]
                     );
      list_pos+= 4;
      desc_pos+= 4;
    } else if (desc_pos + 3 <= desc_end) {
      TRACE_EVENT_P3 ("%02x %02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1],
                      desc->buffer[desc_pos + 2]
                     );
      list_pos+= 3;
      desc_pos+= 3;
    } else if (desc_pos + 2 <= desc_end) {
      TRACE_EVENT_P2 ("%02x %02x ",
                      desc->buffer[desc_pos],
                      desc->buffer[desc_pos + 1]
                     );
      list_pos+= 2;
      desc_pos+= 2;
    } else if (desc_pos + 1 <= desc_end) {
      TRACE_EVENT_P1 ("%02x ",
                      desc->buffer[desc_pos]
                     );
      list_pos++;
      desc_pos++;
    }

    /* break tracing if payload shall not be traced*/
    if(!sndcp_data->trace_ip_datagram && (list_pos >= 40))
      break;

  } /* while(list_pos < frame_len) */

}

#endif /* CF_FAST_EXEC */

#endif /* SNDCP_TRACE_BUFFER */



#ifdef _SNDCP_MEAN_TRACE_
/*
+------------------------------------------------------------------------------
| Function    : sndcp_sn_count_req
+------------------------------------------------------------------------------
| Description : reaction to prim SN_COUNT_REQ
|
| Parameters  : primitive payload
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sndcp_sn_count_req(T_SN_COUNT_REQ* sn_count_req)
{
  TRACE_FUNCTION("sndcp_sn_count_req");

  {
    BOOL ack = FALSE;
    PALLOC(sn_count_cnf, SN_COUNT_CNF);

    sn_count_cnf->nsapi = sn_count_req->nsapi;

    sndcp_get_nsapi_ack(sn_count_req->nsapi, &ack);
    if (ack) {
      sn_count_cnf->octets_uplink =
        sndcp_data->cur_num_uplink_ack[sn_count_req->nsapi];
      sn_count_cnf->octets_downlink =
        sndcp_data->cur_num_downlink_ack[sn_count_req->nsapi];

      sn_count_cnf->packets_uplink =
        sndcp_data->cur_pac_uplink_ack[sn_count_req->nsapi];
      sn_count_cnf->packets_downlink =
        sndcp_data->cur_pac_downlink_ack[sn_count_req->nsapi];

#ifdef SNDCP_UPM_INCLUDED 
      if (sn_count_req->reset == NAS_RESET_YES) {
#else 
      if (sn_count_req->reset == SN_RESET_YES) { 
#endif   /*SNDCP_UPM_INCLUDED*/
        sndcp_data->cur_num_uplink_ack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_num_downlink_ack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_pac_uplink_ack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_pac_downlink_ack[sn_count_req->nsapi] = 0;
      }

    } else {

      sn_count_cnf->octets_uplink =
        sndcp_data->cur_num_uplink_unack[sn_count_req->nsapi];
      sn_count_cnf->octets_downlink =
        sndcp_data->cur_num_downlink_unack[sn_count_req->nsapi];

      sn_count_cnf->packets_uplink =
        sndcp_data->cur_pac_uplink_unack[sn_count_req->nsapi];
      sn_count_cnf->packets_downlink =
        sndcp_data->cur_pac_downlink_unack[sn_count_req->nsapi];

#ifdef SNDCP_UPM_INCLUDED 
      if (sn_count_req->reset == NAS_RESET_YES) {
#else 
      if (sn_count_req->reset == SN_RESET_YES) {
#endif  /*SNDCP_UPM_INCLUDED*/
        sndcp_data->cur_num_uplink_unack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_num_downlink_unack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_pac_uplink_unack[sn_count_req->nsapi] = 0;
        sndcp_data->cur_pac_downlink_unack[sn_count_req->nsapi] = 0;
      }
    }

#ifdef SNDCP_UPM_INCLUDED 
	PSEND(hCommUPM, sn_count_cnf);  /*count_cnf goes via UPM*/
#else
    PSEND(hCommMMI, sn_count_cnf);
#endif  /*SNDCP_UPM_INCLUDED*/

  }

  PFREE(sn_count_req);
}

#endif /* CF_FAST_EXEC */

#endif /*_SNDCP_MEAN_TRACE */

#ifdef SNDCP_TRACE_IP_DATAGRAM

/*
+------------------------------------------------------------------------------
| Function    : sndcp_trace_ip_datagram
+------------------------------------------------------------------------------
| Description : traces content of one desc_list
|
| Parameters  : pointer to desc_list
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_trace_ip_datagram(T_desc_list2* desc_list)
{
#else /*_SNDCP_DTI_2_*/
GLOBAL void sndcp_trace_ip_datagram(T_desc_list* desc_list)
{
#endif /*_SNDCP_DTI_2_*/
  USHORT  frame_len = desc_list->list_len;
#ifdef _SNDCP_DTI_2_
  T_desc2* desc = (T_desc2*)desc_list->first;
  USHORT  desc_pos = desc->offset;
  USHORT  desc_end  = desc->len + desc->offset;
#else /*_SNDCP_DTI_2_*/
  T_desc* desc = (T_desc*)desc_list->first;
  USHORT  desc_pos = 0;
  USHORT  desc_end  = desc->len;
#endif /*_SNDCP_DTI_2_*/
  USHORT  list_pos = 0;
  UBYTE   *pack_hdr, *ip_hdr;
  USHORT  t_length   = 0;
  USHORT  length     = 0;
  USHORT  pid        = 0;
  USHORT  f_offset   = 0;
  USHORT  h_checksum = 0;
  USHORT  src_port   = 0;
  USHORT  dest_port  = 0;
  ULONG   seq_num    = 0;
  ULONG   ack_num    = 0;
  USHORT  window     = 0;
  USHORT  checksum   = 0;
  USHORT  urg_ptr    = 0;
  USHORT  ip_hlen    = 0; /* IP header length */
  USHORT  tcp_hlen   = 0; /* TCP header length */
  UBYTE   tmp_flag   = 0;
  UBYTE   type       = 0;
  UBYTE   code       = 0;
  /*TRACE_FUNCTION("sndcp_trace_ip_datagram");*/

  if(!sndcp_data->trace_ip_datagram && !sndcp_data->trace_ip_header){
    return;
  }

  while(list_pos < frame_len)
  {
    if (desc != NULL) {
      if (desc_pos >= desc_end) {
#ifdef _SNDCP_DTI_2_
        desc = (T_desc2*)desc->next;
        desc_pos = desc->offset;
        desc_end = desc->len + desc->offset;
#else /*_SNDCP_DTI_2_*/
        desc = (T_desc*)desc->next;
        desc_pos = 0;
        desc_end = desc->len;
#endif /*_SNDCP_DTI_2_*/
        if (desc == NULL) {
          break;
        }
      }
    } else {
      return;
    }
    if((list_pos == 0) || (list_pos >= t_length))
    {
      ip_hdr = &desc->buffer[desc_pos];
      if((ip_hdr[9] != 1) && (ip_hdr[9] != 6) && (ip_hdr[9] != 17)) {
        TRACE_EVENT_P1("INFO TRACE: Tracing of protocol type %d not supported",
                                                                    ip_hdr[9]);
        return;
      }

      /* Trace IP Header */
      ip_hlen = (ip_hdr[0] & 0x0F) << 2;
      if ((desc_pos + ip_hlen) <= desc_end){
        TRACE_EVENT("IP Header");
        TRACE_EVENT_P1("  Protocol Version:         %d",
                      ((ip_hdr[0] & 0xF0) >> 4));
        TRACE_EVENT_P1("  Header Length:            %d", (ip_hlen >> 2));
        TRACE_EVENT_P1("  Type Of Service:          %d", ip_hdr[1]);
        t_length |= ip_hdr[2] << 8;
        t_length |= ip_hdr[3];
        TRACE_EVENT_P1("  Total Length:             %d", t_length);
        pid |= ip_hdr[4] << 8;
        pid |= ip_hdr[5];
        TRACE_EVENT_P1("  Packet ID:                %d", pid);
        TRACE_EVENT_P2("  MF|DF:                    %d|%d",
                            (ip_hdr[6] & 0x20) >> 5,
                            (ip_hdr[6] & 0x40) >> 6);
        f_offset |= (ip_hdr[6] & 0x1F) << 8;
        f_offset |= ip_hdr[7];
        TRACE_EVENT_P1("  Fragment offset:          %d", f_offset);
        TRACE_EVENT_P1("  Time To Live:             %d", ip_hdr[8]);
        if(ip_hdr[9] == 17) {
          TRACE_EVENT_P1("  Protocol:                 UDP(%d)",
                                             ip_hdr[9]);
        } else if (ip_hdr[9] == 6) {
          TRACE_EVENT_P1("  Protocol:                 TCP(%d)",
                                             ip_hdr[9]);
        } else {
          TRACE_EVENT_P1("  Protocol:                 %d",
                                             ip_hdr[9]);
        }
        h_checksum |= ip_hdr[10] << 8;
        h_checksum |= ip_hdr[11];
        TRACE_EVENT_P1("  Header Checksum:          %d", h_checksum);
        TRACE_EVENT_P4("  Source Address:           %d.%d.%d.%d",
                          ip_hdr[12], ip_hdr[13],
                          ip_hdr[14], ip_hdr[15]);
        TRACE_EVENT_P4("  Destination Address:      %d.%d.%d.%d",
                          ip_hdr[16], ip_hdr[17],
                          ip_hdr[18], ip_hdr[19]);
        list_pos+= ip_hlen;
        desc_pos+= ip_hlen;
      }
      /* Trace TCP Header */
      if((ip_hdr[9] == 6) && ((desc_pos + 20) <= desc_end))
      {
        TRACE_EVENT("TCP Header");
        pack_hdr = &ip_hdr[ip_hlen];
        tcp_hlen = (pack_hdr[12] & 0xF0) >> 2; /* the same as (x>>4)*4 */
        src_port |= pack_hdr[0] << 8;
        src_port |= pack_hdr[1];
        TRACE_EVENT_P1("  Source Port:              %d", src_port);
        dest_port |= pack_hdr[2] << 8;
        dest_port |= pack_hdr[3];
        TRACE_EVENT_P1("  Destination Port:         %d", dest_port);
        seq_num |= pack_hdr[4] << 24;
        seq_num |= pack_hdr[5] << 16;
        seq_num |= pack_hdr[6] << 8;
        seq_num |= pack_hdr[7];
        TRACE_EVENT_P1("  Sequence Number:          %u", seq_num);
        ack_num |= pack_hdr[8] << 24;
        ack_num |= pack_hdr[9] << 16;
        ack_num |= pack_hdr[10] << 8;
        ack_num |= pack_hdr[11];
        TRACE_EVENT_P1("  Acknowledgment Number:    %u", ack_num);
        TRACE_EVENT_P1("  Data Offset:              %d", (tcp_hlen >> 2));
        TRACE_EVENT_P6("  URG|ACK|PSH|RST|SYN|FIN:  %d|%d|%d|%d|%d|%d",
                                       (pack_hdr[13] & 0x20) >> 5,
                                       (pack_hdr[13] & 0x10) >> 4,
                                       (pack_hdr[13] & 0x08) >> 3,
                                       (pack_hdr[13] & 0x04) >> 2,
                                       (pack_hdr[13] & 0x02) >> 1,
                                       (pack_hdr[13] & 0x01)    );
        window |= pack_hdr[14] << 8;
        window |= pack_hdr[15];
        TRACE_EVENT_P1("  Window Size:              %d", window);
        checksum |= pack_hdr[16] << 8;
        checksum |= pack_hdr[17];
        TRACE_EVENT_P1("  Checksum:                 %d", checksum);
        urg_ptr |= pack_hdr[18] << 8;
        urg_ptr |= pack_hdr[19];
        TRACE_EVENT_P1("  Ungent Pointer:           0x%04x", urg_ptr);
        list_pos+= tcp_hlen;
        desc_pos+= tcp_hlen;
      }
      /* Trace UDP Header */
      else if((ip_hdr[9] == 17) && ((desc_pos + 8) <= desc_end))
      {
        TRACE_EVENT("UDP Header");
        pack_hdr = &ip_hdr[ip_hlen];
        src_port |= pack_hdr[0] << 8;
        src_port |= pack_hdr[1];
        TRACE_EVENT_P1("  Source Port:              %d", src_port);
        dest_port |= pack_hdr[2] << 8;
        dest_port |= pack_hdr[3];
        TRACE_EVENT_P1("  Destination Port:         %d", dest_port);
        length |= pack_hdr[4] << 8;
        length |= pack_hdr[5];
        TRACE_EVENT_P1("  Length:                   %d", length);
        checksum |= pack_hdr[6] << 8;
        checksum |= pack_hdr[7];
        TRACE_EVENT_P1("  Checksum:                 %d", checksum);
        list_pos+= 8;
        desc_pos+= 8;
      }
      /* Trace ICMP Header */
      else if((ip_hdr[9] == 1) && ((desc_pos + 8) <= desc_end))
      {
        TRACE_EVENT("ICMP Header");
        pack_hdr = &ip_hdr[ip_hlen];
        type = pack_hdr[0];
        TRACE_EVENT_P1("  Message Type:             %d", type);
        code = pack_hdr[1];
        TRACE_EVENT_P1("  Message Code:             %d", code);
        checksum |= pack_hdr[2] << 8;
        checksum |= pack_hdr[3];
        TRACE_EVENT_P1("  Checksum:                 %d", checksum);
        list_pos+= 8;
        desc_pos+= 8;
      }
    }/* if((list_pos == 0) || (list_pos >= t_length)) */
    /* Shall payload be traced? */
    if(!sndcp_data->trace_ip_datagram){
      break;
    }
  /* Trace Payload */
    if(!tmp_flag && (desc_pos < frame_len)){
      TRACE_EVENT("Payload");
      tmp_flag = 1;
    }
    /*lint -e661 -e662 possible access/creation of out-of-bounds pointer*/	
    if (desc_pos + 16 <= desc_end) {
      TRACE_EVENT_P16 ("  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6], desc->buffer[desc_pos+7], desc->buffer[desc_pos+8], 
        desc->buffer[desc_pos+9], desc->buffer[desc_pos+10], desc->buffer[desc_pos+11], 
        desc->buffer[desc_pos+12], desc->buffer[desc_pos+13], desc->buffer[desc_pos+14],
        desc->buffer[desc_pos+15]);
      list_pos+= 16;
      desc_pos+= 16;
    } else if (desc_pos + 14 <= desc_end) {
      TRACE_EVENT_P14 ("  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6], desc->buffer[desc_pos+7], desc->buffer[desc_pos+8], 
        desc->buffer[desc_pos+9], desc->buffer[desc_pos+10], desc->buffer[desc_pos+11], 
        desc->buffer[desc_pos+12], desc->buffer[desc_pos+13]);
      list_pos+= 14;
      desc_pos+= 14;
    } else if (desc_pos + 12 <= desc_end) {
      TRACE_EVENT_P12("  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6], desc->buffer[desc_pos+7], desc->buffer[desc_pos+8], 
        desc->buffer[desc_pos+9], desc->buffer[desc_pos+10], desc->buffer[desc_pos+11]);
      list_pos+= 12;
      desc_pos+= 12;
    } else if (desc_pos + 10 <= desc_end) {
      TRACE_EVENT_P10("  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6], desc->buffer[desc_pos+7], desc->buffer[desc_pos+8], 
        desc->buffer[desc_pos+9]);
      list_pos+= 10;
      desc_pos+= 10;
    } else if (desc_pos + 8 <= desc_end) {
      TRACE_EVENT_P8 ("  %02x %02x %02x %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6], desc->buffer[desc_pos+7]);
      list_pos+= 8;
      desc_pos+= 8;
    } else if (desc_pos + 7 <= desc_end) {
      TRACE_EVENT_P7 ("  %02x %02x %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5], 
        desc->buffer[desc_pos+6]);
      list_pos+= 7;
      desc_pos+= 7;
    } else if (desc_pos + 6 <= desc_end) {
      TRACE_EVENT_P6 ("  %02x %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4], desc->buffer[desc_pos+5]);
      list_pos+= 6;
      desc_pos+= 6;
    } else if (desc_pos + 5 <= desc_end) {
      TRACE_EVENT_P5 ("  %02x %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], desc->buffer[desc_pos+2], 
        desc->buffer[desc_pos+3], desc->buffer[desc_pos+4]);
      list_pos+= 5;
      desc_pos+= 5;
    } else if (desc_pos + 4 <= desc_end) {
      TRACE_EVENT_P4 ("  %02x %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], 
        desc->buffer[desc_pos+2], desc->buffer[desc_pos+3]);
      list_pos+= 4;
      desc_pos+= 4;
    } else if (desc_pos + 3 <= desc_end) {
      TRACE_EVENT_P3 ("  %02x %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1], 
        desc->buffer[desc_pos+2]);
      list_pos+= 3;
      desc_pos+= 3;
    } else if (desc_pos + 2 <= desc_end) {
      TRACE_EVENT_P2 ("  %02x %02x ",
        desc->buffer[desc_pos+0], desc->buffer[desc_pos+1]);
      list_pos+= 2;
      desc_pos+= 2;
    } else if (desc_pos + 1 <= desc_end) {
      TRACE_EVENT_P1 ("  %02x ", desc->buffer[desc_pos+0]);
      list_pos++;
      desc_pos++;
    }
  } /* while(list_pos < frame_len) */
    /*lint -e661 -e662 possible access/creation of out-of-bounds pointer*/
}
/*
+------------------------------------------------------------------------------
| Function    : sndcp_bin_trace_ip
+------------------------------------------------------------------------------
| Description : traces content of one desc_list using TRACE_IP macro
|
| Parameters  : pointer to desc_list
|               direction: SNDCP_UL_PACKET (0x1)
|                          SNDCP_DL_PACKET (0x2)
|
+------------------------------------------------------------------------------
*/
#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_bin_trace_ip(T_desc_list2* desc_list, U8 direction)
{
#else /*_SNDCP_DTI_2_*/
GLOBAL void sndcp_bin_trace_ip(T_desc_list* desc_list, U8 direction)
{
#endif /*_SNDCP_DTI_2_*/
  USHORT  frame_len = desc_list->list_len;
#ifdef _SNDCP_DTI_2_
  T_desc2* desc = (T_desc2*)desc_list->first;
#else /*_SNDCP_DTI_2_*/
  T_desc* desc = (T_desc*)desc_list->first;
#endif /*_SNDCP_DTI_2_*/

  U16 pid = 0;
  U16 pos = 0;

  TRACE_FUNCTION("sndcp_bin_trace_ip");

  if(frame_len > 1500){
    TRACE_EVENT("TRACE IP ERROR: IP packet too long.");
    return;
  }

  while(desc != NULL)
  {
#ifdef _SNDCP_DTI_2_
    memcpy(&bin_trace_ip_buf[pos], (U8*)&desc->buffer[desc->offset], desc->len );
    pos += desc->len;
    desc = (T_desc2*)desc->next;
#else /*_SNDCP_DTI_2_*/
    memcpy(&bin_trace_ip_buf[pos], (U8*)&desc->buffer[0], desc->len );
    pos += desc->len;
    desc = (T_desc*)desc->next;
#endif /*_SNDCP_DTI_2_*/
  }

#ifdef TRACE_IP
  if(direction == SNDCP_DL_PACKET){
    TRACE_IP(SNDCP_handle, SNDCP_handle, DOWNLINK_OPC, \
                    (U8*)&bin_trace_ip_buf[0], frame_len);
    pid = bin_trace_ip_buf[5] | (bin_trace_ip_buf[4] << 8); 
    TRACE_EVENT_P1("TRACE IP INFO: Downlink IP Packet, ID: %d", pid);
  } else if (direction == SNDCP_UL_PACKET){
    TRACE_IP(SNDCP_handle, SNDCP_handle, UPLINK_OPC, \
                  (U8*)&bin_trace_ip_buf[0], frame_len);
    pid = bin_trace_ip_buf[5] | (bin_trace_ip_buf[4] << 8); 
    TRACE_EVENT_P1("TRACE IP INFO: Uplink IP Packet, ID: %d", pid);
  } else {
    TRACE_EVENT("TRACE IP ERROR: unknown transfer direction.");
  }
#else /*TRACE_IP*/
  TRACE_EVENT("SNDCP WARNING: TRACE_IP macro not defined");
#endif /*TRACE_IP*/ 
}


/*
+------------------------------------------------------------------------------
| Function    : sndcp_default_ip_trace
+------------------------------------------------------------------------------
| Description : traces TCP, UDP or ICMP header
|
| Parameters  : pointer to desc_list
|               direction: SNDCP_UL_PACKET (0x1)
|                          SNDCP_DL_PACKET (0x2)
|
+------------------------------------------------------------------------------
*/
#ifdef _SNDCP_DTI_2_
GLOBAL void sndcp_default_ip_trace(T_desc_list2 *desc_list, U8 direction)
#else
GLOBAL void sndcp_default_ip_trace(T_desc_list *desc_list, U8 direction)
#endif
{
#ifdef _SNDCP_DTI_2_  
  T_desc2 *desc = (T_desc2 *)desc_list->first;
  U8  *ip_hdr = &desc->buffer[desc->offset];
#else
  T_desc *desc  = (T_desc *)desc_list->first;
  U8  *ip_hdr = &desc->buffer[0];
#endif
  U8  *pk_hdr = NULL; 
  U16  ip_len   = 0;
  U16  icmp_seq = 0;
  U32  seq_num  = 0,
       ack_num  = 0;
  U16  t_length = 0;
  U16  pid      = 0;
  U8   type, code;
  U16  win_size = 0;

  TRACE_FUNCTION("sndcp_default_ip_trace");

  if(desc_list == NULL || desc_list->first == NULL || desc_list->list_len == 0){
    return; 
  }

  ip_len = (ip_hdr[0] & 0x0f) * 4; /* IP header length in bytes*/
  if(desc->len < ip_len){
    TRACE_EVENT("IP TRACE: can not trace ip header, desc too small");
    return;
  }

  pk_hdr = &ip_hdr[ip_len];        /* Pointer to the header on top of IP hdr */  
  t_length |= ip_hdr[2] << 8;      /* Total packet length */
  t_length |= ip_hdr[3];    
  pid |= ip_hdr[4] << 8;           /* Unequal packet ID */
  pid |= ip_hdr[5];

  switch(ip_hdr[9]){
    /* ICMP Protocol */
    case 1:
      if(desc->len < (ip_len + 8)){
        TRACE_EVENT("IP TRACE: can not trace icmp header, desc to small");
        return;
      }
      /* Trace ICMP Header */
      type  = pk_hdr[0];
      if(type == 8 || type == 0){
        icmp_seq |= pk_hdr[6];
        icmp_seq |= pk_hdr[7] << 8;
        TRACE_EVENT_P4("ICMP Echo: type %d, len %d, pid %d, icmp_seq %d", 
                                          type, t_length, pid, icmp_seq);
      } else {
        code = pk_hdr[1];
        TRACE_EVENT_P5("ICMP Packet: dir %d, len %d, pid %d, type: %d, code: %d", 
                                           direction, t_length, pid, type, code);
      }      
     break;
    /* TCP Protocol */
    case 6:
      if(desc->len < (ip_len + 20)){
        TRACE_EVENT("IP TRACE: can not trace tcp header, desc to small");
        return;
      }
      seq_num |= pk_hdr[4] << 24;
      seq_num |= pk_hdr[5] << 16;
      seq_num |= pk_hdr[6] << 8;
      seq_num |= pk_hdr[7];
      ack_num |= pk_hdr[8] << 24;
      ack_num |= pk_hdr[9] << 16;
      ack_num |= pk_hdr[10] << 8;
      ack_num |= pk_hdr[11];
      win_size |= pk_hdr[14] << 8;
      win_size |= pk_hdr[15];

      TRACE_EVENT_P14("TCP Packet: dir %d, len %d, pid %d, seq_num %u, ack_num %u, src_ip %d.%d.%d.%d, dst_ip %d.%d.%d.%d, win_size %d", 
                                      direction, t_length, pid, seq_num, ack_num, 
                                      ip_hdr[12], ip_hdr[13], ip_hdr[14], ip_hdr[15],
                                      ip_hdr[16], ip_hdr[17], ip_hdr[18], ip_hdr[19], win_size);
        break;
    /* UDP Protocol */
    case 17:
      if(desc->len < (ip_len + 8)){
        TRACE_EVENT("IP TRACE: can not trace udp header, desc to small");
        return;
      }
      TRACE_EVENT_P3("UDP Packet: dir %d, len %d, pid %d", 
                                 direction, t_length, pid);
     break;
    default:
        TRACE_EVENT_P1("INFO TRACE: Tracing of protocol type %d not supported",
                                                                    ip_hdr[9]);
      break;
  }

  return;
}

#endif /* CF_FAST_EXEC */
#endif /* SNDCP_TRACE_IP_DATAGRAM */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_swap2
+------------------------------------------------------------------------------
| Description : This routine converts (2 byte) short n from network byte order
|               to host byte order.
|
| Parameters  : USHORT n
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL USHORT sndcp_swap2(USHORT n)
{
  USHORT tmp = n;
  return n = (((tmp & 0xff00) >> 8) | ((tmp & 0x00ff) << 8) );
}

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_swap4
+------------------------------------------------------------------------------
| Description : This routine converts (4 byte) long n from network byte order
|               to host byte order.
|
| Parameters  : ULONG n
|
+------------------------------------------------------------------------------
*/

#ifndef CF_FAST_EXEC

GLOBAL ULONG sndcp_swap4(ULONG n) {
  ULONG tmp = n;
  return n = ((tmp & 0xff000000) >> 24) | ((tmp & 0x00ff0000) >> 8 ) |
             ((tmp & 0x0000ff00) << 8 ) | ((tmp & 0x000000ff) << 24);
}

#endif /* CF_FAST_EXEC */
/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_nsapi_rec_state
+------------------------------------------------------------------------------
| Description : The function returns the receiving state for given NSAPI
|
| Parameters  : IN  : nsapi
|               OUT : state 
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
GLOBAL U8 sndcp_get_nsapi_rec_state (U8 nsapi)
{
  TRACE_FUNCTION( "sndcp_get_nsapi_rec_state" );

  return sndcp_data->rec_states[nsapi];

} /* sndcp_get_nsapi_rec_state() */
#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_nsapi_rec_state
+------------------------------------------------------------------------------
| Description : The function sets the receiving state for given NSAPI
|
| Parameters  : IN : nsapi
|               IN : state 
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
GLOBAL void sndcp_set_nsapi_rec_state (U8 nsapi, U8 state)
{
  TRACE_FUNCTION( "sndcp_set_nsapi_rec_state" );

  sndcp_data->rec_states[nsapi] = state;

} /* sndcp_set_nsapi_rec_state() */
#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_npdu_num
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Gets the N-PDU number from the segment header.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*, npdu_num*
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
LOCAL void sndcp_get_npdu_num (T_LL_UNITDATA_IND* ll_unitdata_ind, USHORT* npdu_num)
{ 
  USHORT msn_off = 1;
  USHORT lsb_off = 2;
  UBYTE msn = 0;
  UBYTE lsb = 0;

  if ((ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8]) & 0x40) {
    msn_off = 2;
    lsb_off = 3;
  }

  msn =
    ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8) + msn_off];
  lsb = 
    ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8) + lsb_off];

  *npdu_num = ((msn & 0xf) << 8) + lsb;
} /* sndcp_get_npdu_num() */
#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_get_seg_num
+------------------------------------------------------------------------------
| Description : E X T R A convenience function, not in SDL.
|               Gets the NSAPI from the segment header.
|
| Parameters  :  ll_unitdata_ind T_LL_UNITDATA_IND*, seg_num*
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
LOCAL void sndcp_get_seg_num (T_LL_UNITDATA_IND* ll_unitdata_ind, UBYTE* seg_num)
{ 
  *seg_num = (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8) + 2] >> 4);
} /* sd_get_seg_num() */
#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_unack_transfer_params
+------------------------------------------------------------------------------
| Description : There is one N-PDU, whose segments are sent to CIA one by one,
| and that is expected reassembled and decompressed back from CIA. Some
| informations are only given in the first segment. 
| This procedure stores the needed elements in service variables that can
| be read before sending a SIG_SD_CIA_TRANSFER_REQ.
|
| Parameters  : ll_unitdata_req*
| Pre         : Correct instance of sd service must be activated.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
GLOBAL void sndcp_set_unack_transfer_params (T_LL_UNITDATA_IND* ll_unitdata_ind) 
{ 

  UBYTE nsapi = 
           (ll_unitdata_ind->sdu.buf[(ll_unitdata_ind->sdu.o_buf / 8)]) & 0xf;

  /*
   * The dcomp value in the first segment of the currently reassembled N-PDU.
   */
  sndcp_data->cur_dcomp[nsapi] = 
   (ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8 + 1] & 0xf0) >> 4 ;
  /*
   * The pcomp value in the first segment of the currently reassembled N-PDU.
   */
  sndcp_data->cur_pcomp[nsapi] = 
          (ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8 + 1]) & 0xf;
  /*
   * Reference to N-PDU.
   */
  sndcp_data->cur_pdu_ref[nsapi].ref_nsapi = nsapi;
  sndcp_get_npdu_num(ll_unitdata_ind, &sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num);
  sndcp_get_seg_num(ll_unitdata_ind, &sndcp_data->cur_pdu_ref[nsapi].ref_seg_num);
  /*
   * First and/or last segment?
   */
  sndcp_data->cur_seg_pos[nsapi] = 0;
  /* 
   * if f bit is set 
   */
  if ((ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8] & 0x40)) {
    sndcp_data->cur_seg_pos[nsapi] += SEG_POS_FIRST;
  }
  /* 
   * if m bit is not set 
   */
  if (!(ll_unitdata_ind->sdu.buf[ll_unitdata_ind->sdu.o_buf / 8] & 0x10)) {
    sndcp_data->cur_seg_pos[nsapi] += SEG_POS_LAST;
  }
} /* sndcp_set_unack_transfer_params() */
#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_set_ack_transfer_params
+------------------------------------------------------------------------------
| Description : There is one N-PDU, whose segments are sent to CIA one by one,
| and that is expected reassembled and decompressed back from CIA. Some
| informations are only given in the first segment. 
| This procedure stores the needed elements in service variables that can
| be read before sending a SIG_SDA_CIA_TRANSFER_REQ.
|
| Parameters  : ll_data_ind*
| Pre         : Correct instance of sda service must be activated.
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC
GLOBAL void sndcp_set_ack_transfer_params (T_LL_DATA_IND* ll_data_ind) 
{ 
  UBYTE nsapi = (ll_data_ind->sdu.buf[(ll_data_ind->sdu.o_buf / 8)]) & 0xf;

  /*
   * The dcomp value in the first segment of the currently reassembled N-PDU.
   */
  sndcp_data->cur_dcomp[nsapi] = 
         (ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8 + 1] & 0xf0) >> 4 ;
  /*
   * The pcomp value in the first segment of the currently reassembled N-PDU.
   */
  sndcp_data->cur_pcomp[nsapi] = 
                  ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8 + 1] & 0xf;
  /*
   * Reference to N-PDU.
   */
  sndcp_data->cur_pdu_ref[nsapi].ref_nsapi = nsapi;
  sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num = 
                     ll_data_ind->sdu.buf[(ll_data_ind->sdu.o_buf >> 3) + 2];

  
  /*
   * First and/or last segment?
   */
  sndcp_data->cur_seg_pos[nsapi] = 0;
  /*
   * if f bit is set
   */ 
  if ((ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8]) & 0x40) {
    sndcp_data->cur_seg_pos[nsapi] += SEG_POS_FIRST;
  }
  /*
   * if m bit is not set
   */
  if (!((ll_data_ind->sdu.buf[ll_data_ind->sdu.o_buf / 8]) & 0x10)) {
    sndcp_data->cur_seg_pos[nsapi] += SEG_POS_LAST;
  }
} /* sndcp_set_ack_transfer_params() */
#endif /* CF_FAST_EXEC */
