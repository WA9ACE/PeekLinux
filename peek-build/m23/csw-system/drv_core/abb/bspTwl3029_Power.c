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
 *  FILE NAME: bspTwl3029_IntC.c
 *
 *
 *  PURPOSE:  All Twl3029 ( Triton ) interrrupt management related functions
 *
 *    This file contains the functions and defines needed to register the Triton
 *    TWL3029 with the main interrupt controller and also for other handlers to
 *    register with the TWL3029 interrupt Controller. The TWL3029 is itself an
 *    interrupt controller that feeds its output Interrrupt lines into the main 
 *    MCU interrupt controller.
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
 *  20/01/2005  Mary Tooher     V1.0.0   First implementation
 */
 
 

/*=============================================================================
 *   includes
 */
#include "types.h"
 
#include "bspUtil_Assert.h"
#include "bspTwl3029.h"
#include "bspTwl3029_Intc.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Power.h"
#include "bspUtil_BitUtil.h"

/*=============================================================================
 *   Defines and Macros
 *============================================================================*/

/*=============================================================================
 *  data
 *============================================================================*/
BspTwl3029_I2C_RegId  cfgStsAddrArray[] =
{
   BSP_TWL_3029_MAP_PMC_SLAVE_VCORE_CFG_STS_OFFSET,           /*  PMC_SLAVE    VCORE_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRSIM_CFG_STS_OFFSET,           /*  PMC_SLAVE    VRSIM_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRMMC_CFG_STS_OFFSET ,          /*  PMC_SLAVE    VRMMC_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VREXTH_CFG_STS_OFFSET ,         /*  PMC_SLAVE    VREXTH_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRPLL_CFG_STS_OFFSET ,          /*  PMC_SLAVE    VRPLL_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRIO_CFG_STS_OFFSET ,           /*  PMC_SLAVE    VRIO_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRMEM_CFG_STS_OFFSET ,          /*  PMC_SLAVE    VRMEM_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRUSB_CFG_STS_OFFSET ,          /*  PMC_SLAVE    VRUSB_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRABB_CFG_STS_OFFSET ,          /*  PMC_SLAVE    VRABB_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VRVBUS_CFG_STS_OFFSET ,         /*  PMC_SLAVE    VRVBUS_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_VREXTL_CFG_STS_OFFSET ,         /*  PMC_SLAVE    VREXTL_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_EXT_REGEN_CFG_STS_OFFSET ,      /*  PMC_SLAVE    EXT_REGEN_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_CBAT32_CFG_STS_OFFSET ,         /*  PMC_SLAVE    CBAT32_CFG_STS  */

   BSP_TWL_3029_MAP_PMC_SLAVE_BGAP_CFG_STS_OFFSET ,           /*  PMC_SLAVE    BGAP_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_BGSLP_CFG_STS_OFFSET ,          /*  PMC_SLAVE    BGSLP_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_TSHUTDOWN_CFG_STS_OFFSET ,      /*  PMC_SLAVE    TSHUTDOWN_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_BIAS_CFG_STS_OFFSET ,           /*  PMC_SLAVE    BIAS_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_CLKON_CFG_STS_OFFSET ,          /*  PMC_SLAVE    CLKON_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_STATE_MNGT_CFG_STS_OFFSET ,     /*  PMC_SLAVE    STATE_MNGT_CFG_STS  */
   BSP_TWL_3029_MAP_PMC_SLAVE_SLEEP_MNGT_CFG_STS_OFFSET ,     /*  PMC_SLAVE    SLEEP_MNGT_CFG_STS  */

   BSP_TWL_3029_MAP_PMC_SLAVE_BAT_PRES_CHECK_CFG_STS_OFFSET , /*  PMC_SLAVE    BAT_PRES_CHECK_CFG_STS  */
} ;
/*=============================================================================
 *   Private Functions
 *============================================================================*/

