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
| Purpose:    Output functions for primitives from UPM to the MM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "upm.h"

#include "upm_sndcp_output_handler.h"
#include "upm_mm_output_handler.h"
#ifndef UPM_WITHOUT_USER_PLANE
#include "upm_dti_output_handler.h"
#endif /* UPM_WITHOUT_USER_PLANE */

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_sn_activate_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_ACTIVATE_REQ primitive.
|
| Parameters  : ptr_context_data - context data
|               active           - active/passive establishment flag to SNDCP
+------------------------------------------------------------------------------
*/
void send_sn_activate_req(struct T_CONTEXT_DATA *ptr_context_data,
                          T_NAS_comp_params *comp_params, BOOL active)
{
  (void)TRACE_FUNCTION("send_sn_activate_req");
  
  {
    struct T_UPM_CONTEXT_DATA_GPRS *gprs_data = &ptr_context_data->gprs_data;
    PALLOC(prim, SN_ACTIVATE_REQ);

/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi             = ptr_context_data->nsapi;
    prim->sapi              = gprs_data->sndcp_llc_sapi;
    prim->radio_prio        = gprs_data->sndcp_radio_prio;
    prim->snsm_qos.delay    = gprs_data->sndcp_delay;
    prim->snsm_qos.relclass = gprs_data->sndcp_relclass;
    prim->snsm_qos.peak     = gprs_data->sndcp_peak_bitrate;
    prim->snsm_qos.preced   = gprs_data->sndcp_preced;
    prim->snsm_qos.mean     = gprs_data->sndcp_mean;

    prim->establish         = (active ? (U8)SN_ACTIVE_ESTABLISHMENT :
                                        (U8)SN_PASSIVE_ESTABLISHMENT);
    if (comp_params != NULL)
    {
      memcpy(&prim->comp_params, comp_params, sizeof(T_NAS_comp_params));
    } else {
      memset(&prim->comp_params, 0, sizeof(T_NAS_comp_params));
    }
    prim->pkt_flow_id       = (U16)gprs_data->pkt_flow_id;
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sn_count_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_COUNT_REQ primitive.
|
| Parameters  : nsapi            - NSAPI
|               reset            - Reset indicator
+------------------------------------------------------------------------------
*/
void send_sn_count_req(U8 nsapi, U8 reset)
{
  (void)TRACE_FUNCTION("send_sn_count_req");

  {
    PALLOC(prim, SN_COUNT_REQ);
/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi = nsapi;
    prim->reset = reset;

/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sn_deactivate_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_DEACTIVATE_REQ primitive.
|
| Parameters  : nsapi            - NSAPI to deactivate
|               rel_ind          - local release indicator for SNDCP
+------------------------------------------------------------------------------
*/
void send_sn_deactivate_req(U8 nsapi, U8 rel_ind)
{
  (void)TRACE_FUNCTION("send_sn_deactivate_req");

  {
    PALLOC(prim, SN_DEACTIVATE_REQ);
/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi     = nsapi;
    prim->rel_ind   = rel_ind;
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sn_modify_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_MODIFY_REQ primitive.
|
| Parameters  : ptr_context_data - context data
+------------------------------------------------------------------------------
*/
void send_sn_modify_req(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("send_sn_modify_req");

  {
    struct T_UPM_CONTEXT_DATA_GPRS *gprs_data = &ptr_context_data->gprs_data;
    PALLOC(prim, SN_MODIFY_REQ);
	
/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi             = ptr_context_data->nsapi;
    prim->sapi              = gprs_data->sndcp_llc_sapi;
    prim->radio_prio        = gprs_data->sndcp_radio_prio;
    prim->snsm_qos.delay    = gprs_data->sndcp_delay;
    prim->snsm_qos.relclass = gprs_data->sndcp_relclass;
    prim->snsm_qos.peak     = gprs_data->sndcp_peak_bitrate;
    prim->snsm_qos.preced   = gprs_data->sndcp_preced;
    prim->snsm_qos.mean     = gprs_data->sndcp_mean;
    prim->pkt_flow_id       = (U16)gprs_data->pkt_flow_id;
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sn_sequence_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_SEQUENCE_REQ primitive.
|
| Parameters  : mmpm_sequence_ind - MMPM primitive containing sequence numbers
+------------------------------------------------------------------------------
*/
void send_sn_sequence_req(T_MMPM_SEQUENCE_IND *mmpm_sequence_ind)
{
  U16  index;
  (void)TRACE_FUNCTION("send_sn_sequence_req");

  {
    PALLOC(prim, SN_SEQUENCE_REQ);
/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->c_receive_no_list = mmpm_sequence_ind->c_npdu_list;

    for (index = 0; index < (U16)mmpm_sequence_ind->c_npdu_list; index++)
    {
      prim->receive_no_list[index].nsapi      = mmpm_sequence_ind->npdu_list[index].nsapi;
      prim->receive_no_list[index].receive_no = mmpm_sequence_ind->npdu_list[index].receive_n_pdu_number_val;
    }
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_sn_get_pending_pdu_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SN_GET_PENDING_PDU_REQ primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
void send_sn_get_pending_pdu_req(void)
{
  (void)TRACE_FUNCTION("send_sn_get_pending_pdu_req");

  {
    PALLOC(prim, SN_GET_PENDING_PDU_REQ);
	
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    (void)PSEND(hCommSNDCP, prim);
  }
}
#endif /* TI_DUAL_MODE */

/*==== END OF FILE ==========================================================*/
