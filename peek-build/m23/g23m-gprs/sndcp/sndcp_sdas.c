/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sdas.c
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
|             described in the SDL-documentation (SD-statemachine)
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

#include "sndcp_sdaf.h"    /* to get the internal functions of this service */
#include "sndcp_nds.h"    /* to get signals to nd service */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sda_cia_decomp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SDA_CIA_DECOMP_IND
|
| Parameters  : cia_decomp_ind, p_id
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sda_cia_decomp_ind (T_CIA_DECOMP_IND* cia_decomp_ind, UBYTE p_id) 
{ 
  UBYTE sapi_index = 0;
  UBYTE sapi = 0;
  UBYTE nsapi= cia_decomp_ind->pdu_ref.ref_nsapi;
  BOOL rec = FALSE;
  T_SN_DATA_IND* sn_data_ind = NULL;

  TRACE_ISIG( "sig_cia_sda_cia_decomp_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_nsapi_sapi(cia_decomp_ind->pdu_ref.ref_nsapi, &sapi);
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];

  
  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SDA_RECEIVE_FIRST_SEGMENT:
    case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
    case SDA_ESTABLISH_REQUESTED:
      {
       
        MALLOC(sn_data_ind, sizeof(T_SN_DATA_IND));

#ifdef _SNDCP_DTI_2_
        sn_data_ind->desc_list2.first = cia_decomp_ind->desc_list2.first;
        sn_data_ind->desc_list2.list_len = cia_decomp_ind->desc_list2.list_len;
#endif /*_SNDCP_DTI_2_*/

#ifdef SNDCP_TRACE_ALL

        sndcp_data->cia.cia_decomp_ind_number[nsapi] --;
        TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                       sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */

        sn_data_ind->nsapi = nsapi;
        sn_data_ind->p_id = p_id;
        sda_is_nsapi_rec(nsapi, &rec);
        if (rec) {
          sda_set_nsapi_rec(nsapi, FALSE);
          sig_sda_nd_data_ind(nsapi, 
                              sn_data_ind, 
                              (UBYTE)cia_decomp_ind->pdu_ref.ref_npdu_num);
          sda_get_data_if_nec(sapi);
        } else {
          if (sndcp_data->sda->cur_sn_data_ind[nsapi] NEQ NULL)
          {
#ifndef NTRACE
#ifdef SNDCP_TRACE_ALL
            TRACE_EVENT_P1("Discard cur_sn.. in %s", sndcp_data->sda->state_name);
#endif /* SNDCP_TRACE_ALL */
#endif /* NTRACE */
            sda_delete_cur_sn_data_ind(nsapi);
          }
          sndcp_data->sda->cur_sn_data_ind[nsapi] = sn_data_ind;
          sndcp_data->sda->uncomp_npdu_num = 
                               (UBYTE)cia_decomp_ind->pdu_ref.ref_npdu_num;
          sndcp_set_nsapi_rec_state(nsapi, SDA_WAIT_NSAPI);
          sda_get_data_if_nec ( sapi );
        }
      }
      MFREE(cia_decomp_ind);
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SDA_CIA_DECOMP_IND unexpected" );
      MFREE_PRIM(cia_decomp_ind); 
      break;
  }
} /* sig_cia_sda_cia_decomp_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sda_delete_npdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SD_DELETE_NPDUS
|
| Parameters  : affected NSAPI, SAPI
| Pre         : If the affected SAPI no longer uses acknowledged LLC operation
|               mode, no LL_GETDATA_REQ shall be sent. So sndcp_sapi_state_ra
|               must already be set to MG_REL_PENDING before this functon is 
|               called (if that sub state is desired).
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sda_delete_npdus (UBYTE nsapi,
                                    UBYTE sapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sda_delete_npdus" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
  
  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SDA_ESTABLISH_REQUESTED:
    case SDA_RECEIVE_FIRST_SEGMENT:
    case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
      if (sndcp_data->sda->cur_sn_data_ind[nsapi] != NULL) {
        sda_delete_cur_sn_data_ind(nsapi);
        sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
      } else {
        sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
      }
      break;
    case SDA_WAIT_NSAPI:
      if (sndcp_data->sda->cur_sn_data_ind[nsapi] != NULL) {
        USHORT sapi_state = MG_IDLE;

        sda_delete_cur_sn_data_ind(nsapi);
        /*
         * Does it make sense to request another acknowlegded segment or is
         * the whole SAPI now using only unacknowledged mode?
         */
        sndcp_get_sapi_state(sapi, &sapi_state);
        if ((sapi_state & MG_REL) == 0) {
          sda_get_data_if_nec(sapi);
        }
        sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
      } 
      break;
    default:
      TRACE_ERROR( "SIG_MG_SDA_DELETE_NPDUS unexpected" );
      break;
  }
} /* sig_mg_sda_delete_npdus() */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sda_end_est
+------------------------------------------------------------------------------
| Description : Handles the signal PD_SD_END_EST
|
| Parameters  : affected sapi, success
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sda_end_est (UBYTE sapi, BOOL success)
{ 
  UBYTE sapi_index = 0;
  UBYTE nsapi = 0;

  TRACE_FUNCTION( "sig_pda_sda_est_cnf" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];

  /*
   * Send LL_GETDATA_REQ. 
   */
  if (success){
    sndcp_data->sda->llc_may_send = FALSE;
    sda_get_data_if_nec(sapi);
  }
  /*
   * All nsapis at this sapi that use ack mode, enter 
   * SDA_ESTABLISH_REQUESTED state.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
    UBYTE local_sapi = 0;
    BOOL used = FALSE;

    sndcp_is_nsapi_used(nsapi, &used);
    if (!used) {
      continue;
    }
    sndcp_get_nsapi_sapi(nsapi, &local_sapi);
    if (local_sapi == sapi) {
      switch( sndcp_get_nsapi_rec_state(nsapi) )
      {
        case SDA_ESTABLISH_REQUESTED:
        case SDA_RECEIVE_FIRST_SEGMENT:
        case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
        case SDA_WAIT_NSAPI:
        case SDA_ACK_DISCARD:
          sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
          break;
        default:
          TRACE_EVENT( "SIG_MG_SDA_END_EST unexpected for this NSAPI" );
          break;
      }
    }
  } /* for all nsapis */


} /* sig_mg_sda_end_est() */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sda_getdata
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SD_GETDATA
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sda_getdata (UBYTE sapi, UBYTE nsapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sda_getdata" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
  
  switch( GET_STATE(SDA) )
  {
    case SDA_DEFAULT:
      sda_get_data_if_nec(sapi);
      sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
      break;
    default:
      TRACE_ERROR( "SIG_MG_SDA_GETDATA unexpected" );
      break;
  }
} /* sig_mg_sda_getdata() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sda_getdata
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SDA_GETDATA
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sda_getdata (UBYTE sapi, UBYTE nsapi)
{

  TRACE_ISIG( "sig_cia_sda_getdata" );

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SDA_RECEIVE_FIRST_SEGMENT:
    case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
    case SDA_WAIT_NSAPI:
      sda_get_data_if_nec(sapi);
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SDA_GETDATA unexpected" );
      break;
  }
} /* sig_cia_sda_getdata() */


