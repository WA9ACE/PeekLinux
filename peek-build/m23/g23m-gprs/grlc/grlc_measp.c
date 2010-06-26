/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|  Purpose :  This module implements primitive handler functions for service
|             MEAS of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_MEASP_C
#define GRLC_MEASP_C
#endif /* #ifndef GRLC_MEASP_C */

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"
#include "ccdapi.h"
#include "prim.h"
#include "message.h"
#include "grlc.h"
#include "grlc_measp.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== LOCAL TYPES===========================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_int_level_req 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_int_level_req ( T_CGRLC_INT_LEVEL_REQ *int_level_req )
{
  TRACE_FUNCTION( "meas_int_level_req" );

  grlc_data->meas.ilev = *int_level_req;

  PFREE( int_level_req );

} /* meas_int_level_req() */
