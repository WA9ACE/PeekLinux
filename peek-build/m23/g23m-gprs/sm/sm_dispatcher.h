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
| Purpose:    Process dispatcher helper functions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_DISPATCHER_H
#define SM_DISPATCHER_H

/*==== INCLUDES =============================================================*/

#include "sm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

/* SMREG SAP */
extern void sm_disp_smreg_pdp_activate_req(T_SMREG_PDP_ACTIVATE_REQ *prim);
extern void sm_disp_smreg_pdp_activate_sec_req(T_SMREG_PDP_ACTIVATE_SEC_REQ *prim);
extern void sm_disp_smreg_pdp_activate_rej_res(T_SMREG_PDP_ACTIVATE_REJ_RES *prim);
extern void sm_disp_smreg_pdp_deactivate_req(T_SMREG_PDP_DEACTIVATE_REQ *prim);
extern void sm_disp_smreg_pdp_modify_req(T_SMREG_PDP_MODIFY_REQ *prim);

/* SM SAP */
extern void sm_disp_sm_activate_res(T_SM_ACTIVATE_RES *prim);
extern void sm_disp_sm_deactivate_res(T_SM_DEACTIVATE_RES *prim);
extern void sm_disp_sm_modify_res(T_SM_MODIFY_RES *prim);
extern void sm_disp_sm_status_req(T_SM_STATUS_REQ *prim);

/* MMPM SAP */
extern void sm_disp_mmpm_attach_ind(T_MMPM_ATTACH_IND *prim);
extern void sm_disp_mmpm_detach_ind(T_MMPM_DETACH_IND *prim);
extern void sm_disp_mmpm_resume_ind(T_MMPM_RESUME_IND *prim);
extern void sm_disp_mmpm_suspend_ind(T_MMPM_SUSPEND_IND *prim);

/* Handled in sm_dispatch_message.c */
extern void sm_disp_mmpm_unitdata_ind(T_MMPM_UNITDATA_IND *prim);

/* Timer handling - sm_timer_handler.c */
extern void sm_disp_pei_timeout(U16 index);
#endif /* SM_DISPATCHER_H */
/*==== END OF FILE ==========================================================*/
