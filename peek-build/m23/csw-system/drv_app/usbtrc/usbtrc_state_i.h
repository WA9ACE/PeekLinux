/**
 * @file
 *
 * @brief Internal declarations for USBTRC state machine.
 *
 * This file contains declaration of the different states and associated
 * functions of the USBTRC SWE.
 *
 * Messages are declared in usbtrc_message.h, since they can be seen from outside.
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

#ifndef __USBTRC_STATE_I_H_
#define __USBTRC_STATE_I_H_


#include "rv/rv_general.h"


/**
 * @brief USBTRC states.
 *
 * USBTRC instance is a state machine with several possible states.
 * Based on the current state, USBTRC handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
	USBTRC_INIT_STATE,
	USBTRC_DISCONNECTED_STATE,
	USBTRC_IDLE_STATE
} T_USBTRC_INTERNAL_STATE;


/*
 * Optional but	recommended, define a function for the processing
 * in each state, like:
 *   T_USBTRC_RETURN usbtrc_state_1					(T_RV_HDR * message_p);
 *   T_USBTRC_RETURN usbtrc_state_2					(T_RV_HDR * message_p);
 */
T_RVM_RETURN usbtrc_init_state (T_RV_HDR *msg_p);
T_RVM_RETURN usbtrc_disconnected_state (T_RV_HDR *msg_p);
T_RVM_RETURN usbtrc_idle_state (T_RV_HDR *msg_p);

#endif // __USBTRC_STATE_I_H_
