
/**
 * @file    bae_api.h
 *
 * API Definition for BAE SWE.
 *
 * @author  Richard Powell (richard@beatnik.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  9/22/2003   Richard Powell (richard@beatnik.com)        Create.
 *  7/21/2004   Richard Powell (richard@beatnik.com)        changed DRC commands to work on specific player.
 *  12/23/2004  Ravi Tatavarthi (ravi@beatnik.com)          Added NEW synchronous BAE API functions.
 *
 * (C) Copyright 2003 by Beatnik, Inc., All Rights Reserved
 */

#ifndef __BAE_API_H_
#define __BAE_API_H_


#include "rvm/rvm_gen.h"        /* Generic RVM types and functions. */
//#include "bae/bae_api_new.h"
//#include "rv/general.h"

/* x0056422 - OMAPS00156759 - Unicode support */
#include "mBAE_Types.h"
#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name BAE Return type and return values.
 *
 * We map our return types to the 4 most common return types:
 * BAE_OK               --> RV_OK               :: Function returned normally
 * BAE_NOT_SUPPORTED    --> RV_NOT_SUPPORTED    :: Operation was not supported
 * BAE_MEMORY_ERR       --> RV_MEMORY_ERR       :: Corrupted or lack of memory caused an error
 * BAE_INTERNAL_ERR     --> RV_INTERNAL_ERR     :: Internal error
 */
typedef INT8 T_BAE_RETURN;
#define BAE_OK                  (0)
#define BAE_NOT_SUPPORTED       (-1)
#define BAE_MEMORY_ERR          (-2)
#define BAE_INTERNAL_ERR        (-3)
#define BAE_INVALID_PLAYER      (-4)
#define BAE_INVALID_PARAMETER   (-5)
#define BAE_ALREADY_PLAYING     (-6)
#define BAE_ALREADY_STOPPED     (-7)
#define BAE_LOAD_ERR            (-8)
#define BAE_NOT_LOADED          (-9)
#define BAE_UNDERFLOW           (-10)

//max number of chars in file path name
#define BAE_MAX_FILEPATH        (100)

/**
 * @name BAE File types.
 *
 * We will use the mobileBAE defined file types.  Note that some file
 * types may not be supported with this build
 */
typedef enum BAE_FILETYPE
{
    BAE_UNKNOWN_FILETYPE = -1,
    BAE_DLS,
    BAE_SMF,
    BAE_SMS,
    BAE_RTX,
    BAE_IMY,
    BAE_IREZ,
    BAE_MIDI,
    BAE_IMELODY,
    BAE_SMAF,
    BAE_XMF,
    BAE_WAV,
    BAE_AU,
    BAE_AIFF,
    BAE_RMF
} T_BAE_FILETYPE;



/**
 * @name BAE File location.
 *
 * Bank file location.
 */
typedef enum BAE_BANK_LOCATION
{
    BAE_SLOW_MEMORY,
    BAE_FAST_MEMORY,
    BAE_SERIALIZED
} T_BAE_BANK_LOCATION;



/**
 * @name channels for output path.
 *
 * Bank file location.
 */
typedef enum BAE_AUDIO_PATH
{
    BAE_HEADPHONES = 3,
    BAE_HANDSET = 4
} T_BAE_AUDIO_PATH;



/**
 * @name BAE Player ID.
 *
 * The player is a struct that contains both a Midi Player and a Ringtone Player.
 */
typedef void *T_BAE_PLAYER_ID;


/**
 * @name BAE DRC Parameters.
 *
 * The DRC parameters is a struct that contains the drc parameters.
 */
typedef struct 
{
    long BAE_DRC_Threshold;
    unsigned long BAE_DRC_Ratio;
    unsigned long BAE_DRC_AttackTime;
    unsigned long BAE_DRC_ReleaseTime;
} T_BAE_DRC_PARAM;


