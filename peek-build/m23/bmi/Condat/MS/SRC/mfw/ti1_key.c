/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: ti1_key.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 13              $|
| CREATED: 28.01.99                     $Modtime:: 21.01.00 15:05   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : TI1_KEY

   PURPOSE : keyboard driver interface (TI1 VERSION)

   EXPORT  :

   TO DO   :
+--------------------------------------------------------------------+
   History:

   Sep 18, 2006 REF: OMAPS00094426 - x0039928
   Description: Locosto - KPD Release event would NOT be generated if a key is pressed 
   several times in a short period of time
   Solution : If the long press timer expires mmi checks the bsp kpd status and accordingly
   sets long press bit in the key map.

   
*/

#define ENTITY_MFW
#include "nucleus.h"
#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"

#include <string.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "mfw_kbd.h"
#include "gdi.h"
#include "kbd.h"
#include "kpd_rot.h"

#include "drv_key.h"
#include "ti1_key.h"

#ifndef NEPTUNE_BOARD
#include "kpd/kpd_api.h"
#else
#define PLATFORM_ARCH_ARM
#define PLATFORM_ENDIAN_LITTLE
#define PLATFORM tcs3100Evm
#include "bspSystem.h"
#include "bspKeypad_Gsp.h"
#endif

/* BUG FIX - OMAPS00084167 */
#ifdef FF_MMI_MULTIMEDIA
#include "typedefs.h"
#include "main_system.h"
/*#include "dev_Mbox.h" */ /* 04-07-2006 */
#endif
/* END - OMAPS00084167 */

BOOL powered_on=FALSE;

#define POWER_KEY 254

extern void mmiInit(void);

//TISH modified for MSIM
#ifdef WIN32
#define _SIMULATION_
#endif

/*
 * Marcus: Issue 811: 05:09/2002:
 * Moved kbd_map, simkbd_map and config_map to \g23m\Condat\COM\SRC\DRIVER\keypad.c
 */
                                        /* driver callback          */
static void kbdCb (drv_SignalID_Type *signal_params);

UBYTE keypad_initialize (void);

void sendKeyInd( T_KPD_VIRTUAL_KEY_ID virtual_key_id,
				    T_KPD_KEY_STATE key_state,
				    T_KPD_PRESS_STATE press_state);

static void (*sig) (char,char) = 0;     /* kbd signaling function   */
static char *keyMap = (char *) kbd_map; /* current keyboard mapping */

T_KPD_SUBSCRIBER      subscriber_p; /* keep the subscription indentification value */

//   Sep 18, 2006 REF: OMAPS00094426 - x0039928
#if (BOARD == 71)
extern UBYTE kpd_key;
#endif

typedef struct
{
    U32     msg_id;
  void    (*callback_func)(void *);
    U16 src_addr_id;
  U16 dest_addr_id;
} T_RV_CHECK;



/* JVJ #1874 - new function to initialize the power key*/


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : key_power_init       |
| STATE   : code                        ROUTINE : key_power_init       |
+--------------------------------------------------------------------+

  PURPOSE : initialize the power key handler in the TI driver

*/



