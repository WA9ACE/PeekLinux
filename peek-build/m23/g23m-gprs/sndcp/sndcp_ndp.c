/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_ndp.c
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
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (ND-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_sds.h"    /* to get signals to sd service */
#include "sndcp_sdas.h"   /* to get signals to sda service */
#include "sndcp_sdf.h"



/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : nd_dti_buffer_ready
+------------------------------------------------------------------------------
| Description : Is called from DTI callback function when reason was
|               DTI_REASON_TX_BUFFER_READY. Does the same as old
|               nd_sn_get(unit)data_req.
|
| Parameters  : affected nsapi
|
+------------------------------------------------------------------------------
*/
GLOBAL void nd_dti_buffer_ready (UBYTE nsapi)
{ 
  UBYTE sapi = 0;
  BOOL used = FALSE;

  TRACE_FUNCTION( "nd_dti_buffer_ready" );

  /*
   * set service instance according to nsapi in primitive
   */
  sndcp_data->nd = & sndcp_data->nd_base[nsapi];
  
  switch( GET_STATE( ND ) )
  {
    case ND_DEFAULT:
    case ND_RECOVER:
    case ND_SUSPEND:
      /*
       * Is NSAPI used at all?
       */
      sndcp_is_nsapi_used(nsapi, &used);
      if (used) {
        BOOL ack = FALSE;
        sndcp_get_nsapi_ack(nsapi, &ack);
        if (ack) {
          sndcp_get_nsapi_sapi(nsapi, &sapi);
          sig_nd_sda_getdata_req(sapi, nsapi);
        } else {
          sndcp_get_nsapi_sapi(nsapi, &sapi);
          sig_nd_sd_getunitdata_req(sapi, nsapi);
        }
	/* dti ready called from mg_sm_activate_ind again */
	TRACE_EVENT("FLOW CONTROL falg is set to false");
	 sndcp_data->tcp_flow = FALSE;
      }
      else
      {
        sndcp_data->nsapi_rcv_rdy_b4_used |= 0x001 << nsapi;
          TRACE_EVENT("FLOW CONTROL falg is set to true");
           /* We have received a flow control before receiving a snsm_activate_ind */
	    sndcp_data->tcp_flow = TRUE;
      }
      break;
    default:
      TRACE_ERROR( "Function nd_dti_buffer_ready unexpected" );
      break;
  }

} /* nd_buffer_ready() */
