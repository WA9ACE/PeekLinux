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
|             TC of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_TCP_C
#define GRR_TCP_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include <string.h>
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"    /* to get air message definitions */
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"      /* to get the grr global function definitions */
#include "grr_tcf.h"    /* to get the service TC functions definitions */
#include "grr_tcs.h"    /* to get the service TC functions definitions */
#include "grr_ctrls.h"  /* to get the service CTRL signal definitions */
#include "grr_cpaps.h"  /* to get the service CPAP signal definitions */
#include "grr_meass.h"

/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== LOCAL TYPES ==========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : tc_t3162
+------------------------------------------------------------------------------
| Description : Handles the primitive T3162
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_t3162 ( void )
{ 
  TRACE_FUNCTION( "tc_t3162" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
      SET_STATE(TC,TC_PIM);
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
      break;
    default:
      TRACE_ERROR( "T3162 unexpected" );
      break;
  }

} /* tc_t3162() */



/*
+------------------------------------------------------------------------------
| Function    : tc_t3168
+------------------------------------------------------------------------------
| Description : Handles the primitive T3168
|               This timer is involved in 3 Procedures
|               1) Two Phase Access Procedure
|               2) Uplink TBF Setup on existing Downlink TBF
|               3) Uplink Realloaction Procedure on existing Uplink TBF
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_t3168 ( void )
{ 
  TRACE_FUNCTION( "tc_t3168" );
  /* 
   * The expiry of this timer may occure in 1 case:
   * 
   * b) two phase access procedure
   *
   */
  
  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_2P_ASSIGN:
      /*
       *  case b)  stop for waiting assignment 
       */
      tc_stop_normal_burst_req( );
      if( grr_t_status( T3174 ) > 0 )
      {
        TRACE_ERROR( " Contention Failure on the network commanded cell: 3168 expired" );
        grr_data->pcco_failure_cause   = 1;
        sig_tc_ctrl_access_reject_on_new_cell();
      }
      break;

    default:
      TRACE_ERROR( "T3168 unexpected" );
      break;
  }

} /* tc_t3168() */



/*
+------------------------------------------------------------------------------
| Function    : tc_t3170
+------------------------------------------------------------------------------
| Description : Handles the primitive T3170
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_t3170 ( void )
{ 

  TRACE_FUNCTION( "tc_t3170" );

  if(grr_t_status( T3176 ) > 0 )/*in case pcco running, t3170 should be ignored.*/
  {
    TRACE_EVENT("t3170 expired when t3176 is running: ignore");
    return;
  }

  switch( GET_STATE( TC ) )
	{
    case TC_SINGLE_BLOCK:
      /* 
       * MPHP_STOP_SINGLE_BLOCK_REQ was sent at receiving the packet access reject.
       */
      SET_STATE(TC,TC_PIM);
      TRACE_EVENT("T3170 expired in TC_SINGLE_BLOCK");
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
      tc_set_stop_ctrl_blk( FALSE, CGRLC_BLK_OWNER_NONE, 0 ); 
      break;
    case TC_WAIT_2P_ASSIGN:

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      if(grr_data->tc.tbf_est_pacch)
      {
        TRACE_EVENT_P1("T3170 expired in TC_WAIT_2P_ASSIGN tbf_est_pacch = %d",grr_data->tc.tbf_est_pacch);
        tc_handle_error_ra();
      }
      else	  
#endif
      {
        /* 
         * GMMRR_STATUS_IND will send in tc_handle_error_ra after MPHP_STOP_SINGLE_BLOCK_CON
         */
        TRACE_EVENT("T3170 expired in TC_WAIT_2P_ASSIGN/");
        tc_stop_normal_burst_req( );
      }
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
      case TC_TBF_ACTIVE : 
      if(grr_data->tc.tbf_est_pacch)
      {
        TRACE_EVENT("T3170 expired in TC_TBF_ACTIVE tbf_est_pacch active");
        tc_handle_error_ra();
      }
      else
      {
        TRACE_ERROR("T3170 expired when TC:TC_TBF_ACTIVE");
      }      
      break;
#endif
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
      SET_STATE(TC,TC_PIM);
      TRACE_EVENT("T3170 expired in TC_WAIT_ASSIGN//TC_POLLING");
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
      tc_set_stop_ctrl_blk( FALSE, CGRLC_BLK_OWNER_NONE, 0 ); 
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
      break;
    default:
      TRACE_ERROR( "T3170 unexpected" );
      break; 
  }
} /* tc_t3170() */



