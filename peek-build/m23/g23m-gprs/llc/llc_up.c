/* 
+----------------------------------------------------------------------------- 
|  Project :   
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (U-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UP_C
#define LLC_UP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get memcpy() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_uf.h"     /* to get local U functions */
#include "llc_ul.h"     /* to get local U labels */
#include "llc_t200s.h"  /* to get signal interface to T200 */
#include "llc_txs.h"    /* to get signal interface to TX */
#include "llc_llmes.h"  /* to get signal interface to LLME */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : u_ll_establish_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_ESTABLISH_REQ
|
| Parameters  : *ll_establish_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_ll_establish_req ( T_LL_ESTABLISH_REQ *ll_establish_req )
{ 
  int bytelen;
  
  TRACE_FUNCTION( "ll_establish_req" );

  SWITCH_LLC (ll_establish_req->sapi);

  TRACE_2_PARA("s:%d xid-len:%d", ll_establish_req->sapi, BYTELEN(ll_establish_req->sdu.l_buf) );

  /*
   * Set TLLI for current transaction.
   */
  llc_data->u->current_tlli = llc_data->tlli_new;

  if (llc_data->u->ll_xid_resp_pending EQ TRUE)
  {
    PFREE (ll_establish_req);
    TRACE_0_INFO( "LL_ESTABLISH_REQ ignored due to LL_XID_IND collision" );
    return;
  }

  switch (GET_STATE(U))
  {
    case U_ADM:
      /*
       * store Layer-3 XID negotiation parameters, free
       * LL_ESTABLISH_REQ, set retransmission counter to 0 
       * and remember to CNF the establishment afterwards
       */

      /*
       * The layer_3 xid is valid any time, even it is a
       * zero lenght sdu. In this case we have to include
       * an empty l3 xid information field.
       */
      llc_data->requested_l3_xid->valid = TRUE;

      bytelen = BYTELEN( ll_establish_req->sdu.l_buf);
      bytelen = (bytelen < LLC_MAX_L3_XID_LEN ) ? bytelen : LLC_MAX_L3_XID_LEN;

      memcpy( llc_data->requested_l3_xid->value,
              &(ll_establish_req->sdu.buf[ll_establish_req->sdu.o_buf >> 3]),
              bytelen );

      llc_data->requested_l3_xid->length = bytelen;

      PFREE (ll_establish_req);

      llc_data->u->retransmission_counter = 0;

      /*
       * Send LL_ESTABLISH_CNF after successful establishment.
       */
      llc_data->u->ind_cnf_establishment  = CNF_ESTABLISHMENT;

      SET_STATE (U, U_LOCAL_ESTABLISHMENT);

      /*
       * send SABM (w/ Layer 3 XID parameters), start T200
       */
      u_send_sabm ();
      break;

    case U_LOCAL_ESTABLISHMENT:
      /*
       * XID Negotiation of layer-3 parameters not supported in 
       * this case.
       */
      PFREE (ll_establish_req);

      /*
       * Send LL_ESTABLISH_CNF after successful establishment.
       */
      llc_data->u->ind_cnf_establishment = CNF_ESTABLISHMENT;
      break;

    case U_ABM:
      /*
       * store Layer-3 XID negotiation parameters, free
       * LL_ESTABLISH_REQ, set retransmission counter to 0 
       * and remember to CNF the establishment afterwards
       */

      /*
       * The layer_3 xid is valid any time, even it is a
       * zero lenght sdu. In this case we have to include
       * an empty l3 xid information field.
       */
      llc_data->requested_l3_xid->valid = TRUE;

      bytelen = BYTELEN( ll_establish_req->sdu.l_buf);
      bytelen = (bytelen < LLC_MAX_L3_XID_LEN ) ? bytelen : LLC_MAX_L3_XID_LEN;

      memcpy( llc_data->requested_l3_xid->value,
              &(ll_establish_req->sdu.buf[ll_establish_req->sdu.o_buf >> 3]),
              bytelen );

      llc_data->requested_l3_xid->length = bytelen;

      PFREE (ll_establish_req);

      llc_data->u->retransmission_counter = 0;

      /*
       * Send LL_ESTABLISH_CNF after successful establishment.
       */
      llc_data->u->ind_cnf_establishment  = CNF_ESTABLISHMENT;

      SET_STATE (U, U_LOCAL_ESTABLISHMENT);

      sig_u_llme_abmrel_ind();

      u_send_llgmm_status_ind (LLGMM_ERRCS_L3_REEST);

      /*
       * send SABM (w/ Layer 3 XID parameters), start T200
       */
      u_send_sabm ();
      break;

    case U_ESTABLISH_RES_PENDING:
    case U_REMOTE_ESTABLISHMENT:
      PFREE (ll_establish_req);
      TRACE_0_INFO( "LL_ESTABLISH_REQ ignored due to collision" );
      break;

    default:
      PFREE (ll_establish_req);
      TRACE_ERROR( "LL_ESTABLISH_REQ unexpected" );
      break;
  }

} /* u_ll_establish_req() */



