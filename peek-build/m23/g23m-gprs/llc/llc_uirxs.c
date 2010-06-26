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
|             described in the SDL-documentation (UIRX-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef LLC_UIRXS_C
#define LLC_UIRXS_C
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

#include "llc_uirxf.h"  /* to get local UIRX functions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uirx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UIRX_ASSIGN_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_llme_uirx_assign_req (void)
{
  TRACE_ISIG( "sig_llme_uirx_assign_req" );

  switch (GET_STATE(UIRX))
  {
    case UIRX_TLLI_UNASSIGNED_NOT_READY:
      SET_STATE (UIRX, UIRX_ADM_NOT_READY);
      /*
       * Initialise current incarnation.
       */
      uirx_init_sapi();
      break;
    case UIRX_TLLI_UNASSIGNED_READY:
      SET_STATE (UIRX, UIRX_ADM_READY);
      /*
       * Initialise current incarnation.
       */
      uirx_init_sapi();
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UIRX_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_uirx_assign_req() */

#endif /* CF_FAST_EXEC */

/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uirx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UIRX_UNASSIGN_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_llme_uirx_unassign_req (void)
{
  TRACE_ISIG( "sig_llme_uirx_unassign_req" );

  switch (GET_STATE(UIRX))
  {
    case UIRX_ADM_NOT_READY:
      SET_STATE (UIRX, UIRX_TLLI_UNASSIGNED_NOT_READY);
      break;
    case UIRX_ADM_READY:
      SET_STATE (UIRX, UIRX_TLLI_UNASSIGNED_READY);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UIRX_UNASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_uirx_unassign_req() */

#endif /* CF_FAST_EXEC */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_uirx_reset_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_UIRX_RESET_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifndef CF_FAST_EXEC

GLOBAL void sig_llme_uirx_reset_req (void)
{
  TRACE_ISIG( "sig_llme_uirx_reset_req" );

  switch (GET_STATE(UIRX))
  {
    case UIRX_TLLI_UNASSIGNED_NOT_READY:
      /*
       * No break!
       */
    case UIRX_TLLI_UNASSIGNED_READY:
      /*
       * No break!
       */
    case UIRX_ADM_NOT_READY:
      /*
       * No break!
       */
    case UIRX_ADM_READY:
      /*
       * Only use this procedure as long as it initalises only the requested
       * variables on reset (V(UR) as of V6.4.0).
       */
      uirx_init_sapi();
      break;
    default:
      TRACE_ERROR( "SIG_LLME_UIRX_RESET_REQ unexpected" );
      break;
  }
} /* sig_llme_uirx_reset_req() */

#endif /* CF_FAST_EXEC */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_uirx_data_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_UIRX_DATA_IND
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL-UNITDATA-IND
|                                 primitive
|               nu - N(U) of the received UI frame
|
+------------------------------------------------------------------------------
*/
/*#if defined(CF_FAST_EXEC) || defined(_SIMULATION_) || \
   !defined(REL99)     || defined(LL_2to1) */

GLOBAL void sig_rx_uirx_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind,
                                  T_FRAME_NUM nu)
{
  USHORT            diff;

  TRACE_ISIG( "sig_rx_uirx_data_ind" );

  /*
   * SAPI has already been checked by service RX.
   */

  switch (GET_STATE(UIRX))
  {
    case UIRX_ADM_NOT_READY:
      /*
       * Calculate relative difference of V(UR) and N(U): add 512 to V(UR),
       * subtract N(U), apply modulo 512 to get back into the valid range
       */
      diff = (((llc_data->sapi->vur +
        (MAX_SEQUENCE_NUMBER+1)) - nu) % (MAX_SEQUENCE_NUMBER+1));

      /*
       * Original formula:
       *   V(UR)-32 <= N(U) < V(UR)
       * Translated:
       *   the relative difference must be maximally 32, and minimally 1
       */
      if ((diff > 0) AND (diff <= 32))
      {
        /*
         * We've got an retranmitted frame. Check if we got it already.
         */
        if (uirx_check_nu (nu, llc_data->sapi->vur) == TRUE)
        {
          /*
           * N(U) already received. Discard frame without any futher action.
           */
          PFREE (ll_unitdata_ind);
          TRACE_0_INFO("Duplicate frame number ignored");
          return;
        }
      }
      else
      {
        /*
         * We've got a new, not retransmitted, frame. Set V(UR) to the new value.
         */
        uirx_set_new_vur ((T_FRAME_NUM)(nu + 1));
      }

      /*
       * Store the N(U) of the frame to be already received.
       */
      uirx_store_nu (nu, llc_data->sapi->vur);

      /*
       * We are not ready to send data. Store frame in UIRX queue,
       * if possible.
       */
      uirx_queue_store (ll_unitdata_ind);
      break;

    case UIRX_ADM_READY:
      /*
       * Calculate relative difference of V(UR) and N(U): add 512 to V(UR),
       * subtract N(U), apply modulo 512 to get back into the valid range
       */
      diff = (((llc_data->sapi->vur +
        (MAX_SEQUENCE_NUMBER+1)) - nu) % (MAX_SEQUENCE_NUMBER+1));

      /*
       * Original formula:
       *   V(UR)-32 <= N(U) < V(UR)
       * Translated:
       *   the relative difference must be maximally 32, and minimally 1
       */
      if ((diff > 0) AND (diff <= 32))
      {
        /*
         * We've got an retranmitted frame. Check if we got it already.
         */
        if (uirx_check_nu (nu, llc_data->sapi->vur) == TRUE)
        {
          /*
           * N(U) already received. Discard frame without any futher action.
           */
          PFREE (ll_unitdata_ind);
          TRACE_0_INFO("Duplicated frame number ignored");
          return;
        }
      }
      else
      {
        /*
         * We've got a new, not retransmitted, frame. Set V(UR) to the new value.
         */
        uirx_set_new_vur ((T_FRAME_NUM)(nu + 1));
      }

      /*
       * Store the N(U) of the frame to be already received.
       */
      uirx_store_nu (nu, llc_data->sapi->vur);

      /*
       * Label S_UNITDATA
       */

      /*
       * No flow control for SAPI 1.
       */
      if (llc_data->current_sapi != LL_SAPI_1)
      {
        /*
         * Transit to state ADM_NOT_READY.
         */
        SET_STATE (UIRX, UIRX_ADM_NOT_READY);
      }

      /*
       * Send primitive LL-UNITDATA-IND to either GMM, SNDCP, or GSMS,
       * depending on the SAPI.
       */
      switch (ll_unitdata_ind->sapi)
      {
        case LL_SAPI_1:
          TRACE_PRIM_TO("GMM");
          TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                           BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                           ll_unitdata_ind->cipher);
          PSEND (hCommGMM, ll_unitdata_ind);
          break;

        case LL_SAPI_3:
        case LL_SAPI_5:
        case LL_SAPI_9:
        case LL_SAPI_11:
          TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                           BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                           ll_unitdata_ind->cipher);
          PSEND (hCommSNDCP, ll_unitdata_ind);
          break;

        case LL_SAPI_7:
#ifdef LL_2to1
          TRACE_PRIM_TO("MM");
          TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                           BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                           ll_unitdata_ind->cipher);
          PSEND (hCommMM, ll_unitdata_ind);
#else
          TRACE_PRIM_TO("GSMS");
          TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                           BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                           ll_unitdata_ind->cipher);
          PSEND (hCommGSMS, ll_unitdata_ind);
#endif
          break;

        default:
          PFREE (ll_unitdata_ind);
          TRACE_ERROR ("invalid global SAPI value");
          break;
      }
      break;

    default:
      PFREE (ll_unitdata_ind);
      TRACE_ERROR( "SIG_RX_UIRX_DATA_IND unexpected" );
      break;
  }
} /* sig_rx_uirx_data_ind() */

/*#endif */ /* CF_FAST_EXEC || _SIMULATION_ */


