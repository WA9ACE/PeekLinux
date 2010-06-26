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
 *  FILE NAME: bspTwl3029_Madc.c
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

#include "sys_types.h"
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



#include "bspUtil_Assert.h"
#include "bspUtil_BitUtil.h"
/////////



#define BSP_TWL3029_MADC_REG_CHANNEL_SELECT_FIELD_WIDTH 1
#define BSP_TWL3029_MADC_REG_CHANNEL_CONVERSION_ENABLE 1

/*=============================================================================
 *  Defines and Macros
 */
			               				   
/*=============================================================================
 * Description:
 */
/*===========================================================================
 *   static Data:
 */
static volatile Uint8 readRtRegs, readAsyncRegs;
/*===========================================================================
 *   global Data:
 */
extern bspTwl3029_Reg shadowRegs;
extern BspTwl3029_IntCInfo bspTwl3029_IntCInfoArray[16];
/*===========================================================================
 *  Description:
 *    This is the info for  rt conversion complete event processing.
 */
BspTwl3029_MadcChannelInfo  bspTwl3029_Madc_rtInfo ;
BspTwl3029_MadcChannelInfo* bspTwl3029_Madc_rtInfoPtr = &bspTwl3029_Madc_rtInfo;
/*===========================================================================
 *  Description:
 *    This is the info for  async conversion complete event processing.
 */
BspTwl3029_MadcChannelInfo  bspTwl3029_Madc_asyncInfo ;
BspTwl3029_MadcChannelInfo *bspTwl3029_Madc_asyncInfoPtr  = NULL;
/*===========================================================================
 *  Description:
 *    This is the global storage for the rt conversion complete callback function.
 */

/*===========================================================================
 *  Description:
 *    This is the global storage for the async conversion complete  callback function.
 */
LOCAL BspTwl3029_I2C_RegisterInfo i2cRegArray[46];
LOCAL BspI2c_TransactionRequest i2cTransArray[48];    

/*=============================================================================
 *   Private Function declarations 
 *============================================================================*/
static void
bspTwl3029_Madc_interruptHandlerCallback_1( BspI2c_TransactionId sourceId );
static void
bspTwl3029_Madc_interruptHandlerCallback_2( BspI2c_TransactionId sourceId );

/*=============================================================================
 *   Private Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 */
/*=============================================================================
 * Description:
 *   This is the BSP_Madc interrupt handler. This function requests the I2c to 
 *   read the conversion results 
 *   on every interrrupt read the RT registers. Read also the Async if activated.
 */
