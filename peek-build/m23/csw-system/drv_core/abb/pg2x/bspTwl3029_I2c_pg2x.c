/*==============================================================================
 *    Copyright 2004 Texas Instruments Inc. All rights reserved.
 */

//#include "main_system.h"
#include "types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"
#include "bspTwl3029_I2c_pg2x.h"


/*==============================================================================
 * File Contents:
 *   This module contains functions associated with Triton ABB. It is used 
 *   within the ABB driver only.
 *
 *   The functions and macros are used for preparing and sending data strutures
 *   for accessing Triton registers via the bsp I2C manager.
 */

/*===========================================================================
 *  Data
 */
extern bspTwl3029_Reg shadowRegs; 
/*===========================================================================
 *  Functions
 *===========================================================================
 */
/*===========================================================================
 * Function:    BspTwl3029_I2c_shadowRegInit
 *
 * Description: inits shadow register strut by reading Triton registers
 *              This function should be called by bspTwl3029_I2cInit function
 * Inputs:     none
 *             
 * Returns:    BspTwl3029_ReturnCode
 *
 * Notes:      called  during initialzation.
 */

BspTwl3029_ReturnCode 
BspTwl3029_I2c_shadowRegInit(void)
{
   
   BspTwl3029_ReturnCode returnVal =  BSP_TWL3029_RETURN_CODE_SUCCESS;
   /* callback function  */
    
   /* This Code use to Take About 4K of stack which we dont want to
    * allocate in initial SP (This Fcuntion gets called from App_Init)
    * . Since Riviera will be initalised after this Call we will reuse the 
    * Riviera pool memory instead.
    * Note:- When Riviera is removed please replace it with any other memory 
    * location */
   /* twl3029 I2C reg info struct */
#if (REMU==1)
   #if (OP_L1_STANDALONE == 1)
   /* In case of L1 Standalone build we dont have Riviera to reuse memory from 
   so creating a local structure. */
    static Uint8 ext_data_pool[1024*6];
   #else
    extern Uint8 ext_data_pool[];
   #endif
#else
   #if (OP_L1_STANDALONE == 1)
   /* In case of L1 Standalone build we dont have Riviera to reuse memory from 
   so creating a local structure. */
    static Uint8 rvf_pool_external_mem[1024*6];
   #else
    extern Uint8 rvf_pool_external_mem[];
   #endif
#endif   

#if (REMU==1)
   Uint8 *data_pool_ptr=ext_data_pool;
#else
   Uint8 *data_pool_ptr=rvf_pool_external_mem;
#endif   

    BspTwl3029_I2C_RegisterInfo* regInfo = 
					   (BspTwl3029_I2C_RegisterInfo*)(data_pool_ptr);
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
   
    
    /*  i2c transction array */
     BspI2c_TransactionRequest* i2cShadowTransArray = 
  						(BspI2c_TransactionRequest*)((Uint8*)data_pool_ptr 
							+ sizeof(BspTwl3029_I2C_RegisterInfo)*255);
     BspI2c_TransactionRequest* i2cShadowTransArrayPtr = i2cShadowTransArray;
    
      
    Uint16 count = 0;
   
   int i;   
   /* set to zeros */
   for ( i = 0; i <= 255; i++)
   {
      shadowRegs.page0[i] = 0;
      shadowRegs.page1[i] = 0;
      shadowRegs.page2[i] = 0;
   }
  
   /* aux  USB regs 0 - 17 */
  
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE0,(Uint8)0, &shadowRegs.page0[0],regInfoPtr);
      
     returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,18,NULL,i2cShadowTransArrayPtr);

 
   /* aux  USB regs 26 - 31 */
  
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE0,(Uint8)26, &shadowRegs.page0[26],regInfoPtr);
     
 	if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)       
	{
		returnVal= BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(31-26+1),NULL,i2cShadowTransArrayPtr);
	}

   
   /* aux   40 - 141 ( MADC,AUX_REG, VIB, WLED,BCI, SIM ) */
  
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE0,(Uint8)40, &shadowRegs.page0[40],regInfoPtr);
    
        if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)       
        {
          		returnVal= BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(141-40+1),NULL,i2cShadowTransArrayPtr);
        }

   /* audio regs */
       /* for ( i = 208; i <= 235; i++)*/

	/*write only 225 to 235 because 208 to 225 is initialized in   bspTwl3029_Audio_init() */
   
         BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE0,(Uint8)225, &shadowRegs.page0[225],regInfoPtr);
  
  
	if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)      
	{
		returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(235-225+1),NULL,i2cShadowTransArrayPtr);
	}    
	    
	  BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE0,(Uint8)244, &shadowRegs.page0[244],regInfoPtr);

	if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)      
	{
		returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(254-244+1),NULL,i2cShadowTransArrayPtr);
    }   
       
    /* now for page 1 (power ) registers */
   
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE1,(Uint8)0, &shadowRegs.page1[0],regInfoPtr);
    
	if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)      
       {
       	returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(148),NULL,i2cShadowTransArrayPtr);
	}
   /* now request to I2C manager to read  Triton Page 1 registers */
   
  
   /* now for page 2 (USB ) registers */
    
   /* aux  USB regs 0 - 17 */
  
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE2,(Uint8)0, &shadowRegs.page2[0],regInfoPtr);
 
   if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)      
   {
   	returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(18),NULL,i2cShadowTransArrayPtr);

   }
   /* aux  USB regs 26 - 31 */
  
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE2,(Uint8)26, &shadowRegs.page2[26],regInfoPtr);
  
    if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)      
    {
    	returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(31-26+1),NULL,i2cShadowTransArrayPtr);
   }
   /* aux  USB regs 160 - 209 */
   
      BspTwl3029_I2c_regQueRead(BSP_TWL3029_I2C_PAGE2,(Uint8)160, &shadowRegs.page2[160],regInfoPtr);
  if(returnVal== BSP_TWL3029_RETURN_CODE_SUCCESS)        
  {
  	returnVal=BspTwl3029_I2c_regInfoSendContiguous(regInfoPtr,(209-160+1),NULL,i2cShadowTransArrayPtr);
  }
   
   return returnVal;
}


 
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
 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_shadowRegRead(BspTwl3029_I2C_TypeId typeId,
                              BspTwl3029_I2C_RegId regAddress,
                              BspTwl3029_I2C_RegData *regData) 
{
    
   if ((typeId > BSP_TWL3029_I2C_PAGEMAX ))
   {
      return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   /* update shadow register  */
   if (typeId == BSP_TWL3029_I2C_PAGE0)
   {
       *regData = shadowRegs.page0[regAddress] ;
   }
   else if (typeId == BSP_TWL3029_I2C_PAGE1)
   {
       *regData = shadowRegs.page1[regAddress] ;
   }
   else
   {
      *regData = shadowRegs.page2[regAddress] ;
   }  
   return BSP_TWL3029_RETURN_CODE_SUCCESS;
}

/*===========================================================================
 * Function:    BspTwl3029_I2c_shadowRegWrite
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
 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_shadowRegWrite(BspTwl3029_I2C_TypeId typeId,
                               BspTwl3029_I2C_RegId regAddress,
                               BspTwl3029_I2C_RegData regData) 
     
 {
    
    if (typeId > BSP_TWL3029_I2C_PAGEMAX )
    {
       return BSP_TWL3029_RETURN_CODE_FAILURE;
    }
    /* update shadow register   */
    if (typeId == BSP_TWL3029_I2C_PAGE0)
    {
        shadowRegs.page0[regAddress] = regData;
	/* for USB also update page 2 */
	if (regAddress < 40)
	{
	   shadowRegs.page2[regAddress] = regData;
	}
    }
    else 

	{
			if (typeId == BSP_TWL3029_I2C_PAGE1)
    {
        shadowRegs.page1[regAddress] = regData;
    }
    else
    {
       shadowRegs.page2[regAddress] = regData;
       /*  for USB also update page 0  ( if shared )*/
	if (regAddress < 40)
	{
	   shadowRegs.page0[regAddress] = regData;
	}
    }  
    }  
    return BSP_TWL3029_RETURN_CODE_SUCCESS;
 }

