/* 
+------------------------------------------------------------------------------
|  File:          rnet_rt_dti.c
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  DTI-based GSM/GPRS network driver for RNET_RT (a.k.a. NexGenIP)
+----------------------------------------------------------------------------- 
*/ 

/* Derived from... */
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


#define RNET_RT_DTI_C
#define ENTITY_TCPIP

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#include "typedefs.h"
#include "vsi.h"
#include "dti.h"

#include "rnet_rt_i.h"
#include "rnet_trace_i.h"
#include "rnet_rt_atp_i.h"
#include "atp_api.h"
#include "atp_messages.h"
#include "rnet_atp_info.h"

#include "tcpip.h"

static int tcpip_dtiOpen(NGifnet *netp)
{
  TRACE_FUNCTION("tcpip_dtiOpen()") ;

  /* As the connection to the network is actually initiated by the
   * TCPIP_DTI_REQ from the ACI, this function is more or less a no-op. It
   * must mainly fulfil NexGenIP's formal requirements. The latter are only
   * derived from the corresponding ATP interface code for the moment.
   */
  
  /* Interface already running? */
  if (netp->if_flags & NG_IFF_RUNNING)
  {
    TRACE_ERROR("tcpip_dtiOpen: interface already opened") ;
    return NG_EALREADY ;
  }

  netp->if_flags |= NG_IFF_RUNNING ;

  /* Use NG_IFF_OACTIVE flag for flow control. */
  netp->if_flags &= ~NG_IFF_OACTIVE ;

  return NG_EOK ;
}


static int tcpip_dtiClose (NGifnet *netp)
{
  TRACE_FUNCTION("tcpip_dtiClose()") ;

  /* Interface should be running to be closed. */
  if (!(netp->if_flags & NG_IFF_RUNNING))
  {
    TRACE_ERROR("tcpip_dtiClose: interface already closed") ;
    return NG_EALREADY ;
  }

  netp->if_flags &= ~NG_IFF_RUNNING ;

  return NG_EOK ;
}


static int tcpip_dtiOutput(NGifnet *netp, NGbuf *bufp, NGuint addr)
{
  int err = NG_EOK ;

  TRACE_FUNCTION("tcpip_dtiOutput()") ;

  /* Interface must be up and running to send data. */
  if ((netp->if_flags & (NG_IFF_RUNNING|NG_IFF_UP))
      != (NG_IFF_RUNNING|NG_IFF_UP))
  {
    TRACE_ERROR("tcpip_dtiOutput: interface isn't up and running") ;
    err = NG_ENETDOWN ;
  }
  else
  {
    /* Send IP packet over DTI if output is on, else discard the packet. */
    if (tcpip_data->ll[0].flowstat_ul EQ TCPIP_FLOWCTL_XON)
    {
      tcpip_dti_send_data_ll(bufp->buf_datap, (U16) bufp->buf_datalen) ;
    }
    else
    {
      TRACE_ERROR("tcpip_dtiOutput(): DTI connection not ready or xoff") ;
      err = NG_EAGAIN ;
    }
  }

  ngBufOutputFree(bufp) ;
  return err ;
}


/** This function, while copied straight from the ATP support module, is
 * apparently unused by the DTI support. Everything it does -- handling
 * interfaces going up and down, arriving data, flow control -- is done
 * elsewhere in this DTI/GPF port of RNET. It remains to be investigated if
 * this is the right thing to do, so this function will be left in place for
 * the moment.
 */
static int tcpip_dtiCntl( NGifnet *netp, int cmd, int opt, void *arg)
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
  
  if (opt == NG_RNETIFO_HANDLE_MSG) {
    /* handle ATP messages */
    switch( ((T_RV_HDR *)arg)->msg_id) {
      case ATP_OPEN_PORT_IND:
        
        RNET_RT_SEND_TRACE("RNET_RT: receive OPEN_PORT_IND",RV_TRACE_LEVEL_ERROR);
        cinfo = (T_RNET_RT_ATP_CUSTOM_INFO *)
          ((T_ATP_OPEN_PORT_IND *) arg)->custom_info_p;
        /* if interface is already up, or if no associated data,
           open is not accepted */
        if (((netp->if_flags & (NG_IFF_UP|NG_IFF_RUNNING)) != NG_IFF_RUNNING) ||
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
          if (ret == RV_OK) {

            /* interface is ready to send and receive packets */
            netp->if_flags |= NG_IFF_UP;
            /* set MTU */
            netp->if_mtu = cinfo->mtu;
            /* Activating the link addresses */
            ngIfGenCntl( netp, NG_CNTL_SET, NG_IFO_ADDR, &cinfo->local_addr);
            ngIfGenCntl( netp, NG_CNTL_SET, NG_IFO_DSTADDR, &cinfo->dest_addr);
            TRACE_EVENT_P2("tcpip_dtiCntl cinfo address local=%08x dest%08x",
                           cinfo->local_addr, cinfo->dest_addr);
            /* set default route */
            if (cinfo->dest_addr == 0) {

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
        if (cinfo != NULL) {
          atp_free_buffer( cinfo);
        }

        break;
      case ATP_PORT_CLOSED:
        RNET_RT_SEND_TRACE("RNET_RT: receive PORT_CLOSED",RV_TRACE_LEVEL_ERROR);
        if ((netp->if_flags & (NG_IFF_RUNNING|NG_IFF_UP)) !=
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
        if (bufp != NULL) {
          /* get new data */
          bufp->buf_flags |= NG_PROTO_IP;
          bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset;
          ret = atp_get_data( ((T_RNET_RT_ATP_IFNET *) netp)->atp_sw_id,
                              ((T_RNET_RT_ATP_IFNET *) netp)->atp_port_nb,
                              (UINT8 *) bufp->buf_datap,
                              ngBufDataMax,
                              &rlen,
                              &left);
          if ((ret != RV_OK) || (rlen == 0)) {
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
        if (((T_ATP_SIGNAL_CHANGED *) arg)->mask & ATP_TX_FLOW_UNMASK) {
          if (((T_ATP_SIGNAL_CHANGED *) arg)->signal & ATP_TX_FLOW_ON) {
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

const NGnetdrv rnet_rt_netdrv_dti = {
  "DTI",
  NG_IFT_PPP,
  NG_IFF_POINTOPOINT|NG_IFF_MULTICAST,
  RNET_RT_DTI_MTU,
  0,
  NULL,
  tcpip_dtiOpen,
  tcpip_dtiClose,
  tcpip_dtiOutput,
  NULL,
  tcpip_dtiCntl
};

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

