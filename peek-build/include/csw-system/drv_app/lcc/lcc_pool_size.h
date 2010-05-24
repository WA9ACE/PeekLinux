/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR MMI Interface
 *
 * lcc_pool_size.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#ifndef __LCC_POOL_SIZE_H_
#define __LCC_POOL_SIZE_H_

#define LCC_STACK_SIZE (3*1024)
#define LCC_MB1_SIZE  (1024)

#define LCC_POOL_SIZE  (LCC_STACK_SIZE + LCC_MB1_SIZE)
#endif
