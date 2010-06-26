/*
+-----------------------------------------------------------------------------
|  Project :  GSM-F&D (8411)
|  Modul   :  BAT library
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
|  Purpose :  configuration for binary AT commands
+-----------------------------------------------------------------------------
*/
#ifndef BAT_CFG_H
#define BAT_CFG_H

#include "typedefs.h"   /* to include some defined types */
#include "l2p_types.h"
#include "gdd.h"


/*********************************************************************************
 *
 * defines
 *
 *********************************************************************************/

#define L2P_MAINTAIN_SIZE L2P_CONTROL_BLOCK_HDR_SIZE


/*********************************************************************************
 *
 * enumerations
 *
 *********************************************************************************/

typedef enum
{
  DEVICE_PACKET = 0
  /*more to be defined*/
}T_BAT_device;


/*********************************************************************************
 *
 * A description of the GDD interface
 *
 *********************************************************************************/

typedef struct
{
  T_GDD_FUNC                gdd_if; 
  T_GDD_CAP                 cap;
}T_BAT_adapter;


/*********************************************************************************
 *
 * A description of the L2P protocol Id
 *
 *********************************************************************************/

typedef struct
{
  T_L2P_SUPPORTED_PROTOCOLS protocol_id;
}T_BAT_l2p;

typedef struct
{
  T_BAT_device              device;
  T_BAT_adapter             adapter;
  T_BAT_l2p                 l2p;
}T_BAT_config;


/*********************************************************************************
 *
 * with this release the following configuration data should be used
 *
 *********************************************************************************/


extern T_BAT_config *BAT_APP_CONFIG;
void app_set_config (void);


#endif /* BAT_CFG_H */


