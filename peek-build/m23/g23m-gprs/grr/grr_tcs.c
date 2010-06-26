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
|             TC of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_TCS_C
#define GRR_TCS_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <string.h>
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"      /* to get the global entity definitions */
#include "grr_tcf.h"    /* to get the service TC procedure definitions */
#include "grr_tcs.h"    /* to get the service TC signal definitions */
#include "grr_ctrls.h"  /* to get the service CTRL signal definitions */
#include "grr_cpaps.h"  /* to get the service CPAP signal definitions */
#include "grr_meass.h"
#include "grr_psif.h"   /* stop timers in Testmode A */
#include "cl_rlcmac.h"

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL TYPES===========================================================*/

LOCAL void tc_handle_new_cell ( BOOL cell_has_changed, UBYTE enable_cause );

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : tc_handle_new_cell
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
LOCAL void tc_handle_new_cell ( BOOL cell_has_changed, UBYTE enable_cause )
{
  UBYTE queue_mode;

  TRACE_FUNCTION( "tc_handle_new_cell" );

  if( cell_has_changed )
  {
    /* inform CPAP because it has to stop T3142 if running */
    sig_tc_cpap_new_cell();

    grr_data->ta_params.ta_valid = FALSE;
  
    TRACE_EVENT_P1( "tc_handle_new_cell: N3102 = %d", psc_db->gprs_cell_opt.pan_struct.pmax );
  }

  queue_mode = ( enable_cause EQ CGRLC_ENAC_NORMAL ? 
                 CGRLC_QUEUE_MODE_GMM : CGRLC_QUEUE_MODE_LLC );
  
  tc_cgrlc_enable_req( queue_mode, 
                       CGRLC_RA_DEFAULT, 
                       cell_has_changed,
                       enable_cause );
} /* tc_handle_new_cell() */

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : tc_get_ctrl_blk
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE* tc_get_ctrl_blk ( UBYTE *index, BOOL unacknowledged )
{ 
  UBYTE *ctrl_blk;

  TRACE_FUNCTION( "tc_get_ctrl_blk" );

  ctrl_blk = tc_set_start_ctrl_blk( index );
 
  if( unacknowledged )
  {
    /* 
     * in case the control blocks are send by the service RU, we are 
     * working in unacknowledged mode, so every block is transmitted 
     * succesfully
     */
    tc_set_stop_ctrl_blk( TRUE, CGRLC_BLK_OWNER_NONE, 0 );
  }
  
  return( ctrl_blk );

} /* tc_get_ctrl_blk() */

/*
+------------------------------------------------------------------------------
| Function    : tc_is_ctrl_blk_rdy
+------------------------------------------------------------------------------
| Description : 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL tc_is_ctrl_blk_rdy ( UBYTE ack_cnt_meas_rpt, UBYTE ack_cnt_other )
{ 
  BOOL        result    = FALSE;
  T_BLK_INDEX blk_index = grr_data->tc.ul_ctrl_blk.seq[0];

  TRACE_FUNCTION( "tc_is_ctrl_blk_rdy" );

  if( blk_index NEQ MAX_CTRL_BLK_NUM )
  {
    switch( grr_data->tc.ul_ctrl_blk.blk[blk_index].owner )
    {
      case( CGRLC_BLK_OWNER_MEAS ):
      case( CGRLC_BLK_OWNER_CS   ): result = ( ack_cnt_meas_rpt EQ 0 ); break;
      default               : result = ( ack_cnt_other    EQ 0 ); break;
    }
  }
  
  return( result );

} /* tc_is_ctrl_blk_rdy() */

/*
+------------------------------------------------------------------------------
| Function    : tc_get_num_ctrl_blck
+------------------------------------------------------------------------------
| Description : This function returns the number of allocated control blocks.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tc_get_num_ctrl_blck ( void )
{ 
  UBYTE i   = 0; /* used for counting                  */
  UBYTE num = 0; /* number of allocated control blocks */

  TRACE_FUNCTION( "tc_get_num_ctrl_blck" );

  while( i                               <   MAX_CTRL_BLK_NUM AND 
         grr_data->tc.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
  {
    if( grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].state
                                                       EQ BLK_STATE_ALLOCATED )
    {
      num++;
    }

    i++;
  }
  
  return( num );

} /* tc_get_num_ctrl_blck() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cpy_ctrl_blk_to_buffer
+------------------------------------------------------------------------------
| Description : This function ...
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL UBYTE tc_cpy_ctrl_blk_to_buffer( UBYTE *buffer )
{ 
  UBYTE  index;
  UBYTE *ctrl_block = tc_get_ctrl_blk( &index, FALSE );

  TRACE_FUNCTION( "tc_cpy_ctrl_blk_to_buffer" );

  if( ctrl_block NEQ NULL )
  {
    memcpy( buffer, ctrl_block, BYTE_UL_CTRL_BLOCK );
    grr_data->tc.v_sb_without_tbf    = FALSE;
    /*
     * After copying Measurement reports (PIM) in single block req 
     * v_sb_without_tbf is not required
     */
  }
  else
  {
    TRACE_ERROR( "tc_cpy_ctrl_blk_to_buffer: ctrl_block EQ NULL" );
  }

  return( index );
} /* tc_cpy_ctrl_blk_to_buffer() */

/*==== SIGNAL FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_control_block
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_CONTROL_BLOCK
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_control_block ( T_BLK_OWNER   blk_owner_i, 
                                          void        * blk_struct_i )
{ 
  TRACE_ISIG( "sig_ctrl_tc_control_block " );
 
  switch( GET_STATE( TC ) )
  {
    case TC_ACCESS_DISABLED:
    case TC_CPAP:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
    case TC_WAIT_STOP_TASK_CNF:
      if( tc_store_ctrl_blk( blk_owner_i, blk_struct_i ) EQ FALSE )
      {
        sig_tc_ctrl_control_block_result( blk_owner_i, FALSE );
      }
      else
      {
        tc_send_control_msg_to_grlc();
      }
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
      if( tc_store_ctrl_blk( blk_owner_i, blk_struct_i ) EQ FALSE )
      {
        sig_tc_ctrl_control_block_result( blk_owner_i, FALSE );
      }
      else
      {
        /*
         * check if grlc has to transmit data
         */
        tc_cgrlc_access_status_req();
/*          grr_data->tc.v_sb_without_tbf    = TRUE;
          grr_data->uplink_tbf.nr_blocks   = 1;
          grr_data->uplink_tbf.prio        = RADIO_PRIO_4;
          grr_data->uplink_tbf.access_type = CGRLC_AT_SINGLE_BLOCK;
          grr_data->uplink_tbf.ti          = 0;  
          tc_check_access_is_needed( CAC_OTHER );*/
      }
      break;
    default:
      sig_tc_ctrl_control_block_result( blk_owner_i, FALSE );

      TRACE_ERROR( "SIG_CTRL_TC_CONTROL_BLOCK unexpected" );
      break;
  }
} /* sig_ctrl_tc_control_block () */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_cancel_control_block
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_CANCEL_CONTROL_BLOCK
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_cancel_control_block ( T_BLK_OWNER blk_owner_i )
{ 
  UBYTE state = GET_STATE( TC );

  TRACE_ISIG( "sig_ctrl_tc_cancel_control_block " );
 
  switch( state )
  {
    case TC_CPAP:
    case TC_WAIT_ASSIGN:
      if( tc_cancel_ctrl_blk( blk_owner_i ) EQ TRUE )
      {
        if( tc_get_num_ctrl_blck( )  EQ 0  )
        {
          if( state EQ TC_CPAP )
          {
            sig_tc_cpap_abort_access();
          }
          else
          {
            SET_STATE( TC, TC_PIM );

            tc_send_ra_stop_req( );
            vsi_t_stop( GRR_handle, T3170 );
            tc_check_access_is_needed( CAC_OTHER );
          }
        }
      }
      break;
    case TC_ACCESS_DISABLED:
    case TC_POLLING:
    case TC_PIM:
      tc_cancel_ctrl_blk( blk_owner_i );
      break;
    case TC_SINGLE_BLOCK:
    case TC_WAIT_2P_ASSIGN:
      tc_cancel_ctrl_blk( blk_owner_i );
      tc_stop_normal_burst_req( );
      break;
    case TC_TBF_ACTIVE:  
    case TC_CONTENTION_RESOLUTION:
      tc_cancel_ctrl_blk( blk_owner_i );
      tc_handle_error_ra();
      break;
    default:
      TRACE_EVENT( "SIG_CTRL_TC_CANCEL_CONTROL_BLOCK unexpected" );
      break;
  }
} /* sig_ctrl_tc_cancel_control_block () */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_enable_grlc
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_ENABLE_GRLC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_enable_grlc  (UBYTE queue_mode, UBYTE  cell_update_cause )
{ 
  TRACE_ISIG( "sig_ctrl_tc_enable_grlc" );

  if(cell_update_cause EQ GMMRR_RA_CU)
    tc_cgrlc_enable_req(queue_mode,CGRLC_RA_CU,FALSE,CGRLC_ENAC_NORMAL);
  else
    tc_cgrlc_enable_req(queue_mode,CGRLC_RA_DEFAULT,FALSE,CGRLC_ENAC_NORMAL);

} /* sig_ctrl_tc_enable_grlc() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_access_disable
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_ACCESS_DISABLE
|
| Parameters  : dc - disable cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_access_disable 
                                   ( T_TC_DISABLE_CAUSE         disable_cause,
                                     T_TC_DISABLE_CALLBACK_FUNC callback_func ) 
{ 
  UBYTE         disable_class;

  T_TC_DC_CLASS dc_class =          ( disable_cause & DCM_CLASS );
  T_TC_DC_TYPE  dc_type  = ( UBYTE )( disable_cause & DCM_TYPE  );
  UBYTE         state    = GET_STATE( TC );
  UBYTE         prim_st  = CGRLC_PRIM_STATUS_NULL;

  TRACE_ISIG( "sig_ctrl_tc_access_disable" );

  disable_class =
    ( dc_class EQ DCC_CR ? CGRLC_DISABLE_CLASS_CR : CGRLC_DISABLE_CLASS_OTHER );

  if( grr_data->tc.disable_class EQ  CGRLC_DISABLE_CLASS_NULL OR 
      disable_class              NEQ CGRLC_DISABLE_CLASS_CR      )
  {
    grr_data->tc.disable_class = disable_class;
  }
  if ( disable_cause EQ TC_DC_CR_NETWORK)
  {
    TRACE_EVENT("T3170 stopped if disable cause is NW CR");
    vsi_t_stop(GRR_handle,T3170);
  }
  switch( state )
  {
    case TC_CPAP:
      sig_tc_cpap_abort_access();

      SET_STATE( TC, TC_ACCESS_DISABLED );

      tc_cgrlc_disable_req(prim_st);

      callback_func( );
      break;

    case TC_WAIT_2P_ASSIGN:
      vsi_t_stop( GRR_handle, T3168 );
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      if(grr_data->tbf_type EQ CGRLC_TBF_MODE_2PA)
      {
        tc_abort_tbf( grr_data->tbf_type );
      }
      else
#endif
      {
        tc_stop_normal_burst_req( );        
      }

      SET_STATE( TC, TC_ACCESS_DISABLED );

      if( dc_class EQ DCC_SUSPEND )
      {
        prim_st = CGRLC_PRIM_STATUS_ONE; 
      }
      tc_cgrlc_disable_req(prim_st);

      grr_data->tc.disable_callback_func = callback_func;
      break;

    case TC_POLLING:
    case TC_WAIT_ASSIGN:
      tc_send_ra_stop_req( );

      SET_STATE( TC, TC_ACCESS_DISABLED );

      if( dc_class EQ DCC_SUSPEND )
      {
        prim_st = CGRLC_PRIM_STATUS_ONE;
      }
      tc_cgrlc_disable_req(prim_st);

      callback_func( );
      break;

    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_EVENT_P1( "SIG_CTRL_TC_ACCESS_DISABLE: %s of TBF",
                      dc_type EQ DCT_NORMAL ? "normal release" : "abort" );

      if(!grr_data->tc.two_2p_w_4_tbf_con AND
         ( dc_type NEQ DCT_NORMAL ))
      {
        TRACE_EVENT("SB SUSPEND Before TBF_C");
        tc_stop_normal_burst_req( );
        SET_STATE( TC, TC_ACCESS_DISABLED );       
        if( dc_class EQ DCC_SUSPEND )
        {
          prim_st = CGRLC_PRIM_STATUS_ONE; 
        }
        tc_cgrlc_disable_req(prim_st);
      }
      else if( dc_type NEQ DCT_NORMAL )
      {
        if( grr_data->tc.num_of_rels_running EQ  0             OR
            grr_data->tbf_type               NEQ CGRLC_TBF_MODE_NULL    )
        {
          tc_abort_tbf( grr_data->tbf_type );

          SET_STATE( TC, TC_ACCESS_DISABLED );
        }
        tc_cgrlc_disable_req(prim_st);
      }

      grr_data->tc.disable_callback_func = callback_func;
      break;

    case TC_SINGLE_BLOCK:
      tc_stop_normal_burst_req( );
      tc_set_stop_all_ctrl_blk( );

      tc_cgrlc_disable_req(prim_st);

      SET_STATE( TC, TC_ACCESS_DISABLED );

      grr_data->tc.disable_callback_func = callback_func;
      break;

    case TC_PIM:
     
      tc_cgrlc_disable_req(prim_st);

      if( grr_data->tc.num_of_rels_running NEQ 0 )
      {
        /* 
         * disable request during TBF release procedure,
         * GRR is waiting for RRGRR_STOP_TASK_CNF
         */
        TRACE_EVENT( "SIG_CTRL_TC_ACCESS_DISABLE: TBF release procedure is running" );

        grr_data->tc.disable_callback_func = callback_func;
      }
      else
      {
        SET_STATE( TC, TC_ACCESS_DISABLED );

        callback_func( );
      }
      break;

    default:
      TRACE_EVENT_P1( "SIG_CTRL_TC_ACCESS_DISABLE in TC state %d", state );

      /*lint -fallthrough*/

    case TC_ACCESS_DISABLED:
    case TC_WAIT_STOP_TASK_CNF:

      tc_cgrlc_disable_req(prim_st);

      SET_STATE( TC, TC_ACCESS_DISABLED );
      
      if( grr_data->tc.disable_callback_func EQ NULL )
      {
        callback_func( );
      }
      else
      {
        grr_data->tc.disable_callback_func = callback_func;
      }
      break;
  }
