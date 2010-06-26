/**
 * @file    rnet_rt_atp.c
 *
 * Coding of the ATP point-to-point network interface for RNET_RT,
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_atp.c,v 1.4 2002/12/05 10:02:19 rf Exp $
 * $Name:  $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  5/6/2002    Regis Feneon  Create
 *  6/9/2002    Regis Feneon  Select default route
 *    on local interface address if no destination address
 * 12/5/2002    Regis Feneon  Flow control handling
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "rnet_rt_i.h"
#include "rnet_trace_i.h"
#include "rnet_rt_atp_i.h"
#include "atp_api.h"
#include "atp_messages.h"
#include "rnet_atp_info.h"

static int atpOpen( NGifnet *netp)
{
  T_ATP_RET ret;
  T_ATP_CALLBACK return_path;
  T_ATP_ENTITY_MODE mode = { CMD_SUPPORT_OFF, TXT_MODE, COPY_ON };

  /* interface already running ? */
  if( netp->if_flags & NG_IFF_RUNNING) {
    RNET_RT_SEND_TRACE("RNET_RT: WINNET_open: already running",RV_TRACE_LEVEL_ERROR);
    return( NG_EALREADY);
  }

  /* register to ATP */
  return_path.addr_id = rnet_rt_env_ctrl_blk_p->addr_id;
  return_path.callback_func = NULL;

  ret = atp_reg( (UINT8*) "RNET_RT",
                 return_path,
                 mode,
                 &((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id);
  if( ret != RV_OK) {
    return( NG_ENODEV);
  }
  /* set a local port... */
  ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb = 1;

  netp->if_flags |= NG_IFF_RUNNING;

  /* use NG_IFF_OACTIVE flag for flow control */
  netp->if_flags &= ~NG_IFF_OACTIVE;

  return( NG_EOK);
}

static int atpClose( NGifnet *netp)
{
  /* interface should be running to be closed */
  if( !(netp->if_flags & NG_IFF_RUNNING)) {
    return( NG_EALREADY);
  }
  /* clear flags */
  netp->if_flags &= ~NG_IFF_RUNNING;

  /* deregister to ATP */
  atp_dereg( ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id);

  return( NG_EOK);
}

static int atpOutput( NGifnet *netp, NGbuf *bufp, NGuint addr)
{
  UINT32 wlen;
  int err;

//    RNET_RT_SEND_TRACE("RNET_RT: In atpOutput",RV_TRACE_LEVEL_ERROR);

  /* point-to-point interfaces dont use destination address */
  ((void)addr);

  /* Interface must be up and running */
  if ( (netp->if_flags & (NG_IFF_RUNNING|NG_IFF_UP)) !=
    (NG_IFF_RUNNING|NG_IFF_UP)) {
    err = NG_ENETDOWN;
  }
  else {
    /* send IP packet to ATP if output is on... */
    /* ...else discard the packet */
    if( !(netp->if_flags & NG_IFF_OACTIVE)) {
      atp_send_data( ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                     ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                     bufp->buf_datap,
                     bufp->buf_datalen,
                     &wlen);
    }
    err = NG_EOK;
  }
  /* release buffer */
  ngBufOutputFree( bufp);
  return( err);
}

static int atpCntl( NGifnet *netp, int cmd, int opt, void *arg)
{
  int ret;
  NGbuf *bufp;
  UINT32 rlen, left;
  UINT8 tmpbuf[16];
  T_RNET_RT_ATP_CUSTOM_INFO *cinfo;
  T_ATP_NO_COPY_INFO no_copy_info = { 0 }; /* not used */
  static const T_ATP_PORT_INFO info = {
    DATA_CONFIG,
    ATP_NO_RING_TYPE,
//  old version received from NexGen and modified 5/Dec/2002 : 0,
    ATP_RX_FLOW_UNMASK | ATP_TX_FLOW_UNMASK,
    0
  };

//    RNET_RT_SEND_TRACE("RNET_RT: in atpCntl",RV_TRACE_LEVEL_ERROR);

  if( opt == NG_RNETIFO_HANDLE_MSG) {
    /* handle ATP messages */
    switch( ((T_RV_HDR *)arg)->msg_id) {
    case ATP_OPEN_PORT_IND:

    RNET_RT_SEND_TRACE("RNET_RT: receive OPEN_PORT_IND",RV_TRACE_LEVEL_ERROR);
      cinfo = (T_RNET_RT_ATP_CUSTOM_INFO *)
        ((T_ATP_OPEN_PORT_IND *) arg)->custom_info_p;
      /* if interface is already up, or if no associated data,
         open is not accepted */
      if( ((netp->if_flags & (NG_IFF_UP|NG_IFF_RUNNING)) != NG_IFF_RUNNING) ||
          (cinfo == NULL)) {

        atp_open_port_rsp( ((T_ATP_OPEN_PORT_IND *) arg)->initiator_id,
                           ((T_ATP_OPEN_PORT_IND *) arg)->initiator_port_nb,
                           ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                           ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                           info,
                           no_copy_info,
                           NULL,
                           OPEN_PORT_NOK);
      }
      else {

        /* send confirmation to ATP client */
        ((T_RNET_RT_ATP_IFNET *) netp)->atp_client_sw_id =
          ((T_ATP_OPEN_PORT_IND *) arg)->initiator_id;
        ((T_RNET_RT_ATP_IFNET *) netp)->atp_client_port_nb =
          ((T_ATP_OPEN_PORT_IND *) arg)->initiator_port_nb;

    no_copy_info.packet_mode = NORMAL_PACKET; /* No L2CAP packet... */
    no_copy_info.rx_mb = no_copy_info.tx_mb = RVF_INVALID_MB_ID;

        ret = atp_open_port_rsp( ((T_ATP_OPEN_PORT_IND *) arg)->initiator_id,
                                 ((T_ATP_OPEN_PORT_IND *) arg)->initiator_port_nb,
                                 ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                                 ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                                 info,
                                 no_copy_info,
                                 NULL,
                                 OPEN_PORT_OK);
        if( ret == RV_OK) {

          /* interface is ready to send and receive packets */
          netp->if_flags |= NG_IFF_UP;
          /* set MTU */
          netp->if_mtu = cinfo->mtu;
          /* Activating the link addresses */
          ngIfGenCntl( netp, NG_CNTL_SET, NG_IFO_ADDR, &cinfo->local_addr);
          ngIfGenCntl( netp, NG_CNTL_SET, NG_IFO_DSTADDR, &cinfo->dest_addr);
          /* set default route */
          if( cinfo->dest_addr == 0) {

            (void)(ngProto_IP.pr_cntl_f( NG_CNTL_SET, NG_IPO_ROUTE_DEFAULT,
              (NGuint *) &cinfo->local_addr));
          }
          else {

           (void)(ngProto_IP.pr_cntl_f( NG_CNTL_SET, NG_IPO_ROUTE_DEFAULT,
             (NGuint *) &cinfo->dest_addr));
          }
          /* set DNS configuration */
          (void)(ngProto_RESOLV.pr_cntl_f( NG_CNTL_SET, NG_RSLVO_SERV1_IPADDR, &cinfo->dns1));
          (void)(ngProto_RESOLV.pr_cntl_f( NG_CNTL_SET, NG_RSLVO_SERV2_IPADDR, &cinfo->dns2));
        }
      }
      /* release custom information buffer */
      if( cinfo != NULL) {
        atp_free_buffer( cinfo);
      }

      break;
    case ATP_PORT_CLOSED:
    RNET_RT_SEND_TRACE("RNET_RT: receive PORT_CLOSED",RV_TRACE_LEVEL_ERROR);
      if( (netp->if_flags & (NG_IFF_RUNNING|NG_IFF_UP)) !=
        (NG_IFF_RUNNING|NG_IFF_UP)) {
        /* interface already down */
        break;
      }
      /* set interface down */
      netp->if_flags &= ~NG_IFF_UP;
      /* call ip layer */
      (void)(ngProto_IP.pr_cntl_f( NG_CNTL_SET, NG_IPO_NETDOWN, netp));
      break;
    case ATP_DATA_RDY:
      /* allocate buffer for receiving new packet */
    RNET_RT_SEND_TRACE("RNET_RT: receive DATA_RDY",RV_TRACE_LEVEL_ERROR);
      ngBufAlloc( bufp);
      if( bufp != NULL) {
        /* get new data */
        bufp->buf_flags |= NG_PROTO_IP;
        bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset;
        ret = atp_get_data( ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                            ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                            (UINT8 *) bufp->buf_datap,
                            ngBufDataMax,
                            &rlen,
                            &left);
        if( (ret != RV_OK) || (rlen == 0)) {
          netp->if_ierrors++;
          ngBufFree( bufp);
        }
      else {
//  rnet_debug[0] = '\0' ;
//  for(rnet_ind=0;rnet_ind<48;rnet_ind++) {
//    char tmp[3] ;
//    sprintf(tmp, "%.2x", bufp->buf_datap[rnet_ind]);
//    strcat(rnet_debug, tmp) ;
//  }

//    RNET_RT_SEND_TRACE("RNET_RT: receive content",RV_TRACE_LEVEL_ERROR);
//    RNET_RT_SEND_TRACE(rnet_debug, RV_TRACE_LEVEL_ERROR);
//  rvf_delay(RVF_MS_TO_TICKS(100));

          /* get length of packet */
          bufp->buf_datalen = rlen;
          /* start IP processing */
          ngIfGenInput( netp, bufp, 1);
        }
      }
      else {
        /* skip packet */
        netp->if_ierrors++;
        atp_get_data( ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                      ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                      tmpbuf,
                      sizeof( tmpbuf),
                      &rlen,
                      &left);
      }
      break;
    case ATP_SIGNAL_CHANGED:
      /* handle flow control flags */
    RNET_RT_SEND_TRACE("RNET_RT: receive SIGNAL_CHANGED",RV_TRACE_LEVEL_ERROR);
//  rvf_delay(RVF_MS_TO_TICKS(1000));
      if( ((T_ATP_SIGNAL_CHANGED *) arg)->mask & ATP_TX_FLOW_UNMASK) {
        if( ((T_ATP_SIGNAL_CHANGED *) arg)->signal & ATP_TX_FLOW_ON) {
          /* enable transimissions */
          netp->if_flags &= ~NG_IFF_OACTIVE;
        }
        else {
          /* disable transmissions */
          netp->if_flags |= NG_IFF_OACTIVE;
        }
      }
      break;
    default:
      /* unknown message */
  RNET_TRACE_MEDIUM_PARAM("RNET_RT: receive default ",((T_RV_HDR *)arg)->msg_id);

      return( NG_EINVAL);
    }
    return( NG_EOK);
  }
  else {
    RNET_RT_SEND_TRACE("RNET_RT: opt not handle msg", RV_TRACE_LEVEL_ERROR);
    return( ngIfGenCntl( netp, cmd, opt, arg));
  }
}

/********************************************************************/
/* Driver entry point */

const NGnetdrv rnet_rt_netdrv_atp = {
  "ATP",
  NG_IFT_PPP,
  NG_IFF_POINTOPOINT|NG_IFF_MULTICAST,
  RNET_RT_ATP_MTU,
  0,
  NULL,
  atpOpen,
  atpClose,
  atpOutput,
  NULL,
  atpCntl
};

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

