/**
 * @file	bae_options.h
 *
 * Private option defined for the BAE instance.
 *
 * @author	Richard Powell (richard@beatnik.com)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/17/2004	Richard Powell (richard@beatnik.com)		Create.
 *	12/8/2004	Richard Powell (richard@beatnik.com)		Updated for Riviera21.
 	02/5/2004	Padmanabha.V(padmanab@sasken.com)		OMAPS00128773: Defined PAL_ENABLE_STEREO_INPUT.
 *
 * (C) Copyright 2004 by Beatnik, Inc., All Rights Reserved
 */

#ifndef __BAE_OPTIONS_PVT_H_
#define __BAE_OPTIONS_PVT_H_

//-------- IMPORTANT ---------//
// This file is read in by PAL_PlatformOptions.h, and must be kept in sync with it.
//  PAL options defined by all configurations:
/*
  PAL_ENABLE_CALLBACKS
  PAL_MAX_ALLOC  128 * 1048576
  PAL_ENABLE_ARM_OPTIMIZATIONS
  PAL_ENABLE_SYNTH_CONTROL
  PAL_USE_INT_BITFIELD
  PAL_CHARSET_ASCII
  PAL_LITTLE_ENDIAN
  PAL_STRUCT_PACKING_RULE 2
  PAL_INLINE  static inline  ?  this needs to be confirmed on the TI compiler
  PAL_REGISTER register  ?  this needs to be confirmed on the TI compiler
  PAL_MULTI_THREADED
  PAL_REVERB_MAX_MEMORY 30000
  PAL_SYNTH_MOBILE_DLS_BASE
  PAL_ENABLE_AUTODISCONNECT
*/
//  PAL options NOT defined by any configuration:
/*
  PAL_GLOBALS_SUPPORTED
  PAL_ENABLE_FLOAT
  PAL_USE_TRANSITION_VOICES
  PAL_ENABLE_STEREO_INPUT
  PAL_ENABLE_AUDIO_CAPTURE
  PAL_ENABLE_CHORUS
  PAL_ENABLE_REVERB
  PAL_ENABLE_MPEG_DECODER_SUN
  PAL_ENABLE_MPEG_DECODER_EMUZED
  PAL_ENABLE_MARKERS
  PAL_ENABLE_MPEG_FILE
  PAL_SYNTH_DLS_LITE
  PAL_SYNTH_MOBILE_DLS_PLUS_OPTIONS
  PAL_SYNTH_DLS2
  PAL_DEBUG
  PAL_ENABLE_TRACE
  PAL_ENABLE_ANSI_DEBUG
  PAL_ENABLE_MEMORY_TRACE
*/
// All other options are configurations configured by this file.
// this file is now the gatekeeper for all the options.  If you want to
// change a particular build configuration, do it in here.  There are 4
// supported configurations:
// BAE_LOCOSTO
// BAE_ULTRA_LOCOSTO
// BAE_CALYPSO_PLUS_FULL
// BAE_CALYPSO_PLUS_MIN
//
// one of these symbols must be defined.  If not, the build will fail.
#if defined(BAE_LOCOSTO) && !defined(BAE_ULTRA_LOCOSTO) && !defined(BAE_CALYPSO_PLUS_FULL) && !defined(BAE_CALYPSO_PLUS_MIN) && !defined(BAE_CALYPSO_PLUS_CUSTOM)
#elif !defined(BAE_LOCOSTO) && defined(BAE_ULTRA_LOCOSTO) && !defined(BAE_CALYPSO_PLUS_FULL) && !defined(BAE_CALYPSO_PLUS_MIN) && !defined(BAE_CALYPSO_PLUS_CUSTOM)
#elif !defined(BAE_LOCOSTO) && !defined(BAE_ULTRA_LOCOSTO) && defined(BAE_CALYPSO_PLUS_FULL) && !defined(BAE_CALYPSO_PLUS_MIN) && !defined(BAE_CALYPSO_PLUS_CUSTOM)
#elif !defined(BAE_LOCOSTO) && !defined(BAE_ULTRA_LOCOSTO) && !defined(BAE_CALYPSO_PLUS_FULL) && defined(BAE_CALYPSO_PLUS_MIN) && !defined(BAE_CALYPSO_PLUS_CUSTOM)
#elif !defined(BAE_LOCOSTO) && !defined(BAE_ULTRA_LOCOSTO) && !defined(BAE_CALYPSO_PLUS_FULL) && !defined(BAE_CALYPSO_PLUS_MIN) && defined(BAE_CALYPSO_PLUS_CUSTOM)
#else
	#error "Must define only one configuration.  Pick one of BAE_LOCOSTO, BAE_ULTRA_LOCOSTO, BAE_CALYPSO_PLUS_FULL, BAE_CALYPSO_PLUS_MIN, BAE_CALYPSO_PLUS_CUSTOM"
