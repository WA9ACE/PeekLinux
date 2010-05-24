/* 
+------------------------------------------------------------------------------
|  File:       balbuis.h
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
|  Purpose :  Definitions for the Protocol Stack Entity baluis.
+----------------------------------------------------------------------------- 
*/ 

#ifndef BAL_AMDB_H
#define BAL_AMDB_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/


#define VSI_CALLER       val_amdb_handle,



/*==== TYPES =================================================================*/


/*==== EXPORTS ===============================================================*/

short val_amdb_pei_create (T_PEI_INFO **info);

#ifdef BAL_AMDB_PEI_C

/* Communication handles */
T_HANDLE                 val_amdb_handle;

#endif /* BAL_AMDB_PEI_C */

#endif /* !BAL_AMDB_H */
