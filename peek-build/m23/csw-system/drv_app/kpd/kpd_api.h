/**
 * @file   kpd_api.h
 *
 * API Definition for keypad driver.
 *
 * This file gathers all the constants, structure and functions declaration
 * useful for a keypad driver user.
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

#ifndef _KPD_API_H_
#define _KPD_API_H_

#include "kpd/kpd_cfg.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

/**
 * @name External types
 *
 * Types used in API.
 *
 */
/*@{*/

/** Definition of the subscriber identification. */
typedef void* T_KPD_SUBSCRIBER;

/** Definition of the virtual key identification. */
typedef UINT8 T_KPD_VIRTUAL_KEY_ID;

/** Definition of the notification level (First press, long press, infinite repeat, release). */
typedef UINT8 T_KPD_NOTIF_LEVEL;

/** Definition of a set of keys. */
typedef struct {  UINT8                nb_notified_keys;
                  T_KPD_VIRTUAL_KEY_ID notified_keys[KPD_NB_PHYSICAL_KEYS];
               } T_KPD_VIRTUAL_KEY_TABLE;


#if ((CHIPSET == 12) || (CHIPSET == 15))

/** Definition of the timer changed indentification */
typedef UINT8 T_KPD_TIMER_TYPE;

/** Definition of the timeout indentification */
typedef UINT8 T_KPD_TIMEOUT_TYPE;

/** Definition of the miss event indentification */
typedef UINT8 T_KPD_MISS_EVENT_TYPE;

#endif /*(CHIPSET == 12) */

/*@}*/


/** Allowed values for T_KPD_NOTIF_LEVEL type.
 * If a key is defined with KPD_NO_NOTIF, this key will be deleted from the key list
 * notified to the client.
 * But client will can set later as KPD_RELEASE_NOTIF (for exemple)
 * without calling unsubscribe, subscribe functions.
 */
#define KPD_NO_NOTIF                (0x00)

/** Allowed values for T_KPD_NOTIF_LEVEL type.
 * If a key is defined with KPD_FIRST_PRESS_NOTIF, client will be notified by :
 *    - The immediate key press
 */
#define KPD_FIRST_PRESS_NOTIF       (0x01)

/** Allowed values for T_KPD_NOTIF_LEVEL type.
 * If a key is defined with KPD_LONG_PRESS, client will be notified by :
 *    - The long press if the key is still pressed for a defined time
 *      (defined in kpd_define_repeat_keys function)
 */
#define KPD_LONG_PRESS_NOTIF        (0x02)

/** Allowed values for T_KPD_NOTIF_LEVEL type.
 * If a key is defined with KPD_INFINITE_REPEAT_NOTIF, client will be notified by :
 *    - The long press if the key is still pressed for a defined time
 *      (defined in kpd_define_repeat_keys function)
 *    - The key pressed every defined time (defined in kpd_define_repeat_keys function),
 *      until the key is released
 */
#define KPD_INFINITE_REPEAT_NOTIF   (0x04)

/** Allowed values for T_KPD_NOTIF_LEVEL type.
 * If a key is defined with KPD_RELEASE_NOTIF, client will be notified by :
 *    - the key release
 */
#define KPD_RELEASE_NOTIF           (0x08)


#if ((CHIPSET == 12) || (CHIPSET == 15))

/** Allowed values for T_KPD_TIMER_TYPE
 *  This values is to be used if client should be notified
 *  if the Long Press Timer is changed
 */
#define KPD_TIMER_LONG_PRESS        (0x01)

/** Allowed values for T_KPD_TIMER_TYPE
 *  This values is to be used if client should be notified
 *  if the Repeat Timer is changed
 */
#define KPD_TIMER_REPEAT            (0x02)

/** Allowed values for T_KPD_TIMER_TYPE
 *  This values is to be used if client should want to cancel
 *  the notification of all subscribed timer change events
 */
#define KPD_TIMER_NONE              (0x00)


/** Allowed values for T_KPD_TIMEOUT_TYPE
 *  This values is to be used if client should be notified
 *  if a time out has occured for the NO KEY TIMEOUT event
 */
