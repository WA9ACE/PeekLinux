/**
 * @file  rnet_api.h
 *
 * Riviera NET.
 *
 * Declarations of the Riviera TCP/IP stack asynchronous
 * (non-blocking) API.
 * See the Readme.txt file.
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date         Author        Modification
 *  --------------------------------------------------
 *  01/25/2002  Vincent Oberle    Create
 *  03/14/2002  Vincent Oberle    Added rnet_shutdown
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_API_H_
#define __RNET_API_H_

/*
 * IMPORTANT NOTE FOR WINDOWS 2000 USERS
 *
 * It has been observed that the Winsocket version of the RNET implementation
 * does not function correctly under Windows 2000. Connection to a host on the
 * Internet does not work.
 * This problem does not appear under NT4 and XP.
 */

#include "rv_general.h"
#include "rvf_api.h"

#include "rnet_cfg.h"
#include "rnet_ip_addr.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************
 * Types and constants *
 ***********************/

/**
 * Protocols supported.
 *
 * Even if the interface has some similarities to the socket API,
 * only the Internet protocols TCP and UDP are supported.
 *
 * RNET_IPPROTO_UNDEFINED can never be passed to a function,
 * but only returned by rnet_get_proto when the stack gets corrupted
 * or any other problem.
 */
typedef enum {
  RNET_IPPROTO_UNDEFINED = 0,
  RNET_IPPROTO_TCP = 6,
  RNET_IPPROTO_UDP = 17
  /* RNET_IPPROTO_ICMP = 1 not supported (yet?) */
} T_RNET_IPPROTO;

/**
 * Traffic class.
 *
 * @todo  Values to be defined.
 */
typedef enum {
  RNET_TRAFFIC_CLASS_DEFAULT,
  RNET_TRAFFIC_CLASS_WAP,
  RNET_TRAFFIC_CLASS_FTP,
  /* or */
  RNET_TRAFFIC_CLASS_LOWDELAY,
  RNET_TRAFFIC_CLASS_BACKGROUND,
  RNET_TRAFFIC_CLASS_BEST_EFFORT,
  RNET_TRAFFIC_CLASS_NETWORK
} T_RNET_TRAFFIC_CLASS;

/**
 * Connection identifier.
 *
 * This structure is equivalent to the socket descriptor.
 * The content of this structure is specific to the TCP/IP implementation
 * and not seen by the user of RNET.
 * In no case should the application developer manipulate directly the content
 * of the structure. He/she only sees a pointer on it.
 */
#ifdef _WINDOWS
  #if defined RNET_CFG_WINSOCK
     typedef struct T_RNET_WS_DESC T_RNET_DESC;
  #elif defined RNET_CFG_REAL_TRANSPORT
     typedef struct T_RNET_RT_SOCK T_RNET_DESC;
  #endif
#else
   #if defined RNET_CFG_BRIDGE
      typedef struct T_RNET_BR_DESC T_RNET_DESC;
   #elif defined RNET_CFG_REAL_TRANSPORT
      typedef struct T_RNET_RT_SOCK T_RNET_DESC;
   #endif
#endif

/**
 * Net package return values and error codes. They are both used for
 * API function return values and in the RNET_ERROR_IND error message.
 *
 * Please note that some are specific to RNET and are not found
 * among the standard Riviera return code.
 */
typedef enum {
  /*
   * Standard RV return values
   ****************************/

  RNET_OK = RV_OK,

  RNET_MEMORY_ERR = RV_MEMORY_ERR,

  RNET_INVALID_PARAMETER = RV_INVALID_PARAMETER,

  /**
   * The protocol type or the specified protocol is not
   * supported by the system.
   * The specified feature is not implemented.
   */
  RNET_NOT_SUPPORTED = RV_NOT_SUPPORTED,

  RNET_NOT_READY = RV_NOT_READY,

  RNET_INTERNAL_ERR = RV_INTERNAL_ERR,

  /*
   * RNET specific return values
   ******************************/

  /**
   * Another connection is bound to the same port,
   * address or socket.
   */
  RNET_IN_USE = -50,

  /**
   * The protocol stack wasn't initialised.
   * See rnet_ws_startup().
   */
  RNET_NOT_INITIALIZED = -51,

  /**
   * The network cannot be reached from this host at this time.
   */
  RNET_NET_UNREACHABLE = -52,

  /**
   * Attempt to connect timed out without establishing a connection.
   */
  RNET_TIMEOUT = -53,

  /**
   * The attempt to connect was forcefully rejected.
   */
  RNET_CONN_REFUSED = -54,

  /**
   * The connection was reset by the remote side.
   */
  RNET_CONN_RESET = -55,

  /**
   * The connection was terminated due to a time-out or other failure.
   */
  RNET_CONN_ABORTED = -56,

  /**
   * The connection was gracefully closed.
   */
  RNET_CONN_CLOSED = -57,

  /**
   * The connection is message oriented (UDP), and the message is larger than the
   * maximum supported by the underlying transport.
   */
  RNET_MSG_SIZE = -58,

  /**
   * Not all bytes have been sent in a send operations.
   */
  RNET_PARTIAL_SENT = -59,

  /**
   * For host search.
   * Indicates that a host was not found.
   */
  RNET_HOST_NOT_FOUND = -60

} T_RNET_RET;

/*************
 * Functions *
 *************/

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
T_RNET_RET rnet_new (T_RNET_IPPROTO proto,
           T_RNET_DESC ** desc,
           T_RV_RETURN_PATH return_path);

