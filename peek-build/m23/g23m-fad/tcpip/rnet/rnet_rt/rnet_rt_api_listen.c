/**
 * @file    rnet_rt_api_listen.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_listen.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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
 * Commands a connection to start listening for incoming connections.
 * When an incoming connection is accepted, an RNET_ACCEPTED message is sent.
 * The connection ID will have to be bound to a local port
 * with the rnet_bind() function.
 *
 * Note that there is no accept function: After a call to listen,
 * the application can receive RNET_ACCEPTED messages.
 *
 * @param  desc  Connection identifier [IN].
 * @return  RNET_MEMORY_ERR      No available memory for the listening connection.
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *                or  No more socket descriptors available.
 *      RNET_INVALID_PARAMETER  Invalid connection ID.
 *                or  Connection not bound with bind.
 *                or  The ID does not support listening operation.
 *      RNET_IN_USE        Connection already connected.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Listening successfully started.
 */

T_RNET_RET rnet_rt_listen (T_RNET_DESC *desc)
{
  int err;

  /* listen for connections */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOListen( (NGsock *) desc, RNET_RT_SOCK_LISTEN_MAX);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

