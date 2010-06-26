/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_rcvf.c
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
|             the component Radio Link Protocol of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_RCVF_C
#define RLP_RCVF_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : rcv_init
+------------------------------------------------------------------------------
|  Description : initialise the rlp data for the rcv formatter process
|
|  Parameters  : rlp_data_ptr -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rcv_init (void)
{
  TRACE_FUNCTION ("rcv_init()");
  
  INIT_STATE(RCV, RPDU_WAIT_FOR_A_BLOCK);
}
