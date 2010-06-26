/**
 * @file    rnet_rt_api_send.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_send.c,v 1.4 2002/10/30 15:23:34 rf Exp $
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
 * Enqueues the data for sending.
 *
 * Data is sent by enqueueing the data with a call to rnet_send().
 * The function is not-blocking.
 *
 * In case of TCP, when the data is successfully transmitted to the remote
 * host, the application will be notified with a RNET_SEND_CFM message.
 *
 * Returns RNET_PARTIAL_SENT if not all data could be sent and the function
 * needs to be called again later to send the rest of the data. In that case,
 * the out value of len_p is the number of bytes effectively sent.
 *
 * The buffer is copied by RNET, so the application can free the data buffer
 * when the function returns.
 *
 * TODO: The proper way to use this function.
 *
 * @param  desc  Connection identifier [IN].
 * @param  buff  Pointer on the data to send [IN].
 * @param  len_p  Pointer on the length of the data to send [IN].
 *          Pointer on the length of the data effectively sent [OUT].
 * @return  RNET_MEMORY_ERR      Not enough memory is available
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *                or  Requested address is a broadcast address.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *                or  The ID is not connected.
 *                or  Invalid buff parameter.
 *                or  Connection not bound with bind.
 *      RNET_CONN_ABORTED    Connection broken due to the "keep-alive" activity
 *                  detecting a failure while the operation was in progress.
 *                or  Virtual circuit terminated due to a time-out or other failure.
 *      RNET_MSG_SIZE      Message oriented connection (UDP), and the message
 *                  is larger than the maximum supported by the underlying transport.
 *      RNET_NET_UNREACHABLE  Remote host cannot be reached from this host at this time.
 *      RNET_CONN_RESET      The virtual circuit was reset by the remote side executing a "hard"
 *                  or "abortive" close.
 *      RNET_TIMEOUT      The connection has been dropped, because of a
 *                  network failure or because the system on the other end went down
 *                  without notice.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Sending in progress
 *                  RNET_SEND_CFM will be sent when the sending
 *                  has been successfully completed.
 */

T_RNET_RET rnet_rt_send (T_RNET_DESC * desc,
            T_RVF_BUFFER * buff,
            UINT16 * len_p)
{
  int ret;

  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  ret = ngSAIOSend( (NGsock *) desc, buff, *len_p, 0, NULL);
  if( (ret == NG_EWOULDBLOCK) || ((ret > 0) && (ret != *len_p))) {
    /* send RNET_SEND_RDY message when data will be acknowledged */
    ((T_RNET_RT_SOCK *) desc)->flags |= RNET_RT_SOCKF_NOTIFY_SEND;
  }
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( ret == NG_EWOULDBLOCK) {
    ret = 0;
  }
  if( ret >= 0) {
    if( ret != *len_p) {
      /* not all data has been sent */
      *len_p = ret;
      return( RNET_PARTIAL_SENT);
    }
    return( RNET_OK);
  }

  return( rnet_rt_ngip_error( ret));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

