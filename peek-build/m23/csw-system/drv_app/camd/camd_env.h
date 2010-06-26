/**
 * @file	camd_env.h
 *
 * Declarations of the Riviera Generic Functions 
 *
 * @author	raymond zandbergen (raymond.zandbergen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen (raymond.zandbergen@ict.nl)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __CAMD_ENV_H_
#define __CAMD_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 2 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN camd_get_info (T_RVM_INFO_SWE * info_swe);

T_RVM_RETURN camd_set_info (T_RVF_ADDR_ID addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID bk_id_table[],
							T_RVM_CB_FUNC call_back_error_ft);

T_RVM_RETURN camd_init (void);

T_RVM_RETURN camd_stop (T_RV_HDR *);
T_RVM_RETURN camd_kill (void);

/* Type 2 specific generic functions */
T_RVM_RETURN camd_start (void);
T_RVM_RETURN camd_handle_message (T_RV_HDR * msg);
T_RVM_RETURN camd_handle_timer (T_RV_HDR * msg);
/* End of specific */

/*@}*/

#endif /*__CAMD_ENV_H_*/
