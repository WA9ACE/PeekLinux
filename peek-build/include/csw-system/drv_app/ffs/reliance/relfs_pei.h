 /*-----------------------------------------------------------------------------
|  Project :  
|  Module  :  RELIANCE 
+------------------------------------------------------------------------------
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
+------------------------------------------------------------------------------
| Purpose:    Definitions for the Protocol Stack Entity Datalight Reliance .
+----------------------------------------------------------------------------*/

#ifndef __REL_PEI_H_
#define __REL_PEI_H_

/*==== INCLUDES =============================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */


/*==== CONSTS ===============================================================*/

#define ENTITY_DATA      ril_data
#define VSI_CALLER	ril_task_handle


/* make the pei_create function unique */
#define pei_create       ffs_pei_create           

/*==== EXPORTS ===============================================================*/

extern short pei_create (T_PEI_INFO **info);

/* Changing entity name to FFS from RIL to have the comptability with other layers */
#define RIL_NAME    		"FFS\0\0"
/* Communication handles */ 




#endif /* !REL_PEI_H */

