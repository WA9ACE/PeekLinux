/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_upp.c
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

#ifndef TRA_UPP_C
#define TRA_UPP_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/

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
+------------------------------------------------------------------------------
|  Function    : rcv_ra_ready_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RA_READY_IND received from RA.
|                This function is called if the RA_READY_IND primtive
|                is received
|
|  Parameters  : ra_ready_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rcv_ra_ready_ind(T_RA_READY_IND *ra_ready_ind)
{
  T_TRA_UP *dul = &tra_data->up;

#ifdef _SIMULATION_
  TRACE_EVENT_P1 ("rcv_ra_ready_ind(), req_frames: 0x%x", ra_ready_ind->req_frames);
  PACCESS (ra_ready_ind);
#endif

  dul->req_frames = ra_ready_ind->req_frames;
  
  switch (GET_STATE (UP))
  {
  case UP_IDLE:
    SET_STATE (UP, UP_WAIT);
    break;

  case UP_SEND:
    snd_data_to_RA();
    if (dul->Prim->desc_list2.first EQ 0)
    {
      if (dul->Prim NEQ NULL)
      {
        PFREE(dul->Prim);
        dul->Prim = NULL;
      }
      up_start_dti_flow();
      SET_STATE (UP, UP_IDLE);
    }
    break;
  }
#ifdef _SIMULATION_
  PFREE (ra_ready_ind);
#endif
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_tra_up_data_received_ind
+------------------------------------------------------------------------------
|  Description : Process primitive DTI2_DATA_REQ received from CSDIWF.
|
|  Parameters  : dti_data_req
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_dti_tra_up_data_received_ind(T_DTI2_DATA_REQ *dti_data_req)
{
  T_TRA_UP *dul = &tra_data->up;

  TRACE_FUNCTION ("sig_dti_tra_up_data_received_ind()");
  
  PACCESS        (dti_data_req);

  dul->Prim = dti_data_req;
  
  dul->sa = dti_data_req->parameters.st_lines.st_line_sa;
  dul->sb = dti_data_req->parameters.st_lines.st_line_sb;
  dul->x  = dti_data_req->parameters.st_lines.st_flow;

  /*
  TRACE_EVENT_P1("new data: %02X", dti_data_req->desc_list2.list_len);
  */

  if (GET_STATE(DTI) NEQ DTI_CLOSED)
  {
    /* 
     * do not send flow control primitives automatically 
     */
    dti_stop(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL);        
  }

  /* Detect break request */
  if (dti_data_req->parameters.st_lines.st_break_len NEQ DTI_BREAK_OFF)
  {
    USHORT break_len = dti_data_req->parameters.st_lines.st_break_len;

    PFREE(dti_data_req); /* no more used */

    switch (GET_STATE (UP))
    {
    case UP_SEND:
      /*
       * No more data to send
       */
      SET_STATE (UP, UP_IDLE);
      /* Fall through */

    case UP_IDLE:
    case UP_WAIT:
      sig_up_tra_mgt_break_ind(break_len);
      up_start_dti_flow();
      break;
    }
    return; /* xxxxx - send data or do not send it??! */
  }

  switch (GET_STATE (UP))
  {
  case UP_SEND:
    TRACE_EVENT("still in UP_SEND!");
    break;

  case UP_IDLE:
    SET_STATE (UP, UP_SEND);
    break;

  case UP_WAIT:
    snd_data_to_RA();
    if (dti_data_req->desc_list2.list_len EQ 0) /* data is copied completely */
    {
      up_start_dti_flow();
      SET_STATE (UP, UP_IDLE);
      PFREE(dti_data_req);
      dul->Prim = NULL;
    }
    else
    {
      SET_STATE (UP, UP_SEND);
    }
    break;
  }
}

