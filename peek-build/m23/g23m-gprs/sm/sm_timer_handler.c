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
| Purpose:    3G timer handler functions implementation in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"

#include "sm_timer_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_timer_start
+------------------------------------------------------------------------------
| Description : Start specified timer for the given NSAPI/context

| Parameters  : context    - Context data
|               timer      - 3G timer (T3380, T3381, or T3390)
+------------------------------------------------------------------------------
*/
void sm_timer_start(struct T_SM_CONTEXT_DATA *context, T_SM_TIMER_TYPE timer)
{
  T_TIME  timeout;
#ifdef DEBUG
  (void)TRACE_EVENT_P2("sm_timer_start: %s on NSAPI %d",
                       sm_timer_name((U8)timer), context->nsapi);
#endif

  switch (timer) {
  case SM_TIMER_T3380:
    timeout = (T_TIME)NWSM_T3380_TIMEOUT;
    break;
  case SM_TIMER_T3381:
    timeout = (T_TIME)NWSM_T3381_TIMEOUT;
    break;
  case SM_TIMER_T3390:
    timeout = (T_TIME)NWSM_T3390_TIMEOUT;
    break;
  case SM_TIMER_NONE:
  default:
    (void)TRACE_ERROR("sm_timer_start: Invalid timer specified - No timers started!");
    return;
  }

  if ((T_SM_TIMER_TYPE)context->active_timer == SM_TIMER_NONE) {
    context->active_timer = (U8)timer;
    context->timeouts     = (U8)NWSM_MAX_TIMEOUTS;
    (void)vsi_t_start(sm_handle, sm_nsapi_to_index(context->nsapi), timeout);
  } else {
    /* W00t?!? Timer already active? Bug? */
    (void)TRACE_ERROR("A timer is already active for this NSAPI -- overriding!");
    (void)vsi_t_stop (sm_handle, sm_nsapi_to_index(context->nsapi));
    (void)vsi_t_start(sm_handle, sm_nsapi_to_index(context->nsapi), timeout);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_timer_restart
+------------------------------------------------------------------------------
| Description : Restart timer after timeout for the given NSAPI/context

| Parameters  : context    - Context data
+------------------------------------------------------------------------------
*/
void sm_timer_restart(struct T_SM_CONTEXT_DATA *context)
{
  T_TIME  timeout;

#ifdef DEBUG
  (void)TRACE_EVENT_P3("sm_timer_restart: %s on NSAPI %d; %d timeouts remaining",
                       sm_timer_name(context->active_timer),
                       context->nsapi, context->timeouts);
#endif

  switch (context->active_timer) {
  case SM_TIMER_T3380:
    timeout = (T_TIME)NWSM_T3380_TIMEOUT;
    break;
  case SM_TIMER_T3381:
    timeout = (T_TIME)NWSM_T3381_TIMEOUT;
    break;
  case SM_TIMER_T3390:
    timeout = (T_TIME)NWSM_T3390_TIMEOUT;
    break;
  case SM_TIMER_NONE:
  default:
    (void)TRACE_ERROR("sm_timer_restart: Attempted to restart void timer - Aborted!");
    return;
  }

  (void)vsi_t_start(sm_handle, sm_nsapi_to_index(context->nsapi), timeout);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_timer_stop
+------------------------------------------------------------------------------
| Description : Stop active timer for the given NSAPI/context

| Parameters  : context    - Context data
+------------------------------------------------------------------------------
*/
void sm_timer_stop(struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG
  (void)TRACE_EVENT_P2("sm_timer_stop: %s on NSAPI %d",
                       sm_timer_name(context->active_timer),
                       context->nsapi);
#endif

  if ((T_SM_TIMER_TYPE)context->active_timer != SM_TIMER_NONE) {
    (void)vsi_t_stop (sm_handle, sm_nsapi_to_index(context->nsapi));
    context->active_timer = (U8)SM_TIMER_NONE;
    context->timeouts     = (U8)0;
  }
  /* If no timers active: No action */
}

/*==== END OF FILE ==========================================================*/
