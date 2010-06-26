/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_kerp
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
|             of incomming primitives for the component T30
|             of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_KERP_C
#define T30_KERP_C
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
/*==== FUNCTIONS ===================================================*/

/*

uplink
--------
DTI_DATA_TEST_REQ (SDU)
      |
      v
DTI_DATA_REQ      (DescList)
      |
      v                     compress TDC
FAD_DATA_REQ      (SDU)

downlink
--------
DTI_DATA_TEST_IND (SDU)
      ^
      |
DTI_DATA_IND      (DescList)
      ^                     expand TDC
      |
FAD_DATA_IND      (SDU)
*/

LOCAL void cleanup_dti_data_req_desc(T_desc2 **desc)
{
  if (*desc AND (*desc)->len)
  {
    MFREE_DESC2(*desc);
  }
  *desc = NULL;
}

LOCAL UBYTE prepare_fad_data_req(void)
{
  if (t30_data->fad_data_req EQ NULL)
  {
    PALLOC_SDU (fad_data_req, FAD_DATA_REQ, (USHORT)(t30_data->frames_per_prim << 3));
    fad_data_req->sdu.l_buf = 0;
    fad_data_req->final = FALSE;
    fad_data_req->trans_rate = t30_data->trans_rate;
    t30_data->fad_data_req = fad_data_req;
  }
  return TDCcompress(t30_data->dti_data_req_desc, &t30_data->fad_data_req->sdu);
}