/*=============================================================================
 *   Public Functions
 *============================================================================*/
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_init
 *
 * @discussion
 *  Description 
 *    Used to initialize the power management:
 *    
 *    - configure any power sourse that require configuration ( excluding 
 *      those configured during the primary boot )
 *       patch for PG1.x : make sure that BIAS_CFG_STATE = 0x0f  
 *       (remap sleep to active on BIAS , to be removed on PG2.x) 
 *       
 *    - ( optional) Provide sequencer software patch. This isn't done
 *      at present, however if required, it needs to be done from this
 *       function
 * :
 *    -  set bit 7 of P2_CFG_TRANSITION = 1 (P2_LVL_WAKEUP) (change WAKEUP2 detection from edge to level)
 *       and bit 7 of P3_CFG_TRANSITION = 1 (P3_LVL_WAKEUP)  (change PCLKREQ detection from edge to level)
 *           
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_init( void )
{  
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */
    BspI2c_TransactionRequest i2cTransArray[12];
    BspI2c_TransactionRequest* i2cTransArrayPtr= i2cTransArray;
        

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[11] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
 
    Uint8 count = 0;
     
    /* set BIAS_CFG_STATE to  0x0f ( i.e. sleep state set to ACTIVE*/ 
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
                                           BSP_TWL_3029_MAP_PMC_SLAVE_BIAS_CFG_STATE_OFFSET, 
                                           0x0f,regInfoPtr++);  
    count++;  
     /* 
       both REGEN and CKEN need to be in Modem dev_grp 
       on hardware reset CKEN is already assigned to both Modem and also to Peripheral dev_grps. 
       Peripherals such as BT require the CKEN signal. 
     */	 
    /* Assign REGEN to Modem dev_grp  */  
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
                                           BSP_TWL_3029_MAP_PMC_SLAVE_EXT_REGEN_CFG_STS_OFFSET, 
                                           0x30,regInfoPtr++);
    count++;					   
    
    /* Assign CKEN to Modem dev_grp only however this means that BT won't work if Modem is in deep sleep */  
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
                                           BSP_TWL_3029_MAP_PMC_SLAVE_CLKON_CFG_STS_OFFSET, 
                                           0x30,regInfoPtr++);
    count++;
    /* Assign VRIO to Modem dev_grp only however this means that BT won't work if Modem is in deep sleep */  
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
                                           BSP_TWL_3029_MAP_PMC_SLAVE_VRIO_CFG_STS_OFFSET, 
                                           0x30,regInfoPtr++);
    count++;
    /*
       set bit 7 of P2_CFG_TRANSITION = 1 (P2_LVL_WAKEUP) (change WAKEUP2 detection from edge to level)
       ( leave bits 0 - 5 at their  reset value of 0x1f ) 
   */  
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
                                           BSP_TWL_3029_MAP_PMC_MASTER_P2_CFG_TRANSITION_OFFSET, 
                                           0x9f,regInfoPtr++);  
    count++;
    /*
       set bit 7 of P3_CFG_TRANSITION = 1 (P3_LVL_WAKEUP) (change WAKEUP3 detection from edge to level)
       ( leave bits 1 - 3,5 at their  reset value of 0x17 ) 
       disable bit 4.   (USB detection). 
   */  
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
                                           BSP_TWL_3029_MAP_PMC_MASTER_P3_CFG_TRANSITION_OFFSET, 
                                           0x9F,regInfoPtr++);  
    count++;

    /* Make WAKEUP1 detection level sensitive */
    returnVal = BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_MASTER,
                                BSP_TWL_3029_MAP_PMC_MASTER_P1_CFG_TRANSITION_OFFSET,
                                0x9F,
                                regInfoPtr++ );  
    count++;

 
    /* Make VRDBB voltage drop to VFLOOR=1.07V in SLEEP mode, independent of if
     * VMODE is enabled. If VMODE is enabled, but not activated in SLEEP mode, a
     * voltage ramp-up will still be performed when coming out of SLEEP mode, but
     * not when entering. With VMODE_STEP=15, VFLOOR=1.07V and VROOF=1.31, VROOF
     * will be reached ~5ms after WAKEUP1 is activated. 
     */
    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                BSP_TWL_3029_MAP_PMC_SLAVE_VCORE_CFG_FLOOR_OFFSET,
                                0x24,
                                regInfoPtr++ );  
    count++;

    /* Set Etii_locosto_psp_auto_idleXT_REGEN_CFG_STATE to shutdown RF LDOs when entering deep sleep */
    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                BSP_TWL_3029_MAP_PMC_SLAVE_EXT_REGEN_CFG_STATE_OFFSET,
                                0x00,
                                regInfoPtr++ );  
    count++;

    /* Set bit BBCHGEN = 1 and bits BBSEL= 01 (3.2V) */
    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                BSP_TWL_3029_MAP_PMC_SLAVE_BBSPOR_CFG_OFFSET,
                                0x1a,
                                regInfoPtr++ );  
    count++;

    /*Enable clock gating */
	
    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PAGE1,
                                BSP_TWL_3029_MAP_PMC_MASTER_CLOCK_GATING_OFFSET,
                                0xFF,
                                regInfoPtr++ );  
    count++; 	