#ifdef _SIMULATION_
      tc_stop_timer_t3172();
#endif
} /* sig_ctrl_tc_access_disable() */






/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_error_pim
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_ERROR_PIM
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_error_pim ( void ) 
{ 
  TRACE_ISIG( "sig_cpap_tc_error_pim" );

  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      SET_STATE( TC, TC_PIM );

      tc_handle_error_pim();
      break;
    
    default:
      TRACE_ERROR( "SIG_CPAP_TC_ERROR_PIM unexpected" );
      break;
  }
} /* sig_cpap_tc_error_pim() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_tbf_created
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_TBF_CREATED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_tbf_created ( T_TBF_TYPE tbf_type ) 
{ 
  TRACE_ISIG( "sig_cpap_tc_tbf_created" );

  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      switch(tbf_type)
      {
        case CGRLC_TBF_MODE_UL:
          SET_STATE(TC,TC_CONTENTION_RESOLUTION);
          tc_activate_tbf(tbf_type);
          tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_UL,CGRLC_PRIM_STATUS_NULL);
          break;
        case CGRLC_TBF_MODE_DL:
          SET_STATE(TC,TC_TBF_ACTIVE);
          tc_activate_tbf(tbf_type);
          tc_cgrlc_dl_tbf_req();
          break;
        default:
          TRACE_ERROR("unknown tbf type! sig_cpap_tc_tbf_created");
          SET_STATE(TC,TC_PIM);
          break;
      }
      break;
    case TC_PIM:
      switch(tbf_type)
      {
        case CGRLC_TBF_MODE_UL:
          TRACE_ERROR("unexpected tbf type! sig_cpap_tc_tbf_created");
          break;
        case CGRLC_TBF_MODE_DL:
          SET_STATE(TC,TC_TBF_ACTIVE);
          tc_activate_tbf(tbf_type);
          tc_cgrlc_dl_tbf_req();
          break;
        default:
          TRACE_ERROR("unknown tbf type! sig_cpap_tc_tbf_created 2");
          SET_STATE(TC,TC_PIM);
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_CPAP_TC_TBF_CREATED unexpected" );
      break;
  }
} /* sig_cpap_tc_tbf_created() */





