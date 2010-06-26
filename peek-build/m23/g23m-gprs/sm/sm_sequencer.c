/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments.
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    Sequencer state machine implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_sequencer.h"
#include "sm_context_control.h"

#include "sm_aci_output_handler.h"
#include "sm_mm_output_handler.h"
#include "sm_qos.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef void (*T_SM_SEQUENCER_TRANSITION_FUNC)(void *data);

typedef struct {
#ifdef DEBUG
  T_SM_SEQUENCER_EVENT         event;
#endif /* DEBUG */
  T_SM_SEQUENCER_TRANSITION_FUNC  func;
} T_SM_SEQUENCER_TRANSITION;

/*==== LOCALS ===============================================================*/

static void handle_mmpm_attach_ind(void *data);
static void handle_mmpm_detach_ind(void *data);
static void handle_smreg_pdp_deactivate_req(void *data);
static void handle_sm_status_req(void *data);
static void handle_context_activation_override(void *data);
static void handle_context_deactivate_completed(void *data);
static void handle_context_tear_down_deactivate(void *data);
static void handle_context_reactivate_completed(void *data);

/*==== PRIVATE FUNCTIONS ====================================================*/

static const
T_SM_SEQUENCER_TRANSITION transition[SM_SEQUENCER_NUMBER_OF_EVENTS] =
{
  M_TRANSITION(SM_P_MMPM_ATTACH_IND,              handle_mmpm_attach_ind),
  M_TRANSITION(SM_P_MMPM_DETACH_IND,              handle_mmpm_detach_ind),
  M_TRANSITION(SM_P_SMREG_PDP_DEACTIVATE_REQ,     handle_smreg_pdp_deactivate_req),
  M_TRANSITION(SM_P_SM_STATUS_REQ,                handle_sm_status_req),
  M_TRANSITION(SM_I_CONTEXT_ACTIVATION_OVERRIDE,  handle_context_activation_override),
  M_TRANSITION(SM_I_CONTEXT_DEACTIVATE_COMPLETED, handle_context_deactivate_completed),
  M_TRANSITION(SM_I_CONTEXT_TEAR_DOWN_DEACTIVATE, handle_context_tear_down_deactivate),
  M_TRANSITION(SM_I_CONTEXT_REACTIVATE_COMPLETED, handle_context_reactivate_completed)
};

static void sm_seq_clear_nsapis_to_deactivate(void)
{
  sm_data.sm_nsapis_requested_deactivated = 0;
  sm_data.sm_nsapis_being_deactivated     = 0;
}

static void sm_seq_set_nsapis_to_deactivate(U16 nsapi_set)
{
  sm_data.sm_nsapis_requested_deactivated |= nsapi_set;
}

static U16 sm_seq_get_nsapis_to_deactivate(void)
{
  return (sm_data.sm_nsapis_requested_deactivated);
}

static BOOL sm_seq_is_nsapi_requested_deactivated(int /*@alt U8@*/ nsapi)
{
  return sm_is_nsapi_in_nsapi_set(nsapi, sm_data.sm_nsapis_requested_deactivated);
}

static void sm_seq_nsapi_started_deactivation(int /*@alt U8@*/ nsapi)
{
  sm_data.sm_nsapis_being_deactivated
    = sm_add_nsapi_to_nsapi_set(nsapi, sm_data.sm_nsapis_being_deactivated);
}

static void sm_seq_nsapi_completed_deactivation(int /*@alt U8@*/ nsapi)
{
  sm_data.sm_nsapis_being_deactivated
    = sm_remove_nsapi_from_nsapi_set(nsapi, sm_data.sm_nsapis_being_deactivated);
}

static BOOL sm_seq_is_nsapi_set_completed(void)
{
  return (sm_data.sm_nsapis_being_deactivated == 0);
}

