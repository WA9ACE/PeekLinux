/******************************************************************************
**
**	mBAE_System.h
**
**	Functions that control the system, mixer, renderer, and audio hardware.
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
**	2002.01.22 AER	Created file.
**	2002.03.13 AER	Converted some static const vars to #defines.
**					Cleaned up some enums (to default to undefined).
**					Fixed some variable names.
**	2002.03.14 AER	Auto Disconnect is now part of Audio Output Hardware.
**	2002.03.20 AER	First complete version of mBAE_System.
**	2002.03.21 AER	Removed unused chorus and reverb parameters.
**					Renamed mbOutputCallback mbIOCallback and moved it and
**					mbTimeCallback to mBAE_Types.h.
**	2002.03.27 SME	Added Audio Capture support (InputHardware).
**					Removed player specific Destroy APIs and replaced with
**					single global mbDestroy.
**	2002.04.02 msd	added mbGetSystemCPULoad, killed other CPU load functions
**					removed MixerInputVolume functions
**					removed mbIsMixerAudioActive()
**					removed mbGetHardwareOutputFrameCount()
**					reworked mbGetRendererRealtimeStatus()
**	2002.04.11 AER	Added mbAreAudioPlayersSupported and moved mbDestroy.
**	2002.04.24 SME	Added mbAreCollectionsSupported.
**	2002.06.09 AER	Replaced use of microseconds (mbTime) with milliseconds
**					(temporarily called mbMilliTime).
**					Converted time units from float to milliseconds.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.06.14 AER	SampleRate converted to an mbUInt32.
**	2002.06.20 AER	Get/SetHardwareAudioLatency now uses standard mbMilliTime.
**					mbAreCallbacksSupported replaces all other callback support
**					queries.
**					Renamed mbSetRendererMasterSoundEffectsVolume to
**					mbSetRendererMasterAudioPlayerVolume.
**					Renamed mbGetRendererTimestamp to mbGetRendererRunningTime.
**	2002.06.24 AER	Renamed mbTimeCallback mbMixerPulseCallback and moved it
**					from mBAE_Types.h to mBAE_System.h.
**					Moved mbIOCallback from mBAE_Types.h to mBAE_System.h.
**	2002.06.25 AER	Corrected rename to mbGetRendererMasterAudioPlayerVolume.
**	2002.06.28 AER	Renamed mbGetSystemCPUType mbGetProcessorType.
**					Renamed mbGetSystemPlatform mbGetHostPlatform.
**					Renamed mbGetMixerAudioData mbGetAudioOutputBuffer.
**					Renamed mbGetMixerAudioDataFrameCountmbGetAudioOutputBufferLengthInFrames.
**	2002.07.18 AER	Fixed mbStartupSystem and mbSetRendererVoiceLimit comments.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.08.08 msd	Added mbSet/GetDefaultInstrument()
**	2002.09.12	DS	Added new API for Dynamic Range Compression (DRC).
**	2002.09.16	DS	Added mb{Set,Get}{Reverb,Chorus}Parameters with tokens and values.
**	2002.09.26 msd	Removed DRC MakeupGain param
**			   AER	Renamed mbQueryRendererInfo mbQueryDefaultBankInfo.
**			   AER	Renamed mb*Hardware* functions to mb*OutputHardware*.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2002.10.15 SME	Made void functions explicitly void due to ARM compiler warnings.
**	2002.10.28 AER	Added mbAreRingtonePlayersSupported.
**			   msd	Renamed mb*DefaultInstrument() -> mb*DefaultRingtoneInstrument()
**	2002.11.02 AER	Fixed comment for mb*DefaultRingtoneInstrument.
**	2002.11.11 SME	Removed incorrect comments regarding PAL_ENABLE_STEREO_INPUT.
**	2003.02.04 msd	Moved all type and param declarations to mBAE_Types.h
**	2003.02.06 AER	All system functions now require an mbSystemID parameter.
**					Renamed mbStartupSystem and mbShutdownSystem mbCreateSystem
**					and mbDestroySystem (because systems are now instantiated
**					explicitly).
**	2003.07.24 AER	Added a custom system creation function to allow client
**					code to communicate with a PAL implementation.
**	2003.11.18	DS	Added mbEnableMidiNormalizer and mbIsMidiNormalizerEnabled.
**	2003.12.01 SME	Removed mbGetSystemCPULoad.
**	2003.12.19 SME	First phase of mBAE 1.4 metadata overhaul.
**	2003.12.22 AER	Updated documentation of system create / destroy functions.
**	2003.12.23 AER	Added mbCreateDefaultSystem.
**	2003.12.29 SME	Added mbGetDefaultBank.
**	2004.01.08 AER	Revised the system heavily: System now uses a regular
**					mbObjectID and is destroyed by mbDestroy.
**	2004.01.08 msd	Moved DRC-related functions to new DRCCtrl
**	2004.01.09 AER	Tidied mbSystemSet/GetDefaultMetadataLanguage.
**	2004.01.09	DS	Added mbGet{Input,Output}HardwareCaps.
**	2004.01.09 msd	Removed controls for MasterAudioPlayerVolume, and RendererMixCount
**	2004.01.16 SME	Added support for input hardware capture to file and memory.
**	2004.01.22 AER	Updated documentation of mbGetErrorString.
**	2004.01.23	DS	Added new bank API and labeled old one as obsolete.
**			   AER	Revised version checking functions by adding core and PAL
**					version functions.
**	2004.01.28 msd	Added new terminal functions
**	2004.01.30 AER	Added mbIsFeatureSupported.
**					Deprecated mbIs/AreSupported fcns (backward compatible).
**	2004.02.12 AER	Improved mbGetBank description.
**	2004.02.13 AER	Moved in creation functions from defunct mBAE_Player.h.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**					Updated mbSetChorusParameters, mbSetReverbParameters, and
**					mbEngageInputHardware to accept const parameters.
**	2004.02.18 AER	Updated mbCreateAndLoadPlayerFromMemory and
**					mbAddBankFromMemory to accept const parameters.
**					Clarified obsolete functions.
**	2004.03.15 AER	Removed mbGetPALVersion per Platform Engineering request.
**					Improved documentation of mbGetErrorString.
**	2004.04.21	DS	All methods which take a bank now take bankMSB and bankLSB.
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**	2004.10.03 AER	Updated auto-disconnect functions.
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_SYSTEM_H_
#define _MBAE_SYSTEM_H_

#include "mBAE_Types.h"



// Utility Functions
// ****************************************************************************
// Global utility functions to operate on globally used data, to help determine
// system capabilities, etc.
// ****************************************************************************

// mbFreeString
// --------------------------------------------------
// Free memory associated with an mbString
//
mbResult			mbFreeString(
								mbString stringToFree);

// mbFreeBinaryData
// --------------------------------------------------
// Free memory associated with an mbBinaryData
//
mbResult			mbFreeBinaryData(
								mbBinaryData binaryDataToFree);

// mbGetErrorString
// --------------------------------------------------
// Get error message for a given mbResult. The string returned is a constant
// and should NOT be handed to mbFreeString.
// This function returns a string only in debug builds of mobileBAE; otherwise
// it returns NULL.
//
const mbChar *		mbGetErrorString(
								mbResult result);

// mbIsFeatureSupported
// --------------------------------------------------
// Returns whether the feature is supported by this build of mobileBAE.
//
mbBool				mbIsFeatureSupported(
								mbFeatureType feature);

// mbIsControlSupported
// --------------------------------------------------
// Returns whether an object supports the given mbControlType.
//
mbBool				mbIsControlSupported(
								mbObjectID theObjectID,
								mbControlType controlType,
								mbResult *outResult);



// System Creation / Destruction / Acquisition Functions
// ****************************************************************************
// Multiple instances of mobileBAE may be created, and each is independent of
// the others. On threaded systems, this API is thread-safe.
// ****************************************************************************

// mbCreateEngageSystem
// --------------------------------------------------
// Creates a stand-alone instance of mobileBAE, instantiating Renderer, Mixer,
// and AudioOutputHardware objects for use by the mobileBAE system.
// 'midiVoices' and 'audioVoices' indicate the the maximum number of voices to
// be allocated by the system renderer for the processing of MIDI and audio
// player voices.
// 'engage' indicates whether mobileBAE should engage the output hardware upon
// creating the system.
// 'context' is a reference that allows client code and the PAL implementation
// to communicate. If no such communication is required, pass NULL.
// Note that 'result' is not optional here, as it is in most other functions.
// Returns:
//   mbNo_Err				- there was no error
//   mbServiceIdle_Required	- it is necessary to call mbSystemServiceIdle to
//                            properly handle events.
//   mbFeature_Not_Supported - the user passed 'audioVoices' greater than zero
//                             and the the current build of mobileBAE does not
//							   support digital audio (call mbIsFeatureSupported
//							   with mbFeature_DigitalAudio to verify).
//
mbObjectID			mbCreateEngageSystem(
								mbInt16 midiVoices,
								mbInt16 audioVoices,
								mbBool engage,
								void *context,
								mbResult *outResult);

// mbCreateDefaultSystem
// --------------------------------------------------
// Creates a stand-alone instance of mobileBAE using system defaults defined
// by the PAL implementor.
// In addition to the values returned by mbCreateEngageSystem, this function
// returns mbFeature_Not_Supported if the current build of mobileBAE does not
// support default system creation (call mbIsFeatureSupported with
// mbFeature_DefaultCreate to verify).
// As with mbCreateEngageSystem, 'result' is not optional.
//
mbObjectID			mbCreateDefaultSystem(
								void *context,
								mbResult *outResult);
// mbSystemServiceIdle
// --------------------------------------------------
// Service internal system state.
// If the call to mbCreateSystem returned mbServiceIdle_Required, call this
// function whenever the application is idle.
//
mbResult			mbSystemServiceIdle(
								mbObjectID system);



// Generic Create / Destroy
// ****************************************************************************
// Players may be instantiated with standard creation functions (found in the
// header file for each player type) or generic create functions (found below).
// All mobileBAE mbObjectIDs in must be destroyed with mbDestroy to release
// their resources.
// ****************************************************************************

// mbCreateAndLoadPlayerFromFile
// --------------------------------------------------
// Creates and loads a player of the appropriate type for the file specified,
// if possible.
// Check mbIsControlSupported to determine which controls are supported by the
// new player.
// Set 'streamedIfAudio' to mbTrue if a streaming audio player is desired, and
// the file is an audio file.
// Memory allocation and/or data decompression, will occur during this call.
// Fails with a result code of mbFile_Type_Not_Supported if the file is not
// recognized as a known type, or if a recognizable file type is not supported
// by the current build of mobileBAE.
// Fails with a result code of mbUnknown_AudioCodec if the file contains data
// that requires a decoder not supported by the current build of mobileBAE.
//
mbObjectID			mbCreateAndLoadPlayerFromFile(
								mbObjectID system,
								mbFilePath theFilePath,
								mbBool streamedIfAudio,
								mbResult *outResult);


// mbCreateAndLoadPlayerFromMemory
// --------------------------------------------------
// Loads a file of length in bytes 'size' from the memory address specified by
// 'memoryPointer' after creating a player of the appropriate type, if
// recognized.
// Check mbIsControlSupported to determine which controls are supported by the
// new player.
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
// Fails with a result code of mbFile_Type_Not_Supported if the file is not
// recognized as a known type, or if a recognizable file type is not supported
// by the current build of mobileBAE
// Fails with a result code of mbUnknown_AudioCodec if the file contains data
// that requires a decoder not supported by the current build of mobileBAE.
//
mbObjectID			mbCreateAndLoadPlayerFromMemory(
								mbObjectID system,
								const void *memoryPointer,
								mbUInt32 size,
								mbBool copyData,
								mbResult *outResult);

// mbDestroy
// --------------------------------------------------
// All players and collections must be destroyed using the global mbDestroy
// function in order to free memory and system resources.
//
mbResult			mbDestroy(
								mbObjectID objectID);



// System Information Functions
// ****************************************************************************
// These functions return information on the system.
// ****************************************************************************

// mbGetProcessorType
// --------------------------------------------------
// Returns the CPU type the system is running on
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbGetProcessorType(
								mbObjectID system,
								mbResult *outResult);

// mbGetHostPlatform
// --------------------------------------------------
// Returns the Platform as an mbString
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbGetHostPlatform(
								mbObjectID system,
								mbResult *outResult);

// mbGetSystemTime
// --------------------------------------------------
// Get the current system time in milliseconds
//
mbTime				mbGetSystemTime(
								mbObjectID system,
								mbResult *outResult);

// mbGetCoreVersion
// --------------------------------------------------
// Returns the version of the mobileBAE core engine as three integers
// representing its major, minor, sub-minor numbers.
//
mbResult			mbGetCoreVersion(
								mbObjectID system,
								mbInt16 *outMajor,
								mbInt16 *outMinor,
								mbInt16 *outSubminor);

// mbGetCoreVersionString
// --------------------------------------------------
// Returns the version of the mobileBAE core engine as a string in the form
// "MMMM.mmmm.ssss" where MMMM is the major version, mmmm is the minor version,
// and ssss is the sub-minor version. Note that the number of characters used
// for each is sized to fit the number (e.g., the function will return "1.0.0"
// for 1.0.0, and "99.999.9999" for 99.999.9999).
//
mbString			mbGetCoreVersionString(
								mbObjectID system,
								mbResult *outResult);

// mbGetPALVersionString
// --------------------------------------------------
// Returns the version of the PAL implementation as a string in a form defined
// by the platform implementor.
//
mbString			mbGetPALVersionString(
								mbObjectID system,
								mbResult *outResult);

// mbSystemSetDefaultMetadataLanguage
// --------------------------------------------------
// Set the default language-country specification to be used when calling
// mbQueryFileInfo on XMF content. This should be a string as specified in
// section 3.2.2.2 of the XMF Meta File Format Specification available from the
// MMA ( http://www.midi.org/about-midi/xmf/index.shtml ).
//
mbResult			mbSystemSetDefaultMetadataLanguage(
								mbObjectID system,
								const mbChar *LangCountrySpec);

// mbSystemGetDefaultMetadataLanguage
// --------------------------------------------------
// Get the current language-country specification as set by a previous call to
// mbSystemSetDefaultMetadataLanguage.
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbSystemGetDefaultMetadataLanguage(
								mbObjectID system,
								mbResult *outResult);



// Terminal Functions
// ****************************************************************************
// These functions control terminal devices.
// ****************************************************************************

// mbActivateVibrator
// --------------------------------------------------
// Activates and deactivates the vibrate function (if any) on the terminal.
// Pass 'activate' as mbTrue to begin vibrating and mbFalse to stop vibrating.
//
mbResult			mbActivateVibrator(
								mbObjectID system,
								mbBool activate);

// mbActivateLED
// --------------------------------------------------
// Activates and deactivates the primary LED (if any) on the terminal.
// Pass 'activate' as mbTrue to turn on the LED and mbFalse to turn it off.
//
mbResult			mbActivateLED(
								mbObjectID system,
								mbBool activate);



// Mixer Functions
// ****************************************************************************
// All renderers feed their output into a mixer, which combines the signals
// and in turn feeds an audio output hardware object.
// These functions are available between startup and shutdown of mobileBAE.
// ****************************************************************************

// mbSetMixerMasterVolume
// --------------------------------------------------
//
mbResult			mbSetMixerMasterVolume(
								mbObjectID system,
								mbMilliUnit volume);

// mbGetMixerMasterVolume
// --------------------------------------------------
//
mbMilliUnit			mbGetMixerMasterVolume(
								mbObjectID system,
								mbResult *outResult);

// mbGetAudioOutputBufferLengthInFrames
// --------------------------------------------------
//
mbInt32				mbGetAudioOutputBufferLengthInFrames(
								mbObjectID system,
								mbResult *outResult);

// mbGetAudioOutputBuffer
// --------------------------------------------------
//
mbResult			mbGetAudioOutputBuffer(
								mbObjectID system,
								mbInt16 *outLeft,
								mbInt16 *outRight);



// Mixer Callback Extension
// ****************************************************************************
// Under some builds of mobileBAE, callbacks may be enabled.
// This extension allows clients to receive callbacks when certain mixer
// events occur.
// All Mixer Callback Extension functions will fail with a result of
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support callbacks (call mbIsFeatureSupported with
// mbFeature_Callbacks to verify).
// ****************************************************************************

// mbSetMixerOutputBufferCallback
// --------------------------------------------------
// Set a callback on the mixer that executes once per mixer cycle.
// The callback includes a pointer to the audio data currently set to be sent
// to the audio output hardware.
// Pass 'callback' as NULL to disable output buffer callbacks.
//
mbResult			mbSetMixerOutputBufferCallback(
								mbObjectID system,
								mbIOCallback callback,
								void *context);

// mbGetMixerOutputBufferCallback
// --------------------------------------------------
// Returns the mixer's output buffer callback, or NULL if none is set.
//
mbIOCallback		mbGetMixerOutputBufferCallback(
								mbObjectID system,
								mbResult *outResult);

// mbSetMixerPulseCallback
// --------------------------------------------------
// Set a callback on the mixer that will execute once per mixer cycle.
// The callback returns the system time in milliseconds.
// Pass 'callback' as NULL to disable pulse callbacks.
//
mbResult			mbSetMixerPulseCallback(
								mbObjectID system,
								mbMixerPulseCallback callback,
								void *context,
								mbBool asynch);

// mbGetMixerPulseCallback
// --------------------------------------------------
// Returns the mixer's pulse callback, or NULL if none is set.
//
mbMixerPulseCallback mbGetMixerPulseCallback(
								mbObjectID system,
								mbResult *outResult);



// Renderer Functions
// ****************************************************************************
// These functions manage the renderer abstraction of mobileBAE.
// These functions are available between startup and shutdown of mobileBAE.
// ****************************************************************************

// mbAddBankFromFile
// --------------------------------------------------
// Add a bank to the renderer's bank list at position 'position'.  Banks at
// lower positions are searched first when loading instruments.
// Failure to load leaves the list unchanged.
//
mbResult			mbAddBankFromFile(
								mbObjectID system,
								mbFilePath filePath,
								mbFileType fileType,
								mbInt32 position);

// mbAddBankFromMemory
// --------------------------------------------------
// Add a bank, via a memory address, to the renderer's bank list at position
// 'position'.
// Banks at lower positions are searched first when loading instruments.
// Failure to load leaves the list unchanged.
//
mbResult			mbAddBankFromMemory(
								mbObjectID system,
								const void *bank,
								mbInt32 length,
								mbFileType fileType,
								mbInt32 position);

// mbGetBankCount
// --------------------------------------------------
// Returns the number of banks current in the renderer's bank list.
mbInt32				mbGetBankCount(
								mbObjectID system,
								mbResult *outResult);

// mbRemoveBank
// --------------------------------------------------
// Removes the bank at position 'position'.  Returns mbNot_Found if there is no
// bank
// at that position.

mbResult			mbRemoveBank(
								mbObjectID system,
								mbInt32 position);

// mbGetBank
// --------------------------------------------------
// Returns an mbObjectID for the bank at position 'position' that can be
// used with the Metadata Control API.
// The returned mbObjectID may become invalidated if mbAddBankFromFile,
// mbAddBankFromMemory or mbRemoveBank is used.
// APIs which are passed an invalidated mbObjectID returned by mbGetBank
// will return mbNo_Longer_Valid.
// Make sure to call mbDestroy on the object to dispose of it.
//
mbObjectID			mbGetBank(
								mbObjectID sysID,
								mbInt32 position,
								mbResult *outResult);

// mbSetDefaultRingtoneInstrument
// --------------------------------------------------
// Set the default instrument to use for the monophonic ringtone file players.
//
mbResult			mbSetDefaultRingtoneInstrument(
								mbObjectID system,
								mbProgram program,
								mbBank bankMSB,
								mbBank bankLSB);

// mbGetDefaultRingtoneInstrument
// --------------------------------------------------
// Get the default instrument that the monophonic ringtone file players use.
//
mbResult			mbGetDefaultRingtoneInstrument(
								mbObjectID system,
								mbProgram *outProgram,
								mbBank *outBankMSB,
								mbBank *outBankLSB);

// mbIsRendererActive
// --------------------------------------------------
// Returns whether any MIDI player or audio player voices are active.
//
mbBool				mbIsRendererActive(
								mbObjectID system,
								mbResult *outResult);

// mbGetRendererRealtimeStatus
// --------------------------------------------------
// Get information about the real time status of renderer while it is running.
// Provide an array of mbVoiceInfo structs, and the number of items in the
// array. 'outNumActiveVoices' will be set to the number of active voices.
//
mbResult			mbGetRendererRealtimeStatus(
								mbObjectID system,
								mbInt16 *outNumActiveVoices,
								mbVoiceInfo *outVoiceInfoArray,
								mbInt16 voiceInfoArrayLength);

// mbGetRendererRunningTime
// --------------------------------------------------
// Get the renderer's current running time in milliseconds.
//
mbTime				mbGetRendererRunningTime(
								mbObjectID system,
								mbResult *outResult);

// mbSetRendererVoiceLimit
// --------------------------------------------------
// Set the limit on the number of active voices for the renderer.
// This is primarily used to limit CPU usage.
// Fails with a result of mbFeature_Not_Supported if if passed 
// 'maxAudioVoices' greater than zero and the current build of mobileBAE does
// not support digital audio (call mbIsFeatureSupported with
// mbFeature_DigitalAudio to verify).
//
mbResult			mbSetRendererVoiceLimit(
								mbObjectID system,
								mbInt16 maxMidiVoices,
								mbInt16 maxAudioVoices);

// mbGetRendererVoiceLimit
// --------------------------------------------------
// Returns the limit on the number of active voices for the renderer.
//
mbResult			mbGetRendererVoiceLimit(
								mbObjectID system,
								mbInt16 *outMaxMidiVoices,
								mbInt16 *outMaxAudioVoices);



// Midi Normalizer Extension
// ****************************************************************************
// Under some builds of mobileBAE, MIDI normalization may be supported.
// ****************************************************************************

// mbEnableMidiNormalizer
// --------------------------------------------------
// Enables the normalizing of the output levels of all active midi players.
//
mbResult			mbEnableMidiNormalizer(
								mbObjectID system,
								mbBool isEnabled);

// mbIsMidiNormalizerEnabled
// --------------------------------------------------
// Returns whether midi player outputs are normalized  as described
// in mbEnableMidiNormalizer.
//
mbBool				mbIsMidiNormalizerEnabled(
								mbObjectID system,
								mbResult *outResult);



// Renderer Chorus Extension
// ****************************************************************************
// Under some builds of mobileBAE, chorus may enabled on the renderer.
// All chorus functions below fail with a result of mbFeature_Not_Supported if
// the current build of mobileBAE does not support chorus (call
// mbIsFeatureSupported with mbFeature_Chorus to verify).
// ****************************************************************************

// mbEnableChorus
// --------------------------------------------------
// Enable chorus for all objects playing through the renderer.
//
mbResult			mbEnableChorus(
								mbObjectID system,
								mbBool doEnable);

// mbIsChorusEnabled
// --------------------------------------------------
// Returns whether chorus is enabled for the renderer.
//
mbBool				mbIsChorusEnabled(
								mbObjectID system,
								mbResult *outResult);

// mbSetChorusParameters
// --------------------------------------------------
// Configure the renderer chorus via a set of parameters.
//
mbResult			mbSetChorusParameters(
								mbObjectID system,
								const mbParam *params);

// mbGetChorusParameters
// --------------------------------------------------
// Retrieve the current chorus settings via a set of parameters.
//
mbResult			mbGetChorusParameters(
								mbObjectID system,
								mbParam *outParams);



// Renderer Reverb Extension
// ****************************************************************************
// Under some builds of mobileBAE, reverb may enabled on the renderer.
// All reverb functions below fail with a result of mbFeature_Not_Supported if
// the current build of mobileBAE does not support reverb (call
// mbIsFeatureSupported with mbFeature_Reverb to verify).
// ****************************************************************************

// mbEnableReverb
// --------------------------------------------------
// Enable reverb for all objects playing through the renderer.
//
mbResult			mbEnableReverb(
								mbObjectID system,
								mbBool doEnable);

// mbIsReverbEnabled
// --------------------------------------------------
// Returns whether reverb is enabled for the renderer.
//
mbBool				mbIsReverbEnabled(
								mbObjectID system,
								mbResult *outResult);

// mbSetReverbParameters
// --------------------------------------------------
// Configure the renderer reverb via a set of parameters.
//
mbResult			mbSetReverbParameters(
								mbObjectID system,
								const mbParam *params);

// mbGetReverbParameters
// --------------------------------------------------
// Retrieve the current reverb settings via a set of parameters.
//
mbResult			mbGetReverbParameters(
								mbObjectID system,
								mbParam *outParams);



// Audio Output Hardware Functions
// ****************************************************************************
// These functions manage the audio output hardware abstraction, which
// represents the audio hardware provided by the host operating system and the
// destination of the output of a Mixer.
// These functions are available between startup and shutdown of mobileBAE.
// ****************************************************************************

// mbEngageOutputHardware
// --------------------------------------------------
// Engages the hardware so that the system mixer plays through it.
//
mbResult			mbEngageOutputHardware(
								mbObjectID system);

// mbDisengageOutputHardware
// --------------------------------------------------
// Disengages the hardware.
//
mbResult			mbDisengageOutputHardware(
								mbObjectID system);

// mbIsOutputHardwareEngaged
// --------------------------------------------------
// Returns whether the hardware is currently engaged.
//
mbBool				mbIsOutputHardwareEngaged(
								mbObjectID system,
								mbResult *outResult);

// mbGetOutputHardwareCaps
// --------------------------------------------------
// Fills out the passed-in struct with information about the capabilities
// of the output audio hardware.

mbResult			mbGetOutputHardwareCaps(
								mbObjectID system,
								mbHardwareCaps *outCaps);

// mbIsOutputHardwareFormatSupported
// --------------------------------------------------
// Returns whether the hardware supports a given audio format (bit depth,
// sample type, channel count, sample rate, etc.).
//
mbBool				mbIsOutputHardwareFormatSupported(
								mbObjectID system,
								const mbAudioFormat * const format,
								mbResult *outResult);

// mbSetOutputHardwareAudioFormat
// --------------------------------------------------
// Set the audio format parameters all at once. This is necessary in cases
// where one parameter is dependent on another.
//
mbResult			mbSetOutputHardwareAudioFormat(
								mbObjectID system,
								const mbAudioFormat * const format);

// mbGetOutputHardwareAudioFormat
// --------------------------------------------------
// Returns the audio format currently being used by the hardware.
//
mbResult			mbGetOutputHardwareAudioFormat(
								mbObjectID system,
								mbAudioFormat *outAudioFormat);

// mbGetOutputHardwareBitDepth
// --------------------------------------------------
// Returns the bit depth setting of the hardware.
// To set, use SetAudioFormat.
//
mbInt16				mbGetOutputHardwareBitDepth(
								mbObjectID system,
								mbResult *outResult);

// mbSetOutputHardwareSampleRate
// --------------------------------------------------
// Set the output sampling rate of the hardware in frames/second.
//
mbResult			mbSetOutputHardwareSampleRate(
								mbObjectID system,
								mbUInt32 sampleRate);

// mbGetOutputHardwareSampleRate
// --------------------------------------------------
// Get the output sampling rate of the hardware in frames/second.
//
mbUInt32			mbGetOutputHardwareSampleRate(
								mbObjectID system,
								mbResult *outResult);

// mbSetOutputHardwareChannelCount
// --------------------------------------------------
// Set the number of interleaved audio channels of the hardware.
//
mbResult			mbSetOutputHardwareChannelCount(
								mbObjectID system,
								mbInt16 channelCount);

// mbGetOutputHardwareChannelCount
// --------------------------------------------------
// Get the number of interleaved audio channels of the hardware.
//
mbInt16				mbGetOutputHardwareChannelCount(
								mbObjectID system,
								mbResult *outResult);

// mbSetOutputHardwareAudioLatency
// --------------------------------------------------
// Set the latency of the hardware in milliseconds.
//
mbResult			mbSetOutputHardwareAudioLatency(
								mbObjectID system,
								mbTime latency);

// mbGetOutputHardwareAudioLatency
// --------------------------------------------------
// Get the latency of the hardware in milliseconds.
//
mbTime				mbGetOutputHardwareAudioLatency(
								mbObjectID system,
								mbResult *outResult);

// mbSetOutputHardwareBalance
// --------------------------------------------------
// Set the output balance of the hardware.
// Range for this operation is -1000 (-1.0, left), 0 (0.0, middle),
// 1000 (1.0, right).
//
mbResult			mbSetOutputHardwareBalance(
								mbObjectID system,
								mbMilliUnit balance);

// mbGetOutputHardwareBalance
// --------------------------------------------------
// Get the output balance of the hardware.
//
mbMilliUnit			mbGetOutputHardwareBalance(
								mbObjectID system,
								mbResult *outResult);

// mbSetOutputHardwareVolume
// --------------------------------------------------
// Set the output volume of of the hardware.
//
mbResult			mbSetOutputHardwareVolume(
								mbObjectID system,
								mbMilliUnit volume);

// mbGetOutputHardwareVolume
// --------------------------------------------------
// Get the output volume of of the hardware.
//
mbMilliUnit			mbGetOutputHardwareVolume(
								mbObjectID system,
								mbResult *outResult);

// mbGetOutputHardwareFrameCount
// --------------------------------------------------
// Get the number of audio frames that have played through the hardware
//	since it was engaged.
//
mbUInt32			mbGetOutputHardwareFrameCount(
								mbObjectID system,
								mbResult *outResult);



// Audio Output Hardware Auto Disconnect Extension
// ****************************************************************************
// Under some builds of mobileBAE, the ability to automatically disconnect
// the audio hardware may be enabled.
// This extension allows clients to manage whether or not, or how the system
// will automatically disconnect from the audio hardware after a period of
// silence.
// All Auto Disconnect Extension functions will fail with a result of
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support auto disconnect (call mbIsFeatureSupported with
// mbFeature_AutoDisconnect to verify).
// ****************************************************************************

// mbEnableAutoDisconnect
// --------------------------------------------------
// Enables the automatic disconnection of the audio hardware when a sufficient
// amount of time has passed since any audio has been sent to the hardware
// (e.g., silence).
//
mbResult			mbEnableAutoDisconnect(
								mbObjectID system,
								mbBool isEnabled);

// mbIsAutoDisconnectEnabled
// --------------------------------------------------
// Returns whether the hardware is set to automatically disconnect as described
// in EnableAutoDisconnect.
//
mbBool				mbIsAutoDisconnectEnabled(
								mbObjectID system,
								mbResult *outResult);

// mbSetAutoDisconnectTime
// --------------------------------------------------
// Sets the minimum amount of silence to wait before automatically disengaging
// the output hardware. it may take additional time to actually disengage,
// depending upon how the underlying platform implementation works.
// Range for time is 1000 to 30000 milliseconds.
//
mbResult			mbSetAutoDisconnectTime(
								mbObjectID system,
								mbTime time);

// mbGetAutoDisconnectTime
// --------------------------------------------------
// Returns the minimum amount of silence mobileBAE will wait before
// automatically disengaging the hardware.
//
mbTime				mbGetAutoDisconnectTime(
								mbObjectID system,
								mbResult *outResult);



// Audio Input Hardware Functions (Audio Capture)
// ****************************************************************************
// These functions manage the audio input hardware abstraction, which
// represents the audio input hardware provided by the host operating system
// and the source of the input that can be captured.
// These functions are available between startup and shutdown of mobileBAE.
// All Audio Input Hardware functions will fail with a result of
// mbFeature_Not_Supported (if possible) if the current build of
// mobileBAE does not support audio capture (call mbIsFeatureSupported with
// mbFeature_AudioCapture to verify).
// ****************************************************************************

// mbInitializeInputHardware
// ----------------------------------------------------------------------------
// Initializes input hardware related resources which are required before other
// InputHardware functions can be called.
//
mbResult			mbInitializeInputHardware(
								mbObjectID system);

// mbUninitializeInputHardware
// ----------------------------------------------------------------------------
// Releases input hardware resources.
//
mbResult			mbUninitializeInputHardware(
								mbObjectID system);

// mbEngageInputHardware
// --------------------------------------------------
// Engages the input hardware so that input can be captured in raw PCM using an
// mbIOCallback or in WAVE file format directly to a file or a block of memory.
// If a file is specified and it already exists, it will be overwritten.
// mbEngageInputHardwareFile will fail with a result of mbFeature_Not_Supported
// if the capture type specified is mbCapture_WaveFormatFile and the current
// build of mobileBAE does not support file IO (call mbIsFeatureSupported with
// mbFeature_AudioCapture to verify).
//
mbResult			mbEngageInputHardware(
								mbObjectID system,
								mbInputHardwareCaptureType type,
								const mbInputHardwareCaptureParams *params);

// mbDisengageInputHardware
// --------------------------------------------------
// Disengages the input hardware.
//
mbResult			mbDisengageInputHardware(
								mbObjectID system);

// mbIsInputHardwareEngaged
// --------------------------------------------------
// Returns whether the input hardware is currently engaged.
//
mbBool				mbIsInputHardwareEngaged(
								mbObjectID system,
								mbResult *outResult);

// mbPauseInputHardwareCapture
// --------------------------------------------------
// Pauses capture of input, but does not disengage the hardware.
// Callbacks will not fire, file and memory will not be updated while paused.
//
mbResult			mbPauseInputHardwareCapture(
								mbObjectID system);

// mbResumeInputHardwareCapture
// --------------------------------------------------
// Resumes capture of previously paused input, but does not reengage the
// hardware.
// Callbacks will resume to be fired, file and memory will resume to be
// updated.
//
mbResult			mbResumeInputHardwareCapture(
								mbObjectID system);

// mbGetInputHardwareCaptureStatus
// --------------------------------------------------
// Returns running capture status.
// Helpful when input hardware was engaged with mbInputHardwareCaptureType
// of mbCapture_WaveFormatMemory or mbCapture_WaveFormatFile.
// Status is retained after mbDisengageInputHardware until the next
// call to mbEngageInputHardware or mbInitializeInputHardware.
//
mbResult			mbGetInputHardwareCaptureStatus(
								mbObjectID system);

// mbGetInputHardwareCaps
// --------------------------------------------------
// Fills out the passed-in struct with information about the capabilities
// of the input audio hardware.
//
mbResult			mbGetInputHardwareCaps(
								mbObjectID system,
								mbHardwareCaps *outCaps);

// mbIsInputHardwareFormatSupported
// --------------------------------------------------
// Returns whether the input hardware supports a given audio format (bit
// depth, sample type, channel count, sample rate, etc.).
//
mbBool				mbIsInputHardwareFormatSupported(
								mbObjectID system,
								const mbAudioFormat * const format,
								mbResult *outResult);

// mbSetInputHardwareAudioFormat
// --------------------------------------------------
// Set the audio format parameters all at once. This is necessary in cases
// where one parameter is dependent on another.
//
mbResult			mbSetInputHardwareAudioFormat(
								mbObjectID system,
								const mbAudioFormat * const format,
								mbUInt32 frames);

// mbGetInputHardwareAudioFormat
// --------------------------------------------------
// Returns the audio format currently being used by the input hardware.
//
mbResult			mbGetInputHardwareAudioFormat(
								mbObjectID system,
								mbAudioFormat *outAudioFormat,
								mbUInt32 *outFrames);

// mbSetInputHardwareBitDepth
// --------------------------------------------------
// Sets the bit depth setting of the input hardware.
//
mbResult			mbSetInputHardwareBitDepth(
								mbObjectID system,
								mbInt16 bitDepth);

// mbGetInputHardwareBitDepth
// --------------------------------------------------
// Returns the bit depth setting of the input hardware.
//
mbInt16				mbGetInputHardwareBitDepth(
								mbObjectID system,
								mbResult *outResult);

// mbSetInputHardwareSampleRate
// --------------------------------------------------
// Set the input sampling rate of the hardware in frames/second.
//
mbResult			mbSetInputHardwareSampleRate(
								mbObjectID system,
								mbUInt32 sampleRate);

// mbGetInputHardwareSampleRate
// --------------------------------------------------
// Get the input sampling rate of the hardware in frames/second.
//
mbUInt32			mbGetInputHardwareSampleRate(
								mbObjectID system,
								mbResult *outResult);

// mbSetInputHardwareChannelCount
// --------------------------------------------------
// Set the number of input hardware interleaved audio channels.
//
mbResult			mbSetInputHardwareChannelCount(
								mbObjectID system,
								mbInt16 channelCount);

// mbGetInputHardwareChannelCount
// --------------------------------------------------
// Get the number of input hardware interleaved audio channels.
//
mbInt16				mbGetInputHardwareChannelCount(
								mbObjectID system,
								mbResult *outResult);

// mbGetInputHardwareFrameCount
// --------------------------------------------------
// Return the number of sample frames that have been received from
// the input hardware since it was engaged.
//
mbUInt32			mbGetInputHardwareFrameCount(
								mbObjectID system,
								mbResult *outResult);



// Deprecated / Obsolete Functions
// ****************************************************************************
// These functions are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// mbCreateSystem
// --------------------------------------------------
// This function has been replaced by mbCreateEngageSystem.
// Note that the parameter 'useAutoNotifier' is no longer supported: mobileBAE
// always attempts to use the auto notification system if it is available.
//
#define mbCreateSystem(midiVoices, audioVoices, useAutoNotifier, result) mbCreateEngageSystem((midiVoices), (audioVoices), mbFalse, NULL, (result))

// mbCreateSystemEx
// --------------------------------------------------
// This function has been replaced by mbCreateEngageSystem.
// Note that the parameter 'useAutoNotifier' is no longer supported: mobileBAE
// always attempts to use the auto notification system if it is available.
//
#define mbCreateSystemEx(midiVoices, audioVoices, useAutoNotifier, context, result) mbCreateEngageSystem((midiVoices), (audioVoices), mbFalse, (context), (result))

// mbDestroySystem
// --------------------------------------------------
// This function has been replaced by the generic mbDestroy.
//
#define mbDestroySystem						mbDestroy

// mbGetSystemVersion
// --------------------------------------------------
// This function has been replaced by mbGetCoreVersion.
//
#define mbGetSystemVersion					mbGetCoreVersion

// mbGetSystemVersionString
// --------------------------------------------------
// This function has been replaced by mbGetCoreVersionString.
//
#define mbGetSystemVersionString			mbGetCoreVersionString

// mbIs/AreSupported
// --------------------------------------------------
// These functions have been replaced with mbIsFeatureSupported.
//
#define mbAreAudioPlayersSupported()		mbIsFeatureSupported(mbFeature_DigitalAudio)
#define mbAreCallbacksSupported()			mbIsFeatureSupported(mbFeature_Callbacks)
#define mbAreCollectionsSupported()			mbIsFeatureSupported(mbFeature_Collections)
#define mbAreMarkersSupported()				mbIsFeatureSupported(mbFeature_Markers)
#define mbAreRingtonePlayersSupported()		mbIsFeatureSupported(mbFeature_RingtonePlayers)
#define mbIsAudioCaptureSupported()			mbIsFeatureSupported(mbFeature_AudioCapture)
#define mbIsAutoDisconnectSupported()		mbIsFeatureSupported(mbFeature_AutoDisconnect)
#define mbIsChorusSupported()				mbIsFeatureSupported(mbFeature_Chorus)
#define mbIsDRCSupported()					mbTrue
#define mbIsReverbSupported()				mbIsFeatureSupported(mbFeature_Reverb)

// The following functions are obsolete beginning with mobileBAE 1.4:
// mbSetDefaultBankFromFile
// mbSetDefaultBankFromMemory
// mbClearDefaultBank
// mbQueryDefaultBankInfo
// mbSetRendererMasterAudioPlayerVolume
// mbGetRendererMasterAudioPlayerVolume
// mbSetRendererVoiceMixCount
// mbGetRendererVoiceMixCount
// mbGetSystemCPULoad

// The following function is obsolete beginning with mobileBAE 1.5:
// mbGetAutoDisconnectStatus

#endif // _MBAE_SYSTEM_H_
