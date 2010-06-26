/*****************************************************************************
 * $Id: res_util.c,v 1.9.2.1 2002/10/28 16:13:56 af Exp $
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
 * ngResolvSocketCb_f()
 * ngResolvConnect()
 * ngResolvSendQuery()
 * ngResolvRecvAnswer()
 * ngResolvPendingQueryAbortAll()
 *----------------------------------------------------------------------------
 * 16/03/2001 - Adrien Felon
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - removing ngresolv.h inclusion (now included from ngresolv/resolv.h)
 * 28/10/2002 -
 *  - ngResolvPendingQueryAbortAll() and ngResolvRecvAnswer() test if
 *    spq_state is NG_RSLVPQ_ST_FRIEND. This checks if a query is a
 *    friend (spq_ce was previously tested errouneously...)
 *****************************************************************************/

#include <ngos.h>
#include <ngsocket.h>
#include <ngresolv/resolv.h>
#include <ngresolv/dns.h>

static int resolvUtilWriteName( NGubyte * dst, const char * src);
static int resolvUtilReadName(
  char * dst,
  int dstlen,
  NGubyte * src,
  NGubyte * src_base
);
static int resolvUtilSkipName( NGubyte * src);
static int resolvMakePTRQuery( void const * addr, char * buf, int buflen);

/*****************************************************************************
 * ngResolvSocketCb_f()
 *****************************************************************************
 * Solver socket callback function
 *****************************************************************************
 * Parameters:
 *   so          Underlying calling socket
 *   cbdata      Backpointer to internal resovler data
 *   ev          Async socket event code
 * Return value:
 *   none
 * Caller:
 *   NexGenIP socket module
 */

void ngResolvSocketCb_f( NGsock * so, void * cbdata, int ev)
{
  NGresolver * slv = (NGresolver *)cbdata;
  NGbuf * bufp;
  NGsockaddr addr;
  NGuint faddr;
  int err;

  if (NG_RSLV_ST_RUNNING != slv->s_state) return;
  if (ev & NG_SAIO_ERROR) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 4, "Socket error event" ));
    return;
  }
  if (ev & NG_SAIO_READ) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 2, "Socket read event" ));
    err = ngSAIOBufRecv( so, NULL, &addr, &bufp, &faddr);
    if (err < 0) {
      ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 4, "Receive error" ));
      return;
    }
    ngResolvRecvAnswer( so, bufp);
  }
}

/*****************************************************************************
 * ngResolvSendQuery()
 *****************************************************************************
 * Send a query to a name server, updating cache entry with outgoing query
 * information
 *****************************************************************************
 * Parameters:
 *   pq             Solver pending query to be sent
 * Return value:
 *   NG_EOK         if the query has been sent
 *   NG_EIO         otherwise
 * Caller:
 *   ngResolvPendingQueryAutomaton()
 */

