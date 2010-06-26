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
| Purpose:    Output functions for primitives from SM to the ACI entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "sm.h"

#include "sm_aci_output_handler.h"
#include "sm_qos.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_activate_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_ACTIVATE_CNF primitive.
|
| Parameters  : context   - Pointer to context structure
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_activate_cnf(struct T_SM_CONTEXT_DATA *context)
{
  U16  pco_len;
  (void)TRACE_FUNCTION("send_smreg_pdp_activate_cnf");

  pco_len = (context->negotiated_pco != NULL
	     ? (U16)context->negotiated_pco->c_pco_value
	     : (U16)0);

  {
    U16 pco_bit_len = pco_len << 3;
    PALLOC_SDU(prim, SMREG_PDP_ACTIVATE_CNF, pco_bit_len);

    if (prim != NULL) {
      prim->nsapi               = context->nsapi;
      prim->pdp_type            = context->pdp_type;
      sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);
      memcpy(&prim->comp_params, &context->comp_params, sizeof(T_NAS_comp_params));
      prim->ctrl_ip_address     = context->negotiated_address.ctrl_ip_address;
      memcpy(&prim->ip_address, &context->negotiated_address.ip_address, sizeof(T_NAS_ip_address));
      if (pco_len > 0 && context->negotiated_pco != NULL) {
	U16 offset = prim->sdu.o_buf >> 3;
	/* sdu.o_buf and sdu.l_buf already by PALLOC_SDU macro above! */
	memcpy(&prim->sdu.buf[offset], context->negotiated_pco->pco_value,
               (size_t)pco_len);
      } else {
	prim->sdu.l_buf = 0;
      }

      (void)PSEND(hCommACI, prim);
    } else {
      (void)TRACE_ERROR("Unable to allocate SMREG_PDP_ACTIVATE_CNF primitive!");
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_activate_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_ACTIVATE_IND primitive.
|
| Parameters  : context   - Pointer to context structure
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_activate_ind(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_activate_ind");

  {
    PALLOC(prim, SMREG_PDP_ACTIVATE_IND);

    prim->ti                  = context->ti;
    prim->pdp_type            = context->pdp_type;
    prim->ctrl_ip_address     = context->negotiated_address.ctrl_ip_address;
    memcpy(&prim->ip_address, &context->negotiated_address.ip_address, sizeof(T_NAS_ip_address));
    if (context->apn != NULL) { /* NOTE: *MUST* be present acc to 3G 24.008, but just in case... */
      memcpy(&prim->apn, context->apn, sizeof(T_SMREG_apn));
    } else {
      prim->apn.c_apn_buf = (U8)0;
    }

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_activate_rej
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_ACTIVATE_REJ primitive.
|
| Parameters  : context   - Pointer to context structure
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_activate_rej(U8 nsapi, T_CAUSE_ps_cause *cause)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_activate_rej");

  {
    PALLOC(prim, SMREG_PDP_ACTIVATE_REJ);

    prim->nsapi               = nsapi;
    prim->ps_cause.ctrl_value = cause->ctrl_value;
    prim->ps_cause.value      = cause->value;

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_activate_sec_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_ACTIVATE_SEC_CNF primitive.
|
| Parameters  : context   - Pointer to context structure
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_activate_sec_cnf(struct T_SM_CONTEXT_DATA *context)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_activate_sec_cnf");

  {
    PALLOC(prim, SMREG_PDP_ACTIVATE_SEC_CNF);

    prim->nsapi               = context->nsapi;

    sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_activate_sec_rej
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_ACTIVATE_SEC_REJ primitive.
|
| Parameters  : nsapi          - Context identifier
|               cause          - Rejection cause
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_activate_sec_rej(U8 nsapi, T_CAUSE_ps_cause *cause)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_activate_sec_rej");

  {
    PALLOC(prim, SMREG_PDP_ACTIVATE_SEC_REJ);
    
    prim->nsapi               = nsapi;
    prim->ps_cause.ctrl_value = cause->ctrl_value;
    prim->ps_cause.value      = cause->value;

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_deactivate_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_DEACTIVATE_CNF primitive.
|
| Parameters  : nsapi_set - Set of NSAPIs successfully deactivated
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_deactivate_cnf(U16 nsapi_set)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_deactivate_cnf");

  {
    PALLOC(prim, SMREG_PDP_DEACTIVATE_CNF);

    prim->nsapi_set           = nsapi_set;

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_deactivate_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_DEACTIVATE_IND primitive.
|
| Parameters  : nsapi_set - Set of NSAPIs being deactivated
|               cause     - Reason for deactivation
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_deactivate_ind(U16 nsapi_set, T_CAUSE_ps_cause *cause)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_deactivate_ind");

  {
    PALLOC(prim, SMREG_PDP_DEACTIVATE_IND);

    prim->nsapi_set           = nsapi_set;
    prim->ps_cause.ctrl_value = cause->ctrl_value;
    prim->ps_cause.value      = cause->value;

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_modify_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_MODIFY_CNF primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_modify_cnf(struct T_SM_CONTEXT_DATA *context,
			       T_SM_UPDATE_FLAGS update_flags)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_modify_cnf");

  {
    PALLOC(prim, SMREG_PDP_MODIFY_CNF);
    
    prim->nsapi               = context->nsapi;

    if ((update_flags & SM_UPDATE_QOS) != (T_SM_UPDATE_FLAGS)0) {
      sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);
    } else {
      prim->ctrl_qos          = PS_is_qos_not_present;
    }

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_modify_ind
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_MODIFY_IND primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_modify_ind(struct T_SM_CONTEXT_DATA *context,
			       T_SM_UPDATE_FLAGS update_flags)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_modify_ind");

  {
    PALLOC(prim, SMREG_PDP_MODIFY_IND);
      
    prim->nsapi               = context->nsapi;

    if ((int)(update_flags & SM_UPDATE_QOS) != 0) {
      sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);
    } else {
      prim->ctrl_qos          = PS_is_qos_not_present;
    }

    if ((int)(update_flags & SM_UPDATE_ADDRESS) != 0) {
      prim->ctrl_ip_address   = context->negotiated_address.ctrl_ip_address;
      memcpy(&prim->ip_address, &context->negotiated_address.ip_address, sizeof(T_NAS_ip_address));
    } else {
      prim->ctrl_ip_address   = NAS_is_ip_not_present;
    }

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_modify_ind_multiple
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_MODIFY_IND primitives for
|               several NSAPIs.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_modify_ind_multiple(U16 nsapi_set,
					T_SM_UPDATE_FLAGS update_flags)
{
  int nsapi;

  (void)TRACE_FUNCTION("send_smreg_pdp_modify_ind_multiple");

  for (nsapi = (int)NAS_NSAPI_5;
       nsapi < NAS_SIZE_NSAPI && sm_is_nsapi_in_nsapi_set(nsapi, nsapi_set);
       nsapi++)
  {
    struct T_SM_CONTEXT_DATA *context;

    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL)
    {
      PALLOC(prim, SMREG_PDP_MODIFY_IND);
    
      prim->nsapi               = context->nsapi;

      if ((update_flags & SM_UPDATE_QOS_DOWNGRADE) != (T_SM_UPDATE_FLAGS)0) {
	sm_qos_copy_from_sm(&prim->qos, &context->accepted_qos, &prim->ctrl_qos);
	/* QoS downgraded to 0kbps */
	prim->qos.qos_r99.max_rate_ul = 0;
	prim->qos.qos_r99.max_rate_dl = 0;
      } else {
	prim->ctrl_qos          = PS_is_qos_not_present;
      }

      /* IP address is not changed/set using this function */
      prim->ctrl_ip_address   = NAS_is_ip_not_present;

      (void)PSEND(hCommACI, prim);
    } /* if */
  } /* for */
}

/*
+------------------------------------------------------------------------------
| Function    : send_smreg_pdp_modify_rej
+------------------------------------------------------------------------------
| Description : Allocate, pack and send SMREG_PDP_MODIFY_REJ primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void send_smreg_pdp_modify_rej(U8 nsapi, T_CAUSE_ps_cause *cause)
{
  (void)TRACE_FUNCTION("send_smreg_pdp_modify_rej");

  {
    PALLOC(prim, SMREG_PDP_MODIFY_REJ);

    prim->nsapi               = nsapi;
    prim->ps_cause.ctrl_value = cause->ctrl_value;
    prim->ps_cause.value      = cause->value;

    (void)PSEND(hCommACI, prim);
  }
}

/*==== END OF FILE ==========================================================*/
