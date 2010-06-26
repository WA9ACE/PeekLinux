/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_mgf.c
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
|             procedures and functions as described in the
|             SDL-documentation (MG-statemachine)
+-----------------------------------------------------------------------------
*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_cias.h"    /* to get the signals to service cia */
#include "sndcp_nds.h"    /* to get the signals to service nd */
#include "sndcp_nus.h"    /* to get the signals to service nu */
#include "sndcp_nuf.h"    
#include "sndcp_sus.h"    /* to get the signals to service su */
#include "sndcp_suas.h"    /* to get the signals to service su */
#include "sndcp_sds.h"    /* to get the signals to service sd */
#include "sndcp_sdas.h"    /* to get the signals to service sda */

#include "sndcp_mgf.h"      /* to get ths file */
#include "sndcp_nup.h"      /* nu_sn_[unit]data_req is called from
                               sig_callback().*/

#include <string.h>

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void mg_get_sapi_dcomp_dntt (UBYTE sapi, UBYTE dcomp, UBYTE* dntt);

LOCAL void mg_get_sapi_dntt_nsapi (UBYTE sapi, UBYTE dntt, UBYTE nsapi, BOOL* used);

LOCAL void mg_get_sapi_dntt_state (UBYTE sapi, UBYTE dntt, UBYTE* state);

LOCAL void mg_get_sapi_dcomp_state (UBYTE sapi, UBYTE dcomp, UBYTE* stat);

LOCAL void mg_get_sapi_pcomp_state (UBYTE sapi, UBYTE pcomp, UBYTE* stat);

LOCAL void mg_get_sapi_pntt_state (UBYTE sapi, UBYTE pntt, UBYTE* state);

LOCAL void mg_set_sapi_dntt_nsapi (UBYTE sapi, UBYTE dntt, UBYTE nsapi, BOOL used);

LOCAL void mg_set_sapi_dcomp_state (UBYTE sapi, UBYTE dcomp, UBYTE stat);

LOCAL void mg_set_sapi_dcomp_dntt (UBYTE sapi, UBYTE dcomp, UBYTE dntt);

LOCAL void mg_set_sapi_dntt_rej (UBYTE sapi, UBYTE dntt, BOOL rej);

LOCAL void mg_set_sapi_dntt_state (UBYTE sapi, UBYTE dntt, UBYTE state);

LOCAL void mg_set_sapi_pntt_nsapi (UBYTE sapi, UBYTE pntt, UBYTE nsapi, BOOL used);

LOCAL void mg_set_sapi_pcomp_state (UBYTE sapi, UBYTE pcomp, UBYTE stat);

LOCAL void mg_set_sapi_pcomp_pntt (UBYTE sapi, UBYTE pcomp, UBYTE pntt);

LOCAL void mg_set_sapi_pntt_rej (UBYTE sapi, UBYTE pntt, BOOL rej);

LOCAL void mg_set_sapi_pntt_state (UBYTE sapi, UBYTE pntt, UBYTE state);

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_dcomp_dntt
+------------------------------------------------------------------------------
| Description : The procedures indicates the dntt assigned to a given dcomp
|               for a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dcomp UBYTE,
|               IN/OUT dntt UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_dcomp_dntt (UBYTE sapi, UBYTE dcomp, UBYTE* dntt)
{
  TRACE_FUNCTION( "mg_get_sapi_dcomp_dntt" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *dntt = sndcp_data->mg.sapi_dcomp_dntt_ra[sapi_index][dcomp];
  }
} /* mg_get_sapi_dcomp_dntt() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_dntt_nsapi
+------------------------------------------------------------------------------
| Description : The procedure informs if a given nsapi uses a given dntt on a
|               given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN nsapi UBYTE,
|               IN/OUT used BOOL
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_dntt_nsapi (UBYTE sapi, UBYTE dntt, UBYTE nsapi, BOOL* used)
{
  TRACE_FUNCTION( "mg_get_sapi_dntt_nsapi" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *used = sndcp_data->mg.sapi_dntt_nsapi_set_ra[sapi_index][dntt][nsapi];
  }
} /* mg_get_sapi_dntt_nsapi() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_dcomp_state
+------------------------------------------------------------------------------
| Description : The procedures informs about the state of a given dcomp on a
|               given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dcomp UBYTE,
|               IN/OUT stat UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_dcomp_state (UBYTE sapi, UBYTE dcomp, UBYTE* stat)
{
  TRACE_FUNCTION( "mg_get_sapi_dcomp_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *stat = sndcp_data->mg.sapi_dcomp_state_ra[sapi_index][dcomp];
  }
} /* mg_get_sapi_dcomp_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_dntt_state
+------------------------------------------------------------------------------
| Description : The procedures informs about the state of a given data
|               compression entity on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN/OUT state UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_dntt_state (UBYTE sapi, UBYTE dntt, UBYTE* state)
{
  TRACE_FUNCTION( "mg_get_sapi_dntt_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *state = sndcp_data->mg.sapi_dntt_state_ra[sapi_index][dntt];
  }



} /* mg_get_sapi_dntt_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_pcomp_state
+------------------------------------------------------------------------------
| Description : The procedures informs about the state of a given pcomp on a
|               given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pcomp UBYTE,
|               IN/OUT stat UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_pcomp_state (UBYTE sapi, UBYTE pcomp, UBYTE* stat)
{
  TRACE_FUNCTION( "mg_get_sapi_pcomp_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *stat = sndcp_data->mg.sapi_pcomp_state_ra[sapi_index][pcomp];
  }
} /* mg_get_sapi_pcomp_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_pntt_state
+------------------------------------------------------------------------------
| Description : The procedures informs about the state of a given header
|               compression entity on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN/OUT state UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_get_sapi_pntt_state (UBYTE sapi, UBYTE pntt, UBYTE* state)
{
  TRACE_FUNCTION( "mg_get_sapi_pntt_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *state = sndcp_data->mg.sapi_pntt_state_ra[sapi_index][pntt];
  }
} /* mg_get_sapi_pntt_state() */




/*
+------------------------------------------------------------------------------
| Function    : mg_check_sense_bitwise
+------------------------------------------------------------------------------
| Description : Checks, if "small" is bitwise smaller than or equal to "big"
|
| Parameters  : USHORT small, big, BOOL wrong is set to TRUE, if small > big
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_ushort_sense_bitwise (USHORT small,
                                    USHORT big,
                                    BOOL* wrong) {
  UBYTE bit = 0;
  TRACE_FUNCTION( "mg_check_sense_bitwise" );
  *wrong = TRUE;
  for (bit = 0; bit < 8 * sizeof(USHORT); bit++) {
    if ((small & (1 << bit)) > (big & (1 << bit))) {
      return;
    }
  }
  *wrong = FALSE;
} /* mg_check_sense_bitwise() */

/*
+------------------------------------------------------------------------------
| Function    : mg_check_sense_bitwise
+------------------------------------------------------------------------------
| Description : Checks, if "small" is bitwise smaller than "big"
|
| Parameters  : UBYTE small, big, BOOL wrong is set to TRUE, if small > big
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_ubyte_sense_bitwise (UBYTE small,
                                    UBYTE big,
                                    BOOL* wrong) {
  UBYTE bit = 0;
  TRACE_FUNCTION( "mg_check_sense_bitwise" );
  *wrong = TRUE;
  for (bit = 0; bit < 8 * sizeof(UBYTE); bit++) {
    if ((small & (1 << bit)) > (big & (1 << bit))) {
      return;
    }
  }
  *wrong = FALSE;
} /* mg_check_sense_bitwise() */

/*
+------------------------------------------------------------------------------
| Function    : mg_decode_v42
+------------------------------------------------------------------------------
| Description : decodes the v42 part of given sdu to given xid block
|
| Parameters  : source sdu,
|               index in sdu,
|               field index in sdu (sub index in compression fields,
|               destination xid block,
|               success ok
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_decode_v42 (T_sdu* sdu,
                           USHORT* index,
                           USHORT* field_index,
                           T_XID_BLOCK* xid_block,
                           BOOL* ok,
                           UBYTE p_bit) {
#define CHECK_XID_BUFFER_LEN2 if (*index + *field_index >= ((sdu->l_buf) >> 3)) return

  USHORT field_length = 0;
  TRACE_FUNCTION( "mg_decode_v42" );
  *ok = FALSE;

  /*
   * Set field_index to length of field octet.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  field_length = sdu->buf[*index + *field_index];
  /*
   * NSAPIS parameter and followers omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_DATA_P_1_NSAPIS_OM) {
      *ok = TRUE;
      return;
    }
  } else {
    if (field_length == MG_DATA_P_0_NSAPIS_OM) {
      *ok = TRUE;
      return;
    }
  }
  /*
   * If p bit set to 1, set field_index to dcomp field.
   */
  if (p_bit == SNDCP_P_BIT_1) {
    (*field_index)++;
    CHECK_XID_BUFFER_LEN2;
    xid_block->v42.dcomp =
      ((USHORT)sdu->buf[*index + *field_index]) >> 4;
  }
  /*
   * Set field_index to MSB of applicable nsapis short.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.nsapis =
    ((USHORT)sdu->buf[*index + *field_index]) << 8;
  xid_block->v42.nsapis_set = TRUE;
  /*
   * Set field_index to LSB of applicable nsapis short.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.nsapis +=
    ((USHORT)sdu->buf[*index + *field_index]);
  /*
   * P0 parameter and followers omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_DATA_P_1_P0_OM) {
      *ok = TRUE;
      return;
    }
  } else {
    if (field_length == MG_DATA_P_0_P0_OM) {
      *ok = TRUE;
      return;
    }
  }
  /*
   * Set field_index to p0.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.p0 =
    sdu->buf[*index + *field_index];
  xid_block->v42.p0_set = TRUE;
  /*
   * P1 parameter and followers omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_DATA_P_1_P1_OM) {
      *ok =TRUE;
      return;
    }
  } else {
    if (field_length == MG_DATA_P_0_P1_OM) {
      *ok =TRUE;
      return;
    }
  }
  /*
   * Set field_index to MSB of p1.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.p1 =
    ((USHORT)sdu->buf[*index + *field_index]) << 8;
  xid_block->v42.p1_set = TRUE;
  /*
   * Set field_index to LSB of p1.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.p1 +=
    ((USHORT)sdu->buf[*index + *field_index]);
  /*
   * P2 parameter and followers omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_DATA_P_1_P2_OM) {
      *ok = TRUE;
      return;
    }
  } else {
    if (field_length == MG_DATA_P_0_P2_OM) {
      *ok = TRUE;
      return;
    }
  }
  /*
   * Set field_index to p2.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN2;
  xid_block->v42.p2 =
    sdu->buf[*index + *field_index];
  xid_block->v42.p2_set = TRUE;

  /*
   * If length of v42 field is longer than specified, ignore the rest.
   */
  while (*field_index < field_length) {
    CHECK_XID_BUFFER_LEN2;
    (*field_index)++;
  }

  *ok = TRUE;

} /* mg_decode_v42() */

/*
+------------------------------------------------------------------------------
| Function    : mg_decode_vj
+------------------------------------------------------------------------------
| Description : decodes the vj part of given sdu to given xid block
|
| Parameters  : source sdu,
|               index in sdu,
|               field index in sdu (sub index in compression fields,
|               destination xid block,
|               success ok
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_decode_vj (T_sdu* sdu,
                           USHORT* index,
                           USHORT* field_index,
                           T_XID_BLOCK* xid_block,
                           BOOL* ok,
                           UBYTE p_bit) {
#define CHECK_XID_BUFFER_LEN3 if (*index + *field_index >= ((sdu->l_buf) >> 3)) return

  USHORT field_length = 0;
  TRACE_FUNCTION( "mg_decode_vj" );

  /*
   * Set field_index to length of field octet.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN3;
  field_length = sdu->buf[*index + *field_index];
  /*
   * NSAPIS parameter and followers omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_HEADER_P_1_NSAPIS_OM) {
      *ok = TRUE;
      return;
    }
  } else {
    if (field_length == MG_HEADER_P_0_NSAPIS_OM) {
      *ok = TRUE;
      return;
    }
  }
  /*
   * If p bit set to 1, set field_index to dcomp field.
   */
  if (p_bit == SNDCP_P_BIT_1) {
    (*field_index)++;
    CHECK_XID_BUFFER_LEN3;
    xid_block->vj.pcomp1 =
      ((USHORT)sdu->buf[*index + *field_index]) >> 4;
    xid_block->vj.pcomp2 =
      ((USHORT)sdu->buf[*index + *field_index]) & 0xf;
  }

  /*
   * Set field_index to MSB of applicable nsapis short.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN3;
  xid_block->vj.nsapis =
    ((USHORT)sdu->buf[*index + *field_index]) << 8;
  xid_block->vj.nsapis_set = TRUE;
  /*
   * Set field_index to LSB of applicable nsapis short.
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN3;
  xid_block->vj.nsapis +=
    ((USHORT)sdu->buf[*index + *field_index]);
  /*
   * S0_M_1 parameter omitted?
   */
  if (p_bit == SNDCP_P_BIT_1) {
    if (field_length == MG_HEADER_P_1_S0_M_1_OM) {
      *ok = TRUE;
      return;
    }
  } else {
    if (field_length == MG_HEADER_P_0_S0_M_1_OM) {
      *ok = TRUE;
      return;
    }
  }
  /*
   * Set field_index to "s0 - 1" Parameter
   */
  (*field_index)++;
  CHECK_XID_BUFFER_LEN3;
  xid_block->vj.s0_m_1 = sdu->buf[*index + *field_index];
  xid_block->vj.s0_m_1_set = TRUE;

  /*
   * If length of vj field is longer than specified, ignore the rest.
   */
  while (*field_index < field_length) {
    CHECK_XID_BUFFER_LEN3;
    (*field_index)++;
  }


  *ok = TRUE;

} /* mg_decode_vj() */

/*
+------------------------------------------------------------------------------
| Function    : mg_detect_mode_clash
+------------------------------------------------------------------------------
| Description : Unacknowledged and acknowledged contexts may not share the same
| compressor entity. This procdure detects possible violations of that rule.
|
| Parameters  : the set of nsapis given as USHORT bit mask,
|               violation detected, TRUE if at least 1 acknowledged and 1
|               unacknowledged context share the same compressor.
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_detect_mode_clash (USHORT nsapis, BOOL* vio)
{
  UBYTE nsapi = 0;
  BOOL unack_found = FALSE;
  BOOL ack_found = FALSE;
  BOOL ack = FALSE;
  TRACE_FUNCTION( "mg_detect_mode_clash" );
  /*
   * Find affected nsapis.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    if ((nsapis & (1 << nsapi)) > 0) {
      sndcp_get_nsapi_ack(nsapi, &ack);
      if (ack) {
        ack_found = TRUE;
      } else {
        unack_found = TRUE;
      }
    }
  }
  *vio = unack_found && ack_found;


} /* mg_detect_mode_clash() */


/*
+------------------------------------------------------------------------------
| Function    : mg_respond_if_nec
+------------------------------------------------------------------------------
| Description : All nsapis connected to the given sapi
|               that are in state MG_ACT are sent an
|               SNSM_ACTIVATE_RES.
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_respond_if_nec (UBYTE sapi) {
  UBYTE nsapi = 0;
  UBYTE sapi_index = 0;
#ifdef SNDCP_UPM_INCLUDED
  BOOL ack = FALSE;
#endif /* SNDCP_UPM_INCLUDED */

  TRACE_FUNCTION( "mg_respond_if_nec" );

  sndcp_get_sapi_index(sapi, &sapi_index);
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    USHORT nsapi_state = MG_IDLE;
    USHORT sapi_state = MG_IDLE;
    UBYTE local_sapi = 0;
    sndcp_get_nsapi_state(nsapi, &nsapi_state);
    sndcp_get_nsapi_sapi(nsapi, &local_sapi);
    sndcp_get_sapi_state(local_sapi, &sapi_state);
    if (((nsapi_state & MG_ACT) > 0)
        &&
        ((sapi_state & MG_XID) == 0)
        &&
        ((sapi_state & MG_EST) == 0)
        &&
        (local_sapi == sapi)) {

      /*
       * Open DTI connection.
       */
#ifndef SNDCP_UPM_INCLUDED
      mg_dti_open(nsapi);
#else 
      sndcp_get_nsapi_ack(nsapi, &ack);
      if (ack) {
        nu_ready_ind_if_nec(nsapi);
      } else {
        nu_unitready_ind_if_nec(nsapi);
      }
#endif

      mg_send_snsm_activate_res(nsapi);
      /*
       * Set nsapi state to MG_IDLE.
       */
      sndcp_unset_nsapi_state (nsapi, MG_ACT);
    }

    if (((nsapi_state & MG_DEACT) > 0)
        &&
        ((sapi_state & MG_XID) == 0)
        &&
        ((sapi_state & MG_REL) == 0)
        &&
        (local_sapi == sapi)) {

#ifdef SNDCP_UPM_INCLUDED 
      PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
#else
      PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
      /*
       * Now the NSAPI is not in use anymore:
       */
      sndcp_set_nsapi_used(nsapi, FALSE);
      sndcp_set_nsapi_ack(nsapi, FALSE);

      snsm_deactivate_res->nsapi = nsapi;
      sndcp_unset_nsapi_state(nsapi, MG_DEACT);
#ifdef SNDCP_UPM_INCLUDED
      PSEND(hCommUPM, snsm_deactivate_res);
#else
      PSEND(hCommSM, snsm_deactivate_res);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    }

  }

} /* mg_respond_if_nec */



/*
+------------------------------------------------------------------------------
| Function    : mg_set_ntt_comp
+------------------------------------------------------------------------------
| Description : Sets ntt and comp values acc. to req_xid_block.
|
| Parameters  : UBYTE sapi, BOOL is this a renegotiation?
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_ntt_comp (UBYTE sapi) {
  UBYTE sapi_index = 0;
  UBYTE nsapi = 0;
  TRACE_FUNCTION( "mg_set_ntt_comp" );
  sndcp_get_sapi_index(sapi, &sapi_index);

  if (sndcp_data->mg.req_xid_block[sapi_index].v42.is_set) {
    mg_set_sapi_dntt_state(sapi,
                          sndcp_data->mg.req_xid_block[sapi_index].v42.ntt,
                          MG_SELECTED);
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if ((sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis & (1 << nsapi))
           > 0) {
        mg_set_sapi_dntt_nsapi(sapi,
                               sndcp_data->mg.req_xid_block[sapi_index].v42.ntt,
                               nsapi,
                               TRUE);
      }
    }
    mg_set_sapi_dcomp_state(sapi,
                            sndcp_data->mg.req_xid_block[sapi_index].v42.dcomp,
                            MG_SELECTED);
    mg_set_sapi_dcomp_dntt(sapi,
                           sndcp_data->mg.req_xid_block[sapi_index].v42.dcomp,
                           sndcp_data->mg.req_xid_block[sapi_index].v42.ntt);
  }
  if (sndcp_data->mg.req_xid_block[sapi_index].vj.is_set) {
    mg_set_sapi_pntt_state(sapi,
                          sndcp_data->mg.req_xid_block[sapi_index].vj.ntt,
                          MG_SELECTED);
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if ((sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis & (1 << nsapi))
           > 0) {
        mg_set_sapi_pntt_nsapi(sapi,
                               sndcp_data->mg.req_xid_block[sapi_index].vj.ntt,
                               nsapi,
                               TRUE);
      }
    }
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp1,
                            MG_SELECTED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp2,
                            MG_SELECTED);
    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp1,
                           sndcp_data->mg.req_xid_block[sapi_index].vj.ntt);
    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp2,
                           sndcp_data->mg.req_xid_block[sapi_index].vj.ntt);
  }


} /* mg_set_ntt_comp() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_dntt_nsapi
+------------------------------------------------------------------------------
| Description : The procedure  stores the info that a given nsapi uses a given
|               dntt on a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN nsapi UBYTE,
|               IN used BOOL
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_dntt_nsapi (UBYTE sapi, UBYTE dntt, UBYTE nsapi, BOOL used)
{
  TRACE_FUNCTION( "mg_set_sapi_dntt_nsapi" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_dntt_nsapi_set_ra[sapi_index][dntt][nsapi] = used;
  }
} /* mg_set_sapi_dntt_nsapi() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_dcomp_state
+------------------------------------------------------------------------------
| Description : The procedures sets the state of a given dcomp on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dcomp UBYTE,
|               IN stat UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_dcomp_state (UBYTE sapi, UBYTE dcomp, UBYTE stat)
{
  TRACE_FUNCTION( "mg_set_sapi_dcomp_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_dcomp_state_ra[sapi_index][dcomp] = stat;
  }
} /* mg_set_sapi_dcomp_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_dcomp_dntt
+------------------------------------------------------------------------------
| Description : The procedures sets the dntt assigned to a given dcomp
|               for a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dcomp UBYTE,
|               IN dntt UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_dcomp_dntt (UBYTE sapi, UBYTE dcomp, UBYTE dntt)
{
  TRACE_FUNCTION( "mg_set_sapi_dcomp_dntt" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_dcomp_dntt_ra[sapi_index][dcomp] = dntt;
  }
} /* mg_set_sapi_dcomp_dntt() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_dntt_rej
+------------------------------------------------------------------------------
| Description : The procedures stores if a given data
|               compression entity on a given SAPI is to be rejected in
|               LL_XID_REQ because it cannot be set up.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN rej BOOL
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_dntt_rej (UBYTE sapi, UBYTE dntt, BOOL rej)
{
  TRACE_FUNCTION( "mg_set_sapi_dntt_rej" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_dntt_rej_ra[sapi_index][dntt] = rej;
  }
} /* mg_set_sapi_dntt_rej() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_dntt_state
+------------------------------------------------------------------------------
| Description : The procedures sets the state of a given data
|               compression entity on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN state UBYTE
| Note        : sapi dntt rej will be set ti FALSE if state is UNASSIGNED
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_dntt_state (UBYTE sapi, UBYTE dntt, UBYTE state)
{
  TRACE_FUNCTION( "mg_set_sapi_dntt_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_dntt_state_ra[sapi_index][dntt] = state;

    if (state == MG_UNASSIGNED) {
      mg_set_sapi_dntt_rej(sapi, dntt, FALSE);
    }
  }
} /* mg_set_sapi_dntt_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_pntt_nsapi
+------------------------------------------------------------------------------
| Description : The procedure  stores the info that a given nsapi uses a given
|               pntt on a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN nsapi UBYTE,
|               IN used BOOL
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_pntt_nsapi (UBYTE sapi, UBYTE pntt, UBYTE nsapi, BOOL used)
{
  TRACE_FUNCTION( "mg_set_sapi_pntt_nsapi" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_pntt_nsapi_set_ra[sapi_index][pntt][nsapi] = used;
  }
} /* mg_set_sapi_pntt_nsapi() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_pcomp_state
+------------------------------------------------------------------------------
| Description : The procedures sets the state of a given pcomp on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pcomp UBYTE,
|               IN stat UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_pcomp_state (UBYTE sapi, UBYTE pcomp, UBYTE stat)
{
  TRACE_FUNCTION( "mg_set_sapi_pcomp_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_pcomp_state_ra[sapi_index][pcomp] = stat;
  }
} /* mg_set_sapi_pcomp_state() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_pcomp_pntt
+------------------------------------------------------------------------------
| Description : The procedures sets the pntt assigned to a given pcomp
|               for a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pcomp UBYTE,
|               IN pntt UBYTE
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_pcomp_pntt (UBYTE sapi, UBYTE pcomp, UBYTE pntt)
{
  TRACE_FUNCTION( "mg_set_sapi_pcomp_pntt" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_pcomp_pntt_ra[sapi_index][pcomp] = pntt;
  }
} /* mg_get_sapi_pcomp_pntt() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_pntt_rej
+------------------------------------------------------------------------------
| Description : The procedures stores if a given data
|               compression entity on a given SAPI is to be rejected in
|               LL_XID_REQ because it cannot be set up.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN rej BOOL
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_pntt_rej (UBYTE sapi, UBYTE pntt, BOOL rej)
{
  TRACE_FUNCTION( "mg_set_sapi_pntt_rej" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_pntt_rej_ra[sapi_index][pntt] = rej;
  }
} /* mg_set_sapi_pntt_rej() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_sapi_pntt_state
+------------------------------------------------------------------------------
| Description : The procedures sets the state of a given header
|               compression entity on a given SAPI.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN state UBYTE
| Note        : sapi pntt rej will be set ti FALSE if state is UNASSIGNED
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_sapi_pntt_state (UBYTE sapi, UBYTE pntt, UBYTE state)
{
  TRACE_FUNCTION( "mg_set_sapi_pntt_state" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_data->mg.sapi_pntt_state_ra[sapi_index][pntt] = state;

    if (state == MG_UNASSIGNED) {
      mg_set_sapi_pntt_rej(sapi, pntt, FALSE);
    }
  }
} /* mg_set_sapi_pntt_state() */

