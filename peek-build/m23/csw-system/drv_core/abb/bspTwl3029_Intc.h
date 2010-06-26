/*=============================================================================
 *    Copyright 2003 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_TWL3029_INTC_HEADER
#define BSP_TWL3029_INTC_HEADER

/*======================================
 * This is to fix the DRT OMAPS00087657
 */
#ifndef USB_HEADSET_HW_BUG
#define USB_HEADSET_HW_BUG 0x01
#endif

#define BSP_TWL3029_INTC_SOURCE_HOOK_MASK 0x40
#define BSP_TWL3029_INTC_SOURCE_HS_MASK 0x80
#define BSP_TWL3029_INTC_SOURCE_USB_VBUS_MASK 0x10
#define BSP_TWL3029_INTC_SOURCE_RTC_MASK 0x04

/*Second Bit in RTC STS reg is for alarm */
#define BSP_TWL3029_RTC_ALARM_MASK 0x40 
/*=============================================================================
 * Component Description:
 *    Interrupt Controller interface for the Analog Base Band (TWL3029) interupts.
 */
/*=============================================================================
 * Defines
 */

#define BSP_TWL3029_INTC_SOURCE_ID_NUM_SOURCES 16
 
#if 0 
/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply disables
 *   interrupts.
 */
#define BSP_TWL3029_INTC_CRITICAL_SECTION_ENTER() { BspIntC_LockState lockState = bspIntC_adjustLock( BSP_INTC_LOCKSTATE_LOCKED );

/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply restores
 *   interrupts to their previous state.
 */
#define BSP_TWL3029_INTC_CRITICAL_SECTION_EXIT() bspIntC_adjustLock( lockState ); }
#endif 
/*=============================================================================
 * Types
 */


/*===========================================================================*/
/*!
 * @typedef BspTwl3029_IntC_SourceId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type used for interrupt sources. Each interrupt controller
 *   implementation must define the valid list of source ids.
 */
enum
{
    BSP_TWL3029_INTC_SOURCE_ID_BCI_CHARGE_STOP         = 0,
    BSP_TWL3029_INTC_SOURCE_ID_VBUS_PRECHARGE_DETECT   = 1,
    BSP_TWL3029_INTC_SOURCE_ID_WD_OVERFLOW             = 2,
    BSP_TWL3029_INTC_SOURCE_ID_MADC_P1                 = 3,
    BSP_TWL3029_INTC_SOURCE_ID_MADC_P2                 = 4,
    BSP_TWL3029_INTC_SOURCE_ID_USB_D                   = 5, /* USB D+/D- Detection */
    BSP_TWL3029_INTC_SOURCE_ID_SIM                     = 6,
    BSP_TWL3029_INTC_SOURCE_ID_PW_FALL_EDGE            = 7,
    BSP_TWL3029_INTC_SOURCE_ID_EVENT                   = 8,
    BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER            = 9,
    BSP_TWL3029_INTC_SOURCE_ID_RTC                     = 10,
    BSP_TWL3029_INTC_SOURCE_ID_BATT_LOW                = 11,
    BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS             = 12, /* USB Vbus Detection */
    BSP_TWL3029_INTC_SOURCE_ID_THERM                   = 13, /* THERMISTOR */
    BSP_TWL3029_INTC_SOURCE_ID_AUD_HOOK                = 14, /* AUD hook dectect*/
    BSP_TWL3029_INTC_SOURCE_ID_AUD_HS                  = 15  /* AUD HEADSET plug/unplug */
   
};
typedef Uint16 BspTwl3029_IntC_SourceId;


#define BSP_TWL3029_INTC_SOURCE_ID_TO_MASK(_id) ( ((Uint8)0x00000001) << (_id) )




/*===========================================================================*/
/*!
 * @typedef BspTwl3029_IntC_Priority
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for assigning priority to interrupts that can happen
 *   simultaneously. A lower number means higher priority. 0 is the highest
 *   priority.
 */
#define BSP_TWL3029_INTC_PRIORITY_INVALID    (-1)

typedef Int16 BspTwl3029_IntC_Priority;