/*
+------------------------------------------------------------------------------
| Function    : u_ll_establish_res
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_ESTABLISH_RES
|
| Parameters  : *ll_establish_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_ll_establish_res ( T_LL_ESTABLISH_RES *ll_establish_res )
{ 
  int bytelen;

  TRACE_FUNCTION( "ll_establish_res" );

  SWITCH_LLC (ll_establish_res->sapi);
  
  switch (GET_STATE(U))
  {
    case U_ESTABLISH_RES_PENDING:
    case U_REMOTE_ESTABLISHMENT:
      if (ll_establish_res->xid_valid == TRUE)
      {
        bytelen = BYTELEN( ll_establish_res->sdu.l_buf);
      
        TRACE_2_PARA("s:%d xid-len:%d", ll_establish_res->sapi, bytelen );

        bytelen = (bytelen < LLC_MAX_L3_XID_LEN ) ? bytelen : LLC_MAX_L3_XID_LEN;

        memcpy( llc_data->requested_l3_xid->value,
                &(ll_establish_res->sdu.buf[ll_establish_res->sdu.o_buf >> 3]),
                bytelen );

        llc_data->requested_l3_xid->length = bytelen;
        llc_data->requested_l3_xid->valid  = TRUE;
      }
      else
      {
        TRACE_1_PARA("s:%d", ll_establish_res->sapi );

        llc_data->requested_l3_xid->valid  = FALSE;
      }
 
      PFREE (ll_establish_res);

      if (GET_STATE(U) == U_REMOTE_ESTABLISHMENT) 
      {
        /*
         * Send UA response with F bit set to 1 and if valid XID
         */
        u_send_ua (1, TRUE);      
      }
      
      SET_STATE (U, U_ABM);
      
      sig_u_t200_stop_req();

      /*
       * <R.LLC.ABMEST_I.A.012>
       * vs. <R.LLC.ABMEST_I.A.015>
       */
      sig_u_llme_abmest_ind();
      break;

    default:
      TRACE_1_PARA("s:%d", ll_establish_res->sapi );
      PFREE (ll_establish_res);
      TRACE_ERROR( "LL_ESTABLISH_RES unexpected" );
      break;
  }

} /* u_ll_establish_res() */



