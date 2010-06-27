/**
 * @file
 *
 * @brief Definition of the Audio configuration functions.
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

#include "as/as_api.h"
#include "as/as_i.h"
#include "as/as_state_i.h"

#include "rvf/rvf_api.h"
#include "Audio/audio_api.h"


// Static function prototypes.

/// Callback function of the audio speaker mode setting.
static void
as_audio_chain_step_1_cb(T_AUDIO_FULL_ACCESS_WRITE_DONE* msg);

/// Callback function of the extra gain value setting.
static void
as_audio_chain_step_2_cb(T_AUDIO_FULL_ACCESS_WRITE_DONE* msg);


/// Volume for audio stereo speaker.
static const UINT8 as_audio_volume[] =
{
    AUDIO_STEREO_SPEAKER_VOLUME_MUTE, // Not used = SILENT.
    22, // AS_VOLUME_LOW
    10, // AS_VOLUME_MEDIUM
    0   // AS_VOLUME_HIGH
};


/**
 * Read the audio configuration.
 *
 * This function reads the following parameters of the audio
 * configuration:
 * - speaker_mode: whether the output device is the speaker or the headset.
 * - extra_gain:   extra gain value if the output is the speaker.
 * - volume_level: the volume used to play the melody.
 *
 * @noparam
 *
 * @retval AS_OK        The processing is successful.
 * @retval AS_AUDIO_ERR An error occurred in the Audio SWE.
 */
T_AS_RET
as_audio_fa_read(void)
{
    T_AUDIO_FULL_ACCESS_READ param;
    INT8 v_int8 = 0;

#if ( AS_OPTION_CHANGE_PATH == 1 )    

    // Set the parameters for full access read.
    param.variable_indentifier = AUDIO_STEREO_SPEAKER_MODE;
    param.data = &as_ctrl_blk->previous.speaker_mode;

    if (audio_full_access_read(&param) != AUDIO_OK)
    {
        // Error while reading audio configuration.
        AS_TRACE_0(ERROR, "audio_fa_read() audio_full_access_read(SPEAKER_MODE) error.");
        return AS_AUDIO_ERR;
    }
    AS_DEBUG_L(HIGH, "audio_fa_read() read SPEAKER_MODE =",
               as_ctrl_blk->previous.speaker_mode);

    // Set the parameters for full access read.
    param.variable_indentifier = AUDIO_SPEAKER_EXTRA_GAIN;
    param.data = &as_ctrl_blk->previous.extra_gain;

    if (audio_full_access_read(&param) != AUDIO_OK)
    {
        // Error while reading audio configuration.
        AS_TRACE_0(ERROR, "audio_fa_read() audio_full_access_read(SPEAKER_EXTRA_GAIN) error.");
        return AS_AUDIO_ERR;
    }
    AS_DEBUG_L(HIGH, "audio_fa_read() read SPEAKER_EXTRA_GAIN =",
               as_ctrl_blk->previous.extra_gain);
#endif
    
    // Set the parameters for full access read.
    param.variable_indentifier = AUDIO_STEREO_SPEAKER_VOLUME_LEVEL;
    param.data = &as_ctrl_blk->previous.volume;

    // Use the regular AS return path for the last request.
    if (audio_full_access_read(&param) != AUDIO_OK)
    {
        // Error while reading audio configuration.
        AS_TRACE_0(ERROR, "audio_fa_read() audio_full_access_read(STEREO_SPEAKER_VOLUME) error.");
        return (T_AS_RET)AS_AUDIO_ERR;
    }
    AS_DEBUG_2(HIGH, "audio_fa_read() read STEREO_SPEAKER_VOLUME L:%d R:%d.",
               as_ctrl_blk->previous.volume.audio_stereo_speaker_level_left,
               as_ctrl_blk->previous.volume.audio_stereo_speaker_level_right);

    return AS_OK;
}


/**
 * Write the audio configuration.
 *
 * This function writes the following parameters of the audio
 * configuration:
 * - speaker_mode: whether the output device is the speaker or the headset.
 * - extra_gain:   extra gain value if the output is the speaker.
 * - volume_level: the volume used to play the melody.
 *
 * This function starts the audio configuration chain, in that is sends
 * the speaker_mode new value and provides the callback function
 * that will continue the audio configuration with the extra_gain.
 *
 * This function can be used for both setting or restoring
 * the audio configuration. The chain will use the AS state to know if
 * this is a setting or a restoring operation.
 *
 * @param new_state @in The new AS state to set.
 *
 * @retval AS_OK        The processing is successful.
 * @retval AS_AUDIO_ERR An error occurred in the Audio full_access API.
 */