static void
bspTwl3029_Madc_interruptHandler( BspTwl3029_IntC_SourceId sourceId )
{
   //BspTwl3029_ReturnCode errorCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
   BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
   Uint8 i, regAdrOffset;
   Uint16 count = 0; 
   Uint8 use_contiguous=0;
   BspTwl3029_MadcChannelId chan2Read = 0;
   //Uint8 readRtRegs, readAsyncRegs = 0; 
   
   BspTwl3029_I2C_Callback  callback;
   /* set up callback stuct */
   callback.callbackFunc = bspTwl3029_Madc_interruptHandlerCallback_1;
   callback.callbackVal = (BspI2c_TransactionId) sourceId;
   callback.i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)&i2cTransArray;
   
     
   if ( bspTwl3029_Madc_asyncInfoPtr != NULL)
   {
      if ( readAsyncRegs == 1 )
      {
         /* reads ( rt and async)  aready queued, so no need for further read request */
	 return;
      }
     else
      {
      /*  most probably an async event ( but could also be RT event ??, so read rt regs also ) */
	   if (bspTwl3029_Madc_rtInfoPtr != NULL )
	   {
	      readRtRegs = 1;
	   }   
	   readAsyncRegs = 1;
      } 
   }    
   else if (bspTwl3029_Madc_rtInfoPtr != NULL )
   {
     
     readRtRegs = 1;
   } 
    
    
    if (( readRtRegs == 1) && (bspTwl3029_Madc_rtInfoPtr != NULL ))
    {
        BspTwl3029_I2C_RegId regAdr = BSP_TWL3029_MAP_MADC_RTCH1_LSB_OFFSET;
       chan2Read = bspTwl3029_Madc_rtInfoPtr->chan2Read;
       
       for ( i = 0; i < 11; i++)
       {
	  chan2Read = bspTwl3029_Madc_rtInfoPtr->chan2Read >> i;
	  if (chan2Read == 0)
	  {
	     /* terminate loop - all required channel readings have alreay been queued*/
	     i = 11;
	  } 
	  /* test lsb */
	  else if ( chan2Read &  1 )
	  // if ( bspTwl3029_Madc_rtInfoPtr->chan2Read & ( 1<< i) )
          {
	     regAdrOffset = i << 1; 
	     BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset), 
	                               &shadowRegs.page0[regAdr + regAdrOffset],i2cRegArrayPtr++ );
	     BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset + 1), 
	                               &shadowRegs.page0[regAdr + regAdrOffset +1],i2cRegArrayPtr++ );
             count += 2;
          } 	        
       } 
    }  
     
    if (( readAsyncRegs == 1)&& (bspTwl3029_Madc_asyncInfoPtr != NULL ))
    {
       BspTwl3029_I2C_RegId regAdr = BSP_TWL3029_MAP_MADC_GPCH1_LSB_OFFSET;
       // /* debug */ BspTwl3029_I2C_RegId regAdr1 = 0;
       if(( readRtRegs == 1) && (bspTwl3029_Madc_rtInfoPtr != NULL ))
      	{
		use_contiguous = 0;    //Need to read RT registers also, all the registers are not contiguous, so proceed in normal way
       	for ( i = 0; i < 11; i++)
       	{
          		regAdrOffset = i << 1; 
	   BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset), 
	                               &shadowRegs.page0[regAdr + regAdrOffset] ,i2cRegArrayPtr++ );
	     
	   BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset + 1), 
	                               &shadowRegs.page0[regAdr + regAdrOffset +1],i2cRegArrayPtr++ );            
	   count += 2;       
       } 
    
       }
	else    
	{
	   use_contiguous = 1;	//Need to read only async. MADC registers, all registers are contiguous
	   
	   BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset), 
	                               &shadowRegs.page0[regAdr + regAdrOffset] ,i2cRegArrayPtr++ );
	   
	   count += 22; 		//read the 22 contiguous registers 
        }
    } 
   /* Now apply/commit the above changes made to the MADC registers.
   */
   i2cRegArrayPtr = i2cRegArray;
   if (use_contiguous == 1)
   	BspTwl3029_I2c_regInfoSendContiguous(i2cRegArrayPtr,count,&callback, i2cTransArrayPtr);
   else
   BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,&callback, i2cTransArrayPtr);
}
   


/*=============================================================================
 * Description:
 *   This is the I2C callback function generated by I2C read request in the 
 *   BSP_Madc interrupt handler. 
 *   This function requests the I2c to 
 *   read the conversion results 
 *   on every interrrupt read the RT registers. Read also the Async if activated.
 */
 
