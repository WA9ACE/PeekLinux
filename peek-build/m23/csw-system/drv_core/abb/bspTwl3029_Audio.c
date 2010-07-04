/*=============================================================================
 *    Copyright 2002-2004 Texas Instruments Inc. All rights reserved.
 */
#include "types.h"

#include "bspTwl3029.h"
#include "bspTwl3029_Aud_Llif.h"
#include "bspTwl3029_Aud_Map.h"
#include "bspTwl3029_Int_Llif.h"
#include "bspTwl3029_I2c.h"

#include "bspTwl3029_Int_Map.h"
#include "nucleus.h" 
#include "bspTwl3029_Intc.h"
#include "sys_types.h"
#include "bspTwl3029_Audio.h"

#if(OP_L1_STANDALONE==0)
#include "../rv/rv_general.h"
#include"../Audio/audio_api.h"
#include "../../riviera/rvf/rvf_original_api.h"
#include "../../riviera/rvm/rvm_use_id_list.h"
#endif


/*For trace */

#if(OP_L1_STANDALONE==0)
#include "../Audio/audio_macro_i.h"
#include"../rv/rv_trace.h"
#include "rv/rv_defined_swe.h"
#include "rvm/rvm_use_id_list.h"
#include"rvf_api.h"
#include"stddef.h"
#endif

#if (ANLG_PG == 1)
#include "pg1x/bspTwl3029_Audio_pg1x.h"
#elif (ANLG_PG == 2)
#include "pg2x/bspTwl3029_Audio_pg2x.h"
#endif 

#ifdef TI_BSP_FF_AUDIO_ON_OFF
#include "nucleus.h"

#define RVF_MS_TO_TICKS(_x)   ((((_x) * 200) + 462) / 923) 
#define RVF_SECS_TO_TICKS(_x) ((((_x) * 1000 * 200) + 462) / 923)
#define RVF_TICKS_TO_MS(_x)   ((((_x) * 923) + 100) / 200)

NU_TIMER tdma_1_ul_on_timer;
NU_TIMER tdma_2_dl_on_timer;
NU_TIMER tdma_ul_on_dl_on_timer;
	
Uint8 audio_uplink_on_downlink_on_state;
Uint8 audio_uplink_on_state;
Uint8 audio_downlink_on_state;
	
T_AUDIO_ON_OFF_CONTROL_RETURN audio_on_off_return[8];
T_AUDIO_OUTEN_REG audio_outen_reg;

T_AUDIO_OUTEN_REG audio_outen_reg_as = { BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT, \
                                      BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT, \
                                      BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT \
                                    };


void bspTwl3029_audio_on_off_timer_expiry(UNSIGNED expiry_id);

BspTwl3029_I2C_RegData globalRegRead;

#endif 


/*=============================================================================
 *   Private Functions
 *============================================================================*/

/*===========================================================================
 * Function:     bspTwl3029_AudioSetGain
 *
 * Description:  sets a field in a gain register
 *
 * Inputs:       BspTwl3029_I2C_RegisterInfo* regInfoPtr - used for I2c queueing
 *               BspTwl3029_Audio_GainSettings*  newGainSettings - stru containin
                                                  gain settings 
		BspTwl3029_AudioRegFieldInfo* arrayPtr - reg, field offset and field width
		Uint8* count  - a counter used for i2c queueing 				  
 *               
 * Returns:     BspTwl3029_ReturnCode
 *
 * Notes:       called by bspTwl3029_Audio_gainControl
 */ 
static BspTwl3029_ReturnCode 
bspTwl3029_AudioSetGain( BspTwl3029_I2C_RegisterInfo** regInfoPtr,
                          Uint8  newGainSetting,
		          BspTwl3029_AudioRegFieldInfo* arrayPtr,
			  Uint8 *count)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   Uint8 tmpRegData, tmpRegRead;   
 
   /* read shadow register */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, arrayPtr->registerId,&tmpRegRead);
   
   tmpRegData  =  tmpRegRead ;
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpRegData,
                                       newGainSetting,
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       arrayPtr->regBitOffset,
                                       arrayPtr->regBitWidth );			    

   /* check if register needs to be updated  */
   if (tmpRegData == tmpRegRead )
   {
      /*if new setting is same as previous setting - then no updating required */
      returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   }
   else if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
   {
      /*else update triton registers */
      	 
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,arrayPtr->registerId,
			         tmpRegData,(*regInfoPtr)++);	
      (*count)++;  	
   
 
   }
  return returnVal;
}


/*===========================================================================
 * Function:     BspTwl3029_I2c_callbackFunctionAud
 *
 * Description:  Triton Audio callback function. this function sets a flag to 
 *               indicate that I2C transaction is complete.
 *
 * Inputs:       BspI2c_TransactionId transNum
 *                                       
 * Returns:    none
 *
 * Notes:       called during bspTwl3029_Audio_init
 */ 
 static Uint8 transactionDoneAud;
 
 static void   
 BspTwl3029_I2c_callbackFunctionAud (BspI2c_TransactionId transNum)
 {
    if (transactionDoneAud != 1)
    {
    transactionDoneAud = 1;
    }
    return;
 } 

/*===========================================================================
 * Function:     BspTwl3029_Audio_fmRemove
 *
 * Description:  this function checks  the 
 *               outPathSource which is in a format supported by registers 
 *               OUTEN 1 and 3. It coverts this format to the format supported
 *               by OUTEN2. 
 *               ( example: if 
 *
 * Inputs:       BspI2c_TransactionId transNum
 *                                       
 * Returns:    none
 *
 * Notes:       If FM radio is requested signal for outPathSource then the
 *              return will exclude the FM radio as the outputs controlled by
 *              OUTEN2 do not support FM radio. ( For example if outPath is 
 *              BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM, this function 
 *              will return BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE
 */  
/* first  get outen2Src from  audioSource - outen2Src should give the same
      output as chosen by audioSource without the FM radio*/
#ifdef BSPTWL3029_FM_REMOVE
static BspTwl3029_AudioOutputSource   
BspTwl3029_Audio_fmRemove(BspTwl3029_AudioOutputSource outPathSource )
{
   BspTwl3029_AudioOutputSource outen2Source = 0;
   if (outPathSource <=  BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO)
   {
      outen2Source = outPathSource;
   }
   else if (outPathSource == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO)
   {
      outen2Source = 3;
   }
   else if (outPathSource == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM)
   {
      outen2Source =  BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
   }
   else if (outPathSource == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM)
   {
      outen2Source =  BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
   } 
   return outen2Source;
}   
#endif
/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   This initializes the Audio section. This contains a SW workaround for
 *   initialization of the audio parts of the ABB to avoid white noise. The
 *   Audio section is powered down, then a delay of 1ms before it is powered
 *   back up.
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_init( void )
{
    
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegisterInfo regArray[2];
   BspTwl3029_I2C_RegisterInfo*  regArrayPtr = regArray;
   
   /* callback function  */
     BspTwl3029_I2C_Callback audInitCallback;
     BspTwl3029_I2C_CallbackPtr audInitCallbackPtr= &audInitCallback; 
   
   /*  i2c transction array */
    BspI2c_TransactionRequest i2cAudInitArray[2];  
    BspI2c_TransactionRequest* i2cAudInitArrayPtr = i2cAudInitArray;
   
   Uint16 count = 0;
   
 
   returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,
       BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,BSP_TWL_3029_MAP_AUDIO_TOGB_DEFAULT,NULL);
       
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      returnVal =  BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
                      BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL1_DEFAULT);
     count++;
   } 
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {   
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
                     BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL2_DEFAULT);
      count++;
   } 
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL3_DEFAULT);
      count++;
   } 
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {    
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
         BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL4_DEFAULT);
      count++;
   }    
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {    
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
         BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL5_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {      
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
         BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET,BSP_TWL_3029_MAP_AUDIO_CTRL6_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {          
      /* default: pop reduction  Manual off */ 
	/*OMAPS00086902*/
	returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
		BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,0x00);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {   
       /* default: pop reduction      auto ( carkit, Aux, Ear, HS) on. */
       returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
		BSP_TWL_3029_MAP_AUDIO_POPAUTO_OFFSET,0x00);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {       
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_SIDETONE_OFFSET,BSP_TWL_3029_MAP_AUDIO_SIDETONE_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {          
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,BSP_TWL_3029_MAP_AUDIO_VULGAIN_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {          
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_VDLGAIN_OFFSET,BSP_TWL_3029_MAP_AUDIO_VDLGAIN_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {       
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
         BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {          
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT);       
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {   
      returnVal =BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT);
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {          
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
          BSP_TWL_3029_MAP_AUDIO_AUDLGAIN_OFFSET,BSP_TWL_3029_MAP_AUDIO_AUDLGAIN_DEFAULT);   
      count++;
   }    
   
   if ( returnVal == BSP_TWL3029_RETURN_CODE_SUCCESS)
   {      
      returnVal = BspTwl3029_I2c_shadowRegWrite(BSP_TWL3029_I2C_AUD,
         BSP_TWL_3029_MAP_AUDIO_AUDRGAIN_OFFSET,BSP_TWL_3029_MAP_AUDIO_AUDRGAIN_DEFAULT);
      count++;
   }    
   
  
    /* now send to I2C */
    regArrayPtr = regArray;
/*do a dummy regqueue write to update the reginfo*/
BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
                      BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,
                      BSP_TWL_3029_MAP_AUDIO_CTRL1_DEFAULT,regArrayPtr);
	
    /* now request to I2C manager to write to Triton registers */
   
        /*
	 * when no  triton callback function is defined,  I2C will function
	 * poll mode
	 */
        audInitCallbackPtr->callbackFunc = BspTwl3029_I2c_callbackFunctionAud;       
	audInitCallbackPtr->callbackVal = NULL;  
	audInitCallbackPtr->i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)i2cAudInitArrayPtr;
    	transactionDoneAud = 0;

#if (ANLG_PG == 1)
    	//BspTwl3029_I2c_regInfoSend(regArrayPtr,count,NULL, i2cAudInitArrayPtr); 
	transactionDoneAud = 1;
	
#ifdef TI_BSP_FF_AUDIO_ON_OFF
   bspTwl3029_audio_on_off_init();
#endif	
#elif (ANLG_PG == 2)
       	BspTwl3029_I2c_regInfoSendContiguous(regArrayPtr,(count),NULL, i2cAudInitArray); 
	transactionDoneAud = 1;
	
#ifdef TI_BSP_FF_AUDIO_ON_OFF
   bspTwl3029_audio_on_off_init();
#endif	
#endif

    	return   returnVal;   
   
}   


/*=============================================================================
 * Description:
 *   This function powers up or down the audio subsystem.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_powerControl( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                               BspTwl3029_Audio_PowerControl powerControl, 
			       BspTwl3029_Audio_PowerModule  pwrModule )
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;    
    BspTwl3029_I2C_RegData regData=0, shadowRegRead, shadowRegData;
    BspTwl3029_I2C_RegData popmanRead, popmanData, temp; 
    
    Uint8 shadowStatusbit, togbSetShift = 0; 
    Uint16 count = 0;
        
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
      

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
    
    /* callback function info pointer */
     BspTwl3029_I2C_CallbackPtr callbackPtr= callbackFuncPtr; 
    
    
    /* note: Power is set/reset through TOGB register and powerstaus is given in  PWRONSTATUS register  */ 
    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);   
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                             &popmanRead);
    
    shadowRegData = shadowRegRead;
    popmanData = popmanRead;
    


    if( powerControl == BSP_TWL3029_POWER_CONTROL_DISABLE )
    {
       
       togbSetShift = 0;  /* reset shift*/
       shadowStatusbit = 0;
    } 
    
    else
    {
        /*set shift  The "set" bits in TOGB register is the same as the 'reset' bit shifted by 1*/
       togbSetShift = 1;
       shadowStatusbit = 1; 
    } 


    if ((pwrModule & BSP_TWL3029_AUDIO_POWER_MODULE_VUL) != 0)
    {
          regData = 1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULR_OFFSET + togbSetShift);
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegData, shadowStatusbit ); 
    }
    if ((pwrModule & BSP_TWL3029_AUDIO_POWER_MODULE_VDL) != 0)
    {
          regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLR_OFFSET + togbSetShift));
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegData, shadowStatusbit ); 
    }
    if ((pwrModule & BSP_TWL3029_AUDIO_POWER_MODULE_STEREO) != 0)
    {
          regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_STR_OFFSET + togbSetShift));
	  BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_STON,  &shadowRegData, shadowStatusbit ); 
    }
             
    
    /* update Triton power */
    
    returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++) ;
    count ++;
    
    
    
    /* update shadow  power  reg, this is used by other functions as an alternative to reading the HW register */
    if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
       BspTwl3029_I2c_shadowRegWrite( BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET, shadowRegData);
    }
    
    
    /* do  pop control check  - only if  audio and stero output power are both OFF at end of this function*/    
    if ( (( shadowRegRead & 0x6 ) != 0)&&((shadowRegData & 0x6) == 0)) 
      
    {
	/* if the DHG bit is one then anti POP can be considered to be enabled to set */
	/* get xDHG bits and shift left 1  to give required values for CHG bits */
	temp = (popmanRead & 0xaa) >> 1;
	
	popmanData = (popmanRead & 0xaa) |  temp;          
        if (popmanData != popmanRead)
	{
	   /* note: it POP auto is enabled  the status of the  xCHG bits are ignored by HW */
	    /*returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
		 *  popmanData,  regInfoPtr++) ;
	     * count++;
	    */
	}
    }
    
    
    /* now  queue and send the registers  to I2C manager */  
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    } 
    
    if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
       regInfoPtr = regInfo;
       /* now request to I2C manager to write to Triton registers */
    
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,count,callbackPtr,
          (BspI2c_TransactionRequest*)i2cTransArrayPtr);
    }
    
    return returnVal;
}



