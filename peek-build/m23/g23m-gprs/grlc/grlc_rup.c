/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  grlc
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
|             RU of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RUP_C
#define GRLC_RUP_C
#endif

#define ENTITY_GRLC




/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>
#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h" 
#include "grlc_ruf.h"
#include "grlc_tms.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : ru_t3164
+------------------------------------------------------------------------------
| Description : Handles the primitive T3164
|
| Parameters  : *t3164 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3164 ( void )
{ 
  TRACE_FUNCTION( "t3164" );
  
  
  switch( GET_STATE( RU ) )
  {
    case RU_ACK:
    case RU_REL_ACK:
    case RU_UACK:
    case RU_REL_UACK:
    case RU_SEND_PCA:
      SET_STATE(RU,RU_NULL); 
      grlc_data->ru.first_usf = FALSE;
      grlc_data->t3164_to_cnt++;
      TRACE_EVENT_P1("timeout T3164  cnt=%d",grlc_data->t3164_to_cnt);
      sig_ru_tm_error_ra();      
      break;
    default:
      TRACE_ERROR( "T3164 unexpected" );
      TRACE_EVENT( "T3164 unexpected" );
      break;
  }

} /* ru_t3164() */




/*
+------------------------------------------------------------------------------
| Function    : ru_t3166
+------------------------------------------------------------------------------
| Description : Handles the primitive T3166
|
| Parameters  : *t3166 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3166 ( void )
{ 
  TRACE_FUNCTION( "t3166" );
  
  
  switch( GET_STATE( RU ) )
  {
    case RU_ACK:
    case RU_REL_ACK:
      SET_STATE(RU,RU_NULL);
      vsi_t_stop(GRLC_handle,T3180);
      vsi_t_stop(GRLC_handle,T3182);
      TRACE_ERROR("timeout T3166");
      TRACE_EVENT("timeout T3166");
      sig_ru_tm_error_ra();
      break;
    default:
      TRACE_ERROR( "T3166 unexpected" );
      break;
  }

} /* ru_t3166() */




/*
+------------------------------------------------------------------------------
| Function    : ru_t3180
+------------------------------------------------------------------------------
| Description : Handles the primitive T3180
|
| Parameters  : *t3180 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3180 ( void )
{ 
  TRACE_FUNCTION( "t3180" );
  
  
  switch( GET_STATE( RU ) )
  {
    case RU_ACK:
    case RU_UACK:
    case RU_REL_ACK:    
    case RU_REL_UACK:    
      SET_STATE(RU,RU_NULL);
      TRACE_ERROR("timeout T3180");
      TRACE_EVENT("timeout T3180");
      ru_handle_n3102(PAN_DEC);
      break;
    default:
      TRACE_ERROR( "T3180 unexpected" );
      TRACE_EVENT( "T3180 unexpected" );
      break;
  }

} /* ru_t3180() */



/*
+------------------------------------------------------------------------------
| Function    : ru_t3182
+------------------------------------------------------------------------------
| Description : Handles the primitive T3182
|
| Parameters  : *t3182 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3182 ( void )
{ 
  TRACE_FUNCTION( "t3182" );

  TRACE_EVENT_P1("timeout T3182 state=%d", grlc_data->ru.state);
  
  switch( GET_STATE( RU ) )
  {
    case RU_ACK:
    case RU_UACK:
      /*
       * Timeout during stall indcation
       */
      SET_STATE(RU,RU_NULL);
      TRACE_ERROR("timeout T3182 ack state");
      ru_handle_n3102(PAN_DEC);      
      break;
    case RU_REL_ACK:
      SET_STATE(RU,RU_NULL);
      TRACE_ERROR("timeout T3182 rel ack");
      sig_ru_tm_error_ra();
      break;
    case RU_REL_UACK:
      SET_STATE(RU,RU_NULL);
      TRACE_ERROR("timeout T3182 rel uack state");
      grlc_data->tbf_ctrl[grlc_data->ul_index].end_fn = grlc_data->ul_fn;
      grlc_data->tbf_ctrl[grlc_data->ul_index].vs_vr  = grlc_data->ru.vs;
      grlc_data->tbf_ctrl[grlc_data->ul_index].va_vq  = grlc_data->ru.va;
      grlc_data->tbf_ctrl[grlc_data->ul_index].cnt_ts = grlc_data->ru.cnt_ts;
      grlc_trace_tbf_par ( grlc_data->ul_index );    
      /* in unack mode we need to delete the last llc pdu that constituted
       * rlc block with cv=0. Unlike other rlc blocks in unack mode, block with
       * cv=0 transmission will not relult in freeing of llc pdu as cv=0 
       * may have to be retransmitted 4 times.
       * In case of timer expiry llc pdu needs to be deleted here.
       * timer t3180 also needs to be stopped.
       */
      ru_delete_prims(grlc_data->ru.vs);
      vsi_t_stop(GRLC_handle,T3180);
      sig_ru_tm_end_of_tbf();
      break;
    default:
      TRACE_ERROR( "T3182 unexpected" );
      break;
  }

} /* ru_t3182() */




/*
+------------------------------------------------------------------------------
| Function    : ru_t3184
+------------------------------------------------------------------------------
| Description : Handles the primitive T3184
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3184 ( void )
{ 
  TRACE_FUNCTION( "t3184" );
  
  TRACE_EVENT_P1("timeout T3184 state=%d", grlc_data->ru.state);
  
  switch( GET_STATE( RU ) )
  {
    case RU_ACK:
    case RU_REL_ACK:
    case RU_SEND_PCA:
      SET_STATE(RU,RU_NULL);
      TRACE_ERROR("timeout T3184");
      sig_ru_tm_error_ra();
      break;
    default:
      TRACE_ERROR( "T3184 unexpected" );
      break;
  }

} /* ru_t3184() */

/*
+------------------------------------------------------------------------------
| Function    : ru_t3314
+------------------------------------------------------------------------------
| Description : Handles the primitive T3314
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void ru_t3314 ( void )
{ 
  TRACE_FUNCTION( "ru_t3314" );
  
  TRACE_EVENT_P1("timeout T3314 state = %d", grlc_data->ru.state);
  
  grlc_enter_standby_state( );

} /* ru_t3314() */