#endif


// Common build options //
// versions of the bae swe, and the engine
#define BAE_SWE_VERSION "2.1.1"
#define MOBILE_BAE_VERSION "1.5.0p6"


//BAE_BANK_IS_SERIALIZED --> default bank is serialized and define BAE_BANK_ADDRESS and BAE_BANK_LENGTH macros with bank address and 
#define BAE_BANK_IS_SERIALIZED
#define BAE_BANK_ADDRESS (Mobile_0100_base)
#define BAE_BANK_LENGTH (kStreamSizeofMobile_0100_base)

// if T_BAE_DEFAULT_BANK_FILE is defined, it will use the default bank and bank location, EVEN IF BAE_BANK_IS_SERIALZED IS USED!!!
//#define T_BAE_DEFAULT_BANK_FILE "/Btk_DLS/Mobile_0100_base.dls"
//#define BAE_DEFAULT_BANK_IN_FAST_MEM
//#define T_BAE_ADDITIONAL_BANK_FILE "/Btk_DLS/TI_inst.dls"
//#define BAE_ADDITIONAL_BANK_IN_FAST_MEM

// THESE ARE DEFAULT OPTIONS WITH ALL BUILDS
#define BAE_USE_DRC
#define	BAE_DRC_THRESHOLD				-18000
#define	BAE_DRC_RATIO					2000
#define	BAE_DRC_ATTACK_TIME				200
#define	BAE_DRC_RELEASE_TIME			1000

#define RIVIERA_DEFAULT_LATENCY			20

#define RIVIERA_DEFAULT_BITDEPTH		16
#define RIVIERA_DEFAULT_SAMPLETYPE		PAL_Sample_Type_Signed

#define RIVIERA_DEFAULT_SAMPLINGRATE	22050
//#define RIVIERA_DEFAULT_FRAME_NUMBER	4404

// This is changed for smaller buffer size



// These are all the debugging options
//#define FAKE_FILL_BUFFER
//#define FAKE_AUDIO_SWE
//#define BAE_DO_FFS_METRICS
#define BAE_RUNTIME_METRICS
//#define BAE_WRITE_OUTPUT_FILE

//#define BAE_BANK_IN_FAST_MEM
//#define BAE_BANK_IN_FILE



// DEFINE WHAT IS IN THE MIN BUILD HERE
//--------  LOCOSTO BUILD -----------//

#ifdef BAE_LOCOSTO

#define PAL_ENABLE_STEREO_OUTPUT
#define RIVIERA_DEFAULT_CHANNELS		 2
#define PAL_ENABLE_XMF
#define PAL_ENABLE_IMELODY
#define PAL_ENABLE_MIDI_NORMALIZER
#define PAL_ENABLE_SMAF_MA3
#define PAL_ENABLE_SMS
#define PAL_ENABLE_DIGITAL_AUDIO
#define PAL_ENABLE_STEREO_INPUT //OMAPS00128773

// voice limit for the system
#define BAE_DEFAULT_STARTUP_MIDI_VOICES  32
#define BAE_DEFAULT_NUMBER_OF_MIDI_VOICES 32
#define PAL_ENABLE_FILE_IO
#define RIVIERA_DEFAULT_FRAME_NUMBER	2640
//#define BAE_AUDIO_PATH_TO_SPEAKER  //Set Audio Path to speaker

#endif // BAE_LOCOSTO

//--------  ULTRA LOCOSTO BUILD -----------//
#ifdef BAE_ULTRA_LOCOSTO

#define PAL_ENABLE_STEREO_OUTPUT
#define RIVIERA_DEFAULT_CHANNELS		1
#define PAL_ENABLE_XMF
#define PAL_ENABLE_IMELODY
#define PAL_ENABLE_MIDI_NORMALIZER
#define PAL_ENABLE_SMAF_MA3