/*
+------------------------------------------------------------------------------
| Function    : u_ll_release_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_RELEASE_REQ
|
| Parameters  : *ll_release_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_ll_release_req ( T_LL_RELEASE_REQ *ll_release_req )
{ 
  TRACE_FUNCTION( "ll_release_req" );

  SWITCH_LLC (ll_release_req->sapi);
  
  TRACE_1_PARA("s:%d", ll_release_req->sapi );

  /*
   * Set TLLI for current transaction.
   */
  llc_data->u->current_tlli = llc_data->tlli_new;
  
  switch (GET_STATE(U))
  {
    case U_TLLI_UNASSIGNED:
    case U_ADM:
      /*
       * We are already released. Send anytime a LL_RELEASE_CNF to 
       * SNDCP to make it more stable.
       */
      {
        PPASS (ll_release_req, ll_release_cnf, LL_RELEASE_CNF);
        TRACE_0_INFO("LL_RELEASE_REQ not necessary");
        TRACE_1_OUT_PARA("s:%d", ll_release_cnf->sapi);
        PSEND (hCommSNDCP, ll_release_cnf);
      }
      break;

    case U_ABM:
      sig_u_llme_abmrel_ind();
      /*
       * No break.
       */ 
    case U_ESTABLISH_RES_PENDING:
      if (ll_release_req->local EQ LL_REL_LOCAL)
      {
        /*
         * Send LL_RELEASE_CNF to SNDCP.
         */
        PPASS (ll_release_req, ll_release_cnf, LL_RELEASE_CNF);
        TRACE_1_OUT_PARA("s:%d", ll_release_cnf->sapi);
        PSEND (hCommSNDCP, ll_release_cnf);
        
        SET_STATE (U, U_ADM);

        sig_u_t200_stop_req();
      }
      else /* LL_REL_NOTLOCAL */
      {
        PFREE (ll_release_req);

        llc_data->u->retransmission_counter = 0;

        SET_STATE (U, U_LOCAL_RELEASE);

        /*
         * Send GRLC_DATA_REQ (DISC), start T200.
         */
        u_send_disc();
      }
      break;

    case U_REMOTE_ESTABLISHMENT:
      /*
       * We are currently waiting for a LL_ESTABLISH_RSP. But
       * because SNDCP want's us to release the connection, we
       * do so and switch back to ADM operation.
       */ 
      SET_STATE (U, U_ADM);

      if (ll_release_req->local NEQ LL_REL_LOCAL)
      {
        /*
         * Send DM response with F bit set to 1
         */
        llc_data->u->retransmission_counter = 0;

        u_send_dm (1);
      }

      sig_u_t200_stop_req();

      {
        PPASS (ll_release_req, ll_release_cnf, LL_RELEASE_CNF);
        TRACE_1_OUT_PARA("s:%d", ll_release_cnf->sapi);
        PSEND (hCommSNDCP, ll_release_cnf);
      }
      break;
     
    case U_LOCAL_ESTABLISHMENT:
      if (ll_release_req->local EQ LL_REL_LOCAL)
      {
        /*
         * Stop all running procedures. The peer will do the same.
         * Send the LL_RELEASE_CNF to SNDCP.
         */
        PPASS (ll_release_req, ll_release_cnf, LL_RELEASE_CNF);
        TRACE_1_OUT_PARA("s:%d", ll_release_cnf->sapi);
        PSEND (hCommSNDCP, ll_release_cnf);

        SET_STATE (U, U_ADM);

        /*
         * NOTE: The peer may have sent an UA or DM before being released
         *       locally itselve. If we get an UA in state ADM we have to
         *       send an status indication to GMM. This seems to be ok (ANS).
         *       If not, we have to wait until T200 timeout, but this is 40s
         *       on SAPI 11 per default. A unexpected DM response will be 
         *       ignored in state ADM and therefore no problem.
         */

        sig_u_t200_stop_req();
      }
      else /* LL_REL_NOTLOCAL */
      {
        /* 
         * We have just sent an SABM to our peer. The release request has to 
         * be stored and handled later:
         *  1. after timeout of T200 don't restart timer and send rel cnf
         *  2. after receive DM send rel cnf
         *  3. after receive of an UA send DISC and after get UA send rel cnf
         */
        llc_data->u->release_requested = TRUE;
        /*
         * Clear flag after sending an release confirmation or indication.
         */

        PFREE (ll_release_req);
        TRACE_0_INFO("Release request stored");
      }
      break;

    case U_LOCAL_RELEASE:        
      /* 
       * release will indicated later 
       */
      TRACE_0_INFO("LL_RELEASE_REQ already received");
      PFREE (ll_release_req);
      break;

    default:
      PFREE (ll_release_req);
      TRACE_ERROR( "LL_RELEASE_REQ unexpected" );
      break;
  }

} /* u_ll_release_req() */



