/*
 * @file
 *
 * @brief Public API Implementation for AS SWE.
 *
 * This file initializes AS global variables and implements AS functions.
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/10/2005	f-maria@ti.com		Create.
 *	15/06/2006      padmanabhav@ti.com      Added support for .mxmf extension
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "as/as_api.h"
#include "as/as_i.h"
#include "as/as_message.h"
#include "as/as_utils.h"

#include "rvf/rvf_api.h"

#if(AS_RFS_API == 1)
#include "rfs/rfs_api.h"
#endif

#include <string.h>


/**
 * Tell the audio player whether there is an active GPRS connection.
 *
 * This function has an immediate effect.
 * Indeed, if a GPRS connection is active,
 * the AS will reduce its requirements in term of CPU consumption.
 *
 * @param gprs_mode @in Whether a GPRS connection is active (TRUE)
 *                      or not (FALSE).
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_PLAYER_ERR  It was impossible to change the player
 *                        parameters.
 *
 * @note The immediate effect is only audible if a midi player is
 *       currently running.
 */
T_AS_RET
as_set_gprs_mode(BOOLEAN gprs_mode)
{
    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "set_gprs_mode() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, set the value.
    as_ctrl_blk->gprs_mode = gprs_mode;

    // If a player is active, try to dynamically adjust it.
    return as_adapt_gprs_mode();
}


/**
 * Deduce the player type from the given filename.
 *
 * The player type is deduced according to the filename extension.
 * Recognized extension are:
 * - PLAYER_TYPE_MIDI .mid .imy .xmf .mmf .mxmf
 *
 * @param filename    @in  The name of the file to deduce the type.
 * @param player_type @out The deduced type of player.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_SUPPORTED  The filename extension is not supported.
 * @retval AS_INVALID_PARAM  The @a player_type or the @a filename parameter
 *                           is NULL.
 */
#if(AS_RFS_API == 1)
T_AS_RET
as_deduce_player_type(const T_WCHAR*       filename,
                      T_AS_PLAYER_TYPE* player_type)
#else
T_AS_RET
as_deduce_player_type(const char*       filename,
                      T_AS_PLAYER_TYPE* player_type)
