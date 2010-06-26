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
| Purpose:    This module implements the process dispatcher
|             for the Session Management (SM) entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"
#include "sm_sequencer.h"

#include "sm_network_control.h"
#include "sm_aci_output_handler.h"
#include "sm_timer_handler.h"

#include "sm_dispatcher.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_allocate_context
+------------------------------------------------------------------------------
| Description : Allocate memory for a context data structure and initialize
|               memory and state machines.  Returns address of data structure.
|
| Parameters  : nsapi               - NSAPI of context to allocate
+------------------------------------------------------------------------------
*/
/*@null@*/ /*@only@*/static struct T_SM_CONTEXT_DATA *
sm_disp_allocate_context(int /*@alt U8@*/ nsapi)
{
  struct T_SM_CONTEXT_DATA *context;

  context                = sm_allocate_context_data();

  if (context != NULL) {
    memset(context, 0, sizeof(struct T_SM_CONTEXT_DATA));

    context->nsapi       = nsapi;

    sm_context_control_init           (context);
    sm_context_deactivate_control_init(context);
    sm_network_control_init           (context);
    sm_user_plane_control_init        (context);
  }

  return context;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_smreg_pdp_activate_req
+------------------------------------------------------------------------------
| Description : Dispatch SMREG_PDP_ACTIVATE_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_smreg_pdp_activate_req(T_SMREG_PDP_ACTIVATE_REQ *prim)
{
  struct T_SM_CONTEXT_DATA *context;
  T_CAUSE_ps_cause          cause;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_smreg_pdp_activate_req");
#endif


  if (!sm_is_attached() OR
     (sm_is_suspended() AND
     (sm_data.sm_suspend_cause EQ CAUSE_MM_LIMITED_SERVICE OR 
      sm_data.sm_suspend_cause EQ CAUSE_MM_NO_SERVICE)))
  {
    /*In case of limited service or no service, where GPRS is not present,
      SM should reject the request immediately T3380 rule is not followed.*/
      (void)TRACE_ERROR("PDP Activation rejected - limited service or no service. ");

      cause.ctrl_value     = CAUSE_is_from_nwsm;
      cause.value.sm_cause = (U16)CAUSE_NWSM_ACTIVATE_REJECTED_UNSPECIFIED;
      send_smreg_pdp_activate_rej(prim->nsapi, &cause);
      return;
  }


  /* If context was MT activated, context data already exists for the TI. */
  if (prim->ti != (U8)SM_TI_NONE) {
    context                = sm_extract_mt_context_data(prim->ti);

    if (context == NULL) {
      (void)TRACE_ERROR("Unable to find data structure allocated by MT activation request!");

      cause.ctrl_value     = CAUSE_is_from_nwsm;
      cause.value.sm_cause = (U16)CAUSE_NWSM_INVALID_TI;
      send_smreg_pdp_activate_rej(prim->nsapi, &cause);
      return;
    } else {
      context->nsapi       = prim->nsapi;
    }
    TRACE_ASSERT(context != NULL);
  } else {
    if (sm_get_context_data_from_nsapi(prim->nsapi) != NULL) {
      /* Context already exists: Reject */
      (void)TRACE_EVENT_P1("ERROR: Context already activated on NSAPI %d; "
			   "activation rejected...", (int)prim->nsapi);
      cause.ctrl_value     = CAUSE_is_from_nwsm;
      cause.value.sm_cause = (U16)CAUSE_NWSM_NSAPI_IN_USE;
      send_smreg_pdp_activate_rej(prim->nsapi, &cause);
      return;
    }
    /* MO activation: Allocate new structure */
    context = sm_disp_allocate_context(prim->nsapi);

    if (context == NULL) {
      (void)TRACE_ERROR("Unable to allocate data structure for new context!");
      return;
    }
  } 

  /*If this is not done comp params will not be passed to UPM*/
  if ( (prim->comp_params.hcomp  != NAS_HCOMP_OFF) || 
       (prim->comp_params.dcomp  != NAS_DCOMP_OFF) )
  {
    (void)TRACE_EVENT("Compression parameters included ");
    context->flags |= (U8)SM_CONTEXT_FLAG_COMP_PARAMS;
    if(prim->comp_params.msid == 0){
      prim->comp_params.msid = 16;
    }
  } else {
    (void)TRACE_EVENT("Compression parameters not included ");
  }

  /* Set SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND flag according to suspend status */
  sm_set_started_during_suspend(context);

  sm_assign_context_data_to_nsapi(context, prim->nsapi);

  context = sm_get_context_data_from_nsapi(prim->nsapi);

  sm_context_control(context, SM_P_SMREG_PDP_ACTIVATE_REQ, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_smreg_pdp_activate_sec_req
+------------------------------------------------------------------------------
| Description : Dispatch SMREG_PDP_ACTIVATE_SEC_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_smreg_pdp_activate_sec_req(T_SMREG_PDP_ACTIVATE_SEC_REQ *prim)
{
  struct T_SM_CONTEXT_DATA *context;
  T_CAUSE_ps_cause          cause;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_smreg_pdp_activate_req");
#endif

  if (sm_get_context_data_from_nsapi(prim->nsapi) != NULL) {
    /* Context already exists: Reject */
    (void)TRACE_EVENT_P1("ERROR: Context already activated on NSAPI %d; "
			 "activation rejected...", (int)prim->nsapi);
    cause.ctrl_value     = CAUSE_is_from_nwsm;
    cause.value.sm_cause = (U16)CAUSE_NWSM_NSAPI_IN_USE;
    send_smreg_pdp_activate_sec_rej(prim->nsapi, &cause);
    return;
  }

  if (sm_get_current_nw_release() == PS_SGSN_98_OLDER) {
    /* Secondary context activation is an R99-only procedure: Fail when in pre-R99! */
    (void)TRACE_ERROR("R99 procedure rejected while active in pre-R99 network!");
    cause.ctrl_value     = CAUSE_is_from_sm;
    cause.value.sm_cause = (U16)CAUSE_SM_R99_PROCEDURE_REJECTED_IN_R97;
    send_smreg_pdp_activate_sec_rej(prim->nsapi, &cause);
    return;
  }

  if (sm_get_context_data_from_nsapi(prim->pri_nsapi) == NULL) {
    (void)TRACE_ERROR("sm_disp_smreg_pdp_activate_req: Primary context referenced in primitive does not exist!");
    cause.ctrl_value     = CAUSE_is_from_nwsm;
    cause.value.sm_cause = (U16)CAUSE_NWSM_UNKNOWN_LINKED_TI;
    send_smreg_pdp_activate_sec_rej(prim->nsapi, &cause);
    return;
  }

  /* Allocate new context data structure */
  context = sm_disp_allocate_context(prim->nsapi);

  if (context == NULL) {
    (void)TRACE_ERROR("Unable to allocate data structure for new context!");
    return;
  }

  sm_set_secondary(context);
  /* Set SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND flag according to suspend status */
  sm_set_started_during_suspend(context);

  sm_assign_context_data_to_nsapi(context, prim->nsapi);

  context = sm_get_context_data_from_nsapi(prim->nsapi);

  sm_context_control(context, SM_P_SMREG_PDP_ACTIVATE_SEC_REQ, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_smreg_pdp_deactivate_req
+------------------------------------------------------------------------------
| Description : Dispatch SMREG_PDP_DEACTIVATE_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_smreg_pdp_activate_rej_res(T_SMREG_PDP_ACTIVATE_REJ_RES *prim)
{
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_smreg_pdp_activate_rej_res");
#endif

  /* Context was MT activated, context data already exists for the TI. */
  context                = sm_extract_mt_context_data(prim->ti);

  if (context == NULL) {
    (void)TRACE_ERROR("Unable to find data structure allocated by MT activation request!");
    return;
  }

  sm_context_control(context, SM_P_SMREG_PDP_ACTIVATE_REJ_RES, prim);

  /* Free context data */
  sm_free_context_data(context);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_smreg_pdp_deactivate_req
+------------------------------------------------------------------------------
| Description : Dispatch SMREG_PDP_DEACTIVATE_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_smreg_pdp_deactivate_req(T_SMREG_PDP_DEACTIVATE_REQ *prim)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_smreg_pdp_deactivate_req");
#endif

  /* Forward to Sequencer */
  sm_sequencer(SM_P_SMREG_PDP_DEACTIVATE_REQ, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_smreg_pdp_modify_req
+------------------------------------------------------------------------------
| Description : Dispatch SMREG_PDP_MODIFY_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_smreg_pdp_modify_req(T_SMREG_PDP_MODIFY_REQ *prim)
{
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_smreg_pdp_modify_req");
#endif

  context = sm_get_context_data_from_nsapi(prim->nsapi);

  if (context == NULL) {
    T_CAUSE_ps_cause          cause;

    (void)TRACE_ERROR("Non-existing context!");

    cause.ctrl_value     = CAUSE_is_from_sm;
    cause.value.sm_cause = (U16)CAUSE_SM_INVALID_NSAPI;
    send_smreg_pdp_modify_rej(prim->nsapi, &cause);
  } else if (sm_get_current_nw_release() == PS_SGSN_98_OLDER) {
    /* MO Modify is an R99-only procedure: Fail when in pre-R99! */
    T_CAUSE_ps_cause     cause;

    (void)TRACE_ERROR("R99 procedure rejected while active in pre-R99 network!");
    cause.ctrl_value     = CAUSE_is_from_sm;
    cause.value.sm_cause = (U16)CAUSE_SM_R99_PROCEDURE_REJECTED_IN_R97;
    send_smreg_pdp_modify_rej(context->nsapi, &cause);
  } else { /* context != NULL */
    /* Set SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND flag according to suspend status */
    sm_set_started_during_suspend(context);

    /* Forward to Context Control */
    sm_context_control(context, SM_P_SMREG_PDP_MODIFY_REQ, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_sm_activate_res
+------------------------------------------------------------------------------
| Description : Dispatch SM_ACTIVATE_RES
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_sm_activate_res(T_SM_ACTIVATE_RES *prim)
{
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_sm_activate_res");
#endif

  context = sm_get_context_data_from_nsapi(prim->nsapi);

  if (context == NULL) {
    (void)TRACE_ERROR("Non-existing context!");
    return;
  }

  /* Forward to User Plane Control */
  sm_user_plane_control(context, SM_P_SM_ACTIVATE_RES, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_sm_deactivate_res
+------------------------------------------------------------------------------
| Description : Dispatch SM_DEACTIVATE_RES
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_sm_deactivate_res(T_SM_DEACTIVATE_RES *prim)
{
  int                       nsapi;
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_sm_deactivate_res");
#endif

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
    if (sm_is_nsapi_in_nsapi_set(nsapi, prim->nsapi_set)) {
      context = sm_get_context_data_from_nsapi(nsapi);

      /* Ignore non-existing contexts */
      if (context == NULL) {
	continue;
      }

      /* Forward to User Plane Control */
      sm_user_plane_control(context, SM_P_SM_DEACTIVATE_RES, prim);

      /* If SM_DEACTIVATE_RES completed deactivation, free context data */
      if (sm_is_context_pending_deallocation(context)) {
	sm_free_context_data_by_nsapi(context->nsapi);
      }
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_sm_modify_res
+------------------------------------------------------------------------------
| Description : Dispatch SM_MODIFY_RES
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_sm_modify_res(T_SM_MODIFY_RES *prim)
{
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_sm_modify_res");
#endif

  context = sm_get_context_data_from_nsapi(prim->nsapi);

  if (context == NULL) {
    (void)TRACE_ERROR("Non-existing context!");
    return;
  }

  /* Forward to User Plane Control */
  sm_user_plane_control(context, SM_P_SM_MODIFY_RES, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_sm_status_req
+------------------------------------------------------------------------------
| Description : Dispatch SM_STATUS_REQ
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_sm_status_req(T_SM_STATUS_REQ *prim)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_sm_status_req");
#endif

  /* Forward to Sequencer */
  sm_sequencer(SM_P_SM_STATUS_REQ, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_mmpm_attach_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_ATTACH_IND
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_mmpm_attach_ind(T_MMPM_ATTACH_IND *prim)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_mmpm_attach_ind");
#endif

  sm_data.sm_attached = TRUE;

  /* Forward to Sequencer */
  sm_sequencer(SM_P_MMPM_ATTACH_IND, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_mmpm_detach_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_DETACH_IND
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_mmpm_detach_ind(T_MMPM_DETACH_IND *prim)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_mmpm_detach_ind");
#endif

  sm_data.sm_attached = FALSE;

  /* Forward to Sequencer */
  sm_sequencer(SM_P_MMPM_DETACH_IND, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_mmpm_suspend_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_SUSPEND_IND
|
| Parameters  : mm_suspend_ind              - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_mmpm_suspend_ind(/*@unused@*/ T_MMPM_SUSPEND_IND *prim)
{
  int                       nsapi;
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_mmpm_suspend_ind");
#endif
  sm_data.sm_suspended = TRUE;
  sm_data.sm_suspend_cause = prim->ps_cause.value.mm_cause;

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL) {
      sm_network_control(context, SM_P_MMPM_SUSPEND_IND, NULL);
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_mmpm_resume_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_RESUME_IND
|
| Parameters  : mm_resume_ind               - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_mmpm_resume_ind(/*@unused@*/ T_MMPM_RESUME_IND *prim)
{
  int                       nsapi;
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_mmpm_resume_ind");
#endif

  sm_data.sm_suspended = FALSE;
  sm_data.sm_suspend_cause = CAUSE_MM_SUCCESS;

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL) {
      sm_network_control(context, SM_P_MMPM_RESUME_IND, NULL);
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_pei_timeout
+------------------------------------------------------------------------------
| Description : Dispatch pei_timeout
|
| Parameters  : index  - system timer index (index into sm_context_array)
+------------------------------------------------------------------------------
*/
void sm_disp_pei_timeout(U16 index)
{
  int                       nsapi;
  struct T_SM_CONTEXT_DATA *context;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_disp_pei_timeout");
#endif

  if (index > (U16)SM_MAX_NSAPI_OFFSET) {
    (void)TRACE_ERROR("Invalid timer index received!");
    return;
  }

  nsapi = sm_index_to_nsapi((U8)index);

  context = sm_get_context_data_from_nsapi(nsapi);

  if (context == NULL) {
    (void)TRACE_EVENT_P1("Warning!  Received timeout on non-existing NSAPI #%d; ignored...", nsapi);
    return;
  }

  if ((T_SM_TIMER_TYPE)context->active_timer == SM_TIMER_NONE) {
    (void)TRACE_EVENT_P1("Warning!  Received timeout on NSAPI #%d, which has no active timer; ignored...", nsapi);
    return;
  }

  if (context->timeouts == (U8)0) {
    (void)TRACE_EVENT_P1("Warning!  Received timeout on NSAPI #%d, which has no remaining timeouts; ignored...", nsapi);
    return;
  }

  /* Decrement remaining timeouts */
  context->timeouts -= (U8)1;

  /* Check remaining timeouts */
  if (context->timeouts > (U8)0) {
    T_SM_NETWORK_CONTROL_EVENT  event;
    /* More timeouts remaining - restart timer */
    sm_timer_restart(context);

    /* Inform Network Control of the timeout */
    switch (context->active_timer) {
    case SM_TIMER_T3380:   event = SM_T_T3380;  break;
    case SM_TIMER_T3381:   event = SM_T_T3381;  break;
    case SM_TIMER_T3390:   event = SM_T_T3390;  break;
    default:               return;
    }
    sm_network_control(context, event, NULL);
  } else {
    T_SM_NETWORK_CONTROL_EVENT  event;
    /* No timeouts remaining - Inform Network Control of the timer expiry */
    switch (context->active_timer) {
    case SM_TIMER_T3380:   event = SM_T_T3380_MAX;  break;
    case SM_TIMER_T3381:   event = SM_T_T3381_MAX;  break;
    case SM_TIMER_T3390:   event = SM_T_T3390_MAX;  break;
    default:               return;
    }
    sm_network_control(context, event, NULL);
  } /* if */ 

  /* Deallocate context if it was marked for deactivation due to T3390 max 
   * This change is separately taken from TI DK. Context data is cleaned up 
   * after deactivation due to max T3390 expiry. If this is not done the next
   * context activation on the same nsapi will fail.
   */
  if (sm_is_context_pending_deallocation(context)) {
    sm_free_context_data_by_nsapi(context->nsapi);
  }
}

/*==== END OF FILE ==========================================================*/
