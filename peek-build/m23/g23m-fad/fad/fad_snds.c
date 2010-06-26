/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_SNDS
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

#ifndef FAD_SNDS_C
#define FAD_SNDS_C
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
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_activate_req |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal ACTIVATE_REQ received from process KER

*/

GLOBAL void sig_ker_snd_activate_req (T_FAD_ACTIVATE_REQ *fad_activate_req)
{
  T_SND *snd = &fad_data->snd;

  snd->trans_rate      = fad_activate_req->trans_rate;
  snd->frames_per_prim = fad_activate_req->frames_per_prim;

  /* T.32: 5-sec. buffer */
#ifdef _SIMULATION_
  snd->threshold = snd_DurationToBytes(snd->trans_rate, TIME_THRESHOLD);
#else
  snd->threshold = snd_DurationToBytes(snd->trans_rate, TIME_THRESHOLD_TI);
#endif
}
/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_sync_req     |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal SYNC_REQ received from process KER

*/

GLOBAL void sig_ker_snd_sync_req (BOOL reset)
{
  T_SND *snd = &fad_data->snd;
  USHORT bytes_to_send;

  TRACE_FUNCTION ("sig_ker_snd_sync_req()");

/*
  TRACE_EVENT ("IDLE:send SYNCs");
*/
  if (reset)				  /* send SYNC frames following return to IDLE */
    snd->reset = SYNC_RESET;

  snd->bytes_to_send = REPEAT_FOREVER;
  snd->seq_bufpos    = 0;
  snd->seq_buflen    = 0;
  snd_SetSequence(snd->seq_buf, &snd->seq_buflen, (UBYTE *)FR_SEQ_SYNC, LEN_SEQ_SYNC, 3);

  INIT_STATE (SND, SYNC_SND);
  INIT_STATE (SNDSUB, SUB_IDLE);

  if (snd->ra_req_frames)
  {
    bytes_to_send = snd->ra_req_frames * FRAME_SIZE;
    snd_SendSequence(bytes_to_send, FR_SYNC);
    snd->ra_req_frames = 0;
  }

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_train_req    |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal TRAIN_REQ received from process KER

*/

GLOBAL void sig_ker_snd_train_req (T_FAD_DATA_REQ *fad_data_req, BOOL send_fax)
{
  T_SND *snd = &fad_data->snd;

  TRACE_FUNCTION ("sig_ker_snd_train_req()");
/*
  TRACE_EVENT ("MSG-REC:start TRAIN");
*/
  if (GET_STATE (SND) EQ SYNC_SND)
  {
    snd->bytes_to_send = REPEAT_FOREVER;
    snd->seq_buflen    = SND_FRAME_LEN * MAX_SND_FRAMES;
    snd->send_status   = TRUE;
    snd->seq_bufpos    = 0;

    SET_STATE (SNDSUB, SUB_IDLE);

    if (send_fax)
    {
      snd->data_to_send = fad_data_req->sdu.l_buf >> 3;
      snd_StoreMsgData (fad_data_req);
      SET_STATE (SND, TRAIN_SND);
    }
    else
      SET_STATE (SND, TRAIN_CNF_SND);

  }
}


/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_tcf_req      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal SND_TCF_REQ received from process KER

*/

GLOBAL void sig_ker_snd_tcf_req (void)
{
  TRACE_FUNCTION ("sig_ker_snd_tcf_req()");
/*
  TRACE_EVENT ("MSG-REC:start TCF");
*/
  if (GET_STATE (SND) EQ SYNC_SND)
  {
    fad_data->snd.seq_bufpos = 0;
    SET_STATE (SND, TCF_SND);
  }
}


/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_preamble_req |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal SND_PREAMBLE_REQ received from process KER
            - store fad_data_req
            - prepare preamble
*/

GLOBAL void sig_ker_snd_preamble_req (T_FAD_DATA_REQ *fad_data_req)
{
  T_SND *snd = &fad_data->snd;

  TRACE_FUNCTION ("sig_ker_snd_preamble_req()");

  if (GET_STATE (SND) EQ SYNC_SND)
  {
    snd->fad_data_req = fad_data_req;

#ifdef _SIMULATION_
    snd->fad_data_req->sdu.l_buf >>= 3; /* set SDU byte len for    */
    snd->fad_data_req->sdu.o_buf >>= 3; /* snd_BuildStatusFrames() */
#endif

    SET_STATE (SND, PREAM_SND);
  }
}


/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_bcs_req      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal BCS_REQ received from process KER

*/

GLOBAL void sig_ker_snd_bcs_req (T_FAD_DATA_REQ *fad_data_req)
{
  TRACE_FUNCTION ("sig_ker_snd_bcs_req()");

  if (GET_STATE (SND) EQ BCS_SND)
  {
#ifdef _SIMULATION_
    snd_BuildStatusFrames(fad_data->snd.fad_data_req, BCS_BUF_LEN);
#else
    snd_BuildStatusFrames(fad_data->snd.fad_data_req);
#endif
    SET_STATE (SNDSUB, SUB_DATA);
  }
}

/*
+------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                |
| STATE   : code                       ROUTINE : sig_ker_snd_status_req  |
+------------------------------------------------------------------------+

  PURPOSE : Process signal STATUS_REQ received from process KER

*/

GLOBAL void sig_ker_snd_status_req (void)
{
  TRACE_FUNCTION ("sig_ker_snd_status_req()");

  if (GET_STATE (SND) EQ TRAIN_SND)
  {
    TRACE_EVENT ("MSG-REC:start C");

    SET_STATE (SND, MSG_SND);
    SET_STATE (SNDSUB, SUB_IDLE);
  }

}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDS                 |
| STATE   : code                       ROUTINE : sig_ker_snd_msg_req      |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal MSG_REQ received from process KER

*/

GLOBAL void sig_ker_snd_msg_req (T_FAD_DATA_REQ *fad_data_req)
{
  TRACE_FUNCTION ("sig_ker_snd_msg_req()");

/* T.32: 5-sec. buffer */
  fad_data->snd.data_to_send += (fad_data_req->sdu.l_buf >> 3);
  snd_StoreMsgData (fad_data_req);
}
