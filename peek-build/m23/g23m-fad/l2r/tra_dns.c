/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  L2r_dns.c
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
|             L2R of the base station
+-----------------------------------------------------------------------------
*/

#ifndef TRA_DNS_C
#define TRA_DNS_C
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

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+-------------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_dn_dti_conn_setup
+-------------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DL_CONNECT received from process management.
|
|  Parameters  :
|
|  Return      : -
+-------------------------------------------------------------------------------------
*/
GLOBAL void sig_mgt_tra_dn_dti_conn_setup ()
{
  TRACE_FUNCTION ("sig_mgt_tra_dn_dti_conn_setup()");

  switch (GET_STATE (DN))
  {
  case DN_INACTIVE:
    dl_init(&tra_data->dn);
    break;
  }
}

/*
+-------------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_dn_dti_conn_open
+-------------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DL_CONNECT received from process management.
|
|  Parameters  :
|
|  Return      : -
+-------------------------------------------------------------------------------------
*/
GLOBAL void sig_mgt_tra_dn_dti_conn_open ()
{
  TRACE_FUNCTION ("sig_mgt_tra_dn_dti_conn_open()");
  
  switch (GET_STATE (DN))
  {
  case DN_INACTIVE:
    dl_init(&tra_data->dn);
    SET_STATE (DN, DN_IDLE);
    break;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_dn_dti_disc
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DL_DISC received
|                from process management.
|
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_tra_dn_dti_disc(void)
{
  T_TRA_DN *ddl = &tra_data->dn;

  TRACE_FUNCTION ("sig_mgt_tra_dn_dti_disc()");

  SET_STATE (DN, DN_INACTIVE);

  if (ddl->prim NEQ NULL) /* free allocated prims and their lists */
  {
    PFREE_DESC2(ddl->prim)
    ddl->prim = NULL;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_any_tra_dn_send_break_req 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_SEND_BREAK_REQ received
|                from process uplink.
|
|
|  Parameters  : - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_any_tra_dn_send_break_req(void)
{
  T_TRA_DN *ddl = &tra_data->dn;

  TRACE_FUNCTION ("sig_any_tra_dn_send_break_req");
 
  switch (GET_STATE (DN))
  {
  case DN_WAIT:
    if (ddl->Brk_dti_data_ind NEQ NULL)
    {
      dti_send_data(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, ddl->Brk_dti_data_ind); 
      ddl->Brk_dti_data_ind = NULL; /* do not send it twice .. */
      SET_STATE (DN, DN_IDLE);
    }
    break;

  default:
    TRACE_ERROR ("[sig_any_tra_dn_send_break_req] unexpected state");
    break;
  }
}

GLOBAL void sig_mgt_tra_dn_break_req(void)
{
  T_TRA_DN *ddl = &tra_data->dn;

  TRACE_FUNCTION ("sig_mgt_tra_dn_break_req()");

  switch (GET_STATE (DN))
  {
  case DN_SEND:
    SET_STATE (DN, DN_IDLE);
    /* Fall through */
    
  case DN_IDLE:
  case DN_WAIT:
    if (ddl->prim NEQ NULL) /* free allocated prims and their lists */
    {
      PFREE_DESC2(ddl->prim)
      ddl->prim = NULL;
    }
    ddl->list_end = NULL;
    ddl->sa       = FL_INACTIVE;
    ddl->sb       = FL_INACTIVE;
    ddl->x        = FL_INACTIVE;
    break;
  }
}