#ifndef NCONFIG 
/*
+------------------------------------------------------------------------------
| Function    : mg_config_delay
+------------------------------------------------------------------------------
| Description : after config prim DELAY each new context activation will be
|               computed with a delay.
|
| Parameters  : delay in milliseconds
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_config_delay (USHORT millis)
{

  TRACE_FUNCTION("mg_config_delay");

  sndcp_data->millis = millis;

} /* mg_config_delay() */
#endif


/*
+------------------------------------------------------------------------------
| Function    : mg_reset_compressors
+------------------------------------------------------------------------------
| Description : All compressors used by this nsapi are reset.
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_reset_compressors (UBYTE nsapi)
{
  BOOL compressed = FALSE;
  TRACE_FUNCTION( "mg_reset_compressors" );

  sndcp_is_nsapi_data_compressed(nsapi, &compressed);
  if (compressed) {
    /*
     * Must be added when data compression is added.
     */
  }

  sndcp_is_nsapi_header_compressed(nsapi, &compressed);
  if (compressed) {
    UBYTE sapi = 0;
    UBYTE sapi_index = 0;
    /*
     * This is implementation dependent and only works as long as only 1
     * header compressor is used in cia service.
     */
    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_sapi_index(sapi, &sapi_index);
    sig_mg_cia_new_xid(&sndcp_data->mg.cur_xid_block[sapi_index]);
  }

} /* mg_reset_compressors() */



/*
+------------------------------------------------------------------------------
| Function    : mg_reset_comp_ack
+------------------------------------------------------------------------------
| Description : all compression entities using
|               acknowledged peer-to-peer LLC operation on this SAPI are reset.
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_reset_comp_ack (UBYTE sapi) {
  UBYTE nsapi = 0;
  TRACE_FUNCTION( "mg_reset_comp_ack" );
  /*
   * All nsapis at this sapi that use ack mode reset their compressors.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
    UBYTE s = 0;
    BOOL ack = FALSE;
    BOOL used = FALSE;
    sndcp_is_nsapi_used(nsapi, &used);
    if (!used) {
      continue;
    }
    sndcp_get_nsapi_sapi(nsapi, &s);
    sndcp_get_nsapi_ack(nsapi, &ack);
    if (ack && s == sapi) {
      mg_reset_compressors(nsapi);
    }
  } /* for all nsapis */

} /* mg_reset_comp_ack() */


/*
+------------------------------------------------------------------------------
| Function    : mg_reset_states_n_rej
+------------------------------------------------------------------------------
| Description : Resets all states for ntts and p/dcomp to "unassigned".
|               Resets the arrays with information on rejcted entities.
|
| Parameters  : index of the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_reset_states_n_rej (UBYTE sapi_index)
{

  UBYTE ntt = 0;
  UBYTE nsapi = 0;
  UBYTE dcomp = 0;
  UBYTE pcomp = 0;
  TRACE_FUNCTION( "mg_reset_states_n_rej" );

  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    sndcp_data->mg.sapi_dntt_state_ra[sapi_index][ntt] = MG_UNASSIGNED;
    sndcp_data->mg.sapi_pntt_state_ra[sapi_index][ntt] = MG_UNASSIGNED;
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      sndcp_data->mg.sapi_dntt_nsapi_set_ra[sapi_index][ntt][nsapi] = FALSE;
      sndcp_data->mg.sapi_pntt_nsapi_set_ra[sapi_index][ntt][nsapi] = FALSE;
    }
    sndcp_data->mg.sapi_dntt_rej_ra[sapi_index][ntt] = FALSE;
    sndcp_data->mg.sapi_pntt_rej_ra[sapi_index][ntt] = FALSE;
  }
  for (dcomp = 0; dcomp < MG_MAX_DCOMP; dcomp++) {
    sndcp_data->mg.sapi_dcomp_state_ra[sapi_index][dcomp] = MG_UNASSIGNED;
  }
  for (pcomp = 0; pcomp < MG_MAX_PCOMP; pcomp++) {
    sndcp_data->mg.sapi_pcomp_state_ra[sapi_index][pcomp] = MG_UNASSIGNED;
  }
  /*
   * sapi_dcomp_dntt_ra, sapi_pcomp_pntt_ra not initialized.
   */

} /* mg_reset_states_n_rej() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_xid_nsapis
+------------------------------------------------------------------------------
| Description : AN LL_XID_IND has delivered a desired set of nsapis for
| a V42.bis or VJ compressor entity. Now this is compared to a possibly
| already existing set of nsapis connected to the given entity.
| Also the rules in GSM 4.65, 6.8.x are checked.
|
| Parameters  : a flag telling if we work on V42 or VanJacobson 'nsapis':
|            MG_XID_V42_NSAPIS or MG_XID_VJ_NSAPIS.
|            affected sapi,
|
+------------------------------------------------------------------------------
*/
LOCAL void mg_set_xid_nsapis (UBYTE sapi, UBYTE p_type)
{
  USHORT* cur_nsapis = 0;
  USHORT* ind_nsapis = 0;
  USHORT* res_nsapis = 0;
  BOOL* ind_nsapis_set = NULL;
  UBYTE sapi_index = 0;
  UBYTE* p_bit = NULL;
  UBYTE ntt_state = MG_UNASSIGNED;
  U8 nsapi = 0;
  U8 s0_m_1_min = 0;


  TRACE_FUNCTION( "mg_set_xid_nsapis" );

  sndcp_get_sapi_index(sapi, &sapi_index);

  /*
   * V42.bis or VanJacobson?
   */
  if (p_type == MG_XID_V42_NSAPIS) {
    cur_nsapis = &(sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis);
    ind_nsapis = &(sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis);
    res_nsapis = &(sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis);
    ind_nsapis_set =
      &(sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis_set);
    p_bit = &(sndcp_data->mg.ind_xid_block[sapi_index].v42.p_bit);

    *res_nsapis = 0;
    /*
     * Only those nsapis will be set in the response for which the user
     * requested compression. 
     * Note: te other params are not yet set, must be implemented later.
     */
    sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis_set = FALSE;
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if ((((1 << nsapi) & sndcp_data->mg.user_xid_block[nsapi].v42.nsapis) > 
           0) && 
           (((1 << nsapi) & 
            sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis) > 0)) {

        *res_nsapis |= (1 << nsapi);
        sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis_set = TRUE;

      }
    } /* for nsapi */

  } else {
    cur_nsapis = &(sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis);
    ind_nsapis = &(sndcp_data->mg.ind_xid_block[sapi_index].vj.nsapis);
    res_nsapis = &(sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis);
    ind_nsapis_set =
      &(sndcp_data->mg.ind_xid_block[sapi_index].vj.nsapis_set);
    p_bit = &(sndcp_data->mg.ind_xid_block[sapi_index].vj.p_bit);

    *res_nsapis = 0;
    /*
     * Only those nsapis will be set in the response for which the user
     * requested compression. 
     * In the same loop we set the s0_m_1 to the minimum of
     * the indicated value and the values requested by the user.
     * Also the direction is set to the minimum of all requested
     * directions.
     */
    if (sndcp_data->mg.ind_xid_block[sapi_index].vj.s0_m_1 > 0) {
      s0_m_1_min = sndcp_data->mg.ind_xid_block[sapi_index].vj.s0_m_1;
    } else if (sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1 > 0) {
      s0_m_1_min = sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1;
    } else {
      s0_m_1_min = SNDCP_VJ_DEFAULT_S0_M_1;
    }
 #ifdef SNDCP_UPM_INCLUDED 
    sndcp_data->mg.res_xid_block[sapi_index].vj.direction = 
      NAS_HCOMP_BOTH_DIRECT;
#else
    sndcp_data->mg.res_xid_block[sapi_index].vj.direction = 
      SNSM_COMP_BOTH_DIRECT;
#endif 
    
    sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis_set = FALSE;
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & sndcp_data->mg.user_xid_block[nsapi].vj.nsapis) > 
          0 && 
           (((1 << nsapi) & 
            sndcp_data->mg.ind_xid_block[sapi_index].vj.nsapis) > 0)) {

        *res_nsapis |= (1 << nsapi);
        sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis_set = TRUE;
        if (sndcp_data->mg.user_xid_block[nsapi].vj.s0_m_1 < s0_m_1_min) {
          s0_m_1_min = sndcp_data->mg.user_xid_block[nsapi].vj.s0_m_1;
        }
        sndcp_data->mg.res_xid_block[sapi_index].vj.direction &=
          sndcp_data->mg.user_xid_block[nsapi].vj.direction;
      }
    } /* for nsapi */
    sndcp_data->mg.res_xid_block[sapi_index].vj.s0_m_1 = s0_m_1_min;
    if (s0_m_1_min > 0) {
      sndcp_data->mg.res_xid_block[sapi_index].vj.s0_m_1_set = TRUE;
    } else {
      sndcp_data->mg.res_xid_block[sapi_index].vj.direction = 0;
    }
  }

    

  /*
   * Rules in GSM 4.65, 6.8.1: acknowledged and unacknowlegded contexts may not
   * share the same compressor. If that is demanded by the indication, reset
   * nsapis parameter to the ones currently used.
   */
  if (*ind_nsapis_set &&
      *ind_nsapis > 0) {
    BOOL vio = FALSE;
    mg_detect_mode_clash(*ind_nsapis, &vio);
    if (!vio) {
      /*
       * Modes are the same, set cur to res.
       */
      *cur_nsapis = *res_nsapis;
    } else {
      *res_nsapis = *cur_nsapis;
    }
  }

  /*
   * Also from 6.8.3: If an unassigned entity number is included with the
   * p bit set to 0, then the Applicable NSAPIs field shall be set to 0.
   */
  if (p_type == MG_XID_V42_NSAPIS) {
    mg_get_sapi_dntt_state
      (sapi, sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt, &ntt_state);
  } else {
    mg_get_sapi_pntt_state
      (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt, &ntt_state);
  }
  if (ntt_state == MG_UNASSIGNED && *p_bit == 0) {
      *cur_nsapis = 0;
      *res_nsapis = 0;
  }

} /* mg_set_xid_nsapis() */


/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : mg_check_cnf_xid
+------------------------------------------------------------------------------
| Description : The confirmation of a requested XID negotiation has been
|               received. The answer is now checked.
|               A main issue is the comparison with the service variable
|               req_xid_block which holds the requested xid block sent to the
|               network.
|               Detected errors:
| - SNDCP version number not correct
| - incorrect entity number (must be proposed one)
| - incorrect sense of negotiation for each parameter (down mainly,
|   NSAPI down bitwise)
| - an entity is included in the cnf that was not requested, does not exist
| Not checked:
| - out of range value of parameters (must be in range if sense ofnegotiation was right.
| - parameters with duplicated instances.
| If one of these errors occurs ret is set to MG_XID_BAD_CONTENT, else
| MG_XID_OK.
|
| Parameters  : ret, affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_check_cnf_xid (UBYTE* ret, UBYTE sapi)
{
  UBYTE sapi_index = 0;
  BOOL wrong = FALSE;
  TRACE_FUNCTION( "mg_check_cnf_xid" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  *ret = MG_XID_BAD_CONTENT;
  /*
   * Check SNDCP version.
   */
  if (sndcp_data->mg.cnf_xid_block[sapi_index].version_set) {
    if (sndcp_data->mg.cnf_xid_block[sapi_index].version !=
        SNDCP_XID_VERSION) {
      return;
    }
  }
  /*
   * If an entity is included that has not been requested / does not exist:
   * bad content!
   */
  if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.is_set &&
      ! sndcp_data->mg.req_xid_block[sapi_index].v42.is_set &&
      ! sndcp_data->mg.cur_xid_block[sapi_index].v42.is_set) {

    return;
  }
  if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.is_set &&
      ! sndcp_data->mg.req_xid_block[sapi_index].vj.is_set &&
      ! sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) {

    return;
  }

  /*
   * Check V42 parameters, if they are set.
   */
  if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.is_set) {
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p_bit == 1) {
      return;
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.ntt !=
        sndcp_data->mg.req_xid_block[sapi_index].v42.ntt) {
      return;
    }
    /*
     * Sense of negotiation. If cnf value has been set, but req has not been
     * set then cnf value will be compared with cur value. This
     * is not applied to "applicable nsapis".
     */
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.nsapis_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis_set) {
        mg_ushort_sense_bitwise
          (sndcp_data->mg.cnf_xid_block[sapi_index].v42.nsapis,
           sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis,
           &wrong);
      }
      if (wrong) {
        return;
      }
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p0_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].v42.p0_set) {
        mg_ubyte_sense_bitwise(sndcp_data->mg.cnf_xid_block[sapi_index].v42.p0,
                               sndcp_data->mg.req_xid_block[sapi_index].v42.p0,
                               &wrong);
      } else {
        mg_ubyte_sense_bitwise(sndcp_data->mg.cnf_xid_block[sapi_index].v42.p0,
                               sndcp_data->mg.cur_xid_block[sapi_index].v42.p0,
                               &wrong);
      }
      if (wrong) {
        return;
      }
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p1_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].v42.p1_set) {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p1 >
          sndcp_data->mg.req_xid_block[sapi_index].v42.p1) {
          return;
        }
      } else {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p1 >
          sndcp_data->mg.cur_xid_block[sapi_index].v42.p1) {
          return;
        }
      }
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p2_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].v42.p2_set) {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p2 >
          sndcp_data->mg.req_xid_block[sapi_index].v42.p2) {
          return;
        }
      } else {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].v42.p2 >
          sndcp_data->mg.cur_xid_block[sapi_index].v42.p2) {
          return;
        }
      }
    }
  }
  /*
   * Check VJ parameters, if they are set.
   */
  if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.is_set) {
    if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.p_bit == 1) {
      return;
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.ntt !=
        sndcp_data->mg.req_xid_block[sapi_index].vj.ntt) {
      return;
    }
    /*
     * Sense of negotiation.
     */
    if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.nsapis_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis_set) {
        mg_ushort_sense_bitwise(sndcp_data->mg.cnf_xid_block[sapi_index].vj.nsapis,
                                sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis,
                                &wrong);
      }
      if (wrong) {
        return;
      }
    }
    if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.s0_m_1_set) {
      if (sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1_set) {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.s0_m_1 >
          sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1) {
          return;
        }
      } else {
        if (sndcp_data->mg.cnf_xid_block[sapi_index].vj.s0_m_1 >
          sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1) {
          return;
        }
      }
    }
  }
  *ret = MG_XID_OK;

} /* mg_check_cnf_xid() */

/*
+------------------------------------------------------------------------------
| Function    : mg_check_ind_xid
+------------------------------------------------------------------------------
| Description : Service variable ind_xid_block holds the xid block indicated
| by the peer. The ind_xid_block is now checked: (from 4.65 6.8.2).
| Criterium 1:
| If the indicated ntt is already used and pcomp or dcomp values are different,
| ret is set to MG_XID_BAD_CONTENT.
| Criterium 2:
| If the indicated algorithm type is already used and pcomp or dcomp values
| are different, ret is set to MG_XID_BAD_CONTENT.
|
| Note: implementation dependent: if data compression is proposed, it will
| be rejected.
|
| Otherwise ret is set to MG_XID_OK.
| Parameters  : ret, affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_check_ind_xid (UBYTE* ret, UBYTE sapi)
{
  UBYTE sapi_index = 0;
  UBYTE status = MG_UNASSIGNED;
  TRACE_FUNCTION( "mg_check_ind_xid" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  *ret = MG_XID_OK;

  /*
   * Check criterium 1.
   */
  if (sndcp_data->mg.ind_xid_block[sapi_index].v42.is_set &&
      sndcp_data->mg.ind_xid_block[sapi_index].v42.p_bit == SNDCP_P_BIT_1) {
    mg_get_sapi_dntt_state
      (sapi, sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt, &status);
    if (status == MG_ASSIGNED) {
      /*
       * Entity already used. DCOMP values differ?
       */
      mg_get_sapi_dcomp_state
        (sapi, sndcp_data->mg.ind_xid_block[sapi_index].v42.dcomp, &status);
      if (status != MG_ASSIGNED) {
        /*
         * The indicated dcomp is not assigned. Error.
         */
        mg_set_sapi_dntt_rej(sapi,
                             sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                             TRUE);
        *ret = MG_XID_BAD_CONTENT;
      } else {
        UBYTE dntt = 0;
        mg_get_sapi_dcomp_dntt
          (sapi, sndcp_data->mg.ind_xid_block[sapi_index].v42.dcomp, &dntt);
        if (dntt != sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt) {
          /*
           * Dcomp values differ, Error.
           */
          mg_set_sapi_dntt_rej(sapi,
                               sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                               TRUE);
          *ret = MG_XID_BAD_CONTENT;
        }
      }
    }
  }
  if (sndcp_data->mg.ind_xid_block[sapi_index].vj.is_set &&
      sndcp_data->mg.ind_xid_block[sapi_index].vj.p_bit == SNDCP_P_BIT_1) {
    mg_get_sapi_pntt_state
      (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt, &status);
    if (status == MG_ASSIGNED) {
      /*
       * Entity already used. PCOMP values differ?
       */
      /*
       * PCOMP 1.
       */
      mg_get_sapi_pcomp_state
        (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1, &status);
      if (status != MG_ASSIGNED) {
        /*
         * The indicated pcomp is not assigned. Error.
         */
        mg_set_sapi_pntt_rej(sapi,
                             sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                             TRUE);
        *ret = MG_XID_BAD_CONTENT;
      } else {
        UBYTE pntt = 0;
        mg_get_sapi_pcomp_pntt
          (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1, &pntt);
        if (pntt != sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt) {
          /*
           * Pcomp values differ, Error.
           */
          mg_set_sapi_pntt_rej(sapi,
                               sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                               TRUE);
          *ret = MG_XID_BAD_CONTENT;
        }
      }
      /*
       * PCOMP 2.
       */
      mg_get_sapi_pcomp_state
        (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1, &status);
      if (status != MG_ASSIGNED) {
        /*
         * The indicated pcomp is not assigned. Error.
         */
        mg_set_sapi_pntt_rej(sapi,
                             sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                             TRUE);
        *ret = MG_XID_BAD_CONTENT;
      } else {
        UBYTE pntt = 0;
        mg_get_sapi_pcomp_pntt
          (sapi, sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1, &pntt);
        if (pntt != sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt) {
          /*
           * Dcomp values differ, Error.
           */
          mg_set_sapi_pntt_rej(sapi,
                               sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                               TRUE);
          *ret = MG_XID_BAD_CONTENT;
        }
      }
    }
  }

  /*
   * Check criterium 2.
   * Since in the current implementation there is only SNDCP_XID_VJ and
   * SNDCP_XID_V42, and only 1 instance of each,
   * it will be sufficient to compare the proposed values for pcomp/dcomp
   * with the ones in cur_xid_block.
   * An error may only occur if cur and ind are set.
   */
  if (sndcp_data->mg.ind_xid_block[sapi_index].v42.is_set &&
      sndcp_data->mg.cur_xid_block[sapi_index].v42.is_set &&
      sndcp_data->mg.ind_xid_block[sapi_index].v42.p_bit == SNDCP_P_BIT_1) {
    if (sndcp_data->mg.ind_xid_block[sapi_index].v42.dcomp !=
      sndcp_data->mg.cur_xid_block[sapi_index].v42.dcomp) {
      mg_set_sapi_dntt_rej(sapi,
                           sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                           TRUE);
      *ret = MG_XID_BAD_CONTENT;
    }
  }
  if (sndcp_data->mg.ind_xid_block[sapi_index].vj.is_set &&
      sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set &&
      sndcp_data->mg.ind_xid_block[sapi_index].v42.p_bit == SNDCP_P_BIT_1) {
    if (sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1 !=
      sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp1) {
      mg_set_sapi_pntt_rej(sapi,
                           sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                           TRUE);
      *ret = MG_XID_BAD_CONTENT;
    }
    if (sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp2 !=
      sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp2) {
      mg_set_sapi_pntt_rej(sapi,
                           sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                           TRUE);
      *ret = MG_XID_BAD_CONTENT;
    }
  }

#ifndef TI_PS_FF_V42BIS
  /*
   * Implementation dependent: If data compression is proposed, reject it!
   */
  if (sndcp_data->mg.ind_xid_block[sapi_index].v42.is_set) {
    mg_set_sapi_dntt_rej(sapi,
                         sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                         TRUE);
  }
#endif /* !TI_PS_FF_V42BIS */
} /* mg_check_ind_xid() */


/*
+------------------------------------------------------------------------------
| Function    : mg_clean_xid
+------------------------------------------------------------------------------
| Description : Cleans up the XID organizing arrays for nsapis and ntts
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_clean_xid (UBYTE sapi) {
  UBYTE nsapi = 0;
  UBYTE sapi_index = 0;
  UBYTE stat = MG_UNASSIGNED;
  TRACE_FUNCTION( "mg_clean_xid" );
  sndcp_get_sapi_index(sapi, &sapi_index);


  /*
   * NSAPIs that were assigned to a compressor unit but are not any more.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    BOOL connected =
      ((1 << nsapi) & (sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis)) > 0;
    mg_set_sapi_dntt_nsapi(sapi,
                           sndcp_data->mg.cur_xid_block[sapi_index].v42.ntt,
                           nsapi,
                           connected);
    connected =
      ((1 << nsapi) & (sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis)) > 0;
    mg_set_sapi_pntt_nsapi(sapi,
                           sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt,
                           nsapi,
                           connected);


  }

  /*
   * Formerly assigned ntts that are now unassigned are cleaned.
   * Implemented here: the one dntt and the one pntt in cur_xid_block are now
   * checked. If they are in state MG_ASSIGNED but the affected 'nsapis'
   * element is set to all '0' then the entities enter state MG_UNASSIGNED,
   * all nsapis re set to FALSE in the sapi_?ntt_nsapi_ra, all
   * affected pcomp/dcomp values are set to MG_UNASSIGNED.
   */
  mg_get_sapi_dntt_state(sapi,
                         sndcp_data->mg.cur_xid_block[sapi_index].v42.ntt,
                         &stat);
  if (stat == MG_ASSIGNED &&
      sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis == 0) {
    /*
     * Reset dntt state.
     */
    mg_set_sapi_dntt_state(sapi,
                           sndcp_data->mg.cur_xid_block[sapi_index].v42.ntt,
                           MG_UNASSIGNED);

    /*
     * reset nsapi connections.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      mg_set_sapi_dntt_nsapi(sapi,
                             sndcp_data->mg.cur_xid_block[sapi_index].v42.ntt,
                             nsapi,
                             FALSE);
    }
    /*
     * Reset dcomp.
     */
    mg_set_sapi_dcomp_state(sapi,
                            sndcp_data->mg.cur_xid_block[sapi_index].v42.dcomp,
                            MG_UNASSIGNED);
  }


  /*
   * The same for header compresion.
   */
  mg_get_sapi_pntt_state(sapi,
                         sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt,
                         &stat);
  if (stat == MG_ASSIGNED &&
      sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis == 0) {
    /*
     * Reset pntt state.
     */
    mg_set_sapi_pntt_state(sapi,
                           sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt,
                           MG_UNASSIGNED);
    /*
     * reset nsapi connections.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      mg_set_sapi_pntt_nsapi(sapi,
                             sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt,
                             nsapi,
                             FALSE);
    }
    /*
     * Reset pcomps.
     */
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp1,
                            MG_UNASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp2,
                            MG_UNASSIGNED);
  }



} /* mg_clean_xid() */