static void
bspTwl3029_Madc_interruptHandlerCallback_1( BspI2c_TransactionId sourceId )
{    
    //BspTwl3029_ReturnCode errorCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
   BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;   
   BspTwl3029_I2C_Callback  callback;

////REMOVE this def. for the blocking call !!

   BspTwl3029_I2C_RegData regData1,regData2;

   Uint16 count = 0;
   
   Uint8 i, regAdrOffset;
   BspTwl3029_MadcRegType regType;
   
   /* set up callback stuct */
   callback.callbackFunc = bspTwl3029_Madc_interruptHandlerCallback_2;
   callback.callbackVal = (BspI2c_TransactionId) sourceId;
   callback.i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)&i2cTransArray;
   
   if ( readRtRegs == 1)
   {
      
      /* put results in array and call the callback routine ( if specified ) */
      /* now call external real time callback routine ( if one defined ) */
      if ((bspTwl3029_Madc_rtInfoPtr) && (bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr != NULL))
      {         
         regType = BSPTWL3029_MADC_REGTYPE_RT;	 

/* REMOVE This blocking Call !!

   BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PAGE0,
                           BSP_TWL3029_MAP_MADC_RTCH10_MSB_OFFSET,
                           &regData1, 
			  NULL);

   BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_PAGE0,
                           BSP_TWL3029_MAP_MADC_RTCH10_LSB_OFFSET,
                           &regData2, 
			  NULL);    

*/ 

	 bspTwl3029_Madc_readShadowResult (bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr,
	                                   bspTwl3029_Madc_rtInfoPtr->chan2Read,
					   regType); 
      }  
      
      
      if((bspTwl3029_Madc_rtInfoPtr) && (bspTwl3029_Madc_rtInfoPtr->clbk != NULL ))
      {	 
	 bspTwl3029_Madc_rtInfoPtr->clbk();
      }
      readRtRegs = 0; 	      
   }
   
   
   if (readAsyncRegs == 1)
   {
      if ( bspTwl3029_Madc_asyncInfoPtr == NULL)
      {    
	 readAsyncRegs = 0;
	 return;
      }
      regType = BSPTWL3029_MADC_REGTYPE_ASYNC;	 
      bspTwl3029_Madc_readShadowResult (bspTwl3029_Madc_asyncInfoPtr->chanConversionResultsPtr,
	                                bspTwl3029_Madc_asyncInfoPtr->chan2Read,
					regType);
      /* check for collision  */
      /* If collisions  read GP registers again */
      if (bspTwl3029_Madc_asyncInfoPtr->chanConversionResultsPtr->chanCollisionFlags != 0 )
      {
         BspTwl3029_I2C_RegId regAdr = BSP_TWL3029_MAP_MADC_GPCH1_LSB_OFFSET;
    
        for ( i = 0; i < 11; i++)
        {
          
	     regAdrOffset = i << 1; 
	     BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset), 
	                               &shadowRegs.page0[regAdr + regAdrOffset] ,i2cRegArrayPtr++ );
	     
	     BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset + 1), 
	                               &shadowRegs.page0[regAdr + regAdrOffset +1],i2cRegArrayPtr++ );
             count += 2;       
        }   
      }
       
       /* If no collisions put results in array and call the callback routine ( if specified ) */
      else
      {
        /* now call external asyn callback routine ( if one defined ) */
         if((bspTwl3029_Madc_asyncInfoPtr)&& (bspTwl3029_Madc_asyncInfoPtr->clbk != NULL ))
         {
            bspTwl3029_Madc_asyncInfoPtr->clbk();
         }
         readAsyncRegs = 0;
         bspTwl3029_Madc_asyncInfoPtr = NULL;
         /* switch of MADC if RT acquisitions not enabled  */
	 if (readRtRegs != 0)
	 {
	  /* disable MADC ( if not disabled ) */
          BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCR_OFFSET ),  i2cRegArrayPtr++);   
	  count++;
	 }
     }	 
   }
   /* Now apply/commit the above changes made to the MADC registers.
   */
   i2cRegArrayPtr = i2cRegArray;
   BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,&callback, i2cTransArrayPtr);
}

/*=============================================================================
 * Description:
 *   This is the 2nd I2C callback function generated as a result of  a 
 *   trtion Madc interrupt . 
 *   This function is called if a read requested during the Triton ISR showed
 *   collision errors. this interrupt occurs on completion of a re read of the 
 *   registers. 
 *   This will not 
 */
static void
bspTwl3029_Madc_interruptHandlerCallback_2( BspI2c_TransactionId sourceId )
{       
   BspTwl3029_MadcRegType regType; 
   //BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
   //BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;   
   BspTwl3029_I2C_Callback  callback;
  
   
   /* set up callback stuct - use dummy callback funct defined in bspTwl3029_I2c */
   callback.callbackFunc = BspTwl3029_I2c_callbackFunction;
   callback.callbackVal = (BspI2c_TransactionId) sourceId;
   callback.i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)&i2cTransArray;
    
   if (readAsyncRegs == 1)
   {          
      /* don't check for collision this time */
      /* put results in array and call the callback routine ( if specified ) */
                  
       if ((bspTwl3029_Madc_asyncInfoPtr != NULL) &&(bspTwl3029_Madc_asyncInfoPtr->chanConversionResultsPtr != NULL))
      {
         
	 regType = BSPTWL3029_MADC_REGTYPE_ASYNC;	 
         bspTwl3029_Madc_readShadowResult (bspTwl3029_Madc_asyncInfoPtr->chanConversionResultsPtr,
	                                   bspTwl3029_Madc_asyncInfoPtr->chan2Read,
					   regType);
      }  
      
      /* now call external asyn callback routine ( if one defined ) */
      if(( bspTwl3029_Madc_asyncInfoPtr !=NULL) && ( bspTwl3029_Madc_asyncInfoPtr->clbk != NULL ))
      {
         bspTwl3029_Madc_asyncInfoPtr->clbk();
      }
      readAsyncRegs = 0;
      /* switch of MADC if RT acquisitions not enabled  */
      if ( (bspTwl3029_Madc_rtInfoPtr == NULL) ||  (bspTwl3029_Madc_rtInfoPtr->clbk == NULL ))
      {
	  /* disable MADC  */
          BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCR_OFFSET ),  
				   &callback);   
	
     }
      bspTwl3029_Madc_asyncInfoPtr = NULL;
   }
   
}

