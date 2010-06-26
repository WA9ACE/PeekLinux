#include "usbtrc/usbtrc_i.h"
#include "usbtrc/usbtrc_env.h"
#include "usbtrc/usbtrc_queue.h"

#include "usb/usb_api.h"
#include "usb/usb_message.h"
#include "usb/usb_interface.h"

#include <string.h>


/**
 * Local and globale variables
 */

/**
 * Pointer on the structure gathering all the global variables
 * used by USBTRC instance.
 */
T_USBTRC_ENV_CTRL_BLK *usbtrc_env_ctrl_blk_p;

/**
 * Pointer on the structure describing USB device
 */
T_USBTRC_USB_DEVICE *usbd;

/**
 * @name endpoint_context_cnfg1
 *
 *	CDC endpoints configuration
 */
/*@{*/
const T_USBTRC_ENDPOINT_CONTEXT_CNFG	endpoint_context_cnfg =
{
	{
		USBTRC_CDC1_IF_CONTROL,
		USBTRC_CDC1_EP_CONTROL,
		USBTRC_ENDPOINT_INTERRUPT_SIZE
	},
	{
		USBTRC_CDC1_IF_INT,
		USBTRC_CDC1_EP_INT,
		USBTRC_ENDPOINT_BULK_SIZE
	},
	{
		USBTRC_CDC1_IF_RX_CONTEXT,
		USBTRC_CDC1_EP_RX_CONTEXT,
		USBTRC_ENDPOINT_BULK_SIZE
	},
	{
		USBTRC_CDC1_IF_TX_CONTEXT,
		USBTRC_CDC1_EP_TX_CONTEXT,
		USBTRC_ENDPOINT_BULK_SIZE
	}
};

/**
 * Pattern of serial state notification as defined in
 * USB Class Definitions for Communication devices document p 75
 */
static UINT8 serial_state_pattern[USBTRC_SERIAL_REQUEST_SIZE] = 
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


/**
 * Functions
 */

/**
 * @name usbtrc_report_dsr_state
 *
 * return the state of DSR: TRUE if hight
 *
 * @param void
 */
/*@{*/
BOOLEAN usbtrc_report_dsr_state (void)
{
	T_USBTRC_USB_DEVICE *usb_p;

	if(usbtrc_env_ctrl_blk_p == NULL)
	{
		return FALSE;
	}

	if(usbtrc_env_ctrl_blk_p->usb_device == NULL)
	{
		return FALSE;
	}

	usb_p = usbtrc_env_ctrl_blk_p->usb_device;

	if((usb_p->usb_serial_state & USBTRC_SERIAL_STATE_TX_CARRIER)
		== USBTRC_SERIAL_STATE_TX_CARRIER)
	{
		return TRUE;
	}

	return FALSE;
}
/*@}*/

/**
 * @name usbtrc_ready
 *
 * return TRUE if USBTRCR is ready for communicate
 *
 * @param T_USBTRC_INTERNAL_STATE :	new state
 */
