/**
 * @file	usb_message.h
 *
 * Data structures:
 * 1) used to send messages to the USB SWE,
 * 2) USB can receive.
 *
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)		
 *  14/01/2004  Pierre-Olivier POUX		Adding suspend and resume messages
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __USB_MESSAGE_H_
#define __USB_MESSAGE_H_


#include "rv/rv_general.h"

#include "usb/usb_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define USB_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(USB_USE_ID)

/****************************************************************************************
 |This part describes the message defintions of messages that are send from the FM		|
 |to the usb driver. the memory claimed for those message by the FM will be freed		|
 |by the usb driver																		|
 V**************************************************************************************V*/

/**
 * @name USB_FM_SUBSCRIBE_MSG
 *
 * this message must be used by a FM to subscribe to an interface.
 *
 * Every interface is controlled by a function manager. This message is used
 * to actually subsribe the defined FM to the specified interface.
 * Theoratically the usb driver can see which FM is subscribing by reading the
 * "hdr". In the case that 1 FM supports more than 1 interface the 
 * FM must indicate during subscription, to which interface it will subscribe
 */
/*@{*/
/** Message ID. */

#define USB_FM_SUBSCRIBE_MSG (USB_MESSAGE_OFFSET | 0x001)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;//his variable is used by FM to indicate to which IF 
									 // wants to subscribe
	T_RV_RETURN			return_path; //use this path to notify swe of buffer full / empty etc
} T_USB_FM_SUBSCRIBE_MSG;
/*@}*/

/**
 * @name USB_FM_UNSUBSCRIBE_MSG
 *
 * this message must be used by a FM to release it subsrciption to an interface.
 *
 * Every interface is controlled by a function manager. This message is used
 * to release the subsription with the defined FM to the specified interface.
 */
/*@{*/
/** Message ID. */
#define USB_FM_UNSUBSCRIBE_MSG (USB_MESSAGE_OFFSET | 0x002)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;/*This variable is used by FM to indicate to which IF 
									  *it wants to subscribe*/
} T_USB_FM_UNSUBSCRIBE_MSG;
/*@}*/

/**
 * @name USB_GET_STATUS_MSG
 *
 * this message must be used by a FM to get status info.
 *
 * The FM send this message to retrieve status information about the endpoints
 * and the USB Hardware. The diver will respond to this message by sending a 
 * T_USB_STATUS_READY_MSG
 */
/*@{*/
/** Message ID. */
#define USB_GET_STATUS_MSG (USB_MESSAGE_OFFSET | 0x003)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;	/*This variable holds an interface_id part of the
										 *FM that placed the get status request so that 
										 *a response can be send to it*/
	T_USB_STATUS*		status_p;		/*pointer to status information storage space
										 *created by FM, filled by usb driver*/
} T_USB_GET_STATUS_MSG;
/*@}*/

/**
 * @name USB_SET_TX_BUFFER_MSG
 *
 * This message provides the usb driver with a buffer containing data to be send
 *
 * The driver splits the buffer in data packets as large as the available endpoints 
 * fifo
 */
/*@{*/
/** Message ID. */
#define USB_SET_TX_BUFFER_MSG (USB_MESSAGE_OFFSET | 0x004)
#define USB_SET_RX_BUFFER_MSG (USB_MESSAGE_OFFSET | 0x005)

/** Message structure. */
typedef struct
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;	//This variable is used by FM to indicate to which IF 
										//the specified endpoint belongs
	UINT8				endpoint;		//endpoint associated with the buffer
	UINT8*				buffer_p;		//pointer to the reserved buffer
	UINT16				size;			//Size of buffer in bytes The size mainly depends on the 
										//interface description
	BOOL				shorter_transfer;	//shorter than expected by the host
} T_USB_SET_TX_BUFFER_MSG;
typedef T_USB_SET_TX_BUFFER_MSG T_USB_SET_RX_BUFFER_MSG;
/*@}*/

