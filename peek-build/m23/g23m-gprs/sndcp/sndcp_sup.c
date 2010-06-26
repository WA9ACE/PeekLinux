/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sup.c
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
|             the SDL-documentation (SU-statemachine)
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/


#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"

#include "sndcp.h"        /* to get the global entity definitions */
#include "sndcp_f.h"       /* to get the functions to access the global arrays*/

#include "sndcp_suf.h"     /* to get internal functions of service su */
#include "sndcp_nus.h"     /* to get signal functions to service nu */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/




#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : su_ll_unitready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITREADY_IND
|
| Parameters  : *ll_unitready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void su_ll_unitready_ind ( T_LL_UNITREADY_IND *ll_unitready_ind )
{ 
  U8 sapi_index = 0;


  TRACE_FUNCTION( "su_ll_unitready_ind" );
#ifdef FLOW_TRACE
  sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                           FLOW_TRACE_UP, 
                           FLOW_TRACE_BOTTOM, 
                           TRUE);
#endif
   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_unitready_ind->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      /*
       * Is queue with outgoing LL_UNITDESC_REQs empty?
       */
      if (sndcp_data->su->ll_unitdesc_q_read ==
          sndcp_data->su->ll_unitdesc_q_write) {
        if (sndcp_data->su->cia_state == CIA_IDLE) {
          su_next_sn_unitdata_req(ll_unitready_ind->sapi);
        }
      }
      SET_STATE(SU, SU_LLC_RECEPTIVE_SUSPEND);
      break;
    case SU_LLC_RECEPTIVE_SUSPEND:
      break;
    case SU_LLC_NOT_RECEPTIVE:
      /*
       * Is queue with outgoing LL_UNITDESC_REQs empty?
       */
      if (sndcp_data->su->ll_unitdesc_q_read ==
          sndcp_data->su->ll_unitdesc_q_write) {

        SET_STATE(SU, SU_LLC_RECEPTIVE);
        if (sndcp_data->su->cia_state == CIA_IDLE) {
          su_next_sn_unitdata_req(ll_unitready_ind->sapi);
        }

      } else { /* sndcp_data->su.ll_unitdesc_q is not empty */
        /*
         * Send next ll_unitdesc_req from queue.
         */
        U8 ll_unitdesc_seg_pos;
        U8 ll_unitdesc_nsapi;
        U8 ll_unitdesc_sapi;

        T_LL_UNITDESC_REQ* ll_unitdesc_req = 
          sndcp_data->su->ll_unitdesc_q[sndcp_data->su->ll_unitdesc_q_read];

        sndcp_data->su->ll_unitdesc_q[sndcp_data->su->ll_unitdesc_q_read] = 
          NULL;

        sndcp_data->su->ll_unitdesc_q_read = 
          (sndcp_data->su->ll_unitdesc_q_read + 1) % 
            SNDCP_SEGMENT_NUMBERS_UNACK;

#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
                       ll_unitdesc_req->desc_list3.list_len);

        sndcp_trace_desc_list3_content(ll_unitdesc_req->desc_list3);

#endif        
#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                                 FLOW_TRACE_UP, 
                                 FLOW_TRACE_BOTTOM, 
                                 FALSE);
#endif
        ll_unitdesc_seg_pos = ll_unitdesc_req->seg_pos;
        ll_unitdesc_nsapi   = su_get_nsapi(ll_unitdesc_req);
        ll_unitdesc_sapi    = ll_unitdesc_req->sapi;


#ifdef _SIMULATION_
        su_send_ll_unitdata_req_test(ll_unitdesc_req);
#else /* _SIMULATION_ */ 
        PSEND(hCommLLC, ll_unitdesc_req);
