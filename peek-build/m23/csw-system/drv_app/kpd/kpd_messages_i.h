/**
 * @file   kpd_messages_i.h
 *
 * Data structures that KPD SWE can receive.
 *
 * These messages are send by the bridge function. There are not available
 * out of the SWE.
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

#ifndef _KPD_MESSAGES_I_H_
#define _KPD_MESSAGES_I_H_

#include "kpd/kpd_api.h"
#include "kpd/kpd_i.h"
#include "kpd/kpd_messages_i.h"

#include "rv/rv_general.h"


/* Strusture used to send a subscription request */
typedef struct 
{
   T_SUBSCRIBER_ID subscriber_id;
   T_KPD_MODE mode;
   T_KPD_VIRTUAL_KEY_TABLE notified_keys;
   T_RV_RETURN return_path;

}  T_SUBSCRIPTION_INFO;


/**
 * @name KPD_SUBSCRIBE_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to process a subscription.
 */
/*@{*/
/** Message ID. */
#define KPD_SUBSCRIBE_MSG (KPD_MESSAGES_OFFSET | 0x003)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Informations for subscription. */
   T_SUBSCRIPTION_INFO  subscription_info;

}  T_KPD_SUBSCRIBE_MSG;
/*@}*/

/**
 * @name KPD_UNSUBSCRIBE_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to process an unsubscription.
 */
/*@{*/
/** Message ID. */
#define KPD_UNSUBSCRIBE_MSG (KPD_MESSAGES_OFFSET | 0x004)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Subscriber Id. */
   T_SUBSCRIBER_ID  subscriber_id;

}  T_KPD_UNSUBSCRIBE_MSG;
/*@}*/

/**
 * @name KPD_NOTIF_KEYS_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to store a notification key level .
 */
/*@{*/
/** Message ID. */
#define KPD_NOTIF_KEYS_MSG (KPD_MESSAGES_OFFSET | 0x005)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /* Keys informations */
   T_SUBSCRIBER_ID         subscriber_id;
   T_KPD_VIRTUAL_KEY_TABLE notif_key_table;
   T_KPD_NOTIF_LEVEL       notif_level;
   UINT16                  long_press_time;
   UINT16                  repeat_time;

}  T_KPD_NOTIF_KEYS_MSG;
/*@}*/

/**
 * @name KPD_CHANGE_MODE_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to change the mode for a subscriber.
 */
/*@{*/
/** Message ID. */
#define KPD_CHANGE_MODE_MSG (KPD_MESSAGES_OFFSET | 0x006)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /* Change mode informations */
   T_SUBSCRIBER_ID         subscriber_id;
   T_KPD_VIRTUAL_KEY_TABLE notified_keys;
   T_KPD_MODE              new_mode;

}  T_KPD_CHANGE_MODE_MSG;
/*@}*/

/**
 * @name KPD_OWN_KEYPAD_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to define the subscriber as the owner of the keypad.
 */
/*@{*/
/** Message ID. */
#define KPD_OWN_KEYPAD_MSG (KPD_MESSAGES_OFFSET | 0x007)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /* Own keypad informations */
   T_SUBSCRIBER_ID         subscriber_id;
   BOOL                    is_keypad_owner;
   T_KPD_VIRTUAL_KEY_TABLE keys_owner;

}  T_KPD_OWN_KEYPAD_MSG;


/*@}*/

/**
 * @name KPD_SET_CONFIG_MODE_MSG
 *
 * Internal message.
 *
 * Message issued by KPD to KPD task.
 * This message is used to define the configurable mode with a set of keys.
 */
/*@{*/
/** Message ID. */
#define KPD_SET_CONFIG_MODE_MSG (KPD_MESSAGES_OFFSET | 0x008)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /* New config mode informations */
   T_SUBSCRIBER_ID         subscriber_id;
   T_KPD_VIRTUAL_KEY_TABLE reference_keys;
   T_KPD_VIRTUAL_KEY_TABLE new_keys;

}  T_KPD_SET_CONFIG_MODE_MSG;
/*@}*/

/**
 * @name T_KPD_KEY_PRESSED_MSG
 *
 * Internal message.
 *
 * Message issued by HISR to KPD task.
 * This message is used by the Keypad HISR to inform the keypad task
 * that a key is pressed.
 */
/*@{*/
/** Message ID. */
#define KPD_KEY_PRESSED_MSG (KPD_MESSAGES_OFFSET | 0x009)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /* Physical key pressed Id, or KPD_PKEY_NULL if key is released.
      Note that this variable is used only for PC Riviera Tool, but is
      not in flag compilation for forward compatibility (Calypso+, IT 
      when key is pressed and released) */
   T_KPD_PHYSICAL_KEY_ID value;

   /* Define if key must be processd or not.
      This field is used in order to limit memory use,
      and allow keypad task to send messages to subscribers */
   BOOL key_to_process;

}  T_KPD_KEY_PRESSED_MSG;
/*@}*/

