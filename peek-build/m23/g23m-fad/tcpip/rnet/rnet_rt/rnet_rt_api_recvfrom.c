/**
 * @file    rnet_rt_api_recvfrom.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_recvfrom.c,v 1.2 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  6/24/2002   Regis Feneon  Create
 *
 */
#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"


/**
 * Read the waiting data (not-blocking).
 *
 * Data reception is message based: When the application receives a
 * T_RNET_RECV_IND message, it can read the data with this
 * rnet_recv() function.
 *
 * The data are copied in the buffer given by the application to RNET
 * via this rnet_recv function.
 *
 * Datagram oriented (UDP)
 * -----------------------
 * When an UDP datagram arrives, the receiver application receives
 * a T_RNET_RECV_IND message. The application can then read the
 * datagram with the rnet_recv function. The buffer passed to the
 * function should be big enough to contain a UDP datagram, otherwise
 * the message is not put in the buffer and the error code
 * RNET_MSG_SIZE is returned.
 *
 * The OUT value of the len_p parameter can have two possible values:
 * - If there was data to read and no error detected (function
 *   returned RNET_OK), len_p contains the size of the received datagram.
 * - If there was no error, but no data to read, len_p contains 0.
 *
 * RNET will only send a new T_RNET_RECV_IND with a specific descriptor
 * when a rnet_recv function has been called that returned a len_p parameter
 * with a 0 value. Therefore, the application should loop on rnet_recv when
 * it receives a T_RNET_RECV_IND message, like:
 *    UINT16 *len_p = -1;
 *    while (*len_p != 0) {
 *      ret = rnet_recv(desc, buff, len_p);
 *      ...
 *    }
 *
 * Stream oriented (TCP)
 * ---------------------
 * When a new stream of data arrives on a connection, the receiver
 * application receives a T_RNET_RECV_IND. It can then read the flow
 * with the rnet_recv function, until the len_p parameter is returned
 * with a 0 value.
 *
 * The stack will only send a new T_RNET_RECV_IND message when
 * rnet_recv has been called and has returned the len_p parameter with a 0.
 *
 *
 * @param  desc  Connection identifier [IN].
 * @param  buff  Buffer to store the received data [OUT].
 * @param  len_p  Pointer on the length of the passed buffer [IN]
 *          Pointer on the size of the received data in the buffer [OUT].
 * @return  RNET_MEMORY_ERR      Not enough memory is available
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *                or  The ID is not connected.
 *                or  Invalid buff parameter.
 *                or  Connection not bound with bind.
 *      RNET_CONN_ABORTED    Connection broken due to the "keep-alive" activity
 *                  detecting a failure while the operation was in progress.
 *                or  Virtual circuit terminated due to a time-out or other failure.
 *      RNET_MSG_SIZE      The socket is message oriented (UDP), and the message
 *                  was too large to fit into the specified buffer and was truncated.
 *      RNET_CONN_RESET      The virtual circuit was reset by the remote side executing a "hard"
 *                  or "abortive" close.
 *      RNET_TIMEOUT      The connection has been dropped, because of a
 *                  network failure or because the system on the other end went down
 *                  without notice.
 *      RNET_NET_UNREACHABLE  Remote host cannot be reached from this host at this time.
 *      RNET_OK          Data successfully read.
 */

T_RNET_RET rnet_rt_recv_from (T_RNET_DESC * desc,
               T_RVF_BUFFER * buff,
               UINT16 * len_p,
               T_RNET_IP_ADDR * from_addr,
               T_RNET_PORT * from_port)
{
  int ret, rflags;
  NGsockaddr faddr;

  rflags = 0;
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  ret = ngSAIORecv( (NGsock *) desc, buff, *len_p, &rflags, &faddr, NULL);
  if( ret == NG_EWOULDBLOCK) {
    /* no more data to read, will send T_RNET_RECV_IND message */
    /* when new data will be received */
    ((T_RNET_RT_SOCK *) desc)->flags |= RNET_RT_SOCKF_NOTIFY_RECV;
    ret = 0;
  }
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( ret >= 0) {
    /* return nb of read bytes */
    *len_p = ret;
    /* return source address and port number */
    *from_addr = ngNTOHL( faddr.sin_addr);
    *from_port = ngNTOHS( faddr.sin_port);
    /* check if message was truncated */
    return( (rflags & NG_IO_TRUNC) ? RNET_MSG_SIZE : RNET_OK);
  }

  return( rnet_rt_ngip_error( ret));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

