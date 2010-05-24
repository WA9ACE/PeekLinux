/**
 * @file	hash_cfg.h
 *
 * Configuration definitions for the SHA instance.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen (ICT)		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_CFG_H_
#define __HASH_CFG_H_


#include "rv/rv_general.h"         /* General Riviera definitions. */
#include "hash/hash_pool_size.h"    /* Stack & Memory Bank sizes definitions */


#define HASH_AUTO_IDLE_DISABLE (0)

#ifndef __HASH_HW_FUNCTIONS_H_
#define HASH_AUTO_IDLE_ENABLE  (1)
#endif

#define HASH_AUTO_IDLE          HASH_AUTO_IDLE_DISABLE



/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 */
/*@{*/
#define HASH_MB_PRIM_SIZE                HASH_MB1_SIZE
#define HASH_MB_PRIM_WATERMARK           (HASH_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_HASH_TASK_PRIORITY
#define HASH_TASK_PRIORITY               RVM_HASH_TASK_PRIORITY
#else
#define HASH_TASK_PRIORITY               170
#endif


#endif /* __HASH_CFG_H_ */
