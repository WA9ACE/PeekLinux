/**
 * @file    rnet_rt_api_new.c
 *
 * RNET_RT API
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_api_new.c,v 1.4 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/22/2002   Regis Feneon  Create
 *  6/24/2002   Regis Feneon  RNET_SEND_CFM renamed RNET_SEND_RDY
 *                            send this message when a connection
 *                            is accepted
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_rt_env.h"
#include "rnet_message.h"


/*
 * Send a message back to the user using socket's return path
 */
static int socket_send_msg( NGsock *so, T_RV_HDR *msg)
{
  if( ((T_RNET_RT_SOCK *) so)->return_path.callback_func != NULL) {
    /* Send the message using callback function. */
    ((T_RNET_RT_SOCK *) so)->return_path.callback_func( msg);
    return( RV_OK);
  }
  else {
    /* Send the message using mailbox. */
    return( rvf_send_msg( ((T_RNET_RT_SOCK *) so)->return_path.addr_id, msg));
  }
}

NGsock *cach_so = NULL ; 

//pinghua add two function to avoid abormal sock close. OMAPS00173156 patch fix 05122008
void cach_sock_message(NGsock *so)
{

     cach_so = so ;
     return ; 	 

}

void Send_cach_message(void )
{

  union {
    T_RNET_ERROR_IND eind;
    T_RNET_CONNECT_IND cind;
    T_RNET_CONNECT_CFM ccfm;
    T_RNET_SEND_RDY srdy;
    T_RNET_RECV_IND rind;
  } *msg;

   if(cach_so==NULL)
   	return ; 
   if(cach_so->so_rcv_cc!=0)
   	return ; 
  
      RNET_RT_SEND_TRACE("RNET_RT: Send_cach_message close! ",RV_TRACE_LEVEL_ERROR);	
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_ERROR_IND),
           RNET_ERROR_IND, (T_RV_HDR **) &msg) == RVF_RED) {
          return ;
        }
        msg->eind.desc = (T_RNET_DESC *) cach_so;
        msg->eind.error = RNET_CONN_CLOSED;
        socket_send_msg( cach_so, (T_RV_HDR *) msg);
        ((T_RNET_RT_SOCK *) cach_so)->flags &= ~RNET_RT_SOCKF_NOTIFY_CLOSED;  

	cach_so= NULL; 	

}
// end 
/*
 * Socket callback function.
 * The socket events are translated to RNET messages and sent the application.
 *
 */
