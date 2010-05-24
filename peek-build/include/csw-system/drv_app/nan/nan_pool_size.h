/**
 * @file  nan_bm_pool_size.h
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
 *  02/27/2006  J.A. Renia   Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2006 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __NAN_BM_POOL_SIZE_H_
#define __NAN_BM_POOL_SIZE_H_

#include "chipset.cfg"
 /*
  * Values used in nan_bm_cfg.h
  */
 #if(CHIPSET  != 15)
#define NAN_BM_STACK_SIZE        (750)
#define NAN_BM_HISR_STACK_SIZE   (512)


//#define NAN_BM_MB1_SIZE (2048)
#define NAN_BM_MB1_SIZE (40960)
#else

#define NAN_BM_STACK_SIZE        (750 +256)
#define NAN_BM_HISR_STACK_SIZE   (512)


//#define NAN_BM_MB1_SIZE (2048)
#define NAN_BM_MB1_SIZE (40960 + 40960)

#endif

#define NAN_BM_POOL_SIZE  (NAN_BM_STACK_SIZE + NAN_BM_MB1_SIZE)


#endif /*__NAN_BM_POOL_SIZE_H_*/

