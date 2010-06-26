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
| Purpose:    Link Control state machine implementation in the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/


/*==== INCLUDES =============================================================*/

#include "upm.h"

#ifdef TI_UMTS
#include "upm_rab_control.h"
#endif /* TI_UMTS */

#ifdef TI_GPRS
#include "upm_sndcp_control.h"
#endif /* TI_GPRS */

#include "upm_sm_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_UPM_LINK_CONTROL_TRANSITION_FUNC)(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_UPM_LINK_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_UPM_LINK_CONTROL_TRANSITION_FUNC  func;
} T_UPM_LINK_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error                           (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void ignored                                     (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sm_activate_started_ind              (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_dti_closed                           (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
#ifdef TI_UMTS
static void go_to_S2                                    (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sm_activate_ind_in_umts              (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sm_modify_ind_in_umts                (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_deactivate_in_umts                   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rab_released_abnormal                (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rab_released_normal                  (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
#endif /* TI_UMTS */
#ifdef TI_GPRS
static void handle_sm_activate_ind_in_gsm               (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_llc_link_established                 (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sm_modify_ind_in_gsm                 (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_modify_response_in_gsm               (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_deactivate_in_gsm                    (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_deactivate_without_link_in_gsm       (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_llc_release_normal                   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_llc_release_abnormal                 (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_gsm_qos_store(struct T_CONTEXT_DATA *ptr_context_data,
                                 U8 sapi, U8 radio_prio, U8 pkt_flow_id,
                                 T_PS_ctrl_qos ctrl_qos, T_PS_qos *qos);
#endif /* TI_GPRS */
#ifdef TI_DUAL_MODE
static void go_to_S8                                    (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rat_change_to_gsm_without_context    (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rat_change_to_gsm                    (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rab_established_in_S6                (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rat_change_to_umts_without_context   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rat_change_to_umts_while_establishing(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_rat_change_to_umts                   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
#else
#define handle_rat_change_to_gsm_without_context        state_event_error
#define handle_rat_change_to_gsm                        state_event_error
#define handle_rat_change_to_umts_without_context       state_event_error
#define handle_rat_change_to_umts_while_establishing    state_event_error
#define handle_rat_change_to_umts                       state_event_error
#endif /* TI_DUAL_MODE */
static void upm_link_mark_nsapi_deactivated(U8 nsapi);

/***********************************************************************
 * State/Transition Table
 */
static const T_UPM_LINK_CONTROL_TRANSITION
transition[UPM_LINK_CONTROL_NUMBER_OF_STATES]
          [UPM_LINK_CONTROL_NUMBER_OF_EVENTS] =
{
  { /* S0: UPM_LINK_RELEASED */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    handle_sm_activate_started_ind),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          state_event_error),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    state_event_error),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           state_event_error)
  },
#ifdef TI_UMTS
  { /* S1: UPM_LINK_ESTABLISHING_RAB */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_umts),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            go_to_S2),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    state_event_error),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_gsm_without_context)
  },
  { /* S2: UPM_LINK_AWAITING_ACTIVATE_UMTS */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            handle_sm_activate_ind_in_umts),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_umts),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               handle_rab_released_abnormal),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    state_event_error),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           state_event_error)
  },
  { /* S3: UPM_LINK_RAB_ESTABLISHED */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_umts),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              handle_sm_modify_ind_in_umts),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            ignored),
    M_TRANSITION(UPM_I_RAB_RELEASED,               handle_rab_released_abnormal),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    ignored),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_gsm)
  },
  { /* S4: UPM_LINK_RELEASING_RAB */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          state_event_error),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               handle_rab_released_normal),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    state_event_error),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           ignored)
  },
#endif /* TI_UMTS */
#ifdef TI_DUAL_MODE
  { /* S5: UPM_LINK_COMPLETING_RAT_CHANGE_TO_GSM */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_gsm),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               ignored),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, go_to_S8),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    handle_llc_release_abnormal),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           state_event_error)
  },
  { /* S6: UPM_LINK_COMPLETING_RAT_CHANGE_TO_UMTS_INITIAL */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_umts),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            handle_rab_established_in_S6),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    ignored),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_gsm)
  },
