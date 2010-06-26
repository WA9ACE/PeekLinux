/**
 * @file  rnet_message.h
 *
 * Riviera NET.
 *
 * Messages that RNET can SEND.
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
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_MESSAGE_H_
#define __RNET_MESSAGE_H_

#include "rv_general.h"
#include "rvm_use_id_list.h"

#include "rnet_api.h"
#include "rnet_cfg.h"
#include "rnet_ip_addr.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * The message offset must differ for each SWE in order to have
 * unique msg_id in the system.
 */
#define RNET_MESSAGE_OFFSET BUILD_MESSAGE_OFFSET(RNET_USE_ID)

/**
 * @name RNET_ERROR_IND
 *
 * Sent by RNET when an error occured in a connection.
 *
 * The following list indicates the values that the error code
 * can take as well as their meaning depending on the processing
 *
 * Connecting:
 * - RNET_MEMORY_ERR    No buffer space is available. The socket cannot be connected.
 * - RNET_NET_UNREACHABLE  The network cannot be reached from this host at this time.
 * - RNET_TIMEOUT      Attempt to connect timed out without establishing a connection
 * - RNET_CONN_REFUSED    The attempt to connect was forcefully rejected.
 * - RNET_NOT_SUPPORTED    Addresses in the specified family cannot be used with this socket.
 * Closing:
 * - RNET_INTERNAL_ERR    The network subsystem has failed.
 * - RNET_CONN_RESET    The connection was reset by the remote side
 * - RNET_CONN_ABORTED    The connection was terminated due to a time-out or other failure.
 * - RNET_CONN_CLOSED    The connection was gracefully closed (not an error actually)..
 *
 * @param  desc  Connection identifier.
 * @param  error  Error code.
 */
/*@{*/
/** Message ID. */
#define RNET_ERROR_IND (RNET_MESSAGE_OFFSET | 0x001)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_DESC * desc;
  T_RNET_RET error;

} T_RNET_ERROR_IND;
/*@}*/

/**
 * @name RNET_CONNECT_IND
 *
 * Sent to the connection oriented server when a client requests a connection.
 *
 * The server gets in this message a new connection identifier that
 * the one the server was listening with.
 * The client address and port are also indicated.
 *
 * @param  new_desc  New connection identifier.
 * @param  listen_desc  Indicates the descriptor of the connection that server
 *            is using for listening.
 * @param  peer_addr  Peer address.
 * @param  peer_port  Peer port.
 */
/*@{*/
/** Message ID. */
#define RNET_CONNECT_IND (RNET_MESSAGE_OFFSET | 0x002)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_DESC * new_desc;
  T_RNET_DESC * listen_desc;
  T_RNET_IP_ADDR peer_addr;
  T_RNET_PORT peer_port;

} T_RNET_CONNECT_IND;
/*@}*/

/**
 * @name RNET_CONNECT_CFM
 *
 * Indicates that a connection request was successfully completed.
 *
 * @param  desc  Connection identifier.
 */
/*@{*/
/** Message ID. */
#define RNET_CONNECT_CFM (RNET_MESSAGE_OFFSET | 0x003)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_DESC * desc;

} T_RNET_CONNECT_CFM;
/*@}*/

/**
 * @name RNET_SEND_RDY
 *
 * Indicates that the connection descriptor is ready for sending data.
 * A first RNET_SEND_RDY message is sent when a connection ID is first
 * connected (function rnet_connect) or accepted (message RNET_CONNECT_IND)
 * and then when a call to the function rnet_send couldn't send all
 * wanted data. The application should then try to send the data again
 * when receiving RNET_SEND_RDY.
 *
 * @param  desc  Connection identifier.
 */
/*@{*/
/** Message ID. */
#define RNET_SEND_RDY (RNET_MESSAGE_OFFSET | 0x004)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_DESC * desc;

} T_RNET_SEND_RDY;
/*@}*/

/**
 * @name RNET_RECV_IND
 *
 * New data arrived on the connection.
 *
 * See the description of the related function rnet_recv.
 *
 * @param  desc    Connection identifier.
 * @param  peer_addr  Peer address.
 * @param  peer_port  Peer port.
 */
/*@{*/
/** Message ID. */
#define RNET_RECV_IND (RNET_MESSAGE_OFFSET | 0x005)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_DESC * desc;
  T_RNET_IP_ADDR peer_addr;
  T_RNET_PORT peer_port;

} T_RNET_RECV_IND;
/*@}*/

/**
 * @name RNET_HOST_INFO
 *
 * Information on a host, names and addresses.
 *
 * If error is RNET_OK, no error occured and the host_name, host_addr
 * and user_data parameters are valid.
 * If is different from RNET_OK, the host information could not be retrieved
 * and only user_data is valid.
 *
 * @param  error      Indicates if the host information could be
 *              retrieved or not.
 * @param  host_name    Official name of host.
 * @param  host_addr    One address for the host.
 * @param  user_data    Application specific pointer set by the user
 *              in rnet_get_host_info.
 */
/*@{*/
/** Message ID. */
#define RNET_HOST_INFO (RNET_MESSAGE_OFFSET | 0x006)

/** Message structure. */
typedef struct
{
  T_RV_HDR hdr;

  T_RNET_RET error;

  T_RNET_IP_ADDR host_addr;
  char host_name[RNET_MAX_HOST_NAME_LEN];

  void * user_data;

} T_RNET_HOST_INFO;
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RNET_MESSAGE_H_ */

