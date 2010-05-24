/******************************************************************************
**
**	PAL_PlatformOptions.h	-- for ARM Processors
**
**	(c) Copyright 1996-2002 Beatnik, Inc., All Rights Reserved.
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
**	Confidential-- Internal use only
**
**	Modification History:
**	2002.05.20 SME	Created file.
**	2002.06.27 AER	AC_ENABLE_STEREO renamed AC_ENABLE_STEREO_OUTPUT.
**	2002.06.28 AER	Renamed AC_ENABLE_LINEAR_AUDIO AC_ENABLE_DIGITAL_AUDIO.
**			   SME	Added AC_ENABLE_STEREO_INPUT to full config.
**			   SME	Added AC_ENABLE_FLOAT symbol.
**	2002.07.15 AER	Added AC_REGISTER symbol.
**	2002.10.04 AER	Revised for new PAL abstraction (renamed AC_ PAL_).
**	2002.10.09 AER	Definition of PAL_ENABLE_TRACE in debug mode is now the
**					responsibility of PAL_PlatformOptions.h, not ACConfig.h.
**	2002.10.10	DS	Added PAL_REVERB_MAX_MEMORY and PAL_USE_TRANSITION_VOICES.
**	2002.10.24 AER	Cleaned up, reorganized, and improved documentation.
**	2004.01.17 RMP	Documented options for TI Riviera.
**	2004.11.04 RAVI	Ported to mobileBAE1.5
**
******************************************************************************/

#ifndef _PAL_BUILDOPTIONS_H_
#define _PAL_BUILDOPTIONS_H_

// read in the options defined for the BAE.
#include "bae/bae_options.h"

// mobileBAE Required Symbols
// ****************************************************************************
// Some form of the following symbols must be defined
// ****************************************************************************
// PAL_*_THREADED
// Required symbol. Define either PAL_SINGLE_THREADED or PAL_MULTI_THREADED.
// mobileBAE supports single- and multi-threaded operation, which defines
// whether it can run from multiple threads, how it handles internal events and
// whether or not it protects itself from multi-threaded access.
// PAL_SINGLE_THREADED requires manual event notification from the Client API
// (via the function mbSystemServiceIdle), but places less burden on the
// platform implementer.
// PAL_MULTI_THREADED requires the platform implementer to support locks and an
// automatic notification system, but relieves the Client API programmer of
// this responsibility and ensures that the core engine and the Client API are
// thread safe.
#define PAL_MULTI_THREADED

// PAL_*_ENDIAN
// Required symbol. Define either PAL_LITTLE_ENDIAN or PAL_BIG_ENDIAN,
// depending upon the CPU configuration.
// TI's platform is little endian
#define PAL_LITTLE_ENDIAN

// PAL_STRUCT_PACKING_RULE
// Required symbol. Set to either 1 or 2.  
// Rule 1: structs have no padding; short and int struct members are allowed on 
//         any byte boundary
// Rule 2: structs may be padded; short struct members must be on even byte 
//         boundaries and int struct members must be on 4 byte boundaries
#define PAL_STRUCT_PACKING_RULE 2

// PAL_USE_*_BITFIELD
// Required symbol.  Define either PAL_USE_INT_BITFIELD or PAL_USE_CHAR_BITFIELD,
// depending upon compiler support for non-integer bitfields.
#define PAL_USE_INT_BITFIELD

// PAL_CHARSET_*
// Required symbol. Define one of the following ENABLE_CHARSET symbols:
// PAL_CHARSET_ASCII, PAL_CHARSET_UTF16, PAL_CHARSET_UTF32, PAL_CHARSET_UTF8
// TI is using the ASCII based character set

/* x0056422 - OMAPS00156759 Dtd Dec 30, 2007 */

#if (BAE_UNICODE == 0)
  #define PAL_CHARSET_ASCII
#elif (BAE_UNICODE == 1)
  #define PAL_CHARSET_UTF16
#else
  #error "BAE_UNICODE not defined"
#endif

#if !defined (PAL_CHARSET_ASCII) && !defined(PAL_CHARSET_UTF8) && !defined (PAL_CHARSET_UTF16) && !defined (PAL_CHARSET_UTF32)
	#define PAL_CHARSET_ASCII
