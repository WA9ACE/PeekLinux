/******************************************************************************
**
**	mBAE_Types.h
**
**	Standard types used by mobileBAE.
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
**	2002.03.13 AER	Cleaned up some enums (to default to undefined).
**	2002.03.21 AER	Added mbTimeCallback and mbIOCallback.
**	2002.03.22 AER	Added mbInvalid_Object.
**					Added mbCorrupt_ObjectID.
**	2002.04.01 SME	Revised unicode compile options (AC_CHARSET_*).
**	2002.04.02 SME	Made mbCHARSET_ASCII the default mbCHARSET_* symbol.
**	2002.04.12 AER	Deprecated mbPlayer_Not_Prerolled and mbPlayer_Cannot_Seek.
**					Added mbPlayer_Operation_Error.
**	2002.04.16	DS	Added mbHardware_Err.
**	2002.04.18 msd	removed unused mbResult codes
**  2002.04.23 tom  #undef TRUE and FALSE before enum'ing them
**	2002.06.09 AER	Added new type, mbMilliTime. mbTime will soon change to
**					milliseconds, and mbMilliTime will be deprecated.
**	2002.06.11 AER	Added new type, mbMilliUnit, to replace use of float in the
**					Client API. mbMilliUnit represents thousandths of a unit.
**					Cleaned up formatting and arguments lists.
**	2002.06.12 AER	Deprecated mbFloat and mbTime.
**	2002.06.14 AER	SampleRate converted to an mbUInt32.
**	2002.06.21 AER	Replaced usage of mbMilliTime with mbTime.
**	2002.06.24 AER	Renamed mbTimeCallback mbMixerPulseCallback and moved it
**					from mBAE_Types.h to mBAE_System.h.
**					Moved mbIOCallback from mBAE_Types.h to mBAE_System.h.
**	2002.07.15 AER	Added mbCopy_Required.
**	2002.08.02 msd	Added SMS and RTX file types.
**	2002.08.08 msd	Added iMelody and SMAF file types
**	2002.09.12	DS	Added mbParam struct.
**	2002.10.22 AER	For Symbian builds, mbCHARSET* has been renamed MBCHARSET*.
**	2002.10.25 AER	Replaced mbUnknown_Audio_Codec and mbAudio_Codec_Not_Found
**					with mbAudio_Codec_Not_Supported
**					Added mbFile_Type_Not_Supported.
**	2003.02.04 msd	Moved type and param declarations from other headers
**	2003.02.06 AER	Altered mbObjectID and added mbSystemID.
**	2003.02.11 AER	Added a context to mbStreamCallback.
**	2003.05.13 SME	Support for runtime struct packing check.
**	2003.07.10 AER	[Bug 1122] Fixed problem with definition of mbT on some
**					compilers. Improved fix with addition of const qualifier.
**	2003.07.15 msd	Added Sysex Callback
**	2003.07.17 AER	[Bug 1054] Exposed mobileBAE version number as constants.
**	2003.07.30	DS	[bug 1166] fixed NULL to be 0 rather than 0L.
**	2003.11.26 SME	Mixer pulse callback no longer returns system time.
**	2003.12.19 SME	First phase of mBAE 1.4 metadata overhaul.
**	2003.12.23 AER	Tidied, incremented mbVersionMinor.
**	2004.01.06 SME	Added new mbResult mbNo_Longer_Valid.
**	2004.01.08 AER	Removed mbSystemID to reflect that system is now a regular
**					mbObjectID (it remains for backward compatibility).
**					Added mbMUAccessToken for multi-user access.
**	2004.01.09	DS	Added struct and enums for input and output HW caps functions.
**	2004.01.12 AER	Added mbPermission_Denied to mbResult.
**	2004.01.16 SME	Added input hardware capture types/structs.
**	2004.01.22 AER	Renamed mbPermission_Denied mbAuthorization_Error.
**	2004.01.22 AER	Removed mbMUAccessToken; added safer mbAuthorizationKey.
**	2004.01.23	DS	Added mbFirst_Bank and mbLast_Bank and mbBank_Already_Loaded.
**	2004.01.28 SME	Added mbTimeCodeFormat and mbTimeCode.
**	2004.01.30 AER	Added mbFeatureType.
**	2004.02.06 SME	Added mbTag and MB_FOUR_CHAR.
**	2004.02.13 AER	Replaced mbInput_FileType with mbFileType and identified
**					RMF files with mbFileType_RMF (replacing IREZ).
**					Added numerous mbFeature types.
**					Replaced mbFourChar with mbFourChar.
**					Moved mbControlType in from defunct mBAE_Player.h.
**	2004.02.19 AER	Gave mbFeatureTypes mbFourChar values and tidied file.
**	2004.03.02 AER	Fixed problem with stream mbResults by creating a new enum,
**					mbStreamResult, and altering mbStreamCallback to use it.
**	2004.03.04	DS	Major overhaul of error handling.
**			   AER	Removed mbPlayer_Operation_Error; it has been superseded by
**					mbOperation_Not_Supported.
**					Reorganized mbResult and documented some results.
**					Included backward compatibility mbResult translations.
**					Deprecated mbPlayer_Not_Playing in favor of mbNot_Playing.
**	2004.03.05 AER	Improved documentation of mbResult codes.
**	2004.03.15 AER	Improved documentation of mbResult codes.
**	2004.05.19 AER	Updated version to 1.4.1.
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**	2004.08.09 SME	[bug 2296/2297] Changed definition of mbChar in UTF32 builds.
**	2004.09.14 SME	Added support for BSB bank files.
**	2004.10.03 AER	Removed obsolete auto-disconnect type.
**	2004.10.20 AER	Added mbFeature_FileType_BSB.
**	2004.10.21 AER	Added mbFeature_MobileDLSPlus.
**	2004.10.29 AER	[Bug 2620] Fixed version number.
**	2004.11.03 SME	Added bank image filetype.
**	2004.11.05 AER	Removed mbFileType_DLS.
**	2004.11.08 AER	Added mbFeature_JSR135, mbFeature_FileType_JTS, and
**					mbFileType_JTS.
**
******************************************************************************/

