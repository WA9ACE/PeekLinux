/**
 * @file
 *
 * @brief Internal declarations for USBTRC.
 *
 * This file contains specific internal declarations for the USBTRC SWE.
 * (Add detailed information here).
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBTRC_INST_I_H_
#define __USBTRC_INST_I_H_


#include "config/swconfig.cfg"
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "usbtrc/usbtrc_cfg.h"
#include "usbtrc/usbtrc_api.h"
#include "usbtrc/usbtrc_message.h"
#include "usbtrc/usbtrc_queue.h"

#include "usbtrc/usbtrc_state_i.h"


/**
 * Constants definition
 */

/**
 * @name Interface data
 *
 * This type definition describes a data type holds all interface specific data
 */

#define USBTRC_ENABLE_TRACE		0

#if ((TRACE_ON_USB == 0) && (USBTRC_ENABLE_TRACE == 1))
	#define USBTRC_SEND_TRACE(string, trace_level) \
		rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, USBTRC_USE_ID)
	#define USBTRC_TRACE_WARNING_PARAM(string, param) { \
		rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, USBTRC_USE_ID); }
#else
	#define USBTRC_SEND_TRACE(string, trace_level)
	#define USBTRC_TRACE_WARNING_PARAM(string, param)
#endif


/*queue critical sections definitions*/
#define rx_lock()  rvf_lock_mutex (&usbtrc_env_ctrl_blk_p->q_rx_mutex)
#define rx_unlock()  rvf_unlock_mutex (&usbtrc_env_ctrl_blk_p->q_rx_mutex)

#define tx_lock()  rvf_lock_mutex (&usbtrc_env_ctrl_blk_p->q_tx_mutex)
#define tx_unlock()  rvf_unlock_mutex (&usbtrc_env_ctrl_blk_p->q_tx_mutex)

/*USB transfers definitions*/
#define	USBTRC_NUM_DATA_PACKET		3	/* number of packet of 64 bytes to tranmit */
#define	USBTRC_BULK_ENDPOINT_SIZE	64
#define	USBTRC_MAX_PACKET_SIZE		( USBTRC_NUM_DATA_PACKET * USBTRC_BULK_ENDPOINT_SIZE )	

/*internal buffers size*/
#define BUFFER_TX_SIZE 0x800

#if	(LOCOSTO_LITE)
#define BUFFER_RX_SIZE 0x400
#else
#define BUFFER_RX_SIZE 4096
#endif // LOCOSTO_LITE

/*USB Class requests defines*/
#define	USBTRC_SERIAL_STATE_SIZE		10
#define USBTRC_SERIAL_REQUEST_SIZE	8

#define	SERIAL_STATE_REQUEST_TYPE	0xA1	/*	bmRequestType : 10100001B */
#define	SERIAL_STATE_NOTIFICATION	0x20
#define	SERIAL_STATE_DATA_LENGTH	0x02

#define LENGTH_VENDOR_REQUEST_COMMAND	8	/* size of SETUP packet is 8 */
#define LENGTH_VENDOR_REQUEST_DATA		7	/* size of line coding structure */

#define LENGTH_VENDOR_REQUEST_PACKET	    ( LENGTH_VENDOR_REQUEST_COMMAND + LENGTH_VENDOR_REQUEST_DATA )
#define LENGTH_SET_LINE_CODING_REQUEST   ( LENGTH_VENDOR_REQUEST_COMMAND + LENGTH_VENDOR_REQUEST_DATA )

#define SET_LINE_CODING_REQUEST			0x20
#define GET_LINE_CODING_REQUEST			0x21
#define SET_CONTROL_LINE_STATE_REQUEST	0x22
#define SEND_BREAK_REQUEST				0x23

