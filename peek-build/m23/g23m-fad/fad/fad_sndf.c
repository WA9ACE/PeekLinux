/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_SNDF
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
|  Purpose :  This Modul defines the procedures and functions for
|             the SND subcomponent SEQ (sequencer)
+----------------------------------------------------------------------------- 
*/ 

#ifndef FAD_SNDF_C
#define FAD_SNDF_C
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
#include "ra_l1int.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialise the sequencer

*/

GLOBAL void snd_init (void)
{
  T_SND *snd = &fad_data->snd;
  T_SBM *sbm = &fad_data->sbm;

  TRACE_FUNCTION ("snd_init()");

  snd->bytes_to_send     = REPEAT_FOREVER;
  snd->ra_req_frames     = 0;
  snd->reset             = SYNC_RESET;
  snd->send_ready_to_t30 = FALSE;
  snd->seq_buflen        = 0;
  snd->seq_bufpos        = 0;
  snd->data_bufpos       = 0;
  snd->fad_data_req      = NULL;

  sbm->FrameSize         = FRAME_SIZE;
  sbm->syncCycle         = SYNC_CYCLE_NONE;
  sbm->framesUntilSync   = 0;

  snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_SYNC, LEN_SEQ_SYNC, 3);
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_setupTCF        |
+--------------------------------------------------------------------+

  PURPOSE : initialise the sequencer for TCF

*/

LOCAL void snd_setupTCF (void)
{
  T_SND *snd = &fad_data->snd;

  snd->seq_buflen = 0;

  switch (snd->trans_rate)
  {
    case R_12000: /* 5 Data frames + 1 SYNC frame */
      snd_SetSequence(snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TCF, LEN_SEQ_TCF, 5);
      snd_SetSequence(&snd->seq_buf [snd->seq_buflen], &snd->seq_buflen, (UBYTE*)FR_SEQ_SYNC, LEN_SEQ_SYNC, 1);
      TRACE_EVENT ("setup TCF 12kbps");
      break;

    case R_7200: /* 3 Data frames + 1 SYNC frame */
      snd_SetSequence(snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TCF, LEN_SEQ_TCF, 3);
      snd_SetSequence(&snd->seq_buf [snd->seq_buflen], &snd->seq_buflen, (UBYTE*)FR_SEQ_SYNC, LEN_SEQ_SYNC, 1);
      TRACE_EVENT ("setup TCF 7.2kbps");
      break;

    case R_14400: /* 5 Data frames */
    case R_9600:
    case R_4800:
    case R_2400:
    default:
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE *)FR_SEQ_TCF, LEN_SEQ_TCF, 5);
      TRACE_EVENT ("setup norm TCF");
      break;
  }
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : snd_SendReset            |
+-------------------------------------------------------------------------+

  PURPOSE : Send SYNC frames following return to IDLE state

*/

GLOBAL void snd_SendReset (USHORT bytes_to_send)
{
  T_SND *snd = &fad_data->snd;

  TRACE_FUNCTION ("snd_SendReset()");

  if (snd->reset)
  {
    snd->reset--;
    snd_SendSequence (bytes_to_send, FR_SYNC);
  }

  if (!snd->reset)
  {
    snd->seq_buflen = 0;
    snd->seq_bufpos = 0;

    switch (GET_STATE (SND))
    {
    case PREAM_SND:
      snd->bytes_to_send = PREAMBLE_BYTES_TI_SND;
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_PREAMBLE, LEN_SEQ_PREAMBLE, 12);
      break;

    case TCF_SND:
      snd->bytes_to_send = snd_DurationToBytes (snd->trans_rate, TIME_TCF_SND);
      snd_setupTCF(); /* set the sequencer for the TCF pattern */
      break;

    case TRAIN_SND:
#ifdef _SIMULATION_
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TRAIN_SND, LEN_SEQ_TRAIN, 4);
      snd_SetSequence (&snd->seq_buf [snd->seq_buflen], &snd->seq_buflen, (UBYTE*)FR_SEQ_SYNC, LEN_SEQ_SYNC, 1);
#else       /* TI: only 2 bytes set - no SYNCs */
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TRAIN_SND, LEN_SEQ_TRAIN, 1);
#endif
      break;
    
    case TRAIN_CNF_SND:
#ifdef _SIMULATION_
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TRAIN_RCV, LEN_SEQ_TRAIN, 4);
      snd_SetSequence (&snd->seq_buf [snd->seq_buflen], &snd->seq_buflen, (UBYTE*)FR_SEQ_SYNC, LEN_SEQ_SYNC, 1);
#else       /* TI: only 2 bytes set - no SYNCs */
      snd_SetSequence (snd->seq_buf, &snd->seq_buflen, (UBYTE*)FR_SEQ_TRAIN_RCV, LEN_SEQ_TRAIN, 1);
#endif
      break;
    }
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_SetSequence     |
+--------------------------------------------------------------------+

  PURPOSE : set frame

*/

