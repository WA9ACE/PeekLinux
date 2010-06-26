/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_suap.c
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
|             the SDL-documentation (SUA-statemachine)
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

#include "sndcp_suaf.h"     /* to get internal functions of service sua */
#include "sndcp_nus.h"     /* to get signal functions to service nu */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sua_ll_data_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_DATA_CNF
|
| Parameters  : *ll_data_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_ll_data_cnf ( T_LL_DATA_CNF *ll_data_cnf )
{ 
  BOOL complete = FALSE;
  BOOL found = FALSE;
  T_SEG_INFO* help = NULL;
  T_SEG_INFO* last_help = NULL;
  UBYTE sapi_index = 0;
  UBYTE refi = 0;

  TRACE_FUNCTION( "sua_ll_data_cnf" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_data_cnf->sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  TRACE_EVENT_P1("ll_data_cnf->c_reference1: %d", ll_data_cnf->c_reference1);
  
  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
    case SUA_LLC_RECEPTIVE_SUSPEND:
    case SUA_LLC_NOT_RECEPTIVE:
    case SUA_LLC_RECEPTIVE:
      for (refi = 0; refi < ll_data_cnf->c_reference1; refi++) {
        complete = FALSE;
        found = FALSE;
        last_help = NULL;
        help = sndcp_data->sua->first_seg_info;
        if (help == NULL) {
          /*
           * There are no N-PDUs buffered. CNF must be wrong.
           */
          if (ll_data_cnf != NULL) {
            TRACE_EVENT("There are no N-PDUs buffered. CNF must be wrong.");
            PFREE(ll_data_cnf);
          }
          return;
        }
        while (help != NULL && !found) {
          TRACE_EVENT_P1("ll_data_cnf->reference1[refi].ref_nsapi: %d",
                         ll_data_cnf->reference1[refi].ref_nsapi);
          TRACE_EVENT_P1("ll_data_cnf->reference1[refi].ref_npdu_num: %d",
                         ll_data_cnf->reference1[refi].ref_npdu_num);
          TRACE_EVENT_P1("ll_data_cnf->reference1[refi].ref_seg_num: %d",
                         ll_data_cnf->reference1[refi].ref_seg_num);
          if (ll_data_cnf->reference1[refi].ref_nsapi == help->nsapi &&
            ll_data_cnf->reference1[refi].ref_npdu_num == help->npdu_number) 
          {
            TRACE_EVENT_P1("ll_data_cnf->reference1[refi].ref_seg_num: %d",
                           ll_data_cnf->reference1[refi].ref_seg_num);
            TRACE_EVENT_P1("old help->cnf_segments: %d", help->cnf_segments);
            help->cnf_segments += 
              1 << ll_data_cnf->reference1[refi].ref_seg_num;
            TRACE_EVENT_P1("new help->cnf_segments: %d", help->cnf_segments);
            found = TRUE;
          }
          last_help = help;
          help = help->next;
        }
        if (!found) {
          /*
           * No affected NPDU found.
           */
          if (ll_data_cnf != NULL) {
            TRACE_EVENT("No affected NPDU found.");
            PFREE(ll_data_cnf);
          }
          return;
        }

        /*
         * If the affected N-PDU is completely confirmed, notify service nu and
         * free last_help.
         */

        TRACE_EVENT_P1("last_help->number_of_segments: %d", 
                       last_help->number_of_segments);
        TRACE_EVENT_P1("last_help->cnf_segments: %d", 
                       last_help->cnf_segments);

        if (last_help->number_of_segments == SNDCP_SEGMENT_NUMBERS_ACK) {
          if (last_help->cnf_segments ==
            SNDCP_MAX_COMPLETE /*4294967295 == 2**32 - 1*/) {
            complete = TRUE;
          }
        } else if (last_help->cnf_segments == 
          ((ULONG)(1 << last_help->number_of_segments) - 1)) {
          complete = TRUE;
        }
        if (complete) {
          /*
           * Notify service nu.
           */
          sig_sua_nu_data_cnf
            (ll_data_cnf->reference1[refi].ref_nsapi, 
            (UBYTE)ll_data_cnf->reference1[refi].ref_npdu_num);
          /*
           * Free last_help.
           */
          help = sndcp_data->sua->first_seg_info;
          if (help == last_help) {
            sndcp_data->sua->first_seg_info = help->next;
            MFREE(help);
            help = NULL;
          } else {
            T_SEG_INFO* help_next = help->next;
            while (help->next != last_help) {
              help = help->next;
            }
            help_next = help->next;
            help->next = help_next->next;
            MFREE (help_next);
          }
        }
      } /* for refi */
      break;
    default:
      TRACE_ERROR( "LL_DATA_CNF unexpected" );
      break;
  }
  if (ll_data_cnf != NULL) {
    PFREE(ll_data_cnf);
  }

} /* sua_ll_data_cnf() */


