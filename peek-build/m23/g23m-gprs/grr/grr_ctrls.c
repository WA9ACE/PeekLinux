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
|             CTRL of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CTRLS_C
#define GRR_CTRLS_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */

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

#include "grr_f.h"      /* */

#include "grr_ctrlf.h"  /* */
#include "grr_ctrls.h"  /* */

#include "grr_psis.h"   /* */
#include "grr_pgs.h"    /* */
#include "grr_css.h"    /* */
#include "grr_meass.h"  /* to get the signals for interference measurements */
#include "grr_tcs.h"    /* */

#include <string.h>     /* for memcpy*/

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_set_pckt_mode
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_SET_PCKT_MODE
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_set_pckt_mode 
                              ( T_GLBL_PCKT_MODE final_pckt_mode, UBYTE param )
{
  TRACE_ISIG( "sig_tc_ctrl_set_pckt_mode" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
    case CTRL_IN_PROGRESS:
    case CTRL_WAIT_RSP_4_RR_CR:
    case CTRL_WAIT_FOR_CNF_OF_SUSPENSION:
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_GRR_NC_CCO:
    case CTRL_READY:
      ctrl_stop_rr_task_req( final_pckt_mode, param );
      break;
    default:
      TRACE_ERROR( "sig_tc_ctrl_set_pckt_mode unexpected" );
      break;
  }
} /* sig_tc_ctrl_set_pckt_mode() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_prepare_abnorm_rel_with_cr
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_TC_CTRL_PREPARE_ABNORM_REL_WITH_CR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_prepare_abnorm_rel_with_cr ( void )
{
  TRACE_ISIG( "sig_tc_ctrl_prepare_abnorm_rel_with_cr" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      ctrl_send_access_disable_if_needed
                         ( TC_DC_CR_ABNORMAL, sig_tc_ctrl_abnorm_rel_with_cr );
      break;

    default:
      TRACE_ERROR( "SIG_TC_CTRL_PREPARE_ABNORM_REL_WITH_CR unexpected" );
      break;
  }
} /* sig_tc_ctrl_prepare_abnorm_rel_with_cr() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_abnorm_rel_with_cr
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_ABNORM_REL_WITH_CR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_abnorm_rel_with_cr ( void )
{
  TRACE_ISIG( "sig_tc_ctrl_abnorm_rel_with_cr" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_ABNORMAL );
      break;

    default:
      TRACE_ERROR( "SIG_TC_CTRL_ABNORM_REL_WITH_CR unexpected" );
      break;
  }
} /* sig_tc_ctrl_abnorm_rel_with_cr() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_channel_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_CHANNEL_REQ  
|
|               SZML-CTRL/002
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_channel_req ( UBYTE channel_req)
{
  TRACE_ISIG( "sig_tc_ctrl_channel_req" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      {
        PALLOC(rrgrr_channel_req, RRGRR_CHANNEL_REQ);
        rrgrr_channel_req->req_data = channel_req;
        PSEND(hCommRR, rrgrr_channel_req);
      }
      break;
    default:
      TRACE_ERROR( "SIG_TC_CTRL_CHANNEL_REQ unexpected" );
      break;
  }
} /* sig_tc_ctrl_channel_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_t3172_running
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_T3172_RUNNING
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_t3172_running ( void )
{
  TRACE_ISIG( "sig_tc_ctrl_t3172_running" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      sig_ctrl_pg_t3172_run();
      break;
    default:
      TRACE_ERROR( "SIG_TC_CTRL_T3172_RUNNING unexpected" );
      break;
  }
} /* sig_tc_ctrl_t3172_running() */








/*
+------------------------------------------------------------------------------
| Function    : sig_pg_ctrl_downlink_transfer
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CTRL_DOWNLINK_TRANSFER
|
| Parameters  : ULONG pg_tlli
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_ctrl_downlink_transfer ( UBYTE page_id)
{
  TRACE_ISIG( "sig_pg_ctrl_downlink_transfer" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      {
        PALLOC(gmmrr_page_ind, GMMRR_PAGE_IND);
        gmmrr_page_ind->page_id = page_id;
        PSEND(hCommGMM,gmmrr_page_ind);
      }
      break;
    default:
      TRACE_ERROR( "SIG_PG_CTRL_DOWNLINK_TRANSFER unexpected" );
      break;
  }
} /* sig_pg_ctrl_downlink_transfer() */



