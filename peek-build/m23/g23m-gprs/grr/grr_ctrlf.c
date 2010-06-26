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
|  Purpose :  This module implements local functions for service CTRL of
|             entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CTRLF_C
#define GRR_CTRLF_C
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

#include "grr_ctrlf.h"  /* */
#include "grr_ctrls.h"

#include "grr_meass.h"  /* */
#include "grr_pgs.h"    /* */
#include "grr_psis.h"   /* */
#include "grr_css.h"    /* */
#include "grr_tcs.h"    /* */

#include <string.h>     /* for memcpy */
#include "pcm.h"

#ifdef _TARGET_
#include "ffs/ffs.h"
#endif



/*==== CONST ================================================================*/

#define BCC_OF_BSIC      7

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

LOCAL void ctrl_set_final_pckt_mode         ( void                          );
LOCAL BOOL ctrl_set_transient_pckt_mode     ( T_GLBL_PCKT_MODE final_pckt_mode,
                                              T_CTRL_TASK *ctrl_task        );
LOCAL void ctrl_enter_pim                   ( void                          );
LOCAL void ctrl_leave_to_pam                ( void                          );
LOCAL void ctrl_enter_pam                   ( void                          );
LOCAL void ctrl_enter_ptm                   ( void                          );
LOCAL BOOL ctrl_check_cco_freq              ( USHORT arfcn                  );
/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : ctrl_stop_all_activities
+------------------------------------------------------------------------------
| Description : The function ctrl_stop_all_activities() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_stop_all_activities ( T_CTRL_DISABLE_CAUSE  dc,
                                       void                 *data )
{ 
  BOOL is_pbcch_reset_needed = FALSE;

  TRACE_FUNCTION( "ctrl_stop_all_activities" );

  /*
   * inform the service PG
   * parameters dc and data are not of interest 
   */
  sig_ctrl_pg_stop( );

  /*
   * inform the service MEAS 
   * parameter data is not of interest 
   */
  switch( dc )
  {
    case( CTRL_DC_OTHER ):
    case( CTRL_DC_SUSPEND_NORMAL ):    
    case( CTRL_DC_SUSPEND_IMM_REL ):
    case( CTRL_DC_CR_NW_RR ):
    case( CTRL_DC_CR_NW_GRR ):
    case( CTRL_DC_CR_MS_GRR_NORMAL ):
    case( CTRL_DC_PSI_MANAGEMENT ):
    case( CTRL_DC_CR_MS_RR_NORMAL ):
    case( CTRL_DC_CR_MS_RR_IMM_REL ):
    case( CTRL_DC_CR_MS_GRR_ABNORMAL ):
      sig_ctrl_meas_stop( );
      break;

    default:
      TRACE_EVENT( "FATAL ERROR 1 in ctrl_stop_all_activities" );
      break;
  }
 
  /*
   * inform the service CS 
   */
  switch( dc )
  {
    case( CTRL_DC_OTHER ):
      vsi_t_stop( GRR_handle, T3174 );
      vsi_t_stop( GRR_handle, T3176 );
#ifdef _SIMULATION_
      sig_ctrl_cs_simu_timer_stop();
#endif

      /*lint -fallthrough*/

    case( CTRL_DC_CR_MS_GRR_NORMAL ):
    case( CTRL_DC_CR_MS_GRR_ABNORMAL ):
    case( CTRL_DC_PSI_MANAGEMENT ):
    case( CTRL_DC_CR_MS_RR_NORMAL ):
    case( CTRL_DC_CR_MS_RR_IMM_REL ):
      sig_ctrl_cs_stop(CS_DC_OTHER);
      break;

    case( CTRL_DC_SUSPEND_NORMAL ):    
    case( CTRL_DC_SUSPEND_IMM_REL ):
      sig_ctrl_cs_stop( CS_DC_DEDICATED );
      break; 

    case( CTRL_DC_CR_NW_RR ):
      sig_ctrl_cs_cc_order_ind( NULL, (T_D_CHANGE_ORDER *)data );
      break;

    case( CTRL_DC_CR_NW_GRR ):
      if (grr_t_status( T3174 ) > 0 )
      {
        sig_ctrl_cs_cc_order_ind( (T_D_CELL_CHAN_ORDER *)data, NULL );
      }
      else
      {
        sig_ctrl_cs_stop(CS_DC_OTHER);
      }
      break;

    default:
      TRACE_EVENT( "FATAL ERROR 2 in ctrl_stop_all_activities" );
      break;  
  }

  /*
   * inform the service PSI
   * parameter data is not of interest 
   */
  switch( dc )
  {
    case( CTRL_DC_OTHER ):
    case( CTRL_DC_CR_MS_RR_NORMAL ):
    case( CTRL_DC_CR_MS_RR_IMM_REL ):
    case( CTRL_DC_CR_MS_GRR_NORMAL ):
      sig_ctrl_psi_stop( );


      is_pbcch_reset_needed = TRUE;
      break;

    case( CTRL_DC_SUSPEND_NORMAL ):    
    case( CTRL_DC_SUSPEND_IMM_REL ):
    case( CTRL_DC_CR_NW_RR ):
    case( CTRL_DC_CR_NW_GRR ):
    case( CTRL_DC_CR_MS_GRR_ABNORMAL ):
      sig_ctrl_psi_suspend( );
      break;

    case( CTRL_DC_PSI_MANAGEMENT ):
      /* do nothing */
      break;

    default:
      TRACE_EVENT( "FATAL ERROR 3 in ctrl_stop_all_activities" );
      break;
  }
  
  /*
   * inform the service TC
   * parameter data is not of interest 
   */
  switch( dc )
  {
    case( CTRL_DC_OTHER ):
    case( CTRL_DC_PSI_MANAGEMENT ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_OTHER,           sig_tc_ctrl_tbf_release_cnf );
      break;

    case( CTRL_DC_CR_NW_RR ):
    case( CTRL_DC_CR_NW_GRR ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_CR_NETWORK,      sig_tc_ctrl_cr_started_rsp  );
      break;

    case( CTRL_DC_CR_MS_RR_NORMAL ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_CR_NORMAL,       sig_tc_ctrl_cr_started_rsp  );
      break;

    case( CTRL_DC_CR_MS_RR_IMM_REL ):
    case( CTRL_DC_CR_MS_GRR_NORMAL ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_CR_IMM_REL,      sig_tc_ctrl_cr_started_rsp  );
      break;

    case( CTRL_DC_CR_MS_GRR_ABNORMAL ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_CR_ABNORMAL,     sig_tc_ctrl_cr_started_rsp  );
      break;

    case( CTRL_DC_SUSPEND_NORMAL ):    
      ctrl_send_access_disable_if_needed
                        ( TC_DC_SUSPEND_NORMAL,  sig_tc_ctrl_suspend_cnf     );
      break;

    case( CTRL_DC_SUSPEND_IMM_REL ):
      ctrl_send_access_disable_if_needed
                        ( TC_DC_SUSPEND_IMM_REL, sig_tc_ctrl_suspend_cnf     );
      break;

    default:
      TRACE_EVENT( "FATAL ERROR 4 in ctrl_stop_all_activities" );
      break;
  }
  
  if( is_pbcch_reset_needed )
  {
    grr_set_pbcch( FALSE );
  }
} /* ctrl_stop_all_activities() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_init
+------------------------------------------------------------------------------
| Description : The function ctrl_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_init ( void)
{ 
  TRACE_FUNCTION( "ctrl_init" );

  /* init parameters of CTRL*/
  
  INIT_STATE( CTRL,      CTRL_NULL           );
  INIT_STATE( CTRL_GLBL, GLBL_PCKT_MODE_NULL );

  grr_data->ctrl.new_cell_responded = FALSE;
  grr_data->ctrl.psi_tbf_rel_cause  = CTRL_PTRC_NONE;
  grr_data->ctrl_glbl.ctrl_task     = CTRL_TASK_NONE;

#ifdef _SIMULATION_

  TRACE_EVENT_P1( "ctrl_init - ctrl_task: %d", grr_data->ctrl_glbl.ctrl_task );

#endif /* #ifdef _SIMULATION_ */

  grr_data->ctrl.rr_state.monitoring_stopped = FALSE;
  grr_data->ctrl.rr_state.cr_ind_return      = CR_IND_RETURN_NONE;
  
  ctrl_read_pcm();

} /* ctrl_init() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_start_monitoring_bcch
+------------------------------------------------------------------------------
| Description : The function ctrl_start_monitoring_bcch() This function starts 
|               reading System Information (SI) on BCCH
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_start_monitoring_bcch ( UBYTE si_to_read )
{ 
  TRACE_FUNCTION( "ctrl_start_monitoring_bcch" );
    
  {
    PALLOC( rrgrr_start_mon_bcch_req, RRGRR_START_MON_BCCH_REQ );

    rrgrr_start_mon_bcch_req->si_to_read = psc_db->si_to_acquire = si_to_read;

    PSEND( hCommRR, rrgrr_start_mon_bcch_req );
  }
} /* ctrl_start_monitoring_bcch() */

  
/*
+------------------------------------------------------------------------------
| Function    : ctrl_stop_monitoring_ccch
+------------------------------------------------------------------------------
| Description : The function ctrl_stop_monitoring_ccch() This function stops 
|               monitoring CCCH
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_stop_monitoring_ccch ( void )
{ 
  TRACE_FUNCTION( "ctrl_stop_monitoring_ccch" );
  
  if(!grr_data->ctrl.rr_state.monitoring_stopped OR (grr_data->test_mode NEQ CGRLC_NO_TEST_MODE))
  {
    PALLOC ( rrgrr_stop_mon_ccch_req, RRGRR_STOP_MON_CCCH_REQ);
    rrgrr_stop_mon_ccch_req->is_pbcch_present = grr_is_pbcch_present();
    PSEND (hCommRR, rrgrr_stop_mon_ccch_req);
  }
  grr_data->ctrl.rr_state.monitoring_stopped = TRUE;
} /* ctrl_stop_monitoring_ccch() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_start_monitoring_ccch
+------------------------------------------------------------------------------
| Description : The function ctrl_start_monitoring_ccch() This function starts 
|               monitoring CCCH
|
| Parameters  : pag_mode : indicates the page, which shall be uese by RR/L1
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_start_monitoring_ccch ( UBYTE pag_mode )
{ 
  TRACE_FUNCTION( "ctrl_start_monitoring_ccch" );
  
  {
    PALLOC ( rrgrr_start_mon_ccch_req, RRGRR_START_MON_CCCH_REQ);
    rrgrr_start_mon_ccch_req->pag_mode = pag_mode;
#if defined (REL99) AND defined (TI_PS_FF_EMR)
#else
    rrgrr_start_mon_ccch_req->split_pg = grr_data->ms.split_pg_cycle;
#endif
    PSEND (hCommRR, rrgrr_start_mon_ccch_req);
  }
  grr_data->ctrl.rr_state.monitoring_stopped = FALSE;
} /* ctrl_start_monitoring_ccch() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_rr_est_req
+------------------------------------------------------------------------------
| Description : The function ctrl_send_rr_est_req() .... 
|
| Parameters  : UBYTE susp_req: whether RR needs to send
|                     GPRS Suspension Request or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_rr_est_req ( UBYTE susp_req)
{ 
  TRACE_FUNCTION( "ctrl_send_rr_est_req" );
  {
    PALLOC ( rrgrr_rr_est_req, RRGRR_RR_EST_REQ);
    
    if(psc_db->v_non_gprs_opt EQ TRUE)
    {
      rrgrr_rr_est_req->non_gprs.v_non_gprs = TRUE;
      
      /* Copy non GPRS options  */
      ctrl_copy_non_gprs_opt(&rrgrr_rr_est_req->non_gprs);
    }
    else
      rrgrr_rr_est_req->non_gprs.v_non_gprs = FALSE;
    
    rrgrr_rr_est_req->ch_needed             = grr_data->pg.ch_needed;
    
    rrgrr_rr_est_req->emlpp_prio.v_emlpp    = grr_data->pg.v_emlpp_prio;
    rrgrr_rr_est_req->emlpp_prio.emlpp      = grr_data->pg.emlpp_prio;

    rrgrr_rr_est_req->susp_req = susp_req;
    
    rrgrr_rr_est_req->ident_type = grr_data->pg.type; /* IMSI TMSI ??*/   

    PSEND (hCommRR, rrgrr_rr_est_req);
  }
} /* ctrl_send_rr_est_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_rr_est_rsp
+------------------------------------------------------------------------------
| Description : The function ctrl_send_rr_est_rsp() .... 
|               Informs the RR whether the RR connection should be established or not!
| Parameters  : UBYTE susp_req: Suspension request needed or not
|                    UBYTE rsp: CS_PAGE was accepted or not
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_rr_est_rsp ( UBYTE susp_req, UBYTE rsp )
{ 
  TRACE_FUNCTION( "ctrl_send_rr_est_rsp" );
  {
    PALLOC(rrgrr_rr_est_rsp, RRGRR_RR_EST_RSP);
    
    rrgrr_rr_est_rsp->rr_est = (rsp EQ GMMRR_CS_PAGE_REJ) ? RR_EST_NOT_ALLOWED : RR_EST_ALLOWED;

    rrgrr_rr_est_rsp->susp_req = susp_req;
        
    PSEND(hCommRR, rrgrr_rr_est_rsp);
  }
}/* ctrl_send_rr_est_rsp*/

