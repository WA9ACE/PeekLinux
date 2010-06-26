/**
 * @file   kpd_handle_msg.c
 *
 * Coding of the kpd_handle_msg function, which is called when the SWE
 * receives a new message.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/12/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "kpd/kpd_messages_i.h"
#include "kpd/kpd_scan_functions.h"
#include "kpd/kpd_process_internal_msg.h"
#include "kpd/kpd_physical_key_def.h"
#include "kpd/kpd_env.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"
#include "l1sw.cfg"

extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;
extern UINT16 kpd_configured_long_key_time;
#define KPD_LONG_TIME_TICKS (kpd_configured_long_key_time/4.6)
#if (L1_RF_KBD_FIX == 1)
extern INT8 present_key_capture[KPD_MAX_DETECTABLE];
extern UINT8 capture_nb_keys;
#endif
/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: kpd_handle_msg
 */


 int kpd_count=0;
T_RV_RET Power_handle_power_key_press(void);

T_RV_RET kpd_handle_msg(T_RV_HDR   *msg_p)
{
   if (msg_p != NULL)
   {
      switch (msg_p->msg_id)
      {
         case KPD_SUBSCRIBE_MSG:
         {
            T_KPD_SUBSCRIBE_MSG* msg_subscribe_p= (T_KPD_SUBSCRIBE_MSG*) msg_p;

            KPD_SEND_TRACE_PARAM("KPD: 'Subscription' message received,Id:",
                                 msg_subscribe_p->subscription_info.subscriber_id,
                                 RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_subscribe_i(msg_subscribe_p->subscription_info.subscriber_id,
                            msg_subscribe_p->subscription_info.mode,
                            &msg_subscribe_p->subscription_info.notified_keys,
                            msg_subscribe_p->subscription_info.return_path);
            break;
         }
         case KPD_UNSUBSCRIBE_MSG:
         {
            T_KPD_UNSUBSCRIBE_MSG* msg_unsubscribe_p= (T_KPD_UNSUBSCRIBE_MSG*) msg_p;

            KPD_SEND_TRACE_PARAM("KPD: 'Unsubscription' message received,Id:",
                                 msg_unsubscribe_p->subscriber_id,
                                 RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_unsubscribe_i(msg_unsubscribe_p->subscriber_id);
            break;
         }
         case KPD_NOTIF_KEYS_MSG:
         {
            T_KPD_NOTIF_KEYS_MSG* msg_notif_keys_p= (T_KPD_NOTIF_KEYS_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Notification keys level' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_define_key_notification_i(msg_notif_keys_p->subscriber_id,
                                          &msg_notif_keys_p->notif_key_table,
                                          msg_notif_keys_p->notif_level,
                                          msg_notif_keys_p->long_press_time,
                                          msg_notif_keys_p->repeat_time);
            break;
         }

         case KPD_CHANGE_MODE_MSG:
         {
            T_KPD_CHANGE_MODE_MSG* msg_change_mode_p= (T_KPD_CHANGE_MODE_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Change mode' message received:",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_change_mode_i(msg_change_mode_p->subscriber_id,
                              &msg_change_mode_p->notified_keys,
                              msg_change_mode_p->new_mode);

            break;
         }
         case KPD_OWN_KEYPAD_MSG:
         {
            T_KPD_OWN_KEYPAD_MSG* msg_own_keypad_p= (T_KPD_OWN_KEYPAD_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Own keypad' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_own_keypad_i(msg_own_keypad_p->subscriber_id,
                             msg_own_keypad_p->is_keypad_owner,
                             &msg_own_keypad_p->keys_owner);

            break;
         }
         case KPD_SET_CONFIG_MODE_MSG:
         {
            T_KPD_SET_CONFIG_MODE_MSG* msg_change_mode_p= (T_KPD_SET_CONFIG_MODE_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Set config mode' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_set_key_config_i(msg_change_mode_p->subscriber_id,
                                 &msg_change_mode_p->reference_keys,
                                 &msg_change_mode_p->new_keys);

            break;
         }
         case KPD_KEY_PRESSED_MSG:
         {
            T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id;
            T_KPD_KEY_PRESSED_MSG* msg_key_pressed= (T_KPD_KEY_PRESSED_MSG*) msg_p;

            KPD_SEND_TRACE_PARAM("KPD: Message key pressed received by keypad task",
                                 msg_key_pressed->value,
                                 RV_TRACE_LEVEL_DEBUG_LOW);
#ifdef _WINDOWS
            /* In case of Riviera Tool test and in order to avoid losing key event,
               key pressed Id is set in the message, and unset only when keypad task
               have read the value */
            kpd_set_key_value(msg_key_pressed->value);
#endif
            /* If there is enough memory to process the key */
            if (msg_key_pressed->key_to_process)
            {
               /* if value = KPD_SHORT_PRESS_PWR_KEY, it's PWR SWE which sent the message,
                  physical key id cannot be retrieved by function kpd_scan_keypad */
               if (msg_key_pressed->value == KPD_SHORT_PRESS_PWR_KEY)
                  physical_key_pressed_id = KPD_SHORT_PRESS_PWR_KEY;
               else
               {

#if ((CHIPSET == 12) || (CHIPSET == 15))
                  kpd_init_ctrl_reg(1, SOFTWARE_MODE_ENABLE, KPD_CLK_DIV32,
                                    KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED,
                                    KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED);
#endif

                  physical_key_pressed_id = kpd_scan_keypad();
               }

               /* The processing is different if keypad is in SN mode and the key is
                  in the table of keypad owner */
               if (kpd_is_key_in_sn_mode(physical_key_pressed_id) == TRUE)
               {
                  kpd_process_key_pressed_sn_mode_i(physical_key_pressed_id);
               }
               else
               {
                  kpd_process_key_pressed_i(physical_key_pressed_id);
               }
            }
            else
            {
               KPD_SEND_TRACE("KPD: Lost key event due to memory lack",
                              RV_TRACE_LEVEL_DEBUG_HIGH);
               kpd_wait_for_key_release();
            }
            break;
         }
#if ((CHIPSET == 12) || (CHIPSET == 15))
         case KPD_REGISTER_FOR_TIMERCHANGES_MSG:
         {
            T_KPD_REGISTER_FOR_TIMERCHANGES_MSG* msg_timerchanges_p= (T_KPD_REGISTER_FOR_TIMERCHANGES_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Register for timerchanges' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_set_timerchanges_i(msg_timerchanges_p->subscriber_id,
                                   msg_timerchanges_p->timer_type);
            break;
         }
         case KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG:
         {
            T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG* msg_keypad_timeout_p= (T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Register for keypad timeout' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_set_keypad_timeout_i(msg_keypad_timeout_p->subscriber_id,
                                     msg_keypad_timeout_p->timeout_type,
                                     msg_keypad_timeout_p->timeout_time);
            break;
         }
         case KPD_REGISTER_FOR_MISS_EVENT_MSG:
         {
            T_KPD_REGISTER_FOR_MISS_EVENT_MSG* msg_miss_event_p= (T_KPD_REGISTER_FOR_MISS_EVENT_MSG*) msg_p;

            KPD_SEND_TRACE("KPD: 'Register for miss events' message received",
                           RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_set_miss_event_i(msg_miss_event_p->subscriber_id,
                                 msg_miss_event_p->missevent_type);
            break;
         }
         case KPD_MISS_EVENT_INTERRUPT_MSG:
         {
            KPD_SEND_TRACE("KPD: 'Miss event interrupt' message received",
                            RV_TRACE_LEVEL_DEBUG_LOW);
            kpd_process_miss_event_i();

            break;
         }
         case KPD_TIMEOUT_INTERRUPT_MSG :
         {
            KPD_SEND_TRACE("KPD: 'Time out interrupt' message received",
                             RV_TRACE_LEVEL_DEBUG_LOW);

            kpd_process_timeout_event_i();

            break;
         }

         case KPD_LONG_KEY_INTERRUPT_MSG :
         {
#if (L1_RF_KBD_FIX == 0)
 			INT8 key_pressed_id[KPD_MAX_DETECTABLE];
            UINT8 nb_keys_pressed;
            UINT8 i;

            KPD_SEND_TRACE("KPD: 'Long key interrupt' message received",
                            RV_TRACE_LEVEL_DEBUG_LOW);
            
            /* Clear key_pressed_id */
            for(i=0; i<KPD_MAX_DETECTABLE; i++)
            {
              key_pressed_id[i] = KPD_PKEY_NULL;
            }

            nb_keys_pressed = kpd_decode_key_registers(msg_p, key_pressed_id);
            kpd_process_long_key_i(nb_keys_pressed, key_pressed_id);
#else
            UINT8 i;
            INT8 key_pressed_id[KPD_MAX_DETECTABLE];
            KPD_SEND_TRACE("KPD: 'Long Key event ' message received",
                            RV_TRACE_LEVEL_ERROR);
            
            /* Clear key_pressed_id */
            for(i=0; i<KPD_MAX_DETECTABLE; i++)
            {
              present_key_capture[i] = key_pressed_id[i]= KPD_PKEY_NULL;
            }

            capture_nb_keys = kpd_decode_key_registers(msg_p, key_pressed_id);
			for (i=0;i<capture_nb_keys;i++)
				present_key_capture[i] = key_pressed_id[i];
            kpd_process_long_key_i(capture_nb_keys, key_pressed_id);
#endif			
            break;
         }
         case KPD_KEY_EVENT_INTERRUPT_MSG :
         {
#if (L1_RF_KBD_FIX == 0)
            INT8 key_pressed_id[KPD_MAX_DETECTABLE];
            UINT8 nb_keys_pressed;
            UINT8 i;

            KPD_SEND_TRACE("KPD: 'Key event interrupt' message received",
                            RV_TRACE_LEVEL_DEBUG_LOW);
            
            /* Clear key_pressed_id */
            for(i=0; i<KPD_MAX_DETECTABLE; i++)
            {
              key_pressed_id[i] = KPD_PKEY_NULL;
            }

            nb_keys_pressed = kpd_decode_key_registers(msg_p, key_pressed_id);

            kpd_process_key_event_i(nb_keys_pressed, key_pressed_id);
#else

          UINT8 i;

            KPD_SEND_TRACE("KPD: 'Key event interrupt' message received",
                            RV_TRACE_LEVEL_ERROR);
            
            /* Clear key_pressed_id */
            for(i=0; i<KPD_MAX_DETECTABLE; i++)
            {
              present_key_capture[i] = KPD_PKEY_NULL;
            }

            capture_nb_keys = kpd_decode_key_registers(msg_p, present_key_capture);
            kpd_process_key_event_i(capture_nb_keys, present_key_capture);
	  	    rvf_stop_timer(RVF_TIMER_0);
			if (capture_nb_keys)
				{
		   	    rvf_start_timer(RVF_TIMER_0,(UINT32) KPD_LONG_TIME_TICKS,TRUE);
	            KPD_SEND_TRACE("KPD: 'starting the timer again",
                            RV_TRACE_LEVEL_DEBUG_HIGH);
				}
#endif			
            break;
         }

#endif /* (CHIPSET == 12) */
#if (CHIPSET==15)
	case KPD_PROCESS_POWER_KEY_MSG:
	 	{
		Power_handle_power_key_press();
		kpd_count++;

/*
		int i=0;
		for (i=0;i<8;i++)
	              rvf_delay(convert_nanosec_to_cycles((20)*4.65*1000));
*/
		break;
		}
#endif
         default:
         {
            /* Unknow message has been received */
            KPD_SEND_TRACE("KPD: Message received unknown",
                           RV_TRACE_LEVEL_ERROR);
            break;
         }
      }

      /* Free message */
      rvf_free_buf(msg_p);
   }

   return RV_OK;
}

/*@}*/