#ifndef _MBAE_TYPES_H_
#define _MBAE_TYPES_H_



// System Version
// ****************************************************************************
// Assemble the mobileBAE version as major.minor.sub-minor.
// ****************************************************************************

#define mbVersionMajor		1		// Major version of mobileBAE
#define mbVersionMinor		5		// Minor version of mobileBAE
#define mbVersionSubMinor	0		// Sub-Minor version of mobileBAE



// Basic Types
// ****************************************************************************
// Standard type definitions
// ****************************************************************************

typedef long				mbMilliUnit;	// Fractional value in thousandths
typedef unsigned char		mbByte;
typedef unsigned char		mbUInt8;
typedef signed char			mbInt8;
typedef unsigned short int	mbUInt16;
typedef short int			mbInt16;
typedef unsigned long		mbUInt32;
typedef long				mbInt32;
typedef mbUInt8				mbBool;

typedef void *				mbObjectID;
typedef unsigned long		mbControlID;
typedef unsigned long		mbTime;			// Time units in milliseconds.
typedef unsigned char		mbBank;
typedef unsigned char		mbProgram;
typedef unsigned long		mbTag;

// One of the following must be defined for proper string handling:
// MBCHARSET_ASCII
// MBCHARSET_UTF8
// MBCHARSET_UTF16
// MBCHARSET_UTF32

/* x0056422 - OMAPS00156759 Dtd:Dec 30,2007  */
/* the flag BAE_UNICODE decides the encoding used */
#if (BAE_UNICODE == 0)
  #define MBCHARSET_ASCII
#elif (BAE_UNICODE == 1)
  #define MBCHARSET_UTF16
#else
  #error "FS_UNICODE not defined"
#endif

/* End x0056422 - OMAPS00156759 Dtd:Dec 30,2007 */


#if defined(MBCHARSET_UTF8)
	typedef char					mbChar;
	#define mbT(x)					(const mbChar *) x
#elif defined(MBCHARSET_UTF16)
	typedef unsigned short			mbChar;
	#define mbT(x)					(const mbChar *) L ## x
#elif defined(MBCHARSET_UTF32)
	typedef long					mbChar;
	#define mbT(x)					(const mbChar *) L ## x
#else
	#if !defined(MBCHARSET_ASCII)
		#define MBCHARSET_ASCII
	#endif
	typedef char					mbChar;
	#define mbT(x)					(const mbChar *) x
#endif // MBCHARSET_*

typedef const mbChar *				mbFilePath;
typedef const mbChar *				mbString;

