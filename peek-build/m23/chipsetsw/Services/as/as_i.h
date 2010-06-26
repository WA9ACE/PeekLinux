/**
 * @file
 *
 * @brief API Definition for AS SWE.
 *
 * This file gathers all the constants, structure and functions declaration
 * useful for a AS SWE user.
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

#ifndef __AS_I_H_
#define __AS_I_H_

#include "swconfig.cfg"

// AS OPTIONS : begin ---------------------------------------------------------
//
// 1 the option is available, 0 it is not.
//

/// Wether or not the BUZZER support is available.
#define AS_OPTION_BUZZER 0

#define AS_OPTION_CHANGE_PATH 0

/// Enable MP3 option in AS
//#define AS_OPTION_MP3 1

/// Disable Linear FFS option in AS
//#define AS_OPTION_LINEAR_FS 1

/// Enable AAC option in AS - This has to be added as part of XML files
#define AS_OPTION_AAC 1

/// Enable E1-Melody option in AS - This has to be added as part of XML files
#define AS_OPTION_E1_MELODY 1

// AS OPTIONS : end -----------------------------------------------------------


#include "as/as_api.h"
#include "as/as_state_i.h"
#include "as/as_message.h"

#ifdef AS_OPTION_MIDI 

#include "bae/bae_api_new.h"

#endif 

#include "audio/audio_api.h"

#include "rvm/rvm_use_id_list.h"
#include "rvf/rvf_api.h"

#include <stdio.h>

#if (AS_OPTION_LINEAR_FS == 1)
#include "ffs/ffs.h"
#endif

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif
// Trace macros

#ifndef DEBUG_ENABLE

#  define AS_TRACE_0(_level, _msg)
#  define AS_TRACE_L(_level, _msg, _p1)
#  define AS_TRACE_1(_level, _msg, _p1)
#  define AS_TRACE_2(_level, _msg, _p1, _p2)
#  define AS_TRACE_3(_level, _msg, _p1, _p2, _p3)
#  define AS_TRACE_4(_level, _msg, _p1, _p2, _p3,_p4)
#  define AS_TRACE_5(_level, _msg, _p1, _p2, _p3, _p4, _p5)

#  define AS_DEBUG_0(_level, _msg)
#  define AS_DEBUG_L(_level, _msg, _p1)
#  define AS_DEBUG_1(_level, _msg, _p1)
#  define AS_DEBUG_2(_level, _msg, _p1, _p2)
#  define AS_DEBUG_3(_level, _msg, _p1, _p2, _p3)
#  define AS_DEBUG_4(_level, _msg, _p1, _p2, _p3,_p4)
#  define AS_DEBUG_5(_level, _msg, _p1, _p2, _p3, _p4, _p5)

#else

#  define AS_TRACE_0(_level, _msg) \
    rvf_send_trace("[AS] " _msg, sizeof("[AS] " _msg) - 1, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID)

#  define AS_TRACE_L(_level, _msg, _p1) \
    rvf_send_trace("[AS] " _msg, sizeof("[AS] " _msg) - 1, _p1, RV_TRACE_LEVEL_ ## _level, AS_USE_ID)

#  define AS_TRACE_1(_level, _msg, _p1) \
  { char myMsg[128]; int len = sprintf(myMsg, "[AS] " _msg, _p1); \
    rvf_send_trace(myMsg, len, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID); }

#  define AS_TRACE_2(_level, _msg, _p1, _p2) \
  { char myMsg[128]; int len = sprintf(myMsg, "[AS] " _msg, _p1, _p2); \
    rvf_send_trace(myMsg, len, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID); }

#  define AS_TRACE_3(_level, _msg, _p1, _p2, _p3) \
  { char myMsg[128]; int len = sprintf(myMsg, "[AS] " _msg, _p1, _p2, _p3); \
    rvf_send_trace(myMsg, len, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID); }

#  define AS_TRACE_4(_level, _msg, _p1, _p2, _p3, _p4) \
  { char myMsg[128]; int len = sprintf(myMsg, "[AS] " _msg, _p1, _p2, _p3, _p4); \
    rvf_send_trace(myMsg, len, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID); }

#  define AS_TRACE_5(_level, _msg, _p1, _p2, _p3, _p4, _p5) \
  { char myMsg[128]; int len = sprintf(myMsg, "[AS] " _msg, _p1, _p2, _p3, _p4, _p5); \
    rvf_send_trace(myMsg, len, NULL_PARAM, RV_TRACE_LEVEL_ ## _level, AS_USE_ID); }

// Specific debug traces, to remove in production mode.
#  if 1
#    define AS_DEBUG_0(_level, _msg) \
            AS_TRACE_0(DEBUG_ ## _level, _msg)
#    define AS_DEBUG_L(_level, _msg, _p1) \
            AS_TRACE_L(DEBUG_ ## _level, _msg, _p1)
#    define AS_DEBUG_1(_level, _msg, _p1) \
            AS_TRACE_1(DEBUG_ ## _level, _msg, _p1)
#    define AS_DEBUG_2(_level, _msg, _p1, _p2) \
            AS_TRACE_2(DEBUG_ ## _level, _msg, _p1, _p2)
#    define AS_DEBUG_3(_level, _msg, _p1, _p2, _p3) \
            AS_TRACE_3(DEBUG_ ## _level, _msg, _p1, _p2, _p3)
#    define AS_DEBUG_4(_level, _msg, _p1, _p2, _p3,_p4) \
            AS_TRACE_4(DEBUG_ ## _level, _msg, _p1, _p2, _p3, _p4)
#    define AS_DEBUG_5(_level, _msg, _p1, _p2, _p3, _p4, _p5) \
            AS_TRACE_5(DEBUG_ ## _level, _msg, _p1, _p2, _p3, _p4, _p5)
#  endif

#endif


#if (AS_OPTION_BUZZER == 1)
/**
 * @brief The context of the buzzer player.
 *
 * This structure gathers all context variables of the buzzer player.
 */