#define	USBTRC_SERIAL_STATE_RX_CARRIER	0x01	// DCD		indicator is set on bitmask 1
#define	USBTRC_SERIAL_STATE_TX_CARRIER	0x02	// DSR		indicator is set on bitmask 2
#define	USBTRC_SERIAL_STATE_BREAK		0x04	// Break	indicator is set on bitmask 3
#define	USBTRC_SERIAL_STATE_RING_SIGNAL	0x08	// Ringmask indicator is set on bitmask 4


/**
 * Endpoints size
 */

/* Interrupt endpoint size */
#define USBTRC_ENDPOINT_INTERRUPT_SIZE	 8

/* Bulk endpoint size */
#define USBTRC_ENDPOINT_BULK_SIZE		64


/**
 * CDC device constants
 */
#if (TRACE_ON_USB == 0)
	/* description of control endpoint */
	#define	USBTRC_CDC1_IF_CONTROL		2
	#define	USBTRC_CDC1_EP_CONTROL		0

	/* description of IN Bulk endpoint */
	#define	USBTRC_CDC1_IF_TX_CONTEXT	2
	#define	USBTRC_CDC1_EP_TX_CONTEXT	1

	/* description of OUT Bulk endpoint */
	#define	USBTRC_CDC1_IF_RX_CONTEXT	2
	#define	USBTRC_CDC1_EP_RX_CONTEXT	2

	/* description of interrupt endpoint */
	#define	USBTRC_CDC1_IF_INT			2
	#define	USBTRC_CDC1_EP_INT			3

	#define	USBTRC_INTERFACE_MAX		1
#else
	/* description of control endpoint */
	#define	USBTRC_CDC1_IF_CONTROL		0
	#define	USBTRC_CDC1_EP_CONTROL		0

	/* description of IN Bulk endpoint */
	#define	USBTRC_CDC1_IF_TX_CONTEXT	0
	#define	USBTRC_CDC1_EP_TX_CONTEXT	1

	/* description of OUT Bulk endpoint */
	#define	USBTRC_CDC1_IF_RX_CONTEXT	0
	#define	USBTRC_CDC1_EP_RX_CONTEXT	2

	/* description of interrupt endpoint */
	#define	USBTRC_CDC1_IF_INT			0
	#define	USBTRC_CDC1_EP_INT			3

	#define	USBTRC_INTERFACE_MAX		1
#endif

/**
 * Structures and enum definitions
 */

/**
 * @name T_USBTRC_ENDPOINT_TYPE
 *
 *
 *
 *
 */
/*@{*/
typedef enum
{
	USBTRC_UNKNOWN,
	USBTRC_CONTROL_CONTEXT,		/* endpoint on which is received */
	USBTRC_INT_CONTEXT,			/* endpoint on which is sent the the serial state */
	USBTRC_TX_CONTEXT,			/* IN bulk endpoint */
	USBTRC_RX_CONTEXT			/* OUT bulk endpoint */
} T_USBTRC_ENDPOINT_TYPE;
/*@}*/

/**
 * @name T_CDC_LINE_CODING
 *
 * Line coding structure
 * updated on receiption host SET_LINE_CODING vendor request
 * This structure is returned by GET_LINE_CODING request
 */
/*@{*/
typedef struct
{
	UINT32	data_rate;		/* Data terminal rate, in bits per second	*/
	UINT8	char_format;	/* Stop bits								*/
	UINT8	parity_type;	/* Parity									*/
	UINT8	data_bits;		/* Data bits								*/
} T_CDC_LINE_CODING;
/*@}*/

/**
 * @name T_CDC_SERIAL_STATE
 *
 * Bitmap representing the state of modem signal 
 * and UART errors. This structure is updated by the DIO user
 *
 *	- D15..D7 RESERVED (future use)
 *	- D6 bOverRun Received data has been discarded due to overrun in the device.
 *	- D5 bParity A parity error has occurred.
 *	- D4 bFraming A framing error has occurred.
 *	- D3 bRingSignal State of ring signal detection of the device.
 *	- D2 bBreak State of break detection mechanism of the device.
 *	- D1 bTxCarrier State of transmission carrier. 
 *	- D0 bRxCarrier State of receiver carrier detection mechanism of device. 
 */
