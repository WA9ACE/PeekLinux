/******************************************************************************
**
**	PAL.h
**
**	Header for the Platform Abstraction Layer functions.
**
**	(c) Copyright 2000-2004 Beatnik, Inc., All Rights Reserved.
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
**	Rights in Technical Data and Computer Software clause in DFACS
**	252.227-7013 or subparagraphs (c)(1) and (2) of the Commercial
**	Computer Software--Restricted Rights at 48 CFR 52.227-19, as
**	applicable.
**
**	Modification History:
**
**	2000.09.12 msd	Created file based on BAE_API.h
**	2000.12.19 AER	Revised for integration of Event Manager notifier.
**	2001.05.22 msd	moved API query/set into AC_HostAPI, rather than
**						being on ACI_AudioHardware.
**	2001.05.25 msd	Added IsEngaged() and GetOutputFrameCount() to ACI_AudioHardware
**	2001.05.30 msd	Added ACI_AudioHardware::GetDefaultFormat()
**	2001.07.27 AER	Deprecated ACHost_Not_Supported in favor of
**						ACHost_Operation_Not_Supported.
**	2001.08.21 SME	Changed signature (signed -> unsigned) of GetOutputFrameCount
**						so that very large values can be stored/returned.
**	2001.08.27 SME	Added new interface methods for EventNotifier thread-safety.
**	2001.08.31	DS	Added sample type field and enum for types.
**	2001.09.07 SME	Added new generic interface methods for thread-safety 
**						(removed the EventNotifier-specific ones).
**	2001.09.10 msd	Added ACI_AudioHardware::Mute()
**	2001.09.10 SME	Reworked the thread-safety host api.
**	2001.11.28 SME	Unicode support.
**	2002.01.03 SME	Better UTF8 support.
**	2002.03.18 SME	Removed unused functions: MemSize and FileSetLength.
**	2002.04.01 SME	Revised unicode compile options (AC_CHARSET_*).
**	2002.04.02 SME	Moved string trace output and assert implementation to Host API.
**	2002.04.16	DS	Added ACHost_Audio_Hardware_Err.
**	2002.05.03 SME	Host output hardware implementation now determines number
**					of frames in each output buffer.
**	2002.06.27 SME	Reworked thread locks and created new lock wrapper code.
**	2002.07.25 AER	Updated to handle single input/output device/API.
**					Removed unused functions.
**					Completely reformatted the file for readability and cleaned
**					up some documentation.
**	2002.07.26 SME	Removed unused functions.
**	2002.07.29 AER	Added many documentation comments and reorganized.
**	2002.07.30 AER	Renamed AC_HostAPI_Assert to AC_HostAPI_AssertFailure.
**					Fixed some comments.
**	2002.08.05 AER	Updated comments.
**	2002.08.29	DS	Added experimental limiter (DRC) API (ifdef'd)
**	2002.09.09 AER	Removed (unnecessary) use of extern.
**					Removed unused file functions.
**					Noted which functions must be interrupt-safe.
**	2002.09.17	DS	Moved DRC functions into ACC_Mixer.c since they are now private.
**	2002.09.26 AER	ACF_FillBuffer/ACF_CaptureBuffer no longer return results.
**					Revised power functions to be implemented per base.
**	2002.10.03 AER	Renamed AC_HostAPI* PAL*.
**	2002.10.09 AER	Removed dependency on ACConfig.h and core engine types.
**			   AER	Revised memory functions to match ANSI specification.
**	2002.10.10 AER	Removed string functions (they're now handled in the core).
**			   msd	Added PAL_ActivateBuzzer()
**	2002.10.15 AER	Added character type translation macros (PAL_T).
**	2002.10.16 AER	Fixed naming of two output hardware functions.
**	2002.11.06 AER	Renamed 'Buzzer' 'Vibrator' to match common industry usage.
**	2003.02.13 AER	Event Notification has changed to allow greater flexibility
**					in multi-user situations (and to remove the reliance of the
**					core system on globals).
**	2003.07.01 msd	Added PAL_ActivateLED()
**	2003.07.24 AER	Added a context pointer to the acquire functions for the
**					event notifier and input/output hardware; this allows an
**					implementor to establish communication between client code
**					and a PAL implementation.
**	2003.07.28 AER	Supplemented change of 2003.07.24 with a context pointer
**					for PAL_Setup.
**					Improved documentation of functions.
**	2003.07.30 AER	Supplemented change of 2003.07.28 with a context pointer
**					for PAL_Cleanup.
**	2003.08.04 AER	Fixed #define for PAL_AudioFormat_Init.
**	2003.11.26 SME	Removed PAL-WaitMicroseconds from PAL.
**	2003.12.01	DS	Added allocation level argument to PAL_Allocate().
**	2004.01.07	DS	Wrapped file io declarations via PAL_ENABLE_FILE_IO.
**					Wrapped lock declarations via PAL_USES_THREADS.
**	2004.01.08	DS	Added PAL_AudioOutputHardware_GetCaps() and its associated enums and
**					struct decl.
**	2004.01.09 SME	Added PAL_FileCreate and PAL_FileWrite.
**	2004.01.09	DS	Added PAL_AudioInputHardware_GetCaps().  Fixed enum names.
**	2004.01.12 msd	Replaced ActivateVibrator/LED with more general SetParameter
**	2004.01.19	DS	Wrapped event notification code in PAL_ENABLE_AUTO_NOTIFIER.
**	2004.01.22 AER	Added PAL_Authorization_Error (helpful for expressing DRM
**					failure in file IO).
**	2004.01.23 AER	Added PAL_GetImplementationVersion.
**	2004.02.11 SME	Added PAL_AuthorizeUserKey.
**	2004.02.12 msd	Introduced PAL_TerminalHandle
**	2004.02.19 AER	Fixed a declaration bug where PAL_EventQueued was not
**					within the #define for PAL_ENABLE_AUTO_NOTIFIER.
**					Tidied some comments.
**	2004.02.23 SME	Updated authorization API.
**	2004.02.24 SME	Added authorizationKeyLength param to PAL_AuthorizeKey.
**	2004.03.15 AER	Revised GetImplementationVersion per Platform Engineering
**					request.
**	2004.03.16 AER	Added #if blocks for AudioOutputHardware_Mute, FileCreate,
**					and FileWrite. Tidied file.
**	2004.09.13 SME	Decomponentized input hardware, output hardware and mixer.
**	2004.10.03 AER	Removed obsolete auto-disconnect function.
**					Removed obsolete math functions.
**	2004.10.07 AER	Replaced use of PAL_USES_THREADS and PAL_USE_AUTO_NOTIFIER
**					with PAL_MULTI_THREADED.
**					ACF_ProcessEvents is now blocked by PAL_MULTI_THREADED.
**	2004.11.11 AER	Removed unused function PAL_TraceStrU, the PAL_TraceStr
**					macro, and PALCharU, and the PAL_T macro.
**					[Bug 2297] Changed definition of mbChar in UTF32 builds.
**
******************************************************************************/

