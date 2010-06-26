/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdyp.c
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
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (RDY-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RDYP_C
#define GMM_RDYP_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */

#include "gmm_kerns.h"  /* to get signals */
#include "gmm_kernp.h"  /* to get timer functions from KERN */
#include "gmm_kernf.h"  /* to get functions from KERN */
#include "gmm_rdyf.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : rdy_cgrlc_standby_state_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_STANDBY_STATE_IND (TCS 2.1).
|
|               TIMEOUT for READY´TIMER
|
|          MSC: 3.17 Timer
|          MSC: 3.17.4 T3314 time-out
|
| Parameters  : *cgrlc_standby_state_ind - Ptr to primitive payload (TCS 2.1)
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_cgrlc_standby_state_ind (const T_CGRLC_STANDBY_STATE_IND *cgrlc_standby_state_ind)
{
  GMM_TRACE_FUNCTION( "rdy_cgrlc_standby_state_ind" );
  switch( GET_STATE( RDY ) )
  {
    default:
      /*
       * <R.GMM.READYTIM.M.020>
       */
      rdy_start_t3312();
      
      SET_STATE ( RDY, RDY_STANDBY );

      break;
    case RDY_DEACTIVATED:
    case RDY_STANDBY_TWICE:
      break;
   }
  
   PFREE(cgrlc_standby_state_ind);
   GMM_RETURN;
} /* rdy_cgrlc_standby_state_ind */


/*
+------------------------------------------------------------------------------
| Function    : rdy_cgrlc_trigger_ind (TCS 2.1)
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_TRIGGER_IND (TCS 2.1)
|               
|               Start of the READY TIMER
|
|               <R.GMM.READYTIM.M.009>
|
| Parameters  : *cgrlc_trigger_ind - Ptr to primitive payload (TCS 2.1)
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_cgrlc_trigger_ind ( T_CGRLC_TRIGGER_IND *cgrlc_trigger_ind )  /* TCS 2.1 */
{ 
  USHORT timer;
  BOOL   stop_timer=FALSE;
  GMM_TRACE_FUNCTION( "rdy_cgrlc_trigger_ind" );  /* TCS 2.1 */

  if (CGRLC_PRIM_TYPE_GMM==cgrlc_trigger_ind->prim_type) /* TCS 2.1 */
  {  
    for ( timer=0;timer<TIMER_MAX;timer++)
    {
      if (gmm_data->rdy.timer_value[timer]>0)
      {
        T_TIME value=0;
        if ( VSI_OK == vsi_t_status ( GMM_handle ,timer, &value )
        && value >0 )
          /*
           * react only if timer has not stopped
          */
        {
          TRACE_1_INFO ("value of Timer %d: %d", timer);
          switch (timer)
          {
            case kern_TPOWER_OFF:
              vsi_t_stop ( GMM_handle, kern_TPOWER_OFF);       
              kern_tpower_off();
              stop_timer=TRUE;
              break;
            case kern_TLOCAL_DETACH:
              vsi_t_stop ( GMM_handle, kern_TLOCAL_DETACH);       
              kern_tlocal_detach();
              stop_timer=TRUE;
              break;        
            default:
              TRACE_1_INFO ( "START: TIMER %d" ,timer);
              vsi_t_start ( GMM_handle , 
              timer, 
              gmm_data->rdy.timer_value[timer]);
              break;
          }        
          gmm_data->rdy.timer_value[timer]=0;
        }
      }
      else
      {
        gmm_data->rdy.timer_value[timer]=0;
      }
    }
  }

   /* 
   * As RR will inform MM about ongoing PS transfer using RR_ABORT_IND, no need
   * to notify MM using this mmgmm_trigger_req primitive
   */

  switch( GET_STATE( RDY ) )
  {
    case RDY_READY:
      vsi_t_stop (GMM_handle, rdy_T3312);
      if (stop_timer)
      {
        SET_STATE (RDY, RDY_STANDBY );
      }
      break;
    case RDY_STANDBY_TWICE:
      SET_STATE(RDY,RDY_STANDBY);
      {
        PALLOC (cgrlc_force_to_standby_req, CGRLC_FORCE_TO_STANDBY_REQ);
        PSEND ( hCommGRLC, cgrlc_force_to_standby_req );
      }
      break;
    case RDY_STANDBY:
    {  
      vsi_t_stop (GMM_handle, rdy_T3312);
      if (stop_timer)
      {
        SET_STATE (RDY, RDY_STANDBY );
      }
      else
      {
        SET_STATE ( RDY, RDY_READY );
      }
      break;
    }
    case RDY_DEACTIVATED:
      break;
    default:
      TRACE_ERROR( "CGRLC_TRIGGER_IND unexpected" ); /* TCS 2.1 */
      break;
  }
  
  PFREE(cgrlc_trigger_ind);
  GMM_RETURN;
} /* rdy_cgrlc_trigger_ind() */ /* TCS 2.1 */


/*
+------------------------------------------------------------------------------
| Function    : rdy_cgrlc_ready_state_ind (TCS 2.1)
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_READY_STATE_IND (TCS 2.1)
|               
|               Start of the READY TIMER
|
|               
|
| Parameters  : *cgrlc_ready_state_ind - Ptr to primitive payload (TCS 2.1)
|
+------------------------------------------------------------------------------
*/
GLOBAL void rdy_cgrlc_ready_state_ind ( const T_CGRLC_READY_STATE_IND *cgrlc_ready_state_ind )  /* TCS 2.1 */
{ 
  GMM_TRACE_FUNCTION( "rdy_cgrlc_ready_state_ind" );  /* TCS 2.1 */

  switch( GET_STATE( RDY ) )
  {
    case RDY_READY:
      vsi_t_stop (GMM_handle, rdy_T3312);
      break;
    case RDY_STANDBY:
      vsi_t_stop (GMM_handle, rdy_T3312);
      SET_STATE ( RDY, RDY_READY );
      break;
    case RDY_DEACTIVATED:
    case RDY_STANDBY_TWICE:
      break;
    default:
      TRACE_ERROR( "CGRLC_READY_STATE_IND unexpected" ); /* TCS 2.1 */
      break;
    
   }
   
   PFREE(cgrlc_ready_state_ind);
   GMM_RETURN;
} /* rdy_cgrlc_ready_state_ind() */ /* TCS 2.1 */