/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_ia_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_IA_RECEIVED
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_ia_received (T_RRGRR_IA_IND *rrgrr_ia_ind )
{ 
  TRACE_ISIG( "sig_ctrl_tc_ia_received" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      sig_tc_cpap_ia_received(rrgrr_ia_ind);
      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_ia_received unexpected" );
      break;
  }
} /* sig_ctrl_tc_ia_received() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_iaext_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_IAEXT_RECEIVED
|
| Parameters  : UBYTE indicates the valid immediate assignment
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_iaext_received ( T_RRGRR_IAEXT_IND *rrgrr_iaext_ind )
{ 
  TRACE_ISIG( "sig_ctrl_tc_iaext_received" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_cpap_iaext_received ( rrgrr_iaext_ind);
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:

      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_iaext_received unexpected" );
      break;
  }
} /* sig_ctrl_tc_iaext_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_iarej_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal sig_ctrl_tc_iarej_received 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_iarej_received(T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind )
{ 
  TRACE_ISIG( "sig_ctrl_tc_iarej_received" );
   
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_cpap_iarej_received(rrgrr_assignment_rej_ind);
      if(grr_data->cc_running)
      {
         SET_STATE(TC,TC_ACCESS_DISABLED)
         /*
          * todo:why state change: grlc will be disabled by sending ctrl_tc_disable
          */
      }
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:

      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_iarej_received unexpected" );
      break;
  }

} /* sig_ctrl_tc_iarej_received() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_ia_downlink_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_IA_DOWNLINK_RECEIVED 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_ia_downlink_received (T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_in )
{ 
  TRACE_ISIG( "sig_ctrl_tc_ia_downlink_received" );

 
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      sig_tc_cpap_ia_downlink_received(rrgrr_ia_downlink_in);
      break;
    default:
      sig_tc_ctrl_send_stop_task_req( INVALID_MSG );
      TRACE_EVENT_P1( "sig_ctrl_tc_ia_downlink_received unexpected tc_state=%d", grr_data->tc.state);
      TRACE_ERROR( "sig_ctrl_tc_ia_downlink_received unexpected" );
      break;
  }
  
} /* sig_ctrl_tc_ia_downlink_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_stop_task_cnf_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_STOP_TASK_CNF
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_stop_task_cnf_received ( void )
{ 
  TRACE_ISIG( "sig_ctrl_tc_stop_task_cnf_received" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
    case TC_PIM:
      sig_tc_cpap_stop_task_cnf_received();
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_TC_STOP_TASK_CNF unexpected" );
      break;
  }
} /* sig_ctrl_tc_stop_task_cnf_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_send_stop_task_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_STOP_TASK_REQ
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_send_stop_task_req( UBYTE v_stop_ccch  )
{ 
  TRACE_ISIG( "sig_cpap_tc_send_stop_task_req" );
  
  sig_tc_ctrl_send_stop_task_req( v_stop_ccch );

} /* sig_cpap_tc_send_stop_task_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_channel_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_CHANNEL_REQ 
|
| Parameters  :  req_data - data which RR has to send
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_channel_req(UBYTE req_data )
{ 
  TRACE_ISIG( "sig_cpap_tc_channel_req" );
 
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_ctrl_channel_req(req_data); 
      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_channel_req unexpected" );
      break;
  }
} /* sig_cpap_tc_channel_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_assign_sb
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_ASSIGN_SB 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_assign_sb(void *ptr2prim, UBYTE purpose)
{ 
  TRACE_ISIG( "sig_cpap_tc_assign_sb" );

  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      if(TWO_PHASE_ACESS EQ purpose)
      {
        SET_STATE(TC,TC_WAIT_2P_ASSIGN);
      }
      else
      {
        SET_STATE(TC,TC_SINGLE_BLOCK);
      }

      sig_tc_meas_update_pch( );

      {
        T_MPHP_SINGLE_BLOCK_REQ* mphp_single_block_req = ( T_MPHP_SINGLE_BLOCK_REQ* )ptr2prim;
        
        TRACE_BINDUMP
          ( hCommGRR, TC_USER4,
            cl_rlcmac_get_msg_name
              ( ( UBYTE )( mphp_single_block_req->l2_frame[1] >> 2 ), RLC_MAC_ROUTE_UL ),
            mphp_single_block_req->l2_frame, MAX_L2_FRAME_SIZE ); /*lint !e569*/
      }

      PSEND(hCommL1,ptr2prim);
      break;
    case TC_PIM:
      if(SINGLE_BLOCK_TRANSFER_DL EQ purpose)
      {
        SET_STATE(TC,TC_SINGLE_BLOCK);
        PSEND(hCommL1,ptr2prim);
      }
      else
      { /* this shall never happen! */
        TRACE_ERROR("FATAL ERROR: single block assignment in wrong state!");
        PFREE(ptr2prim);
      }
      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_assign_sb unexpected" );
      PFREE(ptr2prim);
      break;
  }
} /* sig_cpap_tc_assign_sb() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_pdch_assign_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_PDCH_ASSIGN_RECEIVED 
|
| Parameters  : fn - reception frame number
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_pdch_assign_received( T_RRGRR_DATA_IND *rrgrr_data_ind )
{ 
  TRACE_ISIG( "sig_ctrl_tc_pdch_assign_received" );

  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_cpap_pdch_assign_received(rrgrr_data_ind);
      break;

    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_pdch_assign_received unexpected" );
      break;
  }
} /* sig_ctrl_tc_pdch_assign_received() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_dcch_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_DCCH_DATA_REQ 
|
| Parameters  : pointer - to primitive with data 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_dcch_data_req( T_RRGRR_DATA_REQ * rrgrr_data_req_i )
{ 
  TRACE_ISIG( "sig_cpap_tc_dcch_data_req" );
 
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_ctrl_dcch_data_req(rrgrr_data_req_i);
      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_dcch_data_req unexpected" );
      break;
  }
} /* sig_cpap_tc_dcch_data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_assign_pdch
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_ASSIGN_PDCH 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_assign_pdch(T_MPHP_ASSIGNMENT_REQ *ptr2prim,
                                    T_PDCH_ASS_CAUSE cause )
{ 
  TRACE_ISIG( "sig_cpap_tc_assign_pdch" );

  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      grr_data->tc.dcch_present = FALSE;
      if( (PDCH_UL_ASS_CMD EQ cause) OR (PDCH_DL_ASS_CMD EQ cause) )
      {
        grr_data->tc.dcch_present = TRUE;
      }

      PSEND(hCommL1,ptr2prim);
      break;
    case TC_PIM:
      grr_data->tc.dcch_present = FALSE;
      if((IA_UL EQ cause) OR (PDCH_UL_ASS_CMD EQ cause))
      {
        PFREE(ptr2prim);
        TRACE_ERROR( "uplink assignments unexpected in PIM" );
      }
      else
      {        
        if((PDCH_DL_ASS_CMD EQ cause))
          grr_data->tc.dcch_present = TRUE;

        PSEND(hCommL1,ptr2prim);
      }
      break;
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_cpap_tc_assign_pdch unexpected" );
      PFREE(ptr2prim);
      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_assign_pdch unexpected" );
      FREE(ptr2prim);
      break;
  }
} /* sig_cpap_tc_assign_pdch() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_suspend_dedi_chan
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_SUSPEND_DEDI_CHAN 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_suspend_dedi_chan(void )
{ 
  TRACE_ISIG( "sig_cpap_tc_suspend_dedi_chan" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_ctrl_suspend_dedi_chan();
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_cpap_tc_suspend_dedi_chan unexpected" );

      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_suspend_dedi_chan unexpected" );
      break;
  }
} /* sig_cpap_tc_suspend_dedi_chan() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_resume_dedi_chan
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_RESUME_DEDI_CHAN 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_resume_dedi_chan(void )
{ 
  TRACE_ISIG( "sig_cpap_tc_resume_dedi_chan" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_ctrl_resume_dedi_chan();
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_cpap_tc_resume_dedi_chan unexpected" );

      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_resume_dedi_chan unexpected" );
      break;
  }
} /* sig_cpap_tc_resume_dedi_chan() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_send_gprs_data_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_SEND_GPRS_DATA_REQ 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_send_gprs_data_req(T_RRGRR_GPRS_DATA_REQ *rrgrr_gprs_data_req)
{ 
  TRACE_ISIG( "sig_cpap_tc_send_gprs_data_req" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_ctrl_gprs_data_req(rrgrr_gprs_data_req);
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_cpap_tc_send_gprs_data_req unexpected" );

      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_send_gprs_data_req unexpected" );
      break;
  }
} /* sig_cpap_tc_send_gprs_data_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_suspend_dcch_cnf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_SUSPEND_DCCH_CNF 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_suspend_dcch_cnf(void)
{ 
  TRACE_ISIG( "sig_ctrl_tc_suspend_dcch_cnf" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      sig_tc_cpap_suspend_dcch_cnf();
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_ctrl_tc_suspend_dcch_cnf unexpected" );

      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_suspend_dcch_cnf unexpected" );
      break;
  }
} /* sig_ctrl_tc_suspend_dcch_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_build_res_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_BUILD_RES_REQ 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_build_res_req(T_U_RESOURCE_REQ *ptr2res_req)
{ 
  TRACE_ISIG( "sig_cpap_tc_build_res_req" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
      tc_build_res_req( ptr2res_req,
                        R_BUILD_2PHASE_ACCESS,
                        SRC_TBF_INFO_IMM_ASSIGN ); 
      break;
    case TC_SINGLE_BLOCK:
    case TC_PIM:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
    default:
      TRACE_ERROR( "sig_cpap_tc_build_res_req unexpected" );
      break;
  }
} /* sig_cpap_tc_build_res_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_ctrl_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_CTRL_PTM 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_ctrl_ptm(void)
{ 
  TRACE_ISIG( "sig_cpap_tc_ctrl_ptm" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CPAP:
    case TC_PIM:
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_TRANSFER, TASK_STOP_DUMMY_VALUE );
      break;
    case TC_SINGLE_BLOCK:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      TRACE_ERROR( "sig_cpap_tc_ctrl_ptm unexpected" );

      break;
    default:
      TRACE_ERROR( "sig_cpap_tc_ctrl_ptm unexpected" );
      break;
  }
} /* sig_cpap_tc_ctrl_ptm() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_fix_alloc_ack_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_TC_FIX_ALLOC_ACK_PTM 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_fix_alloc_ack_ptm( void )
{ 
  MCAST(d_ul_ack,D_UL_ACK);
  TRACE_ISIG( "sig_ru_tc_fix_alloc_ack" );

  grr_data->uplink_tbf.cs_mode = d_ul_ack->gprs_ul_ack_nack_info.chan_coding_cmd;
 
  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      if(d_ul_ack->gprs_ul_ack_nack_info.v_f_alloc_ack EQ 0 )
      {
        /*
         * if no fix alloc is present in last uplink ack/nack, delete any existing repeat alloc
         */
        if(grr_data->tc.fa_ctrl.fa_type EQ FA_REPEAT)
        {
          /*kill reapeat alloc, which is not yet started*/
          grr_data->tc.fa_ctrl.fa_type        = FA_NO_NEXT;
          grr_data->tc.fa_ctrl.repeat_alloc   = FALSE;
          tc_send_ul_repeat_alloc_req();
        }
        else if(grr_data->tc.fa_ctrl.fa_type EQ FA_NO_NEXT AND 
                grr_data->tc.fa_ctrl.repeat_alloc  EQ TRUE )
        {
          if(!grr_check_if_tbf_start_is_elapsed ( grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn, grr_data->ul_fn))
            /*grr_data->tc.fa_ctrl.ul_res_remain EQ grr_data->tc.fa_ctrl.current_alloc.ul_res_sum*/
          {
            /*kill reapeat alloc if is not yet started*/
            grr_data->tc.fa_ctrl.fa_type = FA_NO_CURRENT;
            grr_data->tc.fa_ctrl.repeat_alloc   = FALSE;
            tc_send_ul_repeat_alloc_req();
          }
        }
        return;
      }

      /*
       * check if current allocation is active 
       */
      if(grr_data->tc.fa_ctrl.fa_type EQ FA_NO_CURRENT  AND
         d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_ts_overr )
      {
        ULONG next_start_fn;
        ULONG next_end_fn;
        /* repeat allocation in packet uplink ack/nack received,
         * no fixed allocation active, find out the next natural boundary
         * and store new allocation in current allocation
         */
        grr_data->tc.fa_ctrl.fa_type        = FA_NO_NEXT;
        grr_data->tc.fa_ctrl.repeat_alloc   = TRUE;
        grr_data->tc.fa_ctrl.ts_overr       = d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr;
        tc_set_fa_bitmap((UBYTE)(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr & grr_data->uplink_tbf.ts_usage),
                            &grr_data->tc.fa_ctrl.current_alloc);
        /*find  start of alloc (natural boundary of next allocation)*/
        next_end_fn = grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn;
        do
        {
          USHORT len;
          len = grr_data->tc.fa_ctrl.current_alloc.alloc.size_bitmap-1;
          next_start_fn = grr_decode_tbf_start_rel(next_end_fn,0);
          next_end_fn   = grr_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
          if(grr_data->ul_fn EQ next_start_fn)
            break;
          else if((next_end_fn > next_start_fn)   AND 
                  (grr_data->ul_fn <= next_start_fn) AND
                  (grr_data->ul_fn > next_start_fn) )
          {
            next_start_fn = grr_decode_tbf_start_rel(next_end_fn,0);
            next_end_fn   = grr_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
            break;
          }
          else if( (next_end_fn < next_start_fn) AND
                   (
                    ((next_end_fn >= grr_data->ul_fn) AND (next_start_fn > grr_data->ul_fn)) OR
                    ((next_end_fn <  grr_data->ul_fn) AND (next_start_fn < grr_data->ul_fn))
                   )
                 )
          {
            next_start_fn = grr_decode_tbf_start_rel(next_end_fn,0);
            next_end_fn = grr_decode_tbf_start_rel(next_start_fn,(USHORT)(len-1));
            break;
          }
        }
        while (1);
        grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn = next_start_fn;
        grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn   = next_end_fn;          
        tc_send_ul_repeat_alloc_req();
        return;
      }
      else if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_ts_overr )
      {
        UBYTE len;
        /* repeat allocation in packet uplink ack/nack received,
         * current allocation is active, store new alloc in next alloc
         * overwriting is possibe.
         */        
        grr_data->tc.fa_ctrl.fa_type        = FA_REPEAT;
        grr_data->tc.fa_ctrl.repeat_alloc   = TRUE;
        grr_data->tc.fa_ctrl.ts_overr       = d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr;
        tc_set_fa_bitmap((UBYTE)(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.ts_overr & grr_data->uplink_tbf.ts_usage),&grr_data->tc.fa_ctrl.current_alloc);
        
        grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn = 
            grr_decode_tbf_start_rel(grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn,0);
        len = grr_data->tc.fa_ctrl.current_alloc.alloc.size_bitmap-1;
        grr_data->tc.fa_ctrl.current_alloc.alloc_end_fn   =
            grr_decode_tbf_start_rel(grr_data->tc.fa_ctrl.current_alloc.alloc_start_fn,
                                     (USHORT)(len-1));
        tc_send_ul_repeat_alloc_req();
        return;
      }
      grr_data->tc.fa_ctrl.repeat_alloc   = FALSE;
      if( d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.v_fa_s2 )      
      {
        /*
         * new fixed allocation with allocation bitmap
         */
        if(d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.v_ts_alloc)
        {
          UBYTE tx_slots;
          tx_slots = grr_calc_nr_of_set_bits (d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.ts_alloc);
          grr_data->uplink_tbf.nts = tx_slots;
          grr_data->uplink_tbf.ts_usage = d_ul_ack->gprs_ul_ack_nack_info.f_alloc_ack.fa_s2.ts_alloc;
          if(!handle_ms_cap(UL_ASSIGNMENT))
          {
            TRACE_ERROR( "TS_ALLOC not valid" );
            return;
          }
        }
        tc_handle_tbf_start(CGRLC_TBF_MODE_UL);
      }
      else
      {
        TRACE_EVENT("NO FIX ALLOC STRUCT IN D_UL_ACK " );
      }
      break;
    default:
      TRACE_ERROR( "sig_ru_tc_fix_alloc_ack unexpected" );
      break;
  }



} /* sig_gfp_tc_fix_alloc_ack_ptm() */



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_reconnect_cnf
+------------------------------------------------------------------------------
| Description : Cell reselection has failled and we have to inform cpap 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_ctrl_tc_reconnect_dcch_cnf()
{
  TRACE_ISIG( "sig_ctrl_tc_reconnect_cnf" );
  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
    case TC_WAIT_ASSIGN:
      SET_STATE(TC, TC_ACCESS_DISABLED);
      sig_tc_cpap_reconnect_ind();
      break;
    default:
      TRACE_ERROR( "sig_ctrl_tc_reconnect_cnf unexpected" );
  }
}
/*sig_ctrl_tc_reconnect_dcch_cnf()*/

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_cr_timerout_cnf
+------------------------------------------------------------------------------
| Description : Cell reselection has failled because of timer out 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_ctrl_tc_cr_timerout_cnf()
{
  TRACE_ISIG( "sig_ctrl_tc_cr_timerout_cnf" );
  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:    
    case TC_TBF_ACTIVE:
    case TC_WAIT_ASSIGN:
      tc_abort_tbf(grr_data->tbf_type);
      SET_STATE(TC, TC_ACCESS_DISABLED);
      sig_tc_cpap_reconnect_ind();
      break;
    default:
      TRACE_ERROR( "sig_ctrl_cr_timerout_cnf unexpected" );
 }
   

}/*sig_ctrl_tc_cr_timerout_cnf()*/