/*===========================================================================*/
/*!
 * @typedef BspTwl3029_IntC_Handler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for an interrupt service routine handler. It is
 *   a function that returns nothing and takes the source Id of the interrupt
 *   as a parameter.
 */
typedef void (*BspTwl3029_IntC_Handler)( BspTwl3029_IntC_SourceId sourceId );


/*=============================================================================
 * * @typedef BspTwl3029_IntC_HandlerTableEntry
 * 
 * <b> Description  </b><br>
 * @discussion
 *   This is the type for the internal structure holding the software
 *   configuration of the interrupt controller.
 *
 *   Note: Changing the size or the order of the parameters in this type may
 *         effect the dispatchInterrupt function if it is implemented in
 *         assembly.
 */
typedef struct {
    BspTwl3029_IntC_Handler handler;
} BspTwl3029_IntC_HandlerTableEntry;

/*=============================================================================
 * Creators
 */

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_init
 *
 * @discussion
 * <b> Description </b><br>
 *    Used to initialize the Analog Base Band Interrupt Controller.
 *
 * <b> Context </b><br>
 *    It must be called by the application. It must be called before any other
 *    application calls to bspTwl3029_IntC device driver functions. It may be called
 *    afterwards, but any existing configuration from other drivers will be
 *    reset.
 * 
 *    <b>Not Interrupt-Safe</b>
 *    <b>Not Thread-Safe</b>
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
bspTwl3029_IntC_init( void );

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_setHandler
 *
 * @discussion
 * <b> Description </b><br>
 *    This function maps (or unmaps with NULL) a handler to a specific system 
 *    controller interrupt source.
 *
 * <b> Context </b><br>
 *    It must be called by the application to set a handler ISR for each source
 *    before that source is first enabled. This function may be called after
 *    initialization, but before this function is called the interrupt source
 *    being configured must be disabled.
 * 
 *    Interrupt-Safe</br>
 *    Not Thread-Safe - The interrupt source being modified must be disabled.
 * 
 *  @param  sourceId
 *              This is the interrupt source being registered.
 * 
 *  @param  handler
 *              This is a pointer to the function to be called when the
 *              specified interrupt occurs. Use NULL to remove a handler.
 *
 *  @result <br>
 *    BspTwl3029_ReturnCode </br>
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_setHandler( BspTwl3029_IntC_SourceId    sourceId,
                            BspTwl3029_IntC_Handler     handler );

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_enableSource
 *
 * @discussion
 * <b> Description </b><br>
 *    This function enables a single system controller interrupt source.
 *
 * <b> Context </b><br>
 *    By default an interrupt source is not enabled after initialization or even
 *    after it has been configured. This function must be called to enable a
 *    particular interrupt source.
 * 
 *    Interrupt-Safe</br>
 *    Thread-Safe</br>
 * 
 *  @param  sourceId
 *              This is the interrupt source being enabled.
 * 
 *  @result <br>
 *    The interrupt will be enabled.
 *
 *  @return  <br>
 *    BspTwl3029_ReturnCode  </br>
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_enableSource( BspTwl3029_IntC_SourceId    sourceId );

/*=============================================================================
 */
/*!
 * @function bspTwl3029_IntC_disableSource
 *
 * @discussion
 * <b> Description </b><br>
 *    This function disables an interrupt source.
 *
 * <b> Context </b><br>
 *    This function must be called before changing any configuration information
 *    about a particular interrupt source. The only exception to that is after
 *    initialization since all interrupts are disabled after initialization.
 * 
 *    Interrupt-Safe</br>
 *    Thread-Safe</br>
 * 
 *  @param  sourceId
 *              This is the interrupt source being disabled.
 * 
 *  @param  sourceId
 *              This is the interrupt source being enabled.
 * 
 *  @result <br>
 *    The interrupt will be disabled.
 *
 *  @return  <br>
 *    BspTwl3029_ReturnCode  </br> 
 */
BspTwl3029_ReturnCode
bspTwl3029_IntC_disableSource( BspTwl3029_IntC_SourceId    sourceId );



BspTwl3029_ReturnCode
bspTwl3029_IntC_registerVBUSCallback( BspTwl3029_IntC_SourceId    sourceId,
                         BspTwl3029_IntC_Handler     handler );
#endif
