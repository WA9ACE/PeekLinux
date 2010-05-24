/*=============================================================================
 * Copyright 2004 Texas Instruments Incorporated. All Rights Reserved.
 */

#ifndef BSP_TWL3029_SIM_HEADER
#define BSP_TWL3029_SIM_HEADER

/*=============================================================================
 * Types
 */

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_SimVoltage
 *
 * @discussion
 *    Data type for the sim voltage.
 *
 * @constant BSP_TWL3029_SIM_VOLTAGE_18V
 *    sets the sim voltage to 1.8 volts.
 *
 * @constant BSP_TWL3029_SIM_VOLTAGE_285V
 *    Sets the sim voltage to 2.85 volts.
 */

enum
{
    BSP_TWL3029_SIM_VOLTAGE_18V = 0,
    BSP_TWL3029_SIM_VOLTAGE_285V    = 1
};
typedef Int8 BspTwl3029_SimVoltage;


/*==============================================================================*/
/*!
 * @function bspTwl3029_Sim_init
 *
 * @discussion
 *    Initalise the SIM detection module and setup the VRSIM to 1.8 V
 *
 *  @param     callbackFuncPtr
 *       pointer to struct containing  information for i2c callback 
 *    
 * @result
 *    returns BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_init( void );

/*==============================================================================*/
/*!
 * @function bspTwl3029_Sim_enable
 *
 * @discussion
 *    Enable the the SIM module and detection module.
 *
 *  @param     callbackFuncPtr
 *       pointer to struct containing  information for i2c callback 
 *    
 *
 * @result
 *    returns BspTwl3029_ReturnCode
 */

BspTwl3029_ReturnCode
bspTwl3029_Sim_enable(  BspTwl3029_I2C_CallbackPtr    callbackFuncPtr);
/*==============================================================================*/
/*!
 * @function bspTwl3029_Sim_setUpVoltage
 *
 * @discussion
 *    Setup the VRSIM voltage.
 *
 *   @param     callbackFuncPtr
 *       pointer to struct containing  information for i2c callback 
 *
 *  @param simVoltage
 *     Gives the voltage to which SIM card should be set.
 *    
 *
 * @result
 *    returns BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_setUpVoltage(  BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                              BspTwl3029_SimVoltage simVoltage );

/*==============================================================================*/
/*!
 * @function bspTwl3029_Sim_disable
 *
 * @discussion
 *    Disable the the SIM module and detection module.
 *
 * @param     callbackFuncPtr
 *       pointer to struct containing  information for i2c callback 
 *    
 *
 * @result
 *    returns BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_disable(  BspTwl3029_I2C_CallbackPtr    callbackFuncPtr );

#endif
