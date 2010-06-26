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
|             functions to handles the incoming process internal signals as
|             described in the SDL-documentation (LLME-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef LLC_LLMES_C
#define LLC_LLMES_C
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
#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_uirxs.h"  /* to get signal interface to UIRX */
#include "llc_itxs.h"   /* to get signal interface to ITX */
#include "llc_irxs.h"   /* to get signal interface to IRX */
#include "llc_txs.h"    /* to get signal interface to TX */
#include "llc_uf.h"     /* to get signal interface to U */
#include "llc_us.h"     /* to get signal interface to U */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_u_llme_reset_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_LLME_RESET_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_llme_reset_ind (void)
{
  T_SAPI sapi;

  TRACE_ISIG( "sig_u_llme_reset_ind" );

  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /*
       * ATTENTION:
       * The following loop implies that all SAPIs are odd numbers beginning
       * with 1!
       */
      for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_LLC (sapi);

        sig_llme_uitx_reset_req();
        sig_llme_uirx_reset_req();

        if ((sapi NEQ LL_SAPI_1) AND (sapi NEQ LL_SAPI_7))
        {
          sig_llme_itx_reset_req();
          sig_llme_irx_reset_req();
        }

        sig_llme_tx_reset_req();
      }
      break;
    default:
      TRACE_ERROR( "SIG_U_LLME_RESET_IND unexpected" );
      break;
  }
} /* sig_u_llme_reset_ind() */

/*
+------------------------------------------------------------------------------
| Function    : sig_u_llme_ready_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_LLME_READY_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_llme_ready_ind (void)
{
  T_SAPI sapi;

  TRACE_ISIG( "sig_u_llme_ready_ind" );

  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /*
       * ATTENTION:
       * The following loop implies that all SAPIs are odd numbers beginning
       * with 1!
       */
      for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_LLC (sapi);

        sig_llme_tx_ready_req();
      }

      break;
    default:
      TRACE_ERROR( "SIG_U_LLME_READY_IND unexpected" );
      break;
  }
} /* sig_u_llme_ready_ind() */



/*
+------------------------------------------------------------------------------
| Function    : sig_itx_llme_reest_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_ITX_LLME_REEST_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
#ifdef CC_CONCEPT
GLOBAL void sig_itx_llme_reest_ind (USHORT error_cause)
#else
GLOBAL void sig_itx_llme_reest_ind (UBYTE error_cause)
#endif
{
  TRACE_ISIG( "sig_itx_llme_reest_ind" );

  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /*
       * First stop ABM operation
       */
      sig_llme_irx_abmrel_req();
      sig_llme_itx_abmrel_req();
      sig_llme_tx_flush_req(SERVICE_ITX);

      /*
       * Then initiate connection re-establishment
       */
      {
        PALLOC (llgmm_status_ind, LLGMM_STATUS_IND);/*T_LLGMM_STATUS_IND*/

#ifdef LL_2to1
        llgmm_status_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
        llgmm_status_ind->ps_cause.value.llc_cause = error_cause;
#else
        llgmm_status_ind->error_cause = error_cause;
#endif

        PSEND (hCommGMM, llgmm_status_ind);
      }

      TRACE_1_INFO("Init re-establishment s:%d", llc_data->current_sapi);
      sig_llme_u_reest_req();
      break;

    default:
      TRACE_ERROR( "SIG_ITX_LLME_REEST_IND unexpected" );
      break;
  }
} /* sig_itx_llme_reest_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_u_llme_abmest_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_LLME_ABMEST_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_llme_abmest_ind (void)
{
  TRACE_ISIG( "sig_u_llme_abmest_ind" );

  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      sig_llme_tx_flush_req (SERVICE_ITX);
      sig_llme_irx_abmest_req();
      sig_llme_itx_abmest_req();
      break;

    default:
      TRACE_ERROR( "SIG_U_LLME_ABMEST_IND unexpected" );
      break;
  }
} /* sig_u_llme_abmest_ind() */




/*
+------------------------------------------------------------------------------
| Function    : sig_u_llme_abmrel_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_U_LLME_ABMREL_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_u_llme_abmrel_ind (void)
{
  TRACE_ISIG( "sig_u_llme_abmrel_ind" );

  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      sig_llme_irx_abmrel_req();
      sig_llme_itx_abmrel_req();
      sig_llme_tx_flush_req(SERVICE_ITX);
      break;

    default:
      TRACE_ERROR( "SIG_U_LLME_ABMREL_IND unexpected" );
      break;
  }
} /* sig_u_llme_abmrel_ind() */