/*
+------------------------------------------------------------------------------
| Function    : ctrl_suspend_services
+------------------------------------------------------------------------------
| Description : The function ctrl_suspend_services() .... 
|               This function suspends working services in GRR
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_suspend_services ( UBYTE susp_cause )
{ 
  TRACE_FUNCTION( "ctrl_suspend_services" );
  
  if(susp_cause EQ GMMRR_ABNORMAL_RELEASE)
    ctrl_stop_all_activities( CTRL_DC_SUSPEND_IMM_REL, NULL );
  else
    ctrl_stop_all_activities( CTRL_DC_SUSPEND_NORMAL, NULL );
  
}/* ctrl_suspend_services*/

/*
+------------------------------------------------------------------------------
| Function    : ctrl_suspend_services
+------------------------------------------------------------------------------
| Description : The function ctrl_copy_non_gprs_opt() .... 
|               Copies the stored non-GPRS options into the given structure
| Parameters  : p_non_gprs_opt: Pointer to the structure for non-GPRS access; derived from 
|                   RRGRR.doc
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_copy_non_gprs_opt( T_non_gprs *p_non_gprs_opt)
{ 

    TRACE_FUNCTION( "ctrl_copy_non_gprs_opt" );

    /* Copy non GPRS options  */
    p_non_gprs_opt->att                   = psc_db->non_gprs_opt.att;            /*<  0> Attach / detach allowed        */ 
    
    p_non_gprs_opt->v_T3212               = psc_db->non_gprs_opt.v_t3212;        /*<  1> valid-flag                     */ 
    p_non_gprs_opt->T3212                 = psc_db->non_gprs_opt.t3212;          /*<  2> T 3212 time-out value          */ 
    
    p_non_gprs_opt->neci                  = psc_db->non_gprs_opt.neci;           /*<  3> Half rate support              */ 
    p_non_gprs_opt->pwrc                  = psc_db->non_gprs_opt.pwcr;           /*<  4> Power Control                  */ 
    p_non_gprs_opt->dtx                   = psc_db->non_gprs_opt.dtx;            /*<  5> Discontinuous Transmission (BCCH) */ 
    p_non_gprs_opt->radio_link_timeout    = psc_db->non_gprs_opt.rl_timeout;     /*<  6> Radio Link Timeout             */ 
    p_non_gprs_opt->bs_ag_blks_res        = psc_db->non_gprs_opt.bs_ag_blks_res; /*<  7> BS_AG_BLKS_RES                 */ 
    p_non_gprs_opt->ccch_conf             = psc_db->non_gprs_opt.ccch_conf;      /*<  8> CCCF-CONF                      */ 
    p_non_gprs_opt->bs_pa_mfrms           = psc_db->non_gprs_opt.bs_pa_mfrms;    /*<  9> BS-PA-MFRMS                    */ 
    p_non_gprs_opt->max_retrans           = psc_db->non_gprs_opt.max_retrans;    /*< 10> MAX_RETRANS                    */ 
    p_non_gprs_opt->tx_integer            = psc_db->non_gprs_opt.tx_integer;     /*< 11> TX_INTEGER                     */ 
    p_non_gprs_opt->ec                    = psc_db->non_gprs_opt.ec;             /*< 12> EC                             */ 
    p_non_gprs_opt->gprs_ms_txpwr_max_cch = psc_db->non_gprs_opt.txpwr_max_cch;  /*< 13> GPRS_MS_TXPWR_MAX_CCH          */ 

#ifdef REL99
  /* copy non GPRS extension information */
  if( (psc_db->non_gprs_opt.v_non_gprs_ext_bits EQ TRUE) AND 
      (psc_db->non_gprs_opt.non_gprs_ext_bits.ext_len) )
  {
    /* copy Early classmark sending control flag from GRR database */
    p_non_gprs_opt->ecsc = 
      psc_db->non_gprs_opt.non_gprs_ext_bits.non_gprs_ext_info.ecsc;
  }
  else
  {
    p_non_gprs_opt->ecsc = ECS_ALLOWED;
  }
#endif

}/* ctrl_copy_non_gprs_opt*/


/*
+------------------------------------------------------------------------------
| Function    : ctrl_is_gprs_suspension_request_needed
+------------------------------------------------------------------------------
| Description : The function ctrl_is_gprs_suspension_request_needed() .... 
|               checks whether MS has to send GPRS Suspension Request to the network
|                 or not
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ctrl_is_gprs_suspension_request_needed( void)
{ 
    TRACE_FUNCTION( "ctrl_is_gprs_suspension_request_needed" );

    if(
      (psc_db->cell_info_for_gmm.cell_info.net_mode NEQ GMMRR_NET_MODE_III)
      AND
      (
/* BEGIN GRR_NMO_III*/      
      (grr_data->ms.class_of_mode EQ GMMRR_CLASS_B) OR 
/* END GRR_NMO_III*/      
      (grr_data->ms.class_of_mode EQ GMMRR_CLASS_BC) OR 
      (grr_data->ms.class_of_mode EQ GMMRR_CLASS_BG)
      )
      )
    {
        /*  We have to suspend GPRS activities on the network side, because of MS 
         *  is not able to handle GPRS activities during the dedicated mode of operations
         */
      return TRUE;
    }
    return FALSE;
}/* ctrl_is_gprs_suspension_request_needed*/


