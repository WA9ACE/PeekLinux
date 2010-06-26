/*****************************************************************************
 * $Id: so_iscnc.c,v 1.3.2.1 2002/10/04 13:47:23 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Level Internal Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2002 NexGen Software.
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
 * ngSoIsConnected()
 * ngSoIsDisconnected()
 *----------------------------------------------------------------------------
 * 17/12/98 - Regis Feneon
 * 27/06/99 -
 *  ngSoIsDisconnected
 *  added tests for head->so_acceptq and head->so_acceptq0 not NULL
 *  when head is not NULL
 * 30/09/99 -
 *  ngSoIsConnected
 *  active connections: so->so_head was accessed instead of so
 * 14/02/2001 -
 *  calls to ngSoWakeUp()
 * 01/10/2002 -
 *  ngSoIsDisconnected: reset of pending connection
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoIsConnected()
 *****************************************************************************
 * set a socket to connected state
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 * Return value: none
 */

void ngSoIsConnected( NGsock *so)
{
  NGsock *head;

  /* set new socket state */
  (so)->so_state &= ~(NG_SS_ISCONNECTING|NG_SS_ISDISCONNECTING);
  (so)->so_state |= NG_SS_ISCONNECTED;

  if( so->so_head) {
    /* passive connection */
    /* move so from acceptq0 to acceptq */
    head = so->so_head;
    while( head->so_acceptq0 && (head->so_acceptq0 != so))
      head = head->so_acceptq0;
    if( head->so_acceptq0) {
        head->so_acceptq0 = head->so_acceptq0->so_acceptq0;
        so->so_head->so_aq0len--;
    }
    head = so->so_head;
    while( head->so_acceptq) head = head->so_acceptq;
    head->so_acceptq = so;
    so->so_acceptq0 = NULL;
    so->so_head->so_aqlen++;
    /* wake up waiting process on parent socket */
    /* upcall routine */
    ngSoWakeUp( so->so_head, NG_SIOSEL_READ|NG_SIOWAIT_READ);
  }
  /* active connection */
  else {
    /* wake up waiting process */
    /* upcall routine */
    ngSoWakeUp( so, NG_SIOSEL_READ|NG_SIOSEL_WRITE|
                    NG_SIOWAIT_READ|NG_SIOWAIT_WRITE);
  }
}

/*****************************************************************************
 * ngSoIsDisconnected()
 *****************************************************************************
 * set a socket to disconnected state
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 * Return value: none
 */

void ngSoIsDisconnected( NGsock *so)
{
  NGsock *head;

  if( so->so_head) {
    /* remove socket from accept queue */
    head = so->so_head;
    while( (head->so_acceptq0 != so) && (head->so_acceptq0 != NULL))
        head = head->so_acceptq0;
/*    if( head == NULL) { */
    if( head->so_acceptq0 == NULL) {
      head = so->so_head;
      while( (head->so_acceptq != so) && (head->so_acceptq != NULL))
        head = head->so_acceptq;
      if( head->so_acceptq) {
        so->so_head->so_aqlen--;
        head->so_acceptq = so->so_acceptq;
        so->so_acceptq = NULL;
      }
    }
    else { /* if( head->so_acceptq0) { */
      so->so_head->so_aq0len--;
      head->so_acceptq0 = so->so_acceptq0;
      so->so_acceptq0 = NULL;
    }
  }
  /* update socket state */
  so->so_state &= ~(NG_SS_ISCONNECTING|NG_SS_ISCONNECTED|NG_SS_ISDISCONNECTING);
  so->so_state |= NG_SS_CANTRCVMORE|NG_SS_CANTSENDMORE;
  /* wake up waiting process */
  /* upcall routine */
  ngSoWakeUp( so, NG_SIOSEL_READ|NG_SIOSEL_WRITE|
                  NG_SIOWAIT_READ|NG_SIOWAIT_WRITE);

}

