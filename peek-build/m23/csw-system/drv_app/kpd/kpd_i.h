/**
 * @file   kpd_i.h
 *
 * Declaration of internal function for keypad driver.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/10/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _KPD_I_H_
#define _KPD_I_H_

#include "kpd/kpd_api.h"
#include "kpd/kpd_cfg.h"
//#include "kpd/kpd_messages_i.h"

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"



/** Definition of the physical key identification. */
typedef INT8 T_KPD_PHYSICAL_KEY_ID;


/** Definition of single notified-mode and multi-notified mode */
#define SN_MODE 0
#define MN_MODE 1

#if ((CHIPSET == 12) || (CHIPSET == 15))

/** Definion of the maximum detectable keys */
#define KPD_MAX_DETECTABLE (KP_COLS +  KP_ROWS + 7) /* As per HW specs the max keys that can be detected is 8. So we keep the array size as KP_COLS+KP_ROWS = 10*/
//#define KPD_MAX_DETECTABLE (KP_COLS +  KP_ROWS + 14) /* As per HW specs the max keys that can be detected is 8. So we keep the array size as KP_COLS+KP_ROWS = 10*/

/** Possible state a key can have in T_RECEIVED_KEY_INFO */
typedef enum{
              released,
              first_press,
              key_press,
              long_press,
              repeat_press
} T_KEY_STATE;

/** Definition of received key info struct */
typedef struct{
                INT8 key;
                T_KEY_STATE key_state;
}T_KPD_RECEIVED_KEY_INFO;

/* Default timer values */
#define KPD_DEFAULT_LONG_TIME         0x07CF  /* 2  Sec */
#define KPD_DEFAULT_REPEAT_TIME       0x03E7  /* 1  Sec */
#define KPD_DEFAULT_TIME_OUT_TIME     0xEA5F  /* 60 Sec */

/* Used Prescalor factor */
#ifdef BTC_VIN
#define BTC_MODIFICATION_KEYPAD_FOR_WHEEL
#endif

#ifdef BTC_MODIFICATION_KEYPAD_FOR_WHEEL
#define KPD_CLOCK_DIVIDER     KPD_CLK_DIV2
#else
#define KPD_CLOCK_DIVIDER     KPD_CLK_DIV32
#endif

/* Keyboard clock frequency */
#define KPD_CLOCK_FREQ        32  /* 32 KHz -> value in KHz!*/

#define KPD_ENABLE_REPEAT 0x80
#define KPD_DISABLE_REPEAT 0x7F

#endif /* (CHIPSET == 12) */



/** Definition of the subscriber Id */
typedef UINT8 T_SUBSCRIBER_ID;

/** Definition of structure for  */
typedef struct { T_SUBSCRIBER_ID subscriber_id;
               } T_SUBSCRIBER;


/* Macro definition for sending trace */
//#define KPD_USE_TRACE

#ifdef KPD_USE_TRACE
#define KPD_SEND_TRACE(text,level) rvf_send_trace("[KPD] "text, sizeof("[KPD] "text)-1, NULL_PARAM, level, KPD_USE_ID )
#define KPD_SEND_TRACE_PARAM(text,param,level) rvf_send_trace("[KPD] "text, sizeof("[KPD] "text)-1, param, level, KPD_USE_ID )
#else
#define KPD_SEND_TRACE(text,level)
#define KPD_SEND_TRACE_PARAM(text,param,level)
#endif



/**
 * @name Internal functions
 *
 */
/*@{*/

/**
 * function: kpd_initialize_keypad_driver
 *
 * This function is called when SWE is initialized.
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if operation cannot be completed
 */
T_RV_RET kpd_initialize_keypad_driver(void);


/**
 * function: kpd_kill_keypad_driver
 *
 * This function is called SWE is "killed".
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if operation cannot be completed
 */
T_RV_RET kpd_kill_keypad_driver(void);


/**
 * function: kpd_add_subscriber
 *
 * This function reserve an id for a new subscriber.
 *
 * @param   subscriber_id  Subscriber identification value.
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if operation cannot be completed
 */
T_RV_RET kpd_add_subscriber(T_SUBSCRIBER_ID* subscriber_id);


/**
 * function: kpd_remove_subscriber
 *
 * This function free subscriber Id when this one unsubscribe.
 *
 * @param   subscriber_id  Subscriber identification value.
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if operation cannot be completed
 */
T_RV_RET kpd_remove_subscriber(T_SUBSCRIBER_ID subscriber_id);

/**
 * function: kpd_subscriber_id_used
 *
 * This function check if an Id is used by a subscriber.
 *
 * @param   subscriber     Subscriber structure used by client.
 * @param   subscriber_id  Subscriber identification value.
 * @return
 *    - TRUE if subscriber Id is used
 *    - FALSE else
 */
BOOL kpd_subscriber_id_used(T_KPD_SUBSCRIBER subscriber, T_SUBSCRIBER_ID* subscriber_id);


