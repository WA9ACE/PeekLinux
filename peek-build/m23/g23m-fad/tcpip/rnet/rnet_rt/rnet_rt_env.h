/**
 * @file  rnet_rt_env.h
 *
 * Declarations of the Riviera Generic Functions
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
 *
 * (C) Copyright 2002 by TI, All Rights Reserved
 */

#include "rnet_cfg.h"
#ifdef RNET_CFG_REAL_TRANSPORT

#ifndef __RNET_RT_ENV_H_
#define __RNET_RT_ENV_H_

#include "rvm_gen.h" /* Generic RVM types and functions. */
#include "rvf_pool_size.h"

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * Size and watermark are not defined statically,but dynamically
 */

/** length of pool of network buffers */
#define RNET_RT_BUFPOOL_SIZE RVF_RNET_RT_MB1_BUFPOOL_SIZE

/** part of memory pool reserved for messages */
#define RNET_RT_MSGPOOL_SIZE RVF_RNET_RT_MB1_MSGPOOL_SIZE

/** stack size of RNET task */
#define RNET_RT_STACK_SIZE RVF_RNET_RT_STACK_SIZE

#define RNET_RT_MB_PRIM_SIZE RVF_RNET_RT_POOL_SIZE
#define RNET_RT_MB_PRIM_WATERMARK ( RVF_RNET_RT_POOL_SIZE - 128 )

/* Timer index for TCPIP Entity */
#define RNET_RT_NGIP_TIMER 0

/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 4 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN rnet_rt_get_info (T_RVM_INFO_SWE *info_swe);

T_RVM_RETURN rnet_rt_set_info ( T_RVF_ADDR_ID addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft);

T_RVM_RETURN rnet_rt_init (void);
T_RVM_RETURN rnet_rt_start (void);

T_RVM_RETURN rnet_rt_handle_message (T_RV_HDR * msg);
T_RVM_RETURN rnet_rt_handle_timer (T_RV_HDR * msg);

T_RVM_RETURN rnet_rt_stop (void);
T_RVM_RETURN rnet_rt_kill (void);
/*@}*/

#endif /*__RNET_RT_ENV_H_*/

#endif /* ifdef RNET_CFG_REAL_TRANSPORT */

