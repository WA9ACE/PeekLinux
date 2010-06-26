/*****************************************************************************
 * $Id: ngnet.h,v 1.2 2001/03/08 20:34:16 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Network Global Definitions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2001 NexGen Software.
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
 * 21/07/2000 - Regis Feneon
 *****************************************************************************/

#ifndef __NG_NGNET_H_INCLUDED__
#define __NG_NGNET_H_INCLUDED__

#include <ngos.h>

#include <ngos/queue.h>
#include <ngos/netbuf.h>
#include <ngos/netif.h>
#include <ngos/netprot.h>
#include <ngos/netsock.h>
#include <ngos/netglob.h>
#include <ngip/if.h>
#include <ngip/buf.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* loopback driver */
extern const NGnetdrv ngNetDrv_LOOPBACK;

/* network buffer management */
NGbuf *ngBufCopy( NGbuf *bufp);

/* NG_END_DECLS // confuses Source Insight */

#endif

