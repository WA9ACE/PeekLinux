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
| Purpose:    Context Control state machine implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_context_control.h"
#include "sm_network_control.h"
#include "sm_user_plane_control.h"
#include "sm_sequencer.h"

#include "sm_aci_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_SM_CONTEXT_CONTROL_TRANSITION_FUNC)(struct T_SM_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_SM_CONTEXT_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_SM_CONTEXT_CONTROL_TRANSITION_FUNC  func;
} T_SM_CONTEXT_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void go_to_S5         (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void go_to_S15        (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_smreg_pdp_activate_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_smreg_pdp_activate_sec_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_smreg_pdp_activate_rej_res(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_smreg_pdp_modify_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_network_request_activation_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_request_activation_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_request_activation_override(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_request_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_network_activate_completed_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_activate_completed_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_activate_completed_in_S12(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_activate_rejected_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_activate_rejected_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_activate_rejected_in_S12(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_network_modify_completed_in_S7(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_modify_completed_in_S10(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_modify_rejected_in_S7(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_modify_rejected_in_S10(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_network_request_modify(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_network_request_modify_failed(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_user_plane_activated_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_user_plane_activated_in_S13(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_user_plane_error_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_user_plane_error_while_active(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_user_plane_modified_in_S8(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_user_plane_deactivated_in_S11(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_deactivate_completed_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_completed_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_completed_in_S14(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_completed_in_S15(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_context_upgrade(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void handle_context_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_context_deactivate_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_context_reactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_local_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

static void forward_network_deactivate_completed(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void forward_user_plane_deactivated(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/***********************************************************************
 * State/Transition Table
 */
static const T_SM_CONTEXT_CONTROL_TRANSITION
transition[SM_CONTEXT_CONTROL_NUMBER_OF_STATES][SM_CONTEXT_CONTROL_NUMBER_OF_EVENTS] =
{
  {  /* S0: SM_CONTEXT_DEACTIVATED */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              handle_smreg_pdp_activate_req),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          handle_smreg_pdp_activate_sec_req),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate_in_S0),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_context_deactivate_in_S0),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  handle_context_reactivate),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          handle_network_request_activation_in_S0),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              forward_user_plane_deactivated),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                handle_deactivate_completed_in_S0)
  },
  {  /* S1: SM_CONTEXT_ACTIVATING_MT */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              handle_smreg_pdp_activate_req),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          handle_smreg_pdp_activate_rej_res),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          handle_network_request_activation_in_S1),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S2: SM_CONTEXT_ACTIVATING_NETWORK */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          handle_network_activate_completed_in_S2),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           handle_network_activate_rejected_in_S2),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S3: SM_CONTEXT_ACTIVATING_USER_PLANE */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_in_S3),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                handle_user_plane_activated_in_S3),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S4: SM_CONTEXT_ACTIVATE_FAILED_DEACTIVATING */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        forward_network_deactivate_completed),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              forward_user_plane_deactivated),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                handle_deactivate_completed_in_S4)
  },
  {  /* S5: SM_CONTEXT_ACTIVATED */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                handle_smreg_pdp_modify_req),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     handle_context_upgrade),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, handle_network_request_activation_override),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          handle_network_request_deactivate),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              handle_network_request_modify),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       handle_network_request_modify_failed),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S6: SM_CONTEXT_ACTIVATION_OVERRIDE */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          handle_network_activate_completed_in_S6),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           handle_network_activate_rejected_in_S6),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, handle_network_request_activation_override),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S7: SM_CONTEXT_MODIFYING_IN_NETWORK */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             handle_network_modify_rejected_in_S7),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            handle_network_modify_completed_in_S7),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       handle_network_request_modify_failed),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S8: SM_CONTEXT_MODIFYING_USER_PLANE */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 handle_user_plane_modified_in_S8),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S9: SM_CONTEXT_MODIFYING_USER_PLANE_MT */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 go_to_S5),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S10: SM_CONTEXT_UPGRADING_IN_NETWORK */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             handle_network_modify_rejected_in_S10),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            handle_network_modify_completed_in_S10),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S11: SM_CONTEXT_REACTIVATING_DEACTIVATING_USER_PLANE */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              forward_user_plane_deactivated),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                handle_user_plane_deactivated_in_S11)
  },
  {  /* S12: SM_CONTEXT_REACTIVATING_IN_NETWORK */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          handle_network_activate_completed_in_S12),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           handle_network_activate_rejected_in_S12),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S13: SM_CONTEXT_REACTIVATING_USER_PLANE */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  handle_context_deactivate),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            handle_user_plane_error_while_active),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                handle_user_plane_activated_in_S13),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                state_event_error)
  },
  {  /* S14: SM_CONTEXT_DEACTIVATING */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  go_to_S15),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            state_event_error),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        forward_network_deactivate_completed),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              forward_user_plane_deactivated),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                handle_deactivate_completed_in_S14)
  },
  {  /* S15: SM_CONTEXT_DEACTIVATING_SEQUENCED */
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REQ,              state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_REJ_RES,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,          state_event_error),
    M_TRANSITION(SM_P_SMREG_PDP_MODIFY_REQ,                state_event_error),
    M_TRANSITION(SM_I_CONTEXT_DEACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_LOCAL_DEACTIVATE,            handle_local_deactivate),
    M_TRANSITION(SM_I_CONTEXT_REACTIVATE,                  state_event_error),
    M_TRANSITION(SM_I_CONTEXT_UPGRADE,                     state_event_error),
    M_TRANSITION(SM_I_CONTEXT_USER_PLANE_ERROR,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_COMPLETED,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_REJECTED,           state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_COMPLETED,        forward_network_deactivate_completed),
    M_TRANSITION(SM_I_NETWORK_MODIFY_REJECTED,             state_event_error),
    M_TRANSITION(SM_I_NETWORK_MODIFY_COMPLETED,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_DEACTIVATE,          state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_MODIFY_FAILED,       state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_ACTIVATED,                state_event_error),
    M_TRANSITION(SM_I_USER_PLANE_DEACTIVATED,              forward_user_plane_deactivated),
    M_TRANSITION(SM_I_USER_PLANE_MODIFIED,                 state_event_error),
    M_TRANSITION(SM_I_DEACTIVATE_COMPLETED,                handle_deactivate_completed_in_S15)
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
  (void)TRACE_ERROR("SM Context Control: STATE EVENT ERROR!");
}

