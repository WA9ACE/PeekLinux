/**
 * @file    rnet_rt_api_shutdown.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_shutdown.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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
 * Disables the sending on a socket and informs the peer
 * about it.
 *
 * Subsequent calls to the send function are disallowed.
 * For TCP sockets, a FIN will be sent after all data is sent and acknowledged by the receiver.
 * The rnet_shutdown function does not frees the connection ID. Any resources attached
 * to the ID will not be freed until rnet_close is invoked.
 *
 * To assure that all data is sent and received on a connection (TCP) before it
 * is closed, an application should use rnet_shutdown to close connection before calling
 * rnet_close. For example, to initiate a graceful disconnect:
 * 1) Call rnet_shutdown.
 * 2) When RNET_ERROR_IND with RNET_CONN_CLOSED is received, call rnet_recv until
 *    the len parameter is zero.
 * 3) Call rnet_close.
 *
 * Following technique describes how to minimize the chance of problems
 * occurring during connection teardown. In this example, the client is responsible for
 * initiating a graceful shutdown.
 *
 * Client Side              Server Side
 * (1) Invoke rnet_shutdown to
 *  signal end of session and that
 *  client has no more data to send.
 *                    (2) Receive RNET_ERROR_IND with RNET_CONN_CLOSED,
 *                     indicating graceful shutdown in progress
 *                     and that all data has been received.
 *                    (3) Send any remaining response data with rnet_send.
 * (5') Get RNET_RECV_IND and       (4) Invoke rnet_shutdown to
 *  invoke rnet_recv to get any       indicate server has no more data to send.
 *  response data sent by server.
 * (5) Receive RNET_ERROR_IND       (4') Invoke rnet_close.
 *  with RNET_CONN_CLOSED.
 * (6) Invoke rnet_close.
 *
 * The timing sequence is maintained from step (1) to step (6) between the client and
 * the server, except for step (4') and (5') which only has local timing significance in
 * the sense that step (5) follows step (5') on the client side while step (4') follows
 * step (4) on the server side, with no timing relationship with the remote party.
 *
 * @param  desc  Connection identifier.
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *                  The descriptor is not connected (TCP only).
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Connection shutdown, sending impossible
 *                  RNET_ERROR_IND with the parameter RNET_CONN_CLOSED
 *                  will be sent to the peer.
 */

T_RNET_RET rnet_rt_shutdown (T_RNET_DESC * desc)
{
  int err;

  /* stop sending data */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOShutdown( (NGsock *) desc, 1);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