/*
+------------------------------------------------------------------------------
| Function    : mg_col_no_re
+------------------------------------------------------------------------------
| Description : The function mg_col_no_re represents  the SDL label
|               COL_NO_RE: an LL_ESTABLISH_IND has been received, we do
|               have a collision situation like given in [GSM 4.65, 6.2.1.4],
|               we do not have a re-establishment situation.
|
| Parameters  : the received LL_ESTABLISH_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_col_no_re (T_LL_ESTABLISH_IND* ll_establish_ind)
{
  TRACE_FUNCTION( "mg_col_no_re" );

  /*
   * Resume data transfer suspended due to pending establishment;
   */
  sig_mg_su_resume(ll_establish_ind->sapi);
  sig_mg_sua_resume(ll_establish_ind->sapi);
  mg_resume_affected_nus(ll_establish_ind->sapi);

  /*
   * Now proceed like without collision.
   */
  mg_no_col_no_re(ll_establish_ind);


} /* mg_col_no_re() */


/*
+------------------------------------------------------------------------------
| Function    : mg_col_re
+------------------------------------------------------------------------------
| Description : The function mg_col_re represents  the SDL label
|               COL_RE: an LL_ESTABLISH_IND has been received, we do
|               have a collision situation like given in [GSM 4.65, 6.2.1.4],
|               we do have a re-establishment situation.
|
| Parameters  : the received LL_ESTABLISH_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_col_re (T_LL_ESTABLISH_IND* ll_establish_ind)
{
  TRACE_FUNCTION( "mg_col_re" );
  /*
   * Resume data transfer suspended due to pending establishment;
   */
  sig_mg_su_resume(ll_establish_ind->sapi);
  sig_mg_sua_resume(ll_establish_ind->sapi);
  mg_resume_affected_nus(ll_establish_ind->sapi);
  /*
   * Now proceed like without collision.
   */
  mg_no_col_re(ll_establish_ind);
} /* mg_col_re() */

/*
+------------------------------------------------------------------------------
| Function    : mg_decode_xid
+------------------------------------------------------------------------------
| Description : Implementation dependent.
|               The entity number
|               Works only for algorithm types 0
|               (V42 for data and VanJacobson for header).
|               This procedure reads the given xid block (in form of an sdu)
|               and writes the fields to the given xid_block variable, if
|               compression fields for algorithms 0 (V42 or VanJacobson) are
|               given.
|               If the parsed sdu starts a negotiation then the p bit will be
|               set to 1 and the "algorithm type" field will be included. If
|               the parsed sdu ends a negotiation then the p bit will be set
|               to 0 and the "algorithm type" field will not be included.
|               In this case it will be checked if the "entity number" field
|               corresponds with the entity number set in req_xid_block.
|
|               Note: This procedure only re-formats the xid_block. The content
|               of the xid_block are not checked. This will be done in a later
|               procedure on the basis of the xid_block filled in here.
|               The only error possibly detected is returned, if the byte format
|               of the given xid block sdu is not correct: MG_XID_BAD_FORMAT.
|               Then parameter type 1 list is read. If a data comp field has
|               an algorithm of type 0 (V42bis) then its values are written
|               to the given xid_block. If the algorithm is not 0 (V42bis) then
|               the entity is rejected.
|               The same for parameter type 2, here only algorithm type 0
|               (VanJacobson) is written to the xid_block.
|               Note: it is assumed here that the parameters come in line 0, 1, 2.
|               If that will no be the case, the parsing will have to modified
|               slightly.
|               If an unknown parameter type (not 0, 1, 2) is detected, an
|               MG_XID_BAD_FORMAT is returned.
|
| Parameters  : the sdu to be parsed, is negotiation started here?, the 
|               destination T_XID_BLOCK, a return value that should be MG_XID_OK
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_decode_xid (T_sdu* sdu,
                           T_XID_BLOCK* xid_block,
                           UBYTE* ret,
                           UBYTE sapi)
{

#define CHECK_XID_BUFFER_LEN if (index + field_index >= ((sdu->l_buf) >> 3)) return
  UBYTE sapi_index = 0;
  /*
   * Index for the whole sdu.
   */
  USHORT index = sdu->o_buf / 8;
  /*
   * Sub index used in compression fields.
   */
  USHORT field_index = 0;
  /*
   * This is the index where the parameter 2 for header compression begins.
   */
  USHORT beginning_of_header_comp = 0;
  /*
   * Length of complete parameter blocks of type 1 or 2.
   */
  USHORT length = 0;
  /*
   * Was decoding successful?
   */
  BOOL ok = FALSE;

  TRACE_FUNCTION( "mg_decode_xid" );

  /*
   * Reset xid_block.
   */
  sndcp_reset_xid_block(xid_block);

  sndcp_get_sapi_index(sapi, &sapi_index);
  *ret = MG_XID_BAD_FORMAT;

  /*
   * If end is reached, return with ok.
   */
  if (sdu->l_buf == 0) {
    *ret = MG_XID_OK;
    return;
  }
  /*
   * If parameter type 0 is included, store it.
   */
  CHECK_XID_BUFFER_LEN;
  if (sdu->buf[index] == SNDCP_XID_PARAM_TYPE_0) {
    index++;
    xid_block->version_set = TRUE;
    /*
     * Length should be SNDCP_XID_0_LEN, but is not checked.
     */
    CHECK_XID_BUFFER_LEN;
    length = sdu->buf[index];

    index++;
    /*
     * Set the version in xid_block.
     */
    CHECK_XID_BUFFER_LEN;
    xid_block->version = sdu->buf[index];
    index+= length;
  }

  /*
   * If end is reached, return with ok.
   */
  if (index == ((sdu->l_buf) >> 3)) {
    *ret = MG_XID_OK;
    return;
  }

  /*
   * If parameter type 1 is included, store it.
   */
  if (sdu->buf[index] == SNDCP_XID_PARAM_TYPE_1) {
    /*
     * Length of all data compression parameters.
     */
    index++;
    CHECK_XID_BUFFER_LEN;
    length = sdu->buf[index];
    /*
     * This is the index where the parameter 2 infos for header compression
     * begin.
     */
    beginning_of_header_comp = index + length + 1;

    /*
     * Search data compression parameters for algorithm type 1.
     */
    while (length > 0) {
      /*
       * Index is now on p-bit-entity-octet for one field.
       * If the p bit is set to 1 then we read the algorithm
       * type and drop the field if it is not SNDCP_XID_V42.
       * If the p bit is not set to 1 then we read the
       * entity number and drop the field if it is not the entity number
       * proposed in req_xid_block.
       */

      /*
       * Set field_index to beginning of next data compression field.
       */
      field_index++;

      CHECK_XID_BUFFER_LEN;
      if ((sdu->buf[index + field_index] & 0x80) > 0) {
        /*
         * P bit set to 1. Check algorithm type and drop field if
         * type is not known.
         * Set ntt.
         */
        xid_block->v42.ntt = sdu->buf[index + field_index] & 0x1F;
        /*
         * Set field_index to algorithm type.
         */
        field_index ++;
        CHECK_XID_BUFFER_LEN;
        if ((sdu->buf[index + field_index] & 0xf) != SNDCP_XID_V42) {
          /*
           * Wrong algorithm type. Add the ntt to list of rejected ones.
           */
          mg_set_sapi_dntt_rej(sapi, xid_block->v42.ntt, TRUE);
          /*
           * Set index to length octet.
           */
          field_index ++;
          /*
           * Set index to next field. Check field length.
           */
          CHECK_XID_BUFFER_LEN;
          field_index += sdu->buf[index + field_index];
          if (field_index > length) {
            /*
             * Field is too long, bad format.
             */
            return;
          }
          if (field_index == length) {
            /*
             * Field is completed but entity number has not been found because in
             * that case the loop would have been left with break;
             */
            xid_block->v42.is_set = FALSE;
            break;
          }
          continue;
        } else {
          /*
           * Correct algorithm type.
           */
          if (xid_block->v42.is_set == FALSE) {
            /*
             * This is the first occurrence.
             */
            xid_block->v42.is_set = TRUE;
            xid_block->v42.p_bit = 1;


            mg_decode_v42(sdu,
                          &index,
                          &field_index,
                          xid_block,
                          &ok,
                          SNDCP_P_BIT_1);
            if (!ok) {
              /*
               * Decoding was not successful.
               */
              return;
            }
          } else {
            /*
             * There has been an occurrence of this algorithm.
             */
            /*
             * Set index to length octet.
             */
            field_index ++;
            /*
             * Set index to next field. Check field length.
             */
            CHECK_XID_BUFFER_LEN;
            field_index += sdu->buf[index + field_index];
            if (field_index > length) {
              /*
               * Field is too long, bad format.
               */
              return;
            }
            if (field_index == length) {
              /*
               * Field is completed.
               */
              break;
            }

          }
          /*
           * If all data parameters are read, go ahead for header ones.
           */
          if (field_index == length) {
            break;
          }
        }
      } else {
        /*
         * P bit set to 0.
         * Check entity number and drop field if it is not the proposed one or
         * V42 has not been proposed yet.
         * field_index is already on p-bit-entity-octet.
         */
        CHECK_XID_BUFFER_LEN;
        if (((sdu->buf[index + field_index] & 0x1f) !=
            xid_block->v42.ntt) && xid_block->v42.is_set) {
          /*
           * Wrong entity number.
           */
          /*
           * Set field_index to length octet.
           */
          field_index ++;
          /*
           * Set index to next field. Check field length.
           */
          CHECK_XID_BUFFER_LEN;
          field_index += sdu->buf[index + field_index];
          if (field_index > length) {
            /*
             * Field is too long, bad format.
             */
            return;
          }
          if (field_index == length) {
            /*
             * Field is completed but entity number has not been found because in
             * that case the loop would have been left with break;
             */
            xid_block->v42.is_set = FALSE;
            break;
          }
          continue;
        } else {
          /*
           * The entity number of this field equals the entity number in
           * ththat is already in the xid block or there is no entity in
           * the xid block yet.
           */
          xid_block->v42.is_set = TRUE;
          xid_block->v42.p_bit = 0;
          xid_block->v42.ntt =
            (sdu->buf[index + field_index] & 0x1f);

          mg_decode_v42(sdu,
                        &index,
                        &field_index,
                        xid_block,
                        &ok,
                        SNDCP_P_BIT_0);
          if (!ok) {
            /*
             * Decoding was not successful.
             */
            return;
          }

          /*
           * If all data parameters are read, go ahead for header ones.
           */
          if (field_index == length) {
            break;
          }
        }
      } /* p bit set to 0 */
    } /* while  */
    field_index = 0;
    index = beginning_of_header_comp;
  }

  /*
   * Now comes the header compression parameter type 2.
   * Is it omitted?
   */
  if (index == ((sdu->l_buf) >> 3)) {
    *ret = MG_XID_OK;
    return;
  }

  /*
   * Parameter type should be SNDCP_XID_PARAM_TYPE_2.
   */
  CHECK_XID_BUFFER_LEN;
  if (sdu->buf[index] != SNDCP_XID_PARAM_TYPE_2) {
    return;
  }
  /*
   * Length of all header compression parameters.
   */
  index++;
  CHECK_XID_BUFFER_LEN;
  length = sdu->buf[index];


  /*
   * Search header compression parameters for algorithm type 2.
   */
  while (length > 0) {
    /*
     * Index is now on p-bit-entity-octet for one field.
     * If the p bit is set to 1 then we read the algorithm
     * type and drop the field if it is not SNDCP_XID_VJ.
     * If the p bit is not set to 1 then we read the
     * entity number and drop the field if it is not the entity number
     * proposed in req_xid_block.
     */
    /*
     * Set field_index to beginning of next header compression field.
     */
    field_index++;
    CHECK_XID_BUFFER_LEN;
    if ((sdu->buf[index + field_index] & 0x80) > 0) {
      UBYTE ntt = 0;
      /*
       * P bit set to 1. Check algorithm type and drop field if
       * type is not known.
       * Set ntt.
       */
      ntt = sdu->buf[index + field_index] & 0x1F;

      /*
       * Set index to algorithm type.
       */
      field_index ++;
      CHECK_XID_BUFFER_LEN;
      if (((sdu->buf[index + field_index] & 0xf) != SNDCP_XID_VJ)
          ||
          ((sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) &&
           (xid_block->vj.is_set == FALSE) &&
           (ntt != sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt)
          )
         ) {
        /*
         * Wrong algorithm type or
         * vj comp is currently used and proposed ntt is not equal to currently
         * used one.
         * Add ntt to list of rejected ones.
         */
        mg_set_sapi_pntt_rej(sapi, ntt, TRUE);
        /*
         * Set index to length octet.
         */
        field_index ++;
        /*
         * Set index to next field. Check field length.
         */
        CHECK_XID_BUFFER_LEN;
        field_index += sdu->buf[index + field_index];
        if (field_index > length) {
          /*
           * Field is too long, bad format.
           */
          return;
        }
        if (field_index == length) {
          /*
           * Field is completed but entity number has not been found because in
           * that case the loop would have been left with break;
           */
          xid_block->vj.is_set = FALSE;
          break;
        }
        continue;
      }
      /*
       * Correct algorithm type and vj requested.
       */
      /*
       * If no vj comp is currently used and xid_block.vj is not set yet, take
       * the proposed one.
       * Also if vj comp is currently used and xid block is not yet set and
       * proposed ntt is equal to currently used one.
       */
      if (((! sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) &&
           (xid_block->vj.is_set == FALSE))
          ||
          ((sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) &&
           (xid_block->vj.is_set == FALSE) &&
           (ntt == sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt))) {

        xid_block->vj.is_set = TRUE;
        xid_block->vj.p_bit = 1;
        xid_block->vj.ntt = ntt;

        mg_decode_vj (sdu,
                      &index,
                      &field_index,
                      xid_block,
                      &ok,
                      SNDCP_P_BIT_1);
        if (!ok) {
          /*
           * Decoding was not successful.
           */
          return;
        }
        /*
         * If all header parameters are read, go ahead for header ones.
         */
        if (field_index == length) {
          break;
        }
        continue;
      }
      /*
       * This is not the first occurrence. Ignore.
       */
      /*
       * Set index to length octet.
       */
      field_index ++;
      /*
       * Set index to next field. Check field length.
       */
      CHECK_XID_BUFFER_LEN;
      field_index += sdu->buf[index + field_index];
      if (field_index > length) {
        /*
         * Field is too long, bad format.
         */
        return;
      }
      if (field_index == length) {
        /*
         * Field is completed.
         */
        break;
      }

    } else {
      /*
       * P bit set to 0.
       * Check entity number and drop field if it is not the proposed one or
       * VJ has not been proposed yet.
       * Index is already on p-bit-entity-octet.
       */
      CHECK_XID_BUFFER_LEN;
      if (((sdu->buf[index + field_index] & 0x1f) !=
          xid_block->vj.ntt) && xid_block->vj.is_set) {
        /*
         * Wrong entity number.
         * Set index to length octet.
         */
        field_index ++;
        /*
         * Set index to next field. Check field length.
         */
        CHECK_XID_BUFFER_LEN;
        field_index += sdu->buf[index + field_index];
        if (field_index > length) {
          /*
           * Field is too long, bad format.
           */
          return;
        }
        if (field_index == length) {
          /*
           * Field is completed but entity number has not been found because in
           * that case the loop would have been left with break;
           */
          break;
        }
        continue;
      } else {
        /*
         * The entity number of this field equals the entity number in
         * that is already in the xid_block or there is no entity number
         * in the block yet.
         */
        xid_block->vj.is_set = TRUE;
        xid_block->vj.p_bit = 0;
        xid_block->vj.ntt =
          (sdu->buf[index + field_index] & 0x1f);

        mg_decode_vj (sdu,
                      &index,
                      &field_index,
                      xid_block,
                      &ok,
                      SNDCP_P_BIT_0);
        if (!ok) {
          /*
           * Decoding was not successful.
           */
          return;
        }
        /*
         * The one field with same entity number as in req_xid_block has been
         * found and loop of data compression field may be left.
         * If errors in not used fields are to be detected then we must continue here!
         */
        /*
         * If all header parameters are read, leave.
         */
        if (field_index == length) {
          break;
        }
      }
    }
  } /* while */
  /*
   * It is not checked here whether the sdu is completely read.
   */
  *ret = MG_XID_OK;

} /* mg_decode_xid() */


/*
+------------------------------------------------------------------------------
| Function    : mg_dti_close
+------------------------------------------------------------------------------
| Description : Closes DTI connection
| Parameters  : nsapi
+------------------------------------------------------------------------------
*/
GLOBAL void mg_dti_close (UBYTE nsapi)
{

#ifdef _SNDCP_DTI_2_
  UBYTE interfac = SNDCP_INTERFACE_UNACK;
#ifndef SNDCP_UPM_INCLUDED
  sndcp_get_nsapi_interface(nsapi, &interfac);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
  dti_close(sndcp_data->hDTI, /* DTI_HANDLE *hDTI, */
            0, /* U8 instance, */
            interfac, /* U8 interface, */
            nsapi,
            NULL/* U8 channel); */
           );
#else /*_SNDCP_DTI_2_*/
  UBYTE interfac = SNDCP_INTERFACE_UNACK;

  sndcp_get_nsapi_interface(nsapi, &interfac);
  dti_close(sndcp_data->hDTI, /* DTI_HANDLE *hDTI, */
            0, /* U8 instance, */
            interfac, /* U8 interface, */
            nsapi /* U8 channel); */

           );
#endif /*_SNDCP_DTI_2_*/

  /*
   * The following is done in any case since the callback will not be called.
   */
  nu_connection_state(nsapi, FALSE);
   
  /*FIXME ! Added newly. To be verified.*/
#ifdef SNDCP_UPM_INCLUDED 
  {
    U32 linkid = 0;
    PALLOC (sn_dti_cnf, SN_DTI_CNF);
    sndcp_get_nsapi_linkid(nsapi, &linkid);
    sn_dti_cnf->dti_linkid = linkid;
    sn_dti_cnf->dti_conn = NAS_DISCONNECT_DTI;
    PSEND(hCommMMI, sn_dti_cnf);
  }
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
} /* mg_dti_close() */

/*
+------------------------------------------------------------------------------
| Function    : mg_dti_open
+------------------------------------------------------------------------------
| Description : Opens DTI connection
| Parameters  : nsapi
+------------------------------------------------------------------------------
*/
GLOBAL void mg_dti_open (UBYTE nsapi)
{
#ifdef _SNDCP_DTI_2_
  UBYTE* neighbor = NULL;
  ULONG linkid = 0;
#ifndef SNDCP_UPM_INCLUDED
  BOOL ack = FALSE;
#endif 
  U8 direction = DTI_CHANNEL_TO_LOWER_LAYER;
  UBYTE interfac = SNDCP_INTERFACE_UNACK;

  sndcp_get_nsapi_linkid(nsapi, &linkid);
  sndcp_get_nsapi_neighbor(nsapi, &neighbor);
#ifndef SNDCP_UPM_INCLUDED
  sndcp_get_nsapi_ack(nsapi, &ack);
#endif 
  sndcp_get_nsapi_direction(nsapi, &direction);
#ifndef SNDCP_UPM_INCLUDED
  if (ack) {
    interfac = SNDCP_INTERFACE_ACK;
    sndcp_set_nsapi_interface(nsapi, interfac);
  }
#endif

  dti_open(sndcp_data->hDTI, /* DTI_HANDLE hDTI */
           0, /* U8 instance */
           interfac, /* U8 interface */
           nsapi, /* U8 channel */
           0, /* U8 queue_size */
           direction, /* U8 direction */
           DTI_QUEUE_WATERMARK, /* U8 link options */
           DTI_VERSION_10, /* U32 version */
           neighbor, /* U8 *neighbor_entity */
           linkid/* U32 link_id */
           );
#else /*_SNDCP_DTI_2_*/
  UBYTE* neighbor = NULL;
  ULONG linkid = 0;
  BOOL ack = FALSE;
  BOOL direction = HOME;
  UBYTE interfac = SNDCP_INTERFACE_UNACK;

  sndcp_get_nsapi_linkid(nsapi, &linkid);
  sndcp_get_nsapi_neighbor(nsapi, &neighbor);
  sndcp_get_nsapi_ack(nsapi, &ack);
  sndcp_get_nsapi_direction(nsapi, &direction);
#ifndef SNDCP_UPM_INCLUDED
  if (ack) {
    interfac = SNDCP_INTERFACE_ACK;
    sndcp_set_nsapi_interface(nsapi, interfac);
  }
#endif 

  dti_open(sndcp_data->hDTI, /* DTI_HANDLE *hDTI */
           0, /* U8 instance */
           interfac, /* U8 interface */
           nsapi, /* U8 channel */
           0, /* U8 queue_size */
           direction, /* BOOL direction */
           FLOW_CNTRL_ENABLED, /* U8 comm_type */
           DTI_VERSION_10, /* U32 version */
           neighbor, /* U8 *neighbor_entity */
           linkid/* U32 link_id */
           );

#endif /*_SNDCP_DTI_2_*/
} /* mg_dti_open() */

/*
+------------------------------------------------------------------------------
| Function    : mg_init
+------------------------------------------------------------------------------
| Description : The function mg_init() ....
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_init (void)
{

  UBYTE sapi_index = 0;
  UBYTE nsapi = 0;
  TRACE_FUNCTION( "mg_init" );
  INIT_STATE(MG, MG_DEFAULT);
  /*
   * req_xid_block, cnf_xid_block, cur_xid_block not initialized.
   */
  for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {

    mg_reset_states_n_rej(sapi_index);
    /*
     * Init renegotiation counter and cur_xid_block with default values.
     */
    sndcp_data->mg.renego[sapi_index] = 0;

    sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.cnf_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.ind_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.res_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.new_xid_block[sapi_index]);

  }
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    sndcp_reset_xid_block(&sndcp_data->mg.user_xid_block[nsapi]);
    sndcp_data->mg.user_xid_block[nsapi].vj.s0_m_1 = 0;
    sndcp_data->cur_pcomp[nsapi] = 0;
    sndcp_data->cur_dcomp[nsapi] = 0;
    sndcp_data->cur_seg_pos[nsapi] = 0;
    sndcp_data->cur_pdu_ref[nsapi].ref_nsapi = 0;
    sndcp_data->cur_pdu_ref[nsapi].ref_npdu_num = 0;
    sndcp_data->cur_pdu_ref[nsapi].ref_seg_num = 0;
    sndcp_data->big_head[nsapi] = FALSE;
  }


  sndcp_data->mg.mod_expects = MG_MOD_X_NONE;
  sndcp_data->mg.waiting_nsapis = 0;
  sndcp_data->mg.suspended_nsapis = 0;

} /* mg_init() */



/*
+------------------------------------------------------------------------------
| Function    : mg_is_ack
+------------------------------------------------------------------------------
| Description : This procedure takes the snsm_qos!rely
|               information:
|
| 0 SNSM_RELCLASS_SUB Subscribed reliability class
| 1 SNSM_GTP_LLC_RLC_PROT Acknowledged GTP, LLC, and RLC; Protected data
| 2 SNSM_LLC_RLC_PROT Unacknowledged GTP; Acknowledged LLC and RLC, Protected data
| 3 SNSM_RLC_PROT Unacknowledged GTP and LLC; Acknowledged RLC, Protected data
| 4 SNSM_PROT Unacknowledged GTP, LLC, and RLC, Protected data
| 5 SNSM_NO_REL Unacknowledged GTP, LLC, and RLC, Unprotected data
|
| and sets "spec" to TRUE, "b" to TRUE in case of SNSM_GTP_LLC_RLC_PROT or
| SNSM_LLC_RLC_PROT,
| [should be, is not:["spec" to FALSE in case of SNSM_RELCLASS_SUB]] and "spec"
| to TRUE, "b" to FALSE else.
|
| Important note: in case of SNSM_RELCLASS_SUB "spec" will actually be set to
| to TRUE, "b" to FALSE, to be robust in case of downlink protocol error!
|
| Parameters  : snsm_qos
|                       BOOL* spec (FALSE for REL_CLASS_SUB),
|                       BOOL* b)
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_is_ack (T_snsm_qos snsm_qos,
                       BOOL* spec,
                       BOOL* b)
{
  TRACE_FUNCTION( "mg_is_ack" );


#ifdef SNDCP_UPM_INCLUDED 
  if (snsm_qos.relclass ==  PS_GTP_LLC_RLC_PROT ||
    snsm_qos.relclass == PS_LLC_RLC_PROT) {
#else
  if (snsm_qos.relclass ==  SNSM_GTP_LLC_RLC_PROT ||
    snsm_qos.relclass == SNSM_LLC_RLC_PROT) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
    *spec = TRUE;
    *b = TRUE;
   /*  } else if (snsm_qos.relclass == SNSM_RELCLASS_SUB) {
    *spec = FALSE;
    */
  } else {
    *spec = TRUE;
    *b = FALSE;
  }


} /* mg_is_ack() */

