/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  t30_muxp
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
|             of incomming primitives for the component T30
|             of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_MUXP_C
#define T30_MUXP_C
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
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MUXP            |
| STATE   : code                       ROUTINE : mux_fad_mux_ind     |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_MUX_IND received from FAD.
            This primitive switches the state of the multiplexer.
*/

GLOBAL void mux_fad_mux_ind (T_FAD_MUX_IND *fad_mux_ind)
{
  TRACE_FUNCTION ("mux_fad_mux_ind");
  PACCESS        (fad_mux_ind);

  switch (GET_STATE (MUX))
  {
  case T30_MUX_BCS:
  case T30_MUX_MSG:
    switch (fad_mux_ind->mode)
    {
    case MUX_BCS:
      {
      T_TIME v;
      SET_STATE (MUX, T30_MUX_BCS);

      /* restart timer t4 */
      /* MG: I think the timer should only be restarted, when it is running already: */

      TIMERSTATUS(T4_INDEX, &v);
      if (v)
      {
        TIMERSTART (T4_INDEX, T4_VALUE);
      }
      if (t30_data->preamble_ind)
      {
        PALLOC (t30_preamble_ind, T30_PREAMBLE_IND);
        t30_data->preamble_ind = FALSE;
        PSENDX (MMI, t30_preamble_ind);
      }
      }
      break;

    case MUX_MSG:
      SET_STATE (MUX, T30_MUX_MSG);
      break;
    }
    break;

  default:
#ifdef _SIMULATION_ /* test BCS formatter only */
    if (t30_data->test_mode & TST_BCS)
    {
      switch (fad_mux_ind->mode)
      {
        case MUX_BCS: SET_STATE (MUX, T30_MUX_BCS); break;
      }
    }
#endif
    break;
  }
  PFREE (fad_mux_ind);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_MUXP            |
| STATE   : code                       ROUTINE : mux_fad_data_ind    |
+--------------------------------------------------------------------+

  PURPOSE : Process primitive FAD_DATA_IND received from FAD.
            Sends the received data to the message- or the bcs-
            formatter or to nirvana in dependence of the mux-state.
*/

GLOBAL void mux_fad_data_ind (T_FAD_DATA_IND *fad_data_ind)
{
  TRACE_FUNCTION ("mux_fad_data_ind()");
  PACCESS (fad_data_ind);

  switch (GET_STATE (MUX))
  {
  case T30_MUX_BCS:
#if defined _SIMULATION_ || defined KER_DEBUG_BCS
    ker_debug ("FAD_DATA_IND", fad_data_ind->sdu.buf, (USHORT)(fad_data_ind->sdu.l_buf >> 3));
#endif
    sig_mux_bcs_bcs_ind (fad_data_ind);
    break;
  
  case T30_MUX_MSG:
    sig_mux_msg_msg_ind (fad_data_ind);
    break;

  default:
    break;
  }

  PFREE (fad_data_ind);
}

