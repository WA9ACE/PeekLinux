/**
 * @file   lls_env.h
 *
 * Declarations of the Riviera Generic Functions 
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  03/12/2002     L Sollier    Create
 *
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _LLS_ENV_H_
#define _LLS_ENV_H_

#include "rvm/rvm_gen.h"

#include "lls/lls_pool_size.h"	/* Stack & Memory Bank sizes definitions */

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 */
/*@{*/
#define LLS_MB_PRIM_SIZE      LLS_MB1_SIZE
#define LLS_MB_PRIM_WATERMARK LLS_MB_PRIM_SIZE
/*@}*/


/** Define a structure used to store all the informations related to LLS's task
 *   & MBs identifiers.
 */
typedef struct
{
   T_RVF_MB_ID    prim_id;
   BOOL           swe_is_initialized;
   T_RVM_RETURN   (*error_ft)(T_RVM_NAME        swe_name,
                              T_RVM_RETURN      error_cause,
                              T_RVM_ERROR_TYPE  error_type,
                              T_RVM_STRING      error_msg);
   T_RVF_MUTEX mutex; /* Mutex used to protect perform only one action simultaneously */
} T_LLS_ENV_CTRL_BLK;




/**
 * @name Generic functions
 *
 * Generic functions declarations.
 */
/*@{*/


T_RVM_RETURN lls_get_info(T_RVM_INFO_SWE  *infoSWE);

T_RVM_RETURN lls_set_info( T_RVF_ADDR_ID  addr_id,
                           T_RV_RETURN    return_path[],
                           T_RVF_MB_ID    bk_id[],
                           T_RVM_RETURN   (*rvm_error_ft)(T_RVM_NAME swe_name, 
                                                          T_RVM_RETURN error_cause, 
                                                          T_RVM_ERROR_TYPE error_type, 
                                                          T_RVM_STRING error_msg));

T_RVM_RETURN lls_init(void);

T_RVM_RETURN lls_start(void);

T_RVM_RETURN lls_stop(void);

T_RVM_RETURN lls_kill(void);

/*@}*/


#endif /* #ifndef _LLS_ENV_H_ */
