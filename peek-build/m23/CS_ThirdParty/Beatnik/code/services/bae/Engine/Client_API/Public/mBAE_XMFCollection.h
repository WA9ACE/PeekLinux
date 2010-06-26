/******************************************************************************
**
**	mBAE_XMFCollection.h
**
**	XMFCollection represents a collection-style file.
**	The primary purpose of XMF Collection is to encapsulate the querying of
**	information and acquisition of songs and sounds from the file.
**
**	XMFCollections may use the controls found in the following files:
**		mBAE_LoadCtrl.h
**		mBAE_MetadataCtrl.h
**		mBAE_ResourceCtrl.h
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
**	2002.03.27 SME	Removed player specific Destroy APIs.
**	2002.03.28 msd	Eliminated XMFFilePlayer
**	2002.04.22 SME	Removed Sound APIs from XMFCollection since there are no
**					standard or registered ResourceFormatIDs for digital audio.
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.06.28 AER	Renamed AC_USE_LINEAR_AUDIO AC_USE_DIGITAL_AUDIO.
**	2002.10.22	DS	Removed mbQueryCollectionInfo (to LoadCtrl).
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.12 AER	Improved mbCreateMidiFilePlayerFromCollection description.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**
******************************************************************************/

#ifndef _MBAE_XMFCOLLECTION_H_
#define _MBAE_XMFCOLLECTION_H_

#include "mBAE_Types.h"


// mbCreateXMFCollection
// --------------------------------------------------
//
mbObjectID			mbCreateXMFCollection(
								mbObjectID system,
								mbResult *outResult);

// mbGetCollectionSongCount
// --------------------------------------------------
// Returns the number of MIDI file resources in the collection
//
mbInt32				mbGetCollectionSongCount(
								mbObjectID collectionID,
								mbResult *outResult);

// mbGetCollectionSongName
// --------------------------------------------------
// Returns the name of the indicated MIDI file resource.
// 'index' is in the range from 0 to mbGetCollectionSongCount - 1.
//
mbString			mbGetCollectionSongName(
								mbObjectID collectionID,
								mbInt16 index,
								mbResult *outResult);

// mbCreateMidiFilePlayerFromCollection
// --------------------------------------------------
// Creates an Midi file player by the specified index.
// 'index' is in the range from 0 to mbGetCollectionSongCount - 1.
// Make sure to call mbDestroy on the object to dispose of it.
//
mbObjectID			mbCreateMidiFilePlayerFromCollection(
								mbObjectID collectionID,
								mbInt16 index,
								mbResult *outResult);

#endif // _MBAE_XMFCOLLECTION_H_
