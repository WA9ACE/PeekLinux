/*****************************************************************************
 * $Id: loopback.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Loopback Interface
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
 * ngNetDrv_LOOPBACK
 * drvOutput()
 * drvOpen()
 * drvClose()
 *----------------------------------------------------------------------------
 * 08/02/99 - Regis Feneon
 * 30/06/99 -
 *  added mtu and bps in NGnetdrv structure, init function suppressed,
 *  moved file to src directory
 * 29/11/2000 -
 *  drvInit() - initialization of address and mtu
 *  drvOutput() - now call ngIpLoopback()
 *****************************************************************************/

#include <ngip.h>

static int drvInit( NGifnet *netp)
{
  /* initialize default fields */
  netp->if_addr = NG_INADDR( 127, 0, 0, 1);
  netp->if_netmask = NG_INADDR( 255, 0, 0, 0);
  netp->if_subnetmask = NG_INADDR( 255, 0, 0, 0);
  netp->if_net = NG_INADDR( 127, 0, 0, 0);
  netp->if_netbroadcast = NG_INADDR( 127, 255, 255, 255);
  netp->if_subnet = NG_INADDR( 127, 0, 0, 0);
  netp->if_broadaddr = NG_INADDR( 127, 255, 255, 255);

  /* set max buffer size */
  netp->if_mtu = ngBufDataMax;

  return( NG_EOK);
}

static int drvOutput( NGifnet *netp, NGbuf *bufp, NGuint in_addr)
{

    /* no warning */
    ((void)(in_addr));

    /* loopback the message */
    ngIpLoopback( netp, bufp);

    return( NG_EOK);
}

static int drvOpen( NGifnet *netp)
{
    /* interface already running ? */
    if( netp->if_flags & NG_IFF_RUNNING) {
        return( NG_EALREADY);
    }
    /* interface is running */
    netp->if_flags |= NG_IFF_RUNNING|NG_IFF_UP;

    return( NG_EOK);
}

static int drvClose( NGifnet *netp)
{
    /* interface should be running to be closed */
    if( !(netp->if_flags & NG_IFF_RUNNING)) {
        return( NG_EALREADY);
    }
    /* clear flags */
    netp->if_flags &= ~(NG_IFF_RUNNING|NG_IFF_UP);

    return( NG_EOK);
}

/********************************************************************/

const NGnetdrv ngNetDrv_LOOPBACK = {
  "LOOPBACK",
  NG_IFT_LOOP,
  NG_IFF_LOOPBACK|NG_IFF_BROADCAST,
  0,
  0UL,
  drvInit,
  drvOpen,
  drvClose,
  drvOutput,
  NULL,
  ngIfGenCntl
};

