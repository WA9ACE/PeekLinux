/**
 * @file	usb_api.h
 *
 * API Definition for USB SWE.
 * 
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van  (ICT)		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */
#ifndef __USB_API_H_
#define __USB_API_H_
#include "chipset.cfg"

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name USB interface information
 *
 * This is type describes the class of this interface, which is just like the
 * subclass, USB defined (compile time is known what class the interface belongs to)
 * what the class does and how it works is defined by USB (spec 1.1).
 */
/*@{*/
typedef struct
{
	UINT8	interface_id;		/*interface class id (usb defined)*/
	UINT8	subclass_id;	/*interface subclass id*/
}T_USB_FM_ID;
/*@}*/

/**
 * @name USB Status type
 *
 * This is type describes the information that is returned on a get 
 * status request for a specific endpoint
 */
/*@{*/
typedef enum
{
	enabled=0,		/* endpoint is enabled */
	stalled,		/* endpoint is stalled */
	unassociated	/* endpoint does not belong to this interface */
}T_USB_EP_STAT;
/*@}*/

/**
 * @name USB Status type
 *
 * This is type describes the information that is returned on a get 
 * status request
 */
/*@{*/
typedef struct
{
	UINT8	active_config;		//number of the active configuration,
								//-1 if no configuration is active yet
	T_USB_EP_STAT* ep_status_p;	//static list of 30 elements with status per logical endpoint. 
	UINT8	nr_of_ep;			//number of endpoints in list assigned to the interface. 
								//-1 when interface is not part of current active configuration.
	BOOL	driver_ready;		//Indicates if the USB device has been configured and is ready.
	BOOL 	usb_connected;		//Indicates if the USB is connected to an USB bus. Can be used 
								//when a FM is started and does not know if the bus is active.
}T_USB_STATUS;

/**
 * @name USB Status type
 *
 * This is type is used to report back the result of a sucbscribe etc.
 *
 */
/*@{*/
typedef enum
{
	succes = 0,			//action performed successfully
	fail_subscribe,		//generic failure to subscribe
	fail_unsubscribe,	//generic failure to unsubscribe
	not_subscribed,		//unsubscribe while not subscribed
	config_error,		//error in configuration
	param_error,		//parameter error
	unexpected_error	//generic failure
}T_USB_RESULT;
/*@}*/

/**
 * @name USB Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET T_USB_RETURN;

#define	USB_OK					RV_OK				
#define	USB_NOT_SUPPORTED		RV_NOT_SUPPORTED		
#define	USB_MEMORY_ERR			RV_MEMORY_ERR			
#define	USB_INTERNAL_ERR		RV_INTERNAL_ERR	
#define USB_INVALID_CONFIG		(-10)
/*@}*/

/**
 * @name USB Event Type.
 *
 * This type is used to report the USB callback events.
 */
/*@{*/
typedef enum
{
	USB_CABLE_CONNECT_EVENT = 0,	//USB Cable Connection Detected
	USB_CABLE_DISCONNECT_EVENT,		//USB Cable Removal Detected
	USB_ENUM_SUCCESS_EVENT,			//USB Enumeration Successful
	USB_ENUM_FAILURE_EVENT			//USB Enumeration Failed
}T_USB_EVENT;
/*@}*/


/**
 * @name USB event Callback type
 *
 * This is the type of callback to be registered for getting special USb event.
 *
 */
/*@{*/
typedef void (*T_USB_EVENT_CALLBACK)(T_USB_EVENT usbEvent, void *ctx);
/*@}*/

/**
 * @name USB Enumeration Control type.
 *
 * This type is used to control the enumeration.
 */
/*@{*/
typedef enum
{
	USB_ENUM = 0,	//USB Proceed With ENUM
	USB_NO_ENUM		//USB Do not Enumerate
}T_USB_ENUM_CONTROL;
/*@}*/

/**
 * @name USB Enumeration Info Type and Enumeration  values.
 *
 * This type is used to inform USB driver which Drivers to enumerate.
 * Although its possible to make 6 different combination but currently following 
 * configurations are supported.
 * USB_MS
 * USB_AT
 * USB_MS | USB_TRACE
 * USB_AT | USB_TRACE
 * USB_MS | USB_AT | USB_TRACE
 */
/*@{*/
typedef struct
{
	UINT8 info; 
}T_USB_ENUM_INFO;

#define	USB_MS					4				
#define	USB_AT					2		
#define	USB_TRACE				1			
/*@}*/


/**
 * API (bridge) Functions 
 */

/**
 * @name usb_register_notifcation
 *
 * This function must be called by a Application to subscribe for USB event notification.
 *
 *
 * @param	T_USB_EVENT_CALLBACK: callback	the callback function pointer. This function 
 *										    will be called for notifying USB events. 
 * @param	void:				  ctx		This is the context information provided with callback.
 *											This will be provided transparently back to the caller.
 */
/*@{*/
T_USB_RETURN usb_register_notifcation(T_USB_EVENT_CALLBACK callback, void *ctx);
/*@}*/

/**
 * @name usb_enum_control
 *
 * This function must be called by a Application to subscribe for USB event notification.
 *
 *
 * @param	T_USB_ENUM_CONTROL:   enumCtrl	This variable controls the progress of enumeration. 
 *
 * @param	T_USB_ENUM_INFO*: 	  enumInfo	This provides the information of which interface to 
 *                                          enumerate. NULL means enumerate default.
 */
