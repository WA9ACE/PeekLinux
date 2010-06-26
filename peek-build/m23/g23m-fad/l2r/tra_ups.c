/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_ups.c
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


#ifndef TRA_UPS_C
#define TRA_UPS_C
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
|  Function    : sig_mgt_tra_up_dti_conn_setup
+-------------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UL_CONNECT received from process management.
|
|  Parameters  : -
|
|  Return      : -
+-------------------------------------------------------------------------------------
*/
GLOBAL void sig_mgt_tra_up_dti_conn_setup ()
{
  TRACE_FUNCTION ("sig_mgt_tra_up_dti_conn_setup()");

  switch (GET_STATE (UP))
  {
  case UP_INACTIVE:
    ul_init(&tra_data->up);
    break;
  }
}


/*
+-------------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_up_dti_conn_open
+-------------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UL_CONNECT received from process management.
|
|  Parameters  : -
|
|  Return      : -
+-------------------------------------------------------------------------------------
*/
GLOBAL void sig_mgt_tra_up_dti_conn_open ()
{
  TRACE_FUNCTION ("sig_mgt_tra_up_dti_conn_open()");

  switch (GET_STATE (UP))
  {
  case UP_INACTIVE:
    /* changed from _IDLE,
     * for not having to wait for RA_READY_IND
     */
    SET_STATE (UP, UP_WAIT);
    up_start_dti_flow();
    break;
  }
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_up_dti_disc
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UL_DISC received from
|                process management.
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void sig_mgt_tra_up_dti_disc(void)
{
  TRACE_FUNCTION ("sig_mgt_tra_up_dti_disc()");

  if (tra_data->up.Prim NEQ NULL) /* free allocated prims and their lists */
  {
    PFREE_DESC2(tra_data->up.Prim)
    tra_data->up.Prim = NULL;
  }

  SET_STATE (UP, UP_INACTIVE);
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_up_sendbreak_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_SEND_BREAK_REQ 
|                received from process management.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/
GLOBAL void sig_mgt_tra_up_send_break_req(USHORT break_len)
{
  TRACE_FUNCTION ("sig_mgt_tra_up_send_break_req()");

  /* xxxxx correct location? Lock ringbuffer! */

  snd_break_to_RA(break_len);
}


/*
+------------------------------------------------------------------------------
|  Function    : sig_mgt_tra_up_break_req
+------------------------------------------------------------------------------
|  Description : Process signal SIG_MGT_UP_BREAK_REQ received from process management.
|
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void sig_mgt_tra_up_break_req(void)
{
  T_TRA_UP *dul = &tra_data->up;

  TRACE_FUNCTION ("sig_mgt_tra_up_break_req()");

  switch (GET_STATE (UP))
  {
    case UP_SEND:
      SET_STATE (UP, UP_IDLE);
      /* Fall through */
      
    case UP_IDLE:
    case UP_WAIT:
      /*
       * Reset
       */
      dul->sa             = FL_INACTIVE;
      dul->sb             = FL_INACTIVE;
      dul->x              = FL_INACTIVE;
      dul->List_off       = 0;

      if (dul->Prim NEQ NULL) /* free allocated prims and their lists */
      {
        PFREE_DESC2(dul->Prim)
        dul->Prim = NULL;
      }
      break;
  }
}