#if 0    /* BT deep-sleep related changes */
    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                BSP_TWL_3029_MAP_PMC_SLAVE_BGAP_CFG_STS_OFFSET,
                                0xa0,
                                regInfoPtr++ );  
    count++;

    BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                BSP_TWL_3029_MAP_PMC_SLAVE_BIAS_CFG_STS_OFFSET,
                                0xa0,
                                regInfoPtr++ );  
    count++;    
#endif   
	   
    regInfoPtr = regInfo;
    /* now request to I2C manager to write to Triton registers */
    returnVal = BspTwl3029_I2c_regInfoSend(regInfoPtr,(Uint16)count,NULL,
                                           i2cTransArrayPtr);

 #define SLEEP_ON	0xf
#define SLEEP_LOW_PWR	0x8
#define SLEEP_OFF	0x0
#define DEV_MODEM	0x20
#define DEV_PERIPHERAL	0x80
#define DEV_MSK_EN	0x10
    
#define BSP_TWL3029_CONFIG_RESOURCE(resource, sts, state) 	BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_ ## resource ## _CFG_STS_OFFSET, (sts), NULL); \
	BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_ ## resource ## _CFG_STATE_OFFSET, (state), NULL);

	BSP_TWL3029_CONFIG_RESOURCE(VCORE, 0, 0);
