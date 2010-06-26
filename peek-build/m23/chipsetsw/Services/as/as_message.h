/**
 * @file
 *
 * @brief Declaration of messages exchanged with AS.
 *
 * This file declares all data structures :
 * -# used to send messages to the AS SWE,
 * -# the AS SWE can receive.
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

#ifndef __AS_MESSAGE_H_
#define __AS_MESSAGE_H_


#include "rv/rv_general.h"

#include "as/as_api.h"


/**
 * @brief The message offset of the AS SWE.
 *
 * The message offset must differ for each SWE so that each message
 * can be uniquely identified in the system.
 */
#define AS_MESSAGE_OFFSET BUILD_MESSAGE_OFFSET(AS_USE_ID)


// Messages.

/// The start request message identifier.
#define AS_START_REQ (AS_MESSAGE_OFFSET | 0x01)
/// The start indication message identifier.
#define AS_START_IND (AS_MESSAGE_OFFSET | 0x02)
/// The stop request message identifier.
#define AS_STOP_REQ  (AS_MESSAGE_OFFSET | 0x10)
/// The  resume request message identifier
#define AS_RESUME_REQ  (AS_MESSAGE_OFFSET | 0x11)
/// The stop indication message identifier.
#define AS_STOP_IND  (AS_MESSAGE_OFFSET | 0x20)

// Layer1_BT_OMAPS00152447
#define AS_PAUSE_IND  (AS_MESSAGE_OFFSET | 0x30)



// Private messages.

/// The internal stop (end of media) request message identifier.
#define AS_INTERNAL_STOP_REQ  (AS_MESSAGE_OFFSET | 0x100)

#define AS_INTERNAL_BAE_IND  (AS_MESSAGE_OFFSET | 0x200)

/// The pause request message identifier
#define AS_PAUSE_REQ  (AS_MESSAGE_OFFSET | 0x1000)

/// The progress bar indication message identifier.
#define AS_PROBAR_IND  (AS_MESSAGE_OFFSET | 0x101)   // 0x11 reserved for resume request 
/// The Fast Forward, Rewind indication message identifier.
#define AS_FORREW_REQ  (AS_MESSAGE_OFFSET | 0x110)   //0x11 for resume, 0x101 for progress bar

#define AS_BT_CFG_REQ (AS_MESSAGE_OFFSET | 0x50)   

// Structures.

/// The structure conveying the start request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR         header;
    /// The player type required to play the melody data.
    T_AS_PLAYER_TYPE player_type;
    /// The volume of the melody playback.
    T_AS_VOLUME      volume;
    /// Whether the player has to loop on the melody or not.
    BOOLEAN          loop;
    /// Whether progress bar support is required or not
    BOOLEAN                 play_bar_on;
    /// The event that triggered the playback (NONE=player, IC/SMS/ALARM=ringer).
    T_AS_RINGER_MODE ringer_mode;
    /// Where the data resides (FILE or MEMORY).
    T_AS_LOCATION    data_location;
    /// Either a filename or a memory buffer.
    T_AS_PLAYER_DATA data;
    /// The Riviera return path where the START_IND and/or STOP_IND is sent.
    T_RV_RETURN_PATH rp;
}
T_AS_START_REQ;


/// The structure conveying the stop request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR         header;
    /// The player type required to play the melody data.
    T_AS_PLAYER_TYPE player_type;
    /// The volume of the melody playback.
    T_AS_VOLUME      volume;
    /// Whether the player has to loop on the melody or not.
    BOOLEAN          loop;
    /// Whether progress bar support is required or not
    BOOLEAN                 play_bar_on;
    /// The event that triggered the playback (NONE=player, IC/SMS/ALARM=ringer).
    T_AS_RINGER_MODE ringer_mode;
    /// Where the data resides (FILE or MEMORY).
    T_AS_LOCATION    data_location;
    /// MP3 play context
#if (AS_OPTION_MP3 == 1)
    T_AS_PLAYER_MP3_PARAMS  mp3;
