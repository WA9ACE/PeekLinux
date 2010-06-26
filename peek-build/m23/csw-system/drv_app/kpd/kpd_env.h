/**
 * @file   kpd_env.h
 *
 * Declarations of the Riviera Generic Functions 
 * (except handle message and handle timer).
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/10/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _KPD_ENV_H_
#define _KPD_ENV_H_

#include "kpd/kpd_cfg.h"
#include "kpd/kpd_i.h"


#include "rvm/rvm_gen.h"

#if ((CHIPSET == 12) || (CHIPSET == 15))
#include "kpd/kpd_i.h"
#endif

#include "kpd/kpd_pool_size.h"	/* Stack & Memory Bank sizes definitions */


/**
 * Mailbox ID used by the SWE.
 */
#define KPD_MAILBOX_USED    RVF_TASK_MBOX_0


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 */
/*@{*/
#define KPD_MB_PRIM_SIZE      (KPD_MB_BASE_SIZE + KPD_MAX_SUBSCRIBER*(100+3*(KPD_NB_PHYSICAL_KEYS)))
#define KPD_MB_PRIM_WATERMARK (KPD_MB_PRIM_SIZE - 50*KPD_MAX_SUBSCRIBER)
/*@}*/

#if ((!defined DEPENDENCY) && (KPD_MB_PRIM_SIZE != KPD_MB1_SIZE))
  /* If the KPD Memory Bank sizes defined in rvf_pool_size.h differs from */
  /* this computed one, compilation is STOPPED. */
  #error KPD Memory Bank size mismatch!
#endif


/** Define a structure used to store all the informations related to KPD's task
 *   & MBs identifiers.
 */
typedef struct
{
   T_RVF_MB_ID    prim_id;
   T_RVF_ADDR_ID  addr_id;
   BOOL           swe_is_initialized;
   T_RVM_RETURN   (*error_ft)(T_RVM_NAME        swe_name, 
                              T_RVM_RETURN      error_cause,
                              T_RVM_ERROR_TYPE  error_type,
                              T_RVM_STRING      error_msg);
#if ((CHIPSET == 12) || (CHIPSET == 15))
  T_KPD_RECEIVED_KEY_INFO received_key_info[KPD_MAX_DETECTABLE];
  UINT8                   nb_active_keys;
  UINT16                  repeat_time;
  UINT16                  long_time;
#endif
} T_KPD_ENV_CTRL_BLK;

/**
 * @name Generic functions
 *
 * Generic functions declarations.
 */
/*@{*/


T_RVM_RETURN kpd_get_info(T_RVM_INFO_SWE  *infoSWE);

T_RVM_RETURN kpd_set_info( T_RVF_ADDR_ID  addr_id,
                           T_RV_RETURN    return_path[],
                           T_RVF_MB_ID    bk_id[],
                           T_RVM_RETURN   (*rvm_error_ft)(T_RVM_NAME swe_name, 
                                                          T_RVM_RETURN error_cause, 
                                                          T_RVM_ERROR_TYPE error_type, 
                                                          T_RVM_STRING error_msg));

T_RVM_RETURN kpd_init(void);

T_RVM_RETURN kpd_stop(void);

T_RVM_RETURN kpd_kill(void);

/*@}*/


#endif /* #ifndef _KPD_ENV_H_ */
