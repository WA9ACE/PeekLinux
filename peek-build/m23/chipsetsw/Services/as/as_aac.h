/**
 * @file
 *
 * @brief AAC processing functions for AS SWE.
 *
 * This file gathers all the different AAC processing functions.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	04/03/2006	ramanujam@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_AAC_H_
#define __AS_AAC_H_


#include "as/as_api.h"
#include "as/as_message.h"


/// Prepare audio configuration.
T_AS_RET
as_aac_prepare_setting(const T_AS_START_REQ* msg);

/// Start the AAC.
T_AS_RET
as_aac_start_player(void);

/// Pause the AAC
T_AS_RET
as_aac_pause_player(void);

/// Resume the AAC 
T_AS_RET
as_aac_resume_player(void);

/// Stops the AAC playing.
T_AS_RET
as_aac_stop_player(void);

/// Start the player.
T_AS_RET
as_starting_do_aac_status(T_AUDIO_AAC_STATUS* msg);

T_AS_RET
as_stopping_do_aac_status( T_AUDIO_AAC_STATUS* msg);

T_AS_RET
as_playing_do_aac_complete(T_AUDIO_AAC_STATUS* msg);


void
as_send_probar_aac_ind(T_AUDIO_AAC_STATUS_BAR* pro_bar, BOOLEAN interrupted,const T_RV_RETURN_PATH* rp);

#endif // __AS_AAC_H_
