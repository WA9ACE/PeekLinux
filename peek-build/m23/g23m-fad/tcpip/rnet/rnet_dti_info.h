/* 
+------------------------------------------------------------------------------
|  File:       rnet_dti_info.h
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
|  Purpose :  DTI point-to-point network interface for RNET_RT,
|             Custom information
+----------------------------------------------------------------------------- 
*/ 

/* derived from... */
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

#ifndef __RNET_RT_DTI_INFO_H_
#define __RNET_RT_DTI_INFO_H_

#include "rnet_ip_addr.h"
#include "dcfg_api.h"

#define RNET_RT_DTI_MTU 1500

/*
 * DTI custom information
 */
typedef struct
{
  T_RNET_IP_ADDR local_addr ;   /* Local (MS) IP address. */
  T_RNET_IP_ADDR dest_addr ;    /* Destination (GGSN) IP address, this is also
                                 * the default router. */
  UINT16 mtu ;                  /* Maximum transmission unit. */
  T_RNET_IP_ADDR dns1 ;         /* Primary DNS server address. */
  T_RNET_IP_ADDR dns2 ;         /* Secondary DNS server address. */
  T_APPLI_USER IPU_id ;         /* Identity of the user of the channel. */
}
T_RNET_RT_DTI_CUSTOM_INFO ;

#endif /* __RNET_RT_DTI_INFO_H_ */
#endif /* RNET_CFG_REAL_TRANSPORT */

/* EOF */
