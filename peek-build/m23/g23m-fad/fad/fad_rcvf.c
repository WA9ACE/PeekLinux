/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_RCVF
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
|             the component Fax Adaptation 3.45 of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef FAD_RCVF_C
#define FAD_RCVF_C
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

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_init            |
+--------------------------------------------------------------------+

  PURPOSE : initialise the fad data for the rcv formatter process

*/

GLOBAL void rcv_init (void)
{
  T_RCV *rcv = &fad_data->rcv;

  TRACE_FUNCTION ("rcv_init()");

  INIT_STATE (RCV, SYNC_RCV);
  INIT_STATE (RCVSUB, SUB_IDLE);

  rcv->data_in_rbm       = FALSE;
  rcv->preamble_pending  = FALSE;
  rcv->bcs_pending       = TRUE;
  rcv->FlagFound         = 0;
  rcv->train_flg         = FALSE; /*???*/

  rcv_ResetFormatter ();
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_ResetFormatter  |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL void rcv_ResetFormatter (void)
{
  T_RCV *rcv = &fad_data->rcv;

  TRACE_FUNCTION ("rcv_ResetFormatter()");

  SET_STATE (RCVSUB, SUB_IDLE);

  rcv->bytes_good   = 0;
  rcv->bytes_expect = 0;
  rcv->bytes_rcvd   = 0;
  rcv->prim_ready   = FALSE;
  rcv->hdlc_len     = 0;
  rcv->bcs_data_len = 0;
  rcv->data_bufpos  = 0;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : CheckBcsFrame       |
+--------------------------------------------------------------------+

  PURPOSE : get number of correct IDENT and data bytes (BCS data)

*/

LOCAL void CheckBcsFrame (UBYTE *ident_ratio, UBYTE *info_ratio,
                          UBYTE *bcs_data, UBYTE ident,
                          UBYTE info, USHORT bcs_len)
{
  UBYTE i, pream_good = 0, ident_good = 0;

  TRACE_FUNCTION ("CheckBcsFrame()");

  if (!bcs_len)
    return;

  for (i = 0; i < bcs_len; bcs_data++, i++)
  {
    if (i & 0x01)
    {
      if (*bcs_data EQ info)
        pream_good++;
    }
    else
    {
      if (*bcs_data EQ ident)
        ident_good++;
    }
  }
  bcs_len >>= 1;

  if (bcs_len)
  {
    *ident_ratio = (ident_good * 100) / bcs_len;
    *info_ratio  = (pream_good * 100) / bcs_len;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_FrameType       |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL UBYTE rcv_FrameType(T_FD *pFD)
{
UBYTE *bcs_data, fr_type, len, ident_ratio, info_ratio;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("rcv_FrameType()");
#endif

  fr_type   = pFD->type;
  len       = pFD->len;
  bcs_data  = pFD->buf;

  switch (GET_STATE (RCV))
  {
    case SYNC_RCV:
      switch (fr_type)
      {
        case FR_STATUS:
          CheckBcsFrame(&ident_ratio, &info_ratio, bcs_data, IDENT_BCS_REC, HDLC_FLAG, len);
          if (ident_ratio < BCS_TOLERANCE)
            return (FT_NONE);                 /* not a BCS frame */
          if (info_ratio >= BCS_TOLERANCE)
            return (FT_PREAMBLE);
          break;

        case FR_TRAIN:
          CheckBcsFrame(&ident_ratio, &info_ratio, bcs_data, IDENT_MSG_REC, VAL_TRAIN, len);
          if (ident_ratio >= TRAIN_TOLERANCE)
            break;                            /* STATUS-TRAIN frame */

          CheckBcsFrame(&ident_ratio, &info_ratio, bcs_data, IDENT_MSG_TRA, VAL_TRAIN, len);
          if (ident_ratio >= TRAIN_TOLERANCE)
            return (FT_TRAIN_RESPONSE);       /* response to TRAINING */

          return (FT_NONE);
      }
      break;

    case PREAM_RCV:
      switch (fr_type)
      {
        case FR_STATUS:
          CheckBcsFrame(&ident_ratio, &info_ratio, bcs_data, IDENT_BCS_REC, VAL_TCF, len);
          if (ident_ratio < BCS_TOLERANCE)
            return (FT_NONE);                 /* not a BCS frame */
          if (info_ratio >= BCS_TOLERANCE)
            return (FT_PREAMBLE);
          break;
        
        default:
          return (FT_NONE);
      }
      break;

    case BCS_RCV:
      if (fr_type EQ FR_STATUS)
      {
        CheckBcsFrame(&ident_ratio, &info_ratio, bcs_data, IDENT_BCS_REC, HDLC_FLAG, len);
        if (ident_ratio < BCS_TOLERANCE)
          return (FT_NONE);                 /* not a BCS frame */

        if (GET_STATE (RCVSUB) EQ SUB_PREAMBLE)
        {
          if (info_ratio EQ 100)
            return (FT_NONE);               /* still receiving PREAMBLE  */

         SET_STATE (RCVSUB, SUB_DATA);
        }
      }
      break;

    default:
      break;
  }
  return (fr_type);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_SetHdlcFrame    |
+--------------------------------------------------------------------+

  PURPOSE : set BCS data as HDLC frame for T30/ACI

*/

GLOBAL void rcv_SetHdlcFrame (T_FD *pFD, UBYTE *hdlc_data, USHORT *hdlc_len)
{
  UBYTE *bcs_data = pFD->buf;
  USHORT flag_found = 0, i, k;

#define HDLC_MAX 2

#ifdef _SIMULATION_
  TRACE_FUNCTION ("rcv_SetHdlcFrame()");
#endif

  for (i = 0; i < pFD->len; i++, bcs_data++)
  {
    if (i & 0x0001)
    {
      *hdlc_data++ = *bcs_data;
      (*hdlc_len)++;

      /*
       * if preamble is disturbed
       * discard erroneous HDLC data
       */
      if (*hdlc_len > HDLC_MAX) 
      {
        /*
         * check last HDLC data; allow trailing HDLC flags
         */
        for (k = *hdlc_len - HDLC_MAX; k < *hdlc_len; k++)
        {
          if (fad_data->rcv.hdlc[k] EQ HDLC_FLAG)
          {
            flag_found++;
            TRACE_EVENT("HDLC flags during SUB_DATA found");
          }
          if (flag_found EQ HDLC_MAX)
          {
            TRACE_EVENT("HDLC buffer cleared again");
            hdlc_data = fad_data->rcv.hdlc;
            *hdlc_len = 0;
          }
        }
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_SetErrorRatio   |
+--------------------------------------------------------------------+

  PURPOSE : calculate no. of correctly received TCF bytes

*/

GLOBAL void rcv_SetErrorRatio(T_FD *pFD)
{
  void t30_ker_debug(CHAR *header, UBYTE *buf, USHORT len);

  UBYTE *buf = pFD->buf, i;

  TRACE_FUNCTION ("rcv_SetErrorRatio()");

#if defined TRACE_FAD_DL_TCF || defined _SIMULATION_
  t30_ker_debug("TCF", buf, pFD->len);
#endif

  for (i = 0; i < pFD->len; i++, buf++)
  {
    if (*buf EQ VAL_TCF)
    {
      fad_data->rcv.bytes_good++;
    }
  }
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_StoreMsgData    |
+--------------------------------------------------------------------+

  PURPOSE : pass SDU to Receive Buffer Manager (RBM)

*/

GLOBAL BOOL rcv_StoreMsgData (T_FD *pFD, BOOL *primAvail)
{
  TRACE_FUNCTION ("rcv_StoreMsgData()");

  return rbm_store_frames(pFD, primAvail);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_SetFinalBuffer  |
+--------------------------------------------------------------------+

  PURPOSE : set RBM for final MSG data

*/

GLOBAL void rcv_SetFinalBuffer (void)
{
  T_FAD_DATA_IND *fad_data_ind;

  TRACE_FUNCTION ("rcv_SetFinalBuffer()");

  rcv_GetMsgData (&fad_data_ind, FALSE);
  sig_rcv_ker_msg_ind (fad_data_ind);
  fad_data->rcv.t30_req_frames = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVF            |
| STATE   : code                       ROUTINE : rcv_GetMsgData      |
+--------------------------------------------------------------------+

  PURPOSE :

*/

GLOBAL BOOL rcv_GetMsgData (T_FAD_DATA_IND **fad_data_ind, BOOL data_incoming)
{
  TRACE_FUNCTION ("rcv_GetMsgData()");

  *fad_data_ind = rbm_get_prim ();

  if (data_incoming)
  {
    /* MSG data still being received */
    if (*fad_data_ind EQ NULL)
      return FALSE;

    (*fad_data_ind)->final = FALSE;
  }
  else
  {
    /* MSG data no longer being received - SYNC detected */

    if (*fad_data_ind NEQ NULL)
      (*fad_data_ind)->final = FALSE;
    else
    {
      *fad_data_ind = rbm_get_curr_prim ();

       /* buffer empty - end of MSG phase */
      fad_data->rcv.data_in_rbm = FALSE;

      if (*fad_data_ind EQ NULL)
        return FALSE;

      (*fad_data_ind)->final = TRUE;
    }
  }
  return (TRUE);
}

