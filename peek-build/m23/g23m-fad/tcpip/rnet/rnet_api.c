/**
 * @file  rnet_api.c
 *
 * Riviera NET.
 *
 * Code of the Riviera TCP/IP stack (non-blocking) API.
 * Platform independent code.
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
 *  03/14/2002  Vincent Oberle    Support of multiple implementations
  * 03/27/2002 Jose Yp-Tcha Adding Real Transport configuration
 *  06/24/2002  Regis Feneon    Adding RNET_RT+WINDOWS configuration
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

/* See this file for comments */
#include "rnet_api.h"

#include "rnet_trace_i.h"


#ifdef _WINDOWS
   #if defined RNET_CFG_WINSOCK
      #include "rnet_ws/rnet_ws_api.h"
   #elif defined RNET_CFG_REAL_TRANSPORT
      #include "rnet_rt/rnet_rt_api.h"
   #else
      RNET_TRACE_ERROR("RNET: No implementation chosen");
      return RNET_NOT_SUPPORTED;
   #endif
#else
   #if defined RNET_CFG_REAL_TRANSPORT
      #include "rnet_rt/rnet_rt_api.h"
   #elif defined RNET_CFG_BRIDGE
       #include "rnet_br/rnet_br_api.h"
   #endif
#endif

//#define RNET_BENCHMARKING
#ifdef RNET_BENCHMARKING

#include "timer.h"

#define MAX_FUNCTION 20
typedef enum {
new = 0,
set_traffic_class,
bind,
listen,
connect,
send,
recv,
recv_from,
shutdown,
close,
get_local_adr,
get_buff_size,
get_max_packet_size,
get_host_info,
get_proto,
set_user_data,
get_user_data
} RNET_BENCHMARK_FUNC;

struct {
  UINT16 nbtick_avg;
  UINT16 nbtick_max;
  UINT16 nbtick_min;
  UINT32 nb_calls;
  UINT8 overload;
} rnet_benchmarking[MAX_FUNCTION];

T_RNET_RET ret;

void rnet_benchmarking_start_timer(void)
{
      TM_EnableTimer( 1 );
      TM_ResetTimer( 1, 0xFFFF, 0, 0 ); // un tick = 2.4 microsecondes
      TM_StartTimer( 1 );
}

void rnet_benchmarking_stop_timer(void)
{
      TM_StopTimer( 1 );
}

UINT16 rnet_benchmarking_read_timer(void)
{
    return((0xFFFF - TM_ReadTimer(1)));
}

void rnet_benchmarking_todo(RNET_BENCHMARK_FUNC val, char *lib)
{
   UINT16 cur_timer;
   char loclib[80];

   rnet_benchmarking[val].nb_calls++;
   cur_timer = rnet_benchmarking_read_timer();
   if (cur_timer == 0)
      rnet_benchmarking[val].overload++;
   else {
      rnet_benchmarking[val].nbtick_avg = ((rnet_benchmarking[val].nbtick_avg *
                            (rnet_benchmarking[val].nb_calls-1)) + cur_timer ) /
                                            rnet_benchmarking[val].nb_calls;
      if (rnet_benchmarking[val].nbtick_max < cur_timer)
         rnet_benchmarking[val].nbtick_max = cur_timer;
      if ((rnet_benchmarking[val].nbtick_min > cur_timer) || (rnet_benchmarking[val].nbtick_min == 0))
         rnet_benchmarking[val].nbtick_min = cur_timer;
   }
   sprintf(loclib, "RNET_RT benchmarking %s nbtick_avg: %d\n", lib, rnet_benchmarking[val].nbtick_avg);
   RNET_TRACE_LOW(loclib);
   sprintf(loclib, "RNET_RT benchmarking %s nbtick_max: %d\n", lib, rnet_benchmarking[val].nbtick_max);
   RNET_TRACE_LOW(loclib);
   sprintf(loclib, "RNET_RT benchmarking %s nbtick_min: %d\n", lib, rnet_benchmarking[val].nbtick_min);
   RNET_TRACE_LOW(loclib);
   sprintf(loclib, "RNET_RT benchmarking %s nb_calls: %d\n", lib, rnet_benchmarking[val].nb_calls);
   RNET_TRACE_LOW(loclib);
   sprintf(loclib, "RNET_RT benchmarking %s overload: %d\n", lib, rnet_benchmarking[val].overload);
   RNET_TRACE_LOW(loclib);
}

