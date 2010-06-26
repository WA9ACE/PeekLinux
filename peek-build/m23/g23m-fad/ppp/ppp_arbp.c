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
|  Purpose :  This modul is part of the entity PPP and implements all
|             functions to handles the incoming primitives as described in
|             the SDL-documentation (ARB-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef PPP_ARBP_C
#define PPP_ARBP_C
#endif /* !PPP_ARBP_C */

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_ppp.h"    /* to get cnf-definitions */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_rts.h"    /* to get signal interface from rt */
#include "ppp_ptxs.h"   /* to get signal interface from ptx */
#include "ppp_ftxs.h"   /* to get signal interface from ftx */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_lcps.h"   /* to get signal interface from lcp */
#include "ppp_ncps.h"   /* to get signal interface from ncp */
#include "ppp_paps.h"   /* to get signal interface from pap */
#include "ppp_caps.h"   /* to get signal interface from chap */

#include <string.h>     /* to get strcmp */
#include <stdio.h>      /* to get sprintf */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : arb_ppp_establish_req
+------------------------------------------------------------------------------
| Description : Handles the primitive PPP_ESTABLISH_REQ
|
| Parameters  : *ppp_establish_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_ppp_establish_req ( T_PPP_ESTABLISH_REQ *ppp_establish_req )
{
  UBYTE open_channel=0;
  TRACE_FUNCTION( "arb_ppp_establish_req" );

  /*
   * Open the DTI channels. Poll - because the VSI channel
   * may not be ready at once.
   */
  switch( GET_STATE( PPP_SERVICE_ARB ) )
    {
      case ARB_DEAD:
        ppp_data->ppp_cause = 0;
        ppp_data->mode      = ppp_establish_req->mode;
        ppp_data->mru       = ppp_establish_req->mru;

        switch(ppp_data->mode)
        {
          case PPP_TRANSPARENT:
            break;

          case PPP_SERVER:
            ppp_data->mc=ppp_establish_req->mc;
            ppp_data->mt=ppp_establish_req->mt;
            ppp_data->mf=ppp_establish_req->mf;
            sig_arb_rt_parameters_req(ppp_establish_req->rt);
            sig_arb_lcp_parameters_req(ppp_establish_req->mru,
                                       ppp_establish_req->ap,
                                       ppp_establish_req->accm);
            sig_arb_ftx_blocked_mode_req();
            sig_arb_ptx_blocked_mode_req();
            break;

          case PPP_CLIENT:
            ppp_data->mc=ppp_establish_req->mc;
            ppp_data->mt=ppp_establish_req->mt;
            ppp_data->mf=ppp_establish_req->mf;
            sig_arb_rt_parameters_req(ppp_establish_req->rt);
            sig_arb_lcp_parameters_req(ppp_establish_req->mru,
                                       ppp_establish_req->ap,
                                       ppp_establish_req->accm);
            /* login information for authentication for PAP or CHAP */
            ppp_data->login = ppp_establish_req->login;
            sig_arb_ncp_parameters_req(ppp_establish_req->ppp_hc,
                                       PPP_MSID_DEFAULT,
                                       ppp_establish_req->ip,
                                       ppp_establish_req->dns1,
                                       ppp_establish_req->dns2,
                                       PPP_GATEWAY_DEFAULT);
            sig_arb_ftx_blocked_mode_req();
            sig_arb_ptx_blocked_mode_req();
            break;

          default:
            TRACE_ERROR( "PPP_ESTABLISH_REQ with invalid parameter: mode" );
            break;
        }
        break;
      default:
        TRACE_ERROR( "PPP_ESTABLISH_REQ unexpected" );
        break;
  } /* switch */

  while ((dti_open(
    ppp_data->ppphDTI,
    PPP_INSTANCE,
    PROT_LAYER,
    PROT_CHANNEL,
    PTX_QUEUE_SIZE,
    ppp_establish_req->prot_direction,
    DTI_QUEUE_WATERMARK,
    DTI_VERSION_10,
    ppp_establish_req->protocol_channel.protocol_entity,
    ppp_establish_req->prot_link_id
    ) EQ FALSE) && (open_channel < MAX_TRIES_OPEN_VSI_CHAN))
    {
      TRACE_EVENT("Problem to connect VSI channel to prot layer");
      open_channel++;
    }

  if(open_channel < MAX_TRIES_OPEN_VSI_CHAN)
  {
    open_channel=0;

    while ((dti_open(
      ppp_data->ppphDTI,
      PPP_INSTANCE,
      PEER_LAYER,
      PEER_CHANNEL,
      FTX_QUEUE_SIZE,
      ppp_establish_req->peer_direction,
      DTI_QUEUE_WATERMARK,
      DTI_VERSION_10,
      ppp_establish_req->peer_channel.peer_entity,
      ppp_establish_req->peer_link_id
      ) EQ FALSE) && (open_channel < MAX_TRIES_OPEN_VSI_CHAN))
    {
      TRACE_EVENT("Problem to connect VSI channel to peer layer");
      open_channel++;
    }
  }

  /*
   * Do nothing if VSI communication channels not opened
   */

  if(open_channel >= MAX_TRIES_OPEN_VSI_CHAN)
   {
    PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);
    SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
    sig_arb_ncp_down_req();
    sig_arb_pap_down_req();
    sig_arb_chap_down_req();
    sig_arb_lcp_down_req();
    sig_arb_ptx_dead_mode_req();
    sig_arb_ftx_dead_mode_req();
    arb_dti_close_prot_ind();
    arb_dti_close_peer_ind();

    /*
     * set error code
     */
    ppp_data->ppp_cause          = PPP_TERM_NO_CHANNEL;
    ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
    PSEND (hCommMMI, ppp_terminate_ind);
  }
  PFREE(ppp_establish_req);
} /* arb_ppp_establish_req() */

