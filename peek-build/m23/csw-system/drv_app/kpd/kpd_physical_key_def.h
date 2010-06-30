/**
 * @file   kpd_physical_key_def.h
 *
 * Definition of physical key values available on keypad.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/02/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

/** Definition of the physical key ID.
 */

#ifndef _KPD_PHYSICAL_KEY_DEF_H_
#define _KPD_PHYSICAL_KEY_DEF_H_

#ifndef _WINDOWS
   #include "board.cfg"
#endif

/** Definition of the physical key ID.
 */
#define KPD_PKEY_NULL   (-1)
#define KPD_PKEY_0           (0)
#define KPD_PKEY_1           (1)
#define KPD_PKEY_2           (2)
#define KPD_PKEY_3           (3)
#define KPD_PKEY_4           (4)
#define KPD_PKEY_5           (5)
#define KPD_PKEY_6           (6)
#define KPD_PKEY_7           (7)
#define KPD_PKEY_8           (8)
#define KPD_PKEY_9           (9)
#define KPD_PKEY_UP          (10)
#define KPD_PKEY_DOWN        (11)
#define KPD_PKEY_SOFT_LEFT   (12)
#define KPD_PKEY_SOFT_RIGHT  (13)
#define KPD_PKEY_GREEN       (14)
#define KPD_PKEY_RED         (15)
#define KPD_PKEY_DIESE       (16)
#define KPD_PKEY_STAR        (17)

#if (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (defined _WINDOWS)

#define KPD_PKEY_VOL_UP      (18)
#define KPD_PKEY_VOL_DOWN    (19)
#define KPD_PKEY_LEFT        (20)
#define KPD_PKEY_RIGHT       (21)
#define KPD_PKEY_NAV_CENTER  (22)
#define KPD_PKEY_REC         (23)

#endif

#if 0// ((BOARD==70) || (BOARD==71))
#define KPD_PKEY_VOL_UP      (18)
#define KPD_PKEY_VOL_DOWN    (19)
#define KPD_PKEY_LEFT        (20)
#define KPD_PKEY_RIGHT       (21)
#define KPD_PKEY_NAV_CENTER  (22)
#define KPD_PKEY_HOME        (23)
#define KPD_PKEY_BACK        (24)
#endif

#if ((BOARD==70) || (BOARD==71))
#define KPD_PKEY_A          (10)
#define KPD_PKEY_B          (11)
#define KPD_PKEY_C          (12)
#define KPD_PKEY_D          (13)
#define KPD_PKEY_E          (14)
#define KPD_PKEY_F          (15)
#define KPD_PKEY_G          (16)
#define KPD_PKEY_H          (17)
#define KPD_PKEY_I          (18)
#define KPD_PKEY_J          (19)
#define KPD_PKEY_K          (20)
#define KPD_PKEY_L          (21)
#define KPD_PKEY_M          (22)
#define KPD_PKEY_N          (23)
#define KPD_PKEY_O          (24)
#define KPD_PKEY_P          (25)
#define KPD_PKEY_Q          (26)
#define KPD_PKEY_R          (27)
#define KPD_PKEY_S          (28)
#define KPD_PKEY_T          (29)
#define KPD_PKEY_U          (30)
#define KPD_PKEY_V          (31)
#define KPD_PKEY_W          (32)
#define KPD_PKEY_X          (33)
#define KPD_PKEY_Y          (34)
#define KPD_PKEY_Z          (35)
#define KPD_PKEY_AT         (36)
#define KPD_PKEY_SPACE      (37)
#define KPD_PKEY_SHIFT_L      (38)
#define KPD_PKEY_SHIFT_R      (39)
#define KPD_PKEY_ENTER      (40)
#define KPD_PKEY_LOCK       (41)
#define KPD_PKEY_REDUCE     (42)        //-
#define KPD_PKEY_DOT        (43)            //.
#define KPD_PKEY_COMMA      (44)        //,
#define KPD_PKEY_QUOTE      (45)        //'
#define KPD_PKEY_NAV_CENTER (46)    //ok
#define KPD_PKEY_CANCLE (47)
#define KPD_PKEY_BACKSPACE  (48)        //<--
#define KPD_PKEY_POWR        (49)
#define KPD_PKEY_UP          (50)
#define KPD_PKEY_DOWN        (51)//zhangfanghui

#endif



/* The PWR key is mapped on Analog Base Band, it's the SPI which inform
   the keypad that the PWR key is pressed, so the process for this key
   is different of the others, a subscriber can be notified only by a
   short press of the PWR key, even if this key has been set for a long press
   or repeat press with the function kpd_define_repeat_keys. The following
   #define allows to map any key with the PWR key. */
#define KPD_SHORT_PRESS_PWR_KEY KPD_PKEY_RED


#endif /* #ifndef _KPD_PHYSICAL_KEY_DEF_H_ */