/*===========================================================================
 * Function:    BspTwl3029_I2c_regQueRead
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
 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_regQueRead(BspTwl3029_I2C_TypeId typeId,
                           BspTwl3029_I2C_RegId regAddress,
                           BspTwl3029_I2C_RegData *regData, 
                           BspTwl3029_I2C_RegisterInfo* regInfoPtr)
{
    if ((typeId >BSP_TWL3029_I2C_PAGEMAX))
    {
       return BSP_TWL3029_RETURN_CODE_FAILURE;
    }
    
    /* don't allow access to USB page 0 reserved registers */
    /* this is to catch errors due to  any code  using obsolate
       PG1.0 register addresses   */
    
    if ((typeId == BSP_TWL3029_I2C_PAGE0)&&
        ( ( (regAddress >= 18 ) && (regAddress <=25 )) ||
          ( (regAddress >= 32 ) && (regAddress <=39 ))))
    {
       return BSP_TWL3029_RETURN_CODE_FAILURE;
    }
    
    regInfoPtr->typeId = typeId; 
    regInfoPtr->regId = regAddress;
    regInfoPtr->regDataPtr = regData;
    regInfoPtr->regAccType = BSP_TWL3029_I2C_READ;
    return BSP_TWL3029_RETURN_CODE_SUCCESS;
}
 

 

