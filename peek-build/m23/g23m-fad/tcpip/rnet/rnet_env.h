/**
 * @file  rnet_env.h
 *
 * Riviera NET.
 *
 * Declarations of the Riviera Generic Functions
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date         Author        Modification
 *  -------------------------------------------------------------------
 *  01/29/2002  Vincent Oberle    Create
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_ENV_H_
#define __RNET_ENV_H_

#include "rvm_gen.h" /* Generic RVM types and functions. */
#include "rvf_pool_size.h" /* Stack & Memory Bank sizes definitions */

/**
 * @name Memory bank size and stack size
 *
 * Memory bank size and watermark.
 * Wished task stack in bytes.
 */
/*@{*/
#ifdef RVF_RNET_MB1_SIZE
#define RNET_MB_PRIM_SIZE RVF_RNET_MB1_SIZE
#else
#define RNET_MB_PRIM_SIZE 500
#endif
#define RNET_MB_PRIM_WATERMARK (RNET_MB_PRIM_SIZE - 100)
/*@}*/

/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a type 2 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN rnet_get_info (T_RVM_INFO_SWE *info_swe);

T_RVM_RETURN rnet_set_info (T_RVF_ADDR_ID addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft);

T_RVM_RETURN rnet_init (void);
T_RVM_RETURN rnet_start (void);

T_RVM_RETURN rnet_stop (void);
T_RVM_RETURN rnet_kill (void);
/*@}*/

#endif /* __RNET_ENV_H_ */