// mbResult
// --------------------------------------------------
// Result codes returned by mobileBAE.
//
typedef enum
{
	// Success.
	mbNo_Err						=  0,

	// mobileBAE has encountered an unexpected and unhandled internal error.
	// Please contact technical support.
	mbInternal_Error				=  1,

	// mobileBAE is not configured for this functionality due to the compile-
	// time configuration used by the platform implementor.
	mbFeature_Not_Supported			=  2,

	// The operation failed because the object was instantiated in a way that
	// prevents support for the operation (e.g., attempting to set position on
	// a non-seekable audio stream player or setting an underflow callback on
	// a buffered audio file player), or because support for this operation is
	// temporarily disabled on the object (e.g., setting volume on an object
	// that is in the middle of a fade or prerolling an object that is playing
	// or paused).
	mbOperation_Not_Enabled			=  3,

	// The requested function does not accept this type of object.
	mbInvalid_Object				=  4,

	// The function's object ID context is not a valid mobileBAE object.
	// This is the ID passed to mobileBAE as the first parameter in the vast
	// majority of functions, and is similar to a 'this' pointer in C++.
	mbCorrupt_ObjectID				=  5,

	// Client passed a bad or NULL argument.
	mbBad_Parameter					=  6,

	// Client passed an illegal value for a parameter (e.g., out of range...).
	mbBad_Value						=  7,

	// The item in question could not be located.
	mbNot_Found						=  8,

	// The object was not initialized correctly.
	mbNot_Set_Up					=  9,

	// The object was already initialized correctly and cannot be initialized
	// twice.
	mbAlready_Set_Up				= 10,

	// mobileBAE has encountered a memory allocation failure.
	mbMemory_Error					= 11,

	// There has been a security failure within mobileBAE, such as an attempt
	// to load a file without access privilege or DRM permission or an attempt
	// to authorize an unacceptable key.
	mbAuthorization_Error			= 12,

	// The file was not found.
	mbFile_Not_Found				= 13,

	// The file is already in use and cannot be modified.
	mbFile_In_Use					= 14,

	// The file is unrecognizable either because mobileBAE was not configured
	// to handle this file type or because the mbFileType passed to mobileBAE
	// did not match the actual file.
	mbFile_Type_Not_Supported		= 15,

	// The file is either corrupt or unreadable.
	mbBad_File						= 16,

	// The file is recognized by mobileBAE, but contains elements that
	// mobileBAE cannot handle (e.g., SMF type 2, DLS 2 articulations in a
	// Mobile DLS file, unsupported XMF reference types, etc.).
	mbFile_Feature_Not_Supported	= 17,

	// The file requires an audio codec that is not supported. This may be due
	// to the build configuration, or because mobileBAE does not recognize the
	// type of audio codec.
	mbAudio_Codec_Not_Supported		= 18,

	// The file requires a decompressor that is not supported. This may be due
	// to the build configuration, or because mobileBAE does not recognize the
	// type of decompressor.
	mbDecompressor_Not_Supported	= 19,

	// The file requires a decryptor that is not supported. This may be due to
	// the build configuration, or because mobileBAE does not recognize the
	// type of decryptor.
	mbDecryptor_Not_Supported		= 20,

	// The file requires an unpacker that is not supported. This may be due to
	// the build configuration, or because mobileBAE does not recognize the
	// type of unpacker.
	mbUnpacker_Not_Supported		= 21,

	// The audio format is not supported. This may be due to build
	// configuration, or because mobileBAE or the audio hardware does not
	// support the format.
	mbAudio_Format_Not_Supported	= 22,

	// The requested operation failed because it would require mobileBAE to
	// copy the data.
	mbCopy_Required					= 23,

	// Operation requires that the object have a file loaded.
	mbNot_Loaded					= 24,

	// Operation requires that the object be playing.
	mbNot_Playing					= 25,

	// The audio hardware must be engaged for this operation to succeed.
	mbHardware_Not_Engaged			= 26,

	// The audio hardware is already engaged.
	mbHardware_Already_Engaged		= 27,

	// Error configuring, engaging, or disengaging the audio hardware.
	mbHardware_Error				= 28,

	// mobileBAE requires that the client must call mbServiceIdle as noted in
	// mbCreateEngageSystem.
	mbServiceIdle_Required			= 29,

	// mobileBAE cannot function because it has been incorrectly configured and
	// built by the platform implementor. The problem has to do with how the
	// platform implementor has declared the type of struct packing used to
	// build mobileBAE.
	mbStruct_Packing_Error			= 30,

	// No default soundbank has been loaded.
	mbNo_Default_Bank				= 31,

	// The requested soundbank has already been loaded.
	mbBank_Already_Loaded			= 32,

	// A required instrument could not be found.
	mbInstrument_Not_Found			= 33,

	// Failure doe to an attempt to unload via an aliased key.
	mbKey_Is_Aliased				= 34,

	// The audio codec failed to decode the sample data (applicable to calls to
	// mbPreroll).
	mbAudio_Codec_Error				= 35,

	// The operation would exceed voice limit, etc.
	mbLimit_Exceeded				= 36,

	// The returned value may be unreliable due to overflow (e.g., a song that
	// is longer than 71 minutes, the size of a digital audio stream that does
	// not store a length, etc.).
	mbValue_Unreliable				= 37,

	// The operation does not recognize this parameter.
	mbUnknown_Param_Token			= 38,

	// An error occurred on one of the objects in the group player during the
	// operation. Call one of the mbGetLastGroupResult functions to locate the
	// specific failure per contained object.
	mbGroup_Operation_Error			= 39,

	// The string could not be converted to or from ASCII or Unicode.
	mbString_Conversion_Error		= 40,

	// The mbObjectID is no longer valid (e.g., a new system bank was loaded
	// invalidating a previously created bank mbObjectID).
	mbNo_Longer_Valid				= 41,

	mbResult_Count
} mbResult;


#define mbTrue		1
#define mbFalse		0

#ifndef NULL
#define NULL		0
#endif



// Renderer bank position constants. Use these to prepend or append banks without
// needing to know the current bank count.
enum
{
	mbFirst_Bank			= 0,
	mbLast_Bank				= 65535
};



// Utilities
// ****************************************************************************
// ****************************************************************************

// Macro used to create an mbTag value from four literal ASCII characters.
// For example:	mbTag resourceTag = mbFourChar('M','B','A','E');
#define mbFourChar(ch1,ch2,ch3,ch4)	((mbUInt32)(mbUInt8)(ch4) | \
									((mbUInt32)(mbUInt8)(ch3) << 8) | \
									((mbUInt32)(mbUInt8)(ch2) << 16) | \
									((mbUInt32)(mbUInt8)(ch1) << 24))



// Advanced Types
// ****************************************************************************
// Feature, control, file, byte order, audio/sample format, etc.
// ****************************************************************************

// mbFeatureType
// --------------------------------------------------
// Types describing mobileBAE engine features. Use these to query the function
// mbIsFeatureSupported.
//
typedef enum
{
	mbFeature_Undefined					= 0,
	mbFeature_AudioCapture				= mbFourChar('a','c','a','p'),
	mbFeature_AutoDisconnect			= mbFourChar('d','i','s','c'),
	mbFeature_Callbacks					= mbFourChar('c','a','l','l'),
	mbFeature_Chorus					= mbFourChar('c','h','o','r'),
	mbFeature_Collections				= mbFourChar('c','o','l','l'),
	mbFeature_DefaultCreate				= mbFourChar('d','f','l','t'),
	mbFeature_DigitalAudio				= mbFourChar('d','i','g','a'),
	mbFeature_FileIO					= mbFourChar('f','i','l','e'),
	mbFeature_Filter					= mbFourChar('f','i','l','t'),
	mbFeature_JSR135					= mbFourChar('j','1','3','5'),
	mbFeature_Markers					= mbFourChar('m','a','r','k'),
	mbFeature_Metadata					= mbFourChar('m','e','t','a'),
	mbFeature_MidiNormalizer			= mbFourChar('m','n','r','m'),
	mbFeature_MobileDLSPlus				= mbFourChar('d','l','s','p'),
	mbFeature_Reverb					= mbFourChar('r','v','b','_'),
	mbFeature_RingtonePlayers			= mbFourChar('r','i','n','g'),
	mbFeature_StereoInput				= mbFourChar('s','i','n','_'),
	mbFeature_StereoOutput				= mbFourChar('s','o','u','t'),
	mbFeature_SynthControl				= mbFourChar('s','y','n','_'),
	mbFeature_Decoder_G711				= mbFourChar('d','7','1','1'),
	mbFeature_Decoder_IMA_ADPCM			= mbFourChar('d','i','m','a'),
	mbFeature_Decoder_MPEG				= mbFourChar('d','m','p','g'),
	mbFeature_FileType_AIFF				= mbFourChar('f','a','i','f'),
	mbFeature_FileType_AU				= mbFourChar('f','a','u','_'),
	mbFeature_FileType_BSB				= mbFourChar('f','b','s','b'),
	mbFeature_FileType_iMelody			= mbFourChar('f','i','m','y'),
	mbFeature_FileType_JTS				= mbFourChar('f','j','t','s'),
	mbFeature_FileType_MPEG				= mbFourChar('f','m','p','g'),
	mbFeature_FileType_RMF				= mbFourChar('f','r','m','f'),
	mbFeature_FileType_RTX				= mbFourChar('f','r','t','x'),
	mbFeature_FileType_SMAF_MA1			= mbFourChar('f','s','f','1'),
	mbFeature_FileType_SMAF_MA2			= mbFourChar('f','s','f','2'),
	mbFeature_FileType_SMAF_MA3			= mbFourChar('f','s','f','3'),
	mbFeature_FileType_SMS				= mbFourChar('f','s','m','s'),
	mbFeature_FileType_XMF				= mbFourChar('f','x','f','_'),
	mbFeature_FileType_XMF_Compressed	= mbFourChar('f','x','f','c'),
	mbFeature_FileType_XMF_Encrypted	= mbFourChar('f','x','f','e')
} mbFeatureType;

