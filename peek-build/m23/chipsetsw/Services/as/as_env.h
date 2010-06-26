/**
 * @file
 *
 * @brief Declaration of the Riviera Generic Functions.
 *
 * This file contains declaration of the Riviera Generic Functions that
 * are used by the Riviera Manager to deal with the AS SWE.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_ENV_H_
#define __AS_ENV_H_


#include "rvm/rvm_gen.h"
#include "rvm/rvm_priorities.h"


/// Priority of the host task for the SWE.
#define AS_TASK_PRIORITY RVM_AS_TASK_PRIORITY


/**
 * @defgroup as_env Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE.
 * @{
 */

/// Get AS SWE information.
extern T_RVM_RETURN
as_env_get_info(T_RVM_INFO_SWE* info_swe);

/// Set environment information for AS SWE.
extern T_RVM_RETURN
as_env_set_info(T_RVF_ADDR_ID    addr_id,
                T_RV_RETURN_PATH return_path[],
                T_RVF_MB_ID      bk_id_table[],
                T_RVM_CB_FUNC    call_back_error_ft);

/// Initialize global data for AS SWE.
extern T_RVM_RETURN
as_env_init(void);

/// Start the AS SWE (in AS context).
extern T_RVM_RETURN
as_env_start(void);

/// Handle received messages.
extern T_RVM_RETURN
as_env_handle_message(T_RV_HDR* hdr);

/// Handle expired timers.
extern T_RVM_RETURN
as_env_handle_timer(T_RV_HDR* hdr);

/// Request to stop the AS SWE.
extern T_RVM_RETURN
as_env_stop(T_RV_HDR* hdr);

/// Reclaim resources allocated by AS SWE.
extern T_RVM_RETURN
as_env_kill(void);

/** @} */


#endif // __AS_ENV_H_
