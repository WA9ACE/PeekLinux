/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_KERF
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component Fax Adaptation 3.45 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef FAD_KERF_C
#define FAD_KERF_C
#endif

#define ENTITY_FAD

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_fad.h"
#include "mon_fad.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "fad.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERF            |
| STATE   : code                       ROUTINE : ker_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialise the fad data for the kernel process

*/

GLOBAL void ker_init (void)
{
  TRACE_FUNCTION ("ker_init()");

  memset (fad_data, 0, sizeof(T_FAD_DATA));
  INIT_STATE (KER, KER_NULL);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERF            |
| STATE   : code                       ROUTINE : ker_SetError        |
+--------------------------------------------------------------------+

  PURPOSE : report error encountered to T30 (ACI)
            stop forwarding data to T30

*/

GLOBAL void ker_SetError(USHORT cause)
{
  PALLOC (fad_error_ind, FAD_ERROR_IND);

  TRACE_FUNCTION ("ker_SetError()");

  fad_error_ind->cause = cause;
  PSENDX (T30, fad_error_ind);
}