#endif



// mobileBAE Optional Symbols
// ****************************************************************************
// The following symbols are optional, and define the way certain code executes
// in mobileBAE.
// ****************************************************************************

// PAL_ENABLE_PLATFORM_DEFAULTS
// Optional symbol. Enables the application of platform default values and the
// Client API function mbCreateDefaultSystem per PAL_PlatformDefaults.h. If
// this symbol is not defined, the values in that file will be ignored and
// mbCreateDefaultSystem will not be enabled.
#define PAL_ENABLE_PLATFORM_DEFAULTS

// PAL_GLOBALS_SUPPORTED
// Optional symbol. Allows use of global variables within mobileBAE (required
// for tracing).
//#define PAL_GLOBALS_SUPPORTED

// PAL_ENABLE_FLOAT
// Optional symbol. Allows use of floating point code within mobileBAE.
// TI does not have a floating point unit
//#define PAL_ENABLE_FLOAT

// PAL_INLINE
// Optional symbol. Will be defined as static if not defined here.
// TI has different compilers for board and for emulator.  We need to have
// a different option for both
#ifdef TI_BOARD
  #ifdef BAE_ADS_BUILD
    #define PAL_INLINE		__inline
  #else
    #define PAL_INLINE		static inline
  #endif
#else
#define PAL_INLINE		__inline
#endif

// PAL_REGISTER
// Optional symbol. This is used to override the ANSI standard keyword register
// with a more efficient or powerful platform-specific usage.
// TI compilers seem to recognize this
#define PAL_REGISTER	register

// PAL_ENABLE_TRACE
// Optional symbol. Allows mobileBAE to trace runtime information to the
// console or other target determined by the implementer of the PAL.
// TI's platforms cannot do trace statements without serious performance loss
//#define PAL_ENABLE_TRACE

// PAL_DEBUG
// Optional symbol. Allows mobileBAE to run with additional debug checks and
// safety protocols in place.
// TI's platforms cannot do debug without serious performance loss
//#define PAL_DEBUG

// PAL_ENABLE_MEMORY_TRACE
// Optional symbol. Allows mobileBAE to analytically trace memory allocations
// and releases.
// TI's platforms cannot do memory trace without serious performance loss
//#define PAL_ENABLE_MEMORY_TRACE

// PAL_REVERB_MAX_MEMORY
// Optional symbol. Meaningful only if the symbol PAL_ENABLE_REVERB is defined.
// Sets the maximum amount of dynamic memory in bytes that the reverb system is
// allowed to use.
// TI: leave as is because PAL_ENABLE_REVERB will not be defined
#define PAL_REVERB_MAX_MEMORY 30000

// PAL_USE_TRANSITION_VOICES
// Optional symbol. Informs the MIDI renderer to allocate an extra voice for
// each full 8 MIDI voices requested. These "transition" voices are used during
// voice stealing to avoid clipping the ends of stolen voices, providing
// smoother voice stealing at the cost of an extra voice per every 8 requested.
//#define PAL_USE_TRANSITION_VOICES // Don't use these as they hurt bandwidth...

// PAL_ENABLE_ARM_OPTIMIZATIONS
// Optional symbol. mobileBAE has optimized assembly code for ARM platforms.
// Defining this symbol will enable them.
// Should be defined in the Makefile or project/build file if needed.
//#ifdef __arm
#define PAL_ENABLE_ARM_OPTIMIZATIONS
//#endif

// PAL_MAX_ALLOC
// Optional symbol. mobileBAE defaults to allowing memory allocations of any
// size. By defining PAL_MAX_ALLOC, it is possible to restrict the maximum
// individual memory allocation size in bytes.
#define PAL_MAX_ALLOC		128 * 1048576			// 128 megabytes

// PAL_ENABLE_SMAF_FALLBACK
// Optional symbol. Meaningful only if the symbol PAL_ENABLE_SMAF_MA1, 2, or 3
// is defined. Controls behavior if missing or unsupported instruments are
// encountered in SMAF files. If defined, mobileBAE will instead play the GM
// instrument with the corresponding program number. If not defined, mobileBAE
// will play silence.
#define PAL_ENABLE_SMAF_FALLBACK

