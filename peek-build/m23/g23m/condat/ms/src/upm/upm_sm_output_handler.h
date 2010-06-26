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
| Purpose:    Output functions for primitives from UPM to the SM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef _UPM_SM_OUTPUT_HANDLER_H
#define _UPM_SM_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/
#include "typedefs.h"

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/
 
extern void send_sm_activate_res    (struct T_CONTEXT_DATA *, void* data);
extern void send_sm_modify_res      (struct T_CONTEXT_DATA *);
extern void send_sm_deactivate_res  (U16 nsapi_set);
extern void send_sm_status_req      (U16 nsapi_set, T_PS_sapi llc_sapi,
				     T_CAUSE_ctrl_value ctrl, U16 cause);

#endif /* _UPM_SM_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
