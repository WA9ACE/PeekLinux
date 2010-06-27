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

#ifndef __AS_API_H_
#define __AS_API_H_

/******** to be removed ********/
#ifndef AS_RFS_API
#define AS_RFS_API  1
#endif
/******** to be removed ********/

#include "swconfig.cfg"

#include "rvf/rvf_api.h"

#include "rv/rv_general.h"
#include "Audio/audio_api.h"
#if(AS_RFS_API == 1)
#include "rfs/rfs_api.h"
#endif

/**
 * @defgroup as_ret AS Return type and values.
 *
 * They are based on the standard RV return types. Extension start from
 * offset 100.
 * @{
 */
typedef enum 
{
/// The operation was successful.
AS_OK             = RV_OK,
/// The operation is not supported.
AS_NOT_SUPPORTED   = RV_NOT_SUPPORTED,
/// Not enough memory to allocate an object.
AS_MEMORY_ERR    =  RV_MEMORY_ERR,
/// An internal error occurred.
AS_INTERNAL_ERR    = RV_INTERNAL_ERR,
/// A parameter has an invalid value.
AS_INVALID_PARAM =  100,
// The AS software entity is not yet ready, try later.
AS_NOT_READY,       
/// The operation was already performed, or the object already exists.
 AS_ALREADY,         
/// A message was not successfully sent.
	AS_MESSAGING_ERR,   
/// An error ocurred while trying to access an object on the FFS.
	AS_FFS_ERR,         
/// The underlying player was not able to perform an operation.
	AS_PLAYER_ERR      ,
/// The start request was denied due to the fact another player with
/// higher priority is already playing.
	AS_DENIED          ,
/// An error occurred in the audio layer.
	AS_AUDIO_ERR     ,  
/// The start/stop request was not allowed from the current AS state.
/// The recover mechanism is started to go back to the stable IDLE state.
	AS_NOT_ALLOWED   ,  
/// An error ocurred while trying to access an object on the LFS.
	AS_LFS_ERR         
}
T_AS_RET;
/// The AS return type.




/** @} */


/**
 * @defgroup as_player_type AS Player type and values.
 *
 * @{
 */

/// The type of player to use when a play or ring operation is requested.
typedef INT8 T_AS_PLAYER_TYPE;

/// Player type is not defined.
#define AS_PLAYER_TYPE_NONE   0

/// Player type used to set or get the player common parameters.
#define AS_PLAYER_TYPE_COMMON AS_PLAYER_TYPE_NONE

/// Player type used to select the Buzzer simple player.
#define AS_PLAYER_TYPE_BUZZER 1

/// Player type used to select the Midi synthetizer player.
#define AS_PLAYER_TYPE_MIDI   2

// Player type used to select the MP3 player.
#define AS_PLAYER_TYPE_MP3    3

// Player type used to select the AAC player.
#define AS_PLAYER_TYPE_AAC    4

// Player type used to select the E1 Melody player.
#define AS_PLAYER_TYPE_E1_MELODY 5

/// Maximum authorized value for the player type. Used to check player type range.
#define AS_PLAYER_TYPE_MAX    AS_PLAYER_TYPE_E1_MELODY

/** @} */


/**
 * @defgroup as_volume AS Volume type and values.
 *
 * @{
 */

/// The different discrete volume values.
typedef INT8 T_AS_VOLUME;

/// The player/ringer is silent.
#define AS_VOLUME_SILENT 0

/// The player/ringer uses a low volume.
#define AS_VOLUME_LOW    1

/// The player/ringer uses a medium volume.
#define AS_VOLUME_MEDIUM 2

/// The player/ringer uses a high volume (the highest one).
#define AS_VOLUME_HIGH   3

/** @} */
/**
 * @defgroup forward rewind support and values.
 *
 * @{
 */

/// The audio device on which an audio stream is directed to or from.
typedef INT8 T_AS_FORREW;

/// The speaker output device.
#define FORWARD 1

/// The headset output device.
#define REWIND 2


/**
 * @defgroup play bar support and values.
 *
 * @{
 */

