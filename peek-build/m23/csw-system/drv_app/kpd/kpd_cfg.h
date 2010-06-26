/**
 * @file   kpd_cfg.h
 *
 * Configuration file for keypad driver.
 *
 * This file is part of the keypad API.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/11/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _KPD_CFG_H_
#define _KPD_CFG_H_

#ifndef _WINDOWS
   #include "board.cfg"
   #include "chipset.cfg"
#endif

/** Define the number of physical keys on the keypad (including PWR key).
 */
#if (BOARD == 7) || (BOARD == 8) || (BOARD == 9)
   #define KPD_NB_PHYSICAL_KEYS (18)

#elif (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (BOARD == 34) || (BOARD == 35) || (BOARD == 46) || (defined _WINDOWS) 
   #define KPD_NB_PHYSICAL_KEYS (24)
#elif ((BOARD==70) || (BOARD==71))
   #define KPD_NB_PHYSICAL_KEYS (25)
#endif


#if ((CHIPSET == 12) || (CHIPSET == 15))

  #define SetGroupBits16(registre,position,number,value) {\
                                                      UINT16 tmp=registre;\
                                                      volatile UINT16 tmpvalue;\
                                                      tmpvalue = (value<<(16-(number)));\
                                                      tmpvalue = (tmpvalue>>(16-(number)));\
                                                      tmp&=~((0xFFFF>>(16-(number)))<<(position));\
                                                      tmp|=((tmpvalue&(0xFFFF>>(16-(number))))<<(position));\
                                                      registre=tmp;\
                                                      }
/** Types Definitions
 */


/**  T_KPD_PTV : Divisor factor
 */
typedef enum 
  {
    KPD_CLK_DIV2   = 0,
    KPD_CLK_DIV4   = 1,
    KPD_CLK_DIV8   = 2,
    KPD_CLK_DIV16  = 3,
    KPD_CLK_DIV32  = 4,
    KPD_CLK_DIV64  = 5,
    KPD_CLK_DIV128 = 6,
    KPD_CLK_DIV256 = 7
  } T_KPD_PTV;



/**  T_KPD_EnableDetection : Keyboard Detection Enable Type
 */
typedef enum 
  {
    KPD_DETECTION_ENABLED  = 1,
    KPD_DETECTION_DISABLED = 0
  } T_KPD_EnableDetection;

#define    HARDWARE_RESOLUTION  0
#define    NORMAL_RESOLUTION    1


/**  T_KPD_Status : Keyboard State Machine Status Type
 */
typedef enum 
  {
    KPD_IDLE                  = 0x0,
    KPD_SCANNING              = 0x1,
    KPD_LOAD_TIMER_DEBOUNCING = 0x2,
    KPD_TEST_TIMER_DEBOUNCING = 0x3,
    KPD_GEN_IT_EVENT          = 0x4,
    KPD_GEN_IT_KEY_AND_EVENT  = 0x5,
    KPD_LOAD_TIMER_LONG_KEY   = 0x6,
    KPD_TEST_TIMER_LONG_KEY   = 0x7,
    KPD_GEN_IT_LONG_KEY       = 0x8,
    KPD_LOAD_TIMER_TIME_OUT   = 0x9,
    KPD_TEST_TIMER_TIME_OUT   = 0xA,
    KPD_GEN_IT_TIME_OUT       = 0xB,
    KPD_LOAD_TIMER_REPEAT_KEY = 0xC,
    KPD_TEST_TIMER_REPEAT_KEY = 0xD,
    KPD_OTHER                 = 0xF
  } T_KPD_Status;

/**  
 * Keyboard Enable Software Mode Type
 */

#define HARDWARE_DECODING       1
#define SOFTWARE_MODE_ENABLE    0


/**
 * Define the used timer resolution
 */
/*#define KPD_TIME_RESOLUTION   (HARDWARE_RESOLUTION)*/
#define KPD_TIME_RESOLUTION   (NORMAL_RESOLUTION)
/**
 * Define the used hardware resolution
 */
#define KPD_DECODING_MODE     (HARDWARE_DECODING)
/*#define KPD_DECODING_MODE     (SOFTWARE_MODE_ENABLE)*/

