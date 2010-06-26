/**
 * @file   kpd_process_internal_msg.h
 *
 * Declaration of internal function for keypad driver.
 * These functions process internal messages received by the keypad task.
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

#ifndef _KPD_PROCESS_INTERNAL_MSG_H_
#define _KPD_PROCESS_INTERNAL_MSG_H_

#include "kpd/kpd_api.h"

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"



#define  NO_KEY_PRESSED   0



/**
 * @name Internal functions
 *
 */
/*@{*/


/**
 * function: kpd_subscribe_i
 *
 * This function is called by the client before any use of the keypad driver services
 * It is called only once.
 *
 * @param   subscriber_id     Subscriber identification value.
 * @param   mode              Mode used by the keypad client.
 * @param   notified_keys_p   Define all the keys the client want to be notified.
 * @param   return_path         Return path for key pressed.
 * @return  RV_OK.
 */
T_RV_RET kpd_subscribe_i(T_SUBSCRIBER_ID subscriber_id,
                         T_KPD_MODE mode,
                         T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                         T_RV_RETURN return_path);

/**
 * function: kpd_unsubscribe_i
 *
 * This function unsubscribes a client from the keypad driver.
 *
 * @param   subscriber_id  Subscriber identification value.
 * @return  RV_OK.
 */
T_RV_RET kpd_unsubscribe_i( T_SUBSCRIBER_ID subscriber_id);

/**
 * function: kpd_define_key_notification_i
 *
 * This function defines time repetition for a set of keys.
 * By default, all the keys are defined as KPD_RELEASE_NOTIF.
 *
 * @param   subscriber_id        Subscriber identification value.
 * @param   repeat_key_table_p   Set of keys which must be repeated.
 * @param   notif_level          Define what kind of notification is set for all the keys.
 * @param   long_press_time      Time in milliseconds before long press time notification.
 * @param   repeat_time          Time in millliseconds for key repetition. If repeat_time = 0,
 *                               keys defined by parameter keys are not repeated.
 * @return  RV_OK.
 */
T_RV_RET kpd_define_key_notification_i(T_SUBSCRIBER_ID subscriber_id,
                                       T_KPD_VIRTUAL_KEY_TABLE* repeat_key_table_p,
                                       T_KPD_NOTIF_LEVEL notif_level,
                                       UINT16 long_press_time,
                                       UINT16 repeat_time);

/**
 * function: kpd_change_mode_i
 *
 * This function changes the mode for the specific client.
 *
 * @param   subscriber_id     Subscriber identification value.
 * @param   notified_keys_p   Define all the keys the client want to be notified in the new mode.
 * @param   new_mode          New mode in which the client want to switch.
 * @return  RV_OK.
 */
T_RV_RET kpd_change_mode_i(T_SUBSCRIBER_ID  subscriber_id,
                           T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                           T_KPD_MODE new_mode);

/**
 * function: kpd_own_keypad_i
 *
 * This function allows the latest subscriber being the only client to be notified by action
 * on keypad (less CPU time used).
 * After this call, the keypad is in the "single notified" state.
 * This action is cancelled when:
 * -  The function is called with parameter is_keypad_owner to FALSE,
 * -  The subscriber (which own the keypad) unsubscribe from keypad.
 *
 * Note that keypad is in the "multi notified" state if there is no subscriber (particularly
 * at the keypad initialisation).
 *
 * @param   subscriber_id     Subscriber identification value.
 * @param   is_keypad_owner_p Define the state to change.
 *                            TRUE: keypad pass in "single notified" state
 *                            FALSE:  keypad pass in "multi notified" state
 * @param   keys_owner        Set of keys only notified to the subscriber that call this function.
 *                            This is mandatory a subset of the keys defined at subscription.
 * @return  RV_OK.
 */
T_RV_RET kpd_own_keypad_i(T_SUBSCRIBER_ID subscriber_id,
                          BOOL is_keypad_owner,
                          T_KPD_VIRTUAL_KEY_TABLE* keys_owner_p);

/**
 * function: kpd_set_key_config_i
 *
 * This function allows setting dynamically a configuration for new or existing virtual keys.
 * The two tables define a mapping between each entry (new_keys[1] is mapped with reference_keys[1],
 * new_keys[2] is mapped with reference_keys[2], ...).
 * The call of this function doesn't change the mode of the client.
 *
 * @param   subscriber_id     Subscriber identification value.
 * @param   reference_keys_p  Set of keys available on keypad in default mode.
 * @param   new_keys_p        Set of keys which must map with the reference keys.
 * @return  RV_OK.
 */
T_RV_RET kpd_set_key_config_i(T_SUBSCRIBER_ID subscriber_id,
                              T_KPD_VIRTUAL_KEY_TABLE* reference_keys_p,
                              T_KPD_VIRTUAL_KEY_TABLE* new_keys_p);

/**
 * function: kpd_get_available_keys_i
 *
 * This function allows knowing all the available keys in default mode.
 *
 * @param   available_keys_p  Set of keys available on keypad in default mode. The structure
 *                            must be declared by the caller, and is filled by the function (OUT).
 * @return  RV_OK.
 */