/*=============================================================================
 *   bspTwl3029_Madc_readConversionResult_RtCallback
 * Description:
 *   A Triton I2c callback function for  bspTwl3029_Madc_readConversionResult()
 *   this function processes the conversion register  data
 */
static void
bspTwl3029_Madc_readConversionResult_RtCallback (BspI2c_TransactionId transNum)
{
    BspTwl3029_MadcChannelInfo *conversionInfoPtr  = 
                               (BspTwl3029_MadcChannelInfo *) bspTwl3029_IntCInfoArray[transNum].twl2029callbackFuncArgs;
   
    bspTwl3029_Madc_readShadowResult(conversionInfoPtr->chanConversionResultsPtr ,
                                     conversionInfoPtr->chan2Read,
                                     BSPTWL3029_MADC_REGTYPE_RT);
}

/*=============================================================================
 *   bspTwl3029_Madc_readConversionResult_RtCallback
 * Description:
 *   A Triton I2c callback function for  bspTwl3029_Madc_readConversionResult()
 *   this function processes the conversion register  data
 */
static void
bspTwl3029_Madc_readConversionResult_AsyncCallback (BspI2c_TransactionId transNum)
{
    BspTwl3029_MadcChannelInfo *conversionInfoPtr  = 
                              ( BspTwl3029_MadcChannelInfo *) bspTwl3029_IntCInfoArray[transNum].twl2029callbackFuncArgs;
    
    bspTwl3029_Madc_readShadowResult(conversionInfoPtr->chanConversionResultsPtr ,
                                     conversionInfoPtr->chan2Read, 
				     BSPTWL3029_MADC_REGTYPE_ASYNC);
}  
/*=============================================================================
 *   Public Functions
 *============================================================================*/
/*=============================================================================
 * Fucntion bspTwl3029_Madc_init
 *   
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Madc_init(void)
{
    
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
    
    /* init structs for adc info */
    bspTwl3029_Madc_asyncInfoPtr = NULL;
    
    bspTwl3029_Madc_rtInfoPtr = &bspTwl3029_Madc_rtInfo;   
    bspTwl3029_Madc_rtInfoPtr->clbk =  NULL;
    bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr  =  NULL;

    
    /* disable all RT conversions */
    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC, BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
			   0,i2cRegArrayPtr++ );
     BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC, BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
			   0,i2cRegArrayPtr++ );
    /* Register the interrupt handlers */
    bspTwl3029_IntC_setHandler(  BSP_TWL3029_INTC_SOURCE_ID_MADC_P1,
                             bspTwl3029_Madc_interruptHandler );
    /* reset async pointer ( this pointer to NULL when no ASYNC conversion prrocessing is ongoing */
    bspTwl3029_Madc_asyncInfoPtr = NULL;
    
       
    /* now write the whole lot to triton i2c rtc registers */
    
    i2cRegArrayPtr = i2cRegArray;
    returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,2,NULL, i2cTransArrayPtr);
  
    /* enable MADC interrrupt */    
    bspTwl3029_IntC_enableSource((BspTwl3029_IntC_SourceId)BSP_TWL3029_INTC_SOURCE_ID_MADC_P1);
    return returnCode;
}    