#ifndef _PAL_H_
#define _PAL_H_

#include "PAL_PlatformOptions.h"



// Types
// ****************************************************************************
// Type definitions used by numerous function groups.
// ****************************************************************************

// Result Codes
typedef enum
{
	PAL_No_Err = 0,
	PAL_Unknown_Err,
	PAL_Operation_Not_Supported,
	PAL_File_Not_Found,
	PAL_File_In_Use,
	PAL_Parameter_Err,
	PAL_Audio_Hardware_Err,
	PAL_Memory_Err,
	PAL_Authorization_Error,
        PAL_Format_Not_Supported
} PALResult;

// Numbers
typedef unsigned char	PALByte;		// Used for byte arrays
typedef unsigned int	PALsize_t;		// Used from ANSI-like functions

// Characters
typedef char			PALCharA;		// 8-bit narrow ASCII character

#if defined(PAL_CHARSET_ASCII)
	typedef PALCharA		PALChar;
#elif defined(PAL_CHARSET_UTF8)
	typedef PALCharA		PALChar;
#elif defined(PAL_CHARSET_UTF16)
	typedef unsigned short	PALChar;
#elif defined(PAL_CHARSET_UTF32)
	typedef long			PALChar;
#endif // PAL_CHARSET_*

// Macro used to create an int value from four literal ASCII characters.
// For example:	token = PAL_FourChar('T','A','G','_');
#define PAL_FourChar(ch1,ch2,ch3,ch4)	((unsigned int)(PALByte)(ch4) | \
									((unsigned int)(PALByte)(ch3) << 8) | \
									((unsigned int)(PALByte)(ch2) << 16) | \
									((unsigned int)(PALByte)(ch1) << 24))

// Audio Output Hardware token
typedef struct _AC_OutputHardware *		PAL_EngineOutputToken;

#if defined(PAL_ENABLE_AUDIO_CAPTURE)

// Audio Input Hardware token
typedef struct _AC_InputHardware *		PAL_EngineInputToken;

#endif // PAL_ENABLE_AUDIO_CAPTURE

// Event Notifier token
typedef struct _AC_EventMgr *			PAL_EventNotifierToken;



// Hardware Capabilities Data
// ****************************************************************************
// Used in the function PAL_AudioOutputHardware_GetCaps.
// ****************************************************************************

// Possible values for supported HW sample formats returned as HW capabilities.
enum
{
	PAL_AudioHardwareCaps_Unsigned8Bit	= 0x1,
	PAL_AudioHardwareCaps_Signed8Bit	= 0x2,
	PAL_AudioHardwareCaps_Unsigned16Bit	= 0x4,
	PAL_AudioHardwareCaps_Signed16Bit	= 0x8,
	PAL_AudioHardwareCaps_Float			= 0x10
};

// Possible values for supported HW channel counts returned as HW capabilities.
enum
{
	PAL_AudioHardwareCaps_Mono			= 0x1,
	PAL_AudioHardwareCaps_Stereo		= 0x2,
	PAL_AudioHardwareCaps_4Channel		= 0x4,
	PAL_AudioHardwareCaps_8Channel		= 0x8
};