// mbControlType
// --------------------------------------------------
// Types describing the ability of a player to handle functions from a control.
// Use these to query the function mbIsControlSupported.
//
typedef enum
{
	mbControlType_Undefined				= 0,
	mbControlType_Audible				= mbFourChar('h','e','a','r'),
	mbControlType_Load					= mbFourChar('l','o','a','d'),
	mbControlType_Player				= mbFourChar('p','l','a','y'),
	mbControlType_MidiPlayer			= mbFourChar('m','i','d','i'),
	mbControlType_AudioPlayer			= mbFourChar('w','a','v','e'),
	mbControlType_DRC					= mbFourChar('d','r','c','c'),
	mbControlType_Metadata				= mbFourChar('m','e','t','a'),
	mbControlType_Tempo					= mbFourChar('b','e','a','t'),
	mbControlType_Terminal				= mbFourChar('t','e','r','m'),
	mbControlType_Resource				= mbFourChar('r','s','r','c')
} mbControlType;

// mbFileType
// --------------------------------------------------
// File types passed to load functions.
//
typedef enum
{
	mbFileType_Undefined	= 0,
	mbFileType_AIFF			= 50,
	mbFileType_AU			= 51,
	mbFileType_MPEG			= 52,
	mbFileType_WAVE			= 53,
	mbFileType_DLS_GS		= 54,	// DLS file, bank layout: MSB N, LSB 0
	mbFileType_DLS_GM2		= 55,	// DLS file, bank layout: MSB 0x78,0x79 for GM2, other MSBs are for custom instruments
	mbFileType_RMF			= 56,
	mbFileType_SMF			= 57,
	mbFileType_XMF			= 58,
	mbFileType_iMelody		= 59,
	mbFileType_SMS			= 60,
	mbFileType_RTX			= 61,
	mbFileType_SMAF			= 62,
	mbFileType_BSB			= 63,
	mbFileType_BBI			= 64,
	mbFileType_JTS			= 65
} mbFileType;

// mbTimeCodeFormat
// --------------------------------------------------
// Time code formats that specify the format used in mbTimeCode.
// All formats are non-drop frame.
//
typedef enum
{
	mbTimeCodeFormat_Undefined		= 0,
	mbTimeCodeFormat_24fps			= 60,
	mbTimeCodeFormat_25fps			= 61,
	mbTimeCodeFormat_30fps			= 62
} mbTimeCodeFormat;

// mbTimeCode
// --------------------------------------------------
// Parameters which define a time position.
//
typedef struct _mbTimeCode
{
	mbUInt8		mHours;				// range: 0 - 23
	mbUInt8		mMinutes;			// range: 0 - 59
	mbUInt8		mSeconds;			// range: 0 - 59
	mbUInt8		mFrames;			// range: depends on mbTimeCodeFormat in effect (0 - 24, 0 - 25, 0 - 30)
	mbUInt8		mSubFrames;			// range: 0 - 99
} mbTimeCode;

// mbByteOrder
// --------------------------------------------------
// Byte order of data
//
typedef enum
{
	mbByteOrder_Undefined		= 0,
	mbByteOrder_BigEndian		= 50,
	mbByteOrder_LittleEndian	= 51
} mbByteOrder;

// mbSampleType
// --------------------------------------------------
// Data types for audio samples.
//
typedef enum
{
	mbSampleType_Undefined	= 0,
	mbSampleType_Unsigned	= 50,
	mbSampleType_Signed		= 51,
	mbSampleType_Float		= 52
} mbSampleType;

// mbAudioFormat
// --------------------------------------------------
// Data structure describing an audio signal.
//
typedef struct _mbAudioFormat
{
	mbInt16			mBitDepth;		// 8, 16, etc.
	mbUInt32		mSampleRate;	// Frames/second: 22050, 44100, etc.
	mbInt16			mChannels;		// 1, 2, 4
	mbSampleType	mSampleType;	// mbFormat_Unsigned, etc.
} mbAudioFormat;

