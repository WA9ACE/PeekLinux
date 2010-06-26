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
|  Purpose :  This module implements primitive handler functions for service
|             CTRL of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CTRLP_C
#define GRR_CTRLP_C
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

#include "grr_ctrlf.h"  /* */
#include "grr_ctrls.h"
#include "grr_psis.h"   /* */
#include "grr_pgs.h"    /* */
#include "grr_meass.h"  /* */
#include "grr_tcs.h"    /* */
#include "grr_css.h"    /* */
#include "grr_f.h"      /* */
#include "grr_tcf.h"

#include <string.h>     /* for memcpy */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_enable_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_ENABLE_REQ
|
| Parameters  : *gmmrr_enable_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_enable_req ( T_GMMRR_ENABLE_REQ *gmmrr_enable_req )
{ 
  TRACE_FUNCTION( "gmmrr_enable_req" );
  
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_NULL:
      SET_STATE( CTRL,CTRL_READY );
      
      /*  get MS class: A, B, C*/
      grr_data->ms.class_of_mode     = gmmrr_enable_req->mobile_class;
/* BEGIN GRR_NMO_III*/      
      if(grr_data->ms.class_of_mode EQ GMMRR_CLASS_B)
      {
        TRACE_EVENT(" Non reverting MS in GMMRR_NET_MODE_III: MS Class B");
        TRACE_EVENT(" CCCH monitoring during TBF is allowed");
        grr_data->ms.reverts_NMO_III = FALSE;  /* Non reverting MS in NMO_III: MS Class B*/
        grr_data->ms.tbf_mon_ccch    = TRUE;   /* CCCH monitoring during TBF is allowed*/        
      }
/* END GRR_NMO_III*/      
      /*  get MS access control class*/
      grr_data->ms.access_ctrl_class = gmmrr_enable_req->acc_contr_class;

      /* split_paging cycle value! NOT the code*/
      grr_data->ms.split_pg_cycle    = gmmrr_enable_req->split_pg_cycle;

      /* split paging cycle support on CCCH */
      psc_db->net_ctrl.spgc_ccch_supp.ms_supp = gmmrr_enable_req->spgc_ccch_supp;

      psc_db->non_drx_timer          = gmmrr_enable_req->gmmrr_non_drx_timer; /* values between 0 and 7*/
      break;
    default:
      TRACE_ERROR( "GMMRR_ENABLE_REQ unexpected" );
      break;
  }
  PFREE(gmmrr_enable_req);
} /* ctrl_gmmrr_enable_req() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_assign_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_ASSIGN_REQ
|
| Parameters  : *gmmrr_assign_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_assign_req ( T_GMMRR_ASSIGN_REQ *gmmrr_assign_req )
{ 
  TRACE_FUNCTION( "gmmrr_assign_req" );
  
  
  /*
   * PTMSI
   */
  grr_data->db.ms_id.old_ptmsi = gmmrr_assign_req->old_ptmsi;
  grr_data->db.ms_id.new_ptmsi = gmmrr_assign_req->new_ptmsi;

  /*
   * TLLI
   */
  grr_data->db.ms_id.old_tlli = gmmrr_assign_req->old_tlli;
  grr_data->db.ms_id.new_tlli = gmmrr_assign_req->new_tlli;
  
  if(grr_data->db.ms_id.new_tlli EQ GMMRR_TLLI_INVALID)
  {
    TRACE_EVENT("GMM HAS UNASSIGNED GRR, DELETE DATA QUEUE");        
  }
  /* for received TLLI: if old is valid take old tlli for received TLLI*/
  grr_data->db.ms_id.received_tlli = (gmmrr_assign_req->old_tlli NEQ GMMRR_TLLI_INVALID) 
    ?
    gmmrr_assign_req->old_tlli 
    : 
    gmmrr_assign_req->new_tlli 
    ;
  /*send PTMSI to RR*/
  grr_data->db.ms_id.rai   = gmmrr_assign_req->rai;

  {
    PALLOC(rrgrr_gprs_data_req, RRGRR_GPRS_DATA_REQ);
    rrgrr_gprs_data_req->old_ptmsi = grr_data->db.ms_id.old_ptmsi;
    rrgrr_gprs_data_req->new_ptmsi =grr_data->db.ms_id.new_ptmsi;
    rrgrr_gprs_data_req->tlli = gmmrr_assign_req->new_tlli;
    rrgrr_gprs_data_req->rai  = grr_data->db.ms_id.rai;

    memset(&rrgrr_gprs_data_req->p_chan_req_des,0,sizeof(T_p_chan_req_des));
    memset(&rrgrr_gprs_data_req->gprs_meas_results,0,sizeof(T_gprs_meas_results));
    rrgrr_gprs_data_req->mac_req = 0;
    rrgrr_gprs_data_req->cs_req  = 0;

    PSEND(hCommRR, rrgrr_gprs_data_req);
  }   

  grr_data->db.ms_id.imsi.number_of_digits = ctrl_get_number_of_digits(gmmrr_assign_req->imsi.id);
  memcpy(grr_data->db.ms_id.imsi.digit, gmmrr_assign_req->imsi.id, MAX_IMSI_DIGITS);
  sig_ctrl_tc_enable_grlc  (CGRLC_QUEUE_MODE_DEFAULT, CGRLC_RA_DEFAULT );
  PFREE(gmmrr_assign_req);

} /* ctrl_gmmrr_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_disable_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_DISABLE_REQ
|
| Parameters  : *gmmrr_disable_req - Ptr to primitive payload

+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_disable_req ( T_GMMRR_DISABLE_REQ *gmmrr_disable_req )
{ 
  TRACE_FUNCTION( "gmmrr_disable_req" );
  
  
  SET_STATE( CTRL,CTRL_NULL );

  ctrl_stop_all_activities( CTRL_DC_OTHER, NULL );
  ctrl_start_monitoring_ccch( PAG_MODE_DEFAULT );

  /* Disable GRLC , delete all primitives */
  tc_cgrlc_disable_req(CGRLC_PRIM_STATUS_ALL);

  /* Re-initialize GRR database */
  TRACE_EVENT("Reinit GRR db");
  grr_init();
  
  PFREE(gmmrr_disable_req);

} /* ctrl_gmmrr_disable_req() */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_gprs_si13_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_GPRS_SI13_IND
|
| Parameters  : *rrgrr_gprs_si13_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_gprs_si13_ind 
                                ( T_RRGRR_GPRS_SI13_IND * rrgrr_gprs_si13_ind )
{ 
  UBYTE          state        = GET_STATE( CTRL );
  USHORT arfcn;
  UBYTE  bsic;
  T_GPRS_SERVICE gprs_service = ctrl_get_gprs_service( );

  TRACE_FUNCTION( "ctrl_rrgrr_gprs_si13_ind" );  

  TRACE_EVENT_P5( "RRGRR_GPRS_SI13_IND - CELL: %d %d, %d %d, STATE: %d", 
                  rrgrr_gprs_si13_ind->serving_cell_info.bcch_arfcn,
                  rrgrr_gprs_si13_ind->serving_cell_info.bcch_bsic,
                  rrgrr_gprs_si13_ind->cause,
                  rrgrr_gprs_si13_ind->serving_cell_info.limited,
                  state );

  switch( state )
  {
    case CTRL_READY:
    case CTRL_ACCESS_DISABLED:
    case CTRL_READY_TO_PROGRESS:
    case CTRL_IN_PROGRESS:
    case CTRL_AUTO_RR_CR:
    case CTRL_RR_CR_IN_SUSP:
      switch( rrgrr_gprs_si13_ind->cause )
      {
        case GPRS_NOT_SUPPORTED:
          ctrl_si13_received( rrgrr_gprs_si13_ind );

          if( ( state & CTRL_SUSPENSION_MASK ) EQ CTRL_SUSPENSION_YES )
          {
            SET_STATE( CTRL, CTRL_NO_CELL_SUSP );

            /*
             * cell indication is sent after resume:
             * ctrl_send_cell_ind( );
             */
          }
          else
          {
            SET_STATE( CTRL, CTRL_READY );

            ctrl_send_cell_ind( );
          }
      
          ctrl_stop_all_activities( CTRL_DC_OTHER, NULL );
          ctrl_send_cell_reselection_req( CR_COMPLETE );
          break;

        case GPRS_SUPPORTED:
          if( grr_decode_rr
                ( ( T_MSGBUF* )( &rrgrr_gprs_si13_ind->sdu ) ) EQ D_SYS_INFO_13 )
          {
            ctrl_si13_received( rrgrr_gprs_si13_ind );

            if( state        EQ  CTRL_IN_PROGRESS         AND
                gprs_service NEQ ctrl_get_gprs_service( )     )
            {
              ctrl_send_cell_ind( );
            }
          }
          else
          {
            if( ( state & CTRL_CELL_RESEL_MASK ) EQ CTRL_CELL_RESEL_RR )
            {
              ctrl_send_cell_reselection_req( CR_CONT );
            }
            else
            {
              /* impl. missing, what happens if decoding failed? */
            }
          }
          break;

        case GPRS_CELL_NOT_SUITABLE:
        default:
          /* do nothing */
          break;
      }
      break;
  
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
      if( grr_t_status(T3176) > 0 )
      { 
        if(grr_decode_rr((T_MSGBUF *)(&(rrgrr_gprs_si13_ind->sdu))) EQ D_SYS_INFO_13)
        {
          if( rrgrr_gprs_si13_ind->serving_cell_info.bcch_arfcn EQ
              posc_db->pbcch.bcch.arfcn )
          {  
            /* impl. missing, what happend if service on old serving cell has changed? */
        
            ctrl_reinstall_old_scell_cnf( TRUE );
            sig_ctrl_psi_resumpt_cc( );
            ctrl_si13_received( rrgrr_gprs_si13_ind );
            ctrl_enable_serving_cell( FALSE );
            ctrl_send_cell_reselection_req( CR_COMPLETE );
            sig_ctrl_tc_enable_grlc  (CGRLC_QUEUE_MODE_LLC, CGRLC_RA_DEFAULT );
            arfcn         = grr_data->ctrl.pcc_arfcn;
            bsic          = grr_data->ctrl.pcc_bsic;

            ctrl_build_pcco_failure_msg(arfcn, bsic);
          }
        }
      }
      else if( pcr_db NEQ NULL )
      {      
        if( rrgrr_gprs_si13_ind->cause EQ GPRS_CELL_NOT_SUITABLE )
        {
          grr_set_db_ptr( DB_MODE_CC_REQ );
      
          SET_STATE_FAILED_CR( state );

          ctrl_cc_rejected( );
        }
        else if( rrgrr_gprs_si13_ind->serving_cell_info.bcch_arfcn EQ
                   pcr_db->pbcch.bcch.arfcn                           AND
                 rrgrr_gprs_si13_ind->serving_cell_info.bcch_bsic  EQ
                   pcr_db->pbcch.bcch.bsic )
        {
          if( rrgrr_gprs_si13_ind->cause EQ GPRS_NOT_SUPPORTED )
          {
            grr_set_db_ptr( DB_MODE_CC_REQ );
      
            /*
             * The neighbour cell does not support GPRS: No SI13
             * We have to check the GPRS re-selection parameters and 
             * decide whether we can camp on this cell or not
             * Therefore compare GPRS re-selection parameters
             */        
            if( sig_ctrl_cs_check_c1_new_scell
                  ( state EQ CTRL_RR_NC_CCO OR state EQ CTRL_GRR_NC_CCO,
                    rrgrr_gprs_si13_ind->serving_cell_info.gprs_rxlev_access_min,
                    rrgrr_gprs_si13_ind->serving_cell_info.gprs_ms_txpwr_max_cch ) )
            {
              ctrl_si13_received( rrgrr_gprs_si13_ind );

              /*
               * the selected cell is suitable
               */
              if( ( state & CTRL_SUSPENSION_MASK ) EQ CTRL_SUSPENSION_YES )
              {
                SET_STATE( CTRL, CTRL_NO_CELL_SUSP );

                /*
                 * cell indication is sent after resume:
                 * ctrl_send_cell_ind( );
                 */
              }
              else
              {
                SET_STATE( CTRL, CTRL_READY );

                ctrl_send_cell_ind( );
              }

              ctrl_install_new_scell ( );
              ctrl_send_cell_reselection_req( CR_COMPLETE );
              grr_set_db_ptr( DB_MODE_CC_ACC );
            }
            else
            {
              /*
               * the selected cell is not suitable
               */

              SET_STATE_FAILED_CR( state );

              ctrl_cc_rejected( );
            }
          }
          else /* if( rrgrr_gprs_si13_ind->cause EQ GPRS_SUPPORTED ) */
          {
            grr_set_db_ptr( DB_MODE_CC_REQ );

            if(grr_decode_rr((T_MSGBUF *)(&(rrgrr_gprs_si13_ind->sdu))) EQ D_SYS_INFO_13)
            {
              /*
               * Check whether the selected NCELL is OK or not
               */
              if( sig_ctrl_cs_check_c1_new_scell 
                    ( state EQ CTRL_RR_NC_CCO OR state EQ CTRL_GRR_NC_CCO,
                      rrgrr_gprs_si13_ind->serving_cell_info.gprs_rxlev_access_min,
                      rrgrr_gprs_si13_ind->serving_cell_info.gprs_ms_txpwr_max_cch ) )
              {
                ctrl_si13_received( rrgrr_gprs_si13_ind );
              }
              else
              {
                SET_STATE_FAILED_CR( state );

                ctrl_cc_rejected( );
              }
            }
            else
            {
              SET_STATE_FAILED_CR( state );

              ctrl_cc_rejected( );
            }
          }
        }
        else
        {
          TRACE_EVENT_P2( "RRGRR_GPRS_SI13_IND - GPRS indication expected from CELL: %d %d",
                          pcr_db->pbcch.bcch.arfcn, pcr_db->pbcch.bcch.bsic );
        }
      }
      break;

    /*
     * In the following states GRR does not expect a RRGRR_GPRS_SI13_IND
     * due to several different reasons, please see details below.
     */

    /*
     * CTRL is switched off, no processing will be performed at all.
     */
    case CTRL_NULL:

    /*
     * CTRL waits for a confirmation, all states are interim states.
     * Shall GRR be ready for processing RRGRR_GPRS_SI13_IND anyway ???
     */
    case CTRL_WAIT_CS_PAGE_RES:
    case CTRL_WAIT_FOR_CNF_OF_SUSPENSION:
    case CTRL_WAIT_RSP_4_RR_CR:

    /*
     * CTRL is in a temporary state, which shall be set and reset
     * during one primitive processing cycle.
     */
    case CTRL_FAILED_CR:
    case CTRL_FAILED_CR_IN_SUSP:

    /*
     * CTRL expects a RRGRR_GPRS_SI13_IND during suspension only when
     * a RRGRR_CR_IND was sent before, the state changes to
     * CTRL_RR_CR_IN_SUSP or CTRL_CR_IN_SUSP in that case.
     */
    case CTRL_SUSPENDED:
    case CTRL_ACC_ENABLED_SUSP:

    default:
      /* do nothing */
      break;
  }

  PFREE( rrgrr_gprs_si13_ind );

} /* ctrl_rrgrr_gprs_si13_ind() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_packet_paging_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_PACKET_PAGING_IND: this primitve indicates 
|               that the RR part has received a paging for packet data transfer
|               CTRL informs the PG-Service and PG-Service has to decide how to
|               react to this indication
|
| Parameters  : *rrgrr_packet_paging_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_packet_paging_ind ( T_RRGRR_PACKET_PAGING_IND *rrgrr_packet_paging_ind )
{ 
  TRACE_FUNCTION( "rrgrr_packet_paging_ind" );
 
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_pg_indication(rrgrr_packet_paging_ind->pg_type);
      break;
    default:
      TRACE_ERROR( "RRGRR_PACKET_PAGING_IND unexpected" );
      break;
  }
  PFREE (rrgrr_packet_paging_ind);
} /* ctrl_rrgrr_packet_paging_ind() */





