/**
 * @file  mc_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author  Candice Bazanegue
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  07/08/2003  Candice Bazanegue   Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __MC_POOL_SIZE_H_
#define __MC_POOL_SIZE_H_


/*
 * Values used in mmc_cfg.h
 */
//#define MC_STACK_SIZE (1024)
#define MC_STACK_SIZE (4096)//added for MMC in locosto old was 2048

#define MC_MB1_SIZE (4096)  //old 2048

#define MC_POOL_SIZE  (MC_STACK_SIZE + MC_MB1_SIZE)

/**defines needed by other components to use MC api*/
//#define MC_STACK_SIZE MC_STACK_SIZE
//#define MC_MB1_SIZE   MC_MB1_SIZE
//#define MC_POOL_SIZE  MC_POOL_SIZE




#endif /*__MC_POOL_SIZE_H_*/