/**
 * function: kpd_send_key_event_message
 *
 * @param   physical_key_pressed_id Physical key pressed Id.
 * @param   state                   Key state (pressed or released).
 * @param   press_state             Pressed key state (first press, long press or repeat press).
 * @param   mode                    Mode used by the subscriber.
 * @param   return_path             Return path used by the subscriber.
 */
void kpd_send_key_event_message(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id,
                                T_KPD_KEY_STATE state,
                                T_KPD_PRESS_STATE press_state,
                                T_KPD_MODE mode,
                                T_RV_RETURN return_path);

/**
 * function: kpd_send_status_message
 *
 * This function send staus message for asynchronous process.
 *
 * @param   operation      Concerned operation.
 * @param   status_value   Error identification.
 * @param   return_path    Return path used to prevent the subscriber of the error.
 */
void kpd_send_status_message(UINT8 operation,
                             UINT8 status_value,
                             T_RV_RETURN return_path);

/**
 * function: kpd_is_key_in_sn_mode
 *
 * This function check if keypad is in SN mode and if a key is defined by the keypad owner.
 *
 * @param   physical_key_pressed_id Physical key pressed to check.
 *
 * @return  - TRUE if keypad is in Single-notified mode and key is defined by the keypad owner,
 *          - FALSE else.
 *
 */
BOOL kpd_is_key_in_sn_mode(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id);




/**
 * function: kpd_set_keys_in_sn_mode
 *
 * This function set .
 *
 * @param   keys_owner List of keys defined by the keypad owner as keys only
 *                     notified to the keypad owner.
 * @param   mode       Mode of the keypad owner.
 *
 */
void kpd_set_keys_in_sn_mode(T_KPD_VIRTUAL_KEY_TABLE* keys_owner,
                             T_KPD_MODE mode);


/**
 * function: kpd_is_owner_keypad
 *
 * This function set the subscriber Id which own the keypad.
 *
 * @param   subscriber_id Subscriber identification value.
 *
 * @return  TRUE if the keypad is in SN mode and subscriber_id is the keypad owner,
 *          FALSE else.
 *
 */
BOOL kpd_is_owner_keypad(T_SUBSCRIBER_ID subscriber_id);


/**
 * function: kpd_get_keypad_mode
 *
 * This function returns the current keypad mode (single or multi notified mode).
 *
 * @return  Current keypad mode.
 *
 */
UINT8 kpd_get_keypad_mode(void);

/**
 * function: kpd_set_keypad_mode
 *
 * This function set a new keypad mode. If new keypad mode equal MN_MODE, list of
 * notified keys to keypad owner is unset.
 *
 * @param   mode  New mode to set (Values can be SN_MODE or MN_MODE).
 *
 * @return  None.
 *
 */
void kpd_set_keypad_mode(UINT8 mode);

/**
 * function: kpd_get_owner_keypad_id
 *
 * This function returns the owner keypad Id (This value is correct only if keypad mode
 * is the "Multi-notified" mode).
 *
 * @return  Owner keypad Id.
 *
 */
T_SUBSCRIBER_ID kpd_get_owner_keypad_id(void);

/**
 * function: kpd_set_owner_keypad_id
 *
 * This function set the Id of the keypad owner.
 *
 * @param   subscriber_id  Subscriber Id.
 *
 */
void kpd_set_owner_keypad_id(T_SUBSCRIBER_ID subscriber_id);

#if ((CHIPSET == 12) || (CHIPSET == 15))
/**
 * function: kpd_decode_key_registers
 *
 * This function will process the full code registers
 * of the key pad. And wil return the number of keys pressed
 * as well as the key_id in an array
 *
 * @param   msg_p           pointer to the received message by the
 *                          message handler
 * @param   received_keys_p pointer to the array in which the key id's
 *                          can be stored
 * @return  UINT8           The number of keys pressed
 */
UINT8 kpd_decode_key_registers(T_RV_HDR* msg_p, INT8* received_keys_p);


/**
 * function: kpd_update_received_key_info
 *
 * This function will update the received_key_info table
 *
 * key with the status 'released' will be removed
 * key with the statuse 'first_press' will become key_press
 * all other keys will left alone
 * new keys will we added with the status 'first_press'
 *
 * @param  nb_keys_pressed    the number of keys that are currently pressed
 * @param  key_pressed_id     array with the pressed key values
 */
void kpd_update_received_key_info(UINT8 nb_keys_pressed, INT8* key_pressed_id);


/**
 * function: kpd_remove_released_from_key_info
 *
 * This function will remove the released keys from the received_key_info table
 */
void kpd_remove_released_from_key_info(void);

/**
 * function: kpd_process_long_key_received_key_info
 *
 * This function will update the received_key_info table
 *
 * key with the status 'key_press / first_press' will become 'long_press'
 * key with the statuse 'long_press' will become repeat_press
 *
 * @param  nb_keys_pressed    the number of keys that are currently pressed
 * @param  key_pressed_id     array with the pressed key values
 */
void kpd_process_long_key_received_key_info(UINT8 nb_keys_pressed, INT8* key_pressed_id);


#endif /* (CHIPSET == 12) */

/*@}*/
#endif /* #ifndef _KPD_I_H_ */
