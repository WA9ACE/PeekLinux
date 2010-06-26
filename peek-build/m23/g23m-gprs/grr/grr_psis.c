/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|  Purpose :  This module implements signal handler functions for service
|             PSI of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_PSIS_C
#define GRR_PSIS_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include <string.h>

#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grr.h"        /* to get the global entity definitions */

#include "grr_f.h"      /* To use global functions*/

#include "grr_psif.h"   /* */

#include "grr_ctrls.h"   /* */

#include "grr_psis.h"   /* */

#include <stdio.h>

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_si13_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_SI13_RECEIVED
|
| Parameters  : T_RRGRR_SI13_IND *rrgrr_si13_ind
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_si13_received ( T_RRGRR_GPRS_SI13_IND *rrgrr_si13_ind,
                                         BOOL                   ba_bcch_changed )
{
  MCAST(si13,D_SYS_INFO_13);
  UBYTE in_state = GET_STATE( PSI );
  UBYTE pb, ab_type;
  TRACE_ISIG( "sig_ctrl_psi_si13_received" );

#ifndef _TARGET_

    switch(psc_db->acq_type)
    {
      case NONE:
        /*
         * SI13 was not requested
         */
        TRACE_ERROR("SI13 not requested!");
        return;
    }
 
#endif
  
  pb = psc_db->g_pwr_par.pb;
  ab_type = psc_db->gprs_cell_opt.ab_type;
  
  switch( in_state )
  {
    case PSI_NULL:
      grr_data->psi.is_start_of_10_sec_allowed = TRUE;
      psi_start_60sec();
      /*
       * This is the first SI13 message received by the service PSI, except in
       * case of having SI13 without rest octets in the former SI13 message
       */
      switch(psi_process_si13(si13))
      {
        case SI13_OK:
          SET_STATE( PSI, PSI_BCCH_IDLE );
          TRACE_EVENT("NULL: BCCH present");
          psi_is_access_class_changed();
          sig_psi_ctrl_si13_processed();
          /*
           * Stop and restart 60 sec. timer
           */
          psi_stop_60sec ( TRUE );
          psi_start_30sec();
          /*
           * Check acq state of SI messages
           */
          switch(psi_acq_state_of_si(rrgrr_si13_ind->si_states))
          {
            case ACQ_COMP_OK:              
              /*
               * packet access enabled
               */
              psi_send_access_enable_if_needed();
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */

              /*stop the timer 3172 timer if it is already running in old cell*/
              if(grr_t_status( T3172_1 ) > 0)
              {
                vsi_t_stop(GRR_handle,T3172_1);
              }
              break;
            case ACQ_RUNNING:
              sig_psi_ctrl_read_si( COMPLETE_SI );
              psc_db->acq_type = COMPLETE;
              break;
            default:
              TRACE_ERROR("Unexpected acq state of SI in PSI_NULL");
              break;
          }
          break;/* switch SI13_OK */
        case SI13_REREAD: 
          /* 
           * Only in case of no rest octets
           * Do not stop 60 sec. timer. Request new SI13
           */
          psc_db->acq_type = PARTIAL;
          sig_psi_ctrl_read_si( UPDATE_SI13 );
          break;
        case SI13_COMPLETE_PSI:
          /*
           * SI13 contains PBCCH description. Start complete acq of PSI
           */
          SET_STATE( PSI, PSI_IDLE );
          TRACE_EVENT("PBCCH present");
          psi_update_data_to_request(INIT_NEEDED);
          sig_psi_ctrl_si13_processed();
          psi_complete_acq(COMPLETE);
          psi_stop_60sec ( TRUE );
          break;
        default:
          TRACE_ERROR("Unexpected SI13 return in PSI_NULL");
          break;
      }
      break;
    case PSI_BCCH_IDLE:
      /*
       * Reading periodical SI13 message or reread because of absence of SI13 rest octets
       * or partial acq or complete acq because of change field in the former SI13 message
       * or PCCCH was released and we reread SI13
       */
      grr_data->psi.is_start_of_10_sec_allowed = TRUE;
      switch(psi_process_si13(si13))
      {
        case SI13_OK:
          /*
           * Stop and restart 60 sec. timer
           */
          psi_stop_60sec ( TRUE );
          psi_stop_30sec ( TRUE );
          if(psi_is_access_class_changed())
          {
            /*
             * Implies the CTRL to send CELL_IND
             */
            sig_psi_ctrl_access_changed();
          }

          /*
           * Check acq state of SI messages
           */
          switch(psi_acq_state_of_si(rrgrr_si13_ind->si_states))
          {
            case ACQ_PERIOD_OK:              
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_PART_OK:
              psi_stop_10sec(); /* partial acquisition completed */
              /*
               * packet access enabled
               */
              psi_send_access_enable_if_needed();
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_COMP_OK:
              /*
               * packet access enabled
               */
              psi_send_access_enable_if_needed();
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_RUNNING:
              break;
            default:
              TRACE_ERROR("Unexpected acq state of SI in PSI_BCCH_IDLE");
              break;
          }
          sig_psi_ctrl_meas_param_valid_si13( ba_bcch_changed );
          break;/* SI13_OK */
        case SI13_REREAD: 
          /* Only in case of no rest octets
           * Do not stop 60 sec. timer. Request new SI13
           */
          psi_stop_30sec ( TRUE );
          psc_db->acq_type = PARTIAL;
          sig_psi_ctrl_read_si( UPDATE_SI13 );
          sig_psi_ctrl_meas_param_invalid_si13( );
          break;
        case SI13_PARTIAL_SI: 
          /*
           * Partial acq has to be started
           */
          psi_stop_30sec ( TRUE );
          sig_psi_ctrl_read_si(si13->si13_rest_oct.si13_info.si_cf);
          psc_db->acq_type = PARTIAL;
          psi_start_10sec();
          sig_psi_ctrl_meas_param_valid_si13( ba_bcch_changed );
          break;
        case SI13_COMPLETE_SI:
          psi_initiate_read_complete_si( );
          sig_psi_ctrl_meas_param_valid_si13( ba_bcch_changed );
          break;
        case SI13_COMPLETE_PSI:
          SET_STATE( PSI, PSI_NULL );
          /* BCCH reading is automatically stopped by RR/ALR */
          psi_initiate_pbcch_switching( PSI_DC_PBCCH_ESTABLISHED );
          break;
        default:
          TRACE_ERROR("Unexpected SI13 return in PSI_BCCH_IDLE");
          break;
      }
      break;
    case PSI_BCCH_TRANSFER:
      /*
       * Reread SI13 because of PBCCH release indicated in PSI1 
       * or change/release of PBCCH description indicated in PSI13 
       * message. Both PSI13 and PSI1 are sent on PACCH
       */
      /*
       * Read SI13. We probably were not able to read PSI13 in transfer mode within
       * 30 seconds, so we requested SI13.
       */
      grr_data->psi.is_start_of_10_sec_allowed = FALSE;
      switch(psi_process_si13(si13))
      {
        case SI13_OK:
          /*
           * Stop and restart 60 sec. timer
           * The MS may continue its operation on transfer mode
           */
          psi_stop_60sec ( TRUE );
          psi_stop_30sec ( TRUE );

          if(psi_is_access_class_changed())
          {
            /*
             * Implies the CTRL to send CELL_IND
             */
            sig_psi_ctrl_access_changed();
          }

          switch(psi_acq_state_of_si(rrgrr_si13_ind->si_states))
          {
            case ACQ_PERIOD_OK:              
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_PART_OK:
              psi_stop_10sec(); /* partial acquisition completed */
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_COMP_OK:
              /*
               * packet access enabled
               */
              psi_send_access_enable_if_needed();
              psc_db->acq_type = NONE; /* e.g. wait 30 sec for reread SI13*/
              /* BCCH reading is automatically stopped by RR/ALR */
              break;
            case ACQ_RUNNING:
              break;
            default:
              TRACE_ERROR("Unexpected acq state of SI in PSI_BCCH_TRANSFER");
              break;
          }

          if( ab_type NEQ psc_db->gprs_cell_opt.ab_type OR
              pb      NEQ psc_db->g_pwr_par.pb             )
          {
            grr_update_pacch();
          }

          sig_psi_ctrl_meas_param_valid_si13( ba_bcch_changed );
          break;
        case SI13_REREAD: 
          /* 
           * Only in case of no rest octets
           * Do not stop 60 sec. timer. Request new SI13
           */
          psi_stop_30sec ( TRUE );
          psc_db->acq_type = PARTIAL;
          sig_psi_ctrl_read_si( UPDATE_SI13 );
          sig_psi_ctrl_meas_param_invalid_si13( );
          break;

        case SI13_PARTIAL_SI: 
          /*
           * Partial acq has to be started
           */
          psi_stop_30sec ( TRUE );
          sig_psi_ctrl_read_si(si13->si13_rest_oct.si13_info.si_cf);
          psc_db->acq_type = PARTIAL;
          psi_start_10sec();
          sig_psi_ctrl_meas_param_valid_si13( ba_bcch_changed );
          TRACE_EVENT ("SI 13 with partial SI");
          break;
 
        case SI13_COMPLETE_SI:
          psc_db->acq_type =COMPLETE;
          sig_psi_ctrl_read_si( COMPLETE_SI);
          TRACE_EVENT ("SI 13 with complete SI");
          break;

        case SI13_COMPLETE_PSI:
          SET_STATE( PSI, PSI_NULL );
          /* BCCH reading is automatically stopped by RR/ALR */
          psi_initiate_pbcch_switching( PSI_DC_PBCCH_ESTABLISHED );
          break;
        default:
          TRACE_ERROR("SIG_CTRL_PSI_SI13_RECEIVED: unexpected return value of type T_SI13_RET");
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_PSI_SI13_RECEIVED unexpected" );
      break;
  }
} /* sig_ctrl_psi_si13_received() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_pim
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_PIM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_pim ( void )
{
  UBYTE state;
  TRACE_ISIG( "sig_ctrl_psi_pim" );

  state = GET_STATE( PSI );
  switch( state )
  {
    case PSI_NULL:
    case PSI_TRANSFER:
      SET_STATE( PSI, PSI_IDLE );
     /*lint -fallthrough*/
    case PSI_IDLE:
    /* Partial acquisition in this case is not taken care */
      if(psc_db->acq_type EQ PERIODICAL_PSI1_READING)
      {
        TRACE_EVENT("Trigger PSI1 : Packet->Idle");
        psi_receive_psi(READ_PSI1_IN_PSI1_REPEAT_PERIODS);
      } 
      break;
    case PSI_BCCH_TRANSFER:
      SET_STATE( PSI, PSI_BCCH_IDLE );
      
      /*lint -fallthrough*/

    case PSI_BCCH_IDLE:
      if(psc_db->acq_type EQ PERIODICAL_SI13_READING)
      {
        sig_psi_ctrl_read_si( UPDATE_SI13 );
      }
      break;
    default:
      TRACE_EVENT_P1("PSI_PIM PSI: %d",state);
      break;
  }
} /* sig_ctrl_psi_pim() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_pam
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_PAM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_pam ( void )
{
  UBYTE state;
  TRACE_ISIG( "sig_ctrl_psi_pam" );

  state = GET_STATE( PSI );
  switch( state )
  {
    case PSI_IDLE:
      /* PBCCH reading process is running, we have to stop it*/
      if (psc_db->acq_type EQ PERIODICAL_PSI1_READING) 
      {
        /* When entering Idle go for PSI1 acquisition
         * acq_type is changed to PERIODICAL_PSI1_READING */
        psi_stop_psi_reading(PERIODICAL_PSI1_READING);
      }
      else if (psc_db->acq_type NEQ NONE)
      {
        /* Limitation: partial acquisition of other PSIs not taken care */
        psi_stop_psi_reading(NONE);
      }
      break;
    case PSI_BCCH_IDLE:
    case PSI_BCCH_TRANSFER:
      /* BCCH reading is automatically stopped by RR/ALR */
      break;
    default:
      TRACE_EVENT_P1("PSI_PAM PSI: %d",state);
      break;
  }
} /* sig_ctrl_psi_pam() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_PTM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_ptm ( void )
{
  UBYTE state;

  TRACE_ISIG( "sig_ctrl_psi_ptm" );

  state = GET_STATE( PSI ) ;
  switch( state  )
  {
    case PSI_IDLE:
      SET_STATE( PSI, PSI_TRANSFER );
      /* acquisition has been running, so we have to stop this acq.*/      
      if (psc_db->acq_type EQ PERIODICAL_PSI1_READING) 
      {
        /* When entering Idle go for PSI1 acquisition
         * acq_type is changed to PERIODICAL_PSI1_READING */
        psi_stop_psi_reading(PERIODICAL_PSI1_READING);
      }
      else if (psc_db->acq_type NEQ NONE)
      {
        /* Limitation: partial acquisition of other PSIs not taken care */
        psi_stop_psi_reading(NONE);
      }
      
      if(psc_db->send_psi_status AND psc_db->psi1_params.psi_status_supported)
      {
        /* MS should acquire PSI messages; we have to send PACKET PSI STATUS message*/
        psi_send_psi_status();
      }
      /* PSI STATUS*/
      break;
    case PSI_NULL:
    case PSI_BCCH_IDLE:
      SET_STATE( PSI, PSI_BCCH_TRANSFER );
      if(psc_db->acq_type EQ PERIODICAL_SI13_READING )
      {
        /*  SI reading has been running and therefore it has to be restarted. */
        sig_psi_ctrl_read_si( UPDATE_SI13 );
      }
      break;
    default:
/*       TRACE_EVENT_P1( "PSI_PTM PSI: %d",state);*/
      break;
  }
} /* sig_ctrl_psi_ptm() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_suspend
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_SUSPEND
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_suspend ( void )
{
  TRACE_ISIG( "sig_ctrl_psi_suspend" );
  
  SET_STATE(PSI, PSI_SUSPENDED);

  if(psc_db->acq_type NEQ NONE) /* acquisition has been running, so we have to stop this acq.*/
  {
    psi_stop_psi_reading(NONE);
  }  
  psi_stop_timer();

} /* sig_ctrl_psi_suspend() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_resumpt
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_RESUMPT
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_resumpt ( void )
{
  TRACE_ISIG( "sig_ctrl_psi_resumpt" );

  switch( GET_STATE( PSI ) )
  {
    case PSI_SUSPENDED:
      if(grr_is_pbcch_present())
      {
        SET_STATE(PSI, PSI_IDLE);
      }
      else
      {
        SET_STATE(PSI, PSI_BCCH_IDLE);
      }
      /*
       * start timer 30, 60 sec. to be able to read PSI1 or SI13
       */
      psi_start_60sec();
      psi_start_30sec();
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_PSI_RESUMPT unexpected" );
      break;
  }
} /* sig_ctrl_psi_resumpt() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_STOP
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_stop ( void)
{
  TRACE_ISIG( "sig_ctrl_psi_stop" );

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER:
      SET_STATE( PSI, PSI_NULL );
      if(psc_db->acq_type NEQ NONE) /* acquisition has been running, so we have to stop this acq.*/
      {
        TRACE_EVENT("Acq. running: send MPHP_SCELL_PBCCH_STOP_REQ");
        psi_stop_psi_reading(NONE);
      }
      psi_stop_timer();
      psi_init_params();
      break;
    case PSI_SUSPENDED:
      SET_STATE( PSI, PSI_NULL );
      psi_stop_timer();
      psi_init_params();
      break;
    case PSI_BCCH_IDLE:
    case PSI_BCCH_TRANSFER:
      SET_STATE( PSI, PSI_NULL );
      psi_stop_timer();
      if(psc_db->acq_type NEQ COMPLETE) /* acquisition has been running, so we have to stop this acq.*/
      {
        TRACE_EVENT("Acq. running: send STOP SI READING");
        psc_db->acq_type = COMPLETE;
        /* BCCH reading is automatically stopped by RR/ALR */
      }
      psi_reset_si_entries();
      break;
    default:
      TRACE_EVENT( "PSI stopped/not running" );
      break;
  }
} /* sig_ctrl_psi_stop() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_sync_ok
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_SYNC_OK
|               PSI can start PBCCH reading process
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_sync_ok ( void)
{
  TRACE_ISIG( "sig_ctrl_psi_sync_ok" );
  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
        psi_complete_acq(COMPLETE);        
        break;
    default:
      TRACE_ERROR( "SIG_CTRL_PSI_SYNC_OK unexpected" );
      break;
  }
} /* sig_ctrl_psi_sync_ok() */