/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_ia_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_IA_IND
|
| Parameters  : *rrgrr_ia_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_ia_ind ( T_RRGRR_IA_IND *rrgrr_ia_ind )
{ 
  TRACE_FUNCTION( "rrgrr_ia_ind" );

/*

  {
    ULONG trace[5];

    trace[0]  = rrgrr_ia_ind->sdu.buf[0]  << 24;
    trace[0] |= rrgrr_ia_ind->sdu.buf[1]  << 16; 
    trace[0] |= rrgrr_ia_ind->sdu.buf[2]  <<  8; 
    trace[0] |= rrgrr_ia_ind->sdu.buf[3]  <<  0; 

    trace[1]  = rrgrr_ia_ind->sdu.buf[4]  << 24;
    trace[1] |= rrgrr_ia_ind->sdu.buf[5]  << 16; 
    trace[1] |= rrgrr_ia_ind->sdu.buf[6]  <<  8; 
    trace[1] |= rrgrr_ia_ind->sdu.buf[7]  <<  0; 

    trace[2]  = rrgrr_ia_ind->sdu.buf[8]  << 24;
    trace[2] |= rrgrr_ia_ind->sdu.buf[9]  << 16; 
    trace[2] |= rrgrr_ia_ind->sdu.buf[10] <<  8; 
    trace[2] |= rrgrr_ia_ind->sdu.buf[11] <<  0; 

    trace[3]  = rrgrr_ia_ind->sdu.buf[12] << 24;
    trace[3] |= rrgrr_ia_ind->sdu.buf[13] << 16; 
    trace[3] |= rrgrr_ia_ind->sdu.buf[14] <<  8; 
    trace[3] |= rrgrr_ia_ind->sdu.buf[15] <<  0; 

    trace[4]  = rrgrr_ia_ind->sdu.buf[16] << 24;
    trace[4] |= rrgrr_ia_ind->sdu.buf[17] << 16; 
    trace[4] |= rrgrr_ia_ind->sdu.buf[18] <<  8; 
    trace[4] |= rrgrr_ia_ind->sdu.buf[19] <<  0; 

    TRACE_EVENT_P8( "RRGRR_IA_IND: %08X%08X%08X%08X%08X%02X%02X%02X",
                    trace[0], trace[1], trace[2], trace[3], trace[4],
                    rrgrr_ia_ind->sdu.buf[20], rrgrr_ia_ind->sdu.buf[21],
                    rrgrr_ia_ind->sdu.buf[22] ); 
  }

*/
  
  if(D_IMM_ASSIGN EQ grr_decode_rr((T_MSGBUF *)(&(rrgrr_ia_ind->sdu))))
  {
    switch( GET_STATE( CTRL ) )
    {
      case CTRL_IN_PROGRESS:   
      case CTRL_GRR_NC_CCO:
        sig_ctrl_tc_ia_received (rrgrr_ia_ind);                
        break;
      default:
        TRACE_ERROR( "RRGRR_IA_IND unexpected" );
        break;
    }
  }
  else
  {
    TRACE_ERROR( "RRGRR_IA_IND wrong message type" );
  }

  PFREE (rrgrr_ia_ind);
} /* ctrl_rrgrr_ia_ind() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_iaext_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_IAEXT_IND
|
| Parameters  : *rrgrr_iaext_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_iaext_ind (  T_RRGRR_IAEXT_IND *rrgrr_iaext_ind)
{ 
  TRACE_FUNCTION( "rrgrr_iaext_ind" );
  if(D_IMM_ASSIGN_EXT == grr_decode_rr((T_MSGBUF *)(&(rrgrr_iaext_ind->sdu))))
  {
    switch( GET_STATE( CTRL ) )
    {
      case CTRL_IN_PROGRESS:     
        sig_ctrl_tc_iaext_received (rrgrr_iaext_ind);                
        break;
      default:
        TRACE_ERROR( "RRGRR_IAEXT_IND unexpected" );
        break;
    }
  }
  else
  {
    TRACE_ERROR( "RRGRR_IAEXT_IND wrong message type" );
  }
  PFREE (rrgrr_iaext_ind);
  
} /* ctrl_rrgrr_iaext_ind() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_assignment_rej_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_ASSIGNMENT_REJ_IND
|
| Parameters  : *rrgrr_assignment_rej_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_assignment_rej_ind ( T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind )
{ 
 TRACE_FUNCTION( "rrgrr_assignment_rej_ind" );
 

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:     
      sig_ctrl_tc_iarej_received (rrgrr_assignment_rej_ind);                
      break;
    case CTRL_GRR_NC_CCO:     
      sig_ctrl_tc_iarej_received (rrgrr_assignment_rej_ind);
      ctrl_stop_T3174(); 
      ctrl_pcco_failure();
      break;
    default:
      TRACE_ERROR( "RRGRR_ASSIGNMENT_REJ_IND unexpected" );
      break;
  }
 
  PFREE (rrgrr_assignment_rej_ind);

} /* ctrl_rrgrr_assignment_rej_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_t3134
+------------------------------------------------------------------------------
| Description : Handles the time out of T3134 
|
| Parameters  : No parameter
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_t3134 ( void )
{
  TRACE_FUNCTION( "ctrl_t3134" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_RR_NC_CCO:
      sig_ctrl_tc_cr_timerout_cnf();
      ctrl_reinstall_old_scell_req( FALSE, RECONN_TIMER_EXPIRED );
      break;
    default:
      TRACE_ERROR( "time out of T3134 unexpected" );
      break;
  }

}/* ctrl_t3134() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_t3174
+------------------------------------------------------------------------------
| Description : Handles the time out of T3174
|
| Parameters  : No parameter
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_t3174 ( void )
{
  TRACE_FUNCTION( "ctrl_t3174" );
  TRACE_EVENT("T3174 expired");

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_GRR_NC_CCO:
    case CTRL_IN_PROGRESS:
      grr_data->pcco_failure_cause   = 1;  /*No_Resp_On_Target_Cell*/
      ctrl_pcco_failure( );
      break;
    default:
      TRACE_ERROR( "time out of T3174 unexpected" );
      break;
  }
} /* ctrl_t3174() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_start_t3174
+------------------------------------------------------------------------------
| Description : Handles the time starting of T3174
|
| Parameters  : No parameter
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_start_T3174 ( void )
{
  TRACE_FUNCTION( "tc_start_T3174" );

  TRACE_EVENT( "T3174 for packet cell change started" );

  vsi_t_start( GRR_handle,T3174, T3174_VALUE );
  
} /* tc_start_T3174  */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_t3176
+------------------------------------------------------------------------------
| Description : Handles the timer T3176
|
| Parameters  : inctance_i - identifier of timer's inctance  
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_t3176 ( void )
{ 
  TRACE_FUNCTION( "ctrl_t3176" );
  TRACE_EVENT("T3176 expired");
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_GRR_NC_CCO:

      SET_STATE( CTRL,CTRL_IN_PROGRESS );

      /*lint -fallthrough*/

    default:
      ctrl_cancel_control_block( CGRLC_BLK_OWNER_CTRL );
      break;
  }

} /* ctrl_t3176() */