#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sua_ll_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_READY_IND
|
| Parameters  : *ll_ready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_ll_ready_ind ( T_LL_READY_IND *ll_ready_ind )
{ 
  U8 sapi_index = 0;

  TRACE_FUNCTION( "sua_ll_ready_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_ready_ind->sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      /*
       * Is queue with outgoing LL_DESC_REQs empty?
       */
      if (sndcp_data->sua->ll_desc_q_read ==
          sndcp_data->sua->ll_desc_q_write) {
        if (sndcp_data->sua->cia_state == CIA_IDLE) {
          sua_next_sn_data_req(ll_ready_ind->sapi);
        }
      }
      SET_STATE(SUA, SUA_LLC_RECEPTIVE_SUSPEND);
      break;
    case SUA_LLC_RECEPTIVE_SUSPEND:
      break;
    case SUA_LLC_NOT_RECEPTIVE:
      /*
       * Is queue with outgoing LL_DESC_REQs empty?
       */
      if (sndcp_data->sua->ll_desc_q_read ==
          sndcp_data->sua->ll_desc_q_write) {
        SET_STATE(SUA, SUA_LLC_RECEPTIVE);
        if (sndcp_data->sua->cia_state == CIA_IDLE) {
          sua_next_sn_data_req(ll_ready_ind->sapi);
        }
      } else { /* sndcp_data->sua.ll_desc_q is not empty */
        /*
         * Send next ll_desc_req from queue.
         */
        T_LL_DESC_REQ* ll_desc_req = 
          sndcp_data->sua->ll_desc_q[sndcp_data->sua->ll_desc_q_read];
        
        U8 ll_desc_sapi    = ll_desc_req->sapi;
        U8 ll_desc_seg_pos = ll_desc_req->seg_pos;
        U8 ll_desc_nsapi   = sua_get_nsapi(ll_desc_req);

        sndcp_data->sua->ll_desc_q[sndcp_data->sua->ll_desc_q_read] = NULL;
        sndcp_data->sua->ll_desc_q_read =
          (sndcp_data->sua->ll_desc_q_read + 1) % SNDCP_SEGMENT_NUMBERS_ACK;

#ifdef _SIMULATION_
        su_send_ll_data_req_test(ll_desc_req);
#else /* _SIMULATION_ */ 
        PSEND(hCommLLC, ll_desc_req);
#endif /* _SIMULATION_ */

        if ((ll_desc_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_desc_nsapi );
          sua_next_sn_data_req( ll_desc_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }


      } /* sndcp_data->sua.ll_desc_q is not empty */
      break;
    case SUA_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "LL_READY_IND unexpected" );
      break;
  }
  if (ll_ready_ind != NULL) {
    PFREE (ll_ready_ind);
  }

} /* su_ll_ready_ind() */
#else /* _SNDCP_DTI_2_ */
/*
+------------------------------------------------------------------------------
| Function    : sua_ll_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_READY_IND
|
| Parameters  : *ll_ready_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sua_ll_ready_ind ( T_LL_READY_IND *ll_ready_ind )
{ 
  UBYTE sapi_index = 0;

  TRACE_FUNCTION( "sua_ll_ready_ind" );

   /*
    * set service instance according to sapi in primitive
    */
  sndcp_get_sapi_index(ll_ready_ind->sapi, &sapi_index);
  sndcp_data->sua = & sndcp_data->sua_base[sapi_index];

  switch( GET_STATE( SUA ) )
  {
    case SUA_LLC_NOT_RECEPTIVE_SUSPEND:
      /*
       * Is queue with outgoing LL_DATA_REQs empty?
       */
      if (sndcp_data->sua->ll_data_q_read ==
          sndcp_data->sua->ll_data_q_write) {
        if (sndcp_data->sua->cia_state == CIA_IDLE) {
          sua_next_sn_data_req(ll_ready_ind->sapi);
        }
      }
      SET_STATE(SUA, SUA_LLC_RECEPTIVE_SUSPEND);
      break;
    case SUA_LLC_RECEPTIVE_SUSPEND:
      break;
    case SUA_LLC_NOT_RECEPTIVE:
      /*
       * Is queue with outgoing LL_DATA_REQs empty?
       */
      if (sndcp_data->sua->ll_data_q_read ==
          sndcp_data->sua->ll_data_q_write) {
        SET_STATE(SUA, SUA_LLC_RECEPTIVE);
        if (sndcp_data->sua->cia_state == CIA_IDLE) {
          sua_next_sn_data_req(ll_ready_ind->sapi);
        }
      } else { /* sndcp_data->sua.ll_data_q is not empty */
        /*
         * Send next ll_data_req from queue.
         */
        T_LL_DATA_REQ* ll_data_req = 
          sndcp_data->sua->ll_data_q[sndcp_data->sua->ll_data_q_read];
        
        UBYTE ll_data_sapi    = ll_data_req->sapi;
        UBYTE ll_data_seg_pos = ll_data_req->seg_pos;
        UBYTE ll_data_nsapi   = sua_get_nsapi(ll_data_req);

        sndcp_data->sua->ll_data_q[sndcp_data->sua->ll_data_q_read] = NULL;
        sndcp_data->sua->ll_data_q_read =
          (sndcp_data->sua->ll_data_q_read + 1) % SNDCP_SEGMENT_NUMBERS_ACK;

        PSEND (hCommLLC, ll_data_req);

        if ((ll_data_seg_pos & SEG_POS_LAST) > 0) {
          sig_sua_nu_ready_ind( ll_data_nsapi );
          sua_next_sn_data_req( ll_data_sapi );
          sndcp_data->sua->cia_state = CIA_IDLE;
        }


      } /* sndcp_data->sua.ll_data_q is not empty */
      break;
    case SUA_LLC_RECEPTIVE:
      break;
    default:
      TRACE_ERROR( "LL_READY_IND unexpected" );
      break;
  }
  if (ll_ready_ind != NULL) {
    PFREE (ll_ready_ind);
  }

} /* su_ll_ready_ind() */
#endif /* _SNDCP_DTI_2_ */
