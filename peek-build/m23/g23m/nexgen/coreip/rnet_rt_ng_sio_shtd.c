/*****************************************************************************
 * $Id: sio_shtd.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOShutdown()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from shutdown()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOShutdown()
 *****************************************************************************
 * Shutdown part of a connection
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  how     how to shutdown:
 *              0 = read
 *              1 = write
 *              2 = read & write
 * Return value: NG_EOK or negative error code
 */

int ngSAIOShutdown( NGsock *so, int how)
{

  /* how to shutdown ? */

  /* shutdown read-half of the connection */
  if( (how == 0) || (how == 2)) {
    /* clear input buffer */
    ngSoCantRcvMore( so);
    NG_SOCK_RCVBUF_FLUSH( so);
  }

  /* shutdown write-half of the connection */
  if( (how == 1) || (how == 2)) {
    /* call protocol shutdown function */
    return( NG_SOCKCALL_USER( so, NG_PRU_SHUTDOWN, NULL, 0, NULL));
  }

  return( NG_EOK);
}

