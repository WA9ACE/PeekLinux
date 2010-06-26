/*==============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */

#include "types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_Int_Map.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"

// Added for compilation
#ifndef _WINDOWS
  #include "l1sw.cfg"
#endif

/*==============================================================================
 * File Contents:
 *   This module contains functions associated with Triton ABB. It is used 
 *   within the ABB driver only.
 *
 *   The functions and macros are used for preparing and sending data strutures
 *   for accessing Triton registers via the bsp I2C manager.
 */

/*===========================================================================
 * Defines and Macros
 */

/*===========================================================================
 * Local Data
 */
static const BspI2c_ConfigInfo bspTwl3029_i2cConfigInfo =
{

    BSP_I2C_DEVICE_BUS_SPEED_400K,
    BSP_I2C_ADDRESS_OCTET_LENGTH_1,
    BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED,
    BSP_I2C_DEVICE_ADDRESS_LENGTH_7BIT,
    BSP_I2C_DEVICE_ADDRESS_TWL3029,
    BSP_I2C_MULTI_BYTE_READ_SUPPORTED,
    BSP_I2C_ADDRESS_ORDER_LOW_TO_HIGH
};
//static BspTwl3029_I2c_CallbackStatus i2cCallbackStatus;
/*===========================================================================
 * Local function declarations
 */
static 
BspTwl3029_ReturnCode BspTwl3029_setI2cPage(BspTwl3029_I2C_TypeId page,
    BspI2c_TransactionRequest*  transaction);
    
BspTwl3029_ReturnCode
bspTest_i2cScenarioTest(Uint8 page,BspTwl3029_I2C_RegId reg, BspTwl3029_I2C_RegData val);

BspTwl3029_ReturnCode 
BspTwl3029_I2c_shadowRegInit(void);
/*===========================================================================
 * shadow registers
 */
 bspTwl3029_Reg shadowRegs;
/*===========================================================================
 * global Data
 */

/*===========================================================================
 *  variables used for the two Triton defined I2C callback ISRs
 */

 BspTwl3029_I2c_CallbackStatus twl3029_i2cCallbackIsrStatus;
    
extern BspTwl3029_IntCInfo bspTwl3029_IntCInfoArray[16];
static Uint8 idArrayReserveIndex = 0;
static Uint8 idArrayFreeIndex = 0;
/*===========================================================================
 * Public Functions
 */

/*===========================================================================
 * Function:    BspTwl3029_I2c_init
 *
 * Description: initialises a  I2C for Triton
 *
 * Inputs:      none 
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:       Use by other bspTwl3029 files.
 */
 
 
 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_init(void)
 {
    BspTwl3029_ReturnCode returnCode = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    if (bspI2c_config(BSP_I2C_DEVICE_ID_TWL3029,
                  &bspTwl3029_i2cConfigInfo)== BSP_I2C_RETURN_CODE_SUCCESS)
    {
       returnCode = BSP_TWL3029_RETURN_CODE_SUCCESS;
    }		  
    if ( returnCode == BSP_TWL3029_RETURN_CODE_SUCCESS)
    {
       /*  init  callback flag */
       twl3029_i2cCallbackIsrStatus = BSP_TWL3029_I2C_CALLBACK_NOTCOMPLETED;
       
       /* init shadow reg  */ 
       returnCode = BspTwl3029_I2c_shadowRegInit();
    }
    return returnCode;
 }
 
 /*===========================================================================
 * Function: 
 *
 * Description: requests I2C manager to read a single register
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This function is non blocking. 
 *             However it can be make blocking if callbackInfoPtr is defined
 *             and callbackInfoPtr->callbackFunc = NULL
 */
 BspTwl3029_ReturnCode
 BspTwl3029_I2c_ReadSingle(BspTwl3029_I2C_TypeId typeId,
                           BspTwl3029_I2C_RegId regAddress,
                           BspTwl3029_I2C_RegData *regData, 
			   BspTwl3029_I2C_CallbackPtr callbackInfoPtr)
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
      
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = &regInfo;
    
    /* callback function pointer */
    
    BspTwl3029_I2C_CallbackPtr callbackPtr= callbackInfoPtr;
    
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    if ( regData != NULL)
    {       
      /* configure regInfo struct */
      returnVal= BspTwl3029_I2c_regQueRead( typeId, regAddress,regData, regInfoPtr);
    }
    else
    {
        /* invalid pointer */
	return returnVal;
    }
    
    
    /* config callback function struct */
    /*
    * when no  triton callback function is defined,  I2C will function poll mode
    */    
    if (callbackPtr != NULL)
    {
        
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }   
       
    if  (returnVal !=  BSP_TWL3029_RETURN_CODE_FAILURE)
    {
        /* config  I2C struct and send to I2C manager */
        returnVal = BspTwl3029_I2c_regInfoSend(regInfoPtr,1, callbackPtr, (BspI2c_TransactionRequest *) i2cTransArrayPtr);
    }        
    return returnVal;
}
 
 /*===========================================================================
 * Function:    BspTwl3029_I2c_WriteSingle
 *
 * Description: requests I2C manageer to write a single register
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This function is blocking. It won't return until 
 *             I2C has finished its transaction. 
 */
