/*=============================================================================
 * Copyright 2003-2004 Texas Instruments Incorporated. All Rights Reserved.
 */

#ifndef BSP_I2C_MASTER_HEADER
#define BSP_I2C_MASTER_HEADER

#include "types.h"

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspI2cMaster
 *    Public interface to the HW specific I2C master device driver. There are
 *    only a few functions exported by this header because many of the
 *    functions implemented in this component are exported by the generic
 *    API.
 */
 
 
/*=============================================================================
 *  Enumerations and Types
 *============================================================================*/
/*===========================================================================*/
/*!
 * @typedef BspI2c_Result
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for I2C transaction result.
 */
enum
{
    BSP_I2C_TRANSACTION_STARTED,
    BSP_I2C_TRANSACTION_FAILED,
    BSP_I2C_TRANSACTION_DEFERRED
};
typedef Uint8 BspI2c_Result;


/*=============================================================================
 *  Public Functions
 *============================================================================*/
/*============================================================================*/
/*!
 * @function bspI2cMaster_init
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the I2C Master HW block It is called
 *   once at powerup.
 *
 * <b> Context </b><br>
 *   This must be called before any other I2C services and functions
 *
 *  @result <br>
 *     The I2C Master HW and internal SW driver will be initialized.
 */
void
bspI2cMaster_init( void );

#endif
