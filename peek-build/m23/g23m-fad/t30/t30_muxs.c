/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :  J:\g23m-fad\t30\t30_muxs.c
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
|             of incomming signals for the component T30
|             of the mobile station
+-----------------------------------------------------------------------------
*/

#ifndef T30_MUXS_C
#define T30_MUXS_C
#endif

#define ENTITY_T30

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_t30.h"
#include "mon_t30.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */
#include "t30.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MUXS            |
| STATE   : code                       ROUTINE : sig_bcs_mux_bcs_req |
+--------------------------------------------------------------------+

  PURPOSE : Process signal BCS_REQ received from process BCS formatter.
            The signal requests to send BCS data to FAD.
*/

GLOBAL void sig_bcs_mux_bcs_req (T_FAD_DATA_REQ *fad_data_req)
{
  TRACE_FUNCTION ("sig_bcs_mux_bcs_req()");
  PACCESS        (fad_data_req);

  switch (GET_STATE (MUX))
  {
    case T30_MUX_BCS:
#ifdef KER_DEBUG_BCS
      ker_debug ("fad_data_req", fad_data_req->sdu.buf, (USHORT)(fad_data_req->sdu.l_buf >> 3));
#endif
      PSENDX (FAD, fad_data_req);
      break;

    default:
      PFREE (fad_data_req);
      break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MUXS            |
| STATE   : code                       ROUTINE : sig_ker_mux_mux_req |
+--------------------------------------------------------------------+

  PURPOSE : Process signal MUX_REQ received from process kernel.
            The signal switches the state of the multiplexer.
*/

GLOBAL void sig_ker_mux_mux_req (void)
{
  TRACE_FUNCTION ("sig_ker_mux_mux_req()");

  switch (GET_STATE (MUX))
  {
  case T30_MUX_OFF:
  case T30_MUX_BCS:
  case T30_MUX_MSG:
  {
    switch (t30_data->mux.mode)
    {
    case MUX_OFF:  SET_STATE (MUX, T30_MUX_OFF);  break;
    case MUX_BCS:  SET_STATE (MUX, T30_MUX_BCS);  break;
    case MUX_MSG:  SET_STATE (MUX, T30_MUX_MSG);  break;
    }
    break;
  }
  default:
    break;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MUXS            |
| STATE   : code                       ROUTINE : sig_msg_mux_msg_req |
+--------------------------------------------------------------------+

  PURPOSE : Process signal MSG_REQ received from process msg-formatter.
            The signal requests to send msg-data to fad.
*/

GLOBAL void sig_msg_mux_msg_req (T_FAD_DATA_REQ *fad_data_req)
{
  TRACE_FUNCTION ("sig_msg_mux_msg_req()");
  PACCESS        (fad_data_req);

  switch (GET_STATE (MUX))
  {
    case T30_MUX_MSG:
    {
#ifdef KER_DEBUG_MSG
      ker_debug ("fad_data_req", fad_data_req->sdu.buf, (USHORT)(fad_data_req->sdu.l_buf >> 3));
#endif
      PSENDX (FAD, fad_data_req);
      t30_data->fad_data_req = NULL;
      break;
    }

    default:
    {
      PFREE (fad_data_req);
      break;
    }
  }
}