#endif

/**
 * Creates a new connection identifier (T_RNET_DESC).
 */
T_RNET_RET rnet_new (T_RNET_IPPROTO proto,
           T_RNET_DESC ** desc,
           T_RV_RETURN_PATH return_path)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
       return rnet_ws_new(proto, desc, return_path);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_new(proto, desc, return_path);
     #else
        RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_new(proto, desc, return_path);
        rnet_benchmarking_todo(new, "new");
        return(ret);
#else
    return rnet_rt_new(proto, desc, return_path);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_new(proto, desc, return_path);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
 #endif
}

/**
 * Sets the traffic class of a connection ID.
 *
 * Note that this function is NOT implemented under Windows.
 */
T_RNET_RET rnet_set_traffic_class (T_RNET_DESC * desc,
                   T_RNET_TRAFFIC_CLASS traffic_class)
{
#ifdef _WINDOWS
    #if defined RNET_CFG_WINSOCK
    RNET_TRACE_ERROR("RNET: rnet_set_traffic_class not supported under Windows");
    return RNET_NOT_SUPPORTED;
    #elif defined RNET_CFG_REAL_TRANSPORT
     RNET_TRACE_ERROR("RNET: rnet_set_traffic_class not supported for step one");
     return RNET_NOT_SUPPORTED;
    #else
     RNET_TRACE_ERROR("RNET: No implementation chosen");
      return RNET_NOT_SUPPORTED;
    #endif
#else
    #if defined RNET_CFG_REAL_TRANSPORT
     RNET_TRACE_ERROR("RNET: rnet_set_traffic_class not supported for step one");
     return RNET_NOT_SUPPORTED;
    #else
      RNET_TRACE_ERROR("RNET: No implementation chosen");
     return RNET_NOT_SUPPORTED;
    #endif
#endif
}

/**
 * Binds the connection to a local IP address and port number.
 */
T_RNET_RET rnet_bind (T_RNET_DESC * desc,
            T_RNET_IP_ADDR local_addr,
            T_RNET_PORT local_port)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
      return rnet_ws_bind(desc, local_addr, local_port);
     #elif defined RNET_CFG_REAL_TRANSPORT
        return rnet_rt_bind(desc, local_addr, local_port);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
        ret = rnet_rt_bind(desc, local_addr, local_port);
        rnet_benchmarking_todo(bind, "bind");
        return(ret);
#else
        return rnet_rt_bind(desc, local_addr, local_port);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_bind(desc, local_addr, local_port);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Commands a connection to start listening for incoming connections.
 */
T_RNET_RET rnet_listen (T_RNET_DESC *desc)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
      return rnet_ws_listen(desc);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_listen(desc);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_listen(desc);
        rnet_benchmarking_todo(listen, "listen");
        return(ret);
#else
    return rnet_rt_listen(desc);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_listen(desc);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Sets up the connection ID to connect to the remote host.
 */
T_RNET_RET rnet_connect (T_RNET_DESC * desc,
             T_RNET_IP_ADDR peer_addr,
             T_RNET_PORT peer_port)
{
  #ifdef _WINDOWS
    #if defined RNET_CFG_WINSOCK
        return rnet_ws_connect(desc, peer_addr, peer_port);
    #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_connect(desc, peer_addr, peer_port);
    #else
     RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
    #endif
  #else
    #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_connect(desc, peer_addr, peer_port);
        rnet_benchmarking_todo(connect, "connect");
        return(ret);
#else
    return rnet_rt_connect(desc, peer_addr, peer_port);
#endif
    #elif defined RNET_CFG_BRIDGE
       return rnet_br_connect(desc, peer_addr, peer_port);
    #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
    #endif
  #endif
}

/**
 * Enqueues the data for sending.
 */
