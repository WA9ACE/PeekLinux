/**
 * @file  nan_bm_cfg.h
 *
 * Configuration definitions for the NAN_BM instance.
 *
 * @author  J.A. Renia
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia    Create.
 *
 * (C) Copyright 2006 by ICT Embedded, All Rights Reserved
 */

#ifndef __NAN_BM_CFG_H_
#define __NAN_BM_CFG_H_


#include "rv/rv_general.h"    /* General Riviera definitions. */
#include "nan/nan_pool_size.h"  /* Stack & Memory Bank sizes definitions */
#include "chipset.cfg"
#include "swconfig.cfg"
/**
 * Software version 
 */
#define NAN_BM_MAJOR  0
#define NAN_BM_MINOR  1
#define NAN_BM_BUILD  0

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/
#define NAN_BM_MB_PRIM_SIZE                NAN_BM_MB1_SIZE
#define NAN_BM_MB_PRIM_WATERMARK           (NAN_BM_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_NAN_TASK_PRIORITY
#define NAN_BM_TASK_PRIORITY               RVM_NAN_TASK_PRIORITY
#else
#define NAN_BM_TASK_PRIORITY               80
#endif

/**
 * memory sizes
 */
#ifdef _WINDOWS
#define NAN_BM_FLASH_NOF_BLOCKS           (256)  // reduce PC simulation to 4MB!!!!
#define NAN_BM_FLASH_PAGES_PER_BLOCK      (32)
#define NAN_BM_FLASH_NOF_PAGES            (NAN_BM_FLASH_NOF_BLOCKS*NAN_BM_FLASH_PAGES_PER_BLOCK)
#else //target
#if(CHIPSET != 15)
	#define NAN_BM_FLASH_NOF_BLOCKS           (2048)  
#else
	#define NAN_BM_FLASH_NOF_BLOCKS 		(4096)
#endif

#define NAN_BM_FLASH_PAGES_PER_BLOCK      (32)
#define NAN_BM_FLASH_NOF_PAGES            (NAN_BM_FLASH_NOF_BLOCKS*NAN_BM_FLASH_PAGES_PER_BLOCK)
#endif

#define NAN_BM_FLASH_PAGE_SIZE            (528) /* bytes */
#define NAN_BM_FLASH_HALF_PAGE_OFFSET     (256) /* bytes */
#define NAN_BM_FLASH_SPARE_OFFSET         (512) /* bytes */

/**
 * clock divider value for the prescaler, 1 - 15
 */
#define NAN_BM_CLK_DIV  4

/**
 * respect busy timing, 0-15 cycles
 * actual time = value * prescaler
 */
#define NAN_BM_DUMMY_CYCLE 15

/**
 * indicates if ECC is enabled or not. If commented out then disabled
 */
#ifndef DATALIGHT_NAND 
 #define NAN_BMD_ECC_ENABLED 
#elif ((DATALIGHT_NAND != 1) && (DATALIGHT_NAND != 2))
  #define NAN_BMD_ECC_ENABLED    
#endif 

#endif /* __NAN_BM_CFG_H_ */