/*
+------------------------------------------------------------------------------
| Function    : SIG_CTRL_PSI_READ_FULL_PSI_IN_NEW_CELL
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_READ_FULL_PSI_IN_NEW_CELL
|               PSI can starts PBCCH reading process in the possible new cell
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_read_full_psi_in_new_cell ( void)
{
  TRACE_ISIG( "sig_ctrl_psi_read_full_psi_in_new_cell" );
  switch( GET_STATE( PSI ) )
  {
    case PSI_NULL:
      SET_STATE(PSI, PSI_IDLE);
      
      /*lint -fallthrough*/

    case PSI_IDLE:
    case PSI_TRANSFER:
    case PSI_SUSPENDED:
    case PSI_BCCH_IDLE:
    case PSI_BCCH_TRANSFER:

      psc_db->state_of_PSI[PSI13].state = RECEIPT_OK; /* we do not need to read PSI13 again*/
      /*
       * read PSI: update it with PBCCH desc
       */
      psi_update_data_to_request(INIT_NOT_NEEDED);

      psi_complete_acq(FULL_PSI_IN_NEW_CELL);
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_PSI_READ_FULL_PSI_IN_NEW_CELL unexpected" );
      break;
  }
} /* sig_ctrl_psi_read_full_psi_in_new_cell() */

