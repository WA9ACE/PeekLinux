/*****************************************************************************
 * $Id: so_alloc.c,v 1.4 2002/04/23 10:20:04 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Level Internal Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
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
 * ngSoAlloc()
 * ngSoFree()
 *----------------------------------------------------------------------------
 * 12/12/98 - Regis Feneon
 * 31/12/98 -
 *  ngSoFree() added
 * 09/08/99 -
 *  so_error now cleared in ngSoAlloc()
 * 30/09/99 -
 *  so_nsleep cleared in ngSoAlloc()
 * 24/02/2000 -
 *  added so_mcast fields initialization
 * 03/11/2000 -
 *  added igmp/multicast support
 * 14/02/2001 -
 *  initialization of so_upcall fields and so_waitflags
 * 09/04/2002 -
 *  inherit so_rcv_timeo and so_snd_timeo
 *  initialise so_linger
 *  set default mcast ttl to 1
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoAlloc()
 *****************************************************************************
 * Allocate a new socket descriptor structure
 *****************************************************************************
 * Parameters:
 *  type        socket type
 * Returns: pointer to socket structure or NULL
 */

NGsock *ngSoAlloc( NGsock *head)
{
  NGsock *so;
#ifdef NG_IGMP_SUPPORTED
  int i;
#endif

  /* get socket structure */
  NG_QUEUE_OUT( &ngSock_freeq, so);

  if( so == NULL) return( NULL);

  /* initialize structure */
  if( head) {
    /* inherit some values from head socket */
    so->so_options = head->so_options;
    so->so_state = head->so_state & ~NG_SS_UPCALL;
    so->so_timeo = head->so_timeo;
    so->so_proto = head->so_proto;
    so->so_rcv_hiwat = head->so_rcv_hiwat;
    so->so_rcv_lowat = head->so_rcv_lowat;
    so->so_snd_hiwat = head->so_snd_hiwat;
    so->so_snd_lowat = head->so_snd_lowat;
    so->so_ttl = head->so_ttl;
    so->so_tos = head->so_tos;
    so->so_mcast_ttl = head->so_mcast_ttl;
    so->so_mcast_ifnetp = head->so_mcast_ifnetp;
    so->so_head = head;
    so->so_rcv_timeo = head->so_rcv_timeo;
    so->so_snd_timeo = head->so_snd_timeo;
    so->so_linger = head->so_linger;
  }
  else {
    so->so_options = 0;
    so->so_state = 0;
    so->so_timeo = 0;
    so->so_proto = NULL;
    so->so_rcv_hiwat = 0;
    so->so_rcv_lowat = 0;
    so->so_snd_hiwat = 0;
    so->so_snd_lowat = 0;
    so->so_ttl = ngIp_ttl;
    so->so_tos = ngIp_tos;
    so->so_mcast_ttl = 1;
    so->so_mcast_ifnetp = NULL;
    so->so_head = NULL;
    so->so_rcv_timeo = 0;
    so->so_snd_timeo = 0;
    so->so_linger = 0;
  }
#ifdef NG_IGMP_SUPPORTED
  for( i=0; i<NG_SOCK_MAX_MEMBERSHIPS; i++) {
    so->so_mcast_memberships[i] = NULL;
  }
#endif
  so->so_error = 0;
#ifdef NG_RTOS
  so->so_waitflags = 0;
  so->so_nsleep = 0;
  ngOSSemClear( so->so_sem);
#endif
  so->so_rcv_cc = 0;
  NG_QUEUE_INIT( &so->so_rcv_q, 0);
  so->so_snd_cc = 0;
  so->so_faddr = 0;
  so->so_fport = 0;
  so->so_laddr = 0;
  so->so_lport = 0;
  so->so_acceptq0 = NULL;
  so->so_aq0len = 0;
  so->so_acceptq = NULL;
  so->so_aqlen = 0;
  so->so_aqmax = 0;
  so->so_upcall_f = NULL;
  so->so_upcall_data = NULL;
  so->so_data = NULL;

  return( so);
}

/*****************************************************************************
 * ngSoFree()
 *****************************************************************************
 * Free a socket descriptor structure
 *****************************************************************************
 * Parameters:
 *  so        socket pointer
 * Return value: none
 */

void ngSoFree( NGsock *so)
{
#ifdef NG_IGMP_SUPPORTED
  int i;
  NGinmulti *inmp;

  /* leave multicast groups */
  for( i=0; i<NG_SOCK_MAX_MEMBERSHIPS; i++) {
    inmp = so->so_mcast_memberships[i];
    if( inmp != NULL) {
      if( --inmp->inm_refcount == 0) ngIgmpLeaveGroup( inmp);
    }
  }
#endif

  /* discard received pending data */
  NG_SOCK_RCVBUF_FLUSH( so);
  /* move socket control block to the free list */
  NG_QUEUE_IN( &ngSock_freeq, so);

}

