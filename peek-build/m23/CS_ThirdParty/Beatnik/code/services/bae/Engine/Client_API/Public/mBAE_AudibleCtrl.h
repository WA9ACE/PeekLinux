/******************************************************************************
**
**	mBAE_AudibleCtrl.h
**
**	The transport control through which all audible objects are manipulated.
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
**	2002.05.02	DS	Added note about possible distortion to mbSetVolume.
**	2002.06.11 AER	Converted from floating point to mbMilliUnit.
**					Cleaned up formatting and arguments lists.
**	2002.07.25 AER	Updated comments to include information on compile symbols.
**	2002.10.04 AER	Altered comments to match new PAL terminology.
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**	2004.11.15 AER	Revised description of features not build into mobileBAE.
**
******************************************************************************/

#ifndef _MBAE_AUDIBLECTRL_H_
#define _MBAE_AUDIBLECTRL_H_

#include "mBAE_Types.h"


// mbSetVolume
// --------------------------------------------------
// Set the volume of an audible object.
// Range for this operation is 0 (0.0) silence, 1000 (1.0) default, up to
// 3000 (3.0) max.
// Values greater than 1000 may cause distortion in the output.
//
mbResult			mbSetVolume(
								mbObjectID objectID,
								mbMilliUnit volume);

// mbGetVolume
// --------------------------------------------------
// Get the volume of an audible object.
// Range for this operation is 0 (0.0) silence, 1000 (1.0) default, up to
// (3000) 3.0 max.
//
mbMilliUnit			mbGetVolume(
								mbObjectID objectID,
								mbResult *outResult);

// mbSetStereoPosition
// --------------------------------------------------
// Set the stereo position (pan) of an audible object.
// Range for this operation is -1000 (-1.0) left, 0 (0.0) middle,
// 1000 (1.0) right.
// Fails with a result of mbFeature_Not_Supported if the current build of
// mobileBAE does not support stereo output (call mbIsFeatureSupported with
// mbFeature_StereoOutput to verify).
//
mbResult			mbSetStereoPosition(
								mbObjectID objectID,
								mbMilliUnit stereoPosition);

// mbGetStereoPosition
// --------------------------------------------------
// Get the stereo position (pan) of an audible object.
// Range for this operation is -1000 (-1.0) left, 0 (0.0) middle,
// 1000 (1.0) right.
// Fails with a result of mbFeature_Not_Supported if the current build of
// mobileBAE does not support stereo output (call mbIsFeatureSupported with
// mbFeature_StereoOutput to verify).
//
mbMilliUnit			mbGetStereoPosition(
								mbObjectID objectID,
								mbResult *outResult);

// mbSetPriority
// --------------------------------------------------
// Set the priority level of an audible object.
// Higher values indicate that this object should be heard at the expense of
// 	lesser priority objects.
// Range for this operation is 1 (lowest), 5 (default), 10 (highest)
//
mbResult			mbSetPriority(
								mbObjectID objectID,
								mbInt16 priority);

// mbGetPriority
// --------------------------------------------------
// Get the priority level of an audible object.
// Higher values indicate that this object should be heard at the expense of
// 	lesser priority objects.
// Range for this operation is 1 (lowest), 5 (default), 10 (highest)
//
mbInt16				mbGetPriority(
								mbObjectID objectID,
								mbResult *outResult);

#endif // _MBAE_AUDIBLECTRL_H_
