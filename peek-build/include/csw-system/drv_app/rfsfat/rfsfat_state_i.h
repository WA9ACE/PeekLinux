/**
 * @file	rfsfat_state_i.h
 * 
 * Internal definitions for RFSFAT state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in rfsfat_message.h, since they can 
 * be seen from outside.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef __RFSFAT_STATE_I_H_
#define __RFSFAT_STATE_I_H_


#include "rv/rv_general.h"


/**
 * RFSFAT states.
 *
 * RFSFAT instance is a state machine with several possible states.
 * Based on the current state, RFSFAT handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
  RFSFAT_UNINITIALISED,
  RFSFAT_INITIALISED,
  RFSFAT_CONFIGURED,
  RFSFAT_UNMOUNTED,
  RFSFAT_IDLE,
  RFSFAT_STOPPED,
  RFSFAT_KILLED
} T_RFSFAT_INTERNAL_STATE;


/*
 * Optional but	recommanded, define a function for the processing
 * in each state, like:
 *   T_RFSFAT_RETURN rfsfat_state_1					(T_RV_HDR * message_p);
 *   T_RFSFAT_RETURN rfsfat_state_2					(T_RV_HDR * message_p);
 */

#endif /* __RFSFAT_STATE_I_H_ */
