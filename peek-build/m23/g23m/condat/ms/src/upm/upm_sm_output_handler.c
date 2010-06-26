/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  UPM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    Output functions for primitives from UPM to the SM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL ==================================================*/

/*==== INCLUDES =============================================================*/

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_sm_activate_res
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_ACTIVATE_RES primitive.
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/

void send_sm_activate_res(struct T_CONTEXT_DATA *ptr_context_data, void* data)
{
  (void)TRACE_FUNCTION("send_sm_activate_res");

  {
    PALLOC(prim, SM_ACTIVATE_RES);

    prim->nsapi = ptr_context_data->nsapi;

    if (data == NULL) /*Possible for UMTS*/
    {
      prim->v_comp_params = (U8)FALSE;
    } else {
      if( (((T_SN_ACTIVATE_CNF*)data)->comp_params.dcomp != 0) ||
          (((T_SN_ACTIVATE_CNF*)data)->comp_params.hcomp != 0)   )
      {
        prim->v_comp_params = (U8)TRUE;
        prim->comp_params.dcomp = ((T_SN_ACTIVATE_CNF*)data)->comp_params.dcomp;
        prim->comp_params.hcomp = ((T_SN_ACTIVATE_CNF*)data)->comp_params.hcomp;
        prim->comp_params.msid = ((T_SN_ACTIVATE_CNF*)data)->comp_params.msid;
      } else {
        prim->v_comp_params = (U8)FALSE;
      }
    }

    (void)PSEND(hCommSM, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_modify_res
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_MODIFY_RES primitive.
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_modify_res(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("send_sm_modify_res");

  {
    PALLOC(prim, SM_MODIFY_RES);

    /*lint !e613 (Warning -- Possible use of null pointer) */
    prim->nsapi = ptr_context_data->nsapi;

    (void)PSEND(hCommSM, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_deactivate_res
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_DEACTIVATE_RES primitive.
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_deactivate_res(U16 nsapi_set)
{
  (void)TRACE_FUNCTION("send_sm_deactivate_res");

  {
    PALLOC(prim, SM_DEACTIVATE_RES);

    prim->nsapi_set = nsapi_set;

    (void)PSEND(hCommSM, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_status_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_STATUS_REQ primitive.
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_status_req(U16 nsapi_set, T_PS_sapi llc_sapi,
			T_CAUSE_ctrl_value ctrl, U16 cause)
{
  (void)TRACE_FUNCTION("send_sm_status_req");

  {
    PALLOC(prim, SM_STATUS_REQ);

    prim->nsapi_set                = nsapi_set;
    prim->sapi                     = (U8)llc_sapi;
    prim->ps_cause.ctrl_value      = ctrl;
    prim->ps_cause.value.upm_cause = cause;

    (void)PSEND(hCommSM, prim);
  }
}
