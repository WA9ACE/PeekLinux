/**
 * @file    rnet_rt_api_getlocaladdr.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_getlocaladdr.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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
 * Gets the local address and port of a connection ID.
 *
 * @param  desc      Connection identifier [IN].
 * @param  local_addr_p  Local IP address [OUT].
 * @param  local_port_p  Local port [OUT].
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *                or  Not bound to an address with bind.
 *                or  ADDR_ANY is specified in bind but connection not yet occurred.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Local address/port successfully get.
 */

T_RNET_RET rnet_rt_get_local_addr_port (T_RNET_DESC * desc,
                   T_RNET_IP_ADDR * local_addr_p,
                   T_RNET_PORT * local_port_p)
{
  int err, len;
  NGsockaddr addr;

  len = sizeof( addr);

  /* get socket name */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOGetOption( (NGsock *) desc, NG_IOCTL_SOCKET, NG_SO_SOCKNAME,
    &addr, &len);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( err == NG_EOK) {
    *local_addr_p = ngNTOHL( addr.sin_addr);
    *local_port_p = ngNTOHS( addr.sin_port);
    return( RNET_OK);
  }

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

