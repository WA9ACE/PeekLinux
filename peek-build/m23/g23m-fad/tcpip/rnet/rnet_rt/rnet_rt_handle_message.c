/**
 * @file  rnet_rt_handle_message.c
 *
 * RNET_RT handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_handle_message.c,v 1.6 2002/10/30 15:23:34 rf Exp $
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
#include "rnet_trace_i.h"

#ifdef RNET_RT_ATP_SUPPORT
#include "atp_messages.h"
#endif

/**
 * Called every time the SW entity is in WAITING state
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param  msg_p  Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */

T_RVM_RETURN rnet_rt_handle_message( T_RV_HDR *msg_p)
{

  if( msg_p != NULL) {

    switch( msg_p->msg_id) {

    case RNET_RT_NGIP_INPUT:
      /* call NexGenIP input processing */
      rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      rnet_rt_ngip_input( ((T_RNET_RT_NGIP_INPUT *) msg_p)->bufp);
      rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      break;

#ifdef RNET_RT_ATP_SUPPORT

    case ATP_OPEN_PORT_IND:
    case ATP_OPEN_PORT_CFM:
    case ATP_PORT_CLOSED:
    case ATP_TXT_CMD_RDY:
    case ATP_CMD_RDY:
    case ATP_DATA_RDY:
    case ATP_NO_COPY_DATA_RDY:
    case ATP_SIGNAL_CHANGED:
    case ATP_PORT_MODE_CHANGED:
      /* forward the message to the ATP interface */
      rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      ((NGifnet *)&rnet_rt_env_ctrl_blk_p->ifnet_atp)->if_cntl_f(
        (NGifnet *)&rnet_rt_env_ctrl_blk_p->ifnet_atp,
        NG_CNTL_SET,
        NG_RNETIFO_HANDLE_MSG,
        msg_p);
      rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      break;

#endif

    case RNET_RT_NGIP_NETIF_MSG:
      /* encapsulated message for a network interface driver (ATP) */
      /* forward the message to the interface */
      rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      (void)(((T_RNET_RT_NGIP_NETIF_MSG *) msg_p)->netp->if_cntl_f(
        ((T_RNET_RT_NGIP_NETIF_MSG *) msg_p)->netp,
        NG_CNTL_SET,
        NG_RNETIFO_HANDLE_MSG,
        ((T_RNET_RT_NGIP_NETIF_MSG *) msg_p)->msgp));
      rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
      /* release encapsulated message */
      if( (((T_RNET_RT_NGIP_NETIF_MSG *) msg_p)->msgp != NULL) &&
          (rvf_free_msg( ((T_RNET_RT_NGIP_NETIF_MSG *) msg_p)->msgp) != RVF_OK)) {
        RNET_RT_SEND_TRACE("RNET_RT: handle_message: Unable to free interface message",
                RV_TRACE_LEVEL_ERROR);
      }
      break;

    default:
      RNET_RT_SEND_TRACE("RNET_RT: handle_message: Unknown message",
              RV_TRACE_LEVEL_WARNING);
      break;
    }

    /* Free message */
    if( rvf_free_msg( msg_p) != RVF_OK) {
      RNET_RT_SEND_TRACE("RNET_RT: handle_message: Unable to free message",
              RV_TRACE_LEVEL_ERROR);
      return( RVM_MEMORY_ERR);
    }
  }

  return( RVM_OK);
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