T_RNET_RET rnet_send (T_RNET_DESC * desc,
            T_RVF_BUFFER *buff,
            UINT16 * len_p)
{
  #ifdef _WINDOWS
      #if defined RNET_CFG_WINSOCK
       return rnet_ws_send(desc, buff, len_p);
      #elif defined RNET_CFG_REAL_TRANSPORT
     return rnet_rt_send(desc, buff, len_p);
      #else
        RNET_TRACE_ERROR("RNET: No implementation chosen");
       return RNET_NOT_SUPPORTED;
      #endif
  #else
      #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
         rnet_benchmarking_start_timer();
     ret = rnet_rt_send(desc, buff, len_p);
         rnet_benchmarking_todo(send, "send");
         return(ret);
#else
     return rnet_rt_send(desc, buff, len_p);
#endif
      #elif defined RNET_CFG_BRIDGE
       return rnet_br_send(desc, buff, len_p);
      #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
         return RNET_NOT_SUPPORTED;
      #endif
  #endif
}

/**
 * Read the waiting data.
 */
T_RNET_RET rnet_recv (T_RNET_DESC * desc,
            T_RVF_BUFFER *buff,
            UINT16 * len_p)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
      return rnet_ws_recv(desc, buff, len_p);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_recv(desc, buff, len_p);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_recv(desc, buff, len_p);
        rnet_benchmarking_todo(recv, "recv");
        return(ret);
#else
    return rnet_rt_recv(desc, buff, len_p);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_recv(desc, buff, len_p);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Read the waiting data.
 */
T_RNET_RET rnet_recv_from (T_RNET_DESC * desc,
               T_RVF_BUFFER * buff,
               UINT16 * len_p,
               T_RNET_IP_ADDR * from_addr,
               T_RNET_PORT * from_port)
{
#ifdef _WINDOWS
   #if defined RNET_CFG_WINSOCK
    return rnet_ws_recv_from(desc, buff, len_p, from_addr, from_port);
   #elif defined RNET_CFG_REAL_TRANSPORT
        return rnet_rt_recv_from(desc, buff, len_p, from_addr, from_port);
   #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
   #endif
#else
   #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
        ret = rnet_rt_recv_from(desc, buff, len_p, from_addr, from_port);
        rnet_benchmarking_todo(recv_from, "recv_from");
        return(ret);
#else
        return rnet_rt_recv_from(desc, buff, len_p, from_addr, from_port);
#endif
   #elif defined RNET_CFG_BRIDGE
    return rnet_br_recv_from(desc, buff, len_p, from_addr, from_port);
   #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
   #endif
#endif
}

/**
 * Disables the sending on a socket and informs the peer
 * about it.
 */
T_RNET_RET rnet_shutdown (T_RNET_DESC * desc)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
        return rnet_ws_shutdown(desc);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_shutdown(desc);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_shutdown(desc);
        rnet_benchmarking_todo(shutdown, "shutdown");
        return(ret);
#else
    return rnet_rt_shutdown(desc);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_shutdown(desc);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Closes the connection.
 */
T_RNET_RET rnet_close (T_RNET_DESC * desc)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
      return rnet_ws_close(desc);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_close(desc);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_close(desc);
        rnet_benchmarking_todo(close, "close");
        return(ret);
#else
    return rnet_rt_close(desc);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_close(desc);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Gets the local address and port of a connection ID.
 */
T_RNET_RET rnet_get_local_addr_port (T_RNET_DESC * desc,
                   T_RNET_IP_ADDR * local_addr,
                   T_RNET_PORT * local_port)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
        return rnet_ws_get_local_addr_port(desc, local_addr, local_port);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_get_local_addr_port(desc, local_addr, local_port);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
      return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_get_local_addr_port(desc, local_addr, local_port);
        rnet_benchmarking_todo(get_local_adr, "get_local_adr");
        return(ret);
#else
    return rnet_rt_get_local_addr_port(desc, local_addr, local_port);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_local_addr_port(desc, local_addr, local_port);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
   #endif
}

/**
 * Use to determine the amount of data pending in the network's input buffer
 * that can be read from the connection ID.
 */
T_RNET_RET rnet_get_buff_size (T_RNET_DESC * desc,
                 UINT32 * size)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
        return rnet_ws_get_buff_size(desc, size);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_get_buff_size(desc, size);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_get_buff_size(desc, size);
        rnet_benchmarking_todo(get_buff_size, "get_buff_size");
        return(ret);
#else
    return rnet_rt_get_buff_size(desc, size);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_buff_size(desc, size);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Indicates the maximum send size of a message for message-oriented
 * descriptor (UDP) as implemented by a particular service provider.
 */
