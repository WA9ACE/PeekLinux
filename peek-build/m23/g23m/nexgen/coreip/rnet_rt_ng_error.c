/**
 * @file    rnet_rt_ng_error.c
 *
 * Riviera RNET - NexGenIP error codes translation
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_ng_error.c,v 1.1.1.1 2002/04/03 17:12:41 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/30/2002   Regis Feneon  Create
 *
 */

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

T_RNET_RET rnet_rt_ngip_error( int err)
{

  switch( err) {
  case NG_EOK:
  case NG_EINPROGRESS:
    return( RNET_OK);
  case NG_EMFILE:
  case NG_ENFILE:
  case NG_ENOBUFS:
  case NG_ENOMEM:
    return( RNET_MEMORY_ERR);
  case NG_EADDRNOTAVAIL:
  case NG_EBADF:
  case NG_EDESTADDRREQ:
  case NG_EINVAL:
     return( RNET_INVALID_PARAMETER);
  case NG_EAFNOSUPPORT:
  case NG_EPFNOSUPPORT:
  case NG_EPROTONOSUPPORT:
  case NG_ESOCKTNOSUPPORT:
    return( RNET_NOT_SUPPORTED);
  case NG_EADDRINUSE:
  case NG_EALREADY:
  case NG_EISCONN:
    return( RNET_IN_USE);
  case NG_EHOSTDOWN:
  case NG_EHOSTUNREACH:
  case NG_ENETDOWN:
  case NG_ENETRESET:
  case NG_ENETUNREACH:
    return( RNET_NET_UNREACHABLE);
  case NG_ETIMEDOUT:
    return( RNET_TIMEOUT);
  case NG_ECONNREFUSED:
    return( RNET_CONN_REFUSED);
  case NG_ECONNRESET:
  case NG_EPIPE:
    return( RNET_CONN_RESET);
  case NG_ECONNABORTED:
    return( RNET_CONN_ABORTED);
  case NG_EMSGSIZE:
    return( RNET_MSG_SIZE);
  case NG_EWOULDBLOCK:
    return( RNET_PARTIAL_SENT);
  default:
    return( RNET_INTERNAL_ERR);
  }
}

