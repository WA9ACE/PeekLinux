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
 *  FILE NAME: bspTwl3029_Bci.c
 *
 *
 *  PURPOSE:  Driver  for various Twl3029 ( Triton )analog  ADCs
 *
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
 *  17/03/2004  Mary Tooher     V1.0.0   First implementation
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
#include "bspTwl3029_IntC.h"
#include "bspTwl3029_Madc.h"
#include "bspTwl3029_Bci.h"


#include "bspUtil_Assert.h"
#include "bspUtil_BitUtil.h"

#if (OP_L1_STANDALONE == 0)
#include "lcc/lcc.h"
#include "../rvm/rvm_use_id_list.h"
#endif

/*=============================================================================
 *  Defines and Macros
 */

/*=============================================================================
 * Description:
 */
/*===========================================================================
 *   static Data:
 */

 /*===========================================================================
 *   global Data:
 */

#if (OP_L1_STANDALONE == 0)

extern T_PWR_CTRL_BLOCK *pwr_ctrl;
extern uint16 lcc_charger_status;
extern uint16 lcc_USB_status;
extern BspTwl3029_MadcResults MadcResults;

void pwr_madc_callback(void);

#endif

 /* state machine */
 BspTwl3029_Bci_state bspTwl3029_Bci_state = BSP_TWL3029_BCI_IDLE_OR_PRECRG;

 /* thermal bias current polarity */
 BspTwl3029_Bci_thermSign bspTwl3029_Bci_thermBiasSign = BSP_TWL3029_BCI_THERMSIGN_NEG;

 /*=============================================================================
 *   Private Functions
 *============================================================================*/

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci
 *  Description
 *     Enables /disables anti POP
 *     There is no specific bit(s) in the two anti POP registers for
 *     enabling/disabling anti POP, however if anti POP is required the post-
 *     discharge capacitor  must always be enabled,  if not required then the
 *     post discharger capacitoe can be left disabled
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param
 *
 * @param
 *
 * @result
 *    return of type BspTwl3029_ReturnCode
 *
 */

/*=============================================================================
 *   Public Functions
 *============================================================================*/
/*=============================================================================
 * Function bspTwl3029_Bci_init
 *
 * Description: Initializes BCI,
                      enables the battery voltage and charger voltage measurement.
 */

BspTwl3029_ReturnCode
bspTwl3029_Bci_init(void)
{



     BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
     BspTwl3029_I2C_RegData regCtrl3Read = 0;

     bspTwl3029_Bci_state = BSP_TWL3029_BCI_IDLE_OR_PRECRG;

     BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET, &regCtrl3Read);

    /* clear bits 5 & 4 */
    regCtrl3Read = regCtrl3Read & 0xcf;

     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESBAT,&regCtrl3Read, 1);
     BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESVAC,&regCtrl3Read, 1);

     returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,
                                           regCtrl3Read,  NULL);
    return returnCode;

}

/*=============================================================================
 * Function bspTwl3029_Bci_dacConfig
 *
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_dacConfig(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                         BspTwl3029_Bci_dacType     types,
			 BspTwl3029_Bci_dacVal*      valArrayPtr)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regData_I, regData_V1, regData_V2 = 0;
    BspTwl3029_I2C_RegData regRead_I, regRead_V1, regRead_V2 = 0;

    BspTwl3029_Bci_dacVal*      valPtr = valArrayPtr;

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_CHGVREG1_OFFSET, &regRead_V1);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_CHGVREG2_OFFSET, &regRead_V2);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_CHGIREG_OFFSET, &regRead_I);

    if ( types & BSP_TWL3029_BCI_DACTYPE_VOLT)
    {
       /* 8 MSBs */
       regData_V1 = (BspTwl3029_I2C_RegData) ( (*valPtr >> 2)  & 0xff);
       /* 2 LSBs : store in bits 7,6 of register */
       regData_V2 = (BspTwl3029_I2C_RegData) ( (*valPtr & 3) << 6);

       if ( regData_V1 != regRead_V1)
       {
           BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_CHGVREG1_OFFSET,
                                      regData_V1,regInfoPtr++);
          count++;
       }

       if ( regData_V2 != regRead_V2)
       {
           BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_CHGVREG2_OFFSET,
                                      regData_V2,regInfoPtr++);
          count++;
       }
    }

    if ( types & BSP_TWL3029_BCI_DACTYPE_CURRENT)
    {
       /* 8 LSBs */
       regData_I = (BspTwl3029_I2C_RegData) ( *(valPtr+1)  & 0xff);
       if ( regData_I != regRead_I)
       {
           BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_CHGIREG_OFFSET,
                                      regData_I,regInfoPtr++);
          count++;
       }

    }

    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);


    return returnCode;
}