// mbSampleInfo
// --------------------------------------------------
// Data structure describing a PCM sample.
//
typedef struct _mbSampleInfo
{
	mbAudioFormat	mSampleFormat;	// bitdepth, type, channels, sample rate
	mbUInt8			mBaseMidiPitch;	// base Midi pitch of underlying sample,
									// i.e., 60 is middle 'C'
	mbUInt32		mWaveSize;		// total waveform size in bytes
	mbInt32			mWaveFrames;	// total number of frames
	mbInt32			mStartLoop;		// start loop point offset
	mbInt32			mEndLoop;		// end loop point offset
} mbSampleInfo;



// System Type Definitions
// ****************************************************************************
// ****************************************************************************

// mbMixerPulseCallback
// --------------------------------------------------
// Mixer pulse callback.
//
typedef void		(*mbMixerPulseCallback) (
								void *context);

// mbIOCallback
// --------------------------------------------------
// Input/Output callback used by System and Input Hardware.
//
typedef void		(*mbIOCallback) (
								void *context,
								void *samples,
								mbAudioFormat sampleFormat,
								mbUInt32 lengthInFrames);

// mbVoiceType
// --------------------------------------------------
// Description of type of a voice.
//
typedef enum
{
	mbVoice_Undefined	= 0,
	mbVoice_MIDI		= 50,
	mbVoice_AudioPlayer = 51
} mbVoiceType;

// mbVoiceInfo
// --------------------------------------------------
// Container for real time voice information retrieved via
// mbGetRendererRealtimeStatus.
//
typedef struct _mbVoiceInfo
{
	mbUInt8			mVoiceIndex;	// voice index
	mbVoiceType		mVoiceType;		// voice type
	mbProgram		mProgram;		// current program (0 - 127)
	mbBank			mBankMSB;		// current bank MSB
	mbBank			mBankLSB;		// current bank LSB (0 - 127)
	mbUInt8			mMidiVolume;	// current volume (0 - 127)
	mbUInt8			mChannel;		// current channel (0 - 15)
	mbUInt8			mMidiNote;		// current midi note (0 - 127)
} mbVoiceInfo;

// Hardware Capabilities Definitions
// ****************************************************************************
// ****************************************************************************

// Possible values for supported HW sample formats.
enum
{
	mbHardwareCAPS_Unsigned8Bit		= 0x1,
	mbHardwareCAPS_Signed8Bit		= 0x2,
	mbHardwareCAPS_Unsigned16Bit	= 0x4,
	mbHardwareCAPS_Signed16Bit		= 0x8,
	mbHardwareCAPS_Float			= 0x10
};

// Possible values for supported HW channel counts.
enum
{
	mbHardwareCAPS_Mono				= 0x1,
	mbHardwareCAPS_Stereo			= 0x2,
	mbHardwareCAPS_4Channel			= 0x4,
	mbHardwareCAPS_8Channel			= 0x8
};

// Possible values for supported HW sampling rates
// Note that other rates may be supported, but an application would have to
// perform a specific query (via mbIsOutputHardwareFormatSupported) to
// determine if a particular rate is supported.
enum
{
	mbHardwareCAPS_SR_8000			= 0x1,
	mbHardwareCAPS_SR_11025			= 0x2,
	mbHardwareCAPS_SR_16000			= 0x4,
	mbHardwareCAPS_SR_22050			= 0x8,
	mbHardwareCAPS_SR_24000			= 0x10,
	mbHardwareCAPS_SR_32000			= 0x20,
	mbHardwareCAPS_SR_40000			= 0x40,
	mbHardwareCAPS_SR_44100			= 0x80,
	mbHardwareCAPS_SR_48000			= 0x100
};

// Miscellaneous boolean capabilities (for output hardware only).
// These are set in 'otherCaps', below.
enum
{
	mbOutputHardwareCAPS_Volume		= 0x1,		// HW supports volume setting
	mbOutputHardwareCAPS_Balance	= 0x2,		// HW supports balance setting
	mbOutputHardwareCAPS_Latency	= 0x4		// HW supports setting of latency
};

// Container for input and output hardware capabilities retrieved via
// mbGetOutputHardwareCaps and mbGetInputHardwareCaps.
typedef struct _mbHardwareCaps
{
	mbUInt32			mAudioFormats;			// bitmask of supported formats
	mbUInt32			mAudioChannelCounts;	// bitmask of supported channel counts
	mbUInt32			mAudioSampleRates;		// bitmask of supported sampling rates
	mbUInt32			mMinLatency;			// in msec.
	mbUInt32			mMaxLatency;			// in msec.
	mbUInt32			mDefaultLatency;		// in msec.
	mbUInt32			mOtherCaps;				// bitmask of misc caps (volume, balance, etc.)
} mbHardwareCaps;



// System Input Hardware Types
// ****************************************************************************
// ****************************************************************************

// mbInputHardwareCaptureType
// --------------------------------------------------
// Capture types that are used to specify the type of input hardware capture
// that should occur when used with mbEngageInputHardware.
//
typedef enum
{
	mbCapture_RawSampleCallback,
	mbCapture_WaveFormatFile,
	mbCapture_WaveFormatMemory
} mbInputHardwareCaptureType;

// mbCaptureToCallbackParams
// --------------------------------------------------
// Parameters required when mbCapture_RawSampleCallback is passed to
// mbEngageInputHardware.
// The callback includes a pointer to the audio data currently received from
// the audio input hardware.
//
typedef struct _mbCaptureToCallbackParams
{
	mbIOCallback 		mCallback;
	void				*mContext;
} mbCaptureToCallbackParams;

// mbCaptureToFileParams
// --------------------------------------------------
// Parameters required when mbCapture_WaveFormatFile is passed to
// mbEngageInputHardware.
// If the specified file exists, it will be overwritten.
//
typedef struct _mbCaptureToFileParams
{
	mbFilePath			mFilepath;
} mbCaptureToFileParams;

