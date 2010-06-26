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
| Purpose:    SNDCP Control state machine definitions in the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef _UPM_SNDCP_CONTROL_H
#define _UPM_SNDCP_CONTROL_H

/*==== INCLUDES =============================================================*/

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/
typedef enum
{
  /* Incoming primitives */
  UPM_P_SN_ACTIVATE_CNF,
  UPM_P_SN_DEACTIVATE_CNF,
  UPM_P_SN_MODIFY_CNF,
  UPM_P_SN_STATUS_IND,
  UPM_P_MMPM_SEQUENCE_IND,
  /* Events from Link Control */
  UPM_I_SNDCP_AWAIT_ESTABLISH,
  UPM_I_SNDCP_ESTABLISH_LLC_LINK,
  UPM_I_SNDCP_MODIFY_LLC_LINK,
  UPM_I_SNDCP_RELEASE_LLC_LINK,
  /* Number of events */
  UPM_SNDCP_CONTROL_NUMBER_OF_EVENTS
} T_UPM_SNDCP_CONTROL_EVENT;

typedef enum
{
  UPM_SNDCP_INACTIVE,
  UPM_SNDCP_AWAIT_ACTIVATE,
  UPM_SNDCP_ACTIVATING,
  UPM_SNDCP_ACTIVE,
  UPM_SNDCP_MODIFYING,
  UPM_SNDCP_DEACTIVATING,
  UPM_SNDCP_CONTROL_NUMBER_OF_STATES
} T_UPM_SNDCP_CONTROL_STATE;

/*==== EXPORTS ==============================================================*/
extern void upm_sndcp_control_init(struct T_CONTEXT_DATA *ptr_context_data);

extern void upm_sndcp_control_exit(struct T_CONTEXT_DATA *ptr_context_data);

extern void upm_sndcp_control(struct T_CONTEXT_DATA *ptr_context_data,
			      T_UPM_SNDCP_CONTROL_EVENT event,
			      /*@null@*/ void *data);

extern void upm_sndcp_dispatch_sn_count_cnf      (T_SN_COUNT_CNF *);
extern void upm_sndcp_dispatch_sn_status_ind     (T_SN_STATUS_IND *);
extern void upm_sndcp_dispatch_sn_sequence_cnf   (T_SN_SEQUENCE_CNF *);

extern void upm_sndcp_dispatch_mmpm_sequence_ind (T_MMPM_SEQUENCE_IND *);

extern void upm_sndcp_dispatch_upm_count_req     (T_UPM_COUNT_REQ *);
#endif /* _UPM_SNDCP_CONTROL_H */
/*==== END OF FILE ==========================================================*/
