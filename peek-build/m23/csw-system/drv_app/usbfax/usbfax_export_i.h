/**
 * @file	usbfax_export_i.h
 *
 * Declarations of the Riviera Generic Functions
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	11/02/2004	
 */

#ifndef __USBFAX_EXPORT_I_H_
#define __USBFAX_EXPORT_I_H_

#include "usbfax/dio_usb.h"

/**
 * @name SWE exported functions
 *
 * Prototype imposed by DIO SWE
 *
 */
/*@{*/
void	usbfax_user_ready_usb(	U32				device_range_start, 
									U32				device_range_end, 
									U16				drv_handle, 
									T_DRV_CB_FUNC	signal_callback);

U16		usbfax_user_not_ready_usb(U32 device_range_start, U32 device_range_end);

void	usbfax_exit(void);

U16		usbfax_set_rx_buffer(U32 device, T_dio_buffer*	buffer);

U16		usbfax_read(U32 device, T_DIO_CTRL_LINES*	control_info, T_dio_buffer**	buffer);

U16		usbfax_write(U32 device, T_DIO_CTRL_LINES* control_info, T_dio_buffer* buffer);

U16		usbfax_get_tx_buffer(U32 device, T_dio_buffer**	buffer);

U16		usbfax_clear(U32 device);

U16		usbfax_flush(U32 device);

U16		usbfax_get_capabilities(U32 device, T_DIO_CAP_SER**	capabilities);

U16		usbfax_set_config(U32 device, T_DIO_DCB_SER* dcb);

U16		usbfax_get_config(U32 device, T_DIO_DCB_SER* dcb);

U16		usbfax_close_device(U32 device);
/*@}*/
#endif /*__USB_ENV_H_*/
