/**
 * @file  rnet_rt_handle_timer.c
 *
 * RNET_RT handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_handle_timer.c,v 1.2 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  3/??/2002   Regis Feneon  Completed
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"

/**
 * Called every time the SW entity is in WAITING
 * state and one of its timers has expired.
 *
 * @param  Not used
 * @return  RVM_OK
 */
extern void Send_cach_message(void ) ;
T_RVM_RETURN rnet_rt_handle_timer( T_RV_HDR *msg)
{
/*  RNET_RT_SEND_TRACE("RNET_RT: timer", RV_TRACE_LEVEL_DEBUG_LOW); */
  if(rnet_rt_env_ctrl_blk_p == NULL)
  {
    RNET_RT_SEND_TRACE(" rnet_rt_handle_timer ERROR!!!!!!!!", RV_TRACE_LEVEL_ERROR);
    return( RV_NOT_INITIALISED);
  }
  Send_cach_message() ;  //pinghua addd function for OMAPS00173156 patch fix 

  /* call NexGenIP timers */
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  rnet_rt_ngip_timer();
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  rvf_start_timer( 0,
                   RVF_MS_TO_TICKS(1000/NG_TIMERHZ),
                   FALSE);     // pinghua set once timer with OMAPS00169870 by pinghua 05102008 

  return RVM_OK;
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