/// The audio device on which an audio stream is directed to or from.
typedef INT8 T_AS_PROBAR;

/// The speaker output device.
#define PLAYBAR_NOT_SUPPORTED 0

/// The headset output device.
#define PLAYBAR_SUPPORTED 1


/**
 * @defgroup forward rewind support and values.
 *
 * @{
 */

/// The audio device on which an audio stream is directed to or from.

/// The speaker output device.
#define FORWARD 1

/// The headset output device.
#define REWIND 2


/** @} */

/**
 * @defgroup Play Bar Support and values.
 *
 * @{
 */

/// The audio device on which an audio stream is directed to or from.
typedef INT8 T_AS_DEVICE;

/// The speaker output device.
#define AS_DEVICE_SPEAKER 0

/// The headset output device.
#define AS_DEVICE_HEADSET 1

/** @} */



/**
 * @defgroup as_location AS Location type and values.
 *
 * @{
 */

/// The location of the data to play/ring.
typedef INT8 T_AS_LOCATION;

/// Data is located in a file stored on the FFS.
#define AS_LOCATION_FILE   0

/// Data is located into memory.
#define AS_LOCATION_MEMORY 1

/// Data is located in a file stored on the FFS.
#define AS_LOCATION_LINEAR_FILE   2

/** @} */


/**
 * @defgroup as_ringer_mode AS Ringer mode type and values.
 *
 * @{
 */

/// The event that will trigger the ringer.
typedef INT8 T_AS_RINGER_MODE;

/// The ringer is not active. This means the player is active.
#define AS_RINGER_MODE_NONE  -1

/// The ringer is triggered by an alarm clock.
#define AS_RINGER_MODE_ALARM 0

/// The ringer is triggered by an incoming SMS.
#define AS_RINGER_MODE_SMS   1

/// The ringer is triggered by an incoming call.
#define AS_RINGER_MODE_IC    2

/// The size of the internal ringer array (private use).
#define AS_RINGER_MODE_ARRAY_SIZE  3

/** @} */


/**
 * @defgroup as_mp3_channel AS MP3 channels mode type and values.
 *
 * @{
 */

 /// The mp3 channel whether it is Mono or Stereo.
typedef INT8 T_AS_MP3_CHANNEL;

/// The MP3 is played as Mono
#define AS_MP3_MONO            0

/// The MP3 is played as Stereo
#define AS_MP3_STEREO         1

/**
 * @defgroup as_aac_channel AS AAC channels mode type and values.
 *
 * @{
 */

 /// The AAC channel whether it is Mono or Stereo.
typedef INT8 T_AS_AAC_CHANNEL;

/// The AAC is played as Mono
#define AS_AAC_MONO            0

/// The AAC is played as Stereo
#define AS_AAC_STEREO         1

/**
 * @defgroup as_E1_Melody_loopback as_E1_Melody_mode AS E1 Melody loopback and mode type and values.
 *
 * @{
 */

/// The E1_Melody loopback whether it is No-loopback or Loopback.
typedef INT8 T_AS_E1_MELODY_LOOPBACK;

/// The E1_Melody is played without loopback
#define AS_E1_MELODY_NO_LOOPBACK            0

/// The E1_Melody is played with loopback
#define AS_E1_MELODY_LOOPBACK         1

/// The E1_Melody mode whether it is Game-Mode or Normal-Mode.
typedef INT8 T_AS_E1_MELODY_MODE;

/// The E1_Melody is played in Game-Mode
#define AS_E1_MELODY_GAME_MODE            0

/// The E1_Melody is played in Normal-Mode
#define AS_E1_MELODY_NORMAL_MODE         1
/// play bar support typedef struct{    /// The play bar support is there or not;    T_AS_PROBAR progress_bar;}T_AS_PLAYER_PROBAR_PARAMS;
/// The common parameters for the player.
typedef struct
{
    /// The output device to play the audio stream.
    T_AS_DEVICE output_device;
}
T_AS_PLAYER_COMMON_PARAMS;