/*@{*/
typedef UINT16	T_CDC_SERIAL_STATE;
/*@}*/

/**
 * @name T_CDC_LINE_STATE
 *
 * Stores the line signals sets by the USB Host
 * This structure is updated on reception of the 
 * SET_CONTROL_LINE_STATE vendor request 
 */
/*@{*/
typedef struct
{
	UINT8	host_rts;	/* RTS signal of the host */
	UINT8	host_dtr;	/* DTR signal of the host */
} T_CDC_LINE_STATE;
/*@}*/

/**
 * @name T_USB_ENDPOINT_CONTEXT
 *
 *
 *
 *
 */
/*@{*/
typedef struct
{
	UINT8	interface_id;
	UINT8	endpoint_id;
	UINT8	MaxPacketSize;
} T_USBTRC_ENDPOINT_CONTEXT;
/*@}*/

/**
 * @name T_USBTRC_ENDPOINT_CONTEXT_CNFG
 *
 *
 *
 *
 */
/*@{*/
typedef struct
{
  	T_USBTRC_ENDPOINT_CONTEXT	ep_control_context;
 	T_USBTRC_ENDPOINT_CONTEXT	ep_int_context;
	T_USBTRC_ENDPOINT_CONTEXT	ep_rx_context;
  	T_USBTRC_ENDPOINT_CONTEXT	ep_tx_context;
} T_USBTRC_ENDPOINT_CONTEXT_CNFG;
/*@}*/

/**
 * @name T_DIO_USB_DEVICE
 *
 * The structure T_DIO_USB_DEVICE contains a device context. 
 * It represents a device instance.
 *
 */
/*@{*/
typedef struct
{
  	T_CDC_LINE_CODING						usb_line_coding;	/* Line coding configuration */
  	T_CDC_SERIAL_STATE						usb_serial_state;	/* Serial state of the device */
	T_CDC_LINE_STATE						usb_line_state;		/* Line state of the host (RTS and CTS signals) */

	const T_USBTRC_ENDPOINT_CONTEXT_CNFG	*usb_ep_cnfg;	/* endpoints configuration */

  	T_USBTRC_XFER_QUEUE					queue_write;	/* queue of write request */
   	T_USBTRC_XFER_QUEUE					queue_read;		/* queue of read request */
	BOOLEAN									read_suspended;	/* TRUE when read is suspended (due to read queue full) */
	BOOLEAN									write_in_progress; /*TRUE when an USB Write is pending*/

	char*									buffer_tx_temp;			/* Temporary buffer used to store data to transmit */
	UINT16									tx_size;
  	char*									buffer_rx_temp;			/* Temporary buffer used to store data to receive */

  	BOOLEAN									connected;				/* FALSE when USB device is disconnected */

	UINT8									vendor_request_buffer[LENGTH_VENDOR_REQUEST_PACKET];	/* Buffer used to store vendor requests */
	UINT8*									vendor_response_buffer;		/* pointer on a buffer for response in case of device to host request */

	UINT8*									serial_state_buffer;		/* pointer on a buffer for serial state sent on pipe interrupt */

	UINT8									escape_sequence;			/* used to detect escape sequence between two frame */
} T_USBTRC_USB_DEVICE;
/*@}*/

/**
 * @brief The Control Block buffer of USBTRC.
 *
 * This structure gathers all the 'global variables' of the USBTRC instance.
 *
 * Indeed, global variables have not to be defined in order to avoid
 * using static memory.
 *
 * On the contrary, a T_USBTRC_ENV_CTRL_BLK buffer is allocated by the
 * Riviera Manager when creating the USBTRC instance. Then, the USBTRC instance
 * must always refer to this control block when access to 'global variable' 
 * is needed.
 */
