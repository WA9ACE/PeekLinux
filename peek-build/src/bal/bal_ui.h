/* 
+------------------------------------------------------------------------------
|  File:       xxx.h
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
|  Purpose :  Definitions for the Protocol Stack Entity xxx.
+----------------------------------------------------------------------------- 
*/ 

#ifndef VALUI_H
#define VALUI_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

#define VSI_CALLER       valui_handle,



/*==== TYPES =================================================================*/


/*==== EXPORTS ===============================================================*/

short balUI_pei_create (T_PEI_INFO **info);

#ifdef VALUI_PEI_C

/* Communication handles */
T_HANDLE                 valui_handle;

#endif /* VALUI_PEI_C */

#endif /* !VALUI_H */
