/**
 * @file	usbfax_export.c
 *
 * Implementation of USB exported functions,
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
 */

#include <string.h>

#include "usbfax/usbfax_i.h"
#include "usbfax/usbfax_cfg.h"
#include "usbfax/usbfax_env.h"
#include "usbfax/usbfax_task_i.h"

extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;

/**
 * @name usbfax_user_ready_usb
 *
 * User synchronization
 *
 * @param	device_range_start		First device number of a range of device
 *									numbers which can be handled by a DIO user now.
 *			device_range_end		Last device number of a range of device numbers
 *									which can be handled by a DIO user now.
 *			drv_handle				Unique handle of the DIO user
 *			signal_callback			This parameter points to the function that is called 
 * 									at the time an event occurs that is to be signaled.
 *
 * @return NO VALUE
 */
/*@{*/
void	usbfax_user_ready_usb(U32 device_range_start, U32 device_range_end, U16 drv_handle, T_DRV_CB_FUNC signal_callback)
{
	U16		ret_drv = DRV_OK;
	U32		device_index	= 0;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_user_ready_usb called ", RV_TRACE_LEVEL_DEBUG_LOW);

	T_USBFAX_USB_DEVICE		*current_device = NULL;
	if (usbfax_env_ctrl_blk_p==NULL)
		return;
	usbfax_env_ctrl_blk_p->dvr_handle		= drv_handle;
	usbfax_env_ctrl_blk_p->signal_callback	= signal_callback;

	/*
	 * for each device identifier in range [device_range_start, device_range_end]
	 * indicate device is connected with a DIO user
	 */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device = &USBFAX_TAB_OF_DEVICES[device_index];

		if((device_range_start <= current_device->device_number)
			&& (current_device->device_number <= device_range_end))
		{
			current_device->user_connected = TRUE;
			current_device->dtr_host_connected	= FALSE;
		}
/* to be commented if not working - sent connect indication at host connect */
/*		if(current_device->usb_line_state.host_dtr & 1)
		{
			* Notify DIO that device is connected with the host */
/*			usbfax_signal(current_device, DRV_SIGTYPE_CONNECT);
		}*/
	}

	return;
}
/*@}*/

/**
 * @name usbfax_user_not_ready_usb
 *
 * Termination of User Operation
 *
 * @param	device_range_start		First device number of a range of device
 *									numbers which can be handled by a DIO user now.
 *			device_range_end		Last device number of a range of device numbers
 *									which can be handled by a DIO user now.
 *
 * @return	DRV_OK					User operation successfully terminated.
 *			DRV_INVALID_PARAMS		User operation can not be terminated yet.
 *			DRV_INTERNAL_ERROR		Internal driver error.
 *
 */
/*@{*/
U16	usbfax_user_not_ready_usb(U32 device_range_start, U32 device_range_end)
{
	U32						device_index	= 0;
	T_USBFAX_USB_DEVICE		*current_device = NULL;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_user_not_ready_usb called ", RV_TRACE_LEVEL_DEBUG_LOW);

	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device = &USBFAX_TAB_OF_DEVICES[device_index];

		if((device_range_start <= current_device->device_number)
			&& (current_device->device_number <= device_range_end))
		{
			current_device->user_connected = FALSE;
		}
	}

	return(DRV_OK);
}
/*@}*/

/**
 * @name dio_exit 
 *
 * implement termination of the Interface with USBFAX
 *
 *
 * @return	NONE
 */
/*@{*/
void	usbfax_exit(void)
{
//	USBFAX_SEND_TRACE("USBFAX: usbfax_exit called ", RV_TRACE_LEVEL_DEBUG_LOW);
	/* reinitialize of handle of the DIO driver */
	if (usbfax_env_ctrl_blk_p == NULL)
		return;
	usbfax_env_ctrl_blk_p->dvr_handle		= NULL;

	/* reinitialize signal callback function */
	usbfax_env_ctrl_blk_p->signal_callback	= NULL;
}
/*@}*/