BspTwl3029_ReturnCode 
BspTwl3029_I2c_WriteSingle(BspTwl3029_I2C_TypeId typeId,
                                                BspTwl3029_I2C_RegId regAddress,
                                                BspTwl3029_I2C_RegData regData, 
						BspTwl3029_I2C_CallbackPtr callbackInfoPtr)
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = &regInfo;
    
    /* callback function pointer */
    BspTwl3029_I2C_CallbackPtr callbackPtr= callbackInfoPtr;
    
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
     
     
              
    /* configure regInfo struct */
    returnVal = BspTwl3029_I2c_regQueWrite( typeId, regAddress,regData, regInfoPtr);

       
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
        callbackPtr = callbackInfoPtr;
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }   
    
    if (returnVal !=  BSP_TWL3029_RETURN_CODE_FAILURE)  
    {
       /* update shadow */
       returnVal = BspTwl3029_I2c_shadowRegWrite( typeId, regAddress, regData); 
    }
    
    if (returnVal !=  BSP_TWL3029_RETURN_CODE_FAILURE)  
    {
       /* config  I2C struct and send to I2C manager */           
      returnVal = BspTwl3029_I2c_regInfoSend(regInfoPtr,1, callbackPtr, (BspI2c_TransactionRequest *)i2cTransArrayPtr);      
    }
    return returnVal;
}
  

