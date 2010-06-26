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
|  Purpose :  This modul is part of the entity PPP and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (RT-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
/*lint -efile(766,gsm.h) */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
/*lint -efile(766,mon_ppp.h) */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_arbs.h"   /* to get signal interface from arb */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : rt_rt_expired
+------------------------------------------------------------------------------
| Description : Handles the expitration of the Restart Timer.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void rt_rt_expired ()
{
  TRACE_FUNCTION( "rt_rt_expired" );

  switch( GET_STATE( PPP_SERVICE_RT ) )
  {
    case RT_STARTED:
      SET_STATE( PPP_SERVICE_RT, RT_STOPPED );
      sig_rt_arb_to_ind();
      break;

    default:
      TRACE_ERROR( "RT_EXPIRED unexpected" );
      break;
  }
} /* rt_rt_expired() */
