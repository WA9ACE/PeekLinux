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
|             described in the SDL-documentation (U-statemachine)
+-----------------------------------------------------------------------------
*/

#ifndef LLC_US_C
#define LLC_US_C
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
#include "llc_f.h"

#include "llc_uf.h"     /* to get local U functions */
#include "llc_ul.h"     /* to get local U labels */
#include "llc_t200s.h"  /* to get signal interface to T200 */
#include "llc_txs.h"    /* to get signal interface to TX */
#include "llc_llmes.h"  /* to get signal interface to LLME */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_u_assign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_U_ASSIGN_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_u_assign_req (void)
{
  TRACE_ISIG( "sig_llme_u_assign_req" );

  switch (GET_STATE(U))
  {
    case U_TLLI_UNASSIGNED:
      u_init_sapi();
      SET_STATE (U, U_ADM);
      break;
    default:
      TRACE_ERROR( "SIG_LLME_U_ASSIGN_REQ unexpected" );
      break;
  }
} /* sig_llme_u_assign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_llme_u_unassign_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_U_UNASSIGN_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_u_unassign_req (void)
{
  TRACE_ISIG( "sig_llme_u_unassign_req" );

  switch (GET_STATE(U))
  {
    case U_TLLI_UNASSIGNED:
      TRACE_ERROR( "SIG_LLME_U_UNASSIGN_REQ unexpected" );
      break;

    default:
      sig_u_t200_stop_req();
      SET_STATE (U, U_TLLI_UNASSIGNED);
      break;
  }
} /* sig_llme_u_unassign_req() */



