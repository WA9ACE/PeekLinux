/* 
+------------------------------------------------------------------------------
|  File:       tcpip_api_layer.c
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  GPF-based TCP/IP's glue layer towards the socket API.
+----------------------------------------------------------------------------- 
*/ 


#define TCPIP_API_LAYER_C

#define ENTITY_TCPIP

/*==== INCLUDES =============================================================*/

#include <string.h>             /* String functions, e. g. strncpy(). */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "custom.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "dti.h"        /* For DTI library definitions. */
#include "glob_defs.h"
#include "tcpip.h"      /* to get the global entity definitions */
#include "tcpip_int.h"

/* RNET includes
 */
#include "rv_general.h"
#include "rnet_api.h"
#include "rnet_rt_env.h"
#include "rnet_message.h"
#include "rnet_rt_i.h"

/* NexGenIP includes
 */
#include <ngip/if.h>

#ifdef _SIMULATION_
#include "tcpip_sim_utils.h"    /* Utilities for use in the simulation. */
#endif /* _SIMULATION_ */



/*==== Local prototypes =====================================================*/




/*==== Macros ===============================================================*/

/* Two macros to go from socket descriptor to sock_table[] index and vice
 * versa.
 */
#define SOCK_S_INDEX(desc) (SOCKPAR_GET(desc)->s_index)
#define SOCK_RT_DESC(s_index) (sock_table[s_index]->rtdesc)

/* Retrieve the parameter block associated with a socket descriptor (as
 * user_data).
 */
#define SOCKPAR_GET(socket) ((T_sockpar *) rnet_get_user_data(socket))

/* Check if there is a valid socket for the specified socket index.
 */
#define INVALID_S_INDEX(socket) \
  ((socket) >= RNET_RT_SOCK_MAX OR sock_table[socket] EQ NULL)
    

/* We don't have a fixed value for the application handle (there will be more
 * than one). TCPIP wants to send a message to itself, so it is not convenient
 * to redirect all primitives, so we use the MMI handle for the application
 * for testing. This will then be redirected to the TAP.
 */
#ifdef _SIMULATION_
#define APP_HANDLE hCommMMI
#else  /* _SIMULATION_ */
#define APP_HANDLE app_handle
#endif /* _SIMULATION_ */


/*==== Types ================================================================*/

/* This struct is to be associated to a socket descriptor as user_data in
 * order to provide context for the event handler functions. The struct is
 * zero-initialized, so all default values must be zero.
 *
 * Pointers to these structures are kept in sock_table[].
 */
typedef struct sock_params {
  int s_index ;                 /* Index in socket table. */
  T_RNET_DESC *rtdesc ;         /* The RNET_RT socket descriptor. */
  T_HANDLE app_handle ;         /* Communication handle of application
                                 * entity. */
  U8 ipproto ;                  /* IP protocol number of socket. */
  U32 request_id ;              /* Request identification (if present). */
  U32 expected_event ;          /* The event we are waiting for on this
                                 * connection. This is necessary
                                 * for error handling, because the
                                 * RNET_ERROR_IND function gets no information
                                 * *which* request caused the error. */
  BOOL is_connected ;           /* Connected UDP or TCP socket. (NexGenIP does
                                 * not give us an error code when we try to
                                 * send on a non-connected socket, so we have
                                 * to maintain this status by ourselves in
                                 * order to report the error to the
                                 * application.  */
  BOOL recv_waiting ;           /* TRUE iff incoming data is (or might be)
                                 * available. */
  BOOL appl_xoff ;              /* TRUE iff flow control to the application is
                                 * in "xoff" status.  */
  struct
  {
    U16 total_length ;          /* Total length of buffer, zero if no data is
                                 * waiting. */
    U16 offset ;                /* Offset of first byte not yet sent. */
    U8 *buffer ;                /* Pointer to data buffer waiting to be
                                 * sent. */
  } send ;
} T_sockpar ;


/*==== Local data ===========================================================*/

  
/* Table of active socket descriptors; provides the mapping between the small
 * integers used in the primitives and the actual RNET socket descriptors.
 */
static T_sockpar *sock_table[RNET_RT_SOCK_MAX] ;



/*==== Primitive sender functions ===========================================*/


/** Confirm the result of a TCPIP_INITIALIZE_REQ.
 * 
 * @param result   Result of the initialization.
 */
static void tcpip_initialize_cnf(U8 result)
{
  TRACE_FUNCTION("tcpip_initialize_cnf()") ;

  {
    PALLOC(prim, TCPIP_INITIALIZE_CNF) ;
    prim->result = result ;
    PSENDX(MMI, prim) ;
  }
}


/** Confirm the result of a TCPIP_SHUTDOWN_REQ
 * 
 * @param result   Result of the shutdown.
 */
static void tcpip_shutdown_cnf(U8 result)
{
  TRACE_FUNCTION("tcpip_shutdown_cnf()") ;

  {
    PALLOC(prim, TCPIP_SHUTDOWN_CNF) ;
    prim->result = result ;
    PSENDX(MMI, prim) ;
  }
}


/** Confirm the result of a TCPIP_IFCONFIG_REQ
 * 
 * @param result   Result of the configuration.
 */
static void tcpip_ifconfig_cnf(U8 result)
{
  TRACE_FUNCTION("tcpip_ifconfig_cnf()") ;

  {
    PALLOC(prim, TCPIP_IFCONFIG_CNF) ;
    prim->result = result ;
    PSENDX(MMI, prim) ;
  }
}


/** Confirm the result of a TCPIP_DTI_REQ. This function is called
 * from tcpip_dti.c, so it must not be static.
 * 
 * @param dti_conn    Indicates whether the DTI link is to be established or
 *                    disconnected
 * @param link_id     DTI link identifier
 */
void tcpip_dti_cnf(U8 dti_conn, U32 link_id)
{
  TRACE_FUNCTION("tcpip_dti_cnf()") ;

  {
    PALLOC(prim, TCPIP_DTI_CNF) ;
    prim->dti_conn = dti_conn ;
    prim->link_id = link_id ;
    PSENDX(MMI, prim) ;
  }
}


/** Confirm the result of a TCPIP_CREATE_REQ.
 *
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of newly created socket (if OK).
 * @param request_id    Request ID as passed in TCPIP_CREATE_REQ.
 */