/*@{*/
BOOLEAN usbtrc_ready (void)
{
	if(usbtrc_report_dsr_state() == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}
/*@}*/

/**
 * @name usbtrc_change_internal_state
 *
 * this function is called to change the USBTRC driver state
 *
 * @param T_USBTRC_INTERNAL_STATE :	new state
 */
/*@{*/
void usbtrc_change_internal_state(T_USBTRC_INTERNAL_STATE state)
{
	usbtrc_env_ctrl_blk_p->state = state;
}
/*@}*/

/**
 * @name usbtrc_report_dsr_hight
 *
 * tell the USB host that DSR is hight
 *
 * @param void
 */
/*@{*/
void usbtrc_report_dsr_hight (void)
{
	/* set DSR to 1 */
	usbtrc_env_ctrl_blk_p->usb_device->usb_serial_state |= USBTRC_SERIAL_STATE_TX_CARRIER;

	/* send serial state of the driver to the host */
	usbtrc_process_control_lines();
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_report_dsr_low
 *
 * tell the USB host that DSR is low
 *
 * @param void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_report_dsr_low (void)
{
	/* set DSR to 1 */
	usbtrc_env_ctrl_blk_p->usb_device->usb_serial_state &= ~USBTRC_SERIAL_STATE_TX_CARRIER;

	/* send serial state of the driver to the host */
	usbtrc_process_control_lines();
}
/*@}*/


/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_get_mem
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
T_RVM_RETURN usbtrc_get_mem (char** buffer_pp, UINT16 size)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS	mb_status;
	T_RV_RET		ret = RVM_OK;

	/* Create buffer to contain the endpoint 0 setup request additional data*/
	mb_status = rvf_get_buf(usbtrc_env_ctrl_blk_p->prim_mb_id,
							(sizeof(char) * size),
							((T_RVF_BUFFER**) buffer_pp));

	if (mb_status == RVF_RED) 
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 * The environemnt will cancel the USB instance creation.
		 */
		USBTRC_SEND_TRACE("USBTRC: Error to get memory ",RV_TRACE_LEVEL_ERROR);
		ret = RVM_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW) 
	{
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USBTRC_SEND_TRACE("USBTRC: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_control_lines
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
T_RVM_RETURN usbtrc_process_control_lines (void)
{	
	T_RV_RET ret = RVM_OK;
	UINT8	*serial_state_data = NULL;

	/* allocate buffer for the serial state to send */
	usbtrc_get_mem((char**)&serial_state_data, USBTRC_SERIAL_STATE_SIZE);

	/* copy the serial state pattern into the buffer to send */
	memcpy(serial_state_data, serial_state_pattern, USBTRC_SERIAL_REQUEST_SIZE);

	/* Fill low part of the wIndex field with interface number */
	serial_state_data[4] = usbd->usb_ep_cnfg->ep_int_context.interface_id;	
	
	/* Fill data part with the serial state */
	serial_state_data[8] = usbd->usb_serial_state & 0xFF;	/* low  part of serial state data */
	serial_state_data[9] = usbd->usb_serial_state >> 8;		/* high part of serial state data */

	/* Send serial state to the host */
	if(usb_set_tx_buffer(	usbd->usb_ep_cnfg->ep_int_context.interface_id,
							usbd->usb_ep_cnfg->ep_int_context.endpoint_id,
							serial_state_data, USBTRC_SERIAL_STATE_SIZE, FALSE)	 != RVM_OK)
	{
		ret = RVM_INTERNAL_ERR;
	}

	/* store address of the buffer sent */
	usbd->serial_state_buffer = serial_state_data;

	return(ret);
}
/*@}*/

/**
 * @name usbtrc_create_usb_device
 *
 * This function reads configuration from the flash
 * and initializes read and write queues.
 * This function is called at start
 *
 * @return	RVM_OK
 */
/*@{*/
T_RVM_RETURN usbtrc_create_usb_device (void)
{
	/* init USB structure fields to 0 */
	usbtrc_get_mem((char**)&usbtrc_env_ctrl_blk_p->usb_device, sizeof(T_USBTRC_USB_DEVICE));
	usbd = usbtrc_env_ctrl_blk_p->usb_device;
	memset(usbd, 0, sizeof(T_USBTRC_USB_DEVICE));

	usbd->connected = FALSE;
	usbd->read_suspended = TRUE;
	usbd->write_in_progress = FALSE;
	usbd->serial_state_buffer = NULL;

		
	/* get endpoints configuration */
	usbd->usb_ep_cnfg = &endpoint_context_cnfg;

	/* allocate read/wrtie queues */
	usbtrc_q_rx_create();
	usbtrc_q_tx_create();	

	/* initialize read/write queues */
	usbtrc_q_rx_init();
	usbtrc_q_tx_init();

	return RVM_OK;
}
/*@}*/

/**
 * @name usbtrc_destroy_usb_device
 *
 * This function release USB device ressources
 *
 * @return	RVM_OK
 */
/*@{*/
T_RVM_RETURN usbtrc_destroy_usb_device (void)
{
	/* free read/write queues */
	usbtrc_q_rx_destroy();
	usbtrc_q_tx_destroy();

	rvf_free_buf(usbtrc_env_ctrl_blk_p->usb_device);



	return RVM_OK;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtracer_wait_for_message.
 *
 * This function can wait to a specified message 
 *
 * @param	msg_id	Waited message ID, null for any message.
 *
 * @return	The received message.
 *---------------------------------------------------------------------------------* 
 */
T_RV_HDR *usbtracer_wait_for_message (UINT32 msg_id)
{
	T_RV_HDR * msg_p;
	
	USBTRC_SEND_TRACE("USBTRC: enter usbtracer_wait_for_message", RV_TRACE_LEVEL_DEBUG_LOW);

	for (;;)
	{
		/* Waiting for an event, blocking USBTRC task. */
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
 * @name usbtrc_suscribe_to_interfaces
 *
 * This function suscribes to the USB interface
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN usbtrc_suscribe_to_interfaces (UINT8 interface_number)
{
	T_RVM_RETURN	suscribe_result = RVM_OK;
	
	/* storage for awaited messages
	*/ 
	T_RV_HDR  *msg_p;
	T_RV_HDR  *msg_p_ready;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: suscribe to interface", interface_number);

	/* Subscribe to interface number given in parameter */
	if (usb_fm_subscribe(interface_number, usbtrc_env_ctrl_blk_p->usbtrc_return_path) != RVM_OK)
	{
		suscribe_result = RV_INTERNAL_ERR;
	}
	else
	{
		/* Wait for a subscription result from the USB SWE */
		msg_p = usbtracer_wait_for_message(USB_FM_RESULT_MSG);
		if(((T_USB_FM_RESULT_MSG*)msg_p)->result != succes)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		rvf_free_buf(msg_p);
	}

	/* Send Ready to USB using interface number given in parameter */
	if (usb_fm_ready(interface_number, usbtrc_env_ctrl_blk_p->usbtrc_return_path) != RVM_OK)
	{
		suscribe_result = RV_INTERNAL_ERR;
	}
	else
	{
		/* Wait for a subscription result from the USB SWE */
		msg_p_ready = usbtracer_wait_for_message(USB_FM_RESULT_MSG);
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
 * @name usbtrc_suscribe_to_cdc_interfaces
 *
 * This function suscribes to all interfaces of the USB configuration
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN usbtrc_suscribe_to_cdc_interfaces (void)
{	
	T_RVM_RETURN	suscribe_result = RVM_OK;
	UINT32				device_index	= 0;

	UINT8					current_interface;

	/* Boolean array indicating if the interface has been suscribed */
	BOOLEAN	interface_table[USBTRC_INTERFACE_MAX];

	/* Initialize array */
	memset(interface_table, 0x00, USBTRC_INTERFACE_MAX * sizeof(UINT8));

	/* Get interface of the control endpoint */
	current_interface = usbd->usb_ep_cnfg->ep_control_context.interface_id;
	if(interface_table[current_interface] == FALSE)
	{
		if(usbtrc_suscribe_to_interfaces(current_interface) != RVM_OK)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		interface_table[current_interface] = TRUE;
	}

	// usbtrc_prepare_vendor_rx_buffer_request(current_device_p);

	/* Get interface of the interrupt endpoint */
	current_interface = usbd->usb_ep_cnfg->ep_int_context.interface_id;
	if(interface_table[current_interface] == FALSE)
	{
		if(usbtrc_suscribe_to_interfaces(current_interface) != RVM_OK)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		interface_table[current_interface] = TRUE;
	}

	/* Get interface number of the IN Bulk endpoint */
	current_interface = usbd->usb_ep_cnfg->ep_tx_context.interface_id;
	if(interface_table[current_interface] == FALSE)
	{
		if(usbtrc_suscribe_to_interfaces(current_interface) != RVM_OK)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		interface_table[current_interface] = TRUE;
	}

	/* Get interface number of the OUT Bulk endpoint */
	current_interface = usbd->usb_ep_cnfg->ep_rx_context.interface_id;
	if(interface_table[current_interface] == FALSE)
	{
		if(usbtrc_suscribe_to_interfaces(current_interface) != RVM_OK)
		{
			suscribe_result = RV_INTERNAL_ERR;
		}
		interface_table[current_interface] = TRUE;
	}

	return(suscribe_result);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_set_rx_buffer_control
 *
 * This function give a buffer to the USB SWE to receive vendor requests.
 * the buffer used is "vendor_request_buffer" of the T_USBTRC_USB_DEVICE structure.
 *
 * @param current_device_p:	reference to a device
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN usbtrc_set_rx_buffer_control (void)
{
	USBTRC_SEND_TRACE("USBTRC: usbtrc_set_rx_buffer_control", RV_TRACE_LEVEL_ERROR);

	if(usb_set_rx_buffer(	usbd->usb_ep_cnfg->ep_control_context.interface_id,
							usbd->usb_ep_cnfg->ep_control_context.endpoint_id,
							usbd->vendor_request_buffer, LENGTH_VENDOR_REQUEST_PACKET) != RVM_OK)
	{
		return RVM_INTERNAL_ERR;
	}

	return RVM_OK;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_rx_suspended
 *
 * return TRUE if bulk receiving has been suspended
  *
 * @param void
 *
 * @return	TRUE				
 *			FALSE
 *---------------------------------------------------------------------------------* 
 */
BOOLEAN usbtrc_rx_suspended (void)
{
	return usbd->read_suspended;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_write_in_progress
 *
 * return TRUE if a device to host transfer is pending
  *
 * @param void
 *
 * @return	TRUE				
 *			FALSE
 *---------------------------------------------------------------------------------* 
 */
BOOLEAN usbtrc_write_in_progress (void)
{
	return usbd->write_in_progress;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_set_rx_buffer_bulk
 *
 * If there is enought space in receive queue, this function give a buffer to the
 * USB LLD to receive bulks. 
 * If there is not enought space, receiving is desactivated.
 *
 * @param
 *
 * @return	RVM_OK				
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
T_RVM_RETURN usbtrc_set_rx_buffer_bulk(void)
{
		
	if(FALSE == usbtrc_q_rx_try(usbd->usb_ep_cnfg->ep_rx_context.MaxPacketSize)) 
	{
		/* not enought space left in queue */
		/* stop incoming data by stopping providing
						buffer to USB LLD */
		usbd->read_suspended = TRUE;	
	}
	else
	{
		usbd->read_suspended = FALSE;
		
		if(usb_set_rx_buffer(	usbd->usb_ep_cnfg->ep_rx_context.interface_id,
								usbd->usb_ep_cnfg->ep_rx_context.endpoint_id,
								(UINT8*)usbd->buffer_rx_temp, 
								usbd->usb_ep_cnfg->ep_rx_context.MaxPacketSize) != RVM_OK)
		{
			return RVM_INTERNAL_ERR;
		}
	}

	return RVM_OK;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_usb_disconnected
 *
 * This function is called in case of usb disconnect cable
 *
 * @param	no param
 *
 * @return	T_RVM_RETURN
 *---------------------------------------------------------------------------------* 
 */
 extern T_RVF_MUTEX usb_tx_mutex;
T_RVM_RETURN usbtrc_process_usb_disconnected (void)
{	
	T_RV_RET ret = RVM_OK;
       rvf_unlock_mutex (&usb_tx_mutex);
	/* check if DTR of the device is raised */
	if(usbd->usb_line_state.host_dtr == 1)
	{
		/* cancel RX vendor request buffer */
		ret	= usb_reclaim_rx_buffer (
			usbd->usb_ep_cnfg->ep_control_context.interface_id,
			usbd->usb_ep_cnfg->ep_control_context.endpoint_id );

		if(ret != RVM_OK)
		{
			/*USB LDD may return incorrect values if reclaim is tried after unplug*/
			/*no seriousness*/
			USBTRC_SEND_TRACE("USBTRC: Failed to reclaim RX vendor request buffer",
				RV_TRACE_LEVEL_DEBUG_LOW);
		}

		/* cancel RX data buffer */
		ret	= usb_reclaim_rx_buffer (
			usbd->usb_ep_cnfg->ep_rx_context.interface_id,
			usbd->usb_ep_cnfg->ep_rx_context.endpoint_id );

		if(ret != RVM_OK)
		{
			USBTRC_SEND_TRACE("USBTRC: Failed to reclaim RX data buffer",
				RV_TRACE_LEVEL_DEBUG_LOW);
		}
		
		/* set connected flag as disconnected */
		usbtrc_set_connected_flag(FALSE);
		
		/* empty read and write queue */
		usbtrc_q_tx_init();
		usbtrc_q_rx_init();
	}
	
	return RVM_OK;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_set_connected_flag
 *
 * This function tell if the USB device is connected or not to the USB host.
 *
 * @param	connected: TRUE if connected
 *
 * @return	nothing
 *---------------------------------------------------------------------------------* 
 */
void usbtrc_set_connected_flag (BOOLEAN connected)
{
	usbd->connected = connected;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_set_connected_flag
 *
 * This function tell if the USB device is connected or not to the USB host.
 *
 * @param	nothing
 *
 * @return:	TRUE if connected
 *---------------------------------------------------------------------------------* 
 */
BOOLEAN usbtrc_get_connected_flag (void)
{
	return usbd->connected;
}

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_vendor_request
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
T_RVM_RETURN usbtrc_process_vendor_request (T_RV_HDR* msg_p)
{	
	T_RVM_RETURN	ret = RVM_OK;
	UINT8			current_vendor_request;

	if(msg_p->msg_id == USB_RX_BUFFER_FULL_MSG)
	{
		/* Get request field of the SETUP data */
		current_vendor_request = usbd->vendor_request_buffer[1];
			
		switch(current_vendor_request)
		{
			case SET_LINE_CODING_REQUEST :
				/* current request is SET_LINE_CODING */
				USBTRC_SEND_TRACE("USBTRC: Vendor request is Set_Line_Coding", RV_TRACE_LEVEL_WARNING);
				ret = usbtrc_process_set_line_coding_request(msg_p);

				/* Vendor request is processed */
				usbtrc_set_rx_buffer_control(); /* current task can receive next vendor request */
				break;

			case GET_LINE_CODING_REQUEST :
				/* current request is GET_LINE_CODING */
				USBTRC_SEND_TRACE("USBTRC: Vendor request is Get_Line_Coding", RV_TRACE_LEVEL_WARNING);
				ret = usbtrc_process_get_line_coding_request(msg_p);
				break;

			case SET_CONTROL_LINE_STATE_REQUEST :
				/* current request is SET_CONTROL_LINE_STATE */
				USBTRC_SEND_TRACE("USBTRC: Vendor request is Set_Control_Line_State", RV_TRACE_LEVEL_WARNING);
				ret = usbtrc_process_set_line_state_request(msg_p);

				/* Vendor request is processed */
				usbtrc_set_rx_buffer_control(); /* current task can receive next vendor request */
				break;

			default :
				/* Unknwon vendor request */
				USBTRC_SEND_TRACE("USBTRC: Unknown vendor request", RV_TRACE_LEVEL_WARNING);

				/* Vendor request is processed */				
				usbtrc_set_rx_buffer_control(); /* current task can receive next vendor request */
				ret = RVM_INTERNAL_ERR;
				break;
		}
	}
	else if(msg_p->msg_id == USB_TX_BUFFER_EMPTY_MSG)
	{
		USBTRC_SEND_TRACE("USBTRC: usbtrc_process_vendor_request (USB_TX_BUFFER_EMPTY_MSG)", RV_TRACE_LEVEL_WARNING);

		/* Response of a vendor request has been made */
		/* Buffer can be free */
		if(usbd->vendor_response_buffer != NULL)
		{
			USBTRC_SEND_TRACE("USBTRC: usbtrc_process_vendor_request (vendor_response_buffer != NULL)", RV_TRACE_LEVEL_WARNING);

			rvf_free_buf(usbd->vendor_response_buffer);
			usbd->vendor_response_buffer = NULL;

			/* Vendor request is processed */
			usbtrc_set_rx_buffer_control(); /* current task can receive next vendor request */
		}
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_set_line_coding_request
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
T_RVM_RETURN usbtrc_process_set_line_coding_request (T_RV_HDR* msg_p)
{
	T_RVM_RETURN	ret = RVM_OK;
	UINT8*			line_coding_data_p = NULL;

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_SET_LINE_CODING_REQUEST)
	{
		/* Point to data of the SET_LINE_CODING request */
		line_coding_data_p = usbd->vendor_request_buffer + LENGTH_VENDOR_REQUEST_COMMAND;

		/* Update line coding information of the current device */
		
		/* Data terminal rate, in bits per second */
		usbd->usb_line_coding.data_rate	 = line_coding_data_p[0] << 24;
		usbd->usb_line_coding.data_rate	|= line_coding_data_p[1] << 16;
		usbd->usb_line_coding.data_rate	|= line_coding_data_p[2] <<  8;
		usbd->usb_line_coding.data_rate	|= line_coding_data_p[3];

		/* Stop bits */
		usbd->usb_line_coding.char_format = line_coding_data_p[4];
		
		/* Parity */
		usbd->usb_line_coding.parity_type = line_coding_data_p[5];
		
		/* Data bits */
		usbd->usb_line_coding.data_bits	= line_coding_data_p[6];
	}
	else
	{
		USBTRC_SEND_TRACE("USBTRC: Invalid size of SetLineCoding request ", RV_TRACE_LEVEL_ERROR);
		/* Invalid size of SET_LINE_CODING request */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_set_line_state_request
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
T_RVM_RETURN usbtrc_process_set_line_state_request (T_RV_HDR* msg_p)
{
	T_RVM_RETURN	ret = RVM_OK;
	UINT8			line_state = 0;

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_VENDOR_REQUEST_COMMAND)
	{
		/* get line state of the SET_CONTROL_LINE_STATE request */
		line_state = usbd->vendor_request_buffer[2];
		
		/* Update line state of the device */
		usbd->usb_line_state.host_dtr = line_state & 1;	/* bit 0 : activate DTR */
		usbd->usb_line_state.host_rts = line_state & 2;	/* bit 1 : activate RTS */
	}
	else
	{
		USBTRC_SEND_TRACE("USBTRC: Invalid size of SetLineState request ",RV_TRACE_LEVEL_ERROR);
		/* Invalid SET_CONTROL_LINE_STATE command */
		ret = RVM_INTERNAL_ERR;
	}

	if(usbd->usb_line_state.host_dtr != 0)
	{
		usbtrc_report_dsr_hight();
	}
	else
	{
		usbtrc_report_dsr_low();
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_get_line_coding_request
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
T_RVM_RETURN usbtrc_process_get_line_coding_request (T_RV_HDR* msg_p)
{	
	T_RV_RET	ret = RVM_OK;
	UINT16		size_to_return = 0;
	
	UINT8	buffer[LENGTH_VENDOR_REQUEST_DATA];
	UINT8	*line_coding_info = NULL;

	if( ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size == LENGTH_VENDOR_REQUEST_COMMAND)
	{
			/* Update buffer to send with line coding structure of the device */
			/* Data terminal rate, in bits per second */
			buffer[0] = (usbd->usb_line_coding.data_rate >> 24) & 0xFF;
			buffer[1] = (usbd->usb_line_coding.data_rate >> 16) & 0xFF;
			buffer[2] = (usbd->usb_line_coding.data_rate >>  8) & 0xFF;
			buffer[3] = (usbd->usb_line_coding.data_rate      ) & 0xFF;

			/* Stop bits */
			buffer[4] = usbd->usb_line_coding.char_format;
			buffer[5] = usbd->usb_line_coding.parity_type;
			buffer[6] = usbd->usb_line_coding.data_bits;

			/* Size of */
			size_to_return  = usbd->vendor_request_buffer[6];
			size_to_return |= usbd->vendor_request_buffer[7] << 8;

			/* allocate a buffer of the requested size to send line coding information */
			usbtrc_get_mem((char**)&line_coding_info, size_to_return);

			memcpy(line_coding_info, buffer, size_to_return);

			USBTRC_TRACE_WARNING_PARAM("USBTRC: Size of response of GetLineCoding : ",
				size_to_return);
			USBTRC_TRACE_WARNING_PARAM("USBTRC: Post TX vendor request interface: ",
				usbd->usb_ep_cnfg->ep_control_context.interface_id);
			USBTRC_TRACE_WARNING_PARAM("USBTRC: Post TX vendor request endpoint : ",
				usbd->usb_ep_cnfg->ep_control_context.endpoint_id);

			/* Send line coding information to the host */
			if(usb_set_tx_buffer(
				usbd->usb_ep_cnfg->ep_control_context.interface_id,
				usbd->usb_ep_cnfg->ep_control_context.endpoint_id,
				line_coding_info, size_to_return, TRUE)	 != RVM_OK)
			{
				USBTRC_SEND_TRACE("USBTRC: Failed to respond to GetLineCoding", RV_TRACE_LEVEL_ERROR);
				ret = RVM_INTERNAL_ERR;
			}

			/* store address of the buffer sent */
			usbd->vendor_response_buffer = line_coding_info;
	}
	else
	{
		USBTRC_SEND_TRACE("USBTRC: Invalid size of GetLineCoding request ",RV_TRACE_LEVEL_ERROR);
		/* Invalid GET_LINE_CODING command */
		ret = RVM_INTERNAL_ERR;
	}

	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_look_for_endpoint_type
 *
 * From the interface identifier and an endpoint identifier,
 * this function search type of endpoint.
 *
 * @param	interface_id	:	interface identifier
 *			endpoint_id		:	endpoint identifier
 *
 * @return	T_USBTRC_ENDPOINT_TYPE	:	endpoint type (control, interrupt, bulk IN, bulk out)
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_USBTRC_ENDPOINT_TYPE usbtrc_look_for_endpoint_type (UINT16 interface_id, UINT16 endpoint_id)
{	
	/* Check if the endpoint configuration with an control endpoint */
	if(	(usbd->usb_ep_cnfg->ep_control_context.interface_id == interface_id) &&
		(usbd->usb_ep_cnfg->ep_control_context.endpoint_id == endpoint_id) )
	{
		return USBTRC_CONTROL_CONTEXT;
	}

	/* Check if the endpoint configuration with an interrupt endpoint */
	if(	(usbd->usb_ep_cnfg->ep_int_context.interface_id == interface_id) &&
		(usbd->usb_ep_cnfg->ep_int_context.endpoint_id == endpoint_id) )
	{
		return USBTRC_INT_CONTEXT;
	}

	/* Check if the endpoint configuration with a bulk IN endpoint */
	if(	(usbd->usb_ep_cnfg->ep_tx_context.interface_id == interface_id) &&
		(usbd->usb_ep_cnfg->ep_tx_context.endpoint_id == endpoint_id) )
	{
		return USBTRC_TX_CONTEXT;
	}

	/* Check if the endpoint configuration with a bulk OUT endpoint */
	if(	(usbd->usb_ep_cnfg->ep_rx_context.interface_id == interface_id) &&
		(usbd->usb_ep_cnfg->ep_rx_context.endpoint_id == endpoint_id) )
	{
		return USBTRC_RX_CONTEXT;
	}

	return USBTRC_UNKNOWN;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_control_request
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
T_RVM_RETURN usbtrc_process_control_request (T_RV_HDR* msg_p)
{	
	/* Check message validity */
	if((msg_p->msg_id == USB_TX_BUFFER_EMPTY_MSG) && (usbd->serial_state_buffer != NULL))
	{
		/* Free sent buffer */
		rvf_free_buf(usbd->serial_state_buffer);
		usbd->serial_state_buffer = NULL;
		return RVM_OK;
	}

	return RVM_INTERNAL_ERR;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_write_usb
 *
 * Request to USB SWE to write data on IN BULK endpoint.
 *
 * @param buf:	where data to write are located
 * @param size:	amount of bytes to write
 *
 * @return:	RVM_OK when successfully call
 *			RVM_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/

T_RVM_RETURN usbtrc_write_usb (UINT8 *buf, UINT32 size)
{
	T_USBTRC_USB_DEVICE *usbdev = usbtrc_env_ctrl_blk_p->usb_device;

	USBTRC_SEND_TRACE("USBTRC: enter usbtrc_write_usb ",RV_TRACE_LEVEL_ERROR);
	
//	usbdev->write_in_progress = FALSE;
	/*always ok to write no data*/
	if(size == 0)
		return RVM_OK;

	/* if USB is disconnected return 0 data written */
	if(usbtrc_get_connected_flag() == FALSE)
	{
		/*if there is no host data are dropped*/
		return RVM_INTERNAL_ERR;
	}

	/* if DTR is low, return 0 data written */
	if(usbd->usb_line_state.host_dtr == 0)
	{
		/*if host dtr is low, this means there is no host*/
		/*data are dropped and we return error so that upper*/
		/*layer may be notified*/
		return RVM_INTERNAL_ERR;
		
	}

	/* if RTS is low, don't try to write */
	//riviera host trace multiplexer does not set rts
	//if(usbd->usb_line_state.host_rts == 0)
	//{
		/*if host rts is low, this means there is no host*/
		/*data are dropped and we return error so that upper*/
		/*layer may be notified*/
		//return RVM_INTERNAL_ERR;
	//}

	usbdev->write_in_progress = TRUE;

	if(usb_set_tx_buffer(	usbd->usb_ep_cnfg->ep_tx_context.interface_id,
							usbd->usb_ep_cnfg->ep_tx_context.endpoint_id,
							buf, size, FALSE) != RVM_OK)

	{
		usbdev->write_in_progress = FALSE;	
		return RVM_INTERNAL_ERR;
	}

	

	/* report DSR hight to host */
//	usbtrc_report_dsr_hight();

	return RVM_OK;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_tx_context
 *
 * This function is called on receiption of the event USB_TX_BUFFER_EMPTY_MSG
 * on the Bulk IN endpoint.
 * If all data of the request are transmitted, next request is processed in the FIFO.
 * At ther oppossite if the write request is not yet finished 
 *
 * @param void			
 *
 * @return				RVM_OK
 *						RVM_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/


T_RVM_RETURN usbtrc_process_tx_context (void)
{
	UINT16	size;
	T_USBTRC_USB_DEVICE *usb_p = usbtrc_env_ctrl_blk_p->usb_device;

	USBTRC_SEND_TRACE("USBTRC: usbtrc_process_tx_context", RV_TRACE_LEVEL_WARNING);

//	usb_p->write_in_progress = FALSE;

	/* extract some data from queue */
//	size = usbtrc_q_tx_get(usbd->buffer_tx_temp, USBTRC_MAX_PACKET_SIZE);
//	size = usbtrc_q_tx_get(usbd->buffer_tx_temp, qw.size);

	if(usbd->tx_size == 0) /* queue was empty */
	{
		USBTRC_SEND_TRACE("USBTRC: usbtrc_process_tx_context empty queue", RV_TRACE_LEVEL_WARNING);
		usb_p->write_in_progress = FALSE;
		return RVM_OK;
	}

	/* then send data on usb in bulk endpoint */
//	if(usbtrc_write_usb((UINT8*)usbd->buffer_tx_temp, size) == RVM_INTERNAL_ERR)
	if(usbtrc_write_usb((UINT8*)usbd->buffer_tx_temp, usbd->tx_size) == RVM_INTERNAL_ERR)		
	{
		return RVM_INTERNAL_ERR;
	}

	return RVM_OK;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name usbtrc_process_rx_context
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
 *						RVM_INTERNAL_ERR	: Incompatible message
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RVM_RETURN usbtrc_process_rx_context (T_RV_HDR* msg_p)
{
	UINT16 size = ((T_USB_RX_BUFFER_FULL_MSG *)msg_p)->size;
	UINT16	i;

	/* queue received data to rx queue */
	usbtrc_q_rx_put(usbd->buffer_rx_temp,size);

	/* notify USBT of all bytes received */
	if(usbtrc_env_ctrl_blk_p->callback_function != NULL)
	{
		for(i=0; i<size; i++)
		{
			usbtrc_env_ctrl_blk_p->callback_function();
		}
	}

	/* give a buffer to the USB LLD */
	return usbtrc_set_rx_buffer_bulk();
}
/*@}*/