/*
+------------------------------------------------------------------------------
| Function    : handle_mmpm_attach_ind
+------------------------------------------------------------------------------
| Description : Handle incoming MMPM_ATTACH_IND
|
| Parameters  : data              - T_MMPM_ATTACH_IND *
+------------------------------------------------------------------------------
*/ 

#ifndef TI_PS_OP_SM_RETAIN_BEST_CONTEXT
/*Old method. Secondary contexts deactivated after a R99 to R97 RAU*/
static void handle_mmpm_attach_ind(void *data)
{
  int                nsapi;
  T_PS_sgsn_rel      sgsn_rel;
  T_MMPM_ATTACH_IND *prim = (T_MMPM_ATTACH_IND *)data;
#ifdef DEBUG
  /*@observer@*/
  static const char *sgsn_rel_name[3] = {
    "PS_SGSN_UNKNOWN",
    "PS_SGSN_98_OLDER",
    "PS_SGSN_99_ONWARDS"
  };
#endif

  (void)TRACE_FUNCTION("handle_mmpm_attach_ind");

  sgsn_rel = (T_PS_sgsn_rel)cl_nwrl_get_sgsn_release();

  (void)TRACE_EVENT_P1("SGSN release given by CL is (1->R97, 2->R99) >>> %d", 
                        cl_nwrl_get_sgsn_release());

#ifdef DEBUG
  (void)TRACE_EVENT_P2("Got SGSN release value 0x%02x: %s",
                       sgsn_rel,
                       (sgsn_rel <= PS_SGSN_99_ONWARDS
                        ? sgsn_rel_name[(U16)sgsn_rel]
                        : "BAD VALUE!"));
#endif

  /* Check whether the UE has moved from R99 to R97/R98 network. */
  if (sm_get_current_nw_release() == PS_SGSN_99_ONWARDS &&
      sgsn_rel == PS_SGSN_98_OLDER) {
    /* We have moved from R99 network to pre-R99:
     * check for secondary contexts, or contexts with TI > 6;
     * These must be locally deactivated. */
    U16              nsapis_to_deactivate = 0;

    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      struct T_SM_CONTEXT_DATA *context;

      context = sm_get_context_data_from_nsapi(nsapi);
      if (context != NULL &&
	  (context->ti > (U8)SM_MAX_NON_EXT_TI || sm_is_secondary(context)))
      {
	    nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(nsapi, nsapis_to_deactivate);
            /* Make sure to clear the pending reactivation flag */
            sm_set_context_pending_reactivation(context, FALSE);
	    /* Order context deactivation */
	    sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
	  }
    }

    /* If any contexts were deactivated, inform ACI and MM */
    if (nsapis_to_deactivate != 0)
    {
      T_CAUSE_ps_cause cause;
      cause.ctrl_value      = CAUSE_is_from_sm;
      cause.value.sm_cause  = (U16)CAUSE_SM_PRE_R99_NETWORK_ENTERED;

      send_smreg_pdp_deactivate_ind(nsapis_to_deactivate, &cause);
      send_mmpm_pdp_context_status_req();
    }
  }

  /* Check whether any (primary) contexts are pending reactivation:
   * If so, start reactivation now. */
  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && !sm_is_secondary(context)
        && sm_is_context_pending_reactivation(context))
    {
      /* Clear the pending reactivation flag. Will be done in 
         sm_is_address_changed_with_reactivation */
      /*sm_set_context_pending_reactivation(context, FALSE);*/

      /* Order context reactivation */
      sm_context_control(context, SM_I_CONTEXT_REACTIVATE, NULL);
    }
  }

  /* Set current RAT and core network release */
  sm_set_current_rat       ((T_PS_rat)prim->rat);
  sm_set_current_nw_release(sgsn_rel);

  /*
   * The following line is useful if we try a PDP activation after a 
   * GPRS detach
   */
  sm_data.sm_suspended = FALSE;
  sm_data.sm_suspend_cause = CAUSE_MM_SUCCESS;
}