/*
+------------------------------------------------------------------------------
| Function    : ctrl_get_number_of_digits
+------------------------------------------------------------------------------
| Description : The function ctrl_get_number_of_digits() .... 
|               gets the  number of valid digits in the array e.g. IMSI digits
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE ctrl_get_number_of_digits( UBYTE *digits)
{ 
  UBYTE i = 0;

  TRACE_FUNCTION ("ctrl_get_number_of_digits");

  while (digits[i] < 0x0A AND i < 16)
    i++;

  return i;
}/* ctrl_get_number_of_digits*/

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_cell_reselection_req
+------------------------------------------------------------------------------
| Description : The function ctrl_send_cell_reselection_req() .... 
|               Prepares the cell reselection parameters for RR
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_cell_reselection_req ( UBYTE cr_type )
{ 
  TRACE_FUNCTION( "ctrl_send_cell_reselection_req" );

  switch( cr_type )
  {
    case( CR_NEW ):
    case( CR_NEW_NOT_SYNCED ):
    {
      ctrl_stop_rr_task_req( GLBL_PCKT_MODE_NULL, TASK_STOP_DUMMY_VALUE );

      grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_COMPLETE;

      if( grr_data->ctrl.parked_rrgrr_cr_ind EQ TRUE )
      {
        ctrl_handle_parked_rrgrr_cr_ind( GET_STATE( CTRL ) );
      }
      else
      {
        PALLOC( rrgrr_cr_req, RRGRR_CR_REQ );

        rrgrr_cr_req->cr_type = cr_type;

        if( grr_t_status( T3176 ) > 0 ) 
        {
          /*TRACE_ASSERT( posc_db NEQ NULL );*/

          rrgrr_cr_req->arfcn = grr_data->ctrl.old_arfcn;
          rrgrr_cr_req->bsic  = grr_data->ctrl.old_bsic;
        }
        else
        {
          T_NC_MVAL *nc_mval;
  
          nc_mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];

          rrgrr_cr_req->arfcn = nc_mval->arfcn;
          rrgrr_cr_req->bsic  = nc_mval->sync_info.bsic;
        }

        TRACE_EVENT_P3( "RRGRR_CR_REQ: %d %d %d",
                        rrgrr_cr_req->cr_type,
                        rrgrr_cr_req->arfcn,
                        rrgrr_cr_req->bsic );

        PSEND( hCommRR, rrgrr_cr_req );
      }
    }
    break;

    case( CR_COMPLETE ):
    {
      switch( grr_data->ctrl.rr_state.cr_ind_return )
      {
        case( CR_IND_RETURN_NONE ):
        {
          TRACE_EVENT( "RRGRR_CR_REQ: CR_IND_RETURN_NONE" );
        }
        break;

        case( CR_IND_RETURN_RESPONSE ):
        {
          PALLOC( cr_rsp, RRGRR_CR_RSP );

          TRACE_EVENT( "RRGRR_CR_RSP: CR_IND_RETURN_RESPONSE" );

          PSEND( hCommRR, cr_rsp );

          ctrl_stop_rr_task_req( GLBL_PCKT_MODE_NULL, TASK_STOP_DUMMY_VALUE );

          grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_COMPLETE;
        }
        break;

        case( CR_IND_RETURN_COMPLETE ):
        {
          PALLOC( rrgrr_cr_req, RRGRR_CR_REQ );

          rrgrr_cr_req->cr_type = cr_type;
          rrgrr_cr_req->arfcn = 0;
          rrgrr_cr_req->bsic  = 0;

          TRACE_EVENT_P1( "RRGRR_CR_REQ: %d", rrgrr_cr_req->cr_type );

          PSEND( hCommRR, rrgrr_cr_req );

          grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_NONE;
        }
        break;

        default:
        {
          TRACE_ASSERT
            ( grr_data->ctrl.rr_state.cr_ind_return EQ CR_IND_RETURN_NONE     OR 
              grr_data->ctrl.rr_state.cr_ind_return EQ CR_IND_RETURN_RESPONSE OR
              grr_data->ctrl.rr_state.cr_ind_return EQ CR_IND_RETURN_COMPLETE    );
        }
        break;
      }
    }
    break;

    case( CR_CS ):
    {
      ctrl_stop_rr_task_req( GLBL_PCKT_MODE_NULL, TASK_STOP_DUMMY_VALUE );
    }
    
    /*lint -fallthrough*/

    case( CR_CONT ):
    {
      PALLOC( rrgrr_cr_req, RRGRR_CR_REQ );

      rrgrr_cr_req->cr_type = cr_type;

      TRACE_EVENT_P1( "RRGRR_CR_REQ: %d", rrgrr_cr_req->cr_type );

      PSEND( hCommRR, rrgrr_cr_req );

      grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_COMPLETE;
    }
    break;

    default:
    {
      TRACE_ASSERT( cr_type EQ CR_NEW            OR
                    cr_type EQ CR_NEW_NOT_SYNCED OR
                    cr_type EQ CR_COMPLETE       OR
                    cr_type EQ CR_CONT           OR
                    cr_type EQ CR_CS                );
    }
    break;
  }
}/* ctrl_send_cell_reselection_req */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enable_serving_cell
+------------------------------------------------------------------------------
| Description : The function ctrl_enable_serving_cell() .... 
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_enable_serving_cell( UBYTE final )
{ 
  TRACE_FUNCTION( "ctrl_enable_serving_cell" );

  if( final )
  {
    ctrl_install_new_scell( );
  }

#ifdef REL99 
  /* Update SGSN release in the common library */
  cl_nwrl_set_sgsn_release(psc_db->sgsn_rel);
#endif

  ctrl_send_cell_ind();

  sig_ctrl_pg_start();                                                          

  sig_ctrl_cs_pim();

  sig_ctrl_cs_start( );

  sig_ctrl_meas_pim( );

  sig_ctrl_meas_start( );

  ctrl_tc_access_enable( sig_ctrl_tc_disable_prim_queue );

}/* ctrl_enable_serving_cell */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_install_new_scell
+------------------------------------------------------------------------------
| Description : The function ctrl_install_new_scell() .... 
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_install_new_scell( void )
{   
  TRACE_FUNCTION( "ctrl_install_new_scell" );  

  if( ctrl_get_gprs_service( ) NEQ GPRS_SERVICE_NONE )  
  {    
    sig_ctrl_cs_install_new_scell( );    
  }  

  sig_ctrl_cs_cc_result( TRUE );

}/* ctrl_install_new_scell */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_reinstall_old_scell_req
+------------------------------------------------------------------------------
| Description : The function ctrl_reinstall_old_scell_req() .... 
|               
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_reinstall_old_scell_req( BOOL is_pcco, UBYTE cause )
{ 
  TRACE_FUNCTION( "ctrl_reinstall_old_scell_req" );

  if( grr_get_db_mode( ) EQ DB_MODE_CC_REQ )
  {
    ctrl_stop_all_activities( CTRL_DC_OTHER, NULL );
  }
   
  ctrl_send_reconnect_dcch_req( cause );

}/* ctrl_reinstall_old_scell_req */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_reinstall_old_scell_cnf
+------------------------------------------------------------------------------
| Description : The function ctrl_reinstall_old_scell_cnf() .... 
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_reinstall_old_scell_cnf( BOOL is_success )
{ 
  TRACE_FUNCTION( "ctrl_reinstall_old_scell_cnf" );

  if( is_success )
  {
    ctrl_cc_rejected( );  
  }
  else
  {
    grr_set_db_ptr( DB_MODE_CC_REJ );
     
    /*
     * service PSI is still suspended, we have to stop it finally 
     */
    sig_ctrl_psi_stop( );
    /* grr_set_pbcch( FALSE ); */
  }
}/* ctrl_reinstall_old_scell_cnf */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_cell_ind
+------------------------------------------------------------------------------
| Description : ctrl_send_cell_ind: composes and sends a GMMRR_CELL_IND message
|
| Parameters  : void 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_cell_ind()
{
  TRACE_FUNCTION( "ctrl_send_cell_ind" );
  {
    PALLOC(gmmrr_cell_ind, GMMRR_CELL_IND);
    
    grr_set_cell_info_service( );

    memcpy( &gmmrr_cell_ind->cell_info,
            &psc_db->cell_info_for_gmm.cell_info,
             sizeof( T_cell_info ) );
    
#if !defined (NTRACE)

    if( grr_data->cs.v_crp_trace EQ TRUE )
    {
      TRACE_EVENT_P9( "CELL_ENV: %x%x%x %x%x%x %x %x %x",
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[0],
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[1],
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mcc[2],
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[0],
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[1],
                      gmmrr_cell_ind->cell_info.cell_env.rai.plmn.mnc[2],
                      gmmrr_cell_ind->cell_info.cell_env.rai.lac,
                      gmmrr_cell_ind->cell_info.cell_env.rai.rac,
                      gmmrr_cell_ind->cell_info.cell_env.cid );
    }

#endif /* #if !defined (NTRACE) */
    grr_data->cell_res_status = FALSE;
    PSEND(hCommGMM, gmmrr_cell_ind);
  }
}/*ctrl_send_cell_ind */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_init_params
+------------------------------------------------------------------------------
| Description : ctrl_init_params initializes the parameters of the CTRL service
|
| Parameters  : void 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_init_params()
{
  TRACE_FUNCTION( "ctrl_init_params" );
  
  grr_data->ctrl.imm_rel             = 0;
  grr_data->cc_running               = FALSE;
  grr_data->pcco_failure_cause       = 8;

}/* ctrl_init_params */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_copy_pbcch_des_from_ncell
+------------------------------------------------------------------------------
| Description : Copies pbcch description from the ncell description to a 
|               specific database
|
| Parameters  : void 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_copy_pbcch_des_from_ncell( T_SC_DATABASE *db )
{
  T_NC_MVAL    *nc_mval;
  T_ncell_info *ncell_info;
  
  nc_mval    = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];
  ncell_info =  grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

  TRACE_FUNCTION( "ctrl_copy_pbcch_des_from_ncell" );

  if( ncell_info NEQ NULL )
  {
    db->pbcch.pbcch_des.pb      = 0;
    db->pbcch.pbcch_des.tn      = ncell_info->cr_par.si13_pbcch.pbcch_location;

    /*
     * for broadcast and common control channels, the TSC must be equal to BCC
     */
    db->pbcch.pbcch_des.tsc     = ncell_info->bsic & BCC_OF_BSIC;

    /*
     * to indicate that the arfcn of the BCCH carrier should be used
     */
    db->pbcch.pbcch_des.flag    = 0;
    db->pbcch.pbcch_des.v_flag2 = 0;
    db->pbcch.pbcch_des.flag2   = 0;

    db->pbcch.bcch.arfcn         = ncell_info->arfcn;
    db->pbcch.bcch.bsic          = ncell_info->bsic;
    db->pbcch.bcch.pbcch_present = TRUE;

    db->pccch.bs_pbcch_blks     = 0;

    db->psi1_params.psi1_repeat_period 
                                = ncell_info->cr_par.si13_pbcch.psi1_repeat_period;
  }
  else
  {
    TRACE_ERROR( "ctrl_copy_pbcch_des_from_ncell: ncell_info EQ NULL" );
  }
}/*ctrl_copy_pbcch_des_from_ncell*/