typedef struct
{
    /// The timer id. used to trigger periodic buzzer operation.
    T_RVF_TIMER_ID tm_id;
//    UINT32         duration;
//    UINT32         enabled_duration;
//    UINT32         disabled_duration;
//    int            frequency;
    /// The number of buzzer enable period to generate.
    /// May be could be removed and replaced by a lighter implementation.
    UINT16         counter;
    /// The state of the buzzer (either enabled or disabled), so that
    /// next time the timer expires, the buzzer state is changed.
    BOOLEAN        toggle;
}
T_AS_BUZZER_CONTEXT;
#endif // AS_OPTION_BUZZER


/**
 * @brief The context of the midi BAE player.
 *
 * This structure gathers all context variables of the BAE player.
 */
typedef struct
{
    /// The id. of the midi BAE player.
#ifdef AS_OPTION_MIDI 
    mbObjectID              player;
#endif 
    /// The user modifiable midi BAE parameters.
    T_AS_PLAYER_MIDI_PARAMS params;
}
T_AS_MIDI_CONTEXT;

/**
 * @brief The context of the mp3 player.
 *
 * This structure gathers all context variables required to play mp3.
 */
typedef struct
{
    //int                    to_define;
    /// The user modifiable MP3 parameters.
    T_AS_PLAYER_MP3_PARAMS params;
}
T_AS_MP3_CONTEXT;

/**
 * @brief The context of the aac player.
 *
 * This structure gathers all context variables required to play aac.
 */
typedef struct
{
    //int                    to_define;
    /// The user modifiable AAC parameters.
    T_AS_PLAYER_AAC_PARAMS params;
}
T_AS_AAC_CONTEXT;

/**
 * @brief The context of the E1-Melody player.
 *
 * This structure gathers all context variables required to play E1-Melody.
 */
typedef struct
{
    //int                    to_define;
    /// The user modifiable AAC parameters.
    T_AS_PLAYER_E1_MELODY_PARAMS params;
}
T_AS_E1_MELODY_CONTEXT;

/**
 * @brief The Control Block buffer of AS.
 *
 * This structure gathers all the 'global variables' of the AS instance.
 *
 * Indeed, global variables have not to be defined in order to avoid
 * using static memory.
 *
 * On the contrary, a T_AS_CTRL_BLK control block buffer is allocated by the
 * Riviera Manager when creating the AS SWE. Then, the AS SWE
 * must always refer to this control block when access to 'global variable' 
 * is needed.
 */
