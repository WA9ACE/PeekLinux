/*=============================================================================
 * Copyright 2003 Texas Instruments Incorporated. All Rights Reserved.
 */
#ifndef BSP_TWL3029_USBOTG_HEADER
#define BSP_TWL3029_USBOTG_HEADER
 
#if (ANLG_PG == 1)
#include "pg1x/bspTwl3029_UsbOtg_pg1x.h"
#elif (ANLG_PG == 2)
#include "pg2x/bspTwl3029_UsbOtg_pg2x.h"
#endif 

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspTwl3029_UsbOtg.h
 *   Public interface to the Twl3029 USB hardware subsystem.
 */
/*===========================================================================
 *  Defines and Macros
 */
 /*==========================================================================*/
 /*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 */
 enum
{
    BSP_TWL3029_USB_OTG_REG_BIT_LOW,
    BSP_TWL3029_USB_OTG_REG_BIT_HIGH     
};
typedef Uint8 bspTwl3029_UsbOtgBitVal;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_UsbOtgEnable
 *
 * @discussion 
 * <b> Description </b><br>
 *      
 * @constant BSP_TWL3029_USBOTG_DISABLE
 *
 * @constant BSP_TWL3029_USBOTG_ENABLE
 *
 */
enum
{
   BSP_TWL3029_USBOTG_DISABLE = 0,
   BSP_TWL3029_USBOTG_ENABLE  = 1

};
typedef Uint8 BspTwl3029_UsbOtg_Enable;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_UsbOtg_pwrResource
 *
 * @discussion 
 * <b> Description </b><br>
 *      
 * @constant BSP_TWL3029_USBOTG_USBA
 *           analog
 *
 * @constant BSP_TWL3029_USBOTG_USBD
 *           digital
 *
 */
enum
{
   BSP_TWL3029_USBOTG_USBD = 1,
   BSP_TWL3029_USBOTG_USBA = 2

};
typedef Uint8 BspTwl3029_UsbOtg_pwrResource;
/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_UsbOtgRegUpdateInfo
 *
 * @discussion 
 * <b> Description </b><br>
 *      regBitOffset - register address ( from address lister is map file)
 *
 *      regBitWidth - field width within a reg ( from those listed in Llif file) 
 *
 *      regBitOffset - field offset within a reg ( from those listed in Llif file) 
 */
typedef struct
{
    Uint8                      regBitOffset;
    Uint8                      regBitWidth;
    BspTwl3029_I2C_RegId       registerId;
} BspTwl3029_UsbOtgRegUpdateInfo;


 
 
 /*=============================================================================
 * Macro :  bspTwl3029_UsbOtg_configTransceiver_Bit
 *
 * Description:
 *    update a bit in one of the USB registers by calling the function
 *    bspTwl3029_UsbOtg_configTransceiverField
 *
 * Inputs:
 *   callbackInfoPtr - pointer to struct containing callback info
 *   reg - reg name. - same as names specifed in hardware documentation or
 *         spreadsheet. This name is expanded by the macro to match a register
 *          name defined in bspTwl3029_Aux_Map.h
 *   bitOffset: field name. This name is expanded by the macro to match an OFFSET
 *          name defined in bspTwl3029_Aux_Llif.h
 *
 *  Notes:  
 */
