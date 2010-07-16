/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_KBD
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This Module defines the G23 keyboard driver.
|             
|             The driver has two parts:
|             
|             Part 1 is the driver interface (kbd_... )
|             Part 2 is the simulation for Windows
+----------------------------------------------------------------------------- 
*/ 

#ifndef DRV_KBD_C
#define DRV_KBD_C
#endif
/*==== INCLUDES ===================================================*/
#if defined (NEW_FRAME)
#include "nucleus.h"
#include <string.h>
#include "typedefs.h"
#include "gdi.h"
#include "kbd.h"
#else
#include <string.h>
#include "stddefs.h"
#include "gdi.h"
#include "kbd.h"
#endif
/*==== EXPORT =====================================================*/
//TISH modified for MSIM
#if defined (_TMS470) || defined (WIN32)
EXTERN void KP_Init          (void (*mmi_key_pressed)(UBYTE key),
                              void (*mmi_key_relesed)(UBYTE key));
#else
LOCAL  void KP_Init          (void (*mmi_key_pressed)(UBYTE key),
                              void (*mmi_key_relesed)(UBYTE key));
#endif

LOCAL void kbd_key_pressed   (UBYTE act_key);
LOCAL void kbd_key_released  (UBYTE act_key);

GLOBAL void kbd_test (UBYTE key);

/*==== VARIABLES ==================================================*/
drv_SignalCB_Type  kbd_signal_callback = NULL;
UBYTE              kbd_act_key;

/*
 * Marcus: Issue 811: 05:09/2002:
 * Moved kbd_map, simkbd_map and config_map from \bmi\Condat\MS\SRC\mfw\ti1_key.c
 */

const U8 kbd_map [NUM_KPD_KEYS] =            /* ti mapping               */
{                                                   /* default keyboard mapping */
    /* KEY_0              */  0x01,
    /* KEY_1              */  0x02,
    /* KEY_2              */  0x03,
    /* KEY_3              */  0x04,
    /* KEY_4              */  0x05,
    /* KEY_5              */  0x06,
    /* KEY_6              */  0x07,
    /* KEY_7              */  0x08,
    /* KEY_8              */  0x09,
    /* KEY_9              */  0x0A,
    /* KEY_a              */  0x0B,
    /* KEY_b              */  0x0C,
    /* KEY_c              */  0x0D,
    /* KEY_d              */  0x0E,
    /* KEY_e              */  0x0F,
    /* KEY_f              */  0x10,
    /* KEY_g              */  0x11,
    /* KEY_h              */  0x12,
    /* KEY_i              */  0x13,
    /* KEY_j              */  0x14,
    /* KEY_k              */  0x15,
    /* KEY_l              */  0x16,
    /* KEY_m              */  0x17,
    /* KEY_n              */  0x18,
    /* KEY_o              */  0x19,
    /* KEY_p              */  0x1A,
    /* KEY_q              */  0x1B,
    /* KEY_r     	  */  0x1C,
    /* KEY_s              */  0x1D,
    /* KEY_t              */  0x1E,
    /* KEY_u              */  0x1F,
    /* KEY_v              */  0x20,
    /* KEY_w              */  0x21,
    /* KEY_x              */  0x22,
    /* KEY_y              */  0x23,
    /* KEY_z              */  0x24,
    /* KEY_AT             */  0x25, // @
    /* KEY_SPACE          */  0x26, // Space bar
    /* KEY_RSHIFT         */  0x27, // Right shift
    /* KEY_LSHIFT         */  0x28, // Left shift 
    /* KEY_ENTER          */  0x29, // enter 
    /* KEY_LOCK           */  0x2A, // lock
    /* KEY_MINUS          */  0x2B, // -
    /* KEY_COMMA          */  0x2C, // ,
    /* KEY_DOT            */  0x2D, // .
    /* KEY_QUOTE          */  0x2E, // '
    /* KEY_WPUSH          */  0x2F, // Push on wheel
    /* KEY_BACK           */  0x30, // Back
    /* KEY_DEL            */  0x31, // Delete
    /* KEY_POWER          */  0x32, // Power
    /* KEY_NULL		 */   0x33
};