#endif /* TI_DUAL_MODE */
#ifdef TI_GPRS
  { /* S7: UPM_LINK_AWAITING_ACTIVATE_GSM */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            handle_sm_activate_ind_in_gsm),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_without_link_in_gsm),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    state_event_error),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_umts_without_context)
  },
  { /* S8: UPM_LINK_ESTABLISHING_LLC_LINK */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_gsm),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            ignored),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, handle_llc_link_established),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    handle_llc_release_abnormal),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_umts_while_establishing)
  },
  { /* S9: UPM_LINK_LLC_LINK_ESTABLISHED */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          handle_deactivate_in_gsm),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              handle_sm_modify_ind_in_gsm),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            handle_dti_closed),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    handle_modify_response_in_gsm),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    handle_llc_release_abnormal),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           handle_rat_change_to_umts)
  },
  { /* S10: UPM_LINK_RELEASING_LLC_LINK */
    M_TRANSITION(UPM_P_SM_ACTIVATE_STARTED_IND,    state_event_error),
    M_TRANSITION(UPM_P_SM_ACTIVATE_IND,            state_event_error),
    M_TRANSITION(UPM_P_SM_DEACTIVATE_IND,          state_event_error),
    M_TRANSITION(UPM_P_SM_MODIFY_IND,              state_event_error),
    M_TRANSITION(UPM_I_LINK_DTI_CLOSED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_ESTABLISHED,            state_event_error),
    M_TRANSITION(UPM_I_RAB_RELEASED,               state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_ESTABLISHED, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_MODIFIED,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_LLC_LINK_RELEASED,    handle_llc_release_normal),
    M_TRANSITION(UPM_I_LINK_RAT_CHANGED,           ignored)
  }
#endif /* TI_GPRS */
};

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : link_control_goto_state
+------------------------------------------------------------------------------
| Description : Move to the state specified in the new_state parameter
|
| Parameters  : ptr_context_data - Context data
|               new_state        - New state
+------------------------------------------------------------------------------
*/
static void link_control_goto_state(struct T_CONTEXT_DATA *ptr_context_data,
                                    T_UPM_LINK_CONTROL_STATE new_state)
{
  ptr_context_data->link_control_state = new_state;
}

/*
+------------------------------------------------------------------------------
| Function    : link_control_state
+------------------------------------------------------------------------------
| Description : Return the state in which Link Control is in for this
|               context instanse.
|
| Parameters  : ptr_context_data - Context data
+------------------------------------------------------------------------------
*/
static T_UPM_LINK_CONTROL_STATE
link_control_state(struct T_CONTEXT_DATA *ptr_context_data)
{
  return ptr_context_data->link_control_state;
}

/*
+------------------------------------------------------------------------------
| Function    : state_event_error
+------------------------------------------------------------------------------
| Description : General function used to report state event errors.
|
| Parameters  : ptr_context_data - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void state_event_error(/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_ERROR("UPM Link Control: STATE EVENT ERROR!");
}

/*
+------------------------------------------------------------------------------
| Function    : ignored
+------------------------------------------------------------------------------
| Description : General function used for transitions that shall be ignored
|
| Parameters  : ptr_context_data - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void ignored(/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: Event ignored.");
}

/*
+------------------------------------------------------------------------------
| Function    : go_to_S*
+------------------------------------------------------------------------------
| Description : General functions for simple state transitions
|
| Parameters  : ptr_context_data - Not used
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void go_to_S2(/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  /* (void)TRACE_FUNCTION("UPM Link Control: Transition to UPM LINK AWAITING ACTIVATE UMTS state"); */
  link_control_goto_state(ptr_context_data, UPM_LINK_AWAITING_ACTIVATE_UMTS);
}
#endif /* TI_UMTS */

