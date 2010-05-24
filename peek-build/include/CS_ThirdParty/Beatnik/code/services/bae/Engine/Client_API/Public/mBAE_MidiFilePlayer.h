/******************************************************************************
**
**	mBAE_MidiFilePlayer.h
**
**	mbMidiFilePlayer plays back linear MIDI data loaded from disk, flash,
**	memory, etc.
**
**	MidiFilePlayers may use the controls found in the following files:
**		mBAE_AudibleCtrl.h
**		mBAE_LoadCtrl.h
**		mBAE_PlayerCtrl.h
**		mBAE_MidiPlayerCtrl.h
**		mBAE_DRCCtrl.h
**		mBAE_TempoCtrl.h
**		mBAE_TerminalCtrl.h
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
**	2002.03.28 msd	Added mbGetMidiSynthCount(), added index to mbGetMidiSynth()
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.06.24 AER	Moved mbGetMidiSynth/Count to mBAE_MidiPlayerCtrl.h.
**	2002.10.22	DS	Removed mbQueryMidiFileInfo (now part of LoadCtrl).
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**
******************************************************************************/

#ifndef _MBAE_MIDIFILEPLAYER_H_
#define _MBAE_MIDIFILEPLAYER_H_

#include "mBAE_Types.h"


// mbCreateMidiFilePlayer
// --------------------------------------------------
//
mbObjectID			mbCreateMidiFilePlayer(
								mbObjectID system,
								mbResult *outResult);


#endif // _MBAE_MIDIFILEPLAYER_H_