#endif /* _SIMULATION_ */

        if ((ll_unitdesc_seg_pos & SEG_POS_LAST) > 0) {
          sig_su_nu_ready_ind( ll_unitdesc_nsapi );
          su_next_sn_unitdata_req( ll_unitdesc_sapi );
          sndcp_data->su->cia_state = CIA_IDLE;
        }

      } /* sndcp_data->su.ll_unitdesc_q is not empty */
      break;
    case SU_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "LL_UNITREADY_IND unexpected" );
      break;
  }
  if (ll_unitready_ind != NULL) {
    PFREE(ll_unitready_ind);
  }

} /* su_ll_unitready_ind() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : su_ll_unitready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_UNITREADY_IND
|
| Parameters  : *ll_unitready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void su_ll_unitready_ind ( T_LL_UNITREADY_IND *ll_unitready_ind )
{ 
  UBYTE sapi_index = 0;


  TRACE_FUNCTION( "su_ll_unitready_ind" );
#ifdef FLOW_TRACE
  sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                           FLOW_TRACE_UP, 
                           FLOW_TRACE_BOTTOM, 
                           TRUE);
#endif
   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_unitready_ind->sapi, &sapi_index);
  sndcp_data->su = & sndcp_data->su_base[sapi_index];

  switch( GET_STATE( SU ) )
  {
    case SU_LLC_NOT_RECEPTIVE_SUSPEND:
      /*
       * Is queue with outgoing LL_UNITDATA_REQs empty?
       */
      if (sndcp_data->su->ll_unitdata_q_read ==
          sndcp_data->su->ll_unitdata_q_write) {
        if (sndcp_data->su->cia_state == CIA_IDLE) {
          su_next_sn_unitdata_req(ll_unitready_ind->sapi);
        }
      }
      SET_STATE(SU, SU_LLC_RECEPTIVE_SUSPEND);
      break;
    case SU_LLC_RECEPTIVE_SUSPEND:
      break;
    case SU_LLC_NOT_RECEPTIVE:
      /*
       * Is queue with outgoing LL_UNITDATA_REQs empty?
       */
      if (sndcp_data->su->ll_unitdata_q_read ==
          sndcp_data->su->ll_unitdata_q_write) {

        SET_STATE(SU, SU_LLC_RECEPTIVE);
        if (sndcp_data->su->cia_state == CIA_IDLE) {
          su_next_sn_unitdata_req(ll_unitready_ind->sapi);
        }

      } else { /* sndcp_data->su.ll_unitdata_q is not empty */
        /*
         * Send next ll_unitdata_req from queue.
         */
        UBYTE ll_unitdata_seg_pos;
        UBYTE ll_unitdata_nsapi;
        UBYTE ll_unitdata_sapi;

        T_LL_UNITDATA_REQ* ll_unitdata_req = 
          sndcp_data->su->ll_unitdata_q[sndcp_data->su->ll_unitdata_q_read];

        sndcp_data->su->ll_unitdata_q[sndcp_data->su->ll_unitdata_q_read] = 
          NULL;

        sndcp_data->su->ll_unitdata_q_read = 
          (sndcp_data->su->ll_unitdata_q_read + 1) % 
            SNDCP_SEGMENT_NUMBERS_UNACK;

#ifdef SNDCP_TRACE_ALL
        TRACE_EVENT("uplink with sn header at LL SAP: ");
        TRACE_EVENT_P1("%d octets",
                       (ll_unitdata_req->sdu.l_buf + 7) / 8);


        sndcp_trace_sdu(& ll_unitdata_req->sdu);
#endif        
#ifdef FLOW_TRACE
        sndcp_trace_flow_control(FLOW_TRACE_SNDCP, 
                                 FLOW_TRACE_UP, 
                                 FLOW_TRACE_BOTTOM, 
                                 FALSE);
#endif
        ll_unitdata_seg_pos = ll_unitdata_req->seg_pos;
        ll_unitdata_nsapi   = su_get_nsapi(ll_unitdata_req);
        ll_unitdata_sapi    = ll_unitdata_req->sapi;

        PSEND (hCommLLC, ll_unitdata_req);

        if ((ll_unitdata_seg_pos & SEG_POS_LAST) > 0) {
          sig_su_nu_ready_ind( ll_unitdata_nsapi );
          su_next_sn_unitdata_req( ll_unitdata_sapi );
          sndcp_data->su->cia_state = CIA_IDLE;
        }

      } /* sndcp_data->su.ll_unitdata_q is not empty */
      break;
    case SU_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "LL_UNITREADY_IND unexpected" );
      break;
  }
  if (ll_unitready_ind != NULL) {
    PFREE(ll_unitready_ind);
  }

} /* su_ll_unitready_ind() */
#endif /* _SNDCP_DTI_2_ */



