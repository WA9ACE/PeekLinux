/**
 * @file  mc_inth.h
 *
 * Definitions for MC interrupt handler.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  8/12/2003  ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _MC_INTH_H_
#define _MC_INTH_H_

#include "rv/rv_general.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** INTERNAL MC events (maintained locally, NOT as Riviera general purpose event)*/
#define MC_EVENT_CERR    (0x8000)  /* Card status error interrupt enable */
#define MC_EVENT_DTO     (0x4000)  /* Data response time-out interrupt enable */
#define MC_EVENT_CTO     (0x2000)  /* Command response time-out interrupt enable */
#define MC_EVENT_BRS     (0x1000)  /* Block received / sent interrupt enable */
#define MC_EVENT_AE      (0x0800)  /* Buffer almost empty interrupt enable */
#define MC_EVENT_AF      (0x0400)  /* Buffer almost full interrupt enable */
#define MC_EVENT_EOFB    (0x0200)  /* Card exit busy state interrupt enable */
#define MC_EVENT_EOC     (0x0100)  /* End of command interrupt enable */

#define MC_ALL_EVENTS    (0xFF00)


/*defines for general purpose events*/
#define MC_RVF_HOST_EVENT 0x8000 /* RVF general purpose events used for host controller events*/
#define MC_RVF_MAILBOX_2    0x4000 /* RVF general purpose events for second mailbox*/


UINT16 mc_host_wait_event_to (UINT16 expected_event, 
                          UINT16 saved_event,
                          UINT32 timeout);
void mc_int_handler(void);

void mc_gpio_int_handler(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_MC_INTH_H_
