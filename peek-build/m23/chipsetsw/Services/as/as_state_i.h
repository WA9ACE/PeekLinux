/**
 * @file
 *
 * @brief Internal declarations for AS state machine.
 *
 * This file contains declaration of the different states and associated
 * functions of the AS SWE.
 *
 * Messages are declared in as_message.h, since they can be seen from outside.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_STATE_I_H_
#define __AS_STATE_I_H_


#include "as/as_i.h"
#include "rv/rv_general.h"


/**
 * @brief AS states.
 *
 * AS SWE is a state machine with several possible states.
 * Based on the current state, AS handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef INT8 T_AS_STATE;

/// Initial state, used during the whole initialization phase.
#define AS_STATE_INIT      0
/// The AS is ready to process a start request.
#define AS_STATE_IDLE      1
/// The AS is waiting for an AUDIO_FULL_ACCESS_WRITE_DONE message.
#define AS_STATE_SETTING   2
/// Not yet used, future use with MP3 player.
#define AS_STATE_STARTING  3
/// The AS is playing a melody. Ready to accept a stop request.
#define AS_STATE_PLAYING   4
/// Not yet used, future use with MP3 player.
#define AS_STATE_STOPPING  5
/// The AS is waiting for an AUDIO_FULL_ACCESS_WRITE_DONE message.
#define AS_STATE_RESTORING 6
/// The AS is playing a melody.  pause request comes.
#define AS_STATE_PAUSED 7


/// Convenient macro to keep trace of changed state.
#define AS_CHANGE_STATE(_state) \
    AS_DEBUG_2(HIGH, "STATE change from current state %d to new state %d", as_ctrl_blk->state, _state); \
    as_ctrl_blk->state = _state


#endif // __AS_STATE_I_H_
