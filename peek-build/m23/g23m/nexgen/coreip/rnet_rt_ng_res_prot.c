/*****************************************************************************
 * $Id: res_prot.c,v 1.7 2002/07/03 08:49:31 af Exp $
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
 * ngProto_RESOLV
 *----------------------------------------------------------------------------
 * 09/03/2001 - Adrien Felon
 * 11/12/2001 -
 *  - importing semaphore init code from res_star.c
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - removing ngresolv.h inclusion (now included from ngresolv/resolv.h)
 *****************************************************************************/

#include <ngos.h>
#include <ngresolv/resolv.h>

NGresolver ngResolv;

/*****************************************************************************
 * resolvInitSem()
 *****************************************************************************
 * Resolver Semapore Initialization
 *****************************************************************************
 */

#ifdef NG_RTOS

static int resolvInitSem( void) {
  int i, err;
  if ( (0 == ngResolv.s_pquermax) || (NULL == ngResolv.s_sem) )
    return NG_EINVAL;
  for (i=0; i<ngResolv.s_pquermax; i++) {
    NGOSsem * sem = (NGOSsem *)
      ((NGubyte*)ngResolv.s_sem + i * ngOSSemGetSize());
    err = ngOSSemInit( sem, 0);
    if (NG_EOK != err) {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                      "ngOSSemInit( 0x%08lx, 0) has error %d", err));
      ngResolv.s_sem = NULL;
      return err;
    }
  }
  return NG_EOK;
}

#endif

/*****************************************************************************
 * resolvInit()
 *****************************************************************************
 * Protocol Initialization
 *****************************************************************************
 */

static void resolvInit( void)
{
  ngMemSet( &ngResolv, '\0', sizeof( ngResolv));
  /* Setting builtin default values */
  ngResolv.s_timeout = NG_RSLV_TIMEOUT;
  ngResolv.s_rmitmax = NG_RSLV_RMITMAX;
  ngResolv.s_switchmax = NG_RSLV_SWITCHMAX;
  ngResolv.s_serv1_port = NG_RSLV_PORT;
  ngResolv.s_serv2_port = NG_RSLV_PORT;
}

/*****************************************************************************
 * resolvCntl()
 *****************************************************************************
 * Protocol Control
 *****************************************************************************
 */