/*
+------------------------------------------------------------------------------
| Function    : go_to_S*
+------------------------------------------------------------------------------
| Description : General function used for simple state transitions with no
|               actions.
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void go_to_S5(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  context->context_control_state = SM_CONTEXT_ACTIVATED;
}

static void go_to_S15(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  context->context_control_state = SM_CONTEXT_DEACTIVATING_SEQUENCED;
}

/*
+------------------------------------------------------------------------------
| Function    : forward_network_deactivate_completed,
|               forward_user_plane_deactivated
+------------------------------------------------------------------------------
| Description : Functions used for forwarding deactivation primitives to
|               Context Deactivate Control
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void forward_network_deactivate_completed(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK_COMPLETED, NULL);
}

static void forward_user_plane_deactivated(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE_COMPLETED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_smreg_pdp_activate_req
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SMREG_PDP_ACTIVATE_REQ in S0 and S1
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_ACTIVATE_REQ *
+------------------------------------------------------------------------------
*/
static void handle_smreg_pdp_activate_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_smreg_pdp_activate_req");

  /* Go to state SM CONTEXT ACTIVATING NETWORK */
  context->context_control_state = SM_CONTEXT_ACTIVATING_NETWORK;

  /* Start primary context activation towards network */
  sm_network_control(context, SM_I_NETWORK_ACTIVATE_PRIMARY, data);

  /* Inform User Plane Control of pending context activation */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_START, data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_smreg_pdp_activate_sec_req
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SMREG_PDP_ACTIVATE_SEC_REQ
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_ACTIVATE_SEC_REQ *
+------------------------------------------------------------------------------
*/
static void handle_smreg_pdp_activate_sec_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_smreg_pdp_activate_sec_req");

  /* Go to state SM CONTEXT ACTIVATING NETWORK */
  context->context_control_state = SM_CONTEXT_ACTIVATING_NETWORK;

  /* Start secondary context activation towards network */
  sm_network_control(context, SM_I_NETWORK_ACTIVATE_SECONDARY, data);

  /* Inform User Plane Control of pending context activation */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_START, data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_smreg_pdp_activate_rej_res
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SMREG_PDP_ACTIVATE_REJ_RES
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_ACTIVATE_REJ_RES *
+------------------------------------------------------------------------------
*/
static void handle_smreg_pdp_activate_rej_res(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SMREG_PDP_ACTIVATE_REJ_RES *prim = (T_SMREG_PDP_ACTIVATE_REJ_RES *)data;
  (void)TRACE_FUNCTION("handle_smreg_pdp_activate_rej_res");

  TRACE_ASSERT(prim != NULL);
  /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
  sm_set_nw_cause(context, prim->ps_cause.ctrl_value, prim->ps_cause.value.nwsm_cause);
  
  /* Reject MT activation attempt towards the network */
  sm_network_control(context, SM_I_NETWORK_REQUEST_ACTIVATION_REJECT, NULL);
  /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_smreg_pdp_modify_req
+------------------------------------------------------------------------------
| Description : Handle event SM_P_SMREG_PDP_MODIFY_REQ
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_MODIFY_REQ *
+------------------------------------------------------------------------------
*/
static void handle_smreg_pdp_modify_req(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SMREG_PDP_MODIFY_REQ *prim = (T_SMREG_PDP_MODIFY_REQ *)data;
  (void)TRACE_FUNCTION("handle_smreg_pdp_modify_req");

  TRACE_ASSERT(prim != NULL);

  /* Go to state SM CONTEXT MODIFYING IN NETWORK */
  context->context_control_state = SM_CONTEXT_MODIFYING_IN_NETWORK;

/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
  /* Start modification in network */
  sm_network_control(context, SM_I_NETWORK_MODIFY, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_activation_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_ACTIVATION in state S0
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_network_request_activation_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_network_request_activation_in_S0");

  /* Go to state SM CONTEXT ACTIVATING MT */
  context->context_control_state = SM_CONTEXT_ACTIVATING_MT;

  /* Send SMREG_PDP_ACTIVATE_IND to ACI */
  send_smreg_pdp_activate_ind(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_activation_in_S1
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_ACTIVATION in state S1
|
| Parameters  : context          - Context data
|               data             - T_REQUEST_PDP_CONTEXT_ACTIVATION *
+------------------------------------------------------------------------------
*/
static void handle_network_request_activation_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  T_REQUEST_PDP_CONTEXT_ACTIVATION *msg = (T_REQUEST_PDP_CONTEXT_ACTIVATION *)data;
  (void)TRACE_FUNCTION("handle_network_request_activation_in_S1");

  TRACE_ASSERT(msg != NULL);

  /* Check whether PDP type, APN, and address match */
  /*lint -e613 (Possible use of null pointer 'msg' in left argument to operator '->') */
  if (sm_nw_is_address_and_apn_equal(context, &context->requested_address,
                                     &msg->address, msg->v_apn, &msg->apn))
  {
    /* Ignore incoming request - proceed with current activation */
  } else {
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_INSUFFICIENT_RESOURCES);
    sm_network_control(context, SM_I_NETWORK_REQUEST_ACTIVATION_REJECT, NULL);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_activation_override
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE in S5
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_network_request_activation_override(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_network_request_activation_override");

  /* Go to state SM CONTEXT ACTIVATION OVERRIDE */
  context->context_control_state = SM_CONTEXT_ACTIVATION_OVERRIDE;

  /* Inform Sequencer of activation override */
  sm_sequencer(SM_I_CONTEXT_ACTIVATION_OVERRIDE, (void *)(T_NAS_ti)context->ti);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_deactivate
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_DEACTIVATE
|
| Parameters  : context          - Context data
|               data             - Tear-down indicator
+------------------------------------------------------------------------------
*/
static void handle_network_request_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  U32               tear_down    = (U32)data;
  T_CAUSE_ps_cause *cause        = sm_get_aci_cause(context);
  U16               nsapi_as_set = sm_nsapi2nsapi_set(context->nsapi);
  U16               nsapis_to_deactivate;

  (void)TRACE_FUNCTION("handle_network_request_deactivate");

  if (cause->ctrl_value       == CAUSE_is_from_nwsm &&
      cause->value.nwsm_cause == (U16)CAUSE_NWSM_REACTIVATION_REQUESTED)
  {
    /* Go to state SM CONTEXT REACTIVATING DEACTIVATING USER PLANE */
    context->context_control_state = SM_CONTEXT_REACTIVATING_DEACTIVATING_USER_PLANE;

    nsapis_to_deactivate = 0;
  } else {
    /* Go to state SM CONTEXT DEACTIVATED */
    context->context_control_state = SM_CONTEXT_DEACTIVATED;

    nsapis_to_deactivate = nsapi_as_set;
  }

  if (tear_down != 0)
  {
    nsapis_to_deactivate |= sm_linked_nsapis(context->ti);

    /* Inform Sequencer of tear-down deactivation */
    sm_sequencer(SM_I_CONTEXT_TEAR_DOWN_DEACTIVATE,
                 (void *)(T_NAS_ti)context->ti);

    /* Send SMREG_PDP_DEACTIVATE_IND for all context being deactivated */
    if (nsapis_to_deactivate != 0)
    {
      send_smreg_pdp_deactivate_ind(nsapis_to_deactivate,
                                    sm_get_aci_cause(context));
    }
  } else {
    /* Send SMREG_PDP_DEACTIVATE_IND for this single context. */
    if (nsapis_to_deactivate != 0)
    {
      send_smreg_pdp_deactivate_ind(nsapi_as_set, sm_get_aci_cause(context));
    }
  }

  /* Deactivate user plane */
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE,
                                (void *)PS_REL_IND_NO);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_completed_in_S2
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_COMPLETED in state S2
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_network_activate_completed_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_network_activate_completed_in_S2");

  /* Go to state SM CONTEXT ACTIVATING USER PLANE */
  context->context_control_state = SM_CONTEXT_ACTIVATING_USER_PLANE;

  /* Activate User Plane */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_COMPLETE, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_completed_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_COMPLETED in state S6
|
| Parameters  : context          - Context data
|               data             - (enum) T_SM_UPDATE_FLAGS
+------------------------------------------------------------------------------
*/
static void handle_network_activate_completed_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SM_UPDATE_FLAGS update_flags = (T_SM_UPDATE_FLAGS)((UINT)data);
  (void)TRACE_FUNCTION("handle_network_activate_completed_in_S6");

  if (sm_is_aci_update_required(update_flags))
  {
    /* Inform ACI of new parameters */
    send_smreg_pdp_modify_ind(context, update_flags);
  }

  if (sm_is_user_plane_update_required(update_flags))
  {
    /* Go to state SM CONTEXT MODIFYING USER PLANE MT */
    context->context_control_state = SM_CONTEXT_MODIFYING_USER_PLANE_MT;

    /* Modify User Plane */
    sm_user_plane_control(context, SM_I_USER_PLANE_MODIFY, NULL);
  } else {
    /* Go to state SM CONTEXT ACTIVATED */
    context->context_control_state = SM_CONTEXT_ACTIVATED;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_completed_in_S12
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_COMPLETED in state S12
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_network_activate_completed_in_S12(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_network_activate_completed_in_S12");

  /* Go to state SM CONTEXT REACTIVATING USER PLANE */
  context->context_control_state = SM_CONTEXT_REACTIVATING_USER_PLANE;

  /* Activate User Plane */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_COMPLETE, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_rejected_in_S2
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_REJECTED in state S2
|
| Parameters  : context          - Context data
|               data             - Deactivate network trigger (BOOL)
+------------------------------------------------------------------------------
*/
static void handle_network_activate_rejected_in_S2(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  BOOL  deactivate_network = (data != NULL ? TRUE : FALSE);
  (void)TRACE_FUNCTION("handle_network_activate_rejected_in_S2");

  /* Go to state SM CONTEXT ACTIVATE FAILED DEACTIVATING */
  context->context_control_state = SM_CONTEXT_ACTIVATE_FAILED_DEACTIVATING;

  if (deactivate_network)
  {
    /* Deactivate user plane.  Also, network is being deactivated;
     * Deactivation Control should anticipate NETWORK_DEACTIVATE_COMPLETED. */
    sm_context_deactivate_control(context,
                                  SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE,
                                  sm_get_nw_cause(context));
  } else {
    /* Dectivate User Plane */
    sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE,
                                  (void *)PS_REL_IND_NO);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_rejected_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_REJECTED in state S6
|
| Parameters  : context          - Context data
|               data             - Deactivate network trigger (BOOL)
+------------------------------------------------------------------------------
*/
static void handle_network_activate_rejected_in_S6(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_network_activate_rejected_in_S6");

  /* Inform ACI of failure and deactivation */
  send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi),
                                sm_get_aci_cause(context));

  /* Go to state SM CONTEXT DEACTIVATING */
  context->context_control_state = SM_CONTEXT_DEACTIVATING;

  /* Deactivate User Plane */
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE,
                                (void *)PS_REL_IND_NO);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_activate_rejected_in_S12
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_REJECTED in state S12
|
| Parameters  : context          - Context data
|               data             - Deactivate network trigger (BOOL)
+------------------------------------------------------------------------------
*/
static void handle_network_activate_rejected_in_S12(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_network_activate_rejected_in_S12");

  /* Inform ACI of failure and deactivation */
  send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi),
                                sm_get_aci_cause(context));

  /* Go to state SM CONTEXT DEACTIVATING */
  context->context_control_state = SM_CONTEXT_DEACTIVATING;

  /* Deactivate User Plane */
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE,
                                (void *)PS_REL_IND_NO);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_modify_rejected_in_S7
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_MODIFY_REJECTED in state S7
|
| Parameters  : context          - Context data
|               data             - BOOL (Deactivate context due to T3381)
+------------------------------------------------------------------------------
*/
static void handle_network_modify_rejected_in_S7(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  BOOL               deactivate_context = (BOOL)((UINT)data);
  T_CAUSE_ps_cause  *cause = sm_get_aci_cause(context);
  (void)TRACE_FUNCTION("handle_network_modify_rejected_in_S7");

  TRACE_ASSERT(cause != NULL);

  /*lint -e613 (Possible use of null pointer 'cause' in left argument to operator '->') */
  /* Inform ACI of failed modification attempt */
  send_smreg_pdp_modify_rej(context->nsapi, cause);

  /* In case of modify collisions or timeouts, the context remains active */
  if (deactivate_context       != TRUE &&
      (cause->ctrl_value       == CAUSE_is_from_sm &&
       cause->value.sm_cause   == (U16)CAUSE_SM_MODIFY_COLLISION
       ||
       cause->ctrl_value       == CAUSE_is_from_nwsm &&
       cause->value.nwsm_cause == (U16)CAUSE_NWSM_NETWORK_FAILURE))
  {
    /* Go to state SM CONTEXT ACTIVATED */
    context->context_control_state = SM_CONTEXT_ACTIVATED;
  } else {
    /* Other errors: Context is deactivated */
    send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi), cause);

    /* Go to state SM CONTEXT DEACTIVATING */
    context->context_control_state = SM_CONTEXT_DEACTIVATING;

    /* Deactivate Network and User Plane (Network deactivation in progress) */
    sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, cause);
    /*lint +e613 (Possible use of null pointer 'cause' in left argument to operator '->') */
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_modify_rejected_in_S10
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_MODIFY_REJECTED in state S10
|
| Parameters  : context          - Context data
|               data             - BOOL (Deactivate context due to T3381)
+------------------------------------------------------------------------------
*/
static void handle_network_modify_rejected_in_S10(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  BOOL               deactivate_context = (BOOL)((UINT)data);
  T_CAUSE_ps_cause  *cause = sm_get_aci_cause(context);
  (void)TRACE_FUNCTION("handle_network_modify_rejected_in_S10");

  TRACE_ASSERT(cause != NULL);

  /* In case of modify collision, timeout, or temporary out-of-resources
   * the context remains active */
 /*lint -e613 (Possible use of null pointer 'cause' in left argument to operator '->') */
  if (deactivate_context       != TRUE &&
      (cause->ctrl_value       == CAUSE_is_from_sm &&
       cause->value.sm_cause   == (U16)CAUSE_SM_MODIFY_COLLISION
       ||
       cause->ctrl_value       == CAUSE_is_from_nwsm &&
       cause->value.nwsm_cause == (U16)CAUSE_NWSM_NETWORK_FAILURE
       ||
       cause->ctrl_value       == CAUSE_is_from_nwsm &&
       cause->value.nwsm_cause == (U16)CAUSE_NWSM_INSUFFICIENT_RESOURCES))
  {
    /* Go to state SM CONTEXT ACTIVATED */
    context->context_control_state = SM_CONTEXT_ACTIVATED;
  } else {
    /* Other errors: Context is deactivated */
    send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi), cause);
    /*lint +e613 (Possible use of null pointer 'cause' in left argument to operator '->') */	

    /* Go to state SM CONTEXT DEACTIVATING */
    context->context_control_state = SM_CONTEXT_DEACTIVATING;

    /* Deactivate Network and User Plane (Network deactivation in progress) */ 
    sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, sm_get_nw_cause(context));
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_modify_completed_in_S7
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_MODIFY_COMPLETED in state S7
|
| Parameters  : context          - Context data
|               data             - T_SM_UPDATE_FLAGS
+------------------------------------------------------------------------------
*/
static void handle_network_modify_completed_in_S7(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SM_UPDATE_FLAGS  update_flags = (T_SM_UPDATE_FLAGS)((UINT)data);
  (void)TRACE_FUNCTION("handle_network_modify_completed_in_S7");

  if (sm_is_user_plane_update_required(update_flags))
  {
    /* Go to state SM CONTEXT MODIFYING USER PLANE */
    context->context_control_state = SM_CONTEXT_MODIFYING_USER_PLANE;

    /* Start user plane modification */
    sm_user_plane_control(context, SM_I_USER_PLANE_MODIFY, (void *)update_flags);
  } else {
    /* Inform ACI of the successful modification and altered fields */
    send_smreg_pdp_modify_cnf(context, update_flags);

    /* Go to state SM CONTEXT ACTIVATED */
    context->context_control_state = SM_CONTEXT_ACTIVATED;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_modify_completed_in_S10
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_MODIFY_COMPLETED in state S10
|
| Parameters  : context          - Context data
|               data             - (enum) T_SM_UPDATE_FLAGS
+------------------------------------------------------------------------------
*/
static void handle_network_modify_completed_in_S10(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SM_UPDATE_FLAGS update_flags = (T_SM_UPDATE_FLAGS)((UINT)data);
  (void)TRACE_FUNCTION("handle_network_modify_completed_in_S10");

  if (sm_is_aci_update_required(update_flags))
  {
    /* Inform ACI of new parameters */
    send_smreg_pdp_modify_ind(context, update_flags);
  }

  if (sm_is_user_plane_update_required(update_flags))
  {
    /* Go to state SM CONTEXT MODIFYING USER PLANE MT */
    context->context_control_state = SM_CONTEXT_MODIFYING_USER_PLANE_MT;

    /* Modify User Plane */
    sm_user_plane_control(context, SM_I_USER_PLANE_MODIFY, NULL);
  } else {
    /* Go to state SM CONTEXT ACTIVATED */
    context->context_control_state = SM_CONTEXT_ACTIVATED;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_activated_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ACTIVATED in state S3
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_user_plane_activated_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_activated_in_S3");

  /* Go to state SM CONTEXT ACTIVATED */
  context->context_control_state = SM_CONTEXT_ACTIVATED;

  /* Confirm activation towards ACI; response depends on primary/secondary */
  if (!sm_is_secondary(context))
  {
    /* Primary context: send SMREG_PDP_ACTIVATE_CNF */
    send_smreg_pdp_activate_cnf(context);
  } else {
    /* Secondary context: send SMREG_PDP_ACTIVATE_SEC_CNF */
    send_smreg_pdp_activate_sec_cnf(context);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_activated_in_S13
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ACTIVATED in state S13
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_user_plane_activated_in_S13(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_activated_in_S13");

  /* Go to state SM CONTEXT ACTIVATED */
  context->context_control_state = SM_CONTEXT_ACTIVATED;

  /* For primary contexts: Inform sequencer that reactivation was successful */
  if (!sm_is_secondary(context))
  {
    sm_sequencer(SM_I_CONTEXT_REACTIVATE_COMPLETED,
                 (void *)(T_NAS_ti)context->ti);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_error_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ERROR in state S3
|
| Parameters  : context          - Context data
|               data             - BOOL
+------------------------------------------------------------------------------
*/
static void handle_user_plane_error_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_error_in_S3");

  /* Go to state SM CONTEXT ACTIVATE FAILED DEACTIVATING */
  context->context_control_state = SM_CONTEXT_ACTIVATE_FAILED_DEACTIVATING;

  /* Dectivate User Plane locally */
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE_LOCAL, data);

  /* Deactivate towards network */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK,
                                sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_error_while_active
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_ERROR in states S5 - S13
|
| Parameters  : context          - Context data
|               data             - BOOL
+------------------------------------------------------------------------------
*/
static void handle_user_plane_error_while_active(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_error_while_active");

  /* Inform ACI that the context will now be deactivated due to user plane error */
  send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi),
                                sm_get_aci_cause(context));

  /* Go to state SM CONTEXT DEACTIVATING */
  context->context_control_state = SM_CONTEXT_DEACTIVATING;

  /* Dectivate User Plane locally */
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE_LOCAL, data);

  /* Deactivate towards network */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK,
                                sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_modified_in_S8
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_MODIFIED in states S8
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_user_plane_modified_in_S8(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_modified_in_S8");

  /* Inform ACI of the successful modification and altered QoS */
  send_smreg_pdp_modify_cnf(context, SM_UPDATE_QOS);

  /* Go to state SM CONTEXT ACTIVATED */
  context->context_control_state = SM_CONTEXT_ACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_user_plane_deactivated_in_S11
+------------------------------------------------------------------------------
| Description : Handle event SM_I_USER_PLANE_DEACTIVATED in S11
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_user_plane_deactivated_in_S11(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_user_plane_deactivated_in_S11");

  /* Go to state SM CONTEXT REACTIVATING IN NETWORK */
  context->context_control_state = SM_CONTEXT_REACTIVATING_IN_NETWORK;

  /* Start primary context activation towards network */
  sm_network_control(context, SM_I_NETWORK_ACTIVATE_PRIMARY, NULL);

  /* Inform User Plane Control of pending context activation */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_START, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_completed_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_COMPLETED in state S0
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_completed_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_completed_in_S0");

  /* Mark context for deallocation after call returns */
  sm_mark_context_for_deallocation(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_completed_in_S4
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_COMPLETED in state S4
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_completed_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_completed_in_S4");

  /* Go to state SM CONTEXT DEACTIVATED */
  context->context_control_state = SM_CONTEXT_DEACTIVATED;

  /* Report failed activation to ACI: Response depends on primary/secondary */
  if (!sm_is_secondary(context))
  {
    /* Primary context: send SMREG_PDP_ACTIVATE_REJ */
    send_smreg_pdp_activate_rej(context->nsapi, sm_get_aci_cause(context));
  } else {
    /* Secondary context: send SMREG_PDP_ACTIVATE_SEC_REJ */
    send_smreg_pdp_activate_sec_rej(context->nsapi, sm_get_aci_cause(context));
  }

  /* Mark context for deallocation after call returns */
  sm_mark_context_for_deallocation(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_completed_in_S14
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_COMPLETED in state S14
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_completed_in_S14(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_completed_in_S14");

  /* Go to state SM CONTEXT DEACTIVATED */
  context->context_control_state = SM_CONTEXT_DEACTIVATED;

  /* Mark context for deallocation */
  sm_mark_context_for_deallocation(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_completed_in_S15
+------------------------------------------------------------------------------
| Description : Handle event SM_I_DEACTIVATE_COMPLETED in state S15
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_completed_in_S15(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_completed_in_S15");

  /* Go to state SM CONTEXT DEACTIVATED */
  context->context_control_state = SM_CONTEXT_DEACTIVATED;

  /* Mark context for deallocation */
  sm_mark_context_for_deallocation(context);

  /* Confirm deactivation towards Sequencer */
  sm_sequencer(SM_I_CONTEXT_DEACTIVATE_COMPLETED, (void *)(T_NAS_nsapi)context->nsapi);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_modify
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_MODIFY in state S4
|
| Parameters  : context          - Context data
|               data             - Update flags
+------------------------------------------------------------------------------
*/
static void handle_network_request_modify(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SM_UPDATE_FLAGS  update_flags = (T_SM_UPDATE_FLAGS)((UINT)data);
  (void)TRACE_FUNCTION("handle_network_request_modify");

  if (sm_is_aci_update_required(update_flags))
  {
    /* Send SMREG_PDP_MODIFY_IND if required */
    send_smreg_pdp_modify_ind(context, update_flags);
  }

  if (sm_is_user_plane_update_required(update_flags))
  {
    /* Go to state SM CONTEXT MODIFYING USER PLANE MT */
    context->context_control_state = SM_CONTEXT_MODIFYING_USER_PLANE_MT;

    /* Update User Plane */
    sm_user_plane_control(context, SM_I_USER_PLANE_MODIFY, (void *)update_flags);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_network_request_modify_failed
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_MODIFY_FAILED
|
| Parameters  : context          - Context data
|               data             - Not used (cause set in context data)
+------------------------------------------------------------------------------
*/
static void handle_network_request_modify_failed(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_network_request_modify_failed");

  /* Inform ACI of failed modification and deactivation */
  send_smreg_pdp_deactivate_ind(sm_nsapi2nsapi_set(context->nsapi),
                                sm_get_aci_cause(context));

  /* Go to state SM CONTEXT DEACTIVATING */
  context->context_control_state = SM_CONTEXT_DEACTIVATING;

  /* Start user plane deactivation */
  sm_context_deactivate_control(context, SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE, sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_upgrade
+------------------------------------------------------------------------------
| Description : Handle event SM_I_CONTEXT_UPGRADE in S5
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_context_upgrade(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_context_upgrade");

  /* Go to state SM CONTEXT UPGRADING IN NETWORK */
  context->context_control_state = SM_CONTEXT_UPGRADING_IN_NETWORK;

  /* Start a network modify procedure in order to re-upgrade QoS */
  sm_network_control(context, SM_I_NETWORK_MODIFY, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_reactivate
+------------------------------------------------------------------------------
| Description : Handle event SM_I_CONTEXT_REACTIVATE in deactivate(d) states
|
| Parameters  : context          - Context data
|               data             - not used
+------------------------------------------------------------------------------
*/
void handle_context_reactivate(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_context_reactivate");

  /* Go to state SM CONTEXT DEACTIVATING SEQUENCED */
  context->context_control_state = SM_CONTEXT_REACTIVATING_IN_NETWORK;

  /* Start primary or secondary activation depending on context type */
  if (sm_is_secondary(context)) {
    sm_network_control(context, SM_I_NETWORK_ACTIVATE_SECONDARY, NULL);
  } else {
    sm_network_control(context, SM_I_NETWORK_ACTIVATE_PRIMARY, NULL);
  }
  /* (Re)start user plane */
  sm_user_plane_control(context, SM_I_USER_PLANE_ACTIVATE_START, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_deactivate_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_CONTEXT_DEACTIVATE in S0
|               Note: User Plane is deactivating in this state
|
| Parameters  : context          - Context data
|               data             - deactivation type
+------------------------------------------------------------------------------
*/
static void handle_context_deactivate_in_S0(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  int nsapi;
  
  T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT event = (T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT)((UINT)data);
  (void)TRACE_FUNCTION("SM Context Control: handle_context_deactivate_in_S0");

  if (event == SM_I_DEACTIVATE_USER_PLANE ||
      event == SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE)
  {
    /* Network deactivation ignored - already deactivated */
    sm_context_deactivate_control(context, SM_I_DEACTIVATE_USER_PLANE,
                                  (void *)PS_REL_IND_NO); 
    /*ACI requested deactivation of an already inactive context. Acknowledge*/
    send_smreg_pdp_deactivate_cnf(sm_data.sm_nsapis_requested_deactivated);

   /* If the context is not freed, clear it here so that future SMREG_PDP_ACTIVATE_REQ
     * served properly 
     */
    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      if (sm_is_nsapi_in_nsapi_set(nsapi, sm_data.sm_nsapis_requested_deactivated)) 
      {
        sm_free_context_data_by_nsapi(nsapi);
      }
    }
  }
  /* Network-only deactivation ignored - already deactivated */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_deactivate
+------------------------------------------------------------------------------
| Description : Handle event SM_I_CONTEXT_DEACTIVATE in active states
|
| Parameters  : context          - Context data
|               data             - deactivation type
+------------------------------------------------------------------------------
*/
void handle_context_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT event = (T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT)((UINT)data);
  void *param;

  (void)TRACE_FUNCTION("handle_context_deactivate");

  /* Go to state SM CONTEXT DEACTIVATING SEQUENCED */
  context->context_control_state = SM_CONTEXT_DEACTIVATING_SEQUENCED;

  /* Set cause to be sent to the network (if indicated by deactivation type) */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm,
                  (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);

  switch (event)
  {
  case SM_I_DEACTIVATE_USER_PLANE:
    param = (void *)PS_REL_IND_NO;
    break;
  case SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE:
  default:
    event = SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE;
    /*lint -fallthrough*/
  case SM_I_DEACTIVATE_NETWORK:
    param = sm_get_nw_cause(context);
    break;
  }

  /* Forward deactivation event to Context Deactivate Control */
  sm_context_deactivate_control(context, event, param);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_local_deactivate
+------------------------------------------------------------------------------
| Description : Handle event SM_I_CONTEXT_LOCAL_DEACTIVATE
|
| Parameters  : context          - Context data
|               data             - UPM signalling switch
+------------------------------------------------------------------------------
*/
static void handle_local_deactivate(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_local_deactivate");

  /* Go to state SM CONTEXT DEACTIVATED */
  context->context_control_state = SM_CONTEXT_DEACTIVATED;

  /* Locally deactivate Network Control */
  sm_network_control(context, SM_I_NETWORK_DEACTIVATE_LOCAL, NULL);

  /* Locally deactivate user plane (with UPM signalling) */
  sm_user_plane_control(context, SM_I_USER_PLANE_DEACTIVATE_LOCAL, data);

  /* Mark context for deallocation after call returns (unless context
   * is pending reactivation) */
  if (!sm_is_context_pending_reactivation(context)) {
    sm_mark_context_for_deallocation(context);
  }
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_context_control_init
+------------------------------------------------------------------------------
| Description : Context Control state machine initialization function

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
void sm_context_control_init(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_context_control_init");

  sm_set_context_pending_reactivation(context, FALSE);
  context->context_control_state = SM_CONTEXT_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_context_control_exit
+------------------------------------------------------------------------------
| Description : Context Control state machine exit function

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
void sm_context_control_exit(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_context_control_exit");

  sm_set_context_pending_reactivation(context, FALSE);
  context->context_control_state = SM_CONTEXT_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_context_control_state
+------------------------------------------------------------------------------
| Description : Returns a read-only string with the name of the active state.

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
#ifdef DEBUG
/*@observer@*/const char *
sm_context_control_state(struct T_SM_CONTEXT_DATA *context)
{
  /*@observer@*/
  static const char *state_name[SM_CONTEXT_CONTROL_NUMBER_OF_STATES] = {
    "S0_SM_CONTEXT_DEACTIVATED",
    "S1_SM_CONTEXT_ACTIVATING_MT",
    "S2_SM_CONTEXT_ACTIVATING_NETWORK",
    "S3_SM_CONTEXT_ACTIVATING_USER_PLANE",
    "S4_SM_CONTEXT_ACTIVATE_FAILED_DEACTIVATING",
    "S5_SM_CONTEXT_ACTIVATED",
    "S6_SM_CONTEXT_ACTIVATION_OVERRIDE",
    "S7_SM_CONTEXT_MODIFYING_IN_NETWORK",
    "S8_SM_CONTEXT_MODIFYING_USER_PLANE",
    "S9_SM_CONTEXT_MODIFYING_USER_PLANE_MT",
    "S10_SM_CONTEXT_UPGRADING_IN_NETWORK",
    "S11_SM_CONTEXT_REACTIVATING_DEACTIVATING_USER_PLANE",
    "S12_SM_CONTEXT_REACTIVATING_IN_NETWORK",
    "S13_SM_CONTEXT_REACTIVATING_USER_PLANE",
    "S14_SM_CONTEXT_DEACTIVATING",
    "S15_SM_CONTEXT_DEACTIVATING_SEQUENCED"
  };

  TRACE_ASSERT(context->context_control_state < SM_CONTEXT_CONTROL_NUMBER_OF_STATES);

  return state_name[(U16)context->context_control_state];
}
#endif
/*
+------------------------------------------------------------------------------
| Function    : sm_context_control
+------------------------------------------------------------------------------
| Description : Context Control state machine
|
| Parameters  : context          - Context data
|               event            - Internal event (see sm_context_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void sm_context_control(struct T_SM_CONTEXT_DATA *context,
                           T_SM_CONTEXT_CONTROL_EVENT event,
                           /*@null@*/ void *data)
{
#ifdef DEBUG
  const char *old_state_name, *new_state_name;
  T_SM_CONTEXT_CONTROL_STATE old_state;
  /*@observer@*/
  static const char *event_name[SM_CONTEXT_CONTROL_NUMBER_OF_EVENTS] = {
    "SM_P_SMREG_PDP_ACTIVATE_REQ",
    "SM_P_SMREG_PDP_ACTIVATE_REJ_RES",
    "SM_P_SMREG_PDP_ACTIVATE_SEC_REQ",
    "SM_P_SMREG_PDP_MODIFY_REQ",
    "SM_I_CONTEXT_DEACTIVATE",
    "SM_I_CONTEXT_LOCAL_DEACTIVATE",
    "SM_I_CONTEXT_REACTIVATE",
    "SM_I_CONTEXT_UPGRADE",
    "SM_I_CONTEXT_USER_PLANE_ERROR",
    "SM_I_NETWORK_ACTIVATE_COMPLETED",
    "SM_I_NETWORK_ACTIVATE_REJECTED",
    "SM_I_NETWORK_DEACTIVATE_COMPLETED",
    "SM_I_NETWORK_MODIFY_REJECTED",
    "SM_I_NETWORK_MODIFY_COMPLETED",
    "SM_I_NETWORK_REQUEST_ACTIVATION",
    "SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE",
    "SM_I_NETWORK_REQUEST_DEACTIVATE",
    "SM_I_NETWORK_REQUEST_MODIFY",
    "SM_I_NETWORK_REQUEST_MODIFY_FAILED",
    "SM_I_USER_PLANE_ACTIVATED",
    "SM_I_USER_PLANE_DEACTIVATED",
    "SM_I_USER_PLANE_MODIFIED",
    "SM_I_DEACTIVATE_COMPLETED"
  };

  TRACE_ASSERT(event < SM_CONTEXT_CONTROL_NUMBER_OF_EVENTS);

  old_state = context->context_control_state;
  old_state_name = sm_context_control_state(context);

  if (transition[(U16)old_state][(U16)event].event != event)
  {
    (void)TRACE_ERROR("Event table error in sm_context_control!");
  }
#endif /* DEBUG */

  transition[(U16)context->context_control_state][(U16)event].func(context, data);

#ifdef DEBUG
  new_state_name = sm_context_control_state(context);

  (void)TRACE_EVENT_P4("SM CONTEXT #%d: %s => %s to %s", context->nsapi,
                       event_name[(U16)event], old_state_name, new_state_name);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