#ifdef TI_DUAL_MODE
static void go_to_S8(/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  /* (void)TRACE_FUNCTION("UPM Link Control: Transition to UPM LINK LLC LINK ESTABLISHED state"); */
  link_control_goto_state(ptr_context_data, UPM_LINK_LLC_LINK_ESTABLISHED);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_activate_started_ind
+------------------------------------------------------------------------------
| Description : Handle SM_ACTIVATE_STARTED_IND primitive.  Examines RAT,
|               and moves to GSM or UTMS state. Starts RAB in UMTS.
|
| Parameters  : ptr_context_data - Context data
|               data             - SM_ACTIVATE_STARTED_IND primitive (unused)
+------------------------------------------------------------------------------
*/
static void handle_sm_activate_started_ind(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_sm_activate_started_ind");

#ifdef TI_DUAL_MODE
  /* Examine RAT: */
  if (upm_get_current_rat() == PS_RAT_GSM)
#endif /* TI_DUAL_MODE */
#ifdef TI_GPRS
  {
    /* GSM: Do nothing until SM_ACTIVATE_IND arrives.  Just change state. */
    link_control_goto_state(ptr_context_data, UPM_LINK_AWAITING_ACTIVATE_GSM);
  }
#endif /* TI_GPRS */
#ifdef TI_DUAL_MODE
  else if (upm_get_current_rat() == PS_RAT_UMTS_FDD)
#endif /* TI_DUAL_MODE */
#ifdef TI_UMTS
  {
    /* UMTS: Order RAB Control to await RAB establishment, and change state. */
    link_control_goto_state(ptr_context_data, UPM_LINK_ESTABLISHING_RAB);
    upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_INITIAL_ESTABLISH, NULL);
  }
#endif /* TI_UMTS */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_dti_closed
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_DTI_CLOSED
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_dti_closed(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  T_PS_sapi  llc_sapi;

  (void)TRACE_FUNCTION("UPM Link Control: handle_dti_closed");

#ifdef TI_GPRS
  llc_sapi = (T_PS_sapi)ptr_context_data->gprs_data.sndcp_llc_sapi;
#else
  llc_sapi = PS_SAPI_0; /* SAPI not assigned [3G 24.008, 6.1.3.1.1] */
#endif /* TI_GPRS */

  /* Send SM_STATUS_REQ, and expect control plane to take action. */
  send_sm_status_req(upm_nsapi2nsapi_set(ptr_context_data->nsapi),
                     llc_sapi,
                     CAUSE_is_from_upm, (U16)CAUSE_UPM_DTI_DISCONNECT);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_activate_ind_in_utms
+------------------------------------------------------------------------------
| Description : Handle SM_ACTIVATE_IND primitive while in UMTS RAT.
|
| Parameters  : ptr_context_data - Context data
|               data             - SM_ACTIVATE_IND primitive
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void handle_sm_activate_ind_in_umts(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  T_SM_ACTIVATE_IND *prim = (T_SM_ACTIVATE_IND *)data;

  (void)TRACE_FUNCTION("UPM Link Control: handle_sm_activate_ind_in_umts");

  TRACE_ASSERT( data != NULL );

#ifdef TI_DUAL_MODE
  /* Store GSM-RAT relevant QoS parameters in ptr_context_data fields. */
  handle_gsm_qos_store(ptr_context_data, prim->sapi, prim->radio_prio,
                       (U8)prim->pkt_flow_id, prim->ctrl_qos, &prim->qos);
#endif /* TI_DUAL_MODE */

  /* Send SM_ACTIVATE_RES */
  send_sm_activate_res(ptr_context_data, ((T_SN_ACTIVATE_CNF*)data));

  /* Go to state UPM LINK RAB ESTABLISHED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RAB_ESTABLISHED);

  /* Store QoS parameter as needed in RAB Control */
  upm_rab_control(ptr_context_data, UPM_I_RAB_MODIFY, &(prim->qos.qos_r99));

  /* Inform Uplink and Downlink state machines of link up. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_UP, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_UP, NULL);
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_modify_ind_in_utms
+------------------------------------------------------------------------------
| Description : Handle SM_MODIFY_IND primitive while in UMTS RAT.
|
| Parameters  : ptr_context_data - Context data
|               data             - SM_MODIFY_IND primitive
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void handle_sm_modify_ind_in_umts(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  T_SM_MODIFY_IND *prim = (T_SM_MODIFY_IND *)data;

  (void)TRACE_FUNCTION("UPM Link Control: handle_sm_modify_ind_in_umts");

  TRACE_ASSERT( data != NULL );

#ifdef TI_DUAL_MODE
  /* Store GSM-RAT relevant QoS parameters in ptr_context_data fields. */
  handle_gsm_qos_store(ptr_context_data, prim->sapi, prim->radio_prio,
                       (U8)prim->pkt_flow_id, prim->ctrl_qos, &prim->qos);
#endif

  /* Store QoS parameter as needed in RAB Control */
  upm_rab_control(ptr_context_data, UPM_I_RAB_MODIFY,
                  &(prim->qos.qos_r99));

  /* Send SM_MODIFY_RES */
  send_sm_modify_res(ptr_context_data);
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_in_umts
+------------------------------------------------------------------------------
| Description : Handle SM_DEACTIVATE_IND primitive while in UMTS RAT.
|
| Parameters  : ptr_context_data - Context data
|               data             - Local release flag (Not used in UMTS)
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void handle_deactivate_in_umts(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_deactivate_in_umts");

  /* Go to state UPM LINK RELEASING RAB */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASING_RAB);

  /* Terminate Uplink and Downlink state machines. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_DOWN, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_DOWN, NULL);

  /* Order RAB Control to await RAB release. */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_RAB_RELEASE, NULL);
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : handle_rab_released_abnormal
+------------------------------------------------------------------------------
| Description : Handle UPM_I_RAB_RELEASED when not expected
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void handle_rab_released_abnormal(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rab_released_abnormal");

  /* Go to state UPM LINK RELEASED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);

  /* Terminate Uplink and Downlink state machines. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_DOWN, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_DOWN, NULL);
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : handle_rab_released_normal
+------------------------------------------------------------------------------
| Description : Handle UPM_I_RAB_RELEASED during normal deactivation
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_UMTS
static void handle_rab_released_normal(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rab_released_normal");

  /* SM_DEACTIVATE_RES sent from dispatcher; groups responses for all NSAPIs */
  /* send_sm_deactivate_res( ... ); */
  upm_link_mark_nsapi_deactivated(ptr_context_data->nsapi);

  /* Go to state UPM LINK RELEASED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);
}
#endif /* TI_UMTS */

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_activate_ind_in_gsm
+------------------------------------------------------------------------------
| Description : Handle SM_ACTIVATE_IND in S7
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_sm_activate_ind_in_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  T_SM_ACTIVATE_IND *prim = (T_SM_ACTIVATE_IND *)data;

  (void)TRACE_FUNCTION("UPM Link Control: handle_sm_activate_ind_in_gsm");

  TRACE_ASSERT(prim != NULL);

  /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
  /* Store GSM-RAT relevant QoS parameters in ptr_context_data fields. */
  handle_gsm_qos_store(ptr_context_data, prim->sapi, prim->radio_prio,
                       (U8)prim->pkt_flow_id, prim->ctrl_qos, &prim->qos);

  /* Go to state UPM LINK ESTABLISHING LLC LINK */
  link_control_goto_state(ptr_context_data, UPM_LINK_ESTABLISHING_LLC_LINK);

  /* Order SNDCP Control to establish LLC link */

  /* Pass compression parameter fields only if they are present */
  if( (((T_SM_ACTIVATE_IND *)data)->v_comp_params == TRUE) )
  {
    upm_sndcp_control(ptr_context_data, UPM_I_SNDCP_ESTABLISH_LLC_LINK, 
      &((T_SM_ACTIVATE_IND *)data)->comp_params);
  } else {
    upm_sndcp_control(ptr_context_data, UPM_I_SNDCP_ESTABLISH_LLC_LINK, NULL);
  }
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_llc_link_established
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LLC_LINK_ESTABLISHED in S8
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_llc_link_established(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_llc_link_established");

  send_sm_activate_res(ptr_context_data, ((T_SN_ACTIVATE_CNF*)data));

  /* Go to state UPM LINK LLC LINK ESTABLISHED */
  link_control_goto_state(ptr_context_data, UPM_LINK_LLC_LINK_ESTABLISHED);

#ifndef UPM_WITHOUT_USER_PLANE
  /* Report initial LINK UP to Uplink Data */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_UP, NULL);

  /* Report link up to Downlink Data Control.  This triggers flow control
   * initialization towards SNDCP. */
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_UP, NULL);
#endif /* UPM_WITHOUT_USER_PLANE */
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_modify_ind_in_gsm
+------------------------------------------------------------------------------
| Description : Handle SM_MODIFY_IND primitive while in GSM RAT.
|
| Parameters  : ptr_context_data - Context data
|               data             - SM_MODIFY_IND primitive
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_sm_modify_ind_in_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  T_SM_MODIFY_IND *prim = (T_SM_MODIFY_IND *)data;

  (void)TRACE_FUNCTION("UPM Link Control: handle_sm_modify_ind_in_gsm");

  TRACE_ASSERT( data != NULL );
  
  /*lint -e613 (Possible use of null pointer 'unknown-name' in left argument to operator '->') */
  /* Store GSM-RAT relevant QoS parameters in ptr_context_data fields. */
  handle_gsm_qos_store(ptr_context_data, prim->sapi, prim->radio_prio,
                       (U8)prim->pkt_flow_id, prim->ctrl_qos, &prim->qos);

  /* SNDCP Control constructs a modify primitive to SNDCP */
  upm_sndcp_control(ptr_context_data, UPM_I_SNDCP_MODIFY_LLC_LINK,
                    &((T_SM_MODIFY_IND *)data)->qos.qos_r99);
  /*lint +e613 (Possible use of null pointer 'unknown-name' in left argument to operator '->') */
  
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_response_in_gsm
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_LLC_LINK_MODIFIED
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_modify_response_in_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_modify_response_in_gsm");

  /* Send SM_MODIFY_RES */
  send_sm_modify_res(ptr_context_data);
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_in_gsm
+------------------------------------------------------------------------------
| Description : Handle SM_DEACTIVATE_IND while active in GSM RAT
|
| Parameters  : ptr_context_data - Context data
|               data             - Local flag (rel_ind)
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_deactivate_in_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_deactivate_in_gsm");

  /* Go to state UPM LINK RELEASING LLC LINK */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASING_LLC_LINK);

#ifndef UPM_WITHOUT_USER_PLANE
  /* Terminate Uplink and Downlink state machines. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_DOWN, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_DOWN, NULL);
#endif /* UPM_WITHOUT_USER_PLANE */

  /* SNDCP Control constructs a deactivate primitive for SNDCP */
  upm_sndcp_control(ptr_context_data, UPM_I_SNDCP_RELEASE_LLC_LINK, data);
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate_without_link_in_gsm
+------------------------------------------------------------------------------
| Description : Handle SM_DEACTIVATE_IND in GSM, when no link exists (S5)
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_deactivate_without_link_in_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_deactivate_without_link_in_gsm");

  /* SM_DEACTIVATE_RES sent in dispatcher. One primitive for all NSAPIs. */
  /* send_sm_deactivate_res( ... ); */
  upm_link_mark_nsapi_deactivated(ptr_context_data->nsapi);

  /* Go to state UPM LINK RELEASED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_llc_release_normal
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_LLC_LINK_RELEASED in S10
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_llc_release_normal(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_llc_release_normal");

  /* SM_DEACTIVATE_RES sent in dispatcher. One primitive for all NSAPIs. */
  /* send_sm_deactivate_res( ... ); */
  upm_link_mark_nsapi_deactivated(ptr_context_data->nsapi);

  /* Go to state UPM LINK RELEASED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_llc_release_abnormal
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_LLC_LINK_RELEASED in state other than S10
|
| Parameters  : ptr_context_data - Context data
|               data             - SN_STATUS_IND primitive (unused)
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_llc_release_abnormal(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ /*@unused@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_llc_release_abnormal");

  /* SM_STATUS_REQ sent once for all affected NSAPIs in sndcp_control. */
  /* send_sm_status_req(...) */

  /* Go to state UPM LINK RELEASED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);

#ifndef UPM_WITHOUT_USER_PLANE
  /* Terminate Uplink and Downlink state machines. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_DOWN, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_DOWN, NULL);
#endif /* UPM_WITHOUT_USER_PLANE */
}
#endif /* TI_GPRS */

