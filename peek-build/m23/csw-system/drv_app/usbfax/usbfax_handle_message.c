/**
 * @file	usbfax_handle_message.c
 *
 * USBFAX handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author	
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	19/02/2004
 */

#include "usbfax/usbfax_i.h"
#include "usbfax/usbfax_env.h"
#include "rvf/rvf_api.h"

#include "usbfax/usbfax_task_i.h"

#include "usb/usb_api.h"
#include "usb/usb_message.h"
#include "usb/usb_interface.h"

extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;

 /****************************************************************************************
 |	FUNCTION PROTOTYPES																	|
 V**************************************************************************************V*/
T_RVM_RETURN	usbfax_process_message (T_RV_HDR *msg_p);
T_RVM_RETURN	usbfax_process_rx_or_tx_message_buffer (T_RV_HDR *msg_p, U8 current_interface_id,U8 current_endpoint_id);

void	usbfax_change_internal_state(T_USBFAX_INTERNAL_STATE state);

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN usbfax_handle_message (T_RV_HDR *msg_p)
{
	T_RVM_RETURN ret = RVM_OK;

	USBFAX_SEND_TRACE("USBFAX: usbfax_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);

	if (msg_p != NULL)
	{
		switch (usbfax_env_ctrl_blk_p->state)
		{
			case USBFAX_UNINITIALISED:
				/*we cannot process any message while we're USB_UNINITIALISED*/
		  		USBFAX_SEND_TRACE("USBFAX: Got an unexpected event in this (USB_UNINITIALISED) state - Ignored",
									RV_TRACE_LEVEL_WARNING);
				break;

			case USBFAX_INITIALIZED:
				/*we cannot process any message while we're USB_INITIALISING*/
		  		USBFAX_SEND_TRACE("USBFAX: Got an unexpected event in this (USB_INITIALISING) state - Ignored",
									RV_TRACE_LEVEL_WARNING);
				break;

			case USBFAX_IDLE:
				usbfax_change_internal_state(USBFAX_HANDLE_VENDOR_REQUEST);
				if(usbfax_process_message(msg_p) != RVM_OK)
				{
					USBFAX_SEND_TRACE("USBFAX: Unable to process message", RV_TRACE_LEVEL_ERROR);
					ret = RVM_MEMORY_ERR;
				}
				break;

			case USBFAX_WAIT_FOR_CDC_INTERFACES_SUSCRIPTION:
				/*we cannot process any message while state is USB_STOPPED*/
		  		USBFAX_SEND_TRACE("USBFAX: Got an unexpected event in this (USB_STOPPED) state - Ignored", 
									RV_TRACE_LEVEL_WARNING);
				break;

			case USBFAX_KILLED:
				/*we cannot process any message while state is USB_KILLED*/
		  		USBFAX_SEND_TRACE("USBFAX: Got an unexpected event in this (USB_KILLED) state - Ignored", 
									RV_TRACE_LEVEL_WARNING);
				break;

			default: 
				/* Unknow message has been received */
				USBFAX_SEND_TRACE("USBFAX: USB in an unknown state",
								RV_TRACE_LEVEL_ERROR);
				break; 
		}	
		
		/* Free message */
		if (rvf_free_buf(msg_p) != RVF_OK)
		{
			USBFAX_SEND_TRACE("USBFAX: Unable to free message",
							RV_TRACE_LEVEL_ERROR);
			ret = RVM_MEMORY_ERR;
		}

		/*set driver state back to idle*/
		usbfax_change_internal_state(USBFAX_IDLE);
	}
	return ret;
}

/****************************************************************************************
 |	INTERNAL	FUNCTIONS																|
 V**************************************************************************************V*/

T_RVM_RETURN usbfax_process_message (T_RV_HDR *msg_p)
{
	T_RVM_RETURN ret = RVM_OK;

	U8	current_interface_id	= 0;
	U8	current_endpoint_id		= 0;

	static U8	rx_counter = 0;

	/* endpoint type */
	T_USBFAX_ENDPOINT_TYPE	endpoint_type		= USBFAX_UNKNOWN;
	T_USBFAX_USB_DEVICE*		current_device_p	= NULL;

	switch(msg_p->msg_id)
	{
		case USB_BUS_CONNECTED_MSG :
			USBFAX_SEND_TRACE("USBFAX: USB_BUS_CONNECTED_MSG message received", RV_TRACE_LEVEL_ERROR);
			ret = usbfax_post_vendor_rx_buffers();
			return(ret);

		case USB_BUS_DISCONNECTED_MSG :
			USBFAX_SEND_TRACE("USBFAX: USB_BUS_DISCONNECTED_MSG message received", RV_TRACE_LEVEL_ERROR);
			usbfax_env_ctrl_blk_p->host_serial_driver_connected = FALSE;
			ret = usbfax_process_usb_disconnected();
			return(ret);

		case USB_BUS_SUSPEND_MSG :
			USBFAX_SEND_TRACE("USBFAX: USB_BUS_SUSPEND_MSG message received", RV_TRACE_LEVEL_ERROR);
			return(ret);

		case USB_BUS_RESUME_MSG :
			USBFAX_SEND_TRACE("USBFAX: USB_BUS_RESUME_MSG message received", RV_TRACE_LEVEL_ERROR);
			return(ret);

		case USB_TX_BUFFER_EMPTY_MSG :
			USBFAX_SEND_TRACE("USBFAX: USB_TX_BUFFER_EMPTY_MSG message received", RV_TRACE_LEVEL_ERROR);
			current_interface_id = ((T_USB_TX_BUFFER_EMPTY_MSG *)msg_p)->interface;
			current_endpoint_id	 = ((T_USB_TX_BUFFER_EMPTY_MSG *)msg_p)->endpoint;
			usbfax_process_rx_or_tx_message_buffer(msg_p, current_interface_id, current_endpoint_id);
			USBFAX_SEND_TRACE("USBFAX: End of processing USB_TX_BUFFER_EMPTY_MSG message", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case USB_RX_BUFFER_FULL_MSG :
			rx_counter++;
			USBFAX_TRACE_WARNING_PARAM("USBFAX: USB_RX_BUFFER_FULL_MSG message received : ", rx_counter);
			current_interface_id = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->interface;
			current_endpoint_id	 = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->endpoint;
			usbfax_process_rx_or_tx_message_buffer(msg_p, current_interface_id, current_endpoint_id);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: End of processing USB_RX_BUFFER_FULL_MSG message : ", rx_counter);
		break;

		default:
			/* unknown message */
			/* wait for a ne message */
			USBFAX_SEND_TRACE("USBFAX: UNKNOWN MESSAGE", RV_TRACE_LEVEL_ERROR);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: message received: ", msg_p->msg_id);
			return(RVM_INTERNAL_ERR);
	}

	return ret;
}

T_RVM_RETURN usbfax_process_rx_or_tx_message_buffer (T_RV_HDR *msg_p, U8 current_interface_id,U8 current_endpoint_id)
{
	T_RVM_RETURN ret = RVM_OK;

	/* endpoint type */
	T_USBFAX_ENDPOINT_TYPE	endpoint_type		= USBFAX_UNKNOWN;
	T_USBFAX_USB_DEVICE*		current_device_p	= NULL;

	/* Get type of endpoint. Value may be	
	 *		USBFAX_CONTROL_CONTEXT or
	 *		USBFAX_INT_CONTEXT or
	 *		USBFAX_TX_CONTEXT or
	 *		USBFAX_RX_CONTEXT
	 */
	endpoint_type = usbfax_look_for_endpoint_type(	current_interface_id, 
													current_endpoint_id);

	/* Look for device associated to this endpoint */
	current_device_p = usbfax_look_for_associated_device(	current_interface_id, 
														current_endpoint_id);
	/* following endpoint type */
	switch(endpoint_type)
	{
		case USBFAX_CONTROL_CONTEXT : 
			/* Receiption of a vendor request */
			USBFAX_SEND_TRACE("USBFAX: Receiving vendor request",	RV_TRACE_LEVEL_WARNING);
			// usbfax_change_internal_state(USBFAX_PROCESS_VENDOR_REQUEST);
			ret = usbfax_process_vendor_request(current_device_p, msg_p);
		break;

		case  USBFAX_INT_CONTEXT : 
			/* Write notification of the serial state */
			USBFAX_SEND_TRACE("USBFAX: Serial state sent",	RV_TRACE_LEVEL_WARNING);
			ret = usbfax_process_control_request(current_device_p, msg_p);
		break;

		case USBFAX_TX_CONTEXT : 
			usbfax_lock();
			/* Write notification of data on IN bulk endpoint */
			USBFAX_SEND_TRACE("USBFAX: Sending data on IN Bulk endpoint", RV_TRACE_LEVEL_WARNING);
			// usbfax_change_internal_state(USBFAX_SEND_USB_DATA);
			ret = usbfax_process_tx_context(current_device_p, msg_p);
			usbfax_unlock();
		break;

		case USBFAX_RX_CONTEXT :
			usbfax_lock();
			/* Receiption of data on OUT bulk endpoint */
			// USBFAX_SEND_TRACE("USBFAX: Receiving data on OUT Bulk endpoint", RV_TRACE_LEVEL_WARNING);
			// usbfax_change_internal_state(USBFAX_GET_USB_DATA);
			ret = usbfax_process_rx_context(current_device_p, msg_p);
			usbfax_unlock();
		break;

		default:
			USBFAX_SEND_TRACE("USBFAX: Invalid endpoint ", RV_TRACE_LEVEL_ERROR);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: invalid endpoint : ", current_endpoint_id);
			USBFAX_TRACE_WARNING_PARAM("USBFAX: invalid interface : ", current_interface_id);
		break;
	}

	return ret;
}

