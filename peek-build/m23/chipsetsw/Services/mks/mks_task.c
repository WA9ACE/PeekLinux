/**
 * @file   mks_task.c
 *
 * Coding of the main MKS function : mks_core
 * This function loop in the process message function for waiting messages.
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

#include "mks/mks_env.h"
#include "mks/mks_i.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"


/* External declaration until Riviera 1.6 is available*/
extern T_RV_RET mks_handle_msg(T_RV_HDR*  msg_p);
extern T_RV_RET mks_handle_timer(UINT8 timer_num);



/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: mks_core
 */
T_RV_RET mks_core(void)
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p;
   UINT16 received_event;

   MKS_SEND_TRACE("MKS: Initialization", RV_TRACE_LEVEL_DEBUG_HIGH );

   /* Initialize SWE */
   mks_initialize_swe();

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         /* Read the message */
         msg_p = (T_RV_HDR*) rvf_read_mbox( MKS_MAILBOX_USED);

         mks_handle_msg(msg_p);
      }

      if (received_event & RVF_TIMER_0_EVT_MASK)
      {
         mks_handle_timer(RVF_TIMER_0);
      }

      if (received_event & RVF_TIMER_1_EVT_MASK)
      {
         mks_handle_timer(RVF_TIMER_1);
      }
      if (received_event & RVF_TIMER_2_EVT_MASK)
      {
         mks_handle_timer(RVF_TIMER_2);
      }
      if (received_event & RVF_TIMER_3_EVT_MASK)
      {
         mks_handle_timer(RVF_TIMER_3);
      }

   }

   return RV_OK;
}