static void socket_callback( NGsock *so, void *data, int flags)
{
  NGsock *nso;
  NGsockaddr addr;
  NGbuf *bufp;
  char tmp[25]; 
  union {
    T_RNET_ERROR_IND eind;
    T_RNET_CONNECT_IND cind;
    T_RNET_CONNECT_CFM ccfm;
    T_RNET_SEND_RDY srdy;
    T_RNET_RECV_IND rind;
  } *msg;


   sprintf(tmp,"recv data len %d", so->so_rcv_cc);
   RNET_RT_SEND_TRACE(tmp,RV_TRACE_LEVEL_ERROR);

  msg = NULL;
  if( so->so_error != 0) {
    /* send RNET_ERROR_IND msg */
    RNET_RT_SEND_TRACE("RNET_RT: RNET_ERROR_IND ",RV_TRACE_LEVEL_ERROR);	
    if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_ERROR_IND),
       RNET_ERROR_IND, (T_RV_HDR **) &msg) == RVF_RED) {
      goto getbuf_error;
    }
    msg->eind.desc = (T_RNET_DESC *) so;
    msg->eind.error = rnet_rt_ngip_error( so->so_error);
    socket_send_msg( so, (T_RV_HDR *) msg);
    so->so_error = 0;
    return;
  }

  if( flags & NG_SAIO_READ) {
    if( so->so_state & NG_SS_CANTRCVMORE) {
      /* connection has been closed/aborted */
//pinghua add one condiction to avoid close abnormal 20080505
	  
      if( ((T_RNET_RT_SOCK *) so)->flags & RNET_RT_SOCKF_NOTIFY_CLOSED && ( so->so_rcv_cc==0 )  ) {
        /* connection is closed */
        /* send RNET_ERROR_IND/RNET_CONN_CLOSED msg */
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_ERROR_IND),
           RNET_ERROR_IND, (T_RV_HDR **) &msg) == RVF_RED) {
          goto getbuf_error;
        }
        msg->eind.desc = (T_RNET_DESC *) so;
        msg->eind.error = RNET_CONN_CLOSED;
        socket_send_msg( so, (T_RV_HDR *) msg);
        ((T_RNET_RT_SOCK *) so)->flags &= ~RNET_RT_SOCKF_NOTIFY_CLOSED;
      }
      else {              //buffer this close FIN messgae 

           cach_sock_message(so);

      	}

	  
    }
    else if( so->so_options & NG_SO_ACCEPTCONN) {
        

      /* accept a new connection */
      if( ngSAIOAccept( so, &addr, 0, &nso) == NG_EOK) {
        /* initialise rnet part of socket */
        ((T_RNET_RT_SOCK *) nso)->return_path = ((T_RNET_RT_SOCK *) so)->return_path;
        ((T_RNET_RT_SOCK *) nso)->flags =
          RNET_RT_SOCKF_NOTIFY_RECV|RNET_RT_SOCKF_NOTIFY_CLOSED;
        ((T_RNET_RT_SOCK *) nso)->user_data = NULL;
        /* install callback */
        ngSAIOSetCallback( nso, socket_callback, NULL);
        /* send RNET_CONNECT_IND msg */
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_CONNECT_IND),
           RNET_CONNECT_IND, (T_RV_HDR **) &msg) == RVF_RED) {
          ngSAIOClose( nso, 0);
          goto getbuf_error;
        }
        msg->cind.new_desc = (T_RNET_DESC *) nso;
        msg->cind.listen_desc = (T_RNET_DESC *) so;
        msg->cind.peer_addr = ngNTOHL( addr.sin_addr);
        msg->cind.peer_port = ngNTOHS( addr.sin_port);
        socket_send_msg( so, (T_RV_HDR *) msg);
        /* send first RNET_SEND_RDY message */
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_SEND_RDY),
           RNET_SEND_RDY, (T_RV_HDR **) &msg) == RVF_RED) {
          goto getbuf_error;
        }
        msg->srdy.desc = (T_RNET_DESC *) nso;
        socket_send_msg( so, (T_RV_HDR *) msg);
      }
      return;
    }
    else {
      if( ((T_RNET_RT_SOCK *) so)->flags & RNET_RT_SOCKF_NOTIFY_CONNECT) {
        /* active connection is done */
        /* send RNET_CONNECT_CFM msg */
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_CONNECT_CFM),
           RNET_CONNECT_CFM, (T_RV_HDR **) &msg) == RVF_RED) {
          goto getbuf_error;
        }
        msg->ccfm.desc = (T_RNET_DESC *) so;
        socket_send_msg( so, (T_RV_HDR *) msg);
        ((T_RNET_RT_SOCK *) so)->flags &= ~RNET_RT_SOCKF_NOTIFY_CONNECT;
        ((T_RNET_RT_SOCK *) so)->flags |= RNET_RT_SOCKF_NOTIFY_CLOSED;
        ((T_RNET_RT_SOCK *) so)->flags |= RNET_RT_SOCKF_NOTIFY_RECV ;
      }
      if( (((T_RNET_RT_SOCK *) so)->flags & RNET_RT_SOCKF_NOTIFY_RECV) &&
          (so->so_rcv_cc >= so->so_rcv_lowat)) {
        /* data has been received */
        /* send RNET_RECV_IND msg */
        if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_RECV_IND),
           RNET_RECV_IND, (T_RV_HDR **) &msg) == RVF_RED) {
          goto getbuf_error;
        }
        msg->rind.desc = (T_RNET_DESC *) so;
        /* fillin sender's address */
        if( so->so_state & NG_SS_ISCONNECTED) {
          /* socket is connected, get peer address in socket control block */
          msg->rind.peer_addr = ngNTOHL( so->so_faddr);
          msg->rind.peer_port = ngNTOHS( so->so_fport);
        }
        else if( (so->so_proto->pr_flags & NG_PR_ADDR) &&
                 ((bufp = (NGbuf *) so->so_rcv_q.qu_tail) != NULL)) {
          /* get address from last queued buffer */
          ngMemCpy( &addr, bufp->buf_iov, sizeof( NGsockaddr));
          msg->rind.peer_addr = ngNTOHL( addr.sin_addr);
          msg->rind.peer_port = ngNTOHS( addr.sin_port);
        }
        else {
          /* cannot get address, this should not happen... */
          msg->rind.peer_addr = 0;
          msg->rind.peer_port = 0;
        }
        socket_send_msg( so, (T_RV_HDR *) msg);
        ((T_RNET_RT_SOCK *) so)->flags &= ~RNET_RT_SOCKF_NOTIFY_RECV;
      }
    }
  }


  if( flags & NG_SAIO_WRITE) {
    if( !(so->so_state & NG_SS_CANTSENDMORE) &&
        (((T_RNET_RT_SOCK *) so)->flags & RNET_RT_SOCKF_NOTIFY_SEND) &&
        ((so->so_snd_hiwat - so->so_snd_cc) >= so->so_snd_lowat)) {
      /* send data has been acknowledged by peer */
      /* send RNET_SEND_RDY msg */
      if( rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id, sizeof(T_RNET_SEND_RDY),
         RNET_SEND_RDY, (T_RV_HDR **) &msg) == RVF_RED) {
        goto getbuf_error;
      }
      msg->srdy.desc = (T_RNET_DESC *) so;
      socket_send_msg( so, (T_RV_HDR *) msg);
      ((T_RNET_RT_SOCK *) so)->flags &= ~RNET_RT_SOCKF_NOTIFY_SEND;
    }
  }
  return;

