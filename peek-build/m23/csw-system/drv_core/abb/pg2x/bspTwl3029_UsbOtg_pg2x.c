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
 *   includesTo remove the SW work-around for PG2.1
 */
 
//#include "main_system.h"
#include "types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Intc.h"
#include "bspI2c.h"
#include "bspTwl3029_UsbOtg.h"
#include "bspTwl3029_Power.h"

#include "bspTwl3029_Aux_Llif_pg2x.h"
#include "bspTwl3029_Aux_Map_pg2x.h"
#include "abb/bspTwl3029_Int_Llif.h"


/*******************************************************************************
* Defines and Macros
*/

/*=============================================================================
 * Description:
 */

#define BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT { NULL }

#define BSP_TWL3029_USB_OTG_REGISTER_VALUE_DEFAULT 0x00

#define BSP_TWL3029_USB_OTG_INT_TRIGGERED    1
#define BSP_TWL3029_USB_OTG_INT_ENABLED      BSP_TWL3029_INTC_INT_ENABLED
#define BSP_TWL3029_USB_OTG_INT_DISABLED     BSP_TWL3029_INTC_INT_DISABLED



typedef struct {
    BspTwl3029_UsbOtg_IntHandler handler;
} BspTwl3029_UsbOtgIntHandlerInfo;

BspTwl3029_UsbOtgIntHandlerInfo bspTwl3029_UsbOtgIntHandlerTable[] =
{
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT
};



typedef struct {
    BspTwl3029_UsbOtg_CrIntHandler handler;
} BspTwl3029_UsbOtgCrIntHandlerInfo;

BspTwl3029_UsbOtgCrIntHandlerInfo bspTwl3029_UsbOtgCrIntHandlerTable[] =
{
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT,
    BSP_TWL3029_INT_HANDLER_TABLE_EMPTY_SLOT
};


/*******************************************************************************
 * Local Data
 */ 
/*===========================================================================*/
/*!
 * @array bspTwl3029_UsbOtg_setRegArray
 *
 * @discussion
 * <b> Description </b><br>
 *    stores addresses for all SET registers, excluding interrupt related 
 *    SET registers
 *
 * @constant 
 *
 */
BspTwl3029_I2C_RegId  bspTwl3029_UsbOtg_setRegArray[] =
{   BSP_TWL3029_MAP_USB_CTRL_1_SET_OFFSET,
    BSP_TWL3029_MAP_USB_CTRL_2_SET_OFFSET,
    BSP_TWL3029_MAP_USB_CTRL_3_SET_OFFSET,
    BSP_TWL3029_MAP_USB_USB_POWER_SET_OFFSET,
    BSP_TWL3029_MAP_USB_CR_PSM_CTRL_SET_OFFSET,
    BSP_TWL3029_MAP_USB_CR_PSM_CMD_SET_OFFSET
} ;


/********************************************************************************
 * Local function declarations
 */ 
 
 /*********************************************************************************
 * Private (Local) Functions
 */
/*=============================================================================
 * Description:
 */

extern BspTwl3029_IntC_Handler     VBUS_Callback; 
extern BspTwl3029_I2C_RegData gUsbStatus;
/* We will Clear the Pull Up when Cable Detached */
static BspTwl3029_I2C_RegData      bspTwl3029_TritonPull;
static BspI2c_TransactionRequest   i2cIntTransArray[4];



static void
bspTwl3029_UsbOtg_pmVbusintHandler( BspTwl3029_IntC_SourceId sourceId )
{
    
    if(!(gUsbStatus& (1<<BSP_TWL3029_POWER_USB_OFFSET)))
    {

		(bspTwl3029_UsbOtgIntHandlerTable[BSP_TWL3029_USBOTG_INT2_VD_SESS_END].handler)(BSP_TWL3029_USBOTG_INT2_VD_SESS_END);		
    }
    else
    {

		(bspTwl3029_UsbOtgIntHandlerTable[BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD].handler)(BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD);
    }
	
    if(VBUS_Callback) {
    	(*VBUS_Callback)(sourceId); 
    }
}

 /*=============================================================================
 * Description:
 */

