/*****************************************************************************
 * $Id: res_auto.c,v 1.8 2002/07/03 08:49:31 af Exp $
 * $Name: rel_1_3_b_7 $
 *----------------------------------------------------------------------------
 * NexGenRESOLV v1.3
 * Domain Name System Interface
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
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
 * ngResolvPendingQueryAutomaton()
 *----------------------------------------------------------------------------
 * 16/03/2001 - Adrien Felon
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - including ngresolv/resolv.h instead of ngresolv.h
 *****************************************************************************/

#include <ngos.h>
#include <ngsocket.h>
#include <ngresolv/resolv.h>

/*****************************************************************************
 * ngResolvPendingQueryAutomaton()
 *****************************************************************************
 * Automaton for master pending queries. In RTOS mode this function
 * assumes that we are already in the global NexGenIP lock (NG_LOCK()
 * called)
 *****************************************************************************
 * Parameters:
 *   pq          Solver pending query receiving next event
 *   ev          Solver event
 * Return value:
 *   NG_EDNS_OK         if the query is no more busy
 *   NG_EDNS_WOULDBLOCK if the query is still pending
 * Caller:
 *   ngResolvQuery()
 *   ngResolvRecvAnswer()
 *   resolvTimer()
 *   resolvPendingQueriesAbort() */

int ngResolvPendingQueryAutomaton( NGslvpquer * pq, int ev)
{
  int err;
  NGslvquer * sq = &pq->spq_query;
  NGuint curtime = ngOSClockGetTime();
  NGslvcaent * ce;

  err = NG_EDNS_WOULDBLOCK;

  switch (ev) {

  case NG_RSLVPQ_EV_INIT:

    /*
     * Note: if pq->spq_ce is not NULL, application has asked to
     * update the underlying cache entry! So we do not need to
     * allocate a cache entry for this outgoing query...
     */

    /* Getting a free cache entry as required... */

    if (NULL == pq->spq_ce) {
      err = ngResolvCacheEntAlloc( &ce);
      pq->spq_ce = ce;
      if (NG_EOK != err) {
        /* This should never occurs! */
        ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                        "Serious error: could not get a free cache ent!" ));
        pq->spq_ce = NULL;
        break;
      }
    } else {
      /* Updating an existing cache entry (force query...)*/
      ce = pq->spq_ce;
    }

    /* Filling in cache entry with partial information */
    ce->sce_ttl = (NGuint)-1; /* Query pending -> ce never expires... */

    /* Setting query parameters */

    pq->spq_timeout = curtime +
      (ngResolv.s_timeout * ngOSClockGetFreq()) / 1000;
    pq->spq_rmit = 0;
    pq->spq_friend = NULL;
    pq->spq_qid = ngResolv.s_qid++;
    /* Setting name we will ask (eg. pq->spq_qname) for queries "ByName" */
    if (!(pq->spq_query.sq_flags & NG_RSLVF_QTYPE_GET_NAME)) {
      ngResolvBuildQueryByName( sq->sq_name, pq->spq_qname,
                              sizeof( pq->spq_qname));
    }

    /* Sending the query! */

    err = ngResolvSendQuery( pq);
    if (NG_EOK != err) {
      pq->spq_errcode = err;
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 4,
                      "Error %d while sending query", err ));
      break;
    }
    pq->spq_state = NG_RSLVPQ_ST_FIRST_SENT;
    /* Setting backpointer */
    pq->spq_ce->sce_pquer = pq;
    pq->spq_ce->sce_pquercount = 1;
    /* Query is surely pending... */
    err = NG_EDNS_WOULDBLOCK;
    break; /* case NG_RSLVPQ_EV_INIT */

  case NG_RSLVPQ_EV_TIMER:
    if (pq->spq_timeout < curtime) {
      /* Check global rmit count */
      err = ngResolvRmitIncr( pq->spq_rmit);
      if (NG_EOK != err)
        /* ATTENTION: we MUST return here in case of error because
         * resolvRmitIncr may have called the automaton again to
         * remove this pending query (pq no more in busy list...)
         */
        return NG_EDNS_TIMEDOUT;
      pq->spq_rmit++;
      pq->spq_timeout = curtime +
        (ngResolv.s_timeout * ngOSClockGetFreq()) / 1000;
      err = ngResolvSendQuery( pq);
      if (NG_EOK == err) {
        err = NG_EDNS_WOULDBLOCK;
      } else {
        pq->spq_errcode = err;
        ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 4,
                        "Error %d while sending query", err ));
      }
    }