#define KPD_TIMEOUT_NO_KEY          (0x01)

/** Allowed values for T_KPD_TIMEOUT_TYPE
 *  This values is to be used if client should be notified
 *  if a time out has occured for the KEY JAM event
 */
#define KPD_TIMEOUT_KEY_JAM         (0x02)

/** Allowed values for T_KPD_TIMEOUT_TYPE
 *  This values is to be used if client should want to cancel
 *  the notification of all subscribed TIMEOUT events
 */
#define KPD_TIMEOUT_NONE            (0x00)

/** Allowed values for T_KPD_MISS_EVENT_TYPE
 *  This values is to be used if client should be notified
 *  if a Miss Event has occured
 */
#define KPD_MISS_EVENT_ON           (0x01)

/** Allowed values for T_KPD_MISS_EVENT_TYPE
 *  This values is to be used if client should want to cancel
 *  the notification of the MISS EVENT
 */
#define KPD_MISS_EVENT_NONE         (0x00)

#endif /* (CHIPSET == 12) */


/*************************************************************************/
/************************** FUNCTIONS PROTOTYPES *************************/
/*************************************************************************/

/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

/**
 * function: kpd_subscribe
 *
 * This function is called by the client before any use of the keypad driver services
 * It is called only once.
 *
 * @param   subscriber_p      Subscriber identification value (OUT).
 * @param   mode              Mode used by the keypad client.
 * @param   notified_keys_p   Define all the keys the client want to be notified.
 * @param   return_path         Return path for key pressed.
 * @return
 *    - RV_OK if operation is successfull,
 *    - RV_INTERNAL_ERR if
 *          - the max of subscriber is reached,
 *          - the software entity is not started, not yet initialized or initialization has
 *            failed
 *    - RV_INVALID_PARAMETER if number of virtual keys is not correct.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * Message returned: KPD_STATUS_MSG with operation = KPD_SUBSCRIBE_OP.
 *                   Available values for status_value are:
 *                   - KPD_PROCESS_OK if asynchronous operation is successfull,
 *                   - KPD_ERR_KEYS_TABLE if at least one key is not available in the requested mode,
 *                   - KPD_ERR_RETURN_PATH_EXISTING if subscriber return path is already defined by
 *                      another subscriber,
 *                   - KPD_ERR_INTERNAL if an internal error occured.
 *
 * @note
 *       - If number of notified key is KPD_NB_PHYSICAL_KEYS, client has not to fulfill
 *         the structure, this will be automatically done by the software entity.
 */
T_RV_RET kpd_subscribe(T_KPD_SUBSCRIBER* subscriber_p,
                       T_KPD_MODE mode,
                       T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                       T_RV_RETURN return_path);

/**
 * function: kpd_unsubscribe
 *
 * This function unsubscribes a client from the keypad driver.
 *
 * @param   subscriber_p  Subscriber identification value (IN/OUT).
 * @return
 *    - RV_OK if operation is successfull,
 *    - RV_INVALID_PARAMETER if subscriber identification is incorrect.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 *    - Message:  No message is returned for asynchronous processing.
 */
T_RV_RET kpd_unsubscribe( T_KPD_SUBSCRIBER* subscriber_p);

