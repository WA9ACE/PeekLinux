/*=============================================================================
 * Copyright 2002-2004 Texas Instruments Incorporated. All Rights Reserved.
 */                                                                       

#ifndef BSP_I2C_DELAY_HEADER
#define BSP_I2C_DELAY_HEADER

#include "bspI2c.h"


/* Delay in Ticks */
#define BSP_I2C_DEFAULT_DELAY 4

/*=============================================================================
 * @function bspI2cDefaultDelay
 *
 * @discussion
 * <b> Description </b><br>
 *     This function is used to implement the default delay of for the I2C
 *     Bus Busy handling. 
 *
 * @param  delayValue 
 *      This identifies the I2C instance which is to activated.
 *
 * @result <br>
 *      The function returns immidiatly after starting a timer.
 *
 */
void bspI2cDefaultDelay(BspI2c_DelayValue delayValue);

#endif