/*
+------------------------------------------------------------------------------
| Function    : mg_no_col_no_re
+------------------------------------------------------------------------------
| Description : The function mg_no_col_no_re represents  the SDL label
|               NO_COL_NO_RE: an LL_ESTABLISH_IND has been received, we do not
|               have a collision situation like given in [GSM 4.65, 6.2.1.4],
|               we do not have a re-establishment situation.
|
| Parameters  : the received LL_ESTABLISH_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_no_col_no_re (T_LL_ESTABLISH_IND* ll_establish_ind)
{
  UBYTE dec_ret = 0;
  UBYTE check_ret = 0;
  UBYTE sapi_index = 0;
  U8 nsapi = 0;
  TRACE_FUNCTION( "mg_no_col_no_re" );
  sndcp_get_sapi_index(ll_establish_ind->sapi, &sapi_index);

  TRACE_EVENT_P3("l3_valid?: %d, N201_I: %d, N201_U: %d", 
                  ll_establish_ind->xid_valid, ll_establish_ind->n201_i, 
                  ll_establish_ind->n201_u);
  /*
   * Set N201 values in uplink services.
   */
  sig_mg_su_n201(ll_establish_ind->sapi, ll_establish_ind->n201_u);
  sig_mg_sua_n201(ll_establish_ind->sapi, ll_establish_ind->n201_i);
  /*
   * If SNDCP XID block is not valid, we are ready.
   */
  if (ll_establish_ind->xid_valid == LL_XID_INVALID) {

    PALLOC_SDU (ll_establish_res, LL_ESTABLISH_RES, 0);
    /*
     * Set sapi in ll_establish_res.
     */
    ll_establish_res->sapi = ll_establish_ind->sapi;
    ll_establish_res->xid_valid = ll_establish_ind->xid_valid;
    ll_establish_res->sdu.l_buf = 0;

    /*
     * Mark the affected sapi as MG_XID_IDLE.
     */
    sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_EST);

    sig_mg_sda_end_est(ll_establish_res->sapi, TRUE);

    sndcp_set_sapi_ack(ll_establish_res->sapi, TRUE);

    PSEND(hCommLLC, ll_establish_res);

    /*
     * All nsapis at this sapi that use ack mode, enter recovery state.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
      UBYTE sapi = 0;
      BOOL ack = FALSE;

      sndcp_get_nsapi_sapi(nsapi, &sapi);
      sndcp_get_nsapi_ack(nsapi, &ack);
      if (ack && (sapi == ll_establish_ind->sapi)) {
        sig_mg_cia_delete_npdus(nsapi);
        sig_mg_sua_delete_pdus(nsapi, sapi, FALSE);
        sig_mg_nu_recover(nsapi);
        sig_mg_nd_recover(nsapi);
      }
    } /* for all nsapis */

    return;
  }
  /*
   * SNDCP XID block is valid and checked now.
   */
  mg_decode_xid(&(ll_establish_ind->sdu),
                &(sndcp_data->mg.ind_xid_block[sapi_index]),
                &dec_ret,
                ll_establish_ind->sapi);
  if (dec_ret == MG_XID_OK) {
    mg_check_ind_xid(&check_ret, ll_establish_ind->sapi);
    if (check_ret == MG_XID_OK) {
      /*
       * Label MG_IND_OK_EST
       */
      USHORT res_sdu_bit_len = 0;
      UBYTE ntt = 0;
      BOOL v42_rej = FALSE;
      BOOL vj_rej = FALSE;

      mg_set_res_cur_xid_block(ll_establish_ind->sapi, &res_sdu_bit_len);
      /*
       * Add the extra space for ntts with nsapis == 0.
       */
      for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
        BOOL rej = FALSE;
        mg_get_sapi_dntt_rej(ll_establish_ind->sapi, ntt, &rej);
        if (rej) {
          /*
           * length of ntt octet and nsapis.
           */
          res_sdu_bit_len += 32;
          v42_rej = TRUE;
        }
        mg_get_sapi_pntt_rej(ll_establish_ind->sapi, ntt, &rej);
        if (rej) {
          /*
           * length of ntt octet and nsapis.
           */
          res_sdu_bit_len += 32;
          vj_rej = TRUE;
        }
      }
      if (! sndcp_data->mg.res_xid_block[sapi_index].v42.is_set &&
          v42_rej) {
        /*
         * Add length of parameter type and length.
         */
        res_sdu_bit_len += 16;
      }
      if (! sndcp_data->mg.res_xid_block[sapi_index].vj.is_set &&
          vj_rej) {
        /*
         * Add length of parameter type and length.
         */
        res_sdu_bit_len += 16;
      }

      /*
       * SDL Label MG_CNF_OK_ACK
       */

      {

        USHORT sapi_state = MG_IDLE;

        PALLOC_SDU (ll_establish_res, LL_ESTABLISH_RES, res_sdu_bit_len);
        /*
         * Set sapi in ll_establish_res.
         */
        ll_establish_res->sapi = ll_establish_ind->sapi;
        ll_establish_res->xid_valid = ll_establish_ind->xid_valid;
        /*
         * Write res_xid_block struct to sdu byte buffer. Implementation dep..
         */
        mg_set_res_xid_params(&ll_establish_res->sdu, ll_establish_res->sapi);

        /*
         * Mark the affected nsapis and sapi as MG_XID_IDLE.
         */
        sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_XID);
        sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_EST);

        sig_mg_sda_end_est(ll_establish_res->sapi, TRUE);

        sndcp_set_sapi_ack(ll_establish_res->sapi, TRUE);

        PSEND(hCommLLC, ll_establish_res);

        /*
         * All nsapis at this sapi that use ack mode, enter recovery state.
         */
        for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
          UBYTE sapi = 0;
          BOOL ack = FALSE;

          sndcp_get_nsapi_sapi(nsapi, &sapi);
          sndcp_get_nsapi_ack(nsapi, &ack);
          if (ack && (sapi == ll_establish_ind->sapi)) {
            sig_mg_cia_delete_npdus(nsapi);
            sig_mg_sua_delete_pdus(nsapi, sapi, FALSE);
            sig_mg_nu_recover(nsapi);
            sig_mg_nd_recover(nsapi);
          }
        } /* for all nsapis */

        /*
         * Reset nsapis or ntts that were assigned, but are not any more.
         */
        mg_clean_xid(ll_establish_ind->sapi);
        /*
         * If there was a collision and xid has not been negotiated
         * sufficiently.
         */
        mg_resend_xid_if_nec(ll_establish_ind->sapi);

        /*
         * If nsapi has been in state xid_pending or est_pending then
         * an snsm_activate_res will be sent now.!!!
         */
        mg_respond_if_nec(ll_establish_ind->sapi);
        sndcp_get_sapi_state(ll_establish_ind->sapi, &sapi_state);
        if ((sapi_state & MG_XID) == 0) {
          mg_xid_cnf_ok_res(ll_establish_ind->sapi);
        }


      }

    } else {
      /*
       * not (check_ret == MG_IND_XID_OK)
       */
      /*
       * Label MG_CHECK_FAIL_EST
       */
      USHORT res_sdu_bit_len = 0;
      UBYTE ntt = 0;
      BOOL v42_rej = FALSE;
      BOOL vj_rej = FALSE;
      /*
       * Add the extra space for ntts with nsapis == 0.
       */
      for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
        BOOL rej = FALSE;
        mg_get_sapi_dntt_rej(ll_establish_ind->sapi, ntt, &rej);
        if (rej) {
          /*
           * length of ntt octet and nsapis.
           */
          res_sdu_bit_len += 32;
          v42_rej = TRUE;
        }
        mg_get_sapi_pntt_rej(ll_establish_ind->sapi, ntt, &rej);
        if (rej) {
          /*
           * length of ntt octet and nsapis.
           */
          res_sdu_bit_len += 32;
          vj_rej = TRUE;
        }
      }
      if (v42_rej) {
        /*
         * Add length of parameter type and length.
         */
        res_sdu_bit_len += 16;
      }
      if (vj_rej) {
        /*
         * Add length of parameter type and length.
         */
        res_sdu_bit_len += 16;
      }
      /*
       * Allocate response and send it.
       */
      {

        PALLOC_SDU (ll_establish_res, LL_ESTABLISH_RES, res_sdu_bit_len);
        /*
         * Reset res_xid_block, ind_xid_block.
         */
        sndcp_reset_xid_block(&sndcp_data->mg.res_xid_block[sapi_index]);
        sndcp_reset_xid_block(&sndcp_data->mg.ind_xid_block[sapi_index]);
        /*
         * Set sapi in ll_establish_res.
         */
        ll_establish_res->sapi = ll_establish_ind->sapi;
        ll_establish_res->xid_valid = ll_establish_ind->xid_valid;
        /*
         * Write res_xid_block struct to sdu byte buffer. Implementation dep..
         */
        mg_set_res_xid_params(&ll_establish_res->sdu, ll_establish_ind->sapi);
        /*
         * Modify the affected sapi state.
         */
        sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_XID);
        sndcp_unset_sapi_state(ll_establish_ind->sapi, MG_EST);

        sndcp_set_sapi_ack(ll_establish_res->sapi, TRUE);
        /*
         * Send the XID block to LLC.
         */
        PSEND(hCommLLC, ll_establish_res);
                /*
         * All nsapis at this sapi that use ack mode, enter recovery state.
         */
        for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
          UBYTE sapi = 0;
          BOOL ack = FALSE;

          sndcp_get_nsapi_sapi(nsapi, &sapi);
          sndcp_get_nsapi_ack(nsapi, &ack);
          if (ack && (sapi == ll_establish_ind->sapi)) {
            sig_mg_cia_delete_npdus(nsapi);
            sig_mg_sua_delete_pdus(nsapi, sapi, FALSE);
            sig_mg_nu_recover(nsapi);
            sig_mg_nd_recover(nsapi);
          }
        } /* for all nsapis */

      }

      /*
       * Reset nsapis or ntts that were assigned before
       * but are not anymore.
       */
      sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
      mg_clean_xid(ll_establish_ind->sapi);
      /*
       * Allocate status req and send it  (label MG_SEND_STATUS_REQ_EST).
       */
      {
#ifdef SNDCP_UPM_INCLUDED 
        PALLOC (snsm_status_req, SN_STATUS_IND);
#else
        PALLOC (snsm_status_req, SNSM_STATUS_REQ);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
        snsm_status_req->sapi = ll_establish_ind->sapi;
#ifdef SNDCP_UPM_INCLUDED 
        snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
        snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_INVALID_XID;
        PSEND (hCommUPM, snsm_status_req);
#else
        snsm_status_req->status_cause = SNSM_RELCS_INVALID_XID;
        PSEND (hCommSM, snsm_status_req);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
      }
    }
  } else { /* not if (dec_ret == MG_XID_OK_EST) */
    /*
     * Reset nsapis or ntts that were assigned before
     * but are not anymore.
     */
    sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
    mg_clean_xid(ll_establish_ind->sapi);
    /*
     * Decoding of ll_establish_ind failed (label MG_SEND_STATUS_REQ).
     * Allocate status req and send it.
     */

    {
#ifdef SNDCP_UPM_INCLUDED 
      PALLOC (snsm_status_req, SN_STATUS_IND);
      snsm_status_req->sapi = ll_establish_ind->sapi;
      snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
      snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_INVALID_XID;
      PSEND (hCommUPM, snsm_status_req);
#else
      PALLOC (snsm_status_req, SNSM_STATUS_REQ);
      snsm_status_req->sapi = ll_establish_ind->sapi;
      snsm_status_req->status_cause = SNSM_RELCS_INVALID_XID;
      PSEND (hCommSM, snsm_status_req);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    }

  }

} /* mg_no_col_no_re() */

/*
+------------------------------------------------------------------------------
| Function    : mg_no_col_re
+------------------------------------------------------------------------------
| Description : The function mg_no_col_no_re represents  the SDL label
|               NO_COL_RE: an LL_ESTABLISH_IND has been received, we do not
|               have a collision situation like given in [GSM 4.65, 6.2.1.4],
|               we do have a re-establishment situation.
|
| Parameters  : the received LL_ESTABLISH_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_no_col_re (T_LL_ESTABLISH_IND* ll_establish_ind)
{

  TRACE_FUNCTION( "mg_no_col_re" );
  /*
   * Do the same things as without collision.
   */
  mg_no_col_no_re (ll_establish_ind);

} /* mg_no_col_re() */

/*
+------------------------------------------------------------------------------
| Function    : mg_re_negotiate
+------------------------------------------------------------------------------
| Description : The answer to the sent LL_XID_REQ has been invalid
| (bad format or content). If service var renego < MG_MAX_RENEGO then
| same LL_XID_REQ is resent, else SNSM_STATUS_REQ is sent.
| This function represents the SDl label MG_RE_NEGOTIATE.
|
| Parameters  : the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_re_negotiate (UBYTE sapi) {
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "mg_re_negotiate" );
  sndcp_get_sapi_index(sapi, &sapi_index);

  /*
   * If number of re-negotiations is reached, deactivate all pdp contexts for
   * the affected sapi.
   */
  if (sndcp_data->mg.renego[sapi_index] < MG_MAX_RENEGO) {
    PALLOC_SDU(ll_xid_req, LL_XID_REQ, SNDCP_XID_BLOCK_BIT_LEN);
    /*
     * Set sapi in ll_xid_req.
     */
    ll_xid_req->sapi = sapi;

    /*
     * Fill the XID block. Implementation dependent.
     */
    mg_set_xid_params(ll_xid_req->sapi,
                      &ll_xid_req->sdu,
                      sndcp_data->mg.req_xid_block[sapi_index]);
    /*
     * Send the XID block to LLC.
     */

    sndcp_unset_sapi_state(sapi, MG_XID_NEC);
    sndcp_set_sapi_state(ll_xid_req->sapi, MG_XID);
    PSEND(hCommLLC, ll_xid_req);
    /*
     * Increment renegotiation counter.
     */
    sndcp_data->mg.renego[sapi_index]++;
  } else {

#ifdef SNDCP_UPM_INCLUDED 
    PALLOC (snsm_status_req, SN_STATUS_IND);
#else
    PALLOC (snsm_status_req, SNSM_STATUS_REQ);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    /*
     * Set all ntts and dcomp/pcomp states to "unassigned".
     */
    mg_reset_states_n_rej(sapi_index);
    /*
     * Set prim parameters.
     */
#ifdef SNDCP_UPM_INCLUDED 
    snsm_status_req->sapi = sapi;
    snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
    snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_INVALID_XID;
#else
    snsm_status_req->sapi = sapi;
    snsm_status_req->status_cause = SNSM_RELCS_INVALID_XID;
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/

    sndcp_data->mg.renego[sapi_index] = 0;
    /*
     * Reset cur_xid_block.
     */
    sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
    /*
     * Reset all compression entities for the affected sapi.
     */
    mg_clean_xid(sapi);

#ifdef SNDCP_UPM_INCLUDED 
    PSEND (hCommUPM, snsm_status_req);
#else
    PSEND (hCommSM, snsm_status_req);
#endif 
  }


} /* mg_re_negotiate() */

/*
+------------------------------------------------------------------------------
| Function    : mg_re_negotiate_ack
+------------------------------------------------------------------------------
| Description : The answer to the sent LL_ESTABLISH_REQ has been invalid
| (bad format or content). If service var renego < MG_MAX_RENEGO then
| same LL_ESTABLISH_REQ is resent, else SNSM_STATUS_REQ is sent.
| This function represents the SDl label MG_RE_NEGOTIATE_ACK.
|
| Parameters  : the affected sapi, cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_re_negotiate_ack (UBYTE sapi, U16 cause) {
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "mg_re_negotiate_ack" );
  sndcp_get_sapi_index(sapi, &sapi_index);

  /*
   * If number of re-negotiations is reached, deactivate all pdp contexts for
   * the affected sapi.
   */
  if (sndcp_data->mg.renego[sapi_index] < MG_MAX_RENEGO) {
    PALLOC_SDU(ll_establish_req,
               LL_ESTABLISH_REQ,
               SNDCP_XID_BLOCK_BIT_LEN);
    /*
     * Set sapi in ll_establish_req.
     */
    ll_establish_req->sapi = sapi;

    /*
     * Fill the XID block. Implementation dependent.
     */
    mg_set_xid_params(ll_establish_req->sapi,
                      &ll_establish_req->sdu,
                      sndcp_data->mg.req_xid_block[sapi_index]);
    /*
     * Send the XID block to LLC, with establish request.
     */
    sig_mg_sda_start_est(sapi);

    PSEND(hCommLLC, ll_establish_req);
    /*
     * Increment renegotiation counter.
     */
    sndcp_data->mg.renego[sapi_index]++;
  } else {

#ifdef SNDCP_UPM_INCLUDED
    PALLOC (snsm_status_req, SN_STATUS_IND);
#else
    PALLOC (snsm_status_req, SNSM_STATUS_REQ);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    /*
     * Service sda may now leave state SDA_ESTABLISH_REQUESTED.
     */
    sig_mg_sda_end_est(sapi, FALSE);
    /*
     * Set all ntts and dcomp/pcomp states to "unassigned".
     */
    mg_reset_states_n_rej(sapi_index);
    /*
     * Set prim parameters.
     */
#ifdef SNDCP_UPM_INCLUDED 
    snsm_status_req->sapi = sapi;
    snsm_status_req->ps_cause.ctrl_value = CAUSE_is_from_sndcp;
#else
    snsm_status_req->sapi = sapi;
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/

    sndcp_data->mg.renego[sapi_index] = 0;
    /*
     * Reset cur_xid_block.
     */
    sndcp_reset_xid_block(&sndcp_data->mg.cur_xid_block[sapi_index]);
    sndcp_reset_xid_block(&sndcp_data->mg.req_xid_block[sapi_index]);
    /*
     * Reset all compression entities for the affected sapi.
     */
    mg_clean_xid(sapi);
    /*
     * The renegotiation is failed. If the cause is NO_PEER_RESPONSE
     * or DM_RECEIVED, set it to RELCS_NORMAL to make SM deactivate
     * PDP context. Otherwise the cause is forwarded to SM and the SM
     * will deactivate PDP context depending on cause.
     */
#ifdef SNDCP_UPM_INCLUDED 
    if((cause == CAUSE_SN_NO_PEER_RESPONSE) ||
       (cause == CAUSE_SN_DM_RECEIVED) ){
      snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_NORMAL_RELEASE;
#else /* SNDCP_UPM_INCLUDED */    
#ifdef _SNDCP_DTI_2_
    if((cause == LL_RELCS_NO_PEER_RES) ||
       (cause == LL_RELCS_DM_RECEIVED) ){
      snsm_status_req->ps_cause.value.sn_cause = LL_RELCS_NORMAL;
#else
    if((cause == CAUSE_SN_NO_PEER_RESPONSE) ||
       (cause == CAUSE_SN_DM_RECEIVED) ){
      snsm_status_req->ps_cause.value.sn_cause = CAUSE_SN_NORMAL_RELEASE;
#endif 
#endif  /* SNDCP_UPM_INCLUDED */
      sig_mg_su_resume(sapi);
      sig_mg_sua_resume(sapi);
      mg_resume_affected_nus(sapi);
      sndcp_unset_sapi_state (sapi, MG_EST);
    } else {
#ifdef SNDCP_UPM_INCLUDED 
      snsm_status_req->ps_cause.value.sn_cause = cause;
#else      
      snsm_status_req->status_cause = cause;
#endif 
    }
	
#ifdef SNDCP_UPM_INCLUDED 
    PSEND (hCommUPM, snsm_status_req);
#else
    PSEND (hCommSM, snsm_status_req);
#endif 

  }

} /* mg_re_negotiate_ack() */


/*
+------------------------------------------------------------------------------
| Function    : mg_resend_xid_if_nec
+------------------------------------------------------------------------------
| Description : This procedure will be called after reception and computation
| of an LL_XID_IND or LL_ESTABLISH_IND in case of a collision
| or after receiving LL_XID_CNF or LL_ESTABLISH_CNF and does the
| following:
| (GSM 04.65 version 6.5.1 Release 1997), 6.2.1.4:
| If the
| LL-ESTABLISH.request or LL-XID.request contains one or more XID parameters,
| or one or more compression fields
| in an XID parameter, or one or more parameters in a compression field,
| that are not negotiated as part of the collision
| resolution, then negotiation of these XID parameters shall be performed at
| the earliest opportunity after conclusion of
| the collision resolution.
|
| Parameters  : Affected sapi, out: was LL_XID_REQ sent?
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_resend_xid_if_nec (UBYTE sapi) {
  UBYTE sapi_index = 0;
  BOOL resend_necessary = FALSE;
  USHORT sapi_state = MG_IDLE;
  T_XID_BLOCK* req_xid_block = NULL;
  T_XID_BLOCK* cur_xid_block = NULL;
  T_XID_BLOCK* new_xid_block = NULL;

  TRACE_FUNCTION( "mg_resend_xid_if_nec" );

  sndcp_get_sapi_index(sapi, &sapi_index);
  req_xid_block = &sndcp_data->mg.req_xid_block[sapi_index];
  cur_xid_block = &sndcp_data->mg.cur_xid_block[sapi_index];
  new_xid_block = &sndcp_data->mg.new_xid_block[sapi_index];

  /*
   * Data compression.
   */
  /*
   * If a context is deactivated, maybe compressors must be switched off.
   */
  if (cur_xid_block->v42.is_set) {
    USHORT nsapis = cur_xid_block->v42.nsapis;
    UBYTE nsapi = 0;

    /*
     * XID renegotiation will only be necessary if one of the nsapis
     * that are requested to use the data compression is in state
     * MG_DEACT.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & nsapis) > 0) {
        USHORT state = MG_IDLE;
        sndcp_get_nsapi_state(nsapi, &state);
        if ((state & MG_DEACT) > 0) {
          UBYTE dntt = cur_xid_block->v42.ntt;
          resend_necessary = TRUE;
          mg_set_sapi_dntt_rej(sapi, dntt, TRUE);
        }
      }
    }
  }

  if (req_xid_block->v42.is_set && ! cur_xid_block->v42.is_set) {
    UBYTE sapi_index_local = 0;
    BOOL used = FALSE;
    /*
     * If one instance of v42 is used at a different sapi,
     * we may not use another one here.
     */
    for (sapi_index_local = 0;
         sapi_index_local < SNDCP_NUMBER_OF_SAPIS;
         sapi_index_local++) {

      if (sndcp_data->mg.cur_xid_block[sapi_index_local].v42.is_set) {
        used = TRUE;
      }
    }
    if (! used) {
      USHORT nsapis = req_xid_block->v42.nsapis;
      UBYTE nsapi = 0;

      /*
       * XID renegotiation will only be necessary if one of the nsapis
       * that are requested to use the data compression are not all in state
       * MG_DEACT.
       */
      for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
        if (((1 << nsapi) & nsapis) > 0) {
          USHORT state = MG_IDLE;
          sndcp_get_nsapi_state(nsapi, &state);
          if ((state & MG_DEACT) == 0) {
            resend_necessary = TRUE;
          }
        }
      }

    }
  }
  /*
   * Header compression.
   */
  /*
   * If a context is deactivated, maybe compressors must be switched off.
   */
  if (cur_xid_block->vj.is_set) {
    USHORT nsapis = cur_xid_block->vj.nsapis;
    UBYTE nsapi = 0;

    /*
     * XID renegotiation will only be necessary if one of the nsapis
     * that are requested to use the header compression is in state
     * MG_DEACT.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & nsapis) > 0) {
        USHORT state = MG_IDLE;
        sndcp_get_nsapi_state(nsapi, &state);
        if ((state & MG_DEACT) > 0) {
          UBYTE pntt = cur_xid_block->vj.ntt;
          resend_necessary = TRUE;
          mg_set_sapi_pntt_rej(sapi, pntt, TRUE);
        }
      }
    }

  }
  /*
   * If a compressor is requested and not yet negotiated it must be requested
   * now.
   */
  if (req_xid_block->vj.is_set && !cur_xid_block->vj.is_set) {
    USHORT nsapis = req_xid_block->vj.nsapis;
    UBYTE nsapi = 0;

    req_xid_block->vj.p_bit = 1;

    /*
     * XID renegotiation will only be necessary if the affected nsapis
     * is not currently being deactivated.
     */
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & nsapis) > 0) {
        USHORT state = MG_IDLE;
        sndcp_get_nsapi_state(nsapi, &state);
        if ((state & MG_DEACT) == 0) {
          resend_necessary = TRUE;
        }
      }
    }

  }

  /*
   * If in the meantime more compression has been requested,
   * or compressors must be deactivated,
   * re-negotiate.
   */
  if (new_xid_block->v42.is_set) {
    if ((! req_xid_block->v42.is_set) ||
        (req_xid_block->v42.is_set &&
         (new_xid_block->v42.nsapis != req_xid_block->v42.nsapis))) {

      *req_xid_block = *new_xid_block;
      resend_necessary = TRUE;
    }
  }

  /*
   * If in the meantime more compression has been requested,
   * or compressors must be deactivated,
   * re-negotiate.
   */
  if (new_xid_block->vj.is_set) {
    if ((! req_xid_block->vj.is_set) ||
        (req_xid_block->vj.is_set &&
         (new_xid_block->vj.nsapis != req_xid_block->vj.nsapis))) {

      *req_xid_block = *new_xid_block;
      resend_necessary = TRUE;
    }
  }


  sndcp_get_sapi_state(sapi, &sapi_state);
  /*
   * If re-negotiation is necessary but not possible because MG_REL or
   * MG_XID, set MG_XID_NEC.
   */
  if (resend_necessary
      &&
      ((sapi_state & (MG_REL + MG_XID)) > 0))
  {
    sndcp_set_sapi_state(sapi, MG_XID_NEC);
    mg_set_cur_xid_block(sapi);
    return;
  }


  /*
   * If renegotiation of XID is necessary, send LL_XID_REQ.
   */
  if (! resend_necessary) {
//    mg_set_cur_xid_block(sapi);
    return;
  }


  /*
   * Now req_xid_block is in good shape. Send it.
   */
  {
    PALLOC_SDU(ll_xid_req, LL_XID_REQ, SNDCP_XID_BLOCK_BIT_LEN);
    /*
     * Set sapi in ll_xid_req.
     */
    ll_xid_req->sapi = sapi;
    /*
     * Write data from snsm_activate_ind to service variable req_xid_block.
     */
    mg_set_ntt_comp(sapi);
    /*
     * Fill the XID block. Implementation dependent.
     */
    mg_set_xid_params(ll_xid_req->sapi,
                      &ll_xid_req->sdu,
                      sndcp_data->mg.req_xid_block[sapi_index]);
    /*
     * Mark the affected sapi as MG_XID.
     */
    sndcp_set_sapi_state(sapi, MG_XID);
    sndcp_unset_sapi_state(sapi, MG_XID_NEC);

    /*
     * Uplink data transfer on SAPI is completely suspended.
     */
    sig_mg_su_suspend(sapi);
    sig_mg_sua_suspend(sapi);
    mg_suspend_affected_nus(sapi);
    /*
     * Send the XID block to LLC.
     */

    sndcp_unset_sapi_state(sapi, MG_XID_NEC);
    sndcp_set_sapi_state(ll_xid_req->sapi, MG_XID);
    PSEND(hCommLLC, ll_xid_req);
  }

} /* mg_resend_xid_if_nec() */