/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_data_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_DATA_IN
|
| Parameters  : *rrgrr_pdch_assignment_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_data_ind ( T_RRGRR_DATA_IND *rrgrr_data_ind )
{ 
  TRACE_FUNCTION( "ctrl_rrgrr_data_ind" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:     
      switch( grr_decode_rr((T_MSGBUF *)(&(rrgrr_data_ind->sdu))) )
      {
        case D_PDCH_ASS_CMD:
          sig_ctrl_tc_pdch_assign_received (rrgrr_data_ind);   
          break;
        case D_CHANGE_ORDER:
          /*
           * RR Network cell change order was received
           */

          {
            MCAST(d_change_order,D_CHANGE_ORDER);

            ctrl_cell_change_order( CCO_TYPE_RR, (void *)d_change_order );
          }
          break;
        default:
          TRACE_ERROR( "RRGRR_PDCH_ASSIGNMENT_IND grr_decode_rrgrr failed: message type NEQ PDCH_ASSIGN" );     
          break;

      }
      break;
    default:
      TRACE_ERROR( "RRGRR_PDCH_ASSIGNMENT_IND unexpected" );
      break;
  }
  
  PFREE (rrgrr_data_ind);
} /* ctrl_rrgrr_data_ind() */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_ia_downlink_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_IA_DOWNLINK_IND
|
| Parameters  : *rrgrr_ia_downlink_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_ia_downlink_ind ( T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_ind )
{ 
  TRACE_FUNCTION( "rrgrr_ia_downlink_ind" );

  if(D_IMM_ASSIGN == grr_decode_rr((T_MSGBUF *)(&(rrgrr_ia_downlink_ind->sdu))))
  {  
    switch( GET_STATE( CTRL ) )
    {
      case CTRL_IN_PROGRESS:     
	  if (grr_data->cell_res_status)
	  	{	  	
        sig_ctrl_tc_ia_downlink_received (rrgrr_ia_downlink_ind);               
	  	}
	  else
	  	{
	  	TRACE_ERROR ("RRGRR_IA_DOWNLINK_IND received during GMMRR_CELL_IND procedure");
	  	}
        break;
      default:
        ctrl_send_rrgrr_stop_task_req( CTRL_TASK_BCCH, INVALID_MSG );
        TRACE_ERROR( "RRGRR_IA_DOWNLINK_IND unexpected" );        
        break;
    }
  }
  else
  {
    ctrl_send_rrgrr_stop_task_req( CTRL_TASK_BCCH, INVALID_MSG );
    TRACE_ERROR( "RRGRR_IA_IND wrong message type" ); 
  }
  PFREE (rrgrr_ia_downlink_ind);
} /* ctrl_rrgrr_ia_downlink_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_stop_task_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_STOP_TASK_CNF
|
| Parameters  : *rrgrr_stop_task_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_stop_task_cnf
                                 ( T_RRGRR_STOP_TASK_CNF *rrgrr_stop_task_cnf )
{ 
  TRACE_FUNCTION( "ctrl_rrgrr_stop_task_cnf" );

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
      if( grr_data->ctrl_glbl.ctrl_task EQ CTRL_TASK_BCCH )
      {
        sig_ctrl_tc_stop_task_cnf_received( );

        grr_data->ctrl_glbl.ctrl_task = CTRL_TASK_NONE;
      }
      else
      {
        ctrl_stop_rr_task_cnf( TRUE );
      }
      break;
    default:
      TRACE_ERROR( "RRGRR_STOP_TASK_CNF unexpected" );
      break;
  }

  PFREE( rrgrr_stop_task_cnf );

} /* ctrl_rrgrr_stop_task_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : gmmrr_ready_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_READY_REQ
|
| Parameters  : *gmmrr_ready_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_ready_req ( T_GMMRR_READY_REQ *gmmrr_ready_req )
{ 
  TRACE_FUNCTION( "gmmrr_ready_req" );
  
  ctrl_enter_ready_state( );

  PFREE( gmmrr_ready_req );

} /* ctrl_gmmrr_ready_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cgrlc_ready_state_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_READY_STATE_IND
|
| Parameters  : *cgrlc_ready_state_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_cgrlc_ready_state_ind 
                             ( T_CGRLC_READY_STATE_IND *cgrlc_ready_state_ind )
{ 
  TRACE_FUNCTION( "ctrl_cgrlc_ready_state_ind" );
  
  ctrl_enter_ready_state( );

  PFREE( cgrlc_ready_state_ind );

} /* ctrl_cgrlc_ready_state_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_standby_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_STANDBY_REQ
|
| Parameters  : *gmmrr_standby_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_standby_req ( T_GMMRR_STANDBY_REQ *gmmrr_standby_req )
{ 
  TRACE_FUNCTION( "gmmrr_standby_req" );
  
  ctrl_enter_standby_state( );

  PFREE( gmmrr_standby_req );

} /* ctrl_gmmrr_standby_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_cgrlc_standby_state_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive CGRLC_STANDBY_STATE_IND
|
| Parameters  : *cgrlc_standby_state_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_cgrlc_standby_state_ind 
                         ( T_CGRLC_STANDBY_STATE_IND *cgrlc_standby_state_ind )
{ 
  TRACE_FUNCTION( "ctrl_cgrlc_standby_state_ind" );
  
  ctrl_enter_standby_state( );

  PFREE( cgrlc_standby_state_ind );

} /* ctrl_cgrlc_standby_state_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_cs_page_res
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CS_PAGE_RES
|
| Parameters  : *ctrl_gmmrr_cs_page_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_cs_page_res ( T_GMMRR_CS_PAGE_RES *gmmrr_cs_page_res )
{ 
  TRACE_FUNCTION( "gmmrr_cs_page_res" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_WAIT_CS_PAGE_RES:
      SET_STATE(CTRL, grr_data->ctrl.last_state);
      switch(grr_data->ctrl.cs_page_channel)
      {
        case PG_ON_PCCCH:
          if(gmmrr_cs_page_res->response NEQ GMMRR_CS_PAGE_REJ)
          {
            /*  
             * GMM accepted the CS_PAGE_IND
             */
            grr_data->ctrl.is_susp_needed =(gmmrr_cs_page_res->susp_gprs EQ GMMRR_SUSP_GPRS) 
                                            ? SUSPENSION_REQ_NEEDED
                                            :SUSPENSION_REQ_NOT_NEEDED;
            if (grr_data->ms.class_of_mode NEQ GMMRR_CLASS_A)
            {
              /*
               * suspend services
               */
              grr_data->ctrl.after_suspension = SEND_RR_EST_REQ;
              SET_STATE(CTRL, CTRL_WAIT_FOR_CNF_OF_SUSPENSION);
              ctrl_suspend_services(GMMRR_ABNORMAL_RELEASE);
            }
            else
            {
              /* Do not suspend services */
              ctrl_send_rr_est_req(grr_data->ctrl.is_susp_needed);
              sig_ctrl_meas_suspend( );
            }
          }
          break;
        case PG_ON_CCCH:          
          if(gmmrr_cs_page_res->response NEQ GMMRR_CS_PAGE_REJ)
          {
            /*  
             * GMM accepted the CS_PAGE_IND
             */
            grr_data->ctrl.is_susp_needed =(gmmrr_cs_page_res->susp_gprs EQ GMMRR_SUSP_GPRS) 
                                            ? SUSPENSION_REQ_NEEDED
                                            :SUSPENSION_REQ_NOT_NEEDED;
            if (grr_data->ms.class_of_mode NEQ GMMRR_CLASS_A)
            {
              /*
               * suspend services
               */
              grr_data->ctrl.after_suspension = SEND_RR_EST_RSP;
              SET_STATE(CTRL, CTRL_WAIT_FOR_CNF_OF_SUSPENSION);
              ctrl_suspend_services(GMMRR_ABNORMAL_RELEASE);
            }
            else
            {
              /* Do not suspend services */
              ctrl_send_rr_est_req(grr_data->ctrl.is_susp_needed);
              sig_ctrl_meas_suspend( );
            }
          }
          else
          {
            /*  GMM rejected the CS_PAGE_IND*/
              ctrl_send_rr_est_rsp(SUSPENSION_REQ_NOT_NEEDED/*in this case it has no meaning*/,
                                          FALSE /* CS_PAGE was rejected*/);
          }
          break;
      }
      break;
    default:
      TRACE_ERROR( "GMMRR_CS_PAGE_RES unexpected" );
      break;
  }
  PFREE(gmmrr_cs_page_res);

} /* ctrl_gmmrr_cs_page_res() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_suspend_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CS_PAGE_RES
|
| Parameters  : *gmmrr_suspend_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_suspend_req ( T_GMMRR_SUSPEND_REQ *gmmrr_suspend_req )
{ 
  TRACE_FUNCTION( "gmmrr_suspend_req" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_READY:
    case CTRL_IN_PROGRESS:
    case CTRL_ACCESS_DISABLED:
    case CTRL_WAIT_CS_PAGE_RES:
      grr_data->ctrl.last_state = GET_STATE( CTRL );
      grr_data->ctrl.after_suspension = ACTIVATE_RR;
      
      grr_data->ctrl.is_susp_needed =(gmmrr_suspend_req->susp_gprs EQ GMMRR_SUSP_GPRS) 
                                      ? SUSPENSION_REQ_NEEDED
                                      :SUSPENSION_REQ_NOT_NEEDED;

      SET_STATE(CTRL, CTRL_WAIT_FOR_CNF_OF_SUSPENSION);
      ctrl_suspend_services( gmmrr_suspend_req->gmmrr_susp_cause );
      break;
    default:
      TRACE_ERROR( "GMMRR_SUSPEND_REQ unexpected" );
      break;
  }
  PFREE(gmmrr_suspend_req);

} /* ctrl_gmmrr_suspend_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_resume_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_RESUME_REQ
|                   GRR leaves the SUSPENDED-State
| Parameters  : *gmmrr_suspend_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_resume_req ( T_GMMRR_RESUME_REQ *gmmrr_resume_req )
{ 
  TRACE_FUNCTION( "ctrl_gmmrr_resume_req" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_NO_CELL_SUSP:
      SET_STATE(CTRL, CTRL_READY);
      TRACE_EVENT("No GPRS cell Inform GMM");
      ctrl_send_cell_ind( );
      break;
    case CTRL_RR_CR_IN_SUSP:
      TRACE_EVENT("CR not complete in SUSP mode");
      SET_STATE(CTRL, CTRL_AUTO_RR_CR);
      break;
    case CTRL_CR_IN_SUSP:
      SET_STATE(CTRL, CTRL_CR);
      break;
    case CTRL_FAILED_CR_IN_SUSP:
      SET_STATE(CTRL, CTRL_FAILED_CR);
      break;

    case CTRL_ACC_ENABLED_SUSP:
      /*
       * resume PSI
       * enable cell
       */
      SET_STATE(CTRL, CTRL_IN_PROGRESS); 
      TRACE_EVENT("Cell changed within SUSP");
      sig_ctrl_psi_resumpt();
      ctrl_enable_serving_cell( FALSE );
      break;
    case CTRL_WAIT_FOR_CNF_OF_SUSPENSION:
      /*
       * This is an exceptional case->GMM resumes without receiving suspend_cnf
       */
    case CTRL_SUSPENDED:
      switch(grr_data->ctrl.last_state)
      { 
        case CTRL_NULL:
        case CTRL_READY:
        case CTRL_ACCESS_DISABLED:
          /* 
           * CTRL has to wait for receiving SI13
           */
          if (grr_data->ctrl.last_state EQ CTRL_ACCESS_DISABLED)
          {
             SET_STATE (CTRL, CTRL_ACCESS_DISABLED)
          }
          else
          {
             SET_STATE(CTRL, CTRL_READY); 
          sig_ctrl_psi_stop();
          grr_set_pbcch(FALSE);
          }
          psc_db->acq_type = COMPLETE;
          ctrl_start_monitoring_bcch(COMPLETE_SI);
          break;
        case CTRL_IN_PROGRESS:
        default:
          /* 
           * CTRL assumes that the cell has not been changed
           */
          SET_STATE(CTRL, CTRL_IN_PROGRESS); 
          ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
          break;
      }
      break;
    default:
      TRACE_ERROR( "GMMRR_RESUME_REQ unexpected" );
      break;
  }
  PFREE(gmmrr_resume_req);

} /* ctrl_gmmrr_resume_req() */




