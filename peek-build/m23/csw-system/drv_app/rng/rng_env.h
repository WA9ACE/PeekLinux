/**
 * @file	rng_env.h
 *
 * Declarations of the Riviera Generic Functions 
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/10/2003	  ()   		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __RNG_ENV_H_
#define __RNG_ENV_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 1 SWE
 * (Group Member SWE).
 */
/*@{*/
T_RVM_RETURN rng_get_info (T_RVM_INFO_SWE  *info_swe);

T_RVM_RETURN rng_set_info ( T_RVF_ADDR_ID	addr_id,
							T_RV_RETURN_PATH return_path[],
							T_RVF_MB_ID		bk_id_table[],
							T_RVM_CB_FUNC	call_back_error_ft);

T_RVM_RETURN rng_init (void);

T_RVM_RETURN rng_stop (void);
T_RVM_RETURN rng_kill (void);

/* Type 1 specific generic functions */
T_RVM_RETURN rng_start (void);
/* End of specific */

/*@}*/

#endif /*__RNG_ENV_H_*/
