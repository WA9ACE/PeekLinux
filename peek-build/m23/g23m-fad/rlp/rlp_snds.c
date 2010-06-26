/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_snds.c
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

#ifndef RLP_SNDS_C
#define RLP_SNDS_C
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
|  Function    : sig_ker_snd_sabm_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SABM_REQ received from process kernel.
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_sabm_req
            (
              void
            )
{
  TRACE_FUNCTION ("sig_ker_snd_sabm_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      TRACE_EVENT ("Snd SABM");
      snd_send_u_frame (UF_SABM, 1, 1, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_disc_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DISC_REQ received from process kernel.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_disc_req
            (
              T_BIT pFBit
            )
{
  TRACE_FUNCTION ("sig_ker_snd_disc_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      if (pFBit)
      {
        TRACE_EVENT ("Snd DISC (P/F=1)");
      }
      else
      {
        TRACE_EVENT ("Snd DISC (P/F=0)");
      }
      snd_send_u_frame (UF_DISC, pFBit, 1, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_ua_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UA_REQ received from process kernel.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_ua_req
            (
              T_BIT pFBit
            )
{
  TRACE_FUNCTION ("sig_ker_snd_ua_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      if (pFBit)
      {
        TRACE_EVENT ("Snd UA (P/F=1)");
      }
      else
      {
        TRACE_EVENT ("Snd UA (P/F=0)");
      }
      snd_send_u_frame (UF_UA, pFBit, 0, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_dm_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DM_REQ received from process kernel.
|
|  Parameters  : pFBit -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_dm_req
            (
              T_BIT pFBit
            )
{
  TRACE_FUNCTION ("sig_ker_snd_dm_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      if (pFBit)
      {
        TRACE_EVENT ("Snd DM (P/F=1)");
      }
      else
      {
        TRACE_EVENT ("Snd DM (P/F=0)");
      }
      snd_send_u_frame (UF_DM, pFBit, 0, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rr_req
+------------------------------------------------------------------------------
|  Description :  Process signal SIG_RR_REQ received from process kernel.
|
|  Parameters  : cBit  -
|                pFBit -
|                nr    -
|                dtx   -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

#ifdef ENABLE_DTX
GLOBAL void sig_ker_snd_rr_req(T_BIT cBit, T_BIT pFBit, T_FRAME_NUM nr, T_BIT dtx)
#else
GLOBAL void sig_ker_snd_rr_req(T_BIT cBit, T_BIT pFBit, T_FRAME_NUM nr)
#endif
{
  TRACE_FUNCTION ("sig_ker_snd_rr_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
#ifdef ENABLE_DTX
      if (dtx EQ 0)
#endif
        snd_send_s_frame (SF_RR, nr, pFBit, cBit, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rnr_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RNR_REQ received from process kernel.
|
|  Parameters  : cBit   -
|                pFBit  -
|                nr     -
|                dtx    -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_rnr_req
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr,
              T_BIT       dtx
            )
{
  TRACE_FUNCTION ("sig_ker_snd_rnr_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
#ifdef ENABLE_DTX
      if (dtx EQ 0) 
#endif
        snd_send_s_frame (SF_RNR, nr, pFBit, cBit, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rej_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_REJ_REQ received from process kernel.
|
|  Parameters  : cBit  -
|                pFBit -
|                nr    -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_rej_req
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{
  TRACE_FUNCTION ("sig_ker_snd_rej_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_s_frame (SF_REJ, nr, pFBit, cBit, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_srej_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SREJ_REQ received from process kernel.
|
|  Parameters  : cBit  -
|                pFBit -
|                nr    -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_srej_req
            (
              T_BIT       cBit,
              T_BIT       pFBit,
              T_FRAME_NUM nr
            )
{
  TRACE_FUNCTION ("sig_ker_snd_srej_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_s_frame (SF_SREJ, nr, pFBit, cBit, NULL);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rr_i_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RR_I_REQ received from process kernel.
|
|  Parameters  : 
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_rr_i_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_NUM        nr,
              T_FRAME_NUM        ns,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_rr_i_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_si_frame (SF_RR, nr, pFBit, ns, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rnr_i_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_RNR_I_REQ received from process kernel.
|
|  Parameters  : cBit
|                pFBit
|                nr
|                ns
|                dataDesc 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_rnr_i_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_NUM        nr,
              T_FRAME_NUM        ns,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_rnr_i_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_si_frame (SF_RNR, nr, pFBit, ns, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_rej_i_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_REJ_I_REQ received from process kernel.
|
|  Parameters  : cBit     -
|                pFBit    -
|                nr       -
|                ns       -
|                dataDesc -  
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_rej_i_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_NUM        nr,
              T_FRAME_NUM        ns,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_rej_i_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_si_frame (SF_REJ, nr, pFBit, ns, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_srej_i_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SREJ_I_REQ received from process kernel.
|
|  Parameters  : cBit     -
|                pFBit    -
|                nr       -
|                ns       -
|                dataDesc -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_srej_i_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_NUM        nr,
              T_FRAME_NUM        ns,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_srej_i_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      snd_send_si_frame (SF_SREJ, nr, pFBit, ns, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_ui_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UI_REQ received from process kernel.
|
|  Parameters  : cBit      -
|                pFBit     -
|                dataDesc  -
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_ui_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_ui_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      TRACE_EVENT ("Snd UI");
      snd_send_u_frame (UF_UI, pFBit, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_xid_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SABM_REQ received from process kernel.
|
|  Parameters  : cBit
|                pFBit
|                dataDesc
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_xid_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_xid_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      if (pFBit)
      {
        if (cBit)
        {
          TRACE_EVENT ("Snd XID C (P/F=1)");
        }
        else
        {
          TRACE_EVENT ("Snd XID R (P/F=1)");
        }
      }
      else
      {
        if (cBit)
        {
          TRACE_EVENT ("Snd XID C (P/F=0)");
        }
        else
        {
          TRACE_EVENT ("Snd XID R (P/F=0)");
        }
      }

      snd_send_u_frame (UF_XID, pFBit, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_test_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_TEST_REQ received from process kernel.
|
|  Parameters  : cBit     - 
|                pFBit    -
|                dataDesc -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_test_req
            (
              T_BIT              cBit,
              T_BIT              pFBit,
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_test_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      TRACE_EVENT ("Snd TEST");
      snd_send_u_frame (UF_TEST, pFBit, cBit, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_remap_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_REMAP_REQ received from process kernel.
|
|  Parameters  : dataDesc -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_remap_req
            (
              T_FRAME_DESC      *dataDesc
            )
{
  TRACE_FUNCTION ("sig_ker_snd_remap_req()");

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * processing for state SPDU_WAIT_EVENT
       */
      TRACE_EVENT ("Snd REMAP");
      snd_send_u_frame (UF_REMAP, 0, 1, dataDesc);
      break; 
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_null_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_NULL_REQ received from process kernel.
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_null_req(void)
{
  TRACE_FUNCTION ("sig_ker_snd_null_req()");

  if (GET_STATE (SND) EQ SPDU_WAIT_EVENT)
  {
    snd_send_u_frame (UF_NULL, 0, 1, NULL);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_ker_snd_set_frame_size_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_SET_FRAME_SIZE_REQ received from process kernel.
|
|  Parameters  : frameSize -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_ker_snd_set_frame_size_req
            (
              USHORT frameSize
            )
{
  USHORT i;
  /*
  TRACE_FUNCTION ("sig_ker_snd_set_frame_size_req()");
  */

  switch (GET_STATE (SND))
  {
    case SPDU_WAIT_EVENT:
      /*
       * initialize the header only frame descriptor
       */
      rlp_data->snd.HO_FrameDesc.Adr[0] = rlp_data->snd.HO_Frame;
      rlp_data->snd.HO_FrameDesc.Len[0] = frameSize;
      rlp_data->snd.HO_FrameDesc.Adr[1] = NULL;
      rlp_data->snd.HO_FrameDesc.Len[1] = 0;
  
      for (i=2; i<frameSize; i++)
      {
        rlp_data->snd.HO_Frame[i] = 0x41 + (UBYTE)i;
      }

      rlp_data->snd.FrameSize = frameSize;
      break; 
  }
}

