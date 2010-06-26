/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2005 Texas Instruments France. All rights reserved
 *
 *                          Author : Mary  TOOHER
 *
 *
 *  Important Note
 *  --------------
 *
 *  This S/W is a preliminary version. It contains information on a product 
 *  under development and is issued for evaluation purposes only. Features 
 *  characteristics, data and other information are subject to change.
 *
 *  The S/W is furnished under Non Disclosure Agreement and may be used or
 *  copied only in accordance with the terms of the agreement. It is an offence
 *  to copy the software in any way except as specifically set out in the 
 *  agreement. No part of this document may be reproduced or transmitted in any
 *  form or by any means, electronic or mechanical, including photocopying and
 *  recording, for any purpose without the express written permission of Texas
 *  Instruments Inc.
 *
 ******************************************************************************
 *
 *  FILE NAME: bspTwl3029_Aux.h
 *
 *
 *  PURPOSE:  Driver header for various Twl3029 ( Triton )analog  Transceiver and resourse
 *            These are:
 *             LEDA  - for use as LCD back light
 *             LEDB
 *             LEDC
 *             Vibrator
 *
 *             Note: USBotg transceiver and SIM have their own separate drivers
 *
 *  FILE REFERENCES:
 *
 *  Name                  IO      Description
 *  -------------         --      ---------------------------------------------
 *  
 *
 *
 *  EXTERNAL VARIABLES:
 *
 *  Source:
 *
 *  Name                  Type              IO   Description
 *  -------------         ---------------   --   ------------------------------
 *
 *  
 *
 *
 *  EXTERNAL REFERENCES:
 *
 *  Name                Description
 *  ------------------  -------------------------------------------------------
 *
 *
 *
 *  ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
 *  
 *
 *
 *  ASSUMPTION, CONSTRAINTS, RESTRICTIONS:
 *  
 *
 *
 *  NOTES:
 *  
 *
 *
 *  REQUIREMENTS/FUNCTIONAL SPECIFICATION REFERENCES:
 *
 *
 *
 *
 *  DEVELOPMENT HISTORY:
 *
 *  Date        Name(s)         Version  Description
 *  ----------  --------------  -------  --------------------------------------
 *  04/03/2004  Mary Tooher     V1.0.0   First implementation
 */
#ifndef BSP_TWL3029_AUX_HEADER
#define BSP_TWL3029_AUX_HEADER 
 /*******************************************************************************
 *   includes
 */
 
#include "types.h"

#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"

#include "bspUtil_Assert.h"

#if (ANLG_PG == 1)
#include "pg1x/bspTwl3029_Aux_pg1x.h"
#elif (ANLG_PG == 2)
#include "pg2x/bspTwl3029_Aux_pg2x.h"
#endif 

/*=============================================================================
 *  Defines and Macros
 */
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_
 *
 * @discussion 
 * <b> Description </b><br>
 *
 * @constant     
 */	
 
 /*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_Led
 *
 * @discussion 
 * <b> Description </b><br>
 *     enum used for selecting a LED
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_AUX_LEDA  = 1,
    BSP_TWL3029_AUX_LEDB  = 2,
    BSP_TWL3029_AUX_LEDC  = 4
};
typedef Uint8 BspTwl3029_Aux_Led;	
 /*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_LedEnable
 *
 * @discussion 
 * <b> Description </b><br>
 *     enum used for enable/disabling a LED
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_LED_DISABLE  = 0,
    BSP_TWL3029_LED_ENABLE = 1
};
typedef Uint8 BspTwl3029_Aux_LedEnable;			               				   
/*=============================================================================
 * Description:
 */