/*
+------------------------------------------------------------------------------
| Function    : mg_resume_affected_nus
+------------------------------------------------------------------------------
| Description : Resumes all nu service instances affected by
|               If nsapi is waiting for SNSM_SEQUENCE_IND, fct. returns.
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_resume_affected_nus (UBYTE sapi) {
  UBYTE nsapi = 0;
  TRACE_FUNCTION( "mg_resume_affected_nus" );
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    UBYTE local_sapi = 0;
    sndcp_get_nsapi_sapi(nsapi, &local_sapi);
    if (local_sapi == sapi) {
      USHORT nsapi_state = 0;
      sndcp_get_nsapi_state(nsapi, &nsapi_state);
      if ((nsapi_state & MG_SEQ) > 0) {
        return;
      }
      if (((1 << nsapi) & sndcp_data->mg.suspended_nsapis) > 0) {
        sig_mg_nu_resume(nsapi);
        sndcp_data->mg.suspended_nsapis &= (~ (ULONG)(1 << nsapi));
      }
    }
  }
} /* mg_resume_affected_nus() */


/*
+------------------------------------------------------------------------------
| Function    : mg_is_rel_comp_nec
+------------------------------------------------------------------------------
| Description : This function will be called in course of the deactivation of
|               the given nsapi. If the nsapi used a certain compressor,
|               but has been the only one to do this, then with the
|               compressor entity will also have to
|               be deactivated and all affected
|               arrays will be modified.
| Parameters  : UBYTE nsapi -- the given nsapi
|               BOOL* nec,  a compressor
| Post        : An "mg_set_sapi_[p/d]ntt_rej(sapi, pntt, TRUE);" will be called
|               for
|               each entity to be released. Later this information may be used
|               to build up the right xid negotiation.
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_is_rel_comp_nec (UBYTE nsapi, BOOL* nec)
{
  UBYTE dntt = 0;
  UBYTE pntt = 0;
  UBYTE sapi = 0;
  UBYTE nsp = 0;

  TRACE_FUNCTION( "mg_is_rel_comp_nec" );

  sndcp_get_nsapi_sapi(nsapi, &sapi);


  /*
   * Which pntt is used?
   */
  for (pntt = 0; pntt < MG_MAX_ENTITIES; pntt++) {
    UBYTE state = MG_UNASSIGNED;
    BOOL used = FALSE;
    BOOL another = FALSE;
    BOOL rej_known = FALSE;
    /*
     * Is pntt already known to be deactivated?
     */
    mg_get_sapi_pntt_rej(sapi, pntt, &rej_known);
    if (rej_known) {
      *nec = TRUE;
      continue;
    }
    /*
     * Is ntt used?
     */
    mg_get_sapi_pntt_state(sapi, pntt, &state);
    if (state == MG_UNASSIGNED) {
      continue;
    }
    /*
     * Does the given nsapi use it?
     */
    mg_get_sapi_pntt_nsapi(sapi, pntt, nsapi, &used);
    if (! used) {
      continue;
    }
    /*
     * Is the given nsapi the only user?
     */
    for (nsp = 0; nsp < SNDCP_NUMBER_OF_NSAPIS; nsp++) {
      mg_get_sapi_pntt_nsapi(sapi, pntt, nsp, &used);
      if (used && nsapi != nsp) {
        another = TRUE;
      }
    }
    if (another) {
      continue;
    }
    mg_set_sapi_pntt_rej(sapi, pntt, TRUE);
    *nec = TRUE;
  }

  /*
   * Which dntt is used?
   */
  for (dntt = 0; dntt < MG_MAX_ENTITIES; dntt++) {
    UBYTE state = MG_UNASSIGNED;
    BOOL used = FALSE;
    BOOL another = FALSE;
    BOOL rej_known = FALSE;
    /*
     * Is pntt already known to be deactivated?
     */
    mg_get_sapi_dntt_rej(sapi, dntt, &rej_known);
    if (rej_known) {
      *nec = TRUE;
      continue;
    }
    /*
     * Is ntt used?
     */
    mg_get_sapi_dntt_state(sapi, dntt, &state);
    if (state == MG_UNASSIGNED) {
      continue;
    }
    /*
     * Does the given nsapi use it?
     */
    mg_get_sapi_dntt_nsapi(sapi, dntt, nsapi, &used);
    if (! used) {
      continue;
    }
    /*
     * Is the given nsapi the only user?
     */
    for (nsp = 0; nsp < SNDCP_NUMBER_OF_NSAPIS; nsp++) {
      mg_get_sapi_dntt_nsapi(sapi, dntt, nsp, &used);
      if (used && nsapi != nsp) {
        another = TRUE;
      }
    }
    if (another) {
      continue;
    }

    mg_set_sapi_dntt_rej(sapi, dntt, TRUE);
    *nec = TRUE;
  }

} /* mg_is_rel_comp_nec() */


/*
+------------------------------------------------------------------------------
| Function    : mg_rel_nsapi_nec
+------------------------------------------------------------------------------
| Description : The SAPI connected to this NSAPI shall release acknowledged LLC
|               operation mode if the indicated NSAPI is the last one using
|               acknowledged mode on this SAPI. If an LL_RELEASE_REQ is sent
|               then the given NSAPI (the one in parameter!) shall enter state
|               MG_REL and wait for an LL_RELEASE_CNF. If the affected
|               NSAPI doesn't use ack mode then the procedure just returns.
|               If the affected sapi does not use acknowledged LLC operation
|               mode then the procedure just returns.
|               If an LL_ESTABLISH_REQ or LL_XID_REQ for the affected sapi is
|               pending, no LL_RELEASE_REQ primitive will be sent, but the
|               flag MG_REL_NEC_LOC will be set.
| Pre         : This procedure is called after receipt of an
|               SNSM_DEACTIVATE_IND, so the "local" parameter in an
|               LL_RELEASE_REQ will be set to LL_REL_LOCAL.
|               The procedure will only be called if the given nsapi is
|               currently using acknowledged LLC operation mode, so this
|               does not have to be checked.
| Parameters  : UBYTE nsapi -- the given nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_rel_nsapi_nec (UBYTE nsapi)
{
  UBYTE sapi = 0;
  UBYTE npi = 0;
  UBYTE spi = 0;
  USHORT sapi_state = MG_IDLE;
  /*
   * Are there other contexts using acknowledged mode on the same SAPI?
   */
  BOOL another_ack = FALSE;
  BOOL sack = FALSE;
  TRACE_FUNCTION( "mg_rel_nsapi_nec" );
  /*
   * Which SAPI is connected to the given NSAPI?
   */
  sndcp_get_nsapi_sapi(nsapi, &sapi);
  sndcp_get_sapi_ack(sapi, &sack);
  sndcp_get_sapi_state(sapi, &sapi_state);

  if(!sack){
    if ((sapi_state & MG_EST) != 0) {
      sndcp_set_sapi_state(sapi, MG_REL_NEC_LOC);
    }
    return;
  }
  /*
   * If release is pending, no need to send an other release.
   */
  if ((sapi_state & MG_REL) > 0) {
    return;
  }
  /*
   * Are there other contexts using acknowledged mode on the same SAPI?
   */
  for (npi = 0; npi < SNDCP_NUMBER_OF_NSAPIS; npi++) {
    BOOL used = FALSE;
    sndcp_is_nsapi_used(npi, &used);
    if (!used) {
      continue;
    }
    sndcp_get_nsapi_sapi(npi, &spi);
    if (spi == sapi && npi != nsapi) {
      BOOL is_ack = FALSE;
      sndcp_get_nsapi_ack(npi, &is_ack);
      if (is_ack) {
        another_ack = TRUE;
      }
      break;
    }
  }
  if (!another_ack) {

    if (((sapi_state & MG_EST) == 0)
        &&
        ((sapi_state & MG_XID) == 0))
    {
      /*
       * No LL_ESTABLISH_REQ or LL_XID_REQ pending.
       * LL_RELEASE_REQ may be sent.
       */

      PALLOC(ll_release_req, LL_RELEASE_REQ);
      ll_release_req->sapi = sapi;
      /*
       * Note: this is always set to TRUE because the preconditions include that
       * this procedure has been called after an SNSM_DEACTIVATE_IND.
       * If this precondition changes the local flag will have to be a parameter.
       * (GSM 4.65, 6.2.2.2).
       */
      ll_release_req->local = TRUE;
      /*
       * Set the "state" for the affected sapi to MG_REL.
       */
      sndcp_set_sapi_state(sapi, MG_REL);
      sndcp_unset_sapi_state(sapi, MG_EST);

      PSEND(hCommLLC, ll_release_req);

    } else {
      /*
       * LL_ESTABLISH_REQ or LL_XID_REQ pending.
       * LL_RELEASE_REQ may not be sent.
       */
      sndcp_set_sapi_state(sapi, MG_REL_NEC_LOC);

    }
  } /* if (!another_ack)  */


} /* mg_rel_nsapi_nec() */

/*
+------------------------------------------------------------------------------
| Function    : mg_send_empty_xid_req
+------------------------------------------------------------------------------
| Description : This procedure sets the pending-states of the affected sapi
|               and nsapi to MG_XID_PENDING and sends an LL_XID_REQ with
|               the sapi from the given snsm_activate_ind and an empty XID
|               block.
|
| Parameters  : the SNSM_ACTIVATE_IND
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_send_empty_xid_req (T_SN_ACTIVATE_REQ* snsm_activate_ind)
#else
GLOBAL void mg_send_empty_xid_req (T_SNSM_ACTIVATE_IND* snsm_activate_ind)
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
{
  TRACE_FUNCTION( "mg_send_empty_xid_req" );
  {
    PALLOC_SDU(ll_xid_req, LL_XID_REQ, SNDCP_XID_BLOCK_BIT_LEN);
    /*
     * Set sapi in ll_xid_req.
     */
    ll_xid_req->sapi = snsm_activate_ind->sapi;
    ll_xid_req->sdu.l_buf = 0;

    /*
     * Mark sapi as pending.
     */
    sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_XID);
    /*
     * Send the XID block to LLC.
     */

    sndcp_unset_sapi_state(ll_xid_req->sapi, MG_XID_NEC);
    sndcp_set_sapi_state(ll_xid_req->sapi, MG_XID);
    PSEND(hCommLLC, ll_xid_req);
  }
} /* mg_send_xid_req() */

/*
+------------------------------------------------------------------------------
| Function    : mg_send_snsm_activate_res
+------------------------------------------------------------------------------
| Description : Allocates prim, sets parameters and sends prim
|
| Parameters  : the affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_send_snsm_activate_res (UBYTE nsapi)
{

  TRACE_FUNCTION( "mg_send_snsm_activate_res" );
  {
    UBYTE sapi_index = 0;
    UBYTE sapi = 0;

#ifdef SNDCP_UPM_INCLUDED 
    PALLOC(snsm_activate_res, SN_ACTIVATE_CNF);
#else
    PALLOC(snsm_activate_res, SNSM_ACTIVATE_RES); 
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/

    sndcp_get_nsapi_sapi(nsapi, &sapi);
    sndcp_get_sapi_index(sapi, &sapi_index);

    snsm_activate_res->nsapi = nsapi;

    /*
     * If nsapi uses data compressor, set dcomp parameter
     * in snsm_activate_res.
     */
    if (sndcp_data->mg.cur_xid_block[sapi_index].v42.is_set &&
        ((sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis &
         (1 << nsapi)) > 0)) {

#ifdef SNDCP_UPM_INCLUDED 
      snsm_activate_res->comp_params.dcomp = 
        sndcp_data->mg.cur_xid_block[sapi_index].v42.p0;
#else
      snsm_activate_res->dcomp =
        sndcp_data->mg.cur_xid_block[sapi_index].v42.p0; 
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    } else
    {
#ifdef SNDCP_UPM_INCLUDED 
    snsm_activate_res->comp_params.dcomp = 0;
#else
    snsm_activate_res->dcomp = 0; 
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    } 

    /*
     * If nsapi uses header compressor, set hcomp parameter
     * and msid field in snsm_activate_res.
     */

#ifdef SNDCP_UPM_INCLUDED 
    if (sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set &&
        ((sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis &
         (1 << nsapi)) > 0)) {
      snsm_activate_res->comp_params.msid = 
        sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1 + 1;
      /*
       * How is that one negotiated?
       * Missing in VJ XID block.
       */
      snsm_activate_res->comp_params.hcomp = 
        sndcp_data->mg.cur_xid_block[sapi_index].vj.direction;
    } else {
      snsm_activate_res->comp_params.hcomp = 0;
      snsm_activate_res->comp_params.msid = 0;
    }

    PSEND(hCommUPM, snsm_activate_res);
#else  /*#ifdef SNDCP_UPM_INCLUDED*/
      if (sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set &&
        ((sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis &
         (1 << nsapi)) > 0)) {
      snsm_activate_res->msid =
        sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1 + 1; 
      /*
       * How is that one negotiated?
       * Missing in VJ XID block.
       */
      snsm_activate_res->hcomp =
        sndcp_data->mg.cur_xid_block[sapi_index].vj.direction;
    } else {
      snsm_activate_res->hcomp = 0;
      snsm_activate_res->msid = 0;
    }
    PSEND(hCommSM, snsm_activate_res); 
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
}
  } /* mg_send_snsm_activate_res() */



/*
+------------------------------------------------------------------------------
| Function    : mg_send_xid_req
+------------------------------------------------------------------------------
| Description : This procedure sets the pending-states of the affected sapi
|               and nsapi to MG_XID_PENDING and sends an LL_XID_REQ with
|               parameters set according to the compression informations in
|               the given snsm_activate_ind and according to constants that
|               determine the capabilities of the data compression entity.
|               The part of this procedure that deals with the constant
|               compressor capabilities is implementation dependent.
|
| Parameters  : the SNSM_ACTIVATE_IND
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED 
GLOBAL void mg_send_xid_req (T_SN_ACTIVATE_REQ* snsm_activate_ind)
#else
GLOBAL void mg_send_xid_req (T_SNSM_ACTIVATE_IND* snsm_activate_ind)
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{
  UBYTE sapi_index = 0;
  TRACE_FUNCTION( "mg_send_xid_req" );
  sndcp_get_sapi_index(snsm_activate_ind->sapi, &sapi_index);
  {
    PALLOC_SDU(ll_xid_req, LL_XID_REQ, SNDCP_XID_BLOCK_BIT_LEN);
    /*
     * Set sapi in ll_xid_req.
     */
    ll_xid_req->sapi = snsm_activate_ind->sapi;
    /*
     * Write data from snsm_activate_ind to service variable req_xid_block.
     */

    mg_set_req_xid_block(snsm_activate_ind);
    /*
     * Fill the XID block. Implementation dependent.
     */
    mg_set_xid_params(ll_xid_req->sapi,
                      &ll_xid_req->sdu,
                      sndcp_data->mg.req_xid_block[sapi_index]);
    /*
     * Mark the affected sapi as xid pending
     */

    sndcp_set_sapi_state(snsm_activate_ind->sapi, MG_XID);

    /*
     * Trace xid block.
     */
#ifdef SNDCP_TRACE_ALL
    TRACE_EVENT("outcoming xid block:");
    sndcp_trace_sdu(&ll_xid_req->sdu);
#endif
    /*
     * Send the XID block to LLC.
     */

    sndcp_unset_sapi_state(ll_xid_req->sapi, MG_XID_NEC);
    sndcp_set_sapi_state(ll_xid_req->sapi, MG_XID);
    PSEND(hCommLLC, ll_xid_req);
  }
} /* mg_send_xid_req() */

/*
+------------------------------------------------------------------------------
| Function    : mg_send_xid_req_del
+------------------------------------------------------------------------------
| Description : If mg_get_sapi_pntt_rej() or mg_get_sapi_dntt_rej() indicate
|               that compressors must be removed, this function will
|               send an LL_XID_REQ which does this, or if an XID is pending,
|               the information about the rejected compressors will be stored
|               to new_xid_block and the MG_XID_NEC will be set.
|
| Parameters  : the SNSM_ACTIVATE_IND
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_send_xid_req_del (UBYTE sapi)
{
  UBYTE sapi_index = 0;
  UBYTE ntt = 0;
  T_XID_BLOCK mt_xid_block;
  /*
   * Only length of parameter type 0.
   */
  USHORT res_sdu_bit_len = 24;
  BOOL v42_rej = FALSE;
  BOOL vj_rej = FALSE;

  TRACE_FUNCTION( "mg_send_xid_req_del" );

  sndcp_reset_xid_block(&mt_xid_block);

  sndcp_get_sapi_index(sapi, &sapi_index);
  /*
   * How long will xid_block be?
   */
  /*
   * Add the extra space for ntts with nsapis == 0.
   */
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    BOOL rej = FALSE;
    mg_get_sapi_dntt_rej(sapi, ntt, &rej);
    if (rej) {
      /*
       * length of ntt octet and nsapis.
       */
      res_sdu_bit_len += 32;
      v42_rej = TRUE;
      /*
       * Change new_xid_block.
       */
      sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis = 0;
      sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis_set = TRUE;
      sndcp_data->mg.new_xid_block[sapi_index].v42.is_set = TRUE;
    }

    mg_get_sapi_pntt_rej(sapi, ntt, &rej);
    if (rej) {
      /*
       * length of ntt octet and nsapis.
       */
      res_sdu_bit_len += 32;
      vj_rej = TRUE;
      /*
       * Change req_xid_block.
       */
      sndcp_data->mg.new_xid_block[sapi_index].vj.nsapis = 0;
      sndcp_data->mg.new_xid_block[sapi_index].vj.nsapis_set = TRUE;
      sndcp_data->mg.new_xid_block[sapi_index].vj.is_set = TRUE;

    }
  }
  if (v42_rej) {
    /*
     * Add length of parameter type and length.
     */
    res_sdu_bit_len += 16;
  }
  if (vj_rej) {
    /*
     * Add length of parameter type and length.
     */
    res_sdu_bit_len += 16;
  }

  if (v42_rej || vj_rej) {

    USHORT state = MG_IDLE;

    sndcp_get_sapi_state(sapi, &state);
    if ((state & MG_XID) == 0) {

      PALLOC_SDU(ll_xid_req, LL_XID_REQ, res_sdu_bit_len);

      sndcp_data->mg.req_xid_block[sapi_index] =
        sndcp_data->mg.new_xid_block[sapi_index];
      /*
       * Set sapi in ll_xid_req.
       */
      ll_xid_req->sapi = sapi;
      /*
       * Write data from snsm_activate_ind to service variable req_xid_block.
       */
      sndcp_reset_xid_block(&mt_xid_block);
      sndcp_reset_xid_block(&sndcp_data->mg.new_xid_block[sapi_index]);
      /*
       * Fill the XID block. Implementation dependent.
       */
      mg_set_xid_params(ll_xid_req->sapi,
                        &ll_xid_req->sdu,
                        mt_xid_block);
      /*
       * Mark the affected sapi as MG_DEL_XID_PENDING.
       */
      sndcp_set_sapi_state(sapi, MG_XID);
      /*
       * Send the XID block to LLC.
       */

      sndcp_unset_sapi_state(sapi, MG_XID_NEC);
      sndcp_set_sapi_state(ll_xid_req->sapi, MG_XID);
      PSEND(hCommLLC, ll_xid_req);
    } else {
      sndcp_set_sapi_state(sapi, MG_XID_NEC);
    }
  }
} /* mg_send_xid_req_del() */


