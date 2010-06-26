/**
 * @file	tty_env.h
 *
 * Declarations of the Riviera Generic Functions 
 *
 * @author	Frederic Turgis (f-turgis@ti.com) & Gerard Cauvy (g-cauvy@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  01/27/2003	Create
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __TTY_ENV_H_
#define __TTY_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "tty/tty_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 */
/*@{*/
#define TTY_MB_PRIM_SIZE				TTY_MB1_SIZE
#define TTY_MB_PRIM_WATERMARK			(TTY_MB_PRIM_SIZE - 256)
/*@}*/


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a type 2 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN tty_get_info (T_RVM_INFO_SWE  *info_swe);

T_RVM_RETURN tty_set_info ( T_RVF_G_ADDR_ID  addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID      bk_id_table[],
							T_RVM_CB_FUNC    call_back_error_ft);

T_RVM_RETURN tty_init (void);

T_RVM_RETURN tty_stop (void);

T_RVM_RETURN tty_kill (void);
/*@}*/

#endif /*__TTY_ENV_H_*/