/**
 * function: kpd_define_key_notification
 *
 * This function defines notification type for a set of keys.
 * By default, all the keys are defined as KPD_RELEASE_NOTIF.
 * It's not mandatory that all the key defined in the notif_key_table be
 * notified to the subscriber. If at least one key is set in this table but
 * is not notified to the subscriber, this will have no effect.
 *
 * @param   subscriber           Subscriber identification value.
 * @param   notif_key_table_p    Set of keys for level notification definition.
 * @param   notif_level          Define level of notification is set for all the keys.
 * @param   long_press_time      Time in tenth of seconds before long press time notification (>0).
 * @param   repeat_time          Time in tenth of seconds for key repetition (>0).
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INVALID_PARAMETER if :
 *       - subscriber identification is incorrect,
 *       - number of virtual keys is incorrect,
 *       - long_press_time = 0 and repeat_level = KPD_LONG_PRESS_NOTIF or KPD_INFINITE_REPEAT_NOTIF,
 *       - repeat_time = 0 and repeat_level = KPD_INFINITE_REPEAT_NOTIF.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * Message returned: KPD_STATUS_MSG with operation = KPD_REPEAT_KEYS_OP.
 *                   Available values for status_value are:
 *                   - KPD_PROCESS_OK if asynchronous operation is successfull,
 *                   - KPD_ERR_KEYS_TABLE if at least one key is not available in the subscriber mode.
 *
 * @note - Values for long_press_time and repeat_time are available for the subscriber but
 *          for all the keys defined in repeat mode. So, if a subscriber call the function
 *          twice with different values of long_press_time and repeat_time, only the latest
 *          values will be taken into account.
 *       - If number of notified key is KPD_NB_PHYSICAL_KEYS, client has not to fulfill
 *          the structure, this will be automatically done by the software entity.
 *       - If the client set a key to KPD_INFINITE_REPEAT_NOTIF, it will be notified of
 *          the long key pressed and the repeat press.
 */
T_RV_RET kpd_define_key_notification(T_KPD_SUBSCRIBER subscriber,
                                     T_KPD_VIRTUAL_KEY_TABLE* notif_key_table_p,
                                     T_KPD_NOTIF_LEVEL notif_level,
                                     UINT16 long_press_time,
                                     UINT16 repeat_time);

/**
 * function: kpd_change_mode
 *
 * This function changes the mode for the specific client.
 *
 * @param   subscriber        Subscriber identification value.
 * @param   notified_keys_p   Define all the keys the client want to be notified in the new mode.
 * @param   new_mode          New mode in which the client want to switch.
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INVALID_PARAMETER if :
 *       - subscriber identification is incorrect,
 *       - number of virtual keys is incorrect.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * Message returned: KPD_STATUS_MSG with operation = KPD_CHANGE_MODE_OP.
 *                   Available values for status_value are:
 *                   - KPD_PROCESS_OK if asynchronous operation is successfull,
 *                   - KPD_ERR_KEYS_TABLE if at least one key is not available in the new requested mode,
 *
 * @note - Call to this function cancel, for the subscriber, all the repeat mode defined
 *          for the all thekeys with the function kpd_define_repeat_keys.
 *       - If the subscriber was the owner of the keypad, this privilege is cancelled and
 *          keypad is set in multi-notified mode.
 *       - If RV_INVALID_PARAMETER is returned, the current mode for the subscriber is
 *          the old mode and the subscriber is still the keypad owner if it was.
 *       - If number of notified key is KPD_NB_PHYSICAL_KEYS, client has not to fulfill
 *          the structure, this will be automatically done by the software entity.
 */
T_RV_RET kpd_change_mode( T_KPD_SUBSCRIBER subscriber,
                          T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                          T_KPD_MODE new_mode);

/**
 * function: kpd_own_keypad
 *
 * This function allows a subscriber being the only client to be notified by action
 * on keypad (less CPU time used).
 * After this call, the keypad is in the "single notified" state.
 * This action is cancelled when:
 * -  The function is called with parameter is_keypad_owner to FALSE,
 * -  The subscriber (keypad owner) unsubscribe from keypad,
 * -  The subscriber (keypad owner) changes its mode.
 *
 * Note that keypad is in the "multi notified" state if there is no subscriber (particularly
 * at the keypad initialisation).
 *
 * @param   subscriber        Subscriber identification value.
 * @param   is_keypad_owner   Define the state to change.
 *                            TRUE: keypad pass in "single notified" state
 *                            FALSE:  keypad pass in "multi notified" state
 * @param   keys_owner_p      Set of keys only notified to the subscriber that call this function.
 *                            This is mandatory a subset of the keys defined at subscription.
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INVALID_PARAMETER if :
 *       - subscriber identification is incorrect,
 *       - number of virtual keys is incorrect.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * Message returned: KPD_STATUS_MSG with operation = KPD_OWN_KEYPAD_OP.
 *                   Available values for status_value are:
 *                   - KPD_PROCESS_OK if asynchronous operation is successfull,
 *                   - KPD_ERR_KEYS_TABLE if at least one key is not defined in the subscriber mode,
 *                   - KPD_ERR_SN_MODE if keypad driver is already in SN mode,
 *                   - KPD_ERR_ID_OWNER_KEYPAD if the subscriber try to remove own keypad privilege
 *                      whereas it is not the keypad owner.
 */
