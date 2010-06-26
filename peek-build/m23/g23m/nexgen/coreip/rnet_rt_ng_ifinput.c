/**
 * @file    rnet_rt_ng_ifinput.c
 *
 * Riviera RNET - NexGenIP Generic Interface Functions
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/14/2002   Regis Feneon  Riviera version
 *  4/8/2002    Regis Feneon  changed meaning of fromisr argument
 *
 */

/*****************************************************************************
 * $Id: rnet_rt_ng_ifinput.c,v 1.3 2002/04/30 12:44:46 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Generic Interface Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngIfGenInput()
 *----------------------------------------------------------------------------
 * 08/02/2000 - Regis Feneon
 * 19/07/2000 -
 *  added fromisr argument
 *****************************************************************************/

#include "rnet_rt_i.h"
#include "rnet_trace_i.h"

#include "tcpip_int.h"

/*****************************************************************************
 * ngIfGenInput()
 *****************************************************************************
 * Enqueue input buffer in global input queue
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 *  bufp        received message buffer
 *  fromisr     WARNING: changed from original NexGenIP code:
 *                zero to generate a NGIP_INPUT message
 *                non-zero to directly call ngip_input
 * Return value: 0 or error code
 */

void ngIfGenInput( NGifnet *netp, NGbuf *bufp, int fromisr)
{
  T_RVF_MB_STATUS mb_status;
  T_RNET_RT_NGIP_INPUT *msg;

  if( fromisr == 0) {

    /* allocate a NGIP_INPUT message */
    RNET_RT_SEND_TRACE("RNET_RT: In ngIfGenInput, fromisr = 0 ",RV_TRACE_LEVEL_WARNING);

    msg = NULL;
    mb_status = rvf_get_msg_buf( rnet_rt_env_ctrl_blk_p->mb_id,
      sizeof(T_RNET_RT_NGIP_INPUT), RNET_RT_NGIP_INPUT, (T_RV_HDR **) &msg);
    if( (mb_status != RVF_GREEN) && (mb_status != RVF_YELLOW)) {
      RNET_RT_SEND_TRACE("RNET_RT: cannot allocate MSG ",RV_TRACE_LEVEL_WARNING);
      netp->if_iqdrops++;
      /* release buffer */
      NG_QUEUE_IN( &ngBuf_freeq, bufp);
    }
    else {
      /* set the buffer interface pointer */
      bufp->buf_ifnetp = netp;
      /* update statistics */
      netp->if_ipackets++;
      netp->if_ibytes += bufp->buf_datalen;
      if( bufp->buf_flags & (NG_BUFF_BCAST|NG_BUFF_MCAST)) {
        netp->if_imcasts++;
      }
      /* send message for input processing */
      msg->bufp = bufp;
      /* rvf_send_msg( rnet_rt_env_ctrl_blk_p->addr_id, (T_RV_HDR *) msg); */
      tcpip_send_internal_ind((ULONG) msg, msg->hdr.msg_id) ;
    }

  }
  else {

    /* direct call to ngip_input */
    RNET_RT_SEND_TRACE("RNET_RT: In ngIfGenInput, fromisr <> 0 ",RV_TRACE_LEVEL_WARNING);

    /* set the buffer interface pointer */
    bufp->buf_ifnetp = netp;
    /* update statistics */
    netp->if_ipackets++;
    netp->if_ibytes += bufp->buf_datalen;
    if( bufp->buf_flags & (NG_BUFF_BCAST|NG_BUFF_MCAST)) {
      netp->if_imcasts++;
    }
    /* call input processing function */
    rnet_rt_ngip_input( bufp);

  }

}

