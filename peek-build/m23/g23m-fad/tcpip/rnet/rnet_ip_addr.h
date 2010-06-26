/**
 * @file  ipv4/rnet_ip_addr.h
 *
 * Riviera NET.
 *
 * IPv4 address related structures.
 * See the Readme.txt file.
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date         Author        Modification
 *  --------------------------------------------------
 *  01/25/2002  Vincent Oberle    Create
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_IP_ADDR_H_
#define __RNET_IP_ADDR_H_

#include "rv_general.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name  IPv4 address.
 *
 * T_RNET_IP_ADDR is the IPv4 address type.
 *
 * The RNET_IP_ADDR_ANY value corresponds to any IP address.
 * The RNET_IP4_ADDR allows easy creation of IP addresses.
 */
/*@{*/
typedef UINT32 T_RNET_IP_ADDR;

#define RNET_IP_ADDR_ANY 0

#define RNET_IP4_ADDR(ipaddr, a,b,c,d) { \
  ipaddr = ((UINT32)(a & 0xff) << 24) | ((UINT32)(b & 0xff) << 16) | \
       ((UINT32)(c & 0xff) << 8) | (UINT32)(d & 0xff); \
}
/*@}*/

/**
 * Port. 16 bits.
 */
typedef UINT16 T_RNET_PORT;

#ifdef __cplusplus
}
#endif

#endif /* __RNET_IP_ADDR_H_ */