/**
 * @name USB_RECLAIM_RX_BUFFER_MSG
 *
 * This message gives the FM back the control over the buffer.
 * The USB expects to get a new buffer. 
 */
/*@{*/
/** Message ID. */
#define USB_RECLAIM_RX_BUFFER_MSG (USB_MESSAGE_OFFSET | 0x006)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;	//This variable is used by FM to indicate to which IF 
										//the specified endpoint belongs
	UINT8				endpoint;		//endpoint associated with the buffer
	} T_USB_RECLAIM_RX_BUFFER_MSG;
/*@}*/

/****************************************************************************************
 |This part describes the message defintions of messages that are send from the usb		|
 |driver to the FM. the memory claimed for those message by the usb driver will be freed|
 |by the FM																				|
 V**************************************************************************************V*/
/**
 * @name USB_FM_RESULT_MSG
 *
 * This message returns whether the subscribtion or unsubscription was succesfull 
 * or not
 */
/*@{*/
/** Message ID. */
#define USB_FM_RESULT_MSG (USB_MESSAGE_OFFSET | 0x007)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	T_USB_RESULT		result;		//contains the execution result of action
} T_USB_FM_RESULT_MSG;
/*@}*/

/**
 * @name USB_BUS_CONNECTED_MSG
 *
 * This message is used by the usb driver to inform the FM of a USB bus connection.
 *
 * The USB driver sends this message to ALL FMs to indicate that the USB bus has been
 * connected
 */
/*@{*/
/** Message ID. */
#define USB_BUS_CONNECTED_MSG (USB_MESSAGE_OFFSET | 0x008)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_BUS_CONNECTED_MSG;
/*@}*/

/**
 * @name USB_BUS_DISCONNECTED_MSG
 *
 * This message is used by the usb driver to inform the FM of a USB bus disconnection.
 *
 * The USB driver sends this message to ALL FMs to indicate that the USB bus has been
 * disconnected. Thus the FM cannot transfer data anymore
 */
/*@{*/
/** Message ID. */
#define USB_BUS_DISCONNECTED_MSG (USB_MESSAGE_OFFSET | 0x009)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_BUS_DISCONNECTED_MSG;
/*@}*/

/**
 * @name USB_BUS_SUSPEND_MSG
 *
 * This message is used by the usb driver to inform the FM that the bus bus enters the 
 * suspend state.
 *
 * This message is sent to ALL FMs
 */
/*@{*/
/** Message ID. */
#define USB_BUS_SUSPEND_MSG (USB_MESSAGE_OFFSET | 0x010)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_BUS_SUSPEND_MSG;
/*@}*/

/**
 * @name USB_BUS_RESUME_MSG
 *
 * This message is send to the interfaces that are part of the current active configuration 
 * to indicate that the USB bus is now resuming. Host to device transfers will start soon. 
 *
@{*/
/** Message ID. */
#define USB_BUS_RESUME_MSG (USB_MESSAGE_OFFSET | 0x011)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_BUS_RESUME_MSG;
/*@}*/

/**
 * @name USB_STATUS_READY_MSG
 *
 * This message is used by the usb driver to inform the FM that the requested status data
 * is available.
 *
 */
/*@{*/
/** Message ID. */
#define USB_STATUS_READY_MSG (USB_MESSAGE_OFFSET | 0x012)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_STATUS_READY_MSG;
/*@}*/

/**
 * @name USB_RX_BUFFER_FULL_MSG
 *
 * This message is used by the usb driver to inform the FM that the rx buffer is full.
 * 
 * The USB driver sends this message to an FM to indicate that a the buffer of a specified
 * endpoint of the specified interface has been filled  and that it is ready to be consumed.
 */
