/**
 * @file	usbtrc_handle_message.c
 *
 * USBTRC handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author	Pierre-Olivier POUX
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	19/02/2004	Pierre-Olivier POUX (po.poux@philog.com)		
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#include "usbtrc/usbtrc_i.h"
#include "usbtrc/usbtrc_env.h"

#include "usb/usb_api.h"
#include "usb/usb_message.h"



/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN usbtrc_handle_message (T_RV_HDR *msg_p)
{
	T_RVM_RETURN ret = RVM_OK;

	USBTRC_SEND_TRACE("USBTRC: usbtrc_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);

	if (msg_p != NULL)
	{
		switch (usbtrc_env_ctrl_blk_p->state)
		{
			case USBTRC_INIT_STATE:
				usbtrc_init_state(msg_p);
				break;

			case USBTRC_DISCONNECTED_STATE:
				usbtrc_disconnected_state(msg_p);
				break;

			case USBTRC_IDLE_STATE:
				usbtrc_idle_state(msg_p);
				break;

			default: 
				break; 
		}	
		
		/* Free message */
		if (rvf_free_buf(msg_p) != RVF_OK)
		{
			USBTRC_SEND_TRACE("USBTRC: Unable to free message",RV_TRACE_LEVEL_ERROR);
			return RVM_MEMORY_ERR;
		}
	}

	return ret;
}

/**
 * Performs INIT state: wait for USB suscribe notify
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK
 */
T_RVM_RETURN usbtrc_init_state (T_RV_HDR *msg_p)
{
	T_USB_FM_RESULT_MSG				*usb_fm_subscribe_msg	= NULL;

	USBTRC_SEND_TRACE("USBTRC: enter usbtrc_init_state", RV_TRACE_LEVEL_DEBUG_LOW);

	switch (msg_p->msg_id)
	{
		case USB_FM_RESULT_MSG:
			usb_fm_subscribe_msg = (T_USB_FM_RESULT_MSG	*)msg_p;
			if(usb_fm_subscribe_msg->result == succes)
			{
				usbtrc_change_internal_state(USBTRC_DISCONNECTED_STATE);
			}
			break;

		default:
			break;
	}

	return RVM_OK;
}

/**
 * Performs DISCONNECTED state: wait for USB connected to host
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK
 */
T_RVM_RETURN usbtrc_disconnected_state (T_RV_HDR *msg_p)
{
	T_RVM_RETURN result = RVM_OK;
	
	USBTRC_SEND_TRACE("USBTRC: enter usbtrc_disconnected_state", RV_TRACE_LEVEL_DEBUG_LOW);

	switch(msg_p->msg_id)
	{
	case USB_BUS_CONNECTED_MSG:
		USBTRC_SEND_TRACE("USBTRC: received USB_BUS_CONNECTED_MSG", RV_TRACE_LEVEL_DEBUG_LOW);
		
		usbtrc_set_connected_flag(TRUE);
		/*change our state : we are now connected*/
		usbtrc_change_internal_state(USBTRC_IDLE_STATE);
		
		/*register for Class request*/
		result = usbtrc_set_rx_buffer_control();
		
		if(result != RVM_OK)
		{
			USBTRC_SEND_TRACE("USBTRC: usbtrc_disconnected_state error in set_rx_buffer_control", RV_TRACE_LEVEL_DEBUG_LOW);
			return  RVM_INTERNAL_ERR;
		}
		usbtrc_set_rx_buffer_bulk();
		break;
		
	default:
		USBTRC_TRACE_WARNING_PARAM("USBTRC: received message", msg_p->msg_id);
		break;
	}

	return RVM_OK;
}

/**
 * Performs IDLE state: wait for USB LLD messages:
 * - USB_TX_BUFFER_EMPTY_MSG
 * - USB_RX_BUFFER_FULL_MSG
 * - USB_BUS_DISCONNECTED_MSG
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK
 *			RVM_INTERNAL_ERROR
 */