/*
+------------------------------------------------------------------------------
| Function    : ctrl_sync_to_given_cell()
+------------------------------------------------------------------------------
| Description : This function starts synchronisation to the given cell 
|               neighbour cell
|
| Parameters  : void 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_sync_to_given_cell ( void )
{
  T_NC_MVAL    *nc_mval;
  T_ncell_info *ncell_info;
  
  TRACE_FUNCTION( "ctrl_sync_to_given_cell" );

  nc_mval    = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];
  ncell_info =  grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

  if( ncell_info NEQ NULL )
  {
    ctrl_stop_rr_task_req( GLBL_PCKT_MODE_SYNC, TASK_STOP_DUMMY_VALUE );

    grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_COMPLETE;

    if( grr_data->ctrl.parked_rrgrr_cr_ind EQ TRUE )
    {
      ctrl_handle_parked_rrgrr_cr_ind( GET_STATE( CTRL ) );
    }
    else
    {
      PALLOC( rrgrr_sync_req, RRGRR_SYNC_REQ );

      rrgrr_sync_req->arfcn = ncell_info->arfcn;
      rrgrr_sync_req->bsic  = ncell_info->bsic;

      PSEND( hCommRR, rrgrr_sync_req );
    }

  }
  else
  {
    TRACE_ERROR( "ctrl_sync_to_given_cell: ncell_info EQ NULL" );
  }
} /* ctrl_sync_to_given_cell */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_stop_T3174 
+------------------------------------------------------------------------------
| Description : The function stops T3174 when running.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_stop_T3174 ( void )
{
  TRACE_FUNCTION( "ctrl_stop_T3174" );

  TRACE_EVENT( "T3174 for packet cell change stopped" );

  vsi_t_stop( GRR_handle, T3174 );

} /* ctrl_stop_T3174  */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_access_disable_if_needed()
+------------------------------------------------------------------------------
| Description : The function sends access disabled signal to TC, if needed
|
| Parameters  : dc - disable cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_access_disable_if_needed 
                                   ( T_TC_DISABLE_CAUSE         disable_cause,
                                     T_TC_DISABLE_CALLBACK_FUNC callback_func )
{
  TRACE_FUNCTION( "ctrl_send_access_disable_if_needed" );
  
  psc_db->is_access_enabled = FALSE;

  sig_ctrl_tc_access_disable( disable_cause, callback_func );

} /* ctrl_send_access_disable_if_needed() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_copy_cell_env()
+------------------------------------------------------------------------------
| Description : 
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_copy_cell_env( T_cell_env          *cell_env,
                                T_serving_cell_info *scell_info )
{
  TRACE_FUNCTION( "ctrl_copy_cell_env" );

  psc_db->pbcch.bcch.arfcn                 = scell_info->bcch_arfcn;
  psc_db->pbcch.bcch.bsic                  = scell_info->bcch_bsic;

  cell_env->rai.plmn = scell_info->rr_cell_env.plmn;
  cell_env->rai.lac  = scell_info->rr_cell_env.lac;
  cell_env->cid      = scell_info->rr_cell_env.cid;

#ifdef _SIMULATION_

  TRACE_EVENT_P2( "ctrl_copy_cell_env - SCELL: %d %d",
                  psc_db->pbcch.bcch.arfcn, psc_db->pbcch.bcch.bsic );

#endif /* #ifdef _SIMULATION_ */

} /* ctrl_copy_cell_env */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_copy_scell_params()
+------------------------------------------------------------------------------
| Description : This function copies some serving cell specific parameters into 
|               database of GRR
|
| Parameters  : sc_inf: pointer to serving cell information
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ctrl_copy_scell_params( T_serving_cell_info *sc_inf )
{
  BOOL ba_bcch_changed = FALSE;

  TRACE_FUNCTION("ctrl_copy_scell_params");

  psc_db->scell_par.cr_par_1.cr_pow_par.gprs_ms_txpwr_max_cch 
                                           = sc_inf->gprs_ms_txpwr_max_cch;
  psc_db->prach.ac_class                   = sc_inf->ac_class;
  
  ctrl_copy_cell_env( &psc_db->cell_info_for_gmm.cell_info.cell_env, sc_inf );

  grr_data->meas.pwr_offset                = sc_inf->pwr_offset;
  psc_db->net_ctrl.bs_pa_mfrms             = sc_inf->bs_pa_mfrms;

  if(sc_inf->arfcn[0] EQ RRGRR_INVALID_ARFCN) 
  {
    TRACE_EVENT("No BA list sent with SI13 info");
    return ba_bcch_changed;
  }
#if defined (REL99) AND defined (TI_PS_FF_EMR)
/* Copy ba_ind of the NC list */
  if(sc_inf->ba_ind NEQ psc_db->ba_ind)
  {
    grr_init_ba_bcch_nc_list(psc_db);
    psc_db->ba_ind = sc_inf->ba_ind;
  }
#endif

  if( grr_t_status( T3176 ) > 0 ) 
  {
    TRACE_EVENT
      ( "ctrl_copy_scell_params: packet cell change order - do not delete psc_db->nc_cw.list.number" );
  }
  else
  {
    UBYTE i = 0; /* used for counting */

    while( i                  <   RRGRR_BA_LIST_SIZE  AND
           i                  <   MAX_NR_OF_NCELL     AND
           sc_inf->arfcn[i]   NEQ RRGRR_INVALID_ARFCN     )
    {
      if( psc_db->nc_cw.list.info[i].index NEQ i                  OR
          psc_db->nc_cw.list.info[i].arfcn NEQ sc_inf->arfcn[i]   OR 
          psc_db->nc_cw.list.info[i].bsic  NEQ RRGRR_INVALID_BSIC    )
      {
        psc_db->nc_cw.list.info[i].index = i;
        psc_db->nc_cw.list.info[i].arfcn = sc_inf->arfcn[i];
        psc_db->nc_cw.list.info[i].bsic  = RRGRR_INVALID_BSIC;

        NC_SET_TYPE    ( psc_db->nc_cw.list.info[i].info_src, INFO_TYPE_BA_BCCH );
        NC_SET_INSTANCE( psc_db->nc_cw.list.info[i].info_src, 0                 );
        NC_SET_NUMBER  ( psc_db->nc_cw.list.info[i].info_src, 0                 );

        ba_bcch_changed = TRUE;
      }

      i++;
    }
    
    if( psc_db->nc_cw.list.number NEQ i )
    {
      psc_db->nc_cw.list.number = i;
      ba_bcch_changed           = TRUE;
    }
  }

  grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param = TRUE;
  grr_data->pwr_ctrl_valid_flags.v_freq_param          = TRUE;
  
  return( ba_bcch_changed );

}/* ctrl_copy_scell_params */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cell_change_order
+------------------------------------------------------------------------------
| Description : Start processing packet cell change order or cell change order 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_cell_change_order ( T_CTRL_CCO_TYPE type, void *data )
{
  T_NC_MVAL *nc_mval;

  TRACE_FUNCTION("ctrl_cell_change_order");
  TRACE_EVENT("ctrl_cell_change_order_started");/*to show the trace on target*/

  switch( type )
  {
    case( CCO_TYPE_RR ):
      {
        T_D_CHANGE_ORDER *d_change_order = (T_D_CHANGE_ORDER *)data;

        SET_STATE(CTRL,CTRL_RR_NC_CCO); /* set network commanded cell change order */

        ctrl_stop_all_activities( CTRL_DC_CR_NW_RR, d_change_order );

        grr_init_db( pcr_db );

        /* process BCCH ARFCN and BSIC */
        pcr_db->pbcch.bcch.arfcn =
                       ((USHORT)d_change_order->cell_desc.bcch_arfcn_hi << 8) +
                        (USHORT)d_change_order->cell_desc.bcch_arfcn_lo;
        pcr_db->pbcch.bcch.bsic  =
                               (d_change_order->cell_desc.ncc << 3) +
                                d_change_order->cell_desc.bcc;  

        /* process state variable */
        grr_data->cc_running = TRUE;

        vsi_t_start(GRR_handle,T3134,T3134_VALUE);
      }
      break;
  
    case( CCO_TYPE_GRR ):
      {
        T_D_CELL_CHAN_ORDER *d_cell_chan_order = ( T_D_CELL_CHAN_ORDER *)data;

        grr_data->ctrl.pcc_arfcn = d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.arfcn;
        grr_data->ctrl.pcc_bsic  = d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.bsic;;
        grr_data->ctrl.old_arfcn = psc_db->pbcch.bcch.arfcn;
        grr_data->ctrl.old_bsic = psc_db->pbcch.bcch.bsic ;

        TRACE_EVENT_P2("ctrl_cell_change_order:commanded cell is arfcn:%d ,bsic:%d",
          d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.arfcn, 
          d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.bsic);

        if( grr_data->cs.gmm_state EQ STANDBY_STATE )
        {
          TRACE_EVENT("ctrl_cell_change_order:GMM in STANDBY state");
          grr_data->pcco_failure_cause = 5;/*MS in GMM standby mode*/
          ctrl_build_pcco_failure_msg(grr_data->ctrl.pcc_arfcn, grr_data->ctrl.pcc_bsic);
  
        }
        else
        if (ctrl_check_cco_freq(grr_data->ctrl.pcc_arfcn))
        {
          SET_STATE( CTRL, CTRL_GRR_NC_CCO );
          ctrl_start_T3174( );
          sig_ctrl_cs_check_cell_location
              ( d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.arfcn,
                d_cell_chan_order->tgt_cell_gsm_info.gsm_target_cell.bsic);
        
          nc_mval = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];

          /* ncell_info = grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic ); */

          grr_data->ctrl.imm_rel   = d_cell_chan_order->tgt_cell_gsm_info.im_rel_c0;
       
          ctrl_stop_all_activities( CTRL_DC_CR_NW_GRR, d_cell_chan_order );

          grr_init_db( pcr_db );
       
          pcr_db->pbcch.bcch.arfcn = nc_mval->arfcn;
          pcr_db->pbcch.bcch.bsic  = nc_mval->sync_info.bsic;
        }
        else
        {
          TRACE_EVENT("ctrl_cell_change_order:frequency not implemented");
          grr_data->pcco_failure_cause = 0;/*Freq_Not_Impl*/
          ctrl_build_pcco_failure_msg(grr_data->ctrl.pcc_arfcn, grr_data->ctrl.pcc_bsic);
        }
      }  
      break;
  }
}/* ctrl_cell_change_order */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_reconnect_dcch_req
+------------------------------------------------------------------------------
| Description : This is only to be sent when cell change failure occures
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_reconnect_dcch_req ( UBYTE cause )
{
  TRACE_FUNCTION( "ctrl_send_reconnect_dcch_req" );

  {
    PALLOC( rrgrr_reconnect_dcch_req, RRGRR_RECONNECT_DCCH_REQ );

    rrgrr_reconnect_dcch_req->reconn_cause = cause;

    PSEND( hCommRR, rrgrr_reconnect_dcch_req );
  }
}/* ctrl_send_reconnect_dcch_req */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_si13_received
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_si13_received( T_RRGRR_GPRS_SI13_IND * rrgrr_si13_ind )
{
  BOOL ba_bcch_changed;
  
  TRACE_FUNCTION( "ctrl_si13_received" );

  ctrl_set_gprs_service( rrgrr_si13_ind->cause,
                         rrgrr_si13_ind->serving_cell_info.limited );

  ba_bcch_changed = ctrl_copy_scell_params( &rrgrr_si13_ind->serving_cell_info );

  if( rrgrr_si13_ind->cause EQ GPRS_SUPPORTED )
  {
    sig_ctrl_psi_si13_received( rrgrr_si13_ind, ba_bcch_changed );
  }

  grr_cgrlc_pwr_ctrl_req( FALSE );

}/* ctrl_si13_received */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cc_rejected
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_cc_rejected ( void )
{
  TRACE_FUNCTION( "ctrl_cc_rejected" );

  grr_set_db_ptr( DB_MODE_CC_REJ );
  
  if( grr_data->ctrl.parked_rrgrr_cr_ind EQ TRUE )
  {
    sig_ctrl_cs_cc_result( TRUE );
  }
  else
  {
    sig_ctrl_cs_cc_result( FALSE );
  }
}/* ctrl_cc_rejected */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_failure_processing
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_failure_processing ( T_FAILURE_SIGNAL fail_sig )
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_FUNCTION( "ctrl_failure_processing" );

  TRACE_EVENT_P1( "ctrl_failure_processing: failure signal %d", fail_sig );

  switch( state )
  {
    case CTRL_READY_TO_PROGRESS:
      if (grr_data->db.old_scell.pbcch_present EQ TRUE)
      {
        grr_set_db_ptr( DB_MODE_CC_REJ );
        sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_NORMAL );
      }
      else
      {
        /* The old cell is without PBBCH */
        ctrl_send_cell_reselection_req( CR_CONT );
      }
      break;

    case CTRL_READY:
    case CTRL_ACCESS_DISABLED:
    case CTRL_IN_PROGRESS:   
      if (fail_sig EQ FAIL_SIG_DSF_IND)
      {
        sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_DL_SIG_FAIL );
      }
      else
      {
      sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_NORMAL );
      }
      break;

    case CTRL_RR_CR_IN_SUSP:
    case CTRL_AUTO_RR_CR:
      ctrl_send_cell_reselection_req( CR_CONT );
      break;

    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      SET_STATE_FAILED_CR( state );

      ctrl_cc_rejected( );
      break;

    case CTRL_RR_NC_CCO:
      SET_STATE( CTRL, CTRL_IN_PROGRESS );

      ctrl_reinstall_old_scell_req( FALSE, RECONN_NO_ACTIVITY );
      break;
    case CTRL_GRR_NC_CCO:
      if ( grr_t_status(T3174) > 0) 
      { 
        ctrl_stop_T3174();
      }
      ctrl_pcco_failure( );
      break;

    default:
      TRACE_ERROR( "ctrl_failure_processing: failure signal unexpected" );
      break;
  }
} /* ctrl_failure_processing */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_pcco_failure
+------------------------------------------------------------------------------
| Description : Handles the internal signal ctrl_pcco_failure
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void ctrl_pcco_failure(void)
{
  TRACE_ISIG( "ctrl_pcco_failure" );

  vsi_t_start( GRR_handle, T3176, T3176_VALUE );

  TRACE_EVENT( "ctrl_pcco_failure:T3176 started." );
    
  ctrl_stop_all_activities( CTRL_DC_CR_NW_GRR, NULL );

} /* ctrl_pcoo_failure */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_build_pcco_failure_msg
+------------------------------------------------------------------------------
| Description : The function is to send packet cell change failure message
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_build_pcco_failure_msg(USHORT arfcn, UBYTE bsic)
{
  TRACE_FUNCTION( "ctrl_build_pcco_failure_msg" );
  TRACE_EVENT("ctrl_build_pcco_failure_msg");
  {         
     T_U_CELL_CHAN_FAILURE u_cell_chan_failure;

     u_cell_chan_failure.msg_type = U_CELL_CHAN_FAILURE_c;

     grr_set_buf_tlli( &u_cell_chan_failure.tlli_value, grr_get_tlli( ) );

     u_cell_chan_failure.arfcn         = arfcn;
     u_cell_chan_failure.bsic          = bsic;
     u_cell_chan_failure.failure_cause = grr_data->pcco_failure_cause;

     TRACE_EVENT_P2("ctrl_build_pcco_failure_msg: pcc_arfcn = %1d,pcc_bsic = %1d", arfcn,bsic);

     ctrl_send_control_block( CGRLC_BLK_OWNER_CTRL, (void*)(&u_cell_chan_failure) );
  }
} /*ctrl_build_pcco_failure_msg */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_control_block
+------------------------------------------------------------------------------
| Description : This function is used to signal service TC the request for
|               sending a control block.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_control_block ( T_BLK_OWNER   blk_owner, 
                                      void        * blk_struct )
{

  TRACE_FUNCTION( "ctrl_send_control_block" );
  TRACE_EVENT( "ctrl_send_control_block" );

  switch( blk_owner )
  {
    case( CGRLC_BLK_OWNER_MEAS ):
    case( CGRLC_BLK_OWNER_CS   ):
      /*
       * If the timers related to measurement reporting expire while the
       * reselection procedure has not yet been completed, these timers
       * shall be restarted so that the mobile station resumes measurement
       * reporting procedures once camped on the new cell 
       */
      if( grr_t_status( T3174 ) > 0 OR grr_t_status( T3176 ) > 0 )
      {
        ctrl_send_control_block_result( blk_owner, FALSE );

        TRACE_ERROR( "ctrl_send_control_block: measurement reporting not allowed" );
      }
      else
      {
        sig_ctrl_tc_control_block( blk_owner, blk_struct );
      }
      break;

    default:
      sig_ctrl_tc_control_block( blk_owner, blk_struct );
      break;
  }
} /* ctrl_send_control_block */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cancel_control_block
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_cancel_control_block ( T_BLK_OWNER blk_owner )
{
  TRACE_FUNCTION( "ctrl_cancel_control_block" );

  sig_ctrl_tc_cancel_control_block( blk_owner );

} /* ctrl_cancel_control_block */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_control_block_result
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_control_block_result ( T_BLK_OWNER blk_owner,
                                             BOOL        is_success )
{
  TRACE_FUNCTION( "ctrl_send_control_block_result" );

  switch( blk_owner )
  {
    case( CGRLC_BLK_OWNER_MEAS ):
      if( is_success ) sig_ctrl_meas_pmr_accept( );
      else             sig_ctrl_meas_pmr_reject( );
      break;

    case( CGRLC_BLK_OWNER_CS ):
      if( is_success ) sig_ctrl_cs_pmr_accept( );
      else             sig_ctrl_cs_pmr_reject( );
      break;

    case( CGRLC_BLK_OWNER_CTRL ):
      vsi_t_stop( GRR_handle, T3176 );
      TRACE_EVENT("ctrl_send_control_block_result:T3176 stopped");
      break;

    default:
      TRACE_ERROR( "ctrl_send_control_block_result: blk_owner out of range" );
      break;
  }
} /* ctrl_send_control_block_result */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_resp_resume
+------------------------------------------------------------------------------
| Description : The function ctrl_resp_resume() .... 
|               
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_resp_resume( void )
{
  psc_db->is_access_enabled = TRUE;

  ctrl_send_cell_ind();
  
  sig_ctrl_pg_start();

  sig_ctrl_psi_resumpt();

  sig_ctrl_cs_pim( );

  sig_ctrl_cs_start( );

  sig_ctrl_meas_pim( );

  sig_ctrl_meas_resume( );

  ctrl_tc_access_enable( sig_ctrl_tc_disable_prim_queue );

}/* ctrl_resp_resume */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_handle_new_candidate
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_handle_new_candidate( BOOL initial )
{
  UBYTE         state;
  T_NC_MVAL    *nc_mval;
  T_ncell_info *ncell_info;

  TRACE_FUNCTION( "ctrl_handle_new_candidate" );

  grr_init_db( pcr_db );

  state = GET_STATE( CTRL );
    
  switch( state )
  {
    case CTRL_CR:
    case CTRL_FAILED_CR:
    case CTRL_GRR_NC_CCO:
      SET_STATE( CTRL, CTRL_CR );
      break;

    case CTRL_CR_IN_SUSP:
    case CTRL_FAILED_CR_IN_SUSP:
      SET_STATE( CTRL, CTRL_CR_IN_SUSP );
      break;

    default:
      TRACE_EVENT_P1( "ctrl_handle_new_candidate: unexpected state %d", state );
      break;
  }

  nc_mval    = &grr_data->db.nc_mval_list.nc_mval[grr_data->db.cr_cell->idx];
  ncell_info =  grr_get_ncell_info( nc_mval->arfcn, nc_mval->sync_info.bsic );

  /* 
   * Store ARFCN and BSIC of the cell re-selection candidate,
   * these parameters should be used to avoid considering RRGRR_SI13_IND
   * and RRGRR_GPRS_IND primitives which maybe present in the data queue
   * of GRR but belong to the old serving cell.
   */
  pcr_db->pbcch.bcch.arfcn = nc_mval->arfcn;
  pcr_db->pbcch.bcch.bsic  = nc_mval->sync_info.bsic;

  if( ncell_info NEQ NULL )
  {

#ifdef _SIMULATION_

    if(  ncell_info->cr_par.v_si13_pbcch               AND
        !ncell_info->cr_par.si13_pbcch.v_si13_location     )
    {
      /*
       * Copy pbcch description from the ncell description to pcr_db
       */
      ctrl_copy_pbcch_des_from_ncell( pcr_db );

      /* 
       * We have to synchronize to the new cell to be able 
       * to read full PSI in the new cell 
       */
      ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW_PBCCH_INFO, initial );
    }
    else

#endif /* #ifdef _SIMULATION_ */

    {
      ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW, initial );
    }
  }
  else
  {
    ctrl_start_cell_reselection( CTRL_CR_TYPE_NEW, initial );
  }
} /* ctrl_handle_new_candidate */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_handle_no_more_candidate
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_handle_no_more_candidate( T_CTRL_CR_TYPE ctrl_cr_type,
                                           BOOL           initial )
{
  TRACE_FUNCTION( "ctrl_handle_no_more_candidate" );

  SET_STATE( CTRL, CTRL_READY );

  /* grr_set_db_ptr( DB_MODE_INIT ); */
  ctrl_start_cell_reselection( ctrl_cr_type, initial );

} /* ctrl_handle_no_more_candidate */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_start_rr_task
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_start_rr_task ( UBYTE state )
{
  TRACE_FUNCTION( "ctrl_start_rr_task" );

  if( state EQ TASK_STATE_PTM OR grr_is_pbcch_present( ) )
  {
    PALLOC( rrgrr_start_task_req, RRGRR_START_TASK_REQ );

    rrgrr_start_task_req->state = state;

    PSEND( hCommRR, rrgrr_start_task_req );
  }
} /* ctrl_start_rr_task */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_stop_rr_task_req
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_stop_rr_task_req
                              ( T_GLBL_PCKT_MODE final_pckt_mode, UBYTE param )
{
  BOOL        pbcch_present = grr_is_pbcch_present( );
  BOOL        change_pckt_mode;
  UBYTE       curr_pckt_mode;
  T_CTRL_TASK ctrl_task;

  TRACE_FUNCTION( "ctrl_stop_rr_task_req" );

  change_pckt_mode = ctrl_set_transient_pckt_mode( final_pckt_mode, &ctrl_task );
  curr_pckt_mode   = GET_STATE( CTRL_GLBL );

  if( change_pckt_mode )
  {
    if(   curr_pckt_mode EQ GLBL_PCKT_MODE_IDLE_ASSIGNED         OR
          curr_pckt_mode EQ GLBL_PCKT_MODE_IDLE_ACCESS           OR 
        ( curr_pckt_mode EQ GLBL_PCKT_MODE_TRANSFER_ACCESS AND 
          !pbcch_present                                       ) OR
          curr_pckt_mode EQ GLBL_PCKT_MODE_RELEASED_ACCESS          )
    {
      ctrl_leave_to_pam( );
    }

    if( ctrl_task     EQ CTRL_TASK_PBCCH_LEAVE_NULL AND 
        pbcch_present EQ TRUE                           )
    {
      grr_data->ctrl_glbl.ctrl_task = ctrl_task;

      sig_ctrl_psi_rel_state( REL_PERFORMED );
      ctrl_stop_rr_task_cnf( TRUE );
    }
    else if( ctrl_task EQ CTRL_TASK_PBCCH_LEAVE_SUSP )
    {
      grr_data->ctrl_glbl.ctrl_task = ctrl_task;

      ctrl_stop_rr_task_cnf( TRUE );
    }
    else if( pbcch_present )
    {
      if( ctrl_task NEQ CTRL_TASK_NONE             AND 
          ctrl_task NEQ CTRL_TASK_PBCCH_LEAVE_SYNC     )
      {
        ctrl_send_rrgrr_stop_task_req( ctrl_task, param );
      }
      else
      {
        ctrl_stop_rr_task_cnf( change_pckt_mode );
      }
    }
    else
    {
      ctrl_stop_rr_task_cnf( change_pckt_mode );
    }
  }
  else
  {
    if     ( curr_pckt_mode EQ GLBL_PCKT_MODE_ACCESS )
    {
      ctrl_enter_pam( );
    }
    else if( curr_pckt_mode EQ GLBL_PCKT_MODE_ACCESS_TWO_PHASE )
    {
      sig_ctrl_pg_mode_ind( PACKET_MODE_2P_PTM );
    }
    else if( curr_pckt_mode EQ GLBL_PCKT_MODE_MEAS_REP_IDLE )
    {
      sig_ctrl_pg_mode_ind( PACKET_MODE_PIM );
    }
    else if( curr_pckt_mode EQ GLBL_PCKT_MODE_SUSP )
    {
      ctrl_start_rr_task( TASK_STATE_PIM );
    }

    ctrl_stop_rr_task_cnf( change_pckt_mode );
  }
} /* ctrl_stop_rr_task_req */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_stop_rr_task_cnf
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_stop_rr_task_cnf ( BOOL change_pckt_mode )
{
  TRACE_FUNCTION( "ctrl_stop_rr_task_cnf" );

  ctrl_set_final_pckt_mode( );

  if( change_pckt_mode )
  {
    T_GLBL_PCKT_MODE pckt_mode = GET_STATE( CTRL_GLBL );

    if( pckt_mode EQ GLBL_PCKT_MODE_IDLE )
    {
      if     ( grr_data->ctrl_glbl.ctrl_task EQ  CTRL_TASK_PBCCH_LEAVE_SUSP )
      {
        ctrl_resp_resume( );
      }
      else if( grr_data->ctrl_glbl.ctrl_task EQ  CTRL_TASK_PBCCH_LEAVE_SYNC )
      {
        /* 
         * just set the correct state,
         * detailed processing will be performed by the calling function
         */
      }
      else if( grr_data->ctrl_glbl.ctrl_task NEQ CTRL_TASK_PBCCH_LEAVE_NULL )
      {
        ctrl_enter_pim( );
        ctrl_start_rr_task( TASK_STATE_PIM );
      }
    } 
    else if( pckt_mode EQ GLBL_PCKT_MODE_ACCESS )
    {
      ctrl_enter_pam( );
 
      if( grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_NULL )
      {
        ctrl_start_rr_task( TASK_STATE_PAM );
      }
      else
      {
        TRACE_EVENT( "No task start for PAM, DL assignment received" );
      }
    }
    else if( pckt_mode EQ GLBL_PCKT_MODE_ASSIGNED )
    {
      if( grr_data->tc.last_eval_assign.ccch_read EQ DONT_STOP_CCCH )
      {
        sig_ctrl_pg_mode_ind( PACKET_MODE_PAM );
      }

      sig_ctrl_tc_leave_to_ptm( );
    }
    else if( pckt_mode EQ GLBL_PCKT_MODE_TRANSFER )
    {
      ctrl_enter_ptm( );
      ctrl_start_rr_task( TASK_STATE_PTM );
    }
    else if( pckt_mode EQ GLBL_PCKT_MODE_RELEASED )
    {
      sig_ctrl_tc_leave_to_pim( );
    }
  }

  grr_data->ctrl_glbl.ctrl_task = CTRL_TASK_NONE;
} /* ctrl_stop_rr_task_cnf */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_rrgrr_stop_task_req
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_rrgrr_stop_task_req ( T_CTRL_TASK ctrl_task, UBYTE param )
{
  PALLOC( rrgrr_stop_task_req, RRGRR_STOP_TASK_REQ );

  TRACE_FUNCTION( "ctrl_send_rrgrr_stop_task_req" );

  switch( ctrl_task )
  {
    case CTRL_TASK_PBCCH_LEAVE_IDLE:
      rrgrr_stop_task_req->ctrl_task        = LEAVE_PIM_PBCCH;
      rrgrr_stop_task_req->task.v_pim_pbcch = param;
      break;

    case CTRL_TASK_PBCCH_LEAVE_ACCESS:
      rrgrr_stop_task_req->ctrl_task        = LEAVE_PAM_PBCCH;
      rrgrr_stop_task_req->task.v_pam_pbcch = param;
      break;

    case CTRL_TASK_PBCCH_LEAVE_TRANSFER:
      rrgrr_stop_task_req->ctrl_task        = LEAVE_PTM_PBCCH;
      rrgrr_stop_task_req->task.v_ptm_pbcch = param;
      break;

    case CTRL_TASK_BCCH:
      rrgrr_stop_task_req->ctrl_task        = RR_TASK_1;
      rrgrr_stop_task_req->task.v_stop_ccch = param;
      break;

    default:
      PFREE( rrgrr_stop_task_req );

      TRACE_ERROR( "ctrl_send_rrgrr_stop_task_req: invalid control task" );
      TRACE_EVENT_P1( "FATAL ERROR: ctrl_send_rrgrr_stop_task_req: %d", 
                      ctrl_task );
      return;
  }

  /*
   *   Issue #22667, Patch FDS 28-04-04 - Fix for TIL 06 in spain
   *   Don't change the ctrl state if RR doesn't respond to the stop_task_req
   */
  
  if ( (ctrl_task NEQ CTRL_TASK_BCCH) OR (param NEQ INVALID_MSG) )
    grr_data->ctrl_glbl.ctrl_task = ctrl_task;

#ifdef _SIMULATION_

  TRACE_EVENT_P1( "ctrl_send_rrgrr_stop_task_req - ctrl_task: %d",
                  grr_data->ctrl_glbl.ctrl_task );

#endif /* #ifdef _SIMULATION_ */

  PSEND( hCommRR, rrgrr_stop_task_req );

} /* ctrl_send_rrgrr_stop_task_req */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_set_final_pckt_mode
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void ctrl_set_final_pckt_mode ( void )
{
  UBYTE pckt_mode = GET_STATE( CTRL_GLBL );

  TRACE_FUNCTION( "ctrl_set_final_pckt_mode" );

  switch( pckt_mode )
  {
    case( GLBL_PCKT_MODE_NULL_IDLE     ):
    case( GLBL_PCKT_MODE_SUSP_IDLE     ):
    case( GLBL_PCKT_MODE_SYNC_IDLE     ):
    case( GLBL_PCKT_MODE_ACCESS_IDLE   ):
    case( GLBL_PCKT_MODE_ASSIGNED_IDLE ):
    case( GLBL_PCKT_MODE_TRANSFER_IDLE ):
    case( GLBL_PCKT_MODE_RELEASED_IDLE ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_IDLE );
      break;

    case( GLBL_PCKT_MODE_IDLE_ACCESS      ):
    case( GLBL_PCKT_MODE_TRANSFER_ACCESS  ):
    case( GLBL_PCKT_MODE_RELEASED_ACCESS  ):
    case( GLBL_PCKT_MODE_ACCESS_TWO_PHASE ):
    case( GLBL_PCKT_MODE_MEAS_REP_IDLE ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS );
      break;

    case( GLBL_PCKT_MODE_IDLE_ASSIGNED ):
    case( GLBL_PCKT_MODE_ACCESS_ASSIGNED ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ASSIGNED );
      break;

    case( GLBL_PCKT_MODE_IDLE_TRANSFER     ):
    case( GLBL_PCKT_MODE_ASSIGNED_TRANSFER ):
    case( GLBL_PCKT_MODE_ACCESS_TRANSFER   ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_TRANSFER );
      break;

    case( GLBL_PCKT_MODE_TRANSFER_RELEASED ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_RELEASED );
      break;

    case( GLBL_PCKT_MODE_NULL     ):
    case( GLBL_PCKT_MODE_SUSP     ):
    case( GLBL_PCKT_MODE_SYNC     ):
    case( GLBL_PCKT_MODE_IDLE     ):
    case( GLBL_PCKT_MODE_ACCESS   ):
    case( GLBL_PCKT_MODE_ASSIGNED ):
    case( GLBL_PCKT_MODE_TRANSFER ):
    case( GLBL_PCKT_MODE_RELEASED ):
      /* do nothing */
      break;

    default: 
      TRACE_ERROR( "ctrl_set_final_pckt_mode: invalid transition packet mode" );
      TRACE_EVENT_P1( "FATAL ERROR: ctrl_set_final_pckt_mode: %d", pckt_mode );
      break;
  }
} /* ctrl_set_final_pckt_mode */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_set_transient_pckt_mode
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL ctrl_set_transient_pckt_mode 
                   ( T_GLBL_PCKT_MODE final_pckt_mode, T_CTRL_TASK *ctrl_task )
{
  BOOL  change_pckt_mode = TRUE;
  UBYTE curr_pckt_mode   = GET_STATE( CTRL_GLBL );
    
  TRACE_FUNCTION( "ctrl_set_transient_pckt_mode" );

  *ctrl_task = CTRL_TASK_NONE;

  switch( final_pckt_mode )
  {
    case( GLBL_PCKT_MODE_NULL ):      
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_NULL );

      change_pckt_mode = FALSE;
      break;

    case( GLBL_PCKT_MODE_SUSP ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_SUSP );
      
      change_pckt_mode = FALSE;
      break;

    case( GLBL_PCKT_MODE_SYNC ):
      SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_SYNC );

      change_pckt_mode = FALSE;
      break;

    case( GLBL_PCKT_MODE_IDLE ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_NULL ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_NULL_IDLE );

          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_NULL;
          break;

        case( GLBL_PCKT_MODE_SUSP ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_SUSP_IDLE );

          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_SUSP;
          break;

        case( GLBL_PCKT_MODE_SYNC ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_SYNC_IDLE );

          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_SYNC;
          break;

        case( GLBL_PCKT_MODE_IDLE ):
          change_pckt_mode = FALSE;
          break;

        case( GLBL_PCKT_MODE_ACCESS ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS_IDLE );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_ACCESS;
          break;

        case( GLBL_PCKT_MODE_ASSIGNED ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ASSIGNED_IDLE );
          break;

        case( GLBL_PCKT_MODE_TRANSFER ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_TRANSFER_IDLE );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_TRANSFER;
          break;

        case( GLBL_PCKT_MODE_RELEASED ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_RELEASED_IDLE );
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_IDLE );

          change_pckt_mode = FALSE;
          
          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    case( GLBL_PCKT_MODE_ACCESS ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_IDLE ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_IDLE_ACCESS );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_IDLE;
          break;

        case( GLBL_PCKT_MODE_ACCESS ):
          change_pckt_mode = FALSE;
          break;

        case( GLBL_PCKT_MODE_TRANSFER ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_TRANSFER_ACCESS );

          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_TRANSFER;
          break;

        case( GLBL_PCKT_MODE_RELEASED ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_RELEASED_ACCESS );
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d", 
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    case( GLBL_PCKT_MODE_ACCESS_TWO_PHASE ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_ACCESS ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS_TWO_PHASE );
          
          change_pckt_mode = FALSE;
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    case( GLBL_PCKT_MODE_MEAS_REP_IDLE ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_ACCESS ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_MEAS_REP_IDLE );
          
          change_pckt_mode = FALSE;
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;      

    case( GLBL_PCKT_MODE_ASSIGNED ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_IDLE ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_IDLE_ASSIGNED );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_IDLE;
          break;

        case( GLBL_PCKT_MODE_ACCESS ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS_ASSIGNED );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_ACCESS;
          break;

        case( GLBL_PCKT_MODE_ASSIGNED ):
          change_pckt_mode = FALSE;
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ASSIGNED );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    case( GLBL_PCKT_MODE_TRANSFER ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_IDLE ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_IDLE_TRANSFER );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_IDLE;
          break;

        case( GLBL_PCKT_MODE_ACCESS ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ACCESS_TRANSFER );
          
          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_ACCESS;
          break;

        case( GLBL_PCKT_MODE_ASSIGNED ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_ASSIGNED_TRANSFER );
          break;

        case( GLBL_PCKT_MODE_TRANSFER ):
          change_pckt_mode = FALSE;
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_TRANSFER );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    case( GLBL_PCKT_MODE_RELEASED ):
      switch( curr_pckt_mode )
      {
        case( GLBL_PCKT_MODE_TRANSFER ):
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_TRANSFER_RELEASED );

          *ctrl_task = CTRL_TASK_PBCCH_LEAVE_TRANSFER;
          break;

        case( GLBL_PCKT_MODE_RELEASED ):
          change_pckt_mode = FALSE;
          break;

        default: 
          SET_STATE( CTRL_GLBL, GLBL_PCKT_MODE_RELEASED );

          change_pckt_mode = FALSE;

          TRACE_EVENT_P2( "WARNING: ctrl_set_transient_pckt_mode: %d, %d",
                          curr_pckt_mode, final_pckt_mode );
          break;
      }
      break;

    default:
      change_pckt_mode = FALSE;

      TRACE_ERROR   ( "ctrl_set_transient_pckt_mode: invalid final packet mode" );
      TRACE_EVENT_P1( "FATAL ERROR: ctrl_set_transient_pckt_mode: %d",
                      final_pckt_mode );
      break;
  }

  return( change_pckt_mode );

} /* ctrl_set_transient_pckt_mode */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enter_pim
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void ctrl_enter_pim ( void )
{
  TRACE_FUNCTION( "ctrl_enter_pim" );

  /*
   * Reset TA-Values
   */
  memset(&grr_data->ta_params, 0xFF, sizeof(T_TA_PARAMS));
  grr_data->ta_params.ta_valid = FALSE;

  sig_ctrl_pg_mode_ind( PACKET_MODE_PIM );
  sig_ctrl_psi_pim( );
  sig_ctrl_meas_pim( );
  sig_ctrl_cs_pim( );

} /* ctrl_enter_pim */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_leave_to_pam
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void ctrl_leave_to_pam ( void )
{
  TRACE_FUNCTION( "ctrl_leave_to_pam" );

  sig_ctrl_meas_pam( );
  sig_ctrl_psi_pam( );
  sig_ctrl_cs_leave_to_pam( );

} /* ctrl_leave_to_pam */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enter_pam
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void ctrl_enter_pam ( void )
{
  TRACE_FUNCTION( "ctrl_enter_pam" );

  sig_ctrl_pg_mode_ind( PACKET_MODE_PAM );
  sig_ctrl_cs_enter_pam( );

  /*
   * PATCH 1 BEGIN 10.12.2002
   */

  /*
   * wait 2 ms after pccch_req, otherwise prach will not sent --> L1 BUG02724
   */
  vsi_t_sleep( VSI_CALLER 2 );

  /*
   * PATCH 1 END 10.12.2002
   */

  sig_ctrl_tc_enter_pam( );
} /* ctrl_enter_pam */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enter_ptm
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
LOCAL void ctrl_enter_ptm ( void )
{
  TRACE_FUNCTION( "ctrl_enter_ptm" );

  sig_ctrl_psi_ptm( );
  sig_ctrl_pg_mode_ind( PACKET_MODE_PTM );
  sig_ctrl_meas_ptm( );
  sig_ctrl_cs_ptm( );
} /* ctrl_enter_ptm */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_set_old_scell
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_set_old_scell ( BOOL service )
{
  TRACE_FUNCTION( "ctrl_set_old_scell" );

  ctrl_send_gmmrr_cr_ind( );

  if( service )
  {
    grr_data->db.old_scell.arfcn         = psc_db->pbcch.bcch.arfcn;
    grr_data->db.old_scell.bsic          = psc_db->pbcch.bcch.bsic;
    grr_data->db.old_scell.pbcch_present = psc_db->pbcch.bcch.pbcch_present;
  }
  else
  {
    grr_data->db.old_scell.arfcn         = RRGRR_INVALID_ARFCN;
    grr_data->db.old_scell.bsic          = RRGRR_INVALID_BSIC;
    grr_data->db.old_scell.pbcch_present = FALSE;
  }

  grr_data->ctrl.new_cell_responded     = FALSE;
  grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_RESPONSE;

#ifdef _SIMULATION_

  TRACE_EVENT_P2( "ctrl_set_old_scell - old SCELL: %d %d",
                  grr_data->db.old_scell.arfcn, grr_data->db.old_scell.bsic );

#endif /* #ifdef _SIMULATION_ */

} /* ctrl_set_old_scell( ) */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_gmmrr_cr_ind
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_gmmrr_cr_ind ( void )
{
  PALLOC( gmmrr_cr_ind, GMMRR_CR_IND );
  PSEND( hCommGMM, gmmrr_cr_ind );

  TRACE_FUNCTION( "ctrl_send_gmmrr_cr_ind" );
} /* ctrl_send_gmmrr_cr_ind( ) */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_start_cell_reselection 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_start_cell_reselection ( T_CTRL_CR_TYPE ctrl_cr_type,
                                          BOOL           initial )
{ 
  UBYTE cr_type = CR_CS;

  TRACE_FUNCTION( "ctrl_start_cell_reselection" );

  if( initial EQ TRUE )
  {
    ctrl_set_old_scell( TRUE );
  }

  switch( ctrl_cr_type )
  {
    case CTRL_CR_TYPE_CS            : /* nothing to do */          break;
    case CTRL_CR_TYPE_NEW           :
    case CTRL_CR_TYPE_NEW_PBCCH_INFO: cr_type = CR_NEW;            break;
    case CTRL_CR_TYPE_NEW_NOT_SYNCED: cr_type = CR_NEW_NOT_SYNCED; break;
    case CTRL_CR_TYPE_CONT          : cr_type = CR_CONT;           break;

    default: 
      TRACE_ASSERT( ctrl_cr_type EQ CTRL_CR_TYPE_CS             OR 
                    ctrl_cr_type EQ CTRL_CR_TYPE_NEW            OR
                    ctrl_cr_type EQ CTRL_CR_TYPE_NEW_PBCCH_INFO OR
                    ctrl_cr_type EQ CTRL_CR_TYPE_NEW_NOT_SYNCED OR
                    ctrl_cr_type EQ CTRL_CR_TYPE_CONT              );
      break;
  }

  if( ctrl_cr_type EQ CTRL_CR_TYPE_NEW_PBCCH_INFO )
  {
    ctrl_sync_to_given_cell( );
  }
  else
  {
    ctrl_send_cell_reselection_req( cr_type );
  }

  sig_ctrl_cs_start_cell_selection( cr_type ); 

} /* ctrl_start_cell_reselection() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cell_has_changed 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ctrl_cell_has_changed ( void )
{ 
#ifdef _SIMULATION_

  TRACE_EVENT_P5( "ctrl_cell_has_changed - %d - new SCELL: %d %d - old SCELL: %d %d",
                  grr_data->ctrl.new_cell_responded,
                  psc_db->pbcch.bcch.arfcn, 
                  psc_db->pbcch.bcch.bsic,
                  grr_data->db.old_scell.arfcn,
                  grr_data->db.old_scell.bsic );

#endif /* #ifdef _SIMULATION_ */

  return
    (   grr_data->ctrl.new_cell_responded EQ  FALSE                         AND 
      ( grr_data->db.old_scell.arfcn      NEQ psc_db->pbcch.bcch.arfcn OR
        grr_data->db.old_scell.bsic       NEQ psc_db->pbcch.bcch.bsic     )     );
} /* ctrl_cell_has_changed() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_tc_access_enable 
+------------------------------------------------------------------------------
| Description :
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_tc_access_enable( T_TC_ACCESS_ENABLE_FUNC access_enable_func )
{ 
  grr_data->ctrl.new_cell_responded |= 
                                access_enable_func( ctrl_cell_has_changed( ) );

#ifdef _SIMULATION_

  TRACE_EVENT_P5( "ctrl_tc_access_enable - %d - new SCELL: %d %d - old SCELL: %d %d",
                  grr_data->ctrl.new_cell_responded,
                  psc_db->pbcch.bcch.arfcn, 
                  psc_db->pbcch.bcch.bsic,
                  grr_data->db.old_scell.arfcn,
                  grr_data->db.old_scell.bsic );

#endif /* #ifdef _SIMULATION_ */

} /* ctrl_tc_access_enable() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_check_cco_freq 
+------------------------------------------------------------------------------
| Description : Check if the frequency in the Cell change order is capable of 
|               using
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
LOCAL BOOL ctrl_check_cco_freq ( USHORT arfcn )
{
  BOOL result = FALSE;

  TRACE_FUNCTION( "ctrl_check_cco_freq" );
  TRACE_EVENT( "ctrl_check_cco_freq started" );/*for target trace*/

  switch( std )
    {
      case STD_900: /* frequencies must between 1 and 124 */ 

        if(( arfcn >= LOW_CHANNEL_900 ) AND ( arfcn <= HIGH_CHANNEL_900))
        {
          result = TRUE;
        }
        break;

      case STD_EGSM:

        if(( arfcn <= HIGH_CHANNEL_900) OR
           (( arfcn >= LOW_CHANNEL_EGSM) AND ( arfcn <= HIGH_CHANNEL_EGSM - 1)))
        {
          result = TRUE;
        }
        break;

      case STD_1900:

        if(( arfcn >= LOW_CHANNEL_1900 ) AND ( arfcn <= HIGH_CHANNEL_1900)) 
        {
          result = TRUE;
        }
        break;

      case STD_1800:

        if(( arfcn >= LOW_CHANNEL_1800 ) AND ( arfcn <= HIGH_CHANNEL_1800)) 
        {
          result = TRUE;
        }        
        break;

      case STD_850:

        if(( arfcn >= LOW_CHANNEL_850 ) AND ( arfcn <= HIGH_CHANNEL_850)) 
        {
          result = TRUE;
        }     
        break;

      case STD_DUAL:

        if ((( arfcn >= LOW_CHANNEL_900 ) AND ( arfcn <= HIGH_CHANNEL_900)) OR
          (( arfcn >= LOW_CHANNEL_1800 ) AND ( arfcn <= HIGH_CHANNEL_1800))) 
        {     
          result = TRUE;
        }
        break;

      case STD_DUAL_EGSM:
 
        if((( arfcn <= HIGH_CHANNEL_900) OR
          (( arfcn >= LOW_CHANNEL_EGSM) AND ( arfcn <= HIGH_CHANNEL_EGSM - 1))) 
          OR     
          (( arfcn >= LOW_CHANNEL_1800 ) AND
          ( arfcn <= HIGH_CHANNEL_1800)))
        {
           result = TRUE;
        }
        break;

      case STD_DUAL_US:
        if ((( arfcn >= LOW_CHANNEL_850 ) AND ( arfcn <= HIGH_CHANNEL_850)) OR
         (( arfcn >= LOW_CHANNEL_1900 ) AND ( arfcn <= HIGH_CHANNEL_1900)))
        {
           result = TRUE;
        }
        break;

#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
      case STD_850_1800:
        if ((( arfcn >= LOW_CHANNEL_850 ) AND ( arfcn <= HIGH_CHANNEL_850)) OR
           (( arfcn >= LOW_CHANNEL_1800 ) AND ( arfcn <= HIGH_CHANNEL_1800)))
        {
           result = TRUE;
        }
        break;

      case STD_900_1900:
        if(( arfcn <= HIGH_CHANNEL_900) OR
          (( arfcn >= LOW_CHANNEL_EGSM) AND ( arfcn <= HIGH_CHANNEL_EGSM - 1)) OR
          (( arfcn >= LOW_CHANNEL_1900 ) AND ( arfcn <= HIGH_CHANNEL_1900)))
        {
           result = TRUE;
        }
        break;
        
      case STD_850_900_1800:
        if ((( arfcn >= LOW_CHANNEL_850 ) AND ( arfcn <= HIGH_CHANNEL_850)) OR
            ( arfcn <= HIGH_CHANNEL_900) OR
           (( arfcn >= LOW_CHANNEL_EGSM) AND ( arfcn <= HIGH_CHANNEL_EGSM - 1)) OR
           (( arfcn >= LOW_CHANNEL_1800 ) AND ( arfcn <= HIGH_CHANNEL_1800)))
        {
           result = TRUE;
        }
        break;

      case STD_850_900_1900:
        if ((( arfcn >= LOW_CHANNEL_850 ) AND ( arfcn <= HIGH_CHANNEL_850)) OR
            ( arfcn <= HIGH_CHANNEL_900) OR
           (( arfcn >= LOW_CHANNEL_EGSM) AND ( arfcn <= HIGH_CHANNEL_EGSM - 1)) OR
           (( arfcn >= LOW_CHANNEL_1900 ) AND ( arfcn <= HIGH_CHANNEL_1900)))
        {
           result = TRUE;
        }
        break;
#endif      

      default:
        break;
    }
  return(result);

} /* ctrl_check_cco_freq() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_get_gprs_service 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL T_GPRS_SERVICE ctrl_get_gprs_service ( void )
{
  TRACE_FUNCTION( "ctrl_get_gprs_service" );

  return( psc_db->cell_info_for_gmm.gprs_service );
} /* ctrl_get_gprs_service() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_set_gprs_service 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_set_gprs_service ( UBYTE cause, UBYTE limited )
{
  TRACE_FUNCTION( "ctrl_set_gprs_service" );

  switch( cause )
  {
    case GPRS_SUPPORTED:
      psc_db->cell_info_for_gmm.gprs_service = 
        ( limited EQ TRUE ? GPRS_SERVICE_LIMITED : GPRS_SERVICE_FULL );
      break;

    case GPRS_NOT_SUPPORTED:
    case GPRS_CELL_NOT_SUITABLE:
    default:
      psc_db->cell_info_for_gmm.gprs_service = GPRS_SERVICE_NONE;
      break;
  }

  grr_set_cell_info_service( );

} /* ctrl_set_gprs_service() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_send_gmmrr_suspend_cnf 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_send_gmmrr_suspend_cnf ( void )
{
  TRACE_FUNCTION( "ctrl_send_gmmrr_suspend_cnf" );

  {
    PALLOC( gmmrr_suspend_cnf, GMMRR_SUSPEND_CNF );
    PSEND( hCommGMM, gmmrr_suspend_cnf );
  }
} /* ctrl_send_gmmrr_suspend_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_handle_gmmrr_cs_page_ind 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_handle_gmmrr_cs_page_ind( UBYTE state )
{
  TRACE_FUNCTION( "ctrl_handle_gmmrr_cs_page_ind" );

  grr_data->ctrl.last_state = state;

  {
    PALLOC( gmmrr_cs_page_ind, GMMRR_CS_PAGE_IND );
    PSEND( hCommGMM, gmmrr_cs_page_ind );
  }
} /* ctrl_handle_gmmrr_cs_page_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_handle_parked_rrgrr_cr_ind 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_handle_parked_rrgrr_cr_ind( UBYTE state )
{
  UBYTE glbl_state = GET_STATE( CTRL_GLBL );

  TRACE_FUNCTION( "ctrl_handle_parked_rrgrr_cr_ind" );

  if( glbl_state EQ GLBL_PCKT_MODE_NULL OR
      glbl_state EQ GLBL_PCKT_MODE_SYNC OR 
      glbl_state EQ GLBL_PCKT_MODE_SUSP    )
  {
    /*
     * In this state the cell re-selection triggered by GRR
     * is already requested with a RRGRR_CR_REQ
     */
    if( state EQ CTRL_CR )
    {
      SET_STATE( CTRL, CTRL_AUTO_RR_CR );
    }
    else if( state EQ CTRL_CR_IN_SUSP )
    {
      SET_STATE( CTRL, CTRL_RR_CR_IN_SUSP );
    }
    else
    {
      TRACE_EVENT_P1( "ctrl_handle_parked_rrgrr_cr_ind in state %d", state );
    }
            
    ctrl_cc_rejected( );
    ctrl_set_old_scell( FALSE );
    ctrl_send_cell_reselection_req( CR_COMPLETE );

    grr_data->ctrl.parked_rrgrr_cr_ind = FALSE;
  }
} /* ctrl_handle_parked_rrgrr_cr_ind() */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_read_pcm
+------------------------------------------------------------------------------
| Description : The function ctrl_read_pcm read some info from ffs to decide
|               if nc2 should be switched on or off
|       
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void ctrl_read_pcm ( void )
{
#ifdef _TARGET_
  T_GRR_FFS GRR_ffsBuffer;
  T_FFS_SIZE size = ffs_fread("/GPRS/grr", &GRR_ffsBuffer, sizeof(GRR_ffsBuffer));
#endif

  TRACE_FUNCTION( "ctrl_read_pcm" );

  grr_data->nc2_on = TRUE;
#ifdef _TARGET_
  if ( size != sizeof(GRR_ffsBuffer) )
  {
    if ( size < 0 )
    {
      TRACE_EVENT_P1("FFS can not be read \"/GPRS/grr\" (%d)", size);
    }
    else
    {
      TRACE_EVENT_P2("FFS contains old file of \"/GPRS/grr\": %dBytes long, but %dBytes expected",
                      size, sizeof(GRR_ffsBuffer));
    }
  }
  else
  {
    grr_data->nc2_on = GRR_ffsBuffer.nc2_on;
  }
#endif
  TRACE_EVENT_P1("ctrl_read_pcm: grr_data->nc2_on is %d",grr_data->nc2_on);
  return;
}

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enter_standby_state 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_enter_standby_state( void )
{
  TRACE_FUNCTION( "ctrl_enter_standby_state" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_NULL:
      TRACE_ERROR( "ctrl_enter_standby_state: unexpected signalling" );
      break;

    default:
      sig_ctrl_cs_gmm_state( STANDBY_STATE );

      if( grr_is_pbcch_present( ) EQ FALSE )
      {
        PALLOC( rrgrr_standby_state_ind, RRGRR_STANDBY_STATE_IND );
        PSEND( hCommRR, rrgrr_standby_state_ind );
      }
      break;
  }
} /* ctrl_enter_standby_state() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_enter_ready_state 
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_enter_ready_state( void )
{
  TRACE_FUNCTION( "ctrl_enter_ready_state" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_NULL:
      TRACE_ERROR( "ctrl_enter_ready_state: unexpected signalling" );
      break;

    default:
      sig_ctrl_cs_gmm_state( READY_STATE );

      if( grr_is_pbcch_present( ) EQ FALSE )
      {
        PALLOC( rrgrr_ready_state_ind, RRGRR_READY_STATE_IND );
        PSEND( hCommRR, rrgrr_ready_state_ind );
      }
      break;
  }
} /* ctrl_enter_ready_state() */

