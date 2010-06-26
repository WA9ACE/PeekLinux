/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_kbd.h       $|
| $Author:: Le  $ CONDAT GmbH           $Revision:: 7               $|
| CREATED: 21.09.98                     $Modtime:: 21.12.99 16:30   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_KBD

   PURPOSE : Keyboard types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_kbd.h                                             $
 * 
 * *****************  Version 7  *****************
 * User: Le           Date: 6.01.00    Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Alignment of MFW versions
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:29
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_MFW_KBD_H_
#define _DEF_MFW_KBD_H_

#include "mfw_mfw.h"


                                        /* KEYMAP DEFINITION        */
#define KEY_0           0x00000001
#define KEY_1           0x00000002
#define KEY_2           0x00000004
#define KEY_3           0x00000008
#define KEY_4           0x00000010
#define KEY_5           0x00000020
#define KEY_6           0x00000040
#define KEY_7           0x00000080
#define KEY_8           0x00000100
#define KEY_9           0x00000200
#define KEY_STAR        0x00000400
#define KEY_HASH        0x00000800
#define KEY_VOLUP       0x00001000
#define KEY_VOLDOWN		0x00002000
#define KEY_MNUUP		0x00004000
#define KEY_MNUDOWN		0x00008000
#define KEY_LEFT        0x00010000
#define KEY_RIGHT       0x00020000
#define KEY_CALL        0x00040000
#define KEY_HUP         0x00080000
#define KEY_OK          0x00100000
#define KEY_CLEAR       0x00200000
#define KEY_ABC         0x00400000
#define KEY_POWER       0x00800000

#define KEY_MNULEFT		0x01000000
#define KEY_MNURIGHT	0x02000000
//xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
//Wrong hex code was defined earlier. Changed from 0x08000000 
//to 0x04000000 since  KCD_MNUSELECT is 26
#define KEY_MNUSELECT	0x04000000 


#define KEY_MAX         0x10000000
#define KEY_AUTO        0x20000000
#define KEY_LONG        0x40000000
#define KEY_MAKE        0x80000000
                                        /* KEYGROUP DEFINITIONS     */
#define KEY_DIGITS      0x000003ff
#define KEY_ALL         0x0fffffff
#define KEY_FLAGS       0xe0000000

                                        /* KEYCODE DEFINITION       */
#define KCD_0           0
#define KCD_1           1
#define KCD_2           2
#define KCD_3           3
#define KCD_4           4
#define KCD_5           5
#define KCD_6           6
#define KCD_7           7
#define KCD_8           8
#define KCD_9           9
#define KCD_STAR        10
#define KCD_HASH        11
#define KCD_VOLUP       12
#define KCD_VOLDOWN     13
#define KCD_MNUUP       14
#define KCD_MNUDOWN     15
#define KCD_LEFT        16
#define KCD_RIGHT       17
#define KCD_CALL        18
#define KCD_HUP         19
#define KCD_OK          20
#define KCD_CLEAR       21
#define KCD_ABC         22
#define KCD_POWER       23
#define KCD_MNULEFT     24
#define KCD_MNURIGHT    25
#define KCD_MNUSELECT   26
#define KCD_MAX         28
#define KCD_AUTO        29
#define KCD_LONG        30
#define KCD_MAKE        31

#ifdef MMI_EM_ENABLED
#ifndef NEPTUNE_BOARD
/* This is not valid for Neptune Engineering Mode, hence*/

//xrashmic 14 Dec, 2005 OMAPS00050986
//when no match is found in drvGetKeyIndex, it returns -1, 
//Some of the keys have the value set to 0xFE. Hence defining KCD_NONE as 0xFD
#define KCD_NONE    253    //xrashmic 22 Aug, 2004 MMI-SPR-32798
#endif /* ifndef NEPTUNE_BOARD*/
#endif 						        


#define KEY_CONSUMED	1
#define KEY_PASSED	    0

typedef struct MfwKbdTag                /* KEYBOARD CONTROL BLOCK   */
{
    MfwEvt map;                         /* selection of keys        */
    MfwEvt key;                         /* current key              */
    U8 code;                            /* current key code         */
    MfwCb handler;                      /* event handler            */
} MfwKbd;

                                        /* PROTOTYPES               */
MfwRes kbdInit (void);
MfwRes kbdExit (void);
MfwCb kbdAlways (MfwCb f);
void kbdTime (long tLong, long tAuto, long tRepeat);
MfwHnd kbdCreate (MfwHnd w, MfwEvt e, MfwCb f);
MfwRes kbdDelete (MfwHnd h);
void kbdSignal (char make, char key);

//Functions to access key presses in the queue
int kbd_getMakeAndKey( char* make, char* key);
int kbd_putMakeAndKey( char make, char key);
int kbd_getNumElements(void);
int kbd_processKeyInput(void);
int kbd_displayUpdateNeeded(void);
void kbd_setDisplayUpdateNeeded(int set);
int kbd_stillProcessingKeys(void);

int mfwKey_skipDisplay( void );

#define PROCESS_EVERY_KEY 1
#define QUEUE_EVERY_KEY 2
#define QUEUE_N_KEYS 3
//The define below also defines the number of keys to be queued
//NB Should be an odd number (above 1) so when swapping between screens,
//the displayed screen changes. (e.g. every third screen is shown)
#define NUM_QUEUE_KEYS 3

/* Added to remove warning */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);

#endif