T_RV_RET kpd_get_available_keys_i( T_KPD_VIRTUAL_KEY_TABLE* available_keys_p);

/**
 * function: kpd_get_ascii_key_code_i
 *
 * This function return associated ASCII value to defined key.
 *
 * @param   key         Key identification value.
 * @param   mode        Mode in which is defined the link between "key" and "ascii code".
 * @param   ascii_code  Associated ASCII code to parameter "key" (OUT).
 * @return  RV_OK.
 */
T_RV_RET kpd_get_ascii_key_code_i(T_KPD_VIRTUAL_KEY_ID key,
                                  T_KPD_MODE mode,
                                  UBYTE** ascii_code);


/**
 * function: kpd_process_key_pressed_i
 *
 * This function process a key pressed.
 *
 * @param   physical_key_pressed_id Physical key pressed.
 */
void kpd_process_key_pressed_i(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id);


/**
 * function: kpd_process_key_pressed_sn_mode_i
 *
 * This function process a key pressed when the keypad is in Single-notified mode,
 * and if the key is in the table defined by the keypad owner (see function kpd_own_keypad).
 *
 * @param   physical_key_pressed_id Physical key pressed.
 */
void kpd_process_key_pressed_sn_mode_i(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id);



/**
 * function: kpd_wait_for_key_release
 *
 * This function wait that key is released to unmask interrupt.
 * This function is usefull when limits memory size is reached,
 * to be sure that all messages could be sent to all the subscribers
 *
 */
void kpd_wait_for_key_release(void);

#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * function: kpd_set_timerchanges_i
 *
 * Function will set the desired timerchanged notification
 * for the indicated subscriber
 *
 * @param   T_SUBSCRIBER_ID subscriber_id
 * @param   T_KPD_TIMER_TYPE timer_type
 * @return  void
 */
void kpd_set_timerchanges_i(T_SUBSCRIBER_ID subscriber_id,
                            T_KPD_TIMER_TYPE timer_type);


/**
 * function : kpd_set_keypad_timeout_i
 *
 * Function will set the desired timeout notification for
 * the indicated subscriber
 *
 * @param   T_SUBSCRIBER_ID subscriber_id
 * @param   T_KPD_TIMEOUT_TPYE  timeout_type
 * @param   UINT16 timeout_time
 * @return  void
 */ 
void kpd_set_keypad_timeout_i(T_SUBSCRIBER_ID subscriber_id,
                                  T_KPD_TIMEOUT_TYPE  timeout_type,
                                  UINT16 timeout_time);


/**
 * function:  kpd_set_miss_event_i
 *
 * Function will set the desired miss event notification for
 * the indicated subscriber
 *
 * @param   T_SUBSCRIBER_ID subscriber_id
 * @param   T_KPD_MISS_EVENT_TPYE missevent_type
 * @return  void
 */
void kpd_set_miss_event_i(T_SUBSCRIBER_ID subscriber_id,
                          T_KPD_MISS_EVENT_TYPE missevent_type);


/**
 * function:  kpd_process_timeout_event_i
 *
 * Function will send a message to all subscribed clients
 * that a timeout event has occurred
 * 0  - No key pressed
 * XX - Key value of previously pressed key
 *
 * @param presse_state
 *
 */
void kpd_process_timeout_event_i(void);

/**
 * function:  kpd_process_miss_event_i
 *
 * Function will send a message to all subscribed clients
 * that a miss event has occurred
 *
 */
void kpd_process_miss_event_i(void);

/**
 * function:  kpd_process_key_event_i
 * 
 * Function will process the keys pressed and send message
 * to subscribed clients
 *
 * @param   nb_keys_pressed   The number of keys pressed
 * @param   key_pressed_id    pointer to an array containing
 *                            the id's of the keys pressed
 */
void kpd_process_key_event_i(UINT8 nb_keys_pressed, INT8* key_pressed_id);

/**
 * function:  kpd_process_long_key_i
 * 
 * Function will process the Long and repeat keys and send message
 * to subscribed clients
 *
 * @param   nb_keys_pressed   The number of keys pressed
 * @param   key_pressed_id    pointer to an array containing
 *                            the id's of the keys pressed
 */
void kpd_process_long_key_i(UINT8 nb_keys_pressed, INT8* key_pressed_id);

/**
 * function: kpd_send_timer_changed_msg
 *
 * Function will send a msg to all client that are subscribed
 * to timer changes
 *
 * @param timer_type      the timer that has changed
 *
 */
void kpd_send_timer_changed_msg(T_KPD_TIMER_TYPE timer_type);

/**
 * function: kpd_clr_subscriber_table
 *
 * function will clear the subscriber info table
 */
void kpd_clr_subscriber_table(void);

#endif /* (CHIPSET == 12) */

/*@}*/
#endif /* #ifndef _KPD_PROCESS_INTERNAL_MSG_H_ */
