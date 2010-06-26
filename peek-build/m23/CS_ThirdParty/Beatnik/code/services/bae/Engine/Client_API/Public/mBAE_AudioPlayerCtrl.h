/******************************************************************************
**
**	mBAE_AudioPlayerCtrl.h
**
**	AudioPlayer is the control through which digital audio players can
**	manipulate content.
**	All Audio Player functions will fail with a result of
**	mbFeature_Not_Supported (if possible) if the current build of mobileBAE
**	does not support digital audio (call mbIsFeatureSupported with
**	mbFeature_DigitalAudio to verify).
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
**	2002.03.13 AER	Converted some static const vars to #defines.
**	2002.03.28 AER	Altered mbFramePositionCallback.
**	2002.04.02 msd	Removed mbPrerollAudioPlayer(), now on PlayerCtrl
**	2002.04.05 AER	Added mbGetMinimumBufferByteSize.
**	2002.04.12 AER	Updated mbGetSampleInfo.
**	2002.05.22 AER	Adapted to new method of setting the buffer size of sounds
**					by adding SetBufferTime/ByteSize.
**	2002.05.28 AER	Added notes to mbSetBufferTime and mbSetBufferByteSize
**					about calling when playing/paused.
**	2002.06.09 AER	Converted time units from float to milliseconds.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.06.20 AER	mbGetMinimumBufferTime now returns an mbMilliTime.
**	2002.06.21 AER	Replaced usage of mbMilliTime with mbTime.
**	2002.06.23 AER	Renamed mbAudioPlayerCallbackCtrl_AddSampleFrameCallback
**					mbAddSampleFrameCallback.
**					Renamed mbAudioPlayerCallbackCtrl_RemoveSampleFrameCallback
**					mbRemoveSampleFrameCallback.
**					Updated documentation comments.
**	2002.06.30 AER	Deprecated mbIsAudioPlayerReverbSupported (use system
**					function mbIsReverbSupported).
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2003.02.04 msd	Moved all type and param declarations to mBAE_Types.h
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_AUDIOPLAYERCTRL_H_
#define _MBAE_AUDIOPLAYERCTRL_H_

#include "mBAE_Types.h"



// Audio Player Control Functions
// ****************************************************************************
// ****************************************************************************

// mbSetLoopPoints
// --------------------------------------------------
// Sets the loop points of the audio player's digital audio content in sample
// frames.
// Returns mbOperation_Not_Enabled on players that do not support looping.
//
mbResult			mbSetLoopPoints(
								mbObjectID audioPlayerID,
								mbInt32 loopStart,
								mbInt32 loopEnd);

// mbGetLoopPoints
// --------------------------------------------------
// Returns the loop points of the audio player's digital audio content in
// sample frames.
//
mbResult			mbGetLoopPoints(
								mbObjectID audioPlayerID,
								mbInt32 *outLoopStart,
								mbInt32 *outLoopEnd);

// mbSetPositionInSampleFrames
// --------------------------------------------------
// Sets the position of the audio player in sample frames.
// Returns mbOperation_Not_Enabled on audio players that do not support the
// setting of position.
//
mbResult			mbSetPositionInSampleFrames(
								mbObjectID audioPlayerID,
								mbUInt32 framePosition);

// mbGetPositionInSampleFrames
// --------------------------------------------------
// Returns the position of the audio player in sample frames.
//
mbUInt32			mbGetPositionInSampleFrames(
								mbObjectID audioPlayerID,
								mbResult *outResult);

// mbSetTransposeFactor
// --------------------------------------------------
// Sets the transpose factor on the audio player.
// Range for this function is 1 (0.001, 1/1000 speed) through 1000 (1.0, normal
// speed) through 10000 (10.0, ten times normal speed).
//
mbResult			mbSetTransposeFactor(
								mbObjectID audioPlayerID,
								mbMilliUnit factor);

// mbGetTransposeFactor
// --------------------------------------------------
// Returns the transpose factor of the audio player.
// Range for this function is 1 (0.001, 1/1000 speed) through 1000 (1.0, normal
// speed) through 10000 (10.0, ten times normal speed).
//
mbMilliUnit			mbGetTransposeFactor(
								mbObjectID audioPlayerID,
								mbResult *outResult);

// mbSetFilterParameters
// --------------------------------------------------
// Set filter parameters on the audio player.
// See the mobileBAE user guide for appropriate parameter tokens and values.
//
mbResult			mbSetFilterParameters(
								mbObjectID audioPlayerID,
								mbParamToken param,
								mbParamValue value);

// mbGetFilterParameters
// --------------------------------------------------
// Returns the value for the given filter parameter token.
// See the mobileBAE user guide for appropriate parameter tokens.
//
mbResult			mbGetFilterParameters(
								mbObjectID audioPlayerID,
								mbParamToken param,
								mbParamValue *outValue);

// mbGetSampleInfo
// --------------------------------------------------
// Fills out an mbSampleInfo structure that describes the digital audio
// content of the audio player.
// Returns 0.0 mbValue_Unreliable if mWaveSize or mWaveFrames is not known (as
// is the case with raw callback streams or any MPEG-base callback streams).
//
mbResult			mbGetSampleInfo(
								mbObjectID audioPlayerID,
								mbSampleInfo *outSampleInfo);

// mbGetMinimumBufferTime
// --------------------------------------------------
// Returns the minimum buffer time in milliseconds for initializing streaming
// or file-based audio buffers. Playing an audio player with streamed content
// with a value lower than that returned by the function is liable to cause
// stuttering sound.
//
mbTime				mbGetMinimumBufferTime(
								mbObjectID audioPlayerID,
								mbResult *outResult);

// mbSetBufferTime
// --------------------------------------------------
// Sets the amount of time in milliseconds that the streaming buffer should
// occupy for streamed audio players.
// If the player is currently paused or playing, this function does not change
// the buffer size.
// Returns mbOperation_Not_Enabled if the audio player is paused or playing, or
// if the audio player is not streamed.
//
mbResult			mbSetBufferTime(
								mbObjectID audioPlayerID,
								mbTime time);

// mbGetMinimumBufferByteSize
// --------------------------------------------------
// Returns the minimum buffer size in bytes for initializing streaming or
// file-based audio buffers. Playing an audio player with streamed content
// with a value lower than that returned by the function is liable to cause
// stuttering sound.
//
mbUInt32			mbGetMinimumBufferByteSize(
								mbObjectID audioPlayerID,
								mbResult *outResult);

// mbSetBufferByteSize
// --------------------------------------------------
// Sets the byte size for the streaming buffer should occupy for streamed
// audio players.
// If the player is currently paused or playing, this function does not change
// the buffer size.
// Returns mbOperation_Not_Enabled if the audio player is paused or playing, or
// if the audio player is not streamed.
//
mbResult			mbSetBufferByteSize(
								mbObjectID audioPlayerID,
								mbUInt32 bytes);

// mbAddSampleFrameCallback
// --------------------------------------------------
// Adds a position callback to the player that executes when the playback
// position reaches the indicated playback sample frame.
// It is possible to set callbacks on numerous positions in the player.
// It is possible to use the same callback with multiple positions.
//
mbResult			mbAddSampleFrameCallback(
								mbObjectID audioPlayerID,
								mbFramePositionCallback callback,
								void *context,
								mbInt32 sampleFrame,
								mbBool asynch);

// mbRemoveSampleFrameCallback
// --------------------------------------------------
// Removes the specified callback associated with the specified position from
// the player.
//
mbResult			mbRemoveSampleFrameCallback(
								mbObjectID audioPlayerID,
								mbInt32 sampleFrame);



// Audio Player Reverb Extension
// ****************************************************************************
// Under some builds of mobileBAE, reverb may be enabled.
// This extension allows clients to manipulate reverb on an audio player.
// Verify support for reverb with the system function mbIsFeatureSupported.
// ****************************************************************************

// mbSetReverbAmount
// --------------------------------------------------
// Sets the reverb send level on the audio player.
// Fails with a result of mbFeature_Not_Supported if the current build of
// mobileBAE does not support reverb (call mbIsFeatureSupported with
// mbFeature_Reverb to verify).
//
mbResult			mbSetReverbAmount(
								mbObjectID audioPlayerID,
								mbMilliUnit amount);

// mbGetReverbAmount
// --------------------------------------------------
// Returns the reverb send level of the audio player.
// Fails with a result of mbFeature_Not_Supported if the current build of
// mobileBAE does not support reverb (call mbIsFeatureSupported with
// mbFeature_Reverb to verify).
//
mbMilliUnit			mbGetReverbAmount(
								mbObjectID audioPlayerID,
								mbResult *outResult);

#endif // _MBAE_AUDIOPLAYERCTRL_H_
