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
/* KEY_MUX = 0 */

#define KEY_0          0x00000001
#define KEY_1          0x00000002
#define KEY_2          0x00000004
#define KEY_3          0x00000008
#define KEY_4          0x00000010
#define KEY_5          0x00000020
#define KEY_6          0x00000040
#define KEY_7          0x00000080
#define KEY_8          0x00000100
#define KEY_9          0x00000200
#define KEY_A          0x00000400
#define KEY_B          0x00000800
#define KEY_C          0x00001000
#define KEY_D          0x00002000
#define KEY_E          0x00004000
#define KEY_F          0x00008000
#define KEY_G          0x00010000
#define KEY_H          0x00020000
#define KEY_I          0x00040000
#define KEY_J          0x00080000
#define KEY_K          0x00100000
#define KEY_L          0x00200000
#define KEY_M          0x00400000
#define KEY_N          0x00800000
#define KEY_O          0x01000000
#define KEY_P          0x02000000

/* KEY_MUX = 1 */

#define KEY_Q          0x00000001
#define KEY_R          0x00000002
#define KEY_S          0x00000004
#define KEY_T          0x00000008
#define KEY_U          0x00000010
#define KEY_V          0x00000020
#define KEY_W          0x00000040
#define KEY_X          0x00000080
#define KEY_Y          0x00000100
#define KEY_Z          0x00000200
#define KEY_AT         0x00000400
#define KEY_SPACE      0x00000800
#define KEY_SHIFT_L    0x00001000
#define KEY_SHIFT_R    0x00002000
#define KEY_ENTER      0x00004000
#define KEY_LOCK       0x00008000
#define KEY_REDUCE     0x00010000
#define KEY_DOT        0x00020000
#define KEY_COMMA      0x00040000
#define KEY_QUOTE      0x00080000
#define KEY_NAV_CENTER 0x00100000
#define KEY_CANCLE     0x00200000
#define KEY_BACKSPACE  0x00400000
#define KEY_POWR       0x00800000
#define KEY_UP         0x01000000
#define KEY_DOWN       0x02000000

/* Old key maps */
#define KEY_STAR        KEY_Q
#define KEY_HASH        KEY_R
#define KEY_VOLUP       KEY_S
#define KEY_VOLDOWN     KEY_T
#define KEY_MNUUP       KEY_UP
#define KEY_MNUDOWN     KEY_DOWN
#define KEY_LEFT        KEY_U
#define KEY_RIGHT       KEY_V
#define KEY_CALL        KEY_W
#define KEY_HUP         KEY_X
#define KEY_OK          KEY_Y
#define KEY_CLEAR       KEY_Z
#define KEY_ABC         KEY_QUOTE
#define KEY_POWER       KEY_POWR
#define KEY_MNULEFT     KEY_SHIFT_L
#define KEY_MNURIGHT    KEY_SHIFT_R
#define KEY_MNUSELECT   KEY_NAV_CENTER
/*
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
#define KEY_VOLDOW	0x00002000
#define KEY_MNUUP	0x00004000
#define KEY_MNUDOWN	0x00008000
#define KEY_LEFT        0x00010000
#define KEY_RIGHT       0x00020000
#define KEY_CALL        0x00040000
#define KEY_HUP         0x00080000
#define KEY_OK          0x00100000
#define KEY_CLEAR       0x00200000
#define KEY_ABC         0x00400000
#define KEY_POWER       0x00800000
#define KEY_MNULEFT	0x01000000
#define KEY_MNURIGHT	0x02000000
//xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
//Wrong hex code was defined earlier. Changed from 0x08000000 
//to 0x04000000 since  KCD_MNUSELECT is 26
#define KEY_MNUSELECT	0x04000000 
*/

#define KEY_MUX         0x10000000 /* Mux lower 27 bits */
#define KEY_AUTO        0x20000000
#define KEY_LONG        0x40000000
#define KEY_MAKE        0x80000000

/* KEYGROUP DEFINITIONS     */

#define KEY_DIGITS      0x000003ff
#define KEY_ALL         0x0fffffff
#define KEY_FLAGS       0xE0000000

