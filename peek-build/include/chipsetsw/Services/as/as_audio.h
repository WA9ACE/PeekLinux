/**
 * @file
 *
 * @brief AUDIO processing functions for AS SWE.
 *
 * This file gathers all the different AUDIO processing functions.
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

#ifndef __AS_AUDIO_H_
#define __AS_AUDIO_H_


#include "as/as_i.h"
#include "as/as_api.h"


/// Read the audio configuration (audio path and volume).
T_AS_RET
as_audio_fa_read(void);

/// Write the audio configuration (starts the audio config chain).
extern T_AS_RET
as_audio_fa_write(T_AS_STATE new_state);


#endif // __AS_AUDIO_H_