#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * @name T_KPD_REGISTER_FOR_TIMERCHANGES_MSG
 *
 * Internal message.
 *
 * .Message issued by API function: kpd_register_for_timerchanges
 */
/*@{*/
/** Message ID. */
#define KPD_REGISTER_FOR_TIMERCHANGES_MSG (KPD_MESSAGES_OFFSET | 0x00D)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Subscriber ID */
   T_SUBSCRIBER_ID subscriber_id;

   /** Timer changed type */
   T_KPD_TIMER_TYPE timer_type;

}  T_KPD_REGISTER_FOR_TIMERCHANGES_MSG;
/*@}*/


/**
 * @name T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG
 *
 * Internal message.
 *
 * .Message issued by API function: kpd_register_for_keypad_timeout
 */
/*@{*/
/** Message ID. */
#define KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG (KPD_MESSAGES_OFFSET | 0x00E)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Subscriber ID */
   T_SUBSCRIBER_ID    subscriber_id;

   /** Timeout type */
   T_KPD_TIMEOUT_TYPE  timeout_type;

   /** Timeout time */
   UINT16 timeout_time;

}  T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG;
/*@}*/


/**
 * @name T_KPD_REGISTER_FOR_MISS_EVENT_MSG
 *
 * Internal message.
 *
 * Message issued by API function: kpd_register_for_miss_event
 */
/*@{*/
/** Message ID. */
#define KPD_REGISTER_FOR_MISS_EVENT_MSG (KPD_MESSAGES_OFFSET | 0x00F)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Subscriber ID */
   T_SUBSCRIBER_ID    subscriber_id;

   /** Miss event type */
   T_KPD_MISS_EVENT_TYPE missevent_type;
 

}  T_KPD_REGISTER_FOR_MISS_EVENT_MSG;
/*@}*/


/**
 * @name T_KPD_GENERIC_INTERRUPT_MSG
 *
 * Internal message.
 *
 * Message issued by HISR function
 */
/*@{*/
/** Message can be used with the following Message ID. */
#define KPD_MISS_EVENT_INTERRUPT_MSG    (KPD_MESSAGES_OFFSET | 0x010)
#define KPD_TIMEOUT_INTERRUPT_MSG       (KPD_MESSAGES_OFFSET | 0x011)


/** Message structure. */
typedef struct
{
   /** Message header. */
   T_RV_HDR         hdr; 

} T_KPD_GENERIC_INTERRUPT_MSG;

/*@}*/

/**
 * @name T_KPD_KEY_EVENT_INTERRUPT_MSG
 *
 * Internal message.
 *
 * Message issued by HISR function
 */
/*@{*/
/** Message can be used with the following Message ID. */
#define KPD_LONG_KEY_INTERRUPT_MSG      (KPD_MESSAGES_OFFSET | 0x012)
#define KPD_KEY_EVENT_INTERRUPT_MSG     (KPD_MESSAGES_OFFSET | 0x013)

/** Message structure. */
typedef struct
{
   /** Message header. */
   T_RV_HDR         hdr;
   UINT16           full_code_15_0;
   UINT16           full_code_31_16;
   UINT16           full_code_47_32;
   UINT16           full_code_63_48;
} T_KPD_KEY_EVENT_INTERRUPT_MSG;
typedef struct
{
   /** Message header. */
   T_RV_HDR         hdr;
   UINT8           isr_lsb;
   UINT8           kpc_data_byte0;
   UINT8           kpc_data_byte1;
   UINT8           kpc_data_byte2;
} T_KPD_FULL_KEY_EVENT_INTERRUPT_MSG;

#define KPD_PROCESS_POWER_KEY_MSG      (KPD_MESSAGES_OFFSET | 0x014)
#define KPD_KEY_FULL_EVENT_INTERRUPT_MSG     (KPD_MESSAGES_OFFSET | 0x015)
#define KPD_PROCESS_POWER_KEY_PRESS_MSG      (KPD_MESSAGES_OFFSET | 0x016)
#define KPD_PROCESS_POWER_KEY_RELEASE_MSG      (KPD_MESSAGES_OFFSET | 0x017)
/*@}*/

#define KPD_ROT_MSG	(KPD_MESSAGES_OFFSET | 0x18)

#endif /* (CHIPSET == 12) */

#endif /* _KPD_MESSAGES_I_H_ */
