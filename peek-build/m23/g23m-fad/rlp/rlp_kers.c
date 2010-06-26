/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_kers.c
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
|  Purpose :  This Modul defines the functions for processing
|             of incomming signals for the component
|             Radio Link Protocol of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_KERS_C
#define RLP_KERS_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_sabm_ind
+------------------------------------------------------------------------------
|  Description :Process signal SIG_SABM_IND received from process rcv. 
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_rcv_ker_sabm_ind(void)
{
  TRACE_FUNCTION ("sig_rcv_ker_sabm_ind()");

  TRACE_EVENT ("SABM rcvd");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      rlp_data->ker.DM_State = IS_SEND;
      rlp_data->ker.DM_FBit  = 1;
      break; 
    
    case RLP_ADM_AND_ATTACHED:
    {
      PALLOC (rlp_connect_ind, RLP_CONNECT_IND);
      PSENDX (L2R, rlp_connect_ind);
      rlp_data->ker.DM_State  = IS_IDLE;
      SET_STATE (KER, RLP_PENDING_CONNECT_IND);
      break;
    }

    case RLP_PENDING_CONNECT_REQ:
    {
      PALLOC (rlp_connect_cnf, RLP_CONNECT_CNF);

      TIMERSTOP (TT_HANDLE);
            
      rlp_connect_cnf->ack_flg = RLP_ACK;

      PSENDX (L2R, rlp_connect_cnf);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = 1;
      ker_init_link_vars ();
      SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      break; 
    }

    case RLP_CONNECTION_ESTABLISHED:
    case RLP_PENDING_REMAP_REQ:
    case RLP_REMAP_DATA:
    {
      PALLOC (rlp_reset_ind, RLP_RESET_IND);

      TIMERSTOP (TT_HANDLE);
      TIMERSTOP (TRCVR_HANDLE);
      ker_reset_all_t_rcvs ();

      PSENDX (L2R, rlp_reset_ind);
      
      ker_deinit_link_vars ();
      SET_STATE (KER, RLP_PENDING_RESET_IND);
      break;
    }

    case RLP_PENDING_RESET_REQ:
    {
      PALLOC (rlp_reset_cnf, RLP_RESET_CNF);

      TIMERSTOP (TT_HANDLE);
      PSENDX (L2R, rlp_reset_cnf);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = 1;

      ker_init_link_vars ();
      SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_disc_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DISC_IND received from process rcv.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_disc_ind(T_BIT pFBit)
{
  TRACE_FUNCTION ("sig_rcv_ker_disc_ind()");

  TRACE_EVENT ("DISC rcvd");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      rlp_data->ker.DM_State = IS_SEND;
      rlp_data->ker.DM_FBit  = pFBit;
      break; 

    case RLP_ADM_AND_ATTACHED:
      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;
      rlp_data->ker.DM_State = IS_IDLE;
      break;

    case RLP_PENDING_CONNECT_REQ:
    case RLP_PENDING_CONNECT_IND:
    {
      PALLOC (rlp_disc_ind, RLP_DISC_IND);

      TIMERSTOP (TT_HANDLE);
      PSENDX (L2R, rlp_disc_ind);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;
      SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      break; 
    }

    case RLP_CONNECTION_ESTABLISHED:
    case RLP_PENDING_REMAP_REQ:
    case RLP_REMAP_DATA:
    {
      PALLOC (rlp_disc_ind, RLP_DISC_IND);

      TIMERSTOP (TT_HANDLE);
      TIMERSTOP (TRCVR_HANDLE);
      ker_reset_all_t_rcvs ();
      
      PSENDX (L2R, rlp_disc_ind);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;
      ker_deinit_link_vars ();
      SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      break;
    }

    case RLP_DISCONNECT_INITIATED:
    {
      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;

      TIMERSTOP (TT_HANDLE); 

      if (rlp_data->ker.DISC_Ind)
      {
        PALLOC (rlp_disc_ind, RLP_DISC_IND);
        PSENDX (L2R, rlp_disc_ind);
        SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      }
      else
      {
        PALLOC (rlp_disc_cnf, RLP_DISC_CNF);
        PSENDX (L2R, rlp_disc_cnf);
        SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      }

      break;
    }

    case RLP_PENDING_RESET_REQ:
    {
      PALLOC (rlp_disc_ind, RLP_DISC_IND);

      TIMERSTOP (TT_HANDLE);
      PSENDX (L2R, rlp_disc_ind);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;

      SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      break;
    }

    case RLP_PENDING_RESET_IND:
    {
      PALLOC (rlp_disc_ind, RLP_DISC_IND);

      PSENDX (L2R, rlp_disc_ind);

      rlp_data->ker.UA_State = IS_SEND;
      rlp_data->ker.UA_FBit  = pFBit;

      SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_ua_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UA_IND received from process rcv.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_rcv_ker_ua_ind(T_BIT pFBit)
{
  TRACE_FUNCTION ("sig_rcv_ker_ua_ind()");

  TRACE_EVENT ("UA rcvd");

  switch (GET_STATE (KER))
  {
  case RLP_PENDING_CONNECT_REQ:
    switch (rlp_data->ker.SABM_State)
    {
    case ISW_WAIT:
      if (pFBit EQ 1)
      {
        PALLOC (rlp_connect_cnf, RLP_CONNECT_CNF);
        TIMERSTOP (TT_HANDLE);

        rlp_connect_cnf->ack_flg = RLP_ACK;

        PSENDX (L2R, rlp_connect_cnf);
        ker_init_link_vars ();
        SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      }
      break;

    default:
      break;
    }
    break; 

  case RLP_DISCONNECT_INITIATED:
    switch (rlp_data->ker.DISC_State)
    {
    case ISW_WAIT:
      if (pFBit EQ rlp_data->ker.DISC_PBit)
      {
        if (rlp_data->ker.DISC_PBit EQ 1)
          rlp_data->ker.Poll_xchg = IW_IDLE;

        TIMERSTOP (TT_HANDLE); 
        if (rlp_data->ker.DISC_Ind)
        {
          PALLOC (rlp_disc_ind, RLP_DISC_IND);
          PSENDX (L2R, rlp_disc_ind);
          SET_STATE (KER, RLP_ADM_AND_ATTACHED);
        }
        else
        {
          PALLOC (rlp_disc_cnf, RLP_DISC_CNF);
          PSENDX (L2R, rlp_disc_cnf);
          SET_STATE (KER, RLP_ADM_AND_ATTACHED);
        }

      }
      break;
    }
    break;

  case RLP_PENDING_RESET_REQ:
    switch (rlp_data->ker.SABM_State)
    {
    case ISW_WAIT:
      if (pFBit EQ 1)
      {
        PALLOC (rlp_reset_cnf, RLP_RESET_CNF);
      
        TIMERSTOP (TT_HANDLE);
        PSENDX (L2R, rlp_reset_cnf);

        ker_init_link_vars ();
        SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
      }
      break;
    }
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_dm_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DM_IND received from process rcv.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_dm_ind(T_BIT pFBit)
{
  TRACE_FUNCTION ("sig_rcv_ker_dm_ind()");

  TRACE_EVENT ("DM rcvd");

  switch (GET_STATE (KER))
  {
  case RLP_PENDING_CONNECT_REQ:
    switch (rlp_data->ker.SABM_State)
    {
    case ISW_WAIT:
      if (pFBit EQ 1)
      {
        PALLOC (rlp_connect_cnf, RLP_CONNECT_CNF);
        TIMERSTOP (TT_HANDLE);
        rlp_data->ker.Poll_xchg = IW_IDLE;

        rlp_connect_cnf->ack_flg = RLP_NAK;

        PSENDX (L2R, rlp_connect_cnf);
        SET_STATE (KER, RLP_ADM_AND_ATTACHED);
      }
      break;
    }
    break; 

  case RLP_DISCONNECT_INITIATED:
    switch (rlp_data->ker.DISC_State)
    {
    case ISW_WAIT:
      if (pFBit EQ rlp_data->ker.DISC_PBit)
      {
        if (rlp_data->ker.DISC_PBit EQ 1)
          rlp_data->ker.Poll_xchg = IW_IDLE;

        TIMERSTOP (TT_HANDLE); 

        if (rlp_data->ker.DISC_Ind)
        {
          PALLOC (rlp_disc_ind, RLP_DISC_IND);
          PSENDX (L2R, rlp_disc_ind);
          SET_STATE (KER, RLP_ADM_AND_ATTACHED);
        }
        else
        {
          PALLOC (rlp_disc_cnf, RLP_DISC_CNF);
          PSENDX (L2R, rlp_disc_cnf);
          SET_STATE (KER, RLP_ADM_AND_ATTACHED);
        }
      }
      break;          
    }
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rr_ind
+------------------------------------------------------------------------------
|  Description :  Process signal SIG_RR_IND received from process rcv.
|
|  Parameters  : cBit
|                pFBit
|                nr
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rr_ind
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rr_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      rlp_data->ker.SF = SF_RR;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending for acknowledgement ?
         */
        if (sbm_frame_in_range (nr))
        {
          ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
          if (retransError)
          {
            SET_STATE(KER, RLP_DISCONNECT_INITIATED);
            TRACE_EVENT("#2");
          }
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rnr_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RNR_IND received from process rcv.
|
|  Parameters  : cBit
|                pFBit
|                nr                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rnr_ind 
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{
  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rnr_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      rlp_data->ker.SF = SF_RNR;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
        {
          ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
          if (retransError)
          {
            SET_STATE(KER, RLP_DISCONNECT_INITIATED);
            TRACE_EVENT("#3");
          }
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rej_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_REJ_IND received from process rcv.
|
|  Parameters  : cBit
|                pFBit
|                nr 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rej_ind
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rej_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      rlp_data->ker.SF = SF_REJ;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending  for acknowledgement ?
         */
        if (sbm_frame_in_range (nr))
        {
          ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
          if (retransError)
          {
            SET_STATE(KER, RLP_DISCONNECT_INITIATED);
            TRACE_EVENT("#4");
          }
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_srej_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SREJ_IND received from process rcv.
|
|  Parameters  :cBit  -
|               pFBit -
|               nr    -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_srej_ind
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_srej_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      rlp_data->ker.SF = SF_SREJ;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
        {
          ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
          if (retransError)
          {
            SET_STATE(KER, RLP_DISCONNECT_INITIATED);
            TRACE_EVENT("#5");
          }
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rr_i_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RR_I_IND received from process rcv.
|
|  Parameters  : cBit  -
|                pFBit -
|                nr    -
|                ns    -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rr_i_ind 
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_FRAME_NUM ns
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rr_i_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      rlp_data->ker.SF = SF_RR;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
          if (!ker_i_handler (ns))
          {
            ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
            if (retransError)
            {
              SET_STATE(KER, RLP_DISCONNECT_INITIATED);
              TRACE_EVENT("#6");
            }
          }
      }
     break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rnr_i_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RNR_I_IND received from process rcv.
|
|  Parameters  : cBit
|                pFBit
|                nr
|                ns 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rnr_i_ind 
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_FRAME_NUM ns
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rnr_i_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      rlp_data->ker.SF = SF_RNR;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
          if (!ker_i_handler (ns))
          {
            ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
            if (retransError)
            {
              SET_STATE(KER, RLP_DISCONNECT_INITIATED);
              TRACE_EVENT("#7");
            }
          }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rej_i_ind
+------------------------------------------------------------------------------
|  Description :  Process signal SIG_REJ_I_IND received from process rcv.
|
|
|  Parameters  : cBit  -
|                pFBit - 
|                nr    -
|              : ns    -
|  Return 
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_rej_i_ind 
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_FRAME_NUM ns
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_rej_i_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      rlp_data->ker.SF = SF_REJ;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
          if (!ker_i_handler (ns))
          {
            ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
            if (retransError)
            {
              SET_STATE(KER, RLP_DISCONNECT_INITIATED);
              TRACE_EVENT("#8");
            }
          }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_srej_i_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SREJ_I_IND received from process rcv.
|
|  Parameters  : cBit  -
|                pFBit -
|                nr    -
|                ns    -
|                
|  Return      : 
+------------------------------------------------------------------------------
*/
     

GLOBAL void sig_rcv_ker_srej_i_ind
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_FRAME_NUM ns
            )
{

  BOOL retransError;

  TRACE_FUNCTION ("sig_rcv_ker_srej_i_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_CONNECTION_ESTABLISHED:
      /*
       * processing for state RLP_CONNECTION_ESTABLISHED
       */
      rlp_data->ker.SF = SF_SREJ;

      /*
       * unsolicited F_Bit ?
       */
      if (!(pFBit EQ 1 AND cBit EQ 0 AND rlp_data->ker.Poll_State NEQ ISW_WAIT))
      {
        /*
         * NR within range of send frames pending
         * for acknowledgement ??
         */
        if (sbm_frame_in_range (nr))
          if (!ker_i_handler (ns))
          {
            ker_s_handler (cBit, pFBit, rlp_data->ker.SF, nr, &retransError);
            if (retransError)
            {
              SET_STATE(KER, RLP_DISCONNECT_INITIATED);
              TRACE_EVENT("#9");
            }
          }
      }
      break;
  }
}
    
/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_ui_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UI_IND received from process rcv.
|
|  Parameters  : cBit  -
|                pFBit -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_xid_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_XID_IND received from process rcv.
|
|  Parameters  : cBit
|                pFBit 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_xid_ind
            (
              T_BIT cBit,
              T_BIT pFBit
            )
{
  TRACE_FUNCTION ("sig_rcv_ker_xid_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * processing for state RLP_ADM_AND_DETACHED
       */
        
      break; 

    default:
      /*
       * processing for all other kernel states
       */
      if (cBit EQ 1)
      {
        switch (GET_STATE(KERXID_C))
        {
          case ISW_WAIT:
          {
            rlp_data->ker.Poll_xchg = IW_IDLE;
            SET_STATE(KERXID_C, ISW_IDLE);

            /*
            TIMERSTOP (rlp_data->txid_handle);

            TIMERSTART 
            (
              TXID,
              rlp_data->ker.T1,
              rlp_data->txid_handle
            );
            */
          }
          break;

          default:
            {
            PALLOC (rlp_xid_ind, RLP_XID_IND);

            ker_get_xid_data
            (
              rbm_get_current_frame () + HEADER_LEN,
              0,
              rlp_xid_ind,
              FALSE,
              &rlp_data->ker.XID_R_Used_Flg
            );

            PSENDX (L2R, rlp_xid_ind);

            ker_put_xid_data
            (
              rlp_data->ker.XID_R_Frame + HEADER_LEN,
              0,
              rlp_data->ker.XID_R_Used_Flg,
              FALSE,
              rlp_data->ker.Rlp_Vers,
              rlp_data->ker.K_iwf_ms,
              rlp_data->ker.K_ms_iwf,
              rlp_data->ker.T1,
              rlp_data->ker.N2,
              rlp_data->ker.T2,
              rlp_data->ker.Pt,
              rlp_data->ker.P0,
              rlp_data->ker.P1,
              rlp_data->ker.P2
            );
           
            rlp_data->ker.XID_R_State = IS_SEND;

            rlp_data->ker.XID_R_FBit = pFBit;

            SET_STATE(KERXID_C, ISW_IDLE);
            }
            break;
        }
      }
      else
      {
        switch (GET_STATE(KERXID_C))
        {
          case ISW_WAIT:
            if (pFBit EQ 1)
            {
              PALLOC (rlp_xid_ind, RLP_XID_IND);

              rlp_data->ker.Poll_xchg = IW_IDLE;

              TIMERSTOP (TXID_HANDLE);
              ker_get_xid_data
              (
                rbm_get_current_frame () + HEADER_LEN,
                0,
                rlp_xid_ind,
                TRUE,
                &rlp_data->ker.XID_C_Used_Flg
              );

              PSENDX (L2R, rlp_xid_ind);
 
              SET_STATE(KERXID_C, ISW_IDLE);
            }
            break;

          default:
            break;
        }
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_test_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_TEST_IND received from process rcv.
|
|  Parameters  : cBit  -
|                pFBit -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_test_ind
            (
              T_BIT cBit,
              T_BIT pFBit
            )
{
  TRACE_FUNCTION ("sig_rcv_ker_test_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_ADM_AND_DETACHED:
      /*
       * no processing for state RLP_ADM_AND_DETACHED
       */
      break; 

    default:
      /*
       * processing for all other kernel states
       */
      if (cBit EQ 1)
      {
        rlp_data->ker.TEST_R_State = IS_SEND;
        rlp_data->ker.TEST_R_FBit  = pFBit;
        memcpy
        (
          rlp_data->ker.TEST_R_Frame,
          rbm_get_current_frame (),
          rlp_data->ker.FrameSize
        );
      }
      break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_remap_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_REMAP_IND received from process rcv.
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_rcv_ker_remap_ind
            (
              void
            )
{
  T_RBM_FRAMEPTR remapFrame;

  TRACE_FUNCTION ("sig_rcv_ker_remap_ind()");

  switch (GET_STATE (KER))
  {
    case RLP_PENDING_REMAP_REQ:
      {
        PALLOC (rlp_xid_ind, RLP_XID_IND);
        
        remapFrame = rbm_get_current_frame();

        rlp_data->ker.RemapNr = (*(T_RLP_FRAME_LONG *)remapFrame)[HEADER_LEN] >> 2;

        sbm_ack_upto_n (rlp_data->ker.RemapNr);

        ker_get_xid_data
          (
          remapFrame + HEADER_LEN,
          2,
          rlp_xid_ind,
          TRUE,
          &rlp_data->ker.XID_C_Used_Flg
          );
        
        PSENDX (L2R, rlp_xid_ind);

        if (!ker_send_remap_data ())
        {
          PALLOC (rlp_remap_cnf, RLP_REMAP_CNF);

          sbm_reset_after_remap
            (
            rlp_data->ker.FrameSize, 
            rlp_data->ker.RemapNr
            );

          PSENDX (L2R, rlp_remap_cnf);
          SET_STATE (KER, RLP_CONNECTION_ESTABLISHED);
          break; 
        }

        SET_STATE (KER, RLP_REMAP_DATA);
        break;
      }

    default:
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_ready_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_READY_IND received from process rcv.
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_rcv_ker_ready_ind(void)
{

  TRACE_FUNCTION ("sig_rcv_ker_ready_ind()");

  switch (GET_STATE (KER))
  {
  case RLP_ADM_AND_DETACHED:
    switch (rlp_data->ker.DM_State)
    {
    case IS_IDLE:
      sig_ker_snd_null_req ();
      break;

    case IS_SEND:
      sig_ker_snd_dm_req (rlp_data->ker.DM_FBit);
      rlp_data->ker.DM_State = IS_IDLE;
      break;
    }
    break; 

  case RLP_ADM_AND_ATTACHED:
    if (!ker_send_txu())
    {
      switch(rlp_data->ker.DM_State)
      {
      case IS_IDLE: 
        switch (rlp_data->ker.UA_State)
        {
        case IS_IDLE:
          sig_ker_snd_null_req ();
          break;

        case IS_SEND:
          sig_ker_snd_ua_req (rlp_data->ker.UA_FBit);
          rlp_data->ker.UA_State = IS_IDLE;
          break;
        }
        break;

      case IS_SEND:
        sig_ker_snd_dm_req(rlp_data->ker.DM_FBit);
        rlp_data->ker.DM_State = IS_IDLE;
        break;
      }
    }
    break; 

  case RLP_PENDING_CONNECT_REQ:
    if (!ker_send_txu())
    {
      switch (rlp_data->ker.SABM_State)
      {
      case ISW_SEND:
        switch (rlp_data->ker.Poll_xchg)
        {
        case IW_IDLE:
          sig_ker_snd_sabm_req ();
          rlp_data->ker.SABM_State = ISW_WAIT;
          rlp_data->ker.SABM_Count++;
          rlp_data->ker.Poll_xchg  = IW_WAIT;
          TIMERSTART(TT_HANDLE, rlp_data->ker.T1); 
          break;

        default:
          sig_ker_snd_null_req ();
          break;
        }
        break;

      default:
        sig_ker_snd_null_req ();
        break;
      }
    }
    break; 

  case RLP_PENDING_CONNECT_IND:
    if (!ker_send_txu())
      sig_ker_snd_null_req ();
    break;

  case RLP_CONNECTION_ESTABLISHED:
    if (!ker_send_txu())
      ker_send_data ();
    break;

  case RLP_DISCONNECT_INITIATED:
    if (!ker_send_txu())
    {
      switch (rlp_data->ker.DISC_State)
      {
      case ISW_WAIT:
        sig_ker_snd_null_req ();
        break;

      default:
        if (rlp_data->ker.DISC_PBit EQ 1 AND rlp_data->ker.Poll_xchg EQ IW_WAIT)
        {
          sig_ker_snd_null_req ();
        }
        else
        {
          sig_ker_snd_disc_req (rlp_data->ker.DISC_PBit);
          if (rlp_data->ker.DISC_PBit EQ 1)
            rlp_data->ker.Poll_xchg = IW_WAIT;

          rlp_data->ker.DISC_State = ISW_WAIT;
          rlp_data->ker.DISC_Count++;
          TIMERSTART (TT_HANDLE, rlp_data->ker.T1); 
        }
        break;
      }
    }
    break;

  case RLP_PENDING_RESET_REQ:
    if (!ker_send_txu())
    {
      switch (rlp_data->ker.SABM_State)
      {
      case ISW_WAIT:
        sig_ker_snd_null_req ();
        break;

      case ISW_SEND:
        switch (rlp_data->ker.Poll_xchg)
        {
        case IW_IDLE:
          sig_ker_snd_sabm_req ();
          rlp_data->ker.SABM_State = ISW_WAIT;
          rlp_data->ker.SABM_Count++;
          rlp_data->ker.Poll_xchg = IW_WAIT;
          TIMERSTART (TT_HANDLE, rlp_data->ker.T1); 
          break;

        default:
          sig_ker_snd_null_req ();
          break;
        }
        break;
      }
    }
    break;

  case RLP_PENDING_RESET_IND:
    if (!ker_send_txu())
      sig_ker_snd_null_req ();
    break;

  case RLP_PENDING_REMAP_REQ:
    if (!ker_send_txu())
    {
      if (rlp_data->ker.Poll_xchg EQ IW_IDLE)
      {
        TIMERSTART(TT_HANDLE, rlp_data->ker.T1 * rlp_data->ker.N2); 
        rlp_data->ker.Poll_xchg = IW_WAIT;
      }
      sig_ker_snd_remap_req (&rlp_data->ker.REMAP_FrameDesc);
    }
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_interpret_rlp_frame |
+--------------------------------------------------------------------+

  PURPOSE : decodes the elements from the given RLP frame

*/

LOCAL void ker_interpret_rlp_frame
            (
              T_RLP_FRAMEPTR  frame,
              T_PDU_TYPE     *pduType,
              T_BIT          *cBit,
              T_BIT          *pFBit,
              T_FRAME_NUM    *nr,
              T_FRAME_NUM    *ns,
              BOOL           *crcOk
            )
{
#ifdef TRACE_INTERPRETED_RLP_FRAME
  T_RLP_DEBUG *deb = &rlp_data->deb;

  deb->idx=0;
  deb->trc_buf[deb->idx++] = ((crcOk NEQ NULL) ? 'D' : 'U');
  deb->trc_buf[deb->idx++] = ':';
#endif

#ifdef _SIMULATION_
  TRACE_FUNCTION ("ker_interpret_rlp_frame()");
#endif

  *cBit  = (T_BIT) (frame[0] & 1);
  *ns    = (T_FRAME_NUM) ((frame[0] >> 3) | ((frame[1] & 1)<<5));
  *pFBit = (T_BIT) ((frame[1] & 2)>>1);

  switch (*ns)
  {
    case 62: /* S-Frame */
    {
      T_SF sFrame;

      *nr = (T_FRAME_NUM) ((frame[1]>>2) & 0x3f);

#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->trc_buf[deb->idx++] = 'S';
      deb->trc_buf[deb->idx++] = 'r';
      DEC_BYTE (*nr, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = ' ';
#endif

      sFrame = (T_SF) ((frame[0]>>1) & 0x03);

      /*
       * maybe optimize with a table!!
       */
      switch (sFrame)
      {
        case SF_RR:
          *pduType = PDU_RR;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "RR ",3);
#endif
          break;

        case SF_RNR:
          *pduType = PDU_RNR;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "RNR",3);
#endif
          break;
        case SF_REJ:
          *pduType = PDU_REJ;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "REJ",3);
#endif
          break;
        case SF_SREJ:
          *pduType = PDU_SREJ;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "SRJ",3);
#endif
          break;
        default:
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "***",3);
#endif
          break;

      }
#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->idx+=3;
#endif
      break;
    }

    case 63: /* U-Frame */
    {
      T_UF uFrame;
      uFrame = (T_UF) ((frame[1]>>2) & 0x1f);

#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->trc_buf[deb->idx++] = 'U';
      deb->trc_buf[deb->idx++] = ' ';
#endif

      /*
       * maybe optimize with a table!!
       */
      switch (uFrame)
      {
        case UF_UI:
          *pduType = PDU_UI;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "UI  ",4);
#endif
          break;

        case UF_DM:
          *pduType = PDU_DM;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "DM  ",4);
#endif
          break;

        case UF_SABM:
          *pduType = PDU_SABM;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "SABM",4);
#endif
          break;

        case UF_DISC:
          *pduType = PDU_DISC;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "DISC",4);
#endif
          break;

        case UF_UA:
          *pduType = PDU_UA;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "UA  ",4);
#endif
          break;

        case UF_NULL:
          *pduType = PDU_NULL;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "NULL",4);
#endif
          break;

        case UF_XID:
          *pduType = PDU_XID;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "XID ",4);
#endif
          break;

        case UF_TEST:
          *pduType = PDU_TEST;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "TEST",4);
#endif
          break;

        case UF_REMAP:
          *pduType = PDU_REMAP;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "RMAP",4);
#endif
          break;

        default:
          *pduType = PDU_INVALID;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "****",4);
#endif
          break;
      }
      *nr = 0;
#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->idx += 4;
#endif
      break;
    }

    default: /* I+S-Frame */
    {
      T_SF sFrame;

      *nr = (T_FRAME_NUM) ((frame[1]>>2) & 0x3f);

      sFrame = (T_SF) ((frame[0]>>1) & 0x03);

#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->trc_buf[deb->idx++] = 'I';
      deb->trc_buf[deb->idx++] = 'r';
      DEC_BYTE (*nr, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = 's';
      DEC_BYTE (*ns, &deb->trc_buf[deb->idx]); deb->idx+=3;
      deb->trc_buf[deb->idx++] = ' ';
#endif
      /*
       * maybe optimize with a table!!
       */
      switch (sFrame)
      {
        case SF_RR:
          *pduType = PDU_RR_I;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "RR ",3);
#endif
          break;

        case SF_RNR:
          *pduType = PDU_RNR_I;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "RNR",3);
#endif
          break;
        case SF_REJ:
          *pduType = PDU_REJ_I;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "REJ",3);
#endif
          break;
        case SF_SREJ:
          *pduType = PDU_SREJ_I;
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "SRJ",3);
#endif
          break;
        default:
#ifdef TRACE_INTERPRETED_RLP_FRAME
          memcpy(&deb->trc_buf[deb->idx], "***",3);
#endif
          break;
      }
#ifdef TRACE_INTERPRETED_RLP_FRAME
      deb->idx += 3;
#endif
      break;
    }
  }
#ifdef TRACE_INTERPRETED_RLP_FRAME
  if (
       (*pduType NEQ PDU_NULL)
       AND
       (
         (crcOk EQ NULL AND rlp_data->uplink_frame_trace)
         OR
         (crcOk NEQ NULL AND rlp_data->downlink_frame_trace)
       )
     )
  {
    deb->trc_buf[deb->idx++] = ' ';
    deb->trc_buf[deb->idx++] = ((*cBit) ? 'C' : 'R');
    deb->trc_buf[deb->idx++] = ((*cBit) ? 'P' : 'F');
    deb->trc_buf[deb->idx++] = ' ';
    HEX_BYTE (frame[1], &deb->trc_buf[deb->idx]); deb->idx+=2;
    HEX_BYTE (frame[0], &deb->trc_buf[deb->idx]); deb->idx+=2;
    deb->trc_buf[deb->idx++] = ' ';
    HEX_BYTE (frame[2], &deb->trc_buf[deb->idx]); deb->idx+=2;
    HEX_BYTE (frame[3], &deb->trc_buf[deb->idx]); deb->idx+=2;

    deb->trc_buf[deb->idx] = '\0';
    TRACE_EVENT (deb->trc_buf);
  }
#endif
  /*
   * no CRC checking at this point. Checking is performed at
   * Layer 1, so only correct frames are forwared.
   */
#ifdef TRACE_INTERPRETED_RLP_FRAME
  if (crcOk NEQ NULL)
#endif
    *crcOk = TRUE;
}

#ifdef _SIMULATION_
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_get_frame_from_sdu  |
+--------------------------------------------------------------------+

  PURPOSE :

*/

LOCAL T_RLP_FRAMEPTR ker_get_frame_from_sdu(T_sdu *sdu)
{
  TRACE_FUNCTION ("ker_get_frame_from_sdu()");

  return sdu->buf+(sdu->o_buf>>3);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : RLP_KERF                |
| STATE   : code                   ROUTINE : ker_analyse_prim        |
+--------------------------------------------------------------------+

  PURPOSE :

*/

#ifdef _SIMULATION_
LOCAL void ker_analyse_prim(T_RA_DATA_IND *ra_data_ind)
#else
LOCAL void ker_analyse_prim(void)
#endif
{
  T_RLP_FRAMEPTR frame;
  T_PDU_TYPE     pduType;
  T_BIT          cBit;
  T_BIT          pFBit;
  T_FRAME_NUM    nr, ns;
  BOOL           crcOk;

  TRACE_FUNCTION ("ker_analyse_prim()");

#ifdef _SIMULATION_
  PACCESS (ra_data_ind);
  frame = ker_get_frame_from_sdu (&ra_data_ind->sdu);
  rbm_store_frame (frame);
  PFREE (ra_data_ind);
#endif

  frame = rbm_get_current_frame ();

/*
  TRACE_DATA_RLP_FRAME(frame);
*/

  ker_interpret_rlp_frame
  (
    frame,
    &pduType,
    &cBit,
    &pFBit,
    &nr,
    &ns,
    &crcOk
  );

  sig_ker_rcv_rawdata_res
  (
    pduType,
    cBit,
    pFBit,
    nr,
    ns,
    crcOk
  );

}

/*
+------------------------------------------------------------------------------
|  Function    : sig_rcv_ker_rawdata_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RAWDATA_IND received from process rcv.
|
|  Parameters  : ra_data_ind - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

#ifdef _SIMULATION_
GLOBAL void sig_rcv_ker_rawdata_ind(T_RA_DATA_IND *ra_data_ind)
#else
GLOBAL void sig_rcv_ker_rawdata_ind(void)
#endif
{
  TRACE_FUNCTION ("sig_rcv_ker_rawdata_ind()");

#ifdef _SIMULATION_
   ker_analyse_prim(ra_data_ind);
#else
   ker_analyse_prim();
#endif
}