#endif
    /// Set the current offset of file pointer in current playing file 
    unsigned long file_offset;
    /// Either a filename or a memory buffer.
    T_AS_PLAYER_DATA data1;
    /// The Riviera return path where the START_IND and/or STOP_IND is sent.
    T_RV_RETURN_PATH rp;
    /// interrupt status in pause state
    BOOLEAN pause_interrupted;
    /// third play comes
    INT32  third_play;
	
}
T_AS_SAVE_CONTEXT;
/// The structure conveying the stop request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The status of the start request. Possible values are:
    /// - AS_OK  The start request was successful.
    ///          The requested melody is currently playing.
    /// - AS_NOT_ALLOWED  The start request is not allowed when the AS
    ///                   is in transient states (SETTING/RESTORING).
    ///                   Any operation is quesciently stopped so the new
    ///                   future state is IDLE.
    /// - AS_DENIED  The start request has no priority over the current playback.
    /// - AS_*  Any other error described in the as_api.h file.
    T_AS_RET status;
    /// Whether the current start request interrupted a previous player/ringer or not.
    BOOLEAN  interrupted;
}
T_AS_START_IND;


/// The structure conveying the start indication data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR         header;
    /// The Riviera return path where the START_IND and/or STOP_IND is sent.
    T_RV_RETURN_PATH rp;
}
T_AS_STOP_REQ;


/// The structure conveying the internal pause request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The Riviera return path where the START_IND and/or STOP_IND is sent.
   T_RV_RETURN_PATH rp;

}
T_AS_PAUSE_REQ;

/// The structure conveying the resume request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The Riviera return path where the START_IND and/or STOP_IND is sent.
   T_RV_RETURN_PATH rp;

}
T_AS_RESUME_REQ;

/// The structure conveying the fast forward or rewind data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR         header;
    /// The Riviera return path where recover mode is handled.
    T_RV_RETURN_PATH rp;
    /// media skip time
    INT16  media_skip_time;
   /// FORWARD/REWIND
   UINT32 forrew;
	
}
T_AS_FORREW_REQ;

/// The structure conveying the stop indication data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The status of the stop request. Possible values are:
    /// - AS_OK  The stop request was successful.
    ///          The AS is back to IDLE state.
    /// - AS_NOT_ALLOWED  The stop request is not allowed when the AS
    ///                   is in transient states (SETTING/RESTORING).
    ///                   Any operation is quesciently stopped so the new
    ///                   future state is IDLE.
    /// - AS_*  Any other error described in the as_api.h file.
    T_AS_RET status;
    /// Whether the stop indication is sent due to an end of media reached or not.
    BOOLEAN  end_of_media;
    UINT32 file_size; //Daisy tang added for Real Resume feature 20071107
}
T_AS_STOP_IND;

/// The structure conveying the progress bar information
typedef struct
{
   /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The status of the stop request. Possible values are:
    /// - AS_OK  The stop request was successful.
    ///          The AS is back to IDLE state.
    /// - AS_NOT_ALLOWED  The stop request is not allowed when the AS
    ///                   is in transient states (SETTING/RESTORING).
    ///                   Any operation is quesciently stopped so the new
    ///                   future state is IDLE.
    /// - AS_*  Any other error described in the as_api.h file.
    T_AS_RET status;
    UINT16  u32TotalTimePlayed; /* Current time played in seconds*/
    UINT16 media_type;
    UINT16 u16totalTimeEst; /*The total song/file duration in seconds.  */
}
T_AS_PROBAR_IND_AAC;


/// The structure conveying the progress bar information
typedef struct
{
   /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    /// The status of the stop request. Possible values are:
    /// - AS_OK  The stop request was successful.
    ///          The AS is back to IDLE state.
    /// - AS_NOT_ALLOWED  The stop request is not allowed when the AS
    ///                   is in transient states (SETTING/RESTORING).
    ///                   Any operation is quesciently stopped so the new
    ///                   future state is IDLE.
    /// - AS_*  Any other error described in the as_api.h file.
    T_AS_RET status;
    UINT16  u32TotalTimePlayed; /* Current time played in seconds*/
    UINT16 media_type;
    UINT32 u32totalTimeEst; /*The total song/file duration in seconds.  */
}
T_AS_PROBAR_IND_MP3;
	


/// The structure conveying the internal stop request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    // There is no return path as this is an internal purpose message.
}
T_AS_INTERNAL_STOP_REQ;

/// The structure conveying the internal stop request data.
typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR header;
    // There is no return path as this is an internal purpose message.
}
T_AS_INTERNAL_BAE_IND;

typedef struct
{
    /// The Riviera header. Required by the Riviera frame.
    T_RV_HDR         header;
    /// The Riviera return path where recover mode is handled.
    T_RV_RETURN_PATH rp;
    /// bt status
    BOOL connected_status;
}T_AS_BT_CFG_REQ;
#endif // __AS_MESSAGE_H_
