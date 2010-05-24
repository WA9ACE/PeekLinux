/******************************************************************************
**
**	mBAE_MidiPlayerCtrl.h
**
**	mbMidiFilePlayerCtrl represents transport control for a MIDI file.
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
**	2002.04.02 msd	removed mbPrerollMidiPlayer, now on PlayerCtrl
**	2002.04.24 SME	Removed objectID param from mbAreMidiPlayerCallbacksSupported.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.06.18 AER	Corrected signature of mbMidiTimeCallback (it indicated
**					microseconds).
**	2002.06.20 AER	Renamed mbGetDurationInTicks to mbGetDurationInMidiTicks.
**					Renamed mbSetPositionInTicks to mbSetPositionInMidiTicks.
**					Renamed mbGetPositionInTicks to mbGetPositionInMidiTicks.
**					Renamed mbGetTempoInTicksPerBeat to
**					mbGetTempoInMidiTicksPerBeat.
**					Renamed mbMidiTimeCallback to mbMidiPlayerPulseCallback.
**					mbAreCallbacksSupported replaces all other callback support
**					queries.
**	2002.06.21 AER	Moved mbSetMidiPlayerVoiceLimit to mBAE_MidiSynth and
**					renamed it mbSetSynthVoiceLimit.
**					Replaced usage of mbMilliTime with mbTime.
**	2002.06.23 AER	Updated documentation comments.
**	2002.06.24 AER	Moved mbGetMidiSynth/Count to mBAE_MidiPlayerCtrl.h.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.09.12 msd	Added mbEnableSPMidi() and mbIsSPMidiEnabled()
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2002.10.22 msd	Added mbEnableBuzzer/mbIsBuzzerEnabled()
**	2002.10.22 msd	Moved Controller callbacks from MidiPlayerCtrl to MidiSynth
**	2002.11.06 AER	Renamed 'Buzzer' 'Vibrator' to match common industry usage.
**	2003.02.04 msd	Moved all type declarations to mBAE_Types.h
**	2003.07.15 msd	added EnableLed(), IsLedEnabled()
**	2004.01.18 msd	changes for new tempo and terminal controls
**					Renamed mbGetTempoInMidiTicksPerBeat to mbGetMidiTicksPerBeat
**	2004.02.12 AER	Improved mbGetMidiSynth description.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_MIDIPLAYERCTRL_H_
#define _MBAE_MIDIPLAYERCTRL_H_

#include "mBAE_Types.h"


// mbGetTrackCount
// --------------------------------------------------
// Returns the number of tracks in the MIDI player's content.
//
mbInt16				mbGetTrackCount(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbGetMidiSynthCount
// --------------------------------------------------
// Returns the number of synth devices through which the MIDI file player
// plays.
//
mbInt16				mbGetMidiSynthCount(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbGetMidiSynth
// --------------------------------------------------
// Acquires a MIDI synth device through which the MIDI file is playing.
// Range of 'index' is 0 through mbGetMidiSynthCount - 1.
// This creates a MidiSynth object.
// Make sure to call mbDestroy on the object to dispose of it.
//
mbObjectID			mbGetMidiSynth(
								mbObjectID midiPlayerID,
								mbInt16 index,
								mbResult *outResult);

// mbMuteTrack
// --------------------------------------------------
// Mutes/unmutes tracks on the MIDI player.
// Range for this operation is 0 to GetTrackCount - 1.
//
mbResult			mbMuteTrack(
								mbObjectID midiPlayerID,
								mbInt16 track,
								mbBool doMute);

// mbIsTrackMuted
// --------------------------------------------------
//
mbBool				mbIsTrackMuted(
								mbObjectID midiPlayerID,
								mbInt16 track,
								mbResult *outResult);

// mbSoloTrack
// --------------------------------------------------
// Solos/unsolos tracks on the MIDI player.
// Range for this operation is 0 to GetTrackCount - 1.
//
mbResult			mbSoloTrack(
								mbObjectID midiPlayerID,
								mbInt16 track,
								mbBool doSolo);

// mbIsTrackSoloed
// --------------------------------------------------
//
mbBool				mbIsTrackSoloed(
								mbObjectID midiPlayerID,
								mbInt16 track,
								mbResult *outResult);

// mbGetDurationInMidiTicks
// --------------------------------------------------
// Returns the duration of the MIDI player's content in midi ticks.
//
mbUInt32			mbGetDurationInMidiTicks(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbSetPositionInMidiTicks
// --------------------------------------------------
// Sets the current playback position of the MIDI player in MIDI ticks.
//
mbResult			mbSetPositionInMidiTicks(
								mbObjectID midiPlayerID,
								mbUInt32 ticks);

// mbGetPositionInMidiTicks
// --------------------------------------------------
//
mbUInt32			mbGetPositionInMidiTicks(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbGetMidiTicksPerBeat
// --------------------------------------------------
// Returns the resolution of the MIDI player in ticks per beat.
//
mbInt32				mbGetMidiTicksPerBeat(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbEnableSPMidi
// --------------------------------------------------
// When enabled, the player will obey SP-MIDI MIP messages.
// Default is mbTrue.
//
mbResult			mbEnableSPMidi(
								mbObjectID midiPlayerID,
								mbBool isEnabled);

// mbIsSPMidiEnabled
// --------------------------------------------------
// Returns whether SP-MIDI MIP messages are obeyed by the player.
// Default is mbTrue.
mbBool				mbIsSPMidiEnabled(
							   mbObjectID midiPlayerID,
							   mbResult *outResult);



// MIDI Player Callback Extension
// ****************************************************************************
// Under some builds of mobileBAE, callbacks may be enabled.
// These callbacks are specific to MIDI players and require the ID to which
// the callback refers.
// All MIDI Player Callback functions will fail with a result of
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support callbacks (call mbIsFeatureSupported with
// mbFeature_Callbacks to verify).
// ****************************************************************************

// mbSetPulseCallback
// --------------------------------------------------
// Sets a callback on the MIDI player that executes once per mixer cycle.
// Pass 'callback' as NULL to disable output buffer callbacks.
//
mbResult			mbSetMidiPlayerPulseCallback(
								mbObjectID midiPlayerID,
								mbMidiPlayerPulseCallback callback,
								void *context,
								mbBool asynch);

// mbGetPulseCallback
// --------------------------------------------------
//
mbMidiPlayerPulseCallback mbGetMidiPlayerPulseCallback(
								mbObjectID midiPlayerID,
								mbResult *outResult);

// mbSetMidiPlayerMetaEventCallback
// --------------------------------------------------
// Sets a callback on the MIDI player that executes when the player
// passes a Meta Event in the player's content.
// Pass 'callback' as NULL to disable metaevent callbacks.
//
mbResult			mbSetMidiPlayerMetaEventCallback(
								mbObjectID midiPlayerID,
								mbMetaEventCallback callback,
								void *context,
								mbBool asynch);

// mbGetMidiPlayerMetaEventCallback
// --------------------------------------------------
//
mbMetaEventCallback mbGetMidiPlayerMetaEventCallback(
								mbObjectID midiPlayerID,
								mbResult *outResult);



// Deprecated / Obsolete Functions
// ****************************************************************************
// These functions are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// mbGetTempoInMidiTicksPerBeat
// --------------------------------------------------
// This function has been replaced by mbGetMidiTicksPerBeat.
//
#define mbGetTempoInMidiTicksPerBeat		mbGetMidiTicksPerBeat

#endif // _MBAE_MIDIPLAYERCTRL_H_