/*=============================================================================
 * Description:
 *   For Twl3029 the input and output path may be different unless they 
 *   are loop.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_configureInputPath( BspTwl3029_I2C_CallbackPtr callbackFuncPtr, 
                                     BspTwl3029_Audio_InputPathId   inputPathId)
 

{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData tmpCtrl2RegData = 0;
   BspTwl3029_I2C_RegData tmpCtrl3RegData = 0;
   BspTwl3029_Audio_InputMicBias  micBias = 0;
   
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
        

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[5] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
    
    /* callback function  */
    BspTwl3029_I2C_Callback callback;
    BspTwl3029_I2C_CallbackPtr callbackPtr= &callback; 
    
   
   if (inputPathId > INPUT_PATH_MAX )
   {
      /* error */
      return BSP_TWL3029_RETURN_CODE_FAILURE;
   } 
   
   /* read shadow registers for UL paths */
   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,
                             &tmpCtrl2RegData);
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
                             &tmpCtrl3RegData);
			     
   switch( inputPathId )
    {     
	case BSP_TWL3029_AUDIO_INPUT_PATH_ID_HANDSET_MIC_25_6DB:
        {           
	    micBias = BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HANDSET_MIC_25_6DB_DEFAULT;
	}
	break;
	case BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_4_9_DB:
        {           
	    micBias = BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_4_9_DB_DEFAULT;
	}
	break;
	case BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_25_6DB:
        {           
	     micBias = BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_25_6DB_DEFAULT;
	}
	break;

	case BSP_TWL3029_AUDIO_INPUT_PATH_ID_HEADSET_MIC_18DB:
        {           
	    micBias = BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_HEADSET_MIC_18DB_DEFAULT;
	}
	break;
	
	default:
        {
       ; 
        }
    }	

    	
	
   /*  set bias MICBIASSEL*/	 	   	 	   
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL2_MICBIASSEL, &tmpCtrl2RegData,	                             
              (micBias & 1) );            
   /* set bias MICBIASLVL*/
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL2_MICBIASLVL, &tmpCtrl2RegData,	                             
              ((micBias >> 1) & 1) ); 
                    
   /*  now set input */
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL3_INMODE, &tmpCtrl3RegData, inputPathId);
   

   /* now  queue and send the 2 registers  to I2C manager */
   
    /* config callback function struct */
    if (callbackFuncPtr == NULL)
    {
        /*
	 * if no  triton callback is function defined, I2C will function
	 * poll mode
	 */
  
        callbackPtr->callbackFunc = NULL;
        callbackPtr->callbackVal = NULL;  
	callbackPtr->i2cTransArrayPtr = i2cTransArrayPtr;
    }
    
    
    else
    {
        callbackPtr = callbackFuncPtr;
    } 
    /* queue the writes */   

    BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
       BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,tmpCtrl2RegData,regInfoPtr++);
     BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
       BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,tmpCtrl3RegData,regInfoPtr++);
    
    /* reset reg pointer */
    regInfoPtr = regInfo;
     
    /* now request to I2C manager to write to Triton registers */
    returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)2,callbackPtr,
       (BspI2c_TransactionRequest*)callbackPtr->i2cTransArrayPtr);

   return returnVal;
}

/*=============================================================================
 * Description:
 *   For Twl3029 the input and output path may be different unless they 
 *   are loop.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_configureMicBias( BspTwl3029_I2C_CallbackPtr callbackFuncPtr, 
                                     BspTwl3029_Audio_MicBiasId   inputPathId)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData tmpCtrl2RegData = 0;
   BspTwl3029_I2C_RegData tmpRegData = 0;
 
   if (inputPathId > BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_2_5 )
   {
      /* error */
      return BSP_TWL3029_RETURN_CODE_FAILURE;
   }  
   /* read shadow registers for Mic Bias */
   
   BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,
                             &tmpCtrl2RegData);

   tmpRegData = tmpCtrl2RegData;

  /*  set bias MICBIASLVL*/	 	   	 	   
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL2_MICBIASLVL, &tmpCtrl2RegData,	                             
             inputPathId );

   if( tmpRegData == tmpCtrl2RegData)
   {
   	return BSP_TWL3029_RETURN_CODE_SUCCESS;
   }
   else
   {
  
   	BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET, 
   		tmpCtrl2RegData, callbackFuncPtr);
   }

   return returnVal;

   
}
/*=============================================================================
 * Description:
 *   For Twl3029 the input and output path may be different unless they 
 *   are loop.
 */			     
