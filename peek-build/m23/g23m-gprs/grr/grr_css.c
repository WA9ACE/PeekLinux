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
|             CS of entity GRR.
+----------------------------------------------------------------------------- 
*/ 
        
#ifndef GRR_CSS_C
#define GRR_CSS_C
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
#include "grr_f.h"
#include "grr_css.h"
#include "grr_csf.h"
#include "grr_meass.h"
#include "grr_pgs.h"
#include "grr_tcs.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/


EXTERN T_NC_DATA* nc_data;

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : cs_cell_change_order_ind
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : *d_cell_chan_order - Ptr to packet cell change order message
|
+------------------------------------------------------------------------------
*/
LOCAL void cs_cell_change_order_ind ( T_nc_meas_par  *nc_meas_par,
                                      UBYTE           v_nc_freq_list,
                                      T_nc_freq_list *nc_freq_list
#if defined (REL99) AND defined (TI_PS_FF_EMR)
                                      , UBYTE       v_enh_meas_param_pcco,  
                                      T_enh_meas_param_pcco *enh_meas_param_pcco
#endif
                                    )
 
 
{ 
  TRACE_FUNCTION( "cs_cell_change_order_ind" );

  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:

      /* process the CS main process */
      SET_STATE( CS, CS_CR_NETWORK );

      cs_process_cc_order( nc_meas_par, v_nc_freq_list, nc_freq_list
#if defined (REL99) AND defined (TI_PS_FF_EMR)
                          , v_enh_meas_param_pcco, enh_meas_param_pcco 
#endif
                         );



      if( grr_is_pbcch_present( ) EQ TRUE )
      {
        if( grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PTM )
        {
          cs_send_cr_meas_stop_req( );
        }
      }

      grr_data->cs_meas.packet_mode = PACKET_MODE_NULL;

      /*cs_init_nc_ref_list( &grr_data->db.nc_ref_lst );*/

      /* inform service MEAS about stop of cell re-selection measurements */
      sig_cs_meas_stop( );

      /* process the CS measurement process */
      cs_cancel_meas_report( );

      SET_STATE( CS_MEAS, CS_MEAS_NULL );

      cs_process_t3158( );

      TRACE_EVENT_P4( "NC Cell Re-Selection Parameter: %d %d %d %d",
                      cs_get_network_ctrl_order( FALSE ),
                      grr_data->db.nc_ref_lst.param->rep_per_i,
                      grr_data->db.nc_ref_lst.param->rep_per_t,
                      grr_data->db.nc_ref_lst.param->non_drx_per );
      break;
    
    default:
      TRACE_ERROR( "PACKET CELL CHANGE ORDER unexpected" );
      break;
  }   
}/* cs_cell_change_order_ind */

