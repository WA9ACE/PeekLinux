/******************************************************************************
**
**	mobileBAE.h
**
**	Public Interface for the mobile Beatnik Audio Engine
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
**	2002.01.22 AER	Created file.
**	2002.03.28 msd	Eliminated XMFFilePlayer
**	2002.06.11 AER	Cleaned up formatting and arguments lists.
**	2002.10.23 AER	Added support for ringtone file players.
**	2003.12.09 AER	Added #include for mBAE_Player.h.
**	2003.12.19 SME	Added #include for mBAE_MetadataCtrl.h.
**	2004.01.28 msd	Added new TempoCtrl and TerminalCtrl, removed RingtonePlayerCtrl
**	2004.02.06 SME	Added new ResourceCtrl.
**
******************************************************************************/

#ifndef _MOBILEBAE_H_
#define _MOBILEBAE_H_

#include "mBAE_Types.h"
#include "mBAE_System.h"
#include "mBAE_AudibleCtrl.h"
#include "mBAE_AudioFilePlayer.h"
#include "mBAE_AudioPlayerCtrl.h"
#include "mBAE_AudioStreamPlayer.h"
#include "mBAE_DRCCtrl.h"
#include "mBAE_GroupPlayer.h"
#include "mBAE_TerminalCtrl.h"
#include "mBAE_LoadCtrl.h"
#include "mBAE_MetadataCtrl.h"
#include "mBAE_MidiFilePlayer.h"
#include "mBAE_MidiPlayerCtrl.h"
#include "mBAE_MidiSynth.h"
#include "mBAE_PlayerCtrl.h"
#include "mBAE_ResourceCtrl.h"
#include "mBAE_RingtoneFilePlayer.h"
#include "mBAE_TempoCtrl.h"
#include "mBAE_XMFCollection.h"

#endif // _MOBILEBAE_H_
