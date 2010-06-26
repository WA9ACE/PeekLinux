/**
 * @file	i2c_pwr.h
 *
 * Power management implementation for i2c.
 *
 * @author	
 * @version 0.1
 */
/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef I2C_PWR_H_
#define I2C_PWR_H_
#include "types.h"
#define TRUE 1
#define FALSE 0

//typedef unsigned char UINT8;
//typedef unsigned long  UINT32;


// CS23  : I2C registers
#define I2C_CONTROL_REG  (0xFFFFB812L)

#define I2C_REGISTER(CC_XXX_REG) (*((volatile UINT16 *)(CC_XXX_REG)))
#define I2C_EN_ON (0x8000)


extern int sleepFlag;

#if (L1_POWER_MGT != 0x00)
void i2c_vote_deepsleepstatus(void);
#endif
Uint8  i2c_pwr_interface(Uint8);

#endif
