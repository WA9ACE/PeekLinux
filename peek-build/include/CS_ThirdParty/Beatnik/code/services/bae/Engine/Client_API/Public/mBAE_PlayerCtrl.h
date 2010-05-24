/******************************************************************************
**
**	mBAE_PlayerCtrl.h
**
**	The transport control through which all players are manipulated.
**
**	(c) Copyright 2002-2004 Beatnik, Inc., All Rights Reserved.
**
**	Beatnik products contain certain trade secrets and confidential and
**	proprietary information of Beatnik.  Use, reproduction, disclosure
**	and distribution by any means are prohibited, except pursuant to
**	a written license from Beatnik. Use of copyright notice is
**	precautionary and does not imply publication or disclosure.
**
**	Restricted Rights Legend:
**	Use, duplication, or disclosure by the Government is subject to
**	restrictions as set forth in subparagraph (c)(1)(ii) of The
**	Rights in Technical Data and Computer Software clause in DFARS
**	252.227-7013 or subparagraphs (c)(1) and (2) of the Commercial
**	Computer Software--Restricted Rights at 48 CFR 52.227-19, as
**	applicable.
**
**	Notes:
**
**	2002.01.31 AER	Created file.
**	2002.03.29 AER	Heavily revised and improved position callbacks.
**	2002.04.02 msd	added mbPreroll()
**	2002.04.12 AER	Updated mbGetDuration.
**	2002.04.24 SME	Removed objectID param from mbArePlayerCallbacksSupported.
**	2002.06.09 AER	Converted time units from float to milliseconds.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.06.20 AER	mbAreCallbacksSupported replaces all other callback support
**					queries.
**	2002.06.21 AER	Replaced usage of mbMilliTime with mbTime.
**	2002.06.23 AER	Renamed mbIsDone mbIsStopped.
**	2002.06.30 AER	Cleaned up some documentation.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2003.02.04 msd	Moved all type declarations to mBAE_Types.h
**	2003.11.26 SME	mbFade only supports asynchronous fade now.
**	2004.01.28 SME	Added mbGetPositionInTimeCode.
**	2004.01.30 AER	Deprecated mbAreMarkersSupported.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.03.04 AER	Replaced use of mbPlayer_Not_Playing with mbNot_Playing.
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_PLAYERCTRL_H_
#define _MBAE_PLAYERCTRL_H_

#include "mBAE_Types.h"


// mbPreroll
// --------------------------------------------------
// Prerolls the player.
// This function performs any tasks required before playback begins so that
// Start can execute more quickly (if this function is not called explicitly,
// it is called internally when Start is called).
//
mbResult 			mbPreroll(
								mbObjectID playerID);

// mbStart
// --------------------------------------------------
// Starts the player once data has been loaded.
// Returns mbNot_Loaded if the player has no data loaded.
//
mbResult			mbStart(
								mbObjectID playerID);

// mbStop
// --------------------------------------------------
// Stops the player.
// 'fadeTime' specifies how long (in milliseconds) the player should
// asynchronously fade out (0 specifies no fade).
// Returns mbNot_Loaded if the player has no data loaded.
//
mbResult			mbStop(
								mbObjectID playerID,
								mbTime fadeTime);

// mbIsPlaying
// --------------------------------------------------
// Returns whether the player is still active (playing or paused).
//
mbBool				mbIsPlaying(
								mbObjectID playerID,
								mbResult *outResult);

// mbIsStopped
// --------------------------------------------------
// Returns whether the player is neither playing nor paused.
//
mbBool				mbIsStopped(
								mbObjectID playerID,
								mbResult *outResult);

// mbIsPaused
// --------------------------------------------------
// Returns whether the player is paused.
//
mbBool				mbIsPaused(
								mbObjectID playerID,
								mbResult *outResult);

// mbPause
// --------------------------------------------------
// Pauses the player; call Resume to resume playback.
// Returns mbNot_Playing if the player is not actually active.
// Note that mbStart will not resume a paused player.
//
mbResult			mbPause(
								mbObjectID playerID);

// mbResume
// --------------------------------------------------
// Resume playback of the player from the position it was paused.
// Returns mbNot_Playing if the player is not actually active
//
mbResult			mbResume(
								mbObjectID playerID);

// mbFade
// --------------------------------------------------
// Asynchronously ramps the volume of the player from the current volume
// to 'endVolume' over a period of 'fadeTime'.
// Returns mbNot_Playing if the player is not actually active.
//
mbResult			mbFade(
								mbObjectID playerID,
								mbMilliUnit endVolume,
								mbTime fadeTime);

// mbEnableLooping
// --------------------------------------------------
// Enables or disables looping of content of the player.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbResult			mbEnableLooping(
								mbObjectID playerID,
								mbBool isEnabled);

// mbIsLoopingEnabled
// --------------------------------------------------
// Returns whether looping is enables on the player.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbBool				mbIsLoopingEnabled(
								mbObjectID playerID,
								mbResult *outResult);

// mbGetCurrentLoopNum
// --------------------------------------------------
// Returns the number of times the player has looped.
// Returns 0 if looping is disabled or unsupported, or the player is stopped.
//
mbInt32				mbGetCurrentLoopNum(
								mbObjectID playerID,
								mbResult *outResult);

// mbSetLoopCount
// --------------------------------------------------
// Sets the number of times to play the content of the player (e.g., 1 plays
// the content once, 10 plays it 10 times). Passing 0 will cause the player
// to loop indefinitely (e.g., until mbStop is called).
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbResult			mbSetLoopCount(
								mbObjectID playerID,
								mbInt32 loopCount);

// mbGetLoopCount
// --------------------------------------------------
// Returns the player's loop count setting.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbInt32				mbGetLoopCount(
								mbObjectID playerID,
								mbResult *outResult);

// mbGetDuration
// --------------------------------------------------
// Returns the duration of the player in milliseconds.
// This function ignores the effect of loops, client-controlled tempo,
// transpose, etc.
// Returns 0.0 if the player has no data loaded.
// Returns 0.0 and mbValue_Unreliable if the duration is not known (as is the
// case with raw callback streams or any MPEG-base callback streams).
//
mbTime				mbGetDuration(
								mbObjectID playerID,
								mbResult *outResult);

// mbSetPosition
// --------------------------------------------------
// Sets the position of the player in milliseconds.
// Returns mbNot_Loaded if the player has no data loaded.
// Returns mbOperation_Not_Enabled on players that do not support the
// setting of position.
//
mbResult			mbSetPosition(
								mbObjectID playerID,
								mbTime position);

// mbGetPosition
// --------------------------------------------------
// Returns the position of the player in milliseconds.
// Returns 0.0 if the player has no data loaded.
//
mbTime				mbGetPosition(
								mbObjectID playerID,
								mbResult *outResult);

// mbSetTimeCodeFormat
// --------------------------------------------------
// Sets the format that calls to mbGetPositionInTimeCode will use when
// populating mbTimeCode structs.
//
mbResult			mbSetTimeCodeFormat(
								mbObjectID playerID,
								mbTimeCodeFormat format);

// mbGetTimeCodeFormat
// --------------------------------------------------
// Returns the format currently used by calls to mbGetPositionInTimeCode.
//
mbTimeCodeFormat	mbGetTimeCodeFormat(
								mbObjectID playerID,
								mbResult *outResult);

// mbGetPositionInTimeCode
// --------------------------------------------------
// Returns the position of the player in an mbTimeCode struct using
// mbTimeCodeFormat specified via a previous call to mbSetTimeCodeFormat.
//
// The difference in time between the position of a player and when the audio
// that was rendered for that position is actually heard is called latency.
// Latency is composed of mobileBAE internal factors and external audio output
// hardware factors.
// MidiFilePlayers and RingtoneFilePlayers have an internal minimum latency
// equal to a constant renderer buffer size time, which by default is 10ms.
// All players have an additional variable internal latency whose maximum size
// is equal to one sample frame less than the constant renderer buffer size.
// This latency is introduced due to buffer matching that may be required by
// the internal mixer to accommodate output hardware implementations (if they
// ask for a number of sample frames that is not an integral multiple of the
// internal renderer buffer size - to avoid this matching, PAL implementations
// should ask for a multiple of 10ms per call to ACF_FillBuffer).
// External latency is PAL implementation and audio hardware dependent.
// In general, the value returned by mbGetOutputHardwareAudioLatency can be
// treated as a maximum external latency.
//
mbResult			mbGetPositionInTimeCode(
								mbObjectID playerID,
								mbTimeCode *outTimeCode);



// Player Callback Extension
// ****************************************************************************
// Under some builds of mobileBAE, callbacks may be enabled.
// This extension allows client's to receive callbacks when certain general
// player events occur.
// All Player Callback functions will fail with a result of 
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support callbacks (call mbIsFeatureSupported with
// mbFeature_Callbacks to verify).
// ****************************************************************************



// mbSetPlayerLoopCallback
// --------------------------------------------------
// Set a callback to be executed when the player reaches it's loop point.
// The callback must execute synchronously.
// Pass 'callback' as NULL to disable loop callbacks.
// The loop callback has an mbBool parameter that indicates whether the player
// should loop to the beginning of the player's content and begin again.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbResult			mbSetPlayerLoopCallback(
								mbObjectID playerID,
								mbLoopCallback callback,
								void *context);

// mbGetPlayerLoopCallback
// --------------------------------------------------
// Returns the loop callback used by the player, or NULL if none is set.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbLoopCallback		mbGetPlayerLoopCallback(
								mbObjectID playerID,
								mbResult *outResult);

// mbAddPlayerPositionCallback
// --------------------------------------------------
// Adds a position callback to the player that executes when the playback
// position reaches the indicated playback time.
// It is possible to set callbacks on numerous positions in the player.
// It is possible to use the same callback with multiple positions.
//
mbResult			mbAddPlayerPositionCallback(
								mbObjectID playerID,
								mbPositionCallback callback,
								void *context,
								mbTime position,
								mbBool async);

// mbRemovePlayerPositionCallback
// --------------------------------------------------
// Removes the specified callback associated with the specified position from
// the player.
//
mbResult			mbRemovePlayerPositionCallback(
								mbObjectID playerID,
								mbTime position);

// mbSetPlayerTerminationCallback
// --------------------------------------------------
// Set a callback to be executed when the player stops either by coming to the
// end of its content or as a result of a call to mbPlayer_Stop.
// Pass 'callback' as NULL to disable termination callbacks.
//
mbResult			mbSetPlayerTerminationCallback(
								mbObjectID playerID,
								mbTerminationCallback callback,
								void *context,
								mbBool asynch);

// mbGetPlayerTerminationCallback
// --------------------------------------------------
// Returns the termination callback used by the player, or NULL if none is set.
//
mbTerminationCallback  mbGetPlayerTerminationCallback(
								mbObjectID playerID,
								mbResult *outResult);



// Marker Extension
// ****************************************************************************
// Under some builds of mobileBAE, markers may be enabled (which additionally
// implies that callbacks are supported).
// All Marker Extension functions will fail with a result of 
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support markers (call mbIsFeatureSupported with
// mbFeature_Markers to verify).
// ****************************************************************************

// mbGetMarkerCount
// --------------------------------------------------
// Returns the number of markers defined in the content of the player.
// See the mobileBAE user guide for which file formats are supported.
//
mbInt32				mbGetMarkerCount(
								mbObjectID playerID,
								mbResult *outResult);

// mbGetMarkerID
// --------------------------------------------------
// Returns the marker ID for the given index.
//
mbInt32				mbGetMarkerID(
								mbObjectID playerID,
								mbInt32 index,
								mbResult *outResult);

// mbGetMarkerName
// --------------------------------------------------
// Returns the name of the marker specified by 'ID'.
//
mbString			mbGetMarkerName(
								mbObjectID playerID,
								mbInt32 markerID,
								mbResult *outResult);

// mbGetMarkerPosition
// --------------------------------------------------
// Returns the position of the marker specified by 'ID'.
//
mbTime				mbGetMarkerPosition(
								mbObjectID playerID,
								mbInt32 markerID,
								mbResult *outResult);

// mbSetPositionToMarker
// --------------------------------------------------
// Sets the position of the player to that of the marker specified by 'ID'.
//
mbResult			mbSetPositionToMarker(
								mbObjectID playerID,
								mbInt32 markerID);

// mbSetMarkerCallback
// --------------------------------------------------
// Set a callback to be executed when the player passes a marker.
// Pass 'callback' as NULL to disable marker callbacks.
//
mbResult			mbSetMarkerCallback(
								mbObjectID playerID,
								mbMarkerCallback callback,
								void *context,
								mbBool asynch);

// mbGetMarkerCallback
// --------------------------------------------------
// Returns the marker callback used by the player, or NULL if none is set.
//
mbMarkerCallback 	mbGetMarkerCallback(
								mbObjectID playerID,
								mbResult *outResult);

#endif // _MBAE_PLAYERCTRL_H_
