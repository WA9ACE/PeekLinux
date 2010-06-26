/**
 * @file
 *
 * @brief MP3 processing functions for AS SWE.
 *
 * This file gathers all the different MP3 processing functions.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	10/24/2005	ramanujam@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_MP3_H_
#define __AS_MP3_H_


#include "as/as_api.h"
#include "as/as_message.h"


/// Prepare audio configuration.
T_AS_RET
as_mp3_prepare_setting(const T_AS_START_REQ* msg);

/// Start the Mp3.
T_AS_RET
as_mp3_start_player(void);

/// Pause the Mp3.
T_AS_RET
as_mp3_pause_player(void);

/// Resume the Mp3.
T_AS_RET
as_mp3_resume_player(void);

/// forward or rewind the player.
T_AS_RET
as_mp3_forrew_player(void);


/// Stops the Mp3 playing.
T_AS_RET
as_mp3_stop_player(void);

/// Start the player.
T_AS_RET
as_starting_do_mp3_status(T_AUDIO_MP3_STATUS* msg);

T_AS_RET
as_stopping_do_mp3_status( T_AUDIO_MP3_STATUS* msg);

T_AS_RET
as_playing_do_mp3_complete(T_AUDIO_MP3_STATUS* msg);

void
as_send_probar_mp3_ind(T_AUDIO_MP3_STATUS_BAR* pro_bar, BOOLEAN interrupted,const T_RV_RETURN_PATH* rp);

#endif // __AS_MIDI_H_