//	BSP_TWL3029_CONFIG_RESOURCE(VRSIM , DEV_MODEM , SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(VRMMC , DEV_MODEM | DEV_MSK_EN, SLEEP_OFF );
	BSP_TWL3029_CONFIG_RESOURCE(VREXTH , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_OFF );
	BSP_TWL3029_CONFIG_RESOURCE(VRPLL , DEV_MODEM | DEV_MSK_EN , SLEEP_LOW_PWR ); // Maybe we could switch it off
	BSP_TWL3029_CONFIG_RESOURCE(VRIO , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN , SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(VRMEM , DEV_MODEM | DEV_MSK_EN , SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(VRUSB , DEV_MODEM | DEV_MSK_EN, SLEEP_OFF); ///changed from SLEEP_LOW_PWR to SLEEP_OFF for Power consumption optimisations : by Aneesh
	BSP_TWL3029_CONFIG_RESOURCE(VRABB , DEV_MODEM | DEV_MSK_EN, SLEEP_OFF ); // Maybe we couls switch OFF
	BSP_TWL3029_CONFIG_RESOURCE(VRVBUS , 0 , 0);
	BSP_TWL3029_CONFIG_RESOURCE(VREXTL , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(EXT_REGEN , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_ON );
	BSP_TWL3029_CONFIG_RESOURCE(CBAT32 , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, 0xee );
	BSP_TWL3029_CONFIG_RESOURCE(BGAP , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(BGSLP , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, 0xff );
	BSP_TWL3029_CONFIG_RESOURCE(TSHUTDOWN , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_OFF );
	BSP_TWL3029_CONFIG_RESOURCE(BIAS , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(CLKON , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(STATE_MNGT , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_ON );
	BSP_TWL3029_CONFIG_RESOURCE(SLEEP_MNGT , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, SLEEP_LOW_PWR );
	BSP_TWL3029_CONFIG_RESOURCE(BAT_PRES_CHECK , DEV_MODEM | DEV_PERIPHERAL | DEV_MSK_EN, 0xee );

	BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_MASTER, BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_OFFSET, 0, NULL); // Disable free-running power bus
	BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_VREXTL_CFG_DEDICATED_OFFSET, 1, NULL); // CORE = 1.3V active, 1.05V sleep
//	BspTwl3029_I2c_WriteSingle( BSP_TWL3029_I2C_PMC_SLAVE, BSP_TWL_3029_MAP_PMC_SLAVE_VRPLL_CFG_DEDICATED_OFFSET, 0, NULL); // PLL = 1.05V sleep  
    
   return returnVal;
}


BspTwl3029_ReturnCode
bspTwl3029_Power_enableSleep( BspTwl3029_Power_DevGrp modem,
                              BspTwl3029_Power_DevGrp application,
                              BspTwl3029_Power_DevGrp peripheral )
{
    Uint16 registerCount = 0;
    
    BspTwl3029_I2C_RegData cfgTransition;
    
    BspTwl3029_I2C_RegisterInfo registerInfoTable[3];
    BspI2c_TransactionRequest transactionRequestTable[4];

    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;

    
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_MASTER,
                                  BSP_TWL_3029_MAP_PMC_MASTER_P1_CFG_TRANSITION_OFFSET,
                                  &cfgTransition );

    if( BSPUTIL_BITUTIL_GET( &cfgTransition,
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P1_CFG_TRANSITION_WAKEUP ) !=

        ( modem ? 1 : 0 ) )
    {
        BSPUTIL_BITUTIL_SET( &cfgTransition,
                             ( modem ? 1 : 0 ),
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P1_CFG_TRANSITION_WAKEUP );

        BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                    BSP_TWL_3029_MAP_PMC_MASTER_P1_CFG_TRANSITION_OFFSET,
                                    cfgTransition,
                                    &registerInfoTable[ registerCount ] );

        registerCount++;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_MASTER,
                                  BSP_TWL_3029_MAP_PMC_MASTER_P2_CFG_TRANSITION_OFFSET,
                                  &cfgTransition );

    if( BSPUTIL_BITUTIL_GET( &cfgTransition,
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P2_CFG_TRANSITION_WAKEUP ) !=

        ( application ? 1 : 0 ) )
    {
        BSPUTIL_BITUTIL_SET( &cfgTransition,
                             (application ? 1 : 0 ),
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P2_CFG_TRANSITION_WAKEUP );

        BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                    BSP_TWL_3029_MAP_PMC_MASTER_P2_CFG_TRANSITION_OFFSET,
                                    cfgTransition,
                                    &registerInfoTable[ registerCount ] );

        registerCount++;
    }

    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_MASTER,
                                  BSP_TWL_3029_MAP_PMC_MASTER_P3_CFG_TRANSITION_OFFSET,
                                  &cfgTransition );

    if( BSPUTIL_BITUTIL_GET( &cfgTransition,
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P3_CFG_TRANSITION_P3_LVL_WAKEUP ) !=

        ( peripheral ? 1 : 0 ) )
    {
        BSPUTIL_BITUTIL_SET( &cfgTransition,
                             ( peripheral ? 1 : 0 ),
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_MASTER_P3_CFG_TRANSITION_P3_LVL_WAKEUP );

        BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                    BSP_TWL_3029_MAP_PMC_MASTER_P3_CFG_TRANSITION_OFFSET,
                                    cfgTransition,
                                    &registerInfoTable[ registerCount ] );

        registerCount++;
    }
    

    if( registerCount > 0 )
    {
        returnCode = BspTwl3029_I2c_regInfoSend( registerInfoTable,
                                                 registerCount,
                                                 NULL,
                                                 transactionRequestTable );     
    }
   
   return returnCode;
}