/*==============================================================================*/
/*
 *  Triggers the ADC to start conversion on all the enabled channels
 *    for the specified interface.
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_triggerConversion_async( BspTwl3029_I2C_CallbackPtr          callbackInfoPtr,
                                  BspTwl3029_MadcConversionCompleteCallback  madcCallbackFunctionPtr,
				  BspTwl3029_MadcResults *                   resultsPtr)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = callbackInfoPtr;
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;

    Uint16 count = 0;      
    
    
    if ( bspTwl3029_Madc_asyncInfoPtr != NULL)
    {
         /* coversion or reading of conversion results already in process.*/
	 /* return  */
	 return returnCode; 
    }
    /* set pointer to point to dedicated info struct for async conversions
    .This will reset back to NULL when results of conversion has been read */  
    bspTwl3029_Madc_asyncInfoPtr = &bspTwl3029_Madc_asyncInfo;
    
    bspTwl3029_Madc_asyncInfoPtr->clbk =  madcCallbackFunctionPtr;
    bspTwl3029_Madc_asyncInfoPtr->chanConversionResultsPtr  =  resultsPtr;
    //bspTwl3029_Madc_asyncInfoPtr->chanCollisionFlagsPtr = channelCollisionFlagsPtr;
    bspTwl3029_Madc_asyncInfoPtr->chan2Read = 0x07ff;
    /* enable MADC ( if not disabled ) */
    returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCS_OFFSET ),  i2cRegArrayPtr++);     
    
    count++; 
    
    
    /* trigger the async conversion */
    returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_CTRL_P1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_MADC_CTRL_P1_SP1_OFFSET),  i2cRegArrayPtr++);     
    
    count++; 
    
    
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackInfoPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = (BspI2c_TransactionRequest*)callbackInfoPtr->i2cTransArrayPtr;
    } 
    /* Now apply/commit the above changes made to the MADC registers.
     */
     i2cRegArrayPtr = i2cRegArray;
     returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr); 

    return returnCode;
}                                



/*==============================================================================*/
/*   bspTwl3029_Madc_enableRt
 *  
 *    Enables one or more rt interrrupts.
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_enableRt( BspTwl3029_I2C_CallbackPtr                callbackInfoPtr,
                          BspTwl3029_MadcChannelId                  chan2Read,
		          BspTwl3029_MadcConversionCompleteCallback rtCallbackFunctionPtr,
		          BspTwl3029_MadcResults*                   resultsPtr)		       
{
    
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = callbackInfoPtr;
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;

    Uint16 count = 0;
          
    BspTwl3029_I2C_RegData tmpRtMsbRegData,tmpRtMsbRegRead;
    BspTwl3029_I2C_RegData tmpRtLsbRegData,tmpRtLsbRegRead;
    bspTwl3029_Madc_rtInfoPtr = &bspTwl3029_Madc_rtInfo;
        
    /* if no channels enabled then reset the struct */
    /* don't disable MADC here, disable after all channels select regs  are set to zero */
    if ( chan2Read == 0 )
    {
       bspTwl3029_Madc_rtInfoPtr->clbk =  NULL;
       bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr  =  NULL;
       bspTwl3029_Madc_rtInfoPtr->chan2Read = 0;
    
    }
    else 
    {
       bspTwl3029_Madc_rtInfoPtr->clbk =  rtCallbackFunctionPtr;
       bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr  =  resultsPtr;
       //bspTwl3029_Madc_rtInfoPtr->chanCollisionFlagsPtr = channelCollisionFlagsPtr; 
       bspTwl3029_Madc_rtInfoPtr->chan2Read = chan2Read; 
    }
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_MADC,   BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
                             &tmpRtLsbRegRead);
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_MADC,   BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
                             &tmpRtMsbRegRead);			     
    
    
    tmpRtLsbRegData = (BspTwl3029_I2C_RegData)(chan2Read & 0xff);
    tmpRtMsbRegData = (BspTwl3029_I2C_RegData)(chan2Read >> 8);
    
    
    /* enable Madc (if disabled ) before setting RT select regs */
    if (( tmpRtLsbRegRead == 0 ) && (tmpRtMsbRegRead == 0) && ( chan2Read != 0))
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCS_OFFSET ),  i2cRegArrayPtr++);     
    
       count++; 
    }
    
    if (tmpRtLsbRegData != tmpRtLsbRegRead)
    { 
       returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
                                   tmpRtLsbRegData,  i2cRegArrayPtr++);
       count++;
    } 
    
    if (tmpRtMsbRegData != tmpRtMsbRegRead)
    { 
       returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
                                   tmpRtMsbRegData,  i2cRegArrayPtr++);
       count++;
    } 
    
    /* if zero channels are enabled, turn off MADC here (AFTER resetting RT select regs). 
       Note check that no async conversion is on progress 
    */
    if (  (count ) && ( chan2Read == 0 ) && ( bspTwl3029_Madc_asyncInfoPtr == NULL ))
    {
       returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCR_OFFSET ),  i2cRegArrayPtr++);     
    
       count++; 
    }
    
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackInfoPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = (BspI2c_TransactionRequest*)callbackInfoPtr->i2cTransArrayPtr;
    } 
    /* Now apply/commit the above changes made to the MADC registers.
     */
     i2cRegArrayPtr = i2cRegArray;
     returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr); 
     return returnCode;
}		       

