/******************************************************************************
**
**	mBAE_AudioFilePlayer.h
**
**	mbAudioFilePlayer plays back digital audio data loaded from disk, flash,
**	memory, etc.
**	Playback may occur from a buffered chunk of memory that mobileBAE copies
**	upon reading the file, or streamed from the file in buffers whose size is
**	determined by the client.
**	All Audio File Player functions will fail with a result of 
**	mbFeature_Not_Supported (if possible) if the current build of mobileBAE
**	does not support digital audio (call mbIsFeatureSupported with
**	mbFeature_DigitalAudio to verify).
**
**	AudioFilePlayers may use the controls found in the following files:
**		mBAE_AudibleCtrl.h
**		mBAE_LoadCtrl.h
**		mBAE_PlayerCtrl.h
**		mBAE_AudioPlayerCtrl.h
**		mBAE_DRCCtrl.h
**		mBAE_MetadataCtrl.h
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
**	2002.05.20 AER	Added mbSetUnderflowCallback.
**	2002.05.22 AER	Adapted to new method of setting the buffer size of sounds.
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.06.30 AER	mbLoadCustomSample now uses mbByteOrder to determine byte
**					order, not an mbBool.
**	2002.07.15 AER	Modified the mbLoadCustomSample description to reflect that
**					if the client requests that no data be copied, the engine
**					will return mbCopy_Required if the data will not work
**					without a copy.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2002.10.22	DS	Removed mbQueryAudioFileInfo (now in LoadCtrl).
**	2003.01.15 AER	Replaced custom creators with mbCreateAudioFilePlayer.
**	2003.02.04 msd	Moved all type declarations to mBAE_Types.h
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**					Removed backward compatibility macros (due to this change).
**	2003.08.11 AER	[Bug 1115] Revised description of mbLoadCustomSample.
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.02.18 AER	Updated mbLoadCustomSample to accept const parameters.
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_AUDIOFILEPLAYER_H_
#define _MBAE_AUDIOFILEPLAYER_H_

#include "mBAE_Types.h"




// mbCreateAudioFilePlayer
// --------------------------------------------------
// Creates and initializes an audio file player for buffered or streamed,
// playback, depending upon the value of 'streamed'.
// Pass 'streamed' as mbFalse to have mobileBAE create a buffered audio file
// player; pass 'streamed' as mbTrue to have mobileBAE create a streamed audio
// file player.
// If the audio file player is streamed, it will use a default buffer size
// corresponding to half a second of sound if not otherwise set by
// mbSetBufferTime/ByteSize. Make sure to call mbGetMinimumBufferTime/ByteSize
// to determine the minimum reasonable time for the buffer.
//
mbObjectID			mbCreateAudioFilePlayer(
								mbObjectID system,
								mbBool streamed,
								mbResult *outResult);

// mbLoadCustomSample
// --------------------------------------------------
// Loads a raw digital audio sample from a raw pointer.
// Pass in the data, a filled mbSampleInfo reference, the byte order of the
// data, and a flag specifying whether the audio file player should make a
// copy of the data.
// Pass 'copyData' as mbFalse to request that mobileBAE read data from the
// provided buffer without needlessly copying data. While this may prevent
// buffer duplication (saving memory), make sure to deallocate the memory only
// after disposing of the loadable object.
// Pass 'copyData' as mbTrue to have mobileBAE use a duplicate of the provided
// buffer. While this will momentarily require additional memory for the copy,
// the buffer may be deallocated anytime after the function returns.
// If requested to load data that requires some sort of conversion (byte order,
// sign, encoding, encryption, etc.) and copyData is sent as mbFalse, this
// function will return mbCopy_Required.
//
mbResult			mbLoadCustomSample(
								mbObjectID audioFilePlayerID,
								const void *sampleData,
								const mbSampleInfo * const sampleInfo,
								mbByteOrder byteOrder,
								mbBool copyData);

// mbSetUnderflowCallback
// --------------------------------------------------
// Sets a callback on a streamed audio file player that will execute if the
// player is unable to buffer enough data for smooth playback.
// Pass 'callback' as NULL to disable output buffer callbacks.
//
mbResult			mbSetUnderflowCallback(
								mbObjectID audioFilePlayerID,
								mbUnderflowCallback callback,
								void *context,
								mbBool asynch);

#endif // _MBAE_AUDIOFILEPLAYER_H_
