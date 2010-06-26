/**
 * @file	usbfax_task_i.h
 *
 * Declarations of the USBFAX SWE Functions
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

#ifndef __USBFAX_TASK_I_H_
#define __USBFAX_TASK_I_H_

#include "usbfax/usbfax_i.h"

/****************************************************************************************
 |	FUNCTIONS	DECLARATIONS															|
 V**************************************************************************************V*/

/**
 * @name internal functions to USBFAX SWE
 *
 * Prototype imposed by DIO SWE
 *
 */
/*@{*/
T_RVM_RETURN	usbfax_cdc_devices_initialize(void);

T_RVM_RETURN	usbfax_process_vendor_request	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);

T_RVM_RETURN	usbfax_process_set_line_coding_request	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);
T_RVM_RETURN	usbfax_process_set_line_state_request	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);
T_RVM_RETURN	usbfax_process_get_line_coding_request	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);
T_RVM_RETURN	usbfax_process_send_break_request		(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);

T_RVM_RETURN	usbfax_process_control_request			(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR* msg_p);

T_RVM_RETURN	usbfax_process_usb_disconnected();

T_RVM_RETURN	usbfax_process_tx_context	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR * msg_p);

T_RVM_RETURN	usbfax_process_rx_context	(T_USBFAX_USB_DEVICE* current_device_p, T_RV_HDR * msg_p);

T_USBFAX_USB_DEVICE*	usbfax_get_device	(U32 device_id);

T_RVM_RETURN	usbfax_process_write(T_USBFAX_USB_DEVICE* current_device_p, T_USBFAX_XFER* request_p);

T_RVM_RETURN	usbfax_process_read	(T_USBFAX_USB_DEVICE* current_device_p, T_USBFAX_XFER* request_p, U32 buffer_size);

T_RVM_RETURN	usbfax_process_control_lines(T_USBFAX_USB_DEVICE* current_device_p);

T_USBFAX_ENDPOINT_TYPE	usbfax_look_for_endpoint_type		(U16 interface_id, U16 endpoint_id);

T_USBFAX_USB_DEVICE*	usbfax_look_for_associated_device	(U16 interface_id, U16 endpoint_id);

T_RVM_RETURN	usbfax_enqueue_request(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type, T_USBFAX_XFER* xfer_request);

T_RVM_RETURN	usbfax_dequeue_request(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type);

T_USBFAX_XFER*	usbfax_get_queue_element(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type);

T_RVM_RETURN	usbfax_update_control_line(T_USBFAX_USB_DEVICE* current_device_p, T_DIO_CTRL_LINES* current_control);

T_RVM_RETURN	usbfax_empty_queue_request	(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type);

T_RVM_RETURN	usbfax_initialize_queue_request	(T_USBFAX_XFER_QUEUE* current_queue_request);

BOOLEAN			usbfax_transfer_in_progress(T_USBFAX_USB_DEVICE* current_device_p);

T_RV_HDR*		usbfax_wait_for_message(U32 msg_id);

T_RVM_RETURN	usbfax_suscribe_to_cdc_interfaces();

T_RVM_RETURN	usbfax_suscribe_to_interfaces(U8 interface_number);

T_RVM_RETURN	usbfax_post_vendor_rx_buffers();

T_USBFAX_XFER*	usbfax_next_queue_request(T_USBFAX_XFER_QUEUE* current_queue_request);

T_RVM_RETURN	usbfax_signal(T_USBFAX_USB_DEVICE* current_device_p, U8 signal_id);

T_RVM_RETURN	usbfax_next_segment(T_USBFAX_XFER* current_request_p);

T_RVM_RETURN	usbfax_get_mem(U8** buffer_pp, U16 size);

T_RVM_RETURN	usbfax_prepare_vendor_rx_buffer_request(T_USBFAX_USB_DEVICE* current_device_p);

BOOLEAN			usbfax_update_serial_state(T_USBFAX_USB_DEVICE* current_device_p, T_DIO_CTRL_LINES* current_control);

BOOLEAN			usbfax_escape_sequence_detect(T_USBFAX_USB_DEVICE* current_device_p);

BOOLEAN			usbfax_process_requests_from_write_fifo(T_USBFAX_USB_DEVICE* current_device_p);

T_USBFAX_XFER*	usbfax_get_current_request(T_USBFAX_XFER_QUEUE* current_queue_request);

BOOLEAN			usbfax_suppress_write_requests_fifo(T_USBFAX_USB_DEVICE* current_device_p);

BOOLEAN			usbfax_queue_is_empty(U32 device_id, T_USBFAX_XFER_REQUEST_TYPE xfer_request_type);

BOOLEAN			usbfax_timer_escape_sequence(T_USBFAX_USB_DEVICE* current_device_p);

T_USBFAX_ESCAPE_STATE	usbfax_change_esc_seq_state(	T_USBFAX_USB_DEVICE* current_device_p, 
														T_USBFAX_ESCAPE_STATE esc_seq_new_state);

BOOLEAN			usbfax_start_timer(T_USBFAX_USB_DEVICE* current_device_p);

BOOLEAN			usbfax_stop_timer(T_USBFAX_USB_DEVICE* current_device_p);
/*@}*/
#endif /*__USBFAX_TASK_I_H_*/