/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_abnorm_rel_with_cr_failed
+------------------------------------------------------------------------------
| Description : Handles the internal signal 
|               SIG_CTRL_TC_ABNORM_REL_WITH_CR_FAILED
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sig_ctrl_tc_abnorm_rel_with_cr_failed ( BOOL cell_has_changed )
{ 
  TRACE_ISIG( "sig_ctrl_tc_abnorm_rel_with_cr_failed" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_ACCESS_DISABLED:
      SET_STATE_TC_PIM_AFTER_ACCESS_DISABLED
              ( cell_has_changed, CGRLC_ENAC_ABNORM_RELEASE_CRESELECT_FAILED );
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_TC_ABNORM_REL_WITH_CR_FAILED unexpected" );
      break;
  }

  return( TRUE );
} /* sig_ctrl_tc_abnorm_rel_with_cr_failed() */



/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_start_T3142
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_START_T3142,
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_start_T3142(ULONG wait)
{ 
  TRACE_ISIG( "sig_cpap_tc_start_T3142" );

  tc_start_timer_t3172(wait);
  sig_tc_ctrl_t3172_running();

} /* sig_cpap_tc_start_T3142 */



/*
+------------------------------------------------------------------------------
| Function    : sig_cpap_tc_stop_T3142
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CPAP_TC_STOP_T3142,
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_cpap_tc_stop_T3142(void)
{ 
  TRACE_ISIG( "sig_cpap_tc_stop_T3142" );


} /* sig_cpap_tc_stop_T3142 */




/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_disable_prim_queue
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_DISABLE_PRIM_QUEUE
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL BOOL sig_ctrl_tc_disable_prim_queue ( BOOL cell_has_changed ) 
{ 
  TRACE_EVENT( "sig_ctrl_tc_disable_prim_queue" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_ACCESS_DISABLED:
      SET_STATE_TC_PIM_AFTER_ACCESS_DISABLED( cell_has_changed,
                                              CGRLC_ENAC_NORMAL );
      break;
    default:
      TRACE_ERROR( "SIG_CTRL_TC_DISABLE_PRIM_QUEUE unexpected" );
      break;
  }

  return( TRUE);
} /* sig_ctrl_tc_disable_prim_queue() */





/*
+------------------------------------------------------------------------------
| Function    : sig_meas_tc_update_ta_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_MEAS_TC_UPDATE_TA_REQ.
|               New TA values are received, it shall passed to l1
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_meas_tc_update_ta_req ( void)
{ 
  MCAST(d_ctrl_pwr_ta,D_CTRL_PWR_TA); /* T_D_CTRL_PWR_TA */

  TRACE_FUNCTION( "tc_send_reconf_tbf_ta_req" );

  TRACE_EVENT_P9("TAU: %d tav=%d - %d %d %d - %d %d %d tbf_type=%d"
                                                                  ,d_ctrl_pwr_ta->gpta.v_ta_value
                                                                  ,d_ctrl_pwr_ta->gpta.ta_value
                                                                  ,d_ctrl_pwr_ta->gpta.v_ul_ta_index
                                                                  ,d_ctrl_pwr_ta->gpta.ul_ta_index
                                                                  ,d_ctrl_pwr_ta->gpta.ul_ta_tn
                                                                  ,d_ctrl_pwr_ta->gpta.v_dl_ta_index
                                                                  ,d_ctrl_pwr_ta->gpta.dl_ta_index
                                                                  ,d_ctrl_pwr_ta->gpta.dl_ta_tn
                                                                  ,grr_data->tbf_type );
  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_DL_UL:
      {
        PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */
        mphp_timing_advance_req->assign_id = 0;
        grr_handle_ta ( d_ctrl_pwr_ta->gpta.v_ta_value, 
                        d_ctrl_pwr_ta->gpta.ta_value,
                        d_ctrl_pwr_ta->gpta.v_ul_ta_index, 
                        d_ctrl_pwr_ta->gpta.ul_ta_index,
                        d_ctrl_pwr_ta->gpta.ul_ta_tn,
                        d_ctrl_pwr_ta->gpta.v_dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_tn,
                        &mphp_timing_advance_req->p_timing_advance);
        PSEND(hCommL1,mphp_timing_advance_req);
      }
      break;
    case CGRLC_TBF_MODE_DL:
      {
        PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */
        mphp_timing_advance_req->assign_id = 0;
        if(d_ctrl_pwr_ta->gpta.v_ul_ta_index)
        {
          d_ctrl_pwr_ta->gpta.v_ul_ta_index = 0;
          TRACE_EVENT("TAU: only dl tbf is running, ul ta index present");
        }
        if(!d_ctrl_pwr_ta->gpta.v_dl_ta_index)
        {        
          TRACE_EVENT("TAU: only dl tbf is running, no dl ta index present");
        }
        grr_handle_ta ( d_ctrl_pwr_ta->gpta.v_ta_value, 
                        d_ctrl_pwr_ta->gpta.ta_value,
                        d_ctrl_pwr_ta->gpta.v_ul_ta_index, 
                        d_ctrl_pwr_ta->gpta.ul_ta_index,
                        d_ctrl_pwr_ta->gpta.ul_ta_tn,
                        d_ctrl_pwr_ta->gpta.v_dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_tn,
                        &mphp_timing_advance_req->p_timing_advance);
        PSEND(hCommL1,mphp_timing_advance_req);
      }
      break;
    case CGRLC_TBF_MODE_UL:
      {
        PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */
        mphp_timing_advance_req->assign_id = 0;
        if(d_ctrl_pwr_ta->gpta.v_dl_ta_index)
        {
          d_ctrl_pwr_ta->gpta.v_dl_ta_index = 0;
          TRACE_EVENT("TAU: only ul tbf is running, dl ta index present");
        }
        if(!d_ctrl_pwr_ta->gpta.v_ul_ta_index)
        {        
          TRACE_EVENT("TAU: only ul tbf is running, no ul ta index present");
        }
        grr_handle_ta ( d_ctrl_pwr_ta->gpta.v_ta_value, 
                        d_ctrl_pwr_ta->gpta.ta_value,
                        d_ctrl_pwr_ta->gpta.v_ul_ta_index, 
                        d_ctrl_pwr_ta->gpta.ul_ta_index,
                        d_ctrl_pwr_ta->gpta.ul_ta_tn,
                        d_ctrl_pwr_ta->gpta.v_dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_index,
                        d_ctrl_pwr_ta->gpta.dl_ta_tn,
                        &mphp_timing_advance_req->p_timing_advance);
        PSEND(hCommL1,mphp_timing_advance_req);
      }
      break;
    default:
      TRACE_ERROR ( "sig_meas_tc_update_ta_req: TBF type is invalid" );
      TRACE_EVENT_P1 ( "TBF type = %d",grr_data->tbf_type );
      break;
  }
} /* sig_meas_tc_update_ta_req() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_update_ta_req_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_GFP_TC_UPDATE_TA_REQ_PTM.
|               New TA values are received, it shall passed to l1
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_update_ta_req_ptm ( void)
{
  MCAST(d_ul_ack,D_UL_ACK);
  PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */

  TRACE_FUNCTION( "tc_send_reconf_tbf_ta_req" );
   
  mphp_timing_advance_req->assign_id = 0; 

  grr_handle_ta ( d_ul_ack->gprs_ul_ack_nack_info.pta.v_ta_value, 
                  d_ul_ack->gprs_ul_ack_nack_info.pta.ta_value,
                  d_ul_ack->gprs_ul_ack_nack_info.pta.v_ta_index_tn, 
                  d_ul_ack->gprs_ul_ack_nack_info.pta.ta_index_tn.ta_index,
                  d_ul_ack->gprs_ul_ack_nack_info.pta.ta_index_tn.ta_tn,
                  0xFF,
                  0,
                  0,
                  &mphp_timing_advance_req->p_timing_advance);
  PSEND(hCommL1,mphp_timing_advance_req);
} /* sig_gfp_tc_update_ta_req_ptm() */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_enter_pam
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_ENTER_PAM.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_enter_pam ( void )
{
  TRACE_ISIG( "sig_ctrl_tc_enter_pam" );

  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_STOP_TASK_CNF:
      if( grr_is_pbcch_present( ) )
      {
        if( grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_DL )
        {
          sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ASSIGNED,
                                     grr_data->tc.last_eval_assign.ccch_read );
        }
        else
        {
          SET_STATE( TC, TC_WAIT_ASSIGN );
          vsi_t_start( GRR_handle, T3186, T3186_VALUE );
          tc_send_ra_req( );
        }
      }
      else
      {
        SET_STATE( TC, TC_CPAP );
        sig_tc_cpap_acc_req( );
      }
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_TC_ENTER_PAM unexpected" );
      break;
  }
} /* sig_ctrl_tc_enter_pam() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_leave_to_ptm
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_LEAVE_TO_PTM
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_leave_to_ptm ( void )
{ 
  TRACE_ISIG( "sig_ctrl_tc_leave_to_ptm" );

  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_STOP_TASK_CNF:
      tc_mfree_assign( TRUE );

#ifdef _SIMULATION_

      switch( grr_data->tc.last_eval_assign.state )
      {
        case TC_ACCESS_DISABLED      : SET_STATE( TC, TC_ACCESS_DISABLED );       break;
        case TC_CPAP                 : SET_STATE( TC, TC_CPAP );                  break;
        case TC_PIM                  : SET_STATE( TC, TC_PIM );                   break;
        case TC_WAIT_ASSIGN          : SET_STATE( TC, TC_WAIT_ASSIGN );           break;
        case TC_POLLING              : SET_STATE( TC, TC_POLLING );               break;
        case TC_WAIT_2P_ASSIGN       : SET_STATE( TC, TC_WAIT_2P_ASSIGN );        break;
        case TC_CONTENTION_RESOLUTION: SET_STATE( TC, TC_CONTENTION_RESOLUTION ); break;
        case TC_TBF_ACTIVE           : SET_STATE( TC, TC_TBF_ACTIVE);             break;
        case TC_SINGLE_BLOCK         : SET_STATE( TC, TC_SINGLE_BLOCK);           break;
        case TC_WAIT_STOP_TASK_CNF   : SET_STATE( TC, TC_WAIT_STOP_TASK_CNF);     break;

        default:
          SET_STATE( TC, grr_data->tc.last_eval_assign.state );
          TRACE_ERROR( "sig_ctrl_tc_leave_to_ptm: unknown TC state" );
          break;
      }

#else  /* #ifdef _SIMULATION_ */

      SET_STATE( TC, grr_data->tc.last_eval_assign.state );

