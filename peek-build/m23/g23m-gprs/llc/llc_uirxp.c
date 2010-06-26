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
|             the SDL-documentation (UIRX-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef LLC_UIRXP_C
#define LLC_UIRXP_C
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

#include "llc_uirxf.h"  /* to get queue management prototypes */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : uirx_ll_getunitdata_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LL_GETUNITDATA_REQ
|
| Parameters  : *ll_getunitdata_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void uirx_ll_getunitdata_req ( T_LL_GETUNITDATA_REQ *ll_getunitdata_req )
{
  T_LL_UNITDATA_IND *ll_unitdata_ind;

  TRACE_FUNCTION( "ll_getunitdata_req" );

  SWITCH_LLC (ll_getunitdata_req->sapi);

  TRACE_1_PARA("s:%d", ll_getunitdata_req->sapi );

  switch (GET_STATE(UIRX))
  {
    case UIRX_TLLI_UNASSIGNED_NOT_READY:
      PFREE (ll_getunitdata_req);
      SET_STATE (UIRX, UIRX_TLLI_UNASSIGNED_READY);
      break;

    case UIRX_ADM_NOT_READY:
      PFREE (ll_getunitdata_req);

      /*
       * First check, if already a frame is waiting.
       */
      if (uirx_queue_retrieve (&ll_unitdata_ind))
      {
        /*
         * A frame is waiting. Send primitive LL-UNITDATA-IND
         * to either SNDCP or GSMS, depending on the SAPI (GMM
         * uses now flow control!).
         */
        switch (ll_unitdata_ind->sapi)
        {
          case LL_SAPI_3:
          case LL_SAPI_5:
          case LL_SAPI_9:
          case LL_SAPI_11:
            TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                             BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                             ll_unitdata_ind->cipher);
            PSEND (hCommSNDCP, ll_unitdata_ind);
            break;

          case LL_SAPI_7:
#ifdef LL_2to1
            TRACE_PRIM_TO("MM");
            TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                             BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                             ll_unitdata_ind->cipher);
            PSEND (hCommMM, ll_unitdata_ind);
            break;
#else
            TRACE_PRIM_TO("GSMS");
            TRACE_3_OUT_PARA("s:%d len:%d c:%d", ll_unitdata_ind->sapi,
                                             BYTELEN(ll_unitdata_ind->sdu.l_buf),
                                             ll_unitdata_ind->cipher);
            PSEND (hCommGSMS, ll_unitdata_ind);
            break;
#endif

          default:
            PFREE (ll_unitdata_ind);
            TRACE_ERROR ("Invalid SAPI value");
            break;
        }
      }
      else
      {
        /*
         * No frame is waiting. Allow straight forwarding
         * of the next received frame.
         */
        SET_STATE (UIRX, UIRX_ADM_READY);
      }
      break;

    default:
      PFREE (ll_getunitdata_req);
      TRACE_ERROR( "LL_GETUNITDATA_REQ unexpected" );
      break;
  }

} /* uirx_ll_getunitdata_req() */
