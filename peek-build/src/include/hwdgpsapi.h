#ifndef _HWDGPSAPI_H_
#define _HWDGPSAPI_H_
/***********************************************************************************
* 
* FILE NAME   :     hwdgpsapi.h
*
* DESCRIPTION :     GPS Hardware Driver API        
*
* HISTORY     :
*     See Log at end of file
*
* Copyright (c) 2002 LSI Logic. All rights reserved. LSI Confidential information.
************************************************************************************/
#include "sysdefs.h"

/*----------------------------------------------------------------------------
 Global Defines 
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
extern void HwdGpsUnavReset(void);
extern void HwdGpsUnavPowerOn(void);
extern void HwdGpsUnavPowerOff(void);
extern void HwdGpsUnavWiggle( uint16 NumWiggleCycles );
extern void HwdGpsUnavTimeSyncPulseEnable( bool Enable );




/*****************************************************************************
* $Log: hwdgpsapi.h $
* Revision 1.1  2007/10/29 10:52:47  binye
* Initial revision
* Revision 1.1  2007/10/09 15:12:45  binye
* Initial revision
* Revision 1.1  2004/01/22 10:42:28  fpeng
* Initial revision
* Revision 1.2  2002/05/31 16:08:42  robertk
* Added TimeSyncEnable function prototype.
* Revision 1.1  2002/05/20 09:41:03  robertk
* Initial revision
*****************************************************************************/

#endif
