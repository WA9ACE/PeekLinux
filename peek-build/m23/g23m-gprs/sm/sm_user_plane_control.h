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

#ifndef SM_USER_PLANE_CONTROL_H
#define SM_USER_PLANE_CONTROL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Incoming primitives */
  SM_P_SM_ACTIVATE_RES,
  SM_P_SM_DEACTIVATE_RES,
  SM_P_SM_MODIFY_RES,
  /* Events from Context Control */
  SM_I_USER_PLANE_ACTIVATE_START,
  SM_I_USER_PLANE_ACTIVATE_COMPLETE,
  SM_I_USER_PLANE_DEACTIVATE_LOCAL,
  SM_I_USER_PLANE_MODIFY,
  /* Events from Context Deactivate Control */
  SM_I_USER_PLANE_DEACTIVATE,
  /* Number of events */
  SM_USER_PLANE_CONTROL_NUMBER_OF_EVENTS
} T_SM_USER_PLANE_CONTROL_EVENT;

typedef enum
{
  SM_USER_PLANE_DEACTIVATED,
  SM_USER_PLANE_ACTIVATE_STARTED,
  SM_USER_PLANE_ACTIVATE_COMPLETING,
  SM_USER_PLANE_ACTIVATED,
  SM_USER_PLANE_MODIFYING,
  SM_USER_PLANE_DEACTIVATING,
  /* Number of states */
  SM_USER_PLANE_CONTROL_NUMBER_OF_STATES
} T_SM_USER_PLANE_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/

extern void sm_user_plane_control_init(struct T_SM_CONTEXT_DATA *context);
extern void sm_user_plane_control_exit(struct T_SM_CONTEXT_DATA *context);
#ifdef DEBUG
extern /*@observer@*/
const char *sm_user_plane_control_state(struct T_SM_CONTEXT_DATA *context);
#endif
extern void sm_user_plane_control(struct T_SM_CONTEXT_DATA *context,
				  T_SM_USER_PLANE_CONTROL_EVENT event,
				  /*@null@*/ void *data);

#endif /* SM_USER_PLANE_CONTROL_H */
/*==== END OF FILE ==========================================================*/
