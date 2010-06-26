/**
 * @file   mks_handle_timer.c
 *
 * Coding of the mks_handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

 /*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/19/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "mks/mks_i.h"

#include "rv/rv_general.h"
#include "rvm/rvm_use_id_list.h"

/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: mks_handle_timer
 */
T_RV_RET mks_handle_timer(UINT8 timer_num)
{
   if (timer_num == RVF_TIMER_0)
   {
      mks_reset_sequence();
   }
   else
   {
      MKS_SEND_TRACE("MKS: Unexepected Timer expired", RV_TRACE_LEVEL_ERROR);
   }

   return RV_OK;
}

/*@}*/