static void
bspTwl3029_UsbOtg_intHandler( BspTwl3029_IntC_SourceId sourceId )
{
    BspTwl3029_I2C_RegData     usbIntSourceId;
    BspTwl3029_I2C_RegData     usbIntLatch, usbIntLatch2, usbSrc1, usbSrc2 = 0;
    Uint16     status = 0;
    static Uint16 errorCount = 0;
return;
#if 0
    /* read shadow INT LATCH SET  registers  to find out source of USB interrupt*/
    /* notw: shodow reg forINT LATCH SET  registers has been unpdated by main trinton IntC ISR  */

    
    BspTwl3029_I2c_shadowRegRead ( BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_INT_LATCH_SET_OFFSET,
                                    &usbIntLatch);          
    
    
    BspTwl3029_I2c_shadowRegRead ( BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_INT_LATCH_2_SET_OFFSET,
                                    &usbIntLatch2);


    status = ((Uint16) usbIntLatch2 << 8 ) | (Uint16) usbIntLatch;
    
    /*
     * The the int latch  register isn't auto clearing, 
     * It is reset in main Triton IntC  ISR, not here 
     */
     if(gUsbStatus&0x8 == 0)
     {
//		(bspTwl3029_UsbOtgIntHandlerTable[BSP_TWL3029_USBOTG_INT_SESSION_START].handler)(BSP_TWL3029_USBOTG_INT_SESSION_START);
     }
   
    /* Loop through every bit and check if an interrupt is pending.  If it
     * is, then service it.
     */
    for (usbIntSourceId = 0; usbIntSourceId < BSP_TWL3029_USBOTG_INT_MAX; usbIntSourceId++)
    {
        if ((status & (1<< usbIntSourceId)) != 0)
        {
            /* invoke any registered interrupt handlers */
            if( bspTwl3029_UsbOtgIntHandlerTable[usbIntSourceId].handler != NULL )
            {
                (bspTwl3029_UsbOtgIntHandlerTable[usbIntSourceId].handler)(usbIntSourceId);
            }           
            
            else
            {
                /* ERROR: If these occur just count them instead of halting */
                //BSPUTIL_ASSERT( FALSE );
                /* wrap around on 255 */
                errorCount = ((errorCount +1) & 0xff);
            }
        }
    }
	
    return;
#endif
}

/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_crIntHandler
 *
 * Description:
 *    carkit interrrupt routine. This routine is called from   bspTwl3029_UsbOtg_intHandler
 *    if USBOtg interrupt was triggered by one of the carkit interrupt sources. The function 
 *    identifies the carkit interrupt source(s) then triggered the event and then runs the
 *    approupriate carkit interrupt handling routine.
 *
 * Inputs:         interrupt source id         
 *      
 *
 *  Returns:       none
 *  
 *  Notes:  
 */
static void
bspTwl3029_UsbOtg_crIntHandler( BspTwl3029_UsbOtg_InterruptId sourceId )

{
    //BspTwl3029_I2C_RegData     usbIntSourceId;
    BspTwl3029_I2C_RegData     usbIntLatch, usbIntLatch2 = 0;
    Uint16     status = 0;
   
    static Uint16 errorCount = 0;

     /* read shadow INT LATCH SET  registers  to find out source of USB interrupt*/
     /* notw: shodow reg forINT LATCH SET  registers has been unpdated by main trinton IntC ISR  */
    
    
    BspTwl3029_I2c_shadowRegRead ( BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_CR_INT_LATCH_1_SET_OFFSET,
                                    &usbIntLatch);          
    
    
    BspTwl3029_I2c_shadowRegRead ( BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_CR_INT_LATCH_2_SET_OFFSET,
                                    &usbIntLatch2);
    
    status = ((Uint16) usbIntLatch2 << 8 ) | (Uint16) usbIntLatch;
    
    /*
     * The the int latch  register isn't auto clearing, 
     * It is reset in main Triton IntC  ISR, not here 
     */
    
    /* Loop through every bit and check if an interrupt is pending.  If it
     * is, then service it.
     */

      
    /* Loop through every bit and check if an interrupt is pending.  If it
     * is, then service it.
     */
    for (sourceId = 0; sourceId < BSP_TWL3029_USBOTG_CR_INT_MAX; sourceId++)
    {
        if ((status & (1<< sourceId)) != 0)
    {
            /* invoke any registered interrupt handlers */
            if( bspTwl3029_UsbOtgCrIntHandlerTable[sourceId].handler != NULL )
            {
                (bspTwl3029_UsbOtgCrIntHandlerTable[sourceId].handler)(sourceId);
            }           
        
            else
            {
                /* ERROR: If these occur just count them instead of halting */
                //BSPUTIL_ASSERT( FALSE );
        /* wrap around on 255 */
        errorCount = ((errorCount +1) & 0xff);
            }
        }
    }
        
    return;
}