/*
+------------------------------------------------------------------------------
| Function    : arb_ppp_terminate_req
+------------------------------------------------------------------------------
| Description : Handles the primitive PPP_TERMINATE_REQ
|
| Parameters  : *ppp_terminate_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_ppp_terminate_req ( T_PPP_TERMINATE_REQ *ppp_terminate_req )
{
  TRACE_FUNCTION( "arb_ppp_terminate_req" );

  TRACE_EVENT( "ACI requested Termination" );

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_PAP_ESTABLISH:
    case ARB_PAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_pap_down_req();
      break;
    case ARB_CHAP_ESTABLISH:
    case ARB_CHAP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_arb_chap_down_req();
      break;
    case ARB_PDP_WAIT:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
      sig_any_rt_srt_req();
      break;
  }

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_IP:
      sig_arb_ftx_blocked_mode_req();
      sig_arb_ptx_blocked_mode_req();
    case ARB_NCP_ESTABLISH:
    case ARB_NCP_RECONF:
    case ARB_NCP_MOD:
      sig_arb_ncp_down_req();
    case ARB_PDP_SENT:
    case ARB_DEAD:
    case ARB_LCP_ESTABLISH:
    case ARB_LCP_PDP:
    case ARB_LCP_NCP:
    case ARB_LCP_RECONF:
    case ARB_LCP_MOD:
      if(ppp_data->arb.last_ipcp)
      {
        MFREE_DESC2(ppp_data->arb.last_ipcp);
        ppp_data->arb.last_ipcp = NULL;
      }
      if(ppp_terminate_req->lower_layer EQ PPP_LOWER_LAYER_UP)
      {
        /*
         * inform PPP-peer about termination
         */
        SET_STATE( PPP_SERVICE_ARB, ARB_LCP_RECONF );
        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_OK_MMI;
        sig_arb_lcp_close_req();
        /* xxxxx close dti?! */
      }
      else
      {
        /*
         * free all PPP resources and finish
         */
        PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);
        SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
        sig_arb_ncp_down_req();
        sig_arb_pap_down_req();
        sig_arb_chap_down_req();
        sig_arb_lcp_down_req();
        sig_arb_ptx_dead_mode_req();
        sig_arb_ftx_dead_mode_req();
        arb_dti_close_prot_ind();
        arb_dti_close_peer_ind();

        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_OK_MMI;
        ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
        PSEND (hCommMMI, ppp_terminate_ind);
      } /* end else */
      break;
    case ARB_TRANSPARENT:
      {
        PALLOC (ppp_terminate_ind, PPP_TERMINATE_IND);
        SET_STATE( PPP_SERVICE_ARB, ARB_DEAD );
        sig_arb_ncp_down_req();
        sig_arb_pap_down_req();
        sig_arb_chap_down_req();
        sig_arb_lcp_down_req();
        sig_arb_ptx_dead_mode_req();
        sig_arb_ftx_dead_mode_req();

        arb_dti_close_prot_ind();
        arb_dti_close_peer_ind();

        /*
         * set error code
         */
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_OK_MMI;
        ppp_terminate_ind->ppp_cause = ppp_data->ppp_cause;
        PSEND (hCommMMI, ppp_terminate_ind);
      }
      break;
    default:
      TRACE_ERROR( "PPP_TERMINATE_REQ unexpected" );
      break;
  }
  PFREE(ppp_terminate_req);

} /* arb_ppp_terminate_req() */



