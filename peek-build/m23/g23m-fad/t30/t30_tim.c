/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  T30_TIM
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
|  Purpose :  This Modul defines the timer handling functions
|             for the component T30 of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef T30_TIM_C
#define T30_TIM_C
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

/*==== EXPORT =====================================================*/

/*==== PRIVAT =====================================================*/

/*==== VARIABLES ==================================================*/

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : T30_TIM                  |
| STATE   : code                  ROUTINE : tim_t1                   |
+--------------------------------------------------------------------+

  PURPOSE : Timeout of timer T1

*/
LOCAL void tim_t1 (void)
{
  TRACE_FUNCTION ("tim_t1()");

  switch (GET_STATE (KER))
  {
    case T30_IDLE:
      snd_error_ind(ERR_PH_A_T1_TIMEOUT);
      break;

#ifdef _SIMULATION_ /* test of timer T1 */
    case T30_RCV_DIS:
#endif
    case T30_RCV_DCS:
      snd_error_ind(ERR_PH_A_T1_TIMEOUT);
      TIMERSTOP (T4_INDEX);
      SET_STATE (KER, T30_IDLE);
      break;

    default:
      break;
  }
  t30_data->data_cnf = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : T30_TIM                  |
| STATE   : code                  ROUTINE : tim_t2                   |
+--------------------------------------------------------------------+

  PURPOSE : Timeout of timer T2

*/

LOCAL void tim_t2 (void)
{
  TRACE_FUNCTION ("tim_t2()");

  switch (GET_STATE (KER))
  {
    case T30_RCV_TCF:
      snd_error_ind(ERR_PH_B_RCV_NO_RESP);
      SET_STATE (KER, T30_IDLE);
      break;

    case T30_RCV_DCN:
    case T30_RCV_MSG:
    case T30_RCV_PST:
      snd_error_ind(ERR_PH_D_RCV_NO_RESP);
      SET_STATE (KER, T30_IDLE);
      break;

    case T30_RCV_T2:
      snd_complete_ind(CMPL_EOM);
      break;

    default:
      break;
  }
  t30_data->rate_modified = FALSE;
  t30_data->data_cnf = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : T30_TIM                  |
| STATE   : code                  ROUTINE : tim_t4                   |
+--------------------------------------------------------------------+

  PURPOSE : Timeout of timer T4

*/

LOCAL void tim_t4 (void)
{
  TRACE_FUNCTION ("tim_t4()");

  if (!t30_data->data_cnf)
    TRACE_EVENT ("*** no fad_data_cnf received");

  switch (GET_STATE (KER))
  {
    case T30_RCV_DCS:
    {
      if (t30_data->rate_modified)
      {
        TIMERSTART (T4_INDEX, T4_VALUE);
        break;
      }
      SET_STATE (KER, T30_RCV_DIS);

      if (t30_data->data_cnf)
        ker_send_dis ();
      else
        TIMERSTART (T4_INDEX, T4_VALUE);

      break;
    }

    case T30_SND_CFR:
    {
      if (t30_data->repeat EQ 3)
      {
        snd_error_ind(ERR_PH_B_SND_DCS_RPTD_3_TIMES);
        SET_STATE (KER, T30_DCN);
        if (t30_data->data_cnf)
        {
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req (FINAL_YES);
        }
      }
      else
      {
        t30_data->repeat++;
        if (t30_data->data_cnf)
        {
          SET_STATE (KER, T30_SND_CAP);
          snd_t30_sgn_ind(SGN_NO_RES);
        }
        else
        {
          TIMERSTART (T4_INDEX, T4_VALUE);
        }
      }
      break;
    }

    case T30_SND_DCN:
    {
      snd_complete_ind(CMPL_EOP);
      break;
    }

    case T30_SND_MCF:
    {
      if (t30_data->repeat EQ 3)
      {
        switch (t30_data->sgn_req)
        {
          case SGN_EOM:
          case SGN_PRI_EOM:
            snd_error_ind(ERR_PH_D_SND_EOM_RPTD_3_TIMES);
            break;

          case SGN_EOP:
          case SGN_PRI_EOP:
            snd_error_ind(ERR_PH_D_SND_EOP_RPTD_3_TIMES);
            break;

          case SGN_MPS:
          case SGN_PRI_MPS:
            snd_error_ind(ERR_PH_D_SND_MPS_RPTD_3_TIMES);
            break;

          default:
            snd_error_ind(ERR_PH_D_SND_UNSPEC);
            break;
        }
        SET_STATE (KER, T30_DCN);
        if (t30_data->data_cnf)
        {
          _decodedMsg[0] = BCS_DCN;
          sig_ker_bcs_bdat_req (FINAL_YES);
        }
      }
      else /* t30_data->repeat < 3 */
      {
        t30_data->repeat++;
        act_on_sgn_req();
        if (!t30_data->data_cnf)
        {
          TIMERSTART (T4_INDEX, T4_VALUE);
        }
      }
      break;
    }

    default:
      break;

  } /* end switch */

  t30_data->data_cnf = FALSE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : T30_TIM                  |
| STATE   : code                  ROUTINE : t30_timeout              |
+--------------------------------------------------------------------+

  PURPOSE : execute timeout function

*/

GLOBAL void t30_timeout (USHORT index)
{
  TRACE_FUNCTION ("t30_timeout()");
  switch (index)
  {
  case T1_INDEX:
    tim_t1 ();
    break;
  case T2_INDEX:
    tim_t2 ();
    break;
  case T4_INDEX:
    tim_t4 ();
    break;
  }
}