/*
+------------------------------------------------------------------------------
| Function    : SIG_CTRL_PSI_REL_STATE
+------------------------------------------------------------------------------
| Description : Handles the internal signal sig_ctrl_psi_rel_state
|               Check whether we have to start PBCCH request or not
|
| Parameters  : T_RELEASE_STATE rel_state
|
+------------------------------------------------------------------------------
*/
GLOBAL  void sig_ctrl_psi_rel_state(T_RELEASE_STATE rel_state)
{
  TRACE_ISIG("sig_ctrl_psi_rel_state");

  if(rel_state EQ REL_PERFORMED)
  {
    grr_data->psi.is_pbcch_req_allowed = TRUE;
    if(grr_data->psi.is_pbcch_req_needed)
    {
      TRACE_EVENT("Release of TBF, stop of RR task, etc. performed.There is a waiting PBCCH req. We can start it now");
      grr_data->psi.is_pbcch_req_needed =FALSE;
      {
        PALLOC(mphp_scell_pbcch_req, MPHP_SCELL_PBCCH_REQ);
        psi_prepare_scell_pbcch_req(mphp_scell_pbcch_req, grr_data->psi.reading_type);
        PSEND(hCommL1,mphp_scell_pbcch_req);
      }
    }
  /*  else
    {      
      TRACE_EVENT("Release performed.There is NO waiting PBCCH req.");          
    }
    */
  }
  else
  {
    /*
     * If a PBCCH req was running L1 stops automatically the reading procedure
     * we have to set the grr_data->psi.is_pbcch_req_needed to TRUE to continue
     * after performing release
     *
     * if(psc_db->acq_type NEQ NONE)
     * {
     *   grr_data->psi.is_pbcch_req_needed = TRUE;
     * }
     *
     * TRACE_EVENT("We may have to stop running PBCCH request.PBCCH req. NOT allowed");
     */
    grr_data->psi.is_pbcch_req_allowed = FALSE;
  }

}


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_resumpt_cc
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_RESUMPT_CC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_resumpt_cc ( void )
{
  MCAST(si13,D_SYS_INFO_13);

  TRACE_ISIG( "sig_ctrl_psi_resumpt_cc" );
  /* because get_state(PSI) makes no difference of the psi status
   *between the old and new cell, it must be directly from the datadase */

  grr_set_pbcch( si13->si13_rest_oct.si13_info.v_pbcch_des );

  if(( grr_t_status( T3176 ) > 0 ) /* in case of packet cell change failure, SI13 should be processed */
    AND
    (psc_db->psi_assigned_data.state EQ PSI_SUSPENDED))
  {
    if (grr_is_pbcch_present())
    {
      psc_db->psi_assigned_data.state = PSI_IDLE;
      psi_start_60sec();
      psi_start_30sec();
    }
    else
    {
      psc_db->psi_assigned_data.state = PSI_BCCH_IDLE;
    }
  } 
} /* sig_ctrl_psi_resumpt_cc() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_1_ptm
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_1_ptm ( void )
{
  MCAST(psi1, PSI_1);

  TRACE_ISIG( "sig_gfp_psi_1_ptm" );


  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_stop_60sec(TRUE);
      psi_stop_30sec(TRUE);
      {
        UBYTE pb, ab_type;
        
        pb = psc_db->g_pwr_par.pb;
        ab_type = psc_db->gprs_cell_opt.ab_type;
        
        psi_handle_psi1(psi1);

        if(
            (ab_type NEQ psc_db->gprs_cell_opt.ab_type)
            OR
            (pb NEQ psc_db->g_pwr_par.pb)
          )
        {
          /*
           * update PACCH parameters
           */
          grr_update_pacch();
        }
      }
      break;
    case PSI_BCCH_TRANSFER:
      psi_handle_psi1(psi1);
      break;      

    default:
      TRACE_ERROR( "SIG_GFP_PSI_1_PTM unexpected" );
      break;
  }
} /* sig_gfp_psi_1_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_2_ptm 
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_2_ptm (void)
{
  MCAST(psi2, PSI_2);

  TRACE_ISIG( "sig_gfp_psi_2_ptm " );

  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi2(psi2);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_2_PTM  unexpected" );
      break;
  }
} /* sig_gfp_psi_2_ptm () */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3_ptm 
+------------------------------------------------------------------------------
| Description : Handles
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3_ptm (void)
{
  MCAST(psi3, PSI_3);

  TRACE_ISIG( "sig_gfp_psi_3_ptm " );

  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi3(psi3);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3_PTM  unexpected" );
      break;
  }
} /* sig_gfp_psi_3_ptm () */




