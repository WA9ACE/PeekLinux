/*****************************************************************************
 * $Id: res_quer.c,v 1.9 2002/07/03 08:49:31 af Exp $
 * $Name: rel_1_3_b_7 $
 *----------------------------------------------------------------------------
 * NexGenRESOLV v1.3
 * DNS resolver protocol
 *----------------------------------------------------------------------------
 *              Copyright (c) 1998-2001 NexGen Software.
 *
 *    All rights reserved. NexGen Software' source code is an
 *  unpublished work and the use of a copyright notice does not imply
 *  otherwise.  This source code contains confidential, trade secret
 *  material of NexGen Software. Any attempt or participation in
 *  deciphering, decoding, reverse engineering or in any way altering
 *  the source code is strictly prohibited, unless the prior written
 *  consent of NexGen Software is obtained.
 *
 *    This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with NexGen Software, and may not be
 *  copied or disclosed except in accordance with the terms of that
 *  agreement.
 *
 *----------------------------------------------------------------------------
 * ngResolvQuery()
 *----------------------------------------------------------------------------
 * 16/03/2001 - Adrien Felon
 * 02/07/2001 -
 *  - adding cast (NGuint) for sq_addr
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 04/04/2002 -
 *  - replacing call ngSleep( 50) by ngYield()
 * 03/07/2002 -
 *  - removing ngresolv.h inclusion (now included from ngresolv/resolv.h)
 *****************************************************************************/

#include <ngos.h>
#include <ngresolv/resolv.h>
#include "rnet_rt_i.h"
#include "rvf_api.h"

/*****************************************************************************
 * ngResolvQuery()
 *****************************************************************************
 * Resolver internal query (for API functions)
 *****************************************************************************
 * Parameters:
 *   sq          Query
 * Return value:
 *   NG_EOK         if the answer is available
 *   NG_EWOULDBLOCK if the query is pending (non blocking mode only)
 *   a negative error code
 * Caller:
 *   ngResolvByAddr()
 *   ngResolvByName()
 *   ngResolvByAddrAsync()
 *   ngResolvByNameAsync()
 */

int ngResolvQuery( NGslvquer const * sq)
{
  NGslvpquer * pq;
  NGslvcaent * ce;
  int err;
  int flags;
  int master;

  /* Checking argument integrity */
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX0", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));

  if (NULL == sq) return NG_EINVAL;
  if (
      (NULL == sq->sq_hostent) ||
      (NULL == sq->sq_buf) ||
      ((unsigned)(sq->sq_buflen) <= NG_RSLV_APP_MIN_BUF)
  ) {
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX1", RV_TRACE_LEVEL_DEBUG_LOW);

    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Invalid query" ));
    return NG_EINVAL;
  }

//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX2", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
  /* Continue to check args + set additional flags */

  flags = sq->sq_flags; /* shortcut... */

  if (flags & NG_RSLVF_IOMODE_NONBLOCK) {
    /* Non blocking mode... */
    if (NULL == sq->sq_cb_f) {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                      "Missing callback function" ));
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX3", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
      return NG_EINVAL;
    }
  } else {
    /* Blocking mode... */
  }

  if (flags & NG_RSLVF_QTYPE_GET_NAME) {
    /* Getting host by its addr... */
    if (INADDR_ANY == (NGuint)sq->sq_addr) {
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX4", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Missing IP Address" ));
      return NG_EINVAL;
    }
  } else {
    /* Getting host by its name... */
    if ( (NULL == sq->sq_name) ) {
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX5", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Missing host name" ));
      return NG_EINVAL;
    }
  }

  /* Checking that the service is initialized */
  switch (ngResolv.s_state) {
  case NG_RSLV_ST_RUNNING:
    break;
  case NG_RSLV_ST_STOPPED:
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Service not running" ));
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX6", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
    return NG_EINVAL;
  case NG_RSLV_ST_UNINIT:
    /* Trying a start... */
    NG_UNLOCK();
    err = ngResolvStart();
    NG_LOCK();
    if (NG_EOK != err) {
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX7", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                      "Can not start DNS resolver" ));
      return err;
    }
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Solver started..." ));
    break;
  }

