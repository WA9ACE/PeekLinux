/*****************************************************************************
 * $Id: res_quer.h,v 1.1 2002/07/03 08:48:21 af Exp $
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
 * 03/07/2001 - Adrien Felon
 *****************************************************************************/

#ifndef __NG_RES_QUER_H_INCLUDED__
#define __NG_RES_QUER_H_INCLUDED__

#include <ngos.h>
#include <ngdsock.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* Mimimum length of a host name */
#define NG_RSLV_BUF_NAME_LEN 64
/* Length of a host IP address */
#define NG_RSLV_BUF_ADDR_LEN sizeof( NGuint)
/* Length of cached host entry buffer (128 extra bytes for data...) */
#define NG_RSLV_BUF_LEN (NG_RSLV_BUF_NAME_LEN + NG_RSLV_BUF_ADDR_LEN + 128)

#define NG_RSLV_APP_MIN_BUF \
 (sizeof( char *) + 2*sizeof( NGuint *) + \
  + NG_RSLV_BUF_NAME_LEN + NG_RSLV_BUF_ADDR_LEN)

/* Defining structure to post query */

typedef struct NGslvquer_S {
  int sq_flags; /* DNS query type and I/O mode */
  NGhostent * sq_hostent; /* Pointer to store the result */
  void * sq_buf; /* Buf for results */
  int sq_buflen; /* Length of previous buffer */
  int sq_af; /* Address family */
  union {
    char const * sq_name; /* Host name to get address of */
    void const * sq_addr; /* IP address of host to get name of */
  } sq_qtype_args;
  NGslv_cb_f sq_cb_f; /* Application callback fct */
  void * sq_cbdata; /* Application callback data */
} NGslvquer;

/* Defining some shorcuts to acces union of previous structure */

#define sq_name sq_qtype_args.sq_name
#define sq_addr sq_qtype_args.sq_addr

/* Defining DNS resolver cache entries */

typedef struct NGslvcaent_S {
  struct NGslvcaent_S * sce_prev;
  struct NGslvcaent_S * sce_next;
  NGuint sce_ttl; /* Time to live of this entry */
  NGubyte sce_buf[ NG_RSLV_BUF_LEN]; /* Buf holding host entry */
  int sce_addrcount; /* Number of IP address in sce_buf */
  int sce_namecount; /* Number of names in sce_buf */
  struct NGslvpquer_S *
                  sce_pquer; /* List of pending queries (if any) */
  int sce_pquercount; /* Number of pending queries */
} NGslvcaent;

/* Defining DNS resolver pending queries */

typedef struct NGslvpquer_S {
  struct NGslvpquer_S * spq_prev; /* Prev busy query */
  struct NGslvpquer_S * spq_next; /* Next busy query */
  struct NGslvpquer_S * spq_friend; /* Next friend queries */
  int spq_state; /* Pending query state */
  NGuint spq_timeout; /* Timeout for answer (clock tick) */
  int spq_rmit; /* Number of retransmission done */
  int spq_errcode; /* Where to store the err code */
  NGushort spq_qid; /* Query identifier */
  NGslvcaent * spq_ce; /* Cache entry to be filled */
  NGslvquer spq_query; /* Copy of the application query */
  char spq_qname[ NG_RSLV_BUF_NAME_LEN]; /* Generated hostname */
} NGslvpquer;

/* NG_END_DECLS // confuses Source Insight */

#endif /* __NG_RES_QUER_H_INCLUDED__ */

