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
|  Purpose :  This modul is part of the entity LLC and implements all labels
|             in the SDL-documentation (U-statemachine) that are called from
|             more than one diagram/place.
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_UL_C
#define LLC_UL_C
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

#include "llc_uf.h"     /* to get local U functions */
#include "llc_ul.h"     /* to get local U labels */
#include "llc_llmes.h"  /* to get signal interface to LLME */
#include "llc_t200s.h"  /* to get signal interface to T200 */
#include "llc_txs.h"    /* to get signal interface to TX */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : u_handle_mX_zero_rsp
+------------------------------------------------------------------------------
| Description : This function handles the XID response in case we want to set
|               mX to zero and the peer does not. 
|
| Parameters  : mX_req  - requested value of mD or mU
|               n201_i  - ptr to current n201_i
|               mX      - ptr to current (=received) mD or current mU
|               kX      - ptr to current kD or current kU
|               kX_min  - minimum value of kD or kU
|               kX_max  - maximum value of kD or kU
|               kX_type - XID_KU or XID_KD
+------------------------------------------------------------------------------
*/
GLOBAL void u_handle_mX_zero_rsp (USHORT mX_req, USHORT* n201_i, 
                                  USHORT mX,     USHORT  mX_max,    
                                  UBYTE* kX,     UBYTE   kX_min, 
                                  UBYTE  kX_type )
{
  TRACE_FUNCTION( "u_handle_mX_zero_rsp" );

  /*
   * calc new N201-I and/or kX, if necessary
   */
#if XID_N201_I_MIN != 0u /* avoid division with 0 */
  if ( (mX_max == 0u) AND (mX_req != 0u) AND (*kX * mX > mX_req) )
  {
    /*
     * first try to reduce n201-I only, else reduce also k
     * Note: *kX can't be 0 here
     */
    if ( *kX * XID_N201_I_MIN <= mX_req * 16u )
    {
      /*
       * calc and tag N201-I
       */
      *n201_i = (mX_req * 16u) / *kX;
      
      llc_data->u->xid_tag |= (0x00000001L << XID_N201_I);

    }
    else if ( kX_min * XID_N201_I_MIN <= mX_req * 16u )
    {
      /*
       * calc and tag N201-I and kX 
       */
      *n201_i = XID_N201_I_MIN;
      *kX     = (mX_req * 16u) / XID_N201_I_MIN;

      llc_data->u->xid_tag |= (0x00000001L << XID_N201_I);
      llc_data->u->xid_tag |= (0x00000001L << kX_type);
    }
    else
    {
      TRACE_ERROR("Requested value of mX is too small");
    }
  }
#endif /* XID_N201_I_MIN != 0u */
} /* u_handle_mX_zero_rsp() */