BspTwl3029_ReturnCode 
bspTwl3029_Audio_configureOutputPath( BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                      BspTwl3029_Audio_OutputPathId  outputPathId, 
				      //BspTwl3029_AudioOutputSource audioSource )
				      BspTwl3029_AudioOutputSource* audioSourcePtr )
{   
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
    
    BspTwl3029_I2C_RegData tmpOuten1Data, tmpOuten1Read = 0;
    BspTwl3029_I2C_RegData tmpOuten2Data, tmpOuten2Read = 0;
    BspTwl3029_I2C_RegData tmpOuten3Data, tmpOuten3Read = 0;
    Uint8 outputPathId_bit = 0;

    BspTwl3029_I2C_RegData popmanRead=0,popmanData=0; 
    Uint8  outen2Source;

     BspTwl3029_I2C_CallbackPtr callbackPtr= callbackFuncPtr; 
    
   /* now for out put - Note: loopback ( controlled by CTRL3) remains unchanged */ 
   if (outputPathId > OUTPUT_PATH_MAX )
   {
      /* error */
      returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
      return returnVal;
   } 
       
    tmpOuten1Read = audio_outen_reg_as.outen1;
    tmpOuten2Read = audio_outen_reg_as.outen2;
    tmpOuten3Read = audio_outen_reg_as.outen3;
    

    /*
       returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
       &tmpOuten1Read);
       returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
       &tmpOuten2Read);
       returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
       &tmpOuten3Read);                    
    */



   /* Preserve HSPVMID bit  and clear rest - this switches off all output signals */
   tmpOuten1Data =  tmpOuten1Read;
   // below statement is not required
   // tmpOuten1Data &= 1;
   tmpOuten2Data = tmpOuten2Read;
   tmpOuten3Data = tmpOuten3Read;
   

 
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE;
   if(outputPathId_bit  != 0)			     
   {           
      /*changing the below line as the caller takes care of appropriate arguments    */
      /*outen2Source = BspTwl3029_Audio_fmRemove(*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET));*/
      outen2Source = (*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET));
      BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_OUTEN2_EAR, &tmpOuten2Data,
	        outen2Source );   
   }
  
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE;
   if(outputPathId_bit  != 0)	
   {           
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOR, &tmpOuten1Data,
	        *(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET));    
   }
   
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE;   
   if(outputPathId_bit  != 0)	
   {           
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOL, &tmpOuten1Data,
	        *(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET) );       
   }
   
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX;
   if(outputPathId_bit  != 0)
   {           
      /*changing the below line as the caller takes care of appropriate arguments    */
      /*outen2Source = BspTwl3029_Audio_fmRemove(*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET));*/
      outen2Source = (*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET));
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN2_AUXO, &tmpOuten2Data,
	        outen2Source );  
   }
   
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT;
   if(outputPathId_bit  != 0)
   {  	     
      /*changing the below line as the caller takes care of appropriate arguments    */
      /*outen2Source = BspTwl3029_Audio_fmRemove(*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET));*/
      outen2Source = (*(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET));
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN2_CARKIT, &tmpOuten2Data,
	        outen2Source );   
   }
   
   /* for SPEAKER there are two alternatives: either with D Class
       SPK enabled OR D Class SPK disabled 
    */
   outputPathId_bit = outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK;
   if(outputPathId_bit  != 0)
   {  	     
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN3_SPK, &tmpOuten3Data,
	         *(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET));   
   }

   outputPathId_bit = outputPathId &  BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS;
   if ( outputPathId_bit != 0)
   {
      /* first switch on Speaker */
	
      BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN3_SPK, &tmpOuten3Data,
                                  *(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET));
      /* then  switch on D CLASS  (but  only if input source is not POWERDWN) */
      if ( *(audioSourcePtr + BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET) != 0 )
      {
	    audio_outen_reg_as.classD=0x01;

	    /*		The configuration of Class D including Hftest and CTRL3 along with orther outen registers
		 * 	is done in L1audio_sync.c file OMAPS00089393
		 */
	}  

    }
    else 
    {

        audio_outen_reg_as.classD=0x00;
    }



    audio_outen_reg_as.outen1 = tmpOuten1Data;
    audio_outen_reg_as.outen2 = tmpOuten2Data;
    audio_outen_reg_as.outen3 = tmpOuten3Data;

    /* now check for necessary POP mods */
    bspTwl3029_Audio_popEnable(&popmanData);

    BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
            &popmanRead); 

    if(popmanRead!=popmanData){ 

        BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET, 
                popmanData, callbackFuncPtr);
    }
   return returnVal;

			     
   }

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_gainControl(   BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                                BspTwl3029_Audio_GainSources    gainSrc,
                                BspTwl3029_Audio_GainSettings* newGainSettings)
{          
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
        

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[10] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
    
    /* I2C callback function  info pointer*/
    BspTwl3029_I2C_CallbackPtr callbackPtr= callbackFuncPtr; 
    

    BspTwl3029_Audio_GainSources source = gainSrc;
    
    BspTwl3029_AudioRegFieldInfo regFieldInfo;
    Uint8 count = 0;
    int i;

    for (i = 0; i < 10; i++)
    {
       regInfo[i].regDataPtr = NULL;
    }
    
    /*  check validity of gainScr value */
    if ( source == 0 )
    {
       /* no gain updates specified */
       return returnVal;
    }
    /* config callback function struct */
    if (callbackFuncPtr != NULL)
    { 
        callbackPtr = callbackFuncPtr;
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    } 
    /* queue the writes */
    /* now  check which gain values need to be set */
    
    source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP;
   if(source  != 0)
   {
			
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKG_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKG_WIDTH;
      
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->speakerAmpGain,
				&regFieldInfo,
				&count);

   }
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_VUL;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_VULGAIN_VULPG_OFFSET;
      /* mute bit ( DXEN bit 5) and gain bits ( bits 0 - 4) are set with single write to register (VULGAIN) */
      regFieldInfo.regBitWidth = ( BSP_TWL3029_LLIF_AUDIO_VULGAIN_VULPG_WIDTH + BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_WIDTH);
      
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->vulGain,
				&regFieldInfo,
				&count);
   
   }
   
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL;
   if(source  != 0)
   {
      /* vdl gain register consist of 2 field, one for fine gain andthe other for course
	 * gain, however here, the two fields are updated together 
	 */
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_VDLGAIN_OFFSET;
      regFieldInfo.regBitOffset = 0;
      regFieldInfo.regBitWidth = 8;
      
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->vdlGain,
				&regFieldInfo,
				&count);
   }
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SIDETONE;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_SIDETONE_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_SIDETONE_VST_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_SIDETONE_VST_WIDTH;
      
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->sidetoneLevel,
				&regFieldInfo,
				&count);
   }
   
   /*  other gains ( stereo and FM )  */
   
     
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT;   
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_AUDRGAIN_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_AUDRGAIN_AURGA_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_AUDRGAIN_AURGA_WIDTH;
      
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->stereoVolRight,
				&regFieldInfo,
				&count);
   }
   
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_AUDLGAIN_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_AUDLGAIN_AULGA_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_AUDLGAIN_AULGA_WIDTH;
  
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->stereoVolLeft,
				&regFieldInfo,
				&count);      
   }
   
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SOFTVOL;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_CTRL4_BYPSFTVOL_OFFSET;
       /*   bypass bit (bit 0 )  and the gain setting itself are both set with a single write to the register (CTRL4)*/
      regFieldInfo.regBitWidth = ( BSP_TWL3029_LLIF_AUDIO_CTRL4_BYPSFTVOL_WIDTH + BSP_TWL3029_LLIF_AUDIO_CTRL4_SFTVOL_WIDTH) ;

       
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->stereoSoftVol,
				&regFieldInfo,
				&count);     
   }
   
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_RADIO;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_CTRL2_FMG_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_CTRL2_FMG_WIDTH;
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->fmRadioGain,
				&regFieldInfo,
				&count);     
   }
  
   source = gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN;
   if(source  != 0)
   {
      regFieldInfo.registerId = BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET;
      regFieldInfo.regBitOffset = BSP_TWL3029_LLIF_AUDIO_CTRL5_EARG_OFFSET;
      regFieldInfo.regBitWidth = BSP_TWL3029_LLIF_AUDIO_CTRL5_EARG_WIDTH;
      bspTwl3029_AudioSetGain( &regInfoPtr,
                                newGainSettings->earGain,
				&regFieldInfo,
				&count);     
   }
    regInfoPtr = regInfo;
    /* now request to I2C manager to write to Triton registers */
    returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackPtr,
       (BspI2c_TransactionRequest*)i2cTransArrayPtr);

   return returnVal;
}

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_gainVDLConfig( BspTwl3029_I2C_CallbackPtr    callbackFuncPtr,
                                BspTwl3029_Audio_GainVDLSources    gainSrc,
                                BspTwl3029_Audio_GainVDLSettings* newGainSettings)
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
	BspTwl3029_I2C_RegData tmpVdlgainRegRead, tmpStore;

	   /* get current register configuration */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_VDLGAIN_OFFSET,
                             &tmpVdlgainRegRead);

   if( BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_COURSE == gainSrc )
   {
   	   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_VDLGAIN_VOLCTL, \
	   	&tmpVdlgainRegRead,newGainSettings->vdlCourseGain);
   }
   else if (BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_FINE == gainSrc )
   {
   	   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_VDLGAIN_VDLPG, \
	   	&tmpVdlgainRegRead,newGainSettings->vdlFineGain);       
   }
   else
   {
   	   return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   	
   returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VDLGAIN_OFFSET,
                                   tmpVdlgainRegRead,  callbackFuncPtr);

   return returnVal;
	
}

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Audio_setFilterState (BspTwl3029_I2C_CallbackPtr  callbackPtr,
                                  BspTwl3029_Audio_Filter      filter,
                                  BspTwl3029_Audio_FilterState filterState)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    
   BspTwl3029_I2C_RegData tmpCtrl1RegData, tmpStore;
   
   /*  check validity of args */
   if ((filter > 0x3) ||
        (filterState > BSP_TWL3029_AUDIO_FILTER_BYPASS))
   {
       return returnVal;
   }	 
   
    /*  now check filter settings  */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,
                             &tmpCtrl1RegData);
   
   tmpStore = tmpCtrl1RegData;
  
  if (( filter & BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH) > 0)
  {
     /*  update VDLFBYP bit ( bit 5 of CTRL1 reg */
      BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VDLFBYP, &tmpStore,filterState);
  }
  if (( filter & BSP_TWL3029_AUDIO_FILTER_VOICE_UPLINK_HIGH_PASS) > 0)
  {
     /*  update VDLFBYP bit ( bit 5 of CTRL1 reg */
      BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VULHPFBYP, &tmpStore,filterState);
  }
  
  /* now update register ( if required) */
  if (tmpStore == tmpCtrl1RegData)
  {
     returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
     
  }
  else
  {
     returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,
                                   tmpStore,  callbackPtr);
  }			     
   return returnVal;
}				  

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_voiceCodecConfigure(BspTwl3029_I2C_CallbackPtr  callbackFuncPtr,                                      
                                     BspTwl3029_AudioVoicePllMode       pll,                                     
                                     BspTwl3029_AudioVspClkFreq         vspClkFreq,                                     
                                     BspTwl3029_AudioVspClkMode         vspClkMode,                                    
                                     BspTwl3029_VoiceCodecPathBandWidth bandwidth )
{  
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
        

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[6] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
    
    /* I2C callback function info pointer */
    
    BspTwl3029_I2C_CallbackPtr callbackPtr= callbackFuncPtr; 
      
    Uint8 count = 0;
    
    BspTwl3029_I2C_RegData tmpCtrl1RegData,tmpCtrl6RegData;
    BspTwl3029_I2C_RegData tmpCtrl1RegRead,tmpCtrl6RegRead;
    count = 0;
    
    /*  check validity of args */
    if(( pll > BSP_TWL3029_AUDIO_PLL_MAX )||
         (vspClkFreq > BSP_TWL3029_AUDIO_VCLK_FREQ_1_OR_2MHZ) ||
	 (vspClkMode > BSP_TWL3029_AUDIO_VCLK_MODE_CONTINUOUS) ||
	 (bandwidth > BSP_TWL3029_AUDIO_VOICE_WIDEBAND))	
    {
       /* invalid args */
       return returnVal;
    }
    
    /*
     * when no  triton callback function is defined,  I2C will function poll mode
     */
    if (callbackPtr != NULL)
    {
	/* select the I2C struct, the pointer this is in the callback struct */
	i2cTransArrayPtr = callbackPtr->i2cTransArrayPtr;
    }   
    
   
   /* get current register configuration */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET,
                             &tmpCtrl6RegRead);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,
                             &tmpCtrl1RegRead);
   
   tmpCtrl6RegData =  tmpCtrl6RegRead;
   tmpCtrl1RegData =  tmpCtrl1RegRead;
    
    /*  now check PLL bits  */   
   /*  set VPLLON  ( lsb of pll)*/
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL6_VPLLON, &tmpCtrl6RegData,(pll & 0x01));
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL6_VPLLPCHGZ, &tmpCtrl6RegData, ((pll & 0x04)>> 2) );
 
   if ((tmpCtrl6RegData != tmpCtrl6RegRead)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       /*update triton CTRL6 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET, tmpCtrl6RegData,regInfoPtr++);	
	   count++;	
   }
   
   /* now update vspClkFreq */
   
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VSPCK, &tmpCtrl1RegData,
                               vspClkFreq );
   
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VCLKMODE, &tmpCtrl1RegData,
                               vspClkMode );
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_WBA, &tmpCtrl1RegData,
                               bandwidth );			       
   if ((tmpCtrl1RegData != tmpCtrl1RegRead) && (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       /*update triton CTRL6 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET, tmpCtrl1RegData,regInfoPtr++);	
	   count++;	
   }			       

   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }			       
   return returnVal;
}

/*=============================================================================
 * Description:
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoDacConfigure(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                    BspTwl3029_Audio_StereoPllMode       pll,
                                    BspTwl3029_Audio2MonoState           mono,
                                    BspTwl3029_Audio_StereoSamplingRate  samplingRate )
                                                                        
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;
    
    BspTwl3029_I2C_RegData tmpCtrl4RegData,tmpCtrl5RegData,tmpCtrl6RegData, tmpPwrStatus= 0;
    BspTwl3029_I2C_RegData tmpStore6,  tmpStore4,  tmpStore5 = 0;
    count = 0;
       
    /*  check validity of args */
    
    /* config callback function struct */
    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
    
    
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &tmpCtrl5RegData);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET,
                             &tmpCtrl6RegData);
   
   /* determine if stereo is powered on by checking the shadow PWERONSTATUS register */
   /* shadow PWERONSTATUS reg is updated when TOGB is wriiten to in bspTwl3029_Audio_powerControl function */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &tmpPwrStatus);
   
   /* STON bit is only bit of intersest in this register */
   tmpPwrStatus = BSP_TWL3029_LLIF_GET_FIELD ( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_STON, &tmpPwrStatus);
                                  
     
   /* only update sampling frequency and stereo2mono if STON off */
   
   /*  stereo2mono */
   tmpStore4= tmpCtrl4RegData;
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOR, &tmpStore4, (mono &1) );
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOL, &tmpStore4, ((mono & 2)>>1) );
   
   
   /* sampling frequency */ 
   tmpStore5= tmpCtrl5RegData;
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL5_SRW, &tmpStore5, samplingRate );
   
   
   
   if ((tmpStore4 != tmpCtrl4RegData)||(tmpStore5 != tmpCtrl5RegData))
   {
       /* temporarily switch off stereo power if either frequency or mono need to me modified */
	/* error, mono and frequency should not be modified if stereo power is on 
	 */
       if (tmpPwrStatus != 0)
       { 
          returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;	  
       } 

   
      if ((tmpStore4 != tmpCtrl4RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
      {       
          /*update triton CTRL4 reg */
          returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpStore4,regInfoPtr++);	
          count++;	
      }
   
   
      if ((tmpStore5 != tmpCtrl5RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
      {       
          /*update triton CTRL5 reg */
          returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, tmpStore5,regInfoPtr++);	
          count++;	
      }
   } 
   
     
   /* now configure pll */
   tmpStore6= tmpCtrl6RegData;
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
   
   
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_WIDTH );
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
    BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_WIDTH );
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_WIDTH ); 
   
   if ((tmpStore6 != tmpCtrl6RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       /*update triton CTRL6 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET, tmpStore6,regInfoPtr++);	
       count++;	
   }
 
   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);

   }	

   
   return returnVal;
}				    


//Sundi: add code

BspTwl3029_ReturnCode
bspTwl3029_Audio_Configure_Stereopath(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                    BspTwl3029_Audio_StereoPllMode       pll,
                                    BspTwl3029_Audio2MonoState           mono,
                                    BspTwl3029_Audio_StereoSamplingRate  samplingRate )
                                                                        
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;
    
    BspTwl3029_I2C_RegData tmpCtrl4RegData,tmpCtrl5RegData,tmpCtrl6RegData, tmpPwrStatus= 0;
	// added the register value for OUTEN1
    BspTwl3029_I2C_RegData tmpOuten1RegData;
	//Added the tmp value for OUTEN2
    BspTwl3029_I2C_RegData tmpStore6,  tmpStore4,  tmpStore5, tmpStore1 = 0;
    count = 0;
       
    /*  check validity of args */
    
    /* config callback function struct */

    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }

    
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &tmpCtrl5RegData);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET,
                             &tmpCtrl6RegData);
   
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                             &tmpOuten1RegData);
   /* determine if stereo is powered on by checking the shadow PWERONSTATUS register */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &tmpPwrStatus);
   
   
                                  
     
   /* only update sampling frequency and stereo2mono if STON off */
   
   /*  stereo2mono */
   tmpStore4= tmpCtrl4RegData;
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOR, &tmpStore4, (mono &1) );
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOL, &tmpStore4, ((mono & 2)>>1) );
   
   
   /* sampling frequency */ 
   tmpStore5= tmpCtrl5RegData;
#if (ANLG_PG == 2)
   tmpStore5 |= 0x40;
#endif
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL5_SRW, &tmpStore5, samplingRate );
   
   
   
   if ((tmpStore4 != tmpCtrl4RegData)||(tmpStore5 != tmpCtrl5RegData))
   {
         
      if ((tmpStore4 != tmpCtrl4RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
      {       
          /*update triton CTRL4 reg */
          returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpStore4,regInfoPtr++);	
          count++;	
      }
   
   
      if ((tmpStore5 != tmpCtrl5RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
      {       
          /*update triton CTRL5 reg */
          returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, tmpStore5,regInfoPtr++);	
          count++;	
      }
   } 
   
     
   /* now configure pll */
   tmpStore6= tmpCtrl6RegData;
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
   
   
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLON_WIDTH );
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
    BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLPCHGZ_WIDTH );
    /*  set VPPCHGZ ( 2nd lsb of pll)*/
   BSPUTIL_BITUTIL_BIT_FIELD_SET( &tmpStore6,
                                       ((pll >> BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_OFFSET) & 1),
                                       BSPUTIL_BITUTIL_DATAUNIT_8,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_OFFSET,
                                       BSP_TWL3029_LLIF_AUDIO_CTRL6_STPLLSPEEDUP_WIDTH ); 
   
   if ((tmpStore6 != tmpCtrl6RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       /*update triton CTRL6 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET, tmpStore6,regInfoPtr++);	
       count++;	
   }

#if (OP_L1_STANDALONE == 1)

   tmpStore1 = 0x24; // HSOR and HSOL set to audio path

   if ((tmpStore1 != tmpOuten1RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       /*update triton OUTEN1 reg */
       returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET, tmpStore1,regInfoPtr++);	
       count++;	
   }

 #endif
 
   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }	

   
   return returnVal;
}