int ngResolvSendQuery( NGslvpquer * pq)
{
  int err;
  NGbuf * bufp;
  NGdnshdr * dh;
  NGubyte * ptr;
  int datalen;
  NGushort id, qtype, qclass;
  char const * query_str;
  char tmpbuf[ DNS_ARPALEN_MAX];
  NGslvquer * sq = &pq->spq_query;
#ifdef NG_DEBUG
  NGuint curtime = ngOSClockGetTime();
#endif
  /* Clear associated cache record */
  ngMemSet( pq->spq_ce->sce_buf, '\0', sizeof( pq->spq_ce->sce_buf));
  pq->spq_ce->sce_addrcount = 0;
  pq->spq_ce->sce_namecount = 0;

  /* Allocate a buffer for the query message */
  err = ngSAIOBufAlloc( ngResolv.s_so, &bufp);
  if (NG_EOK != err) {
    return err;
  }
  id = pq->spq_qid;

  /* Construct the query */
  dh = (NGdnshdr *) NG_SOCKBUF_PTR( bufp);
  ngMemSet( dh, 0, sizeof( NGdnshdr));
  dh->dns_id = id;
  dh->dns_bitf1 |= DNS_MASK_RD | DNS_QUERY;
  /*lint -e572 (Warning -- Excessive shift value) */
  dh->dns_qdcount = ngConstHTONS( 1); /* 1 query */

  /* */
  if (sq->sq_flags & NG_RSLVF_QTYPE_GET_NAME) {
    /* Make a PTR query */
    resolvMakePTRQuery( sq->sq_addr, tmpbuf, sizeof( tmpbuf));
    query_str = tmpbuf;
    qtype = ngConstHTONS( DNS_T_PTR);
    /* Update cache entry */
    *(NGuint*)
      ( pq->spq_ce->sce_buf + (sizeof( pq->spq_ce->sce_buf)-sizeof(NGuint)) )
      = (NGuint)sq->sq_addr;
    pq->spq_ce->sce_addrcount = 1;
  } else {
    /* Make an A query */
    query_str = pq->spq_qname;
    qtype = ngConstHTONS( DNS_T_A);
    /* Update cache entry */
    /* TODO: check size of query_str */
    ngMemCpy( pq->spq_ce->sce_buf, query_str, ngStrLen( query_str)+1);
    *((char*)pq->spq_ce->sce_buf + ngStrLen( query_str)+2) = '\0';
  }
  qclass = ngConstHTONS( DNS_C_IN);
  /*lint +e572 (Warning -- Excessive shift value) */

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 1,
                  "qid=%u rmit=%d timeout=%lums str=\"%s\"",
                  id, pq->spq_rmit,
                  ((pq->spq_timeout - curtime) * 1000) / ngOSClockGetFreq(),
                  query_str));

  /* Building the query into bufp */
  ptr = ((NGubyte *) dh) + sizeof( NGdnshdr);
  ptr += resolvUtilWriteName( ptr, query_str);
  ngMemCpy( ptr, &qtype, sizeof( qtype));
  ptr += sizeof( qtype);
  ngMemCpy( ptr, &qclass, sizeof( qclass));
  ptr += sizeof( qclass);

  /* Setting size of message */
  datalen = ptr - (NGubyte *) dh;
  NG_SOCKBUF_SETLEN( bufp, datalen);

  /* send the query */
  err = ngSAIOBufSend( ngResolv.s_so, bufp, 0, NULL);
  if (datalen != err) {
    if (err < 0)
      return err;
    return NG_EIO;
  }
  return NG_EOK;
}

/*****************************************************************************
 * ngResolvRecvAnswer()
 *****************************************************************************
 * Receive an answer from a name server
 *****************************************************************************
 * Parameters:
 *   so          Backpointer to socket
 *   bufp        Buffer containing incoming answer (to be freed)
 * Return value:
 *   none
 * Caller:
 *   ngResolvSocketCb_f()
 */

