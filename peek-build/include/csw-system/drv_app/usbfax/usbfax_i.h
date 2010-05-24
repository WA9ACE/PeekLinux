/**
 * @file	usbfax_i.h
 *
 * Internal definitions for USBFAX.
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
 *
 */

#ifndef __USBFAX_I_H_
#define __USBFAX_I_H_

#include "config/swconfig.cfg"
#include "usbfax/dio_usb.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "dar/dar_gen.h"
#include "dar/dar_api.h"

#include "usbfax/usbfax_cfg.h"
#include "usbfax/usbfax_interface_cfg.h"

/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

/****************************************************************************************
 |	MACRO DEFINITIONS																	|
 V**************************************************************************************V*/

/**
 * @name Interface data
 *
 * This type definition describes a data type holds all interface specific data
 */

#define USBFAX_ENABLE_TRACE		0

#if ((TRACE_ON_USB == 0) && (USBFAX_ENABLE_TRACE == 1))
	#define USBFAX_SEND_TRACE(string, trace_level) \
		rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, USBFAX_USE_ID)
	#define USBFAX_TRACE_WARNING_PARAM(string, param) { \
		rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, USBFAX_USE_ID); }
#else
	#define USBFAX_SEND_TRACE(string, trace_level)
	#define USBFAX_TRACE_WARNING_PARAM(string, param)
#endif

#define	USBFAX_INTERFACE_MAX		255
#define	USBFAX_BULK_ENDPOINT_SIZE	 64

#define	MAX_PACKET_USBFAX_SIZE		( USBFAX_NUM_DATA_PACKET * USBFAX_BULK_ENDPOINT_SIZE )

#define USBFAX_TAB_OF_DEVICES		usbfax_env_ctrl_blk_p->tab_of_devices
#define USBFAX_IS_RUNNING			(usbfax_env_ctrl_blk_p->state == USBFAX_IDLE)

#define	USBFAX_SERIAL_STATE_SIZE	 10

#define USBFAX_SERIAL_REQUEST_SIZE	  8

#define	SERIAL_STATE_REQUEST_TYPE	0xA1	/*	bmRequestType : 10100001B */
#define	SERIAL_STATE_NOTIFICATION	0x20
#define	SERIAL_STATE_DATA_LENGTH	0x02

#define LENGTH_VENDOR_REQUEST_COMMAND	 8	/* size of SETUP packet is 8 */
#define LENGTH_VENDOR_REQUEST_DATA		 7	/* size of line coding structure */

#define LENGTH_VENDOR_REQUEST_PACKET	    ( LENGTH_VENDOR_REQUEST_COMMAND + LENGTH_VENDOR_REQUEST_DATA )
#define LENGTH_SET_LINE_CODING_REQUEST   ( LENGTH_VENDOR_REQUEST_COMMAND + LENGTH_VENDOR_REQUEST_DATA )

#define SET_LINE_CODING_REQUEST				0x20
#define GET_LINE_CODING_REQUEST				0x21
#define SET_CONTROL_LINE_STATE_REQUEST		0x22
#define SEND_BREAK_REQUEST					0x23

#define	USBFAX_SERIAL_STATE_RX_CARRIER		0x01	// DCD		indicator is set on bitmask 1
#define	USBFAX_SERIAL_STATE_TX_CARRIER		0x02	// DSR		indicator is set on bitmask 2
#define	USBFAX_SERIAL_STATE_BREAK			0x04	// Break	indicator is set on bitmask 3
#define	USBFAX_SERIAL_STATE_RING_SIGNAL		0x08	// Ringmask indicator is set on bitmask 4

#define	USBFAX_QUEUE_FULL					0x01
#define	USBFAX_UNKNOWN_REQUEST_TYPE			0x02

#define usbfax_lock()		rvf_lock_mutex		(&usbfax_env_ctrl_blk_p->mutex)
#define usbfax_unlock()		rvf_unlock_mutex	(&usbfax_env_ctrl_blk_p->mutex)

/****************************************************************************************
 |	FUNCTIONS	DECLARATIONS															|
 V**************************************************************************************V*/