//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX8", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
  /* Checking for a matching entry in the cache... */
  ce = NULL;
  err = ngResolvCacheLookup( sq, &ce);
  if ( (NG_EOK == err) && (NULL != ce) ) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "Cache has a matching entry!" ));
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX9", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
    if (NULL == ce->sce_pquer) {
      if (!(sq->sq_flags & NG_RSLVF_FORCE_QUERY)) {
        /* Filling user buffer with cached entry */
        ngResolvFillWithCacheEnt( sq->sq_hostent, sq->sq_buf,
                                  sq->sq_buflen, ce);
        return NG_EOK;
      }
#ifdef NG_DEBUG
      else {
        ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                        "Application forcing query!" ));
      }
#endif
    }
#ifdef NG_DEBUG
    else {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Not a master query!" ));
    }
#endif
  }

  /* Getting a free pending query entry... */
  if ((NGuint) ngResolv.s_pquerbusyn >= ngResolv.s_pquermax) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "Too many simultaneous queries" ));
    return NG_ENOMEM;
  }
  pq = ngResolv.s_pquerfreeq;
  ngResolv.s_pquerfreeq = (NGslvpquer *)pq->spq_next;
  ngResolv.s_pquerbusyn++;
  /* Insert it into busy list */
  pq->spq_prev = NULL;
  pq->spq_next = ngResolv.s_pquerbusyq;
  if (NULL != ngResolv.s_pquerbusyq) {
    ngResolv.s_pquerbusyq->spq_prev = pq;
  }
  ngResolv.s_pquerbusyq = pq;

  /* Init query ... */

  /* Copy query application parameters into pending query */
  ngMemCpy( &pq->spq_query, sq, sizeof( pq->spq_query));
  pq->spq_ce = ce;

  master = 0;
  if (NULL == ce) {
    master = 1;
  } else if (NULL == ce->sce_pquer) {
    master = 1;
  }
  /* Note: we have invariant "ce not NULL if master set to 1"... */

  if (master) {
    /* This is a master query: send init event to automaton */
    err = ngResolvPendingQueryAutomaton( pq, NG_RSLVPQ_EV_INIT);
    if (NG_EDNS_WOULDBLOCK != err)
      return err;
  } else {
    pq->spq_state = NG_RSLVPQ_ST_FRIEND;
    /* This is a friend query: register it into master query friend list */
    /*lint -e613 (Warning -- Possible use of null pointer) */
    pq->spq_friend = ce->sce_pquer->spq_friend;
    ce->sce_pquer->spq_friend = pq;
    /*lint +e613 (Warning -- Possible use of null pointer) */
    /* Set err for async mode... */
    err = NG_EDNS_WOULDBLOCK;
  }

  if (!(sq->sq_flags & NG_RSLVF_IOMODE_NONBLOCK)) {
    /* Blocking mode: what to do depends on RTOS vs. polling */
#ifdef NG_RTOS
    int idx = (pq - ngResolv.s_pquer);
    /* Blocking mode: what to do depends on RTOS vs. polling */
    NGOSsem * sem = (NGOSsem *)
      ((NGubyte*)ngResolv.s_sem + idx * ngOSSemGetSize());
    /* Note the timeout is managed by the resolver itself... It will
     * always post the sem (especially if the query times out) */
    NG_UNLOCK();
    ngOSSemWait( sem);
    NG_LOCK();
#else
    /* Blocking mode, so let timer function do the real polling for
     * us. We just wait for the query to reach its "done" state...
     */
    do {
      ngYield();
    } while (pq->spq_state != NG_RSLVPQ_ST_DONE);

#endif
    /* Note: errcode has been set by the automaton... */
    err = pq->spq_errcode;
  }
//  RNET_RT_SEND_TRACE("RNET_RT: XXXXX10", RV_TRACE_LEVEL_DEBUG_LOW);
//rvf_delay(RVF_MS_TO_TICKS(100));
  return err;
}