// Possible values for supported HW sampling rates returned as HW capabilities.
// Note that other rates besides these may be supported, but an application
// will have to perform a specific query (using the function
// PAL_AudioOutputHardware_IsFormatSupported) to determine if a particular rate
// is supported.
enum
{
	PAL_AudioHardwareCaps_SR_8000		= 0x1,
	PAL_AudioHardwareCaps_SR_11025		= 0x2,
	PAL_AudioHardwareCaps_SR_16000		= 0x4,
	PAL_AudioHardwareCaps_SR_22050		= 0x8,
	PAL_AudioHardwareCaps_SR_24000		= 0x10,
	PAL_AudioHardwareCaps_SR_32000		= 0x20,
	PAL_AudioHardwareCaps_SR_40000		= 0x40,
	PAL_AudioHardwareCaps_SR_44100		= 0x80,
	PAL_AudioHardwareCaps_SR_48000		= 0x100
};

// Other boolean capabilities (for output HW only).  These are set in 'otherCaps', below.
enum
{
	PAL_OutputHardwareCaps_Volume		= 0x1,	// HW supports volume setting
	PAL_OutputHardwareCaps_Balance		= 0x2,	// HW supports balance setting
	PAL_OutputHardwareCaps_Latency		= 0x4	// HW supports setting of latency
};

typedef struct _PAL_AudioHardwareCaps
{
	unsigned long			audioFormats;		// bitmask of supported formats
	unsigned long			audioChannelCounts;	// bitmask of supported channel counts
	unsigned long			audioSampleRates;	// bitmask of supported sampling rates
	unsigned long			minLatency;			// in msec.
	unsigned long			maxLatency;
	unsigned long			defaultLatency;
	unsigned long			otherCaps;			// bitmask of misc caps (volume, balance, etc.)
} PAL_AudioHardwareCaps;



// Engine Callbacks
// ****************************************************************************
// Prototypes of functions to call in mobileBAE.
// ****************************************************************************

// ACF_FillBuffer
// ---------------------------------------------------
// Request that mobileBAE render the next buffer of audio.
// Note that if any problems occur in the rendering of the audio, mobileBAE
// will fill the buffer with silence.
//
void				ACF_FillBuffer(
								PAL_EngineOutputToken token,
								void *buffer,
								int frameCount);

#if defined(PAL_ENABLE_AUDIO_CAPTURE)

// ACF_CaptureBuffer
// ---------------------------------------------------
// Supply mobileBAE with the next incoming audio buffer.
// This function should be implemented only if the symbol
// PAL_ENABLE_AUDIO_CAPTURE is defined.
//
void				ACF_CaptureBuffer(
								PAL_EngineInputToken token,
								void *buffer);

#endif // PAL_ENABLE_AUDIO_CAPTURE

#if defined(PAL_MULTI_THREADED)

// ACF_ProcessEvents
// ---------------------------------------------------
// Inform mobileBAE that it is safe to process callbacks and events.
// This function should be implemented only if the symbol PAL_MULTI_THREADED is
// defined.
//
void				ACF_ProcessEvents(
								PAL_EventNotifierToken token);

#endif // defined(PAL_MULTI_THREADED)



// PAL Setup Functions
// ****************************************************************************
// ****************************************************************************

// PAL_Setup
// ---------------------------------------------------
// Perform any one-time initialization or setup required at the start of any
// mobileBAE instance (PAL_Setup is called immediately after the Client API
// function mbCreateEngageSystem is called).
// The 'context' parameter is intended to allow the caller of the Client API
// function mbCreateEngageSystem to pass a pointer to the PAL implementation to
// facilitate direct communication between client code and PAL implementation.
// 'context' is an optional parameter, and may be ignored in implementations
// that do not require such communication.
// Note that the 'context' pointer here is the same as is used in the acquire
// functions for audio input and output hardware, event notifier, and terminal
// handle.
//
PALResult			PAL_Setup(
								void *context);

// PAL_Cleanup
// ---------------------------------------------------
// Clean up any resources allocated by PAL_Setup when mobileBAE has shut down
// (PAL_Cleanup is called immediately after the Client API function mbDestroy
// is called).
// The 'context' parameter is the same one passed to PAL_Setup.
//
PALResult			PAL_Cleanup(
								void *context);



// PAL Platform Functions
// ****************************************************************************
// ****************************************************************************

// PAL_GetPlatformName
// ---------------------------------------------------
// What is the name/description of the system platform?
// 'buffer' is the character buffer into which to write, and 'bufferLen' is the
// length of the buffer in characters.
//
PALResult			PAL_GetPlatformName(
								PALChar *buffer,
								long bufferLen);

// PAL_GetCPUName
// ---------------------------------------------------
// What is the name/description of the system CPU?
// 'buffer' is the character buffer into which to write, and 'bufferLen' is the
// length of the buffer in characters.
//
PALResult			PAL_GetCPUName(
								PALChar *buffer,
								long bufferLen);