BspTwl3029_ReturnCode
bspTwl3029_Audio_Start_Stereopath(  BspTwl3029_I2C_CallbackPtr           callbackFuncPtr
                                    )
                                                                        
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[8] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;

    //Added the tmp value for CTRL4 and TOGB registers
    BspTwl3029_I2C_RegData  tmpCtrl4RegData,tmpTOGBRegData = 0,popmanData=0,popmanRead=0;

    /* config callback function struct */

    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }

 /* now check for necessary POP mods */
    bspTwl3029_Audio_popEnable(&popmanData);

    BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
            &popmanRead); 

    if(popmanRead!=popmanData){ 

        BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET, popmanData,regInfoPtr++);
        count++;    
    }
       
	//  Read the CTRL4 register
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);

/*Make I2SON and AUGA bits 0*/    
tmpCtrl4RegData = ((tmpCtrl4RegData) & (~(0x48)));
	//Set the I2SON bit , AUGA on
    tmpCtrl4RegData  |= 0x48;
	//make a queue write back
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpCtrl4RegData,regInfoPtr++);	
    count++;	

    //Sundi: Read the TOGB register
	returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                             &tmpTOGBRegData);
	//Set the set STS bit
    tmpTOGBRegData  = 0x20;
	//make a queue write back
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET, tmpTOGBRegData,regInfoPtr++);	
    count++;

   
 
   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }	

   
   return returnVal;
}	


BspTwl3029_ReturnCode
bspTwl3029_Audio_Stop_Stereopath(  BspTwl3029_I2C_CallbackPtr           callbackFuncPtr, 
                                     BspTwl3029_I2C_RegData outen_control)
                                                                        
{

    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;

   /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
    
    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[14] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
            
    Uint8 count = 0;
    Uint8 classD=0;
    //Added the tmp value for CTRL4,CTRL6 TOGB registers
    BspTwl3029_I2C_RegData  tmpCtrl4RegData, tmpCtrl6RegData,tmpTOGBRegData = 0;
    BspTwl3029_I2C_RegData tmpctrl3=0;

	/* config callback function struct */

    if (callbackFuncPtr != NULL)
    {
        i2cTransArrayPtr = (Bsp_Twl3029_I2cTransReqArrayPtr)callbackFuncPtr->i2cTransArrayPtr;
    }
    
    count = 0;
       
	//Read the CTRL4 register
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);
	//Set the I2SON bit to 0
      tmpCtrl4RegData &= 0xB7;
	//make a queue write back
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpCtrl4RegData,regInfoPtr++);	
    count++;	

    // Read the CTRL6 register
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET,
                             &tmpCtrl6RegData);
	//Set the STPLLON bit to 0
    tmpCtrl6RegData &= 0xFD;
	//make a queue write back
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL6_OFFSET, tmpCtrl6RegData,regInfoPtr++);	
    count++;

   // clear the OUTEN Registers only if the downlink voice path is off
    bspTwl3029_Audio_getClassD_mode(&classD);
   if(outen_control == OUTEN_DISABLE)
   {
      returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
          BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT,  regInfoPtr++);
      count++;
      returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
          BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT,  regInfoPtr++);
      count++;

	/*OMAPS00089393*/
	if(classD==0x01)
	{
	    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
		    BSP_TWL_3029_MAP_AUDIO_HFTEST1_OFFSET,0x00,regInfoPtr++);
	    count++;

	    BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
		    &tmpctrl3);

	    tmpctrl3&=0x7F;

	    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
		    BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,tmpctrl3,regInfoPtr++);
	    count++;

	}
	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
		BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT,  regInfoPtr++);
	count++;

	if(classD==0x01)
	{	

	    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_CKG,
		    BSP_TWL_3029_MAP_CKG_TESTUNLOCK_OFFSET,0x00,regInfoPtr++);
	    count++;


	}

   }

    // Read the TOGB register
	//returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
           //                  &tmpTOGBRegData);
	//Set the  STR bit to 0 .  Also set the TOGB register to its default value for Power Mgmt
           // tmpTOGBRegData  = BSP_TWL_3029_MAP_AUDIO_TOGB_DEFAULT;

	tmpTOGBRegData  = 0x10;
	//make a queue write back
    returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET, tmpTOGBRegData,regInfoPtr++);	
    count++;

   
 
   /* now request to I2C manager to write to Triton registers */
   if ((count > 0 ) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
       returnVal = BspTwl3029_I2c_regInfoSend(regInfo,(Uint16)count,callbackFuncPtr,
           (BspI2c_TransactionRequest*)i2cTransArrayPtr);	
   }	

   
   return returnVal;
}	



/*=============================================================================
 * Description: 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoSamplingRateConfigure (BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_Audio_StereoSamplingRate  samplingRate )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */

   BspTwl3029_I2C_RegData tmpCtrl5RegData, tmpPwrStatus;
   BspTwl3029_I2C_RegData tmpStore5;

   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &tmpCtrl5RegData);

   /* determine if stereo is powered on by checking the shadow PWERONSTATUS register */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &tmpPwrStatus);

   /* move  STON bit to lsb  ( it is the only bit of interest in this function)*/    
   tmpPwrStatus = tmpPwrStatus >> BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_STON_OFFSET ;

   /* sampling frequency */ 
   tmpStore5= tmpCtrl5RegData;
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL5_SRW, &tmpStore5, samplingRate );

   if (tmpStore5 != tmpCtrl5RegData)
   {
       /* temporarily switch off stereo power if either frequency or mono need to be modified */
       /* error, mono and frequency should not be modified if stereo power is on */
       if (tmpPwrStatus != 0)
       { 
          returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;	  
       }

	   returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD, 
	   						BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, 
					   		tmpStore5, 
					   		callbackFuncPtr);
   	}
   else
   	returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;

   return returnVal;
}

/*=============================================================================
 * Description: 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_stereoMonoConfigure (BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_Audio2MonoState           mono )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */

   BspTwl3029_I2C_RegData tmpCtrl4RegData, tmpPwrStatus;
   BspTwl3029_I2C_RegData tmpStore4;

   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);

      /* determine if stereo is powered on by checking the shadow PWERONSTATUS register */                                                                                                                      
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &tmpPwrStatus);
   
   /* move  STON bit to lsb  ( it is the only bit of interest in this function)*/    
   tmpPwrStatus = tmpPwrStatus >> BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_STON_OFFSET ;

   /* only update sampling frequency and stereo2mono if STON off */
   
   /*  stereo2mono */
   tmpStore4= tmpCtrl4RegData;
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOR, &tmpStore4, (mono &1) );
   BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_MONOL, &tmpStore4, ((mono & 2)>>1) );
   
   if (tmpStore4 != tmpCtrl4RegData)
   {
       /* temporarily switch off stereo power if either frequency or mono need to me modified */
       /* error, mono and frequency should not be modified if stereo power is on */
       if (tmpPwrStatus != 0)
       { 
          returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;	  
       } 

   
      if ((tmpStore4 != tmpCtrl4RegData)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
      {       
          /*update triton CTRL4 reg */
          returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,
          					BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, 
          					tmpStore4,
          					callbackFuncPtr);	
          	
      }
   	}
    else
    {
   		returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
    }

   return returnVal;

                                  

}

/*=============================================================================
 * Description:
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_vmemoConfigure(BspTwl3029_I2C_CallbackPtr           callbackFuncPtr,
                                BspTwl3029_AudioI2sVoiceUplinkState  i2sOn )
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;    
    BspTwl3029_I2C_RegData tmpCtrl4RegData = 0;
    BspTwl3029_I2C_RegData tmpStore4 = 0;
    
   /* now configure for VMEMO */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);
   tmpStore4 = tmpCtrl4RegData;			     
   
   /* note VMEMO/I2SON  can only be activated if STON pwer bit (PWRONSTATUS reg) is already on */  
   /* however the register itself  can be programmed reqardless if STON status */                                                                                                                      
   
   
   /*  set I2SON bit ( reg CTRL4 bit 6) */
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL4_I2SON, &tmpStore4,(i2sOn & 1));
   /*  set VMEMO  bit ( reg CTRL4 bit 7) */
   BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL4_VMEMO, &tmpStore4,((i2sOn & 2)>>1));
   if ((tmpStore4 != tmpCtrl4RegData)||(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
      /*update triton CTRL4 reg */
      returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET, tmpStore4,callbackFuncPtr);		
   }
   

   return returnVal;
}				

/*=============================================================================
 * Description:
 */

BspTwl3029_ReturnCode
bspTwl3029_Audio_popPrecharge(BspTwl3029_I2C_CallbackPtr   callbackFuncPtr,
                                BspTwl3029_AudioPopSource  popOutput,
				BspTwl3029_AudioPopPrechargeEnable chgEnable )
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;    
    BspTwl3029_I2C_RegData tmpPopmanRead, tmpPopmanData = 0;
   
    BspTwl3029_I2C_RegData tmpPwrStatusRead = 0;
           
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                             &tmpPopmanRead);
   tmpPopmanData = tmpPopmanRead;

   /* need to know if which power sources are enabled */			                                                                                                                          
   returnVal = BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET, 
                             &tmpPwrStatusRead);
   
   if (( tmpPwrStatusRead == 0 ) && ( chgEnable == BSP_TWL3029_AUDIO_POP_CHG_DISABLE ))
   {
     /* error. the precharge capacitor shoud be active when power is off. then when power is switched
	 * on the pre charger capacitor will get charged.  the capacitor should be allowed enough time to 
	 * charge ( 15ms ) before the output is activated
     */	
      return returnVal; 
   }
   
   if (popOutput &  BSP_TWL3029_AUDIO_POP_HSO)
   { 
        BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_HSOCHG, &tmpPopmanData, chgEnable);
	/* make sure post discharged is enabled this  should ALWAYs be enabled  */
	/* check status of xDHG register  = It must have prevoisly been enabled*/
	if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_HSODHG, &tmpPopmanData) == 0)
	{
	   /* this function should only be used if anti POP is required. Check the status of xDHG register. 
	     * Thisneeds to be enabled for anti pop to work 
	     * If not enabled return error. It is up to the calling function to enable anti POP - not 
	     * this function
           */ 
	   returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
	}
   }
   
   if ((popOutput &  BSP_TWL3029_AUDIO_POP_EAR) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   { 
        BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_EARCHG, &tmpPopmanData, chgEnable);
	/* check status of xDHG register  = It must have previously been enabled*/
	if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_EARDHG, &tmpPopmanData) == 0)
	{
	   returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
	}
   }
   if ((popOutput &  BSP_TWL3029_AUDIO_POP_AUXO) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   { 
        BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_AUXOCHG, &tmpPopmanData, chgEnable);
	/* check status of xDHG register  = It must have previously been enabled*/
	if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_AUXODHG, &tmpPopmanData) == 0)
	{
	   returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
	}
	
   }
   if ((popOutput &  BSP_TWL3029_AUDIO_POP_CARKIT) &&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   { 
        BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_CARKITCHG, &tmpPopmanData, chgEnable);
	/* check status of xDHG register  = It must have previously been enabled*/
	if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_POPMAN_CARKITDHG, &tmpPopmanData) == 0)
	{
	   returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
	}
	
   }	
  
   if ((tmpPopmanData!= tmpPopmanRead)&&(returnVal != BSP_TWL3029_RETURN_CODE_FAILURE))
   {
      /*update POPMAN reg */
	/*returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET, tmpPopmanData,callbackFuncPtr); */		
    
   }

   return returnVal;
}				
/*=============================================================================
 * Description:
 */

    BspTwl3029_ReturnCode
bspTwl3029_Audio_popEnable(BspTwl3029_I2C_RegData *popEnable )
{
    BspTwl3029_I2C_RegData popmanData=0,tmpOuten1Data=0,tmpOuten2Data=0;

    tmpOuten1Data=audio_outen_reg_as.outen1;
    tmpOuten2Data=audio_outen_reg_as.outen2;



    if ((BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOL, &tmpOuten1Data) != 0) ||
            (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOR, &tmpOuten1Data) != 0))
    {      
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_POPMAN_HSOCHG, &popmanData, 1);
    }

    /* earphone */ 
    if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_OUTEN2_EAR, &tmpOuten2Data) != 0)
    {
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_POPMAN_EARCHG, &popmanData, 1);
    }

    /* aux */
    if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_OUTEN2_AUXO, &tmpOuten2Data) != 0)
    {
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_POPMAN_AUXOCHG, &popmanData, 1);
    }

    /* carkit */
    if (BSP_TWL3029_LLIF_GET_FIELD (BSP_TWL3029_LLIF_AUDIO_OUTEN2_CARKIT, &tmpOuten2Data) != 0)
    {
        BSP_TWL3029_LLIF_SET_FIELD(BSP_TWL3029_LLIF_AUDIO_POPMAN_CARKITCHG, &popmanData, 1);
    }

    *popEnable=popmanData;
    return BSP_TWL3029_RETURN_CODE_SUCCESS;
}


  /*=============================================================================
 * Description:
 *   This returns the Input path configurations.
 */
 BspTwl3029_ReturnCode
  bspTwl3029_Audio_setClassD_mode(Uint8 OnOff)
  {
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData tmpClassDmodeRead, tmpClassDmodeData = 0;
	
    /* Read CTRL3 register store it in a temp var.*/
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
                             &tmpClassDmodeRead);

    tmpClassDmodeData = tmpClassDmodeRead;
	
    /* Based on the value of OnOff, set or reset bit 7 of the temp var.*/
    if(1 == OnOff)
    {
      BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKDIGON, &tmpClassDmodeData, 1);
    }
    else
    {
      BSP_TWL3029_LLIF_SET_FIELD (BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKDIGON, &tmpClassDmodeData, 0);
    }

    /* Write the temp var into the CTRL3 register.*/
      if(tmpClassDmodeData != tmpClassDmodeRead)
      	{
         /*update POPMAN reg */
         returnVal = BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET, tmpClassDmodeData,NULL);      	
      	}

     return returnVal;
  }



