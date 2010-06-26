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
| Purpose:    Context Deactivate Control sub-state machine definitions in
|             the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_CONTEXT_DEACTIVATE_CONTROL_H
#define SM_CONTEXT_DEACTIVATE_CONTROL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Events from Context Control */
  SM_I_DEACTIVATE_NETWORK,
  SM_I_DEACTIVATE_USER_PLANE,
  SM_I_DEACTIVATE_NETWORK_AND_USER_PLANE,
  /* Events from Network Control (Forwarded by Context Control) */
  SM_I_DEACTIVATE_NETWORK_COMPLETED,
  /* Events from User Plane Control (Forwarded by Context Control) */
  SM_I_DEACTIVATE_USER_PLANE_COMPLETED,
  /* Number of events */
  SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_EVENTS
} T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT;

typedef enum
{
  SM_CONTEXT_DEACTIVATE_NULL,
  SM_CONTEXT_DEACTIVATE_NETWORK,
  SM_CONTEXT_DEACTIVATE_USER_PLANE,
  SM_CONTEXT_DEACTIVATE_BOTH,
  /* Number of states */
  SM_CONTEXT_DEACTIVATE_CONTROL_NUMBER_OF_STATES
} T_SM_CONTEXT_DEACTIVATE_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/

extern void sm_context_deactivate_control_init(struct T_SM_CONTEXT_DATA *context);
extern void sm_context_deactivate_control_exit(struct T_SM_CONTEXT_DATA *context);
#ifdef DEBUG
extern /*@observer@*/
const char *sm_context_deactivate_control_state(struct T_SM_CONTEXT_DATA *context);
#endif
extern void sm_context_deactivate_control(struct T_SM_CONTEXT_DATA *context,
					  T_SM_CONTEXT_DEACTIVATE_CONTROL_EVENT event,
					  /*@null@*/ void *data);

#endif /* SM_CONTEXT_DEACTIVATE_CONTROL_H */
/*==== END OF FILE ==========================================================*/
