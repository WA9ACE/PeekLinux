/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_KERS
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
|             Fax Adaptation 3.45 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef FAD_KERS_C
#define FAD_KERS_C
#endif

#define ENTITY_FAD

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
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

/*==== VAR LOCAL ==================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_snd_ker_ready_ind    |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal READY_IND received from process SND

*/

GLOBAL void sig_snd_ker_ready_ind (void)
{
  TRACE_FUNCTION ("sig_snd_ker_ready_ind()");

  switch (GET_STATE (KER))
  {
    case BCS_REC:
    case MSG_REC:
      {
      PALLOC (fad_ready_ind, FAD_READY_IND);
      PSENDX (T30, fad_ready_ind);
      }
      break;
    default:
      break;
  }

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_snd_ker_data_sent_ind|
+-------------------------------------------------------------------------+

  PURPOSE : Process signal DATA_SENT_IND received from process SND

*/

GLOBAL void sig_snd_ker_data_sent_ind (void)
{
  TRACE_FUNCTION ("sig_snd_ker_data_sent_ind()");

  switch (GET_STATE (KER))
  {
  case BCS_REC:
  case MSG_REC:
    {
    PALLOC (fad_data_cnf, FAD_DATA_CNF);
    PSENDX (T30, fad_data_cnf);
    SET_STATE (KER, IDLE);
    sig_ker_snd_sync_req (TRUE);
    }
    break;
  case MSG_REC_TCF:
    {
    PALLOC (fad_snd_tcf_cnf, FAD_SND_TCF_CNF);
    PSENDX (T30, fad_snd_tcf_cnf);
    SET_STATE (KER, IDLE);
    sig_ker_snd_sync_req (TRUE);
    }
    break;
  case BCS_TRA:
    {
    PALLOC (fad_data_cnf, FAD_DATA_CNF);
    PSENDX (T30, fad_data_cnf);
    /* remain in BCS-TRA state ! - don't reset sequencer ! */
    }
    break;
  }
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_preamble_ind |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal PREAMBLE_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_preamble_ind (void)
{

  TRACE_FUNCTION ("sig_rcv_ker_preamble_ind()");

  switch (GET_STATE (KER))
  {
    case IDLE:
      {
      PALLOC (fad_mux_ind, FAD_MUX_IND);
      fad_mux_ind->mode = MUX_BCS;
      PSENDX (T30, fad_mux_ind);
      }
      SET_STATE (KER, BCS_TRA);
      break;
    case MSG_REC_TCF:         /* non-standard: BCS data received while sending TCF */
      sig_snd_ker_data_sent_ind ();
      TRACE_EVENT ("MSG-REC:BCS rec- arbt TCF");
      {
      PALLOC (fad_mux_ind, FAD_MUX_IND);
      fad_mux_ind->mode = MUX_BCS;
      PSENDX (T30, fad_mux_ind);
      }
      SET_STATE (KER, BCS_TRA);
      break;
    default:                /* discard data - reset RCV to idle */
      sig_ker_rcv_reset_req ();
  }
  fad_data->rcv.preamble_pending = FALSE;
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_tcf_ind      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal TCF_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_tcf_ind(void)
{
  T_RCV *rcv = &fad_data->rcv;

  USHORT err_ratio = 100 * rcv->bytes_good/rcv->bytes_rcvd;

  TRACE_FUNCTION ("sig_rcv_ker_tcf_ind()");

  if (GET_STATE (KER) EQ MSG_TRA_TCF)
  {
    PALLOC (fad_rcv_tcf_cnf, FAD_RCV_TCF_CNF);

    TRACE_EVENT_P1("ratio=%d", err_ratio);
    
    fad_rcv_tcf_cnf->ratio = (UBYTE)err_ratio;
    PSENDX (T30, fad_rcv_tcf_cnf);
    SET_STATE (KER, IDLE);
    sig_ker_snd_sync_req (FALSE);
  }
}

/*
+----------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS              |
| STATE   : code                       ROUTINE : sig_rcv_ker_train_ind |
+----------------------------------------------------------------------+

  PURPOSE : Process signal TRAIN_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_train_ind (void)
{
  TRACE_FUNCTION ("sig_rcv_ker_train_ind()");

  if (GET_STATE (KER) EQ IDLE)
  {
    sig_ker_snd_train_req (0, FALSE);
    {
    PALLOC (fad_mux_ind, FAD_MUX_IND);
    fad_mux_ind->mode = MUX_MSG;
    PSENDX (T30, fad_mux_ind);
    }
    fad_data->ker.forward_data = TRUE;
    SET_STATE (KER, MSG_TRA);
  }
  else    /* discard data - reset RCV to idle */
    sig_ker_rcv_reset_req ();
}

/*
+--------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                  |
| STATE   : code                       ROUTINE : sig_rcv_ker_train_end_ind |
+--------------------------------------------------------------------------+

  PURPOSE : Process signal TRAIN_END_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_train_end_ind (void)
{

  TRACE_FUNCTION ("sig_rcv_ker_train_end_ind()");

  if (GET_STATE (KER) EQ MSG_TRA)
    sig_ker_snd_sync_req (FALSE);

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_bcs_ind      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal BCS_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_bcs_ind (UBYTE *hdlc, USHORT hdlc_len)
{
  TRACE_FUNCTION ("sig_rcv_ker_bcs_ind()");

  if (GET_STATE (KER) EQ BCS_TRA)
  {
    USHORT bit_len = hdlc_len << 3;
    {
    PALLOC_SDU (fad_data_ind, FAD_DATA_IND, bit_len);
    fad_data_ind->final = TRUE;
    fad_data_ind->sdu.l_buf = bit_len;
    fad_data_ind->sdu.o_buf = 0;
    memcpy (fad_data_ind->sdu.buf, hdlc, hdlc_len);
    if (fad_data->snd.fad_data_req)   /* BCS data to send detected while receiving BCS: */
      sig_snd_ker_data_sent_ind ();   /* send pseudo-CNF to T30 before forwarding data */

    PSENDX (T30, fad_data_ind);
    }
    fad_data->rcv.FlagFound = 0;
    SET_STATE (KER, IDLE);
    sig_ker_snd_sync_req (TRUE);
    if (fad_data->snd.fad_data_req)   /* BCS data to send detected while receiving BCS: */
    {                                 /* send stored BCS data */
      SET_STATE (KER, BCS_REC);
      sig_ker_snd_preamble_req (fad_data->snd.fad_data_req);
    }
  }
  fad_data->rcv.bcs_pending = FALSE;
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_msg_ind      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal MSG_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_msg_ind (T_FAD_DATA_IND *fad_data_ind)
{

  TRACE_FUNCTION ("sig_rcv_ker_msg_ind()");

  PACCESS        (fad_data_ind);

  if (GET_STATE (KER) EQ MSG_TRA)
  {
    if (!fad_data_ind)
    {
      sig_ker_snd_sync_req (TRUE);
      SET_STATE (KER, IDLE);
    }
    else
    {
      if (fad_data_ind->final)
      {
        sig_ker_snd_sync_req (TRUE);
        SET_STATE (KER, IDLE);
      }
      if (fad_data->ker.forward_data)
      {
        PSENDX (T30, fad_data_ind);
      }
      else
        PFREE (fad_data_ind);
    }
  }

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_status_ind   |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal STATUS_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_status_ind (void )
{

  TRACE_FUNCTION ("sig_rcv_ker_status_ind()");

  if (GET_STATE (KER) EQ MSG_REC)
    sig_ker_snd_status_req ();

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_KERS                 |
| STATE   : code                       ROUTINE : sig_rcv_ker_error_ind    |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal ERROR_IND received from process RCV

*/

GLOBAL void sig_rcv_ker_error_ind (USHORT cause)
{
  TRACE_FUNCTION ("sig_rcv_ker_error_ind()");

  switch (GET_STATE (KER))
  {
    case KER_NULL:
    case IDLE:
      break;
    case BCS_REC:
    case BCS_TRA:
    case MSG_REC_TCF:
    case MSG_REC:
    case MSG_TRA_TCF:
    case MSG_TRA:
      SET_STATE (KER, IDLE);
      ker_SetError (cause);
      break;
    default:
      break;
  }
}

