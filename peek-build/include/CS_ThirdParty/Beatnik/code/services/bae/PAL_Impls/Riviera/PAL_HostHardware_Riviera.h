/******************************************************************************
**
**	PAL_HostHardware_ARM.h
**
**	ARM - specific host hardware
**
**	(c) Copyright 2000-2002 Beatnik, Inc., All Rights Reserved.
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
**	Rights in Technical Data and Computer Software clause in DFACS
**	252.227-7013 or subparagraphs (c)(1) and (2) of the Commercial
**	Computer Software--Restricted Rights at 48 CFR 52.227-19, as
**	applicable.
**
**	Modification History:
**
**	2002.05.09 SME	Created file.
**	2002.10.03 AER	Renamed AC_HostAPI* PAL*.
**
******************************************************************************/

#ifndef _PAL_HOSTHARDWARE_RIVIERA_H_
#define _PAL_HOSTHARDWARE_RIVIERA_H_

#include "PAL.h"

typedef struct _PAL_HostHardware_Riviera PAL_HostHardware_Riviera;

PAL_HostHardware_Riviera *			PAL_HostHardware_Riviera_New(void);
void							PAL_HostHardware_Riviera_Delete(PAL_HostHardware_Riviera *This);

PAL_EngineOutputToken *PAL_HostHardware_Riviera_GetToken(PAL_AudioHardwareHandle hndl);

#endif //_PAL_HOSTHARDWARE_RIVIERA_H_

