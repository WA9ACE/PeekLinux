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
|             CPAP of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_CPAPS_C
#define GRR_CPAPS_C
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
#include <string.h>
#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"
#include "grr_tcs.h"
#include "grr_cpapf.h"




/*==== CONST ================================================================*/

/*==== DIAGNOSTICS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_abort_access
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_ABORT_ACCESS
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_abort_access ( void ) 
{ 
  TRACE_ISIG( "sig_tc_cpap_abort_access" );

  switch( GET_STATE( CPAP ) )
  {
    case CPAP_WAIT_STOP_TASK_CNF:
      cpap_mfree_ia( FALSE );

      /*lint -fallthrough*/

    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
    case CPAP_WAIT_PDCH_ASSIGN: 
      SET_STATE( CPAP, CPAP_IDLE );

      sig_cpap_tc_error_pim();
      break; 
    default:
      TRACE_ERROR( "SIG_TC_CPAP_ABORT_ACCESS unexpected" );
      break;
  }
} /* sig_tc_cpap_abort_access() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_acc_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_ACC_REQ
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_acc_req ( void ) 
{ 
/*  UBYTE random_value = 0; random value is handled by rr */
  UBYTE req_data = 0;

  TRACE_ISIG( "sig_tc_cpap_acc_req" );

  
 
  switch( GET_STATE( CPAP ) )
  {
    case CPAP_IDLE:
      /* estimate access type */
      switch(grr_data->uplink_tbf.access_type)
      {
        case CGRLC_AT_ONE_PHASE:
        case CGRLC_AT_SHORT_ACCESS:
        /*
           * if ms class allows more than one uplink slot, 
           * then two phase access is triggered.
           * Otherwise multislot uplink is not assigned on some network vendors */
#ifdef _TARGET_  
          if(grr_data->ms_cap[grr_get_gprs_ms_class( )-1].Tx > 1)
          {
            SET_STATE(CPAP,CPAP_SB_STARTED);
            req_data = 0x70;
            TRACE_EVENT_P1("2 PHASE FOR UPLINK MULTISLLOT ms_class=%d"
                             ,grr_get_gprs_ms_class( ));
            break;
          }
#endif
        case CGRLC_AT_PAGE_RESPONSE:
        case CGRLC_AT_MM_PROCEDURE:
        case CGRLC_AT_CELL_UPDATE:
          req_data = 0x78;  
          /*
           * due to 04.08. chapter 9.1.8 random value for last 3 bit 
           * shall be between 0 and 7
           */
          /* random value is handled by rr */
          SET_STATE(CPAP,CPAP_1P_STARTED);
          break;

        case CGRLC_AT_TWO_PHASE:
        case CGRLC_AT_SINGLE_BLOCK:
          req_data = 0x70;  
          /*
           * due to 04.08. chapter 9.1.8 random value for last 3 bit 
           * shall be between 0 and 7
           */
          /* random value is handled by rr */
          SET_STATE(CPAP,CPAP_SB_STARTED);
          break;
        default:
          break;
      }
      {
        PALLOC(rrgrr_gprs_data_req,RRGRR_GPRS_DATA_REQ);

        cpap_build_gprs_data_request(rrgrr_gprs_data_req);
        sig_cpap_tc_send_gprs_data_req(rrgrr_gprs_data_req);
      }
      sig_cpap_tc_channel_req(req_data);
      break;

    default:
      /*
       * SZML-CPAP/017
       */
      TRACE_ERROR( "SIG_TC_CPAP_ACC_REQ unexpected" );
      break;
  }
} /* sig_tc_cpap_acc_req() */




