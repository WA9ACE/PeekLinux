/*=============================================================================
 *    Copyright 2004 Texas Instruments Incorporated. All Rights Reserved.
 */
 
#ifndef BSP_TWL3029_POWER_HEADER
#define BSP_TWL3029_POWER_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspTwl3029_Audio.h
 *   Public interface to the Twl3029 power  management subsystem.
 */

/*===========================================================================
 *  enums  and  typedefs 
 */

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_DevGrp
 *
 * @discussion
 * <b> Description </b><br>
 *     power DEV_GRP
 *
 * @constant BSP_TWL3029_POWER_GRP_MODEM
 *
 * @constant BSP_TWL3029_POWER_GRP_APP
 *
 * @constant BSP_TWL3029_POWER_GRP_PERIPH
 *
 */
enum
{
   BSP_TWL3029_POWER_DEV_GRP_MODEM = 1,
   BSP_TWL3029_POWER_DEV_GRP_APPLI = 2,
   BSP_TWL3029_POWER_DEV_GRP_PERIPH = 4
};
typedef Uint8 BspTwl3029_Power_DevGrp;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_MskEn
 *
 * @discussion
 * <b> Description </b><br>
 *     
 *
 * @constant BSP_TWL3029_POWER_MSKEN_ENABLE
 *
 * @constant BSP_TWL3029_POWER_MSKEN_DISABLE
 *
 */
enum
{
   BSP_TWL3029_POWER_MSKEN_DISABLE ,
   BSP_TWL3029_POWER_MSKEN_ENABLE 
};
typedef Uint8 BspTwl3029_Power_MskEn;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_resId
 *
 * @discussion
 * <b> Description </b><br>
 *     power resourse ID
 *
 */

typedef Uint8  BspTwl3029_Power_resId;  

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_State
 *
 * @discussion
 * <b> Description </b><br>
 *     power state
 *
 * @constant BSP_TWL3029_POWER_STATE_OFF
 *
 * @constant BSP_TWL3029_POWER_STATE_SLEEP
 *
 * @constant BSP_TWL3029_POWER_STATE_ACTIVE
 *
 */
enum
{
   BSP_TWL3029_POWER_STATE_OFF      = 0,
   BSP_TWL3029_POWER_STATE_SLEEP    = 8,
   BSP_TWL3029_POWER_STATE_ACTIVE   = 0xf
};
typedef Uint8  BspTwl3029_Power_State ; 


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_Dedicated
 *
 * @discussion
 * <b> Description </b><br>
 *     power dedicated settings for a power resource
 *
 */
typedef Uint8  BspTwl3029_Power_Dedicated  ;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_SwitchOn_Status
 *
 * @discussion
 * <b> Description </b><br>
 *     Switch on Cause.
 *
 */
enum
{
   BSP_TWL3029_POWER_PWON_OFFSET = 0,
   BSP_TWL3029_POWER_RPWON_OFFSET,
   BSP_TWL3029_POWER_CHG_OFFSET,
   BSP_TWL3029_POWER_USB_OFFSET,
   BSP_TWL3029_POWER_WAKEUP1_OFFSET,
   BSP_TWL3029_POWER_WAKEUP2_OFFSET,
   BSP_TWL3029_POWER_WAKEUP3_OFFSET,
   BSP_TWL3029_POWER_VBATOK_OFFSET
};
typedef Uint8 BspTwl3029_Power_SwitchOn_Status;


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_VrdbbVoltage
 *
 * @discussion
 * <b> Description </b><br>
 *     VRDBB voltage.
 *
 */
enum
{
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_0P95V = 0,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_0P98V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P01V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P04V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P07V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P10V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P13V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P16V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P19V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P22V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P25V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P28V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P31V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P34V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P37V,
   BSP_TWL3029_POWER_VRDBB_VOLTAGE_1P40V
};
typedef Uint8 BspTwl3029_Power_VrdbbVoltage;



/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_ResId
 *
 * @discussion
 * <b> Description </b><br>
 *      Used to identify individaul resourses
 *     
 * Note: The resources P2_CKLEN and BAT28 ( present in hardware ver Pg1.x only)
 *       are not included
 */
