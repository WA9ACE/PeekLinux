/**
 * @file	hash_state_i.h
 * 
 * Internal definitions for SHA state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in hash_message.h, since they can 
 * be seen from outside.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_STATE_I_H_
#define __HASH_STATE_I_H_


#include "rv/rv_general.h"


/**
 * SHA states.
 *
 * SHA instance is a state machine with several possible states.
 * Based on the current state, SHA handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
  HASH_STATE_1,
  HASH_STATE_2
}
T_HASH_INTERNAL_STATE;


/*
 * Optional but	recommanded, define a function for the processing
 * in each state, like:
 *   T_HASH_RETURN hash_state_1					(T_RV_HDR * message_p);
 *   T_HASH_RETURN hash_state_2					(T_RV_HDR * message_p);
 */

#endif /* __HASH_STATE_I_H_ */
