/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_mgs.c
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
|             described in the SDL-documentation (MG-statemachine)
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/

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

#include "sndcp_nus.h"      /* to get the internal signals to service nu */
#include "sndcp_nds.h"      /* to get the internal signals to service nd */
#include "sndcp_sdas.h"      /* to get the internal signals to service sda */
#include "sndcp_sus.h"      /* to get the internal signals to service su */
#include "sndcp_suas.h"      /* to get the internal signals to service sua */
#include "sndcp_mgf.h"      /* to get the functions for mg service */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sig_sda_mg_re_est
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_SDA_MG_RE_EST
|
| Parameters  : UBYTE sapi: the affected SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_sda_mg_re_est (UBYTE sapi, UBYTE nsapi) 
{ 
  UBYTE sapi_index = 0;

  TRACE_ISIG( "sig_sda_mg_re_est" );

  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      {
        PALLOC_SDU(ll_establish_req, 
                   LL_ESTABLISH_REQ, 
                   SNDCP_XID_BLOCK_BIT_LEN);

        ll_establish_req->sapi = sapi;
        /*
         * Set the establishment states of sapi, nsapi.
         */
        sndcp_set_sapi_state(sapi, MG_EST);
        /*
         * Data transfer on SAPI is completely suspended
         */
        sig_mg_su_suspend(sapi);
        sig_mg_sua_suspend(sapi);
        mg_suspend_affected_nus(sapi);
        /*
         * Fill the XID block. Implementation dependent.
         */
        sndcp_get_sapi_index(sapi, &sapi_index);
        mg_set_xid_params(ll_establish_req->sapi, 
                          &ll_establish_req->sdu,
                          sndcp_data->mg.req_xid_block[sapi_index]);
        /*
         * Send prim to LLC.
         */
        sig_mg_sda_start_est(sapi);

        PSEND(hCommLLC, ll_establish_req);

      }

      break;
    default:
      TRACE_ERROR( "SIG_SDA_MG_RE_EST unexpected" );
      break;
  }
} /* sig_sda_mg_re_est() */


/*
+------------------------------------------------------------------------------
| Function    : sig_nd_mg_rec_no
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ND_MG_REC_NO
|
| Parameters  : nsapi, receive npdu number
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_nd_mg_rec_no (UBYTE nsapi, UBYTE npdu_number) 
{ 

  TRACE_ISIG( "sig_nd_mg_rec_no" );

  switch( GET_STATE( MG ) )
  {
    case MG_DEFAULT:
      TRACE_EVENT_P1( " SEQ_RES rec_no: %d", npdu_number);
      sig_mg_nd_resume(nsapi);
      sig_mg_nu_resume(nsapi);
      {
#ifndef SNDCP_UPM_INCLUDED
        PALLOC(snsm_sequence_res, SNSM_SEQUENCE_RES);
        snsm_sequence_res->nsapi = nsapi;
        snsm_sequence_res->rec_no = npdu_number;
        PSEND(hCommSM, snsm_sequence_res); 
#endif /* #ifdef SNDCP_UPM_INCLUDED */
       
      }
      break;
    default:
      TRACE_ERROR( "SIG_ND_MG_REC_NO unexpected" );
      break;
  }
} /* sig_nd_mg_rec_no() */