enum
{
   BSP_TWL3029_POWER_VRDBB,
   BSP_TWL3029_POWER_VRSIM,
   BSP_TWL3029_POWER_VRMMC,
   BSP_TWL3029_POWER_VREXTH,
   BSP_TWL3029_POWER_VRPLL,
   BSP_TWL3029_POWER_VRIO,
   BSP_TWL3029_POWER_VRMEM,
   BSP_TWL3029_POWER_VRUSB,
   BSP_TWL3029_POWER_VRABB ,
   BSP_TWL3029_POWER_VRVBUS ,
   BSP_TWL3029_POWER_VREXTL ,
   BSP_TWL3029_POWER_EXT_REGEN ,
   BSP_TWL3029_POWER_BAT32 ,

   BSP_TWL3029_POWER_BGAP ,
   BSP_TWL3029_POWER_BGSLP ,
   BSP_TWL3029_POWER_TSUTDOWN ,
   BSP_TWL3029_POWER_BIAS  , 
   BSP_TWL3029_POWER_CLKON  ,
   BSP_TWL3029_POWER_STATE_MNGT ,
   BSP_TWL3029_POWER_SLEEP_MNGT ,

   BSP_TWL3029_POWER_BAT_PRES_CHECK
};
typedef Uint8 BspTwl3029_Power_Id;



/*===========================================================================*/
/*!
 * @typedef BspTwl3029_Power_ResId
 *
 * @discussion
 * <b> Description </b><br>
 *     PWM message type
 *
 */
enum
{
   BSP_TWL3029_POWER_MESSAGE_TYPE_SINGULAR = 0,
   BSP_TWL3029_POWER_MESSAGE_TYPE_BROADCAST = 1
 };  
typedef Uint8  BspTwl3029_Power_MessageType;
/*===========================================================================
 *   defines and Macros  
 */

   
 #define RES_ID_OFFSET (1)
 #define CFG_STATE_OFFSET (2)
 #define CFG_DEDICATED_OFFSET (3)                                                                    
 /*==========================================================================*/


/*===========================================================================
 *  Structures
 */
 
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_PowerCfg
 *
 * @discussion
 *    A struct consistinig of field for the programming 
 *    of the cfg registers for each power source
 *    For exampe for VRMEM the config registers are:
 *    VRMEM_CFG_STS
 *    VRMEM_RES_ID
 *    VRMEM_CFG_STATE
 *    VRMEM_CFG_DEDICATED
 *
 * @field devGrp
 *    DEV_GRP : selects  MODEM, APPLICATION processor or pheripheral.
 *
 * @field mskEn
 *    mask option
 *
 * @field resId
 *    resourse ID
 *
 * @field offState
 *     defines off state
 *
 * @field sleepState
 *    defines sleep state
 *
 * @field dedic
 *    defines power resouces decicated settings
 *
 */
typedef struct 
{    
  BspTwl3029_I2C_RegId              regId;
  BspTwl3029_Power_DevGrp           devGrp;   
  BspTwl3029_Power_MskEn            mskEn; 
  BspTwl3029_Power_resId            resId;  
  BspTwl3029_Power_State            offState; 
  BspTwl3029_Power_State            sleepState;
  BspTwl3029_Power_Dedicated        dedic;

} BspTwl3029_PowerCfg;


/*===========================================================================
 *  Public functions
 */
 /*==========================================================================*/
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_init
 *
 * @discussion
 * <b> Description </b><br>
 *    Used to initialize the power management:
 *    - assign individual resources to DEV_GRPs
 *    - configure states for OFF and SLEEP power state machine states
 *    - perform any required decicated confiurgations for individual resources
 *    
 *    Note: the power FSM uses the default sequences that are hardcoded in ROM.
 *    and do not require any software configuration.  
 *
 * 
 *
 *  @param none None
 * 
 *  @result <br>
 *    The interrupt controller internal data is initialized ,all sources are
 *    disabled.
 *    BSP_TWL3029_RETURN_CODE_SUCCESS returned if there were no errors,
 *    otherwise BSP_TWL3029_RETURN_CODE_FAILURE is returned.
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_init( void );

