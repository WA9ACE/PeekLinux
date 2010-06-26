/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  MON_FAD
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
|  Purpose :  Monitoring Definitions for the Protocol Stack Entity
|             Fax Adaptation 3.45
+----------------------------------------------------------------------------- 
*/ 

#ifndef MON_FAD_H
#define MON_FAD_H

/*==== CONSTANTS ==================================================*/

/*
 * VERSION
 *
 * Description :  The constants define the type and the value
 *                of a version identification. The version
 *                is part of the monitor struct.
 */

#define T_VERSION    char
#define VERSION_FAD  "FAD 2.0" /* NEW_FRAME only */

/*==== TYPES ======================================================*/

typedef struct
{
  T_VERSION *version;
} T_MONITOR;

/*==== EXPORT =====================================================*/

#endif
