/*=============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */


#include "bspUtil_Assert.h"
#include "types.h"

#include "bspTwl3029.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"
#include "bspTwl3029_Sim.h"
#include "bspTwl3029_Power.h"
#include "sys_types.h"
#include "types.h"
#include "uicc/board/bspUicc_Power.h"

enum {
    /* 8 milli Second delay */
    BSP_TWL3029_MAP_SIM_SIMDEBOUNCING_DEFAULT = 0xF0,
    /*                     VRIO
     *                      |
     *    VRSIM            [ ]
     *      |               |_
     *     [ ] 10 K          _||--CSIMDTCPUZ
     *      |SIMDTC Pin     |
     *      |---------------|
     *SIM| _|               |_
     *  -| _                 _||--CSIMDTCPD
     *CON|  |               |
     *     ---             [ ] 400 K
     *     ///              |
     *                     ---
     *                     ///
     *If CSIMDTCPD bit = 1 and CSIMDTCPUZ = 1 (pull down requested), and if the SIM 
     *card is not in-serted, SIMDTC pin will always be pulled up, as 10K <<  400K.
     * 
     *If CSIMDTCPD bit = 0 and CSIMDTCPUZ = 0 (pull up requested), and if the SIM card is
     * not inserted, SIMDTC pin will be pulled up.
     *
     *If SIM card is inserted, SIMDTC pin is tied to the ground.
     */
    BSP_TWL3029_MAP_SIM_SIMDTCCTRL_DEFAULT = 0,
    BSP_TWL3029_MAP_SIM_SIMDTCENABLE_DEFAULT = 0
};



enum {
    BSP_TWL3029_MAP_SIM_SIMDTCENABLE_DISABLE = 0,
    BSP_TWL3029_MAP_SIM_SIMDTCENABLE_ENABLE = 1
};

#define SLEEP_LOW_PWR	0x8
#define DEV_MODEM	0x20

/*==================================================================
 * init function
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_init()
{
   BspTwl3029_I2C_RegisterInfo regArray[10];
   BspTwl3029_I2C_RegisterInfo*  regArrayPtr = regArray;
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
   
   BspI2c_TransactionRequest i2cArray[15];  
   BspI2c_TransactionRequest* i2cArrayPtr = i2cArray;

   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_SIM,
       BSP_TWL3029_MAP_SIM_SIMDEBOUNCING_OFFSET,BSP_TWL3029_MAP_SIM_SIMDEBOUNCING_DEFAULT,regArrayPtr++);
    
   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_SIM,
       BSP_TWL3029_MAP_SIM_SIMDTCCTRL_OFFSET,BSP_TWL3029_MAP_SIM_SIMDTCCTRL_DEFAULT,regArrayPtr++);

   /* now send to I2C */
   regArrayPtr = regArray;
   /* now request to I2C manager to write to Triton registers */
   BspTwl3029_I2c_regInfoSend(regArrayPtr,2,NULL, i2cArrayPtr);      
   /* set up voltage and enable detection */
   bspTwl3029_Sim_setUpVoltage(NULL, BSP_TWL3029_SIM_VOLTAGE_18V);
#if(OP_L1_STANDALONE==0)
   bspUicc_Init_sim();
#endif
   return returnCode;  
}

/*=============================================================================
 * Description:
 *   Increase the power level
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_setUpVoltage( BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                             BspTwl3029_SimVoltage newVoltage )
{
   BspTwl3029_I2C_RegisterInfo regArray[10];
   BspTwl3029_I2C_RegisterInfo*  regArrayPtr = regArray;
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
   
   Bsp_Twl3029_I2cTransReqArray i2cArray;  
   Bsp_Twl3029_I2cTransReqArrayPtr i2cArrayPtr = &i2cArray;
   
   /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
   
 
   /* DEV_GRP Off bit of VRSIM_CFG_STS  */
//   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
//      BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_STS_OFFSET,0x8,regArrayPtr++);

   /* VSEL bit of VRSIM_CFG_DEDICATED  */
   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
      BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_DEDICATED_OFFSET,newVoltage,regArrayPtr++);

   /* DEV_GRP On bit of VRSIM_CFG_STS  */
//   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
//      BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_STS_OFFSET,0x3F,regArrayPtr++);


   /* now send to I2C */
   regArrayPtr = regArray;
   /* now request to I2C manager to write to Triton registers */
   returnCode = BspTwl3029_I2c_regInfoSend(regArrayPtr,1,callbackFuncPtr,
                                           (BspI2c_TransactionRequest*)i2cArrayPtr);      

   return returnCode;
}


/*==============================================================================*/
/*!
 *    Enable the the SIM module and detection module.
 */

BspTwl3029_ReturnCode
bspTwl3029_Sim_enable(  BspTwl3029_I2C_CallbackPtr    callbackFuncPtr)
{
   
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;   
   BspTwl3029_I2C_RegData regData = 0;
        
      
   /*  set SIM detect bit  */
   /* update the corresponding field */
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &regData,
                                  1,
                                  BSPUTIL_BITUTIL_DATAUNIT_8,
                                  BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_SIMS_OFFSET,
                                  BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_SIMS_WIDTH );
   
//   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_SIM, BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
//                                regData,  callbackFuncPtr) ; 
//   bspTwl3029_Power_setDevGrp ( NULL,BSP_TWL3029_POWER_VRSIM, BSP_TWL3029_POWER_DEV_GRP_MODEM );
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_SIM, BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                regData,  NULL) ; 
   bspTwl3029_Power_setDevGrp ( callbackFuncPtr,BSP_TWL3029_POWER_VRSIM, BSP_TWL3029_POWER_DEV_GRP_MODEM );
   /* pwr management functions. Moved from power_init.c //	BSP_TWL3029_CONFIG_RESOURCE(VRSIM , DEV_MODEM , SLEEP_LOW_PWR ); */

   BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_STS_OFFSET, (DEV_MODEM), NULL); 
   BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_STATE_OFFSET, (SLEEP_LOW_PWR), NULL);
   return returnCode;  
}


/*==============================================================================*/
/*!
 *    Disable the the SIM module and detection module.
 */
BspTwl3029_ReturnCode
bspTwl3029_Sim_disable(BspTwl3029_I2C_CallbackPtr    callbackFuncPtr)
{
   
    
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;   
   BspTwl3029_I2C_RegData regData = 0;
      
  
   /* read shadow registers  */
      BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUX,  BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET, &regData);
     
      
   /*  reset SIM detect bit */
   /* update the corresponding field */
    BSPUTIL_BITUTIL_BIT_FIELD_SET( &regData,
                                   1,
                                   BSPUTIL_BITUTIL_DATAUNIT_8,
                                   BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_SIMR_OFFSET,
                                   BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_SIMR_WIDTH );
   
   
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_SIM, BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                regData,  callbackFuncPtr) ; 
   bspTwl3029_Power_setDevGrp ( NULL,BSP_TWL3029_POWER_VRSIM,0 );   
   return returnCode;   
}
