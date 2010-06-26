/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rdys.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (RDY-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RDYS_C
#define GMM_RDYS_C
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

#include "gmm_rdyf.h"
#include "gmm_kerns.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_force_ie_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_FORCE_IE_REQ
|
| Parameters  : force_to_standby -  parameter from the AIR message
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_force_ie_req ( UBYTE force_to_standby, BOOL 
attach_complete )
{ 
  TRACE_ISIG( "sig_kern_rdy_force_ie_req" );
  
  switch( GET_STATE( RDY ) )
  {
    case RDY_READY:
    case RDY_STANDBY_TWICE:
    case RDY_STANDBY:
      if ( force_to_standby == STANDBY_YES )
      /*
       * force to standby
       */
      {
        /*
         * Start RAU STANDBY TIMER T3312
         * <R.GMM.RAUTIMER.M.005>, <R.GMM.READYTIM.M.012>
         */
        rdy_start_t3312();
        if (attach_complete)
        {
          SET_STATE(RDY,RDY_STANDBY_TWICE)
        }
        else
        {
          SET_STATE ( RDY, RDY_STANDBY );
        }
        {  
          /* Instead of sending GMMRR_STANDBY_REQ primitive send 
           * CGRLC_FORCE_TO_STANDBY_REQ primitive. GRLC will take 
           * care for GRR standby 
           */
          PALLOC (cgrlc_force_to_standby_req,CGRLC_FORCE_TO_STANDBY_REQ);
          PSEND ( hCommGRLC, cgrlc_force_to_standby_req );
        }
      } 
      else
      /* 
       * not force to standby
       */
      {
        /*
         * The state is kept
         * SET_STATE ( - )
         */
      } /* force to standby? */
      break;
    case RDY_DEACTIVATED:
      /* 
       * The state is not changed
       * SET_STATE ( - );
       */
      if ( force_to_standby == STANDBY_YES )
      /*
       * force to standby
       */
      {  
        /*
         * Allocate CGRLC_FORCE_TO_STANDBY_REQ primitive 
         */
        PALLOC (cgrlc_force_to_standby_req,CGRLC_FORCE_TO_STANDBY_REQ);
        PSEND ( hCommGRLC, cgrlc_force_to_standby_req );
      } 
      
      break;
    default:
      TRACE_ERROR( "SIG_KERN_RDY_FORCE_IE_REQ unexpected" );
      break;
  }
} /* sig_kern_rdy_force_ie_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_start_t3312_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_START_T3312_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_start_t3312_req ( void ) 
{ 
  TRACE_ISIG( "sig_kern_rdy_start_t3312_req" );
  
  switch( GET_STATE( RDY ) )
  {
    case RDY_DEACTIVATED:
      /* 
       * <R.GMM.RAUTIMER.M021>
       */
      rdy_start_t3312();
      break;
    default:
      break;
  }
} /* sig_kern_rdy_start_t3312() */
/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_cu_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_CU_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_cu_req ( void ) 
{ 
  TRACE_ISIG( "sig_kern_rdy_cu_req" );
  
  switch( GET_STATE( RDY ) )
  {
    case RDY_READY:
    case RDY_DEACTIVATED:
      /*
       * This message is used to initiated the initial cell update
       *
       *  <R.GMM.READYTIM.M.002>
       *  <R.GMM.READYTIM.M.003>
       *  <R.GMM.READYTIM.M.025>
       *  <R.GMM.READYTIM.M.026>
       */
      sig_rdy_kern_cu_ind();
      TRACE_EVENT("CU");
      vsi_o_ttrace(VSI_CALLER TC_USER4, "CELL UPDATE");
      break;
    default:
      /*
       * if the READY timer has expired  MS shall not perform 
       * the cell updating procedure when a new cell is selected
       *
       *  <R.GMM.READYTIM.M.005>
       */
  
      break;
  }
} /* sig_kern_rdy_cu_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_t3314_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_T3314_REQ
|               This procedure handels the negotiated ready timer comming
|               from the ATTACH_ACCEPT and RAU_ACCEPT message
|
|          MSC: 3.17 Timer
|          MSC: 3.17.6 READY timer behaviour
|
| Parameters  : v_ptmsi       - flag from AIR message if PTMSI was negotiated
|               v_ready_timer - flag from AIR message if  T3314 was negotiated
|               ready_timer   - ready_timer struct from AIR message with time 
|                               unit and value of the negotiated T3314
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_t3314_req ( BOOL v_ptmsi, BOOL v_ready_timer,
                                    T_ready_timer *p_ready_timer,
                                    T_rau_timer *rau_timer,
                                    BOOL attach_complete ) 
{ 
  TRACE_ISIG( "sig_kern_rdy_t3314_req" );
  
  if (TIMER_DEACT == rau_timer->timer_unit)
  {
    gmm_data->rdy.t3312_deactivated = TRUE;
  }
  else
  {
    gmm_data->rdy.t3312_deactivated = FALSE;
    gmm_data->rdy.t3312_val = rdy_get_timer ( rau_timer );  
  }

  if (!v_ready_timer)
  {
    return;
  }
  /*
   * <R.GMM.READYTIM.A.023>
   */
  if ( TIMER_DEACT == p_ready_timer->timer_unit )
  {
    rdy_cgrlc_ready_timer_config_req (CGRLC_DEACTIVATED) ;
    SET_STATE ( RDY, RDY_DEACTIVATED );
  }
  else /* if ( p_ready_timer->timer_unit != TIMER_DEACT ) */
  {
    /*  
     *  The value for  READY TIMER T3314 is set with data from AIR
     */
    ULONG ready_timer = rdy_get_timer ( (T_rau_timer *)p_ready_timer );
    /*
     * changed due t ericsson IOT. I hope that ANITE and R&S will accept this
     *
     * BOOL read_timer_changed = (ready_timer!=gmm_data->rdy.t3314_val);
     */
    /*  BOOL read_timer_negtiated = v_ready_timer; */
    
    gmm_data->rdy.t3314_val = ready_timer;
    rdy_cgrlc_ready_timer_config_req (gmm_data->rdy.t3314_val) ;    

    switch( GET_STATE( RDY ) )
    {
      /*
       *  LABEL READY_CONTINUE
       */
      case RDY_READY:         
      /*
       *  LABEL STANDBY_CONTINUE
       */   
      case RDY_STANDBY_TWICE:
      case RDY_STANDBY:
     
        if ( p_ready_timer->timer_value == 0 )
        {
          /*
           *  <R.GMM.READYTIM.A.024>
           */
          rdy_start_t3312();
          if (attach_complete)
          { 
            SET_STATE(RDY,RDY_STANDBY_TWICE)
          }
          else
          {
            SET_STATE ( RDY, RDY_STANDBY );
          }
          {
            PALLOC (cgrlc_force_to_standby_req, CGRLC_FORCE_TO_STANDBY_REQ);
            PSEND ( hCommGRLC, cgrlc_force_to_standby_req );
          }
        }
        else /* if ( p_ready_timer->timer_value != 0 ) */
        {
          /* 
           * v_ptmsi is an indicator for ready_timer negotiation
           * this decision is here, because the cu will be triggered in
           * service KERNEL by the ACCEPT message
           */
          if ( /*read_timer_negtiated &&*/ (GET_STATE( RDY ) == RDY_READY) && !v_ptmsi ) 
          {
            /*
             * This message is used to initiated the initial cell update
             *
             *  <R.GMM.READYTIM.M.025>
             *  <R.GMM.READYTIM.M.026>
             */
            /*
            PALLOC (llgmm_trigger_req, LLGMM_TRIGGER_REQ);
              llgmm_trigger_req->trigger_cause = LLGMM_TRICS_CELL_UPDATE;
              
            PSEND ( hCommLLC, llgmm_trigger_req );
            */
            sig_rdy_kern_cu_ind();
            TRACE_EVENT ("TRIGGER_REQ because of ready timer neg");
          }
        }
        break;
      case RDY_DEACTIVATED:
        /*
         * I think, that the new  state should be READY, because there is
         * no other way to leave the state DEACTIVATED
         *
         * <R.GMM.READYTIM.A.025>
         */
        SET_STATE ( RDY, RDY_READY );
        break;
      default:
        TRACE_ERROR( "SIG_KERN_RDY_T3314_REQ unexpected" );
        break;
    }
  } 
} /* sig_kern_rdy_t3314_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_start_t3302_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_START_T3302_REQ
|               This signal let the timer T3312 not be startet in state 
|                attampting to update
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_start_t3302_req ( void )
{ 
  TRACE_ISIG( "sig_kern_rdy_start_t3302_req" );
  gmm_data->rdy.attempting_to_update = TRUE;
} /* sig_kern_rdy_start_t3302_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_stop_t3302_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_STOP_T3302_REQ
|               This signal let the timer T3312 not be startet in state 
|                attampting to update
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_stop_t3302_req ( void )
{ 
  TRACE_ISIG( "sig_kern_rdy_stop_t3302_req" );
  gmm_data->rdy.attempting_to_update = FALSE;
} /* sig_kern_rdy_start_t3302_req */

/*
+------------------------------------------------------------------------------
| Function    : sig_kern_rdy_start_timer_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_KERN_RDY_START_TIMER_REQ
|               This signal ask rdy to start the passed timer with given value
|               on reception of GMMRR_TRIGGER_REQ
|
| Parameters  : UBYTE  timer - timer
|              ULONG  value - timeout value 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_rdy_start_timer_req ( UBYTE timer, ULONG value )
{ 
  TRACE_ISIG( "sig_kern_rdy_start_timer_req" );
  TRACE_2_INFO ("START TIMER %d: %dsec",timer, (ULONG) (value/1000) );
  gmm_data->rdy.timer_value[timer] = value;
  vsi_t_start ( GMM_handle , timer, value);
  return;
} /* sig_kern_rdy_start_timer_req */


