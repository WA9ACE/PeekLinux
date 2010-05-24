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

#ifndef TRANS_H
#define TRANS_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/


#define VSI_CALLER       trans_handle,


/*==== TYPES =================================================================*/

/* XXX global typedefs */


/*==== EXPORTS ===============================================================*/

short trans_pei_create (T_PEI_INFO **info);

#ifdef TRANS_PEI_C


/* Communication handles */

T_HANDLE			trans_handle;
T_HANDLE			TranshComm = VSI_ERROR;

#endif /* XXX_PEI_C */

#endif /* !XXX_H */
