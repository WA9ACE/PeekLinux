/**
 * @file  mpk_env.h
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
 *  5/30/2003 Dennis Gallas ()    Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __MPK_ENV_H_
#define __MPK_ENV_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */


/**
 * @name Generic functions
 *
 * Generic functions declarations needed for a Type 1 SWE
 * (Group Member SWE).
 */
/*@{*/
extern T_RVM_RETURN mpk_get_info (T_RVM_INFO_SWE  *info_swe);

extern T_RVM_RETURN mpk_set_info ( T_RVF_ADDR_ID addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID   bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft);

extern T_RVM_RETURN mpk_init (void);

extern T_RVM_RETURN mpk_stop (void);
extern T_RVM_RETURN mpk_kill (void);

/* Type 1 specific generic functions */
extern T_RVM_RETURN mpk_start (void);
/* End of specific */

/*@}*/

#endif /*__MPK_ENV_H_*/
