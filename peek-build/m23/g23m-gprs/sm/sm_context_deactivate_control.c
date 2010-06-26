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
| Purpose:    Context Deactivate Control sub-state machine implementation
|             in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_context_control.h"
#include "sm_context_deactivate_control.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_SM_CONTEXT_DEACTIVATE_CONTROL_TRANSITION_FUNC)(struct T_SM_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_SM_CONTEXT_DEACTIVATE_CONTROL_TRANSITION_FUNC  func;
} T_SM_CONTEXT_DEACTIVATE_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void ignored          (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void go_to_S1         (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void go_to_S2         (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_network_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_user_plane_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_network_and_user_plane_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_user_plane_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_network_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_completed(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/***********************************************************************
 * State/Transition Table
 */
static const T_SM_CONTEXT_DEACTIVATE_CONTROL_TRANSITION
transition[SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_STATES][SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_EVENTS] =
{
  {  /* S0: SM CONTEXT DEACTIVATE NULL */
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK,                handle_deactivate_network_in_S0),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE,             handle_deactivate_user_plane_in_S0),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, handle_deactivate_network_and_user_plane_in_S0),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_COMPLETED,      ignored),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE_COMPLETED,   ignored)
  },
  {  /* S1: SM CONTEXT DEACTIVATE NETWORK */
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK,                ignored),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE,             handle_deactivate_user_plane_in_S1),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, handle_deactivate_user_plane_in_S1),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_COMPLETED,      handle_deactivate_completed),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE_COMPLETED,   state_event_error)
  },
  {  /* S2: SM CONTEXT DEACTIVATE USER PLANE */
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK,                handle_deactivate_network_in_S2),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE,             ignored),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, handle_deactivate_network_in_S2),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_COMPLETED,      state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE_COMPLETED,   handle_deactivate_completed)
  },
  {  /* S3: SM CONTEXT DEACTIVATE BOTH */
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK,                ignored),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE,             ignored),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, ignored),
    M_TRANSITION(SM_I_DEACTIVATE_NETWORK_COMPLETED,      go_to_S2),
    M_TRANSITION(SM_I_DEACTIVATE_USER_PLANE_COMPLETED,   go_to_S1)
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
  (void)TRACE_ERROR("SM Context Deactivate Control: STATE EVENT ERROR!");
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
  (void)TRACE_FUNCTION("SM Context Deactivate Control: Event ignored.");
}

/*
+------------------------------------------------------------------------------
| Function    : go_to_S*
+------------------------------------------------------------------------------
| Description : General functions for simple state transitions
|
| Parameters  : context          - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void go_to_S1(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  /* Go to S1: SM CONTEXT DEACTIVATE NETWORK */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_NETWORK;
}

