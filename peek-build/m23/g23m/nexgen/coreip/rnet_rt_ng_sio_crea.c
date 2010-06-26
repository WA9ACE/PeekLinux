/*****************************************************************************
 * $Id: sio_crea.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Asynchronous Sockets
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
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
 * ngSAIOCreate()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from socket()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOCreate()
 *****************************************************************************
 * Create a new socket
 *****************************************************************************
 * Parameters:
 *  newso       filled with pointer to new socket control block
 *  af          must be NG_AF_INET
 *  type        type of socket
 *  proto       protocol
 *  oflags      open flags (NG_O_NONBLOCK)
 * Return value: NG_EOK or negative error code
 */

int ngSAIOCreate( NGsock **newso, int af, int type, int proto, int oflags)
{
  int err, i;
  const NGproto *pr;
  NGsock *so;

  /* socket domain, only AF_INET supported */
  if( af != NG_AF_INET) {
    return( NG_EPROTONOSUPPORT);
  }

  /* protocol supported ? */
  pr = NULL;
  for( i=0; i<NG_PROTO_MAX; i++) {
    if( (ngProtoList[i] != NULL) &&
      (ngProtoList[i]->pr_type == type)) {
      pr = ngProtoList[i];
      break;
    }
  }
  if( pr == NULL) {
    /* no protocol for this type */
    return( NG_EPROTONOSUPPORT);
  }

  /* allocate socket structure */
  so = ngSoAlloc( NULL);
  if( so == NULL) {
    return( NG_EMFILE);
  }

  so->so_proto = pr;
  /* call protocol attach function */
  err = NG_SOCKCALL_USER( so, NG_PRU_ATTACH, NULL, proto, NULL);
  if( err) {
    /* free socket structure */
    ngSoFree( so);
  }
  else {
    *newso = so;
    /* set other parameters */
    if( oflags & NG_O_NONBLOCK)
      so->so_state |= NG_SS_NBIO;
  }
  return( err);

}

