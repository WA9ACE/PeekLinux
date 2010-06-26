/**
 * @file
 *
 * @brief Configuration definitions for the USBTRC instance.
 *
 * This file contains constant definitions that are used to statically
 * configure the USBTRC SWE.
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBTRC_CFG_H_
#define __USBTRC_CFG_H_


#include "rv/rv_general.h"
#include "usbtrc/usbtrc_pool_size.h"



/**
 * @name Mem bank
 *
  * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
*/

#define USBTRC_MB_PRIM_SIZE                (USBTRC_MB1_SIZE)
#define USBTRC_MB_PRIM_WATERMARK           (USBTRC_MB_PRIM_SIZE - 128)

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */

#ifdef RVM_USBTRC_TASK_PRIORITY
#define USBTRC_TASK_PRIORITY               RVM_USBTRC_TASK_PRIORITY
#else
#define USBTRC_TASK_PRIORITY               248
#endif

#endif // __USBTRC_CFG_H_
