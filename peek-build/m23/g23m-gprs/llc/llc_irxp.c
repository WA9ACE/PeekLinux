/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             functions to handles the incoming primitives as described in 
|             the SDL-documentation (IRX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_IRXP_C
#define LLC_IRXP_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_irxf.h"   /* to get IRX helper functions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : irx_ll_getdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_GETDATA_REQ
|
| Parameters  : *ll_getdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void irx_ll_getdata_req ( T_LL_GETDATA_REQ *ll_getdata_req )
{ 
  BOOL               frame_found;
  T_LL_UNITDATA_IND *ll_unitdata_ind;

  TRACE_FUNCTION( "ll_getdata_req" );

  SWITCH_LLC (ll_getdata_req->sapi);
 
  TRACE_1_PARA("s:%d", ll_getdata_req->sapi);

  switch (GET_STATE(IRX))
  {
    case IRX_TLLI_UNASSIGNED:
      /*
       * Ignore primitive.
       */
      PFREE (ll_getdata_req);
      /* SET_STATE (IRX, SAME_STATE); */
      break;

    case IRX_TLLI_ASSIGNED:
      /*
       * Store L3 ready flag
       */
      llc_data->irx->ll_send_ready = TRUE;
      PFREE (ll_getdata_req);
      /* SET_STATE (IRX, SAME_STATE); */
      break;

    case IRX_ABM:
      /*
       * Try to retrieve a frame from the queue and
       * forward it to L3, else store L3 ready flag.
       */
      irx_queue_retrieve (&ll_unitdata_ind, &frame_found);
      if (frame_found)
      {
        PPASS (ll_unitdata_ind, ll_data_ind, LL_DATA_IND);
        TRACE_2_OUT_PARA("s:%d len:%d", ll_data_ind->sapi, BYTELEN(ll_data_ind->sdu.l_buf));
        PSEND (hCommSNDCP, ll_data_ind);
        
        llc_data->irx->ll_send_ready = FALSE;

        /* SET_STATE (IRX, SAME_STATE); */
      }
      else
      {
        llc_data->irx->ll_send_ready = TRUE;

        /* SET_STATE (IRX, SAME_STATE); */
      }
      PFREE (ll_getdata_req);
      break;

    case IRX_ABM_BUSY:
      /*
       * Try to retrieve a frame from the queue and
       * forward it to L3, else store L3 ready flag.
       */
      irx_queue_retrieve (&ll_unitdata_ind, &frame_found);
      if (frame_found)
      {
        PPASS (ll_unitdata_ind, ll_data_ind, LL_DATA_IND);
        TRACE_2_OUT_PARA("s:%d len:%d", ll_data_ind->sapi, BYTELEN(ll_data_ind->sdu.l_buf));
        PSEND (hCommSNDCP, ll_data_ind);

        llc_data->irx->ll_send_ready = FALSE;

        /*
         * Indicate the clearance of the 'own received 
         * busy' condition to the peer entity
         */
        irx_send_ack (ABIT_SET_REQ);

        SET_STATE (IRX, IRX_ABM);
      }
      else
      {
        llc_data->irx->ll_send_ready = TRUE;

        /* SET_STATE (IRX, SAME_STATE); */
      }
      PFREE (ll_getdata_req);
      break;

    default:
      PFREE (ll_getdata_req);
      TRACE_ERROR( "LL_GETDATA_REQ unexpected" );
      break;
  }

} /* irx_ll_getdata_req() */