/****************************************************************************************
 |	TYPE DEFINITIONS																	|
 V**************************************************************************************V*/

/**
 * @name T_USBFAX_INTERNAL_STATE
 *
 *
 *
 *
 *
 *
 */
typedef enum
{
	USBFAX_UNINITIALISED,
	USBFAX_INITIALIZED,
	USBFAX_WAIT_FOR_CDC_INTERFACES_SUSCRIPTION,
	USBFAX_IDLE,
	USBFAX_STOPPED,
	USBFAX_KILLED,
	USBFAX_HANDLE_VENDOR_REQUEST,
	USBFAX_PROCESS_SEND_USB_DATA,
	USBFAX_PROCESS_RECEIVE_USB_DATA
} T_USBFAX_INTERNAL_STATE;

/**
 * @name T_USBFAX_ENDPOINT_TYPE
 *
 *
 *
 *
 */
/*@{*/
typedef enum
{
	USBFAX_UNKNOWN,
	USBFAX_CONTROL_CONTEXT,		/* endpoint on which is received */
	USBFAX_INT_CONTEXT,			/* endpoint on which is sent the the serial state */
	USBFAX_TX_CONTEXT,			/* IN bulk endpoint */
	USBFAX_RX_CONTEXT			/* OUT bulk endpoint */
} T_USBFAX_ENDPOINT_TYPE;
/*@}*/

/**
 * @name T_USBFAX_XFER_REQUEST_TYPE
 *
 *
 *
 *
 */
/*@{*/
typedef enum
{
	USBFAX_XFER_READ_REQUEST,	/* Read request type */
	USBFAX_XFER_WRITE_REQUEST	/* Write request type */
} T_USBFAX_XFER_REQUEST_TYPE;
/*@}*/

/**
 * @name T_USBFAX_XFER_REQUEST_TYPE
 *
 *
 *
 *
 */
/*@{*/
typedef enum
{
	NO_ESCAPE_RECEIVED,
	ESCAPE_RECEIVE_IDLE,
	ONE_ESCAPE_RECEIVED,
	TWO_ESCAPE_RECEIVED,
	THREE_ESCAPE_RECEIVED,
	COMPLETE_ESCAPE_SEQUENCE
} T_USBFAX_ESCAPE_STATE;
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
	U32	data_rate;		/* Data terminal rate, in bits per second	*/
	U8	char_format;	/* Stop bits								*/
	U8	parity_type;	/* Parity									*/
	U8	data_bits;		/* Data bits								*/
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
typedef U16	T_CDC_SERIAL_STATE;
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
	U8	host_rts;	/* RTS signal of the host */
	U8	host_dtr;	/* DTR signal of the host */
} T_CDC_LINE_STATE;

/*@}*/

/**
 * @name T_USBFAX_XFER
 *
 * read or write requests structure from DIO.
 *
 */
/*@{*/
typedef struct
{
	T_DIO_CTRL_LINES	line_control;		/* Data Control for serial line states */
	T_dio_buffer*		buffer_context;		/* Data buffer description */
	U16					segment_size;		/* Size of the current segment */
  	U16					sent_length;		/* Size of received or transmitted segment */
	U16					total_sent_length;	/* total size of received or transmitted data */
	U8					c_align1;			/* used only for alignment reasons */
	U8					segment_index;		/* current segment index */
	U8					c_align2;			/* used only for alignment reasons */
	U8*					segment;			/* Pointer to the current segment */
	BOOLEAN				usb_posted;			/* Indicate if the request is processing by the USB LLD */
} T_USBFAX_XFER;
/*@}*/

/**
 * @name T_USBFAX_XFER_QUEUE
 *
 * Circular table of read or write requests.
 *
 */
/*@{*/
typedef struct
{
	INT8					first_element;			/* index on the first request of the queue */
	INT8					current_element;		/* index on the current request processing */
	INT8					last_element;			/* index on the last request of the queue */
	T_USBFAX_XFER 		tab_xfer[USBFAX_MAX_REQUESTS];
} T_USBFAX_XFER_QUEUE;
/*@}*/

/**
 * @name T_USBFAX_TEMPORARY_BUFFER
 *
 * This structure is used to store received and sent data from and to the host
 *
 *
 */