T_AS_RET
as_audio_fa_write(T_AS_STATE new_state)
{
    T_RV_RETURN_PATH rp;
    T_AUDIO_FULL_ACCESS_WRITE param;
    INT8 v_int8 = 0;

	#if ( AS_OPTION_CHANGE_PATH == 0 )
	T_AUDIO_FULL_ACCESS_WRITE_DONE full_access;
	#endif

    // Set the return path (callback function).
    rp.addr_id       = RVF_INVALID_ADDR_ID;
    rp.callback_func = (CALLBACK_FUNC) as_audio_chain_step_1_cb;

    // OK, transition: change to new state.
    AS_CHANGE_STATE(new_state);

#if ( AS_OPTION_CHANGE_PATH == 1 )
    // Set the parameters for full access write.
    param.variable_indentifier = AUDIO_STEREO_SPEAKER_MODE;
    param.data = &v_int8;

    // Either SETTING or RESTORING phase:
    if (as_ctrl_blk->state == AS_STATE_SETTING)
    {
        v_int8 = ((as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE) ||
                  (as_ctrl_blk->params.output_device == AS_DEVICE_SPEAKER))
                  #if (ANLG_FAM != 11)
            ? AUDIO_STEREO_SPEAKER_HANDHELD_8OHM
            #else
			? AUDIO_STEREO_SPEAKER_HANDHELD
			#endif
            : AUDIO_STEREO_SPEAKER_HEADPHONE;
    }
    else // AS_STATE_RESTORING
    {
        v_int8 = as_ctrl_blk->previous.speaker_mode;
    }

    if (audio_full_access_write(&param, rp) != AUDIO_OK)
    {
        // Error while configuring audio.
        AS_TRACE_0(ERROR, "audio_fa_write() audio_full_access_write() error.");
        return AS_AUDIO_ERR;
    }
    AS_DEBUG_L(HIGH, "audio_fa_write() send write SPEAKER_MODE =", v_int8);
	
#elif (AS_OPTION_CHANGE_PATH == 0)

	full_access.status = AUDIO_OK;
	as_audio_chain_step_2_cb( &full_access );

#endif
    return AS_OK;
}


/**
 * This private function performs the following operations:
 * - allocate an AUDIO_FULL_ACCESS_WRITE_DONE message buffer from
 *   the SWE memory bank.
 * - fill the message with error status.
 * - send the message to the AS SWE.
 *
 * @param msg @in The template Audio message used to get the Audio addr_id.
 *
 * @noreturn
 */
static void
as_audio_send_fake_error_faw_done(const T_AUDIO_FULL_ACCESS_WRITE_DONE* template)
{
    T_AUDIO_FULL_ACCESS_WRITE_DONE* msg;
    T_RVF_MB_STATUS mb_status;

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_IND),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "audio_send_fake_error_faw_done() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->os_hdr.msg_id       = AUDIO_FULL_ACCESS_WRITE_DONE;
        msg->os_hdr.src_addr_id  = template->os_hdr.src_addr_id;
        msg->os_hdr.dest_addr_id = as_ctrl_blk->addr_id;

        // Ringer start ind parameters.
        msg->status      = AUDIO_ERROR;

        // Send the message using the addess id.
        if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
        {
            AS_TRACE_0(ERROR, "audio_send_fake_error_faw_done() unable to send message.");
            rvf_free_buf(msg);
        }
        else
        {
            AS_TRACE_0(DEBUG_LOW,
                       "audio_send_fake_error_faw_done() successfully sent message.");
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "(audio_send_fake_error_faw_done) not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
}


/**
 * This function is called by the Audio SWE when it sends back the status of
 * the full_access_write on the speaker_mode.
 *
 * If the playback is a 'ringer', then the audio extra_gain will be set;
 * otherwise the callback of the extra_gain is called, faking the
 * reply of the Audio SWE.
 *
 * This function can be used for both setting or restoring
 * the audio configuration. The chain will use the AS state to know if
 * this is a setting or a restoring operation.
 *
 * Error case:
 *
 * If the status is not OK, or the next audio setting could not be sent,
 * a fake AUDIO_FULL_ACCESS_WRITE_DONE message is sent to
 * the AS with ERROR status so that it can manage the error.
 * If the state was RESTORING, a best effort is performed, and the next step
 * in the chain is called.
 *
 * @param msg @in The replied Audio message.
 *
 * @noreturn
 */
