/*=============================================================================
 * Copyright 2002-2004 Texas Instruments Inc. All rights reserved.
 */

/*=============================================================================
 * File Contents:
 *    Implementation for the W2FC USB Local Host device driver.
 */

//#include "bspUsblh.h"
#include "types.h"
#include "bspUsblh_Otg_llif.h"
#include "bspUsblh_Otg_map.h"
#include "bspI2c.h"
#include "bspUtil_BitUtil.h"
#include "string.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_UsbOtg.h"
#include "bspTwl3029_Power.h"


Bool bspUsblh_Otg_Init(void *notused)
{
    Bool retCode=TRUE;

// (*(volatile UINT16 *)BSP_LOCOSTO_USBCLK_REG)|=2; //make usb clock=52MHz
    BSPUTIL_BITUTIL_SET16(
			 BSP_LOCOSTO_USBCLK_REG_CLK_FREQUENCY,
			 BSP_LOCOSTO_USBCLK_REG_PTR,
	    		BSP_LOCOSTO_USBCLK_REG_ENABLE_52MHZ_CLK);

/*Configure  USB_VBUS_CTRL */
/* FIXME - Hack should be set and unset in Triton interrupt handler */
    BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_CTRL_USB_DISCONNECT);

/*Configure  USB_VBUS_MODE to use UIS480  pin for connect/disconnect event */
    BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_USB_VBUS_MODE_USE_USB_VBUS_CTRL_VALUE);

/*Configure USB_FILTER_CD as no filter */
    BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_USB_FILTER_CD_NO_FILTER);

/*COnfigure USB_PRT as available */
    BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_USB_PRT,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_USB_PORT_AVAILABLE);

/*COnfigure  USB_TRX_MODE in 4 pin mode */
    BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_USB_TRX_MODE_4_PIN);

/*REG_USBX_SYNCHRO to default value (1): One 48 MHz clock cycle delay is added to the output signals.*/
	BSPUTIL_BITUTIL_SET16(
            BSP_LOCOSTO_CORE_CONFIG_REG_USBX_SYNCHRO,
            BSP_LOCOSTO_CORE_CONFIG_REG_PTR,
            BSP_LOCOSTO_CORE_CONFIG_REG_USBX_SYNCHRO_NO_DELAY);

    return retCode;
}


extern void usb_otg_attach_hndlr(Uint8 srcId);
extern	void usb_otg_detach_hndlr(Uint8 srcId);

Bool bspUsblh_Otg_Register_Triton_Interrupt()
{
/* FIXME disbaling for now */
	BspTwl3029_ReturnCode rc;
	rc=bspTwl3029_UsbOtg_setHandler( BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD,
	                              usb_otg_attach_hndlr);
	rc=bspTwl3029_UsbOtg_setHandler( BSP_TWL3029_USBOTG_INT2_VD_SESS_END,
                              usb_otg_detach_hndlr);
	return rc;
}

Bool bspUsblh_Otg_Enable_Triton_Interrupt()
{
	#if 0
/* FIXME disbaling for now */
	BspTwl3029_ReturnCode rc;

rc=bspTwl3029_UsbOtg_enableIntSource( NULL,
                                   BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD,
                                   BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION); 
	rc=bspTwl3029_UsbOtg_enableIntSource( NULL,
                                   BSP_TWL3029_USBOTG_INT2_VD_SESS_END,
                                   BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION);
	return rc;
	#else
	Uint8 status;
	BspTwl3029_ReturnCode rc;
	rc=bspTwl3029_Power_SwitchOnStatus(&status);

	if(rc==BSP_TWL3029_RETURN_CODE_SUCCESS && status &(1<<BSP_TWL3029_POWER_USB_OFFSET))
	{
		/* Cable present */
		usb_otg_attach_hndlr(BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD);
	}
	return TRUE;
	#endif
}

Bool bspUsblh_Otg_Disable_Triton_Interrupt()
{
	#if 0
	BspTwl3029_ReturnCode rc;
	rc=bspTwl3029_UsbOtg_disableIntSource( NULL,
                                   BSP_TWL3029_USBOTG_INT2_VD_SESS_VLD,
                                   BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION); 

	rc=bspTwl3029_UsbOtg_disableIntSource( NULL,
                                   BSP_TWL3029_USBOTG_INT2_VD_SESS_END,
                                   BSP_TWL3029_USBOTG_INT_TRIGGER_ON_FALSE_TRUE_SIGNAL_TRANSITION);
	return rc;
	#endif
	return TRUE;
}


