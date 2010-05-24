/******************************************************************************
**
**	mBAE_GroupPlayer.h
**
**	GroupPlayer is designed to associate Player objects together and start,
**	stop, and change them in sync.
**	Create Player objects, associate them, then perform functions.
**	GroupPlayer will return mbGroup_Operation_Error if instructed to
**	perform an operation and one of the contained players returns a code
**	other than mbNo_Error.
**	Deleting an GroupPlayer will NOT deallocate the contained players.
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
**	2002.06.09 AER	Converted time units from float to milliseconds.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.06.20 AER	Fixed description of mbGetPlayerByIndex.
**	2002.06.21 AER	Replaced usage of mbMilliTime with mbTime.
**	2002.06.28 AER	Renamed mbGetPlayerCountOfGroup mbGetNumberOfPlayersInGroup.
**					Renamed mbRemovePlayerFromGroup mbRemovePlayerFromGroupByID.
**	2002.06.30 AER	Cleaned up some documentation.
**					mbFadeGroup no longer supports synchronous fades.
**	2002.09.26 AER	Added group player last result functions.
**					Cleaned up function comments (had improper function names).
**	2003.02.06 AER	Create function now requires an mbSystemID parameter.
**	2004.01.08 AER	Updated to reflect that system is now a regular mbObjectID.
**	2004.02.12 AER	Improved mbGetPlayerFromGroupByIndex description.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**
******************************************************************************/

#ifndef _MBAE_GROUPPLAYER_H_
#define _MBAE_GROUPPLAYER_H_

#include "mBAE_Types.h"


// mbCreateGroupPlayer
// --------------------------------------------------
//
mbObjectID			mbCreateGroupPlayer(
								mbObjectID system,
								mbResult *outResult);

// mbAddPlayerToGroup
// --------------------------------------------------
// Associates a player with the group player.
//
mbResult			mbAddPlayerToGroup(
								mbObjectID groupID,
								mbObjectID playerID);

// mbGetLastGroupResultByID
// --------------------------------------------------
// Returns the last error recorded on a group operation for the given ID.
//
mbResult			mbGetLastGroupResultByID(
								mbObjectID groupID,
								mbObjectID playerID);

// mbGetLastGroupResultByIndex
// --------------------------------------------------
// Returns the last error recorded on a group operation for the given index.
//
mbResult			mbGetLastGroupResultByIndex(
								mbObjectID groupID,
								mbInt16 index);

// mbGetNumberOfPlayersInGroup
// --------------------------------------------------
// Returns the number of players associated with the group player.
//
mbInt16				mbGetNumberOfPlayersInGroup(
								mbObjectID groupID,
								mbResult *outResult);

// mbGetPlayerFromGroupByIndex
// --------------------------------------------------
// Returns the object ID of the player with the indicated index.
// Unlike all other functions that return an mbObjectID, this function merely
// returns the ID of a player stored in the group. Do not call mbDestroy
// on the returned player to dispose of it.
//
mbObjectID			mbGetPlayerFromGroupByIndex(
								mbObjectID groupID,
								mbInt16 index,
								mbResult *outResult);

// mbRemovePlayerFromGroupByID
// --------------------------------------------------
// Disassociates a player from the group player by the specified pointer.
// Note that this function will reorder the index numbers of other players.
//
mbResult			mbRemovePlayerFromGroupByID(
								mbObjectID groupID,
								mbObjectID playerID);

// mbRemovePlayerFromGroupByIndex
// --------------------------------------------------
// Disassociates a player from the group player by the specified index.
// Note that this function will reorder the index numbers of other players.
//
mbResult			mbRemovePlayerFromGroupByIndex(
								mbObjectID groupID,
								mbInt16 index);

// mbStartPriorityAttenuation
// --------------------------------------------------
// Fades the volume of all players associated with the group player with a
// priority less than 'priority' to 'volume' over 'fadeTime' milliseconds.
//
mbResult			mbStartPriorityAttenuation(
								mbObjectID groupID,
								mbInt16 priority,
								mbMilliUnit volume,
								mbTime fadeTime);

// mbStopPriorityAttenuation
// --------------------------------------------------
// Fades the volume of all players associated with the group player affected
// by the most recent call to StartPriorityAttenuation to their original
// levels over 'fadeTime' milliseconds.
//
mbResult			mbStopPriorityAttenuation(
								mbObjectID groupID,
								mbTime fadeTime);

// mbAreAnyPlayingInGroup
// --------------------------------------------------
// Returns whether any of the players associated with the group player are
// playing.
//
mbBool				mbAreAnyPlayingInGroup(
								mbObjectID groupID,
								mbResult *outResult);

// mbSetGroupVolume
// --------------------------------------------------
// Calls the appropriate SetVolume function on all players associated with the
// group player.
//
mbResult			mbSetGroupVolume(
								mbObjectID groupID,
								mbMilliUnit volume);

// mbSetGroupStereoPosition
// --------------------------------------------------
// Calls the appropriate SetStereoPosition function on all players associated
// with the group player.
//
mbResult			mbSetGroupStereoPosition(
								mbObjectID groupID,
								mbMilliUnit stereoPosition);

// mbStartGroup
// --------------------------------------------------
// Calls the appropriate Start function on all players associated with the
// group player.
//
mbResult			mbStartGroup(
								mbObjectID groupID);

// mbStopGroup
// --------------------------------------------------
// Calls the appropriate Stop function on all players associated with the
// group player.
//
mbResult			mbStopGroup(
								mbObjectID groupID,
								mbTime fadeTime);

// mbPauseGroup
// --------------------------------------------------
// Calls the appropriate Pause function on all players associated with the
// group player.
//
mbResult			mbPauseGroup(
								mbObjectID groupID);

// mbResumeGroup
// --------------------------------------------------
// Calls the appropriate Resume function on all players associated with the
// group player.
//
mbResult			mbResumeGroup(
								mbObjectID groupID);

// mbFadeGroup
// --------------------------------------------------
// Calls the appropriate Fade function on all players associated with the
// group player (always does the fade asynchronously).
//
mbResult			mbFadeGroup(
								mbObjectID groupID,
								mbMilliUnit endVolume,
								mbTime fadeTime);

// mbEnableGroupLooping
// --------------------------------------------------
// Calls the appropriate EnableLooping function on all players associated with
// the group player.
//
mbResult			mbEnableGroupLooping(
								mbObjectID groupID,
								mbBool isEnabled);

// mbSetGroupLoopCount
// --------------------------------------------------
// Calls the appropriate SetLoopCount function on all players associated with
// the group player.
//
mbResult			mbSetGroupLoopCount(
								mbObjectID groupID,
								mbInt32 loopCount);

// mbSetGroupPosition
// --------------------------------------------------
// Calls the appropriate SetPosition function on all players associated with
// the group player.
//
mbResult			mbSetGroupPosition(
								mbObjectID groupID,
								mbTime position);

#endif // _MBAE_GROUPPLAYER_H_
