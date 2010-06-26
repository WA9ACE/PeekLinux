/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_mgts.c
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

#ifndef L2R_MGTS_C
#define L2R_MGTS_C
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
|  Function    : sig_dn_mgt_break_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_DN_MGT_BREAK_IND received from process DN. 
|
|  Parameters  : sa   - 
|                sb
|                flow            
|
|  Return      : 
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dn_mgt_break_ind(T_BIT sa, T_BIT sb, T_FLOW flow)
{
  TRACE_FUNCTION ("sig_dn_mgt_break_ind()");
  {
    PALLOC (dti_data_ind, DTI2_DATA_IND);

    dti_data_ind->parameters.st_lines.st_line_sa = sa;
    dti_data_ind->parameters.st_lines.st_line_sb = sb;

    switch (flow)
    {
    case FL_ACTIVE:
      dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_OFF;
      break;
    case FL_INACTIVE:
      dti_data_ind->parameters.st_lines.st_flow = DTI_FLOW_ON;
      break;
    }
    dti_data_ind->desc_list2.first                  = 0;
    dti_data_ind->desc_list2.list_len               = 0;
    dti_data_ind->parameters.st_lines.st_break_len  = L2R_BREAK_LEN;
    dti_data_ind->link_id                           = l2r_data->mgt.link_id;
    dti_data_ind->parameters.p_id                   = DTI_PID_UOS;

    l2r_data->dn.Brk_dti_data_ind = dti_data_ind;
    sig_mgt_dn_send_break_req();
  }
  sig_mgt_up_clear_req();
}

/*
+----------------------------------------------------------------------------------
|  Function    : sig_dn_mgt_first_data_ind
+----------------------------------------------------------------------------------
|  Description : Process signal SIG_DN_MGT_FIRST_DATA_IND received from process DN.
|
|  Parameters  : -
|                 
|
|  Return      : -
+----------------------------------------------------------------------------------
*/

GLOBAL void sig_dn_mgt_first_data_ind(void)
{
  TRACE_FUNCTION ("sig_dn_mgt_first_data_ind()");

  if (GET_STATE (MGT) EQ MGT_CONNECTED)
  {
    if (l2r_data->mgt.ConnectPrimType EQ L2R_CONNECT_IND)
    {
      PALLOC (l2r_connect_ind, L2R_CONNECT_IND);
      PSENDX (CTRL, l2r_connect_ind);
    }
    else
    {    
      mgt_l2r_connect_cnf(L2R_ACK);
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_up_mgt_break_ind
+------------------------------------------------------------------------------
|  Description : Process signal SIG_UP_MGT_BREAK_IND received from process DN.
|
|  Parameters  : T_DTI2_DATA_REQ  *dti_data_req
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_up_mgt_break_ind(T_DTI2_DATA_REQ *dti_data_req)
{
  T_FLOW flow = FL_INVALID;

  TRACE_FUNCTION ("sig_up_mgt_break_ind()");

  PACCESS (dti_data_req);

  if (GET_STATE (MGT) EQ MGT_CONNECTED)
  {
    switch (GET_STATE (BREAK))
    {
    case IW_WAIT:
      break;
    case IW_IDLE:
      switch (dti_data_req->parameters.st_lines.st_flow)
      {
      case DTI_FLOW_ON:
        flow = FL_INACTIVE;
        break;
      case DTI_FLOW_OFF:
        flow = FL_ACTIVE;
        break;
      }
      sig_mgt_dn_break_req();

      sig_mgt_up_break_req
        (
        (UBYTE)(dti_data_req->parameters.st_lines.st_line_sa EQ DTI_SA_ON ? DTI_SA_ON : DTI_SA_OFF),
        (UBYTE)(dti_data_req->parameters.st_lines.st_line_sb EQ DTI_SB_ON ? DTI_SB_ON : DTI_SB_OFF),
        flow);

      SET_STATE (BREAK, IW_WAIT);
    }
  }
}