#else /*#ifndef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/
/*New method. The best context remain after a R99 to R97 RAU*/

static void handle_mmpm_attach_ind(void *data)
{
  int                nsapi;
  T_PS_sgsn_rel      sgsn_rel;
  T_MMPM_ATTACH_IND  *prim = (T_MMPM_ATTACH_IND *)data;
  U16                nsapis_to_deactivate = 0;
  U16                nsapis_deactivated   = 0;

#ifdef DEBUG
  /*@observer@*/
  static const char *sgsn_rel_name[3] = {
    "PS_SGSN_UNKNOWN",
    "PS_SGSN_98_OLDER",
    "PS_SGSN_99_ONWARDS"
  };
#endif

  (void)TRACE_FUNCTION("handle_mmpm_attach_ind");

  sgsn_rel = cl_nwrl_get_sgsn_release();

  (void)TRACE_EVENT_P1("SGSN release given by CL is (1->R97, 2->R99) >>> %d", 
                        cl_nwrl_get_sgsn_release());

#ifdef DEBUG
  (void)TRACE_EVENT_P2("Got SGSN release value 0x%02x: %s",
                       sgsn_rel,
                       (sgsn_rel <= PS_SGSN_99_ONWARDS
                        ? sgsn_rel_name[(U16)sgsn_rel]
                        : "BAD VALUE!"));
#endif

  /* Check whether the UE has moved from R99 to R97/R98 network. */
  if (sm_get_current_nw_release() == PS_SGSN_99_ONWARDS &&
      sgsn_rel == PS_SGSN_98_OLDER) {
    /* We have moved from R99 network to pre-R99:
     * First round of elimination. Check for contexts with TI > 6;
     * These must be locally deactivated. 
     */

    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      struct T_SM_CONTEXT_DATA *context;

      context = sm_get_context_data_from_nsapi(nsapi);
      if (context != NULL && (context->ti > (U8)SM_MAX_NON_EXT_TI ))
      {
        nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(nsapi, 
                                         nsapis_to_deactivate);
        /* Make sure to clear the pending reactivation flag */
        sm_set_context_pending_reactivation(context, FALSE);
        /* Order context deactivation */
        sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
      }		
    }

   /* Second round of elimination. Rank contexts based on Traffic class and 
    * traffic handling priority. Locally deactivate all but the best ranking 
    * context(s). Ref 23.107 Annex C
    */
    nsapis_deactivated   =  sm_rank_del_contexts_based_on_tc(); 
    nsapis_to_deactivate = (nsapis_to_deactivate | nsapis_deactivated);

   /* Third round of elimination. Context(s) with highest values of max bitrate
    * for uplink or downlink remain. Ref 23.107 Annex C
    */
    nsapis_deactivated   =  sm_retain_cntxt_wth_best_bitrate();
    nsapis_to_deactivate = (nsapis_to_deactivate | nsapis_deactivated);

   /* Final round of elimination. Context with smallest nsapi remain. 
    * Ref 23.107 Annex C
    */
    nsapis_deactivated   =  sm_retain_cntxt_with_least_nsapi();
    nsapis_to_deactivate = (nsapis_to_deactivate | nsapis_deactivated);
	
    /* If any contexts were deactivated, inform ACI and MM */
    if (nsapis_to_deactivate != 0)
    {
      T_CAUSE_ps_cause cause;
      cause.ctrl_value      = CAUSE_is_from_sm;
      cause.value.sm_cause  = (U16)CAUSE_SM_PRE_R99_NETWORK_ENTERED;

      send_smreg_pdp_deactivate_ind(nsapis_to_deactivate, &cause);
      send_mmpm_pdp_context_status_req();
    }

    /* Now all the remaining contexts should be primary. Clear the 
     * secondary context flag if it is set in any of the remaining
     * active context(s). 
     */
    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      struct T_SM_CONTEXT_DATA *context;

      context = sm_get_context_data_from_nsapi(nsapi);
      if ( (context != NULL) && (context->context_control_state
                    != SM_CONTEXT_DEACTIVATED)  && sm_is_secondary(context))
      { /* Clear the secondary context flag */
        context->flags &= 0xff^(U8)SM_CONTEXT_FLAG_SECONDARY_CONTEXT;
      }
    }
  }

  /* Check whether any (primary) contexts are pending reactivation:
   * If so, start reactivation now. 
   */
  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && !sm_is_secondary(context)
        && sm_is_context_pending_reactivation(context))
    {
      /* Clear the pending reactivation flag. Will be done in 
         sm_is_address_changed_with_reactivation */
      //sm_set_context_pending_reactivation(context, FALSE);

      /* Order context reactivation */
      sm_context_control(context, SM_I_CONTEXT_REACTIVATE, NULL);
    }
  }

  /* Set current RAT and core network release */
  sm_set_current_rat       ((T_PS_rat)prim->rat);
  sm_set_current_nw_release(sgsn_rel);

  /*
   * The following line is useful if we try a PDP activation after a 
   * GPRS detach
   */
  sm_data.sm_suspended = FALSE;
  sm_data.sm_suspend_cause = CAUSE_MM_SUCCESS;
}
#endif /*#ifndef TI_PS_OP_SM_RETAIN_BEST_CONTEXT*/

