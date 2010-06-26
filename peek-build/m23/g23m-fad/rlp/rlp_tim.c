/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  Rlp_tim.c
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
|  Purpose :  This Modul defines the timer handling functions
|             for the component Radio Link Protocol of the base station
+-----------------------------------------------------------------------------
*/

#ifndef RLP_TIM_C
#define RLP_TIM_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== EXPORT =====================================================*/

/*==== PRIVAT =====================================================*/

/*==== VARIABLES ==================================================*/

/*==== FUNCTIONS ==================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : tim_tt
+------------------------------------------------------------------------------
|  Description : Timeout of timer TT
|
|  Parameters  :
|
|
|  Return      :
+------------------------------------------------------------------------------
*/


LOCAL void tim_tt (void)
{

  BOOL retransError;

  TRACE_FUNCTION ("tim_tt()");

  switch (GET_STATE (KER))
  {
    case RLP_PENDING_CONNECT_REQ:
      /*
       * processing for state RLP_PENDING_CONNECT_REQ
       */
      rlp_data->ker.Poll_xchg = IW_IDLE;

      if (rlp_data->ker.SABM_Count > rlp_data->ker.N2)
      {
        ker_send_rlp_error_ind (RLP_ERR_SABM_RETRANS);
        SET_STATE (KER, RLP_PERMANENT_ERROR);
      }
      else
        rlp_data->ker.SABM_State = ISW_SEND;
      break;

    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      rlp_data->ker.Poll_xchg = IW_IDLE;

      sbm_check_rej_count(&retransError);

      if (!retransError)
      {
        switch (rlp_data->ker.Poll_State)
        {
          case ISW_IDLE:
            rlp_data->ker.Poll_State = ISW_SEND;
            break;

          case ISW_WAIT:
            if (rlp_data->ker.Poll_Count <= rlp_data->ker.N2)
              rlp_data->ker.Poll_State = ISW_SEND;

            else
            {
              ker_reset_all_t_rcvs ();
              rbm_reset_srej_slots();
              TIMERSTOP (TRCVR_HANDLE);
              rlp_data->ker.DISC_State = ISW_SEND;
              rlp_data->ker.DISC_Count = 0;
              rlp_data->ker.DISC_PBit = 1;

              ker_deinit_link_vars ();
              rlp_data->ker.DISC_Ind = TRUE;

              SET_STATE (KER, RLP_DISCONNECT_INITIATED);
              TRACE_EVENT("#10");
            }
            break;
        }
      }
      else
      {
        ker_reset_all_t_rcvs ();
        rbm_reset_srej_slots();
        TIMERSTOP (TRCVR_HANDLE);

        rlp_data->ker.DISC_State = ISW_SEND;
        rlp_data->ker.DISC_Count = 0;
        rlp_data->ker.DISC_PBit = 1;

        ker_deinit_link_vars ();
        rlp_data->ker.DISC_Ind = TRUE;

        SET_STATE (KER, RLP_DISCONNECT_INITIATED);
        TRACE_EVENT("#11");
      }

      break;

    case RLP_DISCONNECT_INITIATED:
      /*
       * processing for state RLP_DISCONNECT_INITIATED
       */
      if (rlp_data->ker.DISC_PBit EQ 1)
        rlp_data->ker.Poll_xchg = IW_IDLE;

      if (rlp_data->ker.DISC_Count > rlp_data->ker.N2)
      {
        ker_send_rlp_error_ind (RLP_ERR_DISC_RETRANS);
        SET_STATE (KER, RLP_PERMANENT_ERROR);
      }
      else
        rlp_data->ker.DISC_State = ISW_SEND;
      break;

    case RLP_PENDING_RESET_REQ:
      /*
       * processing for state RLP_PENDING_RESET_REQ
       */
      rlp_data->ker.Poll_xchg = IW_IDLE;
      if (rlp_data->ker.SABM_Count > rlp_data->ker.N2)
      {
        ker_send_rlp_error_ind (RLP_ERR_SABM_RETRANS);
        SET_STATE (KER, RLP_PERMANENT_ERROR);
      }
      else
        rlp_data->ker.SABM_State = ISW_SEND;
      break;

    case RLP_PENDING_REMAP_REQ:
      /*
       * processing for state RLP_PENDING_REMAP_REQ
       */
      rlp_data->ker.Poll_xchg = IW_IDLE;

      ker_send_rlp_error_ind (RLP_ERR_REMAP_RETRANS);
      SET_STATE (KER, RLP_PERMANENT_ERROR);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : tim_tul_rcv
+------------------------------------------------------------------------------
|  Description : Timeout of timer TUL_RCV
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void tim_tul_rcv (void)
{
  TRACE_FUNCTION ("tim_tul_rcv()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
    case RLP_PENDING_REMAP_REQ:
    case RLP_REMAP_DATA:
      /*
       * processing for states RLP_CONNECTION_ESTABLISHED
       *                       RLP_PENDING_REMAP_REQ:
       *                       RLP_REMAP_DATA:
       */
      switch (rlp_data->ker.UL_Rcv_State)
      {
        case IW_WAIT:
        {
          T_RLP_DATA_IND *rlp_data_ind;

          if (rlp_data->rbm.CurrPrimCou > 0 )
          {
            /* if some data are there */
            rlp_data_ind = rbm_get_curr_prim ();
            /*
             * send the waiting but not complete filled prim to L2R
             */
            PSENDX (L2R, rlp_data_ind);
            rlp_data->ker.UL_Rcv_State = IW_IDLE;
          }
          else
          {
            TRACE_EVENT("no data to send!");
          }
          break;
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : tim_trcvr
+------------------------------------------------------------------------------
|  Description : Timeout of timer TRCVR
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void tim_trcvr (void)
{
  TRACE_FUNCTION ("tim_trcvr()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      switch (rlp_data->ker.REJ_State)
      {
        case ISW_WAIT:
          ker_reset_all_t_rcvs();
          rbm_reset_srej_slots();

          TIMERSTOP(TT_HANDLE);
          rlp_data->ker.REJ_State = ISW_IDLE; /* jk: acc. ETSI after timer reset IDLE */
                                              /* in order to enable new REJ condition */
          /*
           * Don't disconnect
           * a timeout of TRCVR can be ignored
           * The network will get a timeout for the sent frames,
           * which we have not received.
           */
          /*
          rlp_data->ker.DISC_State = ISW_SEND;
          rlp_data->ker.DISC_Count = 0;

          switch(rlp_data->ker.Poll_xchg)
          {
          case IW_IDLE:
            rlp_data->ker.DISC_PBit = 1;
            break;

          default:
            rlp_data->ker.DISC_PBit = 0;
            break;
          }

          ker_deinit_link_vars();
          rlp_data->ker.DISC_Ind = TRUE;
          SET_STATE (KER, RLP_DISCONNECT_INITIATED);
          TRACE_EVENT("#12");
          */
          break;
      }
        break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : tim_trcvs
+------------------------------------------------------------------------------
|  Description : Timeout of timer TRCVS
|
|  Parameters  : rcvs_slot
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void tim_trcvs (T_FRAME_NUM rcvs_slot)
{
  TRACE_FUNCTION ("tim_trcvs()");

  TRACE_EVENT_P1 ("Timeout RCVS[%d]", rcvs_slot);

  if (GET_STATE (KER) EQ RLP_CONNECTION_ESTABLISHED)
  {
    if (rbm_is_state_wait(rcvs_slot))
    {
      rlp_data->ker.SREJ_Count--;

      if(srm_get_counter(rcvs_slot) > rlp_data->ker.N2)
      {
        ker_reset_all_t_rcvs();
        rbm_reset_srej_slots();
        TIMERSTOP(TT_HANDLE);
        rlp_data->ker.DISC_State = ISW_SEND;
        rlp_data->ker.DISC_Count = 0;

        switch(rlp_data->ker.Poll_xchg)
        {
        case IW_IDLE:
          rlp_data->ker.DISC_PBit = 1;
          break;

        default:
          rlp_data->ker.DISC_PBit = 0;
          break;
        }
        ker_deinit_link_vars();
        rlp_data->ker.DISC_Ind = TRUE;
        SET_STATE (KER, RLP_DISCONNECT_INITIATED);
        TRACE_EVENT("#13");
      }
      else
      {
        rbm_set_rslot_wait2srej (rcvs_slot);
      }
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : tim_txid
+------------------------------------------------------------------------------
|  Description : Timeout of timer TXID
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void tim_txid (void)
{
  TRACE_FUNCTION ("tim_txid()");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * no processing for state RLP_ADM_AND_DETACHED
       */
      break;

    default:
      /*
       * processing for all other kernel states
       */
      rlp_data->ker.Poll_xchg = IW_IDLE;
      if (rlp_data->ker.XID_Count >= rlp_data->ker.N2)
      {
        ker_send_rlp_error_ind (RLP_ERR_XID_RETRANS);
        SET_STATE (KER, RLP_PERMANENT_ERROR);
      }
      else
      {
        SET_STATE(KERXID_C, ISW_SEND);
        rlp_data->ker.XID_Count++;
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : rlp_exec_timeout
+------------------------------------------------------------------------------
|  Description : execute timeout function depending on index
|
|  Parameters  : index
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rlp_exec_timeout (USHORT index)
{
  T_FRAME_NUM   rcvs_slot;

  TRACE_FUNCTION ("rlp_exec_timeout()");

  switch (index)
  {
    case TT_HANDLE:
      tim_tt ();
      break;

    case TUL_RCV_HANDLE:
      tim_tul_rcv ();
      break;

    case TRCVR_HANDLE:
      tim_trcvr ();
      break;

    case TXID_HANDLE:
      tim_txid ();
      break;

    default:
      if ( index < MAX_SREJ_COUNT )
      {
        /* case TRCVS_HANDLE: */
        if ((rcvs_slot = ker_getSlotTRCVS (index)) NEQ INVALID_IDX)
        {
          /*
           * store the index of the rcvs_timer
           */
          rlp_data->rcvs_slot = rcvs_slot;
          tim_trcvs (rlp_data->rcvs_slot);
        }
      }
      else
      {
        TRACE_EVENT ("UNDEFINED TIMEOUT");
      }
      break;
  }
}


