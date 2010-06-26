/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdyf.c
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
|  Purpose :  This modul is part of the entity GMM and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (RDY-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef GMM_RDYF_C
#define GMM_RDYF_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/
#include <stdio.h>      /* to get Sprintf */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : rdy_cgrlc_ready_timer_config_req
+------------------------------------------------------------------------------
| Description : this procedure sends ready_timer value to CGRLC
|
| Parameters  : ready_timer value
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_cgrlc_ready_timer_config_req (ULONG timer)
{
  GMM_TRACE_FUNCTION( "rdy_cgrlc_ready_timer_config_req" );

  TRACE_1_OUT_PARA ("ready_timer value: %x", timer);

  {
    PALLOC ( cgrlc_ready_timer_config_req, CGRLC_READY_TIMER_CONFIG_REQ);
    cgrlc_ready_timer_config_req->t3314_val = timer;
    PSEND ( hCommGRLC, cgrlc_ready_timer_config_req );
  }

} /* rdy_cgrlc_ready_timer_config_req */



/*
+------------------------------------------------------------------------------
| Function    : rdy_init
+------------------------------------------------------------------------------
| Description : The function rdy_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_init ( void )
{ 
  USHORT timer;
  TRACE_FUNCTION( "rdy_init" );

  /*
   * Initialise service RDY with state RDY_READY.
   *
   * default the init state has to be IDLE, but i think we do not need IDLE state
   */
  INIT_STATE (RDY, RDY_STANDBY);

  /* 
   * Initialise the timers T3312 and T3314 with default values
   */
  gmm_data->rdy.t3312_val = T3312_VALUE;
  gmm_data->rdy.t3314_val = T3314_VALUE;
  gmm_data->rdy.t3312_deactivated = FALSE;
  gmm_data->rdy.attempting_to_update = FALSE;
  for (timer=0;timer<TIMER_MAX;timer++)
  {
    gmm_data->rdy.timer_value[timer] = 0;
  }
  vsi_t_stop  ( GMM_handle, rdy_T3312);
  rdy_cgrlc_ready_timer_config_req (gmm_data->rdy.t3314_val);
  return;
} /* rdy_init() */

/*
+------------------------------------------------------------------------------
| Function    : rdy_get_timer
+------------------------------------------------------------------------------
| Description : this procedure sets the ready or rau timer value received
|               from the network
|
| Parameters  : timer     - the AIR message timer 
| returns:      timer_val - the timer_value in ms
|
+------------------------------------------------------------------------------
*/
GLOBAL ULONG rdy_get_timer ( T_rau_timer * timer ) 
{
  GMM_TRACE_FUNCTION ("rdy_get_timer()");

  switch ( timer->timer_unit )
  {
    case TIMER_2_SEC: 
      GMM_RETURN_ (timer->timer_value * 2 * SEC);
    case TIMER_1_MIN:
      GMM_RETURN_ (timer->timer_value * 60 * SEC);
    case TIMER_1_DH:
      GMM_RETURN_ (timer->timer_value * 6 * 60 * SEC);
    default:
      GMM_RETURN_ (timer->timer_value * 60 * SEC);
  }
} /* rdy_get_timer */

/*
+------------------------------------------------------------------------------
| Function    : rdy_start_t3312
+------------------------------------------------------------------------------
| Description : this procedure starts the timer t3312 if it is not deactivated
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_start_t3312 ( void ) 
{
  GMM_TRACE_FUNCTION ("rdy_start_t3312()");

  if( !gmm_data->rdy.t3312_deactivated
  &&  !gmm_data->rdy.attempting_to_update)
  {
    TRACE_1_INFO ("Periodic RAU will be started in %d minutes",
        (int)(gmm_data->rdy.t3312_val/(60000.0)));
    vsi_t_start ( GMM_handle ,  rdy_T3312, gmm_data->rdy.t3312_val );
  }
  GMM_RETURN;
} /* rdy_get_timer */
