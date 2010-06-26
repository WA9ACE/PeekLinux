/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_tim.c
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
|             for the component L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_TIM_C
#define L2R_TIM_C
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

/*==== EXPORT =====================================================*/
/*==== PRIVAT =====================================================*/
/*==== VARIABLES ==================================================*/
/*==== FUNCTIONS ==================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : tim_tup_snd
+------------------------------------------------------------------------------
|  Description : Timeout of timer TUP_SND
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void tim_tup_snd (void)
{
  T_UP *dup = &l2r_data->up;

  TRACE_FUNCTION ("tim_tup_snd()");

  if (GET_STATE (UP) EQ UP_CONNECTED AND GET_STATE (UP_LL) EQ ISW_WAIT)
  {
    if (up_some_data_to_send())
    {
      if (dup->LLFlow EQ FL_INACTIVE)
        up_send_prim_timeout ();
      else
        up_send_empty_frame(dup->LastSentSa, dup->LastSentSb, dup->MrgFlow);

      if (!dup->RiBu.idx.filled)
        SET_STATE (UP_LL, ISW_IDLE)
      else
        SET_STATE (UP_LL, ISW_SEND)
    }
    else
      TIMERSTART(TIMER_TUP_SND, LL_TO);
  }
}
