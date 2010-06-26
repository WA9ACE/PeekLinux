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
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Pwr_Llif.h"
#include "bspTwl3029_Pwr_Map.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Aux_Map.h"
#include "sys_inth.h"

#define BSP_TWL3029_INTC_EMPTY_SLOT { NULL }

/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply disables
 *   interrupts.
 */
  /* RANGA - This has to be replaced by Locosto specific critical section */
#define BSP_TWL3029_INTC_CRITICAL_SECTION_ENTER() //{ BspIntC_LockState lockState = bspIntC_adjustLock( BSP_INTC_LOCKSTATE_LOCKED );

/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply restores
 *   interrupts to their previous state.
 */
 /* RANGA - This has to be replaced by Locosto specific critical section */
#define BSP_TWL3029_INTC_CRITICAL_SECTION_EXIT() //bspIntC_adjustLock( lockState ); }

/*=============================================================================
 *typedef TWL3027_Llif_Status
 *
 * Description: used for bits in STATUS registers
 */

#define BSP_TWL3029_LLIF_STATUS_ACTIVE    1
#define BSP_TWL3029_LLIF_STATUS_INACTIVE  0

typedef Uint16 TWL3029_Llif_Status;

/*=============================================================================
 *typedef TWL3029_Llif_MASK
 *
 * Description: used for bits in MASK registers
 */
#define BSP_TWL3029_LLIF_MASK_ENABLE    0
#define BSP_TWL3029_LLIF_MASK_DISABLE   1

typedef Uint16 TWL3029_Llif_Mask;


/*=============================================================================
 *   Global Data
 *============================================================================*/

/*=============================================================================
 * Description:
 *    Instance of an empty slot - suitable for copying.
 */
const BspTwl3029_IntC_HandlerTableEntry bspTwl3029_IntC_emptySlot = BSP_TWL3029_INTC_EMPTY_SLOT;

/*=============================================================================
 * Each entry in this table is a function pointer that corresponds to a bit in
 * the status register.  Bit zero's function pointer is in location zero.  Bit
 * one's function pointer is in location one, etc.
 */
BspTwl3029_IntC_HandlerTableEntry bspTwl3029_IntC_IntTable[] =
{
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT,
    BSP_TWL3029_INTC_EMPTY_SLOT, BSP_TWL3029_INTC_EMPTY_SLOT
};




/*=============================================================================
 *   Public Functions
 *============================================================================*/
void
bspTwl3029_IntC_dispatchInterrupt_1( /*BspIntC_SourceId sourceId*/ );
/*=============================================================================
 * Description:
 *   Initialization code for the TWL3029 interrupts.
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_init( void )
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    Uint32                             index;
    volatile BspTwl3029_I2C_RegData  status;
   
    
     BspTwl3029_I2C_RegisterInfo i2cRegArray[4];
    BspI2c_TransactionRequest I2cTransArray[8];    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = I2cTransArray;
        
    for( index = 0;
         index < BSP_TWL3029_INTC_SOURCE_ID_NUM_SOURCES;
         index++ )
    {
        bspTwl3029_IntC_IntTable[ index ] = bspTwl3029_IntC_emptySlot;
    }

    
    /* mask all Triton interrupts */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_INT2,
       BSP_TWL_3029_MAP_INT2_IT2MASKP1L_OFFSET,0xff,i2cRegArrayPtr++);
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_INT2,
       BSP_TWL_3029_MAP_INT2_IT2MASKP1H_OFFSET,0xff,i2cRegArrayPtr++);   
    
    /* clear any pending interrupts that may be present at startup */
   
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_INT2,
       BSP_TWL_3029_MAP_INT2_IT2STATUSP1H_OFFSET,0xff,i2cRegArrayPtr++);
       
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_INT2,
       BSP_TWL_3029_MAP_INT2_IT2STATUSP1L_OFFSET,0xff,i2cRegArrayPtr++);
    
    i2cRegArrayPtr = i2cRegArray;
    
    /* now request to I2C manager to read/write to Triton registers  */
    returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,4,NULL, i2cTransArrayPtr);
    
    /* register Triton ISR with bspIntC and enable */
#if 0
    bspIntC_setHandler( BSP_INTC_SOURCE_ID_EXT_IRQ,
                        bspTwl3029_IntC_dispatchInterrupt_1 );
    bspIntC_enableSource( BSP_INTC_SOURCE_ID_EXT_IRQ );
#endif

    F_INTH_ENABLE_ONE_IT(C_INTH_ABB_IRQ_IT);

    return( returnCode );
}


