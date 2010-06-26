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
| Purpose:    User Plane Control state machine implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_user_plane_control.h"
#include "sm_context_control.h"

#include "sm_upm_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_SM_USER_PLANE_CONTROL_TRANSITION_FUNC)(struct T_SM_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_SM_USER_PLANE_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_SM_USER_PLANE_CONTROL_TRANSITION_FUNC  func;
} T_SM_USER_PLANE_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void ignored          (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_start(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_complete(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_local(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_sm_activate_res(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_sm_deactivate_res(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_sm_modify_res(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/***********************************************************************
 * State/Transition Table
 */
static const T_SM_USER_PLANE_CONTROL_TRANSITION
transition[SM_USER_PLANE_CONTROL_NUMBER_OF_STATES][SM_USER_PLANE_CONTROL_NUMBER_OF_EVENTS] =
{
  {  /* S0: SM USER PLANE DEACTIVATED */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              state_event_error),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            ignored),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    handle_activate_start),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  ignored),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        ignored)
  },
  {  /* S1: SM USER PLANE ACTIVATE STARTED */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              state_event_error),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            state_event_error),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, handle_activate_complete),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  handle_deactivate_local),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        handle_deactivate)
  },
  {  /* S2: SM USER PLANE ACTIVATE COMPLETING */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              handle_sm_activate_res),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            state_event_error),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  handle_deactivate_local),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        handle_deactivate)
  },
  {  /* S3: SM USER PLANE ACTIVATED */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              state_event_error),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            state_event_error),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  handle_deactivate_local),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            handle_modify),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        handle_deactivate)
  },
  {  /* S4: SM USER PLANE MODIFYING */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              state_event_error),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            state_event_error),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                handle_sm_modify_res),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  handle_deactivate_local),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        handle_deactivate)
  },
  {  /* S5: SM USER PLANE DEACTIVATING */
    M_TRANSITION(SM_P_SM_ACTIVATE_RES,              state_event_error),
    M_TRANSITION(SM_P_SM_DEACTIVATE_RES,            handle_sm_deactivate_res),
    M_TRANSITION(SM_P_SM_MODIFY_RES,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_START,    state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATE_COMPLETE, state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE_LOCAL,  handle_deactivate_local),
    M_TRANSITION(SM_I_USER_PLANE_MODIFY,            state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATE,        ignored)
  }
};

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : state_event_error
+------------------------------------------------------------------------------
| Description : General function used to report state event errors.
|
| Parameters  : context          - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void state_event_error(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_ERROR("SM User Plane Control: STATE EVENT ERROR!");
}

