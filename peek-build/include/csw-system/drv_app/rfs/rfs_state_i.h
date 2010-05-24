/**
 * @file	rfs_state_i.h
 * 
 * Internal definitions for RFS state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in rfs_message.h, since they can 
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
 *	1/23/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __RFS_STATE_I_H_
#define __RFS_STATE_I_H_


#include "rv/rv_general.h"


/**
 * RFS states.
 *
 * RFS instance is a state machine with several possible states.
 * Based on the current state, RFS handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
	RFS_UNINITIALISED,
	RFS_INITIALISED,
	RFS_IDLE,	
	RFS_OPERATIONAL,
	RFS_STOPPED,
	RFS_KILLED
} T_RFS_INTERNAL_STATE;


#endif /* __RFS_STATE_I_H_ */
