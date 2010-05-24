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

#ifndef BALUIS_H
#define BALUIS_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/


#define VSI_CALLER       baluis_handle,



/*==== TYPES =================================================================*/


/*==== EXPORTS ===============================================================*/

short balUIS_pei_create (T_PEI_INFO **info);

#ifdef BALUIS_PEI_C

/* Communication handles */
T_HANDLE                 baluis_handle;

#endif /* BALUIS_PEI_C */

#endif /* !BALUIS_H */