// PAL_GetImplementationVersion
// --------------------------------------------------
// What is the version of the PAL implementation?
// 'buffer' is the character buffer into which to write, and 'bufferLen' is the
// length of the buffer in characters.
//
PALResult			PAL_GetImplementationVersion(
								PALChar *buffer,
								long bufferLen);



// PAL Authorization Functions
// ****************************************************************************
// ****************************************************************************

typedef enum
{
	PAL_AccessToken_Resource	= PAL_FourChar('R','E','S','X')
} PAL_AccessToken;

// PAL_AuthorizeKey
// --------------------------------------------------
// Is the provided key valid for the provided access token? 
// This function is used to verify whether a user is allowed elevated 
// privileges to items of 'accessToWhat' class.
// 'authorizationKey' is implementation dependent; it is passed thru directly 
// from the client API mbAuthorizeResourceAccess function and therefore its 
// use must be coordinated with client API developers.
// Return PAL_No_Err if access is granted or PAL_Authorization_Error if it is 
// not granted.
//
PALResult			PAL_AuthorizeKey(
								PAL_AccessToken accessToWhat,
								const void *authorizationKey,
								unsigned int authorizationKeyLength);



// PAL Memory Functions
// ****************************************************************************
// ****************************************************************************

typedef enum
{
	PAL_Allocate_Level1	= 1,
	PAL_Allocate_Level2	= 2,
	PAL_Allocate_Level3	= 3,
	PAL_Allocate_Level4	= 4,
	PAL_Allocate_Level5	= 5
} PAL_AllocationLevel;

// PAL_Allocate
// ---------------------------------------------------
// Allocate a number of bytes equal to 'size', using allocation level hint
// 'level'.
// PAL_Allocate_Level5 represents the most time-critical level, Level1 the
// least critical.
// This function is otherwise equivalent to the ANSI C function malloc.
//
void *				PAL_Allocate(
								PALsize_t size,
								PAL_AllocationLevel level);

// PAL_Deallocate
// ---------------------------------------------------
// Deallocate the provided memory block.
// This function is equivalent to the ANSI C function free.
//
void				PAL_Deallocate(
								void *memoryBlock);

// PAL_MemSet
// ---------------------------------------------------
// Set each byte of the provided memory block to the provided ASCII character.
// This function is equivalent to the ANSI C function memset.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void *				PAL_MemSet(
								void *dest,
								int val,
								PALsize_t len);

// PAL_MemCmp
// ---------------------------------------------------
// Perform a comparison between the provided memory blocks.
// Returns < 0 if b1 < b2; 0 if b1 == b2; > 0 if b2 > b1.
// This function is equivalent to the ANSI C function memcmp.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
signed long			PAL_MemCmp(
								const void *b1,
								const void *b2,
								PALsize_t len);

// PAL_MemCpy
// ---------------------------------------------------
// Copy 'len' bytes from 'src' into 'dest'.
// This function is equivalent to the ANSI C function memcpy.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void *				PAL_MemCpy(
								void *dest,
								const void *src,
								PALsize_t len);



// PAL Time Functions
// ****************************************************************************
// ****************************************************************************

// PAL_Microseconds
// ---------------------------------------------------
// Return the number of microseconds that have transpired since a fixed point
// in time at or before the system was initialized by PAL_Setup.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
unsigned long		PAL_Microseconds(void);



// PAL Threading Functions
// ****************************************************************************
// These functions only need to be implemented on multithreaded systems.
// PALLockHandle is an opaque pointer that hides a platform specific
// implementation of a recursive threadlock. It should wrap a native semaphore,
// mutex, critical section, or similar locking mechanism.
// If a thread has already acquired a lock, the lock should support recursive 
// calls from the same thread without blocking that thread.
// mobileBAE will ensure that calls to PAL_AcquireLock and PAL_ReleaseLock are
// balanced even though some of those calls may have been recursive.
// ****************************************************************************

typedef void * PALLockHandle;

#if defined(PAL_MULTI_THREADED)

// PAL_CreateLock
// ---------------------------------------------------
// Create/initialize a thread lock object and return an opaque handle to it.
// The lock object must allow recursive acquisitions (see PAL_AcquireLock).
// On non-threaded systems, return NULL.
//
PALLockHandle		PAL_CreateLock(void);

// PAL_AcquireLock
// ---------------------------------------------------
// Acquire ownership of the specified thread lock object, or wait until
// ownership of that thread lock object becomes available.
// Return only when the ownership is established or if the calling thread 
// already has established ownership.
// Lock objects should support recursive calls from the same thread.
// If the calling thread already owns the object, it should not be blocked
// if it calls PAL_AcquireLock for the same lock object again.
// mobileBAE will ensure that calls to PAL_AcquireLock and PAL_ReleaseLock are
// balanced.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void				PAL_AcquireLock(
								PALLockHandle lock);

// PAL_ReleaseLock
// ---------------------------------------------------
// Release ownership of the specified thread lock object.
// If a thread has acquired ownership of a thread lock object multiple 
// consecutive times, then it will release the object the same number of times.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void				PAL_ReleaseLock(
								PALLockHandle lock);

