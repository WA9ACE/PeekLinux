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
|             the SDL-documentation (ITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXP_C
#define LLC_ITXP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */
#include "llc_f.h"        /* to get the global entity definitions */
#include "llc_itxf.h"   /* to get local function definitions */
#include <string.h>     /* to get memcpy */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : itx_ll_desc_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_DATA_REQ
|
| Parameters  : *ll_desc_req/ll_data_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_ll_desc_req 
(
#ifdef LL_DESC
  T_LL_DESC_REQ *ll_desc_req
#else
  T_LL_DATA_REQ *ll_desc_req
#endif
)
{ 
  BOOL        success;

  TRACE_FUNCTION( "ll_desc_req" );

  SWITCH_LLC (ll_desc_req->sapi);
#ifdef REL99
  llc_data->cur_pfi        = ll_desc_req->pkt_flow_id; /* store current packet flow identifier */
#endif 
  TRACE_EVENT_P1("packet flow id = %d",ll_desc_req->pkt_flow_id);
  llc_data->cur_qos        = ll_desc_req->ll_qos;  /* store current QoS */
  TRACE_EVENT_P1("peak throughput = %d",ll_desc_req->ll_qos.peak);
  llc_data->cur_radio_prio = ll_desc_req->radio_prio;  
  TRACE_EVENT_P1("radio priority = %d",ll_desc_req->radio_prio);
  
#ifdef LL_DESC
  TRACE_2_PARA("s:%d len:%d", ll_desc_req->sapi, ll_desc_req->desc_list3.list_len);
#else
  TRACE_2_PARA("s:%d len:%d", ll_desc_req->sapi, BYTELEN(ll_desc_req->sdu.l_buf));
#endif
  /*
   * Set up the attached counter
   */
  ll_desc_req->attached_counter = CCI_NO_ATTACHE;

  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
    case ITX_TLLI_ASSIGNED:
      /*
       * Ignore primitive, but free prim data.
       */
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)ll_desc_req->desc_list3.first);
#endif /* LL_DESC */
          PFREE (ll_desc_req);
      TRACE_ERROR( "ll_desc_req unexpected" );
      break;

    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_i_queue_store (ll_desc_req, &success);
      if ( success == FALSE )
      {
        TRACE_0_INFO("ll_desc_req storing failed" );
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)ll_desc_req->desc_list3.first);
#endif /* LL_DESC */
          PFREE (ll_desc_req);
      }
      else
      {
        itx_send_next_frame (ABIT_NO_REQ);
        itx_handle_ll_ready_ind (FALSE);
      }
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)ll_desc_req->desc_list3.first);
#endif /* LL_DESC */
          PFREE (ll_desc_req);
      TRACE_ERROR( "ll_desc_req unexpected" );
      break;
  }

} /* itx_ll_desc_req() */

/*
+------------------------------------------------------------------------------
| Function    : itx_ll_data_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_DATA_REQ
|
| Parameters  : *ll_data_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
#ifdef LL_DESC
GLOBAL void itx_ll_data_req ( T_LL_DATA_REQ *ll_data_req )
{
   T_desc3 *desc3 = NULL;
   USHORT  len, desc_offset;
   UBYTE   *buf;

  TRACE_FUNCTION( "uitx_ll_data_req" );

  /*
   * Allocate primitive and fill in the structure elements
   */
  {
    PALLOC(ll_desc_req, LL_DESC_REQ);

    ll_desc_req->sapi             = ll_data_req->sapi;       
    ll_desc_req->tlli             = ll_data_req->tlli;       
    ll_desc_req->ll_qos           = ll_data_req->ll_qos;     
    ll_desc_req->radio_prio       = ll_data_req->radio_prio;
    ll_desc_req->reference1       = ll_data_req->reference1;
    ll_desc_req->seg_pos          = ll_data_req->seg_pos;
    ll_desc_req->attached_counter = ll_data_req->attached_counter;
#ifdef REL99 
    ll_desc_req->pkt_flow_id = ll_data_req->pkt_flow_id;
#endif /* REL99 */
    /*
     * Allocate memory block for desc2 and set the values of the 
     * desc2-elements.
     */

    len         = ll_data_req->sdu.l_buf >> 3;
    desc_offset = ll_data_req->sdu.o_buf >> 3;

    desc3  = llc_palloc_desc(len, desc_offset);
    
    buf = (UBYTE*)desc3->buffer; 
   
    memcpy(&buf[desc3->offset], 
           &ll_data_req->sdu.buf[desc_offset], 
           len);
    ll_desc_req->desc_list3.first    = (ULONG)desc3;
    ll_desc_req->desc_list3.list_len = desc3->len;

    /*
     * Primitive is handled in uitx_ll_unitdesc_req
     */
    itx_ll_desc_req (ll_desc_req);
  }
  PFREE(ll_data_req);


}
#endif /* LL_DESC */
