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
 *  FILE NAME: bspTwl3029_Aux.c
 *
 *
 *  PURPOSE:  Driver  for various Twl3029 ( Triton )analog  Transceiver and resourse
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
 
 /*=============================================================================
*   includes
 */
 
#include "types.h"

#include "bspTwl3029.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"
#include "bspTwl3029_Aux.h"




/*=============================================================================
 *  Defines and Macros
 */
			               				   
/*=============================================================================
 * Description:
 */


/*=============================================================================
 *   Private Functions
 *============================================================================*/
/*=============================================================================
 * Description:
 */ 


/*=============================================================================
 * Description:
 */


/*=============================================================================
 *   Public Functions
 *============================================================================*/
/*=============================================================================
 * Fucntion bspTwl3029_Aux_init
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_init(void)
{
    /* configurure LEC currents to default value */
    /* switch on a LED ( if required on boot up  */
    return BSP_TWL3029_RETURN_CODE_SUCCESS;
}    

/*=============================================================================
 * Fucntion bspTwl3029_Aux_getLedStatus
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_getLedStatus(BspTwl3029_Aux_Led led, 
			 BspTwl3029_Aux_LedEnable *enable)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData shadowRegData;
   
   /* check validity of args */
   if (( led > BSP_TWL3029_AUX_LEDC))
   {
      return returnVal;
   }
   
   /* Read the WLED status from the shadow register*/
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_AUX_REG_PWDNSTATUS_OFFSET,
                             &shadowRegData); 
   
    if ((led & BSP_TWL3029_AUX_LEDA) != 0)
    {
	  *enable = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDA_EN, &shadowRegData); 	                                                          
    }
    else if ((led & BSP_TWL3029_AUX_LEDB) != 0)
    {
	  *enable = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDB_EN, &shadowRegData); 
    } 
    else if ((led & BSP_TWL3029_AUX_LEDC) != 0)
    {
	  *enable = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDC_EN, &shadowRegData); 
    } 
    
    return returnVal;
}

/*=============================================================================
 * Fucntion bspTwl3029_Aux_LedEnable
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_LedEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_Led led, 
			 BspTwl3029_Aux_LedEnable enable)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData shadowRegData, tmpAuxToggleData = 0;
    Uint8 togbSetShift, shadowStatusbit = 0;  
   
   
   /* check validity of args */
   if (( led > BSP_TWL3029_AUX_LEDC) ||(enable > BSP_TWL3029_LED_ENABLE))
   {
      return returnVal;
   }
   
   /* note: Power is set/reset through TOGB register and powerstaus is given in  PWRONSTATUS register  */ 
    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_AUX_REG_PWDNSTATUS_OFFSET,
                             &shadowRegData);
   if ( enable == BSP_TWL3029_LED_DISABLE )
   {
        togbSetShift = 0;  /* reset shift*/
        shadowStatusbit = 0;
   }
   
    else
    {
        /*set shift  The "set" bits in TOGGLE register is the same as the 'reset' bit shifted by 1*/
       togbSetShift = 1;
       shadowStatusbit = 1; 
    } 
   
    if ((led & BSP_TWL3029_AUX_LEDA) != 0)
    {
          tmpAuxToggleData = 1 << ( BSP_TWL3029_LLIF_AUX_REG_TOGGLE2_WLEDAR_OFFSET + togbSetShift);
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDA_EN, &shadowRegData, shadowStatusbit ); 	                                                          
    }
    
    if ((led & BSP_TWL3029_AUX_LEDB) != 0)
    {
          tmpAuxToggleData |= 1 << ( BSP_TWL3029_LLIF_AUX_REG_TOGGLE2_WLEDBR_OFFSET + togbSetShift);
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDB_EN, &shadowRegData, shadowStatusbit ); 
    }
    
    if ((led & BSP_TWL3029_AUX_LEDC) != 0)
    {
          tmpAuxToggleData |= 1 << ( BSP_TWL3029_LLIF_AUX_REG_TOGGLE2_WLEDCR_OFFSET + togbSetShift);
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUX_REG_PWDNSTATUS_WLEDC_EN, &shadowRegData, shadowStatusbit ); 
    }
    
    /* update Triton  AUX TOGGLE */
    returnVal= BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL3029_MAP_AUX_REG_TOGGLE2_OFFSET,
                                   tmpAuxToggleData,  callbackFuncPtr) ; 
    
    /* update shadow  power  reg, this is used by other functions as an alternative to reading the HW register */
    if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
       BspTwl3029_I2c_shadowRegWrite( BSP_TWL3029_I2C_AUD, BSP_TWL3029_MAP_AUX_REG_PWDNSTATUS_OFFSET, shadowRegData);
    }
    return returnVal;
}    


