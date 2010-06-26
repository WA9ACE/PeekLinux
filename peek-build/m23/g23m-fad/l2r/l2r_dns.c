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

#ifndef L2R_DNS_C
#define L2R_DNS_C
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

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_conn_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_CONN_REQ received
|                from process management.
|
|  Parameters  : flowCtrlUsed - 
|                riBuSize     -
|                inst_id      -
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_conn_req
            (
            BOOL    flowCtrlUsed,
            T_PRIM_DESC_RIBU_SIZE riBuSize,
            UBYTE   inst_id
            )
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("sig_mgt_dn_conn_req()");

  switch (GET_STATE (DN))
  {
  case DN_DISCONNECTED:
    {
    PALLOC (rlp_getdata_req, RLP_GETDATA_REQ);

    ddn->FlowCtrlUsed = flowCtrlUsed;
    ddn->InstID       = inst_id;

    if (riBuSize > MAX_DPRIM_RIBU_SIZE)
    {
      ddn->RiBu.idx.depth = MAX_DPRIM_RIBU_SIZE;
    }
    else
    {
      ddn->RiBu.idx.depth = riBuSize;
    }

    dn_init_ribu();

    ddn->FlowThresh = ddn->RiBu.idx.depth >> 1;

    ddn->DnFlow = FL_INACTIVE;
    ddn->UpFlow = FL_INACTIVE;
    ddn->ULFlow = FL_INACTIVE;
    ddn->LLFlow = FL_INACTIVE;
    dn_merge_flow();

    ddn->ReportMrgFlow = FALSE;
    
    PSENDX (RLP, rlp_getdata_req);
    
    SET_STATE (DN_LL, IW_WAIT);
    SET_STATE (DN_UL, IW_IDLE);
    SET_STATE (DN, DN_WAIT_FIRST_DATA);
    break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_disc_req 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_DISC_REQ received
|                from process management.
|
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_disc_req(void)
{
  TRACE_FUNCTION ("sig_mgt_dn_disc_req()");

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    dn_free_all_prims();
    SET_STATE(DN, DN_DISCONNECTED);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_break_req 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_BREAK_REQ received
|                from process management.
|
|
|  Parameters  : - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_break_req(void)
{
  TRACE_FUNCTION ("sig_mgt_dn_break_req()");

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    dn_free_all_prims();
    dn_init_ribu();
    dn_check_flow();
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_clear_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_CLEAR_REQ received
|                from process management.
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_clear_req(void)
{
  TRACE_FUNCTION ("sig_mgt_dn_clear_req()");

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    dn_free_all_prims();
    dn_init_ribu();
    dn_check_flow();
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_reconn_req 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_RECONN_REQ received
|                from process management.
|
|  Parameters  : - 
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_reconn_req(void)
{
  TRACE_FUNCTION ("sig_mgt_dn_reconn_req()");

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    PALLOC (rlp_getdata_req, RLP_GETDATA_REQ);
    PSENDX (RLP, rlp_getdata_req);
    SET_STATE (DN_LL, IW_WAIT);
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_dti_conn_setup
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_DN_DTI_CONN_SETUP
|                received from process management.
|
|  Parameters  : link_id            channel id of the link
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_dn_dti_conn_setup (ULONG link_id)
{
  TRACE_FUNCTION ("sig_mgt_dn_dti_conn_setup()");

  l2r_data->dn.link_id = link_id;
  SET_STATE (DN_UL, IW_IDLE); /* jk: to IDLE in order to be able to respond with DTI_GETDATA_REQ */
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_up_dn_flow 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UP_DN_FLOW received
|                from process uplink.
|
|  Parameters  : flow - 
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/

GLOBAL void sig_up_dn_flow(T_FLOW flow)
{
  if (flow EQ FL_ACTIVE)
  {
    TRACE_FUNCTION ("sig_up_dn_flow(ACTIVE)");
  }
  else
  {
    TRACE_FUNCTION ("sig_up_dn_flow(INACTIVE)");
  }

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    T_FLOW oldFlow = l2r_data->dn.UpFlow;
    l2r_data->dn.UpFlow = flow;

    if (flow NEQ oldFlow)
    {
      dn_merge_flow();
    }
    dn_cond_report_status();
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_up_dn_ul_flow 
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UP_DN_UL_FLOW received
|                from process uplink.
|
|
|  Parameters  : flow - 
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/

GLOBAL void sig_up_dn_ul_flow(T_FLOW flow)
{
  if (flow EQ FL_ACTIVE)
  {
    TRACE_FUNCTION ("sig_up_dn_ul_flow(ACTIVE)");
  }
  else
  {
    TRACE_FUNCTION ("sig_up_dn_ul_flow(INACTIVE)");
  }

  if (GET_STATE (DN) EQ DN_CONNECTED)
  {
    l2r_data->dn.ULFlow = flow;
    if (l2r_data->dn.ULFlow EQ FL_INACTIVE AND GET_STATE (DN_UL) EQ IW_WAIT
        AND l2r_data->dn.DtiConnected /*jk: data send only when DTI connected*/
       )
    {
      dn_send_data_ind();
      SET_STATE (DN_UL, IW_IDLE);
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_dn_send_break_req 
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

GLOBAL void sig_mgt_dn_send_break_req(void)
{
  TRACE_FUNCTION ("sig_mgt_dn_send_break_req");
 
  switch (GET_STATE (DN))
  {
  case DN_CONNECTED:
  case DN_WAIT_FIRST_DATA:
     /*
      * processing for state MGT_CONNECTED
      */
    if (l2r_data->dn.Brk_dti_data_ind NEQ NULL && GET_STATE (DN_UL) EQ IW_WAIT)
    {
      dti_send_data (
        l2r_hDTI,
        L2R_DTI_UP_DEF_INSTANCE,
        L2R_DTI_UP_INTERFACE,
        L2R_DTI_UP_CHANNEL,
        l2r_data->dn.Brk_dti_data_ind
        );        
      SET_STATE (DN_UL, IW_IDLE);
      /* do not send it twice .. */
      l2r_data->dn.Brk_dti_data_ind = NULL;
    }
    break;
  default:
    TRACE_ERROR ("[sig_mgt_dn_send_break_req] unexpected state");
    break;
  }
}
