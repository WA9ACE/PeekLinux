/*****************************************************************************
 * $Id: api_rbna.c,v 1.3 2002/04/03 19:35:32 af Exp $
 * $Name: rel_1_3_b_7 $
 *----------------------------------------------------------------------------
 * NexGenRESOLV v1.3
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
 * ngResolvByNameAsync()
 *----------------------------------------------------------------------------
 * 21/03/2001 - Adrien Felon
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 *****************************************************************************/

#include <ngos.h>
#include <ngresolv.h>
#include <ngresolv/resolv.h>

/*****************************************************************************
 * ngResolvByNameAsync()
 *****************************************************************************
 * Resolver API call to resolve given host name asynchroneously
 *****************************************************************************
 * Parameters:
 *   af          Adress family (not yet, as NG_AF_INET always assumed)
 *   name        Host name to resolve
 *   h           Host entry to store the results
 *   buf         Core memory for previous host entry
 *   buflen      Size of previous buffer
 *   flags       Resolver query flags
 *   cb          Application callback function
 *   cbdata      Call back data
 * Return value:
 *   NG_EDNS_OK         if the answer is available
 *   NG_EDNS_WOULDBLOCK if the query is pending (non blocking mode only)
 *   a negative error code
 * Caller:
 *   Application code
 */

int ngResolvByNameAsync(
  int af,
  char const * name,
  NGhostent * h,
  NGubyte * buf,
  int buflen,
  int flags,
  NGslv_cb_f cb,
  void * cbdata
) {
  int err;
  NGslvquer sq;

  ngMemSet( &sq, '\0', sizeof( sq));
  sq.sq_af = af;
  sq.sq_name = name;
  sq.sq_hostent = h;
  sq.sq_buf = buf;
  sq.sq_buflen = buflen;
  sq.sq_flags = flags | NG_RSLVF_QTYPE_GET_ADDR | NG_RSLVF_IOMODE_NONBLOCK;
  sq.sq_cb_f = cb;
  sq.sq_cbdata = cbdata;
  NG_LOCK();
  err = ngResolvQuery( &sq);
  NG_UNLOCK();
  return err;
}

