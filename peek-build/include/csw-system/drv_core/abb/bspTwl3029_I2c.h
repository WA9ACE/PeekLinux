/*==============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_I2C_HEADER
#define BSP_TWL3029_I2C_HEADER

//#include "main_system.h"
#include "types.h"
#include "bspTwl3029.h"
#include "bspI2c.h"


#if (ANLG_PG == 1)
#include "pg1x/bspTwl3029_I2c_pg1x.h"
#elif (ANLG_PG == 2)
#include "pg2x/bspTwl3029_I2c_pg2x.h"
#endif 
/*==============================================================================
 * File Contents:
 *   This module contains function definitions and macros 
 *   associated with Triton ABB.
 */

/*===========================================================================
 * Defines and Macros
 */
 

/*===========================================================================*/
/*
 * typedef BspTwl3029_ReturnCode
 *
 * Description: return code for twl3029 functions
 *
 */
enum
{
    BSP_TWL3029_I2C_PAGE0 = 0,
    BSP_TWL3029_I2C_PAGE1 = 1,
    BSP_TWL3029_I2C_PAGE2 = 2,
    BSP_TWL3029_I2C_PAGE3 = 3
};
typedef Uint8 BspTwl3029_I2C_TypeId;



/*==============================================================================
 * Description:
 *   This data type is used to hold the read/write portion of a command.
 *
 *   Note: the values given below correspond to the bspI2C
 *   enum BspI2cMaster_TransactionType elements
 *    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE  and
 *    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ
 */

enum
{
    BSP_TWL3029_I2C_WRITE = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE,
    BSP_TWL3029_I2C_READ  = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ
   
};
typedef Uint8 BspTwl3029_I2C_ReadWrite;


/*===========================================================================
 * typedef BspTwl3029_I2C_RegId
 *
 *  Description 
 *   A register address. All register addresses are defined in the *.map.h
 *   files
 */
typedef Uint8 BspTwl3029_I2C_RegId;

/*===========================================================================
 * typedef BspTwl3029_I2C_RegData
 *
 *  Description 
 *   Data read from/written to  Triton register
 */
typedef Uint8 BspTwl3029_I2C_RegData;

/*===========================================================================
 * typedef BspTwl3029_I2C_RegMask
 *
 *  Description 
 *   Triton register bit mask. All masks are defined in 1 of 4 Llif.h files
 */
typedef Uint8 BspTwl3029_I2C_RegMask;

/*===========================================================================
 * typedef BspTwl3029_RegisterInfo
 *
 *  Description                 
 *   A TWL device register comprises a typeId which is used as a page 
 *   selector value,  a registeraddress value within the page and a pointer 
 *   to a variable for storing of data to be transferred to/from 
 *   Triton register
 */
typedef struct
{
   BspTwl3029_I2C_TypeId              typeId;   
   BspTwl3029_I2C_RegId               regId;
   BspTwl3029_I2C_RegData *	      regDataPtr;
   BspTwl3029_I2C_ReadWrite           regAccType; 
} BspTwl3029_I2C_RegisterInfo;

/*===========================================================================*/
/*
 * typedef Bsp_Twl3029_I2cTransReqArrayPtr
 *
 * Description: 
 *   points to an array Bsp_Twl3029_I2cTransReqArray
 *   this array consists of 10 elements of type BspI2c_TransactionRequest
 *
 */
   //BspI2c_TransactionRequest* 
  typedef BspI2c_TransactionRequest Bsp_Twl3029_I2cTransReqArray[10];
  typedef Bsp_Twl3029_I2cTransReqArray*  Bsp_Twl3029_I2cTransReqArrayPtr;
/*===========================================================================*/
/*
 * typedef BspTwl3029_I2C_CallbackStatus
 *
 * Description: struct containing call back function STATUS ( COMPLETED OR
 *              NOT COMPLETED ) 
 *
 */
 
 enum 
 {
    BSP_TWL3029_I2C_CALLBACK_NOTCOMPLETED = 0,
    BSP_TWL3029_I2C_CALLBACK_COMPLETED = 1
 };
 typedef volatile Uint8 BspTwl3029_I2c_CallbackStatus;
/*===========================================================================*/
/*
 * typedef BspTwl3029_I2C_CallbackPtr
 *
 * Description: struct containing the following:
 *       - call back function pointer
 *       - callback value. this will be as arg to function by I2C
 *       -  A pointer to a transaction array. Triton driver ( I2c component) will
 *          store the nessary data here for I2C. Note memory allocated to array
 *          is required to remain in scope until completion of requestedI2C 
 *          transactions
 *       
 */
 
 typedef struct 
 {   BspI2c_TransactionDoneCallback  callbackFunc; 
     BspI2c_TransactionId              callbackVal;
     Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr;
 } BspTwl3029_I2C_Callback;

typedef BspTwl3029_I2C_Callback* BspTwl3029_I2C_CallbackPtr;

/*===========================================================================*/
/*
 * typedef BspTwl3029_I2C_CallbackPtr
 *
 * Description: struct containing the following:
 *       - call back function pointer
 *       - callback value. this will be as arg to function by I2C
 *       -  A pointer to a transaction array. Triton driver ( I2c component) will
 *          store the nessary data here for I2C. Note memory allocated to array
 *          is required to remain in scope until completion of requestedI2C 
 *          transactions
 *       
 */
typedef struct
{
    BspI2c_TransactionDoneCallback  twl2029callbackFunc;  /* triton callback func */
    BspI2c_TransactionDoneCallback  callbackFunc;         /* application callback func */
    BspI2c_TransactionId            callbackVal;          /* arg for applcation callback func */ 
    void *                          twl2029callbackFuncArgs; /* arg(s) for triton callback func */
    //Bsp_Twl3029_I2cTransReqArray    i2cTransArray;      
} BspTwl3029_IntCInfo;

