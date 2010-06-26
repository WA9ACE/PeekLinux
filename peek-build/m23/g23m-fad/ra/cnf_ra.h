/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
+----------------------------------------------------------------------------- 
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
|  Purpose :  Dynamic Configuration for Rate Adaptation Module.
+----------------------------------------------------------------------------- 
*/ 

#ifndef CNF_RA_H
#define CNF_RA_H

/*==== CONSTANTS ==================================================*/
/*
 * CONFIGURATION PARAMETER
 *
 * Description :  The constants define the commands for dynamic
 *                configuration proposals.
 */

#ifdef _SIMULATION_
#define  RA_CONFIRM         "RA_CONFIRM"
#define  RA_DEACTIVATE      "RA_DEACTIVATE"
#endif /* _SIMULATION_ */

/*==== TYPES ======================================================*/

/*==== EXPORT =====================================================*/

#endif