void ngResolvRecvAnswer( NGsock * so, NGbuf * bufp)
{
  NGdnshdr * dh;
  NGslvcaent * ce;
  NGslvpquer * pq;
  NGubyte * ptr;
  int i;
  NGushort qtype, reslen;
  NGuint ttl, minttl;
  char * hostname;
  int hostnamelen;
  NGuint * hostAddr;
  int addridx;
  NGuint curtime = ngOSClockGetTime();

  /* Get the answer */
  dh = (NGdnshdr *) NG_SOCKBUF_PTR( bufp);

  /* Test for valid message */
  if (
      ((unsigned)(NG_SOCKBUF_LEN( bufp)) < sizeof( NGdnshdr)) ||
      (dh->dns_bitf1 & DNS_MASK_TC) ||
      (!(dh->dns_bitf1 & DNS_MASK_QR))
  ) goto bad;

  /* Search for an active pending query with this query id... */
  pq = ngResolv.s_pquerbusyq;
  while (NULL != pq) {
    if ( (NG_RSLVPQ_ST_FRIEND != pq->spq_state) &&
         (pq->spq_qid == dh->dns_id) ) break;
    pq = pq->spq_next;
  }
  if (NULL == pq) goto bad;

  switch (DNS_MASK_RCODE & dh->dns_bitf2) {
  case DNS_NOERROR:
    pq->spq_errcode = NG_EDNS_OK;
    break;
  case DNS_NXDOMAIN:
    pq->spq_errcode = NG_EDNS_HOST_NOT_FOUND;
    break;
  case DNS_SERVFAIL:
    pq->spq_errcode = NG_EDNS_TRY_AGAIN;
    break;
  case DNS_FORMERR:
  case DNS_NOTIMP:
  case DNS_REFUSED:
  default:
    pq->spq_errcode = NG_EDNS_NO_RECOVERY;
    break;
  }

  if (NG_EDNS_OK != pq->spq_errcode) {
    ngSAIOBufFree( so, bufp);
    ngResolvPendingQueryAutomaton( pq, NG_RSLVPQ_EV_ERROR);
    return;
  }

  /* test for valid answer */
  dh->dns_qdcount = ngNTOHS( dh->dns_qdcount);
  dh->dns_ancount = ngNTOHS( dh->dns_ancount);

  /* Check we have an answer entry to parse */
  if (0 == dh->dns_ancount)
    goto bad;

  /* Shortcut to underlying cache entry */
  ce = pq->spq_ce;

  /* Preparing to receive packet directly in cache entry */
  hostname = (char *) (ce->sce_buf);
  hostAddr = (NGuint *)(ce->sce_buf + (sizeof(ce->sce_buf)-sizeof(NGuint)));
  /* hostnamelen = (int)((NGubyte*)hostAddr - (NGubyte*)hostname); */

  /* Update pointers to keep query data into cache entry (we will
   * never update both pointers at the same time) */
  if (ce->sce_addrcount == 1)
    hostAddr--;
  if (hostname[0] != '\0')
    hostname += ngStrLen( hostname) + 1;

  /* Ok to parse the reply now! */
  ptr = ((NGubyte *) dh) + sizeof( NGdnshdr);

  /* Skip query */
  for( i=0; i<dh->dns_qdcount; i++) {
    ptr += resolvUtilSkipName( ptr);
    ptr += 2 * sizeof( NGushort);
  }
  /* Parsing answer */

  /*
   * This loop must preserve invariant (hostname < hostAddr) to be sure
   * to always write into ce->sce_buf. Note that hostname goes forward
   * while hostAddr goes backward in memory.
   */
  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 1,
                  "qid=%u ancount=%u (parsing answer) date:%lu",
                  dh->dns_id, dh->dns_ancount, curtime));
  minttl = 0xffffffff;
  addridx = 0;
  for (i=0; i<dh->dns_ancount; i++) {
    NGuint addr;
    ptr += resolvUtilSkipName( ptr);
    ngMemCpy( &qtype, ptr, sizeof( qtype));
    ptr += sizeof( qtype);
    ptr += sizeof( NGushort);
    ngMemCpy( &ttl, ptr, sizeof( ttl));
    ptr += sizeof( NGuint);
    ttl = ngNTOHL( ttl);
    if (ttl < minttl) minttl = ttl;
    ngMemCpy( &reslen, ptr, sizeof( reslen));
    ptr += sizeof( reslen);
    reslen = ngNTOHS( reslen);
    /*lint -e572 (Warning -- Excessive shift value) */
    switch (qtype) {
    case ngConstHTONS( DNS_T_A):
      if (reslen == sizeof( addr)) {
        ngMemCpy( &addr, ptr, sizeof( addr));
        if ((NGubyte*)(hostAddr-1) > (NGubyte*)hostname) {
          *hostAddr = addr;
          hostAddr--;
          addridx++;
          ce->sce_addrcount = addridx;
          ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0,
                          " ADR ttl=%8lu %I ", ttl, addr));
        }
#ifdef NG_DEBUG
        else {
          ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0,
                          " ADR ttl=%8lu %I (skipped)", ttl, addr));
        }