/*
+------------------------------------------------------------------------------
| Function    : sig_pg_ctrl_rr_est_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CTRL_RR_EST_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_ctrl_rr_est_req ( PG_CHANNEL type)
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_ISIG( "sig_pg_ctrl_rr_est_req" );

  grr_data->ctrl.cs_page_channel = type;

  switch( state )
  {
    case CTRL_READY:
    case CTRL_IN_PROGRESS:
      /*
       * To pass CS page request to GMM
       */
      SET_STATE(CTRL, CTRL_WAIT_CS_PAGE_RES);

      ctrl_handle_gmmrr_cs_page_ind( state );
      break;
    default:
      TRACE_ERROR( "SIG_PG_CTRL_RR_EST_REQ unexpected" );
      break;
  }
} /* sig_pg_ctrl_rr_est_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_si13_processed
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_SI13_PROCESSED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_si13_processed ( void )
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_ISIG( "sig_psi_ctrl_si13_processed" );

  switch( state )
  {
    case CTRL_READY:
    case CTRL_READY_TO_PROGRESS:
    case CTRL_ACCESS_DISABLED:
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_AUTO_RR_CR: 
    case CTRL_RR_CR_IN_SUSP:
    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
      if( grr_is_pbcch_present( ) )
      {
        if( state EQ CTRL_READY )
        {
          SET_STATE( CTRL, CTRL_READY_TO_PROGRESS );
        }
        ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
      }
      else
      {
        ctrl_send_cell_reselection_req( CR_COMPLETE );
        if( state EQ CTRL_READY )
        {          
          ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );

          SET_STATE( CTRL, CTRL_IN_PROGRESS );
        }
        else if( state EQ CTRL_ACCESS_DISABLED     OR
                 state EQ CTRL_READY_TO_PROGRESS )

        {
          SET_STATE( CTRL, CTRL_IN_PROGRESS );
        }
      }
      break;

    default:
      TRACE_ERROR( "SIG_PSI_CTRL_SI13_PROCESSED unexpected" );
      break;
  }
} /* sig_psi_ctrl_si13_processed() */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_access_enabled
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_ACCESS_ENABLED
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_access_enabled ( void )
{
  BOOL  send_cr_complete = FALSE;
  UBYTE final            = FALSE;

  TRACE_ISIG( "sig_psi_ctrl_access_enabled" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_RR_CR_IN_SUSP:
      /*
       * enable cell after resume
       */
      SET_STATE(CTRL, CTRL_ACC_ENABLED_SUSP);
      if(grr_is_pbcch_present())
      {
        /*
         * to stop CCCH after resume
         */
        grr_data->ctrl.rr_state.monitoring_stopped = FALSE;
      }
      ctrl_send_cell_reselection_req( CR_COMPLETE );
      
      sig_ctrl_psi_suspend( );
      break;
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_READY_TO_PROGRESS:
      final = TRUE;
      
      /*lint -fallthrough*/

    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
    case CTRL_AUTO_RR_CR:
    case CTRL_ACCESS_DISABLED:
      send_cr_complete = TRUE;
      if(( GET_STATE( CTRL ) NEQ CTRL_RR_NC_CCO ) AND (GET_STATE( CTRL ) NEQ CTRL_GRR_NC_CCO))
      {
        SET_STATE( CTRL, CTRL_IN_PROGRESS );
      }

      /*lint -fallthrough*/

    case CTRL_IN_PROGRESS:
      ctrl_enable_serving_cell( final );

      if( send_cr_complete EQ TRUE )
      {
        ctrl_send_cell_reselection_req( CR_COMPLETE );
      }
      break;    

    default:
      TRACE_ERROR( "SIG_PSI_CTRL_ACCESS_ENABLED unexpected" );
      break;
  }
} /* sig_psi_ctrl_access_enabled() */



/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_access_disabled
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_ACCESS_DISABLED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_access_disabled ( T_PSI_DISABLE_CAUSE dc )
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_ISIG( "sig_psi_ctrl_access_disabled" );

  switch( state )
  {
    case CTRL_IN_PROGRESS:
      SET_STATE(CTRL, CTRL_ACCESS_DISABLED);
          
      /*lint -fallthrough*/

    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
      
      switch(dc)
      {
        case PSI_DC_PBCCH_RELEASED:
          TRACE_EVENT( "PBCCH switched off" );

          sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_SCELL );
          break;
        case PSI_DC_PBCCH_ESTABLISHED:
          TRACE_EVENT( "PBCCH switched on" );

          grr_data->ctrl.psi_tbf_rel_cause = CTRL_PTRC_PBCCH_ESTABLISHED;

          ctrl_stop_all_activities( CTRL_DC_PSI_MANAGEMENT, NULL );
          break;
         case PSI_DC_READ_PSI:
          grr_data->ctrl.psi_tbf_rel_cause = CTRL_PTRC_COMPLETE_PSI_READING;

          ctrl_stop_all_activities( CTRL_DC_PSI_MANAGEMENT, NULL );
          break;
         case PSI_DC_OTHER:
          grr_data->ctrl.psi_tbf_rel_cause = CTRL_PTRC_COMPLETE_SI_READING;

          ctrl_stop_all_activities( CTRL_DC_PSI_MANAGEMENT, NULL );
          break;
        default:
          break;
      }
      break;
    case CTRL_AUTO_RR_CR:
    case CTRL_RR_CR_IN_SUSP:
      /*
       * RR has to continue CR
       */
      ctrl_send_cell_reselection_req( CR_CONT );
      break;
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      SET_STATE_FAILED_CR( state );
      ctrl_cc_rejected( );
      break;  
    default:
      TRACE_ERROR( "SIG_PSI_CTRL_ACCESS_DISABLED unexpected" );
      break;
  }
} /* sig_psi_ctrl_access_disabled() */



/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_psi1or_psi13_receiption_failure
+------------------------------------------------------------------------------
| Description : Handles the internal signal 
|               SIG_PSI_CTRL_PSI1OR_PSI13_RECEIPTION_FAILURE
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_psi1or_psi13_receiption_failure ( void )
{
  TRACE_ISIG( "sig_psi_ctrl_psi1or_psi13_receiption_failure" );
  
  ctrl_failure_processing( FAIL_SIG_PSI1OR_PSI13_RECEIPTION_FAILURE );
} /* sig_psi_ctrl_psi1or_psi13_receiption_failure() */


