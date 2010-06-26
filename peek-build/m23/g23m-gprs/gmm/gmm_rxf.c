/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rxf.c
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
|  Purpose :  This modul is part of the entity GMM and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RXF_C
#define GMM_RXF_C
#endif

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include "typedefs.h"    /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "gmm.h"        /* to get the global entity definitions */
#include "gmm_txs.h"    /* to get some signals */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : rxgmm_init
+------------------------------------------------------------------------------
| Description : The function rxgmm_init() .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/
GLOBAL void rxgmm_init ( void )
{ 
  TRACE_FUNCTION( "rxgmm_init" );

  /*
   * Initialise service RX with state RX_READY.
   */
  INIT_STATE ( RX, RX_READY );
  
} /* rxgmm_init() */

/*
+------------------------------------------------------------------------------
| Function    : rx_gmm_status
+------------------------------------------------------------------------------
| Description : The function kern_gmm_status() .... 
|
| Parameters  : cause - error cause
|
+------------------------------------------------------------------------------
*/
GLOBAL void rx_gmm_status ( UBYTE cause )
{ 
  GMM_TRACE_FUNCTION( "rx_gmm_status" );
  {
    MCAST (gmm_status, GMM_STATUS);
     gmm_status->gmm_cause.cause_value = cause;
     vsi_o_ttrace(VSI_CALLER TC_EVENT,"Info: cause: %i", cause);
    
    sig_rx_tx_data_req (CURRENT_TLLI, GMM_STATUS);
  }
  GMM_RETURN;
} /* rx_gmm_status() */


