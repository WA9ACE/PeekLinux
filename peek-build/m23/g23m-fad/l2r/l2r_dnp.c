/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_dnp.c
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
|             L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_DNP_C
#define L2R_DNP_C
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
|  Function    : sig_dti_dn_tx_buffer_full_ind
+------------------------------------------------------------------------------
|  Description : Process signal DTI_REASON_TX_BUFFER_FULL received from
|                dti library callback function.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
The buffer function of DTILIB is not used.
So there is nothing to be done here yet..
*/
GLOBAL void sig_dti_dn_tx_buffer_full_ind()
{
  TRACE_FUNCTION ("sig_dti_dn_tx_buffer_full_ind()");
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_dn_tx_buffer_ready_ind
+------------------------------------------------------------------------------
|  Description : Process signal DTI_REASON_TX_BUFFER_READY received from
|                dti library callback function.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_dn_tx_buffer_ready_ind()
{
  T_DN *ddn = &l2r_data->dn;

  TRACE_FUNCTION ("sig_dti_dn_tx_buffer_ready_ind()");

  if(ddn->Brk_dti_data_ind NEQ NULL)
  {
    /*
     * if there is a pending break indication to be sent via dti,
     * send it now and go waiting for the next flow control primitive
     */
    SET_STATE (DN_UL, IW_WAIT);
    sig_mgt_dn_send_break_req();
    return;
  }

  switch (GET_STATE (DN))
  {
  case DN_CONNECTED:
    if (!ddn->DtiConnected) /*jk: data send only when DTI connected*/
      break;

    if ((ddn->RiBu.idx.filled AND ddn->ULFlow EQ FL_INACTIVE) OR ddn->ReportMrgFlow)
    {
      dn_send_data_ind();  /* primitive is in ring buffer or flow state must be reported*/
    }
    else
    {
      SET_STATE (DN_UL, IW_WAIT);  /* no primitive is ready */
    }

    dn_cond_free_prims();
    dn_check_flow();

    switch (GET_STATE (DN_LL))
    {
    case IW_IDLE:
      dn_cond_req_data();
      break;
    }
    break;
  }
}

GLOBAL void dn_rlp_data_ind(T_RLP_DATA_IND *rlp_data_ind)
{
  T_DN *ddn = &l2r_data->dn;

  T_PRIM_INDEX frames;
  T_PRIM_INDEX emptyfr;
  T_PRIM_INDEX r_index;
  BOOL break_found;
  T_BIT sa;
  T_BIT sb;
  T_FLOW flow_brk;
  T_FLOW flow_gen;

  TRACE_FUNCTION ("dn_rlp_data_ind()");
  PACCESS        (rlp_data_ind);

  switch (GET_STATE (DN))
  {
  case DN_CONNECTED:
  case DN_WAIT_FIRST_DATA:
    dn_scan_break_req
      (
      rlp_data_ind,
      &break_found,
      &r_index,
      &frames,
      &emptyfr,
      &sa,
      &sb,
      &flow_brk,
      &flow_gen
      );

    if (break_found EQ FALSE)
    {
      if (frames > 0)
      {
        if (GET_STATE (DN) EQ DN_WAIT_FIRST_DATA AND sb EQ 0)
        {
          sig_dn_mgt_first_data_ind();
          SET_STATE (DN, DN_CONNECTED);
        }
        dn_store_status(flow_gen); /* stores flow ctrl state only */
        dn_store_prim(rlp_data_ind, 0); /* L2R-ENH-7527 quick fix */
      }
      else
      {
        dn_cond_report_status();
        PFREE (rlp_data_ind);
      }
    }
    else /* break found */
    {
      dn_free_all_prims();
      dn_init_ribu();
      dn_check_flow();

      if (r_index EQ frames)
      {
        dn_store_status(flow_brk);
        PFREE (rlp_data_ind);
      }
      else
      {
        dn_store_status(flow_gen);
        dn_store_prim(rlp_data_ind, r_index);
      }
      sig_dn_mgt_break_ind(sa, sb, ddn->MrgFlow);
      ddn->LastState = sa << SO_SA_BIT | sb << SO_SB_BIT;
    }
    dn_cond_req_data();
    break;
  }
}

