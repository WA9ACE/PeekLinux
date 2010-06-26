/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Module   : MPK
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This module implements the process body interface
|             for the entity MPK
|
|             Exported functions:
|
|             pei_create    - Create the Protocol Stack Entity
|             pei_init      - Initialize Protocol Stack Entity
|             pei_primitive - Process Primitive
|             pei_timeout   - Process Timeout
|             pei_exit      - Close resources and terminate
|             pei_run       - Process Primitive
|             pei_config    - Dynamic Configuration
|             pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/ 

#include "mpk/mpk_i.h"

#include "mpk/mpk_env.h"

/**
 * Called by the RV manager to initialize the 
 * mpk SWE before creating the task and calling mpk_start. 
 *
 * @return  RVM_OK
 */
T_RVM_RETURN mpk_init (void)
{
  MPK_SEND_TRACE("MPK: init called", RV_TRACE_LEVEL_DEBUG_LOW);

  return RVM_OK;
}


/**
 * Called by the RV manager to start the MPK SWE,
 * Note: Only for type 1, 2 and 3 SWE.
 *
 * @return  RVM_OK.
 */
T_RVM_RETURN mpk_start(void)
{
  MPK_SEND_TRACE("MPK: start called", RV_TRACE_LEVEL_DEBUG_LOW);

  return RVM_OK;
}

T_RVM_RETURN mpk_get_info(T_RVM_INFO_SWE  * swe_info)
{
  /* Used for info */
    swe_info->type_info.type1.version = BUILD_VERSION_NUMBER(MPK_MAJOR,MPK_MINOR,MPK_BUILD);

	return RVM_OK;
}


