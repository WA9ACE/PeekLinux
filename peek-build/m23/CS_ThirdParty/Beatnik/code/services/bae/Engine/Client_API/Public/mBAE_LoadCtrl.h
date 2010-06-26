/******************************************************************************
**
**	mBAE_LoadCtrl.h
**
**	The control through which all objects that load data do so.
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
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.07.15 AER	Modified the mbLoadFromMemory description to reflect that
**					if the client requests that no data be copied, the engine
**					will return mbCopy_Required if the data will not work
**					without a copy.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2002.10.22	DS	Added mbQueryFileInfo().
**	2003.07.17 AER	[Bug 1115] Revised description of mbLoadFromMemory.
**	2003.12.19 SME	First phase of mBAE 1.4 metadata overhaul.
**	2004.02.17	DS	Added mbLoadFromFileOffset.
**	2004.02.18 AER	Updated mbLoadFromMemory to accept a const pointer.
**	2004.03.04 AER	Improved documentation of mbLoad functions.
**
******************************************************************************/

#ifndef _MBAE_LOADCTRL_H_
#define _MBAE_LOADCTRL_H_

#include "mBAE_Types.h"

// mbLoadFromFile
// --------------------------------------------------
// Loads a file of the file type 'fileType' as specified by the file path
// 'filePath'.
// Memory allocation and/or data decompression, will occur during this call.
// This function may return the following results:
// mbFile_Not_Found
//    Unable to locate the file from the given path.
// mbFile_In_Use
//    The file is in use by another process and cannot be accessed.
// mbAuthorization_Error
//    Access to the file was restricted due to insufficient permissions.
// mbFile_Type_Not_Supported
//    Unable to handle the file because mobileBAE does not support the file
//    type at all, because mobileBAE is not currently configured to support the
//    file type, or because the file type was incorrectly identified.
// mbBad_File
//    The internal structure of the file is corrupt and cannot be parsed, or
//    data within the file is corrupt.
// mbFile_Feature_Not_Supported
//    The file contains features not supported by mobileBAE because mobileBAE
//    is not configured to handle the feature or because mobileBAE does not
//    handle the feature at all. Examples: SMF type 2, DLS 2 articulations in
//    a Mobile DLS base configuration, unknown XMF reference types, etc.
// mbAudio_Codec_Not_Supported
//    The file contains data requiring an unknown audio codec.
// mbDecompressor_Not_Supported
//    The file contains data requiring an unknown file decompressor.
// mbDecryptor_Not_Supported
//    The file contains data requiring an unknown decryptor.
// mbUnpacker_Not_Supported
//    The file contains data requiring an unknown unpacker.
//
mbResult			mbLoadFromFile(
								mbObjectID objectID,
								mbFilePath filePath,
								mbFileType fileType);

// mbLoadFromFileOffset
// --------------------------------------------------
// Functions identically to mbLoadFromFile, but the file may be loaded
// beginning at byte offset 'fileOffset', for length in bytes 'fileLength'.
// This function returns the same results as mbLoadFromFile.
//
mbResult			mbLoadFromFileOffset(
								mbObjectID theObject,
								mbFilePath theFilePath,
								mbUInt32 fileOffset,
								mbUInt32 fileLength,
								mbFileType fileType);

// mbLoadFromMemory
// --------------------------------------------------
// Loads a file of the file type 'fileType' of length in bytes 'size' from the
// memory address specified by 'memoryPointer'.
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
// This function returns the same results as mbLoadFromFile, except that it
// will not return mbFile_Not_Found, mbFile_In_Use, or mbAuthorization_Error.
//
mbResult			mbLoadFromMemory(
								mbObjectID objectID,
								const void *memoryPointer,
								mbFileType fileType,
								mbUInt32 size,
								mbBool copyData);

// mbUnload
// --------------------------------------------------
// Unload the file.
//
mbResult			mbUnload(
								mbObjectID objectID);

#endif // _MBAE_LOADCTRL_H_
