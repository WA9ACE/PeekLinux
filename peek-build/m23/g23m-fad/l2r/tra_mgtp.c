/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  tra_mgtp.c
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
|             of incomming primitives for the component TRA
+-----------------------------------------------------------------------------
*/

#ifndef TRA_MGTP_C
#define TRA_MGTP_C
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
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"

#include "cl_ribu.h"
#include "tra_pei.h"
#include "tra.h"

#include "ra_l1int.h"

/*==== CONST =======================================================*/
/*==== TYPES =======================================================*/
/*==== VAR EXPORT ==================================================*/
/*==== VAR LOCAL ===================================================*/
/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : mgt_tra_activate_req
+------------------------------------------------------------------------------
|  Description : Process primitive TRA_ACTIVATE_REQ  received from ACI
|
|  Parameters  : tra_activate_req
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_tra_activate_req(T_TRA_ACTIVATE_REQ *tra_activate_req)
{
  TRACE_EVENT ("mgt_tra_activate_req()");
  PACCESS (tra_activate_req);

  /***************************************
   *
   *  Initialize Shared Fax/Data memory  *
   *
   ***************************************/

  tra_data = tra_data_first_elem;
  memset (tra_data, 0, sizeof (T_TRA_DATA));
  tra_data->up.fd.buf = &tra_data->up.to_ra[0];
  l2r_data_mode = TRANSP;                     /* global flag for mode */
  rbm_init(&tra_data->dn);
  INIT_STATE (MGT, MGT_ACTIVE);
  INIT_STATE (DTI, DTI_CLOSED);
  INIT_STATE (DN, DN_INACTIVE);
  INIT_STATE (UP, UP_INACTIVE);

  {
    PPASS (tra_activate_req, tra_activate_cnf, TRA_ACTIVATE_CNF);
    tra_activate_cnf->ack_flg = TRA_ACK;
    PSENDX (CTRL, tra_activate_cnf);
  }

}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_tra_deactivate_req
+------------------------------------------------------------------------------
|  Description : Process primitive TRA_DEACTIVATE_REQ received from ACI/MMI
|
|  Parameters  : tra_deactivate_req
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_tra_deactivate_req(T_TRA_DEACTIVATE_REQ *tra_deactivate_req)
{
  TRACE_FUNCTION ("mgt_tra_deactivate_req()");
  PACCESS        (tra_deactivate_req);

  switch (GET_STATE (MGT))
  {
  case MGT_IDLE:
    sig_mgt_tra_up_dti_disc();
    sig_mgt_tra_dn_dti_disc();
    /* no break */
  case MGT_ACTIVE:
    {
      cl_ribu_release(&tra_data->dn.ribu); /* frees downlink FIFO */
    }
    send_tra_deactivate_cnf();
    SET_STATE (MGT, MGT_INACTIVE);
    break;

  case MGT_INACTIVE:
    TRACE_EVENT("MGT was inactive!");
    break;

  default:
    TRACE_EVENT("MGT state not recognized!");
    break;
  }
  PFREE (tra_deactivate_req);

  if (GET_STATE (DTI) NEQ DTI_CLOSED)
  {
    dti_close(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, FALSE);
    SET_STATE (DTI, DTI_CLOSED);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_tra_dti_req
+------------------------------------------------------------------------------
|  Description : Process primitive TRA_DTI_REQ received from ACI/MMI
|
|  Parameters  : tra_dti_req
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void mgt_tra_dti_req(T_TRA_DTI_REQ *tra_dti_req)
{
  UBYTE isOK;

  TRACE_FUNCTION ("mgt_tra_dti_req()");
  PACCESS        (tra_dti_req);

  switch (GET_STATE (MGT))
  {
  case MGT_ACTIVE:
    if (tra_dti_req->dti_conn EQ TRA_CONNECT_DTI)
    {
      if (GET_STATE(DTI) NEQ DTI_CLOSED)
      {
        dti_close(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, FALSE);
        SET_STATE (DTI, DTI_CLOSED);
      }
      /*
       * open new dti library communication channel
       */
      SET_STATE (DTI, DTI_SETUP);

      isOK = dti_open(
        l2r_hDTI,
        TRA_DTI_UP_INSTANCE,
        TRA_DTI_UP_INTERFACE,
        TRA_DTI_UP_CHANNEL,
        0,
        tra_dti_req->dti_direction,
        DTI_QUEUE_UNUSED,
        DTI_VERSION_10,
        (U8*)tra_dti_req->entity_name,
        tra_dti_req->link_id
        );

      if (isOK EQ FALSE)
      {
        PALLOC(tra_dti_cnf, TRA_DTI_CNF);
        SET_STATE (DTI, DTI_CLOSED);
        tra_dti_cnf->dti_conn = TRA_DISCONNECT_DTI;
        tra_dti_cnf->link_id = tra_dti_req->link_id;
        PSENDX (CTRL, tra_dti_cnf);
        PFREE(tra_dti_req);
        return;
      }
      sig_mgt_tra_dn_dti_conn_setup ();
      sig_mgt_tra_up_dti_conn_setup ();
    }
    break;

  case MGT_IDLE:
    if (tra_dti_req->dti_conn EQ TRA_DISCONNECT_DTI)
    {
      sig_mgt_tra_up_dti_disc();
      sig_mgt_tra_dn_dti_disc();
      if (GET_STATE (DTI) NEQ DTI_CLOSED)
      {
        dti_close(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, FALSE);
        SET_STATE (DTI, DTI_CLOSED);
      }
      SET_STATE (MGT, MGT_ACTIVE);
      send_tra_dti_cnf(tra_dti_req);
    }
    break;
  }
  PFREE (tra_dti_req);
}

/*
+------------------------------------------------------------------------------
|  Function    : rcv_ra_break_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RA_BREAK_IND received from RA.
|                This function is called if the RA_BREAK_IND primtive
|                is received or from the signal processing in l2r_pei.
|
|  Parameters  : ra_break_ind
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rcv_ra_break_ind(T_RA_BREAK_IND *ra_break_ind)
{
  U8 sa;
  U8 sb;
  U8 flow;
  USHORT break_len = ra_break_ind->break_len;

  TRACE_FUNCTION ("rcv_ra_break_ind()");

#ifdef _SIMULATION_
  PACCESS        (ra_break_ind);
#endif

  /* xxxxx not properly initialized.. */

  sa = tra_data->up.sa;  /* last sa bit in this primitive  */
  sb = tra_data->up.sb;  /* last sb bit in this primitive  */
  flow = tra_data->up.x; /* last x bit in this primitive   */

#ifdef _SIMULATION_
  PFREE(ra_break_ind);
#endif

  /* Send signal to mgt and set BREAK dti_data prim */
  sig_dn_tra_mgt_break_ind(sa, sb, flow, break_len);
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_tra_mgt_connection_opened_ind
+------------------------------------------------------------------------------
| Description : Handles the DTILIB callback call DTI_REASON_CONNECTION_OPENED
|
|               This signal means that a dti connection has been opened
|               successfully.
|               Since it is called directly from the dtilib callback function
|               it is handled like a primitive, here..
|
|  Parameters  : -
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_tra_mgt_connection_opened_ind()
{
  TRACE_FUNCTION ("sig_dti_tra_mgt_connection_opened_ind()");

  if (GET_STATE (DTI) EQ DTI_SETUP)
  {
    PALLOC (tra_dti_cnf, TRA_DTI_CNF);
    TRACE_EVENT("MGT: DTI received from ACI");

    SET_STATE (DTI, DTI_IDLE);
    SET_STATE (MGT, MGT_IDLE);

    sig_mgt_tra_dn_dti_conn_open ();
    sig_mgt_tra_up_dti_conn_open ();

    tra_dti_cnf->dti_conn = TRA_CONNECT_DTI;
    if (dti_resolve_link_id (l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, &tra_dti_cnf->link_id) EQ FALSE)
    {
      TRACE_ERROR ("sig_dti_tra_mgt_connection_opened_ind(): link_id not found!");
      return; /* error, no appropriate link_id found */
    }
    PSENDX (CTRL, tra_dti_cnf);
#ifdef _TARGET_
    {
      T_RA_DATATRANS_REQ RA_datatrans_req;
      l1i_ra_datatrans_req(&RA_datatrans_req);
    }
#endif
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_tra_mgt_connection_closed_ind
+------------------------------------------------------------------------------
| Description : Handles the DTILIB callback call DTI_REASON_CONNECTION_CLOSED
|
|               This signal means that a dti connection has been closed
|               Since it is called directly from the dtilib callback function
|               it is handled like a primitive, here..
|
|  Parameters  : -
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_tra_mgt_connection_closed_ind()
{
  TRACE_FUNCTION ("sig_dti_tra_mgt_connection_closed_ind()");

  if (GET_STATE(DTI) NEQ DTI_CLOSED)
  {
    PALLOC (tra_dti_ind, TRA_DTI_IND);

    SET_STATE (DTI, DTI_CLOSED);

    sig_mgt_tra_up_dti_disc();
    sig_mgt_tra_dn_dti_disc();

    if (dti_resolve_link_id(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, &tra_dti_ind->link_id) EQ FALSE)
    {
      TRACE_ERROR ("sig_dti_tra_mgt_connection_closed_ind(): link_id not found!");
      return; /* error, no appropriate link_id found */
    }
    TRACE_EVENT("MGT: DTI passed to ACI");
    PSENDX (CTRL, tra_dti_ind);
  }
}