/*
+------------------------------------------------------------------------------
| Function    : tc_t3172
+------------------------------------------------------------------------------
| Description : Handles the primitive T3172
|
| Parameters  : inctance_i - identifier of timer's inctance  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_t3172 ( UBYTE inctance_i )
{ 
  TRACE_EVENT( "T3172 expired" );
  
  switch( GET_STATE( TC ) )
  {
    case TC_PIM:
      tc_cgrlc_access_status_req();
      break;
    case TC_CPAP:
      /*
       * set the CPAP state to CPAP_IDLE
       */
      sig_tc_cpap_t3142();
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ALL);

      /*
       * Do we need to set the TC state to SET_STATE(TC, TC_PIM);??
       */
      SET_STATE(TC, TC_PIM);
      break;
    case TC_WAIT_ASSIGN:
    case TC_POLLING:
      SET_STATE(TC,TC_PIM);
      vsi_t_stop(GRR_handle,T3170);
      /*
       * to set the states in services
       */
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ALL);  
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
      break;
    case TC_WAIT_2P_ASSIGN:
      /* Donot send stop single block req as it would have got
       * sent on 3170 expiry */
      if(grr_t_status(T3170) > 0)
      {
        vsi_t_stop(GRR_handle,T3170);
        tc_stop_normal_burst_req();
      }
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ALL);
      sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
      SET_STATE(TC,TC_PIM);
      break;
    case TC_CONTENTION_RESOLUTION:
    case TC_ACCESS_DISABLED:
    case TC_TBF_ACTIVE:
      break;
    default:
      TRACE_ERROR( "T3172 unexpected" );
      break;
  }

} /* tc_t3172() */



/*
+------------------------------------------------------------------------------
| Function    : tc_t3186
+------------------------------------------------------------------------------
| Description : Handles the primitive T3186
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_t3186 ( void )
{ 
  TRACE_FUNCTION( "tc_t3186" );
  TRACE_EVENT("T3186 expired");
  
  
  switch( GET_STATE( TC ) )
  {
    case TC_WAIT_ASSIGN:
      SET_STATE(TC, TC_PIM);
      /* 
       * stop sending packet access request 
       */ 
      tc_send_ra_stop_req();
      tc_cgrlc_ul_tbf_res(CGRLC_TBF_MODE_ACCESS_FAILED,CGRLC_PRIM_STATUS_ONE);
      break;
    default:
      TRACE_ERROR( "T3186 unexpected" );
      break;
  }

} /* tc_t3186() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_tbf_rel_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_tbf_rel_ind
|
| Parameters  : cgrlc_tbf_rel_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_tbf_rel_ind ( T_CGRLC_TBF_REL_IND * cgrlc_tbf_rel_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_tbf_rel_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
      if(( !grr_is_pbcch_present())   AND 
         ( grr_data->tc.dcch_present) AND
           cgrlc_tbf_rel_ind->tbf_rel_cause EQ CGRLC_TBF_REL_CR_FAILED   AND
           cgrlc_tbf_rel_ind->tbf_mode  EQ CGRLC_TBF_MODE_UL )
      {
        SET_STATE(TC,TC_CPAP);
        sig_tc_cpap_con_res_fail();
        break;
      }

      /*lint -fallthrough*/

    case TC_TBF_ACTIVE: 
      if( grr_data->cc_running OR grr_t_status( T3174 ) > 0 AND
          cgrlc_tbf_rel_ind->tbf_rel_cause EQ CGRLC_TBF_REL_CR_FAILED   AND
          cgrlc_tbf_rel_ind->tbf_mode  EQ CGRLC_TBF_MODE_UL)
      {
        TRACE_ERROR( " Contention Resolution failure on new cell" );
        sig_tc_ctrl_contention_failure();
      }
      else if(   cgrlc_tbf_rel_ind->tbf_rel_cause EQ CGRLC_TBF_REL_WITH_CELL_RESELECT      AND 
               ( cgrlc_tbf_rel_ind->tbf_mode      EQ CGRLC_TBF_MODE_UL                OR
                 cgrlc_tbf_rel_ind->tbf_mode      EQ CGRLC_TBF_MODE_DL_UL                )     )
      {
        sig_tc_ctrl_prepare_abnorm_rel_with_cr( );
      }
      else
      {
        if( (grr_data->tbf_type EQ CGRLC_TBF_MODE_DL_UL OR 
             grr_data->tbf_type EQ cgrlc_tbf_rel_ind->tbf_mode) AND
            cgrlc_tbf_rel_ind->tbf_mode NEQ CGRLC_TBF_MODE_NULL)
        {
          if( (grr_data->downlink_tbf.trans_id NEQ cgrlc_tbf_rel_ind->dl_trans_id) AND
              cgrlc_tbf_rel_ind->tbf_mode EQ CGRLC_TBF_MODE_DL)
          {
            /* Dont perform REL_R as just configured L1 with TBF_R*/
            TRACE_EVENT_P2("GRR and GRLC DL Transaction Id Diff grr=%d grlc=%d",
            grr_data->downlink_tbf.trans_id,cgrlc_tbf_rel_ind->dl_trans_id);
          }
          else
          {
            tc_send_tbf_rel  (cgrlc_tbf_rel_ind->tbf_mode);
            tc_deactivate_tbf(cgrlc_tbf_rel_ind->tbf_mode);        
          }
        }
        else
        {
          TRACE_EVENT_P2("cgrlc_tbf_rel_ind  grr_data->tbf_type=%d,cgrlc_tbf_rel_ind->tbf_mode=%d"
                                                          ,grr_data->tbf_type
                                                          ,cgrlc_tbf_rel_ind->tbf_mode);
        }
      }           

      if( cgrlc_tbf_rel_ind->v_c_value EQ TRUE )
      {
         meas_c_set_c_value( &cgrlc_tbf_rel_ind->c_value );
      }
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case TC_WAIT_2P_ASSIGN : 
      /* T3168 timer expires in GRLC */
      if((grr_data->tc.tbf_est_pacch) AND 
         (cgrlc_tbf_rel_ind->tbf_mode  EQ CGRLC_TBF_MODE_2PA))
      {
        tc_send_tbf_rel  (cgrlc_tbf_rel_ind->tbf_mode);
        tc_deactivate_tbf(cgrlc_tbf_rel_ind->tbf_mode);
      }
      break;