/**
 * @name bae_create_player
 * Creates a player object. A player represents a handle to the bae SWE that
 * allows MMI's to play media files.
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_create_player (
                                T_RV_RETURN return_path
                                );


/**
 * @name bae_destroy_player
 * Destroys a player object. 
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_destroy_player ( 
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );


/**
 * @name bae_load_from_file
 * Loads media data from a file into the player. 
 *
 * @param   player  the pointer to the player.
 * @param   filePath    path to the file
 * @param   fileType    the file's media type
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_load_from_file (
                                 T_BAE_PLAYER_ID player,
                                 mbChar filePath[],
                                 T_BAE_FILETYPE fileType,
                                 T_RV_RETURN return_path
                                 );


/**
 * @name bae_load_from_memory
 * Loads media data from memory into the player. 
 *
 * @param   player  the pointer to the player.
 * @param   memoryPointer   media data in memory
 * @param   size    size of the media data in memory
 * @param   fileType    the file's media type
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_load_from_memory (
                                   T_BAE_PLAYER_ID player,
                                   void *memoryPointer,
                                   int size,
                                   T_BAE_FILETYPE fileType,
                                   T_RV_RETURN return_path
                                   );


/**
 * @name bae_unload
 * Unload media data from the player. 
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_unload (
                         T_BAE_PLAYER_ID player,
                         T_RV_RETURN return_path
                         );


/**
 * @name bae_player_preroll
 * Preroll player; have player do CPU intensive preparsing of the media data
 * file to allow very quick start when bae_player_start is called.
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_preroll_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );


/**
 * @name bae_player_start
 * Start playing loaded media.
 *
 * @param   player  the pointer to the player.
 * @param   loop_back   boolean to indicate play once (false) or repeat indefinately (true).
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_start_player (
                               T_BAE_PLAYER_ID player,
                               BOOL loop_back,
                               T_RV_RETURN return_path
                               );


/**
 * @name bae_player_stop
 * Stop playing loaded media.
 *
 * @param   player  the pointer to the player.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_stop_player ( 
                              T_BAE_PLAYER_ID player,
                              T_RV_RETURN return_path
                              );


/**
 * @name bae_get_version
 * Gets the current bae version.
 *
 * @param   version The buffer to write the version string into.
 */
T_BAE_RETURN bae_get_version (
                              char version[]
                              );


/**
 * @name bae_change_voice_limit
 * Changes the bae voice limit.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_change_voice_limit (
                                     int voice_limit,
                                     T_RV_RETURN return_path
                                     );


/**
 * @name bae_get_voice_limit
 * Gets the bae voice limit.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_get_voice_limit (
                                  T_RV_RETURN return_path
                                  );


/**
 * @name bae_change_output_channels
 * Changes the output channels.
 *
 * @param   channels to change output to.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_change_output_channels (
                                         int channels,
                                         T_RV_RETURN return_path
                                         );


/**
 * @name bae_get_output_channels
 * Gets the output channels.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_get_output_channels (
                                      T_RV_RETURN return_path
                                      );


/**
 * @name bae_change_voice_limit
 * Changes the  voice limit for a player.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_change_player_voice_limit (
                                     T_BAE_PLAYER_ID bae_player, 
                                     int voice_limit,
                                     T_RV_RETURN return_path
                                     );

/**
 * @name bae_get_voice_limit
 * Gets the player voice limit.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_get_player_voice_limit (
                                  T_BAE_PLAYER_ID bae_player, 
                                  T_RV_RETURN return_path
                                  );

/**
 * @name bae_pause_player
 * Pause the playback
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_PAUSE_STATUS_MSG message which contains position in milli sec.
 */
T_BAE_RETURN bae_pause_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_resume_player
 * Resume the playback
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_RESUME_STATUS_MSG message which contains position in milli sec.
 */
T_BAE_RETURN bae_resume_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_get_position_player
 * Get position of the player in 
 *
 * @param   player  the pointer to the player.
 * @direct return   the return value indicates if this function executed correctly.
 * @indirect return BAE_GET_POSITION_STATUS_MSG message which contains position in milli sec.
 
 */
T_BAE_RETURN bae_get_position_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_set_position_player
 * Set position of the player.
 *
 * @param   player  the pointer to the player.
 * @param   pos position in milli sec.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_SET_POSITION_STATUS_MSG message.
 */
