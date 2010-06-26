/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: drv_key.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 3               $|
| CREATED: 21.09.98                     $Modtime:: 4.12.98 10:40    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : DRV_KEY

   PURPOSE : keyboard driver interface definitions

   EXPORT  :

   TO DO   :

   $History:: drv_key.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 14.06.99   Time: 12:14
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_DRV_KEY_H_
#define _DEF_DRV_KEY_H_

void key_power_init(void);
void keyInit (void (*signal)(char,char));
void keyExit (void);
void drvKeyUpDown (char upDown, char key);

#endif
