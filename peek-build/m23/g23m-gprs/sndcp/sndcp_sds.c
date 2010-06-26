/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sds.c
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

#include "sndcp_sdf.h"    /* to get the internal functions of this service */
#include "sndcp_nds.h"    /* to get signals to nd service */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sd_cia_decomp_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SD_CIA_DECOMP_CNF
|
| Parameters  : cia_decomp_ind, p_id
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sd_cia_decomp_ind (T_CIA_DECOMP_IND* cia_decomp_ind, UBYTE p_id)
{
  UBYTE sapi_index = 0;
  UBYTE sapi = 0;
  UBYTE nsapi = cia_decomp_ind->pdu_ref.ref_nsapi;
  BOOL rec = FALSE;
  T_SN_UNITDATA_IND* sn_unitdata_ind = NULL;

  TRACE_ISIG( "sig_cia_sd_cia_decomp_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_nsapi_sapi(cia_decomp_ind->pdu_ref.ref_nsapi, &sapi);
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
      {
        MALLOC(sn_unitdata_ind, sizeof(T_SN_UNITDATA_IND));

#ifdef _SNDCP_DTI_2_
        sn_unitdata_ind->desc_list2.first = cia_decomp_ind->desc_list2.first;
        sn_unitdata_ind->desc_list2.list_len = 
              cia_decomp_ind->desc_list2.list_len;
#endif /*_SNDCP_DTI_2_*/

#ifdef SNDCP_TRACE_ALL
        sndcp_data->cia.cia_decomp_ind_number[nsapi] --;
        TRACE_EVENT_P1("number of cia_decomp_ind: % d",
                       sndcp_data->cia.cia_decomp_ind_number[nsapi]);
#endif /* SNDCP_TRACE_ALL */


        sn_unitdata_ind->nsapi = nsapi;

        sn_unitdata_ind->p_id = p_id;
        sd_is_nsapi_rec(nsapi, &rec);
        if (rec) {
          sig_sd_nd_unitdata_ind(nsapi, sn_unitdata_ind);
          sd_set_nsapi_rec(nsapi, FALSE);
          sd_get_unitdata_if_nec(sapi);
        } else {
          if (sndcp_data->sd->cur_sn_unitdata_ind[nsapi] NEQ NULL)
          {
#ifndef NTRACE
#ifdef SNDCP_TRACE_ALL
            TRACE_EVENT_P1("Discard cur_sn.. in %s", sndcp_data->sd->state_name);
#endif /* SNDCP_TRACE_ALL */
#endif /*  NTRACE  */
            sd_delete_cur_sn_unitdata_ind(nsapi);
          }
          sndcp_data->sd->cur_sn_unitdata_ind[nsapi] = sn_unitdata_ind;
          sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_WAIT_NSAPI);
        }
      }
      MFREE(cia_decomp_ind);
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SD_CIA_DECOMP_IND unexpected" );
      MFREE_PRIM(cia_decomp_ind); 
      break;
  }
} /* sig_cia_sd_cia_decomp_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sd_delete_npdus
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SD_DELETE_NPDUS
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sd_delete_npdus (UBYTE nsapi,
                                    UBYTE sapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sd_delete_npdus" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
      break;
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
      if (sndcp_data->sd->cur_sn_unitdata_ind[nsapi] != NULL) {
        sd_delete_cur_sn_unitdata_ind(nsapi);
        sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
      } else {
        sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
      }
      break;
    case SD_UNACK_WAIT_NSAPI:
      if (sndcp_data->sd->cur_sn_unitdata_ind[nsapi] != NULL) {
        sd_delete_cur_sn_unitdata_ind(nsapi);
        sd_get_unitdata_if_nec(sapi);
        sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
      }
      break;
    default:
      TRACE_ERROR( "SIG_MG_SD_DELETE_NPDUS unexpected" );
      break;
  }
} /* sig_mg_sd_delete_npdus() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sd_getunitdata
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SD_GETUNITDATA
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sd_getunitdata (UBYTE sapi, UBYTE nsapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_mg_sd_getunitdata" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  switch( GET_STATE(SD) )
  {
    case SD_DEFAULT:
      sd_get_unitdata_if_nec(sapi);
      sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
      break;
    default:
      TRACE_ERROR( "SIG_MG_SD_GETUNITDATA unexpected" );
      break;
  }
} /* sig_mg_sd_getunitdata() */