/*-----------------------------------------------------------------------------
|  Function    : sig_dti_ker_data_received_ind
+------------------------------------------------------------------------------
|  Description : Process reason DTI_REASON_DATA_RECEIVED
|                received from dtilib callback function.
|
|  Parameters  : dti_data_req
|
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_ker_data_received_ind (T_DTI2_DATA_REQ *dti_data_req)
{
  BOOL ready;

  TRACE_FUNCTION ("sig_dti_ker_data_received_ind()");

  if (dti_data_req) /* got a new DTI_DATA_REQ */
  {
    t30_data->dti_data_req_desc = (T_desc2*)dti_data_req->desc_list2.first;
    PFREE (dti_data_req);
    t30_data->dti_data_req_ptr = NULL;

    switch (GET_STATE (KER))
    {
      case T30_SND_RDY:
        ready  = prepare_fad_data_req();

        if (ready & READY_UPLI_PAGE)
        {
          t30_data->fad_data_req->final = TRUE;
          sig_ker_msg_mdat_req(t30_data->fad_data_req);
          SET_STATE (KER, T30_SND_DATF);
        }
        else if (ready & READY_UPLI_SDU)
        {
          sig_ker_msg_mdat_req(t30_data->fad_data_req);
          SET_STATE (KER, T30_SND_DAT);
        }
        else
        {
          snd_dti_ready_ind();
        }
        break;

      default:
        cleanup_dti_data_req_desc(&t30_data->dti_data_req_desc);
        break;
    }
  }
  else /* more DTI data to process */
  {
    switch (GET_STATE (KER))
    {
      case T30_SND_DAT:
      {
        ready = prepare_fad_data_req();
        if (ready & READY_UPLI_PAGE)
        {
          t30_data->fad_data_req->final = TRUE;
          SET_STATE (KER, T30_SND_DATF);
          sig_ker_msg_mdat_req(t30_data->fad_data_req);
        }
        else if (ready & READY_UPLI_SDU)
        {
          sig_ker_msg_mdat_req(t30_data->fad_data_req);
        }
        else
        {
          SET_STATE (KER, T30_SND_RDY);
          snd_dti_ready_ind();
        }
        break;
      }
      default:
        break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_ker_tx_buffer_ready_ind
+------------------------------------------------------------------------------
|  Description : Process reason DTI_REASON_TX_BUFFER_READY received from
|                dti library callback function.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_ker_tx_buffer_ready_ind()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_ready_ind()");

  if(t30_data->dti_data_ind_empty) /* send flow control primitive if available */
  {
        snd_dti_data_ind(&t30_data->dti_data_ind_empty);
  }
  else /* send regular data primitive */
  {
    switch (GET_STATE (KER))
    {
      case T30_RCV_MSG:
        if (t30_data->dti_data_ind)
        {
          snd_dti_data_ind(&t30_data->dti_data_ind);
          if (t30_data->dti_data_ind_final)
          {
            SET_STATE (KER, T30_RCV_RDYF);
          }
          else
          {
            SET_STATE (KER, T30_RCV_RDY);
          }
        }
        else
          SET_STATE (KER, T30_RCV_DATW);
        break;

      case T30_RCV_RDY:
        if (t30_data->dti_data_ind)
        {
          snd_dti_data_ind(&t30_data->dti_data_ind);
          if (t30_data->dti_data_ind_final)
          {
            SET_STATE (KER, T30_RCV_RDYF);
          }
          else
          {
            SET_STATE (KER, T30_RCV_RDY);
          }
        }
        else
          SET_STATE (KER, T30_RCV_DAT);
        break;

      case T30_RCV_RDYF:
        SET_STATE (KER, T30_RCV_PSTW);
        snd_t30_phase_ind(BCS_PHASE);
        break;

      case T30_SND_RDYW:
        SET_STATE (KER, T30_SND_RDY);
        break;

      default:
        TRACE_EVENT("ERROR: DTI_REASON_TX_BUFFER_READY in wrong KER state");
        break;
    }
  }
}

LOCAL void snd_eol_ind(void)
{
  PALLOC (t30_eol_ind, T30_EOL_IND);
  t30_eol_ind->eol = t30_data->eol;
  t30_data->eol = 0;
  PSENDX (MMI, t30_eol_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_dti_req     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_DTI_REQ received from MMI
            and confirms with T30_DTI_CNF.
*/

GLOBAL void ker_t30_dti_req (T_T30_DTI_REQ *t30_dti_req)
{
  TRACE_FUNCTION ("ker_t30_dti_req()");

  PACCESS (t30_dti_req);

  switch (GET_STATE (KER))
  {
    case T30_RCV_MSGW:
    {
      ker_check_rtc(NULL);
      if (t30_dti_req->dti_conn NEQ T30_CONNECT_DTI)
      {
        TRACE_EVENT("ERROR: DTI not connected");
      }
      else
      {
        SET_STATE (KER, T30_RCV_MSG);
        dti_connect_setup(t30_dti_req);
      }
      break;
    }
    case T30_RCV_PSTW:
    {
      if (t30_dti_req->dti_conn NEQ T30_DISCONNECT_DTI)
      {
        TRACE_EVENT("ERROR: DTI not disconnected");
      }
      else
      {
        SET_STATE (KER, T30_RCV_PST);
        dti_disconnect();
        snd_t30_dti_cnf(T30_DISCONNECT_DTI);
        snd_eol_ind();
      }
      break;
    }
    case T30_SND_MSG:
    {
      if (t30_dti_req->dti_conn NEQ T30_CONNECT_DTI)
      {
        TRACE_EVENT("ERROR: DTI not connected");
      }
      else
      {
        SET_STATE (KER, T30_SND_RDYW);
        dti_connect_setup(t30_dti_req);
      }
      break;
    }
    case T30_SND_PSTW:
    {
      if (t30_dti_req->dti_conn NEQ T30_DISCONNECT_DTI)
      {
        TRACE_EVENT("ERROR: DTI not disconnected");
      }
      else
      {
        SET_STATE(KER, T30_SND_PST);
        dti_disconnect();
        snd_t30_dti_cnf(T30_DISCONNECT_DTI);
        SET_STATE (KER, T30_SND_PST);
        t30_data->mux.mode = MUX_BCS;
        sig_ker_mux_mux_req ();
        snd_t30_sgn_ind(t30_data->sgn_ind);
      }
      break;
    }
    default:
      if (t30_dti_req->dti_conn EQ T30_DISCONNECT_DTI)
      {
        dti_disconnect();
        snd_t30_dti_cnf(T30_DISCONNECT_DTI);
        switch (GET_STATE (KER))
        {
        case T30_RCV_DAT:
        case T30_RCV_DATW:
        case T30_RCV_MSG:
        case T30_RCV_MSGW:
        case T30_RCV_RDY:
        case T30_RCV_RDYF:
        case T30_RCV_RDYW:
          snd_error_ind(ERR_PH_C_RCV_UNSPEC);
          break;

        case T30_SND_DAT:
        case T30_SND_DATF:
        case T30_SND_MSG:
        case T30_SND_RDY:
        case T30_SND_RDYW:
          snd_error_ind(ERR_PH_C_SND_UNSPEC);
          break;
        }
        SET_STATE (KER, T30_IDLE);
      }
      else
        TRACE_EVENT("ERROR: T30_DTI_REQ in wrong KER state");
      break;
  }
  PFREE (t30_dti_req);
}

/*
+------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)   MODULE  : T30_KERP                          |
| STATE   : code             ROUTINE : sig_dti_ker_connection_opened_ind |
+------------------------------------------------------------------------+

  PURPOSE : Process reason DTI_REASON_CONNECTION_OPENED
            received from dtilib callback function
*/

GLOBAL void sig_dti_ker_connection_opened_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_opened_ind()");

  switch (GET_STATE (KER))
  {
    case T30_RCV_MSG:
    case T30_SND_RDYW:
    {
      dti_connect_open();
      break;
    }
    default:
      TRACE_EVENT("ERROR: DTI_REASON_CONNECTION_OPENED in wrong KER state");
      break;
  }
}

/*
+------------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)   MODULE  : T30_KERP                          |
| STATE   : code             ROUTINE : sig_dti_ker_connection_closed_ind |
+------------------------------------------------------------------------+

  PURPOSE : Process reason DTI_REASON_CONNECTION_CLOSED
            received from dtilib callback function
*/

GLOBAL void sig_dti_ker_connection_closed_ind ()
{
  TRACE_FUNCTION ("sig_dti_ker_connection_closed_ind()");

  switch (GET_STATE (KER))
  {
  case T30_RCV_PSTW:
    SET_STATE (KER, T30_RCV_PST);
    dti_connect_close();
    snd_eol_ind();
    break;

  case T30_SND_PSTW:
    SET_STATE(KER, T30_SND_PST);
    dti_connect_close();
    SET_STATE (KER, T30_SND_PST);
    t30_data->mux.mode = MUX_BCS;
    sig_ker_mux_mux_req ();
    snd_t30_sgn_ind(t30_data->sgn_ind);
    break;

  default:
    dti_connect_close();
    switch (GET_STATE (KER))
    {
    case T30_RCV_DAT:
    case T30_RCV_DATW:
    case T30_RCV_MSG:
    case T30_RCV_MSGW:
    case T30_RCV_RDY:
    case T30_RCV_RDYF:
    case T30_RCV_RDYW:
      snd_error_ind(ERR_PH_C_RCV_UNSPEC);
      break;

    case T30_SND_DAT:
    case T30_SND_DATF:
    case T30_SND_MSG:
    case T30_SND_RDY:
    case T30_SND_RDYW:
      snd_error_ind(ERR_PH_C_SND_UNSPEC);
      break;
    }
    SET_STATE (KER, T30_IDLE);
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_activate_cnf|
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_ACTIVATE_CNF received from FAD.
            FAD informs T30 that FAD is activated.
            T30 informs MMI that activation has been completed.
*/
GLOBAL void ker_fad_activate_cnf (T_FAD_ACTIVATE_CNF *fad_activate_cnf)
{
  TRACE_FUNCTION ("ker_fad_activate_cnf()");
  PACCESS        (fad_activate_cnf);

  switch (GET_STATE (KER))
  {
    case T30_ACTIVATE:
    {
      PPASS (fad_activate_cnf, t30_activate_cnf, T30_ACTIVATE_CNF);

#ifdef _TARGET_
      /*
       * sleep for 2 seconds because we should not send DIS to early
       * on a real network
       */
      vsi_t_sleep (VSI_CALLER FRAMES_400);
#endif
      /*
       * we pass also the buffer size parameters to MMI
       */
      SET_STATE (KER, T30_IDLE);
      PSENDX (MMI, t30_activate_cnf);
      break;
    }
    default:
      PFREE (fad_activate_cnf);
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_data_cnf    |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_DATA_CNF received from FAD.
            This primitive is received after a command or response
            has been sent successfully by FAD.
*/

GLOBAL void ker_fad_data_cnf (T_FAD_DATA_CNF *fad_data_cnf)
{
  TRACE_FUNCTION ("ker_fad_data_cnf()");
  PACCESS        (fad_data_cnf);
  t30_data->data_cnf = TRUE;

  switch (GET_STATE (KER))
  {
    case T30_DCN:
      snd_complete_ind(CMPL_DCN);
      break;

    case T30_RCV_CFR:
      TIMERSTART (T2_INDEX, MSG_VALUE);
      SET_STATE (KER, T30_RCV_MSGW);
      snd_t30_phase_ind(MSG_PHASE);
      break;

    case T30_RCV_DIS:
      TIMERSTART (T4_INDEX, T4_VALUE);
      SET_STATE (KER, T30_RCV_DCS);
      break;

    case T30_RCV_MCF:
      TIMERSTART (T2_INDEX, T2_VALUE);
      switch (t30_data->res)
      {
        case BCS_EOM:
          SET_STATE (KER, T30_RCV_T2);
          break;

        case BCS_EOP:
          SET_STATE (KER, T30_RCV_DCN);
          break;

        case BCS_MPS:
          t30_data->prev = BCS_MCF;
          SET_STATE (KER, T30_RCV_MSGW);
          snd_t30_phase_ind(MSG_PHASE);
          break;
      }
      break;
    
    case T30_RCV_PI:
      switch (t30_data->res)
      {
        case SGN_PRI_EOM:
        case SGN_PRI_EOP:
        case SGN_PRI_MPS:
          snd_complete_ind(CMPL_PI);
          break;

        default:
          SET_STATE (KER, T30_RCV_PST);
          break;
      }
      break;

    case T30_RCV_RT:
      SET_STATE (KER, T30_RCV_DCS);
      break;

    case T30_RCV_RT2:
      SET_STATE (KER, T30_RCV_DCS);
      break;

    case T30_SND_DCN:
      TIMERSTART (T4_INDEX, DCN_VALUE);
      break;

    case T30_SND_DCN1:
      snd_complete_ind(CMPL_DCN);
      break;

    case T30_SND_DCS:
      {
      PALLOC (fad_snd_tcf_req, FAD_SND_TCF_REQ);
      fad_snd_tcf_req->trans_rate = t30_data->trans_rate;
      PSENDX (FAD, fad_snd_tcf_req);
      SET_STATE (KER, T30_SND_TCF);
      }
      break;

    case T30_SND_DTC:
      TIMERSTOP (T1_INDEX);
      SET_STATE (KER, T30_RCV_DCS);
      break;

    case T30_SND_DATF:
      TIMERSTOP (T4_INDEX);
      SET_STATE (KER, T30_SND_PSTW);
      cleanup_dti_data_req_desc(&t30_data->dti_data_req_desc);
      snd_t30_phase_ind(BCS_PHASE);
      break;

    case T30_SND_PRI:
      snd_complete_ind(CMPL_PI);
      break;

    case T30_SND_SGN:
      TIMERSTART (T4_INDEX, T4_VALUE);
      SET_STATE (KER, T30_SND_MCF);
      break;
  }
  PFREE (fad_data_cnf);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)           MODULE  : T30_KERP              |
| STATE   : code                     ROUTINE : ker_fad_deactivate_cnf|
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_DEACTIVATE_CNF received from FAD.
            FAD informs T30 that FAD is deactivated.
            T30 informs MMI that deactivation has been completed.
*/

GLOBAL void ker_fad_deactivate_cnf (T_FAD_DEACTIVATE_CNF
                                                  *fad_deactivate_cnf)
{
  TRACE_FUNCTION ("ker_fad_deactivate_cnf()");
  PACCESS (fad_deactivate_cnf);
  PFREE (fad_deactivate_cnf);

  if (GET_STATE (KER) EQ T30_DEACTIVATE)
  {
    PALLOC (t30_deactivate_cnf, T30_DEACTIVATE_CNF);
    SET_STATE (KER, T30_NULL);
    PSENDX (MMI, t30_deactivate_cnf);
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_error_ind   |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_ERROR_IND received from FAD.
            All running timers are stopped and the error message is
            passed to MMI.
*/

GLOBAL void ker_fad_error_ind (T_FAD_ERROR_IND *fad_error_ind)
{
  TRACE_FUNCTION ("ker_fad_error_ind()");
  PACCESS        (fad_error_ind);
  {
  PPASS (fad_error_ind, t30_error_ind, T30_ERROR_IND);
  PSENDX  (MMI, t30_error_ind);
  }
  TIMERSTOP (T1_INDEX);
  TIMERSTOP (T2_INDEX);
  TIMERSTOP (T4_INDEX);
  SET_STATE (KER, T30_IDLE);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_rcv_tcf_cnf |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_RCV_TCF_CNF received from FAD.
            FAD sends the ratio of the received TCF.
            If the ratio is OK a CFR is sent as response otherwise an FTT.
*/

GLOBAL void ker_fad_rcv_tcf_cnf (T_FAD_RCV_TCF_CNF
                                                     *fad_rcv_tcf_cnf)
{
  TRACE_FUNCTION ("ker_fad_rcv_tcf_cnf()");
  PACCESS        (fad_rcv_tcf_cnf);

  switch (GET_STATE (KER))
  {
    case T30_RCV_TCF:
    {
      TIMERSTOP (T2_INDEX);
      t30_data->rate_modified   = FALSE;

      if (fad_rcv_tcf_cnf->ratio >= t30_data->threshold)
      {
        _decodedMsg[0] = BCS_CFR;

        SET_STATE (KER, T30_RCV_CFR);

        sig_ker_bcs_bdat_req (FINAL_YES);
      }
      else
      {
        _decodedMsg[0] = BCS_FTT;

        SET_STATE (KER, T30_RCV_DIS);

        sig_ker_bcs_bdat_req (FINAL_YES);
      }

      break;
    }

    default:
      break;
  }

  PFREE (fad_rcv_tcf_cnf);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_ready_ind   |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_READY_IND received from FAD.
            FAD informs T30 that next fax data block can be sent.
            T30 passes this information to MMI.
*/

GLOBAL void ker_fad_ready_ind (T_FAD_READY_IND *fad_ready_ind)
{
  TRACE_FUNCTION ("ker_fad_ready_ind()");
  PACCESS (fad_ready_ind);

  switch (GET_STATE (KER))
  {
    case T30_SND_DAT:
    {
      if (t30_data->dti_data_req_ptr) /* pending data in DTI_DATA_REQ */
      {
        sig_dti_ker_data_received_ind (NULL);
      }
      else
      {
        SET_STATE (KER, T30_SND_RDY);
        snd_dti_ready_ind();
      }
      break;
    }
    default:
      break;
  }
  PFREE (fad_ready_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)  MODULE  : T30_KERP                       |
| STATE   : code            ROUTINE : sig_dti_ker_tx_buffer_full_ind |
+--------------------------------------------------------------------+

  PURPOSE : Process signal DTI_REASON_TX_BUFFER_FULL received from
            dti library callback function.
            The buffer function of DTILIB is not used.
            So there is nothing to be done here yet..

*/
GLOBAL void sig_dti_ker_tx_buffer_full_ind()
{
  TRACE_FUNCTION ("sig_dti_ker_tx_buffer_full_ind()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_fad_snd_tcf_cnf |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_SND_TCF_CNF received from FAD.
            FAD informs T30 that the TCF has been sent.
*/

GLOBAL void ker_fad_snd_tcf_cnf (T_FAD_SND_TCF_CNF *fad_snd_tcf_cnf)
{
  TRACE_FUNCTION ("ker_fad_snd_tcf_cnf()");
  PACCESS        (fad_snd_tcf_cnf);

  PFREE (fad_snd_tcf_cnf);

  switch (GET_STATE (KER))
  {
    case T30_SND_TCF:
    {
      SET_STATE (KER, T30_SND_CFR);
      TIMERSTART (T4_INDEX, T4_VALUE);
      break;
    }
    default:
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : Initialize_Shared_Fax_Data_Memory|
+--------------------------------------------------------------------+

  PURPOSE :
*/

LOCAL void Initialize_Shared_Fax_Data_Memory(void)
{
  ker_init (&t30_data_base[0]);
  bcs_init (&t30_data_base[0]);
  
  SET_STATE (MUX, T30_MUX_OFF);
  
  t30_data_magic_num = T30_DATA_MAGIC_NUM;  /* memory is initialized */
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_activate_req|
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_ACTIVATE_REQ received from MMI.
            MMI activates T30. T30 then activates FAD.
            The BCS multiplexer is switched on.
*/

GLOBAL void ker_t30_activate_req (T_T30_ACTIVATE_REQ *t30_activate_req)
{
  TRACE_FUNCTION ("ker_t30_activate_req()");
  PACCESS        (t30_activate_req);

  Initialize_Shared_Fax_Data_Memory();

  switch (GET_STATE (KER))
  {
    case T30_NULL:
    {
      PALLOC (fad_activate_req, FAD_ACTIVATE_REQ);

      memset (t30_data, 0, sizeof(T_T30_DATA));

      t30_data->threshold       = t30_activate_req->threshold;
      t30_data->preamble_ind    = TRUE;
      t30_data->trans_rate      = t30_activate_req->trans_rate;
      t30_data->bitorder        = t30_activate_req->bitorder;
      t30_data->rate_modified   = FALSE;

      t30_data->TDC_doli_pending = FALSE;
      t30_data->TDC_upli_pending = FALSE;

      t30_data->frames_per_prim = MSG_SIZE_BITS >> 3; /* traditionally */

      fad_activate_req->trans_rate      = t30_activate_req->trans_rate;
      fad_activate_req->frames_per_prim = t30_activate_req->frames_per_prim;

      SET_STATE (KER, T30_ACTIVATE);
      PSENDX (FAD, fad_activate_req);

      t30_data->mux.mode = MUX_BCS;
      sig_ker_mux_mux_req ();

      TIMERSTART (T1_INDEX, T1_VALUE);
      break;
    }
    default:
      break;
  }
  PFREE (t30_activate_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_cap_req     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_CAP_REQ received from MMI.
            The DIS parameters are prepared for sending to remote site.
*/

GLOBAL void ker_t30_cap_req (T_T30_CAP_REQ *t30_cap_req)
{
  TRACE_FUNCTION ("ker_t30_cap_req()");
  PACCESS        (t30_cap_req);

  switch (GET_STATE (KER))
  {
  case T30_IDLE:
    if (t30_cap_req->hdlc_info.v_dis)
    {
      TIMERSTOP (T1_INDEX);
      TIMERSTART (T1_INDEX, T1_VALUE);
      t30_data->mux.mode = MUX_BCS;
      t30_data->fmod     = FMOD_RCV;
      t30_data->dir      = 0;
      sig_ker_mux_mux_req ();
      memcpy (&t30_data->hdlc_snd, &t30_cap_req->hdlc_info, sizeof (T_hdlc_info));
      memset (&t30_data->hdlc_rcv, 0, sizeof (T_hdlc_info));
      SET_STATE (KER, T30_RCV_DIS);
      ker_send_dis ();
    }
    break;

  case T30_SND_CAP:
    if (t30_cap_req->hdlc_info.v_dcs)
    {
      SET_STATE (KER, T30_SND_DCS);
      t30_data->fmod = FMOD_SND;
      ker_send_dcs (t30_cap_req);
    }
    else if (t30_cap_req->hdlc_info.v_dtc)
    {
      SET_STATE (KER, T30_SND_DTC);
      t30_data->fmod = FMOD_POLL;
      ker_send_dtc (t30_cap_req);
    }
    break;

  default:
    break;
  }

  PFREE (t30_cap_req);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_config_req  |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_CONFIG_REQ received from MMI.
            MMI sends configuration parameters to control the behaviour of T30.
            The parameter test_mode is for internal use only.
*/

GLOBAL void ker_t30_config_req (T_T30_CONFIG_REQ *t30_config_req)
{
  TRACE_FUNCTION ("ker_t30_config_req()");
  PACCESS        (t30_config_req);

  if (t30_config_req->test_mode & TST_BCS)
    Initialize_Shared_Fax_Data_Memory();

  t30_data->hdlc_report = t30_config_req->hdlc_report;

#ifdef _SIMULATION_ /* test_mode */
  t30_data->test_mode = t30_config_req->test_mode;
#endif

  PFREE (t30_config_req);

#ifdef _SIMULATION_ /* test_mode */
  if (t30_data->test_mode)
  {
    TRACE_EVENT(" ");
    TRACE_EVENT ("!!!! T E S T - M O D E !!!!");
    TRACE_EVENT(" ");
    if (t30_data->test_mode & TST_BCS)
      TRACE_EVENT(">>> test-mode: BCS test only");
    if (t30_data->test_mode & TST_STUFF)
      TRACE_EVENT(">>> test-mode: stuffing / destuffing off");
    if (t30_data->test_mode & TST_FCS)
      TRACE_EVENT(">>> test-mode: FCS generating / checking off");
    if (t30_data->test_mode & TST_CTRL)
      TRACE_EVENT(">>> test-mode: HDLC control checking off");
    if (t30_data->test_mode & TST_RTC)
      TRACE_EVENT(">>> test-mode: RTC checking off");
    if (t30_data->test_mode & TST_BUF)
      TRACE_EVENT(">>> test-mode: show contents of some buffers");
    TRACE_EVENT(" ");
  }
  else
  {
    TRACE_EVENT(" ");
    TRACE_EVENT(">>> NORMAL MODE <<<");
    TRACE_EVENT(" ");
  }
#endif
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)           MODULE  : T30_KERP              |
| STATE   : code                     ROUTINE : ker_t30_deactivate_req|
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_DEACTIVATE_REQ received from MMI.
            MMI deactivates T30. T30 then deactivates FAD.
*/

GLOBAL void ker_t30_deactivate_req (T_T30_DEACTIVATE_REQ *t30_deactivate_req)
{
  TRACE_FUNCTION ("ker_t30_deactivate_req()");
  PACCESS        (t30_deactivate_req);
  {
  PPASS (t30_deactivate_req, fad_deactivate_req, FAD_DEACTIVATE_REQ);
  PSENDX  (FAD, fad_deactivate_req);
  SET_STATE (KER, T30_DEACTIVATE);
  }
  TIMERSTOP (T1_INDEX);
  TIMERSTOP (T2_INDEX);
  TIMERSTOP (T4_INDEX);
  t30_data->mux.mode = MUX_OFF;
  sig_ker_mux_mux_req();
  cleanup_dti_data_req_desc(&t30_data->dti_data_req_desc);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_modify_req  |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_MODIFY_REQ received from MMI.
            MMI sends new parameter values to T30.
            T30 passes some of this parameters to FAD.
*/

GLOBAL void ker_t30_modify_req (T_T30_MODIFY_REQ *t30_modify_req)
{
  TRACE_FUNCTION ("ker_t30_modify_req()");
  PACCESS        (t30_modify_req);

  t30_data->trans_rate = t30_modify_req->trans_rate;
  {
  PPASS (t30_modify_req, fad_modify_req, FAD_MODIFY_REQ);
  PSENDX (FAD, fad_modify_req);
  }
  switch (GET_STATE (KER))
  {
    case T30_SND_TCF:
    {
      if (!t30_data->rate_modified)
      {
        /*
         * rate is modified or confirmed (t30_modify_req) by ACI; send TCF in FAD
         */
        PALLOC (fad_snd_tcf_req, FAD_SND_TCF_REQ);
        fad_snd_tcf_req->trans_rate = t30_data->trans_rate;
        PSENDX (FAD, fad_snd_tcf_req);
        t30_data->rate_modified = TRUE;
      }
    }
    break;

    case T30_SND_DCS:
    case T30_RCV_DCS:
    case T30_RCV_TCF:
    {
      t30_data->rate_modified = TRUE;
    }
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_KERP            |
| STATE   : code                       ROUTINE : ker_t30_sgn_req     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive T30_SGN_REQ received from MMI.
            MMI informs T30 which response should be send.
            T30 prepares an appropriate message
            and requests FAD to send this message.
*/

GLOBAL void ker_t30_sgn_req (T_T30_SGN_REQ *t30_sgn_req)
{
  TRACE_FUNCTION ("ker_t30_sgn_req()");
  PACCESS        (t30_sgn_req);

  switch (GET_STATE (KER))
  {
    case T30_IDLE:
    {
      switch (t30_sgn_req->sgn)
      {
        case SGN_DCN:
        {
          _decodedMsg[0] = BCS_DCN;

          SET_STATE (KER, T30_SND_DCN1);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        default:
        {
          ker_send_err (ERR_SGN_NOT_ALLOWED);

          break;
        }
      }

      break;
    }

    case T30_RCV_PST:
    {
      switch (t30_sgn_req->sgn)
      {
        case SGN_MCF:
        {
          _decodedMsg[0] = BCS_MCF;

          SET_STATE (KER, T30_RCV_MCF);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_PIN:
        {
          _decodedMsg[0] = BCS_PIN;

          SET_STATE (KER, T30_RCV_PI);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_PIP:
        {
          _decodedMsg[0] = BCS_PIP;

          SET_STATE (KER, T30_RCV_PI);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_RTN:
        {
          _decodedMsg[0] = BCS_RTN;

          t30_data->prev = BCS_RTN;

          SET_STATE (KER, T30_RCV_RT);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_RTP:
        {
          _decodedMsg[0] = BCS_RTP;

          t30_data->prev = BCS_RTP;

          SET_STATE (KER, T30_RCV_RT);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        default:
        {
          ker_send_err (ERR_SGN_NOT_ALLOWED);

          break;
        }
      }

      break;
    }

    /*-------------------------------------------------------------*/

    case T30_SND_CAP:
    {
      switch (t30_sgn_req->sgn)
      {
        case SGN_DCN:
        {
          _decodedMsg[0] = BCS_DCN;

          SET_STATE (KER, T30_DCN);

          t30_data->fmod = FMOD_IDLE;

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        default:
        {
          ker_send_err (ERR_SGN_NOT_ALLOWED);

          break;
        }
      }

      break;
    }

    /*-------------------------------------------------------------*/

    case T30_SND_PI:
    {
      switch (t30_data->sgn_req)
      {
        case SGN_EOM:
        case SGN_EOP:
        case SGN_MPS:
        case SGN_PRI_EOM:
        case SGN_PRI_EOP:
        case SGN_PRI_MPS:
        {
          switch (t30_sgn_req->sgn)
          {
            case SGN_PRI_EOM:
            {
              _decodedMsg[0] = BCS_PRI_EOM;

              SET_STATE (KER, T30_SND_PRI);

              sig_ker_bcs_bdat_req (FINAL_YES);

              break;
            }

            case SGN_PRI_EOP:
            {
              _decodedMsg[0] = BCS_PRI_EOP;

              SET_STATE (KER, T30_SND_PRI);

              sig_ker_bcs_bdat_req (FINAL_YES);

              break;
            }

            case SGN_PRI_MPS:
            {
              _decodedMsg[0] = BCS_PRI_MPS;

              SET_STATE (KER, T30_SND_PRI);

              sig_ker_bcs_bdat_req (FINAL_YES);

              break;
            }
          }

          break;
        }

        case SGN_CONT:
        {
          SET_STATE (KER, T30_IDLE);

          break;
        }

        default:
        {
          ker_send_err (ERR_SGN_NOT_ALLOWED);

          break;
        }
      }

      break;
    }

    /*-------------------------------------------------------------*/

    case T30_SND_PST:
    {
      t30_data->repeat = 1;

      switch (t30_sgn_req->sgn)
      {
        case SGN_DCN:
        {
          _decodedMsg[0] = BCS_DCN;

          SET_STATE (KER, T30_DCN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_EOM:
        {
          t30_data->sgn_req  = BCS_EOM;
          _decodedMsg[0] = BCS_EOM;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_EOP:
        {
          t30_data->sgn_req  = BCS_EOP;
          _decodedMsg[0] = BCS_EOP;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_MPS:
        {
          t30_data->sgn_req  = BCS_MPS;
          _decodedMsg[0] = BCS_MPS;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_PRI_EOM:
        {
          t30_data->sgn_req  = BCS_PRI_EOM;
          _decodedMsg[0] = BCS_PRI_EOM;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_PRI_EOP:
        {
          t30_data->sgn_req  = BCS_PRI_EOP;
          _decodedMsg[0] = BCS_PRI_EOP;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_PRI_MPS:
        {
          t30_data->sgn_req  = BCS_PRI_MPS;
          _decodedMsg[0] = BCS_PRI_MPS;

          SET_STATE (KER, T30_SND_SGN);

          sig_ker_bcs_bdat_req (FINAL_YES);

          break;
        }

        case SGN_CONT:
        {
          SET_STATE (KER, T30_IDLE);

          break;
        }

        default:
        {
          ker_send_err (ERR_SGN_NOT_ALLOWED);

          break;
        }
      }

      break;
    }

    default:
      break;
  }

  PFREE (t30_sgn_req);
}