#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : ctrl_create_freq_list_cbch
+------------------------------------------------------------------------------
| Description : Creates the frequency list to be used for mobile allocation
|               when CBCH has hopping channel.
| Parameters. : cbch_req -output ; points the cbch information structure
|                                  that would be sent in RRGRR_CBCH_INFO_IND
|               freq_par : frequency parameters struct received in PSI 8 
|                          describing the CBCH channel.
|               gp_ma    : GPRS mobile allocation IE referred in Freq parametrs
|                          struct.
|               ma_num   : MA_NUM received in freq params, when indirect encoding 
|                          is used. Otherwise, this parameter is not important.
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ctrl_create_freq_list_cbch(T_cbch *cbch_req,const T_freq_par *freq_par,const T_gprs_ms_alloc_ie *gp_ma,UBYTE ma_num)
{
  T_LIST list1;     /*lint !e813*/
  USHORT list2[64]; /*lint !e813*/
  USHORT count = 0;
  TRACE_FUNCTION( "ctrl_create_freq_list_cbch" );
  memset(&list1, 0, sizeof(T_LIST));
  memset(list2, NOT_PRESENT_16BIT, sizeof(list2));  
  /* 
   * Now we have the GPRS Mobile Allocation corresponding to the ma_num.
   * Get the referenced set of radio frequency lists for this
   * particular GPRS Mobile Allocation IE.
   * If RFL number list is not present in the IE, then cell allocation
   * would be returned by this function.
   */
  if(!grr_get_ref_list_or_cell_allocation(&list1,gp_ma,ma_num))
  {
    TRACE_ERROR("Could not get ref list");
    return FALSE; 
  }
    
  /*
   * Copy ARFCN values into freq_list
   */
  srv_create_list (&list1, list2, 64, TRUE, 0);

  /*
   * Take MA_BITMAP or ARFCN_INDEX into account
   */
  grr_ma_filter_list( list2,         /* input  */
                      cbch_req->ma,  /* output */
                      gp_ma
                    );

  /*
   * Get number of copied frequencies
   */
  if(!grr_validate_and_count_frequencies( cbch_req->ma,&count))
  {
    TRACE_ERROR( "grr_create_freq_list: validation failed/freq count zero" );  
    return FALSE;
  }

  /* The hopping frequency list has to be sent to RR should be in the form
   * that is used in MPH sap. The T_p_freq_list used in MPHP sap. The temporary
   * variable temp_p_freq_list has been temporarily used here to reuse the
   * code. The last element in temp_p_freq_list should be set to NOT_PRESENT_16BIT
   * and then copied back to T_cbch cbch_req.
   */
  cbch_req->ma[count] = NOT_PRESENT_16BIT;
   
  /*There is no need to convert the frequencies to L1 form. This would be
   * done by ALR when this information is passed to it by RR.
   */

  cbch_req->hsn = gp_ma->hsn;
  return TRUE;
}/* ctrl_create_freq_list_cbch */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_fill_cbch_hopping_params
+------------------------------------------------------------------------------
| Description : Fills the hopping list to be sent in RRGRR_CBCH_INFO_IND
|
| Parameters  : cbch_req - points to cbch info that would go in primitive.
|               freq_par - frequency parameters received in psi8
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL ctrl_fill_cbch_hopping_params(T_cbch *cbch_req,const T_freq_par *freq_par)
{
  T_gprs_ms_alloc_ie *gp_ma  = NULL;
  BOOL                result = FALSE;


  TRACE_FUNCTION("ctrl_fill_cbch_hopping_params");

  if(freq_par->v_indi_encod)
  {
    result = grr_validate_ma_num_in_freq_par(freq_par,GRR_FREQ_PARAM_RECEIVED_IN_NON_ASSIGNMENT);
    if(result)
    {
      gp_ma = grr_get_gprs_ma(freq_par->indi_encod.ma_num);    
      if(gp_ma EQ NULL)
      {
        TRACE_EVENT("No valid GPRS_MA found");
        return FALSE;
      }
      if(!ctrl_create_freq_list_cbch(cbch_req,
                                     freq_par,
                                     gp_ma,
                                     freq_par->indi_encod.ma_num))
      {
        TRACE_EVENT("Failed to create freq list for cbch");
        return FALSE;
      }
      cbch_req->maio = freq_par->indi_encod.maio;
    }
    else
    {
      TRACE_ERROR("error creating frequency list,Invalid MA_NUM");
      result = FALSE;
    }
  } /* if(freq_par->v_indi_encod) */
  else if(freq_par->v_di_encod1)
  {
   
    /* In this case, the GPRS mobile allocation IE is present 
     * in the message itself. 
     */
    if(!ctrl_create_freq_list_cbch(cbch_req,
                                  freq_par,
                                  &freq_par->di_encod1.gprs_ms_alloc_ie,
                                  0))
    {
      TRACE_EVENT("Failed to create freq list for cbch");
      return FALSE;
    }
    cbch_req->maio = freq_par->di_encod1.maio;
   
  } /* if(freq_par->v_di_encod1) */
  else if(freq_par->v_di_encod2)
  {
    /* processing of T_freq_par from assignment and non-assignment messages
     * when direct encoding 2 is used, should be done in same object module as 
     * this involvs use of temporary static array variables which are local to the 
     * object module they are used in(_local_dummy_list and _local_rfl_contents).
     * Hence processing for both assignment and non-assignement messages is done
     * in grr_f.c. If they are put in different files, we will have to duplicate
     * the temporary static arrays.
     */
    result = grr_cnv_freq_para_in_psi8_direct_enc_2(cbch_req,freq_par);
      
  } /* if(freq_par->v_di_encod2) */
  else
  {
    TRACE_ERROR("FATAL ERROR: no frequency params in cbch description.");
  }

  return (result);
}/* ctrl_fill_cbch_hopping_params */
#endif