#ifdef _WINDOWS
#undef KPD_DECODING_MODE
#define KPD_DECODING_MODE     (SOFTWARE_MODE_ENABLE)
#endif

#endif /* (CHIPSET == 12) */


/**
 * Define the debouncing time that should be used by the hardware
 */
#define KPD_DEBOUNCING_TIME   (0x3F)

/** 
 * Define the number max of client that can subscribe to the keypad driver.
 * This value must be lower or equal than 32.
 */
#define KPD_MAX_SUBSCRIBER (4)

/** Define the number of available mode. This value must be greater or equal to 2
 * because default and alphanumeric mode must always be defined.
 * (This value define the size of kpd_vpm_table array).
 */
#define KPD_NB_MODE  (3)

/** Define the configurable mode value. This value must be defined with the enumerated
 * type T_KPD_MODE. If KPD_MODE_CONFIG is not defined, the configurable mode is not supported.
 */
#define KPD_MODE_CONFIG (KPD_GAME_MODE)

/** Base value for memory bank size, can be increased for customer needs.
 */
#define KPD_MB_BASE_SIZE   (100)





/**
 * @var T_KPD_MODE
 * Enumerated type that defines all the available modes.
 * The mandatory default mode are KPD_DEFAULT_MODE and KPD_ALPHANUMERIC_MODE.
 * Some others mode can be added by the customer (Update KPD_NB_MODE value).
 * Values must be consecutive. KPD_DEFAULT_MODE and KPD_ALPHANUMERIC_MODE must always
 * be respectively equal to 1 and 2.
 */
typedef enum   {  KPD_DEFAULT_MODE = 1,
                  KPD_ALPHANUMERIC_MODE = 2,
                  KPD_GAME_MODE = 3
               } T_KPD_MODE;

/**
 * Definition of all the virtual key ID. All Id's must be distinct.
 */
/* KPD_KEY_NULL must always be defined */
/* KPD_KEY_NULL is not a key that can be notified to a subscriber */
#define KPD_KEY_NULL (0)

#define KPD_KEY_0          (1)
#define KPD_KEY_1          (2)
#define KPD_KEY_2          (3)
#define KPD_KEY_3          (4)
#define KPD_KEY_4          (5)
#define KPD_KEY_5          (6)
#define KPD_KEY_6          (7)
#define KPD_KEY_7          (8)
#define KPD_KEY_8          (9)
#define KPD_KEY_9          (10)
#define KPD_KEY_UP         (11)
#define KPD_KEY_DOWN       (12)
#define KPD_KEY_SOFT_LEFT  (13)
#define KPD_KEY_SOFT_RIGHT (14)
#define KPD_KEY_CONNECT    (15)
#define KPD_KEY_DISCONNECT (16)
#define KPD_KEY_STAR       (17)
#define KPD_KEY_DIESE      (18)

#define KPD_KEY_VOL_UP     (19)
#define KPD_KEY_VOL_DOWN   (20)
#define KPD_KEY_LEFT       (21)
#define KPD_KEY_RIGHT      (22)
#define KPD_KEY_ENTER      (23)
#define KPD_KEY_RECORD     (24)


/* This virtual key is defined for Condat callback when Power key is pressed
   until new interface is defined with Condat */
#define KPD_PWR            (25)

#define KPD_KEY_HOME	   (26)
#define KPD_KEY_BACK       (27)

#define KPD_KEY_ABC        (30)
#define KPD_KEY_DEF        (31)
#define KPD_KEY_GHI        (32)
#define KPD_KEY_JKL        (33)
#define KPD_KEY_MNO        (34)
#define KPD_KEY_PQR        (35)
#define KPD_KEY_ST         (36)
#define KPD_KEY_UV         (37)
#define KPD_KEY_WX         (38)
#define KPD_KEY_YZ         (39)

#if (BOARD == 7)
   #define KP_ROWS      5
   #define KP_COLS      4
#elif ((BOARD == 8) || (BOARD == 9))
   #define KP_ROWS      5
   #define KP_COLS      4
#elif ((BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43)|| (BOARD==70) || (BOARD==71))
   #define KP_ROWS      5
   #define KP_COLS      5
#endif


#endif /* _KPD_CFG_H_ */
