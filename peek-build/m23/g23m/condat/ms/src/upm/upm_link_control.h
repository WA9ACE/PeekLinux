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
| Purpose:    Link Control state machine definitions in the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef _UPM_LINK_CONTROL_H
#define _UPM_LINK_CONTROL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Incoming primitives */
  UPM_P_SM_ACTIVATE_STARTED_IND,
  UPM_P_SM_ACTIVATE_IND,
  UPM_P_SM_DEACTIVATE_IND,
  UPM_P_SM_MODIFY_IND,
  /* Events from DTI Control */
  UPM_I_LINK_DTI_CLOSED,
  /* Events from RAB Control */
  UPM_I_RAB_ESTABLISHED,
  UPM_I_RAB_RELEASED,
  /* Events from SNDCP Control */
  UPM_I_SNDCP_LLC_LINK_ESTABLISHED,
  UPM_I_SNDCP_LLC_LINK_MODIFIED,
  UPM_I_SNDCP_LLC_LINK_RELEASED,
  /* Events from RAT Change Control */
  UPM_I_LINK_RAT_CHANGED,
  /* Number of events */
  UPM_LINK_CONTROL_NUMBER_OF_EVENTS
} T_UPM_LINK_CONTROL_EVENT;

typedef enum
{
  UPM_LINK_RELEASED,
#ifdef TI_UMTS
  UPM_LINK_ESTABLISHING_RAB,
  UPM_LINK_AWAITING_ACTIVATE_UMTS,
  UPM_LINK_RAB_ESTABLISHED,
  UPM_LINK_RELEASING_RAB,
#endif /* TI_UMTS */
#ifdef TI_DUAL_MODE
  UPM_LINK_COMPLETING_RAT_CHANGE_TO_GSM,
  UPM_LINK_COMPLETING_RAT_CHANGE_TO_UMTS_INITIAL,
#endif /* TI_DUAL_MODE */
#ifdef TI_GPRS
  UPM_LINK_AWAITING_ACTIVATE_GSM,
  UPM_LINK_ESTABLISHING_LLC_LINK,
  UPM_LINK_LLC_LINK_ESTABLISHED,
  UPM_LINK_RELEASING_LLC_LINK,
#endif /* TI_GPRS */
  UPM_LINK_CONTROL_NUMBER_OF_STATES
} T_UPM_LINK_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/
extern void upm_link_control_init(struct T_CONTEXT_DATA *ptr_context_data);

extern void upm_link_control_exit(struct T_CONTEXT_DATA *ptr_context_data);

extern void upm_link_control(struct T_CONTEXT_DATA *ptr_context_data,
			     T_UPM_LINK_CONTROL_EVENT event,
			     /*@null@*/ void *data);

extern BOOL upm_is_link_released(struct T_CONTEXT_DATA *ptr_context_data);

extern void upm_link_dispatch_sm_deactivate_ind(T_SM_DEACTIVATE_IND *);

#endif /* _UPM_LINK_CONTROL_H */
/*==== END OF FILE ==========================================================*/
