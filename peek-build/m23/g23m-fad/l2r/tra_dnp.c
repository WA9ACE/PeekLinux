/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_dnp.c
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

#ifndef TRA_DNP_C
#define TRA_DNP_C
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
+------------------------------------------------------------------------------
|  Function    : sig_dti_tra_dn_tx_buffer_ready_ind
+------------------------------------------------------------------------------
|  Description : Process signal DTI_REASON_TX_BUFFER_READY received from
|                dti library callback function.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_dti_tra_dn_tx_buffer_ready_ind()
{
  TRACE_FUNCTION ("sig_dti_tra_dn_tx_buffer_ready_ind()");

  if (tra_data->dn.Brk_dti_data_ind NEQ NULL)
  {
    /*
     * if there is a pending break indication to be sent via DTI,
     * send it now
     */
    sig_any_tra_dn_send_break_req();
    return;
  }

  switch (GET_STATE (DN))
  {
    case DN_IDLE:
    {
      SET_STATE (DN, DN_WAIT);
      break;
    }
    case DN_SEND:
    {
      if (dl_send_data_ind())
      {
        SET_STATE (DN, DN_IDLE);
      }
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_dti_tra_dn_tx_buffer_full_ind
+------------------------------------------------------------------------------
|  Description : Process signal DTI_REASON_TX_BUFFER_FULL received from
|                dti library callback function.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------

  The buffer function of DTILIB is not used.
  So there is nothing to be done here yet...

*/

GLOBAL void sig_dti_tra_dn_tx_buffer_full_ind()
{
  TRACE_FUNCTION ("sig_dti_tra_dn_tx_buffer_full_ind()");
}


/*
+------------------------------------------------------------------------------
|  Function    : rcv_ra_data_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RA_DATA_IND received from RA.
|                This function is called if the primitive RA_DATA_IND is
|                received or from the signal processing in l2r_pei (tra).
|
|  Parameters  : ra_data_ind -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rcv_ra_data_ind(T_RA_DATA_IND *ra_data_ind)
{
  T_TRA_DN *ddl = &tra_data->dn;
  T_FD *pFD = NULL;

  TRACE_FUNCTION("rcv_ra_data_ind()");

  if (GET_STATE (MGT) NEQ MGT_IDLE)
  {
    TRACE_EVENT ("data rcvd while not in state MGT_IDLE");

    while (cl_ribu_data_avail(ddl->ribu))
    {
      cl_ribu_get(ddl->ribu);
    }

#ifdef _SIMULATION_
    PFREE (ra_data_ind);
#endif
    return;
  }

#ifdef _SIMULATION_
  PACCESS (ra_data_ind);
  if (ra_data_ind->sdu.l_buf)
  {
    pFD = tra_get_next_FrameDesc();
    TRACE_EVENT_P2("put - wi=%d - ri=%d", ddl->ribu->idx.wi, ddl->ribu->idx.ri);
    memcpy(pFD->buf, ra_data_ind->sdu.buf, ra_data_ind->sdu.l_buf >> 3);
    pFD->len = ra_data_ind->sdu.l_buf >> 3;
  }
#endif

  ddl->sa = 0;
  ddl->sb = 0;
  ddl->x = 0;

  if (ra_data_ind->status & ST_SA)
    ddl->sa = DTI_SA_OFF;
  if (ra_data_ind->status & ST_SB)
    ddl->sb = DTI_SB_OFF;
  if (ra_data_ind->status & ST_X)
    ddl->x = DTI_FLOW_OFF;

  if (ddl->ribu->idx.filled > 1)
    TRACE_EVENT_P3("RIBU ddl: filled=%d; wi=%d; ri=%d", ddl->ribu->idx.filled, ddl->ribu->idx.wi, ddl->ribu->idx.ri);

  while (cl_ribu_data_avail(ddl->ribu))
  {
    pFD = cl_ribu_get(ddl->ribu);
    dl_prep_data_ind(pFD);
  }

  switch (GET_STATE (DN))
  {
  case DN_SEND:
    break;

  case DN_IDLE:
    if (pFD NEQ NULL)
    {
      SET_STATE (DN, DN_SEND);
    }
    break;

  case DN_WAIT:
    if (dl_send_data_ind())
    {
      SET_STATE (DN, DN_IDLE);
    }
    break;

  default:
    break;
  }
#ifdef _SIMULATION_
  PFREE (ra_data_ind);
#endif
}
