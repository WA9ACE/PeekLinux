/**
 * @file	dio_api.h
 *
 * API Definition for DIO SWE.
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

#ifndef __DIO_API_H_
#define __DIO_API_H_

#include "dio_il/dio_drv.h"

/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

/**
 * DIO API (bridge) Functions
 */

/**
 * @name T_DIO_FUNC_USB
 *
 * USB driver functions
 *
 *
 */
/*@{*/
typedef struct
{
	void	(*dio_user_ready_usb)		(U32 device_range_start, U32 device_range_end, U16 drv_handle, T_DRV_CB_FUNC signal_callback);
	U16		(*dio_user_not_ready_usb)	();
	void	(*dio_exit_usb)				();
	U16		(*dio_set_rx_buffer_usb)	(U32 device, T_dio_buffer* buffer);
	U16		(*dio_read_usb)				(U32 device, T_DIO_CTRL_LINES* control_info, T_dio_buffer** buffer);
	U16		(*dio_write_usb)			(U32 device, T_DIO_CTRL_LINES* control_info, T_dio_buffer* buffer);
	U16		(*dio_get_tx_buffer_usb)	(U32 device, T_dio_buffer** buffer);
	U16		(*dio_clear_usb)			(U32 device);
	U16		(*dio_flush_usb)			(U32 device);
	U16		(*dio_get_capabilities_usb)	(U32 device, T_DIO_CAP_SER**	capabilities);
	U16		(*dio_set_config_usb)		(U32 device, T_DIO_DCB_SER* dcb);
	U16		(*dio_get_config_usb)		(U32 device, T_DIO_DCB_SER* dcb);
	U16		(*dio_close_device_usb)		(U32 device);
} T_DIO_FUNC_USB;
/*@}*/

/**
 * @name dio_init_usb
 *
 *	USB driver initialization
 *
 * @param	T_DIO_DRV_USB*:	return_path		This is where the caller wants to be adressed
 *
 * @return	U16:		DRV_OK, DRV_INITIALIZED,DRV_INITFAILURE
 *
 */
/*@{*/
U16	dio_init_usb(T_DIO_DRV* drv_init);
/*@}*/

/**
 * @name dio_export_usb
 *
 *	USB driver initialization
 *
 * @param	T_DIO_FUNC_USB**:	dio_func		commment
 *
 * @return	no returned value
 *
 * last comment
 *
 */
/*@{*/
void	dio_export_usb(T_DIO_FUNC** dio_func);
/*@}*/


#endif /*__DIO_API_H_*/

