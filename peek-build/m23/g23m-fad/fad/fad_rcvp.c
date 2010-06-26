/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_RCVP
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
|             of incomming primitives for the component
|             Fax Adaptation 3.45 of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef FAD_RCVP_C
#define FAD_RCVP_C
#endif

#define ENTITY_FAD

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_fad.h"
#include "mon_fad.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "fad.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

LOCAL void AlignByte(UBYTE *byte)
{
  const USHORT tbl[] =
  {
    0x7E00, /* 01111110 00000000 */
    0x00FC, /* 00000000 11111100 */
    0x01F8, /* 00000001 11111000 */
    0x03F0, /* 00000011 11110000 */
    0x07E0, /* 00000111 11100000 */
    0x0FC0, /* 00001111 11000000 */
    0x1F80, /* 00011111 10000000 */
    0x3F00, /* 00111111 00000000 */
  };
  UBYTE i;

  T_RCV *Rcv = &fad_data->rcv;

  if (Rcv->FlagFound EQ 0) /* initialization */
    Rcv->AlignWord = *byte;

  Rcv->AlignWord <<= 8;
  Rcv->AlignWord += *byte;
  if (Rcv->FlagFound < FLAGFOUND_MIN)
  {
    for (i = 0; i < 8; i++)
    {
      if (tbl[i] EQ (Rcv->AlignWord & tbl[i]) AND ((UBYTE)(Rcv->AlignWord >> i) EQ HDLC_FLAG))
      {
        if (Rcv->AlignShift NEQ i)
        {
          Rcv->AlignShift = i;
          Rcv->FlagFound = 0;
        }
        Rcv->FlagFound++;
        break;
      }
    }
  }
  if (Rcv->FlagFound EQ FLAGFOUND_MIN)
    *byte = (UBYTE)(Rcv->AlignWord >> Rcv->AlignShift);
}

LOCAL void align_ra_data_ind(T_FD *pFD)
{
  USHORT i;

  for (i = 0; i < pFD->len >> 1; i++)
  {
    AlignByte(&pFD->buf[2*i+1]);
  }
}