/*@{*/
/** Message ID. */
#define USB_RX_BUFFER_FULL_MSG (USB_MESSAGE_OFFSET | 0x013)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				endpoint;		//endpoint associated with the buffer
	UINT8				interface;		//interface associated with the buffer
	UINT16				size;			//number of bytes written in buffer
	BOOL				end_of_packet;	//TRUE = driver has detected end of packet
										//FALSE= no end of packet has been detected
} T_USB_RX_BUFFER_FULL_MSG;
/*@}*/

/**
 * @name USB_TX_BUFFER_EMPTY_MSG
 *
 * This message is used by the usb driver to inform the FM that the tx buffer is empty.
 * 
 * This message is sent by the driver to the FM that owns the endpoint to indicate that 
 * the TX buffer has been sent. The FM can now fill the buffer again.
 */
/*@{*/
/** Message ID. */
#define USB_TX_BUFFER_EMPTY_MSG (USB_MESSAGE_OFFSET | 0x014)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				endpoint;		//endpoint associated with the buffer
	UINT8				interface;		//endpoint associated with the buffer
} T_USB_TX_BUFFER_EMPTY_MSG;
/*@}*/

/**
 * @name USB_TX_EP_INTERRUPT
 *
 * This message is used by the usb driver to inform the FM that the tx buffer is empty.
 * 
 * This message is sent by the driver to the FM that owns the endpoint to indicate that 
 * the TX buffer has been sent. The FM can now fill the buffer again.
 */
/*@{*/
/** Message ID. */
#define USB_DMA_TX_EP_INTERRUPT_MSG (USB_MESSAGE_OFFSET | 0x016)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				endpoint;		//endpoint associated with the buffer
} T_USB_TX_EP_INTERRUPT_MSG;
/*@}*/

/**
 * @name USB_TX_BUFFER_EMPTY_MSG
 *
 * This message is used by the usb driver to inform the FM that the tx buffer is empty.
 * 
 * This message is sent by the driver to the FM that owns the endpoint to indicate that 
 * the TX buffer has been sent. The FM can now fill the buffer again.
 */
/*@{*/
/** Message ID. */
#define USB_RX_EP_INTERRUPT_MSG (USB_MESSAGE_OFFSET | 0x017)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				endpoint;		//endpoint associated with the buffer
} T_USB_RX_EP_INTERRUPT_MSG;
/*@}*/


/**
 * @name USB_IF_READY
 *
 * This message is send by the USB higher level interfaces to the USB LLD signalling that they are ready
 */
/*@{*/
/** Message ID. */
#define USB_IF_READY (USB_MESSAGE_OFFSET | 0x018)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
	UINT8				interface_id;
	T_RV_RETURN			return_path; //use this path to notify swe of buffer full / empty etc	
} T_USB_IF_READY;
/*@}*/




/**
 * @name USB_HOST_RESET_MSG
 *
 * This message is send by the driver to signal the interfaces of the FM that the USB Host
 * has reset the USB device. The necessary actions must be taken by the interface.
 */
/*@{*/
/** Message ID. */
#define USB_HOST_RESET_MSG (USB_MESSAGE_OFFSET | 0x015)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_HOST_RESET_MSG;
/*@}*/

/**
 * @name USB_START_TIMER_MSG
 *
 * This message is send by the driver to signal the interfaces of the FM that the USB Host
 * has reset the USB device. The necessary actions must be taken by the interface.
 */
/*@{*/
/** Message ID. */
#define USB_START_TIMER_MSG (USB_MESSAGE_OFFSET | 0x019)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_START_TIMER_MSG;
/*@}*/

/**
 * @name USB_STOP_TIMER_MSG
 *
 * This message is send by the driver to signal the interfaces of the FM that the USB Host
 * has reset the USB device. The necessary actions must be taken by the interface.
 */
/*@{*/
/** Message ID. */
#define USB_STOP_TIMER_MSG (USB_MESSAGE_OFFSET | 0x01A)

/** Message structure. */
typedef struct 
{
	/** Message header. */
	T_RV_HDR			hdr;
} T_USB_STOP_TIMER_MSG;
/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* __USB_MESSAGE_H_ */
