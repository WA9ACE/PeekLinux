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
|             described in the SDL-documentation (FTX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

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

#include "ppp_dti.h"    /* to get local DTI definitions */
#include "ppp_ftxf.h"   /* to get function interface from ftx */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_prxs.h"   /* to get signal interface from prx */
#include "ppp_ptxs.h"   /* to get signal interface from prx */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ftx_parameters_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FTX_PARAMETERS_REQ. It sets
|               the parameter of FTX service.
|
| Parameters  : accm - Async Control Character Map
|               pfc  - Protocol Field Compression
|               acfc - Address and Control Field Compression
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ftx_parameters_req (ULONG accm, UBYTE pfc, UBYTE acfc)
{
  UBYTE i;

  TRACE_ISIG( "sig_arb_ftx_parameters_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_READY:
    case FTX_BLOCKED_DTI_FULL:
    case FTX_READY_DTI_FULL:
    case FTX_BLOCKED:
    case FTX_DEAD:
    case FTX_DEAD_START_FLOW:
      /*
       * store accm in look up table
       */
      for(i=0; i < 0x20; i++)
      {
        ppp_data->ftx.accmtab[i] = (accm & (1UL << i)) ? 0xFF : 0x00;
      }
      ppp_data->ftx.pfc  = pfc;
      ppp_data->ftx.acfc = acfc;
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FTX_PARAMETERS_REQ unexpected" );
      break;
  }
} /* sig_arb_ftx_parameters_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ftx_dead_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FTX_DEAD_MODE_REQ. It stops
|               any data tranfer and releases all still to send HDLC frames.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ftx_dead_mode_req ()
{

  TRACE_ISIG( "sig_arb_ftx_dead_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_READY:
    case FTX_TRANSPARENT:
      sig_ftx_prx_stop_flow_req();
      SET_STATE( PPP_SERVICE_FTX, FTX_DEAD );
      break;

    case FTX_BLOCKED_DTI_FULL:
    case FTX_BLOCKED:
    case FTX_READY_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_DEAD );
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FTX_DEAD_MODE_REQ unexpected" );
      break;
  }
  /*
   * Free also any buffered packet (awaiting the TE to wake up)
   * and reset PRX
   */
  sig_ftx_prx_dead_mode_req();
} /* sig_arb_ftx_dead_mode_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ftx_blocked_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FTX_BLOCKED_MODE_REQ. That
|               means PRX-FTX data transfer is not allowed, but ARB can still
|               send packets.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ftx_blocked_mode_req ()
{
  TRACE_ISIG( "sig_arb_ftx_blocked_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_DEAD:
      /* 
       * Set FTX to blocked and do _not_ start data flow from prx
       * since ARB wants to perform some action first.
       * Mark FTX DTI queue as full until according tx_buffer_ready signal 
       * is received. This is necessary since FTX may be started without an
       * open dti channel towards peer layer.
       */
      SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED_DTI_FULL);
      sig_ftx_prx_ready_mode_req();
      break;

    case FTX_DEAD_START_FLOW:
      /* 
       * Set FTX to blocked and do _not_ start data flow from prx
       * since ARB wants to perform some action first.
       * tx_buffer_ready was already received, so do not mark DTI as full
       */
      SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED);
      sig_ftx_prx_ready_mode_req();
      break;

    case FTX_READY_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED_DTI_FULL );
      break;

    case FTX_READY:
      SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED );
      sig_ftx_prx_stop_flow_req();
      break;

    case FTX_TRANSPARENT:
      SET_STATE( PPP_SERVICE_FTX, FTX_TRANSPARENT_DTI_FULL );
      sig_ftx_prx_stop_flow_req();
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FTX_BLOCKED_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ftx_blocked_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ftx_ready_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FTX_READY_MODE_REQ. It
|               starts PRX-FTX data transfer.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ftx_ready_mode_req ()
{
  TRACE_ISIG( "sig_arb_ftx_ready_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    /*
     * Tranistion from FTX_DEAD means there is a newly opened dti connection
     * with empty buffers so starting the data flow is the right thing to
     * do here
     */
    case FTX_DEAD_START_FLOW:
      SET_STATE( PPP_SERVICE_FTX, FTX_READY );
      sig_ftx_prx_ready_mode_req();
      sig_ftx_prx_start_flow_req();
      sig_ftx_ptx_buffer_ready_ind();
      break;

    case FTX_DEAD:
      SET_STATE( PPP_SERVICE_FTX, FTX_READY_DTI_FULL );
      sig_ftx_prx_ready_mode_req();
      break;

    case FTX_BLOCKED:
      SET_STATE( PPP_SERVICE_FTX, FTX_READY );
      sig_ftx_prx_start_flow_req();
      break;

    case FTX_TRANSPARENT_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_TRANSPARENT );
      sig_ftx_prx_start_flow_req();
      break;

    case FTX_BLOCKED_DTI_FULL:
      /* now waiting for dti */
      SET_STATE( PPP_SERVICE_FTX, FTX_READY_DTI_FULL);
      break;

    case FTX_READY:
    case FTX_TRANSPARENT:      
      /* already in the requested state */
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FTX_READY_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ftx_ready_mode_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_arb_ftx_transparent_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FTX_TRANSPARENT_MODE_REQ.
|               It starts PRX-FTX transparent data transfer. That means we
|               expect PPP frames from PRX and check just for the start and end
|               HDLC flag.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_ftx_transparent_mode_req ()
{
  TRACE_ISIG( "sig_arb_ftx_transparent_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_DEAD:
    case FTX_DEAD_START_FLOW:
      /*
       * Set ftx to transparent mode and enable incoming data flow.
       * The dti connection has been newly opened so that its buffers
       * are empty and data can be received
       */
      SET_STATE( PPP_SERVICE_FTX, FTX_TRANSPARENT );
      sig_ftx_prx_ready_mode_req();
      sig_ftx_prx_start_flow_req();
      sig_ftx_ptx_buffer_ready_ind();
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FTX_TRANSPARENT_MODE_REQ unexpected" );
      break;
  }
} /* sig_arb_ftx_transparent_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_any_ftx_packet_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_FTX_PACKET_REQ. It creates
|               an HDLC frame and sends it if possible or stores it in the send
|               queue.
|
| Parameters  : ptype  - type of packet
|               packet - pointer to a generic data descriptor
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_ftx_packet_req (USHORT ptype, T_desc2* packet)
{
  TRACE_ISIG( "sig_any_ftx_packet_req" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_READY:
    case FTX_BLOCKED_DTI_FULL:
    case FTX_BLOCKED:
    case FTX_READY_DTI_FULL:
    {
      /*
       * create frame and send it
       */
      PALLOC_DESC2 (dti_data_ind, DTI2_DATA_IND);

      ftx_get_frame (ptype, packet, &dti_data_ind->desc_list2);

      sig_any_dti_data_ind(ppp_data->ppphDTI,
                           dti_data_ind,
                           PPP_INSTANCE, PEER_LAYER,
                           PEER_CHANNEL, DTI_PID_UOS);
      break;
    }
      
    case FTX_TRANSPARENT:
      /*
       * check frame and send it
       */
      {
        PALLOC_DESC2 (dti_data_ind, DTI2_DATA_IND);
        dti_data_ind->desc_list2.first = (ULONG)packet;
        ftx_check_frame (&dti_data_ind->desc_list2);

        sig_any_dti_data_ind(ppp_data->ppphDTI,
                             dti_data_ind,
                             PPP_INSTANCE, PEER_LAYER,
                             PEER_CHANNEL, DTI_PID_UOS);
      }
      break;

    default:
      TRACE_ERROR( "SIG_ANY_FTX_PACKET_REQ unexpected" );
      arb_discard_packet(packet);
      break;
  }
} /* sig_any_ftx_packet_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_dti_ftx_tx_buffer_full_ind
+------------------------------------------------------------------------------
| Description : This function handle the signal sig_dti_ftx_tx_buffer_full_ind
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_ftx_tx_buffer_full_ind ()
{
  TRACE_FUNCTION ("sig_dti_ftx_tx_buffer_full_ind()");

  switch( GET_STATE( PPP_SERVICE_FTX ) )
    {
      case FTX_READY:
        SET_STATE( PPP_SERVICE_FTX, FTX_READY_DTI_FULL );
        sig_ftx_prx_stop_flow_req();
        sig_ftx_ptx_buffer_full_ind();
        break;

      case FTX_TRANSPARENT:
        SET_STATE( PPP_SERVICE_FTX, FTX_TRANSPARENT_DTI_FULL );
        sig_ftx_prx_stop_flow_req();
        sig_ftx_ptx_buffer_full_ind();
        break;

      case FTX_BLOCKED:
        /* now both ARB and dtilib want FTX to block */
        SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED_DTI_FULL );
        sig_ftx_ptx_buffer_full_ind();
        break;

      case FTX_BLOCKED_DTI_FULL:
      case FTX_READY_DTI_FULL:
      case FTX_TRANSPARENT_DTI_FULL:
        /* prx has already been blocked, do nothing */
        break;

      case FTX_DEAD_START_FLOW:
        SET_STATE( PPP_SERVICE_FTX, FTX_DEAD );
        break;

      default:
        TRACE_ERROR("Wrong state");
      break;
    }
} /* sig_dti_ftx_tx_buffer_full_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_dti_ftx_tx_buffer_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the signal sig_dti_ftx_tx_buffer_ready_ind
|
| Parameters  : no parameter
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_ftx_tx_buffer_ready_ind ()
{
  TRACE_FUNCTION( "sig_dti_ftx_tx_buffer_ready_ind" );

  switch( GET_STATE( PPP_SERVICE_FTX ) )
  {
    case FTX_BLOCKED_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_BLOCKED );
      sig_ftx_ptx_buffer_ready_ind();
      break;

    case FTX_READY_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_READY );
      sig_ftx_prx_start_flow_req();
      sig_ftx_ptx_buffer_ready_ind();
      break;

    case FTX_TRANSPARENT_DTI_FULL:
      SET_STATE( PPP_SERVICE_FTX, FTX_TRANSPARENT );
      sig_ftx_prx_start_flow_req();
      sig_ftx_ptx_buffer_ready_ind();
      break;

    case FTX_DEAD:
      SET_STATE( PPP_SERVICE_FTX, FTX_DEAD_START_FLOW );
      /* wait for sig_arb_ftx_blocked/ready/transparent_mode_req */
      break;

    case FTX_READY:
    case FTX_TRANSPARENT:
      /* already in the requested state */
      break;

    default:
      TRACE_ERROR( "Signal unexpected" );
      break;
  }
} /* sig_dti_ftx_tx_buffer_ready_ind() */
