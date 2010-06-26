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
| Purpose:    Output functions for primitives from SM to the MM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "sm.h"
#include "message.h"
#include "ccdapi.h"

#include "sm_mm_output_handler.h"
#include "sm_qos.h"
#include "sm_tft.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

#define SM_MM_SEND_MSG(c,e,s,M) sm_mm_send_msg(c,e,s,BSIZE_##M##)

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    :  sm_set_ti_pd
+------------------------------------------------------------------------------
| Description : Prepend TI and PD to an outgoing air interface message
|
| Parameters  : ti, pd
|
+------------------------------------------------------------------------------
*/
static void sm_set_ti_pd(U8 ti, T_MSGBUF *msg)
{
  U8 ti_flag  = ti & SM_TI_FLAG;
  U8 msg_type = msg->buf[msg->o_buf >> 3];

  /* Check for extended ti */
  if ( (ti & SM_TI_MASK) > (U8)SM_MAX_NON_EXT_TI) {  /* TRUE == Extended TI */
    /* Value assigned to variable to shut up Visual C */
    U8 ti_msb = ti_flag | 0x70 | PD_SM;
    U8 ti_lsb = ti & 127 | 0x80; /* TI EXT must be 1 [3G 24.007, 11.2.3.1.3] */

    /* Insert extended ti flag + PD in first octet, and ti value in octet 2 */
    (void)ccd_codeByte(msg->buf, (U16)(msg->o_buf-16), (U16)8, ti_msb);
    (void)ccd_codeByte(msg->buf, (U16)(msg->o_buf-8),  (U16)8, ti_lsb);
    msg->l_buf += 16;
    msg->o_buf -= 16;

    (void)TRACE_EVENT_P8("L3 HDR: 0x%02x [0x%02x] 0x%02x {ti_flag=%d, "
                         "ti=%d, pd=%d, ext_ti=%d, msg_type=0x%02x}",
                         msg->buf[msg->o_buf >> 3],
                         msg->buf[(msg->o_buf >> 3) + 1],
                         msg->buf[(msg->o_buf >> 3) + 2],
                         (ti_flag != (U8)0 ? 1 : 0),
                         ti, PD_SM, ti_lsb, msg_type);
  } else {  /* (ti & SM_TI_MASK) <= SM_MAX_NON_EXT_TI:  Non-extended TI */
    /* Value assigned to variable to shut up Visual C */
    U8 ti_val = ti_flag | ((ti & SM_TI_NON_EXT_MASK) << 4) | PD_SM;

    /* Insert non-extended ti value in msg */
    (void)ccd_codeByte(msg->buf, (USHORT)(msg->o_buf-8), (U16)8, ti_val);
    msg->l_buf += 8;
    msg->o_buf -= 8;

    (void)TRACE_EVENT_P6("L3 HDR: 0x%02x [] 0x%02x {ti_flag=%d, "
                         "ti=%d, pd=%d, msg_type=0x%02x}",
                         msg->buf[msg->o_buf >> 3],
                         msg->buf[(msg->o_buf >> 3) + 1],
                         (ti_flag != (U8)0 ? 1 : 0), ti, PD_SM, msg_type);
 }
}

