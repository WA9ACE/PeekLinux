/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_bcsf
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
|             the component T30 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_BCSF_C
#define T30_BCSF_C
#endif

#define ENTITY_T30

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_t30.h"
#include "mon_t30.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */
#include "t30.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/


LOCAL const USHORT crctab[256] =
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_build_report    |
+--------------------------------------------------------------------+

  PURPOSE : build the T30_REPORT_IND primitive out of a send/rec. frame

*/

LOCAL void bcs_build_report(UBYTE *frame, USHORT len, T_T30_REPORT_IND *t30_report_ind)
{
  if (t30_data->bitorder & FBO_REV_STATUS) /* reverse the bitorder of each byte */
  {
    memcpy(t30_report_ind->sdu.buf, frame, len);
  }
  else
  {
    USHORT i;
    for (i=0; i<len; i++)
      t30_report_ind->sdu.buf[i] = BIT_MIRROR[frame[i]];
  }

  t30_report_ind->sdu.l_buf = (len << 3);
  t30_report_ind->sdu.o_buf = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialize BCS formatter

*/

GLOBAL void bcs_init(T_T30_DATA *pt30_data)
{
  TRACE_FUNCTION ("bcs_init()");
  memset (pt30_data->bcs_frm, 0, BCS_FRM_SIZE);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_check_frames    |
+--------------------------------------------------------------------+

  PURPOSE : This function checks the final flag and the frame checking sequence.
*/

LOCAL UBYTE bcs_check_frames(UBYTE *idx_max)
{

  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;
  UBYTE idx;

  for (idx = 0; idx < *idx_max; idx++)
  {
#ifdef _SIMULATION_ /* FCS generating/checking off */
    if (t30_data->test_mode & TST_FCS)
    {
    }
    else
#endif
    {
      USHORT crctt = 0xffff; /* initialize crc */

      /* shift each bit through polynomial */
      USHORT pos;
      for (pos = t30_data->frm[idx].beg; pos <= t30_data->frm[idx].end; pos++)
      {
        SHORT tmp = (crctt >> 8) ^ sdu->buf[pos];
        crctt = ((crctt << 8) ^ crctab[tmp & 0xff]) & 0xffff;
      }
      if (crctt NEQ 0x1D0F) /* check remainder */
        return CHK_FCS_ERR;
    }
#if defined _SIMULATION_ || defined KER_DEBUG_BCS
    ker_debug ("BCS_DATA_IND", (UBYTE*)&sdu->buf[t30_data->frm[idx].beg], (USHORT)(t30_data->frm[idx].end-t30_data->frm[idx].beg));
#endif
    /* count final flags */
    if (sdu->buf[t30_data->frm[idx].beg + 1] & 0x08)
    {
      *idx_max = (UBYTE)idx + 1;
      return CHK_OK;
    }
  }
#ifdef _SIMULATION_ /* Control checking OFF */
  if (t30_data->test_mode & TST_CTRL)
    return CHK_OK;
#endif
  return CHK_FIN_ERR;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_frame_pos       |
+--------------------------------------------------------------------+

  PURPOSE : This function extracts
            the beginning, the end and the length of each frame.
*/

LOCAL UBYTE bcs_frame_pos(UBYTE *flag_vector, UBYTE *idx_max)
{
  USHORT idx = 0;
  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;

  *idx_max = 0;
  while (idx < sdu->l_buf)
  {
    /* skip flags */
    while (idx < sdu->l_buf AND flag_vector[idx])
      idx++;

    /* ready if last byte is a flag */
    if (idx EQ sdu->l_buf)
      return FRM_OK;

    /* save begin of frame */
    t30_data->frm[*idx_max].beg = idx;

    /* search flag */
    while (idx < sdu->l_buf AND !flag_vector[idx])
      idx++;

    /* error if no flag found */
    if (idx EQ sdu->l_buf)
    {
      t30_data->frm[*idx_max].beg = 0;
      return FRM_OK;  /* return FRM_ERR_NO_FLAG; ??? */
    }

    /* save end of fame and length of frame */
    t30_data->frm[*idx_max].end = idx - 1;
    t30_data->frm[*idx_max].len = idx - t30_data->frm[*idx_max].beg;

    /* error if too many frames received */
    if (++(*idx_max) EQ FRAMES_MAX)
      return FRM_ERR_TOO_MANY_FRAMES;

    /* next frame */
    idx++;
  }
  return FRM_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_report_rcv      |
+--------------------------------------------------------------------+

  PURPOSE : This function reports the received HDLC frames to MMI
            if desired.
*/

LOCAL void bcs_report_rcv(UBYTE idx_max)
{
  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;
  USHORT idx;

  for (idx = 0 ; idx < idx_max ; idx++)
  {
    if (t30_data->frm[idx].len >= 2)
    {
      PALLOC_SDU (t30_report_ind, T30_REPORT_IND, REPORT_SIZE_BITS);
      bcs_build_report (&sdu->buf[t30_data->frm[idx].beg], (USHORT)(t30_data->frm[idx].len - 2), t30_report_ind);
      t30_report_ind->dir = DIR_RCV;
      PSENDX (MMI, t30_report_ind);
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_decode          |
+--------------------------------------------------------------------+

  PURPOSE : This function decodes each hdlc frame and sends the
            decoded data to the kernel.
*/

/* this function is only necessary because of an iarm compiler failure */
LOCAL void bcs_clear_flag (UBYTE *ptr)
{
  if (*ptr & 0x70)
    *ptr &= 0x7F;
}

LOCAL UBYTE bcs_decode(UBYTE idx_max)
{
  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;
  UBYTE idx;

  for (idx = 0; idx < idx_max; idx++)
  {
    USHORT begin = t30_data->frm[idx].beg + 2;

    /* set direction flag to zero for CCD decoding */
    bcs_clear_flag (sdu->buf + begin);

    /* set offset and length of current frame */
    sdu->o_buf = begin << 3;
    sdu->l_buf = ((USHORT)(t30_data->frm[idx].len - 4) << 3);

#ifdef _SIMULATION_ /* show contents of buffer */
    if (t30_data->test_mode /* & TST_BUF */)
    {
      ker_debug ("ccd_decode", &sdu->buf[begin], (USHORT)(sdu->l_buf >> 3));
    }
#endif

    if (ccd_decodeMsg(CCDENT_T30, DOWNLINK, (T_MSGBUF*)sdu, _decodedMsg, HDLC_ADDR) EQ ccdError)
    {
      TRACE_EVENT ("ERROR: ccd_decode");
      return CCD_ERR;
    }
    sig_bcs_ker_bdat_ind(); /* send signal to kernel */
  }
  return CCD_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_destuff         |
+--------------------------------------------------------------------+

  PURPOSE : This function de-stuffs the received BCS data.
*/

LOCAL void bcs_destuff(UBYTE *flag_vector)
{
  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;
  SHORT bcs_tmp_len = 0;
  SHORT ones = 0;
  SHORT bits = 8;
  UBYTE flag = 0;
  UBYTE *bcs_tmp;
  SHORT i;

#ifdef _SIMULATION_ /* bit stuffing/destuffing OFF */
  if (t30_data->test_mode & TST_STUFF)
  {
    for (i = 0; i < sdu->l_buf; i++)
    {
      if (sdu->buf[i] EQ HDLC_FLAG)
        flag_vector[i] = 1;
    }
    if (flag_vector[sdu->l_buf-1] NEQ 1) // trailing HDLC_FLAG is missed
    {
      sdu->l_buf++;
      sdu->buf[sdu->l_buf-1] = HDLC_FLAG;
      flag_vector[sdu->l_buf-1] = 1;
    }
    return;
  }
#endif

  MALLOC(bcs_tmp, sdu->l_buf);
  
  for (i = 0 ; i < sdu->l_buf ; i++)
  {
    UBYTE bit_ptr = 0x80; /* points initially to MSB */

    do /* check each byte for destuffing */
    {
      if (sdu->buf[i] & bit_ptr) /* bit pointed to is 1 */
      {
        ones++;
        bcs_tmp[bcs_tmp_len] = (bcs_tmp[bcs_tmp_len] << 1) | 1;
        flag                 = (flag                 << 1) | 1;

        if (! --bits) /* counter of byte's bits*/
        {
          bcs_tmp_len++;
          bits = 8;
        }
      }
      else /* bit is 0 */
      {
        if (ones != 5)
        {
          bcs_tmp[bcs_tmp_len] <<= 1;
          flag                 <<= 1;

          if (ones EQ 6 && flag EQ HDLC_FLAG)
            flag_vector[bcs_tmp_len] = 1;

          if (! --bits) /* whole byte is scanned */
          {
            bcs_tmp_len++;
            bits = 8;
          }
        }
        ones = 0;
      }
      bit_ptr >>= 1;
    }
    while (bit_ptr);
  }
  /* copy destuffed BCS frame back to sdu */
  memcpy (sdu->buf, bcs_tmp, bcs_tmp_len);
  MFREE(bcs_tmp);

  sdu->l_buf = bcs_tmp_len; /* adjust buffer length */
  if (flag_vector[sdu->l_buf-1] NEQ 1) // trailing HDLC_FLAG is missed
  {
    sdu->l_buf++;
    sdu->buf[sdu->l_buf-1] = HDLC_FLAG;
    flag_vector[sdu->l_buf-1] = 1;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_bcs_decode      |
+--------------------------------------------------------------------+

  PURPOSE : This function gets the frame positions,
            checks the frames,
            reports the contents to MMI if desired,
            decodes the contents and sends the contents to the kernel.
*/

GLOBAL void bcs_bcs_decode(void)
{
  UBYTE *flag_vector, idx_max, ret;

  MALLOC(flag_vector, BCS_FRM_SIZE);
  memset(flag_vector, 0, BCS_FRM_SIZE);
  bcs_destuff(flag_vector);
  ret = bcs_frame_pos(flag_vector, &idx_max); /* determines "idx_max" */
  MFREE(flag_vector);

  switch (ret)
  {
  case FRM_OK:
    ret = bcs_check_frames(&idx_max);
    if (t30_data->hdlc_report)
    {
      bcs_report_rcv(idx_max);
    }
    switch (ret)
    {
    case CHK_OK:
      switch (bcs_decode(idx_max))
      {
      case CCD_OK:
        return;

      case CCD_ERR:
        sig_bcs_ker_err_ind(ERR_CCD_DEC);
        return;
      }
      break; /* dummy */

    case CHK_FCS_ERR:
      sig_bcs_ker_err_ind(ERR_FCS);
      return;

    case CHK_FIN_ERR:
      sig_bcs_ker_err_ind(ERR_FINAL);
      return;
    }
    break;

  case FRM_ERR_NO_FLAG:
    sig_bcs_ker_err_ind(ERR_FRAME_NO_FLAG);
    return;

  case FRM_ERR_TOO_MANY_FRAMES:
    sig_bcs_ker_err_ind(ERR_FRAME_TOO_MANY_FRAMES);
    return;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_report_snd      |
+--------------------------------------------------------------------+

  PURPOSE : This function reports the sent HDLC frames to MMI
            if desired.
*/

LOCAL void bcs_report_snd(void)
{
  T_sdu *BCI_stream = (T_sdu*)t30_data->BCI_stream; // Binary Coded Information

  PALLOC_SDU(t30_report_ind, T30_REPORT_IND, REPORT_SIZE_BITS);
  
  bcs_build_report(&BCI_stream->buf[0], (USHORT)((BCI_stream->l_buf >> 3) + 2), t30_report_ind);
  t30_report_ind->dir = DIR_SND;
  PSENDX (MMI, t30_report_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_fcs_gen         |
+--------------------------------------------------------------------+

  PURPOSE : This function generates the frame checking sequence
            for one HDLC frame.
*/

LOCAL void bcs_fcs_gen (void)
{
  T_sdu *sdu = (T_sdu*)t30_data->BCI_stream;
  USHORT end = (sdu->l_buf >> 3) + 2;
  USHORT pos;
  UBYTE i;

#ifdef _SIMULATION_ /* FCS generating/checking OFF */
  if (t30_data->test_mode & TST_FCS)
  {
    pos = end;
    sdu->buf[pos++] = 0x12;
    sdu->buf[pos++] = 0xEF;
  }
  else
#endif
  {
    USHORT crctt = 0xffff;
    for (pos = 0; pos < end; pos++)
    {
      SHORT tmp =  (crctt >> 8) ^ sdu->buf[pos];
      crctt = ((crctt << 8) ^ crctab[tmp & 0xff]) & 0xffff;
    }
    sdu->buf[pos++] = (~(crctt & 0xffff) & 0xffff) >> 8;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    sdu->buf[pos++] = (~(crctt & 0xffff) & 0xff);/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
  }

  sdu->l_buf += 32; /* 4 bytes more */

  for (i = 0; i < HDLC_FLAGS; i++)
  {
    sdu->buf[pos++] = HDLC_FLAG;/*lint !e661 !e662 (Warning -- Possible access/creation of out-of-bounds pointer)*/
    sdu->l_buf += 8;
  }
  sdu->o_buf  = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_stuff           |
+--------------------------------------------------------------------+

  PURPOSE : This function stuffs the BCS data.
*/

LOCAL void bcs_stuff (UBYTE final)
{
  T_sdu *sdu_inp  = (T_sdu*)t30_data->BCI_stream;
  T_sdu *sdu_out = &t30_data->fad_data_req->sdu;
  
  UBYTE *buf_in  = sdu_inp->buf;
  USHORT len_in  = sdu_inp->l_buf >> 3;
  UBYTE *buf_out;
  USHORT len_out = sdu_out->l_buf >> 3;
  USHORT i;

#ifdef _SIMULATION_ /* bit stuffing/destuffing OFF */
  if (t30_data->test_mode & TST_STUFF)
  {
    buf_out = &sdu_out->buf[len_out];
    memcpy (buf_out, buf_in, len_in);
    sdu_out->l_buf = (len_out + len_in) << 3;
    return;
  }
#endif

  buf_out = sdu_out->buf;

  for (i = 0; i < len_in; i++)
  {
    USHORT bit_ptr = 0x80;
    UBYTE stuff = 1;
    /*
    check HDLC_FLAGS
    */
    if (buf_in[i] EQ HDLC_FLAG)
    {
      USHORT k;
      for (k = 0; k < HDLC_FLAGS; k++)
      {
        if (i EQ len_in - k - 1)
        {
          stuff = 0;
          break;
        }
      }
    }
    do
    {
      switch (buf_in[i] & bit_ptr)
      {
      default:
        buf_out[len_out] = (buf_out[len_out] << 1) | 1;
        if (! --t30_data->stuff_bits)
        {
          len_out++;
          t30_data->stuff_bits = 8;
        }
        if (!stuff)
        {
          t30_data->stuff_ones = 0;
          break;
        }
        if (++t30_data->stuff_ones != 5)
          break;
        /*
         * otherwise fall through
         */

      case 0:
        buf_out[len_out] <<= 1;
        t30_data->stuff_ones = 0;

        if (! --t30_data->stuff_bits)
        {
          len_out++;
          t30_data->stuff_bits = 8;
        }
        break;
      }
      bit_ptr >>= 1;
    } while (bit_ptr);
  }

  if (final EQ FINAL_YES AND t30_data->stuff_bits NEQ 8)
  {
    buf_out[len_out] <<= t30_data->stuff_bits;
    len_out++;
  }

  sdu_out->l_buf = len_out << 3;

  if (final EQ FINAL_YES)
  {
    t30_data->stuff_ones = 0;
    t30_data->stuff_bits = 8;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_bcs_encode      |
+--------------------------------------------------------------------+

  PURPOSE : This function encodes the HDLC frames,
            reports the HDLC frames to MMI if desired,
            generates the frame checking sequence and stuffs the bits.
*/

GLOBAL void bcs_bcs_encode(UBYTE ctrl, UBYTE final)
{
  T_sdu *sdu = (T_sdu*)t30_data->BCI_stream;

  switch (sdu->buf[2])/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
  {
    case BCS_DIS:
      TRACE_EVENT ("Send BCS_DIS");
      break;
    case BCS_CSI:
      TRACE_EVENT ("Send BCS_CSI");
      break;
    case BCS_NSF:
      TRACE_EVENT ("Send BCS_NSF");
      break;
    case BCS_DTC:
      TRACE_EVENT ("Send BCS_DTC");
      break;
    case BCS_CIG:
      TRACE_EVENT ("Send BCS_CIG");
      break;
    case BCS_NSC:
      TRACE_EVENT ("Send BCS_NSC");
      break;
    case BCS_PWD_POLL:
      TRACE_EVENT ("Send BCS_PWD_POLL");
      break;
    case BCS_SEP:
      TRACE_EVENT ("Send BCS_SEP");
      break;

    case BCS_CFR:
      TRACE_EVENT ("Send BCS_CFR");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_CRP:
      TRACE_EVENT ("Send BCS_CRP");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_DCN:
      TRACE_EVENT ("Send BCS_DCN");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_DCS:
      TRACE_EVENT ("Send BCS_DCS");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_EOM:
      TRACE_EVENT ("Send BCS_EOM");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_EOP:
      TRACE_EVENT ("Send BCS_EOP");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_FTT:
      TRACE_EVENT ("Send BCS_FTT");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_MCF:
      TRACE_EVENT ("Send BCS_MCF");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_MPS:
      TRACE_EVENT ("Send BCS_MPS");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_NSS:
      TRACE_EVENT ("Send BCS_NSS");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PIN:
      TRACE_EVENT ("Send BCS_PIN");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PIP:
      TRACE_EVENT ("Send BCS_PIP");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PRI_EOM:
      TRACE_EVENT ("Send BCS_PRI_EOM");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PRI_EOP:
      TRACE_EVENT ("Send BCS_PRI_EOP");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PRI_MPS:
      TRACE_EVENT ("Send BCS_PRI_MPS");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_PWD_SND:
      TRACE_EVENT ("Send BCS_PWD_SND");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_RTN:
      TRACE_EVENT ("Send BCS_RTN");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_RTP:
      TRACE_EVENT ("Send BCS_RTP");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_SUB:
      TRACE_EVENT ("Send BCS_SUB");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
    case BCS_TSI:
      TRACE_EVENT ("Send BCS_TSI");
      sdu->buf[2] |= t30_data->dir;/*lint !e415 !e416 (Warning -- access/creation of out-of-bounds pointer)*/
      break;
  }

  sdu->buf[0] = HDLC_ADDR;
  sdu->buf[1] = ctrl;/*lint !e415 (Warning -- access of out-of-bounds pointer)*/

  if (t30_data->hdlc_report)
  {
    bcs_report_snd();
  }
  bcs_fcs_gen();
  bcs_stuff(final);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_BCSF            |
| STATE   : code                       ROUTINE : bcs_fill_bcs_frm    |
+--------------------------------------------------------------------+

  PURPOSE : This function checks if the BCS frame buffer has enough
            space to append the received data.
            If there is enough space the data is appended.
            If not than BCS_FRM_FULL is returned.

*/

GLOBAL UBYTE bcs_fill_bcs_frm (T_FAD_DATA_IND *fad_data_ind)
{
  USHORT data_len = fad_data_ind->sdu.l_buf >> 3;
  T_sdu *sdu = (T_sdu*)t30_data->bcs_frm;

  TRACE_FUNCTION("bcs_fill_bcs_frm()");

  if (sdu->l_buf + data_len < BCS_FRM_SIZE)
  {
    memcpy (&sdu->buf[sdu->l_buf], fad_data_ind->sdu.buf, data_len);
    sdu->l_buf += data_len;
    return ((fad_data_ind->final) ? BCS_FRM_FILLED : BCS_FRM_FILLING);
  }
  else
  {
    sdu->l_buf = BCS_FRM_SIZE;
    return (BCS_FRM_FULL);
  }
}