T_RVM_RETURN usbtrc_idle_state (T_RV_HDR *msg_p)
{
	T_RVM_RETURN status = RVM_OK;

	UINT8 interface_id	= 0;
	UINT8 endpoint_id	= 0;

	/* endpoint type */
	T_USBTRC_ENDPOINT_TYPE endpoint_type = USBTRC_UNKNOWN;

	USBTRC_SEND_TRACE("USBTRC: enter usbtrc_idle_state", RV_TRACE_LEVEL_DEBUG_LOW);

	switch(msg_p->msg_id)
	{
		case USB_BUS_DISCONNECTED_MSG :
			USBTRC_SEND_TRACE("USBTRC: USB_BUS_DISCONNECTED_MSG message received", RV_TRACE_LEVEL_ERROR);
			status = usbtrc_process_usb_disconnected();
			usbtrc_change_internal_state(USBTRC_DISCONNECTED_STATE);
			break;

		case USB_TX_BUFFER_EMPTY_MSG :
			//USBTRC_SEND_TRACE("USBTRC: USB_TX_BUFFER_EMPTY_MSG message received", RV_TRACE_LEVEL_ERROR);
			interface_id = ((T_USB_TX_BUFFER_EMPTY_MSG *)msg_p)->interface;
			endpoint_id	 = ((T_USB_TX_BUFFER_EMPTY_MSG *)msg_p)->endpoint;

			USBTRC_TRACE_WARNING_PARAM("USBTRC: received message USB_TX_BUFFER_EMPTY_MSG fo ep", endpoint_id);
			process_rx_or_tx_message_buffer(msg_p, interface_id, endpoint_id);
			break;

		case USB_RX_BUFFER_FULL_MSG :
			USBTRC_SEND_TRACE("USBTRC: USB_RX_BUFFER_FULL_MSG message received", RV_TRACE_LEVEL_ERROR);
			interface_id = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->interface;
			endpoint_id	 = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->endpoint;

			USBTRC_TRACE_WARNING_PARAM("USBTRC: received message USB_RX_BUFFER_FULL_MSG fo ep", endpoint_id);

			process_rx_or_tx_message_buffer(msg_p, interface_id, endpoint_id);
			break;

		default:
			break;
	}

	return status;
}

/**
 * Gathers all direction data transfer, in all endpoints
 *
 * @param	msg_p		:	Pointer on the header of the message.
 *			interface_id:	interface id
 *			endpoint_id	:	endpoint id
 * @return	RVM_OK
 *			RVM_INTERNAL_ERROR
 */

T_RVM_RETURN process_rx_or_tx_message_buffer (T_RV_HDR *msg_p, UINT8 interface_id, UINT8 endpoint_id)
{
	T_RVM_RETURN ret = RVM_OK;

	/* endpoint type */
	T_USBTRC_ENDPOINT_TYPE	endpoint_type	= USBTRC_UNKNOWN;

	/* Get type of endpoint. Value may be	
	 *		USBTRC_CONTROL_CONTEXT or
	 *		USBTRC_INT_CONTEXT or
	 *		USBTRC_TX_CONTEXT or
	 *		USBTRC_RX_CONTEXT
	 */
	endpoint_type = usbtrc_look_for_endpoint_type(interface_id, endpoint_id);

	USBTRC_SEND_TRACE("USBTRC: enter process_rx_or_tx_message_buffer", RV_TRACE_LEVEL_DEBUG_LOW);

	/* following endpoint type */
	switch(endpoint_type)
	{
		case USBTRC_CONTROL_CONTEXT : 
			/* Receiption of a vendor request */
			USBTRC_SEND_TRACE("USBTRC: Receiving vendor request",	RV_TRACE_LEVEL_WARNING);
			ret = usbtrc_process_vendor_request(msg_p);
		break;

		case USBTRC_INT_CONTEXT : 
			/* Write notification of the serial state */
			USBTRC_SEND_TRACE("USBTRC: Serial state sent", RV_TRACE_LEVEL_WARNING);
			ret = usbtrc_process_control_request(msg_p);
		break;

		case USBTRC_TX_CONTEXT : 
			/* Write notification of data on IN bulk endpoint */
 			/* USB Tracing is now synchronous. This is not used.*/
//			ret = usbtrc_process_tx_context();
		break;

		case USBTRC_RX_CONTEXT :
			/* Receiption of data on OUT bulk endpoint */
			USBTRC_SEND_TRACE("USBTRC: Receiving data on OUT Bulk endpoint", RV_TRACE_LEVEL_WARNING);
			ret = usbtrc_process_rx_context(msg_p);
		break;

		default:
			USBTRC_SEND_TRACE("USBTRC: Invalid endpoint ", RV_TRACE_LEVEL_ERROR);
			USBTRC_TRACE_WARNING_PARAM("USBTRC: invalid endpoint : ", endpoint_id);
			USBTRC_TRACE_WARNING_PARAM("USBTRC: invalid interface : ", interface_id);
		break;
	}

	return ret;
}