#endif /* #ifdef _SIMULATION_ */

      tc_handle_tbf_start( grr_data->tc.last_eval_assign.tbf_type );
      break;

    default:
      TRACE_ERROR( "SIG_CTRL_TC_LEAVE_TO_PTM unexpected" );
      break;
  }
} /* sig_ctrl_tc_leave_to_ptm() */

/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_leave_to_pim
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_CTRL_TC_LEAVE_TO_PIM
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_ctrl_tc_leave_to_pim ( void )
{ 
  TRACE_ISIG( "sig_ctrl_tc_leave_to_pim" );

  tc_send_tbf_release_req( grr_data->tc.last_tbf_type, FALSE );

} /* sig_ctrl_tc_leave_to_pim() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_access_rej_ptm
+------------------------------------------------------------------------------
| Description : Handles the signal SIG_GFP_TC_ACCESS_REJ_PTM
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_access_rej_ptm ( void )
{ 
  ULONG t3172_value=0;

  TRACE_ISIG( "sig_gfp_tc_access_rej_ptm" );
  TRACE_EVENT("ACC REJ PACCH" );

  
  switch(grr_data->tbf_type)
  {
    case CGRLC_TBF_MODE_UL:
      /*
       * wait for the end of the current UL TBF
       */
      if(grr_t_status( T3172_1 ) EQ 0)
      {
        TRACE_EVENT("Return to idle mode after release of DA");

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        if(grr_data->tc.tbf_est_pacch AND grr_t_status( T3170) EQ 0 )
        {
          /* GRLC has sent PCA or PRR. PAR received before PUA */
          vsi_t_start(GRR_handle,T3170,tc_get_t3170_value());

        }
#endif
      }

      /*lint -fallthrough*/

    case CGRLC_TBF_MODE_DL_UL:

      /*
       * UL TBF ends before the number of requested octets has been transferred, as if FINAL_ALLOCATION bit
       * go to idle mode after release
       * stop 3168
       * check 3172
       * Before initiating a new packet access procedure the mobile station shall decode the 
       * PRACH Control Parameters if they are broadcast.
       */

      if(grr_data->uplink_tbf.mac_mode EQ FIXED_ALLOCATION)
      {
        tc_abort_tbf(CGRLC_TBF_MODE_UL); /* abort UL TBF*/

      }
      else if(grr_t_status( T3172_1 ) EQ 0 )
      {
        ;
      }

      /*lint -fallthrough*/

    case CGRLC_TBF_MODE_DL:
      if(grr_t_status( T3172_1 ) EQ 0 )
      {
        TRACE_EVENT("CGRLC_TBF_MODE_DL, T3172_1 is not running");
        /*
         * primtive is delted if prim_status is set, status indication is sent to higher layers
         */
        tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
        switch( tc_eval_access_rej(&t3172_value) )
        {
          case E_ACCESS_REJ_WAIT:
            tc_start_timer_t3172( t3172_value );
            sig_tc_ctrl_t3172_running();
            break;
          case E_ACCESS_REJ_NO_WAIT:
            TRACE_EVENT("NO WAIT_INDICATION: nothing to do");
            break;
          default:
            break;
        }
      } /* no else because additional rejects have to be ignored!! */
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case CGRLC_TBF_MODE_2PA:
      /* PRR is sent by GRLC. NW responds with PAR */
      if(grr_data->tc.tbf_est_pacch)
      {
    
        if(grr_t_status( T3170) EQ 0 )
           vsi_t_start(GRR_handle,T3170,tc_get_t3170_value());

        switch( tc_eval_access_rej(&t3172_value) )
        {
          case E_ACCESS_REJ_WAIT:
            tc_start_timer_t3172( t3172_value );
            sig_tc_ctrl_t3172_running();
            break;
          case E_ACCESS_REJ_NO_WAIT:
            TRACE_EVENT("NO WAIT_INDICATION: nothing to do");
            break;
          default:
            break;
        }
      }
      break;
#endif
    default:
      break;
  }/* switch tbf_type*/
  
} /* sig_gfp_tc_access_rej_ptm() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_ts_reconfig_ptm
+------------------------------------------------------------------------------
| Description : Handles the 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_ts_reconfig_ptm ( void )
{ 

  TRACE_ISIG( "sig_gfp_tc_ts_reconfig_ptm" );
  /*
   * ASUMPTION:
   * This primitive (re)assigns an uplink AND downlink assignment.
   * if not the primitive is ignored.
   * CASES:
   * 1. ONLY UL_TBF is running:  reassignment of uplink AND assignment of downlink
   * 2. ONLY DL_TBF is running:  reassignment of downlink AND assignment of uplink
   * 3. DL_TBF AND UL_TBF is running:  reassignment of downlink AND reassignment of uplink
   */    

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
      TRACE_ERROR("TS RECONF in CONT: RES. ignored");
      break; /*ignore during contention resolution procedure*/
    case TC_TBF_ACTIVE:
      switch(tc_eval_ts_reconf())
      {
        /*handle tbf starting time for uplink*/
        case E_TS_UL_REASSIG_NEW_DL:
        case E_TS_NEW_UL_DL_REASSIG:
        case E_TS_UL_REASSIG_DL_REASSIG:
          /*
          TRACE_EVENT("Packet TS Reconfigure ");
          */
          tc_handle_tbf_start( CGRLC_TBF_MODE_DL_UL );
          break;
        case E_TS_IGNORE:
          /*no effect on current tbfs*/
          break;
        case E_TS_RECONFIG_ERROR_RA:
          TRACE_EVENT("Packet TS Reconfigure invalid");
          vsi_t_stop(GRR_handle,T3168); 
          tc_handle_error_ra();
          break;
        default:
          TRACE_ERROR( "SIG_GFP_TC_TS_RECONFIG_PTM unexpected" );
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_TS_RECONFIG_PTM unexpected" );
      break;
  }

} /* sig_gfp_tc_ts_reconfig_ptm() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_ul_assign_ptm
+------------------------------------------------------------------------------
| Description : Handles the 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_ul_assign_ptm  (void )
{ 
  TRACE_ISIG( "sig_gfp_tc_ul_assign_ptm " );


  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:      
      switch( tc_eval_ul_assign() )
      {
        case E_UL_ASSIGN_DYNAMIC:
          /*
          TRACE_EVENT("Packet UL Assignment with dynamic allocation");
          */
          tc_handle_tbf_start( CGRLC_TBF_MODE_UL );
          break;
        case E_UL_ASSIGN_FIXED:
          /*
          TRACE_EVENT("Packet UL Assignment with fixed allocation");
          */
          tc_handle_tbf_start( CGRLC_TBF_MODE_UL );
          break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        case  E_UL_ASSIGN_SB_2PHASE_ACCESS:
          if((grr_data->tc.tbf_est_pacch) AND 
             (!psc_db->gprs_cell_opt.ctrl_ack_type))
          {
            /* Control ack type is Access burst. GRLC would have sent 
             * PCA as poll for initiating TBF est on PACCH. Send UL_TBF_RES
             * to GRLC. Packet resource request message will be sent by GRLC.			 
             */
            TRACE_EVENT("TBF on pacch");
            SET_STATE(TC,TC_WAIT_2P_ASSIGN);
            tc_handle_tbf_start( CGRLC_TBF_MODE_2PA);
          }
          break;