/*
+------------------------------------------------------------------------------
| Function    : arb_ppp_pdp_activate_res
+------------------------------------------------------------------------------
| Description : Handles the primitive PPP_PDP_ACTIVATE_RES
|
| Parameters  : *ppp_pdp_activate_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_ppp_pdp_activate_res ( T_PPP_PDP_ACTIVATE_RES *ppp_pdp_activate_res )
{
  ULONG    dns1, dns2, gateway;
  T_desc2* temp_desc;

  TRACE_FUNCTION( "arb_ppp_pdp_activate_res" );
  /*
   * extract DNS addresses
   */
  arb_analyze_pco_list(&ppp_pdp_activate_res->sdu, &dns1, &dns2, &gateway);

  TRACE_EVENT("PDP context activated");
  TRACE_EVENT_P5("IP: %d.%d.%d.%d (%08x)",
                  ((ppp_pdp_activate_res->ip >> 24) & 0x000000ff),
                  ((ppp_pdp_activate_res->ip >> 16) & 0x000000ff),
                  ((ppp_pdp_activate_res->ip >>  8) & 0x000000ff),
                  ( ppp_pdp_activate_res->ip        & 0x000000ff),
                  ppp_pdp_activate_res->ip);
  if(dns1 NEQ 0)
  {
    TRACE_EVENT_P5("primary DNS: %d.%d.%d.%d (%08x)",
                    ((dns1 >> 24) & 0x000000ff),
                    ((dns1 >> 16) & 0x000000ff),
                    ((dns1 >>  8) & 0x000000ff),
                    ( dns1        & 0x000000ff),
                    dns1);
  }
  else
  {
    TRACE_EVENT("no primary DNS");
  }
  if(dns2 NEQ 0)
  {
    TRACE_EVENT_P5("secondary DNS: %d.%d.%d.%d (%08x)",
                    ((dns2 >> 24) & 0x000000ff),
                    ((dns2 >> 16) & 0x000000ff),
                    ((dns2 >>  8) & 0x000000ff),
                    ( dns2        & 0x000000ff),
                    dns2);
  }
  else
  {
    TRACE_EVENT("no secondary DNS");
  }
  if(gateway NEQ 0)
  {
    TRACE_EVENT_P5("Gateway: %d.%d.%d.%d (%08x)",
                    ((gateway >> 24) & 0x000000ff),
                    ((gateway >> 16) & 0x000000ff),
                    ((gateway >>  8) & 0x000000ff),
                    ( gateway        & 0x000000ff),
                    gateway);
  }
  else
  {
    TRACE_EVENT("no Gateway");
  }
  if(ppp_pdp_activate_res->ppp_hc EQ PPP_HC_OFF)
  {
    TRACE_EVENT("do not use header compression");
  }
  else if(ppp_pdp_activate_res->ppp_hc EQ PPP_HC_VJ)
  {
    TRACE_EVENT("using VJ header compression");
  }
  /*
   * set ncp parameters
   */
  sig_arb_ncp_parameters_req(ppp_pdp_activate_res->ppp_hc,
                             ppp_pdp_activate_res->msid,
                             ppp_pdp_activate_res->ip,
                             dns1,
                             dns2,
                             gateway);

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_PDP:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_NCP );
      break;
    case ARB_PDP_SENT:
      SET_STATE( PPP_SERVICE_ARB, ARB_NCP_ESTABLISH );
      sig_arb_ncp_open_req();
      if(ppp_data->arb.last_ipcp)
      {
        temp_desc               = ppp_data->arb.last_ipcp;
        ppp_data->arb.last_ipcp = NULL;
        sig_arb_ncp_packet_req(temp_desc);
      }
      break;
    default:
      TRACE_ERROR( "PPP_PDP_ACTIVATE_RES unexpected" );
      break;
  }
  PFREE(ppp_pdp_activate_res);

} /* arb_ppp_pdp_activate_res() */