#ifdef FEATURE_POWER_DISABLE_UNUSED_API
#else
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_enableSleep
 *
 * @discussion
 * <b> Description </b><br>
 *     Enables hardware triggered sleep transitions for the different device
 *     groups.
 * 
 *
 *  @param modem     TRUE if sleep transitions should be enabled for modem
 *                   device group, FALSE otherwise.
 *
 *  @param application    TRUE if sleep transitions should be enabled for
 *                        application device group, FALSE otherwise.
 *
 *  @param peripheral    TRUE if sleep transitions should be enabled for
 *                       application device group, FALSE otherwise.
 * 
 *  @result    Return code.
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_enableSleep( BspTwl3029_Power_DevGrp modem,
                              BspTwl3029_Power_DevGrp application,
                              BspTwl3029_Power_DevGrp peripheral );

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_setVrdbbSleepVoltage
 *
 * @discussion
 * <b> Description </b><br>
 *     Sets the VRDBB sleep mode voltage.
 * 
 *
 *  @param voltage
 * 
 *  @result    Return code.
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_setVrdbbSleepVoltage( BspTwl3029_Power_VrdbbVoltage voltage );
#endif

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_enable
 *
 * @discussion
  * <b> Description </b><br>
 *    Enable /disable a power resource by sending an power bus command 
 *    over I2C interface
 *
 *    this is for use with LDOs that need to be dynamicaly enabled /disabled
 *    LDOS: USB, SIM and MMC 
 *
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param BspTwl3029_Power_Id
 *   powr resouce
 * 
 * @param state
 *    on off or sleep
 *
 *  @result    Return code.        
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_enable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr, 
                        BspTwl3029_Power_Id resource, 
			BspTwl3029_Power_State state);

#ifdef FEATURE_POWER_DISABLE_UNUSED_API
#else
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_getCfg_Sts
 *
 * @discussion
 * <b> Description </b><br>
 *    reads  CFG_STS register for a given power resource. 
 *    
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param BspTwl3029_Power_Id
 *   powr resouce
 * 
 * @param state
 *    on off or sleep
 *
 *  @result    BspTwl3029_ReturnCode.               
 */
 BspTwl3029_ReturnCode
bspTwl3029_Power_getCfg_Sts(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                             BspTwl3029_Power_Id powerSource,
			     BspTwl3029_I2C_RegData *cfgStatus  );
/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_I2cAccessDisable
 *
 * @discussion
 * <b> Description </b><br>
 *    disables access to power bus.
 *    Access to power bus is automatically enabled by bspTwl3029_Power_enable 
 *   command. 
 *    
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param BspTwl3029_Power_Id
 *   powr resouce
 * 
 * @param state
 *    on off or sleep
 *
 *  @result    BspTwl3029_ReturnCode.            
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_I2cAccessDisable(BspTwl3029_I2C_CallbackPtr callbackInfoPtr );
#endif

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_setDevGrp
 *
 * @discussion
 * <b> Description </b><br>
 *    Assigns a power resource to resource group(s)
 *    Note: if powerSource is set to zero then resource is unassigned from
 *    dev group
 *   
 * @param callbackPtr
 *    pointer to struct containing info for set up of I2C callback transaction
 *
 * @param powerSource
 *   power resource
 * 
 * @param group
 *    dev group ( none, Modem, application, peripheral )
 *
 *  @result    BspTwl3029_ReturnCode.          
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_setDevGrp(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                            BspTwl3029_Power_Id powerSource, 
			    BspTwl3029_Power_DevGrp group);

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_SwitchOnStatus
 *
 * @discussion
 * <b> Description </b><br>
 *    Reads the Hardware Status for the cause of Switch on.
 *   
 *
 *  @result    Status Byte
 */
BspTwl3029_ReturnCode
bspTwl3029_Power_SwitchOnStatus(Uint8 *status);

BspTwl3029_ReturnCode
bspTwl3029_Power_SwitchOnStatusWithCallback(Uint8 *status,BspI2c_TransactionDoneCallback callbackPtr);

/*=============================================================================
 */
/*!
 * @function bspTwl3029_Power_SwitchOff
 *
 * @discussion
 * <b> Description </b><br>
 *    Power Off the System
 *   
 *
 */
void
bspTwl3029_Power_SwitchOff(void);
BspTwl3029_ReturnCode bspTwl3029_Usb_biascell_on(BspTwl3029_I2C_CallbackPtr callbackptr);
BspTwl3029_ReturnCode bspTwl3029_Usb_biascell_off(BspTwl3029_I2C_CallbackPtr callbackptr);


#endif
