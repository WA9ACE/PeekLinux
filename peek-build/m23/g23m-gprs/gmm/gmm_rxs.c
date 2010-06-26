/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_rxs.c
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
|             functions to handles the incoming process internal signals as  
|             described in the SDL-documentation (RX-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_RXS_C
#define GMM_RXS_C
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
#include "gmm_rxf.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

GLOBAL void sig_kern_rx_gmm_status (UBYTE error_cause)
{
  GMM_TRACE_FUNCTION( "sig_kern_rx_gmm_status" );
  rx_gmm_status(error_cause);
  GMM_RETURN;
}