/*
+------------------------------------------------------------------------------
| Function    : sig_nd_sda_getdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ND_SDA_GETDATA_REQ
|
| Parameters  : affected sapi, affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_nd_sda_getdata_req (UBYTE sapi,
                                    UBYTE nsapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_nd_sda_getdata_req" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
  
  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SDA_RECEIVE_FIRST_SEGMENT:
    case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
      sda_set_nsapi_rec(nsapi, TRUE);
      break;
    case SDA_WAIT_NSAPI:
      if (sndcp_data->sda->cur_sn_data_ind[nsapi] != NULL) {
        /*
         * Send the just reassembled N-PDU (must be present because of state!).
         */
        sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
        sig_sda_nd_data_ind(nsapi, 
                            sndcp_data->sda->cur_sn_data_ind[nsapi], 
                            sndcp_data->sda->uncomp_npdu_num);
        sndcp_data->sda->cur_sn_data_ind[nsapi] = NULL;
        sda_get_data_if_nec(sapi);
      } else {
        /*
         * Set the receptive for the given nsapi.
         */
        sda_set_nsapi_rec(nsapi, TRUE);
      }
      break;
    default:
      TRACE_ERROR( "SIG_ND_SDA_GETDATA_REQ unexpected" );
      break;
  }
} /* sig_nd_sda_getdata_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sda_start_est
+------------------------------------------------------------------------------
| Description : Handles the signal PD_SD_START_EST
|
| Parameters  : affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sda_start_est (UBYTE sapi)
{ 
  UBYTE nsapi = 0;

  TRACE_FUNCTION( "sig_pda_sda_start_est" );
  
  /*
   * All nsapis at this sapi that use ack mode, enter 
   * SDA_ESTABLISH_REQUESTED state.
   */
  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi ++) {
    UBYTE sapi_help = 0;
    BOOL used = FALSE;

    sndcp_is_nsapi_used(nsapi, &used);
    if (!used) {
      continue;
    }
    sndcp_get_nsapi_sapi(nsapi, &sapi_help);
    if (sapi_help == sapi) {
      switch( sndcp_get_nsapi_rec_state(nsapi) )
      {
        case SDA_ESTABLISH_REQUESTED:
        case SDA_RECEIVE_FIRST_SEGMENT:
        case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
        case SDA_WAIT_NSAPI:
          sndcp_set_nsapi_rec_state(nsapi, SDA_ESTABLISH_REQUESTED);
          break;
        default:
          TRACE_EVENT( "SIG_MG_SDA_START_EST unexpected for this NSAPI" );
          break;
      }
    }
  } /* for all nsapis */

} /* sig_mg_sda_start_est() */


