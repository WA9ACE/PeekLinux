/**
 * @file    rnet_rt_api_connect.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_connect.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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

#include "rnet_trace_i.h"

/**
 * Sets up the connection ID to connect to the remote host.
 *
 * For the connection-oriented client (TCP), it prepares a connection from the local
 * to the peer system. The connection oriented client does not need
 * to call rnet_bind() before rnet_connect(), since rnet_connect() would automatically
 * use the local address of the client and allocate dynamically a free local port.
 *
 * rnet_connect() returns immediately, does not wait for the connection to be properly setup.
 * The RNET_CONNECT_CFM message is sent when the connection is established.
 * If the connection could not be properly established, a RNET_ERROR_IND
 * message is sent.
 *
 * A connectionless client (UDP) would use rnet_connect() to locally specify
 * the remote server, but no connection is open. But the remote address
 * is then known and does not need to be specified again.
 * rnet_connect() takes as parameter a fully initialized address structure,
 * specifying server address and port.
 *
 * @param  desc    Connection identifier [IN].
 * @param  peer_addr  Peer address [IN].
 * @param  peer_port  Peer port [IN].
 * @return  RNET_MEMORY_ERR      No buffer space available.
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *                or  Attempt to connect forcefully rejected.
 *                or  Attempt to connect datagram socket to broadcast address
 *      RNET_INVALID_PARAMETER  Invalid connection ID.
 *                or  Remote address not a valid address (such as ADDR_ANY).
 *      RNET_NOT_SUPPORTED    Addresses in the specified family cannot be used with this socket.
 *      RNET_IN_USE        Already connected (connection-oriented only).
 *      RNET_NET_UNREACHABLE  The network cannot be reached from this host at this time.
 *      RNET_TIMEOUT      Attempt to connect timed out without establishing a connection.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Connection in progress
 *                  RNET_CONNECT_CFM will be sent when the connection
 *                  has been successfully completed.
 */

T_RNET_RET rnet_rt_connect (T_RNET_DESC * desc,
             T_RNET_IP_ADDR peer_addr,
             T_RNET_PORT peer_port)
{
  NGsockaddr addr;
  int err;

  addr.sin_port = ngHTONS( peer_port);
  addr.sin_addr = ngHTONL( peer_addr);

  /* connect to peer */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOConnect( (NGsock *) desc, &addr, 0);

  if( err == NG_EINPROGRESS) {

    /* send msg when connection completed */
    ((T_RNET_RT_SOCK *) desc)->flags |= RNET_RT_SOCKF_NOTIFY_CONNECT;
    err = NG_EOK;
  }

  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

