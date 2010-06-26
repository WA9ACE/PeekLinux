/**
 * @file    rnet_rt_api_getbufsize.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_getbufsize.c,v 1.3 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/22/2002   Regis Feneon  Create
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

/**
 * Use to determine the amount of data pending in the network's input buffer
 * that can be read from the connection ID.
 *
 * If desc is stream oriented (TCP), returns the amount of data that can be read
 * in a single call to the rnet_recv function; this might not be the same as the
 * total amount of data queued on the socket.
 * If desc is message oriented (UDP), returns the size of the first datagram
 * (message) queued on the socket.
 *
 * Use the socket option SO_NREAD.
 *
 * @param  desc    Connection identifier [IN].
 * @param  size_p    Pointer to an unsigned int in which the result is stored [OUT].
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Value successfully get.
 */

T_RNET_RET rnet_rt_get_buff_size (T_RNET_DESC * desc,
                 UINT32 * size_p)
{
  int err, len, nread;

  len = sizeof( nread);

  /* get number of bytes in receive buffer */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOGetOption( (NGsock *) desc, NG_IOCTL_SOCKET, NG_SO_NREAD,
    &nread, &len);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( err == NG_EOK) {
    *size_p = nread;
    return( RNET_OK);
  }

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

