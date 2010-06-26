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
| Purpose:    This module implements the process dispatcher for incoming
|             air interface messages in the Session Management (SM) entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES =============================================================*/

#include "sm.h"
#include "ccdapi.h"

#include "sm_network_control.h"
#include "sm_aci_output_handler.h"
#include "sm_mm_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_disp_mmpm_unitdata_ind
+------------------------------------------------------------------------------
| Description : Dispatch MMPM_UNITDATA_IND
|
| Parameters  : prim        - received primitive
+------------------------------------------------------------------------------
*/
void sm_disp_mmpm_unitdata_ind(T_MMPM_UNITDATA_IND *prim)
{
  U8                        ti, ti_flag, msg_type, *msg, ccdResult;
  U16                       offset;
  struct T_SM_CONTEXT_DATA *context;
  T_CAUSE_ps_cause          cause;

  (void)TRACE_FUNCTION("sm_disp_mmpm_unitdata_ind");

  offset     = (U16)(prim->sdu.o_buf + 7) >> 3; /* Offset in octets/bytes in message buffer */
  msg        = &prim->sdu.buf[offset];

  /* Read L3 header: First TI/PD octet */
  ti_flag    = (msg[0] & SM_TI_FLAG) == (U8)0 ? (U8)SM_TI_FLAG : (U8)0;
  ti         = (msg[0] >> 4) & SM_TI_NON_EXT_MASK;

  /* Check for extended TI: If TI == 7, read next octet for real TI */
  if (ti == (U8)SM_TI_EXTEND_VALUE) {
    ti       = msg[1]; /* Read real TI from next octet in message */

    if ((ti & (U8)SM_TI_FLAG) == (U8)0) {
      /* Messages with TI EXT bit = 0 shall be ignored: [3G 24.008, 8.3.2] */
      (void)TRACE_ERROR("Warning: TI EXT bit = 0 in message - message ignored!");
      return;
    } else {
      ti    &= SM_TI_MASK;
    }
    offset   = (U16)16; /* # bits to advance message pointer to reach message type */
    msg_type = msg[2]; /* Get message type octet after TI */
    (void)TRACE_EVENT_P1("  Extended TI found: 0x%02x", ti);
  } else {
    offset   = (U16)8;  /* # bits to advance message pointer to reach message type */
    (void)TRACE_EVENT_P1("  Non-extended TI found: 0x%02x", ti);
    msg_type = msg[1]; /* Get message type octet after TI */
  }
  /* Set highest bit iff the direction flag bit was set above */
  ti |= ti_flag;
  /* Advance message pointer past TI/PD octet(s) */
  prim->sdu.l_buf -= offset;
  prim->sdu.o_buf += offset;

  /* Check for message too short: [3G 24.008, 8.2] */
  if (prim->sdu.l_buf < (U16)8) {
    (void)TRACE_ERROR("Warning! Message too short - discarded...");
    return;
  }

  /* Find context with TI = ti, if already active/being activated */
  context = sm_get_context_data_from_ti(ti);

  switch (msg_type) {
  case ACTIVATE_PDP_CONTEXT_ACCEPT:
  case ACTIVATE_PDP_CONTEXT_REJECT:
  case ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT:
  case ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT:
  case D_MODIFY_PDP_CONTEXT_REQUEST:
  case D_MODIFY_PDP_CONTEXT_ACCEPT:
  case MODIFY_PDP_CONTEXT_REJECT:
  case DEACTIVATE_PDP_CONTEXT_REQUEST:
  case DEACTIVATE_PDP_CONTEXT_ACCEPT:
    if (context == NULL) {
      /* Message received on TI not relating to an active context, or context
       * being activated/deactivated: [3G 24.008, 8.3.2 b)]*/
      (void)TRACE_ERROR("Message received on TI not relating to an active context, or context being activated/deactivated");
      cause.ctrl_value       = CAUSE_is_from_nwsm;
      cause.value.nwsm_cause = (U16)CAUSE_NWSM_INVALID_TI;
      send_msg_sm_status(ti, &cause);
      return;
    }
    break;
  case REQUEST_PDP_CONTEXT_ACTIVATION:
    if (ti_flag == (U8)0) {
      /* Ignore REQUEST PDP CONTEXT ACTIVATION msg with TI flag = 1 [3G 24.008, 8.3.2 c)] */
      /* Note: flag bit is inverted above! */
      (void)TRACE_EVENT("REQUEST PDP CONTEXT ACTIVATION msg with TI flag = 1 ignored...");
      return;
    }
    if (context == NULL) {
      /* If no context exists for the TI, create one. */
      context = sm_insert_mt_context_data(ti);
    }
    break;
  case SM_STATUS:
    if (context == NULL) {
      /* Silently ignore message */
      return;
    }
    break;
  case ACTIVATE_PDP_CONTEXT_REQUEST:             /* FALL-THROUGH */
  case ACTIVATE_SECONDARY_PDP_CONTEXT_REQUEST:   /* FALL-THROUGH */
  case REQUEST_PDP_CONTEXT_ACTIVATION_REJECT:    /* FALL-THROUGH */
  case U_MODIFY_PDP_CONTEXT_REQUEST:             /* FALL-THROUGH */
  case U_MODIFY_PDP_CONTEXT_ACCEPT:              /* FALL-THROUGH */
  default:
    /* Message type not defined for the PD in the given direction: [3G 24.008, 8.4] */
    (void)TRACE_ERROR("Message type not defined for the PD in the given direction");
    cause.ctrl_value       = CAUSE_is_from_nwsm;
    cause.value.nwsm_cause = (U16)CAUSE_NWSM_MSG_TYPE_NOT_IMPLEMENTED;
    send_msg_sm_status(ti, &cause);
    return;
  } /* switch */

  TRACE_ASSERT(context != NULL);

  /* Decode message */
  ccdResult = ccd_decodeMsg((U8)CCDENT_SM,
			    (U8)DOWNLINK,
			    (T_MSGBUF *)&prim->sdu,
			    (UBYTE *) _decodedMsg,
			    (U8)NOT_PRESENT_8BIT);

  if (ccdResult != (U8)ccdOK) {
    /* Error Handling */
    U16    parlist[6];
    U8     first_err;
    U8     ccd_err;

    /* clear parlist */
    memset (parlist, 0, sizeof (parlist));
    first_err = ccd_getFirstError((U8)CCDENT_SM, parlist);

    ccd_err = first_err;
    /* Loop over errors in parlist.  Fix inherited from TI Berlin. */
    do
    {
    /* Error Handling */
    switch (ccd_err) {
      case ERR_COMPREH_REQUIRED:    /* Comprehension required */
      case ERR_MAND_ELEM_MISS: {    /* Mandatory elements missing */
        (void)TRACE_ERROR("CCD ERROR: Mandatory elements missing");
	if (msg_type == (U8)DEACTIVATE_PDP_CONTEXT_REQUEST) {
	  /* [3G 24.008, 8.5.5]: If msg_type is DEACTIVATE PDP CONTEXT REQUEST,
	   * respond with DEACTIVATE PDP CONTEXT ACCEPT and release context. */
	  send_msg_deactivate_pdp_context_accept(context);
	} else if (msg_type == (U8)REQUEST_PDP_CONTEXT_ACTIVATION) {
	  /* [3G 24.008, 8.5.5]: If msg_type is REQUEST PDP CONTEXT ACTIVATION,
	   * respond with SM STATUS, cause #96 "invalid mandatory information". */
	  cause.ctrl_value       = CAUSE_is_from_nwsm;
	  cause.value.nwsm_cause = (U16)CAUSE_NWSM_INVALID_MANDATORY_ELEMENT;
	  send_msg_sm_status(ti, &cause);
          return;
        } else {
 	  cause.ctrl_value       = CAUSE_is_from_nwsm;
	  cause.value.nwsm_cause = (U16)CAUSE_NWSM_INVALID_MANDATORY_ELEMENT;
	  send_msg_sm_status(ti, &cause);
          return;
	}
        /*@switchbreak@*/
	break;
      }
    } /* switch */
    ccd_err = ccd_getNextError ((U8)CCDENT_SM, parlist);
    } while (ccd_err != (U8)ERR_NO_MORE_ERROR);
  } /* if (ccdResult...) */

  switch (msg_type) {
  case ACTIVATE_PDP_CONTEXT_ACCEPT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT, _decodedMsg);
    break;
  case ACTIVATE_PDP_CONTEXT_REJECT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_ACTIVATE_PDP_CONTEXT_REJECT, _decodedMsg);
    break;
  case ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT, _decodedMsg);
    break;
  case ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT, _decodedMsg);
    break;
  case REQUEST_PDP_CONTEXT_ACTIVATION:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_REQUEST_PDP_CONTEXT_ACTIVATION, _decodedMsg);
    break;
  case D_MODIFY_PDP_CONTEXT_REQUEST:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_MODIFY_PDP_CONTEXT_REQUEST, _decodedMsg);
    break;
  case D_MODIFY_PDP_CONTEXT_ACCEPT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_MODIFY_PDP_CONTEXT_ACCEPT, _decodedMsg);
    break;
  case MODIFY_PDP_CONTEXT_REJECT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_MODIFY_PDP_CONTEXT_REJECT, _decodedMsg);
    break;
  case DEACTIVATE_PDP_CONTEXT_REQUEST:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST, _decodedMsg);
    break;
  case DEACTIVATE_PDP_CONTEXT_ACCEPT:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT, _decodedMsg);
    break;
  case SM_STATUS:
    /* Forward to Network Control */
    sm_network_control(context, SM_M_SM_STATUS, _decodedMsg);
    break;
  } /* switch */

  /* Deallocate context if it was marked for deactivation by the above operation */
  /*lint -e613 (Possible use of null pointer 'context' in left argument to operator '->') */
  if (sm_is_context_pending_deallocation(context)) {
    sm_free_context_data_by_nsapi(context->nsapi);
  }
}

/*==== END OF FILE ==========================================================*/
