/**
* @file i2c_cfg.h
*
* Configuration definitions for the I2C instance.
*
* @author Remco Hiemstra (remco.hiemstra@ict.nl)
* @version 0.1
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 12/30/2003  Remco Hiemstra (remco.hiemstra@ict.nl)    Create.
*
* (C) Copyright 2003 by ICT Automatisering, All Rights Reserved
*/

#ifndef __I2C_CFG_H_
#define __I2C_CFG_H_

#include "rv/rv_general.h"   /* General Riviera definitions. */
#include "i2c/i2c_pool_size.h"  /* Stack & Memory Bank sizes definitions */


#define I2C_VERSION_MAJOR 0
#define I2C_VERSION_MINOR 1
#define I2C_VERSION_BUILD 0

/**
* I2C_OWN_ADDRESS value.
*/
#define I2C_OWN_ADDRESS                  0x05


/**
* I2C_PRESCALE_VALUE value.
*/
#define I2C_PRESCALE_VALUE               1

/**
* I2C_CLOCK_TIME_HIGH value.
*/
#define I2C_CLOCK_TIME_HIGH              ESAMPLE_100_KHZ_HIGH

/**
* I2C_CLOCK_TIME_LOW value.
*/
#define I2C_CLOCK_TIME_LOW               ESAMPLE_100_KHZ_LOW

#define I2C_ADDRESSMODE                  I2C_07_BITS_ADDRESS_MODE

#define ESAMPLE_100_KHZ_HIGH             (20)

#define ESAMPLE_100_KHZ_LOW              (25)

#define ESAMPLE_400_KHZ_HIGH             (0)

#define ESAMPLE_400_KHZ_LOW              (2)


#define I2C_07_BITS_ADDRESS_MODE         (0x00)

#define I2C_10_BITS_ADDRESS_MODE         (0x0100)

/** 
* Size of task stack in bytes.
*
* During development, put the hardcoded value here.
* After integration, the value should be in rvf_pool_size.h
*/
#ifdef I2C_STACK_SIZE
#undef I2C_STACK_SIZE
#endif

#ifdef RVF_I2C_STACK_SIZE
#define I2C_STACK_SIZE                  RVF_I2C_STACK_SIZE
#else
#define I2C_STACK_SIZE                  512
#endif

/**
* @name Mem bank
*
* Memory bank size and watermark.
*@{*/
#define I2C_MB_PRIM_SIZE                I2C_MB1_SIZE
#define I2C_MB_PRIM_WATERMARK           (I2C_MB_PRIM_SIZE - 128)
/*@}*/

/** 
* Wished priority of the host task for the SWE.
*
* During development, put the hardcoded value here.
* After integration, the value should be in rvm_priorities.h
*/
#ifdef RVM_I2C_TASK_PRIORITY
#define I2C_TASK_PRIORITY               RVM_I2C_TASK_PRIORITY
#else
#define I2C_TASK_PRIORITY               80
#endif

#endif  /* __I2C_CFG_H_ */
