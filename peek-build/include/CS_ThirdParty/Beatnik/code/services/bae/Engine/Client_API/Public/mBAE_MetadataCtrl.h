/******************************************************************************
**
**	mBAE_MetadataCtrl.h
**
**	The control through which all objects that support metadata queries do so.
**
**	(c) Copyright 2003-2004 Beatnik, Inc., All Rights Reserved.
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
**	2003.12.05 SME	Created file.
**	2004.01.06 SME	Added obsolete symbol section for backwards compatibility.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.02.19 AER	Removed 'ByIndex' from several functions names.
**					Tidied the file a bit.
**
******************************************************************************/

#ifndef _MBAE_METADATACTRL_H_
#define _MBAE_METADATACTRL_H_

#include "mBAE_Types.h"

// Cross-file generic metadata types which may be queried via mbQueryMetadata.
// These are presented as alternatives to looking up the equivalent metadata
// types in the mobileBAE user guide for each file format that one might query.
//
#define mbName_Title		mbT("mBAE_MetadataName_Title")
#define mbName_Comment		mbT("mBAE_MetadataName_Comment")
#define mbName_Copyright	mbT("mBAE_MetadataName_Copyright")
#define mbName_Genre		mbT("mBAE_MetadataName_Genre")
#define mbName_Source		mbT("mBAE_MetadataName_Source")
#define mbName_Date			mbT("mBAE_MetadataName_Date")
#define mbName_Composer		mbT("mBAE_MetadataName_Composer")
#define mbName_Artist		mbT("mBAE_MetadataName_Artist")


// mbQueryMetadata
// --------------------------------------------------
// Returns information about the content of a file associated with an mbObject.
// Pass an mbString of the information type desired, as documented in the
// mobileBAE user guide.
// Alternatively, pass an mbName_* macro of the information type desired.
// The mbName_* macros will be mapped to the file specific information type
// that applies to the mbObject depending upon the content it has loaded.
// Note that not all mbName_* macros have mappings in each file format.
// The memory for the returned mbString must be freed via mbFreeString.
//

mbString			mbQueryMetadata(
								mbObjectID objectID,
								const mbChar *infoType,
								mbResult *outResult);


// mbGetMetadataItemCount
// --------------------------------------------------
// Returns the number of metadata items found in the file currently loaded
// by the mbObject.
//
mbInt32				mbGetMetadataItemCount(
								mbObjectID objectID,
								mbResult *outResult);


// mbGetMetadataItemName
// --------------------------------------------------
// Returns the name of the metadata item at 'index'.
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbGetMetadataItemName(
								mbObjectID objectID,
								mbInt32 index,
								mbResult *outResult);


// mbGetMetadataItemLanguage
// --------------------------------------------------
// Returns the language of the metadata item at 'index'. The format of the
// string is defined in the Internet Engineering Task Force's HTTP standard
// (RFC 2068 at section 3.10), as derived from RFC 1766, ISO 639 (language
// codes) and ISO 3166 (country codes).
// Returns NULL (and mbNo_Err) if language is not specified (as is the case for
// most file formats).
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbGetMetadataItemLanguage(
								mbObjectID objectID,
								mbInt32 index,
								mbResult *outResult);


// mbGetMetadataItemSize
// --------------------------------------------------
// Returns the size in bytes of the metadata item at 'index'.
//
mbUInt32			mbGetMetadataItemSize(
								mbObjectID objectID,
								mbInt32 index,
								mbResult *outResult);


// mbGetMetadataItemEncoding
// --------------------------------------------------
// Returns the mbMetadataEncoding of the metadata item at 'index'.
//
mbMetadataEncoding	mbGetMetadataItemEncoding(
								mbObjectID objectID,
								mbInt32 index,
								mbResult *outResult);


// mbGetMetadataItemString
// --------------------------------------------------
// Returns the contents of the metadata item at 'index', if possible, as an
// mbString (using possibly lossy string conversion).
// The memory for the returned mbString must be freed via mbFreeString.
//
mbString			mbGetMetadataItemString(
								mbObjectID objectID,
								mbInt32 index,
								mbResult *outResult);


// mbGetMetadataItemData
// --------------------------------------------------
// Returns the contents of the metadata item at 'index' as an  mbBinaryData
// (raw binary block).
// If the metadata contents are a string, it is not converted to the type that
// defines mbChar.
// The memory for the returned mbBinaryData must be freed via mbFreeBinaryData.
//
mbBinaryData		mbGetMetadataItemData(
								mbObjectID objectID,
								mbInt32 index,
								mbUInt32 *outDataLen,
								mbResult *outResult);



// Deprecated / Obsolete Functions
// ****************************************************************************
// These functions are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// mbQueryFileInfo
// --------------------------------------------------
// mbQueryFileInfo has been replaced by mbQueryMetadata.
//
#define mbQueryFileInfo						mbQueryMetadata


#endif // _MBAE_METADATACTRL_H_