/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3_bis_ptm 
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3_bis_ptm ( void )
{
  MCAST(psi3bis, PSI_3_BIS);

  TRACE_ISIG( "sig_gfp_psi_3_bis_ptm " );


  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi3bis(psi3bis);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3_BIS_PTM unexpected" );
      break;
  }
} /* sig_gfp_psi_3_bis_ptm () */


#if defined  (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3_ter_ptm 
+------------------------------------------------------------------------------
| Description : Handles  the internal signal SIG_GFP_PSI_3_TER_PTM
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3_ter_ptm ( void )
{
  MCAST(psi3ter, PSI_3_TER);

  TRACE_ISIG( "sig_gfp_psi_3_ter_ptm " );


  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi3ter(psi3ter);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3_TER_PTM unexpected" );
      break;
  }
} /* sig_gfp_psi_3_ter_ptm () */
#endif

/*
+------------------------------------------------------------------------------
| Function    : sig_gff_psi_4 
+------------------------------------------------------------------------------
| Description : Handles
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_4_ptm  ( void )
{
  MCAST(psi4, PSI_4);

  TRACE_ISIG( "sig_gfp_psi_4_ptm " );

  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi4(psi4);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_4_PTM  unexpected" );
      break;
  }
} /* sig_gfp_psi_4_ptm() */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_8_ptm
+------------------------------------------------------------------------------
| Description : Handles psi 8 messsage received on PACCH.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_8_ptm  ( void )
{
  MCAST(psi8, PSI_8);

  TRACE_ISIG( "sig_gfp_psi_8_ptm " );

  switch( GET_STATE( PSI ) )
  {
    case PSI_TRANSFER:
      psi_handle_psi8(psi8);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_8_PTM  unexpected" );
      break;
  }
} /* sig_gfp_psi_8_ptm() */
#endif

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_13_ptm
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_13_ptm(void)
{
  MCAST(psi13,PSI_13);

  UBYTE pb, ab_type;

  TRACE_ISIG( "sig_gfp_psi_13_ptm" );
  
  grr_data->psi.is_start_of_10_sec_allowed = TRUE;

  pb      = psc_db->g_pwr_par.pb;
  ab_type = psc_db->gprs_cell_opt.ab_type;

  switch( GET_STATE( PSI ) )
  {
    case PSI_BCCH_TRANSFER:
    case PSI_TRANSFER:
      switch(psi_process_psi13(psi13))
      {
        case PSI13_OK:
          psi_stop_60sec(TRUE);
          psi_stop_30sec(TRUE);
        
          if( ab_type NEQ psc_db->gprs_cell_opt.ab_type OR
              pb      NEQ psc_db->g_pwr_par.pb             )
          {
            grr_update_pacch();
          }
          sig_psi_ctrl_meas_param_valid_si13( FALSE );
          break;
        case PSI13_PARTIAL_SI:
          psi_stop_60sec(TRUE);
          psi_stop_30sec(TRUE);
          sig_psi_ctrl_read_si(psi13->si_change_ma);
          psc_db->acq_type = PARTIAL;
          psi_start_10sec();
          sig_psi_ctrl_meas_param_valid_si13( FALSE );
          break;
        case PSI13_COMPLETE_SI:
          psi_stop_60sec(TRUE);
          psi_stop_30sec(TRUE);
          psc_db->acq_type =COMPLETE;
          sig_psi_ctrl_read_si( COMPLETE_SI);
          psi_start_10sec();
          sig_psi_ctrl_meas_param_valid_si13( FALSE );
          TRACE_EVENT ("PSI 13 with complete SI");		
          //psi_initiate_read_complete_si( );
          //sig_psi_ctrl_meas_param_valid_si13( FALSE );
          break;
        case PSI13_COMPLETE_PSI:
          SET_STATE( PSI, PSI_IDLE );
          psi_initiate_read_complete_psi( INIT_NEEDED );
          break;
        case PSI13_PBCCH_RELEASED:
          SET_STATE( PSI, PSI_NULL );
          psi_initiate_pbcch_switching( PSI_DC_PBCCH_RELEASED );
          break;
        case PSI13_PBCCH_ESTABLISHED:
          SET_STATE( PSI, PSI_NULL );
          psi_initiate_pbcch_switching( PSI_DC_PBCCH_ESTABLISHED );
          break;
        default:
          TRACE_ERROR("SIG_GFP_PSI_13_PTM: unexpected return value of type T_PSI13_RET");
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_13_PTM unexpected" );
      break;
  }
} /* sig_gfp_psi_13_ptm() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_1
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_1 ( void )
{
  MCAST(psi1, PSI_1);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_1" );

  switch(psc_db->acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
      /*
       * Handle 60, 30, 10 sec. timer within psi_handle_psi1
       */
      grr_data->psi.is_start_of_10_sec_allowed = FALSE;
      psi_handle_psi1(psi1);
      break;
    case PSI_TRANSFER:
      /*
       * Handle 60, 30, 10 sec. timer within psi_handle_psi1
       */
      grr_data->psi.is_start_of_10_sec_allowed = TRUE;
      {
        UBYTE pb, ab_type;
        
        pb = psc_db->g_pwr_par.pb;
        ab_type = psc_db->gprs_cell_opt.ab_type;
        
        psi_handle_psi1(psi1);

        if(
            (ab_type NEQ psc_db->gprs_cell_opt.ab_type)
            OR
            (pb NEQ psc_db->g_pwr_par.pb)
          )
        {
          /*
           * update PACCH parameters
           */
          grr_update_pacch();
        }
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_1 unexpected" );
      break;
  }
  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }

} /* sig_gfp_psi_1() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_2
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_2 (UBYTE rel_pos_i)
{
  MCAST (psi2, PSI_2);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_2" );

  psi_store_rel_pos(grr_data->psi.psi2_pos, rel_pos_i, MAX_NR_OF_INSTANCES_OF_PSI2); /*MODIF*/

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI2 was not requested
       */