#endif
      }
      break;
    case ngConstHTONS( DNS_T_CNAME):
    case ngConstHTONS( DNS_T_PTR):
      if ((NGubyte*)(hostname + reslen) < (NGubyte*)hostAddr) {
        hostnamelen = (int)((NGubyte*)hostAddr - (NGubyte*)hostname);
        resolvUtilReadName( hostname, hostnamelen, ptr, (NGubyte *) dh);
        ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0,
                        " PTR ttl=%8lu %s", ttl, hostname));
        hostname += ngStrLen( hostname) + 1;
      }
#ifdef NG_DEBUG
      else {
        ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0,
                        " PTR ttl=%8lu (skipped)", ttl));
      }
#endif

      break;
    }
    /*lint +e572 (Warning -- Excessive shift value) */
    ptr += reslen;
  }
  /* Terminating list of hostnames... (ended with doubled '\0') */
  hostname[0] = '\0';

  /* Update ttl of cache entry with min ttl of all answers... */
  ce->sce_ttl = curtime + (minttl * ngOSClockGetFreq());
  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INPUT, 0,
                  " Cache entry expiring in %lus (%lu clock ticks)",
                  minttl, minttl * ngOSClockGetFreq()));

  /* Freeing buffer (no more needing it...) */
  ngSAIOBufFree( so, bufp);

  /* We just got something from a server so reset global rmit count... */
  ngResolv.s_rmit = 0;
  ngResolv.s_switch = 0;

  ngResolvPendingQueryAutomaton( pq, NG_RSLVPQ_EV_RECV);
  return;

 bad:
  ngSAIOBufFree( so, bufp);
}

/*****************************************************************************
 * ngResolvRmitIncr()
 *****************************************************************************
 * Resolver rmit function. Called by a pending query when it wishes to
 * rmit its query, this function is used to update global rmit counter
 * (max rmit of all pending queries).
 *****************************************************************************
 * Parameters:
 *   rmit        Retransmission count for underlying pending query
 * Return value:
 *   NG_EOK     on success
 *   NG_EINVAL  on error
 * Caller:
 *   ngResolvPendingQueryAutomaton()
 */

int ngResolvRmitIncr( NGuint rmit)
{

  /* First transmission to any server are fine! */
  if (0 == rmit) return NG_EOK;
  if (0 != (rmit % ngResolv.s_rmitmax)) return NG_EOK;

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2,
                  "rmitmax reached! (rmit = %d)",
                  rmit % ngResolv.s_rmitmax));

  return ngResolvSwitch();
}

/*****************************************************************************
 * ngResolvSwitch()
 *****************************************************************************
 * Switch to the other DNS server (if any).
 *****************************************************************************
 * Parameters:
 *   none
 * Return value:
 *   NG_EOK     on success
 *   NG_EINVAL  on error
 * Caller:
 *   ngResolvRmitIncr()
 */

int ngResolvSwitch( void)
{
  NGuint addr;
  NGushort port;
  int err;

  /* Check if we can switch servers... */
  switch (ngResolv.s_serv) {
  case 1:
    addr = ngResolv.s_serv2_addr;
    port = ngResolv.s_serv2_port;
    break;
  default:
  case 2:
    addr = ngResolv.s_serv1_addr;
    port = ngResolv.s_serv1_port;
    break;
  }

  if ( (INADDR_ANY == addr) || (0 == port) ) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2,
                    "Can not switch server!" ));
    goto abort_pquer;
  }

  /* Switching server... */
  ngResolv.s_switch++;
  if (ngResolv.s_switch > ngResolv.s_switchmax) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2, "switchmax reached!" ));
    /* Reseting Forcing primary DNS server... */
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2,
                    "Reset to primary server!" ));
    ngResolvConnect( ngResolv.s_serv1_addr, ngResolv.s_serv1_port);
    ngResolv.s_serv = 1;
    ngResolv.s_switch = 0;
    goto abort_pquer;
  }

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2, "Switching to server %I:%u",
                  addr, port));

  err = ngResolvConnect( 0, 0);
