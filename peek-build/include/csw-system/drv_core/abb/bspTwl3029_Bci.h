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
 *  FILE NAME: bspTwl3029_Bci.h
 *
 *
 *  PURPOSE:  
 *    This module exposes the interface for the BCI driver.
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
 *  04/03/2004  Mary Tooher     V1.0.0   First implementation
 */
#ifndef BSP_TWL3029_BCI_HEADER
#define BSP_TWL3029_BCI_HEADER 
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
 /*!
 * @typedef  BspTwl3029_Bci_state
 *
 * @discussion 
 * <b> Description </b><br>
 *     used to enable /disable a feature by setting/clearing 
 *     a bit in a register
 * @constant 
 *   BSP_TWL3029_BCI_ENABLE 
 *
 * @constant 
 *   BSP_TWL3029_BCI_DISABLE   
 */
 
 enum
{
    BSP_TWL3029_BCI_IDLE_OR_PRECRG = 0,
    BSP_TWL3029_BCI_LINCRG         = 1,         
    BSP_TWL3029_BCI_PWMCRG         = 2,
    BSP_TWL3029_BCI_ACCESSORY      = 3   

};
typedef Uint8 BspTwl3029_Bci_state;

 /*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_enable
 *
 * @discussion 
 * <b> Description </b><br>
 *     used to enable /disable a feature by setting/clearing 
 *     a bit in a register
 * @constant 
 *   BSP_TWL3029_BCI_ENABLE 
 *
 * @constant 
 *   BSP_TWL3029_BCI_DISABLE   
 */
 
 enum
{
    BSP_TWL3029_BCI_DISABLE  = 0,
    BSP_TWL3029_BCI_ENABLE = 1    

};
typedef Uint8 BspTwl3029_Bci_enable;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_dacType
 *
 * @discussion 
 * <b> Description </b><br>
 *
 * @constant 
 *   BSP_TWL3029_BCI_DACTYPE_VOLT 
 *
 * @constant 
 *   BSP_TWL3029_BCI_DACTYPE_CURRENT   
 */
 
 enum
{
    BSP_TWL3029_BCI_DACTYPE_VOLT    = 1,
    BSP_TWL3029_BCI_DACTYPE_CURRENT = 2    

};
typedef Uint8 BspTwl3029_Bci_dacType;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci
 *
 * @discussion 
 * <b> Description </b><br>
 *     Value for DAC (used  either for V DAC or I DAC )
 *     
 */

typedef Uint16 BspTwl3029_Bci_dacVal;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_pathType
 *
 * @discussion 
 * <b> Description </b><br>
 *
 * @constant  
 *  BSP_TWL3029_BCI_CHARGEPATH_AC
 *
 * @constant  
 *  BSP_TWL3029_BCI_CHARGEPATH_USB      
 */
 enum
{
    BSP_TWL3029_BCI_CHARGEPATH_USB  = 0,
    BSP_TWL3029_BCI_CHARGEPATH_AC = 1 

};
typedef Uint8 BspTwl3029_Bci_pathType;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_chargeMode
 *
 * @discussion 
 * <b> Description </b><br>
 *    defines charger mode - either constant voltage ( used in 2nd stage of
 *    Li-Ion battary ) or constant current (used for 1st stage for Li-Ion
 *    battery and also used for N-MH and Ni-Cd batteries )   
 * @constant     
 */
 enum
{
    BSP_TWL3029_BCI_CHARGEMODE_CONST_V = 0,
    BSP_TWL3029_BCI_CHARGEMODE_CONST_I = 1  

};
typedef Uint8 BspTwl3029_Bci_chargeMode;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_i2vOffset
 *
 * @discussion 
 * <b> Description </b><br>
 *     typedef used for the setting of a  voltage offset value
 *     for I to V converter
 * @constant  BSP_TWL3029_BCI_I2VOFF_100MV
 *    +100mV offset ( for CGAIN4 bit = 0  gain of 10)   
 * @constant  BSP_TWL3029_BCI_I2VOFF_200MV
 *    +200mV offset ( for CGAIN4 bit = 0  gain of 10)   
 * @constant  BSP_TWL3029_BCI_I2VOFF_300MV
 *    +300mV offset ( for CGAIN4 bit = 0  gain of 10)   
 * @constant  BSP_TWL3029_BCI_I2VOFF_400MV
 *    +400mV offset ( for CGAIN4 bit = 0  gain of 10)   
 */
 enum
{
    BSP_TWL3029_BCI_I2VOFF_100MV = 0 ,
    BSP_TWL3029_BCI_I2VOFF_200MV = 1 ,
    BSP_TWL3029_BCI_I2VOFF_300MV = 2 ,
    BSP_TWL3029_BCI_I2VOFF_400MV = 3  

};
typedef Uint8 BspTwl3029_Bci_i2vOffset;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_i2vGain
 *
 * @discussion 
 * <b> Description </b><br>
 *
 * @constant   BSP_TWL3029_BCI_I2VGAIN_10
 *    gain of 10 for I to V vonverter  
 *
 * @constant   BSP_TWL3029_BCI_I2VGAIN_4
 *    gain of 4 for I to V vonverter   
 */
 enum
{
    BSP_TWL3029_BCI_I2VGAIN_10 = 0 ,
    BSP_TWL3029_BCI_I2VGAIN_4  = 1   

};
typedef Uint8 BspTwl3029_Bci_i2vGain;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci
 *
 * @discussion 
 * <b> Description </b><br>
 *     Defines type of sensor. Note multiple sensors types can be selected 
 *    by ORing the sensors given in this enum.
 *
 * @constant  BSP_TWL3029_BCI_SENS_BATTYPE
 *    battery type detection
 *
 * @constant  BSP_TWL3029_BCI_SENS_TEMP
 *    battery temperature
 *       
 */
 enum
{
    BSP_TWL3029_BCI_SENS_BATTYPE  = 1,
    BSP_TWL3029_BCI_SENS_TEMP     = 2   

};
typedef Uint8 BspTwl3029_Bci_sensorType;