/*********************************************************************************
 * Public Functions
 */

/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_init
 *
 * Description:
 *    Main H/W initialization  
 *
 * Inputs:
 *  none
 *
 *  Returns:       BspTwl3029_ReturnCode
 *  
 *  Notes:  
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_init(void)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
        
     /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[23] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
   
    
    /*  i2c transction array */
    BspI2c_TransactionRequest i2cTransArray[32];  
    BspI2c_TransactionRequest* i2cTransArrayPtr = i2cTransArray;
      
    Uint16 count = 0;
    
    
    /* turn off carkit detection of USB precharge (reg: BCICTRL3 bit: PREUSBOFF )  */
    
    returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_BCI,
       BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,(1<<BSP_TWL3029_LLIF_BCI_BCICTL3_PREUSBOFF_OFFSET),regInfoPtr++);
       count++;
    
    /*set bits 2,3, and4 of reg CFG_PU_PD_LSB addr:25(0x19)  ( note reset val = 0x82) */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             0x19,0x9e,regInfoPtr++);
        count++;
    }
    
    /*set bit 5, address 0x68:  reg: AUX_REG_TOGGLE1, bit USBDS  */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,
             BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
             (1 <<BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_USBDS_OFFSET),regInfoPtr++);
        count++;
    }
 
    
    /*set bit 7, address 0x10, reg: USB_POWER_SET, bit VBUS_VRVUSB */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_USB_POWER_SET_OFFSET,
             (1 << BSP_TWL3029_LLIF_USB_USB_POWER_SET_VBUS_VRUSB_OFFSET), regInfoPtr++);                   
    count++;
    }
 
     /*Power BUS : set bit 0, address 0x35, reg: PB_CFG_I2C, bit BWEN */
     /* this allows I2C access to power bus */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
             (1 << BSP_TWL3029_LLIF_PMC_MASTER_PB_CFG_I2C_PB_I2C_BWEN_OFFSET),regInfoPtr++);
        count++;
    } 
    /*Power BUS : set bit 7 ( DEV_GRP to MODEM, address 0x93, bits 5-7= 0b100   */
        if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_SLAVE,
             BSP_TWL_3029_MAP_PMC_SLAVE_VRUSB_CFG_STS_OFFSET,
             0x30,regInfoPtr++);
        count++;
    } 
    
    /*Power BUS : write via I2C  address I2C word_C, bits 0-7= 0x80   */
        if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_I2C_WORD_C_OFFSET,
             0x20,regInfoPtr++);
        count++;
    } 
    
        /*Power BUS : write via I2C  address I2C word_D, bits 0-7= 0xaf   */
        if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_I2C_WORD_D_OFFSET,
             0xaf,regInfoPtr++);
        count++;
    } 

    /*  release I2C access to power bus be resetting BWEN (bit 0 ) */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_PMC_MASTER,
             BSP_TWL_3029_MAP_PMC_MASTER_PB_CFG_I2C_OFFSET,
             0x00,regInfoPtr++);
        count++;
    }  
    
    /*  set addr 0x69 bit 7 reg bit  . reg TOGGLE2 ,bit  USBAS*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,
             BSP_TWL3029_MAP_AUX_REG_TOGGLE2_OFFSET,
             (1<< BSP_TWL3029_LLIF_AUX_REG_TOGGLE2_USBAS_OFFSET),regInfoPtr++);
        count++;
    } 
    
    /*  USB CTRL1_CLR reg: clear SUSPEND (bit 1) and SE0 ( bit 2)R*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_CTRL_1_CLR_OFFSET,
             ((1<< BSP_TWL3029_LLIF_USB_CTRL_1_CLR_SUSPEND_OFFSET) |
             (1 << BSP_TWL3029_LLIF_USB_CTRL_1_CLR_DAT_SE0_OFFSET)),
             regInfoPtr++);
        count++;
    } 

    /*  USB CTRL2_SET: set DP_PULLUP (bit 0) */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_CTRL_2_CLR_OFFSET,
             (1<< BSP_TWL3029_LLIF_USB_CTRL_2_CLR_DP_PULLUP_OFFSET),regInfoPtr++);
        count++;
    }
