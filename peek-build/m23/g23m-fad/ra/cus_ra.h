/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  CUS_RA
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
|  Purpose :  Custom dependent definitions for MMI of the
|             mobile station
|             
|             Use this header for definitions to integrate the
|             protocol stack entity MMI in your target system !
+----------------------------------------------------------------------------- 
*/ 

#ifndef CUS_RA_H
#define CUS_RA_H

/*==== CONSTANTS ==================================================*/

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */

#define T_VERSION   char
#define VERSION_RA  "RA 2.0" /* NEW_FRAME only */

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
  #define VSI_CALLER ra_handle,
  #define VSI_CALLER_SINGLE ra_handle
#else
  #define VSI_CALLER
  #define VSI_CALLER_SINGLE
#endif

#endif
