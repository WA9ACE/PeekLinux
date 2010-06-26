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
| Purpose:    Network Control state machine definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_NETWORK_CONTROL_H
#define SM_NETWORK_CONTROL_H

/*==== INCLUDES =============================================================*/

#include "message.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Incoming air interface messages */
  SM_M_ACTIVATE_PDP_CONTEXT_ACCEPT,
  SM_M_ACTIVATE_PDP_CONTEXT_REJECT,
  SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_ACCEPT,
  SM_M_ACTIVATE_SECONDARY_PDP_CONTEXT_REJECT,
  SM_M_DEACTIVATE_PDP_CONTEXT_REQUEST,
  SM_M_DEACTIVATE_PDP_CONTEXT_ACCEPT,
  SM_M_MODIFY_PDP_CONTEXT_REQUEST,
  SM_M_MODIFY_PDP_CONTEXT_ACCEPT,
  SM_M_MODIFY_PDP_CONTEXT_REJECT,
  SM_M_REQUEST_PDP_CONTEXT_ACTIVATION,
  SM_M_SM_STATUS,
  /* Incoming primitives */
  SM_P_MMPM_RESUME_IND,
  SM_P_MMPM_SUSPEND_IND,
  /* Incoming timeout events */
  SM_T_T3380,
  SM_T_T3380_MAX,
  SM_T_T3381,
  SM_T_T3381_MAX,
  SM_T_T3390,
  SM_T_T3390_MAX,
  /* Events from Context Control */
  SM_I_NETWORK_ACTIVATE_PRIMARY,
  SM_I_NETWORK_ACTIVATE_SECONDARY,
  SM_I_NETWORK_DEACTIVATE_LOCAL,
  SM_I_NETWORK_MODIFY,
  SM_I_NETWORK_REQUEST_ACTIVATION_REJECT,
  /* Events from Context Deactivate Control */
  SM_I_NETWORK_DEACTIVATE,
  /* Number of events */
  SM_NETWORK_CONTROL_NUMBER_OF_EVENTS
} T_SM_NETWORK_CONTROL_EVENT;

typedef enum
{
  SM_NETWORK_DEACTIVATED,
  SM_NETWORK_ACTIVATING_PRIMARY,
  SM_NETWORK_ACTIVATING_SECONDARY,
  SM_NETWORK_ACTIVATING_ADDING_TFTS,
  SM_NETWORK_ACTIVATED,
  SM_NETWORK_MODIFYING,
  SM_NETWORK_MODIFYING_ADDITIONAL_TFTS,
  SM_NETWORK_DEACTIVATING,
  /* Number of states */
  SM_NETWORK_CONTROL_NUMBER_OF_STATES
} T_SM_NETWORK_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/

extern void sm_network_control_init(struct T_SM_CONTEXT_DATA *context);
extern void sm_network_control_exit(struct T_SM_CONTEXT_DATA *context);

extern /*@observer@*/const char *sm_network_control_state(struct T_SM_CONTEXT_DATA *context);
extern void sm_network_control(struct T_SM_CONTEXT_DATA *context,
				  T_SM_NETWORK_CONTROL_EVENT event,
				  /*@null@*/ void *data);

#endif /* SM_NETWORK_CONTROL_H */

/*==== END OF FILE ==========================================================*/