static U8 sm_mmpm_establish_cause(struct T_SM_CONTEXT_DATA *context)
{
  T_PS_tc traffic_class;

  if (context != NULL) {
    /*
     * Call QoS helper function to extract requested traffic class QoS
     * parameter from the requested QoS parameters for this context/NSAPI.
     */
    traffic_class = (T_PS_tc)sm_qos_get_traffic_class(context);

    switch (traffic_class) {
    case PS_TC_CONV:
      return (U8)MMPM_CAUSE_ESTABL_ORIG_CONVERSATIONAL;
    case PS_TC_STREAM:
      return (U8)MMPM_CAUSE_ESTABL_ORIG_STREAMING;
    case PS_TC_INTER:
      return (U8)MMPM_CAUSE_ESTABL_ORIG_INTERACTIVE;
    case PS_TC_BG:
      return (U8)MMPM_CAUSE_ESTABL_ORIG_BACKGROUND;
    default:
      /* For subscribed QoS, high priority signalling is used by default. */
      return (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING;
    }
  } else {
    return (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING;
  }
}

static U8 sm_cause_ps_to_air(T_CAUSE_ps_cause *cause)
{
  switch (cause->ctrl_value) {
  case CAUSE_is_from_nwsm:
    return (U8)cause->value.nwsm_cause;
  default:
    (void)TRACE_ERROR("Causes not from M_SM not supported yet!");
    return (U8)cause->value.nwsm_cause;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_mmpm_unitdata_req
+------------------------------------------------------------------------------
| Description : Call CCD to code AIM and send MMPM_UNITDATA_REQ primitive.
|
| Parameters  : ti          - Transaction identifier
|               est_cause   - establish cause (for MM)
|               prim        - Formatted air interface message
+------------------------------------------------------------------------------
*/
static void send_mmpm_unitdata_req(U8 ti, U8 est_cause,
                                   /*@null@*/struct T_SM_CONTEXT_DATA *context,
                                   /*@only@*/ /*@null@*/
                                   T_MMPM_UNITDATA_REQ *prim)
{
  (void)TRACE_FUNCTION("send_mmpm_unitdata_req");

  TRACE_ASSERT (prim != NULL);

  if (prim EQ NULL)
    return;

  /* Originating entity is SM */
  prim->org_entity        = (U8)NAS_ORG_ENTITY_SM;

  /* Add establish cause and PDP context status elements */
  prim->v_establish_cause = (U8)TRUE;
  prim->establish_cause   = est_cause;

  prim->v_nsapi_set       = (U8)TRUE;
  prim->nsapi_set         = sm_get_pdp_context_status();

  if (ccd_codeMsg((U8)CCDENT_SM, (U8)UPLINK, (T_MSGBUF *)&prim->sdu,
                  _decodedMsg, (U8)NOT_PRESENT_8BIT) != (U8)ccdOK) {
    /*
     * Error Handling
     */
    (void)TRACE_ERROR( "Error in outgoing message" ) ;
    sm_pfree(prim);
  } else {
    /*
     * Add TI and PD to air interface message
     * Note: "sm_set_ti_pd" updates SDU length and offset according
     * to TI size (i.e. whether extended TIs are needed)
     */
    sm_set_ti_pd(ti, (T_MSGBUF *)&prim->sdu);

    if (context != NULL) {
      /* Store coded message for retransmission etc. */
      sm_allocate_and_copy_coded_msg(context, est_cause, &prim->sdu);
    }

    /* No sending primitives if SM is suspended or not attached
     * - resume will cause retransmission */
    if (!sm_is_suspended() && sm_is_attached()) {
      (void)PSEND(hCommMM, prim);
    } else {
      (void)TRACE_EVENT( "Primitive NOT sent to GMM. SM is suspended or GPRS not attached" );
      sm_pfree(prim);
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_mm_send_msg
+------------------------------------------------------------------------------
| Description : Send message using MM.  Codes message using CCD, stores it
|               for retransmission, and sends it (unless SM is suspended).
|
| Parameters  : context     - Context data
|               est_cause   - establish cause (for MM)
|               msg_bitlen  - Maximum message bit length
+------------------------------------------------------------------------------
*/
static void sm_mm_send_msg(struct T_SM_CONTEXT_DATA *context, U8 est_cause,
                           BOOL store_coded_msg, int msg_bitlen)
{
  (void)TRACE_FUNCTION("sm_mm_send_msg");

  {
    T_MMPM_UNITDATA_REQ *mmpm_unitdata_req;

    mmpm_unitdata_req = (T_MMPM_UNITDATA_REQ *)
      vsi_c_new_sdu(SIZ(T_MMPM_UNITDATA_REQ), MMPM_UNITDATA_REQ,
                    (U16)msg_bitlen, offsetof(T_MMPM_UNITDATA_REQ, sdu), /*lint -e413*/
                    ENCODE_OFFSET  FILE_LINE_MACRO);

    TRACE_ASSERT(mmpm_unitdata_req != NULL);

    send_mmpm_unitdata_req(context->ti, est_cause,
                           (store_coded_msg ? context : NULL),
                           mmpm_unitdata_req);
  }
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : resend_msg
+------------------------------------------------------------------------------
| Description : Resend previously coded message
|
| Parameters  : ti          - Transaction identifier
|               est_cause   - establish cause (for MM)
+------------------------------------------------------------------------------
*/
void resend_msg(struct T_SM_CONTEXT_DATA *context)
{
  T_sdu  *message = context->coded_msg;

  (void)TRACE_FUNCTION("resend_msg");

  /* Send nothing if SM is suspended or not attached */
  if (sm_is_suspended() || !sm_is_attached())
  {
    return;
  }

  if (message != NULL)
  {
    T_MMPM_UNITDATA_REQ *prim;

    prim = (T_MMPM_UNITDATA_REQ *)
      vsi_c_new_sdu(SIZ(T_MMPM_UNITDATA_REQ), MMPM_UNITDATA_REQ,
                    message->l_buf, offsetof(T_MMPM_UNITDATA_REQ, sdu), /*lint -e413*/
                    ENCODE_OFFSET  FILE_LINE_MACRO);

    TRACE_ASSERT (prim != NULL);

    if (prim EQ NULL)
      return;

    prim->sdu.l_buf = message->l_buf;
    memcpy(&prim->sdu.buf[(prim->sdu.o_buf >> 3)], message->buf,
           (size_t)(message->l_buf >> 3));

    /* Originating entity is SM */
    prim->org_entity        = (U8)NAS_ORG_ENTITY_SM;

    /* Add establish cause and PDP context status elements */
    prim->v_establish_cause = (U8)TRUE;
    prim->establish_cause   = (U8)message->o_buf;

    prim->v_nsapi_set       = (U8)TRUE;
    prim->nsapi_set         = sm_get_pdp_context_status();

    (void)PSEND(hCommMM, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_mmpm_pdp_context_status_req
+------------------------------------------------------------------------------
| Description : Allocate, pack and send MMPM_PDP_CONTEXT_STATUS_REQ primitive.
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void send_mmpm_pdp_context_status_req(void)
{
  (void)TRACE_FUNCTION("send_mmpm_pdp_context_status_req");
  {
    PALLOC(prim, MMPM_PDP_CONTEXT_STATUS_REQ);

    TRACE_ASSERT(prim != NULL);
/*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi_set         = sm_get_pdp_context_status();

    (void)PSEND(hCommMM, prim);
/*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */	
  }
}

/* Air Interface Message formatting functions */
void send_msg_activate_pdp_context_request(struct T_SM_CONTEXT_DATA *context)
{
  int   addr_len;
  MCAST(msg, ACTIVATE_PDP_CONTEXT_REQUEST);

  (void)TRACE_FUNCTION("send_msg_activate_pdp_context_request");

  msg->msg_type        = (U8)ACTIVATE_PDP_CONTEXT_REQUEST;
  msg->nsapi.nsapi_val = context->nsapi;

  msg->llc_sapi.sapi   = (U8)PS_SAPI_3;

  /*
   * Assign air message QoS parameters from SMREG parameters. Release conversion included.
   */
  sm_qos_convert_to_aim(&context->requested_qos,
                        &msg->qos, sm_get_current_nw_release());

  switch (context->pdp_type) {
  case SMREG_PDP_PPP:
    msg->address.pdp_type_org = (U8)M_SM_ETSI_ORG;
    msg->address.pdp_type_no  = (U8)M_SM_PPP_TYPE;
    break;
  case SMREG_PDP_IPV6:
    msg->address.pdp_type_org = (U8)M_SM_IETF_ORG;
    msg->address.pdp_type_no  = (U8)M_SM_IP6_TYPE;
    break;
  case SMREG_PDP_IPV4:
    msg->address.pdp_type_org = (U8)M_SM_IETF_ORG;
    msg->address.pdp_type_no  = (U8)M_SM_IP4_TYPE;
    break;
  case SMREG_PDP_EMPTY:
  default:
    msg->address.pdp_type_org = (U8)M_SM_PDP_MT;
    msg->address.pdp_type_no  = (U8)0;
  }

  switch (context->requested_address.ctrl_ip_address) {
  case NAS_is_ipv4:
    addr_len = NAS_SIZE_IPv4_ADDR;
    break;
  case NAS_is_ipv6:
    addr_len = NAS_SIZE_IPv6_ADDR;
    break;
  case NAS_is_ip_not_present: /* No IP address requested == dynamic IP */
  default:
    addr_len = 0;
    break;
  }
  msg->address.c_add_info = (U8)addr_len;
  memcpy(&msg->address.add_info,
         &context->requested_address.ip_address,
         (size_t)addr_len);

  /*
   * Access Point Name.
   */
  if (context->apn != NULL && context->apn->c_apn_buf > (U8)0) {
    msg->v_apn           = (U8)TRUE;
    msg->apn.c_apn_value = context->apn->c_apn_buf;
    memcpy(msg->apn.apn_value,
           context->apn->apn_buf,
           (size_t)context->apn->c_apn_buf);
  } else {
    msg->v_apn           = (U8)FALSE;
    msg->apn.c_apn_value = (U8)0;
  }

  /*
   * Protocol configuration options.
   */
  if (context->requested_pco != NULL &&
      context->requested_pco->c_pco_value > (U8)0) {
    msg->v_pco           = (U8)TRUE;
    msg->pco.c_pco_value = context->requested_pco->c_pco_value;
    memcpy(msg->pco.pco_value,
           context->requested_pco->pco_value,
           (size_t)context->requested_pco->c_pco_value);
  } else {
    msg->v_pco           = (U8)FALSE;
  }

  SM_MM_SEND_MSG(context, sm_mmpm_establish_cause(context),
                 TRUE, ACTIVATE_PDP_CONTEXT_REQUEST);
}

void send_msg_activate_secondary_pdp_context_request(struct T_SM_CONTEXT_DATA *context)
{
  MCAST(msg, ACTIVATE_SECONDARY_PDP_CONTEXT_REQUEST);

  (void)TRACE_FUNCTION("send_msg_activate_secondary_pdp_context_request");

  msg->msg_type        = (U8)ACTIVATE_SECONDARY_PDP_CONTEXT_REQUEST;
  msg->nsapi.nsapi_val = (U8)context->nsapi;

  msg->llc_sapi.sapi   = (U8)PS_SAPI_3;

  /*
   * Assign air message QoS parameters from SMREG parameters. Release conversion included.
   */
  sm_qos_convert_to_aim(&context->requested_qos,
                        &msg->qos, sm_get_current_nw_release());

  if (context->requested_tft.ptr_tft_pf != NULL)
  {
    msg->v_tft = (U8)TRUE;
    sm_tft_convert_to_aim(context, &msg->tft);
  } else {
    msg->v_tft = (U8)FALSE;
  }

  SM_MM_SEND_MSG(context, sm_mmpm_establish_cause(context),
                 TRUE, ACTIVATE_SECONDARY_PDP_CONTEXT_REQUEST);
}

void send_msg_deactivate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context)
{
  MCAST(msg, DEACTIVATE_PDP_CONTEXT_ACCEPT);

  (void)TRACE_FUNCTION("send_msg_deactivate_pdp_context_accept");

  msg->msg_type        = (U8)DEACTIVATE_PDP_CONTEXT_ACCEPT;

  SM_MM_SEND_MSG(context, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                 FALSE, DEACTIVATE_PDP_CONTEXT_ACCEPT);
}

void send_msg_deactivate_pdp_context_request(struct T_SM_CONTEXT_DATA *context, /*@partial@*/T_CAUSE_ps_cause *cause, BOOL tear_down)
{
  MCAST(msg, DEACTIVATE_PDP_CONTEXT_REQUEST);

  (void)TRACE_FUNCTION("send_msg_deactivate_pdp_context_request");

  msg->msg_type        = (U8)DEACTIVATE_PDP_CONTEXT_REQUEST;

  msg->sm_cause.sm_cause_val = sm_cause_ps_to_air(cause);

  if (tear_down) {
    msg->v_tear_down   = (U8)TRUE;
    msg->tear_down.tear_down_flag = (U8)1;
  } else {
    msg->v_tear_down   = (U8)FALSE;
  }

  SM_MM_SEND_MSG(context, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                 TRUE, DEACTIVATE_PDP_CONTEXT_REQUEST);
}

void send_msg_modify_pdp_context_accept(struct T_SM_CONTEXT_DATA *context)
{
  MCAST(msg, U_MODIFY_PDP_CONTEXT_ACCEPT);

  (void)TRACE_FUNCTION("send_msg_modify_pdp_context_accept");

  msg->msg_type        = (U8)U_MODIFY_PDP_CONTEXT_ACCEPT;

  SM_MM_SEND_MSG(context, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                 FALSE, U_MODIFY_PDP_CONTEXT_ACCEPT);
}

void send_msg_modify_pdp_context_request(struct T_SM_CONTEXT_DATA *context)
{
  MCAST(msg, U_MODIFY_PDP_CONTEXT_REQUEST);

  (void)TRACE_FUNCTION("send_msg_modify_pdp_context_request");

  msg->msg_type        = (U8)U_MODIFY_PDP_CONTEXT_REQUEST;

  if (context->sapi != (U8)PS_SAPI_0) {
    msg->v_llc_sapi    = (U8)TRUE;
    msg->llc_sapi.sapi = (U8)context->sapi;
  } else {
    msg->v_llc_sapi    = (U8)FALSE;
  }

  if (sm_qos_is_requested_qos_present(context)) {
    msg->v_qos         = (U8)TRUE;
    /* Assign air message QoS parameters from SMREG parameters. Release conversion included. */
    sm_qos_convert_to_aim(&context->requested_qos,
                          &msg->qos, sm_get_current_nw_release());
  } else {
    msg->v_qos         = (U8)FALSE;
  }

  if (sm_tft_more_to_modify(context))
  {
    msg->v_tft = (U8)TRUE;
    sm_tft_convert_to_aim(context, &msg->tft);
  } else {
    msg->v_tft = (U8)FALSE;
  }

  SM_MM_SEND_MSG(context, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                 TRUE, U_MODIFY_PDP_CONTEXT_REQUEST);
}

void send_msg_request_pdp_context_activation_reject(struct T_SM_CONTEXT_DATA *context, /*@partial@*/T_CAUSE_ps_cause *cause)
{
  MCAST(msg, REQUEST_PDP_CONTEXT_ACTIVATION_REJECT);

  (void)TRACE_FUNCTION("send_msg_request_pdp_context_activation_reject");

  msg->msg_type              = (U8)REQUEST_PDP_CONTEXT_ACTIVATION_REJECT;
  msg->sm_cause.sm_cause_val = (U8)sm_cause_ps_to_air(cause);
  {
    /* This message must ALWAYS have TI flag set, as it is a reply to the network. */
    U8 ti = context->ti | SM_TI_FLAG;
    PALLOC_MSG(mmpm_unitdata_req, MMPM_UNITDATA_REQ, REQUEST_PDP_CONTEXT_ACTIVATION_REJECT);

    send_mmpm_unitdata_req(ti, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                           NULL, mmpm_unitdata_req);
  }
}

void send_msg_sm_status(U8 ti, /*@partial@*/T_CAUSE_ps_cause *cause)
{
  MCAST(msg, SM_STATUS);

  (void)TRACE_FUNCTION("send_msg_sm_status");

  msg->msg_type              = (U8)SM_STATUS;
  msg->sm_cause.sm_cause_val = sm_cause_ps_to_air(cause);
  {
    PALLOC_MSG(mmpm_unitdata_req, MMPM_UNITDATA_REQ, SM_STATUS);

    send_mmpm_unitdata_req(ti, (U8)MMPM_CAUSE_ESTABL_ORIG_HIGH_PRIO_SIGNALLING,
                           NULL, mmpm_unitdata_req);
  }
}

/*==== END OF FILE ==========================================================*/
