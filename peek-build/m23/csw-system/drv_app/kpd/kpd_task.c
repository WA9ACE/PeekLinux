/**
 * @file   kpd_task.c
 *
 * Coding of the main keypad function : kpd_core
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
 *  10/10/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "kpd/kpd_env.h"
#include "kpd/kpd_i.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"


/* External declaration until Riviera 1.6 is available*/
extern T_RV_RET kpd_handle_msg(T_RV_HDR*  msg_p);
extern T_RV_RET kpd_handle_timer(UINT8 timer_num);



/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: kpd_core
 */
T_RV_RET kpd_core(void)
{   
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p;
   UINT16 received_event;

   KPD_SEND_TRACE("KPD: Initialization", RV_TRACE_LEVEL_DEBUG_HIGH);

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
       received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         /* Read the message */
         msg_p = (T_RV_HDR*) rvf_read_mbox( KPD_MAILBOX_USED);

         kpd_handle_msg(msg_p);
      }

      if (received_event & RVF_TIMER_0_EVT_MASK)
      {
         kpd_handle_timer(RVF_TIMER_0);
      }

      if (received_event & RVF_TIMER_1_EVT_MASK)
      {
         kpd_handle_timer(RVF_TIMER_1);
      }
      if (received_event & RVF_TIMER_2_EVT_MASK)
      {
         kpd_handle_timer(RVF_TIMER_2);
      }
      if (received_event & RVF_TIMER_3_EVT_MASK)
      {
         kpd_handle_timer(RVF_TIMER_3);
      }

   }

   return RV_OK;
}
