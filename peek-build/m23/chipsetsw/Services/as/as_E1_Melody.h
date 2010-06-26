/**
 * @file
 *
 * @brief E1-Melody processing functions for AS SWE.
 *
 * This file gathers all the different E1-Melody processing functions.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	12/12/2006							Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_E1_MELODY_H_
#define __AS_E1_MELODY_H_


#include "as/as_api.h"
#include "as/as_message.h"


/// Prepare audio configuration.
T_AS_RET
as_e1_melody_prepare_setting(const T_AS_START_REQ* msg);

/// Start the Mp3.
T_AS_RET
as_e1_melody_start_player(void);

/// Stops the Mp3 playing.
T_AS_RET
as_e1_melody_stop_player(void);

/// Start the player.
T_AS_RET
as_starting_do_e1_melody_status(T_AUDIO_MELODY_E1_STATUS* msg);

T_AS_RET
as_stopping_do_e1_melody_status( T_AUDIO_MELODY_E1_STATUS* msg);

T_AS_RET
as_playing_do_e1_melody_complete(T_AUDIO_MELODY_E1_STATUS* msg);


#endif // __AS_E1_MELODY_H_