/*
+------------------------------------------------------------------------------
| Function    : handle_mmpm_detach_ind
+------------------------------------------------------------------------------
| Description : Handle incoming MMPM_DETACH_IND primitive
|
| Parameters  : data              - T_MMPM_DETACH_IND *
+------------------------------------------------------------------------------
*/
static void handle_mmpm_detach_ind(void *data)
{
  T_MMPM_DETACH_IND *prim = data;
  int   nsapi;
  U16   nsapis_to_deactivate = 0;
  BOOL  cause_reattach;

  (void)TRACE_FUNCTION("handle_mmpm_detach_ind");

  TRACE_ASSERT (data != NULL);

  if (prim->ps_cause.ctrl_value     == CAUSE_is_from_mm &&
      prim->ps_cause.value.mm_cause == (U16)CAUSE_MM_DETACH_WITH_REATTACH)
  {
    cause_reattach = TRUE;
  } else {
    cause_reattach = FALSE;
  }

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL)
    {
      if (cause_reattach)
      {
        sm_set_context_pending_reactivation(context, TRUE);
      } else {
        sm_set_context_pending_reactivation(context, FALSE);
        nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(nsapi, nsapis_to_deactivate);
      }

      sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
    }
  }

  /* Inform ACI that the contexts are gone. */
  if (nsapis_to_deactivate != 0)
  {
    send_smreg_pdp_deactivate_ind(nsapis_to_deactivate, &prim->ps_cause);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_smreg_pdp_deactivate_req
+------------------------------------------------------------------------------
| Description : Handle incoming SMREG_PDP_DEACTIVATE_REQ
|
| Parameters  : data              - T_SMREG_PDP_DEACTIVATE_REQ *
+------------------------------------------------------------------------------
*/
static void handle_smreg_pdp_deactivate_req(void *data)
{
  int                         nsapi;
  U16                         nsapis_deactivated = 0;
  T_SMREG_PDP_DEACTIVATE_REQ *prim = (T_SMREG_PDP_DEACTIVATE_REQ *)data;
  (void)TRACE_FUNCTION("handle_smreg_pdp_deactivate_req");

  TRACE_ASSERT(prim != NULL);

  if (prim EQ NULL) 
    return;

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    if (sm_is_nsapi_in_nsapi_set(nsapi, prim->nsapi_set))
    {
      struct T_SM_CONTEXT_DATA *context;

      context = sm_get_context_data_from_nsapi(nsapi);
      if (context != NULL)
      {
	nsapis_deactivated = sm_add_nsapi_to_nsapi_set(nsapi, nsapis_deactivated);

        /* Set SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND flag according to suspend status */
        sm_set_started_during_suspend(context);

        /* Clear pending reactivation flag */
        sm_set_context_pending_reactivation(context, FALSE);

        /* Mark requested nsapi for deactivation */
        sm_seq_set_nsapis_to_deactivate(sm_nsapi2nsapi_set(nsapi));

	/* Mark NSAPI "deactivation in progress" */
	sm_seq_nsapi_started_deactivation(nsapi);

	if (prim->rel_ind == (U8)PS_REL_IND_YES)
        {
	  /* Local deactivation */
	  sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
	} else {
	  /* Deactivation with network signalling */
	  sm_context_control(context, SM_I_CONTEXT_DEACTIVATE,
                             (void *)SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE);
	} /* if */
      } /* if */
    } /* if */
  } /* for */

  if (nsapis_deactivated != 0) {
    if (prim->rel_ind == (U8)PS_REL_IND_YES) {
      /* For local deactivations, inform MM if any contexts were deactivated */
      send_mmpm_pdp_context_status_req();
      /* Inform ACI of the deactivated contexts */
      send_smreg_pdp_deactivate_cnf(prim->nsapi_set);
    }
  } else {
    /* Contexts must already be deactivated - inform ACI */
    send_smreg_pdp_deactivate_cnf(prim->nsapi_set);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_sm_status_req
+------------------------------------------------------------------------------
| Description : Handle incoming SM_STATUS_REQ
|
| Parameters  : data              - T_SM_STATUS_REQ *
+------------------------------------------------------------------------------
*/
static void handle_sm_status_req(void *data)
{
  int                         nsapi;
  T_SM_STATUS_REQ            *prim = (T_SM_STATUS_REQ *)data;
  T_SM_CONTEXT_CONTROL_EVENT  event;
  void                       *edata;
  (void)TRACE_FUNCTION("handle_sm_status_req");

  if (prim->ps_cause.ctrl_value == CAUSE_is_from_llc) {
    switch (prim->ps_cause.value.llc_cause) {
    case CAUSE_LLC_NO_PEER_RES:
      /* Do nothing */
      return;
    case CAUSE_LLC_NORMAL_REL:      /* Normal deactivation */
    default:      /* Unknown user plane error:  Deactivate contexts */
      /* Deactivate with network signalling */
      event = SM_I_CONTEXT_USER_PLANE_ERROR;
      edata = NULL;
      break;
    } /* switch */
  } else if (prim->ps_cause.ctrl_value == CAUSE_is_from_upm) {
    switch (prim->ps_cause.value.upm_cause) {
    case CAUSE_UPM_RT_QOS_RELEASE:
      /* Inform ACI of QoS downgrade */
      send_smreg_pdp_modify_ind_multiple(prim->nsapi_set,
                                         SM_UPDATE_QOS_DOWNGRADE);
      return;
    case CAUSE_UPM_REEST_NEEDED:
      event = SM_I_CONTEXT_UPGRADE;
      edata = NULL;
      break;
    case CAUSE_UPM_REEST_REJECT:
      /* FIXME:  How to handle reactivation reject? */
      return;
    case CAUSE_UPM_NORMAL_RELEASE:
      event = SM_I_CONTEXT_USER_PLANE_ERROR;
      edata = NULL;
      break;
    default: /* Unknown user plane error:  Deactivate contexts */
      event = SM_I_CONTEXT_DEACTIVATE;
      edata = (void *)SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE;
      break;
    } /* switch */
  } else {
    /* Unknown user plane error:  Assume user plane down and deactivate contexts */
    event = SM_I_CONTEXT_USER_PLANE_ERROR;
    edata = NULL;
  }

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    if (sm_is_nsapi_in_nsapi_set(nsapi, prim->nsapi_set))
    {
      struct T_SM_CONTEXT_DATA *context;

      context = sm_get_context_data_from_nsapi(nsapi);
      if (context != NULL) {
	sm_set_aci_cause(context, prim->ps_cause.ctrl_value,
			 prim->ps_cause.value.nwsm_cause);
	sm_context_control(context, event, edata);
      } /* if */
    } /* if */
  } /* for */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_activation_override
+------------------------------------------------------------------------------
| Description : Handle SM_I_CONTEXT_ACTIVATION_OVERRIDE
|
| Parameters  : data              - ti
+------------------------------------------------------------------------------
*/
static void handle_context_activation_override(void *data)
{
  int                 nsapi, ti            = (int)data;
  U16                 nsapis_to_deactivate = 0;
  (void)TRACE_FUNCTION("handle_context_activation_override");

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && context->ti != (U8)ti &&
	sm_is_secondary(context) && context->linked_ti == (U8)ti)
    {
      /* Locally deactivate contexts linked to ti of input context */
      nsapis_to_deactivate = sm_add_nsapi_to_nsapi_set(nsapi, nsapis_to_deactivate);
    } /* if */
  } /* for */

  if (nsapis_to_deactivate != 0)
  {
    T_CAUSE_ps_cause cause;

    /* Found contexts to deactivate: inform ACI */
    cause.ctrl_value     = CAUSE_is_from_sm;
    cause.value.sm_cause = (U16)CAUSE_SM_ACTIVATION_OVERRIDE;

    send_smreg_pdp_deactivate_ind(nsapis_to_deactivate, &cause);

    /* Perform local deactivation */
    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      if (sm_is_nsapi_in_nsapi_set(nsapi, nsapis_to_deactivate))
      {
	struct T_SM_CONTEXT_DATA *context;

	context = sm_get_context_data_from_nsapi(nsapi);
	sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
      } /* if */
    } /* for */
  } /* if */
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_deactivate_completed
+------------------------------------------------------------------------------
| Description : Handle SM_I_CONTEXT_DEACTIVATE_COMPLETED
|
| Parameters  : data              - NSAPI
+------------------------------------------------------------------------------
*/
static void handle_context_deactivate_completed(void *data)
{
  int                 nsapi = (int)data;
  (void)TRACE_FUNCTION("handle_context_deactivate_completed");

  /* Mark nsapi as deactivated */
  sm_seq_nsapi_completed_deactivation(nsapi);

  /* Check that this NSAPI is part of the requested NSAPIs being deactivated.
   * Could also be due to SMREG_CONFIGURE_REQ etc. */
  if (sm_seq_is_nsapi_requested_deactivated(nsapi)) {
   /* Yes, NSAPI is in set. Check whether deactivation completes with this NSAPI */
    if (sm_seq_is_nsapi_set_completed()) {
      /* Done.  Confirm deactivation towards ACI */
      send_smreg_pdp_deactivate_cnf(sm_seq_get_nsapis_to_deactivate());

      /* Reset requested deactivation nsapi_set. */
      sm_seq_clear_nsapis_to_deactivate();
    }
  } else {
    /* No. NSAPI was deactivated for reasons other than deactivation from ACI */
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_reactivate_completed
+------------------------------------------------------------------------------
| Description : Handle SM_I_CONTEXT_REACTIVATE_COMPLETED
|
| Parameters  : data              - TI (of primary context)
+------------------------------------------------------------------------------
*/
static void handle_context_reactivate_completed(void *data)
{
  int   ti = (int)data;
  int   nsapi;

  (void)TRACE_FUNCTION("handle_context_reactivate_completed");

  /* Check whether any secondary contexts are pending reactivation with
   * linked_ti == ti of the primary context just reactivated.
   * If so, start reactivation now. */
  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && sm_is_secondary(context)
        && context->linked_ti == (U8)ti)
    {
      /* Clear the pending reactivation flag */
      sm_set_context_pending_reactivation(context, FALSE);

      /* Order context reactivation */
      sm_context_control(context, SM_I_CONTEXT_REACTIVATE, NULL);
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : handle_context_tear_down_deactivate
+------------------------------------------------------------------------------
| Description : Handle SM_I_CONTEXT_TEAR_DOWN_DEACTIVATE
|
| Parameters  : data              - TI
+------------------------------------------------------------------------------
*/
static void handle_context_tear_down_deactivate(void *data)
{
  int                 nsapi, ti            = (int)data;
  U16                 nsapis_to_deactivate = 0;
  (void)TRACE_FUNCTION("handle_context_tear_down_deactivate");

  nsapis_to_deactivate = sm_linked_nsapis((U8)ti);

  if (nsapis_to_deactivate != 0) {
    /* Perform local deactivation */
    for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
    {
      if (sm_is_nsapi_in_nsapi_set(nsapi, nsapis_to_deactivate))
      {
	struct T_SM_CONTEXT_DATA *context;

	context = sm_get_context_data_from_nsapi(nsapi);
	sm_context_control(context, SM_I_CONTEXT_LOCAL_DEACTIVATE, (void *)TRUE);
      } /* if */
    } /* for */
  } /* if */
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_sequencer_init
+------------------------------------------------------------------------------
| Description : Sequencer state machine initialization function
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void sm_sequencer_init(void)
{
  (void)TRACE_FUNCTION("sm_sequencer_init");


  sm_data.sm_attached = FALSE;
  sm_data.sm_suspended = FALSE;
  sm_data.sm_suspend_cause = CAUSE_MM_SUCCESS;


  sm_seq_clear_nsapis_to_deactivate();
}

/*
+------------------------------------------------------------------------------
| Function    : sm_sequencer_exit
+------------------------------------------------------------------------------
| Description : Sequencer state machine exit function

| Parameters  : None
+------------------------------------------------------------------------------
*/
void sm_sequencer_exit(void)
{
  (void)TRACE_FUNCTION("sm_sequencer_exit");

  sm_data.sm_attached = FALSE;
  sm_seq_clear_nsapis_to_deactivate();
}

/*
+------------------------------------------------------------------------------
| Function    : sm_sequencer
+------------------------------------------------------------------------------
| Description : Sequencer state machine
|
| Parameters  : event            - Internal event (see sm_sequencer.h)
|               data             - Event dependent parameter
+------------------------------------------------------------------------------
*/
void sm_sequencer(T_SM_SEQUENCER_EVENT event,
		  void *data)
{
#ifdef DEBUG
  /*@observer@*/
  static const char *event_name[SM_SEQUENCER_NUMBER_OF_EVENTS] = {
    "SM_P_MMPM_ATTACH_IND",
    "SM_P_MMPM_DETACH_IND",
    "SM_P_SMREG_PDP_DEACTIVATE_REQ",
    "SM_P_SM_STATUS_REQ",
    "SM_I_CONTEXT_ACTIVATION_OVERRIDE",
    "SM_I_CONTEXT_DEACTIVATE_COMPLETED",
    "SM_I_CONTEXT_TEAR_DOWN_DEACTIVATE",
    "SM_I_CONTEXT_REACTIVATE_COMPLETED"
  };
  char  state_array[SM_MAX_NSAPI_OFFSET + 1];

  TRACE_ASSERT(event < SM_SEQUENCER_NUMBER_OF_EVENTS);

  if (transition[(U16)event].event != event) {
    (void)TRACE_ERROR("Event table error in sm_sequencer!");
  }
#endif /* DEBUG */

  transition[(U16)event].func(data);

#ifdef DEBUG
  (void)TRACE_EVENT_P2("SM SEQUENCER : %s => %s",
		       event_name[(U16)event],
		       sm_context_bitfield(state_array, sm_data.sm_context_activation_status));
#endif /* DEBUG */
}

/*==== END OF FILE ==========================================================*/
