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
|             described in the SDL-documentation (FRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
/*lint -efile(766,gsm.h) */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
/*lint -efile(766,mon_ppp.h) */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_frxf.h"   /* to get function interface from frx */
#include "ppp_arbf.h"   /* to get function interface from arb */
#include "ppp_ptxs.h"   /* to get signal interface from ptx */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_frx_dead_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_FRX_DEAD_MODE_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_frx_dead_mode_req () 
{
  TRACE_ISIG( "sig_ptx_frx_dead_mode_req" );

  arb_discard_packet(ppp_data->frx.received_data);
  arb_discard_packet(ppp_data->frx.stored_packet);
  ppp_data->frx.received_data   = NULL;
  ppp_data->frx.stored_packet   = NULL;
  ppp_data->frx.store_state     = FRX_ADD_ERROR;
  ppp_data->frx.frame_complete  = FALSE;
  ppp_data->frx.data_flow_state = FRX_DATA_FLOW_DEAD;

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_READY:
    case FRX_READY_NDTI:
    case FRX_TRANSPARENT:
    case FRX_TRANSPARENT_NDTI:
      SET_STATE( PPP_SERVICE_FRX, FRX_DEAD );
      break;

    default:
      TRACE_ERROR( "SIG_PTX_FRX_DEAD_MODE_REQ unexpected" );
      break;
  }
} /* sig_ptx_frx_dead_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_frx_ready_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_FRX_READY_MODE_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_frx_ready_mode_req () 
{
  TRACE_ISIG( "sig_ptx_frx_ready_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_DEAD:
      SET_STATE( PPP_SERVICE_FRX, FRX_READY_NDTI );
      break;

    case FRX_DEAD_DTI:
      SET_STATE( PPP_SERVICE_FRX, FRX_READY );
      break;

    default:
      TRACE_ERROR( "SIG_PTX_FRX_READY_MODE_REQ unexpected" );
      break;
  }
} /* sig_ptx_frx_ready_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_frx_transparent_mode_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_FRX_TRANSPARENT_MODE_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_frx_transparent_mode_req () 
{
  TRACE_ISIG( "sig_ptx_frx_transparent_mode_req" );

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_DEAD:
      SET_STATE( PPP_SERVICE_FRX, FRX_TRANSPARENT_NDTI );
      break;

    case FRX_DEAD_DTI:
      SET_STATE( PPP_SERVICE_FRX, FRX_TRANSPARENT );
      break;

    default:
      TRACE_ERROR( "SIG_PTX_FRX_TRANSPARENT_MODE_REQ unexpected" );
      break;
  }
} /* sig_ptx_frx_transparent_mode_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_frx_start_flow_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_FRX_START_FLOW_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_frx_start_flow_req () 
{
  TRACE_ISIG( "sig_ptx_frx_start_flow_req" );
  /*
   * set FRX PTX data flow state to ready.
   */
  ppp_data->frx.data_flow_state = FRX_DATA_FLOW_READY;

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_READY:
      /*
       * Send if necessary residuary packets from frame received before
       */
      frx_send_pack_ready_mode ();
      /* 
       * check if all packet of frame were sent and start dti
       */
      if((ppp_data->frx.frame_complete  EQ FALSE )         AND
         (ppp_data->frx.data_flow_state EQ FRX_DATA_FLOW_READY))
      {
        /*
         * Send a start signal to DTI 
         */
        dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PEER_LAYER, PEER_CHANNEL);
      }
      break;

    case FRX_TRANSPARENT:
      /*
       * Send if necessary residuary packets from frame received before
       */
      frx_send_pack_transp_mode ();
      /* 
       * check if all packet of frame were sent and start dti 
       */
      if((ppp_data->frx.frame_complete  EQ FALSE)          AND
         (ppp_data->frx.data_flow_state EQ FRX_DATA_FLOW_READY))
      {
        /*
         * Send a start signal to DTI 
         */
        dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PEER_LAYER, PEER_CHANNEL);
      }
      break;
    case FRX_READY_NDTI:
    case FRX_TRANSPARENT_NDTI: 
      /*
       * silently ignore duplicate signal..
       */
      break;

    default:
      TRACE_ERROR( "SIG_PTX_FRX_START_FLOW_REQ unexpected" );
      break;
  }

} /* sig_ptx_frx_start_flow_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ptx_frx_stop_flow_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PTX_FRX_STOP_FLOW_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ptx_frx_stop_flow_req () 
{
  TRACE_ISIG( "sig_ptx_frx_stop_flow_req" );

  /*
   * set FRX-PTX data flow state to dead.
   */
  ppp_data->frx.data_flow_state = FRX_DATA_FLOW_DEAD;
  
  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_READY:
    case FRX_TRANSPARENT:
      /*
       * Send a stop signal to DTI 
       */
      dti_stop(ppp_data->ppphDTI, PPP_INSTANCE, PEER_LAYER, PEER_CHANNEL);
      break;

    case FRX_READY_NDTI:
    case FRX_TRANSPARENT_NDTI:
      /*
       * silently ignore duplicate signal..
       */
      break;
     default:
      TRACE_ERROR( "SIG_PTX_FRX_STOP_FLOW_REQ unexpected" );
      break;
  }

} /* sig_ptx_frx_stop_flow_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_arb_frx_dti_connected_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_FRX_DTI_CONNECTED_REQ
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_frx_dti_connected_req ()
{
  TRACE_ISIG( "sig_arb_frx_dti_connected_req" );

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_DEAD:
      SET_STATE( PPP_SERVICE_FRX, FRX_DEAD_DTI );
      break;

    case FRX_READY_NDTI:
      SET_STATE( PPP_SERVICE_FRX, FRX_READY );
      /*
       * Send a start signal to DTI 
       */
      dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PEER_LAYER, PEER_CHANNEL);
      break;

    case FRX_TRANSPARENT_NDTI: 
      SET_STATE( PPP_SERVICE_FRX, FRX_TRANSPARENT );
      /*
       * Send a start signal to DTI 
       */
      dti_start(ppp_data->ppphDTI, PPP_INSTANCE, PEER_LAYER, PEER_CHANNEL);
      break;

    default:
      TRACE_ERROR( "SIG_ARB_FRX_DTI_CONNECTED_REQ unexpected" );
      break;
  }
} /* sig_arb_frx_dti_connected_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_dti_frx_data_received_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_DTI_FRX_DATA_RECEIVED_IND
|
| Parameters  : dti_data_ind - Ptr. to primitive payload
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_frx_data_received_ind (T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION( "sig_dti_frx_data_received_ind" );

  PACCESS(dti_data_ind);

#ifdef PPP_HDLC_TRACE
  {
    T_desc2* trace_desc;
    
    trace_desc = (T_desc2*)dti_data_ind->desc_list2.first;
    while(trace_desc)
    {
      if(trace_desc->len)
      {
        TRACE_EVENT_P4("new desc=0x%08x: first=0x%02x last=0x%02x len=%d", 
                       trace_desc,
                       trace_desc->buffer[0], 
                       trace_desc->buffer[trace_desc->len - 1], 
                       trace_desc->len);
      }
      else
      {
        TRACE_EVENT_P1("new desc=0x%08x: len=0", trace_desc);
      }
      trace_desc = (T_desc2*)trace_desc->next;
    }
  }
#endif /* PPP_HDLC_TRACE */

  switch( GET_STATE( PPP_SERVICE_FRX ) )
  {
    case FRX_READY:
      ppp_data->frx.received_data = (T_desc2*)dti_data_ind->desc_list2.first;
      ppp_data->frx.proceed_data  = 0;

      /*
       * send as many packets to ptx as can be extracted
       * from the one just received
       */
      frx_add_desc();
      frx_send_pack_ready_mode();
      break;

    case FRX_TRANSPARENT:
      ppp_data->frx.received_data = (T_desc2*)dti_data_ind->desc_list2.first;
      ppp_data->frx.proceed_data  = 0;

      /*
       * send as many packets to ptx as can be extracted
       * from the one just received
       */
      frx_detect_frame();
      frx_send_pack_transp_mode();
      break;

    default:
      TRACE_ERROR( "DTI signal unexpected" );
      /*
       * free the data
       */
      arb_discard_packet((T_desc2*)dti_data_ind->desc_list2.first);
      break;
  }
  
  /*
   * free the primitive
   */
  PFREE(dti_data_ind);
} /* sig_dti_frx_data_received_ind() */