int resolvCntl( int cmd, int opt, void * arg)
{
  int err;
  int curserv_addr_changed = 0;

  err = NG_EOK;

  if (NG_CNTL_SET == cmd) {

    switch (opt) {

    case NG_RSLVO_SERV1_IPADDR:
      if (1 == ngResolv.s_serv) curserv_addr_changed = 1;
      ngResolv.s_serv1_addr = *((NGuint*)arg);
      break;
    case NG_RSLVO_SERV1_PORT:
      if (1 == ngResolv.s_serv) curserv_addr_changed = 1;
      ngResolv.s_serv1_port = *((NGushort*)arg);
      break;
    case NG_RSLVO_SERV2_IPADDR:
      if (2 == ngResolv.s_serv) curserv_addr_changed = 1;
      ngResolv.s_serv2_addr = *((NGuint*)arg);
      break;
    case NG_RSLVO_SERV2_PORT:
      if (2 == ngResolv.s_serv) curserv_addr_changed = 1;
      ngResolv.s_serv2_port = *((NGushort*)arg);
      break;
    case NG_RSLVO_DOMAIN:
      if (NULL == *(char const **)arg) {
        ngResolv.s_domain[0] = '\0';
      } else {
        err = ngStrLen( *(char const **)arg);
        if (err > ((signed) sizeof( ngResolv.s_domain)-1)) {
          ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                          "Domain Name too long (%d bytes max...)",
                          (sizeof( ngResolv.s_domain)-1)));
          return NG_ENOBUFS;
        }
        ngMemCpy( ngResolv.s_domain, *(char const **)arg, err);
        ngResolv.s_domain[ err] = '\0';
        err = NG_EOK;
      }
      break;
    case NG_RSLVO_QUERY_MAX:
      ngResolv.s_pquermax = *((NGuint*)arg);
      break;
    case NG_RSLVO_QUERY:
      ngResolv.s_pquer = *((NGslvpquer **)arg);
      break;
#ifdef NG_RTOS
    case NG_RSLVO_SEM:
      ngResolv.s_sem = *((NGOSsem **)arg);
      err = resolvInitSem();
      break;
#endif
    case NG_RSLVO_CACHE_MAX:
      ngResolv.s_camax = *((NGuint*)arg);
      break;
    case NG_RSLVO_CACHE_ENT:
      ngResolv.s_ca = *((NGslvcaent **)arg);
      break;
    case NG_RSLVO_HOST_MAX:
      break;
    case NG_RSLVO_HOST_ENT:
      break;
    case NG_RSLVO_TO:
      ngResolv.s_timeout = *((NGuint*)arg);
      break;
    case NG_RSLVO_RMITMAX:
      ngResolv.s_rmitmax = *((NGuint*)arg);
      break;
    case NG_RSLVO_SWITCHMAX:
      ngResolv.s_switchmax = *((NGuint*)arg);
      break;
    default:
      err = NG_EINVAL;
      break;
    }

    /* We may need to change the connection to current server */
    if ( (curserv_addr_changed) && (NG_RSLV_ST_RUNNING == ngResolv.s_state) ) {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 2,
                      "Current server changed!"));
      /* Force connection to PRIMARY server*/
      ngResolvConnect( ngResolv.s_serv1_addr, ngResolv.s_serv1_port);
      ngResolv.s_serv = 1;
      /* Abort all pending queries! */
      ngResolvPendingQueryAbortAll();
    }

  } else { /* NG_CNTL_GET */

    switch (opt) {

    case NG_RSLVO_SERV1_IPADDR:
      *((NGuint*)arg) = ngResolv.s_serv1_addr;
      break;
    case NG_RSLVO_SERV1_PORT:
      *((NGushort*)arg) = ngResolv.s_serv1_port;
      break;
    case NG_RSLVO_SERV2_IPADDR:
      *((NGuint*)arg) = ngResolv.s_serv2_addr;
      break;
    case NG_RSLVO_SERV2_PORT:
      *((NGushort*)arg) = ngResolv.s_serv2_port;
      break;
    case NG_RSLVO_DOMAIN:
      *((char const **)arg) = ngResolv.s_domain;
      break;
    case NG_RSLVO_TO:
      *((NGuint*)arg) = ngResolv.s_timeout;
      break;
    case NG_RSLVO_RMITMAX:
      *((NGuint*)arg) = ngResolv.s_rmitmax;
      break;
    case NG_RSLVO_SWITCHMAX:
      *((NGuint*)arg) = ngResolv.s_switchmax ;
      break;
    default:
      err = NG_EINVAL;
      break;
    }

  }

  return err;
}

/*****************************************************************************
 * resolvTimer()
 *****************************************************************************
 * Solver client timers processing
 *****************************************************************************
 */

static void resolvTimer( void)
{
  NGslvpquer * pq;
  int i, nbpq;

  if (ngResolv.s_state != NG_RSLV_ST_RUNNING) return;

  /* Checking pending queries timeouts! */
  pq = ngResolv.s_pquerbusyq;
  nbpq = ngResolv.s_pquerbusyn;
  i = 0;

  /* ATTENTION: the automaton may change the busy/free lists */
  while (i < nbpq) {
    NGslvpquer * pqnext = pq->spq_next;
    /* Calling timer function for active pending query (filling a cache ent) */
    if (NG_RSLVPQ_ST_FRIEND != pq->spq_state)
      ngResolvPendingQueryAutomaton( pq, NG_RSLVPQ_EV_TIMER);
    pq = pqnext;
    i++;
  }
}

/*****************************************************************************
 * ngProto_RESOLV()
 *****************************************************************************
 * Protocol structure definition
 *****************************************************************************
 */

const NGproto ngProto_RESOLV = {
  "SOLVE",
  NULL,
  NULL,
  NG_PROTO_RESOLV,
  resolvInit,
  resolvCntl,
  resolvTimer,
  NULL,
  NULL,
  NULL,
  NULL
};

