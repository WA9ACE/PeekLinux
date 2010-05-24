/******************************************************************************
**
**	mBAE_TempoCtrl.h
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
**	2003.12.30 msd	Created file, extracted from mBAE_MidiPlayerCtrl.h
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.07.22 msd	[Bug 1718] Added SetTempoInBPM
**
******************************************************************************/

#ifndef _MBAE_TEMPOCTRL_H_
#define _MBAE_TEMPOCTRL_H_

#include "mBAE_Types.h"


// mbSetMasterTempo
// --------------------------------------------------
// Sets the master tempo of the player.
// Range for this function is 1 (0.001, 1/1000 speed) through 1000 (1.0, normal
// speed) through 10000 (10.0, ten times normal speed).
//
mbResult			mbSetMasterTempo(
								mbObjectID thePlayerID,
								mbMilliUnit tempoFactor);

// mbGetMasterTempo
// --------------------------------------------------
// Returns the master tempo of the player.
// Range for this function is 1 (0.001, 1/1000 speed) through 1000 (1.0, normal
// speed) through 10000 (10.0, ten times normal speed).
//
mbMilliUnit			mbGetMasterTempo(
								mbObjectID thePlayerID,
								mbResult *outResult);

// mbGetTempoInMicrosecondsPerBeat
// --------------------------------------------------
// Returns the tempo of the player in microseconds per quarter note.
//
mbInt32				mbGetTempoInMicrosecondsPerBeat(
								mbObjectID thePlayerID,
								mbResult *outResult);

// mbSetTempoInBeatsPerMinute
// --------------------------------------------------
// Sets the tempo of the player in beats per minute.
//
mbResult			mbSetTempoInBeatsPerMinute(
								mbObjectID thePlayerID,
								mbInt32 tempoBPM);

// mbGetTempoInBeatsPerMinute
// --------------------------------------------------
// Returns the tempo of the player in beats per minute.
//
mbInt32				mbGetTempoInBeatsPerMinute(
								mbObjectID thePlayerID,
								mbResult *outResult);



// Deprecated / Obsolete Functions
// ****************************************************************************
// These functions are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// mbSetRingtonePlayerTempo
// --------------------------------------------------
// This function has been replaced by mbSetMasterTempo.
//
#define mbSetRingtonePlayerTempo			mbSetMasterTempo

// mbGetRingtonePlayerTempo
// --------------------------------------------------
// This function has been replaced by mbGetMasterTempo.
//
#define mbGetRingtonePlayerTempo			mbGetMasterTempo

// mbGetRingtonePlayerBeatsPerMinute
// --------------------------------------------------
// This function has been replaced by mbGetTempoInBeatsPerMinute.
//
#define mbGetRingtonePlayerBeatsPerMinute	mbGetTempoInBeatsPerMinute


#endif // _MBAE_TEMPOCTRL_H_