/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_read_si
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_READ_SI: it sends a
|               request to RR to read some special SI message. This signal is used
|               in case of acquisition
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_read_si ( UBYTE si_to_read )
{
  TRACE_ISIG( "sig_psi_ctrl_read_si" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
    case CTRL_IN_PROGRESS:
      ctrl_start_monitoring_bcch( si_to_read );
      break;
    default:
      TRACE_ERROR( "SIG_PSI_CTRL_READ_SI unexpected" );
      break;
  }
} /* sig_psi_ctrl_read_si() */





/*
+------------------------------------------------------------------------------
| Function    : sig_meas_ctrl_meas_report
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MEAS_CTRL_MEAS_REPORT
|
| Parameters  : T_U_MEAS_REPORT *meas_report
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_meas_ctrl_meas_report ( T_U_MEAS_REPORT *meas_report)
{
  TRACE_ISIG( "sig_meas_ctrl_meas_report" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      ctrl_send_control_block( CGRLC_BLK_OWNER_MEAS, (void*)meas_report );
      break;
    default:
      sig_ctrl_meas_pmr_reject( );

      TRACE_ERROR( "SIG_MEAS_CTRL_MEAS_REPORT unexpected" );
      break;
  }
} /* sig_meas_ctrl_meas_report() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_ctrl_meas_report
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_CTRL_MEAS_REPORT
|
| Parameters  : T_U_MEAS_REPORT *meas_report
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_ctrl_meas_report ( T_U_MEAS_REPORT *meas_report)
{
  TRACE_ISIG( "sig_cs_ctrl_meas_report" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      ctrl_send_control_block( CGRLC_BLK_OWNER_CS, (void*)meas_report );
      break;
    default:
      sig_ctrl_cs_pmr_reject( );

      TRACE_ERROR( "SIG_CS_CTRL_MEAS_REPORT unexpected" );
      break;
  }
} /* sig_cs_ctrl_meas_report() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_ctrl_cancel_meas_report
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_CTRL_CANCEL_MEAS_REPORT
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_ctrl_cancel_meas_report ( void )
{
  TRACE_ISIG( "sig_cs_ctrl_cancel_meas_report" );

  ctrl_cancel_control_block( CGRLC_BLK_OWNER_CS );

} /* sig_cs_ctrl_cancel_meas_report() */


/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_access_barred
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_ACCESS_BARRED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_access_barred ( void )
{
  TRACE_ISIG( "sig_psi_ctrl_access_barred" );

  ctrl_failure_processing( FAIL_SIG_CTRL_ACCESS_BARRED );
}/* sig_psi_ctrl_access_barred*/

/*
+------------------------------------------------------------------------------
| Function    : sig_pg_ctrl_stop_mon_ccch
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CTRL_STOP_MON_CCCH: it stops the
|               CCCH monitoring process in RR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_ctrl_stop_mon_ccch(void)
{
  TRACE_ISIG( "sig_pg_ctrl_stop_mon_ccch" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      ctrl_stop_monitoring_ccch();
      break;
    default:
      TRACE_ERROR( "SIG_PG_CTRL_STOP_MON_CCCH unexpected" );
      break;
  }
}/* sig_pg_ctrl_stop_mon_ccch*/

/*
+------------------------------------------------------------------------------
| Function    : sig_pg_ctrl_start_mon_ccch
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CTRL_START_MON_CCCH: it starts the
|               CCCH monitoring process in RR
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_ctrl_start_mon_ccch(UBYTE pg_type)
{
  TRACE_ISIG( "sig_pg_ctrl_start_mon_ccch" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      switch(pg_type)
      {
        case PAG_MODE_PTM_NP:
        case PAG_MODE_PIM_NP:
          /*
           * use PTM_NP or PIM_NP
           */
          ctrl_start_monitoring_ccch(pg_type);
          break;
        default:
          if(grr_is_non_drx_period())
          {
            /*
             * RR continue with paging reorg
             */
            ctrl_start_monitoring_ccch(PAG_MODE_REORG);
          }
          else
          {
            /*
             * RR continue with default paging
             */
            ctrl_start_monitoring_ccch(pg_type);
          }
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_PG_CTRL_START_MON_CCCH unexpected" );
      break;
  }
}/* sig_pg_ctrl_start_mon_ccch*/