#ifdef _TARGET_
void switch_ra_detect_req(UBYTE detect)
{
  T_RA_DETECT_REQ ra_detect_req;
  ra_detect_req.detect = detect;
  l1i_ra_detect_req (&ra_detect_req);
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVP            |
| STATE   : code                       ROUTINE : rcv_ra_data_ind     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive RA_DATA_IND received from RA.

*/

GLOBAL void rcv_ra_data_ind (T_RA_DATA_IND *ra_data_ind)
{
  T_RCV *rcv = &fad_data->rcv;
  UBYTE frame_type;
  T_FD *pFD;

  TRACE_FUNCTION ("rcv_ra_data_ind()");

  if (GET_STATE(KER) EQ KER_NULL)
    return;

#ifdef _SIMULATION_
  PACCESS(ra_data_ind);

  if (GET_STATE(KER) NEQ KER_NULL)
  {
    pFD = cl_ribu_get_new_frame_desc(fad_data->rbm.ribu);
    pFD->type = ra_data_ind->fr_type;
    pFD->len = ra_data_ind->sdu.l_buf >> 3;
    memcpy(pFD->buf, ra_data_ind->sdu.buf, pFD->len);
  }

  PFREE (ra_data_ind);
#endif

  if (fad_data->rbm.ribu->idx.filled > 1)
    TRACE_EVENT_P3("rbm: filled=%d; wi=%d; ri=%d", fad_data->rbm.ribu->idx.filled, fad_data->rbm.ribu->idx.wi, fad_data->rbm.ribu->idx.ri);

  pFD = cl_ribu_get(fad_data->rbm.ribu);

  if (pFD->type EQ FR_STATUS)
  {

#if defined TRACE_FAD_DL_STATUS || defined _SIMULATION_
    {
    char buf[14] = "RS:0x  ->0x  ";
    BYTE2HEXSTR(*(pFD->buf+1), &buf[5]);
    align_ra_data_ind(pFD);
    BYTE2HEXSTR(*(pFD->buf+1), &buf[11]);
    TRACE_EVENT(buf);
    }
#else
    align_ra_data_ind(pFD);
#endif

  }

  frame_type = rcv_FrameType(pFD);

  switch (GET_STATE (RCV))
  {
  case SYNC_RCV:
    switch (frame_type)
    {
    case FT_PREAMBLE:
      rcv->bytes_expect = PREAMBLE_BYTES_TI_EXP;
      rcv->bytes_expect -= pFD->len;
      SET_STATE (RCV, PREAM_RCV);
      if (!rcv->data_in_rbm)
      {
        sig_rcv_ker_preamble_ind();
#ifdef _SIMULATION_
        TRACE_EVENT ("BCS-TRA:first PREAMBLE FLAG rcvd");
#endif
      }
      else
      {
        rcv->preamble_pending = TRUE;
#ifdef _SIMULATION_
        TRACE_EVENT ("BCS-TRA:first PREAM FLAG rcvd (p)");
#endif
      }
      break;

    case FR_TRAIN:
      SET_STATE (RCV, TRAIN_RCV);
      /*
       * set READY flag to send first DATA to T30
       */
      rcv->t30_req_frames = TRUE;
      sig_rcv_ker_train_ind();
      break;

    case FT_TRAIN_RESPONSE:
      sig_rcv_ker_status_ind();
      break;

    default:
      break;
    }
    break; /* case SYNC_RCV: */

  case PREAM_RCV:
    if (frame_type EQ FR_STATUS)
    {
      if (rcv->bytes_expect <= pFD->len)
      {
        rcv->bytes_expect = 0;
        SET_STATE (RCVSUB, SUB_PREAMBLE);
#ifdef _SIMULATION_
        TRACE_EVENT ("BCS-TRA:PREAM time elapsed");
#endif
        SET_STATE (RCV, BCS_RCV);
      }
      else
        rcv->bytes_expect -= pFD->len;
    }
    break; /* case PREAM_RCV: */

  case BCS_RCV:
    switch (GET_STATE (RCVSUB))
    {
    case SUB_IDLE:
      if (frame_type EQ FR_SYNC)
      {
        if (!rcv->data_in_rbm)
        {
          sig_rcv_ker_bcs_ind(rcv->hdlc, rcv->hdlc_len);
#ifdef _SIMULATION_
          TRACE_EVENT ("BCS-TRA:SYNC rec - HDLC compl");
#endif
        }
        else
        {
          rcv->bcs_pending = TRUE;
#ifdef _SIMULATION_
          TRACE_EVENT ("BCS-TRA:SYNC rec - HDLC compl (p)");
#endif
        }
        rcv_ResetFormatter();
        SET_STATE (RCV, SYNC_RCV);
      }
      break;

    case SUB_DATA:
      switch (frame_type)
      {
      case FR_SYNC:
        sig_rcv_ker_bcs_ind (rcv->hdlc, rcv->hdlc_len);
#ifdef _SIMULATION_
        TRACE_EVENT ("BCS-TRA:SYNC rec - HDLC compl");
#endif
        rcv_ResetFormatter ();
        SET_STATE (RCV, SYNC_RCV);
        break;

      case FR_STATUS:
        if (rcv->hdlc_len + pFD->len > HDLC_LEN)
        {
          SET_STATE (RCVSUB, SUB_IDLE);
          TRACE_EVENT ("BCS-TRA:HDLC frame too long !!!");
          break;
        }
        rcv_SetHdlcFrame(pFD, &rcv->hdlc[rcv->hdlc_len], &rcv->hdlc_len);
        break;

      default: /* discard non-HDLC data */
        break;
      }
      break;

    default:
      break;
    }
    break; /* case BCS_RCV: */

  case TCF_RCV:
    switch (GET_STATE (RCVSUB))
    {
    case SUB_IDLE:
      if (frame_type NEQ FR_SYNC)
      {
       rcv->bytes_expect = snd_DurationToBytes (fad_data->snd.trans_rate, TIME_TCF_RCV);
       rcv->bytes_expect -= pFD->len;
       rcv->bytes_rcvd   = 0;
       rcv->bytes_good   = 0;
       SET_STATE (RCVSUB, SUB_TCF);
       TRACE_EVENT("SET_STATE (RCVSUB, SUB_TCF)");
      }
      break;

    case SUB_TCF:
      if (frame_type EQ FR_SYNC)
      {

#ifdef BMI_TCF_WORKAROUND
        if (rcv->bytes_rcvd < rcv->bytes_expect) /* ignore TCF frames which are destroyed by BMI */
          break;
#endif

        sig_rcv_ker_tcf_ind();
        rcv_ResetFormatter();
        TRACE_EVENT ("MSG-TRA: rec TCF compl (too short)");
        SET_STATE (RCV, SYNC_RCV);
      }
      else if (frame_type EQ FR_STATUS)
      {
        rcv_ResetFormatter();
        TRACE_EVENT ("MSG-TRA: rec FR_STATUS during SUB_TCF");
        SET_STATE (KER, IDLE);
        SET_STATE (RCV, SYNC_RCV);
      }
      else /* frame_type NEQ FR_SYNC */
      {
        rcv->bytes_rcvd += pFD->len;
        if (rcv->bytes_expect <= pFD->len)
        {
          rcv->bytes_expect = 0;
          SET_STATE (RCVSUB, SUB_POST_TCF);
        }
        else
          rcv->bytes_expect -= pFD->len;

        rcv_SetErrorRatio(pFD);
      }
      break;

    case SUB_POST_TCF:
      if (frame_type EQ FR_SYNC)
      {
        sig_rcv_ker_tcf_ind();
        rcv_ResetFormatter();
        TRACE_EVENT ("MSG-TRA:rec TCF compl");
        SET_STATE (RCV, SYNC_RCV);
      }
      else
      {
        while (fad_data->rbm.ribu->idx.filled)
        {
          cl_ribu_read_index(&fad_data->rbm.ribu->idx);
#ifdef _SIMULATION_
          TRACE_EVENT_P3("ribu: f=%2d - w=%2d - r=%2d - SUB_POST_TCF",
                          fad_data->rbm.ribu->idx.filled,
                          fad_data->rbm.ribu->idx.wi,
                          fad_data->rbm.ribu->idx.ri);
#endif
        }
      }
      break;

    default:
      rcv_ResetFormatter ();
      SET_STATE (RCV, SYNC_RCV);
      sig_rcv_ker_error_ind (ERR_FAD_SUB_TCF);
      break;
    }
    break; /* case TCF_RCV: */

  case TRAIN_RCV:
    switch (frame_type)
    {
    case FR_T4DATA:
      {
#ifdef _TARGET_
      switch_ra_detect_req(DET_NO_STATUS);
#else
      /*
       * handling of first frame restored
       */
      rcv->train_flg = FALSE;
      rcv->data_incoming = TRUE;
      if (!rcv_StoreMsgData (pFD, &rcv->prim_ready))
      {
        rcv->t30_req_frames = FALSE;
        rcv_ResetFormatter ();
        SET_STATE (RCV, SYNC_RCV);
        sig_rcv_ker_error_ind (ERR_OVERFLOW_BCS_RCV);
        break;
      }
      rcv->data_in_rbm = TRUE;
      /* check if T30 READY flag set */
      if (rcv->t30_req_frames)
        sig_ker_rcv_ready_req ();
#endif

      TRACE_EVENT ("MSG-TRA:TRAIN compl");
      rcv->data_incoming = TRUE;
      rcv->bytes_expect = REPEAT_FOREVER;
      rcv->train_flg = TRUE; /*???*/
      SET_STATE (RCV, MSG_RCV);
      sig_rcv_ker_train_end_ind (); /* first MSG data received: resume sending SYNCS */
      break;
    }
    default:
      break;
    }
    break; /* case TRAIN_RCV: */

  case MSG_RCV:
    switch (frame_type)
    {
    case FR_SYNC:
      if (rcv->train_flg EQ FALSE) /*???*/
      {
#ifdef _TARGET_
        switch_ra_detect_req(DET_STATUS);
#endif
        TRACE_EVENT ("MSG-TRA:DATA compl");
        rcv->data_incoming = FALSE;
        if (!fad_data->ker.forward_data)
        {
          rcv->t30_req_frames = FALSE;
          sig_rcv_ker_msg_ind (NULL);
        }
        if (rcv->t30_req_frames)
          rcv_SetFinalBuffer ();

        SET_STATE (RCV, SYNC_RCV);
        rcv_ResetFormatter ();
      }
      break;

    case FR_T4DATA:
      rcv->train_flg = FALSE;
      if (!rcv_StoreMsgData (pFD, &rcv->prim_ready))
      {
        rcv->t30_req_frames = FALSE;
        rcv_ResetFormatter ();
        SET_STATE (RCV, SYNC_RCV);
        sig_rcv_ker_error_ind (ERR_OVERFLOW_BCS_RCV);
        break;
      }
      rcv->data_in_rbm = TRUE;
      if (rcv->t30_req_frames) /* check if T30 READY flag set */
        sig_ker_rcv_ready_req ();
      break;

    default:
      break;
    }
    break; /* case MSG_RCV: */

  default:
    break;
  }
}

#ifdef _TARGET_
/*
+----------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)         MODULE  : FAD_RCVP                  |
| STATE   : code                   ROUTINE : fad_rcv_GetDestDescriptor |
+----------------------------------------------------------------------+

  PURPOSE : WRAPPER: Get frame descriptor for Layer 1 (TI)

*/
GLOBAL T_FD *fad_rbm_get_next_FrameDesc(void)
{
    return (cl_ribu_get_new_frame_desc(fad_data->rbm.ribu));
}
#endif