#endif


 

        default:
          TRACE_EVENT("Packet UL Assignment invalid");
          tc_handle_error_ra();
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_UL_ASSIGN_PTM  unexpected" );
      break;
  }

} /* sig_gfp_tc_ul_assign_ptm () */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_dl_assign_ptm 
+------------------------------------------------------------------------------
| Description : Handles the 
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_dl_assign_ptm ( void )
{ 
  TRACE_ISIG( "sig_gfp_tc_dl_assign_ptm" );

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
      TRACE_ERROR("P. DL. ASS. in CONT: RES. ignored");
      break; /*ignore during contention resolution procedure*/
    case TC_TBF_ACTIVE:
      switch( tc_eval_dl_assign() )
      {
        case E_DL_ASSIGN:
          /*
          TRACE_EVENT("Packet DL Assignment");
          */
          tc_handle_tbf_start( CGRLC_TBF_MODE_DL );
          break;
        case E_DL_ASSIGN_ERROR_RA:
          TRACE_EVENT("Packet DL Assignment invalid");
          tc_handle_error_ra();
          break;
        case E_DL_ASSIGN_IGNORE:
          /*
           *  If a failure in the PACKET DOWNLINK ASSIGNMENT is due to any reason, 
           *  the mobile station shall abort the procedure and continue the normal 
           *  operation of the uplink TBF.
           */
          break;
        case E_DL_ASSIGN_ABORT_DL:
          tc_abort_tbf(CGRLC_TBF_MODE_DL);
          break;
        default:
          TRACE_ERROR( "Packet DL Assignment with unexpected content" );
          break;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_DL_ASSIGN_PTM unexpected" );
      break;
  }
} /* sig_gfp_tc_dl_assign_ptm() */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_pdch_release_ptm ( tn );
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : tn_i
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_pdch_release_ptm ( UBYTE tn)
{ 
  TRACE_ISIG( "sig_gfp_tc_pdch_release_ptm" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      switch( tc_eval_pdch_rel(tn) )
      {
        case E_PDCH_REL_RECONF:
          tc_send_pdch_rel(grr_data->pdch_rel_ts_mask);
          break;
        case E_PDCH_REL_RELEASE_DL_RECONF_UL:
          tc_abort_tbf(CGRLC_TBF_MODE_DL);
          tc_send_pdch_rel(grr_data->pdch_rel_ts_mask);
          break;
        case E_PDCH_REL_RELEASE_UL_RECONF_DL:
          tc_abort_tbf(CGRLC_TBF_MODE_UL);
          tc_send_pdch_rel(grr_data->pdch_rel_ts_mask);
          break;
        case E_PDCH_REL_RELEASE_BOTH:
          tc_abort_tbf(CGRLC_TBF_MODE_DL_UL);
          break;
        case E_PDCH_REL_RELEASE_UL:
          tc_abort_tbf(CGRLC_TBF_MODE_UL);
          break;
        case E_PDCH_REL_RELEASE_DL:
          tc_abort_tbf(CGRLC_TBF_MODE_DL);
          break;
        default:
          break;
      } 
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_PDCH_RELEASE_PTM unexpected" );
      break;
  }

  /*
   *  in all states
   */ 
  if(grr_data->tbf_type EQ CGRLC_TBF_MODE_NULL)
  {
    /* new access is done when release confirm arrives */
    SET_STATE(TC,TC_PIM);
  }
  else if(grr_data->tbf_type EQ CGRLC_TBF_MODE_DL)
  {
    SET_STATE(TC,TC_TBF_ACTIVE);
  }


} /* sig_gfp_tc_pdch_release_ptm() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_queuing_not 
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_queuing_not  ( void )
{ 
  TRACE_ISIG( "sig_gfp_tc_queuing_not " );
  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_ASSIGN:

      SET_STATE(TC,TC_POLLING);
      /* 
       * stop sending packet access request 
       */ 
      tc_send_ra_stop_req();
      
      vsi_t_stop(GRR_handle,T3170);         
      vsi_t_start(GRR_handle,T3162,T3162_VALUE);      

      /*
       * save tqi
       */
      {
        MCAST(d_queuing_not,D_QUEUING_NOT);
        grr_data->tqi = d_queuing_not->tqi;
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_QUEUING_NOT  unexpected" );
      break;
  }

} /* sig_gfp_tc_queuing_not () */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_ul_assign
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_ul_assign ( void )
{ 
  UBYTE  old_state = GET_STATE( TC );

  TRACE_ISIG( "sig_gfp_tc_ul_assign" );
  
  if( grr_t_status( T3172_1 ) > 0 ) 
  {
    if( grr_t_status( T3170 ) > 0 )
    {
      /*
       * stop T3172 and set some parameters
       */
      tc_stop_timer_t3172();
    }
    else if ( grr_t_status( T3176 ) EQ 0)
    {
      TRACE_EVENT("Ignore UL Assignment");
      return;
    }
  }
  
  switch( old_state )
  {
    case TC_WAIT_2P_ASSIGN:
      grr_data->uplink_tbf.ti = 0; 
      /*  mark that contention resulution is not yet done    */

      /*lint -fallthrough*/

    case TC_WAIT_STOP_TASK_CNF:       
      if (grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_UL)
      {
        TRACE_EVENT("Reassignment for UL ASS in CNF state");
      }
      else if(grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_DL)
      {
        TRACE_EVENT("Ignore UL ASS, wait for DL start");
        break;
      }
      else if(grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_NULL AND grr_data->uplink_tbf.ti)
      {
        TRACE_EVENT("Ignore UL ASS, wait for condirn to start RA procedure  !!!!");
        break;
      }

      /*lint -fallthrough*/

      /*
       * else :TWO PHASE ACCESS IS RUNNING
       */
    case TC_POLLING:
    case TC_WAIT_ASSIGN:
     /* 
      * stop sending packet access request if in progress
      */ 
      tc_send_ra_stop_req();

      /*lint -fallthrough*/

    case TC_SINGLE_BLOCK:
    case TC_CONTENTION_RESOLUTION:
      switch( tc_eval_ul_assign() )
      {
        case E_UL_ASSIGN_SB_2PHASE_ACCESS:
          SET_STATE(TC,TC_WAIT_2P_ASSIGN); 
          /* 
           * send MPHP_SINGLE_BLOCK_REQ with PACKET RESOURCE REQUEST
           */
          tc_send_resource_request_p();
          vsi_t_stop(GRR_handle,T3170);
          break;

        case E_UL_ASSIGN_DYNAMIC:
          /*
          TRACE_EVENT("Packet UL Assignment with dynamic allocation");
          */
          if(grr_data->uplink_tbf.ti)
          {
            SET_STATE(TC,TC_CONTENTION_RESOLUTION);
          }
          else
          {
            SET_STATE(TC,TC_TBF_ACTIVE);
            sig_tc_ctrl_contention_ok();
            grr_data->tc.two_2p_w_4_tbf_con = FALSE;
          }
          tc_prepare_handle_tbf_start( CGRLC_TBF_MODE_UL, GET_STATE( TC ), old_state );
          break;
        case E_UL_ASSIGN_FIXED:
          /*TRACE_EVENT("Packet UL Assignment with fixed allocation");*/
          if(grr_data->uplink_tbf.ti)
          {
            SET_STATE(TC,TC_CONTENTION_RESOLUTION);
          }
          else
          {
            SET_STATE(TC,TC_TBF_ACTIVE);
            sig_tc_ctrl_contention_ok();
          }
          tc_prepare_handle_tbf_start( CGRLC_TBF_MODE_UL, GET_STATE( TC ), old_state );
          break;
        case E_UL_ASSIGN_SB_WITHOUT_TBF: 
          TRACE_EVENT("Packet UL Assignment with single block allocation");
          /* send MPHP_SINGLE_BLOCK_REQ with DATA      
           * (measurement report)                              */
          tc_send_single_block();
          SET_STATE(TC,TC_SINGLE_BLOCK);
          vsi_t_stop(GRR_handle,T3170);
          break;
        default:
          vsi_t_stop(GRR_handle,T3170);
          if(grr_data->tc.state EQ TC_WAIT_2P_ASSIGN OR
             grr_data->tc.state EQ TC_SINGLE_BLOCK)
          {
            vsi_t_stop(GRR_handle,T3168);
            TRACE_EVENT_P1("Packet UL Assignment invalid during 2 phase access state=%d",grr_data->tc.state);            
            tc_stop_normal_burst_req( );
          }
          else
          {
            TRACE_EVENT("Packet UL Assignment invalid");            
            tc_handle_error_ra();
          }
          break;
      }
      break;
    default:
      vsi_t_stop(GRR_handle,T3170);
      TRACE_ERROR( "SIG_GFP_TC_UL_ASSIGN unexpected" );
      break;
  }

} /* sig_gfp_tc_ul_assign() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_dl_assign  
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : -
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_dl_assign  (void)
{ 
  UBYTE old_state = GET_STATE( TC );

  TRACE_ISIG( "sig_gfp_tc_dl_assign  " );

  
  switch( old_state  )
  {        
    case TC_WAIT_STOP_TASK_CNF:
      TRACE_EVENT_P4("ASS IN CNF state:ccch_read=%d  tbf_Type =%d state=%d p_assign=%ld" 
                       ,grr_data->tc.last_eval_assign.ccch_read
                       ,grr_data->tc.last_eval_assign.tbf_type
                       ,grr_data->tc.last_eval_assign.state
                       ,grr_data->tc.p_assign);
      if (grr_data->tc.last_eval_assign.tbf_type EQ CGRLC_TBF_MODE_UL)
      {
        TRACE_EVENT("Ignore DL ASS, wait for UL start");
        break;
      }

      /*lint -fallthrough*/

    case TC_WAIT_ASSIGN:
     /* 
      * stop sending packet access request if in progress
      */ 
      tc_send_ra_stop_req();

      /*lint -fallthrough*/

    case TC_PIM:
    case TC_POLLING:
    case TC_SINGLE_BLOCK:
    case TC_TBF_ACTIVE:
      switch( tc_eval_dl_assign() )
      {
        case E_DL_ASSIGN:
          /*
          TRACE_EVENT("Packet DL Assignment");
          */
          SET_STATE(TC,TC_TBF_ACTIVE);
          if( grr_t_status( T3162 ) NEQ 0 )
          {
            vsi_t_stop(GRR_handle,T3162);/*ETSI 4.60 7.1.2.2.2.*/
          }
          tc_prepare_handle_tbf_start( CGRLC_TBF_MODE_DL, GET_STATE( TC ), old_state );
          break;
        case E_DL_ASSIGN_ERROR_RA:
          TRACE_EVENT("Packet DL Assignment invalid");
          vsi_t_stop(GRR_handle,T3170);

          if(grr_data->tc.state EQ TC_SINGLE_BLOCK)
          {
            TRACE_EVENT_P1("Packet DL Assignment invalid during IA_DL_SB state=%d",grr_data->tc.state);            
            tc_stop_normal_burst_req( );
          }
          else
          {
            tc_handle_error_ra();
          }
          break;
        case E_DL_ASSIGN_IGNORE:
          /*
           *  If a failure in the PACKET DOWNLINK ASSIGNMENT is due to any reason, 
           *  the mobile station shall abort the procedure and continue the normal 
           *  operation of the uplink TBF.
           */
          vsi_t_stop(GRR_handle,T3170);
          break;
        default:
          vsi_t_stop(GRR_handle,T3170);
          TRACE_ERROR( "DL_ASSIGNMENT with unexpected content" );
          break;
      }
      break;
    default:
      vsi_t_stop(GRR_handle,T3170);
      TRACE_ERROR( "SIG_GFP_TC_DL_ASSIGN    unexpected" );
      break;
  }

} /* sig_gfp_tc_dl_assign  () */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_access_rej 
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_access_rej  ( void )
{ 
  ULONG t3172_value=0;
  BOOL start_t3172 = TRUE;

  TRACE_ISIG( "sig_gfp_tc_access_rej " );

  if(grr_t_status( T3172_1 ) > 0)
  {
    TRACE_EVENT("T3172 running: Ignore PACKET ACCESS REJECT");
  }
  else
  {
    /*  The T3168 could running.  */
    vsi_t_stop(GRR_handle,T3168); 

    switch( GET_STATE( TC ) )
    {
      case TC_WAIT_ASSIGN:
      case TC_POLLING:
      case TC_WAIT_2P_ASSIGN:
      case TC_SINGLE_BLOCK:
        tc_send_ra_stop_req(); 
        /*
         *  stop Packet Access Request sending (T3186 will be stopped if runnig)
         */
        if( grr_t_status( T3174 ) > 0 )
        {
          TRACE_ERROR( " Access reject on the network commanded cell" );
          grr_data->pcco_failure_cause   = 2;
          sig_tc_ctrl_access_reject_on_new_cell();
          /* Do not start the 3172 timer if on the new cell access reject is received */
          start_t3172 = FALSE;
        }
        else
        if(grr_t_status( T3170 ) EQ 0 )
        {   /* T3170 is not running */
          vsi_t_start(GRR_handle,T3170,tc_get_t3170_value());   
          TRACE_EVENT("sig_gfp_tc_access_rej: t3170 started");
        }
        switch( tc_eval_access_rej( &t3172_value ) )
        {
          case E_ACCESS_REJ_WAIT:
            if (start_t3172 EQ TRUE)
            {
              tc_start_timer_t3172( t3172_value );
              sig_tc_ctrl_t3172_running();
            }
            break;
          case E_ACCESS_REJ_NO_WAIT:
            TRACE_EVENT("No value for T3172, go to idle mode after T3170 expiration");
            break;
          default:
            break;
        }
        break;
      default:
        TRACE_ERROR( "SIG_GFP_TC_ACCESS_REJ  unexpected" );
        break;
    }
  }/* ignore reject*/

} /* sig_gfp_tc_access_rej () */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_ra_con
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : payload to primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_ra_con ( T_MPHP_RA_CON * mphp_ra_con_i )
{ 
  TRACE_ISIG( "sig_gfp_tc_ra_con" ); /*MODIF*/

  /*
   *  save last packet access request reference
   *  see also 0408d640, 10.5.2.28
   */

  grr_data->req_ref[grr_data->tc.n_acc_req%3].fn_mod.t1 = 
                           (UBYTE)( ( mphp_ra_con_i->fn / 1326) % 32 );
  grr_data->req_ref[grr_data->tc.n_acc_req%3].fn_mod.t2 = 
                           (UBYTE)( mphp_ra_con_i->fn % 26 ); 
  grr_data->req_ref[grr_data->tc.n_acc_req%3].fn_mod.t3 = 
                           (UBYTE)( mphp_ra_con_i->fn % 51 );


  /*
   * 0 8 bit access burst
   * 1 11 bit access burst
   */
  if(psc_db->gprs_cell_opt.ab_type)
  {
    /*
     *  11 Bit access burst
     * b: bit
     * b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0
     * should be sent to the network -according 04.60 and 0404- in the
     * following 16-bit format:
     * 0 0 0 0 0 b2 b1 b0 b10 b9 b8 b7 b6 b5 b4 b3
     *
     * We have to store the access info in the 
     * b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0 format
     */
    USHORT acc_inf, dummy = 0;
    dummy = mphp_ra_con_i->channel_request_data;
    dummy = dummy >> 8; /* 0 0 0 0 0 0 0 0 0 0 0 0 0 b2 b1 b0*/
    acc_inf = mphp_ra_con_i->channel_request_data;
    acc_inf = (acc_inf & 0xFF);/* 0 0 0 0 0 0 0 0 b10 b9 b8 b7 b6 b5 b4 b3 */
    acc_inf = acc_inf << 3; /* 0 0 0 0 0 b10 b9 b8 b7 b6 b5 b4 b3 0 0 0 */
    acc_inf = acc_inf | dummy;/* 0 0 0 0 0 b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0 */
    grr_data->req_ref[grr_data->tc.n_acc_req%3].access_info = acc_inf;
  }
  else
  {
    grr_data->req_ref[grr_data->tc.n_acc_req%3].access_info = mphp_ra_con_i->channel_request_data;
  }

  grr_data->tc.n_acc_req++;

  grr_data->tc.ra_once_sent++;

  if (grr_data->tc.ra_once_sent >= 2)
  {
    grr_data->r_bit = 1;
  }

  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_ASSIGN:
      switch( tc_handle_ra_con() )
      {
      case H_RA_CON_STOP_REQ:
        tc_send_ra_stop_req();
        break;
      case H_RA_CON_CONTINUE:
        tc_send_ra_req();
        break;
      default:
        break;
      } 
      break;
    default:
      tc_send_ra_stop_req();
      break;
  }
} /* sig_gfp_tc_ra_con() */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_ra_stop_con 
+------------------------------------------------------------------------------
| Description : Handles the primitive T_MPHP_RA_STOP_CON 
|
| Parameters  : Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_ra_stop_con ( T_MPHP_RA_STOP_CON * mphp_ra_stop_con_i )
{ 
  TRACE_ISIG( "sig_gfp_tc_ra_stop_con" );
   
  switch( GET_STATE( TC ) )
  {
    case TC_ACCESS_DISABLED:
      tc_call_disable_callback_func( );
      break;
    default:
      /* nothing to do */
      break;
  }
} /* sig_gfp_tc_ra_stop_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_single_block_cnf
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : *mphp_single_block_con - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_single_block_cnf ( T_MPHP_SINGLE_BLOCK_CON *mphp_single_block_con)
{ 
  TRACE_ISIG( "sig_gfp_tc_single_block_cnf" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_2P_ASSIGN:
      if(!mphp_single_block_con->sb_status)
      {
        /*
         * TRACE_EVENT_P1("T3168 start: %d ms",((psc_db->gprs_cell_opt.t3168)+1)*500);
         */
        vsi_t_start(GRR_handle,T3168,(psc_db->gprs_cell_opt.t3168+1)*500); 
        sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ACCESS_TWO_PHASE, TASK_STOP_DUMMY_VALUE );
      }
      else
      {
        vsi_t_stop(GRR_handle,T3168);
        TRACE_ERROR("Single Block not sent by layer 1");
        TRACE_EVENT_P1("sb_status = %d",mphp_single_block_con->sb_status);
        sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ACCESS_TWO_PHASE, TASK_STOP_DUMMY_VALUE );
        tc_stop_normal_burst_req( );
      }
      break;
    case TC_SINGLE_BLOCK:
      if(mphp_single_block_con->purpose EQ SINGLE_BLOCK_TRANSFER_UL)   
      {
        BOOL is_tx_success = mphp_single_block_con->sb_status EQ SB_STAT_OK;       
        tc_set_stop_ctrl_blk ( is_tx_success,CGRLC_BLK_OWNER_NONE, 0 ); 
        sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_MEAS_REP_IDLE, TASK_STOP_DUMMY_VALUE );
      }
      else
      {
        sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_ACCESS_TWO_PHASE, TASK_STOP_DUMMY_VALUE );
      }
      tc_stop_normal_burst_req( );
      break;
    case TC_ACCESS_DISABLED:
      /* 
       * After receiving a Packet Cell Change Order the signal SIG_GFP_SINGLE_BLOCK_CNF
       * was unexpected. MPHP_STOP_SINGLE_BLOCK_REQ is sent during disableing of the
       * GRR services.
       */
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_SINGLE_BLOCK_CNF unexpected" );
      break;
  }
} /* sig_gfp_tc_single_block_cnf () */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_assignment_con 
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_assignment_con  ( void )
{ 
  TRACE_ISIG( "sig_gfp_tc_assignment_con " );

  grr_data->tc.two_2p_w_4_tbf_con = TRUE;
  
  if(grr_data->tc.state NEQ TC_ACCESS_DISABLED)
  {
    if(grr_is_pbcch_present( ))
    {
      grr_data->tc.last_eval_assign.ccch_read = 0xFF;  
      grr_data->tc.last_eval_assign.tbf_type  = CGRLC_TBF_MODE_NULL; 
      grr_data->tc.last_eval_assign.state     = 0xFF;
    }
    sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_TRANSFER, TASK_STOP_DUMMY_VALUE );
  }
  else
  {
    TRACE_EVENT_P1("TBF_C in TC_ACCESS_DISABLED: do not sent sig_tc_ctrl_set_pckt_mode num_rel=%d ",grr_data->tc.num_of_rels_running );
  }


} /* sig_gfp_tc_assignment_con () */