#ifdef NG_DEBUG
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2,
                    "Cannot UDP connect to server" ));
  }
#endif

  err = ngResolvConnect( addr, port);
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_OUTPUT, 2,
                    "Cannot UDP connect to server" ));
    goto abort_pquer;
  }

  ngResolv.s_serv = (ngResolv.s_serv == 1) ? 2 : 1;
  ngResolv.s_rmit = 0;
  return NG_EOK;

 abort_pquer:
  ngResolvPendingQueryAbortAll();
  return NG_EINVAL;
}

/*****************************************************************************
 * ngResolvConnect()
 *****************************************************************************
 * Resolver UDP connect function
 *****************************************************************************
 * Parameters:
 *   addr        IP address of DNS server to connect to
 *   port        UDP port number to use
 * Return value:
 *   NG_EOK      on success
 *   a negative error code otherwise
 * Caller:
 *   ngResolvSwitch()
 *   resolvCntl()
 */

int ngResolvConnect( NGuint addr, NGushort port)
{
  int err;
  NGsockaddr saddr;

  /* Connecting socket... */
  ngMemSet( &saddr, 0, sizeof( saddr));
  saddr.sin_len = sizeof( addr);
  saddr.sin_family = AF_INET;
  saddr.sin_addr = addr;
  saddr.sin_port = ngHTONS( port);

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 2, "Connecting to server %I:%u",
                  addr, port));
  err = ngSAIOConnect( ngResolv.s_so, &saddr, 0);
  if (NG_EOK != err) {
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 4,
                    "ngSAIOConnect() has error %d", err));
    return err;
  }

  ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 2, "Connected" ));

  return NG_EOK;

}

/*****************************************************************************
 * ngResolvPendingQueryAbortAll()
 *****************************************************************************
 * Abort all active pending queries
 *****************************************************************************
 * Parameters:
 *   none
 * Return value:
 *   none
 * Caller:
 *   ngResolvSwitch()
 *   resolvCntl()
 */

void ngResolvPendingQueryAbortAll( void)
{
  NGslvpquer * pq = ngResolv.s_pquerbusyq;
  while (NULL != pq) {
    NGslvpquer * pqnext = pq->spq_next;
    pq->spq_errcode = NG_EDNS_TIMEDOUT;
    if (NG_RSLVPQ_ST_FRIEND != pq->spq_state)
      ngResolvPendingQueryAutomaton( pq, NG_RSLVPQ_EV_ERROR);
    pq = pqnext;
  }
}

/*****************************************************************************
 * ngResolvBuildQueryByName()
 *****************************************************************************
 * Build a full qualified query string (hostname + domain name)
 *****************************************************************************
 */

void ngResolvBuildQueryByName( char const * name, char * obuf, int obuflen)
{
  int namelen = ngStrLen( name);
  obuf[0] = '\0';
  /* For query "ByName" without any dot we want to add domain name */
  if ('\0' != ngResolv.s_domain[0]) {
    int domainlen = ngStrLen( ngResolv.s_domain);
    /* Check if the requested name has any dots in it */
    int i = 0;
    int dots = 0;
    while ('\0' != name[i]) {
      if ('.' == name[i]) {
        dots = 1;
        break;
      }
      i++;
    }
    /* TODO: invalidating any trailing dot! */

    if ( (!dots) && ( (namelen + 1 + domainlen) < obuflen ) ) {
      /* Append domain to the end of requested name */
      ngMemCpy( obuf, name, namelen);
      obuf[ namelen++] = '.';
      ngMemCpy( obuf + namelen, ngResolv.s_domain, domainlen );
      obuf[ namelen + domainlen ] = '\0';
    }
  }
  if ('\0' == obuf[0]) {
    namelen = NG_MIN( namelen, obuflen-1);
    ngMemCpy( obuf, name, namelen);
    obuf[ namelen] = '\0';
  }
}