/*===========================================================================
 * Function:    BspTwl3029_I2c_regInfoSend
 *
 * Description: Formats  Triton reigster information into an I2C struct and 
 *              sends this to I2C manager which will read/write 
 *              from/to registers given in the I2C struct. Also sends pointer
 *              to Triton callback fucntion.
 *
 * Inputs:     pointer to struct containing Triton register/data pairs
 *             Uint16 count - number of registers to be accessed ( exluding
 *                          paging registers. Count starts from 1. If zero
 *                          count is sent. ffunction returns without doing
 *                          anything( with BSP_TWL3029_RETURN_CODE_SUCCESS return
 *                          code)
 *             pointer to struct containing Triton callback function info
 *             pointer to I2C struct. This function inits the strut before 
 *                          sending it to I2C manager
 *             
 * Returns:    BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             Will need to replace last parameter with pointer at a later
 *             release.
 */
 BspTwl3029_ReturnCode
 BspTwl3029_I2c_regInfoSend(BspTwl3029_I2C_RegisterInfo* infoPtr,
                            Uint16 count,
                            BspTwl3029_I2C_CallbackPtr callback, 
                            BspI2c_TransactionRequest* I2cTransArrayPtr)
 {
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    
    /* use local pointers */
    BspTwl3029_I2C_RegisterInfo* infoArrayPtr = infoPtr;
    BspI2c_TransactionRequest* transArrayPtr = I2cTransArrayPtr;    
    BspI2c_ScenarioDescriptor i2cScenario;
   
    
    Uint16 i = 0; 
    BspI2c_TransactionCount j= 0;  /* same type as BspI2c_ScenarioDescriptor.count */
    BspTwl3029_I2C_TypeId typeTemp;
     /* check that count is > 0 */
     if ( count == 0 )    {
         returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
	 return returnVal;
     }
    /* select new page */
    typeTemp = infoArrayPtr->typeId ;
    returnVal = BspTwl3029_setI2cPage(infoArrayPtr->typeId, transArrayPtr); 
    j++;
    transArrayPtr++;
    
    
      
    
    /* next the transactions */
    for ( i = 0; i < count; i++)
    {
        /* check if change of  page is required */
	if (typeTemp != infoArrayPtr->typeId)   {
	   typeTemp = infoArrayPtr->typeId ;
           /*  select new page  */
	   returnVal = BspTwl3029_setI2cPage(infoArrayPtr->typeId, transArrayPtr); 
	   j++;
	   transArrayPtr++;
	}
 		  
       transArrayPtr->transactionType =
       (BspI2cMaster_TransactionType)infoArrayPtr->regAccType; 
       transArrayPtr->dataStartAddress = (BspI2c_DataAddress)infoArrayPtr->regId;
       transArrayPtr->dataPtr = infoArrayPtr->regDataPtr;
       transArrayPtr->dataCount = 1;
	#ifdef BSP_I2C_DMA_Enable
       transArrayPtr->dmaRequestId = BSP_DMA_REQ_NONE; // BSP_DMA_REQ_NONE;
       #endif
       j++;
       transArrayPtr++;
       infoArrayPtr++;
  
    }
    /* now set up I2C scenario descriptor */
    i2cScenario.count = j;
    i2cScenario.deviceId = BSP_I2C_DEVICE_ID_TWL3029;
    if (callback != NULL)    {
        i2cScenario.transactionDoneCallback =  callback->callbackFunc;     	   
        i2cScenario.transactionId =
	(BspI2c_TransactionId)callback->callbackVal;
	i2cScenario.delayFunctionPtr =  NULL;
        i2cScenario.delayValue = 0;
	   
    }
   else   {
        
	
	i2cScenario.transactionDoneCallback =  NULL;     	   
        i2cScenario.transactionId = 0;
	i2cScenario.delayFunctionPtr =  NULL;
        i2cScenario.delayValue = 0;
	
	
   }
   /* now call I2c function.
   // note. It is assumed that *scenarioDesc goes out of scope on return from
   // the I2C function. Values remaining in scope are the two structs that were
   // passed to this function ( infoArrayPtr and I2cTransArrayPtr).*/
    
   
   if (returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)   {
      transArrayPtr = I2cTransArrayPtr;
	  #if (VIRTIO==1)
      if (bspI2c_submitTransaction(BSP_I2C_1, &i2cScenario, transArrayPtr) == BSP_I2C_RETURN_CODE_SUCCESS)
	  #else	
      if (bspI2c_submitTransaction(BSP_I2C_2, &i2cScenario, transArrayPtr) == BSP_I2C_RETURN_CODE_SUCCESS)
	  #endif	
      {
          /* there are two types of sucess codes, 1 for callback pending 
            and the other is for no callback (poll mode)
          */ 
          if ( i2cScenario.transactionDoneCallback ==  NULL)  {
    	       returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
          } 
          else  {
                 returnVal = BSP_TWL3029_RETURN_CODE_CALLBACK_PENDING;
           }
        }
      else   {
         returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
      }
   }
   return returnVal;
} 



/*===========================================================================

 * Function:    BspTwl3029_I2c_regInfoSendContiguous

 *

 * Description: Formats  Triton reigster information into an I2C struct and
 *              sends this to I2C manager which will read/write
 *              from/to registers which are contiguous given in the I2C struct. 
 				Also sends pointer to Triton callback fucntion. *

 * Inputs:     pointer to struct containing Triton register/data pairs(only first register
 				in the transaction)

 *             Uint16 count - number of  registers (continuous locations) to be accessed ( exluding
 *                          paging registers. Count starts from 1. If zero
 *                          count is sent. ffunction returns without doing
 *                          anything( with BSP_TWL3029_RETURN_CODE_SUCCESS return
 *                          code)

 *             pointer to struct containing Triton callback function info
 *             pointer to I2C struct. This function inits the strut before
 *                          sending it to I2C manager
			   The array BspTwl3029_I2C_RegisterInfo and BspI2c_TransactionRequest need to have 
			   only 2 elements. One for the page change and the other for The first element of the 
			   Transactions, the remaining transactions do not need a transaction element.
			   The calling function should make sure that the loction where data is written to(after reading
			   from triton),are in contiguous loctions.
 *             

 * Returns:    BspTwl3029_ReturnCode

 *

 * 

 */

 

BspTwl3029_ReturnCode

 BspTwl3029_I2c_regInfoSendContiguous(BspTwl3029_I2C_RegisterInfo* infoPtr,

                            Uint16 count,

                            BspTwl3029_I2C_CallbackPtr callback, 

                            BspI2c_TransactionRequest* I2cTransArrayPtr)

