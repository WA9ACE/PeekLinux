/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_ups.c
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
|             of incomming signals for the component
|             L2R of the base station
+----------------------------------------------------------------------------- 
*/ 


#ifndef L2R_UPS_C
#define L2R_UPS_C
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
+-------------------------------------------------------------------------------------
|  Function    : sig_mgt_up_conn_req
+-------------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_CONN_REQ received from process management.
|
|  Parameters  : framesPerPrim - Number of frames per primitive
|                frameSize     - Size of the frame
|                flowCtrlUsed  - Indicates wheather Flow Control is used or not.
|                riBuSize      - Size of the ring buffer
|                inst_id       - Instance Number
|
|
|  Return      : -
+-------------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_up_conn_req
            (
              UBYTE                  framesPerPrim,
              USHORT                 frameSize,
              BOOL                   flowCtrlUsed,
              T_PRIM_DESC_RIBU_SIZE  riBuSize,
              UBYTE                  inst_id
            )
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("sig_mgt_up_conn_req()");

  switch (GET_STATE (UP))
  {
  case UP_DISCONNECTED:
    dup->FramesPerPrim = framesPerPrim;
    dup->FrameSize     = frameSize;
    dup->DataSize      = frameSize - HT_LEN;
    dup->FlowCtrlUsed  = flowCtrlUsed;
    dup->RiBu.idx.depth = riBuSize;
    dup->FlowCtrlUsed  = flowCtrlUsed;
    dup->InstID        = inst_id; /* ###jkaa: and what about this??  l2r_data->mgt.InstID */

	up_rq_init();

    dup->DiscardRemapData = FALSE;

    if (dup->RiBu.idx.depth < UP_RIBU_PREALLOC + 1)
    {
      dup->RiBu.idx.depth = UP_RIBU_PREALLOC + 1;
    }
    if (dup->RiBu.idx.depth > MAX_UPRIM_RIBU_SIZE)
    {
      dup->RiBu.idx.depth = MAX_UPRIM_RIBU_SIZE;
    }

    dup->FlowThreshHi = 3 * dup->RiBu.idx.depth / 4;
    dup->FlowThreshLo = dup->FlowThreshHi - 2;
    if (dup->FlowThreshLo < 1)
    {
      dup->FlowThreshLo = 1;
    }
    dup->DnFlow = FL_INACTIVE;
    dup->UpFlow = FL_INACTIVE;
    dup->ULFlow = FL_INACTIVE;
    dup->LLFlow = FL_INACTIVE;
    up_merge_flow();

    SET_STATE (UP_LL, ISW_IDLE);
    SET_STATE (UP_UL, IW_IDLE);

    up_init_ribu();
    up_send_status(   /* status must be sent after connecting */
      dup->LastSentSa,
      dup->LastSentSb,
      dup->MrgFlow,
      SO_END_EMPTY
      );
    up_send_prim_cond();
    SET_STATE (UP, UP_CONNECTED);
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_up_disc_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_DISC_REQ received from
|                process management.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_mgt_up_disc_req(void)
{
  TRACE_FUNCTION ("sig_mgt_up_disc_req()");

  switch (GET_STATE (UP))
  {
  case UP_CONNECTED:
  case UP_REMAP:
    up_deinit_ribu();
    SET_STATE (UP, UP_DISCONNECTED);
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_up_break_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_BREAK_REQ received from process management.
|
|  Parameters  : sa   - status bit ( general ready signal)
|                sb   - staus bit  (data are valid or)
|                flow - flow control staus (active or inactive)
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_mgt_up_break_req
            (
              T_BIT sa,
              T_BIT sb,
              T_FLOW flow
            )
{
  TRACE_FUNCTION ("sig_mgt_up_break_req()");

  switch (GET_STATE (UP))
  {
    case UP_CONNECTED:
    case UP_REMAP:
      up_deinit_ribu();
      up_init_ribu();
      up_store_status(sa, sb, flow);
      up_send_status(sa, sb, l2r_data->up.MrgFlow, SO_BREAK_REQ); /*jk:030501*/
      up_send_prim_cond();
      break;
  }
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_up_clear_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_CLEAR_REQ received from
|                process management.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_mgt_up_clear_req(void)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("sig_mgt_up_clear_req()");

  switch (GET_STATE (UP))
  {
    case UP_CONNECTED:
    case UP_REMAP:
      up_deinit_ribu();
      up_init_ribu();
      /*
      status must be sent after reset
      */
      up_send_status(dup->LastRcvdSa, dup->LastRcvdSb, dup->MrgFlow, SO_END_EMPTY);
      up_send_prim_cond();
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_up_dti_conn_setup
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_DTI_CONN_SETUP
|                received from process management.
|
|  Parameters  : link_id            channel id of the link
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_up_dti_conn_setup (ULONG link_id)
{
  TRACE_FUNCTION ("sig_mgt_up_dti_conn_setup()");
  l2r_data->up.link_id = link_id;
  SET_STATE (UP_UL, IW_IDLE); /* jk: to IDLE in order to be able to respond with DTI_READY_IND */
                                   /* when the next condition is fullfilled */
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_up_dti_conn_open
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_DTI_CONN_OPEN
|                received from process management.
|
|  Parameters  : -
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_up_dti_conn_open()
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("sig_mgt_up_dti_conn_open()");

  dup->DtiConnected = TRUE;

  /* jk: DTI_READY_IND only when the primitive is sent to UL */
  if (dup->Prim EQ NULL)
  {
    up_send_ready();
  }
  else if (dup->Prim->desc_list2.first EQ 0)
  {
    PFREE (dup->Prim);
    up_send_ready();
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dn_up_flow
+------------------------------------------------------------------------------
|  Description :Process signal SIG_DN_UP_FLOW received from process management.
|
|  Parameters  : flow - flow control active or inactive
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dn_up_flow(T_FLOW flow)
{
  T_UP *dup = &l2r_data->up;

  T_FLOW oldFlow;

  TRACE_FUNCTION ("sig_dn_up_flow()");

  switch (GET_STATE (UP))
  {
  case UP_CONNECTED:
    oldFlow = dup->DnFlow;
    dup->DnFlow = flow;
    if (flow EQ oldFlow)
      return;

    up_merge_flow();
    if ((dup->LastSentFlow EQ dup->MrgFlow) OR (GET_STATE (UP_LL) NEQ ISW_WAIT))
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
      SET_STATE (UP_LL, ISW_IDLE)
    else
      SET_STATE (UP_LL, ISW_SEND)
    break;

  case UP_REMAP:
    oldFlow = dup->DnFlow;
    dup->DnFlow = flow;
    if (flow NEQ oldFlow)
    {
      up_merge_flow();
    }
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dn_up_ll_flow
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DN_UP_LL_FLOW received from process management.
|
|  Parameters  : flow - flow control status (active - inactive)
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dn_up_ll_flow(T_FLOW flow)
{
  T_UP *dup = &l2r_data->up;
  TRACE_FUNCTION ("sig_dn_up_ll_flow()");

  switch (GET_STATE (UP))
  {
  case UP_CONNECTED:
    dup->LLFlow = flow;

    if (dup->LLFlow EQ FL_INACTIVE AND GET_STATE (UP_LL) EQ ISW_WAIT AND dup->RiBu.idx.filled)
    {
      TIMERSTOP (TIMER_TUP_SND);
      up_send_current_prim();

      if (!dup->RiBu.idx.filled)
        SET_STATE (UP_LL, ISW_IDLE)
      else
        SET_STATE (UP_LL, ISW_SEND)
    }
    break;

  case UP_REMAP:
    dup->LLFlow = flow;
    break;
  }
}