/*
+------------------------------------------------------------------------------
| Function    : arb_ppp_pdp_activate_rej
+------------------------------------------------------------------------------
| Description : Handles the primitive PPP_PDP_ACTIVATE_REJ
|
| Parameters  : *ppp_pdp_activate_rej - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_ppp_pdp_activate_rej ( T_PPP_PDP_ACTIVATE_REJ *ppp_pdp_activate_rej )
{
  TRACE_FUNCTION( "arb_ppp_pdp_activate_rej" );

  TRACE_EVENT_P1("PDP context activation rejected by the network (cause=%x)",
                 ppp_pdp_activate_rej->ppp_cause);
  /*
   * set error code
   */
  if(ppp_data->ppp_cause EQ 0)
    ppp_data->ppp_cause = ppp_pdp_activate_rej->ppp_cause;

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_PDP_SENT:
    case ARB_LCP_PDP:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_ESTABLISH );
      if(ppp_data->arb.last_ipcp)
      {
        MFREE_DESC2(ppp_data->arb.last_ipcp);
        ppp_data->arb.last_ipcp = NULL;
      }
      sig_arb_lcp_close_req();
      break;
    default:
      TRACE_ERROR( "PPP_PDP_ACTIVATE_REJ unexpected" );
      break;
  }
  PFREE(ppp_pdp_activate_rej);

} /* arb_ppp_pdp_activate_rej() */



/*
+------------------------------------------------------------------------------
| Function    : arb_ppp_modification_req
+------------------------------------------------------------------------------
| Description : Handles the primitive PPP_MODIFICATION_REQ
|
| Parameters  : *ppp_modification_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void arb_ppp_modification_req ( T_PPP_MODIFICATION_REQ *ppp_modification_req )
{
  TRACE_FUNCTION( "arb_ppp_modification_req" );

  sig_arb_ncp_modify_hc_req(ppp_modification_req->ppp_hc,
                            ppp_modification_req->msid);

  switch( GET_STATE( PPP_SERVICE_ARB ) )
  {
    case ARB_LCP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_LCP_MOD );
    case ARB_LCP_MOD:
      break;
    case ARB_IP:
      sig_arb_ftx_blocked_mode_req();
      sig_arb_ptx_blocked_mode_req();
    case ARB_NCP_RECONF:
      SET_STATE( PPP_SERVICE_ARB, ARB_NCP_MOD );
    case ARB_NCP_MOD:
      sig_arb_ncp_open_req();
      break;
    default:
      TRACE_ERROR( "PPP_MODIFICATION_REQ unexpected" );
      break;
  }
  PFREE(ppp_modification_req);

} /* arb_ppp_modification_req() */
