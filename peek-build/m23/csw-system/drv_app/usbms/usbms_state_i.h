/**
 * @file    usbms_state_i.h
 *
 * Internal definitions for USBMS state machine,
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in usbms_message.h, since they can
 * be seen from outside.
 *
 * @author  Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  3/19/2004   Virgile COULANGE        Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBMS_STATE_I_H_
#define __USBMS_STATE_I_H_


#include "rv/rv_general.h"


/**
 * USBMS states.
 *
 * USBMS instance is a state machine with several possible states.
 * Based on the current state, USBMS handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
    INIT,
    UNPLUGGED,
    IDLE,
    USB_SEND_NO_GBI,
    GET_FROM_USB,
    GET_FROM_GBI,
    SEND_TO_USB,
    SEND_TO_GBI,
    SBW_PROCESS,
    RESET_IN_PROGRESS,
    REINIT_USB,
    REINIT_GBI,
    SEND_TO_USB_GET_FROM_GBI
} T_USBMS_INTERNAL_STATE;


/*
 * Optional but recommanded, define a function for the processing
 * in each state, like:
 *   T_USBMS_RETURN usbms_state_1                   (T_RV_HDR * message_p);
 *   T_USBMS_RETURN usbms_state_2                   (T_RV_HDR * message_p);
 */

T_USBMS_RETURN usbms_init_state(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_unplugged(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_reset_in_progress(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_idle(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_usb_send_no_gbi(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_get_from_usb(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_get_from_gbi(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_send_to_usb(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_send_to_gbi(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_sbw_process(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_reinit_usb(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_restart(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_reinit_gbi(T_RV_HDR *message_p);
T_USBMS_RETURN usbms_send_to_usb_get_from_gbi(T_RV_HDR *message_p);

#endif /* __USBMS_STATE_I_H_ */