// PAL_DestroyLock
// ---------------------------------------------------
// Release the resources used by a lock allocated by PAL_CreateLock.
// The lock may not be owned when this function is called.
//
void				PAL_DestroyLock(
								PALLockHandle lock);

#endif // PAL_MULTI_THREADED



// PAL Terminal Functions
// ****************************************************************************
// ****************************************************************************

typedef enum
{
	PAL_Param_Class_Custom		= 0x0001,
	PAL_Param_Class_Vibrator	= 0x0002,
	PAL_Param_Class_LED			= 0x0004,
	PAL_Param_Class_Display		= 0x0010,
	PAL_Param_Class_Keyboard	= 0x0020,
	PAL_Param_Class_Misc		= 0xffd0, // everything else
	PAL_Param_Class_All			= 0xffff
} PAL_Param_Class;

typedef enum
{
	PAL_Param_ID_All			= 0x7f
} PAL_Param_ID;

typedef enum
{
	PAL_Param_Command_Reset		= 1,
	PAL_Param_Command_On		= 2,
	PAL_Param_Command_Off		= 3,
	PAL_Param_Command_ColorRGB	= 4
} PAL_Param_Command;

// Ranges for R, G, B values are 0x00 to 0x7f.
#define PAL_RGB(r,g,b)		((((r)&0x7f)<<16)|(((g)&0x7f)<<8)|((b)&0x7f))
#define PAL_GetRValue(rgb)	(((rgb)>>16)&0x7f)
#define PAL_GetGValue(rgb)	(((rgb)>> 8)&0x7f)
#define PAL_GetBValue(rgb)	(((rgb)    )&0x7f)


// Command							Data
// ----------------------------------------------------------------------------
// PAL_Param_Command_Custom			void *
// PAL_Param_Command_Reset			none (value = 0)
// PAL_Param_Command_On				none (value = 0)
// PAL_Param_Command_Off			none (value = 0)
// PAL_Param_Command_ColorRGB		RGB (use PAL_Get[R/G/B]Value() macros)


typedef void * PAL_TerminalHandle;

// PAL_AcquireTerminalHandle
// ---------------------------------------------------
// Acquire resources for accessing Terminal devices.
// The 'context' parameter is intended to allow the caller of the Client API
// function mbCreateEngageSystem to pass a pointer to the PAL implementation to
// facilitate direct communication between client code and PAL implementation.
// 'context' is an optional parameter, and may be ignored in implementations
// that do not require such communication.
// Note that the 'context' pointer here is the same as is used in the acquire
// functions for audio input and output hardware and the event notifier.
//
PAL_TerminalHandle	PAL_AcquireTerminalHandle(
								void *context);

// PAL_ReleaseTerminalHandle
// ---------------------------------------------------
// Release resources for the terminal handle.
//
PALResult			PAL_ReleaseTerminalHandle(
								PAL_TerminalHandle handle);

// PAL_Terminal_SetParameter
// ---------------------------------------------------
// Sets parameters on terminal devices such as Vibrators and LEDs.
// 'deviceClassFlags' is a bit field of target devices with ID 'deviceID'
// See enums above for valid devices, commands, and values.
//
PALResult			PAL_Terminal_SetParameter(
								PAL_TerminalHandle handle,
								unsigned short deviceClassFlags,
								long deviceID,
								PAL_Param_Command command,
								long value);

// PAL_Terminal_SetParameterPtr
// ---------------------------------------------------
// Sets custom parameters on terminal devices such as Vibrators and LEDs.
// 'deviceClassFlags' is a bit field of target devices with ID 'deviceID'
//
PALResult			PAL_Terminal_SetParameterPtr(
								PAL_TerminalHandle handle,
								unsigned short deviceClassFlags,
								long deviceID,
								const void *data,
								long dataLen);



// PAL Event Notification Functions
// ****************************************************************************
// ****************************************************************************

typedef void * PAL_EventNotifierHandle;

#if defined(PAL_MULTI_THREADED)

// PAL_AcquireDefaultEventNotifier
// ---------------------------------------------------
// Acquire resources for the system event notifier.
// The 'context' parameter is intended to allow the caller of the Client API
// function mbCreateEngageSystem to pass a pointer to the PAL implementation to
// facilitate direct communication between client code and PAL implementation.
// 'context' is an optional parameter, and may be ignored in implementations
// that do not require such communication.
// Note that the 'context' pointer here is the same as is used in the acquire
// functions for audio input and output hardware and terminal handle.
//
PAL_EventNotifierHandle PAL_AcquireDefaultEventNotifier(
								void *context);

// PAL_ReleaseEventNotifier
// ---------------------------------------------------
// Release resources for the system event notifier.
//
PALResult			PAL_ReleaseEventNotifier(
								PAL_EventNotifierHandle handle);

