/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|             RD of entity GRLC.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RDP_C
#define GRLC_RDP_C
#endif

#define ENTITY_GRLC

/*==== INCLUDES =============================================================*/

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
#include "grlc_tms.h"
#include "grlc_rdf.h"
#include "grlc_f.h"

/*==== CONST ================================================================*/


/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : rd_t3190
+------------------------------------------------------------------------------
| Description : Handles the primitive T3190
|
| Parameters  : *t3190 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_t3190 ( void )
{ 
  TRACE_FUNCTION( "t3190" );
  
  
  switch( GET_STATE( RD ) )
  {
    case RD_ACK:
    case RD_UACK:
    case RD_REL_ACK:
    case RD_REL_UACK:
      SET_STATE(RD,RD_NULL);
      TRACE_EVENT("timeout T3190");

      rd_free_desc_list_partions();
      rd_free_database_partions();

      grlc_data->tbf_ctrl[grlc_data->dl_index].vs_vr  = grlc_data->rd.vr;
      grlc_data->tbf_ctrl[grlc_data->dl_index].va_vq  = grlc_data->rd.vq;
      grlc_trace_tbf_par ( grlc_data->dl_index );
      sig_rd_tm_end_of_tbf(TRUE);
      break;
    default:
      TRACE_ERROR( "T3190 unexpected" );
      break;
  }

} /* rd_t3190() */



/*
+------------------------------------------------------------------------------
| Function    : rd_t3192
+------------------------------------------------------------------------------
| Description : Handles the primitive T3192
|
| Parameters  : *t3192 - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void rd_t3192 ( void )
{ 
  TRACE_FUNCTION( "t3192" );
  
  
  switch( GET_STATE( RD ) )
  {
    case RD_REL_ACK:
    case RD_REL_UACK:
      SET_STATE(RD,RD_NULL);
      grlc_data->tbf_ctrl[grlc_data->dl_index].vs_vr  = grlc_data->rd.vr;
      grlc_data->tbf_ctrl[grlc_data->dl_index].va_vq  = grlc_data->rd.vq;  
      grlc_trace_tbf_par ( grlc_data->dl_index );
      sig_rd_tm_end_of_tbf(FALSE);
      break;
    default:
      TRACE_ERROR( "T3192 unexpected" );
      break;
  }

} /* rd_t3192() */