/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_thermCurrent
 *
 * @discussion 
 * <b> Description </b><br>
 *     used for  setting 8 values for thermal sensore bias current
 *
 * @constant   BSP_TWL3029_BCI_10
 *   set thermal bias current to  10 uA  ( min )  
 *
 * @constant   BSP_TWL3029_BCI_80
 *   set thermal bias current to  80 uA  ( max )  
 */

 enum
{
    BSP_TWL3029_BCI_THERM_I_10  = 0, 
    BSP_TWL3029_BCI_THERM_I_20,
    BSP_TWL3029_BCI_THERM_I_30,
    BSP_TWL3029_BCI_THERM_I_40,
    BSP_TWL3029_BCI_THERM_I_50,
    BSP_TWL3029_BCI_THERM_I_60,
    BSP_TWL3029_BCI_THERM_I_70,
    BSP_TWL3029_BCI_THERM_I_80   

};
typedef Uint8 BspTwl3029_Bci_thermCurrent;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_thermSign
 *
 * @discussion 
 * <b> Description </b><br>
 *    sign for thermal bias current
 *
 * @constant  BSP_TWL3029_BCI_THERMBIAS_NEG
 *  battery temperature with negative codfficient sensing 
 *
 * @constant  BSP_TWL3029_BCI_THERMBIAS_POS
 *  battery temperature with positive codfficient sensing 
 *  
 */
 enum
{
    BSP_TWL3029_BCI_THERMSIGN_NEG = 0 ,
    BSP_TWL3029_BCI_THERMSIGN_POS   

};
typedef Uint8 BspTwl3029_Bci_thermSign;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_skey
 *
 * @discussion 
 * <b> Description </b><br>
 *     Key codes for used for programming what triggers the end of charge interupt
 * @constant     
 */
 enum
{
    BSP_TWL3029_BCI_SKEY1_T_POS = 0xAA,    /* batt over temp ( positive thermister) */
    BSP_TWL3029_BCI_SKEY2_T_NEG = 0x55,    /* batt over temp (negative thermister) */
    BSP_TWL3029_BCI_SKEY3_EOC   = 0xDB,    /* Li Ion end of charge */
    BSP_TWL3029_BCI_SKEY4_V     = 0xBD,    /* batt over voltage  */
    BSP_TWL3029_BCI_SKEY5_REV_I = 0xd3,    /* reverse current */
    BSP_TWL3029_BCI_SKEY6_OFF   = 0x1A     /* comparator power down */
    

};
typedef Uint8 BspTwl3029_Bci_skey;


