/**
 * @file  rnet_env.c
 *
 * Riviera NET.
 *
 * Coding of the Riviera Generic Functions,
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date         Author        Modification
 *  -------------------------------------------------------------------
 *  01/29/2002  Vincent Oberle    Create
 *  03/15/2002  Vincent Oberle    Passed in type 1
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "rnet_env.h"

#include "rvm_use_id_list.h"

#include <string.h>

#include "rnet_cfg.h"
#include "rnet_trace_i.h"

/**
 * Called by RVM to learn RNET requirements in terms of memory, SWEs...
 *
 * @param  swe_info  Pointer to the structure to fill
 *            containing infos related to the rnet SWE.
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_get_info(T_RVM_INFO_SWE * swe_info)
{
  swe_info->swe_type = RVM_SWE_TYPE_1;

  memcpy(swe_info->type_info.type1.swe_name, "RNET", sizeof("RNET"));
  swe_info->type_info.type1.swe_use_id = RNET_USE_ID;

  swe_info->type_info.type1.version = BUILD_VERSION_NUMBER(0,1,0);

  /* Memory bank info */
  swe_info->type_info.type1.nb_mem_bank = 1;
  memcpy(swe_info->type_info.type1.mem_bank[0].bank_name, "RNET_PRIM", sizeof("RNET_PRIM"));
  swe_info->type_info.type1.mem_bank[0].initial_params.size = RNET_MB_PRIM_SIZE;
  swe_info->type_info.type1.mem_bank[0].initial_params.watermark = RNET_MB_PRIM_WATERMARK;

  /* No linked SWE. */
  swe_info->type_info.type1.nb_linked_swe = 0;

#ifdef _WINDOWS
  #ifdef RNET_CFG_WINSOCK
  swe_info->type_info.type1.linked_swe_id[0] = RNET_WS_USE_ID;
  swe_info->type_info.type1.nb_linked_swe = 1;
  #elif defined RNET_CFG_REAL_TRANSPORT
  swe_info->type_info.type1.linked_swe_id[0] = RNET_RT_USE_ID;
  swe_info->type_info.type1.linked_swe_id[1] = DCM_USE_ID;
  swe_info->type_info.type1.nb_linked_swe = 2;
  #endif
#else
  #if defined RNET_CFG_BRIDGE
  swe_info->type_info.type1.linked_swe_id[0] = RNET_BR_USE_ID;
  swe_info->type_info.type1.nb_linked_swe = 1;
  #elif defined RNET_CFG_REAL_TRANSPORT
  swe_info->type_info.type1.linked_swe_id[0] = RNET_RT_USE_ID;
  swe_info->type_info.type1.linked_swe_id[1] = DCM_USE_ID;
  swe_info->type_info.type1.nb_linked_swe = 2;
  #endif
#endif

  /* Set the return path: NOT USED. */
  swe_info->type_info.type1.return_path.callback_func = NULL;
  swe_info->type_info.type1.return_path.addr_id = 0;

  /* Generic functions */
  swe_info->type_info.type1.set_info = rnet_set_info;
  swe_info->type_info.type1.init = rnet_init;
  swe_info->type_info.type1.start = rnet_start;
  swe_info->type_info.type1.stop = rnet_stop;
  swe_info->type_info.type1.kill = rnet_kill;

  return RVM_OK;
}

/**
 * Called by the RV manager to inform the RNET SWE about
 * addr_id, return path, mb_id and error function.
 *
 * It is called only once.
 *
 * @param  addr_id      Address ID of the RNET SWE.
 *              Used to send messages to the SWE.
 * @param  return_path    Return path array of the linked SWEs.
 * @param  bk_id_table    Array of memory bank ids allocated to the SWE.
 * @param  call_back_error_ft Callback function to call in case of unrecoverable error.
 * @return  RVM_MEMORY_ERR ou RVM_OK.
 */
T_RVM_RETURN rnet_set_info (T_RVF_ADDR_ID addr_id,
              T_RV_RETURN_PATH return_path[],
              T_RVF_MB_ID bk_id_table[],
              T_RVM_CB_FUNC call_back_error_ft)
{
  return RVM_OK;
}

/**
 * Called by the RV manager to initialize the
 * RNET SWE before creating the task and calling rnet_start.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_init (void)
{
  return RVM_OK;
}

/**
 * Called by the RV manager to start the RNET SWE,
 *
 * @return  RVM_OK.
 */
T_RVM_RETURN rnet_start(void)
{
  return RVM_OK;
}

/**
 * Called by the RV manager to stop the RNET SWE.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_stop (void)
{
  return RVM_OK;
}

/**
 * Called by the RV manager to kill the rnet SWE,
 * after the rnet_stop function has been called.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN rnet_kill (void)
{
  return RVM_OK;
}