/*==============================================================================*/
/*   bspTwl3029_Madc_disableRt
 *  
 *    disables all Er channel conversions.
 *    Disables MADC interrrupt (if not already enabled)
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_disableRt( BspTwl3029_I2C_CallbackPtr          callbackInfoPtr)
{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   
    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = callbackInfoPtr;
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;
 
    Uint16 count = 0;    
  
    bspTwl3029_Madc_rtInfoPtr->clbk =  NULL;
    bspTwl3029_Madc_rtInfoPtr->chanConversionResultsPtr  =  NULL;
    //bspTwl3029_Madc_rtInfoPtr->chanCollisionFlagsPtr = NULL; 
    
    
    
    if (bspTwl3029_Madc_rtInfoPtr->chan2Read != 0 )
    {
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
                                   0,  i2cRegArrayPtr++);
        returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
                                   0,  i2cRegArrayPtr++);
        count+=2; 
        /* turn off MADC here (AFTER resetting RT select regs). 
        Note check that no async conversion is on progress 
       */
       if ( bspTwl3029_Madc_asyncInfoPtr == NULL)
       {
          returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUX,BSP_TWL3029_MAP_AUX_REG_TOGGLE1_OFFSET,
                                   (1 << BSP_TWL3029_LLIF_AUX_REG_TOGGLE1_MADCR_OFFSET ),  i2cRegArrayPtr++);     
    
          count++; 
       }
	
   
       /*
        * when no  triton callback function is defined,  I2C will function in poll mode
        */
       if (callbackInfoPtr != NULL)
       {
	   /* select the I2C struct, the pointer to this is in the callback struct */
	   i2cTransArrayPtr = (BspI2c_TransactionRequest*)callbackInfoPtr->i2cTransArrayPtr;
       } 
       /* Now apply/commit the above changes made to the MADC registers.
       */
       i2cRegArrayPtr = i2cRegArray;
       returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr); 
   }
   
   else 
   {
       /*  rt conversions are already disabled ! */
       returnCode =  BSP_TWL3029_RETURN_CODE_SUCCESS;
   }     
   return  returnCode;
}		       

/*==============================================================================*/
/* bspTwl3029_Madc_setRtChannels
* allows selective enabling/disabling of MADC channels for the RT conversion.
*
*/
BspTwl3029_ReturnCode
bspTwl3029_Madc_setRtChannels(BspTwl3029_I2C_CallbackPtr     callbackInfoPtr , BspTwl3029_MadcChannelId  chan2Read )
{
     BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;

    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = callbackInfoPtr;
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;

    Uint16 count = 0;
          
    BspTwl3029_I2C_RegData tmpRtMsbRegData,tmpRtMsbRegRead;
    BspTwl3029_I2C_RegData tmpRtLsbRegData,tmpRtLsbRegRead;
    
    bspTwl3029_Madc_rtInfoPtr->chan2Read = chan2Read; 
    
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_MADC,   BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
                             &tmpRtLsbRegRead);
    returnCode = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_MADC,   BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
                             &tmpRtMsbRegRead);			     
    
    
    tmpRtLsbRegData = (BspTwl3029_I2C_RegData)(chan2Read & 0xff);
    tmpRtMsbRegData = (BspTwl3029_I2C_RegData)(chan2Read >> 8);
    
    
    
    
    if (tmpRtLsbRegData != tmpRtLsbRegRead)
    { 
       returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_LSB_OFFSET,
                                   tmpRtLsbRegData,  i2cRegArrayPtr++);
       count++;
    } 
    
    if (tmpRtMsbRegData != tmpRtMsbRegRead)
    { 
       returnCode = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_MADC,BSP_TWL3029_MAP_MADC_RTSELECT_MSB_OFFSET,
                                   tmpRtMsbRegData,  i2cRegArrayPtr++);
       count++;
    } 
    
     /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */

    if (callbackInfoPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = (BspI2c_TransactionRequest*)callbackInfoPtr->i2cTransArrayPtr;
    } 

     /* Now apply/commit the above changes made to the MADC registers.
     */

     i2cRegArrayPtr = i2cRegArray;
     returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,count,callbackFuncPtr, i2cTransArrayPtr); 
     return returnCode;

}


