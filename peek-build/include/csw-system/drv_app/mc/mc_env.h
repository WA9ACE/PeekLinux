/**
 * @file  mc_env.h
 *
 * Declarations of the Riviera Generic Functions 
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_ENV_H_
#define __MC_ENV_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 3 SWE
 * (Group Member SWE).
 */
/*@{*/
extern T_RVM_RETURN mc_get_info (T_RVM_INFO_SWE  *info_swe);

extern T_RVM_RETURN mc_set_info ( T_RVF_ADDR_ID  addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID   bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft);

T_RVM_RETURN mc_init(void);

extern T_RVM_RETURN mc_stop (T_RV_HDR * msg);
extern T_RVM_RETURN mc_kill (void);

/* Type 3 specific generic functions */
extern T_RVM_RETURN mc_start (void);
extern T_RVM_RETURN mc_handle_message (T_RV_HDR * msg);
extern T_RVM_RETURN mc_handle_timer (T_RV_HDR * msg);
/* End of specific */

/*@}*/

#endif /*__MC_ENV_H_*/
