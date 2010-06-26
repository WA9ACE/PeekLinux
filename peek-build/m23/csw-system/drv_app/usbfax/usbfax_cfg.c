/**
 * @file	usbfax_cfg.c
 *
 * USBFAX endpoints configuration 
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
 *	20/02/2004
 */

#include <string.h>

#include "usbfax/usbfax_i.h"
#include "usbfax/usbfax_env.h"

#include "rvf/rvf_api.h"

/*--------------------------------------------------------------------------------------*/
/* Endpoints size																		*/
/*--------------------------------------------------------------------------------------*/

/* Interrupt endpoint size */
#define USBFAX_ENDPOINT_INTERRUPT_SIZE	 8

/* Bulk endpoint size */
#define USBFAX_ENDPOINT_BULK_SIZE		64

/*--------------------------------------------------------------------------------------*/
/* CDC device constants																	*/
/*																						*/
/*--------------------------------------------------------------------------------------*/
/* description of control endpoint */

#if (TRACE_ON_USB == 0)
	#define	USBFAX_CDC1_IF_CONTROL			 0	
	#define	USBFAX_CDC1_EP_CONTROL			 0

	/* description of interrupt endpoint */
	#define	USBFAX_CDC1_IF_INT				 0
	#define	USBFAX_CDC1_EP_INT				 3

	/* description of IN Bulk endpoint */
	#define	USBFAX_CDC1_IF_TX_CONTEXT		 0
	#define	USBFAX_CDC1_EP_TX_CONTEXT		 1

	/* description of OUT Bulk endpoint */
	#define	USBFAX_CDC1_IF_RX_CONTEXT		 0
	#define	USBFAX_CDC1_EP_RX_CONTEXT		 2
#else
	#define	USBFAX_CDC1_IF_CONTROL			 1	
	#define	USBFAX_CDC1_EP_CONTROL			 0

	/* description of interrupt endpoint */
	#define	USBFAX_CDC1_IF_INT				 1
	#define	USBFAX_CDC1_EP_INT				 3

	/* description of IN Bulk endpoint */
	#define	USBFAX_CDC1_IF_TX_CONTEXT		 1
	#define	USBFAX_CDC1_EP_TX_CONTEXT		 1

	/* description of OUT Bulk endpoint */
	#define	USBFAX_CDC1_IF_RX_CONTEXT		 1
	#define	USBFAX_CDC1_EP_RX_CONTEXT		 2
#endif

/* Transmission rate supported by the device */
#define ALL_DIO_BAUD_CONFIGURATION						\
									DIO_BAUD_921600 |	\
									DIO_BAUD_812500 |	\
									DIO_BAUD_460800 |	\
									DIO_BAUD_406250 |	\
									DIO_BAUD_230400 |	\
									DIO_BAUD_203125 |	\
									DIO_BAUD_115200 |	\
									DIO_BAUD_57600  |	\
									DIO_BAUD_38400  |	\
									DIO_BAUD_33900  |	\
									DIO_BAUD_28800  |	\
									DIO_BAUD_19200  |	\
									DIO_BAUD_14400  |	\
									DIO_BAUD_9600   |	\
									DIO_BAUD_7200   |	\
									DIO_BAUD_4800   |	\
									DIO_BAUD_2400   |	\
									DIO_BAUD_1200   |	\
									DIO_BAUD_600    |	\
									DIO_BAUD_300    |	\
									DIO_BAUD_150    |	\
									DIO_BAUD_110    |	\
									DIO_BAUD_75

/* supported character framing of the device */
#define ALL_DIO_CF_CONFIGURATION					\
									DIO_CF_7N1 |	\
									DIO_CF_7S1 |	\
									DIO_CF_7M1 |	\
									DIO_CF_7E1 |	\
									DIO_CF_7O1 |	\
									DIO_CF_7N2 |	\
									DIO_CF_8N1 |	\
									DIO_CF_8S1 |	\
									DIO_CF_8M1 |	\
									DIO_CF_8E1 |	\
									DIO_CF_8O1 |	\
									DIO_CF_8N2


extern T_USBFAX_ENV_CTRL_BLK	*usbfax_env_ctrl_blk_p;

/*--------------------------------------------------------------------------------------*/
/* Endpoints context configuration 1																*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_context_cnfg1
 *
 *	CDC endpoints configuration
 */
/*@{*/
const T_USBFAX_ENDPOINT_CONTEXT_CNFG	endpoint_context_cnfg1 =
{
	{
		USBFAX_CDC1_IF_CONTROL,
		USBFAX_CDC1_EP_CONTROL,
		USBFAX_ENDPOINT_INTERRUPT_SIZE
	},
	{
		USBFAX_CDC1_IF_INT,
		USBFAX_CDC1_EP_INT,
		USBFAX_ENDPOINT_BULK_SIZE
	},
	{
		USBFAX_CDC1_IF_TX_CONTEXT,
		USBFAX_CDC1_EP_TX_CONTEXT,
		USBFAX_ENDPOINT_BULK_SIZE
	},
	{
		USBFAX_CDC1_IF_RX_CONTEXT,
		USBFAX_CDC1_EP_RX_CONTEXT,
		USBFAX_ENDPOINT_BULK_SIZE
	}
};

