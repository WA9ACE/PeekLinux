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
 
#ifndef BSP_TWL3029_MADC_HEADER
#define BSP_TWL3029_MADC_HEADER 
 /*******************************************************************************
 *   includes
 */

#include "types.h" 
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"


#include "bspUtil_Assert.h"



/*=============================================================================
 *  Defines and Macros
 */
/*==========================================================================*/
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcRegType
 *
 * @discussion 
 * <b> Description </b><br>
 *     MADc reg type 
 *    
 * @constant   BSPTWL3029_MADC_REGTYPE_RT
 *    Real Time conversion
 *
 * @constant   BSPTWL3029_MADC_REGTYPE_ASYNC
 *    Aysnc ( or general purpose) conversion
 */
enum 
{
    BSPTWL3029_MADC_REGTYPE_RT    = 0x0,
    BSPTWL3029_MADC_REGTYPE_ASYNC = 0x01
};

typedef Uint16 BspTwl3029_MadcRegType;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcChannelId
 *
 * @discussion 
 * <b> Description </b><br>
 *     A typedef which  is used for storage of a 16 bit channel conversion result 
 *    
 */ 
typedef Uint16 BspTwl3029_MadcConversionResult;


/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcChannelId
 *
 * @discussion 
 * <b> Description </b><br>
 *     A typedef which is used to select a channel. Multiple channels can be 
 *     selected by OR'ing individual members of the enum defined below.
 *     Note: ADIN 1 - 5 are external to Triton; ADIN 6 - 11 are internal.
 *
 *    
 */ 
enum
{
  BSP_TWL3029_ADC_CHANNEL_1_ADIN1     = 1,        /*    ADIN 1  */ 
         
  BSP_TWL3029_ADC_CHANNEL_2_ADIN1     = 2,         /*   ADIN 2 */

  BSP_TWL3029_ADC_CHANNEL_3_ADIN1     = 4 ,       /*    ADIN 3 */

  BSP_TWL3029_ADC_CHANNEL_4_BATT_TYPE = 8,        /* battery type*/

  BSP_TWL3029_ADC_CHANNEL_5_BTEMP     = 0x10,     /*    battery temp*/

  BSP_TWL3029_ADC_CHANNEL_6_USBVBUS   = 0x20,     /* USB Vbus voltage */
     
  BSP_TWL3029_ADC_CHANNEL_7_VBKP      = 0x40,     /*    backup battery voltage*/
 
  BSP_TWL3029_ADC_CHANNEL_8_ICHG      = 0x80,     /*  battery current charger */

  BSP_TWL3029_ADC_CHANNEL_9_VCHG      = 0x100,    /* battery charger voltage */

  BSP_TWL3029_ADC_CHANNEL_10_VBAT     = 0x200,    /* battery voltage */

  BSP_TWL3029_ADC_CHANNEL_11_HOTDIE    = 0x400    /*die temperature sens-ing */   
};
typedef Uint16 BspTwl3029_MadcChannelId;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcConversionCompleteCallback
 *
 * @discussion 
 * <b> Description </b><br>
 *     A typdef which defines the callback type for notification once the conversion is complete.
 *     This callback should be provided at the interface level and not for each
 *     channel type
 *    
 */ 
 typedef void (* BspTwl3029_MadcConversionCompleteCallback) (void);
 /*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcResults
 *
 * @discussion 
 * <b> Description </b><br>
 *     A struct for storing  the channel conversion results and also 
 *     collision error results
 * 
 * @field adc1
 *   result for ADC 1 channel
 *           -
 *           -
 *
 * @field adc11_hotdie
 *    result for ADC 11 channel  ( hotdie )
 *
 * @field chanCollisionFlags
 *    Holds collision error flags for  conversion results.
 *    If bit Bit (N-1) is   high, then this indicates that  a collison error occurred for 
 *    channel ADC N.  
 */ 