/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_tbf_release_con
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : T_MPHP_TBF_RELEASE_CON * mphp_tbf_release_con_i  
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_tbf_release_con (  T_MPHP_TBF_RELEASE_CON * mphp_tbf_release_con_i  )
{ 
  T_RELEASE_STATE rel_state= REL_RUNNING;
  TRACE_ISIG( "sig_gfp_tc_tbf_release_con" );

  if(mphp_tbf_release_con_i->tbf_type EQ UL_DL_ASSIGNMENT)
    grr_data->tc.num_of_rels_running = 0;
  else if (grr_data->tc.num_of_rels_running)
    grr_data->tc.num_of_rels_running--;
  else
  {
    TRACE_EVENT_P2("REL CON UNEXPECTED num=%d  tbf_rel_type=%d "
                                    ,grr_data->tc.num_of_rels_running
                                    ,mphp_tbf_release_con_i->tbf_type);
  }
  
  if(!grr_data->tc.num_of_rels_running)
  {
    rel_state = REL_PERFORMED;
    if(grr_is_pbcch_present( ))
    {
      grr_data->tc.last_eval_assign.ccch_read = 0xFF;  
      grr_data->tc.last_eval_assign.tbf_type  = CGRLC_TBF_MODE_NULL; 
      grr_data->tc.last_eval_assign.state     = 0xFF;
    }
  }

  sig_tc_ctrl_rel_state(rel_state);

  switch( GET_STATE( TC ) )
  {
    case TC_PIM:
      grr_data->ta_params.ta_value = 0xFF;
      grr_data->ta_params.ta_valid = FALSE;
      memset(&grr_data->ta_params, 0xFF, sizeof(T_TA_PARAMS));

      if(0 EQ grr_data->tc.num_of_rels_running)
      {
        grr_data->tc.v_freq_set = FALSE;

        if( grr_data->tc.disable_callback_func EQ NULL )
        {
          /*enable access for grlc*/
          tc_cgrlc_tbf_rel_res( mphp_tbf_release_con_i->tbf_type ); /* release of all tbfs, grr has entered PIM, transition rule is taken into acocount*/
        }
        else
        {
          SET_STATE( TC, TC_ACCESS_DISABLED );

          tc_cgrlc_disable_req ( CGRLC_PRIM_STATUS_NULL );
          tc_call_disable_callback_func( );
        }
      }/* else: wait for last release confirm */
      else
      {
        tc_cgrlc_tbf_rel_res( mphp_tbf_release_con_i->tbf_type );

        TRACE_EVENT_P1("WAIT FOR %d TBF RELEASE CON ",grr_data->tc.num_of_rels_running);
      }
      break;
    case TC_ACCESS_DISABLED:      
      grr_data->ta_params.ta_value    = 0xFF;
      grr_data->ta_params.ta_valid    = FALSE;
      memset(&grr_data->ta_params, 0xFF, sizeof(T_TA_PARAMS));
      grr_data->tc.v_freq_set         = FALSE;
      grr_data->tc.two_2p_w_4_tbf_con = TRUE;

      tc_cgrlc_disable_req ( CGRLC_PRIM_STATUS_NULL );

      if(0 EQ grr_data->tc.num_of_rels_running)
      {
        tc_call_disable_callback_func( );
      }
      else
      {
        TRACE_EVENT_P1("REL_C in disable state: nr_rel=%d",grr_data->tc.num_of_rels_running);
      }
      break;
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      /*
       * update timimg advance
       */
      switch(mphp_tbf_release_con_i->tbf_type )
      {
        case DL_ASSIGNMENT:
          { 
            tc_cgrlc_tbf_rel_res( mphp_tbf_release_con_i->tbf_type );
          
            /* 
             * downlink tbf released, set ul ta, if it is different 
             */
            if(!((grr_data->ta_params.l1_ta_value EQ grr_data->ta_params.ta_value) AND
                 (grr_data->ta_params.l1_ta_i     EQ grr_data->ta_params.ul_ta_i)  AND
                 (grr_data->ta_params.l1_ta_tn    EQ grr_data->ta_params.ul_ta_tn)))
            {
              PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */
              mphp_timing_advance_req->assign_id = 0;
              mphp_timing_advance_req->p_timing_advance.ta_value = grr_data->ta_params.ta_value;
              mphp_timing_advance_req->p_timing_advance.ta_index = grr_data->ta_params.ul_ta_i;
              mphp_timing_advance_req->p_timing_advance.tn       = grr_data->ta_params.ul_ta_tn;
              PSEND(hCommL1,mphp_timing_advance_req);
              TRACE_EVENT_P5("DL TBF_REL TAU: new: %d %d %d-- old %d %d"
                                                            ,grr_data->ta_params.ta_value
                                                            ,grr_data->ta_params.ul_ta_i
                                                            ,grr_data->ta_params.ul_ta_tn
                                                            ,grr_data->ta_params.dl_ta_i
                                                            ,grr_data->ta_params.dl_ta_tn);
              grr_data->ta_params.dl_ta_i  = 0xFF;
              grr_data->ta_params.dl_ta_tn = 0xFF;
            }
          }
          break;
        case UL_ASSIGNMENT:
          { 
            tc_cgrlc_tbf_rel_res( mphp_tbf_release_con_i->tbf_type );
          
            /* 
             * uplink tbf released, set dl ta, if it is different
             */
            if(!((grr_data->ta_params.l1_ta_value EQ grr_data->ta_params.ta_value) AND
                 (grr_data->ta_params.l1_ta_i     EQ grr_data->ta_params.dl_ta_i)  AND
                 (grr_data->ta_params.l1_ta_tn    EQ grr_data->ta_params.dl_ta_tn)))
            {
              PALLOC(mphp_timing_advance_req,MPHP_TIMING_ADVANCE_REQ);/* T_MPHP_TIMING_ADVANCE_REQ */
              mphp_timing_advance_req->assign_id = 0;
              mphp_timing_advance_req->p_timing_advance.ta_value = grr_data->ta_params.ta_value;
              mphp_timing_advance_req->p_timing_advance.ta_index = grr_data->ta_params.dl_ta_i;
              mphp_timing_advance_req->p_timing_advance.tn       = grr_data->ta_params.dl_ta_tn;
              PSEND(hCommL1,mphp_timing_advance_req);
              TRACE_EVENT_P5("UL TBF_REL TAU: new: %d %d %d-- old %d %d"
                                                            ,grr_data->ta_params.ta_value
                                                            ,grr_data->ta_params.dl_ta_i
                                                            ,grr_data->ta_params.dl_ta_tn
                                                            ,grr_data->ta_params.ul_ta_i
                                                            ,grr_data->ta_params.ul_ta_tn);

              grr_data->ta_params.ul_ta_i  = 0xFF;
              grr_data->ta_params.ul_ta_tn = 0xFF;
            }
          }
          break;
        default:
          TRACE_ERROR( "TBF RELEASED, stay in tbf mode, concurrent tbf or no tbf ????" );
      break;
      }
      break;
    case TC_CPAP:
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
    case TC_WAIT_2P_ASSIGN:
      /* in these cases the prim shall not occur */
    default:
      TRACE_ERROR( "SIG_GFP_TC_TBF_RELEASE_CON unexpected" );
      break;
  }
   
} /* sig_gfp_tc_tbf_release_con() */

