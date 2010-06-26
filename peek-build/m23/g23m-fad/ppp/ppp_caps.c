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
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (CHAP-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef PPP_CHAPS_C
#define PPP_CHAPS_C
#endif /* !PPP_CAPS_C */

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

#include "ppp_capf.h"   /* to get function interface from chap */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_ftxs.h"   /* to get signal interface from ftx */
#include "ppp_arbs.h"   /* to get signal interface from arb */
#include "ppp_rts.h"    /* to get the rt signal interface */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_chap_packet_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_CHAP_PACKET_REQ
|
| Parameters  : packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_chap_packet_req (T_desc2* packet)
{

  T_desc2* temp_desc;
  UBYTE   forward;

  TRACE_ISIG( "sig_arb_chap_packet_req" );

  /*
   * depend on the code-field call appropriate function
   */
  switch(packet->buffer[0])
  {
    case CODE_CHALLENGE:
      chap_rc(&packet, &forward);
      break;
    case CODE_RESPONSE:
      chap_rr(&packet, &forward);
      break;
    case CODE_SUCCESS:
      chap_rs(packet, &forward);
      break;
    case CODE_FAILURE:
      chap_rf(packet, &forward);
      break;
    default:
      forward = FORWARD_DISCARD;
      break;
  }
  switch(forward)
  {
    case FORWARD_RC:  /* Received Challenge */
      /*
       * send Response packet and wait for an appropriate answer
       */
      SET_STATE( PPP_SERVICE_CHAP, CHAP_UP );
      ppp_data->chap.counter = ppp_data->mc;
      sig_any_ftx_packet_req(DTI_PID_CHAP, packet); /* Response */
      if(ppp_data->chap.counter > 0)
        ppp_data->chap.counter--;
      sig_any_rt_rrt_req();
      break;
    case FORWARD_RRP:  /* Received Response Positive */
      /*
       * send Success packets until receive a stop signal from arb
       * or the counter becomes zero
       */
      SET_STATE( PPP_SERVICE_CHAP, CHAP_UP );
      ppp_data->chap.counter = ppp_data->mc;
      sig_any_ftx_packet_req(DTI_PID_CHAP, packet); /* Success */
      if(ppp_data->chap.counter > 0)
        ppp_data->chap.counter--;
      sig_any_rt_rrt_req();
      sig_any_arb_tlu_ind();
      break;
    case FORWARD_RRN:  /* Received Response Negative  */
      /*
       * send Failure packet
       */
      sig_any_ftx_packet_req(DTI_PID_CHAP, packet); /* Failure */
      if(ppp_data->chap.fails > 0)
      {
        /*
         * if failure counter is not zero then create a new Challenge packet
         */
        ppp_data->chap.fails--;
        ppp_data->chap.counter = ppp_data->mc;
        chap_get_sc(&temp_desc);
        sig_any_ftx_packet_req(DTI_PID_CHAP, temp_desc); /* Challenge */
        if(ppp_data->chap.counter > 0)
          ppp_data->chap.counter--;
        sig_any_rt_rrt_req();
      }
      else
      {
        /*
         * if failure counter expired then set error code
         */
        sig_any_rt_srt_req();
        if(ppp_data->ppp_cause EQ 0)
          ppp_data->ppp_cause = PPP_TERM_USE_AUTHED_FAILED;
        sig_any_arb_tlf_ind();
      }
      break;
    case FORWARD_RS:  /* Received Success */
      sig_any_rt_srt_req();
      sig_any_arb_tlu_ind();
      break;
    case FORWARD_RF:  /* Received Failure */
      /*
       * restart timer and wait for new challenge packet
       */
      SET_STATE( PPP_SERVICE_CHAP, CHAP_DOWN );
      ppp_data->chap.counter = ppp_data->mc;
      if(ppp_data->chap.counter > 0)
        ppp_data->chap.counter--;
      sig_any_rt_rrt_req();
      break;
    default:
      arb_discard_packet(packet);
      break;
  }
} /* sig_arb_chap_packet_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_chap_open_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_CHAP_OPEN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_chap_open_req ()
{
  T_desc2* packet;

  TRACE_ISIG( "sig_arb_chap_open_req" );

  SET_STATE( PPP_SERVICE_CHAP, CHAP_DOWN );
  ppp_data->chap.sc      = FALSE;
  ppp_data->chap.counter = ppp_data->mc;
  ppp_data->chap.fails   = ppp_data->mf;

  if(ppp_data->mode EQ PPP_SERVER)
  {
    /*
     * create and send challenge packet
     */
    chap_get_sc(&packet);
    sig_any_ftx_packet_req(DTI_PID_CHAP, packet); /* Challenge */
  }

  /*
   * start timer (client and server)
   */
  if(ppp_data->chap.counter > 0)
    ppp_data->chap.counter--;
  sig_any_rt_rrt_req();
} /* sig_arb_chap_open_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_chap_down_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_CHAP_DOWN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_chap_down_req ()
{
  TRACE_ISIG( "sig_arb_chap_down_req" );

  sig_any_rt_srt_req();
  arb_discard_packet(ppp_data->chap.c_packet);
  ppp_data->chap.c_packet = NULL;
  arb_discard_packet(ppp_data->chap.r_packet);
  ppp_data->chap.r_packet = NULL;
} /* sig_arb_chap_down_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_chap_stop_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_CHAP_STOP_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_chap_stop_req ()
{
  TRACE_ISIG( "sig_arb_chap_stop_req" );

  sig_any_rt_srt_req();
} /* sig_arb_chap_down_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_chap_to_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_CHAP_TO_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_chap_to_req ()
{
  T_desc2* packet = NULL;

  TRACE_ISIG( "sig_arb_chap_to_req" );

  if (ppp_data->chap.counter EQ 0)
  {
    /*
     * authentication failed (client and server mode)
     */
    if((ppp_data->ppp_cause EQ 0) && (ppp_data->mode EQ PPP_CLIENT))
      ppp_data->ppp_cause = PPP_TERM_NO_RESPONSE;
    sig_any_arb_tlf_ind();
  }
  else
  {
    if(ppp_data->mode EQ PPP_SERVER)
    {
      switch( GET_STATE( PPP_SERVICE_CHAP ) )
      {
        case CHAP_UP:
          /*
           * create Success packet
           */
          chap_get_ss(&packet);
          break;
        case CHAP_DOWN:
          /*
           * create Challenge packet
           */
          chap_get_sc(&packet);
          break;
        default:
          TRACE_EVENT("WARNING: unexpected chap time out request in server mode");
          break;
      }
      /*
       * send packet
       */
      sig_any_ftx_packet_req(DTI_PID_CHAP, packet);
    }
    else if(ppp_data->mode EQ PPP_CLIENT)
    {
      switch( GET_STATE( PPP_SERVICE_CHAP ) )
      {
        case CHAP_UP:
          if(ppp_data->chap.sr)
          {
            /*
             * Retransmit Response packet.
             */
            packet = ppp_data->chap.c_packet;
            chap_get_sr(&packet);
            /*
             * Send Response packet.
             */
            sig_any_ftx_packet_req(DTI_PID_CHAP, packet);
          } else {
            TRACE_EVENT("WARNING: CHAP_UP state, but no Response transmitted");
          }
          break;
        case CHAP_DOWN:
          /*
           * Wait for CHAP Challenge,
           * GET_DATA_REQ will be sent after time out by FTX service.
           */
          break;
        default:
          TRACE_EVENT("WARNING: unexpected chap time out request in client mode");
          break;
      }
    } else {
      TRACE_EVENT_P1("ERROR: undefined PPP mode - chap_caps.c (%d)", __LINE__);
    }
    /*
     * restart timer
     */
    ppp_data->chap.counter--;
    sig_any_rt_rrt_req();
  }
} /* sig_arb_chap_to_req() */