// PAL_EventNotifier_Engage
// ---------------------------------------------------
// Start the event notifier that periodically calls ACF_ProcessEvents.
//
PALResult			PAL_EventNotifier_Engage(
								PAL_EventNotifierHandle handle,
								PAL_EventNotifierToken token);

// PAL_EventNotifier_Disengage
// ---------------------------------------------------
// Stop the event notifier begun by PAL_EventNotifier_Engage.
//
PALResult			PAL_EventNotifier_Disengage(
								PAL_EventNotifierHandle handle);

// PAL_EventQueued
// ---------------------------------------------------
// A new event has been queued by mobileBAE. Call ACF_ProcessEvents as soon as
// possible.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void				PAL_EventQueued(
								PAL_EventNotifierHandle handle);

#endif // PAL_MULTI_THREADED



// PAL File IO Functions
// ****************************************************************************
// These functions only need to be implemented on systems which support
// disk-based file I/O.
// ****************************************************************************

#if BAE_USE_RFS
        typedef unsigned int PAL_NativeFilePtr;
#else
        typedef void *	PAL_NativeFilePtr;
#endif

typedef enum
{
	PAL_File_OpenRead	= 0x01,
	PAL_File_OpenWrite	= 0x02
} PAL_FileAttributes;

#if defined(PAL_ENABLE_FILE_IO)

// PAL_FileOpen
// ---------------------------------------------------
// Open the file corresponding to the provided URI path and name with the
// provided attributes.
//
PAL_NativeFilePtr	PAL_FileOpen(
								const PALChar *URI,
								PAL_FileAttributes attr,
								PALResult *outResult);

// PAL_FileClose
// ---------------------------------------------------
// Close the provided file.
//
PALResult			PAL_FileClose(
								PAL_NativeFilePtr file);

// PAL_FileRead
// ---------------------------------------------------
// Read a number of bytes equal to 'bufferLen' from the read head of the
// provided file into 'buffer'.
//
unsigned long		PAL_FileRead(
								PAL_NativeFilePtr file,
								PALByte *buffer,
								unsigned long bufferLen,
								PALResult *outResult);

// PAL_FileSeek
// ---------------------------------------------------
// Set the read head of the provided file to the provided byte position
// relative to the beginning of the file.
//
PALResult			PAL_FileSeek(
								PAL_NativeFilePtr file,
								long fileOffset);

// PAL_FileGetPosition
// ---------------------------------------------------
// Return the byte position of the read head of the provided file.
//
long				PAL_FileGetPosition(
								PAL_NativeFilePtr file,
								PALResult *outResult);

// PAL_FileGetLength
// ---------------------------------------------------
// Return the byte length of the provided file.
//
long				PAL_FileGetLength(
								PAL_NativeFilePtr file,
								PALResult *outResult);

#if defined(PAL_ENABLE_AUDIO_CAPTURE) // && defined(PAL_ENABLE_FILE_IO)

// PAL_FileCreate
// ---------------------------------------------------
// Create a file with a path and name corresponding to the provided URI.
// This function is used exclusively in conjuntion with writing files captured
// from the audio input hardware.
//
PALResult			PAL_FileCreate(
								const PALChar *URI);

// PAL_FileWrite
// ---------------------------------------------------
// Write a number of bytes equal to 'bufferLen' from 'buffer' to the 
// current position of the provided file.
// This function is used exclusively in conjuntion with writing files captured
// from the audio input hardware.
//
unsigned long		PAL_FileWrite(
								PAL_NativeFilePtr file,
								PALByte *buffer,
								unsigned long bufferLen,
								PALResult *outResult);

#endif // PAL_ENABLE_AUDIO_CAPTURE

#endif // PAL_ENABLE_FILE_IO



// PAL Trace Functions
// ****************************************************************************
// Trace functions should be implemented only if the symbol PAL_ENABLE_TRACE is
// defined, and in accordance with the character set symbol definitions.
// ****************************************************************************

#if defined(PAL_ENABLE_TRACE)

// PAL_TraceStrA
// ---------------------------------------------------
// Send the given string to the system output. This function can be implemented
// as simply as passing the string to printf, or use a more complicated and
// system-dependent method.
// PAL_TraceStrA is always defined, even in Unicode builds.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void				PAL_TraceStrA(
								const PALCharA *str);

#else

#define				PAL_TraceStrA(x)

#endif // PAL_ENABLE_TRACE



// PAL Assert Function
// ****************************************************************************
// This function should be implemented only if the symbol PAL_DEBUG is defined.
// ****************************************************************************

#if defined(PAL_DEBUG)

// PAL_AssertFailure
// ---------------------------------------------------
// Halt or terminate execution of the program, providing the given information.
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
void				PAL_AssertFailure(
								const PALCharA *fileName,
								int lineNo,
								const PALCharA *expression);
#endif // PAL_DEBUG



// PAL Output Hardware Functions
// ****************************************************************************
// The Acquire and Release methods work with an PAL_AudioHardwareHandle handle,
// which is nothing more that a void pointer defined and maintained by the
// implementor of the PAL. Each PAL_AudioHardware function takes this handle as
// a parameter.
// PAL_AudioHardware is the interface to the sound card / output device.
// ****************************************************************************

