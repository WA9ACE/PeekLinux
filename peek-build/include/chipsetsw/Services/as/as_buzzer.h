/**
 * @file
 *
 * @brief BUZZER processing functions for AS SWE.
 *
 * This file gathers all the different BUZZER processing functions.
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

#ifndef __AS_BUZZER_H_
#define __AS_BUZZER_H_


#include "as/as_i.h"


#if (AS_OPTION_BUZZER == 1)

#include "as/as_api.h"
#include "as/as_message.h"


/// Start the buzzer player.
extern T_AS_RET
as_buzzer_start_player(const T_AS_START_REQ* msg);

/// The internal buzzer timer expired, toggle buzzer output generation.
extern void
as_buzzer_timer_expired(void);

/// Stop the buzzer player.
extern T_AS_RET
as_buzzer_stop_player(T_AS_RET status, BOOLEAN end_of_media);

#endif // AS_OPTION_BUZZER


#endif // __AS_BUZZER_H_