/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_suspend_cnf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_SUSPEND_CNF:
|               GRR is suspended
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_suspend_cnf(void)
{
  TRACE_ISIG( "sig_tc_ctrl_suspend_cnf" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_WAIT_FOR_CNF_OF_SUSPENSION:
      SET_STATE(CTRL, CTRL_SUSPENDED);
      ctrl_stop_rr_task_req( GLBL_PCKT_MODE_SUSP, TASK_STOP_DUMMY_VALUE );
      psc_db->is_access_enabled = FALSE; /* at this point , we do not have any GPRS access*/
      switch(grr_data->ctrl.after_suspension)
      {
        case ACTIVATE_RR:
          {
            /*  activate RR. We have to check whether we have non-GPRS access parameters or not.
             *  Non-GPRS parameters are present in case of PCCCH presence
             *  GRR has to inform RR whether it should send GPRS Suspension request or not
             */
            PALLOC(rrgrr_activate_req, RRGRR_ACTIVATE_REQ);

            /*  Suspension needed*/
            rrgrr_activate_req->susp_req = grr_data->ctrl.is_susp_needed;
            if(grr_is_pbcch_present() AND (psc_db->v_non_gprs_opt EQ TRUE))
            {
              /*  There is valid non-GPRS parameters present in the database, so we have to send
               *  it to RR to allow a fast access to the network
               */
              rrgrr_activate_req->non_gprs.v_non_gprs = TRUE;
              ctrl_copy_non_gprs_opt(&rrgrr_activate_req->non_gprs);
            }
            else
              rrgrr_activate_req->non_gprs.v_non_gprs = FALSE;
            /*
             * SZML-CTRL/004
             */
            rrgrr_activate_req->rac = psc_db->cell_info_for_gmm.cell_info.cell_env.rai.rac;

            PSEND(hCommRR, rrgrr_activate_req);
          }

          ctrl_send_gmmrr_suspend_cnf( );
          break;
        case SEND_RR_EST_REQ:
          /*  */
          ctrl_send_rr_est_req(grr_data->ctrl.is_susp_needed);
          break;
        case SEND_RR_EST_RSP:
          ctrl_send_rr_est_rsp(grr_data->ctrl.is_susp_needed,
                                          TRUE  /* CS_PAGE was accepted*/);

          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_TC_CTRL_SUSPEND_CNF unexpected" );
      break;
  }
}/* sig_tc_ctrl_suspend_cnf*/

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_dcch_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_DCCH_DATA_REQ
|
| Parameters  : rrgrr_data_req-pointer to primitive which includes the data
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_dcch_data_req(T_RRGRR_DATA_REQ * rrgrr_data_req_i )
{
  TRACE_ISIG( "sig_tc_ctrl_dcch_data_req" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      {
        PSEND(hCommRR,rrgrr_data_req_i);
      }
      break;
    default:
      TRACE_ERROR( "sig_tc_ctrl_dcch_data_req unexpected" );
      break;
  }
}/* sig_tc_ctrl_dcch_data_req */




/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_gprs_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_GPRS_DATA_REQ
|
| Parameters  : rrgrr_gprs_data_req-pointer to primitive which includes the data
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_gprs_data_req(T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req_i)
{
  TRACE_ISIG( "sig_tc_ctrl_gprs_data_req" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      {
        PSEND(hCommRR,rrgrr_gprs_data_req_i);
      }
      break;
    default:
      TRACE_ERROR( "sig_tc_ctrl_gprs_data_req unexpected" );
      break;
  }
}/* sig_tc_ctrl_gprs_data_req*/



/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_suspend_dedi_chan
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_SUSPEND_DEDICATED_CHANNEL
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_suspend_dedi_chan(void)
{
  TRACE_ISIG( "sig_tc_ctrl_suspend_dedi_chan" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      {
        PALLOC(rrgrr_suspend_dcch_req, RRGRR_SUSPEND_DCCH_REQ);
        PSEND(hCommRR, rrgrr_suspend_dcch_req);
      }
      break;
    default:
      TRACE_ERROR( "SIG_TC_CTRL_SUSPEND_DEDICATED_CHANNEL unexpected" );
      break;
  }
}/* sig_tc_ctrl_suspend_dedi_chan*/




/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_resume_dedi_chan
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_RESUME_DEDICATED_CHANNEL
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_resume_dedi_chan(void)
{
  TRACE_ISIG( "sig_tc_ctrl_resume_dedi_chan" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      ctrl_reinstall_old_scell_req( FALSE, RECONN_PROT_UNSPECIFIED );
      break;
    default:
      TRACE_ERROR( "SIG_TC_CTRL_RESUME_DEDICATED_CHANNEL unexpected" );
      break;
  }
}/* sig_tc_ctrl_resume_dedi_chan*/


/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_contention_ok
+------------------------------------------------------------------------------
| Description : Cell change ok
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_contention_ok(void)
{
  TRACE_ISIG( "sig_tc_ctrl_contention_ok" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_GRR_NC_CCO:
    case CTRL_IN_PROGRESS:
      if( grr_data->cc_running )
      {
        vsi_t_stop( GRR_handle, T3134 );

        grr_data->cc_running = FALSE;

        ctrl_install_new_scell( );

        {
          PALLOC( rrgrr_resumed_tbf_req, RRGRR_RESUMED_TBF_REQ );
          PSEND( hCommRR, rrgrr_resumed_tbf_req );
        }
      }

      if( grr_t_status( T3174 ) > 0 )
      {
        SET_STATE( CTRL, CTRL_IN_PROGRESS );
        ctrl_stop_T3174( );
        ctrl_install_new_scell( );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
        grr_init_nc_list(&pcr_db->nc_cw.list );
        grr_init_ba_bcch_nc_list(pcr_db);
#endif
      }
      break;
    default:
      TRACE_ERROR( "sig_tc_ctrl_contention_ok unexpected" );
      break;
  }
}/* sig_tc_ctrl_contention_ok()*/

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_contention_failure
+------------------------------------------------------------------------------
| Description : Cell reselection has failled because of different 
| of contention resolution sending by the MS in RLC and received uplink assigment
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_tc_ctrl_contention_failure()
{
  TRACE_ISIG( "sig_tc_ctrl_contention_failure" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_GRR_NC_CCO:
      ctrl_stop_T3174();  
      TRACE_EVENT("ctrl_cell_change_order:No response on target cell");
      grr_data->pcco_failure_cause   = 1;
      ctrl_pcco_failure();
      break;
    default:
      TRACE_ERROR( "sig_tc_contention_failure unexpected" );
      break;
  }
}  /*sig_tc_ctrl_contention_failure*/

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_access_reject_on_new_cell
+------------------------------------------------------------------------------
| Description : Handles the internal signal ACCESS_REJECT_ON_NEW_CELL
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_tc_ctrl_access_reject_on_new_cell(void)
{
  TRACE_ISIG( "sig_tc_ctrl_access_reject_on_new_cell" );

  ctrl_stop_T3174();
  ctrl_pcco_failure();

} /*sig_tc_ctrl_access_reject_on_new_cell*/



/*
+------------------------------------------------------------------------------
| Function    : sig_cs_ctrl_new_candidate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_CTRL_NEW_CANDIDATE
|               This signal indicates that the CS service has detected
|               a new cell candidate for a cell reselection
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_ctrl_new_candidate( T_CS_RESELECT_CAUSE reselect_cause )
{
  TRACE_ISIG( "sig_cs_ctrl_new_candidate" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_READY:
    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
    case CTRL_IN_PROGRESS:
      {
        SET_STATE( CTRL, CTRL_CR );

        if( reselect_cause EQ CS_RESELECT_CAUSE_CTRL_ABNORMAL )
        {
          ctrl_stop_all_activities( CTRL_DC_CR_MS_GRR_ABNORMAL, NULL );
        }
        else
        {
          ctrl_stop_all_activities( CTRL_DC_CR_MS_GRR_NORMAL, NULL );
        }
      }
      break;  
    case CTRL_CR_IN_SUSP:
    case CTRL_FAILED_CR_IN_SUSP:
      ctrl_stop_all_activities( CTRL_DC_OTHER, NULL );
      
      /*lint -fallthrough*/
    
    case CTRL_CR:
    case CTRL_FAILED_CR:
    case CTRL_GRR_NC_CCO:
      ctrl_handle_new_candidate( FALSE );
      break;
    default:
      TRACE_ERROR( "SIG_CS_CTRL_NEW_CANDIDATE unexpected" );
      break;
  }
}/* sig_cs_ctrl_new_candidate*/

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_ncell_param_valid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_NCELL_PARAM_VALID
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_ncell_param_valid ( void )
{
  TRACE_ISIG( "sig_psi_ctrl_ncell_param_valid" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS: 
#if defined (REL99) AND defined (TI_PS_FF_EMR)
      psc_db->nc_ms.ncmeas.list.number=0;
      psc_db->nc_ms.rfreq.number = 0; 
      /*This is called to reset the only Freq List from PMO as BA(GPRS) is changed.
        Spec 5.08 - sec 10.1.4.1
        The list is valid until an empty list is sent to the MS, there is a downlink 
        signalling failure or the MS selects a new cell or the BA(GPRS) that is modified 
        by the NC_FREQUENCY_LIST changes or the MS enters dedicated mode
  
        The individual parameters are valid until the RESET command is sent to the MS or 
        there is a downlink signalling failure or the MS goes to the Standby state or the
        MS enters dedicated mode*/
        /*NO BREAK */
#endif 
	  /*lint -fallthrough*/
    case CTRL_GRR_NC_CCO: 
      if( grr_data->is_pg_started )

      {
        sig_ctrl_cs_start();
      }
      break; 
    default:
      /* ignore */
      break; 
  }
} /* sig_psi_ctrl_ncell_param_valid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_ncell_param_invalid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_NCELL_PARAM_INVALID
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_ncell_param_invalid ( void )
{
  TRACE_ISIG( "sig_psi_ctrl_ncell_param_invalid" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      sig_ctrl_cs_stop(CS_DC_OTHER);
      break;
    default:
      /*ignore*/
      break;
  }
} /* sig_psi_ctrl_ncell_param_invalid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_access_changed
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_ACCESS_CHANGED
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_access_changed ( void  )
{
  TRACE_ISIG( "sig_psi_ctrl_access_changed" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      ctrl_send_cell_ind();
      break;
    default:
      TRACE_ERROR( "SIG_PSI_CTRL_ACCESS_CHANGED unexpected" );
      break;
  }
} /* sig_psi_ctrl_access_changed() */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_ncell_psi_read
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_NCELL_PSI_READ
|
|
| Parameters  : BOOL read_successfully
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_ncell_psi_read( BOOL read_successfully)
{
  TRACE_ISIG( "sig_psi_ctrl_ncell_psi_read" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      if(read_successfully)
      {
        /*
         * Inform RR and wait for positive indication of the cell reselection request
         */
        ctrl_send_cell_reselection_req( CR_COMPLETE );
      }
      else
      {
        ctrl_cc_rejected( );
      }
      break;
    default:
      TRACE_ERROR( "SIG_PSI_CTRL_NCELL_PSI_READ unexpected" );
      break;
  }
}/* sig_psi_ctrl_ncell_psi_read*/

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_int_list_invalid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_INT_LIST_INVALID
|
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_int_list_invalid(void)
{
  TRACE_ISIG( "sig_psi_ctrl_int_list_invalid" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_meas_int_list_invalid();
      break;
    default:
      /*ignore*/
      break;
  }
  
}/* sig_psi_ctrl_int_list_invalid*/
/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_int_list_valid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_INT_LIST_VALID
|
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_int_list_valid(void)
{
  TRACE_ISIG( "sig_psi_ctrl_int_list_valid" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      if(grr_data->is_pg_started)
        sig_ctrl_meas_int_list_valid();
      break;
    default:
      /*ignore*/
      break;
  }
}/* sig_psi_ctrl_int_list_valid*/

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_meas_param_invalid_psi5
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_MEAS_PARAM_INVALID_PSI5
|
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_meas_param_invalid_psi5(void)
{
  TRACE_ISIG( "sig_psi_ctrl_meas_param_invalid_psi5" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_meas_ext_invalid_psi5();
      sig_ctrl_cs_nc_param_invalid();
      break;
    default:
      /*ignore*/
      break;
  }

}/* sig_psi_ctrl_meas_param_invalid_psi5*/

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_meas_param_valid_psi5
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_MEAS_PARAM_VALID_PSI5
|
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_meas_param_valid_psi5(void)
{
  TRACE_ISIG( "sig_psi_ctrl_meas_param_valid_psi5" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      if(grr_data->is_pg_started)
      {
        sig_ctrl_meas_ext_valid_psi5();
        sig_ctrl_cs_nc_param_valid();
      }
      break;
    default:
      /*ignore*/
      break;
  }
}/* sig_psi_ctrl_meas_param_valid_psi5*/

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_meas_param_invalid_si13
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_MEAS_PARAM_INVALID_SI13
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_meas_param_invalid_si13( void )
{
  TRACE_ISIG( "sig_psi_ctrl_meas_param_invalid_si13" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_cs_nc_param_invalid( );
      break;

    default:
      /* do nothing */
      break;
  }
}/* sig_psi_ctrl_meas_param_invalid_si13 */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_meas_param_valid_si13
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_MEAS_PARAM_VALID_SI13
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_meas_param_valid_si13( BOOL ba_bcch_changed )
{
  TRACE_ISIG( "sig_psi_ctrl_meas_param_valid_si13" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_cs_nc_param_valid( );
      sig_ctrl_tc_enable_grlc  (CGRLC_QUEUE_MODE_DEFAULT, CGRLC_RA_DEFAULT );

      if( ba_bcch_changed EQ TRUE )
      {
        sig_ctrl_meas_ba_changed( );
        sig_ctrl_cs_ba_changed( );
      }
      break;

    default:
      /* do nothing */
      break;
  }
}/* sig_psi_ctrl_meas_param_valid_si13 */

