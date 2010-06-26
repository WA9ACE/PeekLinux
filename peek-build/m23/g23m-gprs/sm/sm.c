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
| Purpose:    Utility functions implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include "sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_nsapi2nsapi_set
+------------------------------------------------------------------------------
| Description : Returns nsapi_set corresponding to the input NSAPI
|
| Parameters  : nsapi               - NSAPI to convert to nsapi_set
+------------------------------------------------------------------------------
*/
U16 sm_nsapi2nsapi_set(int /*@alt U8@*/ nsapi)
{
  return (U16)(1UL << (U8)nsapi);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_add_nsapi_to_nsapi_set
+------------------------------------------------------------------------------
| Description : Returns nsapi_set with nsapi added to it (1 << nsapi).
|
| Parameters  : nsapi               - NSAPI to add
|               nsapi_set           - nsapi_set to which to add NSAPI
+------------------------------------------------------------------------------
*/
U16 sm_add_nsapi_to_nsapi_set(int /*@alt U8@*/ nsapi, U16 nsapi_set)
{
  return (nsapi_set | sm_nsapi2nsapi_set(nsapi));
}

/*
+------------------------------------------------------------------------------
| Function    : sm_remove_nsapi_from_nsapi_set
+------------------------------------------------------------------------------
| Description : Returns nsapi_set with nsapi removed from it (1 << nsapi).
|
| Parameters  : nsapi               - NSAPI to remove
|               nsapi_set           - nsapi_set in which to remove NSAPI
+------------------------------------------------------------------------------
*/
U16 sm_remove_nsapi_from_nsapi_set(int /*@alt U8@*/ nsapi, U16 nsapi_set)
{
  return (nsapi_set & (0xffff ^ sm_nsapi2nsapi_set(nsapi)));
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_nsapi_in_nsapi_set
+------------------------------------------------------------------------------
| Description : Returns TRUE iff the input NSAPI (converted to nsapi_set) is
|               set in the input nsapi_set.
|
| Parameters  : nsapi               - NSAPI whose presence to check
|               nsapi_set           - nsapi_set to compare
+------------------------------------------------------------------------------
*/
BOOL sm_is_nsapi_in_nsapi_set(int /*@alt U8,U16@*/ nsapi, U16 nsapi_set)
{
  return ((nsapi_set & sm_nsapi2nsapi_set(nsapi)) != 0);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nsapi_to_index
+------------------------------------------------------------------------------
| Description : Returns array index corresponding to NSAPI (nsapi - 5)
|
| Parameters  : nsapi               - NSAPI value
+------------------------------------------------------------------------------
*/
U16 /*@alt int@*/ sm_nsapi_to_index(U16 /*@alt U8,int@*/nsapi)
{
  TRACE_ASSERT((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15);
  return (nsapi - (U16)NAS_NSAPI_5);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_index_to_nsapi
+------------------------------------------------------------------------------
| Description : Returns nsapi value corresponding to array index (index + 5)
|
| Parameters  : index               - index value
+------------------------------------------------------------------------------
*/
U16 /*@alt int@*/ sm_index_to_nsapi(U16 /*@alt U8,int@*/index)
{
  TRACE_ASSERT(index < SM_MAX_NSAPI_OFFSET);
  return (index + (U16)NAS_NSAPI_5);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_pdp_context_status
+------------------------------------------------------------------------------
| Description : Get the network status of all NSAPIs.  A bit is set in the
|               nsapi_set, iff the context corresponding to the NSAPI is
|               not in network state INACTIVE.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
U16 sm_get_pdp_context_status(void)
{
  return sm_data.sm_context_activation_status;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_current_rat
+------------------------------------------------------------------------------
| Description : Returns the RAT in which SM is currently active
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
T_PS_rat /*@alt U8@*/sm_get_current_rat(void)
{
  return sm_data.sm_current_rat;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_current_rat
+------------------------------------------------------------------------------
| Description : Sets the RAT in which SM is currently active
|
| Parameters  : rat                 - Active RAT (GSM / UMTS)
+------------------------------------------------------------------------------
*/
void sm_set_current_rat(T_PS_rat rat)
{
  if (rat == PS_RAT_GSM || rat == PS_RAT_UMTS_FDD) {
    sm_data.sm_current_rat = rat;
  } else {
    (void)TRACE_ERROR("Tried to set unknown RAT! (This is OK for EDGE)");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_current_nw_release
+------------------------------------------------------------------------------
| Description : Returns the core network release of the network in which SM
|               is currently active
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
T_PS_sgsn_rel /*@alt U8@*/ sm_get_current_nw_release(void)
{
  return sm_data.sm_current_nw_release;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_suspended
+------------------------------------------------------------------------------
| Description : Returns TRUE if SM is suspended, i.e. has received an
|               MMPM_SUSPEND_IND primitive from MM.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
BOOL sm_is_suspended(void)
{
  return sm_data.sm_suspended;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_attached
+------------------------------------------------------------------------------
| Description : Returns TRUE if SM is attached, i.e. has received an
|               MMPM_ATTACH_IND primitive from MM.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
BOOL sm_is_attached(void)
{
  return sm_data.sm_attached;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_secondary
+------------------------------------------------------------------------------
| Description : Returns TRUE if secondary flag is set for the context.
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_is_secondary(struct T_SM_CONTEXT_DATA *context)
{
  return (context->flags & (U8)SM_CONTEXT_FLAG_SECONDARY_CONTEXT) != (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_secondary
+------------------------------------------------------------------------------
| Description : Set secondary flag for the input context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_set_secondary(struct T_SM_CONTEXT_DATA *context)
{
  context->flags |= (U8)SM_CONTEXT_FLAG_SECONDARY_CONTEXT;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_started_during_suspend
+------------------------------------------------------------------------------
| Description : Set started during suspend flag for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_set_started_during_suspend(struct T_SM_CONTEXT_DATA *context)
{
  if (sm_is_suspended()) {
    context->flags |= (U8)SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND;
  } else {
    context->flags &= 0xff ^ (U8)SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_clear_started_during_suspend
+------------------------------------------------------------------------------
| Description : Clear started during suspend flag for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_clear_started_during_suspend(struct T_SM_CONTEXT_DATA *context)
{
  context->flags &= 0xff ^ (U8)SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_started_during_suspend
+------------------------------------------------------------------------------
| Description : Return TRUE if started during suspend flag is set for the
|               context.
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_is_started_during_suspend(struct T_SM_CONTEXT_DATA *context)
{
  return (context->flags & (U8)SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND) != (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_mark_context_for_deallocation
+------------------------------------------------------------------------------
| Description : Set pending deallocation flag for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_mark_context_for_deallocation(struct T_SM_CONTEXT_DATA *context)
{
  context->flags |= (U8)SM_CONTEXT_FLAG_PENDING_DEALLOCATION;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_context_pending_deallocation
+------------------------------------------------------------------------------
| Description : Returns TRUE if pending deallocation flag is set for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_is_context_pending_deallocation(struct T_SM_CONTEXT_DATA *context)
{
  return (context->flags & (U8)SM_CONTEXT_FLAG_PENDING_DEALLOCATION) != (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_context_pending_reactivation
+------------------------------------------------------------------------------
| Description : Set or clear pending reactivation flag for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_set_context_pending_reactivation(struct T_SM_CONTEXT_DATA *context, BOOL flag)
{
  if (flag)
  {
    context->flags |= (U8)(U8)SM_CONTEXT_FLAG_PENDING_REACTIVATION;
  } else {
    context->flags &= 0xff ^ (U8)(U8)SM_CONTEXT_FLAG_PENDING_REACTIVATION;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_context_pending_reactivation
+------------------------------------------------------------------------------
| Description : Returns TRUE if pending reactivation flag is set for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_is_context_pending_reactivation(struct T_SM_CONTEXT_DATA *context)
{
  return (context->flags & (U8)SM_CONTEXT_FLAG_PENDING_REACTIVATION) != (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_aci_update_required
+------------------------------------------------------------------------------
| Description : Returns TRUE if one of the flags affecting the SMREG interface
|               have changed/are set.
|
| Parameters  : update_flags        - Update flags
+------------------------------------------------------------------------------
*/
BOOL sm_is_aci_update_required(T_SM_UPDATE_FLAGS update_flags)
{
  if ((int)(update_flags & SM_UPDATE_QOS)           != 0 ||
      (int)(update_flags & SM_UPDATE_QOS_DOWNGRADE) != 0 ||
      (int)(update_flags & SM_UPDATE_ADDRESS)       != 0 ||
      (int)(update_flags & SM_UPDATE_COMP_PARAMS)   != 0) {
    return TRUE;
  }
  return FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_user_plane_update_required
+------------------------------------------------------------------------------
| Description : Returns TRUE if one of the flags affecting the SM interface
|               have changed/are set.
|
| Parameters  : update_flags        - Update flags
+------------------------------------------------------------------------------
*/
BOOL sm_is_user_plane_update_required(T_SM_UPDATE_FLAGS update_flags)
{
  if ((int)(update_flags & SM_UPDATE_QOS)                 != 0 ||
      (int)(update_flags & SM_UPDATE_COMP_PARAMS)         != 0 ||
      (int)(update_flags & SM_UPDATE_SAPI_RADIO_PRIO_PFI) != 0) {
    return TRUE;
  }
  return FALSE;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_pfi_included
+------------------------------------------------------------------------------
| Description : Set PFI included flag for the context
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_set_pfi_included(struct T_SM_CONTEXT_DATA *context, BOOL flag)
{
  if (flag)
  {
    context->flags |= (U8)SM_CONTEXT_FLAG_PFI_PRESENT;
  } else {
    context->flags &= 0xff ^ (U8)SM_CONTEXT_FLAG_PFI_PRESENT;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_pfi_included
+------------------------------------------------------------------------------
| Description : Return TRUE if PFI included flag is set for the context.
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
BOOL sm_is_pfi_included(struct T_SM_CONTEXT_DATA *context)
{
  return (context->flags & (U8)SM_CONTEXT_FLAG_PFI_PRESENT) != (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_is_llc_sapi_valid
+------------------------------------------------------------------------------
| Description : Return TRUE if the LLC SAPI provided is valid, FALSE otherwise.
|
| Parameters  : llc_sapi            - LLC SAPI
|               ti                  - TI for context
+------------------------------------------------------------------------------
*/
BOOL sm_is_llc_sapi_valid(U8 llc_sapi, U8 ti)
{
  /* This check is only valid in non-UMTS-single mode configurations */
#if !defined(TI_UMTS) || defined(TI_DUAL_MODE)
  T_CAUSE_ps_cause          cause;

  if ( (llc_sapi == PS_SAPI_3) ||
       (llc_sapi == PS_SAPI_5) ||
       (llc_sapi == PS_SAPI_9) ||
       (llc_sapi == PS_SAPI_11) )
  {
    /* LLC SAPI is OK.  Return TRUE. */
    return TRUE;
  } else {
    /* LLC SAPI has a reserved value. Send a status message. */
    cause.ctrl_value       = CAUSE_is_from_nwsm;
    cause.value.nwsm_cause = (U16)CAUSE_NWSM_INVALID_MANDATORY_ELEMENT;
    send_msg_sm_status(ti, &cause);
    return FALSE;
  } 
#else
  /* In UMTS single mode, this check is disabled */
  return TRUE;
#endif
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_current_nw_release
+------------------------------------------------------------------------------
| Description : Sets the core network release of the network in which SM
|               is currently active
|
| Parameters  : sgsn_rel            - Network release (pre-R99 / R99)
+------------------------------------------------------------------------------
*/
void sm_set_current_nw_release(T_PS_sgsn_rel sgsn_rel)
{
  if (sgsn_rel == PS_SGSN_98_OLDER || sgsn_rel == PS_SGSN_99_ONWARDS) {
    sm_data.sm_current_nw_release = sgsn_rel;
  } 
  else { /*The Berlin way*/
    /*If SGSN release is unknown set the value to R98*/
    (void)TRACE_ERROR("Tried to set unknown core network release. \
                         Setting R97 as default !");
    sm_data.sm_current_nw_release = PS_SGSN_98_OLDER;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_aci_cause
+------------------------------------------------------------------------------
| Description : Sets the cause value for a context. Used during retransmissions
|               etc.
|
| Parameters  : context             - context data
|               ctrl_cause          - cause originator
|               cause               - vause value
+------------------------------------------------------------------------------
*/
void sm_set_aci_cause(struct T_SM_CONTEXT_DATA *context,
		  T_CAUSE_ctrl_value ctrl_cause, U16 cause)
{
  (void)TRACE_EVENT_P2("sm_set_aci_cause(ctrl=%d, cause=%04x)", (int)ctrl_cause, cause);
  TRACE_ASSERT(context != NULL);
  /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  context->aci_cause.ctrl_value       = ctrl_cause;
  context->aci_cause.value.nwsm_cause = cause;
  /*lint +e613 (Possible use of null pointer 'context' in left argument to operator '->') */
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_aci_cause
+------------------------------------------------------------------------------
| Description : Gets the cause value for a context. Used during retransmissions
|               etc.
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
/*@observer@*/
T_CAUSE_ps_cause *sm_get_aci_cause(struct T_SM_CONTEXT_DATA *context)
{
  TRACE_ASSERT(context != NULL);
  /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  return &context->aci_cause;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_set_nw_cause
+------------------------------------------------------------------------------
| Description : Sets the cause value for a context. Used during retransmissions
|               etc.
|
| Parameters  : context             - context data
|               ctrl_cause          - cause originator
|               cause               - vause value
+------------------------------------------------------------------------------
*/
void sm_set_nw_cause(struct T_SM_CONTEXT_DATA *context,
		  T_CAUSE_ctrl_value ctrl_cause, U16 cause)
{
  TRACE_ASSERT(context != NULL);
  /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  context->nw_cause.ctrl_value       = ctrl_cause;
  context->nw_cause.value.nwsm_cause = cause;
  /*lint +e613 (Possible use of null pointer 'context' in left argument to operator '->') */
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_nw_cause
+------------------------------------------------------------------------------
| Description : Gets the cause value for a context. Used during retransmissions
|               etc.
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
/*@observer@*/
T_CAUSE_ps_cause *sm_get_nw_cause(struct T_SM_CONTEXT_DATA *context)
{ 
TRACE_ASSERT(context != NULL);
/*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
#ifdef DEBUG
  if (context->nw_cause.ctrl_value != CAUSE_is_from_nwsm &&
      context->nw_cause.ctrl_value != CAUSE_is_from_sm) {
    (void)TRACE_EVENT_P2("sm_get_nw_cause(): ERROR! Cause element not "
			 "initialized or not from SM (ctrl=%d, cause=%d)",
		   context->nw_cause.ctrl_value, context->nw_cause.value.nwsm_cause);
  }
#endif
  return &context->nw_cause;
/*lint +e613 (Possible use of null pointer 'context' in left argument to operator '->') */
}

/*==== END OF FILE ==========================================================*/
