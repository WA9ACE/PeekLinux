/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :  BAT
+-----------------------------------------------------------------------------
|  Copyright 2005 Texas Instruments Berlin, AG
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
|  Purpose :  This Modul holds the functions for configuring the
|             binary AT command library at APPlication side, the application
|             has to call this function before using the BAT Lib interface.
+-----------------------------------------------------------------------------
*/
#ifndef _BAT_CFG_C_
#define _BAT_CFG_C_

/*==== INCLUDES =============================================================*/
#include "bat_cfg.h"
#include "gdd.h"

T_BAT_config *BAT_APP_CONFIG;
/*=====FUNCTIONS=============================================================*/
void app_set_config (void)
{
  static T_BAT_config config;
  config.adapter.gdd_if = gdd_func_dio;
  config.adapter.cap.dio_cap.mtu_size = 300; /*GDD_DIO_MTU_SIZE*/
  config.device = DEVICE_PACKET;
  config.l2p.protocol_id = L2P_SP_PSI;

  BAT_APP_CONFIG = &config;
  return;
}


#endif /*_BAT_CFG_C_*/