/*
+------------------------------------------------------------------------------
| Function    : sig_pd_sda_data_ind
+------------------------------------------------------------------------------
| Description : Handles the signal PD_SD_DATA_IND
|
| Parameters  : *ll_data_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pda_sda_data_ind ( T_LL_DATA_IND *ll_data_ind )
{ 
  UBYTE sapi_index = 0;
  BOOL valid = FALSE;
  UBYTE nsapi = 0;
  UBYTE sapi = 0;

  TRACE_FUNCTION( "sig_pda_sda_data_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_data_ind->sapi, &sapi_index);
  sndcp_data->sda = & sndcp_data->sda_base[sapi_index];
  sda_get_nsapi(ll_data_ind, &nsapi);
  
  /*
   * LLC has now "used up" it's pending LL_GETDATA_REQ and will have 
   * to wait for the next one.
   */
  sndcp_data->sda->llc_may_send = FALSE;
  sapi = ll_data_ind->sapi;

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SDA_ESTABLISH_REQUESTED:
      /* 
       * Just discard the segment. Next segment will be requested after 
       * confirmation of establishment.
       */
      PFREE(ll_data_ind);
      ll_data_ind = NULL;
      break;

    case SDA_RECEIVE_FIRST_SEGMENT:
      /* 
       * Invalid segments are discarded without error notification
       */
      sda_is_seg_valid(ll_data_ind, &valid);
      if (!valid) {
        PFREE(ll_data_ind);
        sda_get_data_if_nec(sapi);
        return;
      }
      if (sda_f_bit(ll_data_ind)) {
        if (sda_m_bit(ll_data_ind)) {
          sda_ac_f_f1_m1(ll_data_ind);
        } else {
          sda_ac_f_f1_m0(ll_data_ind);
        }
      } else {
        sda_ac_f_f0(ll_data_ind);
      }
      
      break;

    case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
      /* 
       * Invalid segments are discarded without error notification
       */
      sda_is_seg_valid(ll_data_ind, &valid);
      if (!valid) {
        PFREE(ll_data_ind);
        sda_get_data_if_nec(sapi);
        return;
      }
      if (sda_f_bit(ll_data_ind)) {
        if (sda_m_bit(ll_data_ind)) {
          sda_ac_s_f1_m1(ll_data_ind);
        } else {
          sda_ac_s_f1_m0(ll_data_ind);
        }
      } else {
        if (sda_m_bit(ll_data_ind)) {
          sda_ac_s_f0_m1(ll_data_ind);
        } else {
          sda_ac_s_f0_m0(ll_data_ind);
        }
      }
      
      break;

    case SDA_WAIT_NSAPI:
      if (ll_data_ind != NULL)
      {
        PFREE(ll_data_ind);
      }
      sda_get_data_if_nec(sapi); 
      break;

    case SDA_ACK_DISCARD:
      /*
       * In this state we will discard every downlink segment till we receive.
       * the last segment.
       * Once we receive the last segment, we will move to the state
       * SDA_RECEIVE_FIRST_SEGMENT.
       */
      TRACE_EVENT( "SDA_ACK_DISCARD state,Discard Segments ..." );
      sndcp_data->cur_segment_number[nsapi] ++;
      if (!sda_m_bit(ll_data_ind))
      {
        TRACE_EVENT( "Received the last Segment in SDA_ACK_DISCARD state " );
        sndcp_data->cur_segment_number[nsapi] = 0;
        sndcp_set_nsapi_rec_state(nsapi, SDA_RECEIVE_FIRST_SEGMENT);
      }
      PFREE(ll_data_ind);
      sda_get_data_if_nec(sapi);
      break;

    default:
      TRACE_ERROR( "SIG_PDA_SDA_DATA_IND unexpected" );
      /*
       * The primitive LL_DATA_IND is unexpected or the
       * NSAPI value is wrong. Discard the prim and send a FC prim.
       */
      if (ll_data_ind != NULL)
      {
        PFREE(ll_data_ind);
      }
      sda_get_data_if_nec(sapi);
      break;
  }
  
} /* sig_pda_sda_data_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sda_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SDA_RESET_IND
|
| Parameters  : affected sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sda_reset_ind (UBYTE nsapi) 
{ 

  TRACE_ISIG( "sig_mg_sda_reset_ind" );

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
      case SDA_ESTABLISH_REQUESTED:
      case SDA_RECEIVE_FIRST_SEGMENT:
      case SDA_RECEIVE_SUBSEQUENT_SEGMENT:
      case SDA_WAIT_NSAPI:
      case SDA_ACK_DISCARD:
        /*
         * Similar to sd.
         */
        sndcp_data->cur_dcomp[nsapi] = 0;
        sndcp_data->cur_pcomp[nsapi] = 0;
      break;
    default:
      TRACE_ERROR( "SIG_MG_SDA_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_sda_reset_ind() */