typedef void * PAL_AudioHardwareHandle;

typedef enum
{
	PAL_Sample_Type_Undefined = 0,
	PAL_Sample_Type_Unsigned = 1,
	PAL_Sample_Type_Signed = 2,
	PAL_Sample_Type_Float = 3
} PAL_OutputSampleType;

typedef struct _PAL_AudioFormat
{
	PAL_OutputSampleType	sampleType;			// Unsigned, Signed, Float
	unsigned long			bitDepth;			// bits per sample
	unsigned long			channels;			// samples per frame
	unsigned long			samplingRate;		// in Hz, frames per second
} PAL_AudioFormat;

#define PAL_AudioFormat_Init(This, OutputSampleType, theBitDepth, channelCnt, rate) \
		(This)->sampleType = OutputSampleType;	\
		(This)->bitDepth = theBitDepth;			\
		(This)->channels = channelCnt;			\
		(This)->samplingRate = rate;

// PAL_AcquireDefaultAudioOutputHardware
// ---------------------------------------------------
// Acquire resources for the system audio output hardware device.
// The 'context' parameter is intended to allow the caller of the Client API
// function mbCreateEngageSystem to pass a pointer to the PAL implementation to
// facilitate direct communication between client code and PAL implementation.
// 'context' is an optional parameter, and may be ignored in implementations
// that do not require such communication.
// Note that the 'context' pointer here is the same as is used in the acquire
// functions for audio input hardware, event notifier, and terminal handle.
//
PAL_AudioHardwareHandle PAL_AcquireDefaultAudioOutputHardware(
								void *context);

// PAL_ReleaseAudioOutputHardware
// ---------------------------------------------------
// Release resources for the system audio output hardware device.
//
PALResult			PAL_ReleaseAudioOutputHardware(
								PAL_AudioHardwareHandle handle);

// PAL_AudioOutputHardware_GetCaps
// ---------------------------------------------------
// Retrieve a set of hardware capabilities for the system audio output hardware
// device, including supported audio formats, channel counts, sampling rates,
// and other parameters.
// All bit values for the PAL_AudioHardwareCaps fields are reserved.

PALResult			PAL_AudioOutputHardware_GetCaps(
								PAL_AudioHardwareHandle handle,
								PAL_AudioHardwareCaps *pCaps);

// PAL_AudioOutputHardware_IsFormatSupported
// ---------------------------------------------------
// Is the provided audio format supported by the system audio output hardware
// device?
//
int					PAL_AudioOutputHardware_IsFormatSupported(
								PAL_AudioHardwareHandle handle,
								const PAL_AudioFormat *format);

// PAL_AudioOutputHardware_GetDefaultFormat		
// ---------------------------------------------------
// What is the default audio format used by the system audio output hardware
// device?
//
PALResult			PAL_AudioOutputHardware_GetDefaultFormat(
								PAL_AudioHardwareHandle handle,
								PAL_AudioFormat *outFormat);

// PAL_AudioOutputHardware_Engage		
// ---------------------------------------------------
// Engage the system audio output hardware device so that it begins requesting
// data from mobileBAE.
//
PALResult			PAL_AudioOutputHardware_Engage(
								PAL_AudioHardwareHandle handle,
								const PAL_AudioFormat *format,
								PAL_EngineOutputToken token);

// PAL_AudioOutputHardware_Disengage		
// ---------------------------------------------------
// Disengage the system audio output hardware device.
//
PALResult			PAL_AudioOutputHardware_Disengage(
								PAL_AudioHardwareHandle handle);

// PAL_AudioOutputHardware_IsEngaged					
// ---------------------------------------------------
// Is the system audio output hardware device engaged?
//
int					PAL_AudioOutputHardware_IsEngaged(
								PAL_AudioHardwareHandle handle);

// PAL_AudioOutputHardware_GetOutputFrameCount
// ---------------------------------------------------
// How many frames have been served since the system audio output hardware
// device was most recently engaged?
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
unsigned long		PAL_AudioOutputHardware_GetOutputFrameCount(
								PAL_AudioHardwareHandle handle);

// PAL_AudioOutputHardware_SetVolume
// ---------------------------------------------------
// Set the volume of the system audio output hardware device.
// Range is 0 to 65535.
//
PALResult			PAL_AudioOutputHardware_SetVolume(
								PAL_AudioHardwareHandle handle,
								unsigned short volume);

// PAL_AudioOutputHardware_GetVolume
// ---------------------------------------------------
// Get the volume of the system audio output hardware device.
// Range is 0 to 65535.
//
unsigned short		PAL_AudioOutputHardware_GetVolume(
								PAL_AudioHardwareHandle handle,
								PALResult *outResult);

// PAL_AudioOutputHardware_SetBalance
// ---------------------------------------------------
// Set the stereo position of the system audio output hardware device.
// Range is -32767 to 32767, with 0 at center.
//
PALResult			PAL_AudioOutputHardware_SetBalance(
								PAL_AudioHardwareHandle handle,
								short balance);