/*
+------------------------------------------------------------------------------
| Function    : u_label_xid
+------------------------------------------------------------------------------
| Description : Describes label XID
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL_UNITDATA_IND
|                                 primitive
|               command         - contains command of primitive
|               cr_bit          - setting of C/R bit
|               pf_bit          - setting of P/F bit
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_xid (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                         T_COMMAND command,
                         T_BIT cr_bit,
                         T_BIT pf_bit)
{ 
  BOOL              reset_received;
  BOOL              xid_ind;

  
  TRACE_FUNCTION( "u_label_xid" );

  if (pf_bit EQ 1)
  {
    /*
     * Check XID parameters and store the decoded values in 
     * llc_data->decoded_xid.
     */
    if (u_check_xid (&ll_unitdata_ind->sdu, cr_bit, command))
    {
      /*
       * In case of reset, all incarnations in ABM are set to ADM 
       * by u_eval_xid()!
       */
      u_eval_xid (cr_bit, &reset_received, &xid_ind);

      if (llc_data->u->xid_pending)
      {
        /*
         * We have an XID command collision!
         */

        if ( (llc_data->decoded_l3_xid.valid EQ TRUE) AND 
             !(llc_data->u->xid_tag_sent & (0x00000001L << XID_LAYER_3)))
        {
          /*
           * Peer XID command contains layer-3 parameters, our XID command
           * doesn't.
           */
          llc_data->u->xid_pending = FALSE;

          sig_u_t200_stop_req();

          /*
           * Label XID_VALID
           */
          u_label_xid_valid (ll_unitdata_ind, reset_received, xid_ind);
        }
        else if (reset_received)
        {
          /*
           * An XID command including a valid reset shall not be ignored
           * and takes precedence over all collision issues
           */
          llc_data->u->xid_pending = FALSE;

          sig_u_t200_stop_req();

          /*
           * Label XID_VALID
           */
          u_label_xid_valid (ll_unitdata_ind, reset_received, xid_ind);
        }
        else
        {
          /*
           * Layer-3 XID parameters are present in both or neither XID 
           * commands.
           */
          PFREE (ll_unitdata_ind);
        }
      }
      else /* xid_pending == FALSE (no XID command sent) */
      {
        /*
         * Label XID_VALID
         */
        u_label_xid_valid (ll_unitdata_ind, reset_received, xid_ind);
      }
    }
    else /* u_check_xid() == FALSE (XID invalid) */
    {
      PFREE (ll_unitdata_ind);
      TRACE_0_INFO("invalid XID information field");
    }
  }
  else /* pf_bit == 0 (bit is not set) */
  {
    PFREE (ll_unitdata_ind);
  }

  return;
} /* u_label_xid() */


/*
+------------------------------------------------------------------------------
| Function    : u_label_xidres
+------------------------------------------------------------------------------
| Description : Describes label XIDRES
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL_UNITDATA_IND
|                                 primitive
|               command         - contains command of primitive
|               cr_bit          - setting of C/R bit
|               pf_bit          - setting of P/F bit
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_xidres (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                            T_COMMAND command,
                            T_BIT cr_bit,
                            T_BIT pf_bit)
{ 
  BOOL              reset_received;
  BOOL              xid_ind;
  BOOL              l3p_outstanding;

  
  TRACE_FUNCTION( "u_label_xidres" );

  if (pf_bit EQ 1)
  {
    /*
     * Check XID parameters and store the decoded
     * values in llc_decode_xid
     */
    if (u_check_xid (&ll_unitdata_ind->sdu, cr_bit, command))
    {
      l3p_outstanding = (llc_data->u->xid_tag_sent & (0x1L << XID_LAYER_3)) ? TRUE : FALSE;

      /*
       * An XID response frame should only be handled if the XID L3 
       * para are included or not same as requested. Else ignore it!
       */
      if (llc_data->decoded_l3_xid.valid == l3p_outstanding)
      {
        /*
         * If an XID parameter is not included in the response and
         * if it was included in the request, the requested value is
         * implicit accepted.
         */
        u_handle_optimization ();

        /*
         * Now check XID parameter
         */
        u_eval_xid (cr_bit, &reset_received, &xid_ind);

        /*
         * Label XIDRES_VALID
         */

        llc_data->u->xid_pending = FALSE;

        sig_u_t200_stop_req ();

        if (l3p_outstanding)
        {
          u_send_ll_xid_cnf ();
        }
        else
        {
          if (xid_ind == TRUE)
          {
            u_send_ll_xid_ind ();
          }
        }

        PFREE (ll_unitdata_ind);
      }
      else
      {
        /*
         * Ignore frame
         */
        PFREE (ll_unitdata_ind);
      }
    }
    else
    {
      /*
       * Request T200 to let the timer manually expire.
       */
      sig_u_t200_expire_req ();
      PFREE (ll_unitdata_ind);
    }
  }
  else /* bit is not set */
  {
    PFREE (ll_unitdata_ind);
  }
} /* u_label_xidres() */