T_RV_RET kpd_own_keypad( T_KPD_SUBSCRIBER subscriber,
                         BOOL is_keypad_owner,
                         T_KPD_VIRTUAL_KEY_TABLE* keys_owner_p);

/**
 * function: kpd_set_key_config
 *
 * This function allows setting dynamically a configuration for new or existing virtual keys.
 * The two tables define a mapping between each entry (new_keys[1] is mapped with reference_keys[1],
 * new_keys[2] is mapped with reference_keys[2], ...).
 * The call of this function doesn't change the mode of the client.
 *
 * @param   subscriber        Subscriber identification value.
 * @param   reference_keys_p  Set of keys available on keypad in default mode.
 * @param   new_keys_p        Set of keys which must map with the reference keys.
 * @return
 *    - RV_OK if operation is successfull,
 *    - RV_INVALID_PARAMETER if :
 *       - subscriber identification is incorrect,
 *       - at least one reference key is not defined in the default mode,
 *       - number of virtual keys is incorrect (in reference keys or new keys table,
 *    - RV_NOT_SUPPORTED if configurable mode is not supported.
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * Message returned: KPD_STATUS_MSG with operation = KPD_SET_CONFIG_MODE_OP.
 *                   Available values for status_value are:
 *                   - KPD_PROCESS_OK if asynchronous operation is successfull,
 *                   - KPD_ERR_CONFIG_MODE_USED if config mode is used by some subscribers.
 */
T_RV_RET kpd_set_key_config(T_KPD_SUBSCRIBER subscriber,
                            T_KPD_VIRTUAL_KEY_TABLE* reference_keys_p,
                            T_KPD_VIRTUAL_KEY_TABLE* new_keys_p);

/**
 * function: kpd_get_available_keys
 *
 * This function allows knowing all the available keys in default mode.
 *
 * @param   available_keys_p Set of keys available on keypad in default mode. The structure
 *                            must be declared by the caller, and is filled by the function (OUT).
 * @return  RV_OK.
 */
T_RV_RET kpd_get_available_keys( T_KPD_VIRTUAL_KEY_TABLE* available_keys_p);

/**
 * function: kpd_get_ascii_key_code
 *
 * This function return associated ASCII value to defined key.
 *
 * @param   key         Key identification value.
 * @param   mode        Mode in which is defined the link between "key" and "ascii code".
 * @param   ascii_code  Associated ASCII code to parameter "key" (OUT).
 * @return  
 *    - RV_OK if operation is successfull,
 *    - RV_INVALID_PARAMETER if :
 *       - mode is different of KPD_DEFAULT_MODE or KPD_ALPHANUMERIC_MODE,
 *       - the key doesn't exist in the defined mode.
 *
 * @note If return value is RV_INVALID_PARAMETER, empty string is set in ascii_code_pp variable.
 */
T_RV_RET kpd_get_ascii_key_code( T_KPD_VIRTUAL_KEY_ID key,
                                 T_KPD_MODE mode,
                                 char** ascii_code_pp);

/**
 * function: KP_Init
 *
 * This function is defined for backward compatibility with Condat.
 * It register two functions which notify Condat that Power key is long pressed.
 * It is used by PWR SWE.
 *
 * @param   pressed     Callback function to notify that Power key is long pressed.
 * @param   released    Callback function to notify that Power key is released.
 */
void KP_Init( void(pressed(T_KPD_VIRTUAL_KEY_ID)), void(released(void)) );