#if 0
    /*  USB CTRL2_SET: clear  DP_PULLDOWN (bit 0) */   
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)    
    {        
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB, 
                BSP_TWL3029_MAP_USB_CTRL_2_CLR_OFFSET, 
                ((1<< BSP_TWL3029_LLIF_USB_CTRL_2_CLR_DP_PULLDOWN_OFFSET)), //|(1<<7)/*|(1<<5)*/),
                   regInfoPtr++); 
        count++;    
    }
	

    /*  USB CTRL2_SET: clear  DP_PULLUP (bit 0) */   
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)    
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB, 
                BSP_TWL3029_MAP_USB_CABLE_DEBOUNCE_OFFSET, 
                0xA0,
                regInfoPtr++); 
        count++;    
    }
#endif
     /* mask all interrupts  */
    
    /*  USB interrupt  1*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
        BSP_TWL3029_MAP_USB_INT_EN_LO_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    /*  USB interrupt */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_INT_EN_HI_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    /* now clear any pending interrupts */ 
    /*  USB interrupt */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_INT_LATCH_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    }  
  
    /* reset reg pointer */
    regInfoPtr = regInfo;
     
    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend(regInfoPtr,count,NULL,i2cTransArrayPtr);
    

    /* register the CARKIT interrupt handler before seting and enabling usb interrupt  */
//    bspTwl3029_UsbOtg_setHandler( BSP_TWL3029_USBOTG_INT_CR_INT,  bspTwl3029_UsbOtg_crIntHandler);

//////
    /* reset reg pointer */
    regInfoPtr = regInfo;
    i2cTransArrayPtr = i2cTransArray;     
    count = 0;
    
    /* mask all interrupts  USB int2*/
    
    /*  USB interrupt */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
        BSP_TWL3029_MAP_USB_INT_EN_LO_2_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    /*  USB interrupt 2*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_INT_EN_HI_2_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    /* now clear any pending interrupts */ 

    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_INT_LATCH_2_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    }  
    
    /* mask all interrupts  Carkit 1*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
        BSP_TWL3029_MAP_USB_CR_INT_EN_1_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    
    /* now clear any pending interrupts */ 
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_CR_INT_LATCH_1_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    }  
    
    /* mask all interrupts  Carkit 2*/
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
        BSP_TWL3029_MAP_USB_CR_INT_EN_2_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    } 
    
    /* now clear any pending interrupts */ 
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB,
             BSP_TWL3029_MAP_USB_CR_INT_LATCH_2_CLR_OFFSET, 0xff,regInfoPtr++);
        count++;
    }  
   
    /* reset reg pointer */
    regInfoPtr = regInfo;
     
    /* now request to I2C manager to write to Triton registers */
    returnCode = BspTwl3029_I2c_regInfoSend(regInfoPtr,count,NULL,i2cTransArrayPtr);

//////                             
    /* register handler for handling OTG trnasceiver interrupt */
    if (returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {

       returnCode = bspTwl3029_IntC_setHandler( (BspTwl3029_IntC_SourceId) BSP_TWL3029_INTC_SOURCE_ID_USB_D,
                                bspTwl3029_UsbOtg_intHandler );

       returnCode = bspTwl3029_IntC_setHandler( (BspTwl3029_IntC_SourceId) BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS,
                                bspTwl3029_UsbOtg_pmVbusintHandler);
    }

    /* register the CARKIT interrupt handler before seting and enabling usb interrupt  */
    bspTwl3029_UsbOtg_setHandler( BSP_TWL3029_USBOTG_INT_CR_INT,  bspTwl3029_UsbOtg_crIntHandler);

   return returnCode;

}    


/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_configTransceiverBit
 *
 * Description:
 *    update a bit in one of the USB registers 
 *
 * Inputs:
 *  callbackInfoPtr callbackInfoPtr       pointer to sruct containig callback info
 *  reg             BspTwl3029_I2C_RegId  register address
 *  bitOffset       Uint8                 register bit
 *  val             BspTwl3029_I2C_RegData    value ( 0 or 1)
 *
 *  Returns:       BspTwl3029_ReturnCode
 *  
 *  Notes:  
 */


BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_configTransceiverBit(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                    BspTwl3029_I2C_RegId reg, 
                                    Uint8 bitOffset, 
                                    BspTwl3029_I2C_RegData val )
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData  tmpCtrl1RegData = 0;
   
   /* check validity of args */
   if ( (reg < BSP_TWL3029_MAP_USB_CTRL_1_SET_OFFSET) || 
      (( reg > BSP_TWL3029_MAP_USB_CTRL_3_CLR_OFFSET) && ( reg <  BSP_TWL3029_MAP_USB_INT_LATCH_2_SET_OFFSET ))
      || (reg > BSP_TWL3029_MAP_USB_CR_PSM_CMD_CLR_OFFSET)        
      || (bitOffset >=8)
      || (val >1) )
   {
      return  BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   
   
   /* these are all SET/CLR registers: so reading of the shadows is not required */
   tmpCtrl1RegData = 0; 
   
   /*  set bit */
   /* update the corresponding field */
        BSPUTIL_BITUTIL_BIT_FIELD_SET8( &tmpCtrl1RegData,
                                       val,
                                       bitOffset,
                                       (Uint8)1 );
                                       
 
 
  {
     /* update Triton USB and triton USB shadow  registers */
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,  reg, tmpCtrl1RegData, 
                                        callbackInfoPtr);

  }
   return returnCode;

}
 
/*=============================================================================
 * Function:  bspTwl3029_UsbOtg_configTransceiverField
 *
 * Description:
 *    update a field in one of the USB registers 
 *
 * Inputs:
 *  callbackInfoPtr   pointer to struct containig callback info
 *  arrayPtr          pointer to struct containing reg address, bit field and offset
 *
 *  val               value  which field is to be updated to
 *
 *  Returns:          BspTwl3029_ReturnCode
 *  
 *  Notes:  
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_configTransceiverField(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,  
                                        BspTwl3029_UsbOtgRegUpdateInfo* arrayPtr,
                                        BspTwl3029_I2C_RegData val)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData tmpCtrl1RegRead = 0;
   BspTwl3029_I2C_RegData  tmpCtrl1RegData = 0;
   BspTwl3029_I2C_RegId reg = arrayPtr->registerId;
   Uint8 setClrFlag = 0;
    
   /* check validity of args */

   if ((reg < BSP_TWL3029_MAP_USB_CTRL_1_SET_OFFSET) || 
       ( ( reg > BSP_TWL3029_MAP_USB_RX_RECOVERY_OFFSET) && ( reg <  BSP_TWL3029_MAP_USB_INT_LATCH_2_SET_OFFSET ))
       ||(reg > BSP_TWL3029_MAP_USB_TPH_DET_RST_OFFSET)           
        )
   
   {
      return  BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   
   if ( ((arrayPtr->registerId >= BSP_TWL3029_MAP_USB_CTRL_1_SET_OFFSET) &&
         (arrayPtr->registerId <= BSP_TWL3029_MAP_USB_CTRL_3_CLR_OFFSET)) ||
        ((arrayPtr->registerId >= BSP_TWL3029_MAP_USB_INT_LATCH_2_SET_OFFSET) &&
         (arrayPtr->registerId <= BSP_TWL3029_MAP_USB_CR_PSM_CMD_CLR_OFFSET)) )   
   {
      /* for any of the SET/CLR registers: reading of the shadows is not required */
      tmpCtrl1RegData = 0;
      tmpCtrl1RegRead = 0;
      setClrFlag = 1;
   }
   else
      /* maintain values of other bits in register */
   {
      /* read shadow registers  */
      BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB,  arrayPtr->registerId, &tmpCtrl1RegRead);
      /* temp storage of reg contents*/
      tmpCtrl1RegData = tmpCtrl1RegRead;
   }   
   /*  set bit */
   /* update the corresponding field */
        BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpCtrl1RegData,
                                       val,
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       arrayPtr->regBitOffset,
                                       arrayPtr->regBitWidth );
                                       
  /* now check if same as value originally read from shadow register ( except for SET/CLR regs) */
  if ((tmpCtrl1RegData == tmpCtrl1RegRead) && (setClrFlag == 0))
  
  {
     return returnCode;
  }
  else
  {
     /* update Triton USB and triton USB shadow  registers */
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,  arrayPtr->registerId, tmpCtrl1RegData, 
                                        callbackInfoPtr);

  }
   return returnCode;

}                                         
 