#endif
{
    INT16 len =0;
    INT16 end = 0;
    INT16 i = 0;
    BOOLEAN found = FALSE;
#if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[8];
		T_WCHAR mp_uc2[8];
		T_WCHAR mp_uc3[8];
		T_WCHAR mp_uc4[8];
		T_WCHAR mp_uc5[8];
		T_WCHAR mp_uc6[8];
		T_WCHAR mp_uc7[8];
		T_WCHAR mp_uc8[8];
		T_WCHAR mp_uc9[8];
		T_WCHAR mp_uc20[8];
		T_WCHAR dotp[1];

		char *str1 = "mid";
		char *str2 = "imy";
		char *str3 = "xmf";
		char *str4 = "mmf";
		char *str5 = "mxmf";
		char *str6 = "wav";
		char *str7 = "sms";
		char *str8 = "mp3";
		char *str9 = "aac";
		char *str20 = "e1";
		char *dot    = ".";
		


#endif
#if(AS_RFS_API == 1)
		convert_u8_to_unicode(str1, mp_uc1);
		convert_u8_to_unicode(str2, mp_uc2);
		convert_u8_to_unicode(str3, mp_uc3);
		convert_u8_to_unicode(str4, mp_uc4);
		convert_u8_to_unicode(str5, mp_uc5);
		convert_u8_to_unicode(str6, mp_uc6);
		convert_u8_to_unicode(str7, mp_uc7);
		convert_u8_to_unicode(str8, mp_uc8);
		convert_u8_to_unicode(str9, mp_uc9);
		convert_u8_to_unicode(str20, mp_uc20);
		convert_u8_to_unicode(dot, dotp);

#endif


    if (player_type == NULL)
    {
        AS_TRACE_0(ERROR,
                   "deduce_player_type() invalid parameter, player_type NULL");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if (filename == NULL)
    {
        AS_TRACE_0(ERROR,
                   "deduce_player_type() invalid parameter, filename NULL.");
        return(T_AS_RET) AS_INVALID_PARAM;
    }

#if(AS_RFS_API == 1)
	len = wstrlen(filename);
#else
	len = strlen(filename);
#endif
    end = (len >= 5) ? len - 5 : 0;

    // Find the extension.
    for (i = (len - 1); i >= end; i--)
    {
    
#if(AS_RFS_API == 1)
		if (filename[i] == dotp[0])
#else
        if (filename[i] == '.')
#endif
        {
            found = TRUE;
            // Skip '.' character.
            i++;

#if(AS_RFS_API == 1)
			if ((wstrncmp(filename + i, mp_uc1,3) == 0) ||
                (wstrncmp(filename + i, mp_uc2,3) == 0) ||
                (wstrncmp(filename + i, mp_uc3,3) == 0) ||
                (wstrncmp(filename + i, mp_uc4,3) == 0) ||
                (wstrcmp(filename + i, mp_uc5) == 0)  ||
                (wstrncmp(filename + i, mp_uc6,3) == 0) ||
                (wstrncmp(filename + i, mp_uc7,3) == 0) )
#else
            if ((strncmp(filename + i, "mid",3) == 0) ||
                (strncmp(filename + i, "imy",3) == 0) ||
                (strncmp(filename + i, "xmf",3) == 0) ||
                (strncmp(filename + i, "mmf",3) == 0) ||
                (strcmp(filename + i, "mxmf") == 0)  ||
                (strncmp(filename + i, "wav",3) == 0) ||
                (strncmp(filename + i, "sms",3) == 0) )
#endif

            {
                *player_type = AS_PLAYER_TYPE_MIDI;
            }
	#if (AS_OPTION_MP3 == 1)
#if(AS_RFS_API == 1)
			else if (wstrncmp(filename + i, mp_uc8,3) == 0)
#else
            else if (strncmp(filename + i, "mp3",3) == 0)
#endif
            {
                *player_type = AS_PLAYER_TYPE_MP3;
            }
	#endif
	#if (AS_OPTION_AAC == 1)
#if(AS_RFS_API == 1)
			else if (wstrncmp(filename + i, mp_uc9,3) == 0)
#else
            else if (strncmp(filename + i, "aac",3) == 0)
#endif
            {
                *player_type = AS_PLAYER_TYPE_AAC;
            }
	#endif

       #if (AS_OPTION_E1_MELODY == 1)
#if(AS_RFS_API == 1)
			else if (wstrncmp(filename + i, mp_uc20,3) == 0)
#else
            else if (strncmp(filename + i, "e1",3) == 0)
#endif
            {
                *player_type = AS_PLAYER_TYPE_E1_MELODY;
            }
	#endif

	    else
            {
                AS_TRACE_1(ERROR,
                           "deduce_player_type() extension not supported, file '%s'.",
                           filename);
                return (T_AS_RET)AS_NOT_SUPPORTED;
            }
            break;
        }
    }
    if (!found)
    {
        AS_TRACE_1(ERROR,
                   "deduce_player_type() invalid parameter, file '%s'.",
                   filename);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    return AS_OK;
}


/**
 * Set the specific parameters for the player of given player_type.
 *
 * The @a player_type allows to set the parameters for the following players:
 * - @b PLAYER_TYPE_MIDI Midi player parameters.
 * - @b PLAYER_TYPE_COMMON Parameters common to all players
 *                         (generic parameters).
 *
 * @param player_type @in The type of player to set the parameters.
 * @param params      @in The structure containing new parameters for the
 *                        player type.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The value of the @a player_type is invalid.
 *
 * @note All parameters will be used during the next as_play_file/memory().
 *       Hence, this function has no impact on the current melody playback.
 */
T_AS_RET
as_player_set_params(T_AS_PLAYER_TYPE          player_type,
                     const T_AS_PLAYER_PARAMS* params)
{
    if (params == NULL)
    {
        AS_TRACE_0(ERROR, "player_set_params() invalid parameter, NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "player_set_params() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, set parameters.
    switch (player_type)
    {
#if (AS_OPTION_CHANGE_PATH == 1)
    case AS_PLAYER_TYPE_COMMON:
        as_ctrl_blk->params = params->common;
        break;
#endif
    case AS_PLAYER_TYPE_MIDI:
        as_ctrl_blk->midi.params = params->midi;
        break;
#if (AS_OPTION_MP3 == 1)
    case AS_PLAYER_TYPE_MP3:
        as_ctrl_blk->mp3.params = params->mp3;
        break;
#endif
#if (AS_OPTION_AAC == 1)
    case AS_PLAYER_TYPE_AAC:
        as_ctrl_blk->aac.params = params->aac;
        break;
#endif
#if (AS_OPTION_E1_MELODY == 1)
    case AS_PLAYER_TYPE_E1_MELODY:
        as_ctrl_blk->e1_melody.params = params->e1_melody;
        break;
#endif

    default:
        AS_TRACE_L(ERROR,
                   "player_set_params() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    return (T_AS_RET)AS_OK;
}


/**
 * Get the specific parameters for the player of given @a player_type.
 *
 * The output data is passed as a pointer to internal structure,
 * thus the pointed data must not be changed by the caller.
 *
 * The @a player_type allows to get the parameters for the following players:
 * - @b PLAYER_TYPE_MIDI Midi player parameters.
 * - @b PLAYER_TYPE_COMMON Parameters common to all players
 *                         (generic parameters).
 *
 * @param player_type @in  The type of player to get the parameters.
 * @param params      @out The structure containing current parameters for the
 *                         player type.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The value of the @a player_type is invalid or
 *                           the @a params parameter is NULL.
 */
T_AS_RET
as_player_get_params(T_AS_PLAYER_TYPE           player_type,
                     const T_AS_PLAYER_PARAMS** params)
{
    if (params == NULL)
    {
        AS_TRACE_0(ERROR, "player_get_params() invalid parameter, NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "player_get_params() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, get parameters.
    switch (player_type)
    {
#if (AS_OPTION_CHANGE_PATH == 1)
    case AS_PLAYER_TYPE_COMMON:
        *params = (const T_AS_PLAYER_PARAMS*) &as_ctrl_blk->params;
        break;
#endif
    case AS_PLAYER_TYPE_MIDI:
        *params = (const T_AS_PLAYER_PARAMS*) &as_ctrl_blk->midi.params;
        break;
#if(AS_OPTION_MP3 == 1)
    case AS_PLAYER_TYPE_MP3:
        *params = (const T_AS_PLAYER_PARAMS*) &as_ctrl_blk->mp3.params;
        break;
#endif
#if(AS_OPTION_AAC == 1)
    case AS_PLAYER_TYPE_AAC:
        *params = (const T_AS_PLAYER_PARAMS*) &as_ctrl_blk->aac.params;
        break;
#endif
#if (AS_OPTION_E1_MELODY == 1)
    case AS_PLAYER_TYPE_E1_MELODY:
        *params = (const T_AS_PLAYER_PARAMS*) &as_ctrl_blk->e1_melody.params;
        break;
#endif

    default:
        AS_TRACE_L(ERROR,
                   "player_get_params() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    return (T_AS_RET)AS_OK;
}


/**
 * Set the ringer filename for the given @a ringer mode.
 *
 * Finally, this function writes back the filename information to the
 * associated configuration file (depending on the @a ringer_mode).
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param player_type @in  The type of player needed to ring the file.
 * @param filename    @in  The name of the file to ring. There is no checking
 *                         whether the file exists or contains valid data.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The @a filename is NULL or its length is greater
 *                           than AS_FILENAME_MAX_LEN, the @a ringer_mode or
 *                           the @a player_type is not valid.
 * @retval AS_FFS_ERR        An FFS error occurred during the write operation.
 *                           However, the internal data is correctly updated.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_ringer_set_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const T_WCHAR*      filename)
#else
T_AS_RET
as_ringer_set_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const char*      filename)
#endif
{
    UINT16 len;

    if (filename == NULL)
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_file() invalid parameter, filename NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if(AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
#endif
#if(AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
#endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif
        )
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

#if(AS_RFS_API == 1)
	len = wstrlen(filename);
#else
	len = strlen(filename);
#endif

    if (len > AS_FILENAME_MAX_LEN)
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() file name length too long, len=", len);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    // OK, set file.
    as_ctrl_blk->ringer[ringer_mode].player_type   = player_type;
    as_ctrl_blk->ringer[ringer_mode].data_location = AS_LOCATION_FILE;

#if(AS_RFS_API == 1)
	wstrcpy(as_ctrl_blk->ringer[ringer_mode].data.filename, filename);
#else
	strcpy(as_ctrl_blk->ringer[ringer_mode].data.filename, filename);
#endif

    AS_TRACE_3(DEBUG_LOW,
               "ringer_set_file() ringer_mode %d: filename %s, player_type %d successfully updated.",
               ringer_mode, filename, player_type);

    // Update the ringer information to the mode file.
    return as_ringer_write_file(as_ctrl_blk->filename[ringer_mode],
                                &as_ctrl_blk->ringer[ringer_mode]);
}

#if(AS_OPTION_LINEAR_FS == 1)
/**
 * Set the ringer linear filename for the given @a ringer mode.
 *
 * Finally, this function writes back the filename information to the
 * associated configuration file (depending on the @a ringer_mode).
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param player_type @in  The type of player needed to ring the file.
 * @param filename    @in  The name of the file to ring. There is no checking
 *                         whether the file exists or contains valid data.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The @a filename is NULL or its length is greater
 *                           than AS_FILENAME_MAX_LEN, the @a ringer_mode or
 *                           the @a player_type is not valid.
 * @retval AS_LFS_ERR        An LFS error occurred during the write operation.
 *                           However, the internal data is correctly updated.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_ringer_set_linear_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const T_WCHAR*      filename)
#else

T_AS_RET
as_ringer_set_linear_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const char*      filename)
#endif
{
    UINT16 len;

    if (filename == NULL)
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_file() invalid parameter, filename NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if (AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
#endif
#if (AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
#endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif


        )
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }


    if(
#if (AS_OPTION_MP3 == 1)
	AS_PLAYER_TYPE_MP3 == player_type ||
#endif
#if (AS_OPTION_AAC == 1)
	AS_PLAYER_TYPE_AAC == player_type ||
#endif
#if (AS_OPTION_E1_MELODY == 1)
        AS_PLAYER_TYPE_E1_MELODY == player_type ||
#endif
	AS_PLAYER_TYPE_BUZZER == player_type)
    {
    	AS_TRACE_L(ERROR,
                   "ringer_set_file() player type not supported for LFS, player_type=",
                   player_type);
        return (T_AS_RET)AS_NOT_SUPPORTED;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


#if(AS_RFS_API == 1)
	len = wstrlen(filename);
#else
	len = strlen(filename);
#endif

    if (len > AS_FILENAME_MAX_LEN)
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_file() file name length too long, len=", len);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    // OK, set file.
    as_ctrl_blk->ringer[ringer_mode].player_type   = player_type;
    as_ctrl_blk->ringer[ringer_mode].data_location = AS_LOCATION_LINEAR_FILE;

#if(AS_RFS_API == 1)
	wstrcpy(as_ctrl_blk->ringer[ringer_mode].data.filename, filename);
#else
	strcpy(as_ctrl_blk->ringer[ringer_mode].data.filename, filename);
#endif

    AS_TRACE_3(DEBUG_LOW,
               "ringer_set_file() ringer_mode %d: filename %s, player_type %d successfully updated.",
               ringer_mode, filename, player_type);

    // Update the ringer information to the mode file.
    return as_ringer_write_file(as_ctrl_blk->filename[ringer_mode],
                                &as_ctrl_blk->ringer[ringer_mode]);
}

/**
 * Start the playback melody stored in the given linear flash file according to the given
 * @a player type.
 * This function allocates the AS_START_REQ message and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param player_type @in  The type of player to use to play the melody stored
 *                         in the @a filename.
 * @param filename    @in  The name of the file to play.
 * @param volume      @in  The volume to set.
 *                         It shall be one of the defined values:
 *                         RINGER_VOLUME_SILENT, RINGER_VOLUME_LOW,
 *                         RINGER_VOLUME_MEDIUM, RINGER_VOLUME_HIGH.
 * @param loop        @in  Whether the player has to loop on the melody (TRUE)
 *                         or not (FALSE).
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @retval AS_INVALID_PARAM  The @a filename is NULL or its length is greater
 *                           than AS_FILENAME_MAX_LEN, the @a player_type or
 *                           the @a volume is not valid.
 *
 * @async AS_START_IND This message is sent when the player melody is started.
 *                     Or if an error occurred during the player set-up phase.
 *                     The T_AS_START_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @async AS_STOP_IND  This message is sent when the player melody
 *                     automatically stopped after an end of media.
 *                     The T_AS_STOP_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @note This function is a bridge function.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_play_linear_file(T_AS_PLAYER_TYPE        player_type,
             const T_WCHAR*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             const T_RV_RETURN_PATH* rp)
#else
T_AS_RET
as_play_linear_file(T_AS_PLAYER_TYPE        player_type,
             const char*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             const T_RV_RETURN_PATH* rp)
#endif
{
    T_AS_START_REQ* msg;
    T_RVF_MB_STATUS mb_status;
    UINT16          len;

    if (filename == NULL)
    {
        AS_TRACE_0(ERROR,
                   "play_file() invalid parameter, filename NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if (AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
#endif
#if (AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
#endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif
        )
    {
        AS_TRACE_L(ERROR,
                   "play_file() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if(
#if (AS_OPTION_MP3 == 1)
	AS_PLAYER_TYPE_MP3 == player_type ||
#endif
#if (AS_OPTION_AAC == 1)
	AS_PLAYER_TYPE_AAC == player_type ||
#endif
#if (AS_OPTION_E1_MELODY == 1)
       AS_PLAYER_TYPE_E1_MELODY == player_type  ||
#endif
	AS_PLAYER_TYPE_BUZZER == player_type)
    {
    	AS_TRACE_L(ERROR,
                   "ringer_set_file() player type not supported for LFS, player_type=",
                   player_type);
        return (T_AS_RET)AS_NOT_SUPPORTED;
    }

    if ((volume < AS_VOLUME_SILENT) ||
        (volume > AS_VOLUME_HIGH))
    {
        AS_TRACE_L(ERROR,
                   "play_file() volume not supported, volume=",
                   volume);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "play_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


#if(AS_RFS_API == 1)
	len = wstrlen(filename);
#else
	len = strlen(filename);
#endif

    if (len > AS_FILENAME_MAX_LEN)
    {
        AS_TRACE_L(ERROR,
                   "play_file() file name length too long, len=", len);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "play_file() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return (T_AS_RET)AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "play_file() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_START_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

    // Player start parameters.
    msg->player_type   = player_type;
    msg->volume        = volume;
    msg->loop          = loop;
    msg->ringer_mode   = AS_RINGER_MODE_NONE;
    msg->data_location = AS_LOCATION_LINEAR_FILE;
#if(AS_RFS_API == 1)
	wstrcpy(msg->data.filename, filename);
#else
	strcpy(msg->data.filename, filename);
#endif

    // Return path.
    msg->rp = *rp;

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "play_file() not able to send message.");
        rvf_free_buf(msg);
        return AS_MESSAGING_ERR;
    }

    AS_TRACE_4(DEBUG_LOW,
               "play_file() successfully sent message: player_type %d, volume %d, loop %d, filename %s.",
               player_type, volume, loop, filename);

    return (T_AS_RET)AS_OK;
}

#endif

/**
 * Set the ringer buffer for the given @a ringer_mode.
 *
 * Finally, this function writes back the buffer information to the
 * associated configuration file (depending on the @a ringer_mode).
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param player_type @in  The type of player needed to ring the file.
 * @param address     @in  The address of the buffer to ring.
 * @param size        @in  The size of the buffer to ring.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The @a ringer_mode or the @a player_type is not valid.
 * @retval AS_FFS_ERR        An FFS error occurred during the write operation.
 *                           However, the internal data is correctly updated.
 *
 * @warning This function does not check the buffer validity. If @ buffer
 *          does not point to a valid data, unexpected behavior may occur.
 */
T_AS_RET
as_ringer_set_memory(T_AS_RINGER_MODE ringer_mode,
                     T_AS_PLAYER_TYPE player_type,
                     const UINT32*    address,
                     UINT32           size)
{
    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_memory() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if(AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
 #endif
#if(AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
 #endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif

       )
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_memory() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

#if (AS_OPTION_MP3 == 1)
   if ( player_type == AS_PLAYER_TYPE_MP3 )
	return (T_AS_RET)AS_NOT_SUPPORTED;
#endif

#if (AS_OPTION_AAC == 1)
   if ( player_type == AS_PLAYER_TYPE_AAC )
	return (T_AS_RET)AS_NOT_SUPPORTED;
#endif

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_memory() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, set memory.
    as_ctrl_blk->ringer[ringer_mode].player_type   = player_type;
    as_ctrl_blk->ringer[ringer_mode].data_location = AS_LOCATION_MEMORY;
    as_ctrl_blk->ringer[ringer_mode].data.memory.address = address;
    as_ctrl_blk->ringer[ringer_mode].data.memory.size    = size;

    AS_TRACE_4(DEBUG_LOW,
               "ringer_set_memory() ringer_mode %d: address 0x%08x, size %d, player_type %d successfully updated.",
               ringer_mode, (UINT32) address, size, player_type);

    // Update the ringer information to the mode file.
    return as_ringer_write_file(as_ctrl_blk->filename[ringer_mode],
                                &as_ctrl_blk->ringer[ringer_mode]);
}


/**
 * Set the volume for the given @a ringer mode.
 *
 * Finally, this function writes back the volume information to the
 * associated configuration file (depending on the @a ringer_mode).
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param volume      @in  The volume to set.
 *                         It shall be one of the defined values:
 *                         RINGER_VOLUME_SILENT, RINGER_VOLUME_LOW,
 *                         RINGER_VOLUME_MEDIUM, RINGER_VOLUME_HIGH.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The @a ringer_mode or the @a volume is not valid.
 * @retval AS_FFS_ERR        An FFS error occurred during the write operation.
 *                           However, the internal data is correctly updated.
 */
T_AS_RET
as_ringer_set_volume(T_AS_RINGER_MODE ringer_mode,
                     T_AS_VOLUME      volume)
{
    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_volume() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((volume < AS_VOLUME_SILENT) ||
        (volume > AS_VOLUME_HIGH))
    {
        AS_TRACE_L(ERROR,
                   "ringer_set_volume() volume not supported, volume=",
                   volume);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "ringer_set_volume() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, set parameters.
    as_ctrl_blk->ringer[ringer_mode].volume = volume;

    AS_TRACE_2(DEBUG_LOW,
               "ringer_set_volume() ringer_mode %d: volume %d successfully updated.",
               ringer_mode, volume);

    // Update the ringer information to the mode file.
    return as_ringer_write_file(as_ctrl_blk->filename[ringer_mode],
                                &as_ctrl_blk->ringer[ringer_mode]);
}


/**
 * Get the ringer information for the given @a ringer_mode.
 *
 * This function returns the ringer information @a ringer_info
 * according to the @a ringer_mode.
 *
 * The output data is passed as a pointer to internal structure, thus the
 * pointed data must not be changed by the caller.
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param ringer      @out The address of a valid pointer that will point
 *                         to the internal ringer information.
 *
 * @retval AS_OK             The call is successful.
 * @retval AS_NOT_READY      The AS has not finished its initialization phase.
 * @retval AS_INVALID_PARAM  The @a ringer_mode parameter is not valid, or the
 *                           @a ringer is NULL.
 */
T_AS_RET
as_ringer_get(T_AS_RINGER_MODE         ringer_mode,
              const T_AS_RINGER_INFO** ringer)
{
    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ringer_get() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if (ringer == NULL)
    {
        AS_TRACE_0(ERROR, "ringer_get() invalid parameter, ringer NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR,
                   "ringer_get() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // OK, get parameters.
    *ringer = &as_ctrl_blk->ringer[ringer_mode];

    AS_TRACE_L(DEBUG_LOW,
               "ringer_get() successfully got, ringer_mode=", ringer_mode);

    return (T_AS_RET)AS_OK;
}


/**
 * Start the playback melody stored in the given file according to the given
 * @a player type.
 * This function allocates the AS_START_REQ message and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param player_type @in  The type of player to use to play the melody stored
 *                         in the @a filename.
 * @param filename    @in  The name of the file to play.
 * @param volume      @in  The volume to set.
 *                         It shall be one of the defined values:
 *                         RINGER_VOLUME_SILENT, RINGER_VOLUME_LOW,
 *                         RINGER_VOLUME_MEDIUM, RINGER_VOLUME_HIGH.
 * @param loop        @in  Whether the player has to loop on the melody (TRUE)
 *                         or not (FALSE).
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @retval AS_INVALID_PARAM  The @a filename is NULL or its length is greater
 *                           than AS_FILENAME_MAX_LEN, the @a player_type or
 *                           the @a volume is not valid.
 *
 * @async AS_START_IND This message is sent when the player melody is started.
 *                     Or if an error occurred during the player set-up phase.
 *                     The T_AS_START_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @async AS_STOP_IND  This message is sent when the player melody
 *                     automatically stopped after an end of media.
 *                     The T_AS_STOP_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @note This function is a bridge function.
 */

#if(AS_RFS_API == 1)
T_AS_RET
as_play_file(T_AS_PLAYER_TYPE        player_type,
             const T_WCHAR*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             BOOLEAN                 play_bar_on,
             const T_RV_RETURN_PATH* rp)
#else

T_AS_RET
as_play_file(T_AS_PLAYER_TYPE        player_type,
             const char*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             BOOLEAN                 play_bar_on,
             const T_RV_RETURN_PATH* rp)
#endif
{
    T_AS_START_REQ* msg;
    T_RVF_MB_STATUS mb_status;
    UINT16          len;

    if (filename == NULL)
    {
        AS_TRACE_0(ERROR,
                   "play_file() invalid parameter, filename NULL.");
        return (T_AS_RET)AS_INVALID_PARAM;
    }

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if (AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
#endif
#if (AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
#endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif

        )
    {
        AS_TRACE_L(ERROR,
                   "play_file() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((volume < AS_VOLUME_SILENT) ||
        (volume > AS_VOLUME_HIGH))
    {
        AS_TRACE_L(ERROR,
                   "play_file() volume not supported, volume=",
                   volume);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "play_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


#if(AS_RFS_API == 1)
	len = wstrlen(filename);
#else
	len = strlen(filename);
#endif

    if (len > AS_FILENAME_MAX_LEN)
    {
        AS_TRACE_L(ERROR,
                   "play_file() file name length too long, len=", len);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "play_file() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return (T_AS_RET)AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "play_file() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_START_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

    // Player start parameters.
    msg->player_type   = player_type;
    msg->volume        = volume;
    msg->loop          = loop;
    msg->play_bar_on = play_bar_on;
    msg->ringer_mode   = AS_RINGER_MODE_NONE;
    msg->data_location = AS_LOCATION_FILE;

#if(AS_RFS_API == 1)
	wstrcpy(msg->data.filename, filename);
#else
	strcpy(msg->data.filename, filename);
#endif

    // Return path.
    msg->rp = *rp;

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "play_file() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    //to be removed
    AS_TRACE_1(DEBUG_LOW, "play_file() msg->play_bar_on %d",msg->play_bar_on);

    AS_TRACE_4(DEBUG_LOW,
               "play_file() successfully sent message: player_type %d, volume %d, loop %d, filename %s.",
               player_type, volume, loop, filename);
    return (T_AS_RET)AS_OK;
}

/**
 *  Pause the current melody playback.
 * This function allocates the AUDIO_MP3_PAUSE_REQ meessage and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @note This function is a bridge function.
 */
T_AS_RET
as_pause(const T_RV_RETURN_PATH* rp)
{
    T_AS_PAUSE_REQ* msg;
    T_RVF_MB_STATUS mb_status;


   if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "pause_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_PAUSE_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "pause() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "pause() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_PAUSE_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

   // Return path.
    msg->rp = *rp;

   //AS_TRACE_1(DEBUG_LOW, "paused state msg->msg_id = %d",AS_PAUSE_REQ);

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "pause() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_0(DEBUG_LOW, "pause() successfully sent message");

    return AS_OK;
}


/**
 *  Resume the current melody playback.
 * This function allocates the AUDIO_MP3_RESUME_REQ meessage and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @note This function is a bridge function.
 */
T_AS_RET
as_resume(const T_RV_RETURN_PATH* rp)
{
    T_AS_RESUME_REQ* msg;
    T_RVF_MB_STATUS mb_status;



    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "resume_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_RESUME_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "resume() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "resume() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_RESUME_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

   // Return path.
    msg->rp = *rp;

   //AS_TRACE_1(DEBUG_LOW, "resume state msg->msg_id = %d",AS_RESUME_REQ);

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "resume() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_0(DEBUG_LOW, "resume() successfully sent message");

    return (T_AS_RET)AS_OK;
}



/**
 * Start the playback melody stored in the given buffer (@a addres)
 * according to the given @a player type.
 * This function allocates the AS_START_REQ message and fills it
 * with the given parameters (memory oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param player_type @in  The type of player to use to play the melody stored
 *                         in the buffer pointed by @a address.
 * @param address     @in  The address of the buffer to play.
 * @param size        @in  The size of the buffer to play.
 * @param volume      @in  The volume to set.
 *                         It shall be one of the defined values:
 *                         RINGER_VOLUME_SILENT, RINGER_VOLUME_LOW,
 *                         RINGER_VOLUME_MEDIUM, RINGER_VOLUME_HIGH.
 * @param loop        @in  Whether the player has to loop on the melody (TRUE)
 *                         or not (FALSE).
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @retval AS_INVALID_PARAM  The @a player_type or the @a volume is not valid.
 *
 * @async AS_START_IND This message is sent when the player melody is started.
 *                     Or if an error occurred during the player set-up phase.
 *                     The T_AS_START_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @async AS_STOP_IND  This message is sent when the player melody
 *                     automatically stopped after an end of media.
 *                     The T_AS_STOP_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @note This function is a bridge function.
 *
 * @warning This function does not check the buffer validity. If @ buffer
 *          does not point to a valid data, unexpected behavior may occur.
 */
T_AS_RET
as_play_memory(T_AS_PLAYER_TYPE        player_type,
               const UINT32*           address,
               UINT32                  size,
               T_AS_VOLUME             volume,
               BOOLEAN                 loop,
               const T_RV_RETURN_PATH* rp)
{
    T_AS_START_REQ* msg;
    T_RVF_MB_STATUS mb_status;

//    if ((player_type <= AS_PLAYER_TYPE_NONE) ||
//        (player_type > AS_PLAYER_TYPE_MAX))
    if (
#if (AS_OPTION_BUZZER == 1)
        (player_type != AS_PLAYER_TYPE_BUZZER) &&
#endif // AS_OPTION_BUZZER
        (player_type != AS_PLAYER_TYPE_MIDI)
#if(AS_OPTION_MP3 == 1)
        && (player_type != AS_PLAYER_TYPE_MP3)
#endif
#if(AS_OPTION_AAC == 1)
        && (player_type != AS_PLAYER_TYPE_AAC)
#endif
#if (AS_OPTION_E1_MELODY == 1)
         && (player_type != AS_PLAYER_TYPE_E1_MELODY)
#endif

        )
    {
        AS_TRACE_L(ERROR,
                   "play_memory() player type not supported, player_type=",
                   player_type);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

   if ( player_type == AS_PLAYER_TYPE_MP3 )
	return (T_AS_RET)AS_NOT_SUPPORTED;

   if ( player_type == AS_PLAYER_TYPE_AAC )
	return (T_AS_RET)AS_NOT_SUPPORTED;

    if ( player_type == AS_PLAYER_TYPE_E1_MELODY)
	return AS_NOT_SUPPORTED;


    if ((volume < AS_VOLUME_SILENT) ||
        (volume > AS_VOLUME_HIGH))
    {
        AS_TRACE_L(ERROR,
                   "play_memory() volume not supported, volume=",
                   volume);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "play_memory() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "play_memory() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return (T_AS_RET)AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "play_memory() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_START_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

    // Player start parameters.
    msg->player_type   = player_type;
    msg->volume        = volume;
    msg->loop          = loop;
    msg->ringer_mode   = AS_RINGER_MODE_NONE;
    msg->data_location = AS_LOCATION_MEMORY;
    msg->data.memory.address = address;
    msg->data.memory.size    = size;

    // Return path.
    msg->rp = *rp;

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "play_memory() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_5(DEBUG_LOW,
               "play_memory() successfully sent message: player_type %d, volume %d, loop %d, address 0x%08x, size %d.",
               player_type, volume, loop, address, size);

    return (T_AS_RET)AS_OK;
}
/*
T_AS_RET
as_forward(INT16  skip_time,const T_RV_RETURN_PATH* rp)
{
   INT32 command;

   command = FORWARD;
   as_forrew(skip_time,rp,command);

}

T_AS_RET
as_rewind(INT16  skip_time,const T_RV_RETURN_PATH* rp)
{
   INT32 command;

   command = REWIND;
      as_forrew(skip_time,rp,command);

}

*/
/**
 *  Forward/Rewind the current melody playback.
 * This function allocates the AS_FORREW_REQ meessage and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @note This function is a bridge function.
 */
/*
	T_AS_RET
as_forrew(INT16  skip_time,const T_RV_RETURN_PATH* rp,INT32 forrew)
{
    T_AS_FORREW_REQ* msg;
    T_RVF_MB_STATUS mb_status;


   if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "pause_file() sorry, SWE is not initialized.");
        return AS_NOT_READY;
    }


    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_FORREW_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "pause() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "pause() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_FORREW_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;
    msg->media_skip_time = skip_time;
    msg->forrew = forrew;

   // Return path.
    msg->rp = *rp;


    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "forrew() not able to send message.");
        rvf_free_buf(msg);
        return AS_MESSAGING_ERR;
    }

    AS_TRACE_0(DEBUG_LOW, "forrew() successfully sent message");

    return AS_OK;
}
*/
T_AS_RET
as_forward(INT16  skip_time,const T_RV_RETURN_PATH* rp)
{
   INT32 command;

   command = FORWARD;
   return(as_forrew(skip_time,rp,command));

}

T_AS_RET
as_rewind(INT16  skip_time,const T_RV_RETURN_PATH* rp)
{
   INT32 command;

   command = REWIND;
      return(as_forrew(skip_time,rp,command));

}


/**
 *  Forward/Rewind the current melody playback.
 * This function allocates the AS_FORREW_REQ meessage and fills it
 * with the given parameters (file oriented).
 *
 * The message is then sent to the AS service for processing.
 *
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @note This function is a bridge function.
 */

T_AS_RET
as_forrew(INT16  skip_time,const T_RV_RETURN_PATH* rp,INT32 forrew)
{
    T_AS_FORREW_REQ* msg;
    T_RVF_MB_STATUS mb_status;


   if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "pause_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_FORREW_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "pause() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "pause() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_FORREW_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;
    msg->media_skip_time = skip_time;
    msg->forrew = forrew;

   // Return path.
    msg->rp = *rp;


    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "forrew() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_0(DEBUG_LOW, "forrew() successfully sent message");

    return AS_OK;
}


/**
 * Start the ringer melody playback for the given ringer mode.
 * This function allocates the AS_START_REQ message and fills it
 * with the player parameters associated to the given @a ringer_mode.
 *
 * The message is then sent to the AS service for processing.
 *
 * @param ringer_mode @in  The ringer mode.
 *                         It shall be one of the defined values:
 *                         RINGER_MODE_IC, RINGER_MODE_SMS,
 *                         RINGER_MODE_ALARM.
 * @param loop        @in  Whether the ringer has to loop on the melody (TRUE)
 *                         or not (FALSE).
 * @param rp          @in  The Riviera return path where the asynchronous
 *                         response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 * @retval AS_INVALID_PARAM  The @a ringer_mode is not valid.
 *
 * @async AS_START_IND This message is sent when the ringer melody is started.
 *                     Or if an error occurred during the ringer set-up phase.
 *                     The T_AS_START_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @async AS_STOP_IND  This message is sent when the ringer melody
 *                     automatically stopped after an end of media.
 *                     The T_AS_STOP_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @note This function is a bridge function.
 */
T_AS_RET
as_ring(T_AS_RINGER_MODE        ringer_mode,
        BOOLEAN                 loop,
        const T_RV_RETURN_PATH* rp)
{
    T_AS_START_REQ* msg;
    T_RVF_MB_STATUS mb_status;

    if ((ringer_mode < 0) ||
        (ringer_mode >= AS_RINGER_MODE_ARRAY_SIZE))
    {
        AS_TRACE_L(ERROR,
                   "ring() ringer mode not supported, ringer_mode=",
                   ringer_mode);
        return (T_AS_RET)AS_INVALID_PARAM;
    }

    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "ring() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }

    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_START_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "ring() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return (T_AS_RET)AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "ring() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_START_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

    if(as_ctrl_blk->ringer_file_created == FALSE)
    {
           as_ringer_create_dir();

           // Read the configuration files for each supported ringer mode.
	   // Do not take error into account. The as_ringer_read_file()
           // function sets default values if file is not available.
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_IC],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_IC]);
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_SMS],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_SMS]);
           (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_ALARM],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_ALARM]);

	   as_ctrl_blk->ringer_file_created = TRUE;
     }
    // Ringer start parameters.
    msg->player_type   = as_ctrl_blk->ringer[ringer_mode].player_type;
    msg->volume        = as_ctrl_blk->ringer[ringer_mode].volume;
    msg->loop          = loop;
    msg->play_bar_on = PLAYBAR_NOT_SUPPORTED;
    msg->ringer_mode   = ringer_mode;
    msg->data_location = as_ctrl_blk->ringer[ringer_mode].data_location;

    if (msg->data_location == AS_LOCATION_FILE
#if( AS_OPTION_LINEAR_FS == 1)
		|| msg->data_location == AS_LOCATION_LINEAR_FILE
#endif
		)
    {
        // Here, the length of the filename is valid.
        // A check was performed in as_ringer_set_file().
#if(AS_RFS_API == 1)
		wstrcpy(msg->data.filename,
               as_ctrl_blk->ringer[ringer_mode].data.filename);
#else
        strcpy(msg->data.filename,
               as_ctrl_blk->ringer[ringer_mode].data.filename);
#endif
    }
    else
    {
        msg->data.memory.address = as_ctrl_blk->ringer[ringer_mode].data.memory.address;
        msg->data.memory.size    = as_ctrl_blk->ringer[ringer_mode].data.memory.size;
    }

    // Return path.
    msg->rp = *rp;

    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "ring() not able to send message.");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_2(DEBUG_LOW,
               "ring() successfully sent message: ringer_mode %d, loop %d.",
               ringer_mode, loop);

    return (T_AS_RET)AS_OK;
}


/**
 * Stop the current melody playback (either player or ringer).
 * This function allocates the AS_STOP_REQ message and send it
 * to the AS service for processing.
 *
 * @param rp @in  The Riviera return path where the asynchronous
 *                response message has to be sent.
 *
 * @retval AS_OK          The call is successful.
 * @retval AS_NOT_READY   The AS has not finished its initialization phase.
 * @retval AS_MEMORY_ERR  Not enough memory to allocate the buffer to send.
 * @retval AS_MESSAGING_ERR  The message could not be sent.
 *
 * @async AS_STOP_IND  This message is sent when the melody is stopped.
 *                     After all previous audio configuration settings
 *                     are restored.
 *                     The T_AS_STOP_IND structure is used to convey the
 *                     asynchronous response.
 *
 * @note This function is a bridge function.
 */
T_AS_RET
as_stop(const T_RV_RETURN_PATH* rp)
{
    T_AS_STOP_REQ*  msg;
    T_RVF_MB_STATUS mb_status;


    if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "stop() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }



    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_STOP_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "stop() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return (T_AS_RET)AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "stop() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_STOP_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;

    // Ringer stop parameters : none.

    // Return path.
    msg->rp = *rp;
    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "stop() not able to send message");
        rvf_free_buf(msg);
        return (T_AS_RET)AS_MESSAGING_ERR;
    }



    AS_TRACE_0(DEBUG_LOW, "stop() successfully sent message");

    return (T_AS_RET)AS_OK;
}

#if(AS_RFS_API == 1)
int wstrncmp(const T_WCHAR *string1, const T_WCHAR *string2, UINT16 n)
{
     if (n)
     {
	 const T_WCHAR *s1 = string1 - 1;
	 const T_WCHAR *s2 = string2 - 1;
	 T_WCHAR        cp;
	 int         result;

	 do
	    if (result = *++s1 - (cp = *++s2)) return result;
	 while (cp && --n);
     }
     return 0;
}
#endif



// Layer_BT_OMAPS00152447 

T_AS_RET
as_bt_cfg(BOOL connected_status, const T_RV_RETURN_PATH* rp)
 {
    T_AS_BT_CFG_REQ* msg;
    T_RVF_MB_STATUS mb_status;


   if ((as_ctrl_blk == NULL) ||
        (as_ctrl_blk->state == AS_STATE_INIT))
    {
        AS_TRACE_0(ERROR, "pause_file() sorry, SWE is not initialized.");
        return (T_AS_RET)AS_NOT_READY;
    }


    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof( T_AS_BT_CFG_REQ),
                            (T_RVF_BUFFER**) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status == RVF_RED)
    {
        AS_TRACE_L(ERROR,
                   "bt() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
        return AS_MEMORY_ERR;
    }

    // If reached the memory bank threshold, log a warning.
    if (mb_status == RVF_YELLOW)
    {
        AS_TRACE_L(WARNING,
                   "bt() memory usage reached the threshold, mb_id=",
                   as_ctrl_blk->mb_external);
    }

    // Fill the message.

    // System information.
    msg->header.msg_id       = AS_BT_CFG_REQ;
    msg->header.src_addr_id  = rvf_get_taskid();
    msg->header.dest_addr_id = as_ctrl_blk->addr_id;
    msg->connected_status = connected_status;

   // Return path.
    msg->rp = *rp;


    // Send the messsage to the AS SWE.
    if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
    {
        AS_TRACE_0(ERROR, "bt() not able to send message.");
        rvf_free_buf(msg);
         return (T_AS_RET)AS_MESSAGING_ERR;
    }

    AS_TRACE_0(DEBUG_LOW, "bt() successfully sent message");

    return AS_OK;
}
//Daisy tang added for Real Resume feature 20071107 
int as_get_state(void)
{
 return as_ctrl_blk->state;
}