BspTwl3029_ReturnCode
bspTwl3029_Power_setVrdbbSleepVoltage( BspTwl3029_Power_VrdbbVoltage voltage )
{
    Uint16 registerCount = 0;
    
    BspTwl3029_I2C_RegData vcoreCfgVfloor;
    
    BspTwl3029_I2C_RegisterInfo registerInfoTable[1];
    BspI2c_TransactionRequest transactionRequestTable[2];

    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;

    BSPUTIL_ASSERT( voltage <= BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P31V );
    
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_SLAVE,
                                  BSP_TWL_3029_MAP_PMC_SLAVE_VCORE_CFG_FLOOR_OFFSET,
                                  &vcoreCfgVfloor);

    if( BSPUTIL_BITUTIL_GET( &vcoreCfgVfloor,
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_FLOOR_VFLOOR ) !=

        voltage )
    {
        BSPUTIL_BITUTIL_SET( &vcoreCfgVfloor,
                             voltage,
                             BSPUTIL_BITUTIL_DATAUNIT_8,
                             BSP_TWL3029_LLIF_PMC_SLAVE_VCORE_CFG_FLOOR_VFLOOR );

        BspTwl3029_I2c_regQueWrite( BSP_TWL3029_I2C_PMC_SLAVE,
                                    BSP_TWL_3029_MAP_PMC_SLAVE_VCORE_CFG_FLOOR_OFFSET,
                                    vcoreCfgVfloor,
                                    &registerInfoTable[ registerCount ] );

        registerCount++;
    }

    if( registerCount > 0 )
    {
        returnCode = BspTwl3029_I2c_regInfoSend( registerInfoTable,
                                                 registerCount,
                                                 NULL,
                                                 transactionRequestTable );     
    }
   
   return returnCode;  
}

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_enable
 *
 * @discussion
 *  Description 
 *    Enable /disable a power resource by sending an power bus command 
 *    over I2C interface
 *
 *    this is for use with LDOs that need to be dynamicaly enabled /disabled
 *    LDOS: USB, SIM and MMC  
 *    
 * Note: This function enables I2C access to the power bus. This access cannot
 *      be disabled until the required power source has been enabled/disabled 
 *      which happens after a delay.       
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_enable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr, 
                        BspTwl3029_Power_Id resource, 
			BspTwl3029_Power_State state)