/*=============================================================================
 * Function:  bspTwl3029_UsbOtg_getTransceiverStatusField
 *
 * Description:
 *    reads teh status  of some status field in one of the USB registers 
 *
 * Inputs:
 *  callbackInfoPtr   pointer to struct containig callback info
 *  arrayPtr          pointer to struct containing reg address, bit field and offset
 *
 *  val               value  which field is to be updated to
 *
 *  Returns:          BspTwl3029_ReturnCode
 *  
 *  Notes:  
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_getTransceiverStatusReg(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,  
                                        BspTwl3029_I2C_RegId reg,
                                        BspTwl3029_I2C_RegData* regDataPtr)
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
   
      /* read  register  */
   returnCode =  BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_USB,  reg, regDataPtr,callbackInfoPtr);
  
  
   return returnCode;

}                                         
 
/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_enableIntSource
 *
 * @discussion
 *    Enables the interrupt for the usb otg transceiver block.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 * @param triggerType
 *    Defines the interrupt signal transition to trigger an interrupt.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_enableIntSource( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                   BspTwl3029_UsbOtg_InterruptId    intSrcId,
                                   BspTwl3029_UsbOtgIntTriggerType  triggerType )
{
   
    BspTwl3029_I2C_RegId        intRegId=0;
    BspTwl3029_I2C_RegData      intMaskBit, regData;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    if(intSrcId >= BSP_TWL3029_USBOTG_INT_MAX)
    {
       /* invalid int id */
       return (returnCode);
    }
    
    
    if (intSrcId < 8)
    {
       if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_TRUE_FALSE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_LO_SET_OFFSET;    
       }
       else if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_HI_SET_OFFSET;   
       }
       intMaskBit =  (1 <<intSrcId);
    }
    else
    {
       if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_TRUE_FALSE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_LO_2_SET_OFFSET;    
       }
       else if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_HI_2_SET_OFFSET;   
       }

       intMaskBit  =  1 <<  (intSrcId - 8 );
       
    }

   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_USB, intRegId, &regData);

    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,intRegId, regData | intMaskBit, callbackInfoPtr);
    
    
    return (returnCode);
}
        

/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_disableIntSource
 *
 * @discussion
 *    Disables the interrupt for the usb otg transceiver block.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 * @param triggerType
 *    Defines the interrupt signal transition to trigger an interrupt.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_disableIntSource( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                    BspTwl3029_UsbOtg_InterruptId    intSrcId,
                                    BspTwl3029_UsbOtgIntTriggerType  triggerType )
{
    BspTwl3029_I2C_RegId        intRegId=0;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData      intMaskBit;
    
    if(intSrcId >=BSP_TWL3029_USBOTG_INT_MAX)
    {
       /* invalid int id */
       return (returnCode);
    }
    

   if (intSrcId < 8)
    {
       if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_TRUE_FALSE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_LO_CLR_OFFSET;    
       }
       else if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_HI_CLR_OFFSET;   
       }
       intMaskBit =  (1 <<intSrcId);
    }
    else
    {
       if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_TRUE_FALSE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_LO_2_CLR_OFFSET;    
       }
       else if( triggerType == BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION )
       {
           intRegId = BSP_TWL3029_MAP_USB_INT_EN_HI_2_CLR_OFFSET;   
       }
      
       intMaskBit  =  1 <<  (intSrcId - 8 );

   }
    
    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,intRegId, intMaskBit,
                                            callbackInfoPtr);
    
    
    return (returnCode);
}                                           


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_setHandler
 *
 * @discussion
 *    Registers the client handler for an interrupt.
 *
 * @param intSrcId
 *    The interrupt for which the handler is to be registered.
 *
 * @param handler
 *    The handler to register.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_setHandler( BspTwl3029_UsbOtg_InterruptId intSrcId,
                              BspTwl3029_UsbOtg_IntHandler  handler )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;

    //BspIntC_LockState lockState = bspIntC_adjustLock( BSP_INTC_LOCKSTATE_LOCKED );

    bspTwl3029_UsbOtgIntHandlerTable[intSrcId].handler = handler;

    //bspIntC_adjustLock( lockState );

    return (returnCode);
}
        
