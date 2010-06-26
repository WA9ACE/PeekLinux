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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_RXS_C
#define LLC_RXS_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_rx_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_RX_ASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_rx_assign_req (void)
{ 
  TRACE_ISIG( "sig_llme_rx_assign_req" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_UNASSIGNED:
      SET_STATE (RX, RX_TLLI_ASSIGNED);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_RX_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_rx_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_rx_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_RX_UNASSIGN_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_rx_unassign_req (void)
{ 
  TRACE_ISIG( "sig_llme_rx_unassign_req" );
  
  switch( GET_STATE( RX ) )
  {
    case RX_TLLI_ASSIGNED:
      SET_STATE (RX, RX_TLLI_UNASSIGNED);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_RX_UNASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_rx_unassign_req() */