GLOBAL void snd_SetSequence (UBYTE *seq_buf, USHORT *seq_buflen,
                             UBYTE *sequence, USHORT seq_len, USHORT repeat)
{
  TRACE_FUNCTION ("snd_SetSequence()");

  *seq_buflen += (seq_len * repeat);

  while (repeat--)
  {
    memcpy (seq_buf, sequence, seq_len);
    seq_buf += seq_len;
  }
}

#if defined TRACE_FAD_UL_STATUS || defined _SIMULATION_

LOCAL void trace_fad_ul_frame_type(USHORT cus_frame_type, UBYTE* buf1, USHORT bytes_to_write)
{
  const char type[5] = "DSYT";
  char buf[15] = "TISx:0x00-0x00";

  if (cus_frame_type < 4)
    buf[3] = type[cus_frame_type];

  if (buf1)
    BYTE2HEXSTR(*(buf1+1), &buf[7]);

  BYTE2HEXSTR((UBYTE)bytes_to_write, &buf[12]);
  TRACE_EVENT (buf);
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_ra_data_req    |
+--------------------------------------------------------------------+

  PURPOSE : Converts contents of frame descriptor to primitive

*/
LOCAL void snd_ra_data_req(T_FD *pFD)
{

#if defined TRACE_FAD_UL_STATUS || defined _SIMULATION_
  
  if (pFD->type EQ FR_STATUS)
  {
    trace_fad_ul_frame_type(FR_STATUS, pFD->buf, pFD->len);
  }
  /*
  void t30_ker_debug(CHAR *header, UBYTE *buf, USHORT len);

  if (pFD->type EQ FR_STATUS)
  {
    t30_ker_debug("BCS-RCV", pFD->buf, pFD->len);
  }
  */
#endif

#ifdef _SIMULATION_
  {
  PALLOC_SDU (ra_data_req, RA_DATA_REQ, (USHORT)(pFD->len<<3));

  TRACE_FUNCTION ("snd_ra_data_req()");

  ra_data_req->sdu.l_buf = pFD->len<<3;
  memcpy (&ra_data_req->sdu.buf, pFD->buf, pFD->len);
  ra_data_req->fr_type = pFD->type;
  ra_data_req->dtx_flg = DTX_DIS;
  ra_data_req->status = ST_SA;
  PSENDX (RA, ra_data_req);
  }
#else

  l1i_ra_data_req_new(pFD);

#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_SendSequence    |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL USHORT snd_SendSequence (USHORT bytes_to_send, UBYTE fr_type)
{
  T_SND *snd = &fad_data->snd;
  USHORT sdu_buf_pos = 0;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("snd_SendSequence()");
#endif

#ifdef _TARGET_
  switch (fr_type)
  {
  case FR_SYNC:
    break;

  case FR_STATUS:
  case FR_TRAIN:
    bytes_to_send = 2;
    break;

  default:
    break;
  }
#endif

  if (snd->bytes_to_send NEQ REPEAT_FOREVER)
  {
    if (snd->bytes_to_send > bytes_to_send)
      snd->bytes_to_send -= bytes_to_send;
    else
      snd->bytes_to_send = 0;
  }

  snd->FD.type = fr_type;
  snd->FD.len = (U8)bytes_to_send;

  while (bytes_to_send)
  {
    UBYTE bytes_to_read = MINIMUM(snd->seq_buflen - snd->seq_bufpos, bytes_to_send);

    memcpy(&snd->FD.buf[sdu_buf_pos], &snd->seq_buf[snd->seq_bufpos], bytes_to_read);
    sdu_buf_pos += bytes_to_read;
    snd->seq_bufpos += bytes_to_read;        /* reset pos. in sequence  */

    if (bytes_to_send > bytes_to_read)
      bytes_to_send -= bytes_to_read;
    else
      bytes_to_send = 0;
    
    if (snd->seq_bufpos >= snd->seq_buflen)  /* check sequence boundary */
      snd->seq_bufpos -= snd->seq_buflen;
  }

  snd_ra_data_req(&snd->FD);

  return (snd->bytes_to_send);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_DurationToBytes |
+--------------------------------------------------------------------+

  PURPOSE : convert duration of sequence to be sent to bytes, taking
            account of the transmission rate set

*/

GLOBAL USHORT snd_DurationToBytes (USHORT trans_rate, USHORT duration)
{

  TRACE_FUNCTION ("snd_DurationToBytes()");

  return (trans_rate * duration / (8 * 1000));

}

#ifdef _SIMULATION_
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : SetBcsFrameBuffer   |
+--------------------------------------------------------------------+

  PURPOSE : copy HDLC data for buffer sending - add BCS-REC ident & repeats

*/

LOCAL void SetBcsFrameBuffer (UBYTE *bcs_buf, UBYTE *hdlc_buf, USHORT len, USHORT repeat)
{
USHORT i, j, k;

  TRACE_FUNCTION ("SetBcsFrameBuffer()");

  for (i = 0; i < len; hdlc_buf++)
  {
    for (j = 0; j < repeat; j++)
      for (k = 0; k < HDLC_REPEAT; i++, k++, bcs_buf++)
        if (i & 0x0001)
          *bcs_buf = *hdlc_buf;
        else
          *bcs_buf = IDENT_BCS_REC;
  }

}

#endif

/*
+-----------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF               |
| STATE   : code                       ROUTINE : snd_BuildStatusFrames  |
+-----------------------------------------------------------------------+

  PURPOSE : build BCS frames from HDLC data received from T30
            pass to SBM buffer

*/

#ifdef _SIMULATION_
GLOBAL void snd_BuildStatusFrames(T_FAD_DATA_REQ *fad_data_req, USHORT max_bytes)
#else
GLOBAL void snd_BuildStatusFrames(T_FAD_DATA_REQ *fad_data_req)
#endif
{
  T_SND *snd = &fad_data->snd;

#ifdef _SIMULATION_
  USHORT repeat;

  TRACE_FUNCTION ("snd_BuildStatusFrames()");

  snd->final = fad_data_req->final;

  /* set total bytes to be sent */
  
  repeat = (USHORT)(snd->trans_rate / (BCS_RATE << 3));
  snd->data_to_send = fad_data_req->sdu.l_buf * HDLC_REPEAT * repeat;

  if (snd->data_to_send <= max_bytes)
	  fad_data_req->sdu.l_buf = 0;
  else
  {
    snd->data_to_send = max_bytes;
	  fad_data_req->sdu.l_buf -= (snd->data_to_send / HDLC_REPEAT / repeat);
  }

  SetBcsFrameBuffer (&snd->seq_buf [0], &fad_data_req->sdu.buf [fad_data_req->sdu.o_buf], snd->data_to_send, repeat);

  snd->data_bufpos = 0;
  if (fad_data_req->sdu.l_buf)
    fad_data_req->sdu.o_buf += (snd->data_to_send / HDLC_REPEAT / repeat);

#else /* _TARGET_ */

   snd->data_to_send = fad_data_req->sdu.l_buf >> 3;
   fad_data_req->sdu.l_buf = 0;
   snd->data_bufpos = 0;

#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE :  snd_StoreMsgData   |
+--------------------------------------------------------------------+

  PURPOSE : store MSG data and pass to SBM buffer

*/

GLOBAL void snd_StoreMsgData (T_FAD_DATA_REQ *fad_data_req)
{
  T_SND *snd = &fad_data->snd;

  TRACE_FUNCTION ("snd_StoreMsgData()");

  snd->final = fad_data_req->final;

  if (fad_data_req->sdu.l_buf)
  {
    snd->data_bufpos += (fad_data_req->sdu.l_buf >> 3);
    sbm_store_prim (fad_data_req);   /* pass primitive to send buffer */
  }
  if (!snd->final)
  {
    if (snd->data_to_send < snd->threshold)
      sig_snd_ker_ready_ind ();
    else
      snd->send_ready_to_t30 = TRUE;
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_SendMsgData     |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL BOOL snd_SendMsgData (UBYTE req_frames)
{
  T_SND *snd = &fad_data->snd;
  T_FRAME_DESC ul_FD;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("snd_SendMsgData()");
#endif

  if (sbm_get_frame(&ul_FD, req_frames) EQ TRUE)
  {
    U8 sdu_len = (U8)(ul_FD.Len[0] + ul_FD.Len[1]);

    snd->data_to_send -= sdu_len;
    snd->FD.type = FR_T4DATA;
    snd->FD.len = sdu_len;
    memcpy(snd->FD.buf, ul_FD.Adr[0], ul_FD.Len[0]);
    memcpy(&snd->FD.buf[ul_FD.Len[0]], ul_FD.Adr[1], ul_FD.Len[1]);
    snd_ra_data_req(&snd->FD);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDF            |
| STATE   : code                       ROUTINE : snd_SendBcsData     |
+--------------------------------------------------------------------+

  PURPOSE :

*/
#ifdef _SIMULATION_
GLOBAL void snd_SendBcsData (USHORT bytes_to_send)
#else
GLOBAL void snd_SendBcsData (void)
#endif
{
  T_SND *snd = &fad_data->snd;

  TRACE_FUNCTION ("snd_SendBcsData()");

  if (!snd->data_to_send)
    return;

  snd->FD.type = FR_STATUS;

#ifdef _SIMULATION_

  if (snd->data_to_send >= bytes_to_send)
    snd->data_to_send -= bytes_to_send;
  else
  {
    bytes_to_send = snd->data_to_send;
    snd->data_to_send = 0;
  }
  snd->FD.len = (U8)bytes_to_send;
  memcpy(snd->FD.buf, &snd->seq_buf[snd->data_bufpos], snd->FD.len);
  snd->data_bufpos += bytes_to_send; 

#else /* _TARGET_ */
  snd->FD.buf[0] = IDENT_BCS_REC;
  snd->FD.buf[1] = snd->fad_data_req->sdu.buf[snd->data_bufpos++];
  snd->FD.len = 2;
  snd->data_to_send--;

#endif /* _TARGET_ */

  snd_ra_data_req(&snd->FD);
}

