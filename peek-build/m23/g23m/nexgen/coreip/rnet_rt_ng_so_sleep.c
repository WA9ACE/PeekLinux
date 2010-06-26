/*****************************************************************************
 * $Id: so_sleep.c,v 1.4 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Level Internal Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngSoSleepOn() (RTOS)
 * ngSoWakeUp()
 *----------------------------------------------------------------------------
 * 19/01/99 - Regis Feneon
 * 14/02/2001 -
 *  added flags to ngSoSleepOn()
 *  added ngSoWakeUp()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoSleepOn()
 *****************************************************************************
 * Wait for events on a socket (RTOS)
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  flags   events to wait for
 *  timeo   timeout in clock ticks or zero for no timeout
 * Return value: 0 or error code if timeout
 */

#ifdef NG_RTOS

int ngSoSleepOn( NGsock *so, int flags, u_long timeo)
{
  int err;

  /* set flags */
  so->so_waitflags |= flags;

  /* block on socket' semaphore */
  so->so_nsleep++;
  NG_UNLOCK();
  if( timeo) {
    err = ngOSSemTimedWait( so->so_sem, timeo);
  }
  else {
    ngOSSemWait( so->so_sem);
    err = 0;
  }
  NG_LOCK();
  so->so_nsleep--;
  return( err);
}

#endif

/*****************************************************************************
 * ngSoWakeUp()
 *****************************************************************************
 * Wakeup tasks waiting on a socket (RTOS)
 * Call upcall routine
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  flags   events to be signaled
 * Return value: none
 */

void ngSoWakeUp( NGsock *so, int flags)
{
#ifdef NG_RTOS

  int aflags, count;

  aflags = so->so_waitflags & flags;

  /* tasks waiting on recv/send ? */
  if( aflags & (NG_SIOWAIT_READ|NG_SIOWAIT_WRITE)) {
    /* clear flags */
    so->so_waitflags &= ~(NG_SIOWAIT_READ|NG_SIOWAIT_WRITE);
    /* wakeup tasks */
    count = so->so_nsleep;
    while( count--) ngOSSemPost( so->so_sem);
  }

  /* tasks waiting on select ? */
  if( aflags & (NG_SIOSEL_READ|NG_SIOSEL_WRITE|NG_SIOSEL_EXCEPT)) {
    /* clear flags */
    so->so_waitflags &= ~(NG_SIOSEL_READ|NG_SIOSEL_WRITE|NG_SIOSEL_EXCEPT);
    /* wakeup tasks */
    count = ngSock_selnsleep;
    while( count--) ngOSSemPost( &ngSock_selsem);
  }

#endif

  /* upcall routine ? (forbid re-entrency) */
  if( (so->so_upcall_f != NULL) && !(so->so_state & NG_SS_UPCALL)) {
    /* set error flag */
    if( so->so_error) flags |= NG_SIOSEL_ERROR;
    /* call routine */
    so->so_state |= NG_SS_UPCALL;
    so->so_upcall_f( so, so->so_upcall_data, flags);
    so->so_state &= ~NG_SS_UPCALL;
  }

}