/*===========================================================================
 * Local Data
 */

/*===========================================================================
 * Public Functions 
 *===========================================================================
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
 BspTwl3029_ReturnCode BspTwl3029_I2c_init(void);
 
/*===========================================================================
 * Function:    BspTwl3029_I2c_shadowRegRead
 *
 * Description: Stores the Triton register access info ( reg address, data buffer 
 *              mask) in the struct pointer to by regInfoPtr
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This function DOES NOT update the shadow registers. 
 */
 BspTwl3029_ReturnCode BspTwl3029_I2c_shadowRegRead( BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData *regData);
 
 /*===========================================================================
 * Function: 
 *
 * Description: requests I2C manageer to read a single register
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This function is blocking. It won't return until 
 *             I2C has finished its transaction. 
 */
BspTwl3029_ReturnCode BspTwl3029_I2c_WriteSingle(BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData regData, BspTwl3029_I2C_CallbackPtr callbackInfoPtr);  
       
/*===========================================================================
 * Function:    BspTwl3029_I2c_regQueWrite
 *
 * Description: Stores the Triton register access info ( reg address, data buffer 
 *              mask) in the struct pointer to by regInfoPtr
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This writes to COMPLETE register, for partial field update use
 *             BSP_TWL3027_LLIF_SET_FIELD command on shadow reg prior to 
 *             this function
 */
 BspTwl3029_ReturnCode BspTwl3029_I2c_shadowRegWrite(BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData regData);
 
 /*===========================================================================
 * Function: 
 *
 * Description: requests I2C manageer to read a single register
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
 BspTwl3029_ReturnCode BspTwl3029_I2c_ReadSingle(BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData *regData, BspTwl3029_I2C_CallbackPtr callbackInfoPtr);
    
/*===========================================================================
 * Function: 
 *
 * Description: Stores the Triton register access info ( reg address, data buffer 
 *              mask) in the struct pointer to by regInfoPtr
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This function DOES NOT update the shadow registers. 
 */
 BspTwl3029_ReturnCode BspTwl3029_I2c_regQueRead( BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData *regData,
      BspTwl3029_I2C_RegisterInfo *regInfoPtr);
     
/*===========================================================================
 * Function:    BspTwl3029_I2c_regQueWrite
 *
 * Description: Stores the Triton register access info ( reg address, data buffer 
 *              mask) in the struct pointer to by regInfoPtr
 *
 * Inputs:      regAddress - triton register address 
 *           
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 *             This writes to COMPLETE register, for partial field update use
 *             BSP_TWL3027_LLIF_SET_FIELD command on shadow reg prior to 
 *             this function
 */
 BspTwl3029_ReturnCode BspTwl3029_I2c_regQueWrite(BspTwl3029_I2C_TypeId typeId,
     BspTwl3029_I2C_RegId regAddress,
     BspTwl3029_I2C_RegData regData,
     BspTwl3029_I2C_RegisterInfo *regInfoPtr); 
    


BspTwl3029_ReturnCode
 BspTwl3029_I2c_regInfoSendContiguous(BspTwl3029_I2C_RegisterInfo* infoPtr,

                                                Uint16 count,

                                              BspTwl3029_I2C_CallbackPtr callback, 

                                                  BspI2c_TransactionRequest* I2cTransArrayPtr);





/*===========================================================================
 * Function: 
 *
 * Description: Sends the I2C strut to I2C manager which will read/write 
 *              from/tpo registers given in the I2C struct. Also sends pointer
 *              to Triton callback fucntion.
 *
 * Inputs:     I2C struct pointer
 *             Triton callback function info
 *             
 * Returns:    BspTwl3029_ReturnCode
 *
 * Notes:      Use by other bspTwl3029 files.
 */



 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_regInfoSend(BspTwl3029_I2C_RegisterInfo*,
                           Uint16 count,
			   BspTwl3029_I2C_CallbackPtr,
                           BspI2c_TransactionRequest*);

/*===========================================================================
 * Function: 
 *
 * Description: default Triton callback function.
 *              This function sets a status flag
 *
 * Inputs:     none
 *             
 * Returns:    none
 *
 * Notes:      called  during I2C ISR on completion of
 *             Triton I2C transaction .
 */
 void BspTwl3029_I2c_callbackFunction (BspI2c_TransactionId transNum);

/*=============================================================================
 * Function: bspTwl3029_i2cCallbackSetup
 *
 * Description:  .
 *              configures the BspTwl3029_I2cCallbackHandler
 *              which  calls on a triton 
 *                driver defined routine to do Triton specific stuff ( such 
 *                processing Triton register data that has just been read)
 *                It then calls the external interrupt rountine, that was 
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
                                  void * twl2029callbackFuncArgs );
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
 BspTwl3029_I2cCallbackHandler (BspI2c_TransactionId transNum);
/*===========================================================================
 * Private Functions
 *===========================================================================
 */

/*===========================================================================
 * Function:     BspTwl3029_setI2cPage
 *
 * Description:  select a new page
 *
 * Inputs:      BspTwl3029_I2C_TypeId page - new page
 * Returns:     
 *
 * Notes:
 */
static 
BspTwl3029_ReturnCode BspTwl3029_setI2cPage(BspTwl3029_I2C_TypeId page,
    BspI2c_TransactionRequest*  transaction);
    
    
                             
#endif
