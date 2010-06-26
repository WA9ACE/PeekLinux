/**
 * @file	gbi_state_i.h
 * 
 * Internal definitions for GBI state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in gbi_message.h, since they can 
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
 *	12/29/2003	 ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_STATE_I_H_
#define __GBI_STATE_I_H_

/**
 * GBI states.
 *
 * GBI instance is a state machine with several possible states.
 * Based on the current state, GBI handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */

typedef enum
{
	GBI_UNINITIALISED,
	GBI_INITIALISED,
	GBI_IDLE,
	GBI_OPERATIONAL,
	GBI_STOPPED,
	GBI_KILLED
} T_GBI_INTERNAL_STATE;


#endif /* __GBI_STATE_I_H_ */