/**
 * @name usbfax_set_rx_buffer
 *
 * Provide a receive buffer to the driver to the USBFAX driver
 *
 * @param device:	Data device number
 * @param buffer:	pointer to a buffer description
 *
 * @return		:	returns DRV_OK				when successfully executed this function
 *					returns DRV_BUFFER_FULL		when read requests queue is full.
 *					returns DRV_INVALID_PARAMS	when the specified device does not exist 
 *												or the pointer data buffer is NULL.
 *					returns DRV_NOTCONFIGURED	when the device is not yet configured.
 *					returns DRV_INTERNAL_ERROR	on internal driver error.
 *
 */
/*@{*/
U16	usbfax_set_rx_buffer(U32 device, T_dio_buffer*	buffer)
{
	U16	ret_drv = DRV_OK;

	/* pointer to the associated device */
	T_USBFAX_USB_DEVICE		*current_device_p = NULL;

	/* read request to queue */
	T_USBFAX_XFER			xfer_request_p;

	usbfax_lock();

	USBFAX_SEND_TRACE("USBFAX: usbfax_set_rx_buffer called ", RV_TRACE_LEVEL_DEBUG_LOW);

	if(buffer != NULL)
	{
		/* Look for associated device */
		current_device_p = usbfax_get_device(device);
		if(current_device_p != NULL)
		{
			/* Check if device is configured */
			if((current_device_p->user_connected == TRUE)
				&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
			{
				/* fill request structure data */
				xfer_request_p.buffer_context = buffer;
				/* xfer_request_p.line_control	= NULL; */
				xfer_request_p.segment		= NULL;
				xfer_request_p.segment_size	= 0;
				xfer_request_p.sent_length	= 0;
				xfer_request_p.total_sent_length = 0;
				xfer_request_p.segment_index = 0;

				xfer_request_p.line_control.control_type	= 0;
				xfer_request_p.line_control.length			= 0;
				xfer_request_p.line_control.state			= 0;

				/*initialize memory validity padding*/
				xfer_request_p.c_align1 = 0;
				xfer_request_p.c_align2 = 0;

				xfer_request_p.usb_posted = FALSE;

				/* try to queue read request */
				if(usbfax_enqueue_request(device, USBFAX_XFER_READ_REQUEST, &xfer_request_p)
					!= RVM_OK)
				{
					/* read requests queue  is full */
					ret_drv = DRV_BUFFER_FULL;
				}
			}
			else
			{
				/* the device is not yet configured */
				ret_drv = DRV_INVALID_PARAMS;
			}
		}
		else
		{
			/* the specified device does not exist */
			ret_drv = DRV_INVALID_PARAMS;
		}
	}
	else
	{
		/* pointer to data buffer is NULL */
		USBFAX_SEND_TRACE("USBFAX: usbfax_set_rx_buffer NULL buffer provided ", RV_TRACE_LEVEL_DEBUG_LOW);
		ret_drv = DRV_INVALID_PARAMS;
	}

	usbfax_unlock();

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_read
 *
 * Get filled receive buffer
 *
 * @param device:		Data device number
 * @param control_info:	control information of the device.
 * @param buffer:		pointer to a buffer description
 *
 * @return		:	returns DRV_OK				when successfully executed this function
 *					returns DRV_INVALID_PARAMS	when the specified device does not exist 
 *												or the pointer data buffer is NULL.
 *					returns DRV_NOTCONFIGURED	when the device is not yet configured.
 *					returns DRV_INTERNAL_ERROR	when internal driver error.
 */
/*@{*/
U16	usbfax_read(U32	device, T_DIO_CTRL_LINES*	control_info, T_dio_buffer**	buffer)
{
	U16	ret_drv = DRV_OK;

	/* pointer to the associated device */
	T_USBFAX_USB_DEVICE*	current_device_p = NULL;

	/* read request to queue */
	T_USBFAX_XFER*		xfer_request_p = NULL;

	USBFAX_SEND_TRACE("USBFAX: usbfax_read called ", RV_TRACE_LEVEL_DEBUG_LOW);
	/* Look for associated device */
	current_device_p = usbfax_get_device(device);
	if(current_device_p != NULL)
	{
		/* Check if device is configured */
		if((current_device_p->user_connected == TRUE)
			&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
		{
			if( NULL != control_info )
			{
				usbfax_update_control_line(current_device_p, control_info);
			}
			/* Get request to dequeue */
			xfer_request_p = usbfax_get_queue_element(device, USBFAX_XFER_READ_REQUEST);
			
			if(xfer_request_p)
			{
				if( ( xfer_request_p->line_control.state & DIO_ESC)&& ( NULL != control_info ) )
				{
					USBFAX_SEND_TRACE("USBFAX: DIO_ESC Bitmask set for PSI", RV_TRACE_LEVEL_DEBUG_LOW);
					control_info->state |= DIO_ESC;
				}

				/* Update length of read data in the dio buffer */
				xfer_request_p->buffer_context->length = xfer_request_p->total_sent_length;
				
				*buffer = xfer_request_p->buffer_context;

				if(usbfax_dequeue_request(device, USBFAX_XFER_READ_REQUEST)
					!= RVM_OK)
				{
					/* the queue is empty */
					ret_drv = DRV_INTERNAL_ERROR;
				}
			}
			else
			{
				/* No buffer to return */
				USBFAX_SEND_TRACE("USBFAX: usbfax_read no buffer to return ", RV_TRACE_LEVEL_DEBUG_LOW);
				buffer = NULL;
			}
		}
		else
		{
			/* the device is not yet configured */
			ret_drv = DRV_INVALID_PARAMS;
		}
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_write
 *
 * Provide data to send
 *
 * @param device:		Data device number
 * @param control_info:	control information of the device.
 * @param buffer:		pointer to a buffer description
 *
 * @return		:	returns DRV_OK				when successfully executed this function
 *					returns DRV_BUFFER_FULL		when write requests queue is full.
 *					returns DRV_INVALID_PARAMS	when the specified device does not exist 
 *												or the pointer data buffer is NULL.
 *					returns DRV_NOTCONFIGURED	when the device is not yet configured.
 *					returns DRV_INTERNAL_ERROR	on internal driver error.
 *
 */
/*@{*/
U16	usbfax_write(U32 device, T_DIO_CTRL_LINES* control_info, T_dio_buffer* buffer)
{
	U16	ret_drv = DRV_OK;

	T_USBFAX_USB_DEVICE*	current_device_p = NULL;
	T_USBFAX_XFER		xfer_request_p;

	usbfax_lock();

	USBFAX_SEND_TRACE("USBFAX: usbfax_write called ", RV_TRACE_LEVEL_DEBUG_LOW);

	if((control_info != NULL) || (buffer != NULL))
	{
		USBFAX_SEND_TRACE("USBFAX: usbfax_write process buffer ", RV_TRACE_LEVEL_DEBUG_LOW);
		current_device_p = usbfax_get_device(device);
	
		if ((control_info != NULL) && (buffer != NULL))
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_write Data and Control info to handle ", RV_TRACE_LEVEL_DEBUG_LOW);
		}
		if (control_info == NULL)
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_write Data only to handle ", RV_TRACE_LEVEL_DEBUG_LOW);
		}

		if (buffer == NULL)
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_write Control info only to handle ", RV_TRACE_LEVEL_DEBUG_LOW);
		}


		if(current_device_p != NULL)
		{
			/* Check if device is configured */
			if((current_device_p->user_connected == TRUE)
				&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
			{
				/* fill request structure data */
				xfer_request_p.buffer_context = buffer;
				if(control_info != NULL)
				{
					/* Copy line control information from the provided control buffer */
					xfer_request_p.line_control.control_type	= control_info->control_type;
					xfer_request_p.line_control.length			= control_info->length;
					xfer_request_p.line_control.state			= control_info->state;
				}
				else
				{
					xfer_request_p.line_control.control_type	= 0;
					xfer_request_p.line_control.length			= 0;
					xfer_request_p.line_control.state			= 0;
				}
				
				xfer_request_p.segment		= NULL;
				xfer_request_p.segment_size	= 0;
				xfer_request_p.sent_length	= 0;
				xfer_request_p.total_sent_length	= 0;
				xfer_request_p.segment_index = 0;

				xfer_request_p.c_align1 = 0;
				xfer_request_p.c_align2 = 0;

				/* try to queue write request */
				if(usbfax_enqueue_request(device, USBFAX_XFER_WRITE_REQUEST, &xfer_request_p)
					!= RVM_OK)
				{
					/* write requests queue  is full */
					ret_drv = DRV_BUFFER_FULL;
				}
			}
			else
			{
				/* the device is not yet configured */
				ret_drv = DRV_INVALID_PARAMS;
			}

		}
		else
		{
			ret_drv = DRV_INVALID_PARAMS;
		}
	}
	else
	{
		ret_drv = DRV_INVALID_PARAMS;
	}

	USBFAX_SEND_TRACE("USBFAX: ending usbfax_write process buffer ", RV_TRACE_LEVEL_DEBUG_LOW);

	usbfax_unlock();

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_get_tx_buffer
 *
 * Get send buffer
 *
 * @param device:		Data device number
 * @param buffer:		pointer to a buffer description
 *
 * @return		:	DRV_OK				when successfully executed this function
 *					DRV_INVALID_PARAMS	when the specified device does not exist 
 *												or the pointer data buffer is NULL.
 *					DRV_NOTCONFIGURED	when the device is not yet configured.
 *					DRV_INTERNAL_ERROR	on internal driver error.
 */
/*@{*/
U16	usbfax_get_tx_buffer(U32 device, T_dio_buffer**	buffer)
{
	U16	ret_drv = DRV_OK;

	/* pointer to the associated device */
	T_USBFAX_USB_DEVICE*	current_device_p = NULL;

	/* read request to queue */
	T_USBFAX_XFER*		xfer_request_p = NULL;

	/* Look for associated device */
	current_device_p = usbfax_get_device(device);
	if(current_device_p != NULL)
	{
		/* Check if device is configured */
		if((current_device_p->user_connected == TRUE)
			&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
		{
			/* Get request to dequeue */
			xfer_request_p = usbfax_get_queue_element(device, USBFAX_XFER_WRITE_REQUEST);
			
			if(xfer_request_p)
			{
				/* Get buffer from the request */
				*buffer = xfer_request_p->buffer_context;
				USBFAX_SEND_TRACE("USBFAX: usbfax_get_tx_buffer getting the buffer ", RV_TRACE_LEVEL_DEBUG_LOW);

				if(usbfax_dequeue_request(device, USBFAX_XFER_WRITE_REQUEST)
					!= RVM_OK)
				{
					/* the queue is empty */
					ret_drv = DRV_INTERNAL_ERROR;
				}
			}
			else
			{
				/* No buffer to return */
				buffer = NULL;

				/* the queue is empty */
				ret_drv = DRV_INTERNAL_ERROR;
			}
		}
		else
		{
			/* the device is not yet configured */
			ret_drv = DRV_NOTCONFIGURED;
		}
	}
	else
	{
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_clear
 *
 * Clear hardware send buffer of the selected device
 *
 * @param	device	:	device identifier
 *
 * @return	DRV_OK				Function successful
 *			DRV_INVALID_PARAMS	The specified device does not exist
 *			DRV_INTERNAL_ERROR	Internal driver error
 *			DRV_NOTCONFIGURED	The device is not yet configured.
 *			DRV_INPROCESS		The driver is busy clearing the buffer.
 */
/*@{*/
U16	usbfax_clear(U32 device)
{
	U16	ret_drv = DRV_OK;

	//USBFAX_SEND_TRACE("USBFAX: usbfax_clear called ", RV_TRACE_LEVEL_DEBUG_LOW);

	/* pointer to the associated device */
	T_USBFAX_USB_DEVICE*	current_device_p = NULL;

	/* read request to queue */
	T_USBFAX_XFER*		xfer_request_p = NULL;

	/* Look for associated device */
	current_device_p = usbfax_get_device(device);
	if(current_device_p != NULL)
	{
		/* Check if device is configured */
		if((current_device_p->user_connected == TRUE)
			&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
		{
			if(usbfax_transfer_in_progress(current_device_p))
			{
				current_device_p->write_clear_pending = TRUE;
				
				/* DIO user requires to clear writes requests in the FIFO */
				usbfax_empty_queue_request(current_device_p->device_number, USBFAX_XFER_WRITE_REQUEST);
				usbfax_signal(current_device_p, DRV_SIGTYPE_CLEAR);
				
				current_device_p->write_clear_pending = FALSE;
				
				ret_drv = DRV_OK;
			}
		}
		else
		{
			/* the device is not yet configured */
			ret_drv = DRV_NOTCONFIGURED;
		}
	}
	else
	{
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name 
 *
 * Clear hardware send buffer of the selected device
 *
 * @param	device	:	device identifier
 *
 * @return	DRV_OK				Function successful
 *			DRV_INVALID_PARAMS	The specified device does not exist
 *			DRV_INTERNAL_ERROR	Internal driver error
 *			DRV_NOTCONFIGURED	The device is not yet configured.
 *			DRV_INPROCESS		The driver is busy clearing the buffer.
 */
/*@{*/
U16	usbfax_flush(U32 device)
{
	U16	ret_drv = DRV_OK;

	/* pointer to the associated device */
	T_USBFAX_USB_DEVICE*	current_device_p = NULL;

	/* read request to queue */
	T_USBFAX_XFER*		xfer_request_p = NULL;

	USBFAX_SEND_TRACE("USBFAX: usbfax_flush called ", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Look for associated device */
	current_device_p = usbfax_get_device(device);
	if(current_device_p != NULL)
	{
		/* Check if device is configured */
		if((current_device_p->user_connected == TRUE)
			&& (current_device_p->device_config.device_type != DIO_TYPE_SER))
		{
			if(usbfax_transfer_in_progress(current_device_p))
			{
				USBFAX_SEND_TRACE("USBFAX : Write flush bitmask is set", RV_TRACE_LEVEL_ERROR);

				current_device_p->write_flush_pending = TRUE;
				ret_drv = DRV_INPROCESS;
			}
		}
		else
		{
			/* the device is not yet configured */
			ret_drv = DRV_NOTCONFIGURED;
		}
	}
	else
	{
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_get_capabilities
 *
 * Retrieve device capabilities of the selected device
 *
 * @param	device			:	device identifier
 * @param	capabilities	:	pointer to capabilities of the device
 *
 * @return	DRV_OK				Function successfully completed.
 *			DRV_INVALID_PARAMS	The specified device does not exist.
 *			DRV_INTERNAL_ERROR	Internal driver error.
 */
/*@{*/
U16	usbfax_get_capabilities(U32 device, T_DIO_CAP_SER**	capabilities)
{
	U16	ret_drv = DRV_OK;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_get_capabilities called ", RV_TRACE_LEVEL_DEBUG_LOW);

	T_USBFAX_USB_DEVICE		*current_device = NULL;

	/* Check pointer validity */
	if(capabilities != NULL)
	{
		/* Look for associated device */
		current_device = usbfax_get_device(device);
		if(current_device != NULL)
		{
			/* Copy capabilities of the device */
			*capabilities = current_device->device_capabilities;
			USBFAX_SEND_TRACE("USBFAX: usbfax_get_capabilities - handling capabilities", RV_TRACE_LEVEL_DEBUG_LOW);
		}
		else
		{
			/* Invalid device */
			ret_drv = DRV_INVALID_PARAMS;
		}
	}
	else
	{
		/* Bad argument */
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_set_config
 *
 * Set configuration of the selected device
 *
 * @param	device			:	device identifier
 * @param	dcb				:	pointer to a T_DIO_DCB
 *
 * @return	DRV_OK				Function successfully completed.
 *			DRV_INVALID_PARAMS	The specified device does not exist.
 *			DRV_INTERNAL_ERROR	Internal driver error.
 *
 */
/*@{*/
U16	usbfax_set_config(U32 device, T_DIO_DCB_SER* dcb)
{
	U16	ret_drv = DRV_OK;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_set_config called ", RV_TRACE_LEVEL_DEBUG_LOW);

	T_USBFAX_USB_DEVICE		*current_device = NULL;

	if(dcb != NULL)
	{
		/* Look for associated device */
		current_device = usbfax_get_device(device);
		if(current_device != NULL)
		{
			/* Copy configuration given in parameter to the selected device */
			memcpy(&current_device->device_config, dcb, sizeof(T_DIO_DCB_SER));
			USBFAX_SEND_TRACE("USBFAX: usbfax_set_config - configuration copied ", RV_TRACE_LEVEL_DEBUG_LOW);
		}
		else
		{
			/* Invalid device */
			ret_drv = DRV_INVALID_PARAMS;
		}
	}
	else
	{
		/* Bad argument */
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_get_config
 *
 *	Retrieve device congiguration
 *
 * @param	device			:	device identifier
 * @param	dcb				:	pointer to a T_DIO_DCB
 *
 * @return	DRV_OK				Function successfully completed.
 *			DRV_INVALID_PARAMS	The specified device does not exist.
 *			DRV_INTERNAL_ERROR	Internal driver error.
 *
 */
/*@{*/
U16	usbfax_get_config(U32 device, T_DIO_DCB_SER* dcb)
{
	U16	ret_drv = DRV_OK;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_get_config called ", RV_TRACE_LEVEL_DEBUG_LOW);

	T_USBFAX_USB_DEVICE		*current_device = NULL;

	if(dcb != NULL)
	{
		/* Look for associated device */
		current_device = usbfax_get_device(device);
		if(current_device != NULL)
		{
			/* Copy configuration of the device */
			memcpy(dcb, &current_device->device_config, sizeof(T_DIO_DCB_SER));
			USBFAX_SEND_TRACE("USBFAX: usbfax_get_config - copied device config ", RV_TRACE_LEVEL_DEBUG_LOW);
		}
		else
		{
			/* Invalid device */
			ret_drv = DRV_INVALID_PARAMS;
		}
	}
	else
	{
		/* Bad argument */
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/

/**
 * @name usbfax_close_device
 *
 * @param device:	Data device number
 *
 * @return	DRV_OK				Function successfully completed.
 *			DRV_INVALID_PARAMS	The specified device does not exist.
 *			DRV_INTERNAL_ERROR	Internal driver error.
 */
/*@{*/
U16	usbfax_close_device(U32 device)
{
	U16	ret_drv = DRV_OK;

//	USBFAX_SEND_TRACE("USBFAX: usbfax_close_device called ", RV_TRACE_LEVEL_DEBUG_LOW);

	T_USBFAX_USB_DEVICE		*current_device = NULL;

	/* Look for associated device */
	current_device = usbfax_get_device(device);

	if(current_device != NULL)
	{
		/* unset user_connected variable of the device structure */
		// Commented for AT command interpreted working on cable unplug-replug
		// current_device->user_connected = FALSE;
		USBFAX_SEND_TRACE("USBFAX: usbfax_close_device - unset connected state ", RV_TRACE_LEVEL_DEBUG_LOW);
	}
	else
	{
		/* Invalid device */
		ret_drv = DRV_INVALID_PARAMS;
	}

	return(ret_drv);
}
/*@}*/