/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_new_pccch
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PSI_CTRL_NEW_PCCCH
|
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_new_pccch(void)
{
  TRACE_ISIG( "sig_psi_ctrl_new_pccch" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      if(grr_data->is_pg_started)
      {
        TRACE_EVENT("Restart PCCCH: Config may have changed");
        sig_ctrl_pg_start();
      }
      break;
    default:
      TRACE_EVENT_P1( "NEW_PCCCH not handled in CTRL state: %d", grr_data->ctrl.state);
      break;
  }
}/* sig_psi_ctrl_new_pccch*/

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : sig_psi_ctrl_cbch_info_ind
+------------------------------------------------------------------------------
| Description : This function gives the RRGRR_CBCH_INFO ind to RR.
|               This would be called on success cell selection/reselection
|               or when PSIs are completely acquired or when there is change
|               in PSI2,PSI13 or PSI8 messages. 
|               This primitive is sent to RR only when the mobile is in R99
|               network.
|               The CBCH channel description present in this primitive is in
|               a form which could be used by MPH_* primitives in RR. 
|               The frequency list is NOT in TI L1 form. When RR passes this
|               information to ALR, it is converted to TI L1 FORMAT.
| Parameters  : None.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_psi_ctrl_cbch_info_ind()
{
  T_cbch *cbch_req = NULL;
  PALLOC(rrgrr_cbch_info_ind, RRGRR_CBCH_INFO_IND);      
  TRACE_ISIG( "sig_psi_ctrl_cbch_info_ind" );
  
  cbch_req = &(rrgrr_cbch_info_ind->cbch);
  memset (cbch_req, 0, sizeof (T_cbch));

  /* fill in cbch info from psc_db */
  if(psc_db->v_cbch_chan_desc)
  {
    cbch_req->stat = STAT_ACT;   
    cbch_req->ch = psc_db->cbch_chan_desc.chan_typ_tdma_offset;
    cbch_req->tn   = psc_db->cbch_chan_desc.tn;
    cbch_req->tsc = psc_db->cbch_chan_desc.freq_par.tsc;
    /* If frequency encoding has indirect encoding/direct encoding 1
     * /direct encoding 2, then hopping is used. Otherwise channel
     * description describes a non-hopping channel.
     * When none of the above three coding methods are used, arfcn choice
     * should be set to true.Ref 3GPP 04.60 Section 12.8 
     */
    cbch_req->h = (psc_db->cbch_chan_desc.freq_par.v_arfcn)?H_NO:H_FREQ;
    if(cbch_req->h)
    {
      /*
       * CBCH uses frequency hopping, then configure MAIO and HSN
       * and create a frequency hopping list from the cell allocation/
       * PSI13/direct encoding 2 scheme and the mobile allocation
       * stored in Frequency Parameters.
       */
      /* When hopping is allowed - hsn,maio and ma have to be filled. */
      ctrl_fill_cbch_hopping_params(cbch_req,&psc_db->cbch_chan_desc.freq_par);
    }
    else
    {
      /* non hopping channel */
      cbch_req->arfcn = psc_db->cbch_chan_desc.freq_par.arfcn;
    }
  }
  else
  {
    cbch_req->stat = STAT_INACT;
  }
  PSEND(hCommRR,rrgrr_cbch_info_ind);
} /* sig_psi_ctrl_cbch_info_ind */
#endif