#define bspTwl3029_UsbOtg_configTransceiver_Bit( callbackInfoPtr,   \
                                    reg,                         \
                                    bitOffset,                   \
                                    val )                        \
{	                                                         \
      bspTwl3029_UsbOtg_configTransceiverBit( callbackInfoPtr,     \
      BSP_TWL3029_MAP_USB_## reg ##_OFFSET,                      \
      BSP_TWL3029_LLIF_USB_## reg ##_ ## bitOffset ##_OFFSET,val );  \
}                                                                            

/*=============================================================================
 * Macro :  bspTwl3029_UsbOtg_configTransceiver_Field
 *
 * Description:
 *    update a field in one of the USB registers by calling the function
 *    bspTwl3029_UsbOtg_configTransceiverField
 *
 * Inputs:
 *   callbackInfoPtr - pointer to struct containing callback info
 *
 *   arrayPtr - pointer to a atruct which filled by the macro before calling 
 *               the function
 *   reg - reg name. - same as names specifed in hardware documentation or
 *         spreadsheet. This name is expanded by the macro to match a register
 *          name defined in bspTwl3029_Aux_Map.h
 *   field field name. This name is expanded by the macro to match a register
 *          name defined in bspTwl3029_Aux_Llif.h
 *
 *  Notes:  
 */
#define bspTwl3029_UsbOtg_configTransceiver_Field( callbackInfoPtr,   \
                                    reg,                              \
                                    field,                            \
                                    val )                             \
{	                                                              \
      BspTwl3029_UsbOtgRegUpdateInfo array;                           \
      BspTwl3029_UsbOtgRegUpdateInfo* arrayPtr = &array;              \
      arrayPtr->registerId =  BSP_TWL3029_MAP_USB_## reg ##_OFFSET;   \
      arrayPtr->regBitOffset =  BSP_TWL3029_LLIF_USB_## reg ##_ ## field ##_OFFSET ; \
      arrayPtr->regBitWidth   = BSP_TWL3029_LLIF_USB_## reg ##_ ## field ##_WIDTH ; \
          \
      bspTwl3029_UsbOtg_configTransceiverField( callbackInfoPtr,  arrayPtr,val);   \
}  

/*=============================================================================
 * Macro :  bspTwl3029_UsbOtg_getTransceiverStatus_Reg
 *
 * Description:
 *    Read an 8 bit USB status register
 *
 * Inputs:
 *   callbackInfoPtr - pointer to struct containing callback info
 *
 *   reg - reg name. - same as names specifed in hardware documentation or
 *         spreadsheet. This name is expanded by the macro to match a register
 *          name defined in bspTwl3029_Aux_Map.h
 *         regDataPtr. Pointer to location ( thype Uint8) where the read data is to 
           be stored
 *
 *  Notes:  
 */ 
#define bspTwl3029_UsbOtg_getTransceiverStatus_Reg( callbackInfoPtr,  \
                                          reg,                        \
					  regDataPtr )                \
{                                                                     \
    BspTwl3029_I2c_ReadSingle(BSP_TWL3029_I2C_USB,                    \
                              BSP_TWL3029_MAP_USB_## reg ##_OFFSET,   \
                              regDataPtr, callbackInfoPtr );          \
} 
  
 
/*============================================================================*/
/*!
 * @constant
 *
 * @discussion 
 * <b> Description </b><br>
 *    
 */
 # define CONFIG_ID_MAX 24
 
/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbOtg_IntHandler
 *
 * @discussion
 *     Interrupt handler for handling usb itg transceiver events.
 *
 * @param intId
 *    Identifies the interrupt source which triggered the interrupt.
 *
 * @return NOTHING.
 */
typedef void (*BspTwl3029_UsbOtg_IntHandler)( BspTwl3029_UsbOtg_InterruptId intId );

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbOtg_ReturnCode
 *
 * @discussion
 *    Defines the return code for the driver apis.
 *
 * @constant BSP_TWL3029_USB_OTG_RETURN_CODE_FAILURE
 *    Indicates the driver operation failed.
 *
 * @constant BSP_TWL3029_USB_OTG_RETURN_CODE_SUCCESS
 *    Indicates the driver operation was successfull.
 */
enum
{
    BSP_TWL3029_USB_OTG_RETURN_CODE_SUCCESS       = 0,
    BSP_TWL3029_USB_OTG_RETURN_CODE_FAILURE       = (-1)
    
};
typedef Int16 BspTwl3029_UsbOtg_ReturnCode;


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbOtg_IntEnable
 *
 * @discussion
 *    Defines the return code for the driver apis.
 *
 * @constant BSP_TWL3029_INTC_INT_DISABLED
 *    Indicates the interrupt is disabled.
 *
 * @constant BSP_TWL3029_INTC_INT_ENABLED
 *    Indicates the interrupt is enabled.
 */
enum
{
    BSP_TWL3029_INTC_INT_DISABLED       = 0,
    BSP_TWL3029_INTC_INT_ENABLED        = 1
    
};
typedef Int16 BspTwl3029_UsbOtg_IntEnable;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbVendorRegister
 *
 * @discussion
 * <b> Description </b><br>
 */
typedef Uint16 BspTwl3029_UsbVendorRegister;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbIdRegister
 *
 * @discussion
 * <b> Description </b><br>
 */