#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * function: kpd_register_for_timerchanges
 *
 * This function allows a subscriber to register it self to be called back when 
 * the actual time value of the specified timer type(s) are changed.
 *
 * Each time a subscriber calls the function kpd_define_key_notification the 
 * passed timer values for long_press_time and repeat_time becomes the values 
 * that the keypad driver uses from that moment on. Subscribers can register 
 * them self for notification when another subscriber changes one or more of 
 * these time values.
 *
 * @param subscriber  Subscriber identification value
 * @param timer_type  Logical OR of possible timer types
 * @return
 *    - RV_OK           The API function was succeessfully executed
 *    - RV_INVALID_PARAMETER if:
 *       - Subscriber identification is incorrect
 *       - Timer type is incorrect
 *    - RV_MEMORY_ERR   Memory reaches its size limit
 *
 * Message returned: KPD_TIMERS_CHANGED_MSG
 *                   Available values for timer_type are:
 *                   - KPD_TIMER_LONG_PRESS
 *                   - KPD_TIMER_REPEAT
 *
 * @note    This function is only available when the keypad entity is configured 
 *          for hardware decoding.
 */
T_RV_RET kpd_register_for_timerchanges( T_KPD_SUBSCRIBER subscriber,
                                        T_KPD_TIMER_TYPE timer_type);


/**
 * function: kpd_register_for_keypad_timeout
 *
 * This function allows a subscriber to register it self to be called back when 
 * a keypad timeout has oc-curred.
 *
 * A keypad timeout occurs when a long time of inactivity of the keypad is detected. 
 * This can be while no key is pressed, or when one or more keys are pressed.
 *
 * @param subscriber    Subscriber identification value
 * @param timeout_type  Timeout types to register for (Logical OR)
 * @param timeout_time  Timeout time in msec  
 * @return
 *    - RV_OK             The API function was succeessfully executed
 *    - RV_INVALID_PARAMETER if:
 *       - Subscriber identification is incorrect
 *       - Timeout type is incorrect
 *    - RV_MEMORY_ERR     Memory reaches its size limit
 *    - RV_NOT_SUPPORTED  Not allowed in current configuration
 *
 * Message returned: KPD_KEYPAD_TIMEOUT_MSG
 *                   Available values for timeout_type are:
 *                   - KPD_TIMEOUT_NO_KEY
 *                   - KPD_TIMEOUT_KEY_JAM
 *
 * @note    This function is only available when the keypad entity is configured 
 *          for hardware decoding.
 */
T_RV_RET kpd_register_for_keypad_timeout( T_KPD_SUBSCRIBER    subscriber,
                                          T_KPD_TIMEOUT_TYPE  timeout_type,
                                          UINT16 timeout_time);

/**
 * function: kpd_register_for_miss_event
 *
 * This function allows a subscriber to register it self to be called back when 
 * a keypad miss event has been detected.
 *
 * A keypad miss event occurs when two events are waiting for service and the 
 * third event occurs. This third event can not be served and will be lost.
 *
 * @param subscriber      Subscriber identification value
 * @param missevent_type  Miss event type to register for
 * @return
 *    - RV_OK             The API function was succeessfully executed
 *    - RV_INVALID_PARAMETER if:
 *       - Subscriber identification is incorrect
 *       - Missevent type is incorrect
 *    - RV_MEMORY_ERR     Memory reaches its size limit
 *
 * Message returned: KPD_KEYPAD_MISS_EVENT_MSG
 *                   Available values for missevent_type are:
 *                   - KPD_MISS_EVENT_ON
 *                   - KPD_MISS_EVENT_NONE
 *
 * @note    This function is only available when the keypad entity is configured 
 *          for hardware decoding.
 */
T_RV_RET kpd_register_for_miss_event( T_KPD_SUBSCRIBER subscriber,
                                      T_KPD_MISS_EVENT_TYPE missevent_type);



#endif /* (CHIPSET == 12) */


/*@}*/


/*************************************************************************/
/************************** MESSAGES DEFINITION **************************/
/*************************************************************************/

/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define KPD_MESSAGES_OFFSET      (0x36 << 10)

