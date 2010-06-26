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
|             CS of entity GRR.
+----------------------------------------------------------------------------- 
*/ 
         
#ifndef GRR_CSP_C
#define GRR_CSP_C
#endif

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */

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
#include "grr_csf.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/
/*
+------------------------------------------------------------------------------
| Function    : cs_t3158
+------------------------------------------------------------------------------
| Description : Handles the timeout of timer T3158
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_t3158 ( void )
{ 
#if defined (REL99) AND defined (TI_PS_FF_EMR)
  BOOL is_enh_meas = cs_is_enh_meas_reporting();
#endif

  TRACE_FUNCTION( "cs_t3158" );

  TRACE_EVENT( "T3158 timeout" );

  cs_store_rxlev_values( );
  cs_reset_all_rxlev_results( );

  switch( GET_STATE( CS ) )
  {
    case CS_IDLE:
  
      switch( GET_STATE( CS_MEAS ) )
      {
        case CS_MEAS_NULL:
          if( grr_is_pbcch_present( ) EQ FALSE )
          {
            cs_send_meas_rep_req( NC_REPORT_MEAS );

            SET_STATE( CS_MEAS, CS_MEAS_REP_REQ ); 
          }
          else
          {
            SET_STATE( CS_MEAS, CS_MEAS_PMR_SENDING );

            
#if defined (REL99) AND defined (TI_PS_FF_EMR)
          if( ((is_enh_meas EQ TRUE) AND (cs_send_enh_meas_rpt() EQ FALSE))
              OR
              ((is_enh_meas EQ FALSE) AND (cs_send_meas_rpt(TRUE) EQ FALSE)) )
#else
          if( cs_send_meas_rpt( TRUE ) EQ FALSE )
#endif

            {
              /*
               * The additional state check is necessary because signals might
               * be send during the processing of the function cs_send_meas_rpt.
               */
              if( GET_STATE( CS_MEAS ) EQ CS_MEAS_PMR_SENDING )
              {
                SET_STATE( CS_MEAS, CS_MEAS_NULL );
              }
            }
          }
          break;

        default:
          break;
      }
 
      cs_process_t3158( );
      break;

    case CS_CR_NETWORK:
    default:
      /*
       * RR performs an IA reject procedure resulting in a cell re-selection 
       * when timer T3158 expires. Timeout of T3158 resulted in a "T3158 timeout
       * unexpected", this should be avoided.
       */
      cs_process_t3158( );
      break;
  }
} /* cs_t3158() */

/*
+------------------------------------------------------------------------------
| Function    : cs_t_reselect
+------------------------------------------------------------------------------
| Description : Handles the timeout of timer T_RESELECT
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void cs_t_reselect ( void )
{ 
  TRACE_FUNCTION( "cs_t_reselect" );

  if( grr_t_status( T3174 ) > 0 )
  {
    TRACE_EVENT( "t_reslect expired when t3174 is running" );
    return;
  }
  cs_cr_decision( CRDM_T_RESELECT );
} /* cs_t_reselect() */