/// The parameters for the midi player.
typedef struct
{
    /// The limit number of simultaneous played voices.
    INT16 voice_limit;
    /// The number of output channels (mono=1 or stereo=2).
    INT16 output_channels;
    /// play bar option
//    BOOLEAN play_bar_on;
    INT16 media_skip_time;

}
T_AS_PLAYER_MIDI_PARAMS;

/// The parameters for the MP3 player.
typedef struct
{
	/// channel configuration
	BOOLEAN   mono_stereo;
	/// size of the file where the melody must start
	UINT32    size_file_start;
	/// play bar option
	//BOOLEAN play_bar_on;
}
T_AS_PLAYER_MP3_PARAMS;

typedef T_AS_PLAYER_MP3_PARAMS T_AS_PLAYER_AAC_PARAMS;

/// The parameters for the E1-Melody Player.
typedef struct
{
	/// loopback configuration
	BOOLEAN  loopback;
	/// size of the file where the melody must start
	BOOLEAN melody_mode;
}
T_AS_PLAYER_E1_MELODY_PARAMS;

/// The union gathering the different player type parameters.
typedef union
{
    /// Player common parameters.
    T_AS_PLAYER_COMMON_PARAMS common;
    /// Midi player parameters.
    T_AS_PLAYER_MIDI_PARAMS   midi;
    // MP3 player parameters.
    T_AS_PLAYER_MP3_PARAMS    mp3;
    // AAC player parameters.
    T_AS_PLAYER_AAC_PARAMS    aac;
    // E1_Melody player parameters.
    T_AS_PLAYER_E1_MELODY_PARAMS e1_melody;
}
T_AS_PLAYER_PARAMS;

/// Maximum length of the audio data filename (excluded EOS character).
#define AS_FILENAME_MAX_LEN    80     /* 47  */

/// The player data location. This can be either a filename of a memory buffer.
typedef union
{
    /// The name of the file which contains the player data.
#if(AS_RFS_API == 1)
	T_WCHAR    filename[AS_FILENAME_MAX_LEN + 1];
#else
	char       filename[AS_FILENAME_MAX_LEN + 1];
#endif

    /// The anonymous structure that describes the memory buffer.
    struct
    {
        /// The buffer address.
        const UINT32* address;
        /// The buffer size.
        UINT32        size;
    } memory;
}
T_AS_PLAYER_DATA;


/**
 * @brief The ringer information.
 *
 * This ringer information is used to get all parameters of a specific
 * ringer mode. It is also used internally to keep a cache of the
 * different ringer mode configuration files (IC, SMS and ALARM).
 */
typedef struct
{
    /// The player type of the data to play.
    T_AS_PLAYER_TYPE player_type;
    /// The volume at which the data has to be played (ringed).
    T_AS_VOLUME      volume;
    /// The location of the data (either file or memory).
    T_AS_LOCATION    data_location;
    /// The melody data to play.
    T_AS_PLAYER_DATA data;
}
T_AS_RINGER_INFO;




/**
 * @defgroup as_api AS API public function prototypes.
 *
 * @{
 */

/// Set the GPRS mode for the Audio Services.
extern T_AS_RET
as_set_gprs_mode(BOOLEAN gprs_mode);

/// Deduce the player type from the filename extension.
#if(AS_RFS_API == 1)
extern T_AS_RET
as_deduce_player_type(const T_WCHAR*       filename,
                      T_AS_PLAYER_TYPE* player_type);
#else
extern T_AS_RET
as_deduce_player_type(const char*       filename,
                      T_AS_PLAYER_TYPE* player_type);
#endif

/// Set parameters of the player selected by the player type.
extern T_AS_RET
as_player_set_params(T_AS_PLAYER_TYPE          player_type,
                     const T_AS_PLAYER_PARAMS* params);

/// Get parameters of the player selected by the player type.
extern T_AS_RET
as_player_get_params(T_AS_PLAYER_TYPE           player_type,
                     const T_AS_PLAYER_PARAMS** params);

/// Set the filename to play,and its type, for the given ringer mode.

#if(AS_RFS_API == 1)
extern T_AS_RET
as_ringer_set_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const T_WCHAR*      filename);
#else
extern T_AS_RET
as_ringer_set_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const char*      filename);
#endif

