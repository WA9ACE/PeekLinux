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
|             the SDL-documentation (LLME-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_LLMEP_C
#define LLC_LLMEP_C
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

#include "llc_f.h"      /* to get global functions, e.g. llc_init_parameters */
#include "llc_llmef.h"  /* to get local LLME functions */
#include "llc_us.h"     /* to get signal interface to U */
#include "llc_itxs.h"   /* to get signal interface to ITX */
#include "llc_irxs.h"   /* to get signal interface to IRX */
#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_uirxs.h"  /* to get signal interface to UIRX */
#include "llc_t200s.h"  /* to get signal interface to T200 */
#include "llc_txs.h"    /* to get signal interface to TX */
#include "llc_rxs.h"    /* to get signal interface to RX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : llme_llgmm_assign_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_ASSIGN_REQ
|
| Parameters  : *llgmm_assign_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void llme_llgmm_assign_req ( T_LLGMM_ASSIGN_REQ *llgmm_assign_req )
{
  T_SAPI sapi;


  TRACE_FUNCTION( "llme_llgmm_assign_req" );
  
  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_UNASSIGNED:
      /*
       * ATTENTION: Every ASSIGN_REQ primitive must contain a valid new_tlli
       * when LLC is unassigned, otherwise it should be a protocol error...?
       * Therefore this check could be omitted.
       */
#ifdef LL_2to1
      if (llgmm_assign_req->new_tlli == PS_TLLI_INVALID)
#else
      if (llgmm_assign_req->new_tlli == LLGMM_TLLI_INVALID)
#endif
      {
        PFREE (llgmm_assign_req);
        TRACE_ERROR ("LLME in state TLLI_UNASSIGNED and new_tlli is invalid!");
      }
      else /* LLGMM_TLLI_VALID */
      {
        TRACE_1_INFO("TLLI 0x%.8X assigned", llgmm_assign_req->new_tlli);

        llc_data->tlli_new = llgmm_assign_req->new_tlli;
        llc_data->tlli_old = llgmm_assign_req->old_tlli;

        llme_init_cipher (llgmm_assign_req->llgmm_kc, 
          llgmm_assign_req->ciphering_algorithm);

        PFREE (llgmm_assign_req);

        /*
         * Initialises the LLC layer parameters.
         * NOTE: IOV-UI and IOV-I[] are initialised the second time with this
         * function call because they were already set in llme_init_cipher().
         */
        llc_init_parameters();

        /*
         * Label ASSIGN
         */

        SET_STATE (LLME, LLME_TLLI_ASSIGNED);

        /*
         * ATTENTION:
         * The following loop implies that all SAPIs are odd numbers beginning
         * with 1!
         */
#ifdef LL_2to1
        for (sapi = PS_SAPI_1; sapi <= PS_SAPI_11; sapi += 2)
#else
        for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
#endif
        {
          SWITCH_LLC (sapi);

          sig_llme_u_assign_req();
          sig_llme_uitx_assign_req();
          sig_llme_uirx_assign_req();
          sig_llme_t200_assign_req();

#ifdef LL_2to1
          if ((sapi != PS_SAPI_1) && (sapi != PS_SAPI_7))
#else
          if ((sapi != LL_SAPI_1) && (sapi != LL_SAPI_7))
#endif
          {
            sig_llme_itx_assign_req();
            sig_llme_irx_assign_req();
          }
        }

        sig_llme_rx_assign_req();
        sig_llme_tx_assign_req();

        llc_init_requested_xid();
      }
      break;

    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /* 
       * No break
       */
    case LLME_TLLI_ASSIGNED:
      llc_data->tlli_new = llgmm_assign_req->new_tlli;
      llc_data->tlli_old = llgmm_assign_req->old_tlli;

      llme_init_cipher (llgmm_assign_req->llgmm_kc,
                        llgmm_assign_req->ciphering_algorithm);

#ifdef LL_2to1
      if (llgmm_assign_req->new_tlli EQ PS_TLLI_INVALID)
#else
      if (llgmm_assign_req->new_tlli EQ LLGMM_TLLI_INVALID)
#endif
      {
        TRACE_0_INFO("TLLI unassigned");

        /*
         * Label UNASSIGN
         */

        SET_STATE (LLME, LLME_TLLI_UNASSIGNED);

        /*
         * ATTENTION:
         * The following loop implies that all SAPIs are odd numbers beginning
         * with 1!
         */
#ifdef LL_2to1
        for (sapi = PS_SAPI_1; sapi <= PS_SAPI_11; sapi += 2)
#else
        for (sapi = LL_SAPI_1; sapi <= LL_SAPI_11; sapi += 2)
#endif
        {
          SWITCH_LLC (sapi);

          sig_llme_u_unassign_req();
          sig_llme_uitx_unassign_req();
          sig_llme_uirx_unassign_req();
          sig_llme_t200_unassign_req();

          if ((sapi != LL_SAPI_1) && (sapi != LL_SAPI_7))
          {
            sig_llme_itx_unassign_req();
            sig_llme_irx_unassign_req();
          }
        }

        sig_llme_rx_unassign_req();
        sig_llme_tx_unassign_req();

        /*
         * Clear suspended mode with unassign.
         */
        llc_data->suspended = FALSE;

        /*
         * In case we have suspended GRLC we have to resume GRLC to 
         * 'normal operation'
         */
        if (llc_data->grlc_suspended == TRUE)
        {
          /*
           * Resume TX operation and indicate, that GRLC was suspened.
           */
          sig_llme_tx_resume_req (llc_data->grlc_suspended);

          llc_data->grlc_suspended = FALSE;

        /*  {
            PALLOC (grr_resume_req, GRR_RESUME_REQ);
            PSEND (hCommGRR, grr_resume_req);
          }*/
        }

        PFREE (llgmm_assign_req);
      }
      else /* LLGMM_TLLI_VALID */
      {
        TRACE_1_INFO("TLLI 0x%.8X (re-)assigned", llgmm_assign_req->new_tlli);
        PFREE (llgmm_assign_req);
      }
      break;

    default:
      PFREE (llgmm_assign_req);
      TRACE_ERROR( "LLGMM_ASSIGN_REQ unexpected" );
      break;
  }

} /* llme_llgmm_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : llme_llgmm_trigger_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_TRIGGER_REQ
|
| Parameters  : *llgmm_trigger_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void llme_llgmm_trigger_req ( T_LLGMM_TRIGGER_REQ *llgmm_trigger_req )
{ 
  TRACE_FUNCTION( "llme_llgmm_trigger_req" );
  
  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      /* No break. */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /*
       * Switch LLC to SAPI 1 in case TX forwards the trigger request to UITX.
       */
      SWITCH_LLC (LL_SAPI_1);

      sig_llme_tx_trigger_req (llgmm_trigger_req->trigger_cause);

      PFREE (llgmm_trigger_req);
      break;
    default:
      PFREE (llgmm_trigger_req);
      TRACE_ERROR( "LLGMM_TRIGGER_REQ unexpected" );
      break;
  }

} /* llme_llgmm_trigger_req() */



