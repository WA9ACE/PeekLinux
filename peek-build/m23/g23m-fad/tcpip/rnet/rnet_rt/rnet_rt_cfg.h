/**
 * @file  rnet_rt_cfg.h
 *
 * Configuration definitions for the RNET_RT instance.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  3/19/2002  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *  3/29/2002 Regis Feneon      NexGenIP configuration
 *  10/28/2002 Regis Feneon      added RNET_RT_ETH_SUPPORT
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_CFG_H_
#define __RNET_RT_CFG_H_

#include "rv_general.h" /* General Riviera definitions. */

/**
 * NexGenIP configuration
 */

/** number of socket descriptors */
#define RNET_RT_SOCK_MAX 32

/** maximum number of pending connections on a listening socket */
#define RNET_RT_SOCK_LISTEN_MAX 4

/** number of hosts in ARP table */
#define RNET_RT_ARP_MAX 12

/**
 * NexGenRESOLV configuration
 */

/** maximum number of simultaneous queries */
#define RNET_RT_RESOLV_QUERY_MAX 4

/** size of cache */
#define RNET_RT_RESOLV_CACHE_MAX 8

/** query timeout */
#define RNET_RT_RESOLV_TIMEOUT 1000

/** optional network interfaces */
//#define RNET_RT_LOOPBACK_SUPPORT

#define RNET_RT_DTI_SUPPORT
/* #define RNET_RT_ATP_SUPPORT */
//#define RNET_RT_ETH_SUPPORT

#endif /* __RNET_RT_CFG_H_ */

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

