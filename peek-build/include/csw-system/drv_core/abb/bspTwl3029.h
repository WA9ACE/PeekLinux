/*==============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_HEADER
#define BSP_TWL3029_HEADER

#include "types.h"


/*==============================================================================
 * File Contents:
 *   This module contains function definitions and macros 
 *   associated with Triton ABB.
 */
 
/*===========================================================================
 * Defines and Macros
 */
 /*=============================================================================*/
/*!
 * @typedef BspTwl3029_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *        return code for twl3029 functions
 * @constant BSP_TWL3029_RETURN_CODE_CALLBACK_PENDING
 *    I2C callback funnction sent to I2c manager.
 *
 * @constant BSP_TWL3029_RETURN_CODE_SUCCESS
 *    function completed sucessfully
 *.
 * @constant BSP_TWL3029_RETURN_CODE_FAILURE
 *    error.
 *
 */
 

enum
{
    BSP_TWL3029_RETURN_CODE_CALLBACK_PENDING =  1,
    BSP_TWL3029_RETURN_CODE_SUCCESS =  0,   
    BSP_TWL3029_RETURN_CODE_FAILURE = -1
};
typedef Int8 BspTwl3029_ReturnCode;

#define call_fun {bspTwl3029_init();}

 
/*=============================================================================
 *  Public Functions
 *============================================================================*/

/*============================================================================*/
/*!
 * @function bspTwl3029_init
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the Triton device.
 *   All accesses are forked through this device driver.
 *
 * <b> Context </b><br>
 *   This must be called before any other TWL3014 accesses are attempted.
 *
 *  @result <br>
 *     The TWL3014 device driver will be initialized.
 */
 
BspTwl3029_ReturnCode bspTwl3029_init(void);

#endif