/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci_wdkey
 *
 * @discussion 
 * <b> Description </b><br>
 *     Key codes for used for programming watch dog timer 
 *    On time out a Watchdog over interrrupt is generated
 *    
 * @constant     
 */
 enum
{
    BSP_TWL3029_BCI_WDKEY_1S = 0xAA,       /* batt over temp ( positive thermister) */
    BSP_TWL3029_BCI_WDKEY_2S = 0x55,       /* batt over temp (negative thermister) */
    BSP_TWL3029_BCI_WDKEY_4S = 0xDB,       /* Li Ion end of charge */
    BSP_TWL3029_BCI_WDKEY_8S = 0xBD,       /* batt over voltage  */
    BSP_TWL3029_BCI_WDKEY_DISABLE = 0xf3,  /* reverse current */
    BSP_TWL3029_BCI_WDKEY_INT_ACK = 0x33   /* comparator power down */

};
typedef Uint8 BspTwl3029_Bci_wdkey;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_Bci
 *
 * @discussion 
 * <b> Description </b><br>
 *
 * @constant     
 */
 enum
{
    BSP_TWL3029_BCI ,
    BSP_TWL3029_BCI_   

};
typedef Uint8 BspTwl3029_Bci_;


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Bci_i2vConfigInfo
 *
 * <b> Description  </b><br>
 *    A struct BspTwl3029_Bci_i2vConfigInfo.
 *
 * @field 
 *
 */
typedef struct 
{    
   BspTwl3029_Bci_enable     offsetEn; 
   BspTwl3029_Bci_i2vOffset  offsetVal; 
   BspTwl3029_Bci_i2vGain    gain;
   BspTwl3029_Bci_enable     calibEn;
}  BspTwl3029_Bci_i2vConfigInfo; 
 /*=============================================================================
 *   Public Functions
 *============================================================================*/

 
 
 
 /*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_dacConfig
 * <b> Description  </b><br>
 *     Sets values of voltage DAC registers and/or current DAC register
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param types
 *    bit map which specifies type(s) of DAC ( V  and or I )
 *
 * @param valArrayPtr
 *   pointer to 2 element array, one for V DAC value, the other for I DAC value
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 
 
BspTwl3029_ReturnCode 
bspTwl3029_Bci_dacConfig(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                         BspTwl3029_Bci_dacType     types, 
			 BspTwl3029_Bci_dacVal*      valArrayPtr);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_linChargeEnable
 * <b> Description  </b><br>
 *     Enable linear charging
 *     
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param path
 *   select USB(0) or AC(1) path 
 *
 * @param constVorI
 *   select constant I (0) or constant V(1) charge mode
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode 
bspTwl3029_Bci_linChargeEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                BspTwl3029_Bci_pathType path, 
		                BspTwl3029_Bci_chargeMode constVorI);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_disable
 * <b> Description  </b><br>
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
bspTwl3029_Bci_linChargeDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr);




/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_i2vConfig
 * <b> Description  </b><br>
 *     configures I to V converter. 
 *       - enables /diables offset settings
 *       - defines offset value
 *       - sets gain to either 10 or 4
 *       - enables /disables a HW calibration routine to I to V converter
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *   pointer to struct containing info for I to V converter set up
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode 
bspTwl3029_Bci_i2vConfig( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                          BspTwl3029_Bci_i2vConfigInfo* configInfoPtr);  




/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_prechargeDisable
 * <b> Description  </b><br>
 *        disables precharging. Note Pre charger is enabled by HW when voltage 
 *        is less than 3.2V
 *        Note: Precharing is automatiically disabled by HW once voltage reaches
 *          3.6 V.
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
bspTwl3029_Bci_prechargeDisable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr );


/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_accessoryEnable
 * <b> Description  </b><br>
 *     enable accesory supply
 *     Note: This function can only be called after first checking that CHGSTS is off
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
bspTwl3029_Bci_accessoryEnable(  BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                 BspTwl3029_Bci_pathType path);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_accessoryEnable
 * <b> Description  </b><br>
 *  Description: disable accesory supply
 *   Note: Obvious place to call this  function 
 *         is during  routine resulting from an the handling of an ISR event 
 *         resulting from an interrrupt being generated after  LIMITEN had been 
 *          programmed and I threshold reached. 
 *         ( interrupt 1 - charge stop even T)
 *         The callbackInfoPtr CANNOT be defined as NULL when 
 *         calling during an ISR.  *
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
bspTwl3029_Bci_accessoryDisable(  BspTwl3029_I2C_CallbackPtr callbackInfoPtr);
                    

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_
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
bspTwl3029_Bci_pwmChargeEnable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr ,
                          Uint16 period,
			  BspTwl3029_Bci_chargeMode constVorI);

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_sensorsEnable
 * <b> Description  </b><br>
 *    Enables bias current for battery type AND/OR temperature .
 *
 *     There is a risk that CHEN and CHIV bits could be changed by HW or other
 *   Triton functions (if called from ISR ) 
 *   Work around for this is to forbid modifcation of sensors while CHEN  or PWM  
 *   is enabled.
 *   So if enabled beforehand, then the sensors remain enabled and v.v.
 *   Dynamic control of the current supplies for battery and temperature
 *   should not be an issue w.r.t power saving when a charger is connected to 
 *   a charger.
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param  sensorType
 *     bit map of sensor types. bit 0 - battery type sensor; bit 1 temperature
 *
 * @param  thermCurrent
 *    thermistor current ( 10 - 80 uA)
 *
 * @param  sign
 *    polarity of bias current
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode 
bspTwl3029_Bci_ChangeDutyCycle( BspTwl3029_I2C_CallbackPtr callbackInfoPtr , Uint16 period);
/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_sensorsEnable
 * <b> Description  </b><br>
 *    The dutycycle of the PWM signal in the pulse charge can be set using this.  
 *
 *     
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param callbackPtr
 *	the period of the PWM wave (0x000 to 0x03FF)
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */

