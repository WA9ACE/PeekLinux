/****************************************************************************/
/*                                                                          */
/*  Name        rvf_env.h                                                   */
/*                                                                          */
/*  Function    this file contains rvf definitions specific for the RVM.	*/
/*                                                                          */
/*  Version		0.1															*/
/*																			*/
/* 	Date       	Modification												*/
/*  ------------------------------------									*/
/*  28/01/2000	Create														*/
/*																			*/
/*	Author		David Lamy-Charrier (dlamy@tif.ti.com)						*/
/*																			*/
/* (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#ifndef _RVF_ENV_H
#define _RVF_ENV_H

#include "rvf/rvf_api.h"


/// Return the size of the available memory in bytes.
T_RVF_RET rvf_get_available_mem( UINT32 * total_size, UINT32 * used_size );

/// Return the size of the available memory of the given pool.
T_RVF_RET rvf_get_pool_available_mem( T_RVF_POOL_ID pool_id, UINT32 * total_size, UINT32 * used_size );

/// Get parameters of a specific memory bank.
T_RVF_RET rvf_get_mb_param( T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM * param);

/// Change parameters of a specific memory bank.
T_RVF_RET rvf_set_mb_param( T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM * param);

/// Retrieve the amount of memory available before the memory bank size.
UINT32	rvf_get_mb_unused_mem(T_RVF_MB_ID mb_id);

/// Retrieve the amount of memory available before the memory bank watermark.
UINT32	rvf_get_mb_unused_green_mem(T_RVF_MB_ID mb_id);


#endif // _RVF_ENV_H

