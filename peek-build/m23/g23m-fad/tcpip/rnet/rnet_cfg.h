/**
 * @file  rnet_cfg.h
 *
 * Riviera NET.
 *
 * Configuration definitions for the RNET instance.
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date         Author        Modification
 *  --------------------------------------------------
 *  01/29/2002  Vincent Oberle    Create
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_CFG_H_
#define __RNET_CFG_H_

#include "rv_defined_swe.h"

/**
 * RNET underlying implementation.
 *
 * In normal usage, this doesn't need to be modified.
 * rv_defined_swe.h takes care to check that only one of the
 * RVM_RNET_xx_SWE is defined.
 */
#ifdef RVM_RNET_WS_SWE
  #define RNET_CFG_WINSOCK
#endif
#ifdef RVM_RNET_BR_SWE
  #define RNET_CFG_BRIDGE
#endif
#ifdef RVM_RNET_RT_SWE
  #ifndef RNET_CFG_REAL_TRANSPORT
    #define RNET_CFG_REAL_TRANSPORT
  #endif  /* RNET_CFG_REAL_TRANSPORT */
#endif

/**
 * Maximum size that a host name can have.
 * Increase this value to be sure not to get a truncated host name
 */
#define RNET_MAX_HOST_NAME_LEN 100 /* should be 255 see RFC... */

/**
 * Use the Loopback interface.
 */
#define RNET_RT_LOOPBACK_SUPPORT

#endif /* __RNET_CFG_H_ */

