/**
 * @file  mc_cfg.h
 *
 * Configuration definitions for the MC instance.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_CFG_H_
#define __MC_CFG_H_


#include "rv/rv_general.h"       /* General Riviera definitions. */
#include "mc/mc_pool_size.h"  /* Stack & Memory Bank sizes definitions */
#include "mc/mc_api.h"

/**
 * Software version 
 */
#define MC_MAJOR  1
#define MC_MINOR  0
#define MC_BUILD  0


/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
*@{*/
#define MC_MB_PRIM_SIZE                MC_MB1_SIZE
#define MC_MB_PRIM_WATERMARK           (MC_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_MC_TASK_PRIORITY
#define MC_TASK_PRIORITY               RVM_MC_TASK_PRIORITY 
#else
#define MC_TASK_PRIORITY               71 //old 70
#endif

/**
 * Enable/Disable mc tracing
 */
#undef MC_ENABLE_TRACE
//#define MC_ENABLE_TRACE

/** define the minimum used trace level and up (DEBUG_LOW -> ERROR)*/
#define MC_USED_TRACE_LEVEL   RV_TRACE_LEVEL_ERROR

/**defines the used timer interval of the MC SWE*/
/**Inside the timer event, the insertion/removal of a card is checked*/
#define MC_CONF_TIMER_INTERVAL 1000    /** in MS*/

/**Compiler SWITCH to use DMA*/
#define MC_DMA_USED                  

/**defines the used default datatransfer method*/
#define MC_DMA_DEFAULT_MODE    MC_DMA_AUTO

/**Defines the Threshold used when using DMA_AUTO mode.*/
/** When requested amount of data is above this value DMA transfer will be used*/
#define MC_DMA_AUTO_THRESHOLD    512  /* previous value 2072 */



#endif /* __MC_CFG_H_ */
