/**
 * @file	cry_state_i.h
 * 
 * Internal definitions for CRY state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in cry_message.h, since they can 
 * be seen from outside.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/14/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __CRY_STATE_I_H_
#define __CRY_STATE_I_H_


#include "rv/rv_general.h"


/**
 * CRY states.
 *
 * CRY instance is a state machine with several possible states.
 * Based on the current state, CRY handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
	CRY_STATE_1,
	CRY_STATE_2
} T_CRY_INTERNAL_STATE;


/*
 * Optional but	recommanded, define a function for the processing
 * in each state, like:
 *   T_CRY_RETURN cry_state_1					(T_RV_HDR * message_p);
 *   T_CRY_RETURN cry_state_2					(T_RV_HDR * message_p);
 */

#endif /* __CRY_STATE_I_H_ */