/*
+------------------------------------------------------------------------------
| Function    : u_ll_xid_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_XID_REQ
|
| Parameters  : *ll_xid_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_ll_xid_req ( T_LL_XID_REQ *ll_xid_req )
{ 
  int bytelen;

  TRACE_FUNCTION( "u_ll_xid_req" );

  SWITCH_LLC (ll_xid_req->sapi);
  
  TRACE_2_PARA("s:%d xid-len:%d", ll_xid_req->sapi, BYTELEN(ll_xid_req->sdu.l_buf) );

  if (llc_data->u->ll_xid_resp_pending EQ TRUE)
  {
    PFREE (ll_xid_req);
    TRACE_0_INFO( "LL_XID_REQ ignored due to LL_XID_IND collision" );
    return;
  }

  switch (GET_STATE(U))
  {
    case U_ABM:
    case U_ADM:
      /*
       * Copy L3-XID parameter to requested_xid
       * and if currently no cmd frame outstanding, 
       * send XID command frame
       */
      bytelen = BYTELEN( ll_xid_req->sdu.l_buf);
      bytelen = (bytelen < LLC_MAX_L3_XID_LEN ) ? bytelen : LLC_MAX_L3_XID_LEN;

      memcpy( llc_data->requested_l3_xid->value,
              &(ll_xid_req->sdu.buf[ll_xid_req->sdu.o_buf >> 3]),
              bytelen );

      llc_data->requested_l3_xid->length = bytelen;
      llc_data->requested_l3_xid->valid  = TRUE;

      PFREE (ll_xid_req);

      if (llc_data->sapi->pbit_outstanding == FALSE)
      {
        /*
         * No frame with pbit set to 1 currently
         * outstanding, send XID comand
         */
        llc_data->u->retransmission_counter = 0;
        u_tag_xid_parameters(MS_COMMAND, FALSE);
        u_send_xid (MS_COMMAND);
      }
      /* SET_STATE (U, SAME_STATE); */
      break;

    case U_ESTABLISH_RES_PENDING:
    case U_REMOTE_ESTABLISHMENT:
      PFREE (ll_xid_req);
      TRACE_0_INFO( "LL_XID_REQ ignored due to collision" );
      break;

    default:
      /*
       * XID only supported in states ADM + ABM
       */
      PFREE (ll_xid_req);
      TRACE_ERROR( "LL_XID_REQ unexpected" );
      break;
  }

} /* u_ll_xid_req() */



/*
+------------------------------------------------------------------------------
| Function    : u_ll_xid_res
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_XID_RES
|
| Parameters  : *ll_xid_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_ll_xid_res ( T_LL_XID_RES *ll_xid_res )
{ 
  int bytelen;

  TRACE_FUNCTION( "ll_xid_res" );

  SWITCH_LLC (ll_xid_res->sapi);
  
  TRACE_2_PARA("s:%d xid-len:%d", ll_xid_res->sapi, BYTELEN(ll_xid_res->sdu.l_buf) );

  llc_data->u->ll_xid_resp_pending = FALSE;

  switch (GET_STATE(U))
  {
    case U_ABM:
    case U_ADM:
      /*
       * Copy L3-XID parameter to requested_xid
       * and send XID response frame
       */
      bytelen = BYTELEN( ll_xid_res->sdu.l_buf);
      bytelen = (bytelen < LLC_MAX_L3_XID_LEN ) ? bytelen : LLC_MAX_L3_XID_LEN;

      memcpy( llc_data->requested_l3_xid->value,
              &(ll_xid_res->sdu.buf[ll_xid_res->sdu.o_buf >> 3]),
              bytelen );

      llc_data->requested_l3_xid->length = bytelen;
      llc_data->requested_l3_xid->valid  = TRUE;

      PFREE (ll_xid_res);

      u_tag_xid_parameters(MS_RESPONSE, FALSE);
      u_send_xid (MS_RESPONSE);

      /* SET_STATE (U, SAME_STATE); */
      break;
    
    default:
      /*
       * XID only supported in states ADM + ABM
       */
      PFREE (ll_xid_res);
      TRACE_ERROR( "LL_XID_RES unexpected" );
      break;
  }

} /* u_ll_xid_res() */
