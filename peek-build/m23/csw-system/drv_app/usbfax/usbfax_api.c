/**
 * @file	usbfax_api.c
 *
 * Interface of the USBFAX SWE.
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	18/02/2004
 *
 * 
 */

#include "usbfax/usbfax_i.h"

extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;

static T_DIO_FUNC_USB	dio_func_usb = 
{
	usbfax_user_ready_usb,
	usbfax_user_not_ready_usb,
	usbfax_exit,
	usbfax_set_rx_buffer,
	usbfax_read,
	usbfax_write,
	usbfax_get_tx_buffer,
	usbfax_clear,
	usbfax_flush,
	usbfax_get_capabilities,
	usbfax_set_config,
	usbfax_get_config,
	usbfax_close_device
};

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 * @name dio_init_usb
 *
 * USB driver initialization
 *
 * @param	drv_handle			Unique handle for DIO drivers
 *
 * @return	DRV_OK				when initialization successful
 *			DRV_INITIALIZED		when driver is already initialized
 *			DRV_INITFAILURE		on failed initialization
 *
 */
/*@{*/
U16	dio_init_usb(T_DIO_DRV* drv_init)
{
	U16	ret_drv = DRV_OK;
	USBFAX_SEND_TRACE("USBFAX: dio_init_usb called ", RV_TRACE_LEVEL_DEBUG_LOW);

	return(ret_drv);
}
/*@}*/

/**
 * @name dio_export_usb
 *
 * USB driver function set export
 *
 * @param	dio_func	pointer to the list of functions exported by the driver
 *
 * @return	no value
 */
/*@{*/
void	dio_export_usb(T_DIO_FUNC** dio_func)
{
	*dio_func = (T_DIO_FUNC*)&dio_func_usb;
	
	return;
}
/*@}*/

/**
 * @name usbfax_getdio_sw_version
 *
 * comment
 *
 * @param
 *
 * @return
 */
/*@{*/
U32 usbfax_getdio_sw_version(void)
{
	return(0);
}
/*@}*/