/* Internal routines */

static int resolvUtilWriteName( NGubyte * dst, const char * src)
{
  int totlen, len;

  totlen = 0;
  while( 1) {
    len = 0;
    while( (*(src+len) != '.') && (*(src+len) != 0)) len++;
    if( len > 63) return( -1); /* invalid name */
    if( len == 0) break;
    totlen += len + 1;
    *dst++ = len;
    while( len > 0) {
      *dst++ = *src++;
      len--;
    }
    if( *src == 0) break;
    src++; /* skip dot */
  }
  *dst = 0x00;
  totlen++;
  return( totlen);
}

static int resolvUtilReadName(
  char * dst,
  int dstlen,
  NGubyte * src,
  NGubyte * src_base
) {
  int totlen, len, rflag, i;

  totlen = 0;
  rflag = 0;
  while( 1) {
    if( !rflag) totlen++;
    len = *src++;
    if( len == 0) break; /* end of string */
    if( len > 63) {
      /* compressed name */
      if( !rflag) totlen++;
      rflag++;
      /* new pointer */
      len = (len & 0x3f)<<8;
      len |= *src;
      src = src_base + len;
    }
    else {
      if( !rflag) totlen += len;
      /* copy string */
      for( i=0; i<len; i++) {
        if( dstlen > 1) {
          dstlen--;
          *dst++ = *src++;
        }
        else src++;
      }
      if( dstlen > 1) *dst++ = '.';
    }
  }
  if( totlen > 0) --dst; /* remove last dot */
  *dst = 0;
  return( totlen);
}

static int resolvUtilSkipName( NGubyte * src)
{
  int totlen, len;

  totlen = 0;
  while( 1) {
    totlen++;
    len = *src++;
    if( len == 0) break; /* end of string */
    if( len > 63) {
      /* compressed name */
      totlen++; /* one more byte for pointer */
      break;
    }
    /* skip name */
    src += len;
    totlen += len;
  }
  return( totlen);
}

static int dns_itoa( char *ptr, int val);

static char * dns_arpadomain = ".in-addr.arpa";

/*****************************************************************************
 * resolvMakePTRQuery()
 *****************************************************************************
 * construct a pointer query from an IP address
 *****************************************************************************
 * Parameters:
 * Return value: 0 on success or an error code
 */

static int resolvMakePTRQuery( void const * addr, char * buf, int buflen)
{
  char * ptr, * sptr;
  NGubyte * aptr;
  NGuint inetaddr = (NGuint)addr;

  if ((unsigned)buflen < DNS_ARPALEN_MAX) return NG_EINVAL;

  /* construct ip address string (bytes are reversed) */
  aptr = (NGubyte *) &inetaddr;
  ptr = buf;
  ptr += dns_itoa( ptr, aptr[3]);
  *ptr++ = '.';
  ptr += dns_itoa( ptr, aptr[2]);
  *ptr++ = '.';
  ptr += dns_itoa( ptr, aptr[1]);
  *ptr++ = '.';
  ptr += dns_itoa( ptr, aptr[0]);
  /* add arpa domain name */
  sptr = dns_arpadomain;
  while (1) {
    if ( (*ptr++ = *sptr++) == 0) break;
  }
  return NG_EOK;
}

/* convert byte (0-255) to ascii */
static int dns_itoa( char *ptr, int val)
{
  if( val >= 100) {
    *ptr++ = (val/100) + '0';
    val = val % 100;
    *ptr++ = (val/10) + '0';
    val = val % 10;
    /* *ptr++ = val + '0'; */
    *ptr = val + '0';
    return 3;
  } else if( val >= 10) {
    *ptr++ = (val/10) + '0';
    val = val % 10;
    /* *ptr++ = val + '0'; */
    *ptr = val + '0';
    return( 2);
  }
  *ptr = val + '0';
  return 1;
}

