/**
 * @file
 *
 * @brief Message processing functions for AS SWE.
 *
 * This file gathers all the different message processing functions
 * according to AS SWE state.
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

#ifndef __AS_PROCESSING_H_
#define __AS_PROCESSING_H_


#include "as/as_message.h"

#include "audio/audio_api.h"

/// Save the passed START request @a msg for future processing.
extern void
as_request_save(const T_AS_START_REQ* msg);

/// Restore a saved START request.
extern const T_AS_START_REQ*
as_request_restore(void);


/// Perform common processing before to change to IDLE state.
extern void
as_process_to_idle_state(T_AS_RET status, BOOLEAN end_of_media);

/// Process the START_REQ message from IDLE state.
extern void
as_idle_do_start_req(const T_AS_START_REQ* msg);


/// Process the AUDIO_FULL_ACCESS_WRITE_DONE message from SETTING state.
extern void
as_setting_do_fa_write_done(const T_AUDIO_FULL_ACCESS_WRITE_DONE* msg);


/// Perform common processing before to change to PLAYING state.
extern void
as_process_to_playing_state(T_AS_RET status);

/// Perform the real stop processing.
extern void
as_playing_process_stop(void);

/// Process the PAUSE_REQ message from PLAYING  state
extern void
as_playing_to_pause_state(const T_AS_PAUSE_REQ* msg);

/// Process the RESUME_REQ message from PAUSE  state
extern void
as_pause_do_resume_req(const T_AS_RESUME_REQ* msg);

/// Process the STOP_REQ message from PAUSE state
extern void
as_pause_do_stop_req(const T_AS_STOP_REQ* msg);

/// Process the STOP_REQ message from PLAYING state.
extern void
as_playing_do_stop_req(const T_AS_STOP_REQ* msg);

/// Process the START_REQ message from PLAYING state.
extern void
as_playing_do_start_req(const T_AS_START_REQ* msg);

extern void
as_playing_do_forrew_req(const T_AS_FORREW_REQ* msg);

/// Process the START_REQ message from RESTORING/STOPPING state during internal stop.
extern void
as_internal_stop_do_start_req(const T_AS_START_REQ* msg);

/// Process the PAUSE_REQ message from RESTORING/STOPPING state during internal stop.
extern void
as_internal_stop_do_pause_req(const T_AS_START_REQ* msg);


/// Process the AUDIO_FULL_ACCESS_WRITE_DONE message from RESTORING state.
extern void
as_restoring_do_fa_write_done(const T_AUDIO_FULL_ACCESS_WRITE_DONE* msg);

//Process the BT_CFG_REQ message
extern void
as_do_bt_cfg_req(const T_AS_BT_CFG_REQ * msg);

#endif // __AS_PROCESSING_H_
