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
| Purpose:    User Plane Control state machine definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_CONTEXT_CONTROL_H
#define SM_CONTEXT_CONTROL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Incoming primitives */
  SM_P_SMREG_PDP_ACTIVATE_REQ,
  SM_P_SMREG_PDP_ACTIVATE_REJ_RES,
  SM_P_SMREG_PDP_ACTIVATE_SEC_REQ,
  SM_P_SMREG_PDP_MODIFY_REQ,
  /* Events from Sequencer */
  SM_I_CONTEXT_DEACTIVATE,
  SM_I_CONTEXT_LOCAL_DEACTIVATE,
  SM_I_CONTEXT_REACTIVATE,
  SM_I_CONTEXT_UPGRADE,
  SM_I_CONTEXT_USER_PLANE_ERROR,
  /* Events from Network Control */
  SM_I_NETWORK_ACTIVATE_COMPLETED,
  SM_I_NETWORK_ACTIVATE_REJECTED,
  SM_I_NETWORK_DEACTIVATE_COMPLETED,
  SM_I_NETWORK_MODIFY_REJECTED,
  SM_I_NETWORK_MODIFY_COMPLETED,
  SM_I_NETWORK_REQUEST_ACTIVATION,
  SM_I_NETWORK_REQUEST_ACTIVATION_OVERRIDE,
  SM_I_NETWORK_REQUEST_DEACTIVATE,
  SM_I_NETWORK_REQUEST_MODIFY,
  SM_I_NETWORK_REQUEST_MODIFY_FAILED,
  /* Events from User Plane Control */
  SM_I_USER_PLANE_ACTIVATED,
  SM_I_USER_PLANE_DEACTIVATED,
  SM_I_USER_PLANE_MODIFIED,
  /* Events from Context Deactivate Control */
  SM_I_DEACTIVATE_COMPLETED,
  /* Number of events */
  SM_CONTEXT_CONTROL_NUMBER_OF_EVENTS
} T_SM_CONTEXT_CONTROL_EVENT;

typedef enum
{
  SM_CONTEXT_DEACTIVATED,
  SM_CONTEXT_ACTIVATING_MT,
  SM_CONTEXT_ACTIVATING_NETWORK,
  SM_CONTEXT_ACTIVATING_USER_PLANE,
  SM_CONTEXT_ACTIVATE_FAILED_DEACTIVATING,
  SM_CONTEXT_ACTIVATED,
  SM_CONTEXT_ACTIVATION_OVERRIDE,
  SM_CONTEXT_MODIFYING_IN_NETWORK,
  SM_CONTEXT_MODIFYING_USER_PLANE,
  SM_CONTEXT_MODIFYING_USER_PLANE_MT,
  SM_CONTEXT_UPGRADING_IN_NETWORK,
  SM_CONTEXT_REACTIVATING_DEACTIVATING_USER_PLANE,
  SM_CONTEXT_REACTIVATING_IN_NETWORK,
  SM_CONTEXT_REACTIVATING_USER_PLANE,
  SM_CONTEXT_DEACTIVATING,
  SM_CONTEXT_DEACTIVATING_SEQUENCED,
  /* Number of states */
  SM_CONTEXT_CONTROL_NUMBER_OF_STATES
} T_SM_CONTEXT_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/

extern void sm_context_control_init(struct T_SM_CONTEXT_DATA *context);
extern void sm_context_control_exit(struct T_SM_CONTEXT_DATA *context);
#ifdef DEBUG
extern /*@observer@*/
const char *sm_context_control_state(struct T_SM_CONTEXT_DATA *context);
#endif
extern void sm_context_control(struct T_SM_CONTEXT_DATA *context,
			       T_SM_CONTEXT_CONTROL_EVENT event,
			       /*@null@*/ void *data);

#endif /* SM_CONTEXT_CONTROL_H */
/*==== END OF FILE ==========================================================*/