/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_stop_single_block_con
+------------------------------------------------------------------------------
| Description : Handles 
|
| Parameters  : T_MPHP_STOP_SINGLE_BLOCK_CON * mphp_stop_single_block_con_i
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_stop_single_block_con (T_MPHP_STOP_SINGLE_BLOCK_CON * mphp_stop_single_block_con_i )
{ 
  TRACE_ISIG( "sig_gfp_tc_stop_single_block_con" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_PIM:
      TRACE_EVENT("MPHP_STOP_SINGLE_BLOCK_CON in TC_PIM: nothing to do");
      tc_cgrlc_access_status_req();
      break;
    case TC_WAIT_2P_ASSIGN:
      if(grr_t_status( T3172_1 ) > 0)
      {        
        tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
        sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
      }
      else
      {
        /*
         * two phase access has failed
         */
        SET_STATE(TC,TC_PIM);
        tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_NULL);
      }
      break;
    case TC_ACCESS_DISABLED:      
      if(!grr_data->tc.two_2p_w_4_tbf_con)
      {
        TRACE_EVENT("BLC_C abort TBF");
        tc_abort_tbf( grr_data->tbf_type );       
      }
      else
      {
        tc_call_disable_callback_func( );
      }

      break;
    case TC_SINGLE_BLOCK:
      /*
       * single block procedure for uplink ctrl message or receiving DL block stopped
       */
       /*
        * Stay in TC_SINGLE_BLOCK state while access reject is running.
        * Go to TC_PIM after T3170 is expired
        */
      if( grr_t_status( T3170 ) EQ 0 )
      {
        SET_STATE(TC,TC_PIM);
        tc_cgrlc_access_status_req();
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_STOP_SINGLE_BLOCK_CON unexpected" );
      break;
  }
} /* sig_gfp_tc_stop_single_block_con () */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_packet_tbf_rel_ptm 
+------------------------------------------------------------------------------
| Description : Handles the signal sig_gfp_tc_packet_tbf_rel_ptm
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_packet_tbf_rel_ptm (ULONG fn, UBYTE rrbp, UBYTE sp  )
{
  MCAST(d_tbf_release,D_TBF_RELEASE);

  TRACE_ISIG( "sig_gfp_tc_packet_tbf_rel_ptm" );
   
  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:


      if( !d_tbf_release->dl_release AND  
          !d_tbf_release->ul_release) 
      {
        TRACE_ERROR("no release request in packet tbf release");
        return;
      }
      else
      {
        T_TBF_TYPE tbf_type = CGRLC_TBF_MODE_NULL;
        UBYTE rel_cause     = CGRLC_TBF_REL_ABNORMAL;
        ULONG rel_fn        = CGRLC_STARTING_TIME_NOT_PRESENT;


        if( d_tbf_release->dl_release AND 
            d_tbf_release->ul_release )
        {
          if (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL)
          {
            tbf_type = CGRLC_TBF_MODE_DL_UL;
          }
          else if (grr_data->tbf_type EQ CGRLC_TBF_MODE_UL)
          {
            tbf_type = CGRLC_TBF_MODE_UL;
          }
          else if (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL)
          {
            tbf_type = CGRLC_TBF_MODE_DL;
          }    
        }

        else 
        if (  d_tbf_release->dl_release                  AND 
           ((grr_data->tbf_type EQ CGRLC_TBF_MODE_DL)    OR 
            (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL)))
        {
          tbf_type = CGRLC_TBF_MODE_DL;
        }

        else 
        if (  d_tbf_release->ul_release                  AND 
           ((grr_data->tbf_type EQ CGRLC_TBF_MODE_UL)    OR
            (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL)))
        {
          tbf_type = CGRLC_TBF_MODE_UL;
        }


        if( sp )
        {
          rel_fn = grr_calc_new_poll_pos(fn,rrbp);
        }
           
        if( d_tbf_release->rel_cause EQ NORMAL_RELEASE)
        {
          if ( d_tbf_release->ul_release                AND 
             ((grr_data->tbf_type EQ CGRLC_TBF_MODE_UL)  OR
             (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL)))
              
          {
             rel_cause = CGRLC_TBF_REL_NORMAL; 
          }
        }

        tc_cgrlc_tbf_rel_req(tbf_type,rel_cause,rel_fn);
      }
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_PACKET_TBF_REL_PTM unexpected" );
      break;
  }

} /* sig_gfp_tc_packet_tbf_rel_ptm () */



/*
+------------------------------------------------------------------------------
| Function    : sig_gfp_tc_poll_ind 
+------------------------------------------------------------------------------
| Description : Handles the signal sig_gfp_tc_poll_ind
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_gfp_tc_poll_ind (ULONG fn, UBYTE rrbp, UBYTE poll_type,UBYTE pctrl_ack  )
{

  TRACE_ISIG( "sig_gfp_tc_poll_ind" );
   
  switch( GET_STATE( TC ) )
  {
    case TC_PIM:
    case TC_CPAP:
    case TC_POLLING:
    case TC_WAIT_ASSIGN:
    case TC_WAIT_2P_ASSIGN:
    case TC_SINGLE_BLOCK:
    case TC_WAIT_STOP_TASK_CNF:

      switch(grr_data->tbf_type)
      {
        case CGRLC_TBF_MODE_NULL: 
          tc_send_polling_res ( poll_type,fn,rrbp,pctrl_ack);
          break;
          default:
            TRACE_ERROR( "SIG_GFP_TC_POLL_IND unexpected tbf_type" );
            break;
      } 
      break;
    default:
      TRACE_ERROR( "SIG_GFP_TC_POLL_IND unexpected" );
      break;
  }

} /* sig_gfp_tc_poll_ind () */


/*
+------------------------------------------------------------------------------
| Function    : sig_ctrl_tc_delete_cs_msg
+------------------------------------------------------------------------------
| Description : This function deletes the control blocks of cs owner. This
|               function is called when MS is moving from PAM to PTM. In this 
|               scenario it has to delete PMRs if any.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_ctrl_tc_delete_cs_msg        ( void )
{
  UBYTE i= 0;

  TRACE_FUNCTION( "sig_ctrl_tc_delete_cs_msg" );

    
  while( i                               <   MAX_CTRL_BLK_NUM AND 
         grr_data->tc.ul_ctrl_blk.seq[i] NEQ MAX_CTRL_BLK_NUM     )
  {
    if( grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].state
                                                       EQ BLK_STATE_ALLOCATED )
    {
      /* remove control block */
      TRACE_EVENT_P2("Delete Control msg sequence:%d owner:%d",
          grr_data->tc.ul_ctrl_blk.seq[i],
          grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].owner);
      if (CGRLC_BLK_OWNER_CS EQ grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].owner)
      {
          tc_cancel_ctrl_blk(grr_data->tc.ul_ctrl_blk.blk[grr_data->tc.ul_ctrl_blk.seq[i]].owner);
      }
	  
    }
  }


}/* sig_ctrl_tc_delete_cs_msg*/ 