/*==============================================================================*/
/*   bspTwl3029_Madc_readShadowResults
 *  
 *    reads results of last conversion from values stored in shadow memory
 */
 
BspTwl3029_ReturnCode
bspTwl3029_Madc_readShadowResult(  BspTwl3029_MadcResults*      madcResultsPtr,
				   BspTwl3029_MadcChannelId     chan2Read,                               
				   BspTwl3029_MadcRegType       regType)
  
{
   BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   Uint16 i = 0; 
   BspTwl3029_MadcChannelId   chanels2Read = chan2Read;
   
   BspTwl3029_MadcConversionResult*  resultsPtr = 
                          (BspTwl3029_MadcConversionResult*)&madcResultsPtr->adc1;
   
   BspTwl3029_I2C_RegId regId ;
   BspTwl3029_I2C_RegData readData1, readData2;

   /* clear  collisions flag */
   madcResultsPtr->chanCollisionFlags = 0;
   
   
   if ( regType == BSPTWL3029_MADC_REGTYPE_RT )
   {
      regId = BSP_TWL3029_MAP_MADC_RTCH1_LSB_OFFSET;
   }
   else if ( regType == BSPTWL3029_MADC_REGTYPE_ASYNC )
   {
      regId = BSP_TWL3029_MAP_MADC_GPCH1_LSB_OFFSET;
      /* read all the channels */
      chanels2Read = 0x07ff;
       /* debug   regId = 0;  */
   }
   else
   {
      return returnCode;
   }
   
   /* each  10 bit conversion result and the colliison error bit has been stored in two shadow registers */
   for (i = 0; i  < 11; i++)
   {
      if ( chanels2Read & ( 1<< i))
      {        
	 /*combine LSB and MSB  and mask out Collisin result ( bit 2 of MSB ) and all the reserved bit); */
         //  *resultsPtr++ = (( (Uint16)shadowRegs.page0[regId++] & (Uint16) (shadowRegs.page0[regId] << 8) ) & 0x03ff);	                   
	 BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_MADC, regId++, &readData1); 
         BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_MADC, regId++, &readData2); 
      
         *resultsPtr++ = ( ((Uint16)readData1 | (Uint16)(readData2 << 8) ) & 0x03ff);
	 	 
	 /* store collision result ( bit 2 of MSB in chanCollisionFlagsPtr bit map */
	 madcResultsPtr->chanCollisionFlags |=  (((readData2 & 4)>> 2) <<  i);
      }
      else 
      {
         regId += 2;
         resultsPtr++;
      }
   }
   
   return BSP_TWL3029_RETURN_CODE_SUCCESS;
}
 
 
 
 
/*==============================================================================*/
/*   bspTwl3029_Madc_readConversionResult
 *  
 *    reads results of last conversion from Triton register via I2C
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_readConversionResult( BspTwl3029_I2C_CallbackPtr   callbackInfoPtr,
                                      BspTwl3029_MadcResults*      madcResultsPtr,
				      BspTwl3029_MadcChannelId     chan2Read,
				      BspTwl3029_MadcRegType       type)
				      

{
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
   
    BspTwl3029_I2C_Callback callbackFuncInfo;
    BspTwl3029_I2C_CallbackPtr callbackFuncPtr = &callbackFuncInfo;
    BspTwl3029_I2C_CallbackPtr apiCallbackFuncPtr = callbackInfoPtr;
    
    BspTwl3029_I2C_RegisterInfo* i2cRegArrayPtr = i2cRegArray;
    BspI2c_TransactionRequest *i2cTransArrayPtr = i2cTransArray;

    BspTwl3029_I2C_RegId regAdr, regAdrOffset;
   
    Uint8 i = 0; 
    Uint16 count = 0;
    
    BspTwl3029_MadcChannelInfo conversionInfo;
   
   
   /* clear  collisions flag */
   madcResultsPtr->chanCollisionFlags = 0;
   
   if (type == BSPTWL3029_MADC_REGTYPE_RT)
   { 
      regAdr = BSP_TWL3029_MAP_MADC_RTCH1_LSB_OFFSET;
   } 
   else if (type == BSPTWL3029_MADC_REGTYPE_ASYNC)
   { 
      regAdr = BSP_TWL3029_MAP_MADC_GPCH1_LSB_OFFSET;
   } 
   else 
   {
      /* unrecognised type */
      return (returnCode);
   }     
   
   /* now  set up I2c reads from acquisition result registers. results will  be
      stored in shadow memory.
      On completion of the I2C reads the data stored in the shadow registers
      will be processed and the 10 bit results and error information  will be  
      then stored in the array pointed to by madcResultsPtr. -this will be done
      as part of the I2C callback routine resulting from the I2C transaction
      requested during this function.
   */
   for ( i = 0; i < 11; i++)
   {
      if ( chan2Read & ( 1<< i))
      {
         regAdrOffset = i << 1; 
         BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset), 
	                               &shadowRegs.page0[regAdr + regAdrOffset] ,i2cRegArrayPtr++ );
	     
         BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_MADC, (regAdr + regAdrOffset + 1), 
	                               &shadowRegs.page0[regAdr + regAdrOffset +1],i2cRegArrayPtr++ );
         count += 2;       
      }       
   }

