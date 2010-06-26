/**
 * @file
 *
 * @brief MIDI processing functions for AS SWE.
 *
 * This file gathers all the different MIDI processing functions.
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

#ifndef __AS_MIDI_H_
#define __AS_MIDI_H_


#include "as/as_api.h"
#include "as/as_message.h"


/// Set the maximum number of voice to play simultaneously.
extern T_AS_RET
as_midi_set_voice_limit(void);

/// Prepare the player and make it ready to be started.
extern T_AS_RET
as_midi_prepare_setting(const T_AS_START_REQ* msg);


#ifdef AS_OPTION_MIDI
// progress bar callback
static void
as_midi_position_cb(void* client_data,
                    mbTime requestedPos,
                    mbTime actualPos );
#endif 

/// Start the player.
T_AS_RET
as_midi_start_player(void);

/// Pause the player.
T_AS_RET
as_midi_pause_player(void);

/// Resume the player.
T_AS_RET
as_midi_resume_player(void);
/// forward or rewind the player.
T_AS_RET
as_midi_forrew_player(void);


/// Stop the player.
T_AS_RET
as_midi_stop_player(void);

void
as_terminated_l1_conf_cb();

T_AS_RET
as_stopping_do_midi_bae_ind();

#endif // __AS_MIDI_H_
