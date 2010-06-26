/*=============================================================================
 *    Copyright 2002-2004 Texas Instruments Inc. All rights reserved.
 */
#include "bspI2cDelay.h"



static Uint8 bspI2cDeviceCount[BSP_I2C_NUM_DEVICES];
static Uint8 bspI2cDeviceWaiting[BSP_I2C_NUM_DEVICES];
Uint32 BspI2cTickCounter=0;

void bspI2c_Tick(void)
{
	Uint32 i=0;

	while(i< BSP_I2C_NUM_DEVICES)
	{
		if(bspI2cDeviceWaiting[i] && (--bspI2cDeviceCount[i]==0))
    	{	
    		bspI2cDeviceWaiting[i]=0;
        	bspI2c_activateTransaction(i,TRUE);			
    	}
		i++;
	}

	BspI2cTickCounter++;
}

void bspI2cDefaultDelay(BspI2c_DelayValue delayValue)
{
	/* Will wait for 4 OS Ticks before checking the bus again */
	bspI2cDeviceWaiting[delayValue]=1;
	bspI2cDeviceCount[delayValue]=4;
	return;
}
