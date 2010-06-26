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
| Purpose:    This module implements the process dispatcher for GSM primitives
|             in the User Plane Manager (UPM) entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/


/*==== INCLUDES =============================================================*/

#include "upm.h"

#ifndef UPM_WITHOUT_USER_PLANE
#include "upm_dti_output_handler.h"
#endif

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_mmpm_sequence_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_SEQUENCE_IND
|
| Parameters  : prim                         - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_mmpm_sequence_ind (T_MMPM_SEQUENCE_IND *prim)
{
  (void)TRACE_FUNCTION("upm_disp_mmpm_sequence_ind");

  upm_sndcp_dispatch_mmpm_sequence_ind(prim);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sn_activate_cnf
+------------------------------------------------------------------------------
| Description : Dispatch SN_ACTIVATE_CNF
|
| Parameters  : prim                       - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sn_activate_cnf (T_SN_ACTIVATE_CNF *prim)
{
  struct T_CONTEXT_DATA *context;

  (void)TRACE_FUNCTION("upm_disp_sn_activate_cnf");

  /* Find context data using NSAPI as index */
  context = upm_get_context_data_from_nsapi(prim->nsapi);

  if (context != NULL) {
    /* Forward primitive to SNDCP Control */
    upm_sndcp_control(context, UPM_P_SN_ACTIVATE_CNF, prim);
  } else {
    (void)TRACE_ERROR("Received SN_ACTIVATE_CNF for non-existing context "
                      "- discarded!");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sn_count_cnf
+------------------------------------------------------------------------------
| Description : Dispatch SN_COUNT_CNF
|
| Parameters  : prim                       - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sn_count_cnf (T_SN_COUNT_CNF *prim)
{
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_sn_count_cnf");

  ptr_context_data = upm_get_context_data_from_nsapi(prim->nsapi);
  if (ptr_context_data != NULL) {
    /* Forward primitive to SNDCP Control */
    upm_sndcp_dispatch_sn_count_cnf(prim);
  } else {
    (void)TRACE_ERROR("Received SN_COUNT_CNF for non-existing context "
                      "- discarded!");
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sn_deactivate_cnf
+------------------------------------------------------------------------------
| Description : Dispatch SN_DEACTIVATE_CNF
|
| Parameters  : prim                       - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sn_deactivate_cnf (T_SN_DEACTIVATE_CNF *prim)
{
  struct T_CONTEXT_DATA *ptr_context_data;

  (void)TRACE_FUNCTION("upm_disp_sn_deactivate_cnf");

  ptr_context_data = upm_get_context_data_from_nsapi(prim->nsapi);
  if (ptr_context_data != NULL) {
    /* Forward primitive to SNDCP Control */
    upm_sndcp_control(ptr_context_data, UPM_P_SN_DEACTIVATE_CNF, NULL);

    upm_check_for_released_context_and_release(ptr_context_data);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_disp_sn_modify_cnf
+------------------------------------------------------------------------------
| Description : Dispatch SN_MODIFY_CNF
|
| Parameters  : prim                      - received primitive
+------------------------------------------------------------------------------
*/
void upm_disp_sn_modify_cnf (T_SN_MODIFY_CNF *prim)
{
  struct T_CONTEXT_DATA *context;

  (void)TRACE_FUNCTION("upm_disp_sn_modify_cnf");

  /* Find context data using NSAPI as index */
  context = upm_get_context_data_from_nsapi(prim->nsapi);

  if (context != NULL) {
    /* Forward primitive to SNDCP Control */
    upm_sndcp_control(context, UPM_P_SN_MODIFY_CNF, prim);
  } else {
    (void)TRACE_ERROR("Received SN_MODIFY_CNF for non-existing context "
                      "- discarded!");
  }
}

/*==== END OF FILE ==========================================================*/
