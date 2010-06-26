/**
 * @file	usb_state_i.h
 * 
 * Internal definitions for USB state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in usb_message.h, since they can 
 * be seen from outside.
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
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __USB_STATE_I_H_
#define __USB_STATE_I_H_


#include "rv/rv_general.h"


/**
 * USB states.
 *
 * USB instance is a state machine with several possible states.
 * Based on the current state, USB handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
	USB_UNINITIALISED,
	USB_INITIALISING,
	USB_IDLE,
	USB_STOPPED,
	USB_KILLED,
	USB_HANDLE_INTERRUPT,
	USB_HANDLE_REQUEST
} T_USB_INTERNAL_STATE;

/**
 * USB HW states.
 *
 * USB instance is a state machine with several possible states.
 * Based on the current state, USB Hardware is allowed to process specific acions.
 * States are listed in the order they should normally be sequenced.
 *
 */
typedef enum
{
	USB_HW_UNATTACHED,
	USB_HW_ATTACHED,
	USB_HW_DEFAULT,
	USB_HW_SUSPENDED,
	USB_HW_ADRESSED,
	USB_HW_CONFIGURED
} T_USB_HW_STATE;

/*
 * Optional but	recommanded, define a function for the processing
 * in each state, like:
 *   T_USB_RETURN usb_state_1					(T_RV_HDR * message_p);
 *   T_USB_RETURN usb_state_2					(T_RV_HDR * message_p);
 */

#endif /* __USB_STATE_I_H_ */