typedef Uint16 BspTwl3029_UsbIdRegister;

/*===========================================================================*/
/*!
 * @typedef BspTwl3029_UsbOtg_RegModifyInfo
 *
 * @discussion
 * <b> Description </b><br>
 *    Defines which bits in a register to be modified
 *    and values that they are to be modified to.
 *
 * @field usbOtgReg
 *     specifies a register
 *
 * @field usbOtgRegFieldBits
 *    Indicates bits in field to be modified.
 *
 * @field usbOtgRegFieldVal
 *    Indicates the values that bits are to be modified to. 
 */
typedef struct  bitFieldInfo
{
   BspTwl3029_I2C_RegId  usbOtgReg;
   Uint8                 usbOtgRegFieldBits;
   Uint8                 usbOtgRegFieldVal;
}BspTwl3029_UsbOtg_RegModifyInfo;

/*==========================================================================*/
 /*!
 * @typedef  BspTwl3029_UsbOtgPullControl
 *
 * @discussion 
 * <b> Description </b><br>
 *      
 * @constant BSP_TWL3029_USB_SET_PULL
 *
 * @constant BSP_TWL3029_USB_CLEAR_PULL
 *
 */
enum {
	BSP_TWL3029_USB_SET_PULL=0,
	BSP_TWL3029_USB_CLEAR_PULL=1
};
typedef Uint8 BspTwl3029_UsbOtgPullControl;

/*==============================================================================*/
/*!
 *   FUNCTIONS
 */
/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_PullControl
 *
 * @discussion
 * <b> Description </b><br>
 *    Control DP PULL.
 *
 * @param BspTwl3029_UsbOtgPullControl.
 *
 * @return Nothing.
 */
 void bspTwl3029_UsbOtg_PullControl(BspTwl3029_UsbOtgPullControl id);



/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_init
 *
 * @discussion
 * <b> Description </b><br>
 *    Initializes the driver.
 *
 * @param Nothing.
 *
 * @return Nothing.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_init(void);


/*=============================================================================
 *!
 * @function   bspTwl3029_UsbOtg_configTransceiverBit
 *
 * @discussion
 * <b> Description </b><br>
 *    update a bit in one of the USB registers 
 *
 * @param callbackInfoPtr 
 *       pointer to sruct containig callback info
 *
 * @param  reg    
 *         BspTwl3029_I2C_RegId  register address
 *
 * @param bitOffset       
 *          register bit
 *
 * @param  val    
 *         BspTwl3029_I2C_RegData    value ( 0 or 1)
 *
 * @return       BspTwl3029_ReturnCode
 *  
 *  Notes:  
 */

BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_configTransceiverBit(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                    BspTwl3029_I2C_RegId reg, 
				    Uint8 bitOffset, 
				    BspTwl3029_I2C_RegData val );
 /*=============================================================================
 * @function  bspTwl3029_UsbOtg_configTransceiverField
 *
 * @discussion
 * <b> Description </b><br>
 *    update a field in one of the USB registers 
 *
 * @param   callbackInfoPtr
 *     pointer to struct containig callback info
 *
 * @param  arrayPtr  
 *        pointer to struct containing reg address, bit field and offset
 *
 * @param  val     
 *           value  which field is to be updated to
 *
 * @returns   BspTwl3029_ReturnCode
 *            
 *  
 *  Notes:  
 */

BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_configTransceiverField(BspTwl3029_I2C_CallbackPtr callbackInfoPtr,  
                                        BspTwl3029_UsbOtgRegUpdateInfo* arrayPtr,
					BspTwl3029_I2C_RegData val);