/*
+------------------------------------------------------------------------------
| Function    : sig_rx_u_data_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_U_DATA_IND
|
| Parameters  : ll_unitdata_ind - a valid pointer to an LL-UNITDATA-IND
|                                 primitive
|               command - contains the command within the primitive
|               cr_bit - setting of the C/R bit
|               pf_bit - setting of the P/F bit
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_u_data_ind (T_LL_UNITDATA_IND *ll_unitdata_ind,
                               T_COMMAND command,
                               T_BIT cr_bit,
                               T_BIT pf_bit)
{
  BOOL              reset_received;
  BOOL              xid_ind;
  BOOL              rc_xid_valid;
  BOOL              l3p_outstanding;


  TRACE_ISIG( "sig_rx_u_data_ind" );

  /*
   * Set TLLI for current transaction.
   */
  llc_data->u->current_tlli = ll_unitdata_ind->tlli;

  switch (GET_STATE(U))
  {
    case U_ADM:
      if (cr_bit EQ SGSN_COMMAND)
      {
        /*
         * Label ADM_COMMAND
         */
        switch (command)
        {
          case U_SABM: /* COMMAND ADM */
            /*
             * Label ADM_SABM
             */
            if (pf_bit EQ 1)
            {
              if ( (llc_data->current_sapi EQ LL_SAPI_1) ||
                   (llc_data->current_sapi EQ LL_SAPI_7)  )
              {
                /*
                 * ABM not possible on SAPI 1 and 7
                 */
                PFREE (ll_unitdata_ind);

                u_send_dm (pf_bit);
              }
              else /* current_sapi is one of 3, 5, 9, or 11 */
              {
                rc_xid_valid = u_check_xid (&(ll_unitdata_ind->sdu),
                  cr_bit, command);

                PFREE (ll_unitdata_ind);

                if (rc_xid_valid)
                {
                  /*
                   * No check needed, reset_received cannot be TRUE
                   * (<R.LLC.XID_INVA.A.003>) and xid_ind is meaningless
                   * (LL_ESTABLISH_IND contains N201-U/I).
                   */
                  u_eval_xid (cr_bit, &reset_received, &xid_ind);

                  /*
                   * Check XID/SABM collision issue - XID command shall
                   * be treated as not transmitted.
                   */
                  if (llc_data->u->xid_pending)
                  {
                    llc_data->u->xid_pending = FALSE;

                    sig_u_t200_stop_req();
                  }

                  /*
                   * Label S_EST_IND
                   */
                  u_label_s_est_ind (pf_bit);
                }
              }
            }
            else /* pf_bit is not set */
            {
              PFREE (ll_unitdata_ind);
            }
            break;

          case U_XID: /* COMMAND  ADM */
            /*
             * Label XID
             */
            u_label_xid (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          case U_DISC: /* COMMAND ADM */
            PFREE (ll_unitdata_ind);
            if (pf_bit EQ 1)
            {
              u_send_dm (pf_bit);
            }
            break;

          default:
            if (pf_bit EQ 1)
            {
              llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;
              /*
               * Label FRMR_COND
               */
              u_label_frmr_cond (ll_unitdata_ind, cr_bit,
                llc_data->u->frmr_reason);
            }
            else
            {
              PFREE (ll_unitdata_ind);
            }
            break;
        }
      }
      else /* SGSN_RESPONSE */
      {
        /*
         * Label ADM_RESPONSE
         */
        switch (command)
        {
          case U_UA: /* RESPONSE ADM */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_MULT_ASS_TLLI);
            break;

          case U_DM: /* RESPONSE ADM */
            PFREE (ll_unitdata_ind);
            break;

          case U_XID: /* RESPONSE ADM */
           /*
            * Label XIDRES
            */
            u_label_xidres (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          case U_FRMR: /* RESPONSE ADM */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_RECEIVED);
            break;

          default:
            llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;
            /*
             * Label FRMR_COND
             */
            u_label_frmr_cond (ll_unitdata_ind, cr_bit,
              llc_data->u->frmr_reason);
            break;
        }
      }
      break; /* U_ADM */

    case U_LOCAL_ESTABLISHMENT:
      if (cr_bit EQ SGSN_COMMAND)
      {
        /*
         * Label LEST_COMMAND
         */
        switch (command)
        {
          case U_SABM: /* COMMAND LOCAL_ESTABLISHMENT */
            /*
             * Label LEST_SABM
             */

            if (pf_bit EQ 1)
            {
              rc_xid_valid = u_check_xid (&ll_unitdata_ind->sdu, cr_bit,
                command);

              PFREE (ll_unitdata_ind);

              if (rc_xid_valid)
              {
                if ((llc_data->decoded_l3_xid.valid EQ TRUE) AND
                  !(llc_data->u->xid_tag_sent & (0x00000001L << XID_LAYER_3)))
                {
                  /*
                   * No check needed, reset_received cannot be TRUE
                   * (<R.LLC.XID_INVA.A.003>) and xid_ind is meaningless
                   * (LL_ESTABLISH_IND contains N201-U/I).
                   */
                  u_eval_xid (cr_bit, &reset_received, &xid_ind);

                  /*
                   * Label S_EST_IND
                   */
                  u_label_s_est_ind (pf_bit);
                }
              }
            }
            else /* pf_bit is not set */
            {
              PFREE (ll_unitdata_ind);
            }
            break;

          case U_DISC: /* COMMAND LOCAL_ESTABLISHMENT */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              PALLOC (ll_release_ind, LL_RELEASE_IND);

              ll_release_ind->sapi  = llc_data->current_sapi;
#ifdef LL_2to1
              ll_release_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
              ll_release_ind->ps_cause.value.llc_cause = LL_RELCS_NORMAL;
#else
              ll_release_ind->cause = LL_RELCS_NORMAL;
#endif

              TRACE_1_OUT_PARA("s:%d", ll_release_ind->sapi);
              PSEND (hCommSNDCP, ll_release_ind);

              u_send_dm (1);

              SET_STATE (U, U_ADM);
            }
            break;

          case U_XID: /* COMMAND  LOCAL_ESTABLISHMENT */
            /*
             * Label XID_SABM_SEND
             */
            u_label_xid_sabm_send (ll_unitdata_ind, cr_bit, pf_bit);
            break;

          default:
            if (pf_bit EQ 1)
            {
              llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;

              /*
               * Label FRMR_COND
               */
              u_label_frmr_cond (ll_unitdata_ind, cr_bit,
                llc_data->u->frmr_reason);
            }
            else
            {
              PFREE (ll_unitdata_ind);
            }
            break;
        }
      }
      else /* SGSN_RESPONSE */
      {
        /*
         * Label LEST_RESPONSE
         */
        switch (command)
        {
          case U_UA: /* RESPONSE LOCAL_ESTABLISHMENT */
            /*
             * Label LEST_UA
             */

            if (pf_bit EQ 1)
            {
              rc_xid_valid = u_check_xid (&ll_unitdata_ind->sdu,
                                          cr_bit, command);

              if (rc_xid_valid)
              {
                l3p_outstanding = (llc_data->u->xid_tag_sent & (0x1L << XID_LAYER_3)) ? TRUE : FALSE;

                /*
                 * An UA response frame should only be handled if the XID L3
                 * para are included or not same as requested. Else ignore it!
                 */
                if (llc_data->decoded_l3_xid.valid == l3p_outstanding)
                {
                  /*
                   * <R.LLC.ABMEST_R.A.018>
                   */
                  sig_u_t200_stop_req();

                  /*
                   * If an XID parameter is not included in the response and
                   * if it was included in the request, the requested value is
                   * implicit accepted.
                   */
                  u_handle_optimization ();

                  /*
                   * Reset_received cannot be TRUE, otherwise
                   * frame would not be valid
                   */
                  u_eval_xid (cr_bit, &reset_received, &xid_ind);

                  PFREE (ll_unitdata_ind);

                  if (llc_data->u->release_requested == TRUE)
                  {
                    /*
                     * Do not send any establish confirmation to L3 and start
                     * release procedure
                     */
                    llc_data->u->retransmission_counter = 0;

                    SET_STATE (U, U_LOCAL_RELEASE);

                    /*
                     * Send GRLC_DATA_REQ (DISC), start T200.
                     */
                    u_send_disc();
                  }
                  else
                  {
                    if (llc_data->u->ind_cnf_establishment EQ IND_ESTABLISHMENT)
                    {
                      u_send_ll_establish_ind ();

                      SET_STATE (U, U_ESTABLISH_RES_PENDING);

                      /*
                       * start ABM (re-)establish when establish response is received
                       */
                    }
                    else /* CNF_ESTABLISHMENT */
                    {
                      u_send_ll_establish_cnf ();

                      SET_STATE (U, U_ABM);

                      /*
                       * inform LLME about (re-)establischment
                       */
                      sig_u_llme_abmest_ind();
                    }
                  }
                }
                else
                {
                  PFREE (ll_unitdata_ind);
                }
              }
              else
              {
                PFREE (ll_unitdata_ind);

                /*
                 * request T200 to let the timer manually expire,
                 * thus retransmitting the frame immediately
                 */
                sig_u_t200_expire_req ();

                /* SET_STATE (U, SAME_STATE); */
              }
            }
            else /* pf_bit == 0 */
            {
              PFREE (ll_unitdata_ind);

              u_send_llgmm_status_ind (LLGMM_ERRCS_MULT_ASS_TLLI);

              /* SET_STATE (U, SAME_STATE); */
            }
            break;

          case U_DM: /* RESPONSE LOCAL_ESTABLISHMENT */
            /*
             * Label LEST_DM
             */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              sig_u_t200_stop_req();

              if (llc_data->u->release_requested == TRUE)
              {
                u_send_ll_release_cnf ();
              }
              else
              {
                u_send_ll_release_ind (LL_RELCS_DM_RECEIVED);
              }

              SET_STATE (U, U_ADM);
            }
            break;

          case U_XID: /* RESPONSE LOCAL_ESTABLISHMENT */
           /*
            * Label XIDRES
            */
            u_label_xidres (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          case U_FRMR: /* RESPONSE LOCAL_ESTABLISHMENT */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_RECEIVED);
            break;

          default:
            llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;

            /*
             * Label FRMR_COND
             */
            u_label_frmr_cond (ll_unitdata_ind, cr_bit,
              llc_data->u->frmr_reason);
            break;
        }
      }
      break; /* U_LOCAL_ESTABLISHMENT */

    case U_ABM:
      if (cr_bit EQ SGSN_COMMAND)
      {
        /*
         * Label ABM_COMMAND
         */
        switch (command)
        {
          case U_SABM: /* COMMAND ABM */
            /*
             * Label ABM_REEST_PEER
             */
            if (pf_bit EQ 1)
            {
              u_send_llgmm_status_ind (LLGMM_ERRCS_PEER_REEST);

              rc_xid_valid = u_check_xid (&ll_unitdata_ind->sdu,
                cr_bit, command);

              PFREE (ll_unitdata_ind);

              if (rc_xid_valid)
              {
                /*
                 * No check needed, reset_received cannot be TRUE
                 * (<R.LLC.XID_INVA.A.003>) and xid_ind is meaningless
                 * (LL_ESTABLISH_IND contains N201-U/I).
                 */
                u_eval_xid (cr_bit, &reset_received, &xid_ind);

                /*
                 * Label S_EST_IND
                 */
                u_label_s_est_ind (pf_bit);
              }
            }
            else /* pf_bit == 0 */
            {
              PFREE (ll_unitdata_ind);
            }
            break;

          case U_DISC: /* COMMAND ABM */
            /*
             * Label ABM_DISC
             */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              PALLOC (ll_release_ind, LL_RELEASE_IND);

              ll_release_ind->sapi  = llc_data->current_sapi;
#ifdef LL_2to1
              ll_release_ind->ps_cause.ctrl_value = CAUSE_is_from_llc;
              ll_release_ind->ps_cause.value.llc_cause = LL_RELCS_NORMAL;
#else
              ll_release_ind->cause = LL_RELCS_NORMAL;
#endif
              TRACE_1_OUT_PARA("s:%d", ll_release_ind->sapi);
              PSEND (hCommSNDCP, ll_release_ind);

              SET_STATE (U, U_ADM);

              sig_u_llme_abmrel_ind();

              /*
               * Send UA response without XID information field (= FALSE).
               */
              u_send_ua (pf_bit, FALSE);
            }
            break;

          case U_XID: /* COMMAND  ABM */
            /*
             * Label XID
             */
            u_label_xid (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          default:
            if (pf_bit EQ 1)
            {
              llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL_ABM;

              /*
               * Label FRMR_COND_REEST
               */
              u_label_frmr_cond_reest (ll_unitdata_ind, cr_bit,
                llc_data->u->frmr_reason);
            }
            else
            {
              PFREE (ll_unitdata_ind);
            }
            break;
        }
      }
      else /* SGSN_RESPONSE */
      {
        /*
         * Label ABM_RESPONSE
         */
        switch (command)
        {
          case U_UA: /* RESPONSE ABM */
            /*
             * Label ABM_UA
             */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_MULT_ASS_TLLI);
            break;

          case U_DM: /* RESPONSE ABM */
            /*
             * Label ABM_DM
             */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              u_send_llgmm_status_ind (LLGMM_ERRCS_DM1_RECEIVED);
            }
            else /* pf_bit == 0 */
            {
              u_send_llgmm_status_ind (LLGMM_ERRCS_DM0_RECEIVED_REEST);

              llc_data->u->retransmission_counter = 0;

              /*
               * Send LL_ESTABLISH_IND after successful establishment.
               */
              llc_data->u->ind_cnf_establishment = IND_ESTABLISHMENT;

              SET_STATE (U, U_LOCAL_ESTABLISHMENT);

              sig_u_llme_abmrel_ind();

              /*
               * Re-establishment procedure: send GRLC_DATA_REQ (SABM),
               * start T200.
               */
              u_send_sabm();
            }
            break;

          case U_XID: /* RESPONSE ABM */
           /*
            * Label XIDRES
            */
            u_label_xidres (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          case U_FRMR: /* RESPONSE ABM */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_RECEIVED);
            break;

          default:
            llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL_ABM;

            /*
             * Label FRMR_COND_REEST
             */
            u_label_frmr_cond_reest (ll_unitdata_ind, cr_bit,
              llc_data->u->frmr_reason);
            break;
        }
      }
      break; /* U_ABM */

    case U_LOCAL_RELEASE:
      if (cr_bit EQ SGSN_COMMAND)
      {
        /*
         * Label LREL_COMMAND
         */
        switch (command)
        {
          case U_SABM: /* COMMAND LOCAL_RELEASE */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              /*
               * Send DM response with F bit set to 1.
               */
              u_send_dm (1);
            }
            break;

          case U_DISC: /* COMMAND LOCAL_RELEASE */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              /*
               * Send UA response with F bit set to 1, but without information
               * field (= FALSE).
               */
              u_send_ua (1, FALSE);
            }
            break;

          case U_XID: /* COMMAND  LOCAL_RELEASE */
            /*
             * Label XID
             */
            u_label_xid (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          default:
            if (pf_bit EQ 1)
            {
              llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;

              /*
               * Label FRMR_COND
               */
              u_label_frmr_cond (ll_unitdata_ind, cr_bit,
                llc_data->u->frmr_reason);
            }
            else
            {
              PFREE (ll_unitdata_ind);
            }
            break;
        }
      }
      else /* SGSN_RESPONSE */
      {
        /*
         * Label LREL_RESPONSE
         */
        switch (command)
        {
          case U_UA: /* RESPONSE LOCAL_RELEASE */
            /*
             * no break!
             */
          case U_DM: /* RESPONSE LOCAL_RELEASE */
            /*
             * Label LREL_UA_DM
             */
            PFREE (ll_unitdata_ind);

            if (pf_bit EQ 1)
            {
              SET_STATE (U, U_ADM);

              sig_u_t200_stop_req();

              u_send_ll_release_cnf();
            }
            else /* pf_bit == 0 */
            {
              if (command EQ U_UA)
              {
                u_send_llgmm_status_ind (LLGMM_ERRCS_MULT_ASS_TLLI);
              }
            }
            break;

          case U_XID: /* RESPONSE LOCAL_RELEASE */
           /*
            * Label XIDRES
            */
            u_label_xidres (ll_unitdata_ind, command, cr_bit, pf_bit);
            break;

          case U_FRMR: /* RESPONSE LOCAL_RELEASE */
            PFREE (ll_unitdata_ind);
            u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_RECEIVED);
            break;

          default:
            llc_data->u->frmr_reason = FRMR_UNDEFINED_CTRL;

            /*
             * Label FRMR_COND
             */
            u_label_frmr_cond (ll_unitdata_ind, cr_bit,
              llc_data->u->frmr_reason);
            break;
        }
      }
      break; /* U_LOCAL_RELEASE */

    default:
      PFREE (ll_unitdata_ind);
      TRACE_ERROR( "SIG_RX_U_DATA_IND unexpected" );
      break;
  }
} /* sig_rx_u_data_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_rx_u_frmr_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_RX_U_FRMR_IND
|
| Parameters  : ll_unitdata_ind   - valid pointer to the frame that caused the
|                                   frame rejection condition
|               pdu_type          - type of frame
|               frmr_ctrl_length  - length of frame control field, if known
|               cr_bit            - C/R bit setting of frame
|               frmr_reason       - reason of frame rejection condition
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_rx_u_frmr_ind (T_LL_UNITDATA_IND *ll_unitdata_ind,
                               T_PDU_TYPE pdu_type,
                               USHORT frmr_ctrl_length,
                               T_BIT cr_bit,
                               UBYTE frmr_reason)
{
  TRACE_ISIG( "sig_rx_u_frmr_ind" );

  /*
   * Set TLLI for current transaction.
   */
  llc_data->u->current_tlli = ll_unitdata_ind->tlli;

  switch (GET_STATE (U))
  {
    case U_ADM:
    case U_LOCAL_ESTABLISHMENT:
    case U_ESTABLISH_RES_PENDING:
    case U_REMOTE_ESTABLISHMENT:
      /*
       * No break!
       */
    case U_LOCAL_RELEASE:
      u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_COND);

      u_send_frmr (ll_unitdata_ind, pdu_type, frmr_ctrl_length,
        llc_data->sapi->vs, llc_data->sapi->vr,
        cr_bit, frmr_reason);

      PFREE (ll_unitdata_ind);
      break;
    case U_ABM:
      u_send_llgmm_status_ind (LLGMM_ERRCS_FRMR_COND_REEST);

      SET_STATE (U, U_LOCAL_ESTABLISHMENT);

      /*
       * Set bit W4 in frmr_reason to indicate ABM mode.
       */
      frmr_reason |= FRMR_W4;

      u_send_frmr (ll_unitdata_ind, pdu_type, frmr_ctrl_length,
        llc_data->sapi->vs, llc_data->sapi->vr,
        cr_bit, frmr_reason);

      sig_u_llme_abmrel_ind();

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
      break;
    default:
      PFREE (ll_unitdata_ind);
      TRACE_ERROR( "SIG_RX_U_FRMR_IND unexpected" );
      break;
  }

} /* sig_rx_u_frmr_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_t200_u_expired_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_T200_U_EXPIRED_IND
|
| Parameters  : grlc_data_req - a valid pointer to the frame (primitive) that
|                               is associated with the timer
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_t200_u_expired_ind
(
#ifndef LL_DESC
  T_LL_UNITDATA_REQ  *ll_unitdesc_req,
#else
  T_LL_UNITDESC_REQ  *ll_unitdesc_req,
#endif
  UBYTE cause,
  T_EXPIRY_MODE_TYPE mode
)
{
  UBYTE  u_state;

  TRACE_ISIG( "sig_t200_u_expired_ind" );

  /*
   * Set TLLI for current transaction.
   */
  llc_data->u->current_tlli = llc_data->tlli_new;

  u_state = GET_STATE(U);
  switch (u_state)
  {
    case U_LOCAL_ESTABLISHMENT:
      /*
       * First check if an L3 release requested is pendling
       */
      if (llc_data->u->release_requested == TRUE)
      {
        /*
         * Do not restart timer. Send the release confirm.
         */
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
        PFREE (ll_unitdesc_req);

        u_send_ll_release_cnf();

        SET_STATE (U, U_ADM);
      }
      else
      {
        /*
         * Increment retransmission_counter.
         */
        llc_data->u->retransmission_counter++;

        if (llc_data->u->retransmission_counter <= *(llc_data->n200))
        {
          /*
           * T200 has to be started before the primitive is sent to TX, because
           * the primitive is copied by t200_start() and it may not be valid
           * anymore after sending to TX.
           */
          sig_u_t200_start_req (ll_unitdesc_req, cause);

          sig_u_tx_data_req (ll_unitdesc_req, cause);
        }
        else /* retransmission_counter exceeds N200 */
        {
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
          PFREE (ll_unitdesc_req);

          /*
           * Label LEST_N200
           */

          u_send_llgmm_status_ind (LLGMM_ERRCS_SABM_NO_PEER_RES);

          if (mode == EXPIRY_TIMED)
          {
            u_send_ll_release_ind (LL_RELCS_NO_PEER_RES);
          }
          else /* EXPIRY_REQUESTED */
          {
            /*
             * <R.LLC.ABMEST_R.A.015>
             * vs. <R.LLC.ABMEST_R.A.030>
             */
            u_send_ll_release_ind (LL_RELCS_INVALID_XID);
          }

          SET_STATE (U, U_ADM);
        }
      }
      break; /* U_LOCAL_ESTABLISHMENT */

    case U_LOCAL_RELEASE:
      /*
       * First check if an L3 release requested is pendling
       */
      if (llc_data->u->release_requested == TRUE)
      {
        /*
         * Do not restart timer. Send the release confirm.
         */
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
        PFREE (ll_unitdesc_req);

        u_send_ll_release_cnf();

        SET_STATE (U, U_ADM);
      }
      else
      {
        /*
         * Increment retransmission_counter.
         */
        llc_data->u->retransmission_counter++;

        if (llc_data->u->retransmission_counter <= *(llc_data->n200))
        {
          /*
           * T200 has to be started before the primitive is sent to TX, because
           * the primitive is copied by t200_start() and it may not be valid
           * anymore after sending to TX.
           */
          sig_u_t200_start_req (ll_unitdesc_req, cause);
          sig_u_tx_data_req (ll_unitdesc_req, cause);
        }
        else /* retransmission_counter exceeds N200 */
        {
#ifdef LL_DESC
          llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
          PFREE (ll_unitdesc_req);

          SET_STATE (U, U_ADM);

          u_send_llgmm_status_ind (LLGMM_ERRCS_DISC_NO_PEER_RES);
          u_send_ll_release_cnf ();
        }
      }
      break;

    case U_ADM:
    case U_ABM:
      llc_data->u->retransmission_counter++;
      if (llc_data->u->retransmission_counter <= *(llc_data->n200))
      {
        /* If T200 expires and needs to send SABM the state should
         * be set properly
         */
        if (u_state == U_ADM AND
            llc_data->u->xid_pending NEQ TRUE )
        {
          SET_STATE (U,U_LOCAL_ESTABLISHMENT);
        }
        /*
         * T200 has to be started before the primitive is sent to TX, because
         * the primitive is copied by t200_start() and it may not be valid
         * anymore after sending to TX.
         */
        sig_u_t200_start_req (ll_unitdesc_req, cause);
        sig_u_tx_data_req (ll_unitdesc_req, cause);

        /* SET_STATE (U, SAME_STATE); */
      }
      else
      {
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
        PFREE (ll_unitdesc_req);

        /*
         * Label XIDSENT_N200
         */

        u_send_llgmm_status_ind (LLGMM_ERRCS_XID_NO_PEER_RES);

        if (llc_data->u->xid_pending)
        {
          llc_data->u->xid_pending = FALSE;
        }

        if (u_state == U_ADM)
        {
          if (llc_data->u->xid_tag_sent & (0x00000001L << XID_LAYER_3))
          {
            if (mode == EXPIRY_TIMED)
            {
              u_send_ll_status_ind (LL_ERRCS_NO_PEER_RES);
            }
            else /* EXPIRY_REQUESTED */
            {
              u_send_ll_status_ind (LL_ERRCS_INVALID_XID);
            }
          }

          /* SET_STATE (U, SAME_STATE); */
        }
        else /* U_ABM */
        {
          if (mode == EXPIRY_TIMED)
          {
            u_send_ll_release_ind (LL_RELCS_NO_PEER_RES);
          }
          else /* EXPIRY_REQUESTED */
          {
            /*
             * <R.LLC.XIDNEG_O.A.016>
             * vs. <R.LLC.XIDNEG_O.A.022>
             */
            u_send_ll_release_ind (LL_RELCS_INVALID_XID);
          }

          SET_STATE (U, U_ADM);

          sig_u_llme_abmrel_ind();
        }
      }
      break;

    default:
#ifdef LL_DESC
      llc_cl_desc3_free((T_desc3*)ll_unitdesc_req->desc_list3.first);
#endif /* LL_DESC */
      PFREE (ll_unitdesc_req);
      TRACE_ERROR( "SIG_T200_U_EXPIRED_IND unexpected" );
      break;
  }
} /* sig_t200_u_expired_ind() */


