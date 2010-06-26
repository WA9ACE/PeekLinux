/**
 * @file    rnet_rt_atp_i.h
 *
 * ATP point-to-point network interface for RNET_RT
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_atp_i.h,v 1.3 2002/10/30 15:23:34 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  5/6/2002    Regis Feneon  Create
 *  6/9/2002    Regis Feneon  moved structure
 *    T_RNET_RT_ATP_CUSTOM_INFO to rnet_atp_info.h
 * (C) Copyright 2002 by TI, All Rights Reserved
 *
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_ATP_I_H_
#define __RNET_RT_ATP_I_H_

#include "rnet_rt_i.h"
#include "atp_api.h"
#include "rnet_atp_info.h"

/** default MTU */
//#define RNET_RT_ATP_MTU     1500

/** Network interface data */
typedef struct {
  /** generic NexGenIP network interface */
  NGifnet atp_ifnet;
  /** ATP specific data: */
  T_ATP_SW_ENTITY_ID atp_sw_id;
  T_ATP_PORT_NB atp_port_nb;
  /** client SWE */
  T_ATP_SW_ENTITY_ID atp_client_sw_id;
  T_ATP_PORT_NB atp_client_port_nb;
} T_RNET_RT_ATP_IFNET;

/** Network Driver */
extern const NGnetdrv rnet_rt_netdrv_atp;

#endif

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