/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_pdch_assign_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_PDCH_ASSIGN_RECEIVED
|
| Parameters  : fn - reception frame number
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_pdch_assign_received (T_RRGRR_DATA_IND *rrgrr_data_ind) 
{ 
  TRACE_ISIG( "sig_tc_cpap_pdch_assign_received" );
  grr_data->dl_fn = rrgrr_data_ind->fn;
  grr_data->ul_fn = rrgrr_data_ind->fn;
  switch( GET_STATE( CPAP ) )
  {
    case CPAP_WAIT_PDCH_ASSIGN:
      switch(cpap_eval_pdch_assignment())
      {
        case E_PDCH_ASSIGN_UL:
          memcpy(&grr_data->cpap.pdch_ass_cmd,_decodedMsg,sizeof(T_D_PDCH_ASS_CMD));
          grr_data->cpap.new_tbf_type = CGRLC_TBF_MODE_UL;
          SET_STATE(CPAP,CPAP_WAIT_DEDI_SUSPEND);
          sig_cpap_tc_suspend_dedi_chan();
          break;
        case E_PDCH_ASSIGN_SB:
          break;
        case E_PDCH_ASSIGN_DL:
          break;
        case E_PDCH_ASSIGN_IGNORE:
          break;
        case E_PDCH_ASSIGN_ERROR:
          cpap_send_ass_fail(RRC_CHANNEL_MODE);
          break;
        default:
          break;
      }
      break;
      
    default:
      TRACE_ERROR( "sig_tc_cpap_pdch_assign_received unexpected" );
      break;
  }
} /* sig_tc_cpap_pdch_assign_received() */


