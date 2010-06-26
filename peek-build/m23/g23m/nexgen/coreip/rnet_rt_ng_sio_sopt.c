/*****************************************************************************
 * $Id: sio_sopt.c,v 1.2 2001/02/21 16:02:38 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1
 * Asynchronous Sockets
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
 * ngSAIOSetOption()
 *----------------------------------------------------------------------------
 * 15/02/2001 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOSetOption()
 *****************************************************************************
 * Set a socket option.
 * socket level and ip level are processed in the routine, the other levels
 * options are passed to the protocol user option routine.
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  level   NG_IOCTL_SOCKET, NG_IOCTL_IP
 *  option  option name
 *  optval  value
 *  optlen  size of optval
 * Return value: NG_EOK or negative error code
 */

int ngSAIOSetOption( NGsock *so, int level, int option, void *optval,
                     int optlen)
{
  return( ngSAIOOption( so, level, option|NG_SO_SET, optval, &optlen));
}

