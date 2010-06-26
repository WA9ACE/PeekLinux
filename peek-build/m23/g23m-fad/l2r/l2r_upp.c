/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  L2r_upp.c
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
|  Purpose :  This Modul defines the functions for processing
|             of incomming primitives for the component
|             L2R of the base station
+-----------------------------------------------------------------------------
*/

#ifndef L2R_UPP_C
#define L2R_UPP_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_l2r.h"
#include "cnf_l2r.h"
#include "mon_l2r.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */

#include "cl_ribu.h"
#include "l2r.h"

/*==== CONST =======================================================*/
/*==== TYPES =======================================================*/
/*==== VAR EXPORT ==================================================*/
/*==== VAR LOCAL ===================================================*/
/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_up_data_received_ind
+------------------------------------------------------------------------------
|  Description : Process primitive DTI2_DATA_REQ received from CSDIWF.
|
|  Parameters  : dti_data_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_up_data_received_ind(T_DTI2_DATA_REQ *dti_data_req)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("sig_dti_up_data_received_ind()");

  PACCESS        (dti_data_req);

  switch (GET_STATE (UP))
  {
  case UP_CONNECTED:
    {
      if (GET_STATE (DTI) NEQ DTI_CLOSED)
      {
        /*
         * do not send flow control primitives automatically
         */
        dti_stop(l2r_hDTI, L2R_DTI_UP_DEF_INSTANCE, L2R_DTI_UP_INTERFACE, L2R_DTI_UP_CHANNEL);
      }

      SET_STATE (UP_UL, IW_IDLE);
      dup->Prim = dti_data_req;

      /* detect break request */
      if (dup->Prim->parameters.st_lines.st_break_len NEQ DTI_BREAK_OFF)
      {
        sig_up_mgt_break_ind(dup->Prim);
      }
      else
      {
        up_store_data();
      }

      switch (GET_STATE (UP_LL))
      {
      case ISW_WAIT:
        if (dup->LLFlow EQ FL_INACTIVE AND dup->RiBu.idx.filled)
        {
          /* L_UDR_1 */
          TIMERSTOP (TIMER_TUP_SND);
          up_send_current_prim();

          if (!dup->RiBu.idx.filled)
          {
            SET_STATE (UP_LL, ISW_IDLE);
          }
          else
          {
            SET_STATE (UP_LL, ISW_SEND);
          }
        }
        else
        {
          /* L_UDR_2 */
          if( dup->LastSentFlow EQ dup->MrgFlow )
            return;

          TIMERSTOP (TIMER_TUP_SND);

          switch (dup->LLFlow)
          {
          case FL_INACTIVE:
            up_send_prim_timeout();
            break;

          case FL_ACTIVE:
            up_send_empty_frame(dup->LastSentSa, dup->LastSentSb, dup->MrgFlow);
            break;
          }

          if (!dup->RiBu.idx.filled)
          {
            SET_STATE (UP_LL, ISW_IDLE);
          }
          else
          {
            SET_STATE (UP_LL, ISW_SEND);
          }
        }
        break;

      default:
        if (!dup->RiBu.idx.filled) /* no primitive is ready */
          return;

        /* L_UDR3 */
        up_check_flow();
        up_check_alloc();
        SET_STATE (UP_LL, ISW_SEND);
        break;
      }
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : up_rlp_ready_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_READY_IND received from RLP.
|
|  Parameters  : rlp_ready_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_rlp_ready_ind(T_RLP_READY_IND *rlp_ready_ind)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("rlp_ready_ind()");
  PACCESS        (rlp_ready_ind);

  switch (GET_STATE (UP))
  {
  case UP_CONNECTED:
    switch (GET_STATE (UP_LL))
    {
    case ISW_SEND:
      if (dup->LLFlow EQ FL_INACTIVE OR dup->UrgentMsg)
      {
        up_send_current_prim();
        if (!dup->RiBu.idx.filled)
        {
          SET_STATE (UP_LL, ISW_IDLE);
        }
      }
      else
      {
        if (dup->LastSentFlow NEQ dup->MrgFlow)
        {
          up_send_empty_frame(dup->LastSentSa, dup->LastSentSb, dup->MrgFlow);
        }
        else
        {
          SET_STATE (UP_LL, ISW_WAIT);
          TIMERSTART(TIMER_TUP_SND, LL_TO);
        }
      }
      break;

    default:
      SET_STATE (UP_LL, ISW_WAIT);
      TIMERSTART(TIMER_TUP_SND, LL_TO);
      break;
    }
    break;

  case UP_REMAP:
    SET_STATE (UP_LL, ISW_WAIT);
    break;
  }
  PFREE (rlp_ready_ind);
}