const U8 simkbd_map [NUM_KPD_KEYS] =         /* MOB SIM mapping          */
{                                                   /* default keyboard mapping. Same as kbd_map */
    /* KEY_0              */  0x01,
    /* KEY_1              */  0x02,
    /* KEY_2              */  0x03,
    /* KEY_3              */  0x04,
    /* KEY_4              */  0x05,
    /* KEY_5              */  0x06,
    /* KEY_6              */  0x07,
    /* KEY_7              */  0x08,
    /* KEY_8              */  0x09,
    /* KEY_9              */  0x0A,
    /* KEY_STAR           */  0x11,
    /* KEY_HASH          */  0x12,
    /* KEY_VOLUP          */  0xFE,
    /* KEY_VOLDOWN       */ 0xFE,
    /* KEY_MNUUP          */ 0x0B,
    /* KEY_MNUDOWN        */0x0C,
    /* KEY_LEFT            */  0x0D,
    /* KEY_RIGHT          */  0x0E,
    /* KEY_CALL           */  0x0F,
    /* KEY_HUP            */  0x10,
    /* KEY_OK             */  0xFE,
    /* KEY_CLEAR          */  0xFE,
    /* KEY_ABC            */  0xFE,
    /* KEY_POWER          */  0x19,
    /* KEY_MNULEFT         */  0x15,
    /* KEY_MNURIGHT       */  0x16,
    /* KEY_MNUSELECT      */  0x17,
    /* KEY_F4              */  0xFE,
    /* KEY_MAX            */  0xFE,
    /* KEY_AUTO           */  0xFE,
    /* KEY_LONG           */  0xFE,
    /* KEY_MAKE           */  0xFE
};

#if 0
/* the following codes are at least unuseable for d-sample!!!  */
const char * const config_map [NUM_KPD_KEYS] =
{                                                   /* CONFIG prim key names    */
    /* KEY_0            */      "0",
    /* KEY_1            */      "1",
    /* KEY_2            */      "2",
    /* KEY_3            */      "3",
    /* KEY_4            */      "4",
    /* KEY_5            */      "5",
    /* KEY_6            */      "6",
    /* KEY_7            */      "7",
    /* KEY_8            */      "8",
    /* KEY_9            */      "9",
    /* KEY_STAR         */      "STAR",
    /* KEY_HASH         */      "HASH",
    /* KEY_VOLUP        */      "VOL_PLUS",
    /* KEY_VOLDOWN      */      "VOL_MINUS",
    /* KEY_MNUUP        */      "UP",
    /* KEY_MNUDOWN      */      "DOWN",
    /* KEY_LEFT         */      "LEFT",
    /* KEY_RIGHT        */      "RIGHT",
    /* KEY_CALL         */      "SEND",
    /* KEY_HUP          */      "END",
    /* KEY_OK           */      "OK",
    /* KEY_CLEAR        */      "CLEAR",
    /* KEY_ABC          */      "ABC",
    /* KEY_POWER        */      "POWER",
    /* KEY_F1           */      "F1",
    /* KEY_F2           */      "F2",
    /* KEY_F3           */      "F3",
    /* KEY_F4           */      "F4",
    /* KEY_MAX          */      "",
    /* KEY_AUTO         */      "",
    /* KEY_LONG         */      "",
    /* KEY_MAKE         */      ""
};
#endif

 char * const config_map [NUM_KPD_KEYS] =
{                                                   /* CONFIG prim key names    */
    /* unused           */      "X0",  /* dummy placeholder */
    /* KEY_0            */      "0",
    /* KEY_1            */      "1",
    /* KEY_2            */      "2",
    /* KEY_3            */      "3",
    /* KEY_4            */      "4",
    /* KEY_5            */      "5",
    /* KEY_6            */      "6",
    /* KEY_7            */      "7",
    /* KEY_8            */      "8",
    /* KEY_9            */      "9",
    /* KEY_MNUUP        */      "UP",
    /* KEY_MNUDOWN      */      "DOWN",
    /* KEY_LEFT         */      "LSOFT",
    /* KEY_RIGHT        */      "RSOFT",
    /* KEY_CALL         */      "SEND",
    /* KEY_HUP          */      "END",
    /* KEY_STAR         */      "*",
    /* KEY_HASH         */      "#",
    /* KEY_VOLUP        */      "VOL_PLUS",
    /* KEY_VOLDOWN      */      "VOL_MINUS",
    /* KEY_MNULEFT      */      "LEFT",
    /* KEY_MNURIGHT     */      "RIGHT",
    /* KEY_MNUSELECT    */      "CENTER",
    /* voice memo ???   */      "EXTRA",
    /* unused           */      "X26",  /* dummy placeholder */
    /* unused           */      "X27",  /* dummy placeholder */
    /* unused           */      "X28",  /* dummy placeholder */
    /* unused           */      "X29",  /* dummy placeholder */
    /* unused           */      "X30",  /* dummy placeholder */
    /* unused           */      "X31",  /* dummy placeholder */
    /* unused           */      "X32",  /* dummy placeholder */
};



/*==== FUNCTIONS ==================================================*/
/*==== CONSTANTS ==================================================*/
/*******************************************************************
 *                                                                 *
 * PART I: Driver Interface                                        *
 *                                                                 *
 *******************************************************************/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_Init                   |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the driver´s internal data. The
            function returns KBD_INITIALIZED if the driver has already
            been initialized and is ready to be used or already in use.
            In case of an initialization failure, which means that the
            driver cannot be used, the function returns KBD_INITFAILURE.
            After initialization, the driver is ready to handle
            keyboard status changes.

*/