{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
      
    /*  i2c transction array */
    BspI2c_TransactionRequest i2cTransArray[6];  
    BspI2c_TransactionRequest* i2cTransArrayPtr = i2cTransArray;
      
    Uint16 count = 0;
    Uint16 pwmCmd = 0;
           
    BspTwl3029_I2C_RegId cfgStsAddr = 0;
    BspTwl3029_I2C_RegData pbCfgI2cData = 0;
    

    Uint8 devGrp, resId = 0;
    
              
    /* check if args  are  valid */
    if ((( state != BSP_TWL3029_POWER_STATE_OFF ) 
        && (state != BSP_TWL3029_POWER_STATE_SLEEP )
	&& (state != BSP_TWL3029_POWER_STATE_ACTIVE ))\
	|| (resource > BSP_TWL3029_POWER_BAT_PRES_CHECK) )
    {
        /* invalid arg */
	return BSP_TWL3029_RETURN_CODE_FAILURE;
    }	
    
    /* get DEV_GRP and RES_ID from shadow memory)  */
    cfgStsAddr =  cfgStsAddrArray[resource];
    /* get DevGrp  ( bits 5:7 )of CFG_STS reg*/
    returnCode =  BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_SLAVE,
                                  cfgStsAddr,
                                  &devGrp);
    
    /* get  res_id */	
    returnCode =  BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_SLAVE,
                                  (cfgStsAddr + RES_ID_OFFSET),
                                  &resId);
				  
   /* if device has not previously been assigned to a group then the power cannot be enabled/diabled */
   if (( devGrp  & 0xe0)== 0 )
   {
      return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   
   
   /* check that no ongoing I2C power bus command is in progress 
       Queuing of I2C power bus commands is not supported */
   
   returnCode =  BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_MASTER,
                                  BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
                                  &pbCfgI2cData);
    
   if ((pbCfgI2cData & 1) != 0)
   {
      /* I2C is connectted to power bus: Interpret this as 
         ongoing power bus I2C command  currently in progress */
      
      /* note: No point in checking the status bit (bit 7  STS_PB_I2C_BUSY ) as it 
        doesn't apppear to get reset to 0 (in Triton harware reg)
	on completion of the I2C PB command for Triton hardware PG 1.x .
	Bit 0 is checked instead. This bit is controlled by 
	software (i.e by this driver )
      */
      return BSP_TWL3029_RETURN_CODE_FAILURE;	 
   }
   
   
   /* now form 16 bit PWM word   */
   /* bits 13:15 DEV_GRP  
     bit 12 MT
     bit 4:11 RES_ID
     bits 0:3 RES_STATE
   */
   pwmCmd = ((Uint16)(devGrp & 0xe0)  << 8 ) | 
           (BSP_TWL3029_POWER_MESSAGE_TYPE_SINGULAR << 12 ) |
	   ((Uint16)resId << 4) | 
  	   state ;
    
    /* now access PWM bus via I2C */	    			  			  

    /*Power BUS : set bit 0, address 35, reg: PB_CFG_I2C, bit BWEN */
     /* this allows I2C access to power bus */      

   if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
             (1 << BSP_TWL3029_LLIF_PMC_MASTER_PB_CFG_I2C_PB_I2C_BWEN_OFFSET),regInfoPtr++);
	count++;
    } 
    
    /*Power BUS : write via I2C  address I2C word_C, bits 15 - 8 of PMB command   */
        if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_I2C_WORD_C_OFFSET,
             (Uint8)(pwmCmd >> 8),
	     regInfoPtr++);
        count++;
    } 
    
        /*Power BUS : write via I2C  address I2C word_D, bits 0-7 of PMB command   */
        if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_I2C_WORD_D_OFFSET,
             (Uint8)(pwmCmd & 0xff),
	     regInfoPtr++);
        count++;
    } 
    
    /*  release I2C access to power bus be resetting BWEN (bit 0 ) */
/*    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
             0x00,regInfoPtr++);
        count++;
    }  
*/

    /* now prepare for request to I2C manager to write to Triton registers */
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackInfoPtr != NULL)
    {	       	
        /* set pointer to point to correct transaction stuct - not the local one defined above */ 
	i2cTransArrayPtr = (BspI2c_TransactionRequest*) callbackInfoPtr->i2cTransArrayPtr;

    }

    /* reset reg pointer */
    regInfoPtr = regInfo;
     
    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend(regInfoPtr,count,callbackInfoPtr,i2cTransArrayPtr);
     
    return returnCode;
}			
 
 
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_getCfg_Sts
 *
 * @discussion
 *  Description 
 *    reads  CFG_STS register for a given power resource. 
 *    
 *           
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_getCfg_Sts(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                             BspTwl3029_Power_Id powerSource,
			     BspTwl3029_I2C_RegData *cfgStatus  )