#ifdef TI_BSP_AUDIO_CONFIG_READ

/*=============================================================================
 * Description:
 *   This returns the Input path configurations.
 */
BspTwl3029_ReturnCode  
bspTwl3029_Audio_getInputPath( BspTwl3029_Audio_InputPathId* inputPath )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegData inmodeReg;

   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
                             &inmodeReg);

   if( BSP_TWL3029_RETURN_CODE_FAILURE == returnVal)
   	return BSP_TWL3029_RETURN_CODE_FAILURE;

   /*  now get input path */
   *inputPath = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL3_INMODE, &inmodeReg);

   return returnVal;
}

/*=============================================================================
 * Description:
 *   This gives the gain value for different sources.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getGain (   BspTwl3029_Audio_GainSources    gainSrc,
                             BspTwl3029_Audio_GainSettings* newGainSettings)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   BspTwl3029_I2C_RegData regValue;

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
                             &regValue);
	  
	  newGainSettings->speakerAmpGain = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKG, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_VUL)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                             &regValue);

	  if( regValue & (1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET))
	  	newGainSettings->vulGain = BSP_TWL3029_AUDIO_GAIN_DB_MUTE;
	  else
	  	newGainSettings->vulGain = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_VULGAIN_VULPG, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SIDETONE)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_SIDETONE_OFFSET,
                             &regValue);
	  
	  newGainSettings->sidetoneLevel = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_SIDETONE_VST, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_AUDRGAIN_OFFSET,
                             &regValue);
	  
	  newGainSettings->stereoVolRight = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_AUDRGAIN_AURGA, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_AUDLGAIN_OFFSET,
                             &regValue);
	  
	  newGainSettings->stereoVolLeft = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_AUDLGAIN_AULGA, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_SOFTVOL)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &regValue);
	  
	  newGainSettings->stereoSoftVol = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL4_BYPSFTVOL, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_RADIO)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,
                             &regValue);
	  
	  newGainSettings->fmRadioGain = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL2_FMG, &regValue);
   }

   if( gainSrc & BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN)
   {
      returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &regValue);
	  
	  newGainSettings->earGain = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL5_EARG, &regValue);
   }

   return returnVal;
}

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getVDLGain( BspTwl3029_Audio_GainVDLSources    gainSrc,
                             BspTwl3029_Audio_GainVDLSettings* newGainSettings)
{
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
	BspTwl3029_I2C_RegData tmpVdlgainRegRead;

	   /* get current register configuration */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_VDLGAIN_OFFSET,
                             &tmpVdlgainRegRead);

   if( BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_COURSE == gainSrc )
   {
   	   newGainSettings->vdlCourseGain = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_VDLGAIN_VOLCTL, \
	   														&tmpVdlgainRegRead);
   }
   else if (BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_FINE == gainSrc )
   {
   	   newGainSettings->vdlFineGain = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_VDLGAIN_VDLPG, \
														   	&tmpVdlgainRegRead);       
   }
   else
   {
   	   return BSP_TWL3029_RETURN_CODE_FAILURE;
   }
   	
   return returnVal;
	
}

/*=============================================================================
 * Description:
 *   Get the output Mic Bias configurtion
 *   are loop.
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getMicBias( BspTwl3029_Audio_MicBiasId * inputPathId)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData tmpCtrl2RegData = 0;
 
   /* read shadow registers for Mic Bias */
   if ( BSP_TWL3029_RETURN_CODE_FAILURE == BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL2_OFFSET,
                             &tmpCtrl2RegData))
   	{
    	return BSP_TWL3029_RETURN_CODE_FAILURE;
   	}

   /*  set bias MICBIASLVL*/	 	   	 	   
   *inputPathId = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL2_MICBIASLVL, &tmpCtrl2RegData);

   return returnVal;

   
}

/*=============================================================================
 * Description:
 *   For Twl3029 the input and output path may be different unless they 
 *   are loop.
 *
 *   audioSourcePtr needs to be of BSP_TWL3029_AUDIO_OUTPUT_SOURCE_MAX_SIZE   
 *=============================================================================   
 */			     
BspTwl3029_ReturnCode 
bspTwl3029_Audio_getOutputPath( BspTwl3029_Audio_OutputPathId*  outputPathId, 
								BspTwl3029_AudioOutputSource* audioSourcePtr )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_SUCCESS;
   BspTwl3029_I2C_RegData tmpOuten1Read;
   BspTwl3029_I2C_RegData tmpOuten2Read;
   BspTwl3029_I2C_RegData tmpOuten3Read, tmpRead;
   BspTwl3029_Audio_OutputPathId tmpPathId = 0;


   tmpOuten1Read = audio_outen_reg_as.outen1;
   tmpOuten2Read = audio_outen_reg_as.outen2;
   tmpOuten3Read = audio_outen_reg_as.outen3;

   /*
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                             &tmpOuten1Read);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                             &tmpOuten2Read);
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
                             &tmpOuten3Read);
  */

   
   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;   
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_OUTEN2_EAR, &tmpOuten2Read);
   if( tmpRead )
   {
   		tmpPathId = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE;
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] = tmpRead;
   }

   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;   
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOR, &tmpOuten1Read);      
   if(tmpRead)	
   {           
   		tmpPathId |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE;     
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = tmpRead;
   }

   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN1_HSOL, &tmpOuten1Read);      
   if(tmpRead)	
   {           
   		tmpPathId |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE;     
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = tmpRead;      
   }


   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;   
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN2_AUXO, &tmpOuten2Read);
   if(tmpRead)
   {           
   		tmpPathId |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX;     
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] = tmpRead;            
   }

   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;   
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN2_CARKIT, &tmpOuten2Read);   
   if(tmpRead)
   {           
   		tmpPathId |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT;     
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] = tmpRead;                  
   }   
   
   audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;   
   tmpRead = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_OUTEN3_SPK, &tmpOuten3Read);   
   if(tmpRead)
   {           
   		tmpPathId |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK;     
      audioSourcePtr[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = tmpRead;                  
   }

   *outputPathId = tmpPathId;
   return returnVal;	
}

/*=============================================================================
 * Description:
 */
BspTwl3029_ReturnCode 
bspTwl3029_Audio_getFilterState (BspTwl3029_Audio_Filter      filter,
                                  BspTwl3029_Audio_FilterState *filterState)
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    
   BspTwl3029_I2C_RegData tmpCtrl1RegData;
   
   /*  check validity of args */
   if ((filter > 0x3))
   {
       return returnVal;
   }	 
   
    /*  now check filter settings  */
   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL1_OFFSET,
                             &tmpCtrl1RegData);
  
  if (( filter & BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH) > 0)
  {
     /*  get VDLFBYP bit ( bit 5 of CTRL1 reg */
      *filterState = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VDLFBYP, &tmpCtrl1RegData);
  }
  if (( filter & BSP_TWL3029_AUDIO_FILTER_VOICE_UPLINK_HIGH_PASS) > 0)
  {
     /*  get VDLFBYP bit ( bit 5 of CTRL1 reg */
      *filterState = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL1_VULHPFBYP, &tmpCtrl1RegData);
  }
  			     
   return returnVal;
}

/*=============================================================================
 * Description: 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getStereoSamplingRate (BspTwl3029_Audio_StereoSamplingRate  *samplingRate )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */

   BspTwl3029_I2C_RegData tmpCtrl5RegData;

   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET,
                             &tmpCtrl5RegData);

   *samplingRate = BSP_TWL3029_LLIF_GET_FIELD( BSP_TWL3029_LLIF_AUDIO_CTRL5_SRW, &tmpCtrl5RegData);

   return returnVal;
}

/*=============================================================================
 * Description: 
 */
BspTwl3029_ReturnCode
bspTwl3029_Audio_getStereoMonoConfigData ( BspTwl3029_Audio2MonoState           *mono )
{
   BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
   /* I2C array */

   BspTwl3029_I2C_RegData tmpCtrl4RegData;

   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL4_OFFSET,
                             &tmpCtrl4RegData);

    /* We are interested in bits 4:5(MONOR:MONOL)*/
   tmpCtrl4RegData = ((tmpCtrl4RegData>>4)&0x03);
   
   *mono = tmpCtrl4RegData;

   return returnVal;                                

}

  /*=============================================================================
 * Description:
 *   This returns the Input path configurations.
 */
 BspTwl3029_ReturnCode
 bspTwl3029_Audio_getClassD_mode(Uint8* classDmode)
  {
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData tmpClassDmodeRead;
    Uint8 tmpData;
	
    /* Read CTRL3 register store it in a temp var.*/
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,   BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
                             &tmpClassDmodeRead);

    tmpData = BSP_TWL3029_LLIF_GET_FIELD(BSP_TWL3029_LLIF_AUDIO_CTRL3_SPKDIGON, &tmpClassDmodeRead);

    if(tmpData != 0)
      *classDmode = 1;
    else
      *classDmode = 0;

    return returnVal;
  }

#endif

#ifdef TI_BSP_FF_AUDIO_ON_OFF


#define antipop_time 5

void bspTwl3029_audio_on_off_init( )
{
	STATUS returnVal;

	returnVal =  NU_Create_Timer(&tdma_1_ul_on_timer, "1tdma", bspTwl3029_audio_on_off_timer_expiry, 
	    AUDIO_UPLINK_ON, antipop_time, 0 ,NU_DISABLE_TIMER);

	returnVal =  NU_Create_Timer(&tdma_2_dl_on_timer, "2tdma", bspTwl3029_audio_on_off_timer_expiry, 
	    AUDIO_DOWNLINK_ON, antipop_time, 0 ,NU_DISABLE_TIMER);

	returnVal =  NU_Create_Timer(&tdma_ul_on_dl_on_timer, "dlulon", bspTwl3029_audio_on_off_timer_expiry, 
	    AUDIO_UPLINK_ON_DOWNLINK_ON, antipop_time, 0 ,NU_DISABLE_TIMER);
}