/* KEYCODE DEFINITION       */
/* KCD_MUX = 0 */
#define KCD_0          (1)
#define KCD_1          (2)
#define KCD_2          (3)
#define KCD_3          (4)
#define KCD_4          (5)
#define KCD_5          (6)
#define KCD_6          (7)
#define KCD_7          (8)
#define KCD_8          (9)
#define KCD_9          (10)
#define KCD_A          (11)
#define KCD_B          (12)
#define KCD_C          (13)
#define KCD_D          (14)
#define KCD_E          (15)
#define KCD_F          (16)
#define KCD_G          (17)
#define KCD_H          (18)
#define KCD_I          (19)
#define KCD_J          (20)
#define KCD_K          (21)
#define KCD_L          (22)
#define KCD_M          (23)
#define KCD_N          (24)
#define KCD_O          (25)
#define KCD_P          (26)

/* KCD_MUX = 1 */
#define KCD_Q          (27)
#define KCD_R          (28)
#define KCD_S          (29)
#define KCD_T          (30)
#define KCD_U          (31)
#define KCD_V          (32)
#define KCD_W          (33)
#define KCD_X          (34)
#define KCD_Y          (35)
#define KCD_Z          (36)
#define KCD_AT         (37)
#define KCD_SPACE      (38)
#define KCD_SHIFT_L    (39)
#define KCD_SHIFT_R    (40)
#define KCD_ENTER      (41)
#define KCD_LOCK       (42)
#define KCD_REDUCE     (43)
#define KCD_DOT        (44)
#define KCD_COMMA      (45)
#define KCD_QUOTE      (46)
#define KCD_NAV_CENTER (47)
#define KCD_CANCLE     (48)
#define KCD_BACKSPACE  (49)
#define KCD_POWR       (50)
#define KCD_UP         (51)
#define KCD_DOWN       (52)

/* Old key codes */
#define KCD_STAR        KCD_Q
#define KCD_HASH        KCD_R
#define KCD_VOLUP       KCD_S
#define KCD_VOLDOWN     KCD_T
#define KCD_MNUUP       KCD_UP
#define KCD_MNUDOWN     KCD_DOWN
#define KCD_LEFT        KCD_U
#define KCD_RIGHT       KCD_V
#define KCD_CALL        KCD_W
#define KCD_HUP         KCD_X
#define KCD_OK          KCD_Y
#define KCD_CLEAR       KCD_Z
#define KCD_ABC         KCD_QUOTE
#define KCD_POWER       KCD_POWR
#define KCD_MNULEFT     KCD_SHIFT_L
#define KCD_MNURIGHT    KCD_SHIFT_R
#define KCD_MNUSELECT   KCD_NAV_CENTER

/*
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
#define KCD_BACK	10
#define KCD_MNUSELECT   11
#define KCD_MNUUP	12
#define KCD_MNUDOWN	13
#define KCD_POWER	14
#define KCD_LSHIFT	15
#define KCD_RSHIFT	16
#define KCD_ENTER	17
#define KCD_LOCK	18
*/

//#define KCD_STAR        10

//#define KCD_HASH        11
//#define KCD_VOLUP       12
//#define KCD_VOLDOWN     13
//#define KCD_MNUUP       14
//#define KCD_MNUDOWN     15
//#define KCD_LEFT        16
//#define KCD_RIGHT       17
//#define KCD_CALL        18
//#define KCD_HUP         19
//#define KCD_OK          20
//#define KCD_CLEAR       21
//#define KCD_ABC         22
//#define KCD_POWER       23
//#define KCD_MNULEFT     24
//#define KCD_MNURIGHT    25
//#define KCD_MNUSELECT   26
#define KCD_MUX		28
#define KCD_AUTO        29
#define KCD_LONG        30
#define KCD_MAKE        31

#define KCD_MAX         51 /* Max key ID */

#ifdef MMI_EM_ENABLED
#ifndef NEPTUNE_BOARD
/* This is not valid for Neptune Engineering Mode, hence*/

//xrashmic 14 Dec, 2005 OMAPS00050986
//when no match is found in drvGetKeyIndex, it returns -1, 
//Some of the keys have the value set to 0xFE. Hence defining KCD_NONE as 0xFD
#define KCD_NONE    0xFD
#endif /* ifndef NEPTUNE_BOARD*/
#endif 						        


#define KEY_CONSUMED	1
#define KEY_PASSED      0

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


