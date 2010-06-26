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
|             described in the SDL-documentation (PTX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

/*
 * define for ptxs debug traces
 */
/* #define PTXS_DEBUG */

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
/*lint -efile(766,mon_ppp.h) */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_dti.h"    /* to get the local DTILIB definitions */
#include "ppp_frxs.h"   /* to get signal interface from frx */
#include "ppp_arbs.h"   /* to get signal interface from arb */

#include "ppp_arbf.h"   /* to get arb functions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ptx_dead_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PTX_DEAD_MODE_REQ. It sets
|               the frame-reception-packet-transission direction to DEAD mode.
|               That means these two services will not receive or send any more
|               data.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ptx_dead_mode_req ()
{
  TRACE_ISIG( "sig_arb_ptx_dead_mode_req" );

  ppp_data->ptx.ftx_buffer_state = PTX_FTX_BUFFER_FULL;
  ppp_data->ptx.ptx_buffer_state = PTX_BUFFER_FULL;

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_READY:
    case PTX_BLOCKED:
      SET_STATE( PPP_SERVICE_PTX, PTX_DEAD );
      sig_ptx_frx_stop_flow_req();
      break;

    default:
      TRACE_ERROR( "SIG_ARB_PTX_DEAD_MODE_REQ unexpected" );
      break;
  }
  
  /*
   * set FRX to dead mode
   */
  sig_ptx_frx_dead_mode_req();

} /* sig_arb_ptx_dead_mode_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ptx_ready_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PTX_READY_MODE_REQ. It sets
|               the frame-reception-packet-transission direction to READY mode.
|               That means these two services will receive and send data.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ptx_ready_mode_req ()
{
  TRACE_ISIG( "sig_arb_ptx_ready_mode_req" );

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_BLOCKED:
      SET_STATE( PPP_SERVICE_PTX, PTX_READY );
      /*
       * incoming data flow had not been stopped but redirected to
       * ARB, so normally don't need to start, but it is possible that the data flow  
       * has been stopped bacause of ftx_buffer_full_ind
       */
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY AND
         ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_READY        )
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(READY_MODE_REQ): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      /* 
       * data is to be sent directly to the peer, and the dtilib buffer is
       * full, so stop incoming data flow.
       */
      if(ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_FULL)
      {
        sig_ptx_frx_stop_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("INFO(READY_MODE_REQ): UL data flow wasn't stopped, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    case PTX_DEAD:
      /*
       * incoming data is to be redirected to ARB
       */
      SET_STATE( PPP_SERVICE_PTX, PTX_READY);
      sig_ptx_frx_ready_mode_req();
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY AND
         ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_READY        )
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(READY_MODE_REQ): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    default:
      TRACE_ERROR( "SIG_ARB_PTX_READY_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ptx_ready_mode_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ptx_blocked_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PTX_BLOCKED_MODE_REQ. It
|               sets the frame-reception-packet-transission direction to
|               BLOCKED mode. That means PPP receives frames, but it dos not
|               send packts.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ptx_blocked_mode_req ()
{
  TRACE_ISIG( "sig_arb_ptx_blocked_mode_req" );

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_DEAD:
      /* 
       * Start incoming data flow - data will be sent to ARB.
       */
      SET_STATE( PPP_SERVICE_PTX, PTX_BLOCKED);
      sig_ptx_frx_ready_mode_req();
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY)
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(BLOCKED_MODE_REQ): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    case PTX_READY:
      /*
       * Do not stop incoming data flow, just redirect it to ARB
       * Restart incoming data flow if it should be now redirected to ARB
       */
      SET_STATE( PPP_SERVICE_PTX, PTX_BLOCKED );
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY AND
         ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_FULL         )
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(BLOCKED_MODE_REQ): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    default:
      TRACE_ERROR( "SIG_ARB_PTX_BLOCKED_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ptx_blocked_mode_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ptx_transparent_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_PTX_TRANSPARENT_MODE_REQ.
|               It sets the frame-reception-packet-transission direction to
|               TRANSPARENT mode. That means PPP receives frames and sends
|               these frame transparent to the upper layer.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ptx_transparent_mode_req ()
{
  TRACE_ISIG( "sig_arb_ptx_transparent_mode_req" );

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_DEAD:
      /*
       * Set ptx to transparent mode and enable incoming data flow.
       * The dti connection has been newly opened so that its buffers
       * are empty and data can be received
       */
      SET_STATE( PPP_SERVICE_PTX, PTX_READY );
      sig_ptx_frx_transparent_mode_req();
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY AND
         ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_READY        )
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(TRANSP_MODE_REQ): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    default:
      TRACE_ERROR( "SIG_ARB_PTX_TRANSPARENT_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ptx_transparent_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_frx_ptx_packet_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_FRX_PTX_PACKET_IND. The service
|               FRX received a complete frame and forwards the encapsulated
|               packet to PTX.
|
| Parameters  : ptype      - type of packet
|               packet_len - length of packet
|               packet     - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_frx_ptx_packet_ind (USHORT ptype,
                                    USHORT packet_len,
                                    T_desc2* packet)
{
  TRACE_ISIG( "sig_frx_ptx_packet_ind" );

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_READY:
      switch(ptype)
      {
        case DTI_PID_CTCP:
        case DTI_PID_UTCP:
          /*
           * if Van Jacobson Header Compression is not negotiated such packets
           * will not be sent to upper layer
           */
          if(ppp_data->n_hc NEQ PPP_HC_VJ)
          {
            sig_ptx_arb_packet_ind(ptype, packet);
            if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_FULL)
            {
              sig_ptx_frx_stop_flow_req();
              #ifdef PTXS_DEBUG
              TRACE_EVENT_P2("INFO(PTX_PACKET_IND): UL data flow was stopped, %s(%d)", __FILE__, __LINE__);
              #endif /* PTXS_DEBUG */
            }
            break;
          }
          /* fall through */
        case DTI_PID_IP:
        case DTI_PID_UOS:
          /*
           * send packet to upper layer
           */
          {
            PALLOC_DESC2 (dti_data_ind, DTI2_DATA_IND);

            dti_data_ind->desc_list2.list_len = packet_len;
            dti_data_ind->desc_list2.first    = (ULONG)packet;

            /*
             *  Send the DTI data primitive
             */

            sig_any_dti_data_ind(ppp_data->ppphDTI,
                                 dti_data_ind,
                                 PPP_INSTANCE, PROT_LAYER,
                                 PROT_CHANNEL,(UBYTE)ptype);
          }
          break;

        default:
          /*
           * received packet is nether an IP packet nor
           * a PPP frame (transparent mode), so we forward it to ARB
           */
          sig_ptx_arb_packet_ind(ptype, packet);
          if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_FULL)
          {
            sig_ptx_frx_stop_flow_req();
            #ifdef PTXS_DEBUG
            TRACE_EVENT_P2("INFO(PTX_PACKET_IND): UL data flow was stopped, %s(%d)", __FILE__, __LINE__);
            #endif /* PTXS_DEBUG */
          }
          break;
      }
    break;

    case PTX_BLOCKED:
      /*
       * In BLOCKED mode we forward each packet to ARB
       */
      sig_ptx_arb_packet_ind(ptype, packet);
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_FULL)
      {
        sig_ptx_frx_stop_flow_req();
        #ifdef PTXS_DEBUG
        TRACE_EVENT_P2("INFO(PTX_PACKET_IND): UL data flow was stopped, %s(%d)", __FILE__, __LINE__);
        #endif /* PTXS_DEBUG */
      }
      break;

    default:
      TRACE_ERROR( "SIG_FRX_PTX_PACKET_IND unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_frx_ptx_packet_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_dti_ptx_tx_buffer_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the signal sig_dti_ptx_tx_buffer_ready_ind
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_ptx_tx_buffer_ready_ind ()
{
  TRACE_FUNCTION( "sig_dti_ptx_tx_buffer_ready_ind" );

  ppp_data->ptx.ptx_buffer_state = PTX_BUFFER_READY;

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_READY:
      if(ppp_data->ptx.ftx_buffer_state EQ PTX_FTX_BUFFER_READY)
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(BUFFER_READY_IND): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    case PTX_BLOCKED:
    case PTX_DEAD:
      /* wait for sig_arb_ptx_blocked/ready/transparent_mode_req */
      break;

    default:
      TRACE_ERROR( "DTI signal unexpected" );
      break;
  }
} /* sig_dti_ptx_tx_buffer_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_dti_ptx_tx_buffer_full_ind
+------------------------------------------------------------------------------
| Description : This function handle the signal sig_dti_ptx_tx_buffer_full_ind
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_ptx_tx_buffer_full_ind ()
{
  TRACE_FUNCTION ("sig_dti_ptx_tx_buffer_full_ind()");

  ppp_data->ptx.ptx_buffer_state = PTX_BUFFER_FULL;

  switch( GET_STATE( PPP_SERVICE_PTX ) )
    {
      case PTX_READY:
        sig_ptx_frx_stop_flow_req();
        break;

      case PTX_DEAD:
        break;

      case PTX_BLOCKED:
        /*
         * do not stop incoming data flow here since data is not sent
         * to the peer but to ARB, not having to be buffered in dtilib
         */
        break;

      default:
        TRACE_ERROR("Wrong state");
        break;
    }
} /* sig_dti_ptx_tx_buffer_full_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_ptx_tx_buffer_full_ind
+------------------------------------------------------------------------------
| Description : This function handle the signal sig_ftx_ptx_tx_buffer_full_ind
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_ptx_buffer_full_ind ()
{
  TRACE_FUNCTION ("sig_ftx_ptx_tx_buffer_full_ind()");

  ppp_data->ptx.ftx_buffer_state = PTX_FTX_BUFFER_FULL;

} /* sig_ftx_ptx_tx_buffer_full_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ftx_ptx_tx_buffer_ready_ind
+------------------------------------------------------------------------------
| Description : This function handle the signal sig_ftx_ptx_tx_buffer_ready_ind
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ftx_ptx_buffer_ready_ind ()
{
  TRACE_FUNCTION ("sig_ftx_ptx_tx_buffer_ready_ind()");

  ppp_data->ptx.ftx_buffer_state = PTX_FTX_BUFFER_READY;

  switch( GET_STATE( PPP_SERVICE_PTX ) )
  {
    case PTX_READY:
      if(ppp_data->ptx.ptx_buffer_state EQ PTX_BUFFER_READY)
      {
        sig_ptx_frx_start_flow_req();
      }
      #ifdef PTXS_DEBUG
      else
      {
        TRACE_EVENT_P2("WARNING(FTX_BUFFER_READY_IND): UL data flow wasn't started, %s(%d)", __FILE__, __LINE__);
      }
      #endif /* PTXS_DEBUG */
      break;

    case PTX_BLOCKED:
        /*
         * Start data flow also if PTX buffer is full,
         * because the data will be redirected to ARB.
         */
        sig_ptx_frx_start_flow_req();
      break;

    case PTX_DEAD:
      /* wait for sig_arb_ptx_blocked/ready/transparent_mode_req */
      break;

    default:
      TRACE_ERROR( "DTI signal unexpected" );
      break;
  }


} /* sig_ftx_ptx_tx_buffer_ready_ind() */


