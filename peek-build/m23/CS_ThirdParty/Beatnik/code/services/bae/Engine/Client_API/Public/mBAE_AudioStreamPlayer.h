/******************************************************************************
**
**	mBAE_AudioStreamPlayer.h
**
**	mbAudioStreamPlayer plays back digital audio data streamed from memory
**	via callbacks from mobileBAE to the client.
**	All Audio Stream Player functions will fail with a result of
**	mbFeature_Not_Supported (if possible) if the current build of mobileBAE
**	does not support digital audio (call mbIsFeatureSupported with
**	mbFeature_DigitalAudio to verify).
**
**	AudioStreamPlayers may use the controls found in the following files:
**		mBAE_AudibleCtrl.h
**		mBAE_PlayerCtrl.h
**		mBAE_AudioPlayerCtrl.h
**		mBAE_DRCCtrl.h
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
**	2002.03.27 SME	Removed player specific Destroy APIs.
**	2002.04.09 AER	Updated documentation of client stream callback.
**	2002.05.06 AER	Added comment to mbSetupFileCallbackStream.
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.09.26 AER	Altered mbStreamCallback comments re: interrupt safety.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2003.02.04 msd	Moved all type declarations to mBAE_Types.h
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**	2003.02.11 AER	Added a context to stream callbacks to allow help clients
**					avoid the use of globals and allow multiple streams.
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_AUDIOSTREAMPLAYER_H_
#define _MBAE_AUDIOSTREAMPLAYER_H_

#include "mBAE_Types.h"


// mbCreateAudioStreamPlayer
// --------------------------------------------------
// Creates and initializes the audio stream player for playback.
//
mbObjectID			mbCreateAudioStreamPlayer(
								mbObjectID system,
								mbResult *outResult);

// mbSetupFileCallbackStream
// --------------------------------------------------
// Sets up the audio stream player to stream file data via a client-serviced
// callback.
// Some file types (currently, MPEG files) do not support seeking in callback
// streams. This will return mbOperation_Not_Enabled in these cases.
// 'clientCallback' specifies a pointer to the callback that executes when
// mobileBAE requires more data to render (see mbStreamCallback).
// 'canSeek' specifies whether the client-serviced callback can handle
// requests to jump to a new position in the stream.
// The audio stream player will use a default buffer size corresponding to
// half a second of sound if not otherwise set by mbSetBufferTime/ByteSize.
// Make sure to call mbGetMinimumBufferTime/ByteSize to determine the minimum
// reasonable time for the buffer.
//
mbResult			mbSetupFileCallbackStream(
								mbObjectID audioStreamPlayerID,
								mbStreamCallback callback,
								void *context,
								mbFileType fileType,
								mbBool canSeek);

// mbSetupRawCallbackStream
// --------------------------------------------------
// Sets up the audio stream player to stream raw digital audio data via a
// client-serviced callback.
// 'clientCallback' specifies a pointer to the callback that executes when
// mobileBAE requires more data to render (see mbStreamCallback).
// 'format' specifies the audio format of the raw digital audio data.
// 'byteOrder' specifies the byte order of the raw digital audio data.
// 'canSeek' specifies whether the client-serviced callback can handle
// requests to jump to a new position in the stream.
// The audio stream player will use a default buffer size corresponding to
// half a second of sound if not otherwise set by mbSetBufferTime/ByteSize.
// Make sure to call mbGetMinimumBufferTime/ByteSize to determine the minimum
// reasonable time for the buffer.
//
mbResult			mbSetupRawCallbackStream(
								mbObjectID audioStreamPlayerID,
								mbStreamCallback callback,
								void *context,
								const mbAudioFormat * const format,
								mbByteOrder byteOrder,
								mbBool canSeek);

#endif // _MBAE_AUDIOSTREAMPLAYER_H_