// PAL_ENABLE_FILE_IO
// Optional symbol.  Enables use of disk-based file I/O.
// TI: Don't use FILE IO
//#define PAL_ENABLE_FILE_IO

// PAL_ENABLE_MIDI_NORMALIZER
// Optional symbol. Enables the use of code that 'normalized' MIDI notes to
// improve and balance overall gain. This has no effect of runtime performance
// but will slow load times.
//#ifdef RIVIERA_ENABLE_MIDI_NORMALIZER
//	#define PAL_ENABLE_MIDI_NORMALIZER
//#endif



// mobileBAE Component Symbols
// ****************************************************************************
// The following symbols are optional, and define whether or not components are
// compiled into mobileBAE. The PAL documentation for an explanation of what
// each symbol does.
// Here, we set up a pair of example top-level symbols which provide a standard
// set of compile options.
// ****************************************************************************

#define PAL_ENABLE_SYNTH_CONTROL

// Secondary System Features
#define PAL_ENABLE_CALLBACKS
//#define PAL_ENABLE_MARKERS


// General System Capabilities
// TI supports stereo input and output
//#ifdef RIVIERA_USE_STEREO
//	// don't redefine symbols
//	#ifndef PAL_ENABLE_STEREO_OUTPUT
//		#define PAL_ENABLE_STEREO_OUTPUT
//	#endif // PAL_ENABLE_STEREO_OUTPUT
//#endif // RIVIERA_USE_STEREO

// Primary System Features
// TI does not have an audio capture system
//#define PAL_ENABLE_AUDIO_CAPTURE
// TI does support auto-disconnect
#define PAL_ENABLE_AUTODISCONNECT
// TI supports digital audio
//#ifdef RIVIERA_USE_DIGITAL_AUDIO
//	#define PAL_ENABLE_DIGITAL_AUDIO
//#endif


// Effects, etc.
// TI platform does not have chorus, reverb

//#define PAL_ENABLE_CHORUS
//#define PAL_ENABLE_REVERB

// File Types
//#ifdef RIVIERA_USE_DIGITAL_AUDIO
//	#define PAL_ENABLE_AIFF
//#endif
//#define PAL_ENABLE_IMELODY
//#define PAL_ENABLE_RTX
//#define PAL_ENABLE_SMS
//#undef PAL_ENABLE_MPEG_FILE

//#ifdef RIVIERA_USE_RMF
//#define PAL_ENABLE_RMF
//#endif

//#ifdef RIVIERA_USE_SMAF
//#define PAL_ENABLE_SMAF_MA3
//#endif

//#ifdef RIVIERA_USE_XMF
//#define PAL_ENABLE_XMF
////#define PAL_ENABLE_XMF_DECOMPRESSION
////#define PAL_ENABLE_XMF_DECRYPTION
//#endif

// Audio Decoders
//#define PAL_ENABLE_G711
//#define PAL_ENABLE_IMA
// TI does not have mp3
//#undef PAL_ENABLE_MPEG_DECODER


// PAL_SYNTH_*
// Required symbol. This defines the operation of the mobileBAE synthesizer.
// One of the following symbols should be defined:
// PAL_SYNTH_MOBILE_DLS_BASE defines compliance with the basic Mobile DLS
// specification without optional features.
// PAL_SYNTH_MOBILE_DLS_PLUS_OPTIONS defines compliance with the Mobile DLS
// specification with optional features (such as filter, Vib LFO, etc.).
#define PAL_SYNTH_MOBILE_DLS_BASE

#ifdef USE_FAST_CODE
#define _MB_SLOW_CODE_START_    !defined(FAST_CODE)
#define _MB_FAST_CODE_START_   	defined(FAST_CODE)
#define _MB_BOTH_CODE_START_    1
#define _MB_NEITHER_CODE_START_    0
#else
#define _MB_SLOW_CODE_START_    1
#define _MB_FAST_CODE_START_   	1
#define _MB_BOTH_CODE_START_    1
#define _MB_NEITHER_CODE_START_    0
#endif


#endif	// _PAL_BUILDOPTIONS_H_