/*=============================================================================
 * Function bspTwl3029_Bci_enable
 *
 * Description:
 *       enables linear charging
 *      programmes BCICTL1: CHEN, CHIV, BCICTL2: ACPkATHEN, USBPATHEN
 *      Note
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_linChargeEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                BspTwl3029_Bci_pathType path,
		                BspTwl3029_Bci_chargeMode constVorI)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;


    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl2Data, regCtrl3Data = 0;
    BspTwl3029_I2C_RegData regCtrl1Read, regCtrl2Read = 0;

    if (( path >= 2 ) ||( constVorI >= 2))
    {
        /* invalid args */
	return returnCode;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);

    regCtrl1Data = regCtrl1Read;
    regCtrl2Data = regCtrl2Read;


    /* disable accessory supply mode  and enable either USB or AC path charging */
    /* clear 3 lsbs */
    regCtrl2Data = regCtrl2Data & 0xf8;
    /* set bit for with AC or USB path */
    regCtrl2Data |= (1 << path) ;
    /* queue I2C read */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                      regCtrl2Data,regInfoPtr++);
    count++;

    /* set CHIV bit to select either const voltage or const I charging */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHIV, &regCtrl1Data, constVorI );
    /* set CHEN  bit to 1 to enable main charge */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, 1 );


    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                      regCtrl1Data,regInfoPtr++);
    count++;

    /* if constant current then set MESBAT (BATCTL3 reg) to 1 to enable battery voltage measure */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESBAT, &regCtrl3Data, 1 );
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESVAC, &regCtrl3Data, 1 );

    /* make sure prechager off bits ( bits 7,6) are set */
    regCtrl3Data |= 0xC0;

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,
                                      regCtrl3Data,regInfoPtr++);
    count++;

    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);

    bspTwl3029_Bci_state = BSP_TWL3029_BCI_LINCRG;
    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_linChargeDisable
 *
 * Description:
 *
 *
 * Fucntion to be called only of state is BSP_TWL3029_BCI_LINCRG or
 *    BSP_TWL3029_BCI_IDLE_OR_PRECRG ( the latter is for clean up purposeds )
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_linChargeDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl2Data = 0;
    BspTwl3029_I2C_RegData regCtrl1Read, regCtrl2Read = 0;


    if ( bspTwl3029_Bci_state == BSP_TWL3029_BCI_PWMCRG )

    {
        /* return */
	return returnCode;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);

    regCtrl1Data = regCtrl1Read;
    regCtrl2Data = regCtrl2Read;


    /* disable  all charge modes   */
    /* clear 3 lsbs */

    regCtrl2Data = regCtrl2Data & 0xf8;


    /* queue I2C read */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                      regCtrl2Data,regInfoPtr++);
    count++;

    /* set CHIV bit to  const I charging as charging cycle always starts off with const I*/
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHIV, &regCtrl1Data, 1 );
    /* reset CHEN  bit to  disable main charge */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, 0 );


    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                      regCtrl1Data,regInfoPtr++);
    count++;


    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);
    if ( bspTwl3029_Bci_state == BSP_TWL3029_BCI_LINCRG )
    {
       bspTwl3029_Bci_state = BSP_TWL3029_BCI_IDLE_OR_PRECRG;
    }
    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_i2vConfig
 *
 * Description: configure I to V converter
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_i2vConfig( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                          BspTwl3029_Bci_i2vConfigInfo* configInfoPtr)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* sets  BCICTL2: OFFEN, OFFSN, CGAIN4, CLIB (??) */

    BspTwl3029_I2C_RegData regCtrl2Data = 0;
    BspTwl3029_I2C_RegData regCtrl2Read = 0;




    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);
    regCtrl2Data = regCtrl2Read;
    /* clear 5 MSBs */
    regCtrl2Data = regCtrl2Read & 0x7;

    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_OFFEN, &regCtrl2Data, configInfoPtr->offsetEn );
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_OFFSN, &regCtrl2Data, configInfoPtr->offsetVal);
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_CGAIN4, &regCtrl2Data, configInfoPtr->gain);
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_CLIB, &regCtrl2Data, configInfoPtr->calibEn);

    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                           regCtrl2Data,  callbackInfoPtr);
    return returnCode;
}