/*
+------------------------------------------------------------------------------
| Function    : rrgrr_rr_est_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive T_RRGRR_RR_EST_IND: This primitive idicates 
|               that the RR has received a RR connection establishment. This notification
|               should be passed to PG-Service
|
| Parameters  : *rrgrr_rr_est_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_rr_est_ind ( T_RRGRR_RR_EST_IND *rrgrr_rr_est_ind)
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_FUNCTION( "ctrl_rrgrr_rr_est_ind" );

  switch( state )
  {
    case CTRL_NULL: /* to allow class CC mobile accepting CS call*/
      /*
       * To pass CS page request to GMM
       */
      grr_data->ctrl.cs_page_channel = PG_ON_CCCH;

      SET_STATE(CTRL, CTRL_WAIT_CS_PAGE_RES);
      ctrl_handle_gmmrr_cs_page_ind( state );
      break;
//    case CTRL_READY:
      /*
       * To pass the connection est. request to PG and GMM
       */
    case CTRL_IN_PROGRESS:
      sig_ctrl_pg_rr_est();
      break;
    default:
      {
        TRACE_ERROR( "T_RRGRR_RR_EST_IND unexpected" );
        ctrl_send_rr_est_rsp(FALSE,FALSE  /* CS_PAGE was NOT accepted*/);
      }
      break;
  }
  
  PFREE(rrgrr_rr_est_ind); 
}/* rrgrr_rr_est_ind() */