/*=============================================================================
 * Fucntion bspTwl3029_Aux_LedConfig
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_LedConfig(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_Led led, 
			 Uint8  dac_reg,
			 Uint8  control_reg)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData shadowRegData, shadowLedAData, shadowLedBData,shadowLedCData = 0;
 
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;
    BspTwl3029_I2C_RegData tmpCtrl4RegData, tmpPwrStatus= 0;
    BspTwl3029_I2C_RegData tmpStore4, togbData = 0;
    
    /* check validity of args */
   if ( led > BSP_TWL3029_AUX_LEDC )
   {
      return returnVal;
   }
        
    /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
         
   /* note: Power is set/reset through TOGB register and powerstaus is given in  PWRONSTATUS register  */ 
    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_CTRL_LEDA_OFFSET,
                             &shadowLedAData);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_CTRL_LEDB_OFFSET,
                             &shadowLedBData);	
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_CTRL_LEDC_OFFSET,
                             &shadowLedCData);			     		     
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_WLED_CTRL_OFFSET,
                             &shadowRegData);	
    if ((led & BSP_TWL3029_AUX_LEDA) != 0)
    {
          if ((shadowLedAData != dac_reg)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
          {
            /*update triton CTRL4 reg */
             returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_WLED_CTRL_LEDA_OFFSET, 
	                 dac_reg,regInfoPtr++);	
             count++;	
          }	                                                          
    }
    
    if ((led & BSP_TWL3029_AUX_LEDB) != 0)
    {
           if ((shadowLedBData != dac_reg)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
          {
            /*update triton CTRL4 reg */
             returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_WLED_CTRL_LEDB_OFFSET, 
	                 dac_reg,regInfoPtr++);	
             count++;	
          }
    }
    
    if ((led & BSP_TWL3029_AUX_LEDC) != 0)
    {
         if ((shadowLedCData != dac_reg)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
          {
            /*update triton CTRL4 reg */
             returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_WLED_CTRL_LEDC_OFFSET, 
	                 dac_reg,regInfoPtr++);	
             count++;	
          }
    }
    
    if ((shadowRegData != control_reg)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
    {
        /*update triton CTRL4 reg */
         returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_WLED_CTRL_OFFSET, 
	                 control_reg,regInfoPtr++);	
         count++;	
    }
    
   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }	
    
    return returnVal;
}    
/*=============================================================================
 * Fucntion bspTwl3029_Aux_VibConfig
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_VibConfig(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_VibTrimV trimVoltage, 
                         BspTwl3029_Aux_VibVsel  outputVoltage,
                         BspTwl3029_Aux_VibCycle  cycle)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_RegData RegVibCtrlRead, RegVibCtrlData = 0;
    
    /* check input parameters */
    if ( (outputVoltage > BSP_TWL3029_VIB_TRIM_VSEL_2_7V) ||
         (cycle > BSP_TWL3029_VIB_DUTY_CYCLE_25) ||
        ( trimVoltage > BSP_TWL3029_VIB_TRIM_BGV_1_200V ) ||
        ( trimVoltage == 0) || ( trimVoltage == 2))
    {
      /* invalid arg */
      return returnVal;
    }
    
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_VIB_VIB_CTRL_OFFSET,
                             &RegVibCtrlRead);
   
    RegVibCtrlData = (trimVoltage << BSP_TWL3029_LLIF_VIB_VIB_CTRL_TRIM_OFFSET ) | 
                      (outputVoltage << BSP_TWL3029_LLIF_VIB_VIB_CTRL_VSEL_OFFSET )|
                      cycle;
    
    /* update HW register over I2c ( if required */
    if ( RegVibCtrlRead != RegVibCtrlData)
    {
    returnVal= BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL3029_MAP_VIB_VIB_CTRL_OFFSET,
                                   RegVibCtrlData,  callbackFuncPtr) ; 
    }      
    return  returnVal;
}    


/*=============================================================================
 * Fucntion bspTwl3029_Aux_VibConfig
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_getVibConfig( BspTwl3029_Aux_VibTrimV* trimVoltage, 
                         BspTwl3029_Aux_VibVsel*  outputVoltage,
                         BspTwl3029_Aux_VibCycle*  cycle)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_RegData RegVibCtrlRead, RegVibCtrlData = 0;
    
    /* check input parameters */
    if ( (outputVoltage == NULL) &&
         (cycle == NULL) &&
        ( trimVoltage == NULL ) )
    {
      /* invalid arg */
      return returnVal;
    }
    
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_VIB_VIB_CTRL_OFFSET,
                             &RegVibCtrlRead);
   
    if(trimVoltage != NULL)
    {
        *trimVoltage = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_VIB_VIB_CTRL_TRIM, &RegVibCtrlRead);
    }
    if(outputVoltage != NULL)
    {
        *outputVoltage = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_VIB_VIB_CTRL_VSEL, &RegVibCtrlRead);
    }
    if(cycle != NULL)
    {
        *cycle = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_VIB_VIB_CTRL_DSEL, &RegVibCtrlRead);
    }
	
    return  returnVal;
}

/*=============================================================================
 * Fucntion bspTwl3029_Aux_VibEnable
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_VibEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                         BspTwl3029_Aux_VibEnable enable)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData  tmpAuxToggleData = 0;
    Uint8 togbSetShift = 0;  
   
   
   /* check validity of args */
   if (enable > BSP_TWL3029_VIB_ENABLE)
   {
      return returnVal;
   }
   
   /* note: Power is set/reset through TOGB register and powerstaus is given in  PWRONSTATUS register  */ 
   
   if ( enable == BSP_TWL3029_VIB_DISABLE )
   {
        togbSetShift = 0;  /* reset shift*/
   }
   
    else
    {
        /*set shift  The "set" bits in TOGGLE register is the same as the 'reset' bit shifted by 1*/
       togbSetShift = 1;
    } 
   
    
    tmpAuxToggleData = 1 << ( BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_VIBR_OFFSET + togbSetShift);
     
    
    /* update Triton  AUX TOGGLE */
    returnVal= BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   tmpAuxToggleData,  callbackFuncPtr) ; 
    
    
    return returnVal;
}

