/**
 * @file    rnet_rt_api_getproto.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_getproto.c,v 1.3 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/29/2002   Regis Feneon  Create
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

/*
 * Retrieves the protocol associated to a connection descriptor.
 * The protocol is retrieved with the SO_TYPE socket option.
 *
 * @param  desc  Connection identifier.
 * @return  A value of the T_RNET_IPPROTO enumeration.
 */

T_RNET_IPPROTO rnet_rt_get_proto (T_RNET_DESC *desc)
{
  int err, len, type;

  len = sizeof( type);

  /* get type of socket */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOGetOption( (NGsock *) desc, NG_IOCTL_SOCKET, NG_SO_TYPE,
    &type, &len);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  if( err == NG_EOK) {
    switch( type) {
    case NG_SOCK_STREAM:
      return( RNET_IPPROTO_TCP);
    case NG_SOCK_DGRAM:
      return( RNET_IPPROTO_UDP);
    default:
      break;
    }
  }

  /* should not happen... */
  return( RNET_IPPROTO_UNDEFINED);
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

