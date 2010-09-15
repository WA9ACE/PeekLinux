/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: drv_tmr.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 3               $|
| CREATED: 21.09.98                     $Modtime:: 11.06.99 13:36   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : DRV_TMR

   PURPOSE : timer driver interface definitions

   EXPORT  :

   TO DO   :

   $History:: drv_tmr.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 14.06.99   Time: 12:13
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_DRV_TMR_H_
#define _DEF_DRV_TMR_H_

int tmrInit (void (*s)(void));
int tmrExit (void);
void tmrStart (U32 ms);
U32 tmrStop (void);
U32 tmrLeft (void);

#endif