/*=============================================================================
 * Function bspTwl3029_Bci_prechargDisable
 *
 * Description: disables precharging. Note Pre charger is enabled by HW when voltage
 *               is les than 3.2V
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_prechargeDisable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* switch off precharge   */
    /* check if charging */
    /* if so keep shadow reg values for MESVAC and MESBAT else clear */
    BspTwl3029_I2C_RegData regCtrl3Data = 0;

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET, &regCtrl3Data);

    /* disable AC and USB precharging */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_PREACOFF, &regCtrl3Data, 1 );
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_PREUSBOFF, &regCtrl3Data, 1 );


    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,
                                            regCtrl3Data,  callbackInfoPtr);

    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_accessoryEnable
 *
 * Description: enable accesory supply
 *   Note: This function can only be called after first checking that CHGSTS is off
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_accessoryEnable(  BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                 BspTwl3029_Bci_pathType path)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl2Data, regCtrl3Data = 0;
    BspTwl3029_I2C_RegData regCtrl1Read, regCtrl2Read, regCtrl3Read, regPwm2Read = 0;


    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET, &regCtrl3Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET, &regPwm2Read);

    regCtrl1Data = regCtrl1Read;
    regCtrl2Data = regCtrl2Read;
    regCtrl3Data = regCtrl3Read;



    if ( regCtrl1Data & (1 << BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN_OFFSET))
    {
       /* reset CHEN  bit to  disable main charge */
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, 0 );


       BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                      regCtrl1Data,regInfoPtr++);
       count++;
    }
    if ( regPwm2Read & (1 << BSP_TWL3029_LLIF_BCI_BCIPWM2_PWMEN_OFFSET))
    {
        /* reset PWMEN  bit to  disable main charge */
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCIPWM2_PWMEN, &regPwm2Read, 0 );


       BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET,
                                      regPwm2Read,regInfoPtr++);
       count++;

    }
    /* clear  paths (2 lsbs ) */
    regCtrl2Data = regCtrl2Data & 0xfc;;

    /* now set bit for  AC or USB path */
    regCtrl2Data |= (1 << path);

    /* and enable accessory supply  by setting ACCSUPEN  */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_ACCSUPEN, &regCtrl2Data, 1 );

    /* now queue the I2C read */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                      regCtrl2Data,regInfoPtr++);
    count++;

    /* switch off  MESVAC to avoid leakage current */

    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESVAC, &regCtrl3Data, 0 );
    if ( regCtrl3Data != regCtrl3Read)
    {
       /* now queue the I2C read */
       BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,
                                      regCtrl3Data,regInfoPtr++);
       count++;
    }

    /* note enable reverse current limitation trigger  not set here - it can be set using SKEY command */

    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);

    bspTwl3029_Bci_state = BSP_TWL3029_BCI_ACCESSORY;
    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_accessoryDisable
 *
 * Description: enable accesory supply
 *   Note: Obvious place to call this  function
 *         is during  routine resulting from an the handling of an ISR event
 *         resulting from an interrrupt being generated after  LIMITEN had been
 *          programmed and I threshold reached.
 *         ( interrupt 1 - charge stop even T)
 *         The callbackInfoPtr CANNOT be defined as NULL when
 *         calling during an ISR.
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_accessoryDisable(  BspTwl3029_I2C_CallbackPtr callbackInfoPtr )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData  regCtrl2Data = 0;



    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Data);

   /* clear  paths (2 lsbs ) */
    regCtrl2Data = regCtrl2Data & 0xfc;

    /* and disable accessory supply  by setting ACCSUPEN  */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL2_ACCSUPEN, &regCtrl2Data, 0 );

    /* now queue the I2C read */
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                            regCtrl2Data,  callbackInfoPtr);
   if (  bspTwl3029_Bci_state == BSP_TWL3029_BCI_ACCESSORY )
   {
      bspTwl3029_Bci_state = BSP_TWL3029_BCI_IDLE_OR_PRECRG;
   }
   return returnCode;
}
/*=============================================================================
 * Function bspTwl3029_Bci_pwmChargeEnable
 *
 * Description: enable pwm
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_pwmChargeEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr ,
                                Uint16 period,
			        BspTwl3029_Bci_chargeMode constVorI)
{
     BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl2Data, regCtrl3Data=0, regPwm1Data, regPwm2Data = 0;
    BspTwl3029_I2C_RegData regCtrl1Read, regCtrl2Read = 0;



    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);

    regCtrl1Data = regCtrl1Read;
    regCtrl2Data = regCtrl2Read;


    /* disable accessory supply mode  and enable either USB or AC path charging */
    /* clear 3 lsbs */
    regCtrl2Data = regCtrl2Data & 0xf8;
    /* set path bit for with AC ( PWM charging cannot be used wuth USB path ) */
    regCtrl2Data |= (1 << BSP_TWL3029_LLIF_BCI_BCICTL2_ACPATHEN_OFFSET) ;
    /* queue I2C read */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                      regCtrl2Data,regInfoPtr++);
    count++;

    /* set CHIV bit to select either const voltage or const I charging */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHIV, &regCtrl1Data, constVorI );
    /* make sure that  CHEN   bit is reset to 1  */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, BSP_TWL3029_BCI_DISABLE );


    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                      regCtrl1Data,regInfoPtr++);
    count++;

    /* if constant current then set MESBAT (BATCTL3 reg) to 1 to enable battery voltage measure */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL3_MESBAT, &regCtrl3Data, BSP_TWL3029_BCI_ENABLE );

    /* make sure prechager off bits ( bits 7,6) are set */
    regCtrl3Data |= 0xC0;

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,
                                      regCtrl3Data,regInfoPtr++);
    count++;

    /* PWM charging config and enable */
    /* Load 8 MSBs of period into BCIPWM1 */
    regPwm1Data =  (Uint8)(period>> 2) ;

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM1_OFFSET,
                                      regPwm1Data,regInfoPtr++);
    count++;
    /* Load 2 LSBs of period into  bits 7,6 of BCIPWM2  and enable by setting PWMEN ( bit 5 of BCIPWM2)*/
    regPwm2Data = (Uint8)((period &  3 ) << 6);
    regPwm2Data |= (1 <<  BSP_TWL3029_LLIF_BCI_BCIPWM2_PWMEN_OFFSET);
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET,
                                      regPwm2Data,regInfoPtr++);
    count++;

    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */

    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);

    bspTwl3029_Bci_state = BSP_TWL3029_BCI_PWMCRG;

    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_pwmChargeEnable
 *
 * Description: enable pwm
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_ChangeDutyCycle( BspTwl3029_I2C_CallbackPtr callbackInfoPtr , Uint16 period)
{
 BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData  regPwm1Data,regPwm2Data;
    BspTwl3029_I2C_RegData regPwm2Read;



    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET, &regPwm2Read);
	/* Load 8 MSBs of period into BCIPWM1 */
    regPwm1Data =  (Uint8)(period>> 2) ;

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM1_OFFSET,
                                      regPwm1Data,regInfoPtr++);
    count++;

	/*clear bits 7,6 of BCIPWM2*/
	regPwm2Read = regPwm2Read & 0x3F;

	/* Load 2 LSBs of period into  bits 7,6 of BCIPWM2  */
	regPwm2Data = regPwm2Read | (Uint8)((period &  3 ) << 6);

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET,
                                      regPwm2Data,regInfoPtr++);
    count++;

    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */

    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);

    bspTwl3029_Bci_state = BSP_TWL3029_BCI_PWMCRG;

    return returnCode;


}
/*=============================================================================
 * Function bspTwl3029_Bci_pwmChargeDisable
 *
 * Description: enable pwm
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_pwmChargeDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

/* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

    /* callback function info pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr = callbackInfoPtr;

    Uint16 count = 0;

    BspTwl3029_I2C_RegData regPwm2Data, regCtrl2Data = 0;
    BspTwl3029_I2C_RegData regPwm2Read, regCtrl2Read = 0;


   if (  bspTwl3029_Bci_state == BSP_TWL3029_BCI_LINCRG )
   {
      /* pwm charging was already disabled when Lin charging was enabled */
      return returnCode;
   }

   if (  bspTwl3029_Bci_state == BSP_TWL3029_BCI_PWMCRG )
   {


    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET, &regPwm2Read);

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET, &regCtrl2Read);

    regPwm2Data = regPwm2Read;
    regCtrl2Data = regCtrl2Read;


    /* disable  all charge modes   */
    /* clear 3 lsbs */

    regCtrl2Data = regCtrl2Data & 0xf8;


    /* queue I2C read */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCICTL2_OFFSET,
                                      regCtrl2Data,regInfoPtr++);
    count++;

    /* reset PWMEN  bit to  disable pulse charge */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCIPWM2_PWMEN, &regPwm2Data, BSP_TWL3029_BCI_DISABLE );


    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIPWM2_OFFSET,
                                      regPwm2Data,regInfoPtr++);
    count++;


    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }
    /* reset reg pointer */
    regInfoPtr = regInfo;

    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend( regInfo,count,callbackPtr,
                                           (BspI2c_TransactionRequest*)i2cTransArrayPtr);



      bspTwl3029_Bci_state = BSP_TWL3029_BCI_IDLE_OR_PRECRG;
   }
   return returnCode;
}
/*=============================================================================
 * Function bspTwl3029_Bci_
 *
 * Description:
 *   There is a risk that CHEN and CHIV bits could be changed by HW or other
 *   Triton functions (if called from ISR )
 *   Work around for this is to forbid modifcation of sensors while CHEN  or PWM
 *   is enabled.
 *   So if enabled beforehand, then the sensors remain enabled and v.v.
 *   Dynamic control of the current supplies for battery and temperature
 *   should not be an issue w.r.t power saving when a charger is connected to
 *   a charger.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_sensorsEnable( BspTwl3029_I2C_CallbackPtr  callbackInfoPtr,
                              BspTwl3029_Bci_sensorType   sensorType,
			      BspTwl3029_Bci_thermCurrent thermCurrent,
			      BspTwl3029_Bci_thermSign    sign )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl1Read = 0;

    if (( bspTwl3029_Bci_state == BSP_TWL3029_BCI_PWMCRG) ||
        ( bspTwl3029_Bci_state == BSP_TWL3029_BCI_LINCRG))
    {
      /* no modification of sensor control allowed during charging */
      return returnCode;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    regCtrl1Data = regCtrl1Read;



    /* make sure CHEN is off (Note: CHEN bit in shadow reg  may not be same as actual HW status )*/
    /* clear  lsb (CHEN) */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, BSP_TWL3029_BCI_DISABLE );


    if ( sensorType & BSP_TWL3029_BCI_SENS_BATTYPE )
    {
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_TYPEN, &regCtrl1Data, BSP_TWL3029_BCI_ENABLE );

    }

    if ( sensorType & BSP_TWL3029_BCI_SENS_TEMP )
    {
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_THEN, &regCtrl1Data, BSP_TWL3029_BCI_ENABLE );
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_THSENS, &regCtrl1Data, ( thermCurrent & 0x7) );
       bspTwl3029_Bci_thermBiasSign = sign & 1;


    }

    if (regCtrl1Read != regCtrl1Data)
    {
       returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                           regCtrl1Data,  callbackInfoPtr);
    }
    else
    {
       returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
    }
    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_sensorsDisable
 *
 * Description:
 *   There is a risk that CHEN and CHIV bits could be changed by HW or other
 *   Triton functions (if called from ISR )
 *   Work around for this is to forbid modifcation of sensors while CHEN  or PWM
 *   is enabled.
 *   So if enabled beforehand, then the sensors remain enabled and v.v.
 *   Dynamic control of the current supplies for battery and temperature
 *   should not be an issue w.r.t power saving when a charger is connected to
 *   a charger.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_sensorsDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                               BspTwl3029_Bci_sensorType  sensorType )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    BspTwl3029_I2C_RegData regCtrl1Data, regCtrl1Read = 0;

    if (( bspTwl3029_Bci_state == BSP_TWL3029_BCI_PWMCRG) ||
        ( bspTwl3029_Bci_state == BSP_TWL3029_BCI_LINCRG))
    {
      /* no modification of sensor control allowed during charging */
      return returnCode;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_BCI,
                                  BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET, &regCtrl1Read);

    regCtrl1Data = regCtrl1Read;



    /* make sure CHEN is off (Note: CHEN bit in shadow reg  may not be same as actual HW status )*/
    /* clear  lsb (CHEN) */
    BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_CHEN, &regCtrl1Data, BSP_TWL3029_BCI_DISABLE );


    if ( sensorType & BSP_TWL3029_BCI_SENS_BATTYPE )
    {
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_TYPEN, &regCtrl1Data, BSP_TWL3029_BCI_DISABLE );

    }

    if ( sensorType & BSP_TWL3029_BCI_SENS_TEMP )
    {
       BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_BCI_BCICTL1_THEN, &regCtrl1Data, BSP_TWL3029_BCI_DISABLE );
    }

    if (regCtrl1Read != regCtrl1Data)
    {
       returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCICTL1_OFFSET,
                                           regCtrl1Data,  callbackInfoPtr);
    }
    else
    {
       returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
    }
    return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_
 *
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_GetStatus( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                          BspTwl3029_I2C_RegId regAdr,
			  BspTwl3029_I2C_RegData* regStatus )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* sets   */


   returnCode = BspTwl3029_I2c_ReadSingle( BSP_TWL3029_I2C_BCI,regAdr,
                                           regStatus,  callbackInfoPtr);

   return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_SetSkey
 *
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_SetSkey( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
			  BspTwl3029_Bci_skey skey )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* sets   */


   returnCode = BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_BCI,BSP_TWL3029_MAP_BCI_BCISKEY_OFFSET,
                                            skey,  callbackInfoPtr);

   return returnCode;
}