BspTwl3029_ReturnCode
bspTwl3029_audio_on_off_control (T_AUDIO_UPLINK_ON_OFF_CONTROL uplink_on_off_control,
					T_AUDIO_DOWNLINK_ON_OFF_CONTROL downlink_on_off_control, 
          T_AUDIO_ON_OFF_CONTROL_RETURN on_off_return, BspTwl3029_I2C_RegData outen_control)
{
  	T_AUDIO_ON_OFF_CONTROL on_off_control;
    BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
    BspTwl3029_I2C_RegData regData, shadowRegRead, shadowVULGain;
    Uint8 classD=0;
    Uint16 count = 0;
  
    /* I2C array */
    Bsp_Twl3029_I2cTransReqArray i2cTransArray;
    Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
      

    /* twl3029 I2C reg info struct */
    BspTwl3029_I2C_RegisterInfo regInfo[12] ;
    BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;
    
    /* callback function info pointer */
	BspTwl3029_I2C_Callback i2c_callback;
    BspTwl3029_I2C_CallbackPtr callbackPtr = &i2c_callback;
    BspTwl3029_I2C_RegData tmpctrl3=0;

	callbackPtr->callbackFunc = bspTwl3029_audio_on_off_callback;

	on_off_control = uplink_on_off_control | downlink_on_off_control;

    /*if Downlink ON set the corresponding popman bits*/
    if((on_off_control==AUDIO_UPLINK_OFF_DOWNLINK_ON)||(on_off_control==AUDIO_DOWNLINK_ON)||(on_off_control==AUDIO_UPLINK_ON_DOWNLINK_ON ))
    {
        BspTwl3029_I2C_RegData popmanData=0,popmanRead=0;

        bspTwl3029_Audio_popEnable(&popmanData);

        BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                &popmanRead); 



        if(popmanRead!=popmanData){ 

            returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                    popmanData,  regInfoPtr++);
            count++; 
        } 

        /* rvf_send_trace("pop",strlen("pop"),popmanData,RV_TRACE_LEVEL_ERROR, FFS_USE_ID ); 
           rvf_send_trace("o1",2,audio_outen_reg_as.outen1,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
           rvf_send_trace("o2",2,audio_outen_reg_as.outen2,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
           rvf_send_trace("o3",2,audio_outen_reg_as.outen3,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
           */
    }

	switch (on_off_control)
	{
	  case AUDIO_UPLINK_ON:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_ON_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_ON_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_ON_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/******************** Set DXEN bit in VULGAIN register **********************/
	    /* Read Shadow Register */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                             &shadowVULGain);

		shadowVULGain |= (1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET);

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                                   shadowVULGain,  regInfoPtr++);
	    count++;

		/******************** Switch ON Uplink **********************/
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULS_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 1); 

		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_ON;

	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;

	  }
	  break;

	  case AUDIO_UPLINK_OFF:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/******************** Switch OFF Uplink **********************/
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULR_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 0); 

		
		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_OFF;

	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;
	  }
	  break;

	  case AUDIO_DOWNLINK_ON:
	  {
		if (audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback; 
			audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].callback_val = on_off_return.callback_val; 
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/******************** Clear OUTEN registers **********************/

		/*returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
                                   0,  regInfoPtr++);
	    count++;
		  */
		/****************** Switch Downlink ON ************************************/
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      


                /*OMAPS00086902*/ 
                /*	BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                    BSP_TWL_3029_MAP_AUDIO_POPMAN_ENABLE,regInfoPtr++);

                    count++;*/
		
					
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLS_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 1); 

		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_DOWNLINK_ON ;
		
	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;

 
	  }
	  break;

	  case AUDIO_DOWNLINK_OFF:
	  {
		if (audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/****************** Switch Downlink OFF ************************************/
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLR_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 0); 

		/* create I2C transaction to switch on uplink */

		callbackPtr->callbackVal = AUDIO_DOWNLINK_OFF;

      if(outen_control == OUTEN_DISABLE)
      {
        returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT,  regInfoPtr++);
        count++;
        returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT,  regInfoPtr++);
        count++;

		    /*OMAPS00089393*/
		    bspTwl3029_Audio_getClassD_mode(&classD);
		    if(classD==0x01)
		    {
			//rvf_send_trace("classD",strlen("classDl"),outen_control,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_HFTEST1_OFFSET,0x00,regInfoPtr++);
			count++;

			BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
				&tmpctrl3);

			tmpctrl3&=0x7F;

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,tmpctrl3,regInfoPtr++);
			count++;

		    }
		    returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
			    BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT,  regInfoPtr++);
		    count++;

		    if(classD==0x01)
		    {	

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_CKG,
				BSP_TWL_3029_MAP_CKG_TESTUNLOCK_OFFSET,0x00,regInfoPtr++);
			count++;


		    }

      }

	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;
		
	  }
	  break;

	  case AUDIO_UPLINK_ON_DOWNLINK_ON:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].audio_on_off_callback  == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}
		
		/******************** Set DXEN bit in VULGAIN register **********************/
	    /* Read Shadow Register */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                             &shadowVULGain);

		shadowVULGain |= (1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET);

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                                   shadowVULGain,  regInfoPtr++);
	    count++;

		/******************** Clear OUTEN registers **********************/

		/*	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
                                   0,  regInfoPtr++);
	    count++;
			*/
		/*********************** Enable Uplink and Downlink Path ******************/
		
                /*OMAPS00086902*/
                /*	BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                    BSP_TWL_3029_MAP_AUDIO_POPMAN_ENABLE,regInfoPtr++);
                    count++;
                    */	

	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULS_OFFSET ));
		regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLS_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 1); 
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 1); 
		
		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_ON_DOWNLINK_ON;

	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;
		
		
		 
	  }
	  break;
	  case AUDIO_UPLINK_OFF_DOWNLINK_ON:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/******************** Clear OUTEN registers **********************/

		/*returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   0,  regInfoPtr++);
	    count++;

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
                                   0,  regInfoPtr++);
	    count++;
		  */	
		/*********************** Disable Uplink and Enable Downlink Path ***************/
		
                /*OMAPS00086902*/

                /*BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_POPMAN_OFFSET,
                  BSP_TWL_3029_MAP_AUDIO_POPMAN_ENABLE,regInfoPtr++);

                  count++;
                  */	
		
		
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULR_OFFSET ));
		regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLS_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 0); 
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 1); 
		
	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;

		             

		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_OFF_DOWNLINK_ON;
	  }
	  break;

	  case AUDIO_UPLINK_ON_DOWNLINK_OFF:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/******************** Set DXEN bit in VULGAIN register **********************/
	    /* Read Shadow Register */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                             &shadowVULGain);

		shadowVULGain |= (1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET);

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                                   shadowVULGain,  regInfoPtr++);
	    count++;

		/*********************** Enable Uplink and Disable Downlink Path ***************/
		
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULS_OFFSET ));
		regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLR_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 1); 
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 0); 
		
    if(outen_control == OUTEN_DISABLE)
    {
      returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT,  regInfoPtr++);
      count++;
      returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT,  regInfoPtr++);
      count++;
		    /*OMAPS00089393*/
		    bspTwl3029_Audio_getClassD_mode(&classD);
		    if(classD==0x01)
		    {
			//rvf_send_trace("classD",strlen("classDl"),outen_control,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_HFTEST1_OFFSET,0x00,regInfoPtr++);
			count++;

			BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
				&tmpctrl3);

			tmpctrl3&=0x7F;

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,tmpctrl3,regInfoPtr++);
			count++;

		    }
		    returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
			    BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT,  regInfoPtr++);
		    count++;

		    if(classD==0x01)
		    {	

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_CKG,
				BSP_TWL_3029_MAP_CKG_TESTUNLOCK_OFFSET,0x00,regInfoPtr++);
			count++;


		    }

    }
	    /* update Triton power */
    
    	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
	    count++;		
		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_ON_DOWNLINK_OFF;

	  }
	  break;


	  case AUDIO_UPLINK_OFF_DOWNLINK_OFF:
	  {
		if (audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].audio_on_off_callback == NULL) 
		{
			audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].audio_on_off_callback = on_off_return.audio_on_off_callback;
			audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].callback_val = on_off_return.callback_val;
		}
		else
		{
			return BSP_TWL3029_RETURN_CODE_FAILURE;
		}

		/*********************** Enable Uplink and Disable Downlink Path ***************/
		
	    /* Keep track of power settings in shadow register  by updating shadow PWRONSTATUS register  */                                                                                                                      
	    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
                             &shadowRegRead);

		regData = (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VULR_OFFSET ));
		regData |= (1 << ( BSP_TWL3029_LLIF_AUDIO_TOGB_VDLR_OFFSET ));
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON, &shadowRegRead, 0); 
		BSP_TWL3029_LLIF_SET_FIELD( BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON, &shadowRegRead, 0); 
		
      if(outen_control == OUTEN_DISABLE)
      {
        returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN1_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN1_DEFAULT,  regInfoPtr++);
        count++;
        returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN2_OFFSET,
                                   BSP_TWL_3029_MAP_AUDIO_OUTEN2_DEFAULT,  regInfoPtr++);
        count++;
		    /*OMAPS00089393*/
		    bspTwl3029_Audio_getClassD_mode(&classD);
		    if(classD==0x01)
		    {
			//rvf_send_trace("classD",strlen("classDl"),outen_control,RV_TRACE_LEVEL_ERROR, FFS_USE_ID );
			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_HFTEST1_OFFSET,0x00,regInfoPtr++);
			count++;

			BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,
				&tmpctrl3);

			tmpctrl3&=0x7F;

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
				BSP_TWL_3029_MAP_AUDIO_CTRL3_OFFSET,tmpctrl3,regInfoPtr++);
			count++;

		    }
		    returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_OUTEN3_OFFSET,
			    BSP_TWL_3029_MAP_AUDIO_OUTEN3_DEFAULT,  regInfoPtr++);
		    count++;

		    if(classD==0x01)
		    {	

			returnVal = BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_CKG,
				BSP_TWL_3029_MAP_CKG_TESTUNLOCK_OFFSET,0x00,regInfoPtr++);
			count++;


		    }

       }
      
      /* update Triton power */
    
      returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_TOGB_OFFSET,
                                   regData,  regInfoPtr++);
      count++;
		/* create I2C transaction to switch on uplink */
		callbackPtr->callbackVal = AUDIO_UPLINK_OFF_DOWNLINK_OFF;
    
	  }
	  break;

	  default:
	  	return BSP_TWL3029_RETURN_CODE_FAILURE;
	}

	    /* update shadow  power  reg, this is used by other functions as an alternative to reading the HW register */
    if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
       BspTwl3029_I2c_shadowRegWrite( BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET, shadowRegRead);
    }
    

	if (returnVal != BSP_TWL3029_RETURN_CODE_FAILURE)
    {
    	regInfoPtr = regInfo;
    	/* now request to I2C manager to write to Triton registers */
    
	   	returnVal = BspTwl3029_I2c_regInfoSend(regInfo,count,callbackPtr,
        	  (BspI2c_TransactionRequest*)i2cTransArrayPtr);
   	}

	return returnVal;
}

/* This is I2C call back and is called in HISR context */
void bspTwl3029_audio_on_off_callback (BspI2c_TransactionId transaction_id)
{
	switch(transaction_id)
	{
	case AUDIO_UPLINK_ON:
		audio_uplink_on_state = AUDIO_UPLINK_ON;
		NU_Reset_Timer(&tdma_1_ul_on_timer, bspTwl3029_audio_on_off_timer_expiry,
		    antipop_time, 0 ,NU_ENABLE_TIMER);

		/* Start 1 TDMA timer */
		break;
	case AUDIO_UPLINK_OFF:
		/* call the callback function */
		audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].audio_on_off_callback ( audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].callback_val);
		audio_on_off_return[AUDIO_UPLINK_OFF_INDEX].audio_on_off_callback = NULL;
		break;
	case AUDIO_DOWNLINK_ON:
		audio_downlink_on_state = AUDIO_DOWNLINK_ON;
		/* Start 2 TDMA timer */
		NU_Reset_Timer(&tdma_2_dl_on_timer, bspTwl3029_audio_on_off_timer_expiry,
		    antipop_time, 0 ,NU_ENABLE_TIMER);

		break;
	case AUDIO_DOWNLINK_OFF:
		/* call the callback function */
		audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].audio_on_off_callback ( audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].callback_val);
		audio_on_off_return[AUDIO_DOWNLINK_OFF_INDEX].audio_on_off_callback = NULL;
		break;
	case AUDIO_UPLINK_OFF_DOWNLINK_ON:
		audio_downlink_on_state = AUDIO_UPLINK_OFF_DOWNLINK_ON;
		/* Start 2 TDMA timer */
		NU_Reset_Timer(&tdma_2_dl_on_timer, bspTwl3029_audio_on_off_timer_expiry,
		    antipop_time, 0 ,NU_ENABLE_TIMER);

		break;
	case AUDIO_UPLINK_ON_DOWNLINK_OFF:
		/* Start 1 TDMA timer */
		audio_uplink_on_state = AUDIO_UPLINK_ON_DOWNLINK_OFF;
		NU_Control_Timer(&tdma_1_ul_on_timer, NU_ENABLE_TIMER);
		break;
	case AUDIO_UPLINK_OFF_DOWNLINK_OFF:
		/* call the callback function */		
		audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].audio_on_off_callback ( audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].callback_val);
		audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_OFF_INDEX].audio_on_off_callback = NULL;
		break;
	case AUDIO_UPLINK_ON_DOWNLINK_ON:
		/* Start 1 TDMA timer */
		audio_uplink_on_downlink_on_state = UPLINK_WAIT_1_TDMA_EXPIRY;
		NU_Reset_Timer(&tdma_ul_on_dl_on_timer, bspTwl3029_audio_on_off_timer_expiry,
		   antipop_time, 0 ,NU_ENABLE_TIMER);

		break;
	}
}

void bspTwl3029_audio_final_callback (BspI2c_TransactionId transaction_id)
{
	return;
}