/*===========================================================================
 * Function:     BspTwl3029_I2c_regQueWrite
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
 BspTwl3029_ReturnCode 
 BspTwl3029_I2c_regQueWrite(BspTwl3029_I2C_TypeId typeId,
                            BspTwl3029_I2C_RegId regAddress,
                            BspTwl3029_I2C_RegData regData, 
                            BspTwl3029_I2C_RegisterInfo *regInfoPtr)
 {
    
    if (typeId > BSP_TWL3029_I2C_PAGEMAX )
    {
       return BSP_TWL3029_RETURN_CODE_FAILURE;
    }
    /* don't allow access to USB page 0 reserved registers */
    /* this is to catch errors due to  any code  using obsolate
       PG1.0 register addresses   */
    
    if ((typeId == BSP_TWL3029_I2C_PAGE0)&&
        ( ( (regAddress >= 18 ) && (regAddress <=25 )) ||
          ( (regAddress >= 32 ) && (regAddress <=39 ))))
    {
       return BSP_TWL3029_RETURN_CODE_FAILURE;
    }
    if (typeId == BSP_TWL3029_I2C_PAGE0)
    {
        /* update shadow register */
	shadowRegs.page0[regAddress] = regData;
	
	/*  the following pointer will be passed to I2c manager*/
	regInfoPtr->regDataPtr = &shadowRegs.page0[regAddress];
    }
else 
{
   if (typeId == BSP_TWL3029_I2C_PAGE1)
    {
        /* update shadow register */
	shadowRegs.page1[regAddress] = regData;
	
	/*  the following pointer will be passed to I2c manager*/
	regInfoPtr->regDataPtr = &shadowRegs.page1[regAddress];
    }
    else
    {
       shadowRegs.page2[regAddress] = regData;
       regInfoPtr->regDataPtr = &shadowRegs.page2[regAddress];
    }  
}
    regInfoPtr->typeId = typeId; 
    regInfoPtr->regId = regAddress;
    
    regInfoPtr->regAccType = BSP_TWL3029_I2C_WRITE; 
    return BSP_TWL3029_RETURN_CODE_SUCCESS;
 }