getbuf_error:
  RNET_RT_SEND_TRACE("RNET_RT: cannot allocate MSG ",RV_TRACE_LEVEL_ERROR);
  return;
}

/**
 * Creates a new connection identifier (T_RNET_DESC).
 *
 * The connection ID is not active until it has either been bound
 * to a local address or connected to a remote address.
 *
 * @param  proto  Protocol that should be used [IN].
 * @param  desc  Connection identifier created [OUT].
 * @param  return_path  Return path that should be used to send
 *          the messages like accept, connect, etc [IN].
 * @return  RNET_MEMORY_ERR      No available memory to create the new connection id.
 *      RNET_NOT_INITIALIZED  NET subsystem not initialized (internal error).
 *      RNET_INTERNAL_ERR    Network subsystem failed.
 *                or  No more socket descriptors available.
 *      RNET_NOT_READY      Still processing a callback function.
 *      RNET_NOT_SUPPORTED    Specified protocol not supported.
 *      RNET_OK          Connection ID successfully created.
 */

T_RNET_RET rnet_rt_new (T_RNET_IPPROTO proto,
           T_RNET_DESC ** desc,
           T_RV_RETURN_PATH return_path)
{
  NGsock *so;
  int type, err;

  /* check if SW entity has been initialised */
  if( rnet_rt_env_ctrl_blk_p == NULL) {
    return( RNET_NOT_INITIALIZED);
  }

  /* select type of socket, only TCP and UDP allowed */
  switch( proto) {
  case RNET_IPPROTO_TCP:
    type = NG_SOCK_STREAM;
    break;
  case RNET_IPPROTO_UDP:
    type = NG_SOCK_DGRAM;
    break;
  default:
    /* invalid protocol */
    /* could create raw IP socket... */
    return( RNET_NOT_SUPPORTED);
  }

  /* create ngip socket */
  
  rvf_lock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);
  err = ngSAIOCreate( &so, NG_AF_INET, type, 0, NG_O_NONBLOCK);

  if( err == NG_EOK) {
    /* initialise rnet part of socket */
    ((T_RNET_RT_SOCK *) so)->return_path = return_path;
#if 0 /* RNET_RT bug!? [ni 2003-12-05 */
    ((T_RNET_RT_SOCK *) so)->flags = RNET_RT_SOCKF_NOTIFY_RECV |
                                           RNET_RT_SOCKF_NOTIFY_SEND;
#else
    if (proto == RNET_IPPROTO_UDP) {
      /* A UDP socket is ready for communication right after it has been
       * created, ... */
      ((T_RNET_RT_SOCK *) so)->flags =
        RNET_RT_SOCKF_NOTIFY_RECV | RNET_RT_SOCKF_NOTIFY_SEND;
    } else {
      /* ... whereas a TCP socket has to be connected first. */
      ((T_RNET_RT_SOCK *) so)->flags = 0 ;
    }
#endif /* 0 */
    ((T_RNET_RT_SOCK *) so)->user_data = NULL;

    
    /* install callback */
    ngSAIOSetCallback( so, socket_callback, NULL);


  }
  rvf_unlock_mutex( &rnet_rt_env_ctrl_blk_p->mutex);

  /* returns new socket */
  if( err == NG_EOK) {
    *desc = (T_RNET_DESC *) so;
    return( RNET_OK);
  }

  /* convert error code */
  return( rnet_rt_ngip_error( err));
}

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

