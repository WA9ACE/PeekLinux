/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2004 Texas Instruments France. All rights reserved
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
 *  FILE NAME: UsbOtg.c
 *
 *
 *  PURPOSE:  All Twl3029 ( Triton ) USB Transceiver related functions
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
 *  d_ram_loader          T_RAM_LOADER      IO   RAM loader structure
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
 *  29/11/2004  Mary Tooher     V1.0.0   First implementation
 */
 
 /*******************************************************************************
 *   includes
 */
 
#include "types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_IntC.h"
#include "bspI2c.h"
#include "bspTwl3029_UsbOtg.h"





/*******************************************************************************
* Defines and Macros
*/


/*******************************************************************************
 * Local Data
 */ 
 
/********************************************************************************
 * Local function declarations
 */ 
 
 /*********************************************************************************
 * Private (Local) Functions
 */

/*********************************************************************************
 * Public Functions
 */

/*=============================================================================
 * Function:  bspTwl3029_UsbOtg_getVendorId
 *
 * Description:
 *    reads the two vendor registers and returns 16 bit vendor ID 
 *
 * Inputs:     none
 *      
 *
 *  Returns:   BspTwl3029_UsbVendorRegister - 16 bit vendor ID
 *  
 *  Notes:    this function requires no callback arg shadow register 
 *            ( not h/w ) are read
 */
BspTwl3029_UsbVendorRegister
bspTwl3029_UsbOtg_getVendorId(void)
{
    /* just read from shadow registers  */
    BspTwl3029_UsbVendorRegister regValue;
    
    BspTwl3029_I2C_RegData tmpCtrlRegData_Lsb, tmpCtrlRegData_Msb;
    /* just read from shadow registers  */
    
    /* read shadow registers  */
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB, 
   BSP_TWL3029_MAP_USB_VENDOR_ID_LSB_OFFSET, &tmpCtrlRegData_Lsb);
   
    /* read shadow registers  */
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB, 
   BSP_TWL3029_MAP_USB_VENDOR_ID_MSB_OFFSET, &tmpCtrlRegData_Msb);
    
   regValue = (((BspTwl3029_UsbIdRegister)tmpCtrlRegData_Msb << 8)
               |((BspTwl3029_UsbIdRegister)tmpCtrlRegData_Lsb ));
    return regValue;
}

/*=============================================================================
 * Function:  bspTwl3029_UsbOtg_getProductId
 *
 * Description:
 *    reads the two vendor registers and returns 16 bit product ID 
 *
 * Inputs:     none
 *      
 *
 *  Returns:   BspTwl3029_UsbIdRegister - 16 bit product ID
 *  
 *  Notes:    this function requires no callback arg shadow register 
 *            ( not h/w ) are read 
 */
BspTwl3029_UsbIdRegister
bspTwl3029_UsbOtg_getProductId(void)
{
    BspTwl3029_I2C_RegData tmpCtrlRegData_Lsb, tmpCtrlRegData_Msb;
    /* just read from shadow registers  */
    BspTwl3029_UsbIdRegister regValue;
    
    
    /* read shadow registers  */
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB, 
   BSP_TWL3029_MAP_USB_PRODUCT_ID_LSB_OFFSET, &tmpCtrlRegData_Lsb);
   
    /* read shadow registers  */
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB, 
   BSP_TWL3029_MAP_USB_PRODUCT_ID_MSB_OFFSET, &tmpCtrlRegData_Msb);
    
   regValue = (((BspTwl3029_UsbIdRegister)tmpCtrlRegData_Msb << 8)
               |((BspTwl3029_UsbIdRegister)tmpCtrlRegData_Lsb ));
    return regValue;
}

/*=============================================================================
 * Fucntion bspTwl3029_UsbOtg_TransceiverEnable
 *
 * Description:  switchs on/off analog and /or digital parts of USB transceiver
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_TransceiverEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_UsbOtg_Enable enable,
				    BspTwl3029_UsbOtg_pwrResource pwr)
{
    BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_FAILURE;
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
   
    BspTwl3029_I2C_RegData  tmpAuxToggle1Data , tmpAuxToggle2Data = 0;
    Uint8 togbSetShift = 0;  
    Uint16 count = 0;
    
   /* check validity of args */
   if ((enable > BSP_TWL3029_USBOTG_ENABLE) ||( pwr > 3))
    {
      return returnVal;
    }
    
   /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
   /* note: Power is set/reset through TOGB register and powerstaus is given in  PWRONSTATUS register  */ 
   
   if ( enable == BSP_TWL3029_USBOTG_DISABLE )
    {
        togbSetShift = 0;  /* reset shift*/
}
        
    else
    {
        /*set shift  The "set" bits in TOGGLE register is the same as the 'reset' bit shifted by 1*/
       togbSetShift = 1;
    }
    
    if ( pwr & 1)
    {
        tmpAuxToggle1Data = ( 1 << (BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_USBDR_OFFSET + togbSetShift ));
	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                              tmpAuxToggle1Data,  regInfoPtr++) ; 
	count++;
    }
    if ( pwr & 2)
    {
    	tmpAuxToggle2Data = ( 1 << (BSP_TWL3029_LLIF_AUX_REG_TOGGLE2_USBAR_OFFSET + togbSetShift ));
	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL3029_MAP_AUX_REG_TOGGLE2_OFFSET,
                                              tmpAuxToggle2Data,  regInfoPtr++) ; 
	count++;
    }

   
    
    /* now request to I2C manager to write to Triton AUX_TOGGLE registers */
   if ((count > 0 ) && (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
                                             (BspI2c_TransactionRequest*)i2cTransArrayPtr);   
}                                           


   return returnVal;    
}
    
        