/*
+------------------------------------------------------------------------------
| Function    : rrgrr_suspend_dcch_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_SUSPEND_DCCH_CNF: This primitive 
|               idicates that the RR has released the DCCH connection.
|               This notification should be passed to TC-Service
|
| Parameters  : *rrgrr_suspend_dcch_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_suspend_dcch_cnf ( T_RRGRR_SUSPEND_DCCH_CNF *rrgrr_suspend_dcch_cnf)
{
  TRACE_FUNCTION( "ctrl_rrgrr_suspend_dcch_cnf" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_tc_suspend_dcch_cnf();
      break;
    default:
      TRACE_ERROR( "RRGRR_SUSPEND_DCCH_CNF unexpected" );
      break;
  }
  
  PFREE(rrgrr_suspend_dcch_cnf); 
}/* ctrl_rrgrr_suspend_dcch_cnf() */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_reconnect_dcch_cnf
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_RECONNECT_DCCH_CNF: This primitive 
|               idicates that the RR has reestablished the DCCH connection.
|               This notification should be passed to TC-Service
|
| Parameters  : *rrgrr_reconnect_dcch_cnf - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_reconnect_dcch_cnf
                       ( T_RRGRR_RECONNECT_DCCH_CNF *rrgrr_reconnect_dcch_cnf )
{
  TRACE_FUNCTION( "ctrl_rrgrr_reconnect_dcch_cnf" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_RR_NC_CCO:
      if(grr_data->cc_running)
      {
        BOOL is_success = ( rrgrr_reconnect_dcch_cnf->reconn_state EQ RECONN_OK );

        if( is_success )
        {
          SET_STATE( CTRL, CTRL_IN_PROGRESS );
          ctrl_reinstall_old_scell_cnf( TRUE );
          ctrl_enable_serving_cell( FALSE );
        }
        else
        {
          SET_STATE( CTRL, CTRL_READY );
          ctrl_reinstall_old_scell_cnf( FALSE );
          ctrl_enable_serving_cell( FALSE );

        }        
        sig_ctrl_tc_reconnect_dcch_cnf( );
      }
      grr_data->cc_running = FALSE;      
      break;
    default:
      TRACE_ERROR( "RRGRR_RECONNECT_DCCH_CNF unexpected" );
      break;
  }
  
  PFREE(rrgrr_reconnect_dcch_cnf);
   
}/* ctrl_rrgrr_reconnect_dcch_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_stop_dcch_ind
+------------------------------------------------------------------------------
| Description : 
|               
|               
|               
|
| Parameters  : *rrgrr_stop_dcch_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_stop_dcch_ind(T_RRGRR_STOP_DCCH_IND* rrgrr_stop_dcch_ind)
{
  TRACE_FUNCTION( "ctrl_rrgrr_stop_dcch_ind" );
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      ;
      break;
    default:
      TRACE_ERROR( "RRGRR_STOP_DCCH_IND unexpected" );
      break;
  }
  
  PFREE(rrgrr_stop_dcch_ind); 
}/*  ctrl_ctrl_rrgrr_stop_dcch_ind() */