// voice limit for the system
#define BAE_DEFAULT_STARTUP_MIDI_VOICES 16
#define BAE_DEFAULT_NUMBER_OF_MIDI_VOICES 16
#define PAL_ENABLE_FILE_IO
#define RIVIERA_DEFAULT_FRAME_NUMBER	968 // 22ms buffer stereo at 22050 sampling rate; 1320

#endif // BAE_ULTRA_LOCOSTO


//--------  CALYPSO+ FULL BUILD -----------//
#ifdef BAE_CALYPSO_PLUS_FULL

#define PAL_ENABLE_STEREO_OUTPUT
#define RIVIERA_DEFAULT_CHANNELS		2
#define PAL_ENABLE_DIGITAL_AUDIO
#define PAL_ENABLE_IMA
#define PAL_ENABLE_FILE_IO
#define PAL_ENABLE_XMF
#define PAL_ENABLE_SMS
#define PAL_ENABLE_RTX
#define PAL_ENABLE_IMELODY
#define PAL_ENABLE_MIDI_NORMALIZER
#define PAL_ENABLE_SMAF_MA3
#define PAL_ENABLE_XMF_DECOMPRESSION
#define PAL_ENABLE_XMF_DECRYPTION
#define PAL_ENABLE_AIFF
#define PAL_ENABLE_AU
#define PAL_ENABLE_METADATA
#define PAL_ENABLE_JSR_135
#define PAL_ENABLE_RMF

// voice limit for the system
#define BAE_DEFAULT_STARTUP_MIDI_VOICES 40
#define BAE_DEFAULT_NUMBER_OF_MIDI_VOICES 24

#endif // BAE_CALYPSO_PLUS_FULL


//--------  CALYPSO+ CUSTOM BUILD -----------//
#ifdef BAE_CALYPSO_PLUS_CUSTOM

#define PAL_ENABLE_STEREO_OUTPUT
#define RIVIERA_DEFAULT_CHANNELS		1
#define PAL_ENABLE_DIGITAL_AUDIO
#define PAL_ENABLE_IMA
#define PAL_ENABLE_FILE_IO
#define PAL_ENABLE_XMF
//#define PAL_ENABLE_SMS
//#define PAL_ENABLE_RTX
#define PAL_ENABLE_IMELODY
#define PAL_ENABLE_MIDI_NORMALIZER
#define PAL_ENABLE_SMAF_MA3
#define PAL_ENABLE_XMF_DECOMPRESSION
#define PAL_ENABLE_XMF_DECRYPTION
//#define PAL_ENABLE_AIFF
//#define PAL_ENABLE_AU
#define PAL_ENABLE_METADATA
//#define PAL_ENABLE_JSR_135
#define PAL_ENABLE_RMF

// voice limit for the system
#define BAE_DEFAULT_STARTUP_MIDI_VOICES 40
#define BAE_DEFAULT_NUMBER_OF_MIDI_VOICES 24

#endif // BAE_CALYPSO_PLUS_CUSTOM


//--------  CALYPSO+ MIN BUILD -----------//
#ifdef BAE_CALYPSO_PLUS_MIN

#define RIVIERA_DEFAULT_CHANNELS		1

// voice limit for the system
#define BAE_DEFAULT_STARTUP_MIDI_VOICES 40
#define BAE_DEFAULT_NUMBER_OF_MIDI_VOICES 24

#endif // BAE_CALYPSO_PLUS_MIN



// the digital audio option must come before this.
#ifdef PAL_ENABLE_DIGITAL_AUDIO
	#define BAE_DEFAULT_NUMBER_OF_AUDIO_VOICES 1
#else
	#define BAE_DEFAULT_NUMBER_OF_AUDIO_VOICES 0
#endif

// do a check on the default number of voices verses startup voices
#if (BAE_DEFAULT_NUMBER_OF_MIDI_VOICES > BAE_DEFAULT_STARTUP_MIDI_VOICES)
	#error "DEFAULT NUMBER OF VOICES MUST BE LESS THAN STARTUP NUMBER OF VOICES"
#endif


#endif /* __BAE_OPTIONS_PVT_H_ */