#endif
    default:
      TRACE_ERROR( "cgrlc_tbf_rel_ind unexpected" );
      break;
  }

  PFREE(cgrlc_tbf_rel_ind);

} /* tc_cgrlc_tbf_rel_ind() */



/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_ul_tbf_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_ul_tbf_ind
|
| Parameters  : cgrlc_ul_tbf_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_ul_tbf_ind ( T_CGRLC_UL_TBF_IND * cgrlc_ul_tbf_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_ul_tbf_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_PIM:
      grr_data->r_bit          = 0;
      grr_data->tc.sending_req = FALSE;
      grr_data->tc.n_acc_req   = 0;
      grr_data->tc.ra_once_sent= 0;

      if(cgrlc_ul_tbf_ind->access_type EQ CGRLC_AT_NULL )
      {
        /*
         * check if single block access is required
         */
        if(tc_get_num_ctrl_blck( )  NEQ 0)
        {
          grr_data->tc.v_sb_without_tbf    = TRUE;
          grr_data->uplink_tbf.nr_blocks   = 1;
          grr_data->uplink_tbf.prio        = RADIO_PRIO_4;
          grr_data->uplink_tbf.access_type = CGRLC_AT_SINGLE_BLOCK;
          grr_data->uplink_tbf.ti          = 0;  /* contention resolution NOT required */
          tc_check_access_is_needed( CAC_OTHER );
        }
        else   
        {
          sig_tc_ctrl_set_pckt_mode( GLBL_PCKT_MODE_IDLE, TASK_STOP_DUMMY_VALUE );
        }
      }
      else
      {
        grr_data->uplink_tbf.access_type = cgrlc_ul_tbf_ind->access_type;
        grr_data->uplink_tbf.prio        = cgrlc_ul_tbf_ind->ra_prio; 
        grr_data->uplink_tbf.nr_blocks   = cgrlc_ul_tbf_ind->nr_blocks;
        grr_data->uplink_tbf.prim_type   = cgrlc_ul_tbf_ind->llc_prim_type;
        grr_data->uplink_tbf.peak        = cgrlc_ul_tbf_ind->peak;
        grr_data->uplink_tbf.rlc_oct_cnt = cgrlc_ul_tbf_ind->rlc_oct_cnt;

        if(grr_data->uplink_tbf.access_type EQ CGRLC_AT_TWO_PHASE)
          grr_data->uplink_tbf.ti = 0;  /* contention resolution NOT required */
        else
          grr_data->uplink_tbf.ti = 1;  /* contention resolution required */


        TRACE_EVENT_P1("tc_cgrlc_ul_tbf_ind: nr_block:  %d", grr_data->uplink_tbf.nr_blocks);
        
        tc_check_access_is_needed( CAC_OTHER );
        /*
         * forward control messages to GRLC
         */
        if(tc_get_num_ctrl_blck( )  NEQ 0)
        {
          tc_send_control_msg_to_grlc();
        }

      }
      break;
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
      /* TBF establishment on PACCH is initiated by GRLC */		
      if(cgrlc_ul_tbf_ind->tbf_est_pacch)
      {
        /* Set TBF establishment on PACCH flag */
        grr_data->tc.tbf_est_pacch = TRUE;
        
        /* Contention resolution is not required */
        grr_data->uplink_tbf.ti = 0;
      }
      break;
#endif
    default:
      TRACE_ERROR( "cgrlc_ul_tbf_ind unexpected" );
      break;
  }

  PFREE(cgrlc_ul_tbf_ind);

} /* tc_cgrlc_ul_tbf_ind() */