/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_cr_ind
+------------------------------------------------------------------------------
| Description : This function indicates that the cell reselection has been
|               startet by the RR entity
| Parameters  : *cr_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_cr_ind( T_RRGRR_CR_IND *cr_ind )
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_FUNCTION( "ctrl_rrgrr_cr_ind" );

  TRACE_ASSERT( cr_ind->cr_type EQ CR_NORMAL         OR
                cr_ind->cr_type EQ CR_ABNORMAL       OR
                cr_ind->cr_type EQ CR_REQ_CANDIDATE  OR
                cr_ind->cr_type EQ CR_SUSPENDED_IDLE    );

  TRACE_EVENT_P2( "RRGRR_CR_IND: %d in state %d", cr_ind->cr_type, state );

  switch( state )
  {
    case CTRL_NULL:
    case CTRL_READY:
      switch( cr_ind->cr_type )
      {
        case CR_NORMAL:
        case CR_ABNORMAL:
          ctrl_set_old_scell( FALSE );
          ctrl_send_cell_reselection_req( CR_COMPLETE );
          break;
        
        case CR_REQ_CANDIDATE:
          TRACE_ERROR( "RRGRR_CR_IND unexpected" );
          break;

        case CR_SUSPENDED_IDLE:
          /* to be defined */
          break;
      }
      break;

    case CTRL_ACC_ENABLED_SUSP:
    case CTRL_NO_CELL_SUSP:
    case CTRL_RR_CR_IN_SUSP:
    case CTRL_SUSPENDED:
      switch( cr_ind->cr_type )
      {
        case CR_ABNORMAL:
          grr_init_ncmeas_extd_struct(&psc_db->nc_ms,FALSE);
        case CR_NORMAL:
          SET_STATE( CTRL, CTRL_RR_CR_IN_SUSP );
     
          ctrl_set_old_scell( TRUE );
          ctrl_stop_all_activities( CTRL_DC_CR_MS_RR_IMM_REL, NULL );
          grr_prepare_db_for_new_cell( );
          break;
        
        case CR_REQ_CANDIDATE:
          SET_STATE( CTRL, CTRL_CR_IN_SUSP );

          sig_ctrl_cs_reselect( CS_RESELECT_CAUSE_CTRL_NORMAL );
          break;

        case CR_SUSPENDED_IDLE:
          /* to be defined */
          break;
      }
      break;

    case CTRL_IN_PROGRESS:
    case CTRL_ACCESS_DISABLED:
      switch( cr_ind->cr_type )
      {
        case CR_NORMAL:
        case CR_ABNORMAL:
          SET_STATE( CTRL, CTRL_WAIT_RSP_4_RR_CR );

          ctrl_set_old_scell( TRUE );

          if( cr_ind->cr_type NEQ CR_NORMAL )
          {
            ctrl_stop_all_activities( CTRL_DC_CR_MS_RR_IMM_REL, NULL );
          }
          else
          {
            ctrl_stop_all_activities( CTRL_DC_CR_MS_RR_NORMAL, NULL );
          }
          break;
        
        case CR_REQ_CANDIDATE:
          TRACE_ERROR( "RRGRR_CR_IND unexpected" );
          break;

        case CR_SUSPENDED_IDLE:
          /* to be defined */
          break;
      }
      break;

    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      switch( cr_ind->cr_type )
      {
        case CR_NORMAL:
        case CR_ABNORMAL:
          grr_data->ctrl.parked_rrgrr_cr_ind = TRUE;
              
          ctrl_handle_parked_rrgrr_cr_ind( state );
          break;
        
        case CR_REQ_CANDIDATE:
          TRACE_ERROR( "RRGRR_CR_IND unexpected" );
          break;

        case CR_SUSPENDED_IDLE:
          /* to be defined */
          break;
      }
      break;

    case CTRL_AUTO_RR_CR:
      switch( grr_data->ctrl.rr_state.cr_ind_return )
      {
        case CR_IND_RETURN_COMPLETE:
          grr_data->ctrl.rr_state.cr_ind_return = CR_IND_RETURN_RESPONSE;
      
          ctrl_send_cell_reselection_req( CR_COMPLETE );
          break;

        case CR_IND_RETURN_NONE:
        case CR_IND_RETURN_RESPONSE:
        default:
          TRACE_EVENT_P1( "RRGRR_CR_IND: cr_ind_return EQ %d", 
                          grr_data->ctrl.rr_state.cr_ind_return );
          break;
      }
      break;

    /*
     * RR is not allowed to do a MS controlled cell re-selection since 
     * a network controlled cell re-selection is in progress.
     */
    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
      /*
       * break;
       */

    /*
     * GRR is in a critical CTRL_WAIT_... state, just store the RRGRR_CR_IND 
     * and process is after leaving the critical state.
     */
    case CTRL_WAIT_CS_PAGE_RES:
    case CTRL_WAIT_FOR_CNF_OF_SUSPENSION:

    /*
     * RR has sent a RRGRR_GPRS_SI13_IND for a new serving cell and has to wait
     * for response of GRR ( RRGRR_CR_REQ with appropriate parameter set )
     * before sending a new cell re-selection indication.
     */
    case CTRL_READY_TO_PROGRESS:
      /*
       * It is possible to store the RRGRR_CR_IND, to wait until the above
       * states have been left and to post-process the RRGRR_CR_IND afterwards.
       * But implemenation effort is quite high and it is not clear whether
       * the situations can occur -> for further study.
       */
     
      /*
       * break;
       */
      
    /*
     * RR indicates a new cell re-selection but the previous one is not yet
     * responded by GRR, just wait for it.
     */
    case CTRL_WAIT_RSP_4_RR_CR:
    
    /*
     * The following states should never be seen when RRGRR_CR_IND has been
     * received, they are just set and reset during one primitive processing
     * cycle.
     */
    case CTRL_FAILED_CR:
    case CTRL_FAILED_CR_IN_SUSP:
    default:
      TRACE_ERROR( "RRGRR_CR_IND unexpected" );
      break;
  }

  PFREE( cr_ind );

}/* ctrl_rrgrr_cr_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_check_bsic_ind
+------------------------------------------------------------------------------
| Description : 
|               
| Parameters  : *rrgrr_check_bsic_ind - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_check_bsic_ind
                               ( T_RRGRR_NCELL_SYNC_IND *rrgrr_check_bsic_ind )
{
  TRACE_FUNCTION( "ctrl_rrgrr_check_bsic_ind" );

  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
    case CTRL_READY:
    case CTRL_CR:
    case CTRL_FAILED_CR:
    case CTRL_CR_IN_SUSP:
    case CTRL_FAILED_CR_IN_SUSP:
    case CTRL_GRR_NC_CCO:
      sig_ctrl_cs_check_bsic_ind( rrgrr_check_bsic_ind );
      break;
    default:
      TRACE_ERROR( "RRGRR_CHECK_BSIC_IND unexpected" );
      break;
  }
  
  PFREE( rrgrr_check_bsic_ind ); 
}/* ctrl_rrgrr_check_bsic_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_sync_ind
+------------------------------------------------------------------------------
| Description : This function handle the primitive rrgrr_sync_ind
|               and checks whether the synchronisation was successfully or not
|               
|               
|
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_sync_ind(T_RRGRR_SYNC_IND *rrgrr_sync_ind)               
{
  UBYTE state = GET_STATE( CTRL );

  TRACE_FUNCTION( "ctrl_rrgrr_sync_ind" );

  switch( state )
  {
    case CTRL_RR_CR_IN_SUSP:
    case CTRL_AUTO_RR_CR:
       if(rrgrr_sync_ind->sync_res EQ SYNC_OK)
       {
         /* 
          * Stop TBF etc., if running. We want to start 
          * to read PSI messages in the new cell
          */
         ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );         
         ctrl_send_access_disable_if_needed( TC_DC_OTHER, sig_tc_ctrl_tbf_release_cnf );
         grr_set_db_ptr( DB_MODE_CC_REQ );
         sig_ctrl_psi_sync_ok();
       }
       else
       {
         SET_STATE(CTRL, CTRL_READY);
         TRACE_ERROR( "Synchronisation to new NCELL failed" );
       }
      break;
    case CTRL_CR:
    case CTRL_CR_IN_SUSP:
      grr_set_db_ptr( DB_MODE_CC_REQ );

      if(rrgrr_sync_ind->sync_res EQ SYNC_OK)
      {
        /*
         * There is no information about the service state available, 
         * so we assume that full service is offered,
         * GPRS is supported anyway, because PBCCH is present
         */
        ctrl_set_gprs_service( GPRS_SUPPORTED, FALSE );

        /* 
         * We want to start 
         * to read PSI messages in the new cell
         */
        ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
        sig_ctrl_psi_read_full_psi_in_new_cell();
      }
      else
      {
        SET_STATE_FAILED_CR( state );

        ctrl_cc_rejected( );

        TRACE_ERROR( "Sync to new NCELL failed: new/no candidate" );
      }
      break;
    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
      if( rrgrr_sync_ind->sync_res EQ SYNC_OK )
      { 
        /* Start reading psi information on the new cell */
        /* sig_ctrl_psi_read_full_psi_in_new_cell( ); */
        ctrl_stop_rr_task_req( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
        grr_set_db_ptr( DB_MODE_CC_REQ );
        sig_ctrl_psi_sync_ok( );
      }
      break;
    default:
      TRACE_ERROR( "RRGRR_SYNC_IND unexpected" );
      break;
  }
  
  PFREE(rrgrr_sync_ind); 
}/* ctrl_rrgrr_sync_ind() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_meas_rep_cnf
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_meas_rep_cnf( T_RRGRR_MEAS_REP_CNF *rrgrr_meas_rep_cnf )               
{
  TRACE_FUNCTION( "ctrl_rrgrr_meas_rep_cnf" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_cs_meas_rep_cnf( rrgrr_meas_rep_cnf );
      break;
    default:
      TRACE_ERROR( "RRGRR_MEAS_REP_CNF unexpected" );
      break;
  }
  
  PFREE( rrgrr_meas_rep_cnf );
}/* ctrl_rrgrr_meas_rep_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_ext_meas_cnf
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_ext_meas_cnf ( T_RRGRR_EXT_MEAS_CNF *rrgrr_ext_meas_cnf )
{
  TRACE_FUNCTION( "ctrl_rrgrr_ext_meas_cnf" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_meas_ext_meas_cnf( rrgrr_ext_meas_cnf );
      break;
    default:
      TRACE_ERROR( "RRGRR_EXT_MEAS_CNF unexpected" );
      break;
  }
  
  PFREE( rrgrr_ext_meas_cnf );
}/* ctrl_rrgrr_ext_meas_cnf() */

  

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_attach_started_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_ATTACH_STARTED_REQ
|
| Parameters  : *gmmrr_attach_started_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_attach_started_req 
                       ( T_GMMRR_ATTACH_STARTED_REQ *gmmrr_attach_started_req )
{ 
  TRACE_FUNCTION( "ctrl_gmmrr_attach_started_req" );
  
  grr_handle_non_drx_period( GMM_NDRX, TRUE );

  PFREE( gmmrr_attach_started_req );

} /* ctrl_gmmrr_attach_started_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_attach_finished_req
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_ATTACH_FINISHED_REQ
|
| Parameters  : *gmmrr_attach_finished_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_attach_finished_req 
                     ( T_GMMRR_ATTACH_FINISHED_REQ *gmmrr_attach_finished_req )
{ 
  TRACE_FUNCTION( "ctrl_gmmrr_attach_finished_req" );
  
  grr_handle_non_drx_period( GMM_NDRX, FALSE );

  PFREE( gmmrr_attach_finished_req );

} /* ctrl_gmmrr_attach_finished_req() */