void bspTwl3029_audio_on_off_timer_expiry(UNSIGNED expiry_id)
{

  BspTwl3029_I2C_RegData shadowVULGain;
  /* callback function info pointer */
  BspTwl3029_I2C_Callback i2c_callback;
  BspTwl3029_I2C_CallbackPtr callbackPtr= &i2c_callback;
  BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;

  /* I2C array */
  Bsp_Twl3029_I2cTransReqArray i2cTransArray;
  Bsp_Twl3029_I2cTransReqArrayPtr i2cTransArrayPtr= &i2cTransArray;
      
  /* twl3029 I2C reg info struct */
  BspTwl3029_I2C_RegisterInfo regInfo[8] ;
  BspTwl3029_I2C_RegisterInfo* regInfoPtr = regInfo;

  Uint16 count = 0;
  
  switch (expiry_id)
  {
  case AUDIO_UPLINK_ON:
  {
	/* Reset the DXEN bit in VULGAIN. Send the I2C transaction */
    /* Read Shadow Register */                                                                                                                      
    returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                            &shadowVULGain);

	shadowVULGain &= ~(1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET);

	returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                                   shadowVULGain,  regInfoPtr++);
    count++;

  	if( AUDIO_UPLINK_ON == audio_uplink_on_state )
  	{
	  	callbackPtr->callbackFunc = audio_on_off_return[AUDIO_UPLINK_ON_INDEX].audio_on_off_callback;
		callbackPtr->callbackVal = audio_on_off_return[AUDIO_UPLINK_ON_INDEX].callback_val;
		audio_on_off_return[AUDIO_UPLINK_ON_INDEX].audio_on_off_callback = NULL;
  	}
	else if ( AUDIO_UPLINK_ON_DOWNLINK_OFF == audio_uplink_on_state )
	{
	  	callbackPtr->callbackFunc = audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].audio_on_off_callback;
		callbackPtr->callbackVal = audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].callback_val;
		audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_OFF_INDEX].audio_on_off_callback = NULL;
	}
	
		
   	returnVal = BspTwl3029_I2c_regInfoSend(regInfo,count,callbackPtr,
        	  (BspI2c_TransactionRequest*)i2cTransArrayPtr);
  	break;
  }
  case AUDIO_DOWNLINK_ON:
  {

  	if( AUDIO_DOWNLINK_ON == audio_downlink_on_state )
  	{
	  	callbackPtr->callbackFunc = audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].audio_on_off_callback;
		callbackPtr->callbackVal = audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].callback_val;
		audio_on_off_return[AUDIO_DOWNLINK_ON_INDEX].audio_on_off_callback = NULL;
  	}
	else if ( AUDIO_UPLINK_OFF_DOWNLINK_ON == audio_downlink_on_state )
	{
	  	callbackPtr->callbackFunc = audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].audio_on_off_callback;
		callbackPtr->callbackVal = audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].callback_val;
		audio_on_off_return[AUDIO_UPLINK_OFF_DOWNLINK_ON_INDEX].audio_on_off_callback = NULL;
	}
  (*callbackPtr->callbackFunc)(callbackPtr->callbackVal);
  	break;
  }
  case AUDIO_UPLINK_ON_DOWNLINK_ON:
  {
  	if( UPLINK_WAIT_1_TDMA_EXPIRY == audio_uplink_on_downlink_on_state)
  	{
		/* Reset the DXEN bit in VULGAIN. Send the I2C transaction */
	        /* Read Shadow Register */                                                                                                                      
	        returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                            &shadowVULGain);

		shadowVULGain &= ~(1 << BSP_TWL3029_LLIF_AUDIO_VULGAIN_DXEN_OFFSET);

		returnVal= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_VULGAIN_OFFSET,
                                   shadowVULGain,  regInfoPtr++);
	        count++;

		callbackPtr->callbackFunc = bspTwl3029_audio_final_callback;
		callbackPtr->callbackVal = 0;

		returnVal = BspTwl3029_I2c_regInfoSend(regInfo,count,callbackPtr,
        	  (BspI2c_TransactionRequest*)i2cTransArrayPtr);

		/* Restore the outen settings */
		audio_uplink_on_downlink_on_state = DOWNLINK_WAIT_1_TDMA_EXPIRY;
		NU_Reset_Timer(&tdma_ul_on_dl_on_timer, bspTwl3029_audio_on_off_timer_expiry,
			    antipop_time, 0 ,NU_ENABLE_TIMER);

	}
	else if (DOWNLINK_WAIT_1_TDMA_EXPIRY == audio_uplink_on_downlink_on_state )
	{
	      	
	  	callbackPtr->callbackFunc = audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].audio_on_off_callback;
		callbackPtr->callbackVal = audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].callback_val;
		audio_on_off_return[AUDIO_UPLINK_ON_DOWNLINK_ON_INDEX].audio_on_off_callback = NULL;
		    (*callbackPtr->callbackFunc)(callbackPtr->callbackVal);
	}
   	break;
  }
  }
}


#ifdef TI_BSP_FF_AUDIO_ON_OFF_TEST

void audio_callback(Uint8 callbackVal)
{
#if 0
	BspTwl3029_ReturnCode returnVal = BSP_TWL3029_RETURN_CODE_FAILURE;
	

	returnVal = BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
	                        &globalRegRead, NULL);

	if(!(globalRegRead & BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VULON_OFFSET) && !(globalRegRead & BSP_TWL3029_LLIF_AUDIO_PWRONSTATUS_VDLON_OFFSET))
	{
	   /* SUCCESS */
	   returnVal = BspTwl3029_I2c_shadowRegRead(BSP_TWL3029_I2C_AUD, BSP_TWL_3029_MAP_AUDIO_PWRONSTATUS_OFFSET,
	                        &globalRegRead);

	}
	else
	{
		/* FAILURE */
	}

#endif
	T_AUDIO_ON_OFF_CONTROL_RETURN callback;

	callback.audio_on_off_callback = audio_callback;

	if( callbackVal == (AUDIO_UPLINK_ON | AUDIO_DOWNLINK_ON))
	{
		callback.callback_val = AUDIO_UPLINK_OFF | AUDIO_DOWNLINK_OFF;
    bspTwl3029_audio_on_off_control( AUDIO_UPLINK_OFF, AUDIO_DOWNLINK_OFF, callback, OUTEN_ENABLE);
	}
	else if (callbackVal == (AUDIO_UPLINK_OFF | AUDIO_DOWNLINK_OFF))
	{
		callback.callback_val = AUDIO_UPLINK_NONE | AUDIO_DOWNLINK_ON;
    bspTwl3029_audio_on_off_control( AUDIO_UPLINK_NONE, AUDIO_DOWNLINK_ON, callback, OUTEN_ENABLE);
	}
	else if ( callbackVal == (AUDIO_UPLINK_NONE | AUDIO_DOWNLINK_ON))
	{
		callback.callback_val = AUDIO_UPLINK_NONE | AUDIO_DOWNLINK_OFF;
    bspTwl3029_audio_on_off_control( AUDIO_UPLINK_NONE, AUDIO_DOWNLINK_OFF, callback, OUTEN_ENABLE);
	}
	else if (callbackVal == (AUDIO_UPLINK_NONE | AUDIO_DOWNLINK_OFF))
	{
		callback.callback_val = 6345;
    bspTwl3029_audio_on_off_control( AUDIO_UPLINK_ON, AUDIO_DOWNLINK_ON, callback, OUTEN_ENABLE);
	}
}

BspTwl3029_ReturnCode 
audio_on_off_test ()
{


	T_AUDIO_ON_OFF_CONTROL_RETURN callback;
	BspTwl3029_AudioOutputSource outputSource[6];

	callback.audio_on_off_callback = audio_callback;
	callback.callback_val = AUDIO_UPLINK_ON | AUDIO_DOWNLINK_ON;
	outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
#if 0
	bspTwl3029_audio_on_off_control( AUDIO_UPLINK_ON, AUDIO_DOWNLINK_NONE, callback);

	bspTwl3029_audio_on_off_control( AUDIO_UPLINK_NONE, AUDIO_DOWNLINK_ON, callback);
#endif
//	bspTwl3029_Audio_configureOutputPath(NULL, BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE,
//			outputSource);

//	bspTwl3029_audio_on_off_control( AUDIO_UPLINK_ON, AUDIO_DOWNLINK_ON, callback);

//	bspTwl3029_audio_on_off_control( AUDIO_UPLINK_OFF, AUDIO_DOWNLINK_ON, callback);
	
  bspTwl3029_audio_on_off_control( AUDIO_UPLINK_ON, AUDIO_DOWNLINK_ON, callback, OUTEN_ENABLE);
	

}

#endif /* TI_BSP_FF_AUDIO_ON_OFF_TEST end */


void BspTwl3029_cache_outen_reg(BspTwl3029_I2C_RegData outen1,
                                          BspTwl3029_I2C_RegData outen2,
                                          BspTwl3029_I2C_RegData outen3)
{
    audio_outen_reg_as.outen1 = outen1;
    audio_outen_reg_as.outen2 = outen2;
    audio_outen_reg_as.outen3 = outen3;
}

#endif



#ifndef HISR_STACK_SHARING
// #if (ACCESSORY_NOTIFICATION)
  // Accessory HISR stack size
  #define ACCESSORY_HISR_STACK_SIZE 512
  static char Audio_accessory_HisrStack[ACCESSORY_HISR_STACK_SIZE];
#endif
  // global flag to track headset plugin/unplug state
static   SYS_BOOL gpluginFlag = FALSE;

  // to store interrupt source id globally. used by HISR
 static BspTwl3029_IntC_SourceId gSourceId;

  static NU_HISR Accessory_HISR;
  static NU_TIMER Hook_Timer;


static  T_ACCESSORY_CALLBACK accessory_notify_cb[AUDIO_ACCESSORY_MAX];
  



static void Accessory_HISREntry(void)
  {

      switch(gSourceId)
      {
        case BSP_TWL3029_INTC_SOURCE_ID_AUD_HS:
	  {
	  	if(gpluginFlag == TRUE)
	  	  {
	  	               if(accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc!=NULL)
	  	            	{
			  	    accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc(AUDIO_HEADSET_PLUGGED,
					                                                    accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackVal);
	  	            	}
	  	  }
		  else
		  {
		  	if(accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc!=NULL)
		  		{
	  	    			accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc(AUDIO_HEADSET_UNPLUGGED, 
				                                                    accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackVal);
		  		}
		  }
	  }
	    break;

	  case BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK:
	  {
			if(accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc!=NULL)
			{
				  accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackFunc(AUDIO_HEADSET_HOOK_DETECT, 
			                                               accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackVal);
			}
	   }
	        break;

	default:
	  	  break;
      }

 	  // enable interrupt.
     // bspTwl3029_IntC_enableSource( gSourceId );
  }


//timer routine  is called in Timer HISR context to make it non blocking use I2c write with a dummy callback
 static void hook_dummy(BspI2c_TransactionId x)
{

    //AUDIO_SEND_TRACE("HOOK CALBACK WITH CONTIGOUS", (3));
}


static BspTwl3029_I2C_Callback hookcallback;
static BspI2c_TransactionRequest hooktransreq[2];

 void Hook_Timer_routine(UNSIGNED x)
 {
BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;
BspTwl3029_I2C_RegData   oldMask;
BspTwl3029_I2C_CallbackPtr callbackptr;
BspTwl3029_I2C_RegisterInfo reginfo;
BspTwl3029_I2C_RegisterInfo * reginfptr=&reginfo;
 BspI2c_TransactionRequest *transreqptr;
 
 int cnt=0;

   
   transreqptr=hooktransreq;
   hookcallback.callbackFunc=hook_dummy;
   hookcallback.callbackVal=0;
 
	
// turn on hook detect bit in ctrl5 register
retval=BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PAGE0,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, &oldMask);
 //retval=BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_PAGE0,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, &oldMask);
 	
      
        oldMask= oldMask|0x10;    
        
  retval= BspTwl3029_I2c_regQueWrite(BSP_TWL3029_I2C_AUD,
	BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, oldMask,reginfptr);    


cnt++;
BspTwl3029_I2c_regInfoSendContiguous(reginfptr,cnt, &hookcallback,transreqptr);





}


  SYS_BOOL Activate_Accessory_HISR()
  {
	  if(NU_SUCCESS != NU_Activate_HISR(&Accessory_HISR))
	  {
	    return 1;
	  }
	  return 0;
  }


#if(OP_L1_STANDALONE==0)
extern BspTwl3029_I2C_RegData Audintsts;
#endif 


  void BspTwl3029_audio_accessory_int_handler(BspTwl3029_IntC_SourceId    sourceId)
  {
    
      Uint8 flag=0;
#if(OP_L1_STANDALONE==0)
         flag=Audintsts&(Uint8)0x01;//check if HSINTSTS bit in POPTEST1 REGISTERis zero
#endif 
	gSourceId = sourceId;
	

	
		if(gpluginFlag==FALSE)
		  {
		               if(flag==0)
				{

		                           gpluginFlag=TRUE;
						if(Activate_Accessory_HISR()) 
						{
		                
		                                      //AUDIO_SEND_TRACE("Handler not sucessful", (3));
						  //handler NOT successfully executed 
		                          // handle this error
			                       } 
		              }
		}
		if(gpluginFlag==TRUE)
		  {
		               if(flag!=0 )
		              {
		              gpluginFlag=FALSE;
		                            
					if(Activate_Accessory_HISR())
					{
		                             //AUDIO_SEND_TRACE("Handler not sucessful", (3));   
					      //handler NOT successfully executed 
		                          // handle this error
			               }                       
				}
		}

  }



