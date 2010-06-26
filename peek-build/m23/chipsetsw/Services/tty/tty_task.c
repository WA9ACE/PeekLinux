/**
 * @file   tty_task.c
 *
 * Coding of the main TTY function : tty_core
 * This function loop in the process message function for waiting messages.
 *
 * @author	Frederic Turgis (f-turgis@ti.com) & Gerard Cauvy (g-cauvy@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  01/27/2003	Create
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _WINDOWS
  #include "config/l1sw.cfg"
#endif

#include "tty/tty_env.h"
#include "tty/tty_i.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
  #ifndef _WINDOWS
    #include "config/sys.cfg"
    #include "config/chipset.cfg"
  #endif
  #include "l1_types.h"
  #include "l1_confg.h"
  #include "l1_trace.h"
#endif

#if (L1_GTT == 1)
  /* External prototype. */
  extern void l1gtt_initialize(void);
  extern void l1gtt_exit(void);
  extern void l1gtt_main_processing(void);
  extern void l1gtt_init_trace(void);
#else
  /* Dummy functions */
  void l1gtt_initialize(void) {}
  void l1gtt_exit(void) {}
  void l1gtt_main_processing(void) {}
  void l1gtt_init_trace(void) {}
#endif


/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: tty_core
 */
T_RV_RET tty_core(void)
{
  BOOLEAN error_occured = FALSE;
  T_RV_HDR* msg_p;
  UINT16 received_event;

  TTY_SEND_TRACE("TTY TASK Started", RV_TRACE_LEVEL_DEBUG_HIGH );

  l1gtt_init_trace();

  /* loop to process messages */
  while (error_occured == FALSE)
  {
    /* Wait for the necessary events. */
    received_event = rvf_wait ( 0xffff,0);

    if (received_event & TTY_INIT_EVENT_MASK)
    {
      l1gtt_initialize();
    }
    else if (received_event & TTY_EXIT_EVENT_MASK)
    {
      l1gtt_exit();
    }
    else if (received_event & TTY_DATA_EVENT_MASK)
    {
      l1gtt_main_processing();
    }
  }

  return RV_OK;
}