GLOBAL UBYTE kbd_Init (drv_SignalCB_Type in_SignalCBPtr)
{
  kbd_signal_callback = in_SignalCBPtr;    /* store call-back function */

  /*
   * Initialise TI driver with internal callback functions
   */
  KP_Init (kbd_key_pressed, kbd_key_released);
  

  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_Exit                   |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when the driver functionality is
            not longer needed. The function "de-allocates" all
            allocated resources and finalizes the driver.

*/

GLOBAL void kbd_Exit (void)
{
  kbd_signal_callback = NULL;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_SetConfig              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to set the typematic rate settings
            of the keyboard driver. After a successful completion,
            the driver uses the new configuration on following
            keyboard events (e.g. key press). If one of the parameters
            included in the driver control block is invalid, the function
            returns DRV_INVALID_PARAMS. To retrieve the driver´s default
            configuration, call the function kbd_GetConfig().

*/

GLOBAL UBYTE kbd_SetConfig (kbd_DCB_Type * in_DCB_Ptr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_GetConfig              |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to retrieve the typematic rate
            settings of the driver. The configuration is returned
            in the driver control block to which the pointer provided
            out_DCBPtr points. The typematic configuration can be set
            by using the kbd_SetConfig() function.
            If the driver is not configured, the function returns
            KBD_NOTCONFIGURED.

*/

GLOBAL UBYTE kbd_GetConfig (kbd_DCB_Type * out_DCBPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_SetSignal              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to define a signal that indicates
            keyboard status changes to a process. A keyboard status
            change is an event identified in the signal information
            data type as SignalType. The only signal that can be set is
            the keyboard status change signal as described in the
            interface documentation.

*/

GLOBAL UBYTE kbd_SetSignal (drv_SignalID_Type * in_SignalIDPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_ResetSignal            |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to remove a signal that has previously
            set. The signal that is removed is identified by the
            signal information data element called SignalType.

*/

GLOBAL UBYTE kbd_ResetSignal (drv_SignalID_Type * in_SignalIDPtr)
{
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_KBD                    |
| STATE   : code                ROUTINE : kbd_GetStatus              |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to retrieve the current (latest)
            status of the keyboard.

*/

GLOBAL ULONG kbd_GetStatus (void)
{
  return 0L;
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : TIL_MMI                    |
| STATE   : code                ROUTINE : kbd_key_pressed            |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when a key is pressed.

*/

LOCAL void kbd_key_pressed (UBYTE act_key)
{
  drv_SignalID_Type signal_params;
  signal_params.SignalType  = KBD_SIGTYPE_STATUSCHG;

#if defined (NEW_FRAME)
  /*
  	UserData should not be a pointer, NM 12.10.01
  */
  signal_params.UserData = (ULONG*)(0x00010000L | (ULONG)act_key);
#else
  signal_params.SignalValue = 0;
  signal_params.UserData    = 0x00010000L + (ULONG)act_key;
#endif

  kbd_act_key = act_key;

  if (kbd_signal_callback NEQ NULL)
    (*kbd_signal_callback)(&signal_params);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : TIL_MMI                    |
| STATE   : code                ROUTINE : kbd_key_released           |
+--------------------------------------------------------------------+

  PURPOSE : The function is called when a key is released.

*/

LOCAL void kbd_key_released (UBYTE act_key)
{
  drv_SignalID_Type signal_params;
  signal_params.SignalType  = KBD_SIGTYPE_STATUSCHG;


#if defined (NEW_FRAME)
  /*
  	UserData should be use as a pointer;

  	The "act_key" wasnt even used in the old frame.
  	"kbd_act_key" keeps the key_code from the "key-press CB function"
  	and is used for the key-release CB function as well !!
  	It seems to be that the key_code for the key-release from the TI driver
  	isnt correct. This way looks like a work around but it works.

    //old
    //signal_params.UserData = (ULONG*)(0x00000000L | (ULONG)act_key);

  	NM 12.10.01
  */
  signal_params.UserData = (ULONG*)(0x00000000L | (ULONG)kbd_act_key);
#else
  signal_params.SignalValue = 0;
  signal_params.UserData    = 0x00000000L + (ULONG)kbd_act_key;
#endif

  if (kbd_signal_callback NEQ NULL)
    (*kbd_signal_callback)(&signal_params);
}


/*******************************************************************
 *                                                                 *
 * PART II: Simulation for Windows                                 *
 *                                                                 *
 *******************************************************************/

//TISH modified for MSIM
#if defined (_WIN32_)
/*
 * Dummies for driver calls
 */
LOCAL void KP_Init   (void (*mmi_key_pressed)(UBYTE),
                      void (*mmi_key_relesed)(UBYTE))
{
}

/*
 * Stimulation of the keyboard driver
 */
GLOBAL void kbd_test (UBYTE key)
{
  kbd_key_pressed (key);
  kbd_key_released (key);
}

#endif