{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegId cfgStsAddr; 
    if (powerSource > BSP_TWL3029_POWER_BAT_PRES_CHECK)
    {
      /* invalid arg */
      return returnCode;
    }
    else
    {
        cfgStsAddr = cfgStsAddrArray[powerSource];
    }    
    
    BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_SLAVE,cfgStsAddr, 
                              cfgStatus, callbackInfoPtr); 
    
    return returnCode;
        
} 
               
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_I2cAccessDisable
 *
 * @discussion
 *  Description 
 *    disables access to power bus.
 *    Access to power bus is automatically enabled by bspTwl3029_Power_enable 
 *   command. 
 *    
 *           
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_I2cAccessDisable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr )

{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
             0x00,callbackInfoPtr);
    
    return returnCode;    
}    


/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_setDevGrp
 *
 * @discussion
 *  Description 
 *    Assigns a power resource to resource group(s)
 *    
 *           
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_setDevGrp(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                            BspTwl3029_Power_Id powerSource, 
			    BspTwl3029_Power_DevGrp group)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegId cfgStsAddr; 
    BspTwl3029_I2C_RegData cfgStsRead, cfgStsData  = 0;
    
    if ((powerSource > BSP_TWL3029_POWER_BAT_PRES_CHECK) ||
       (group > 7))
    {
      /* invalid arg(s) */
      return returnCode;
    }
    else
    {
        cfgStsAddr = cfgStsAddrArray[powerSource];

    }    
    
    returnCode =  BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PMC_SLAVE,
                                  cfgStsAddr,
                                  &cfgStsRead);
    /* set 3 MSBs to devgrp */
    cfgStsData = (cfgStsRead & 0x10 ) | (group << 5);
        
    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PMC_SLAVE,cfgStsAddr, 
                              cfgStsData, callbackInfoPtr); 
			      
    return returnCode;
}			      

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_SwitchOnStatus
 *
 * @discussion
 * <b> Description </b><br>
 *    Reads the Hardware Status for the cause of Switch on.
 *   
 *
 *  @result    BspTwl3029_ReturnCode
 */


BspTwl3029_ReturnCode
bspTwl3029_Power_SwitchOnStatus(Uint8 *status)
{
	BspTwl3029_ReturnCode ret_val;

	ret_val = BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER, 
		BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,status, NULL);
	return ret_val;
}

static Bsp_Twl3029_I2cTransReqArray i2cTransArray;
static BspTwl3029_I2C_Callback i2cCallback={NULL,10,&i2cTransArray};
static BspTwl3029_I2C_Callback i2cCallback1={NULL,10,&i2cTransArray};

BspTwl3029_ReturnCode
bspTwl3029_Power_SwitchOnStatusWithCallback(Uint8 *status,BspI2c_TransactionDoneCallback callbackPtr)
{
	BspTwl3029_ReturnCode ret_val;
	i2cCallback.callbackFunc=callbackPtr;
	ret_val = BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER, BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,status, &i2cCallback);
	return ret_val;
}

bspTwl3029_Power_SwitchOnStatusWithCallbackval(Uint8 *status,BspI2c_TransactionDoneCallback callbackPtr,
	                                           BspI2c_TransactionId callback_val)
{
	BspTwl3029_ReturnCode ret_val;
	i2cCallback1.callbackFunc=callbackPtr;
	i2cCallback1.callbackVal=callback_val;
	ret_val = BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PMC_MASTER, BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,status, &i2cCallback1);
	return ret_val;
}
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_SwitchOff
 *
 * @discussion
 * <b> Description </b><br>
 *    Power Off the System
 *   
 *
 */
