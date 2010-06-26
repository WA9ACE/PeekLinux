/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: ti1_key.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 2               $|
| CREATED: 28.01.99                     $Modtime:: 28.01.99 15:02   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : TI1_KEY

   PURPOSE : keyboard driver (TI1 VERSION)

   EXPORT  :

   TO DO   :

   $History:: ti1_key.h                                             $
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

#ifndef _DEF_TI1_KEY_H_
#define _DEF_TI1_KEY_H_

/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifdef NEPTUNE_BOARD

#include "bmi_integ_misc.h"

/** Definition of the virtual key identification. */
typedef UINT8 T_KPD_VIRTUAL_KEY_ID;

/** Definition of the key state (pressed or released). */
typedef UINT8 T_KPD_KEY_STATE;

/** Definition of the key press state (first press, long press, repeat press). */
typedef UINT8 T_KPD_PRESS_STATE;

/** Definition of the subscriber identification. */
typedef void* T_KPD_SUBSCRIBER;


#endif /* NEPTUNE_BOARD */
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

#endif

