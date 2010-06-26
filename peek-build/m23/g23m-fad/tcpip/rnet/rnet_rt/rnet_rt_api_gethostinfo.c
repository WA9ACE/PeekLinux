/**
 * @file    rnet_rt_api_gethostinfo.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_gethostinfo.c,v 1.5 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/22/2002   Regis Feneon  Create
 *  4/4/2002    Regis Feneon  implementation
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"
#include "rnet_message.h"
#include "tcpip_int.h"

/*
 * Resolver asynchronous callback.
 * Called by the resolver when a query is completed,
 * a message with the result is sent back to the application.
 */

static void gethost_cb( int code, T_RNET_RT_GETHOSTINFO *data_p)
{
  T_RNET_HOST_INFO msg ;
  T_RNET_HOST_INFO *msg_p = &msg ;
  T_RNET_IP_ADDR addr;

#if 0
  /* allocate message for reply */
  msg_p = NULL;
  if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof( T_RNET_HOST_INFO),
     RNET_HOST_INFO, (T_RV_HDR **) &msg_p) == RVF_RED) {
    RNET_RT_SEND_TRACE("RNET_RT: get_host_info: cannot allocate MSG",
      RV_TRACE_LEVEL_ERROR);
  }
  else {
#endif /* 0 */
    /* prepare message */
    if( code == NG_EDNS_OK) {
      /* get address, convert to host-byte order */
      memcpy( &addr, data_p->hostent.h_addr_list[0], sizeof(T_RNET_IP_ADDR));
      msg_p->host_addr = ngNTOHL( addr);
      /* get full name */
      strncpy( msg_p->host_name, data_p->hostent.h_name,
        RNET_MAX_HOST_NAME_LEN-1);
      /* null-terminate host_name */
      msg_p->host_name[RNET_MAX_HOST_NAME_LEN-1] = 0;
      msg_p->error = RNET_OK;
    }
    else {
      /* host not found... */
      msg_p->host_addr = RNET_IP_ADDR_ANY;
      msg_p->host_name[0] = 0;
      switch( code) {
      case NG_EDNS_HOST_NOT_FOUND:
      case NG_EDNS_NO_ADDRESS:
      case NG_EDNS_NO_RECOVERY:
      case NG_EDNS_TRY_AGAIN:
        msg_p->error = RNET_HOST_NOT_FOUND;
        break;
      case NG_EDNS_PROTO_SHUTDOWN:
        msg_p->error = RNET_NOT_READY;
        break;
      case NG_EDNS_TIMEDOUT:
        msg_p->error = RNET_TIMEOUT;
        break;
      default:
        msg_p->error = rnet_rt_ngip_error( code);
        break;
      }
    }
    msg_p->user_data = data_p->user_data;
    tcpip_hostinfo_callback(msg_p) ;
#if 0
    if( data_p->return_path.callback_func != NULL) {
      /* Send the message using callback function. */
      data_p->return_path.callback_func( msg_p);
    }
    else {
      /* Send the message using mailbox. */
      rvf_send_msg( data_p->return_path.addr_id, (T_RV_HDR *) msg_p);
    }
  }
#endif /* 0 */

  /* release data buffer */
/*   PatternVibrator("o20f10", 1); */
  rvf_free_buf( data_p);
/*   PatternVibrator("o20f10", 0); */
  return;
}

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
 * @return  RNET_MEMORY_ERR  Not enough memory is available.
 *      RNET_OK      The message could be correctly send.
 */
T_RNET_RET rnet_rt_get_host_info (char *name,
                  T_RNET_IP_ADDR addr,
                  T_RV_RETURN_PATH return_path,
                  void * user_data)
{
  T_RNET_RT_GETHOSTINFO *data_p;
  T_RNET_IP_ADDR naddr;
  int err;

  /* check if SW entity has been initialised */
  if( rnet_rt_env_ctrl_blk_p == NULL) {
    return( RNET_NOT_INITIALIZED);
  }

  if( ((name == NULL) && (addr == RNET_IP_ADDR_ANY)) ||
      ((name != NULL) && (addr != RNET_IP_ADDR_ANY))) {
    return( RNET_INVALID_PARAMETER);
  }

  /* allocate callback data */
  if( rvf_get_buf( rnet_rt_env_ctrl_blk_p->mb_id,
    sizeof( T_RNET_RT_GETHOSTINFO), (T_RVF_BUFFER **)&data_p) == RVF_RED) {
    RNET_RT_SEND_TRACE("RNET_RT: get_host_info: cannot allocate data ",
      RV_TRACE_LEVEL_WARNING);
    return( RNET_MEMORY_ERR);
  }

  /* save return path and user data */
  data_p->return_path = return_path;
  data_p->user_data = user_data;

  /* send query */
  if( name == NULL) {
    naddr = ngHTONL( addr);
    rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

    err = ngResolvByAddrAsync( NG_AF_INET,
                               (void *) naddr, /* XXX bug in ngresolv XXX */
                               &data_p->hostent,
                               data_p->tmpbuf,
                               RNET_RT_NGIP_GETHOSTINFO_BUFMAX,
                               0,
                               (NGslv_cb_f) gethost_cb,
                               data_p);
    rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  }
  else {
    rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
    err = ngResolvByNameAsync( NG_AF_INET,
                               name,
                               &data_p->hostent,
                               data_p->tmpbuf,
                               RNET_RT_NGIP_GETHOSTINFO_BUFMAX,
                               0,
                               (NGslv_cb_f) gethost_cb,
                               data_p);
    rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  }

  switch( err) {
  case NG_EDNS_WOULDBLOCK:
    /* message will be sent later */
    return( RNET_OK);
  case NG_EDNS_OK:
  case NG_EDNS_HOST_NOT_FOUND:
  case NG_EDNS_NO_ADDRESS:
  case NG_EDNS_NO_RECOVERY:
  case NG_EDNS_TIMEDOUT:
  case NG_EDNS_TRY_AGAIN:
    /* entry was in cache, send message now */
    gethost_cb( err, data_p);
    return( RNET_OK);
  case NG_EDNS_PROTO_SHUTDOWN:
    /* invalid configuration */
    RNET_RT_SEND_TRACE("RNET_RT: get_host_info: invalid configuration.",
      RV_TRACE_LEVEL_WARNING);
    err = RNET_NOT_READY;
    break;
  default:
    RNET_RT_SEND_TRACE("RNET_RT: get_host_info: default used.",
      RV_TRACE_LEVEL_WARNING);
    err = rnet_rt_ngip_error( err);
    break;
  }
  /* release buffer and return error code */
  rvf_free_buf( data_p);
  return((T_RNET_RET)err);
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

