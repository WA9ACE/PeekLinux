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
#include "bspTwl3029_IntC.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aud_Map.h"
#include "bspTwl3029_Aux_Map_pg2x.h"
#include "bspTwl3029_Aux_Llif_pg2x.h"
#include "sys_inth.h"



#define TRITON_SOURCE_ID 0

/*=============================================================================
 *   Global Data
 *============================================================================*/
/*=============================================================================
 * Description:
 *    Instance of an empty slot - suitable for copying.
 */
extern BspTwl3029_IntC_HandlerTableEntry bspTwl3029_IntC_emptySlot;
/*=============================================================================
 * Each entry in this table is a function pointer that corresponds to a bit in
 * the status register.  Bit zero's function pointer is in location zero.  Bit
 * one's function pointer is in location one, etc.
 */
extern BspTwl3029_IntC_HandlerTableEntry bspTwl3029_IntC_IntTable[];


/*=============================================================================
 *   Local Functions
 *============================================================================*/

/* variables used for reading INT2 registers */

static    BspTwl3029_I2C_RegData    status_L = 0;
static    BspTwl3029_I2C_RegData    status_H = 0;

/* variables used for reading USB registers */

BspTwl3029_I2C_RegData gUsbStatus = 0;



/* variables used for reading RTC  registers */

static  BspTwl3029_I2C_RegData rtcStatusData = 0;

/* for SW debouncing of hook and headset detect*/

#if (OP_L1_STANDALONE==0)
BspTwl3029_I2C_RegData Audintsts=0;
#endif

/* i2c transaction array rewerved  for intC interrupt routines only */

static BspI2c_TransactionRequest i2cIntTransArray[14];

void
bspTwl3029_IntC_dispatchInterrupt_2( BspI2c_TransactionId sourceId );
void
bspTwl3029_IntC_dispatchInterrupt_3( BspI2c_TransactionId sourceId );

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_dispatchInterrupt_1
 *
 * @discussion
 * Description
 *     Triton ISR. this function disables the IRQ and request the I2C to read
 *     Triton interrupt status register and also the Triton RTC and Triton
 *     USBOTG interrupt status registers.  Whatever event causing this interrupt
 *     is identified and handled in the I2C callback routine generated by the
 *     I2C transaction request done here.
 *
 *  @param  sourceId
 *              The interrupt source .
 *
 *  @return  none
 *
 */

void bspTwl3029_IntC_dispatchInterrupt_1()
{

    BspTwl3029_I2C_RegisterInfo i2cRegArray[5];
BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
BspTwl3029_I2C_Callback callback;
BspTwl3029_I2C_CallbackPtr callbackPtr = &callback;

    callbackPtr->callbackFunc = bspTwl3029_IntC_dispatchInterrupt_2;
    callbackPtr->callbackVal = (BspI2c_TransactionId)TRITON_SOURCE_ID;
    callbackPtr->i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)i2cIntTransArray;

    /* queue two I2C reads of  Triton int2 status registers */

    BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_INT2, BSP_TWL_3029_MAP_INT2_IT2STATUSP1H_OFFSET,
	    					  &status_H, i2cRegArrayPtr++);

    BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_INT2, BSP_TWL_3029_MAP_INT2_IT2STATUSP1L_OFFSET,
	                          &status_L, i2cRegArrayPtr++);

    /*RTC status registers are intentionally read twice as per the HW document */

    BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET,
	                          &rtcStatusData, i2cRegArrayPtr++);

    BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET,
	                          &rtcStatusData, i2cRegArrayPtr++);


    /* now request to I2C manager to read/write to Triton registers */
    i2cRegArrayPtr = i2cRegArray;

    /*Harcoded value 4 is intentional to reduce accesee time to registers*/
    BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,4,callbackPtr,
	    (BspI2c_TransactionRequest*)callbackPtr->i2cTransArrayPtr);

}

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_dispatchInterrupt_2
 *
 * @discussion
 * Description
 *     1st I2C callback resulting from I2C transaction reequest from main Triton
 *     ISR.  Whatever event that caused the Triton  interrupt
 *     is identified and handled in this routine. An I2C transation is
 *     requested in order to reset the status registers.
 *
 *  @param  sourceId
 *              The interrupt source .
 *
 *  @return  none
 *
 */