/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_ctrl_msg_sent_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_ctrl_msg_sent_ind
|
| Parameters  : cgrlc_ctrl_msg_sent_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_ctrl_msg_sent_ind ( T_CGRLC_CTRL_MSG_SENT_IND * cgrlc_ctrl_msg_sent_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_ctrl_msg_sent_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:   
      /*
       * todo : delete the first ctrl msg. Only type of cntrl msgs will be stored
       */

      break;
    default:
      TRACE_ERROR( "cgrlc_ctrl_msg_sent_ind unexpected" );
      break;
  }

  PFREE(cgrlc_ctrl_msg_sent_ind);

} /* tc_cgrlc_ctrl_msg_sent_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_starting_time_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_starting_time_ind
|
| Parameters  : cgrlc_starting_time_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_starting_time_ind ( T_CGRLC_STARTING_TIME_IND * cgrlc_starting_time_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_starting_time_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE: 
      /*
       * store the tfi
       */
      if(cgrlc_starting_time_ind->tbf_mode EQ CGRLC_TBF_MODE_UL)
      {
        grr_data->uplink_tbf.tfi            = cgrlc_starting_time_ind->tfi;
        grr_data->uplink_tbf.st_tfi         = 0xFF;
        grr_data->uplink_tbf.tbf_start_fn   = CGRLC_STARTING_TIME_NOT_PRESENT;
        grr_data->uplink_tbf.polling_bit    = 0xFF;
        grr_data->uplink_tbf.rlc_db_granted = 0;
        if(grr_data->uplink_tbf.ts_usage)
        {
          grr_data->uplink_tbf.ts_mask        = grr_data->uplink_tbf.ts_usage;
        }
        else
        {
          TRACE_EVENT("tc_cgrlc_starting_time_ind: uplink ts_usage is 0");
        }
        grr_data->uplink_tbf.ts_usage       = 0;

#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
        grr_data->tc.tbf_est_pacch = FALSE;
#endif
      }
      else
      {
        grr_data->downlink_tbf.tfi          = cgrlc_starting_time_ind->tfi;
        grr_data->downlink_tbf.st_tfi       = 0xFF;
        grr_data->downlink_tbf.tbf_start_fn = CGRLC_STARTING_TIME_NOT_PRESENT;
        grr_data->downlink_tbf.polling_bit  = 0xFF;
        if(grr_data->downlink_tbf.ts_usage)
        {
          grr_data->downlink_tbf.ts_mask      = grr_data->downlink_tbf.ts_usage;
        }
        else
        {
          TRACE_EVENT("tc_cgrlc_starting_time_ind: downlink ts_usage is 0");
        }
        grr_data->downlink_tbf.ts_usage     = 0;
      }

      break;
    default:
      TRACE_ERROR( "cgrlc_starting_time_ind unexpected" );
      break;
  }

  PFREE(cgrlc_starting_time_ind);

} /* tc_cgrlc_starting_time_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_t3192_started_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_t3192_started_ind
|
| Parameters  : cgrlc_t3192_started_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_t3192_started_ind ( T_CGRLC_T3192_STARTED_IND * cgrlc_t3192_started_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_t3192_started_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE: 
      grr_data->downlink_tbf.t3192 = TRUE;
      break;
    default:
      TRACE_ERROR( "cgrlc_t3192_started_ind unexpected" );
      break;
  }

  PFREE(cgrlc_t3192_started_ind);

} /* tc_cgrlc_t3192_started_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_cont_res_done_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_cont_res_done_ind
|
| Parameters  : cgrlc_cont_res_done_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_cont_res_done_ind ( T_CGRLC_CONT_RES_DONE_IND * cgrlc_cont_res_done_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_cont_res_done_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
      SET_STATE(TC,TC_TBF_ACTIVE);
      grr_data->uplink_tbf.ti          = 0; 
      /*TRACE_EVENT_P1("CONT_RES:nacc set from %d to zero",grr_data->tc.n_acc_req_procedures);*/
      if(grr_data->cc_running OR grr_t_status( T3174 ) > 0)
      { 
        sig_tc_ctrl_contention_ok();
      }
      break;
    default:
      TRACE_ERROR( "cgrlc_cont_res_done_ind unexpected" );
      break;
  }

  PFREE(cgrlc_cont_res_done_ind);

} /* tc_cgrlc_cont_res_done_ind() */

