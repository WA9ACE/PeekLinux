/**
 * @file    rnet_rt_api_getmaxpacketsize.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_getmaxpacketsize.c,v 1.3 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  7/23/2002   Regis Feneon  Create
 *
 */
#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

/**
 * Gets the maximum send size of a message.
 *
 * @param  desc      Connection identifier [IN].
 * @param  size_p    Maximum send size [OUT].
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Local address/port successfully get.
 */

T_RNET_RET rnet_rt_get_max_packet_size(T_RNET_DESC * desc,
                   UINT32 * size_p)
{
  int err, len, maxsize;

  len = sizeof( maxsize);

  /* get socket buffer size */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOGetOption( (NGsock *) desc, NG_IOCTL_SOCKET, NG_SO_SNDBUF,
    &maxsize, &len);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( err == NG_EOK) {
    *size_p = maxsize;
    return( RNET_OK);
  }

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

