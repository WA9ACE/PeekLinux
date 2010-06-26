/**
 * @file
 *
 * @brief Declarations of the Riviera Generic Functions.
 *
 * This file contains declarations of the Riviera Generic Functions that
 * are used by the Riviera Manager to deal with the USBTRC SWE.
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBTRC_ENV_H_
#define __USBTRC_ENV_H_


#include "rvm/rvm_gen.h"


 /**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN usbtrc_get_info (T_RVM_INFO_SWE  *info_swe);

T_RVM_RETURN usbtrc_set_info ( T_RVF_ADDR_ID	addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID		bk_id_table[],
							T_RVM_CB_FUNC	call_back_error_ft);

T_RVM_RETURN usbtrc_init (void);


/* Type 3 specific generic functions */
/// Start the USBTRC SWE (in USBTRC context).
T_RVM_RETURN usbtrc_start(void);

/// Handle received messages.
T_RVM_RETURN usbtrc_handle_message(T_RV_HDR * msg);

/// Handle expired timers.
T_RVM_RETURN usbtrc_handle_timer(T_RV_HDR * msg);
T_RVM_RETURN usbtrc_stop (T_RV_HDR * msg);
T_RVM_RETURN usbtrc_kill (void);
/* End of specific */

/*@}*/

#endif // __USBTRC_ENV_H_