void
bspTwl3029_Power_SwitchOff(void)
{
	BspTwl3029_ReturnCode ret_val;

	ret_val = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_PMC_MASTER, 
		BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_TEST_OFFSET,0x10, NULL);

	/*  allow I2C access to power bus be setting BWEN (bit 0 ) */

	ret_val = BspTwl3029_I2c_WriteSingle(1,0x23, 0x01, NULL);
	/*Power BUS : write via I2C reg PB_I2C_WORD_C (addr 0x35) , bits 0-7=0xff ( active) or 0xf8 (sleep) or 0xf0 (OFF )  */

	ret_val = BspTwl3029_I2c_WriteSingle(1,0x35, 0xF0, NULL);

	/*Power BUS : write via I2C reg PB_I2C_WORD_D (addr 0x36) , bits 0-7= 0x33   */

	ret_val = BspTwl3029_I2c_WriteSingle(1,0x36, 0x33, NULL);

 	/*  release I2C access to power bus be resetting BWEN (bit 0 ) */

	ret_val = BspTwl3029_I2c_WriteSingle(1,0x23, 0x00, NULL);

	/* I2C_I2C_MasterPollingSendAbb(PMC_MASTER_P1_DEV, DEVOFF); */
	 ret_val = BspTwl3029_I2c_WriteSingle(1,0x30,0x01, NULL);

}



//this function is used to turn off the leakage current  in the USB bias cell.It should be called before entering the sleep mode



 BspTwl3029_ReturnCode bspTwl3029_Usb_biascell_off(BspTwl3029_I2C_CallbackPtr callbackptr)

{
      BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;


       BspI2c_TransactionRequest transactionarray[10];
       int cnt=0;
       BspI2c_TransactionRequest* transactionptr=transactionarray;

	BspTwl3029_I2C_RegisterInfo regInfo[10] ;
       BspTwl3029_I2C_RegisterInfo* regInfoPtr =regInfo;  
	BspTwl3029_I2C_RegData dummy;



//	S1
	 BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE0, 254,0xb6,  regInfoPtr++);
     cnt++;  
// S2
           //BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_PAGE2, 208, &dummy);

	   //dummy=dummy|0x80;	   
           
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE2, 208,0x80,  regInfoPtr++);
    cnt++;

// S3
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE2,206 ,0x00,  regInfoPtr++);	
    cnt++;
//S4
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE0, 254,0x00,  regInfoPtr++);
    cnt++;


regInfoPtr=regInfo;
if(callbackptr!=NULL)
{
  transactionptr=(BspI2c_TransactionRequest*)callbackptr->i2cTransArrayPtr;
  
}

retval=BspTwl3029_I2c_regInfoSend	(regInfoPtr, (Uint16)cnt,  callbackptr,  transactionptr);

return(retval);


}

//this should be called after exiting the sleep mode.





 BspTwl3029_ReturnCode bspTwl3029_Usb_biascell_on(BspTwl3029_I2C_CallbackPtr callbackptr)
{
 BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;

	  BspI2c_TransactionRequest transactionarray[10];
       int cnt=0;
       BspI2c_TransactionRequest* transactionptr=transactionarray;

	BspTwl3029_I2C_RegisterInfo regInfo[10] ;
       BspTwl3029_I2C_RegisterInfo* regInfoPtr =regInfo;  
	BspTwl3029_I2C_RegData dummy;

// S1


	 BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE0, 254,0xb6,  regInfoPtr++);
     cnt++;  


// S2
      BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE2,206 ,0x0F,  regInfoPtr++);	
    cnt++;

// S3 

          // BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_PAGE2, 209, &dummy);

	   //dummy=dummy|0x7F;	   
           
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE2, 209,0x80,  regInfoPtr++);
    cnt++;

// S4
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PAGE0, 254,0x00,  regInfoPtr++);
    cnt++;


regInfoPtr=regInfo;
if(callbackptr!=NULL)
{
  transactionptr=(BspI2c_TransactionRequest*)callbackptr->i2cTransArrayPtr;
  
}

retval=BspTwl3029_I2c_regInfoSend	(regInfoPtr, (Uint16)cnt,  callbackptr,  transactionptr);
 
 return(retval);

}   
