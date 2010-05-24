/**
 * @file	bae_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Vincent Oberle
 * @version 0.1
 */

/*
 * History:
 *
 *  Date       	Author               Modification
 *  -------------------------------------------------------------------
 *  09/07/2004  Frederic Maria       Creation.
 *                                   Split pool into external and internal memory.
 *
 * (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __BAE_POOL_SIZE_H_
#define __BAE_POOL_SIZE_H_
#include "bae/bae_options.h"

/*
 * Values used in bae_env.h
 */
#ifdef BAE_ULTRA_LOCOSTO
#define BAE_STACK_SIZE   (2 * 1024)
#else
#define BAE_STACK_SIZE   (4 * 1024) //OMAPS00092694 : As defined in bae_cfg.h
#endif
#define BAE_MB1_SIZE     (150 * 1024) //(0x40000)
#define BAE_MB2_SIZE     (25 * 1024)

#define BAE_POOL_SIZE      (BAE_MB1_SIZE)
#define BAE_INT_POOL_SIZE  (BAE_STACK_SIZE + BAE_MB2_SIZE)


#endif /*__BAE_POOL_SIZE_H_*/
