/**
 * @file  rnet_rt_message.h
 *
 * Data structures:
 * 1) used to send messages to the RNET_RT SWE,
 * 2) RNET_RT can receive.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * $Id: rnet_rt_message.h,v 1.2 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  3/29/2002 Regis Feneon      NexGenIP messages
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_MESSAGE_H_
#define __RNET_RT_MESSAGE_H_

#include "rv_general.h"

#include "rnet_rt_cfg.h"

#include "ngip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * The message offset must differ for each SWE in order to have
 * unique msg_id in the system.
 */
#define RNET_RT_MESSAGE_OFFSET BUILD_MESSAGE_OFFSET(RNET_RT_USE_ID)

/**
 * @name RNET_RT_NGIP_INPUT
 */
/*@{*/
/** Message ID. */
#define RNET_RT_NGIP_INPUT (RNET_RT_MESSAGE_OFFSET | 0x001)

/** Message structure. */
typedef struct {
  /** Message header. */
  T_RV_HDR hdr;
  /** Pointer to network buffer. */
  NGbuf *bufp;
} T_RNET_RT_NGIP_INPUT;
/*@}*/

/**
 * @name RNET_RT_NGIP_IF_MSG
 */
/*@{*/
/** Message ID. */
#define RNET_RT_NGIP_NETIF_MSG (RNET_RT_MESSAGE_OFFSET | 0x002)

/** Message structure. */
typedef struct {
  /** Message header. */
  T_RV_HDR hdr;
  /** Pointer to network interface. */
  NGifnet *netp;
  /** Pointer to encapsulated message. */
  T_RV_HDR *msgp;
} T_RNET_RT_NGIP_NETIF_MSG;
/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RNET_RT_MESSAGE_H_ */

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

