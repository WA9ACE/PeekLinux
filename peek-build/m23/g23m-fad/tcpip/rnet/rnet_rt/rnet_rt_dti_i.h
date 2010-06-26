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
|  Purpose :  Global definitions for DTI-based GSM/GPRS network driver for
|             RNET_RT (a.k.a. NexGenIP)
+----------------------------------------------------------------------------- 
*/ 

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_DTI_I_H_
#define __RNET_RT_DTI_I_H_

#include "rnet_rt_i.h"
#include "rnet_dti_info.h"

/** default MTU */
//#define RNET_RT_DTI_MTU     1500

/** Network interface data */
typedef struct
{
  /** generic NexGenIP network interface */
  NGifnet dti_ifnet ;
}
T_RNET_RT_DTI_IFNET ;

/** Network Driver */
extern const NGnetdrv rnet_rt_netdrv_dti ;

#endif

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

