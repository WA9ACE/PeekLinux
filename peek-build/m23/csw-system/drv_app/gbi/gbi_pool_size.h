/**
 * @file	gbi_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Candice Bazanegue
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	07/08/2003	Candice Bazanegue		Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __GBI_POOL_SIZE_H_
#define __GBI_POOL_SIZE_H_

#include "chipset.cfg"
 /*
  * Values used in gbi_cfg.h
  */

//Plugins
#define GBI_MC_STACK_SIZE    (512)
#define GBI_NAND_STACK_SIZE  (2048)
//Plugins
#define GBI_MC_MB_PRIM_SIZE   (4096) //old 1024
// #define GBI_NAND_MB_PRIM_SIZE (40960+40960)
#define GBI_NAND_MB_PRIM_SIZE 4096

//Total
#define GBI_STACK_SIZE        (1024 + GBI_MC_STACK_SIZE + GBI_NAND_STACK_SIZE+256+4096+1024) // +256 to add the timing measurements in GBI
#define GBI_HISR_STACK_SIZE   (512)


//Total
#define GBI_MB1_SIZE				 (4096 + GBI_MC_MB_PRIM_SIZE + GBI_NAND_MB_PRIM_SIZE) // old (2048 +...)

#define GBI_POOL_SIZE  (GBI_STACK_SIZE + GBI_MB1_SIZE)


#endif /*__GBI_POOL_SIZE_H_*/
