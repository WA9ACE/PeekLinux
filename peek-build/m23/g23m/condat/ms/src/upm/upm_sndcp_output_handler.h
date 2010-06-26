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
| Purpose:    Output functions for primitives from UPM to the MM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef UPM_SNDCP_OUTPUT_HANDLER_H
#define UPM_SNDCP_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/
#include "typedefs.h"

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void send_sn_activate_req       (struct T_CONTEXT_DATA *,
                                        /*@null@*/ T_NAS_comp_params *, BOOL);
extern void send_sn_count_req          (U8 nsapi, U8 reset);
extern void send_sn_deactivate_req     (U8 nsapi, U8 rel_ind);
extern void send_sn_modify_req         (struct T_CONTEXT_DATA *);
extern void send_sn_sequence_req       (T_MMPM_SEQUENCE_IND *);

#ifdef TI_DUAL_MODE
extern void send_sn_get_pending_pdu_req(void);
#endif /* TI_DUAL_MODE */

#endif /* UPM_SNDCP_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