void bspTwl3029_IntC_dispatchInterrupt_2( BspI2c_TransactionId sourceId )
{
    BspTwl3029_I2C_RegisterInfo i2cRegArray[8];
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspTwl3029_I2C_Callback callback;
    BspTwl3029_I2C_CallbackPtr callbackPtr= &callback;
   Uint16 cnt=0;

   if ((status_H==0)&&(status_L==0))   {

      F_INTH_ENABLE_ONE_IT(C_INTH_ABB_IRQ_IT);
      return;
   }

    /* i2c callback fucntion is the 3rd part of the 3 part of the Triton interrupt handling sequence */

    callbackPtr->callbackFunc = bspTwl3029_IntC_dispatchInterrupt_3;
   callbackPtr->callbackVal = (BspI2c_TransactionId)TRITON_SOURCE_ID;
    callbackPtr->i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)i2cIntTransArray;


   if ((status_H&BSP_TWL3029_INTC_SOURCE_RTC_MASK)!=0)   {

   	  /*Mask the alarm bit*/
  	  if((rtcStatusData&BSP_TWL3029_RTC_ALARM_MASK)!=0x00)  {

         BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET,
  	                             rtcStatusData, i2cRegArrayPtr++);
         cnt++;
  	  }
	  else   {

		/*If it is not a alarm interrupt do not waste time in I2c write*/

	  	BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_RTC, BSP_TWL_3029_MAP_RTC_RTC_STATUS_REG_OFFSET,rtcStatusData);
	  	}

	 }

    if((status_H!=0)||(status_L!=0))	{

       /*-A Write access to one register clears both the registers Please take a look at the DRT
	*-OMAPS00102351. This condition checking is not needed, just to make things clear it is left, also the USB-
	*-headset plug interaction is going to be removed in the next Isample board,and the first check in this fuction
	*-can be removed
	*/

   BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_INT2, BSP_TWL_3029_MAP_INT2_IT2STATUSP1H_OFFSET,
	                          status_H, i2cRegArrayPtr++);
   cnt++;
    }




   if (((status_H&BSP_TWL3029_INTC_SOURCE_HS_MASK)!=0)||((status_H&BSP_TWL3029_INTC_SOURCE_HOOK_MASK)!=0))   {
#if (OP_L1_STANDALONE==0)
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_INT2, BSP_TWL_3029_MAP_AUDIO_POPTEST1_OFFSET,
	                            &Audintsts, i2cRegArrayPtr++);
      cnt++;
#endif


   }
   if (((status_H&BSP_TWL3029_INTC_SOURCE_USB_VBUS_MASK)!=0))   {
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PMC_MASTER, BSP_TWL_3029_MAP_PMC_MASTER_STS_HW_CONDITIONS_OFFSET,
	                            &gUsbStatus, i2cRegArrayPtr++);
      cnt++;

#if (OP_L1_STANDALONE==0)
#ifdef USB_HEADSET_HW_BUG
      if ((status_H&BSP_TWL3029_INTC_SOURCE_HS_MASK)==0)   {
		 BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_INT2, BSP_TWL_3029_MAP_AUDIO_POPTEST1_OFFSET,
			                       &Audintsts, i2cRegArrayPtr++);
	     cnt++;
      }
#endif
#endif
   }

   i2cRegArrayPtr = i2cRegArray;

   BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr, cnt, callbackPtr, (BspI2c_TransactionRequest*)callbackPtr->i2cTransArrayPtr );
    return;
}


/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_dispatchInterrupt_3
 *
 * @discussion
 * Description
 *     2nd I2C callback routine resulting from a Triton ISR. This callback is a
 *     called on completion of the I2C transaction reequest done during the
 *     bspTwl3029_IntC_dispatchInterrupt_2 routine for resetting the
 *     twl3029 interrrupt status registers
 *     This function reenables the main Triton interrupt
 *
 *  @param  sourceId
 *              The interrupt source .
 *
 *  @return  none
 *
 */

void bspTwl3029_IntC_dispatchInterrupt_3( BspI2c_TransactionId sourceId )
{
    Uint16 status = 0;
    BspTwl3029_IntC_SourceId   acessorySourceId;

  /* combine two 8 bit status registers  */

    status = ((Uint16)status_H << 8) | (Uint16)status_L;

    /* Loop through every bit and check if an interrupt is pending.  If it
     * is, then service it.
     */

    for (acessorySourceId = 0; acessorySourceId < 16; acessorySourceId++)   {
        if ((status & (1<< acessorySourceId)) != 0)   {
	       if (bspTwl3029_IntC_IntTable[ acessorySourceId ].handler != NULL )   {
                bspTwl3029_IntC_IntTable[ acessorySourceId ].handler( acessorySourceId );
            }
        }
    }



#if USB_HEADSET_HW_BUG

    /*This is a workaround for I-sample board.
     *when Headset is plugged in there was a dummy Usb interrupt.OMAPS00087657
     */

    if (((Uint8)(status>>8)&BSP_TWL3029_INTC_SOURCE_USB_VBUS_MASK)!=0)   {
	   if (bspTwl3029_IntC_IntTable[BSP_TWL3029_INTC_SOURCE_ID_AUD_HS].handler!= NULL)   {
          bspTwl3029_IntC_IntTable[BSP_TWL3029_INTC_SOURCE_ID_AUD_HS].handler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HS);
	   }
	}
#endif

    F_INTH_ENABLE_ONE_IT(C_INTH_ABB_IRQ_IT);




}