/*
+------------------------------------------------------------------------------
| Function    : mg_set_cur_xid_block
+------------------------------------------------------------------------------
| Description : This procedure sets the cur_xid_block service variable
| according to the values given in req_xid_block and cnf_xid_block.
| It also sets the unassigned, selected, assigned states of the affected
| dcomp, pcomp, ntt.
| The p bit in the req_xid_block is unset, if affected.
| See GSM 4.65, 6.8.2.
|
| Parameters  : the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_set_cur_xid_block (UBYTE sapi) {
  UBYTE sapi_index = 0;
  T_XID_BLOCK* cur_xid_block;
  T_XID_BLOCK* req_xid_block;
  T_XID_BLOCK* cnf_xid_block;
  TRACE_FUNCTION( "mg_set_cur_xid_block" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  cur_xid_block = &sndcp_data->mg.cur_xid_block[sapi_index];
  req_xid_block = &sndcp_data->mg.req_xid_block[sapi_index];
  cnf_xid_block = &sndcp_data->mg.cnf_xid_block[sapi_index];
  /*
   * SNDCP version has been tested to be SNDCP_XID_VERSION.
   */
  cur_xid_block->version = SNDCP_XID_VERSION;

  /*
   * V42bis parameters.
   */
  if (req_xid_block->v42.is_set) {
    cur_xid_block->v42.is_set = TRUE;
    cur_xid_block->v42.ntt = req_xid_block->v42.ntt;
    mg_set_sapi_dntt_state(sapi,
                           req_xid_block->v42.ntt,
                           MG_ASSIGNED);
    req_xid_block->v42.p_bit = SNDCP_P_BIT_0;

    /*
     * Algorithm type and DCOMP are only set in req.
     */
    cur_xid_block->v42.algo_type = req_xid_block->v42.algo_type;
    cur_xid_block->v42.dcomp = req_xid_block->v42.dcomp;
    mg_set_sapi_dcomp_state(sapi,
                            req_xid_block->v42.dcomp,
                            MG_ASSIGNED);

    if (cnf_xid_block->v42.is_set) {

      if (cnf_xid_block->v42.nsapis_set) {
        cur_xid_block->v42.nsapis = cnf_xid_block->v42.nsapis;
        cur_xid_block->v42.nsapis_set = TRUE;
      } else if (req_xid_block->v42.nsapis_set) {
        cur_xid_block->v42.nsapis = req_xid_block->v42.nsapis;
        cur_xid_block->v42.nsapis_set = TRUE;
      }
      if (cnf_xid_block->v42.p0_set) {
        cur_xid_block->v42.p0 = cnf_xid_block->v42.p0;
        cur_xid_block->v42.p0_set = TRUE;
      } else if (req_xid_block->v42.p0_set) {
        cur_xid_block->v42.p0 = req_xid_block->v42.p0;
        cur_xid_block->v42.p0_set = TRUE;
      }
      if (cnf_xid_block->v42.p1_set) {
        cur_xid_block->v42.p1 = cnf_xid_block->v42.p1;
        cur_xid_block->v42.p1_set = TRUE;
      } else if (req_xid_block->v42.p1_set) {
        cur_xid_block->v42.p1 = req_xid_block->v42.p1;
        cur_xid_block->v42.p1_set = TRUE;
      }
      if (cnf_xid_block->v42.p2_set) {
        cur_xid_block->v42.p2 = cnf_xid_block->v42.p2;
        cur_xid_block->v42.p2_set = TRUE;
      } else if (req_xid_block->v42.p2_set) {
        cur_xid_block->v42.p2 = req_xid_block->v42.p2;
        cur_xid_block->v42.p2_set = TRUE;
      }

    } else {
      /*
       * NOT cnf_xid_block->v42.is_set.
       */

      if (req_xid_block->v42.nsapis_set) {
        cur_xid_block->v42.nsapis = req_xid_block->v42.nsapis;
        cur_xid_block->v42.nsapis_set = TRUE;
      }
      if (req_xid_block->v42.p0_set) {
        cur_xid_block->v42.p0 = req_xid_block->v42.p0;
        cur_xid_block->v42.p0_set = TRUE;
      }
      if (req_xid_block->v42.p1_set) {
        cur_xid_block->v42.p1 = req_xid_block->v42.p1;
        cur_xid_block->v42.p1_set = TRUE;
      }
      if (req_xid_block->v42.p2_set) {
        cur_xid_block->v42.p2 = req_xid_block->v42.p2;
        cur_xid_block->v42.p2_set = TRUE;
      }

    }
  } else {
    /*
     * NOT req_xid_block->v42.is_set.
     */
    if (cnf_xid_block->v42.is_set) {

      cur_xid_block->v42.is_set = TRUE;
      if (cnf_xid_block->v42.nsapis_set) {
        cur_xid_block->v42.nsapis = cnf_xid_block->v42.nsapis;
        cur_xid_block->v42.nsapis_set = TRUE;
      }
      if (cnf_xid_block->v42.p0_set) {
        cur_xid_block->v42.p0 = cnf_xid_block->v42.p0;
        cur_xid_block->v42.p0_set = TRUE;
      }
      if (cnf_xid_block->v42.p1_set) {
        cur_xid_block->v42.p1 = cnf_xid_block->v42.p1;
        cur_xid_block->v42.p1_set = TRUE;
      }
      if (cnf_xid_block->v42.p2_set) {
        cur_xid_block->v42.p2 = cnf_xid_block->v42.p2;
        cur_xid_block->v42.p2_set = TRUE;
      }

    } else {
      /*
       * Req and cnf are not set, cur_xid_block keeps its values.
       */
    }
  }


  /*
   * VJ parameters.
   */
  if (req_xid_block->vj.is_set) {
    cur_xid_block->vj.is_set = TRUE;
    cur_xid_block->vj.ntt = req_xid_block->vj.ntt;
    mg_set_sapi_pntt_state(sapi,
                           req_xid_block->vj.ntt,
                           MG_ASSIGNED);
    req_xid_block->vj.p_bit = SNDCP_P_BIT_0;
    /*
     * Algorithm type and PCOMPs are only set in req.
     * The direction is only set in req.
     */
    cur_xid_block->vj.algo_type = req_xid_block->vj.algo_type;
    cur_xid_block->vj.pcomp1 = req_xid_block->vj.pcomp1;
    mg_set_sapi_pcomp_state(sapi,
                            req_xid_block->vj.pcomp1,
                            MG_ASSIGNED);
    cur_xid_block->vj.pcomp2 = req_xid_block->vj.pcomp2;
    mg_set_sapi_pcomp_state(sapi,
                            req_xid_block->vj.pcomp2,
                            MG_ASSIGNED);
    cur_xid_block->vj.direction = req_xid_block->vj.direction;
    if (cnf_xid_block->vj.is_set) {
      if (cnf_xid_block->vj.nsapis_set) {
        cur_xid_block->vj.nsapis = cnf_xid_block->vj.nsapis;
        cur_xid_block->vj.nsapis_set = TRUE;
      } else {
        cur_xid_block->vj.nsapis = req_xid_block->vj.nsapis;
        cur_xid_block->vj.nsapis_set = TRUE;
      }
      if (cnf_xid_block->vj.s0_m_1_set) {
        cur_xid_block->vj.s0_m_1 = cnf_xid_block->vj.s0_m_1;
        cur_xid_block->vj.s0_m_1_set = TRUE;
      } else {
        cur_xid_block->vj.s0_m_1 = req_xid_block->vj.s0_m_1;
        cur_xid_block->vj.s0_m_1_set = TRUE;
      }
    } else {
      if (req_xid_block->vj.nsapis_set) {
        cur_xid_block->vj.nsapis = req_xid_block->vj.nsapis;
        cur_xid_block->vj.nsapis_set = TRUE;
      }
      if (req_xid_block->vj.s0_m_1_set) {
        cur_xid_block->vj.s0_m_1 = req_xid_block->vj.s0_m_1;
        cur_xid_block->vj.s0_m_1_set = TRUE;
      }
    }
  } else {
    /*
     * NOT req_xid_block->vj.es_set.
     */
    if (cnf_xid_block->vj.is_set) {
      cur_xid_block->vj.is_set = TRUE;
      if (cnf_xid_block->vj.nsapis_set) {
        cur_xid_block->vj.nsapis = cnf_xid_block->vj.nsapis;
        cur_xid_block->vj.nsapis_set = TRUE;
      }
      if (cnf_xid_block->vj.s0_m_1_set) {
        cur_xid_block->vj.s0_m_1 = cnf_xid_block->vj.s0_m_1;
        cur_xid_block->vj.s0_m_1_set = TRUE;
      }
    } else {
      /*
       * Req and cnf are not set, cur_xid_block keeps it's values.
       */
    }
  }

  /*
   * If nsapis are 0, deactivate compressor.
   */
  if (cur_xid_block->v42.nsapis == 0 ||
      cur_xid_block->v42.nsapis_set == FALSE) {

    /*
     * Find the affected compressor entity.
     */
    UBYTE dntt = 0;
    mg_get_sapi_dcomp_dntt(sapi, req_xid_block->v42.dcomp, &dntt);

    cur_xid_block->v42.is_set = FALSE;
    mg_set_sapi_dcomp_state(sapi,
                            req_xid_block->v42.dcomp,
                            MG_UNASSIGNED);
    mg_set_sapi_dntt_state(sapi,
                           dntt,
                           MG_UNASSIGNED);
    /*
     * One compressor less, something like sndcp_data->v42_count--;
     * should come here!
     */

  }
  if (cur_xid_block->vj.nsapis == 0 ||
      cur_xid_block->vj.nsapis_set == FALSE) {

    /*
     * Find the affected compressor entity.
     */
    UBYTE pntt = 0;
    mg_get_sapi_pcomp_pntt(sapi, req_xid_block->vj.pcomp1, &pntt);
    mg_get_sapi_pcomp_pntt(sapi, req_xid_block->vj.pcomp2, &pntt);
    cur_xid_block->vj.is_set = FALSE;
    mg_set_sapi_pcomp_state(sapi,
                            req_xid_block->vj.pcomp1,
                            MG_UNASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            req_xid_block->vj.pcomp2,
                            MG_UNASSIGNED);
    mg_set_sapi_pntt_state(sapi,
                           pntt,
                           MG_UNASSIGNED);
    /*
     * One compressor less.
     */
    if (sndcp_data->vj_count > 0) {
      sndcp_data->vj_count--;
    }
  }


  /*
   * Send new block to service cia.
   */
  sig_mg_cia_new_xid(cur_xid_block);

} /* mg_set_cur_xid_block() */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_new_xid_block
+------------------------------------------------------------------------------
| Description : This procedure reads data from the given snsm_activate_ind and
|               from the already requested xid block and
|               writes them to the service variable new_xid_block.
|               When the pending establishment or xid negotiation is finished
|               this new_xid_block will be newly evaluated.
|
| Parameters  :
|               the new SNSM_ACTIVATE_IND
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_set_new_xid_block (T_SN_ACTIVATE_REQ* snsm_activate_ind)
#else
GLOBAL void mg_set_new_xid_block (T_SNSM_ACTIVATE_IND* snsm_activate_ind)
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{
  UBYTE sapi_index = 0;
  TRACE_FUNCTION( "mg_set_new_xid_block" );
  sndcp_get_sapi_index(snsm_activate_ind->sapi, &sapi_index);
  /*
   * Set the version number.
   */
  sndcp_data->mg.new_xid_block[sapi_index].version = SNDCP_XID_VERSION;
  /*
   * Set the V42.bis parameters,
   */
#ifdef TI_PS_FF_V42BIS
  sndcp_data->mg.new_xid_block[sapi_index].v42.is_set =
#ifdef SNDCP_UPM_INCLUDED
    (snsm_activate_ind->comp_params.dcomp != NAS_DCOMP_NEITHER_DIRECT);
#else
    (snsm_activate_ind->dcomp != SNSM_COMP_NEITHER_DIRECT);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  sndcp_data->mg.new_xid_block[sapi_index].v42.p_bit = SNDCP_P_BIT_1;
  sndcp_data->mg.new_xid_block[sapi_index].v42.ntt = SNDCP_NTT_0;
  sndcp_data->mg.new_xid_block[sapi_index].v42.algo_type = SNDCP_XID_V42;
  sndcp_data->mg.new_xid_block[sapi_index].v42.dcomp = SNDCP_DCOMP1;
  sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis =
    1 << (snsm_activate_ind->nsapi);
  sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis_set = TRUE;
#ifdef SNDCP_UPM_INCLUDED
  sndcp_data->mg.new_xid_block[sapi_index].v42.p0 = snsm_activate_ind->comp_params.dcomp;
#else
  sndcp_data->mg.new_xid_block[sapi_index].v42.p0 = snsm_activate_ind->dcomp;
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  sndcp_data->mg.new_xid_block[sapi_index].v42.p0_set = TRUE;

  /*
   * Set p1 and p2 to default values.
   */
  sndcp_data->mg.new_xid_block[sapi_index].v42.p1 = SNDCP_V42_DEFAULT_P1;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p1_set = TRUE;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p2 =
    SNDCP_V42_DEFAULT_P2;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p2_set = TRUE;

  /*
   * Set affected entities and dcomp/pcomp values.
   */
  mg_set_ntt_comp(snsm_activate_ind->sapi);

#else /* !TI_PS_FF_V42BIS */


  sndcp_data->mg.new_xid_block[sapi_index].v42.is_set = FALSE;

  sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis = 0;

  sndcp_data->mg.new_xid_block[sapi_index].v42.nsapis_set = FALSE;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p0_set = FALSE;

  /*
   * Set p1 and p2 to default values.
   */
  sndcp_data->mg.new_xid_block[sapi_index].v42.p1 = SNDCP_V42_DEFAULT_P1;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p1_set = TRUE;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p2 =
    SNDCP_V42_DEFAULT_P2;
  sndcp_data->mg.new_xid_block[sapi_index].v42.p2_set = TRUE;

#endif /* TI_PS_FF_V42BIS */

  /*
   * Set the Van Jacobson parameters,
   */
  sndcp_data->mg.new_xid_block[sapi_index].vj.is_set =
#ifdef SNDCP_UPM_INCLUDED 
    (snsm_activate_ind->comp_params.hcomp != NAS_HCOMP_OFF) ||
#else
    (snsm_activate_ind->hcomp != SNSM_COMP_NEITHER_DIRECT) ||
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.new_xid_block[sapi_index].vj.is_set ||
    sndcp_data->mg.req_xid_block[sapi_index].vj.is_set;

  if (! sndcp_data->mg.req_xid_block[sapi_index].vj.is_set) {
    sndcp_data->mg.new_xid_block[sapi_index].vj.p_bit = SNDCP_P_BIT_1;
  } else {
    sndcp_data->mg.new_xid_block[sapi_index].vj.p_bit = SNDCP_P_BIT_0;
  }

  sndcp_data->mg.new_xid_block[sapi_index].vj.ntt = SNDCP_NTT_0;
  sndcp_data->mg.new_xid_block[sapi_index].vj.algo_type = SNDCP_XID_VJ;
  sndcp_data->mg.new_xid_block[sapi_index].vj.pcomp1 = SNDCP_PCOMP1;
  sndcp_data->mg.new_xid_block[sapi_index].vj.pcomp2 = SNDCP_PCOMP2;

  sndcp_data->mg.new_xid_block[sapi_index].vj.nsapis |=
    (sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis |
     1 << (snsm_activate_ind->nsapi));
  sndcp_data->mg.new_xid_block[sapi_index].vj.nsapis_set = TRUE;
  
#ifdef SNDCP_UPM_INCLUDED
  sndcp_data->mg.new_xid_block[sapi_index].vj.s0_m_1 = 
    snsm_activate_ind->comp_params.msid - 1;
#else
  sndcp_data->mg.new_xid_block[sapi_index].vj.s0_m_1 =
    snsm_activate_ind->msid - 1;
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  sndcp_data->mg.new_xid_block[sapi_index].vj.s0_m_1_set = TRUE;
  /*
   * Only used internally, not in XID block.
   * Not: it is assumed that the indicated values match the existing ones!
   */
#ifdef SNDCP_UPM_INCLUDED
  sndcp_data->mg.new_xid_block[sapi_index].vj.direction = 
    snsm_activate_ind->comp_params.hcomp;
#else
  sndcp_data->mg.new_xid_block[sapi_index].vj.direction = 
    snsm_activate_ind->hcomp;
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

} /* mg_set_new_xid_block() */



/*
+------------------------------------------------------------------------------
| Function    : mg_set_req_xid_block
+------------------------------------------------------------------------------
| Description : This procedure reads data from the given snsm_activate_ind and
|               writes it to the service variable req_xid_block.
|
| Parameters  :
|               the SNSM_ACTIVATE_IND that caused the negotiation.
|
+------------------------------------------------------------------------------
*/
#ifdef SNDCP_UPM_INCLUDED
GLOBAL void mg_set_req_xid_block (T_SN_ACTIVATE_REQ* snsm_activate_ind)
#else
GLOBAL void mg_set_req_xid_block (T_SNSM_ACTIVATE_IND* snsm_activate_ind)
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
{
  UBYTE sapi_index = 0;
  BOOL vio;
  TRACE_FUNCTION( "mg_set_req_xid_block" );
  sndcp_get_sapi_index(snsm_activate_ind->sapi, &sapi_index);
  /*
   * Set the version number.
   */
  sndcp_data->mg.req_xid_block[sapi_index].version = SNDCP_XID_VERSION;
  /*
   * Set the V42.bis parameters,
   */
#ifdef TI_PS_FF_V42BIS

#ifdef SNDCP_UPM_INCLUDED
    sndcp_data->mg.req_xid_block[sapi_index].v42.is_set =
      (snsm_activate_ind->comp_params.dcomp != NAS_DCOMP_NEITHER_DIRECT);
#else
    sndcp_data->mg.req_xid_block[sapi_index].v42.is_set = 
      (snsm_activate_ind->dcomp != SNSM_COMP_NEITHER_DIRECT);
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.req_xid_block[sapi_index].v42.p_bit = SNDCP_P_BIT_1;
    sndcp_data->mg.req_xid_block[sapi_index].v42.ntt = SNDCP_NTT_0;
    sndcp_data->mg.req_xid_block[sapi_index].v42.algo_type = SNDCP_XID_V42;
    sndcp_data->mg.req_xid_block[sapi_index].v42.dcomp = SNDCP_DCOMP1;

    sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis =
      1 << (snsm_activate_ind->nsapi);
    sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis_set = TRUE;
#ifdef SNDCP_UPM_INCLUDED
    sndcp_data->mg.req_xid_block[sapi_index].v42.p0 = snsm_activate_ind->comp_params.dcomp;
#else
    sndcp_data->mg.req_xid_block[sapi_index].v42.p0 = snsm_activate_ind->dcomp;
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.req_xid_block[sapi_index].v42.p0_set = TRUE;

    /*
     * Set p1 and p2 to default values.
     */
    sndcp_data->mg.req_xid_block[sapi_index].v42.p1 = SNDCP_V42_DEFAULT_P1;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p1_set = TRUE;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p2 =
      SNDCP_V42_DEFAULT_P2;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p2_set = TRUE;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.is_set =
  #ifdef SNDCP_UPM_INCLUDED
      (snsm_activate_ind->comp_params.dcomp != NAS_DCOMP_NEITHER_DIRECT);
  #else
      (snsm_activate_ind->dcomp != SNSM_COMP_NEITHER_DIRECT);
  #endif /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p_bit = SNDCP_P_BIT_1;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.ntt = SNDCP_NTT_0;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.algo_type = SNDCP_XID_V42;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.dcomp = SNDCP_DCOMP1;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.nsapis =
      1 << (snsm_activate_ind->nsapi);
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.nsapis_set = TRUE;
  #ifdef SNDCP_UPM_INCLUDED
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p0 = snsm_activate_ind->comp_params.dcomp;
  #else
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p0 = snsm_activate_ind->dcomp;
  #endif /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p0_set = TRUE;

    /*
     * Set p1 and p2 to default values.
     */
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p1 = SNDCP_V42_DEFAULT_P1;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p1_set = TRUE;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p2 =
      SNDCP_V42_DEFAULT_P2;
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].v42.p2_set = TRUE;

    /*
     * Set affected entities and dcomp/pcomp values.
     */
    mg_set_ntt_comp(snsm_activate_ind->sapi);


#else /* !TI_PS_FF_V42BIS */

    sndcp_data->mg.req_xid_block[sapi_index].v42.is_set = FALSE;

    sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis = 0;

    sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis_set = FALSE;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p0_set = FALSE;

    /*
     * Set p1 and p2 to default values.
     */
    sndcp_data->mg.req_xid_block[sapi_index].v42.p1 = SNDCP_V42_DEFAULT_P1;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p1_set = TRUE;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p2 =
      SNDCP_V42_DEFAULT_P2;
    sndcp_data->mg.req_xid_block[sapi_index].v42.p2_set = TRUE;

#endif /* TI_PS_FF_V42BIS */

  /*
   * Set the Van Jacobson parameters.
   * Note:
   * If number of state slots is set to 0, do not request VJ in XID request.
   */
#ifdef SNDCP_UPM_INCLUDED
  if (snsm_activate_ind->comp_params.msid == 0) {
#else
  if (snsm_activate_ind->msid == 0) {
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
    sndcp_data->mg.req_xid_block[sapi_index].vj.is_set = FALSE;
    return;
  }

  sndcp_data->mg.req_xid_block[sapi_index].vj.is_set =
  #ifdef SNDCP_UPM_INCLUDED 
    (snsm_activate_ind->comp_params.hcomp != NAS_HCOMP_OFF);
  #else
    (snsm_activate_ind->hcomp != SNSM_COMP_NEITHER_DIRECT);
  #endif  /*#ifdef SNDCP_UPM_INCLUDED*/
  if (!sndcp_data->mg.req_xid_block[sapi_index].vj.is_set) {
    return;
  }
  /*
   * This function will only be called after snsm_activate_ind. So the
   * user_xid_block will be set for the affected nsapi.
   */
  if (sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) {
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.p_bit = 
      SNDCP_P_BIT_0;
  } else {
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.p_bit = 
      SNDCP_P_BIT_1;
  }
    
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.ntt = 
    SNDCP_NTT_0;
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.algo_type = 
    SNDCP_XID_VJ;
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.pcomp1 = 
    SNDCP_PCOMP1;
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.pcomp2 = 
    SNDCP_PCOMP2;
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.nsapis +=
    1 << (snsm_activate_ind->nsapi);
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.nsapis_set = TRUE;
#ifdef SNDCP_UPM_INCLUDED
  if (snsm_activate_ind->comp_params.msid > SNDCP_MAX_NUMBER_OF_VJ_SLOTS) { 
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.s0_m_1 = 
      SNDCP_MAX_NUMBER_OF_VJ_SLOTS - 1;
  } else {
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.s0_m_1 = 
      snsm_activate_ind->comp_params.msid - 1;
  }
#else
  if (snsm_activate_ind->msid > SNDCP_MAX_NUMBER_OF_VJ_SLOTS) {
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.s0_m_1 =
      SNDCP_MAX_NUMBER_OF_VJ_SLOTS - 1;
  } else {
    sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.s0_m_1 =
      snsm_activate_ind->msid - 1;
  }
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.s0_m_1_set = TRUE;
  /*
   * Only used internally, not in XID block.
   */
#ifdef SNDCP_UPM_INCLUDED
  sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.direction = 
    snsm_activate_ind->comp_params.hcomp;
#else
   sndcp_data->mg.user_xid_block[snsm_activate_ind->nsapi].vj.direction = 
   snsm_activate_ind->hcomp;
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

  /*
   * Set affected entities and dcomp/pcomp values.
   */
  mg_set_ntt_comp(snsm_activate_ind->sapi);

  mg_detect_mode_clash ((USHORT)
                         (sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis +
                         (1 << (snsm_activate_ind->nsapi))),
                        &vio);
  if (vio) {
    return;
  }

  if (sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) {
    sndcp_data->mg.req_xid_block[sapi_index].vj.p_bit = SNDCP_P_BIT_0;
  } else {
    sndcp_data->mg.req_xid_block[sapi_index].vj.p_bit = SNDCP_P_BIT_1;
  }
  sndcp_data->mg.req_xid_block[sapi_index].vj.ntt = SNDCP_NTT_0;
  sndcp_data->mg.req_xid_block[sapi_index].vj.algo_type = SNDCP_XID_VJ;
  sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp1 = SNDCP_PCOMP1;
  sndcp_data->mg.req_xid_block[sapi_index].vj.pcomp2 = SNDCP_PCOMP2;

  /*
   * All the nsapis that currently use the compressor and the new one
   * shall be set in the nsapis field of the XID block.
   */
  sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis = 
    (sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis | 
     (1 << (snsm_activate_ind->nsapi)));

  sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis_set = TRUE;
#ifdef SNDCP_UPM_INCLUDED
  if (snsm_activate_ind->comp_params.msid > SNDCP_MAX_NUMBER_OF_VJ_SLOTS) { 
    sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1 = 
      SNDCP_MAX_NUMBER_OF_VJ_SLOTS - 1;
  } else {
    sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1 = 
      snsm_activate_ind->comp_params.msid - 1;
  }
#else
  if (snsm_activate_ind->msid > SNDCP_MAX_NUMBER_OF_VJ_SLOTS) {
    sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1 =
      SNDCP_MAX_NUMBER_OF_VJ_SLOTS - 1;
  } else {
    sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1 =
      snsm_activate_ind->msid - 1;
  }
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  sndcp_data->mg.req_xid_block[sapi_index].vj.s0_m_1_set = TRUE;
  /*
   * Only used internally, not in XID block.
   */
#ifdef SNDCP_UPM_INCLUDED
  sndcp_data->mg.req_xid_block[sapi_index].vj.direction = 
    snsm_activate_ind->comp_params.hcomp;
#else
  sndcp_data->mg.req_xid_block[sapi_index].vj.direction = 
    snsm_activate_ind->hcomp;
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  /*
   * Set affected entities and dcomp/pcomp values.
   */
  mg_set_ntt_comp(snsm_activate_ind->sapi);

} /* mg_set_req_xid_block() */


