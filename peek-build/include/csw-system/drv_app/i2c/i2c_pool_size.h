/**
 * @file	i2c_pool_size.h
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

#ifndef __I2C_POOL_SIZE_H_
#define __I2C_POOL_SIZE_H_


/*
 * Values used in i2c_cfg.h
 */
#define I2C_STACK_SIZE (512)

#define I2C_MB1_SIZE (1024)

#define I2C_POOL_SIZE  (I2C_STACK_SIZE + I2C_MB1_SIZE)


#endif /*__I2C_POOL_SIZE_H_*/
