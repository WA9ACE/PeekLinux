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

#ifndef UPM_DISPATCHER_GSM_H
#define UPM_DISPATCHER_GSM_H

/*==== INCLUDES =============================================================*/

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void upm_disp_mmpm_sequence_ind (T_MMPM_SEQUENCE_IND *prim);
extern void upm_disp_sn_activate_cnf   (T_SN_ACTIVATE_CNF   *prim);
extern void upm_disp_sn_count_cnf      (T_SN_COUNT_CNF      *prim);
extern void upm_disp_sn_deactivate_cnf (T_SN_DEACTIVATE_CNF *prim);
extern void upm_disp_sn_modify_cnf     (T_SN_MODIFY_CNF     *prim);
#endif /* UPM_DISPATCHER_GSM_H */
/*==== END OF FILE ==========================================================*/
