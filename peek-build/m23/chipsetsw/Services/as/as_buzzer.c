/**
 * @file
 *
 * @brief Definition of the Buzzer player management functions.
 *
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "as/as_i.h"


#if (AS_OPTION_BUZZER == 1)

#include "as/as_buzzer.h"
#include "as/as_utils.h"
#include "as/as_processing.h"

#include "sys_types.h"
#include "buzzer/buzzer.h"

#include "rvf/rvf_api.h"
#include "rvf/rvf_target.h"


/// Counter value to simulate a continuous buzzer player.
#define AS_BUZZER_COUNTER_CONTINUOUS 65535

/// Counter value to simulate a single shot buzzer player (two bips).
#define AS_BUZZER_COUNTER_SINGLE_SHOT 2

/// Buzzer frequency generation (in unit*500/58=Hz).
#define AS_BUZZER_FREQUENCY 51 // == 440 Hz

/// Duration of enable and disable time (in ms).
#define AS_BUZZER_DURATION 2000 // = 2s

/// Volume for buzzer.
static const UINT8 as_buzzer_volume[] =
{
    0,  // Not used = SILENT.
    7,  // AS_VOLUME_LOW
    21, // AS_VOLUME_MEDIUM
    63  // AS_VOLUME_HIGH
};


/**
 * Start the buzzer player.
 *
 * This function performs the following processing to initialize
 * the buzzer player:
 * - Create the timer that allows to generate the intermittent tones.
 * - Set the correct buzzer attributes and enable the buzzer output.
 *
 * @param msg @in The start request from the AS client.
 *
 * @retval AS_OK           The processing is successful.
 * @retval AS_INTERNAL_ERR An error occurred while creating the timer.
 */
T_AS_RET
as_buzzer_start_player(const T_AS_START_REQ* msg)
{
    T_AS_RET rc = AS_OK;

    // Create a timer with automatic re-arm.
    as_ctrl_blk->buzzer.toggle = TRUE;
    as_ctrl_blk->buzzer.counter = (msg->loop)
        ? AS_BUZZER_COUNTER_CONTINUOUS
        : AS_BUZZER_COUNTER_SINGLE_SHOT;

    as_ctrl_blk->buzzer.tm_id = rvf_create_timer(as_ctrl_blk->addr_id,
                                                 RVF_MS_TO_TICKS(AS_BUZZER_DURATION),
                                                 TRUE,
                                                 NULL);
    // Timer expiration is processed in as_buzzer_expired()

    if (as_ctrl_blk->buzzer.tm_id != NULL)
    {
        AS_DEBUG_L(HIGH, "buzzer_start_player() timer created =",
                   as_ctrl_blk->buzzer.tm_id);

        // Set the buzzer parameters and start the generation.
        BZ_Tone(AS_BUZZER_FREQUENCY);
        BZ_Volume(as_buzzer_volume[msg->volume]);
        BZ_Enable();

        AS_DEBUG_0(HIGH, "buzzer_start_player() buzzer enabled.");

        // Change to PLAYING state.
        as_process_to_playing_state(AS_OK);
    }
    else
    {
        AS_TRACE_0(ERROR, "buzzer_start_player() unable to create timer.");
        rc = AS_INTERNAL_ERR;
    }

    return rc;
}


/**
 * Called each time the buzzer timer expires.
 *
 * This function performs the following processing to toggle the
 * buzzer output generation.
 * - Toggle the buzzer output (enable/disable).
 * - In case of a single shot timer (not looping), decrements the number of
 *   intermittent periods and if reaches 0, stops the buzzer.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_buzzer_timer_expired(void)
{
    // Timer is the buzzer timer.
    if (as_ctrl_blk->buzzer.toggle)
    {
        as_ctrl_blk->buzzer.toggle = FALSE;
        BZ_Disable();

        AS_DEBUG_0(HIGH, "buzzer_timer_expired() buzzer disabled.");

        if (as_ctrl_blk->buzzer.counter != AS_BUZZER_COUNTER_CONTINUOUS)
        {
            // Check the counter.
            if (as_ctrl_blk->buzzer.counter <= 1)
            {
                // Stop the buzzer player. End of media is true.
                (void) as_buzzer_stop_player(AS_OK, TRUE);
            }
            else
            {
                // Decrement the counter.
                as_ctrl_blk->buzzer.counter--;
            }
        }
    }
    else
    {
        as_ctrl_blk->buzzer.toggle = TRUE;
        BZ_Enable();

        AS_DEBUG_0(HIGH, "buzzer_timer_expired() buzzer enabled.");
    }
    // Timer is automatically re-armed.
}


/**
 * Stop the buzzer player.
 *
 * This function performs the following processing to terminate
 * the buzzer player:
 * - Delete the timer.
 * - Disable the buzzer output.
 *
 * @param status       @in The status to reply in the STOP_IND message.
 * @param end_of_media @in The end of media flag to reply in STOP_IND.
 *
 * @retval AS_OK The processing is always successful.
 */
T_AS_RET
as_buzzer_stop_player(T_AS_RET status, BOOLEAN end_of_media)
{
    AS_DEBUG_L(HIGH, "buzzer_stop_player() stopping timer =",
               as_ctrl_blk->buzzer.tm_id);

    // Delete the buzzer timer.
    if (as_ctrl_blk->buzzer.tm_id != NULL)
    {
        rvf_del_timer(as_ctrl_blk->buzzer.tm_id);
    }
    BZ_Disable();

    AS_DEBUG_0(HIGH, "buzzer_stop_player() buzzer disabled.");

    // Change to IDLE state.
    as_process_to_idle_state(status, end_of_media);

    return AS_OK;
}

#endif // AS_OPTION_BUZZER