{

            BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;

    /* use local pointers */

    BspTwl3029_I2C_RegisterInfo* infoArrayPtr = infoPtr;
    BspI2c_TransactionRequest* transArrayPtr = I2cTransArrayPtr;    
    BspI2c_ScenarioDescriptor i2cScenario;
    Uint16 i = 0; 
    BspI2c_TransactionCount j= 0;  /* same type as BspI2c_ScenarioDescriptor.count */
    BspTwl3029_I2C_TypeId typeTemp;

	/* check that count is > 0 */

     if ( count == 0 )  {
         returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
         return returnVal;
     }


    /* select new page */

    typeTemp = infoArrayPtr->typeId ;
    returnVal = BspTwl3029_setI2cPage(infoArrayPtr->typeId, transArrayPtr); 
    j++;
    transArrayPtr++;
    transArrayPtr->transactionType =(BspI2cMaster_TransactionType)infoArrayPtr->regAccType; 
    transArrayPtr->dataStartAddress = (BspI2c_DataAddress)infoArrayPtr->regId;
    transArrayPtr->dataPtr = infoArrayPtr->regDataPtr;
    transArrayPtr->dataCount = count;
#ifdef BSP_I2C_DMA_Enable
    transArrayPtr->dmaRequestId = BSP_DMA_REQ_NONE;
#endif
    j++;

  // transArrayPtr++;

   //infoArrayPtr++;


/* now set up I2C scenario descriptor */

i2cScenario.count = j;
i2cScenario.deviceId = BSP_I2C_DEVICE_ID_TWL3029;

    if (callback != NULL)   {
        i2cScenario.transactionDoneCallback =  callback->callbackFunc; 
 	 i2cScenario.transactionId =(BspI2c_TransactionId)callback->callbackVal;
        i2cScenario.delayFunctionPtr =  NULL;
        i2cScenario.delayValue = 0;
    }

   else   {
        i2cScenario.transactionDoneCallback =  NULL; 
        i2cScenario.transactionId = 0;
        i2cScenario.delayFunctionPtr =  NULL;
        i2cScenario.delayValue = 0;
   }

   /* now call I2c function.
   // note. It is assumed that *scenarioDesc goes out of scope on return from
   // the I2C function. Values remaining in scope are the two structs that were
   // passed to this function ( infoArrayPtr and I2cTransArrayPtr).*/

   if (returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)   {
       transArrayPtr = I2cTransArrayPtr;
       if (bspI2c_submitTransaction(BSP_I2C_2, &i2cScenario, transArrayPtr) == BSP_I2C_RETURN_CODE_SUCCESS)  {

          /* there are two types of sucess codes, 1 for callback pending 
            and the other is for no callback (poll mode)
          */ 
          if ( i2cScenario.transactionDoneCallback ==  NULL)    {
              returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
          }
          else {
             returnVal = BSP_TWL3029_RETURN_CODE_CALLBACK_PENDING;
          }

      }
      else   {
         returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
      }
   }

   return returnVal;

 

}



 /*===========================================================================
 * Function: 
 *
 * Description: default Triton callback function.
 *              This function sets a status flag
 *
 * Inputs:     transNum 
 *             
 * Returns:    none
 *
 * Notes:      can be called  during I2C ISR on completion of
 *             Triton I2C transaction if Triton passes a pointer
 *             to this function to I2C driver.
 * 
 */
 void BspTwl3029_I2c_callbackFunction (BspI2c_TransactionId transNum)
 {
     twl3029_i2cCallbackIsrStatus = BSP_TWL3029_I2C_CALLBACK_COMPLETED;
 }
/*=============================================================================
 * Function: bspTwl3029_i2cCallbackSetup
 *
 * Description:  .
 *              configures the BspTwl3029_I2cCallbackHandler
 *              which  calls on a triton 
 *                driver defined routine to do Triton specific stuff ( such 
 *                processing Triton register data that has just been read)
 *                It then call the external interrupt rountine, that was 
 *                passed as an arg to a Triton driver function by an external
 *                application. 
 *                This function stores the information needed by the handler in
 *                an array of structs 
 *
 * Inputs:     callbackFuncPtr      pointer to struct containing info for i2c callback 
 *             apiCallbackFuncPtr      pointer to struct containing info for i2c callback info that 
 *               was defined by  external application. The callback function defined in this struct 
 *               will be called during the I2C callback routine
 *		 
 *             twl2029callbackFuncPtr  pointer to function originating from the calling triton function
 *                This function will be called during the I2C callback routine. 
 *             
 *             twl2029callbackFuncArgs  pointer to args for triton callback function
 *          
 * Returns:    none
 *
 * Notes:     
 */
