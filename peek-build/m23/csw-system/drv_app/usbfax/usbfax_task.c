/**
 * @file	usbfax_task.c
 *
 * 
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

#include "usbfax/usbfax_task_i.h"

#include "rvm/rvm_api.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "usb/usb_api.h"
#include "usb/usb_message.h"
#include "usb/usb_interface.h"

/****************************************************************************************
 |	VARIABLE DECLARATIONS																|
 V**************************************************************************************V*/
int	usbf_psi_rx_mutex = RV_INTERNAL_ERR;
int 			usbf_psi_use_rx_mutex = 0;

int	usbf_psi_tx_mutex = RV_INTERNAL_ERR;
int 			usbf_psi_use_tx_mutex = 0;

 /**
 * Pointer on the structure gathering all the global variables
 * used by USBFAX instance.
 */
extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;

 /**
 * Pattern of serial state notification as defined in
 * USB Class Definitions for Communication devices document p 75
 */
U8	serial_state_pattern[USBFAX_SERIAL_REQUEST_SIZE] = 
{
	SERIAL_STATE_REQUEST_TYPE,	/* bmRequestType	:	10100001B		*/
	SERIAL_STATE_NOTIFICATION,	/* bNotification	:	SERIAL_STATE	*/
	0x00,						/* wValue field low  part	:	0		*/
	0x00,						/* wValue field high part	:	0		*/
	0x00,						/* wIndex field low  part	: interface number	:	0	*/
	0x00,						/* wIndex field high part	: interface number	:	0	*/
	SERIAL_STATE_DATA_LENGTH,	/* wLength field low part	sizeof(T_CDC_SERIAL_STATE)	*/
	0x00						/* wLength field high part	:	0		*/
};

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_cdc_devices_initialize
 *
 * core function of the USB task.
 * This task loops on all incoming messages from the USB SWE and process it.
 *		- receiption of a vendor request
 *		- write notification of serial state
 *		- data received on IN bulk endpoint
 *		- write notification of data on OUT Bulk endpoint
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR	
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_cdc_devices_initialize(void)
{
	/* storage for awaited messages
	*/ 
	T_RV_HDR * msg_p = NULL;

	U8	current_interface_id	= 0;
	U8	current_endpoint_id		= 0;

	/* endpoint type */
	T_USBFAX_ENDPOINT_TYPE	endpoint_type		= USBFAX_UNKNOWN;
	T_USBFAX_USB_DEVICE*		current_device_p	= NULL;

	T_RVM_RETURN			ret;

	/* Start USB SWE */
	USBFAX_SEND_TRACE("USBFAX: USBFAX task starts USB SWE", RV_TRACE_LEVEL_DEBUG_LOW);
	// usbfax_start_usb_swe();

	usbfax_init_cdc_devices();

	/* USBFAX suscribe to all CDC interfaces */
	USBFAX_SEND_TRACE("USBFAX: USBFAX task suscribes to all CDC interfaces", RV_TRACE_LEVEL_DEBUG_LOW);

	ret = usbfax_suscribe_to_cdc_interfaces();
	
	USBFAX_SEND_TRACE("USBFAX: End of CDC subscriptions", RV_TRACE_LEVEL_ERROR);
	
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_prepare_vendor_rx_buffer_request
 *
 * This function give a buffer to the USB SWE to receive vendor requests.
 * the buffer used is "vendor_request_buffer" of the T_USBFAX_USB_DEVICE structure.
 *
 * @param current_device_p:	reference to a device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_prepare_vendor_rx_buffer_request(T_USBFAX_USB_DEVICE* current_device_p)
{
	T_RVM_RETURN			ret = RVM_OK;

	USBFAX_TRACE_WARNING_PARAM("USBFAX: Post RX vendor request interface: ", current_device_p->usb_ep_cnfg->ep_control_context.interface_id);
	USBFAX_TRACE_WARNING_PARAM("USBFAX: Post RX vendor request endpoint : ", current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id);

	if(usb_set_rx_buffer(	current_device_p->usb_ep_cnfg->ep_control_context.interface_id,
							current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id,
							current_device_p->vendor_request_buffer, LENGTH_VENDOR_REQUEST_PACKET) != RVM_OK)
	{
		ret = RVM_INTERNAL_ERR;
	}

	return ret;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_vendor_request
 *
 * This function analyze content of the vendor request buffer
 * Supported vendor requests are :
 *		- SET_LINE_CODING_REQUEST
 *		- GET_LINE_CODING_REQUEST 
 *		- SET_CONTROL_LINE_STATE_REQUEST
 *		- SEND_BREAK
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR	
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_vendor_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{	
	T_RVM_RETURN	ret = RVM_OK;
	U8			current_vendor_request;

	if(current_device_p)
	{
		if(msg_p->msg_id == USB_RX_BUFFER_FULL_MSG)
		{
			/* Get request fiel of the SETUP data */
			current_vendor_request = current_device_p->vendor_request_buffer[1];
			
			switch(current_vendor_request)
			{
				case SET_LINE_CODING_REQUEST :
					/* current request is SET_LINE_CODING */
					USBFAX_SEND_TRACE("USBFAX: Vendor request is Set_Line_Coding",	RV_TRACE_LEVEL_WARNING);
					ret = usbfax_process_set_line_coding_request(current_device_p, msg_p);

					/* Vendor request is processed */
					/* current task can receive next vendor request */
					usbfax_prepare_vendor_rx_buffer_request(current_device_p);
				break;

				case GET_LINE_CODING_REQUEST :
					/* current request is GET_LINE_CODING */
					USBFAX_SEND_TRACE("USBFAX: Vendor request is Get_Line_Coding",	RV_TRACE_LEVEL_WARNING);
					ret = usbfax_process_get_line_coding_request(current_device_p, msg_p);
				break;

				case SET_CONTROL_LINE_STATE_REQUEST :
					/* current request is SET_CONTROL_LINE_STATE */
					USBFAX_SEND_TRACE("USBFAX: Vendor request is Set_Control_Line_State",	RV_TRACE_LEVEL_WARNING);
					ret = usbfax_process_set_line_state_request(current_device_p, msg_p);

					/* Vendor request is processed */
					/* current task can receive next vendor request */
					usbfax_prepare_vendor_rx_buffer_request(current_device_p);
				break;

				case SEND_BREAK_REQUEST :
					/* current request is SET_CONTROL_LINE_STATE */
					USBFAX_SEND_TRACE("USBFAX: Vendor request is Send_Break",	RV_TRACE_LEVEL_WARNING);
					ret = usbfax_process_send_break_request(current_device_p, msg_p);

					/* Vendor request is processed */
					/* current task can receive next vendor request */
					usbfax_prepare_vendor_rx_buffer_request(current_device_p);
				break;

				default :
					/* Unknwon vendor request */
					USBFAX_SEND_TRACE("USBFAX: Unknown vendor request",	RV_TRACE_LEVEL_ERROR);

					/* Vendor request is processed */
					/* current task can receive next vendor request */
					usbfax_prepare_vendor_rx_buffer_request(current_device_p);
					ret = RVM_INTERNAL_ERR;
				break;
			}
		}
		else if(msg_p->msg_id == USB_TX_BUFFER_EMPTY_MSG)
		{
			/* Response of a vendor request has been made */
			/* Buffer can be free */
			if(current_device_p->vendor_response_buffer != NULL)
			{
				rvf_free_buf(current_device_p->vendor_response_buffer);
				current_device_p->vendor_response_buffer = NULL;

				/* Vendor request is processed */
				/* current task can receive next vendor request */
				usbfax_prepare_vendor_rx_buffer_request(current_device_p);

			}
		}
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_set_line_coding_request
 *
 * This function implement receiption of a SET_LINE_CODING vendor request.
 * Line coding information of the current are updated with data of the 
 * set_line_coding command
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message
 *
 * @return	RVM_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_set_line_coding_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{
	T_RVM_RETURN	ret = RVM_OK;
	U8*			line_coding_data_p = NULL;

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_SET_LINE_CODING_REQUEST)
	{
		/* Point to data of the SET_LINE_CODING request */
		line_coding_data_p = 
			current_device_p->vendor_request_buffer + LENGTH_VENDOR_REQUEST_COMMAND;

		/* Update line coding information of the current device */
		
		/* Data terminal rate, in bits per second */
		current_device_p->usb_line_coding.data_rate	 = line_coding_data_p[3] << 24;
		current_device_p->usb_line_coding.data_rate	|= line_coding_data_p[2] << 16;
		current_device_p->usb_line_coding.data_rate	|= line_coding_data_p[1] <<  8;
		current_device_p->usb_line_coding.data_rate	|= line_coding_data_p[0];
		USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->data_rate: ", current_device_p->usb_line_coding.data_rate);
		
		/* Stop bits */
		current_device_p->usb_line_coding.char_format = line_coding_data_p[4];
		USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->Stop_bits: ", current_device_p->usb_line_coding.char_format);
		
		/* Parity */
		current_device_p->usb_line_coding.parity_type = line_coding_data_p[5];
		USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->Parity: ", current_device_p->usb_line_coding.parity_type);
		
		/* Data bits */
		current_device_p->usb_line_coding.data_bits	= line_coding_data_p[6];
		USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->data_bits: ", current_device_p->usb_line_coding.data_bits);
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Invalid size of SetLineCoding request ",RV_TRACE_LEVEL_ERROR);
		/* Invalid size of SET_LINE_CODING request */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_get_line_coding_request
 *
 * This function implement receiption of a GET_LINE_CODING vendor request.
 * The line coding structure of the device is sent to the host
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR	
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_get_line_coding_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{	
	T_RV_RET ret = RVM_OK;
	U16	size_to_return = 0;
	
	U8	buffer[LENGTH_VENDOR_REQUEST_DATA];
	U8	*line_coding_info = NULL;

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_VENDOR_REQUEST_COMMAND)
	{
			/* Update buffer to send with line coding structure of the device */
			/* Data terminal rate, in bits per second */
			buffer[3] = (current_device_p->usb_line_coding.data_rate >> 24) & 0xFF;
			buffer[2] = (current_device_p->usb_line_coding.data_rate >> 16) & 0xFF;
			buffer[1] = (current_device_p->usb_line_coding.data_rate >>  8) & 0xFF;
			buffer[0] = (current_device_p->usb_line_coding.data_rate	  ) & 0xFF;
			USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->data_rate: ", current_device_p->usb_line_coding.data_rate);

			/* Stop bits */
			buffer[4] = current_device_p->usb_line_coding.char_format;
			USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->Stop_bits: ", current_device_p->usb_line_coding.char_format);

			buffer[5] = current_device_p->usb_line_coding.parity_type;
			USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->Parity: ", current_device_p->usb_line_coding.parity_type);

			buffer[6] = current_device_p->usb_line_coding.data_bits;
			USBFAX_TRACE_WARNING_PARAM("USBFAX: LineCoding->data_bits: ", current_device_p->usb_line_coding.data_bits);
	
			/* Size of */
			size_to_return  = current_device_p->vendor_request_buffer[6];
			size_to_return |= current_device_p->vendor_request_buffer[7] << 8;

			/* allocate a buffer of the requested size to send line coding information */
			usbfax_get_mem(&line_coding_info, size_to_return);

			memcpy(line_coding_info, buffer, size_to_return);

			USBFAX_TRACE_WARNING_PARAM("USBFAX: Size of response of GetLineCoding : ", size_to_return);

			USBFAX_TRACE_WARNING_PARAM("USBFAX: Post TX vendor request interface: ", current_device_p->usb_ep_cnfg->ep_control_context.interface_id);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: Post TX vendor request endpoint : ", current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id);

			/* Send line coding information to the host */
			if(usb_set_tx_buffer(
				current_device_p->usb_ep_cnfg->ep_control_context.interface_id,
				current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id,
				line_coding_info, size_to_return, TRUE)	 != RVM_OK)
			{
				USBFAX_SEND_TRACE("USBFAX: Failed to respond to GetLineCoding", RV_TRACE_LEVEL_ERROR);
				ret = RVM_INTERNAL_ERR;
			}

			/* store address of the buffer sent */
			current_device_p->vendor_response_buffer = line_coding_info;

	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Invalid size of GetLineCoding request ",RV_TRACE_LEVEL_ERROR);
		/* Invalid GET_LINE_CODING command */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_set_line_state_request
 *
 * This function implement receiption of a SET_CONTROL_LINE_STATE vendor request.
 * line state of the device is updated.
 * If DTR is raised signal to the DIO, a connexion with the host
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR	
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_set_line_state_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{
	T_RVM_RETURN	ret = RVM_OK;
	U8			line_state = 0;
	BOOLEAN		host_signal_changed = FALSE;	

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_VENDOR_REQUEST_COMMAND)
	{
		/* get line state of the SET_CONTROL_LINE_STATE request */
		line_state = current_device_p->vendor_request_buffer[2];

		/* check if DTR of the device is raised */
		if(current_device_p->usb_line_state.host_dtr == 0
			&& (line_state & 1)
			&& current_device_p->user_connected		== TRUE
			&& current_device_p->dtr_host_connected == FALSE)
		{
			/* 
				The host set DTR signal and there is only one 
				DRV_SIGTYPE_CONNECT signal sent to PSI 
			*/
			current_device_p->dtr_host_connected = TRUE;

			/* Update line state of the device */
			current_device_p->usb_line_state.host_dtr = line_state & 1;	/* bit 0 : activate DTR */
			current_device_p->usb_line_state.host_rts = (line_state & 2) >> 1;	/* bit 1 : activate RTS */
			
			/* The serial state has changed, therefore new serial state is sent to host */
			usbfax_process_control_lines(current_device_p);

			/* Notify DIO that device is connected with the host */
			usbfax_signal(current_device_p, DRV_SIGTYPE_CONNECT);

			if(usbfax_env_ctrl_blk_p->host_serial_driver_connected == FALSE)
			{
				T_USBFAX_XFER*	current_write_request_p 
								= usbfax_next_queue_request(&current_device_p->write_request_queue);
				USBFAX_SEND_TRACE("USBFAX: usbfax_process_set_line_state_request -> usbfax_process_requests_from_write_fifo", RV_TRACE_LEVEL_ERROR);
				usbfax_env_ctrl_blk_p->host_serial_driver_connected = TRUE;
				usbfax_process_requests_from_write_fifo(current_device_p);
			}

			usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
			usbfax_start_timer(current_device_p);

			USBFAX_TRACE_WARNING_PARAM("USBFAX: Host line state DTR: ", current_device_p->usb_line_state.host_dtr);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: Host line state RTS: ", current_device_p->usb_line_state.host_rts);
			return(ret);


		}
		else
		{
			if((current_device_p->usb_line_state.host_dtr == 0)
				&& (line_state & 1))
			{
				/* DTR up */
				host_signal_changed = TRUE;
			}
			else if ((current_device_p->usb_line_state.host_dtr == 1)
				&& ((line_state & 1) == 0))
			{
				/* DTR drops */
				host_signal_changed = TRUE;
			}
			else if((current_device_p->usb_line_state.host_rts == 0)
				&& (line_state & 2))
			{
				/* RTS up */
				host_signal_changed = TRUE;
			}
			else if((current_device_p->usb_line_state.host_rts == 1)
				&& ((line_state & 2 )== 0))
			{
				/* RTS drops */
				host_signal_changed = TRUE;
			}

		/* Update line state of the device */
		current_device_p->usb_line_state.host_dtr = line_state & 1;	/* bit 0 : activate DTR */
		current_device_p->usb_line_state.host_rts = (line_state & 2) >> 1;	/* bit 1 : activate RTS */

		USBFAX_TRACE_WARNING_PARAM("USBFAX: Host line state DTR: ", current_device_p->usb_line_state.host_dtr);
		USBFAX_TRACE_WARNING_PARAM("USBFAX: Host line state RTS: ", current_device_p->usb_line_state.host_rts);

			if(host_signal_changed == TRUE)
			{
				usbfax_signal(current_device_p, DRV_SIGTYPE_READ);
			}
		}
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Invalid size of SetLineState request ",RV_TRACE_LEVEL_ERROR);
		/* Invalid SET_CONTROL_LINE_STATE command */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_set_line_state_request
 *
 * This function implement receiption of a SEND_BREAK vendor request.
 * The received request is a special carrier 
 * The wValue field of request contains the length of time, in milliseconds, of the break signal
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_send_break_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{
	T_RVM_RETURN	ret = RVM_OK;

	if(current_device_p != NULL)
	{
		/* Updates USBFAX_SERIAL_STATE_BREAK bitmask of the serial state of the device */
		current_device_p->usb_serial_state |= USBFAX_SERIAL_STATE_BREAK;
	}
	else
	{
		/* non covered condition */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_control_lines
 *
 * This function send serial state of the driver to the host.
 * It is called on serial state modifcation
 *
 * @param current_device_p:	reference to the current device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_control_lines(T_USBFAX_USB_DEVICE* current_device_p)
{	
	T_RV_RET ret = RVM_OK;
	U8	*serial_state_data = NULL;

	/* allocate buffer for the serial state to send */
	usbfax_get_mem(&serial_state_data, USBFAX_SERIAL_STATE_SIZE);

	/* copy the serial state pattern into the buffer to send */
	memcpy(serial_state_data, serial_state_pattern, USBFAX_SERIAL_REQUEST_SIZE);

	/* Fill low part of the wIndex field with interface number */
	serial_state_data[4] = current_device_p->usb_ep_cnfg->ep_int_context.interface_id;	
	
	/* Fill data part with the serial state */
	serial_state_data[8] = current_device_p->usb_serial_state >> 8;		/* high part of serial state data */
	serial_state_data[9] = current_device_p->usb_serial_state & 0xFF;	/* low  part of serial state data */

	USBFAX_SEND_TRACE("USBFAX: Serial state of device sent", RV_TRACE_LEVEL_ERROR);

	if(current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_TX_CARRIER)
	{
		USBFAX_SEND_TRACE("USBFAX: Serial State DSR is UP", RV_TRACE_LEVEL_DEBUG_LOW);
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Serial State DSR is LOW", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	if(current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RX_CARRIER)
	{
		USBFAX_SEND_TRACE("USBFAX: Serial State DCD is UP", RV_TRACE_LEVEL_DEBUG_LOW);
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Serial State DCD is DOWN", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	/* Send serial state to the host */
	if(usb_set_tx_buffer(	current_device_p->usb_ep_cnfg->ep_int_context.interface_id,
							current_device_p->usb_ep_cnfg->ep_int_context.endpoint_id,
							serial_state_data, USBFAX_SERIAL_STATE_SIZE, TRUE)	 != RVM_OK)
	{
		ret = RVM_INTERNAL_ERR;
	}

	/* store address of the buffer sent */
	current_device_p->serial_state_buffer = serial_state_data;

	/* Give a chance to USB controler to send serial state */
    rvf_delay(RVF_MS_TO_TICKS(100));

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_control_request
 *
 * This function is called when serial state has been sent to the host.
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return	RVM_OK				when successfully call
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_control_request(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{	
	BOOLEAN			write_fifo_is_empty = FALSE;
	T_RVM_RETURN	ret = RVM_INTERNAL_ERR;

	if(current_device_p)
	{
		/* Check message validity */
		if((msg_p->msg_id == USB_TX_BUFFER_EMPTY_MSG)
			&& (current_device_p->serial_state_buffer != NULL))
		{
			/* Free sent buffer */
			rvf_free_buf(current_device_p->serial_state_buffer);
			current_device_p->serial_state_buffer = NULL;

			if(current_device_p->signal_write_serial_state == TRUE)
			{
				T_USBFAX_XFER*	current_write_request = 
						usbfax_get_current_request(&current_device_p->write_request_queue);

				current_device_p->signal_write_serial_state = FALSE;

				if(current_write_request->buffer_context == NULL)
				{
					/* The driver uses the DRV_SIGTYPE_WRITE signal only when 
						the data of the buffer is sent.
					*/
					// usbfax_signal(current_device_p, DRV_SIGTYPE_WRITE);

					/* Shift request data in the circular fifo */
					write_fifo_is_empty = usbfax_suppress_write_requests_fifo(current_device_p);

					if(write_fifo_is_empty == FALSE)
				{
						T_USBFAX_XFER*	current_write_request_p 
								= usbfax_next_queue_request(&current_device_p->write_request_queue);
						USBFAX_SEND_TRACE("USBFAX: usbfax_process_control_request -> usbfax_process_requests_from_write_fifo", RV_TRACE_LEVEL_ERROR);
					
					/* Write fifo then process next request */
						usbfax_process_requests_from_write_fifo(current_device_p);
					}
				}
			}
			

			ret = RVM_OK;
		}
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_tx_context
 *
 * This function is called on receiption of the event USB_TX_BUFFER_EMPTY_MSG
 * on the Bulk IN endpoint.
 * If all data of the request are transmitted, next request is processed in the FIFO.
 * At ther oppossite if the write request is not yet finished 
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_tx_context(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{	
	INT8					current_request_index;	
	T_USBFAX_XFER*	current_write_request_p = NULL;
	T_RVM_RETURN	ret = RVM_OK;
	BOOLEAN			write_in_progress = FALSE;
	
	/* Get current write request */
	current_request_index = current_device_p->write_request_queue.current_element;
	if(current_request_index != -1)
	{
		/* Current write request has been found in the FIFO */
		USBFAX_TRACE_WARNING_PARAM("usbfax_process_tx_context: index of current request", current_request_index);
		current_write_request_p = &current_device_p->write_request_queue.tab_xfer[current_request_index];
	}
	else
	{
		/* Write FIFO is empty */
		/* There is no write request in progress */
		USBFAX_SEND_TRACE("USBfax_process_tx_context : no request", RV_TRACE_LEVEL_DEBUG_LOW);
		memset(&current_device_p->buffer_tx_temp, 0, sizeof(T_USBFAX_TEMPORARY_BUFFER));
		return(RV_OK);
	}

	/* Check kind of received message */
	/* Only TX_BUFFER_EMPTY messages are processed */
	if(msg_p->msg_id != USB_TX_BUFFER_EMPTY_MSG)
	{
		USBFAX_SEND_TRACE("USBfax_process_tx_context : bad message", RV_TRACE_LEVEL_ERROR);
		return(RV_INTERNAL_ERR);
	}

	if(current_device_p->write_clear_pending == TRUE)
	{
		USBFAX_SEND_TRACE("process_tx_context : Write clear pending", RV_TRACE_LEVEL_DEBUG_LOW);
		/* DIO user requires to clear writes requests in the FIFO */
		usbfax_empty_queue_request(current_device_p->device_number, USBFAX_XFER_WRITE_REQUEST);
		usbfax_signal(current_device_p, DRV_SIGTYPE_CLEAR);
		current_device_p->write_clear_pending = FALSE;
		return(RVM_OK);
	}

	/* Checks if all data of the write request has been transmitted */
	if((current_write_request_p->segment == NULL)
		&& (current_write_request_p->sent_length == current_write_request_p->segment_size))
	{
		USBFAX_SEND_TRACE("process_tx_context : All data Sent", RV_TRACE_LEVEL_DEBUG_LOW);
		
		/* Signal to the DIO end of write request */
		usbfax_unlock();
		usbfax_signal(current_device_p, DRV_SIGTYPE_WRITE);
		usbfax_lock();

		USBFAX_SEND_TRACE("USBFAX: exit usbfax_signal function",RV_TRACE_LEVEL_ERROR);

		/*get next request and process it*/
		write_in_progress = usbfax_process_requests_from_write_fifo(current_device_p);
	
	}
	else
	{
		USBFAX_SEND_TRACE("process_tx_context : There are remaining data to send", RV_TRACE_LEVEL_DEBUG_LOW);
		if (current_write_request_p != NULL)
		{
			ret = usbfax_process_write(current_device_p, current_write_request_p);
			if(ret == RVM_OK)
				write_in_progress = TRUE;

		}
	}

	if (write_in_progress == FALSE)
	{
		/* All write requests has been flushed */
		if (current_device_p->write_flush_pending == TRUE)
		{
			USBFAX_SEND_TRACE("USBFAX : Signal DRV_SIGTYPE_FLUSH", RV_TRACE_LEVEL_ERROR);
			/* DIO user is signaled that all requests has been flushed */
			usbfax_signal(current_device_p, DRV_SIGTYPE_FLUSH);
			current_device_p->write_flush_pending = FALSE;
		}
	}

	return(RVM_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_write
 *
 * From a write request, this function will fill a temporary buffer
 * to send to the host.
 *
 * @param current_device_p:			reference to a device
 * @param current_write_request_p	write request to process
 *
 * @return		RVM_OK
 *				RVM_INTERNAL_ERR	USB SWE cannot send data to host
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_write(T_USBFAX_USB_DEVICE* current_device_p, T_USBFAX_XFER* current_write_request_p)
{	
	T_USBFAX_TEMPORARY_BUFFER*	buffer_tx_temp = &current_device_p->buffer_tx_temp;
	
	U32	data_remaining_in_buffer_temp	= MAX_PACKET_USBFAX_SIZE;
	U32	data_remaining_to_sent			= 0;
	U32	offset_buffer_temp				= 0;

	BOOL shorter_transfer = FALSE;

	T_RVM_RETURN	ret	= RVM_OK;

	/* Check if segment is valid */
	if(current_write_request_p->segment != NULL)
	{
		do
		{
			/* Checking size of remaining data to send for the current segment */
			data_remaining_to_sent = 
				current_write_request_p->segment_size - current_write_request_p->sent_length;

			if(data_remaining_to_sent > data_remaining_in_buffer_temp)
			{
				/* Temporary buffer is not large enough to contain data of the segment */
				memcpy(	buffer_tx_temp->buff + offset_buffer_temp, 
						current_write_request_p->segment + current_write_request_p->sent_length,
						data_remaining_in_buffer_temp);

				/* Update length of sent data for the current segment */
				current_write_request_p->sent_length += data_remaining_in_buffer_temp;

				/* Temporary buffer is full and ready to be sent */
				offset_buffer_temp = MAX_PACKET_USBFAX_SIZE;
				data_remaining_in_buffer_temp = 0;
			}
			else
			{
				/* Copy segment data in the temporary buffer */
				memcpy(	buffer_tx_temp->buff + offset_buffer_temp, 
						current_write_request_p->segment + current_write_request_p->sent_length, 
						data_remaining_to_sent);
			
				/* Update length of sent data for the current segment */
				current_write_request_p->sent_length += data_remaining_to_sent;

				/* Update remaining size in the buffer temp */
				data_remaining_in_buffer_temp -= data_remaining_to_sent;
				
				/* Update offset of the temporary buffer */
				offset_buffer_temp += data_remaining_to_sent;

				/* Get next segment of the write request */
				USBFAX_SEND_TRACE("USBFAX_process_write: usbfax_next_segment ",RV_TRACE_LEVEL_DEBUG_LOW);
				usbfax_next_segment(current_write_request_p);
			}
		}
		while ((current_write_request_p->segment != NULL) 
				&& (data_remaining_in_buffer_temp > 0));

		/* Update size of the temporary buffer */
		buffer_tx_temp->size = offset_buffer_temp;

		USBFAX_TRACE_WARNING_PARAM	("USBFAX_process_write: new TX buffer", buffer_tx_temp->size);

		if((buffer_tx_temp->size % 64)==0)
			shorter_transfer = TRUE;

		/* Requests to USB SWE to send data on IN BULK endpoint */
		if(usb_set_tx_buffer(	current_device_p->usb_ep_cnfg->ep_tx_context.interface_id,
								current_device_p->usb_ep_cnfg->ep_tx_context.endpoint_id,
								buffer_tx_temp->buff, buffer_tx_temp->size, shorter_transfer) != RVM_OK)
		{
			ret = RVM_INTERNAL_ERR;
		}
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX_process_write: Oops, no segment(s) available", RV_TRACE_LEVEL_ERROR);
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_rx_context
 *
 * This function is called on receiption of the event USB_RX_BUFFER_FULL_MSG
 * on the Bulk OUT endpoint.
 * If all data of the request are received, next request is processed in the FIFO.
 * At ther oppossite if the read request is not yet completed, 
 *
 * @param current_device_p:	reference to the current device
 * @param msg_p:			reference to the received message by this device
 *
 * @return				RVM_OK
 *						RV_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_rx_context(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p)
{	
	/* Index of the current request */
	INT8				current_request_index	= 0;	
	
	U16				buffer_size				= 0;
	U8				i						= 0;
	
	/* pointer on the current request */
	T_USBFAX_XFER*	current_read_request_p = NULL;

	/* Get current read request */
	current_request_index = current_device_p->read_request_queue.current_element;
	if(current_request_index != -1)
	{
		USBFAX_TRACE_WARNING_PARAM("usbfax_process_rx_context: index of current request", current_request_index);
		current_read_request_p = &current_device_p->read_request_queue.tab_xfer[current_request_index];
	}
	else
	{
		/* the read queue of the device is empty */
		USBFAX_SEND_TRACE("USBfax_process_rx_context : no reqeust to put data in", RV_TRACE_LEVEL_DEBUG_LOW);
		memset(&current_device_p->buffer_rx_temp, 0, sizeof(T_USBFAX_TEMPORARY_BUFFER));
		return(RV_OK);
	}


	/* check message type */
	if(msg_p->msg_id != USB_RX_BUFFER_FULL_MSG)
	{
		return(RV_INTERNAL_ERR);
	}

	if(current_read_request_p != NULL)
	{
		BOOLEAN	b_esc_seq = FALSE;
		
		/* Get size of the received buffer */
		current_device_p->buffer_rx_temp.size = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size;

		if(current_device_p->device_config.guard_period != DIO_ESC_OFF)
		{
			/* Try to detect a sequence escape "+++" */
			b_esc_seq = usbfax_escape_sequence_detect(current_device_p);

		}

		if(b_esc_seq == FALSE)
		{
			/* Start processing of the received message */
			usbfax_process_read(current_device_p, current_read_request_p, ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size);
		}
	}

	return(RVM_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_read
 *
 * The current read request found in the FIFO, is updated with data buffer 
 * of the temporary buffer
 *
 * @param current_device_p:			reference to a device
 * @param current_write_request_p	write request to process
 * @param size						size of received data
 *
 * @return		RVM_OK
 *				RVM_INTERNAL_ERR	USB SWE cannot send data to host
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_process_read(T_USBFAX_USB_DEVICE* current_device_p, T_USBFAX_XFER* current_read_request_p, U32 buffer_size)
{	
	T_USBFAX_TEMPORARY_BUFFER*	buffer_rx_temp = &current_device_p->buffer_rx_temp;

	U32	data_remaining_in_buffer_temp	= 0;
	U32	data_remaining_to_read			= 0;
	U32	offset_buffer_temp				= 0;
	U32 i								= 0;

	U16	size_to_request = 0;
	U8	First_read = 0;

				
	T_RVM_RETURN ret	= RVM_OK;

	data_remaining_in_buffer_temp = buffer_rx_temp->size;
				
	/* Checking size of remaining data to send for the current segment */
	if(data_remaining_in_buffer_temp > 0)
	{
		do
		{
			data_remaining_to_read = 
				current_read_request_p->segment_size - current_read_request_p->sent_length;

			if(current_read_request_p->sent_length == 0)
			{
				/*if the segment is empty, this means we are copying the first bytes*/
				First_read = 1;
					
			}
			
			/*assert the request is valid and has not been dealocated by upper layer*/
			if(current_read_request_p->sent_length > current_read_request_p->segment_size)
			{
					data_remaining_to_read = 0;
				//we want to drop this unvalid request
				First_read = 0;
				USBFAX_SEND_TRACE("USBFAX: usbfax_process_read bad sent length", RV_TRACE_LEVEL_ERROR);
				return RVM_INTERNAL_ERR;
					
			}

			/*assert the request is valid and has not been dealocated by upper layer*/
			if((current_read_request_p->c_align1 != 0) || (current_read_request_p->c_align2 != 0))
			{
					data_remaining_to_read = 0;
					
				USBFAX_SEND_TRACE("USBFAX: usbfax_process_read bad alignment", RV_TRACE_LEVEL_ERROR);

				USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: sent_length",		current_read_request_p->sent_length);
				USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: total_sent_length",current_read_request_p->total_sent_length);
				USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: segment_index",	current_read_request_p->segment_index);
				USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: c_align1 ",		current_read_request_p->c_align1);
				USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: c_align2",			current_read_request_p->c_align2);

				//we want to drop this unvalid request
				return RVM_INTERNAL_ERR;
					
			}
			
			if(data_remaining_to_read >= data_remaining_in_buffer_temp)
			{
				U8 *buf_src, *buf_dest;
					/* Copy data of the temporary buffer to the current segment */
					/*memcpy(	current_read_request_p->segment + current_read_request_p->sent_length, 
						buffer_rx_temp->buff + offset_buffer_temp,
						data_remaining_in_buffer_temp);*/
				buf_dest = current_read_request_p->segment + current_read_request_p->sent_length;
				buf_src = buffer_rx_temp->buff + offset_buffer_temp;
				for(i=0;i<data_remaining_in_buffer_temp;i++)
				{
					*(buf_dest + i) = *(buf_src +i);
				}

				/* Update read size of read data for the segment */
				current_read_request_p->sent_length			+= data_remaining_in_buffer_temp;
				data_remaining_to_read			-= data_remaining_in_buffer_temp;
				
				/* Update read size of read data for the read request */
				current_read_request_p->total_sent_length	+= data_remaining_in_buffer_temp;
				
				data_remaining_in_buffer_temp	= 0;
				offset_buffer_temp				= 0;
		

				if(First_read == 1)
				{
					current_read_request_p->usb_posted = FALSE;
					
					/*notify upper layer we put something in the buffer*/
					USBFAX_SEND_TRACE("USBFAX_process_read: send DRV_SYGTYPE_READ-1", RV_TRACE_LEVEL_DEBUG_LOW);
					if (RV_INTERNAL_ERR == usbf_psi_rx_mutex)
						usbf_psi_rx_mutex = vsi_s_open(0, "fax_rx", 0);
					usbf_psi_use_rx_mutex = 1;					
					usbfax_unlock();
					usbfax_signal(current_device_p, DRV_SIGTYPE_READ);
					usbfax_lock();
				}

				if(FALSE == usbfax_queue_is_empty(current_device_p->device_number, USBFAX_XFER_READ_REQUEST))
				{
					INT8 current_request_index = current_device_p->read_request_queue.current_element;
					if(current_request_index != -1)
					{
						USBFAX_TRACE_WARNING_PARAM("usbfax_process_read: index of current request after signal", current_request_index);
						current_read_request_p = &current_device_p->read_request_queue.tab_xfer[current_request_index];

						if(FALSE == current_read_request_p->usb_posted)
						{
							data_remaining_to_read = current_read_request_p->segment_size;
							USBFAX_TRACE_WARNING_PARAM("USBFAX_process_read: data_remaining_to_read", data_remaining_to_read);
						}
						else 
						{
							data_remaining_to_read = 0;
							USBFAX_SEND_TRACE("USBFAX_process_read: Read fifo is empty -> no more data to read", RV_TRACE_LEVEL_ERROR);
						}
					}
				}
			}
			else
			{
				U8 *buf_src, *buf_dest;

				/*memcpy(	current_read_request_p->segment + current_read_request_p->sent_length, 
						buffer_rx_temp->buff + offset_buffer_temp, 
						data_remaining_to_read);
				*/
				buf_dest = current_read_request_p->segment + current_read_request_p->sent_length;
				buf_src = buffer_rx_temp->buff + offset_buffer_temp;

				for(i=0;i<data_remaining_to_read;i++)
		{
					*(buf_dest + i) = *(buf_src +i);
				}

				current_read_request_p->sent_length			+= data_remaining_to_read;
				current_read_request_p->total_sent_length	+= data_remaining_to_read;
				offset_buffer_temp += data_remaining_to_read;
				data_remaining_in_buffer_temp -= data_remaining_to_read;

				/* Signal to DIO SWE that the buffer of the segment is full */
				//usbfax_signal(current_device_p, DRV_SIGTYPE_READ);
				if(First_read == 1)
				{
			/*notify upper layer we put something in the buffer*/
					/*here, it means we filled the segment in one usb read*/
					USBFAX_SEND_TRACE("USBFAX_process_read: send DRV_SYGTYPE_READ-2", RV_TRACE_LEVEL_DEBUG_LOW);
			usbfax_signal(current_device_p, DRV_SIGTYPE_READ);

				}

				/* Get next segment of the buffer */
				USBFAX_SEND_TRACE("USBFAX_process_read: usbfax_next_segment ",RV_TRACE_LEVEL_DEBUG_LOW);
				usbfax_next_segment(current_read_request_p);

				/* If there is no segment to process in read request */
				if (current_read_request_p->segment == NULL)
				{
					/* Next read request is processed */
					USBFAX_SEND_TRACE("USBFAX_process_read: get-next request", RV_TRACE_LEVEL_DEBUG_LOW);
					current_read_request_p = usbfax_next_queue_request(&current_device_p->read_request_queue);
					usbfax_next_segment(current_read_request_p);
				}
			}
		}
		while ((current_read_request_p != NULL) 
				&& (data_remaining_in_buffer_temp > 0));
	}
	else
	{
		data_remaining_to_read = 
			current_read_request_p->segment_size - current_read_request_p->sent_length;
	}

	buffer_rx_temp->size = data_remaining_in_buffer_temp;

	if(usbfax_queue_is_empty(current_device_p->device_number, USBFAX_XFER_READ_REQUEST) == FALSE)
	{
	/* If there is a read request in progress and the segment is valid */
		if ((data_remaining_in_buffer_temp == 0)
			&& (data_remaining_to_read > 0)
			&& current_read_request_p)
		{
			/* current request is given to USB driver */
			current_read_request_p->usb_posted = TRUE;

			if(data_remaining_to_read >= MAX_PACKET_USBFAX_SIZE)
			{
				size_to_request = MAX_PACKET_USBFAX_SIZE;
			}
			else
			{
				if(data_remaining_to_read % 64)
				{
						size_to_request = (data_remaining_to_read / 64) * 64;
				}
				else
				{
						size_to_request = data_remaining_to_read;
				}
			}
		
			USBFAX_TRACE_WARNING_PARAM	("USBFAX_process_read: new RX buffer", size_to_request);
			
			/* the temporary read buffer is provided to the usb driver */
			/* Size of the provided buffer is 192 (multiple of 64 bytes) */
 			if(usb_set_rx_buffer(	current_device_p->usb_ep_cnfg->ep_rx_context.interface_id,
								current_device_p->usb_ep_cnfg->ep_rx_context.endpoint_id,
								buffer_rx_temp->buff, size_to_request) != RVM_OK)
			{
				USBFAX_SEND_TRACE("USBFAX_process_read: usb_set_rx_buffer error", RV_TRACE_LEVEL_ERROR);
				ret = RVM_INTERNAL_ERR;
			}
		}
		else
		{
			USBFAX_SEND_TRACE("USBFAX_process_read: not reading anymore", RV_TRACE_LEVEL_ERROR);
		}
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX_process_read: read fifo is empty", RV_TRACE_LEVEL_ERROR);
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_update_serial_state
 *
 * Update serial state of the driver from following bitset of control line
 *	- DIO_SA
 *	- DIO_SB
 *	- DIO_X
 *
 * @param	current_device_p:		reference to the current device
 * @param	current_control_p:		control line used to update serial state
 *
 * @return	DRV_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_update_serial_state(T_USBFAX_USB_DEVICE* current_device_p, T_DIO_CTRL_LINES* current_control_p)
{
	BOOLEAN	serial_state_changed = FALSE;

	/* update bTxCarrier bit (DSR)
	 * if the DIO_SA bit is set to 0, the device is ready to communicate
	 */
	
	USBFAX_TRACE_WARNING_PARAM("USBFAX: USBFAX_usbfax_update_serial_state: current_control_p->state : ", current_control_p->state);

	if(current_control_p->state & DIO_SA)
	{
		/* Check if DSR signal has changed */
		if(current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_TX_CARRIER)
		{
			serial_state_changed = TRUE;
		}

		/* set DSR to 0 */
		current_device_p->usb_serial_state &= ~USBFAX_SERIAL_STATE_TX_CARRIER;
	}
	else
	{
		/* Check if DSR signal has changed */
		if( (current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_TX_CARRIER ) == 0)
		{
			serial_state_changed = TRUE;
		}

		/* set DSR to 1 */
		current_device_p->usb_serial_state |= USBFAX_SERIAL_STATE_TX_CARRIER;
	}

	/* update bRxCarrier bit (DCD)
	 * if the DIO_SB bit is set to 0, the device is ready to receive data
	 */
	if(current_control_p->state & DIO_SB)
	{
		if((current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RX_CARRIER))
		{
			serial_state_changed = TRUE;
		}

		/* set DCD to 0 */
		current_device_p->usb_serial_state &= ~USBFAX_SERIAL_STATE_RX_CARRIER;
	}
	else
	{
		if((current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RX_CARRIER) == 0)
		{
			serial_state_changed = TRUE;
		}

		/* set DCD to 1 */
		current_device_p->usb_serial_state |= USBFAX_SERIAL_STATE_RX_CARRIER;
	}

	/* update bRingSignal bit (Ring detection)
	 * if the DIO_X bit is set to 1, the device receive data
	 */
	if(current_control_p->state & DIO_RING)
	{
		if((current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RING_SIGNAL) == 0)
		{
			serial_state_changed = TRUE;
		}

		/* set Ring indicator set to 1 */
		current_device_p->usb_serial_state |= USBFAX_SERIAL_STATE_RING_SIGNAL;
	}
	else
	{
		if((current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RING_SIGNAL))
		{
			serial_state_changed = TRUE;
		}

		/* set Ring indicator set to 0 */
		current_device_p->usb_serial_state &= ~USBFAX_SERIAL_STATE_RING_SIGNAL;
	}

	return(serial_state_changed);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_update_control_line
 *
 * Update control line buffer from serial state of the driver
 * in case of read operation
 *
 * @param	current_device_p:	reference to the current device
 * @param	current_control_p:	control line to update
 *
 * @return	DRV_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_update_control_line(T_USBFAX_USB_DEVICE* current_device_p, T_DIO_CTRL_LINES* current_control_p)
{
	/* update DIO_SA bit of the line control (DSR)
	 * if the DIO_SA bit is set to 0, the device is ready to communicate
	 */
	if(current_device_p->usb_line_state.host_dtr == 1)
	{
		/* DTR = 1 */
		/* DIO_SA bit is set to 0 */
		/* device ready to communicate */
		current_control_p->state &= ~DIO_SA;
	}
	else
	{
		/* DTR = 0 */
		/* DIO_SA bit is set to 1 */
		current_control_p->state |= DIO_SA;
	}

	/* update ring indicator bit of the control line */
	if(current_device_p->usb_line_state.host_rts == 1)
	{
		/* DCD = 1 *
		 * DIO_SB bit is set to 0 
		 * device ready to receive */
		current_control_p->state &= ~DIO_SB;
	}
	else
	{
		/* DCD = 0 */
		/* DIO_SB bit is set to 1 */
		current_control_p->state |= DIO_SB;
	}

	/* update ring indicator bit of the control line */
	if(current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_RING_SIGNAL)
	{
		/* set RING indicator to 1 */
		/* incoming call */
		current_control_p->state |= DIO_X;
	}
	else
	{
		/* set RING indicator to 0 */
		current_control_p->state &= ~DIO_X;
	}

	/* update ring indicator bit of the control line */
	if(current_device_p->usb_serial_state & USBFAX_SERIAL_STATE_BREAK)
	{
		/* Break received */
		/* set BREAK indicator to 1 */
		current_control_p->state |= DIO_BRK;
		
		/* Reset break status of the serial state */
		current_device_p->usb_serial_state &= ~USBFAX_SERIAL_STATE_BREAK;

	}
	else
	{
		/* No Break vendor request received */
		/* set BREAK indicator to 0 */
		current_control_p->state &= ~DIO_BRK;
	}


	return((T_RV_RET)DRV_OK);
}


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_get_device
 *
 * This function gets a reference to a device instance 
 * from his device identifier.
 *
 * @param device_id		:	Device indentifier
 *
 * @return	T_USBFAX_USB_DEVICE*		:	refernce to a device instance
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_USB_DEVICE*	usbfax_get_device(U32 device_id)
{	
	U32					device_index	= 0;
	T_USBFAX_USB_DEVICE		*current_device_p = NULL;
	BOOLEAN					found			= FALSE;
		
	/* For each device contained in the array of devices */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];

		/* Check if device identifier is matching */
		if(device_id == current_device_p->device_number)
		{
			/* Stop search */
			found = TRUE;
			break;
		}
	}

	if(found == TRUE)
	{
		/* return device instance */
		return(current_device_p);
	}
	else
	{
		/* device has not been found */
		return(NULL);
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_look_for_endpoint_type
 *
 * From the interface identifier and an endpoint identifier,
 * this function search type of endpoint.
 *
 * @param	interface_id	:	interface identifier
 *			endpoint_id		:	endpoint identifier
 *
 * @return	T_USBFAX_ENDPOINT_TYPE	:	endpoint type (control, interrupt, bulk IN, bulk out)
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_ENDPOINT_TYPE	usbfax_look_for_endpoint_type(U16 interface_id, U16 endpoint_id)
{	
	U32					device_index	= 0;
	T_USBFAX_USB_DEVICE		*current_device_p = NULL;
	T_USBFAX_ENDPOINT_TYPE	returned_type	= USBFAX_UNKNOWN;

	/* For each device contained in the array of devices */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		/* Get device instance */
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];

		/* Check if the endpoint configuration with an control endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_control_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id == endpoint_id) )
		{
			returned_type = USBFAX_CONTROL_CONTEXT;
			break;
		}

		/* Check if the endpoint configuration with an interrupt endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_int_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_int_context.endpoint_id == endpoint_id) )
		{
			returned_type = USBFAX_INT_CONTEXT;
			break;
		}

		/* Check if the endpoint configuration with a bulk IN endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_tx_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_tx_context.endpoint_id == endpoint_id) )
		{
			returned_type = USBFAX_TX_CONTEXT;
			break;
		}

		/* Check if the endpoint configuration with a bulk OUT endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_rx_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_rx_context.endpoint_id == endpoint_id) )
		{
			returned_type = USBFAX_RX_CONTEXT;
			break;
		}
	}

	return(returned_type);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_wait_for_message.
 *
 * This function can wait to a specified message 
 *
 * @param	msg_id	Waited message ID, null for any message.
 *
 * @return	The received message.
 *---------------------------------------------------------------------------------* 
 */
T_RV_HDR*	usbfax_wait_for_message(U32 msg_id)
{
	T_RV_HDR * msg_p;
	
	for (;;)
	{
		/* Waiting for an event, blocking USBFAX task. */
		if (rvf_wait(0xFFFF, 0) & RVF_TASK_MBOX_0_EVT_MASK)
		{
			msg_p = (T_RV_HDR*)rvf_read_mbox(0);
			if (msg_p != NULL) 
			{
				if (msg_id)
				{
					/* If expected msg received, return it */
					if (msg_p->msg_id == msg_id)
					{
						return msg_p;
					}

					rvf_free_buf(msg_p);
				}
				else 
				{
					return msg_p;
				}
			}
		}
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_suscribe_to_cdc_interfaces
 *
 * This function suscribes to all interfaces of the USB configuration
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN	usbfax_suscribe_to_cdc_interfaces()
{	
	T_RVM_RETURN			suscribe_result = RVM_OK;
	U32					device_index	= 0;
	T_USBFAX_USB_DEVICE	*current_device_p = NULL;

	U8					current_interface;

	/* Boolean array indicating if the interface has been suscribed */
	BOOLEAN	interface_table[USBFAX_INTERFACE_MAX];

	/* Initialize array */
	memset(interface_table, 0x00, USBFAX_INTERFACE_MAX * sizeof(U8));

	/* For each device contained in the array of devices */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];

		/* Get interface of the control endpoint */
		current_interface = current_device_p->usb_ep_cnfg->ep_control_context.interface_id;
		if(interface_table[current_interface] == FALSE)
		{
			if(usbfax_suscribe_to_interfaces(current_interface) != RVM_OK)
			{
				suscribe_result = RV_INTERNAL_ERR;
			}
			interface_table[current_interface] = TRUE;
		}

		// usbfax_prepare_vendor_rx_buffer_request(current_device_p);

		/* Get interface of the interrupt endpoint */
		current_interface = current_device_p->usb_ep_cnfg->ep_int_context.interface_id;
		if(interface_table[current_interface] == FALSE)
		{
			if(usbfax_suscribe_to_interfaces(current_interface) != RVM_OK)
			{
				suscribe_result = RV_INTERNAL_ERR;
			}
			interface_table[current_interface] = TRUE;
		}

		/* Get interface number of the IN Bulk endpoint */
		current_interface = current_device_p->usb_ep_cnfg->ep_tx_context.interface_id;
		if(interface_table[current_interface] == FALSE)
		{
			if(usbfax_suscribe_to_interfaces(current_interface) != RVM_OK)
			{
				suscribe_result = RV_INTERNAL_ERR;
			}
			interface_table[current_interface] = TRUE;
		}

		/* Get interface number of the OUT Bulk endpoint */
		current_interface = current_device_p->usb_ep_cnfg->ep_rx_context.interface_id;
		if(interface_table[current_interface] == FALSE)
		{
			if(usbfax_suscribe_to_interfaces(current_interface) != RVM_OK)
			{
				suscribe_result = RV_INTERNAL_ERR;
			}
			interface_table[current_interface] = TRUE;
		}
	}
	
	return(suscribe_result);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_post_vendor_rx_buffers
 *
 * Initialize vendor buffer of all devices
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN	usbfax_post_vendor_rx_buffers()
{	
	T_RVM_RETURN			ret = RVM_OK;
	U32					device_index	= 0;
	T_USBFAX_USB_DEVICE	*current_device_p = NULL;

	U8					current_interface;

	/* For all devices of the current configuration */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		/* Post a RX buffer to USB SWE for receiption of vendor requests */
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];
		usbfax_prepare_vendor_rx_buffer_request(current_device_p);
	}
	
	return(ret);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_usb_disconnected
 *
 * This function is called in case of usb disconnect cable
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN	usbfax_process_usb_disconnected()
{	
	T_RVM_RETURN			ret = RVM_OK;
	U32					device_index		= 0;
	T_USBFAX_USB_DEVICE	*current_device_p	= NULL;
	T_RV_RET				ret_reclaim			= RV_OK;

	U8					current_interface;

	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];
		
		if(current_device_p != NULL)
		{
			/* check if DTR of the device is raised */
			/* and a user is connected				*/
			if((current_device_p->usb_line_state.host_dtr == 1)
				|| (current_device_p->user_connected == TRUE))
			{

				current_device_p->usb_line_state.host_dtr = 0;
				current_device_p->usb_line_state.host_rts = 0;

				current_device_p->usb_serial_state = 0;

				current_device_p->dtr_host_connected = FALSE;
				
				/* Notify DIO that device is disconnected with the host */
				/* because of a unplug usb cable */
				usbfax_signal(current_device_p, DRV_SIGTYPE_DISCONNECT);

				/* cancel RX vendor request buffer */
				ret_reclaim	= usb_reclaim_rx_buffer (
					current_device_p->usb_ep_cnfg->ep_control_context.interface_id,
					current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id );

				if(ret_reclaim != RV_OK)
				{
					USBFAX_SEND_TRACE("USBFAX: Failed to reclaim RX vendor request buffer", RV_TRACE_LEVEL_DEBUG_LOW);
				}

				/* cancel RX data buffer */
				ret_reclaim	= usb_reclaim_rx_buffer (	
					current_device_p->usb_ep_cnfg->ep_rx_context.interface_id,
					current_device_p->usb_ep_cnfg->ep_rx_context.endpoint_id );

				if(ret_reclaim != RV_OK)
				{
					USBFAX_SEND_TRACE("USBFAX: Failed to reclaim RX data buffer", RV_TRACE_LEVEL_DEBUG_LOW);
				}
				
				
				/* Empty read fifo request */
				usbfax_empty_queue_request(current_device_p->device_number, 
												USBFAX_XFER_READ_REQUEST);
				
				/* Empty write fifo request */
				usbfax_empty_queue_request(current_device_p->device_number, 
												USBFAX_XFER_WRITE_REQUEST);

			}
		}
		else
		{
			ret = RVM_INTERNAL_ERR;
		}

	}
	
	
	return(ret);
}


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_suscribe_to_cdc_interfaces
 *
 * This function suscribes to an interface 
 *
 * @param	interface_number	:	Interface identifier 
 *
 * @return	RVM_OK	:			Suscribe to interface successful
 *			RV_INTERNAL_ERR	:	Failed to suscribe
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN	usbfax_suscribe_to_interfaces(U8 interface_number)
{
	T_RVM_RETURN	suscribe_result = RVM_OK;
	
	/* storage for awaited messages
	*/ 
	T_RV_HDR  *msg_p;
	T_RV_HDR  *msg_p_ready;

	USBFAX_TRACE_WARNING_PARAM("USBFAX: main task suscribe to interface", interface_number);

	/* Subscribe to interface number given in parameter */
	if (usb_fm_subscribe(interface_number, usbfax_env_ctrl_blk_p->usbfax_return_path) != RVM_OK)
	{
		suscribe_result = RV_INTERNAL_ERR;
	}
	else
	{
		/* Wait for a subscription result from the USB SWE */
		msg_p = usbfax_wait_for_message(USB_FM_RESULT_MSG);
		if(((T_USB_FM_RESULT_MSG*)msg_p)->result != succes)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		rvf_free_buf(msg_p);
	}


	/* Send ready to USB using interface number given in parameter */
	if (usb_fm_ready(interface_number, usbfax_env_ctrl_blk_p->usbfax_return_path) != RVM_OK)
	{
		suscribe_result = RV_INTERNAL_ERR;
	}
	else
	{
		/* Wait for a ready result from the USB SWE */
		msg_p_ready = usbfax_wait_for_message(USB_FM_RESULT_MSG);
		if(((T_USB_FM_RESULT_MSG*)msg_p_ready)->result != succes)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		rvf_free_buf(msg_p_ready);
	}

	return(suscribe_result);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_look_for_associated_device
 *
 * Look for an associated device from the interface number and endpoint number 
 * given in parameter.
 *
 * @param	interface_id	:	interface identifier
 *			endpoint_id		:	endpoint identifier
 *
 * @return	T_USBFAX_USB_DEVICE*		:	reference to a found device instance 
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_USB_DEVICE*	usbfax_look_for_associated_device(U16 interface_id, U16 endpoint_id)
{	
	U32					device_index	= 0;
	T_USBFAX_USB_DEVICE	*current_device_p = NULL;
	BOOLEAN					found			= FALSE;

	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		current_device_p = &USBFAX_TAB_OF_DEVICES[device_index];

		/* Check if endpoint match with the control endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_control_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_control_context.endpoint_id == endpoint_id) )
		{
			found = TRUE;
			break;
		}

		/* Check if endpoint match with the interrupt endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_int_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_int_context.endpoint_id == endpoint_id) )
		{
			found = TRUE;
			break;
		}

		/* Check if endpoint match with the TX bulk endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_tx_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_tx_context.endpoint_id == endpoint_id) )
		{
			found = TRUE;
			break;
		}

		/* Check if endpoint match with the RX bulk endpoint */
		if(	(current_device_p->usb_ep_cnfg->ep_rx_context.interface_id == interface_id)
			&& (current_device_p->usb_ep_cnfg->ep_rx_context.endpoint_id == endpoint_id) )
		{
			found = TRUE;
			break;
		}
	}

	if(found == TRUE)
	{
		/* associated device has been found */
		/* returns a reference to the device */
		return(current_device_p);
	}
	else
	{
		return(NULL);
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_in_progress
 *
 * Check if a write request is in progress for a specified device
 *
 * @param T_USBFAX_USB_DEVICE*		:	reference to a device instance 
 *
 * @return	TRUE		: request is in progress
 *			FALSE		: otherwise
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN		usbfax_transfer_in_progress(T_USBFAX_USB_DEVICE* current_device_p)
{
	BOOLEAN	result = FALSE;

	U8				current_request_index;	
	T_USBFAX_XFER*	current_write_request_p = NULL;

	current_write_request_p = usbfax_get_current_request(&current_device_p->write_request_queue);

	/* Check if write FIFO is empty */
	/* if not a write transfer is in progress */
	if(current_write_request_p)
	{
		if(current_write_request_p->sent_length < current_write_request_p->segment_size)
		{
			result = TRUE;
		}
	}

	return(result);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_queue_full
 *
 * Indicates if the read or write request FIFO is full.
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	TRUE		: FIFO full
 *			FALSE		: not yet full
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_queue_full(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type)
{	
	T_USBFAX_USB_DEVICE	*current_device_p = NULL;
	BOOLEAN				queue_full = FALSE;

	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		/* Following request type (read or write)*/
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
				
				/* If next element of the ciruclar FIFO is the last element */
				/* Read FIFO is full therefore a new element cannot be inserted */
				if((current_device_p->read_request_queue.last_element + 1) % USBFAX_MAX_REQUESTS
					== current_device_p->read_request_queue.first_element)
				{
					queue_full = TRUE;
				}

			break;

			case USBFAX_XFER_WRITE_REQUEST :
				/* If next element of the ciruclar FIFO is the last element */
				/* Write FIFO is full therefore a new element cannot be inserted */
				if((current_device_p->write_request_queue.last_element + 1) % USBFAX_MAX_REQUESTS
					== current_device_p->write_request_queue.first_element)
				{
					queue_full = TRUE;
				}
			break;
			
			default :
				/* request type is invalid */
				/* in the doubt, returns full */
				queue_full = TRUE;
			break;
		}
	}

	return(queue_full);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_enqueue_request
 *
 * stores a new request in read or write queue
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	RVM_OK				:
 *			RV_INTERNAL_ERR		:	FIFO full, cannot insert a new request
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_enqueue_request(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type, T_USBFAX_XFER* xfer_request)
{	
	T_USBFAX_USB_DEVICE*		current_device_p			= NULL;
	T_USBFAX_XFER*			xfer_request_in_queue	= NULL;

	T_RVM_RETURN			ret = RVM_OK;
	BOOLEAN					process_request = FALSE;

	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		/* 
		 * Following request type (read or write),
		 * select the associated FIFO
		 */
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
				/* Check if the queue is empty or not */
				if(current_device_p->read_request_queue.current_element == -1)
				{
					/* Queue is empty then initialize queue */
					usbfax_initialize_queue_request(&current_device_p->read_request_queue);
					process_request = TRUE;
				} 
				else if((current_device_p->read_request_queue.last_element + 1) % USBFAX_MAX_REQUESTS
					!= current_device_p->read_request_queue.first_element)
				{
					current_device_p->read_request_queue.last_element = 
						(current_device_p->read_request_queue.last_element + 1)  % USBFAX_MAX_REQUESTS;

					process_request = FALSE;
				}
				else
				{
					ret = RV_INTERNAL_ERR;
				}
				
				if(ret == RVM_OK)
				{
					xfer_request_in_queue = &(current_device_p->read_request_queue.tab_xfer[current_device_p->read_request_queue.last_element]);
					memcpy(xfer_request_in_queue, xfer_request, sizeof(T_USBFAX_XFER));
					
					// USBFAX_SEND_TRACE("USBFAX_enqueue_request(read): usbfax_next_segment ",RV_TRACE_LEVEL_DEBUG_LOW);
					usbfax_next_segment(xfer_request_in_queue);

					if(process_request == TRUE)
					{
						process_request = FALSE;
						current_device_p->buffer_rx_temp.size = 0;
						//YL
						USBFAX_SEND_TRACE("USB_enqueue_request: start new read", RV_TRACE_LEVEL_ERROR);
						usbfax_process_read(current_device_p, xfer_request_in_queue, 0);
					}
					else
					{
						xfer_request_in_queue->usb_posted = FALSE;
					}
				}
			break;

			case USBFAX_XFER_WRITE_REQUEST :
				if(current_device_p->write_request_queue.current_element == -1)
				{
					usbfax_initialize_queue_request(&current_device_p->write_request_queue);
					process_request = TRUE;
				}
				else if((current_device_p->write_request_queue.last_element + 1) % USBFAX_MAX_REQUESTS
					!= current_device_p->write_request_queue.first_element)
				{
					current_device_p->write_request_queue.last_element = 
						(current_device_p->write_request_queue.last_element + 1)  % USBFAX_MAX_REQUESTS;
					
					process_request = FALSE;
				}
				else
				{
					USBFAX_SEND_TRACE("USBFAX_enqueue_request(write): no more place in fifo",RV_TRACE_LEVEL_ERROR);
					ret = RV_INTERNAL_ERR;
				}

				USBFAX_TRACE_WARNING_PARAM("usbfax_enqueue_request: first_element", 
					current_device_p->write_request_queue.first_element);

				USBFAX_TRACE_WARNING_PARAM("usbfax_enqueue_request: current_element", 
					current_device_p->write_request_queue.current_element);

				USBFAX_TRACE_WARNING_PARAM("usbfax_enqueue_request: last_element", 
					current_device_p->write_request_queue.last_element);

				if(ret == RVM_OK)
				{
					xfer_request_in_queue = &(current_device_p->write_request_queue.tab_xfer[current_device_p->write_request_queue.last_element]);
					memcpy(xfer_request_in_queue, xfer_request, sizeof(T_USBFAX_XFER));
					
					USBFAX_SEND_TRACE("USBFAX_enqueue_request(write): usbfax_next_segment ",RV_TRACE_LEVEL_DEBUG_LOW);
					usbfax_next_segment(xfer_request_in_queue);

					if((process_request == TRUE)
						&& (usbfax_env_ctrl_blk_p->host_serial_driver_connected == TRUE))
					{
						USBFAX_SEND_TRACE("USBFAX_enqueue_request(write): Send data",RV_TRACE_LEVEL_DEBUG_LOW);
						current_device_p->buffer_tx_temp.size = 0;
						process_request = FALSE;
						/* correct bug for first curent element use */
						/* current_device_p->write_request_queue.current_element = -1;*/
						/*process the request*/
						usbfax_process_requests_from_write_fifo(current_device_p);
						
					
					}
				}
			break;

			default :
				ret = RVM_INTERNAL_ERR;
			break;
		}
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_dequeue_request
 *
 * Dequeue a request of the FIFO
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	RVM_OK				:
 *			RV_INTERNAL_ERR		:	caller tried to dequeue a request and the queue is empty
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_dequeue_request(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type)
{	
	T_USBFAX_USB_DEVICE*		current_device_p			= NULL;
	T_USBFAX_XFER*			xfer_request_in_queue	= NULL;

	T_RVM_RETURN	ret = RVM_OK;

	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
				if(current_device_p->read_request_queue.current_element != -1)
				{
					if(current_device_p->read_request_queue.first_element == 
						current_device_p->read_request_queue.last_element)
					{
						usbfax_empty_queue_request(device_id, USBFAX_XFER_READ_REQUEST);
					}
					else
					{
						/* POP 05/07/2004 */
						/* If the current element is the element to dequeue */
						/* therefore, we need take the next element */
						if(current_device_p->read_request_queue.first_element
							== current_device_p->read_request_queue.current_element)
						{
							T_USBFAX_XFER*	current_read_request_p 
								= usbfax_next_queue_request(&current_device_p->read_request_queue);
						}
						/* End POP 05/07/2004 */

						current_device_p->read_request_queue.first_element = 
							(current_device_p->read_request_queue.first_element + 1)  % USBFAX_MAX_REQUESTS;
					}
				}
				else
				{
					ret = RVM_INTERNAL_ERR;
				}

			break;
			
			case USBFAX_XFER_WRITE_REQUEST :
				if(current_device_p->write_request_queue.current_element != -1)
				{
					if(current_device_p->write_request_queue.first_element == 
						current_device_p->write_request_queue.last_element)
					{
						/* 
						 * There is just one element in the write FIFO, therefore queue is flushed
						 */
						usbfax_empty_queue_request(device_id, USBFAX_XFER_WRITE_REQUEST);
						USBFAX_SEND_TRACE("usbfax_dequeue_request: Write fifo is empty", RV_TRACE_LEVEL_DEBUG_LOW);
					}
					else
					{
						/* POP 05/07/2004 */
						/* If the current element is the element to dequeue */
						/* therefore, we need take the next element */
						if(current_device_p->write_request_queue.first_element
							== current_device_p->write_request_queue.current_element)
						{
							T_USBFAX_XFER*	current_write_request_p 
								= usbfax_next_queue_request(&current_device_p->write_request_queue);
						}
						/* End POP 05/07/2004 */

						current_device_p->write_request_queue.first_element = 
							(current_device_p->write_request_queue.first_element + 1)  % USBFAX_MAX_REQUESTS;
					}
				}
				else
				{
					ret = RVM_INTERNAL_ERR;
				}

				USBFAX_TRACE_WARNING_PARAM("usbfax_dequeue_request: first_element", 
					current_device_p->write_request_queue.first_element);

				USBFAX_TRACE_WARNING_PARAM("usbfax_dequeue_request: current_element", 
					current_device_p->write_request_queue.current_element);

				USBFAX_TRACE_WARNING_PARAM("usbfax_dequeue_request: last_element", 
					current_device_p->write_request_queue.last_element);
			break;
		}
	}
	
	return(RVM_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_initialize_queue_request
 *
 * Initiaize a queue request
 *
 * @param	T_USBFAX_XFER_QUEUE*		:	current_queue_request
 *
 * @return	RVM_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_initialize_queue_request(T_USBFAX_XFER_QUEUE* current_queue_request)
{	
	current_queue_request->current_element	= 0;
	current_queue_request->first_element	= 0;
	current_queue_request->last_element		= 0;
	
	return(RVM_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_next_queue_request
 *
 * Get next request of the request queue 
 *
 * @param 
 *
 * @return
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_XFER*	usbfax_next_queue_request(T_USBFAX_XFER_QUEUE* current_queue_request)
{	
	T_USBFAX_XFER*	current_request_p;

	if(current_queue_request->current_element == current_queue_request->last_element)
	{
		current_request_p = NULL;
	}
	else
	{
		current_queue_request->current_element = 
			(current_queue_request->current_element + 1)  % USBFAX_MAX_REQUESTS;

		current_request_p = &current_queue_request->tab_xfer[current_queue_request->current_element];
	}

	return(current_request_p);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_get_current_request
 *
 * Get next request of the request queue 
 *
 * @param 
 *
 * @return
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_XFER*	usbfax_get_current_request(T_USBFAX_XFER_QUEUE* current_queue_request)
{	
	T_USBFAX_XFER*	current_request_p;

	U8	current_request_index = current_queue_request->current_element;

	if(current_request_index != 0xFF)
	{
		current_request_p = &current_queue_request->tab_xfer[current_request_index];
	}
	else
	{
		current_request_p = NULL;
	}

	return(current_request_p);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_get_queue_element
 *
 * Get current read or write request of the FIFO
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	T_USBFAX_XFER*	:	request instance 
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_XFER*	usbfax_get_queue_element(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type)
{
	T_USBFAX_XFER*			request_p		= NULL;
	T_USBFAX_USB_DEVICE*		current_device_p	= NULL;
	
	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
				if(current_device_p->read_request_queue.first_element != -1)
				{
					request_p = 
						&current_device_p->read_request_queue.tab_xfer[current_device_p->read_request_queue.first_element];
				}
			break;

			case USBFAX_XFER_WRITE_REQUEST :
				if(current_device_p->write_request_queue.first_element != -1)
				{
					request_p = 
						&current_device_p->write_request_queue.tab_xfer[current_device_p->write_request_queue.first_element];
				}
			break;
		}
	}

	return(request_p);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_requests_from_write_fifo
 *
 * Get current read or write request of the FIFO
 *
 * @param	device_id			:	Device identifier
 *
 * @return	BOOLEAN	:	TRUE if a request is pending
 *						FALSE if there is no request in queue
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_process_requests_from_write_fifo(T_USBFAX_USB_DEVICE* current_device_p)
{
	T_USBFAX_XFER*		current_write_request_p	= NULL;
	INT8					current_request_index;	
	T_RVM_RETURN	ret	= RVM_OK;

	if(current_device_p != NULL)
	{
		/* Get next request in the write queue */
		current_write_request_p = usbfax_get_current_request(&current_device_p->write_request_queue);

		if (current_write_request_p != NULL)
		{
			INT8	current_request_index = current_device_p->write_request_queue.current_element;
			if(current_request_index != -1)
			{
				/* Current write request has been found in the FIFO */
				USBFAX_TRACE_WARNING_PARAM("usbfax_process_requests_from_write_fifo: index of write request to process", current_request_index);
			}

			/* write request has been found in the write FIFO */
			current_device_p->buffer_tx_temp.size = 0;
			/* check wether we process control or data request */
			/* The serial state of the device is updated with the control line */
			if(TRUE == usbfax_update_serial_state(current_device_p, &(current_write_request_p->line_control)))
			{
				/* If the serial state has changed, new serial state is sent to host */
				current_device_p->signal_write_serial_state = TRUE;
				usbfax_process_control_lines(current_device_p);

				ret = RVM_OK;
			}
			else
			{
				BOOLEAN	write_fifo_is_empty = FALSE;
				
				/* In case of a dio_buffer with only the control information filled
				   and no change on the serial state
				   then suppress current request from the FIFO and process next inside
				   if there is.
				*/
				if(current_write_request_p->buffer_context == NULL)
				{
					USBFAX_SEND_TRACE("USBFAX: usbfax_process_requests_from_write_fifo, suppress a control infomation request",RV_TRACE_LEVEL_ERROR);

					write_fifo_is_empty = usbfax_suppress_write_requests_fifo(current_device_p);

					/* Get next request in the write queue */
					current_write_request_p = usbfax_next_queue_request(&current_device_p->write_request_queue);
				}
			}
			
				if(current_write_request_p != NULL)
				{
					/*the buffer is for data*/
					ret = usbfax_process_write(current_device_p, current_write_request_p);
				}
			}
		else
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_process_requests_from_write_fifo, no request to process",RV_TRACE_LEVEL_ERROR);

			if(usbfax_queue_is_empty(current_device_p->device_number, USBFAX_XFER_WRITE_REQUEST) == TRUE)
			{
				ret = RVM_INTERNAL_ERR;
				USBFAX_SEND_TRACE("usbfax_process_requests_from_write_fifo: write fifo is empty", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			else
			{
				INT8	current_request_index = 0;
				USBFAX_SEND_TRACE("usbfax_process_requests_from_write_fifo: Oops Oops Oops Oops", RV_TRACE_LEVEL_ERROR);

				current_request_index = current_device_p->write_request_queue.current_element;
				if(current_request_index != -1)
				{
					/* Current write request has been found in the FIFO */
					USBFAX_TRACE_WARNING_PARAM("usbfax_process_requests_from_write_fifo: index of write request not processed", current_request_index);
				}
			}
		}
	}

	if(current_write_request_p == NULL)
	{
		return FALSE;
	}
	else
	{
			return TRUE;
	}

}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_empty_queue_request
 *
 * Empty a read/write request FIFO
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	RVM_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_empty_queue_request(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type)
{	
	T_USBFAX_USB_DEVICE*		current_device_p			= NULL;
	
	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
				current_device_p->read_request_queue.first_element	= -1;
				current_device_p->read_request_queue.last_element		= -1;
				current_device_p->read_request_queue.current_element	= -1;

				current_device_p->buffer_rx_temp.size			= 0;
				current_device_p->buffer_rx_temp.short_packet	= FALSE;
			break;

			case USBFAX_XFER_WRITE_REQUEST :
				current_device_p->write_request_queue.first_element	= -1;
				current_device_p->write_request_queue.last_element	= -1;
				current_device_p->write_request_queue.current_element	= -1;

				current_device_p->buffer_tx_temp.size			= 0;
				current_device_p->buffer_tx_temp.short_packet	= FALSE;
			break;
		}
	}

	return(RVM_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_empty_queue_request
 *
 * Empty a read/write request FIFO
 *
 * @param	device_id			:	Device identifier
 *			xfer_request_type	:	Request type (read or write)
 *
 * @return	RVM_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_queue_is_empty(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type)
{	
	T_USBFAX_USB_DEVICE*	current_device_p	= NULL;
	BOOLEAN	fifo_is_empty						= FALSE;

	U8	current_request_index;

	current_device_p = usbfax_get_device(device_id);
	if(current_device_p != NULL)
	{
		switch(xfer_request_type)
		{
			case USBFAX_XFER_READ_REQUEST  :
			{
				current_request_index = 
					current_device_p->read_request_queue.current_element;
				if(current_request_index == 0xFF)
					fifo_is_empty = TRUE;
			}
			break;

			case USBFAX_XFER_WRITE_REQUEST :
				current_request_index	= 
					current_device_p->write_request_queue.current_element;
				if(current_request_index == 0xFF)
					fifo_is_empty = TRUE;
			break;
		}
	}

	return(fifo_is_empty);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_signal
 *
 * DIO callbak is called with parameter a signal identifier
 *
 * @param	current_device_p	:	pointer to device reference
 * @param	signal_id			:	signal identifier
 *
 * @return	RVM_OK
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_signal(T_USBFAX_USB_DEVICE* current_device_p, U8 signal_id)
{
	T_DRV_SIGNAL	signal_to_send;

	USBFAX_TRACE_WARNING_PARAM("USBFAX: DIO SIGNAL : ", signal_id);

	if(usbfax_env_ctrl_blk_p->signal_callback)
	{
		signal_to_send.DrvHandle  = usbfax_env_ctrl_blk_p->dvr_handle;
		signal_to_send.SignalType = signal_id;
		signal_to_send.DataLength = sizeof(U32);
		signal_to_send.UserData	  = (T_VOID_STRUCT*)&current_device_p->device_number;

		(*usbfax_env_ctrl_blk_p->signal_callback)(&signal_to_send);
	}
	else
	{
		USBFAX_SEND_TRACE("USBFAX: Invalid value for user callback",RV_TRACE_LEVEL_ERROR);
	}

	USBFAX_SEND_TRACE("USBFAX: Exit DIO SIGNAL", RV_TRACE_LEVEL_DEBUG_LOW);

	return(RVM_OK);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_next_segment
 *
 * Get next segment of the current request
 *
 * @param	current_request_p	:	reference to a read/write request
 *
 * @return
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_next_segment(T_USBFAX_XFER* current_request_p)
{
	T_RVM_RETURN	ret = RVM_OK;

		
	if(current_request_p != NULL)
	{
		if (current_request_p->buffer_context != NULL)
		{
			if((current_request_p->buffer_context->c_dio_segment != 0)&&
				(current_request_p->segment_index < current_request_p->buffer_context->c_dio_segment))
			{
				current_request_p->segment = 
					current_request_p->buffer_context->ptr_dio_segment[current_request_p->segment_index].ptr_data;
	
				current_request_p->segment_size =
					current_request_p->buffer_context->ptr_dio_segment[current_request_p->segment_index].c_data;

				current_request_p->sent_length = 0;

				if(current_request_p->segment_index != 255)
					current_request_p->segment_index++;

				if(current_request_p->segment == NULL)
				{
					USBFAX_SEND_TRACE("USBFAX: Pb->segment is NULL",RV_TRACE_LEVEL_ERROR);
				}
			}
			else
			{
				/* There is no segment anymore to process */
				USBFAX_SEND_TRACE("USBFAX: usbfax_next_segment, no more segment ",RV_TRACE_LEVEL_ERROR);
				current_request_p->segment		= NULL;
				current_request_p->segment_size = 0;
			}
		}

		current_request_p->sent_length = 0;

	}
	else
	{
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_post_usb_rx_buffer
 *
 * This function detects presence of a escape sequence in data of the read request
 *
 * @param current_device_p:	reference to the current device
 *
 * @return	TRUE			: when escape sequence has been found
 *			FALSE			: escape sequence not found
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_post_usb_rx_buffer(T_USBFAX_USB_DEVICE* current_device_p)
{
	T_RVM_RETURN	ret = RVM_OK;

	T_USBFAX_XFER*	current_read_request_p 
		= usbfax_get_current_request(&current_device_p->read_request_queue);

	U16	size_to_request = 0;

	if(current_read_request_p->segment_size >= MAX_PACKET_USBFAX_SIZE)
	{
		size_to_request = MAX_PACKET_USBFAX_SIZE;
	}
	else
	{
		if(current_read_request_p->segment_size % 64)
		{
				size_to_request = (current_read_request_p->segment_size / 64) * 64;
		}
		else
		{
				size_to_request = current_read_request_p->segment_size;
		}
	}

	if(usb_set_rx_buffer(	current_device_p->usb_ep_cnfg->ep_rx_context.interface_id,
							current_device_p->usb_ep_cnfg->ep_rx_context.endpoint_id,
							current_device_p->buffer_rx_temp.buff, size_to_request) != RVM_OK)
	{
		USBFAX_SEND_TRACE("USBFAX_process_read: usb_set_rx_buffer error", RV_TRACE_LEVEL_ERROR);

		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_get_mem
 *
 * This function reserves memory for the data that will be send 
 * to or from the device.
 *
 * @param		buffer_pp:		pointer to a memory that will hold 
 *								the setup data
 * @param		size:			holds the size of the setup data buffer
 *
 * @return	RVM_OK, RVM_MEMORY_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN	usbfax_get_mem(U8** buffer_pp, U16 size)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;
	T_RV_RET ret = RVM_OK;

	/* Create buffer to contain the endpoint 0 setup request additional data*/
	mb_status = rvf_get_buf(usbfax_env_ctrl_blk_p->prim_mb_id, 
								(sizeof(U8) * size),
								((T_RVF_BUFFER**) buffer_pp));

	if (mb_status == RVF_RED) 
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 * The environemnt will cancel the USB instance creation.
		 */
		USBFAX_SEND_TRACE("USBFAX: Error to get memory ",RV_TRACE_LEVEL_ERROR);
		ret = RVM_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW) 
	{
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USBFAX_SEND_TRACE("USBFAX: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_change_esc_seq_state
 *
 * This function detects presence of a escape sequence in data of the read request
 *
 * @param current_device_p:	reference to the current device
 * @param request_p:		reference to a read request
 *
 * @return	TRUE			: when escape sequence has been found
 *			FALSE			: escape sequence not found
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBFAX_ESCAPE_STATE	usbfax_change_esc_seq_state(	T_USBFAX_USB_DEVICE* current_device_p, 
														T_USBFAX_ESCAPE_STATE esc_seq_new_state)
{
	current_device_p->esc_sequence_state = esc_seq_new_state;

	switch(current_device_p->esc_sequence_state)
	{
		case NO_ESCAPE_RECEIVED :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : NO_ESCAPE_RECEIVED",	RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case ESCAPE_RECEIVE_IDLE :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : ESCAPE_RECEIVE_IDLE", RV_TRACE_LEVEL_DEBUG_LOW);
		break;
		
		case ONE_ESCAPE_RECEIVED :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : ONE_ESCAPE_RECEIVED", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case TWO_ESCAPE_RECEIVED :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : TWO_ESCAPE_RECEIVED", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case THREE_ESCAPE_RECEIVED :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : THREE_ESCAPE_RECEIVED", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case COMPLETE_ESCAPE_SEQUENCE :
			USBFAX_SEND_TRACE("USBFAX: EscSeq State : COMPLETE_ESCAPE_SEQUENCE", RV_TRACE_LEVEL_DEBUG_LOW);
		break;
	}

	return(current_device_p->esc_sequence_state);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_escape_sequence_detect
 *
 * This function detects presence of a escape sequence in data of the read request
 *
 * @param current_device_p:	reference to the current device
 *
 * @return	TRUE			: when escape sequence has been found
 *			FALSE			: escape sequence not found
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_escape_sequence_detect(T_USBFAX_USB_DEVICE* current_device_p)
{
	U8	i = 0;
	BOOLEAN	escape_sequence_detect_running = FALSE;

	T_USBFAX_ESCAPE_STATE	previous_state = current_device_p->esc_sequence_state;

	if(current_device_p->buffer_rx_temp.size <= 3)
	{
		for(i = 0; (i < current_device_p->buffer_rx_temp.size) 
					&& (current_device_p->esc_sequence_state != NO_ESCAPE_RECEIVED); i++)
		{
			if (current_device_p->buffer_rx_temp.buff[i] == current_device_p->device_config.esc_char)
			{
				switch(current_device_p->esc_sequence_state)
				{
					case ESCAPE_RECEIVE_IDLE:
						usbfax_change_esc_seq_state(current_device_p, ONE_ESCAPE_RECEIVED);
					break;

					case ONE_ESCAPE_RECEIVED:
						usbfax_change_esc_seq_state(current_device_p, TWO_ESCAPE_RECEIVED);
					break;

					case TWO_ESCAPE_RECEIVED:
						usbfax_change_esc_seq_state(current_device_p, THREE_ESCAPE_RECEIVED);
					break;

					case THREE_ESCAPE_RECEIVED:
						usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
					break;

					default:
						USBFAX_SEND_TRACE("USBFAX: usbfax_escape_sequence_detect : Escape state error", 
							RV_TRACE_LEVEL_DEBUG_LOW);
				}
			}
			else
			{
				usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
			}
		}

		if (current_device_p->esc_sequence_state == NO_ESCAPE_RECEIVED)
		{
			/* pointer on the current request */
			T_USBFAX_XFER*	current_read_request_p 
				= usbfax_get_current_request(&current_device_p->read_request_queue);

			switch(previous_state)
			{
				case ONE_ESCAPE_RECEIVED:
					current_read_request_p->segment[0] = current_device_p->device_config.esc_char;
					current_read_request_p->sent_length			= 1;
					current_read_request_p->total_sent_length	= 1;
				break;

				case TWO_ESCAPE_RECEIVED:
					current_read_request_p->segment[0] = 
					current_read_request_p->segment[1] = current_device_p->device_config.esc_char;
					current_read_request_p->sent_length			= 2;
					current_read_request_p->total_sent_length	= 2;
				break;

				case THREE_ESCAPE_RECEIVED:
					current_read_request_p->segment[0] = 
					current_read_request_p->segment[1] = 
					current_read_request_p->segment[2] = current_device_p->device_config.esc_char;
					current_read_request_p->sent_length			= 3;
					current_read_request_p->total_sent_length	= 3;
				break;

				case NO_ESCAPE_RECEIVED:
				case ESCAPE_RECEIVE_IDLE:
				break;

				default:
					USBFAX_SEND_TRACE("USBFAX: usbfax_escape_sequence_detect : Escape state error", 
						RV_TRACE_LEVEL_DEBUG_LOW);
			}
		
			if	(	(previous_state == ONE_ESCAPE_RECEIVED)
				||	(previous_state == TWO_ESCAPE_RECEIVED)
				||	(previous_state == THREE_ESCAPE_RECEIVED)	)
			{
				usbfax_signal(current_device_p, DRV_SIGTYPE_READ);
			}

			usbfax_stop_timer(current_device_p);	
			usbfax_start_timer(current_device_p);
		}
		else if(current_device_p->esc_sequence_state == ESCAPE_RECEIVE_IDLE)
		{
			usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
			usbfax_stop_timer(current_device_p);	
			usbfax_start_timer(current_device_p);
		}
		else if (		(current_device_p->esc_sequence_state == ONE_ESCAPE_RECEIVED)
					||	(current_device_p->esc_sequence_state == TWO_ESCAPE_RECEIVED)
					||	(current_device_p->esc_sequence_state == THREE_ESCAPE_RECEIVED))
		{
			usbfax_stop_timer(current_device_p);	
			usbfax_start_timer(current_device_p);

			usbfax_post_usb_rx_buffer(current_device_p);

			escape_sequence_detect_running = TRUE;
		}
	}
	else
	{
		usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
		usbfax_stop_timer(current_device_p);	
		usbfax_start_timer(current_device_p);
	}

	return(escape_sequence_detect_running);
	}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_timer_escape_sequence
 *
 * This function detects presence of a escape sequence in data of the read request
 *
 * @param current_device_p:	reference to the current device
 * @param request_p:		reference to a read request
 *
 * @return	TRUE			: when escape sequence has been found
 *			FALSE			: escape sequence not found
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_timer_escape_sequence(T_USBFAX_USB_DEVICE* current_device_p)
{
	/* pointer on the current request */
	T_USBFAX_XFER*	current_read_request_p 
		= usbfax_get_current_request(&current_device_p->read_request_queue);

	switch(current_device_p->esc_sequence_state)
	{
		case NO_ESCAPE_RECEIVED:
			usbfax_change_esc_seq_state(current_device_p, ESCAPE_RECEIVE_IDLE);
		break;
		
		case ESCAPE_RECEIVE_IDLE:
			usbfax_change_esc_seq_state(current_device_p, ESCAPE_RECEIVE_IDLE);
		break;

		case ONE_ESCAPE_RECEIVED:
			usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
			current_read_request_p->segment[0] = current_device_p->device_config.esc_char;
			current_read_request_p->sent_length			= 1;
			current_read_request_p->total_sent_length	= 1;

			usbfax_stop_timer(current_device_p);	
			usbfax_start_timer(current_device_p);
		break;

		case TWO_ESCAPE_RECEIVED:
			usbfax_change_esc_seq_state(current_device_p, NO_ESCAPE_RECEIVED);
			current_read_request_p->segment[0] = current_device_p->device_config.esc_char;
			current_read_request_p->segment[1] = current_device_p->device_config.esc_char;
			current_read_request_p->sent_length			= 2;
			current_read_request_p->total_sent_length	= 2;

			usbfax_stop_timer(current_device_p);	
			usbfax_start_timer(current_device_p);
		break;

		case THREE_ESCAPE_RECEIVED:
			usbfax_stop_timer(current_device_p);
			usbfax_change_esc_seq_state(current_device_p, COMPLETE_ESCAPE_SEQUENCE);
		break;

		case COMPLETE_ESCAPE_SEQUENCE:
		break;
	}

	if (current_device_p->esc_sequence_state == COMPLETE_ESCAPE_SEQUENCE)
	{
		/* Set DIO_ESC bit of the line control of the current read request */
		current_read_request_p->line_control.state |= DIO_ESC;
		usbfax_signal(current_device_p, DRV_SIGTYPE_READ);

		usbfax_change_esc_seq_state(current_device_p, ESCAPE_RECEIVE_IDLE);
	}
	else if ((current_device_p->esc_sequence_state == NO_ESCAPE_RECEIVED)
			&& (current_read_request_p->sent_length > 0))
	{
		usbfax_signal(current_device_p, DRV_SIGTYPE_READ);
	}

	return TRUE;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbfax_process_requests_from_write_fifo
 *
 * Suppress current element from the write request fifo
 * by shifting all incoming request to process 
 *
 * @param	device_id			:	Reference to the current device
 *
 * @return	BOOLEAN	:	TRUE	when write requests of FIFO is empty
 *						FALSE	otherwise
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOLEAN	usbfax_suppress_write_requests_fifo(T_USBFAX_USB_DEVICE* current_device_p)
	{
	U8					current_request_index;	
	T_RVM_RETURN		ret	= RVM_OK;
	BOOLEAN				waiting_write_requests = TRUE;

	T_USBFAX_XFER*		srce_write_request_p	= NULL;
	T_USBFAX_XFER*		dest_write_request_p	= NULL;

	INT8	dest_idx_element =  current_device_p->write_request_queue.current_element;
	INT8	srce_idx_element = (dest_idx_element +1) % USBFAX_MAX_REQUESTS;


	/* On deleting current write request, write fifo is becoming empty*/
	if(current_device_p->write_request_queue.first_element == 
		current_device_p->write_request_queue.last_element)
	{
		USBFAX_SEND_TRACE("USBFAX: usbfax_suppress_write_requests_fifo : write fifo is empty", RV_TRACE_LEVEL_DEBUG_LOW);

		usbfax_empty_queue_request(current_device_p->device_number, USBFAX_XFER_WRITE_REQUEST);
		waiting_write_requests = FALSE;
	}
	else
	{
		/* Check if write request to delete is first */
		if(current_device_p->write_request_queue.first_element 
			== current_device_p->write_request_queue.current_element)
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_suppress_write_requests_fifo : request to suppress is first", RV_TRACE_LEVEL_DEBUG_LOW);

			current_device_p->write_request_queue.first_element = 
				(current_device_p->write_request_queue.first_element + 1)  % USBFAX_MAX_REQUESTS;

			/* current_device_p->write_request_queue.current_element = 
				current_device_p->write_request_queue.first_element;
			*/
}
		/* Check if write request to delete is last */
		else if(current_device_p->write_request_queue.last_element 
			== current_device_p->write_request_queue.current_element)
		{
			USBFAX_SEND_TRACE("USBFAX: usbfax_suppress_write_requests_fifo : request to suppress is last", RV_TRACE_LEVEL_DEBUG_LOW);

			current_device_p->write_request_queue.last_element = 
				(current_device_p->write_request_queue.last_element - 1)  % USBFAX_MAX_REQUESTS;

			current_device_p->write_request_queue.current_element = 
				current_device_p->write_request_queue.last_element;
		}
		else
		{
			USBFAX_SEND_TRACE("USBFAX: USBFAX Shift all incoming write requests in the fifo", RV_TRACE_LEVEL_DEBUG_LOW);
				
			dest_idx_element =  current_device_p->write_request_queue.current_element;
			srce_idx_element = (dest_idx_element +1) % USBFAX_MAX_REQUESTS;

			/* Shift all incoming write requests */
			while(dest_idx_element != current_device_p->write_request_queue.last_element)
			{
				srce_write_request_p = &current_device_p->write_request_queue.tab_xfer[srce_idx_element];
				dest_write_request_p = &current_device_p->write_request_queue.tab_xfer[dest_idx_element];

				memcpy(dest_write_request_p, srce_write_request_p, sizeof(T_USBFAX_XFER));
				
				dest_idx_element =  (dest_idx_element +1) % USBFAX_MAX_REQUESTS;
				srce_idx_element =	(dest_idx_element +1) % USBFAX_MAX_REQUESTS;
			}

			/* Update last element of the fifo */
			current_device_p->write_request_queue.last_element = 
				(current_device_p->write_request_queue.last_element - 1)  % USBFAX_MAX_REQUESTS;
		}
	}

	return waiting_write_requests;
}
