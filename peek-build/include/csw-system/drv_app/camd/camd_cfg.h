/**
 * @file	camd_cfg.h
 *
 * Configuration definitions for the CAMD instance.
 *
 * @author	raymond zandbergen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __CAMD_CFG_H_
#define __CAMD_CFG_H_


#include "rv/rv_general.h"			/* General Riviera definitions. */
#include "camd/camd_cfg.h"			/* General Riviera definitions. */
#include "camd/camd_pool_size.h"	/* General Riviera definitions. */

#define CAMD_HISR_STACK_SIZE 		(512)

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/
#define CAMD_MB_PRIM_SIZE                CAMD_MB1_SIZE
#define CAMD_MB_PRIM_WATERMARK           (CAMD_MB_PRIM_SIZE - 128)
#define CAMD_MB_SEC_SIZE                 CAMD_MB2_SIZE
#define CAMD_MB_SEC_WATERMARK            (CAMD_MB_SEC_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_CAMD_TASK_PRIORITY
#define CAMD_TASK_PRIORITY               RVM_CAMD_TASK_PRIORITY
#else
#define CAMD_TASK_PRIORITY               80
#endif

/**
 * Software version 
 */
#define CAMD_MAJOR  0
#define CAMD_MINOR  1
#define CAMD_BUILD  0

#endif /* __CAMD_CFG_H_ */