/*
To keep track of the used message ID's all used id are placed
in this comment block.

Used message ID's

KPD_KEY_EVENT_MSG                       0x001   external
KPD_STATUS_MSG                          0x002   external
KPD_SUBSCRIBE_MSG                       0x003   internal
KPD_UNSUBSCRIBE_MSG                     0x004   internal
KPD_NOTIF_KEYS_MSG                      0x005   internal
KPD_CHANGE_MODE_MSG                     0x006   internal
KPD_OWN_KEYPAD_MSG                      0x007   internal
KPD_SET_CONFIG_MODE_MSG                 0x008   internal
T_KPD_KEY_PRESSED_MSG                   0x009   internal
-- CHIPSET 12 --
KPD_TIMER_CHANGED_MSG                   0x00A   external
KPD_KEYPAD_TIMEOUT_MSG                  0x00B   external
KPD_KEYPAD_MISS_EVENT_MSG               0x00C   external
KPD_REGISTER_FOR_TIMERCHANGES_MSG       0x00D   internal
KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG     0x00E   internal
KPD_REGISTER_FOR_MISS_EVENT_MSG         0x00F   internal
KPD_MISS_EVENT_INTERRUPT_MSG            0x010   internal
KPD_TIMEOUT_INTERRUPT_MSG               0x011   internal
KPD_LONG_KEY_INTERRUPT_MSG              0x012   internal
KPD_KEY_EVENT_INTERRUPT_MSG             0x013   internal
*/



/**
 * @name KPD_KEY_EVENT_MSG
 *
 * This message is sent to a subscriber when a key is pressed or released.
 *
 * Message issued by KPD to a subscriber.
 */
/*@{*/

/** Definition of the key state (pressed or released). */
typedef UINT8 T_KPD_KEY_STATE;

/** Definition of the key press state (first press, long press, repeat press). */
typedef UINT8 T_KPD_PRESS_STATE;


/** Information sent to a client for a key notification. */
typedef struct {  T_KPD_VIRTUAL_KEY_ID virtual_key_id;
                  T_KPD_KEY_STATE      state;
                  T_KPD_PRESS_STATE    press_state;
                  char*                ascii_value_p;
               } T_KPD_KEY_INFO;

/** Allowed values for T_KPD_KEY_STATE type. */
#define KPD_KEY_PRESSED          (0)
/** Allowed values for T_KPD_KEY_STATE type. */
#define KPD_KEY_RELEASED         (1)

/** Allowed value for T_KPD_PRESS_STATE type. */
#define KPD_FIRST_PRESS          (0)
/** Allowed value for T_KPD_PRESS_STATE type. */
#define KPD_LONG_PRESS           (1)
/** Allowed value for T_KPD_PRESS_STATE type. */
#define KPD_REPEAT_PRESS         (2)
/** Allowed value for T_KPD_PRESS_STATE type (when state==KPD_KEY_RELEASED). */
#define KPD_INSIGNIFICANT_VALUE  (0xff)

/** Message ID. */
#define KPD_KEY_EVENT_MSG (KPD_MESSAGES_OFFSET | 0x001)



/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR    hdr;

   /** Informations about key event. */
   T_KPD_KEY_INFO key_info;

}  T_KPD_KEY_EVENT_MSG;
/*@}*/


/**
 * @name KPD_STATUS_MSG
 *
 * Status message.
 *
 * Message issued by KPD to a subscriber.
 * This message is used to return the status of an asynchronous process
 * requested by a subscriber.
 */
/*@{*/

/* Allowed values for 'operation" field */
#define KPD_SUBSCRIBE_OP        1
#define KPD_REPEAT_KEYS_OP      2
#define KPD_CHANGE_MODE_OP      3
#define KPD_OWN_KEYPAD_OP       4
#define KPD_SET_CONFIG_MODE_OP  5

/* Available values for "status_value" field */
/* This define value is set when asynchronous process is successfull. */
#define KPD_PROCESS_OK 1
/* This define value is set in a status message (KPD_STATUS_MSG) when a client try to
   subscribe whereas keypad is in single-notified mode. Subscription
   is so rejected. */