T_BAE_RETURN bae_set_position_player (
                                 T_BAE_PLAYER_ID player,
                                 unsigned long pos,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_get_duration_player
 * Get the total duration of the content file
 * file to allow very quick start when bae_player_start is called.
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_GET_DURATION_STATUS_MSG message which contains duration in milli sec.
 */
T_BAE_RETURN bae_get_duration_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_set_volume_player
 * Sets volume of the player. 
 * file to allow very quick start when bae_player_start is called.
 *
 * @param   player  the pointer to the player.
 * @param   volume: 0 (0% or silence) to 1000, with 1000 meaning 100%, or full, normal volume.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_SET_VOLUME_STATUS_MSG message.
 */
T_BAE_RETURN bae_set_volume_player (
                                 T_BAE_PLAYER_ID player,
                                 long volume,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_get_volume_player
 * gets volume of the player. 
 * file to allow very quick start when bae_player_start is called.
 *
 * @param   player  the pointer to the player.
 * @return          the return value indicates if this function executed correctly.
 * @indirect return BAE_SET_VOLUME_STATUS_MSG message which contains volume (0 to 1000).
 */
T_BAE_RETURN bae_get_volume_player (
                                 T_BAE_PLAYER_ID player,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_add_bank
 * Add a bank file.
 *
 * @param   filePath    path to the bank file, must be dls
 * @param   location to bank location to.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_add_bank (
                                 mbChar filePath[],
                                 T_BAE_BANK_LOCATION location,
                                 T_RV_RETURN return_path
                                 );


/**
 * @name bae_remove_bank
 * Remove the last added bank.
 *
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_remove_bank (
                                     T_RV_RETURN return_path
                                     );

/**
 * @name bae_engage_drc
 * engages or disengages the drc.
 *
 * @param   player  the pointer to the player.
 * @param   engage  boolean that engages or disengages the.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_engage_drc (
                             T_BAE_PLAYER_ID bae_player, 
                             BOOL engage,
                             T_RV_RETURN return_path
                             );

/**
 * @name bae_is_drc_engaged
 * returns a boolean of the DRC engaged.
 *
 * @param   player  the pointer to the player.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_is_drc_engaged (
                                 T_BAE_PLAYER_ID bae_player, 
                                  T_RV_RETURN return_path
                                  );

/**
 * @name bae_set_drc_parameters
 * sets the drc parameters.
 *
 * @param   player  the pointer to the player.
 * @param   params  structure that contains the drc parameters
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_set_drc_parameters (
                             T_BAE_PLAYER_ID bae_player, 
                             T_BAE_DRC_PARAM params,
                             T_RV_RETURN return_path
                             );

/**
 * @name bae_get_drc_parameters
 * gets the drc parameters.
 *
 * @param   player  the pointer to the player.
 * @param   return_path return path though which bae may send messages.
 * @return          the return value indicates if this function executed correctly.
 */
T_BAE_RETURN bae_get_drc_parameters (
                                     T_BAE_PLAYER_ID bae_player, 
                                     T_RV_RETURN return_path
                                     );

/**
 * @name bae_enable_midi_normalizer
 * enable/disable midi normalizer
 *
 * @param   enable. TRUE means enable, FALSE means disable the midi normalization.
 * @direct return   the return value indicates if this function executed correctly.
 * @indirect return T_BAE_ENABLE_MIDI_NORMALIZER_STATUS_MSG message which contains position in milli sec.
 
 */
T_BAE_RETURN bae_enable_midi_normalizer (
                                 BOOL enabled,
                                 T_RV_RETURN return_path
                                 );

/**
 * @name bae_audio_data_request
 * This function is called by the audio system to get audio data.
 * When the audio data is ready, a messages is sent over the return path.
 *
 */
T_BAE_RETURN bae_audio_data_request (
                                     void *audio_data_buffer,
                                     int size_of_audio_data_buffer
                                     );


/**************************************************************************************
        NEW synchronous BAE API functions
**************************************************************************************/

#ifdef __cplusplus
}
#endif


#endif /*__BAE_API_H_*/