/*
+------------------------------------------------------------------------------
| Function    : cs_all_cells_with_cr_param
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
LOCAL BOOL cs_all_cells_with_cr_param ( void )
{ 
  UBYTE i;                              /* used for counting */
  BOOL  all_cells_with_cr_param = TRUE;
    
  TRACE_FUNCTION( "cs_all_cells_with_cr_param" );

  for( i = 0;
       i < grr_data->db.nc_ref_lst.number AND all_cells_with_cr_param EQ TRUE;
       i++ )
  {
    all_cells_with_cr_param = grr_data->db.nc_ref_lst.info[i]->v_cr_par;
  }

  return( all_cells_with_cr_param );
}/* cs_all_cells_with_cr_param */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_start
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_START
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_start ( void) 
{ 
  UBYTE state = GET_STATE( CS );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  UBYTE psi3_cm = grr_get_psi3_cm();
#endif

  TRACE_ISIG( "sig_ctrl_cs_start" );
  
  switch( state )
  {
    case CS_NULL:
      SET_STATE( CS, CS_IDLE );

      /*lint -fallthrough*/

    case CS_IDLE:

      /*lint -fallthrough*/

    case CS_CR_MOBILE:
    case CS_CR_NETWORK:

      grr_data->db.scell_info.cell_barred = FALSE;
      grr_data->cs.reselect_cause         = CS_RESELECT_CAUSE_CS_NORMAL;

#if defined (REL99) AND defined (TI_PS_FF_EMR)
      /* If PCCO is received with PSI3_CM and BA_IND, then NC_FREQ list is 
         used in the network directed cell, only if PSI3_CM and BA_IND matches
         with the ones received in PSIs or SIs in the new cell */
      if( state EQ CS_CR_NETWORK )
      {
        if(  ( grr_is_pbcch_present() AND (psc_db->nc_ms.psi3_cm NEQ NOT_SET) AND
             (psc_db->nc_ms.psi3_cm NEQ psi3_cm) )
                                      OR
             ( !grr_is_pbcch_present() AND (psc_db->nc_ms.ba_ind NEQ NOT_SET) AND
             (psc_db->nc_ms.ba_ind NEQ psc_db->ba_ind) )  )
        {
          psc_db->nc_ms.ncmeas.list.number = 0;
          psc_db->nc_ms.rfreq.number = 0;
        }
        psc_db->nc_ms.psi3_cm = NOT_SET;
        psc_db->nc_ms.ba_ind  = NOT_SET;
      }
#endif


      if( grr_is_pbcch_present( ) )
      {
        grr_data->cs.is_upd_ncell_stat_needed = TRUE;

        cs_build_nc_ref_list( psc_db, FALSE );
        cs_reset_nc_change_mark( psc_db );
        cs_send_cr_meas_req( CS_DELETE_MEAS_RSLT );
        sig_cs_meas_start( );
      }
      else
      {
        cs_init_nc_mval_lst( );
        cs_build_nc_ref_list( psc_db, FALSE );
        cs_reset_nc_change_mark( psc_db );
        
        if( state EQ CS_CR_NETWORK )
        {
          cs_send_update_ba_req ( psc_db );
        }
      }
      
      cs_process_t3158( );

      TRACE_EVENT_P4( "NC Cell Re-Selection Parameter: %d %d %d %d",
                      cs_get_network_ctrl_order( FALSE ),
                      grr_data->db.nc_ref_lst.param->rep_per_i,
                      grr_data->db.nc_ref_lst.param->rep_per_t,
                      grr_data->db.nc_ref_lst.param->non_drx_per );
      break;

    default:
      /* do nothing */
      break;
  }
} /* sig_ctrl_cs_start() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_STOP
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_stop ( T_CS_DC_STATUS dc_stat ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_stop" );
  
  switch(dc_stat)
  {
    case CS_DC_DEDICATED:
      cs_stop_t3158( );
      cs_reset_all_rxlev_results( );
      /* 
       * Stop rx_lev averaging when NC=1 or NC=2 in ALR 
       */
      if(  grr_is_pbcch_present( ) EQ FALSE      AND
        grr_data->cs_meas.nc_meas_cause NEQ NC_STOP_MEAS   )
      {
        cs_send_meas_rep_req ( NC_STOP_MEAS ); 
      }
      break;
    case CS_DC_OTHER:
    default:
      /* do nothing */
      break;
  } 

  switch( GET_STATE( CS ) )
  {
    case CS_NULL:
      /* do nothing */
      break;

    case CS_IDLE:
      SET_STATE( CS, CS_NULL );

      /*lint -fallthrough*/

    case CS_CR_NETWORK:
      cs_stop_t_reselect( );

      /*lint -fallthrough*/

    default:
      cs_cancel_meas_report( );
      grr_data->cs.nc_sync_rep_pd = 0;

      SET_STATE( CS_MEAS, CS_MEAS_NULL );
  
      if( GET_STATE( CS )               NEQ CS_CR_MOBILE    AND
          grr_is_pbcch_present( )       EQ  TRUE            AND 
          grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PTM     )
      {
        cs_send_cr_meas_stop_req( );
      }

      grr_data->cs_meas.packet_mode = PACKET_MODE_NULL;

      if( grr_t_status( T3158) NEQ 0 )
      {
         vsi_t_stop( GRR_handle, T3158);
         TRACE_EVENT( "Timer T3158 stopped" );
      }

      /*cs_init_nc_ref_list( &grr_data->db.nc_ref_lst );*/

      /* inform service MEAS about stop of cell re-selection measurements */
      sig_cs_meas_stop( );
      break;
  }
} /* sig_ctrl_cs_stop() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_check_bsic_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_CHECK_BSIC_IND
|
| Parameters  : rrgrr_check_bsic_ind - pointer to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_check_bsic_ind 
                               ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind ) 
{
  TRACE_ISIG( "sig_ctrl_cs_check_bsic_ind" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_MOBILE:
    case CS_CR_NETWORK:
      cs_compare_bsic( rrgrr_check_bsic_ind );
     break;

    default:
      TRACE_ERROR( "SIG_CTRL_CS_CHECK_BSIC_IND unexpected" );
      break;
  }
} /* sig_ctrl_cs_check_bsic_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_cc_result
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_CC_RESULT
|
| Parameters  : cc_status - if cc_status is true then cell reselection was 
|                           successful performed. Otherwise cell reselection is 
|                           failed.
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_cc_result ( BOOL cc_status ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_cc_result" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_CR_MOBILE:
      if( cc_status )
      {
        /*
         * Cell Reselection with the given candidate was successfull
         */
        SET_STATE( CS, CS_NULL );

        cs_stop_t_reselect( );
      }
      else
      {
        /*
         * Cell Reselection with the given candidate was NOT successfull
         */
        SET_STATE( CS, CS_IDLE );

        cs_cr_decision( CRDM_CR_CONT );
      }
      break;

    case CS_CR_NETWORK:
      /*
       * Timer T_RESLECT should be stopped during processing of a
       * Packet Cell Change Order. Timer T3174 is used instead.
       */
      cs_stop_t_reselect( );

      if( cc_status )
      {
        /*
         * network controlled cell re-selection was successfull, so we
         * stay in the already selected cell and just go to idle mode,
         * cell re-slection measurements are already requested so no
         * further action is required.
         */
        SET_STATE( CS, CS_IDLE );
      }
      else
      {
        /*
         * network controlled cell re-selection was unsuccessfull, so
         * we are back in the old cell and start of cell re-selection
         * measurements will be requested immediately.
         */
        SET_STATE( CS, CS_NULL );
      }
      break;

    default:
      TRACE_EVENT( "SIG_CTRL_CS_CC_RESULT unexpected" );
      break;
  }
} /* sig_ctrl_cs_cc_result() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_gmm_state 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_GMM_STATE 
|               If this signal is received GMM has changed its state.
|
| Parameters  : state
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_gmm_state (T_GMM_STATES state ) 
{ 
  T_NC_ORDER ctrl_order;
    
  TRACE_ISIG( "sig_ctrl_cs_gmm_state " );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_MOBILE:
    case CS_CR_NETWORK:

      if( state EQ READY_STATE )
      {
        grr_data->cs.gmm_state = state;
        ctrl_order             = cs_get_network_ctrl_order( TRUE );
      }
      else
      {
        ctrl_order             = cs_get_network_ctrl_order( TRUE );
        /* A set of measurement reporting parameters (NETWORK_CONTROL_ORDER and 
         * NC_REPORTING_PERIOD(s)) is broadcast on PBCCH. The parameters may also 
         * be sent individually to an MS on PCCCH or PACCH, in which case it 
         * overrides the broadcast parameters. The individual parameters are valid 
         * until the RESET command is sent to the MS or there is a downlink 
         * signalling failure or the MS goes to the Standby state or the MS enters 
         * dedicated mode. */
        cs_reset_meas_rep_params (psc_db);
        grr_data->cs.gmm_state = state;
      }

      if( grr_is_pbcch_present( )       EQ TRUE   AND 
          ctrl_order                    EQ NC_NC2 AND
          cs_all_cells_with_cr_param( ) EQ FALSE      )
      {
        cs_send_cr_meas_req( CS_DELETE_MEAS_RSLT );
      }

      if( cs_is_meas_reporting( ) EQ FALSE )
      {
        cs_build_nc_ref_list( psc_db, FALSE );
        /* if we already sent a measurement report to L1 then GRR should wait untill 
           mphp_single_block_con comes from the L1, GRR should not send stop_single_block_req
        cs_cancel_meas_report( ); */
      }
     
      cs_process_t3158( );
      break;

    default:
      grr_data->cs.gmm_state = state;
      break;
  }
} /* sig_ctrl_cs_gmm_state () */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_reselect 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_RESELECT 
|
| Parameters  : state
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_reselect ( T_CS_RESELECT_CAUSE reselect_cause ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_reselect" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_NULL:
    case CS_IDLE:
      grr_data->db.scell_info.cell_barred = TRUE;
      grr_data->cs.reselect_cause         = reselect_cause;

      cs_cr_decision( CRDM_CR_CTRL );
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_CS_RESELECT unexpected" );
      break;
  }
} /* sig_ctrl_cs_reselect () */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_PTM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_ptm ( void )
{ 
  TRACE_ISIG( "sig_ctrl_cs_ptm" );
  
  if( grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PTM )
  {
    sig_ctrl_tc_delete_cs_msg ();
    switch( GET_STATE( CS ) )
    {
      case CS_NULL:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PTM;
        break;

      case CS_IDLE:
      case CS_CR_MOBILE:
      case CS_CR_NETWORK:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PTM;

        /*
         * reset the update condition in GRR
         */
        if( !grr_is_pbcch_present( ) )
        {
          cs_send_cr_meas_stop_req( );
        }

        cs_process_t3158( );
        break;

      default:
        TRACE_ERROR( "SIG_CTRL_CS_PTM unexpected" );
        break;
    }
  }

#ifdef _SIMULATION_

  else
  {
    TRACE_EVENT( "Service CS is already in PTM" );
  }

#endif /* #ifdef _SIMULATION_ */

} /* sig_ctrl_cs_ptm() */

 
/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_leave_to_pam 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_LEAVE_TO_PAM.
|               If this signal is received GRR starts the packet access
|               procedure.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_leave_to_pam ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_leave_to_pam" );

  
  if( grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PAM AND 
      grr_data->cs_meas.packet_mode NEQ PACKET_MODE_PTM    )
  {
    switch( GET_STATE( CS ) )
    {
      case CS_NULL:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PAM;
        break;
  
      case CS_IDLE:
      case CS_CR_MOBILE:
      case CS_CR_NETWORK:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PAM;

        /*
         * reset the update condition in GRR
         */
        if( !grr_is_pbcch_present( ) )
        {
          cs_send_cr_meas_stop_req( );
        }
        break;

      default:
        TRACE_ERROR( "SIG_CTRL_CS_LEAVE_TO_PAM unexpected" );
        break;
    }
  }