/*
+------------------------------------------------------------------------------
| Function    : sig_llme_u_reest_req
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_LLME_U_REEST_REQ
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_llme_u_reest_req ( void )
{
  TRACE_FUNCTION( "sig_llme_u_reest_req" );

  switch (GET_STATE(U))
  {
    case U_ABM:
      llc_data->u->retransmission_counter = 0;

      /*
       * Send LL_ESTABLISH_IND after successful re-establishment.
       */
      llc_data->u->ind_cnf_establishment = IND_ESTABLISHMENT;

      SET_STATE (U, U_LOCAL_ESTABLISHMENT);

      /*
       * Send GRLC_DATA_REQ (SABM), start T200.
       */
      u_send_sabm();
      break;

    default:
      TRACE_ERROR( "SIG_LLME_U_REEST_REQ unexpected" );
      break;
  }

} /* sig_llme_u_reest_req() */

/*
+------------------------------------------------------------------------------
| Function    : sig_irx_u_no_frame_expected_ind
+------------------------------------------------------------------------------
| Description : Handles the internal signal SIG_IRX_U_NO_FRAME_EXPECTED_IND
|
| Parameters  :
|
+------------------------------------------------------------------------------
*/
GLOBAL void sig_irx_u_no_frame_expected_ind ( void )
{
  TRACE_FUNCTION( "sig_irx_u_no_frame_expected_ind" );

  switch (GET_STATE(U))
  {
    case U_ADM:
      /*
       * An LLE shall transmit a DM resonse to any valid command received
       * that it cannot action: GSM 04.64 Ver 6.7.0 Chapter 6.4.1.4
       * Upon receiption of an I+S frame in ADM state, the MS shall send
       * a DM response with F bit set 0.
       */
      u_send_dm (0);
      break;

    case U_LOCAL_ESTABLISHMENT:
    case U_ESTABLISH_RES_PENDING:
    case U_REMOTE_ESTABLISHMENT:
      /*
       * Ignore S- and I/S-frames received while ABM establishment. Do
       * not send any response: GSM 04.64 Ver 6.7.0 Chapter 8.5.1.1
       */
      break;

    case U_LOCAL_RELEASE:
      /*
       * Ignore S- and I/S-frames received while ABM termination. Do
       * not send any response: GSM 04.64 Ver 6.7.0 Chapter 8.5.2.1
       */
      break;

    case U_ABM:
    default:
      TRACE_ERROR( "SIG_IRX_U_NO_FRAME_EXPECTED_IND unexpected" );
      break;
  }

} /* sig_irx_u_no_frame_expected_ind() */

