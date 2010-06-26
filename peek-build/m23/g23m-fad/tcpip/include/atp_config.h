/*******************************************************************************
 *
 * File Name : atp_config.h
 *
 * Configuration setting of ATP SW entity
 *
 * (C) Texas Instruments, all rights reserved
 *
 * Version number  : 0.1      Date : 28-Feb-2000
 *
 * History      : 0.1  - Created by E. Baissus
 *
 *
 * Author           : Eric Baissus : e-baissus@ti.com
 *
 *   (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved
 ******************************************************************************/
#ifndef ATP_CONFIG_H
#define ATP_CONFIG_H

#include "rvf_pool_size.h" /* Stack & Memory Bank sizes definitions */

/* Maximum number of SW entity which can register simultaneously
   register to the ATP SW entity */
#define ATP_MAX_NB_SW_ENTITY 10 // This does not includes GSM SW entity
#define ATP_SW_ENTITY_NAME_MAX_CARAC 10
#define ATP_SPP_MAX_PENDING_PORT 10 /* Maximum number of port which can be in open port
                  pending state at the same time
                  Note that a port is considered to be in pending state when
                  in server mode */

#define ATP_HSG_NAME "HSG" /* Sw entity name : used to register to ATP the HSG*/
#define ATP_GSM_NAME "GSM" /* Sw entity name of the GSM-PS used to register to the ATP */
#define ATP_SPP_NAME "SPP" /* Sw entity name : used to register to ATP the SPP */
#define ATP_HS_NAME "HS" /* Sw entity name : used to register to ATP the HS */
#define ATP_AAA_NAME "AAA" /* Sw entity name of the GSM-PS used to register to the ATP */
#define ATP_DEFAULT_GSM_MAX_PACKET_SIZE (1500) // Default value indicating the maximum size that is supported
                         // when GSM sends a packet to ATP

#define ATP_MAX_NB_OF_PORT_SUPPORTED_BY_GSM (3)
#define ATP_MAX_CMD_LENGTH (0xFF) // Maximum length of a command handled by ATP

/* Configuration parameters for environment */
#define ATP_MB_PRIM_SIZE RVF_ATP_MB1_SIZE
#define ATP_MB_PRIM_WATERMARK (ATP_MB_PRIM_SIZE - 1000)

/*
** This def is in rvm_priorities.h
** #define ATP_SWE_LEVEL (4); // SHOULD BE DEFINED IN RVM_PRIORITIES.H !!!
*/

#endif