#ifdef _SIMULATION_

  else
  {
    TRACE_EVENT( "Service CS is already in PAM" );
  }

#endif /* #ifdef _SIMULATION_ */

} /* sig_ctrl_cs_leave_to_pam () */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_enter_pam 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_ENTER_PAM.
|               If this signal is received GRR starts the packet access
|               procedure.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_enter_pam ( void ) 
{ 
  T_PACKET_MODE packet_mode = grr_data->cs_meas.packet_mode;

  TRACE_ISIG( "sig_ctrl_cs_enter_pam" );
  
  if( packet_mode EQ PACKET_MODE_PTM )
  {
    switch( GET_STATE( CS ) )
    {
      case CS_NULL:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PAM;
        break;

      case CS_IDLE:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PAM;

        if( grr_is_pbcch_present( ) EQ TRUE )
        {
          cs_send_cr_meas_req( CS_KEEP_MEAS_RSLT );
        }

        cs_process_t3158( );
        break;

      case CS_CR_NETWORK:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PAM;

        cs_process_t3158( );
        break;

      default:
        TRACE_ERROR( "SIG_CTRL_CS_ENTER_PAM unexpected" );
        break;
    }
  }

#ifdef _SIMULATION_

  else
  {
    TRACE_EVENT( "Service CS is already in PAM" );
  }

#endif /* #ifdef _SIMULATION_ */

} /* sig_ctrl_cs_enter_pam () */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_pim 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_PIM 
|               If this signal is received GRR returns in packet idle mode. 
|               Cell reselection measurements has to restarted in idle mode.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_pim ( void )
{ 
  T_PACKET_MODE packet_mode = grr_data->cs_meas.packet_mode;

  TRACE_ISIG( "sig_ctrl_cs_pim" );
  
  if( packet_mode NEQ PACKET_MODE_PIM )
  {
    switch( GET_STATE( CS ) )
    {
      case CS_NULL:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PIM;
        break;

      case CS_IDLE:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PIM;

        if( grr_is_pbcch_present( ) EQ TRUE            AND
            packet_mode             EQ PACKET_MODE_PTM     )
        {
          cs_send_cr_meas_req( CS_KEEP_MEAS_RSLT );
        }

        cs_process_t3158( );
        break;

      case CS_CR_NETWORK:
        grr_data->cs_meas.packet_mode = PACKET_MODE_PIM;

        cs_process_t3158( );
        break;

      default:
        TRACE_ERROR( "SIG_CTRL_CS_PIM unexpected" );
        break;
    }
  }

#ifdef _SIMULATION_

  else
  {
    TRACE_EVENT( "Service CS is already in PIM" );
  }

#endif /* #ifdef _SIMULATION_ */

} /* sig_ctrl_cs_pim () */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_meas_rep_cnf 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_MEAS_REP_CNF
|
| Parameters  : rrgrr_meas_rep_cnf - Pointer to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_meas_rep_cnf (T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf)
{ 

#if defined (REL99) AND defined (TI_PS_FF_EMR)
BOOL is_enh_meas = cs_is_enh_meas_reporting();
#endif

  TRACE_ISIG( "sig_ctrl_cs_meas_rep_cnf" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
      switch( GET_STATE( CS_MEAS ) )
      {
        case( CS_MEAS_REP_REQ ):
          cs_store_meas_rep_cnf( rrgrr_meas_rep_cnf );
          cs_build_strongest( );

          SET_STATE( CS_MEAS, CS_MEAS_PMR_SENDING );
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          if( ((is_enh_meas EQ TRUE) AND (cs_send_enh_meas_rpt() EQ FALSE))
              OR
              ((is_enh_meas EQ FALSE) AND (cs_send_meas_rpt(TRUE) EQ FALSE)) )
#else
          if(cs_send_meas_rpt( TRUE ) EQ FALSE )
#endif
          {
            /*
             * The additional state check is necessary because signals might
             * be send during the processing of the function cs_send_meas_rpt.
             */
            if( GET_STATE( CS_MEAS ) EQ CS_MEAS_PMR_SENDING )
            {
              SET_STATE( CS_MEAS, CS_MEAS_NULL );
            /*
              cs_process_t3158( );
             */
            }
          }
          break;

        default:
          TRACE_ERROR( "SIG_CTRL_CS_MEAS_REP_CNF unexpected 1" );
          break;
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_CS_MEAS_REP_CNF unexpected 2" );
      break;
  }
} /* sig_ctrl_cs_meas_rep_cnf () */


/*
+------------------------------------------------------------------------------
| Function    : sig_meas_cs_meas_order
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MEAS_CS_MEAS_ORDER
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_meas_cs_meas_order ( T_NC_ORDER prev_ctrl_order )
{ 
  T_NC_ORDER ctrl_order;
  T_NC_ORDER applied_ctrl_order;
  T_NC_ORDER prev_applied_ctrl_order;

  TRACE_ISIG( "sig_meas_cs_meas_order" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_NETWORK:
      cs_build_nc_ref_list( psc_db, FALSE );
      
      if( grr_is_pbcch_present( ) )
      {
        /* begin temporary replacement of NETWORK CONTROL ORDER value */
        ctrl_order = cs_get_network_ctrl_order( FALSE );

        grr_data->db.nc_ref_lst.param->ctrl_order = prev_ctrl_order;
        prev_applied_ctrl_order                   = cs_get_network_ctrl_order( TRUE );

        grr_data->db.nc_ref_lst.param->ctrl_order = ctrl_order;
        applied_ctrl_order                        = cs_get_network_ctrl_order( TRUE );
        /* end temporary replacement of NETWORK CONTROL ORDER value */

        if( 
            (
              psc_db->nc_cw.list.chng_mrk.prev        NEQ
              psc_db->nc_cw.list.chng_mrk.curr
              OR 
              psc_db->nc_ms.ncmeas.list.chng_mrk.prev NEQ
              psc_db->nc_ms.ncmeas.list.chng_mrk.curr
            )
            OR
            (
              (
                (
                  applied_ctrl_order          EQ  NC_NC2 AND
                  prev_applied_ctrl_order     NEQ NC_NC2 
                )  
                OR
                (
                  applied_ctrl_order          NEQ NC_NC2 AND
                  prev_applied_ctrl_order     EQ  NC_NC2 
                )
              )
              AND
              (
                cs_all_cells_with_cr_param( ) EQ FALSE
              )
            )
          )
        {
          cs_send_cr_meas_req( CS_DELETE_MEAS_RSLT );

          psc_db->nc_cw.list.chng_mrk.prev = psc_db->nc_cw.list.chng_mrk.curr;
          psc_db->nc_ms.ncmeas.list.chng_mrk.prev =
                                      psc_db->nc_ms.ncmeas.list.chng_mrk.curr;
        }
      }
      else
      {
        if( 
            psc_db->nc_cw.list.chng_mrk.prev        NEQ
            psc_db->nc_cw.list.chng_mrk.curr
            OR 
            psc_db->nc_ms.ncmeas.list.chng_mrk.prev NEQ
            psc_db->nc_ms.ncmeas.list.chng_mrk.curr
            OR
            prev_ctrl_order                         NEQ 
            cs_get_network_ctrl_order( FALSE )
          )
        {
          cs_send_update_ba_req ( psc_db );

          if( grr_data->cs_meas.packet_mode EQ PACKET_MODE_PTM )
          {
            cs_send_cr_meas_req( CS_DELETE_MEAS_RSLT );
          }

          psc_db->nc_cw.list.chng_mrk.prev = psc_db->nc_cw.list.chng_mrk.curr;
          psc_db->nc_ms.ncmeas.list.chng_mrk.prev =
                                      psc_db->nc_ms.ncmeas.list.chng_mrk.curr;
        }
      }

      if( 
          psc_db->nc_cw.param.chng_mrk.prev        NEQ
          psc_db->nc_cw.param.chng_mrk.curr
          OR 
          psc_db->nc_ms.ncmeas.param.chng_mrk.prev NEQ
          psc_db->nc_ms.ncmeas.param.chng_mrk.curr
        )
      {
        if( cs_is_meas_reporting( ) EQ FALSE )
        {
          cs_cancel_meas_report( );
        }

        cs_process_t3158( );

        psc_db->nc_cw.param.chng_mrk.prev = psc_db->nc_cw.param.chng_mrk.curr;
        psc_db->nc_ms.ncmeas.param.chng_mrk.prev =
                                     psc_db->nc_ms.ncmeas.param.chng_mrk.curr;
      }

      TRACE_EVENT_P4( "NC Cell Re-Selection Parameter: %d %d %d %d",
                      cs_get_network_ctrl_order( FALSE ),
                      grr_data->db.nc_ref_lst.param->rep_per_i,
                      grr_data->db.nc_ref_lst.param->rep_per_t,
                      grr_data->db.nc_ref_lst.param->non_drx_per );
      break;

    default:
      TRACE_ERROR( "SIG_MEAS_CS_MEAS_ORDER unexpected" );
      break;
  }
} /* sig_meas_cs_meas_order() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_install_new_scell
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_install_new_scell ( void )
{ 
  TRACE_ISIG( "sig_ctrl_cs_install_new_scell" );
 
  grr_data->db.cr_cell         = NULL;

  grr_set_db_ptr( DB_MODE_CC_ACC );
  /* cs_build_nc_ref_list( posc_db, FALSE ); */
  /* cs_reset_nc_change_mark( posc_db ); */

}/* sig_ctrl_cs_install_new_scell */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_start_cell_selection
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_start_cell_selection ( UBYTE cr_type )
{ 
  TRACE_ISIG( "sig_ctrl_cs_start_cell_selection" );
 
  switch( GET_STATE( CS ) )
  {
    case( CS_NULL ):
    case( CS_IDLE ):
    case( CS_CR_MOBILE ):

      switch( cr_type )
      {
        case( CR_NEW ):
          TRACE_EVENT( "CTRL->CS: start cell re-selection" );
          
          grr_data->db.cr_cell->mode = CS_MODE_SELECTION_PERFORMED;

          SET_STATE( CS, CS_CR_MOBILE );

          cs_start_t_reselect( );
          break;

        case( CR_CS ):
          TRACE_EVENT( "CTRL->CS: start cell selection" );

          SET_STATE( CS, CS_NULL );

          cs_stop_t_reselect( );
          break;

        case( CR_CONT ):
          TRACE_EVENT( "CTRL->CS: continue cell (re-)selection" );
          break;

        default:
          TRACE_ASSERT( cr_type EQ CR_NEW  OR
                        cr_type EQ CR_CS   OR
                        cr_type EQ CR_CONT    );
          break;
      }
      break;
    case( CS_CR_NETWORK ):

      switch( cr_type )
      {
        case( CR_NEW ):
        case( CR_NEW_NOT_SYNCED ):
          TRACE_EVENT( "CTRL->CS: CS_CR_NETWORK:cell re-selection" );
          
          grr_data->db.cr_cell->mode = CS_MODE_SELECTION_PERFORMED;

          /*
           * Timer T_RESLECT should be stopped during processing of a 
           * Packet Cell Change Order. Timer T3174 is used instead.
           */
          break;

        default:
          TRACE_ASSERT( cr_type EQ CR_NEW OR cr_type EQ CR_NEW_NOT_SYNCED );
          break;
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_CS_START_CELL_SELECTION unexpected" );
      break;
  }
}/* sig_ctrl_cs_start_cell_selection */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_cc_order_ind
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_cc_order_ind ( T_D_CELL_CHAN_ORDER *d_cell_chan_order,
                                       T_D_CHANGE_ORDER    *d_change_order )
{ 
  TRACE_ISIG( "sig_ctrl_cs_cc_order_ind" );

  if( d_cell_chan_order NEQ NULL )
  {
    if( d_change_order EQ NULL )
    {
      cs_cell_change_order_ind
        ( &d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.nc_meas_par_list.nc_meas_par,
          d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.nc_meas_par_list.v_nc_freq_list,
          &d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.nc_meas_par_list.nc_freq_list
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          , d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.release_98_str_pcco.v_release_99_str_pcco,
          &d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.release_98_str_pcco.release_99_str_pcco.enh_meas_param_pcco
#endif
        );
    }
    else
    {
      TRACE_ERROR( "Call error 1 in sig_ctrl_cs_cc_order_ind" );
    }
  }
  else if( d_change_order NEQ NULL )
  {
    T_nc_meas_par nc_meas_par;

    nc_meas_par.v_nc_meas_per = FALSE;
    
    switch( d_change_order->nc_mode.ncm )
    {
      case( NC_0      ): nc_meas_par.ctrl_order = NCMEAS_NC0;   break; 
      case( NC_1      ): nc_meas_par.ctrl_order = NCMEAS_NC1;   break;
      case( NC_2      ): nc_meas_par.ctrl_order = NCMEAS_NC2;   break;
      case( NC_TARGET ): 
      default          : nc_meas_par.ctrl_order = NCMEAS_RESET; break;
    }

    cs_cell_change_order_ind( &nc_meas_par, FALSE, NULL
#if defined (REL99) AND defined (TI_PS_FF_EMR)
                            , FALSE, NULL
#endif                     
                            );  
  }
  else
  {
    TRACE_ERROR( "Call error 2 in sig_ctrl_cs_cc_order_ind" );
  }
}/* sig_ctrl_cs_cc_order_ind */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_check_c1_new_scell
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sig_ctrl_cs_check_c1_new_scell ( BOOL  cell_change_order, 
                                             UBYTE gprs_rxlev_access_min,
                                             UBYTE gprs_ms_txpwr_max_cch )
{ 
  BOOL             result = TRUE;
  T_ncell_info    *ncell_info;
  T_NC_MVAL       *nc_mval;
  T_CR_PAR_1       cr_par_1;
  T_CR_CRITERIONS  cr_crit;
  
  TRACE_ISIG( "sig_ctrl_cs_check_c1_new_scell" );

  /*
   * GRR should not recalculate the C1 value for the serving cell once received 
   * the System Information on the new cell in case a cell change order has been
   * processed previously.
   */
  if( cell_change_order EQ FALSE )
  {
    if( grr_data->db.cr_cell NEQ NULL )
    {
      nc_mval    = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];
      ncell_info =  grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

      if( ncell_info NEQ NULL )
      {
        cr_par_1.cr_pow_par.gprs_rxlev_access_min = gprs_rxlev_access_min; 
        cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch = gprs_ms_txpwr_max_cch; 

        /*
         * the HCS parameter are not present because 
         * they are not determined by the entity RR
         */
        cr_par_1.v_hcs_par = FALSE;

        /*
         * calculate the cell re-selection criterions for the new serving cell
         * taking under account the new cell re-selection parameters determined 
         * by the entity RR
         */
        cs_calc_cr_criterion( &cr_crit,
                               ncell_info->arfcn, 
                              &nc_mval->rla_p,
                              &cr_par_1 );

        TRACE_EVENT_P1( "RECALC C1 SCELL: %d", cr_crit.c1 );
  
        /*
         * if the path loss criterion is not fulfilled for the new serving cell
         * forget it for any further operations
         */
        result = ( cr_crit.c1 >= 0 );
      }
      else
      {
        TRACE_ERROR( "sig_ctrl_cs_check_c1_new_scell: ncell_info EQ NULL" );
      }
    }
    else
    {
      TRACE_ERROR( "sig_ctrl_cs_check_c1_new_scell: grr_data->db.cr_cell EQ NULL" );
    }
  }
  else
  {
    TRACE_EVENT( "sig_ctrl_cs_check_c1_new_scell: skip check" );
  }

  return( result );

}/* sig_ctrl_cs_check_c1_new_scell */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_pmr_accept
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_PMR_ACCEPT
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_pmr_accept ( void )
{ 
  UBYTE nc_non_drx_period = grr_data->db.nc_ref_lst.param->non_drx_per;

  TRACE_ISIG( "sig_ctrl_cs_pmr_accept" );

  switch( GET_STATE( CS_MEAS ) )
  {
    case( CS_MEAS_PMR_SENDING ):

#if defined (REL99) AND (TI_PS_FF_EMR)
      if( ( ( cs_is_enh_meas_reporting() EQ FALSE ) AND ( cs_send_meas_rpt( FALSE ) EQ FALSE ) )
            OR
            ( cs_is_enh_meas_reporting() EQ TRUE ) 
          )
#else
        if( cs_send_meas_rpt( FALSE ) EQ FALSE )
#endif
      {
        SET_STATE( CS_MEAS, CS_MEAS_NULL );
        cs_process_t3158( );
      }

      sig_cs_pg_start_t_nc2_ndrx( nc_non_drx_period );
      break;

    default:
      TRACE_ERROR( "SIG_MEAS_CS_PMR_ACCEPT unexpected" );
      break;
  }
} /* sig_ctrl_cs_pmr_accept() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_pmr_reject
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_PMR_REJECT
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_pmr_reject ( void )
{ 
  TRACE_ISIG( "sig_ctrl_cs_pmr_reject" );

  switch( GET_STATE( CS_MEAS ) )
  {
    case( CS_MEAS_PMR_SENDING ):
      SET_STATE( CS_MEAS, CS_MEAS_NULL );

      cs_process_t3158( );
      break;

    default:
      TRACE_ERROR( "SIG_MEAS_CS_PMR_REJECT unexpected" );
      break;
  }
} /* sig_ctrl_cs_pmr_reject() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_check_cell_location
+------------------------------------------------------------------------------
| Description : Check the location of the target cell when packet cell change
|               has to be processed
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_check_cell_location ( USHORT arfcn, UBYTE bsic )
{
  T_NC_MVAL *nc_mval;
  UBYTE      nc_mval_idx;

  TRACE_EVENT( "sig_ctrl_cs_check_cell_location" );

  /* check whether the cell is in the neighbour cell list */
  nc_mval = grr_get_nc_mval( arfcn, bsic, &nc_mval_idx );

  if( nc_mval NEQ NULL )
  {            
    TRACE_EVENT( "sig_ctrl_cs_check_cell_location: cell part of neighbour cell list" );

    grr_data->db.cr_cell             = &grr_data->db.nc_nw_slctd.strgst;
    grr_data->db.cr_cell->idx        = nc_mval_idx;
    grr_data->db.cr_cell->mode       = CS_MODE_IDLE;
    grr_data->db.cr_cell->avail_time = 0;
  }
  else
  {
    /* 
     * In case of a BCCH only environment no neighbor cell measured values
     * nc_mval are present at all. As result always the cell re-selection 
     * cell cr_cell should be created and error traces should be suppressed.
     */
    cs_create_cr_cell( arfcn, bsic );
  }
} /* sig_ctrl_cs_check_cell_location */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_cs_cr_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_CS_CR_MEAS_IND
|
| Parameters  : *mphp_cr_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_cs_cr_meas_ind( T_MPHP_CR_MEAS_IND * mphp_cr_meas_ind )
{ 

  UBYTE state          = GET_STATE( CS );
  BOOL  cr_initiated   = FALSE;
  BOOL  strgst_changed = FALSE;

  TRACE_FUNCTION( "sig_gfp_cs_cr_meas_ind" );

  switch( state )
  {
    case CS_IDLE:
    case CS_CR_NETWORK:
      if( mphp_cr_meas_ind->list_id EQ ( UBYTE )( grr_data->cs.list_id - 1 ) )
      {
        if( mphp_cr_meas_ind->nmeas >   MPHP_NUMC_BA_GPRS_SC OR 
            mphp_cr_meas_ind->nmeas NEQ nc_data->c_cell         )
        {
          TRACE_EVENT_P2( "sig_gfp_cs_cr_meas_ind: wrong number of measurements - %d %d",
                          mphp_cr_meas_ind->nmeas, nc_data->c_cell ); 
          return;
        }   

        cs_fill_meas_rslt( mphp_cr_meas_ind, NULL );
        cs_store_meas_values( );
        
        if( cs_find_strongest( mphp_cr_meas_ind->reporting_period ) OR
            grr_data->cs.is_upd_ncell_stat_needed              )
        {
          strgst_changed                        = TRUE;
          grr_data->cs.is_upd_ncell_stat_needed = FALSE;
        }
      
 
        if( state EQ CS_IDLE )
        {
          cr_initiated = cs_cr_decision( CRDM_CR_INITIAL );
        }
#if defined (REL99) AND (TI_PS_FF_EMR)
        if(  ( (strgst_changed EQ TRUE) OR (cs_is_enh_meas_reporting()) ) 
                AND cr_initiated EQ FALSE  )
#else
        if( strgst_changed EQ TRUE AND cr_initiated EQ FALSE )
#endif

        {
          cs_update_bsic( );
        }
      }
      else
      {
        TRACE_EVENT_P2( "sig_gfp_cs_cr_meas_ind: mismatch of list_id - %d %d",
                        mphp_cr_meas_ind->list_id, ( UBYTE )( grr_data->cs.list_id - 1 ) );
      }
      cs_check_nc_sync_timer(mphp_cr_meas_ind->reporting_period);
      break;

    default:
      TRACE_ERROR( "SIG_GFP_CS_CR_MEAS_IND unexpected" );
      break;
  }
} /* sig_gfp_cs_cr_meas_ind() */




