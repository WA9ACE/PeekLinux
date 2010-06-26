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
| Purpose:    Output functions for primitives from UPM to the ACI entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef UPM_ACI_OUTPUT_HANDLER_H
#define UPM_ACI_OUTPUT_HANDLER_H

/*==== INCLUDES =============================================================*/
#include "typedefs.h"

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void send_upm_count_cnf(U8 nsapi, U32 ul_bytecount, U32 dl_bytecount,
                               U32 ul_pktcount, U32 dl_pktcount);
extern void send_upm_dti_cnf(U32 dti_linkid, U8 dti_conn);

#endif /* UPM_ACI_OUTPUT_HANDLER_H */
/*==== END OF FILE ==========================================================*/