typedef struct
{
    /// The different ringer mode configuration file names.
#if(AS_RFS_API == 1)
	const T_WCHAR*      filename[AS_RINGER_MODE_ARRAY_SIZE];
#else
	const char*      filename[AS_RINGER_MODE_ARRAY_SIZE];
#endif
    /// The return path of the AS SWE.
    T_RV_RETURN_PATH rp;
    /// The id of the AS memory bank (external memory).
    T_RVF_MB_ID      mb_external;
    /// The address id. of the AS.
    T_RVF_ADDR_ID    addr_id;
    /// The inner state of the AS.
    T_AS_STATE       state;
    /// Whether the GPRS mode is active or not.
    BOOLEAN          gprs_mode;
    /// Whether the current player is interrupted by a priority one (or not).
    BOOLEAN          interrupted;
    /// Whether the current message has to be freed or not
    /// (if not, it is saved for further processing).
    BOOLEAN          free_msg;
    /// Whether the end of media has been reached.
    BOOLEAN          end_of_media;
    /// Whether the internal stop specific processing was activated.
    BOOLEAN          internal_stop;
    /// Whether progress bar option is required for current media playing
    BOOLEAN 		play_bar_on;
    /// Whether an error was detected during the processing.
    T_AS_RET         error;
    /// The type of player that will be used to play a file or a memory buffer.
    T_AS_PLAYER_TYPE player_type;


    #if (AS_OPTION_MP3 == 1 || AS_OPTION_AAC == 1)
    /// The player data that gives the filename needs to be played.
    T_AS_PLAYER_DATA player_data;
    /// The ringer is in loop or not
    BOOLEAN 		loop;
    /// Whether MP3 play is called for the first  time in a loop call or
    /// not. This is helpful in sending AS_START_IND message to the
    /// caller. This will be reset when loop variable is initialized.
    BOOLEAN		first_play;
#endif

#if (AS_OPTION_LINEAR_FS == 1)
   /// The Linear file descriptor
   fd_t 		linear_file_descriptor;
#endif
    /// The volume to play.
    T_AS_VOLUME      volume;
    /// The event that triggered the ringer.
    T_AS_RINGER_MODE ringer_mode;
    /// The player data associated with each ringer mode.
    T_AS_RINGER_INFO ringer[AS_RINGER_MODE_ARRAY_SIZE];
    /// The return path to use to reply message back.
    T_RV_RETURN_PATH client_rp;
    /// The time value by which media playing is skipped or rolled back
    INT16  media_skip_time;
    ///  The request is for foward the playtime or reverse the play time
    INT32 forrew;
    /// The message that triggered the recover mode or 0.
    UINT32           recover;
    /// The saved start request when the current player is interrupted.
    const T_AS_START_REQ* saved;
    /// Audio previous configuration.
    struct {
    #if (AS_OPTION_CHANGE_PATH == 1)
        /// Speaker mode.
        INT8 speaker_mode;
        /// Extra gain.
        INT8 extra_gain;
     #endif
        /// Stereo volume level.
        T_AUDIO_STEREO_SPEAKER_LEVEL volume;
    } previous;

#if ( AS_OPTION_CHANGE_PATH == 1)
    /// User parameters that are common to all players.
    T_AS_PLAYER_COMMON_PARAMS  params;
#endif
    /// Midi BAE context (internal data and user parameters).
    T_AS_MIDI_CONTEXT   midi;

#if (AS_OPTION_MP3 == 1)
    T_AS_MP3_CONTEXT  mp3;
#endif
#if (AS_OPTION_AAC == 1)
    T_AS_AAC_CONTEXT  aac;
#endif

#if(AS_OPTION_E1_MELODY ==1) 
   T_AS_E1_MELODY_CONTEXT e1_melody;
#endif
    
#if (AS_OPTION_BUZZER == 1)
    /// Buzzer context (internal data and user parameters).
    T_AS_BUZZER_CONTEXT buzzer;
#endif // AS_OPTION_BUZZER

BOOLEAN bt_connect_status;
    /// Synchronization barrier used during compatibility management.
    /// @todo: check utility:
//    T_RVF_MUTEX barrier;
#if(AS_RFS_API == 1)
// Ringer file created
    BOOLEAN ringer_file_created;
#endif
}
T_AS_CTRL_BLK;


// External reference to the "global variables" structure pointer.
extern T_AS_CTRL_BLK* as_ctrl_blk;


#endif // __AS_I_H_