/*
+------------------------------------------------------------------------------
| Function    : llme_llgmm_suspend_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_SUSPEND_REQ
|
| Parameters  : *llgmm_suspend_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void llme_llgmm_suspend_req ( T_LLGMM_SUSPEND_REQ *llgmm_suspend_req )
{ 
  T_SAPI sapi;

  
  TRACE_FUNCTION( "llme_llgmm_suspend_req" );

  TRACE_1_PARA("cause:%d", llgmm_suspend_req->susp_cause );
  
  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED:
      TRACE_0_INFO("LLC suspended");

      /*
       * Set global suspended indicator for the other services.
       */
      llc_data->suspended = TRUE;

      SET_STATE (LLME, LLME_TLLI_ASSIGNED_SUSPENDED);

      /*
       * ATTENTION:
       * The following loop implies that all the relevant SAPIs are the odd 
       * numbers from 3 to 11 (except 7)
       */
      for (sapi = LL_SAPI_3; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_LLC (sapi);

        if (sapi != LL_SAPI_7)
        {
          sig_llme_itx_suspend_req();
        }
      }

      /*
       * No break!
       */
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      /*
       * In case of RAU suspend GRLC if not already done
       */
      if ( (llc_data->grlc_suspended == FALSE)           AND 
           (llgmm_suspend_req->susp_cause == LLGMM_RAU)   )
      {
        sig_llme_tx_suspend_req ();

        llc_data->grlc_suspended = TRUE;

        {
          PALLOC (grlc_activate_gmm_queue_req, GRLC_ACTIVATE_GMM_QUEUE_REQ);
          PSEND (hCommGRLC, grlc_activate_gmm_queue_req);
        }
      }

      break;
  
    default:
      TRACE_ERROR( "LLGMM_SUSPEND_REQ unexpected" );
      break;
  
  }
  if ( (llgmm_suspend_req->susp_cause == LLGMM_LIMITED) OR
           (llgmm_suspend_req->susp_cause == LLGMM_NO_GPRS_SERVICE))
      /* init current values */