/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_cs_tcr_meas_ind
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_TCS_CR_MEAS_IND
|
| Parameters  : *mphp_tcr_meas_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_cs_tcr_meas_ind( T_MPHP_TCR_MEAS_IND * mphp_tcr_meas_ind )
{ 
  UBYTE state          = GET_STATE( CS );
  BOOL  cr_initiated   = FALSE;
  BOOL  strgst_changed = FALSE;

  TRACE_FUNCTION( "sig_gfp_cs_cr_meas_ind" );

  switch( state )
  {
    case CS_IDLE:
    case CS_CR_NETWORK:
      if( grr_is_pbcch_present() )
      {
        cs_fill_meas_rslt( NULL, mphp_tcr_meas_ind );
        cs_store_meas_values( );
     
        if( cs_find_strongest( CS_RPT_PRD_PTM )  OR
            grr_data->cs.is_upd_ncell_stat_needed   )
        {
          strgst_changed                        = TRUE;
          grr_data->cs.is_upd_ncell_stat_needed = FALSE;
        }      

        if( state EQ CS_IDLE )
        {
          cr_initiated = cs_cr_decision( CRDM_CR_INITIAL );
        }
#if defined (REL99) AND defined (TI_PS_FF_EMR)
        if(  ( (strgst_changed EQ TRUE) OR (cs_is_enh_meas_reporting()) )
          AND cr_initiated EQ FALSE  )
#else
        if( strgst_changed EQ TRUE AND cr_initiated EQ FALSE )
#endif
        {
          cs_update_bsic( );
        }
        cs_check_nc_sync_timer(CS_RPT_PRD_PTM);
      }
      else
      {
        cs_tcr_meas_ind_to_pl( mphp_tcr_meas_ind );
      }
      break;

    default:
      TRACE_ERROR( "SIG_GFP_CS_TCR_MEAS_IND unexpected" );
      break;
  }
} /* sig_gfp_cs_tcr_meas_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_pg_cs_pause
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CS_PAUSE
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_cs_pause ( void )
{ 
  TRACE_ISIG( "sig_pg_cs_pause" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_NULL:
    case CS_IDLE:
      if( grr_data->cs_meas.packet_mode EQ  PACKET_MODE_PIM AND 
          grr_data->nc_data.c_cell  NEQ 0           )
      {
        cs_stop_cr_meas( );
      }
      break;

    default:
      TRACE_ERROR( "SIG_PG_CS_PAUSE unexpected" );
      break;
  }
} /* sig_pg_cs_pause() */