/*
+------------------------------------------------------------------------------
| Function    : sig_nd_sd_getunitdata_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ND_SD_GETUNITDATA_REQ
|
| Parameters  : affected sapi, affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_nd_sd_getunitdata_req (UBYTE sapi,
                                        UBYTE nsapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_nd_sd_getunitdata_req" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
      sd_set_nsapi_rec(nsapi, TRUE);
      break;
    case SD_UNACK_WAIT_NSAPI:
      if (sndcp_data->sd->cur_sn_unitdata_ind[nsapi] NEQ NULL) {
        /*
         * Send the just reassembled N-PDU (must be present because of state!).
         */
        sig_sd_nd_unitdata_ind(nsapi, sndcp_data->sd->cur_sn_unitdata_ind[nsapi]);
        sndcp_data->sd->cur_sn_unitdata_ind[nsapi] = NULL;
        sd_get_unitdata_if_nec(sapi);
        sndcp_set_nsapi_rec_state(nsapi, SD_UNACK_RECEIVE_FIRST_SEGMENT);
      } else {
        /*
         * Set the receptive for the given nsapi.
         */
        sd_set_nsapi_rec(nsapi, TRUE);
      }
      break;
    default:
      TRACE_ERROR( "SIG_ND_SD_GETUNITDATA_REQ unexpected" );
      break;
  }
} /* sig_nd_sd_getunitdata_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_mg_sd_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MG_SD_RESET_IND
|
| Parameters  : affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mg_sd_reset_ind (UBYTE nsapi)
{
  TRACE_ISIG( "sig_mg_sd_reset_ind" );

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
    case SD_UNACK_WAIT_NSAPI:
      sndcp_data->cur_dcomp[nsapi] = 0;
      sndcp_data->cur_pcomp[nsapi] = 0;
      break;
    default:
      TRACE_ERROR( "SIG_MG_SD_RESET_IND unexpected" );
      break;
  }
} /* sig_mg_sd_reset_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_pd_sd_unitdata_ind
+------------------------------------------------------------------------------
| Description : Handles the signal PD_SD_UNITDATA_IND
|
| Parameters  : *ll_unitdata_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pd_sd_unitdata_ind ( T_LL_UNITDATA_IND *ll_unitdata_ind )
{
  UBYTE sapi_index = 0;
  BOOL valid = FALSE;
  UBYTE nsapi = 0;

  TRACE_ISIG( "sig_pd_sd_unitdata_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_unitdata_ind->sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  sd_get_nsapi(ll_unitdata_ind, &nsapi);
  sndcp_data->big_head[nsapi] = FALSE;

  /*
   * LLC has now "used up" it's pending LL_GETUNITDATA_REQ and will have
   * to wait for the next one.
   */
  sndcp_data->sd->llc_may_send = FALSE;

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
      /*
       * Invalid segments are discarded without error notification
       */
      sd_is_seg_valid(ll_unitdata_ind, &valid);
      if (!valid) {
        if (ll_unitdata_ind != NULL) {
          UBYTE sapi = ll_unitdata_ind->sapi;
          PFREE(ll_unitdata_ind);
          sd_get_unitdata_if_nec(sapi);
        }
        return;
      }
      if (sd_f_bit(ll_unitdata_ind)) {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_d_f1_m1(ll_unitdata_ind);
        } else {
          sd_un_d_f1_m0(ll_unitdata_ind);
        }
      } else {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_d_f0_m1(ll_unitdata_ind);
        } else {
          sd_un_d_f0_m0(ll_unitdata_ind);
        }
      }
      break;
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
      /*
       * Invalid segments are discarded without error notification
       */
      sd_is_seg_valid(ll_unitdata_ind, &valid);
      if (!valid) {
        if (ll_unitdata_ind != NULL) {
          UBYTE sapi = ll_unitdata_ind->sapi;
          PFREE (ll_unitdata_ind);
          sd_get_unitdata_if_nec(sapi);
        }
        return;
      }
      if (sd_f_bit(ll_unitdata_ind)) {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_f_f1_m1(ll_unitdata_ind);
        } else {
          sd_un_f_f1_m0(ll_unitdata_ind);
        }
      } else {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_f_f0_m1(ll_unitdata_ind);
        } else {
          sd_un_f_f0_m0(ll_unitdata_ind);
        }
      }

      break;
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
      /*
       * Invalid segments are discarded without error notification
       */
      sd_is_seg_valid(ll_unitdata_ind, &valid);
      if (!valid) {
        if (ll_unitdata_ind != NULL) {
          UBYTE sapi = ll_unitdata_ind->sapi;
          PFREE(ll_unitdata_ind);
          sd_get_unitdata_if_nec(sapi);
        }
        return;
      }
      if (sd_f_bit(ll_unitdata_ind)) {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_s_f1_m1(ll_unitdata_ind);
        } else {
          sd_un_s_f1_m0(ll_unitdata_ind);
        }
      } else {
        if (sd_m_bit(ll_unitdata_ind)) {
          sd_un_s_f0_m1(ll_unitdata_ind);
        } else {
          sd_un_s_f0_m0(ll_unitdata_ind);
        }
      }

      break;

    case SD_UNACK_WAIT_NSAPI:
      /*
       * This NSAPI is waiting for the DL flow control prim
       * from upper layer. The data for other NSAPI's could get passed,
       * but since this packet is for the blocked NSAPI, the prim is discarded
       * and next packet is not requested.
       */
      if (ll_unitdata_ind != NULL) {
        PFREE (ll_unitdata_ind);
      }
      break;

    default:
      TRACE_ERROR( "SIG_PD_SD_UNITDATA_IND unexpected" );
      if (ll_unitdata_ind != NULL) {
        UBYTE sapi = ll_unitdata_ind->sapi;
        PFREE (ll_unitdata_ind);
        sd_get_unitdata_if_nec(sapi);
      }
      break;
  }

} /* sig_pd_sd_unitdata_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cia_sd_getunitdata
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CIA_SD_GETUNITDATA
|
| Parameters  : sapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cia_sd_getunitdata (UBYTE sapi, UBYTE nsapi)
{
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_cia_sd_getunitdata" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(sapi, &sapi_index);
  sndcp_data->sd = & sndcp_data->sd_base[sapi_index];

  switch( sndcp_get_nsapi_rec_state(nsapi) )
  {
    case SD_UNACK_DISCARD:
    case SD_UNACK_RECEIVE_FIRST_SEGMENT:
    case SD_UNACK_RECEIVE_SUBSEQUENT_SEGMENT:
    case SD_UNACK_WAIT_NSAPI:
      sd_get_unitdata_if_nec(sapi);
      break;
    default:
      TRACE_ERROR( "SIG_CIA_SD_GETUNITDATA unexpected" );
      break;
  }
} /* sig_cia_sd_getunitdata() */

