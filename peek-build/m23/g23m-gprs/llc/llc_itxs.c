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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (ITX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXS_C
#define LLC_ITXS_C
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

#include "llc_itxs.h"   /* to get ITX signal definitions */
#include "llc_itxf.h"   /* to get ITX function definitions */
#include "llc_itxt.h"   /* to get ITX T201 function definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_assign_req (void)
{ 
  TRACE_ISIG( "sig_llme_itx_assign_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
      /*
       * Init Sapi
       */
      itx_init_sapi();
      SET_STATE (ITX, ITX_TLLI_ASSIGNED);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_ITX_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_itx_assign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_unassign_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_unassign_req" );

  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
      /*
       * Ignore unexpected signal.
       */
      break;

    default:
      TIMERSTOP (T201);
      itx_s_queue_clean ();
      itx_i_queue_clean ();
      itx_init_sapi ();
      SET_STATE (ITX, ITX_TLLI_UNASSIGNED);
      break;
  }
} /* sig_llme_itx_unassign_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_reset_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_RESET_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_reset_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_reset_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
    case ITX_TLLI_ASSIGNED:
      /*
       * nothing to do
       */
      break;

    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      TIMERSTOP (T201);
      itx_s_queue_clean ();
      itx_i_queue_clean ();
      itx_init_sapi ();
      SET_STATE (ITX, ITX_TLLI_ASSIGNED);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_ITX_RESET_REQ unexpected" );
      break;
  }
} /* sig_llme_itx_reset_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_abmest_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_ABMEST_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_abmest_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_abmest_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
      /*
       * Ignore unexpected signal.
       */
      TRACE_ERROR( "SIG_LLME_ITX_ABMEST_REQ unexpected" );
      break;

    default:
      TIMERSTOP (T201);
      itx_s_queue_clean ();
      itx_i_queue_clean ();
      itx_init_sapi ();
      itx_handle_ll_ready_ind (FALSE);
      SET_STATE (ITX, ITX_ABM);
      TRACE_1_INFO("ABM established s:%d", llc_data->current_sapi);
      break;
  }
} /* sig_llme_itx_abmest_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_abmrel_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_ABMREL_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_abmrel_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_abmrel_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
      /*
       * Ignore unexpected signal.
       */
      TRACE_ERROR( "SIG_LLME_ITX_ABMREL_REQ unexpected" );
      break;

    default:
      TIMERSTOP (T201);
      itx_s_queue_clean ();
      itx_i_queue_clean ();
      itx_init_sapi ();
      SET_STATE (ITX, ITX_TLLI_ASSIGNED);
      TRACE_1_INFO("ABM released s:%d", llc_data->current_sapi);
      break;
  }
} /* sig_llme_itx_abmrel_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_suspend_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_SUSPEND_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_suspend_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_suspend_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
    case ITX_TLLI_ASSIGNED:
      /*
       * nothing to do
       */
      break;

    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      /*
       * Reset T201, but do NOT reset t201_entry pointer
       * because we have to restart T201 later!
       */
      TIMERSTOP (T201);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_LLME_ITX_SUSPEND_REQ unexpected" );
      break;
  }
} /* sig_llme_itx_suspend_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_itx_resume_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_ITX_RESUME_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_itx_resume_req (void) 
{ 
  TRACE_ISIG( "sig_llme_itx_resume_req" );

  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_UNASSIGNED:
    case ITX_TLLI_ASSIGNED:
      /*
       * nothing to do
       */
      break;

    case ITX_ABM:
      /*
       * Restart T201 if a frame was associated to it.
       * Do not increment frame retransmission counter.
       */
      if ( llc_data->itx->t201_entry != NULL )
      {
        TIMERSTART (T201, llc_data->t200->length);
      }

      /*
       * Continue sending of frames
       */
      itx_send_next_frame (ABIT_NO_REQ);
      break;

    case ITX_ABM_PEER_BUSY:
      /*
       * Restart T201, but do not increment counter
       */
      TIMERSTART (T201, llc_data->t200->length);
      break;

    default:
      TRACE_ERROR( "SIG_LLME_ITX_RESUME_REQ unexpected" );
      break;
  }
} /* sig_llme_itx_resume_req() */