// mbCaptureToMemoryParams
// --------------------------------------------------
// Parameters required when mbCapture_WaveFormatMemory is passed to
// mbEngageInputHardware.
// Input is captured to the specified block of memory in WAVE file format.
//
typedef struct _mbCaptureToMemoryParams
{
	mbByte				*mMemoryBlock;
	mbUInt32			mBlockSize;
} mbCaptureToMemoryParams;

// mbInputHardwareCaptureParams
// --------------------------------------------------
// Used to specify required parameters in calls to mbEngageInputHardware.
// Interpretation depends upon value of mbInputHardwareCaptureType that is also
// specified in the call to mbEngageInputHardware.
//
typedef union _mbInputHardwareCaptureParams
{
	mbCaptureToCallbackParams		mCallbackParams;
	mbCaptureToFileParams			mFileParams;
	mbCaptureToMemoryParams			mMemoryParams;
} mbInputHardwareCaptureParams;



// DRC Parameters
// ****************************************************************************
// ****************************************************************************

/* x0083025 - OMAPS00156759 - Jan 29, 2008 - Start */
/* changing constants from strings to enum type which can be compared without problems */

#if(0)
// mbDRC_Threshold is the parameter token that allows control over the minimum
// loudness above which the compressor works.
// The mbParamValue matching this token should be an mbMilliUnit with a range
// of 0 to 60000, which scales to 0 dB and -60 dB.
#define mbDRC_Threshold		mbT("DRC Threshold")

// mbDRC_Ratio is the parameter token that allows control over the ratio or
// compression/limiting applied to sound.
// The mbParamValue matching this token should be an mbMilliUnit with a range of
// 1000 to 10000, which scales to 1:1 and 10:1 compression.
#define mbDRC_Ratio			mbT("DRC Ratio")

// mbDRC_AttackTime is the parameter token that allows control over the attack
// time of the compressor.
// The mbParamValue matching this token should be an mbTime measuring the
// attack time in milliseconds.
#define mbDRC_AttackTime	mbT("DRC Attack Time")	// Range 0 to 1000

// mbDRC_ReleaseTime is the parameter token that allows control over the
// release time of the compressor.
// The mbParamValue matching this token should be an mbTime measuring the
// release time in milliseconds.
#define mbDRC_ReleaseTime	mbT("DRC Release Time")	// Range 0 to 10000
#endif

typedef enum 
{
	mbDRC_Threshold,
	mbDRC_Ratio, 
	mbDRC_AttackTime,	// Range 0 to 1000
	mbDRC_ReleaseTime   // Range 0 to 10000
} mbDRC_PARAMETER;


// Token describing a particular parameter.
//typedef const mbChar *				mbParamToken;
typedef mbDRC_PARAMETER 			mbParamToken;

// Value for a particular parameter.
typedef mbMilliUnit					mbParamValue;

// Container for token/value pairs. The last token in any array should be set
// to NULL.
typedef struct _mbParam
{
	mbParamToken	mToken;
	mbParamValue	mValue;
} mbParam;

/* x0083025 - OMAPS00156759 - Jan 29, 2008 - End */

// Chorus Parameters
// ****************************************************************************
// ****************************************************************************

// mbChorus_Type is the parameter token that allows control over chorus type.
// The mbParamValue matching this token should be one of the following values
// given by the enum mbChorusType, below.
#define mbChorus_Type		mbT("Chorus Type")

// mbChorusType
// --------------------------------------------------
// mbChorusType is to be used as an mbParamValue to determine the GM2-defined
// chorus type.
//
typedef enum
{
	mbChorus_Type_1			= 0,
	mbChorus_Type_2			= 1,
	mbChorus_Type_3			= 2,
	mbChorus_Type_4			= 3,
	mbChorus_Type_FB		= 4,
	mbChorus_Type_Flanger	= 5
} mbChorusType;

// mbChorus_Rate is the parameter token that allows control over chorus
// modulation frequency.
// The mbParamValue matching this token should be an mbMilliUnit measuring the
// modulation frequency in Hz (where 1000 is 1.0 Hz).
#define mbChorus_Rate		mbT("Chorus Rate")	// Range 0 to 15500

// mbChorus_Depth is the parameter token that allows control over chorus depth.
// The mbParamValue matching this token should be an mbTime measuring the peak
// to peak swing of the modulation in milliseconds.
#define mbChorus_Depth		mbT("Chorus Depth")	// Range 300 to 40000

// mbChorus_Feedback is the parameter token that allows control over the
// percentage of chorus feedback.
// The mbParamValue matching this token should be an mbMilliUnit with a range
// of 0 to 1000, which scales between 0% and 100% feedback.
#define mbChorus_Feedback	mbT("Chorus Feedback")	// Range 0 to 1000 (1000 == 100%)



// Reverb Parameters
// ****************************************************************************
// ****************************************************************************

// mbReverb_Type is the parameter token that allows control over reverb type.
// The mbParamValue matching this token should be one of the following values
// given by the enum mbReverbType, below.
#define mbReverb_Type		mbT("Reverb Type")

// mbReverbType
// --------------------------------------------------
// mbReverbType is to be used as an mbParamValue to determine the GM2-defined
// reverb type.
//
typedef enum
{
	mbReverb_SmallRoom		= 0,
	mbReverb_MedRoom		= 1,
	mbReverb_LargeRoom		= 2,
	mbReverb_MedHall		= 3,
	mbReverb_LargeHall		= 4,
	mbReverb_Plate			= 5
} mbReverbType;