////////////////Car KIT STUFF ///////////////////////


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_enableCrIntSource
 *
 * @discussion
 *    Enables a carkit interrupt.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_enableCrIntSource( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                   BspTwl3029_UsbOtg_InterruptId    intSrcId)
{
   
    BspTwl3029_I2C_RegId        intRegId;
    BspTwl3029_I2C_RegData      intMaskBit;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    if(intSrcId >= BSP_TWL3029_USBOTG_CR_INT_MAX)
    {
       /* invalid int id */
       return (returnCode);
    }
    
    
  if (intSrcId < 8)
    {
        intRegId = BSP_TWL3029_MAP_USB_CR_INT_EN_1_SET_OFFSET;          
        intMaskBit =  (1 << intSrcId);
    }
    else
    {
       intRegId = BSP_TWL3029_MAP_USB_CR_INT_EN_2_SET_OFFSET;        
       intMaskBit  =  1 <<  (intSrcId - 8 );              
    }
  

    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,intRegId, intMaskBit, callbackInfoPtr);
    
    
    return (returnCode);
}
        

/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_disableCrIntSource
 *
 * @discussion
 *    Disables the interrupt for the usb carkit.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_disableCrIntSource( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                    BspTwl3029_UsbOtg_InterruptId    intSrcId)
{
    BspTwl3029_I2C_RegId        intRegId;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData      intMaskBit;
    
    
    if(intSrcId >= BSP_TWL3029_USBOTG_CR_INT_MAX)
    {
       /* invalid int id */
       return (returnCode);
    }
    
if (intSrcId < 8)
    {
        intRegId = BSP_TWL3029_MAP_USB_CR_INT_EN_1_CLR_OFFSET;          
        intMaskBit =  (1 << intSrcId);
    }
    else
    {
       intRegId = BSP_TWL3029_MAP_USB_CR_INT_EN_2_CLR_OFFSET;        
       intMaskBit  =  1 <<  (intSrcId - 8 );              
    }
   
    
    returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,intRegId, intMaskBit,
                                            callbackInfoPtr);
    
    
    return (returnCode);
}                                           


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_setCrHandler
 *
 * @discussion
 *    Registers the client handler for an interrupt.
 *
 * @param intSrcId
 *    The interrupt for which the handler is to be registered.
 *
 * @param handler
 *    The handler to register.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_setCrHandler( BspTwl3029_UsbOtg_InterruptId intSrcId,
                                BspTwl3029_UsbOtg_CrIntHandler  handler )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;

    //BspIntC_LockState lockState = bspIntC_adjustLock( BSP_INTC_LOCKSTATE_LOCKED );

    bspTwl3029_UsbOtgCrIntHandlerTable[intSrcId].handler = handler;
    
    //bspIntC_adjustLock( lockState );

    return (returnCode);
}
        
/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_set( BspTwl3029_UsbOtg_SetClrReg reg )
 *
 * @discussion
 *    sets one or more features for a given  internal register accesable
 *    via I2C by SEt/ CRL /STATUS type registers.
 *    Note: this function cannot be used for registers related to interrupt
 *    All of the interrrupt registers are accessed thru the twl3029UsbOtg 
 *    or twl3029_IntC APIs dedicated to interrupt handling.
 *
 * @param BspTwl3029_UsbOtg_SetClrReg
 *    neame of register 
 *
 * @param val
 *    bit map of features to be set.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_set( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                       BspTwl3029_UsbOtg_SetClrReg  reg,
               BspTwl3029_UsbOtg_SetClrVal val )
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegId        regId;
   
   /* check args */
   if (reg >( BSPTWL3029_SETCLRREG_MAX-1) )
   {
      return (  returnCode);
   }
   /* get the register's address */
   regId = bspTwl3029_UsbOtg_setRegArray[reg];
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,regId, val,
                                            callbackInfoPtr);
   
   return (  returnCode);
}

/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_clr( BspTwl3029_UsbOtg_SetClrReg reg )
 *
 * @discussion
 *    clrears one or more features for a given  internal register accesable
 *    via I2C by SEt/ CRL /STATUS type registers.
 *    Note: this function cannot be used for registers related to interrupt
 *    All of the interrrupt registers are accessed thru the twl3029UsbOtg 
 *    or twl3029_IntC APIs dedicated to interrupt handling.
 *
 * @param BspTwl3029_UsbOtg_SetClrReg
 *    neame of register 
 *
 * @param val
 *    bit map of features to be cleared.  
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_clr( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                       BspTwl3029_UsbOtg_SetClrReg  reg,
               BspTwl3029_UsbOtg_SetClrVal val )
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegId        regId;
   
   /* check args */
   if (reg > (BSPTWL3029_SETCLRREG_MAX-1) )
   {
      return (  returnCode);
   }
   /* get the register's SET address */
   regId = bspTwl3029_UsbOtg_setRegArray[reg];
   
   /* offset by 1 for CLR address*/
   regId++;
   returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_USB,regId, val,
                                            callbackInfoPtr);
   
   return (  returnCode);
}