#ifdef _SIMULATION_
      TRACE_ERROR("PSI2 not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi2(psi2);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_2 unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }

} /* sig_gfp_psi_2() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3 (UBYTE rel_pos_i)
{
  MCAST (psi3, PSI_3);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_3" );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  psi_store_rel_pos(grr_data->psi.psi3_set_pos,
                    rel_pos_i,
                    MAX_NR_OF_INSTANCES_OF_PSI3TER+ 
                    MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3); /*MODIF*/
#else
  psi_store_rel_pos(grr_data->psi.psi3bis_pos, 
                    rel_pos_i, 
                    MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3); /*MODIF*/
#endif

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI3 was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI3 not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi3(psi3);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3 unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }

} /* sig_gfp_psi_3() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3_bis
+------------------------------------------------------------------------------
| Description : Handles
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3_bis (UBYTE rel_pos_i)
{
  MCAST(psi3bis, PSI_3_BIS);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_3_bis" );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  psi_store_rel_pos(grr_data->psi.psi3_set_pos,
                    rel_pos_i,
                    MAX_NR_OF_INSTANCES_OF_PSI3TER+ 
                    MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3); /*MODIF*/
#else
  psi_store_rel_pos(grr_data->psi.psi3bis_pos, 
                    rel_pos_i, 
                    MAX_NR_OF_INSTANCES_OF_PSI3BIS+MAX_NR_OF_INSTANCES_OF_PSI3); /*MODIF*/