/*@{*/
typedef struct
{
	U8		buff[MAX_PACKET_USBFAX_SIZE];
	U16		size;				/* size of the temporary buffer */
	BOOLEAN	short_packet;		/* indicates if the temporary is a USB short packet */
} T_USBFAX_TEMPORARY_BUFFER;
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
	U32									device_number;			/* Device number */

  	T_DIO_CAP_SER*					device_capabilities;	/* Device capabilities of the device */
  	T_DIO_DCB_SER							device_config;			/* Device Control Block of a serial device */

  	T_CDC_LINE_CODING						usb_line_coding;		/* Line coding configuration */
  	T_CDC_SERIAL_STATE						usb_serial_state;		/* Serial state of the device */
	T_CDC_LINE_STATE						usb_line_state;			/* Line state of the host (RTS and CTS signals) */

	const T_USBFAX_ENDPOINT_CONTEXT_CNFG*	usb_ep_cnfg;			/* endpoints configuration */

  	T_USBFAX_XFER_QUEUE						write_request_queue;	/* FIFO structure of write request */
   	T_USBFAX_XFER_QUEUE						read_request_queue;		/* FIFO structure of read request */

	BOOLEAN									write_flush_pending;
  	BOOLEAN									write_clear_pending;		/* flag indicating */
  	BOOLEAN									user_connected;				/* indicates if a use	r is connected with the device */
  	BOOLEAN									dtr_host_connected;			/* indicates if a use	r is connected with the device */

	BOOLEAN									signal_write_serial_state;	/* This flag is set when dio buffer is null and serial state has changed */

	T_USBFAX_TEMPORARY_BUFFER			buffer_tx_temp;				/* Temporary buffer used to store data to transmit */
  	T_USBFAX_TEMPORARY_BUFFER			buffer_rx_temp;				/* Temporary buffer used to store data to receive */

	U8									vendor_request_buffer[LENGTH_VENDOR_REQUEST_PACKET];	/* Buffer used to store vendor requests */
	U8*									vendor_response_buffer;		/* pointer on a buffer for response in case of device to host request */

	U8*									serial_state_buffer;		/* pointer on a buffer for serial state sent on pipe interrupt */

	T_USBFAX_ESCAPE_STATE				esc_sequence_state;			/* used to detect escape sequence between two frame */

	T_RVF_TIMER_ID						timer;						/** Timer used for detecting escape sequences */
} T_USBFAX_USB_DEVICE;
/*@}*/

/**
 * @name T_USBFAX_ENV_CTRL_BLK
 *
 * The Control Block buffer of USB, which gathers all 'Global variables'
 * used by USB instance.
 *
 * A structure should gather all the 'global variables' of USB instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_USBFAX_ENV_CTRL_BLK buffer is allocated when creating USBFAX instance and is
 * then always refered by USBFAX instance when access to 'global variable'
 * is necessary.
 */
/*@{*/
typedef struct
{
	T_RVF_MB_ID				prim_mb_id;					/* Mem bank id. */
	T_RVF_ADDR_ID			addr_id;					/* Address id of the current SWE */
	T_RV_RETURN_PATH		usbfax_return_path;		/* stores returnpath to this test SWE */

	/**return path for other SWE to the USB*/
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);

	U16					dvr_handle;								/* driver identifier */
	T_USBFAX_USB_DEVICE	tab_of_devices[USBFAX_NUM_DEVICES];	/* Array of devices */
	T_DRV_CB_FUNC			signal_callback;						/* callback used to signal event to DIO user */

	T_USBFAX_INTERNAL_STATE	state;					/* Store the current state of the USB instance */
	BOOL						driver_ready;			/* indicates if the driver is ready to work */
	BOOL						host_serial_driver_connected;

	T_RVF_MUTEX					mutex;						/* mutex used to protect read and write operations */
} T_USBFAX_ENV_CTRL_BLK;
/*@}*/

/****************************************************************************************
 |	VARIABLE DEFINITIONS																|
 V**************************************************************************************V*/

#endif /* __USB_INST_I_H_ */
