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
| Purpose:    SNDCP Control state machine implementation in the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "upm.h"

#include "upm_sndcp_control.h"
#include "upm_sndcp_output_handler.h"
#include "upm_aci_output_handler.h"
#include "upm_sm_output_handler.h"
#include "upm_mm_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_UPM_SNDCP_CONTROL_TRANSITION_FUNC)(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

typedef struct {
#ifdef DEBUG
  T_UPM_SNDCP_CONTROL_EVENT            event;
#endif /* DEBUG */
  T_UPM_SNDCP_CONTROL_TRANSITION_FUNC  func;
} T_UPM_SNDCP_CONTROL_TRANSITION;

/*==== LOCALS ===============================================================*/

static void state_event_error   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void ignored             (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_passive_activate(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_active_activate(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_passive_activate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_active_activate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_deactivate   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sn_deactivate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_modify_llc_link(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_sn_modify_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_mmpm_sequence_ind(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);
static void handle_status_ind   (struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data);

/***********************************************************************
 * State/Transition Table
 */
static const T_UPM_SNDCP_CONTROL_TRANSITION
transition[UPM_SNDCP_CONTROL_NUMBER_OF_STATES]
          [UPM_LINK_CONTROL_NUMBER_OF_EVENTS] =
{
  { /* S0: UPM SNDCP INACTIVE */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          state_event_error),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        state_event_error),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            state_event_error),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            state_event_error),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        state_event_error),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    handle_passive_activate),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, handle_active_activate),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   state_event_error)
  },
  { /* S1: UPM SNDCP AWAIT ACTIVATE */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          handle_passive_activate_cnf),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        state_event_error),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            state_event_error),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            handle_status_ind),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        state_event_error),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   handle_deactivate)
  },
  { /* S2: UPM SNDCP ACTIVATING */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          handle_active_activate_cnf),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        state_event_error),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            state_event_error),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            handle_status_ind),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        state_event_error),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   handle_deactivate)
  },
  { /* S3: UPM SNDCP ACTIVE */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          state_event_error),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        state_event_error),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            state_event_error),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            handle_status_ind),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        handle_mmpm_sequence_ind),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    handle_modify_llc_link),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   handle_deactivate)
  },
  { /* S4: UPM SNDCP MODIFYING */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          state_event_error),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        state_event_error),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            handle_sn_modify_cnf),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            handle_status_ind),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        handle_mmpm_sequence_ind),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   handle_deactivate)
  },
  { /* S5: UPM SNDCP DEACTIVATING */
    M_TRANSITION(UPM_P_SN_ACTIVATE_CNF,          state_event_error),
    M_TRANSITION(UPM_P_SN_DEACTIVATE_CNF,        handle_sn_deactivate_cnf),
    M_TRANSITION(UPM_P_SN_MODIFY_CNF,            ignored),
    M_TRANSITION(UPM_P_SN_STATUS_IND,            handle_status_ind),
    M_TRANSITION(UPM_P_MMPM_SEQUENCE_IND,        state_event_error),
    M_TRANSITION(UPM_I_SNDCP_AWAIT_ESTABLISH,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_ESTABLISH_LLC_LINK, state_event_error),
    M_TRANSITION(UPM_I_SNDCP_MODIFY_LLC_LINK,    state_event_error),
    M_TRANSITION(UPM_I_SNDCP_RELEASE_LLC_LINK,   ignored)
  }
};

