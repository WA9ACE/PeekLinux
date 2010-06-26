/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_mgtf.c
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_MGTF_C
#define L2R_MGTF_C
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
|  Function    : mgt_init
+------------------------------------------------------------------------------
|  Description : initialise the l2r data for the management process
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_init(T_MGT *dmgt)
{
  TRACE_FUNCTION ("mgt_init()");

  dmgt->FlowCtrlUsed = FALSE;
  dmgt->RiBuSize     = 0;
  dmgt->ConnectPrimType = L2R_CONNECT_IND;
  dmgt->Connected = FALSE;

  INIT_STATE (CONIND, IW_IDLE);
  INIT_STATE (BREAK, IW_IDLE);
  INIT_STATE (MGT, MGT_DETACHED);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_deinit_connection
+------------------------------------------------------------------------------
|  Description : is called when L2R is disconnected
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_deinit_connection(void)
{

  TRACE_FUNCTION ("mgt_deinit_connection()");

  if (l2r_data->mgt.Connected)
  {
    l2r_data->mgt.Connected = FALSE;
    sig_mgt_dn_disc_req();
    sig_mgt_up_disc_req();
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_checkpar
+------------------------------------------------------------------------------
|  Description : Checks the parameter values of an l2r_activate_req
|
|
|  Parameters  : ar -
|
|
|  Return      : TRUE  -
|                FALSE -
+------------------------------------------------------------------------------
*/

GLOBAL BOOL mgt_checkpar(T_L2R_ACTIVATE_REQ *ar)
{
  TRACE_FUNCTION ("mgt_checkpar()");

  /*lint -e568 -e685 operator > always evaluates to false non-negative quantity is never less than zero */
  if (      
      ar->k_ms_iwf > L2R_K_MS_IWF_MAX OR

      ar->k_iwf_ms > L2R_K_IWF_MS_MAX OR

      ar->t2 > L2R_T2_MAX OR 

      ar->n2 < L2R_N2_MIN OR 
      ar->n2 > L2R_N2_MAX OR

      ar->pt > L2R_PT_MAX OR

#ifndef _SIMULATION_
      ar->p0 > L2R_P0_MAX OR
#endif
      ar->p1 < L2R_P1_MIN OR
      ar->p1 > L2R_P1_MAX OR 

      ar->p2 < L2R_P2_MIN OR 
      ar->p2 > L2R_P2_MAX OR

      ar->bytes_per_prim < L2R_BYTES_PER_PRIM_MIN OR
      ar->bytes_per_prim > L2R_BYTES_PER_PRIM_MAX OR

      ar->buffer_size < L2R_BUFFER_SIZE_MIN OR
      ar->buffer_size > L2R_BUFFER_SIZE_MAX OR

      ar->t1 > L2R_T1_MAX
      )
   /*lint +e568 +e685 operator > always evaluates to false non-negative quantity is never less than zero */
  {
    return (FALSE);
  }

  if (
      ar->uil2p NEQ L2R_ISO6429 AND
      ar->uil2p NEQ L2R_COPnoFlCt
      )
  {
    return (FALSE);
  }

  switch (ar->rate)
  {
  case L2R_FULLRATE_14400:
    if (ar->t1 < L2R_T1_MIN_FULLRATE_14400)
    {
      return (FALSE);
    }
    break;

  case L2R_FULLRATE_9600:
    if (ar->t1 < L2R_T1_MIN_FULLRATE_9600)
    {
      return (FALSE);
    }
    break;

  case L2R_FULLRATE_4800:
    if (ar->t1 < L2R_T1_MIN_FULLRATE_4800)
    {
      return (FALSE);
    }
    break;

  case L2R_HALFRATE_4800:
    if (ar->t1 < L2R_T1_MIN_HALFRATE_4800)
    {
      return (FALSE);
    }
    break;

  default:
    return (FALSE);
  }

  /* Finally its really true: all parameters are valid */
  return (TRUE);
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_init_connection
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : indication -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void  mgt_init_connection(BOOL indication)
{
  T_MGT *dmgt = &l2r_data->mgt;
    
  TRACE_FUNCTION ("mgt_init_connection()");

  SET_STATE (BREAK, IW_IDLE);
  sig_mgt_dn_conn_req
  (
    dmgt->FlowCtrlUsed,
    dmgt->RiBuSize,
    dmgt->InstID
  );
  if (indication)
  {
    dmgt->ConnectPrimType = L2R_CONNECT_IND;
  }
  else
  {
    dmgt->ConnectPrimType = L2R_CONNECT_CNF;
  }
  sig_mgt_up_conn_req
  (
    dmgt->FramesPerPrim,
    dmgt->FrameSize,
    dmgt->FlowCtrlUsed,
    dmgt->RiBuSize,
    dmgt->InstID
  );
  dmgt->Connected = TRUE;
}

/*
+------------------------------------------------------------------------------
|  Function    : mgt_send_l2r_error_ind
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : cause -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void mgt_send_l2r_error_ind(USHORT cause)
{
  PALLOC (l2r_error_ind, L2R_ERROR_IND);

  TRACE_FUNCTION ("mgt_send_l2r_error_ind()");

  l2r_error_ind->cause = cause;
  PSENDX (CTRL, l2r_error_ind);
  mgt_deinit_connection();
}

