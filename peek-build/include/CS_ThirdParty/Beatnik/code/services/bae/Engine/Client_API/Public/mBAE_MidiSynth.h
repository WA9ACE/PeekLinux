/******************************************************************************
**
**	mBAE_MidiSynth.h
**
**	MidiSynth represents the front-panel control for a MIDI synthesizer.
**
**	MidiSynths may use the controls found in the following file:
**		mBAE_AudibleCtrl.h
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
**	2002.03.20 SME	Removed transposition and pressure APIs.
**	2002.03.27 SME	Removed player specific Destroy APIs.
**	2002.04.26	DS	Added mbUnmapProgram().
**	2002.06.05 AER	Removed mbUnloadInstrumentForProgram and mbUnloadPercussion
**					because mobileBAE can't safely garbage collect.
**	2002.06.06 AER	Removed mbUnloadAllInstruments (oversight).
**	2002.06.09 AER	Replaced use of microseconds (mbTime) with milliseconds
**					(temporarily called mbMilliTime).
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.06.17 msd	Added mbFlushPendingMidiEvents()
**	2002.06.20 AER	Removed mbGetTimestamp (replaced by the mbSystem call
**					mbRendererGetRunningTime).
**	2002.06.21 AER	Moved mbSetMidiPlayerVoiceLimit to mBAE_MidiSynth and
**					renamed it mbSetSynthVoiceLimit.
**					Replaced usage of mbMilliTime with mbTime.
**	2002.08.23	DS	Restored all mbUnload functions for 1.1.
**	2002.10.22 msd	Moved Controller callbacks from MidiPlayerCtrl to MidiSynth
**	2003.02.04 msd	Moved all type declarations to mBAE_Types.h
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**	2003.07.15 msd	Added Sysex Callback
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.02.17 AER	Updated mbSetSysex to accept a const sysex stream.
**	2004.04.21	DS	All methods which take a bank now take bankMSB and bankLSB.
**	2004.05.19 AER	Updated several functions to use mbProgram, not mbUInt8.
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_MIDISYNTH_H_
#define _MBAE_MIDISYNTH_H_

#include "mBAE_Types.h"


// mbCreateMidiSynth
// --------------------------------------------------
//
mbObjectID			mbCreateMidiSynth(
								mbObjectID system,
								mbResult *outResult);

// mbSetSynthVoiceLimit
// --------------------------------------------------
// Sets the maximum number of sounding MIDI voices allowed on the synth.
//
mbResult			mbSetSynthVoiceLimit(
								mbObjectID midiSynthID,
								mbInt16 midiVoices);

// mbGetSynthVoiceLimit
// --------------------------------------------------
//
mbInt16				mbGetSynthVoiceLimit(
								mbObjectID midiSynthID,
								mbResult *outResult);


// mbMuteChannel
// --------------------------------------------------
// Mute/unmute channels on the synth device.
// Range for this operation is 0 to 15.
//
mbResult			mbMuteChannel(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbBool doMute);

// mbIsChannelMuted
// --------------------------------------------------
// Returns whether the channel is currently muted.
//
mbBool				mbIsChannelMuted(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbResult *outResult);

// mbSoloChannel
// --------------------------------------------------
// Solos/unsolos channels on the synth device
// Range for this operation is 0 to 15.
//
mbResult			mbSoloChannel(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbBool doSolo);

// mbIsChannelSoloed
// --------------------------------------------------
// Returns whether the channel is currently soloed.
//
mbBool				mbIsChannelSoloed(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbResult *outResult);

// mbNoteOn
// --------------------------------------------------
//
mbResult			mbNoteOn(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt8 note,
								mbUInt8 velocity,
								mbTime time);

// mbNoteOff
// --------------------------------------------------
//
mbResult			mbNoteOff(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt8 note,
								mbUInt8 velocity,
								mbTime time);

// mbNoteOnWithLoad
// --------------------------------------------------
// Executes a NoteOn after loading the appropriate instrument (if necessary).
// Do not call this function during an interrupt, as it may allocate memory.
//
mbResult			mbNoteOnWithLoad(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt8 note,
								mbUInt8 velocity,
								mbTime time);

// mbSetController
// --------------------------------------------------
//
mbResult			mbSetController(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt8 controllerNumber,
								mbUInt8 controllerValue,
								mbTime time);

// mbGetController
// --------------------------------------------------
// Returns the current MIDI controller value on the specified channel of the
// synth device.
//
mbUInt16			mbGetController(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt8 controller,
								mbResult *outResult);

// mbSetRPN
// --------------------------------------------------
// Sets an RPN (Registered Parameter) on the synth device.
// See the mobileBAE user guide for details about how to use this function.
//
mbResult			mbSetRPN(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt16 RPNNumber,
								mbUInt16 RPNValue,
								mbTime time);

// mbGetRPN
// --------------------------------------------------
// Returns the current RPN value on the specified channel of the synth device.
//
mbUInt16			mbGetRPN(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt16 RPNNumber,
								mbResult *outResult);

// mbSetNRPN
// --------------------------------------------------
// Sets an NRPN (Non-Registered Parameter) on the synth device.
// See the mobileBAE user guide for details about how to use this function.
//
mbResult			mbSetNRPN(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt16 NRPNNumber,
								mbUInt16 NRPNValue,
								mbTime time);

// mbGetNRPN
// --------------------------------------------------
// Returns the current NRPN value on the specified channel of the synth
// device.
//
mbUInt16			mbGetNRPN(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt16 NRPNNumber,
								mbResult *outResult);

// mbSetProgram
// --------------------------------------------------
//
mbResult			mbSetProgram(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbProgram program,
								mbTime time);

// mbSetProgramAndBank
// --------------------------------------------------
//
mbResult			mbSetProgramAndBank(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB,
								mbTime time);

// mbGetProgramAndBank
// --------------------------------------------------
//
mbResult			mbGetProgramAndBank(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbProgram *program,
								mbBank *outBankMSB,
								mbBank *outBankLSB);

// mbSetPitchBend
// --------------------------------------------------
//
mbResult			mbSetPitchBend(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbUInt16 value,
								mbTime time);

// mbGetPitchBend
// --------------------------------------------------
// Returns the current pitch bend value for the specified channel on the synth
// as a 14-bit value.
// See the mobileBAE user guide for details on how to use this function.
//
mbUInt16			mbGetPitchBend(
								mbObjectID midiSynthID,
								mbUInt8 channel,
								mbResult *outResult);

// mbSetSysex
// --------------------------------------------------
// Sends a system exclusive message to the synth device.
//
mbResult			mbSetSysex(
								mbObjectID midiSynthID,
								const mbUInt8 *sysExData,
								mbUInt32 sysExLength,
								mbTime time);

// mbAllNotesOff
// --------------------------------------------------
// Instructs the synth to put all sounding voices into release.
//
mbResult			mbAllNotesOff(
								mbObjectID midiSynthID,
								mbTime time);

// mbPanicNotesOff
// --------------------------------------------------
// Instructs the synth to immediately shut down all voices.
//
mbResult			mbPanicNotesOff(
								mbObjectID midiSynthID,
								mbTime time);

// mbGetPendingMidiEventCount
// --------------------------------------------------
// Returns the number of queued midi events pending in the synth device.
//
mbUInt32			mbGetPendingMidiEventCount(
								mbObjectID midiSynthID,
								mbResult *outResult);

// mbFlushPendingMidiEvents
// --------------------------------------------------
// Flushes all queued midi events pending in the synth deviced.
//
mbResult			mbFlushPendingMidiEvents(
								mbObjectID midiSynthID);

// mbLoadInstrumentForProgram
// --------------------------------------------------
// All loaded instruments are unloaded on the destruction of the synth.
//
mbResult			mbLoadInstrumentForProgram(
								mbObjectID midiSynthID,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB);

//	mbUnloadInstrumentForProgram
//	--------------------------------------------------
//
mbResult			mbUnloadInstrumentForProgram(
								mbObjectID midiSynthID,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB);

// mbIsProgramInstrumentLoaded
// --------------------------------------------------
//
mbBool				mbIsProgramInstrumentLoaded(
								mbObjectID midiSynthID,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB,
								mbResult *outResult);

// mbLoadPercussion
// --------------------------------------------------
// All loaded percussion is unloaded on the destruction of the synth.
//
mbResult			mbLoadPercussion(
								mbObjectID midiSynthID,
								mbUInt8 note,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB);

//	mbUnloadPercussion
//	--------------------------------------------------
//
mbResult			mbUnloadPercussion(
								mbObjectID midiSynthID,
								mbUInt8 note,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB);

// mbIsPercussionLoaded
// --------------------------------------------------
//
mbBool				mbIsPercussionLoaded(
								mbObjectID midiSynthID,
								mbUInt8 note,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB,
								mbResult *outResult);

// mbLoadAllInstruments
// --------------------------------------------------
// All loaded instruments are unloaded on the destruction of the synth.
//
mbResult			mbLoadAllInstruments(
								mbObjectID midiSynthID);

//	mbUnloadAllInstruments
//	--------------------------------------------------
//
mbResult			mbUnloadAllInstruments(
								mbObjectID midiSynthID);

// mbRemapProgram
// --------------------------------------------------
//
mbResult			mbRemapProgram(
								mbObjectID midiSynthID,
								mbProgram fromProgram,
								mbBank fromBankMSB,
								mbBank fromBankLSB,
								mbProgram toProgram,
								mbBank toBankMSB,
								mbBank toBankLSB);

// mbUnmapProgram
// --------------------------------------------------
//
mbResult			mbUnmapProgram(
								mbObjectID midiSynthID,
								mbProgram fromProgram,
								mbBank fromBankMSB,
								mbBank fromBankLSB);

// mbIsProgramRemapped
// --------------------------------------------------
// Return whether the program and bank sent are remapped on the synth device.
// If this function returns mbTrue, then 'toProgram' will contain the final
// program and 'toBank' will contain the final bank.
//
mbBool				mbIsProgramRemapped(
								mbObjectID midiSynthID,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB,
								mbProgram *outToProgram,
								mbBank *outToBankMSB,
								mbBank *outToBankLSB,
								mbResult *outResult);

// mbHandleMidiEvent
// --------------------------------------------------
// Sends a MIDI event to the synth device.
// See the mobileBAE user guide for details on how to use this function.
//
mbResult			mbHandleMidiEvent(
								mbObjectID midiSynthID,
								mbUInt8 commandByte,
								mbUInt8 data1Byte,
								mbUInt8 data2Byte,
								const mbUInt8 *sysEx,
								mbTime time);



// MIDI Synth Callback Extension
// ****************************************************************************
// Under some builds of mobileBAE, callbacks may be enabled.
// These callbacks are specific to MIDI synths and require the ID to which
// the callback refers.
// All MIDI Synth Callback functions will fail with a result of
// mbFeature_Not_Supported (if possible) if the current build of mobileBAE does
// not support callbacks (call mbIsFeatureSupported with mbFeature_Callbacks to
// verify).
// ****************************************************************************

// mbSetControllerCallback
// --------------------------------------------------
// Sets a callback on the MIDI synth that executes when the synth
// passes the specified controller in the synth's content.
// Pass 'callback' as NULL to disable controller callbacks.
//
mbResult			mbSetControllerCallback(
								mbObjectID midiSynthID,
								mbUInt8 controller,
								mbMidiControllerCallback callback,
								void *context,
								mbBool asynch);

// mbGetControllerCallback
// --------------------------------------------------
//
mbMidiControllerCallback mbGetControllerCallback(
								mbObjectID midiSynthID,
								mbUInt8 controller,
								mbResult *outResult);

// mbSetSysexCallback
// --------------------------------------------------
// Sets a callback on the MIDI synth that executes when the synth
// encounters a System Exclusive message.
// Pass 'callback' as NULL to disable.
//
mbResult			mbSetSysexCallback(
								mbObjectID midiSynthID,
								mbSysexCallback callback,
								void *context,
								mbBool asynch);

// mbGetSysexCallback
// --------------------------------------------------
//
mbSysexCallback		mbGetSysexCallback(
								mbObjectID midiSynthID,
								mbResult *outResult);


#endif // _MBAE_MIDISYNTH_H_