/*
+------------------------------------------------------------------------------
| Function    : mg_set_res_cur_xid_block
+------------------------------------------------------------------------------
| Description : This procedure sets the service variables res_xid_block and
| cur_xid_block. The needed data comes from the service variable ind_xid_block,
| and from default settings.
| Note: if the indicated compressors do not match with compressors requested
| or currently used then they will be unset by adding them to the list of
| rejected compressors.
|
|
| Parameters  :
|               the affected sapi,
|               the necessary bit length of an sdu that will later store the
|               xid block derived from res_xid_block.
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_set_res_cur_xid_block (UBYTE sapi, USHORT* res_sdu_bit_len)
{
  UBYTE sapi_index = 0;
  UBYTE nsapi = 0;
  /*
   * Which compressors are indicated?
   */
  BOOL v42_ind, vj_ind;
  /*
   * Which compressors are possible?
   * (The indicated set of nsapis must overlap with the
   * currently used one.)
   */
  BOOL v42_possible, vj_possible;
  /*
   * Is compressor going to rejected? In this case the compressor should not
   * be requested.
   */
  BOOL v42_rejected, vj_rejected;

  TRACE_FUNCTION( "mg_set_res_cur_xid_block" );

  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_reset_xid_block(&(sndcp_data->mg.res_xid_block[sapi_index]));

  /*
   * Set bit length to "only version", 3 bytes.
   */
  *res_sdu_bit_len = 24;
  /*
   * Set the version number.
   */
  sndcp_data->mg.res_xid_block[sapi_index].version = SNDCP_XID_VERSION;

  v42_ind = sndcp_data->mg.ind_xid_block[sapi_index].v42.is_set;
  v42_possible = FALSE;
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    if ((sndcp_data->mg.user_xid_block[nsapi].v42.nsapis & 
        sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis) > 0) {

      U8 local_sapi = 0;
      sndcp_get_nsapi_sapi(nsapi, &local_sapi);
      if (local_sapi == sapi) {
        v42_possible = TRUE;
      }
    }
  }


  if (v42_ind && ! v42_possible) {
    sndcp_data->mg.res_xid_block[sapi_index].v42.is_set = FALSE;
    mg_set_sapi_dntt_rej (sapi,
                          sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                          TRUE);
  }

  mg_get_sapi_dntt_rej (sapi,
                        sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt,
                        &v42_rejected);


  if (v42_ind && ! v42_rejected) {
    /*
     * Data compression will be included, increment bit len (+ 10 bytes).
     */
    *res_sdu_bit_len += 80;

    /*
     * Set the V42.bis parameters,
     */
    sndcp_data->mg.res_xid_block[sapi_index].v42.is_set = TRUE;

    sndcp_data->mg.res_xid_block[sapi_index].v42.p_bit = SNDCP_P_BIT_0;

    sndcp_data->mg.res_xid_block[sapi_index].v42.ntt =
      sndcp_data->mg.ind_xid_block[sapi_index].v42.ntt;


    sndcp_data->mg.res_xid_block[sapi_index].v42.algo_type = SNDCP_XID_V42;

    sndcp_data->mg.res_xid_block[sapi_index].v42.dcomp =
      sndcp_data->mg.ind_xid_block[sapi_index].v42.dcomp;

    /*
     * Set the 'nsapis' parameter in res_xid_block:
     * Check rules in 4.65, subclause 6.8.1, compare with cur_xid_block.
     */
    mg_set_xid_nsapis(sapi, MG_XID_V42_NSAPIS);

    /*
     * It is assumed that the values desired by the user / the
     * values possible with the current implementation are written in
     * req_xid_block, the responded values are then the minimum of
     * req and ind values:
     */
    /*
     * For the direction parameter p0 take the logical AND:
     */
    sndcp_data->mg.res_xid_block[sapi_index].v42.p0 =
      (sndcp_data->mg.req_xid_block[sapi_index].v42.p0 &
       sndcp_data->mg.ind_xid_block[sapi_index].v42.p0);
    sndcp_data->mg.res_xid_block[sapi_index].v42.p0_set = TRUE;

    /*
     * For p1 and p2 take minimum.
     */
    sndcp_data->mg.res_xid_block[sapi_index].v42.p1 =
      (sndcp_data->mg.req_xid_block[sapi_index].v42.p1 <
       sndcp_data->mg.ind_xid_block[sapi_index].v42.p1)?
       sndcp_data->mg.req_xid_block[sapi_index].v42.p1 :
        sndcp_data->mg.ind_xid_block[sapi_index].v42.p1;
    sndcp_data->mg.res_xid_block[sapi_index].v42.p1_set = TRUE;

    sndcp_data->mg.res_xid_block[sapi_index].v42.p2 =
      (sndcp_data->mg.req_xid_block[sapi_index].v42.p2 <
       sndcp_data->mg.ind_xid_block[sapi_index].v42.p2)?
       sndcp_data->mg.req_xid_block[sapi_index].v42.p2 :
        sndcp_data->mg.ind_xid_block[sapi_index].v42.p2;
    sndcp_data->mg.res_xid_block[sapi_index].v42.p2_set = TRUE;

  } /* v42.is_set */


  /*
   * Header compression,
   */

  
  vj_ind = sndcp_data->mg.ind_xid_block[sapi_index].vj.is_set;
  vj_possible = FALSE;
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    if ((sndcp_data->mg.user_xid_block[nsapi].vj.nsapis & 
        sndcp_data->mg.ind_xid_block[sapi_index].vj.nsapis) > 0) {

      U8 local_sapi = 0;
      sndcp_get_nsapi_sapi(nsapi, &local_sapi);
      if (local_sapi == sapi) {
        vj_possible = TRUE;
      }
    }
  }

  if (vj_ind && ! vj_possible) {
    sndcp_data->mg.res_xid_block[sapi_index].vj.is_set = FALSE;
    mg_set_sapi_pntt_rej (sapi,
                          sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                          TRUE);
  }

  mg_get_sapi_pntt_rej (sapi,
                        sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt,
                        &vj_rejected);

  if (vj_ind && ! vj_rejected) {
    /*
     * Header compression will be included, increment bit len (+ 7 bytes).
     */
    *res_sdu_bit_len += 56;

    /*
     * Set the VJ parameters,
     */
    sndcp_data->mg.res_xid_block[sapi_index].vj.is_set = TRUE;

    sndcp_data->mg.res_xid_block[sapi_index].vj.p_bit = SNDCP_P_BIT_0;

    sndcp_data->mg.res_xid_block[sapi_index].vj.ntt =
      sndcp_data->mg.ind_xid_block[sapi_index].vj.ntt;

    sndcp_data->mg.res_xid_block[sapi_index].vj.algo_type = SNDCP_XID_VJ;

    if (sndcp_data->mg.ind_xid_block[sapi_index].vj.p_bit == SNDCP_P_BIT_1) {
      sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1 =
        sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp1;
      sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2 =
        sndcp_data->mg.ind_xid_block[sapi_index].vj.pcomp2;
    } else {
      sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1 =
        sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp1;
      sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2 =
        sndcp_data->mg.cur_xid_block[sapi_index].vj.pcomp2;
    }

    /*
     * Set the 'nsapis' parameter in res_xid_block:
     * Check rules in 4.65, subclause 6.8.1, compare with cur_xid_block.
     * State slots and direction are also set here.
     */
    mg_set_xid_nsapis(sapi, MG_XID_VJ_NSAPIS);


  } /* vj.is_set */


  /*
   * Set affected entities and dcomp/pcomp values.
   */
  if (v42_ind && v42_possible) {
    mg_set_sapi_dntt_state(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].v42.ntt,
                           MG_ASSIGNED);

    mg_set_sapi_dcomp_state(sapi,
                            sndcp_data->mg.res_xid_block[sapi_index].v42.dcomp,
                            MG_ASSIGNED);
    mg_set_sapi_dcomp_dntt(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].v42.dcomp,
                           sndcp_data->mg.res_xid_block[sapi_index].v42.ntt);
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis) > 0) {
        mg_set_sapi_dntt_nsapi(sapi,
                              sndcp_data->mg.res_xid_block[sapi_index].v42.ntt,
                               nsapi,
                               TRUE);
      } /* if nsapi is selected */
    } /* for loop over all nsapis */

  }
  /*
   * If VJ is indicated and possible, switch on compressor.
   */
  if (vj_ind && vj_possible) {
    mg_set_sapi_pntt_state(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt,
                           MG_ASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1,
                            MG_ASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2,
                            MG_ASSIGNED);
    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt);
    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt);
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis) > 0) {
        mg_set_sapi_pntt_nsapi(sapi,
                             sndcp_data->mg.res_xid_block[sapi_index].vj.ntt,
                             nsapi,
                             TRUE);
      } /* if nsapi is selected */
    } /* for loop over all nsapis */
  }

  /*
   * If VJ has been switched on and is now indicated to be switched off:
   * switch off compressor.
   */
  if (vj_ind
      &&
      sndcp_data->mg.ind_xid_block[sapi_index].vj.nsapis == 0
      &&
      sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis > 0)
  {
    mg_set_sapi_pntt_state(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt,
                           MG_UNASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1,
                            MG_UNASSIGNED);
    mg_set_sapi_pcomp_state(sapi,
                            sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2,
                            MG_UNASSIGNED);
/*    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp1,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt);
    mg_set_sapi_pcomp_pntt(sapi,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.pcomp2,
                           sndcp_data->mg.res_xid_block[sapi_index].vj.ntt);
*/
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      if (((1 << nsapi) & sndcp_data->mg.ind_xid_block[sapi_index].v42.nsapis) > 0) {
        mg_set_sapi_pntt_nsapi(sapi,
                             sndcp_data->mg.cur_xid_block[sapi_index].vj.ntt,
                             nsapi,
                             FALSE);
      } /* if nsapi is selected */
    } /* for loop over all nsapis */
  }


  /*
   * Add same values for cur_xid_block.
   */

  if (sndcp_data->mg.res_xid_block[sapi_index].v42.is_set) {
    if (sndcp_data->mg.cur_xid_block[sapi_index].v42.is_set) {
      sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis =
        sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis;
      if (sndcp_data->mg.cur_xid_block[sapi_index].v42.p0_set) {
        sndcp_data->mg.cur_xid_block[sapi_index].v42.p0 =
          sndcp_data->mg.res_xid_block[sapi_index].v42.p0;
      }
      if (sndcp_data->mg.cur_xid_block[sapi_index].v42.p1_set) {
        sndcp_data->mg.cur_xid_block[sapi_index].v42.p1 =
          sndcp_data->mg.res_xid_block[sapi_index].v42.p1;
      }
      if (sndcp_data->mg.cur_xid_block[sapi_index].v42.p2_set) {
        sndcp_data->mg.cur_xid_block[sapi_index].v42.p2 =
          sndcp_data->mg.res_xid_block[sapi_index].v42.p2;
      }
    } else {
      sndcp_data->mg.cur_xid_block[sapi_index].v42 =
        sndcp_data->mg.res_xid_block[sapi_index].v42;
    }
  }

  if (sndcp_data->mg.res_xid_block[sapi_index].vj.is_set) {
    if (sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set) {
      sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis =
        sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis;
      if (sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1_set) {
        sndcp_data->mg.cur_xid_block[sapi_index].vj.s0_m_1 =
          sndcp_data->mg.res_xid_block[sapi_index].vj.s0_m_1;
      }
    } else {
      sndcp_data->mg.cur_xid_block[sapi_index].vj =
        sndcp_data->mg.res_xid_block[sapi_index].vj;
    }
  }


  if (vj_rejected) {
    sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis = 0;
  }
  if (sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis == 0) {
    sndcp_data->mg.cur_xid_block[sapi_index].vj.is_set = FALSE;
  }

  if (v42_rejected) {
    sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis = 0;
  }
  if (sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis == 0) {
    sndcp_data->mg.cur_xid_block[sapi_index].v42.is_set = FALSE;
  }

  sig_mg_cia_new_xid(& sndcp_data->mg.cur_xid_block[sapi_index]);


} /* mg_set_res_cur_xid_block */

/*
+------------------------------------------------------------------------------
| Function    : mg_set_res_xid_params
+------------------------------------------------------------------------------
| Description : This procedure fills the XID block. It is implementation
|               dependent. In the current version V42bis and VanJacobson
|               header compression are applied.
|
| Parameters  :
|               the sdu that will be filled,
| Post        : Reset arrays with reject information to all FALSE.
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_set_res_xid_params (T_sdu* sdu, UBYTE sapi)
{
  /*
   * Byte index in destination sdu.
   */
  UBYTE index = 3;
  UBYTE type_2_header_index = 0;
  UBYTE sapi_index = 0;
  UBYTE ntt = 0;
  BOOL type_1_header_set = FALSE;
  BOOL type_2_header_set = FALSE;
  TRACE_FUNCTION( "mg_set_res_xid_params" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  /*
   * Set the values for parameter type 0 (version number).
   */
  sdu->buf[0] = SNDCP_XID_PARAM_TYPE_0;
  /*lint -e{415} (Warning -- access of out-of-bounds pointer)*/
  sdu->buf[1] = SNDCP_XID_0_LEN;
  /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
  sdu->buf[2] =
    sndcp_data->mg.res_xid_block[sapi_index].version;

  sdu->l_buf = 3 * 8;
  sdu->o_buf = 0;
  /*
   * Set the values for data compression, if necessary.
   */
  /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
  if (sndcp_data->mg.res_xid_block[sapi_index].v42.is_set) {
    sdu->buf[3] = SNDCP_XID_PARAM_TYPE_1;
    sdu->buf[4] = SNDCP_XID_1_LEN_RES;
    sdu->buf[5] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].v42.p_bit << 7) +
            sndcp_data->mg.res_xid_block[sapi_index].v42.ntt;
    sdu->buf[6] = SNDCP_XID_V42_LEN_RES;
    sdu->buf[7] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis >> 8);
    sdu->buf[8] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].v42.nsapis & 0xff);
    sdu->buf[9] = sndcp_data->mg.res_xid_block[sapi_index].v42.p0;
    sdu->buf[10] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].v42.p1 >> 8);
    sdu->buf[11] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].v42.p1 & 0xff);
    sdu->buf[12] = sndcp_data->mg.res_xid_block[sapi_index].v42.p2;

    index = 13;
    sdu->l_buf = 13 * 8;

    type_1_header_set = TRUE;
  }
  /*
   * Add rejected data compression entities.
   */
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    BOOL rej = FALSE;
    mg_get_sapi_dntt_rej(sapi, ntt, &rej);
    /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
    if (rej) {
      if (! type_1_header_set) {
        sdu->buf[index] = SNDCP_XID_PARAM_TYPE_1;
        index++;
        sdu->buf[index] = 0;
        index++;
        type_1_header_set = TRUE;
        sdu->l_buf += 16;
      }

      sdu->buf[index] = ntt;
      index++;
      /*
       * Length of field.
       */
      sdu->buf[index] = 2;
      index++;
      /*
       * Set 2 'nsapis' octets to 0.
       */
      sdu->buf[index] = 0;
      index++;
      sdu->buf[index] = 0;
      index++;
      /*
       * Increment sdu length.
       */
      sdu->l_buf = sdu->l_buf + 32;
      /*
       * Increment parameter type 1 length octet.
       */
       /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
      sdu->buf[4] += 4;
    }
  }
  type_2_header_index = index;

  /*
   * Set the values for header compression, if requested.
   */
   /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
  if (sndcp_data->mg.res_xid_block[sapi_index].vj.is_set) {
    sdu->buf[index] = SNDCP_XID_PARAM_TYPE_2;
    index++;
    sdu->buf[index] = SNDCP_XID_2_LEN_RES;
    index++;
    sdu->buf[index] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].vj.p_bit << 7) +
       sndcp_data->mg.res_xid_block[sapi_index].vj.ntt;
    index++;
    sdu->buf[index] = SNDCP_XID_VJ_LEN_RES;
    index++;
    sdu->buf[index] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis >> 8);
    index++;
    sdu->buf[index] =
      (UBYTE)(sndcp_data->mg.res_xid_block[sapi_index].vj.nsapis & 0xff);
    index++;
    sdu->buf[index] =
      sndcp_data->mg.res_xid_block[sapi_index].vj.s0_m_1;
    index++;

    sdu->l_buf = sdu->l_buf + 7 * 8;

    type_2_header_set = TRUE;
  }
  /*
   * Add rejected header compression entities.
   */
   /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    BOOL rej = FALSE;
    mg_get_sapi_pntt_rej(sapi, ntt, &rej);
    if (rej) {
      if (! type_2_header_set) {
        sdu->buf[index] = SNDCP_XID_PARAM_TYPE_2;
        index++;
        sdu->buf[index] = 0;
        index++;
        type_2_header_set = TRUE;
        sdu->l_buf += 16;
      }
      sdu->buf[index] = ntt;
      index++;
      /*
       * Length of field.
       */
      sdu->buf[index] = 2;
      index++;
      /*
       * Set 2 'nsapis' octets to 0.
       */
      sdu->buf[index] = 0;
      index++;
      sdu->buf[index] = 0;
      index++;
      /*
       * Increment sdu length.
       */
      sdu->l_buf = sdu->l_buf + 32;
      /*
       * Increment parameter type 2 length octet.
       */
      sdu->buf[type_2_header_index + 1] += 4;
    }
  }
  /*
   * Set sdu offset to 0.
   */
  sdu->o_buf = 0;
  /*
   * Reset the arrays with rejected params to all FALSE.
   */
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    sndcp_data->mg.sapi_dntt_rej_ra[sapi_index][ntt] = FALSE;
    sndcp_data->mg.sapi_pntt_rej_ra[sapi_index][ntt] = FALSE;
  }

} /* mg_set_res_xid_params() */


/*
+------------------------------------------------------------------------------
| Function    : mg_set_xid_params
+------------------------------------------------------------------------------
| Description : This procedure fills the XID block. It is implementation
|               dependent. In the current version V42bis and VanJacobson
|               header compression are applied.
|
| Parameters  :
|               sapi and sdu from the LL_XID_REQ that will be filled and
|               xid_block that defines the desired compressors
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_set_xid_params (UBYTE sapi, T_sdu* sdu, T_XID_BLOCK xid_block)
{
  /*
   * Byte index in destination sdu.
   */
  UBYTE index = 3;
  UBYTE sapi_index = 0;
  UBYTE ntt = 0;
  BOOL type_1_header_set = FALSE;
  BOOL type_2_header_set = FALSE;
  UBYTE type_2_header_index = 0;
  UBYTE type_1_header_index = 3;
  USHORT p1 = SNDCP_V42_DEFAULT_P1;
  TRACE_FUNCTION( "mg_set_xid_params" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  /*
   * Set the values for parameter type 0 (version number).
   */
  sdu->buf[0] = SNDCP_XID_PARAM_TYPE_0;
  /*lint -e{415} (Warning -- access of out-of-bounds pointer)*/
  sdu->buf[1] = SNDCP_XID_0_LEN;
  /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
  sdu->buf[2] = SNDCP_XID_VERSION;

  sdu->l_buf = 24; /* 3 * 8 */
  sdu->o_buf = 0;
  /*
   * Set the values for data compression, if necessary.
   */
   /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
  if (xid_block.v42.is_set &&
      #ifdef SNDCP_UPM_INCLUDED 
       xid_block.v42.p0 != NAS_DCOMP_OFF &&
      #else
       xid_block.v42.p0 != SNSM_COMP_NEITHER_DIRECT &&
      #endif  /*#ifdef SNDCP_UPM_INCLUDED*/
      xid_block.v42.nsapis_set &&
      xid_block.v42.nsapis > 0) {

    if (xid_block.v42.p_bit > 0) {
      sdu->buf[3] = SNDCP_XID_PARAM_TYPE_1;
      sdu->buf[4] = SNDCP_XID_1_LEN;
      sdu->buf[5] = (UBYTE)(xid_block.v42.p_bit << 7) + xid_block.v42.ntt;
      sdu->buf[6] = xid_block.v42.algo_type;
      sdu->buf[7] = SNDCP_XID_V42_LEN;
      sdu->buf[8] = (UBYTE)(xid_block.v42.dcomp << 4);
      sdu->buf[9] = (UBYTE)(xid_block.v42.nsapis >> 8);
      sdu->buf[10] = (UBYTE)(xid_block.v42.nsapis & 0xff);
      sdu->buf[11] = xid_block.v42.p0;
      /*
       * P1, P2
       */
      sdu->buf[12] = (p1 & 0xff00) >> 8;
      sdu->buf[13] = p1 & 0x00ff;
      sdu->buf[14] = SNDCP_V42_DEFAULT_P2;

      index = 15;
      sdu->l_buf = 15 * 8;
    } else {
      sdu->buf[3] = SNDCP_XID_PARAM_TYPE_1;
      sdu->buf[4] = SNDCP_XID_1_LEN_RES;
      sdu->buf[5] = xid_block.v42.ntt;
      sdu->buf[6] = SNDCP_XID_V42_LEN_RES;
      sdu->buf[7] = (UBYTE)(xid_block.v42.nsapis >> 8);
      sdu->buf[8] = (UBYTE)(xid_block.v42.nsapis & 0xff);
      sdu->buf[9] = xid_block.v42.p0;
      /*
       * P1, P2
       */
      sdu->buf[10] = (p1 & 0xff00) >> 8;
      sdu->buf[11] = p1 & 0x00ff;
      sdu->buf[12] = SNDCP_V42_DEFAULT_P2;

      index = 13;
      sdu->l_buf = 13 * 8;

    }
  }
  type_2_header_index = index;
  /*
   * Set the values for header compression, if requested.
   */
   /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
  if (xid_block.vj.is_set &&
      #ifdef SNDCP_UPM_INCLUDED
       xid_block.vj.direction != NAS_HCOMP_OFF &&
      #else
       xid_block.vj.direction != SNSM_COMP_NEITHER_DIRECT &&
      #endif  /*#ifdef SNDCP_UPM_INCLUDED*/
      xid_block.vj.nsapis_set &&
      xid_block.vj.nsapis > 0) {

    if (xid_block.vj.p_bit > 0) {

      sdu->buf[index] = SNDCP_XID_PARAM_TYPE_2;
      sdu->buf[index + 1] = SNDCP_XID_2_LEN;
      sdu->buf[index + 2] =
        (UBYTE)(xid_block.vj.p_bit << 7) + xid_block.vj.ntt;
      sdu->buf[index + 3] = SNDCP_XID_VJ;
      sdu->buf[index + 4] = SNDCP_XID_VJ_LEN;
      sdu->buf[index + 5] =
        (UBYTE)(xid_block.vj.pcomp1 << 4) + xid_block.vj.pcomp2;
      sdu->buf[index + 6] =
        (UBYTE)(xid_block.vj.nsapis >> 8);
      sdu->buf[index + 7] =
        (UBYTE)(xid_block.vj.nsapis & 0xff);
      sdu->buf[index + 8] = xid_block.vj.s0_m_1;

      sdu->l_buf = sdu->l_buf + 9 * 8;

      type_2_header_set = TRUE;
    } else {
      sdu->buf[index] = SNDCP_XID_PARAM_TYPE_2;
      sdu->buf[index + 1] = SNDCP_XID_2_LEN_RES;
      sdu->buf[index + 2] = xid_block.vj.ntt;

      sdu->buf[index + 3] = SNDCP_XID_VJ_LEN_RES;

      sdu->buf[index + 4] =
        (UBYTE)(xid_block.vj.nsapis >> 8);
      sdu->buf[index + 5] =
        (UBYTE)(xid_block.vj.nsapis & 0xff);
      sdu->buf[index + 6] = xid_block.vj.s0_m_1;

      sdu->l_buf = sdu->l_buf + 7 * 8;

      type_2_header_set = TRUE;
    }
  }
  /*
   * Add rejected data compression entities.
   */
   /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    BOOL rej = FALSE;
    mg_get_sapi_dntt_rej(sapi, ntt, &rej);
    if (rej) {
      if (! type_1_header_set) {
        sdu->buf[index] = SNDCP_XID_PARAM_TYPE_1;
        index++;
        sdu->buf[index] = 0;
        index++;
        type_1_header_set = TRUE;
        sdu->l_buf += 16;
      }
      sdu->buf[index] = ntt;
      index++;
      /*
       * Length of field.
       */
      sdu->buf[index] = 2;
      index++;
      /*
       * Set 2 'nsapis' octets to 0.
       */
      sdu->buf[index] = 0;
      index++;
      sdu->buf[index] = 0;
      index++;
      /*
       * Increment sdu length.
       */
      sdu->l_buf = sdu->l_buf + 32;
      /*
       * Increment parameter type 2 length octet.
       */
       /*lint -e{415, 416} (Warning -- access/creation of out-of-bounds pointer)*/
      sdu->buf[type_1_header_index + 1] += 4;
    }
  } /* for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++)  */
  /*
   * Add rejected header compression entities.
   */
   /*lint -e{661, 662} (Warning -- Possible access/creation of out-of-bounds pointer)*/
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    BOOL rej = FALSE;
    mg_get_sapi_pntt_rej(sapi, ntt, &rej);
    if (rej) {
      if (! type_2_header_set) {
        sdu->buf[index] = SNDCP_XID_PARAM_TYPE_2;
        index++;
        sdu->buf[index] = 0;
        index++;
        type_2_header_set = TRUE;
        sdu->l_buf += 16;
      }
      sdu->buf[index] = ntt;
      index++;
      /*
       * Length of field.
       */
      sdu->buf[index] = 2;
      index++;
      /*
       * Set 2 'nsapis' octets to 0.
       */
      sdu->buf[index] = 0;
      index++;
      sdu->buf[index] = 0;
      index++;
      /*
       * Increment sdu length.
       */
      sdu->l_buf = sdu->l_buf + 32;
      /*
       * Increment parameter type 2 length octet.
       */
      sdu->buf[type_2_header_index + 1] += 4;
    }
  } /* for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++)  */
  /*
   * Set sdu offset to 0.
   */
  sdu->o_buf = 0;
  /*
   * Reset the arrays with rejected params to all FALSE.
   */
  for (ntt = 0; ntt < MG_MAX_ENTITIES; ntt++) {
    sndcp_data->mg.sapi_dntt_rej_ra[sapi_index][ntt] = FALSE;
    sndcp_data->mg.sapi_pntt_rej_ra[sapi_index][ntt] = FALSE;
  }

} /* mg_set_xid_params() */



