/*****************************************************************************
 * $Id: ngresolv.h,v 1.4 2002/07/03 08:48:56 af Exp $
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
 * 09/03/2001 - Adrien Felon
 * 03/07/2002 -
 *   structure definition moved to new header file ngresolv/res_quer.h
 *   no more exporting private functions
 *****************************************************************************/

#ifndef __NG_NGRESOLV_H_INCLUDED__
#define __NG_NGRESOLV_H_INCLUDED__

#include <ngos.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* Defining our own hostent structure */

typedef struct NGhostent_S {
  char * h_name; /* host name */
  char ** h_aliases; /* list of aliases */
  int h_addrtype; /* type of address (always AF_INET) */
  int h_length; /* length of address */
  void ** h_addr_list; /* list of addresses */
} NGhostent;

/* Defining Callback prototype */

typedef void (*NGslv_cb_f)( int code, void * cbdata);

/* Including resolver structures (opaque to applications...) */

#include <ngresolv/res_quer.h>

/* Protocol configuration options */

/* Primary DNS server IP Address. */
#define NG_RSLVO_SERV1_IPADDR 0x0501 /* NGuint */
/* Primary DNS server UDP port number. */
#define NG_RSLVO_SERV1_PORT 0x0502 /* NGushort */
/* Secondary DNS server IP Address. */
#define NG_RSLVO_SERV2_IPADDR 0x0503 /* NGuint */
/* Secondary DNS server UDP port number. */
#define NG_RSLVO_SERV2_PORT 0x0504 /* NGushort */
/* Resolver domain nam. */
#define NG_RSLVO_DOMAIN 0x0505 /* char * */
/* Size of next 2 tables. */
#define NG_RSLVO_QUERY_MAX 0x0510 /* NGuint */
/* Table for pending DNS queries. */
#define NG_RSLVO_QUERY 0x0511 /* NGslvpquer */
/* Semaphore tables for sync API calls in RTOS mode.*/
#define NG_RSLVO_SEM 0x0512 /* NGOSsem * */
/* Size of next table. */
#define NG_RSLVO_CACHE_MAX 0x0513 /* NGuint */
/* Table for cached DNS entries. */
#define NG_RSLVO_CACHE_ENT 0x0514 /* NGslvcacheent */
/* Size of next table. */
#define NG_RSLVO_HOST_MAX 0x0515 /* NGuint */
/* Table for static host entries. */
#define NG_RSLVO_HOST_ENT 0x0516 /* NGslvsthent */
/* Timeout for queries (in milli seconds). */
#define NG_RSLVO_TO 0x0517 /* NGuint */
/* Number of retries before switching server. */
#define NG_RSLVO_RMITMAX 0x0518 /* NGuint */
/* Number of switch before aborting queries. */
#define NG_RSLVO_SWITCHMAX 0x0519 /* NGuint */

int ngResolvGetOption( int opt, void * optval);
int ngResolvSetOption( int opt, void * optval);

/* Returned codes for following API calls (+ usual NexGenOS error codes) */

/* Common returned codes (not error codes...) */
#define NG_EDNS_OK NG_EOK
#define NG_EDNS_WOULDBLOCK -1000
/* Error code from RFC 2553 sect 6.2 */
#define NG_EDNS_HOST_NOT_FOUND -1001
#define NG_EDNS_NO_ADDRESS -1002
#define NG_EDNS_NO_RECOVERY -1003
#define NG_EDNS_TRY_AGAIN -1004
/* NexGenRESOLV specific error codes */
#define NG_EDNS_PROTO_SHUTDOWN -1006
#define NG_EDNS_TIMEDOUT -1007

/* Defining flags for resolver queries */

#define NG_RSLVF_IOMODE_BLOCK 0x00
#define NG_RSLVF_IOMODE_NONBLOCK 0x01
#define NG_RSLVF_QTYPE_GET_ADDR 0x00
#define NG_RSLVF_QTYPE_GET_NAME 0x02
#define NG_RSLVF_FORCE_QUERY 0x04

/* Sync API calls */

int ngResolvByAddr(
  int af,
  void const * addr,
  NGhostent * h,
  NGubyte * buf,
  int buflen,
  int flags
);

int ngResolvByName(
  int af,
  char const * name,
  NGhostent * h,
  NGubyte * buf,
  int buflen,
  int flags
);

/* Async API calls */

int ngResolvByAddrAsync(
  int af,
  void const * addr,
  NGhostent * h,
  NGubyte * buf,
  int buflen,
  int flags,
  NGslv_cb_f cb,
  void * cbdata
);

int ngResolvByNameAsync(
  int af,
  char const * name,
  NGhostent * h,
  NGubyte * buf,
  int buflen,
  int flags,
  NGslv_cb_f cb,
  void * cbdata
);

#ifdef NG_RTOS
#endif

/* Protocol description structure */

extern const NGproto ngProto_RESOLV;

/* NG_END_DECLS // confuses Source Insight */

#endif /* __NG_NGRESOLV_H_INCLUDED__ */