/*
+------------------------------------------------------------------------------
| Function    : sig_pg_cs_resume
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_PG_CS_RESUME
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_pg_cs_resume ( void )
{ 
  TRACE_ISIG( "sig_pg_cs_resume" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_NULL:
    case CS_IDLE:
      cs_req_cr_meas( );
      break;

    default:
      TRACE_ERROR( "SIG_PG_CS_RESUME unexpected" );
      break;
  }
} /* sig_pg_cs_resume() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_cs_cr_meas_stop_con
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_CS_CR_MEAS_STOP_CON
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_cs_cr_meas_stop_con ( T_MPHP_CR_MEAS_STOP_CON * dummy)
{ 
  TRACE_ISIG( "sig_gfp_cs_cr_meas_stop_con" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
      grr_data->cs.stop_req = FALSE;
      break;
    default:
      TRACE_ERROR( "SIG_GFP_CS_CR_MEAS_STOP_CON unexpected" );
      break;
  }
} /* sig_gfp_cs_cr_meas_stop_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_simu_timer_stop
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_SIMU_TIMER_STOP
|
| Parameters  : dummy - description of parameter dummy
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_simu_timer_stop ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_simu_timer_stop" );

#ifdef _SIMULATION_ 
  cs_stop_t_reselect( );
  cs_stop_t3158( );
  
  if(  grr_is_pbcch_present( ) EQ FALSE      AND
       grr_data->cs_meas.nc_meas_cause NEQ NC_STOP_MEAS   )
  {      
     cs_send_meas_rep_req ( NC_STOP_MEAS ); 
  }
#endif
} /* sig_ctrl_cs_stop() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_nc_param_invalid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_NC_PARAM_INVALID
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_nc_param_invalid ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_nc_param_invalid" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_MOBILE:
    case CS_CR_NETWORK:
      psc_db->nc_cw.param.ctrl_order = NC_NC0;

      cs_process_t3158( );

      TRACE_EVENT_P4( "NC Cell Re-Selection Parameter: %d %d %d %d",
                      cs_get_network_ctrl_order( FALSE ),
                      grr_data->db.nc_ref_lst.param->rep_per_i,
                      grr_data->db.nc_ref_lst.param->rep_per_t,
                      grr_data->db.nc_ref_lst.param->non_drx_per );
      break;

    case CS_NULL:
    default:
      /* do nothing */
      break;
  }
} /* sig_ctrl_cs_nc_param_invalid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_nc_param_valid
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_NC_PARAM_VALID
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_nc_param_valid ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_nc_param_valid" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_MOBILE:
    case CS_CR_NETWORK:
      cs_process_t3158( );

      TRACE_EVENT_P4( "NC Cell Re-Selection Parameter: %d %d %d %d",
                      cs_get_network_ctrl_order( FALSE ),
                      grr_data->db.nc_ref_lst.param->rep_per_i,
                      grr_data->db.nc_ref_lst.param->rep_per_t,
                      grr_data->db.nc_ref_lst.param->non_drx_per );
      break;

    case CS_NULL:
    default:
      /* do nothing */
      break;
  }
} /* sig_ctrl_cs_nc_param_valid() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_ba_changed
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_BA_CHANGED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_ba_changed ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_ba_changed" );
  
  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
    case CS_CR_MOBILE:
    case CS_CR_NETWORK:
      cs_init_nc_mval_lst( );
      cs_build_nc_ref_list( psc_db, FALSE );
      cs_reset_nc_change_mark( psc_db );

      if( grr_data->cs_meas.packet_mode EQ PACKET_MODE_PTM )
      {
        cs_send_cr_meas_req( CS_DELETE_MEAS_RSLT );
      }
      break;

    case CS_NULL:
    default:
      /* do nothing */
      break;
  }
} /* sig_ctrl_cs_ba_changed() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_reset_meas_rep_params
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_RESET_MEAS_REP_PARAMS
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_reset_meas_rep_params ( void ) 
{ 
  TRACE_ISIG( "sig_ctrl_cs_reset_meas_rep_params" );
  
  cs_reset_meas_rep_params (psc_db);

} /* sig_ctrl_cs_reset_meas_rep_params() */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_cs_si2quater_ind 
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_CS_SI2QUATER_IND
|
| Parameters  : rrgrr_si2quater_ind - Pointer to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_cs_si2quater_ind ( T_RRGRR_SI2QUATER_IND 
                                      *rrgrr_si2quater_ind)
{
  TRACE_ISIG( "sig_ctrl_cs_si2quater_ind" );

  switch( GET_STATE( CS ) )
  {
  case CS_IDLE:
  case CS_CR_NETWORK:
    grr_copy_enh_and_nc_params_from_si2qtr(rrgrr_si2quater_ind);
    break;

  default:
    TRACE_ERROR( "SIG_CTRL_CS_SI2QUATER_IND unexpected" );
    break;
  }
}

#endif

