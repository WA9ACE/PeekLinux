#include "nucleus.h"
#include "typedefs.h"
#include "kpd_api.h"
#include "kpd_cfg.h"
#include "kpd_rot.h"

#include "exeapi.h"
#include "hwtask.h"
#include "emo_kbd.h"

static T_KPD_SUBSCRIBER subscriber_p;

typedef struct
{
	U32     msg_id;
	void    (*callback_func)(void *);
	U16 src_addr_id;
	U16 dest_addr_id;
} T_RV_CHECK;

static void keySignalEvent(char state, char key)
{
	static KBD_EVENT event;

	event.state = state;
	event.key = key;

	hwEventTrigger(HW_KEYBOARD_EVENT, (void *)(&event));
}

static void keypad_cb (void* parameter)
{
	T_KPD_KEY_EVENT_MSG* event;
	T_KPD_STATUS_MSG*    status;
	T_RV_CHECK*  check;

	emo_printf("keypad_cb(): got called\n");
	check = (T_RV_CHECK*)parameter;

	if (check->msg_id EQ KPD_STATUS_MSG)
	{
		//status message is used
		status = (T_KPD_STATUS_MSG*) parameter;

		if (status->status_value != KPD_PROCESS_OK)
			return;
	}
	else
	{
	//	int nPresses; //Number of key presses still in the queue
		//event message is used
		event = (T_KPD_KEY_EVENT_MSG*) parameter;

		//Add keypress to queue of key presses
	//	nPresses = kbd_getNumElements();
		/*
		if (event->key_info.press_state EQ KPD_INSIGNIFICANT_VALUE)
			keySignalEvent((char)event->key_info.state, (char)event->key_info.virtual_key_id);
		else
			keySignalEvent((char)(event->key_info.state | (event->key_info.press_state << 4)), (char)event->key_info.virtual_key_id);
		*/
		//Only this element present - create and send message.
		//if ((!kbd_stillProcessingKeys()) && (nPresses == 0))
		//	sendKeyInd(event->key_info.virtual_key_id, event->key_info.state, event->key_info.press_state);
	}
	BOSMsgSend(EXE_BAL_ID, EXE_MAILBOX_1_ID, HW_KEY_MSG, 0x0, 0x0);
}

static void HwdKypadRotCB(KPD_ROT_DIRECTION_T direction) 
{
	T_KPD_KEY_EVENT_MSG param;

	emo_printf("HwdKypadRotCB: %d\n", direction);

	param.key_info.virtual_key_id = (direction) ? (KPD_KEY_UP): (KPD_KEY_DOWN);
	param.key_info.state = KPD_KEY_PRESSED;
	param.key_info.press_state = KPD_FIRST_PRESS;

	keypad_cb((void *)&param);
}

int keyInit(void) 
{
	T_KPD_MODE          mode;
	T_KPD_VIRTUAL_KEY_TABLE   notified_keys;
	T_RV_RETURN         return_path;
	T_RV_RET            return_value;

	U16 long_press_time = 30; // 3.0s
	U16 repeat_time = 50;     // 5.0s <not used>

	T_KPD_NOTIF_LEVEL     notif_level;

	emo_printf("keyInit()");
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

	return_value = kpd_subscribe (&subscriber_p, mode, &notified_keys, return_path);

	if(kpd_rot_subscribe(HwdKypadRotCB))
		emo_printf("kpd_rot_subscribe failed");

	if (return_value == RV_INTERNAL_ERR || return_value == RV_INVALID_PARAMETER || return_value == RV_MEMORY_ERR)
		return 1;

	/* Subscribe to key press and key release (at the moment) Subscribe to KPD_LONG_KEY_PRESS_NOTIF once implemented */
	notif_level = KPD_FIRST_PRESS_NOTIF | KPD_RELEASE_NOTIF; 
	return_value = kpd_define_key_notification( subscriber_p, &notified_keys, notif_level, long_press_time, repeat_time);

	if (return_value == RV_INVALID_PARAMETER || return_value == RV_MEMORY_ERR)
		return 1;

	return 0;
}

void keyExit(void) 
{
	kpd_unsubscribe(&subscriber_p);
}

void mfw_keystroke_long (char * key, UBYTE mode)  { }
void mfw_keystroke (char * key) { }