/*
+------------------------------------------------------------------------------
| Function    : sig_cs_ctrl_no_more_candidate
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_CTRL_NO_MORE_CANDIDATE
|               This signal indicates that the RR has to start a complete cell
|               selection procedure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_ctrl_no_more_candidate( T_CS_RESELECT_CAUSE reselect_cause )
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_ISIG( "sig_cs_ctrl_no_more_candidate" );

  switch( state )
  {
    case CTRL_IN_PROGRESS:
      if( reselect_cause EQ CS_RESELECT_CAUSE_CTRL_ABNORMAL )
      {
        ctrl_tc_access_enable( sig_ctrl_tc_abnorm_rel_with_cr_failed );
      }
      else
      {
        SET_STATE( CTRL, CTRL_READY );

        ctrl_stop_all_activities( CTRL_DC_CR_MS_GRR_NORMAL, NULL );
      }
      break;

    case CTRL_CR:
    case CTRL_FAILED_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_FAILED_CR_IN_SUSP:

      switch( reselect_cause )
      {
        case CS_RESELECT_CAUSE_CTRL_ABNORMAL:
          SET_STATE( CTRL, CTRL_IN_PROGRESS );

          grr_set_db_ptr( DB_MODE_CC_REJ );
          ctrl_tc_access_enable( sig_ctrl_tc_abnorm_rel_with_cr_failed );
          sig_ctrl_psi_resumpt( );
          ctrl_enable_serving_cell( FALSE );
          break;
    
        default:
          SET_STATE( CTRL, CTRL_READY );

          ctrl_stop_all_activities( CTRL_DC_CR_MS_GRR_NORMAL, NULL );
          break;  
      }
      break;

    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
      {
        T_CTRL_CR_TYPE ctrl_cr_type;

        ctrl_cr_type = ( reselect_cause EQ CS_RESELECT_CAUSE_CS_NORMAL ?
                         CTRL_CR_TYPE_CS : CTRL_CR_TYPE_CONT             );
        
        ctrl_stop_all_activities( CTRL_DC_CR_MS_GRR_NORMAL, NULL );
        ctrl_handle_no_more_candidate( ctrl_cr_type, FALSE );
      }
      break;

    case CTRL_READY:
    case CTRL_AUTO_RR_CR:
    case CTRL_RR_CR_IN_SUSP:
      ctrl_send_cell_reselection_req( CR_CONT );
      break;

    default:
      TRACE_ERROR( "SIG_CS_CTRL_NO_MORE_CANDIDATE unexpected 2" );
      break;
  }
}/* sig_cs_ctrl_no_more_candidate*/

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_cr_started_rsp
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_CR_STARTED_RSP
|               This signal indicates that the service TC is ready for
|               cell reselection, e.g. it stopped running procedures, controlled 
|               the flow control etc.
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_cr_started_rsp( void )
{
  T_NC_MVAL *nc_mval;

  TRACE_ISIG( "sig_tc_ctrl_cr_started_rsp" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_WAIT_RSP_4_RR_CR:
      SET_STATE(CTRL, CTRL_AUTO_RR_CR);
      /*
       * Init PSI and psc_db parameters to be able to read/process new SI (PSI)
       */
      grr_prepare_db_for_new_cell();
      ctrl_send_cell_reselection_req( CR_COMPLETE );
      break;    
    case CTRL_GRR_NC_CCO:
    if(grr_t_status( T3176 ) > 0 )
      {       
        ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW, TRUE );
      }
      else
      {
        TRACE_ASSERT( grr_data->db.cr_cell NEQ NULL );

        nc_mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];

        switch( nc_mval->sync_info.sync.status )
        {
          case( STAT_SYNC_OK ):
            ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW, TRUE );

            TRACE_EVENT( "sig_tc_ctrl_cr_started_rsp: cell already known(sync ok!)" );
            break;

          case( STAT_SYNC_NONE ):
            ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW_NOT_SYNCED, TRUE );
            break;

          case( STAT_SYNC_FAILED ):
          default:
            /* stay in the serving cell */
            break;
        }
      }
      break;
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      ctrl_handle_new_candidate( TRUE );
      break;

    case CTRL_READY:
      ctrl_handle_no_more_candidate( CTRL_CR_TYPE_CS, TRUE );
      break;

    case CTRL_RR_CR_IN_SUSP:
      ctrl_send_cell_reselection_req( CR_COMPLETE );
      break;

    default:
      sig_tc_ctrl_tbf_release_cnf( );
      break;
  }
}/* sig_tc_ctrl_cr_started_rsp*/


