/*****************************************************************************
 * $Id: sio_bind.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOBind()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from bind()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOBind()
 *****************************************************************************
 * Bind an address to a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  addr    address to bind
 * Return value: NG_EOK or negative error code
 */

int ngSAIOBind( NGsock *so, NGsockaddr *addr)
{
  /* call protocol' bind routine */
  return( NG_SOCKCALL_USER( so, NG_PRU_BIND, NULL, 0, addr));
}