#endif

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI3BIS was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI3BIS not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi3bis(psi3bis);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3_BIS unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }

} /* sig_gfp_psi_3_bis() */


#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_3_ter
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_PSI_3_TER
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_3_ter (UBYTE rel_pos_i)
{
  MCAST(psi3ter, PSI_3_TER);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_3_ter" );

  psi_store_rel_pos(grr_data->psi.psi3_set_pos, 
    rel_pos_i, 
    MAX_NR_OF_INSTANCES_OF_PSI3TER+MAX_NR_OF_INSTANCES_OF_PSI3BIS
    +MAX_NR_OF_INSTANCES_OF_PSI3); 
  
  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI3TER was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI3TER not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi3ter(psi3ter);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_3_TER unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }

} /* sig_gfp_psi_3_ter() */
#endif

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_4
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_4 ( UBYTE rel_pos_i)
{
  MCAST(psi4, PSI_4);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_4" );

  psi_store_rel_pos(grr_data->psi.psi4_pos, rel_pos_i, MAX_NR_OF_INSTANCES_OF_PSI4);

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI4 was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI4 not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi4(psi4);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_4 unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }
} /* sig_gfp_psi_4() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_5
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_5 ( UBYTE rel_pos_i)
{
  MCAST(psi5, PSI_5);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );
  
  TRACE_ISIG( "sig_gfp_psi_5" );

  psi_store_rel_pos(grr_data->psi.psi5_pos, rel_pos_i, MAX_NR_OF_INSTANCES_OF_PSI5); /*MODIF*/

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI5 was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI5 not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      switch(psi_process_psi5(psi5))
      {
        case PSI5_REREAD:
          psi_reread_psi5();
          /*
           * NC measurement parameters and Extended Measurement Parameters are invalid.
           * We have to inform CS and MEAS-Service
           */
          sig_psi_ctrl_meas_param_invalid_psi5();
          break;
        case PSI5_MEAS_PARAM_VALID:
          sig_psi_ctrl_meas_param_valid_psi5();
          
          /*lint -fallthrough*/

        case PSI5_OK:
          /*
           * check the acquisition state: it is only for having a complete set of information
           */
          switch(psi_check_acq_state())
          {
            case ACQ_RUNNING:
              /*
               * Nothing to do
               */
              break;
            case ACQ_PART_OK:
              psi_stop_psi_reading(NONE);
#ifdef _TARGET_
          TRACE_EVENT("PSI5->NEW_PCCCH");
          sig_psi_ctrl_new_pccch();
#endif
              break;
            case ACQ_COMP_OK:
            case ACQ_NPSI_OK:
              psi_stop_psi_reading(NONE);
              /*
               * Access to the network is allowed
               */
              psi_send_access_enable_if_needed();
              break;
            default:
              TRACE_ERROR("Unexpected acq_ret in PSI5");
              break;
          }/*switch check_acq*/
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_5 unexpected" );
      break;
  }
  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }
} /* sig_gfp_psi_5() */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_8
+------------------------------------------------------------------------------
| Description : Handles PSI8 message received on PBCCH. 
|
| Parameters  : UBYTE received_in: relative position to B0
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_8 ( UBYTE rel_pos_i)
{
  MCAST(psi8, PSI_8);

  ACQ_TYPE  old_acq_type = psc_db->acq_type;
  T_DB_MODE old_db_mode  = grr_get_db_mode( );

  TRACE_ISIG( "sig_gfp_psi_8" );

  psi_store_rel_pos(grr_data->psi.psi8_pos, rel_pos_i, MAX_NR_OF_INSTANCES_OF_PSI8);

  switch(psc_db->acq_type)
  {
    case NONE:
    case PERIODICAL_PSI1_READING:
      /*
       * PSI8 was not requested
       */
#ifdef _SIMULATION_
      TRACE_ERROR("PSI8 not requested!");
#endif /* #ifdef _SIMULATION_ */
      return;
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
  }

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER: /* In case of acq on PBCCH, if we are in transfer mode*/
      psi_handle_psi8(psi8);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_8 unexpected" );
      break;
  }

  switch(old_acq_type)
  {
    case FULL_PSI_IN_NEW_CELL:
      if( old_db_mode NEQ DB_MODE_CC_REQ )
      {
        grr_set_db_ptr( DB_MODE_SWAP );
      }
      break;
    default:
      break;
  }
} /* sig_gfp_psi_8() */
#endif


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_prach
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_prach (void )
{
  MCAST(prach, D_PRACH_PAR);

  TRACE_ISIG( "sig_gfp_psi_prach" );

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
      psi_process_prach(&prach->prach_ctrl_par);
      if(psi_is_access_class_changed())
      {
        /*
         * Implies the CTRL to send CELL_IND
         */
        sig_psi_ctrl_access_changed();
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_PRACH unexpected" );
      break;
  }

} /* sig_gfp_psi_prach() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_scell_pbcch_stop_con
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_PSI_SCELL_PBCCH_STOP_CON
|
| Parameters  : T_MPHP_SCELL_PBCCH_STOP_CON *mphp_scell_pbcch_stop_con
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_scell_pbcch_stop_con ( T_MPHP_SCELL_PBCCH_STOP_CON *mphp_scell_pbcch_stop_con) 
{ 
  TRACE_ISIG( "sig_gfp_psi_scell_pbcch_stop_con" );
  
} /* sig_gfp_psi_scell_pbcch_stop_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_ncell_pbcch_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_PSI_NCELL_PBCCH_IND
|
| Parameters  : T_MPHP_NCELL_PBCCH_IND *mphp_ncell_pbcch_ind
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_ncell_pbcch_ind ( T_MPHP_NCELL_PBCCH_IND *mphp_ncell_pbcch_ind) 
{ 
  TRACE_ISIG( "sig_gfp_psi_ncell_pbcch_ind" );
  /* Not needed */
  PFREE(mphp_ncell_pbcch_ind);
} /* sig_gfp_psi_ncell_pbcch_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_psi_save_persistence_level
+------------------------------------------------------------------------------
| Description : Handles
|
| Parameters  : T_pers_lev *pers_lev
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_psi_save_persistence_level( T_pers_lev *pers_lev )
{
  TRACE_ISIG( "sig_gfp_psi_save_persistence_level" );

  switch( GET_STATE( PSI ) )
  {
    case PSI_IDLE:
    case PSI_TRANSFER:
    case PSI_SUSPENDED:
    case PSI_BCCH_IDLE:
    case PSI_BCCH_TRANSFER:
      grr_save_persistence_level(pers_lev);
      break;
    default:
      TRACE_ERROR( "SIG_GFP_PSI_SAVE_PERSISTENCE_LEVEL unexpected" );
      break;
  }

} /* sig_gfp_psi_save_persistence_level() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_psi_access_disabled
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_PSI_ACCESS_DISABLED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_psi_access_disabled ( T_PSI_DISABLE_CAUSE dc )
{
  TRACE_ISIG( "sig_ctrl_psi_access_disabled" );

  switch( dc )
  {
    case PSI_DC_OTHER:
      psc_db->acq_type = COMPLETE;

      psi_start_10sec( );
      break;
    
    case PSI_DC_READ_PSI:
      psi_complete_acq(COMPLETE);
      psi_start_10sec();
      break;
  }
} /* sig_ctrl_psi_access_disabled() */