/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_final_callback
 *
 * Description:
 *    Internal callback for the PULL up SET or PULL up CLEAR.
 *
 *  Inputs:        BspI2c_TransactionId id Indicates if the callback is for 
 *                 Set Pull or Clear Pull
 *      
 *
 *  Returns:       none
 *  
 *  Notes:  
 */
extern usb_discon_int();
static void bspTwl3029_UsbOtg_final_callback(BspI2c_TransactionId id)
{
    if(id==BSP_TWL3029_USB_CLEAR_PULL)
    {
		usb_discon_int();
	}
	/* Pull Up SET or Pull Up CLEAR Complete */
    return;
}

/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_callback
 *
 * Description:
 *    Internal callback for the PULL up SET or PULL up CLEAR.
 *
 *  Inputs:        BspI2c_TransactionId id Indicates if the callback is for 
 *                 Set Pull or Clear Pull
 *      
 *
 *  Returns:       none
 *  
 *  Notes:  
 */
static void bspTwl3029_UsbOtg_callback(BspI2c_TransactionId id)
{
    BspI2c_TransactionRequest 		i2cTransArray[4];
    BspTwl3029_I2C_RegisterInfo 	i2cRegArray[1];
    BspTwl3029_I2C_RegisterInfo* 	i2cRegArrayPtr = i2cRegArray;
        
    BspTwl3029_I2C_Callback 		callback;
    BspTwl3029_I2C_CallbackPtr 		callbackPtr= &callback; 
    
    callbackPtr->callbackFunc = 	bspTwl3029_UsbOtg_final_callback;       
    callbackPtr->callbackVal  = 	id;  
    callbackPtr->i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)i2cIntTransArray;



    if(id==BSP_TWL3029_USB_SET_PULL)
    {

        BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_USB_CTRL_2_SET_DP_PULLUP, &bspTwl3029_TritonPull, 1);
        BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_CTRL_2_SET_OFFSET,
        bspTwl3029_TritonPull,i2cRegArrayPtr++);

    }
    else
    {

        BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_USB_CTRL_2_CLR_DP_PULLUP, &bspTwl3029_TritonPull, 1);
        BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_USB, BSP_TWL3029_MAP_USB_CTRL_2_CLR_OFFSET,
        bspTwl3029_TritonPull,i2cRegArrayPtr++);

    }
        
    /* now request to I2C manager to read/write to Triton registers */           
    i2cRegArrayPtr = i2cRegArray;
          
    BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,1,callbackPtr,
                           (BspI2c_TransactionRequest*)callbackPtr->i2cTransArrayPtr);
    
    return;
}

/*=============================================================================
 * Function:   bspTwl3029_UsbOtg_PullControl
 *
 * Description:
 *    Controls the PULL of DP on USB Transiever.
 *
 *  Inputs:        BspTwl3029_UsbOtgPullControl id Indicates if it is for 
 *                 Set Pull or Clear Pull
 *      
 *
 *  Returns:       none
 *  
 *  Notes:  
 */
void bspTwl3029_UsbOtg_PullControl(BspTwl3029_UsbOtgPullControl id)
{   
    BspTwl3029_I2C_RegisterInfo i2cRegArray[1];
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
        
    BspTwl3029_I2C_Callback callback;
    BspTwl3029_I2C_CallbackPtr callbackPtr= &callback; 
    
    callbackPtr->callbackFunc = bspTwl3029_UsbOtg_callback;       
    callbackPtr->i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)i2cIntTransArray;
    callbackPtr->callbackVal  = id;  
	if(id){
        BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_USB,
                BSP_TWL3029_MAP_USB_CTRL_2_CLR_OFFSET,&bspTwl3029_TritonPull,i2cRegArrayPtr++);

	} else {
        BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_USB,
                BSP_TWL3029_MAP_USB_CTRL_2_SET_OFFSET,&bspTwl3029_TritonPull,i2cRegArrayPtr++);

    }


    /* now request to I2C manager to read/write to Triton registers */           
    i2cRegArrayPtr = i2cRegArray;
          
    BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,1,callbackPtr,
                           (BspI2c_TransactionRequest*)callbackPtr->i2cTransArrayPtr);

	return;
}

