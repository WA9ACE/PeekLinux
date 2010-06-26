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
| Purpose:    Output functions for primitives from SM to the UPM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_UPM_OUTPUT_HANDLER_H
#define SM_UPM_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/

#include "sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void send_sm_activate_started_ind(struct T_SM_CONTEXT_DATA *context);
extern void send_sm_activate_ind(struct T_SM_CONTEXT_DATA *context);
extern void send_sm_deactivate_ind(U16 nsapi_set, T_PS_rel_ind rel_ind);
extern void send_sm_modify_ind(struct T_SM_CONTEXT_DATA *context);

#endif /* SM_UPM_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
