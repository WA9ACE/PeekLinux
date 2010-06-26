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
 *  FILE NAME: bspTwl3029_pg2x_Audio.c
 *
 *
 *  PURPOSE:  
 *    This module defines functions for specific use with
 *    hardware twl3029 PG2.0
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
 *  04/05/2005  Mary Tooher     V1.0.0   First implementation
 */
/*******************************************************************************
 *   includes
 */ 
// #include "main_system.h"
#include "types.h"

#include "bspTwl3029.h"
#include "../bspTwl3029_Aud_Llif.h"
#include "../bspTwl3029_Aud_Map.h"
#include "../bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Aud_Llif_pg2x.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Audio.h"
#include "bspTwl3029_Audio_pg2x.h"
/*=============================================================================
 *   Public Functions
 *============================================================================*/

 /*=============================================================================
 * Description:
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_vmemoConfigure(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                BspTwl3029_AudioI2sVoiceUplinkState  i2sOn )
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;    
    
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;
    
    BspTwl3029_I2C_RegData tmpCtrl4RegData, tmpCtrl5RegData = 0;
    BspTwl3029_I2C_RegData tmpStore4, tmpStore5 = 0;
    
    /* check validity of arg */
    if (i2sOn > BSP_TWL3029_AUDIO_STEREO_I2S_MAX )
    {
       return returnVal;
    }
    
    /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
    
   /* now configure for VMEMO */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);
   tmpStore4 = tmpCtrl4RegData;			     
   
   /* note VMEMO/I2SON  can only be activated if STON pwer bit (PWRONSTATUS reg) is already on */  
   /* however the register itself  can be programmed reqardless if STON status */                                                                                                                      
   
   
   /*  set I2SON bit ( reg CTRL4 bit 6) */
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL4_I2SON, &tmpStore4,(i2sOn & 1));
   /*  set VMEMO  bit ( reg CTRL4 bit 7) */
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL4_VMEMO, &tmpStore4,((i2sOn & 2)>>1));
   if ((tmpStore4 != tmpCtrl4RegData)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
      /*update triton CTRL4 reg */
      //returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpStore4,callbackFuncPtr);		
   
       /*update triton CTRL4 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpStore4,regInfoPtr++);	
 
      count++;
   }
   
   if ( i2sOn && 4 )
   {
       /* set the justify bit */
       /* note: the above condition will only be true for PG 2.0 as a value of 4 or above
          for PG1.0 results in func returning an error code during the arg validity checking
          at start of this function
       */	  
       returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &tmpCtrl5RegData);
        tmpStore5 = tmpCtrl5RegData;
        /* bit 3 of the i2sOn is the value to set CTRL%_I2SLRJ bit  ( bit 5 on the CTRL5 reg ) */
	BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL5_I2SLRJ, &tmpStore5,(i2sOn >> 3 ));
        if ((tmpStore4 != tmpCtrl4RegData)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
       {
          /*update triton CTRL5 reg */
          returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, tmpStore5,regInfoPtr++);	
 
         count++;
       }
   }
    /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }	
   return returnVal;
}	