/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_ms_id_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive RRGRR_MS_ID_IND
|
| Parameters  : *ms_id - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_ms_id_ind(T_RRGRR_MS_ID_IND *ms_id)
{ 
  TRACE_FUNCTION( "ctrl_rrgrr_ms_id_ind" );


  grr_data->db.ms_id.tmsi = ms_id->tmsi;
  
  TRACE_EVENT_P1("TMSI: %d", ms_id->tmsi);
  
  PFREE(ms_id);
}/*ctrl_rrgrr_ms_id_ind*/


/*
+------------------------------------------------------------------------------
| Function    : ctrl_gmmrr_cell_res
+------------------------------------------------------------------------------
| Description : Handles the primitive GMMRR_CELL_RES
|
| Parameters  : *gmmrr_cell_res - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_gmmrr_cell_res ( T_GMMRR_CELL_RES *gmmrr_cell_res )
{ 
  TRACE_FUNCTION( "ctrl_gmmrr_cell_res" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_READY:
    case CTRL_IN_PROGRESS:
      sig_ctrl_tc_enable_grlc(CGRLC_QUEUE_MODE_LLC,gmmrr_cell_res->cu_cause);
	grr_data->cell_res_status = TRUE;
      break;
    case CTRL_RR_NC_CCO:
    case CTRL_GRR_NC_CCO:
 
      TRACE_EVENT_P1("NCC ctrl_gmmrr_cell_res: cause is %d", gmmrr_cell_res->cu_cause);
   
      if( grr_t_status( T3174 ) > 0 )
      {
        TRACE_EVENT("ctrl_gmmrr_cell_res: t3174 is running");
        sig_ctrl_tc_enable_grlc(CGRLC_QUEUE_MODE_LLC,gmmrr_cell_res->cu_cause);
              }
      else
      { 
        TRACE_EVENT("ctrl_gmmrr_cell_res: t3176 is running");

        SET_STATE( CTRL,CTRL_IN_PROGRESS );
      }
	grr_data->cell_res_status = TRUE;
      break;
    default:
      TRACE_ERROR( "GMMRR_CELL_RES unexpected" );
      break;
  }
  PFREE(gmmrr_cell_res);

} /* ctrl_gmmrr_cell_res() */


