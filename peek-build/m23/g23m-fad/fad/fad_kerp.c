/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  FAD_KERP
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

#ifndef FAD_KERP_C
#define FAD_KERP_C
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

#ifdef _TARGET_
  #include "ra_l1int.h"
#endif

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_activate_req |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_ACTIVATE_REQ received from T30.

*/

GLOBAL void ker_fad_activate_req (T_FAD_ACTIVATE_REQ *fad_activate_req)
{
  TRACE_EVENT ("ker_fad_activate_req()");
  PACCESS (fad_activate_req);

  /*******************************/
  /*
   *  Initialize Shared Fax/Data memory
   */

  fad_data = &fad_data_base[0];
  ker_init ();
  snd_init ();
  rcv_init ();

  /*
   *  reset RBM
   */
  rbm_reset ();

  fad_data_magic_num = FAD_DATA_MAGIC_NUM;  /* memory is initialized */

  /*******************************/

  if (GET_STATE (KER) EQ KER_NULL)
  {
    sig_ker_snd_activate_req (fad_activate_req);
    sbm_init (FRAME_SIZE);
    rbm_init (fad_activate_req->frames_per_prim);
    sig_ker_snd_sync_req (FALSE);
    {
    PALLOC (fad_activate_cnf, FAD_ACTIVATE_CNF);
    /*
     * calculate the RX and TX buffersizes and notify it
     * to T30
     */
    fad_activate_cnf->buf_size_rx
      = fad_activate_req->frames_per_prim * FRAME_SIZE * RBM_PQ_MAX_PRIM;
    fad_activate_cnf->buf_size_tx
      = fad_activate_req->frames_per_prim * FRAME_SIZE * SBM_MAX_PRIMS;
    PSENDX (T30, fad_activate_cnf);
    }
    SET_STATE (KER, IDLE);

#ifdef _TARGET_
    {
    T_RA_DATATRANS_REQ RA_datatrans_req;
    l1i_ra_datatrans_req(&RA_datatrans_req);
    }
#endif

  }

  PFREE (fad_activate_req);
}


/*
+----------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP               |
| STATE   : code                      ROUTINE : ker_fad_deactivate_req |
+----------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_DEACTIVATE_REQ received from T30.

*/

