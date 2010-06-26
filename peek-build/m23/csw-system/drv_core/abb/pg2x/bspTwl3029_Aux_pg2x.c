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
//#include "main_system.h"
#include "bspTwl3029.h"
#include "../bspTwl3029_Int_Map.h"
#include "../bspTwl3029_Int_Llif.h"
#include "../bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Map_pg2x.h"
#include "../bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Aux_Llif_pg2x.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"
#include "bspTwl3029_Aux_pg2x.h"
#include "bspTwl3029_Aux.h"

//#include "main_Platform.h"

//#include "bspUtil_Assert.h"




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
 * Fucntion bspTwl3029_Aux_LedsCtrl
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Aux_LedsCtrl(BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                        BspTwl3029_Aux_LedCtrlField   control_field,
			BspTwl3029_Aux_LedsCtrlData*  ctrlData)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
       
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint16 count = 0;
     
    BspTwl3029_I2C_RegData shadowWledCtrlRead, shadowWledCtrlData = 0;
    BspTwl3029_I2C_RegData shadowWledResTrimData=0, shadowWledResTrimRead = 0 ;
   
   /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
    
   
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_WLED_CTRL_OFFSET,
                             &shadowWledCtrlRead);
    shadowWledCtrlData = shadowWledCtrlRead;
    
    if ((control_field & 1) != 0)
    {
          /*  do nothing */	  
    }
    
    if ((control_field & 2) != 0)
    {
         BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_WLED_WLED_CTRL_TIMER_CTRL, &shadowWledCtrlData, ctrlData->timer);
    }
    
    if ((control_field & 4) != 0)
    {
         BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_WLED_WLED_CTRL_DAC_IREF_CTRL, &shadowWledCtrlData, ctrlData->dacIref);
    }
    
    if ((control_field & 8) != 0)
    {
         BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_WLED_WLED_CTRL_WLED_SWFREQ, &shadowWledCtrlData, ctrlData->swFreq);
    }
    
    
    if ((control_field & 16) != 0)
    {
        /* Get current TRIM value by reading shadow WLED_RES_TRIM */
	returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX, BSP_TWL3029_MAP_WLED_WLED_RES_TRIM_OFFSET,
                             &shadowWledResTrimRead);
	/* trim coontrol is a 5 bit value  */
	shadowWledResTrimRead &=  0x1f;
	shadowWledResTrimData = ctrlData->trim & 0x1f;		     			     
    }
   
   
   
   /* now request to I2C manager to write to Triton registers */
   if ((shadowWledCtrlData != shadowWledCtrlRead) && ( returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_WLED,BSP_TWL3029_MAP_WLED_WLED_CTRL_OFFSET, 
                                              shadowWledCtrlData,regInfoPtr++);      
       count++;
   }
   /* now request to I2C manager to write to Triton registers */
   if ((shadowWledResTrimData != shadowWledResTrimRead) && ( returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_WLED,BSP_TWL3029_MAP_WLED_WLED_RES_TRIM_OFFSET,                      
					      shadowWledResTrimData,regInfoPtr++);      
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