//////////
   /* for I2C callback mode we need to call a Triton i2c ISR to process the results that  have just been read*/
   /* this is done by setting up I2C callback to use a default isr routine defined in twl3029 I2c driver.
      this in turn will call the API defined I2C ISr routine and also a Triton ISR routine specific
      to this function
    */  
    if (apiCallbackFuncPtr != NULL)
    {	
        /* prepare the  BspTwl3029_MadcConversionResult strut. This will be used
	in I2C callback routine */
	
	conversionInfo.chanConversionResultsPtr = madcResultsPtr; 
        conversionInfo.chan2Read = chan2Read;
	
	if (type == BSPTWL3029_MADC_REGTYPE_RT)
	{
	/* for interrupt callback we need to call a Triton callback function 
	   This processed the conversion values that have just been read
	 */  
	   if ( bspTwl3029_I2cCallbackSetup( callbackFuncPtr,apiCallbackFuncPtr,
	                                     bspTwl3029_Madc_readConversionResult_RtCallback, 
					   (BspTwl3029_I2C_RegData*)&conversionInfo ) == -1)
           {
	      /* error */
	      return (returnCode);	  
           }
	}    
	else 
	{
	   if ( bspTwl3029_I2cCallbackSetup( callbackFuncPtr,apiCallbackFuncPtr,
	                                   bspTwl3029_Madc_readConversionResult_AsyncCallback, 
					   (BspTwl3029_I2C_RegData*)&conversionInfo ) == -1)
           {
	      /* error */
	      return (returnCode);	  
           } 
	}   
        /* set pointer to point to correct transaction stuct - not the local one defined above */ 
	i2cTransArrayPtr = (BspI2c_TransactionRequest*) callbackFuncPtr->i2cTransArrayPtr;

    }
    else
    {
       callbackFuncPtr = NULL;
    }


    /* Now apply/commit the above changes made to the MADC registers.
    */
     i2cRegArrayPtr = i2cRegArray;
     returnCode = BspTwl3029_I2c_regInfoSend(i2cRegArrayPtr,2,callbackFuncPtr, i2cTransArrayPtr);   
        
	
    /* if i2C POLL mode, process the results from shadow memory bank now.*/
    if ( callbackFuncPtr == NULL )
    {
       returnCode = bspTwl3029_Madc_readShadowResult( madcResultsPtr,chan2Read, type );
    }
    return returnCode;
}





