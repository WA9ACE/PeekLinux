/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  CUS_T30
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
|             T30
+----------------------------------------------------------------------------- 
*/ 

#ifndef CUS_T30_H
#define CUS_T30_H

/*==== CONSTANTS ==================================================*/
/*
 * TIMER_VALUES
 *
 * Description :  The constants define the timer values
 *                for the T30 timers
 *                If the target system uses other units please
 *                change the values.
 */


#define T1_VALUE   35000  /* ms +/- 5000 ms */
#define T2_VALUE    6000  /* ms +/- 1000 ms */
#define T4_VALUE    3000  /* ms +/- 15%     */
#define DCN_VALUE    500
#define MSG_VALUE  12500
#define FRAMES_400  1846

/*
 * TIMER IDENTIFIER
 *
 * Description :  The constants define the identifier of the
 *                timer resources of T30. The identifier is used
 *                to reference a timer in a config primitive
 */

#define T1_NAME "T1"
#define T2_NAME "T2"
#define T4_NAME "T4"

/*
 * MAX_T30_TIMER
 *
 * Description :  The constant define the number of timer
 *                available in the timer pool. This should be
 *                the amount of timer which can be active
 *                simultaneously.
 */

#define MAX_T30_TIMER           3

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */

#define T_VERSION      char
#define VERSION_T30  "T30 2.0" /* NEW_FRAME only */

/*
 * VSI_CALLER
 *
 * Description :  For multithread applications the constant VSI_CALLER
 *                must be defined to identify the calling thread of the
 *                VSI-Interface. This must be done correponding to the
 *                type of T_VSI_CALLER in VSI.H. The comma symbol is
 *                neccessary because the vsi-functions are called
 *                like this vsi_t30 (VSI_CALLER par2, par3 ...)
 */

#ifdef OPTION_MULTITHREAD
  #define VSI_CALLER t30_handle,
  #define VSI_CALLER_SINGLE t30_handle
#else
  #define VSI_CALLER
  #define VSI_CALLER_SINGLE
#endif

#endif
