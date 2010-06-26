/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_syncs.c
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
|             described in the SDL-documentation (SYNC-statemachine)
+----------------------------------------------------------------------------- 
*/ 


#ifndef GMM_SYNCS_C
#define GMM_SYNCS_C
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
#include "gmm_syncs.h"

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_kern_sync_set_mm_state ( mm_state );
+------------------------------------------------------------------------------
| Description : Handles the internal signal sig_kern_sync_set_mm_state
|
| Parameters  : mm_state -  state of MM
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_kern_sync_set_mm_state ( U8 mm_status )
{ 
  gmm_data->sync.mm_cell_info.status = mm_status;
} /* sig_kern_rdy_force_ie_req() */

