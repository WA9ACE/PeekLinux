/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_RCVS
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

#ifndef FAD_RCVS_C
#define FAD_RCVS_C
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
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVS                 |
| STATE   : code                       ROUTINE : sig_ker_rcv_await_tcf_req|
+-------------------------------------------------------------------------+

  PURPOSE : Process signal AWAIT_TCF_REQ received from KERNEL process

*/

GLOBAL void sig_ker_rcv_await_tcf_req (void)
{
  TRACE_FUNCTION ("sig_ker_rcv_await_tcf_req()");

  if (GET_STATE (RCV) EQ SYNC_RCV)
  {
    fad_data->rcv.bytes_expect = snd_DurationToBytes(fad_data->snd.trans_rate, TIME_AWAIT_TCF);
    SET_STATE (RCVSUB, SUB_IDLE);
    SET_STATE (RCV, TCF_RCV);
  }
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVS                 |
| STATE   : code                       ROUTINE : sig_ker_rcv_ready_req    |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal READY_REQ received from KERNEL process

*/

GLOBAL void sig_ker_rcv_ready_req (void)
{
  T_RCV *rcv = &fad_data->rcv;
  T_FAD_DATA_IND *fad_data_ind;

  TRACE_FUNCTION ("sig_ker_rcv_ready_req()");

  rcv->t30_req_frames = TRUE;
  if (rcv->prim_ready OR !rcv->data_incoming)
  {
    if (rcv_GetMsgData (&fad_data_ind, rcv->data_incoming))
    {
      if (fad_data_ind->final)
      {
        if (rcv->data_incoming) /* data incoming - buffer empty */
        {                               
          sig_rcv_ker_error_ind (ERR_EMPTY_BUFFER);
          rcv->t30_req_frames = FALSE;
          rcv_ResetFormatter ();
          SET_STATE (RCV, SYNC_RCV);    
          return;
        }
        else
        {
          rcv_ResetFormatter ();
          SET_STATE (RCV, SYNC_RCV);
        }
      }
      sig_rcv_ker_msg_ind (fad_data_ind);
      rcv->t30_req_frames = FALSE;

      if (rcv->preamble_pending AND !rcv->data_in_rbm)
      {
        /* TRACE_EVENT ("pend FAD_MUX_IND to T30"); */
        sig_rcv_ker_preamble_ind (); /**/
      }
      if (rcv->bcs_pending AND !rcv->data_in_rbm)
      {
        /* TRACE_EVENT ("pend FAD_DATA_IND (BCS) to T30"); */
        sig_rcv_ker_bcs_ind (rcv->hdlc, rcv->hdlc_len);
      }
    }
  }
}

/*
+-------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_RCVS                 |
| STATE   : code                       ROUTINE : sig_ker_rcv_reset_req    |
+-------------------------------------------------------------------------+

  PURPOSE : Process signal AWAIT_REQ received from KERNEL process

*/

GLOBAL void sig_ker_rcv_reset_req (void)
{

  TRACE_FUNCTION ("sig_ker_rcv_reset_req()");

  rcv_ResetFormatter ();
  SET_STATE (RCV, SYNC_RCV);

}