/**
 * Sets the traffic class of a connection ID.
 *
 * Note that this function is NOT implemented under Windows.
 *
 * @param  desc  Connection identifier [IN].
 * @param  traffic_class  Traffic class, see T_RNET_TRAFFIC_CLASS
 *          for more details.
 * @return  RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  Invalid connection ID.
 *                or  Specified traffic class not valid.
 *      RNET_OK          Traffic class successfully set.
 */
T_RNET_RET rnet_set_traffic_class (T_RNET_DESC * desc,
                   T_RNET_TRAFFIC_CLASS traffic_class);

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
T_RNET_RET rnet_bind (T_RNET_DESC * desc,
            T_RNET_IP_ADDR local_addr,
            T_RNET_PORT local_port);

/**
 * Commands a connection to start listening for incoming connections.
 * When an incoming connection is accepted, an RNET_CONNECT_IND message is sent.
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
T_RNET_RET rnet_listen (T_RNET_DESC *desc);

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
T_RNET_RET rnet_connect (T_RNET_DESC * desc,
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
 * If no RNET_PARTIAL_SENT is returned, but RNET_OK, more data can be
 * immediately sent. No RNET_SEND_RDY is sent to the SWE.
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
 *      RNET_OK          Sending in progress.
 */
T_RNET_RET rnet_send (T_RNET_DESC * desc,
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
T_RNET_RET rnet_recv (T_RNET_DESC * desc,
            T_RVF_BUFFER *buff,
            UINT16 * len_p);

/**
 * Read the waiting data (not-blocking). Similar to rnet_recv,
 * BUT FOR UDP ONLY.
 *
 * See rnet_recv.
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
 *                or  The ID is connected.
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
T_RNET_RET rnet_recv_from (T_RNET_DESC * desc,
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
T_RNET_RET rnet_shutdown (T_RNET_DESC * desc);

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
 * The connection ID is deallocated by a call to rnet_close().
 *
 * IMPORTANT: If an application wants to call rnet_close after having received
 * a close event (RNET_ERROR_IND message with error parameter set to RNET_CONN_CLOSED),
 * it should check that there are no remaining data to be read (data arrived after
 * the RNET_ERROR_IND message, so that no RNET_RECV_IND was received yet).
 * A way to do it is:
 *    T_RVF_BUFFER *  buff;
 *    UINT16 len;
 *
 *    RNET_TRACE_LOW("Read remaining data");
 *    len = 100;
 *    while (len > 0) {
 *      rvf_get_buf(rtest_get_mb_id(), len, (T_RVF_BUFFER**)&buff);
 *      rnet_recv(error_ind_msg_p->desc, buff, (UINT16*)&len);
 *      rvf_send_trace(buff, len, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, RTEST_USE_ID);
 *      rvf_free_buf(buff);
 *    }
 *
 * @param  desc  Connection identifier.
 * @return  RNET_MEMORY_ERR      Not enough memory is available
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *      RNET_INVALID_PARAMETER  The connection ID is invalid.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_OK          Socket closed.
 */
T_RNET_RET rnet_close (T_RNET_DESC * desc);

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
T_RNET_RET rnet_get_local_addr_port (T_RNET_DESC * desc,
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
T_RNET_RET rnet_get_buff_size (T_RNET_DESC * desc,
                 UINT32 * size_p);

/**
 * Indicates the maximum send size of a message for message-oriented
 * descriptor (UDP) as implemented by a particular service provider.
 * It has no meaning for TCP.
 *
 * @param  desc    Connection identifier [IN].
 * @param  size_p    Pointer to an unsigned int in which the result is stored [OUT].
 */
T_RNET_RET rnet_get_max_packet_size (T_RNET_DESC * desc,
                   UINT32 * size_p);

/**
 * Requests host information corresponding to a host name or to a
 * network address.
 *
 * One of the two parameters name or addr must be NULL.
 *
 * This function is bridge function sending a corresponding message to RNET.
 * The message RNET_HOST_INFO is sent back when the requested information
 * is available or if the request failed:
 * If RNET_HOST_INFO "error" parameter is RNET_OK, no error occured and
 * the host_name, host_addr and user_data parameters are valid.
 * If RNET_HOST_INFO "error" parameter is different from RNET_OK, the host
 * information could not be retrieved and only user_data is valid.
 *
 * Note that this function does not need a connection identifier and specifies
 * its own return path.
 *
 * @param  name      Name of the host to resolve [IN].
 * @param  addr      Network address [IN].
 * @param  return_path    Return path for sending the response [IN].
 * @param  user_data    Pointer on some user-defined data that
 *              will be contained in the RNET_HOST_INFO message [IN].
 * @return  RNET_MEMORY_ERR  Not enough memory is available.
 *      RNET_OK      The message could be correctly send.
 */
T_RNET_RET rnet_get_host_info (char *name,
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
T_RNET_IPPROTO rnet_get_proto (T_RNET_DESC *desc);

/**
 * Associates an application specific pointer to a connection ID.
 *
 * @param  desc    Connection identifier.
 * @param  user_data  Pointer that can be used by an application to store
 *            application specific data.
 */
void rnet_set_user_data (T_RNET_DESC *desc, void *user_data);

/**
 * Returns the application specific pointer associated to the connection ID.
 *
 * @param  desc  Connection identifier.
 * @return  Application specific data.
 */
void * rnet_get_user_data (T_RNET_DESC *desc);

#ifdef __cplusplus
}
#endif

#endif /* __RNET_API_H_ */

