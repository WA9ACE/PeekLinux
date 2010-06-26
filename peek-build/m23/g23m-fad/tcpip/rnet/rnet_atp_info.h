/**
 * @file    rnet_atp_info.h
 *
 * ATP point-to-point network interface for RNET_RT
 * Custom information
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_atp_info.h,v 1.1 2002/09/06 08:39:52 rf Exp $
 * $Name:  $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  6/9/2002    Regis Feneon  Create
 *
 */
#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_ATP_INFO_H_
#define __RNET_RT_ATP_INFO_H_

#include "rnet_ip_addr.h"
#include "dcfg_api.h"

#define RNET_RT_ATP_MTU 1500

/** ATP custom information */
typedef struct {

  T_ATP_CUSTOM_TYPE custom_type;
  /** local (MS) IP address */
  T_RNET_IP_ADDR local_addr;
  /** destination (GGSN) IP address, this is also the default router */
  T_RNET_IP_ADDR dest_addr;
  /** maximum transmission unit */
  UINT16 mtu;
  /** primary DNS server address */
  T_RNET_IP_ADDR dns1;
  /** secondary DNS server address */
  T_RNET_IP_ADDR dns2;
  /* Identity of the user of the channel */
  T_APPLI_USER IPU_id;
} T_RNET_RT_ATP_CUSTOM_INFO;

#endif
#endif