/*--------------------------------------------------------------------------------------*/
/* 																						*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name cdc_ep_cnfg_table
 *
 * This table contains all available CDC USB configurations
 */
/*@{*/
const T_USBFAX_ENDPOINT_CONTEXT_CNFG	*cdc_ep_cnfg_table[USBFAX_NUM_DEVICES] =
{
	&endpoint_context_cnfg1
};
/*@}*/

/**
 * @name cdc_device_capabilities
 *
 * Device Capabilities of the line serial device emulator
 *
 */
/*@{*/
T_DIO_CAP_SER	cdc_device_capabilities_cnfg1 =
{
	DIO_TYPE_SER,				/* device type  : DIO_TYPE_SER is used for serial devices like UART.*/
	DIO_FLAG_SLEEP,				/* device flags : The device can enter a power saving state */
	sizeof(T_DIO_CTRL_LINES),	/* mtu control		*/
	MAX_PACKET_USBFAX_SIZE,	/* mtu data			*/
	"USBFAX",				/* driver name		*/
	ALL_DIO_BAUD_CONFIGURATION,	/* baudrate_auto	*/
	ALL_DIO_BAUD_CONFIGURATION,	/* baudrate_fixed	*/
	ALL_DIO_CF_CONFIGURATION,	/* char_frame		*/
	DIO_FLOW_RTS_CTS,			/* flow_control		*/
	DIO_FLAG_SER_ESC			/* ser_flags : The device supports escape sequence detection. */
};

/**
 * @name cdc_ep_cnfg_table
 *
 * This table contains all available CDC USB configurations
 */
/*@{*/
T_DIO_CAP_SER	*cdc_device1_capabilities_table[USBFAX_NUM_DEVICES] =
{
	&cdc_device_capabilities_cnfg1
};
/*@}*/


// Function declaration
SHORT usbfax_pei_timeout (int h1,int h2,USHORT tnum);



/**
 * @name usbfax_init_cdc_devices
 *
 * This function reads configuration from the flash
 * and initializes read and write queues.
 * This function is called at start
 *
 * @return	RVM_OK
 */
/*@{*/
T_RVM_RETURN	usbfax_init_cdc_devices()
{
	T_RVM_RETURN			ret				= RVM_OK;

	/* Index on the current device */
	U32						device_index	= 0;
	
	/* Reference to the current device */
	T_USBFAX_USB_DEVICE		*current_device = NULL;
	
	/* For each device */
	for(device_index = 0; device_index < USBFAX_NUM_DEVICES; device_index++)
	{
		/* Get reference to the device */
		current_device = &USBFAX_TAB_OF_DEVICES[device_index];
		memset(current_device, 0x00, sizeof(T_USBFAX_USB_DEVICE));

		/* Assign device number to correct as defined in the DIO interface */
		current_device->device_number		= DIO_DRV_USB| DIO_TYPE_SER | (device_index + 1);
		USBFAX_TRACE_WARNING_PARAM("USBFAX: Device number instance", current_device->device_number);
		
		/* get from flash endpoints configuration */
		current_device->usb_ep_cnfg			= cdc_ep_cnfg_table[device_index];
		
		
		current_device->device_capabilities	= cdc_device1_capabilities_table[device_index];

		/* Initialize read reaquests queue */
		current_device->read_request_queue.first_element	= -1;
		current_device->read_request_queue.last_element		= -1;
		current_device->read_request_queue.current_element	= -1;

		/* Initialize write reaquests queue */
		current_device->write_request_queue.first_element	= -1;
		current_device->write_request_queue.last_element	= -1;
		current_device->write_request_queue.current_element	= -1;

		/* 
		 * Obtain a timer resource.
		 * The zero parameter initialises it in a disabled state.
		 * The boolean parameter signifies a non-continuous behaviour.
		 * Note: it is possible to enable it on invocation, by specifying
		 * an expiry value. A timer ID of Zero is a failure.
		 */
#if (REMU==0) /* no need to create timer for remu pasive entities */
		current_device->timer = rvf_create_timer(usbfax_env_ctrl_blk_p->addr_id, 0, FALSE, current_device);
#else
	       current_device->timer=rvf_create_timer(usbfax_env_ctrl_blk_p->addr_id,0,FALSE, usbfax_pei_timeout );
#endif
	}
	return ret;
}
/*@}*/