typedef struct
{
	/// The current state of the USBTRC instance.
	T_USBTRC_INTERNAL_STATE state;

	/// A pointer to the error function.
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);
	/// The memory bank identifier used to allocate memory buffers.
	T_RVF_MB_ID prim_mb_id;

	/// The USBTRC SWE address identifier used for communication purpose.
	T_RVF_ADDR_ID	addr_id;

	/// Stores return path to this test SWE
	T_RV_RETURN_PATH	usbtrc_return_path;

	/// USB device instance
	T_USBTRC_USB_DEVICE	*usb_device;

/*###==>ANO USBTRC  22102004 YL*/
	/// Mutex to access the write queue
	T_RVF_MUTEX q_tx_mutex;

	/// Mutex to access the read queue
	T_RVF_MUTEX q_rx_mutex;

/*<==###ANO USBTRC*/

	/// Callback used to signal received bytes to USBT user
	void (*callback_function) (void);

	/// Interface id
	UINT8	interface_id;

} T_USBTRC_ENV_CTRL_BLK;

/**
 * Global variables declaration
 */
// External reference to the "global variables" structure pointer.
extern T_USBTRC_ENV_CTRL_BLK	*usbtrc_env_ctrl_blk_p;


/**
 * Functions declaration
 */
extern BOOLEAN usbtrc_ready (void);
extern void usbtrc_change_internal_state (T_USBTRC_INTERNAL_STATE state);
extern T_RVM_RETURN usbtrc_get_mem (char** buffer_pp, UINT16 size);

extern T_RVM_RETURN usbtrc_create_usb_device (void);
extern T_RVM_RETURN usbtrc_destroy_usb_device (void);

extern T_RVM_RETURN usbtrc_process_usb_disconnected (void);
extern T_RVM_RETURN usbtrc_process_control_lines (void);
extern T_RVM_RETURN usbtrc_process_vendor_request (T_RV_HDR* msg_p);
extern T_RVM_RETURN usbtrc_process_control_request (T_RV_HDR* msg_p);
extern T_RVM_RETURN usbtrc_process_set_line_coding_request (T_RV_HDR* msg_p);
extern T_RVM_RETURN usbtrc_process_set_line_state_request (T_RV_HDR* msg_p);
extern T_RVM_RETURN usbtrc_process_get_line_coding_request (T_RV_HDR* msg_p);
extern T_RVM_RETURN usbtrc_process_tx_context (void);
extern T_RVM_RETURN usbtrc_process_rx_context (T_RV_HDR* msg_p);

/*extern T_RVM_RETURN usbtrc_suscribe_usb (void);*/
extern T_RVM_RETURN	usbtrc_suscribe_to_cdc_interfaces (void);
extern T_RVM_RETURN usbtrc_suscribe_to_interfaces (UINT8 interface_number);
extern T_USBTRC_ENDPOINT_TYPE usbtrc_look_for_endpoint_type (UINT16 interface_id, UINT16 endpoint_id);
extern void usbtrc_set_connected_flag (BOOLEAN connected);
extern BOOLEAN usbtrc_get_connected_flag (void);
extern void usbtrc_report_dsr_hight (void);
extern void usbtrc_report_dsr_low (void);
extern T_RVM_RETURN usbtrc_write_usb (UINT8 *buf, UINT32 size);
extern T_RVM_RETURN usbtrc_set_rx_buffer_control (void);
extern T_RVM_RETURN usbtrc_set_rx_buffer_bulk (void);
extern BOOLEAN usbtrc_rx_suspended (void);
extern BOOLEAN usbtrc_write_in_progress (void);
extern T_RVM_RETURN process_rx_or_tx_message_buffer (T_RV_HDR *msg_p, UINT8 interface_id, UINT8 endpoint_id);
extern UINT16 usbtrc_write_fifo(T_USBTRC_XFER_QUEUE *fifo_struct, char *buf_p, UINT16 size);
extern UINT16 usbtrc_read_fifo(T_USBTRC_XFER_QUEUE *fifo_struct, char *buf_p, UINT16 size);




#endif // __USBTRC_INST_I_H_
