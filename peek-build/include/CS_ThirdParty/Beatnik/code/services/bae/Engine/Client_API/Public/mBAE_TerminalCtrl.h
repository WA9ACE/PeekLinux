/******************************************************************************
**
**	mBAE_TerminalCtrl.h
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
**	2004.01.05 msd	Created file.
**
******************************************************************************/

#ifndef _MBAE_TERMINALCTRL_H_
#define _MBAE_TERMINALCTRL_H_

#include "mBAE_Types.h"


// mbEnableVibrator
// --------------------------------------------------
// Sets whether the object responds to events that attempt to activate the vibrator.
//
mbResult			mbEnableVibrator(
								mbObjectID objectID,
								mbBool isEnabled);

// mbIsVibratorEnabled
// --------------------------------------------------
// Returns whether the object responds to events that activate the vibrator.
//
mbBool				mbIsVibratorEnabled(
								mbObjectID objectID,
								mbResult *outResult);

// mbEnableLED
// --------------------------------------------------
// Sets whether the object responds to events that attempt to activate the LED.
//
mbResult			mbEnableLED(
								mbObjectID objectID,
								mbBool isEnabled);

// mbIsLEDEnabled
// --------------------------------------------------
// Returns whether the object responds to events that activate the LED.
//
mbBool				mbIsLEDEnabled(
								mbObjectID objectID,
								mbResult *outResult);

// mbEnableMiscDevices
// --------------------------------------------------
// Sets whether the object responds to events that attempt to activate other
// terminal devices (devices other than the Vibrator and LED).
//
mbResult			mbEnableMiscTerminalDevices(
								mbObjectID objectID,
								mbBool isEnabled);

// mbIsLEDEnabled
// --------------------------------------------------
// Returns whether the object responds to events that activate misc terminal
// devices (devices other than the Vibrator and LED).
//
mbBool				mbAreMiscTerminalDevicesEnabled(
								mbObjectID objectID,
								mbResult *outResult);



// Deprecated / Obsolete Functions
// ****************************************************************************
// These functions are outdated and should be replaced with newer versions.
// These definitions remain for backward compatibility.
// ****************************************************************************

// mbAllowRingtonePlayerVibrator
// --------------------------------------------------
// This function has been replaced by mbEnableVibrator.
//
#define mbAllowRingtonePlayerVibrator		mbEnableVibrator

// mbIsVibratorAllowedOnRingtonePlayer
// --------------------------------------------------
// This function has been replaced by mbIsVibratorEnabled.
//
#define mbIsVibratorAllowedOnRingtonePlayer(obj)	mbIsVibratorEnabled(obj, NULL)

// mbAllowVibrator
// --------------------------------------------------
// This function has been replaced by mbEnableVibrator.
//
#define mbAllowVibrator						mbEnableVibrator

// mbIsVibratorAllowed
// --------------------------------------------------
// This function has been replaced by mbIsVibratorEnabled.
//
#define mbIsVibratorAllowed(obj)			mbIsVibratorEnabled(obj, NULL)

// mbAllowLed
// --------------------------------------------------
// This function has been replaced by mbEnableLED.
//
#define mbAllowLed							mbEnableLED

// mbIsLedAllowed
// --------------------------------------------------
// This function has been replaced by mbIsLEDEnabled.
//
#define mbIsLedAllowed(obj)					mbIsLEDEnabled(obj, NULL)

#endif // _MBAE_TERMINALCTRL_H_
