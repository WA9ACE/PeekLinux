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
| Purpose:    Output functions for primitives from SM to the ACI entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_ACI_OUTPUT_HANDLER_H
#define SM_ACI_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/

#include "sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void send_smreg_pdp_activate_cnf(struct T_SM_CONTEXT_DATA *context);
extern void send_smreg_pdp_activate_ind(struct T_SM_CONTEXT_DATA *context);
extern void send_smreg_pdp_activate_rej(U8 nsapi, /*@partial@*/T_CAUSE_ps_cause *cause);

extern void send_smreg_pdp_activate_sec_cnf(struct T_SM_CONTEXT_DATA *context);
extern void send_smreg_pdp_activate_sec_rej(U8 nsapi, /*@partial@*/T_CAUSE_ps_cause *cause);

extern void send_smreg_pdp_deactivate_cnf(U16 nsapi_set);
extern void send_smreg_pdp_deactivate_ind(U16 nsapi_set, /*@partial@*/T_CAUSE_ps_cause *cause);

extern void send_smreg_pdp_modify_cnf(struct T_SM_CONTEXT_DATA *context,
				      T_SM_UPDATE_FLAGS update_flags);
extern void send_smreg_pdp_modify_ind(struct T_SM_CONTEXT_DATA *context,
				      T_SM_UPDATE_FLAGS update_flags);
extern void send_smreg_pdp_modify_ind_multiple(U16 nsapi_set,
					       T_SM_UPDATE_FLAGS update_flags);
extern void send_smreg_pdp_modify_rej(U8 nsapi, /*@partial@*/T_CAUSE_ps_cause *cause);

#endif /* SM_ACI_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
