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

#ifndef UPM_MM_OUTPUT_HANDLER_H
#define UPM_MM_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/
#include "typedefs.h"

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

#ifdef TI_UMTS
extern void send_mmpm_reestablish_req(U8 most_demanding);
#endif /* TI_UMTS */

#ifdef TI_GPRS
extern void send_mmpm_sequence_res(T_SN_SEQUENCE_CNF *);
#endif /* TI_GPRS */

#endif /* UPM_MM_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
