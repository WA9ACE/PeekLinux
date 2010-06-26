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
| Purpose:    Network Control state machine implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_network_control.h"
#include "sm_context_control.h"

#include "sm_mm_output_handler.h"

#include "sm_qos.h"
#include "sm_tft.h"
#include "sm_timer_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_SM_NETWORK_CONTROL_TRANSITION_FUNC)(struct T_SM_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_SM_NETWORK_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_SM_NETWORK_CONTROL_TRANSITION_FUNC  func;
} T_SM_NETWORK_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void ignored          (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void message_incompatible_with_state(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void message_for_invalid_ti(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/* Handler functions for internal events */
static void handle_activate_primary(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_secondary(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_request_activation_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_during_activation(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_during_modification(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_local(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/* Handler functions for incoming air interface messages */
static void handle_activate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_pdp_context_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_secondary_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_activate_secondary_pdp_context_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_request_pdp_context_activation_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_request_pdp_context_activation_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_request_pdp_context_activation_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_request_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_request_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_request_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_request_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_accept_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_accept_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_accept_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_reject_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_reject_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_modify_pdp_context_reject_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_pdp_context_request_while_active(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_pdp_context_request_while_deactivating(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_deactivate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
/* Handler functions for timer timeouts */
static void handle_T3380(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3380_max  (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3381      (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3381_max_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3381_max_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3381_max_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3390      (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_T3390_max  (struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
/* Handler functions for incoming primitives */
static void handle_mmpm_suspend_ind(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);
static void handle_mmpm_resume_ind(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data);

/***********************************************************************
 * State/Transition Table
 */
static const T_SM_NETWORK_CONTROL_TRANSITION
transition[SM_NETWORK_CONTROL_NUMBER_OF_STATES][SM_NETWORK_CONTROL_NUMBER_OF_EVENTS] =
{
  {  /* S0: SM NETWORK DEACTIVATED */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_for_invalid_ti),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_for_invalid_ti),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_for_invalid_ti),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_for_invalid_ti),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        state_event_error),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         state_event_error),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            message_for_invalid_ti),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             message_for_invalid_ti),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             message_for_invalid_ti),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        handle_request_pdp_context_activation_in_S0),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       ignored),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 state_event_error),
    M_TRANSITION(SM_T_T3381_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              handle_activate_primary),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            handle_activate_secondary),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     handle_request_activation_reject),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_in_S0)
  },
  {  /* S1: SM NETWORK ACTIVATING PRIMARY */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           handle_activate_pdp_context_accept),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           handle_activate_pdp_context_reject),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        handle_request_pdp_context_activation_in_S1),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 handle_T3380),
    M_TRANSITION(SM_T_T3380_MAX,                             handle_T3380_max),
    M_TRANSITION(SM_T_T3381,                                 state_event_error),
    M_TRANSITION(SM_T_T3381_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_during_activation)
  },
  {  /* S2: SM NETWORK ACTIVATING SECONDARY */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, handle_activate_secondary_pdp_context_accept),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, handle_activate_secondary_pdp_context_reject),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        message_incompatible_with_state),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 handle_T3380),
    M_TRANSITION(SM_T_T3380_MAX,                             handle_T3380_max),
    M_TRANSITION(SM_T_T3381,                                 state_event_error),
    M_TRANSITION(SM_T_T3381_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_during_activation)
  },
  {  /* S3: SM NETWORK ACTIVATING ADDING TFTS */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            handle_modify_pdp_context_request_in_S3),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             handle_modify_pdp_context_accept_in_S3),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             handle_modify_pdp_context_reject_in_S3),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        message_incompatible_with_state),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 handle_T3381),
    M_TRANSITION(SM_T_T3381_MAX,                             handle_T3381_max_in_S3),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_during_modification)
  },
  {  /* S4: SM NETWORK ACTIVATED */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            handle_modify_pdp_context_request_in_S4),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        handle_request_pdp_context_activation_in_S4),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       ignored),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 state_event_error),
    M_TRANSITION(SM_T_T3381_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        handle_modify_in_S4),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_in_S4)
  },
  {  /* S5: SM NETWORK MODIFYING */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            handle_modify_pdp_context_request_in_S5),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             handle_modify_pdp_context_accept_in_S5),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             handle_modify_pdp_context_reject_in_S5),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        message_incompatible_with_state),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 handle_T3381),
    M_TRANSITION(SM_T_T3381_MAX,                             handle_T3381_max_in_S5),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_during_modification)
  },
  {  /* S6: SM NETWORK MODIFYING ADDITIONAL TFTS */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_active),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            handle_modify_pdp_context_request_in_S6),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             handle_modify_pdp_context_accept_in_S6),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             handle_modify_pdp_context_reject_in_S6),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        message_incompatible_with_state),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 handle_T3381),
    M_TRANSITION(SM_T_T3381_MAX,                             handle_T3381_max_in_S6),
    M_TRANSITION(SM_T_T3390,                                 state_event_error),
    M_TRANSITION(SM_T_T3390_MAX,                             state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    handle_deactivate_during_modification)
  },
  {  /* S7: SM NETWORK DEACTIVATING */
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_PDP_CONTEXT_REJECT,           message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, message_incompatible_with_state),
    M_TRANSITION(SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, message_incompatible_with_state),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,        handle_deactivate_pdp_context_request_while_deactivating),
    M_TRANSITION(SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,         handle_deactivate_pdp_context_accept),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REQUEST,            message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_ACCEPT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_MODIFY_PDP_CONTEXT_REJECT,             message_incompatible_with_state),
    M_TRANSITION(SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,        message_incompatible_with_state),
    M_TRANSITION(SM_M_SM_STATUS,                             state_event_error),
    M_TRANSITION(SM_P_MMPM_RESUME_IND,                       handle_mmpm_resume_ind),
    M_TRANSITION(SM_P_MMPM_SUSPEND_IND,                      handle_mmpm_suspend_ind),
    M_TRANSITION(SM_T_T3380,                                 state_event_error),
    M_TRANSITION(SM_T_T3380_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3381,                                 state_event_error),
    M_TRANSITION(SM_T_T3381_MAX,                             state_event_error),
    M_TRANSITION(SM_T_T3390,                                 handle_T3390),
    M_TRANSITION(SM_T_T3390_MAX,                             handle_T3390_max),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_PRIMARY,              state_event_error),
    M_TRANSITION(SM_I_NETWORK_ACTIVATE_SECONDARY,            state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE_LOCAL,              handle_deactivate_local),
    M_TRANSITION(SM_I_NETWORK_MODIFY,                        state_event_error),
    M_TRANSITION(SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,     state_event_error),
    M_TRANSITION(SM_I_NETWORK_DEACTIVATE,                    ignored)
  }
};

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_mark_context_active
+------------------------------------------------------------------------------
| Description : Mark context in network state ACTIVE
|
| Parameters  : nsapi              - context index
+------------------------------------------------------------------------------
*/
static void sm_nw_mark_context_active(int /*@alt U8@*/ nsapi)
{
  sm_data.sm_context_activation_status
    = sm_add_nsapi_to_nsapi_set(nsapi, sm_data.sm_context_activation_status);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_mark_context_inactive
+------------------------------------------------------------------------------
| Description : Mark context in network state INACTIVE
|
| Parameters  : nsapi              - context index
+------------------------------------------------------------------------------
*/
static void sm_nw_mark_context_inactive(int /*@alt U8@*/ nsapi)
{
  sm_data.sm_context_activation_status
    = sm_remove_nsapi_from_nsapi_set(nsapi, sm_data.sm_context_activation_status);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_mark_nsapi_set_inactive
+------------------------------------------------------------------------------
| Description : Mark a set of contexts in network state INACTIVE
|
| Parameters  : nsapi_set          - Set of NSAPIs to mark inactive
+------------------------------------------------------------------------------
*/
static void sm_nw_mark_nsapi_set_inactive(U16 nsapi_set)
{
  sm_data.sm_context_activation_status &= 0xffff ^ nsapi_set;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_get_next_free_ti
+------------------------------------------------------------------------------
| Description : Traverse sm_context_array and find the lowest unused TI.
|
| Parameters  : context             - context that needs TI
+------------------------------------------------------------------------------
*/
static U8 sm_nw_get_next_free_ti(struct T_SM_CONTEXT_DATA *context_to_get_ti)
{
  int nsapi;
  U16 ti_mask = (U16)0x07ff;

  /* Iterate through sm_context_array and find the lowest available TI */
  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && context != context_to_get_ti
        && (context->ti & (U8)SM_TI_FLAG) == (U8)0 )
    {
      ti_mask &= 0x7ff ^ (1 << context->ti);
    }
  }

  if (ti_mask != 0)
  {
    int index;
    for (index = 0;
         index < SM_MAX_NSAPI_OFFSET && (ti_mask & (1UL << (U16)index)) == 0;
         index++)
      {};
    return (U8)index;
  } else {
    /* No TIs available */
    return (U8)0;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_initiate_deactivation
+------------------------------------------------------------------------------
| Description : General function for aborting a procedure and deactivating
|
| Parameters  : context          - context data
+------------------------------------------------------------------------------
*/
static void sm_nw_initiate_deactivation(struct T_SM_CONTEXT_DATA *context,
                                        T_CAUSE_ps_cause *ps_cause)
{
  /* Stop any active timer and start T3390 in stead */
  sm_timer_stop(context);
  sm_timer_start(context, SM_TIMER_T3390);

  /* Send message DEACTIVATE PDP CONTEXT REQUEST */
  send_msg_deactivate_pdp_context_request(context, ps_cause, FALSE);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_store_mt_modify_parameters
+------------------------------------------------------------------------------
| Description : Examines a T_D_MODIFY_PDP_CONTEXT_REQUEST message and stores
|               the parameters included.  Returns flags which indicate the
|               parameters that were updated.
|
| Parameters  : context          - context data
|               msg              - T_D_MODIFY_PDP_CONTEXT_REQUEST
+------------------------------------------------------------------------------
*/
static T_SM_UPDATE_FLAGS
sm_nw_store_mt_modify_parameters(struct T_SM_CONTEXT_DATA *context,
                                 T_D_MODIFY_PDP_CONTEXT_REQUEST *msg)
{
  T_SM_UPDATE_FLAGS  update_flags;
  sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);

  update_flags             = SM_UPDATE_QOS;

  /* Store GSM RAT parameters, if present */
  if (context->radio_prio != msg->radio_prio.radio_prio_val ||
      context->sapi       != msg->llc_sapi.sapi)
  {
    context->radio_prio    = msg->radio_prio.radio_prio_val;
    context->sapi          = msg->llc_sapi.sapi;
    update_flags          |= SM_UPDATE_SAPI_RADIO_PRIO_PFI;
  }

  /* Store PFI, if present */
  if (msg->v_pfi != (U8)FALSE && msg->pfi.pfi_val != context->pfi)
  {
    sm_set_pfi_included(context, TRUE);
    context->pfi           = msg->pfi.pfi_val;
    update_flags          |= SM_UPDATE_SAPI_RADIO_PRIO_PFI;
  }

  /* IP address "modified" (== assigned late)? Store, if so. */
  if (msg->v_address != (U8)FALSE)
  {
    sm_nw_store_negotiated_address(context, &msg->address, msg->v_address);
    update_flags          |= SM_UPDATE_ADDRESS;
  }

  return update_flags;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_store_modify_accept_parameters
+------------------------------------------------------------------------------
| Description : Examines a T_D_MODIFY_PDP_CONTEXT_ACCEPT message and stores
|               the parameters included.  Returns flags which indicate the
|               parameters that were updated.
|
| Parameters  : context          - context data
|               msg              - T_D_MODIFY_PDP_CONTEXT_REQUEST
+------------------------------------------------------------------------------
*/
static T_SM_UPDATE_FLAGS
sm_nw_store_modify_accept_parameters(struct T_SM_CONTEXT_DATA *context,
                                     T_D_MODIFY_PDP_CONTEXT_ACCEPT *msg)
{
  T_SM_UPDATE_FLAGS  update_flags = (T_SM_UPDATE_FLAGS)0;

  /* Store GSM RAT parameters, if present */
  if (msg->v_llc_sapi != (U8)FALSE)
  {
    context->sapi       = msg->llc_sapi.sapi;
    update_flags        = SM_UPDATE_SAPI_RADIO_PRIO_PFI;
  }

  if (msg->v_radio_prio != (U8)FALSE &&
      msg->radio_prio.radio_prio_val != context->radio_prio)
  {
    context->radio_prio = msg->radio_prio.radio_prio_val;
    update_flags        = SM_UPDATE_SAPI_RADIO_PRIO_PFI;
  }

  /* Store PFI, if present */
  if (msg->v_pfi != (U8)FALSE &&
      msg->pfi.pfi_val != context->pfi)
  {
    sm_set_pfi_included(context, TRUE);
    context->pfi         = msg->pfi.pfi_val;
    update_flags         = SM_UPDATE_SAPI_RADIO_PRIO_PFI;
  }

  return update_flags;
}

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
  (void)TRACE_ERROR("SM Network Control: STATE EVENT ERROR!");
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
  (void)TRACE_FUNCTION("SM Network Control: Event ignored.");
}

/*
+------------------------------------------------------------------------------
| Function    : message_incompatible_with_state
+------------------------------------------------------------------------------
| Description : General function used for reporting illegal air interface
|               messages in the current state
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void message_incompatible_with_state(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("message_incompatible_with_state");

  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_MSG_TYPE_INCOMPATIBLE_WITH_STATE);
  send_msg_sm_status(context->ti, sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : message_for_invalid_ti
+------------------------------------------------------------------------------
| Description : General function used for replying to network messages
|               received for a context in state S0 (invalid TI)
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void message_for_invalid_ti(/*@unused@*/ struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("message_for_invalid_ti");

  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_INVALID_TI);
  send_msg_sm_status(context->ti, sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_primary
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_PRIMARY in S0
|
| Parameters  : context          - Context data
|               data             - SMREG_PDP_ACTIVATE_REQ *
+------------------------------------------------------------------------------
*/
static void handle_activate_primary(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SMREG_PDP_ACTIVATE_REQ *prim = (T_SMREG_PDP_ACTIVATE_REQ *)data;

  (void)TRACE_FUNCTION("handle_activate_primary");

  if (prim != NULL)
  {
    memcpy(&context->comp_params, &prim->comp_params, sizeof(T_NAS_comp_params));

    sm_qos_copy_to_sm(&context->requested_qos, &prim->qos, prim->ctrl_qos);
    sm_qos_copy_to_sm(&context->minimum_qos, (T_PS_qos *)&prim->min_qos,
                      (T_PS_ctrl_qos)prim->ctrl_min_qos);

    sm_nw_store_requested_address(context, prim->pdp_type,
                                  prim->ctrl_ip_address, &prim->ip_address);

    /* Free APN (necessary if this is a network requested activation) */
    sm_nw_free_apn(context);

    /* Store APN for negotiation, if present */
    if (prim->apn.c_apn_buf > (U8)0)
    {
      sm_nw_allocate_and_copy_apn(context, prim->apn.c_apn_buf, prim->apn.apn_buf);
    }

    /* Store TFT for later addition, if present */
    TRACE_ASSERT(context->requested_tft.ptr_tft_pf == NULL);
    if (prim->v_tft != (U8)FALSE && prim->tft.c_tft_pf > (U8)0)
    {
      sm_nw_allocate_and_copy_requested_tft(context, &prim->tft);
    } else {
      context->requested_tft.c_tft_pf = (U8)0;
    }

    /* Store PCO for negotiation, if present */
    sm_nw_free_requested_pco(context);
    if (prim->sdu.l_buf > 0)
    {
      U16 pco_len = prim->sdu.l_buf >> 3;
      sm_nw_allocate_and_copy_requested_pco(context, pco_len,
                                            &prim->sdu.buf[(prim->sdu.o_buf >> 3)]);
    }

    /* Allocate TI for context unless this was a NW initiated activation */
    if ((context->ti & SM_TI_FLAG) == (U8)0)
    {
      context->ti = sm_nw_get_next_free_ti(context);
    }
  } else {
    /* No SMREG primitive: This is a re-activation.  Use current parameters. */ 
    /* The following function call can cause problems in reactivation cases 
     * where the first pdp context uses dynamic addressing. When the following
     * function is called the 'reactivated' context uses address issued by the 
     * network for the first pdp activation. This will result in static addressing
     * and may cause pdp activation reject from networks that doesn't support 
     * static addressing.
     */
    /*sm_nw_copy_negotiated_address_to_requested(context);*/
  }

  /* Mark context as active */
  sm_nw_mark_context_active(context->nsapi);

  /* Start timer T3380 for this NSAPI/context */
  sm_timer_start(context, SM_TIMER_T3380);

  /* Send ACTIVATE PDP CONTEXT REQUEST message */
  send_msg_activate_pdp_context_request(context);

  /* Go to state SM NETWORK ACTIVATING PRIMARY */
  context->network_control_state = SM_NETWORK_ACTIVATING_PRIMARY;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_secondary
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_ACTIVATE_SECONDARY in S0
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_ACTIVATE_SEC_REQ *
+------------------------------------------------------------------------------
*/
static void handle_activate_secondary(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SMREG_PDP_ACTIVATE_SEC_REQ *prim = (T_SMREG_PDP_ACTIVATE_SEC_REQ *)data;

  (void)TRACE_FUNCTION("handle_activate_secondary");

  if (prim != NULL)
  {
    memcpy(&context->comp_params, &prim->comp_params, sizeof(T_NAS_comp_params));

    sm_qos_copy_to_sm(&context->requested_qos, &prim->qos, prim->ctrl_qos);
    sm_qos_copy_to_sm(&context->minimum_qos, (T_PS_qos *)&prim->min_qos,
                      (T_PS_ctrl_qos)prim->ctrl_min_qos);

    /* Store TFT for later addition, if present */
    sm_nw_free_requested_tft(context);
    if (prim->v_tft != (U8)FALSE && prim->tft.c_tft_pf > (U8)0)
    {
      sm_nw_allocate_and_copy_requested_tft(context, &prim->tft);
    } else {
      context->requested_tft.c_tft_pf = (U8)0;
    }
    /* Allocate TI for context */
    context->ti = sm_nw_get_next_free_ti(context);
  } else {
    /* No SMREG primitive: This is a re-activation.  Use current parameters. */
  }

  /* Mark context as active */
  sm_nw_mark_context_active(context->nsapi);

  /* Start timer T3380 for this NSAPI/context */
  sm_timer_start(context, SM_TIMER_T3380);

  /* Send ACTIVATE SECONDARY PDP CONTEXT REQUEST message */
  send_msg_activate_secondary_pdp_context_request(context);

  /* Go to state SM NETWORK ACTIVATING SECONDARY */
  context->network_control_state = SM_NETWORK_ACTIVATING_SECONDARY;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_request_activation_reject
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_REQUEST_ACTIVATION_REJECT in S0
|
| Parameters  : context          - Context data
|               data             - Not used (cause set in context data)
+------------------------------------------------------------------------------
*/
static void handle_request_activation_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_request_activation_reject");

  /* Send REQUEST PDP CONTEXT ACTIVATION REJECT message */
  send_msg_request_pdp_context_activation_reject(context, sm_get_nw_cause(context));
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_in_S4
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_MODIFY in S4
|
| Parameters  : context          - Context data
|               data             - T_SMREG_PDP_MODIFY_REQ *
+------------------------------------------------------------------------------
*/
static void handle_modify_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_SMREG_PDP_MODIFY_REQ *prim = (T_SMREG_PDP_MODIFY_REQ *)data;
  BOOL                    start_modification = FALSE;

  (void)TRACE_FUNCTION("handle_modify_in_S4");

  /* If prim == NULL this is a context upgrade (RAB re-establishment) */
  if (prim != NULL)
  {
    if (prim->ctrl_qos != PS_is_qos_not_present)
    {
      sm_qos_copy_to_sm(&context->requested_qos, &prim->qos, prim->ctrl_qos);
      start_modification          = TRUE;
    }

    if (prim->ctrl_min_qos != PS_is_min_qos_not_present)
    {
      sm_qos_copy_to_sm(&context->minimum_qos, (T_PS_qos *)&prim->min_qos,
                        (T_PS_ctrl_qos)prim->ctrl_min_qos);
      if (!sm_qos_is_minimum_satisfied_by_sm(context, &context->accepted_qos))
      {
        start_modification        = TRUE;
      }
    }

    /* Store TFT for later addition, if present */
    if (prim->v_tft != (U8)FALSE)
    {
      if (prim->tft.c_tft_pf > (U8)0)
      {
        sm_nw_allocate_and_copy_requested_tft(context, &prim->tft);
      } else {
        context->requested_tft.c_tft_pf = (U8)0;
      }
      if (sm_tft_more_to_modify(context))
      {
        start_modification        = TRUE;
      }
    }
  } else {
    /* Prim == NULL: context upgrade (RAB re-establishment) */
    /* Start modification with originally requested parameters */
    start_modification            = TRUE;
  }

  if (start_modification)
  {
    /* Start timer T3381 for this NSAPI/context */
    sm_timer_start(context, SM_TIMER_T3381);

    /* Send MODIFY PDP CONTEXT REQUEST message */
    send_msg_modify_pdp_context_request(context);

    /* Go to state SM NETWORK MODIFYING */
    context->network_control_state = SM_NETWORK_MODIFYING;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_DEACTIVATE in S0
|
| Parameters  : context          - Context data
|               data             - rel_ind flag
+------------------------------------------------------------------------------
*/
static void handle_deactivate_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_in_S0");

  /* Report success deactivation to Context Control (already inactive) */
  sm_context_control(context, SM_I_NETWORK_DEACTIVATE_COMPLETED, data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_during_activation
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_DEACTIVATE in S1 or S2
|
| Parameters  : context          - Context data
|               data             - Cause (T_CAUSE_ps_cause *)
+------------------------------------------------------------------------------
*/
static void handle_deactivate_during_activation(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_during_activation");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Stop T3380, and start deactivation */
  sm_nw_initiate_deactivation(context, (T_CAUSE_ps_cause *)data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_in_S4
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_DEACTIVATE in S4
|
| Parameters  : context          - Context data
|               data             - Cause (T_CAUSE_ps_cause *)
+------------------------------------------------------------------------------
*/
static void handle_deactivate_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_in_S4");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Initiate deactivation; data contains cause */
  sm_nw_initiate_deactivation(context, (T_CAUSE_ps_cause *)data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_during_modification
+------------------------------------------------------------------------------
| Description : Handle event SM_I_NETWORK_DEACTIVATE in S3, S5, or S6
|
| Parameters  : context          - Context data
|               data             - Cause (T_CAUSE_ps_cause *)
+------------------------------------------------------------------------------
*/
static void handle_deactivate_during_modification(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_during_modification");

  TRACE_ASSERT(data != NULL);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Initiate deactivation; data contains cause */
  sm_nw_initiate_deactivation(context, (T_CAUSE_ps_cause *)data);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_local
+------------------------------------------------------------------------------
| Description : Handle SM_I_NETWORK_DEACTIVATE_LOCAL
|
| Parameters  : context          - context data
|               data             - unused
+------------------------------------------------------------------------------
*/
static void handle_deactivate_local(struct T_SM_CONTEXT_DATA *context, /*@unused@*/ /*@null@*/ void *data)
{
  /* Remove active TFT - this is necessary in case of reactivations */
  sm_nw_free_active_tft(context);

  /* Free stored coded message - if any */
  sm_free_coded_msg(context);

  /* Mark context as inactive */
  sm_nw_mark_context_inactive(context->nsapi);

  context->network_control_state = SM_NETWORK_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_pdp_context_accept
+------------------------------------------------------------------------------
| Description : Handle event SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT in S1
|
| Parameters  : context          - Context data
|               data             - T_ACTIVATE_PDP_CONTEXT_ACCEPT *
+------------------------------------------------------------------------------
*/
static void handle_activate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_ACTIVATE_PDP_CONTEXT_ACCEPT *msg = (T_ACTIVATE_PDP_CONTEXT_ACCEPT *)data;
  T_SM_UPDATE_FLAGS     update_flags = (T_SM_UPDATE_FLAGS)0; 
  BOOL  is_address_changed = FALSE;

  (void)TRACE_FUNCTION("handle_activate_pdp_context_accept");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3380 */
  sm_timer_stop(context);

  if (!sm_is_llc_sapi_valid(msg->llc_sapi.sapi, context->ti))
  {
    return;
  }
   
  is_address_changed = sm_is_address_changed_with_reactivation(context, 
                                               &msg->address, msg->v_address);
  /* Check whether negotiated QoS satisfies minimum QoS */
  if ( sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos) && 
                                         !(is_address_changed) )
  {
    if (context->accepted_qos.ctrl_qos != PS_is_qos_not_present)
    {
      update_flags |= SM_UPDATE_QOS;
    }

    /* QoS >= minimum QoS: Store negotiated parameters */
    sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);

    /* Store GSM RAT parameters */
    context->radio_prio        = msg->radio_prio.radio_prio_val;
    context->sapi              = msg->llc_sapi.sapi;

    /* Store PFI, if present */
    if (msg->v_pfi != (U8)FALSE)
    {
      sm_set_pfi_included(context, TRUE);
      context->pfi             = msg->pfi.pfi_val;
    } else {
      sm_set_pfi_included(context, FALSE);
    }

    /* Store negotiated IP address, if any */
    sm_nw_store_negotiated_address(context, &msg->address, msg->v_address);

    /* Store negotiated PCO, if any */
    if (msg->v_pco != (U8)FALSE && msg->pco.c_pco_value > (U8)0)
    {
      sm_nw_allocate_and_copy_negotiated_pco(context, msg->pco.c_pco_value,
                                             msg->pco.pco_value);
    } else {
      context->negotiated_pco     = NULL;
    }

    /* Check whether to add TFT */
    if (sm_tft_more_to_modify(context))
    {
      /* Start T3381 */
      sm_timer_start(context, SM_TIMER_T3381);
      /* Send MODIFY PDP CONTEXT REQUEST with remaining TFTs */
      send_msg_modify_pdp_context_request(context);
      /* Go to state SM NETWORK ACTIVATING ADDING TFTS */
      context->network_control_state = SM_NETWORK_ACTIVATING_ADDING_TFTS;
    } else {
      /* No TFT: go to state SM NETWORK ACTIVATED, and inform Context Control */
      context->network_control_state = SM_NETWORK_ACTIVATED;

      sm_context_control(context, SM_I_NETWORK_ACTIVATE_COMPLETED,
                         (void *)update_flags);
    }
  } else {  /* if (sm_qos_is_minimum_satisfied_by_aim()) */
    /* Minimum QoS not satisfied - Start deactivation, T3390 */
    sm_timer_start(context, SM_TIMER_T3390);

    /* Send DEACTIVATE PDP CONTEXT REQUEST message */
    if (is_address_changed) {
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, 
                               (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
    }
    else {
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    }
    send_msg_deactivate_pdp_context_request(context, sm_get_nw_cause(context), FALSE);

    /* Go to state SM NETWORK DEACTIVATING */
    context->network_control_state = SM_NETWORK_DEACTIVATING;

    /* Report failed activation to Context Control (with network deactivation) */
     
    sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
  } /* if */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_pdp_context_reject
+------------------------------------------------------------------------------
| Description : Handle event SM_M_ACTIVATE_PDP_CONTEXT_REJECT in S1
|
| Parameters  : context          - Context data
|               data             - T_ACTIVATE_PDP_CONTEXT_REJECT *
+------------------------------------------------------------------------------
*/
static void handle_activate_pdp_context_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_ACTIVATE_PDP_CONTEXT_REJECT *msg = (T_ACTIVATE_PDP_CONTEXT_REJECT *)data;
  (void)TRACE_FUNCTION("handle_activate_pdp_context_reject");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Store erroneous PCO, if any */
  if (msg->v_pco != (U8)FALSE && msg->pco.c_pco_value > (U8)0)
  {
    sm_nw_allocate_and_copy_negotiated_pco(context, msg->pco.c_pco_value,
                                           msg->pco.pco_value);
  } else {
    context->negotiated_pco     = NULL;
  }

  /* Stop timer T3380 */
  sm_timer_stop(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Mark context as inactive */
  sm_nw_mark_context_inactive(context->nsapi);

  /* Build cause structure */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Re-synchronise MMs PDP context status */
  send_mmpm_pdp_context_status_req();

  /* Report failed activation to Context Control (no network deactivation) */
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)FALSE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_secondary_pdp_context_accept
+------------------------------------------------------------------------------
| Description : Handle event SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT
|
| Parameters  : context          - Context data
|               data             - T_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT *
+------------------------------------------------------------------------------
*/
static void handle_activate_secondary_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT *msg = (T_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT *)data;

  (void)TRACE_FUNCTION("handle_activate_secondary_pdp_context_accept");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;
	  
  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3380 */
  sm_timer_stop(context);

  if (!sm_is_llc_sapi_valid(msg->llc_sapi.sapi, context->ti))
  {
    return;
  }

  /* Check whether negotiated QoS satisfies minimum QoS */
  if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
  {
    /* QoS >= minimum QoS: Store negotiated parameters */
    sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);

    /* Store GSM RAT parameters */
    context->radio_prio        = msg->radio_prio.radio_prio_val;
    context->sapi              = msg->llc_sapi.sapi;

    /* Store PFI, if present */
    if (msg->v_pfi != (U8)FALSE)
    {
      sm_set_pfi_included(context, TRUE);
      context->pfi             = msg->pfi.pfi_val;
    } else {
      sm_set_pfi_included(context, FALSE);
    }

    /* Check whether to add TFT */
    if (sm_tft_more_to_modify(context))
    {
      /* Start T3381 */
      sm_timer_start(context, SM_TIMER_T3381);
      /* Send MODIFY PDP CONTEXT REQUEST with remaining TFTs */
      send_msg_modify_pdp_context_request(context);
      /* Go to state SM NETWORK ACTIVATING ADDING TFTS */
      context->network_control_state = SM_NETWORK_ACTIVATING_ADDING_TFTS;
    } else {
      /* No TFT: go to state SM NETWORK ACTIVATED, and inform Context Control */
      context->network_control_state = SM_NETWORK_ACTIVATED;

      sm_context_control(context, SM_I_NETWORK_ACTIVATE_COMPLETED, NULL);
    }
  } else {  /* if (sm_qos_is_minimum_satisfied_by_aim()) */
    /* Minimum QoS not satisfied - go to state SM NETWORK DEACTIVATING */
    context->network_control_state = SM_NETWORK_DEACTIVATING;

    /* Start deactivation, T3390 */
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

    /* Report failed activation to Context Control (with network deactivation) */
    sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
  } /* if */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_activate_secondary_pdp_context_reject
+------------------------------------------------------------------------------
| Description : Handle event SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT
|
| Parameters  : context          - Context data
|               data             - T_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT *
+------------------------------------------------------------------------------
*/
static void handle_activate_secondary_pdp_context_reject(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT *msg = (T_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT *)data;
  (void)TRACE_FUNCTION("handle_activate_secondary_pdp_context_reject");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3380 */
  sm_timer_stop(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Mark context as inactive */
  sm_nw_mark_context_inactive(context->nsapi);

  /* Build cause structure */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Re-synchronise MMs PDP context status */
  send_mmpm_pdp_context_status_req();

  /* Report failed activation to Context Control (no network deactivation) */
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)FALSE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_request_pdp_context_activation_in_S0
+------------------------------------------------------------------------------
| Description : Handle event SM_M_REQUEST_PDP_CONTEXT_ACTIVATION in S0
|
| Parameters  : context          - Context data
|               data             - T_REQUEST_PDP_CONTEXT_ACTIVATION *
+------------------------------------------------------------------------------
*/
static void handle_request_pdp_context_activation_in_S0(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_REQUEST_PDP_CONTEXT_ACTIVATION *msg = (T_REQUEST_PDP_CONTEXT_ACTIVATION *)data;

  (void)TRACE_FUNCTION("handle_request_pdp_context_activation_in_S0");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  context->pdp_type         = msg->address.pdp_type_no;

  sm_nw_store_negotiated_address(context, &msg->address, (U8)TRUE);

  if (msg->v_apn != (U8)FALSE)
  {
    sm_nw_allocate_and_copy_apn(context, msg->apn.c_apn_value, msg->apn.apn_value);
  } else {
    context->apn            = NULL;
  }

  /* Inform Context Control of activation request */
  sm_context_control(context, SM_I_NETWORK_REQUEST_ACTIVATION, msg);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_request_pdp_context_activation_in_S1
+------------------------------------------------------------------------------
| Description : Handle event SM_M_REQUEST_PDP_CONTEXT_ACTIVATION in S1
|
| Parameters  : context          - Context data
|               data             - T_REQUEST_PDP_CONTEXT_ACTIVATION *
+------------------------------------------------------------------------------
*/
static void handle_request_pdp_context_activation_in_S1(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_REQUEST_PDP_CONTEXT_ACTIVATION *msg = (T_REQUEST_PDP_CONTEXT_ACTIVATION *)data;
  (void)TRACE_FUNCTION("handle_request_pdp_context_activation_in_S1");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /*lint !e613 (Warning -- Possible use of null pointer) */
  if (sm_nw_is_address_and_apn_equal(context, &context->requested_address,
                                     &msg->address, msg->v_apn, &msg->apn))
  {
    /* Do nothing */
  } else {
    /* Reject incoming context activation */
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_INSUFFICIENT_RESOURCES);

    send_msg_request_pdp_context_activation_reject(context,
                                                   sm_get_nw_cause(context));
  } /* if */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_request_pdp_context_activation_in_S4
+------------------------------------------------------------------------------
| Description : Handle event SM_M_REQUEST_PDP_CONTEXT_ACTIVATION in S4
|
| Parameters  : context          - Context data
|               data             - T_REQUEST_PDP_CONTEXT_ACTIVATION *
+------------------------------------------------------------------------------
*/
static void handle_request_pdp_context_activation_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_REQUEST_PDP_CONTEXT_ACTIVATION *msg = (T_REQUEST_PDP_CONTEXT_ACTIVATION *)data;
  (void)TRACE_FUNCTION("handle_request_pdp_context_activation_in_S4");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  if (sm_nw_is_address_and_apn_equal(context, &context->negotiated_address,
                                     &msg->address, msg->v_apn, &msg->apn))
  {
    /* Go to state SM NETWORK ACTIVATING PRIMARY */
    context->network_control_state = SM_NETWORK_ACTIVATING_PRIMARY;

    /* Inform Context Control of the activation override
     * NOTE!  Normally other state machines are informed last, but in this
     * case we need to update sm_data.sm_context_activation_status _before_
     * sending ACTIVATE PDP CONTEXT REQUEST through MM. */
    sm_context_control(context, SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE, NULL);

    /* Context activation override - start context activation procedure */
    sm_timer_start(context, SM_TIMER_T3380);

    /* This is akin to a context reactivation: Use currently negotiated IP address */
    sm_nw_copy_negotiated_address_to_requested(context);

    send_msg_activate_pdp_context_request(context);
  } else {
    /* Reject incoming context activation */
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_INSUFFICIENT_RESOURCES);
    send_msg_request_pdp_context_activation_reject(context, sm_get_nw_cause(context));
  } /* if */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_request_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REQUEST in S3
|
| Parameters  : context          - Context data
|               data             - T_MODIFY_PDP_CONTEXT_REQUEST * (unused)
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_request_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_modify_pdp_context_request_in_S3");

  /* Free coded message */
  sm_free_coded_msg(context);

  /* Stop T3381, start T3390, and send DEACTIVATE PDP CONTEXT REQUEST message */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm,
                  (U16)CAUSE_NWSM_MSG_INCOMPATIBLE_WITH_STATE);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Inform Context Control of the failed activation (with network deactivation) */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm,
                   (U16)CAUSE_NWSM_MSG_INCOMPATIBLE_WITH_STATE);
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_request_in_S4
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REQUEST in S4
|
| Parameters  : context          - Context data
|               data             - T_D_MODIFY_PDP_CONTEXT_REQUEST *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_request_in_S4(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_D_MODIFY_PDP_CONTEXT_REQUEST *msg = (T_D_MODIFY_PDP_CONTEXT_REQUEST *)data;
  T_SM_UPDATE_FLAGS      update_flags;

  (void)TRACE_FUNCTION("handle_modify_pdp_context_request_in_S4");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  if (!sm_is_llc_sapi_valid(msg->llc_sapi.sapi, context->ti))
  {
    return;
  }

  if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
  {
    /* QoS >= minimum QoS: Store negotiated parameters */
    update_flags = sm_nw_store_mt_modify_parameters(context, msg);

    /* Go to state SM NETWORK ACTIVATED */
    context->network_control_state = SM_NETWORK_ACTIVATED;

    /* Reply to network: MODIFY PDP CONTEXT ACCEPT */
    send_msg_modify_pdp_context_accept(context);

    /* Report successful NW initiated context modification */
    sm_context_control(context, SM_I_NETWORK_REQUEST_MODIFY,
                       (void *)update_flags);
  } else {
    /* Minimum QoS not satisfied - go to state SM NETWORK DEACTIVATING */
    context->network_control_state = SM_NETWORK_DEACTIVATING;

    /* Start deactivation, T3390 */
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

    /* Report failed activation to Context Control */
    sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_context_control(context, SM_I_NETWORK_REQUEST_MODIFY_FAILED, NULL);
  } /* if (sm_qos_is_minimum_satisfied_by_aim()) */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_request_in_S5
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REQUEST in S5
|
| Parameters  : context          - Context data
|               data             - T_D_MODIFY_PDP_CONTEXT_REQUEST *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_request_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_D_MODIFY_PDP_CONTEXT_REQUEST *msg = (T_D_MODIFY_PDP_CONTEXT_REQUEST *)data;
  T_SM_UPDATE_FLAGS      update_flags;

  (void)TRACE_FUNCTION("handle_modify_pdp_context_request_in_S5");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free coded message */
  sm_free_coded_msg(context);

  /* Reject currently active UE initiated modification */
  sm_set_aci_cause(context, CAUSE_is_from_sm, (U16)CAUSE_SM_MODIFY_COLLISION);
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)FALSE);

  if (!sm_is_llc_sapi_valid(msg->llc_sapi.sapi, context->ti))
  {
    return;
  }

  if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
  {
    /* QoS >= minimum QoS: Store negotiated parameters */
    update_flags = sm_nw_store_mt_modify_parameters(context, msg);

    /* Go to state SM NETWORK ACTIVATED */
    context->network_control_state = SM_NETWORK_ACTIVATED;

    /* Reply to network: MODIFY PDP CONTEXT ACCEPT */
    send_msg_modify_pdp_context_accept(context);

    /* Report successful NW initiated context modification */
    sm_context_control(context, SM_I_NETWORK_REQUEST_MODIFY,
                       (void *)update_flags);
  } else {
    /* Minimum QoS not satisfied - go to state SM NETWORK DEACTIVATING */
    context->network_control_state = SM_NETWORK_DEACTIVATING;

    /* Start deactivation, T3390 */
    sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

    /* Report failed activation to Context Control */
    sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
    sm_context_control(context, SM_I_NETWORK_REQUEST_MODIFY_FAILED, NULL);
  } /* if (sm_qos_is_minimum_satisfied_by_aim()) */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_request_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REQUEST in S6
|
| Parameters  : context          - Context data
|               data             - T_MODIFY_PDP_CONTEXT_REQUEST * (unused)
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_request_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_modify_pdp_context_request_in_S6");

  /* Free coded message */
  sm_free_coded_msg(context);

  /* Reject currently active UE initiated modification */
  sm_set_aci_cause(context, CAUSE_is_from_sm, (U16)CAUSE_SM_MODIFY_COLLISION);
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)FALSE);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Stop T3381, start T3390, and send DEACTIVATE PDP CONTEXT REQUEST message */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm,
                  (U16)CAUSE_NWSM_MSG_INCOMPATIBLE_WITH_STATE);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed modification */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm,
                   (U16)CAUSE_NWSM_MSG_INCOMPATIBLE_WITH_STATE);
  sm_context_control(context, SM_I_NETWORK_REQUEST_MODIFY_FAILED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_accept_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_ACCEPT in S3
|
| Parameters  : context          - Context data
|               data             - T_D_MODIFY_PDP_CONTEXT_ACCEPT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_accept_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_D_MODIFY_PDP_CONTEXT_ACCEPT *msg = (T_D_MODIFY_PDP_CONTEXT_ACCEPT *)data;
  T_SM_UPDATE_FLAGS     update_flags;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_accept_in_S3");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3381 */
  sm_timer_stop(context);

  /* Store GSM RAT parameters, if present */
  update_flags = sm_nw_store_modify_accept_parameters(context, msg);

  if (msg->v_qos != (U8)FALSE)
  {
    update_flags       |= SM_UPDATE_QOS;
    /* We requested no new QoS, but received one nontheless. */
    if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
    {
      /* QoS >= minimum QoS: Store negotiated parameters */
      sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);
    } else {
      /* Minimum QoS no longer satisfied: Abort activation */
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

      context->network_control_state = SM_NETWORK_DEACTIVATING;

      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      /* Report failed activation to Context Control (with network deactivation) */
      sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
      return;
    } /* if (sm_qos_is_minimum_satisfied_by_aim) */
  }

  if (sm_tft_more_to_modify(context))
  {
    /* Start T3381 */
    sm_timer_start(context, SM_TIMER_T3381);
    /* Send MODIFY PDP CONTEXT REQUEST with remaining TFTs */
    send_msg_modify_pdp_context_request(context);
  } else {
    /* No more TFTs to add: go to state SM NETWORK ACTIVATED, and inform Context Control */
    context->network_control_state = SM_NETWORK_ACTIVATED;

    sm_context_control(context, SM_I_NETWORK_ACTIVATE_COMPLETED,
                       (void *)update_flags);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_accept_in_S5
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_ACCEPT in S5
|
| Parameters  : context          - Context data
|               data             - T_D_MODIFY_PDP_CONTEXT_ACCEPT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_accept_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_D_MODIFY_PDP_CONTEXT_ACCEPT *msg = (T_D_MODIFY_PDP_CONTEXT_ACCEPT *)data;
  T_SM_UPDATE_FLAGS     update_flags = (T_SM_UPDATE_FLAGS)0;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_accept_in_S5");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3381 */
  sm_timer_stop(context);

  /* Store GSM RAT parameters, if present */
  update_flags = sm_nw_store_modify_accept_parameters(context, msg);

  if (msg->v_qos != (U8)FALSE)
  {
    /* QoS updated */
    if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
    {
      /* QoS >= minimum QoS: Store negotiated parameters */
      sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);
      update_flags     |= SM_UPDATE_QOS;
    } else {
      /* Minimum QoS no longer satisfied: Deactivate */
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

      context->network_control_state = SM_NETWORK_DEACTIVATING;

      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)FALSE);
      return;
    } /* if (sm_qos_is_minimum_satisfied_by_aim) */
  } else {
    /* Check that minimum QoS is still satisfied by current QoS */
    if (!sm_qos_is_minimum_satisfied_by_sm(context, &context->accepted_qos))
    {
      /* Minimum QoS no longer satisfied: Deactivate */
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

      context->network_control_state = SM_NETWORK_DEACTIVATING;

      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)TRUE);

      return;
    }
  }

  if (sm_tft_more_to_modify(context))
  {
    /* Start T3381 */
    sm_timer_start(context, SM_TIMER_T3381);
    /* Send MODIFY PDP CONTEXT REQUEST with more TFTs */
    send_msg_modify_pdp_context_request(context);

    context->network_control_state = SM_NETWORK_MODIFYING_ADDITIONAL_TFTS;
  } else {
    /* No more TFTs to add: Modification completed. */
    /* Go to state SM NETWORK ACTIVATED, and inform Context Control */
    context->network_control_state = SM_NETWORK_ACTIVATED;

    sm_context_control(context, SM_I_NETWORK_MODIFY_COMPLETED,
                       (void *)update_flags);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_accept_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_ACCEPT in S6
|
| Parameters  : context          - Context data
|               data             - T_D_MODIFY_PDP_CONTEXT_ACCEPT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_accept_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_D_MODIFY_PDP_CONTEXT_ACCEPT *msg = (T_D_MODIFY_PDP_CONTEXT_ACCEPT *)data;
  T_SM_UPDATE_FLAGS     update_flags = (T_SM_UPDATE_FLAGS)0;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_accept_in_S6");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop timer T3381 */
  sm_timer_stop(context);

  /* Store GSM RAT parameters, if present */
  update_flags = sm_nw_store_modify_accept_parameters(context, msg);

  if (msg->v_qos != (U8)FALSE)
  {
    /* We requested no new QoS, but received one nontheless. */
    if (sm_qos_is_minimum_satisfied_by_aim(context, &msg->qos))
    {
      /* QoS >= minimum QoS: Store negotiated parameters */
      sm_qos_assign_from_aim(&context->accepted_qos, &msg->qos);
      update_flags     |= SM_UPDATE_QOS;
    } else {
      /* Minimum QoS no longer satisfied: Deactivate */
      sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

      context->network_control_state = SM_NETWORK_DEACTIVATING;

      sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_QOS_NOT_ACCEPTED);
      sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)TRUE);

      return;
    } /* if (sm_qos_is_minimum_satisfied_by_aim) */
  }

  if (sm_tft_more_to_modify(context))
  {
    /* Start T3381 */
    sm_timer_start(context, SM_TIMER_T3381);
    /* Send MODIFY PDP CONTEXT REQUEST with remaining TFTs */
    send_msg_modify_pdp_context_request(context);
  } else {
    /* No more TFTs to add: Modification completed. */
    /* Go to state SM NETWORK ACTIVATED, and inform Context Control */
    context->network_control_state = SM_NETWORK_ACTIVATED;

    sm_context_control(context, SM_I_NETWORK_MODIFY_COMPLETED,
                       (void *)update_flags);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_reject_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REJECT in S3
|
| Parameters  : context          - Context data
|               data             - T_MODIFY_PDP_CONTEXT_REJECT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_reject_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_MODIFY_PDP_CONTEXT_REJECT *msg = (T_MODIFY_PDP_CONTEXT_REJECT *)data;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_reject_in_S3");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Save cause value for ACI */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Stop T3381, start T3390, and send DEACTIVATE PDP CONTEXT REQUEST message */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed activation (with network deactivation) */
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_reject_in_S5
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REJECT in S5
|
| Parameters  : context          - Context data
|               data             - T_MODIFY_PDP_CONTEXT_REJECT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_reject_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_MODIFY_PDP_CONTEXT_REJECT *msg = (T_MODIFY_PDP_CONTEXT_REJECT *)data;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_reject_in_S5");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Save cause value for ACI */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Stop T3381, start T3390, and send DEACTIVATE PDP CONTEXT REQUEST message */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed modification */
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_pdp_context_reject_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_M_MODIFY_PDP_CONTEXT_REJECT in S6
|
| Parameters  : context          - Context data
|               data             - T_MODIFY_PDP_CONTEXT_REJECT *
+------------------------------------------------------------------------------
*/
static void handle_modify_pdp_context_reject_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_MODIFY_PDP_CONTEXT_REJECT *msg = (T_MODIFY_PDP_CONTEXT_REJECT *)data;
  (void)TRACE_FUNCTION("handle_modify_pdp_context_reject_in_S6");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Save cause value for ACI */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Stop T3381, start T3390, and send DEACTIVATE PDP CONTEXT REQUEST message */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed modification */
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_pdp_context_request_while_active
+------------------------------------------------------------------------------
| Description : Handle event SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST in S1 - S6
|
| Parameters  : context          - Context data
|               data             - T_DEACTIVATE_PDP_CONTEXT_REQUEST *
+------------------------------------------------------------------------------
*/
static void handle_deactivate_pdp_context_request_while_active(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  BOOL tear_down;
  T_DEACTIVATE_PDP_CONTEXT_REQUEST *msg = (T_DEACTIVATE_PDP_CONTEXT_REQUEST *)data;
  (void)TRACE_FUNCTION("handle_deactivate_pdp_context_request_while_active");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Stop any active timers */
  sm_timer_stop(context);

  /* Free stored coded message - if any */
  sm_free_coded_msg(context);

  if (msg->v_tear_down != (U8)FALSE && msg->tear_down.tear_down_flag == (U8)1)
  {
    tear_down = TRUE;
    /* Mark linked contexts as inactive */
    sm_nw_mark_nsapi_set_inactive(sm_linked_nsapis(context->ti));
  } else {
    tear_down = FALSE;
  };

  /* Mark context as inactive */
  sm_nw_mark_context_inactive(context->nsapi);

  /* Save cause for ACI */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Send DEACTIVATE PDP CONTEXT ACCEPT message */
  send_msg_deactivate_pdp_context_accept(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Report deactivation to Context Control */
  sm_context_control(context, SM_I_NETWORK_REQUEST_DEACTIVATE, (void *)tear_down);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_pdp_context_request_while_deactivating
+------------------------------------------------------------------------------
| Description : Handle event SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST in S7
|
| Parameters  : context          - Context data
|               data             - T_DEACTIVATE_PDP_CONTEXT_REQUEST *
+------------------------------------------------------------------------------
*/
static void handle_deactivate_pdp_context_request_while_deactivating(struct T_SM_CONTEXT_DATA *context, /*@null@*/ void *data)
{
  T_DEACTIVATE_PDP_CONTEXT_REQUEST *msg = (T_DEACTIVATE_PDP_CONTEXT_REQUEST *)data;
  (void)TRACE_FUNCTION("handle_deactivate_pdp_context_request_while_deactivating");

  TRACE_ASSERT(msg != NULL);

  if (msg EQ NULL)
    return;

  /* Save cause for ACI */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)msg->sm_cause.sm_cause_val);

  /* Send DEACTIVATE PDP CONTEXT ACCEPT message */
  send_msg_deactivate_pdp_context_accept(context);

  /* Report deactivation to Context Control iff tear_down == TRUE */
  if (msg->v_tear_down != (U8)FALSE && msg->tear_down.tear_down_flag == (U8)1)
  {
    sm_context_control(context, SM_I_NETWORK_REQUEST_DEACTIVATE, (void *)TRUE);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_pdp_context_accept
+------------------------------------------------------------------------------
| Description : Handle event SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT in S7
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_deactivate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_deactivate_pdp_context_accept");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Stop T3390 */
  sm_timer_stop(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Mark context as inactive */
  sm_nw_mark_context_inactive(context->nsapi);

  /* Report deactivate complete to Context Control */
  sm_context_control(context, SM_I_NETWORK_DEACTIVATE_COMPLETED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3380
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3380
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3380(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3380");

  /* If SM is not suspended, resend ACTIVATE (SECONDARY) PDP CONTEXT REQUEST */
  resend_msg(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3380_max
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3380_MAX
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3380_max(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3380_max");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Inform Context Control of the failed activation (no network deactivation) */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_NETWORK_FAILURE);
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)FALSE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3381
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3381
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3381(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3381");

  /* If SM is not suspended, resend MODIFY PDP CONTEXT REQUEST */
  resend_msg(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3381_max_in_S3
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3381_MAX in S3
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3381_max_in_S3(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3381_max_in_S3");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Initiate deactivation */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed activation */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_NETWORK_FAILURE);
  sm_context_control(context, SM_I_NETWORK_ACTIVATE_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3381_max_in_S5
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3381_MAX in S5
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3381_max_in_S5(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3381_max_in_S5");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Modify procedure has failed, but the context remains active. */
  /* Go to state SM NETWORK ACTIVATED */
  context->network_control_state = SM_NETWORK_ACTIVATED;

  /* Inform Context Control of the failed activation */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_NETWORK_FAILURE);
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)FALSE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3381_max_in_S6
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3381_MAX in S6
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3381_max_in_S6(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3381_max_in_S6");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATING */
  context->network_control_state = SM_NETWORK_DEACTIVATING;

  /* Initiate deactivation */
  sm_set_nw_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_REGULAR_DEACTIVATION);
  sm_nw_initiate_deactivation(context, sm_get_nw_cause(context));

  /* Inform Context Control of the failed activation */
  sm_set_aci_cause(context, CAUSE_is_from_nwsm, (U16)CAUSE_NWSM_NETWORK_FAILURE);
  sm_context_control(context, SM_I_NETWORK_MODIFY_REJECTED, (void *)TRUE);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3390
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3390
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3390(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3390");

  /* If SM is not suspended, resend DEACTIVATE PDP CONTEXT REQUEST */
  resend_msg(context);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_T3390_max
+------------------------------------------------------------------------------
| Description : Handle event SM_T_T3390_MAX
|
| Parameters  : context          - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_T3390_max(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/void *data)
{
  (void)TRACE_FUNCTION("handle_T3390_max");

  /* Free stored coded message */
  sm_free_coded_msg(context);

  /* Go to state SM NETWORK DEACTIVATED */
  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Context is considered deactivated.  Inform Context Control. */
  sm_context_control(context, SM_I_NETWORK_DEACTIVATE_COMPLETED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_mmpm_suspend_ind
+------------------------------------------------------------------------------
| Description : Handle incoming MMPM_SUSPEND_IND primitive
|
| Parameters  : context         - Context data
|               data            - Unused
+------------------------------------------------------------------------------
*/
static void handle_mmpm_suspend_ind(/*@unused@*/struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_mmpm_suspend_ind");
}

/*
+------------------------------------------------------------------------------
| Function    : handle_mmpm_resume_ind
+------------------------------------------------------------------------------
| Description : Handle incoming MMPM_RESUME_IND primitive
|
| Parameters  : context         - Context data
|               data            - Unused
+------------------------------------------------------------------------------
*/
static void handle_mmpm_resume_ind(struct T_SM_CONTEXT_DATA *context, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("handle_mmpm_resume_ind");

  if (sm_is_started_during_suspend(context))
  {
    resend_msg(context);
    sm_clear_started_during_suspend(context);
  }
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_network_control_init
+------------------------------------------------------------------------------
| Description : Network Control state machine initialization function

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
void sm_network_control_init(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_network_control_init");

  context->network_control_state = SM_NETWORK_DEACTIVATED;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_network_control_exit
+------------------------------------------------------------------------------
| Description : Network Control state machine exit function

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
void sm_network_control_exit(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("sm_network_control_exit");

  context->network_control_state = SM_NETWORK_DEACTIVATED;

  /* Free any memory allocated to this context (fields managed by Network Control) */
  sm_free_coded_msg(context);
  sm_nw_free_apn(context);
  sm_nw_free_requested_tft(context);
  sm_nw_free_active_tft(context);
  sm_nw_free_requested_pco(context);
  sm_nw_free_negotiated_pco(context);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_network_control_state
+------------------------------------------------------------------------------
| Description : Returns a read-only string with the name of the active state.

| Parameters  : context  - Context data
+------------------------------------------------------------------------------
*/
#ifdef DEBUG
/*@observer@*/const char *
sm_network_control_state(struct T_SM_CONTEXT_DATA *context)
{
  /*@observer@*/
  static const char *state_name[SM_NETWORK_CONTROL_NUMBER_OF_STATES] = {
    "S0_SM_NETWORK_DEACTIVATED",
    "S1_SM_NETWORK_ACTIVATING_PRIMARY",
    "S2_SM_NETWORK_ACTIVATING_SECONDARY",
    "S3_SM_NETWORK_ACTIVATING_ADDING_TFTS",
    "S4_SM_NETWORK_ACTIVATED",
    "S5_SM_NETWORK_MODIFYING",
    "S6_SM_NETWORK_MODIFYING_ADDITIONAL_TFTS",
    "S7_SM_NETWORK_DEACTIVATING"
  };

  TRACE_ASSERT(context->network_control_state < SM_NETWORK_CONTROL_NUMBER_OF_STATES);

  return state_name[(U16)context->network_control_state];
}
#endif
/*
+------------------------------------------------------------------------------
| Function    : sm_network_control
+------------------------------------------------------------------------------
| Description : User Plane Control state machine
|
| Parameters  : context          - Context data
|               event            - Internal event (see sm_network_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void sm_network_control(struct T_SM_CONTEXT_DATA *context,
                        T_SM_NETWORK_CONTROL_EVENT event,
                        /*@null@*/ void *data)
{
#ifdef DEBUG
  const char *old_state_name, *new_state_name;
  T_SM_NETWORK_CONTROL_STATE old_state;
  /*@observer@*/
  static const char *event_name[SM_NETWORK_CONTROL_NUMBER_OF_EVENTS] = {
    "SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT",
    "SM_M_ACTIVATE_PDP_CONTEXT_REJECT",
    "SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT",
    "SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT",
    "SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST",
    "SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT",
    "SM_M_MODIFY_PDP_CONTEXT_REQUEST",
    "SM_M_MODIFY_PDP_CONTEXT_ACCEPT",
    "SM_M_MODIFY_PDP_CONTEXT_REJECT",
    "SM_M_REQUEST_PDP_CONTEXT_ACTIVATION",
    "SM_M_SM_STATUS",
    "SM_P_MMPM_RESUME_IND",
    "SM_P_MMPM_SUSPEND_IND",
    "SM_T_T3380",
    "SM_T_T3380_MAX",
    "SM_T_T3381",
    "SM_T_T3381_MAX",
    "SM_T_T3390",
    "SM_T_T3390_MAX",
    "SM_I_NETWORK_ACTIVATE_PRIMARY",
    "SM_I_NETWORK_ACTIVATE_SECONDARY",
    "SM_I_NETWORK_DEACTIVATE_LOCAL",
    "SM_I_NETWORK_MODIFY",
    "SM_I_NETWORK_REQUEST_ACTIVATION_REJECT",
    "SM_I_NETWORK_DEACTIVATE"
  };

  TRACE_ASSERT(event < SM_NETWORK_CONTROL_NUMBER_OF_EVENTS);

  old_state = context->network_control_state;
  old_state_name = sm_network_control_state(context);

  if (transition[(U16)old_state][(U16)event].event != event)
  {
    (void)TRACE_ERROR("Event table error in sm_network_control!");
  }
#endif /* DEBUG */

  transition[(U16)context->network_control_state][(U16)event].func(context, data);

#ifdef DEBUG
  new_state_name = sm_network_control_state(context);

  (void)TRACE_EVENT_P4("SM NETWORK #%d: %s => %s to %s", context->nsapi,
                       event_name[(U16)event], old_state_name, new_state_name);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
