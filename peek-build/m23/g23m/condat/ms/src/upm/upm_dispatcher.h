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
| Purpose:    Process dispatcher helper functions in the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef UPM_DISPATCHER_H
#define UPM_DISPATCHER_H

/*==== INCLUDES =============================================================*/
#include "typedefs.h"

#include "upm.h"

#ifdef TI_UMTS
#include "upm_dispatcher_umts.h"
#endif

#ifdef TI_GPRS
#include "upm_dispatcher_gsm.h"
#endif

#ifdef TI_DUAL_MODE
#include "upm_dispatcher_dual_mode.h"
#endif

#ifndef UPM_WITHOUT_USER_PLANE
#include "upm_dispatcher_user_plane.h"
#endif

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void upm_disp_sm_activate_started_ind (T_SM_ACTIVATE_STARTED_IND *prim);
extern void upm_disp_sm_activate_ind (T_SM_ACTIVATE_IND *sm_activate_ind);
extern void upm_disp_sm_modify_ind (T_SM_MODIFY_IND *sm_modify_ind);

extern void upm_disp_upm_count_req (T_UPM_COUNT_REQ *prim);

#endif /* UPM_DISPATCHER_H */
/*==== END OF FILE ==========================================================*/
