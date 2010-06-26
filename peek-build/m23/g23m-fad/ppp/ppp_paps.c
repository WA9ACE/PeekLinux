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
|             described in the SDL-documentation (PAP-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_PAPS_C
#define PPP_PAPS_C
#endif /* !PPP_PAPS_C */

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

#include "ppp_papf.h"   /* to get function interface from pap */
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
| Function    : sig_arb_pap_packet_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PAP_PACKET_REQ
|
| Parameters  : packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_pap_packet_req (T_desc2* packet)
{ 
  UBYTE   forward;

  TRACE_ISIG( "sig_arb_pap_packet_req" );
  
  /*
   * depend on the code-field call appropriate function
   */
  switch(packet->buffer[0])
  {
    case CODE_AUTH_REQ:
      pap_rar(&packet, &forward);
      break;
    case CODE_AUTH_ACK:
      pap_raa(packet, &forward);
      break;
    case CODE_AUTH_NAK:
      pap_ran(packet, &forward);
      break;
    default:
      forward = FORWARD_DISCARD;
      break;
  }
  switch(forward)
  {
    case FORWARD_RARP:
      /*
       * send Authentication Ack packets until receive a stop signal from arb
       * or the counter becomes zero
       */
      SET_STATE( PPP_SERVICE_PAP, PAP_UP );
      ppp_data->pap.counter = ppp_data->mc;
      sig_any_ftx_packet_req(DTI_PID_PAP, packet); /* saa */
      if(ppp_data->pap.counter > 0)
        ppp_data->pap.counter--;
      sig_any_rt_rrt_req();
      sig_any_arb_tlu_ind();
      break;
    case FORWARD_RARN:
      /*
       * just send Authentication NAK packet and 
       * wait for next Authentication Request
       * to avoid endless authentication timer will not restarted
       */
      sig_any_ftx_packet_req(DTI_PID_PAP, packet); /* san */
      break;
    case FORWARD_RAA:
      /*
       * Authentication successful
       */
      sig_any_rt_srt_req();
      sig_any_arb_tlu_ind();
      break;
    case FORWARD_RAN:
      /*
       * send new Authentication Request
       */
      ppp_data->pap.counter = ppp_data->mc;
      /*
       * set new identifier
       */
      ppp_data->pap.nari++;
      /*
       * create and send authenticate request packet
       */
      pap_get_sar(&packet);
      sig_any_ftx_packet_req(DTI_PID_PAP, packet);
      if(ppp_data->pap.counter > 0)
        ppp_data->pap.counter--;
      sig_any_rt_rrt_req();
      break;
    default:
      arb_discard_packet(packet);
      break;
  }
} /* sig_arb_pap_packet_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_pap_open_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PAP_OPEN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_pap_open_req () 
{
  T_desc2* packet;

  TRACE_ISIG( "sig_arb_pap_open_req" );

  SET_STATE( PPP_SERVICE_PAP, PAP_DOWN );
  ppp_data->pap.counter = ppp_data->mc;

  if(ppp_data->mode EQ PPP_CLIENT)
  {
    /*
     * set new identifier
     */
    ppp_data->pap.nari++;
    /*
     * create and send authenticate request packet
     */
    pap_get_sar(&packet);
    sig_any_ftx_packet_req(DTI_PID_PAP, packet);
  }

  if(ppp_data->pap.counter > 0)
    ppp_data->pap.counter--;
  sig_any_rt_rrt_req();
} /* sig_arb_pap_open_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_pap_down_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PAP_DOWN_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_pap_down_req () 
{ 
  TRACE_ISIG( "sig_arb_pap_down_req" );

  sig_any_rt_srt_req();
  arb_discard_packet(ppp_data->pap.ar_packet);
  ppp_data->pap.ar_packet = NULL;
} /* sig_arb_pap_down_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_pap_stop_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PAP_STOP_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_pap_stop_req () 
{ 
  TRACE_ISIG( "sig_arb_pap_stop_req" );
  
  sig_any_rt_srt_req();
} /* sig_arb_pap_stop_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_pap_to_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PAP_TO_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_pap_to_req () 
{
  T_desc2* packet;

  TRACE_ISIG( "sig_arb_pap_to_req" );
  
  if (ppp_data->pap.counter EQ 0)
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
    if(ppp_data->mode EQ PPP_CLIENT)
    {
      /*
       * send new authentication request packet
       */
      pap_get_sar(&packet);
      sig_any_ftx_packet_req(DTI_PID_PAP, packet);
    }
    else /* server mode */
    {
      switch( GET_STATE( PPP_SERVICE_PAP ) )
      {
        case PAP_UP:
          /*
           * send new authentication ack packet
           */
          pap_get_saa(&packet);
          sig_any_ftx_packet_req(DTI_PID_PAP, packet);
          break;
      }
    }
    /*
     * restart timer
     */
    ppp_data->pap.counter--;
    sig_any_rt_rrt_req();
  }
} /* sig_arb_pap_to_req() */
