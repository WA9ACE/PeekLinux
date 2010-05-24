/**
 * @file	rfsfat_cfg.h
 *
 * Configuration definitions for the RFSFAT instance.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef __RFSFAT_CFG_H_
#define __RFSFAT_CFG_H_


#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "nan/nan_pool_size.h"	/* Stack & Memory Bank sizes definitions */

#define FSCORE_MAX_NR_OPEN_FSCORE_FILES  10


/**
 * Software version 
 */
#define RFSFAT_MAJOR  0
#define RFSFAT_MINOR  1
#define RFSFAT_BUILD  0


/**
 * Sample value.
 */
#define RFSFAT_SAMPLE_VALUE	            10



/**
 * @name Mem bank
 *
  * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
*/
/*@{*/
#define RFSFAT_MB_PRIM_SIZE                (RFSFAT_MB1_SIZE)
#define RFSFAT_MB_PRIM_WATERMARK           (RFSFAT_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_RFSFAT_TASK_PRIORITY
#define RFSFAT_TASK_PRIORITY               RVM_RFSFAT_TASK_PRIORITY
#else
#define RFSFAT_TASK_PRIORITY               102
#endif





#endif /* __RFSFAT_CFG_H_ */