typedef struct
{
   BspTwl3029_MadcConversionResult    adc1;          /* channel 1 */
   BspTwl3029_MadcConversionResult    adc2;          /* channel 2 */
   BspTwl3029_MadcConversionResult    adc3;          /* channel 3  */   
   BspTwl3029_MadcConversionResult    adc4_bt;       /* battery type*/
   BspTwl3029_MadcConversionResult    adc5_btemp;    /* battery temp*/
   BspTwl3029_MadcConversionResult    adc6_usbvbus;  /* USB Vbus voltage*/
   BspTwl3029_MadcConversionResult    adc7_vbkp;     /* backup batt voltage*/
   BspTwl3029_MadcConversionResult    adc8_ichg;     /* batt charger current*/
   BspTwl3029_MadcConversionResult    adc9_vchg;     /* batt charger voltage*/
   BspTwl3029_MadcConversionResult    adc10_vbat;    /* batt voltage*/
   BspTwl3029_MadcConversionResult    adc11_hotdie;  /*die temperature sensing */
   Uint16                             chanCollisionFlags;  /* bib map of channel errors */
} BspTwl3029_MadcResults;

 
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_MadcChannelInfo
 *
 * @discussion 
 * <b> Description </b><br>
 *    A struct for storing  the channel  info used for reading  and processing  of the results   registers on completion of conversions.
 *
 * @field chanConversionResultsPtr
 *  pointer to  struct for storing  the channel conversion results and also collision
 *  error results
 *
 * @field chan2Read
 *    channel(s) to read/ trigger. To select channel ADC N set bit (N-1) to 1. 
 *    Multiple channels can be selected.  
 *
 * @field clbk
 *    pointer to a function to be run on completion of conversion.
 *    
 */  
typedef struct
{
    BspTwl3029_MadcResults*                   chanConversionResultsPtr;
    BspTwl3029_MadcChannelId                  chan2Read;            
    BspTwl3029_MadcConversionCompleteCallback clbk;
} BspTwl3029_MadcChannelInfo; 
 

/*=============================================================================
 *   Public Functions
 *============================================================================*/


