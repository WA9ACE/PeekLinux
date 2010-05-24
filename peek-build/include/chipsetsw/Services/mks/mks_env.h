/**
 * @file   mks_env.h
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
 *  11/16/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _MKS_ENV_H_
#define _MKS_ENV_H_

#include "rvm/rvm_gen.h"
#include "mks/mks_cfg.h"

#include "mks/mks_pool_size.h"	/* Stack & Memory Bank sizes definitions */

/**
 * Mailbox ID used by the SWE.
 */
#define MKS_MAILBOX_USED    RVF_TASK_MBOX_0


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 */
/*@{*/
#define MKS_MB_PRIM_SIZE      (MKS_NB_MAX_OF_KEY_SEQUENCE*(2*KPD_MAX_CHAR_NAME + MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE + 50))
#define MKS_MB_PRIM_WATERMARK (MKS_MB_PRIM_SIZE)
/*@}*/

#if ((!defined DEPENDENCY) && (MKS_MB_PRIM_SIZE != MKS_MB1_SIZE))
  /* If the MKS Memory Bank sizes defined in rvf_pool_size.h differs from */
  /* this computed one, compilation is STOPPED. */
  #error MKS Memory Bank size mismatch!
#endif


/** Define a structure used to store all the informations related to MKS's task
 *   & MBs identifiers.
 */
typedef struct
{
   T_RVF_MB_ID      prim_id;
   T_RVF_ADDR_ID   addr_id;
   T_RVM_RETURN   (*error_ft)(T_RVM_NAME        swe_name, 
                              T_RVM_RETURN      error_cause,
                              T_RVM_ERROR_TYPE  error_type,
                              T_RVM_STRING      error_msg);
} T_MKS_ENV_CTRL_BLK;




/**
 * @name Generic functions
 *
 * Generic functions declarations.
 */
/*@{*/


T_RVM_RETURN mks_get_info(T_RVM_INFO_SWE  *infoSWE);

T_RVM_RETURN mks_set_info( T_RVF_ADDR_ID  addr_id,
                           T_RV_RETURN    return_path[],
                           T_RVF_MB_ID    bk_id[],
                           T_RVM_RETURN   (*rvm_error_ft)(T_RVM_NAME swe_name, 
                                                          T_RVM_RETURN error_cause, 
                                                          T_RVM_ERROR_TYPE error_type, 
                                                          T_RVM_STRING error_msg));

T_RVM_RETURN mks_init(void);

T_RVM_RETURN mks_stop(void);

T_RVM_RETURN mks_kill(void);

/*@}*/


#endif /* #ifndef _MKS_ENV_H_ */