/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_VibEnable
 *
 * @discussion 
 * <b> Description </b><br>
 *     enum used for enable/disabling a LED
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_VIB_DISABLE  = 0,
    BSP_TWL3029_VIB_ENABLE = 1
};
typedef Uint8 BspTwl3029_Aux_VibEnable;		
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_VibTrimV
 *
 * @discussion 
 * <b> Description </b><br>
 *     enum used trimming an internal resistor ladder depending on val of
 *     bandgap voltage.
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_VIB_TRIM_BGV_1_190V  = 1,
    BSP_TWL3029_VIB_TRIM_BGV_1_183V  = 3,
    BSP_TWL3029_VIB_TRIM_BGV_1_200V  = 4
};
typedef Uint8 BspTwl3029_Aux_VibTrimV;		

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_VibVsel
 *
 * @discussion 
 * <b> Description </b><br>
 *     Output voltage level
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_VIB_TRIM_VSEL_1_2V  = 0,
    BSP_TWL3029_VIB_TRIM_VSEL_1_3V  = 1,
    BSP_TWL3029_VIB_TRIM_VSEL_1_4V  = 2,
    BSP_TWL3029_VIB_TRIM_VSEL_2_7V  = 3
};
typedef Uint8 BspTwl3029_Aux_VibVsel;		

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Aux_VibCycle
 *
 * @discussion 
 * <b> Description </b><br>
 *     enum susted for duty cycle selection for vibrator
 *
 * @constant  
 *      
 */
 enum
{
    BSP_TWL3029_VIB_DUTY_CYCLE_100  = 0,  /* 100 % duty cycle */
    BSP_TWL3029_VIB_DUTY_CYCLE_75   = 1,
    BSP_TWL3029_VIB_DUTY_CYCLE_50   = 2,
    BSP_TWL3029_VIB_DUTY_CYCLE_25   = 3   /* 25 % ( default cycle) */
};
typedef Uint8 BspTwl3029_Aux_VibCycle;		

/*=============================================================================
 * Description:
 */


/*=============================================================================
 *   Public Functions
 *============================================================================*/
/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_init
 *
 * <b> Description  </b><br>
 *    init function for aux. initializes LEDS and vibrator
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 
 BspTwl3029_ReturnCode 
bspTwl3029_Aux_init(void);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_LedEnable
 *
 * <b> Description  </b><br>
 *    Gets status of the three LEDS
 *
 * @param led
 *   bit map of LEDs to be LEDS to be enabled/disabled
 *
 * @param enable
 *   enable or disable 
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 
BspTwl3029_ReturnCode 
bspTwl3029_Aux_getLedStatus(BspTwl3029_Aux_Led led, 
			 BspTwl3029_Aux_LedEnable *enable);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_LedEnable
 *
 * <b> Description  </b><br>
 *    Enables/disables one or more of the three LEDS
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param led
 *   bit map of LEDs to be LEDS to be enabled/disabled
 *
 * @param enable
 *   enable or disable 
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */

BspTwl3029_ReturnCode 
bspTwl3029_Aux_LedEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_Led led, 
			 BspTwl3029_Aux_LedEnable enable);
			 
			 
/*=============================================================================
 * @function bspTwl3029_Aux_LedConfig
 *
 * <b> Description  </b><br>
 *    configures Led drivers
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param led
 *    LEDs to be configured 
 *
 * @param dac_reg
 *    current DAC value
 *
 * @param control_reg
 *   contro reg value 
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *   
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_LedConfig(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_Led led, 
			 Uint8  dac_reg,
			 Uint8  control_reg);




/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_VibConfig
 *
 * <b> Description  </b><br>
 *    configures vibrator motor
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 *
 * @param trimVoltage
 *   trimming an internal resistor ladder depending on val of bandgap voltage 
 *
 * @param outputVoltage
 *    output voltage
 *
 * @param cycle
 *   duty cycle ( percentage value )
 * @return 
 *   return of type BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_VibConfig(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_VibTrimV trimVoltage, 
			 BspTwl3029_Aux_VibVsel  outputVoltage,
			 BspTwl3029_Aux_VibCycle  cycle);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_getVibConfig
 *
 * <b> Description  </b><br>
 *    configures vibrator motor
 *
 * @param trimVoltage
 *   trimming an internal resistor ladder depending on val of bandgap voltage 
 *
 * @param outputVoltage
 *    output voltage
 *
 * @param cycle
 *   duty cycle ( percentage value )
 * @return 
 *   return of type BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_getVibConfig(BspTwl3029_Aux_VibTrimV* trimVoltage, 
			 BspTwl3029_Aux_VibVsel*  outputVoltage,
			 BspTwl3029_Aux_VibCycle*  cycle);
/*===========================================================================*/
/*!
 * @function bspTwl3029_Aux_VibEnable
 *
 * <b> Description  </b><br>
 *    Enables/disables vibrator motor
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 *
 * @param enable
 *   enable or disable 
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_VibEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
			 BspTwl3029_Aux_VibEnable enable);


#endif