#define KPD_ERR_SN_MODE 2
/* This defined value is set in a status message (KPD_STATUS_MSG) when a client try to
   modify configuration mode whereas this mode is already used. */
#define KPD_ERR_CONFIG_MODE_USED 3
/* This defined value is set in a status message (KPD_STATUS_MSG) when a client try to
   change keypad state from single-notified to multi-notified whereas it is not
   the keypad owner. */
#define KPD_ERR_ID_OWNER_KEYPAD 4
/* This defined value is set in a status message (KPD_STATUS_MSG) when a client
   defines a key table which is not correct. */
#define KPD_ERR_KEYS_TABLE 5
/* This defined value is set in a status message (KPD_STATUS_MSG) when a client
   try to subscribe to the keypad driver with a return path which is already
   defined by another subscriber. */
#define KPD_ERR_RETURN_PATH_EXISTING 6
/* This defined value is set in a status message (KPD_STATUS_MSG) when an internal
   error cause the failure of the process. */
#define KPD_ERR_INTERNAL 7


/** Message ID. */
#define KPD_STATUS_MSG (KPD_MESSAGES_OFFSET | 0x002)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR hdr;

   /** Operation. */
   UINT8    operation;

   /** Return status value. */
   UINT8    status_value;

}  T_KPD_STATUS_MSG;
/*@}*/


#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * @name KPD_TIMER_CHANGED_MSG
 *
 * This message is sent to a subscriber when a timer value
 * has changed (long press timer and/or repeat timer)
 *
 * Message issued by KPD to a subscriber.
 */
/*@{*/

/** Message ID. */
#define KPD_TIMER_CHANGED_MSG (KPD_MESSAGES_OFFSET | 0x00A)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR hdr;

   /** Return timer type. */
   T_KPD_TIMER_TYPE    timer_type;

}  T_KPD_TIMER_CHANGED_MSG;
/*@}*/


/**
 * @name KPD_KEYPAD_TIMEOUT_MSG
 *
 * This message is sent to a subscriber when a timeout in the
 * keyboard has occured
 *
 * Message issued by KPD to a subscriber.
 */
/*@{*/

/** Message ID. */
#define KPD_KEYPAD_TIMEOUT_MSG (KPD_MESSAGES_OFFSET | 0x00B)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR hdr;

   /** Return timeout type. */
   T_KPD_TIMEOUT_TYPE    timeout_type;

}  T_KPD_KEYPAD_TIMEOUT_MSG;
/*@}*/


/**
 * @name KPD_KEYPAD_MISS_EVENT_MSG
 *
 * This message is sent to a subscriber when a miss event has
 * occured in the keyboard
 *
 * Message issued by KPD to a subscriber.
 */
/*@{*/

/** Message ID. */
#define KPD_KEYPAD_MISS_EVENT_MSG (KPD_MESSAGES_OFFSET | 0x00C)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR hdr;

}  T_KPD_KEYPAD_MISS_EVENT_MSG;

/*@}*/

/**
 * function: kpd_retrieve_key_status
 *
 * This function allows application to check the status of a key (whether pressed or released).This API has been 
 * newly added to fix KPD release interrupt miss issue (OMAPS00093237).
 
 * @param   key_id      Key identification value.
 * @param   mode        Mode in which is defined the link between "key" and "ascii code".
 * @param   state       This will habe the status of the key (KPD_KEY_RELEASED or KPD_KEY_PRESSED).
 * @return  
 *    - RV_OK if operation is successfull,
 *    - RV_INVALID_PARAMETER if :
 *       - mode is different of KPD_DEFAULT_MODE or KPD_ALPHANUMERIC_MODE,
 *       - the key doesn't exist in the defined mode.

 *
 * @note    This function is only available when the keypad entity is configured 
 *          for hardware decoding.
 */
T_RV_RET kpd_retrieve_key_status(  T_KPD_VIRTUAL_KEY_ID key_id,
                                             T_KPD_MODE mode,
                                             T_KPD_KEY_STATE* state);


#endif /* (CHIPSET == 12) || (CHIPSET==15)*/

#endif /* #ifndef _KPD_API_H_ */