static void
as_audio_chain_step_1_cb(T_AUDIO_FULL_ACCESS_WRITE_DONE* msg)
{
#if ( AS_OPTION_CHANGE_PATH == 1 )
    AS_TRACE_L(DEBUG_HIGH, "audio_chain_step_1_cb() status =", msg->status);

    // Check operation status.
    // Following processing is always performed if in RESTORING state.
    if ((msg->status == AUDIO_OK) || (as_ctrl_blk->state == AS_STATE_RESTORING))
    {
        if ((as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE) ||
            (as_ctrl_blk->params.output_device == AS_DEVICE_SPEAKER))
        {
            T_RV_RETURN_PATH rp;
            T_AUDIO_FULL_ACCESS_WRITE param;
            INT8 v_int8 = 0;

            // Set the return path (callback function).
            rp.addr_id       = RVF_INVALID_ADDR_ID;
            rp.callback_func = (CALLBACK_FUNC) as_audio_chain_step_2_cb;

            // Set the parameters for full access write.
            param.variable_indentifier = AUDIO_SPEAKER_EXTRA_GAIN;
            param.data = &v_int8;

            // Either SETTING or RESTORING phase:
            if (as_ctrl_blk->state == AS_STATE_SETTING)
            {
                v_int8 = AUDIO_SPEAKER_SPK_GAIN_8_5dB;
            }
            else // AS_STATE_RESTORING
            {
                v_int8 = as_ctrl_blk->previous.extra_gain;
            }

            if (audio_full_access_write(&param, rp) != AUDIO_OK)
            {
                AS_TRACE_0(ERROR, "audio_chain_step_1_cb() audio_full_access_write() error.");

                if (as_ctrl_blk->state == AS_STATE_SETTING)
                {
                    // Setting: need to abort by sending a fake error audio message to AS.
                    as_audio_send_fake_error_faw_done(msg);
                }
                else
                {
                    // Restoring: best effort, go to next step in chain.
                    as_audio_chain_step_2_cb(msg);
                }
            }
            else
            {
                AS_DEBUG_L(HIGH, "audio_chain_step_1_cb() send write SPEAKER_EXTRA_GAIN =",
                           v_int8);
            }
        }
        else
        {
            AS_DEBUG_0(HIGH, "audio_chain_step_1_cb() skip SPEAKER_EXTRA_GAIN setting.");
            // Skip the speaker extra_gain setting.
            // Act as if extra_gain setting is already done.
            as_audio_chain_step_2_cb(msg);
        }
    }
    else
    {
        AS_TRACE_0(ERROR, "audio_chain_step_1_cb() bad status");

        // Send a fake error audio message to AS.
        as_audio_send_fake_error_faw_done(msg);
    }
#endif
}


/**
 * This function is called by the Audio SWE when it sends back the status of
 * the full_access_write on the extra_gain.
 *
 * It can be used for both setting or restoring
 * the audio configuration. The chain will use the AS state to know if
 * this is a setting or a restoring operation.
 *
 * Error case:
 *
 * If the status is not OK, or the next audio setting could not be sent,
 * a fake AUDIO_FULL_ACCESS_WRITE_DONE message is sent to
 * the AS with ERROR status so that it can manage the error.
 * If the state was RESTORING, a best effort is performed, and the next step
 * in the chain is called.
 *
 * @param msg @in The replied Audio message.
 *
 * @noreturn
 */
static void
as_audio_chain_step_2_cb(T_AUDIO_FULL_ACCESS_WRITE_DONE* msg)
{
    T_AS_RET res;

    AS_TRACE_L(DEBUG_HIGH, "audio_chain_step_2_cb() status =", msg->status);

    // Check operation status.
    if (msg->status == AUDIO_OK)
    {
        T_AUDIO_FULL_ACCESS_WRITE param;
        T_AUDIO_STEREO_SPEAKER_LEVEL volume;

        // Set the parameters for full access write.
        param.variable_indentifier = AUDIO_STEREO_SPEAKER_VOLUME_LEVEL;
        param.data = &volume;

        // Either SETTING or RESTORING phase:
        if (as_ctrl_blk->state == AS_STATE_SETTING)
        {
            volume.audio_stereo_speaker_level_left = as_audio_volume[as_ctrl_blk->volume];
            volume.audio_stereo_speaker_level_right = volume.audio_stereo_speaker_level_left;
        }
        else // AS_STATE_RESTORING
        {
            volume = as_ctrl_blk->previous.volume;
        }

        // Use the regular AS return path for the last request.
        if (audio_full_access_write(&param, as_ctrl_blk->rp) != AUDIO_OK)
        {
            AS_TRACE_0(ERROR, "audio_chain_step_2_cb() audio_full_access_write() error.");

            // Setting/Restoring: need to send a fake error audio message to AS.
            as_audio_send_fake_error_faw_done(msg);
        }
        else
        {
            AS_DEBUG_2(HIGH,
                       "audio_chain_step_2_cb() send write STEREO_SPEAKER_VOLUME L:%d R:%d.",
                       volume.audio_stereo_speaker_level_left,
                       volume.audio_stereo_speaker_level_right);
        }
    }
    else
    {
        AS_TRACE_0(ERROR, "audio_chain_step_2_cb() bad status");

        // Send a fake error audio message to AS.
        as_audio_send_fake_error_faw_done(msg);
    }
}
