/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_srm.c
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
|  Purpose :  This Modul defines the SREJ timer manager for
|             the component Radio Link Protocol of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#ifndef RLP_SRM_C
#define RLP_SRM_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
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
|  Function    : srm_init
+------------------------------------------------------------------------------
|  Description : initialize the srej timer manager 
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void           srm_init
                      (
                        void
                      )
{
  SHORT n;

  TRACE_FUNCTION ("srm_init()");

  n =0;
  while (n < SRM_DATA_SIZE)
  {
    rlp_data->srm.Data[n].count = 0;
    n++;
  }
}

/*
+------------------------------------------------------------------------------
|  Function    : srm_deinit
+------------------------------------------------------------------------------
|  Description : deintialize the srej timer manager at a certain point of time
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void           srm_deinit
                      (
                        void
                      )
{
  TRACE_FUNCTION ("srm_deinit()");
  srm_init();
}

/*
+------------------------------------------------------------------------------
|  Function    : srm_reset
+------------------------------------------------------------------------------
|  Description : reset the srej timer manager
|
|  Parameters  : -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void           srm_reset
                      (
                        void
                      )
{
  
  TRACE_FUNCTION ("srm_reset()");

  srm_init();
}

/*
+------------------------------------------------------------------------------
|  Function    : srm_clear
+------------------------------------------------------------------------------
|  Description : clear the srej count for one slot
|
|  Parameters  : n -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void           srm_clear
                      (
                        T_FRAME_NUM n
                      )
{
  
  TRACE_FUNCTION ("srm_clear()");

  rlp_data->srm.Data[n].count = 0;
}

/*
+------------------------------------------------------------------------------
|  Function    : srm_count
+------------------------------------------------------------------------------
|  Description : increment the srej count for one slot
|
|  Parameters  : n -
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void srm_count(T_FRAME_NUM n)
{
    TRACE_FUNCTION ("srm_count()");

  rlp_data->srm.Data[n].count++;
}

/*
+------------------------------------------------------------------------------
|  Function    : srm_get_counter
+------------------------------------------------------------------------------
|  Description : get the srej count for one slot
|
|  Parameters  : n - 
|                 
|
|  Return      : count -
+------------------------------------------------------------------------------
*/


GLOBAL T_COUNTER srm_get_counter(T_FRAME_NUM n)
{
  
  TRACE_FUNCTION ("srm_get_counter()");

  return rlp_data->srm.Data[n].count;
}