// mbReverb_Time is the parameter token that allows control over reverb decay.
// The mbParamValue matching this token should be an mbTime with a range of 360
// to 9000 (0.36 to 9.0 seconds) that measures the time in milliseconds for
// which the low frequency portion of the original sound declines by -60 dB.
#define mbReverb_Time		mbT("Reverb Time")	// Range 360 to 9000



// MetadataCtrl Types
// ****************************************************************************
// ****************************************************************************

// mbMetadataEncoding
// --------------------------------------------------
// Type of encoding for a metadata item.
//
typedef enum
{
	mbEncoding_ASCII				= 51,
	mbEncoding_Latin1				= 52,
	mbEncoding_ShiftJIS				= 53,
	mbEncoding_EUCKR				= 54,
	mbEncoding_HZGB2312				= 55,
	mbEncoding_Big5					= 56,
	mbEncoding_KOI8R				= 57,
	mbEncoding_TCVN5773				= 58,

	mbEncoding_UTF7					= 60,
	mbEncoding_UTF8					= 61,
	mbEncoding_UTF16				= 62,
	mbEncoding_UTF32				= 63,
	mbEncoding_CompressedUnicode	= 64,
	mbEncoding_UCS2					= 65,
	mbEncoding_UCS4					= 66,

	mbEncoding_Binary				= 67,
	mbEncoding_Undefined			= 80
} mbMetadataEncoding;

// Container for data returned from mbGetMetadataItemData.
// Always free by calling mbFreeBinaryData.
typedef const void * mbBinaryData;



// PlayerCtrl Types
// ****************************************************************************
// ****************************************************************************

// mbLoopCallback
// --------------------------------------------------
// Callback used to indicate that a loop end has been reached and request
// whether looping should continue.
// This callback is always synchronous.
//
typedef mbBool		(*mbLoopCallback) (
								void *context);

// mbTerminationCallback
// --------------------------------------------------
// Callback indicating that the specified player has stopped playing.
//
typedef void		(*mbTerminationCallback) (
								void *context);

// mbPositionCallback
// --------------------------------------------------
// Callback used to indicate that a given position in the data has been passed.
// 'requestedPos' is the position in milliseconds originally requested for the
// callback.
// 'actualPos' is the the position in milliseconds at which the callback is
// actually executed (the renderer processes groups of midi events and sample
// frames at the same time and executes this callback when the process is
// complete).
//
typedef void		(*mbPositionCallback) (
								void *context,
								mbTime requestedPos,
								mbTime actualPos);

// mbMarkerCallback
// --------------------------------------------------
// Callback used by Markers
//
typedef void		(*mbMarkerCallback) (
								void *context,
								mbInt32 markerID);



// MidiPlayerCtrl Types
// ****************************************************************************
// ****************************************************************************

// mbMetaEventType
// --------------------------------------------------
// Types of MIDI meta events that may be handled via a MetaEvent Callback.
//
typedef enum
{
	mbMeta_Sequence_Number		= 0x00,
	mbMeta_Text_Event			= 0x01,
	mbMeta_Copyright_Notice		= 0x02,
	mbMeta_Track_Name			= 0x03,
	mbMeta_Instrument_Name		= 0x04,
	mbMeta_Lyric				= 0x05,
	mbMeta_Marker				= 0x06,
	mbMeta_Cue_Point			= 0x07,
	mbMeta_Channel_Prefix		= 0x20,
	mbMeta_End_of_Track			= 0x2F,
	mbMeta_Set_Tempo			= 0x51,
	mbMeta_SMPTE_Offset			= 0x54,
	mbMeta_Time_Signature		= 0x58,
	mbMeta_Key_Signature		= 0x59,
	mbMeta_XMF_Patch_Type		= 0x60,
	mbMeta_Sequencer_Specific	= 0x7F
} mbMetaEventType;

// mbMidiPlayerPulseCallback
// --------------------------------------------------
// Callback that returns each time the MIDI player returns to parse data.
//
typedef void		(*mbMidiPlayerPulseCallback) (
								void *context,
								mbTime milliseconds,
								mbUInt32 midiClock);

// mbMetaEventCallback
// --------------------------------------------------
// Callback used to return real-time MIDI metadata events.
//
typedef void		(*mbMetaEventCallback) (
								void *context,
								mbMetaEventType eventType,
								mbUInt32 dataLen,
								const mbByte *data);



// MidiSynth Types
// ****************************************************************************
// ****************************************************************************

// The following NRPN messages may be sent to the synth.
enum
{
	mbNRPN_Channel_Voice_Limit	= 128 // Channel-specific voice limit.
};

// mbMidiControllerCallback
// --------------------------------------------------
// Callback used to return real-time controller events.
//
typedef void		(*mbMidiControllerCallback) (
								void *context,
								mbUInt8 midiChannel,
								mbUInt16 midiTrack,
								mbUInt8 controllerNumber,
								mbUInt8 controllerValue);

// mbSysexCallback
// --------------------------------------------------
// Callback used to return real-time sysex messages.
// 'dataLen' includes the final 0xF7, but not the beginning F0 or length VLQ
// 'data' points to the first data byte past the length VLQ
//
typedef void		(*mbSysexCallback) (
								void *context,
								mbUInt32 dataLen,
								const mbByte *data);



// AudioPlayerCtrl Types
// ****************************************************************************
// ****************************************************************************

// mbFramePositionCallback
// --------------------------------------------------
// Callback used to indicate that a given sample frame has been passed.
// 'requestedFrame' is the frame originally requested for the callback
// 'actualFrame' is the the frame on which the callback is actually executed
// (the renderer processes groups of frames at the same time an reports this
// callback when the process is complete).
//
typedef void		(*mbFramePositionCallback) (
								void *context,
								mbUInt32 requestedFrame,
								mbUInt32 actualFrame);

