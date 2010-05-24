/******************************************************************************
**
**	mBAE_DRCCtrl.h
**
**	DRC Control
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
**	2003.11.24 msd	Created file.
**	2004.01.08 msd	Enabled DRC Ctrl
**	2004.02.17 AER	Tidied file and cleaned up out-parameter names.
**					Added outResult to mbIsDRCEngaged
**	2004.07.26 msd	Replaced TRUE/FALSE with mbTrue/mbFalse
**
******************************************************************************/

#ifndef _MBAE_DRCCTRL_H_
#define _MBAE_DRCCTRL_H_

#include "mBAE_Types.h"


// mbEngageDRC
// --------------------------------------------------
// Switches the DRC on and off.
//
mbResult			mbEngageDRC(
								mbObjectID theObject,
								mbBool engage);

// mbIsDRCEngaged
// --------------------------------------------------
// Returns mbTrue if the DRC is currently on (engaged), otherwise mbFalse.
//
mbBool				mbIsDRCEngaged(
								mbObjectID theObject,
								mbResult *outResult);

// mbSetDRCParameters
// --------------------------------------------------
// Set any or all of the parameters associated with the Dynamic Range Compressor.
// 'params' is an array of mbParams with the terminating mbParam's mbToken set to NULL.
//
mbResult			mbSetDRCParameters(
								mbObjectID theObject,
								const mbParam *params);

// mbGetDRCParameters
// --------------------------------------------------
// Retrieve any or all of the parameters associated with the Dynamic Range Compressor.
// 'params' is an array of mbParams with the terminating mbParam's mbToken set to NULL.
//
mbResult			mbGetDRCParameters(
								mbObjectID theObject,
								mbParam *outParams);


#endif // _MBAE_DRCCTRL_H_
