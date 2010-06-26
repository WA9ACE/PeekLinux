/**
 * @file    rnet_rt_api_getuserdata.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_getuserdata.c,v 1.3 2002/10/30 15:23:34 rf Exp $
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

/**
 * Associates an application specific pointer to a connection ID.
 *
 * @param  desc    Connection identifier.
 * @param  user_data  Pointer that can be used by an application to store
 *            application specific data.
 */

void * rnet_rt_get_user_data (T_RNET_DESC *desc)
{
  void *user_data;

  /* get user data */
  //rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  user_data = ((T_RNET_RT_SOCK *) desc)->user_data;
  //rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( user_data);
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