static void tcpip_create_cnf(T_HANDLE app_handle, U8 result, int socket,
                             U32 request_id)
{
  TRACE_FUNCTION("tcpip_create_cnf()") ;
  TRACE_EVENT_P1("app_handle %d",APP_HANDLE);

  {
    PALLOC(prim, TCPIP_CREATE_CNF) ;
    prim->event_type = TCPIP_EVT_CREATE_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    prim->request_id = request_id ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_CLOSE_REQ.
 * 
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of (no longer valid) socket.
 */
static void tcpip_close_cnf(T_HANDLE app_handle, U8 result, int socket)
{
  TRACE_FUNCTION("tcpip_close_cnf()") ;

  {
    PALLOC(prim, TCPIP_CLOSE_CNF) ;
    prim->event_type = TCPIP_EVT_CLOSE_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_BIND_REQ.
 *
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 */
static void tcpip_bind_cnf(T_HANDLE app_handle, U8 result, int socket)
{
  TRACE_FUNCTION("tcpip_bind_cnf()") ;

  {
    PALLOC(prim, TCPIP_BIND_CNF) ;
    prim->event_type = TCPIP_EVT_BIND_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_LISTEN_REQ.
 * 
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 */
static void tcpip_listen_cnf(T_HANDLE app_handle, U8 result, int socket)
{
  TRACE_FUNCTION("tcpip_listen_cnf()") ;

  {
    PALLOC(prim, TCPIP_LISTEN_CNF) ;
    prim->event_type = TCPIP_EVT_LISTEN_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_CONNECT_REQ.
 * 
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 */
static void tcpip_connect_cnf(T_HANDLE app_handle, U8 result, int socket)
{
  TRACE_FUNCTION("tcpip_connect_cnf()") ;

  {
    PALLOC(prim, TCPIP_CONNECT_CNF) ;
    prim->event_type = TCPIP_EVT_CONNECT_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_DATA_REQ.
 * 
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 * @param window        Window size for sender.
 */
static void tcpip_data_cnf(T_HANDLE app_handle, U8 result, int socket,
                           U16 window)
{
  TRACE_FUNCTION("tcpip_data_cnf()") ;

  {
    PALLOC(prim, TCPIP_DATA_CNF) ;
    prim->event_type = TCPIP_EVT_FLOW_READY_IND ;
    prim->result = result ;
    prim->socket = socket ;
    prim->window = window ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Indicate incoming data.
 * 
 * @param app_handle    Communication handle of requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 * @param ipaddr        Source IP address.
 * @param port          Source port number.
 * @param buflen        Length of payload data.
 * @param data          Adress of payload data buffer.
 */
static void tcpip_data_ind(T_HANDLE app_handle, U8 result, int socket,
                           U32 ipaddr, U16 port, U16 buflen, U8 *data)
{
  TRACE_FUNCTION("tcpip_data_ind()") ;

  {
    PALLOC(prim, TCPIP_DATA_IND) ;
    prim->event_type = TCPIP_EVT_RECV_IND ; /* Unfortunately not _DATA_IND */
    prim->result = result ;
    prim->socket = socket ;
    prim->ipaddr = ipaddr ;
    prim->port = port ;
    prim->buflen = buflen ;
    prim->data = (U32) data ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_SOCKNAME_REQ.
 * 
 * @param app_handle    Communication handle of the requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 * @param ipaddr        IP address of the socket.
 * @param port          Port number of the socket.
 */
static void tcpip_sockname_cnf(T_HANDLE app_handle, U8 result, int socket,
                               U32 ipaddr, U16 port)
{
  TRACE_FUNCTION("tcpip_sockname_cnf()") ;

  {
    PALLOC(prim, TCPIP_SOCKNAME_CNF) ;
    prim->event_type = TCPIP_EVT_SOCKNAME_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    prim->ipaddr = ipaddr ;
    prim->port = port ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_PEERNAME_REQ.
 * 
 * @param app_handle    Communication handle of the requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 * @param ipaddr        IP address of the remote peer.
 * @param port          Remore port number of the socket.
 */
static void tcpip_peername_cnf(T_HANDLE app_handle, U8 result, int socket,
                               U32 ipaddr, U16 port)
{
  TRACE_FUNCTION("tcpip_peername_cnf()") ;

  {
    PALLOC(prim, TCPIP_PEERNAME_CNF) ;
    prim->event_type = TCPIP_EVT_PEERNAME_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    prim->ipaddr = ipaddr ;
    prim->port = port ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_HOSTINFO_REQ.
 * 
 * @param app_handle    Communication handle of the requesting task.
 * @param result        Result of the operation.
 * @param request_id    Request ID as passed in TCPIP_CREATE_REQ.
 * @param hostname      Full-qualified domain name of the host, may be NULL.
 * @param ipaddr        IP address of the host.
 */
static void tcpip_hostinfo_cnf(T_HANDLE app_handle, U8 result, U32 request_id,
                        char *hostname, U32 ipaddr)
{
  TRACE_FUNCTION("tcpip_hostinfo_cnf()") ;

  {
    PALLOC(prim, TCPIP_HOSTINFO_CNF) ;
    prim->event_type = TCPIP_EVT_HOSTINFO_CNF ;
    prim->result = result ;
    prim->request_id = request_id ;
    if (hostname NEQ NULL)
    {
      strncpy((char *) prim->hostname, hostname, TCPIP_HNAMELEN) ;
      prim->hostname[TCPIP_HNAMELEN-1] = '\0' ;
    }
    else
    {
      prim->hostname[0] = '\0' ;
    }
    prim->ipaddr = ipaddr ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Confirm the result of a TCPIP_MTU_SIZE_REQ.
 * 
 * @param app_handle    Communication handle of the requesting task.
 * @param result        Result of the operation.
 * @param socket        Index of the socket.
 * @param mtu_size      Size of the MTU.
 */
static void tcpip_mtu_size_cnf(T_HANDLE app_handle, U8 result, int socket,
                               U16 mtu_size)
{
  TRACE_FUNCTION("tcpip_mtu_size_cnf()") ;

  {
    PALLOC(prim, TCPIP_MTU_SIZE_CNF) ;
    prim->event_type = TCPIP_EVT_MTU_SIZE_CNF ;
    prim->result = result ;
    prim->socket = socket ;
    prim->mtu_size = mtu_size ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Indicate an incoming TCP connection.
 * 
 * @param app_handle    Communication handle of the task.
 * @param socket        Index of the listening socket.
 * @param new_socket    New socket for this connection.
 * @param ipaddr        IP address of the remote peer.
 * @param port          Remore port number of the socket.
 */
static void tcpip_connect_ind(T_HANDLE app_handle, int socket,
                              int new_socket, U32 ipaddr, U16 port)
{
  TRACE_FUNCTION("tcpip_connect_ind()") ;

  {
    PALLOC(prim, TCPIP_CONNECT_IND) ;
    prim->event_type = TCPIP_EVT_CONNECT_IND ;
    prim->result = TCPIP_RESULT_OK ;
    prim->socket = socket ;
    prim->new_socket = new_socket ;
    prim->ipaddr = ngHTONL(ipaddr) ;
    prim->port = ngHTONS(port) ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Indicate that a connection has been closed.
 * 
 * @param app_handle    Communication handle of the task.
 * @param socket        Index of the socket.
 */
static void tcpip_conn_closed_ind(T_HANDLE app_handle, int socket)
{
  TRACE_FUNCTION("tcpip_conn_closed_ind()") ;

  {
    PALLOC(prim, TCPIP_CONN_CLOSED_IND) ;
    prim->event_type = TCPIP_EVT_CONN_CLOSED_IND ;
    prim->result = TCPIP_RESULT_OK ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Indicate an asynchronous error on a socket.
 * 
 * @param app_handle    Communication handle of the task.
 * @param result        Result code of the error.
 * @param socket        Index of the socket.
 */
static void tcpip_error_ind(T_HANDLE app_handle, U8 result, int socket)
{
  TRACE_FUNCTION("tcpip_error_ind()") ;
  
  {
    PALLOC(prim, TCPIP_ERROR_IND) ;
    prim->event_type = TCPIP_EVT_ERROR_IND ;
    prim->result = result ;
    prim->socket = socket ;
    PSEND(APP_HANDLE, prim) ;
  }
}


/** Send a message to self.
 * 
 * @param msg_p    pointer to message
 * @param msg_id   message identification
 */
void tcpip_send_internal_ind(U32 msg_p, U32 msg_id)
{
  TRACE_FUNCTION("tcpip_send_internal_ind()") ;

  {
    PALLOC(prim, TCPIP_INTERNAL_IND) ;
    prim->msg_p = msg_p ;
    prim->msg_id = msg_id ;
    PSEND(hCommTCPIP, prim) ;
  }
}


/*==== Local utility functions ==============================================*/

/** Allocate a new socket parameter block, initialize it with the given
 * parameters, and put it into the list. Allocate a slot in the sock_table[]
 * and fill it. If we cannot allocate a slot, return NULL. This is considered
 * an internal error, because we *have* a slot free for each possible socket.
 * 
 * @param socket        The socket descriptor (if applicable).
 * @param app_handle    Communication handle of application task.
 * @param request_id    Request identification (if applicable).
 * @return a pointer to the sock_params struct or NULL on error.
 */
static T_sockpar *sockpar_new(T_RNET_DESC *socket, T_HANDLE app_handle,
                              U8 ipproto, U32 request_id)
{
  T_sockpar *sp ;               /* Pointer to new struct. */
  int sti ;                     /* Socket table index. */

  TRACE_FUNCTION("sockpar_new()") ;

  /* Allocate and enqueue. */
  MALLOC(sp, sizeof(T_sockpar)) ;
  /* TRACE_EVENT_P1("MALLOC gives us %08x", sp) ; */
  memset(sp, 0, sizeof(T_sockpar)) ;

  sp->rtdesc = socket ;
  sp->app_handle = app_handle ;
  sp->ipproto = ipproto ;
  sp->request_id = request_id ;

  for (sti = 0; sti < RNET_RT_SOCK_MAX; sti++)
  {
    if (sock_table[sti] EQ NULL)
    {
      sock_table[sti] = sp ;
      sp->s_index = sti ;
      return sp ;
    }
  }
  /* No free slot in table found -- this must be an error, because we have a
   * slot for each possible socket. */
  TRACE_ERROR("No free slot in sock_table[] found") ;
  /* TRACE_EVENT_P1("We MFREE %08x", sp) ; */
  MFREE(sp) ;
  return NULL ;
}


/** Dequeue and deallocate a socket parameter block. Free the slot in
 * sock_table[].
 * 
 * @param sp    Pointer to sock_params struct.
 */
static void sockpar_delete(T_sockpar *sp)
{
  TRACE_FUNCTION("sockpar_delete()") ;

  sock_table[sp->s_index] = 0 ;
  /* TRACE_EVENT_P1("We MFREE %08x", sp) ; */
  MFREE(sp) ;
}


/** Clear the send buffer of a socket parameter block and free the associated
 * data.
 * 
 * @param sockpar    The socket parameter block.
 */
static void tcpip_clear_send_buffer(T_sockpar *sockpar)
{
  sockpar->send.total_length = 0 ;
  sockpar->send.offset = 0 ;
#ifndef _SIMULATION_
  /* The simulation would crash in the MFREE(), as the send.buffer is not a
   * frame-allocated piece of memory. */
  /* TRACE_EVENT_P1("MFREE sockpar->send.buffer %x", sockpar->send.buffer) ; */
  MFREE(sockpar->send.buffer) ;
#endif /* _SIMULATION_ */
  sockpar->send.buffer = NULL ;
}


/** Convert an RNET error code to the appropriate TCPIP result code.
 * 
 * @param rnet_ret    The RNET error code.
 * @return            The TCPIP result code.
 */
static U8 rnet_error_to_tcpip_result(T_RNET_RET rnet_ret)
{
  switch (rnet_ret)
  {
    case RNET_OK:
      TRACE_EVENT("RNET_OK -> TCPIP_RESULT_OK") ;
      return TCPIP_RESULT_OK ;
    case RNET_MEMORY_ERR:
      TRACE_EVENT("RNET_MEMORY_ERR -> TCPIP_RESULT_OUT_OF_MEMORY") ;
      return TCPIP_RESULT_OUT_OF_MEMORY ;
    case RNET_INVALID_PARAMETER:
      TRACE_EVENT("RNET_INVALID_PARAMETER -> TCPIP_RESULT_INVALID_PARAMETER") ;
      return TCPIP_RESULT_INVALID_PARAMETER ;
    case RNET_NOT_SUPPORTED:
      TRACE_EVENT("RNET_NOT_SUPPORTED -> TCPIP_RESULT_NOT_SUPPORTED") ;
      return TCPIP_RESULT_NOT_SUPPORTED ;
    case RNET_NOT_READY:
      TRACE_EVENT("RNET_NOT_READY -> TCPIP_RESULT_NOT_READY") ;
      return TCPIP_RESULT_NOT_READY ;
    case RNET_INTERNAL_ERR:
      TRACE_EVENT("RNET_INTERNAL_ERR -> TCPIP_RESULT_INTERNAL_ERROR") ;
      return TCPIP_RESULT_INTERNAL_ERROR ;
    case RNET_IN_USE:
      TRACE_EVENT("RNET_IN_USE -> TCPIP_RESULT_ADDR_IN_USE") ;
      return TCPIP_RESULT_ADDR_IN_USE ;
    case RNET_NOT_INITIALIZED:
      TRACE_EVENT("RNET_NOT_INITIALIZED -> TCPIP_RESULT_NOT_READY") ;
      return TCPIP_RESULT_NOT_READY ;
    case RNET_NET_UNREACHABLE:
      TRACE_EVENT("RNET_NET_UNREACHABLE -> TCPIP_RESULT_UNREACHABLE") ;
      return TCPIP_RESULT_UNREACHABLE ;
    case RNET_TIMEOUT:
      TRACE_EVENT("RNET_TIMEOUT -> TCPIP_RESULT_TIMEOUT") ;
      return TCPIP_RESULT_TIMEOUT ;
    case RNET_CONN_REFUSED:
      TRACE_EVENT("RNET_CONN_REFUSED -> TCPIP_RESULT_CONN_REFUSED") ;
      return TCPIP_RESULT_CONN_REFUSED ;
    case RNET_CONN_RESET:
      TRACE_EVENT("RNET_CONN_RESET -> TCPIP_RESULT_CONN_RESET") ;
      return TCPIP_RESULT_CONN_RESET ;
    case RNET_CONN_ABORTED:
      TRACE_EVENT("RNET_CONN_ABORTED -> TCPIP_RESULT_CONN_ABORTED") ;
      return TCPIP_RESULT_CONN_ABORTED ;
    case RNET_MSG_SIZE:
      TRACE_EVENT("RNET_MSG_SIZE -> TCPIP_RESULT_MSG_TOO_BIG") ;
      return TCPIP_RESULT_MSG_TOO_BIG ;
    case RNET_HOST_NOT_FOUND:
      TRACE_EVENT("RNET_HOST_NOT_FOUND -> TCPIP_RESULT_HOST_NOT_FOUND") ;
      return TCPIP_RESULT_HOST_NOT_FOUND ;

      /* The following should not be delivered as a result code: */
    case RNET_CONN_CLOSED:
      TRACE_EVENT("RNET_CONN_CLOSED -> TCPIP_RESULT_INTERNAL_ERROR") ;
      return TCPIP_RESULT_INTERNAL_ERROR ;
    case RNET_PARTIAL_SENT:
      TRACE_EVENT("RNET_PARTIAL_SENT -> TCPIP_RESULT_INTERNAL_ERROR") ;
      return TCPIP_RESULT_INTERNAL_ERROR ;
    default:
      TRACE_EVENT_P1("unknown (%d) ->TCPIP_RESULT_INTERNAL_ERROR", rnet_ret) ;
      return TCPIP_RESULT_INTERNAL_ERROR ;
  }
}


/** Read incoming data (from TCP/IP to the application). This function is
 * called only when the flow control status towards the application is in xon
 * state.
 * 
 * @param sockpar      Socket parameter block.
 */
static void tcpip_read_incoming_to_app(T_sockpar *sockpar)
{
  U8 *buffer ;                  /* Payload data buffer. */
  U16 length ;                  /* Payload data length. */
  T_RNET_RET retval ;           /* Return value of rnet_recv(). */
  T_RNET_IP_ADDR ipaddr ;       /* IP address of sender. */
  T_RNET_PORT port ;            /* Port numer at remote end. */

  TRACE_FUNCTION("tcpip_read_incoming_to_app()") ;

  /* If flow control status is off, we must not send incoming data. */
  if (sockpar->appl_xoff)
  {
    TRACE_EVENT("tcpip_read_incoming_to_app() called in xoff state") ;
    return ;
  }

  /* We don't expect to read packets larger than this. To be precise, we
   * aren't able to. */
  MALLOC(buffer, TCPIP_DEFAULT_MTU_SIZE) ;
  /* TRACE_EVENT_P1("MALLOC gives us %08x", buffer) ; */
  length = TCPIP_DEFAULT_MTU_SIZE ;

  /* Should be unspecified for TCP; will be set by rnet_recv_from() for
   * UDP. */
  ipaddr = TCPIP_UNSPECIFIED_IPADDR ;
  port = TCPIP_UNSPECIFIED_PORT ;
  
  switch (sockpar->ipproto)
  {
    case TCPIP_IPPROTO_TCP:
      TRACE_EVENT_P2("Calling rnet_recv() for socket %d length %d",
                     sockpar->s_index, length) ;
      retval = rnet_recv(sockpar->rtdesc, buffer, &length) ;
      TRACE_EVENT_P2("rnet_recv() returns %d length %d", retval, length) ;
      break ;
    case TCPIP_IPPROTO_UDP:     /* Need to read sender address with UDP. */
      TRACE_EVENT_P4("Calling rnet_recv_from() for socket %d length %d "
                     "ipaddr %x port %d",
                     sockpar->s_index, length, ipaddr, port) ;
      retval = rnet_recv_from(sockpar->rtdesc, buffer, &length,
                              &ipaddr, &port) ;
      TRACE_EVENT_P4("rnet_recv_from() returns %d length %d ipaddr %x port %d",
                     retval, length, ipaddr, port) ;
      break ;
    default:
      retval = RNET_INVALID_PARAMETER ;
      TRACE_ERROR("tcpip_read_incoming_to_app: unknown IP protocol") ;
      break ;
  }

  if (retval EQ RNET_OK)
  {
    /* Only if the length is zero, there is no more data waiting. */
    if (length EQ 0)
    {
      sockpar->recv_waiting = FALSE ;
      /* TRACE_EVENT_P1("We MFREE %08x", buffer) ; */
      MFREE(buffer) ;
    }
    else
    {
      /* We use an effective window size of zero, so flow control status is
       * xoff after sending a primitive. */
      tcpip_data_ind(sockpar->app_handle, TCPIP_RESULT_OK,
                     sockpar->s_index, ipaddr, port, length, buffer) ;
      TRACE_EVENT("switch flow control towards application to xoff") ;
      dti_stop(tcpip_data->dti_handle, 0, TCPIP_DTI_TO_LOWER_LAYER, 0) ;    // Add one flow control to not allow SNDCP send next data package. OMAPS00172999  05132008
	  
      sockpar->appl_xoff = TRUE ;
#ifdef _SIMULATION_
      /* In the simulation, free the buffer -- it is meaningless for the TAP
       * and will not be freed at any other place. */
      MFREE(buffer) ;
#endif /* _SIMULATION_ */
    }
  }
  else                          /* retval != RNET_OK */
  {
    /* TRACE_EVENT_P1("We MFREE %08x", buffer) ; */
    MFREE(buffer) ;
    tcpip_error_ind(sockpar->app_handle, rnet_error_to_tcpip_result(retval),
                    sockpar->s_index) ;
  }
}


/** Try to send data over RNET. To be called after the application has sent
 * data, and if we have waiting data and RNET has signalled that we may send
 * again.
 * 
 * @param sockpar      Socket parameter block.
 */
static void tcpip_try_send_data(T_sockpar *sockpar)
{
  U16 length ;                  /* Length of data to send or sent. */
  T_RNET_RET retval ;           /* Return value of rnet_send(). */
  
  TRACE_FUNCTION("tcpip_try_send_data()") ;
  
  if (sockpar->send.total_length EQ 0)
  {
    TRACE_ERROR("tcpip_try_send_data: called although no data present") ;
  }
  else
  {
    length = sockpar->send.total_length - sockpar->send.offset ;
    TRACE_EVENT_P2("Calling rnet_send() socket %d length %d",
                   sockpar->s_index, length) ;
    retval = rnet_send(sockpar->rtdesc, sockpar->send.buffer, &length) ;
    TRACE_EVENT_P2("rnet_send() returns %d length %d", retval, length) ;
    switch (retval)
    {
      case RNET_OK:             /* We could send all data, so clear send
                                 * buffer and send a confirmation to the
                                 * application. */
        tcpip_clear_send_buffer(sockpar) ;
        tcpip_data_cnf(sockpar->app_handle, TCPIP_RESULT_OK,
                       sockpar->s_index, TCPIP_DEFAULT_WINDOW) ;
        break ;
      case RNET_PARTIAL_SENT:   /* Not all of the data could be sent. We
                                 * update the send buffer offset and wait for
                                 * an RNET_SEND_RDY event to continue. */
        sockpar->send.offset += length ;
        break ;
      default:                  /* Every other return value indicates an
                                 * error. We translate the return value to our
                                 * result codes and send an error indication
                                 * to the application. The data will no longer
                                 * be needed and is freed. */
        tcpip_clear_send_buffer(sockpar) ;
        tcpip_data_cnf(sockpar->app_handle,
                       rnet_error_to_tcpip_result(retval),
                       sockpar->s_index, TCPIP_DEFAULT_WINDOW) ;
        break ;
    }
  }
}


/** Initialize RNET and the data of the TCPIP entity.
 *
 * This in a separate function to make control flow more elegant -- this way
 * we can jump out of the initialization sequence without having to use a
 * goto.
 * 
 * @return a result code with the usual semantics
 */
static U8 tcpip_do_initialization(void)
{
  static T_RVF_MB_ID   entity_bk_id_table[8];
  static T_RV_RETURN_PATH entity_return_pathes[8];
  T_RNET_RET retval ;           /* Return value of RNET initialisation. */

  TRACE_FUNCTION("tcpip_do_initialization()") ;

  if (tcpip_data->is_initialized)
  {
    TRACE_ERROR("initialization called although tcpip_data->is_initialized") ;
    return TCPIP_RESULT_INTERNAL_ERROR ;
  }

  memset(sock_table,0,sizeof(sock_table));
  
  /* quite ad-hoc: both arrays "entity_return_pathes",
    "entity_bk_id_table" are uninitialized and arbitrarily set to length
    8. last param, call_back_error_ft function, undefined. */
  rnet_rt_set_info((T_RVF_ADDR_ID) tcpip_handle, entity_return_pathes,
                   entity_bk_id_table, 0);

  retval = (T_RNET_RET)rnet_rt_init() ;
  if (retval NEQ RNET_OK )
  {
    TRACE_ERROR("rnet_rt_init() != RV_OK") ;
    return rnet_error_to_tcpip_result(retval) ;
  }

  retval = (T_RNET_RET)rnet_rt_start() ;
  if (retval NEQ RNET_OK )
  {
    TRACE_ERROR("rnet_rt_start() != RV_OK") ;
    rnet_rt_kill() ;
    return rnet_error_to_tcpip_result(retval) ;
  }

#ifdef _SIMULATION_
  tcpip_if_properties(&rnet_rt_env_ctrl_blk_p->ifnet_lo) ;
#endif  /* _SIMULATION_ */

  tcpip_data->is_initialized = TRUE ;

  return TCPIP_RESULT_OK ;
}




/** Mark an event as expected for the specified socket.
 * 
 * @param sock_desc          The Socket descriptor.
 * @param expected_event     The event type.
 */
static void socket_expect_event(T_RNET_DESC* sock_desc, U32 expected_event)
{
#ifdef TRACING
  char *event_name ;
  
  switch (expected_event)
  {
    case TCPIP_EVT_CONNECT_CNF:
      event_name = "CONNECT_CNF" ;
      break ;
    case TCPIP_EVT_RECV_IND:
      event_name = "RECV_IND" ;
      break ;
    case TCPIP_EVT_CONNECT_IND:
      event_name = "CONNECT_IND" ;
      break ;
    default:
      event_name = "<none>" ;
      break ;
  }
  TRACE_EVENT_P1("ready for TCPIP_EVT_%s for %d",
                 event_name, SOCK_S_INDEX(sock_desc)) ;
#endif /* TRACING */
  SOCKPAR_GET(sock_desc)->expected_event = expected_event ;
}



/*==== Specific event handler functions =====================================*/


/** Handle an RNET_CONNECT_IND event; pass it through to the application.
 * 
 * @param connect_ind    Pointer to the event message.
 */
static void tcpip_handle_rnet_connect_ind(T_RNET_CONNECT_IND *connect_ind)
{
  T_sockpar *sockpar, *sp_new ;

  TRACE_FUNCTION("tcpip_handle_rnet_connect_ind()") ;

  sockpar = SOCKPAR_GET(connect_ind->listen_desc) ;
  sp_new = sockpar_new(connect_ind->new_desc,
                       sockpar->app_handle,
                       sockpar->ipproto, 0) ;
  if (sp_new EQ NULL)
  {
    tcpip_error_ind(sockpar->app_handle, TCPIP_RESULT_INTERNAL_ERROR,
                    sockpar->s_index) ;
  }
  else
  {
    sp_new->is_connected = TRUE ;
    rnet_set_user_data(connect_ind->new_desc, (void *) sp_new) ;
    tcpip_connect_ind(sockpar->app_handle,
                      sockpar->s_index,
                      sp_new->s_index,
                      connect_ind->peer_addr,
                      connect_ind->peer_port) ;
    socket_expect_event(connect_ind->new_desc, TCPIP_EVT_RECV_IND) ;
  }
  rvf_free_buf(connect_ind) ;
}


/** Handle an RNET_CONNECT_CFM event; pass it through to the application.
 * 
 * @param connect_cfm    Pointer to the event message.
 */
static void tcpip_handle_rnet_connect_cfm(T_RNET_CONNECT_CFM *connect_cfm)
{
  T_sockpar *sockpar ;

  TRACE_FUNCTION("tcpip_handle_rnet_connect_cfm()") ;
  
  sockpar = SOCKPAR_GET(connect_cfm->desc) ;
  sockpar->is_connected = TRUE ;
  tcpip_connect_cnf(sockpar->app_handle,
                    TCPIP_RESULT_OK, SOCK_S_INDEX(connect_cfm->desc)) ;
  socket_expect_event(connect_cfm->desc, TCPIP_EVT_RECV_IND) ;
  rvf_free_buf(connect_cfm) ;
}


/** Handle an RNET_SEND_RDY event; try to send more data if anything is left.
 * 
 * @param send_rdy    Pointer to the event message.
 */
static void tcpip_handle_rnet_send_rdy(T_RNET_SEND_RDY *send_rdy)
{
  T_sockpar *sockpar ;

  TRACE_FUNCTION("tcpip_handle_rnet_send_rdy()") ;

  sockpar = SOCKPAR_GET(send_rdy->desc) ;
  if( sockpar )
  {
    if (sockpar->send.total_length)
    {
      tcpip_try_send_data(sockpar) ;
    }
    else
    {
      TRACE_EVENT("received RNET_SEND_RDY; no data waiting") ;
    }
  }else
    TRACE_ERROR("tcpip_handle_rnet_send_rdy(): WARNING: sockpar=0");

  rvf_free_buf(send_rdy) ;
}


/** Handle an RNET_RECV_IND event; read incoming data.
 * 
 * @param recv_ind    Pointer to the event message.
 */
static void tcpip_handle_rnet_recv_ind(T_RNET_RECV_IND *recv_ind)
{
  T_sockpar *sockpar ;      /* Socket parameters. */

  TRACE_FUNCTION("tcpip_handle_rnet_recv_ind()") ;

  sockpar = SOCKPAR_GET(recv_ind->desc) ;
  sockpar->recv_waiting = TRUE ;
  tcpip_read_incoming_to_app(sockpar) ;

  rvf_free_buf(recv_ind) ;
}


/** Handle an RNET_ERROR_IND event; 
 * 
 * @param error_ind    Pointer to the event message.
 */
static void tcpip_handle_rnet_error_ind(T_RNET_ERROR_IND *error_ind)
{
  T_sockpar *sockpar ;

  TRACE_FUNCTION("tcpip_handle_rnet_error_ind()") ;
  TRACE_EVENT_P1("RNET_ERROR_IND for socket %08x", error_ind->desc) ;
    
  sockpar = SOCKPAR_GET(error_ind->desc) ;
  if (error_ind->error EQ RNET_CONN_CLOSED)
  {
    TRACE_EVENT("RNET_CONN_CLOSED") ;
    tcpip_conn_closed_ind(sockpar->app_handle, sockpar->s_index) ;
  }
  else
  {
    /* TODO: this switch looks bogus -- we are only interested in
     * TCPIP_EVT_CONNECT_CNF, right? Everything else is handled the same
     * anyway. */
    switch (sockpar->expected_event)
    {
      case TCPIP_EVT_CONNECT_CNF:
        TRACE_EVENT("error received when expecting TCPIP_EVT_CONNECT_CNF") ;
        tcpip_connect_cnf(sockpar->app_handle,
                          rnet_error_to_tcpip_result(error_ind->error),
                          sockpar->s_index) ;
        break ;
      case TCPIP_EVT_RECV_IND:
        TRACE_EVENT("error received when expecting TCPIP_EVT_RECV_IND") ;
        tcpip_error_ind(sockpar->app_handle,
                        rnet_error_to_tcpip_result(error_ind->error),
                        sockpar->s_index) ;
        break ;
      case TCPIP_EVT_CONNECT_IND:
        TRACE_EVENT("error received when expecting TCPIP_EVT_CONNECT_IND") ;
        tcpip_error_ind(sockpar->app_handle,
                        rnet_error_to_tcpip_result(error_ind->error),
                        sockpar->s_index) ;
        break ;
      default:
        TRACE_EVENT_P1("error received when expecting unknown event (%d)?",
                       sockpar->expected_event) ;
        TRACE_ERROR("Unexpected sockpar->expected_event in "
                    "tcpip_handle_rnet_error_ind()") ;
        tcpip_error_ind(sockpar->app_handle,
                        rnet_error_to_tcpip_result(error_ind->error),
                        sockpar->s_index) ;
        break ;
    }
  }

  rvf_free_buf(error_ind) ;
}



/*==== Callback functions ===================================================*/


/** Callback for rnet_get_host_info().
 * 
 * @param 
 * @return 
 */
void tcpip_hostinfo_callback(void *msg)
{
  T_RNET_HOST_INFO *hinfo ;
  T_TCPIP_HOSTINFO_REQ *request ;

  TRACE_FUNCTION("tcpip_hostinfo_callback()") ;

  hinfo = msg ;
  request = hinfo->user_data ;
  switch (hinfo->error)
  {
    case RNET_OK:
      tcpip_hostinfo_cnf(request->app_handle,
                         TCPIP_RESULT_OK,
                         request->request_id,
                         hinfo->host_name,
                         ngHTONL(hinfo->host_addr)) ;
      break ;
    default:
      tcpip_hostinfo_cnf(request->app_handle,
                         rnet_error_to_tcpip_result(hinfo->error),
                         request->request_id,
                         NULL,
                         0) ;
      break ;
  }
  PFREE(request) ;
  //PatternVibrator("o20f10", 1);
  //rvf_free_buf(msg) ;
  //PatternVibrator("o20f10", 1);
}


/** Callback for RNET events.
 * 
 * @param rv_msg    Pointer to Riviera message.
 */
static void tcpip_rnet_callback(void *rv_msg)
{
  T_RV_HDR *rv_hdr ;            /* Header of Riviera message. */
  rv_hdr = (T_RV_HDR *) rv_msg ;

  TRACE_FUNCTION("tcpip_rnet_callback()") ;
  TRACE_EVENT_P1("rv_hdr->msg_id = %d",rv_hdr->msg_id);

  switch (rv_hdr->msg_id)
  {
    case RNET_CONNECT_IND:
      TRACE_EVENT("tcpip_rnet_callback() called with RNET_CONNECT_IND") ;
      tcpip_handle_rnet_connect_ind((T_RNET_CONNECT_IND *) rv_hdr) ;
      break ;
    case RNET_CONNECT_CFM:
      TRACE_EVENT("tcpip_rnet_callback() called with RNET_CONNECT_CFM") ;
      tcpip_handle_rnet_connect_cfm((T_RNET_CONNECT_CFM *) rv_hdr) ;
      break ;
    case RNET_SEND_RDY:
      TRACE_EVENT("tcpip_rnet_callback() called with RNET_SEND_RDY") ;
      tcpip_handle_rnet_send_rdy((T_RNET_SEND_RDY *) rv_hdr) ;
      break ;
    case RNET_RECV_IND:
      TRACE_EVENT("tcpip_rnet_callback() called with RNET_RECV_IND") ;
      tcpip_handle_rnet_recv_ind((T_RNET_RECV_IND *) rv_hdr) ;
      break ;
    case RNET_ERROR_IND:
      TRACE_EVENT("tcpip_rnet_callback() called with RNET_ERROR_IND") ;
      tcpip_handle_rnet_error_ind((T_RNET_ERROR_IND *) rv_hdr) ;
      break ;
    default:
      TRACE_ERROR("Default: unknown RNET event:") ;
      TRACE_EVENT_P1("[ERROR] event 0x%08x from RNET\n", rv_hdr->msg_id) ;
      break ;
  }
}


/*==== Other public functions ===============================================*/

/** Shut down RNET and deallocate data. This defined as a separate function
 * because it will also be called by pei_exit().
 * 
 */
void tcpip_do_shutdown(void)
{
  int s_index ;                 /* Socket index in sock_table[]. */

  TRACE_FUNCTION("tcpip_do_shutdown()") ;

  if (tcpip_data->is_initialized)
  {
    /* The error code conversion is done only for the trace in
     * rnet_error_to_tcpip_result(). It doesn't hurt anyway. */
    rnet_error_to_tcpip_result((T_RNET_RET)rnet_rt_stop()) ;
    rnet_error_to_tcpip_result((T_RNET_RET)rnet_rt_kill()) ;

    for (s_index = 0; s_index < RNET_RT_SOCK_MAX; s_index++)
    {
      T_sockpar *sp ;           /* Pointer to socket parameter struct. */

      sp = sock_table[s_index] ;
      if (sp)
      {
        tcpip_error_ind(sp->app_handle, TCPIP_RESULT_NETWORK_LOST,
                        sp->s_index) ;
        sockpar_delete(sp) ;
      }
    }
    tcpip_data->is_initialized = FALSE ;
  }
}




/*==== Primitive handler functions ==========================================*/


/** Handle a TCPIP_INITIALIZE_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_initialize_req(void *primdata)
{
  U8 result ;                   /* Result code of initialization. */
  
  TRACE_FUNCTION("tcpip_initialize_req()") ;

  /* The variable should be optimized away by the compiler, but it looks
   * clearer with the initialization call on a separate line.
   */
  result = tcpip_do_initialization() ;
  tcpip_initialize_cnf(result) ;
  PFREE(primdata) ;
}


/** Handle a TCPIP_SHUTDOWN_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_shutdown_req(void *primdata)
{
  TRACE_FUNCTION("tcpip_shutdown_req()") ;

  tcpip_do_shutdown() ;
  tcpip_shutdown_cnf(TCPIP_RESULT_OK) ;
  PFREE(primdata) ;
}


/** Handle a TCPIP_IFCONFIG_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_ifconfig_req(void *primdata)
{
  T_TCPIP_IFCONFIG_REQ *prim ;
  NGifnet *netp ;               /* Pointer to network interface struct. */
  NGuint local_addr ;           /* Local address of interface (host byte
                                 * order). */
  NGuint dest_addr ;            /* Destination address (always zero in our
                                 * case). */
  NGuint netmask ;
  U8 result = TCPIP_RESULT_INTERNAL_ERROR ; /* Result code of operation. */

  /* We don't jump through all the hoops of constructing a message in a
   * message and sending it to the network interface control function, but
   * rather twiddle the necessary bits by ourselves. This saves quite some
   * code and is lots easier to read. */
  TRACE_FUNCTION("tcpip_ifconfig_req()") ;

  prim = (T_TCPIP_IFCONFIG_REQ *) primdata ;

  /* First, find the network interface. This turned out to be surprisingly
   * easy. :-) */
  netp = &rnet_rt_env_ctrl_blk_p->ifnet_dti.dti_ifnet ;
  
  switch (prim->if_up)
  {
    case TCPIP_IFCONFIG_DOWN:
      TRACE_EVENT("ifconfig down") ;
      netp->if_flags &= ~NG_IFF_UP ;
      /* Lint loves the void: */
      (void) ngProto_IP.pr_cntl_f(NG_CNTL_SET, NG_IPO_NETDOWN, netp) ;
      result = TCPIP_RESULT_OK ;
      break ;
    case TCPIP_IFCONFIG_UP:
      netp->if_flags |= NG_IFF_UP ;
      netp->if_mtu = prim->mtu_size ;
      local_addr = prim->ipaddr ;
      dest_addr = TCPIP_UNSPECIFIED_IPADDR ;
      netmask = 0xffffffff ;

      if (tcpip_data->config_dns_address)
      {
        TRACE_EVENT("override dnsaddr1 by address from config primitive") ;
        prim->dnsaddr1 = tcpip_data->config_dns_address ;
      }        

      TRACE_EVENT_P3("ifconfig %08x dns %08x, %08x up",
                     ngNTOHL(local_addr),
                     ngNTOHL(prim->dnsaddr1), ngNTOHL(prim->dnsaddr2)) ;
      
      ngIfGenCntl(netp, NG_CNTL_SET, NG_IFO_ADDR, &local_addr) ;
      ngIfGenCntl(netp, NG_CNTL_SET, NG_IFO_DSTADDR, &dest_addr) ;
      ngIfGenCntl(netp, NG_CNTL_GET, NG_IFO_NETMASK, &netmask) ;
      (void) ngProto_IP.pr_cntl_f(NG_CNTL_SET, NG_IPO_ROUTE_DEFAULT,
                                  &local_addr);
      (void) ngProto_RESOLV.pr_cntl_f(NG_CNTL_SET, NG_RSLVO_SERV1_IPADDR,
                                      &prim->dnsaddr1) ;
      (void) ngProto_RESOLV.pr_cntl_f(NG_CNTL_SET, NG_RSLVO_SERV2_IPADDR,
                                      &prim->dnsaddr2) ;
      result = TCPIP_RESULT_OK ;
      break ;
    default:
      TRACE_ERROR("ifconfig: bogus prim->if_up value") ;
      result = TCPIP_RESULT_INVALID_PARAMETER ;
      break ;
  }

#ifdef _SIMULATION_
  tcpip_if_properties(netp) ;
#endif  /* _SIMULATION_ */
  
  tcpip_ifconfig_cnf(result) ;
  PFREE(primdata) ;
}


/** Handle a TCPIP_DTI_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_dti_req(void *primdata)
{
  T_TCPIP_DTI_REQ *prim ;

  TRACE_FUNCTION("tcpip_dti_req()") ;

  prim = (T_TCPIP_DTI_REQ *) primdata ;
  if (prim->dti_direction EQ TCPIP_DTI_TO_LOWER_LAYER)
  {
    tcpip_data->ll[0].link_id = prim->link_id ;

    if(prim->dti_conn == TCPIP_CONNECT_DTI)
    {
      if(dti_open(tcpip_data->dti_handle,
                  0,  /* instance */
                  prim->dti_direction, 
                  0,                /* channel */
                  TCPIP_DTI_QUEUE_SIZE, 
                  prim->dti_direction, 
                  DTI_QUEUE_WATERMARK,
                  DTI_VERSION_10,
#ifdef _SIMULATION_
                  "SND",
#else
                  (U8 *) prim->entity_name,
#endif
                  prim->link_id) != TRUE)
	    {
        TRACE_ERROR("dti_open returns with error") ;
	    }
    }
    else
    {
      dti_close(tcpip_data->dti_handle,0,prim->dti_direction,0,FALSE);
      // TCPIP_DISCONNECT_CNF is sent here, because the DTI callback is not called
      // after DTI2_DISCONNECT_REQ was sent (no CNF-primitive)
      tcpip_dti_cnf(TCPIP_DISCONNECT_DTI,prim->link_id);
    }
  }
  else
  {
    TRACE_ERROR("DTI link to other than upper layer not (yet) supported!") ;
  }    
  /* The result will be signalled by DTI. */
  PFREE(primdata) ;
}


/** Handle a TCPIP_CREATE_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_create_req(void *primdata)
{
  T_TCPIP_CREATE_REQ *prim = primdata ;
  T_RNET_RET retval ;
  T_RNET_DESC *sdesc ;          /* The socket descriptor. */
  T_RV_RETURN_PATH retpath = { 0, tcpip_rnet_callback } ;
  T_sockpar *sockpar ;          /* Pointer to socket parameter struct ; */

  TRACE_FUNCTION("tcpip_create_req()") ;
  
  TRACE_EVENT_P1("Calling rnet_new() for ipproto %d", prim->ipproto) ;
  retval = rnet_new((T_RNET_IPPROTO) prim->ipproto, &sdesc, retpath) ;
  TRACE_EVENT_P1("rnet_new() returns %d", retval) ;

  if (retval EQ RNET_OK)
  {
    sockpar = sockpar_new(sdesc, prim->app_handle, prim->ipproto, 0) ;
    TRACE_EVENT_P1("New socket is %d", sockpar->s_index) ;
    if (sockpar EQ NULL)
    {
      tcpip_create_cnf(prim->app_handle, TCPIP_RESULT_INTERNAL_ERROR,
                       0, prim->request_id) ;
      rnet_close(sdesc) ;
    }
    else
    {
      rnet_set_user_data(sdesc, (void *) sockpar) ;
      tcpip_create_cnf(prim->app_handle, TCPIP_RESULT_OK,
                       sockpar->s_index, prim->request_id) ;
    }
  }
  else
  {
    tcpip_create_cnf(prim->app_handle, rnet_error_to_tcpip_result(retval),
                     0, prim->request_id) ;
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_CLOSE_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_close_req(void *primdata)
{
  T_TCPIP_CLOSE_REQ *prim = primdata ;
  T_RNET_RET retval ;           /* Return value of rnet_close(). */
  T_sockpar *sockpar ;          /* Socket parameter block. */
  T_RNET_DESC *sdesc ;          /* Socket descriptor. */

  TRACE_FUNCTION("tcpip_close_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_close_req()") ;
    tcpip_close_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                    prim->socket) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    sockpar = SOCKPAR_GET(sdesc) ;
    TRACE_EVENT_P1("Calling rnet_close() for socket %d", sockpar->s_index) ;
    retval = rnet_close(sdesc) ;
    TRACE_EVENT_P1("rnet_close() returns %d", retval) ;
    tcpip_close_cnf(prim->app_handle,
                    (U8) ((retval EQ RNET_OK) ?
                          TCPIP_RESULT_OK : rnet_error_to_tcpip_result(retval)),
                    prim->socket) ;
    sockpar_delete(sockpar) ;
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_BIND_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_bind_req(void *primdata)
{
  T_TCPIP_BIND_REQ *prim = primdata ;
  T_RNET_RET retval ;           /* Return value of rnet_bind(). */
  T_RNET_DESC *sdesc ;          /* Socket descriptor. */

  TRACE_FUNCTION("tcpip_bind_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_bind_req()") ;
    tcpip_bind_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                   prim->socket) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    TRACE_EVENT_P2("Calling rnet_bind() for socket %d port %d",
                   prim->socket, prim->port) ;
    retval = rnet_bind(sdesc, TCPIP_UNSPECIFIED_IPADDR,
                       (U16) ngNTOHS(prim->port)) ;
    TRACE_EVENT_P1("rnet_bind() returns %d", retval) ;
    tcpip_bind_cnf(prim->app_handle,
                   (U8) ((retval EQ RNET_OK) ?
                         TCPIP_RESULT_OK : rnet_error_to_tcpip_result(retval)),
                   prim->socket) ;
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_LISTEN_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_listen_req(void *primdata)
{
  T_TCPIP_LISTEN_REQ *prim = primdata ;
  T_RNET_RET retval ;           /* Return value of rnet_listen(). */
  T_RNET_DESC *sdesc ;          /* Socket descriptor. */

  TRACE_FUNCTION("tcpip_listen_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_listen_req()") ;
    tcpip_listen_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                    prim->socket) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    TRACE_EVENT_P1("Calling rnet_listen() for socket %d", prim->socket) ;
    retval = rnet_listen(sdesc) ;
    TRACE_EVENT_P1("rnet_listen() returns %d", retval) ;
    switch (retval)
    {
      case RNET_OK:
        socket_expect_event(sdesc, TCPIP_EVT_CONNECT_IND) ;
        tcpip_listen_cnf(prim->app_handle, TCPIP_RESULT_OK, prim->socket) ;
        break ;
      default:
        tcpip_listen_cnf(prim->app_handle, rnet_error_to_tcpip_result(retval),
                         prim->socket) ;
        break ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_CONNECT_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_connect_req(void *primdata)
{
  T_TCPIP_CONNECT_REQ *prim = primdata ;
  T_RNET_RET retval ;           /* Return value of rnet_connect(). */
  T_RNET_DESC *sdesc ;          /* Socket descriptor. */
  T_sockpar *sockpar ;          /* Socket parameter block. */

  TRACE_FUNCTION("tcpip_connect_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_connect_req()") ;
    tcpip_connect_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                    prim->socket) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    socket_expect_event(sdesc, TCPIP_EVT_CONNECT_CNF) ;
    TRACE_EVENT_P3("Calling rnet_connect() for socket %d ipaddr %x port %d",
                   prim->socket, ngNTOHL(prim->ipaddr),
                   ngNTOHS(prim->port)) ;
    retval = rnet_connect(sdesc, ngNTOHL(prim->ipaddr),
                          (U16) ngNTOHS(prim->port)) ;
    TRACE_EVENT_P1("rnet_connect() returns %d", retval) ;

    sockpar = SOCKPAR_GET(sdesc) ;
    switch (sockpar->ipproto)
    {
      case TCPIP_IPPROTO_TCP:
        if (retval EQ RNET_OK)
        {
          TRACE_EVENT("wait... TCPIP_CONNECT_CNF");
          /* Wait for the result of the connect; we will send a
           * TCPIP_CONNECT_CNF then. */
        }
        else
        {
          tcpip_connect_cnf(prim->app_handle,
                            rnet_error_to_tcpip_result(retval), prim->socket) ;
          socket_expect_event(sdesc, 0) ;
        }
        break ;
      case TCPIP_IPPROTO_UDP:
        sockpar->is_connected = TRUE ;
        tcpip_connect_cnf(prim->app_handle, rnet_error_to_tcpip_result(retval),
                          prim->socket) ;
        socket_expect_event(sdesc, 0) ;
        break ;
      default:
        TRACE_ERROR("unknown protocol in tcpip_connect_req()!?") ;
        break ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_DATA_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_data_req(void *primdata)
{
  T_TCPIP_DATA_REQ *prim = primdata ;
  T_sockpar *sockpar ;
  T_RNET_DESC *sdesc ;          /* Socket descriptor. */

  TRACE_FUNCTION("tcpip_data_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    tcpip_data_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                    prim->socket, 0) ;
    TRACE_ERROR("Invalid socket index in tcpip_data_req()") ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    sockpar = SOCKPAR_GET(sdesc) ;
    
    if (sockpar->send.total_length)
    {
      /* We haven't sent the previous block completely, but the API already
       * sends more data. That must be an error -- either an error of the
       * socket API or we have sent out a TCPIP_DATA_CNF too early. */
      TRACE_ERROR("tcpip_data_req: new data although old data is still left") ;
      tcpip_data_cnf(prim->app_handle, TCPIP_RESULT_INTERNAL_ERROR,
                     prim->socket, 0) ;
    }
    else if (!sockpar->is_connected AND
             (sockpar->ipproto EQ TCPIP_IPPROTO_TCP
              OR prim->ipaddr EQ 0
              OR prim->port EQ 0))
    {
      /* Application tried to send on a non-connected TCP socket or a
       * non-connected UDP socket without specifying IP address and port
       * number. RNET or, respectively, NexGenIP does for some reason not
       * catch this error, so we do it here.
       */
      tcpip_data_cnf(prim->app_handle, TCPIP_RESULT_NOT_CONNECTED,
                     prim->socket, 1) ;
    }
    else                        /* Finally ok. */
    {
      sockpar->send.total_length = prim->buflen ;
      sockpar->send.offset = 0 ;
#ifdef _SIMULATION_
      prim->data = tcpip_sim_fake_data(prim->socket, prim->buflen) ;
#endif /* _SIMULATION_ */
      sockpar->send.buffer = (U8 *) prim->data ;
      tcpip_try_send_data(sockpar) ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_DATA_RES primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_data_res(void *primdata)
{
  T_TCPIP_DATA_RES *prim = primdata ;
  T_sockpar *sockpar ;

  TRACE_FUNCTION("tcpip_data_res()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    /* Do nothing -- what *could* we do in response to a response? */
    TRACE_ERROR("Invalid socket index in tcpip_data_res()") ;
  }
  else
  {
    TRACE_EVENT("switch flow control towards application to xon") ;
    sockpar = sock_table[prim->socket] ;
    sockpar->appl_xoff = FALSE ;  
    dti_start(tcpip_data->dti_handle, 0, TCPIP_DTI_TO_LOWER_LAYER, 0) ; // when receive the application data confrim then allow SNDCP send next data package 05132008 
                         // OMAPS00172999 fix 
    if (sockpar->recv_waiting)
    {
      tcpip_read_incoming_to_app(sockpar) ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_SOCKNAME_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_sockname_req(void *primdata)
{
  T_TCPIP_SOCKNAME_REQ *prim = primdata ;
  T_RNET_RET retval ;
  T_RNET_IP_ADDR ipaddr ;
  T_RNET_PORT port ;
  T_RNET_DESC *sdesc ;          /* The socket descriptor. */

  TRACE_FUNCTION("tcpip_sockname_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_sockname_req()") ;
    tcpip_sockname_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                       prim->socket, TCPIP_UNSPECIFIED_IPADDR,
                       TCPIP_UNSPECIFIED_PORT) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    TRACE_EVENT_P1("Calling rnet_get_local_addr_port() for socket %d",
                   prim->socket) ;
    retval = rnet_get_local_addr_port(sdesc, &ipaddr, &port) ;
    TRACE_EVENT_P3("rnet_get_local_addr_port() returns %d, "
                   "ipaddr %d port %d", retval, ipaddr, port) ;
    switch (retval)
    {
      case RNET_OK:
        tcpip_sockname_cnf(prim->app_handle, TCPIP_RESULT_OK, prim->socket,
                           ngHTONL(ipaddr), (U16) ngHTONS(port)) ;
        break ;
      default:
        tcpip_sockname_cnf(prim->app_handle, rnet_error_to_tcpip_result(retval),
                           prim->socket, TCPIP_UNSPECIFIED_IPADDR,
                           TCPIP_UNSPECIFIED_PORT) ;
        break ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_PEERNAME_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_peername_req(void *primdata)
{
  T_TCPIP_PEERNAME_REQ *prim = primdata ;
  T_RNET_RET retval ;
  T_RNET_DESC *sdesc ;          /* The socket descriptor. */
  NGsockaddr addr ;             /* Socket address struct. */
  int optlen ;                  /* Length of option (address struct). */

  TRACE_FUNCTION("tcpip_peername_req()") ;
  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_peername_req()") ;
    tcpip_peername_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                       prim->socket, TCPIP_UNSPECIFIED_IPADDR,
                       TCPIP_UNSPECIFIED_PORT) ;
  }
  else
  {
    sdesc = SOCK_RT_DESC(prim->socket) ;
    optlen = sizeof(addr) ;
    TRACE_EVENT_P1("Calling ngSAIOGetOption() for peername socket %d",
                   prim->socket) ;
    retval = (T_RNET_RET) ngSAIOGetOption((NGsock *) sdesc, NG_IOCTL_SOCKET, 
                          NG_SO_PEERNAME, &addr, &optlen) ;
    TRACE_EVENT_P3("ngSAIOGetOption() returns %d ipaddr %x port %d (net order)",
                   retval, addr.sin_addr, addr.sin_port) ;
    switch (rnet_rt_ngip_error(retval))
    {
      case RNET_OK:
        tcpip_peername_cnf(prim->app_handle, TCPIP_RESULT_OK, prim->socket,
                           addr.sin_addr, addr.sin_port) ;
        break ;
      default:
        tcpip_peername_cnf(prim->app_handle, rnet_error_to_tcpip_result(retval),
                           prim->socket, TCPIP_UNSPECIFIED_IPADDR,
                           TCPIP_UNSPECIFIED_PORT) ;
        break ;
    }
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_HOSTINFO_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_hostinfo_req(void *primdata)
{
  T_TCPIP_HOSTINFO_REQ *prim = primdata ;
  T_RV_RETURN_PATH retpath = { 0, tcpip_hostinfo_callback } ;

  /* Trick: We use the primitive itself as user data for the RNET call. This
   * way we do not have to allocate extra memory to save the request_id and
   * the app_handle. */
 
	emo_printf("TRICKYDICKY: %08X", primdata);
 
  TRACE_FUNCTION("tcpip_hostinfo_req()") ;
  
  switch (rnet_get_host_info((char *) prim->hostname, ngNTOHL(prim->ipaddr),
                             retpath, primdata))
  {
    case RNET_OK:
      /* We now wait for the hostinfo callback being called. */
      break ;
    case RNET_MEMORY_ERR:
      tcpip_hostinfo_cnf(prim->app_handle, TCPIP_RESULT_OUT_OF_MEMORY,
                         prim->request_id, NULL, TCPIP_UNSPECIFIED_IPADDR) ;
      PFREE(prim) ;
      break ;
    default:                    /* Unexpected error code. */
      tcpip_hostinfo_cnf(prim->app_handle, TCPIP_RESULT_INTERNAL_ERROR,
                         prim->request_id, NULL, TCPIP_UNSPECIFIED_IPADDR) ;
      PFREE(prim) ;
      break ;
  }
  /* Do *not* PFREE(primdata) -- the primitive is used as userdata for the
   * RNET call. */
}


/** Handle a TCPIP_MTU_SIZE_REQ primitive from the Socket API.
 * 
 * @param primdata   Data part of the primitive.
 */
void tcpip_mtu_size_req(void *primdata)
{
  T_TCPIP_MTU_SIZE_REQ *prim = primdata ;

  TRACE_FUNCTION("tcpip_mtu_size_req()") ;

  if (INVALID_S_INDEX(prim->socket))
  {
    TRACE_ERROR("Invalid socket index in tcpip_mtu_size_req()") ;
    tcpip_mtu_size_cnf(prim->app_handle, TCPIP_RESULT_INVALID_PARAMETER,
                       prim->socket, TCPIP_DEFAULT_MTU_SIZE) ;
  }
  else
  {
    /* The MTU size is usually not negotiated between the network and the
     * mobile station. It is guaranteed, though, that it is not less than 1500
     * bytes, and that is what we report here. This might be changed to some
     * "real" value queried from the interface when the need comes up.
     */  
    tcpip_mtu_size_cnf(prim->app_handle, TCPIP_RESULT_OK, prim->socket,
                       TCPIP_DEFAULT_MTU_SIZE) ;
  }
  PFREE(primdata) ;
}


/** Handle a TCPIP_INTERNAL_IND primitive sent by TCPIP itself.
 * 
 * @param primdata   Data part of the primitive.
 * @return 
 */
void tcpip_internal_ind(void *primdata)
{
  T_TCPIP_INTERNAL_IND *prim = primdata ;
  T_RVM_RETURN retval ;

  TRACE_FUNCTION("tcpip_internal_ind()") ;

  TRACE_EVENT_P1("received TCPIP_INTERNAL_IND id %d", prim->msg_id) ;
  retval = rnet_rt_handle_message((T_RV_HDR *) prim->msg_p) ;
  if (retval != RV_OK)
  {
    TRACE_EVENT_P1("rnet_rt_handle_message() returned %d", retval) ;
  }
  PFREE(primdata) ;
}

/* EOF */
