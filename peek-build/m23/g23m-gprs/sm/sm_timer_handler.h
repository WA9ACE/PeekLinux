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
| Purpose:    3G Timer handler function definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_TIMER_HANDLER_H
#define SM_TIMER_HANDLER_H

/*==== INCLUDES =============================================================*/

#include "sm.h"

/*==== CONSTS ===============================================================*/

#define NWSM_T3380_TIMEOUT   30000
#define NWSM_T3381_TIMEOUT    8000
#define NWSM_T3390_TIMEOUT    8000

#define NWSM_MAX_TIMEOUTS        5

/*==== TYPES ================================================================*/

typedef enum {
  SM_TIMER_NONE,
  SM_TIMER_T3380,
  SM_TIMER_T3381,
  SM_TIMER_T3390
} T_SM_TIMER_TYPE;

/*==== EXPORTS ==============================================================*/

extern void sm_timer_start  (struct T_SM_CONTEXT_DATA *context, T_SM_TIMER_TYPE timer);
extern void sm_timer_restart(struct T_SM_CONTEXT_DATA *context);
extern void sm_timer_stop   (struct T_SM_CONTEXT_DATA *context);

#endif /* SM_TIMER_HANDLER_H */
/*==== END OF FILE ==========================================================*/