/*
+------------------------------------------------------------------------------
| Function    : ignored
+------------------------------------------------------------------------------
| Description : General function used for transitions that shall be ignored
|
| Parameters  : context          - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void ignored(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("SM User Plane Control: Event ignored.");
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_start
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ACTIVATE_START in S0
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_activate_start(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_activate_start");

  /* Send SM_ACTIVATE_STARTED_IND */
  send_sm_activate_started_ind(context);

  /* Go to state SM USER PLANE ACTIVATE STARTED */
  context->user_plane_control_state = SM_USER_PLANE_ACTIVATE_STARTED;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_complete
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ACTIVATE_COMPLETE in S1
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_activate_complete(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_activate_complete");

  /* Send SM_ACTIVATE_IND */
  send_sm_activate_ind(context);

  /* Go to state SM USER PLANE ACTIVATE COMPLETING */
  context->user_plane_control_state = SM_USER_PLANE_ACTIVATE_COMPLETING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_MODIFY in S3
|
| Parameters  : context          - Context data
|               data             - Update flags
+------------------------------------------------------------------------------
*/
static void handle_modify(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_modify");

  /* Send SM_MODIFY_IND */
  send_sm_modify_ind(context);

  /* Go to state SM USER PLANE MODIFYING */
  context->user_plane_control_state = SM_USER_PLANE_MODIFYING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_DEACTIVATE in any state
|
| Parameters  : context          - Context data
|               data             - PS_rel_ind
+------------------------------------------------------------------------------
*/
static void handle_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate");

  /* Send SM_DEACTIVATE_IND with local parameter taken from input */
  send_sm_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi),
                         (data == NULL ? PS_REL_IND_NO : PS_REL_IND_YES));

  /* Go to state SM USER PLANE DEACTIVATING */
  context->user_plane_control_state = SM_USER_PLANE_DEACTIVATING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_local
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_DEACTIVATE_LOCAL in any state
|
| Parameters  : context          - Context data
|               data             - BOOL (TRUE = send SM_DEACTIVATE_IND)
+------------------------------------------------------------------------------
*/
static void handle_deactivate_local(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_local");

  if (data)
  {
    /* Send SM_DEACTIVATE_IND with local parameter true */
    send_sm_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi), PS_REL_IND_YES);

    /* Go to state SM USER PLANE DEACTIVATING */
    context->user_plane_control_state = SM_USER_PLANE_DEACTIVATING;
  } else {
    /* Go to state SM USER PLANE DEACTIVATED */
    context->user_plane_control_state = SM_USER_PLANE_DEACTIVATED;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_activate_res
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SM_ACTIVATE_RES in S2
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_sm_activate_res(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_sm_activate_res");

  /* Go to state SM USER PLANE ACTIVATED */
  context->user_plane_control_state = SM_USER_PLANE_ACTIVATED; 
 
  /* If comp params is sent by UPM copy it to context. When SMREG_ACTIVATE_CNF
   * primitive is composed this values are copied.
   */
  if ( (((T_SM_ACTIVATE_RES*)data)->v_comp_params == TRUE) )
  {
    memcpy(&context->comp_params, &((T_SM_ACTIVATE_RES*)data)->comp_params,
                                  sizeof(T_NAS_comp_params));
  } else {
    memset(&context->comp_params, 0, sizeof(T_NAS_comp_params));
  }
  /* Notify Context Control of activation completion */
  sm_context_control(context, SM_I_USER_PLANE_ACTIVATED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_deactivate_res
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SM_DEACTIVATE_RES in S5
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_sm_deactivate_res(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_sm_deactivate_res");

  /* Go to state SM USER PLANE DEACTIVATED */
  context->user_plane_control_state = SM_USER_PLANE_DEACTIVATED;

  /* Notify Context Control of deactivation completion */
  sm_context_control(context, SM_I_USER_PLANE_DEACTIVATED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_modify_res
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SM_MODIFY_RES in S4
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_sm_modify_res(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_sm_modify_res");

  /* Go to state SM USER PLANE ACTIVATED */
  context->user_plane_control_state = SM_USER_PLANE_ACTIVATED;

  /* Notify Context Control of modification completion */
  sm_context_control(context, SM_I_USER_PLANE_MODIFIED, NULL);
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_user_plane_control_init
+------------------------------------------------------------------------------
| Description : User Plane Control state machine initialization function

| Parameters  : context           - Context data
+------------------------------------------------------------------------------
*/
void sm_user_plane_control_init(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_user_plane_control_init");

  context->user_plane_control_state = SM_USER_PLANE_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_user_plane_control_exit
+------------------------------------------------------------------------------
| Description : User Plane Control state machine exit function

| Parameters  : context           - Context data
+------------------------------------------------------------------------------
*/
void sm_user_plane_control_exit(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_user_plane_control_exit");

  context->user_plane_control_state = SM_USER_PLANE_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_user_plane_control_state
+------------------------------------------------------------------------------
| Description : Returns a read-only string with the name of the active state.

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
#ifdef DEBUG
/*@observer@*/const char *
sm_user_plane_control_state(struct T_SM_CONTEXT_DATA *context)
{
  /*@observer@*/
  static const char *state_name[SM_USER_PLANE_CONTROL_NUMBER_OF_STATES] = {
    "S0_SM_USER_PLANE_DEACTIVATED",
    "S1_SM_USER_PLANE_ACTIVATE_STARTED",
    "S2_SM_USER_PLANE_ACTIVATE_COMPLETING",
    "S3_SM_USER_PLANE_ACTIVATED",
    "S4_SM_USER_PLANE_MODIFYING",
    "S5_SM_USER_PLANE_DEACTIVATING"
  };

  TRACE_ASSERT(context->user_plane_control_state < SM_USER_PLANE_CONTROL_NUMBER_OF_STATES);

  return state_name[(U16)context->user_plane_control_state];
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sm_user_plane_control
+------------------------------------------------------------------------------
| Description : User Plane Control state machine
|
| Parameters  : context          - Context data
|               event            - Internal event (see sm_user_plane_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void sm_user_plane_control(struct T_SM_CONTEXT_DATA *context,
			   T_SM_USER_PLANE_CONTROL_EVENT event,
			   /*@null@*/ void *data)
{
#ifdef DEBUG
  const char *old_state_name, *new_state_name;
  T_SM_USER_PLANE_CONTROL_STATE old_state;
  /*@observer@*/
  static const char *event_name[SM_USER_PLANE_CONTROL_NUMBER_OF_EVENTS] = {
    "SM_P_SM_ACTIVATE_RES",
    "SM_P_SM_DEACTIVATE_RES",
    "SM_P_SM_MODIFY_RES",
    "SM_I_USER_PLANE_ACTIVATE_START",
    "SM_I_USER_PLANE_ACTIVATE_COMPLETE",
    "SM_I_USER_PLANE_DEACTIVATE_LOCAL",
    "SM_I_USER_PLANE_MODIFY",
    "SM_I_USER_PLANE_DEACTIVATE"
  };

  TRACE_ASSERT(event < SM_USER_PLANE_CONTROL_NUMBER_OF_EVENTS);

  old_state = context->user_plane_control_state;
  old_state_name = sm_user_plane_control_state(context);

  if (transition[(U16)old_state][(U16)event].event != event) {
    (void)TRACE_ERROR("Event table error in sm_user_plane_control!");
  }
#endif /* DEBUG */

  transition[(U16)context->user_plane_control_state]
            [(U16)event].func(context, data);

#ifdef DEBUG
  new_state_name = sm_user_plane_control_state(context);

  (void)TRACE_EVENT_P4("SM U-PLANE #%d: %s => %s to %s", context->nsapi,
		       event_name[(U16)event], old_state_name, new_state_name);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