/*@{*/
T_USB_RETURN usb_enum_control(T_USB_ENUM_CONTROL enumCtrl, T_USB_ENUM_INFO * enumInfo);
/*@}*/


/**
 * @name usb_fm_subscribe
 *
 * This function must be called by a FM to subscribe to an interface.
 *
 * Every interface is controlled by a function manager. 
 * This function is used to actually subsribe the defined FM to the specified interface.
 * Theoratically the usb driver can see which FM is subscribing by reading the
 * "hdr". In the case that 1 FM supports more than 1 interface the 
 * FM must indicate during subscription, to which interface it will subscribe
 *
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @param	T_RV_RETURN:	return_path		This is where the caller wants to be adressed
 *											when the USB driver needs to communicate
 *
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 *
 * Normally the USB driver returns the message T_USB_FM_RESULT_MSG to indicate that 
 * subscription was succesfull or not
 */
/*@{*/
T_RV_RET usb_fm_subscribe(UINT8 interface_id, T_RV_RETURN return_path);
/*@}*/

/**
 * @name usb_fm_unsubscribe
 *
 * this function must be called by a FM to release it subsrciption to an interface.
 *
 * Every interface is controlled by a function manager. In the "usb_interface_cfg.h"
 * is defined which FM controls which interface. This function is used
 * to release the subsription with the defined FM to the specified interface.
 *
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 * 
 * Normally the USB driver returns the message T_USB_FM_RESULT_MSG to indicate that 
 * unsubscription was succesfull or not
 */
/*@{*/
T_RV_RET usb_fm_unsubscribe(UINT8 interface_id);
/*@}*/

/**
 * @name usb_get_status
 *
 * this function must be called by a FM to get status info.
 *
 * The FM calls this function to retrieve status information about the endpoints
 * and the USB Hardware. The diver will respond to this function call by sending a 
 * T_RV_HDR type variable to the callback function that belongs with the requester FM 
 *
 * @param	T_USB_STATUS*:	status_p	pointer to status information storage space
 *									 	created by FM, filled by usb driver
 * @param	UINT8:			interface_id	holds the interface number as described 
 *											in "usb_interface_cfg.h"
 * @return	T_RV_RET:		RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_get_status(UINT8 interface_id, T_USB_STATUS* status_p);
/*@}*/

/**
 * @name usb_set_tx_buffer
 *
 * This function provides the usb driver with a buffer containing data to be send
 *
 * The driver splits the buffer in data packets as large as the available endpoints 
 * fifo
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint.
 * @param	UINT8:		endpoint		endpoint number 
 * @param   UINT8*:		buffer_p		pointer to the provided data buffer
 * @param   UINT16:		size			size of the provided buffer
 * @param   BOOL:		shorter_transfer	shorter transfer than expected by host
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_set_tx_buffer(UINT8 interface_id, UINT8 endpoint, UINT8* buffer_p, UINT16 size, 
			   BOOL shorter_transfer);
/*@}*/

/**
 * @name usb_set_rx_buffer
 *
 * This function provides the usb driver with a buffer into which the recieved data can be
 * be placed.
 *
 * The driver places all the data packets as large as the available endpoints into this buffer
 * as one large data packet
 * 
 * THIS FUNCTION MUST ALWAYS BE CALLED AFTER THE INTERACE HAS BEEN NOTIFIED OF RECIEVED DATA
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint.
 * @param	UINT8:		endpoint		endpoint number 
 * @param   UINT8*:		buffer_p		pointer to the provided data buffer
 * @param   UINT16:		size			size of the provided buffer
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_set_rx_buffer(UINT8 interface_id, UINT8 endpoint, UINT8* buffer_p, UINT16 size);
/*@}*/

/**
 * @name usb_reclaim_rx_buffer
 *
 * This function gives the FM back the control over the buffer.
 * The USB expects to get a new buffer. 
 * 
 * @param	UINT8:		interface_id	holds the interface number of the specified 
 *										endpoint.
 * @param	UINT8:		endpoint		endpoint number 
 *
 * @return	T_RV_RET:	RV_OK, RV_NOT_READY, RV_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET usb_reclaim_rx_buffer(UINT8 interface_id, UINT8 endpoint);
/*@}*/

/**
 * @name usb_get_hw_version
 *
 * this function must be called to get the USB hardware version
 *
 * This function is called this to retrieve hardware version information of the USB 
 * hardware.
 *
 * @return	UINT8:		b0-3 minor version number (4bits)
 *						b4-7 major version number (4bits)	
 */
/*@{*/
UINT8 usb_get_hw_version(void);
/*@}*/

/**
 * @name usb_get_sw_version
 *
 * this function must be called to get the USB software driver version
 *
 * This function is called this to retrieve software version information of the USB 
 * driver.
 *
 * @return	UINT32:		b0-15  build number			(8bits)
 *						b16-23 minor version number (8bits)
 *						b24-32 major version number (8bits)	
 */
/*@{*/
UINT32 usb_get_sw_version(void);
/*@}*/

void usb_con_int(void);
void usb_discon_int(void);


/*
	Following two functions are added for handling USBFAX functionality
*/
T_RV_RET usb_start_timer();
T_RV_RET usb_stop_timer();

#ifdef __cplusplus
}
#endif


#endif /*__USB_API_H_*/