Int8 bspTwl3029_I2cCallbackSetup( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                  BspTwl3029_I2C_CallbackPtr apiCallbackInfoPtr,
                                  BspI2c_TransactionDoneCallback  twl2029callbackFuncPtr,
                                 void * twl2029callbackFuncArgs )
{
   /* negative regurn code indicates error */
   Int8 returnCode = -1;   
   if (((idArrayReserveIndex + 1) & 0xf) == idArrayFreeIndex)
   {
       /* error, no space free in array */
       return (returnCode);
   }
   else
   {
       /* store ifoo for Triton and API callback routines */
       ++idArrayReserveIndex;	   
       idArrayReserveIndex = ((idArrayReserveIndex) & 0xf);
       bspTwl3029_IntCInfoArray[idArrayReserveIndex].twl2029callbackFunc = twl2029callbackFuncPtr;
       bspTwl3029_IntCInfoArray[idArrayReserveIndex].callbackFunc = apiCallbackInfoPtr->callbackFunc;
       bspTwl3029_IntCInfoArray[idArrayReserveIndex].callbackVal  = apiCallbackInfoPtr->callbackVal;
       bspTwl3029_IntCInfoArray[idArrayReserveIndex].twl2029callbackFuncArgs = twl2029callbackFuncArgs;
       
       /* now  set up the info  for I2C callback */
       callbackInfoPtr->callbackFunc = BspTwl3029_I2cCallbackHandler;
       callbackInfoPtr->callbackVal = idArrayReserveIndex;
       /* use the api 's transaction array */
       callbackInfoPtr->i2cTransArrayPtr = apiCallbackInfoPtr->i2cTransArrayPtr;
   }
   return ((Int8)idArrayReserveIndex); 
} 
/*===========================================================================
 * Function:     BspTwl3029_I2cCallbackHandler
 *
 * Description:  A Triton  callback function. 
 *                this function calls on a triton 
 *                driver defined routine to do Triton specific stuff ( such 
 *                processing Triton register data that has just been read)
 *                It then call the external interrupt rountine, that was 
 *                passed as an arg to a Triton driver function by an external
 *                application.  
 *
 * Inputs:       BspI2c_TransactionId transNum
 *                this  is used to access information from an array of structs
 *                this is used to retreive the Triton defined callbackfunction 
 *                pointer and args, the application defined functions pointer 
 *                and args 
 *                
 *                                       
 * Returns:    none
 *
 * Notes:       
 */ 
 void   
 BspTwl3029_I2cCallbackHandler (BspI2c_TransactionId transNum)
 {
    /* first call the  triton routine */
    /* Perform callback if defined */
    if ( bspTwl3029_IntCInfoArray[transNum].twl2029callbackFunc != NULL )
    {
        (*(bspTwl3029_IntCInfoArray[transNum].twl2029callbackFunc))(transNum);
	/* reset to NULL*/
	bspTwl3029_IntCInfoArray[transNum].twl2029callbackFunc = NULL;
    
    }

    
    /* then call the applications routine */
    if ( bspTwl3029_IntCInfoArray[transNum].callbackFunc != NULL )
    {
        (*(bspTwl3029_IntCInfoArray[transNum].callbackFunc))(bspTwl3029_IntCInfoArray[transNum].callbackVal);
	/* reset to NULL*/
	bspTwl3029_IntCInfoArray[transNum].callbackFunc = NULL;
    
    }
    ++idArrayFreeIndex;
    idArrayFreeIndex = (idArrayFreeIndex) & 0xf;
    return;
 } 
   

/*===========================================================================
 * Private Functions
 */
 
   /*===========================================================================
 * Function:     BspTwl3029_setI2cPage
 *
 * Description:  select a new page
 *
 * Inputs:      BspTwl3029_I2C_TypeId page - new page
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:
 */
static 
BspTwl3029_ReturnCode 
BspTwl3029_setI2cPage(BspTwl3029_I2C_TypeId page,
                      BspI2c_TransactionRequest*  transaction)

{
   static BspI2c_Data pageId[4] = {0,1,2,3};
   
   /* only two out of the 4 pages are used  for PG1.x, 3 for harware PG2.0*/
   if ( page >BSP_TWL3029_I2C_PAGEMAX)
   {
      return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   /*  select new page  */
   transaction->transactionType = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
   /* select page register */
   transaction->dataStartAddress = BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET;
   /* select page register data */
   transaction->dataPtr =   &pageId[page];//&page;
   transaction->dataCount = 1;
   #ifdef BSP_I2C_DMA_Enable
   transaction->dmaRequestId = BSP_DMA_REQ_NONE;
   #endif
   
   return BSP_TWL3029_RETURN_CODE_SUCCESS;
	   
}


