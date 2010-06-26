/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
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
| Purpose:    Output functions for primitives from SM to the UPM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "sm.h"

#include "sm_upm_output_handler.h"
#include "sm_qos.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_sm_activate_started_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_ACTIVATE_STARTED_IND primitive.
|
| Parameters  : context   - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_activate_started_ind(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("send_sm_activate_started_ind");

  {
    PALLOC(prim, SM_ACTIVATE_STARTED_IND);

    /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi           = context->nsapi;
    prim->rat             = sm_get_current_rat();

    (void)PSEND(hCommUPM, prim);
   /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */	
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_activate_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_ACTIVATE_IND primitive.
|
| Parameters  : context   - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_activate_ind(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("send_sm_activate_ind");

  {
    PALLOC(prim, SM_ACTIVATE_IND);

    /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi           = context->nsapi;
    prim->sapi            = context->sapi;
    prim->radio_prio      = context->radio_prio;
    if (sm_is_pfi_included(context)) {
      prim->pkt_flow_id   = context->pfi;
    } else {
      prim->pkt_flow_id = (U8)PS_PKT_FLOW_ID_NOT_PRES;
    }
    sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);

    if ((context->flags & SM_CONTEXT_FLAG_COMP_PARAMS) != (U8)0) {
      prim->v_comp_params = (U8)TRUE;
      memcpy(&prim->comp_params, &context->comp_params, sizeof(T_NAS_comp_params));
    } else {
      prim->v_comp_params = (U8)FALSE;
    }

    (void)PSEND(hCommUPM, prim);
    /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */	
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_deactivate_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_DEACTIVATE_IND primitive.
|
| Parameters  : context   - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_deactivate_ind(U16 nsapi_set, T_PS_rel_ind rel_ind)
{
  (void)TRACE_FUNCTION("send_sm_deactivate_ind");

  {
    PALLOC(prim, SM_DEACTIVATE_IND);
   
    /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi_set       = nsapi_set;
    prim->rel_ind         = (U8)rel_ind;

    (void)PSEND(hCommUPM, prim);
    /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */	
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sm_modify_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SM_MODIFY_IND primitive.
|
| Parameters  : context   - Pointer to context data
+------------------------------------------------------------------------------
*/
void send_sm_modify_ind(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("send_sm_modify_ind");

  {
    PALLOC(prim, SM_MODIFY_IND);

    /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi           = context->nsapi;
    prim->sapi            = context->sapi;
    prim->radio_prio      = context->radio_prio;
    if (sm_is_pfi_included(context)) {
      prim->pkt_flow_id   = context->pfi;
    } else {
      prim->pkt_flow_id = (U8)PS_PKT_FLOW_ID_NOT_PRES;
    }
    sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);

    (void)PSEND(hCommUPM, prim);
    /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */	
  }
}

/*==== END OF FILE ==========================================================*/