/*=============================================================================
 * Function bspTwl3029_Bci_SetSkey
 *
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Bci_SetWatchDog( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
			    BspTwl3029_Bci_wdkey watchdogKey )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    /* sets   */


   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_BCI, BSP_TWL3029_MAP_BCI_BCIWDKEY_OFFSET,
                                           watchdogKey,  callbackInfoPtr);

   return returnCode;
}
#if (OP_L1_STANDALONE == 0)
#if (ANLG_FAM == 11)
/*=============================================================================
 * Function pwr_madc_callback_test
 *
 * Description:
 */
void
pwr_madc_callback(void)
{
	struct pwr_adc_ind_s *addr;

	rvf_send_trace("IQ EXT: ADC End",15, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
    // Send ADC measurement to LCC task
    // NOTE that memory is allocated externally in the LCC task
       if (rvf_get_buf(pwr_ctrl->prim_id, sizeof(struct pwr_adc_ind_s), (void *)&addr) == RVF_RED) {
          rvf_send_trace("rvf_get_buf failed",18, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
          rvf_dump_mem();
          return;
	}
       addr->header.msg_id        = PWR_ADC_IND;
       addr->header.src_addr_id   = (T_RVF_ADDR_ID)NULL_PARAM;
       addr->header.dest_addr_id  = pwr_ctrl->addr_id;
       addr->header.callback_func = NULL;
       addr->data[0] = MadcResults.adc10_vbat;
	addr->data[1] = MadcResults.adc9_vchg;
	addr->data[2] = MadcResults.adc8_ichg;
	addr->data[3] = MadcResults.adc7_vbkp;
	addr->data[4] = MadcResults.adc4_bt;
	addr->data[5] = MadcResults.adc5_btemp;
	addr->data[6] = MadcResults.adc1;
	addr->data[7] = MadcResults.adc2;
	addr->data[8] = MadcResults.adc3;
       addr->data[9] = lcc_charger_status;
	addr->data[10] = lcc_USB_status;
	addr->data[11] = MadcResults.adc6_usbvbus;

    	if (rvf_send_msg(pwr_ctrl->addr_id, addr) != RV_OK) {
          rvf_send_trace("SPI FATAL: Send failed!",23, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
       }
}
#endif
#endif
