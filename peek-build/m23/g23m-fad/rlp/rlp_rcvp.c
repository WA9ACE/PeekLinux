/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_rcvp.c
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

#ifndef RLP_RCVP_C
#define RLP_RCVP_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

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
|                This function is called if the RA_READY_IND primitive
|                is received or from the signal processing in rlp_pei.
|
|
|  Parameters  : ra_ready_ind -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

#ifdef _SIMULATION_
GLOBAL const void rcv_ra_ready_ind(T_RA_READY_IND *ra_ready_ind)
#else
GLOBAL void rcv_ra_ready_ind(void)
#endif
{
#ifdef _SIMULATION_
  TRACE_FUNCTION ("rcv_ra_ready_ind()");
  PACCESS        (ra_ready_ind);
#endif

  if (GET_STATE (RCV) EQ RPDU_WAIT_FOR_A_BLOCK)
    sig_rcv_ker_ready_ind ();

#ifdef _SIMULATION_
  PFREE (ra_ready_ind);
#endif
}

/*
+------------------------------------------------------------------------------
|  Function    : rcv_ra_data_ind
+------------------------------------------------------------------------------
|  Description : Process primitive RA_DATA_IND received from RA.
|                This function is called if the primitive RA_DATA_IND is
|                received or from the signal processing in rlp_pei.
|
|  Parameters  : ra_data_ind -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void rcv_ra_data_ind(T_RA_DATA_IND *ra_data_ind)
{
#ifdef _SIMULATION_
  TRACE_FUNCTION ("rcv_ra_data_ind()");
  PACCESS        (ra_data_ind);
#endif

  if (GET_STATE (RCV) EQ RPDU_WAIT_FOR_A_BLOCK)
  {
#ifdef _SIMULATION_
    sig_rcv_ker_rawdata_ind (ra_data_ind);
#else
    sig_rcv_ker_rawdata_ind ();
#endif
  }
#ifdef _SIMULATION_
  else
  {
    PFREE (ra_data_ind);
  }
#endif
}