/*
+------------------------------------------------------------------------------
| Function    : u_label_xid_sabm_send
+------------------------------------------------------------------------------
| Description : Describes label XID_SABM_SEND
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL_UNITDATA_IND
|                                 primitive
|               cr_bit          - setting of C/R bit
|               pf_bit          - setting of P/F bit
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_xid_sabm_send (T_LL_UNITDATA_IND *ll_unitdata_ind, 
                                   T_BIT cr_bit,
                                   T_BIT pf_bit)
{ 
  BOOL              reset_received;
  BOOL              xid_ind;

  
  TRACE_FUNCTION( "u_label_xid_sabm_send" );

  if (pf_bit EQ 1)
  {
    /*
     * Check XID parameters and store the decoded values in 
     * llc_data->decoded_xid.
     */
    if (u_check_xid (&ll_unitdata_ind->sdu, cr_bit, U_XID))
    {
      /*
       * We have received an valid XID. If a reset is included
       * this must be handled even in case of an collision
       */
      if (llc_data->decoded_xid.reset.valid)
      {
        /*
         * All incarnations in ABM are set to ADM by u_eval_xid()!
         */
        u_eval_xid (cr_bit, &reset_received, &xid_ind);

        /*
         * Label XID_VALID
         */
        u_label_xid_valid (ll_unitdata_ind, reset_received, xid_ind);
      }
      else
      {
        PFREE (ll_unitdata_ind);
      }
    }
    else /* u_check_xid() == FALSE (XID invalid) */
    {
      PFREE (ll_unitdata_ind);
      TRACE_0_INFO("invalid XID information field");
    }
  }
  else /* pf_bit == 0 (bit is not set) */
  {
    PFREE (ll_unitdata_ind);
  }

  return;
} /* u_label_xid_sabm_send() */




/*
+------------------------------------------------------------------------------
| Function    : u_label_xid_valid
+------------------------------------------------------------------------------
| Description : Describes label XID_VALID
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL_UNITDATA_IND
|                                 primitive
|               reset_received  - indicates if reset parameter has been 
|                                 received
|               xid_ind         - indicates if LL_XID_IND must be sent to
|                                 layer 3
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_xid_valid (T_LL_UNITDATA_IND *ll_unitdata_ind,
                               BOOL reset_received,
                               BOOL xid_ind)
{ 
  UBYTE  inc;
  
  TRACE_FUNCTION( "u_label_xid_valid" );

  if (reset_received)
  {
  	/* LLC informs GRLC to flush out LLC PDUs maintained in GRLC when XID reset
     * is received by LLC */
    PALLOC (grlc_flush_data_req, GRLC_FLUSH_DATA_REQ);
    PSEND (hCommGRLC, grlc_flush_data_req);

    /*
     * First send reset indication, then handle it.
     */
    {
    PALLOC (ll_reset_ind, LL_RESET_IND);
    ll_reset_ind->sapi = llc_data->current_sapi;
    TRACE_PRIM_TO("SND");
    PSEND (hCommSNDCP, ll_reset_ind);
    }
    /*
     * V(U) and V(UR) are set to 0. All services that are 
     * affected from ABM -> ADM are also reset.
     */
    sig_u_llme_reset_ind();
    /*
     * Check if the config primitive "DELAY" was received and wait 
     * for certain number of milliseconds
     */
    if (llc_data->millis > 0) {
      TRACE_1_OUT_PARA("Sleeping for :%d milliseconds", llc_data->millis);
      vsi_t_sleep(VSI_CALLER llc_data->millis); 
    }
    /* 
     * After receiving a reset all running L3 requests should 
     * be ignored and no confirmation has to be send.
     */
    for (inc = 0; inc < U_NUM_INC; inc++)
    {
      SWITCH_SERVICE (llc, u, inc);

      llc_data->u->release_requested   = FALSE;
      llc_data->u->ll_xid_resp_pending = FALSE;
    }

    /*
     * Switch back to original SAPI
     */
    SWITCH_LLC (ll_unitdata_ind->sapi);
  }

  u_tag_xid_parameters (MS_RESPONSE, FALSE);

  /*
   * Check if we have to send an LL_XID_IND
   */
  if (llc_data->u->xid_tag & (0x00000001L << XID_N201_U) AND llc_data->u->requested_xid.n201_u.valid)
  {
     xid_ind = TRUE;
  }
  if (llc_data->u->xid_tag & (0x00000001L << XID_N201_I) AND llc_data->u->requested_xid.n201_i.valid)
  {
     xid_ind = TRUE;
  }

  /*
   * If no Layer 3 XID parameters are included we don't have to wait 
   * for an LL_XID_RES
   */
  if (llc_data->decoded_l3_xid.valid NEQ TRUE)
  {
    /*
     * Label S_XID_RES
     */
    u_send_xid (MS_RESPONSE);
  }

  if (xid_ind)
  {
    u_send_ll_xid_ind ();
  }

  if (reset_received)
  {

    /*
     * V(U) and V(UR) are set to 0. All services that are 
     * affected from ABM -> ADM are also reset.
     */
    sig_u_llme_ready_ind();
    
    /*
     * Switch back to original SAPI
     */
    SWITCH_LLC (ll_unitdata_ind->sapi);
  }



  PFREE (ll_unitdata_ind);

  return;
} /* u_label_xid_valid() */