#ifdef nomore
    else {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_TIMER, 2, "Query times out!" ));
      pq->spq_errcode = NG_EDNS_TIMEDOUT;
      err = NG_EDNS_TIMEDOUT;
    }
#endif
    break; /* case NG_RSLVPQ_EV_TIMER */

  case NG_RSLVPQ_EV_RECV:
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 2,
                    "Got answer from server!"));
    pq->spq_ce->sce_pquer = NULL;
    pq->spq_ce->sce_pquercount = 0;
    pq->spq_errcode = NG_EDNS_OK;
    err = NG_EDNS_OK;
    break; /* case NG_RSLVPQ_EV_RECV */

  case NG_RSLVPQ_EV_ERROR:
    switch (pq->spq_errcode) {
    case NG_EDNS_HOST_NOT_FOUND:
      break;
    default:
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 2, "Query error %d!",
                      pq->spq_errcode ));
      break;
    }
    err = pq->spq_errcode;
    break; /* case NG_RSLVPQ_EV_ERROR */

  } /* switch (ev) */

  /* Now we only need to do something if the query is no more pending... */

  if (NG_EDNS_WOULDBLOCK != err) {
    NGslvpquer * fq;

    /* The query is no more pending... */
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0, "Removing pending query"));
    /* Returning cache entry to free list if we got some error */
    if ( (NG_EDNS_OK != err) && (NULL != pq->spq_ce) ) {
      ngResolvCacheEntFree( pq->spq_ce);
    }
    /* TODO: loop to remove all friend queries */

    fq = pq; /* pq remains pointer to master query... */

    while (NULL != fq) {

      NGslvpquer * fqnext = fq->spq_friend;
      NGslvquer * sq = &fq->spq_query;

      if (NG_EDNS_OK == err) {
        /* Copying cache entry to user memory */
        ngResolvFillWithCacheEnt( sq->sq_hostent,
                                sq->sq_buf, sq->sq_buflen, pq->spq_ce);
      }

      if (NG_RSLVPQ_ST_FRIEND == fq->spq_state) {
        /* Copying error code of master query... */
        fq->spq_errcode = pq->spq_errcode;
      }

      /* Falling into DONE state */
      fq->spq_state = NG_RSLVPQ_ST_DONE;

      /* Remove pending query from busy list */
      if (1 == ngResolv.s_pquerbusyn) {
        ngResolv.s_pquerbusyq = NULL;
      } else {
        NGslvpquer * prev = fq->spq_prev;
        NGslvpquer * next = fq->spq_next;
        if (NULL != next) next->spq_prev = prev;
        if (NULL != prev) prev->spq_next = next;
        if (fq == ngResolv.s_pquerbusyq) {
          if (NULL != prev)
            ngResolv.s_pquerbusyq = prev;
          else
            ngResolv.s_pquerbusyq = next;
        }
      }
      ngResolv.s_pquerbusyn--;
      /* Insert it in free list */
      fq->spq_next = ngResolv.s_pquerfreeq;
      ngResolv.s_pquerfreeq = fq;

      /* Checking what to do to signal this to the application... */
      if (sq->sq_flags & NG_RSLVF_IOMODE_NONBLOCK) {
        /* Non blocking mode => calling callback... */
        /* Attention: do not call callback if the error will be returned
         * through ngResolvByXXX() API calls */
        if (NG_RSLVPQ_EV_INIT != ev)
          sq->sq_cb_f( err, sq->sq_cbdata);
      }
#ifdef NG_RTOS
      else {
        /* Blocking mode */
        NGOSsem * sem = (NGOSsem *)
          ( (NGubyte*)ngResolv.s_sem +
            (fq - ngResolv.s_pquer) * ngOSSemGetSize());
        ngOSSemPost( sem);
      }
#endif
      fq = fqnext;
    } /* while (NULL != fq) */
  }

  return err;
}

