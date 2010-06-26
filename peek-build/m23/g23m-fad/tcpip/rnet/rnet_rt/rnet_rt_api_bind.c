/**
 * @file    rnet_rt_api_bind.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_bind.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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
 * Binds the connection to a local IP address and port number.
 *
 * @param  desc  Connection identifier [IN].
 * @param  local_addr  Local IP address
 *          The IP address can be specified as RNET_IP_ADDR_ANY
 *          in order to bind the connection to all local IP addresses [IN].
 * @param  local_port  If the port is not specified, the allocation of a port
 *          is done automatically by the system [IN].
 * @return  RNET_IN_USE        Another connection bound to the same address/port.
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  Invalid connection ID.
 *                or  Specified address not a valid address
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_MEMORY_ERR      Not enough buffers available, too many connections.
 *      RNET_OK          Connection successfully bound.
 */

T_RNET_RET rnet_rt_bind (T_RNET_DESC * desc,
            T_RNET_IP_ADDR local_addr,
            T_RNET_PORT local_port)
{
  NGsockaddr addr;
  int err;

  /* convert address to network-byte order */
  addr.sin_port = ngHTONS( local_port);
  addr.sin_addr = ngHTONL( local_addr);

  /* bind local address & port */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOBind( (NGsock *) desc, &addr);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