#ifdef REL99
      llc_data->cur_pfi          = LL_PFI_SIGNALING; 
#endif
      llc_data->cur_qos.delay    = LL_DELAY_SUB;
      llc_data->cur_qos.relclass = LL_NO_REL;
      llc_data->cur_qos.peak     = LL_PEAK_SUB;
      llc_data->cur_qos.preced   = LL_PRECED_SUB;
      llc_data->cur_qos.mean     = LL_MEAN_SUB;
      llc_data->cur_radio_prio   = LL_RADIO_PRIO_1;  
  PFREE (llgmm_suspend_req);

} /* llme_llgmm_suspend_req() */



/*
+------------------------------------------------------------------------------
| Function    : llme_llgmm_resume_req
+------------------------------------------------------------------------------
| Description : Handles the primitive LLGMM_RESUME_REQ
|
| Parameters  : *llgmm_resume_req - Ptr to primitive payload
|
+------------------------------------------------------------------------------
*/
GLOBAL void llme_llgmm_resume_req ( T_LLGMM_RESUME_REQ *llgmm_resume_req )
{ 
  T_SAPI sapi;

  
  TRACE_FUNCTION( "llme_llgmm_resume_req" );
  
  switch( GET_STATE( LLME ) )
  {
    case LLME_TLLI_ASSIGNED_SUSPENDED:
      TRACE_0_INFO("LLC resumed");

      /*
       * Clear global suspended indicator for the other services.
       */
      llc_data->suspended = FALSE;

      SET_STATE (LLME, LLME_TLLI_ASSIGNED);

      /*
       * FIRST! indicate to TX that we resume normal operation and 
       * that LLC/GRLC flow control should be reseted in case GRLC was 
       * suspened.
       */
      sig_llme_tx_resume_req (llc_data->grlc_suspended);

      /*
       * Reset GRLC, if suspended, to 'normal operation'
       */
      if (llc_data->grlc_suspended == TRUE)
      {
       /* PALLOC (grr_resume_req, GRR_RESUME_REQ);
        PSEND (hCommGRR, grr_resume_req);*/

        llc_data->grlc_suspended = FALSE;
      }

      /*
       * ATTENTION:
       * The following loop implies that all the relevant SAPIs are the odd 
       * numbers from 3 to 11 (except 7)
       */
      for (sapi = LL_SAPI_3; sapi <= LL_SAPI_11; sapi += 2)
      {
        SWITCH_LLC (sapi);

        if (sapi != LL_SAPI_7)
        {
          sig_llme_itx_resume_req();
        }
      }
      break;
  
    default:
      TRACE_ERROR( "LLGMM_RESUME_REQ unexpected" );
      break;
  }

  PFREE (llgmm_resume_req);

} /* llme_llgmm_resume_req() */

