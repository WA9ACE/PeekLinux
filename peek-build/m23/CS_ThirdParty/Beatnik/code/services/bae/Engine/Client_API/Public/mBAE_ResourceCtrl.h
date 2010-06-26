/******************************************************************************
**
**	mBAE_ResourceCtrl.h
**
**	The control through which all objects that support extraction of resources do so.
**
**	(c) Copyright 2004 Beatnik, Inc., All Rights Reserved.
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
**	2004.01.30 SME	Created file.
**	2004.02.11 SME	Add support for encrypted resource access protection.
**	2004.02.13 AER	Changed definition for MB_FOUR_CHAR to mbFourChar.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.02.23 SME	Renamed mbAuthorizeUser to mbAuthorizeResourceAccess.
**	2004.02.24 SME	Added authorizationKeyLength param to mbAuthorizeResourceAccess.
**	2004.03.10 SME	Fixed comment typo.
**
******************************************************************************/

#ifndef _MBAE_RESOURCECTRL_H_
#define _MBAE_RESOURCECTRL_H_

#include "mBAE_Types.h"


// mbGetResourceCount
// --------------------------------------------------
// Returns the number of 'resourceType' resources available for extraction by
// this control.
// 'resourceType' should be initialized via the mbFourChar macro using 4 ASCII
// character parameters, e.g. mbFourChar('J','P','E','G').
//
mbUInt16			mbGetResourceCount(
								mbObjectID objectID,
								mbTag resourceType,
								mbResult *outResult);


// mbGetResourcePtr
// ----------------------------------------------------------------------------
// Returns const pointer to resource in memory if available.
// The pointer returned by the call should not be freed.
// The pointer should not be used once the content from which it was extracted
// has been unloaded.
// If the resource is encrypted an error will be returned.
// 'resourceType' should be initialized via the mbFourChar macro using 4 ASCII
// character parameters, e.g. mbFourChar('J','P','E','G').
//
const void *		mbGetResourcePtr(
								mbObjectID objectID,
								mbTag resourceType,
								mbUInt16 index,
								mbUInt32 *outDataLen,
								mbResult *outResult);


// mbCopyResource
// --------------------------------------------------
// Returns a copy of the resource in an allocated memory block as an
// mbBinaryData (raw binary block).
// The returned block must be freed via mbFreeBinaryData.
// 'resourceType' should be initialized via the mbFourChar macro using 4 ASCII
// character parameters, e.g. mbFourChar('J','P','E','G').
// 'passphrase' indicates the passphrase used when encrypting the file, and may
// be passed as NULL. If mbCopyResource returns mbAuthorization_Error, then the
// resource is encrypted and a passphrase will be required to decrypt it.
//
mbBinaryData		mbCopyResource(
								mbObjectID objectID,
								mbTag resourceType,
								mbUInt16 index,
								const mbChar *passphrase,
								mbUInt32 *outDataLen,
								mbResult *outResult);


// mbAuthorizeResourceAccess
// --------------------------------------------------
// Requests elevated privileges for attempts to extract encrypted resources.
// Grant of access facilitates extraction of Beatnik Type 2 encrypted resources
// without the use of a passphrase.
// Authorization is granted for access to a particular piece of content,
// therefore mbAuthorizeResourceAccess must be called when new content has been 
// loaded into a player object.
//
mbResult			mbAuthorizeResourceAccess(
								mbObjectID objectID,
								const void *authorizationKey,
								mbUInt32 authorizationKeyLength);


#endif // _MBAE_RESOURCECTRL_H_
