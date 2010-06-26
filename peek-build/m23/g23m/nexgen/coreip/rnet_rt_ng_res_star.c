/*****************************************************************************
 * $Id: res_star.c,v 1.8 2002/07/03 08:49:31 af Exp $
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
 * ngResolvStart()
 *----------------------------------------------------------------------------
 * 14/03/2001 - Adrien Felon
 * 24/07/2001 -
 *  - removing superflous "i++" statement in loop to init query struct
 * 11/12/2001 -
 *  - moving semaphore init code (i.e. calls to ngOSSemInit()) to res_prot.c
 *  - trying secundary DNS if failed to connect to primary
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - removing ngresolv.h inclusion (now included from ngresolv/resolv.h)
 *****************************************************************************/

#include <ngos.h>
#include <ngsockio.h>
#include <ngresolv/resolv.h>

/*****************************************************************************
 * ngResolvStart()
 *****************************************************************************
 * Try to start the resolver with the init parameters set by the application
 *****************************************************************************
 * Parameters:
 *   none
 * Return value:
 *   NG_EOK         if the resolver has sucessfully started
 *   NG_EALREADY    if the resolver is already running
 *   a negative error code
 * Caller:
 *   ngResolvQuery()
 */

int ngResolvStart( void)
{
  unsigned i;
  int err;
  NGsock * so;
  NGslvpquer * pq;
  NGslvcaent * ce;

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 2, "START"));

  so = NULL;

  NG_LOCK();
  if (ngResolv.s_state == NG_RSLV_ST_RUNNING) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Solver already running" ));
    NG_UNLOCK();
    return NG_EALREADY;
  }
  /* Checking configuration integrity */
  if (
      (ngResolv.s_camax == 0) ||
      (NULL == ngResolv.s_ca) ||
      (ngResolv.s_pquermax == 0) ||
      (NULL == ngResolv.s_pquer) ||
#ifdef NG_RTOS
      (NULL == ngResolv.s_sem) ||
#endif
      (INADDR_ANY == ngResolv.s_serv1_addr)
  ) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "Invalid resolv config"));
    err = NG_EINVAL;
    goto error;
  }

  if (ngResolv.s_camax < ngResolv.s_pquermax) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "Needing more cache entries..."));
    err = NG_EINVAL;
    goto error;
  }

  /* Performing intitialization */

  ngResolv.s_so = NULL;
  ngResolv.s_rmit = 0;
  ngResolv.s_switch = 0;
  ngResolv.s_serv = 1;

  /* Opening socket to primary DNS server */
  err = ngSAIOCreate( &so, NG_AF_INET, NG_SOCK_DGRAM, NG_PROTO_UDP, 0);
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "ngSAIOCreate() has error %d", err));
    goto error;
  }
  ngResolv.s_so = so;

  /* Dealing with DNS cache initialization */

  /* Init free cache entry list */
  ngMemSet( ngResolv.s_ca, '\0', ngResolv.s_camax * sizeof( NGslvcaent));
  ngResolv.s_cafreeq = ce = ngResolv.s_ca;
  for (i=0; i<ngResolv.s_camax; i++) {
    ce[i].sce_next = &ce[i+1];
  }
  ce[i-1].sce_next = NULL;

  /* Init busy cache entry list */
  ngResolv.s_cabusyq = NULL;
  ngResolv.s_cabusyn = 0;

  /* Init free pending query queue */
  ngMemSet( ngResolv.s_pquer, '\0', ngResolv.s_pquermax * sizeof( NGslvpquer));
  pq = (NGslvpquer *)ngResolv.s_pquer;
  for (i=0; i<ngResolv.s_pquermax; i++) {
    pq[i].spq_next = &pq[i+1];
  }
  pq[i-1].spq_next = NULL;
  ngResolv.s_pquerfreeq = ngResolv.s_pquer;

  /* Init busy pending query queue */
  ngResolv.s_pquerbusyq = NULL;
  ngResolv.s_pquerbusyn = 0;

  err = ngSAIOSetCallback( so, ngResolvSocketCb_f, &ngResolv);
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "ngSAIOSetCallback() has error %d", err));
    goto error;
  }

  err = ngResolvConnect( ngResolv.s_serv1_addr, ngResolv.s_serv1_port);
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "ngResolvConnect() has error %d", err));
    if (0L == ngResolv.s_serv2_addr) goto error;
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4, "trying secundary DNS"));
    err = ngResolvConnect( ngResolv.s_serv2_addr, ngResolv.s_serv2_port);
    if (NG_EOK != err) {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                      "ngResolvConnect() has error %d", err));
      goto error;
    }
    ngResolv.s_serv = 2; /* Current server is secundary */
  }

  /* Generate first query id */
  ngResolv.s_qid = (NGushort) ngOSClockGetTime();
  ngResolv.s_qid = ngHTONS( ngResolv.s_qid);

  ngResolv.s_state = NG_RSLV_ST_RUNNING;
  NG_UNLOCK();
  return NG_EOK;

 error:
  if (NULL != so)
    ngSAIOClose( so, 0);
  NG_UNLOCK();
  return err;
}

