/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_SNDP
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

#ifndef FAD_SNDP_C
#define FAD_SNDP_C
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
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_SNDP            |
| STATE   : code                       ROUTINE : snd_ra_ready_ind    |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive RA_READY_IND received from RA.

*/

GLOBAL void snd_ra_ready_ind (T_RA_READY_IND *ra_ready_ind)
{
  USHORT bytes_to_send;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("snd_ra_ready_ind()");
  PACCESS        (ra_ready_ind);
#endif

  if (GET_STATE(KER) EQ KER_NULL)
  {
#ifdef _SIMULATION_
    PFREE (ra_ready_ind);
#endif
    return;
  }

  bytes_to_send = ra_ready_ind->req_frames * FRAME_SIZE;

  if ((GET_STATE (SND) NEQ SYNC_SND) AND fad_data->snd.reset)
  {
    snd_SendReset (bytes_to_send);
#ifdef _SIMULATION_
    PFREE (ra_ready_ind);
#endif
    return;
  }

  switch (GET_STATE (SND))
  {
    case SYNC_SND:
      sbm_free_empty_prims();
      snd_SendSequence (bytes_to_send, FR_SYNC);
      break;

    case PREAM_SND:
      if (!snd_SendSequence(bytes_to_send, FR_STATUS))
      {
/*
        TRACE_EVENT ("BCS-REC:PREAM sent - start HDLC");
*/
        SET_STATE (SNDSUB, SUB_DATA);
#ifdef _SIMULATION_
        snd_BuildStatusFrames(fad_data->snd.fad_data_req, BCS_BUF_LEN);
#else
        snd_BuildStatusFrames(fad_data->snd.fad_data_req);
#endif
        SET_STATE (SND, BCS_SND);
      }
      break;

    case BCS_SND:
      if (GET_STATE (SNDSUB) NEQ SUB_DATA)
        break;

#ifdef _SIMULATION_
      snd_SendBcsData(bytes_to_send);
#else
      snd_SendBcsData();
#endif
      if (!fad_data->snd.data_to_send)
      {
        if (fad_data->snd.fad_data_req->sdu.l_buf)
        {
#ifdef _SIMULATION_
          snd_BuildStatusFrames(fad_data->snd.fad_data_req, BCS_BUF_LEN);
#else
          snd_BuildStatusFrames(fad_data->snd.fad_data_req);
#endif
        }
        else
        {
/*
          TRACE_EVENT ("BCS-REC:HDLC sent");
*/
          SET_STATE (SNDSUB, SUB_IDLE);
          PFREE (fad_data->snd.fad_data_req);
          fad_data->snd.fad_data_req = NULL;
          sig_snd_ker_data_sent_ind ();
        }
      }
      break;

    case TCF_SND:
      if (!(snd_SendSequence(bytes_to_send, FR_T4DATA)))
      {
        sig_snd_ker_data_sent_ind ();
/*
        TRACE_EVENT ("MSG-REC:TCF sent");
*/
      }
      break;

    case TRAIN_SND:
    case TRAIN_CNF_SND:
      snd_SendSequence (bytes_to_send, FR_TRAIN);
      break;

    case MSG_SND:
      sbm_free_empty_prims();

      if (GET_STATE (SNDSUB) EQ SUB_IDLE)   /* 1st MSG block < threshold */
      {
        if (!fad_data->snd.final)
          if (fad_data->snd.data_to_send < fad_data->snd.threshold)
          {
            snd_SendSequence (bytes_to_send, FR_TRAIN);
            break;
          }
        SET_STATE (SNDSUB, SUB_DATA);
      }
      if (fad_data->snd.final)
      {
        if (!snd_SendMsgData (ra_ready_ind->req_frames))
        {
          TRACE_EVENT ("MSG-REC:DATA sent");
          SET_STATE (SNDSUB, SUB_IDLE);
          sig_snd_ker_data_sent_ind ();
#ifdef _SIMULATION_
          snd_SendSequence(bytes_to_send, FR_SYNC);
#endif
        }
      }
      else
      {
        /* FINAL flag not set */

        if (!snd_SendMsgData (ra_ready_ind->req_frames))
        {
          TRACE_EVENT ("MSG-REC:DATA buffer underflow");
          sig_rcv_ker_error_ind (ERR_LOCAL_DATA_MSG_UFL);
        }
        if (    fad_data->snd.send_ready_to_t30
            AND fad_data->snd.data_to_send < fad_data->snd.threshold)
          {
            sig_snd_ker_ready_ind ();
            fad_data->snd.send_ready_to_t30 = FALSE;
          }
      }
      break;

    default:
      break;
  }

#ifdef _SIMULATION_
  PFREE (ra_ready_ind);
#endif
}