// mbFilter_Cutoff_Freq is the parameter token that allows control over filter
// cutoff frequency.
// The mbParamValue matching this token should be an mbMilliUnit with a range
// of 0 to 1000, which corresponds to percentages of the sample's Nyquist
// frequency (e.g., 0 to 1000 scales to 0 Hz to (sampling rate / 2) Hz).
// Note that while you may set this to any frequency, the effective frequency
// range is (sampling rate / 240) Hz to (sampling rate / 6) Hz.
#define mbFilter_Cutoff_Freq	mbT("Filter Cutoff Freq")

// mbFilter_Resonance is the parameter token that allows control over the
// amount of resonance at the cutoff/center frequency.
// The mbParamValue matching this token should be an mbMilliUnit with a range
// of 0 to 1000, which scales between 0 dB (no resonance) and +20 db (full
// resonance).
#define mbFilter_Resonance		mbT("Filter Resonance")



// AudioStreamPlayer Types
// ****************************************************************************
// ****************************************************************************

// mbStreamResult
// --------------------------------------------------
// Result code used to inform mobileBAE about the status of client-serviced
// audio streams.
//
typedef enum
{
	mbStream_No_Err					= 0,
	mbStream_EOF					= 100,
	mbStream_Underflow				= 101,
	mbStream_Seek_Not_Supported		= 102
} mbStreamResult;

// mbStreamCallback
// --------------------------------------------------
// Callback used to service Audio Stream Players
// The generic callback through which mobileBAE client-services streams
// communicate with the client.
// 'fillBuffer' specifies the memory address into which the client should
// write the requested data.
// 'bufferSizeInBytes' is an in/out parameter that specifies how many bytes
// mobileBAE requires (incoming) and how many bytes the client is actually
// able to supply (outgoing). Always set this value.
// 'bytePos' indicates the byte position in the stream mobileBAE requires to
// render further data. During seeks, this value will not match the end of the
// previous request.
// Appropriate result codes are:
// * mbStream_No_Err:
//   The callback was processed as requested.
// * mbStream_EOF:
//   The callback either reached the end of the stream or encountered a fatal
//   error that necessitates that streaming end.
// * mbStream_Underflow:
//   The callback is not capable of fulfilling the request, but has confidence
//   that it may be able to do so at some later time.
// * mbStream_Seek_Not_Supported:
//   The request to stream from a location other the end of the last request
//   is not supported. Set bufferSizeInBytes to 0 should this occur.
// To stop BAE from asking for more data during underflow conditions, return
// mbStream_EOF, and BAE will terminate the playback of the stream.
// Note: While this callback is synchronous, it will not be called from a
// hardware interrupt; as a result, it is safe to allocate/free memory, draw to
// the screen, etc. However, ensure that this callback returns promptly, since
// it will block other mobileBAE callbacks until it returns.
//
typedef mbStreamResult (*mbStreamCallback) (
								void *context,
								void *fillBuffer,
								mbInt32 *bufferSizeInBytes,
								mbInt32 bytePos);



// AudioFilePlayer Types
// ****************************************************************************
// ****************************************************************************

// mbUnderflowCallback
// --------------------------------------------------
// Callback used to indicate that a streamed audio file player is unable to
// fill its buffers quickly enough.
// This call will execute at most once per call to mbStart or mbPreroll.
// Because this usually means that the file system is unable to cope with
// requests for additional data in general (as opposed to a one-time problem
// such as network congestion), it is advisable to terminate the audio file
// player when the callback is executed.
// If the callback is initialized to be synchronous, remember to wait until a
// safe time to call mbStop.
//
typedef void		(*mbUnderflowCallback) (
								void *context);



// Deprecated / Obsolete Types
// ****************************************************************************
// These types are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// System is now a standard object; this type will be deprecated in future
// versions of mobileBAE
#define mbSystemID							mbObjectID

// mbInput_FileType is deprecated in favor of mbFileType...
#define mbInput_FileType					mbFileType
#define mbInputFile_Undefined				mbFileType_Undefined
#define mbInputFile_AIFF					mbFileType_AIFF
#define mbInputFile_AU						mbFileType_AU
#define mbInputFile_DLS						mbFileType_DLS
#define mbInputFile_MPEG					mbFileType_MPEG
#define mbInputFile_SMF						mbFileType_SMF
#define mbInputFile_WAVE					mbFileType_WAVE
#define mbInputFile_XMF						mbFileType_XMF
#define mbInputFile_IREZ					mbFileType_RMF
#define mbInputFile_SMS						mbFileType_SMS
#define mbInputFile_RTX						mbFileType_RTX
#define mbInputFile_IMY						mbFileType_iMelody
#define mbInputFile_SMAF					mbFileType_SMAF

// The following mbResult codes have been renamed, consolidated, or removed:
#define mbNot_Available						mbFeature_Not_Supported
#define mbFormat_Not_Supported				mbAudio_Format_Not_Supported
#define mbPlayer_Not_Loaded					mbNot_Loaded
#define mbPlayer_Not_Playing				mbNot_Playing
#define mbPlayer_Operation_Error			mbOperation_Not_Enabled
#define mbUnsupported_Audio_Codec			mbAudio_Codec_Not_Supported
#define mbUnsupported_File_Type				mbFile_Type_Not_Supported
#define mbInfo_Not_Found					mbNot_Found
#define mbInfo_Not_Supported				mbNot_Found

// The following mbResult codes are obsolete beginning with mobileBAE 1.4:
// mbHardware_Not_Available
// mbParameter_Not_Supported
// mbComponent_Not_Found

// mbAutoDiscStatus is obsolete beginning with mobileBAE 1.5
// The mbFileType mbFileType_DLS is obsolete beginning with mobileBAE 1.5.

#endif // _MBAE_TYPES_H_