BspTwl3029_ReturnCode 
bspTwl3029_Bci_sensorsEnable( BspTwl3029_I2C_CallbackPtr  callbackInfoPtr,
                              BspTwl3029_Bci_sensorType   sensorType,
			      BspTwl3029_Bci_thermCurrent thermCurrent,
			      BspTwl3029_Bci_thermSign    sign )	;

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_
 * <b> Description  </b><br>
 *    Disables bias current for battery type AND/OR temperature .
 *
 *     There is a risk that CHEN and CHIV bits could be changed by HW or other
 *   Triton functions (if called from ISR ) 
 *   Work around for this is to forbid modifcation of sensors while CHEN  or PWM  
 *   is enabled.
 *   So if enabled beforehand, then the sensors remain enabled and v.v.
 *   Dynamic control of the current supplies for battery and temperature
 *   should not be an issue w.r.t power saving when a charger is connected to 
 *   a charger.
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param  sensorType
 *     bit map of sensor types. bit 0 - battery type sensor; bit 1 temperature
 *
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
BspTwl3029_ReturnCode 
bspTwl3029_Bci_sensorsDisable( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                              BspTwl3029_Bci_sensorType  sensorType );
			      	  

/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_SetSkey
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *    BspTwl3029_Bci_skey skey 
 *    values for keys 1 - 5 set the charge-stop interrupt ( interrupt 1) to trigger
 *    in certain events
 *    A key6 value switchs off comparator It is mandatory to program a key6
 *    after a charge-stop interrupt ( i.e  after the PROTECt bit in SCISECU
 *    has been set by hardware event ) 
 *    Otherwise, the software cannot set CHEN, ACCSUPEN or PWM  bits.
 *    ( this bits are used to enable linear charged, accessory supply and PWM
 *      charge respectively) 
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */

BspTwl3029_ReturnCode 
bspTwl3029_Bci_SetSkey( BspTwl3029_I2C_CallbackPtr callbackInfoPtr, 
			  BspTwl3029_Bci_skey skey );
/*===========================================================================*/
/*!
 * @function bspTwl3029_Bci_SetWatchDog
 * <b> Description  </b><br>
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param 
 *   watchdogKey
 *     WDKEYs 1 - 4 sets watchdog timer to 1,2,4,or 8 s. 
 *     WDKEY 5 disables watchdog timer
 *     WDKEY 6 is  used to ack a watchdog interrupt. It is mandatory to program 
 *     a WDKEY 6 after a watchdog interrupt.
 *     Otherwise, the software cannot set CHEN, ACCSUPEN or PWM  bits again.
 *    ( this bits are used to enable linear charged, accessory supply and PWM
 *      charge respectively)  
 * @return 
 *   return of type BspTwl3029_ReturnCode
 *
 * @result
 *    
 */
 BspTwl3029_ReturnCode 
bspTwl3029_Bci_SetWatchDog( BspTwl3029_I2C_CallbackPtr callbackInfoPtr, 
			  BspTwl3029_Bci_wdkey watchdogKey );			  
#endif	
