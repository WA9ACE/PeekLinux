/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: ti1_tmr.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 3               $|
| CREATED: 28.01.99                     $Modtime:: 20.05.99 10:50   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : TI1_TMR

   PURPOSE : timer driver (TI1 VERSION)

   EXPORT  :

   TO DO   :

   $History:: ti1_tmr.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 14.06.99   Time: 12:15
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 9.02.99    Time: 14:54
 * Created in $/GSM/DEV/MS/SRC/MFW
 * TI display & keyboard interface for MFW
*/

#ifndef _DEF_TI1_TMR_H_
#define _DEF_TI1_TMR_H_

#define TMR_PREC    10
#if defined (_TMS470)
#define TMR_TICK    46                  /* ti hardware time         */
#else
#define TMR_TICK    500                 /* use dos ticks            */
#endif

#endif