// PAL_AudioOutputHardware_GetBalance
// ---------------------------------------------------
// Get the stereo position of the system audio output hardware device.
// Range is -32767 to 32767, with 0 at center.
//
short				PAL_AudioOutputHardware_GetBalance(
								PAL_AudioHardwareHandle handle,
								PALResult *outResult);

// PAL_AudioOutputHardware_SetLatency
// ---------------------------------------------------
// Set the latency of the system audio output hardware device in milliseconds.
// If it is not possible to set the latency of the device to the requested
// amount, round up to the nearest possible time.
// Call PAL_AudioOutputHardware_GetLatency to verify the actual resultant
// latency.
//
PALResult			PAL_AudioOutputHardware_SetLatency(
								PAL_AudioHardwareHandle handle,
								unsigned long msec);

// PAL_AudioOutputHardware_GetLatency
// ---------------------------------------------------
// Returns the latency of the system audio output hardware device in
// milliseconds.
//
unsigned long		PAL_AudioOutputHardware_GetLatency(
								PAL_AudioHardwareHandle handle,
								PALResult *outResult);



// PAL Input Hardware Functions
// ****************************************************************************
// The Acquire and Release methods work with an PAL_AudioHardwareHandle handle,
// which is nothing more that a void pointer defined and maintained by the
// implementor of the PAL. Each PAL_AudioOutputHardware function takes this
// handle as a parameter.
// PAL_AudioInputHardware is the interface to the input device.
// Input Hardware functions should be implemented only if the symbol
// PAL_ENABLE_AUDIO_CAPTURE is defined.
// ****************************************************************************

#if defined(PAL_ENABLE_AUDIO_CAPTURE)

// PAL_AcquireDefaultAudioInputHardware
// ---------------------------------------------------
// Acquire resources for the system audio input hardware device.
// The 'context' parameter is intended to allow the caller of the Client API
// function mbCreateEngageSystem to pass a pointer to the PAL implementation to
// facilitate direct communication between client code and PAL implementation.
// 'context' is an optional parameter, and may be ignored in implementations
// that do not require such communication.
// Note that the 'context' pointer here is the same as is used in the acquire
// functions for audio output hardware, event notifier, and terminal handle.
//
PAL_AudioHardwareHandle PAL_AcquireDefaultAudioInputHardware(
								void *context);

// PAL_ReleaseAudioInputHardware
// ---------------------------------------------------
// Release resources for the system audio input hardware device.
//
PALResult			PAL_ReleaseAudioInputHardware(
								PAL_AudioHardwareHandle handle);

// PAL_AudioInputHardware_GetCaps
// ---------------------------------------------------
// Retrieve a set of hardware capabilities for the system audio output hardware
// device, including supported audio formats, channel counts, sampling rates,
// and other parameters.
// All bit values for the PAL_AudioOutputHardwareCaps fields are reserved.

PALResult			PAL_AudioInputHardware_GetCaps(
								PAL_AudioHardwareHandle handle,
								PAL_AudioHardwareCaps *pCaps);

// PAL_AudioInputHardware_IsFormatSupported
// ---------------------------------------------------
// Is the provided audio format supported by the system audio input hardware
// device?
//
int					PAL_AudioInputHardware_IsFormatSupported(
								PAL_AudioHardwareHandle handle,
								const PAL_AudioFormat *format);

// PAL_AudioInputHardware_GetDefaultFormat
// ---------------------------------------------------
// What is the default audio format used by the system audio input hardware
// device?
//
PALResult			PAL_AudioInputHardware_GetDefaultFormat(
								PAL_AudioHardwareHandle handle,
								PAL_AudioFormat *outFormat,
								long *outFrames);

// PAL_AudioInputHardware_Engage
// ---------------------------------------------------
// Engage the system audio input hardware device so that it begins requesting
// data from mobileBAE.
//
PALResult			PAL_AudioInputHardware_Engage(
								PAL_AudioHardwareHandle handle,
								const PAL_AudioFormat *format,
								unsigned long numFrames,
								PAL_EngineInputToken token);

// PAL_AudioInputHardware_Disengage
// ---------------------------------------------------
// Disengage the system audio input hardware device.
//
PALResult			PAL_AudioInputHardware_Disengage(
								PAL_AudioHardwareHandle handle);

// PAL_AudioInputHardware_IsEngaged
// ---------------------------------------------------
// Is the system audio input hardware device engaged?
//
int					PAL_AudioInputHardware_IsEngaged(
								PAL_AudioHardwareHandle handle);

// PAL_AudioInputHardware_GetInputFrameCount
// ---------------------------------------------------
// How many frames have been served since the system audio input hardware
// device was most recently engaged?
// Note that this function may be called from the render thread/interrupt, and
// as a result should not allocate or free memory, or do anything else that may
// cause problems on your platform.
//
unsigned long		PAL_AudioInputHardware_GetInputFrameCount(
								PAL_AudioHardwareHandle handle);

#endif // PAL_ENABLE_AUDIO_CAPTURE

#endif // _PAL_H_
