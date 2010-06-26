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

#ifndef SM_MM_OUTPUT_HANDLER_H
#define SM_MM_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/

#include "sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

/* Primitive sending functions */
extern void send_mmpm_pdp_context_status_req(void);

#if 0 /* Not used directly - only by message sending functions below */
extern void send_mmpm_unitdata_req(U8 ti, U8 est_cause, /*@only@*/ /*@null@*/T_MMPM_UNITDATA_REQ *prim);
#endif
extern void resend_msg(struct T_SM_CONTEXT_DATA *context);

/* Air Interface Message formatting functions */
extern void send_msg_activate_pdp_context_request(struct T_SM_CONTEXT_DATA *context);
extern void send_msg_activate_secondary_pdp_context_request(struct T_SM_CONTEXT_DATA *context);
extern void send_msg_deactivate_pdp_context_accept(struct T_SM_CONTEXT_DATA *context);
extern void send_msg_deactivate_pdp_context_request(struct T_SM_CONTEXT_DATA *context, /*@partial@*/T_CAUSE_ps_cause *cause, BOOL tear_down);
extern void send_msg_modify_pdp_context_accept(struct T_SM_CONTEXT_DATA *context);
extern void send_msg_modify_pdp_context_request(struct T_SM_CONTEXT_DATA *context);
extern void send_msg_request_pdp_context_activation_reject(struct T_SM_CONTEXT_DATA *context, /*@partial@*/T_CAUSE_ps_cause *cause);
extern void send_msg_sm_status(U8 ti, /*@partial@*/T_CAUSE_ps_cause *cause);

#endif /* SM_MM_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