/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_enableIntSource
 *
 * @discussion
 * <b> Description </b><br>
 *    Enables the interrupt for the usb otg transceiver block.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 * @param triggerType
 *    Defines the interrupt signal transition to trigger an interrupt.
 *
 * @result
 *    returns BSP_TWL3029_USB_OTG_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_USB_OTG_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_enableIntSource( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                   BspTwl3029_UsbOtg_InterruptId    intSrcId,
                                   BspTwl3029_UsbOtgIntTriggerType  triggerType );


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_disableIntSource
 *
 * @discussion
 * <b> Description </b><br>
 *    Disables the interrupt for the usb otg transceiver block.
 *
 * @param intSrcId
 *    The interrupt to enable.
 *
 * @param triggerType
 *    Defines the interrupt signal transition to trigger an interrupt.
 *
 * @result
 *    returns BSP_TWL3029_USB_OTG_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_USB_OTG_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_disableIntSource(  BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                                     BspTwl3029_UsbOtg_InterruptId    intSrcId,
                                     BspTwl3029_UsbOtgIntTriggerType  triggerType );


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_setHandler
 *
 * @discussion
 * <b> Description </b><br>
 *    Registers the client handler for an interrupt.
 *
 * @param intSrcId
 *    The interrupt for which the handler is to be registered.
 *
 * @param handler
 *    The handler to register.
 *
 * @result
 *    returns BSP_TWL3029_USB_OTG_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_USB_OTG_RETURN_CODE_FAILURE on failure.
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_setHandler( BspTwl3029_UsbOtg_InterruptId intSrcId,
                              BspTwl3029_UsbOtg_IntHandler  handler );


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_getVendorId
 *
 * @discussion
 * <b> Description </b><br>
 *    Gets the usb otg vendor id.
 *
 * @param Nothing
 *
 * @return
 *    The vendor id.
 */
BspTwl3029_UsbVendorRegister
bspTwl3029_UsbOtg_getVendorId(void);


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_getProductId
 *
 * @discussion
 * <b> Description </b><br>
 *    Gets the usb otg product id.
 *
 * @param Nothing
 *
 * @return
 *    The product id.
 */
BspTwl3029_UsbIdRegister
bspTwl3029_UsbOtg_getProductId(void);

/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_set( BspTwl3029_UsbOtg_SetClrReg reg )
 *
 * @discussion
 * <b> Description </b><br>
 *    sets one or more features for a given  internal register accesable
 *    via I2C by SEt/ CRL /STATUS type registers.
 *    Note: this function cannot be used for registers related to interrupt
 *    All of the interrrupt registers are accessed thru the twl3029UsbOtg 
 *    or twl3029_IntC APIs dedicated to interrupt handling.
 *
 * @param BspTwl3029_UsbOtg_SetClrReg
 *    neame of register 
 *
 * @param val
 *    bit map of features to be set.
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_set( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                       BspTwl3029_UsbOtg_SetClrReg  reg,
		       BspTwl3029_UsbOtg_SetClrVal val );


/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_clr( BspTwl3029_UsbOtg_SetClrReg reg )
 *
 * @discussion
 * <b> Description </b><br>
 *    clrears one or more features for a given  internal register accesable
 *    via I2C by SEt/ CRL /STATUS type registers.
 *    Note: this function cannot be used for registers related to interrupt
 *    All of the interrrupt registers are accessed thru the twl3029UsbOtg 
 *    or twl3029_IntC APIs dedicated to interrupt handling.
 *
 * @param BspTwl3029_UsbOtg_SetClrReg
 *    neame of register 
 *
 * @param val
 *    bit map of features to be cleared.  
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode
bspTwl3029_UsbOtg_clr( BspTwl3029_I2C_CallbackPtr callbackInfoPtr,
                       BspTwl3029_UsbOtg_SetClrReg  reg,
		       BspTwl3029_UsbOtg_SetClrVal val ); 

 
/*==============================================================================*/
/*!
 * @function bspTwl3029_UsbOtg_clr( BspTwl3029_UsbOtg_SetClrReg reg )
 *
 * @discussion
 * <b> Description </b><br>
 *    switchs on/off analog and /or digital parts of USB transceiver
 *
 * @param callbackFuncPtr
 *    enable 
 *
 * @param val
 *    bit map of usb power to be enbled/disabled ( analog and /or digital).  
 *
 * @result
 *    returns BSP_TWL3029_RETURN_CODE_SUCCESS on success
 *            BSP_TWL3029_RETURN_CODE_FAILURE on failure.
 *
 */
BspTwl3029_ReturnCode 
bspTwl3029_UsbOtg_TransceiverEnable(BspTwl3029_I2C_CallbackPtr callbackFuncPtr,
                                    BspTwl3029_UsbOtg_Enable enable,
				    BspTwl3029_UsbOtg_pwrResource pwr);
#endif
