/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-FaD (8411)
|  Modul   :  CUS_IP
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
|  Purpose :  Custom dependent definitions for the 
|             Protocol Stack Entity
|             Radio Link Protocol
+----------------------------------------------------------------------------- 
*/ 

#ifndef CUS_IP_H
#define CUS_IP_H

/*==== CONSTANTS ==================================================*/
/*
 * TIMER_VALUES
 *
 * Description :  The constants define the timer values
 *                for the IP timers 
 *                If the target system uses other units please
 *                change the values.
 */


/*
 * TIMER IDENTIFIER
 *
 * Description :  The constants define the identifier of the
 *                timer resources of IP. The identifier is used
 *                to reference a timer in a config primitive
 */

#define TREASSEMBLY_NAME   "TREASSEMBLY"

/*
 * MAX_IP_TIMER
 *
 * Description :  The constant define the number of timer
 *                needed. This number is equal to the number
 *                of simultaneously instances.
 */

#define MAX_IP_TIMER   ( MAX_SEGM_SERVER * MAX_LAYER )

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */

#define T_VERSION      char

/* 
 * This is the first version with DTILIB
 */

#define VERSION_IP   "IP 1.1"

/*
 * VSI_CALLER
 *
 * Description :  For multithread applications the constant VSI_CALLER
 *                must be defined to identify the calling thread of the
 *                VSI-Interface. This must be done correponding to the
 *                type of T_VSI_CALLER in VSI.H. The comma symbol is
 *                neccessary because the vsi-functions are called
 *                like this vsi_xxx (VSI_CALLER par2, par3 ...)
 */


#ifdef OPTION_MULTITHREAD
#define VSI_CALLER ip_handle,
#define VSI_CALLER_SINGLE ip_handle
#else
#define VSI_CALLER
#define VSI_CALLER_SINGLE
#endif

#endif

