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
| Purpose:    3G QoS utility function definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_TFT_H
#define SM_TFT_H

/*==== INCLUDES =============================================================*/

#include "sm.h"
#include "message.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern BOOL sm_tft_more_to_modify(struct T_SM_CONTEXT_DATA *context);
extern U8   sm_tft_precence_mask(T_NAS_tft_pf *tft, U8 count);

extern void sm_tft_convert_to_aim(struct T_SM_CONTEXT_DATA *context,
				  /*@out@*/T_M_SM_tft *dst_tft);

#endif /* SM_TFT_H */
/*==== END OF FILE ==========================================================*/