/*
+------------------------------------------------------------------------------
| Function    : handle_rat_change_to_gsm_without_context
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_RAT_CHANGED(GSM) in S1 or S2
|
| Parameters  : ptr_context_data - Context Data
|               data             - GSM/UMTS
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rat_change_to_gsm_without_context(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rat_change_to_gsm_without_context");
  TRACE_ASSERT( (T_PS_rat)data == PS_RAT_GSM );

  /* Go to state UPM LINK AWAITING ACTIVATE GSM */
  link_control_goto_state(ptr_context_data, UPM_LINK_AWAITING_ACTIVATE_GSM);

  /* Order RAB Control to await RAB release,
   * or to stop awaiting RAB establish */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_RAB_RELEASE, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_rat_change_to_gsm
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_RAT_CHANGED(GSM) in S3
|
| Parameters  : ptr_context_data - Context data
|               data             - GSM/UMTS
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rat_change_to_gsm(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rat_change_to_gsm");

  TRACE_ASSERT( (T_PS_rat)data == PS_RAT_GSM );

  /* Go to state UPM LINK COMPLETING RAT CHANGE TO GSM */
  link_control_goto_state(ptr_context_data, UPM_LINK_COMPLETING_RAT_CHANGE_TO_GSM);

  /* Order RAB Control to await RAB release */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_RAB_RELEASE, NULL);

  /* Order SNDCP Control to await LLC link establishment */
  upm_sndcp_control(ptr_context_data, UPM_I_SNDCP_AWAIT_ESTABLISH, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_rab_established_in_S6
+------------------------------------------------------------------------------
| Description : Handle UPM_I_RAB_ESTABLISHED in S7
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rab_established_in_S6(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rab_established_in_S6");

  /* Go to state UPM LINK RAB ESTABLISHED */
  link_control_goto_state(ptr_context_data, UPM_LINK_RAB_ESTABLISHED);

  /* Send SM_ACTIVATE_RES */
  send_sm_activate_res(ptr_context_data, ((T_SN_ACTIVATE_CNF*)data));

  /* Inform Uplink and Downlink state machines. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_LINK_UP, NULL);
  upm_downlink_data_control(ptr_context_data, UPM_I_DOWNLINK_DATA_LINK_UP, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_rat_change_to_umts_without_context
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_RAT_CHANGED(UMTS) in S7
|
| Parameters  : ptr_context_data - Context data
|               data             - GSM/UMTS
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rat_change_to_umts_without_context(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rat_change_to_umts_without_context");

  TRACE_ASSERT( (T_PS_rat)data == PS_RAT_UMTS_FDD );

  /* Go to state UPM LINK ESTABLISHING RAB */
  link_control_goto_state(ptr_context_data, UPM_LINK_ESTABLISHING_RAB);

  /* Order RAB Control to await RAB establishment */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_INITIAL_ESTABLISH, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_rat_change_to_umts_without_context
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_RAT_CHANGED(UMTS) in S8
|
| Parameters  : ptr_context_data - Context data
|               data             - GSM/UMTS
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rat_change_to_umts_while_establishing(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rat_change_to_umts_while_establishing");

  TRACE_ASSERT( (T_PS_rat)data == PS_RAT_UMTS_FDD );

  /* Go to state UPM LINK COMPLETING RAT CHANGE TO UMTS INITIAL */
  link_control_goto_state(ptr_context_data, UPM_LINK_COMPLETING_RAT_CHANGE_TO_UMTS_INITIAL);

  /* Order RAB Control to await RAB establishment */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_INITIAL_ESTABLISH, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_rat_change_to_umts
+------------------------------------------------------------------------------
| Description : Handle UPM_I_LINK_RAT_CHANGED(UMTS) in S9
|
| Parameters  : ptr_context_data - Context data
|               data             - GSM/UMTS
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
static void handle_rat_change_to_umts(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  (void)TRACE_FUNCTION("UPM Link Control: handle_rat_change_to_umts");

  TRACE_ASSERT( (T_PS_rat)data == PS_RAT_UMTS_FDD );

  /* Go to state UPM LINK RAB ESTABLISHED*/
  link_control_goto_state(ptr_context_data, UPM_LINK_RAB_ESTABLISHED);

  /* Order RAB Control to await RAB establishment */
  upm_rab_control(ptr_context_data, UPM_I_RAB_AWAIT_RAB_ESTABLISH, NULL);
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : handle_gsm_qos_store
+------------------------------------------------------------------------------
| Description : Store GSM-RAT relevant QoS parameters for the NSAPI
|
| Parameters  : ptr_context_data - Context data
|               qos              - R99 format QoS data
+------------------------------------------------------------------------------
*/
#ifdef TI_GPRS
static void handle_gsm_qos_store(struct T_CONTEXT_DATA *ptr_context_data,
                                 U8 sapi, U8 radio_prio, U8 pkt_flow_id,
                                 T_PS_ctrl_qos ctrl_qos, T_PS_qos *qos)
{
  struct T_UPM_CONTEXT_DATA_GPRS *gprs_data = &ptr_context_data->gprs_data;

  (void)TRACE_FUNCTION("UPM Link Control: handle_gsm_qos_store");

  gprs_data->sndcp_llc_sapi   = sapi;
  gprs_data->sndcp_radio_prio = radio_prio;

  /* [3G 24.008] version 6 says to keep PFI if omitted. */
  if (pkt_flow_id != (U8)PS_PKT_FLOW_ID_NOT_PRES) {
    gprs_data->pkt_flow_id      = pkt_flow_id;
  }

  if (ctrl_qos == PS_is_R97)
  {
    gprs_data->sndcp_delay        = qos->qos_r97.delay;
    gprs_data->sndcp_relclass     = qos->qos_r97.relclass;
    gprs_data->sndcp_peak_bitrate = qos->qos_r97.peak;
    gprs_data->sndcp_preced       = qos->qos_r97.preced;
    gprs_data->sndcp_mean         = qos->qos_r97.mean;
  } else if (ctrl_qos == PS_is_R99) {
    T_PS_qos_r97 temp_qos_r97;

    cl_qos_convert_r99_to_r97(&qos->qos_r99, &temp_qos_r97);

    gprs_data->sndcp_delay        = temp_qos_r97.delay;
    gprs_data->sndcp_relclass     = temp_qos_r97.relclass;
    gprs_data->sndcp_peak_bitrate = temp_qos_r97.peak;
    gprs_data->sndcp_preced       = temp_qos_r97.preced;
    gprs_data->sndcp_mean         = temp_qos_r97.mean;
  } else {
    (void)TRACE_ERROR("Link Control (handle_gsm_qos_store): Invalid QoS union controller!");
  }
}
#endif /* TI_GPRS */
/*
+------------------------------------------------------------------------------
| Function    : upm_link_mark_nsapi_deactivated
+------------------------------------------------------------------------------
| Description : Helper function to group nsapis in SM_DEACTIVATE_RES
|
| Parameters  : nsapi            - NSAPI        
+------------------------------------------------------------------------------
*/
static U16 nsapis_to_deactivate = 0, nsapis_currently_deactivated = 0;

static void upm_link_mark_nsapi_deactivated(U8 nsapi)
{
  (void)TRACE_FUNCTION("UPM Link Control: upm_link_mark_nsapi_deactivated");

  nsapis_currently_deactivated |= upm_nsapi2nsapi_set(nsapi);

  if (nsapis_currently_deactivated == nsapis_to_deactivate)
  {
    send_sm_deactivate_res(nsapis_to_deactivate);
    /* All context requested deactivated have been deactivated */
    /* Zero out store */
    nsapis_to_deactivate = 0;
    nsapis_currently_deactivated = 0;
  }
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : upm_is_link_released
+------------------------------------------------------------------------------
| Description : Link Control state query function. Return TRUE if Link Control
|               is in state UPM LINK RELEASED; returns FALSE otherwise.
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
BOOL upm_is_link_released(struct T_CONTEXT_DATA *ptr_context_data)
{
  TRACE_ASSERT(ptr_context_data != NULL);

  if (link_control_state(ptr_context_data) == UPM_LINK_RELEASED)
  {
    (void)TRACE_FUNCTION("UPM Link Control: upm_is_link_released (TRUE)");
    return TRUE;
  } else {
    (void)TRACE_FUNCTION("UPM Link Control: upm_is_link_released (FALSE)");
    return FALSE;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_link_dispatch_sm_deactivate_ind
+------------------------------------------------------------------------------
| Description : Dispatch SM_DEACTIVATE_IND
|
| Parameters  : sm_deactivate_ind          - received primitive
+------------------------------------------------------------------------------
*/
void upm_link_dispatch_sm_deactivate_ind(T_SM_DEACTIVATE_IND *sm_deactivate_ind)
{
  int                    nsapi;
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("UPM Link Control: upm_link_dispatch_sm_deactivate_ind");

  /* Add NSAPI to nsapi_set to deactivate */
  nsapis_to_deactivate         |= sm_deactivate_ind->nsapi_set;

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    if ( (sm_deactivate_ind->nsapi_set & upm_nsapi2nsapi_set(nsapi)) != 0)
    {
      ptr_context_data = upm_get_context_data_from_nsapi(nsapi);
      if (ptr_context_data != NULL)
      {
        if (upm_is_link_released(ptr_context_data))
        {
          nsapis_currently_deactivated |= upm_nsapi2nsapi_set(nsapi);
        } else {
          upm_link_control(ptr_context_data, UPM_P_SM_DEACTIVATE_IND,
                           (void *)sm_deactivate_ind->rel_ind);
        }

        upm_check_for_released_context_and_release(ptr_context_data);
      } else {
        (void)TRACE_ERROR("Tried to deactivate non-existent context!");
      }
    }
  } /* for */
}

/*
+------------------------------------------------------------------------------
| Function    : upm_link_control_init
+------------------------------------------------------------------------------
| Description : Link Control state machine initialization function
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void upm_link_control_init(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("UPM Link Control: upm_link_control_init");

  TRACE_ASSERT(ptr_context_data != NULL);

  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);

  /* Zero out store for deactivation synchronization */
  nsapis_to_deactivate                 = 0;
  nsapis_currently_deactivated         = 0;
}

/*
+------------------------------------------------------------------------------
| Function    : upm_link_control_exit
+------------------------------------------------------------------------------
| Description : Link Control state machine exit function
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void upm_link_control_exit(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("UPM Link Control: upm_link_control_exit");

  TRACE_ASSERT(ptr_context_data != NULL);

  link_control_goto_state(ptr_context_data, UPM_LINK_RELEASED);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_link_control
+------------------------------------------------------------------------------
| Description : Link Control state machine
|
| Parameters  : ptr_context_data - Pointer to context data
|               event            - Internal event (see upm_link_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void upm_link_control (struct T_CONTEXT_DATA *ptr_context_data,
                      T_UPM_LINK_CONTROL_EVENT event,
                      /*@null@*/ void *data)
{
#ifdef DEBUG
  T_UPM_LINK_CONTROL_STATE old_state;
  /*@observer@*/ /*@notnull@*/
  static const char *state_name[UPM_LINK_CONTROL_NUMBER_OF_STATES] = {
    "S0_UPM_LINK_RELEASED",
#ifdef TI_UMTS
    "S1_UPM_LINK_ESTABLISHING_RAB",
    "S2_UPM_LINK_AWAITING_ACTIVATE_UMTS",
    "S3_UPM_LINK_RAB_ESTABLISHED",
    "S4_UPM_LINK_RELEASING_RAB",
#endif /* TI_UMTS */
#ifdef TI_DUAL_MODE
    "S5_UPM_LINK_COMPLETING_RAT_CHANGE_TO_GSM",
    "S6_UPM_LINK_COMPLETING_RAT_CHANGE_TO_UMTS_INITIAL",
#endif /* TI_DUAL_MODE */
#ifdef TI_GPRS
    "S7_UPM_LINK_AWAITING_ACTIVATE_GSM",
    "S8_UPM_LINK_ESTABLISHING_LLC_LINK",
    "S9_UPM_LINK_LLC_LINK_ESTABLISHED",
    "S10_UPM_LINK_RELEASING_LLC_LINK"
#endif /* TI_GPRS */
  };
  /*@observer@*/ /*@notnull@*/
  static const char *event_name[UPM_LINK_CONTROL_NUMBER_OF_EVENTS] = {
    "UPM_P_SM_ACTIVATE_STARTED_IND",
    "UPM_P_SM_ACTIVATE_IND",
    "UPM_P_SM_DEACTIVATE_IND",
    "UPM_P_SM_MODIFY_IND",
    "UPM_I_LINK_DTI_CLOSED",
    "UPM_I_RAB_ESTABLISHED",
    "UPM_I_RAB_RELEASED",
    "UPM_I_SNDCP_LLC_LINK_ESTABLISHED",
    "UPM_I_SNDCP_LLC_LINK_MODIFIED",
    "UPM_I_SNDCP_LLC_LINK_RELEASED",
    "UPM_I_LINK_RAT_CHANGED"
  };

  TRACE_ASSERT(ptr_context_data != NULL);
  TRACE_ASSERT(event < UPM_LINK_CONTROL_NUMBER_OF_EVENTS);

  old_state = link_control_state(ptr_context_data);
  TRACE_ASSERT(old_state < UPM_LINK_CONTROL_NUMBER_OF_STATES);

  if (transition[(U16)old_state][(U16)event].event != event)
  {
    (void)TRACE_ERROR("Event table error in upm_link_control!");
  }
#endif /* DEBUG */

  transition[(U16)link_control_state(ptr_context_data)][(U16)event].func(ptr_context_data, data);

#ifdef DEBUG
  (void)TRACE_EVENT_P4("LINK  #%d: %s => %s to %s",
                       ptr_context_data->nsapi,
                       event_name[(U16)event],
                       state_name[(U16)old_state],
                       state_name[(U16)link_control_state(ptr_context_data)]);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