/*==== PRIVATE FUNCTIONS ====================================================*/

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
static void state_event_error(/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_ERROR("UPM SNDCP Control: STATE EVENT ERROR!");
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
static void ignored (/*@unused@*/ struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("UPM SNDCP Control: Event ignored.");
}

/*
+------------------------------------------------------------------------------
| Function    : handle_passive_activate
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_AWAIT_ESTABLISH in S0
|
| Parameters  : ptr_context_data - Context data
|               data             - Pointer to compression parameters
+------------------------------------------------------------------------------
*/
static void handle_passive_activate(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_passive_activate");

  /* Send SN_ACTIVATE_REQ with establish flag == FALSE */
  send_sn_activate_req(ptr_context_data, (T_NAS_comp_params *)data, FALSE);

  /* Go to state UPM SNDCP AWAIT ACTIVATE */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_AWAIT_ACTIVATE;

#ifndef UPM_WITHOUT_USER_PLANE
  /* Suspend Uplink Data */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_SUSPEND,
                          (void *)UPM_LINK);
#endif /* UPM_WITHOUT_USER_PLANE */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_active_activate
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_ESTABLISH_LLC_LINK in S0
|
| Parameters  : ptr_context_data - Context data
|               data             - Pointer to compression parameters
+------------------------------------------------------------------------------
*/
static void handle_active_activate(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_active_activate");

  /* Send SN_ACTIVATE_REQ with establish flag == TRUE */
  send_sn_activate_req(ptr_context_data, (T_NAS_comp_params *)data, TRUE);

  /* Go to state UPM SNDCP ACTIVATING */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_ACTIVATING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_passive_activate_cnf
+------------------------------------------------------------------------------
| Description : Handle UPM_P_SN_ACTIVATE_CNF in S1
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_passive_activate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_passive_activate_cnf");

  /* Go to state UPM SNDCP ACTIVE */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_ACTIVE;

#ifndef UPM_WITHOUT_USER_PLANE
  /* Resume Uplink Data */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_RESUME,
                          (void *)UPM_LINK);
#endif /* UPM_WITHOUT_USER_PLANE */

  /* Report link establishment to Link Control */
  upm_link_control(ptr_context_data, UPM_I_SNDCP_LLC_LINK_ESTABLISHED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_active_activate_cnf
+------------------------------------------------------------------------------
| Description : Handle UPM_P_SN_ACTIVATE_CNF in S2
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_active_activate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_active_activate_cnf");

  /* Go to state UPM SNDCP ACTIVE */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_ACTIVE;

  /* Report link establishment to Link Control */ 
  upm_link_control(ptr_context_data, UPM_I_SNDCP_LLC_LINK_ESTABLISHED, 
    ((T_SN_ACTIVATE_CNF*)data)); /*To pass comp params*/
}

/*
+------------------------------------------------------------------------------
| Function    : handle_deactivate
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_RELEASE_LLC_LINK in S1, S2, S3, or S4.
|
| Parameters  : ptr_context_data - Context data
|               data             - Local flag (rel_ind)
+------------------------------------------------------------------------------
*/
static void handle_deactivate(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void *data)
{
  U32  rel_ind = (U32)data;
  (void)TRACE_FUNCTION("handle_deactivate");

  /* SN_DEACTIVATE_REQ no longer sent in dispatcher function, so send here. */
  send_sn_deactivate_req( ptr_context_data->nsapi, (U8)rel_ind);

  /* Go to state UPM SNDCP DEACTIVATING */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_DEACTIVATING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sn_deactivate_cnf
+------------------------------------------------------------------------------
| Description : Handle UPM_P_SN_DEACTIVATE_CNF in S5
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_sn_deactivate_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_sn_deactivate_cnf");

  /* Go to state UPM SNDCP INACTIVE */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_INACTIVE;

  /* Report LLC link release to Link Control */
  upm_link_control(ptr_context_data, UPM_I_SNDCP_LLC_LINK_RELEASED, NULL);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_modify_llc_link
+------------------------------------------------------------------------------
| Description : Handle UPM_I_SNDCP_MODIFY_LLC_LINK
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_modify_llc_link(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_modify_llc_link");

  /* At this point, relevant flags in Context Data have been set by Link Control */

  /* Send SN_MODIFY_REQ */
  send_sn_modify_req(ptr_context_data);

  /* Go to state UPM SNDCP MODIFYING */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_MODIFYING;
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sn_modify_cnf
+------------------------------------------------------------------------------
| Description : Handle UPM_P_SN_MODIFY_CNF in S4
|
| Parameters  : ptr_context_data - Context data
|               data             - Not used
+------------------------------------------------------------------------------
*/
static void handle_sn_modify_cnf(struct T_CONTEXT_DATA *ptr_context_data, /*@unused@*/ /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_sn_modify_cnf");

  /* Go to state UPM SNDCP ACTIVE */
  ptr_context_data->sndcp_control_state = UPM_SNDCP_ACTIVE;

  /* Report LLC link modification completion to Link Control */
  upm_link_control(ptr_context_data, UPM_I_SNDCP_LLC_LINK_MODIFIED, NULL);
}


/*
+------------------------------------------------------------------------------
| Function    : handle_mmpm_sequence_ind
+------------------------------------------------------------------------------
| Description : Handle UPM_P_MMPM_SEQUENCE_IND
|
| Parameters  : ptr_context_data - Context data
|               data             - sequence number
+------------------------------------------------------------------------------
*/
static void handle_mmpm_sequence_ind(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void* data)
{
  U32  seq_no = (U32)data;
  (void)TRACE_FUNCTION("handle_mmpm_sequence_ind");

  /* Set sequence number for this NSAPI */
  upm_set_sequence_number(ptr_context_data, (U8)seq_no);

#ifndef UPM_WITHOUT_USER_PLANE
  /* Uplink Data Control should discard data older than the sequence number
   * given in data for this NSAPI. */
  upm_uplink_data_control(ptr_context_data, UPM_I_UPLINK_DATA_DISCARD,
                          data);
#endif /* UPM_WITHOUT_USER_PLANE */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_status_ind
+------------------------------------------------------------------------------
| Description : Handle UPM_P_SN_STATUS_IND
|
| Parameters  : ptr_context_data - Context data
|               data             - SN_STATUS_IND primitive
+------------------------------------------------------------------------------
*/
static void handle_status_ind(struct T_CONTEXT_DATA *ptr_context_data, /*@null@*/ void* data)
{
  (void)TRACE_FUNCTION("handle_status_ind");

  TRACE_ASSERT( data != NULL );

  /* If cause is any other than NO_PEER_RESPONSE, SNDCP Control should
   * deactivate SNDCP. */
  /*lint -e613 (Possible use of null pointer 'unknown-name' in left argument to operator '->') */
  if ( (((T_SN_STATUS_IND *)data)->ps_cause.ctrl_value != CAUSE_is_from_llc ||
        ((T_SN_STATUS_IND *)data)->ps_cause.value.llc_cause 
                                 !=(U16)CAUSE_LLC_NO_PEER_RES ) &&
       (((T_SN_STATUS_IND *)data)->ps_cause.ctrl_value != CAUSE_is_from_sndcp ||
        ((T_SN_STATUS_IND *)data)->ps_cause.value.sn_cause 
                                 != (U16)CAUSE_SN_NO_PEER_RESPONSE )  )
  {
    /* Go to state UPM SNDCP INACTIVE */
    ptr_context_data->sndcp_control_state = UPM_SNDCP_INACTIVE;

   /*lint +e613 (Possible use of null pointer 'unknown-name' in left argument to operator '->') */
    /* Report link release to Link Control */
    upm_link_control(ptr_context_data, UPM_I_SNDCP_LLC_LINK_RELEASED, data);
  }
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_dispatch_mmpm_sequence_ind
+------------------------------------------------------------------------------
| Description : Helper function for Dispatcher. Dispatches MMPM_SEQUENCE_IND
|
| Parameters  : ptr_prim_data    - Pointer to primitive
+------------------------------------------------------------------------------
*/
void upm_sndcp_dispatch_mmpm_sequence_ind(T_MMPM_SEQUENCE_IND *ptr_prim_data)
{
  int               entries;
  T_MMPM_npdu_list *entry;

  (void)TRACE_FUNCTION("upm_sndcp_dispatch_mmpm_sequence_ind");

  entries = (int)ptr_prim_data->c_npdu_list;
  entry   = ptr_prim_data->npdu_list;

  /* Loop over NSAPIs in primitive, and store sequence numbers. */
  while (entries > 0) {
    struct T_CONTEXT_DATA *context;

    context = upm_get_context_data_from_nsapi(entry->nsapi);
    if (context != NULL) {
      upm_sndcp_control(context, UPM_P_MMPM_SEQUENCE_IND,
                        (void *)entry->receive_n_pdu_number_val);
    } else {
      (void)TRACE_ERROR("Received sequence numbers for non-existent context!");
    }

    entries--;
  }

  send_sn_sequence_req(ptr_prim_data);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_dispatch_sn_status_ind
+------------------------------------------------------------------------------
| Description : Helper function for Dispatcher. Dispatches SN_STATUS_IND
|
| Parameters  : ptr_prim_data    - Pointer to primitive
+------------------------------------------------------------------------------
*/
void upm_sndcp_dispatch_sn_status_ind(T_SN_STATUS_IND *prim)
{
  int                     nsapi;
  struct T_CONTEXT_DATA  *context;

  (void)TRACE_FUNCTION("upm_sndcp_dispatch_sn_status_ind");

  /* If cause is any other than NO_PEER_RESPONSE, SNDCP Control should
   * deactivate SNDCP and inform Session Management. */
  if ( (prim->ps_cause.ctrl_value != CAUSE_is_from_llc ||
        prim->ps_cause.value.llc_cause != (U16)CAUSE_LLC_NO_PEER_RES ) &&
       (prim->ps_cause.ctrl_value != CAUSE_is_from_sndcp ||
        prim->ps_cause.value.sn_cause != (U16)CAUSE_SN_NO_PEER_RESPONSE ) 
     ) {
    /* Alert state machine */
    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++) {
      if ( (prim->nsapi_set & upm_nsapi2nsapi_set(nsapi)) != 0 ) {
        context = upm_get_context_data_from_nsapi(nsapi);
	if (context != NULL) {
	  upm_sndcp_control(context, UPM_P_SN_STATUS_IND, prim);

          /* Release context data if applicable (link and DTI both down) */
          upm_check_for_released_context_and_release(context);
	}
      }
    }

    /* Inform Session Management -- Once for all NSAPIs affected */
    send_sm_status_req(prim->nsapi_set, (T_PS_sapi)prim->sapi,
                       prim->ps_cause.ctrl_value, prim->ps_cause.value.sn_cause);
  }
  /* No action if cause is NO_PEER_RESPONSE */
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_dispatch_sn_sequence_cnf
+------------------------------------------------------------------------------
| Description : Helper function for Dispatcher. Dispatches SN_SEQUENCE_CNF
|
| Parameters  : ptr_prim_data    - Pointer to primitive
+------------------------------------------------------------------------------
*/
void upm_sndcp_dispatch_sn_sequence_cnf(T_SN_SEQUENCE_CNF *prim)
{
  (void)TRACE_FUNCTION("upm_sndcp_dispatch_sn_sequence_cnf");

  /* No action needed but forwarding sequence numbers to MM */
  send_mmpm_sequence_res(prim);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_dispatch_sn_count_cnf
+------------------------------------------------------------------------------
| Description : Helper function for Dispatcher. Dispatches SN_COUNT_CNF
|
| Parameters  : prim             - Pointer to primitive
+------------------------------------------------------------------------------
*/
void upm_sndcp_dispatch_sn_count_cnf(T_SN_COUNT_CNF *prim)
{
  (void)TRACE_FUNCTION("upm_sndcp_dispatch_sn_count_cnf");

  /* No action needed but forwarding as UPM_COUNT_CNF */
  send_upm_count_cnf(prim->nsapi, prim->octets_uplink, prim->octets_downlink,
                     prim->packets_uplink, prim->packets_downlink);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_dispatch_upm_count_req
+------------------------------------------------------------------------------
| Description : Helper function for Dispatcher. Dispatches UPM_COUNT_REQ
|
| Parameters  : prim             - Pointer to primitive
+------------------------------------------------------------------------------
*/
void upm_sndcp_dispatch_upm_count_req(T_UPM_COUNT_REQ *prim)
{
  (void)TRACE_FUNCTION("upm_sndcp_dispatch_upm_count_req");

  /* No action needed but forwarding as SN_COUNT_REQ */
  send_sn_count_req(prim->nsapi, prim->reset);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_control_init
+------------------------------------------------------------------------------
| Description : SNDCP Control state machine initialization function
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void upm_sndcp_control_init(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("upm_sndcp_control_init");

  TRACE_ASSERT(ptr_context_data != NULL);
  /*lint -e613 (Possible use of null pointer 'ptr_context_data' in left argument to operator '->') */
  ptr_context_data->sndcp_control_state   = UPM_SNDCP_INACTIVE;
  ptr_context_data->gprs_data.pkt_flow_id = (U8)PS_PKT_FLOW_ID_NOT_PRES; 
  /*lint +e613 (Possible use of null pointer 'ptr_context_data' in left argument to operator '->') */ 	
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_control_exit
+------------------------------------------------------------------------------
| Description : SNDCP Control state machine exit function
|
| Parameters  : ptr_context_data - Pointer to context data
+------------------------------------------------------------------------------
*/
void upm_sndcp_control_exit(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("upm_sndcp_control_exit");

  TRACE_ASSERT(ptr_context_data != NULL);
/*lint -e613 (Possible use of null pointer 'ptr_context_data' in left argument to operator '->') */
  ptr_context_data->sndcp_control_state   = UPM_SNDCP_INACTIVE;
  ptr_context_data->gprs_data.pkt_flow_id = (U8)PS_PKT_FLOW_ID_NOT_PRES;
  /*lint +e613 (Possible use of null pointer 'ptr_context_data' in left argument to operator '->') */
}

/*
+------------------------------------------------------------------------------
| Function    : upm_sndcp_control
+------------------------------------------------------------------------------
| Description : SNDCP Control state machine
|
| Parameters  : ptr_context_data - Pointer to context data
|               event            - Internal event (see upm_sndcp_control.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void upm_sndcp_control (struct T_CONTEXT_DATA *ptr_context_data,
                        T_UPM_SNDCP_CONTROL_EVENT event,
                        /*@null@*/ void* data)
{
#ifdef DEBUG
  T_UPM_SNDCP_CONTROL_STATE old_state;
  /*@observer@*/
  static const char *state_name[UPM_SNDCP_CONTROL_NUMBER_OF_STATES] = {
    "S0_UPM_SNDCP_INACTIVE",
    "S1_UPM_SNDCP_AWAIT_ACTIVATE",
    "S2_UPM_SNDCP_ACTIVATING",
    "S3_UPM_SNDCP_ACTIVE",
    "S4_UPM_SNDCP_MODIFYING",
    "S5_UPM_SNDCP_DEACTIVATING"
  };
  /*@observer@*/
  static const char *event_name[UPM_LINK_CONTROL_NUMBER_OF_EVENTS] = {
    "UPM_P_SN_ACTIVATE_CNF",
    "UPM_P_SN_DEACTIVATE_CNF",
    "UPM_P_SN_MODIFY_CNF",
    "UPM_P_SN_STATUS_IND",
    "UPM_P_MMPM_SEQUENCE_IND",
    "UPM_I_SNDCP_AWAIT_ESTABLISH",
    "UPM_I_SNDCP_ESTABLISH_LLC_LINK",
    "UPM_I_SNDCP_MODIFY_LLC_LINK",
    "UPM_I_SNDCP_RELEASE_LLC_LINK"
  };

  TRACE_ASSERT(ptr_context_data != NULL);
  TRACE_ASSERT(event < UPM_SNDCP_CONTROL_NUMBER_OF_EVENTS);

  old_state = ptr_context_data->sndcp_control_state;

  if (transition[(U16)old_state][(U16)event].event != event) {
    (void)TRACE_ERROR("Event table error in upm_sndcp_control!");
  }
#endif /* DEBUG */

  transition[(U16)ptr_context_data->sndcp_control_state][(U16)event].func(ptr_context_data, data);

#ifdef DEBUG
  (void)TRACE_EVENT_P4("SNDCP #%d: %s => %s to %s",
                       ptr_context_data->nsapi,
                       event_name[(U16)event],
                       state_name[(U16)old_state],
                       state_name[(U16)ptr_context_data->sndcp_control_state]);
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