/*===========================================================================*/
/*!
 * @function bspTwl3029_Madc_init
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    initialization function
 *
 * @param none
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_init(void);


/*===========================================================================*/
/*!
 * @function bspTwl3029_Madc_triggerConversion_async
 * <b> Description  </b><br>
 *     Function for triggering of an asynchronous MADC conversion. 
 *
 *    On end of conversion a twl3029 MADC interrupt is generated. 
 *    During the event handling for this  interrupt the following is done:
 *     1.The  results of the conversion are read and stored in the struct 
 *       pointed to by the arg resultsPtr.
 *     2.Collision results are also stored  using the same struct. 
 *     3.The MADC callback routine pointed to by madcCallbackFunc-tionPtr is executed.
 *
 *    Note: conversions are performed for all 11 channels when an aync 
 *     conversion is triggered.
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param madcCallbackFunctionPtr
 *   pointer to function to be run on completion of MADC
 *    conversion
 *
 * @param resultsPtr
 *   Pointer to a struct for returning results of conversion and also a 
 *   a 16 bit collision  results flag. If bit (N-1) of this flag  is high after 
 *       the completion of conversion, then this indicates that results register
 *       for ADC N was read while  a conversion was in progress.
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_triggerConversion_async( BspTwl3029_I2C_CallbackPtr          callbackInfoPtr,
                                  BspTwl3029_MadcConversionCompleteCallback  madcCallbackFunctionPtr,
				  BspTwl3029_MadcResults *                   resultsPtr);


/*===========================================================================*/
/*!
 * @function  bspTwl3029_Madc_enableRt
 * <b> Description  </b><br>
 *     Enables the one or more specified channels for RT conversion.
 *
 *   On end of a conversion a twl3029 MADC interrupt is generated. 
 *  During the event handling for this  interrupt the following is done:
 *  1.	The  results of the conversion are read and stored in the struct 
 *      pointed to by the arg resultsPtr.
 *  2.	Collision results are also stored  using the same struct. 
 *  3.	The MADC callback routine pointed to by rtCallbackFunctionPtr 
 *     is executed.
 *
 *   Note: Triggering is controlled by the Neptune TPU and is not part of the 
 *      twl3029 driver. 
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param chan2Read
 *    The channel id(s) to be enabled for conversion. ADC N is enabled if 
 *     bit (N-1) of  chan2Read  is high. All other channels are disabled
 *
 * @param madcCallbackFunctionPtr
 *   pointer to function to be run on completion of MADC
 *    conversion
 *
 * @param resultsPtr
 *   Pointer to a struct for returning results of conversion and also a 
 *   a 16 bit collision  results flag. If bit (N-1) of this flag  is high after 
 *    the completion of conversion, then this indicates that results register
 *    for ADC N was read while  a conversion was in progress.
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_enableRt( BspTwl3029_I2C_CallbackPtr                callbackInfoPtr,
                          BspTwl3029_MadcChannelId                  chan2Read,
		          BspTwl3029_MadcConversionCompleteCallback rtCallbackFunctionPtr,
		          BspTwl3029_MadcResults*                   resultsPtr);

/*===========================================================================*/
/*!
 * @function 
 * <b> Description  </b><br>
 *     Disables all channels for RT conversion.
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 * 
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_disableRt( BspTwl3029_I2C_CallbackPtr          callbackInfoPtr);

/*===========================================================================*/
/*!
 * @function 
 * <b> Description  </b><br>
 *     Reads conversion results for the last conversion from  twl3029 registers
 *     and stores them in the struct pointed to by resultsPtr and. Collision 
 *    errors are also store in same struct.
 *
 *    Note: When using I2C CALLBACK mode, data in struct pointed to by 
 *    madcResultsPtr will only be valid on completion of the I2C transaction 
 *    ( i.e. when the I2C callback function is called ).
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param madcResultsPtr 
 *    Pointer to a struct for returning results of conversion and also a 
 *   a 16 bit collision  results flag. If bit (N-1) of this flag  is high after 
 *   the completion of conversion, then this indicates that results register
 *   for ADC N was read while  a conversion was in progress.
 *
 * @param chan2Read
 *   The channel id(s) to be enabled for conversion. ADC N is enabled 
 *   if bit (N-1) of  chan2Read  is high. All other channels are disabled
 * 
 * @param type
 *   select either async or Real Time bank of result registers.
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode
bspTwl3029_Madc_readConversionResult( BspTwl3029_I2C_CallbackPtr   callbackInfoPtr,
                                      BspTwl3029_MadcResults*      madcResultsPtr,
				      BspTwl3029_MadcChannelId     chan2Read,
				      BspTwl3029_MadcRegType       type); 
 

/*===========================================================================*/
/*!
 * @function 
 * <b> Description  </b><br>
 *     Reads conversion results for the last conversion from the  twl3029 
 *     driver's bank of shadow  registers and stores them in the struct pointed to 
 *     by re-sultsPtr and. Collision errors are also stored in same struct.
 *
 *    Note: twl3029 shadow registers are updated every time the  MADC 
 *    hardware conversion results registers are read.
 *
 * @param madcResultsPtr 
 *     Pointer to a struct for returning results of conversion and also a 
 *    a 16 bit collision  results flag. If bit (N-1) of this flag  is high after 
 *    the completion of conversion, then this indicates that results register
 *    for ADC N was read while  a conversion was in progress.
 *
 * @param chan2Read
 *    The channel id(s) to be enabled for conversion. ADC N is enabled 
 *    if bit (N-1) of  chan2Read  is high. All other channels are disabled
 * 
 * @param type
 *    select either async or Real Time bank of result registers.
 *   
 * @return 
 *    return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */ 
BspTwl3029_ReturnCode
bspTwl3029_Madc_readShadowResult(  BspTwl3029_MadcResults*      madcResultsPtr,
				   BspTwl3029_MadcChannelId     chan2Read,                               
				   BspTwl3029_MadcRegType       regType);

/*===========================================================================*/
/*!
 * @function 
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 
 BspTwl3029_ReturnCode
bspTwl3029_Madc_setRtChannels(BspTwl3029_I2C_CallbackPtr     callbackInfoPtr , 
						BspTwl3029_MadcChannelId	chan2Read );

 
/*===========================================================================*/
/*!
 * @function 
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *   
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 
 
#endif