/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_ta_value_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_ta_value_ind
|
| Parameters  : cgrlc_ta_value_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_ta_value_ind ( T_CGRLC_TA_VALUE_IND * cgrlc_ta_value_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_ta_value_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE:
#if defined REL99 AND defined TI_PS_FF_TBF_EST_PACCH
    case TC_WAIT_2P_ASSIGN:		
#endif
      grr_data->ta_params.ta_valid = TRUE;
      grr_data->ta_params.ta_value = cgrlc_ta_value_ind->ta_value;
/*

      {
        ULONG trace[2];

        trace[0]  = grr_data->ta_params.ul_ta_tn <<  0;
        trace[0] |= grr_data->ta_params.ul_ta_i  <<  8;
        trace[0] |= grr_data->ta_params.ta_value << 16;
        trace[0] |= grr_data->ta_params.ta_valid << 24;

        trace[1]  = grr_data->ta_params.l1_ta_i     <<  0;
        trace[1] |= grr_data->ta_params.l1_ta_value <<  8;
        trace[1] |= grr_data->ta_params.dl_ta_tn    << 16;
        trace[1] |= grr_data->ta_params.dl_ta_i     << 24;

        TRACE_EVENT_P4( "TA_PARAM_2: %08X%08X%02X %02X",
                        trace[0], trace[1], grr_data->ta_params.l1_ta_tn,
                        grr_data->ta_params.ta_value );
      }

*/

      break;
    default:
      TRACE_ERROR( "cgrlc_ta_value_ind unexpected" );
      break;
  }

  PFREE(cgrlc_ta_value_ind);

} /* tc_cgrlc_ta_value_ind() */


/*
+------------------------------------------------------------------------------
| Function    : tc_cgrlc_test_mode_ind
+------------------------------------------------------------------------------
| Description : Handles the primitive cgrlc_test_mode_ind, GRLC is indicating
|               that test mode is runnig
|
| Parameters  : cgrlc_test_mode_ind - Ptr to primitive payload  
|
+------------------------------------------------------------------------------
*/
GLOBAL void tc_cgrlc_test_mode_ind ( T_CGRLC_TEST_MODE_IND * cgrlc_test_mode_ind )
{ 
  TRACE_FUNCTION( "tc_cgrlc_test_mode_ind" );
  

  switch( GET_STATE( TC ) )
  {
    case TC_CONTENTION_RESOLUTION:
    case TC_TBF_ACTIVE: 
    case TC_PIM:/*testmode ended*/

      grr_data->test_mode = cgrlc_test_mode_ind->test_mode_flag;
      sig_tc_ctrl_test_mode_ind();

      break;
    default:
      TRACE_ERROR( "cgrlc_test_mode_ind unexpected" );
      break;
  }

  PFREE(cgrlc_test_mode_ind);

} /* tc_cgrlc_test_mode_ind() */
