/**
 * @file  rnet_rt_api.h
 *
 * API Definition for RNET_RT SWE.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api.h,v 1.5 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  4/3/2002  Regis Feneon      ngip api
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"

#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_API_H_
#define __RNET_RT_API_H_

#include "rvm_gen.h" /* Generic RVM types and functions. */
#include "rnet_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name RNET_RT Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/

#define RNET_RT_OK RV_OK
#define RNET_RT_NOT_SUPPORTED RV_NOT_SUPPORTED
#define RNET_RT_MEMORY_ERR RV_MEMORY_ERR
#define RNET_RT_INTERNAL_ERR RV_INTERNAL_ERR
/*@}*/

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/**
 * Creates a new connection identifier (T_RNET_DESC).
 *
 * The connection ID is not active until it has either been bound
 * to a local address or connected to a remote address.
 *
 * @param  proto  Protocol that should be used [IN].
 * @param  desc  Connection identifier created [OUT].
 * @param  return_path  Return path that should be used to send
 *          the messages like accept, connect, etc [IN].
 * @return  RNET_MEMORY_ERR      No available memory to create the new connection id.
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *                or  No more socket descriptors available.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_NOT_SUPPORTED    Specified protocol not supported.
 *      RNET_OK          Connection ID successfully created.
 */
T_RNET_RET rnet_rt_new ( T_RNET_IPPROTO proto,
             T_RNET_DESC ** desc,
               T_RV_RETURN_PATH return_path);

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
               T_RNET_PORT local_port);

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
T_RNET_RET rnet_rt_listen (T_RNET_DESC *desc);

/**
 * Sets up the connection ID to connect to the remote host and sends the
 * initial SYN segment which opens the connection.
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
                T_RNET_PORT peer_port);

/**
 * Enqueues the data for sending.
 *
 * Data is sent by enqueueing the data with a call to rnet_send().
 * The function is not-blocking.
 *
 * Returns RNET_PARTIAL_SENT if not all data could be sent and the function
 * needs to be called again later to send the rest of the data. In that case,
 * the out value of len_p is the number of bytes effectively sent.
 * The remaining data can be sent when the message RNET_SEND_RDY is received.
 *
 * The buffer is copied by RNET, so the application can free the data buffer
 * when the function returns.
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
 */
T_RNET_RET rnet_rt_send (T_RNET_DESC * desc,
               T_RVF_BUFFER * buff,
               UINT16 * len_p);

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
T_RNET_RET rnet_rt_recv (T_RNET_DESC * desc,
                T_RVF_BUFFER *buff,
               UINT16 * len_p);

T_RNET_RET rnet_rt_recv_from (T_RNET_DESC * desc,
               T_RVF_BUFFER * buff,
               UINT16 * len_p,
               T_RNET_IP_ADDR * from_addr,
               T_RNET_PORT * from_port);

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
T_RNET_RET rnet_rt_shutdown (T_RNET_DESC * desc);

/**
 * Closes the connection.
 *
 * Use it to release the connection ID so further references to it will fail with
 * the error RNET_INVALID_PARAMETER.
 *
 * An application should always have a matching call to rnet_close for each successful
 * call to rnet_new to return any resources to the system.
 *
 * The function may return RNET_MEMORY_ERR if no memory
 * was available for closing the connection. If so, the application
 * should wait and try again either by using the error message
 * or the polling functionality.
 * If the close succeeds, the function returns RNET_OK.
 *
 * The connection ID is deallocated after a call to rnet_close().
 *
 * @param  desc  Connection identifier.
 * @return  RNET_MEMORY_ERR      Not enough memory is available
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Socket closed.
 */
T_RNET_RET rnet_rt_close (T_RNET_DESC * desc);

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
                      T_RNET_PORT * local_port_p);

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
 * @param  desc    Connection identifier [IN].
 * @param  size_p    Pointer to an unsigned int in which the result is stored [OUT].
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Value successfully get.
 */
T_RNET_RET rnet_rt_get_buff_size (T_RNET_DESC * desc,
                    UINT32 * size_p);

/**
 * Requests host information corresponding to a host name or to a
 * network address.
 *
 * One of the two parameters name or addr must be NULL.
 *
 * This function is bridge function sending a corresponding message to RNET.
 * The message RNET_HOST_INFO is sent back when the requested information
 * is available.
 *
 * Note that this function does not need a connection identifier and specifies
 * its own return path.
 *
 * @param  name      Name of the host to resolve [IN].
 * @param  addr      Network address [IN].
 * @param  return_path    Return path for sending the response.
 * @param  user_data    Pointer on some user-defined data that
 *              will be contained in the RNET_HOST_INFO message [IN].
 * @return  RNET_MEMORY_ERR  Not enough memory is available.
 *      RNET_OK      The message could be correctly send.
 */
T_RNET_RET rnet_rt_get_host_info (char *name,
                    T_RNET_IP_ADDR addr,
                    T_RV_RETURN_PATH return_path,
                  void * user_data);

/*
 * Following functions are not directly part of the networking API,
 * but they are used to retrieve some simple information from a connection
 * descriptor. They are simple blocking functions, not sending any messages.
 */

/**
 * Retrieves the protocol associated to a connection descriptor.
 *
 * @param  desc  Connection identifier.
 * @return  A value of the T_RNET_IPPROTO enumeration.
 */
T_RNET_IPPROTO rnet_rt_get_proto (T_RNET_DESC *desc);

/**
 * Associates an application specific pointer to a connection ID.
 *
 * @param  desc    Connection identifier.
 * @param  user_data  Pointer that can be used by an application to store
 *            application specific data.
 */
void rnet_rt_set_user_data (T_RNET_DESC *desc, void *user_data);

/**
 * Returns the application specific pointer associated to the connection ID.
 *
 * @param  desc  Connection identifier.
 * @return  Application specific data.
 */
void * rnet_rt_get_user_data (T_RNET_DESC *desc);
/*@}*/

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

T_RNET_RET rnet_rt_get_max_packet_size (T_RNET_DESC *desc,
                   UINT32 *size_p);

/**
 * NexGenIP specific API
 */

T_RNET_RET rnet_rt_route_add( T_RNET_IP_ADDR dest, T_RNET_IP_ADDR netmask, T_RNET_IP_ADDR gateway);
T_RNET_RET rnet_rt_route_del( T_RNET_IP_ADDR dest);
T_RNET_RET rnet_rt_route_default( T_RNET_IP_ADDR gateway);

T_RNET_RET rnet_rt_if_setaddr( const char *ifname, T_RNET_IP_ADDR addr, T_RNET_IP_ADDR netmask);
T_RNET_RET rnet_rt_if_getaddr( const char *ifname, T_RNET_IP_ADDR *addr, T_RNET_IP_ADDR *netmask);
T_RNET_RET rnet_rt_if_setdstaddr( const char *ifname, T_RNET_IP_ADDR dstaddr);
T_RNET_RET rnet_rt_if_getdstaddr( const char *ifname, T_RNET_IP_ADDR *dstaddr);

T_RNET_RET rnet_rt_set_resolver( const char *domain, T_RNET_IP_ADDR dns1, T_RNET_IP_ADDR dns2);

#ifdef __cplusplus
}
#endif

#endif /*__RNET_RT_API_H_*/

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