/*=============================================================================
 * Description:
 *   Registers an TWL3029 ISR.
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_setHandler( BspTwl3029_IntC_SourceId    sourceId,
                         BspTwl3029_IntC_Handler     handler )
{
     BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    
    
    if  ( sourceId < BSP_TWL3029_INTC_SOURCE_ID_NUM_SOURCES ) 
        
    {
        /*
         * Protect access to the global table
         */
        BSP_TWL3029_INTC_CRITICAL_SECTION_ENTER();

        bspTwl3029_IntC_IntTable[ sourceId ].handler = handler;

        BSP_TWL3029_INTC_CRITICAL_SECTION_EXIT();

        returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
    }

    return( returnCode );
}


/*====================================================================================
 * Description:
 *   This function disables an interrupt.
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_disableSource(  BspTwl3029_IntC_SourceId    sourceId )
{
    BspTwl3029_I2C_RegData   oldMask;
    BspTwl3029_I2C_RegData   newMask;
    
    BspTwl3029_I2C_RegId  maskReg;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
     Uint8 intSourceId = (Uint8)sourceId;
    Uint8 maskBit =0;

    
    if( intSourceId >= BSP_TWL3029_INTC_SOURCE_ID_NUM_SOURCES )
    {    
        /* ERROR */
        return  (returnCode);
       
    }
    
    /* check if interrupt mask bit is in LSB or MSB mask reg */  
    if (intSourceId >= 8 ) 
    {
       maskBit =  intSourceId - 8;
       maskReg = BSP_TWL_3029_MAP_INT2_IT2MASKP1H_OFFSET;
    }  
    else
    {
       maskBit = intSourceId;
       maskReg = BSP_TWL_3029_MAP_INT2_IT2MASKP1L_OFFSET;
    }
    
    /*
     * See if it's already disabled
     */
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_INT2,maskReg, &oldMask);
    
    
    if( (oldMask & (BSP_TWL3029_INTC_SOURCE_ID_TO_MASK( maskBit )) ) ==
        BSP_TWL3029_LLIF_MASK_DISABLE )
    {
       /* it is already disabled   */
       returnCode =  BSP_TWL3029_RETURN_CODE_SUCCESS;
    }
            
    else     
    {
        newMask = oldMask;
        newMask |= (1<< maskBit);
                                                      
        /* I2C write for  Triton mask reg */ 
        
       returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_INT2,
                             maskReg, newMask, NULL);
 
    }
    
    return( returnCode );
}



/*====================================================================================
 * Description:
 *    This function enables a source Id on the precongfigured mask. To change the
 *    configured mask use the IntC_config interface.
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_enableSource( BspTwl3029_IntC_SourceId    sourceId )
{
    BspTwl3029_I2C_RegData   oldMask;
    BspTwl3029_I2C_RegData   newMask;
    
    BspTwl3029_I2C_RegId  maskReg;
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    
    Uint8 intSourceId = (Uint8)sourceId;
    Uint8 maskBit =0;


    if( intSourceId >= BSP_TWL3029_INTC_SOURCE_ID_NUM_SOURCES )
    {    
        /* ERROR */
        return (returnCode); 
       
    }
    
    /* check if interrupt mask bit is in LSB or MSB mask reg */ 
    if (intSourceId >= 8 ) 
    {
       maskBit =  intSourceId - 8;
       maskReg = BSP_TWL_3029_MAP_INT2_IT2MASKP1H_OFFSET;
    }  
    else
    {
       maskBit = intSourceId;
       maskReg = BSP_TWL_3029_MAP_INT2_IT2MASKP1L_OFFSET;
    }
    
    /*
     * See if it's already enabled
     */
    BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_INT2,maskReg, &oldMask);
    
    
    if( (oldMask & (BSP_TWL3029_INTC_SOURCE_ID_TO_MASK( maskBit )) ) ==
        BSP_TWL3029_LLIF_MASK_ENABLE )
    {
       /* it is already enabled   */
       returnCode =  BSP_TWL3029_RETURN_CODE_SUCCESS;
    }
    else
    {      
        newMask = oldMask;
                
        BSPUTIL_BITUTIL_BIT_FIELD_SET8( &newMask,
                                      BSP_TWL3029_LLIF_MASK_ENABLE,
                                      maskBit,
                                      1);
                
       /* I2C write for  Triton mask reg */ 
        
       returnCode = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_INT2,
                             maskReg, newMask, NULL);
                                      
    }
    
    return( returnCode );
}

BspTwl3029_IntC_Handler     VBUS_Callback=NULL;

BspTwl3029_ReturnCode
bspTwl3029_IntC_registerVBUSCallback( BspTwl3029_IntC_SourceId    sourceId,
                         BspTwl3029_IntC_Handler     handler )
{


BspTwl3029_ReturnCode	  returnCode =  BSP_TWL3029_RETURN_CODE_FAILURE;

if((sourceId == BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS) && (handler !=NULL) )
{
	VBUS_Callback = handler;

	returnCode =  BSP_TWL3029_RETURN_CODE_SUCCESS;
}
else
	returnCode =  BSP_TWL3029_RETURN_CODE_FAILURE;
return returnCode;
}
