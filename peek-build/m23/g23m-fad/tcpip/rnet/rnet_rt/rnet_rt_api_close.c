/**
 * @file    rnet_rt_api_close.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_close.c,v 1.3 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/22/2002   Regis Feneon  Create
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"

#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"

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

T_RNET_RET rnet_rt_close (T_RNET_DESC * desc)
{
  int err;

  /* remove callback and close socket */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  ngSAIOSetCallback( (NGsock *) desc, NULL, NULL);
  err = ngSAIOClose( (NGsock *) desc, 1);
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