/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_tbf_release_cnf
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_tbf_release_cnf ( void )
{
  TRACE_ISIG( "sig_tc_ctrl_tbf_release_cnf" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
      switch( grr_data->ctrl.psi_tbf_rel_cause )
      {
        case CTRL_PTRC_PBCCH_ESTABLISHED:
          ctrl_start_monitoring_ccch( PAG_MODE_DEFAULT );
          ctrl_start_monitoring_bcch( UPDATE_SI13 );
          ctrl_stop_rr_task_req( GLBL_PCKT_MODE_NULL, TASK_STOP_DUMMY_VALUE );

          grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_COMPLETE;
          break;

        case CTRL_PTRC_COMPLETE_SI_READING:
          ctrl_start_monitoring_ccch( PAG_MODE_DEFAULT );
          ctrl_start_monitoring_bcch( COMPLETE_SI );
          sig_ctrl_psi_access_disabled( PSI_DC_OTHER );
          break;

        case CTRL_PTRC_COMPLETE_PSI_READING:
          sig_ctrl_psi_access_disabled( PSI_DC_READ_PSI );
          break;
      }

      grr_data->ctrl.psi_tbf_rel_cause = CTRL_PTRC_NONE;
      break;

    default:
      TRACE_ERROR( "SIG_TC_CTRL_TBF_RELEASE_CNF unexpected" );
      break;
  }
}/* sig_tc_ctrl_tbf_release_cnf */