/*
+------------------------------------------------------------------------------
| Function    : mg_del_comp_pdus_ack
+------------------------------------------------------------------------------
| Description : The function mg_del_comp_pdus_ack()
|               GSM 4.65, 5.1.2.7:
|               "compressed N-PDUs queuing to be forwarded to the affected
|               SAPI are deleted from the SNDCP layer."
|               Assumption: It is assumed here that acknowledged and
|               unacknowledged npdus are deleted the same.
|
| Parameters  : UBYTE sapi: the affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_del_comp_pdus_ack (UBYTE sapi)
{
  UBYTE nsapi = 0;

  TRACE_FUNCTION( "mg_del_comp_pdus_ack" );

  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    UBYTE sp = 0;
    sndcp_get_nsapi_sapi(nsapi, &sp);
    if (sp == sapi) {
      sig_mg_cia_delete_npdus(nsapi);
      sig_mg_su_delete_pdus(nsapi, sapi);
      sig_mg_sua_delete_pdus(nsapi, sapi, TRUE);
      sig_mg_sd_delete_npdus(nsapi, sapi);
      sig_mg_sda_delete_npdus(nsapi, sapi);
    }
  }
} /* mg_del_comp_pdus_ack() */

/*
+------------------------------------------------------------------------------
| Function    : mg_delete_npdus
+------------------------------------------------------------------------------
| Description : The function mg_delete_npdus() sends signals to all affected
|               services to delete the buffered npdus.
|
| Parameters  : UBYTE nsapi: the affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_delete_npdus (UBYTE nsapi)
{
  UBYTE sapi = 0;
  TRACE_FUNCTION( "mg_delete_npdus" );
  sndcp_get_nsapi_sapi(nsapi, &sapi);

  sig_mg_cia_delete_npdus(nsapi);
  sig_mg_su_delete_pdus(nsapi, sapi);
  sig_mg_sua_delete_pdus(nsapi, sapi, TRUE);
  sig_mg_sd_delete_npdus(nsapi, sapi);
  sig_mg_sda_delete_npdus(nsapi, sapi);

  sig_mg_nu_delete_npdus(nsapi);

} /* mg_delete_npdus() */


/*
+------------------------------------------------------------------------------
| Function    : mg_suspend_affected_nus
+------------------------------------------------------------------------------
| Description : Suspends all nu service instances affected by XID negotiation
|               Sets service variable 'suspended_nsapis'.
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_suspend_affected_nus (UBYTE sapi) {
  UBYTE nsapi = 0;
  UBYTE sapi_index = 0;
  TRACE_FUNCTION( "mg_resume_affected_nus" );
  sndcp_get_sapi_index(sapi, &sapi_index);
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    if (1 << nsapi & sndcp_data->mg.req_xid_block[sapi_index].vj.nsapis ||
        1 << nsapi & sndcp_data->mg.cur_xid_block[sapi_index].vj.nsapis ||
        1 << nsapi & sndcp_data->mg.req_xid_block[sapi_index].v42.nsapis ||
        1 << nsapi & sndcp_data->mg.cur_xid_block[sapi_index].v42.nsapis) {

      sndcp_data->mg.suspended_nsapis |= (1 << nsapi);
      sig_mg_nu_suspend(nsapi);
    }
  }
} /* mg_suspend_affected_nus() */






/*
 * Getters and setters for the organizing arrays.
 */
/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_dntt_rej
+------------------------------------------------------------------------------
| Description : The procedures informs about if a given data
|               compression entity on a given SAPI is to be rejected in
|               LL_XID_REQ because it cannot be set up.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN dntt UBYTE,
|               IN/OUT rej BOOL
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_get_sapi_dntt_rej (UBYTE sapi, UBYTE dntt, BOOL* rej)
{
  TRACE_FUNCTION( "mg_get_sapi_dntt_rej" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *rej = sndcp_data->mg.sapi_dntt_rej_ra[sapi_index][dntt];
  }
} /* mg_get_sapi_dntt_rej() */

/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_pntt_rej
+------------------------------------------------------------------------------
| Description : The procedures informs about if a given header
|               compression entity on a given SAPI is to be rejected in
|               LL_XID_REQ because it cannot be set up.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN/OUT rej BOOL
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_get_sapi_pntt_rej (UBYTE sapi, UBYTE pntt, BOOL* rej)
{
  TRACE_FUNCTION( "mg_get_sapi_pntt_rej" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *rej = sndcp_data->mg.sapi_pntt_rej_ra[sapi_index][pntt];
  }
} /* mg_get_sapi_pntt_rej() */




/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_pntt_nsapi
+------------------------------------------------------------------------------
| Description : The procedure informs if a given nsapi uses a given pntt on a
|               given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pntt UBYTE,
|               IN nsapi UBYTE,
|               IN/OUT used BOOL
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_get_sapi_pntt_nsapi (UBYTE sapi, UBYTE pntt, UBYTE nsapi, BOOL* used)
{
  TRACE_FUNCTION( "mg_get_sapi_pntt_nsapi" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *used = sndcp_data->mg.sapi_pntt_nsapi_set_ra[sapi_index][pntt][nsapi];
  }
} /* mg_get_sapi_pntt_nsapi() */




/*
+------------------------------------------------------------------------------
| Function    : mg_get_sapi_pcomp_pntt
+------------------------------------------------------------------------------
| Description : The procedures indicates the pntt assigned to a given pcomp
|               for a given sapi.
|
| Parameters  : FPAR IN sapi UBYTE,
|               IN pcomp UBYTE,
|               IN/OUT dntt UBYTE
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_get_sapi_pcomp_pntt (UBYTE sapi, UBYTE pcomp, UBYTE* pntt)
{
  TRACE_FUNCTION( "mg_get_sapi_pcomp_pntt" );
  {
    UBYTE sapi_index = 0;
    sndcp_get_sapi_index(sapi, &sapi_index);
    *pntt = sndcp_data->mg.sapi_pcomp_pntt_ra[sapi_index][pcomp];
  }
} /* mg_get_sapi_pcomp_pntt() */



/*
+------------------------------------------------------------------------------
| Function    : mg_xid_cnf_ok_res
+------------------------------------------------------------------------------
| Description :
| After an LL_XID_REQ has been sent there are contexts in state MG_XID.
| These are now reset after reception of the LL_XID_CNF.
| The "applicable nsapis" in req_xid_block.v42 and req_xid_block.vj, each one
| only once.
| SIG_MG_NU_RESET(nsapi, discard_ready) is used.
| In every nu instance: N-PDU number is set to 0 for this instance and an
| SN_UNITREADY_IND is sent. State is set to NU_UNACK_SU_RECEPTIVE
| Then the SNSM_ACTIVATE_RES is sent for each one of these.
|
| Parameters  : affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_xid_cnf_ok_res (UBYTE sapi)
{
  UBYTE nsapi = 0;

  TRACE_FUNCTION( "mg_xid_cnf_ok_res" );
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    USHORT nsapi_state = MG_IDLE;
    USHORT sapi_state = MG_IDLE;
    UBYTE sapi_index = 0;
    UBYTE func_sapi = 0;
    sndcp_get_nsapi_sapi(nsapi, &func_sapi);
    if (func_sapi != sapi) {
      continue;
    }
    sndcp_get_sapi_index(sapi, &sapi_index);
    sndcp_get_nsapi_state(nsapi, &nsapi_state);
    sndcp_get_sapi_state(func_sapi, &sapi_state);


    if (((sapi_state & MG_XID_NEC) > 0)
        &&
        ((sapi_state & MG_REL) == 0)) {
      mg_resend_xid_if_nec(sapi);

      sndcp_get_sapi_state(func_sapi, &sapi_state);
      /*
       * If now XID is sent, no further actions for this nsapi.
       */
      if ((sapi_state & MG_XID) > 0) {
        continue;
      }

    } else if ((nsapi_state & MG_ACT) > 0 &&
        (sapi_state & MG_EST) == 0) {

      BOOL ack = FALSE;

      sndcp_get_nsapi_ack(nsapi, &ack);
      /*
       * Open DTI connection.
       */
#ifndef SNDCP_UPM_INCLUDED 
      mg_dti_open(nsapi);
#endif 
      if (ack) {
        sig_mg_nu_reset_ack(nsapi, 0, 0, FALSE);
        sig_mg_sda_getdata(sapi, nsapi);
      } else {
        sig_mg_nu_reset(nsapi, FALSE);
        sig_mg_sd_getunitdata(sapi, nsapi);
      }

      if ((sapi_state & MG_XID) == 0) {
        mg_send_snsm_activate_res(nsapi);
        /*
         * Set nsapi state to MG_IDLE.
         */
        sndcp_unset_nsapi_state(nsapi, MG_ACT);
      }
    } /* if state is MG_ACT and not MG_EST  */

    if ((nsapi_state & MG_DEACT) > 0) {

      USHORT local_sapi_state = MG_IDLE;

      sndcp_get_sapi_state(sapi, &local_sapi_state);

      if ((local_sapi_state &
            (MG_REL_NEC_LOC + MG_REL_NEC_PEER + MG_XID_NEC + MG_XID))
          == 0)
      {
        /*
         * No LL_RELEASE_REQ necessary. Notify sndcp.
         */
#ifdef SNDCP_UPM_INCLUDED 
        PALLOC(snsm_deactivate_res, SN_DEACTIVATE_CNF);
#else
        PALLOC(snsm_deactivate_res, SNSM_DEACTIVATE_RES);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
        /*
         * Now the NSAPI is not in use anymore:
         */
        sndcp_set_nsapi_used(nsapi, FALSE);
        sndcp_set_nsapi_ack(nsapi, FALSE);

        snsm_deactivate_res->nsapi = nsapi;
        sndcp_unset_nsapi_state(nsapi, MG_DEACT);
#ifdef SNDCP_UPM_INCLUDED 
        PSEND(hCommUPM, snsm_deactivate_res);
#else
        PSEND(hCommSM, snsm_deactivate_res);
#endif  /*#ifdef SNDCP_UPM_INCLUDED*/
        /*
         * Resume nsapi.
         */
        mg_resume_affected_nus(sapi);

      } else if ((local_sapi_state & (MG_REL_NEC_LOC + MG_REL_NEC_PEER)) > 0) {
        /*
         * LL_RELEASE_REQ must be sent.
         */

        PALLOC(ll_release_req, LL_RELEASE_REQ);

        sndcp_unset_sapi_state(sapi, MG_REL_NEC_LOC);
        ll_release_req->sapi = sapi;
        ll_release_req->local = TRUE;
        /*
         * Set the "state" for the affected and sapi to MG_REL_PENDING.
         */
        sndcp_set_sapi_state(sapi, MG_REL);

        PSEND(hCommLLC, ll_release_req);
      } else {

        UBYTE local_sapi = 0;
        UBYTE local_nsapi = 0;
        BOOL local_nec = FALSE;
        for (local_nsapi = 0;
             local_nsapi < SNDCP_NUMBER_OF_NSAPIS;
             local_nsapi++) {

          sndcp_get_nsapi_sapi(local_nsapi, &local_sapi);
          if (sapi == local_sapi) {
            /*
             * is compressor deactivation necessary?
             */
            sndcp_get_nsapi_state(local_nsapi, &nsapi_state);
            if ((nsapi_state & MG_DEACT) > 0) {
              mg_is_rel_comp_nec(local_nsapi, &local_nec);
            }
          }
        }

        /*
         * Negotiate possible deactivation of compressors.
         */
        if (local_nec) {
          mg_send_xid_req_del(sapi);
        }
      }

    }

  }

} /* mg_xid_cnf_ok_res */


#ifdef TI_DUAL_MODE
/*
+------------------------------------------------------------------------------
| Function    : mg_get_unsent_unconfirmed_npdus
+------------------------------------------------------------------------------
| Description : This procedure retrieves any unsent or unconfirmed acknowledged
|               NPDU from SNDCP's internal buffers. ANY retrieved NPDU is packed
|               the primitive SN_GET_PENDING_PDU_CNF data.
|
| Parameters  : nsapi, pointer to the primitive.
|
+------------------------------------------------------------------------------
*/
GLOBAL T_SN_GET_PENDING_PDU_CNF* mg_get_unsent_unconfirmed_npdus
                             (U8 nsapi, 
                              T_SN_GET_PENDING_PDU_CNF* sn_get_pending_pdu_cnf)
{

  int count = 0;
  U8 used_sapi=0, sapi_index=0;
  T_NPDU_BUFFER* help = NULL;

  TRACE_FUNCTION( "mg_get_unsent_unconfirmed_npdus" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nu = & sndcp_data->nu_base[nsapi];

  /*
   * Find number of unconfirmed NPDUs
   */
  help = sndcp_data->nu->first_buffered_npdu;
  while(help != NULL)
  {
    count++;
    help = help->next;
  }

  sn_get_pending_pdu_cnf->ul_pdus[nsapi].ptr_desc_list2 = 
           (T_SN_desc_list2*)DRP_ALLOC( count * sizeof( T_SN_desc_list2 ), 0 );

  sn_get_pending_pdu_cnf->ul_pdus[nsapi].nsapi = nsapi;
  sn_get_pending_pdu_cnf->ul_pdus[nsapi].dl_sequence_number = 
                                           sndcp_data->nu->rec_npdu_number_ack;
  sn_get_pending_pdu_cnf->ul_pdus[nsapi].c_desc_list2 = count;

  count = 0;

  /*
   * Check if there are buffered any acknowledged type NPDUs.
   */
  help = sndcp_data->nu->first_buffered_npdu;
  while(help != NULL)
  {

    sn_get_pending_pdu_cnf->ul_pdus[nsapi].ptr_desc_list2[count].first = 
                                        help->sn_data_req->desc_list2.first;
    sn_get_pending_pdu_cnf->ul_pdus[nsapi].ptr_desc_list2[count].list_len = 
                                        help->sn_data_req->desc_list2.list_len;
    count++;
    help = help->next;
  }
  return sn_get_pending_pdu_cnf;
}

/*
+------------------------------------------------------------------------------
| Function    : mg_clean_ack_npdu_queues_leave_data
+------------------------------------------------------------------------------
| Description : This procedure deletes SNDCP's internal buffers containing
|               unconfirmed acknowledged type NPDU's.
|               The data stored in the queues are not deleted.
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_clean_ack_npdu_queues_leave_data(U8 nsapi)
{

  U8 used_sapi=0, sapi_index=0;
  T_NPDU_BUFFER* help = NULL;
  T_SN_DATA_REQ* sn_data_req = NULL;

  TRACE_FUNCTION( "mg_clean_ack_npdu_queues_leave_data" );

   /*
    * set service instance according to sapi
    */
  sndcp_get_nsapi_sapi(nsapi, &used_sapi);
  sndcp_get_sapi_index(used_sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  if (sndcp_data->sua->sn_data_q_write != sndcp_data->sua->sn_data_q_read) 
  {
    /*
     * remove unhandled sn_data_req from queue.
     */
    sn_data_req = sndcp_data->sua->sn_data_q[sndcp_data->sua->sn_data_q_read];
    FREE(sn_data_req);
  }

  /*
   * set service instance according to nsapi
   */

  sndcp_data->nu = & sndcp_data->nu_base[nsapi];

  /*
   * clean buffer with unconfirmed NPDUs
   */

  while(sndcp_data->nu->first_buffered_npdu != NULL)
  {
    help = sndcp_data->nu->first_buffered_npdu;
    sndcp_data->nu->first_buffered_npdu = help->next;
    MFREE(help->sn_data_req);
    MFREE(help);
  }
}


/*
+------------------------------------------------------------------------------
| Function    : mg_clean_unack_npdu_queues_including_data
+------------------------------------------------------------------------------
| Description : This procedure deletes SNDCP's internal buffers containing
|               unacknowledged type NPDU's.
|               The data stored in the queues are also deleted.
|
| Parameters  : nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void mg_clean_unack_npdu_queues_including_data(U8 nsapi)
{
  /*
   * Delete affected N-PDUS from sn_unitdata_q.
   */
  BOOL still = TRUE;
  U8 index = sndcp_data->su->sn_unitdata_q_read;

  while (still) 
  {
    if (index == sndcp_data->su->sn_unitdata_q_write) {
      break;
    }
    if (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) {
      /*
       * The index for the prims to be shifted when 1 entry is deleted.
       */
      U8 i = 0;

      if (sndcp_data->su->sn_unitdata_q[index] != NULL) {
        PFREE_DESC2(sndcp_data->su->sn_unitdata_q[index]);
        sndcp_data->su->sn_unitdata_q[index] = NULL;
      }

      for (i = index;
           i != sndcp_data->su->sn_unitdata_q_write;
           i = (i + 1) % SN_UNITDATA_Q_LEN) 
           {
              sndcp_data->su->sn_unitdata_q[i] =
                sndcp_data->su->sn_unitdata_q[(i + 1) % SN_UNITDATA_Q_LEN];

              sndcp_data->su->sn_unitdata_q[(i + 1) % SN_UNITDATA_Q_LEN]=NULL;
           }
      sndcp_data->su->sn_unitdata_q_write =
        (sndcp_data->su->sn_unitdata_q_write - 1
         + SN_UNITDATA_Q_LEN) % SN_UNITDATA_Q_LEN;
    } 
    else 
    {
      index = (index + 1) % SN_UNITDATA_Q_LEN;
    } /* else (sndcp_data->su->sn_unitdata_q[index]->nsapi == nsapi) */
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_make_test_pending_pdu_cnf
+------------------------------------------------------------------------------
| Description : Initialize SN_TEST_GET_PENDING_PDU_CNF primitive
| Parameters  : sn_get_pending_pdu_cnf - primitive  
|               sn_test_get_pending_pdu_cnf - primitive 
|
+------------------------------------------------------------------------------
*/

GLOBAL void sm_make_test_pending_pdu_cnf
                   (T_SN_GET_PENDING_PDU_CNF* sn_get_pending_pdu_cnf,
                    T_SN_TEST_GET_PENDING_PDU_CNF* sn_test_get_pending_pdu_cnf)
{
  int i,j;

  for (i = 0; i < SN_SIZE_NSAPI; i++) {
    sn_test_get_pending_pdu_cnf->c_test_ul_pdus = 
                         sn_get_pending_pdu_cnf->c_ul_pdus;
    sn_test_get_pending_pdu_cnf->test_ul_pdus[i].nsapi = 
                         sn_get_pending_pdu_cnf->ul_pdus[i].nsapi;
    sn_test_get_pending_pdu_cnf->test_ul_pdus[i].test_dl_sequence_number = 
                         sn_get_pending_pdu_cnf->ul_pdus[i].dl_sequence_number;
    sn_test_get_pending_pdu_cnf->test_ul_pdus[i].c_test_desc_list2 = 
                         sn_get_pending_pdu_cnf->ul_pdus[i].c_desc_list2;

    if (sn_get_pending_pdu_cnf->ul_pdus[i].c_desc_list2) {
      for(j = 0; j < sn_get_pending_pdu_cnf->ul_pdus[i].c_desc_list2; j++) {
        sn_test_get_pending_pdu_cnf->test_ul_pdus[i].c_test_desc_list2 = 
                 sn_get_pending_pdu_cnf->ul_pdus[i].c_desc_list2;
        sn_test_get_pending_pdu_cnf->test_ul_pdus[i].test_desc_list2[j].c_test_pending_pdu = 
             (U8)sn_get_pending_pdu_cnf->ul_pdus[i].ptr_desc_list2[j].list_len;
        memcpy(
          sn_test_get_pending_pdu_cnf->test_ul_pdus[i].test_desc_list2[j].test_pending_pdu,
          ((T_desc2 *)sn_get_pending_pdu_cnf->ul_pdus[i].ptr_desc_list2[j].first)->buffer, 
          sn_get_pending_pdu_cnf->ul_pdus[i].ptr_desc_list2[j].list_len);
      }
    }
  }
}

#endif /*#ifdef TI_DUAL_MODE*/
