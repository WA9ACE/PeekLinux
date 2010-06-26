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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (RT-statemachine)
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

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_arb_rt_parameters_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ARB_RT_PARAMETERS_REQ. It 
|               initializes the timer value.
|
| Parameters  : rt_time - restart time in seconds
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_arb_rt_parameters_req ( UBYTE rt_time ) 
{
  TRACE_ISIG( "sig_arb_rt_parameters_req" );

  switch( GET_STATE( PPP_SERVICE_RT ) )
  {
    case RT_STOPPED:
    case RT_STARTED:
      ppp_data->rt.time=(T_TIME)rt_time * 1000; /* convert sec. to msec.  */
      break;

    default:
      TRACE_ERROR( "SIG_ARB_RT_PARAMETERS_REQ unexpected" );
      break;
  }
} /* sig_arb_rt_parameters_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_rt_rrt_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_RT_RRT_REQ. It (re)starts 
|               the timer.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_rt_rrt_req () 
{
  TRACE_ISIG( "sig_any_rt_rrt_req" );

  vsi_t_start (VSI_CALLER RT_INDEX, ppp_data->rt.time); /*lint !e534 Ignoring return value */

  switch( GET_STATE( PPP_SERVICE_RT ) )
  {
    case RT_STOPPED:
      SET_STATE( PPP_SERVICE_RT, RT_STARTED );
      break;

    case RT_STARTED:
      break;

    default:
      TRACE_ERROR( "SIG_ANY_RT_RRT_REQ unexpected" );
      break;
  }
} /* sig_any_rt_rrt_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_any_rt_srt_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ANY_RT_SRT_REQ. It stops the 
|               timer.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_any_rt_srt_req () 
{
  TRACE_ISIG( "sig_any_rt_srt_req" );

  vsi_t_stop (VSI_CALLER RT_INDEX); /*lint !e534 Ignoring return value */

  switch( GET_STATE( PPP_SERVICE_RT ) )
  {
    case RT_STARTED:
      SET_STATE( PPP_SERVICE_RT, RT_STOPPED );
      break;

    case RT_STOPPED:
      break;

    default:
      TRACE_ERROR( "SIG_ANY_RT_SRT_REQ unexpected" );
      break;
  }
} /* sig_any_rt_srt_req() */
