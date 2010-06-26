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
|             the SDL-documentation (T201-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_ITXT_C
#define LLC_ITXT_C
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

#include "llc_itxf.h"   /* to get local function definitions */
#include "llc_llmes.h"  /* to get LLME signal definitions */
#include "llc_irxs.h"   /* to get ITX signal definitions */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : itx_T201_start
+------------------------------------------------------------------------------
| Description : This procedure starts timer T201 for the current SAPI. 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void itx_T201_start (T_TIME length)
{
  TRACE_FUNCTION( "itx_T201_start" );

  /*
   * Start timer T201 for the current SAPI.
   */
#ifdef _SIMULATION_
  /*
   * In simulation mode, use always a timer value of 5 seconds.
   */
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T201_BASE + IMAP(llc_data->current_sapi)), 5000);
#else
  vsi_t_start (VSI_CALLER
    (USHORT)(TIMER_T201_BASE + IMAP(llc_data->current_sapi)), length);
#endif /* _SIMULATION_ */

  return;
} /* itx_T201_start() */


/*
+------------------------------------------------------------------------------
| Function    : itx_T201_stop
+------------------------------------------------------------------------------
| Description : This procedure stops timer T201 for the current SAPI. 
|
| Parameters  : 
+------------------------------------------------------------------------------
*/
GLOBAL void itx_T201_stop (void)
{
  TRACE_FUNCTION( "itx_T201_stop" );

  /*
   * Stops timer T201 for the current SAPI.
   */
  vsi_t_stop (VSI_CALLER 
    (USHORT)(TIMER_T201_BASE + IMAP(llc_data->current_sapi)));

  return;
} /* itx_T201_stop() */



/*
+------------------------------------------------------------------------------
| Function    : itx_timer_t201
+------------------------------------------------------------------------------
| Description : Handles expiry of timer T201. This function is called from
|               pei_timeout().
|
| Parameters  : sapi - indicates the SAPI for which the timer has expired
|
+------------------------------------------------------------------------------
*/
GLOBAL void itx_timer_t201 (UBYTE sapi)
{ 
  TRACE_FUNCTION( "itx_timer_t201" );
  
  TRACE_0_INFO("T201 EXPIRED" );

  SWITCH_LLC (sapi);

  switch (GET_STATE(ITX))
  {
    case ITX_ABM:
      /*
       * If a frame is associated with T201, increment the 
       * retransmission counter and if below N200, restart 
       * T201. Else initate the ABM-Re-Establish procedure.
       */
      if ( llc_data->itx->t201_entry != NULL)
      {
        llc_data->itx->t201_entry->status = IQ_RETR_FRAME;

        if ( llc_data->itx->t201_entry->n_retr <= *(llc_data->n200) )
        {
          TIMERSTART (T201, llc_data->t200->length);
          itx_send_next_frame (ABIT_SET_REQ);
        }
        else
        {
          /*
           * Initate ABM Re-Establish procedure. The state change 
           * will be done by the reestablish procedure later.
           */
          sig_itx_llme_reest_ind (LLGMM_ERRCS_ACK_NO_PEER_RES_REEST);
        }
      }
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    case ITX_ABM_PEER_BUSY:
      /*
       * If the Peer busy retransmission counter is below N200
       * restart T201 and send an appropriated S frame. Else
       * initate the ABM-Re-Establish procedure.
       */
      if ( llc_data->itx->n_pb_retr < *(llc_data->n200) )
      {
        TIMERSTART (T201, llc_data->t200->length);
        sig_itx_irx_s_frame_req (ABIT_SET_REQ); 
        llc_data->itx->n_pb_retr += 1;
      }
      else
      {
        sig_itx_llme_reest_ind (LLGMM_ERRCS_PEER_BUSY_REEST);
      }
      /* SET_STATE (ITX, SAME_STATE); */
      break;

    default:
      TRACE_ERROR( "T201 unexpected" );
      break;
  }

} /* itx_timer_t201() */
