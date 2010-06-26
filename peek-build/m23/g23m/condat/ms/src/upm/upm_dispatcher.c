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
| Purpose:    This module implements the process dispatcher
|             for the User Plane Manager (UPM) entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "upm.h"
#include "upm_dispatcher.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sm_activate_started_ind
+------------------------------------------------------------------------------
| Description : Dispatch SM_ACTIVATE_STARTED_IND
|
| Parameters  : sm_activate_started_ind   - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sm_activate_started_ind(T_SM_ACTIVATE_STARTED_IND *prim)
{
  int                    nsapi;
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_sm_activate_started_ind");

  nsapi            = (int)prim->nsapi;
  ptr_context_data = upm_get_context_data_from_nsapi(nsapi);

  /* UPM_DTI_REQ arrived first (or activation override).  Use present data structures. */
  if (ptr_context_data != NULL)
  {
#ifdef UPM_WITHOUT_USER_PLANE /*T_UPM_USER_PLANE_DATA is not visible for GPRS world*/
    memset(ptr_context_data, 0, sizeof(struct T_CONTEXT_DATA)
                 /* - sizeof(struct T_UPM_USER_PLANE_DATA)*/);
#else /*#ifndef UPM_WITHOUT_USER_PLANE*/
    memset(ptr_context_data, 0, sizeof(struct T_CONTEXT_DATA) 
                       - sizeof(struct T_UPM_USER_PLANE_DATA));
#endif /*#ifndef UPM_WITHOUT_USER_PLANE*/
    ptr_context_data->nsapi = (U8)nsapi;
  } else {
    MALLOC(ptr_context_data, (U32)sizeof(struct T_CONTEXT_DATA));

    if (ptr_context_data == NULL)
    {
      (void)TRACE_ERROR("Unable to allocate data structure for new context!");
      return;
    }

    memset(ptr_context_data, 0, sizeof(struct T_CONTEXT_DATA));

    upm_assign_context_data_to_nsapi(ptr_context_data, nsapi);
  }

  TRACE_ASSERT(ptr_context_data != NULL);

  upm_link_control_init(ptr_context_data);
#ifdef TI_UMTS
  upm_rab_control_init(ptr_context_data);
#endif /* TI_UMTS */
#ifdef TI_GPRS
  upm_sndcp_control_init(ptr_context_data);
#endif /* TI_GPRS */
#ifdef TI_DUAL_MODE
  upm_rat_control_init( (T_PS_rat)prim->rat );
#endif /* TI_DUAL_MODE */
  upm_link_control(ptr_context_data, UPM_P_SM_ACTIVATE_STARTED_IND, prim);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sm_activate_ind
+------------------------------------------------------------------------------
| Description : Dispatch SM_ACTIVATE_IND
|
| Parameters  : sm_activate_ind            - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sm_activate_ind(T_SM_ACTIVATE_IND *sm_activate_ind)
{
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_sm_activate_ind");

  ptr_context_data = upm_get_context_data_from_nsapi(sm_activate_ind->nsapi);
  if (ptr_context_data != NULL)
  {
    upm_link_control(ptr_context_data, UPM_P_SM_ACTIVATE_IND, sm_activate_ind);
  } else {
    (void)TRACE_ERROR("Tried to complete activation of non-existent context!");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sm_modify_ind
+------------------------------------------------------------------------------
| Description : Dispatch SM_MODIFY_IND
|
| Parameters  : sm_modify_ind              - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sm_modify_ind (T_SM_MODIFY_IND *sm_modify_ind)
{
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_sm_modify_ind");

  ptr_context_data = upm_get_context_data_from_nsapi(sm_modify_ind->nsapi);
  if (ptr_context_data != NULL)
  {
    upm_link_control(ptr_context_data, UPM_P_SM_MODIFY_IND, sm_modify_ind);
  } else {
    (void)TRACE_ERROR("Tried to modify non-existent context!");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_upm_count_req
+------------------------------------------------------------------------------
| Description : Dispatch UPM_COUNT_REQ
|
| Parameters  : prim                       - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_upm_count_req (T_UPM_COUNT_REQ *prim)
{
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_upm_count_req");

  ptr_context_data = upm_get_context_data_from_nsapi(prim->nsapi);

  if (ptr_context_data != NULL) {
#ifdef TI_GPRS
#ifdef TI_DUAL_MODE
    if (upm_get_current_rat() == PS_RAT_GSM)
#endif /* TI_DUAL_MODE */
    {
      upm_sndcp_dispatch_upm_count_req(prim);
    }
#endif /* TI_GPRS */
#ifdef TI_DUAL_MODE
    else
#endif /* TI_DUAL_MODE */
#ifdef TI_UMTS
    {
      upm_dti_dispatch_upm_count_req(ptr_context_data, prim->reset);
    }
#endif
  } else {
    (void)TRACE_EVENT_P1("Warning: Received COUNT_REQ for undefined "
                         "NSAPI/context; ignored...", prim->nsapi);
  }
}

/*==== END OF FILE ==========================================================*/