/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_rel_state
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_REL_STATE
|               This signal indicates that the service TC started or performed 
|               tbf release
| Parameters  : T_RELEASE_STATE rel_state
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_rel_state(T_RELEASE_STATE rel_state)
{
  TRACE_ISIG( "sig_tc_ctrl_rel_state" );

  sig_ctrl_psi_rel_state(rel_state);

}/* sig_tc_ctrl_rel_state*/

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_control_block_result
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_control_block_result
                                     ( T_BLK_OWNER blk_owner, BOOL is_success )
{
  TRACE_ISIG( "sig_tc_ctrl_control_block_result" );
  
  ctrl_send_control_block_result( blk_owner, is_success );

}/* sig_tc_ctrl_control_block_result */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_send_stop_task_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CTRL_STOP_TASK_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_send_stop_task_req( UBYTE v_stop_ccch  )
{
  TRACE_ISIG( "sig_tc_ctrl_send_stop_task_req" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      ctrl_send_rrgrr_stop_task_req( CTRL_TASK_BCCH, v_stop_ccch );
      break;
    default:
      sig_ctrl_tc_stop_task_cnf_received( );      
      TRACE_ERROR( "SIG_TC_CTRL_STOP_TASK_REQ unexpected" );
      break;
  }
}/* sig_tc_ctrl_send_stop_task_req */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_ctrl_cc_order
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_CTRL_CC_ORDER
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
#ifdef REL99
GLOBAL void sig_gfp_ctrl_cc_order(T_TIME time_to_poll )
#else
GLOBAL void sig_gfp_ctrl_cc_order( void )
#endif
{
  MCAST( d_cell_chan_order, D_CELL_CHAN_ORDER );
  TRACE_ISIG( "sig_gfp_ctrl_cc_order" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:

      if (grr_data->nc2_on)
      {
#ifdef REL99
        if(time_to_poll > 0)
        {
          vsi_t_start(GRR_handle, T_POLL_TIMER,time_to_poll);
          memcpy(&grr_data->ctrl.pcco, d_cell_chan_order, sizeof(T_D_CELL_CHAN_ORDER));
          grr_data->ctrl.poll_for_msg = GRR_PCCO_POLL;
        }
        else
#endif
        {
          TRACE_EVENT( "PACKET CELL CHANGE ORDER RECEIVED" );

          ctrl_cell_change_order
                  ( CCO_TYPE_GRR, (void *)d_cell_chan_order );
        }
      }
      else
      {
        TRACE_EVENT( "PACKET CELL CHANGE ORDER RECEIVED,but NC2 is deactivated" );
      }

      break;

    default:
      TRACE_ERROR( "SIG_GFP_CTRL_CC_ORDER unexpected" );
      break;
  }

} /* sig_gfp_ctrl_cc_order() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_ctrl_dsf_ind
+------------------------------------------------------------------------------
| Description : This signal indicates that a downlink signalling failure has 
|               occured, we have to reselect a new cell
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_ctrl_dsf_ind ( void )
{
  TRACE_ISIG( "sig_gfp_ctrl_dsf_ind" );
  
  /* A set of measurement reporting parameters (NETWORK_CONTROL_ORDER and 
   * NC_REPORTING_PERIOD(s)) is broadcast on PBCCH. The parameters may also 
   * be sent individually to an MS on PCCCH or PACCH, in which case it 
   * overrides the broadcast parameters. The individual parameters are valid 
   * until the RESET command is sent to the MS or there is a downlink 
   * signalling failure or the MS goes to the Standby state or the MS enters 
   * dedicated mode. */
  sig_ctrl_cs_reset_meas_rep_params ( );

  ctrl_failure_processing( FAIL_SIG_DSF_IND );

}/* sig_gfp_ctrl_dsf_ind*/

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : sig_cs_ctrl_enh_meas_report
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CS_CTRL_ENH_MEAS_REPORT
|
| Parameters  : T_U_ENHNC_MEAS_REPORT *enh_meas_report
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cs_ctrl_enh_meas_report ( T_U_ENHNC_MEAS_REPORT *enh_meas_report)
{
  TRACE_ISIG( "sig_cs_ctrl_enh_meas_report" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_GRR_NC_CCO:
      ctrl_send_control_block( CGRLC_BLK_OWNER_CS, (void*)enh_meas_report );
      break;
    default:
      TRACE_ERROR( "SIG_CS_CTRL_ENH_MEAS_REPORT unexpected" );
      break;
  }
} /* sig_cs_ctrl_enh_meas_report() */
#endif



/*
+------------------------------------------------------------------------------
| Function    : sig_tc_ctrl_test_mode_ind
+------------------------------------------------------------------------------
| Description :  
|                
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_ctrl_test_mode_ind(void)
{
  TRACE_EVENT("sig_tc_ctrl_test_mode_ind");
  if(grr_data->test_mode NEQ CGRLC_NO_TEST_MODE) 
  {
    ctrl_stop_monitoring_ccch();
  }

}