#ifdef REL99
/*
+------------------------------------------------------------------------------
| Function    : ctrl_t_poll_timer
+------------------------------------------------------------------------------
| Description : This is the timer handler. It is started when PCCO/PCCC is received
|               with RRBP set
|
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_t_poll_timer()
{

  if(grr_data->ctrl.poll_for_msg EQ GRR_PCCO_POLL)
  {
    ctrl_cell_change_order ( CCO_TYPE_GRR, (void *)&grr_data->ctrl.pcco );
  }
  
  grr_data->ctrl.poll_for_msg= 0;

  return;

}
#endif

#if defined (REL99) AND defined (TI_PS_FF_EMR)
/*
+------------------------------------------------------------------------------
| Function    : ctrl_rrgrr_si2quater_ind
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void ctrl_rrgrr_si2quater_ind( T_RRGRR_SI2QUATER_IND 
                                    *rrgrr_si2quater_ind )               
{
  TRACE_FUNCTION( "ctrl_rrgrr_si2quater_ind" );
  
  switch( GET_STATE( CTRL ) )
  {
    case CTRL_IN_PROGRESS:
      sig_ctrl_cs_si2quater_ind( rrgrr_si2quater_ind );
      break;
    default:
      TRACE_ERROR( "RRGRR_SI2QUATER_IND unexpected" );
      break;
  }
  
  PFREE( rrgrr_si2quater_ind );
}/* ctrl_rrgrr_si2quater_ind() */
#endif