#if (AS_OPTION_LINEAR_FS == 1)
/// Set the linear filename to play,and its type, for the given ringer mode.
#if(AS_RFS_API == 1)
extern T_AS_RET
as_ringer_set_linear_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const T_WCHAR*      filename);
#else
extern T_AS_RET
as_ringer_set_linear_file(T_AS_RINGER_MODE ringer_mode,
                   T_AS_PLAYER_TYPE player_type,
                   const char*      filename);
#endif

/// Play the given filename from LFS according to the different parameters.
#if(AS_RFS_API == 1)
extern T_AS_RET
as_play_linear_file(T_AS_PLAYER_TYPE        player_type,
             const T_WCHAR*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             const T_RV_RETURN_PATH* rp);
#else
extern T_AS_RET
as_play_linear_file(T_AS_PLAYER_TYPE        player_type,
             const char*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             const T_RV_RETURN_PATH* rp);
#endif
#endif

/// Set the memory address to play, its size and its type, for the given ringer mode.
extern T_AS_RET
as_ringer_set_memory(T_AS_RINGER_MODE ringer_mode,
                     T_AS_PLAYER_TYPE player_type,
                     const UINT32*    address,
                     UINT32           size);

/// Set the volume for the given ringer mode.
extern T_AS_RET
as_ringer_set_volume(T_AS_RINGER_MODE ringer_mode,
                     T_AS_VOLUME      volume);

/// Get the ringer information for the given ringer mode.
extern T_AS_RET
as_ringer_get(T_AS_RINGER_MODE         ringer_mode,
              const T_AS_RINGER_INFO** info);

/// Play the given filename according to the different parameters.
#if(AS_RFS_API == 1)
extern T_AS_RET
as_play_file(T_AS_PLAYER_TYPE        player_type,
             const T_WCHAR*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             BOOLEAN                 play_bar_on,
             const T_RV_RETURN_PATH* rp);
#else
extern T_AS_RET
as_play_file(T_AS_PLAYER_TYPE        player_type,
             const char*             filename,
             T_AS_VOLUME             volume,
             BOOLEAN                 loop,
             BOOLEAN                 play_bar_on,
             const T_RV_RETURN_PATH* rp);
#endif

/// Play the given memory address according to the different parameters.
extern T_AS_RET
as_play_memory(T_AS_PLAYER_TYPE        player_type,
               const UINT32*           address,
               UINT32                  size,
               T_AS_VOLUME             volume,
               BOOLEAN                 loop,
               const T_RV_RETURN_PATH* rp);

T_AS_RET
as_forward(INT16  skip_time,const T_RV_RETURN_PATH* rp);

T_AS_RET
as_rewind(INT16  skip_time,const T_RV_RETURN_PATH* rp);

/// forward or rewind the play position of the media by given duration.
extern T_AS_RET
as_forrew(INT16  skip_time,const T_RV_RETURN_PATH* rp,INT32 FORREW);
/// Start the ringer according to the given ringer mode and loop option.
extern T_AS_RET
as_ring(T_AS_RINGER_MODE        ringer_mode,
        BOOLEAN                 loop,
        const T_RV_RETURN_PATH* rp);

// Pause the given filename according to the different parameters
extern T_AS_RET
as_pause(const T_RV_RETURN_PATH* rp);

// Resume playing the given filename according to the different parameters
extern T_AS_RET
as_resume(const T_RV_RETURN_PATH* rp);
/// Stop any playing or ringing operation.
extern T_AS_RET
as_stop(const T_RV_RETURN_PATH* rp);

extern int as_get_state(void); //Daisy tang added for Real Resume feature 20071107 
#if(AS_RFS_API == 1)
int wstrncmp(const T_WCHAR *string1, const T_WCHAR *string2, UINT16 n);
#endif

/** @} */

// Layer_BT_OMAPS00152447 
extern T_AS_RET as_bt_cfg(BOOL connected_status, const T_RV_RETURN_PATH* rp);


#endif // __AS_API_H_