void BspTwl3029_audio_hook_accessory_int_handler(BspTwl3029_IntC_SourceId    sourceId)
{

        Uint8 flag=0;
#if(OP_L1_STANDALONE==0)
	flag=Audintsts&0x02;//check if HOOKINTSTS bit in POPTEST1 REGISTERis zero
#endif
	  gSourceId=sourceId;

	if((gpluginFlag==TRUE)&&(flag==0))
	{
	//AUDIO_SEND_TRACE("HOOK ISR activated", RV_TRACE_LEVEL_ERROR);
			if(Activate_Accessory_HISR()) 
			{
			                
			                            //handler NOT successfully executed 
			                          // handle this error
			 } 
	}

}



BspTwl3029_ReturnCode Bsptwl3029_audio_accessory_register_notification(T_ACCESSORY_CALLBACK accessory_callback, 
 								T_AUDIO_ACCESSORY_TYPE accessory)
  {

    BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;

	// validate input
    if(accessory >= AUDIO_ACCESSORY_MAX)
    {
      	return BspAudioError;//already a handler exists
    }
 
if(accessory_notify_cb[accessory].callbackFunc!=NULL)
{
  	return BspAudioError;

}
     
#if(OP_L1_STANDALONE==0)
switch(accessory)
{
  case AUDIO_ACCESSORY_HEADSET:
	{
               	  
                    //AUDIO_SEND_TRACE("HS REgister ",RV_TRACE_LEVEL_ERROR);
		  // enable the Plug/unplug interrupt.
               // update the local callback array with the callback fun.,
               // register the interrupt handler for the accessories
          // this is common handler for plugin/unplugin and 
          // within this handler the particular source will be identified.
          // this can be HISR.
          // handler yet to be decided.
		     accessory_notify_cb[accessory]= accessory_callback;
		     if(retval==BSP_TWL3029_RETURN_CODE_SUCCESS)
                   { 
                              retval= bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HS,
                               BspTwl3029_audio_accessory_int_handler);
		     }
		  
 	           if(retval==BSP_TWL3029_RETURN_CODE_SUCCESS)
          	    {

          	    	retval=BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_INT2,
                            BSP_TWL_3029_MAP_AUDIO_POPTEST1_OFFSET , &Audintsts, NULL);
 		    }
   		   if(retval==BSP_TWL3029_RETURN_CODE_SUCCESS)         
		   { 

     			   if( (Audintsts&0x01)==0)
       		     {
                      	   gSourceId=BSP_TWL3029_INTC_SOURCE_ID_AUD_HS;
		       	      gpluginFlag=TRUE;
                     	      if(Activate_Accessory_HISR()) 
					{
                     	        //AUDIO_SEND_TRACE("headset Handler not sucessful", RV_TRACE_LEVEL_DEBUG_HIGH);   
				   	   return BspAudioError;
	             } 		      


	         }
   	          }
//ENALBLE headset  INTERRUPTS
	if(retval==BSP_TWL3029_RETURN_CODE_SUCCESS)    
		retval=bspTwl3029_IntC_enableSource( BSP_TWL3029_INTC_SOURCE_ID_AUD_HS);
                
      
       if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
	   	return BspAudioError;

	  }
             break;

			 
    case AUDIO_ACCESSORY_HOOK:
		{
                             //AUDIO_SEND_TRACE("Hook REgister ",RV_TRACE_LEVEL_ERROR);
			if( (accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc!=NULL)&&(gpluginFlag==TRUE))
                     { 
				accessory_notify_cb[accessory]= accessory_callback;
				bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK,
                            BspTwl3029_audio_hook_accessory_int_handler );	

				retval= BspTwl3029_hook_detect_enable(BSP_TWL3029_AUDIO_INPUT_MIC_BIAS_2_5 );

                        	if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                            {
                                   // AUDIO_SEND_TRACE("HOOK enable fail",RV_TRACE_LEVEL_DEBUG_HIGH);
				      	 return BspAudioError;
                        	}
			      retval= bspTwl3029_IntC_enableSource(BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK);		  

				if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                    		{
                   		        return BspAudioError;
                  	       }
			return retval;
                     
		      }

                        else
				{///there is no hook detection without headset present
						return BspAudioError;			
                                 //AUDIO_SEND_TRACE("Hook REgister error ",RV_TRACE_LEVEL_ERROR);
				}		



	         }
	  	
	  case AUDIO_ACCESSORY_CARKIT:
	  	// check whether callback is registered
              return BspAudioError;
		
	  default:
	  	return BspAudioError;
	}

#endif    
    return BspAudioSuccess;
  }

 
  


  /**
 *-----------------------------------------------------------------------*
 * @name audio_accessory_int_init
 *
 * This function is called to initialise the usb interrupt handler		 
 * mechanism															 
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */

  BspAudioReturn audio_accessory_int_init()
{
	  STATUS status_ret;
	 UNSIGNED ticks;

	  // Fill the entire stack with the pattern 0xFE
#ifndef HISR_STACK_SHARING
	  memset (Audio_accessory_HisrStack, 0xFE, sizeof(Audio_accessory_HisrStack));
#endif

	  // Create the HISR which is called when an RNG interrupt is received.
	  status_ret = NU_Create_HISR(&Accessory_HISR, "Accessory HISR", Accessory_HISREntry, 2, 
#ifndef HISR_STACK_SHARING
	  Audio_accessory_HisrStack,
	  sizeof(Audio_accessory_HisrStack)); // lowest prty
#else
	  HISR_STACK_PRIO2,HISR_STACK_PRIO2_SIZE);
#endif
	
	  if (status_ret != NU_SUCCESS)
	  {
		  return BspAudioError;
	  }

ticks=100;//20ms delay is needed 5 Time constantys forthe capacitor to charge only then hook en bit in ctrl 5 is set,
//each os tick is one TDMA=4.85ms

		 status_ret=NU_Create_Timer(&Hook_Timer, "hoktmr", Hook_Timer_routine,(UNSIGNED)1, ticks,
(UNSIGNED)0,NU_DISABLE_TIMER);

	if (status_ret != NU_SUCCESS) 
	{
		  return BspAudioError;
	}
 bspTwl3029_IntC_enableSource(BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK);	
accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc=NULL;
accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackFunc=NULL;

 return BspAudioSuccess;
}






BspAudioReturn Bsptwl3029_audio_accessory_Unregister_notification(T_AUDIO_ACCESSORY_TYPE accessory)

{
     BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;


       if(accessory >= AUDIO_ACCESSORY_MAX)
       {
       	  return BspAudioError;
        }


          if(accessory_notify_cb[accessory].callbackFunc==NULL)
	    {
	    	return BspAudioError;
           }


         if(accessory==AUDIO_ACCESSORY_HEADSET)
         {   
                    retval=bspTwl3029_IntC_disableSource(BSP_TWL3029_INTC_SOURCE_ID_AUD_HS);		  
                     if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                     {
                     	   return BspAudioError;
              	 }		  

			retval=bspTwl3029_IntC_disableSource(BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK);

                     if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                     {
                     	 return BspAudioError;
                     }

			retval=BspTwl3029_hook_detect_disable();

			retval=bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HS,NULL );	
                     if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                     {
              	         return BspAudioError;
                    	}

			retval= bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK,
                                                                                 NULL );

			if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
                     {
                             return BspAudioError;
			}
                                       
                       accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackFunc= NULL;
      		   	accessory_notify_cb[AUDIO_ACCESSORY_HEADSET].callbackVal= NULL;								
                           
			
                      accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackFunc= NULL;
                      accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackVal= NULL;	  

                      		  


            }
     if(accessory==AUDIO_ACCESSORY_HOOK)
     {     
       	               //AUDIO_SEND_TRACE("Hook  UNREgister ",RV_TRACE_LEVEL_ERROR);
          retval=bspTwl3029_IntC_disableSource(BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK);	
          if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
          { 
                        return BspAudioError;
           }
          BspTwl3029_hook_detect_disable();    	
                             
           retval= bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK,
                                                                               NULL );
            if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
            {
                        return BspAudioError;
             }

	 accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackFunc= NULL;
        accessory_notify_cb[AUDIO_ACCESSORY_HOOK].callbackVal= NULL;

     }

return BspAudioSuccess;

}
#if(OP_L1_STANDALONE==0) && (PSP_STANDALONE==0)
T_AUDIO_FULL_ACCESS_WRITE hookmode;

T_AUDIO_MODE_ONOFF hook_onoff_req;


T_RV_RETURN hook_rv_ret_audio;

#endif

 //this function is a call back from L1 


 void hook_enable_callback(void *dummy)
{
BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_FAILURE;
    STATUS stsret;


if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
{
   //handle this error
}
//*************WAIT***********
   stsret= NU_Control_Timer(&Hook_Timer, NU_ENABLE_TIMER);

	if (stsret != NU_SUCCESS)
	{
		//AUDIO_SEND_TRACE("HOOK detect timer enable fail",RV_TRACE_LEVEL_ERROR);  //handle this error
              //Handle this error
	}



 }
    





BspTwl3029_ReturnCode BspTwl3029_hook_detect_enable(BspTwl3029_Audio_MicBiasId biasval)  	                                                                
{
#if(OP_L1_STANDALONE==0) && (PSP_STANDALONE==0)
   BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_SUCCESS;
   T_AUDIO_RET audioretval;

	hook_onoff_req.vul_onoff = AUDIO_VUL_ON;
        hook_onoff_req.vdl_onoff = AUDIO_VDL_NO_ACTION;

	hookmode.variable_indentifier=AUDIO_ONOFF;
       hookmode.data = (void *) &hook_onoff_req;
hook_rv_ret_audio.addr_id = NULL;

            hook_rv_ret_audio.callback_func = hook_enable_callback; 

//*********SET micbias to 2.0 *******
//retval=bspTwl3029_Audio_configureMicBias(NULL,biasval);
//if(retval!=BSP_TWL3029_RETURN_CODE_SUCCESS)
//	   return BSP_TWL3029_RETURN_CODE_FAILURE;


//*******************AUDIO UL ON**************************************************

// this turns on UL ;this is the L1 interface


	audioretval=audio_full_access_write(&hookmode,hook_rv_ret_audio);
	if(audioretval!=AUDIO_OK)
      {	
      		//AUDIO_SEND_TRACE("L1 REgister fail",RV_TRACE_LEVEL_ERROR);
   	      return BSP_TWL3029_RETURN_CODE_FAILURE;
      }

   return( retval );
#else/*#endif*/
  return BSP_TWL3029_RETURN_CODE_SUCCESS;
#endif  
  }


//A dummy Call Back is needed for L1 call back


void hook_disable_callback(void* dummy)
{
   
//AUDIO_SEND_TRACE("HOOK disable callback",RV_TRACE_LEVEL_ERROR);
}

BspTwl3029_ReturnCode BspTwl3029_hook_detect_disable()

{
#if(OP_L1_STANDALONE==0) && (PSP_STANDALONE==0)
BspTwl3029_ReturnCode retval=BSP_TWL3029_RETURN_CODE_FAILURE;
BspTwl3029_I2C_RegData   Mask;
 T_AUDIO_RET audioretval;  

retval=BspTwl3029_I2c_shadowRegRead( BSP_TWL3029_I2C_AUD,BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, &Mask);
Mask=Mask&(0xEF);//disable hook detection in ctrl5 reg
retval= BspTwl3029_I2c_WriteSingle(BSP_TWL3029_I2C_AUD,
                             BSP_TWL_3029_MAP_AUDIO_CTRL5_OFFSET, Mask, NULL);




  hook_onoff_req.vul_onoff = AUDIO_VUL_OFF;
  hook_onoff_req.vdl_onoff = AUDIO_VDL_NO_ACTION;

hookmode.variable_indentifier=AUDIO_ONOFF;
hookmode.data = (void  *) &hook_onoff_req;
hook_rv_ret_audio.addr_id = NULL;

hook_rv_ret_audio.callback_func = hook_disable_callback; 

audioretval=audio_full_access_write(&hookmode,hook_rv_ret_audio);
if(audioretval==AUDIO_OK)
{
	return BSP_TWL3029_RETURN_CODE_SUCCESS;
}

#endif

return BSP_TWL3029_RETURN_CODE_SUCCESS;

}


 