static void go_to_S2(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  /* Go to S0: SM CONTEXT DEACTIVATE USER PLANE */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_USER_PLANE;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_network_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_NETWORK in S0
|
| Parameters  : context          - Context data
|               data             - Cause
+------------------------------------------------------------------------------
*/
static void handle_deactivate_network_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_network_in_S0");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM CONTEXT DEACTIVATE NETWORK */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_NETWORK;

  /* Order Network Control to deactivate */
  sm_network_control(context, SM_I_NETWORK_DEACTIVATE, data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_user_plane_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_USER_PLANE in S0
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_user_plane_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_user_plane_in_S0");

  /* Go to state SM CONTEXT DEACTIVATE USER PLANE */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_USER_PLANE;

  /* Order User Plane Control to deactivate */
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_network_and_user_plane_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE in S0
|
| Parameters  : context          - Context data
|               data             - Cause
+------------------------------------------------------------------------------
*/
static void handle_deactivate_network_and_user_plane_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_network_and_user_plane_in_S0");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM CONTEXT DEACTIVATE BOTH */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_BOTH;

  /* Order Network Control and User Plane Control to deactivate */
  sm_network_control   (context, SM_I_NETWORK_DEACTIVATE,    data);
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_user_plane_in_S1
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_USER_PLANE in S1
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_user_plane_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_user_plane_in_S1");

  /* Go to state SM CONTEXT DEACTIVATE BOTH */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_BOTH;

  /* Order User Plane Control to deactivate */
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_network_in_S2
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_NETWORK in S2
|
| Parameters  : context          - Context data
|               data             - Cause
+------------------------------------------------------------------------------
*/
static void handle_deactivate_network_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_network_in_S2");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM CONTEXT DEACTIVATE BOTH */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_BOTH;

  /* Order Network Control to deactivate */
  sm_network_control(context, SM_I_NETWORK_DEACTIVATE, data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_completed
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_DEACTIVATE_COMPLETED in S1, or
|               event SM_I_USER_PLANE_DEACTIVATED in S2.
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_completed(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_completed");

  /* Go to state SM CONTEXT DEACTIVATE NULL */
  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_NULL;

  /* Report deactivation complete to Context Control */
  sm_context_control(context, SM_I_DEACTIVATE_COMPLETED, NULL);
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_context_deactivate_control_init
+------------------------------------------------------------------------------
| Description : User Plane Control state machine initialization function

| Parameters  : context           - Context data
+------------------------------------------------------------------------------
*/
void sm_context_deactivate_control_init(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_context_deactivate_control_init");

  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_context_deactivate_control_exit
+------------------------------------------------------------------------------
| Description : User Plane Control state machine exit function

| Parameters  : context           - Context data
+------------------------------------------------------------------------------
*/
void sm_context_deactivate_control_exit(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_context_deactivate_control_exit");

  context->context_deactivate_control_state = SM_CONTEXT_DEACTIVATE_NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_context_deactivate_control_state
+------------------------------------------------------------------------------
| Description : Returns a read-only string with the name of the active state.

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
#ifdef DEBUG
/*@observer@*/const char *
sm_context_deactivate_control_state(struct T_SM_CONTEXT_DATA *context)
{
  /*@observer@*/
  static const char *state_name[SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_STATES] = {
    "S0_SM_CONTEXT_DEACTIVATE_NULL",
    "S1_SM_CONTEXT_DEACTIVATE_NETWORK",
    "S2_SM_CONTEXT_DEACTIVATE_USER_PLANE",
    "S3_SM_CONTEXT_DEACTIVATE_BOTH"
  };

  TRACE_ASSERT(context->context_deactivate_control_state < SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_STATES);

  return state_name[(U16)context->context_deactivate_control_state];
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sm_context_deactivate_control
+------------------------------------------------------------------------------
| Description : User Plane Control state machine
|
| Parameters  : context          - Context data
|               event            - Internal event (see sm_context_deactivate_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void sm_context_deactivate_control(struct T_SM_CONTEXT_DATA *context,
				   T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT event,
				   /*@null@*/ void *data)
{
#ifdef DEBUG
  const char *old_state_name, *new_state_name;
  T_SM_CONTEXT_DEACTIVATE_CONTROL_STATE old_state;
  /*@observer@*/
  static const char *event_name[SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_EVENTS] = {
    "SM_I_DEACTIVATE_NETWORK",
    "SM_I_DEACTIVATE_USER_PLANE",
    "SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE",
    "SM_I_DEACTIVATE_NETWORK_COMPLETED",
    "SM_I_DEACTIVATE_USER_PLANE_COMPLETED"
  };

  TRACE_ASSERT(event < SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_EVENTS);

  old_state = context->context_deactivate_control_state;
  old_state_name = sm_context_deactivate_control_state(context);

  if (transition[(U16)old_state][(U16)event].event != event) {
    (void)TRACE_ERROR("Event table error in sm_context_deactivate_control!");
  }
#endif /* DEBUG */

  transition[(U16)context->context_deactivate_control_state]
            [(U16)event].func(context, data);

#ifdef DEBUG
  new_state_name = sm_context_deactivate_control_state(context);

  (void)TRACE_EVENT_P4("SM CX DEAC #%d: %s => %s to %s", context->nsapi,
		       event_name[(U16)event], old_state_name, new_state_name);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