GLOBAL void ker_fad_deactivate_req (T_FAD_DEACTIVATE_REQ *fad_deactivate_req)
{
  TRACE_EVENT ("ker_fad_deactivate_req()");

  PACCESS (fad_deactivate_req);

  sig_ker_snd_sync_req (FALSE);
  rbm_deinit(TRUE);
  SET_STATE (KER, KER_NULL);

  {
  PALLOC (fad_deactivate_cnf, FAD_DEACTIVATE_CNF);
  PSENDX (T30, fad_deactivate_cnf);
  }

  PFREE (fad_deactivate_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_modify_req   |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_MODIFY_REQ received from T30.

*/

GLOBAL void ker_fad_modify_req (T_FAD_MODIFY_REQ *fad_modify_req)
{
  TRACE_EVENT ("ker_fad_modify_req()");
  PACCESS (fad_modify_req);

  if (GET_STATE (KER) NEQ KER_NULL)
  {
    fad_data->snd.trans_rate = fad_modify_req->trans_rate;
    fad_data->rcv.FlagFound = 0;
    TRACE_EVENT_P1("MODIFY %d bps", fad_data->snd.trans_rate);
  }

  PFREE (fad_modify_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_data_req     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_DATA_REQ received from T30.

*/

GLOBAL void ker_fad_data_req (T_FAD_DATA_REQ *fad_data_req)
{
  T_SBM *sbm = &fad_data->sbm;
  USHORT err_cause = 0;

#ifdef _SIMULATION_
  TRACE_FUNCTION ("ker_fad_data_req()");
#endif

  PACCESS (fad_data_req);

  switch (GET_STATE (KER))
  {
  case KER_NULL:
    err_cause = ERR_LOCAL_DATA_NULL;
    break;

  case IDLE:
    if (fad_data_req->trans_rate EQ R_BCS)
    {
      SET_STATE (KER, BCS_REC);
      sig_ker_snd_preamble_req (fad_data_req);
    }
    else
    {
      if (fad_data_req->trans_rate EQ R_7200)
      {
        sbm->syncCycle = SYNC_CYCLE_7200;
        sbm->framesUntilSync = (SYNC_CYCLE_7200-1);
      }
      else if (fad_data_req->trans_rate EQ R_12000)
      {
        sbm->syncCycle = SYNC_CYCLE_12000;
        sbm->framesUntilSync = (SYNC_CYCLE_12000-1);
      }

      SET_STATE (KER, MSG_REC);
      sig_ker_snd_train_req (fad_data_req, TRUE);
    }
    break;

  case BCS_REC:
    if (fad_data_req->trans_rate EQ R_BCS)
      sig_ker_snd_bcs_req (fad_data_req);
    else
      err_cause = ERR_LOCAL_MSG_RATE;
    break;

  case MSG_REC_TCF:
    err_cause = ERR_LOCAL_DATA_TCF_SND;
    break;

  case MSG_REC:
    if (fad_data_req->trans_rate NEQ R_BCS)
      sig_ker_snd_msg_req (fad_data_req);
    else
      err_cause = ERR_LOCAL_BCS_RATE;
    break;

  case BCS_TRA:
    if (fad_data_req->trans_rate EQ R_BCS)
      fad_data->snd.fad_data_req = fad_data_req; /* store data, send later */
    break;

  case MSG_TRA_TCF:
     err_cause = ERR_LOCAL_DATA_TCF_RCV;
   break;

  case MSG_TRA:
    err_cause = ERR_LOCAL_DATA_MSG;
    break;

  default:
    err_cause = ERR_LOCAL_DATA_UNDEF;
    break;
  }

  if (err_cause)
  {
    ker_SetError (err_cause);
    sig_ker_snd_sync_req (FALSE);
    SET_STATE (KER, IDLE);
    PFREE (fad_data_req);
  }

}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_snd_tcf_req  |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_SND_TCF_REQ received from T30.

*/

GLOBAL void ker_fad_snd_tcf_req (T_FAD_SND_TCF_REQ *fad_snd_tcf_req)
{
  T_SBM *sbm = &fad_data->sbm;

  TRACE_FUNCTION ("ker_fad_snd_tcf_req()");
  PACCESS        (fad_snd_tcf_req);

  if (GET_STATE (KER) EQ IDLE)
  {
    SET_STATE (KER, MSG_REC_TCF);

    if (fad_snd_tcf_req->trans_rate EQ R_7200)
    {
      sbm->syncCycle = SYNC_CYCLE_7200;
      sbm->framesUntilSync = (SYNC_CYCLE_7200-1);
    }
    else if (fad_snd_tcf_req->trans_rate EQ R_12000)
    {
      sbm->syncCycle = SYNC_CYCLE_12000;
      sbm->framesUntilSync = (SYNC_CYCLE_12000-1);
    }

    sig_ker_snd_tcf_req ();
  }

  PFREE (fad_snd_tcf_req);
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_rcv_tcf_req  |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_RCV_TCF_REQ received from T30.

*/

GLOBAL void ker_fad_rcv_tcf_req (T_FAD_RCV_TCF_REQ *fad_rcv_tcf_req)
{
  T_SBM *sbm = &fad_data->sbm;

  TRACE_FUNCTION ("ker_fad_rcv_tcf_req()");

  PACCESS        (fad_rcv_tcf_req);

  if (GET_STATE (KER) EQ IDLE)
  {
    if (fad_rcv_tcf_req->trans_rate EQ R_7200)
    {
      sbm->syncCycle = SYNC_CYCLE_7200;
      sbm->framesUntilSync = (SYNC_CYCLE_7200-1);
    }
    else if (fad_rcv_tcf_req->trans_rate EQ R_12000)
    {
      sbm->syncCycle = SYNC_CYCLE_12000;
      sbm->framesUntilSync = (SYNC_CYCLE_12000-1);
    }

    SET_STATE (KER, MSG_TRA_TCF);
    sig_ker_rcv_await_tcf_req ();
  }

  PFREE (fad_rcv_tcf_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_ready_req    |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_READY_REQ received from T30.

*/

GLOBAL void ker_fad_ready_req (T_FAD_READY_REQ *fad_ready_req)
{

  TRACE_FUNCTION ("ker_fad_ready_req()");
  PACCESS        (fad_ready_req);

  /* PZ 25.5.99
  if (GET_STATE (KER) EQ MSG_TRA)
    sig_ker_rcv_ready_req ();
  */
  sig_ker_rcv_ready_req ();

  PFREE (fad_ready_req);
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)            MODULE  : FAD_KERP             |
| STATE   : code                      ROUTINE : ker_fad_ignore_req   |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_IGNORE_REQ received from T30.

*/

GLOBAL void ker_fad_ignore_req (T_FAD_IGNORE_REQ *fad_ignore_req)
{
  TRACE_FUNCTION ("ker_fad_ignore_req()");
  PACCESS        (fad_ignore_req);

  if (GET_STATE (KER) EQ MSG_TRA)
    fad_data->ker.forward_data = FALSE;

  PFREE (fad_ignore_req);
}



