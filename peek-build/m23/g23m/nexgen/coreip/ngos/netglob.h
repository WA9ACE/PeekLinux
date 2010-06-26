/*****************************************************************************
 * $Id: netglob.h,v 1.4 2001/03/06 15:30:32 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Network Global Variables
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
 * 18/07/2000 - Regis Feneon
 *  v1.2 - moved from net.h
 * 14/02/2001 -
 *  added ngSock_selsem and ngSock_selnsleep
 *****************************************************************************/

#ifndef __NG_NETGLOB_H_INCLUDED__
#define __NG_NETGLOB_H_INCLUDED__

#include <ngos/queue.h>
#include <ngos/netbuf.h>
#include <ngos/netif.h>
#include <ngos/netprot.h>
#include <ngos/netsock.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* global data */

extern const NGproto *ngProtoList[NG_PROTO_MAX]; /* list of protocols */
extern NGifnet *ngIfList; /* chained list of interfaces */

extern void *ngBuf_pool; /* pointer to message buffer pool */
extern NGuint ngBuf_physaddr; /* physical address of message buffer pool */
extern NGqueue ngBuf_freeq; /* queue of free message buffers */
extern NGqueue ngBuf_inputq; /* global message input queue */

extern u_int ngBufDataMax; /* maximum data size in message buffer */
extern u_int ngBufDataOffset; /* ip header offset in message buffer */

extern NGqueue ngSock_freeq; /* queue of free socket descriptors */
#ifdef NG_RTOS
extern NGOSsem ngSock_selsem; /* semaphore to block tasks on */
extern int ngSock_selnsleep; /* number of tasks wainting on select() */
#endif
extern void *ngFd_table[NG_FD_MAX]; /* table of 'file' descriptors */

#ifdef NG_RTOS
/* internal tasks */
void ngInputFunc( void *data);
void ngTimerFunc( void *data);
/* tasks control blocks */
extern NGOStask ngInputTask;
extern NGOStask ngTimerTask;
/* synchros */
extern NGOSievent ngInputEvent;

#endif

/* NG_END_DECLS // confuses Source Insight */

#endif