void key_power_init()
{
    kbd_Init(kbdCb);
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : keyInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize keyboard driver

*/

void keyInit (void (*s)(char,char))
{
    sig = s;
    /*
    *   read keyboard mapping from EEPROM.
    *   if this fails set keyboard mapping with default values.
    *
    *   EEPROM reading not now !!!
    */
    keyMap = (char *) kbd_map;


//new keypad driver
#ifdef NEPTUNE_BOARD
// Commented by Nitin for Neptune Platfrom. 05/04/2005
//keypad_initialize ();
#else
	keypad_initialize ();
#endif


}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : keyExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize keyboard driver

*/

void keyExit (void)
{
    sig = 0;

	// by Nitin 05/04/2004
#ifndef NEPTUNE_BOARD
  //unsubscribe the client from the keypad driver
#ifndef _SIMULATION_
    kpd_unsubscribe (&subscriber_p);
#endif
// Added by Nitin 05/04/2004
#else /* NOT defined NEPTUNE_BOARD  */
	kpdMgr_unsubscribe(); // Sumit; commented for time being
#endif

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : getKeyIndex       |
+--------------------------------------------------------------------+

  PURPOSE : returns the index to the key provided (or -1 if no match)

*/
char drvGetKeyIndex( char key)
{
	int i;
	TRACE_EVENT_P1("drvGetKeyIndex key = %d", key);
#ifndef _SIMULATION_
    for (i = 0; i < NUM_KPD_KEYS; i++)
    {
        if (keyMap[i] == key)
        {
        	return (i);
        }
    }

#else
                                        /* TEST only ES!!           */
    for (i = 0; i < NUM_KPD_KEYS; i++)
    {
        if (simkbd_map[i] == key)
        {
            return (i);
        }
    }
#endif
    return (char)(0xFE); //No match./*a0393213 warnings removal - -1 changed to 0xFE*/


}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : drvKeyUpDown       |
+--------------------------------------------------------------------+

  PURPOSE : handle key event (called from keyboard)

*/

void drvKeyUpDown (char upDown, char key)
{
    TRACE_EVENT_P2("drvKeyUpDown : key = %d : state = %x", key, upDown);
/* JVJ #1705 The first KEY event can only be the Power Key*/
/* This will start MMI                                    */
    if ((!powered_on))
    {
        powered_on =TRUE;

        /* ADDED BY RAVI - 22-11-2005 - OMAPS00056702 FIX */
#if (BOARD!=61)   
		/* JVJ #1705 MMI will be started here */
        mmiInit();
        return;
#endif
      /* END RAVI - 22-11-2005 */
    }

	if (key != (char)0)
	{
		if ((key >= 48) && (key <=57))
		{
			key -= 47;
		}
		else
		{
			switch(key)
			{
				case 35:
					key = (char)18;
					break;

				case 42:
					key = (char)17;
					break;

				case 45:
					key = (char)11;
					break;

				case 67:
					key = (char)13;
					break;

				case 69:
					key = (char)25;
					break;

				case 86:
					key = (char)16;
					break;

				case 94:
					key = (char)14;
					break;

				case 97:
					key = (char)15;
					break;

				case 98:
					key = (char)12;
					break;

				default:
					key = (char)0;
					break;
			}
		}

		kbd_putMakeAndKey( upDown, key);
	}

    if (!sig) return;                   /* no event handler         */

    sig((char)(!upDown),0);

    return;

}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : mfw_keystroke      |
+--------------------------------------------------------------------+

  PURPOSE : handle keystroke primitive (called from aci-pei)
            keystroke sent via 'PEI-CONFIG'-primitive
            used in Test cases and by RT;

*/

void mfw_keystroke (char * key)
{
  char i;  //, number;  // RAVI

  if (!sig) return;                   /* no event handler         */
  if (!key) return;                   /* no valid key             */

//  number = 0;  // RAVI
  while (*key)
  {
    if (!strcmp(key,"#*43*1#"))
    {
      // EF we need to place the key code into the buffer
      // together with the inverse of the key status, and then call sig
      kbd_putMakeAndKey(0,19);  /* simulate HUP             */
      sig(0,0);
      kbd_putMakeAndKey(1,19);
      sig(0,0);
      key += 7;
      continue;
    }

    if (!strcmp(key,"#*43*0#"))
    {
      kbd_putMakeAndKey(0,18);   /* simulate CALL            */
      sig(0,0);
      kbd_putMakeAndKey(1,18);
      sig(0,0);
      key += 7;
      continue;
    }

    for (i = 0; i < NUM_KPD_KEYS; i++)
    {
      if (!strncmp(key, config_map[i], strlen(config_map[i])))
      {
        kbd_putMakeAndKey(0,i); /* add key down event       */
        sig(0,0);
        kbd_putMakeAndKey(1,i); /* assume key down event    */
        sig(0,0);
        key += strlen(config_map[i]);
        break;
      }
    }
    if (i < NUM_KPD_KEYS)
      continue;                   /* found config code        */

    key++;
  }
}

void mfw_keystroke_long (char * key, UBYTE mode) /* long pressed key         */
{
    char i; //  , number;  // RAVI

    if (!sig) return;                   /* no event handler         */
    if (!key) return;                   /* no valid key             */

   // number = 0;  // RAVI
    while (strlen(key))
    {
      for (i = 0; i < NUM_KPD_KEYS; i++)
      {
        if (!strcmp(key,config_map[i]))
        {
          if (mode)
		      {
	          kbd_putMakeAndKey(0,i);			/* assume key down event    */
	          sig(0,0);
		      }
			    else
		      {
	          kbd_putMakeAndKey(1,i);			/* assume key up event    */
	          sig(0,0);
		      }
            return;
        }
      }
      return;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : kbd_callback       |
+--------------------------------------------------------------------+

  PURPOSE : Called by keyboard driver

*/

#define hCommACI _ENTITY_PREFIXED(hCommACI)

static void kbdCb (drv_SignalID_Type *signal_params)
{
#if defined (NEW_FRAME)
ULONG signal_raw;

#endif

    U16 signal_high;
    U16 signal_low;
	U8   key_code;
	U8   key_state;

	int nPresses; //Number of key presses still in the queue





#if defined (NEW_FRAME)
    signal_raw = (ULONG)signal_params->UserData;
    signal_high = (U16) (signal_raw >> 16);
    signal_low = (U16) (signal_raw & 0xFFFF);
#else
    signal_high = (U16) (signal_params->UserData >> 16);
    signal_low = (U16) (signal_params->UserData & 0xFFFF);
#endif

	key_code = (U8) (signal_low & 0xFF);
    if (signal_high == 1)
		key_state = KEY_STAT_PRS;
    else
		key_state = KEY_STAT_REL;

/*JVJ #1705 When the first POWER KEY event arrives, an event is sent to MMI */
/*to start it */
if ((!powered_on)&&(key_code==POWER_KEY))
{
		sendKeyInd(key_code, key_state, 0);
        return;
}


	//Add keypress to queue of key presses
	nPresses = kbd_getNumElements();


	kbd_putMakeAndKey( (char)key_state, (char)key_code);

	if ((!kbd_stillProcessingKeys()) && (nPresses == 0))
	{	//Only this element present - create and send message.
		sendKeyInd(key_code, key_state, 0);

	}

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE :  keypad_cb          |
+--------------------------------------------------------------------+

  PURPOSE : This CB function is called by the driver when a key is pressed/released
             or a status of an asynch. process is requested.

*/
void keypad_cb (void* parameter)
{
// NITIN
#ifndef NEPTUNE_BOARD
	T_KPD_KEY_EVENT_MSG* event;
	T_KPD_STATUS_MSG*    status;
	T_RV_CHECK*  check;
	




	emo_printf("keypad_cb(): got called\n");
	check = (T_RV_CHECK*)parameter;

	if (check->msg_id EQ KPD_STATUS_MSG)
	{
	    //status message is used
	    status = (T_KPD_STATUS_MSG*) parameter;



	    if (status->status_value EQ KPD_PROCESS_OK)
	    {
	      //ok
	    }
	    else
	    {
	      //failed !
	      return;
	    }
	}
	else
	{
		int nPresses; //Number of key presses still in the queue
	    //event message is used
	    event = (T_KPD_KEY_EVENT_MSG*) parameter;


		//Add keypress to queue of key presses
		nPresses = kbd_getNumElements();

		if (event->key_info.press_state EQ KPD_INSIGNIFICANT_VALUE)
		{
				kbd_putMakeAndKey( (char)event->key_info.state, (char)event->key_info.virtual_key_id );
		}
		else
		{

			//key_ind->key_stat (U8):
			//        00xx 0000 = KEY_PRESSED
			//        00xx 0001 = KEY_RELEASED
			//        0000 00xx = KPD_FIRST_PRESS
			//        0001 00xx = KPD_LONG_PRESS
			//        0010 00xx = KPD_REPEAT_PRESS
			//
			//    x = anystate
//   Sep 18, 2006 REF: OMAPS00094426 - x0039928
// Fix : Store the virtual key info in the global variable kpd_key for later use
#if (BOARD == 71)
			kpd_key = (char)event->key_info.virtual_key_id;
#endif
			kbd_putMakeAndKey( (char)(event->key_info.state | (event->key_info.press_state << 4)),
								  (char)event->key_info.virtual_key_id );
		}

		if ((!kbd_stillProcessingKeys()) && (nPresses == 0))

		{	//Only this element present - create and send message.



			sendKeyInd(event->key_info.virtual_key_id, event->key_info.state, event->key_info.press_state);

		}

	}
#endif // NEPTUNE_BOARD
	return;

}

void HwdKypadRotCB(KPD_ROT_DIRECTION_T direction) {
        T_KPD_KEY_EVENT_MSG param;
	
	emo_printf("HwdKypadRotCB: %d\n", direction);

	param.key_info.virtual_key_id = (direction) ? (KCD_MNUUP+1): (KCD_MNUDOWN+1);
	param.key_info.state = KPD_KEY_PRESSED;
	param.key_info.press_state = KPD_FIRST_PRESS;

        keypad_cb((void *)&param);

	return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE :  keypad_initialize    |
+--------------------------------------------------------------------+

  PURPOSE : initialize keyboard driver

*/
// Commented by Nitin for Neptune Platfrom. 05/04/2005

#ifndef NEPTUNE_BOARD
UBYTE keypad_initialize (void)
{
    T_KPD_MODE          mode;
    T_KPD_VIRTUAL_KEY_TABLE   notified_keys;
    T_RV_RETURN         return_path;
    T_RV_RET            return_value;

  U16 long_press_time = 30; // 3.0s
  U16 repeat_time = 50;     // 5.0s <not used>

  T_KPD_NOTIF_LEVEL     notif_level;

  emo_printf("keypad_initialize(): got called\n");

  mode = KPD_DEFAULT_MODE;

  return_path.callback_func = keypad_cb;
  return_path.addr_id = 0;

  notified_keys.nb_notified_keys = KPD_NB_PHYSICAL_KEYS;

  notified_keys.notified_keys [0] = KPD_KEY_0;
  notified_keys.notified_keys [1] = KPD_KEY_1;
  notified_keys.notified_keys [2] = KPD_KEY_2;
  notified_keys.notified_keys [3] = KPD_KEY_3;
  notified_keys.notified_keys [4] = KPD_KEY_4;
  notified_keys.notified_keys [5] = KPD_KEY_5;
  notified_keys.notified_keys [6] = KPD_KEY_6;
  notified_keys.notified_keys [7] = KPD_KEY_7;
  notified_keys.notified_keys [8] = KPD_KEY_8;
  notified_keys.notified_keys [9] = KPD_KEY_9;
  notified_keys.notified_keys [10] = KPD_KEY_A;
  notified_keys.notified_keys [11] = KPD_KEY_B;
  notified_keys.notified_keys [12] = KPD_KEY_C;
  notified_keys.notified_keys [13] = KPD_KEY_D;
  notified_keys.notified_keys [14] = KPD_KEY_E;
  notified_keys.notified_keys [15] = KPD_KEY_F;
  notified_keys.notified_keys [16] = KPD_KEY_G;
  notified_keys.notified_keys [17] = KPD_KEY_H;
  notified_keys.notified_keys [18] = KPD_KEY_I;
  notified_keys.notified_keys [19] = KPD_KEY_J;
  notified_keys.notified_keys [20] = KPD_KEY_K;
  notified_keys.notified_keys [21] = KPD_KEY_L;
  notified_keys.notified_keys [22] = KPD_KEY_M;
  notified_keys.notified_keys [23] = KPD_KEY_N;
  notified_keys.notified_keys [24] = KPD_KEY_O;
  notified_keys.notified_keys [25] = KPD_KEY_P;
  notified_keys.notified_keys [26] = KPD_KEY_Q;
  notified_keys.notified_keys [27] = KPD_KEY_R;
  notified_keys.notified_keys [28] = KPD_KEY_S;
  notified_keys.notified_keys [29] = KPD_KEY_T;
  notified_keys.notified_keys [30] = KPD_KEY_U;
  notified_keys.notified_keys [31] = KPD_KEY_V;
  notified_keys.notified_keys [32] = KPD_KEY_W;
  notified_keys.notified_keys [33] = KPD_KEY_X;
  notified_keys.notified_keys [34] = KPD_KEY_Y;
  notified_keys.notified_keys [35] = KPD_KEY_Z;
  notified_keys.notified_keys [36] = KPD_KEY_AT;
  notified_keys.notified_keys [37] = KPD_KEY_SPACE;
  notified_keys.notified_keys [38] = KPD_KEY_SHIFT_L;
  notified_keys.notified_keys [39] = KPD_KEY_SHIFT_R;
  notified_keys.notified_keys [40] = KPD_KEY_ENTER;
  notified_keys.notified_keys [41] = KPD_KEY_LOCK;
  notified_keys.notified_keys [42] = KPD_KEY_REDUCE;
  notified_keys.notified_keys [43] = KPD_KEY_DOT;
  notified_keys.notified_keys [44] = KPD_KEY_COMMA;
  notified_keys.notified_keys [45] = KPD_KEY_QUOTE;
  notified_keys.notified_keys [46] = KPD_KEY_NAV_CENTER;
  notified_keys.notified_keys [47] = KPD_KEY_CANCLE;
  notified_keys.notified_keys [48] = KPD_KEY_BACKSPACE;
  notified_keys.notified_keys [49] = KPD_KEY_POWR;
  notified_keys.notified_keys [50] = KPD_KEY_UP;
  notified_keys.notified_keys [51] = KPD_KEY_DOWN;

  if(kpd_rot_subscribe(HwdKypadRotCB))
        emo_printf("kpd_rot_subscribe failed\n");

#ifndef _SIMULATION_
  return_value = kpd_subscribe (&subscriber_p, mode, &notified_keys, return_path);
#else
  return_value = RV_OK; /* do some faking here, to get it compileable */
#endif

  if (return_value EQ RV_INTERNAL_ERR ||
     return_value EQ RV_INVALID_PARAMETER ||
     return_value EQ RV_MEMORY_ERR)
  {
    //failed

    //- RV_INTERNAL_ERR if
    //   - the max of subscriber is reached,
    //   - the software entity is not started, not yet initialized or initialization has
    //     failed
    //- RV_INVALID_PARAMETER if number of virtual keys is not correct.
    //- RV_MEMORY_ERR if memory reach its size limit.

    return 1;
  }

  //Subscribe to key press and key release (at the moment)
  //subscribe to KPD_LONG_KEY_PRESS_NOTIF once implemented.
  notif_level = KPD_FIRST_PRESS_NOTIF | KPD_RELEASE_NOTIF;

#ifndef _SIMULATION_
  return_value = kpd_define_key_notification( subscriber_p, &notified_keys, notif_level,
              long_press_time, repeat_time );
#else
  return_value = RV_OK;
#endif

  if (return_value EQ RV_INVALID_PARAMETER ||
     return_value EQ RV_MEMORY_ERR)
  { //failed
    return 1;
  }
  return 0;
}
// Commented by Nitin for Neptune Platfrom. 05/04/2005
#endif /* NEPTUNE_BOARD*/


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : ti1_key            |
| STATE   : code                        ROUTINE : sendKeyInd       |
+--------------------------------------------------------------------+

  PURPOSE : Sends an MMI_KEYPAD_IND to the protocol stack
*/
void sendKeyInd( T_KPD_VIRTUAL_KEY_ID virtual_key_id,
				    T_KPD_KEY_STATE key_state,
				    T_KPD_PRESS_STATE press_state)
{
#if defined (NEW_FRAME)
	EXTERN T_HANDLE hCommACI;




//	ULONG signal_raw;   // RAVI
#else
	EXTERN T_VSI_CHANDLE hCommACI;
#endif

	PALLOC(key_ind,MMI_KEYPAD_IND);

	key_ind->key_code = (U8)0;

	key_ind->key_stat = 0;

#if defined (NEW_FRAME)
	PSENDX(ACI,key_ind);
#else
#if defined (_TMS470)
	vsi_c_send("",hCommACI,D2P(key_ind),
	        sizeof(T_PRIM_HEADER)+sizeof(T_MMI_KEYPAD_IND));
#else
	PSEND(ACI,key_ind);
#endif
#endif

	return;

}







