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
| Purpose:    Sequencer state machine definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_SEQUENCER_H
#define SM_SEQUENCER_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

typedef enum
{
  /* Incoming primitives */
  SM_P_MMPM_ATTACH_IND,
  SM_P_MMPM_DETACH_IND,
  SM_P_SMREG_PDP_DEACTIVATE_REQ,
  SM_P_SM_STATUS_REQ,
  /* Events from Context Control */
  SM_I_CONTEXT_ACTIVATION_OVERRIDE,
  SM_I_CONTEXT_DEACTIVATE_COMPLETED,
  SM_I_CONTEXT_TEAR_DOWN_DEACTIVATE,
  SM_I_CONTEXT_REACTIVATE_COMPLETED,
  /* Number of events */
  SM_SEQUENCER_NUMBER_OF_EVENTS
} T_SM_SEQUENCER_EVENT;

/*==== EXPORTS ==============================================================*/

extern void sm_sequencer_init(void);
extern void sm_sequencer_exit(void);

extern void sm_sequencer(T_SM_SEQUENCER_EVENT event,
			 void *data);

#endif /* SM_SEQUENCER_H */
/*==== END OF FILE ==========================================================*/
