/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  L2r_mgtp.c
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
|             Radio Link Protocol of the base station
+-----------------------------------------------------------------------------
*/

#ifndef L2R_MGTP_C
#define L2R_MGTP_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_l2r.h"
#include "cnf_l2r.h"
#include "mon_l2r.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */

#include "cl_ribu.h"
#include "l2r.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

GLOBAL void mgt_l2r_connect_cnf(UBYTE nack)
{
  PALLOC (l2r_connect_cnf, L2R_CONNECT_CNF);
  l2r_connect_cnf->ack_flg = nack;
  PSENDX (CTRL, l2r_connect_cnf);
}

LOCAL void snd_rlp_connect_res(UBYTE nack)
{
  PALLOC (rlp_connect_res, RLP_CONNECT_RES);
  rlp_connect_res->ack_flg = nack;
  PSENDX (RLP, rlp_connect_res);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_l2r_activate_req
+------------------------------------------------------------------------------
|  Description : Process primitive L2R_ACTIVATE_REQ  received from the Controller
|
|  Parameters  : l2r_activate_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_l2r_activate_req(T_L2R_ACTIVATE_REQ *l2r_activate_req)
{
  USHORT BytesPerPrim; /* Number of bytes per primitive */
  T_L2R_DATA *l2r_data_orig;

  TRACE_FUNCTION ("mgt_l2r_activate_req()");
  PACCESS        (l2r_activate_req);

  /***************************************
   *
   *  Initialize Shared Fax/Data memory  *
   *
   ***************************************
   */

  l2r_data_mode = NONTRANSP;

  l2r_data_orig = l2r_data;

  if (l2r_data_magic_num NEQ L2R_DATA_MAGIC_NUM)
  {
    l2r_data = &l2r_data_base[0];
  }

  memset (l2r_data, 0, sizeof (T_L2R_DATA));

  mgt_init(&l2r_data->mgt);
  dn_init(&l2r_data->dn);
  up_init(&l2r_data->up);

  l2r_data_magic_num = L2R_DATA_MAGIC_NUM; /* memory is initialized */

  l2r_data = l2r_data_orig;

  switch (GET_STATE (MGT))
  {
  case MGT_ATTACHED:
    {
    PPASS (l2r_activate_req, l2r_activate_cnf, L2R_ACTIVATE_CNF);
    l2r_activate_cnf->ack_flg = L2R_NAK;
    PSENDX (CTRL, l2r_activate_cnf);
    break;
    }

  case MGT_DETACHED:
    if (mgt_checkpar(l2r_activate_req) EQ FALSE) /* invalid parameter values */
    {
      PPASS (l2r_activate_req, l2r_activate_cnf, L2R_ACTIVATE_CNF);
      l2r_activate_cnf->ack_flg = L2R_NAK;
      PSENDX (CTRL, l2r_activate_cnf);
      return;
    }

    {
    T_MGT *dmgt = &l2r_data->mgt;

    PALLOC (rlp_attach_req, RLP_ATTACH_REQ);

    rlp_attach_req->k_ms_iwf        = l2r_activate_req->k_ms_iwf;
    rlp_attach_req->k_iwf_ms        = l2r_activate_req->k_iwf_ms;
    rlp_attach_req->t1              = l2r_activate_req->t1;
    rlp_attach_req->t2              = l2r_activate_req->t2;
    rlp_attach_req->n2              = l2r_activate_req->n2;
    rlp_attach_req->pt              = l2r_activate_req->pt;
    rlp_attach_req->p1              = l2r_activate_req->p1;
    rlp_attach_req->p2              = l2r_activate_req->p2;
    rlp_attach_req->bytes_per_prim  = l2r_activate_req->bytes_per_prim;

    switch (l2r_activate_req->uil2p)
    {
    case L2R_COPnoFlCt:
      dmgt->FlowCtrlUsed = FALSE;
      break;
    case L2R_ISO6429:
      dmgt->FlowCtrlUsed = TRUE;
      break;
    }

    switch (l2r_activate_req->rate)
    {
    case L2R_FULLRATE_14400:
      rlp_attach_req->rate = RLP_FULLRATE_14400;
      dmgt->FrameSize = RLP_FRAME_SIZE_LONG;
      break;

    case L2R_FULLRATE_9600:
      rlp_attach_req->rate = RLP_FULLRATE_9600;
      dmgt->FrameSize = RLP_FRAME_SIZE_SHORT;
      break;

    case L2R_FULLRATE_4800:
      rlp_attach_req->rate = RLP_FULLRATE_4800;
      dmgt->FrameSize = RLP_FRAME_SIZE_SHORT;
      break;

    case L2R_HALFRATE_4800:
      rlp_attach_req->rate = RLP_HALFRATE_4800;
      dmgt->FrameSize = RLP_FRAME_SIZE_SHORT;
      break;
    }

    BytesPerPrim        = l2r_activate_req->bytes_per_prim;
    dmgt->FramesPerPrim = (BytesPerPrim + dmgt->FrameSize - 1) / dmgt->FrameSize;
    dmgt->RiBuSize      = ((l2r_activate_req->buffer_size + BytesPerPrim - 1) / BytesPerPrim) + 1;

    if (dmgt->RiBuSize < MIN_RIBU_SIZE)
    {
      dmgt->RiBuSize = MIN_RIBU_SIZE;
    }

    rlp_attach_req->p0 = RLP_COMP_DIR_NONE;
    PSENDX (RLP, rlp_attach_req);
    SET_STATE (CONIND, IW_IDLE);
    SET_STATE (MGT, MGT_PENDING_ATTACH);
    break;
    }
  }
  PFREE (l2r_activate_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_l2r_deactivate_req
+------------------------------------------------------------------------------
|  Description : Process primitive L2R_DEACTIVATE_REQ received from the Controller.
|
|  Parameters  : l2r_deactivate_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_l2r_deactivate_req
                  (
                    T_L2R_DEACTIVATE_REQ *l2r_deactivate_req
                  )
{
  TRACE_FUNCTION ("mgt_l2r_deactivate_req()");
  PACCESS        (l2r_deactivate_req);

  if (GET_STATE (MGT) NEQ MGT_DETACHED)
  {
    PALLOC (rlp_detach_req, RLP_DETACH_REQ);
    mgt_deinit_connection();
    PSENDX (RLP, rlp_detach_req);
    SET_STATE (MGT, MGT_PENDING_DETACH);
  }

  PFREE (l2r_deactivate_req);

  if (GET_STATE (DTI) NEQ DTI_CLOSED)
  {
    dti_close(l2r_hDTI, L2R_DTI_UP_DEF_INSTANCE, L2R_DTI_UP_INTERFACE, L2R_DTI_UP_CHANNEL, FALSE);
    SET_STATE (DTI, DTI_CLOSED);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_l2r_connect_req
+------------------------------------------------------------------------------
|  Description : Process primitive L2R_CONNECT_REQ received from the Controller.
|
|  Parameters  : l2r_connect_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_l2r_connect_req(T_L2R_CONNECT_REQ *l2r_connect_req)
{
  TRACE_FUNCTION ("mgt_l2r_connect_req()");
  PACCESS        (l2r_connect_req);

  if (GET_STATE (MGT) EQ MGT_ATTACHED)
  {
    PALLOC (rlp_connect_req, RLP_CONNECT_REQ);
    PSENDX (RLP, rlp_connect_req);
    SET_STATE (MGT, MGT_PENDING_CONNECT);
  }
  PFREE (l2r_connect_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_l2r_disc_req
+------------------------------------------------------------------------------
|  Description : Process primitive L2R_DISC_REQ received from the Controller.
|
|  Parameters  : l2r_disc_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_l2r_disc_req(T_L2R_DISC_REQ *l2r_disc_req)
{
  TRACE_FUNCTION ("mgt_l2r_disc_req()");
  PACCESS        (l2r_disc_req);

  switch (GET_STATE (MGT))
  {
  case MGT_DETACHED:
  case MGT_PENDING_ATTACH:
  case MGT_ATTACHED:
    break;

  default:
    {
    PALLOC (rlp_disc_req, RLP_DISC_REQ);
    PSENDX (RLP, rlp_disc_req);
    mgt_deinit_connection();
    SET_STATE (MGT, MGT_DISCONNECT_INITIATED);
    break;
    }
  }
  PFREE (l2r_disc_req);
}

LOCAL void send_l2r_dti_cnf(U8 dti_conn, U32 link_id)
{
  PALLOC (l2r_dti_cnf, L2R_DTI_CNF);
  l2r_dti_cnf->dti_conn = dti_conn;
  l2r_dti_cnf->link_id = link_id;
  PSENDX (CTRL, l2r_dti_cnf);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_l2r_dti_req
+------------------------------------------------------------------------------
|  Description : Process primitive L2R_DTI_REQ received from the Controller.
|
|  Parameters  : l2r_dti_req -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_l2r_dti_req(T_L2R_DTI_REQ *l2r_dti_req)
{
  BOOL isOK;

  TRACE_FUNCTION ("mgt_l2r_dti_req()");
  PACCESS        (l2r_dti_req);

  switch (GET_STATE (MGT))
  {
  case MGT_CONNECTED:
    switch (l2r_dti_req->dti_conn)
    {
    case L2R_CONNECT_DTI:
      l2r_data->mgt.link_id = l2r_dti_req->link_id;

      if (GET_STATE (DTI) NEQ DTI_CLOSED)
      {
        dti_close(l2r_hDTI, L2R_DTI_UP_DEF_INSTANCE, L2R_DTI_UP_INTERFACE, L2R_DTI_UP_CHANNEL, FALSE);
        SET_STATE (DTI, DTI_CLOSED);
      }
      /*
       * open new dti library communication channel
       */
      INIT_STATE (DTI, DTI_SETUP);
      sig_mgt_up_dti_conn_setup (l2r_dti_req->link_id);
      sig_mgt_dn_dti_conn_setup (l2r_dti_req->link_id);

      isOK = dti_open (
        l2r_hDTI,
        L2R_DTI_UP_DEF_INSTANCE,
        L2R_DTI_UP_INTERFACE,
        L2R_DTI_UP_CHANNEL,
        L2R_DTI_UPLINK_QUEUE_SIZE,
        l2r_dti_req->dti_direction,
        DTI_QUEUE_UNUSED,
        DTI_VERSION_10,
        (U8*)l2r_dti_req->entity_name,
        l2r_dti_req->link_id
        );

      if (isOK EQ FALSE)
      {
        send_l2r_dti_cnf(L2R_DISCONNECT_DTI, l2r_dti_req->link_id);
        SET_STATE (DTI, DTI_CLOSED);
        PFREE(l2r_dti_req);
        return;
      }
      PFREE(l2r_dti_req);
      /*
       * L2R_DTI_CNF will be sent in sig_dti_mgt_connection_opened_ind
       */
      return;

    case L2R_DISCONNECT_DTI:
      l2r_data->up.DtiConnected = FALSE;
      l2r_data->dn.DtiConnected = FALSE;
      /*
      TRACE_EVENT_P2 ("Air interface: Received:%d Sent:%d", l2r_data->dn.RcvdChar, l2r_data->up.SentChar);
      */
      if (GET_STATE (DTI) NEQ DTI_CLOSED)
      {
        dti_close(l2r_hDTI, L2R_DTI_UP_DEF_INSTANCE, L2R_DTI_UP_INTERFACE, L2R_DTI_UP_CHANNEL, FALSE);
        SET_STATE (DTI, DTI_CLOSED);
      }
      TRACE_EVENT("MGT: DTI snatched away by ACI");
      break;
    }
    send_l2r_dti_cnf(l2r_dti_req->dti_conn, l2r_dti_req->link_id);
    break;

  default:
    break;
  }
  PFREE(l2r_dti_req);
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_mgt_connection_opened_ind
+------------------------------------------------------------------------------
| Description : Handles the DTILIB callback call DTI_REASON_CONNECTION_OPENED
|
|               This signal means that a dti connection has been opened
|               successfully.
|               Since it is called directy from the dtilib callback function
|               it is handled like a primitive, here..
|
|  Parameters  : -
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_mgt_connection_opened_ind()
{
  TRACE_FUNCTION ("sig_dti_mgt_connection_opened_ind()");

  if (GET_STATE (DTI) EQ DTI_SETUP)
  {
    send_l2r_dti_cnf(L2R_CONNECT_DTI, l2r_data->mgt.link_id);
    sig_mgt_up_dti_conn_open();

    l2r_data->dn.DtiConnected = TRUE;
    TRACE_EVENT("MGT: DTI received from ACI");
    SET_STATE (DTI, DTI_IDLE);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_mgt_connection_closed_ind
+------------------------------------------------------------------------------
| Description : Handles the DTILIB callback call DTI_REASON_CONNECTION_CLOSED
|
|               This signal means that a dti connection has been closed
|               Since it is called directy from the dtilib callback function
|               it is handled like a primitive, here..
|
|  Parameters  : -
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_mgt_connection_closed_ind()
{
  TRACE_FUNCTION ("sig_dti_mgt_connection_closed_ind()");

  if (GET_STATE (DTI) NEQ DTI_CLOSED)
  {
    PALLOC (l2r_dti_ind, L2R_DTI_IND);

    SET_STATE (DTI, DTI_CLOSED);

    l2r_data->up.DtiConnected = FALSE;
    l2r_data->dn.DtiConnected = FALSE;

    l2r_dti_ind->link_id = l2r_data->mgt.link_id;
    TRACE_EVENT("MGT: DTI passed to ACI");
    PSENDX (CTRL, l2r_dti_ind);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_connect_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_CONNECT_IND received from RLP.
|
|  Parameters  : rlp_connect_ind -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_connect_ind(T_RLP_CONNECT_IND *rlp_connect_ind)
{
  TRACE_FUNCTION ("mgt_rlp_connect_ind()");
  PACCESS        (rlp_connect_ind);

  switch (GET_STATE (MGT))
  {
  case MGT_PENDING_ATTACH:
    SET_STATE (CONIND, IW_WAIT);
    break;

  case MGT_ATTACHED:
    snd_rlp_connect_res(RLP_ACK);
    mgt_init_connection(TRUE);
    SET_STATE (MGT, MGT_CONNECTED);
    break;

  case MGT_PENDING_CONNECT:
    snd_rlp_connect_res(RLP_ACK);
    mgt_init_connection(FALSE);
    SET_STATE (MGT, MGT_CONNECTED);
    break;
  }
  PFREE (rlp_connect_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_connect_cnf
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_CONNECT_CNF received from RLP.
|
|  Parameters  : rlp_connect_cnf -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_connect_cnf(T_RLP_CONNECT_CNF *rlp_connect_cnf)
{
  TRACE_FUNCTION ("mgt_rlp_connect_cnf()");
  PACCESS        (rlp_connect_cnf);

  if (GET_STATE (MGT) EQ MGT_PENDING_CONNECT)
  {
    switch (rlp_connect_cnf->ack_flg)
    {
    case RLP_ACK:
      mgt_init_connection(FALSE);
      SET_STATE (MGT, MGT_CONNECTED);
      break;

    case RLP_NAK:
      mgt_l2r_connect_cnf(L2R_NAK);
      SET_STATE (MGT, MGT_ATTACHED);
      break;
    }
  }
  PFREE (rlp_connect_cnf);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_disc_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_DISC_IND received from RLP.
|
|  Parameters  : rlp_disc_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_rlp_disc_ind(T_RLP_DISC_IND *rlp_disc_ind)
{
  TRACE_FUNCTION ("mgt_rlp_disc_ind()");
  PACCESS        (rlp_disc_ind);

  switch (GET_STATE (MGT))
  {
  case MGT_PENDING_CONNECT:
    mgt_l2r_connect_cnf(L2R_NAK);
    SET_STATE (MGT, MGT_ATTACHED);
    break;

  case MGT_CONNECTED:
    {
    PALLOC (l2r_disc_ind, L2R_DISC_IND);
    PSENDX (CTRL, l2r_disc_ind);
    }
    mgt_deinit_connection();
    SET_STATE (MGT, MGT_ATTACHED);
    break;
  }
  PFREE (rlp_disc_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_disc_cnf
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_DISC_CNF received from RLP.
|
|  Parameters  : rlp_disc_cnf -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_rlp_disc_cnf
                  (
                    T_RLP_DISC_CNF       *rlp_disc_cnf
                  )
{
  TRACE_FUNCTION ("mgt_rlp_disc_cnf()");
  PACCESS        (rlp_disc_cnf);

  switch (GET_STATE (MGT))
  {
  case MGT_DISCONNECT_INITIATED:
    {
     /*
      * processing for state MGT_DISCONNECT_INITIATED
      */
      PALLOC (l2r_disc_cnf, L2R_DISC_CNF);
      PSENDX (CTRL, l2r_disc_cnf);
      SET_STATE (MGT, MGT_ATTACHED);
      break;
    }
  }
  PFREE (rlp_disc_cnf);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_reset_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_RESET_IND received from RLP.
|
|  Parameters  : rlp_reset_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_rlp_reset_ind
                  (
                    T_RLP_RESET_IND      *rlp_reset_ind
                  )
{
  TRACE_FUNCTION ("mgt_rlp_reset_ind()");
  PACCESS        (rlp_reset_ind);

  switch (GET_STATE (MGT))
  {
  case MGT_CONNECTED:
    {
     /*
      * processing for state MGT_CONNECTED
      */
      {
        PALLOC (l2r_reset_ind, L2R_RESET_IND);
        PSENDX (CTRL, l2r_reset_ind);
      }
      sig_mgt_up_clear_req();
      sig_mgt_dn_clear_req();

      {
        PALLOC (rlp_reset_res, RLP_RESET_RES);
        PSENDX (RLP, rlp_reset_res);
      }
      sig_mgt_dn_reconn_req();
      break;
    }
  }
  PFREE (rlp_reset_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_reset_cnf
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_RESET_CNF received from RLP.
|
|  Parameters  : rlp_reset_cnf -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_rlp_reset_cnf
                  (
                    T_RLP_RESET_CNF      *rlp_reset_cnf
                  )
{
  TRACE_FUNCTION ("mgt_rlp_reset_cnf()");
  PACCESS        (rlp_reset_cnf);
  /*
   * RLP_RESET_REQ is never used.
   * Therefore RLP_RESET_CNF does not need to be handled.
   */
  PFREE (rlp_reset_cnf);
}

/*
+------------------------------------------------------------------------------
|  Function    :  mgt_rlp_ui_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_UI_IND received from RLP.
|
|  Parameters  : rlp_ui_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_rlp_ui_ind(T_RLP_UI_IND *rlp_ui_ind)
{
  TRACE_FUNCTION ("mgt_rlp_ui_ind()");
  PACCESS        (rlp_ui_ind);
  /*
  * processing for all states
  * unnumbered information is ignored by L2R
  */
  PFREE (rlp_ui_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_xid_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_XID_IND received from RLP.
|
|  Parameters  : rlp_xid_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_xid_ind(T_RLP_XID_IND *rlp_xid_ind)
{
  UBYTE state;
  TRACE_FUNCTION ("mgt_rlp_xid_ind()");
  PACCESS        (rlp_xid_ind);

  state = GET_STATE (MGT);
  switch (state)
  {
  case MGT_PENDING_ATTACH:
  case MGT_ATTACHED:
  case MGT_PENDING_CONNECT:
  case MGT_CONNECTED:
    {
     /*
      * processing for states MGT_PENDING_ATTACH, MGT_ATTACHED,
      * MGT_PENDING_CONNECT and MGT_CONNECTED
      */

      PALLOC (l2r_xid_ind, L2R_XID_IND);

      l2r_xid_ind->rlp_vers = rlp_xid_ind->rlp_vers;
      l2r_xid_ind->k_ms_iwf = rlp_xid_ind->k_ms_iwf;
      l2r_xid_ind->k_iwf_ms = rlp_xid_ind->k_iwf_ms;
      l2r_xid_ind->t1       = rlp_xid_ind->t1;
      l2r_xid_ind->t2       = rlp_xid_ind->t2;
      l2r_xid_ind->n2       = rlp_xid_ind->n2;
      l2r_xid_ind->pt       = rlp_xid_ind->pt;
      l2r_xid_ind->p0       = rlp_xid_ind->p0;
      l2r_xid_ind->p1       = rlp_xid_ind->p1;
      l2r_xid_ind->p2       = rlp_xid_ind->p2;

      PSENDX (CTRL, l2r_xid_ind);
      if (state EQ MGT_PENDING_ATTACH)
      {
        {
          PALLOC (l2r_activate_cnf, L2R_ACTIVATE_CNF);
          l2r_activate_cnf->ack_flg = L2R_ACK;
          PSENDX (CTRL, l2r_activate_cnf);
        }

        switch (GET_STATE (CONIND))
        {
        case IW_IDLE:
          {
            SET_STATE (MGT, MGT_ATTACHED);
            break;
          }
        case IW_WAIT:
          snd_rlp_connect_res(RLP_ACK);
          mgt_init_connection(TRUE);
          SET_STATE (MGT, MGT_CONNECTED);
          break;
        }
      }
      break;
    }
  }
  PFREE (rlp_xid_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_error_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_ERROR_IND received from RLP.
|
|  Parameters  : rlp_error_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_error_ind(T_RLP_ERROR_IND *rlp_error_ind)
{
  TRACE_FUNCTION ("mgt_rlp_error_ind()");
  PACCESS        (rlp_error_ind);

  mgt_send_l2r_error_ind(rlp_error_ind->cause);
  SET_STATE (MGT, MGT_PERMANENT_ERROR);
  PFREE (rlp_error_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_statistic_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_STATISTIC_IND received from RLP.
|
|  Parameters  : rlp_statistic_ind -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_statistic_ind
                  (
                    T_RLP_STATISTIC_IND *rlp_statistic_ind
                  )
{
  TRACE_FUNCTION ("mgt_rlp_statistic_ind()");
  PACCESS        (rlp_statistic_ind);
  {
  PALLOC (l2r_statistic_ind, L2R_STATISTIC_IND);
  l2r_statistic_ind->error_rate = rlp_statistic_ind->error_rate;
  PSENDX (CTRL, l2r_statistic_ind);
  }
  PFREE (rlp_statistic_ind);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_rlp_detach_cnf
+------------------------------------------------------------------------------
|  Description : Process primitive RLP_DETACH_CNF received from RLP.
|
|  Parameters  : rlp_detach_cnf -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_rlp_detach_cnf(T_RLP_DETACH_CNF *rlp_detach_cnf)
{
  TRACE_FUNCTION ("mgt_rlp_detach_cnf()");
  PACCESS        (rlp_detach_cnf);

  switch (GET_STATE (MGT))
  {
  case MGT_PENDING_DETACH:
    {
    PPASS (rlp_detach_cnf, l2r_deactivate_cnf, L2R_DEACTIVATE_CNF);
    PSENDX (CTRL, l2r_deactivate_cnf);
    SET_STATE (MGT, MGT_DETACHED);
    l2r_data_magic_num = 0;       /* memory is no more initialized */
    break;
    }

  default:
    PFREE (rlp_detach_cnf);
    break;
  }
}