/*
+------------------------------------------------------------------------------
| Function    : u_label_frmr_cond
+------------------------------------------------------------------------------
| Description : Describes label FRMR_COND
|
| Parameters  : ll_unitdata_ind - a valid pointer to the frame that caused the
|                                 frame rejection condition
|               cr_bit          - C/R bit setting of frame
|               frmr_reason     - reason of frame rejection condition
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_frmr_cond (T_LL_UNITDATA_IND *ll_unitdata_ind,
                               T_BIT cr_bit,
                               UBYTE frmr_reason)
{
  TRACE_FUNCTION ( "u_label_frmr_cond" );

  u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_COND);

  u_send_frmr (ll_unitdata_ind, U_FRAME, FRMR_CTRL_LENGTH_UNKNOWN, 
    llc_data->sapi->vs, llc_data->sapi->vr,
    cr_bit, frmr_reason);

  PFREE (ll_unitdata_ind);

  return;
} /* u_label_frmr_cond() */


/*
+------------------------------------------------------------------------------
| Function    : u_label_frmr_cond_reest
+------------------------------------------------------------------------------
| Description : Describes label FRMR_COND_REEST
|
| Parameters  : ll_unitdata_ind - a valid pointer to the frame that caused the
|                                 frame rejection condition
|               cr_bit          - C/R bit setting of frame
|               frmr_reason     - reason of frame rejection condition
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_frmr_cond_reest (T_LL_UNITDATA_IND *ll_unitdata_ind,
                                     T_BIT cr_bit,
                                     UBYTE frmr_reason)
{
  TRACE_FUNCTION ( "u_label_frmr_cond_reest" );

  u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_COND_REEST);

  SET_STATE (U, U_LOCAL_ESTABLISHMENT);

  sig_u_llme_abmrel_ind();

  u_send_frmr (ll_unitdata_ind, U_FRAME, FRMR_CTRL_LENGTH_UNKNOWN, 
    llc_data->sapi->vs, llc_data->sapi->vr,
    cr_bit, frmr_reason);

  PFREE (ll_unitdata_ind);

  llc_data->u->retransmission_counter = 0;

  /*
   * Send LL_ESTABLISH_IND after successful establishment.
   */
  llc_data->u->ind_cnf_establishment = IND_ESTABLISHMENT;

  /*
   * Re-establishment procedure: send GRLC_DATA_REQ (SABM), start T200.
   */
  u_send_sabm();

  return;
} /* u_label_frmr_cond_reest() */


/*
+------------------------------------------------------------------------------
| Function    : u_label_s_est_ind
+------------------------------------------------------------------------------
| Description : Describes label S_EST_IND
|
| Parameters  : pf_bit  - setting of P/F bit
|
+------------------------------------------------------------------------------
*/
GLOBAL void u_label_s_est_ind (T_BIT pf_bit)
{
  TRACE_FUNCTION ( "u_label_s_est_ind" );

  SET_STATE (U, U_REMOTE_ESTABLISHMENT);

  u_send_ll_establish_ind ();

  sig_u_llme_abmrel_ind();

  return;
} /* u_label_s_est_ind() */