/*
+------------------------------------------------------------------------------
| Function    : sig_tx_itx_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TX_ITX_READY_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tx_itx_ready_ind (void) 
{ 
  TRACE_ISIG( "sig_tx_itx_ready_ind" );

  switch (GET_STATE(ITX))
  {
    case ITX_TLLI_ASSIGNED:
      llc_data->itx->tx_waiting = TRUE;
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      llc_data->itx->tx_waiting = TRUE;
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_TX_ITX_READY_IND unexpected" );
      break;
  }
} /* sig_tx_itx_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_ack_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_ACK_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_ack_ind (BOOL is_ack, T_FRAME_NUM num) 
{ 
  TRACE_ISIG( "sig_irx_itx_ack_ind" );
  
  num %= (MAX_SEQUENCE_NUMBER+1);

#ifdef TRACE_EVE
  {
    if (is_ack == TRUE)
    {
      TRACE_1_INFO("got frame ack:%d", num);
    }
    else
    {
      TRACE_1_INFO("got frame request:%d", num);
    }
  }
#endif

  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      /*
       * Set frame acknowledge status in the ITX queue.
       */
      if (is_ack)
        itx_i_queue_set_status (IQ_IS_ACK_FRAME, num);
      else
        itx_i_queue_set_status (IQ_RETR_FRAME, num);

      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_ACK_IND unexpected" );
      break;
  }
} /* sig_irx_itx_ack_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_cnf_l3data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_CNF_L3DATA_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_cnf_l3data_req (void) 
{ 
  UBYTE           state;
  BOOL            found;
  BOOL            data_retrieved = FALSE;
#ifdef LL_2to1
  T_LL_reference1    reference;
#else
  T_reference1    reference;
#endif

  TRACE_ISIG( "sig_irx_itx_cnf_l3data_req" );
  
  state = GET_STATE(ITX);
  
  switch (state)
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      /*
       * Get all acknowledged frames from the ITX queue.
       * If one or more are found, send an LL_DATA_CNF 
       * for each group of max LL_MAX_CNF L3-PDUs.
       */
      itx_i_queue_get_ready (&found, &reference, state);
      data_retrieved = found;

      /*
       * Label LLC_ACK_IND
       */

      while (found)
      {
        int num_ref = 0;

        PALLOC (ll_data_cnf, LL_DATA_CNF);

        ll_data_cnf->sapi = llc_data->current_sapi;

        do {

          ll_data_cnf->reference1[num_ref] = reference;
          num_ref++;

          itx_i_queue_get_ready (&found, &reference, state);

        } while (found && (num_ref < LLC_MAX_CNF));

        ll_data_cnf->c_reference1 = num_ref;

        TRACE_1_OUT_PARA("s:%d", ll_data_cnf->sapi );
        PSEND (hCommSNDCP, ll_data_cnf);
      }

      /*
       * If we have taken data from queue, check 
       * if a L3 ready indication is required
       */
      if (data_retrieved)
      {
        itx_handle_ll_ready_ind (data_retrieved);
      }

      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_CNF_L3DATA_REQ unexpected" );
      break;
  }
} /* sig_irx_itx_cnf_l3data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_trigger_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_TRIGGER_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_trigger_ind (void) 
{ 
  TRACE_ISIG( "sig_irx_itx_trigger_ind" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_TRIGGER_IND unexpected" );
      break;
  }
} /* sig_irx_itx_trigger_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_peer_busy_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_PEER_BUSY_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_peer_busy_ind (void) 
{ 
  TRACE_ISIG( "sig_irx_itx_peer_busy_ind" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
      /*
       * Start T201 to supervise peer busy condition,
       * init T201 peer busy restart counter and change 
       * into peer busy state.
       */
      TIMERSTART (T201, llc_data->t200->length);
      llc_data->itx->n_pb_retr = 0;
      SET_STATE (ITX, ITX_ABM_PEER_BUSY);
      TRACE_1_INFO("Peer Busy s:%d", llc_data->current_sapi);
      break;

    case ITX_ABM_PEER_BUSY:
      /*
       * Restart T201 to supervise peer busy condition,
       * reset T201 peer busy restart counter.
       */
      TIMERSTART (T201, llc_data->t200->length);
      llc_data->itx->n_pb_retr = 0;
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_PEER_BUSY_IND unexpected" );
      break;
  }
} /* sig_irx_itx_peer_busy_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_peer_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_PEER_READY_IND
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_peer_ready_ind (void) 
{ 
  TRACE_ISIG( "sig_irx_itx_peer_ready_ind" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
      /*
       * ignore it
       */
      break;

    case ITX_ABM_PEER_BUSY:
      /*
       * Clear peer busy condition
       */
      SET_STATE (ITX, ITX_ABM);

      /*
       * Restart T201 if a frame was associated to it.
       * Do not increment frame retransmission counter.
       */
      if ( llc_data->itx->t201_entry != NULL )
      {
        TIMERSTART (T201, llc_data->t200->length);
      }

      /*
       * Restart sending of frames
       */
      TRACE_1_INFO("Peer Ready s:%d", llc_data->current_sapi);
      itx_send_next_frame (ABIT_NO_REQ);
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_PEER_READY_IND unexpected" );
      break;
  }
} /* sig_irx_itx_peer_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_send_rr_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_SEND_RR_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_send_rr_req (T_ABIT_REQ_TYPE req_type)
{ 
  TRACE_ISIG( "sig_irx_itx_send_rr_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_s_queue_store (I_RR, req_type, llc_data->sapi->vr, NULL);
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_SEND_RR_REQ unexpected" );
      break;
  }
} /* sig_irx_itx_send_rr_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_send_ack_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_SEND_ACK_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_send_ack_req (T_ABIT_REQ_TYPE req_type)
{ 
  TRACE_ISIG( "sig_irx_itx_send_ack_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_s_queue_store (I_ACK, req_type, llc_data->sapi->vr, NULL);
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_SEND_ACK_REQ unexpected" );
      break;
  }
} /* sig_irx_itx_send_ack_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_send_sack_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_SEND_SACK_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_send_sack_req (T_ABIT_REQ_TYPE req_type, T_SACK_BITMAP* bitmap)
{ 
  TRACE_ISIG( "sig_irx_itx_send_sack_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_s_queue_store (I_SACK, req_type, llc_data->sapi->vr, bitmap);
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_SEND_SACK_REQ unexpected" );
      break;
  }
} /* sig_irx_itx_send_sack_req() */




/*
+------------------------------------------------------------------------------
| Function    : sig_irx_itx_send_rnr_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_ITX_SEND_RNR_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_itx_send_rnr_req (T_ABIT_REQ_TYPE req_type)
{ 
  TRACE_ISIG( "sig_irx_itx_send_rnr_req" );
  
  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
    case ITX_ABM_PEER_BUSY:
      itx_s_queue_store (I_RNR, req_type, llc_data->sapi->vr, NULL);
      itx_send_next_frame (ABIT_NO_REQ);
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "SIG_IRX_ITX_SEND_RNR_REQ unexpected" );
      break;
  }
} /* sig_irx_itx_send_rnr_req() */

