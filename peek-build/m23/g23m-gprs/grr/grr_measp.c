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
|             MEAS of entity GRR.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRR_MEASP_C
#define GRR_MEASP_C
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
#include "grr_f.h"
#include "grr_measf.h"  /* to get the definitions for interference measurements */
#include "grr_css.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : meas_p_meas_order_ind
+------------------------------------------------------------------------------
| Description : ...
|
| Parameters  : *d_meas_order - Ptr to packet measurement order message
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_p_meas_order_ind ( T_D_MEAS_ORDER *d_meas_order )
{ 
  T_NC_ORDER ctrl_order;
  UBYTE      valid_pars;

  TRACE_FUNCTION( "meas_p_meas_order_ind" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_NULL     :
    case MEAS_IDLE     :
    case MEAS_ACCESS   :
    case MEAS_TRANSFER :
      valid_pars = meas_em_prcs_meas_order( d_meas_order, &ctrl_order );

      if( ( valid_pars & PMO_EXT_VLD ) EQ PMO_EXT_VLD )
      {
        switch( GET_STATE( MEAS_EM ) )
        {
          case MEAS_EM_REP_REQ:
            meas_em_stop_req( );
           
            /*lint -fallthrough*/

          default:
            meas_em_valid_pmo( );
            meas_em_process_t3178( );
            break;
        }
      }
      if( ( valid_pars & PMO_NC_VLD ) EQ PMO_NC_VLD )
      {
        sig_meas_cs_meas_order( ctrl_order );
      }
      break;

    default:
      TRACE_ERROR( "PACKET MEASUREMENT ORDER unexpected 2" );
      break;
  }
} /* meas_p_meas_order_ind() */

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : meas_t_im_sync
+------------------------------------------------------------------------------
| Description : Handles the timeout of timer T_IM_SYNC
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_t_im_sync ( void )
{ 
  TRACE_FUNCTION( "meas_t_im_sync" );
  
  meas_im_timer_elapsed( );
} /* meas_t_im_sync() */

/*
+------------------------------------------------------------------------------
| Function    : meas_t3178
+------------------------------------------------------------------------------
| Description : Handles the timeout of timer T3178
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_t3178 ( void )
{ 
  TRACE_FUNCTION( "meas_t3178" );

  TRACE_EVENT( "T3178 timeout" );

  switch( GET_STATE( MEAS ) )
  {
    case MEAS_IDLE:
      switch( GET_STATE( MEAS_EM ) )
      {
        case MEAS_EM_NULL:
          meas_em_start( );
          
          SET_STATE( MEAS_EM, MEAS_EM_REP_REQ );
          break;

        default:
          meas_em_process_t3178( );
          break;
      }
      break;

    case MEAS_ACCESS:
    case MEAS_TRANSFER:
      /*
       * if the MS is in packet transfer mode when the measurement shall start,
       * the MS shall wait until it returns to packet idle mode
       */
      switch( GET_STATE( MEAS_EM ) )
      {
        case MEAS_EM_NULL:
        case MEAS_EM_REP_REQ:
          SET_STATE( MEAS_EM, MEAS_EM_PENDING );

          meas_em_process_t3178( );
          break;
        
        case MEAS_EM_PMR_SENDING:
        case MEAS_EM_PENDING:
          meas_em_process_t3178( );
          break;
      }
      break;

    default:
      TRACE_ERROR( "T3178 timeout unexpected 3" );
      break;
  }  
} /* meas_t3178() */

/*
+------------------------------------------------------------------------------
| Function    : meas_tb_rxlev_sc_req
+------------------------------------------------------------------------------
| Description : Handles the primitive TB_RXLEV_SC_REQ
|
| Parameters  : *tb_rxlev_sc_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void meas_tb_rxlev_sc_req ( T_TB_RXLEV_SC_REQ *tb_rxlev_sc_req )
{ 
  TRACE_FUNCTION( "meas_tb_rxlev_sc_req" );
  
  switch( GET_STATE( MEAS ) )
  {
    case MEAS_NULL:
      break;

    case MEAS_IDLE:
    case MEAS_ACCESS:
      if( grr_is_pbcch_present( ) EQ FALSE )
      {
        meas_c_val_update_pim( tb_rxlev_sc_req->sc_rxlev );
        grr_cgrlc_pwr_ctrl_req( TRUE );
      }
      break;

    default:
      TRACE_ERROR( "TB_RXLEV_SC_REQ unexpected" );
      break;
  }

  PFREE( tb_rxlev_sc_req );

} /* meas_tb_rxlev_sc_req() */