T_RNET_RET rnet_get_max_packet_size (T_RNET_DESC * desc,
                   UINT32 * size)
{
#ifdef _WINDOWS
  #if defined RNET_CFG_WINSOCK
    return rnet_ws_get_max_packet_size(desc, size);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_get_max_packet_size(desc, size);
  #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
  #endif
#else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_get_max_packet_size(desc, size);
        rnet_benchmarking_todo(get_max_packet_size, "get_max_packet_size");
        return(ret);
#else
    return rnet_rt_get_max_packet_size(desc, size);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_max_packet_size(desc, size);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
#endif

}

/**
 * Requests host information corresponding to a host name or to a
 * network address.
 */
T_RNET_RET rnet_get_host_info (char *name,
                 T_RNET_IP_ADDR addr,
                 T_RV_RETURN_PATH return_path,
                 void * user_data)
{
  #ifdef _WINDOWS
    #if defined RNET_CFG_WINSOCK
    return rnet_ws_get_host_info(name, addr, return_path,user_data);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_get_host_info(name, addr, return_path,user_data);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_get_host_info(name, addr, return_path,user_data);
        rnet_benchmarking_todo(get_host_info, "get_host_info");
        return(ret);
#else

    return rnet_rt_get_host_info(name, addr, return_path,user_data);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_host_info(name, addr, return_path,user_data);
     #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #endif
}

/**
 * Retrieves the protocol associated to a connection descriptor.
 */
T_RNET_IPPROTO rnet_get_proto (T_RNET_DESC *desc)
{
  #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
        return rnet_ws_get_proto(desc);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return rnet_rt_get_proto(desc);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
  #else
     #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
    ret = rnet_rt_get_proto(desc);
        rnet_benchmarking_todo(get_proto, "get_proto");
        return(ret);
#else
    return rnet_rt_get_proto(desc);
#endif
     #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_proto(desc);
     #else
     RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_IPPROTO_UDP;
     #endif
   #endif
}

/**
 * Associates an application specific pointer to a connection ID.
 */
void rnet_set_user_data (T_RNET_DESC *desc, void *user_data)
{
   #ifdef _WINDOWS
      #if defined RNET_CFG_WINSOCK
         rnet_ws_set_user_data(desc, user_data);
      #elif defined RNET_CFG_REAL_TRANSPORT
     rnet_rt_set_user_data(desc, user_data);
      #else
        RNET_TRACE_ERROR("RNET: No implementation chosen");
       return RNET_NOT_SUPPORTED;
      #endif
   #else
      #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
        rnet_benchmarking_start_timer();
     rnet_rt_set_user_data(desc, user_data);
        rnet_benchmarking_todo(set_user_data, "set_user_data");
#else
     rnet_rt_set_user_data(desc, user_data);
#endif
      #elif defined RNET_CFG_BRIDGE
       rnet_br_set_user_data(desc, user_data);
      #else
     RNET_TRACE_ERROR("RNET: No implementation chosen");
      #endif
   #endif
}

/**
 * Returns the application specific pointer associated to the connection ID.
 */
void * rnet_get_user_data (T_RNET_DESC *desc)
{
   #ifdef _WINDOWS
     #if defined RNET_CFG_WINSOCK
        return rnet_ws_get_user_data(desc);
     #elif defined RNET_CFG_REAL_TRANSPORT
    return (void *)rnet_rt_get_user_data(desc);
     #else
       RNET_TRACE_ERROR("RNET: No implementation chosen");
    return RNET_NOT_SUPPORTED;
     #endif
   #else
      #if defined RNET_CFG_REAL_TRANSPORT
#ifdef RNET_BENCHMARKING
      void *retv;
        rnet_benchmarking_start_timer();
    retv = (void *)rnet_rt_get_user_data(desc);
        rnet_benchmarking_todo(get_user_data, "get_user_data");
        return(retv);
#else
    return (void *)rnet_rt_get_user_data(desc);
#endif
      #elif defined RNET_CFG_BRIDGE
    return rnet_br_get_user_data(desc);
      #else
    RNET_TRACE_ERROR("RNET: No implementation chosen");
    return NULL;
      #endif
   #endif
}