/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_ia_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_IA_RECEIVED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_ia_received (T_RRGRR_IA_IND *rrgrr_ia_ind) 
{ 
  MCAST(d_imm_assign,D_IMM_ASSIGN); /* T_D_IMM_ASSIGN */
  T_EVAL_IA eval;

  TRACE_ISIG( "sig_tc_cpap_ia_received" );
  
  grr_data->dl_fn = rrgrr_ia_ind->fn;
  grr_data->ul_fn = rrgrr_ia_ind->fn;
  grr_data->r_bit = rrgrr_ia_ind->r_bit; /* chan req is sent at least twice on ccch */
  switch( GET_STATE( CPAP ) )
  {
    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
      eval = cpap_eval_ia();
      if(E_IA_TMA_SECOND EQ eval)
      { /* 2nd message of TMA */
        if(grr_data->cpap.v_tma_ia)
        {
          cpap_join_tma_messages();
          eval = cpap_eval_ia();
        }
        else
        {
          TRACE_ERROR("2nd TMA IA received without 1st!");
          SET_STATE(CPAP,CPAP_IDLE);

          sig_cpap_tc_error_pim( );
          return;
        }
      }

      grr_data->cpap.last_ia_type                  = IA_TYPE_UL;
      grr_data->cpap.last_eval_ia.param_ul.eval_ia = eval;
        
      if( E_IA_UL EQ eval )
      { /* Dynamic Allocation */
        grr_data->cpap.v_tma_ia = FALSE;

        if(d_imm_assign->ia_rest_oct.ia_assign_par.pck_upl_ass_ia.tfi_ass_alloc.v_tbf_start_time)
          sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        else
          sig_cpap_tc_send_stop_task_req( STOP_CCCH );
        
        cpap_malloc_ia( );

        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
        return;
      }
      else if(E_IA_DCCH EQ eval)
      { /* continue on DCCH */
        if(grr_t_status( T3172_1  ) > 0 )
        {
            /*
             * Timer is stil running
             */
            sig_cpap_tc_stop_T3142();
        }
        
        grr_data->cpap.v_tma_ia = FALSE;
        SET_STATE(CPAP,CPAP_WAIT_PDCH_ASSIGN);
      }
      else if(E_IA_SB_2PHASE_ACCESS EQ eval)
      { /* Allocation for 2 Phase of 2_Phase-Access */
        grr_data->cpap.v_tma_ia = FALSE;

        sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        cpap_malloc_ia( );

        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
      }
      else if(E_IA_TMA EQ eval)
      { /* 1st message of TMA */
        cpap_store_ia_message();
      }
      else if(E_IA_ERROR_RA EQ eval)
      {
        /* errornous messages are ignored?!? */
        TRACE_ERROR("ia message contents nonsense!");
        SET_STATE(CPAP,CPAP_IDLE);
        grr_data->cpap.v_tma_ia = FALSE;
        sig_cpap_tc_error_pim();
      }
      else if(E_IA_NULL EQ eval)
      {
        /* unforeseen mesage  content*/
      }
      else if(E_IA_SB_WITHOUT_TBF EQ eval)
      {
        grr_data->cpap.v_tma_ia = FALSE;

        sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        cpap_malloc_ia( );

        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
        /*TRACE_EVENT("E_IA_SB_WITHOUT_TBF: stop task");*/
      }
    
      if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      OR
          grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param OR
          grr_data->pwr_ctrl_valid_flags.v_freq_param             )
      {
        grr_cgrlc_pwr_ctrl_req( FALSE );
      }
      break;

    case CPAP_IDLE:
    case CPAP_WAIT_PDCH_ASSIGN:
    default:
      TRACE_ERROR( "SIG_TC_CPAP_IA_RECEIVED unexpected" );
      break;
  }
} /* sig_tc_cpap_ia_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_iaext_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_IAEXT_RECEIVED
|
| Parameters  : ia_index - indicates the correct immediate assignment
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_iaext_received ( T_RRGRR_IAEXT_IND *rrgrr_iaext_ind ) 
{ 
  TRACE_ISIG( "sig_tc_cpap_iaext_received" );
  /* 
   * SZML-CPAP/024
   */
  grr_data->dl_fn = rrgrr_iaext_ind->fn;
  grr_data->ul_fn = rrgrr_iaext_ind->fn;
  grr_data->r_bit = rrgrr_iaext_ind->r_bit; /* chan req is sent at least twice on ccch */

  switch( GET_STATE( CPAP ) )
  {
    case CPAP_IDLE:
      /* SZML-CPAP/025 */
      break;
    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
      SET_STATE(CPAP,CPAP_WAIT_PDCH_ASSIGN);
      break;
    case CPAP_WAIT_PDCH_ASSIGN:
      break;

    default:
      TRACE_ERROR( "SIG_TC_CPAP_IAEXT_RECEIVED unexpected" );
      break;
  }
} /* sig_tc_cpap_iaext_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_iarej_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_IAREJ_RECEIVED
|
| Parameters  : wait_ind - is taken from the corresponding air message 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_iarej_received (T_RRGRR_ASSIGNMENT_REJ_IND *rrgrr_assignment_rej_ind) 
{ 
  TRACE_ISIG( "sig_tc_cpap_iarej_received" );
  grr_data->r_bit = rrgrr_assignment_rej_ind->r_bit; /* chan req is sent at least twice on ccch */

  switch( GET_STATE( CPAP ) )
  {
    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
      if(grr_t_status( T3172_1 ) EQ 0 )
      {
        /*Timer not running*/
        if(rrgrr_assignment_rej_ind->wait_ind)
        {
          sig_cpap_tc_start_T3142(1000 * rrgrr_assignment_rej_ind->wait_ind);
        }
        else
        {
          SET_STATE(CPAP, CPAP_IDLE);

          sig_cpap_tc_error_pim();
        }
      } /* no else path because additional rejects have to be ignored!! */
      break;

    default:
      TRACE_ERROR( "SIG_TC_CPAP_IAREJ_RECEIVED unexpected" );
      break;
  }
} /* sig_tc_cpap_iarej_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_ia_downlink_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_IA_DOWNLINK_RECEIVED
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_ia_downlink_received (T_RRGRR_IA_DOWNLINK_IND *rrgrr_ia_downlink_ind) 
{
  MCAST(d_imm_assign,D_IMM_ASSIGN);/*  T_D_IMM_ASSIGN */
  UBYTE        state = GET_STATE( CPAP );
  T_EVAL_IA_DL eval;

  TRACE_ISIG( "sig_tc_cpap_ia_downlink_received" );

  /* SZML-CPAP/026 */

  grr_data->dl_fn = rrgrr_ia_downlink_ind->fn;
  grr_data->ul_fn = rrgrr_ia_downlink_ind->fn;

  switch( state )
  {
    case CPAP_IDLE:
      eval = cpap_eval_ia_dl();

      if(E_IA_DL_TMA_SECOND EQ eval)
      {
        if(grr_data->cpap.v_tma_ia)
        {
          cpap_join_tma_messages();
          eval = cpap_eval_ia_dl();
        }
        else
        {
          TRACE_ERROR("2nd TMA IA received without 1st!");
          sig_cpap_tc_send_stop_task_req( INVALID_MSG );
          return;
        }
      }

      grr_data->cpap.last_ia_type                     = IA_TYPE_DL;
      grr_data->cpap.last_eval_ia.param_dl.state      = state;
      grr_data->cpap.last_eval_ia.param_dl.eval_ia_dl = eval;  
        
      if(E_IA_DL_SB EQ eval)
      {
        grr_data->cpap.v_tma_ia = FALSE;
        cpap_malloc_ia( );
        sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
      }
      else if(E_IA_DL_ASSIGN EQ eval)
      {
        grr_data->cpap.v_tma_ia = FALSE;
        if(d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.v_tbf_start_time)
          sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        else
          sig_cpap_tc_send_stop_task_req( STOP_CCCH );

        cpap_malloc_ia( );
        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
      }
      else if(E_IA_DL_TMA EQ eval)
      {
        cpap_store_ia_message();
      }
      else if(E_IA_DL_NOT_OURS EQ eval)
      {
        TRACE_EVENT("Message not adressed to MS");
        sig_cpap_tc_send_stop_task_req( INVALID_MSG );
      }
      else if(E_IA_DL_IGNORE EQ eval)
      {
        TRACE_EVENT("INVALID DL IA RECEIVED 1 ");
        sig_cpap_tc_send_stop_task_req( INVALID_MSG );
      }

      if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      OR
          grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param OR
          grr_data->pwr_ctrl_valid_flags.v_freq_param             )
      {
        grr_cgrlc_pwr_ctrl_req( FALSE );
      }
      break;      
    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
      eval = cpap_eval_ia_dl();

      if(E_IA_DL_TMA_SECOND EQ eval)
      {
        if(grr_data->cpap.v_tma_ia)
        {
          cpap_join_tma_messages();
          eval = cpap_eval_ia_dl();
        }
        else
        {
          TRACE_ERROR("2nd TMA IA received without 1st!");
          sig_cpap_tc_send_stop_task_req( INVALID_MSG );
          return;
        }
      }

      grr_data->cpap.last_ia_type                     = IA_TYPE_DL;
      grr_data->cpap.last_eval_ia.param_dl.state      = state;
      grr_data->cpap.last_eval_ia.param_dl.eval_ia_dl = eval;  

        
      if(E_IA_DL_ASSIGN EQ eval)
      {
        grr_data->cpap.v_tma_ia = FALSE;       
        if(d_imm_assign->ia_rest_oct.ia_assign_par.pck_downl_ass_ia.v_tbf_start_time)
          sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        else
          sig_cpap_tc_send_stop_task_req( STOP_CCCH );
        
        cpap_malloc_ia( );
        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
      }
      else if(E_IA_DL_SB EQ eval)
      {
        TRACE_EVENT_P3("IA_DL_SB ia_type=%d dl_state =%d eval_ia=%d",grr_data->cpap.last_ia_type
                                                                    ,grr_data->cpap.last_eval_ia.param_dl.state
                                                                    ,grr_data->cpap.last_eval_ia.param_dl.eval_ia_dl);
        grr_data->cpap.v_tma_ia = FALSE;
        cpap_malloc_ia( );
        sig_cpap_tc_send_stop_task_req( DONT_STOP_CCCH );
        SET_STATE( CPAP, CPAP_WAIT_STOP_TASK_CNF );
      }
      else if(E_IA_DL_TMA EQ eval)
      {
        cpap_store_ia_message();
      }
      else if(E_IA_DL_NOT_OURS EQ eval)
      {
        sig_cpap_tc_send_stop_task_req( INVALID_MSG );
        TRACE_EVENT("Message not addressed to MS");
      }
      else if(E_IA_DL_IGNORE EQ eval)
      {
        TRACE_EVENT("INVALID DL IA RECEIVED  2 ");
        sig_cpap_tc_send_stop_task_req( INVALID_MSG );
      }
    
      if( grr_data->pwr_ctrl_valid_flags.v_pwr_ctrl_param      OR
          grr_data->pwr_ctrl_valid_flags.v_glbl_pwr_ctrl_param OR
          grr_data->pwr_ctrl_valid_flags.v_freq_param             )
      {
        grr_cgrlc_pwr_ctrl_req( FALSE );
      }
      break;      
    case CPAP_WAIT_STOP_TASK_CNF:
      {
        TRACE_EVENT( "SIG_TC_CPAP_IA_DOWNLINK_RECEIVED processing of another IA not finished" );
      }
      break;
    default:
      {
        TRACE_ERROR( "SIG_TC_CPAP_IA_DOWNLINK_RECEIVED unexpected" );
        sig_cpap_tc_send_stop_task_req( INVALID_MSG );
      }
      break;
  }
} /* sig_tc_cpap_ia_downlink_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_stop_task_cnf_received
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_STOP_TASK_CNF
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_stop_task_cnf_received( void )
{ 
  T_TBF_TYPE tbf_type;

  TRACE_ISIG( "sig_tc_cpap_stop_task_cnf_received" );
/*
  TRACE_EVENT_P3("IA_DL_SB ia_type=%d dl_state =%d eval_ia=%d",grr_data->cpap.last_ia_type
                                                                    ,grr_data->cpap.last_eval_ia.param_dl.state
                                                                    ,grr_data->cpap.last_eval_ia.param_dl.eval_ia_dl);
*/
  switch( GET_STATE( CPAP ) )
  {
    case CPAP_WAIT_STOP_TASK_CNF:

      cpap_mfree_ia( TRUE );

      switch( grr_data->cpap.last_ia_type )
      {
        case IA_TYPE_UL:
          
          switch( grr_data->cpap.last_eval_ia.param_ul.eval_ia )
          {
            case E_IA_UL:
              tbf_type = CGRLC_TBF_MODE_UL;            
              if( cpap_send_assign_req_ia( tbf_type ) )
              {
                if(grr_t_status( T3172_1 ) > 0 )
                {
                  sig_cpap_tc_stop_T3142( );
                }
                
                SET_STATE( CPAP,CPAP_IDLE );
                
                sig_cpap_tc_tbf_created( tbf_type );
              }
              else
              {
                SET_STATE( CPAP,CPAP_IDLE );
                
                sig_cpap_tc_error_pim();
              }
              break;
          case E_IA_SB_2PHASE_ACCESS:              
              if(cpap_send_resource_request_ia())
              {
                SET_STATE( CPAP,CPAP_IDLE );
                if(grr_t_status( T3172_1 ) > 0 )
                {
                  sig_cpap_tc_stop_T3142();
                }
              }
              else
              {
                SET_STATE( CPAP,CPAP_IDLE );
                sig_cpap_tc_error_pim();
              }
              break;
            case E_IA_SB_WITHOUT_TBF:
              SET_STATE(CPAP,CPAP_IDLE);
              /*TRACE_EVENT("E_IA_SB_WITHOUT_TBF: stop task cnf received");*/
              if(cpap_send_single_block_without_tbf())
              {
                if(grr_t_status( T3172_1 ) > 0 )
                {
                  sig_cpap_tc_stop_T3142();
                }
              }
              else
              {
                sig_cpap_tc_error_pim();
              }
              break;
             default:
               SET_STATE( CPAP, CPAP_IDLE );
               TRACE_ERROR( "SIG_TC_CPAP_STOP_TASK_CNF: eval_ia invalid" );
               return;
          }

          break;

        case IA_TYPE_DL:

          switch( grr_data->cpap.last_eval_ia.param_dl.state )
          {
            case CPAP_IDLE:
            case CPAP_SB_STARTED:
            case CPAP_1P_STARTED:
              switch( grr_data->cpap.last_eval_ia.param_dl.eval_ia_dl )
              {
                case E_IA_DL_ASSIGN:
                  if( cpap_send_assign_req_ia( CGRLC_TBF_MODE_DL ) )
                  {
                    if(grr_t_status( T3172_1 ) > 0 )
                    {
                      sig_cpap_tc_stop_T3142( );
                    }                
                    sig_cpap_tc_tbf_created( CGRLC_TBF_MODE_DL );
                  }
                  else
                  {               
                    sig_cpap_tc_error_pim();
                  }
                break;
                case E_IA_DL_SB:
                  if(!cpap_send_receive_normal_burst())
                  {
                    sig_cpap_tc_error_pim();
                  }
                  else if(grr_t_status( T3172_1 ) > 0)
                  {
                    sig_cpap_tc_stop_T3142();
                  }
                  break;
                default:
                  TRACE_ERROR( "SIG_TC_CPAP_STOP_TASK_CNF: eval_ia_dl invalid cpap_sb_1p_started" );
                  sig_cpap_tc_error_pim();
                  return;
              }
              SET_STATE( CPAP,CPAP_IDLE );
              break;

            default:
               SET_STATE( CPAP, CPAP_IDLE );
               TRACE_ERROR( "SIG_TC_CPAP_STOP_TASK_CNF: state invalid" );
               return;
          }
          break;

        default:
          SET_STATE( CPAP, CPAP_IDLE );
          TRACE_ERROR( "SIG_TC_CPAP_STOP_TASK_CNF: last_ia_type invalid" );
          return;
      }
      break;

    default:
      TRACE_ERROR( "SIG_TC_CPAP_STOP_TASK_CNF unexpected" );
      break;
  }
} /* sig_tc_cpap_stop_task_cnf_received() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_new_cell
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_NEW_CELL in case of 
|               T3142 is running it shall bestopped. New Access procedures if 
|               necessary will be started by service TC.
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_new_cell ( void ) 
{ 
  if( GET_STATE( CPAP ) EQ CPAP_WAIT_STOP_TASK_CNF )
  {
    cpap_mfree_ia( FALSE );
  }

  SET_STATE(CPAP,CPAP_IDLE);
} /* sig_tc_cpap_new_cell() */




