/*****************************************************************************
 * $Id: resolv.h,v 1.6 2002/07/08 09:48:32 dg Exp $
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
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - including structure definition from new header file ngresolv/res_quer.h
 *  - defining macros for internal state of pending queries
 * 08/07/2002 - dg
 *  - remove forward declaration of NGhostent_S since it cause a compile
 *    error with IAR compilers (m16c & h8s)
 *****************************************************************************/

#ifndef __NG_RESOLV_H_INCLUDED__
#define __NG_RESOLV_H_INCLUDED__

#include <ngos.h>
#include <ngdsock.h>

#include <ngresolv.h>
#include "rnet_rt_i.h"

/* NG_BEGIN_DECLS // confuses Source Insight */

/* Structure holding internal state of DNS resolver (cf ngSolver global var) */

typedef struct {
  /* Member set by application */
  NGslvcaent * s_ca; /* Table of cache entries */
  NGuint s_camax; /* Size of cache (max num of entry) */
  NGslvpquer * s_pquer; /* Table for pending queries */
  NGuint s_pquermax; /* Max num of pending query */
  NGuint s_serv1_addr; /* Primary DNS server IP address */
  NGushort s_serv1_port; /* Primary DNS server UDP port num */
  NGuint s_serv2_addr; /* Secundary DNS server IP address */
  NGushort s_serv2_port; /* Secundary DNS server UDP port num */
#define MAX_DOMAIN 32
  char s_domain[MAX_DOMAIN]; /* Resolver domain name */
  NGuint s_timeout; /* Default to for pending queries */
  NGuint s_rmitmax; /* Max number of retries */
  NGuint s_switchmax; /* Max number of server switch */
  /* Runtime members */
  int s_state; /* Solver state */
#define NG_RSLV_ST_UNINIT 0
#define NG_RSLV_ST_STOPPED 1
#define NG_RSLV_ST_RUNNING 2
  NGsock * s_so; /* Socket for coms with DNS servers */
  NGslvcaent * s_cafreeq; /* Head of free cache entry queue */
  NGslvcaent * s_cabusyq; /* Head of busy cache entry queue */
  int s_cabusyn; /* Number of busy cache entries */
  NGslvpquer * s_pquerfreeq; /* Head of free pending query queue */
  NGslvpquer * s_pquerbusyq; /* Head of busy pending query queue */
  int s_pquerbusyn; /* Number of busy pending queries */
  NGushort s_qid; /* Next query identifier */
  NGuint s_rmit; /* Current number of retries */
  NGuint s_switch; /* Current number of server switch */
  int s_serv; /* Current server index (1 or 2) */
#ifdef NG_RTOS
  NGOSsem * s_sem; /* Mutex for API calls in RTOS mode */
#endif
} NGresolver;

/* Defining builtin DNS resolver default values  */

#define NG_RSLV_TIMEOUT 1000 /* 1 seconde before retransmit */
#define NG_RSLV_RMITMAX 3 /* retransmit 3 times */
#define NG_RSLV_SWITCHMAX 1 /* only switching once */
#define NG_RSLV_PORT 53 /* UDP server port number */

int ngResolvStart( void);
int ngResolvQuery( NGslvquer const * sq);

/* Exporting global var to other module source files */

extern NGresolver ngResolv;

void ngResolvSocketCb_f( NGsock * so, void * slv, int ev);
int ngResolvSendQuery( NGslvpquer * pq);
void ngResolvRecvAnswer( NGsock * so, NGbuf * bufp);
int ngResolvRmitIncr( NGuint rmit);
int ngResolvSwitch( void);
int ngResolvConnect( NGuint addr, NGushort port);

void ngResolvPendingQueryAbortAll( void);
int ngResolvPendingQueryAutomaton( NGslvpquer * pq, int ev);

/* Defining events for automaton (cf. "ev" arg of funtion above) */

#define NG_RSLVPQ_EV_INIT 1
#define NG_RSLVPQ_EV_TIMER 2
#define NG_RSLVPQ_EV_SEND 3
#define NG_RSLVPQ_EV_RECV 4
#define NG_RSLVPQ_EV_ERROR 5

/* Defining states of pending queries (cf. member spq_state of NGslvpquer) */

#define NG_RSLVPQ_ST_FIRST_SENT 0
#define NG_RSLVPQ_ST_RMIT 1
#define NG_RSLVPQ_ST_FRIEND 2
#define NG_RSLVPQ_ST_DONE 3

/* Resolver util functions */

void ngResolvBuildQueryByName( char const * name, char * obuf, int obuflen);

/* Resolver cache functions */

int ngResolvCacheLookup( NGslvquer const * sq, NGslvcaent ** ce);
int ngResolvCacheEntAlloc( NGslvcaent ** ce);
void ngResolvCacheEntFree( NGslvcaent * ce);
void ngResolvFillWithCacheEnt( NGhostent * h, void * buf, int buflen,
                               NGslvcaent * ce);

#ifdef NG_DEBUG
#define ngDebugResolv(arg) ngDebug arg
#else
#define ngDebugResolv(arg)
#endif

/* NG_END_DECLS // confuses Source Insight */

#endif /* __NG_RESOLV_H_INCLUDED__ */

