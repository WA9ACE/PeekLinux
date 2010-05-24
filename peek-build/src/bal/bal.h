/* 
+------------------------------------------------------------------------------
|  File:       val.h
+------------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for the Protocol Stack Entity val.
+----------------------------------------------------------------------------- 
*/ 

#ifndef BAL_H
#define BAL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/


#define VSI_CALLER       bal_handle,



/*==== TYPES =================================================================*/


/*==== EXPORTS ===============================================================*/

short bal_pei_create (T_PEI_INFO **info);

#ifdef BAL_PEI_C


/* Communication handles */

T_HANDLE                 Bal2AcihComm = VSI_ERROR;//val recieve to ValUI
T_HANDLE                 bal_handle;



#endif /* BAL_PEI_C */

#endif /* !BAL_H */