/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_con_res_fail
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_CON_RES_FAIL  
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_con_res_fail ( void ) 
{ 
  TRACE_ISIG( "sig_tc_cpap_con_res_fail" );

  switch(GET_STATE(CPAP))
  {
    default:
      TRACE_ERROR("sig_tc_cpap_con_res_fail unexpected!");
      break;
    case CPAP_IDLE:
      SET_STATE(CPAP,CPAP_WAIT_PDCH_ASSIGN);
      sig_cpap_tc_resume_dedi_chan();
      cpap_send_ass_fail(RRC_PROT_UNSPECIFIED);
      break;
  }
  
      
} /* sig_tc_cpap_con_res_fail() */



/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_suspend_dcch_cnf
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_SUSPEND_DCCH_CNF  
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_suspend_dcch_cnf ( void ) 
{ 
  TRACE_ISIG( "sig_tc_cpap_suspend_dcch_cnf" );

  switch(GET_STATE(CPAP))
  {
    case CPAP_WAIT_DEDI_SUSPEND:
      memcpy(_decodedMsg,&grr_data->cpap.pdch_ass_cmd,sizeof(T_D_PDCH_ASS_CMD));
      cpap_send_assign_req_pdch(grr_data->cpap.new_tbf_type);
      sig_cpap_tc_ctrl_ptm();
      sig_cpap_tc_tbf_created(grr_data->cpap.new_tbf_type);

      break;
    default:
      TRACE_ERROR("sig_tc_cpap_suspend_dcch_cnf unexpected!");
      break;
  }
  
      
} /* sig_tc_cpap_suspend_dcch_cnf() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_t3142
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_T3142
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_tc_cpap_t3142 ( void )
{ 
  TRACE_FUNCTION( "sig_tc_cpap_t3142" );
  
  switch( GET_STATE( CPAP ) )
  {

      
    case CPAP_IDLE:
      /* nothing to do */
      TRACE_EVENT( "SIG_TC_CPAP_T3142 in CPAP_IDLE: nothing to do" );
      /* this should not happen */
      break;
    case CPAP_1P_STARTED:
    case CPAP_SB_STARTED:
      SET_STATE(CPAP,CPAP_IDLE);
      /* sig_cpap_tc_error_pim(); */
      break;
    case CPAP_WAIT_STOP_TASK_CNF:
      SET_STATE(CPAP,CPAP_IDLE);

      cpap_mfree_ia( FALSE );
      /* sig_cpap_tc_error_pim(); */
      break;
    default:
      TRACE_ERROR( "SIG_TC_CPAP_T3142 unexpected" );
      break;

  }

} /* sig_tc_cpap_t3142() */

/*
+------------------------------------------------------------------------------
| Function    : sig_tc_cpap_reconnect_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_TC_CPAP_RECONNECT_IND  
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_tc_cpap_reconnect_ind ( void )
{
    switch(GET_STATE(CPAP))
  {
    case CPAP_WAIT_STOP_TASK_CNF:
      cpap_mfree_ia( FALSE );

      /*lint -fallthrough*/

    case CPAP_IDLE:
    case CPAP_SB_STARTED:
    case CPAP_1P_STARTED:
    case CPAP_WAIT_DEDI_SUSPEND:
      SET_STATE(CPAP,CPAP_WAIT_PDCH_ASSIGN);
      break;
    default:
      TRACE_ERROR("sig_tc_cpap_reconnect_ind unexpected!");
      break;
  }
} /* sig_tc_cpap_reconnect_ind ()*/
