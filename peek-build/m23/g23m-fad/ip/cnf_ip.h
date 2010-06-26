/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-FaD (8411)
|  Modul   :  CNF_IP
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
|  Purpose :  Dynamic Configuration for
|             Radio Link Protocol
+----------------------------------------------------------------------------- 
*/ 

#ifndef CNF_IP_H
#define CNF_IP_H

/*==== CONSTANTS ==================================================*/
/*
 * CONFIGURATION PARAMETER
 *
 * Description :  The constants define the commands for dynamic
 *                configuration proposals.
 */

#define  IP_TIMER_SET       "TIMER_SET"
#define  IP_TIMER_RESET     "TIMER_RESET"
#define  IP_TIMER_SPEED_UP  "TIMER_SPEED_UP"
#define  IP_TIMER_SLOW_DOWN "TIMER_SLOW_DOWN"
#define  IP_TIMER_SUPPRESS  "TIMER_SUPPRESS"
#define  IP_FRAME_TRACE     "FRAME_TRACE"

/*==== TYPES ======================================================*/

/*==== EXPORT =====================================================*/

#endif

